"""Extend the checked canonical projection with reviewed helpers, without aliases.

The existing canonical files must match their checkpoint exactly. New entries
are checked against the accepted recovery records and the archived native C
bodies. Const annotations affect declaration heads only. No compiler worker
may replace an independently recovered canonical frontend.
"""
import argparse,json,re,shutil,zipfile
from pathlib import Path
from canonicalize_helpers import ROOT,PLAN,CANDIDATE,EVIDENCE,BEFORE,read,sha,span,project_file,recipe_sha,declarations
from audit_source import function_body

CHECKPOINT=Path('.recovery/helper-canonicalization-541.json')
SELECTION=Path('.recovery/helper-batch-selection.json')
BATCH_PLAN=Path('.recovery/helper-canonical-batch-plan.json')
BATCH_ARCHIVE=Path('.recovery/helper-canonical-batch-before.zip')

def check_body(native,f):
    if 'reconstruction_body_sha256' in f:
        assert sha(function_body(native,f['name']).encode())==f['reconstruction_body_sha256'],f['name']
        return
    # Early records used complete reconstruction recipes rather than a final
    # body digest. Their original recipes still run in the full source audit.
    assert f['name'] in {'counter_number_as_cost_2022','count_zone_by_type_2022'}
    raw=(ROOT/'.recovery/decompiled-abi'/f"{f['address']}.c").read_text()
    assert sha(raw.encode())==f['raw_sha256']
    audit=json.loads((ROOT/'.recovery/recovery-audit.json').read_text())
    assert not audit['issues'] and audit['canonical_helpers']['canonical_functions']>=541

