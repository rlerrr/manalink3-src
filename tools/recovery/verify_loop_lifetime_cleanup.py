"""Verify scalar lifetime partitions, native local vocabulary and actual expansions."""
import collections,json
from contextlib import contextmanager
from canonicalize_helpers import ROOT,read,span,sha
from loop_lifetime_source import analyze,project
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from verify_role_local_names import native_vocabulary
from source_reader_view import source_reader_view,source_bytes

KEY='native_loop_lifetime_cleanup'


def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    data=(root/path).read_bytes();assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
    return json.loads(data)


def verify(proof,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    p=root/r['preprocess_before_record'];assert sha(p.read_bytes())==r['preprocess_before_record_sha256'];meta=json.loads(p.read_bytes());pp={v['source']:v for v in meta['rows']};assert sha((root/meta['compiler_path']).read_bytes())==meta['compiler_sha256']
    groups=collections.defaultdict(list)
    for row in r['rows']:groups[row['source']].append(row)
    totals=collections.Counter();assertions=0
    for unit in r['files']:
        source=unit['source'];rows=groups.pop(source);raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pp[source]['source_bytes_sha256'];before=raw.decode('latin1').replace('\r\n','\n');current=read(root/source);assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(pp[source],root).decode('latin1').replace('\r\n','\n');get_function,_=preprocess_view(expanded);projected=before;expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True))
        for row in rows:
            name=row['name'];a,b=span(projected,name);assert projected[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256'];after,facts=project(row['before_definition'],[dict(group=v['source_group'],new_name=v['new_name']) for v in row['renames']]);assert (after,facts)==(row['after_definition'],row['facts']) and sha(after.encode())==row['after_sha256'];projected=projected[:a]+after+projected[b:]
            body,origin=get_function(name);assert body==row['expanded_before_definition'];raw_groups=analyze(row['before_definition']);expanded_groups=analyze(body);start=span(before,name)[0]
            for rename in row['renames']:
                g,e=rename['source_group'],rename['expanded_group'];assert g in raw_groups and e in expanded_groups and g['old_name']==e['old_name']==rename['old_name'] and g['roles']==e['roles'];lines=[before.count('\n',0,start+at)+1 for at in g['definition_offsets']];assert lines==rename['definition_lines'] and [origin(at) for at in e['definition_offsets']]==[(source,n) for n in lines]
            result,ep=project(body,[dict(group=v['expanded_group'],new_name=v['new_name']) for v in row['renames']]);assert (result,ep)==(row['expanded_projected_definition'],row['expanded_facts']) and ep==facts;assert expected.count(body)==1;expected=expected.replace(body,result,1)
            edits=[e for e in proof['source_edits'] if e.get('loop_lifetime_cleanup_extension')==proof[KEY] and (e['source'],e.get('name'))==(source,name)];assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256']);totals.update(facts)
        assert projected==current
        observed=unpack(unit['preprocess_after'],root).decode('latin1');et,n=normalized_expansion(expected,source,before);at,m=normalized_expansion(observed,source,current);assert n==m and et==at;assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256'];assertions+=n
    assert not groups and len(r['rows'])==r['counts']['functions'] and len(r['files'])==r['counts']['source_units']
    for key in ['value_groups','named_bindings']:assert totals[key]==r['counts'][key]
    assert totals['renamed_identifier_occurrences']==r['counts']['identifier_occurrences'] and totals['reused_placeholder_declarations']==r['counts']['removed_placeholder_declarations'] and totals['added_int_declarations']==r['counts']['added_int_declarations']
    return dict(cleaned_definitions=len(r['rows']),source_units=len(r['files']),**totals,native_local_references=native_vocabulary(r,root),assertion_sites=assertions,scope='Conservative reaching definitions separate nonescaping ordinary int value groups. Shared reads and read/modify/write uses merge groups; entry/uninitialized and opaque-write groups cannot acquire names. Each renamed group has an induction role and direct non-inline native int32 local vocabulary. Independent induction components have explicit initialization, unit updates and loop comparison uses; player-index vocabulary is supported by player-argument uses. C member names retain their separate namespace and ordinary identifiers cannot be captured. Merging the new bindings reproduces every predecessor byte. All actual expanded units match. Native optimized-variable identity and new function coverage are not claimed; no gameplay tests.')


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
