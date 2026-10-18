"""Recover the five-argument leaves-play dispatcher under a compatible new name."""
import argparse
import csv
import hashlib
import json
import shutil
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
    evidence = None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/functions.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if value(die, 'DW_AT_low_pc') != 0x26a2080:
                continue
            assert inherited_value(die, 'DW_AT_name') == 'leaves_play'
            parameters = [(inherited_value(p, 'DW_AT_name'), type_name(type_of(p)))
                          for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
            assert parameters == [('player', 'int'), ('card', 'int'), ('event', 'event_t'),
                                  ('can_be_suppressed', 'int'), ('can_be_duplicated', 'int')]
            assert value(die, 'DW_AT_high_pc') == 686
            evidence = dict(line=str(inherited_value(die, 'DW_AT_decl_line')), parameters=parameters,
                scope='Full five-argument target body uses the recovered card-count cache. Preserve the existing three-argument source leaves_play API and body.')
        break
    assert evidence
    assert any(n == '_leaves_play' and storage == 2 and debug.pe.OPTIONAL_HEADER.ImageBase +
        debug.pe.sections[s - 1].VirtualAddress + off == 0x26a2080 for n, off, s, kind, storage in debug.functions())
    log = (cache / 'leaves-play-console.log').read_text()
    assert 'RECOVERY_LEAVES_PLAY_INTERFACE_COMPLETE' in log and 'RECOVERY_EXPORT_COMPLETE success=49 failed=0' in log and 'ERROR' not in log
    raw = (cache / 'decompiled-leaves-play/26a2080.c').read_text()
    body = clean(raw, ['trigger_condition'])
    assert 'card_counts[player][5] + 1' in body and 'is_humiliated_for_ability(' in body
    assert '0x4ef1a7' not in body and '(unsigned int)player_bits[player] >> 24 & 0xff' in body
    body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
        '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
        input=body.encode(), capture_output=True, check=True).stdout.decode()
    source = root / 'functions/functions.c'; original = source.read_bytes()
    assert b'int leaves_play_2022(' not in original
    addition = f"\n\n/* ManalinkEh 2022: 0x26a2080; original functions/functions.c:{evidence['line']}. */\n" + body
    candidate = cache / 'leaves-play-candidate.c'; candidate.write_bytes(original + addition.encode())
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
        '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
        '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(root), str(candidate)],
        capture_output=True, text=True)
    (cache / 'leaves-play-check.log').write_text(result.stderr); result.check_returncode()
    entry = dict(name='leaves_play_2022', original_name='leaves_play', address='26a2080', source='functions/functions.c',
        line=evidence['line'], static=False, prototype=body[:body.index('{')].strip() + ';',
        raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[], reconstruction=evidence['scope'], interface_evidence=evidence)
    if args.apply:
        path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(path.read_text())
        accepted = {f['address'] for f in manifest['accepted_functions']}; assert entry['address'] not in accepted
        source.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        path_header = root / 'manalink.h'; header = path_header.read_bytes(); at = header.rfind(b'#ifdef __cplusplus')
        path_header.write_bytes(header[:at] + ('/* October 2022 five-argument leaves-play dispatcher. */\n' + entry['prototype'] + '\n\n').replace('\n', '\r\n').encode() + header[at:])
        manifest['accepted_functions'].append(entry); manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_text(json.dumps(manifest, indent=2))
        exports_path = cache / 'decompiled-abi/export.tsv'; rows = list(csv.DictReader(exports_path.open(), delimiter='\t')); by_address = {r['address']: r for r in rows}
        for row in csv.DictReader((cache / 'decompiled-leaves-play/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-leaves-play' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            by_address[row['address']] = row
        with exports_path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t'); writer.writeheader(); writer.writerows(by_address.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
        interfaces['leaves_play_five_argument'] = evidence; path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helper=entry['name'], refreshed_exports=49), indent=2))


if __name__ == '__main__':
    main()
