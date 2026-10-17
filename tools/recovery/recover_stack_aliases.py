"""Recover the Wrenn emblem's DWARF-proven overlapping lexical stack objects."""
import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path

from inspect_release import Image, inherited_value, value
from export_abi import type_name, type_of
from promote_recovery import clean


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    source = root / 'cards/modern_horizons.c'
    address, name = '233e8b0', 'card_wrenn_and_six_emblem'
    raw = (cache / 'decompiled-abi' / (address + '.c')).read_text()
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    locals = {}
    for cu in image.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'cards/modern_horizons.c':
            continue
        def walk(die):
            local_name = inherited_value(die, 'DW_AT_name')
            if die.tag == 'DW_TAG_variable' and local_name in {'test_hand', 'positions', 'iids'}:
                locals[local_name] = dict(type=type_name(type_of(die)), location=value(die, 'DW_AT_location'))
            for child in die.iter_children():
                walk(child)
        for die in cu.get_top_DIE().iter_children():
            if inherited_value(die, 'DW_AT_name') == name:
                walk(die)
    assert locals['positions']['type'] == locals['iids']['type'] == 'int[500]'
    assert locals['test_hand']['type'] == 'test_definition_t'
    assert locals['positions']['location'] == locals['test_hand']['location']
    body = clean(raw)
    body, changes = re.subn(r'\*\(int \*\)\(test_hand\.message \+ (iVar[23]) \* 4\)',
                           r'positions[\1]', body)
    assert changes == 2, changes
    assert body.count('int iids [500];') == 1
    body = body.replace('int iids [500];', 'int iids [500];\n  int positions[500];')
    command = ['C:/Program Files/LLVM/bin/clang-format.exe',
               '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
    body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
    addition = ('\n\n/* ManalinkEh 2022: 0x233e8b0; original cards/modern_horizons.c:4088. */\n'
                '/* DWARF gives positions[500] and test_hand the same stack location in\n'
                ' * different lexical scopes. Keep them as separate source objects. */\n' + body)
    original = source.read_bytes()
    if b'int card_wrenn_and_six_emblem(' in original:
        raise ValueError('Wrenn emblem already recovered')
    scratch = cache / 'stack-alias-candidate.c'
    scratch.write_bytes(original + addition.encode())
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc',
               '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration',
               '-Werror=incompatible-pointer-types', '-Werror=return-type',
               '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(scratch)]
    result = subprocess.run(command, capture_output=True, text=True)
    (cache / 'stack-alias-source-check.log').write_text(result.stderr)
    result.check_returncode()
    evidence = dict(name=name, address=address, source='cards/modern_horizons.c', line='4088',
                    static=False, prototype=body[:body.index('{')].strip() + ';',
                    raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[],
                    reconstruction='DWARF-proven separate positions[500] array replaces two accesses through overlapping test_hand storage.',
                    stack_variable_evidence=locals)
    if args.apply:
        source.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        manifest = root / 'docs/recovery-evidence/recovered-functions.json'
        saved = json.loads(manifest.read_text())
        saved['accepted_functions'].append(evidence)
        saved['accepted'] = len(saved['accepted_functions'])
        manifest.write_text(json.dumps(saved, indent=2))
    print(json.dumps(dict(applied=args.apply, evidence=evidence), indent=2))


if __name__ == '__main__':
    main()
