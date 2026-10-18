"""Recover the private protection worker and repair its conflated public wrapper."""
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
from promote_recovery import clean, code_only, UNRESOLVED
from audit_source import function_body


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    source = 'functions/abilities.c'; evidence = {}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != source:
            continue
        for die in cu.get_top_DIE().iter_children():
            at = value(die, 'DW_AT_low_pc')
            if at not in {0x26dd200, 0x26e09a0}:
                continue
            assert inherited_value(die, 'DW_AT_name') == 'get_protection_source'
            parameters = [(inherited_value(p, 'DW_AT_name'), type_name(type_of(p)))
                          for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
            assert parameters == [('can_return_dmg', 'int8_t'), ('eff_acc', 'int *'), ('eff_ac', 'int *')]
            evidence[f'{at:x}'] = dict(line=str(inherited_value(die, 'DW_AT_decl_line')),
                                      parameters=parameters, bytes=value(die, 'DW_AT_high_pc'))
        break
    assert len(evidence) == 2
    for at, name, linkage in [(0x26dd200, '_get_protection_source.part.0', 3),
                              (0x26e09a0, '_get_protection_source', 2)]:
        assert any(n == name and storage == linkage and debug.pe.OPTIONAL_HEADER.ImageBase +
                   debug.pe.sections[s - 1].VirtualAddress + off == at
                   for n, off, s, kind, storage in debug.functions())

    def read(at, length):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections
                       if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + length]

    checks = {'26e09a3': '833d9891730000', '26e09aa': '8b45088b550c8b4d10',
              '26e09b3': '780b0fbec05de942c8ffff',  # signed byte to EAX, jump to worker
              '26dd205': '89c75389ce83ec1089d3',    # EAX, ECX, EDX worker parameters
              '26dd249': '8b14d534707e006681fa8603',
              '26dd257': '81e2ffff000081ea8603000083fa01',  # only low 16 bits observed
              '26dd2e8': '8b900c0100008b8810010000'}
    for at, expected in checks.items():
        assert read(int(at, 16), len(bytes.fromhex(expected))) == bytes.fromhex(expected)
    log = (cache / 'protection-worker-console.log').read_text()
    assert 'RECOVERY_PROTECTION_WORKER_COMPLETE count=1' in log
    assert 'RECOVERY_EXPORT_COMPLETE success=2 failed=0' in log and 'ERROR' not in log
    raw = {a: (cache / 'decompiled-protection-worker' / (a + '.c')).read_text() for a in evidence}
    worker = raw['26dd200']
    pattern = r'uVar3\._0_2_ = cards_data\[iVar2\]\.id;\s*uVar3\._2_2_ = cards_data\[iVar2\]\.reserved2;'
    worker, count = re.subn(pattern, 'uVar3 = (uint)cards_data[iVar2].id;', worker)
    assert count == 1
    worker = worker.replace('(undefined2)uVar3', '(uint16_t)uVar3')
    bodies = {'26dd200': 'static ' + clean(worker), '26e09a0': clean(raw['26e09a0'])}
    for at, body in bodies.items():
        assert not UNRESOLVED.search(code_only(body)) and not re.search(r'\._\d+_\d+_', body)
        bodies[at] = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
            '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
            input=body.encode(), capture_output=True, check=True).stdout.decode()
    path = root / source; original = path.read_bytes().decode('latin1').replace('\r\n', '\n')
    old = function_body(original, 'get_protection_source')
    assert old.count('get_protection_source(') == 2
    replacement = bodies['26e09a0'].removeprefix('void ').rstrip()
    assert original.count(old) == 1
    candidate = original.replace(old, replacement, 1)
    marker = '/* ManalinkEh 2022: 0x26e09a0;'
    at = candidate.index(marker)
    declaration = 'static void get_protection_source_impl(int8_t can_return_dmg, int *eff_acc, int *eff_ac);\n\n'
    candidate = candidate[:at] + declaration + candidate[at:]
    candidate += ('\n\n/* ManalinkEh 2022: 0x26dd200; original functions/abilities.c:2930. */\n' + bodies['26dd200'])
    check = cache / 'protection-worker-candidate.c'; check.write_bytes(candidate.encode('latin1') +
        b'\n_Static_assert(sizeof(cards_data[0].id) == 2, "CSV ID width");\n')
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc',
        '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration',
        '-Werror=incompatible-pointer-types', '-Werror=return-type', '-Werror=uninitialized',
        '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(root), str(check)],
        capture_output=True, text=True)
    (cache / 'protection-worker-check.log').write_text(result.stderr); result.check_returncode()
    scope = ('Distinct public wrapper and COFF-private .part.0 worker. Replace the spurious '
             'recursive wrapper call with the verified worker call. Only the low 16 bits '
             'of the combined ID/reserved load are observed, represented by the ID field.')
    proof = dict(functions=evidence, machine_checks=checks, scope=scope,
        hashes={a: hashlib.sha256(read(int(a, 16), e['bytes'])).hexdigest() for a, e in evidence.items()})
    if args.apply:
        manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(manifest_path.read_text())
        wrapper = next(f for f in manifest['accepted_functions'] if f['address'] == '26e09a0')
        assert '26dd200' not in {f['address'] for f in manifest['accepted_functions']}
        old_hash = wrapper['raw_sha256']
        wrapper.update(raw_sha256=hashlib.sha256(raw['26e09a0'].encode()).hexdigest(),
                       reconstruction=scope, interface_evidence=proof, previous_raw_sha256=old_hash)
        manifest['accepted_functions'].append(dict(name='get_protection_source_impl',
            original_name='get_protection_source', address='26dd200', source=source, line='2930', static=True,
            prototype=declaration.strip(), raw_sha256=hashlib.sha256(raw['26dd200'].encode()).hexdigest(),
            recovered_literals=[], reconstruction=scope, interface_evidence=proof))
        manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_bytes(candidate.replace('\n', '\r\n').encode('latin1'))
        rows_path = cache / 'decompiled-abi/export.tsv'
        rows = list(csv.DictReader(rows_path.open(), delimiter='\t'))
        replacements = {r['address']: r for r in csv.DictReader(
            (cache / 'decompiled-protection-worker/export.tsv').open(), delimiter='\t')}
        assert set(replacements) == set(evidence)
        for row in rows:
            if row['address'] in replacements:
                row.update(replacements[row['address']])
        for a in evidence:
            shutil.copyfile(cache / 'decompiled-protection-worker' / (a + '.c'), cache / 'decompiled-abi' / (a + '.c'))
        with rows_path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t')
            writer.writeheader(); writer.writerows(rows)
        manifest_path.write_text(json.dumps(manifest, indent=2))
        p = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(p.read_text())
        interfaces['protection_source_worker'] = proof; p.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, added_helpers=1, corrected_wrappers=1), indent=2))


if __name__ == '__main__':
    main()
