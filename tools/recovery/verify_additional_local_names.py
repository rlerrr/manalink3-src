"""Token-only native-vocabulary naming and explicit predecessor source links."""
import json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from verify_local_identifier_cleanup import project
from inspect_release import Image,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof
from verify_native_absolute_bindings import runtime_sections,stable_sections
from verify_inline_helpers import ranges

_records={}


def record(proof,root=ROOT):
    path=proof.get('native_additional_local_names')
    if not path:return None
    p=root/path;key=str(p),p.stat().st_mtime_ns
    if key not in _records:_records[key]=json.loads(p.read_bytes())
    return _records[key]


def has_source_repair(proof,root,source,name,old_sha):
    r=record(proof,root)
    if not r:return False
    rows=[v for v in r['rows'] if (v['source'],v['name'])==(source,name)]
    if not rows:return False
    assert len(rows)==1;row=rows[0];after,count=project(row['before_definition'],row['old_name'])
    assert after==row['after_definition'] and count==row['occurrences'] and sha(after.encode())==row['after_sha256']
    assert sha(row['before_definition'].encode())==row['before_sha256'];text=read(root/source);a,b=span(text,name);assert text[a:b]==after
    # This extension proves exactly an alpha rename. Earlier semantic repairs
    # must retain their own validators; no arbitrary predecessor graph is used.
    return old_sha==row['before_sha256']


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    assert has_source_repair(proof,root,source,name,old_sha),(source,name,old_sha)
    r=record(proof,root);assert sha(image.data)==r['native_dll_sha256'];row=next(v for v in r['rows'] if (v['source'],v['name'])==(source,name))
    for part in row['native_views']:
        a,b=int(part['address'],16),int(part['end'],16);assert sha(image.pe.get_data(a-image.pe.OPTIONAL_HEADER.ImageBase,b-a))==part['bytes_sha256']


def source_hash_matches(proof,root,source,name,current_sha,expected_sha):
    return current_sha==expected_sha or (proof is not None and has_source_repair(proof,root,source,name,expected_sha))


def current_matches(proof,image,root,source,name,expected):
    text=read(root/source);a,b=span(text,name)
    if text[a:b]==expected:return
    verify_superseded_definition(proof,image,root,source,name,sha(expected.encode()))


def verify(proof,image,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    assert sha(image.data)==r['native_dll_sha256'];debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==r['native_dbg_sha256'];dw=debug.dwarf();count=0;seen=set()
    for row in r['rows']:
        key=row['source'],row['name'];assert key not in seen;seen.add(key);verify_superseded_definition(proof,image,root,*key,row['before_sha256'])
        for v in row['native_candidates']:
            d=dw.get_DIE_from_refaddr(int(v['die'],16));assert inherited_value(d,'DW_AT_name')=='instance';t=unqualify(inherited_type(d));assert t.tag=='DW_TAG_pointer_type' and sizeof(t)==4 and sizeof(unqualify(inherited_type(t)))==300
        for part in row['native_views']:
            d=dw.get_DIE_from_refaddr(int(part['die'],16));assert inherited_value(d,'DW_AT_name')==row['name'];assert ranges(dw,d)==[(int(part['address'],16),int(part['end'],16))]
        edits=[e for e in proof['source_edits'] if e.get('additional_local_names_extension')==proof['native_additional_local_names'] and (e['source'],e.get('name'))==key];assert len(edits)==1 and edits[0]['before_sha256']==row['before_sha256'] and edits[0]['after_sha256']==row['after_sha256'];count+=row['occurrences']
    sections=[]
    for label in ['before','after']:
        a=r['runtime_'+label];im=Image((root/a['dll_path']).read_bytes());assert sha(im.data)==a['dll_sha256'] and sha((root/a['dbg_path']).read_bytes())==a['dbg_sha256'];s=runtime_sections(im);assert s==a['sections'];sections.append(stable_sections(s))
    assert sections[0]==sections[1];assert len(seen)==65 and count==724
    return dict(renamed_function_locals=65,identifier_occurrences=724,source_units=len({s for s,n in seen}),unchanged_runtime_sections=len(sections[0]),scope=r['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
