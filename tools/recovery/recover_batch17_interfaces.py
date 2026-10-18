"""Recover counter copies and the generalized dies-trigger core, preserving older APIs."""
import argparse
import collections
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, code_only, UNRESOLVED


ENTRIES = [
    ('26d4ca0', 'copy_counters', 'copy_counters_from_source', 'functions/counters.c', 7, 2),
    ('26d5970', 'add_counters_predoubled', 'add_counters_predoubled_from_source', 'functions/counters.c', 6, 2),
    ('26d5a00', 'move_counters', 'move_counters_from_source', 'functions/counters.c', 8, 2),
    ('269b2e0', 'lobotomy_effect', 'lobotomy_effect_2022', 'functions/functions.c', 5, 2),
    ('2679600', 'effect_this_dies', 'effect_this_dies_2022', 'functions/functions.c', 3, 2),
    ('2686130', 'this_dies_trigger_impl', 'this_dies_trigger_impl_2022', 'functions/functions.c', 7, 3),
]


def descendants(die):
    for child in die.iter_children():
        yield child
        yield from descendants(child)


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--prepare', action='store_true')
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    evidence = {}; record = None; flag = None
    for cu in debug.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {'functions/functions.c', 'functions/counters.c'}:
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_enumeration_type':
                for e in die.iter_children():
                    if value(e, 'DW_AT_name') == 'SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE':
                        assert value(e, 'DW_AT_const_value') == 0x10000000
                        flag = value(e, 'DW_AT_const_value')
            at = value(die, 'DW_AT_low_pc')
            entry = next((e for e in ENTRIES if int(e[0], 16) == at), None)
            if entry is None:
                continue
            assert source == entry[3] and inherited_value(die, 'DW_AT_name') == entry[1]
            parameters = [(inherited_value(p, 'DW_AT_name'), type_name(type_of(p)))
                          for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
            assert len(parameters) == entry[4]
            evidence[entry[0]] = dict(line=str(inherited_value(die, 'DW_AT_decl_line')),
                parameters=parameters, bytes=value(die, 'DW_AT_high_pc'))
            if entry[0] == '26d4ca0':
                local = next(d for d in descendants(die) if d.tag == 'DW_TAG_variable' and
                             inherited_value(d, 'DW_AT_name') == 'counters')
                array = type_of(local); struct = type_of(array)
                assert array.tag == 'DW_TAG_array_type' and value(next(array.iter_children()), 'DW_AT_upper_bound') == 8
                members = {value(m, 'DW_AT_name'): (type_name(type_of(m)), value(m, 'DW_AT_data_member_location'))
                           for m in struct.iter_children()}
                assert value(struct, 'DW_AT_byte_size') == 8 and members == {'typ': ('counter_t', 0), 'num': ('int', 4)}
                record = dict(elements=9, element_bytes=8, members=members, die=hex(struct.offset))
    assert len(evidence) == 6 and record is not None and flag == 0x10000000
    for address, original, name, source, count, linkage in ENTRIES:
        assert any(n == '_' + original and storage == linkage and debug.pe.OPTIONAL_HEADER.ImageBase +
                   debug.pe.sections[s - 1].VirtualAddress + off == int(address, 16)
                   for n, off, s, kind, storage in debug.functions())
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text()); accepted = {f['address'] for f in manifest['accepted_functions']}
    assert not accepted & {e[0] for e in ENTRIES}
    if args.prepare:
        requests = {}; names = {e[1] for e in ENTRIES} | {'add_or_remove_counters_impl'}
        for row in csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'):
            if row['address'] in accepted:
                continue
            raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
            if names & set(re.findall(r'\b[A-Za-z_]\w*\b', code_only(raw))):
                requests[row['address']] = row
        functions = {r['low_pc'].removeprefix('0x'): r for r in csv.DictReader((cache / '2022-functions.csv').open()) if r['low_pc']}
        for address, original, name, source, count, linkage in ENTRIES:
            requests[address] = dict(address=address, source=source, name=original)
        lines = ['\t'.join([at, row['source'], row['name'], functions[at]['high_pc'].removeprefix('0x')])
                 for at, row in requests.items()]
        (cache / 'export-request-batch17-interfaces.tsv').write_text('\n'.join(lines) + '\n')
        print(json.dumps(dict(prepared=len(lines)), indent=2))
        return
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())

    def read(at, length):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + length]

    checks = {'2679647': 'f60485a7f14e0002', '268620d': '8b402cf680bb00000010',
              '26862d4': 'b898000000', '26862f5': 'f3a5'}
    for at, expected in checks.items():
        assert read(int(at, 16), len(bytes.fromhex(expected))) == bytes.fromhex(expected)
    log = (cache / 'batch17-interfaces-console.log').read_text()
    expected_exports = len((cache / 'export-request-batch17-interfaces.tsv').read_text().splitlines())
    assert 'RECOVERY_BATCH17_INTERFACES_COMPLETE count=7' in log
    assert f'RECOVERY_EXPORT_COMPLETE success={expected_exports} failed=0' in log and 'ERROR' not in log
    additions = collections.defaultdict(str); new = []
    chosen = {'26d4ca0', '2679600', '2686130'}
    for address, original, name, source, count, linkage in ENTRIES:
        if address not in chosen:
            continue
        raw = (cache / 'decompiled-batch17-interfaces' / (address + '.c')).read_text()
        body = raw
        if address == '26d4ca0':
            typename = 'anon_struct_8_2_43e80440'
            assert body.count(typename) == 3
            body = body.replace(typename, 'counter_copy_entry_2022')
            body = clean(body)
            at = body.index('{') + 1
            body = body[:at] + '\n  typedef struct { counter_t typ; int num; } counter_copy_entry_2022;\n' + body[at:]
            scope = 'Restore the nine-element local counter snapshot and retain source, recipient and donor arguments. Calls use the already recovered source-aware placement core.'
        elif address == '2686130':
            old = '(((pcVar2->field_21).ext)->special_flags2 & SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE)'
            assert body.count(old) == 1
            body = body.replace(old, '(get_special_flags2_by_instance(pcVar2) & SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE)')
            body = body.replace('!= SF2_0', '!= 0')
            start = body.index('      ptVar7 = pcVar1->targets;')
            end = body.index('      pcVar1->number_of_targets =', start)
            assert 'uVar4 = 0x98;' in body[start:end] and 'for (uVar5' in body[start:end]
            body = body[:start] + '      memcpy(pcVar1->targets, pcStack_18->targets, sizeof(pcVar1->targets));\n' + body[end:]
            body = 'static ' + clean(body, ['trigger_condition'])
            scope = 'Restore the 152-byte target-array copy. Read the verified exile flag through the existing flag accessor, without projecting the extension pointer onto old storage. Retain the seven-argument kill-code selector and trigger duplication.'
        else:
            body = clean(body, ['trigger_condition'])
            assert '((unsigned int)player_bits[player] >> 24 & 0xff)' in body
            scope = 'Recover the target callback under a distinct name, using the verified player_bits byte read and fixed trigger-mode byte. Existing callback and callers remain intact.'
        assert not UNRESOLVED.search(code_only(body)) and 'field_21' not in body and not re.search(r'\._\d+_\d+_', body)
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
            '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
            input=body.encode(), capture_output=True, check=True).stdout.decode()
        line = evidence[address]['line']; additions[source] += f'\n\n/* ManalinkEh 2022: 0x{address}; original {source}:{line}. */\n' + body
        new.append(dict(name=name, original_name=original, address=address, source=source, line=line,
            static=linkage == 3, prototype=body[:body.index('{')].strip() + ';',
            raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[], reconstruction=scope,
            interface_evidence=dict(evidence[address], machine_checks=checks, counter_snapshot=record if address == '26d4ca0' else None)))
    originals = {s: (root / s).read_bytes() for s in additions}
    for source, addition in additions.items():
        prototypes = '\n'.join(f['prototype'] for f in new if f['source'] == source) + '\n'
        candidate = cache / ('batch17-' + Path(source).name)
        candidate.write_bytes(originals[source].rstrip() + ('\n' + prototypes + addition).encode() +
            b'\n_Static_assert(sizeof(((card_instance_t*)0)->targets) == 152, "target array size");\n_Static_assert(SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE == 0x10000000, "exile flag");\n')
        result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc',
            '-std=gnu99', '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
            '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format',
            '-I', str(root), str(candidate)], capture_output=True, text=True)
        (cache / ('batch17-' + Path(source).stem + '-check.log')).write_text(result.stderr); result.check_returncode()
    if args.apply:
        for source, addition in additions.items():
            prototypes = '\n'.join(f['prototype'] for f in new if f['source'] == source) + '\n'
            (root / source).write_bytes(originals[source].rstrip() + ('\n\n' + prototypes + addition).replace('\r\n', '\n').replace('\n', '\r\n').encode())
        path = root / 'manalink.h'; header = path.read_bytes(); at = header.rfind(b'#ifdef __cplusplus')
        block = '/* October 2022 counter-copy and dies callback interfaces. */\n' + '\n'.join(f['prototype'] for f in new if not f['static']) + '\n\n'
        path.write_bytes(header[:at] + block.replace('\n', '\r\n').encode() + header[at:])
        manifest['accepted_functions'].extend(new); manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
        path = cache / 'decompiled-abi/export.tsv'; rows = list(csv.DictReader(path.open(), delimiter='\t')); by_address = {r['address']: r for r in rows}
        for row in csv.DictReader((cache / 'decompiled-batch17-interfaces/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-batch17-interfaces' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            by_address[row['address']] = row
        with path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t'); writer.writeheader(); writer.writerows(by_address.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
        interfaces['batch17_interfaces'] = dict(functions=evidence, machine_checks=checks, counter_snapshot=record,
            scope='Changed target APIs coexist with old source bodies. Three full helpers reconstructed; remaining exports pass normal promotion checks. Engine integration remains deferred.')
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, recovered_helpers=[f['name'] for f in new]), indent=2))


if __name__ == '__main__':
    main()
