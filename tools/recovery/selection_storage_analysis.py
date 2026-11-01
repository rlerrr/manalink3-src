"""Inspect native selection matrices and typed adventure target slots."""
import collections
from capstone.x86 import X86_OP_REG,X86_OP_MEM,X86_OP_IMM
from array_role_analysis import snapshots,effects
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof,member_offset
from shared_stack_test_analysis import walk,frame_offset,FAMILIES
from shared_stack_record_analysis import layout
from verify_inline_helpers import ranges
from verify_wide_stack_test_recovery import decode,graph_for,symbol_at


def adventure_writes(image,debug,snapshot):
    """Track definite instance pointers and owner/IID results through the CFG.

    Eight-byte indexed advances are abstracted as another target slot; this
    checks field/result identity and slot stride, not the chosen index or
    arbitrary aliasing. Calls kill caller-save registers and all loads from
    memory are unknown. Joins retain identical tags only.
    """
    md,ins=decode(snapshot['view'],image);graph=graph_for(ins);by_at={i.address:i for i in ins}
    def family(reg):
        f=FAMILIES.get(md.reg_name(reg))
        return f[0] if f and f[1:]==(32,0) else None
    def operand(state,op):
        if op.type==X86_OP_REG:return state.get(family(op.reg))
        if op.type==X86_OP_IMM:return ('const',op.imm)
        return None
    def address(state,op):
        if op.type!=X86_OP_MEM:return None
        m=op.mem;base=state.get(family(m.base))
        if base is None or base[0]!='instance':return None
        if m.index and m.scale!=8:return None
        return ('instance',base[1]+m.disp)
    def step(i,initial):
        state=dict(initial);ops=i.operands;writes=[];strides=[]
        if i.mnemonic=='mov' and len(ops)==2 and ops[0].type==X86_OP_MEM and ops[0].size==4:
            dest=address(initial,ops[0]);val=operand(initial,ops[1])
            if dest and val and val[0] in ['owner','iid']:writes.append(dict(address=hex(i.address),slot_relative_field_offset=dest[1],result=val[0]))
        for r in i.regs_access()[1]:
            if family(r):state[family(r)]=None
        if i.mnemonic=='call':
            for r in ['eax','ecx','edx']:state[r]=None
            name=symbol_at(image,debug,ops[0].imm) if ops[0].type==X86_OP_IMM else None
            tags={'_get_card_instance':'instance','_get_owner':'owner','_get_original_internal_card_id':'iid'}
            if name in tags:state['eax']=(tags[name],0)
        elif len(ops)==2 and ops[0].type==X86_OP_REG and family(ops[0].reg):
            dest=family(ops[0].reg)
            if i.mnemonic=='mov':state[dest]=operand(initial,ops[1])
            elif i.mnemonic=='lea':
                state[dest]=address(initial,ops[1])
                if state[dest] and ops[1].mem.index:strides.append(dict(address=hex(i.address),bytes=ops[1].mem.scale))
            elif i.mnemonic in ['add','sub']:
                a=operand(initial,ops[0]);b=operand(initial,ops[1])
                if a and a[0]=='instance' and b and b[0]=='const':state[dest]=('instance',a[1]+b[1]*(1 if i.mnemonic=='add' else -1))
        return state,writes,strides
    states={ins[0].address:{}};todo=collections.deque([ins[0].address]);updates=0
    while todo:
        at=todo.popleft();after,*_=step(by_at[at],states[at])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(after);todo.append(dest);continue
            old=states[dest];merged={k:old.get(k) if old.get(k)==after.get(k) else None for k in set(old)|set(after)}
            if merged!=old:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    writes=[];strides=[]
    for at,state in states.items():
        _,w,s=step(by_at[at],state);writes.extend(w);strides.extend(s)
    return dict(writes=sorted(writes,key=lambda r:int(r['address'],16)),slot_advances=sorted(strides,key=lambda r:int(r['address'],16)))


def inspect(image,debug,rows):
    owners=[r for r in rows if r['name']!='resolve_an_adventure']
    result=snapshots(image,debug,owners,with_effects=False)
    dw=debug.dwarf()
    for row in result.values():
        row['effects']=effects(image,debug,row['view'],include_frame_copies=True,call_argument_words=24)
        _,ins=decode(row['view'],image)
        row['instructions']=[[hex(i.address),i.mnemonic,i.op_str] for i in ins]
        die=dw.get_DIE_from_refaddr(int(row['die_offset'],16));types=[]
        for v in walk(die):
            if v.tag!='DW_TAG_variable':continue
            t=unqualify(inherited_type(v))
            if t is None or t.tag not in ['DW_TAG_base_type','DW_TAG_pointer_type']:continue
            item=dict(name=inherited_value(v,'DW_AT_name'),kind=t.tag,size=sizeof(t),encoding=value(t,'DW_AT_encoding'))
            if t.tag=='DW_TAG_pointer_type':
                target=unqualify(inherited_type(t))
                if target is not None:item['target']=dict(kind=target.tag,name=value(target,'DW_AT_name'),size=sizeof(target))
            types.append(item)
        row['local_types']=types
        if inherited_value(die,'DW_AT_name')=='adventure':row['adventure_writes']=adventure_writes(image,debug,row)
    return result


def plain_target_record(dw,row,name='td2'):
    """Bind an actual typed target local for replay of its old field proof."""
    assert (row['source'],row['name'])==('cards/modern_horizons_2.c','card_geyadrone_dihada') and name=='td2'
    found=[]
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!=row['source']:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag!='DW_TAG_subprogram' or inherited_value(die,'DW_AT_name')!=row['name'] or not ranges(dw,die):continue
            variables=[v for v in walk(die) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')==name]
            assert len(variables)==1
            v=variables[0];t=unqualify(inherited_type(v))
            assert t.tag=='DW_TAG_structure_type' and sizeof(t)==84
            found.append(dict(die=die,variable=v,frame_offset=frame_offset(dw,v),size=84,members={'target':layout(t)}))
    assert len(found)==1
    return found[0]


def target_geometry(debug):
    """Read the native card-instance target-array and target_t member offsets."""
    dw=debug.dwarf();found=[]
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!='cards/throne_of_eldraine.c':continue
        for die in cu.iter_DIEs():
            if die.tag!='DW_TAG_structure_type' or value(die,'DW_AT_name') not in ['card_instance_struct','card_instance_t']:continue
            members=[m for m in die.iter_children() if m.tag=='DW_TAG_member' and value(m,'DW_AT_name')=='targets']
            if not members:continue
            m=members[0];array=unqualify(inherited_type(m));assert array.tag=='DW_TAG_array_type'
            t=unqualify(inherited_type(array));assert t.tag=='DW_TAG_structure_type'
            fields={value(f,'DW_AT_name'):member_offset(f) for f in t.iter_children() if f.tag=='DW_TAG_member'}
            found.append(dict(targets_offset=member_offset(m),target_bytes=sizeof(t),fields=fields))
    assert found and all(r==found[0] for r in found)
    return found[0]
