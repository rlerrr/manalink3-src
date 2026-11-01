"""Verify six scoped prompt/output/matrix reconstructions against native code."""
import collections,json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from scoped_array_analysis import inspect,source_changes,NAMES
from verify_array_role_recovery import variable,footprint,replay


PROMPTS={'card_tezzeret_the_seeker':500,'do_multicascade':1000,'card_inniaz_the_gale_force':150}
TESTS={'card_tezzeret_the_seeker':2,'do_multicascade':1,'card_inniaz_the_gale_force':2,
       'card_tevesh_szat_doom_of_fools':1,'card_nevinyrral_urborg_tyrant':1,'card_go_to_jail':0}


def array(snapshot,name,dims,width,encoding):
    v=variable(snapshot,name);size=width
    for d in dims:size*=d
    assert v['shape']==dict(kind='DW_TAG_array_type',size=size,dimensions=dims,element_size=width,element_encoding=encoding)
    assert v['frame_offset']+size<=-8
    return v


def records(snapshot):
    tests=[v for v in snapshot['typed_records'] if v['bytes']==216]
    assert len({v['role'] for v in tests})==len(tests)
    return {v['role']:v for v in tests}


def frame_call(snapshot,name,arg,frame):
    found=[c for c in snapshot['storage_calls'] if c['name']==name and c['arguments'][arg]==['frame',frame+8]]
    assert len(found)==1,(name,arg,frame,found)
    return found[0]


def disjoint_reused_storage(snapshot):
    variables=snapshot['variables'];pairs=0
    for j,a in enumerate(variables):
        for b in variables[:j]:
            if max(a['frame_offset'],b['frame_offset'])>=min(a['frame_offset']+a['shape']['size'],b['frame_offset']+b['shape']['size']):continue
            assert not any(max(lo,x)<min(hi,y) for lo,hi in a['scope_ranges'] for x,y in b['scope_ranges']),(a,b)
            pairs+=1
    return pairs


def initial_bytes(snapshot,var,payload):
    covered={};frame=var['frame_offset']
    for w in snapshot['effects']['writes']:
        relative=w['frame_offset']-frame
        if not 0<=relative<=len(payload)-w['size']:continue
        if not any(lo<=int(w['address'],16)<hi for lo,hi in var['scope_ranges']):continue
        data=bytes.fromhex(w['bytes'])
        if data!=payload[relative:relative+len(data)]:continue
        for j,b in enumerate(data):covered[relative+j]=b
    assert covered==dict(enumerate(payload))


def validate(snapshot,row):
    name=row['name'];tests=records(snapshot);assert len(tests)==TESTS[name]
    for test in tests.values():assert test['frame_offset']+216<=-8
    disjoint_reused_storage(snapshot)
    result=dict(scoped_tests=len(tests),test_parameter_events={k:v['parameter_events'] for k,v in tests.items()})
    if name in PROMPTS:
        size=PROMPTS[name];buffer=array(snapshot,'buffer',[size],1,6);frame=buffer['frame_offset']
        formatters=[c for c in snapshot['storage_calls'] if c['name']=='_scnprintf']
        assert len(formatters)==(2 if size==150 else 1)
        for c in formatters:
            assert c['arguments'][0]==['frame',frame+8] and c['arguments'][1]==['const',0xffffffff,size]
            assert any(lo<=int(c['address'],16)<hi for lo,hi in buffer['scope_ranges'])
        if size==500:
            consumer=frame_call(snapshot,'_new_default_test_definition',2,frame)
            target=tests['_new_default_test_definition']['frame_offset']
            assert consumer['arguments'][0]==['frame',target+8]
            frame_call(snapshot,'_new_global_tutor',6,target)
        elif size==1000:
            frame_call(snapshot,'_do_dialog',5,frame)
            target=tests['_new_default_test_definition']['frame_offset'];frame_call(snapshot,'_select_card_from_zone',7,target)
            exiled=array(snapshot,'exiled',[500],4,5);frame_call(snapshot,'_select_card_from_zone',2,exiled['frame_offset'])
        else:
            frame_call(snapshot,'_new_pick_target',1,frame)
            assert any(w['frame_offset']==frame and w['size']==1 and w['bytes']=='00' and any(lo<=int(w['address'],16)<hi for lo,hi in buffer['scope_ranges']) for w in snapshot['effects']['writes'])
        result.update(prompt_bytes=size,formatters=len(formatters),prompt_consumers=1)
    elif name=='card_tevesh_szat_doom_of_fools':
        rv=array(snapshot,'rv',[3],4,5);frame_call(snapshot,'_put_all_commanders_from_command_zone_to',2,rv['frame_offset'])
        initial_bytes(snapshot,rv,b'\xff'*12)
        flow=snapshot['output_word_flow'];assert flow
        assert sorted(c['card_argument'][1] for c in flow['gain_control_calls'] if c['card_argument'])==[4,8]
        result.update(output_elements=3,initialized_bytes=12,gain_control_array_offsets=[4,8])
    elif name=='card_nevinyrral_urborg_tyrant':
        totals=array(snapshot,'totals',[2,2],4,5);initial_bytes(snapshot,totals,b'\0'*16)
        flow=snapshot['matrix_updates'];assert flow
        assert collections.Counter((r['column'],r['operation']) for r in flow)==collections.Counter({(0,'count_plus_one'):2,(4,'cmc_sum'):2})
        result.update(matrix_dimensions=[2,2],matrix_bytes=16,verified_count_updates=2,verified_cmc_sums=2)
    else:
        assert name=='card_go_to_jail';r=array(snapshot,'r',[2],4,5)
        call=frame_call(snapshot,'_roll_2d6t',5,r['frame_offset']);assert call['arguments'][6]==['frame',r['frame_offset']+12]
        flow=snapshot['output_word_flow'];assert flow and [c['offsets'] for c in flow['comparisons']]==[[0,4]]
        result.update(dice_elements=2,compared_array_offsets=[0,4])
    tokens=[r for r in snapshot['typed_records'] if r['bytes']==76]
    assert len(tokens)==(1 if name in ['card_tevesh_szat_doom_of_fools','card_nevinyrral_urborg_tyrant'] else 0)
    if tokens:
        token=tokens[0];frame_call(snapshot,'_default_token_definition',3,token['frame_offset']);frame_call(snapshot,'_generate_token',0,token['frame_offset'])
        result['scoped_token_bytes']=76
        result['token_constant_writes']=sorted(token['constant_writes'],key=lambda w:(w['offset'],w['size'],w['bytes']))
    return result


