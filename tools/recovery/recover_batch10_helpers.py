"""Recover Amass, two shared interfaces, and the all-player color-count clone."""
import argparse
import collections
import csv
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

import capstone
from inspect_release import Image, inherited_value, value
from export_abi import type_of, type_name
from promote_recovery import clean, UNRESOLVED, code_only


ENTRIES = [
    ('21c7f50', 'amass', 'amass', 'cards/war_of_the_spark.c', False),
    ('222bd80', 'basic_landcycling', 'basic_landcycling_2022', 'cards/modern_horizons_2.c', False),
    ('22c64f0', 'locate_edh_card', 'locate_edh_card_2022', 'cards/vanguard.c', False),
    ('2488800', 'most_common_color', 'most_common_color_all_permanents', 'cards/invasion.c', True)]


def descendants(die):
    for child in die.iter_children():
        yield child
        yield from descendants(child)


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--prepare', action='store_true')
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    image = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    evidence, flags, layout = {}, {}, {}
    for cu in image.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {entry[3] for entry in ENTRIES}:
            continue
        for die in cu.get_top_DIE().iter_children():
            name = value(die, 'DW_AT_name')
            if source == 'cards/war_of_the_spark.c' and die.tag == 'DW_TAG_typedef' and name in {'token_generation_t', 'target_definition_t'}:
                struct = type_of(die)
                layout[name] = {value(c, 'DW_AT_name'): value(c, 'DW_AT_data_member_location')
                                for c in struct.iter_children() if c.tag == 'DW_TAG_member'}
            entry = next((entry for entry in ENTRIES if value(die, 'DW_AT_low_pc') == int(entry[0], 16)), None)
            if not entry or die.tag != 'DW_TAG_subprogram':
                continue
            assert inherited_value(die, 'DW_AT_name') == entry[1]
            parameters = [dict(name=inherited_value(p, 'DW_AT_name'), type=type_name(type_of(p)),
                               constant=value(p, 'DW_AT_const_value'), location=value(p, 'DW_AT_location'))
                          for p in die.iter_children() if p.tag == 'DW_TAG_formal_parameter']
            if entry[0] == '2488800':
                assert parameters == [dict(name='player', type='int', constant=2, location=None)]
                expected = '_most_common_color.constprop.0'
            else:
                expected = '_' + entry[1]
                assert len(parameters) == {'21c7f50': 3, '222bd80': 9, '22c64f0': 1}[entry[0]]
            assert any(n == expected and storage == (3 if entry[4] else 2) and
                       image.pe.OPTIONAL_HEADER.ImageBase + image.pe.sections[s - 1].VirtualAddress + offset == int(entry[0], 16)
                       for n, offset, s, kind, storage in image.functions())
            item = dict(line=str(inherited_value(die, 'DW_AT_decl_line')), parameters=parameters, coff_symbol=expected)
            if entry[0] == '21c7f50':
                variables = [dict(name=inherited_value(v, 'DW_AT_name'), type=type_name(type_of(v)),
                                  location=value(v, 'DW_AT_location'), die=hex(v.offset))
                             for v in descendants(die) if v.tag == 'DW_TAG_variable' and inherited_value(v, 'DW_AT_name') in {'token', 'td'}]
                assert {v['type'] for v in variables} == {'token_generation_t', 'target_definition_t'}
                assert all(v['location'] == variables[0]['location'] for v in variables)
                item['shared_stack_objects'] = variables
            if entry[0] == '2488800':
                counts = [v for v in descendants(die) if v.tag == 'DW_TAG_variable' and inherited_value(v, 'DW_AT_name') == 'counts']
                assert len(counts) == 1 and type_name(type_of(counts[0])) == 'int[6]'
                item['counts_array'] = dict(type='int[6]', die=hex(counts[0].offset), location=value(counts[0], 'DW_AT_location'))
            evidence[entry[0]] = item
        if source == 'cards/war_of_the_spark.c':
            for die in cu.iter_DIEs():
                name = value(die, 'DW_AT_name')
                if die.tag == 'DW_TAG_enumerator' and name in {'SUBTYPE_ARMY', 'TOKEN_ACTION_ADD_COUNTERS', 'COUNTER_P1_P1'}:
                    flags[name] = value(die, 'DW_AT_const_value')
    assert len(evidence) == 4
    assert flags == {'COUNTER_P1_P1': 0, 'SUBTYPE_ARMY': 254, 'TOKEN_ACTION_ADD_COUNTERS': 1 << 20}
    assert layout['token_generation_t']['action'] == layout['target_definition_t']['power_requirement'] == 48
    assert layout['token_generation_t']['action_argument'] == layout['target_definition_t']['toughness_requirement'] == 52
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    def read(at, length):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + length]
    code = read(0x2488800, 0x125)
    assert read(0x2488801, 5) == bytes.fromhex('b906000000')
    assert read(0x248880c, 3) == bytes.fromhex('8d75dc')
    assert read(0x2488815, 2) == bytes.fromhex('f3ab')
    disassembler = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    disassembler.detail = True
    maximum = list(disassembler.disasm(read(0x2488862, 0x8e), 0x2488862))
    # The maximum-selection paths only update registers. The array is read,
    # never overwritten as the misleading aggregate-hinted C output suggests.
    assert maximum and maximum[-1].address + maximum[-1].size == 0x24888f0
    assert not any(op.type == capstone.x86.X86_OP_MEM and op.access & capstone.CS_AC_WRITE
                   for instruction in maximum for op in instruction.operands)
    assert read(0x2488917, 3) == bytes.fromhex('89148e')
    assert read(0x2488898, 3) == bytes.fromhex('3b048e')
    evidence['2488800']['machine_reconstruction'] = dict(bytes_sha256=hashlib.sha256(code).hexdigest(),
        maximum_selection_range=['2488862', '24888f0'], explicit_memory_writes=0,
        count_increment={'address': '2488917', 'bytes': '89148e'}, final_count_comparison={'address': '2488898', 'bytes': '3b048e'},
        semantics='Six integer counters; count colors 1..5 across both players, compute maximum in registers, return every color tied at that maximum, including empty-board ties.')
    exports = list(csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'))
    manifest_path = root / 'docs/recovery-evidence/recovered-functions.json'
    manifest = json.loads(manifest_path.read_text())
    accepted = {f['address'] for f in manifest['accepted_functions']}
    if args.prepare:
        functions = {row['low_pc'].removeprefix('0x'): row for row in csv.DictReader(
            (cache / '2022-functions.csv').open()) if row['low_pc']}
        requests = {}
        pattern = r'\b(?:' + '|'.join(entry[1] for entry in ENTRIES) + r')\s*\('
        for row in exports:
            if row['address'] in accepted:
                continue  # Existing private same-name helpers keep their own interfaces/exports.
            raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
            if re.search(pattern, raw):
                requests[row['address']] = row
        for address, original, name, source, private in ENTRIES:
            requests[address] = dict(address=address, source=source, name=original)
        (cache / 'export-request-batch10.tsv').write_text(''.join(
            f"{at}\t{row['source']}\t{row['name']}\t{int(functions[at]['high_pc'],16):x}\n" for at, row in requests.items()))
        (cache / 'batch10-helper-evidence.json').write_text(json.dumps(dict(functions=evidence, flags=flags, layout=layout), indent=2))
        print(json.dumps(dict(exports=len(requests), functions=list(evidence))))
        return
    metadata, additions = [], collections.defaultdict(str)
    for address, original, name, source, private in ENTRIES:
        raw = (cache / 'decompiled-batch10' / (address + '.c')).read_text()
        body = clean(raw)
        if address == '21c7f50':
            assert 'target_definition_t td;' in body
            body = body.replace('target_definition_t td;', 'target_definition_t td;\n  token_generation_t token;\n  enum { SUBTYPE_ARMY = 254 };')
            old = '''default_token_definition(player,card,0x4e58,(token_generation_t *)&td);
      td.power_requirement = 0x100000;
      td.toughness_requirement = amount;
      generate_token((token_generation_t *)&td);'''
            new = '''default_token_definition(player,card,0x4e58,&token);
      token.action = TOKEN_ACTION_ADD_COUNTERS;
      token.action_argument = amount;
      generate_token(&token);'''
            assert old in body
            body = body.replace(old, new)
        if address == '2488800':
            assert re.search(r'int\s+most_common_color_all_permanents\(void\)', body)
            body = '''int most_common_color_all_permanents(void)
{
  int counts[6] = {0};
  int p, c, color, most = 0, result = 0;
  for (p = 0; p < 2; ++p)
    for (c = 0; c < active_cards_count[p]; ++c)
      if (in_play(p, c) && is_what(p, c, 0x47))
      {
        unsigned int colors = get_color(p, c);
        for (color = 1; color < 6; ++color)
          if (colors & (1U << color))
            ++counts[color];
      }
  for (color = 1; color < 6; ++color)
    if (counts[color] > most)
      most = counts[color];
  for (color = 1; color < 6; ++color)
    if (counts[color] >= most)
      result |= 1U << color;
  return result;
}
'''
        if private:
            body = 'static ' + body
        assert not UNRESOLVED.search(code_only(body)) and '._' not in body
        command = ['C:/Program Files/LLVM/bin/clang-format.exe',
                   '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}']
        body = subprocess.run(command, input=body.encode(), capture_output=True, check=True).stdout.decode()
        additions[source] += f"\n\n/* ManalinkEh 2022: 0x{address}; original {source}:{evidence[address]['line']}. */\n" + body
        item = dict(name=name, original_name=original, address=address, source=source, line=evidence[address]['line'], static=private,
                    prototype=body[:body.index('{')].strip() + ';', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(), recovered_literals=[],
                    reconstruction='Verified concrete interface and focused source reconstruction; see interface evidence.', interface_evidence=evidence[address])
        if address == '21c7f50':
            item['local_enum_evidence'] = [dict(name=None, bytes=4, enumerators={'SUBTYPE_ARMY': 254})]
        metadata.append(item)
    for source, addition in additions.items():
        original = (root / source).read_bytes()
        assert all((' ' + f['name'] + '(').encode() not in original for f in metadata if f['source'] == source)
        candidate = cache / ('batch10-candidate-' + Path(source).name)
        candidate.write_bytes(original + addition.encode())
        command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99',
                   '-fsyntax-only', '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types',
                   '-Werror=return-type', '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)]
        result = subprocess.run(command, capture_output=True, text=True)
        (cache / (candidate.stem + '-check.log')).write_text(result.stderr)
        result.check_returncode()
    if args.apply:
        for source, addition in additions.items():
            path = root / source
            path.write_bytes(path.read_bytes().rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        header = root / 'manalink.h'; data = header.read_bytes(); at = data.rfind(b'#ifdef __cplusplus')
        declarations = '\n/* Focused October 2022 shared helpers. */\n' + '\n'.join(f['prototype'] for f in metadata if not f['static']) + '\n\n'
        header.write_bytes(data[:at] + declarations.replace('\n', '\r\n').encode() + data[at:])
        manifest['accepted_functions'].extend(metadata); manifest['accepted'] = len(manifest['accepted_functions'])
        manifest_path.write_text(json.dumps(manifest, indent=2))
        rows = {row['address']: row for row in exports}
        for row in csv.DictReader((cache / 'decompiled-batch10/export.tsv').open(), delimiter='\t'):
            assert row['address'] not in accepted
            shutil.copyfile(cache / 'decompiled-batch10' / (row['address'] + '.c'), cache / 'decompiled-abi' / (row['address'] + '.c'))
            rows[row['address']] = row
        with (cache / 'decompiled-abi/export.tsv').open('w', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=exports[0], delimiter='\t'); writer.writeheader(); writer.writerows(rows.values())
        path = root / 'docs/recovery-evidence/interface-recovery.json'
        interfaces = json.loads(path.read_text()); interfaces['batch10_shared_helpers'] = dict(functions=metadata, flags=flags, layout=layout,
            scope='Amass uses recovered token counter action; public aliases avoid collisions with older private helpers; color maximum is a scalar, with all ties preserved.')
        path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, functions=[f['name'] for f in metadata]), indent=2))


if __name__ == '__main__':
    main()
