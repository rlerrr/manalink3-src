"""Inventory native names against accepted addresses, rebuilt code and source bodies.

Unmatched names are review candidates, not a count of unrecovered functions:
the compiler can inline or omit existing bodies, and C++ template/method names
need additional review. This inventory does not detect modified 2020 bodies.
"""
import argparse
import collections
import csv
import json
from pathlib import Path

from audit_source import function_body
from inspect_release import Image
from recover_temporary_exile import save_json, sha
from recover_test_consumers import read


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    manifest = json.loads((root / 'docs/recovery-evidence/recovered-functions.json').read_text())
    debug = Image((root / 'ManalinkEh.dbg').read_bytes())
    names = {f['name'] for f in manifest['accepted_functions']}
    accepted = {f['address'] for f in manifest['accepted_functions']}
    names.update(n.lstrip('_') for n, v, section, kind, storage in debug.functions()
                 if section > 0 and kind == 32 and
                 debug.pe.sections[section - 1].Characteristics & 0x20000000)
    rows = [r for r in csv.DictReader((cache / '2022-functions.csv').open())
            if r['low_pc'] and r['high_pc']]
    cards = [r for r in rows if r['name'].startswith('card_')]
    missing_cards = {r['name']: dict(address=r['low_pc'], source=r['source'])
                     for r in cards if r['name'] not in names and r['low_pc'][2:] not in accepted}
    candidates = []
    source_hashes = {}
    for row in rows:
        if row['name'] in names or row['low_pc'][2:] in accepted:
            continue
        path = root / row['source']
        if path.is_file():
            source_hashes[row['source']] = sha(path.read_bytes())
        try:
            function_body(read(path), row['name'])
            present = True
        except (ValueError, FileNotFoundError):
            present = False
        category = ('deferred chooser UI' if row['source'] == 'functions/choose_a_card.cpp' else
                    'existing source body; COFF emission absent' if present else
                    'source body absent in named unit; investigate')
        candidates.append(dict(address=row['low_pc'][2:], name=row['name'], source=row['source'],
                               native_bytes=int(row['high_pc'], 16) - int(row['low_pc'], 16),
                               source_body_present=present, category=category))
    result = dict(native_card_function_ranges=len(cards),
                  native_card_names=len({r['name'] for r in cards}),
                  missing_card_names=len(missing_cards), missing_cards=missing_cards,
                  unmatched_native_ranges=len(candidates),
                  unmatched_categories=dict(collections.Counter(c['category'] for c in candidates)),
                  unmatched=candidates, source_sha256=source_hashes,
                  inputs={str(path): sha(path.read_bytes()) for path in
                          [cache / '2022-functions.csv', cache / '2022-ManalinkEh.dll',
                           Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg'),
                           root / 'ManalinkEh.dll', root / 'ManalinkEh.dbg']},
                  accepted_functions=len(accepted),
                  scope='Name/address inventory only. Existing source can be inlined or omitted; '
                        'unmatched ranges require review. Does not establish helper completion, '
                        '2020-to-2022 modification coverage or gameplay correctness.')
    if args.output:
        save_json(args.output, result)
    print(json.dumps({k: result[k] for k in ['native_card_function_ranges', 'native_card_names',
                                           'missing_card_names', 'unmatched_native_ranges',
                                           'unmatched_categories']}, indent=2))


if __name__ == '__main__':
    main()
