"""Recover Zendikar Rising's private bit-count table before promoting Party users."""
import argparse
import json
from pathlib import Path

from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    proof = None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'cards/zendikar_rising.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_variable' and inherited_value(die, 'DW_AT_name') == 'number_bits_set':
                typ = type_of(die)
                assert type_name(typ) == 'int[16]' and value(type_of(typ), 'DW_AT_byte_size') == 4
                assert value(die, 'DW_AT_location') == [3, 32, 64, 119, 2]
                proof = dict(variable_die=hex(die.offset), type_die=hex(typ.offset),
                             type='int[16]', bytes=64, address='2774020', line=value(die,'DW_AT_decl_line'))
        cu._dielist.clear();cu._diemap.clear()
    assert proof
    symbols = [(off,s,storage) for name,off,s,kind,storage in debug.symbols if name == '_number_bits_set']
    assert len(symbols) == 1
    off,section,storage = symbols[0]
    assert storage == 3 and debug.pe.sections[section-1].Name.rstrip(b'\0') == b'.bss'
    assert debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[section-1].VirtualAddress + off == 0x2774020
    declaration = 'static int number_bits_set[16];'
    source = root/'cards/zendikar_rising.c'
    assert declaration.encode() not in source.read_bytes()
    if args.apply:
        source.write_bytes(source.read_bytes().rstrip() + (
            '\r\n\r\n/* ManalinkEh 2022 private Party bit-count cache @ 0x2774020; initialized by party_size. */\r\n' +
            declaration + '\r\n').encode())
        path = root/'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(path.read_text())
        manifest.setdefault('source_globals',[]).append(dict(source='cards/zendikar_rising.c',name='number_bits_set',
            declaration=declaration,target_address='2774020',zero_initialized=True,linkage='static'))
        path.write_text(json.dumps(manifest,indent=2))
        path = root/'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(path.read_text());interfaces['party_bit_count_cache'] = proof
        path.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply, global_bytes=64),indent=2))


if __name__ == '__main__':
    main()
