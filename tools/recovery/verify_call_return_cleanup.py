"""Validate direct returns using matching int conversions and expanded source flow."""
import collections,json
from contextlib import contextmanager
from canonicalize_helpers import ROOT,read,span,sha
from call_return_source import analyze,project
from preprocessed_conditions import preprocess_view
from special_call_condition_source import analysis_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from source_reader_view import source_reader_view,source_bytes

KEY='native_call_return_cleanup'


def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    raw=(root/path).read_bytes()
    assert sha(raw)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
    return json.loads(raw)


def verify(proof,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    assert sha((root/r['compiler_path']).read_bytes())==r['compiler_sha256']
    p=root/r['preprocess_before_record'];assert sha(p.read_bytes())==r['preprocess_before_record_sha256']
    pp={v['source']:v for v in json.loads(p.read_bytes())['rows']};by_source=collections.defaultdict(list)
    for row in r['rows']:by_source[row['source']].append(row)
    count=removed=assertions=0
    for unit in r['files']:
        source=unit['source'];rows=by_source.pop(source)
        raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pp[source]['source_bytes_sha256']
        before=raw.decode('latin1').replace('\r\n','\n');current=read(root/source)
        assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(pp[source],root).decode('latin1').replace('\r\n','\n')
        assert '#define __SIZEOF_INT__ 4\n' in expanded
        get_function,_=preprocess_view(expanded)
        projected_source=before;expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True))
        for row in rows:
            name=row['name'];a,b=span(projected_source,name)
            assert projected_source[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256']
            after,facts=project(row['before_definition'],row['facts']['source_candidates'],remove_only=row['facts']['removed_local_declarations'])
            assert (after,facts)==(row['after_definition'],row['facts']) and sha(after.encode())==row['after_sha256']
            projected_source=projected_source[:a]+after+projected_source[b:]
            body,origin=get_function(name);assert body==row['expanded_before_definition']
            view,adapters=analysis_view(body,name,True) if name in {'card_karn_liberated','card_torgaar_famine_incarnate'} else (body,[])
            assert adapters==row['expanded_adapters'];candidates=analyze(view)
            assert len(row['checks'])==facts['inlined_assignments'];original_start=span(before,name)[0]
            for raw_fact,check in zip(facts['source_candidates'],row['checks']):
                fact=check['expanded_fact'];assert fact in candidates and fact['variable']==raw_fact['variable']
                assert before.count('\n',0,original_start+raw_fact['assignment_offset'])+1==check['source_assignment_line']
                assert before.count('\n',0,original_start+raw_fact['return_offset'])+1==check['source_return_line']
                assert origin(fact['assignment_offset'])==(source,check['source_assignment_line'])
                assert origin(fact['return_offset'])==(source,check['source_return_line'])
            expansion,ef=project(body,[v['expanded_fact'] for v in row['checks']],analysis_text=view,remove_only=facts['removed_local_declarations'])
            assert ef['removed_local_declarations']==facts['removed_local_declarations'] and expansion==row['expanded_projected_definition']
            assert expected.count(body)==1;expected=expected.replace(body,expansion,1)
            edits=[v for v in proof['source_edits'] if v.get('call_return_cleanup_extension')==proof[KEY] and (v['source'],v.get('name'))==(source,name)]
            assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256'])
            count+=facts['inlined_assignments'];removed+=len(facts['removed_local_declarations'])
        assert projected_source==current,source
        observed=unpack(unit['preprocess_after'],root).decode('latin1')
        expected_tokens,n=normalized_expansion(expected,source,before);actual_tokens,m=normalized_expansion(observed,source,current)
        assert n==m and expected_tokens==actual_tokens,(source,'Unprojected expanded-code change')
        assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256']
        assertions+=n
    assert not by_source
    expected=r['counts'];assert len(r['rows'])==expected['functions'] and len(r['files'])==expected['source_units'] and count==expected['inlined_assignments'] and removed==expected['removed_declarations']
    return dict(cleaned_definitions=len(r['rows']),source_units=len(r['files']),inlined_call_results=count,removed_unused_int_locals=removed,assertion_sites=assertions,pending_statements=len(r['pending']),
                scope=r['conversion_proof']+' Raw and expanded CFGs bind the sole adjacent return and private nonvolatile automatic int object; complete expanded units admit only projected edits, whitespace and independently checked ASSERT line literals. No gameplay or whole-DLL byte-identity claim.')


@contextmanager
def historical_readers(proof,root=ROOT):
    """Validate live returns, then expose their exact predecessor for old receipts."""
    r=record(proof,root)
    if not r:
        yield {};return
    stats=verify(proof,root)
    inputs={root/unit['source']:(root/unit['before_path']).read_bytes() for unit in r['files']}
    texts={p:raw.decode('latin1').replace('\r\n','\n') for p,raw in inputs.items()}
    with source_reader_view(root,texts,exclude={__name__},byte_inputs=inputs):yield stats


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load()),indent=2))
