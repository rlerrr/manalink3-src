"""Restore the two machine-verified array endpoints in the IID subtype comparison."""
import argparse
import hashlib
import json
import subprocess
from pathlib import Path

from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, code_only, UNRESOLVED


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    name = 'shares_creature_subtype_by_iid'
    address = '2673b90'
    source = 'functions/has_creature_type.c'
    evidence = None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != source:
            continue
        die = next(d for d in cu.get_top_DIE().iter_children()
                   if value(d, 'DW_AT_low_pc') == int(address, 16))
        assert inherited_value(die, 'DW_AT_name') == name
        parameters = [(inherited_value(p, 'DW_AT_name'), type_name(type_of(p)))
                      for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
        assert parameters == [(p, 'int') for p in ['p0', 'c0', 'p1', 'iid']]
        local = next(d for d in die.iter_children() if value(d, 'DW_AT_name') == 'subtypes')
        assert type_name(type_of(local)) == 'int[2][26]'
        evidence = dict(line=str(inherited_value(die, 'DW_AT_decl_line')), parameters=parameters,
                        local_type='int[2][26]', size=value(die, 'DW_AT_high_pc'))
        break
    assert evidence is not None
    assert any(n == '_' + name and storage == 2 and debug.pe.OPTIONAL_HEADER.ImageBase +
               debug.pe.sections[s - 1].VirtualAddress + off == int(address, 16)
               for n, off, s, kind, storage in debug.functions())

    def read(at, length):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections
                       if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + length]

    checks = {
        '2673c1f': '8d9d24ffffff',   # first row at EBP-220
        '2673c27': 'b934000000',     # zero 52 dwords
        '2673c93': '894c9368',       # second row is first +104 bytes
        '2673c98': '83fa1a',         # copy 26 dwords
        '2673ca5': '8d758c8d4df4',   # row 1 at EBP-116; end at EBP-12
        '2673cd0': '83c00439c874d9', # inner comparison advances to row-1 end
        '2673d27': '83fa1a',         # first-row copy also 26 dwords
        '2673d70': '8d558c8d4df4',   # changeling scan has the same row/end
        '2673d78': '83c20439ca74e1',
    }
    for at, expected in checks.items():
        assert read(int(at, 16), len(bytes.fromhex(expected))) == bytes.fromhex(expected)
    raw = (cache / 'decompiled-abi' / (address + '.c')).read_text()
    body = clean(raw)
    assert body.count('(int *)&stack0xfffffff0') == 2
    body = body.replace('(int *)&stack0xfffffff0', '(subtypes[1] + 26)')
    assert not UNRESOLVED.search(code_only(body)) and 'stack0x' not in body
    body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
        '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
        input=body.encode(), capture_output=True, check=True).stdout.decode()
    scope = ('Replace only the two stack-end artifacts with the end of subtypes[1], '
             'using the target 26-element row stride. Retain existing get_subtypes '
             'for battlefield lookup; its full extension-based target migration remains deferred.')
    evidence.update(machine_checks=checks, array_bytes=208, row_bytes=104,
                    frame_offsets=dict(row0=-220, row1=-116, end=-12),
                    machine_sha256=hashlib.sha256(read(int(address, 16), evidence['size'])).hexdigest(),
                    scope=scope)
    entry = dict(name=name, address=address, source=source, line=evidence['line'], static=False,
                 prototype=body[:body.index('{')].strip() + ';', recovered_literals=[],
                 raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), reconstruction=scope,
                 interface_evidence=evidence)
    addition = (f'\n\n/* ManalinkEh 2022: 0x{address}; original {source}:{evidence["line"]}. */\n' + body)
    original = (root / source).read_bytes()
    assert (name + '(').encode() not in original
    candidate = cache / 'subtype-comparison-candidate.c'
    candidate.write_bytes(original + addition.encode())
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc',
        '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration',
        '-Werror=incompatible-pointer-types', '-Werror=return-type', '-Werror=uninitialized',
        '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(root), str(candidate)],
        capture_output=True, text=True)
    (cache / 'subtype-comparison-check.log').write_text(result.stderr)
    result.check_returncode()
    if args.apply:
        path = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(path.read_text())
        assert address not in {e['address'] for e in manifest['accepted_functions']}
        (root / source).write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        header_path = root / 'manalink.h'
        header = header_path.read_bytes(); at = header.rfind(b'#ifdef __cplusplus')
        block = ('/* October 2022 subtype comparison. */\n' + entry['prototype'] + '\n\n').replace('\n', '\r\n').encode()
        header_path.write_bytes(header[:at] + block + header[at:])
        manifest['accepted_functions'].append(entry)
        manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_text(json.dumps(manifest, indent=2))
        path = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(path.read_text()); interfaces['iid_subtype_comparison'] = evidence
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helper=name), indent=2))


if __name__ == '__main__':
    main()
