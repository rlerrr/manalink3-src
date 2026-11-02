"""Capture-free scalar names supported by source roles and native local vocabulary."""
import collections,json,re
from contextlib import contextmanager
from canonicalize_helpers import ROOT,read,span,mask,sha
from role_local_source import candidates,project
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from source_reader_view import source_reader_view,source_bytes
from inspect_release import Image,value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof
from verify_native_absolute_bindings import runtime_sections,stable_sections

KEY='native_role_local_names'


def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    raw=(root/path).read_bytes()
    assert sha(raw)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
    return json.loads(raw)


def native_vocabulary(r,root=ROOT):
    data=(root/r['native_dbg_path']).read_bytes();assert sha(data)==r['native_dbg_sha256']
    dw=Image(data).dwarf();groups=collections.defaultdict(list)
    for row in r['rows']:groups[row['source']].append(row)
    checked=set()
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if source=='cards/odyssey.c':source='cards/odissey.c'
        if source not in groups:continue
        for row in groups[source]:
            for rename in row['renames']:
                assert rename['native_locals']
                for ref in rename['native_locals']:
                    d=dw.get_DIE_from_refaddr(int(ref['die'],16));owner=dw.get_DIE_from_refaddr(int(ref['owner_die'],16))
                    if d.cu.cu_offset!=cu.cu_offset:continue
                    assert owner.tag=='DW_TAG_subprogram' and inherited_value(owner,'DW_AT_name')==row['name']
                    assert d.tag=='DW_TAG_variable' and 'DW_AT_location' in d.attributes and inherited_value(d,'DW_AT_name')==rename['new_name']==ref['native_name']
                    parent=d.get_parent();path=[]
                    while parent is not None and parent.offset!=owner.offset:path.append(parent.tag);parent=parent.get_parent()
                    assert parent is not None and path==ref['scope_path'] and not set(path)&{'DW_TAG_inlined_subroutine','DW_TAG_subprogram'}
                    t=unqualify(inherited_type(d));assert t.tag=='DW_TAG_base_type' and value(t,'DW_AT_name')=='int' and sizeof(t)==4 and value(t,'DW_AT_encoding')==5
                    checked.add((source,row['name'],rename['old_name'],ref['die']))
        cu._dielist.clear();cu._diemap.clear()
    expected={(row['source'],row['name'],v['old_name'],ref['die']) for row in r['rows'] for v in row['renames'] for ref in v['native_locals']}
    assert checked==expected
    return len(checked)


def verify(proof,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    p=root/r['preprocess_before_record'];assert sha(p.read_bytes())==r['preprocess_before_record_sha256']
    metadata=json.loads(p.read_bytes());assert sha((root/metadata['compiler_path']).read_bytes())==metadata['compiler_sha256']
    pp={v['source']:v for v in metadata['rows']};by_source=collections.defaultdict(list)
    for row in r['rows']:by_source[row['source']].append(row)
    count=renames=0
    for unit in r['files']:
        source=unit['source'];rows=by_source.pop(source)
        raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pp[source]['source_bytes_sha256']
        before=raw.decode('latin1').replace('\r\n','\n');current=read(root/source);assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(pp[source],root).decode('latin1').replace('\r\n','\n');get_function,_=preprocess_view(expanded)
        projected_source=before;expected=''.join('\n' if line.startswith('#') else line for line in expanded.splitlines(keepends=True))
        for row in rows:
            a,b=span(projected_source,row['name']);assert projected_source[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256']
            after,n=project(row['before_definition'],row['renames']);assert after==row['after_definition'] and n==row['occurrences'] and sha(after.encode())==row['after_sha256']
            projected_source=projected_source[:a]+after+projected_source[b:]
            body,_=get_function(row['name']);assert body==row['expanded_before_definition']
            raw_facts={v['old_name']:v for v in candidates(row['before_definition'])};expanded_facts={v['old_name']:v for v in candidates(body)}
            for rename in row['renames']:
                old,new=rename['old_name'],rename['new_name'];a=raw_facts[old];b=expanded_facts[old]
                assert a==rename['source_role'] and b==rename['expanded_role'] and a['role']==b['role']==rename['role']
                assert (a['writes'],a['unary_updates'])==(b['writes'],b['unary_updates'])
                assert not re.search(r'\b'+re.escape(new)+r'\b',mask(body))
            replacement,n=project(body,row['renames']);assert replacement==row['expanded_projected_definition'] and n==row['expanded_occurrences']
            assert expected.count(body)==1;expected=expected.replace(body,replacement,1)
            edits=[v for v in proof['source_edits'] if v.get('role_local_names_extension')==proof[KEY] and (v['source'],v.get('name'))==(source,row['name'])]
            assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256'])
            count+=row['occurrences'];renames+=len(row['renames'])
        assert current==projected_source and before.count('\n')==current.count('\n')
        observed=unpack(unit['preprocess_after'],root).decode('latin1')
        # Renaming preserves every line; even ASSERT line literals must stay
        # equal. The normalizer also checks their exact physical source lines.
        assert normalized_expansion(expected,source,before)==normalized_expansion(observed,source,current)
        assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256']
    assert not by_source
    native_refs=native_vocabulary(r,root);sections=[]
    for label in ['before','after']:
        item=r['runtime_'+label];data=(root/item['path']).read_bytes();assert sha(data)==item['sha256'];observed=runtime_sections(Image(data));assert observed==item['sections'];sections.append(stable_sections(observed))
    identical=[name for name in sections[0] if name in sections[1] and sections[0][name]==sections[1][name]]
    comparison=dict(identical_sections=len(identical),changed_sections=sorted(name for name in set(sections[0])|set(sections[1]) if name not in identical))
    assert comparison==r['runtime_comparison']
    assert (len(r['rows']),len(r['files']),renames,count)==(r['counts']['functions'],r['counts']['source_units'],r['counts']['variables'],r['counts']['identifier_occurrences'])
    return dict(renamed_locals=renames,functions=len(r['rows']),source_units=len(r['files']),identifier_occurrences=count,native_local_references=native_refs,runtime_comparison=comparison,
                scope='Identifier-only edits with unchanged lines, complete expanded-unit alpha projection, unique ordinary int bindings, no target-name capture and stable source/expanded value roles. Proposed spellings are direct non-inline native int32 local vocabulary; optimized-variable identity is not asserted. No new native coverage or gameplay validation.')


@contextmanager
def historical_readers(proof,root=ROOT):
    r=record(proof,root)
    if not r:
        yield {};return
    stats=verify(proof,root);inputs={root/u['source']:(root/u['before_path']).read_bytes() for u in r['files']}
    texts={p:raw.decode('latin1').replace('\r\n','\n') for p,raw in inputs.items()}
    with source_reader_view(root,texts,exclude={__name__},byte_inputs=inputs):yield stats


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load()),indent=2))
