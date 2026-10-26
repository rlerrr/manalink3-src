"""Stage a reviewed interface-consolidation batch without rebuilding the project.

Inputs record exact native bodies, removed predecessor bodies and explicit
caller/domain edits. Generate and syntax-check the candidate before --apply.
The expensive full recovery/build/comparison audits run after consolidation.
"""
import argparse,json,shutil,zipfile
from pathlib import Path
from canonicalize_helpers import ROOT,EVIDENCE,BEFORE,CANDIDATE,read,sha,span,project_file,recipe_sha,verify_auxiliary_functions
from audit_source import function_body

CHECKPOINT=Path('.recovery/helper-canonicalization-587.json')
SELECTION=Path('.recovery/helper-interface-selection.json')
PLAN=Path('.recovery/helper-interface-plan.json')
ARCHIVE=Path('.recovery/helper-interface-before.zip')

def prepare():
    previous=json.loads((ROOT/CHECKPOINT).read_text())
    assert (ROOT/EVIDENCE).read_bytes()==(ROOT/CHECKPOINT).read_bytes()
    assert sha((ROOT/BEFORE).read_bytes())==previous['predecessor_archive_sha256']
    for row in previous['files']:
        assert sha(read(ROOT/row['source']).encode())==row['canonical_sha256'],row['source']
    with zipfile.ZipFile(ROOT/BEFORE) as archive:
        base={name:archive.read(name) for name in archive.namelist()}
    paths=set()
    for folder in ['cards','functions']:
        paths.update(p for p in (ROOT/folder).rglob('*') if p.is_file() and p.suffix.lower() in {'.c','.h','.cpp'})
    paths.update(p for p in ROOT.iterdir() if p.is_file() and p.suffix.lower() in {'.c','.h','.cpp'})
    for path in paths:base.setdefault(path.relative_to(ROOT).as_posix(),path.read_bytes())
    ledger=json.loads((ROOT/'docs/recovery-evidence/recovered-functions.json').read_text())
    accepted={f['name']:f for f in ledger['accepted_functions']}
    selection=json.loads((ROOT/SELECTION).read_text())
    # Explicit non-C edits, such as retiring an EXE-address linker assignment,
    # participate in the same pinned preimage/projection proof as C sources.
    for edit in previous.get('source_edits',[])+selection.get('source_edits',[]):
        path=(ROOT/edit['source']).resolve()
        assert path.is_relative_to(ROOT.resolve()) and path.is_file(),edit['source']
        paths.add(path);base.setdefault(edit['source'],path.read_bytes())
    entries=list(previous['functions']);added=[]
    for row in selection['functions']:
        f=accepted[row['recovered_name']];source=f['source'];text=base[source].decode('latin1').replace('\r\n','\n')
        old=row['canonical_name'];new=f['name'];b=span(text,new);assert b,new
        native=text[b[0]:b[1]]
        if 'reconstruction_body_sha256' in f:
            assert sha(function_body(native,new).encode())==f['reconstruction_body_sha256'],new
        else:
            # Early accepted records pin raw exports and reconstruction recipes
            # instead of a final body digest. Their source was fully audited at
            # the 587 checkpoint; retain the archived body verbatim.
            assert source=='functions/counters.c' or new in {
                'cascade_2022','beginning_of_combat_2022','this_dies_trigger_impl_2022','lobotomy_effect_2022',
                'is_humiliated_for_ability',
                'target_is_token_too_2022','cast_historic_spell_2022',
                'basic_landcycling_2022','locate_edh_card_2022',
                'generic_split_card_2022',
                'leaves_play_2022','other_leaves_play_2022','get_subtypes_by_csvid_2022','has_subtype_by_id_2022'}
            raw=ROOT/'.recovery/decompiled-abi'/f"{f['address']}.c"
            assert sha(raw.read_text().encode())==f['raw_sha256'],new
            for support in f.get('supporting_exports',[]):
                path=ROOT/'.recovery/decompiled-abi'/f"{support['address']}.c"
                assert sha(path.read_text().encode())==support['raw_sha256'],(new,support['address'])
        e=dict(source=source,canonical_name=old,recovery_name=new,native_address=f['address'],static=f['static'],
               source_prototype=row['prototype'],recovery_prototype=f['prototype'],
               accepted_recovery_record_sha256=sha(json.dumps(f,sort_keys=True).encode()),
               live_definition_sha256=sha(native.encode()),cleanup_retained_declarations=True,
               keep_native_order=True,allow_compiler_clone=f['static'],origin=row['reason'])
        if row.get('rename_only'):
            assert span(text,old) is None,old
            e['rename_only']=True
        elif row.get('alias_source_replacement'):
            replacement=row['alias_source_replacement']
            assert text.count(replacement)==1,(source,old)
            e['alias_source_replacement']=replacement
            e['retained_definition_sha256']=sha(replacement.encode())
        else:
            assert old not in accepted,('Independent accepted native frontend',old)
            a=span(text,old);assert a,old
            e['retained_definition_sha256']=sha(text[a[0]:a[1]].encode())
            if row.get('defer_retained_removal'):
                live=read(ROOT/source);bounds=span(live,old);assert bounds
                retained=live[bounds[0]:bounds[1]]
                e.update(defer_retained_removal=True,post_edit_retained_definition=retained,
                         post_edit_retained_definition_sha256=sha(retained.encode()))
        if row.get('const_parameters'):e['const_parameters']=row['const_parameters']
        if row.get('no_forward_declaration'):e['no_forward_declaration']=True
        entries.append(e);added.append(e)
    auxiliary=list(previous.get('auxiliary_functions',[]))
    for row in selection.get('auxiliary_functions',[]):
        source=row['source'];text=base[source].decode('latin1').replace('\r\n','\n')
        bounds=span(text,row['recovery_name']);assert bounds,row['recovery_name']
        auxiliary.append({**row,'live_definition_sha256':sha(text[bounds[0]:bounds[1]].encode()),'rename_only':True})
    projected_entries=entries+auxiliary
    assert len(projected_entries)==len({e['canonical_name'] for e in projected_entries})==len({e['recovery_name'] for e in projected_entries})
    mapping={e['recovery_name']:e['canonical_name'] for e in projected_entries}
    sharing=json.loads((ROOT/'docs/recovery-evidence/helper-unification.json').read_text())
    retirements=list(previous.get('source_retirements',[]))
    for row in selection.get('retirements',[]):
        text=base[row['source']].decode('latin1').replace('\r\n','\n');a=span(text,row['name']);assert a
        retirements.append({**row,'definition_sha256':sha(text[a[0]:a[1]].encode())})
    edits=list(previous.get('source_edits',[]))+selection.get('source_edits',[])
    for e in edits:
        assert sha(e['before'].encode())==e['before_sha256']
        assert sha(e['after'].encode())==e['after_sha256']
    changed=[]
    for path in sorted(paths):
        source=path.relative_to(ROOT).as_posix();before=base[source].decode('latin1').replace('\r\n','\n')
        after=project_file(source,before,projected_entries,mapping,sharing,previous.get('source_annotations',[]),retirements,edits)
        if after==before:continue
        target=ROOT/CANDIDATE/source;target.parent.mkdir(parents=True,exist_ok=True)
        target.write_bytes(after.replace('\n','\r\n').encode('latin1'))
        changed.append(dict(source=source,predecessor_sha256=sha(before.encode()),canonical_sha256=sha(after.encode()),
                            previous_live_sha256=sha(read(path).encode())))
    with zipfile.ZipFile(ROOT/ARCHIVE,'w',zipfile.ZIP_DEFLATED) as archive:
        for row in changed:archive.writestr(row['source'],base[row['source']])
    proof={**previous,'functions':entries,'auxiliary_functions':auxiliary,'source_retirements':retirements,'source_edits':edits,'files':changed,
           'recipe_sha256':recipe_sha(),'predecessor_archive_sha256':sha((ROOT/ARCHIVE).read_bytes()),
           'status':'reviewed interface candidate; C untouched',
           'live_checkpoint_sha256':sha((ROOT/EVIDENCE).read_bytes()),
           'additional_direct_replacements':previous['additional_direct_replacements']+sum(not e.get('rename_only') for e in added),
           'interface_extension':dict(functions=[e['recovery_name'] for e in added],selection_sha256=sha((ROOT/SELECTION).read_bytes()),
                                      source_edits_added=len(selection.get('source_edits',[])),
                                      previous_checkpoint=str(CHECKPOINT),previous_checkpoint_sha256=sha((ROOT/CHECKPOINT).read_bytes())),
           'native_domain_evidence':dict(path='docs/recovery-evidence/canonical-helper-domains.json',
                                        sha256=sha((ROOT/'docs/recovery-evidence/canonical-helper-domains.json').read_bytes()))}
    proof['interface_supporting_inputs']=previous.get('interface_supporting_inputs',[])+selection.get('evidence_receipts',[])
    spellings={**previous.get('source_spelling_names',{}),**selection.get('source_spelling_names',{})}
    assert not (set(spellings)&set(mapping)), 'Spelling cleanup must not replace function identities'
    if spellings:proof['source_spelling_names']=spellings
    domains=previous.get('source_domain_migrations',[])+selection.get('source_domain_migrations',[])
    if domains:proof['source_domain_migrations']=domains
    verify_auxiliary_functions(proof)
    if selection.get('caller_migration'):proof['caller_migration']=selection['caller_migration']
    (ROOT/PLAN).write_text(json.dumps(proof,indent=2)+'\n')
    print(json.dumps(dict(canonical_functions=len(entries),added=len(added),retirements=len(retirements),source_edits=len(edits),files=len(changed))))

