"""Verify counter recovery receipts, extracted preferences and native sounds."""
import csv,json,re,struct
from canonicalize_helpers import ROOT,read,sha,mask,span
from inspect_release import Image,value


def verify(proof,image,root=ROOT):
    path=proof.get('native_counter_foundation_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256'])
              for e in proof['source_edits']}
    for row in record['helpers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
    preferences=record['card_preferences'];base=image.pe.OPTIONAL_HEADER.ImageBase
    lo,hi=int(preferences['native_address'],16),int(preferences['native_end'],16)
    assert sha(image.pe.get_data(lo-base,hi-lo))==preferences['native_bytes_sha256']
    lookup=preferences['native_lookup_range'];l,h=int(lookup['address'],16),int(lookup['end'],16)
    assert sha(image.pe.get_data(l-base,h-l))==lookup['bytes_sha256']
    for receipt in [preferences['emulation_output'],preferences['extractor']]:
        assert sha((root/receipt['path']).read_bytes())==receipt['sha256'],receipt['path']
    lines=(root/preferences['emulation_output']['path']).read_text().splitlines()
    assert lines[0]==f"# native={hex(lo)} end={hex(hi)} sha256="+preferences['native_bytes_sha256']
    assert lines[1]==f"# lookup={hex(l)} end={hex(h)} sha256="+lookup['bytes_sha256']
    extracted={int(r['csvid']):int(r['counter_type']) for r in csv.DictReader(lines[2:],delimiter='\t')}
    assert len(extracted)==65542 and all(i in extracted for i in range(65536))
    expected={int(k):v for k,v in preferences['entries'].items()}
    assert {k:v for k,v in extracted.items() if v!=-1}==expected
    # Confirm the extracted function has no external calls and every computed
    # switch branch reads the original checked lookup interval.
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_MEM
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;tables=[]
    for i in decoder.disasm(image.pe.get_data(lo-base,hi-lo),lo):
        assert i.mnemonic!='call'
        if i.mnemonic=='jmp' and i.operands[0].type==X86_OP_MEM:
            assert l<=i.operands[0].mem.disp<h and not i.operands[0].mem.base
            tables.append(i.operands[0].mem.disp)
    assert len(tables)==18
    header=read(root/'defs.h');ids={n:int(v) for n,v in re.findall(r'\b(CARD_ID_\w+)\s*=\s*(\d+)',read(root/'card_id.h'))}
    counters=json.loads((root/proof['native_events_damage_recovery']).read_text())['counter_enum']['values']
    text=read(root/'functions/counters.c');a,b=span(text,'get_counter_type_by_id');body=mask(text[a:b]);actual={};pending=[]
    for m in re.finditer(r'\bcase\s+([A-Za-z_]\w*|0x[0-9a-f]+|\d+)\s*:|\breturn\s+([^;]+);',body):
        if m[1]:pending.append(ids[m[1]] if m[1] in ids else int(m[1],0))
        elif pending:
            typ=counters[m[2].strip()]
            for csvid in pending:
                assert csvid not in actual;actual[csvid]=typ
            pending=[]
        else:assert m[2].strip()=='-1'
    assert not pending and actual==expected and len(actual)==721
    debug=Image((root/'ManalinkEh.dbg').read_bytes());rebuilt=Image((root/'ManalinkEh.dll').read_bytes())
    sound=record['sound_table'];start=int(sound['address'],16);raw=image.pe.get_data(start-base,89*4)
    assert sha(raw)==sound['bytes_sha256']
    symbol=[s for s in debug.symbols if s[0]=='_wav_filenames'];assert len(symbol)==1 and symbol[0][2]>0
    _,offset,section,_,_=symbol[0];rva=debug.pe.sections[section-1].VirtualAddress+offset
    rebuilt_pointers=rebuilt.pe.get_data(rva,89*4)
    for n,row in enumerate(sound['rows']):
        address=struct.unpack_from('<I',raw,n*4)[0];assert hex(address)==row['address'] and row['index']==n
        expected_name=row['filename'].encode()+b'\0'
        assert image.pe.get_data(address-base,len(expected_name))==expected_name
        address=struct.unpack_from('<I',rebuilt_pointers,n*4)[0]
        assert rebuilt.pe.get_data(address-rebuilt.pe.OPTIONAL_HEADER.ImageBase,len(expected_name))==expected_name
    original_debug=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original_debug.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    wave=record['sound_enum'];die=original_debug.dwarf().get_DIE_from_refaddr(int(wave['die_offset'],16))
    values={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'}
    assert value(die,'DW_AT_byte_size')==wave['byte_size']==4
    for name,num in {**wave['added_members'],'WAV_HIGHEST':wave['highest']}.items():
        assert values[name]==num and re.search(r'\b'+name+r'\s*=\s*'+str(num)+r'\b',mask(header))
    # Verify the physical fifth vararg that Ghidra's raw formatter call omits.
    ins=list(decoder.disasm(image.pe.get_data(0x6cebf0,102),0x26cebf0))
    assert any(i.mnemonic=='mov' and i.op_str=='dword ptr [esp + 0x10], ecx' for i in ins)
    return dict(helper_definitions=record['helper_definitions'],native_ranges=record['native_helper_ranges'],
                changed_helpers=record['changed_helper_definitions'],native_card_preferences=len(actual),
                emulated_inputs=len(extracted),checked_switch_tables=len(tables),native_sound_filenames=89,
                scope='Full-body replay receipts, complete unsigned-16 preference domain plus six sentinel inputs, original lookup bytes, and all original/rebuilt sound filename bytes. Gameplay validation remains downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
