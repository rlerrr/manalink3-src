"""Inspect native ability-name storage and selected-IID address arithmetic."""
import collections
from capstone.x86 import X86_OP_REG,X86_OP_IMM,X86_OP_MEM,X86_REG_EBP
from array_role_analysis import snapshots,effects
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof
from shared_stack_test_analysis import walk,FAMILIES
from verify_wide_stack_test_recovery import decode,graph_for


def selected_iid_reads(image,snapshot):
    """Follow affine expressions of dialog EAX through the direct CFG.

    Memory loads are unknown, calls kill caller-save registers, and joins
    retain identical expressions only. This proves an address relative to
    the dialog result; it does not prove the validity of that result.
    """
    calls=[c for c in snapshot['effects']['calls'] if c['name']=='_dialog_impl']
    assert len(calls)==1
    call=int(calls[0]['address'],16)
    md,ins=decode(snapshot['view'],image);graph=graph_for(ins);by_at={i.address:i for i in ins}
    start=next(i.address+i.size for i in ins if i.address==call)
    def family(reg):
        f=FAMILIES.get(md.reg_name(reg))
        return f[0] if f and f[1:]==(32,0) else None
    def op(state,operand):
        if operand.type==X86_OP_IMM:return (0,operand.imm)
        if operand.type==X86_OP_REG:return state.get(family(operand.reg))
        return None
    def add(a,b,scale=1):
        return (a[0]+b[0]*scale,a[1]+b[1]*scale) if a is not None and b is not None else None
    def step(i,initial):
        state=dict(initial);ops=i.operands
        for r in i.regs_access()[1]:
            if family(r):state[family(r)]=None
        if i.mnemonic=='call':
            for r in ['eax','ecx','edx']:state[r]=None
        elif len(ops)==2 and ops[0].type==X86_OP_REG:
            dest=family(ops[0].reg)
            if dest:
                if i.mnemonic=='mov':state[dest]=op(initial,ops[1])
                elif i.mnemonic in ['add','sub']:state[dest]=add(op(initial,ops[0]),op(initial,ops[1]),1 if i.mnemonic=='add' else -1)
                elif i.mnemonic=='lea' and ops[1].type==X86_OP_MEM:
                    m=ops[1].mem
                    base=initial.get(family(m.base)) if m.base else (0,0)
                    index=initial.get(family(m.index)) if m.index else (0,0)
                    state[dest]=add(add(base,index,m.scale),(0,m.disp))
        elif len(ops)==1 and ops[0].type==X86_OP_REG and i.mnemonic in ['inc','dec']:
            state[family(ops[0].reg)]=add(op(initial,ops[0]),(0,1 if i.mnemonic=='inc' else -1))
        return state
    states={start:{'eax':(1,0)}};todo=collections.deque([start]);updates=0
    while todo:
        at=todo.popleft();after=step(by_at[at],states[at])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(after);todo.append(dest);continue
            old=states[dest];merged={k:old.get(k) if old.get(k)==after.get(k) else None for k in set(old)|set(after)}
            if merged!=old:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    result=[]
    for at,state in states.items():
        for operand in by_at[at].operands:
            if operand.type!=X86_OP_MEM or operand.size!=2 or not operand.access&1:continue
            m=operand.mem
            if m.base!=X86_REG_EBP or not m.index:continue
            index=state.get(family(m.index))
            if index is None or index[0]!=1:continue
            result.append(dict(address=hex(at),extension=by_at[at].mnemonic,choice_coefficient=m.scale,
                choice_zero_frame_offset=m.disp-8+m.scale*index[1],bytes=2))
    return sorted(result,key=lambda r:int(r['address'],16))


def inspect(image,debug,rows):
    result=snapshots(image,debug,rows,with_effects=False)
    dw=debug.dwarf()
    for row in result.values():
        row['effects']=effects(image,debug,row['view'],include_frame_copies=True,call_argument_words=56)
        die=dw.get_DIE_from_refaddr(int(row['die_offset'],16));scalars=[]
        for v in walk(die):
            if v.tag!='DW_TAG_variable':continue
            t=unqualify(inherited_type(v))
            if t is None or t.tag!='DW_TAG_base_type':continue
            scalars.append(dict(name=inherited_value(v,'DW_AT_name'),size=sizeof(t),encoding=value(t,'DW_AT_encoding')))
        row['scalar_types']=scalars
        if inherited_value(die,'DW_AT_name')=='card_dark_impostor':row['selected_iid_reads']=selected_iid_reads(image,row)
    return result
