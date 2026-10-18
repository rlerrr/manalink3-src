"""Recover the spell/Magecraft core using the existing copied-spell flag accessor.

This is a source compatibility adaptation, not an adoption of the target extension
pointer or its lifecycle. The named flag is bit 31 in both releases.
"""
import argparse
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

from inspect_release import Image, value
from promote_recovery import clean


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert any(n == '_sp_ability_trigger_impl' and storage == 3 and
               image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s - 1].VirtualAddress + v == 0x2684f40
               for n, v, s, kind, storage in image.functions())
    flag = None
    for cu in image.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/functions.c':
            continue
        for die in cu.iter_DIEs():
            if die.tag == 'DW_TAG_enumerator' and value(die, 'DW_AT_name') == 'SF2_COPIED_FROM_STACK':
                flag = value(die, 'DW_AT_const_value')
    assert flag == -2147483648
    assert re.search(r'SF2_COPIED_FROM_STACK\s*=\s*1\s*<<\s*31', (root / 'defs.h').read_text())
    function = next(row for row in csv.DictReader((cache / '2022-functions.csv').open())
                    if row['low_pc'] == '0x2684f40')
    line = function['line']
    raw = (cache / 'decompiled-abi/2684f40.c').read_text()
    globals = [row[1] for line in (cache / 'absolute-interfaces.tsv').read_text().splitlines()
               if (row := line.split('\t'))[2] == 'global']
    body = clean(raw, globals)
    body, count = re.subn(r'\(\((\w+)->field_21\)\.ext\)->special_flags2',
                         r'get_special_flags2_by_instance(\1)', body)
    assert count == 1
    body = body.replace('SF2_0', '0')
    body = 'static ' + body
    command = ['C:/Program Files/LLVM/bin/clang-format.exe',
               '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
    body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
    addition = (f'\n\n/* ManalinkEh 2022: 0x2684f40; original functions/functions.c:{line}. */\n'
                '/* Copied-spell flag read adapted to the existing source accessor.\n'
                ' * The target stores this flag in card_ext_t at offset 184. */\n' + body)
    source = root / 'functions/functions.c'
    original = source.read_bytes()
    if b'void sp_ability_trigger_impl(' in original:
        raise ValueError('Spell trigger core already recovered')
    candidate = cache / 'spell-trigger-core-candidate.c'
    candidate.write_bytes(original + addition.encode())
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
               '-fsyntax-only', '-ferror-limit=0', '-Werror=implicit-function-declaration',
               '-Werror=incompatible-pointer-types', '-Werror=return-type', '-Werror=uninitialized',
               '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)]
    result = subprocess.run(command, capture_output=True, text=True)
    (cache / 'spell-trigger-core-check.log').write_text(result.stderr)
    result.check_returncode()
    metadata = dict(name='sp_ability_trigger_impl', address='2684f40', source='functions/functions.c',
                    line=line, static=True, prototype=body[:body.index('{')].strip() + ';',
                    raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[],
                    reconstruction='Copied-spell bit read uses get_special_flags2_by_instance rather than projecting card_ext_t.',
                    interface_adaptations=['Three-argument is_humiliated calls use is_humiliated_for_ability.',
                                            'The legacy accessor maps its -1 sentinel to no flags; target raw extension storage has no sentinel.'],
                    copied_spell_flag=dict(name='SF2_COPIED_FROM_STACK', target_value=flag, bit=31))
    if args.apply:
        source.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        path = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(path.read_text())
        manifest['accepted_functions'].append(metadata)
        manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_text(json.dumps(manifest, indent=2))
    print(json.dumps(dict(applied=args.apply, function=metadata), indent=2))


if __name__ == '__main__':
    main()
