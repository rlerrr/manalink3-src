"""Validate inline-only helper definitions against their native caller evidence."""
import json
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value
from verify_inline_helpers import ranges
from verify_split_cards_recovery import contract
from global_initializers import inherited_type
from object_initializers import unqualify
from verify_native_absolute_bindings import runtime_sections,stable_sections


def verify(proof,image,root=ROOT):
    path=proof.get('native_inline_predicate_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    dw=debug.dwarf();md=Cs(CS_ARCH_X86,CS_MODE_32);instances=0;fragments=0;owners=set()
    for row in record['helpers']:
        text=read(root/row['source']);a,b=span(text,row['name']);assert text[a:b]==row['definition'] and sha(text[a:b].encode())==row['definition_sha256']
        for origin in row['native_origins']:
            d=dw.get_DIE_from_refaddr(int(origin['die_offset'],16));assert d.tag=='DW_TAG_subprogram' and not ranges(dw,d)
            assert value(d,'DW_AT_inline')==origin['inline']==1 and not value(d,'DW_AT_external',False)
            assert inherited_value(d,'DW_AT_name')==row['name'] and contract(dw,d)==origin['contract']
            for witness in origin['instances']:
                inline=dw.get_DIE_from_refaddr(int(witness['die_offset'],16));assert inline.tag=='DW_TAG_inlined_subroutine'
                assert inline.get_DIE_from_attribute('DW_AT_abstract_origin').offset==d.offset
                assert contract(dw,inline)==witness['contract']
                parent=dw.get_DIE_from_refaddr(int(witness['owner_die_offset'],16));owner_ranges=ranges(dw,parent)
                actual=inline.get_parent()
                while actual and actual.tag!='DW_TAG_subprogram':actual=actual.get_parent()
                assert actual.offset==parent.offset
                assert inherited_value(parent,'DW_AT_name')==witness['owner']
                assert owner_ranges==[(int(p['address'],16),int(p['end'],16)) for p in witness['native_owner_ranges']]
                for p,(lo,hi) in zip(witness['native_owner_ranges'],owner_ranges):assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==p['sha256']
                source_owner=witness['source_owner'];t=read(root/source_owner['source']);x,y=span(t,source_owner['name']);assert sha(t[x:y].encode())==source_owner['definition_sha256']
                parts=ranges(dw,inline);assert parts==[(int(p['address'],16),int(p['end'],16)) for p in witness['ranges']]
                for p,(lo,hi) in zip(witness['ranges'],parts):
                    assert any(x<=lo<hi<=y for x,y in owner_ranges)
                    raw=image.pe.get_data(lo-0x2000000,hi-lo);assert raw.hex()==p['bytes'] and sha(raw)==p['sha256']
                    ins=list(md.disasm(raw,lo));assert sum(i.size for i in ins)==hi-lo
                    assert [[hex(i.address),i.mnemonic,i.op_str] for i in ins]==p['instructions'];fragments+=1
                instances+=1;owners.add((origin['source'],parent.offset))
    for table in record['native_tables_and_support']:
        raw=image.pe.get_data(int(table['address'],16)-0x2000000,len(bytes.fromhex(table['bytes'])))
        assert raw.hex()==table['bytes'] and sha(raw)==table['sha256']
    enum=record['party_enum'];d=dw.get_DIE_from_refaddr(int(enum['native_function_die'],16));t=unqualify(inherited_type(d))
    assert t.tag=='DW_TAG_enumeration_type' and value(t,'DW_AT_byte_size')==4
    assert [(value(c,'DW_AT_name'),value(c,'DW_AT_const_value')) for c in t.iter_children() if c.tag=='DW_TAG_enumerator']==[tuple(x) for x in enum['enumerators']]
    assert enum['definition'] in read(root/'cards/zendikar_rising.c')
    edits=[e for e in proof['source_edits'] if e.get('native_inline_predicate_extension')==path]
    assert len(edits)==len(record['source_edits'])==14
    for r in record['source_edits']:assert any(all(e[k]==r[k] for k in ['source','before','after']) for e in edits)
    builds=[]
    for key in ['runtime_before','runtime_after']:
        artifact=record[key];im=Image((root/artifact['path']).read_bytes());assert sha(im.data)==artifact['dll_sha256'];builds.append(im)
    assert stable_sections(runtime_sections(builds[0]))==stable_sections(runtime_sections(builds[1]))
    assert len(record['helpers'])==20
    return dict(restored_inline_helper_definitions=20,native_inline_instances=instances,native_inline_fragments=fragments,native_callers=len(owners),source_units=14,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
