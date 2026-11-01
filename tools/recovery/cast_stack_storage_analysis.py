"""Inspect shared prompt/payment/test storage, including inlined source scopes."""
import collections
from capstone.x86 import X86_OP_MEM,X86_OP_REG,X86_OP_IMM,X86_REG_EBP,X86_REG_ESP
from canonicalize_helpers import sha
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import sizeof,unqualify,member_offset,array_count
from shared_stack_test_analysis import walk,frame_offset,test_layout,collect,FAMILIES
from verify_inline_helpers import ranges
from verify_wide_stack_test_recovery import decode,graph_for,symbol_at
from header_type_analysis import descriptor,promoted_fields


def scoped_view(image,dw,owner,scope,offset,fields):
    view=collect(image,dw,owner,offset,fields)
    parts=ranges(dw,scope)
    assert parts
    view['scope_ranges']=[dict(address=hex(a),end=hex(b),sha256=sha(image.pe.get_data(a-image.pe.OPTIONAL_HEADER.ImageBase,b-a))) for a,b in parts]
    view['stores']=[s for s in view['stores'] if any(a<=int(s['address'],16)<b for a,b in parts)]
    counts=collections.Counter()
    for s in view['stores']:
        for field in fields:
            if field['offset']<100:continue
            for at in range(max(s['offset'],field['offset']),min(s['offset']+s['size'],field['offset']+field['size'])):
                counts[field['name'],at-field['offset'],None if s['value'] is None else s['value']>>(8*(at-s['offset']))&255]+=1
    view['byte_events']=[dict(field=f,byte=b,value=v,count=c) for (f,b,v),c in sorted(counts.items(),key=lambda kv:(kv[0][0],kv[0][1],str(kv[0][2])))]
    return view


def lifetime(dw,variable,scope):
    parent=variable.get_parent()
    while parent is not None:
        if ranges(dw,parent):return parent
        assert parent.offset!=scope.offset
        parent=parent.get_parent()
    raise AssertionError('No concrete lifetime scope')


