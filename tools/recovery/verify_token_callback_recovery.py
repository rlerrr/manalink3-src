"""Verify Mirror Match uses the shared native end-of-combat token callback."""
import json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from token_callback_analysis import inspect,source_changes,SOURCE,NAME
from verify_array_role_recovery import replay


def verify(proof,image,root=ROOT):
    path=proof.get('native_token_callback_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(nd.data)==record['native_dbg_sha256']
    native=inspect(image,nd);assert native==record['native_snapshot']
    assert native['callback_store']['value']==0x2073940 and native['callback_symbol']=='_exile_at_end_of_combat'
    aux=next(e for e in proof['auxiliary_functions'] if e.get('native_address')=='2073940')
    assert aux['source']=='cards/innistrad.c' and aux['canonical_name']=='innistrad_exile_at_end_of_combat_legacy'
    builds=[]
    for label in ['before','after']:
        a=record['runtime_'+label];im=Image((root/a['dll_path']).read_bytes());dbg=Image((root/a['dbg_path']).read_bytes())
        assert sha(im.data)==a['dll_sha256'] and sha(dbg.data)==a['dbg_sha256'];s=inspect(im,dbg);assert s==record[label+'_compiled'];builds.append(s)
    assert builds[0]['callback_symbol']=='_exile_at_end_of_combat'
    current=inspect(Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()))
    for snapshot in [builds[1],current]:
        assert snapshot['callback_symbol']=='_'+aux['canonical_name']
        assert snapshot['callback_signature']==native['callback_signature']
        assert snapshot['noncallback_byte_events']==native['noncallback_byte_events']==builds[0]['noncallback_byte_events']
    row=record['repair'];assert (row['source'],row['name'])==(SOURCE,NAME)
    changes,after=source_changes(row['before_definition']);assert changes==row['changes'] and after==row['after_definition']==replay(row['before_definition'],changes)
    assert sha(row['before_definition'].encode())==row['before_sha256']
    text=read(root/SOURCE);a,b=span(text,NAME);assert text[a:b]==after and sha(after.encode())==row['after_sha256']
    edits=[e for e in proof['source_edits'] if e.get('native_token_callback_extension')==path];assert len(edits)==1 and edits[0]['after_sha256']==row['after_sha256']
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    assert {p for p in before if before[p]!=after[p]}=={'.recovery/build-gcc93/obj/cards/commander_2015.o'}
    return dict(corrected_card_handlers=1,native_callback_address='0x2073940',callback_field_offset=60,callback_pointer_bytes=4,
        callback_name=aux['canonical_name'],noncallback_byte_events_preserved=True,changed_linker_objects=1,unchanged_linker_objects=len(before)-1,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
