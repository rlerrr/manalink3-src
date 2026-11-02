"""Verify typed locals replacing merged token/target/test storage."""
import json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from token_aggregate_source import source_changes,ghired_helper_changes,NAMES
from token_aggregate_analysis import inspect,validate_locals
from verify_array_role_recovery import replay

_current_cache={}


def record(proof,root):
    path=proof.get('native_token_aggregate_recovery')
    return json.loads((root/path).read_bytes()) if path else None


def has_source_repair(proof,root,source,name,old_sha):
    from verify_disentangled_pair_locals import has_source_repair as pair_repair
    if pair_repair(proof,root,source,name,old_sha):return True
    r=record(proof,root)
    if not r:return False
    matches=[v for v in r['rows']+[r['inline_helper_edit']] if (v['source'],v['name'])==(source,name)]
    if not matches:return False
    assert len(matches)==1;row=matches[0]
    changes,after=ghired_helper_changes(row['before_definition']) if name=='ghireds_special_populate' else source_changes(name,row['before_definition'])
    assert changes==row['changes'] and after==row['after_definition']==replay(row['before_definition'],changes)
    assert sha(row['before_definition'].encode())==row['before_sha256'] and sha(after.encode())==row['after_sha256']
    text=read(root/source);a,b=span(text,name);assert text[a:b]==after
    # Preserve each older exact receipt. Follow only whole-definition source
    # edits for this same source/name, validating both ends of every hash edge.
    todo=[old_sha];seen=set();found=False
    while todo:
        digest=todo.pop()
        if digest==row['before_sha256']:found=True;break
        if digest in seen:continue
        seen.add(digest)
        edges=list(proof['source_edits'])
        # The original Ghired correction is a fragment edit. Its earlier
        # native-backed receipt also retains both complete definitions.
        inline=json.loads((root/proof['native_inline_card_mechanics_recovery']).read_bytes())
        correction=inline['source_correction']
        assert correction['before_definition'].replace(correction['before'],correction['after'])==correction['after_definition']
        edges.append(dict(source=correction['source'],before=correction['before_definition'],after=correction['after_definition'],
            before_sha256=correction['before_sha256'],after_sha256=correction['after_sha256']))
        for e in edges:
            if e['source']!=source or e.get('before_sha256')!=digest:continue
            before,after=e.get('before'),e.get('after')
            if not isinstance(before,str) or not isinstance(after,str):continue
            try:
                assert span(before,name) and span(after,name)
            except (AssertionError,ValueError,TypeError):continue
            assert sha(before.encode())==digest and sha(after.encode())==e['after_sha256']
            todo.append(e['after_sha256'])
    if not found:return False
    edits=[e for e in proof['source_edits'] if e.get('native_token_aggregate_extension')==proof['native_token_aggregate_recovery'] and e.get('name')==name]
    assert len(edits)==1 and edits[0]['before_sha256']==row['before_sha256'] and edits[0]['after_sha256']==row['after_sha256']
    return True


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    from verify_disentangled_pair_locals import has_source_repair as pair_repair,verify_superseded_definition as pair_successor
    if pair_repair(proof,root,source,name,old_sha):return pair_successor(proof,image,root,source,name,old_sha)
    assert has_source_repair(proof,root,source,name,old_sha)
    r=record(proof,root);assert sha(image.data)==r['native_dll_sha256']
    owner='card_ghired_conclave_exile' if name=='ghireds_special_populate' else name
    row=next(v for v in r['rows'] if (v['source'],v['name'])==(source,owner));v=row['native_snapshot']['view']
    lo,hi=int(v['address'],16),int(v['end'],16)
    assert sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo))==v['bytes_sha256']


def current_rows(proof,root):
    r=record(proof,root);key=(r['runtime_after']['dll_sha256'],r['runtime_after']['dbg_sha256'],
        (root/'ManalinkEh.dll').stat().st_mtime_ns,(root/'ManalinkEh.dbg').stat().st_mtime_ns)
    if key not in _current_cache:
        im=Image((root/'ManalinkEh.dll').read_bytes());dbg=Image((root/'ManalinkEh.dbg').read_bytes())
        _current_cache[key]=inspect(im,dbg,r['rows'])
    return _current_cache[key]


def verify_current_row(proof,image,root,source,name,old_sha):
    from verify_disentangled_pair_locals import has_source_repair as pair_repair,verify_current_row as pair_current
    if pair_repair(proof,root,source,name,old_sha):return pair_current(proof,image,root,source,name,old_sha)
    verify_superseded_definition(proof,image,root,source,name,old_sha)
    r=record(proof,root);row=next(v for v in r['rows'] if (v['source'],v['name'])==(source,name))
    return validate_locals(row['native_snapshot'],current_rows(proof,root)[source,name])


def verify(proof,image,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    assert sha(image.data)==r['native_dll_sha256'];nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(nd.data)==r['native_dbg_sha256']
    rows=r['rows'];assert len(rows)==18 and {v['name'] for v in rows}==NAMES
    native=inspect(image,nd,rows,native=True);built=[]
    for label in ['before','after']:
        a=r['runtime_'+label];im=Image((root/a['dll_path']).read_bytes());dbg=Image((root/a['dbg_path']).read_bytes())
        assert sha(im.data)==a['dll_sha256'] and sha(dbg.data)==a['dbg_sha256'];built.append(inspect(im,dbg,rows))
    current=current_rows(proof,root)
    for row in rows:
        key=row['source'],row['name'];assert native[key]==row['native_snapshot']
        assert built[0][key]==row['before_compiled'] and built[1][key]==row['after_compiled']
        assert validate_locals(native[key],built[1][key])==row['after_validation']
        validate_locals(native[key],current[key]);verify_superseded_definition(proof,image,root,*key,row['before_sha256'])
    helper=r['inline_helper_edit'];verify_superseded_definition(proof,image,root,helper['source'],helper['name'],helper['before_sha256'])
    old_inline=json.loads((root/proof['native_inline_card_mechanics_recovery']).read_bytes())
    origin=next(v for v in old_inline['helpers'] if v['name']==helper['name'])
    assert origin['definition_sha256']==helper['before_sha256'] and origin['native_origins']==helper['native_origins']
    for edit in r['file_edits']:
        assert edit['before'] not in read(root/edit['source']) if edit['after']=='' else edit['after'] in read(root/edit['source'])
        assert any(e.get('native_token_aggregate_file_extension')==proof['native_token_aggregate_recovery'] and all(e[k]==edit[k] for k in ['source','before','after']) for e in proof['source_edits'])
    before,after=r['before_link_inputs'],r['after_link_inputs'];assert set(before)==set(after)
    changed={p for p in before if before[p]!=after[p]};expected={'.recovery/build-gcc93/obj/'+v['source'][:-2]+'.o' for v in rows};assert changed==expected
    return dict(restored_source_functions=19,card_handlers=17,helpers=2,source_units=17,
        restored_inline_helper_call='ghireds_special_populate',changed_linker_objects=len(changed),unchanged_linker_objects=len(before)-len(changed),scope=r['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
