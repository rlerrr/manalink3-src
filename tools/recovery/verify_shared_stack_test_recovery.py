"""Verify native scalar test parameters restored after shared-stack decompilation."""
import json
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from canonicalize_helpers import ROOT,read,sha,span
from verify_additional_local_names import source_hash_matches
from inspect_release import Image,value
from global_initializers import inherited_type
from shared_stack_test_analysis import collect,test_layout,frame_offset,compiled_functions,compare_parameter_stores,dynamic_value_flow


def verify_source(row,root,proof=None):
    text=read(root/row['source']);a,b=span(text,row['name'])
    assert sha(row['after_definition'].encode())==row['after_sha256']
    if not source_hash_matches(proof,root,row['source'],row['name'],sha(text[a:b].encode()),row['after_sha256']):
        from verify_token_scope_recovery import has_source_repair
        from verify_token_aggregate_recovery import has_source_repair as aggregate_repair
        assert proof is not None and (has_source_repair(proof,root,row['source'],row['name'],row['after_sha256']) or aggregate_repair(proof,root,row['source'],row['name'],row['after_sha256']))
    assert sha(row['before_definition'].encode())==row['before_sha256']
    assert row['before_definition']!=row['after_definition']
    return True


def verify_superseded_definition(proof,image,root,source,name,old_hash):
    """A historical receipt stays immutable when a later repair supersedes it."""
    assert (source,name)==('cards/m20.c','card_kykar_winds_fury')
    path=proof.get('native_shared_stack_test_recovery');assert path
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    rows=[r for r in record['repairs'] if (r['source'],r['name'])==(source,name)]
    assert len(rows)==1 and rows[0]['before_sha256']==old_hash
    verify_source(rows[0],root,proof)
    native=next(r for r in record['audit_rows'] if r['name']==name)['native']
    lo,hi=int(native['address'],16),int(native['end'],16)
    assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==native['bytes_sha256']
    assert rows[0]['after_definition'].count('.test.subtype = SUBTYPE_SPIRIT;')==3
    assert rows[0]['after_definition'].count('.test.qty = 1;')==2


def verify_dynamic_value_flows(record):
    # These are native register/control-flow facts, not inferred from a card's
    # rules text. Return values became dead in source when their stores vanished.
    instructions={a:(m,o) for r in record['audit_rows'] for a,m,o in r['native']['instructions']}
    for row in record['dynamic_native_proofs']:
        for address,mnemonic,operands in row['instruction_sites']:
            assert instructions[address]==(mnemonic,operands)
    repairs={r['name']:r for r in record['repairs']}
    torgaar=repairs['card_torgaar_famine_incarnate']['after_definition']
    assert 'int max_required = round_up_value(uVar3);' in torgaar
    assert 'int available = max_can_sacrifice_as_cost(player, card, &recovered_stack_td.test);' in torgaar
    assert 'recovered_stack_td.test.qty = (uint16_t)MIN(max_required, available);' in torgaar
    glissa=repairs['card_glissa_sunseeker']['after_definition']
    assert 'recovered_stack_td.test.cmc = has_mana(player, 7, -1) + 1;' in glissa
    assert 'recovered_stack_td.test.cmc_flag = F5_CMC_LESSER_THAN_VALUE;' in glissa


