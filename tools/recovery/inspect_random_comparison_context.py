"""Check data and optimized-parameter traps encountered in the random sample."""
import csv
import json
import math
import re
import struct
from pathlib import Path

from export_abi import type_name, type_of
from inspect_release import Image, inherited_value, value


def descendants(die):
    for child in die.iter_children():
        yield child
        if child.has_children:
            yield from descendants(child)


def main():
    root = Path(__file__).resolve().parents[2]
    output = root / '.recovery/comparison-random'
    evidence = {}
    for year, debug_path in [('2020', root / 'ManalinkEh.dbg'),
                             ('2022', Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg'))]:
        image = Image((root / f'.recovery/{year}-ManalinkEh.dll').read_bytes())
        dwarf = Image(debug_path.read_bytes()).dwarf()
        rows = list(csv.DictReader((root / f'.recovery/{year}-functions.csv').open()))
        row = next(r for r in rows if r['name'] == 'get_hacked_land_text' and r['low_pc'])
        die = dwarf.get_DIE_from_refaddr(int(row['die_offset'], 16))
        words = next(c for c in descendants(die) if c.tag == 'DW_TAG_variable' and inherited_value(c, 'DW_AT_name') == 'words')
        location = value(words, 'DW_AT_location')
        assert len(location) == 5 and location[0] == 3  # DW_OP_addr, 32-bit
        address = struct.unpack('<I', bytes(location[1:]))[0]
        typename = type_name(type_of(words))
        count = math.prod(map(int, re.findall(r'\[(\d+)\]', typename)))
        assert count == 24, typename
        pointers = struct.unpack('<' + 'I' * count, image.pe.get_data(address - image.pe.OPTIONAL_HEADER.ImageBase, count * 4))
        texts = []
        for pointer in pointers:
            data = image.pe.get_data(pointer - image.pe.OPTIONAL_HEADER.ImageBase, 100)
            assert b'\0' in data
            texts.append(data.split(b'\0', 1)[0].decode('ascii'))
        row = next(r for r in rows if r['name'] == 'real_copy_spell' and r['low_pc'])
        die = dwarf.get_DIE_from_refaddr(int(row['die_offset'], 16))
        parameters = []
        for c in die.iter_children():
            if c.tag == 'DW_TAG_formal_parameter':
                parameters.append(dict(name=inherited_value(c, 'DW_AT_name'),
                    type=type_name(type_of(c)), constant=value(c, 'DW_AT_const_value'),
                    location=value(c, 'DW_AT_location')))
        evidence[year] = dict(land_words_address=hex(address), land_words_type=typename,
                              land_words=texts, copy_worker_address=row['low_pc'],
                              copy_worker_die=row['die_offset'], copy_worker_concrete_parameters=parameters)
        type_name.cache_clear()
    assert evidence['2020']['land_words'] == evidence['2022']['land_words']
    old = evidence['2020']['copy_worker_concrete_parameters']
    new = evidence['2022']['copy_worker_concrete_parameters']
    constant = next(p for p in old if p['name'] == 'special_flags')
    assert constant['constant'] == 0xc0000
    assert next(p for p in new if p['name'] == 'forced_target')['type'] == 'target_t *'
    evidence['assessment'] = dict(
        land_word_tables_identical=True,
        old_copy_flags_optimized_out_as_constant='0xc0000',
        copy_caller_change='The old ECX=0 is special_flags3, not special_flags. The old special_flags is a DWARF constant 0xc0000. The new worker restores that physical argument and adds forced_target=NULL. Do not count the ECX literal difference as a new flag behavior.',
        show_deck_limitation='Unmapped native tail jump causes halt_baddata in raw C. The assembly independently shows a new zero store to hack_show_deck_force_expansion before show_deck_exe.')
    path = output / 'supporting-context.json'
    path.write_text(json.dumps(evidence, indent=2) + '\n')
    print(json.dumps(evidence['assessment'], indent=2))


if __name__ == '__main__':
    main()
