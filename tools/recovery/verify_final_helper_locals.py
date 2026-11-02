"""Check typed helper locals, byte-lane dependencies and the native chooser fix."""
import collections,json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify
from token_aggregate_analysis import inspect
from helper_local_source import project,TYPEDEFS
from helper_word_flow import analyze
from helper_local_proofs import prove,loop_predicate_proofs
from final_helper_local_analysis import declarations,validate_types,validate

_records={}


def record(proof,root):
    path=proof.get('native_final_helper_locals')
    if not path:return None
    p=root/path;key=str(p),p.stat().st_mtime_ns
    if key not in _records:_records[key]=json.loads(p.read_bytes())
    return _records[key]


def projected(row):
    after,changes=project(row['name'],row['before_definition'])
    assert after==row['after_definition'] and changes==row['changes']
    assert sha(after.encode())==row['after_sha256'] and sha(row['before_definition'].encode())==row['before_sha256']
    return after


def has_source_repair(proof,root,source,name,old_sha):
    r=record(proof,root)
    if not r:return False
    rows=[v for v in r['rows'] if (v['source'],v['name'])==(source,name)]
    if not rows:return False
    assert len(rows)==1;row=rows[0];after=projected(row);text=read(root/source);a,b=span(text,name);assert text[a:b]==after
    todo=[old_sha];seen=set()
    while todo:
        digest=todo.pop()
        if digest==row['before_sha256']:break
        if digest in seen:continue
        seen.add(digest)
        for e in proof['source_edits']:
            if e['source']!=source or e.get('before_sha256')!=digest:continue
            before,next_text=e.get('before'),e.get('after')
            if not isinstance(before,str) or not isinstance(next_text,str):continue
            try:assert span(before,name) and span(next_text,name)
            except (AssertionError,ValueError,TypeError):continue
            assert sha(before.encode())==digest and sha(next_text.encode())==e['after_sha256'];todo.append(e['after_sha256'])
    else:return False
    edits=[e for e in proof['source_edits'] if e.get('final_helper_locals_extension')==proof['native_final_helper_locals'] and (e['source'],e.get('name'))==(source,name)]
    assert len(edits)==1 and edits[0]['before_sha256']==row['before_sha256'] and edits[0]['after_sha256']==row['after_sha256']
    return True


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    assert has_source_repair(proof,root,source,name,old_sha);r=record(proof,root);assert sha(image.data)==r['native_dll_sha256']
    row=next(v for v in r['rows'] if (v['source'],v['name'])==(source,name));v=row['native_snapshot']['view'];a,b=int(v['address'],16),int(v['end'],16)
    assert sha(image.pe.get_data(a-image.pe.OPTIONAL_HEADER.ImageBase,b-a))==v['bytes_sha256']


def verify_current_row(proof,image,root,source,name,old_sha):
    verify_superseded_definition(proof,image,root,source,name,old_sha);r=record(proof,root);row=next(v for v in r['rows'] if (v['source'],v['name'])==(source,name))
    ci,cd=Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes());nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    current=inspect(ci,cd,[row])[source,name]
    return validate(row,row['native_snapshot'],current,image,nd,ci,cd)


def event_values(debug,snapshot):
    dw=debug.dwarf();d=dw.get_DIE_from_refaddr(int(snapshot['owner_die'],16))
    params=[v for v in d.iter_children() if v.tag=='DW_TAG_formal_parameter' and inherited_value(v,'DW_AT_name')=='event'];assert len(params)==1
    t=unqualify(inherited_type(params[0]));assert t.tag=='DW_TAG_enumeration_type'
    return {value(v,'DW_AT_name'):value(v,'DW_AT_const_value') for v in t.iter_children() if v.tag=='DW_TAG_enumerator'}


