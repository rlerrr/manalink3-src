"""Bind reviewed control, cycling and mana card bodies to source, DWARF, native bytes and exports."""
import json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value
from verify_split_cards_recovery import native_ranges,contract


def verify(proof,image,root=ROOT):
    path=proof.get('native_control_cycling_cards_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
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
    assert re.search(r'_player_bits\s*=\s*0x4ef1a4;',read(root/'manalink.lds'))
    definitions=read(root/'defs.h')
    assert re.search(r'PB_GY_ABILITIES_DISABLED\s*=\s*1\s*<<\s*25\s*,',definitions)
    assert re.search(r'COUNTER_P1_P1\s*=\s*0\s*,',definitions)
    assert re.search(r'F1_NO_CREATURE\s*=\s*2\s*,',definitions)
    assert re.search(r'GAA_MANA_ABILITY\s*=\s*1\s*<<\s*21\s*,',definitions)
    assert len(record['cards'])==100
    return dict(card_handlers=100,changed_definitions=sum(r['source_changed'] for r in record['cards']),
                retained_after_full_review=sum(not r['source_changed'] for r in record['cards']),
                source_dependency_bodies=len(record['source_dependencies']),native_data_ranges=len(record['native_data']),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
