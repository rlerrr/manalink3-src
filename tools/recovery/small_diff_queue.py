"""Rank full reccmp JSON by instruction delta, without claiming equivalence."""
import argparse
import collections
import csv
import hashlib
import json
import re
from pathlib import Path


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def review_pattern(changes):
    """Suggest a review group, never an equivalence verdict or exclusion."""
    original = [line for block in changes for _, line in block['original']]
    rebuilt = [line for block in changes for _, line in block['rebuilt']]
    diagnostic = re.compile(r'((?:cards|functions)/[^:\s"]+\.(?:c|cpp)):\d+(?=\\n)')
    normalize = lambda lines: [diagnostic.sub(r'\1:<line>', line) for line in lines]
    if original and original != rebuilt and normalize(original) == normalize(rebuilt):
        return 'assertion_line_numbers'
    events = re.compile(r'cmp .*, (?:0x400|0x5400)$')
    if (original and collections.Counter(original) == collections.Counter(rebuilt)
            and all(events.fullmatch(line) for line in original)):
        return 'retarget_event_comparison_order'
    if original and collections.Counter(original) == collections.Counter(rebuilt):
        return 'instruction_reordering'
    lines = original + rebuilt
    padding = {'nop ', 'lea esi, [esi]'}
    branch = re.compile(r'(j\w+) -?0x[0-9a-f]+$')
    branch_shape = lambda items: [branch.fullmatch(line)[1] for line in items if line not in padding]
    if (lines and all(line in padding or branch.fullmatch(line) for line in lines)
            and branch_shape(original) == branch_shape(rebuilt)):
        return 'branch_layout_or_padding'
    if lines and any(line.startswith(('call ', 'jmp ')) and
                     ('<OFFSET' in line or '(FUNCTION)' in line or '(IMPORT)' in line)
                     for line in lines):
        return 'callee_or_address_mapping'
    return 'inspect_operands_and_control_flow'


def rank(report, metadata, minimum, maximum):
    candidates = []
    histogram = collections.Counter()
    for row in report['data']:
        if row.get('type') != 1 or row.get('stub'):
            continue
        score = row['matching']
        histogram['exact' if score == 1 else 'effective' if row.get('effective') else 'different'] += 1
        if score == 1 or row.get('effective') or score < minimum or not row.get('diff'):
            continue
        changes = [dict(original=block.get('orig', []), rebuilt=block.get('recomp', []))
                   for _, blocks in row['diff'] for block in blocks if 'both' not in block]
        removed = sum(len(block['original']) for block in changes)
        added = sum(len(block['rebuilt']) for block in changes)
        if max(removed, added) > maximum:
            continue
        source = metadata.get(row['address'], {})
        candidate = {key: value for key, value in row.items() if key != 'diff'}
        source_path = source.get('source')
        if source_path == 'cards/odyssey.c':
            source_path = 'cards/odissey.c'
        candidate.update(source=source_path, native_name=source.get('native_name'),
                         native_size=int(source['size']) if source.get('size') else None,
                         removed=removed, added=added, changed_lines=removed + added, changes=changes,
                         review_pattern=review_pattern(changes))
        candidates.append(candidate)
    candidates.sort(key=lambda row: (row['changed_lines'], -row['matching'], row['address']))
    return dict(histogram), candidates


def change_signature(changes):
    """Group repeated textual deltas; retain constants and branch conditions.

    Branch destinations and comparator-assigned OFFSET numbers are local to a
    function. Masking them helps find repeated review work, but does not establish
    that destinations, address identities or surrounding control flow agree.
    """
    def normalize(line):
        line = re.sub(r'<OFFSET\d+>', '<OFFSET>', line)
        return re.sub(r'^(j\w+) -?0x[0-9a-f]+$', r'\1 <destination>', line)
    return [{side: [normalize(line) for _, line in block[side]]
             for side in ('original', 'rebuilt')} for block in changes]


def group_changes(candidates):
    groups = {}
    for row in candidates:
        signature = change_signature(row['changes'])
        key = hashlib.sha256(json.dumps(signature, sort_keys=True).encode()).hexdigest()
        row['change_group'] = key
        group = groups.setdefault(key, dict(id=key, signature=signature, members=[]))
        group['members'].append(dict(address=row['address'], name=row['name'],
                                    source=row['source'], review_pattern=row['review_pattern']))
    result = list(groups.values())
    for group in result:
        group['count'] = len(group['members'])
    result.sort(key=lambda group: (-group['count'], group['members'][0]['address']))
    return result


