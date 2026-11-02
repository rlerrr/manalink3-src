"""Exact typed-local source projection and native/rebuilt record evidence."""
import json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from verify_additional_local_names import source_hash_matches
from inspect_release import Image,value,inherited_value
from global_initializers import inherited_type
from shared_stack_test_analysis import frame_offset,test_layout
from token_aggregate_analysis import inspect,locals_walk
from disentangle_union_source import project
from structured_flow_source import guild_summit,trace_proof
from disentangled_pair_analysis import validate

_cache={};_records={}


def record(proof,root):
    path=proof.get('native_disentangled_pair_locals')
    if not path:return None
    p=root/path;key=str(p),p.stat().st_mtime_ns
    if key not in _records:_records[key]=json.loads(p.read_bytes())
    return _records[key]


def projected(row,r):
    after,facts=project(row['before_definition'],row['native_snapshot'],r['layouts']);assert facts==row['facts']
    if row['name']=='card_guild_summit':
        assert after==row['unstructured_typed_definition'] and trace_proof()==row['control_flow_proof'];after=guild_summit(after)
    assert after==row['after_definition'] and sha(after.encode())==row['after_sha256'] and sha(row['before_definition'].encode())==row['before_sha256']
    return after


def has_source_repair(proof,root,source,name,old_sha):
    from verify_multiphase_card_locals import has_source_repair as multi_repair
    if multi_repair(proof,root,source,name,old_sha):return True
    r=record(proof,root)
    if not r:return False
    rows=[v for v in r['rows'] if (v['source'],v['name'])==(source,name)]
    if not rows:return False
    assert len(rows)==1;row=rows[0];after=projected(row,r);text=read(root/source);a,b=span(text,name);assert source_hash_matches(proof,root,source,name,sha(text[a:b].encode()),sha(after.encode()))
    todo=[old_sha];seen=set()
    while todo:
        digest=todo.pop()
        if digest==row['before_sha256']:break
        if digest in seen:continue
        seen.add(digest)
        for e in proof['source_edits']:
            if e['source']!=source or e.get('before_sha256')!=digest:continue
            before,next_text=e.get('before'),e.get('after')
            if not isinstance(before,str) or not isinstance(next_text,str) or not span(before,name) or not span(next_text,name):continue
            assert sha(before.encode())==digest and sha(next_text.encode())==e['after_sha256'];todo.append(e['after_sha256'])
    else:return False
    edits=[v for v in proof['source_edits'] if v.get('disentangled_pair_extension')==proof['native_disentangled_pair_locals'] and (v['source'],v['name'])==(source,name)]
    assert len(edits)==1 and edits[0]['before_sha256']==row['before_sha256'] and edits[0]['after_sha256']==row['after_sha256'];return True


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    from verify_multiphase_card_locals import has_source_repair as multi_repair,verify_superseded_definition as multi_successor
    if multi_repair(proof,root,source,name,old_sha):return multi_successor(proof,image,root,source,name,old_sha)
    assert has_source_repair(proof,root,source,name,old_sha);r=record(proof,root);assert sha(image.data)==r['native_dll_sha256']
    row=next(v for v in r['rows'] if (v['source'],v['name'])==(source,name));v=row['native_snapshot']['view'];a,b=int(v['address'],16),int(v['end'],16);assert sha(image.pe.get_data(a-image.pe.OPTIONAL_HEADER.ImageBase,b-a))==v['bytes_sha256']


def current_rows(proof,root):
    r=record(proof,root);key=(str(root),(root/'ManalinkEh.dll').stat().st_mtime_ns,(root/'ManalinkEh.dbg').stat().st_mtime_ns)
    if key not in _cache:_cache[key]=inspect(Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()),r['rows'])
    return _cache[key]


def verify_current_row(proof,image,root,source,name,old_sha):
    from verify_multiphase_card_locals import has_source_repair as multi_repair,verify_current_row as multi_successor
    if multi_repair(proof,root,source,name,old_sha):return multi_successor(proof,image,root,source,name,old_sha)
    verify_superseded_definition(proof,image,root,source,name,old_sha);r=record(proof,root);row=next(v for v in r['rows'] if (v['source'],v['name'])==(source,name))
    return validate(row['native_snapshot'],current_rows(proof,root)[source,name],Image((root/'ManalinkEh.dll').read_bytes()),{f['native_variable_die']:f['name'] for f in row['facts']['restored_locals']})


def plain_test_record(dw,die,source,name,root=ROOT):
    from verify_multiphase_card_locals import plain_test_record as multi_test
    result=multi_test(dw,die,source,name,root)
    if result is not None:return result
    # Called only after the old union variable is absent. Historical binaries
    # retain their original union path and exact stored snapshots.
    r=json.loads((root/'docs/recovery-evidence/disentangled-pair-locals.json').read_bytes());rows=[v for v in r['rows'] if (v['source'],v['name'])==(source,name)];assert len(rows)==1
    facts=[v for v in rows[0]['facts']['restored_locals'] if v['role']=='test'];assert len(facts)==1
    variables=[v for v in locals_walk(die) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')==facts[0]['name']];assert len(variables)==1
    v=variables[0];return dict(die=die,frame_offset=frame_offset(dw,v),layout=test_layout(inherited_type(v)))


def verify(proof,image,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    assert sha(image.data)==r['native_dll_sha256'];debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==r['native_dbg_sha256']
    native=inspect(image,debug,r['rows'],native=True);builds=[]
    for label in ['before','after']:
        a=r['runtime_'+label];im=Image((root/a['dll_path']).read_bytes());db=Image((root/a['dbg_path']).read_bytes());assert sha(im.data)==a['dll_sha256'] and sha(db.data)==a['dbg_sha256'];builds.append((im,inspect(im,db,r['rows'])))
    current=current_rows(proof,root);ci=Image((root/'ManalinkEh.dll').read_bytes());kinds={}
    for row in r['rows']:
        key=row['source'],row['name'];assert native[key]==row['native_snapshot'];assert builds[0][1][key]==row['before_snapshot'] and builds[1][1][key]==row['after_snapshot']
        bindings={f['native_variable_die']:f['name'] for f in row['facts']['restored_locals']}
        assert validate(native[key],builds[1][1][key],builds[1][0],bindings)==row['after_validation'];stats=validate(native[key],current[key],ci,bindings)
        for v in stats['scalar_comparisons']:kinds[v['kind']]=kinds.get(v['kind'],0)+1
        verify_superseded_definition(proof,image,root,*key,row['before_sha256'])
    assert len(r['rows'])==66
    left,right=r['before_link_inputs'],r['after_link_inputs'];assert set(left)==set(right);expected={'.recovery/build-gcc93/obj/'+v['source'][:-2]+'.o' for v in r['rows']};assert len(expected)==36 and {p for p in left if left[p]!=right[p]}==expected
    return dict(restored_functions=66,source_units=36,removed_synthetic_unions=66,typed_field_rewrites=sum(len(v['facts']['field_rewrites']) for v in r['rows']),scalar_comparison_kinds=kinds,
                removed_lab_gotos=7,structured_card_handlers=1,changed_linker_objects=36,unchanged_linker_objects=len(left)-36,scope=r['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
