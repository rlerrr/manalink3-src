"""Audit classic death/land body receipts, native frames and EXE references."""
import csv,json,re
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value,value
from export_abi import type_of


def verify(proof,image,root=ROOT):
    path=proof.get('native_classic_death_land_recovery')
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
    assert abi['0x2160790']=={'player':{'first_location':[80],'constant':None},'card':{'first_location':[82],'constant':None},'event':{'first_location':[81],'constant':None},'mana_color':{'first_location':[145,0],'constant':None}}
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
            element=type_of(typ);row=dict(dimensions=dims,element_bytes=element.attributes['DW_AT_byte_size'].value)
            key=at+'::'+name
            if key in arrays:assert arrays[key]==row
            arrays[key]=row
    assert arrays==record['native_arrays'] and len(arrays)==13
    assert arrays['0x2043920::added']==dict(dimensions=[18],element_bytes=4)
    assert arrays['0x204f200::added']==dict(dimensions=[20],element_bytes=4)
    assert arrays['0x2043d50::buffer']==dict(dimensions=[500],element_bytes=1)
    for row in record['native_card_ids']:
        die=dw.get_DIE_from_refaddr(int(row['die_offset'],16));assert die.tag=='DW_TAG_enumerator'
        assert value(die,'DW_AT_name')==row['name'] and value(die,'DW_AT_const_value')==row['value']
        assert re.search(r'\b'+row['name']+r'\s*=\s*'+str(row['value'])+r'\b',read(root/'card_id.h'))
    assert record['native_card_ids']==[dict(name='CARD_ID_BLIGHTBEETLE',value=20284,die_offset='0x13b066c')]
    globals_=record['native_global_evidence']
    symbols={s[0]:hex(s[1]) for s in original.symbols if s[2]==-1 and s[0]=='_tapped_for_mana_color'}
    assert symbols==globals_['absolute_symbols']=={'_tapped_for_mana_color':'0x7904bc'}
    assert globals_['check_abilities_keywords']=='0x78fef0'
    assert '#define check_abilities_keywords EXE_DWORD(0x78fef0)' in read(root/'manalink.h')
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    md=Cs(CS_ARCH_X86,CS_MODE_32);sites=globals_['first_strike_instruction_blocks'];assert len(sites)==2
    for site in sites:
        at=int(site['address'],16);data=bytes.fromhex(site['bytes_hex']);assert image.pe.get_data(at-0x2000000,len(data))==data
        ins=[dict(mnemonic=i.mnemonic,op_str=i.op_str) for i in md.disasm(data,at)]
        assert ins==site['instructions']==[dict(mnemonic='mov',op_str='eax, dword ptr [0x78fef0]'),dict(mnemonic='or',op_str='ah, 1'),dict(mnemonic='mov',op_str='dword ptr [0x78fef0], eax')]
    callers={r['name']:r for r in record['card_callers']}
    for name,color in [('card_stromgald_crusader','COLOR_BLACK'),('card_white_shield_crusader','COLOR_WHITE')]:
        row=callers[name];text=read(root/row['source']);a,b=span(text,name);assert 'return coldsnap_crusaders(player, card, event, '+color+');' in text[a:b]
    assert len(record['helpers'])==record['helper_definitions']==record['native_helper_ranges']==32
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==24
    assert len(record['card_callers'])==31 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==10
    return dict(helper_definitions=32,native_helper_ranges=32,changed_helpers=24,complete_card_callers=31,changed_card_callers=10,
                native_frame_arrays=13,native_card_ids=1,
                scope='Complete source/native-range receipts, native fixed arrays, Crusader calling convention and both color callers, EXE first-strike instruction blocks and Blightbeetle enum value. Native behavior/quirks retained; built-in dialog backends and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
