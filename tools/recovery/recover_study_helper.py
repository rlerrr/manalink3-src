"""Recover the Study-counter legacy constructor's masked 16-bit card-id load."""
import argparse
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

from inspect_release import Image, inherited_value, value
from promote_recovery import clean


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    address = '23c7a80'
    name = 'create_exile_with_study_counter_legacy'
    line, counter = None, None
    for cu in image.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'cards/strixhaven.c':
            continue
        for die in cu.iter_DIEs():
            if value(die, 'DW_AT_low_pc') == int(address, 16):
                assert inherited_value(die, 'DW_AT_name') == name
                line = str(inherited_value(die, 'DW_AT_decl_line'))
            if die.tag == 'DW_TAG_enumerator' and value(die, 'DW_AT_name') == 'COUNTER_STUDY':
                counter = value(die, 'DW_AT_const_value')
        break
    assert line and counter == 107
    assert any(n == '_' + name and storage == 3 and
               image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[section - 1].VirtualAddress + offset == int(address, 16)
               for n, offset, section, kind, storage in image.functions())
    raw = (cache / 'decompiled-abi' / (address + '.c')).read_text()
    body = 'static ' + clean(raw)
    assert len(re.findall(r'\buVar1\b', body)) == 5
    old = 'uVar1._0_2_ = cards_data[iid].id;\n    uVar1._2_2_ = cards_data[iid].reserved2;'
    assert old in body
    body = body.replace(old, 'uVar1 = cards_data[iid].id;')
    assert '(uint16_t)uVar1' in body and 'uVar1 & 0xffff' in body
    assert '._' not in body
    command = ['C:/Program Files/LLVM/bin/clang-format.exe',
               '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
    body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
    addition = f'\n\n/* ManalinkEh 2022: 0x{address}; original cards/strixhaven.c:{line}. */\n' + body
    path = root / 'cards/strixhaven.c'
    original = path.read_bytes()
    assert ('void ' + name + '(').encode() not in original
    candidate = cache / 'study-helper-candidate.c'
    candidate.write_bytes(original + addition.encode())
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
               '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
               '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)]
    result = subprocess.run(command, capture_output=True, text=True)
    (cache / 'study-helper-check.log').write_text(result.stderr)
    result.check_returncode()
    entry = dict(name=name, address=address, source='cards/strixhaven.c', line=line, static=True,
                 prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
                 recovered_literals=[], reconstruction='Only low 16 bits of the combined card-data load are consumed; read the existing uint16_t id field directly.',
                 enum_value_checks={'COUNTER_STUDY': counter})
    if args.apply:
        path.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(manifest_path.read_text())
        manifest['accepted_functions'].append(entry)
        manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
    print(json.dumps(dict(applied=args.apply, helper=name, counter=counter), indent=2))


if __name__ == '__main__':
    main()
