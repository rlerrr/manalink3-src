"""Recover missing target enum constants in their callers' own source scopes.

Only explicit, unambiguous enumeration DIEs at compilation-unit scope qualify.
Each value is checked in the same DWARF unit as the function that uses it.
This preserves target constants without migrating the live engine's enums.
"""
import collections
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
    missing = set(re.findall(r"use of undeclared identifier '([A-Z][A-Z0-9_]+)'", diagnostics))
    # Command choice enums have function/lexical scope and a separate recovery.
    missing = {n for n in missing if not n.startswith('CHOICE_')}
    assert missing
    # Compiler diagnostics establish absence in the active Manalink headers.
    # Some names exist only in the conditional SHANDALAR definitions, with
    # different values; function scope avoids changing that other build.
    accepted = {f['address'] for f in json.loads(
        (root / 'docs/recovery-evidence/recovered-functions.json').read_text())['accepted_functions']}
    candidates = {}; wanted = collections.defaultdict(set)
    for row in csv.DictReader((cache / 'decompiled-abi/export.tsv').open(), delimiter='\t'):
        if row['address'] in accepted or row['status'] != 'ok':
            continue
        raw = (cache / 'decompiled-abi' / (row['address'] + '.c')).read_text()
        used = set(re.findall(r'\b[A-Z][A-Z0-9_]+\b', code_only(strip_comments(raw)))) & missing
        if used:
            candidates[row['address']] = dict(row, missing=used)
            wanted[row['source']].update(used)
    data = Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    debug = Image(data); found = {}
    for cu in debug.dwarf().iter_CUs():
        source = value(cu.get_top_DIE(), 'DW_AT_name')
        if source not in wanted:
            continue
        values = collections.defaultdict(list)
        for die in cu.get_top_DIE().iter_children():
            if die.tag != 'DW_TAG_enumeration_type':
                continue
            for enumerator in die.iter_children():
                name = value(enumerator, 'DW_AT_name')
                if enumerator.tag == 'DW_TAG_enumerator' and name in wanted[source]:
                    number = value(enumerator, 'DW_AT_const_value')
                    assert isinstance(number, int) and 0 <= number <= 0xffffffff
                    assert value(die, 'DW_AT_byte_size') == 4
                    values[name].append(dict(value=number, die=hex(enumerator.offset),
                        enum_die=hex(die.offset), enum_bytes=4))
        found[source] = {name: entries[0] for name, entries in values.items() if len(entries) == 1}
    path = root / 'docs/recovery-evidence/local-enums.json'
    result = json.loads(path.read_text())
    assert result['debug_sha256'] == hashlib.sha256(data).hexdigest()
    scope = ('Function-local aliases for target enum values, verified in this function\'s '
             'own compilation unit. Full counter, event, token-action and engine integration remains deferred.')
    functions = {}; constants = {}
    for address, row in candidates.items():
        values = {n: found[row['source']][n] for n in sorted(row['missing']) if n in found[row['source']]}
        if not values:
            continue
        enumerators = {n: e['value'] for n, e in values.items()}
        declaration = 'enum { ' + ', '.join(n + ' = ' + str(v) for n, v in enumerators.items()) + ' };'
        entry = result['functions'].setdefault(address,
            dict(source=row['source'], name=row['name'], types=[], declarations=[]))
        assert entry['source'] == row['source'] and entry['name'] == row['name']
        existing = {n: v for t in entry['types'] for n, v in t['enumerators'].items()}
        assert all(n not in existing or existing[n] == v for n, v in enumerators.items())
        enumerators = {n: v for n, v in enumerators.items() if n not in existing}
        if not enumerators:
            continue
        declaration = 'enum { ' + ', '.join(n + ' = ' + str(v) for n, v in enumerators.items()) + ' };'
        entry['types'].append(dict(name=None, bytes=4, enumerators=enumerators, recovery=scope,
                                  dwarf_enumerators=values))
        entry['declarations'].append(declaration)
        functions[address] = dict(source=row['source'], name=row['name'], enumerators=enumerators)
        for n, v in enumerators.items():
            assert n not in constants or constants[n] == v
            constants[n] = v
    result.setdefault('target_constant_recoveries', []).append(dict(scope=scope, functions=functions,
        constants=constants, unresolved_names=sorted(missing - set(constants))))
    path.write_text(json.dumps(result, indent=2))
    print(json.dumps(dict(function_scopes=len(functions), constants=len(constants), values=constants), indent=2))


if __name__ == '__main__':
    main()
