"""Check bounded retry-loop reconstruction with CFGs and actual GCC types."""
import collections,concurrent.futures,json,os,subprocess
from contextlib import contextmanager
from pathlib import Path
from canonicalize_helpers import ROOT,read,span,sha
from target_loop_source import analyze,project,probe_unit
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from source_reader_view import source_reader_view,source_bytes

KEY='native_target_loop_cleanup'

def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    data=(root/path).read_bytes();assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
    return json.loads(data)

def verify_record(proof,r,key,root=ROOT,read_source=read):
    if not r:return {}
    p=root/r['preprocess_before_record'];assert sha(p.read_bytes())==r['preprocess_before_record_sha256'];meta=json.loads(p.read_bytes());pp={v['source']:v for v in meta['rows']}
    assert sha((root/meta['compiler_path']).read_bytes())==meta['compiler_sha256']
    p=root/r['compiler_type_record'];assert sha(p.read_bytes())==r['compiler_type_record_sha256'];types=json.loads(p.read_bytes());assert types['compiler_sha256']==meta['compiler_sha256'] and not types['failed'];type_units={v['source']:v for v in types['rows']}
    by_source=collections.defaultdict(list)
    for row in r['rows']:by_source[row['source']].append(row)
    loops=labels=raw_pairs=expanded_pairs=assertions=0;probes=[]
    for unit in r['files']:
        source=unit['source'];rows=by_source.pop(source);raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pp[source]['source_bytes_sha256']
        before=raw.decode('latin1').replace('\r\n','\n');current=read_source(root/source);assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(pp[source],root).decode('latin1').replace('\r\n','\n');get_function,_=preprocess_view(expanded);projected=before;expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True))
        for row in rows:
            name=row['name'];a,b=span(projected,name);assert projected[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256']
            after,facts=project(row['before_definition'],row['facts']['source_candidates']);assert (after,facts)==(row['after_definition'],row['facts']) and sha(after.encode())==row['after_sha256'];projected=projected[:a]+after+projected[b:]
            body,origin=get_function(name);assert body==row['expanded_before_definition'];ef=analyze(body);original_start=span(before,name)[0];assert len(row['checks'])==facts['structured_loops']
            for fact,check in zip(facts['source_candidates'],row['checks']):
                bound=check['expanded_fact'];assert bound in ef and bound['counter']==fact['counter']
                for field,stored in [('loop_offset','source_loop_line'),('exit_guard_offset','source_guard_line'),('outer_condition_offset','source_outer_condition_line')]:
                    assert before.count('\n',0,original_start+fact[field])+1==check[stored] and origin(bound[field])==(source,check[stored])
            expansion,expanded_facts=project(body,[c['expanded_fact'] for c in row['checks']]);assert (expansion,expanded_facts)==(row['expanded_projected_definition'],row['expanded_facts'])
            assert expected.count(body)==1;expected=expected.replace(body,expansion,1)
            edits=[e for e in proof['source_edits'] if e.get('target_loop_cleanup_extension')==proof[key] and (e['source'],e.get('name'))==(source,name)]
            assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256'])
            loops+=facts['structured_loops'];labels+=len(facts['removed_labels']);raw_pairs+=facts['cfg_bisimulation']['state_pairs'];expanded_pairs+=expanded_facts['cfg_bisimulation']['state_pairs']
        assert projected==current
        observed=unpack(unit['preprocess_after'],root).decode('latin1');et,n=normalized_expansion(expected,source,before);at,m=normalized_expansion(observed,source,current);assert n==m and et==at
        assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256'];assertions+=n
        stdin=probe_unit(expanded,rows).encode('latin1');t=type_units[source];assert sha(stdin)==t['stdin_sha256'] and t['exit_code']==0 and t['integer_bound_checks']==sum(len(v['checks']) for v in rows)
        flags=pp[source]['argv'][1:pp[source]['argv'].index('-E')];argv=[meta['compiler_path'],*flags,'-fpreprocessed','-fsyntax-only','-x','c','-'];assert argv==t['argv'];probes.append((source,argv,stdin))
    assert not by_source
    env=dict(os.environ);env['PATH']=str(Path(meta['compiler_path']).parent)+os.pathsep+env['PATH']
    def replay(probe):
        source,argv,stdin=probe;result=subprocess.run(argv,input=stdin,cwd=root,env=env,capture_output=True);assert result.returncode==0,(source,result.stderr.decode(errors='replace'))
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:list(pool.map(replay,probes))
    c=r['counts'];assert (len(r['rows']),len(r['files']),loops,labels,raw_pairs,expanded_pairs)==(c['functions'],c['source_units'],c['structured_loops'],c['removed_labels'],c['raw_cfg_state_pairs'],c['expanded_cfg_state_pairs'])
    return dict(cleaned_definitions=len(r['rows']),source_units=len(r['files']),structured_loops=loops,removed_lab_gotos=loops,removed_lab_labels=labels,raw_cfg_state_pairs=raw_pairs,expanded_cfg_state_pairs=expanded_pairs,replayed_integer_bound_assertions=loops,assertion_sites=assertions,pending_candidates=len(r['pending']),scope='Nested retry loops become skip/continue branches of the same outer do/while. Complete raw and expanded statement CFGs are bisimilar after integer bound complementation and silent jumps; counters are private ordinary ints, bounds are readonly expressions, and valid payloads/counter updates remain in place. GCC proves actual integer operand types and complete actual units match their projections. No fixed-bound or helper-purity assumption, new native coverage, whole-DLL identity or gameplay claim.')

def verify(proof,root=ROOT):
    return verify_record(proof,record(proof,root),KEY,root,read)

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
