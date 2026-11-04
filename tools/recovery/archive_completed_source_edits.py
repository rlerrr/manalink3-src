"""Move completed projection records out of the live manifest without losing them."""
import argparse
import gzip
import hashlib
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
MASTER = ROOT / 'docs/recovery-evidence/helper-canonicalization.json'


def digest(data):
    return hashlib.sha256(data).hexdigest()


def encode(value):
    return (json.dumps(value, separators=(',', ':')) + '\n').encode()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--keep-evidence', required=True, type=Path)
    parser.add_argument('--archive', required=True, type=Path)
    parser.add_argument('--receipt', required=True, type=Path)
    args = parser.parse_args()
    if not (ROOT / 'docs/recovery-evidence/historical-artifact-retirement.json').is_file():
        parser.error('Historical replay must already be explicitly retired.')
    paths = [args.keep_evidence, args.archive, args.receipt]
    if any(path.is_absolute() or not (ROOT / path).resolve().is_relative_to(ROOT) for path in paths):
        parser.error('Use repository-relative paths inside the workspace.')
    active_path = args.keep_evidence.as_posix()
    if not (ROOT / args.keep_evidence).is_file():
        parser.error('The current batch evidence must exist.')
    archive_path, receipt_path = ROOT / args.archive, ROOT / args.receipt
    pending = MASTER.with_suffix('.pending.json')
    if archive_path.exists() or receipt_path.exists() or pending.exists():
        parser.error('Archive, receipt and pending output paths must be new.')
    before = MASTER.read_bytes()
    proof = json.loads(before)
    edits = proof['source_edits']
    batch = [edit for edit in edits if active_path in edit.values()]
    prior = [edit for edit in edits if active_path not in edit.values()]
    if not batch or not prior or edits != prior + batch:
        parser.error('Keep one complete trailing batch; preserve chronological order.')
    keep = {i for i, edit in enumerate(edits) if active_path in edit.values()}
    retained_receipts = []
    if proof.get('native_jump_table_recovery'):
        # The routine fixed-slot verifier still binds this live hook to its
        # source receipt. Retain the actual record, not a weaker replacement.
        hook = json.loads((ROOT / proof['native_jump_table_recovery']).read_bytes())['hook']
        candidates = [i for i, edit in enumerate(edits)
                      if edit.get('native_source_reconstruction', {}).get('function') == hook['name']
                      and edit['after_sha256'] == hook['definition_sha256']]
        if not candidates:
            parser.error('The live jump-table hook source receipt is missing.')
        keep.add(candidates[-1])
        retained_receipts.append(dict(name=hook['name'], original_index=candidates[-1],
                                      reason='Required by routine fixed-slot verification.'))
    active_indices = sorted(keep)
    completed_indices = [i for i in range(len(edits)) if i not in keep]
    active = [edits[i] for i in active_indices]
    completed = [edits[i] for i in completed_indices]
    staged = digest(subprocess.check_output(['git', 'diff', '--cached', '--binary'], cwd=ROOT))
    payload = encode(dict(format=1, source_master_sha256=digest(before), source_edits=completed,
                          source_edit_positions=completed_indices))
    compressed = gzip.compress(payload, compresslevel=6, mtime=0)
    assert json.loads(gzip.decompress(compressed))['source_edits'] == completed
    pin = dict(path=args.archive.as_posix(), sha256=digest(compressed),
               uncompressed_sha256=digest(payload), entries=len(completed),
               retained_edit_positions=active_indices,
               receipt=args.receipt.as_posix())
    receipt = dict(format=1, status='Completed edit records losslessly archived; live pins and current batch retained.',
                   before_master_sha256=digest(before), before_master_bytes=len(before),
                   archive=pin, compressed_bytes=len(compressed), uncompressed_bytes=len(payload),
                   current_batch=active_path, current_batch_edits=len(batch),
                   retained_active_edits=len(active), retained_current_receipts=retained_receipts,
                   roundtrip='All completed records compare equal after decompression.',
                   current_file_pins=len(proof['files']), current_function_names=len(proof['functions']),
                   staged_diff_sha256=staged,
                   tool_sha256=digest(Path(__file__).read_bytes()),
                   scope='Storage compaction only. Complete historical records remain in the tracked gzip JSON. '
                         'Current verification hashes the compressed payload without parsing or replaying it. '
                         'The previously retired historical compiler/runtime replay remains unsupported.')
    receipt_bytes = (json.dumps(receipt, indent=2) + '\n').encode()
    proof['source_edits'] = active
    proof.setdefault('source_edit_archives', []).append(pin)
    proof['interface_supporting_inputs'].extend([
        dict(path=args.archive.as_posix(), sha256=digest(compressed)),
        dict(path=args.receipt.as_posix(), sha256=digest(receipt_bytes))])
    after = encode(proof)
    # Reconstruct the entire predecessor manifest before writing anything.
    restored = json.loads(after)
    archived = json.loads(gzip.decompress(compressed))
    by_position = dict(zip(archived['source_edit_positions'], archived['source_edits']))
    by_position.update(zip(active_indices, active))
    assert set(by_position) == set(range(len(edits)))
    restored['source_edits'] = [by_position[i] for i in range(len(edits))]
    restored['source_edit_archives'].pop()
    if not restored['source_edit_archives']:
        del restored['source_edit_archives']
    del restored['interface_supporting_inputs'][-2:]
    assert encode(restored) == before
    assert len(after) < len(before) and len(after) < 104857600
    assert MASTER.read_bytes() == before
    archive_path.parent.mkdir(parents=True, exist_ok=True)
    receipt_path.parent.mkdir(parents=True, exist_ok=True)
    archive_path.write_bytes(compressed)
    receipt_path.write_bytes(receipt_bytes)
    pending.write_bytes(after)
    pending.replace(MASTER)
    assert digest(subprocess.check_output(['git', 'diff', '--cached', '--binary'], cwd=ROOT)) == staged
    print(json.dumps(dict(before_bytes=len(before), after_bytes=len(after), archived_edits=len(completed),
                          active_edits=len(active), compressed_bytes=len(compressed), staging='preserved')))


if __name__ == '__main__':
    main()
