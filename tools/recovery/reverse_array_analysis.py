"""Inspect fixed arrays and their definite row-to-consumer value flow."""
import collections
from capstone.x86 import X86_OP_REG,X86_OP_MEM,X86_OP_IMM,X86_REG_EBP,X86_REG_ESP
from array_role_analysis import snapshots
from shared_stack_test_analysis import FAMILIES,walk
from inspect_release import inherited_value,value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof,member_offset
from verify_wide_stack_test_recovery import decode,graph_for

ARRAYS={'pias_revolution_effect':('dead_array',[20],8,None),
    'card_pias_revolution':('dead_array',[22],8,None),
    'card_out_of_time':('marked',[151,2],4,5)}
CONSUMERS={'_find_in_owners_graveyard':(0,1),'_untap_card':(0,1),
    '_phase_out_target_until_source_leaves_play':(2,3)}


def row_flow(image,debug,snapshot,array):
    """Follow a modulo-eight row pointer and its two four-byte columns.

    The row index is abstracted. This proves fixed-array origin, field identity
    and consumer operands, not bounds, array contents or arbitrary aliases.
    Unsupported operations become unknown. Calls kill caller-save registers
    and all tracked argument/frame memory. Joins retain identical tags only.
    """
    md,ins=decode(snapshot['view'],image);graph=graph_for(ins);by_at={i.address:i for i in ins}
    targets={}
    for name,v,section,kind,storage in debug.symbols:
        if name not in CONSUMERS:continue
        at=v if section==-1 else image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[section-1].VirtualAddress+v
        targets[at]=name
    assert set(targets.values())==set(CONSUMERS)
    def family(reg):
        f=FAMILIES.get(md.reg_name(reg))
        return f[0] if f and f[1:]==(32,0) else None
    def key(op):
        if op.type!=X86_OP_MEM or op.mem.index:return None
        if op.mem.base==X86_REG_ESP:return ('arg',op.mem.disp,op.size)
        if op.mem.base==X86_REG_EBP:return ('frame',op.mem.disp,op.size)
    def address(state,op):
        if op.type!=X86_OP_MEM:return None
        m=op.mem
        if m.base==X86_REG_EBP:
            relative=m.disp-8-array['frame_offset']
            if m.index and m.scale==8 and -8<=relative<=4:return ('row',relative%8)
            if not m.index and relative in [0,4]:return ('row',relative)
        base=state.get(('reg',family(m.base)))
        if base and base[0]=='row' and not m.index:return ('row',(base[1]+m.disp)%8)
        return None
    def operand(state,op,site):
        if op.type==X86_OP_REG:return state.get(('reg',family(op.reg)))
        if op.type==X86_OP_IMM:return ('const',op.imm)
        if op.type==X86_OP_MEM:
            k=key(op)
            if k and k in state:return state[k]
            a=address(state,op)
            if a and op.size==4 and a[1] in [0,4]:return ('column',a[1],hex(site))
        return None
    def step(i,initial):
        state=dict(initial);ops=i.operands;call=None;origins=[];steps=[]
        for r in i.regs_access()[1]:
            f=FAMILIES.get(md.reg_name(r))
            if f:state['reg',f[0]]=None
        for op in ops:
            if op.type==X86_OP_MEM and op.access&2:
                k=key(op)
                for old in list(state):
                    if old[0]!='reg' and (k is None or old[0]==k[0] and max(old[1],k[1])<min(old[1]+old[2],k[1]+k[2])):state[old]=None
        if i.mnemonic=='call':
            target=targets.get(ops[0].imm) if ops[0].type==X86_OP_IMM else None
            if target:call=dict(address=hex(i.address),name=target,arguments=[list(initial.get(('arg',j*4,4))) if initial.get(('arg',j*4,4)) is not None else None for j in range(4)])
            state={k:v for k,v in state.items() if k[0]=='reg'}
            for r in ['eax','ecx','edx']:state['reg',r]=None
        elif i.mnemonic=='mov' and len(ops)==2:
            val=operand(initial,ops[1],i.address)
            if ops[0].type==X86_OP_REG and family(ops[0].reg):state['reg',family(ops[0].reg)]=val
            elif ops[0].type==X86_OP_MEM:
                k=key(ops[0])
                if k:state[k]=val
        elif i.mnemonic=='lea' and len(ops)==2 and family(ops[0].reg):
            val=address(initial,ops[1]);state['reg',family(ops[0].reg)]=val
            if val and ops[1].mem.base==X86_REG_EBP and ops[1].mem.index:
                origins.append(dict(address=hex(i.address),row_bytes=ops[1].mem.scale,relative_frame_bias=ops[1].mem.disp-8-array['frame_offset']))
            if val and ops[1].mem.base!=X86_REG_EBP and not ops[1].mem.index and ops[1].mem.disp==-8:
                steps.append(dict(address=hex(i.address),bytes=-8))
        elif i.mnemonic in ['add','sub'] and len(ops)==2 and ops[0].type==X86_OP_REG and family(ops[0].reg):
            a,b=operand(initial,ops[0],i.address),operand(initial,ops[1],i.address)
            if a and a[0]=='row' and b and b[0]=='const':
                delta=b[1]*(1 if i.mnemonic=='add' else -1)
                state['reg',family(ops[0].reg)]=('row',(a[1]+delta)%8)
                if delta==-8:steps.append(dict(address=hex(i.address),bytes=-8))
        return state,call,origins,steps
    states={ins[0].address:{}};todo=collections.deque([ins[0].address]);updates=0
    while todo:
        at=todo.popleft();after,*_=step(by_at[at],states[at])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(after);todo.append(dest);continue
            old=states[dest];merged={k:old.get(k) if old.get(k)==after.get(k) else None for k in set(old)|set(after)}
            if merged!=old:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    calls=[];origins=[];steps=[]
    for at,state in states.items():
        _,call,origin,step_rows=step(by_at[at],state)
        if call:calls.append(call)
        origins.extend(origin);steps.extend(step_rows)
    pairs=[];positions={i.address:j for j,i in enumerate(ins)}
    for call in calls:
        p,c=CONSUMERS[call['name']];a,b=call['arguments'][p],call['arguments'][c]
        assert a and b and a[:2]==['column',0] and b[:2]==['column',4],call
        def source(tag):
            i=by_at[int(tag[2],16)];assert i.mnemonic=='mov' and i.operands[1].type==X86_OP_MEM
            m=i.operands[1].mem
            return dict(address=tag[2],base=family(m.base),index=family(m.index) if m.index else None,
                scale=m.scale if m.index else 1,displacement=m.disp-tag[1])
        left,right=source(a),source(b)
        assert all(left[k]==right[k] for k in ['base','index','scale','displacement']),(call,left,right)
        lo,hi=sorted([positions[int(a[2],16)],positions[int(b[2],16)]])
        pointer_regs={left['base'],left['index']}-{None}
        for i in ins[lo:hi]:
            assert not (i.mnemonic.startswith('j') or i.mnemonic in ['call','ret'])
            assert not pointer_regs&{family(r) for r in i.regs_access()[1]}
        pairs.append(dict(consumer=call['address'],player_load=a[2],card_load=b[2],
            same_row_operand={k:left[k] for k in ['base','index','scale','displacement']}))
    return dict(consumer_calls=sorted(calls,key=lambda r:int(r['address'],16)),same_row_pairs=pairs,
        indexed_row_origins=sorted(origins,key=lambda r:int(r['address'],16)),
        reverse_row_steps=sorted(steps,key=lambda r:int(r['address'],16)))


