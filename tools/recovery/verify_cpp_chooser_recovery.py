"""Verify the resumed C++ card-picker recovery and retire only its pinned stubs."""
import csv,json,re,struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name

SOURCE='functions/choose_a_card.cpp'


def evidence(proof,root=ROOT):
    path=proof.get('native_cpp_chooser_recovery')
    if not path:return None
    support=next(r for r in proof['interface_supporting_inputs'] if r['path']==path)
    assert sha((root/path).read_bytes())==support['sha256']
    return json.loads((root/path).read_text())


def coverage(proof,root=ROOT):
    record=evidence(proof,root)
    if not record:return {}
    text=read(root/SOURCE)
    for row in record['header_dependencies']:
        assert sha(Path(row['path']).read_bytes())==row['sha256']
    for row in record['functions']:
        if row['kind']=='cpp_source':
            a,b=span(text,row['name']);assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
    a,b=span(text,'generate_choose_a_card_dlg_template')
    assert sha(text[a:b].encode())==record['template_definition_sha256']
    return {int(r['address'],16):dict(category='reviewed_cpp_source' if r['kind']=='cpp_source' else 'reviewed_compiler_source',
                source=SOURCE,source_function=r['name'] if r['kind']=='cpp_source' else 'dlgproc_choose_a_card_helper',
                source_body_sha256=r.get('definition_sha256',sha(text.encode())),
                source_body=text[slice(*span(text,r['name']))] if r['kind']=='cpp_source' else text,
                review='Complete native chooser body and C++/Win32 contracts reviewed. Compiler artifacts come from the pinned STL/stdio source and vector lifecycle; interactive behavior is not runtime-tested.')
            for r in record['functions']}


def resolved_stub_addresses(proof,root=ROOT):
    # Historical audits must still see and verify the exact original placeholders.
    from canonicalize_helpers import _historical_active
    if _historical_active:return set()
    record=evidence(proof,root)
    if not record:return set()
    covered=coverage(proof,root)
    authored={int(r['address'],16):r for r in record['functions'] if r['kind']=='cpp_source'}
    result=set()
    for stub in record['resolved_stubs']:
        at=int(stub['native_address'],16)
        assert at in covered and authored[at]['name']==stub['name'] and stub['source']==SOURCE
        result.add(at)
    assert len(result)==5
    return result


