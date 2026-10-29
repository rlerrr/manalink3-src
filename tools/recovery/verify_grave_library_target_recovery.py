"""Verify grave/library/target source receipts against native ranges, DWARF and instructions."""
import csv,json,re
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value,value
from export_abi import type_of


def verify(proof,image,root=ROOT):
    path=proof.get('native_grave_library_target_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    for row in record['helpers']+record['card_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name']);assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16);native=inventory[part['address']]
            assert native['name']==row['name'] and native['high_pc']==part['end']
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)==record['dbg_sha256']=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw=original.dwarf();parser=LocationParser(dw.location_lists())
    def fn(at):return dw.get_DIE_from_refaddr(int(inventory[at]['die_offset'],16))
    def locs(d):
        a=d.attributes.get('DW_AT_location')
        if not a:return []
        p=parser.parse_from_attribute(a,d.cu['version'],die=d)
        return [p.loc_expr] if hasattr(p,'loc_expr') else [r.loc_expr for r in p if hasattr(r,'loc_expr')]
    abi={at:{inherited_value(d,'DW_AT_name'):dict(first_location=next(iter(locs(d)),None),constant=inherited_value(d,'DW_AT_const_value')) for d in fn(at).iter_children() if d.tag=='DW_TAG_formal_parameter'} for at in record['optimized_parameter_evidence']}
    assert abi==record['optimized_parameter_evidence'] and len(abi)==32
    for row in record['native_frame_arrays']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16));assert d.tag=='DW_TAG_variable'
        assert inherited_value(d,'DW_AT_name')==row['name']
        owner=fn(row['owner']);assert d.cu==owner.cu
        t=type_of(d);assert t.tag=='DW_TAG_array_type';dims=[]
        for s in t.iter_children():
            a=s.attributes['DW_AT_upper_bound'];v=dict(form=a.form)
            if a.form=='DW_FORM_ref4':v['expressions']=locs(s.get_DIE_from_attribute('DW_AT_upper_bound'))
            elif a.form=='DW_FORM_exprloc':v['expressions']=[a.value]
            else:assert a.form in {'DW_FORM_data1','DW_FORM_data2','DW_FORM_data4'};v['extent']=a.value+1
            dims.append(v)
        assert dims==row['dimensions']
        e=type_of(t)
        while e and 'DW_AT_byte_size' not in e.attributes and 'DW_AT_type' in e.attributes:e=type_of(e)
        assert value(e,'DW_AT_byte_size')==row['element_bytes']
    arrays=record['native_frame_arrays'];assert len(arrays)==24
    gyruda=next(r for r in arrays if r['name']=='gyruda_iids')
    assert [r['extent'] for r in gyruda['dimensions']]==[5,8] and gyruda['element_bytes']==4
    assert sum(r['dimensions'][0]['form']=='DW_FORM_ref4' for r in arrays)==10
    mill=read(root/'functions/deck.c');a,b=span(mill,'special_mill');mill=mill[a:b]
    assert 'gyruda_iids[5][8]' in mill and 'local_milled[number[k] + 1]' in mill
    assert 'XTRIGGER_REPLACE_MILL_CARDS' in mill and 'goto collect_gyruda' in mill
    assert 'REANIMATE_NO_CONTROL_LEGACY' in mill and 'set_inst_info(inst, CARD_ID_GRIST_THE_HUNGER_TIDE, 0, 66)' in mill
    for row in record['native_card_ids']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16));assert d.tag=='DW_TAG_enumerator'
        assert value(d,'DW_AT_name')==row['name'] and value(d,'DW_AT_const_value')==row['value']
        assert re.search(r'\b'+row['name']+r'\s*=\s*'+str(row['value'])+r'\b',read(root/'card_id.h'))
    assert len(record['native_card_ids'])==5
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    md=Cs(CS_ARCH_X86,CS_MODE_32);blocks={}
    for name,row in record['native_instruction_blocks'].items():
        lo=int(row['address'],16);hi=int(row['end'],16);owner=inventory[row['owner']]
        assert int(owner['low_pc'],16)<=lo<hi<=int(owner['high_pc'],16)
        data=image.pe.get_data(lo-0x2000000,hi-lo);assert sha(data)==row['bytes_sha256']
        ins=list(md.disasm(data,lo));assert sum(i.size for i in ins)==hi-lo
        assert [[hex(i.address),i.mnemonic,i.op_str] for i in ins]==row['instructions']
        blocks[name]=[(i.mnemonic,i.op_str) for i in ins]
    assert len(blocks)==10
    assert ('call','0x26910e0') in blocks['black_life_payment'] and ('mov','byte ptr [0x55cf19], bl') in blocks['black_life_payment']
    assert ('je','0x26b8182') in blocks['gyruda_replacement_exit']
    assert ('call','0x26b2e30') in blocks['gyruda_exile_branch'] and ('call','0x26b3030') not in blocks['gyruda_exile_branch']
    assert any('0x1000500' in s for m,s in blocks['mill_count_replacement'])
    assert any('0x78fef0' in s for m,s in blocks['ring_regeneration_hint'])
    assert len(record['helpers'])==record['helper_definitions']==record['native_helper_ranges']==32
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==12
    assert len(record['card_callers'])==67 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==5
    return dict(helper_definitions=32,native_helper_ranges=32,changed_helpers=12,complete_card_callers=67,changed_card_callers=5,
                native_frame_arrays=24,native_instruction_blocks=10,native_card_ids=5,
                scope='Complete direct native bodies, native ABI/storage, count replacement, fixed Gyruda capacity, native early-exit/exile quirks and canonical card IDs verified. Independent dependencies remain separately tracked; dialog backends/gameplay tests deferred.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
