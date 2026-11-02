"""Validate recovered forward guards against source and actual expansions."""
import collections,json
from contextlib import contextmanager
from pycparser import c_ast
from canonicalize_helpers import ROOT,read,span,sha
from forward_guard_source import project
from source_record_flow import parse
from call_condition_source import walk
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from source_reader_view import source_reader_view,source_bytes

KEY='native_forward_guard_cleanup'


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
    gotos=labels=raw_pairs=expanded_pairs=assertions=0
    for unit in r['files']:
        source=unit['source'];rows=groups.pop(source);raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pp[source]['source_bytes_sha256'];before=raw.decode('latin1').replace('\r\n','\n');current=read(root/source);assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(pp[source],root).decode('latin1').replace('\r\n','\n');get_function,_=preprocess_view(expanded);projected=before;expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True))
        for row in rows:
            name=row['name'];a,b=span(projected,name);assert projected[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256'];after,facts=project(row['before_definition'],row['facts']['rounds']);assert (after,facts)==(row['after_definition'],row['facts']) and sha(after.encode())==row['after_sha256'];projected=projected[:a]+after+projected[b:]
            body,origin=get_function(name);assert body==row['expanded_before_definition'];fn,offset=parse(row['before_definition']);ef,eo=parse(body);start=span(before,name)[0];bindings=[dict(label=n.name,source_line=before.count('\n',0,start+offset(n))+1) for n in walk(fn) if isinstance(n,c_ast.Goto)];assert bindings==row['goto_bindings'];assert [(n.name,origin(eo(n))) for n in walk(ef) if isinstance(n,c_ast.Goto)]==[(v['label'],(source,v['source_line'])) for v in bindings]
            result,ep=project(body,row['facts']['rounds']);assert (result,ep)==(row['expanded_projected_definition'],row['expanded_facts']);assert expected.count(body)==1;expected=expected.replace(body,result,1)
            edits=[e for e in proof['source_edits'] if e.get('forward_guard_cleanup_extension')==proof[KEY] and (e['source'],e.get('name'))==(source,name)];assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256'])
            gotos+=facts['removed_gotos'];labels+=len(facts['removed_labels']);raw_pairs+=sum(v['cfg_bisimulation']['state_pairs'] for v in facts['rounds']);expanded_pairs+=sum(v['cfg_bisimulation']['state_pairs'] for v in ep['rounds'])
        assert projected==current
        observed=unpack(unit['preprocess_after'],root).decode('latin1');et,n=normalized_expansion(expected,source,before);at,m=normalized_expansion(observed,source,current);assert n==m and et==at;assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256'];assertions+=n
    assert not groups and gotos==r['counts']['removed_gotos'] and labels==r['counts']['removed_labels'] and len(r['rows'])==r['counts']['functions'] and len(r['files'])==r['counts']['source_units']
    return dict(cleaned_definitions=len(r['rows']),source_units=len(r['files']),removed_gotos=gotos,removed_labels=labels,raw_cfg_state_pairs=raw_pairs,expanded_cfg_state_pairs=expanded_pairs,assertion_sites=assertions,scope='Forward jump guards become conditional blocks. Conditions evaluate once; boolean negation swaps successors, with no integer-complement assumptions. No declarations move into a narrower scope. Every action, call, predicate and return retains its order in raw and expanded CFGs. Complete actual expansions match. No new native function coverage or gameplay tests.')


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
