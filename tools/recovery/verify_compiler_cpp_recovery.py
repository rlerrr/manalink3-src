"""Verify reviewed C++ utility and compiler-header bodies against native evidence."""
import csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from elftools.dwarf.locationlists import LocationParser
from canonicalize_helpers import ROOT,read,sha,span,mask
from inspect_release import Image,inherited_value,value
from export_abi import type_of,type_name


def source_body(text,name):
    if name!='csvid_t::iid':
        a,b=span(text,name)
        return text[a:b]
    # The general C extractor deliberately does not parse qualified const members.
    code=mask(text)
    m=re.search(r'(?m)^iid_t\s+csvid_t::iid\(\) const\s*\{',code)
    assert m,'Missing real csvid_t::iid() const member'
    end=m.end();depth=1
    while depth:
        depth+=(code[end]=='{')-(code[end]=='}');end+=1
    return text[m.start():end]


def evidence(proof,root=ROOT):
    path=proof.get('native_compiler_cpp_recovery')
    return json.loads((root/path).read_text()) if path else None


def coverage(proof,root=ROOT):
    """Return only hash-pinned, explicitly reviewed rows; no name-based exemptions."""
    record=evidence(proof,root)
    if not record:return {}
    result={}
    for row in record['cpp_sources']:
        body=source_body(read(root/row['source']),row['source_function'])
        assert sha(body.encode())==row['definition_sha256']
        result[int(row['address'],16)]=dict(category='reviewed_cpp_source',
            source=row['source'],source_function=row['source_function'],
            source_body_sha256=row['definition_sha256'],source_body=body,
            review='Complete native body, C++ ABI/storage and corresponding authored definition reviewed; dependencies remain separate.')
    for row in record['compiler_sources']:
        part=record['header_fragments'][row['header_fragment']]
        path=Path(part['path']);text=read(path);body=text[part['start']:part['end']]
        assert sha(path.read_bytes())==part['file_sha256']
        assert sha(body.encode())==part['body_sha256']
        result[int(row['address'],16)]=dict(category='reviewed_compiler_source',
            source=path.as_posix(),source_function=row['source_function'],
            source_body_sha256=part['body_sha256'],source_body=body,
            review='Complete native specialization, optimized parameters and corresponding toolchain header source reviewed. No per-TU byte identity, CRT or caller-equivalence claim.')
    assert len(result)==33
    return result


