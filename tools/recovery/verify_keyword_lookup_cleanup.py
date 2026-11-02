"""Check keyword-table equivalence and the original-machine comparison receipt."""
import gzip,json
from contextlib import contextmanager
from canonicalize_helpers import ROOT,read,span,sha
from keyword_lookup_analysis import recover
from keyword_lookup_source import bindings,project,enum_probes
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from source_reader_view import source_reader_view,source_bytes

KEY='native_keyword_lookup_cleanup'


def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    data=(root/path).read_bytes();assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
    return json.loads(data)


def verify(proof,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    assert len(r['rows'])==len(r['files'])==1;row=r['rows'][0];unit=r['files'][0];source=row['source'];meta=r['preprocess_before'];pre=meta['rows'][0];assert sha((root/meta['compiler_path']).read_bytes())==meta['compiler_sha256']
    raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pre['source_bytes_sha256'];before=raw.decode('latin1').replace('\r\n','\n');a,b=span(before,row['name']);assert before[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256']
    assert bindings(row['bdd']['table'],read(root/'card_id.h'),read(root/'defs.h'))==row['bindings'];bdd=recover(row['before_definition']);assert bdd==row['bdd'];after,facts=project(row['before_definition'],bdd['table'],row['bindings']);assert (after,facts)==(row['after_definition'],row['facts']) and sha(after.encode())==row['after_sha256']
    current=read(root/source);assert current==before[:a]+after+before[b:] and sha(current.encode())==unit['after_source_sha256'];assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256']
    expanded=unpack(pre,root).decode('latin1').replace('\r\n','\n');getfn,_=preprocess_view(expanded);body,_=getfn(row['name']);assert body==row['expanded_before_definition'] and recover(body)==row['expanded_bdd'];assert row['expanded_bdd']['table']==bdd['table'] and row['expanded_bdd']['bits']==bdd['bits'];rendered,ef=project(body,bdd['table'],row['bindings']);assert (rendered,ef)==(row['expanded_projected_definition'],row['expanded_facts'])
    expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True));assert expected.count(body)==1;expected=expected.replace(body,rendered,1);actual=unpack(unit['preprocess_after'],root).decode('latin1');et,n=normalized_expansion(expected,source,before);at,m=normalized_expansion(actual,source,current);assert n==m and et==at
    probe=r['enum_probe'];assert probe['exit_code']==0;data=unpack(probe,root);assert data==unpack(pre,root)+b'\n'+enum_probes(row['bindings']).encode();assert sha((root/probe['log_path']).read_bytes())==probe['log_sha256']
    native=row['native_comparison'];assert sha((root/native['native_path']).read_bytes())==native['native_sha256'];assert sha((root/native['source_path']).read_bytes())==native['source_sha256'];assert sha((root/native['executable_path']).read_bytes())==native['executable_sha256'];assert sha((root/meta['compiler_path']).read_bytes())==native['compiler_sha256'];result=native['result'];assert result['all_equal'] and result['comparisons']==9231550 and result['fixed_request_masks']==70 and (result['exhaustive_card_min'],result['exhaustive_card_max'])==(-65536,65535)
    edits=[e for e in proof['source_edits'] if e.get('keyword_lookup_cleanup_extension')==proof[KEY]];assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256'])
    for key,value in facts.items():assert r['counts'][key]==value
    return dict(cleaned_definitions=1,source_units=1,**facts,native_comparisons=result['comparisons'],whole_int32_c_equivalence=True,readonly_release_function=True,assertion_sites=n,enum_bindings_checked=len(row['bindings']['cards'])+len(row['bindings']['keywords']),scope='Canonical BDD identity over all64 input bits proves finite per-card keyword membership with no cross-keyword interactions. Reached local reads, shifts and signed arithmetic are checked. Actual GCC enumerator/type probes and whole-unit expansion bind the rendered switch to that map. Original machine code matches predecessor and switch on9231550 recorded comparisons. Native full32-bit input space and gameplay are not exhaustively checked; no new native function coverage claimed.')


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
