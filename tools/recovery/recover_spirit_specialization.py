"""Recover the constant-quantity Spirit clone without inventing a generic quantity API."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from inspect_release import Image, inherited_value, value
from promote_recovery import clean


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--prepare', action='store_true')
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    quantity, line = None, None
    for cu in image.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'cards/strixhaven.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if value(die, 'DW_AT_low_pc') != 0x23c8990:
                continue
            line = str(inherited_value(die, 'DW_AT_decl_line'))
            parameter = next(c for c in die.iter_children() if inherited_value(c, 'DW_AT_name') == 'amount')
            quantity = value(parameter, 'DW_AT_const_value')
    assert quantity == 1
    symbols = [n for n, v, s, k, st in image.functions()
               if image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s - 1].VirtualAddress + v == 0x23c8990]
    assert any(n.startswith('_make_spirit_strixhaven.constprop.') for n in symbols), symbols
    if args.prepare:
        requests = []
        exports = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'))
        functions = {row['low_pc'].removeprefix('0x'): row for row in csv.DictReader(
            (cache / '2022-functions.csv').open()) if row['low_pc']}
        for row in exports:
            raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
            if re.search(r'\b(?:make_spirit_strixhaven|make_one_spirit_strixhaven)\s*\(', raw):
                assert row['source'] == 'cards/strixhaven.c'
                high = int(functions[row['address']]['high_pc'], 16)
                requests.append(f"{row['address']}\t{row['source']}\t{row['name']}\t{high:x}\n")
        (cache / 'export-request-spirit.tsv').write_text(''.join(requests))
        print(json.dumps(dict(quantity=quantity, symbols=symbols, exports=len(requests))))
        return
    raw_path = cache / 'decompiled-spirit/23c8990.c'
    raw = raw_path.read_text()
    body = 'static ' + clean(raw)
    assert re.search(r'void\s+make_one_spirit_strixhaven\(int player,int card\)', body)
    command = ['C:/Program Files/LLVM/bin/clang-format.exe',
               '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
    body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
    addition = (f'\n\n/* ManalinkEh 2022: 0x23c8990; original cards/strixhaven.c:{line}. */\n'
                '/* Compiler specialization: quantity is always one in this binary. */\n' + body)
    source = root / 'cards/strixhaven.c'
    original = source.read_bytes()
    assert b'void make_one_spirit_strixhaven(' not in original
    candidate = cache / 'spirit-specialization-candidate.c'
    candidate.write_bytes(original + addition.encode())
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
               '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
               '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)]
    result = subprocess.run(command, capture_output=True, text=True)
    (cache / 'spirit-specialization-check.log').write_text(result.stderr)
    result.check_returncode()
    entry = dict(name='make_one_spirit_strixhaven', original_name='make_spirit_strixhaven',
                 address='23c8990', source='cards/strixhaven.c', line=line, static=True,
                 prototype=body[:body.index('{')].strip() + ';',
                 raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[],
                 reconstruction='Concrete DWARF amount=1 removed from machine interface; recover only the quantity-one specialization.',
                 specialization=dict(symbols=symbols, constant_parameters={'amount': quantity},
                                     machine_parameters=['player', 'card'], convention='__regparm2'))
    if args.apply:
        source.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        path = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(path.read_text())
        manifest['accepted_functions'].append(entry)
        manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_text(json.dumps(manifest, indent=2))
        # None of these six addresses have a promoted implementation yet,
        # except the just-added helper whose raw input is copied unchanged.
        new = list(csv.DictReader((cache / 'decompiled-spirit/export.tsv').open(), delimiter='\t'))
        old = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'))
        rows = {row['address']: row for row in old}
        accepted = {f['address'] for f in manifest['accepted_functions']} - {'23c8990'}
        for row in new:
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-spirit' / (row['address'] + '.c'),
                            cache / 'decompiled-abi' / (row['address'] + '.c'))
            rows[row['address']] = row
        with (cache / 'decompiled-abi/export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=old[0], delimiter='\t')
            writer.writeheader()
            writer.writerows(rows.values())
    print(json.dumps(dict(applied=args.apply, entry=entry), indent=2))


if __name__ == '__main__':
    main()
