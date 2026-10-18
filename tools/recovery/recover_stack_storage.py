"""Repair verified overlapping DWARF locals using explicit, adequately sized union views.

Ghidra sometimes declares only the smallest of several locals sharing a frame
offset. Its casts then let an initializer overwrite that C object's allocation.
Preserve the decoded byte views with real union members and the DWARF extent.
This does not upgrade the older engine's target/test/token aggregate APIs.
"""
import argparse
import collections
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

from elftools.dwarf.locationlists import LocationParser, LocationExpr, BaseAddressEntry
from elftools.dwarf.dwarf_expr import DWARFExprParser
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, aggregate_stack_aliases, STACK_AGGREGATES, LITERAL
from audit_source import function_body
from format_recovery import definition_span


COMMENT = r'/\*.*?\*/|//[^\r\n]*'
MEMBERS = dict(test_definition_t='test', target_definition_t='target', token_generation_t='token', int='value', **{'char[100]': 'buffer'})


def descendants(die):
    for child in die.iter_children():
        yield child
        yield from descendants(child)


def rewrite(body, aliases, groups, sizes):
    proof = []
    by_root = collections.defaultdict(set)
    for cast, name, original in aliases:
        by_root[name].update([cast, original])
    # Gustha's second shared object is a 100-byte prompt buffer, not an int.
    for name in list(by_root):
        if name == 'idx' and '(char *)&leg' in body:
            by_root['leg'] = {'int', 'char[100]'}
    for number, (name, types) in enumerate(by_root.items()):
        declaration = re.search(r'(?m)^(\s+)([A-Za-z_]\w*)\s+' + re.escape(name) + r'\s*;', body)
        assert declaration, name
        original_type = declaration[2]
        assert original_type in types and types <= set(MEMBERS)
        matches = [(offset, variables) for offset, variables in groups.items()
                   if types <= {v['type'] for v in variables}]
        assert len(matches) == 1, (name, types, matches)
        offset, variables = matches[0]
        # A generated name can acquire a numerical suffix after stack merging.
        assert any(v['type'] == original_type and
                   (v['name'] == name or re.sub(r'_\d+$', '', name) == v['name']) for v in variables), (name, variables)
        extent = max(sizes[t] for t in types)
        storage = 'recovered_stack_' + name
        assert storage not in body
        members = {t: MEMBERS[t] for t in sorted(types)}
        fields = []
        for typ, member in members.items():
            fields.append(('char ' + member + '[100];') if typ == 'char[100]' else typ + ' ' + member + ';')
        fields.append('uint8_t bytes[' + str(extent) + '];')
        new_decl = '\n  /* DWARF locals share this frame offset; keep every typed view. */\n  union\n  {\n' + ''.join('    ' + f + '\n' for f in fields) + '  } ' + storage + ';'
        placeholder = 'RECOVERY_STORAGE_DECL_' + str(number)
        body = body[:declaration.start()] + '\n  ' + placeholder + '\n' + body[declaration.end():]
        # Normalize the casts before replacing ordinary accesses to the old view.
        for typ, member in members.items():
            cast_type = 'char' if typ == 'char[100]' else typ
            pattern = r'\(' + re.escape(cast_type) + r'\s*\*\)\s*&\s*' + re.escape(name) + r'\b'
            replacement = storage + '.' + member if typ == 'char[100]' else '&' + storage + '.' + member
            body = re.sub(pattern, replacement, body)
        def identifier(match):
            text = match[0]
            if text.startswith(('"', "'", '/*', '//')):
                return text
            return storage + '.' + members[original_type]
        body = re.sub(LITERAL + '|' + COMMENT + r'|\b' + re.escape(name) + r'\b', identifier, body, flags=re.S)
        body = body.replace(placeholder, new_decl)
        proof.append(dict(original_name=name, original_type=original_type, frame_offset=int(offset),
            variables=variables, target_extent=extent, storage=storage, members=members))
    assert not aggregate_stack_aliases(body)
    return body, proof


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(path.read_text())
    source_text = {}; requests = {}
    for function in manifest['accepted_functions']:
        source = function['source']
        source_text.setdefault(source, (root / source).read_bytes().decode('latin1').replace('\r\n', '\n'))
        text = source_text[source]; marker = text.index('/* ManalinkEh 2022: 0x' + function['address'] + ';')
        _, start, end = definition_span(text[marker:], function['name'])
        body = text[marker + start:marker + end]
        aliases = aggregate_stack_aliases(body)
        if aliases:
            requests[function['address']] = dict(function=function, body=body, aliases=aliases)
    # The scalar-union hint fixes this otherwise new handler, which also has
    # an overlapping target/test object. Reconstruct that storage before use.
    extra_address = '23ddcf0'
    assert extra_address not in {f['address'] for f in manifest['accepted_functions']}
    raw_extra = (cache / 'decompiled-scalar-info-slots' / (extra_address + '.c')).read_text()
    extra_body = clean(raw_extra)
    row = next(r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc'] == '0x' + extra_address)
    extra = dict(name=row['name'], address=extra_address, source=row['source'], line=row['line'], static=False,
                 raw_sha256=hashlib.sha256(raw_extra.encode()).hexdigest(), recovered_literals=[])
    requests[extra_address] = dict(function=extra, body=extra_body, aliases=aggregate_stack_aliases(extra_body))
    data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes(); debug = Image(data); dwarf = debug.dwarf()
    locations = LocationParser(dwarf.location_lists()); expressions = DWARFExprParser(dwarf.structs)
    evidence = {}; layouts = {}
    for cu in dwarf.iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {r['function']['source'] for r in requests.values()}:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_typedef' and value(die, 'DW_AT_name') in STACK_AGGREGATES:
                name = value(die, 'DW_AT_name'); record = type_of(die)
                layout = dict(size=value(record, 'DW_AT_byte_size'),
                    members={value(m, 'DW_AT_name'): value(m, 'DW_AT_data_member_location') for m in record.iter_children()})
                assert name not in layouts or layouts[name] == layout
                layouts[name] = layout
            at = value(die, 'DW_AT_low_pc')
            if at is None or f'{at:x}' not in requests:
                continue
            request = requests[f'{at:x}']; function = request['function']
            assert source == function['source'] and inherited_value(die, 'DW_AT_name') == function['name']
            groups = collections.defaultdict(list)
            for variable in descendants(die):
                if variable.tag != 'DW_TAG_variable' or 'DW_AT_location' not in variable.attributes:
                    continue
                typ = type_name(type_of(variable))
                if typ not in MEMBERS:
                    continue
                loc = locations.parse_from_attribute(variable.attributes['DW_AT_location'], cu['version'], variable)
                exprs = [loc.loc_expr] if isinstance(loc, LocationExpr) else [e.loc_expr for e in loc if not isinstance(e, BaseAddressEntry)]
                offsets = set()
                for expression in exprs:
                    ops = expressions.parse_expr(expression)
                    if len(ops) == 1 and ops[0].op_name == 'DW_OP_fbreg':
                        offsets.add(ops[0].args[0])
                for offset in offsets:
                    groups[offset].append(dict(name=inherited_value(variable, 'DW_AT_name'), type=typ, die=hex(variable.offset)))
            evidence[f'{at:x}'] = dict(groups=dict(groups), frame_base=value(die, 'DW_AT_frame_base'))
    assert set(evidence) == set(requests) and len(layouts) == 3
    sizes = {name: record['size'] for name, record in layouts.items()}; sizes.update({'int': 4, 'char[100]': 100})
    # Shared representation offsets, plus the known older source layouts.
    checks = ['_Static_assert(sizeof(test_definition_t) == 208, "source test size");',
              '_Static_assert(sizeof(target_definition_t) == 80, "source target size");',
              '_Static_assert(sizeof(token_generation_t) == 72, "source token size");']
    for typ, record in layouts.items():
        for member, offset in record['members'].items():
            if (typ == 'test_definition_t' and offset >= 140) or member in {'flags', 'special_flags3'}:
                continue
            checks.append(f'_Static_assert(__builtin_offsetof({typ}, {member}) == {offset}, "shared view offset");')
    changed = {}; recovered = {}
    for address, request in requests.items():
        body, storage = rewrite(request['body'], request['aliases'], evidence[address]['groups'], sizes)
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
            '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
            input=body.encode('latin1'), capture_output=True, check=True).stdout.decode('latin1').rstrip()
        f = request['function']; source = f['source']; changed.setdefault(source, source_text.get(source, (root / source).read_bytes().decode('latin1').replace('\r\n', '\n')))
        if address == extra_address:
            changed[source] += f'\n\n/* ManalinkEh 2022: 0x{address}; original {source}:{f["line"]}. */\n' + body + '\n'
        else:
            assert changed[source].count(request['body']) == 1
            changed[source] = changed[source].replace(request['body'], body, 1)
        proof = dict(storage=storage, frame_base=evidence[address]['frame_base'],
            previous_body_sha256=hashlib.sha256(function_body(request['body'], f['name']).encode()).hexdigest(),
            body_sha256=hashlib.sha256(function_body(body, f['name']).encode()).hexdigest(),
            scope='Explicit union members preserve verified overlapping stack views and prevent larger initializers overwriting small C locals. Byte storage has the exact DWARF extent. Older aggregate APIs remain in use; this is not an engine-ABI migration.')
        f['stack_storage_reconstruction'] = proof
        f['reconstruction'] = (f.get('reconstruction', '') + ' ' + proof['scope']).strip()
        recovered[address] = dict(name=f['name'], source=source, **proof)
    for source, content in changed.items():
        candidate = cache / ('stack-storage-' + Path(source).name)
        candidate.write_bytes(content.encode('latin1') + ('\n' + '\n'.join(checks)).encode())
        result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc',
            '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
            '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format',
            '-I', str(root), str(candidate)], capture_output=True, text=True)
        (cache / ('stack-storage-' + Path(source).stem + '.log')).write_text(result.stderr)
        result.check_returncode()
    extra['prototype'] = extra_body[:extra_body.index('{')].strip() + ';'
    if args.apply:
        for source, content in changed.items():
            (root / source).write_bytes(content.replace('\n', '\r\n').encode('latin1'))
        manifest['accepted_functions'].append(extra); manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_text(json.dumps(manifest, indent=2))
        (root / 'docs/recovery-evidence/stack-storage.json').write_text(json.dumps(dict(debug_sha256=hashlib.sha256(data).hexdigest(),
            target_layouts=layouts, source_layouts=dict(test_definition_t=208, target_definition_t=80, token_generation_t=72),
            functions=recovered), indent=2))
    print(json.dumps(dict(applied=args.apply, corrected_functions=len(requests) - 1, new_handlers=1), indent=2))


if __name__ == '__main__':
    main()
