"""Check structured spell guards against native enums and preserved opaque blocks."""
import collections,json
from contextlib import contextmanager
from canonicalize_helpers import ROOT,read,span,sha
from shared_spell_source import project,trace_proof,guard_definition,EVENTS
from preprocessed_conditions import preprocess_view
from verify_assertion_call_conditions import unpack,normalized_expansion
from source_reader_view import source_reader_view,source_bytes
from inspect_release import Image,value,inherited_value
from verify_inline_helpers import ranges
from verify_final_helper_locals import event_values

KEY='native_shared_spell_cleanup'


def record(proof,root=ROOT):
    path=proof.get(KEY)
    if not path:return None
    data=(root/path).read_bytes()
    assert sha(data)==next(v['sha256'] for v in proof['interface_supporting_inputs'] if v['path']==path)
    return json.loads(data)


def native_bindings(r,root=ROOT):
    native=Image((root/r['native_dll_path']).read_bytes());debug=Image((root/r['native_dbg_path']).read_bytes())
    assert sha(native.data)==r['native_dll_sha256'] and sha(debug.data)==r['native_dbg_sha256']
    dw=debug.dwarf();groups=collections.defaultdict(list);seen=set()
    for row in r['rows']:groups[row['source']].append(row)
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if source=='cards/odyssey.c':source='cards/odissey.c'
        if source not in groups:continue
        for row in groups[source]:
            assert row['native_views']
            for view in row['native_views']:
                d=dw.get_DIE_from_refaddr(int(view['owner_die'],16))
                if d.cu.cu_offset!=cu.cu_offset:continue
                assert inherited_value(d,'DW_AT_name')==row['name']
                parts=ranges(dw,d);assert parts==[(int(v['address'],16),int(v['end'],16)) for v in view['ranges']]
                for (a,b),part in zip(parts,view['ranges']):assert sha(native.pe.get_data(a-native.pe.OPTIONAL_HEADER.ImageBase,b-a))==part['sha256']
                observed=event_values(debug,view);assert {n:observed[n] for n in EVENTS}==view['event_values']==row['native_event_values']
                seen.add((source,row['name'],view['owner_die']))
        cu._dielist.clear();cu._diemap.clear()
    assert seen=={(v['source'],v['name'],p['owner_die']) for v in r['rows'] for p in v['native_views']}
    return len(seen)


def current_events(r,root=ROOT):
    """Check each compiled handler's event type, including abstract/folded DIEs."""
    debug=Image((root/'ManalinkEh.dbg').read_bytes());dw=debug.dwarf();wanted={(v['source'],v['name']):v['native_event_values'] for v in r['rows']};seen=set()
    sources={source for source,name in wanted}
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if source not in sources:continue
        for d in cu.get_top_DIE().iter_children():
            key=source,inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_subprogram' or key not in wanted:continue
            try:ev=event_values(debug,dict(owner_die=hex(d.offset)))
            except (AssertionError,KeyError,AttributeError):continue
            assert {n:ev[n] for n in EVENTS}==wanted[key];seen.add(key)
        cu._dielist.clear();cu._diemap.clear()
    assert seen==set(wanted),set(wanted)-seen
    return len(seen)


def verify_record(proof,r,key,root=ROOT,read_source=read):
    if not r:return {}
    p=root/r['preprocess_before_record'];assert sha(p.read_bytes())==r['preprocess_before_record_sha256']
    metadata=json.loads(p.read_bytes());assert sha((root/metadata['compiler_path']).read_bytes())==metadata['compiler_sha256']
    pp={v['source']:v for v in metadata['rows']};by_source=collections.defaultdict(list)
    for row in r['rows']:by_source[row['source']].append(row)
    cases=captures=0
    for unit in r['files']:
        source=unit['source'];rows=by_source.pop(source)
        raw=(root/unit['before_path']).read_bytes();assert sha(raw)==unit['before_bytes_sha256']==pp[source]['source_bytes_sha256']
        before=raw.decode('latin1').replace('\r\n','\n');current=read_source(root/source);assert sha(current.encode())==unit['after_source_sha256']
        expanded=unpack(pp[source],root).decode('latin1').replace('\r\n','\n');assert guard_definition(expanded)==r['guard_definitions'][source]
        get_function,_=preprocess_view(expanded);projected_source=before;expected=''.join('\n' if l.startswith('#') else l for l in expanded.splitlines(keepends=True))
        for row in rows:
            a,b=span(projected_source,row['name']);assert projected_source[a:b]==row['before_definition'] and sha(row['before_definition'].encode())==row['before_sha256']
            after,facts=project(row['before_definition']);assert after==row['after_definition'] and facts==row['facts'] and sha(after.encode())==row['after_sha256']
            projected_source=projected_source[:a]+after+projected_source[b:]
            body,_=get_function(row['name']);assert body==row['expanded_before_definition']
            replacement,ef=project(body,expanded=True);assert replacement==row['expanded_projected_definition'] and ef==row['expanded_facts']
            trace=trace_proof(facts,row['native_event_values']);assert trace==row['trace_proof']==trace_proof(ef,row['native_event_values'])
            assert expected.count(body)==1;expected=expected.replace(body,replacement,1)
            edits=[e for e in proof['source_edits'] if e.get('shared_spell_cleanup_extension')==proof[key] and (e['source'],e.get('name'))==(source,row['name'])]
            assert len(edits)==1 and (edits[0]['before_sha256'],edits[0]['after_sha256'])==(row['before_sha256'],row['after_sha256'])
            cases+=trace['case_count'];captures+=len(facts['private_captures'])
        assert projected_source==current
        observed=unpack(unit['preprocess_after'],root).decode('latin1')
        assert normalized_expansion(expected,source,before)==normalized_expansion(observed,source,current)
        assert sha(source_bytes(root/source))==unit['preprocess_after']['source_bytes_sha256']
    assert not by_source
    native=native_bindings(r,root);compiled=current_events(r,root)
    assert (len(r['rows']),len(r['files']),cases,captures)==(r['counts']['functions'],r['counts']['source_units'],r['counts']['trace_cases'],r['counts']['private_pointer_captures'])
    return dict(structured_handlers=len(r['rows']),source_units=len(r['files']),removed_lab_gotos=sum(v['facts']['removed_gotos'] for v in r['rows']),removed_lab_labels=sum(v['facts']['removed_labels'] for v in r['rows']),source_lines_removed=r['counts']['source_lines_removed'],parsed_trace_cases=cases,private_pointer_captures=captures,native_owner_bindings=native,compiled_event_bindings=compiled,
                scope='Shared constructor/setup executes once on the same supported events; source-identical resolution blocks and tail calls retain their bindings. Five native event values, affinity/global-read order, arbitrary setup global changes and payload return/fallthrough are checked. The pure inline affinity predicate is compiler-bound. An extra private pointer assignment is permitted only when unread outside resolution. This preserves the accepted reconstruction, not a full native/gameplay equivalence proof.')


def verify(proof,root=ROOT):
    return verify_record(proof,record(proof,root),KEY,root,read)


@contextmanager
def historical_readers(proof,root=ROOT):
    r=record(proof,root)
    if not r:
        yield {};return
    stats=verify(proof,root);inputs={root/u['source']:(root/u['before_path']).read_bytes() for u in r['files']}
    texts={p:data.decode('latin1').replace('\r\n','\n') for p,data in inputs.items()}
    with source_reader_view(root,texts,exclude={__name__},byte_inputs=inputs):yield stats


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load()),indent=2))
