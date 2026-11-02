"""Check reaching-zero direction folds, dead initializers and a native byte name."""
import collections,concurrent.futures,json,os,subprocess
from contextlib import contextmanager
from pathlib import Path
from canonicalize_helpers import ROOT,read,span,sha
from residual_direction_source import analyze,project,probe_unit
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from source_reader_view import source_reader_view,source_bytes

KEY='native_residual_direction_cleanup'

def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    data=(root/path).read_bytes();assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path);return json.loads(data)

def native_name(r,root):
    from inspect_release import Image,inherited_value,value
    from global_initializers import inherited_type
    from object_initializers import unqualify,sizeof
    p=root/r['native_name_record'];data=p.read_bytes();assert sha(data)==r['native_name_record_sha256'];v=json.loads(data);raw=(root/v['native_dbg_path']).read_bytes();assert sha(raw)==v['native_dbg_sha256'];dw=Image(raw).dwarf();owner=dw.get_DIE_from_refaddr(int(v['owner_die'],16));local=dw.get_DIE_from_refaddr(int(v['local_die'],16));assert inherited_value(owner,'DW_AT_name')==v['name'] and inherited_value(local,'DW_AT_name')==v['local_name'];parent=local.get_parent()
    while parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
    assert parent.offset==owner.offset;t=unqualify(inherited_type(local));assert value(t,'DW_AT_name')==v['type_name']=='unsigned char' and sizeof(t)==v['type_bytes']==1;return 1

def verify(proof,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    p=root/r['preprocess_before_record'];assert sha(p.read_bytes())==r['preprocess_before_record_sha256'];meta=json.loads(p.read_bytes());pp={v['source']:v for v in meta['rows']};assert sha((root/meta['compiler_path']).read_bytes())==meta['compiler_sha256']
    p=root/r['compiler_type_record'];assert sha(p.read_bytes())==r['compiler_type_record_sha256'];types=json.loads(p.read_bytes());assert types['compiler_sha256']==meta['compiler_sha256'] and not types['failed'];type_units={v['source']:v for v in types['rows']};groups=collections.defaultdict(list)
    for row in r['rows']:groups[row['source']].append(row)
    counts=collections.Counter();assertions=0;probes=[]
    for unit in r['files']:
        source=unit['source'];rows=groups.pop(source);raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pp[source]['source_bytes_sha256'];before=raw.decode('latin1').replace('\r\n','\n');current=read(root/source);assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(pp[source],root).decode('latin1').replace('\r\n','\n');get_function,_=preprocess_view(expanded);projected=before;expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True))
        for row in rows:
            name=row['name'];a,b=span(projected,name);assert projected[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256'];after,facts=project(row['before_definition'],row['facts']['source_candidates'],name);assert (after,facts)==(row['after_definition'],row['facts']) and sha(after.encode())==row['after_sha256'];projected=projected[:a]+after+projected[b:]
            body,origin=get_function(name);assert body==row['expanded_before_definition'];ef=analyze(body,name,True);original_start=span(before,name)[0];assert len(row['checks'])==len(facts['source_candidates'])
            for fact,check in zip(facts['source_candidates'],row['checks']):
                bound=check['expanded_fact'];assert bound in ef and bound['variable']==fact['variable'];assert [before.count('\n',0,original_start+u['flag_offset'])+1 for u in fact['stride_uses']]==check['source_stride_lines'];assert [origin(u['flag_offset']) for u in bound['stride_uses']]==[(source,n) for n in check['source_stride_lines']]
                assert [(u['kind'],u['scale'],u['forward_step'],u['fold_literal']) for u in fact['stride_uses']]==[(u['kind'],u['scale'],u['forward_step'],u['fold_literal']) for u in bound['stride_uses']]
            expansion,ep=project(body,[c['expanded_fact'] for c in row['checks']],name,True);assert (expansion,ep)==(row['expanded_projected_definition'],row['expanded_facts']);assert expected.count(body)==1;expected=expected.replace(body,expansion,1)
            edits=[e for e in proof['source_edits'] if e.get('residual_direction_cleanup_extension')==proof[KEY] and (e['source'],e.get('name'))==(source,name)];assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256'])
            counts.update({k:facts[k] for k in ['folded_strides','removed_flags','removed_initializations','renamed_role_locals']})
        assert projected==current
        observed=unpack(unit['preprocess_after'],root).decode('latin1');et,n=normalized_expansion(expected,source,before);at,m=normalized_expansion(observed,source,current);assert n==m and et==at;assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256'];assertions+=n
        stdin=probe_unit(expanded,rows).encode('latin1');t=type_units[source];assert sha(stdin)==t['stdin_sha256'] and t['exit_code']==0 and t['stride_type_checks']==sum(len(c['expanded_fact']['stride_uses']) for row in rows for c in row['checks']);opts=pp[source]['argv'][1:pp[source]['argv'].index('-E')];argv=[meta['compiler_path'],*opts,'-fpreprocessed','-fsyntax-only','-x','c','-'];assert argv==t['argv'];probes.append((source,argv,stdin))
    assert not groups
    env=dict(os.environ);env['PATH']=str(Path(meta['compiler_path']).parent)+os.pathsep+env['PATH']
    def replay(probe):
        source,argv,stdin=probe;p=subprocess.run(argv,input=stdin,cwd=root,env=env,capture_output=True);assert p.returncode==0,(source,p.stderr.decode(errors='replace'))
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:list(pool.map(replay,probes))
    assert all(counts[k]==r['counts'][k] for k in counts) and len(r['rows'])==r['counts']['functions'] and len(r['files'])==r['counts']['source_units']
    return dict(cleaned_definitions=len(r['rows']),source_units=len(r['files']),**counts,replayed_stride_type_assertions=counts['folded_strides'],native_name_bindings=native_name(r,root),assertion_sites=assertions,pending_candidates=len(r['pending']),scope='Statement-CFG reaching states prove zero at every folded read with no same-node flag write. Whole-function liveness proves the initializer value dead after folding; a later character role remains and uses native unsigned-byte vocabulary. Offset-preserving views retain all real engine-call descriptors in compiler/product input. GCC type/value/base checks and complete actual expansions pass. No new native function coverage or gameplay claim.')

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
