"""Restore missing target card IDs in function scope without changing the live ID table."""
import csv
import hashlib
import json
import re
from pathlib import Path

from inspect_release import Image, value
from promote_recovery import code_only, strip_comments


def main():
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    diagnostics = (cache / 'promotion-compiler.log').read_text()
    missing = set(re.findall(r"use of undeclared identifier '(CARD_ID_[A-Z0-9_]+)'", diagnostics))
    assert missing
    existing = '\n'.join(p.read_text(errors='replace') for p in root.glob('*.h'))
    assert not any(re.search(r'\b' + name + r'\b', existing) for name in missing)
    data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    image = Image(data)
    values, units = {}, {}
    for cu in image.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in {'cards/commander_legends.c', 'cards/modern_horizons_2.c'}:
            continue
        found = {}
        for die in cu.iter_DIEs():
            name = value(die, 'DW_AT_name')
            if die.tag == 'DW_TAG_enumerator' and name in missing:
                number = value(die, 'DW_AT_const_value')
                assert name not in values or values[name] == number
                values[name] = number
                found[name] = dict(value=number, die=hex(die.offset))
        units[source] = found
    assert set(values) == missing and len(units) == 2
    assert all(0 <= number < 65536 for number in values.values())
    accepted = {f['address'] for f in json.loads((root / 'docs/recovery-evidence/recovered-functions.json').read_text())['accepted_functions']}
    functions = {}
    for row in csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'):
        raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
        used = set(re.findall(r'\bCARD_ID_[A-Z0-9_]+\b', code_only(strip_comments(raw)))) & missing
        if not used:
            continue
        assert row['address'] not in accepted
        enumerators = {name: values[name] for name in sorted(used)}
        declaration = 'enum { ' + ', '.join(name + ' = ' + str(number) for name, number in enumerators.items()) + ' };'
        functions[row['address']] = dict(source=row['source'], name=row['name'],
                                         types=[dict(name=None, bytes=4, enumerators=enumerators,
                                                     recovery='Function-local aliases for target-release IDs; live registration/ID tables are not upgraded.')],
                                         declarations=[declaration])
    result = dict(debug_sha256=hashlib.sha256(data).hexdigest(), values=values, dwarf_units=units, functions=functions,
                  scope='Function-local constants for otherwise undeclared target card IDs. No global IDs or registration records changed.')
    (root / 'docs/recovery-evidence/local-card-ids.json').write_text(json.dumps(result, indent=2))
    print(json.dumps(dict(constants=len(values), functions=len(functions), values=values), indent=2))


if __name__ == '__main__':
    main()
