"""Verify planeswalker/scheme/monarch source receipts against native ranges, DWARF and instructions."""
import csv,json,re
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value,value
from export_abi import type_of


def verify(proof,image,root=ROOT):
    path=proof.get('native_planes_schemes_monarch_recovery')
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
    assert abi==record['optimized_parameter_evidence'] and len(abi)==33
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
    arrays=record['native_frame_arrays'];assert len(arrays)==8
    marked=next(r for r in arrays if r['owner']=='0x220d080')
    assert [r['extent'] for r in marked['dimensions']]==[2,100]
    planes=next(r for r in arrays if r['name']=='planeswalkers')
    assert [r['extent'] for r in planes['dimensions']]==[10,2]
    assert abi['0x231a270']['x']==dict(first_location=None,constant=4)
    for name,offset in [('cols',0),('b',4),('u',8),('g',12),('r',16),('w',20)]:
        assert abi['0x231a270'][name]['first_location']==[145,offset]
    assert abi['0x220d080']['player']['first_location']==[80]
    assert abi['0x220d4f0']['player']['first_location']==[145,0]
    partitions=[dict(symbol=n,address=hex(image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+a),storage=st) for n,a,s,k,st in original.symbols if 'you_become_the_monarch' in n]
    assert partitions==record['native_compiler_partitions']
    assert {r['symbol'] for r in partitions}=={'_you_become_the_monarch','_you_become_the_monarch.part.0'}
    def walk(d):
        yield d
        for c in d.iter_children():yield from walk(c)
    for row in record['native_inline_dependencies']:
        actual=[]
        for d in walk(fn(row['owner'])):
            if d.tag=='DW_TAG_inlined_subroutine' and inherited_value(d,'DW_AT_name')==row['name']:
                actual.append(dict(die_offset=hex(d.offset),low_pc=value(d,'DW_AT_low_pc'),high_pc=value(d,'DW_AT_high_pc'),high_pc_form=d.attributes['DW_AT_high_pc'].form if 'DW_AT_high_pc' in d.attributes else None,ranges=value(d,'DW_AT_ranges'),abstract_origin=value(d,'DW_AT_abstract_origin')))
        assert actual==row['instances'] and len(actual)==1
    for row in record['source_dependencies']:
        text=read(root/row['source']);a,b=span(text,row['name']);assert sha(text[a:b].encode())==row['definition_sha256']
        assert row['standalone_native_range'] is None
    defs=read(root/'defs.h');block=defs[defs.index('SETTING_AUTO_MILL'):defs.index('} settings_t;')]
    setting_values={};next_value=0
    for name,explicit in re.findall(r'\b(SETTING_\w+)(?:\s*=\s*(\d+))?',block):
        if explicit:next_value=int(explicit)
        setting_values[name]=next_value;next_value+=1
    assert setting_values=={r['name']:r['value'] for r in record['native_settings']}
    assert setting_values['SETTING_OLD_PW_RULES']==29 and setting_values['SETTING_OPPONENT_SKILL']==30 and setting_values['SETTING_MAX_PLUS_1']==31
    for row in record['native_settings']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16));assert d.tag=='DW_TAG_enumerator'
        assert value(d,'DW_AT_name')==row['name'] and value(d,'DW_AT_const_value')==row['value']
    ids={n:int(v) for n,v in re.findall(r'(CARD_ID_\w+)\s*=\s*(\d+)',read(root/'card_id.h'))}
    import struct
    table=record['native_gate_table'];data=image.pe.get_data(int(table['address'],16)-0x2000000,40)
    assert sha(data)==table['bytes_sha256'] and list(struct.unpack('<10i',data))==table['values']
    text=read(root/'cards/dragons_maze.c');a,b=span(text,'get_gate_code')
    assert re.findall(r'\bCARD_ID_\w+\b',text[a:b])==table['source_names']
    assert [ids[n] for n in table['source_names']]==table['values']
    for row in record['native_registration_tables']:
        text=read(root/row['source']);a,b=span(text,row['name'])
        actual=[dict(card_id=i,value=ids[i],function=f) for i,f in re.findall(r'REGISTER_SCHEME\((CARD_ID_\w+),\s*(\w+)\)',text[a:b])]
        assert actual==row['entries']
        raw=(root/row['raw_export']).read_text()
        pairs=[(int(i,16),f) for i,f in re.findall(r'get_internal_card_id_from_csv_id\((0x[0-9a-f]+)\);\s*cards_data\[iVar1\]\.code_pointer = (\w+);',raw)]
        expected=[(r['value'],r['function']) for r in actual];assert pairs==expected+expected[1:]
    assert [len(r['entries']) for r in record['native_registration_tables']]==[46,11]
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    md=Cs(CS_ARCH_X86,CS_MODE_32);blocks={}
    for name,row in record['native_instruction_blocks'].items():
        lo=int(row['address'],16);hi=int(row['end'],16);owner=inventory[row['owner']]
        assert int(owner['low_pc'],16)<=lo<hi<=int(owner['high_pc'],16)
        data=image.pe.get_data(lo-0x2000000,hi-lo);assert sha(data)==row['bytes_sha256']
        ins=list(md.disasm(data,lo));assert sum(i.size for i in ins)==hi-lo
        assert [[hex(i.address),i.mnemonic,i.op_str] for i in ins]==row['instructions']
        blocks[name]=[(i.mnemonic,i.op_str) for i in ins]
    assert len(blocks)==7
    assert ('call','0x220cad0') in blocks['monarch_guard_wrapper']
    assert any('0x20' in s for m,s in blocks['planeswalker_state_machine'])
    assert ('rep stosd','dword ptr es:[edi], eax') in blocks['monarch_worker']
    assert len(record['helpers'])==record['helper_definitions']==31 and record['native_helper_ranges']==32
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==14
    assert len(record['support_helpers'])==1 and record['support_helpers'][0]['name']=='check_for_jared_carthalion_legacy'
    assert not record['support_helpers'][0]['source_changed']
    assert record['support_helpers'][0]['method']=='retained_source_reviewed_against_complete_native_body'
    assert read(root/'cards/conspiracy-take-the-crown.c').count('static int check_for_jared_carthalion_legacy(int player)\n{')==1
    assert len(record['card_callers'])==43 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==10
    return dict(helper_definitions=31,native_helper_ranges=32,changed_helpers=14,reviewed_existing_support_helpers=1,complete_card_callers=43,changed_card_callers=10,
                native_frame_arrays=8,native_instruction_blocks=7,concrete_inline_dependencies=2,registration_bindings=57,
                scope='76 complete native ranges, Monarch compiler partition and existing private guard, native settings/arrays/inline dependencies, scheme registration and gate data verified. Original files/canonical APIs; independent callees/dialog backends/gameplay tests retain separate status.')



if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
