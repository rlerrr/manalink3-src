"""Recover the partial basic-land color helper and its two callers without inventing a full API."""
import argparse
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from inspect_release import Image
from promote_recovery import clean, UNRESOLVED, strip_comments
from audit_source import function_body


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    source = 'cards/ice_age.c'
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    metadata = {r['low_pc'][2:]: r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    addresses = ['23ed750', '2403360', '2406c20']
    assert any(n == '_basic_subtype_to_color.part.0' and storage == 3 and runtime.pe.OPTIONAL_HEADER.ImageBase + runtime.pe.sections[s - 1].VirtualAddress + off == 0x23ed750 for n, off, s, kind, storage in runtime.functions())
    bodies, entries = {}, []
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    proof = dict(scope='Retain the compiler part function as a private partial helper. The first three basic-land cases stay in each card body; all partial-helper calls retain their arguments and canonical machine destinations.', functions={})
    for address in addresses:
        row = metadata[address]
        assert row['source'] == source
        raw = (cache / 'decompiled-abi' / (address + '.c')).read_text()
        code = runtime.pe.get_data(int(address, 16) - runtime.pe.OPTIONAL_HEADER.ImageBase, int(row['high_pc'], 16) - int(address, 16))
        instructions = list(decoder.disasm(code, int(address, 16)))
        assert sum(i.size for i in instructions) == len(code)
        calls = [dict(address=f'{i.address:x}', bytes=i.bytes.hex(), target='23ed750') for i in instructions if i.mnemonic == 'call' and i.op_str == '0x23ed750']
        if address == '23ed750':
            assert 'int __regparm1 basic_subtype_to_color(int subt)' in raw
            assert any(i.mnemonic == 'cmp' and i.op_str == 'eax, 0x5006' for i in instructions)
            assert any(i.mnemonic == 'cmp' and i.op_str == 'eax, 0x5007' for i in instructions)
            assert not calls
            name = 'basic_subtype_to_color_part_2022'
        else:
            name = row['name']
            count = 5 if address == '2403360' else 4
            assert len(calls) == count and len(re.findall(r'\bbasic_subtype_to_color\s*\(', strip_comments(raw))) == count
        body = clean(raw, use_mana_api_2022=True)
        body = re.sub(r'\bbasic_subtype_to_color\b', 'basic_subtype_to_color_part_2022', body)
        if address == '23ed750':
            body = 'static ' + body
        assert not UNRESOLVED.search(strip_comments(body))
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe', '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'], input=body.encode(), capture_output=True, check=True).stdout.decode().replace('\r\n', '\n')
        prototype = body[:body.index('{')].strip() + ';'
        detail = dict(machine_bytes=len(code), machine_sha256=hashlib.sha256(code).hexdigest(), calls=calls, partial_helper=address == '23ed750')
        proof['functions'][address] = detail
        entries.append(dict(name=name, original_name=row['name'], address=address, source=source, line=row['line'], static=address == '23ed750', prototype=prototype, raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[], reconstruction=proof['scope'], basic_color_part_evidence=detail, reconstruction_body_sha256=hashlib.sha256(function_body(body, name).encode()).hexdigest()))
        bodies[address] = body
    candidate = (root / source).read_bytes().decode('latin1').replace('\r\n', '\n')
    for entry in entries:
        candidate += f"\n\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n" + bodies[entry['address']]
    header = (root / 'manalink.h').read_bytes().decode('latin1').replace('\r\n', '\n')
    at = header.rfind('#ifdef __cplusplus')
    header = header[:at] + '/* October 2022 basic-land color replacement callers. */\n#ifndef SHANDALAR\n' + '\n'.join(e['prototype'] for e in entries if not e['static']) + '\n#endif\n\n' + header[at:]
    scratch = cache / 'basic-color-part-check'
    scratch.mkdir(exist_ok=True)
    (scratch / 'manalink.h').write_bytes(header.encode('latin1'))
    (scratch / 'ice_age.c').write_bytes(candidate.encode('latin1'))
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(scratch), '-I', str(root), str(scratch / 'ice_age.c')], capture_output=True, text=True)
    (scratch / 'check.log').write_text(result.stderr)
    result.check_returncode()
    if args.apply:
        p = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(p.read_text())
        assert not set(addresses) & {f['address'] for f in manifest['accepted_functions']}
        (root / source).write_bytes(candidate.replace('\n', '\r\n').encode('latin1'))
        (root / 'manalink.h').write_bytes(header.replace('\n', '\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries)
        manifest['accepted'] = len(manifest['accepted_functions'])
        p.write_text(json.dumps(manifest, indent=2))
        p = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(p.read_text())
        interfaces['basic_color_part_2022'] = proof
        p.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, implementations=3, card_functions=2, partial_helpers=1)))


if __name__ == '__main__':
    main()
