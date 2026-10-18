"""Verify partial engine reads and expose the existing public Suspend helper."""
import argparse
import csv
import json
import re
import subprocess
from pathlib import Path

from inspect_release import Image, inherited_value, value
from export_abi import type_of, type_name


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    globals_expected = {'event_flags': 0x4ef188, 'land_can_be_played': 0x4ef190, 'stack_data': 0x509630}
    absolute = {name.removeprefix('_'): address for name, address, section, kind, storage in image.symbols if section == -1}
    bindings = (root / 'manalink.lds').read_text()
    for name, address in globals_expected.items():
        assert absolute[name] == address
        assert re.search(r'_' + name + r'\s*=\s*0x' + f'{address:x}' + r'\s*;', bindings)
    layouts, types, suspend = {}, {}, None
    for cu in image.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {'functions/functions.c', 'cards/time_spiral.c'}:
            continue
        for die in cu.get_top_DIE().iter_children():
            name = inherited_value(die, 'DW_AT_name')
            if source == 'functions/functions.c' and die.tag == 'DW_TAG_variable' and name in globals_expected:
                typ = type_of(die)
                types[name] = dict(type=type_name(typ), bytes=value(typ, 'DW_AT_byte_size'))
            if source == 'functions/functions.c' and die.tag == 'DW_TAG_typedef' and name == 'stack_data_t':
                typ = type_of(die)
                layouts[name] = dict(bytes=value(typ, 'DW_AT_byte_size'),
                                     fields={value(c, 'DW_AT_name'): value(c, 'DW_AT_data_member_location') for c in typ.iter_children()})
            if value(die, 'DW_AT_low_pc') == 0x20eb0c0:
                assert name == 'suspend_only'
                actual = (type_name(type_of(die)), [type_name(type_of(p)) for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter'])
                assert actual == ('int', ['int', 'int', 'event_t', *(['int'] * 7)])
                suspend = dict(address='20eb0c0', returns=actual[0], parameters=actual[1])
    assert suspend and types['event_flags'] == types['land_can_be_played'] == dict(type='int', bytes=4)
    assert layouts['stack_data_t']['bytes'] == 4 and layouts['stack_data_t']['fields']['generating_event'] == 2
    assert globals_expected['stack_data'] - 4 + 2 == 0x50962e
    assert any(n == '_suspend_only' and storage == 2 and
               image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s - 1].VirtualAddress + offset == 0x20eb0c0
               for n, offset, s, kind, storage in image.functions())
    candidate = root / '.recovery/engine-views-layout.c'
    candidate.write_text('''#include "manalink.h"
#include <stddef.h>
_Static_assert(sizeof(event_flags) == 4, "event word");
_Static_assert(sizeof(land_can_be_played) == 4, "land word");
_Static_assert(sizeof(stack_data_t) == 4, "stack record");
_Static_assert(offsetof(stack_data_t, generating_event) == 2, "event byte");
''')
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only',
                             '-I', str(root), str(candidate)], capture_output=True, text=True)
    result.check_returncode()
    if args.apply:
        path = root / 'cards/time_spiral.c'
        data = path.read_bytes()
        declaration = b'static int suspend_only(int player, int card, event_t event, int turns, int colorless, int black, int blue, int green, int red, int white)'
        assert data.count(declaration) == 1
        path.write_bytes(data.replace(declaration, declaration.removeprefix(b'static ')))
        header = root / 'manalink.h'
        data = header.read_bytes()
        assert not re.search(rb'\bsuspend_only\s*\(', data)
        at = data.rfind(b'#ifdef __cplusplus')
        addition = b'/* Existing Suspend helper: public in October 2022. */\r\n' + declaration.removeprefix(b'static ') + b';\r\n\r\n'
        header.write_bytes(data[:at] + addition + data[at:])
        path = root / 'docs/recovery-evidence/interface-recovery.json'
        evidence = json.loads(path.read_text())
        evidence['partial_engine_word_reads'] = dict(globals=globals_expected, types=types, layouts=layouts,
            normalization='Read-only byte/halfword views of event_flags and land_can_be_played use unsigned shifts/masks; no writer is synthesized.',
            stack_event_read='*(char*)(stack_size*4+0x50962e) is (char)stack_data[stack_size-1].generating_event; caller checks stack_size>0.')
        evidence['existing_suspend_interface'] = dict(**suspend, source='cards/time_spiral.c',
            change='Only static linkage removed and declaration added; existing body retained, not counted as recovered target implementation.')
        path.write_text(json.dumps(evidence, indent=2))
    print(json.dumps(dict(applied=args.apply, globals=globals_expected, suspend=suspend), indent=2))


if __name__ == '__main__':
    main()
