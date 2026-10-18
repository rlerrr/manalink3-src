"""Recover the shared token predicate and export the set-private basic-land callback."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean


ENTRIES = [('22d6fa0', 'target_is_token_too', 'cards/amonkhet.c', 5, 2),
           ('2359730', 'is_basic_land_by_internal_id', 'cards/theros_beyond_death.c', 4, 3)]


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--prepare', action='store_true')
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()); evidence = {}
    for cu in debug.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {r[2] for r in ENTRIES}:
            continue
        for die in cu.get_top_DIE().iter_children():
            at = value(die, 'DW_AT_low_pc')
            entry = next((r for r in ENTRIES if int(r[0], 16) == at), None)
            if not entry:
                continue
            assert source == entry[2] and inherited_value(die, 'DW_AT_name') == entry[1]
            parameters = [(inherited_value(p, 'DW_AT_name'), type_name(type_of(p)))
                          for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
            assert len(parameters) == entry[3] and all(t == 'int' for n, t in parameters)
            evidence[entry[0]] = dict(line=str(inherited_value(die, 'DW_AT_decl_line')), parameters=parameters,
                return_type=type_name(type_of(die)), bytes=value(die, 'DW_AT_high_pc'))
    assert len(evidence) == 2
    for address, name, source, count, linkage in ENTRIES:
        assert any(n == '_' + name and storage == linkage and debug.pe.OPTIONAL_HEADER.ImageBase +
            debug.pe.sections[s - 1].VirtualAddress + off == int(address, 16)
            for n, off, s, kind, storage in debug.functions())
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(manifest_path.read_text())
    accepted = {f['address'] for f in manifest['accepted_functions']}; assert not accepted & set(evidence)
    functions = {r['low_pc'].removeprefix('0x'): r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc']}
    if args.prepare:
        requests = {a: dict(address=a, source=s, name=n) for a,n,s,c,l in ENTRIES}
        for row in csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'):
            if row['address'] not in accepted and re.search(r'\btarget_is_token_too\b',
                    (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()):
                requests[row['address']] = row
        (cache / 'export-request-token-predicate.tsv').write_text('\n'.join('\t'.join([a, r['source'], r['name'],
            functions[a]['high_pc'].removeprefix('0x')]) for a, r in requests.items()) + '\n')
        print(json.dumps(dict(prepared=len(requests)), indent=2)); return
    count = len((cache / 'export-request-token-predicate.tsv').read_text().splitlines())
    log = (cache / 'token-predicate-console.log').read_text()
    assert 'RECOVERY_TOKEN_PREDICATE_COMPLETE count=1' in log and f'RECOVERY_EXPORT_COMPLETE success={count} failed=0' in log and 'ERROR' not in log
    raw = (cache / 'decompiled-token-predicate/22d6fa0.c').read_text()
    body = clean(raw)
    body = re.sub(r'^char\s*\*', 'const char *', body)
    body = re.sub(r'(?m)^(\s+)char\s*\*\s*(\w+)\s*;', r'\1const char *\2;', body)
    body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
        '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
        input=body.encode(), capture_output=True, check=True).stdout.decode()
    source = root / 'cards/amonkhet.c'; original = source.read_bytes()
    addition = '\n\n/* ManalinkEh 2022: 0x22d6fa0; original cards/amonkhet.c:4475. */\n' + body
    candidate = cache / 'token-predicate-candidate.c'; candidate.write_bytes(original + addition.encode())
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
        '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
        '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)], capture_output=True, text=True)
    (cache / 'token-predicate-check.log').write_text(result.stderr); result.check_returncode()
    scope = 'Shared five-argument token predicate under a distinct name, avoiding existing set-private predicates. Borrowed diagnostic return and local are const-qualified; the pointer ABI and literal bytes are unchanged.'
    entry = dict(name='target_is_token_too_2022', original_name='target_is_token_too', address='22d6fa0', source='cards/amonkhet.c',
        line=evidence['22d6fa0']['line'], static=False, prototype=body[:body.index('{')].strip() + ';',
        raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[], reconstruction=scope,
        interface_evidence=evidence['22d6fa0'])
    if args.apply:
        source.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        p = root / 'manalink.h'; header = p.read_bytes(); at = header.rfind(b'#ifdef __cplusplus')
        block = ('/* October 2022 shared token-target predicate. */\n' + entry['prototype'] + '\n\n').replace('\n', '\r\n').encode()
        p.write_bytes(header[:at] + block + header[at:])
        manifest['accepted_functions'].append(entry); manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
        path = cache / 'decompiled-abi/export.tsv'; rows = list(csv.DictReader(path.open(), delimiter='\t')); by_address = {r['address']: r for r in rows}
        for row in csv.DictReader((cache / 'decompiled-token-predicate/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-token-predicate' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            by_address[row['address']] = row
        with path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t'); writer.writeheader(); writer.writerows(by_address.values())
        p = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(p.read_text())
        interfaces['token_and_basic_land_predicates'] = dict(functions=evidence, scope=scope); p.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, recovered_helper=entry['name']), indent=2))


if __name__ == '__main__':
    main()
