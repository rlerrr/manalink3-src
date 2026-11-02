"""Check call-result cleanup while allowing ASSERT diagnostics to follow source lines."""
import collections,gzip,json,re
from pathlib import Path
from source_reader_view import source_bytes
from canonicalize_helpers import ROOT,read,span,sha,mask
from call_condition_source import analyze,project
from call_result_types import inspect as compiler_types
from preprocessed_conditions import preprocess_view
from promote_recovery import LITERAL

KEY='native_assertion_call_condition_cleanup'
ASSERT_DEFINITION='#define ASSERT(cond) do { if (!(cond)) backtrace_and_abort(__FILE__ ":" STRINGIZE(__LINE__) "\\n" #cond); } while (0)'


def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    data=(root/path).read_bytes()
    pin=next(p for p in proof['interface_supporting_inputs'] if p['path']==path)
    assert sha(data)==pin['sha256'],path
    return json.loads(data)


def unpack(entry,root=ROOT):
    data=(root/entry['archive_path']).read_bytes()
    assert sha(data)==entry['archive_sha256']
    raw=gzip.decompress(data);assert sha(raw)==entry['raw_sha256']
    return raw


def normalized_expansion(text,source,original):
    """Ignore whitespace/directives, and only verified ASSERT line literals.

    Filenames, assertion expressions, their stringified text and all other
    literals must still match exactly. Every ignored number must equal the
    actual corresponding ASSERT invocation line in that source revision.
    """
    text=text.replace('\r\n','\n')
    text=''.join(line for line in text.splitlines(keepends=True) if not line.startswith('#'))
    lines=[original.count('\n',0,m.start())+1 for m in re.finditer(r'\bASSERT\s*\(',mask(original))]
    pattern=r'\bbacktrace_and_abort\s*\(\s*'+re.escape(json.dumps(source))+r'\s*":"\s*("(\d+)")\s*"\\n"'
    hits=list(re.finditer(pattern,text))
    assert [int(m[2]) for m in hits]==lines,(source,'ASSERT invocation/diagnostic line mismatch')
    for m in reversed(hits):text=text[:m.start(1)]+'"ASSERT_SOURCE_LINE"'+text[m.end(1):]
    tokens=re.findall(LITERAL+r'|/\*.*?\*/|//[^\n]*|[A-Za-z_0-9]+|>>=|<<=|\.\.\.|->|\+\+|--|&&|\|\||==|!=|<=|>=|<<|>>|[+*/%&|^\-]=|[^\s]',text,re.S)
    return [t for t in tokens if not t.startswith(('/*','//'))],len(hits)


def verify(proof,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    header=read(root/'manalink.h');assert ASSERT_DEFINITION in header
    assert sha((root/r['compiler_path']).read_bytes())==r['compiler_sha256']
    types_path=root/r['compiler_type_record'];assert sha(types_path.read_bytes())==r['compiler_type_record_sha256']
    types={v['source']:v for v in json.loads(types_path.read_bytes())['rows']}
    by_source=collections.defaultdict(list)
    for row in r['rows']:by_source[row['source']].append(row)
    assignments=removed=assertions=0
    for unit in r['files']:
        source=unit['source'];rows=by_source.pop(source)
        raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==types[source]['source_bytes_sha256']
        before=raw.decode('latin1').replace('\r\n','\n');current=read(root/source)
        assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(unit['preprocess_before'],root).decode('latin1').replace('\r\n','\n')
        observed=unpack(unit['preprocess_after'],root).decode('latin1').replace('\r\n','\n')
        get_function,sensitive=preprocess_view(expanded)
        assert set(re.findall(r'\b\w+\b',mask(before)))&sensitive=={'ASSERT'}
        compiled_types=compiler_types(unpack(types[source],root).decode('latin1'),{v['name'] for v in rows})
        projected_source=before;projected_expansion=''.join('\n' if line.startswith('#') else line for line in expanded.splitlines(keepends=True))
        for row in rows:
            name=row['name'];a,b=span(projected_source,name)
            assert projected_source[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256']
            after,facts=project(row['before_definition'],row['facts']['source_candidates'])
            assert after==row['after_definition'] and facts==row['facts'] and sha(after.encode())==row['after_sha256']
            projected_source=projected_source[:a]+after+projected_source[b:]
            body,origin=get_function(name);assert body==row['expanded_before_definition']
            candidates=analyze(body);assert len(row['checks'])==facts['inlined_assignments']
            original_start=span(before,name)[0]
            for raw_fact,check in zip(facts['source_candidates'],row['checks']):
                fact=check['expanded_fact'];assert fact in candidates
                assert before.count('\n',0,original_start+raw_fact['assignment_offset'])+1==check['source_assignment_line']
                assert before.count('\n',0,original_start+raw_fact['condition_offset'])+1==check['source_condition_line']
                assert origin(fact['assignment_offset'])==(source,check['source_assignment_line'])
                assert origin(fact['condition_offset'])==(source,check['source_condition_line'])
                assert (fact['variable'],fact['condition_operator'])==(raw_fact['variable'],raw_fact['condition_operator'])
                evidence=compiled_types[name][fact['callee']]
                assert evidence==check['compiler_type_evidence'] and evidence and all(v['signed_int32'] for v in evidence)
            expansion,_=project(body,[v['expanded_fact'] for v in row['checks']])
            assert expansion==row['expanded_projected_definition'] and projected_expansion.count(body)==1
            projected_expansion=projected_expansion.replace(body,expansion,1)
            edits=[e for e in proof['source_edits'] if e.get('assertion_call_condition_extension')==proof[KEY] and (e['source'],e.get('name'))==(source,name)]
            assert len(edits)==1 and edits[0]['before_sha256']==row['before_sha256'] and edits[0]['after_sha256']==row['after_sha256']
            assignments+=facts['inlined_assignments'];removed+=len(facts['removed_local_declarations'])
        assert projected_source==current,source
        expected,n=normalized_expansion(projected_expansion,source,before)
        actual,m=normalized_expansion(observed,source,current)
        assert n==m and expected==actual,(source,'expanded C changed beyond projected calls and ASSERT source lines')
        assert unit['preprocess_after']['source_bytes_sha256']==sha(source_bytes(root/source))
        assertions+=n
    assert not by_source
    assert (len(r['rows']),len(r['files']),assignments,removed,assertions)==(63,15,154,20,45)
    return dict(cleaned_definitions=63,source_units=15,inlined_call_results=154,removed_unused_int_locals=20,assertion_sites=45,pending_candidates=4,
                scope='Raw and GCC-expanded statement CFG/liveness checks plus signed-int32 GCC call types. Whole-unit expanded C is identical to the projected rewrites except 45 independently checked ASSERT diagnostic source lines and whitespace. No byte matching or gameplay-equivalence claim.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load()),indent=2))
