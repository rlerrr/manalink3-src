"""Check shared target/token records and the native Windgrace token fields."""
import collections,json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value
from shared_stack_record_analysis import compiled_records,frame_locals,collect_bytes
from other_stack_value_proofs import controllers,packed_choice
from verify_wide_stack_test_recovery import symbol_at


def events(view,size,image,debug,exclude=()):
    result=collections.Counter()
    for s in view['stores']:
        if s['address'] in exclude:continue
        name=None
        if s['size']==4 and s['value'] is not None and 0x2000000<s['value']<0x2000000+image.pe.OPTIONAL_HEADER.SizeOfImage:
            name=symbol_at(image,debug,s['value'])
            assert name in ['_is_my_tetravite','_haste_and_remove_token_at_end_of_next_turn'],name
        for b in range(s['size']):
            at=s['offset']+b
            if 0<=at<size:
                v='callback:'+name+':byte'+str(b) if name else None if s['value'] is None else s['value']>>(8*b)&255
                result[(at,v)]+=1
    return result


def comparison(row,left,right,ni,nd,ci,cd,ndw,die,cdw,fn):
    name=row['name'];size=row['source_union_size']
    a=events(left,size,ni,nd);b=events(right,size,ci,cd)
    if a==b:
        return dict(kind='exact_scalar_byte_events' if left['byte_events']==right['byte_events'] else 'relocated_callback_identity')
    if name=='card_mechanized_production':
        lp=controllers(left,ni,nd,row['native_frame_offset']);rp=controllers(right,ci,cd,fn['frame_offset'])
        le=set(lp['player_store_sites']);re=set(rp['player_store_sites'])
        assert le=={s['address'] for s in left['stores']} and re=={s['address'] for s in right['stores']}
        assert events(left,size,ni,nd,le)==events(right,size,ci,cd,re)
        return dict(kind='equivalent_controller_values',native_proof=lp,compiled_proof=rp)
    assert name=='new_sacrifice_impl',name
    lp=packed_choice(left,ni,nd,die,ndw,row['native_frame_offset'],native=True)
    rp=packed_choice(right,ci,cd,fn['die'],cdw,fn['frame_offset'])
    assert lp['result_bit_lanes']==rp['result_bit_lanes']
    le={i[0] for i in lp['instruction_window']};re={i[0] for i in rp['instruction_window']}
    assert len([s for s in left['stores'] if s['address'] in le])==3
    assert len([s for s in right['stores'] if s['address'] in re])==1
    assert events(left,size,ni,nd,le)==events(right,size,ci,cd,re)
    return dict(kind='equivalent_packed_sacrifice_choice',native_proof=lp,compiled_proof=rp)


