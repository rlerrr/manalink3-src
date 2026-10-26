"""Count native non-card ranges lacking an address-based recovery record.

This measures review/reconstruction coverage, not missing source or changes
between releases. Existing 2020 bodies may already match the 2022 behavior.
"""
import argparse,collections,csv,json,re
from pathlib import Path
from recover_temporary_exile import save_json,sha
from recover_test_consumers import read


def main():
    parser=argparse.ArgumentParser(__doc__)
    parser.add_argument('--output',type=Path)
    args=parser.parse_args()
    root=Path(__file__).resolve().parents[2]
    metadata=root/'.recovery/2022-functions.csv'
    ledger=root/'docs/recovery-evidence/recovered-functions.json'
    m=json.loads(ledger.read_text());accepted={int(f['address'],16) for f in m['accepted_functions']}
    rows=[r for r in csv.DictReader(metadata.open()) if r['low_pc'] and r['high_pc'] and
          int(r['high_pc'],16)>int(r['low_pc'],16) and not r['name'].startswith('card_')]
    assert len(rows)==len({r['low_pc'] for r in rows})
    left=[r for r in rows if int(r['low_pc'],16) not in accepted]
    counts=collections.Counter(r['source'] for r in left);texts={};source_hashes={};presence=collections.Counter()
    for r in left:
        source=r['source'];path=root/source
        if source.endswith('.cpp'):
            presence['C++ range; definition parser not applied']+=1;continue
        if source not in texts:
            texts[source]=read(path) if path.is_file() else ''
            if path.is_file():source_hashes[source]=sha(path.read_bytes())
        # Anchor an actual C definition, not an earlier call in a condition.
        definition=re.compile(r'(?m)^[ \t]*(?:[A-Za-z_][\w]*[ \t*]+)+'+re.escape(r['name'])+r'\s*\([^;{}]*\)\s*\{')
        presence['existing C definition found' if definition.search(texts[source]) else
                 'C definition not found by anchored parser; review needed']+=1
    result=dict(native_noncard_ranges=len(rows),ranges_with_recovery_record=len(rows)-len(left),
                remaining_ranges=len(left),remaining_distinct_names=len({r['name'] for r in left}),
                remaining_core_ranges=sum(r['source'].startswith('functions/') for r in left),
                remaining_set_local_ranges=sum(r['source'].startswith('cards/') for r in left),
                remaining_deferred_chooser_ranges=counts['functions/choose_a_card.cpp'],
                source_definition_screen=dict(presence),remaining_by_source=dict(counts.most_common()),
                inputs={str(p):sha(p.read_bytes()) for p in [metadata,ledger]},source_sha256=source_hashes,
                scope='Unique contiguous native DWARF ranges whose original name does not start with card_, minus accepted recovery addresses. Includes compiler workers, set-local helpers and deferred chooser/C++ code. Distinct names can collide across units. Source-definition presence is a lexical screen, not semantic equivalence. Does not count confirmed 2020-to-2022 changes, missing source, or prove recovery completion.')
    if args.output:save_json(args.output,result)
    print(json.dumps({k:v for k,v in result.items() if k not in {'inputs','source_sha256','remaining_by_source'}},indent=2))


if __name__=='__main__':main()