def annotate_sites(parser, review_path, reviewer_name, label, kind, metric, report_sha256, candidates):
    """Attach one input-bound local review without changing the candidate set."""
    raw = review_path.read_bytes()
    reviewed = json.loads(raw)
    evidence_sha256 = hashlib.sha256(raw).hexdigest()
    if (reviewed.get('format') != 1 or reviewed.get('discovery_report_sha256') != report_sha256
            or not isinstance(reviewed.get('reviewed'), list)):
        parser.error(label + ' review is not bound to this comparison report.')
    reviewer = Path(__file__).with_name(reviewer_name).resolve()
    required = {'.recovery/2022-ManalinkEh.dll', 'ManalinkEh.dll',
                'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg', 'ManalinkEh.dbg', str(reviewer)}
    normalized = {str(Path(path).resolve()): value for path, value in reviewed.get('inputs', {}).items()}
    if (not {str(Path(path).resolve()) for path in required}.issubset(normalized)
            or reviewed.get('script_sha256') != digest(reviewer)
            or normalized.get(str(reviewer)) != digest(reviewer)):
        parser.error(label + ' review lacks the current reviewer and binary/debug inputs.')
    if any(not Path(path).is_file() or digest(Path(path)) != expected
           for path, expected in reviewed['inputs'].items()):
        parser.error(label + ' review inputs changed; rerun tools/recovery/' + reviewer_name + '.')
    by_address = {row['address']: row for row in reviewed['reviewed']}
    if len(by_address) != len(reviewed['reviewed']):
        parser.error(label + ' review contains duplicate native identities.')
    annotated = 0
    for candidate in candidates:
        row = by_address.get(candidate['address'])
        if row is None:
            continue
        if (row['name'] != candidate['name'] or row['source'] != candidate['source']
                or row['discovery_changes'] != candidate['changes']):
            parser.error(label + ' review differs from the candidate sites: ' + candidate['name'])
        if 'change_site_review' in candidate:
            parser.error('Overlapping site reviews require a combined proof: ' + candidate['name'])
        candidate['change_site_review'] = dict(
            kind=kind, evidence=str(review_path), evidence_sha256=evidence_sha256,
            **{metric: row['cases' if metric == 'abstract_guard_cases' else metric]},
            scope='Only the listed ' + label.lower() + ' differences, not whole-function or gameplay equivalence.')
        annotated += 1
    if digest(review_path) != evidence_sha256:
        parser.error(label + ' review changed while annotating.')
    return dict(path=str(review_path), sha256=evidence_sha256, current=True,
                annotated_change_sites=annotated, candidates_without_site_review=len(candidates)-annotated,
                scope='Annotations only. All candidates, scores and original changes are retained.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report', required=True, type=Path)
    parser.add_argument('--metadata', type=Path,
                        help='Matching CSV; defaults to the report snapshot, then the project CSV.')
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--min-score', type=float, default=.90)
    parser.add_argument('--max-changed-per-side', type=int, default=8)
    parser.add_argument('--preview', type=int, default=20)
    parser.add_argument('--group-preview', type=int, default=10)
    parser.add_argument('--event-review', type=Path,
                        help='Annotate verified event-guard sites; retain all candidates and scores.')
    parser.add_argument('--address-review', type=Path,
                        help='Annotate verified card-data address calculations; retain all candidates and scores.')
    parser.add_argument('--diagnostic-review', type=Path,
                        help='Annotate assertion source-line changes; diagnostic output still differs.')
    parser.add_argument('--cfg-review', type=Path,
                        help='Annotate decoded CFG correspondence; retain raw scores and differences.')
    args = parser.parse_args()
    if (not 0 <= args.min_score <= 1 or args.max_changed_per_side < 1
            or args.preview < 0 or args.group_preview < 0):
        parser.error('Expected score in [0, 1], positive instruction limit, nonnegative preview.')
    receipt_path = args.report.with_suffix('.inputs.json')
    receipt = json.loads(receipt_path.read_bytes())
    if args.metadata is None:
        snapshot = Path(receipt.get('metadata_snapshot', args.report.with_suffix('.metadata.csv')))
        args.metadata = snapshot if snapshot.is_file() else Path('config/reccmp/functions.csv')
    if digest(args.report) != receipt['report_sha256']:
        parser.error('Report differs from its input receipt; regenerate the JSON.')
    if digest(args.metadata) != receipt['metadata_sha256']:
        parser.error('CSV differs from the report receipt; provide its matching metadata.')
    for field, suffix in (('global_metadata', '.globals.csv'), ('global_bindings', '.globals.json'),
                          ('external_metadata', '.externals.csv'), ('external_bindings', '.externals.json'),
                          ('external_audit', '.external-audit.json')):
        if field + '_sha256' in receipt:
            snapshot = Path(receipt.get(field + '_snapshot', args.report.with_suffix(suffix)))
            if not snapshot.is_file() or digest(snapshot) != receipt[field + '_sha256']:
                parser.error('Mapping/audit snapshot differs from the report receipt: ' + str(snapshot))
    report = json.loads(args.report.read_bytes())
    if report.get('format') != 1 or not any(row.get('diff') for row in report['data']):
        parser.error('Expected format 1 with instruction diffs; omit --json-diet when comparing.')
    with args.metadata.open(newline='', encoding='utf-8') as stream:
        metadata = {row['address']: row for row in csv.DictReader(stream)}
    histogram, candidates = rank(report, metadata, args.min_score, args.max_changed_per_side)
    event_review = None
    if args.event_review is not None:
        event_review = annotate_sites(parser, args.event_review, 'review_event_guards.py', 'Event',
                                      'verified_event_predicate_permutation', 'abstract_guard_cases',
                                      receipt['report_sha256'], candidates)
    address_review = None
    if args.address_review is not None:
        address_review = annotate_sites(parser, args.address_review, 'review_card_data_bias.py', 'Address',
                                        'verified_card_data_address_bias', 'cfg_states',
                                        receipt['report_sha256'], candidates)
    diagnostic_review = None
    if args.diagnostic_review is not None:
        diagnostic_review = annotate_sites(parser, args.diagnostic_review, 'review_diagnostic_lines.py', 'Diagnostic',
                                           'diagnostic_source_lines_only', 'diagnostic_sites',
                                           receipt['report_sha256'], candidates)
    cfg_review = None
    if args.cfg_review is not None:
        cfg_review = annotate_sites(parser, args.cfg_review, 'review_branch_layout.py', 'CFG',
                                    'structural_cfg_correspondence', 'cfg_states',
                                    receipt['report_sha256'], candidates)
    groups = group_changes(candidates)
    # Old reports remain useful snapshots. Label them rather than pretending
    # their scores describe whatever DLL happens to be on disk today.
    inputs_current = all(Path(path).is_file() and digest(Path(path)) == expected
                         for path, expected in receipt['inputs'].items())
    result = dict(report=str(args.report), report_sha256=receipt['report_sha256'],
                  receipt_sha256=digest(receipt_path), inputs_current=inputs_current,
                  summary=histogram, candidates=len(candidates), functions=candidates,
                  review_patterns=dict(collections.Counter(row['review_pattern'] for row in candidates)),
                  change_groups=groups, distinct_change_groups=len(groups),
                  grouping_scope='Textual review batches only. Branch destinations and comparator-local '
                                 'OFFSET numbers are masked; all other operands and branch conditions remain. '
                                 'Each member still needs its own address/control-flow check. No exclusions.',
                  scope='Review ordering only. Unknown address placeholders, folded/moved blocks, '
                        'branch layout, width optimizations and diagnostic lines require inspection. '
                        'Patterns are review hints; all candidates remain in the queue. '
                        'Scores and small deltas alone do not prove behavior or authorize source edits.')
    if event_review is not None:
        result['event_review'] = event_review
    if address_review is not None:
        result['address_review'] = address_review
    if diagnostic_review is not None:
        result['diagnostic_review'] = diagnostic_review
    if cfg_review is not None:
        result['cfg_review'] = cfg_review
    if any(review is not None for review in (event_review, address_review, diagnostic_review, cfg_review)):
        count = sum('change_site_review' in row for row in candidates)
        result['site_review_summary'] = dict(annotated_change_sites=count, candidates_without_site_review=len(candidates)-count)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(dict(summary=histogram, small_diff_candidates=len(candidates),
                          distinct_change_groups=len(groups),
                          inputs_current=inputs_current, output=str(args.output)), indent=2))
    if event_review is not None:
        print(json.dumps(event_review, indent=2))
    if address_review is not None:
        print(json.dumps(address_review, indent=2))
    if diagnostic_review is not None:
        print(json.dumps(diagnostic_review, indent=2))
    if cfg_review is not None:
        print(json.dumps(cfg_review, indent=2))
    if 'site_review_summary' in result:
        print(json.dumps(result['site_review_summary'], indent=2))
    for row in candidates[:args.preview]:
        print(f"{row['address']} {row['name']}: {100 * row['matching']:.2f}%, "
              f"-{row['removed']}/+{row['added']} instructions ({row['source']})")
    for group in groups[:args.group_preview]:
        print(f"Group {group['id'][:12]}: {group['count']} functions; "
              f"example {group['members'][0]['name']}")


if __name__ == '__main__':
    main()
