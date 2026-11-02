"""Native/current helper record geometry, local types and scoped store profiles."""
import collections,itertools
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof
from shared_stack_record_analysis import layout
from verify_inline_helpers import ranges
from token_aggregate_analysis import validate_locals
from multiphase_record_analysis import exclusive_view,clone_proof


def owned_walk(d):
    for c in d.iter_children():
        if c.tag in ['DW_TAG_subprogram','DW_TAG_inlined_subroutine']:continue
        yield c
        if c.has_children:yield from owned_walk(c)


def declarations(debug,rows):
    dw=debug.dwarf();wanted={v['name'] for v in rows};result={}
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!='functions/functions.c':continue
        for d in cu.get_top_DIE().iter_children():
            name=inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_subprogram' or name not in wanted or not ranges(dw,d):continue
            assert name not in result;variables=[]
            for v in owned_walk(d):
                if v.tag!='DW_TAG_variable':continue
                t=unqualify(inherited_type(v))
                if t is None:continue
                try:size=sizeof(t)
                except ValueError:size=None
                item=dict(name=inherited_value(v,'DW_AT_name'),die=hex(v.offset),kind=t.tag,bytes=size)
                if t.tag=='DW_TAG_structure_type' and size==8:item['layout']=layout(t)
                if t.tag=='DW_TAG_pointer_type':
                    target=unqualify(inherited_type(t));item['pointee_kind']=target.tag if target else None
                    try:item['pointee_bytes']=sizeof(target) if target else None
                    except ValueError:item['pointee_bytes']=None
                variables.append(item)
            result[name]=variables
        cu._dielist.clear();cu._diemap.clear()
    assert set(result)==wanted;return result


def validate_types(name,native,current):
    assert not any(v['kind']=='DW_TAG_union_type' for v in current)
    if name=='generic_spell':
        expected=['old_number_of_targets','rv','min_targets','normal_targeting','old_nt','p','c']
        derived=['aura_spell','graveyard_flags','target_limit']
        for n in expected+derived:
            found=[v for v in current if v['name']==n];assert found and all(v['kind']=='DW_TAG_base_type' and v['bytes']==4 for v in found),(n,found)
            if n in expected:assert any(v['name']==n and v['kind']=='DW_TAG_base_type' and v['bytes']==4 for v in native),n
        for n in ['instance','storage_inst']:
            assert any(v['name']==n and v['kind']=='DW_TAG_pointer_type' and v['bytes']==4 and v.get('pointee_bytes')==300 for v in current)
            assert any(v['name']==n and v['kind']=='DW_TAG_pointer_type' and v['bytes']==4 and v.get('pointee_bytes')==300 for v in native)
        old=next(v for v in native if v['name']=='old_t0');new=[v for v in current if v['name']=='old_t0'];assert len(new)==1 and old['kind']==new[0]['kind']=='DW_TAG_structure_type' and old['bytes']==new[0]['bytes']==8 and old['layout']==new[0]['layout']
        assert not any(v['name'] in ['aiStack_114','pcStack_fc','pcStack_f8','pcStack_f4'] for v in current)
        return dict(native_named_integer_roles=expected,derived_integer_roles=derived,native_instance_pointer_roles=['instance','storage_inst'],native_target_snapshot='old_t0',snapshot_bytes=8)
    assert name=='granted_generic_activated_ability'
    expected=collections.Counter({('td_default','DW_TAG_structure_type',84):2,('test','DW_TAG_structure_type',216):1,('tgt','DW_TAG_structure_type',8):1,('old_number_of_targets','DW_TAG_base_type',4):1})
    for rows in [native,current]:
        found=collections.Counter((v['name'],v['kind'],v['bytes']) for v in rows if v['name'] in {k[0] for k in expected});assert found==expected,found
    return dict(target_definitions=2,filters=1,selected_target_pairs=1,native_saved_target_count_integer=True)


def validate(row,native,current,ni,nd,ci,cd):
    geometry=validate_locals(native,current);comparisons=[]
    ns=[v for v in native['variables'] if 'layout' in v];cs=[v for v in current['variables'] if 'layout' in v];assert len(ns)==len(cs)
    left={v['variable_die']:exclusive_view(native,v,ni,nd,True) for v in ns};right={v['variable_die']:exclusive_view(current,v,ci,cd) for v in cs}
    for size in sorted({v['bytes'] for v in ns}):
        old=[v for v in ns if v['bytes']==size];new=[v for v in cs if v['bytes']==size];assert len(old)==len(new);solutions=[]
        for perm in itertools.permutations(new):
            result=[]
            for n,c in zip(old,perm):
                a,b=left[n['variable_die']],right[c['variable_die']];kind=None;proof=None
                if a['events']==b['events']:kind='exact_exclusive_scope_scalar_events'
                elif a['events'] and b['events']==collections.Counter({k:v*2 for k,v in a['events'].items()}):
                    try:proof=clone_proof(current,b,ci);kind='exclusive_compiled_store_copies'
                    except AssertionError:pass
                elif b['events'] and a['events']==collections.Counter({k:v*2 for k,v in b['events'].items()}):
                    try:proof=clone_proof(native,a,ni);kind='exclusive_native_store_copies'
                    except AssertionError:pass
                if kind is None:break
                result.append(dict(native_die=n['variable_die'],current_die=c['variable_die'],kind=kind,clone_proof=proof,native_scope_exclusions=a['excluded_contained_scope_stores'],current_scope_exclusions=b['excluded_contained_scope_stores']))
            else:solutions.append(result)
        assert solutions,(row['name'],size,[(v['name'],left[v['variable_die']]['events']) for v in old],[(v['name'],right[v['variable_die']]['events']) for v in new]);solutions.sort(key=lambda rs:sum(v['kind']!='exact_exclusive_scope_scalar_events' for v in rs));comparisons.extend(solutions[0])
    return dict(geometry=geometry,record_scalar_profile_comparisons=comparisons,scope='Complete frame-located record/array geometry and bounded scalar-event profiles. Contained same-frame scopes are excluded, filters cover bytes100..215, and every doubled group requires mutually unreachable corresponding stores. Register/piece-located declarations are checked separately. Unknown values and profile matches do not prove unique variable identity, API input provenance or full behavior.')
