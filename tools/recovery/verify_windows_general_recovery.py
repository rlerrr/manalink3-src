"""Verify Windows/general C recovery against native bodies, ABI, storage and UI boundaries."""
import csv,json,re,struct
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value,value
from export_abi import type_of
from verify_rules_challenge_recovery import without_comments,integer


def verify(proof,image,root=ROOT):
    path=proof.get('native_windows_general_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    def body(row):
        text=read(root/row['source']);a,b=span(text,row['name']);return text[a:b]
    for row in record['helpers']:
        assert (row['source'],row['name'],row.get('receipt_sha256',row['definition_sha256'])) in receipts,row['name']
        assert sha(body(row).encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16);native=inventory[part['address']]
            assert native['name']==row['name'] and native['source']==row['source'] and native['high_pc']==part['end']
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
    for row in record['source_dependencies']:
        assert sha(body(row).encode())==row['definition_sha256']
        assert any(e['source']==row['source'] and e.get('native_inline_source_dependency',{}).get('function')==row['name']
                   and e['after_sha256']==row['definition_sha256'] for e in proof['source_edits'])
    for row in record['source_moves']:
        assert span(read(root/row['from']),row['name']) is None
        assert any(e['source']==row['from'] and e.get('native_source_move_retirement',{}).get('function')==row['name'] and e['after']=='' for e in proof['source_edits'])
        assert row['to']=='functions/windows.c'
    assert len(record['source_moves'])==2
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)==record['dbg_sha256']=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw=original.dwarf();parser=LocationParser(dw.location_lists())
    def fn(at):return dw.get_DIE_from_refaddr(int(inventory[at]['die_offset'],16))
    def locs(d):
        a=d.attributes.get('DW_AT_location')
        if not a:return []
        p=parser.parse_from_attribute(a,d.cu['version'],die=d)
        return [p.loc_expr] if hasattr(p,'loc_expr') else [r.loc_expr for r in p if hasattr(r,'loc_expr')]
    def abi(at):
        return {inherited_value(d,'DW_AT_name'):dict(first_location=next(iter(locs(d)),None),constant=inherited_value(d,'DW_AT_const_value'))
                for d in fn(at).iter_children() if d.tag=='DW_TAG_formal_parameter'}
    assert {at:abi(at) for at in record['optimized_parameter_evidence']}==record['optimized_parameter_evidence']
    assert len(record['optimized_parameter_evidence'])==39
    abi_map=record['optimized_parameter_evidence']
    assert abi_map['0x2656550']['offset_x']==dict(first_location=None,constant=0)
    assert abi_map['0x2656550']['offset_y']['first_location']==[145,0]
    assert abi_map['0x2656550']['bmp']['first_location']==[145,4]
    assert abi_map['0x2656550']['num_counters']['first_location']==[145,8]
    assert abi_map['0x2656550']['blt_fn']['first_location']==[145,12]
    assert abi_map['0x2656850']['player']['first_location']==[82]
    assert abi_map['0x2656850']['card']['first_location']==[81]
    assert abi_map['0x2656850']['is_damage_card']['first_location']==[145,0]
    assert abi_map['0x2656f70']['force_to_activation']['first_location']==[145,4]
    assert abi_map['0x265b780']['force_to_activation']['first_location']==[145,28]
    for row in record['native_frame_arrays']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert d.tag=='DW_TAG_variable' and inherited_value(d,'DW_AT_name')==row['name']
        assert d.cu==fn(row['owner']).cu
        t=type_of(d);assert t.tag=='DW_TAG_array_type';dims=[]
        for s in t.iter_children():
            a=s.attributes['DW_AT_upper_bound'];v=dict(form=a.form)
            if a.form=='DW_FORM_ref4':v['expressions']=locs(s.get_DIE_from_attribute('DW_AT_upper_bound'))
            elif a.form=='DW_FORM_exprloc':v['expressions']=[a.value]
            else:
                assert a.form in {'DW_FORM_data1','DW_FORM_data2','DW_FORM_data4'}
                v['extent']=a.value+1
            dims.append(v)
        assert dims==row['dimensions']
        e=type_of(t)
        while e and 'DW_AT_byte_size' not in e.attributes and 'DW_AT_type' in e.attributes:e=type_of(e)
        assert value(e,'DW_AT_byte_size')==row['element_bytes']
    assert len(record['native_frame_arrays'])==22
    by_owner={}
    for r in record['native_frame_arrays']:by_owner.setdefault(r['owner'],{})[r['name']]=r
    for at in ['0x2658a70','0x2658b40','0x2658bf0']:
        assert by_owner[at]['path']['dimensions'][0]['extent']==297
    assert by_owner['0x26e9c50']['choice']['dimensions'][0]['extent']==16
    assert by_owner['0x26e9c50']['choice']['element_bytes']==36
    for name,row in record['native_records'].items():
        t=dw.get_DIE_from_refaddr(int(row['die_offset'],16));assert value(t,'DW_AT_byte_size')==row['bytes']
        members=[dict(name=value(m,'DW_AT_name'),offset=value(m,'DW_AT_data_member_location'),die_offset=hex(m.offset)) for m in t.iter_children() if m.tag=='DW_TAG_member']
        assert members==row['members']
    assert {n:r['bytes'] for n,r in record['native_records'].items()}==dict(bitset128=16,choice_t=36,card_instance_t=300,card_ext_t=192,inst_and_ext_t=492)
    assert {m['name']:m['offset'] for m in record['native_records']['inst_and_ext_t']['members']}==dict(inst=0,ext=300)
    for row in record['native_globals']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert inherited_value(d,'DW_AT_name')==row['name'] and locs(d)==row['locations']
        assert [value(s,'DW_AT_upper_bound')+1 for s in type_of(d).iter_children()]==row['dimensions']
    assert {r['name']:r['dimensions'] for r in record['native_globals']}==dict(ability_tooltip_names=[77],counter_names=[188])
    parts=[dict(symbol=n,address=hex(image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+a),storage=st)
           for n,a,s,k,st in original.symbols if n.startswith(('_draw_smallcard_normal','_draw_smallcard_special_effect_card','_get_icons')) and s>0]
    assert parts==record['native_compiler_partitions'] and len(parts)==5
    def walk(d):
        yield d
        for c in d.iter_children():yield from walk(c)
    for row in record['native_inline_dependencies']:
        actual=[dict(die_offset=hex(d.offset),low_pc=value(d,'DW_AT_low_pc'),high_pc=value(d,'DW_AT_high_pc'),ranges=value(d,'DW_AT_ranges'),abstract_origin=value(d,'DW_AT_abstract_origin'))
                for d in walk(fn(row['owner'])) if d.tag=='DW_TAG_inlined_subroutine' and inherited_value(d,'DW_AT_name')==row['name']]
        assert actual==row['instances'] and actual
    assert {r['name'] for r in record['native_inline_dependencies']}=={'card_instances_will_have_same_icons','get_name_for_ability_icon_at_point'}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    md=Cs(CS_ARCH_X86,CS_MODE_32)
    for row in record['native_instruction_blocks'].values():
        lo=int(row['address'],16);hi=int(row['end'],16);owner=inventory[row['owner']]
        assert lo==int(owner['low_pc'],16) and hi==int(owner['high_pc'],16)
        data=image.pe.get_data(lo-0x2000000,hi-lo);assert sha(data)==row['bytes_sha256']
        ins=list(md.disasm(data,lo));assert sum(i.size for i in ins)==hi-lo
        assert [[hex(i.address),i.mnemonic,i.op_str] for i in ins]==row['instructions']
    blocks=record['native_instruction_blocks'];assert len(blocks)==39
    for row in record['native_title_calls']:
        ins=blocks[row['owner'][2:]]['instructions']
        at=next(i for i,r in enumerate(ins) if r[0]==row['call'])
        assert ins[at][1:]==['call','dword ptr [0x27b535c]']
        assert row['ninth_argument_store'] in ins[max(0,at-32):at]
        assert row['ninth_argument_store'][2].startswith('dword ptr [esp + 0x20],')
    assert len(record['native_title_calls'])==6
    assert len(record['native_redisplay_calls'])==2
    for row in record['native_redisplay_calls']:
        ins=blocks[row['owner'][2:]]['instructions']
        at=next(i for i,r in enumerate(ins) if r[0]==row['call'])
        assert ins[at][1:]==['call','0x437ec0'] and row['arguments']==[0,255]
        assert any(m=='mov' and o=='dword ptr [esp], 0' for a,m,o in ins[max(0,at-7):at])
        assert any(m=='mov' and o=='dword ptr [esp + 4], 0xff' for a,m,o in ins[max(0,at-7):at])
    assert read(root/'functions/windows.c').count('EXE_FN(int, 0x437ec0, int, int)(0, 0xff);')==2
    contracts=record['native_api_contracts']
    for key,digest in [('path','sha256'),('script','script_sha256'),('export_log','export_log_sha256')]:
        assert sha((root/contracts[key]).read_bytes())==contracts[digest]
    assert [l.split('\t') for l in (root/contracts['path']).read_text().splitlines()]==contracts['rows']
    image.pe.parse_data_directories(directories=[1])
    imports={hex(s.address):(ent.dll.decode(),s.name.decode()) for ent in image.pe.DIRECTORY_ENTRY_IMPORT for s in ent.imports if s.name}
    for row in contracts['rows']:
        if row[1]!='direct':assert imports[row[0]]==(row[1],row[2])
    assert len(contracts['rows'])==66 and sum(r[1]!='direct' for r in contracts['rows'])==48
    assert 'RECOVERY_WINDOWS_INTERFACES_COMPLETE count=66' in (root/contracts['export_log']).read_text()
    assert 'RECOVERY_EXPORT_COMPLETE success=39 failed=0' in (root/contracts['export_log']).read_text()
    for row in record['native_literals']:
        data=row['value'].encode('latin1')+b'\0'
        assert sha(data)==row['bytes_sha256'] and image.pe.get_data(int(row['address'],16)-0x2000000,len(data))==data
    assert len(record['native_literals'])==8
    table=record['native_icon_mapping'];data=struct.pack('<17i',*table['values'])
    assert sha(data)==table['bytes_sha256'] and image.pe.get_data(int(table['address'],16)-0x2000000,len(data))==data
    text=without_comments(read(root/'functions/windows.c'))
    m=re.search(r'internal_from_icon\[17\]\s*=\s*\{([\s\S]*?)\};',text);assert m
    names={}
    defs=without_comments(read(root/'defs.h'))
    for row in record['native_enums']['icons']:
        # This batch verifies all icon names/values, including the native69 spelling.
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert value(d,'DW_AT_name')==row['name'] and value(d,'DW_AT_const_value')==row['value']
        found=re.search(r'\b'+row['name']+r'\s*=\s*(\d+)',defs);assert found and int(found[1])==row['value']
        names[row['name']]=row['value']
    assert [integer(s,names) for s in m[1].split(',') if s.strip()]==table['values']
    for row in record['native_restored_enums']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert d.tag=='DW_TAG_enumerator' and value(d,'DW_AT_name')==row['name'] and value(d,'DW_AT_const_value')==row['value']
        text=without_comments(read(root/row['source']))
        found=re.search(r'\b'+row['name']+r'\s*=\s*([^,}\n]+)',text);assert found
        assert integer(found[1],{})==row['value']
    assert len(record['native_restored_enums'])==7
    deferred=record['deferred_boundaries'];assert len(deferred)==2
    for row in deferred:
        assert not row['counted_as_recovered'] and sha(body(row).encode())==row['definition_sha256']
        registration={k:row[k] for k in ['source','name','native_address','return_value']}
        assert registration in proof['additional_source_stubs']
        assert abi('0x'+row['native_address'])==record['deferred_chooser_abi']['0x'+row['native_address']]
        assert not any(p['address']=='0x'+row['native_address'] for helper in record['helpers'] for p in helper['native_ranges'])
    window=read(root/'functions/windows.c')
    assert 'calloc(1, sizeof(inst_and_ext_t))' in window
    assert 'xactual.inst.ext = xstashed->inst.ext;' in window
    assert 'memcmp(xstashed, &xactual, sizeof(xactual))' in window
    assert 'SP_KEYWORD_SQUIRRELLINK' in window
    assert len(record['helpers'])==37 and sum(len(r['native_ranges']) for r in record['helpers'])==39
    assert sum(r['source_changed'] for r in record['helpers'])==15
    assert len(record['source_dependencies'])==2 and sum(r['source_changed'] for r in record['source_dependencies'])==1
    return dict(helper_definitions=37,native_helper_ranges=39,changed_helper_definitions=15,
                moved_to_original_unit=2,concrete_inline_dependencies=2,changed_inline_dependencies=1,
                native_frame_arrays=22,native_records=5,native_title_call_frames=6,deferred_chooser_boundaries=2,
                scope='Complete Windows/general C bodies and concrete inline dependencies reviewed. Real chooser boundaries remain stubbed and excluded from recovery credit; EXE dialog backend and gameplay remain separate.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
