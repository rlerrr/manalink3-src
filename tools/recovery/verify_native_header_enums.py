"""Verify recovered native enumerators and the compiled crew-event consumers."""
import json
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify
from verify_split_cards_recovery import native_ranges


def enumerators(debug,source='functions/functions.c'):
    result={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!=source:continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_typedef':continue
            name=value(d,'DW_AT_name');enum=unqualify(d)
            if enum is None or enum.tag!='DW_TAG_enumeration_type':continue
            result[name]={value(c,'DW_AT_name'):(value(c,'DW_AT_const_value'),hex(c.offset)) for c in enum.iter_children() if c.tag=='DW_TAG_enumerator'}
        return result
    raise AssertionError('Missing common-header CU')


def immediate_sites(image,start,end,constant):
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    raw=image.pe.get_data(start-image.pe.OPTIONAL_HEADER.ImageBase,end-start)
    ins=list(md.disasm(raw,start));assert sum(i.size for i in ins)==end-start
    return [dict(address=hex(i.address),bytes=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str) for i in ins if any(o.type==X86_OP_IMM and o.imm==constant for o in i.operands)]


def crew_ranges(debug):
    dw=debug.dwarf();result={}
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!='cards/kaladesh.c':continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_subprogram' or value(d,'DW_AT_declaration',False):continue
            name=inherited_value(d,'DW_AT_name')
            if name not in ['vehicle','this_creature_can_crew_a_vehicle','card_revoke_privileges']:continue
            ranges=native_ranges(dw,d)
            if ranges:result[name]=ranges
        return result
    raise AssertionError('Missing Kaladesh CU')


def verify(proof,image,root=ROOT):
    path=proof.get('native_header_enums_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    native=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(native.data)==record['native_dbg_sha256']
    current=Image((root/'ManalinkEh.dbg').read_bytes());compiled=Image((root/'ManalinkEh.dll').read_bytes())
    old=enumerators(native);new=enumerators(current);ranges=crew_ranges(current)
    for r in record['enumerators']:
        assert old[r['enum']][r['name']]==(r['value'],r['native_die_offset'])
        assert new[r['enum']][r['name']][0]==r['value'],r['name']
    edits=[e for e in proof['source_edits'] if e.get('native_header_enums_extension')==path]
    assert len(edits)==len(record['source_edits'])
    for edit in record['source_edits']:
        assert any(e['before']==edit['before'] and e['after']==edit['after'] for e in edits)
    for r in record['crew_consumers']:
        t=read(root/r['source']);a,b=span(t,r['name']);assert sha(t[a:b].encode())==r['definition_sha256']
        lo=int(r['native_address'],16);hi=int(r['native_end'],16)
        assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==r['native_bytes_sha256']
        assert immediate_sites(image,lo,hi,0x1d00)==r['native_event_sites']
        matches=[(0x2000000+compiled.pe.sections[s-1].VirtualAddress+v) for n,v,s,k,c in current.symbols if n=='_'+r['name'] and s>0 and k==32]
        assert len(set(matches))==1
        start=matches[0]
        # COFF entries have no reliable size, so use the rebuilt DWARF bounds.
        parts=ranges[r['name']];assert len(parts)==1 and parts[0][0]==start
        assert immediate_sites(compiled,*parts[0],0x1d00),r['name']
        assert not immediate_sites(compiled,*parts[0],0x4f00),r['name']
    return dict(recovered_native_enum_names=sum(r['previous_value'] is None for r in record['enumerators']),corrected_enum_values=sum(r['previous_value'] is not None for r in record['enumerators']),compiled_crew_consumers=len(record['crew_consumers']),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
