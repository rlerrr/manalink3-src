"""Verify every fixed October 2022 veneer slot against native bytes and rebuilt COFF."""
import argparse,collections,json,re,struct
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value


def verify(proof,image=None,root=ROOT,binary_only=False):
    path=proof.get('native_jump_table_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text())
    native=None;nd=None
    if not binary_only:
        native=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes())
        assert sha(native.data)==record['dll_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
        nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
        assert sha(nd.data)==record['dbg_sha256']
    current=Image((root/'ManalinkEh.dll').read_bytes())
    rebuilt=Image((root/'ManalinkEh.dbg').read_bytes())
    def symbols(binary):
        out=collections.defaultdict(set)
        for name,offset,section,kind,storage in binary.symbols:
            if kind==32 and 0<section<=len(binary.pe.sections):
                out[name].add(binary.pe.OPTIONAL_HEADER.ImageBase+binary.pe.sections[section-1].VirtualAddress+offset)
        return out
    original_symbols=symbols(nd) if nd else {};current_symbols=symbols(rebuilt)
    assert current.pe.FILE_HEADER.Machine==0x14c
    assert current.pe.OPTIONAL_HEADER.ImageBase==0x2000000
    assert current.pe.OPTIONAL_HEADER.AddressOfEntryPoint==0x1000
    assert current.pe.get_data(0x1000,6)==bytes.fromhex('b801000000c3')
    if native:assert native.pe.get_data(0x1000,6)==current.pe.get_data(0x1000,6)
    current.pe.parse_data_directories(directories=[0])
    assert {s.name: s.address for s in current.pe.DIRECTORY_ENTRY_EXPORT.symbols}=={b'CodeSectionEx':0x1006,b'DllEntry':0x1000}
    source=read(root/'ManalinkEh.asm')
    entries=re.findall(r'(?m)^[ \t]*jmp near (\S+)[ \t]*;[ \t]*([0-9a-f]+)[ \t]*$',source)
    assert len(entries)==len(record['slots'])==record['native_slot_count']==15169
    assert record['old_slot_count']==11814
    aliases=0
    for i,row in enumerate(record['slots']):
        at=0x2001006+5*i
        assert row['slot']==hex(at)
        assert entries[i]==(row['binding'],format(at,'x'))
        if not binary_only:
            raw=native.pe.get_data(at-0x2000000,5);assert raw[0]==0xe9
            target=at+5+struct.unpack('<i',raw[1:])[0]
            assert row['target']==hex(target)
            assert len(row['coff'])==1 and original_symbols[row['coff'][0]]=={target}
            chain=row.get('native_tail_jump')
            if chain:
                aliases+=1
                data=native.pe.get_data(target-0x2000000,5)
                assert data.hex()==chain['bytes_hex'] and data[0]==0xe9
                target=target+5+struct.unpack('<i',data[1:])[0]
                assert chain['target']==hex(target) and original_symbols[row['binding']]=={target}
            else:assert row['binding']==row['coff'][0]
        raw=current.pe.get_data(at-0x2000000,5)
        assert len(raw)==5 and raw[0]==0xe9,('Missing fixed veneer',hex(at))
        destination=at+5+struct.unpack('<i',raw[1:])[0]
        assert current_symbols[row['binding']]=={destination},('Wrong veneer target',hex(at),row['binding'],hex(destination))
    if not binary_only:assert aliases==4
    assert int(record['end'],16)==0x2001006+15169*5
    hook=record['hook'];text=read(root/hook['source']);a,b=span(text,hook['name'])
    assert sha(text[a:b].encode())==hook['definition_sha256']
    receipts=[e for e in proof['source_edits'] if e.get('native_source_reconstruction',{}).get('function')==hook['name']]
    assert any(e['after_sha256']==hook['definition_sha256'] for e in receipts)
    if not binary_only:
        dw=nd.dwarf();f=dw.get_DIE_from_refaddr(int(hook['die_offset'],16))
        assert value(f,'DW_AT_name')==hook['name']
        assert value(f,'DW_AT_linkage_name')=='dlgproc_show_deck_hook@16'
        ranges=dw.range_lists().get_range_list_at_offset(f.attributes['DW_AT_ranges'].value,cu=f.cu)
        assert [(r.begin_offset,r.end_offset) for r in ranges if hasattr(r,'begin_offset')]==[(int(r['address'],16),int(r['end'],16)) for r in hook['ranges']]
        md=Cs(CS_ARCH_X86,CS_MODE_32)
        for row in hook['ranges']:
            lo=int(row['address'],16);hi=int(row['end'],16);data=native.pe.get_data(lo-0x2000000,hi-lo)
            assert sha(data)==row['bytes_sha256']
            assert sha((root/row['raw_export']).read_bytes())==row['raw_export_sha256']
            ins=list(md.disasm(data,lo));assert sum(i.size for i in ins)==hi-lo
            assert [[hex(i.address),i.mnemonic,i.op_str] for i in ins]==row['instructions']
    assert current_symbols['_dlgproc_show_deck_hook@16']
    if binary_only:
        return dict(verified_built_slots=15169,
                    scope='Compiled veneer positions, source entries, COFF targets and hook body only; original-byte provenance is checked by the canonical recovery audit.')
    return dict(verified_slots=15169,restored_missing_slots=3355,corrected_prefix_bindings=6,
                verified_native_tail_aliases=4,restored_split_range_windows_hook=1,
                scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--binary-only',action='store_true')
    print(json.dumps(verify(load(),binary_only=parser.parse_args().binary_only),indent=2))
