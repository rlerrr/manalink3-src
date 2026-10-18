"""Recover source-aware counter removal and its DWARF-proven shared stack objects."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from elftools.dwarf.locationlists import LocationParser, LocationExpr, BaseAddressEntry
from inspect_release import Image, inherited_value, value
from export_abi import type_of, type_name
from promote_recovery import clean


ENTRIES = [
    ('26ceef0', 'remove_all_counters', 'clear_all_counters_2022', True),
    ('26d48d0', 'remove_counters', 'remove_counters_from_source_2022_impl', True),
    ('26d4c10', 'remove_counters', 'remove_counters_from_source', False),
    ('26d4c50', 'remove_all_counters', 'remove_all_counters_from_source', False)]


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--prepare', action='store_true')
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    dwarf = image.dwarf()
    locations = LocationParser(dwarf.location_lists())
    evidence, enum_values, layouts = {}, {}, {}
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/counters.c':
            continue
        def walk(die):
            for child in die.iter_children():
                yield child
                yield from walk(child)
        for die in cu.get_top_DIE().iter_children():
            name = value(die, 'DW_AT_name')
            if name in {'token_generation_t', 'target_definition_t'} and die.tag == 'DW_TAG_typedef':
                struct = type_of(die)
                layouts[name] = dict(bytes=value(struct, 'DW_AT_byte_size'),
                                     fields={value(c, 'DW_AT_name'): value(c, 'DW_AT_data_member_location')
                                             for c in struct.iter_children() if c.tag == 'DW_TAG_member'})
            if value(die, 'DW_AT_low_pc') not in [int(row[0], 16) for row in ENTRIES]:
                continue
            address = f"{value(die, 'DW_AT_low_pc'):x}"
            item = dict(line=str(inherited_value(die, 'DW_AT_decl_line')))
            if address == '26ceef0':
                parameters = []
                for parameter in die.iter_children():
                    if parameter.tag != 'DW_TAG_formal_parameter':
                        continue
                    loc = locations.parse_from_attribute(parameter.attributes['DW_AT_location'], cu['version'], parameter)
                    expression = None
                    base = value(cu.get_top_DIE(), 'DW_AT_low_pc', 0)
                    for entry in loc:
                        if isinstance(entry, BaseAddressEntry):
                            base = entry.base_address
                        else:
                            begin = entry.begin_offset if entry.is_absolute else base + entry.begin_offset
                            end = entry.end_offset if entry.is_absolute else base + entry.end_offset
                            if begin <= int(address, 16) < end:
                                expression = entry.loc_expr
                                break
                    parameters.append(dict(name=inherited_value(parameter, 'DW_AT_name'), entry_location=expression))
                assert parameters[:2] == [dict(name='t_player', entry_location=[0x50]), dict(name='t_card', entry_location=[0x52])]
                assert all(p['entry_location'][0] == 0xfa and p['entry_location'][-1] == 0x9f for p in parameters[2:])
                item['concrete_parameters'] = parameters
            if address == '26d48d0':
                variables = [dict(name=inherited_value(c, 'DW_AT_name'), type=type_name(type_of(c)),
                                  location=value(c, 'DW_AT_location'), die=hex(c.offset))
                             for c in walk(die) if c.tag == 'DW_TAG_variable' and inherited_value(c, 'DW_AT_name') in {'token', 'td'}]
                assert len(variables) == 4
                assert {v['type'] for v in variables} == {'token_generation_t', 'target_definition_t'}
                assert all(v['location'] == variables[0]['location'] for v in variables)
                item['shared_stack_objects'] = variables
            evidence[address] = item
        for die in cu.iter_DIEs():
            if die.tag == 'DW_TAG_enumerator' and value(die, 'DW_AT_name') in {'COUNTER_TIME', 'COUNTER_P1_P1'}:
                enum_values[value(die, 'DW_AT_name')] = value(die, 'DW_AT_const_value')
        break
    assert len(evidence) == 4 and len(enum_values) == 2
    assert layouts['token_generation_t']['fields']['qty'] == layouts['target_definition_t']['fields']['extra'] + 3 == 43
    symbols = {image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s - 1].VirtualAddress + offset: n
               for n, offset, s, kind, storage in image.functions() if n.startswith(('_remove_counters', '_remove_all_counters'))}
    assert symbols[int(ENTRIES[0][0], 16)] == '_remove_all_counters.part.0'
    assert symbols[int(ENTRIES[1][0], 16)] == '_remove_counters.part.0'
    exports = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'))
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    accepted = {f['address'] for f in manifest['accepted_functions']}
    if args.prepare:
        functions = {row['low_pc'].removeprefix('0x'): row for row in csv.DictReader(
            (cache / '2022-functions.csv').open()) if row['low_pc']}
        requests = {}
        for row in exports:
            raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
            if re.search(r'\b(?:remove_counters|remove_all_counters)\s*\(', raw):
                assert row['address'] not in accepted
                requests[row['address']] = row
        for address, original, name, private in ENTRIES:
            requests[address] = dict(address=address, source='functions/counters.c', name=original)
        (cache / 'export-request-counter-removal-callers.tsv').write_text(''.join(
            f"{address}\t{row['source']}\t{row['name']}\t{int(functions[address]['high_pc'],16):x}\n" for address, row in requests.items()))
        (cache / 'counter-removal-evidence.json').write_text(json.dumps(dict(functions=evidence, layouts=layouts, enum_values=enum_values), indent=2))
        print(json.dumps(dict(exports=len(requests), functions=list(evidence))))
        return
    metadata, addition = [], '\n\n/* Recovered October 2022 source-aware counter removal. */\n'
    for address, original, name, private in ENTRIES:
        raw = (cache / 'decompiled-counter-removal-callers' / (address + '.c')).read_text()
        body = clean(raw)
        assert re.search(r'\b' + name + r'\s*\(', body)
        if address == '26d48d0':
            assert 'target_definition_t td;' in body
            body = body.replace('target_definition_t td;', 'target_definition_t td;\n  token_generation_t token;')
            body = body.replace('add_or_remove_counters_impl(', 'add_or_remove_counters_from_source_impl(')
            old = '''default_token_definition(t_player,t_card,0x4986,(token_generation_t *)&td);
      td.extra._3_1_ = (uint8_t)iStack_70;
      generate_token((token_generation_t *)&td);'''
            new = '''default_token_definition(t_player,t_card,0x4986,&token);
      token.qty = (uint8_t)iStack_70;
      generate_token(&token);'''
            assert old in body
            body = body.replace(old, new)
        if private:
            body = 'static ' + body
        assert not re.search(r'\b(?:remove_counters|remove_all_counters)\s*\(', body)
        assert '._' not in body
        command = ['C:/Program Files/LLVM/bin/clang-format.exe',
                   '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
        body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
        addition += f"\n/* ManalinkEh 2022: 0x{address}; original functions/counters.c:{evidence[address]['line']}. */\n" + body
        metadata.append(dict(name=name, original_name=original, address=address, source='functions/counters.c',
                             line=evidence[address]['line'], static=private, prototype=body[:body.index('{')].strip() + ';',
                             raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[],
                             reconstruction='Source-aware compatibility API; concrete clone interfaces and shared stack objects verified from DWARF.',
                             interface_evidence=evidence[address], enum_value_checks=enum_values))
    path = root / 'functions/counters.c'
    original = path.read_bytes()
    assert all(('void ' + f['name'] + '(').encode() not in original for f in metadata)
    candidate = cache / 'counter-removal-candidate.c'
    checks = '\n#include <stddef.h>\n_Static_assert(offsetof(token_generation_t, qty) == 43, "token qty");\n'
    checks += ''.join(f'_Static_assert({name} == {number}, "counter value");\n' for name, number in enum_values.items())
    candidate.write_bytes(original + addition.encode() + checks.encode())
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
               '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
               '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)]
    result = subprocess.run(command, capture_output=True, text=True)
    (cache / 'counter-removal-check.log').write_text(result.stderr)
    result.check_returncode()
    if args.apply:
        path.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        header = root / 'manalink.h'
        data = header.read_bytes()
        at = data.rfind(b'#ifdef __cplusplus')
        declarations = '\n/* October 2022 source-aware counter removal interfaces. */\n' + '\n'.join(f['prototype'] for f in metadata if not f['static']) + '\n\n'
        header.write_bytes(data[:at] + declarations.replace('\n', '\r\n').encode() + data[at:])
        manifest['accepted_functions'].extend(metadata)
        manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
        rows = {row['address']: row for row in exports}
        for row in csv.DictReader((cache / 'decompiled-counter-removal-callers/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-counter-removal-callers' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            rows[row['address']] = row
        with (cache / 'decompiled-abi/export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=exports[0], delimiter='\t')
            writer.writeheader(); writer.writerows(rows.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(path.read_text())
        interfaces['source_aware_counter_removal'] = dict(functions=metadata, layouts=layouts, enum_values=enum_values,
            scope='Old four/three-argument removal APIs and bodies retained; source-aware APIs use recovered counter core. Counter dispatch integration remains pending.')
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, functions=[f['name'] for f in metadata]), indent=2))


if __name__ == '__main__':
    main()
