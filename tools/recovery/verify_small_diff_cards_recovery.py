"""Bind reviewed small-difference card bodies to source, DWARF, native bytes and exports."""
import json
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value
from verify_split_cards_recovery import native_ranges,contract


def verify(proof,image,root=ROOT):
    path=proof.get('native_small_diff_cards_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['dbg_sha256']
    dw=debug.dwarf();md=Cs(CS_ARCH_X86,CS_MODE_32)
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['cards']+record['support_helpers']:
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert inherited_value(d,'DW_AT_name')==row['name'] and contract(dw,d)==row['contract']
        assert native_ranges(dw,d)==[(int(p['address'],16),int(p['end'],16)) for p in row['native_ranges']]
        source_function=row.get('source_function',row['name'])
        t=read(root/row['source']);a,b=span(t,source_function);assert sha(t[a:b].encode())==row['definition_sha256'],source_function
        if row in record['cards']:
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
    specialization=record['landcycling_specialization']
    d=dw.get_DIE_from_refaddr(int(specialization['die_offset'],16))
    assert contract(dw,d)==specialization['contract']
    fixed=next(p for p in specialization['contract']['parameters'] if p['name']=='colorless')
    assert fixed['constant']==1 and fixed['locations']==[]
    active=[p for p in specialization['contract']['parameters'] if p['locations']]
    assert [p['name'] for p in active]==['player','card','event','black','blue','green','red','white']
    assert specialization['physical_arity']==8 and specialization['source_arity']==9
    assert len(record['cards'])==60
    return dict(card_handlers=60,changed_definitions=sum(r['source_changed'] for r in record['cards']),
                retained_after_full_review=sum(not r['source_changed'] for r in record['cards']),
                reviewed_support_helpers=len(record['support_helpers']),source_dependency_bodies=len(record['source_dependencies']),native_data_ranges=len(record['native_data']),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
