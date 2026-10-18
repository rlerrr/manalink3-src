"""Restore the target's five-value query enum from its dispatcher compilation unit."""
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
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes(); debug = Image(data)
    proof = None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/functions.c':
            continue
        die = next(d for d in cu.get_top_DIE().iter_children() if d.tag == 'DW_TAG_typedef' and
                   value(d, 'DW_AT_name') == 'event_query_t')
        enum = type_of(die)
        assert enum.tag == 'DW_TAG_enumeration_type' and value(enum, 'DW_AT_byte_size') == 4
        enumerators = {value(e, 'DW_AT_name'): value(e, 'DW_AT_const_value') for e in enum.iter_children()}
        assert enumerators == dict(QUERY_ADVENTURER=1, QUERY_COMPANION=2,
            QUERY_IS_PLAIN_MANA_PRODUCER=3, QUERY_STORM=4, QUERY_GRAVESTORM=5)
        proof = dict(debug_sha256=hashlib.sha256(data).hexdigest(), source='functions/functions.c',
                     typedef_die=hex(die.offset), enum_die=hex(enum.offset), bytes=4,
                     enumerators=enumerators, encoding=value(enum, 'DW_AT_encoding'))
        break
    assert proof is not None
    declaration = ('/* October 2022 EVENT_QUERY selectors. */\ntypedef enum\n{\n' +
        ',\n'.join('  ' + n + ' = ' + str(v) for n, v in enumerators.items()) + '\n} event_query_t;\n')
    path = root / 'defs.h'; original = path.read_bytes()
    assert b'} event_query_t;' not in original and original.count(b'} event_t;') == 1
    updated = original.replace(b'} event_t;', b'} event_t;\r\n\r\n' + declaration.replace('\n', '\r\n').encode().rstrip(), 1)
    checkdir = cache / 'query-type-check'; checkdir.mkdir(exist_ok=True)
    (checkdir / 'defs.h').write_bytes(updated)
    (checkdir / 'query-type.c').write_text('#include "defs.h"\n_Static_assert(sizeof(event_query_t) == 4, "query width");\n')
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc',
        '-std=gnu99', '-fsyntax-only', '-I', str(checkdir), '-I', str(root), str(checkdir / 'query-type.c')],
        capture_output=True, text=True)
    (checkdir / 'diagnostics.log').write_text(result.stderr); result.check_returncode()
    if args.apply:
        path.write_bytes(updated)
        p = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(p.read_text())
        interfaces['event_query_type'] = proof; p.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, selectors=enumerators), indent=2))


if __name__ == '__main__':
    main()
