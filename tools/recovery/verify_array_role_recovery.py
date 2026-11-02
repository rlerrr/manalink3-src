"""Verify native draw/selection matrices, prompt copy and sacrifice arrays."""
import json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from verify_additional_local_names import source_hash_matches
from inspect_release import Image
from array_role_analysis import snapshots
from verify_wide_stack_test_recovery import symbol_at


ROLES={
    'card_soldevi_sage':('dtt',[2,100],4,5),
    'card_krovikan_sorcerer':('dtt',[2,100],4,5),
    'card_aminatou_the_fateshifter':('marked',[2,100],4,5),
    'card_liliana_dreadhorde_general':('marked',[2,151],1,6),
    'card_ib_halfheart_goblin_tactician':('sacs',[2],4,5),
    'card_bolass_citadel':('sac',[10],4,5),
}


def variable(snapshot,name):
    found=[v for v in snapshot['variables'] if v['name']==name]
    assert found and all(v['shape']==found[0]['shape'] and v['frame_offset']==found[0]['frame_offset'] for v in found)
    return found[0]


def footprint(snapshot,frame,payload):
    candidates=[w for w in snapshot['effects']['writes'] if frame<=w['frame_offset'] and w['frame_offset']+w['size']<=frame+len(payload)]
    covered={};matching=[]
    for w in candidates:
        data=bytes.fromhex(w['bytes']);assert len(data)==w['size']
        if data!=payload[w['frame_offset']-frame:w['frame_offset']-frame+len(data)]:continue
        matching.append(w)
        for b,val in enumerate(data):covered[w['frame_offset']-frame+b]=val
    assert covered==dict(enumerate(payload)),(frame,len(payload),len(covered))
    if not any(payload):assert any(w['size']>=len(payload)-4 for w in matching)
    return dict(bytes=len(payload),sites=[w['address'] for w in matching])


def validate(snapshot,row,image,debug):
    name=row['name'];result={}
    if name in ROLES:
        var,dims,element_size,encoding=ROLES[name];v=variable(snapshot,var);n=element_size
        for d in dims:n*=d
        assert v['shape']==dict(kind='DW_TAG_array_type',size=n,dimensions=dims,element_size=element_size,element_encoding=encoding)
        assert v['frame_offset']+n<=-8
        result['array']=dict(name=var,frame_offset=v['frame_offset'],bytes=n,dimensions=dims)
        if name in ['card_soldevi_sage','card_krovikan_sorcerer','card_liliana_dreadhorde_general','card_bolass_citadel']:
            result['clear']=footprint(snapshot,v['frame_offset'],b'\0'*n)
        if name in ['card_soldevi_sage','card_krovikan_sorcerer']:
            calls=[c for c in snapshot['effects']['calls'] if c['name']=='_select_card_from_zone']
            assert any(c['arguments'][2]==['frame',v['frame_offset']+8] for c in calls)
            indexed=snapshot['effects']['indexed_accesses']
            for offset in [v['frame_offset'],v['frame_offset']+400]:
                assert any(a['frame_offset']==offset and a['scale']==4 and a['size']==4 and a['access']&2 for a in indexed),(name,offset)
            assert any(a['frame_offset']==v['frame_offset']+400 and a['scale']==4 and a['size']==4 and a['access']&1 for a in indexed)
            result['draw_rows']=dict(iid_offset=0,hand_index_offset=400,entry_bytes=4,select_uses_first_row=True)
        if name=='card_liliana_dreadhorde_general':
            test=variable(snapshot,'this_test');pointers=snapshot['effects']['frame_pointer_stores']
            assert any(p['frame_offset']==test['frame_offset']+200 and p['target_frame_offset']==v['frame_offset'] for p in pointers)
            stores=[w for w in snapshot['effects']['writes'] if w['frame_offset']==test['frame_offset']+196 and w['size']==4]
            assert len(stores)==1 and symbol_at(image,debug,int.from_bytes(bytes.fromhex(stores[0]['bytes']),'little'))=='_not_marked'
            result['callback']=dict(name='_not_marked',matrix_argument_bytes=302,rows=2,stride=151)
    else:
        assert name=='card_the_mimeoplasm';test=variable(snapshot,'test')
        assert test['shape']['message']==dict(offset=0,size=100)
        result['prompt_copy']=footprint(snapshot,test['frame_offset'],b'Select a creature card for counters.\0')
        assert result['prompt_copy']['bytes']==37
    return result


def replay(text,changes):
    for change in changes:
        assert text.count(change['before'])==change['occurrences']
        text=text.replace(change['before'],change['after'])
    return text


def verify(proof,image,root=ROOT):
    path=proof.get('native_array_role_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    rows=record['rows'];assert len(rows)==7 and {r['name'] for r in rows}==set(ROLES)|{'card_the_mimeoplasm'}
    native=snapshots(image,debug,rows)
    artifacts=[]
    for label in ['before','after']:
        artifact=record['runtime_'+label]
        im=Image((root/artifact['dll_path']).read_bytes());dbg=Image((root/artifact['dbg_path']).read_bytes())
        assert sha(im.data)==artifact['dll_sha256'] and sha(dbg.data)==artifact['dbg_sha256']
        artifacts.append((im,dbg,snapshots(im,dbg,rows)))
    current=Image((root/'ManalinkEh.dll').read_bytes());cd=Image((root/'ManalinkEh.dbg').read_bytes());fresh=snapshots(current,cd,rows)
    for row in rows:
        key=(row['source'],row['name']);assert native[key]==row['native_snapshot']
        assert artifacts[0][2][key]==row['before_compiled'] and artifacts[1][2][key]==row['after_compiled']
        assert validate(native[key],row,image,debug)==row['native_validation']
        assert validate(artifacts[1][2][key],row,*artifacts[1][:2])==row['compiled_validation']
        validate(fresh[key],row,current,cd)
        assert replay(row['before_definition'],row['changes'])==row['after_definition']
        assert sha(row['before_definition'].encode())==row['before_sha256']
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert source_hash_matches(proof,root,row['source'],row['name'],sha(text[a:b].encode()),row['after_sha256']) and sha(row['after_definition'].encode())==row['after_sha256']
        assert '(test_definition_t *)' not in row['after_definition']
        edits=[e for e in proof['source_edits'] if e.get('native_array_role_extension')==path and e['source']==row['source'] and e['before_sha256']==row['before_sha256']]
        assert len(edits)==1 and edits[0]['after_sha256']==row['after_sha256']
        name=row['name']
        if name in ['card_soldevi_sage','card_krovikan_sorcerer','card_aminatou_the_fateshifter','card_liliana_dreadhorde_general']:
            role=ROLES[name][0];assert not any(v['name']==role for v in row['before_compiled']['variables'])
        if name=='card_ib_halfheart_goblin_tactician':
            assert row['after_definition'].count('for (int i = 0; i < 2; ++i)')==1
        if name=='card_bolass_citadel':
            assert row['after_definition'].count('for (int i = 0; i < 10; ++i)')==3
            assert row['after_definition'].count('spell_fizzled = 1;')==row['before_definition'].count('spell_fizzled = 1;')
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+r['source'][:-2]+'.o' for r in rows}
    assert {p for p in before if before[p]!=after[p]}==expected
    return dict(recovered_card_definitions=7,restored_large_arrays=4,restored_matrix_bytes=[800,800,800,302],
        fixed_clear_footprints=[800,800,302,40],prompt_copy_bytes=37,native_sacrifice_capacities=[2,10],
        changed_linker_objects=6,unchanged_linker_objects=len(before)-6,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
