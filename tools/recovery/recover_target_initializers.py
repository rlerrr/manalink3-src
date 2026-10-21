"""Recover the separate 84-byte target definition and guarded protection initializer APIs."""
import argparse
import collections
import csv
import hashlib
import json
import re
import subprocess
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from inspect_release import Image, value
from export_abi import type_of, type_name
from promote_recovery import clean, strip_comments, UNRESOLVED
from audit_source import function_body


ENTRIES = {
    '265d2e0': ('get_protections_from_part_2022', '_get_protections_from.part.0', True),
    '265d550': ('get_protections_from_2022', '_get_protections_from', False),
    '26601a0': ('base_target_definition_2022', '_base_target_definition', False),
    '2660240': ('default_target_definition_2022', '_default_target_definition', False),
    '2660300': ('opponent_or_pw_target_definition_2022', '_opponent_or_pw_target_definition', False),
    '2660340': ('counterspell_target_definition_2022', '_counterspell_target_definition', False),
    '26603e0': ('counter_activated_target_definition_2022', '_counter_activated_target_definition', False),
    '2660480': ('counter_triggered_target_definition_2022', '_counter_triggered_target_definition', False),
}


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    source = 'functions/targets.c'
    debug_data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(debug_data)
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    layout = None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != source:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_typedef' and value(die, 'DW_AT_name') == 'target_definition_t':
                record = type_of(die)
                layout = dict(die=hex(record.offset), bytes=value(record, 'DW_AT_byte_size'), members=[
                    dict(name=value(m, 'DW_AT_name'), offset=value(m, 'DW_AT_data_member_location'),
                         type=type_name(type_of(m)), bytes=4)
                    for m in record.iter_children() if m.tag == 'DW_TAG_member'])
        break
    assert layout and layout['bytes'] == 84 and len(layout['members']) == 21
    assert all(m['offset'] == i * 4 and m['type'] == 'int32_t' for i, m in enumerate(layout['members']))
    assert layout['members'][-1]['name'] == 'flags'
    metadata = {r['low_pc'][2:]: r for r in csv.DictReader((cache / '2022-functions.csv').open())
                if r['low_pc'] and r['high_pc']}
    calls = {'265d550': [('265d564', 'e977fdffff', '265d2e0')],
             '2660240': [('26602e1', 'e8facfffff', '265d2e0')],
             '2660300': [('2660323', 'e818ffffff', '2660240')],
             '2660480': [('26604a2', 'e899fdffff', '2660240')]}
    flags = {'26601a0': ('266022d', 'c7405000000000'),
             '2660240': ('26602ce', 'c7435000000000'),
             '2660340': ('26603bf', 'c7405000000000'),
             '26603e0': ('266045f', 'c7405000000000')}
    proof = dict(debug_sha256=hashlib.sha256(debug_data).hexdigest(), layout=layout, functions={},
                 scope='Separate 84-byte target definition with flags at +80. Preserve the legacy 80-byte type and APIs. '
                       'The public protection getter and default initializer call the private compiler part, '
                       'not the identically named public getter. Retain their negative-source guards and the '
                       'activation-parent traversal; reuse the existing hack_gpf source cache.',
                 integration_pending='2022 target validation/availability/selection readers and Aura/generic-spell '
                                     'callers still require recovery. No legacy caller is silently migrated.')
    entries, bodies, raw_texts = [], {}, {}
    for address, (name, symbol, private) in ENTRIES.items():
        assert any(n == symbol and s == (3 if private else 2) and
                   debug.pe.OPTIONAL_HEADER.ImageBase + debug.pe.sections[sec - 1].VirtualAddress + offset == int(address, 16)
                   for n, offset, sec, kind, s in debug.functions())
        row = metadata[address]
        assert row['source'] == source
        directory = cache / ('decompiled-abi' if address == '2660480' else 'decompiled-target-core')
        raw = (directory / (address + '.c')).read_text()
        raw_texts[address] = raw
        text = strip_comments(raw)
        mapping = {metadata[a]['name']: e[0] for a, e in ENTRIES.items() if a not in {'265d2e0', '265d550'}}
        mapping['get_protections_from'] = 'get_protections_from_part_2022'
        mapping['target_definition_t'] = 'target_definition_2022_t'
        text = re.sub(r'\b(?:' + '|'.join(mapping) + r')\b', lambda m: mapping[m[0]], text)
        if address == '265d550':
            # Only the definition is public: its raw body call is the private tail destination.
            text, count = re.subn(r'\bget_protections_from_part_2022\b', name, text, count=1)
            assert count == 1
        body = clean(text)
        if private:
            body = 'static ' + body
        assert not UNRESOLVED.search(strip_comments(body))
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
                               '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
                              input=body.encode(), capture_output=True, check=True).stdout.decode().replace('\r\n', '\n')
        code = runtime.pe.get_data(int(address, 16) - 0x2000000, int(row['high_pc'], 16) - int(address, 16))
        detail = dict(machine_bytes=len(code), machine_sha256=hashlib.sha256(code).hexdigest(), calls=[], flags_store=None)
        for at, expected_hex, destination in calls.get(address, []):
            data = bytes.fromhex(expected_hex)
            assert runtime.pe.get_data(int(at, 16) - 0x2000000, len(data)) == data
            instructions = list(Cs(CS_ARCH_X86, CS_MODE_32).disasm(data, int(at, 16)))
            assert len(instructions) == 1 and instructions[0].mnemonic in {'call', 'jmp'}
            assert int(instructions[0].op_str, 16) == int(destination, 16)
            detail['calls'].append(dict(address=at, bytes=expected_hex, target=destination, name=ENTRIES[destination][0]))
        if address in flags:
            at, expected_hex = flags[address]
            data = bytes.fromhex(expected_hex)
            assert runtime.pe.get_data(int(at, 16) - 0x2000000, len(data)) == data
            assert 'td->flags = 0;' in body
            detail['flags_store'] = dict(address=at, bytes=expected_hex, offset=80, value=0)
        proof['functions'][address] = detail
        entries.append(dict(name=name, address=address, source=source, line=row['line'], static=private,
                            prototype=body[:body.index('{')].strip() + ';',
                            raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[],
                            target_initializer_evidence=detail, reconstruction=proof['scope'],
                            reconstruction_body_sha256=hashlib.sha256(function_body(body, name).encode()).hexdigest()))
        bodies[address] = body
    def read(path):
        return path.read_bytes().decode('latin1').replace('\r\n', '\n')
    definitions = read(root / 'defs.h')
    type_block = '/* October 2022 targeting layout; the original target_definition_t remains 80 bytes. */\n#ifndef SHANDALAR\ntypedef struct\n{\n'
    type_block += ''.join('  int32_t ' + m['name'] + ';\n' for m in layout['members'])
    type_block += '} target_definition_2022_t;\n#endif\n'
    assert definitions.count('} target_definition_t;') == 1 and 'target_definition_2022_t' not in definitions
    definitions = definitions.replace('} target_definition_t;', '} target_definition_t;\n\n' + type_block.rstrip())
    candidate = read(root / source)
    old_marker = '/* ManalinkEh 2022: 0x2660480; original functions/targets.c:1385. */'
    assert candidate.count(old_marker) == 1
    candidate = candidate.replace(old_marker, '/* Legacy 80-byte compatibility wrapper retained; the 84-byte 2022 implementation is below. */')
    candidate += '\n\n#ifndef SHANDALAR\n'
    for entry in entries:
        candidate += f"\n/* ManalinkEh 2022: 0x{entry['address']}; original {source}:{entry['line']}. */\n" + bodies[entry['address']] + '\n'
    candidate += '#endif\n'
    header = read(root / 'manalink.h')
    at = header.rfind('#ifdef __cplusplus')
    header_block = '/* October 2022 targeting initializers; require the separate 84-byte definition. */\n#ifndef SHANDALAR\n'
    header_block += '\n'.join(e['prototype'] for e in entries if not e['static']) + '\n#endif\n\n'
    header = header[:at] + header_block + header[at:]
    scratch = cache / 'target-initializer-check'
    scratch.mkdir(exist_ok=True)
    for filename, text in [('defs.h', definitions), ('manalink.h', header), ('targets.c', candidate)]:
        (scratch / filename).write_bytes(text.encode('latin1'))
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
                             '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
                             '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized',
                             '-Werror=format', '-I', str(scratch), '-I', str(root), str(scratch / 'targets.c')],
                            capture_output=True, text=True)
    (scratch / 'check.log').write_text(result.stderr)
    result.check_returncode()
    if args.apply:
        path = root / 'docs/recovery-evidence/recovered-functions.json'
        manifest = json.loads(path.read_text())
        accepted = {f['address']: f for f in manifest['accepted_functions']}
        assert set(ENTRIES) & set(accepted) == {'2660480'}
        # Do not overwrite exports on which an earlier function/fragment relies.
        for address in set(ENTRIES) - {'2660480'}:
            assert all(address not in json.dumps(f) for f in manifest['accepted_functions'] + manifest['source_fragments'])
        old = accepted['2660480']
        assert old['name'] == 'counter_triggered_target_definition'
        proof['retained_legacy_wrapper'] = dict(name=old['name'], source=source,
            body_sha256=hashlib.sha256(function_body(candidate, old['name']).encode()).hexdigest(),
            original_raw_sha256=old['raw_sha256'])
        for filename, text in [('defs.h', definitions), ('manalink.h', header), (source, candidate)]:
            (root / filename).write_bytes(text.replace('\n', '\r\n').encode('latin1'))
        for address in set(ENTRIES) - {'2660480'}:
            (cache / 'decompiled-abi' / (address + '.c')).write_text(raw_texts[address])
        # Preserve export ordering and names, replacing only the focused raw rows.
        export_path = cache / 'decompiled-abi/export.tsv'
        rows = list(csv.DictReader(export_path.open(), delimiter='\t'))
        focused = {r['address']: r for r in csv.DictReader((cache / 'decompiled-target-core/export.tsv').open(), delimiter='\t')}
        assert all(a in focused for a in set(ENTRIES) - {'2660480'})
        rows = [focused[r['address']] if r['address'] in ENTRIES and r['address'] != '2660480' else r for r in rows]
        with export_path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, delimiter='\t', fieldnames=list(rows[0]))
            writer.writeheader(); writer.writerows(rows)
        accepted.update({e['address']: e for e in entries})
        manifest['accepted_functions'] = list(accepted.values())
        manifest['accepted'] = len(accepted)
        manifest['source_files'] = dict(collections.Counter(f['source'] for f in accepted.values()))
        for fragment in manifest['source_fragments']:
            manifest['source_files'].setdefault(fragment['source'], 0)
        manifest['helper_functions'] = manifest['accepted'] - manifest['card_handler_functions']
        manifest['private_functions'] = sum(f['static'] for f in accepted.values())
        manifest['private_helpers'] = sum(f['static'] and not f['name'].startswith('card_') for f in accepted.values())
        manifest['shared_helper_interfaces'] = manifest['helper_functions'] - manifest['private_helpers']
        path.write_text(json.dumps(manifest, indent=2))
        path = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(path.read_text()); interfaces['target_initializers_2022'] = proof
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, new_helpers=7, corrected_existing_implementations=1,
                          legacy_target_bytes=80, target_2022_bytes=84)))


if __name__ == '__main__':
    main()
