"""Audit classic-card/Sliver body receipts, frames, fixed EXE references and native quirks."""
import csv,json,re
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value,value
from export_abi import type_of


def verify(proof,image,root=ROOT):
    path=proof.get('native_classic_sliver_protection_recovery')
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
        parsed=parser.parse_from_attribute(a,d.cu['version'],die=d)
        return [parsed.loc_expr] if hasattr(parsed,'loc_expr') else [e.loc_expr for e in parsed if hasattr(e,'loc_expr')]
    def walk(d):
        yield d
        for c in d.iter_children():yield from walk(c)
    abi={at:{inherited_value(d,'DW_AT_name'):dict(first_location=next(iter(locs(d)),None),constant=inherited_value(d,'DW_AT_const_value')) for d in fn(at).iter_children() if d.tag=='DW_TAG_formal_parameter'} for at in record['optimized_parameter_evidence']}
    assert abi==record['optimized_parameter_evidence']
    assert abi['0x211ff90']['primary']['first_location']==[145,0] and abi['0x211ff90']['secondary']['first_location']==[145,4]
    assert abi['0x241b160']['mana_color']['first_location']==[145,0]
    assert abi['0x24a61c0']['event']==dict(first_location=None,constant=None)
    for name,offset in [('t_player',8),('t_card',12),('event',16)]:assert abi['0x26a3630'][name]['first_location']==[145,offset]
    arrays={};wanted={}
    for key in record['native_arrays']:
        at,name=key.split('::');wanted.setdefault(at,set()).add(name)
    for at,names in wanted.items():
        for d in walk(fn(at)):
            name=inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_variable' or name not in names:continue
            typ=type_of(d)
            if typ.tag!='DW_TAG_array_type':continue
            dims=[]
            for sub in typ.iter_children():
                upper=sub.attributes['DW_AT_upper_bound'];assert upper.form in {'DW_FORM_data1','DW_FORM_data2','DW_FORM_data4'};dims.append(upper.value+1)
            element=type_of(typ)
            while element and 'DW_AT_byte_size' not in element.attributes and 'DW_AT_type' in element.attributes:element=type_of(element)
            row=dict(dimensions=dims,element_bytes=value(element,'DW_AT_byte_size'));key=at+'::'+name
            if key in arrays:assert arrays[key]==row
            arrays[key]=row
    assert arrays==record['native_arrays'] and len(arrays)==6
    assert arrays['0x241b160::c']==dict(dimensions=[6],element_bytes=4)
    assert arrays['0x24a61c0::prompt']==dict(dimensions=[200],element_bytes=1)
    assert arrays['0x24fb710::found']==dict(dimensions=[2],element_bytes=4)
    absolute={n:hex(a) for n,a,section,kind,storage in original.symbols if section==-1 and n in {'_check_abilities_power','_check_abilities_toughness','_check_abilities_keywords'}}
    assert absolute==record['native_absolute_symbols']
    expected_globals={'check_abilities_keywords':'0x78fef0','check_abilities_power':'0x739b50','check_abilities_toughness':'0x7a371c'}
    assert record['native_exe_references']==expected_globals
    for name,at in expected_globals.items():assert re.search(r'#define\s+'+name+r'\s+EXE_DWORD\('+at+r'\)',read(root/'manalink.h'))
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    md=Cs(CS_ARCH_X86,CS_MODE_32)
    for name,row in record['native_instruction_blocks'].items():
        lo=int(row['address'],16);hi=int(row['end'],16);owner=inventory[row['owner']]
        assert int(owner['low_pc'],16)<=lo<hi<=int(owner['high_pc'],16)
        data=image.pe.get_data(lo-0x2000000,hi-lo);assert sha(data)==row['bytes_sha256']
        instructions=list(md.disasm(data,lo));assert sum(i.size for i in instructions)==hi-lo
        assert [[hex(i.address),i.mnemonic,i.op_str] for i in instructions]==row['instructions']
        pairs=[(i.mnemonic,i.op_str) for i in instructions]
        if name=='grixis_native_argument_order':
            assert ('mov','eax, dword ptr [ebp + 0x10]') in pairs
            assert ('mov','dword ptr [esp + 8], eax') in pairs and ('mov','dword ptr [esp + 0xc], edi') in pairs
            assert ('mov','dword ptr [esp + 0x10], edx') in pairs and ('call','0x26a3630') in pairs
        elif name=='hunter_native_info_slot':assert ('mov','ebx, dword ptr [eax + 0x38]') in pairs
        elif name.endswith('_keywords'):
            assert ('or','ah, 1') in pairs and ('mov','dword ptr [0x78fef0], eax') in pairs
        elif name=='deftblade_toughness':assert ('mov','dword ptr [0x7a371c], eax') in pairs
        else:
            assert name=='deftblade_power_toughness'
            assert ('mov','dword ptr [0x739b50], edx') in pairs and ('mov','dword ptr [0x7a371c], ecx') in pairs
    inline=record['native_inlined_creation'];assert inline['owner']=='0x24fbd70' and inline['name']=='create_minds_desire_legacy'
    instances=[]
    for d in walk(fn(inline['owner'])):
        if d.tag=='DW_TAG_inlined_subroutine' and inherited_value(d,'DW_AT_name')==inline['name']:
            instances.append(dict(die_offset=hex(d.offset),low_pc=value(d,'DW_AT_low_pc'),high_pc=value(d,'DW_AT_high_pc'),high_pc_form=d.attributes['DW_AT_high_pc'].form,ranges=value(d,'DW_AT_ranges'),abstract_origin=value(d,'DW_AT_abstract_origin')))
    assert instances==inline['instances'] and len(instances)==1
    for row in record['source_dependencies']:
        text=read(root/row['source']);a,b=span(text,row['name']);assert sha(text[a:b].encode())==row['definition_sha256']
        assert row['standalone_native_range'] is None and row['inline_owner']=='0x24fbd70'
    assert len(record['helpers'])==record['helper_definitions']==record['native_helper_ranges']==32
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==14
    assert len(record['card_callers'])==61 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==18
    return dict(helper_definitions=32,native_helper_ranges=32,changed_helpers=14,complete_card_callers=61,changed_card_callers=18,
                native_frame_arrays=6,native_instruction_blocks=6,concrete_inline_instances=1,
                scope='93 complete direct native bodies/source definitions. Five Homelands lands, five storage lands, five Karoo lands, modal activation sources, Provoke/Sliver dispatch and death-effect callers paired. Native Grixis argument-order and Hunter info-slot quirks pinned in assembly, EXE accumulator references and Mind\'s Desire inline creation checked. Independent dependencies remain separately tracked; gameplay tests and built-in dialogs remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
