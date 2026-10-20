"""Recover the 2022 mana-color chooser with the native dialog kept out of its body."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import struct
import subprocess
from pathlib import Path
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, UNRESOLVED, strip_comments
from audit_source import function_body


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    address = '266ae20'
    source = 'functions/produce_mana.c'
    directory = cache / 'decompiled-mana-chooser'
    raw = (directory / (address + '.c')).read_text()
    assert raw.count('choose_a_color_exe(player,prompt,1,iVar4,available)') == 1
    assert 'receive_network_packet' not in raw and 'charging[5] = available' not in raw
    metadata = next(r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc'] == '0x' + address)
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    executable = Image((cache / '2022-Magic.exe').read_bytes())
    length = int(metadata['high_pc'], 16) - int(address, 16)
    machine = runtime.pe.get_data(int(address, 16) - runtime.pe.OPTIONAL_HEADER.ImageBase, length)
    # Stores to the five outgoing caller slots, epilogue, then canonical E9.
    tail_at = 0x266afdc
    tail_hex = '8b45d48955148945188b45c889450c8b45d0c745100100000089450883c4345b5e5f5de9bc71e0fd'
    tail = bytes.fromhex(tail_hex)
    assert runtime.pe.get_data(tail_at - 0x2000000, len(tail)) == tail
    assert 0x266afff + 5 + struct.unpack('<i', tail[-4:])[0] == 0x4721c0
    native = executable.pe.get_data(0x4721c0 - executable.pe.OPTIONAL_HEADER.ImageBase, 0x98)
    assert native[:9].hex() == '558bec83ec04535657' and native[-5:].hex() == '5f5e5bc9c3'
    assert re.search(r'_choose_a_color_exe\s*=\s*0x4721c0;', (root / 'manalink.lds').read_text(), re.I)
    assert 'int choose_a_color_exe(int player, const char* prompt, int use_color_names_instead_of_land, int ai_choice, int color_tests);' in (root / 'manalink.h').read_text()
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    parameters = None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != source:
            continue
        for die in cu.get_top_DIE().iter_children():
            if value(die, 'DW_AT_low_pc') == int(address, 16):
                parameters = [(inherited_value(p, 'DW_AT_name'), type_name(type_of(p))) for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
        break
    assert parameters == [('player', 'int'), ('available', 'color_test_t'), ('num_mana', 'int'), ('prompt', 'char *'), ('default_color', 'color_test_t')]
    assert any(n == '_choose_mana_all_one_color' and storage == 2 and debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[s - 1].VirtualAddress + off == int(address, 16) for n, off, s, kind, storage in debug.functions())
    body = clean(raw, use_mana_api_2022=True).replace('char *prompt', 'const char *prompt')
    assert 'choose_mana_all_one_color_2022' in body and not UNRESOLVED.search(strip_comments(body))
    body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe', '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'], input=body.encode(), capture_output=True, check=True).stdout.decode().replace('\r\n', '\n')
    prototype = body[:body.index('{')].strip() + ';'
    original = (root / source).read_bytes().decode('latin1').replace('\r\n', '\n')
    candidate = original + f"\n\n/* ManalinkEh 2022: 0x{address}; original {source}:{metadata['line']}. */\n" + body
    header = (root / 'manalink.h').read_bytes().decode('latin1').replace('\r\n', '\n')
    at = header.rfind('#ifdef __cplusplus')
    header = header[:at] + '/* October 2022 mana-color chooser. */\n#ifndef SHANDALAR\n' + prototype + '\n#endif\n\n' + header[at:]
    scratch = cache / 'mana-chooser-check'
    scratch.mkdir(exist_ok=True)
    (scratch / 'manalink.h').write_bytes(header.encode('latin1'))
    (scratch / 'produce_mana.c').write_bytes(candidate.encode('latin1'))
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(scratch), '-I', str(root), str(scratch / 'produce_mana.c')], capture_output=True, text=True)
    (scratch / 'check.log').write_text(result.stderr)
    result.check_returncode()
    proof = dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(), parameters=parameters, machine_bytes=length, machine_sha256=hashlib.sha256(machine).hexdigest(), tail=dict(address=f'{tail_at:x}', bytes=tail_hex, jump_address='266afff', target='4721c0'), native_dialog=dict(address='4721c0', bytes=len(native), machine_sha256=hashlib.sha256(native).hexdigest(), executable_sha256=hashlib.sha256(executable.data).hexdigest()), scope='Preserve the complete chooser algorithm and five outgoing dialog arguments. Native UI, speculation and network behavior stay behind the existing choose_a_color_exe import. Read-only borrowed prompt; original static 2020 chooser remains intact.')
    if args.apply:
        p = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(p.read_text())
        assert address not in {f['address'] for f in manifest['accepted_functions']}
        (root / source).write_bytes(candidate.replace('\n', '\r\n').encode('latin1'))
        (root / 'manalink.h').write_bytes(header.replace('\n', '\r\n').encode('latin1'))
        manifest['accepted_functions'].append(dict(name='choose_mana_all_one_color_2022', original_name='choose_mana_all_one_color', address=address, source=source, line=metadata['line'], static=False, prototype=prototype, raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[], reconstruction=proof['scope'], mana_chooser_evidence=proof, reconstruction_body_sha256=hashlib.sha256(function_body(body, 'choose_mana_all_one_color_2022').encode()).hexdigest()))
        manifest['accepted'] = len(manifest['accepted_functions'])
        p.write_text(json.dumps(manifest, indent=2))
        p = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(p.read_text())
        interfaces['mana_chooser_2022'] = proof
        p.write_text(json.dumps(interfaces, indent=2))
        canonical_path = cache / 'decompiled-abi/export.tsv'
        rows = list(csv.DictReader(canonical_path.open(), delimiter='\t'))
        by_address = {r['address']: r for r in rows}
        for row in csv.DictReader((directory / 'export.tsv').open(), delimiter='\t'):
            assert row['status'] == 'ok'
            if row['address'] in by_address:
                by_address[row['address']].update(row)
            else:
                rows.append(row)
            shutil.copyfile(directory / (row['address'] + '.c'), canonical_path.parent / (row['address'] + '.c'))
        with canonical_path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t')
            writer.writeheader()
            writer.writerows(rows)
    print(json.dumps(dict(applied=args.apply, new_helpers=1, exact_native_tail_call='4721c0')))


if __name__ == '__main__':
    main()
