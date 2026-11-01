"""Verify native ability-label matrices, prompt capacity and scalar roles."""
import json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from ability_storage_analysis import inspect
from verify_array_role_recovery import variable,replay

SCALARS={'card_ajani_unyielding':'qty','card_serras_hymn':'prev','card_dark_impostor':'can_activate'}


def array(snapshot,name,dimensions,element_size,encoding):
    v=variable(snapshot,name);size=element_size
    for d in dimensions:size*=d
    assert v['shape']==dict(kind='DW_TAG_array_type',size=size,dimensions=dimensions,
        element_size=element_size,element_encoding=encoding)
    assert v['frame_offset']+size<=-8
    return v


def validate(snapshot,row,native=False):
    name=row['name'];result={}
    if name in SCALARS:
        found=[s for s in snapshot['scalar_types'] if s['name']==SCALARS[name]]
        assert len(found)==1 and found[0]['size']==4 and found[0]['encoding']==5,(name,found)
        result['integer_role']=SCALARS[name]
    if name=='card_dark_impostor':
        matrix=array(snapshot,'stolen_abilities_name',[12,100],1,6)
        stolen=array(snapshot,'stolen_abilities',[12],2,5)
        iids=[v for v in snapshot['variables'] if v['name']=='iids']
        assert len(iids)==2 and all(v['shape']==dict(kind='DW_TAG_array_type',size=24,
            dimensions=[12],element_size=2,element_encoding=5) for v in iids)
        assert all(v['frame_offset']+24<=-8 for v in iids)
        calls=[c for c in snapshot['effects']['calls'] if c['name']=='_dialog_impl']
        assert len(calls)==1;call=calls[0]
        assert call['arguments'][2]==['const',0xffffffff,109]
        for i in range(1,12):
            assert call['arguments'][9+3*i]==['frame',matrix['frame_offset']+8+100*i],(name,i,call['arguments'][9+3*i])
        first=call['arguments'][9]
        if first is not None:assert first==['frame',matrix['frame_offset']+8]
        for i in range(12):assert call['arguments'][11+3*i]==['const',0xffffffff,1]
        assert call['arguments'][45:49]==[['const',0xffffffff,0]]*4
        reads=snapshot['selected_iid_reads']
        assert len(reads)==1 and reads[0]['extension']=='movsx' and reads[0]['choice_coefficient']==2 and reads[0]['bytes']==2
        assert reads[0]['choice_zero_frame_offset']==stolen['frame_offset']-4
        result['ability_table']=dict(bytes=1200,rows=12,row_bytes=100,
            definite_dialog_rows=list(range(1,12)) if first is None else list(range(12)),
            first_row_operand_scope='Unknown pointer spill; typed source row zero retained' if first is None else 'Exact frame operand',
            selected_iid_expression='stolen_abilities[dialog_result - 2]',selected_iid_read=reads[0])
    if name=='card_brain_in_a_jar':
        buffer=array(snapshot,'buffer',[500],1,6);array(snapshot,'cmc',[16],4,5)
        test=variable(snapshot,'this_test');assert test['shape']['size']==216
        pointer=['frame',buffer['frame_offset']+8]
        calls=snapshot['effects']['calls']
        fmt=[c for c in calls if c['name']=='_scnprintf' and c['arguments'][0]==pointer and c['arguments'][1]==['const',0xffffffff,500]]
        use=[c for c in calls if c['name']=='_new_default_test_definition' and c['arguments'][0]==['frame',test['frame_offset']+8] and c['arguments'][2]==pointer]
        assert len(fmt)==len(use)==1
        result['prompt']=dict(bytes=500,formatter=fmt[0]['address'],consumer=use[0]['address'],separate_cmc_entries=16)
    return result


def retained_native(snapshot):
    exiled=array(snapshot,'exiled',[2],4,5);test=variable(snapshot,'this_test')
    assert test['shape']['size']==216 and test['frame_offset']==exiled['frame_offset']+8
    reads=[a for a in snapshot['effects']['indexed_accesses'] if a['frame_offset']==test['frame_offset']
        and a['scale']==4 and a['size']==4 and a['access']&1]
    assert len(reads)==1
    return dict(exiled_bytes=8,adjacent_test_bytes=216,native_prompt_word_read=reads[0],
        scope='Native binary itself reads the adjacent prompt during the shift. Retain the existing explicit message read; no gameplay correction.')


def verify(proof,image,root=ROOT):
    path=proof.get('native_ability_storage_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(debug.data)==record['native_dbg_sha256']
    rows=record['rows'];retained=record['retained'];assert len(rows)==4 and len(retained)==1
    owners=rows+retained
    native=inspect(image,debug,owners);builds=[]
    for label in ['before','after']:
        artifact=record['runtime_'+label];im=Image((root/artifact['dll_path']).read_bytes());dbg=Image((root/artifact['dbg_path']).read_bytes())
        assert sha(im.data)==artifact['dll_sha256'] and sha(dbg.data)==artifact['dbg_sha256']
        builds.append(inspect(im,dbg,owners))
    current=inspect(Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()),owners)
    for row in rows:
        key=(row['source'],row['name']);assert native[key]==row['native_snapshot']
        assert builds[0][key]==row['before_compiled'] and builds[1][key]==row['after_compiled']
        assert validate(native[key],row,native=True)==row['native_validation']
        assert validate(builds[1][key],row)==row['compiled_validation'];validate(current[key],row)
        assert sha(row['before_definition'].encode())==row['before_sha256']
        assert replay(row['before_definition'],row['changes'])==row['after_definition']
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert text[a:b]==row['after_definition'] and sha(text[a:b].encode())==row['after_sha256']
        receipts=[e for e in proof['source_edits'] if e.get('native_ability_storage_extension')==path
            and e['source']==row['source'] and e['before_sha256']==row['before_sha256']]
        assert len(receipts)==1 and receipts[0]['after_sha256']==row['after_sha256']
    for row in retained:
        key=(row['source'],row['name']);assert row['name']=='card_etali_primal_storm'
        assert native[key]==row['native_snapshot'] and retained_native(native[key])==row['native_validation']
        assert builds[0][key]==row['before_compiled'] and builds[1][key]==row['after_compiled']
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert text[a:b]==row['before_definition'] and sha(text[a:b].encode())==row['before_sha256']
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+r['source'][:-2]+'.o' for r in rows}
    assert {p for p in before if before[p]!=after[p]}==expected
    return dict(recovered_card_definitions=4,ability_name_matrix_bytes=1200,ability_name_rows=12,
        ability_name_row_bytes=100,selected_iid_array_entries=12,selected_iid_entry_bytes=2,
        restored_prompt_bytes=500,integer_roles_restored=3,retained_native_quirk_cards=1,
        changed_linker_objects=len(expected),unchanged_linker_objects=len(before)-len(expected),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
