"""Check original COFF-only card identities without inventing DWARF extents."""
import json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from canonicalize_helpers import ROOT,read,span,sha
from inspect_release import Image,inherited_value
from verify_split_cards_recovery import contract


def verify(proof,image,root=ROOT):
    path=proof.get('native_coff_forwarders_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['dbg_sha256']
    dw=debug.dwarf();md=Cs(CS_ARCH_X86,CS_MODE_32)
    current=Image((root/'ManalinkEh.dll').read_bytes());rebuilt=Image((root/'ManalinkEh.dbg').read_bytes())
    def built_entries(name):
        return {rebuilt.pe.OPTIONAL_HEADER.ImageBase+rebuilt.pe.sections[s-1].VirtualAddress+v for n,v,s,k,c in rebuilt.symbols if n=='_'+name and s>0 and k==32}
    def entries(name):
        return {image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+v for n,v,s,k,c in image.symbols if n=='_'+name and s>0 and k==32}
    for row in record['cards']:
        at=int(row['native_address'],16);target=int(row['native_target'],16)
        assert entries(row['name'])=={at} and entries(row['callee'])=={target}
        raw=image.pe.get_data(at-0x2000000,5);assert raw.hex()==row['native_bytes'] and sha(raw)==row['native_bytes_sha256']
        ins=list(md.disasm(raw,at));assert len(ins)==1 and ins[0].mnemonic=='jmp' and ins[0].op_str==hex(target)
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16));assert inherited_value(d,'DW_AT_name')==row['name'] and contract(dw,d)==row['contract']
        assert not any(x in d.attributes for x in ['DW_AT_low_pc','DW_AT_high_pc','DW_AT_ranges'])
        t=read(root/row['source']);a,b=span(t,row['name']);body=t[a:b]
        assert sha(body.encode())==row['definition_sha256']
        assert re.fullmatch(r'int\s+'+re.escape(row['name'])+r'\(int player, int card, event_t event\)\s*\{\s*return '+re.escape(row['callee'])+r'\(player, card, event\);\s*\}',body)
        assert any(e.get('native_coff_source_reconstruction')==row for e in proof['source_edits'])
        actual=built_entries(row['name']);callee=built_entries(row['callee']);assert len(actual)==len(callee)==1
        start=next(iter(actual));branch=list(md.disasm(current.pe.get_data(start-0x2000000,5),start))
        assert len(branch)==1 and branch[0].mnemonic=='jmp' and branch[0].op_str==hex(next(iter(callee)))
    assert len(record['cards'])==5 and sum(r['registered'] for r in record['cards'])==4
    return dict(recovered_coff_card_entries=5,registered_identities=4,native_bytes=25,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
