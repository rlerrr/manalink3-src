"""Recover the concrete historic trigger clone and a missing private Thopter helper."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from elftools.dwarf.locationlists import LocationParser, BaseAddressEntry
from inspect_release import Image, inherited_value, value
from export_abi import type_of, type_name
from promote_recovery import clean


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--prepare', action='store_true')
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    dwarf = image.dwarf(); locations = LocationParser(dwarf.location_lists())
    evidence = None
    for cu in dwarf.iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'cards/dominaria.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if value(die, 'DW_AT_low_pc') != 0x22ca3f0:
                continue
            parameters = []
            for parameter in die.iter_children():
                if parameter.tag != 'DW_TAG_formal_parameter':
                    continue
                location = locations.parse_from_attribute(parameter.attributes['DW_AT_location'], cu['version'], parameter)
                base = value(cu.get_top_DIE(), 'DW_AT_low_pc', 0); expression = None
                for entry in location:
                    if isinstance(entry, BaseAddressEntry):
                        base = entry.base_address
                    else:
                        begin = entry.begin_offset if entry.is_absolute else base + entry.begin_offset
                        end = entry.end_offset if entry.is_absolute else base + entry.end_offset
                        if begin <= 0x22ca3f0 < end:
                            expression = entry.loc_expr; break
                parameters.append(dict(name=inherited_value(parameter, 'DW_AT_name'), type=type_name(type_of(parameter)), entry_location=expression))
            assert [p['entry_location'] for p in parameters[:3]] == [[0x50], [0x52], [0x51]]
            assert [p['name'] for p in parameters] == ['player', 'card', 'event', 'trigger_mode']
            assert parameters[3]['entry_location'][0] == 0xfa and parameters[3]['entry_location'][-1] == 0x9f
            evidence = dict(address='22ca3f0', line=str(inherited_value(die, 'DW_AT_decl_line')), parameters=parameters,
                            scope='Concrete three-register clone; trigger mode is computed by duh_mode in this body, not an ABI argument.')
        break
    assert evidence
    assert any(n == '_cast_historic_spell.isra.0' and storage == 3 and
               image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s - 1].VirtualAddress + offset == 0x22ca3f0
               for n, offset, s, kind, storage in image.functions())
    exports = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'))
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text()); accepted = {f['address'] for f in manifest['accepted_functions']}
    functions = {row['low_pc'].removeprefix('0x'): row for row in csv.DictReader((cache / '2022-functions.csv').open()) if row['low_pc']}
    if args.prepare:
        requests = {}
        for row in exports:
            raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
            if re.search(r'\bcast_historic_spell\s*\(', raw):
                assert row['address'] not in accepted
                requests[row['address']] = row
        requests['20ba940'] = dict(address='20ba940', source='cards/aether_revolt.c', name='make_thopter')
        (cache / 'export-request-historic-thopter.tsv').write_text(''.join(
            f"{at}\t{row['source']}\t{row['name']}\t{int(functions[at]['high_pc'],16):x}\n" for at, row in requests.items()))
        print(json.dumps(dict(exports=len(requests), evidence=evidence), indent=2)); return
    raw = (cache / 'decompiled-historic-thopter/22ca3f0.c').read_text()
    body = 'static ' + clean(raw, ['trigger_condition'])
    assert re.search(r'void\s+cast_historic_spell_2022\(int player,int card,event_t event\)', body)
    assert 'trigger_mode' not in body and 'duh_mode(player)' in body
    command = ['C:/Program Files/LLVM/bin/clang-format.exe', '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
    body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
    addition = f"\n\n/* ManalinkEh 2022: 0x22ca3f0; original cards/dominaria.c:{evidence['line']}. */\n" + body
    source = root / 'cards/dominaria.c'; original = source.read_bytes()
    assert b'void cast_historic_spell_2022(' not in original
    candidate = cache / 'historic-helper-candidate.c'; candidate.write_bytes(original + addition.encode())
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only',
               '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
               '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)]
    result = subprocess.run(command, capture_output=True, text=True); (cache / 'historic-helper-check.log').write_text(result.stderr); result.check_returncode()
    entry = dict(name='cast_historic_spell_2022', original_name='cast_historic_spell', address='22ca3f0', source='cards/dominaria.c',
                 line=evidence['line'], static=True, prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
                 recovered_literals=[], reconstruction='Concrete clone removes trigger_mode with no ABI storage; preserve body-computed duh mode.', interface_evidence=evidence)
    if args.apply:
        source.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        manifest['accepted_functions'].append(entry); manifest['accepted'] = len(manifest['accepted_functions']); manifest_path.write_text(json.dumps(manifest, indent=2))
        rows = {row['address']: row for row in exports}
        for row in csv.DictReader((cache / 'decompiled-historic-thopter/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-historic-thopter' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c')); rows[row['address']] = row
        with (cache / 'decompiled-abi/export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=exports[0], delimiter='\t'); writer.writeheader(); writer.writerows(rows.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
        interfaces['historic_spell_clone'] = evidence
        interfaces.setdefault('supplemental_exports', []).append(dict(address='20ba940', end='20ba98c', source='cards/aether_revolt.c', name='make_thopter', reason='Private same-name helper is separate from the existing Kaladesh body.'))
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helper=entry['name']), indent=2))


if __name__ == '__main__':
    main()
