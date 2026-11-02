"""Check the last synthetic card-union reconstruction and event dispatch."""
import collections,json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from verify_additional_local_names import source_hash_matches
from inspect_release import Image,inherited_value
from global_initializers import inherited_type
from shared_stack_test_analysis import frame_offset,test_layout
from token_aggregate_analysis import inspect,locals_walk
from multiphase_record_source import project
from multiphase_card_flow import project as structure,traces
from multiphase_record_analysis import validate

_records={};_current={};_before={}


def record(proof,root):
    path=proof.get('native_multiphase_card_locals')
    if not path:return None
    p=root/path;key=str(p),p.stat().st_mtime_ns
    if key not in _records:_records[key]=json.loads(p.read_bytes())
    return _records[key]


def projected(row,r):
    typed,facts=project(row['before_definition'],row['native_snapshot'],r['layouts']);assert typed==row['typed_definition'] and facts==row['facts']
    after=structure(row['name'],typed);assert after==row['after_definition'] and sha(after.encode())==row['after_sha256'] and sha(row['before_definition'].encode())==row['before_sha256'];return after


def has_source_repair(proof,root,source,name,old_sha):
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
    edits=[v for v in proof['source_edits'] if v.get('multiphase_card_extension')==proof['native_multiphase_card_locals'] and (v['source'],v['name'])==(source,name)];assert len(edits)==1 and edits[0]['before_sha256']==row['before_sha256'] and edits[0]['after_sha256']==row['after_sha256'];return True


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    assert has_source_repair(proof,root,source,name,old_sha);r=record(proof,root);assert sha(image.data)==r['native_dll_sha256'];row=next(v for v in r['rows'] if (v['source'],v['name'])==(source,name));v=row['native_snapshot']['view'];a,b=int(v['address'],16),int(v['end'],16);assert sha(image.pe.get_data(a-image.pe.OPTIONAL_HEADER.ImageBase,b-a))==v['bytes_sha256']


def before_rows(proof,root):
    r=record(proof,root);a=r['runtime_before'];key=str(root),a['dll_sha256'],a['dbg_sha256']
    if key not in _before:_before[key]=inspect(Image((root/a['dll_path']).read_bytes()),Image((root/a['dbg_path']).read_bytes()),r['rows'])
    return _before[key]


def current_rows(proof,root):
    r=record(proof,root);key=str(root),(root/'ManalinkEh.dll').stat().st_mtime_ns,(root/'ManalinkEh.dbg').stat().st_mtime_ns
    if key not in _current:_current[key]=inspect(Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()),r['rows'])
    return _current[key]


def verify_current_row(proof,image,root,source,name,old_sha):
    verify_superseded_definition(proof,image,root,source,name,old_sha);r=record(proof,root);row=next(v for v in r['rows'] if (v['source'],v['name'])==(source,name));key=source,name
    return validate(row,current_rows(proof,root)[key],before_rows(proof,root)[key],image,Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()),Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()))


def plain_test_record(dw,die,source,name,root=ROOT):
    path=root/'docs/recovery-evidence/multiphase-card-locals.json'
    if not path.exists():return None
    r=json.loads(path.read_bytes());rows=[v for v in r['rows'] if (v['source'],v['name'])==(source,name)]
    if not rows:return None
    assert len(rows)==1;names={v['name'] for v in rows[0]['facts']['restored_locals'] if v['role']=='test'};variables=[v for v in locals_walk(die) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name') in names];assert len(variables)==len(names)>0
    offsets={frame_offset(dw,v) for v in variables};assert len(offsets)==1,'Historical whole-frame check needs separate per-record successor validation'
    layouts=[test_layout(inherited_type(v)) for v in variables];assert all(v==layouts[0] for v in layouts);return dict(die=die,frame_offset=offsets.pop(),layout=layouts[0])


def verify(proof,image,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    assert sha(image.data)==r['native_dll_sha256'];nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(nd.data)==r['native_dbg_sha256'];native=inspect(image,nd,r['rows'],native=True)
    builds=[]
    for label in ['before','after']:
        a=r['runtime_'+label];im=Image((root/a['dll_path']).read_bytes());db=Image((root/a['dbg_path']).read_bytes());assert sha(im.data)==a['dll_sha256'] and sha(db.data)==a['dbg_sha256'];builds.append((im,db,inspect(im,db,r['rows'])))
    assert traces()==r['event_dispatch_proof'];current=current_rows(proof,root);ci,cd=Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes());kinds=collections.Counter();locals_count=0
    for row in r['rows']:
        key=row['source'],row['name'];assert native[key]==row['native_snapshot'];assert builds[0][2][key]==row['before_snapshot'] and builds[1][2][key]==row['after_snapshot']
        assert validate(row,builds[1][2][key],builds[0][2][key],image,nd,builds[1][0],builds[1][1])==row['after_validation'];stats=validate(row,current[key],builds[0][2][key],image,nd,ci,cd);locals_count+=stats['restored_records']
        kinds.update(v['kind'] for v in stats['scalar_profile_comparisons']);verify_superseded_definition(proof,image,root,*key,row['before_sha256'])
    assert len(r['rows'])==28 and locals_count==83
    left,right=r['before_link_inputs'],r['after_link_inputs'];assert set(left)==set(right);expected={'.recovery/build-gcc93/obj/'+v['source'][:-2]+'.o' for v in r['rows']};assert len(expected)==20 and {p for p in left if left[p]!=right[p]}==expected
    return dict(restored_card_functions=28,source_units=20,removed_card_unions=28,restored_typed_locals=83,typed_field_rewrites=sum(len(v['facts']['field_rewrites']) for v in r['rows']),scalar_profile_comparison_kinds=dict(kinds),structured_event_handlers=2,removed_lab_gotos=4,changed_linker_objects=20,unchanged_linker_objects=len(left)-20,scope=r['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
