"""Audit complete Journey/Avacyn/tribal body receipts and optimized native storage."""
import csv,json
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value
from export_abi import type_of


def verify(proof,image,root=ROOT):
    path=proof.get('native_journey_avacyn_tribal_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['helpers']+record['card_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name']);assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16)
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw=original.dwarf();parser=LocationParser(dw.location_lists())
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    def fn(at):return dw.get_DIE_from_refaddr(int(inventory[at]['die_offset'],16))
    def locs(d):
        a=d.attributes.get('DW_AT_location')
        if not a:return []
        parsed=parser.parse_from_attribute(a,d.cu['version'],die=d)
        return [parsed.loc_expr] if hasattr(parsed,'loc_expr') else [e.loc_expr for e in parsed if hasattr(e,'loc_expr')]
    def walk(d):
        yield d
        for c in d.iter_children():yield from walk(c)
    abi={}
    for at in record['optimized_parameter_evidence']:
        abi[at]={inherited_value(d,'DW_AT_name'):dict(first_location=next(iter(locs(d)),None),constant=inherited_value(d,'DW_AT_const_value')) for d in fn(at).iter_children() if d.tag=='DW_TAG_formal_parameter'}
    assert abi==record['optimized_parameter_evidence']
    assert abi['0x24d1010']['key']['first_location']==[81]
    assert {n:abi['0x24d1010'][n]['constant'] for n in ['pow','tou','s_key']}==dict(pow=0,tou=0,s_key=1024)
    assert abi['0x24d0b20']['td']['first_location']==[81] and abi['0x24d0800']['n']['first_location']==[81]
    assert abi['0x227e8a0']['subtype']['first_location']==[145,0]
    extents={};dynamic={}
    for at,names in [('0x206aa80',{'csvids'}),('0x2136fc0',{'stored'}),('0x21374d0',{'lands','lc'}),('0x23403d0',{'cost'}),('0x21426e0',{'csvids'})]:
        for d in walk(fn(at)):
            name=inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_variable' or name not in names:continue
            typ=type_of(d);assert typ.tag=='DW_TAG_array_type'
            element=typ.get_DIE_from_attribute('DW_AT_type');assert element.attributes['DW_AT_byte_size'].value==4
            dims=[]
            for sub in typ.iter_children():
                upper=sub.attributes['DW_AT_upper_bound']
                if upper.form=='DW_FORM_ref4':
                    dims.append('dynamic');dynamic[at+'::'+name]=locs(sub.get_DIE_from_attribute('DW_AT_upper_bound'))
                else:
                    assert upper.form in {'DW_FORM_data1','DW_FORM_data2','DW_FORM_data4'};dims.append(upper.value+1)
            key=at+'::'+name
            if key in extents:assert extents[key]==dims
            extents[key]=dims
    assert extents==record['native_array_extents'] and dynamic==record['native_dynamic_array_bounds']
    assert dynamic=={'0x2136fc0::stored':[[113,127,159],[145,136,126,6,49,28,159],[113,127,159]]}
    # The spurious lands[1][499] assignment was a stack-probe return address.
    prelude=record['realm_stack_probe_prefix'];lo=int(prelude['address'],16)
    data=image.pe.get_data(lo-0x2000000,18);assert data.hex()==prelude['bytes_hex']
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    ins=list(Cs(CS_ARCH_X86,CS_MODE_32).disasm(data,lo))
    assert [i.mnemonic for i in ins]==['push','mov','mov','push','push','push','call','sub']
    assert ins[-2].op_str=='0x26fc1d0' and ins[-1].op_str=='esp, eax'
    text=read(root/'cards/shards_of_alara.c');a,b=span(text,'realm_razer_legacy');assert '0x21374e0' not in text[a:b]
    assert len(record['helpers'])==record['helper_definitions']==30
    assert sum(len(r['native_ranges']) for r in record['helpers'])==record['native_helper_ranges']==31
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==22
    assert len(record['card_callers'])==14 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==7
    return dict(helper_definitions=30,native_helper_ranges=31,changed_helpers=22,complete_card_callers=14,changed_card_callers=7,
                native_array_extents=extents,native_dynamic_array_bounds=dynamic,
                scope='Complete source/native-range receipts; two tribal helper owner ranges; optimized parameter, fixed/dynamic frame arrays and stack-probe artifact evidence. Built-in dialog backends and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
