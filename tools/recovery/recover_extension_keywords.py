"""Recover extension keyword accessors and separate the private protection worker."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from inspect_release import Image
from promote_recovery import clean, project_extension_keywords, strip_comments, UNRESOLVED
from audit_source import function_body


def sha(text):
    return hashlib.sha256(text.encode()).hexdigest()


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    image = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    foundation = root / 'docs/recovery-evidence/extension-foundation.json'
    layouts = json.loads(foundation.read_text())
    assert layouts['source_recovered']
    words = {m['name']: (m['offset'], m['bytes']) for m in layouts['types']['card_ext_t']['members']}
    assert words['sp_keywords'] == (0, 4) and words['sp_keywords2'] == (4, 4)
    metadata = {f"{int(r['low_pc'], 16):x}": r for r in csv.DictReader(
        (cache / '2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}

    def machine(address):
        r = metadata[address]
        start, end = int(r['low_pc'], 16), int(r['high_pc'], 16)
        rva = start - image.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in image.pe.sections if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return image.data[offset:offset + end - start]

    # Worker receives player/card in EAX/EDX, loads ext at +44, and ORs word +4.
    assert machine('26dd130').hex() == '5589e583ec0889542404890424e86e59fdff8b402c8b500483ca01895004c9c3'
    new = {'26dd130': ('set_misc_protection_2022_impl', True),
           '26df800': ('get_special_abilities_by_instance_2022', False),
           '26e05a0': ('remove_special_ability_2022', False)}
    repaired = {'26de880': 'misc_protection', '26e0db0': 'protection_from_everything',
                '26e0e10': 'protection_from_colored_spells'}
    directories = {a: cache / 'decompiled-keyword-foundation' for a in [*new, '26de880']}
    directories.update({a: cache / 'decompiled-keyword-callers' for a in repaired if a != '26de880'})
    raw = {a: (d / (a + '.c')).read_text() for a, d in directories.items()}
    proof = dict(layout_evidence='extension-foundation.json', layout_sha256=hashlib.sha256(foundation.read_bytes()).hexdigest(),
        scope='Typed keyword-word projections only; 2020 accessors retained. Private regparm2 worker distinguished from event-gated public wrapper.',
        integration_pending='Keyword refresh, remaining legacy keyword readers/writers, native binding, and game behavior remain downstream work.',
        functions={})
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    for address in directories:
        instructions = list(decoder.disasm(machine(address), int(address, 16)))
        branches = [dict(address=f'{i.address:x}', bytes=i.bytes.hex(), instruction=i.mnemonic + ' ' + i.op_str)
                    for i in instructions if i.mnemonic in ('call', 'jmp') and i.op_str == '0x26dd130']
        if address in repaired:
            assert len(branches) == 1 and raw[address].count('set_misc_protection_2022_impl(player,card)') == 1
        _, projections = project_extension_keywords(strip_comments(raw[address]))
        proof['functions'][address] = dict(machine_sha256=hashlib.sha256(machine(address)).hexdigest(),
            bytes=len(machine(address)), worker_branches=branches, projections=projections)

    source = 'functions/abilities.c'
    path = root / source
    candidate = path.read_bytes().decode('latin1').replace('\r\n', '\n')
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    assert not set(new) & {f['address'] for f in manifest['accepted_functions']}
    for address, name in repaired.items():
        old = function_body(candidate, name)
        assert old.count('misc_protection(player, card, player);') == 1
        fixed = old.replace('misc_protection(player, card, player);', 'set_misc_protection_2022_impl(player, card);')
        candidate = candidate.replace(old, fixed, 1)
        entry = next(f for f in manifest['accepted_functions'] if f['address'] == address)
        entry.update(previous_raw_sha256=entry['raw_sha256'], raw_sha256=sha(raw[address]),
            keyword_worker_repair=proof['functions'][address], reconstruction_body_sha256=sha(fixed))
    marker = '/* ManalinkEh 2022: 0x26de880;'
    assert marker in candidate
    declaration = 'static void set_misc_protection_2022_impl(int player, int card);\n\n'
    candidate = candidate.replace(marker, declaration + marker, 1)
    prototypes = []
    for address, (name, static) in new.items():
        body = clean(raw[address])
        if static:
            body = 'static ' + body
        assert name + '(' in body and not UNRESOLVED.search(body)
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
            '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
            input=body.encode(), capture_output=True, check=True).stdout.decode().replace('\r\n', '\n')
        prototype = body[:body.index('{')].strip() + ';'
        if not static:
            prototypes.append(prototype)
        candidate += f"\n\n/* ManalinkEh 2022: 0x{address}; original {source}:{metadata[address]['line']}. */\n" + body
        manifest['accepted_functions'].append(dict(name=name, original_name=metadata[address]['name'],
            address=address, source=source, line=metadata[address]['line'], static=static, prototype=prototype,
            raw_sha256=sha(raw[address]), recovered_literals=[], reconstruction=proof['scope'],
            keyword_interface_evidence=proof['functions'][address], reconstruction_body_sha256=sha(function_body(body, name))))
    check = cache / 'extension-keyword-candidate.c'
    check.write_bytes(candidate.encode('latin1'))
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc',
        '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
        '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format',
        '-I', str(root), str(check)], capture_output=True, text=True)
    (cache / 'extension-keyword-check.log').write_text(result.stderr)
    result.check_returncode()
    if args.apply:
        path.write_bytes(candidate.replace('\n', '\r\n').encode('latin1'))
        header = root / 'manalink.h'
        text = header.read_bytes().decode('latin1').replace('\r\n', '\n')
        at = text.rfind('#ifdef __cplusplus')
        text = text[:at] + '/* Extension keyword APIs recovered from October 2022. */\n#ifndef SHANDALAR\n' + '\n'.join(prototypes) + '\n#endif\n\n' + text[at:]
        header.write_bytes(text.replace('\n', '\r\n').encode('latin1'))
        exports = cache / 'decompiled-abi/export.tsv'
        rows = list(csv.DictReader(exports.open(), delimiter='\t'))
        updates = {r['address']: r for d in set(directories.values()) for r in csv.DictReader((d / 'export.tsv').open(), delimiter='\t') if r['address'] in directories}
        rows = [updates.pop(r['address'], r) for r in rows]
        rows.extend(updates.values())
        with exports.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t')
            writer.writeheader(); writer.writerows(rows)
        for a, d in directories.items():
            shutil.copyfile(d / (a + '.c'), exports.parent / (a + '.c'))
        # Unaccepted callers also benefit from the corrected private prototype.
        for p in (cache / 'decompiled-keyword-callers').glob('*.c'):
            if p.stem not in directories:
                shutil.copyfile(p, exports.parent / p.name)
        manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
        p = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(p.read_text()); interfaces['extension_keywords'] = proof
        p.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, new_helpers=len(new), repaired_functions=len(repaired))))


if __name__ == '__main__':
    main()