def verify(proof,image,root=ROOT):
    record=evidence(proof,root)
    if not record:return {}
    assert sha(image.data)==record['dll_sha256']
    dbg=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(dbg.data)==record['dbg_sha256']
    dw=dbg.dwarf();lp=LocationParser(dw.location_lists())
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    md=Cs(CS_ARCH_X86,CS_MODE_32)
    def locs(d):
        a=d.attributes.get('DW_AT_location')
        if not a:return []
        p=lp.parse_from_attribute(a,d.cu['version'],die=d)
        return [p.loc_expr] if hasattr(p,'loc_expr') else [r.loc_expr for r in p if hasattr(r,'loc_expr')]
    receipt={(e['source'],e.get('native_cpp_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['functions']:
        old=inventory[row['address']]
        assert (old['name'],old['source'],old['high_pc'],old['die_offset'])==(row['name'],SOURCE,row['end'],row['die_offset'])
        lo=int(row['address'],16);hi=int(row['end'],16);data=image.pe.get_data(lo-0x2000000,hi-lo)
        assert sha(data)==row['bytes_sha256']
        assert sha((root/row['raw_export']).read_bytes())==row['raw_export_sha256']
        ins=list(md.disasm(data,lo));assert sum(i.size for i in ins)==hi-lo
        assert [[hex(i.address),i.mnemonic,i.op_str] for i in ins]==record['native_instruction_blocks'][row['address']]['instructions']
        f=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        no=inherited_value(f,'DW_AT_decl_file');line=dw.line_program_for_CU(f.cu)
        assert (line['file_entry'][no-1].name.decode('latin1') if no else None)==row['declaration_file']
        assert inherited_value(f,'DW_AT_decl_line')==row['declaration_line']
        abi=[dict(tag=d.tag,name=inherited_value(d,'DW_AT_name'),type=type_name(type_of(d)),constant=inherited_value(d,'DW_AT_const_value'),locations=locs(d))
             for d in f.iter_children() if d.tag in {'DW_TAG_formal_parameter','DW_TAG_unspecified_parameters'}]
        assert abi==record['optimized_parameter_evidence'][row['address']]
        if row['kind']=='cpp_source':
            assert (SOURCE,row['name'],row['definition_sha256']) in receipt
    abi=record['optimized_parameter_evidence']
    assert [(r['name'],r['locations'][0] if r['locations'] else None,r['constant']) for r in abi['0x26ef2c0']]==[
        ('constraints',[80],None),('prompt',[82],None),('colors',[81],None),
        ('types',[145,0],None),('allow_cancel',[145,4],None),('initial_color',[145,8],None),
        ('initial_type',[145,12],None),('who_chooses',None,0)]
    assert [(r['name'],r['locations'][0]) for r in abi['0x26ef950']]==[
        ('hwnd',[80]),('want_col',[82]),('want_type',[81]),('constraints',[145,0])]
    for name,row in record['native_records'].items():
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16));assert value(d,'DW_AT_byte_size')==row['bytes']==28
        assert [dict(name=value(m,'DW_AT_name'),offset=value(m,'DW_AT_data_member_location'),type=type_name(type_of(m)),die_offset=hex(m.offset))
                for m in d.iter_children() if m.tag=='DW_TAG_member']==row['members']
    assert {r['name']:r['offset'] for r in record['native_records']['choose_a_card_args_t']['members']}==dict(colors=0,types=4,prompt=8,constraints=12,allow_cancel=16,initial_btn_color=20,initial_btn_type=24)
    for row in record['native_globals']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert inherited_value(d,'DW_AT_name')==row['name'] and type_name(type_of(d))==row['type'] and locs(d)==row['locations']
    for row in record['native_frame_arrays']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert inherited_value(d,'DW_AT_name')==row['name']=='path'
        assert [value(s,'DW_AT_upper_bound')+1 for s in type_of(d).iter_children()]==row['dimensions']==[262]
    text=read(root/SOURCE)
    for row in record['native_control_enum']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert value(d,'DW_AT_name')==row['name'] and value(d,'DW_AT_const_value')==row['value']
        assert re.search(r'\b'+row['name']+r'\s*=\s*'+str(row['value'])+r'\b',text),row
    template=record['dialog_template'];data=bytes.fromhex(template['hex'])
    assert len(data)==template['bytes']==990 and sha(data)==template['sha256']
    assert image.pe.get_data(int(template['address'],16)-0x2000000,990)==data
    a,b=span(text,'generate_choose_a_card_dlg_template');body=text[a:b]
    assert bytes(int(h,16) for h in re.findall(r'0x([0-9a-f]{2})\b',body))==data
    assert struct.unpack('<H',data[8:10])[0]==22
    table=record['last_type_table'];data=struct.pack('<9i',*table['values'])
    assert sha(data)==table['sha256'] and image.pe.get_data(int(table['address'],16)-0x2000000,len(data))==data
    assert [int(v.strip()) for v in re.search(r'types_by_button\[9\]\s*=\s*\{([^}]+)',text)[1].split(',')]==table['values']
    contracts=record['native_api_contracts']
    for key,digest in [('path','sha256'),('export_log','export_log_sha256')]:
        assert sha((root/contracts[key]).read_bytes())==contracts[digest]
    assert [l.split('\t') for l in (root/contracts['path']).read_text().splitlines()]==contracts['rows']
    image.pe.parse_data_directories(directories=[1])
    imports={hex(s.address):(e.dll.decode(),s.name.decode()) for e in image.pe.DIRECTORY_ENTRY_IMPORT for s in e.imports if s.name}
    for row in contracts['rows']:
        if row[1]!='direct':assert imports[row[0]]==(row[1],row[2])
    log=(root/contracts['export_log']).read_text()
    assert 'RECOVERY_EXPORT_COMPLETE success=18 failed=0' in log
    assert 'RECOVERY_WINDOWS_INTERFACES_COMPLETE count=84' in log
    assert 'RECOVERY_CHOOSER_MEMBER int __thiscall iid(void * self) ECX:4' in log
    assert '_choose_a_card = 0x4a9750;' not in read(root/'manalink.lds')
    rebuilt=Image((root/'ManalinkEh.dbg').read_bytes())
    for name in ['choose_a_card','choose_a_card_allow_cancel_with_defaults','choose_a_card_iid','choose_a_card_test','choose_a_card_test_ct','choose_a_card_test_ct_allow_cancel','choose_a_card_get_last_options']:
        matches=[s for s in rebuilt.functions() if s[0]=='_'+name]
        assert len(matches)==1 and matches[0][2]>0,name
        assert rebuilt.pe.OPTIONAL_HEADER.ImageBase+rebuilt.pe.sections[matches[0][2]-1].VirtualAddress+matches[0][1]>0x2000000
    layouts=[]
    for cu in rebuilt.dwarf().iter_CUs():
        top=cu.get_top_DIE()
        if not (value(top,'DW_AT_name') or '').replace('\\','/').endswith('choose_a_card.cpp'):continue
        for d in top.iter_children():
            if d.tag=='DW_TAG_structure_type' and value(d,'DW_AT_name')=='choose_a_card_args_t' and value(d,'DW_AT_byte_size'):
                layouts.append(dict(bytes=value(d,'DW_AT_byte_size'),members=[(value(m,'DW_AT_name'),value(m,'DW_AT_data_member_location')) for m in d.iter_children() if m.tag=='DW_TAG_member']))
    native_args=record['native_records']['choose_a_card_args_t']
    assert layouts==[dict(bytes=28,members=[(m['name'],m['offset']) for m in native_args['members']])]
    callback=[s for s in rebuilt.functions() if 'dlgproc_choose_a_card' in s[0] and s[0].endswith('@16')]
    assert len(callback)==1,'Expected real stdcall dialog callback with four stack arguments'
    assert all(r[2]=='0x10' for r in record['native_instruction_blocks']['0x26efbb0']['instructions'] if r[1]=='ret')
    covered=coverage(proof,root);assert len(covered)==18
    assert len(resolved_stub_addresses(proof,root))==5
    return dict(cpp_source_functions=10,compiler_generated_ranges=8,complete_native_ranges=18,
                restored_stub_boundaries=5,dialog_template_bytes=990,dialog_controls=22,
                scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
