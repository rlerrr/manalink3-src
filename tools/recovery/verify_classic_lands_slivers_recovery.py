"""Audit classic land/Sliver complete body receipts and native frame arrays."""
import csv,json
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value
from export_abi import type_of


def verify(proof,image,root=ROOT):
    path=proof.get('native_classic_lands_slivers_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['helpers']+record['card_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name'])
        current_hash=sha(text[a:b].encode())
        if current_hash!=row['definition_sha256']:
            # Narrow later correction: native instructions store preferred_controller
            # after initialization; Ghidra misleadingly hoisted it before the call.
            from verify_transform_awaken_exile_recovery import BAD_SONG_ORDER,GOOD_SONG_ORDER
            assert row['source']=='cards/legends.c' and row['name']=='song'
            refinements=[e for e in proof['source_edits'] if e.get('native_target_initialization_refinement') and e['source']==row['source'] and e['before_sha256']==row['definition_sha256']]
            assert len(refinements)==1
            correction=refinements[0]
            assert correction['before'].count(BAD_SONG_ORDER)==1
            assert correction['after']==correction['before'].replace(BAD_SONG_ORDER,GOOD_SONG_ORDER,1)
            assert correction['after_sha256']==current_hash
            assert correction['native_target_initialization_refinement']['supersedes']==path
        else:
            assert current_hash==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16)
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw=original.dwarf();inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    def walk(d):
        yield d
        for c in d.iter_children():yield from walk(c)
    fixed={};dynamic={}
    for at,names in [('0x209dab0',{'buffer2'}),('0x21ecfe0',{'prompt1','prompt2'}),('0x20a2f70',{'horrors'})]:
        fn=dw.get_DIE_from_refaddr(int(inventory[at]['die_offset'],16))
        for d in walk(fn):
            if d.tag!='DW_TAG_variable' or inherited_value(d,'DW_AT_name') not in names:continue
            name=inherited_value(d,'DW_AT_name');typ=type_of(d);assert typ.tag=='DW_TAG_array_type'
            subranges=[x for x in typ.iter_children() if x.tag=='DW_TAG_subrange_type'];assert len(subranges)==1
            upper=subranges[0].attributes['DW_AT_upper_bound']
            element=typ.get_DIE_from_attribute('DW_AT_type')
            assert element.attributes['DW_AT_byte_size'].value==(4 if name=='horrors' else 1)
            if upper.form=='DW_FORM_exprloc':dynamic[name]=upper.value
            else:
                assert upper.form in {'DW_FORM_data1','DW_FORM_data2','DW_FORM_data4'}
                fixed[name]=upper.value+1
    assert fixed=={'buffer2':100,'prompt1':300,'prompt2':600}
    assert dynamic=={'horrors':[112,127]}
    assert fixed==record['native_frame_arrays']['fixed_char_arrays']
    assert dynamic==record['native_frame_arrays']['dynamic_int_upper_bounds']
    assert len(record['helpers'])==record['helper_definitions']==32
    assert sum(len(r['native_ranges']) for r in record['helpers'])==record['native_helper_ranges']==33
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==14
    assert len(record['card_callers'])==9 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==1
    assert all(r['name']!='snprintf' for r in record['helpers'])
    return dict(helper_definitions=32,native_helper_ranges=33,changed_helpers=14,complete_card_callers=9,
      changed_card_callers=1,native_fixed_char_arrays=fixed,native_dynamic_int_array_upper_bounds=dynamic,
      scope='Complete source/native-range receipts, two pitch-spell owner ranges, and native fixed/dynamic frame array contracts. Specialized CRT snprintf, built-in dialog backends and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
