"""Read GCC's original call arguments, including optimized-away parameters.

DW_OP_GNU_parameter_ref is a debugger recipe, not an ABI stack/register slot.
Its CU-relative formal reference can match a GNU call-site parameter that has
no physical location, but still records the original argument value. This
extractor preserves expressions it cannot interpret; it never guesses values.
"""
import argparse
import json
from pathlib import Path

from elftools.dwarf.dwarf_expr import DWARFExprParser
from canonicalize_helpers import sha
from helper_contracts import owner
from inspect_release import Image, inherited_value, value


def constant(ops):
    """Only literal/constant expressions; register and memory values stay open."""
    if len(ops) == 2 and ops[-1]['op'] == 'DW_OP_stack_value':
        ops = ops[:-1]
    if len(ops) != 1:
        return None
    op, args = ops[0]['op'], ops[0]['args']
    if op.startswith('DW_OP_lit') and op[9:].isdigit() and not args:
        return int(op[9:])
    if op in {'DW_OP_constu', 'DW_OP_consts', 'DW_OP_const1u', 'DW_OP_const1s',
              'DW_OP_const2u', 'DW_OP_const2s', 'DW_OP_const4u', 'DW_OP_const4s',
              'DW_OP_const8u', 'DW_OP_const8s'} and len(args) == 1:
        return args[0]
    return None


def extract(debug_path, candidates):
    raw = debug_path.read_bytes()
    assert sha(raw) == '7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw = Image(raw).dwarf()
    parser = DWARFExprParser(dw.structs)
    groups = {}
    for row in candidates:
        die = dw.get_DIE_from_refaddr(int(row['native_owner_die'], 16))
        groups.setdefault(die.cu.cu_offset, []).append((row, die, owner(die)))
    result = []
    for group in groups.values():
        targets = {abstract.offset: (row, concrete, abstract)
                   for row, concrete, abstract in group}
        cu = group[0][1].cu
        calls = {key: [] for key in targets}
        for site in cu.iter_DIEs():
            if site.tag != 'DW_TAG_GNU_call_site' or 'DW_AT_abstract_origin' not in site.attributes:
                continue
            target = owner(site.get_DIE_from_attribute('DW_AT_abstract_origin'))
            if target.offset not in targets:
                continue
            parent = site.get_parent()
            while parent is not None and parent.tag != 'DW_TAG_subprogram':
                parent = parent.get_parent()
            parameters = []
            for parameter in site.iter_children():
                if parameter.tag != 'DW_TAG_GNU_call_site_parameter':
                    continue
                formal = (parameter.get_DIE_from_attribute('DW_AT_abstract_origin')
                          if 'DW_AT_abstract_origin' in parameter.attributes else None)
                fields = {}
                for attr in ['DW_AT_location', 'DW_AT_GNU_call_site_value',
                             'DW_AT_GNU_call_site_data_value']:
                    if attr in parameter.attributes:
                        fields[attr] = [dict(op=op.op_name, args=op.args)
                                        for op in parser.parse_expr(parameter.attributes[attr].value)]
                parameters.append(dict(die=hex(parameter.offset),
                    original_parameter=inherited_value(formal, 'DW_AT_name') if formal else None,
                    original_parameter_die=hex(formal.offset) if formal else None,
                    constant=constant(fields.get('DW_AT_GNU_call_site_value', [])),
                    expressions=fields))
            calls[target.offset].append(dict(die=hex(site.offset),
                return_address=hex(value(site, 'DW_AT_low_pc')) if value(site, 'DW_AT_low_pc') else None,
                caller=inherited_value(parent, 'DW_AT_name') if parent else None,
                caller_die=hex(parent.offset) if parent else None,
                tail_call=value(site, 'DW_AT_GNU_tail_call', False), parameters=parameters))
        for abstract_offset, (row, concrete, abstract) in targets.items():
            result.append(dict(source=row['source'], name=row['name'], native_name=row['native_name'],
                native_owner_die=hex(concrete.offset), native_abstract_die=hex(abstract.offset),
                calls=calls[abstract_offset]))
    return dict(native_debug_path=str(debug_path), native_debug_sha256=sha(raw),
        extractor_sha256=sha(Path(__file__).read_bytes()), functions=result,
        scope='Original argument recipes from GNU call-site/formal references in each owning CU. '
              'Not a physical calling convention or recovery of unobserved generic helper behavior.')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--queue', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--debug', type=Path,
                        default=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg'))
    args = parser.parse_args()
    queue = json.loads(args.queue.read_bytes())
    report = extract(args.debug, queue['candidates'])
    args.output.write_text(json.dumps(report, indent=2) + '\n', encoding='utf8')
    print(json.dumps(dict(functions=len(report['functions']),
        call_sites=sum(len(row['calls']) for row in report['functions']),
        original_argument_records=sum(bool(p['original_parameter'])
            for row in report['functions'] for call in row['calls'] for p in call['parameters']))))
