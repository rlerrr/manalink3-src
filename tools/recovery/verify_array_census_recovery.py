"""Verify six native fixed-array, buffer-scope and element-type repairs."""
import collections,json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from array_census_analysis import inspect,source_changes,NAMES
from verify_array_role_recovery import replay,variable


def declaration(snapshot,name,dims,width,encoding,element_kind='DW_TAG_base_type'):
    found=[v for v in snapshot['array_declarations'] if v['name']==name and v['shape']['dimensions']==dims]
    size=width
    for d in dims:size*=d
    assert len(found)==1
    assert found[0]['shape']==dict(kind='DW_TAG_array_type',size=size,dimensions=dims,element_size=width,element_encoding=encoding)
    assert found[0]['element_kind']==element_kind
    located=[v for v in snapshot['variables'] if v['name']==name and v['shape']==found[0]['shape']]
    assert len(located)==1 and located[0]['frame_offset']+size<=-8
    return found[0],located[0]


def validate(snapshot,row):
    name=row['name']
    if name=='card_multani_yavimayas_avatar':
        declaration(snapshot,'to_bounce',[2],4,5);declaration(snapshot,'msg',[50],1,6)
        assert snapshot['target_geometry']==dict(targets_offset=116,target_bytes=8,card_field_offset=120)
        flow=snapshot['selection_flow'];assert flow and len(flow['selected_card_stores'])==1
        assert [c['card_argument'] for c in flow['bounce_calls']]==[['array_word',0],['array_word',4]]
        return dict(selected_card_stores=1,bounce_array_offsets=[0,4],array_bytes=8,prompt_bytes=50)
    if name=='card_faerie_dragon':
        declaration(snapshot,'buf',[301],1,6);declaration(snapshot,'tgts',[300],8,None,'DW_TAG_structure_type')
        return dict(prompt_bytes=301,target_array_bytes=2400)
    if name in ['put_card_on_stack','resolve_trigger']:
        role='str' if name=='put_card_on_stack' else 'buf'
        declaration(snapshot,role,[500],1,6);declaration(snapshot,role,[300],1,6)
        calls=snapshot['buffer_calls'];formatters=[c for c in calls if c['name']=='_sprintf'];dialogs=[c for c in calls if c['name']=='_raw_do_dialog']
        assert all(c['array_name']==role for c in calls)
        assert collections.Counter(c['array_bytes'] for c in formatters)==collections.Counter({500:1,300:2 if name=='put_card_on_stack' else 1})
        trace=next(c for c in formatters if c['array_bytes']==500)
        expected='%d: Player %d is playing %s(%d).\n' if name=='put_card_on_stack' else '%d: Player #%d is processing %s(%d).\n'
        assert trace['literal_format']==expected
        assert len(dialogs)==1 and dialogs[0]['array_bytes']==300
        return dict(trace_bytes=500,prompt_bytes=300,checked_formatters=len(formatters),checked_prompt_consumers=1)
    if name=='new_reanimate_all_impl':
        for role in ['to_reanimate','auras']:declaration(snapshot,role,[2,500],4,5)
        for role in ['trc','ac']:declaration(snapshot,role,[2],4,5)
        return dict(array_dimensions=[2,500],array_bytes=4000,arrays=2,counter_dimensions=[2])
    assert name=='card_dromokas_command'
    decl,_=declaration(snapshot,'choices',[2],4,7,'DW_TAG_enumeration_type')
    assert decl['values']==dict(CHOICE_PREVENT=1,CHOICE_SAC=2,CHOICE_COUNTER=3,CHOICE_FIGHT=4)
    for role in ['td_spell','td_player','td_creature','td_control','td_dont_control']:assert variable(snapshot,role)['shape']['size']==84
    return dict(choice_array_bytes=8,choice_element_bytes=4,native_choice_values=decl['values'])


def has_source_repair(proof,root,source,name,old_sha):
    """Exact supersession of the two immutable original card-body receipts."""
    path=proof.get('native_array_census_recovery')
    if not path:return False
    record=json.loads((root/path).read_bytes())
    rows=[r for r in record['rows'] if r['source']==source and r['name']==name and r['before_sha256']==old_sha]
    if not rows:return False
    assert len(rows)==1 and name in ['card_faerie_dragon','card_dromokas_command']
    row=rows[0];changes,after=source_changes(name,row['before_definition'])
    assert changes==row['changes'] and after==row['after_definition'] and sha(row['before_definition'].encode())==old_sha
    text=read(root/source);a,b=span(text,name)
    assert text[a:b]==after and sha(after.encode())==row['after_sha256']
    return True


def verify(proof,image,root=ROOT):
    path=proof.get('native_array_census_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    rows=record['rows'];assert len(rows)==6 and {r['name'] for r in rows}==NAMES
    native=inspect(image,debug,rows);builds=[]
    for label in ['before','after']:
        artifact=record['runtime_'+label];im=Image((root/artifact['dll_path']).read_bytes());dbg=Image((root/artifact['dbg_path']).read_bytes())
        assert sha(im.data)==artifact['dll_sha256'] and sha(dbg.data)==artifact['dbg_sha256']
        builds.append(inspect(im,dbg,rows))
    current=inspect(Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()),rows)
    for row in rows:
        key=(row['source'],row['name']);assert native[key]==row['native_snapshot']
        assert builds[0][key]==row['before_compiled'] and builds[1][key]==row['after_compiled']
        assert validate(native[key],row)==row['native_validation']
        assert validate(builds[1][key],row)==row['compiled_validation'];validate(current[key],row)
        assert sha(row['before_definition'].encode())==row['before_sha256']
        changes,after=source_changes(row['name'],row['before_definition'])
        assert changes==row['changes'] and after==row['after_definition']==replay(row['before_definition'],changes)
        text=read(root/row['source']);a,b=span(text,row['name']);assert text[a:b]==after and sha(after.encode())==row['after_sha256']
        edits=[e for e in proof['source_edits'] if e.get('native_array_census_extension')==path and e['source']==row['source'] and e['before_sha256']==row['before_sha256']]
        assert len(edits)==1 and edits[0]['after_sha256']==row['after_sha256']
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+r['source'][:-2]+'.o' for r in rows}
    changed={p for p in before if before[p]!=after[p]};assert changed<=expected
    assert '.recovery/build-gcc93/obj/cards/dominaria.o' in changed
    return dict(reviewed_definitions=6,card_handlers=3,helpers=3,multani_selected_card_store=1,
        multani_bounce_array_reads=2,scoped_trace_prompt_pairs=2,checked_formatters=5,
        checked_prompt_consumers=2,reanimation_matrices=2,reanimation_matrix_dimensions=[2,500],
        faerie_prompt_bytes=301,enum_choice_elements=2,changed_linker_objects=len(changed),
        unchanged_linker_objects=len(before)-len(changed),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