def prepare(repair=False):
    previous=json.loads((ROOT/CHECKPOINT).read_text())
    live=json.loads((ROOT/EVIDENCE).read_text())
    if not repair:assert (ROOT/EVIDENCE).read_bytes()==(ROOT/CHECKPOINT).read_bytes()
    assert sha((ROOT/BEFORE).read_bytes())==live['predecessor_archive_sha256']
    for row in live['files']:
        assert sha(read(ROOT/row['source']).encode())==row['canonical_sha256'],row['source']
    with zipfile.ZipFile(ROOT/BEFORE) as archive:base={name:archive.read(name) for name in archive.namelist()}
    paths=set()
    for folder in ['cards','functions']:
        paths.update(p for p in (ROOT/folder).rglob('*') if p.is_file() and p.suffix.lower() in {'.c','.h','.cpp'})
    paths.update(p for p in ROOT.iterdir() if p.is_file() and p.suffix.lower() in {'.c','.h','.cpp'})
    for path in paths:base.setdefault(path.relative_to(ROOT).as_posix(),path.read_bytes())
    ledger=json.loads((ROOT/'docs/recovery-evidence/recovered-functions.json').read_text())
    accepted={f['name']:f for f in ledger['accepted_functions']}
    selection=json.loads((ROOT/SELECTION).read_text())
    entries=list(previous['functions']);added=[]
    for row in selection['replacements']:
        f=accepted[row['recovered_name']];source=f['source'];text=base[source].decode('latin1').replace('\r\n','\n')
        old=row['retained_name'];new=f['name'];a=span(text,old);b=span(text,new);assert a and b,(old,new)
        assert old not in accepted or accepted[old]['address']==f['address'],('Separate native frontend must survive',old)
        native=text[b[0]:b[1]];retained=text[a[0]:a[1]]
        check_body(native,f)
        entry=dict(source=source,canonical_name=old,recovery_name=new,native_address=f['address'],static=f['static'],
            source_prototype=row['source_prototype'],recovery_prototype=f['prototype'],
            accepted_recovery_record_sha256=sha(json.dumps(f,sort_keys=True).encode()),
            live_definition_sha256=sha(native.encode()),retained_definition_sha256=sha(retained.encode()),
            cleanup_retained_declarations=True,keep_native_order=True,allow_compiler_clone=f['static'],
            const_parameters=[n for n in row['annotations'] if n!='return'],const_return_annotation='return' in row['annotations'],
            origin='Reviewed matching native interface, with explicit declaration-only const annotations where recorded')
        if row.get('return_change'):entry['return_change']=row['return_change']
        if new=='target_must_be_permanent_or_suspend_and_have_time_counters_2022':entry['const_locals']=['pcVar1']
        entries.append(entry);added.append(entry)
    for new,old in selection['worker_renames']:
        f=accepted[new];text=base[f['source']].decode('latin1').replace('\r\n','\n');b=span(text,new);assert b
        native=text[b[0]:b[1]]
        check_body(native,f)
        assert span(text,old) is None,('Worker rename collides with a definition',old)
        entry=dict(source=f['source'],canonical_name=old,recovery_name=new,native_address=f['address'],static=f['static'],
            source_prototype=f['prototype'],recovery_prototype=f['prototype'],rename_only=True,allow_compiler_clone=True,
            accepted_recovery_record_sha256=sha(json.dumps(f,sort_keys=True).encode()),live_definition_sha256=sha(native.encode()),
            origin='Native private worker retained independently; remove recovery-year spelling without collapsing its frontend')
        entries.append(entry);added.append(entry)
    assert len(entries)==len({e['canonical_name'] for e in entries})==len({e['recovery_name'] for e in entries})
    mapping={e['recovery_name']:e['canonical_name'] for e in entries}
    sharing=json.loads((ROOT/'docs/recovery-evidence/helper-unification.json').read_text())
    annotations=[]
    f=accepted['aura_impl'];text=base[f['source']].decode('latin1').replace('\r\n','\n');b=span(text,f['name']);assert b
    native=text[b[0]:b[1]];check_body(native,f)
    annotations.append(dict(source=f['source'],canonical_name=f['name'],recovery_name=f['name'],const_parameters=['prompt'],
        live_definition_sha256=sha(native.encode()),accepted_recovery_record_sha256=sha(json.dumps(f,sort_keys=True).encode()),
        reason='Private Aura selector only forwards the prompt to the read-only target selector; declaration qualifier only.'))
    source='cards/onslaught.c';text=base[source].decode('latin1').replace('\r\n','\n');b=span(text,'target_is_attached_to_creature');assert b
    retirements=[dict(source=source,name='target_is_attached_to_creature',definition_sha256=sha(text[b[0]:b[1]].encode()),
        reason='Same native callback as the canonical Urza Legacy implementation; preserve the callback reference and remove the private copy.')]
    from inspect_release import Image
    from helper_fingerprints import LocalCode
    native=Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes());native.pe.parse_data_directories()
    code=LocalCode(native.pe,{a:{a} for a in [0x26b2ab0,0x2681ca0]})
    fp=code.fingerprint(0x25b2290,76,[b'attached to a creature']);other=code.fingerprint(0x25e73f0,76,[b'attached to a creature'])
    assert not fp.issues and not other.issues and fp.instructions==other.instructions
    equivalents=[dict(canonical_address='0x25b2290',duplicate_address='0x25e73f0',bytes=76,
        canonical_source='cards/urza_legacy.c',duplicate_source=source,canonical_name='target_is_attached_to_creature',
        canonical_sha256=sha(native.pe.get_data(0x25b2290-0x2000000,76)),duplicate_sha256=sha(native.pe.get_data(0x25e73f0-0x2000000,76)),
        callees=['0x26b2ab0','0x2681ca0'],readonly_strings=['attached to a creature'],fingerprint_sha256=fp.sha256)]
    changed=[];candidate=ROOT/CANDIDATE
    for path in sorted(paths):
        source=path.relative_to(ROOT).as_posix();before=base[source].decode('latin1').replace('\r\n','\n')
        after=project_file(source,before,entries,mapping,sharing,annotations,retirements)
        if after==before:continue
        target=candidate/source;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(after.replace('\n','\r\n').encode('latin1'))
        changed.append(dict(source=source,predecessor_sha256=sha(before.encode()),canonical_sha256=sha(after.encode()),
                            previous_live_sha256=sha(read(path).encode())))
    with zipfile.ZipFile(ROOT/BATCH_ARCHIVE,'w',zipfile.ZIP_DEFLATED) as archive:
        for row in changed:archive.writestr(row['source'],base[row['source']])
    result={**previous,'format':2,'functions':entries,'source_annotations':annotations,'source_retirements':retirements,'native_equivalences':equivalents,'files':changed,'status':'reviewed extension candidate; C untouched',
        'live_checkpoint_sha256':sha((ROOT/EVIDENCE).read_bytes()),
        'additional_direct_replacements':previous['additional_direct_replacements']+len(selection['replacements']),
        'recipe_sha256':recipe_sha(),'predecessor_archive_sha256':sha((ROOT/BATCH_ARCHIVE).read_bytes()),
        'previous_projection':dict(checkpoint=str(CHECKPOINT),sha256=sha((ROOT/CHECKPOINT).read_bytes()),recipe_sha256=previous['recipe_sha256']),
        'extension':dict(replacements=len(selection['replacements']),worker_names=len(selection['worker_renames']),
                         functions=[e['recovery_name'] for e in added],selection_sha256=sha((ROOT/SELECTION).read_bytes())),
        'scope':'Canonical native helpers; immutable accepted records; checked qualifier-only declaration heads; distinct native frontends and compiler workers preserved.'}
    (ROOT/BATCH_PLAN).write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(total=len(entries),replacements=len(selection['replacements']),worker_names=len(selection['worker_renames']),files=len(changed)),indent=2))

def apply():
    proof=json.loads((ROOT/BATCH_PLAN).read_text());assert proof['recipe_sha256']==recipe_sha()
    assert sha((ROOT/EVIDENCE).read_bytes())==proof['live_checkpoint_sha256']
    assert sha((ROOT/BATCH_ARCHIVE).read_bytes())==proof['predecessor_archive_sha256']
    backup=ROOT/('.recovery/canonical-helper-batch-live-before-'+proof['live_checkpoint_sha256'][:12]);assert not backup.exists();backup.mkdir()
    for row in proof['files']:
        source=row['source'];assert sha(read(ROOT/source).encode())==row['previous_live_sha256'],source
        assert sha(read(ROOT/CANDIDATE/source).encode())==row['canonical_sha256'],source
        target=backup/source;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(ROOT/source,target)
    for row in proof['files']:shutil.copyfile(ROOT/CANDIDATE/row['source'],ROOT/row['source'])
    shutil.copyfile(ROOT/BATCH_ARCHIVE,ROOT/BEFORE)
    proof['status']='applied'
    (ROOT/EVIDENCE).write_text(json.dumps(proof,indent=2)+'\n')
    from canonicalize_helpers import verify
    print(json.dumps(verify(),indent=2))

if __name__=='__main__':
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--repair',action='store_true');args=parser.parse_args()
    if args.apply:apply()
    else:prepare(args.repair)
