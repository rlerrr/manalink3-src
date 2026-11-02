"""Validate the last four call-result rewrites with bounded syntax adapters."""
import gzip,json
from source_reader_view import source_bytes
from canonicalize_helpers import ROOT,read,span,sha
from call_condition_source import analyze,project
from call_result_types import inspect as compiler_types
from preprocessed_conditions import preprocess_view
from special_call_condition_source import analysis_view
from verify_assertion_call_conditions import unpack,normalized_expansion

KEY='native_special_call_condition_cleanup'


def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    raw=(root/path).read_bytes();assert sha(raw)==next(v for v in proof['interface_supporting_inputs'] if v['path']==path)['sha256']
    return json.loads(raw)


def verify(proof,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    assert sha((root/r['compiler_path']).read_bytes())==r['compiler_sha256']
    p=root/r['before_preprocess_record'];assert sha(p.read_bytes())==r['before_preprocess_record_sha256']
    pp={v['source']:v for v in json.loads(p.read_bytes())['rows']}
    p=root/r['compiler_type_record'];assert sha(p.read_bytes())==r['compiler_type_record_sha256']
    types={v['source']:v for v in json.loads(p.read_bytes())['rows']};count=0
    for unit in r['files']:
        source=unit['source'];raw=(root/unit['before_path']).read_bytes();assert sha(raw)==pp[source]['source_bytes_sha256']==unit['before_bytes_sha256']
        before=raw.decode('latin1').replace('\r\n','\n');current=read(root/source)
        assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(pp[source],root).decode('latin1').replace('\r\n','\n')
        assert '#define __SIZEOF_INT__ 4\n' in expanded
        get_function,_=preprocess_view(expanded);expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True));projected_source=before
        for row in [v for v in r['rows'] if v['source']==source]:
            name=row['name'];a,b=span(projected_source,name);assert projected_source[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256']
            view,adapters=analysis_view(row['before_definition'],name)
            assert adapters==row['source_adapters']
            after,facts=project(row['before_definition'],row['facts']['source_candidates'],analysis_text=view)
            assert (after,facts)==(row['after_definition'],row['facts']) and sha(after.encode())==row['after_sha256']
            projected_source=projected_source[:a]+after+projected_source[b:]
            body,origin=get_function(name);assert body==row['expanded_before_definition']
            ev,adapters=analysis_view(body,name,True);assert adapters==row['expanded_adapters']
            candidates=analyze(ev);assert len(row['checks'])==facts['inlined_assignments']
            observed_types=compiler_types(unpack(types[source],root).decode('latin1'),{name})[name] if name!='charge_spell_cost' else None
            original_start=span(before,name)[0]
            for raw_fact,check in zip(facts['source_candidates'],row['checks']):
                e=check['expanded_fact'];assert e in candidates
                assert before.count('\n',0,original_start+raw_fact['assignment_offset'])+1==check['source_assignment_line']
                assert before.count('\n',0,original_start+raw_fact['condition_offset'])+1==check['source_condition_line']
                assert origin(e['assignment_offset'])==(source,check['source_assignment_line']) and origin(e['condition_offset'])==(source,check['source_condition_line'])
                assert (e['variable'],e['condition_operator'],e['callee'])==(raw_fact['variable'],raw_fact['condition_operator'],raw_fact['callee'])
                if name=='charge_spell_cost':
                    call=body[e['call_start']:e['call_end']]
                    assert call=='((int (*)(int, int, int, int))(0x4302c0))(player, card, 0, 0)'
                    assert row['before_definition'][raw_fact['call_start']:raw_fact['call_end']]=='EXE_FN(int, 0x4302c0, int, int, int, int)(player, card, 0, 0)'
                    assert check['compiler_type_evidence']==dict(kind='literal_executable_pointer',expanded_call=call,return_type='int',address='0x4302c0',parameter_types=['int']*4)
                else:
                    evidence=observed_types[e['callee']];assert evidence==check['compiler_type_evidence'] and evidence and all(v['signed_int32'] for v in evidence)
            expansion,_=project(body,[v['expanded_fact'] for v in row['checks']],analysis_text=ev)
            assert expansion==row['expanded_projected_definition'] and expected.count(body)==1
            expected=expected.replace(body,expansion,1)
            edits=[v for v in proof['source_edits'] if v.get('special_call_condition_extension')==proof[KEY] and (v['source'],v.get('name'))==(source,name)]
            assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256'])
            count+=facts['inlined_assignments']
        assert projected_source==current
        observed=unpack(unit['preprocess_after'],root).decode('latin1')
        assert normalized_expansion(expected,source,before)==normalized_expansion(observed,source,current)
        assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256']
    assert len(r['rows'])==3 and len(r['files'])==3 and count==4
    return dict(cleaned_definitions=3,inlined_call_results=4,removed_unused_int_locals=0,pending_candidates=0,
                scope='Offset-preserving analysis adapters for one pure GNU minimum expression, one compile-time offsetof and seven literal-address callee casts. Raw/expanded CFG and result-type checks cover four rewrites. Whole-unit GCC expansion matches the projected rewrites; real compiler inputs retain every original construct. No gameplay or whole-DLL byte-identity claim.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load()),indent=2))
