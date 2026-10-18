"""Recover player-aware subtype APIs with explicit target bounds and table strides."""
import argparse
import collections
import csv
import hashlib
import json
import shutil
import subprocess
from pathlib import Path

from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from promote_recovery import clean, UNRESOLVED, code_only


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    evidence = {}; layouts = {}
    addresses = {'2673320': ('get_subtypes_by_csvid', 'get_subtypes_by_csvid_2022'),
                 '26736e0': ('has_subtype_by_id', 'has_subtype_by_id_2022')}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'functions/has_creature_type.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_typedef' and value(die, 'DW_AT_name') == 'reported_subtypes_t':
                record = type_of(die)
                assert value(record, 'DW_AT_byte_size') == 104
                member = next(record.iter_children())
                assert value(member, 'DW_AT_data_member_location') == 0 and type_name(type_of(member)) == 'unsigned int[26]'
                layouts['reported_slots'] = 26
            at = value(die, 'DW_AT_low_pc')
            if at not in {int(a, 16) for a in addresses}:
                continue
            address = f'{at:x}'; assert inherited_value(die, 'DW_AT_name') == addresses[address][0]
            parameters = [(inherited_value(p, 'DW_AT_name'), type_name(type_of(p)))
                          for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
            assert len(parameters) == 3 and parameters[0] == ('player', 'int')
            evidence[address] = dict(line=str(inherited_value(die, 'DW_AT_decl_line')), parameters=parameters,
                                     size=value(die, 'DW_AT_high_pc'))
        break
    assert layouts['reported_slots'] == 26 and len(evidence) == 2
    for address, (original, name) in addresses.items():
        assert any(n == '_' + original and storage == 2 and debug.pe.OPTIONAL_HEADER.ImageBase +
            debug.pe.sections[s - 1].VirtualAddress + off == int(address, 16)
            for n, off, s, kind, storage in debug.functions())
    def read(at, length):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + length]
    checks = {
        '267332e': '8d7768',                          # 104-byte / 26-element end pointer
        '2673358': '0fbf845a84000000',                # signed int16 natural subtypes @ 132
        '2673364': '83fb07',                          # seven natural slots
        '26733b2': 'c1e0078d5078',                    # row stride 128; 15 records * 8
        '26733ce': '8b9020707902',                    # modifier subtype
        '26733d7': '81f9fe0f0000',                    # unsigned subtype-1 <= 4094
        '26733df': '80b82470790202',                  # int8 mode at +4
        '2673773': '8d758c',                          # local array begins EBP-116
        '2673794': '8d4df4',                          # clear end EBP-12 => 26 slots
        '26737a0': '8b9cd7f86f7902',                  # index 5 -> modifier record 0
        '26737b1': '80bcd7fc6f790202',                # byte mode, same indexing
        '26737d8': '83fa14',                          # process indices 5..19
        '26737e0': '8d55dc',                          # scan end EBP-36 => first 20 slots
        '26738a1': '39c1',
    }
    for at, code in checks.items():
        assert read(int(at, 16), len(bytes.fromhex(code))) == bytes.fromhex(code)
    log = (cache / 'subtype-interfaces-console.log').read_text()
    assert 'RECOVERY_SUBTYPE_INTERFACES_COMPLETE count=2' in log and 'RECOVERY_EXPORT_COMPLETE success=17 failed=0' in log and 'ERROR' not in log
    body0 = '''void get_subtypes_by_csvid_2022(int player, int csvid, reported_subtypes_t* my_subtypes)
{
  for (int i = 0; i < 26; ++i)
    my_subtypes->reported_subtypes[i] = 0;
  card_ptr_t* cp = cards_ptr[csvid];
  for (int i = 0; i < 7; ++i)
    my_subtypes->reported_subtypes[i] = (int)cp->types[i];
  int count = 7;
  if (player >= 0 && is_what(-1, get_internal_card_id_from_csv_id(csvid), TYPE_CREATURE))
    for (int i = 0; i < 15; ++i)
    {
      unsigned int subtype = added_subtypes_not_in_play[player][i].subtype;
      if (subtype - 1u > 0xffeU)
        break;
      int slot = count;
      if (added_subtypes_not_in_play[player][i].added_forced == '\\x02')
      {
        for (int j = 0; j < 26; ++j)
          my_subtypes->reported_subtypes[j] = 0;
        slot = 0;
        count = 1;
      }
      else
        ++count;
      my_subtypes->reported_subtypes[slot] = subtype;
    }
  if (csvid == 0x5a13)
    my_subtypes->reported_subtypes[count] = 0x61;
}
'''
    raw1 = (cache / 'decompiled-subtype-interfaces/26736e0.c').read_text(); body1 = clean(raw1)
    assert body1.count('0xffe < subtype - SUBTYPE_ALLY') == 2
    body1 = body1.replace('0xffe < subtype - SUBTYPE_ALLY', '0xffeU < (unsigned int)subtype - SUBTYPE_ALLY')
    replacements = {
        '*(unsigned int *)(player * 0x80 + 0x2796ff8 + iVar2 * 8)': 'added_subtypes_not_in_play[player][iVar2 - 5].subtype',
        '*(char *)(player * 0x80 + 0x2796ffc + iVar2 * 8)': 'added_subtypes_not_in_play[player][iVar2 - 5].added_forced',
        '(unsigned int *)&stack0xfffffff0 != puVar4': 'stypes.reported_subtypes + 26 != puVar4',
    }
    for original, replacement in replacements.items():
        assert original in body1, original
        body1 = body1.replace(original, replacement)
    assert not UNRESOLVED.search(code_only(body1)) and 'stack0x' not in body1 and '0x2796' not in body1
    additions = ''; entries = []
    for (address, (original, name)), body in zip(addresses.items(), [body0, body1]):
        raw = (cache / 'decompiled-subtype-interfaces' / (address + '.c')).read_text()
        evidence[address].update(machine_checks=checks, target_reported_slots=26, source_reported_slots=100,
            scope='Operate on the first 26 entries of the larger existing source buffer. The ID predicate deliberately repeats the modifier pass and scans only the first 20 entries, matching machine bounds.',
            machine_sha256=hashlib.sha256(read(int(address, 16), evidence[address]['size'])).hexdigest())
        body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
            '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
            input=body.encode(), capture_output=True, check=True).stdout.decode()
        line = evidence[address]['line']; additions += f'\n\n/* ManalinkEh 2022: 0x{address}; original functions/has_creature_type.c:{line}. */\n' + body
        entries.append(dict(name=name, original_name=original, address=address, source='functions/has_creature_type.c',
            line=line, static=False, prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
            recovered_literals=[], reconstruction=evidence[address]['scope'], interface_evidence=evidence[address]))
    path_source = root / 'functions/has_creature_type.c'; original = path_source.read_bytes()
    assert b'void get_subtypes_by_csvid_2022(' not in original and b'int has_subtype_by_id_2022(' not in original
    candidate = cache / 'subtype-interfaces-candidate.c'; candidate.write_bytes(original + additions.encode() +
        b'\n_Static_assert(sizeof(subtypes_not_in_play_t) == 8 && __builtin_offsetof(subtypes_not_in_play_t, added_forced) == 4, "modifier layout");\n_Static_assert(sizeof(added_subtypes_not_in_play) == 256, "modifier table");\n_Static_assert(sizeof(reported_subtypes_t) == 400 && __builtin_offsetof(card_ptr_t, types) == 132, "source subtype layout");\n')
    result = subprocess.run(['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
        '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
        '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-Werror=format', '-I', str(root), str(candidate)],
        capture_output=True, text=True)
    (cache / 'subtype-interfaces-check.log').write_text(result.stderr); result.check_returncode()
    if args.apply:
        path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(path.read_text())
        accepted = {f['address'] for f in manifest['accepted_functions']}; assert not accepted & set(addresses)
        path_source.write_bytes(original.rstrip() + additions.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        path_header = root / 'manalink.h'; header = path_header.read_bytes(); at = header.rfind(b'#ifdef __cplusplus')
        block = '/* October 2022 player-aware subtype lookup interfaces. */\n' + '\n'.join(e['prototype'] for e in entries) + '\n\n'
        path_header.write_bytes(header[:at] + block.replace('\n', '\r\n').encode() + header[at:])
        manifest['accepted_functions'].extend(entries); manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_text(json.dumps(manifest, indent=2))
        exports_path = cache / 'decompiled-abi/export.tsv'; rows = list(csv.DictReader(exports_path.open(), delimiter='\t')); by_address = {r['address']: r for r in rows}
        for row in csv.DictReader((cache / 'decompiled-subtype-interfaces/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-subtype-interfaces' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            by_address[row['address']] = row
        with exports_path.open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=rows[0], delimiter='\t'); writer.writeheader(); writer.writerows(by_address.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
        interfaces['player_aware_subtype_lookups'] = evidence; path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helpers=[e['name'] for e in entries]), indent=2))


if __name__ == '__main__':
    main()
