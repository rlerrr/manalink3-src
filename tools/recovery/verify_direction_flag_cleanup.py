"""Check private zero-direction-state removal and unsigned stride types."""
import collections,concurrent.futures,json,os,subprocess
from contextlib import contextmanager
from pathlib import Path
from canonicalize_helpers import ROOT,read,span,sha
from direction_flag_source import analyze,project,probe_unit
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from source_reader_view import source_reader_view,source_bytes

KEY='native_direction_flag_cleanup'

def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    data=(root/path).read_bytes();assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
    return json.loads(data)

def verify_record(proof,r,key,root=ROOT,read_source=read):
    if not r:return {}
    p=root/r['preprocess_before_record'];assert sha(p.read_bytes())==r['preprocess_before_record_sha256'];meta=json.loads(p.read_bytes());pp={v['source']:v for v in meta['rows']};assert sha((root/meta['compiler_path']).read_bytes())==meta['compiler_sha256']
    p=root/r['compiler_type_record'];assert sha(p.read_bytes())==r['compiler_type_record_sha256'];types=json.loads(p.read_bytes());assert types['compiler_sha256']==meta['compiler_sha256'] and not types['failed'];type_units={v['source']:v for v in types['rows']}
    by_source=collections.defaultdict(list)
    for row in r['rows']:by_source[row['source']].append(row)
    flags=strides=assertions=0;probes=[]
    for unit in r['files']:
        source=unit['source'];rows=by_source.pop(source);raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pp[source]['source_bytes_sha256'];before=raw.decode('latin1').replace('\r\n','\n');current=read_source(root/source);assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(pp[source],root).decode('latin1').replace('\r\n','\n');get_function,_=preprocess_view(expanded);projected=before;expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True))
        for row in rows:
            name=row['name'];a,b=span(projected,name);assert projected[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256'];after,facts=project(row['before_definition'],row['facts']['source_candidates']);assert (after,facts)==(row['after_definition'],row['facts']) and sha(after.encode())==row['after_sha256'];projected=projected[:a]+after+projected[b:]
            body,origin=get_function(name);assert body==row['expanded_before_definition'];ef=analyze(body);original_start=span(before,name)[0];assert len(row['checks'])==facts['removed_flags']
            for fact,check in zip(facts['source_candidates'],row['checks']):
                bound=check['expanded_fact'];assert bound in ef and bound['variable']==fact['variable'];assert before.count('\n',0,original_start+fact['zero_assignment_offset'])+1==check['source_assignment_line'];assert origin(bound['zero_assignment_offset'])==(source,check['source_assignment_line'])
                assert [before.count('\n',0,original_start+v['flag_offset'])+1 for v in fact['stride_uses']]==check['source_stride_lines'];assert [origin(v['flag_offset']) for v in bound['stride_uses']]==[(source,n) for n in check['source_stride_lines']]
                assert [(v['kind'],v['scale'],v['forward_step']) for v in fact['stride_uses']]==[(v['kind'],v['scale'],v['forward_step']) for v in bound['stride_uses']]
            expansion,expanded_facts=project(body,[c['expanded_fact'] for c in row['checks']]);assert (expansion,expanded_facts)==(row['expanded_projected_definition'],row['expanded_facts']);assert expected.count(body)==1;expected=expected.replace(body,expansion,1)
            edits=[e for e in proof['source_edits'] if e.get('direction_flag_cleanup_extension')==proof[key] and (e['source'],e.get('name'))==(source,name)];assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256']);flags+=facts['removed_flags'];strides+=facts['folded_strides']
        assert projected==current
        observed=unpack(unit['preprocess_after'],root).decode('latin1');et,n=normalized_expansion(expected,source,before);at,m=normalized_expansion(observed,source,current);assert n==m and et==at;assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256'];assertions+=n
        stdin=probe_unit(expanded,rows).encode('latin1');t=type_units[source];assert sha(stdin)==t['stdin_sha256'] and t['exit_code']==0 and t['stride_type_checks']==sum(len(c['expanded_fact']['stride_uses']) for row in rows for c in row['checks'])
        options=pp[source]['argv'][1:pp[source]['argv'].index('-E')];argv=[meta['compiler_path'],*options,'-fpreprocessed','-fsyntax-only','-x','c','-'];assert argv==t['argv'];probes.append((source,argv,stdin))
    assert not by_source
    env=dict(os.environ);env['PATH']=str(Path(meta['compiler_path']).parent)+os.pathsep+env['PATH']
    def replay(probe):
        source,argv,stdin=probe;result=subprocess.run(argv,input=stdin,cwd=root,env=env,capture_output=True);assert result.returncode==0,(source,result.stderr.decode(errors='replace'))
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:list(pool.map(replay,probes))
    c=r['counts'];assert (len(r['rows']),len(r['files']),flags,strides)==(c['functions'],c['source_units'],c['removed_flags'],c['folded_strides'])
    return dict(cleaned_definitions=len(r['rows']),source_units=len(r['files']),removed_direction_flags=flags,folded_stride_expressions=strides,replayed_stride_type_assertions=strides,assertion_sites=assertions,pending_candidates=len(r['pending']),scope='Private ordinary integer flags are zero at function entry and referenced only by recognized stride math. Unsigned literals preserve conversions; GCC checks nonvolatile primitive flag storage, old/new expression-type identity, constant-zero values, integer/pointer bases and the32-bit model. Raw/expanded projections and complete actual units match; clear/copy loops and memory actions remain in place. No new native coverage, whole-DLL identity or gameplay claim.')

def verify(proof,root=ROOT):return verify_record(proof,record(proof,root),KEY,root,read)

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