def inspect(image,debug,rows):
    result=snapshots(image,debug,rows,with_effects=False);dw=debug.dwarf()
    for (source,name),snapshot in result.items():
        array_name=ARRAYS[name][0];arrays=[v for v in snapshot['variables'] if v['name']==array_name]
        assert len(arrays)<=1
        d=dw.get_DIE_from_refaddr(int(snapshot['die_offset'],16))
        declared=[v for v in walk(d) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')==array_name]
        assert len(declared)==1
        t=unqualify(inherited_type(declared[0]));assert t.tag=='DW_TAG_array_type'
        snapshot['array_declaration']=dict(name=array_name,bytes=sizeof(t),has_emitted_location=value(declared[0],'DW_AT_location') is not None)
        element=t
        while element.tag=='DW_TAG_array_type':element=unqualify(inherited_type(element))
        layout=dict(kind=element.tag,size=sizeof(element),encoding=value(element,'DW_AT_encoding'))
        if element.tag=='DW_TAG_structure_type':
            layout['fields']=[dict(name=value(f,'DW_AT_name'),offset=member_offset(f),size=sizeof(inherited_type(f)),encoding=value(unqualify(inherited_type(f)),'DW_AT_encoding'))
                for f in element.iter_children() if f.tag=='DW_TAG_member']
        snapshot['element_layout']=layout
        snapshot['row_flow']=row_flow(image,debug,snapshot,arrays[0]) if arrays else None
    return result
