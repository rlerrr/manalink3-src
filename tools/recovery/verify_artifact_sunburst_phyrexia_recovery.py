"""Audit artifact, sunburst and Phyrexia body receipts and native contracts."""
import csv,json,re
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value,value
from export_abi import type_of


def verify(proof,image,root=ROOT):
    path=proof.get('native_artifact_sunburst_phyrexia_recovery')
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
    assert abi['0x2028e70']['equip_cost']['constant']==1 and abi['0x2028e70']['s_key']['constant']==0
    assert {n:abi['0x2028e70'][n]['first_location'] for n in ['manacolor','pow','tou','key']}==dict(manacolor=[145,0],pow=[145,4],tou=[145,8],key=[145,12])
    extents={}
    for at,names in [('0x2028e70',{'c','abils'}),('0x232af80',{'dead'}),('0x253eeb0',{'removed'}),('0x2539690',{'abilities'})]:
        for d in walk(fn(at)):
            name=inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_variable' or name not in names:continue
            typ=type_of(d)
            if typ.tag!='DW_TAG_array_type':continue
            element=typ.get_DIE_from_attribute('DW_AT_type');assert element.attributes['DW_AT_byte_size'].value==4
            dims=[]
            for sub in typ.iter_children():
                upper=sub.attributes['DW_AT_upper_bound'];assert upper.form in {'DW_FORM_data1','DW_FORM_data2','DW_FORM_data4'};dims.append(upper.value+1)
            key=at+'::'+name
            if key in extents:assert extents[key]==dims
            extents[key]=dims
    assert extents==record['native_array_extents']=={'0x2028e70::c':[6],'0x2028e70::abils':[3],'0x232af80::dead':[2],'0x253eeb0::removed':[10],'0x2539690::abilities':[2]}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    md=Cs(CS_ARCH_X86,CS_MODE_32)
    assert len(record['target_initialization_sites'])==2
    for site in record['target_initialization_sites']:
        lo=int(site['call_address'],16);data=bytes.fromhex(site['call_bytes'])
        assert image.pe.get_data(lo-0x2000000,len(data))==data
        ins=next(md.disasm(data,lo));assert ins.mnemonic=='call' and ins.op_str=='0x2660240'
        assert len(site['controller_stores'])==2
        for store in site['controller_stores']:
            at=int(store['address'],16);data=bytes.fromhex(store['bytes']);assert at>lo
            assert image.pe.get_data(at-0x2000000,len(data))==data
            ins=next(md.disasm(data,at));assert ins.mnemonic=='mov' and ins.op_str==store['op_str']
    inline=record['inlined_equipment_query'];text=read(root/inline['source']);a,b=span(text,inline['function'])
    assert sha(text[a:b].encode())==inline['definition_sha256']
    assert inline['owner_range']=='0x2028e70' and inline['standalone_native_range'] is None
    for row in record['native_card_ids']:
        die=dw.get_DIE_from_refaddr(int(row['die_offset'],16));assert die.tag=='DW_TAG_enumerator'
        assert value(die,'DW_AT_name')==row['name'] and value(die,'DW_AT_const_value')==row['value']
        assert re.search(r'\b'+row['name']+r'\s*=\s*'+str(row['value'])+r'\b',read(root/'card_id.h'))
    assert len(record['native_card_ids'])==1 and record['native_card_ids'][0]['value']==23212
    assert 'static char *not_already_dying(int who_chooses, int player, int card);' in read(root/'cards/darksteel.c')
    for row in record['dialog_option_data']:
        data=bytes.fromhex(row['bytes_hex']);assert image.pe.get_data(int(row['address'],16)-0x2000000,len(data))==data
    assert len(record['helpers'])==record['helper_definitions']==33
    assert sum(len(r['native_ranges']) for r in record['helpers'])==record['native_helper_ranges']==33
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==23
    assert len(record['card_callers'])==16 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==9
    return dict(helper_definitions=33,native_helper_ranges=33,changed_helpers=23,complete_card_callers=16,changed_card_callers=9,
                native_array_extents=extents,native_card_ids=1,
                scope='Complete source/native-range receipts, optimized equipment parameter mappings, native fixed arrays, controller-store assembly order and Zabaz enum value. Inlined equipment query reviewed within owner range; no invented standalone range. Built-in dialogs and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
