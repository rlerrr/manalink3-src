"""Recover focused helpers through legacy flag accessors, preserving storage ownership."""
import argparse
import csv
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
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    flags, callback = {}, None
    for cu in image.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/functions.c':
            continue
        for die in cu.iter_DIEs():
            name = value(die, 'DW_AT_name')
            if die.tag == 'DW_TAG_enumerator' and name in {'SF2_FACE_DOWN_DUE_TO_MANIFEST', 'SF3_CARD_IS_FLIPPED', 'SF2_0', 'SF3_0'}:
                flags[name] = value(die, 'DW_AT_const_value')
            if die.tag == 'DW_TAG_subprogram' and value(die, 'DW_AT_low_pc') == 0x2681710:
                parameter = next(c for c in die.iter_children() if inherited_value(c, 'DW_AT_name') == 'fn')
                signature = type_of(type_of(parameter))
                callback = (type_name(type_of(signature)), [type_name(type_of(c)) for c in signature.iter_children() if c.tag == 'DW_TAG_formal_parameter'])
    assert flags == {'SF2_FACE_DOWN_DUE_TO_MANIFEST': 256, 'SF3_CARD_IS_FLIPPED': 512, 'SF2_0': 0, 'SF3_0': 0}
    assert callback == ('int', ['int', 'int'])
    functions = {row['low_pc'].removeprefix('0x'): row for row in csv.DictReader(
        (cache / '2022-functions.csv').open()) if row['low_pc']}
    entries = [('2679db0', 'get_special_flags_by_instance'),
               ('2681710', 'enters_the_battlefield_with_counters_fn'),
               ('26955c0', 'put_into_play_transformed')]
    addition = '\n\n/* Focused October 2022 helpers using existing source flag storage. */\n'
    metadata = []
    for address, name in entries:
        raw = (cache / 'decompiled-abi' / (address + '.c')).read_text()
        body = clean(raw)
        if name == 'get_special_flags_by_instance':
            body = body.replace('return ((instance->field_21).ext)->special_flags;',
                                'int flags = instance->targets[14].card;\n  return flags == -1 ? 0 : flags;')
        else:
            index = 2 if name == 'enters_the_battlefield_with_counters_fn' else 3
            body, count = re.subn(r'\(\((\w+)->field_21\)\.ext\)->special_flags' + str(index),
                                 rf'get_special_flags{index}_by_instance(\1)', body)
            assert count == 1
            body = body.replace('SF2_0', '0').replace('SF3_0', '0')
            body = body.replace('_func_int_int_int *', 'CardPairFunction ')
        assert '.ext' not in body
        command = ['C:/Program Files/LLVM/bin/clang-format.exe',
                   '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
        body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
        line = functions[address]['line']
        addition += f'\n/* ManalinkEh 2022: 0x{address}; original functions/functions.c:{line}. */\n' + body
        metadata.append(dict(name=name, address=address, source='functions/functions.c', line=line,
                             static=False, prototype=body[:body.index('{')].strip() + ';',
                             raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[],
                             reconstruction='Source compatibility adaptation uses legacy special-flag storage and its -1 sentinel convention.',
                             interface_adaptations=['Extension flags use existing source storage; no extension pointer is projected.']))
    path = root / 'functions/functions.c'
    original = path.read_bytes()
    assert all((' ' + name + '(').encode() not in original for address, name in entries)
    candidate = cache / 'flag-helpers-candidate.c'
    candidate.write_bytes(original + addition.encode())
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
               '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
               '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)]
    result = subprocess.run(command, capture_output=True, text=True)
    (cache / 'flag-helpers-check.log').write_text(result.stderr)
    result.check_returncode()
    if args.apply:
        path.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        header = root / 'manalink.h'
        data = header.read_bytes()
        marker = data.rfind(b'#ifdef __cplusplus')
        prototypes = '\n/* Focused October 2022 flag helper interfaces. */\n' + '\n'.join(x['prototype'] for x in metadata) + '\n\n'
        header.write_bytes(data[:marker] + prototypes.replace('\n', '\r\n').encode() + data[marker:])
        manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(manifest_path.read_text())
        manifest['accepted_functions'].extend(metadata)
        manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
    print(json.dumps(dict(applied=args.apply, functions=[x['name'] for x in metadata]), indent=2))


if __name__ == '__main__':
    main()
