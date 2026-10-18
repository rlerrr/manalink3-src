"""Recover the end-turn callback's two overlapping player_bits scalar writes."""
import argparse
import hashlib
import json
import subprocess
from pathlib import Path

from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    line = None; array = None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/functions.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if value(die, 'DW_AT_name') == 'player_bits':
                array = type_of(die)
                assert type_name(array) == 'player_bits_t[2]'
                assert value(type_of(type_of(array)), 'DW_AT_byte_size') == 4
            if value(die, 'DW_AT_low_pc') == 0x26a2e90:
                assert inherited_value(die, 'DW_AT_name') == 'end_the_turn_legacy'
                assert value(die, 'DW_AT_high_pc') == 449
                line = str(value(die, 'DW_AT_decl_line'))
        break
    assert line and array
    assert any(n == '_player_bits' and offset == 0x4ef1a4 and section == -1 for n, offset, section, kind, storage in debug.symbols)
    assert '_player_bits = 0x4ef1a4;' in (root / 'Manalink.lds').read_text()
    assert any(n == '_end_the_turn_legacy' and st == 3 and debug.pe.OPTIONAL_HEADER.ImageBase +
        debug.pe.sections[s - 1].VirtualAddress + off == 0x26a2e90
        for n, off, s, kind, st in debug.functions())
    def read(at, length):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + length]
    code = '8b0da4f14e00a1a8f14e0081c9000000030d00000003890da4f14e00a3a8f14e00'
    assert read(0x26a2ed7, len(bytes.fromhex(code))) == bytes.fromhex(code)
    raw = (cache / 'decompiled-abi/26a2e90.c').read_text()
    assert raw.count('_player_bits') == 2 and raw.count('_DAT_004ef1a8') == 2
    body = 'static ' + clean(raw.replace('_player_bits', 'player_bits[0]').replace('_DAT_004ef1a8', 'player_bits[1]'))
    body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
        '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
        input=body.encode(), capture_output=True, check=True).stdout.decode()
    addition = f'\n\n/* ManalinkEh 2022: 0x26a2e90; original functions/functions.c:{line}. */\n' + body
    path_source = root / 'functions/functions.c'; original = path_source.read_bytes()
    assert b'int end_the_turn_legacy(' not in original
    candidate = cache / 'end-turn-legacy-candidate.c'; candidate.write_bytes(original + addition.encode() +
        b'\n_Static_assert(sizeof(player_bits) == 8 && sizeof(player_bits[0]) == 4, "player bits array");\n')
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
        '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
        '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(root), str(candidate)],
        capture_output=True, text=True)
    (cache / 'end-turn-legacy-check.log').write_text(result.stderr); result.check_returncode()
    evidence = dict(global_address='4ef1a4', type='player_bits_t[2]', element_size=4,
        machine_address='26a2ed7', machine_bytes=code, mask='3000000',
        machine_sha256=hashlib.sha256(read(0x26a2e90, 449)).hexdigest(),
        scope='Two separate 32-bit OR operations on player_bits[0] and [1]. Preserve target ability-disable mask writes; broader ability-mask consumer migration remains pending.')
    entry = dict(name='end_the_turn_legacy', address='26a2e90', source='functions/functions.c', line=line, static=True,
        prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
        recovered_literals=[], reconstruction=evidence['scope'], interface_evidence=evidence)
    if args.apply:
        path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(path.read_text())
        assert not any(f['address'] == entry['address'] for f in manifest['accepted_functions'])
        path_source.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        manifest['accepted_functions'].append(entry); manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_text(json.dumps(manifest, indent=2))
        path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
        interfaces['end_turn_player_bits'] = evidence; path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helper=entry['name']), indent=2))


if __name__ == '__main__':
    main()
