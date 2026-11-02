"""Verify scalar lifetime partitions, native local vocabulary and actual expansions."""
import collections,gzip,json
from contextlib import contextmanager
from canonicalize_helpers import ROOT,read,span,sha
from instance_lifetime_source import analyze,project,probe_definition
from direction_flag_source import probe_unit
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from inspect_release import Image,value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof
from source_reader_view import source_reader_view,source_bytes

KEY='native_instance_lifetime_cleanup'


def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    data=(root/path).read_bytes();assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
    return json.loads(data)


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
                    t=unqualify(inherited_type(d));assert t.tag=='DW_TAG_pointer_type' and sizeof(t)==4;target=unqualify(inherited_type(t));assert target.tag=='DW_TAG_structure_type' and value(target,'DW_AT_name')=='card_instance_struct' and sizeof(target)==300
                    checked.add((source,row['name'],rename['old_name'],ref['die']))
        cu._dielist.clear();cu._diemap.clear()
    expected={(row['source'],row['name'],v['old_name'],ref['die']) for row in r['rows'] for v in row['renames'] for ref in v['native_locals']}
    assert checked==expected
    return len(checked)


def verify(proof,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    p=root/r['preprocess_before_record'];assert sha(p.read_bytes())==r['preprocess_before_record_sha256'];meta=json.loads(p.read_bytes());pp={v['source']:v for v in meta['rows']};assert sha((root/meta['compiler_path']).read_bytes())==meta['compiler_sha256']
    groups=collections.defaultdict(list)
    for row in r['rows']:groups[row['source']].append(row)
    totals=collections.Counter();assertions=0
    probes_path=root/r['type_probe_record'];assert sha(probes_path.read_bytes())==r['type_probe_record_sha256'];probes=json.loads(probes_path.read_bytes());assert not probes['failed']
    for unit in r['files']:
        source=unit['source'];rows=groups.pop(source);raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pp[source]['source_bytes_sha256'];before=raw.decode('latin1').replace('\r\n','\n');current=read(root/source);assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(pp[source],root).decode('latin1').replace('\r\n','\n');get_function,_=preprocess_view(expanded);projected=before;expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True))
        for row in rows:
            name=row['name'];a,b=span(projected,name);assert projected[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256'];after,facts=project(row['before_definition'],[dict(group=v['source_group'],new_name=v['new_name']) for v in row['renames']]);assert (after,facts)==(row['after_definition'],row['facts']) and sha(after.encode())==row['after_sha256'];projected=projected[:a]+after+projected[b:]
            body,origin=get_function(name);assert body==row['expanded_before_definition'];raw_groups=analyze(row['before_definition']);expanded_groups=analyze(body);start=span(before,name)[0]
            for rename in row['renames']:
                g,e=rename['source_group'],rename['expanded_group'];assert g in raw_groups and e in expanded_groups and g['old_name']==e['old_name']==rename['old_name'] and g['roles']==e['roles'];lines=[before.count('\n',0,start+at)+1 for at in g['definition_offsets']];assert lines==rename['definition_lines'] and [origin(at) for at in e['definition_offsets']]==[(source,n) for n in lines]
            result,ep=project(body,[dict(group=v['expanded_group'],new_name=v['new_name']) for v in row['renames']]);assert (result,ep)==(row['expanded_projected_definition'],row['expanded_facts']) and ep==facts;assert expected.count(body)==1;expected=expected.replace(body,result,1)
            edits=[e for e in proof['source_edits'] if e.get('instance_lifetime_cleanup_extension')==proof[KEY] and (e['source'],e.get('name'))==(source,name)];assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256']);totals.update(facts)
        assert projected==current
        probe=next(v for v in probes['rows'] if v['source']==source);assert probe['exit_code']==0
        packed=(root/probe['probe_path']).read_bytes();assert sha(packed)==probe['probe_archive_sha256'];raw_probe=gzip.decompress(packed);assert sha(raw_probe)==probe['probe_sha256'];assert sha(probe_unit(expanded,rows,definition_probe=probe_definition).encode('latin1'))==probe['probe_sha256'];assert sha((root/probe['log_path']).read_bytes())==probe['log_sha256']
        observed=unpack(unit['preprocess_after'],root).decode('latin1');et,n=normalized_expansion(expected,source,before);at,m=normalized_expansion(observed,source,current);assert n==m and et==at;assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256'];assertions+=n
    assert not groups and len(r['rows'])==r['counts']['functions'] and len(r['files'])==r['counts']['source_units']
    for key in ['value_groups','named_bindings']:assert totals[key]==r['counts'][key]
    assert totals['renamed_identifier_occurrences']==r['counts']['identifier_occurrences'] and totals['reused_placeholder_declarations']==r['counts']['removed_placeholder_declarations'] and totals['added_typed_declarations']==r['counts']['added_typed_declarations']
    return dict(cleaned_definitions=len(r['rows']),source_units=len(r['files']),**totals,native_local_references=native_vocabulary(r,root),assertion_sites=assertions,scope='Conservative reaching definitions separate nonescaping ordinary card-instance pointer value groups. Shared reads and read/modify/write uses merge groups; entry/uninitialized and opaque-write groups cannot acquire names. Each renamed group has a card-instance lookup role and direct non-inline native pointer local vocabulary. Every definition preserves its exact card_instance_t pointer type; actual compiler probes check the record and pointer identity. C member names retain their separate namespace and ordinary identifiers cannot be captured. Merging the new bindings reproduces every predecessor byte. All actual expanded units match. Native optimized-variable identity and new function coverage are not claimed; no gameplay tests.')


@contextmanager
def historical_readers(proof,root=ROOT):
    r=record(proof,root)
    if not r:
        yield {};return
    stats=verify(proof,root);inputs={root/u['source']:(root/u['before_path']).read_bytes() for u in r['files']};texts={p:b.decode('latin1').replace('\r\n','\n') for p,b in inputs.items()}
    with source_reader_view(root,texts,exclude={__name__},byte_inputs=inputs):yield stats


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load()),indent=2))
