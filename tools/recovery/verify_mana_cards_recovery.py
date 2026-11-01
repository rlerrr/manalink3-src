"""Check complete changed-card bodies, native ABI and immutable source receipts."""
import json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value
from verify_split_cards_recovery import native_ranges,contract


def verify(proof,image,root=ROOT):
    path=proof.get('native_mana_cards_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['dbg_sha256']
    dw=debug.dwarf();md=Cs(CS_ARCH_X86,CS_MODE_32)
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['cards']+record['support_helpers']:
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
    for row in record['retired_predecessors']:
        assert not re.search(r'\b'+row['name']+r'\s*\(',read(root/row['source']))
    specialization=record['soldier_specialization']
    d=dw.get_DIE_from_refaddr(int(specialization['die_offset'],16))
    assert contract(dw,d)==specialization['contract']
    fixed=next(p for p in specialization['contract']['parameters'] if p['name']=='sleight')
    assert fixed['constant']==1 and fixed['locations']==[]
    public=next(r for r in record['support_helpers'] if r['name']=='attacking_planeswalker')
    d=dw.get_DIE_from_refaddr(int(public['die_offset'],16))
    from inspect_release import value
    assert value(d,'DW_AT_external') is True
    assert [s[4] for s in debug.symbols if s[0]=='_attacking_planeswalker' and s[3]==32]==[2]
    rebuilt=Image((root/'ManalinkEh.dbg').read_bytes())
    assert [s[4] for s in rebuilt.symbols if s[0]=='_attacking_planeswalker' and s[3]==32]==[2]
    assert 'int attacking_planeswalker(int player, int card, event_t event);' in read(root/'manalink.h')
    assert len(record['cards'])==80
    return dict(card_handlers=80,changed_definitions=sum(r['source_changed'] for r in record['cards']),
                retained_after_full_review=sum(not r['source_changed'] for r in record['cards']),
                reviewed_support_helpers=len(record['support_helpers']),dependency_bodies=len(record['source_dependencies']),retired_predecessors=len(record['retired_predecessors']),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
