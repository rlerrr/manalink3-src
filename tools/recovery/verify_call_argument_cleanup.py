"""Verify call-result contractions against actual GCC expansions and type probes."""
import collections,json
from contextlib import contextmanager
from canonicalize_helpers import ROOT,read,span,sha
from call_argument_source import analyze,project,probe_definition
from direction_flag_source import probe_unit
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from source_reader_view import source_reader_view,source_bytes

KEY='native_call_argument_cleanup'


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
    totals=collections.Counter();assertions=0;pairs=0
    for unit in r['files']:
        source=unit['source'];rows=groups.pop(source);raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pp[source]['source_bytes_sha256'];before=raw.decode('latin1').replace('\r\n','\n');current=read(root/source);assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(pp[source],root).decode('latin1').replace('\r\n','\n');get_function,_=preprocess_view(expanded);projected=before;expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True))
        for row in rows:
            name=row['name'];a,b=span(projected,name);assert projected[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256'];after,facts=project(row['before_definition'],[v['source_fact'] for v in row['checks']]);assert (after,facts)==(row['after_definition'],row['facts']) and sha(after.encode())==row['after_sha256'];projected=projected[:a]+after+projected[b:]
            body,origin=get_function(name);assert body==row['expanded_before_definition'];raw_groups=analyze(row['before_definition']);expanded_groups=analyze(body);start=span(before,name)[0]
            for check in row['checks']:
                g,e=check['source_fact'],check['expanded_fact'];assert {k:v for k,v in g.items() if k!='omit_int_cast'} in raw_groups and {k:v for k,v in e.items() if k!='omit_int_cast'} in expanded_groups and g['omit_int_cast']==e['omit_int_cast'];lines=[before.count('\n',0,start+g[k])+1 for k in ['assignment_start','consumer_offset']];assert lines==check['lines'] and [origin(e[k]) for k in ['assignment_start','consumer_offset']]==[(source,n) for n in lines]
            result,ep=project(body,[v['expanded_fact'] for v in row['checks']]);assert (result,ep)==(row['expanded_projected_definition'],row['expanded_facts']);assert expected.count(body)==1;expected=expected.replace(body,result,1)
            edits=[e for e in proof['source_edits'] if e.get('call_argument_cleanup_extension')==proof[KEY] and (e['source'],e.get('name'))==(source,name)];assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256']);totals['inlined_call_results']+=len(row['checks']);totals['removed_locals']+=len(facts['removed_locals']);pairs+=facts['cfg_contraction']['state_pairs']+ep['cfg_contraction']['state_pairs']
        assert projected==current
        observed=unpack(unit['preprocess_after'],root).decode('latin1');et,n=normalized_expansion(expected,source,before);at,m=normalized_expansion(observed,source,current);assert n==m and et==at;assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256'];assertions+=n
        probe=next(v for v in r['accepted_probes'] if v['source']==source);assert probe['exit_code']==0
        text=probe_unit(expanded,rows,definition_probe=lambda before,row:probe_definition(before,row));assert sha(text.encode('latin1'))==probe['probe_sha256'];assert sha((root/probe['probe_path']).read_bytes())==probe['probe_sha256'];assert sha((root/probe['log_path']).read_bytes())==probe['log_sha256']
    assert not groups and len(r['rows'])==r['counts']['functions'] and len(r['files'])==r['counts']['source_units']
    for key in ['inlined_call_results','removed_locals']:assert totals[key]==r['counts'][key]
    return dict(cleaned_definitions=len(r['rows']),source_units=len(r['files']),**totals,cfg_state_pairs=pairs,assertion_sites=assertions,scope='Adjacent dead nonescaping int call-result stores become direct consumer arguments. Raw and actual expanded CFGs retain calls and control flow after store contraction. GCC proves producer result conversion, direct consumer function type, nonvolatile integral sibling locals and compile-time constants. Address formation reads no object contents. Other side effects and mutable global siblings are excluded. Exact whole-unit expansion and current source pins checked. No new native coverage or gameplay validation claimed.')


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