def frames(image,debug,rows,*,before=False):
    dw=debug.dwarf();wanted={(r['source'],r['name']):r for r in rows};result={}
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if source not in {s for s,n in wanted}:continue
        for owner in cu.get_top_DIE().iter_children():
            if owner.tag!='DW_TAG_subprogram' or not ranges(dw,owner):continue
            scopes=[owner]+[d for d in walk(owner) if d.tag=='DW_TAG_inlined_subroutine']
            for scope in scopes:
                name=inherited_value(scope,'DW_AT_name');key=(source,name)
                if key not in wanted:continue
                row=wanted[key];variables=[v for v in walk(scope) if v.tag=='DW_TAG_variable' and
                    inherited_value(v,'DW_AT_name')==row['source_array'] and 'DW_AT_location' in v.attributes]
                assert len(variables)==1,(key,row['source_array'],len(variables))
                assert key not in result,key
                variable=variables[0];t=unqualify(inherited_type(variable));offset=frame_offset(dw,variable)
                assert value(owner,'DW_AT_frame_base')==[156]
                shape=dict(size=sizeof(t),kind=t.tag,element_size=sizeof(inherited_type(t)),count=array_count(t))
                assert t.tag=='DW_TAG_array_type' and sizeof(t)==row['source_array_size']
                array_scope=lifetime(dw,variable,scope)
                item=dict(owner=inherited_value(owner,'DW_AT_name'),owner_die=hex(owner.offset),scope_die=hex(scope.offset),
                    scope_kind=scope.tag,array=dict(variable_die=hex(variable.offset),frame_offset=offset,shape=shape,
                        scope_die=hex(array_scope.offset),scope_ranges=[[a,b] for a,b in ranges(dw,array_scope)]),tests=[])
                if not before:
                    for test_name in row['test_names']:
                        tests=[v for v in walk(scope) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')==test_name and
                            'DW_AT_location' in v.attributes and unqualify(inherited_type(v)).tag=='DW_TAG_structure_type']
                        assert len(tests)==1,(key,test_name,len(tests))
                        test=tests[0];test_scope=lifetime(dw,test,scope);fields=test_layout(inherited_type(test));test_offset=frame_offset(dw,test)
                        assert fields==row['native_tests'][0]['layout']
                        item['tests'].append(dict(name=test_name,variable_die=hex(test.offset),frame_offset=test_offset,
                            scope_die=hex(test_scope.offset),size=sizeof(inherited_type(test)),layout=fields,
                            view=scoped_view(image,dw,owner,test_scope,test_offset,fields)))
                if name=='card_engineered_explosives':
                    instances=[v for v in walk(scope) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')=='instance']
                    assert len(instances)==1
                    pointer=unqualify(inherited_type(instances[0]));assert pointer.tag=='DW_TAG_pointer_type'
                    target=unqualify(inherited_type(pointer));assert sizeof(target)==300
                    field=next(f for f in promoted_fields(descriptor(target)) if f[0]=='info_slot')
                    assert field[1:]==[56,None,None,['DW_TAG_base_type',4,5]]
                    item['instance_info_slot']=dict(record_size=300,offset=56,size=4)
                result[key]=item
        cu._dielist.clear();cu._diemap.clear()
    assert set(result)==set(wanted),set(wanted)-set(result)
    return result


def info_slot_flow(view,image,debug,frame):
    """Prove the CMC store is get_card_instance(player,card)->info_slot."""
    md,ins=decode(view,image);graph=graph_for(ins);by_at={i.address:i for i in ins}
    unknown=frozenset({'unknown'});states={ins[0].address:{('ebp',8,4):frozenset({'formal:player'}),('ebp',12,4):frozenset({'formal:card'})}}
    todo=collections.deque([ins[0].address]);callees={}
    for i in ins:
        if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM:
            try:callees[i.address]=symbol_at(image,debug,i.operands[0].imm)
            except AssertionError:pass
    def regkey(reg):
        f=FAMILIES.get(md.reg_name(reg));return ('reg',f[0]) if f else None
    def memkey(op):
        if op.type==X86_OP_MEM and not op.mem.index and op.mem.base in [X86_REG_EBP,X86_REG_ESP]:
            return ('ebp' if op.mem.base==X86_REG_EBP else 'esp',op.mem.disp,op.size)
    def operand(state,op):
        if op.type==X86_OP_REG:
            f=FAMILIES.get(md.reg_name(op.reg))
            if f and f[1:]==(32,0):return state.get(regkey(op.reg),unknown)
        if op.type==X86_OP_MEM:
            if memkey(op):return state.get(memkey(op),unknown)
            if op.size==4 and not op.mem.index and op.mem.disp==56 and state.get(regkey(op.mem.base),unknown)==frozenset({'instance:player,card'}):
                return frozenset({'instance.info_slot'})
        return unknown
    updates=0;instance_calls=set()
    while todo:
        at=todo.popleft();i=by_at[at];state=dict(states[at]);ops=i.operands
        if i.mnemonic=='call':
            player=state.get(('esp',0,4),unknown);card=state.get(('esp',4,4),unknown)
            state={k:v for k,v in state.items() if k[0]!='esp'}
            for n in ['eax','ecx','edx']:state['reg',n]=unknown
            if callees.get(at)=='_get_card_instance' and player==frozenset({'formal:player'}) and card==frozenset({'formal:card'}):
                state['reg','eax']=frozenset({'instance:player,card'});instance_calls.add(at)
        else:
            for reg in i.regs_access()[1]:
                key=regkey(reg)
                if key:state[key]=unknown
            if i.mnemonic=='mov' and len(ops)==2:
                if ops[0].type==X86_OP_REG:
                    f=FAMILIES.get(md.reg_name(ops[0].reg))
                    if f and f[1:]==(32,0):state[regkey(ops[0].reg)]=operand(states[at],ops[1])
                elif memkey(ops[0]):state[memkey(ops[0])]=operand(states[at],ops[1])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(state);todo.append(dest);continue
            old=states[dest];keys=set(old)|set(state)
            merged={k:old.get(k,unknown)|state.get(k,unknown) for k in keys}
            if merged!=old:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    stores=[s for s in view['stores'] if s['offset']==128]
    assert len(stores)==1 and stores[0]['size']==4 and stores[0]['value'] is None
    site=int(stores[0]['address'],16);i=by_at[site]
    assert i.mnemonic=='mov' and i.operands[0].mem.disp==frame+8+128
    assert operand(states[site],i.operands[1])==frozenset({'instance.info_slot'})
    assert instance_calls
    return dict(kind='all_direct_cfg_paths_use_instance_info_slot',store=hex(site),field_offset=56,field_size=4,
        test_cmc_offset=128,instance_calls=[hex(a) for a in sorted(instance_calls)],visited_instructions=len(states))
