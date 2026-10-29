"""Check death/tribal-return source receipts against complete native ranges and frames."""
import csv,json
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value,value
from export_abi import type_of


def verify(proof,image,root=ROOT):
    path=proof.get('native_death_tribal_return_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    for row in record['helpers']+record['support_helpers']+record['card_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name']);assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16)
            native=inventory[part['address']];assert native['name']==row['name'] and native['high_pc']==part['end']
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
    assert abi['0x23634e0']['c1']['first_location']==[145,0] and abi['0x23634e0']['c2']['first_location']==[145,4]
    assert abi['0x23634e0']['trig_mode']['first_location']==[250,129,79,12,0,159]
    for name,number in [('cless',1),('black',0),('blue',0),('green',0),('red',0),('white',1)]:
        assert abi['0x25e7a60'][name]==dict(first_location=None,constant=number)
    arrays={};dynamic={};wanted={}
    for key in record['native_arrays']:
        at,name=key.split('::');wanted.setdefault(at,set()).add(name)
    for at,names in wanted.items():
        for d in walk(fn(at)):
            name=inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_variable' or name not in names:continue
            typ=type_of(d)
            if typ.tag!='DW_TAG_array_type':continue
            key=at+'::'+name;dims=[]
            for sub in typ.iter_children():
                upper=sub.attributes['DW_AT_upper_bound']
                if upper.form=='DW_FORM_ref4':
                    dims.append('dynamic');bound=sub.get_DIE_from_attribute('DW_AT_upper_bound');dynamic[key]=dict(form=upper.form,expressions=locs(bound))
                else:
                    assert upper.form in {'DW_FORM_data1','DW_FORM_data2','DW_FORM_data4'};dims.append(upper.value+1)
            element=type_of(typ)
            while element and 'DW_AT_byte_size' not in element.attributes and 'DW_AT_type' in element.attributes:element=type_of(element)
            row=dict(dimensions=dims,element_bytes=value(element,'DW_AT_byte_size'))
            if key in arrays:assert arrays[key]==row
            arrays[key]=row
    assert arrays==record['native_arrays'] and len(arrays)==9
    assert dynamic==record['native_dynamic_array_bounds'] and len(dynamic)==2
    for at in ['0x23e6150','0x23e9ca0']:
        assert arrays[at+'::stored']==dict(dimensions=['dynamic'],element_bytes=4)
        assert dynamic[at+'::stored']['expressions'][0]==[114,0,8,255,26,68,30,49,28,159]
    assert arrays['0x25e7a60::stored']==dict(dimensions=[18],element_bytes=4)
    assert arrays['0x20dc230::marked']==dict(dimensions=[2,151],element_bytes=1)
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    md=Cs(CS_ARCH_X86,CS_MODE_32)
    for name,row in record['native_instruction_blocks'].items():
        lo=int(row['address'],16);hi=int(row['end'],16);owner=inventory[row['owner']]
        assert int(owner['low_pc'],16)<=lo<hi<=int(owner['high_pc'],16)
        data=image.pe.get_data(lo-0x2000000,hi-lo);assert sha(data)==row['bytes_sha256']
        instructions=list(md.disasm(data,lo));assert sum(i.size for i in instructions)==hi-lo
        assert [[hex(i.address),i.mnemonic,i.op_str] for i in instructions]==row['instructions']
        pairs=[(i.mnemonic,i.op_str) for i in instructions]
        if name.endswith('_allocation'):
            assert ('lea','eax, [edx + edx*4]') in pairs and ('shl','eax, 4') in pairs
            assert ('call','0x26fc1d0') in pairs and ('sub','esp, eax') in pairs
        elif name.endswith('_copy'):
            assert ('cmp','ecx, -1') in pairs and ('dec','edx') in pairs
            assert ('mov','dword ptr [eax], 0xffffffff') in pairs and ('mov','dword ptr [eax + 4], 0xffffffff') in pairs
            assert ('cmp','edx, 0xa') in pairs
        else:
            assert name=='sigil_unbounded_sentinel_scan'
            assert all(i.op_str.endswith(', -1') for i in instructions if i.mnemonic=='cmp')
    for row in record['source_dependencies']:
        text=read(root/row['source']);a,b=span(text,row['name']);assert sha(text[a:b].encode())==row['definition_sha256']
        assert row['standalone_native_range'] is None
    assert len(record['helpers'])==record['helper_definitions']==record['native_helper_ranges']==32
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==24
    assert len(record['support_helpers'])==1 and record['support_helpers'][0]['source_changed']
    assert len(record['card_callers'])==42 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==16
    return dict(helper_definitions=32,native_helper_ranges=32,changed_helpers=24,supporting_helpers=1,changed_supporting_helpers=1,
                complete_card_callers=42,changed_card_callers=16,native_frame_arrays=9,native_instruction_blocks=5,
                scope='75 complete direct native bodies/source definitions. Paired live-card/legacy differences, optimized liege/Sigil arguments, native Reyhan dynamic storage/copy quirks and sentinel-only Sigil scan checked. Independent dependencies retain separate tracking; built-in dialogs and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
