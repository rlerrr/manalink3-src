"""Recover Yorion, its five-argument ownership predicate and exact two-row zero fill."""
import argparse
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path
from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED
from audit_source import function_body


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    directory = cache / 'decompiled-yorion'
    source = 'cards/ikoria.c'
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    shape, parameters = None, None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != source:
            continue
        def walk(die):
            nonlocal shape
            for child in die.iter_children():
                if child.tag == 'DW_TAG_variable' and inherited_value(child, 'DW_AT_name') == 'flicker_array':
                    array = type_of(child)
                    dimensions = [value(c, 'DW_AT_upper_bound') + 1 for c in array.iter_children() if c.tag == 'DW_TAG_subrange_type']
                    assert type_name(array) == 'char[2][151]' and dimensions == [2, 151]
                    shape = dict(die=hex(child.offset), dimensions=dimensions, bytes=302, type='char[2][151]')
                walk(child)
        for die in cu.get_top_DIE().iter_children():
            if value(die, 'DW_AT_low_pc') == 0x228cd10:
                walk(die)
            if value(die, 'DW_AT_low_pc') == 0x22887d0:
                parameters = [(inherited_value(p, 'DW_AT_name'), type_name(type_of(p)), value(p, 'DW_AT_location')) for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
        break
    assert shape and parameters == [('who_chooses','int',[145,0]), ('player','int',[145,4]), ('card','int',[145,8]), ('targeting_player','int',[145,12]), ('targeting_card','int',[145,16])]
    assert any(n == '_permanent_you_control_and_own' and storage == 3 and debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[s - 1].VirtualAddress + off == 0x22887d0 for n, off, s, kind, storage in debug.functions())
    target_source = (root / 'functions/targets.c').read_text()
    assert 'typedef const char* (*Predicate)(int, int, int, int, int);' in target_source
    assert 'predicate(who_chooses, tgt_player, tgt_card, p, c)' in target_source
    rules = (root / 'functions/rules_engine.c').read_bytes().decode('latin1').replace('\r\n', '\n')
    mode_block = re.search(r'typedef enum \{\n\tSPM_NONE = 0,.*?\} singleplayer_mode_t;\nextern singleplayer_mode_t singleplayer_mode;\n', rules, re.S)
    assert mode_block
    mode_members = {'SPM_NONE':0, 'SPM_MOMIR':1, 'SPM_DRAFT':2, 'SPM_TESTING':3}
    mode_enum = None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/rules_engine.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_enumeration_type':
                members = {value(c, 'DW_AT_name'):value(c, 'DW_AT_const_value') for c in die.iter_children()}
                if 'SPM_NONE' in members:
                    assert members == mode_members and value(die, 'DW_AT_byte_size') == 4
                    mode_enum = dict(die=hex(die.offset), bytes=4, members=members)
        break
    assert mode_enum and '0x4e95a8\tsingleplayer_mode\tglobal\tsingleplayer_mode_t' in (cache / 'absolute-interfaces.tsv').read_text()
    assert re.search(r'_singleplayer_mode\s*=\s*0x4e95a8;', (root / 'manalink.lds').read_text(), re.I)
    moved = mode_block[0]
    rules = rules[:mode_block.start()] + rules[mode_block.end():]
    defs = (root / 'defs.h').read_bytes().decode('latin1').replace('\r\n', '\n')
    at = defs.rfind('#endif')
    defs = defs[:at] + '\n/* Shared native singleplayer mode type; moved unchanged from rules_engine.c. */\n' + moved.split('extern ')[0] + '\n' + defs[at:]
    header = (root / 'manalink.h').read_bytes().decode('latin1').replace('\r\n', '\n')
    at = header.rfind('#ifdef __cplusplus')
    header = header[:at] + '/* Shared native singleplayer-mode import. */\nextern singleplayer_mode_t singleplayer_mode;\n\n' + header[at:]
    checks = {'228cdfb':'c78598feffffd0872802', '228cdd8':'8dbdc6feffff',
              '228ce19':'ba2e010000', '228ce1e':'a802', '228ce20':'0f85fa000000',
              '228ce26':'89d1', '228ce28':'31c0', '228ce2a':'c1e902', '228ce2d':'83e202',
              '228ce30':'f3ab', '228ce32':'7405', '228ce34':'66c7070000',
              '228cf20':'66c785c6feffff0000', '228cf29':'8dbdc8feffff',
              '228cf2f':'ba2c010000', '228cf34':'e9edfeffff',
              '22887e3':'e8189e4000', '22887e8':'3b4514', '22887ed':'b829a17302',
              '228ced0':'833da8954e0002'}
    for at, expected_hex in checks.items():
        expected_bytes = bytes.fromhex(expected_hex)
        assert runtime.pe.get_data(int(at, 16) - 0x2000000, len(expected_bytes)) == expected_bytes
    metadata = {r['low_pc'][2:]: r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    entries, bodies = [], {}
    proof = dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(), shape=shape, predicate_parameters=parameters, machine_checks=checks, functions={}, singleplayer_interface=dict(enum=mode_enum, moved_original=moved, native_address='4e95a8', scope='Move the existing enum and native extern into shared headers without changing their values, widths or address.'), scope='Five-argument private ownership predicate, compatible with the retained target callback dispatcher. Replace only the native complete 302-byte alignment-aware zero fill with memset; retain selection, exile and companion deck-size paths.')
    for address in ('22887d0', '228cd10'):
        row = metadata[address]
        raw = (directory / (address + '.c')).read_text()
        original = strip_comments(raw)
        detail = {}
        if address == '228cd10':
            assert original.count('_singleplayer_mode') == 1
            original = original.replace('_singleplayer_mode', 'singleplayer_mode')
            start = original.index('      uVar3 = 0x12e;')
            end = original.index('      iVar1 = mark_up_to_n_targets_noload', start)
            block = original[start:end]
            assert block.count("'\\0'") == 8
            replacement = '      memset(flicker_array, 0, sizeof(flicker_array));\n'
            original = original[:start] + replacement + original[end:]
            detail['zero_fill'] = dict(original=block, replacement=replacement, original_sha256=hashlib.sha256(block.encode()).hexdigest(), bytes=302)
            for declaration in ('uint uVar3;', 'char *pcVar4;', 'bool bVar5;', 'byte bVar6;', 'bVar6 = 0;', 'pcVar4 = flicker_array[0];'):
                assert original.count(declaration) == 1
                original = original.replace(declaration, '', 1)
        body = clean(original, use_mana_api_2022=True)
        private = address == '22887d0'
        if private:
            assert body.lstrip().startswith('char * permanent_you_control_and_own')
            body = 'static const ' + body
            body = body.replace('(char *)0x0', '(const char *)0x0')
        assert not UNRESOLVED.search(strip_comments(body))
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe', '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'], input=body.encode(), capture_output=True, check=True).stdout.decode().replace('\r\n', '\n')
        code = runtime.pe.get_data(int(address, 16) - 0x2000000, int(row['high_pc'], 16) - int(address, 16))
        detail.update(machine_bytes=len(code), machine_sha256=hashlib.sha256(code).hexdigest())
        proof['functions'][address] = detail
        prototype = body[:body.index('{')].strip() + ';'
        entries.append(dict(name=row['name'], address=address, source=source, line=row['line'], static=private, prototype=prototype, raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[], reconstruction=proof['scope'], yorion_evidence=detail, reconstruction_body_sha256=hashlib.sha256(function_body(body, row['name']).encode()).hexdigest()))
        bodies[address] = body
    candidate = (root / source).read_bytes().decode('latin1').replace('\r\n', '\n')
    for entry in entries:
        candidate += f"\n\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n" + bodies[entry['address']]
    scratch = cache / 'yorion-check'
    scratch.mkdir(exist_ok=True)
    (scratch / 'defs.h').write_bytes(defs.encode('latin1'))
    (scratch / 'manalink.h').write_bytes(header.encode('latin1'))
    (scratch / 'ikoria.c').write_bytes(candidate.encode('latin1'))
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(scratch), '-I', str(root), str(scratch / 'ikoria.c')], capture_output=True, text=True)
    (scratch / 'check.log').write_text(result.stderr)
    result.check_returncode()
    (scratch / 'rules_engine.c').write_bytes(rules.encode('latin1'))
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only', '-Dstrncasecmp=_strnicmp', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-I', str(scratch), '-I', str(root), str(scratch / 'rules_engine.c')], capture_output=True, text=True)
    (scratch / 'rules-check.log').write_text(result.stderr)
    result.check_returncode()
    if args.apply:
        p = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(p.read_text())
        assert not set(bodies) & {f['address'] for f in manifest['accepted_functions']}
        (root / source).write_bytes(candidate.replace('\n', '\r\n').encode('latin1'))
        (root / 'defs.h').write_bytes(defs.replace('\n', '\r\n').encode('latin1'))
        (root / 'manalink.h').write_bytes(header.replace('\n', '\r\n').encode('latin1'))
        (root / 'functions/rules_engine.c').write_bytes(rules.replace('\n', '\r\n').encode('latin1'))
        manifest['accepted_functions'].extend(entries)
        manifest['accepted'] = len(manifest['accepted_functions'])
        p.write_text(json.dumps(manifest, indent=2))
        p = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(p.read_text())
        interfaces['yorion_2022'] = proof
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
    print(json.dumps(dict(applied=args.apply, card_functions=1, private_helpers=1, zero_fill_bytes=302)))


if __name__ == '__main__':
    main()
