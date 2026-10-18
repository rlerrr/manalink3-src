"""Recover incompatible Cascade, beginning-combat and other-leaves-play APIs."""
import argparse
import collections
import csv
import hashlib
import json
import shutil
import subprocess
from pathlib import Path

import pefile
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean


ENTRIES = [('22fabf0', 'cascade', 'cascade_2022', 'cards/alara_reborn.c', 3),
           ('2697910', 'beginning_of_combat', 'beginning_of_combat_2022', 'functions/functions.c', 4),
           ('26a3630', 'other_leaves_play', 'other_leaves_play_2022', 'functions/functions.c', 7)]


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    evidence = {}
    for cu in debug.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {e[3] for e in ENTRIES}:
            continue
        for die in cu.get_top_DIE().iter_children():
            entry = next((e for e in ENTRIES if value(die, 'DW_AT_low_pc') == int(e[0], 16)), None)
            if entry is None:
                continue
            assert inherited_value(die, 'DW_AT_name') == entry[1]
            parameters = [(inherited_value(p, 'DW_AT_name'), type_name(type_of(p)))
                          for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
            assert len(parameters) == entry[4]
            evidence[entry[0]] = dict(line=str(inherited_value(die, 'DW_AT_decl_line')), parameters=parameters,
                                     return_type=type_name(type_of(die)))
    assert len(evidence) == 3
    for address, original, name, source, count in ENTRIES:
        assert any(n == '_' + original and storage == 2 and debug.pe.OPTIONAL_HEADER.ImageBase +
            debug.pe.sections[s - 1].VirtualAddress + off == int(address, 16)
            for n, off, s, kind, storage in debug.functions())
    magic_bytes = (cache / '2022-Magic.exe').read_bytes()
    assert hashlib.sha256(magic_bytes).hexdigest() == '0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    magic = pefile.PE(data=magic_bytes)
    def native(at, size):
        return magic.get_data(at - magic.OPTIONAL_HEADER.ImageBase, size)
    checks = {'434be0': '558bec83ec0456', '434bf1': '8b4508',
              '434bf7': '390c8520f54e00', '434c05': '837e6cff', '434c0b': 'f6460806',
              '434c13': 'e81800000083c408', '434c1f': 'b801000000eb0233c05ec9c3'}
    for at, code in checks.items():
        assert native(int(at, 16), len(bytes.fromhex(code))) == bytes.fromhex(code)
    assert native(0x434c30, 5) == bytes.fromhex('e9fa67bd01')
    dll = pefile.PE(str(cache / '2022-ManalinkEh.dll'))
    assert dll.get_data(0x200b42f - dll.OPTIONAL_HEADER.ImageBase, 5) == bytes.fromhex('e9fc026a00')
    assert any(n == '_can_attack' and debug.pe.OPTIONAL_HEADER.ImageBase +
        debug.pe.sections[s - 1].VirtualAddress + off == 0x26ab730
        for n, off, s, kind, storage in debug.functions())
    assert '//0x434c30' in (root / 'functions/engine.c').read_text()
    evidence['2697910']['native_interface'] = dict(address='434be0', signature='int __cdecl(int player)',
        executable_sha256=hashlib.sha256(magic_bytes).hexdigest(), machine_checks=checks,
        body_sha256=hashlib.sha256(native(0x434be0, 75)).hexdigest(),
        can_attack_route=[dict(address='434c30', bytes='e9fa67bd01'),
                          dict(address='200b42f', bytes='e9fc026a00'), dict(address='26ab730', symbol='_can_attack')],
        scope='One stack parameter; iterate active cards, test native state mask 6 and the patched can_attack entry at 0x434c30; return 0/1 with plain RET.')
    log = (cache / 'batch14-interfaces-console.log').read_text()
    assert 'RECOVERY_BATCH14_INTERFACES_COMPLETE count=3' in log and 'RECOVERY_EXPORT_COMPLETE success=53 failed=0' in log and 'ERROR' not in log
    log = (cache / 'begin-combat-console.log').read_text()
    assert 'RECOVERY_RUNTIME_COMPLETE 1' in log and 'RECOVERY_EXPORT_COMPLETE success=1 failed=0' in log and 'ERROR' not in log
    additions = collections.defaultdict(str); entries = []
    for address, original, name, source, count in ENTRIES:
        directory = 'decompiled-begin-combat' if address == '2697910' else 'decompiled-batch14-interfaces'
        raw = (cache / directory / (address + '.c')).read_text()
        body = clean(raw)
        if address == '2697910':
            assert 'engine_any_legal_attacker_2022(current_turn)' in body
            body = body.replace('engine_any_legal_attacker_2022(current_turn)', 'EXE_FN(int, 0x434be0, int)(current_turn)')
        if address == '22fabf0':
            assert 'multicascade(player,card,event,1);' in body
        if address == '26a3630':
            assert 'other_leaves_play_mode' in body and 'RESOLVE_TRIGGER_MANDATORY' in body
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
            '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
            input=body.encode(), capture_output=True, check=True).stdout.decode()
        line = evidence[address]['line']
        additions[source] += f'\n\n/* ManalinkEh 2022: 0x{address}; original {source}:{line}. */\n' + body
        entries.append(dict(name=name, original_name=original, address=address, source=source, line=line, static=False,
            prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
            recovered_literals=[], reconstruction='Recover the target body under a distinct API name; retain the existing source interface. Native combat call is bound to the verified release-executable signature where applicable.',
            interface_evidence=evidence[address]))
    for source, addition in additions.items():
        assert all((e['name'] + '(').encode() not in (root / source).read_bytes() for e in entries if e['source'] == source)
        candidate = cache / ('batch14-interface-' + Path(source).name)
        candidate.write_bytes((root / source).read_bytes() + addition.encode())
        result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
            '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
            '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(root), str(candidate)],
            capture_output=True, text=True)
        (cache / (candidate.stem + '.log')).write_text(result.stderr); result.check_returncode()
    if args.apply:
        path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(path.read_text())
        accepted = {f['address'] for f in manifest['accepted_functions']}
        assert not accepted & {e['address'] for e in entries}
        for source, addition in additions.items():
            path_source = root / source
            path_source.write_bytes(path_source.read_bytes().rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        path_header = root / 'manalink.h'; header = path_header.read_bytes(); at = header.rfind(b'#ifdef __cplusplus')
        block = '/* October 2022 interfaces with distinct source-compatible names. */\n' + '\n'.join(e['prototype'] for e in entries) + '\n\n'
        path_header.write_bytes(header[:at] + block.replace('\n', '\r\n').encode() + header[at:])
        manifest['accepted_functions'].extend(entries); manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_text(json.dumps(manifest, indent=2))
        exports_path = cache / 'decompiled-abi/export.tsv'; rows = list(csv.DictReader(exports_path.open(), delimiter='\t')); by_address = {r['address']: r for r in rows}
        for row in csv.DictReader((cache / 'decompiled-batch14-interfaces/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in accepted
            directory = 'decompiled-begin-combat' if row['address'] == '2697910' else 'decompiled-batch14-interfaces'
            shutil.copyfile(cache / directory / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            by_address[row['address']] = row
        with exports_path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t'); writer.writeheader(); writer.writerows(by_address.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
        interfaces['batch14_compatibility_interfaces'] = evidence; path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helpers=[e['name'] for e in entries], refreshed_exports=53), indent=2))


if __name__ == '__main__':
    main()
