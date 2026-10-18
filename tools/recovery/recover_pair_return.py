"""Verify GCC's two-register target_t return and recover the repeat-effect helper."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from inspect_release import Image, inherited_value, value
from export_abi import type_of, type_name
from promote_recovery import clean


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--prepare', action='store_true')
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    address, name = '267b0a0', 'find_repeat_legacy_effect_alt'
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    evidence = None
    for cu in image.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/functions.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if value(die, 'DW_AT_low_pc') != int(address, 16):
                continue
            ret = type_of(die)
            assert type_name(ret) == 'target_t'
            struct = type_of(ret)
            fields = {value(c, 'DW_AT_name'): value(c, 'DW_AT_data_member_location') for c in struct.iter_children()}
            assert value(struct, 'DW_AT_byte_size') == 8 and fields == {'player': 0, 'card': 4}
            evidence = dict(line=str(inherited_value(die, 'DW_AT_decl_line')), return_type='target_t', bytes=8,
                            fields=fields, storage={'player': 'EAX', 'card': 'EDX'}, return_die=hex(ret.offset),
                            ghidra_join_storage='EDX:4,EAX:4 (verified against field assignments and caller card use)')
        break
    assert evidence
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    def read(at, length):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + length]
    assert read(0x267b0f2, 6) == bytes.fromhex('83c8ff83caff')  # EAX=EDX=-1
    assert read(0x267b190, 4) == bytes.fromhex('89d889fa')  # EAX=player EBX, EDX=card EDI
    evidence['return_sequences'] = {'267b0f2': '83c8ff83caff', '267b190': '89d889fa'}
    source = (root / 'functions/functions.c').read_text(errors='replace')
    assert 'EXE_FN(void, 0x4373b0, int, int)(player, card)' in source
    assert 'EXE_FN(void, 0x437400, void)()' in source
    evidence['timestamp_adapter'] = dict(source='functions/functions.c::new_timestamp',
                                         calls=['4373b0(void,int,int)', '437400(void)'])
    exports = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'))
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    accepted = {f['address'] for f in manifest['accepted_functions']}
    if args.prepare:
        functions = {row['low_pc'].removeprefix('0x'): row for row in csv.DictReader(
            (cache / '2022-functions.csv').open()) if row['low_pc']}
        requests = {}
        for row in exports:
            raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
            if re.search(r'\b' + name + r'\s*\(', raw):
                assert row['address'] not in accepted
                requests[row['address']] = row
        requests[address] = dict(address=address, source='functions/functions.c', name=name)
        (cache / 'export-request-pair-return.tsv').write_text(''.join(
            f"{at}\t{row['source']}\t{row['name']}\t{int(functions[at]['high_pc'],16):x}\n" for at, row in requests.items()))
        (cache / 'timestamp-interfaces.tsv').write_text('0x4373b0\tengine_set_timestamp\tvoid\tint\tint\n0x437400\tengine_compact_timestamps\tvoid\n')
        (cache / 'pair-return-evidence.json').write_text(json.dumps(evidence, indent=2))
        print(json.dumps(dict(exports=len(requests), evidence=evidence), indent=2))
        return
    raw = (cache / 'decompiled-pair-return' / (address + '.c')).read_text()
    body = clean(raw)
    assert re.search(r'target_t\s+find_repeat_legacy_effect_alt\s*\(', body)
    assert 'tVar2.card = card_00;' in body and 'tVar2.player = player_00;' in body
    caller = (cache / 'decompiled-pair-return/267b1b0.c').read_text()
    assert 'if (tVar3.card != -1)' in caller and 'return tVar3.card;' in caller
    # Rendering of the by-value return is checked after the focused decompilation.
    assert 'extraout_' not in body and 'unaff_' not in body
    timestamp = re.search(r'engine_set_timestamp\(([^,()]+),([^()]+)\);\s*engine_compact_timestamps\(\);', body)
    assert timestamp
    body = body[:timestamp.start()] + 'new_timestamp(' + timestamp[1] + ',' + timestamp[2] + ');' + body[timestamp.end():]
    command = ['C:/Program Files/LLVM/bin/clang-format.exe',
               '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
    body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
    addition = f"\n\n/* ManalinkEh 2022: 0x{address}; original functions/functions.c:{evidence['line']}. */\n" + body
    path = root / 'functions/functions.c'
    original = path.read_bytes()
    assert ('target_t ' + name + '(').encode() not in original
    candidate = cache / 'pair-return-candidate.c'
    candidate.write_bytes(original + addition.encode())
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
               '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
               '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)]
    result = subprocess.run(command, capture_output=True, text=True)
    (cache / 'pair-return-check.log').write_text(result.stderr)
    result.check_returncode()
    entry = dict(name=name, address=address, source='functions/functions.c', line=evidence['line'], static=False,
                 prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
                 recovered_literals=[], reconstruction='DWARF eight-byte target_t and verified machine sequences return EAX/EDX by value; timestamp pair uses existing source wrapper.',
                 interface_evidence=evidence)
    if args.apply:
        path.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        header = root / 'manalink.h'
        data = header.read_bytes(); at = data.rfind(b'#ifdef __cplusplus')
        header.write_bytes(data[:at] + ('\n/* Recovered by-value pair-return helper. */\n' + entry['prototype'] + '\n\n').replace('\n', '\r\n').encode() + data[at:])
        manifest['accepted_functions'].append(entry); manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
        rows = {row['address']: row for row in exports}
        for row in csv.DictReader((cache / 'decompiled-pair-return/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-pair-return' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            rows[row['address']] = row
        with (cache / 'decompiled-abi/export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=exports[0], delimiter='\t'); writer.writeheader(); writer.writerows(rows.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(path.read_text()); interfaces['repeat_effect_pair_return'] = evidence
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helper=name), indent=2))


if __name__ == '__main__':
    main()
