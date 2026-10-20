"""Recover the second keyword-word enum from its concrete DWARF declaration."""
import argparse
import hashlib
import json
import subprocess
from pathlib import Path

from inspect_release import Image, value
from export_abi import type_of


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(data)
    expected = {'SP_KEYWORD2_0': 0, 'SP_KEYWORD2_PROT_MISC': 1,
        'SP_KEYWORD2_GRANTED_PROT_MISC': 2, 'SP_KEYWORD2_HEXPROOF_MISC': 4, 'SP_KEYWORD2_CANNON_FODDER': 8}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/abilities.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag != 'DW_TAG_typedef' or value(die, 'DW_AT_name') != 'sp_keyword2_t':
                continue
            enum = type_of(die)
            assert enum.tag == 'DW_TAG_enumeration_type' and value(enum, 'DW_AT_byte_size') == 4
            members = {value(c, 'DW_AT_name'): value(c, 'DW_AT_const_value') for c in enum.iter_children()}
            assert members == expected
            proof = dict(debug_sha256=hashlib.sha256(data).hexdigest(), source='functions/abilities.c',
                typedef_die=hex(die.offset), enum_die=hex(enum.offset), bytes=4, members=members)
            break
        else:
            raise ValueError('Missing concrete keyword enum')
        break
    else:
        raise ValueError('Missing abilities unit')
    block = '\n#ifndef SHANDALAR\n/* October 2022 second keyword word; verified against DWARF. */\ntypedef enum\n{\n' + ',\n'.join(f'  {n} = {v}' for n,v in expected.items()) + '\n} sp_keyword2_t;\n#endif\n\n'
    path = root / 'defs.h'
    original = path.read_bytes().decode('latin1').replace('\r\n', '\n')
    assert '} sp_keyword2_t;' not in original
    at = original.rfind('#endif')
    candidate = original[:at] + block + original[at:]
    probe = root / '.recovery/keyword-enum-check.c'
    probe.write_text('#include "manalink.h"\n' + block + '\n_Static_assert(sizeof(sp_keyword2_t) == 4, "keyword word size");\n' + '\n'.join(f'_Static_assert({n} == {v}, "keyword bit");' for n,v in expected.items()))
    subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
        '-fsyntax-only', '-I', str(root), str(probe)], check=True)
    if args.apply:
        path.write_bytes(candidate.replace('\n', '\r\n').encode('latin1'))
        p = root / 'docs/recovery-evidence/interface-recovery.json'
        evidence = json.loads(p.read_text()); evidence['extension_keyword_enum'] = proof
        p.write_text(json.dumps(evidence, indent=2))
    print(json.dumps(dict(applied=args.apply, enum='sp_keyword2_t', members=len(expected))))


if __name__ == '__main__':
    main()
