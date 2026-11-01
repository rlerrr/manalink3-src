"""Bind reviewed damage, combat-AI, phasing and trigger-dispatch card bodies to source, DWARF, native bytes and exports."""
import json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value
from verify_split_cards_recovery import native_ranges,contract


def verify(proof,image,root=ROOT):
    path=proof.get('native_damage_dispatch_cards_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text(encoding='utf-8'));assert sha(image.data)==record['dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['dbg_sha256']
    dw=debug.dwarf();md=Cs(CS_ARCH_X86,CS_MODE_32)
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['cards']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert inherited_value(d,'DW_AT_name')==row['name'] and contract(dw,d)==row['contract']
        assert native_ranges(dw,d)==[(int(p['address'],16),int(p['end'],16)) for p in row['native_ranges']]
        t=read(root/row['source']);a,b=span(t,row['name']);assert sha(t[a:b].encode())==row['definition_sha256'],row['name']
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16);data=image.pe.get_data(lo-0x2000000,hi-lo)
            assert sha(data)==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
            ins=list(md.disasm(data,lo));assert sum(i.size for i in ins)==hi-lo
            assert [[hex(i.address),i.mnemonic,i.op_str] for i in ins]==part['instructions']
    for row in record['source_dependencies']:
        t=read(root/row['source']);a,b=span(t,row['name']);assert sha(t[a:b].encode())==row['definition_sha256'],row['name']
    for row in record['native_data']:
        data=bytes.fromhex(row['bytes']);assert sha(data)==row['sha256']
        assert image.pe.get_data(int(row['address'],16)-0x2000000,len(data))==data
    for row in record['support_helpers']:
        t=read(root/row['source']);a,b=span(t,row['name'])
        assert sha(t[a:b].encode())==row['definition_sha256']
        assert (row['source'],row['name'],row['definition_sha256']) in receipts
        assert row['native_owner'] in {r['name'] for r in record['cards']}
    for row in record['support_contracts']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert inherited_value(d,'DW_AT_name')==row['name'] and contract(dw,d)==row['contract']
        assert native_ranges(dw,d)==[(int(p['address'],16),int(p['end'],16)) for p in row['native_ranges']]
    for row in record['physical_interface_inputs']:
        assert sha((root/row['path']).read_bytes())==row['sha256']
    definitions=read(root/'defs.h');subtypes=read(root/'subtypes.h')
    for key,pattern in record['header_constants'].items():
        assert re.search(pattern,definitions+'\n'+subtypes),key
    for row in record['native_enumerators']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert inherited_value(d,'DW_AT_name')==row['name']
        assert d.attributes['DW_AT_const_value'].value==row['value']
    for row in record['source_constants']:
        assert re.search(row['pattern'],read(root/row['source'])),row['name']
    for row in record['absolute_symbols']:
        assert any(n==row['name'] and v==int(row['address'],16) and s==-1 for n,v,s,k,c in image.symbols),row['name']
    for row in record['callee_bindings']:
        t=read(root/row['source']);a,b=span(t,row['name']);sig=t[a:t.index('{',a)].strip()
        assert sha(sig.encode())==row['signature_sha256'],(row['source'],row['name'])
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert inherited_value(d,'DW_AT_name')==row['native_name']
        assert int(row['native_address'],16) in [a for a,b in native_ranges(dw,d)]
    for source,decls in record['forward_declarations'].items():
        t=read(root/source)
        for decl in decls:
            assert t.count(decl)==1,(source,decl)
            name=re.search(r'\b(\w+)\(',decl).group(1)
            assert t.index(decl)<span(t,name)[0],name
    assert len(record['cards'])==100
    return dict(card_handlers=100,changed_definitions=sum(r['source_changed'] for r in record['cards']),
                retained_after_full_review=sum(not r['source_changed'] for r in record['cards']),
                source_dependency_bodies=len(record['source_dependencies']),inlined_support_helpers=len(record['support_helpers']),native_enum_contracts=len(record['native_enumerators']),native_data_ranges=len(record['native_data']),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
