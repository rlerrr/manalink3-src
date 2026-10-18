"""Recover drawing imports and small rarity/watermark consumers with ABI evidence."""
import argparse
import csv
import hashlib
import json
import shutil
import subprocess
from pathlib import Path

import pefile
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    dll = pefile.PE(str(cache / '2022-ManalinkEh.dll'))
    signatures = {'get_rarity': ('const Rarities*', 'int csvid', 0x27b5374),
                  'get_watermarks': ('const Watermark*', 'void', 0x27b5378),
                  'get_border_color': ('border_t', 'uint32_t csvid', 0x27b536c)}
    imported = {entry.name.decode(): entry.address for library in dll.DIRECTORY_ENTRY_IMPORT
                if library.dll.lower() == b'drawcardlib.dll' for entry in library.imports if entry.name}
    declarations = (root / 'drawcardlib/drawcardlib.h').read_text()
    producer = (root / 'drawcardlib/Drawcardlib.def').read_text().split()
    for name, (result, parameters, slot) in signatures.items():
        assert imported[name] == slot and name in producer
        assert f'{result} {name}({parameters});' in declarations
    evidence = {'imports': {name: dict(return_type=r, parameters=p, iat=hex(at))
                            for name, (r, p, at) in signatures.items()},
                'signature_source': 'drawcardlib/drawcardlib.h',
                'provider_exports': 'drawcardlib/Drawcardlib.def',
                'scope': 'Existing drawing-library APIs; IAT slots are function pointers, not executable entry points.'}
    functions = {}
    for cu in image.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'cards/unstable.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            name = inherited_value(die, 'DW_AT_name')
            if die.tag == 'DW_TAG_typedef' and name in ('Rarities', 'Watermark'):
                structure = type_of(die)
                evidence[name] = dict(size=value(structure, 'DW_AT_byte_size'),
                    members={value(m, 'DW_AT_name'): value(m, 'DW_AT_data_member_location')
                             for m in structure.iter_children() if m.tag == 'DW_TAG_member'})
            if die.tag == 'DW_TAG_variable' and name == 'watermarks':
                assert value(die, 'DW_AT_location') == [3, 64, 65, 119, 2]
                assert type_name(type_of(die)) == 'Watermark *'
                assert type_of(type_of(die)).tag == 'DW_TAG_const_type'
                evidence['cache'] = dict(address='2774140', type='const Watermark *', initial=0,
                                         line=value(die, 'DW_AT_decl_line'))
        break
    assert evidence['Watermark'] == dict(size=16, members=dict(name=0, disp=4, expansions=8,
        row=12, col=13, copied_expansions=14, saturation=15))
    assert evidence['Rarities']['members']['watermark'] == 2
    assert evidence['Rarities']['members']['default_rarity'] == 1
    symbol = next(s for s in image.symbols if s[0] == '_watermarks' and s[2] > 0 and
                  image.pe.sections[s[2] - 1].VirtualAddress + s[1] + image.pe.OPTIONAL_HEADER.ImageBase == 0x2774140)
    assert symbol[4] == 3
    section = image.pe.sections[symbol[2] - 1]
    assert section.Name.rstrip(b'\0') == b'.bss' and section.SizeOfRawData == 0
    # The compiler uses an unsigned range comparison. Preserve wraparound after
    # integer promotion of the uint8_t member (the decompiler omits that cast).
    code = dll.get_data(0x2302bc5 - dll.OPTIONAL_HEADER.ImageBase, 11)
    assert code == bytes.fromhex('89d083e80383f8010f96c0')
    evidence['rarity_unsigned_range'] = dict(address='2302bc5', bytes=code.hex(),
        interpretation='32-bit subtract 3, unsigned <= 1; rarities 3 and 4 only.')
    log = (cache / 'drawing-console.log').read_text()
    assert 'RECOVERY_DRAWING_INTERFACES_COMPLETE count=3' in log
    assert 'RECOVERY_EXPORT_COMPLETE success=11 failed=0' in log and 'ERROR' not in log

    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    accepted = {f['address'] for f in manifest['accepted_functions']}
    exports = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'))
    selected = {'2302ba0': 'test_rare_or_mythic', '25c49b0': 'test_has_rarity_val',
                '25c49f0': 'get_watermark_by_iid'}
    rows = {r['address']: r for r in exports}
    definitions = {r['low_pc'].removeprefix('0x'): r for r in csv.DictReader(
        (cache / '2022-functions.csv').open()) if r['low_pc']}
    additions = {'cards/unstable.c': '\n\n/* October 2022 private .bss cache @ 0x2774140, original line 17. */\nstatic const Watermark* watermarks = NULL;\n'}
    entries = []
    for address, name in selected.items():
        assert address not in accepted
        raw = (cache / 'decompiled-drawing' / (address + '.c')).read_text()
        body = 'static ' + clean(raw)
        body = body.replace('Rarities *pRVar1;', 'const Rarities *pRVar1;')
        assert 'const Rarities *pRVar1;' in body
        if address == '2302ba0':
            assert '(pRVar1->default_rarity - 3 < 2)' in body
            body = body.replace('(pRVar1->default_rarity - 3 < 2)', '((uint32_t)pRVar1->default_rarity - 3u < 2u)')
        formatter = ['C:/Program Files/LLVM/bin/clang-format.exe',
                     '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
        body = subprocess.run(formatter, input=body.encode(), capture_output=True, check=True).stdout.decode()
        row = rows[address]; line = definitions[address]['line']; source = row['source']
        assert any(n == '_' + name and storage == 3 and image.pe.OPTIONAL_HEADER.ImageBase +
                   image.pe.sections[s - 1].VirtualAddress + off == int(address, 16)
                   for n, off, s, kind, storage in image.functions())
        additions[source] = additions.get(source, '') + f'\n\n/* ManalinkEh 2022: 0x{address}; original {source}:{line}. */\n' + body
        entries.append(dict(name=name, address=address, source=source, line=line, static=True,
            prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
            recovered_literals=[], reconstruction='Retain imported const result and verified private watermark cache; preserve unsigned rarity range where applicable.',
            interface_evidence=evidence))
    header = (root / 'manalink.h').read_bytes()
    at = header.rfind(b'#ifdef __cplusplus')
    declaration_block = '\n/* Existing Drawcardlib APIs used by recovered card predicates. */\n' + ''.join(
        f'__attribute__((dllimport)) {r} {n}({p});\n' for n, (r, p, slot) in signatures.items()) + '\n'
    assert b'get_watermarks(' not in header
    new_header = header[:at] + declaration_block.replace('\n', '\r\n').encode() + header[at:]
    definition_path = root / 'Drawcardlib.def'
    definition_bytes = definition_path.read_bytes()
    assert all(name not in definition_bytes.decode().split() for name in signatures)
    check = cache / 'drawing-check'; check.mkdir(exist_ok=True)
    (check / 'manalink.h').write_bytes(new_header)
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only',
               '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
               '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(check), '-I', str(root)]
    for source, addition in additions.items():
        candidate = check / Path(source).name
        candidate.write_bytes((root / source).read_bytes() + addition.encode())
        result = subprocess.run(command + [str(candidate)], capture_output=True, text=True)
        (check / (candidate.stem + '.log')).write_text(result.stderr); result.check_returncode()
    layout = check / 'layout.c'
    layout.write_text('#include "manalink.h"\n#include <stddef.h>\n' + '\n'.join(
        f'_Static_assert(offsetof({t}, {m}) == {offset}, "{t}.{m}");'
        for t in ('Rarities', 'Watermark') for m, offset in evidence[t]['members'].items()) +
        '\n_Static_assert(sizeof(Watermark) == 16, "watermark stride");\n')
    subprocess.run(command + [str(layout)], check=True, capture_output=True)
    if args.apply:
        shutil.copyfile(definition_path, cache / 'batch11-before/Drawcardlib.def')
        definition_path.write_bytes(definition_bytes.rstrip() + b'\r\n' + ''.join('  ' + n + '\r\n' for n in signatures).encode())
        (root / 'manalink.h').write_bytes(new_header)
        for source, addition in additions.items():
            path = root / source
            path.write_bytes(path.read_bytes().rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        manifest['accepted_functions'].extend(entries); manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
        for row in csv.DictReader((cache / 'decompiled-drawing/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-drawing' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            rows[row['address']] = row
        with (cache / 'decompiled-abi/export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=exports[0], delimiter='\t'); writer.writeheader(); writer.writerows(rows.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(path.read_text()); interfaces['drawing_imports_and_cache'] = evidence
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helpers=list(selected.values()), evidence=evidence), indent=2))


if __name__ == '__main__':
    main()