def verify(proof,image,root=ROOT):
    path=proof.get('native_other_stack_record_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    original=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(original.data)==record['native_dbg_sha256']
    dw=original.dwarf();assert len(record['rows'])==22 and not record['unresolved']
    row=record['repair'];t=read(root/row['source']);a,b=span(t,row['name'])
    assert sha(row['after_definition'].encode())==row['after_sha256']
    if t[a:b]!=row['after_definition']:
        from verify_card_array_locals_recovery import has_source_repair
        assert has_source_repair(proof,root,row['source'],row['name'],row['after_sha256'])
    assert sha(row['before_definition'].encode())==row['before_sha256']
    assert row['after_definition'].count('storage.token.qty = 6;')==1
    assert row['after_definition'].count('storage.token.color_forced = COLOR_TEST_GREEN;')==1
    edits=[r for r in proof['source_edits'] if r.get('native_other_stack_record_extension')==path]
    assert len(edits)==1 and edits[0]['before']==row['before_definition'] and edits[0]['after']==row['after_definition']
    builds=[]
    for key in ['runtime_before','runtime_after']:
        artifact=record[key];data=(root/artifact['path']).read_bytes();dbg=(root/artifact['debug_path']).read_bytes()
        assert sha(data)==artifact['dll_sha256'] and sha(dbg)==artifact['dbg_sha256']
        im,debug=Image(data),Image(dbg);cd,functions=compiled_records(debug,record['rows']);builds.append((im,debug,cd,functions))
    current=Image((root/'ManalinkEh.dll').read_bytes());current_debug=Image((root/'ManalinkEh.dbg').read_bytes())
    current_rows=record['rows'];plain=None
    if proof.get('native_selection_storage_recovery'):
        plain=next(r for r in current_rows if r['name']=='card_geyadrone_dihada')
        from verify_selection_storage_recovery import verify_superseded_definition
        verify_superseded_definition(proof,image,root,plain['source'],plain['name'],plain['before_definition_sha256'])
        current_rows=[r for r in current_rows if r is not plain]
    token_plain=[]
    if proof.get('native_card_array_locals_recovery'):
        from card_array_locals_analysis import TOKEN_NAMES,plain_record
        from verify_card_array_locals_recovery import has_source_repair
        token_plain=[r for r in current_rows if r['name'] in TOKEN_NAMES]
        assert len(token_plain)==2
        current_rows=[r for r in current_rows if r not in token_plain]
    current_dw,current_functions=compiled_records(current_debug,current_rows);kinds=collections.Counter()
    for entry in token_plain:
        old=record['repair']['after_sha256'] if entry['name']=='card_lord_windgrace' else entry['before_definition_sha256']
        assert has_source_repair(proof,root,entry['source'],entry['name'],old)
        current_functions[(entry['source'],entry['name'])]=plain_record(current_dw,entry,token=True)
    if plain is not None:
        from selection_storage_analysis import plain_target_record
        current_functions[(plain['source'],plain['name'])]=plain_target_record(current_dw,plain)

    for row in record['rows']:
        d=dw.get_DIE_from_refaddr(int(row['native_die'],16));assert value(d,'DW_AT_frame_base')==[156]
        aliases=[v for v in frame_locals(dw,d) if v['frame_offset']==row['native_frame_offset']]
        assert aliases==row['native_aliases']
        assert all(any(v.get('layout')==shape for v in aliases) for shape in row['source_members'].values())
        native=collect_bytes(image,dw,d,row['native_frame_offset'],row['source_union_size']);assert native==row['native']
        for j,(im,debug,cd,functions) in enumerate(builds):
            f=functions[(row['source'],row['name'])]
            assert f['size']==row['source_union_size'] and f['members']==row['source_members']
            view=collect_bytes(im,cd,f['die'],f['frame_offset'],f['size'])
            assert view==row['before_compiled' if j==0 else 'after_compiled']
        ci,cd,cdw,functions=builds[1];f=functions[(row['source'],row['name'])]
        checked=comparison(row,native,row['after_compiled'],image,original,ci,cd,dw,d,cdw,f)
        assert checked==row['after_comparison'];kinds[checked['kind']]+=1
        f=current_functions[(row['source'],row['name'])]
        assert f['size']==row['source_union_size'] and f['members']==row['source_members']
        fresh=collect_bytes(current,current_dw,f['die'],f['frame_offset'],f['size'])
        comparison(row,native,fresh,image,original,current,current_debug,dw,d,current_dw,f)
        if row['name']!='card_lord_windgrace':
            t=read(root/row['source']);a,b=span(t,row['name'])
            if sha(t[a:b].encode())!=row['before_definition_sha256']:
                if row['name']=='card_geyadrone_dihada':
                    from verify_selection_storage_recovery import verify_superseded_definition
                elif row['name']=='card_vraska_relic_seeker':
                    from verify_card_array_locals_recovery import verify_superseded_definition
                elif row['name']=='card_ovinomancer':
                    from verify_target_role_recovery import verify_superseded_definition
                else:
                    from verify_cast_stack_storage_recovery import verify_superseded_definition
                verify_superseded_definition(proof,image,root,row['source'],row['name'],row['before_definition_sha256'])
        else:
            missing={(s['offset'],s['size'],s['value']) for s in native['stores']} - {
                (s['offset'],s['size'],s['value']) for s in row['before_compiled']['stores']}
            assert missing=={(43,1,6),(40,1,8)}
    left,right=record['before_link_inputs'],record['after_link_inputs'];assert set(left)==set(right)
    assert [p for p in left if left[p]!=right[p]]==['.recovery/build-gcc93/obj/cards/commander_2018.o']
    assert dict(kinds)==dict(exact_scalar_byte_events=18,relocated_callback_identity=2,
                            equivalent_controller_values=1,equivalent_packed_sacrifice_choice=1)
    return dict(reviewed_shared_storage_cases=22,corrected_card_handlers=1,restored_native_token_writes=2,
                retained_definitions=21,comparison_kinds=dict(kinds),changed_linker_objects=1,
                unchanged_linker_objects=len(left)-1,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
