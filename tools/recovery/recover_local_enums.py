"""Restore function-local CommandChoices enums from their own DWARF scopes."""
import argparse
import hashlib
import json
import re
from pathlib import Path

from elftools.dwarf.dwarf_expr import DWARFExprParser
from inspect_release import Image, inherited_value, value
from export_abi import type_of, type_name
from audit_source import function_body


def descendants(die):
    for child in die.iter_children():
        yield child
        yield from descendants(child)


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply-bounds', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    debug_path = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg')
    data = debug_path.read_bytes()
    image = Image(data)
    functions = {}
    dwarf = image.dwarf()
    expressions = DWARFExprParser(dwarf.structs)
    for cu in dwarf.iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source != 'cards/strixhaven.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            address = value(die, 'DW_AT_low_pc')
            if die.tag != 'DW_TAG_subprogram' or not address:
                continue
            types = []
            for child in descendants(die):
                if child.tag != 'DW_TAG_typedef' or value(child, 'DW_AT_name') != 'CommandChoices':
                    continue
                enum = type_of(child)
                assert enum.tag == 'DW_TAG_enumeration_type' and value(enum, 'DW_AT_byte_size') == 4
                enumerators = {value(e, 'DW_AT_name'): value(e, 'DW_AT_const_value')
                               for e in enum.iter_children() if e.tag == 'DW_TAG_enumerator'}
                assert all(n.isidentifier() and isinstance(v, int) for n, v in enumerators.items())
                types.append(dict(name='CommandChoices', bytes=4, encoding=value(enum, 'DW_AT_encoding'),
                                  typedef_die=hex(child.offset), enum_die=hex(enum.offset), enumerators=enumerators))
            if types:
                assert len(types) == 1
                declarations = ['typedef enum { ' + ', '.join(n + ' = ' + str(v) for n, v in t['enumerators'].items()) +
                                ' } ' + t['name'] + ';' for t in types]
                row = dict(source=source, name=inherited_value(die, 'DW_AT_name'), types=types, declarations=declarations)
                if address != 0x23d8240:  # Quandrix retains other unresolved partial stack values.
                    adjacent, pointer, alias = {
                        0x23d35a0: ('td', 'pCVar4', 'ptStack_19c'),
                        0x23d7480: ('token', 'pCVar6', '&token'),
                        0x23dbb20: ('td', 'pCVar8', 'ptStack_1a0'),
                        0x23dfe70: ('td', 'pCVar6', 'ptStack_1f0')}[address]
                    layout = {}
                    for variable in descendants(die):
                        name = inherited_value(variable, 'DW_AT_name')
                        if variable.tag != 'DW_TAG_variable' or name not in {'choices', adjacent}:
                            continue
                        loc = value(variable, 'DW_AT_location')
                        assert isinstance(loc, list)
                        ops = expressions.parse_expr(loc)
                        assert len(ops) == 1 and ops[0].op_name == 'DW_OP_fbreg'
                        assert name not in layout
                        layout[name] = dict(type=type_name(type_of(variable)), frame_offset=ops[0].args[0],
                                            die=hex(variable.offset), expression=loc)
                    assert layout['choices']['type'] == 'CommandChoices[2]'
                    assert layout[adjacent]['frame_offset'] - layout['choices']['frame_offset'] == 2 * types[0]['bytes']
                    raw = (root / '.recovery/decompiled-abi' / f'{address:x}.c').read_text()
                    if adjacent == 'td':
                        assert set(re.findall(r'\b' + alias + r'\s*=(?!=)\s*([^;]+);', raw)) == {'&td'}
                    cast = '(token_generation_t *)' if adjacent == 'token' else '(target_definition_t *)'
                    boundaries = [dict(original=old, replacement=pointer + ' == choices + 2') for old in
                                  [alias + ' == ' + cast + pointer, cast + pointer + ' == ' + alias]]
                    assert all(raw.count(b['original']) == 1 for b in boundaries)
                    row.update(array_boundaries=boundaries, array_boundary_evidence=layout)
                functions[f'{address:x}'] = row
        break
    assert len(functions) == 5
    result = dict(debug_sha256=hashlib.sha256(data).hexdigest(),
                  scope='Each typedef is restored inside its own function; no global CHOICE_* names.', functions=functions)
    (root / 'docs/recovery-evidence/local-enums.json').write_text(json.dumps(result, indent=2))
    if args.apply_bounds:
        path = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(path.read_text())
        source = root / 'cards/strixhaven.c'
        content = source.read_bytes().decode('latin1')
        for function in manifest['accepted_functions']:
            row = functions.get(function['address'])
            if not row or not row.get('array_boundaries'):
                continue
            body = function_body(content, function['name'])
            corrected = body
            for boundary in row['array_boundaries']:
                assert corrected.count(boundary['original']) == 1
                corrected = corrected.replace(boundary['original'], boundary['replacement'])
            assert content.count(body) == 1
            content = content.replace(body, corrected)
            function['array_boundaries'] = row['array_boundaries']
            function['array_boundary_evidence'] = row['array_boundary_evidence']
        source.write_bytes(content.encode('latin1'))
        path.write_text(json.dumps(manifest, indent=2))
    print(json.dumps({address: row['name'] for address, row in functions.items()}, indent=2))


if __name__ == '__main__':
    main()
