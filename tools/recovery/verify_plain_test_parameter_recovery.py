"""Replay seven omitted native test settings in six existing definitions."""
import json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from test_parameter_repair_analysis import inspect,events,card_argument_proof


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    path=proof.get('native_plain_test_parameter_recovery');assert path
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    rows=[r for r in record['rows'] if (r['source'],r['name'],r['before_sha256'])==(source,name,old_sha)]
    assert len(rows)==1;row=rows[0]
    assert sha(row['before_definition'].encode())==old_sha
    t=read(root/source);a,b=span(t,name)
    assert t[a:b]==row['after_definition'] and sha(t[a:b].encode())==row['after_sha256']
    assert row['after_definition']==row['before_definition'].replace(row['anchor'],row['anchor']+row['insertion'])
    assert row['before_definition'].count(row['anchor'])==1
    receipts=[e for e in proof['source_edits'] if e.get('native_plain_test_parameter_extension')==path and e['source']==source]
    assert len(receipts)==1 and receipts[0]['before_sha256']==old_sha and receipts[0]['after_sha256']==row['after_sha256']
    native=row['native']['view'];lo,hi=int(native['address'],16),int(native['end'],16)
    assert sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo))==native['bytes_sha256']


def verify(proof,image,root=ROOT):
    path=proof.get('native_plain_test_parameter_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    rows=record['rows'];assert len(rows)==6
    native=inspect(image,debug,rows)
    artifacts=[]
    for label in ['before','after']:
        artifact=record['runtime_'+label]
        im=Image((root/artifact['dll_path']).read_bytes());dbg=Image((root/artifact['dbg_path']).read_bytes())
        assert sha(im.data)==artifact['dll_sha256'] and sha(dbg.data)==artifact['dbg_sha256']
        artifacts.append((im,dbg,inspect(im,dbg,rows)))
    current=Image((root/'ManalinkEh.dll').read_bytes());current_debug=Image((root/'ManalinkEh.dbg').read_bytes())
    current_rows=inspect(current,current_debug,rows)
    restored=0
    for row in rows:
        key=(row['source'],row['name']);left=native[key];assert left==row['native']
        verify_superseded_definition(proof,image,root,*key,row['before_sha256'])
        before,after=[group[2][key] for group in artifacts]
        assert before==row['before_compiled'] and after==row['after_compiled']
        for candidate,im,dbg in [(after,*artifacts[1][:2]),(current_rows[key],current,current_debug)]:
            assert left['layout']==candidate['layout']
            assert events(left['view'],image,debug)==events(candidate['view'],im,dbg),row['name']
        missing=events(left['view'],image,debug)-events(before['view'],*artifacts[0][:2])
        expected={}
        for setting in row['settings']:
            for byte in range(setting['size']):
                expected[(setting['field'],byte,None if setting['value'] is None else setting['value']>>(8*byte)&255)]=1
        assert dict(missing)==expected,row['name']
        assert not events(before['view'],*artifacts[0][:2])-events(left['view'],image,debug)
        restored+=len(row['settings'])
        if row['name']=='card_saheeli_rai':
            assert card_argument_proof(left['view'],image,left['frame_offset'])==row['native_storage_proof']
            assert card_argument_proof(after['view'],artifacts[1][0],after['frame_offset'])==row['compiled_storage_proof']
            card_argument_proof(current_rows[key]['view'],current,current_rows[key]['frame_offset'])
    assert restored==7
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+r['source'][:-2]+'.o' for r in rows}
    assert {p for p in before if before[p]!=after[p]}==expected
    return dict(corrected_definitions=6,corrected_card_handlers=5,corrected_helpers=1,
        restored_native_test_settings=7,exact_parameter_store_comparisons=6,
        saheeli_storage_card_argument_proved=True,changed_linker_objects=6,
        unchanged_linker_objects=len(before)-6,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
