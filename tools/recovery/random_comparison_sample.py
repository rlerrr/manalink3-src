"""Freeze a deterministic stratified random sample before inspecting differences.

The frame is unambiguous shared C function identities, excluding prior pilot
functions and already recovered target implementations. No code-size, instruction
similarity, or expected-change criterion is used to select or replace functions.
"""
import argparse
import collections
import csv
import hashlib
import json
import shutil
from pathlib import Path

from compare_releases import Release, digest, pair_identities, read_functions


def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('--seed', default='manalink-shared-functions-2026-10-18-v1')
    parser.add_argument('--per-stratum', type=int, default=15)
    args = parser.parse_args()
    assert args.per_stratum > 0
    root = Path(__file__).resolve().parents[2]
    cache = root / '.recovery'
    output = cache / 'comparison-random'
    output.mkdir(exist_ok=True)
    rows = {y: read_functions(cache / f'{y}-functions.csv') for y in ['2020', '2022']}
    pairs, ambiguous, _, _ = pair_identities(rows['2020'], rows['2022'])
    prior = json.loads((root / 'docs/recovery-evidence/modified-functions-pilot.json').read_text())
    recovered_path = root / 'docs/recovery-evidence/recovered-functions.json'
    recovered = json.loads(recovered_path.read_text())
    prior_names = {r['name'] for r in prior['pilot']}
    recovered_addresses = {int(f['address'], 16) for f in recovered['accepted_functions']}
    counts = collections.Counter()
    eligible = collections.defaultdict(list)
    for old, new, method in pairs:
        if not old['source'].endswith('.c') or not new['source'].endswith('.c'):
            counts['non-C-source'] += 1
        elif old['name'] in prior_names:
            counts['prior-pilot-name'] += 1
        elif new['start'] in recovered_addresses:
            counts['already-recovered-target'] += 1
        elif old['end'] <= old['start'] or new['end'] <= new['start']:
            counts['invalid-contiguous-extent'] += 1
        else:
            stratum = 'cards' if old['name'].startswith('card_') else 'helpers'
            identity = '\t'.join([old['source'], old['name'], old['low_pc'], new['source'], new['low_pc']])
            rank = hashlib.sha256((args.seed + '\0' + stratum + '\0' + identity).encode()).hexdigest()
            eligible[stratum].append((rank, old, new, method))
    frame_rows = []
    selection = []
    for stratum in ['cards', 'helpers']:
        ranked = sorted(eligible[stratum], key=lambda x: x[0])
        assert len(ranked) >= args.per_stratum
        for i, (rank, old, new, method) in enumerate(ranked):
            frame_rows.append([stratum, rank, old['source'], old['name'], old['low_pc'], new['source'], new['low_pc']])
            if i < args.per_stratum:
                selection.append(dict(stratum=stratum, rank=rank, old=old, new=new,
                                      method=method, artifact=old['name'] + '__' + format(old['start'], 'x')))
    frozen = dict(seed=args.seed, per_stratum=args.per_stratum,
                  design='SHA-256 ranking of seed, stratum and sorted identity fields; lowest ranks selected without replacement before decoding or decompilation.',
                  frame_counts={s: len(eligible[s]) for s in ['cards', 'helpers']},
                  excluded_pair_counts=dict(counts), paired_identities=len(pairs),
                  ambiguous_shared_keys=len(ambiguous),
                  prior_review_sha256=digest((root / 'docs/recovery-evidence/modified-functions-pilot.json').read_bytes()),
                  recovered_manifest_sha256=digest(recovered_path.read_bytes()),
                  inventory_sha256={y: digest((cache / f'{y}-functions.csv').read_bytes()) for y in rows},
                  selection=selection)
    frozen_path = output / 'selection.json'
    if frozen_path.exists():
        assert json.loads(frozen_path.read_text()) == frozen, 'Frozen selection changed; use a separate experiment directory.'
    else:
        frozen_path.write_text(json.dumps(frozen, indent=2) + '\n')
    with (output / 'sampling-frame.csv').open('w', newline='') as stream:
        writer = csv.writer(stream)
        writer.writerow(['stratum', 'random_rank', 'source_2020', 'name', 'va_2020', 'source_2022', 'va_2022'])
        writer.writerows(frame_rows)
    # Only after freezing the selection do we open and decode the code bytes.
    identities = {y: collections.defaultdict(set) for y in rows}
    for old, new, _ in pairs:
        key = f'{old["source"]}::{old["name"]}'
        identities['2020'][old['start']].add(key)
        identities['2022'][new['start']].add(key)
    releases = {y: Release(cache, y, rows[y], {a: '|'.join(sorted(keys)) for a, keys in identities[y].items()}) for y in rows}
    for year in rows:
        assert digest(releases[year].image.data) == prior['inputs'][year]['sha256']
        requests = []
        for item in selection:
            r = item['old' if year == '2020' else 'new']
            requests.append(f'{r["start"]:x}\t{r["source"]}\t{r["name"]}\t{r["end"]:x}\t{item["artifact"]}\n')
        (output / f'{year}-requests.tsv').write_text(''.join(requests))
        shutil.copytree(cache / f'comparison-pilot/{year}-metadata', output / f'{year}-metadata', dirs_exist_ok=True)
    observations = []
    for item in selection:
        details = {}
        for year in ['2020', '2022']:
            row = item['old' if year == '2020' else 'new']
            detail = releases[year].describe(row)
            (output / f'{year}-{item["artifact"]}.asm').write_text('\n'.join(detail.pop('listing')) + '\n')
            details[year] = detail
        a, b = details.values()
        status = 'same-symbolic-instructions' if a['fully_decoded'] and b['fully_decoded'] and a['tokens'] == b['tokens'] else 'review-required'
        for detail in details.values():
            detail.pop('tokens')
        observations.append(dict(name=item['old']['name'], artifact=item['artifact'], stratum=item['stratum'],
                                 identity_method=item['method'], status=status, releases=details,
                                 direct_targets_added=sorted(set(b['calls']) - set(a['calls'])),
                                 direct_targets_removed=sorted(set(a['calls']) - set(b['calls']))))
    (output / 'observations.json').write_text(json.dumps(observations, indent=2) + '\n')
    print(json.dumps({k: v for k, v in frozen.items() if k != 'selection'}, indent=2))
    for r in observations:
        print(r['stratum'], r['name'], r['status'],
              r['releases']['2020']['bytes'], '->', r['releases']['2022']['bytes'],
              '+', r['direct_targets_added'], '-', r['direct_targets_removed'])


if __name__ == '__main__':
    main()
