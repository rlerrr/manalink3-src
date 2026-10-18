"""Recover the 14-byte card-count cache, reset and both AI backup pairs."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

import capstone
from inspect_release import Image, value
from export_abi import type_of, type_name


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    log = (cache / 'card-counts-console.log').read_text()
    assert 'RECOVERY_EXPORT_COMPLETE success=6 failed=0' in log and 'ERROR' not in log
    global_info = {}
    for name, private in [('card_counts', False), ('backup_card_counts', True), ('backup0_card_counts', True)]:
        symbol = next(s for s in debug.symbols if s[0] == '_' + name)
        section = debug.pe.sections[symbol[2] - 1]
        assert symbol[4] == (3 if private else 2)
        assert section.Name.rstrip(b'\0') == b'.bss' and section.SizeOfRawData == 0
        global_info[name] = debug.pe.OPTIONAL_HEADER.ImageBase + section.VirtualAddress + symbol[1]
    assert global_info == dict(card_counts=0x2797200, backup_card_counts=0x2784e7c, backup0_card_counts=0x2775bdc)
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/functions.c':
            continue
        die = next(d for d in cu.get_top_DIE().iter_children() if value(d, 'DW_AT_name') == 'card_counts')
        assert type_name(type_of(die)) == 'uint8_t[2][7]'
        assert value(type_of(type_of(type_of(die))), 'DW_AT_byte_size') == 1
        break
    def read(at, length):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + length]
    reset_checks = {'26a9e49': 'c7050072790200000000', '26a9e58': 'c7050472790200000000',
                    '26a9e67': 'c7050872790200000000', '26a9e80': '66c7050c7279020000'}
    for at, code in reset_checks.items():
        assert read(int(at, 16), len(bytes.fromhex(code))) == bytes.fromhex(code)
    functions = {r['name']: r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc']}
    entries = [('backup_data_for_ai', 'backup_card_counts', False), ('restore_data_for_ai', 'backup_card_counts', True),
               ('backup_data_for_ai_0', 'backup0_card_counts', False), ('restore_data_for_ai_0', 'backup0_card_counts', True)]
    disassembler = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); disassembler.detail = True
    copy_evidence = {}
    for function, backup, restoring in entries:
        row = functions[function]; start = int(row['low_pc'], 16); end = int(row['high_pc'], 16)
        source_at, dest_at = ((global_info[backup], global_info['card_counts']) if restoring else
                              (global_info['card_counts'], global_info[backup]))
        source_offsets = set(); dest_offsets = {}; instructions = []
        for instruction in disassembler.disasm(read(start, end - start), start):
            matched = False
            for operand in instruction.operands:
                if operand.type != capstone.x86.X86_OP_MEM or operand.mem.base or operand.mem.index:
                    continue
                address = operand.mem.disp
                if source_at <= address < source_at + 14 and operand.access & capstone.CS_AC_READ:
                    source_offsets.add(address - source_at); matched = True
                if dest_at <= address < dest_at + 14 and operand.access & capstone.CS_AC_WRITE:
                    dest_offsets[address - dest_at] = operand.size; matched = True
            if matched:
                instructions.append(dict(address=f'{instruction.address:x}', bytes=instruction.bytes.hex(),
                                         instruction=instruction.mnemonic + ' ' + instruction.op_str))
        assert source_offsets == {0, 4, 8, 12} and dest_offsets == {0: 4, 4: 4, 8: 4, 12: 2}
        copy_evidence[function] = dict(source=hex(source_at), destination=hex(dest_at), bytes=14, instructions=instructions)
    save_raw = (cache / 'decompiled-card-counts/26b1f10.c').read_text()
    assert 'save_or_load_data(card_counts,0xe)' in save_raw
    # Preserve the source's save-file layout. Target serialization belongs to
    # the complete newer supplemental record, not an isolated extra field.
    evidence = dict(type='uint8_t[2][7]', bytes=14, globals={n: hex(a) for n, a in global_info.items()},
        initial_value='Zero-initialized .bss', reset_checks=reset_checks, ai_copies=copy_evidence,
        serialization='Target serializes 14 bytes within its larger supplemental record. Save-format migration remains deferred; this recovery does not alter BACKUP_AND_SAVE_LIST.')
    header_path = root / 'manalink.h'; header = header_path.read_bytes()
    assert b'card_counts[2][7]' not in header
    at = header.rfind(b'#ifdef __cplusplus')
    new_header = header[:at] + b'/* October 2022 per-player card-count cache (14 bytes). */\r\nextern uint8_t card_counts[2][7];\r\n\r\n' + header[at:]
    engine_path = root / 'functions/engine.c'; ai_path = root / 'functions/ai.c'
    # These units have no recovered full functions yet; retain pre-pass bytes.
    for path in [engine_path, ai_path]:
        snapshot = cache / 'batch13-before' / path.relative_to(root)
        if not snapshot.exists():
            snapshot.parent.mkdir(parents=True, exist_ok=True); shutil.copyfile(path, snapshot)
    engine = engine_path.read_bytes().decode('latin1').replace('\r\n', '\n')
    ai = ai_path.read_bytes().decode('latin1').replace('\r\n', '\n')
    engine_definition = '/* October 2022 .bss cache @ 0x2797200; declared in target manalink.h. */\nuint8_t card_counts[2][7];\n'
    ai_definitions = '/* October 2022 private AI snapshots of the 14-byte card-count cache. */\nstatic uint8_t backup_card_counts[2][7];\nstatic uint8_t backup0_card_counts[2][7];\n'
    engine = engine.replace('#include "manalink.h"\n', '#include "manalink.h"\n\n' + engine_definition, 1)
    ai = ai.replace('#include "manalink.h"\n', '#include "manalink.h"\n\n' + ai_definitions, 1)
    fragments = []
    def fragment(source, function, body, recovery):
        address = functions[function]['low_pc'].removeprefix('0x')
        start = f'/* Recovered 2022 card-count fragment: {function} @ 0x{address}. */'
        end = f'/* End recovered 2022 card-count fragment: {function}. */'
        raw = (cache / 'decompiled-card-counts' / (address + '.c')).read_text()
        fragments.append(dict(source=source, original_function=function, address=address,
            raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), start_marker=start, end_marker=end,
            body_sha256=hashlib.sha256(body.strip().encode()).hexdigest(), recovery=recovery))
        return f'  {start}\n  {body}\n  {end}\n'
    needle = 'void recalculate_all_cards_in_play(void)\n{\n'
    assert engine.count(needle) == 1
    engine = engine.replace(needle, needle + fragment('functions/engine.c', 'recalculate_all_cards_in_play',
        'memset(card_counts, 0, sizeof(card_counts));', 'Only the target cache reset before recalculation; existing engine body retained.'), 1)
    for function, backup, restoring in entries:
        needle = f'void {function}(void)\n{{\n'; assert ai.count(needle) == 1
        destination, source = ('card_counts', backup) if restoring else (backup, 'card_counts')
        ai = ai.replace(needle, needle + fragment('functions/ai.c', function,
            f'memcpy({destination}, {source}, sizeof(card_counts));',
            'Only the independently verified 14-byte cache snapshot/restore; existing AI body retained.'), 1)
    candidate_dir = cache / 'card-counts-check'; candidate_dir.mkdir(exist_ok=True)
    (candidate_dir / 'manalink.h').write_bytes(new_header)
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only',
        '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
        '-I', str(candidate_dir), '-I', str(root)]
    for name, text in [('engine.c', engine), ('ai.c', ai)]:
        candidate = candidate_dir / name; candidate.write_bytes(text.encode('latin1'))
        result = subprocess.run(command + [str(candidate)], capture_output=True, text=True)
        (candidate_dir / (name + '.log')).write_text(result.stderr); result.check_returncode()
    if args.apply:
        header_path.write_bytes(new_header)
        engine_path.write_bytes(engine.replace('\n', '\r\n').encode('latin1'))
        ai_path.write_bytes(ai.replace('\n', '\r\n').encode('latin1'))
        path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(path.read_text())
        manifest.setdefault('source_fragments', []).extend(fragments)
        manifest.setdefault('source_globals', []).extend([
            dict(source='functions/engine.c', name='card_counts', declaration='uint8_t card_counts[2][7];',
                 target_address='2797200', zero_initialized=True, linkage='external'),
            dict(source='functions/ai.c', name='backup_card_counts', declaration='static uint8_t backup_card_counts[2][7];',
                 target_address='2784e7c', zero_initialized=True, linkage='static'),
            dict(source='functions/ai.c', name='backup0_card_counts', declaration='static uint8_t backup0_card_counts[2][7];',
                 target_address='2775bdc', zero_initialized=True, linkage='static')])
        manifest['source_files'].update({'functions/engine.c': 0, 'functions/ai.c': 0})
        path.write_text(json.dumps(manifest, indent=2))
        rows = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t')); by_address = {r['address']: r for r in rows}
        for row in csv.DictReader((cache / 'decompiled-card-counts/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in by_address
            shutil.copyfile(cache / 'decompiled-card-counts' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            by_address[row['address']] = row
        with (cache / 'decompiled-abi/export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t'); writer.writeheader(); writer.writerows(by_address.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
        interfaces['card_count_cache'] = evidence; path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, cache_bytes=14, source_fragments=len(fragments)), indent=2))


if __name__ == '__main__':
    main()