def verify(proof,image,root=ROOT):
    path=proof.get('native_shared_stack_test_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    original=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)==record['native_dbg_sha256']
    dw=original.dwarf()
    assert len(record['audit_rows'])==37 and len(record['repairs'])==24
    repairs={r['name']:r for r in record['repairs']}
    for row in repairs.values():verify_source(row,root,proof)
    edits=[e for e in proof['source_edits'] if e.get('native_shared_stack_test_extension')==path]
    assert len(edits)==24
    for row in repairs.values():
        assert any(e['source']==row['source'] and e['before']==row['before_definition'] and
                   e['after']==row['after_definition'] for e in edits)
    built=[]
    for key in ['runtime_before','runtime_after']:
        artifact=record[key];raw=(root/artifact['path']).read_bytes();assert sha(raw)==artifact['dll_sha256']
        debug=(root/artifact['debug_path']).read_bytes();assert sha(debug)==artifact['dbg_sha256']
        built.append((Image(raw),Image(debug)))
    before_dw,before_functions=compiled_functions(built[0][1],record['audit_rows'])
    after_dw,after_functions=compiled_functions(built[1][1],record['audit_rows'])
    current=Image((root/'ManalinkEh.dll').read_bytes());current_debug=Image((root/'ManalinkEh.dbg').read_bytes())
    plain=[]
    if proof.get('native_scoped_array_recovery'):
        plain=[r for r in record['audit_rows'] if r['name'] in ['card_tevesh_szat_doom_of_fools','card_nevinyrral_urborg_tyrant']]
        assert len(plain)==2
    if proof.get('native_card_array_locals_recovery'):
        plain.extend(r for r in record['audit_rows'] if r['name']=='card_gusthas_scepter')
    if proof.get('native_token_scope_recovery'):
        plain.extend(r for r in record['audit_rows'] if r['name']=='card_shark_typhoon')
    if proof.get('native_token_aggregate_recovery'):
        plain.extend(r for r in record['audit_rows'] if r['name'] in ['card_inalla_archmage_ritualist','card_ghired_conclave_exile'])
    current_dw,current_functions=compiled_functions(current_debug,[r for r in record['audit_rows'] if r not in plain])
    if plain:
        from scoped_array_analysis import plain_test_record
        from verify_scoped_array_recovery import has_source_repair
        for row in plain:
            if row['name'] in ['card_inalla_archmage_ritualist','card_ghired_conclave_exile']:
                from token_aggregate_analysis import plain_test_record as aggregate_test
                from verify_token_aggregate_recovery import has_source_repair as aggregate_repair
                old=repairs[row['name']]['after_sha256'] if row['name'] in repairs else row['retained_definition_sha256']
                assert aggregate_repair(proof,root,row['source'],row['name'],old)
                current_functions[row['name']]=aggregate_test(current_dw,row)
            elif row['name']=='card_shark_typhoon':
                from token_scope_analysis import plain_test_record as scoped_test
                from verify_token_scope_recovery import has_source_repair as scope_repair
                assert scope_repair(proof,root,row['source'],row['name'],repairs[row['name']]['after_sha256'])
                current_functions[row['name']]=scoped_test(current_dw,row)
            elif row['name']=='card_gusthas_scepter':
                from card_array_locals_analysis import plain_record
                from verify_card_array_locals_recovery import has_source_repair as card_repair
                assert card_repair(proof,root,row['source'],row['name'],row['retained_definition_sha256'])
                current_functions[row['name']]=plain_record(current_dw,row)
            else:
                assert has_source_repair(proof,root,row['source'],row['name'],row['retained_definition_sha256'])
                current_functions[row['name']]=plain_test_record(current_dw,row)
    md=Cs(CS_ARCH_X86,CS_MODE_32)
    missing_stores=0;matched_before=0
    for row in record['audit_rows']:
        d=dw.get_DIE_from_refaddr(int(row['native_die'],16))
        variable=dw.get_DIE_from_refaddr(int(row['native_variable_die'],16))
        assert value(d,'DW_AT_frame_base')==[156]
        assert frame_offset(dw,variable)==row['storage']['frame_offset']
        assert test_layout(inherited_type(variable))==row['layout']
        native=collect(image,dw,d,row['storage']['frame_offset'],row['layout'])
        assert native==row['native']
        for index,(compiled,debug) in enumerate(built):
            fn=(before_functions if index==0 else after_functions)[row['name']]
            view=collect(compiled,before_dw if index==0 else after_dw,fn['die'],fn['frame_offset'],fn['layout'])
            assert view==row['before_compiled' if index==0 else 'after_compiled']
        fn=current_functions[row['name']]
        assert fn['layout']==row['layout']
        fresh=collect(current,current_dw,fn['die'],fn['frame_offset'],fn['layout'])
        compare_parameter_stores(row['name'],native,fresh,current)
        assert compare_parameter_stores(row['name'],native,row['after_compiled'],built[1][0])==row['after_comparison']
        if row['after_comparison']['kind']!='exact_scalar_byte_events':
            assert repairs[row['name']]['after_definition'].count('.test.subtype = SUBTYPE_GATE;')==1
        if row['name'] in ['card_torgaar_famine_incarnate','card_glissa_sunseeker']:
            assert dynamic_value_flow(row['name'],native,image,original)==row['native_dynamic_flow']
            assert dynamic_value_flow(row['name'],row['after_compiled'],built[1][0],built[1][1])==row['after_dynamic_flow']
            dynamic_value_flow(row['name'],fresh,current,current_debug)
        if row['name'] in repairs:
            assert row['before_compiled']['byte_events']!=native['byte_events']
            missing_stores+=len(native['stores'])
        else:
            assert row['before_compiled']['byte_events']==native['byte_events']
            t=read(root/row['source']);a,b=span(t,row['name'])
            if not source_hash_matches(proof,root,row['source'],row['name'],sha(t[a:b].encode()),row['retained_definition_sha256']):
                from verify_scoped_array_recovery import has_source_repair
                from verify_card_array_locals_recovery import has_source_repair as card_repair
                from verify_token_aggregate_recovery import has_source_repair as aggregate_repair
                assert aggregate_repair(proof,root,row['source'],row['name'],row['retained_definition_sha256']) or has_source_repair(proof,root,row['source'],row['name'],row['retained_definition_sha256']) or card_repair(proof,root,row['source'],row['name'],row['retained_definition_sha256'])
            matched_before+=1
    assert missing_stores==43 and matched_before==13
    verify_dynamic_value_flows(record)
    left,right=record['before_link_inputs'],record['after_link_inputs']
    assert set(left)==set(right)
    changed=sorted(p for p in left if left[p]!=right[p])
    expected=sorted('.recovery/build-gcc93/obj/'+s[:-2]+'.o' for s in {r['source'] for r in repairs.values()})
    assert changed==expected and len(changed)==16
    return dict(reviewed_shared_stack_handlers=37,corrected_card_handlers=24,restored_native_scalar_stores=43,
                retained_matching_handlers=13,changed_linker_objects=16,unchanged_linker_objects=len(left)-16,
                dynamic_value_flows_restored=2,remaining_inline_review_names=22,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
