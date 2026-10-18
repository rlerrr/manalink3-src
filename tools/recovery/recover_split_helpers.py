"""Recover the nine-argument wrapper for the already recovered split-card worker."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from inspect_release import Image, inherited_value, value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED, code_only
from recover_literals import recover_strings


ENTRIES = [('2488e80', 'generic_split_card', 'generic_split_card_2022', False, 9)]


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--prepare', action='store_true')
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    evidence, event_values = {}, {}
    for cu in image.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'cards/invasion.c':
            continue
        for die in cu.iter_DIEs():
            if die.tag == 'DW_TAG_enumerator' and value(die, 'DW_AT_name') in {'EVENT_PLAY_FIRST_HALF', 'EVENT_PLAY_SECOND_HALF'}:
                event_values[value(die, 'DW_AT_name')] = value(die, 'DW_AT_const_value')
            if die.tag != 'DW_TAG_subprogram':
                continue
            entry = next((entry for entry in ENTRIES if value(die, 'DW_AT_low_pc') == int(entry[0], 16)), None)
            if not entry:
                continue
            parameters = [type_name(type_of(p)) for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
            assert inherited_value(die, 'DW_AT_name') == entry[1] and type_name(type_of(die)) == 'int'
            assert parameters == ['int', 'int', 'event_t', *(['int'] * (entry[4] - 3))]
            evidence[entry[0]] = dict(line=str(inherited_value(die, 'DW_AT_decl_line')), parameters=parameters)
        break
    assert len(evidence) == 1 and event_values == {'EVENT_PLAY_FIRST_HALF': 4096, 'EVENT_PLAY_SECOND_HALF': 4352}
    exports = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'))
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    accepted = {f['address'] for f in manifest['accepted_functions']}
    worker = next(f for f in manifest['accepted_functions'] if f['address'] == '2485f50')
    assert worker['name'] == 'split_card_impl' and worker['source'] == 'cards/invasion.c' and worker['static']
    if args.prepare:
        functions = {row['low_pc'].removeprefix('0x'): row for row in csv.DictReader(
            (cache / '2022-functions.csv').open()) if row['low_pc']}
        requests = {}
        for row in exports:
            raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
            if re.search(r'\b(?:generic_split_card|generic_split_card_2022)\s*\(', raw):
                assert row['address'] not in accepted
                requests[row['address']] = row
        for address, original, name, private, count in ENTRIES:
            requests[address] = dict(address=address, source='cards/invasion.c', name=original)
        (cache / 'export-request-split-callers.tsv').write_text(''.join(
            f"{at}\t{row['source']}\t{row['name']}\t{int(functions[at]['high_pc'],16):x}\n" for at, row in requests.items()))
        print(json.dumps(dict(exports=len(requests), evidence=evidence), indent=2))
        return
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    metadata, addition = [], '\n\n/* Recovered October 2022 split-card interfaces; legacy API retained. */\n'
    for address, original, name, private, count in ENTRIES:
        raw = (cache / 'decompiled-split-callers' / (address + '.c')).read_text()
        resolved, literals = recover_strings(strip_comments(raw), runtime, {})
        body = clean(resolved)
        assert re.search(r'\b' + name + r'\s*\(', body)
        assert not UNRESOLVED.search(code_only(body))
        if private:
            body = 'static ' + body
        command = ['C:/Program Files/LLVM/bin/clang-format.exe',
                   '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
        body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
        addition += f"\n/* ManalinkEh 2022: 0x{address}; original cards/invasion.c:{evidence[address]['line']}. */\n" + body
        metadata.append(dict(name=name, original_name=original, address=address, source='cards/invasion.c',
                             line=evidence[address]['line'], static=private, prototype=body[:body.index('{')].strip() + ';',
                             raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=literals,
                             supporting_exports=[dict(address=worker['address'], raw_sha256=worker['raw_sha256'])],
                             reconstruction='Target cost-by-card-ID split interface recovered under a compatibility name; existing sixteen-argument API retained.',
                             interface_evidence=evidence[address], enum_value_checks=event_values))
    source = root / 'cards/invasion.c'
    original = source.read_bytes()
    assert all(('int ' + f['name'] + '(').encode() not in original for f in metadata)
    candidate = cache / 'split-helpers-candidate.c'
    checks = '\n' + ''.join(f'_Static_assert({n} == {v}, "split event");\n' for n, v in event_values.items())
    candidate.write_bytes(original + addition.encode() + checks.encode())
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
               '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
               '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)]
    result = subprocess.run(command, capture_output=True, text=True)
    (cache / 'split-helpers-check.log').write_text(result.stderr)
    result.check_returncode()
    if args.apply:
        source.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        header = root / 'manalink.h'; data = header.read_bytes(); at = data.rfind(b'#ifdef __cplusplus')
        declarations = '\n/* October 2022 split-card interface. */\n' + metadata[0]['prototype'] + '\n\n'
        header.write_bytes(data[:at] + declarations.replace('\n', '\r\n').encode() + data[at:])
        manifest['accepted_functions'].extend(metadata); manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
        rows = {row['address']: row for row in exports}
        for row in csv.DictReader((cache / 'decompiled-split-callers/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-split-callers' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            rows[row['address']] = row
        with (cache / 'decompiled-abi/export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=exports[0], delimiter='\t'); writer.writeheader(); writer.writerows(rows.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(path.read_text()); interfaces['split_card_2022_interfaces'] = dict(functions=metadata, enum_values=event_values,
            existing_worker=dict(address=worker['address'], name=worker['name'], raw_sha256=worker['raw_sha256']),
            scope='New wrapper uses the already recovered private worker. No worker duplication or global ID/registration upgrade; legacy split API unchanged.')
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, functions=[f['name'] for f in metadata]), indent=2))


if __name__ == '__main__':
    main()
