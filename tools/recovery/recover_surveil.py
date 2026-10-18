"""Recover Surveil's VLA, selection loop and post-stack-restore event dispatch."""
import argparse
import csv
import hashlib
import json
import shutil
import subprocess
from pathlib import Path

from inspect_release import Image, value, inherited_value
from export_abi import type_of, type_name
from recover_literals import readonly_string, byte_literal


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    debug = Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    runtime = Image((cache / '2022-ManalinkEh.dll').read_bytes())
    evidence = {}; flags = {}
    def descendants(die):
        for child in die.iter_children():
            yield child
            yield from descendants(child)
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(), 'DW_AT_name') != 'cards/guilds_of_ravnica.c':
            continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag == 'DW_TAG_typedef' and value(die, 'DW_AT_name') == 'test_definition_t':
                evidence['test_layout'] = {value(m, 'DW_AT_name'): value(m, 'DW_AT_data_member_location')
                    for m in type_of(die).iter_children() if m.tag == 'DW_TAG_member'}
            if value(die, 'DW_AT_low_pc') != 0x26198e0:
                continue
            assert inherited_value(die, 'DW_AT_name') == 'surveil'
            evidence['line'] = str(value(die, 'DW_AT_decl_line'))
            assert value(die, 'DW_AT_high_pc') == 436
            variables = {inherited_value(v, 'DW_AT_name'): v for v in descendants(die) if v.tag == 'DW_TAG_variable'}
            assert type_name(type_of(variables['added'])).startswith('int[')
            assert type_of(variables['added']).tag == 'DW_TAG_array_type'
            assert type_name(type_of(variables['to_look'])) == type_name(type_of(variables['ac'])) == 'int'
            evidence['locals'] = {n: dict(type=type_name(type_of(v)), location=value(v, 'DW_AT_location'))
                                  for n, v in variables.items() if n in ('added', 'to_look', 'ac', 'this_test')}
            assert evidence['locals']['this_test']['location'] == [145, 148, 126]
            sites = {value(s, 'DW_AT_low_pc'): s for s in descendants(die) if s.tag == 'DW_TAG_GNU_call_site'}
            for at, name in [(0x2619997, 'new_default_test_definition'), (0x26199cb, 'new_select_a_card'),
                             (0x2619a65, 'dispatch_event')]:
                assert inherited_value(sites[at], 'DW_AT_name') == name
            evidence['dwarf_call_arguments'] = {f'{at:x}': [value(p, 'DW_AT_GNU_call_site_value')
                for p in sites[at].iter_children()] for at in (0x2619997, 0x26199cb, 0x2619a65)}
            assert evidence['dwarf_call_arguments']['2619997'][-1] == [3, 236, 169, 117, 2]
            assert evidence['dwarf_call_arguments']['2619a65'] == [[115, 0], [9, 255], [10, 0, 83]]
        for die in cu.iter_DIEs():
            if die.tag == 'DW_TAG_enumerator' and value(die, 'DW_AT_name') in ('EVENT_SURVEIL', 'TYPE_ANY', 'TUTOR_FROM_DECK', 'AI_MIN_VALUE'):
                flags[value(die, 'DW_AT_name')] = value(die, 'DW_AT_const_value')
        break
    assert flags == dict(EVENT_SURVEIL=0x5300, TYPE_ANY=255, TUTOR_FROM_DECK=1, AI_MIN_VALUE=3)
    assert evidence['test_layout']['create_minideck'] == 194
    # This is a source-level object consumed by the repository's C routines,
    # not a binary structure projected onto the newer frame allocation.
    assert 'zone_count = this_test->create_minideck;' in (root / 'functions/tutors.c').read_text()
    assert 'this_test->create_minideck = 0;' in (root / 'functions/global_checking.c').read_text()
    evidence['source_test_adapter'] = dict(target_create_minideck=194, source_create_minideck=186,
        initialization='functions/global_checking.c::new_default_test_definition',
        consumer='functions/tutors.c::new_select_a_card',
        scope='Use the named member of the source-level object with source C consumers; do not project the target frame offsets onto the old structure.')
    assert any(n == '_surveil' and storage == 2 and debug.pe.OPTIONAL_HEADER.ImageBase +
               debug.pe.sections[s - 1].VirtualAddress + off == 0x26198e0
               for n, off, s, kind, storage in debug.functions())
    def read(at, length):
        rva = at - runtime.pe.OPTIONAL_HEADER.ImageBase
        section = next(s for s in runtime.pe.sections if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize)
        offset = section.PointerToRawData + rva - section.VirtualAddress
        return runtime.data[offset:offset + length]
    checks = {
        '26198f7': '3b450c89c67e038b750c',  # signed minimum(count, number)
        '2619909': '8d04b500000000',        # VLA count * sizeof(int)
        '2619916': 'e8b5280e0029c4',        # probe, then allocate once
        '2619953': '8b148289148f',          # save deck[selected] in added[ac]
        '261996b': '474e',                  # ++ac, --remaining
        '2619997': '668975de',              # uint16 create_minideck, remaining
        '26199cb': '83f8ff0f856cffffff',    # selection -1 ends loop
        '26199fb': 'e830940900',            # add_card_to_hand
        '2619a07': 'e804a90900',            # put_on_top_of_deck
        '2619a27': 'e804fa0900',            # mill selected count
        '2619a3b': 'e870a20300',            # rearrange unselected remainder
        '2619a40': '8ba50cffffff',          # restore ESP before event arguments
        '2619a49': '85d27e18',              # dispatch iff original number > 0
        '2619a4d': 'c744240800530000c7442404ffffffff891c24',
        '2619a60': 'e84bbfe1fd',
    }
    for at, expected in checks.items():
        assert read(int(at, 16), len(bytes.fromhex(expected))) == bytes.fromhex(expected)
    evidence['machine_checks'] = checks
    evidence['machine_sha256'] = hashlib.sha256(read(0x26198e0, 436)).hexdigest()
    evidence['scope'] = 'VLA capacity is the initial signed minimum; remaining shrinks only after a selection. Return selected cards via hand/top, mill their count, then reorder only the remainder. Positive original number dispatches even on an empty deck.'
    prompt = readonly_string(runtime, 0x275a9ec)
    assert prompt == b'Select a card to put into your graveyard'
    literal = dict(symbol='UNK_0275a9ec', address='0x275a9ec', bytes=prompt.hex(), call='new_default_test_definition',
                   argument=2, load_address='2619979', load_bytes='c7442408eca97502',
                   call_address='2619992', call_target='26bea60', call_bytes='e8c9500a00')
    raw = (cache / 'decompiled-surveil/26198e0.c').read_text()
    assert raw.count('&UNK_0275a9ec') == 1
    assert '0x5300' in raw and "'S'" not in raw
    body = '''void surveil(int player, int number)
{
  int to_look = count_deck(player);
  if (number < to_look)
    to_look = number;
  if (to_look != 0)
  {
    int added[to_look];
    int ac = 0;
    do
    {
      test_definition_t this_test;
      new_default_test_definition(&this_test, TYPE_ANY, PROMPT);
      this_test.create_minideck = (unsigned short)to_look;
      int selected = new_select_a_card(player, player, TUTOR_FROM_DECK, 0, AI_MIN_VALUE, -1, &this_test);
      if (selected == -1)
        break;
      added[ac] = deck_ptr[player][selected];
      remove_card_from_deck(player, selected);
      ++ac;
      --to_look;
    } while (to_look != 0);
    if (ac != 0)
    {
      for (int i = 0; i < ac; ++i)
      {
        int card_added = add_card_to_hand(player, added[i]);
        put_on_top_of_deck(player, card_added);
      }
      mill(player, ac);
    }
    if (to_look != 0)
      rearrange_top_x(player, player, to_look);
  }
  if (number > 0)
    dispatch_event(player, -1, EVENT_SURVEIL);
}
'''.replace('PROMPT', byte_literal(prompt))
    body = subprocess.run(['C:/Program Files/LLVM/bin/clang-format.exe',
        '-style={BasedOnStyle: LLVM, IndentWidth: 2, BreakBeforeBraces: Allman, ColumnLimit: 100}'],
        input=body.encode(), capture_output=True, check=True).stdout.decode()
    source = root / 'cards/guilds_of_ravnica.c'
    original = source.read_bytes(); assert b'void surveil(' not in original
    addition = f"\n\n/* ManalinkEh 2022: 0x26198e0; original cards/guilds_of_ravnica.c:{evidence['line']}. */\n" + body
    candidate = cache / 'surveil-candidate.c'; candidate.write_bytes(original + addition.encode() +
        b'\n_Static_assert(__builtin_offsetof(test_definition_t, create_minideck) == 186, "source minideck slot");\n')
    command = ['C:/Program Files/LLVM/bin/clang.exe', '-target', 'i686-pc-windows-msvc', '-std=gnu99', '-fsyntax-only',
               '-Werror=implicit-function-declaration', '-Werror=incompatible-pointer-types', '-Werror=return-type',
               '-Werror=uninitialized', '-Werror=sometimes-uninitialized', '-I', str(root), str(candidate)]
    result = subprocess.run(command, capture_output=True, text=True)
    (cache / 'surveil-check.log').write_text(result.stderr); result.check_returncode()
    entry = dict(name='surveil', address='26198e0', source='cards/guilds_of_ravnica.c', line=evidence['line'], static=False,
                 prototype='void surveil(int player, int number);', raw_sha256=hashlib.sha256(raw.encode()).hexdigest(),
                 recovered_literals=[], reconstructed_literal_loads=[literal], interface_evidence=evidence,
                 reconstruction='Reconstruct VLA and scoped test object from DWARF and machine instructions; preserve selected-card reinsertion order, mill and final dispatch after stack restoration.')
    if args.apply:
        path = root / 'docs/recovery-evidence/recovered-functions.json'; manifest = json.loads(path.read_text())
        assert not any(f['address'] == entry['address'] for f in manifest['accepted_functions'])
        source.write_bytes(original.rstrip() + addition.replace('\r\n', '\n').replace('\n', '\r\n').encode())
        path_header = root / 'manalink.h'; header = path_header.read_bytes(); at = header.rfind(b'#ifdef __cplusplus')
        assert b'void surveil(' not in header
        path_header.write_bytes(header[:at] + b'/* Recovered October 2022 Surveil helper. */\r\nvoid surveil(int player, int number);\r\n\r\n' + header[at:])
        manifest['accepted_functions'].append(entry); manifest['accepted'] = len(manifest['accepted_functions'])
        path.write_text(json.dumps(manifest, indent=2))
        shutil.copyfile(cache / 'decompiled-surveil/26198e0.c', cache / 'decompiled-abi/26198e0.c')
        path = root / 'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(path.read_text())
        interfaces['surveil_vla_reconstruction'] = evidence; path.write_text(json.dumps(interfaces, indent=2))
    print(json.dumps(dict(applied=args.apply, helper=entry['name'], prompt=prompt.decode(), evidence=evidence), indent=2))


if __name__ == '__main__':
    main()
