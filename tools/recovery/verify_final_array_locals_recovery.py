"""Verify the final named-array queue using native declarations and scoped operands."""
import json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from final_array_locals_source import source_changes,NAMES,TYPEDEFS
from final_array_locals_analysis import inspect,validate
from verify_array_role_recovery import replay


def record(proof,root):
    path=proof.get('native_final_array_locals_recovery')
    return json.loads((root/path).read_bytes()) if path else None


def has_source_repair(proof,root,source,name,old_sha):
    from verify_final_helper_locals import has_source_repair as helper_repair
    if helper_repair(proof,root,source,name,old_sha):return True
    r=record(proof,root)
    if not r:return False
    rows=[v for v in r['rows'] if (v['source'],v['name'])==(source,name)]
    if not rows:return False
    assert len(rows)==1;row=rows[0];changes,after=source_changes(name,row['before_definition'])
    assert changes==row['changes'] and after==row['after_definition']==replay(row['before_definition'],changes)
    assert sha(row['before_definition'].encode())==row['before_sha256'] and sha(after.encode())==row['after_sha256']
    text=read(root/source);a,b=span(text,name);assert text[a:b]==after
    todo=[old_sha];seen=set();found=False
    while todo:
        digest=todo.pop()
        if digest==row['before_sha256']:found=True;break
        if digest in seen:continue
        seen.add(digest)
        for e in proof['source_edits']:
            if e['source']!=source or e.get('before_sha256')!=digest:continue
            before,next_text=e.get('before'),e.get('after')
            if not isinstance(before,str) or not isinstance(next_text,str):continue
            try:assert span(before,name) and span(next_text,name)
            except (AssertionError,TypeError,ValueError):continue
            assert sha(before.encode())==digest and sha(next_text.encode())==e['after_sha256']
            todo.append(e['after_sha256'])
    if not found:return False
    edits=[e for e in proof['source_edits'] if e.get('native_final_array_locals_extension')==proof['native_final_array_locals_recovery'] and e.get('name')==name]
    assert len(edits)==1 and edits[0]['before_sha256']==row['before_sha256'] and edits[0]['after_sha256']==row['after_sha256']
    return True


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    from verify_final_helper_locals import has_source_repair as helper_repair,verify_superseded_definition as helper_successor
    if helper_repair(proof,root,source,name,old_sha):return helper_successor(proof,image,root,source,name,old_sha)
    assert has_source_repair(proof,root,source,name,old_sha)
    r=record(proof,root);assert sha(image.data)==r['native_dll_sha256']
    row=next(v for v in r['rows'] if (v['source'],v['name'])==(source,name));v=row['native_snapshot']['view']
    lo,hi=int(v['address'],16),int(v['end'],16)
    assert sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo))==v['bytes_sha256']


def verify_current_row(proof,image,root,source,name,old_sha):
    from verify_final_helper_locals import has_source_repair as helper_repair,verify_current_row as helper_successor
    if helper_repair(proof,root,source,name,old_sha):return helper_successor(proof,image,root,source,name,old_sha)
    verify_superseded_definition(proof,image,root,source,name,old_sha)
    r=record(proof,root);row=next(v for v in r['rows'] if (v['source'],v['name'])==(source,name))
    im=Image((root/'ManalinkEh.dll').read_bytes());dbg=Image((root/'ManalinkEh.dbg').read_bytes())
    current=inspect(im,dbg,[row])[source,name]
    return validate(row['native_snapshot'],current,name)


def verify(proof,image,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    assert sha(image.data)==r['native_dll_sha256'];nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(nd.data)==r['native_dbg_sha256']
    rows=r['rows'];assert len(rows)==7 and {v['name'] for v in rows}==NAMES
    native=inspect(image,nd,rows);builds=[]
    for label in ['before','after']:
        a=r['runtime_'+label];im=Image((root/a['dll_path']).read_bytes());dbg=Image((root/a['dbg_path']).read_bytes())
        assert sha(im.data)==a['dll_sha256'] and sha(dbg.data)==a['dbg_sha256'];builds.append(inspect(im,dbg,rows))
    current=inspect(Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()),rows)
    arrays=tests=0
    for row in rows:
        key=row['source'],row['name'];assert native[key]==row['native_snapshot']
        assert builds[0][key]==row['before_snapshot'] and builds[1][key]==row['after_snapshot']
        assert validate(native[key],builds[1][key],row['name'])==row['after_validation']
        stats=validate(native[key],current[key],row['name']);arrays+=stats['restored_native_array_instances'];tests+=stats['native_test_instances']
        verify_superseded_definition(proof,image,root,*key,row['before_sha256'])
    assert arrays==19 and tests==7
    assert [(e['source'],e['before']) for e in r['file_edits']]==TYPEDEFS
    for e in r['file_edits']:
        assert not e['after'] and e['before'] not in read(root/e['source'])
        assert any(v.get('native_final_array_locals_file_extension')==proof['native_final_array_locals_recovery'] and all(v[k]==e[k] for k in ['source','before','after','before_sha256','after_sha256']) for v in proof['source_edits'])
    before,after=r['before_link_inputs'],r['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+v['source'][:-2]+'.o' for v in rows};assert len(expected)==5
    assert {p for p in before if before[p]!=after[p]}==expected
    return dict(recovered_helper_definitions=7,source_units=5,reviewed_native_fixed_arrays=19,reviewed_native_filter_scopes=7,
        restored_array_name_flags=15,removed_shared_storage_typedefs=6,sacrifice_parameter_stores=9,sacrifice_parameter_bits=208,
        changed_linker_objects=5,unchanged_linker_objects=len(before)-5,scope=r['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
