"""Native/current fixed arrays, filter fields and definite pointer operands."""
import collections
from capstone.x86 import X86_OP_IMM,X86_OP_MEM
from array_census_analysis import symbol_map
from array_role_analysis import snapshots
import array_role_analysis as frame_effects
from bounded_switch_analysis import graph
from scalar_frame_analysis import FAMILIES,collect
from shared_stack_test_analysis import frame_offset,test_layout
from token_aggregate_analysis import locals_walk
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import sizeof,unqualify
from verify_inline_helpers import ranges
from verify_wide_stack_test_recovery import decode


def inspect(image,debug,rows):
    result=snapshots(image,debug,rows,with_effects=False);dw=debug.dwarf();symbols=symbol_map(image,debug)
    for row in result.values():
        d=dw.get_DIE_from_refaddr(int(row['die_offset'],16));md,ins=decode(row['view'],image);cfg,tables=graph(ins,image)
        old_graph,old_lanes=frame_effects.graph_for,frame_effects.FAMILIES
        try:
            frame_effects.graph_for=lambda _:cfg;frame_effects.FAMILIES=FAMILIES
            effects=frame_effects.effects(image,debug,row['view'],call_argument_words=24)
        finally:frame_effects.graph_for,frame_effects.FAMILIES=old_graph,old_lanes
        instructions={hex(i.address):i for i in ins}
        for call in effects['calls']:
            i=instructions[call['address']];op=i.operands[0]
            target=op.imm if op.type==X86_OP_IMM else op.mem.disp if op.type==X86_OP_MEM and not op.mem.base and not op.mem.index else None
            names=symbols.get(target,set());normalized=set()
            for n in names:
                n=n.lstrip('_')
                for prefix in ['imp__','imp_']:
                    if n.startswith(prefix):n=n[len(prefix):];break
                normalized.add(n)
            known=normalized&{'sprintf','scnprintf','type_text','show_deck_exe','get_sacrifice_prompt_text','select_target',
                'validate_target_impl','display_error_message','backtrace_and_abort','new_default_test_definition'}
            if len(known)==1:
                call['name']='_'+next(iter(known));call['resolved_coff_names']=sorted(names)
        row['effects']=effects;row['checked_switch_tables']=tables;tests=[]
        for v in locals_walk(d):
            if v.tag!='DW_TAG_variable':continue
            t=unqualify(inherited_type(v))
            if t is None or t.tag!='DW_TAG_structure_type' or sizeof(t)!=216:continue
            try:off=frame_offset(dw,v)
            except AssertionError:continue
            scope=v.get_parent()
            while not ranges(dw,scope):scope=scope.get_parent()
            fields=test_layout(t);raw=collect(image,dw,d,off,fields,start=100,end=216)
            stores=[s for s in raw['stores'] if any(a<=int(s['address'],16)<b for a,b in ranges(dw,scope))]
            events=collections.Counter()
            for s in stores:
                for b in range(s['size']):events[s['offset']+b,None if s['value'] is None else s['value']>>(8*b)&255]+=1
            tests.append(dict(name=inherited_value(v,'DW_AT_name'),frame_offset=off,layout=fields,
                stores=stores,byte_events=[[o,v,n] for (o,v),n in sorted(events.items(),key=lambda kv:(kv[0][0],str(kv[0][1])))]))
        row['tests']=tests
        if inherited_value(d,'DW_AT_name')=='pick_special_permanent_for_sacrifice' and tests:
            from target_storage_analysis import store_values
            td=next(v for v in row['variables'] if v['name']=='td')
            assert len(tests)==1
            t=tests[0];row['sacrifice_parameter_values']=store_values(image,dw,d,row['view'],td['frame_offset'],t['stores'])
    return result


def array_shape(v):
    return tuple(v['shape']['dimensions']),v['shape']['element_size'],v['shape']['size']


def pointer_uses(row,array):
    """Physical stack words inside reported scopes; reused slots can alias.

    Private optimized callees can also receive register arguments. Word indices
    are not claimed to be source parameter ordinals or unique variable identity.
    """
    target=['frame',array['frame_offset']+8];parts=array['scope_ranges'];uses=[]
    for c in row['effects']['calls']:
        if not any(a<=int(c['address'],16)<b for a,b in parts):continue
        args=[j for j,a in enumerate(c['arguments']) if a==target]
        if args:uses.append(dict(address=c['address'],callee=c['name'],stack_word_indices=args,stack_words=c['arguments']))
    return uses


def validate(native,current,name):
    left=[v for v in native['variables'] if v['shape']['kind']=='DW_TAG_array_type']
    right=[v for v in current['variables'] if v['shape']['kind']=='DW_TAG_array_type']
    # The pre-existing generic-spell two-word decompiler temporary is outside
    # this array-name queue. Every actual native fixed array must be present.
    wanted=collections.Counter((v['name'],array_shape(v)) for v in left)
    found=collections.Counter((v['name'],array_shape(v)) for v in right)
    assert not wanted-found,(name,wanted-found)
    assert len(native['tests'])==len(current['tests'])
    for old in native['tests']:
        assert any(t['layout']==old['layout'] and t['byte_events']==old['byte_events'] for t in current['tests']),(name,old['name'],old['stores'])
    if name=='pick_special_permanent_for_sacrifice':
        def values(r):return [(s['offset'],s['size'],s['bits']) for s in r['sacrifice_parameter_values']]
        assert len(values(native))==9 and values(native)==values(current)
    facts=[]
    for old in left:
        candidates=[v for v in right if (v['name'],array_shape(v))==(old['name'],array_shape(old))]
        native_uses=pointer_uses(native,old)
        current_uses=[dict(frame_offset=v['frame_offset'],pointer_uses=pointer_uses(current,v)) for v in candidates]
        native_signatures={(u['callee'],tuple(u['stack_word_indices'])) for u in native_uses}
        current_signatures={(u['callee'],tuple(u['stack_word_indices'])) for v in current_uses for u in v['pointer_uses']}
        for v in current_uses:
            for u in v['pointer_uses']:
                if u['callee']=='_scnprintf' and 0 in u['stack_word_indices']:
                    assert u['stack_words'][1]==['const',0xffffffff,100] and old['shape']['size']>=100
        facts.append(dict(name=old['name'],shape=old['shape'],native_frame=old['frame_offset'],
            native_pointer_uses=native_uses,current_candidates=current_uses,
            unmatched_definite_native_signatures=[dict(callee=n,stack_word_indices=list(a)) for n,a in sorted(native_signatures-current_signatures,key=str)]))
    return dict(restored_native_array_instances=len(left),native_test_instances=len(native['tests']),arrays=facts,
        unmatched_additional_current_arrays=[dict(name=k[0],dimensions=list(k[1][0]),element_bytes=k[1][1],bytes=k[1][2],count=n) for k,n in (found-wanted).items()])
