"""Verify native scoped tests recovered from undersized decompiler casts."""
import collections,json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from cast_stack_storage_analysis import frames,info_slot_flow


def replay(text,changes):
    for change in changes:
        assert text.count(change['before'])==change['occurrences']
        text=text.replace(change['before'],change['after'],change.get('replace_count',change['occurrences']))
    return text


def events(view):
    return collections.Counter({(r['field'],r['byte'],r['value']):r['count'] for r in view['byte_events']})


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    path=proof.get('native_cast_stack_storage_recovery');assert path
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    rows=[r for r in record['rows'] if (r['source'],r['name'],r['before_sha256'])==(source,name,old_sha)]
    assert len(rows)==1;row=rows[0]
    assert sha(row['before_definition'].encode())==old_sha
    assert replay(row['before_definition'],row['changes'])==row['after_definition']
    text=read(root/source);a,b=span(text,name)
    assert text[a:b]==row['after_definition'] and sha(text[a:b].encode())==row['after_sha256']
    edits=[e for e in proof['source_edits'] if e.get('native_cast_stack_storage_extension')==path and e['source']==source]
    assert len(edits)==1 and edits[0]['before_sha256']==old_sha and edits[0]['after_sha256']==row['after_sha256']
    for test in row['native_tests']:
        view=test['view'];lo,hi=int(view['address'],16),int(view['end'],16)
        assert sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo))==view['bytes_sha256']


def verify(proof,image,root=ROOT):
    path=proof.get('native_cast_stack_storage_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    rows=record['rows'];assert len(rows)==3
    native=frames(image,debug,rows)
    artifacts=[]
    for label in ['before','after']:
        artifact=record['runtime_'+label]
        im=Image((root/artifact['dll_path']).read_bytes());dbg=Image((root/artifact['dbg_path']).read_bytes())
        assert sha(im.data)==artifact['dll_sha256'] and sha(dbg.data)==artifact['dbg_sha256']
        artifacts.append((im,dbg,frames(im,dbg,rows,before=label=='before')))
    current=Image((root/'ManalinkEh.dll').read_bytes());current_debug=Image((root/'ManalinkEh.dbg').read_bytes())
    current_rows=frames(current,current_debug,rows)
    tests=0;settings=0
    for row in rows:
        key=(row['source'],row['name']);left=native[key];assert left==row['native_scoped']
        verify_superseded_definition(proof,image,root,*key,row['before_sha256'])
        assert '(test_definition_t *)' not in row['after_definition']
        before=artifacts[0][2][key];after=artifacts[1][2][key]
        assert before==row['before_compiled'] and after==row['after_compiled']
        element_size=4 if row['source_array']=='paid' else 1
        assert before['array']['shape']==dict(size=row['source_array_size'],kind='DW_TAG_array_type',element_size=element_size,count=row['source_array_size']//element_size)
        assert not before['tests'] and len(left['tests'])==len(row['test_names'])
        # Original DWARF records prove separate source scopes reusing one
        # allocation. No union or guessed larger character array is required.
        lifetimes=[left['array']['scope_ranges']]+[[[int(p['address'],16),int(p['end'],16)] for p in t['view']['scope_ranges']] for t in left['tests']]
        assert all(left['array']['frame_offset']==t['frame_offset']==-236 and t['size']==216 for t in left['tests'])
        for a in range(len(lifetimes)):
            for b in range(a):assert not any(max(x,u)<min(y,v) for x,y in lifetimes[a] for u,v in lifetimes[b])
        for view,im,dbg in [(after,*artifacts[1][:2]),(current_rows[key],current,current_debug)]:
            assert view['array']['shape']==left['array']['shape']
            assert [t['name'] for t in view['tests']]==row['test_names']
            for l,r in zip(left['tests'],view['tests']):
                assert r['size']==216 and l['layout']==r['layout']
                assert r['frame_offset']+216<=-8
                assert events(l['view'])==events(r['view']),(row['name'],r['name'])
            if row['name']=='card_engineered_explosives':
                r=view['tests'][0]
                info_slot_flow(r['view'],im,dbg,r['frame_offset'])
        if row['name']=='card_engineered_explosives':
            l=left['tests'][0];r=after['tests'][0]
            assert info_slot_flow(l['view'],image,debug,l['frame_offset'])==row['native_cmc_flow']
            assert info_slot_flow(r['view'],artifacts[1][0],artifacts[1][1],r['frame_offset'])==row['compiled_cmc_flow']
        tests+=len(left['tests']);settings+=sum(len(t['view']['stores']) for t in left['tests'])
    assert tests==4 and settings==4
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+r['source'][:-2]+'.o' for r in rows}
    assert {p for p in before if before[p]!=after[p]}==expected
    return dict(repaired_source_functions=3,restored_scoped_test_locals=4,restored_native_parameter_stores=4,
        removed_undersized_api_buffers=[28,100,100],original_prompt_payment_arrays_retained=True,
        native_disjoint_scope_frame_reuse_verified=True,engineered_explosives_cmc_flow_verified=True,
        changed_linker_objects=3,unchanged_linker_objects=len(before)-3,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
