"""Verify direct recovery receipts and the physical native subtype-token table."""
import json,re,struct
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image


def verify(proof,image,root=ROOT):
    path=proof.get('native_damage_token_tutor_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256'])
              for e in proof['source_edits']}
    for row in record['helpers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
    for row in record['inline_helpers']:
        assert any(e['source']==row['source'] and e['after_sha256']==row['definition_sha256']
                   and (e.get('native_inline_worker_reconstruction',{}).get('source_function')==row['name']
                        or e.get('native_inline_worker_route_review',{}).get('source_function')==row['name'])
                   for e in proof['source_edits']),row['name']
    table=record['native_token_table'];at=int(table['address'],16)
    data=image.pe.get_data(at-0x2000000,table['element_count']*2)
    assert len(data)==442 and sha(data)==table['bytes_sha256']
    values=list(struct.unpack('<221h',data));assert values==table['values']
    text=read(root/'functions/token_generation.c');a,b=span(text,'from_subtype_to_token_id')
    initializer=re.search(r'token_ids\[221\]\s*=\s*\{(.*?)\}',text[a:b],re.S).group(1)
    assert list(map(int,re.findall(r'\b\d+\b',initializer)))==values
    rebuilt=Image((root/'ManalinkEh.dll').read_bytes())
    assert rebuilt.data.count(data)==1,'Native token table must occur once in the rebuilt DLL'
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    decoder=Cs(CS_ARCH_X86,CS_MODE_32)
    instructions=list(decoder.disasm(image.pe.get_data(0x666af0,201),0x2666af0))
    assert any(i.mnemonic=='cmp' and i.op_str=='eax, 0xdc' for i in instructions)
    assert any(i.mnemonic=='movsx' and i.op_str=='edi, word ptr [eax + eax + 0x275e480]' for i in instructions)
    assert any(i.mnemonic=='mov' and i.op_str=='dword ptr [ebp - 0x1c], 0x2664e30' for i in instructions)
    assert len(record['helpers'])==record['helper_definitions']==record['new_native_helper_ranges']==46
    return dict(helper_definitions=46,changed_helpers=record['changed_helper_definitions'],
                inline_lookup_recovered=True,aura_inline_route_reviewed='subtype == 0',
                native_and_rebuilt_token_table_verified=True,token_table_elements=len(values),
                scope='Direct-body replay receipts and physical native/rebuilt table equality. Gameplay validation remains downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
