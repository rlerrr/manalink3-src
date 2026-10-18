"""Recover the out-of-play subtype table and its reset/AI-copy fragments."""
import argparse
import csv
import hashlib
import json
import shutil
import struct
import subprocess
from pathlib import Path

from inspect_release import Image, value
from export_abi import type_of, type_name


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    addresses = {}
    for name, storage in [('added_subtypes_not_in_play', 2), ('backup_added_subtypes_not_in_play', 3),
                          ('backup0_added_subtypes_not_in_play', 3)]:
        symbol = next(s for s in debug.symbols if s[0] == '_' + name)
        section = debug.pe.sections[symbol[2] - 1]
        assert symbol[4] == storage and section.Name.rstrip(b'\0') == b'.bss' and section.SizeOfRawData == 0
        addresses[name] = debug.pe.OPTIONAL_HEADER.ImageBase + section.VirtualAddress + symbol[1]
    assert addresses == dict(added_subtypes_not_in_play=0x2797020, backup_added_subtypes_not_in_play=0x2784d60,
                             backup0_added_subtypes_not_in_play=0x2775ac0)
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/has_creature_type.c':
            continue
        die = next(d for d in cu.get_top_DIE().iter_children() if value(d, 'DW_AT_name') == 'added_subtypes_not_in_play')
        array = type_of(die); assert type_name(array) == 'subtypes_not_in_play_t[2][16]'
        record = type_of(type_of(array)); assert value(record, 'DW_AT_byte_size') == 8
        members = {value(m, 'DW_AT_name'): value(m, 'DW_AT_data_member_location') for m in record.iter_children()}
        assert members == dict(subtype=0, added_forced=4)
        break
    def read(at, length):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + length]
    reset = 'c704c5a070790200000000c704c52070790200000000c604c5a470790200c604c524707902004083f80f75d4'
    assert read(0x26a9ed0, len(bytes.fromhex(reset))) == bytes.fromhex(reset)
    functions = {r['name']: r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc']}
    entries = [('backup_data_for_ai', 'backup_added_subtypes_not_in_play', False),
               ('restore_data_for_ai', 'backup_added_subtypes_not_in_play', True),
               ('backup_data_for_ai_0', 'backup0_added_subtypes_not_in_play', False),
               ('restore_data_for_ai_0', 'backup0_added_subtypes_not_in_play', True)]
    copy_evidence = {}
    for function, backup, restoring in entries:
        row = functions[function]; start = int(row['low_pc'], 16); length = int(row['high_pc'], 16) - start
        src, dst = ((addresses[backup], addresses['added_subtypes_not_in_play']) if restoring else
                    (addresses['added_subtypes_not_in_play'], addresses[backup]))
        sequence = b'\xbe' + struct.pack('<I', src) + b'\xbf' + struct.pack('<I', dst) + bytes.fromhex('b940000000')
        if function == 'backup_data_for_ai_0':
            sequence += bytes.fromhex('a3c05b7702')  # unrelated EAX store; ESI/EDI/ECX unchanged
        sequence += bytes.fromhex('f3a5')
        code = read(start, length); at = code.find(sequence)
        assert at >= 0 and code.count(sequence) == 1, function
        copy_evidence[function] = dict(address=f'{start + at:x}', bytes=sequence.hex(), dwords=64, total_bytes=256)
    assert 'save_or_load_data(added_subtypes_not_in_play,0x100)' in (cache / 'decompiled-abi/26b1f10.c').read_text()
    typedef = 'typedef struct\n{\n  subtype_t subtype;\n  int8_t added_forced;\n} subtypes_not_in_play_t;\n'
    path_defs = root / 'defs.h'; defs = path_defs.read_bytes().decode('latin1').replace('\r\n', '\n')
    assert 'subtypes_not_in_play_t;' not in defs
    anchor = '} reported_subtypes_t;\n'; assert defs.count(anchor) == 1
    defs = defs.replace(anchor, anchor + '\n/* October 2022 out-of-play subtype modifiers: eight-byte records. */\n' + typedef, 1)
    path_header = root / 'manalink.h'; header = path_header.read_bytes(); at = header.rfind(b'#ifdef __cplusplus')
    assert b'added_subtypes_not_in_play[2][16]' not in header
    new_header = header[:at] + b'/* October 2022 per-player subtype modifiers outside the battlefield. */\r\nextern subtypes_not_in_play_t added_subtypes_not_in_play[2][16];\r\n\r\n' + header[at:]
    paths = {s: root / s for s in ['functions/has_creature_type.c', 'functions/engine.c', 'functions/ai.c']}
    sources = {s: p.read_bytes().decode('latin1').replace('\r\n', '\n') for s, p in paths.items()}
    global_declaration = 'subtypes_not_in_play_t added_subtypes_not_in_play[2][16];'
    backup_declarations = [f'static uint8_t {name}[256];' for name in addresses if name != 'added_subtypes_not_in_play']
    declarations = [('functions/has_creature_type.c', 'added_subtypes_not_in_play', global_declaration),
                    ('functions/ai.c', 'backup_added_subtypes_not_in_play', backup_declarations[0]),
                    ('functions/ai.c', 'backup0_added_subtypes_not_in_play', backup_declarations[1])]
    sources['functions/has_creature_type.c'] = sources['functions/has_creature_type.c'].replace('#include "manalink.h"\n',
        '#include "manalink.h"\n\n/* October 2022 .bss subtype table @ 0x2797020. */\n' + global_declaration + '\n', 1)
    sources['functions/ai.c'] = sources['functions/ai.c'].replace('#include "manalink.h"\n',
        '#include "manalink.h"\n\n/* October 2022 private subtype-table snapshots. */\n' + '\n'.join(backup_declarations) + '\n', 1)
    fragments = []
    def fragment(source, function, body, recovery):
        address = functions[function]['low_pc'].removeprefix('0x')
        start = f'/* Recovered 2022 subtype-table fragment: {function} @ 0x{address}. */'
        end = f'/* End recovered 2022 subtype-table fragment: {function}. */'
        raw = (cache / 'decompiled-abi' / (address + '.c')).read_text()
        indented = '  ' + body.replace('\n', '\n  ') + '\n'
        fragments.append(dict(source=source, original_function=function, address=address,
            raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), start_marker=start, end_marker=end,
            body_sha256=hashlib.sha256(indented.strip().encode()).hexdigest(), recovery=recovery))
        return '  ' + start + '\n' + indented + '  ' + end + '\n'
    body = '''for (int i = 0; i < 15; ++i)
{
  added_subtypes_not_in_play[1][i].subtype = SUBTYPE_INVALID;
  added_subtypes_not_in_play[0][i].subtype = SUBTYPE_INVALID;
  added_subtypes_not_in_play[1][i].added_forced = 0;
  added_subtypes_not_in_play[0][i].added_forced = 0;
}'''
    engine = sources['functions/engine.c']; needle = '  minimize_nondraining_mana();\n'; assert engine.count(needle) == 1
    sources['functions/engine.c'] = engine.replace(needle, fragment('functions/engine.c', 'recalculate_all_cards_in_play', body,
        'Reset the subtype and mode fields of slots 0..14 for both players; preserve slot 15 and padding.') + '\n' + needle, 1)
    for function, backup, restoring in entries:
        needle = f'void {function}(void)\n{{\n'; assert sources['functions/ai.c'].count(needle) == 1
        src, dst = ('added_subtypes_not_in_play', backup) if not restoring else (backup, 'added_subtypes_not_in_play')
        sources['functions/ai.c'] = sources['functions/ai.c'].replace(needle, needle + fragment('functions/ai.c', function,
            f'memcpy({dst}, {src}, sizeof(added_subtypes_not_in_play));', 'Only the independently verified 256-byte subtype-table copy; existing AI body retained.'), 1)
    candidate_dir = cache / 'subtype-cache-check'; candidate_dir.mkdir(exist_ok=True)
    (candidate_dir / 'defs.h').write_bytes(defs.encode('latin1')); (candidate_dir / 'manalink.h').write_bytes(new_header)
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only',
        '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
        '-I', str(candidate_dir), '-I', str(root)]
    for source, text in sources.items():
        candidate = candidate_dir / Path(source).name; candidate.write_bytes(text.encode('latin1'))
        result = subprocess.run(command + [str(candidate)], capture_output=True, text=True)
        (candidate_dir / (candidate.stem + '.log')).write_text(result.stderr); result.check_returncode()
    if args.apply:
        path_defs.write_bytes(defs.replace('\n', '\r\n').encode('latin1')); path_header.write_bytes(new_header)
        for source, text in sources.items():
            paths[source].write_bytes(text.replace('\n', '\r\n').encode('latin1'))
        path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(path.read_text())
        manifest.setdefault('source_fragments', []).extend(fragments)
        manifest.setdefault('source_globals', []).extend(dict(source=s, name=n, declaration=d, target_address=f'{addresses[n]:x}',
            zero_initialized=True, linkage='external' if n == 'added_subtypes_not_in_play' else 'static') for s, n, d in declarations)
        path.write_text(json.dumps(manifest, indent=2))
        path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
        interfaces['out_of_play_subtype_cache'] = dict(record_size=8, members=members, dimensions=[2, 16], bytes=256,
            globals={n: f'{a:x}' for n, a in addresses.items()}, reset=dict(address='26a9ed0', bytes=reset, slots=15), ai_copies=copy_evidence,
            serialization='Target serializes 256 bytes in the newer supplemental record; complete save-format migration remains deferred.')
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, table_bytes=256, fragments=len(fragments)), indent=2))


if __name__ == '__main__':
    main()
