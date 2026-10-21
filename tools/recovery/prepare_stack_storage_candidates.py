"""Prepare verified stack views for deferred candidates; normal promotion still checks them."""
import collections
import csv
import hashlib
import json
from pathlib import Path

from elftools.dwarf.locationlists import LocationParser, LocationExpr, BaseAddressEntry
from elftools.dwarf.dwarf_expr import DWARFExprParser
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, aggregate_stack_aliases
from recover_stack_storage import descendants, rewrite, MEMBERS


def main():
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    manifest = json.loads((root / 'docs/recovery-evidence/recovered-functions.json').read_text())
    accepted = {f['address'] for f in manifest['accepted_functions']}
    report = json.loads((cache / 'promotion-report.json').read_text())
    wanted = {n for n, why in report['deferred'].items() if why.startswith('overlapping stack objects')}
    existing_storage = json.loads((root / 'docs/recovery-evidence/stack-storage.json').read_text())
    wanted.update(c['name'] for c in existing_storage.get('candidates', {}).values())
    requests = {}
    for row in csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'):
        if row['name'] not in wanted or row['address'] in accepted:
            continue
        raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
        body = clean(raw)
        aliases = aggregate_stack_aliases(body)
        if aliases:
            requests[row['address']] = dict(row=row, body=body, aliases=aliases,
                raw_sha256=hashlib.sha256(raw.encode()).hexdigest())
    path = root / 'docs/recovery-evidence/stack-storage.json'; storage = json.loads(path.read_text())
    sizes = {n: t['size'] for n, t in storage['target_layouts'].items()}; sizes.update({'int': 4, 'char[100]': 100})
    data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    assert hashlib.sha256(data).hexdigest() == storage['debug_sha256']
    debug = Image(data); dwarf = debug.dwarf(); parser = LocationParser(dwarf.location_lists()); expressions = DWARFExprParser(dwarf.structs)
    evidence = {}; rejected = {}; field_layouts = {}
    for cu in dwarf.iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {r['row']['source'] for r in requests.values()}:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_typedef' and value(die, 'DW_AT_name') in storage['target_layouts']:
                typename = value(die, 'DW_AT_name'); record = type_of(die)
                sizes_by_field = {}; kinds = {}
                for member in record.iter_children():
                    name = value(member, 'DW_AT_name'); typ = type_of(member)
                    while typ.tag in {'DW_TAG_typedef', 'DW_TAG_const_type', 'DW_TAG_volatile_type'}:
                        typ = type_of(typ)
                    size = value(typ, 'DW_AT_byte_size')
                    if typ.tag == 'DW_TAG_array_type':
                        base = type_of(typ)
                        while base.tag == 'DW_TAG_typedef':
                            base = type_of(base)
                        subrange = next(typ.iter_children())
                        size = value(base, 'DW_AT_byte_size') * (value(subrange, 'DW_AT_upper_bound') + 1)
                    assert isinstance(size, int) and size > 0
                    sizes_by_field[name] = size
                    kinds[name] = 'pointer' if typ.tag == 'DW_TAG_pointer_type' else ('integer' if typ.tag in {'DW_TAG_base_type', 'DW_TAG_enumeration_type'} else 'aggregate')
                layout = dict(member_sizes=sizes_by_field, member_kinds=kinds)
                assert typename not in field_layouts or field_layouts[typename] == layout
                field_layouts[typename] = layout
            at = value(die, 'DW_AT_low_pc'); address = f'{at:x}' if at else ''
            if address not in requests:
                continue
            request = requests[address]
            assert source == request['row']['source'] and inherited_value(die, 'DW_AT_name') == request['row']['name']
            groups = collections.defaultdict(list)
            for variable in descendants(die):
                if variable.tag != 'DW_TAG_variable' or 'DW_AT_location' not in variable.attributes:
                    continue
                typ = type_name(type_of(variable))
                if typ not in MEMBERS:
                    continue
                loc = parser.parse_from_attribute(variable.attributes['DW_AT_location'], cu['version'], variable)
                exprs = [loc.loc_expr] if isinstance(loc, LocationExpr) else [e.loc_expr for e in loc if not isinstance(e, BaseAddressEntry)]
                offsets = set()
                for expression in exprs:
                    ops = expressions.parse_expr(expression)
                    if len(ops) == 1 and ops[0].op_name == 'DW_OP_fbreg':
                        offsets.add(ops[0].args[0])
                for offset in offsets:
                    groups[offset].append(dict(name=inherited_value(variable, 'DW_AT_name'), type=typ, die=hex(variable.offset)))
            try:
                corrected, views = rewrite(request['body'], request['aliases'], groups, sizes)
                assert not aggregate_stack_aliases(corrected)
            except AssertionError as error:
                rejected[address] = dict(name=request['row']['name'], error=str(error))
                continue
            evidence[address] = dict(name=request['row']['name'], source=source, raw_sha256=request['raw_sha256'],
                groups=dict(groups), target_sizes=sizes, expected_aliases=request['aliases'], views=views,
                frame_base=value(die, 'DW_AT_frame_base'),
                scope='Verified shared DWARF frame offsets and local types. Use explicit union views before normal machine-state, dependency, interface and compiler screening.')
    for source_type,native_type in storage.get('source_type_aliases',{}).items():
        assert source_type in storage['target_layouts'] and native_type in field_layouts
        assert storage['target_layouts'][source_type]['size']==storage['target_layouts'][native_type]['size']
        assert storage['target_layouts'][source_type]['members']==storage['target_layouts'][native_type]['members']
        field_layouts[source_type]=dict(field_layouts[native_type])
    assert set(field_layouts) == set(storage['target_layouts'])
    for typename, fields in field_layouts.items():
        storage['target_layouts'][typename].update(fields)
    storage['candidates']={a:e for a,e in storage.get('candidates', {}).items() if a in accepted}
    storage['candidates'].update(evidence)
    storage['last_candidate_preparation'] = dict(prepared=len(evidence), rejected=rejected)
    path.write_text(json.dumps(storage, indent=2))
    print(json.dumps(dict(prepared=len(evidence), rejected=rejected), indent=2))


if __name__ == '__main__':
    main()
