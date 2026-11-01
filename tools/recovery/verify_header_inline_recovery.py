"""Bind restored header helpers to abstract DIEs and caller-owned native code."""
import json
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value,value
from verify_inline_helpers import ranges
from verify_split_cards_recovery import contract
from verify_native_absolute_bindings import runtime_sections,stable_sections


def verify(proof,image,root=ROOT):
    path=proof.get('native_header_inline_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    dw=debug.dwarf();md=Cs(CS_ARCH_X86,CS_MODE_32);text=read(root/'manalink.h');instances=0;fragments=0
    for row in record['helpers']:
        a,b=span(text,row['name']);assert text[a:b]==row['definition'] and sha(text[a:b].encode())==row['definition_sha256']
        target=int(row['native_target_address'],16)
        assert any(n=='_'+row['callee'] and s>0 and k==32 and 0x2000000+image.pe.sections[s-1].VirtualAddress+v==target for n,v,s,k,c in image.symbols)
        for origin in row['native_origins']:
            d=dw.get_DIE_from_refaddr(int(origin['die_offset'],16));assert d.tag=='DW_TAG_subprogram'
            assert inherited_value(d,'DW_AT_name')==row['name'] and contract(dw,d)==origin['contract']
            assert not ranges(dw,d) and value(d,'DW_AT_inline') in [1,3]
            assert value(d,'DW_AT_external')==origin['external']
            for witness in origin['instances']:
                inline=dw.get_DIE_from_refaddr(int(witness['die_offset'],16));assert inline.tag=='DW_TAG_inlined_subroutine'
                assert inline.get_DIE_from_attribute('DW_AT_abstract_origin').offset==d.offset
                assert contract(dw,inline)==witness['contract']
                parent=inline.get_parent()
                while parent and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
                assert inherited_value(parent,'DW_AT_name')==witness['owner'];owners=ranges(dw,parent)
                actual=ranges(dw,inline)
                assert actual==[(int(p['address'],16),int(p['end'],16)) for p in witness['ranges']]
                calls=[]
                for p,(lo,hi) in zip(witness['ranges'],actual):
                    assert any(a<=lo<hi<=b for a,b in owners)
                    raw=image.pe.get_data(lo-0x2000000,hi-lo);assert raw.hex()==p['bytes'] and sha(raw)==p['sha256']
                    instructions=list(md.disasm(raw,lo));assert sum(i.size for i in instructions)==hi-lo
                    assert [[hex(i.address),i.mnemonic,i.op_str] for i in instructions]==p['instructions']
                    calls.extend(i.op_str for i in instructions if i.mnemonic=='call');fragments+=1
                assert calls==[hex(target)],(row['name'],witness['owner'],calls)
                instances+=1
    edits=[e for e in proof['source_edits'] if e.get('native_header_inline_extension')==path]
    assert len(edits)==2
    for r in record['source_edits']:assert any(e['before']==r['before'] and e['after']==r['after'] for e in edits)
    builds=[]
    for key in ['runtime_before','runtime_after']:
        artifact=record[key];im=Image((root/artifact['path']).read_bytes());assert sha(im.data)==artifact['dll_sha256'];builds.append(im)
    assert stable_sections(runtime_sections(builds[0]))==stable_sections(runtime_sections(builds[1]))
    assert len(record['helpers'])==5 and instances==21 and fragments==27
    return dict(restored_original_header_helpers=5,native_inline_instances=instances,native_caller_owned_fragments=fragments,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
