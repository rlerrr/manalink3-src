"""Replay the final direct target-flag and shared sacrifice-test review."""
import collections,json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from target_storage_analysis import CASES,census,cases


def validate_values(rows):
    for name,row in rows.items():
        values=row['flag_values']
        if name=='pick_special_permanent_for_sacrifice':assert not values
        else:
            assert len(values) in ([1,2] if name=='basic_equipment' else [1])
            for store in values:
                assert store['offset']==80 and store['size']==4
                if name=='autoselect_target':expected=['parameter:flags:bit'+str(b) for b in range(32)]
                elif name in ['this_effect_doesnt_remove_this','can_activate_basic_equipment','check_for_equipment_targets']:
                    mask=3 if name=='this_effect_doesnt_remove_this' else 5
                    expected=['1' if mask>>b&1 else 'old_flags:bit'+str(b) for b in range(32)]
                else:expected=[str(5>>b&1) for b in range(32)]
                assert store['bits']==expected,(name,store)
        if name!='pick_special_permanent_for_sacrifice':continue
        expected={(108,2):'flag1',(104,4):'subtype',(110,2):'flag2',(112,4):'color',
            (124,2):'flag3',(116,4):'id',(126,2):'flag4',(128,4):'cc',(136,2):'flag5'}
        assert len(row['test_values'])==9
        assert {(s['offset'],s['size']) for s in row['test_values']}==set(expected)
        for store in row['test_values']:
            parameter=expected[store['offset'],store['size']]
            assert store['bits']==['parameter:'+parameter+':bit'+str(b) for b in range(store['size']*8)]


def verify(proof,image,root=ROOT):
    path=proof.get('native_target_storage_review')
    if not path:return {}
    record=json.loads((root/path).read_bytes())
    assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(debug.data)==record['native_dbg_sha256']
    fresh=census(image,debug);assert fresh==record['native_census']
    assert fresh['fixed_frame_target_functions']==5357 and not fresh['split_functions']
    assert len(fresh['flag_writers'])==8 and sum(len(r['writes']) for r in fresh['flag_writers'])==8
    assert {r['name'] for r in fresh['flag_writers']}==set(CASES)-{'pick_special_permanent_for_sacrifice'}
    assert len(fresh['other_locations'])==57 and all(r['category']=='no_emitted_location' for r in fresh['other_locations'])
    native=cases(image,debug,native=True);assert native==record['native_cases'];validate_values(native)
    artifact=record['compiled_artifact']
    compiled=Image((root/artifact['dll_path']).read_bytes());compiled_debug=Image((root/artifact['dbg_path']).read_bytes())
    assert sha(compiled.data)==artifact['dll_sha256'] and sha(compiled_debug.data)==artifact['dbg_sha256']
    sealed=cases(compiled,compiled_debug);assert sealed==record['compiled_cases'];validate_values(sealed)
    current=Image((root/'ManalinkEh.dll').read_bytes());current_debug=Image((root/'ManalinkEh.dbg').read_bytes())
    current_rows=cases(current,current_debug);validate_values(current_rows)
    for name,row in native.items():
        after=current_rows[name]
        assert row['contract']==after['contract']
        assert row['td_layout']==after['td_layout']
        if name=='pick_special_permanent_for_sacrifice':
            assert row['test_layout']==after['test_layout'] and row['test']['byte_events']==after['test']['byte_events']
            assert row['test_storage_aliases']==[
                dict(name='prompt',size=200,kind='DW_TAG_array_type'),
                dict(name='test',size=216,kind='DW_TAG_structure_type')]
            assert after['test_storage_aliases']==[dict(name='storage',size=216,kind='DW_TAG_union_type',members=[
                dict(name='prompt',offset=0,size=200),dict(name='test',offset=0,size=216)])]
        t=read(root/CASES[name]);a,b=span(t,name)
        assert sha(t[a:b].encode())==record['source_definitions'][name]['sha256'],name
    assert len(native['basic_equipment']['flag_values'])==1
    assert len(sealed['basic_equipment']['flag_values'])==2
    return dict(native_fixed_frame_target_functions=5357,reviewed_target_flag_functions=8,
        native_flag_store_sites=8,sealed_compiled_flag_store_sites=9,
        optimized_away_target_declarations=57,additional_shared_test_typedef_cases=1,
        sacrifice_parameter_stores=9,retained_function_definitions=9,
        flag_expression_bits_per_store=32,sacrifice_parameter_bits=208,
        scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