def apply():
    proof=json.loads((ROOT/PLAN).read_text())
    assert proof['recipe_sha256']==recipe_sha()
    assert sha((ROOT/EVIDENCE).read_bytes())==proof['live_checkpoint_sha256']
    assert sha((ROOT/ARCHIVE).read_bytes())==proof['predecessor_archive_sha256']
    assert sha((ROOT/SELECTION).read_bytes())==proof['interface_extension']['selection_sha256']
    backup=ROOT/('.recovery/helper-interface-live-before-'+proof['live_checkpoint_sha256'][:12])
    assert not backup.exists();backup.mkdir()
    for row in proof['files']:
        source=row['source'];assert sha(read(ROOT/source).encode())==row['previous_live_sha256'],source
        assert sha(read(ROOT/CANDIDATE/source).encode())==row['canonical_sha256'],source
        target=backup/source;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(ROOT/source,target)
    for row in proof['files']:shutil.copyfile(ROOT/CANDIDATE/row['source'],ROOT/row['source'])
    shutil.copyfile(ROOT/ARCHIVE,ROOT/BEFORE)
    proof['status']='applied; full verification deferred until interface consolidation is finished'
    (ROOT/EVIDENCE).write_text(json.dumps(proof,indent=2)+'\n')
    print(json.dumps(dict(applied=len(proof['interface_extension']['functions']),caller_edits=proof['interface_extension'].get('source_edits_added',0),canonical_functions=len(proof['functions']))))

if __name__=='__main__':
    p=argparse.ArgumentParser(__doc__);p.add_argument('--apply',action='store_true')
    p.add_argument('--checkpoint',type=Path,default=CHECKPOINT)
    p.add_argument('--selection',type=Path,default=SELECTION)
    p.add_argument('--plan',type=Path,default=PLAN)
    p.add_argument('--archive',type=Path,default=ARCHIVE);args=p.parse_args()
    CHECKPOINT,SELECTION,PLAN,ARCHIVE=args.checkpoint,args.selection,args.plan,args.archive
    apply() if args.apply else prepare()
