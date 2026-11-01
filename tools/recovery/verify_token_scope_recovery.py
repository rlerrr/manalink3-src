"""Verify native token scopes and an address-bound comparison name repair."""
import collections,json,sys
from pathlib import Path
from canonicalize_helpers import ROOT,read,span,sha
from inspect_release import Image
from token_scope_analysis import inspect,source_changes,NAMES
from verify_scoped_array_recovery import disjoint_reused_storage
from verify_array_role_recovery import replay


def validate(snapshot,row):
    name=row['name'];tokens=[v for v in snapshot['typed_records'] if v['bytes']==76]
    assert len(tokens)==(1 if name=='amass' else 2)
    tests=[v for v in snapshot['typed_records'] if v['bytes']==216];assert len(tests)==(1 if name in ['card_shark_typhoon','card_the_circle_of_loyalty'] else 0)
    disjoint_reused_storage(snapshot);facts=[]
    for t in tokens:
        assert t['name']=='token' and t['frame_offset']+76<=-8
        def calls(callee,arg):
            found=[c for c in snapshot['storage_calls'] if c['name']==callee and c['arguments'][arg]==['frame',t['frame_offset']+8]
                and any(a<=int(c['address'],16)<b for a,b in t['scope_ranges'])]
            assert len(found)==1,(name,callee,found)
            return found[0]
        initializer=calls('_default_token_definition',3);calls('_generate_token',0)
        assert initializer['arguments'][2] and initializer['arguments'][2][0]=='const'
        events=collections.Counter()
        for s in t['scalar_stores']:
            for b in range(s['size']):
                offset=s['offset']+b
                if 0<=offset<76:events[offset,None if s['value'] is None else s['value']>>(8*b)&255]+=1
        facts.append(dict(csvid=initializer['arguments'][2][2],events=[[o,v,n] for (o,v),n in sorted(events.items(),key=lambda x:(x[0][0],str(x[0][1])))]))
    return dict(tokens=sorted(facts,key=lambda t:json.dumps(t,sort_keys=True)),tests=[dict(name=t['name'],layout=t['layout'],parameter_events=t['parameter_events']) for t in tests])


def has_source_repair(proof,root,source,name,old_sha):
    path=proof.get('native_token_scope_recovery')
    if not path:return False
    record=json.loads((root/path).read_bytes());matches=[r for r in record['rows'] if (r['source'],r['name'],r['before_sha256'])==(source,name,old_sha)]
    if not matches:return False
    assert len(matches)==1;row=matches[0];changes,after=source_changes(name,row['before_definition'])
    assert changes==row['changes'] and after==row['after_definition'] and sha(row['before_definition'].encode())==old_sha
    t=read(root/source);a,b=span(t,name);assert t[a:b]==after and sha(after.encode())==row['after_sha256']
    edits=[e for e in proof['source_edits'] if e.get('native_token_scope_extension')==path and (e['source'],e['before_sha256'])==(source,old_sha)]
    assert len(edits)==1 and edits[0]['after_sha256']==row['after_sha256']
    return True


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    assert has_source_repair(proof,root,source,name,old_sha)
    record=json.loads((root/proof['native_token_scope_recovery']).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    row=next(r for r in record['rows'] if (r['source'],r['name'])==(source,name));view=row['native_snapshot']['view']
    lo,hi=int(view['address'],16),int(view['end'],16)
    assert sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo))==view['bytes_sha256']


def verify(proof,image,root=ROOT):
    path=proof.get('native_token_scope_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(nd.data)==record['native_dbg_sha256']
    rows=record['rows'];assert len(rows)==4 and {r['name'] for r in rows}==NAMES;native=inspect(image,nd,rows);builds=[]
    for label in ['before','after']:
        a=record['runtime_'+label];im=Image((root/a['dll_path']).read_bytes());dbg=Image((root/a['dbg_path']).read_bytes())
        assert sha(im.data)==a['dll_sha256'] and sha(dbg.data)==a['dbg_sha256'];builds.append(inspect(im,dbg,rows))
    current=inspect(Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()),rows)
    for row in rows:
        key=row['source'],row['name'];assert native[key]==row['native_snapshot']
        assert builds[0][key]==row['before_compiled'] and builds[1][key]==row['after_compiled']
        left=validate(native[key],row);assert left==row['native_validation']==row['compiled_validation']
        assert validate(builds[1][key],row)==validate(current[key],row)==left
        assert sha(row['before_definition'].encode())==row['before_sha256']
        changes,after=source_changes(row['name'],row['before_definition']);assert changes==row['changes'] and after==row['after_definition']==replay(row['before_definition'],changes)
        assert has_source_repair(proof,root,*key,row['before_sha256'])
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+r['source'][:-2]+'.o' for r in rows};assert {p for p in before if before[p]!=after[p]}==expected
    # Exercise the actual project binding function on the real native callback
    # and its colliding old spelling; the complete setup later consumes all rows.
    sys.path.insert(0,str(root/'tools'));from reccmp_project import bind_native_function_names
    mapping=record['comparison_name_repair'];aux=next(e for e in proof['auxiliary_functions'] if e.get('native_address')=='2073940')
    assert aux['source']=='cards/innistrad.c' and aux['canonical_name']=='innistrad_exile_at_end_of_combat_legacy'
    row=dict(address=0x2073940,name='exile_at_end_of_combat',source='cards/innistrad.c')
    bind_native_function_names([row],{}, {},{},[aux]);assert row['match_name']==aux['canonical_name'] and row['mapping']=='verified auxiliary native binding'
    assert sha((root/'tools/reccmp_project.py').read_bytes())==mapping['tool_after_sha256']
    assert sha((root/mapping['csv_before_path']).read_bytes())==mapping['csv_before_sha256']
    # Do not pin the whole generated CSV here; setup regenerates it after the
    # canonical audit. Its one explicit native binding is checked independently.
    import csv
    current_csv=list(csv.DictReader((root/'config/reccmp/functions.csv').read_text(encoding='utf8').splitlines()))
    entries=[r for r in current_csv if r['address']=='0x2073940'];assert len(entries)==1
    assert entries[0]['name']==aux['canonical_name'] and entries[0]['source']==aux['source']
    return dict(restored_source_functions=4,card_handlers=3,helpers=1,restored_scoped_tokens=7,restored_scoped_tests=2,
        removed_source_unions=2,typed_vigilance_writes=2,repaired_comparison_bindings=1,
        changed_linker_objects=4,unchanged_linker_objects=len(before)-4,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