def verify(proof,image,root=ROOT):
    record=evidence(proof,root)
    if not record:return {}
    assert sha(image.data)==record['dll_sha256']
    dbg=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(dbg.data)==record['dbg_sha256']
    dw=dbg.dwarf();parser=LocationParser(dw.location_lists());md=Cs(CS_ARCH_X86,CS_MODE_32)
    inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    def locs(d):
        a=d.attributes.get('DW_AT_location')
        if not a:return []
        p=parser.parse_from_attribute(a,d.cu['version'],die=d)
        return [p.loc_expr] if hasattr(p,'loc_expr') else [r.loc_expr for r in p if hasattr(r,'loc_expr')]
    receipts={(e['source'],e.get('native_cpp_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['cpp_sources']+record['compiler_sources']:
        at=row['address'];native=inventory[at]
        assert (native['source'],native['name'],native['high_pc'],native['die_offset'])==(row['source'],row['name'],row['end'],row['die_offset'])
        lo=int(at,16);hi=int(row['end'],16);data=image.pe.get_data(lo-0x2000000,hi-lo)
        assert sha(data)==row['bytes_sha256']
        assert sha((root/row['raw_export']).read_bytes())==row['raw_export_sha256']
        block=record['native_instruction_blocks'][at];ins=list(md.disasm(data,lo))
        assert block['address']==at and block['end']==row['end'] and block['bytes_sha256']==sha(data)
        assert sum(i.size for i in ins)==hi-lo
        assert [[hex(i.address),i.mnemonic,i.op_str] for i in ins]==block['instructions']
        f=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        line=dw.line_program_for_CU(f.cu);no=inherited_value(f,'DW_AT_decl_file')
        assert line['file_entry'][no-1].name.decode('latin1')==row['declaration_file']
        assert inherited_value(f,'DW_AT_decl_line')==row['declaration_line']
        assert value(f.cu.get_top_DIE(),'DW_AT_producer')==row['producer']
        # Preserve the list: GCC adds duplicate constant-parameter DIEs.
        abi=[dict(tag=d.tag,name=inherited_value(d,'DW_AT_name'),type=type_name(type_of(d)),constant=inherited_value(d,'DW_AT_const_value'),locations=locs(d))
             for d in f.iter_children() if d.tag in {'DW_TAG_formal_parameter','DW_TAG_unspecified_parameters'}]
        assert abi==record['optimized_parameter_evidence'][at]
        if row['kind']=='cpp_source':
            body=source_body(read(root/row['source']),row['source_function'])
            assert sha(body.encode())==row['definition_sha256']
            assert (row['source'],row['source_function'],row['definition_sha256']) in receipts
        if row['name']=='snprintf':
            assert row['declaration_file']=='stdio.h' and row['declaration_line']==886
            assert abi[:3]==[dict(tag='DW_TAG_formal_parameter',name=n,type=t,constant=None,locations=[[145,o]])
                            for n,t,o in [('__stream','char *',0),('__n','size_t',4),('__format','char *',8)]]
            assert row['specialized_constants']==[v for v in abi[3:] if v['constant'] is not None or v['locations']]
            assert [i.op_str for i in ins if i.mnemonic=='call']==['0x26fd110']
            assert any(i.mnemonic=='lea' and i.op_str.endswith('[ebp + 0x14]') for i in ins)
    for row in record['header_dependencies']:
        assert sha(Path(row['path']).read_bytes())==row['sha256']
    for name,row in record['native_records'].items():
        t=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert value(t,'DW_AT_name')==name and value(t,'DW_AT_byte_size')==row['bytes']
        assert [dict(name=value(m,'DW_AT_name'),offset=value(m,'DW_AT_data_member_location'),type=type_name(type_of(m)),die_offset=hex(m.offset))
                for m in t.iter_children() if m.tag=='DW_TAG_member']==row['members']
    expected=dict(SetVoidptr_t=24,UnorderedMapIntInt_t=28,iid_t=4,csvid_t=4)
    assert {n:r['bytes'] for n,r in record['native_records'].items()}==expected
    for n in ['iid_t','csvid_t']:
        assert [(m['name'],m['offset'],m['type']) for m in record['native_records'][n]['members']]==[('raw',0,'int')]
    def member_symbols(binary):
        return [dict(name=n,storage=st) for n,a,s,k,st in binary.symbols
                if n=='__ZNK7csvid_t3iidEv' and 0<s<=len(binary.pe.sections)]
    method=next(r for r in record['cpp_sources'] if r['name']=='iid')
    assert member_symbols(dbg)==method['coff']==[dict(name='__ZNK7csvid_t3iidEv',storage=2)]
    assert [dbg.pe.OPTIONAL_HEADER.ImageBase+dbg.pe.sections[s-1].VirtualAddress+a
            for n,a,s,k,st in dbg.symbols if n=='__ZNK7csvid_t3iidEv' and 0<s<=len(dbg.pe.sections)]==[0x26eea80]
    native_member=dw.get_DIE_from_refaddr(int(method['die_offset'],16))
    assert type_name(type_of(native_member))=='iid_t'
    assert record['optimized_parameter_evidence']['0x26eea80'][0]['locations'][0]==[81]
    assert record['native_instruction_blocks']['0x26eea80']['instructions']==[
        ['0x26eea80','push','ebp'],['0x26eea81','mov','ebp, esp'],['0x26eea83','push','eax'],
        ['0x26eea84','mov','eax, dword ptr [ecx]'],['0x26eea86','mov','dword ptr [esp], eax'],
        ['0x26eea89','call','0x26b3410'],['0x26eea8e','leave',''],['0x26eea8f','ret','']]
    assert inventory['0x26b3410']['name']=='get_internal_card_id_from_csv_id'
    assert source_body(read(root/'functions/utility.cpp'),'csvid_t::iid')==(
        'iid_t csvid_t::iid() const\n{\n  return iid_t(get_internal_card_id_from_csv_id(raw));\n}')
    # Check the emitted real method and concrete 32-bit C++ records, never a C alias.
    rebuilt=Image((root/'ManalinkEh.dbg').read_bytes())
    assert member_symbols(rebuilt)==method['coff']
    current={};rebuilt_members=[]
    for cu in rebuilt.dwarf().iter_CUs():
        top=cu.get_top_DIE()
        if not (value(top,'DW_AT_name') or '').replace('\\','/').endswith('utility.cpp'):continue
        for d in top.iter_children():
            name=value(d,'DW_AT_name')
            if d.tag=='DW_TAG_subprogram' and inherited_value(d,'DW_AT_linkage_name')=='_ZNK7csvid_t3iidEv' and value(d,'DW_AT_low_pc') is not None:
                rebuilt_members.append(d)
            if name in expected and d.tag in {'DW_TAG_structure_type','DW_TAG_class_type'} and value(d,'DW_AT_byte_size') is not None:
                current[name]=dict(bytes=value(d,'DW_AT_byte_size'),members=[(value(m,'DW_AT_name'),value(m,'DW_AT_data_member_location')) for m in d.iter_children() if m.tag=='DW_TAG_member'])
    assert len(rebuilt_members)==1
    f=rebuilt_members[0]
    assert type_name(type_of(f))=='iid_t'
    this=next(d for d in f.iter_children() if d.tag=='DW_TAG_formal_parameter' and inherited_value(d,'DW_AT_name')=='this')
    a=this.attributes['DW_AT_location']
    p=LocationParser(f.cu.dwarfinfo.location_lists()).parse_from_attribute(a,f.cu['version'],die=this)
    exprs=[p.loc_expr] if hasattr(p,'loc_expr') else [r.loc_expr for r in p if hasattr(r,'loc_expr')]
    assert exprs[0]==[81]
    assert {n:r['bytes'] for n,r in current.items()}==expected
    for name in expected:
        assert current[name]['members']==[(m['name'],m['offset']) for m in record['native_records'][name]['members']]
    reviewed=coverage(proof,root)
    assert len(record['cpp_sources'])==7 and len(record['compiler_sources'])==26
    assert sum(r['name']=='snprintf' for r in record['compiler_sources'])==21
    return dict(cpp_source_functions=7,restored_cpp_members=1,retained_cpp_bodies=6,
        compiler_header_ranges=26,snprintf_specializations=21,library_templates=5,
        native_ranges=len(reviewed),native_and_rebuilt_cpp_records=4,
        scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