def chooser_proof(image,debug,snapshot):
    """DWARF p frame location plus a straight-line load/store window."""
    from elftools.dwarf.locationlists import LocationParser
    from elftools.dwarf.dwarf_expr import DWARFExprParser
    from final_helper_local_analysis import owned_walk
    from verify_inline_helpers import ranges
    dw=debug.dwarf();d=dw.get_DIE_from_refaddr(int(snapshot['owner_die'],16));pc=0x26a125c;found=[]
    for v in owned_walk(d):
        if v.tag!='DW_TAG_variable' or inherited_value(v,'DW_AT_name')!='p' or 'DW_AT_location' not in v.attributes:continue
        scope=v.get_parent()
        while not ranges(dw,scope):scope=scope.get_parent()
        if not any(a<=pc<b for a,b in ranges(dw,scope)):continue
        locs=LocationParser(dw.location_lists()).parse_from_attribute(v.attributes['DW_AT_location'],v.cu['version'],die=v)
        base=value(v.cu.get_top_DIE(),'DW_AT_low_pc',0)
        for loc in locs:
            if hasattr(loc,'base_address'):base=loc.base_address;continue
            a,b=loc.begin_offset+(0 if loc.is_absolute else base),loc.end_offset+(0 if loc.is_absolute else base)
            if a<=pc<b:
                ops=DWARFExprParser(dw.structs).parse_expr(loc.loc_expr)
                found.append((hex(v.offset),a,b,[(x.op_name,x.args) for x in ops]))
    assert len(found)==1 and found[0][3]==[('DW_OP_fbreg',[-248])]
    td=[v for v in snapshot['variables'] if v['name']=='td9'];assert len(td)==1 and td[0]['frame_offset']==-236
    fields={v['name']:v for v in td[0]['layout']['fields']};assert fields['who_chooses']['offset']==0 and fields['player']['offset']==72
    code=image.pe.get_data(0x26a1242-image.pe.OPTIONAL_HEADER.ImageBase,32)
    # mov eax,[ebp-240]; two literal stores to other words; mov [ebp-228],eax.
    assert code.hex()=='8b8510ffffffc78528ffffff00100000c78538ffffff0000000089851cffffff'
    return dict(variable_die=found[0][0],location_range=[hex(found[0][1]),hex(found[0][2])],p_cfa_offset=-248,
                load_address='0x26a1242',store_address=hex(pc),instruction_window_hex=code.hex(),target_cfa_offset=-236,field='who_chooses',field_offset=0,
                scope='Native p-to-who_chooses dataflow and exact destination. Caller-input origin of p and opaque values are separate.')


def verify(proof,image,root=ROOT):
    r=record(proof,root)
    if not r:return {}
    assert sha(image.data)==r['native_dll_sha256'];nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(nd.data)==r['native_dbg_sha256']
    rows=r['rows'];assert len(rows)==2 and {v['name'] for v in rows}=={'generic_spell','granted_generic_activated_ability'}
    native=inspect(image,nd,rows,native=True);dn=declarations(nd,rows);builds=[]
    for label in ['before','after']:
        a=r['runtime_'+label];im,db=Image((root/a['dll_path']).read_bytes()),Image((root/a['dbg_path']).read_bytes())
        assert sha(im.data)==a['dll_sha256'] and sha(db.data)==a['dbg_sha256'];builds.append((im,db,inspect(im,db,rows)))
    ci,cd=Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes());current=inspect(ci,cd,rows);dc=declarations(cd,rows);da=declarations(builds[1][1],rows);kinds=collections.Counter()
    for row in rows:
        key=row['source'],row['name'];assert native[key]==row['native_snapshot'] and builds[0][2][key]==row['before_snapshot'] and builds[1][2][key]==row['after_snapshot']
        assert validate(row,native[key],builds[1][2][key],image,nd,builds[1][0],builds[1][1])==row['after_validation']
        assert validate_types(row['name'],dn[row['name']],da[row['name']])==row['type_validation']==validate_types(row['name'],dn[row['name']],dc[row['name']])
        stats=validate(row,native[key],current[key],image,nd,ci,cd);kinds.update(v['kind'] for v in stats['record_scalar_profile_comparisons'])
        verify_superseded_definition(proof,image,root,*key,row['before_sha256'])
    generic=next(v for v in rows if v['name']=='generic_spell');key=generic['source'],generic['name'];events=event_values(nd,native[key]);compiled_events=event_values(cd,current[key]);assert events==r['word_flow']['event_values'] and all(compiled_events[n]==v for n,v in events.items())
    flow=analyze(generic['before_definition'],events);assert flow==r['word_flow']['analysis'] and not flow['unresolved'] and len(flow['writes'])==23 and len(flow['reads'])==40 and len(flow['event_classes'])==6
    assert prove(generic['before_definition'],generic['after_definition'],generic['changes'],events)==r['source_liveness_proof'];assert loop_predicate_proofs()==r['loop_predicate_proofs']
    assert chooser_proof(image,nd,native[key])==r['native_chooser_value_proof']
    assert 'td9.player = pcStack_f4.number;' in generic['before_definition'] and 'td9.who_chooses = p;' in generic['after_definition']
    assert [e['before'] for e in r['file_edits']]==TYPEDEFS
    for e in r['file_edits']:
        assert not e['after'] and e['before'] not in read(root/e['source'])
        assert any(v.get('final_helper_locals_file_extension')==proof['native_final_helper_locals'] and all(v[k]==e[k] for k in ['source','before','after','before_sha256','after_sha256']) for v in proof['source_edits'])
    a,b=r['before_link_inputs'],r['after_link_inputs'];assert set(a)==set(b) and {p for p in a if a[p]!=b[p]}=={'.recovery/build-gcc93/obj/functions/functions.o'}
    return dict(helper_definitions=2,removed_synthetic_union_locals=4,removed_unused_union_typedefs=2,scalar_profile_comparison_kinds=dict(kinds),union_word_writes=23,union_word_reads=40,event_classes=6,native_chooser_field_corrections=1,changed_linker_objects=1,unchanged_linker_objects=len(a)-1,scope=r['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
