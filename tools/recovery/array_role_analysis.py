"""Native/rebuilt array geometry and conservative constant frame effects."""
import collections
from capstone.x86 import X86_OP_MEM,X86_OP_REG,X86_OP_IMM,X86_REG_EBP,X86_REG_ESP
from canonicalize_helpers import sha
from inspect_release import value,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof,member_offset
from shared_stack_test_analysis import walk,frame_offset,FAMILIES
from verify_inline_helpers import ranges
from verify_wide_stack_test_recovery import decode,graph_for


def shape(t):
    t=unqualify(t);result=dict(kind=t.tag,size=sizeof(t))
    if t.tag=='DW_TAG_array_type':
        dims=[]
        while t.tag=='DW_TAG_array_type':
            for bound in t.iter_children():
                if bound.tag=='DW_TAG_subrange_type':
                    lower=value(bound,'DW_AT_lower_bound',0);upper=value(bound,'DW_AT_upper_bound')
                    assert isinstance(lower,int) and isinstance(upper,int)
                    dims.append(upper-lower+1)
            t=unqualify(inherited_type(t))
        result.update(dimensions=dims,element_size=sizeof(t),element_encoding=value(t,'DW_AT_encoding'))
    elif t.tag=='DW_TAG_structure_type' and sizeof(t)==216:
        field=next(m for m in t.iter_children() if m.tag=='DW_TAG_member' and value(m,'DW_AT_name')=='message')
        assert member_offset(field)==0 and sizeof(inherited_type(field))==100
        result['message']=dict(offset=0,size=100)
    return result


def snapshots(image,debug,rows,*,with_effects=True):
    dw=debug.dwarf();wanted={(r['source'],r['name']) for r in rows};result={}
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if source not in {s for s,n in wanted}:continue
        for die in cu.get_top_DIE().iter_children():
            key=(source,inherited_value(die,'DW_AT_name'))
            if die.tag!='DW_TAG_subprogram' or key not in wanted or not ranges(dw,die):continue
            assert key not in result and value(die,'DW_AT_frame_base')==[156]
            variables=[]
            for v in walk(die):
                if v.tag!='DW_TAG_variable':continue
                t=unqualify(inherited_type(v))
                if t is None or t.tag not in ['DW_TAG_array_type','DW_TAG_structure_type']:continue
                try:offset=frame_offset(dw,v);layout=shape(t)
                except (AssertionError,ValueError,KeyError,StopIteration):continue
                scope=v.get_parent()
                while not ranges(dw,scope):scope=scope.get_parent()
                variables.append(dict(name=inherited_value(v,'DW_AT_name'),die_offset=hex(v.offset),frame_offset=offset,
                    shape=layout,scope_ranges=[[a,b] for a,b in ranges(dw,scope)]))
            parts=ranges(dw,die);assert len(parts)==1;lo,hi=parts[0]
            view=dict(address=hex(lo),end=hex(hi),bytes_sha256=sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo)))
            result[key]=dict(die_offset=hex(die.offset),view=view,variables=variables)
            if with_effects:result[key]['effects']=effects(image,debug,view)
        cu._dielist.clear();cu._diemap.clear()
    assert set(result)==wanted,wanted-set(result)
    return result


