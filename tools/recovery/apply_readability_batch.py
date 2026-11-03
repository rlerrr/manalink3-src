"""Apply reviewed readability drafts without replaying historical audits.

Drafts contain exact before/after definitions and local rename maps. This tool
checks the affected files and alpha-only edits, records the source projection,
and updates the existing canonical source pins. Build separately after a batch.
"""
import argparse
import json
import re
import subprocess
import time
from pathlib import Path

from canonicalize_helpers import mask, read, sha, span
from verify_assertion_call_conditions import normalized_expansion
from verify_source_spelling_edits import substitute


def apply(args):
    master = Path('docs/recovery-evidence/helper-canonicalization.json')
    original_master = master.read_bytes()
    proof = json.loads(original_master)
    leaf = Path('docs/recovery-evidence') / (args.name + '.json')
    backup = Path('.recovery') / (args.name + '-before')
    publication = master.with_suffix('.pending.json')
    assert not leaf.exists() and not backup.exists(), 'Batch already started; inspect before retrying'
    assert not publication.exists(), 'Finish the previous metadata publication first'
    index_sha = sha(subprocess.check_output(['git', 'diff', '--cached', '--binary']))
    rows, files, pending = [], [], {}
    placeholder = re.compile(r'\b(?:[A-Za-z_]*Var\d+|[A-Za-z_]*Stack_[0-9a-fA-F]+|local_[0-9a-fA-F]+|param_\d+|unaff_\w+|extraout_\w+)\b')
    for path in args.draft:
        draft = json.loads(Path(path).read_bytes())
        source = draft['source']
        assert re.fullmatch(r'(?:cards|functions)/[a-z_0-9-]+\.c', source) and source not in pending
        raw, before = Path(source).read_bytes(), read(source)
        assert draft['source_state_sha256'] == sha(original_master)
        assert draft['raw_source_bytes_sha256'] == sha(raw) and draft['index_sha256'] == index_sha
        after = before
        for row in draft['rows']:
            assert row['source'] == source
            old, new = row['before_definition'], row['after_definition']
            assert sha(old.encode()) == row['before_sha256'] and sha(new.encode()) == row['after_sha256']
            old_signature, new_signature = old.split('{', 1)[0], new.split('{', 1)[0]
            assert normalized_expansion(old_signature, source, old_signature) == normalized_expansion(new_signature, source, new_signature)
            if row['alpha_only']:
                renamed = substitute(old, row['renames'])
                assert normalized_expansion(renamed, source, renamed) == normalized_expansion(new, source, new), row['name']
            elif row.get('local_enum_restoration'):
                implementation = new
                assert len(row['inserted_local_enums']) == len(row['native_local_enums'])
                for declaration, native_enum in zip(row['inserted_local_enums'], row['native_local_enums']):
                    assert native_enum['size'] == 4 and native_enum['tag_name'] is None and not native_enum['scoped']
                    assert native_enum['source'] == source and native_enum['name'] == row['name']
                    values = native_enum['enumerators']
                    assert values and all(re.fullmatch(r'[A-Za-z_]\w*', name) and isinstance(value, int) for name, value in values)
                    expected = 'enum { ' + ', '.join(name + ' = ' + str(value) for name, value in values) + ' };'
                    assert normalized_expansion(declaration, source, declaration) == normalized_expansion(expected, source, expected)
                    assert implementation.count(declaration) == 1, row['name']
                    implementation = implementation.replace(declaration, '', 1)
                renamed = substitute(old, row.get('renames', {}))
                assert normalized_expansion(renamed, source, renamed) == normalized_expansion(implementation, source, implementation), row['name']
            elif row.get('native_enum_choice_uses'):
                from enum_choice_uses import project as project_enum_uses
                projected = project_enum_uses(old, row['native_choice_enum'], row['enum_use_sites'])
                assert normalized_expansion(projected, source, projected) == normalized_expansion(new, source, new), row['name']
            code = mask(new)
            assert not placeholder.search(code), row['name']
            # Exact alpha-token checks above also prove that existing labels and
            # jumps are unchanged. Large handlers can be renamed independently
            # of a later control-flow reconstruction.
            if not row['alpha_only'] and not row.get('local_enum_restoration') and not row.get('native_enum_choice_uses'):
                assert not re.search(r'\b(?:goto|LAB_\w+)\b', code), row['name']
            assert code.count('{') == code.count('}') and code.count('(') == code.count(')'), row['name']
            assert all(ord(c) >= 32 or c in '\n\t' for c in new), row['name']
            start, end = span(after, row['name'])
            assert after[start:end] == old, row['name']
            after = after[:start] + new + after[end:]
            row['batch_readability_cleanup'] = True
            rows.append(row)
        pin = next(v for v in proof['files'] if v['source'] == source)
        assert pin['canonical_sha256'] == sha(before.encode())
        pin['canonical_sha256'] = sha(after.encode())
        files.append(dict(source=source, before_path=(backup / Path(source).name).as_posix(),
                          before_bytes_sha256=sha(raw), after_source_sha256=sha(after.encode())))
        pending[source] = raw, after
    assert rows
    record = dict(rows=rows, files=files, index_sha256=index_sha,
                  source_state_sha256=sha(original_master), review_notes=args.note,
                  verification_policy='Affected-file hashes and exact token checks for alpha-only edits; focused review for structured edits. Build separately after the combined batch. Inventories and reccmp deferred to a larger checkpoint.',
                  scope='Readability cleanup of existing recovered definitions; no new native coverage or gameplay-equivalence claim.')
    for row in rows:
        proof['source_edits'].append(dict(source=row['source'], name=row['name'],
            before=row['before_definition'], after=row['after_definition'],
            before_sha256=row['before_sha256'], after_sha256=row['after_sha256'],
            card_control_cleanup_extension=leaf.as_posix(),
            reason=('Use equal native enum constants for dialog choices; preserve other implementation tokens.'
                    if row.get('native_enum_choice_uses') else
                    'Restore native local enum declarations and names; preserve implementation tokens.'
                    if row.get('local_enum_restoration') else
                    'Batched readability cleanup: exact alpha token checks and focused control-flow review.')))
    if any(row.get('local_enum_restoration') for row in rows):
        assert all(row.get('local_enum_restoration') for row in rows)
        proof['native_local_enum_restoration'] = leaf.as_posix()
    if any(row.get('native_enum_choice_uses') for row in rows):
        assert all(row.get('native_enum_choice_uses') for row in rows)
        proof.setdefault('native_local_enum_symbolic_batches', []).append(leaf.as_posix())
        if not any(item['path'] == 'tools/recovery/enum_choice_uses.py' for item in proof['interface_supporting_inputs']):
            proof['interface_supporting_inputs'].append(dict(path='tools/recovery/enum_choice_uses.py', sha256=sha(Path('tools/recovery/enum_choice_uses.py').read_bytes())))
    proof.setdefault('native_card_readability_batches', []).append(leaf.as_posix())
    record_bytes = (json.dumps(record, indent=2) + '\n').encode()
    proof['interface_supporting_inputs'].append(dict(path=leaf.as_posix(), sha256=sha(record_bytes)))
    registry = Path('tools/recovery/verify_call_condition_cleanup.py')
    registry_pin = next(v for v in proof['interface_supporting_inputs'] if v['path'] == registry.as_posix())
    registry_pin['sha256'] = sha(registry.read_bytes())
    for item in proof['interface_supporting_inputs']:
        if item['path'] in ['tools/recovery/verify_local_enum_values_recovery.py', 'tools/recovery/enum_choice_uses.py']:
            item['sha256'] = sha(Path(item['path']).read_bytes())
    assert master.read_bytes() == original_master
    assert sha(subprocess.check_output(['git', 'diff', '--cached', '--binary'])) == index_sha
    for source, (raw, _) in pending.items():
        assert Path(source).read_bytes() == raw
    backup.mkdir()
    for source, (raw, after) in pending.items():
        (backup / Path(source).name).write_bytes(raw)
        Path(source).write_bytes(after.replace('\n', '\r\n').encode('latin1'))
    leaf.write_bytes(record_bytes)
    with publication.open('w', encoding='utf-8', newline='\n') as stream:
        json.dump(proof, stream, indent=0)
        stream.write('\n')
    assert master.read_bytes() == original_master
    for attempt in range(20):
        try:
            publication.replace(master)
            break
        except PermissionError:
            if attempt == 19:
                raise
            time.sleep(.1)
    print(json.dumps(dict(definitions=len(rows), alpha_only=sum(v['alpha_only'] for v in rows),
        card_handlers=sum(v['name'].startswith('card_') for v in rows),
        **{k: sum(v['facts'][k] for v in rows) for k in rows[0]['facts']}), indent=2))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--name', required=True)
    parser.add_argument('--draft', action='append', required=True)
    parser.add_argument('--note', action='append', default=[])
    arguments = parser.parse_args()
    assert re.fullmatch(r'[a-z0-9-]+', arguments.name)
    apply(arguments)