def has_source_repair(proof,root,source,name,old_sha):
    path=proof.get('native_scoped_array_recovery')
    if not path:return False
    record=json.loads((root/path).read_bytes());rows=[r for r in record['rows'] if (r['source'],r['name'],r['before_sha256'])==(source,name,old_sha)]
    if not rows:return False
    assert len(rows)==1 and name in ['do_multicascade','card_tevesh_szat_doom_of_fools','card_nevinyrral_urborg_tyrant']
    row=rows[0];changes,after=source_changes(name,row['before_definition'])
    assert sha(row['before_definition'].encode())==old_sha and changes==row['changes'] and after==row['after_definition']
    text=read(root/source);a,b=span(text,name);assert text[a:b]==after and sha(after.encode())==row['after_sha256']
    edits=[e for e in proof['source_edits'] if e.get('native_scoped_array_extension')==path and (e['source'],e['before_sha256'])==(source,old_sha)]
    assert len(edits)==1 and edits[0]['after_sha256']==row['after_sha256']
    return True


def verify(proof,image,root=ROOT):
    path=proof.get('native_scoped_array_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    rows=record['rows'];assert len(rows)==6 and {r['name'] for r in rows}==NAMES
    native=inspect(image,debug,rows);builds=[]
    for label in ['before','after']:
        artifact=record['runtime_'+label];im=Image((root/artifact['dll_path']).read_bytes());dbg=Image((root/artifact['dbg_path']).read_bytes())
        assert sha(im.data)==artifact['dll_sha256'] and sha(dbg.data)==artifact['dbg_sha256'];builds.append(inspect(im,dbg,rows))
    current=inspect(Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()),rows)
    for row in rows:
        key=row['source'],row['name'];assert native[key]==row['native_snapshot']
        assert builds[0][key]==row['before_compiled'] and builds[1][key]==row['after_compiled']
        left=validate(native[key],row);assert left==row['native_validation']
        assert validate(builds[1][key],row)==row['compiled_validation']==left
        assert validate(current[key],row)==left
        assert sha(row['before_definition'].encode())==row['before_sha256']
        changes,after=source_changes(row['name'],row['before_definition']);assert changes==row['changes'] and after==row['after_definition']==replay(row['before_definition'],changes)
        text=read(root/row['source']);a,b=span(text,row['name']);assert text[a:b]==after and sha(after.encode())==row['after_sha256']
        edits=[e for e in proof['source_edits'] if e.get('native_scoped_array_extension')==path and (e['source'],e['before_sha256'])==(row['source'],row['before_sha256'])]
        assert len(edits)==1 and edits[0]['after_sha256']==row['after_sha256']
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+r['source'][:-2]+'.o' for r in rows};assert {p for p in before if before[p]!=after[p]}==expected
    return dict(repaired_source_functions=6,card_handlers=5,helpers=1,restored_prompt_arrays=3,prompt_capacities=[500,1000,150],
        checked_formatters=4,checked_prompt_consumers=3,restored_scoped_tests=7,restored_scoped_tokens=2,
        commander_output_elements=3,commander_array_consumers=2,count_cmc_matrix_dimensions=[2,2],verified_matrix_updates=4,
        dice_output_elements=2,dice_array_comparisons=1,native_reused_storage_pairs=sum(disjoint_reused_storage(v) for v in native.values()),
        changed_linker_objects=len(expected),unchanged_linker_objects=len(before)-len(expected),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