def effects(image,debug,view,*,include_frame_copies=False,call_argument_words=8):
    """Track definite constants/frame pointers through the complete direct CFG.

    Unsupported register definitions become unknown; calls kill caller-save
    registers and tracked memory. Joins retain only identical values. These
    effects prove fixed clears/copies and pointer operands, not arbitrary
    indexed data values or whole-function equivalence.
    """
    md,ins=decode(view,image);graph=graph_for(ins);by_at={i.address:i for i in ins}
    symbols=collections.defaultdict(set)
    for n,v,s,k,c in debug.symbols:
        if 0<s<=len(image.pe.sections):symbols[image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[s-1].VirtualAddress+v].add(n)
    def const(n,mask=0xffffffff):return ('const',mask,n&mask)
    def number(v):return v[2] if v and v[0]=='const' and v[1]==0xffffffff else None
    def family(reg):return FAMILIES.get(md.reg_name(reg))
    def reg(state,reg):
        f=family(reg)
        if not f:return None
        v=state.get(('reg',f[0]))
        if v and v[0]=='const':
            mask=((1<<f[1])-1)<<f[2]
            return const(v[2]>>f[2]&((1<<f[1])-1)) if v[1]&mask==mask else None
        return v if f[1:]==(32,0) else None
    def put(state,register,val):
        f=family(register)
        if not f:return
        key=('reg',f[0])
        if f[1:]==(32,0):state[key]=val;return
        old=state.get(key);old=old if old and old[0]=='const' else const(0,0)
        mask=((1<<f[1])-1)<<f[2];n=number(val)
        state[key]=const((old[2]&~mask)|((n or 0)<<f[2]&mask),(old[1]&~mask)|(mask if n is not None else 0))
    def address(state,op,partial=False):
        if op.type!=X86_OP_MEM:return None
        m=op.mem;base=('frame',0) if m.base==X86_REG_EBP else reg(state,m.base) if m.base else const(0)
        index=number(reg(state,m.index)) if m.index else 0
        if index is None and not partial:return None
        offset=m.disp+(index or 0)*m.scale
        if base and base[0]=='frame':return ('frame',base[1]+offset)
        n=number(base)
        return const(n+offset) if n is not None else None
    def memkey(state,op):
        if op.type!=X86_OP_MEM:return None
        if op.mem.base==X86_REG_ESP and not op.mem.index:return ('esp',op.mem.disp,op.size)
        a=address(state,op)
        return ('frame',a[1],op.size) if a and a[0]=='frame' else None
    def readonly(at,size):
        base=image.pe.OPTIONAL_HEADER.ImageBase
        if not base<=at<base+image.pe.OPTIONAL_HEADER.SizeOfImage:return None
        section=image.pe.get_section_by_rva(at-base)
        if section is None or section.Characteristics&0x80000000:return None
        data=image.pe.get_data(at-base,size)
        return data if len(data)==size else None
    def operand(state,op):
        if op.type==X86_OP_IMM:return const(op.imm)
        if op.type==X86_OP_REG:return reg(state,op.reg)
        if op.type==X86_OP_MEM:
            key=memkey(state,op)
            if key:return state.get(key)
            at=number(address(state,op))
            data=readonly(at,op.size) if at is not None else None
            return const(int.from_bytes(data,'little')) if data is not None else None
    def callee(i):
        op=i.operands[0];at=op.imm if op.type==X86_OP_IMM else op.mem.disp if op.type==X86_OP_MEM and not op.mem.base and not op.mem.index else None
        names=symbols.get(at,set())
        for expected in ['memset','strcpy','select_card_from_zone','new_make_test_in_play']:
            if any(n.lstrip('_') in [expected,'imp__'+expected,'imp_'+expected] for n in names):return '_'+expected
        return next(iter(names)) if len(names)==1 else None
    def step(i,initial):
        state=dict(initial);ops=i.operands;events=[];pointers=[];call=None;copies=[]
        def write(dest,size,val):
            if dest and dest[0]=='frame':
                n=number(val)
                if n is not None:events.append(dict(address=hex(i.address),frame_offset=dest[1]-8,size=size,bytes=(n&((1<<(8*size))-1)).to_bytes(size,'little').hex()))
                elif val and val[0]=='frame' and size==4:pointers.append(dict(address=hex(i.address),frame_offset=dest[1]-8,target_frame_offset=val[1]-8,size=4))
        if i.mnemonic=='call':
            name=callee(i);args=[initial.get(('esp',j,4)) for j in range(0,4*call_argument_words,4)];call=dict(address=hex(i.address),name=name,arguments=[list(a) if a is not None else None for a in args])
            state={k:v for k,v in state.items() if k[0]=='reg'}
            for r in ['eax','ecx','edx']:state['reg',r]=None
            if name=='_memset' and args[0] and args[0][0]=='frame' and number(args[1])==0 and number(args[2]) is not None:
                n=number(args[2]);assert n<100000;events.append(dict(address=hex(i.address),frame_offset=args[0][1]-8,size=n,bytes='00'*n));state['reg','eax']=args[0]
            if name=='_strcpy' and args[0] and args[0][0]=='frame' and number(args[1]) is not None:
                data=readonly(number(args[1]),256)
                if data is not None and b'\0' in data:
                    data=data[:data.index(0)+1];events.append(dict(address=hex(i.address),frame_offset=args[0][1]-8,size=len(data),bytes=data.hex()));state['reg','eax']=args[0]
        elif i.mnemonic in ['rep stosd','rep movsd']:
            count=number(initial.get(('reg','ecx')));dest=initial.get(('reg','edi'));source=initial.get(('reg','esi'))
            assert count is None or count<100000
            n=4*count if count is not None else None
            payload=(b'\0'*n if i.mnemonic=='rep stosd' and n is not None and number(initial.get(('reg','eax')))==0 else
                readonly(number(source),n) if i.mnemonic=='rep movsd' and number(source) is not None and n is not None else None)
            if dest and dest[0]=='frame' and payload is not None:events.append(dict(address=hex(i.address),frame_offset=dest[1]-8,size=len(payload),bytes=payload.hex()))
            if include_frame_copies and i.mnemonic=='rep movsd' and dest and dest[0]=='frame' and source and source[0]=='frame' and n is not None:
                copies.append(dict(address=hex(i.address),source_frame_offset=source[1]-8,destination_frame_offset=dest[1]-8,size=n))
            state={k:v for k,v in state.items() if k[0]=='reg'}
            state['reg','edi']=('frame',dest[1]+n) if dest and dest[0]=='frame' and n is not None else None
            if i.mnemonic=='rep movsd':state['reg','esi']=const(number(source)+n) if number(source) is not None and n is not None else None
            if include_frame_copies and i.mnemonic=='rep movsd' and source and source[0]=='frame' and n is not None:state['reg','esi']=('frame',source[1]+n)
            state['reg','ecx']=const(0)
        else:
            for r in i.regs_access()[1]:
                f=family(r)
                if f:state['reg',f[0]]=None
            for op in ops:
                if op.type==X86_OP_MEM and op.access&2:
                    key=memkey(initial,op)
                    for k in list(state):
                        if k[0]!='reg' and (key is None or k[0]==key[0] and max(k[1],key[1])<min(k[1]+k[2],key[1]+key[2])):state[k]=None
            if i.mnemonic=='mov' and len(ops)==2:
                val=operand(initial,ops[1])
                if ops[0].type==X86_OP_REG:
                    f=family(ops[0].reg)
                    if f:state['reg',f[0]]=initial.get(('reg',f[0]));put(state,ops[0].reg,val)
                elif ops[0].type==X86_OP_MEM:
                    write(address(initial,ops[0]),ops[0].size,val);key=memkey(initial,ops[0])
                    if key:state[key]=val
            elif i.mnemonic=='lea' and len(ops)==2:put(state,ops[0].reg,address(initial,ops[1]))
            elif i.mnemonic=='xor' and len(ops)==2 and ops[0].type==X86_OP_REG and ops[1].type==X86_OP_REG and ops[0].reg==ops[1].reg:put(state,ops[0].reg,const(0))
            elif i.mnemonic=='movzx' and len(ops)==2:put(state,ops[0].reg,operand(initial,ops[1]))
            elif i.mnemonic in ['add','sub'] and len(ops)==2 and ops[0].type==X86_OP_REG:
                a=operand(initial,ops[0]);n=number(operand(initial,ops[1]));n=n if i.mnemonic=='add' or n is None else -n
                if a and a[0]=='frame' and n is not None:put(state,ops[0].reg,('frame',a[1]+n))
                elif number(a) is not None and n is not None:put(state,ops[0].reg,const(number(a)+n))
        return state,events,pointers,call,copies
    states={ins[0].address:{}};todo=collections.deque([ins[0].address]);updates=0
    while todo:
        at=todo.popleft();state,*_=step(by_at[at],states[at])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(state);todo.append(dest);continue
            old=states[dest];merged={k:old.get(k) if old.get(k)==state.get(k) else None for k in set(old)|set(state)}
            if merged!=old:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    writes=[];pointers=[];calls=[];indexed=[];copies=[]
    for at,state in states.items():
        i=by_at[at];_,w,p,c,cp=step(i,state);writes.extend(w);pointers.extend(p);copies.extend(cp)
        if c:calls.append(c)
        for op in i.operands:
            if op.type==X86_OP_MEM and op.mem.index:
                a=address(state,op,partial=True)
                if a and a[0]=='frame':indexed.append(dict(address=hex(at),frame_offset=a[1]-8,scale=op.mem.scale,size=op.size,access=op.access))
    result=dict(writes=sorted(writes,key=lambda r:int(r['address'],16)),frame_pointer_stores=sorted(pointers,key=lambda r:int(r['address'],16)),
        calls=sorted(calls,key=lambda r:int(r['address'],16)),indexed_accesses=sorted(indexed,key=lambda r:int(r['address'],16)),visited_instructions=len(states))
    if include_frame_copies:result['frame_copies']=sorted(copies,key=lambda r:int(r['address'],16))
    return result
