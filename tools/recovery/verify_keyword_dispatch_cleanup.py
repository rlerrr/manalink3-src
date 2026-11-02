"""Validate structured keyword dispatch and its native-composed workers."""
import json,re
from contextlib import contextmanager
from canonicalize_helpers import ROOT,read,span,sha,mask
from special_dispatch_source import project as dispatch
from special_dispatch_flow import verify as trace_proof
from keyword_worker_source import project as worker,BODIES
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from source_reader_view import source_reader_view,source_bytes

KEY='native_keyword_dispatch_cleanup'


def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    data=(root/path).read_bytes();assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
    return json.loads(data)


def prefixed(body,p):
    for name in BODIES:body=re.sub(r'\b'+name+r'\b',p+name,body)
    if re.search(r'\bspecial_abilities\s*\(',body):body=body.replace('special_abilities(',p+'special_abilities(',1)
    return body


def verify(proof,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    assert len(r['rows'])==7 and len(r['files'])==1;unit=r['files'][0];source=unit['source'];meta=r['preprocess_before'];pre=meta['rows'][0];assert sha((root/meta['compiler_path']).read_bytes())==meta['compiler_sha256'];raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pre['source_bytes_sha256'];before=raw.decode('latin1').replace('\r\n','\n');current=read(root/source);assert sha(current.encode())==unit['after_source_sha256'] and sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256']
    native=r['native_comparison'];harness=(root/native['source_path']).read_bytes();assert sha(harness)==native['source_sha256'];harness_text=harness.decode().replace('\r\n','\n');assert sha((root/native['executable_path']).read_bytes())==native['executable_sha256'];assert sha((root/native['native_path']).read_bytes())==native['native_sha256'];assert sha((root/meta['compiler_path']).read_bytes())==native['compiler_sha256'];assert len(native['functions'])==8 and len(native['external_data_bindings'])==42
    result=native['result'];assert result['all_equal'] and result['three_way_comparisons']==7768000 and result['callback_mutation_modes']==4 and result['call_trace_and_record_writes_checked']
    isolated=r['isolated_dispatch_comparison'];assert isolated['result']['all_equal'] and isolated['result']['three_way_comparisons']==7768000
    for key in ['source','executable']:assert sha((root/isolated[key+'_path']).read_bytes())==isolated[key+'_sha256']
    a,b=span(current,'is_humiliated');assert current[a:b]==native['supporting_is_humiliated_source']
    expanded=unpack(pre,root).decode('latin1').replace('\r\n','\n');getfn,_=preprocess_view(expanded);expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True));projected=before;totals={k:0 for k in r['rows'][0]['facts']}
    for row in r['rows']:
        name=row['name'];a,b=span(projected,name);assert projected[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256'];after=dispatch(row['before_definition']) if name=='special_abilities' else worker(row['before_definition'],name);assert after==row['after_definition'] and sha(after.encode())==row['after_sha256'];projected=projected[:a]+after+projected[b:]
        body,_=getfn(name);assert body==row['expanded_before_definition'];replacement=dispatch(body,expanded=True) if name=='special_abilities' else worker(body,name);assert replacement==row['expanded_projected_definition'] and expected.count(body)==1;expected=expected.replace(body,replacement,1)
        assert harness_text.count(prefixed(row['before_definition'],'predecessor_'))==1 and harness_text.count(prefixed(after,'recovered_'))==1
        if name=='special_abilities':assert trace_proof(row['before_definition'],after,r['constants'])==r['dispatch_proof'] and trace_proof(body,replacement,r['constants'])==r['expanded_dispatch_proof']
        pattern=r'\b(?:[A-Za-z]*Var\d+|[A-Za-z]*Stack_[0-9a-fA-F]+)\b';facts=dict(source_lines_removed=row['before_definition'].count('\n')-after.count('\n'),removed_lab_gotos=len(re.findall(r'\bgoto\s+LAB_[0-9a-fA-F]+',row['before_definition'])),removed_gotos=len(re.findall(r'\bgoto\b',row['before_definition'])),removed_placeholder_occurrences=len(re.findall(pattern,mask(row['before_definition']))),removed_placeholder_names=len(set(re.findall(pattern,mask(row['before_definition'])))));assert facts==row['facts'];assert not re.search(pattern,mask(after))
        for k,v in facts.items():totals[k]+=v
        edits=[v for v in proof['source_edits'] if v.get('keyword_dispatch_cleanup_extension')==proof[KEY] and v.get('name')==name];assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256'])
    assert projected==current and all(r['counts'][k]==v for k,v in totals.items());observed=unpack(unit['preprocess_after'],root).decode('latin1');assert normalized_expansion(expected,source,before)==normalized_expansion(observed,source,current)
    probe=r['enum_probe'];assert probe['exit_code']==0 and unpack(probe,root)==unpack(pre,root)+b'\n#include <stddef.h>\n'+r['enum_probe_text'].encode();assert sha((root/probe['log_path']).read_bytes())==probe['log_sha256'];storage=r['ordinary_storage_probe'];assert storage['exit_code']==0 and unpack(storage,root)==unpack(pre,root)+b'\n'+storage['assertion_text'].encode();assert sha((root/storage['log_path']).read_bytes())==storage['log_sha256']
    return dict(cleaned_definitions=7,source_units=1,**totals,dispatch_product_states=r['dispatch_proof']['product_states'],dispatch_matched_actions=r['dispatch_proof']['matched_actions'],native_composition_cases=result['three_way_comparisons'],native_isolated_dispatch_cases=isolated['result']['three_way_comparisons'],enum_and_layout_assertions=r['enum_probe_text'].count('_Static_assert'),ordinary_storage_types_checked=True,scope='Full32-bit request-mask symbolic action traces preserve calls, pointer-result stores, record updates and return paths with arbitrary fresh engine facts after every action. Native dispatch and six real worker bodies agree with predecessor and reconstruction on7768000 composition cases, with another7768000 isolated dispatch cases. Callback traces, instances/extensions, must-attack and player flags are checked. Typed global/record probes bind the ordinary-storage assumptions. Complete native32 state space and gameplay are not exhausted; no new native coverage claimed.')


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
