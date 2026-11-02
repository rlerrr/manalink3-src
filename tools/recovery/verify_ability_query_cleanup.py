"""Verify the reconstructed activation exclusion switch and ability queries."""
import json,re
from contextlib import contextmanager
from canonicalize_helpers import ROOT,read,span,sha,mask
from ability_query_source import project,excluded_cards,canonical,EXPECTED
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from source_reader_view import source_reader_view,source_bytes

KEY='native_ability_query_cleanup'


def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    data=(root/path).read_bytes();assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
    return json.loads(data)


def verify(proof,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    assert len(r['rows'])==7 and len(r['files'])==1;unit=r['files'][0];source=unit['source'];meta=r['preprocess_before'];pre=meta['rows'][0];assert sha((root/meta['compiler_path']).read_bytes())==meta['compiler_sha256'];raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pre['source_bytes_sha256'];before=raw.decode('latin1').replace('\r\n','\n');current=read(root/source);assert sha(current.encode())==unit['after_source_sha256'] and sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256'];names={v['csvid']:v['name'] for v in r['card_names']}
    header_names={int(v):n for n,v in re.findall(r'\b(CARD_ID_\w+)\s*=\s*(\d+)',read(root/'card_id.h'))};assert all(header_names[k]==v for k,v in names.items())
    expanded=unpack(pre,root).decode('latin1').replace('\r\n','\n');getfn,_=preprocess_view(expanded);expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True));projected=before;lines=placeholders=locals_=0
    native=r['native_comparison'];harness=(root/native['source_path']).read_bytes();assert sha(harness)==native['source_sha256'];harness_text=harness.decode().replace('\r\n','\n');assert sha((root/native['executable_path']).read_bytes())==native['executable_sha256'];assert sha((root/native['native_path']).read_bytes())==native['native_sha256'];assert sha((root/meta['compiler_path']).read_bytes())==native['compiler_sha256'];assert len(native['functions'])==7 and len(native['external_data_bindings'])==5
    result=native['result'];assert result['all_equal'] and result['three_way_comparisons']==33213200 and result['classifier_cases']==16777216 and result['rules_call_trace_checked'] and result['instance_and_extension_writes_checked']
    for row in r['rows']:
        name=row['name'];a,b=span(projected,name);assert projected[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256'];after=project(row['before_definition'],name,r['cards'],names);assert after==row['after_definition'] and sha(after.encode())==row['after_sha256'];projected=projected[:a]+after+projected[b:]
        body,_=getfn(name);assert body==row['expanded_before_definition'] and canonical(body)==EXPECTED[name];replacement=project(body,name,r['cards'],names,expanded=True);assert replacement==row['expanded_projected_definition'];assert expected.count(body)==1;expected=expected.replace(body,replacement,1)
        assert harness_text.count(row['before_definition'].replace(name+'(','predecessor_'+name+'(',1))==1 and harness_text.count(after.replace(name+'(','recovered_'+name+'(',1))==1
        pattern=r'\b(?:[A-Za-z]*Var\d+|[A-Za-z]*Stack_[0-9a-fA-F]+)\b';facts=dict(source_lines_removed=row['before_definition'].count('\n')-after.count('\n'),removed_placeholder_occurrences=len(re.findall(pattern,mask(row['before_definition']))),removed_placeholder_names=len(set(re.findall(pattern,mask(row['before_definition'])))));assert facts==row['facts'];assert not re.search(pattern,mask(after));lines+=facts['source_lines_removed'];placeholders+=facts['removed_placeholder_occurrences'];locals_+=facts['removed_placeholder_names']
        if name=='has_an_activated_ability':assert excluded_cards(row['before_definition'])==(r['cards'],r['predicate']) and excluded_cards(body)==(r['cards'],r['predicate'])
        edits=[v for v in proof['source_edits'] if v.get('ability_query_cleanup_extension')==proof[KEY] and v.get('name')==name];assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256'])
    assert projected==current
    observed=unpack(unit['preprocess_after'],root).decode('latin1');assert normalized_expansion(expected,source,before)==normalized_expansion(observed,source,current)
    probe=r['enum_probe'];assert probe['exit_code']==0 and unpack(probe,root)==unpack(pre,root)+b'\n#include <stddef.h>\n'+r['enum_probe_text'].encode();assert sha((root/probe['log_path']).read_bytes())==probe['log_sha256']
    assert (lines,placeholders,locals_)==(r['counts']['source_lines_removed'],r['counts']['removed_placeholder_occurrences'],r['counts']['removed_placeholder_names'])
    return dict(cleaned_definitions=7,source_units=1,excluded_cards=len(r['cards']),source_lines_removed=lines,removed_placeholder_occurrences=placeholders,removed_placeholder_names=locals_,native_three_way_comparisons=result['three_way_comparisons'],classifier_feature_cases=result['classifier_cases'],query_cases=result['query_cases'],enum_and_layout_assertions=r['enum_probe_text'].count('_Static_assert'),scope='Complete uint16 exclusion classification with proven low16-bit dependencies; source flag/control-flow reconstruction and actual enum/record probes. Original native branch bodies use five relocated data operands and three shared opaque callees. Native, predecessor and reconstructed results, call traces and instance/extension writes match on33213200 cases. Other complete32-bit query states and gameplay are not exhausted; no new native coverage claimed.')


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
