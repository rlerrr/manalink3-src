"""Verify remaining card/helper source receipts against native ranges, DWARF and instructions."""
import csv,json,re
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value,value
from export_abi import type_of


def verify(proof,image,root=ROOT):
    path=proof.get('native_remaining_card_helpers_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    for row in record['helpers']+record['support_helpers']+record['card_callers']:
        assert (row['source'],row['name'],row.get('receipt_sha256',row['definition_sha256'])) in receipts,row['name']
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
    assert abi==record['optimized_parameter_evidence'] and len(abi)==28
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
    arrays=record['native_frame_arrays'];assert len(arrays)==18

    assert abi['0x24889b0']['cless']==dict(first_location=None,constant=2)
    for name,offset in [('black',0),('blue',4),('green',8),('red',12),('white',16)]:
        assert abi['0x24889b0'][name]['first_location']==[145,offset]
    assert abi['0x2186250']['c1']['first_location']==[81]
    assert abi['0x2186250']['c2']['first_location']==[145,0]
    assert abi['0x24fe9c0']['t_player']['first_location']==[82]
    assert abi['0x24fe9c0']['card']['first_location']!=[82]
    assert len(abi['0x2303140'])==5
    assert {r['name']:r['dimensions'][0]['extent'] for r in arrays if r['owner']=='0x2060f50'}=={'options':10,'mana_to_charge':6}
    assert next(r for r in arrays if r['owner']=='0x25650d0')['dimensions'][0]['extent']==151
    modes=record['native_special_effect_modes'];assert len(modes)==7
    defs=read(root/'defs.h')
    for row in modes:
        die=dw.get_DIE_from_refaddr(int(row['die_offset'],16));assert die.tag=='DW_TAG_enumerator'
        assert value(die,'DW_AT_name')==row['name'] and value(die,'DW_AT_const_value')==row['value']
        assert re.search(r'\b'+row['name']+r'\s*=\s*1\s*<<\s*'+str(row['value'].bit_length()-1)+r'\b',defs)
    coff=[dict(symbol=n,address=hex(image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+a),storage=st) for n,a,s,k,st in original.symbols if n=='_ward']
    assert coff==record['native_ward_coff']==[dict(symbol='_ward',address='0x222c1a0',storage=2)]
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    md=Cs(CS_ARCH_X86,CS_MODE_32);blocks={}
    for name,row in record['native_instruction_blocks'].items():
        lo=int(row['address'],16);hi=int(row['end'],16);owner=inventory[row['owner']]
        assert int(owner['low_pc'],16)<=lo<hi<=int(owner['high_pc'],16)
        data=image.pe.get_data(lo-0x2000000,hi-lo);assert sha(data)==row['bytes_sha256']
        ins=list(md.disasm(data,lo));assert sum(i.size for i in ins)==hi-lo
        assert [[hex(i.address),i.mnemonic,i.op_str] for i in ins]==row['instructions']
        blocks[name]=[(i.mnemonic,i.op_str) for i in ins]
    assert len(blocks)==35
    for row in record['native_literals']:
        block=record['native_instruction_blocks'][row['owner']]['instructions']
        assert row['references']
        for ref in row['references']:
            address=int(ref['address'],16);data=row['value'].encode()+bytes([0])
            assert image.pe.get_data(address-0x2000000,len(data))==data
            assert any(at==ref['instruction'] and ref['address'] in ops for at,m,ops in block)
    assert len(record['native_literals'])==6
    source=read(root/'cards/invasion.c')
    colors=dict(COLOR_BLACK=1,COLOR_BLUE=2,COLOR_GREEN=3,COLOR_RED=4,COLOR_WHITE=5)
    for name,pair in record['native_color_pairs'].items():
        a,b=span(source,name);text=source[a:b]
        helper='invasion_masters' if name.endswith('_master') else 'invasion_apprentices'
        actual=re.search(helper+r'\(player,\s*card,\s*event,\s*(COLOR_\w+),\s*(COLOR_\w+)\)',text)
        assert actual and [colors[actual[1]],colors[actual[2]]]==pair and pair[0]<pair[1]
        raw=next(r for r in record['card_callers'] if r['name']==name)['native_ranges'][0]['raw_export']
        assert re.search(helper+r'\(player,card,event,'+str(pair[0])+','+str(pair[1])+r'\)',(root/raw).read_text())
    assert len(record['native_color_pairs'])==10
    # Pin the native state/data operations behind the semantic edits.
    assert ('cmp','dword ptr [0x4ef184], 0x1d') in blocks['effect_must_attack']
    assert any('0x78fef0' in s for m,s in blocks['card_thornscape_apprentice'])
    assert any('0x78fef0' in s for m,s in blocks['card_nightscape_apprentice'])
    assert any(m=='or' and s.endswith(', 0x28') for m,s in blocks['card_special_effect'])
    assert ('mov','dword ptr [0x628b34], 1') in blocks['card_special_effect']
    assert ('mov','byte ptr [0x55cf18], dl') in blocks['card_special_effect']
    assert len(record['helpers'])==record['helper_definitions']==record['native_helper_ranges']==20
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==10
    assert sum(bool(r.get('new_definition')) for r in record['helpers'])==1
    assert len(record['support_helpers'])==7 and not any(r['source_changed'] for r in record['support_helpers'])
    assert len(record['card_callers'])==60 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==10
    return dict(helper_definitions=20,native_helper_ranges=20,changed_existing_helpers=9,new_native_helpers=1,
                reviewed_support_helpers=7,complete_card_callers=60,changed_card_callers=10,native_frame_arrays=18,
                native_instruction_blocks=35,native_literals=6,special_effect_modes=7,
                scope='87 complete native ranges, source receipts, native ABI/frame extents, optimized dispatch callers, original ward entry and special-effect flags verified. Built-in dialogs and gameplay tests retain separate status.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
