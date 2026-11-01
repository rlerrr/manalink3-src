"""Inspect affine global-array addresses in complete direct function CFGs."""
import collections
from capstone.x86 import X86_OP_IMM,X86_OP_REG,X86_OP_MEM,X86_REG_EBP
from canonicalize_helpers import sha
from inspect_release import value,inherited_value
from array_role_analysis import snapshots
from shared_stack_test_analysis import FAMILIES
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof,member_offset
from verify_wide_stack_test_recovery import decode,graph_for

INLINE_HELPER='counter_activated_or_triggered_ability_cycling'
GLOBALS={'player_bits':(0x4ef1a4,4,32),'stack_data':(0x509630,2,512)}


def global_types(debug):
    dw=debug.dwarf();result={}
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!='functions/functions.c':continue
        for d in cu.get_top_DIE().iter_children():
            name=inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_variable' or name not in GLOBALS:continue
            assert name not in result
            t=unqualify(inherited_type(d));assert t.tag=='DW_TAG_array_type'
            bounds=[b for b in t.iter_children() if b.tag=='DW_TAG_subrange_type'];assert len(bounds)==1
            b=bounds[0];upper=b.attributes['DW_AT_upper_bound']
            assert upper.form in ['DW_FORM_data1','DW_FORM_data2','DW_FORM_data4','DW_FORM_udata']
            assert value(b,'DW_AT_lower_bound',0)==0
            element=unqualify(inherited_type(t));count=upper.value+1
            row=dict(count=count,element_bytes=sizeof(element),bytes=sizeof(t))
            if name=='stack_data':
                assert element.tag=='DW_TAG_structure_type'
                field=next(f for f in element.iter_children() if f.tag=='DW_TAG_member' and value(f,'DW_AT_name')=='internal_card_id')
                ft=unqualify(inherited_type(field));row['internal_card_id']=dict(offset=member_offset(field),bytes=sizeof(ft),encoding=value(ft,'DW_AT_encoding'))
            symbols=[s for s in debug.symbols if s[0]=='_'+name]
            assert len(symbols)==1 and symbols[0][1]==GLOBALS[name][0] and symbols[0][2]==-1
            row['address']=hex(symbols[0][1]);result[name]=row
    assert set(result)==set(GLOBALS)
    return result


def addresses(image,view):
    """Exact affine expressions relative to opaque loaded integer values.

    Root identities describe one load/call result, not a proven source-level
    selector. Unsupported operations become unknown; calls kill caller-save
    registers and tracked frame spills. CFG joins retain identical values or
    factor identical coefficients/biases over a fresh piecewise selector;
    this does not assert that the alternative selector values are equal.
    This measures local address scales and fixed biases, not whole-function
    equivalence or arbitrary pointer aliases.
    """
    md,ins=decode(view,image);graph=graph_for(ins);by_at={i.address:i for i in ins}
    def family(reg):
        f=FAMILIES.get(md.reg_name(reg))
        return f[0] if f and f[1:]==(32,0) else None
    def const(n):return (None,0,n)
    def add(a,b,k=1):
        if a is None or b is None:return None
        if a[0] is not None and b[0] is not None and a[0]!=b[0]:return None
        return (a[0] or b[0],a[1]+b[1]*k,a[2]+b[2]*k)
    def scale(a,n):return (a[0],a[1]*n,a[2]*n) if a is not None else None
    def framekey(op):
        return ('frame',op.mem.disp,op.size) if op.type==X86_OP_MEM and op.mem.base==X86_REG_EBP and not op.mem.index else None
    def operand(state,op,at):
        if op.type==X86_OP_IMM:return const(op.imm)
        if op.type==X86_OP_REG:return state.get(('reg',family(op.reg)))
        if op.type==X86_OP_MEM:
            key=framekey(op)
            if key and state.get(key) is not None:return state[key]
            return ('load:'+hex(at),1,0)
    def address(state,op):
        if op.type!=X86_OP_MEM:return None
        m=op.mem
        base=state.get(('reg',family(m.base))) if m.base else const(0)
        index=state.get(('reg',family(m.index))) if m.index else const(0)
        return add(add(base,scale(index,m.scale)),const(m.disp))
    def step(i,initial):
        state=dict(initial);ops=i.operands
        for r in i.regs_access()[1]:
            f=FAMILIES.get(md.reg_name(r))
            if f:state['reg',f[0]]=None
        for op in ops:
            if op.type!=X86_OP_MEM or not op.access&2:continue
            key=framekey(op)
            for k in list(state):
                if k[0]!='reg' and (key is None or max(k[1],key[1])<min(k[1]+k[2],key[1]+key[2])):state[k]=None
        if i.mnemonic=='call':
            state={k:v for k,v in state.items() if k[0]=='reg'}
            for r in ['eax','ecx','edx']:state['reg',r]=None
            state['reg','eax']=('call:'+hex(i.address),1,0)
        elif len(ops)==2:
            dest=family(ops[0].reg) if ops[0].type==X86_OP_REG else None
            if i.mnemonic=='mov' and ops[0].type==X86_OP_MEM:
                key=framekey(ops[0])
                if key:state[key]=operand(initial,ops[1],i.address)
            if dest:
                a=operand(initial,ops[0],i.address);b=operand(initial,ops[1],i.address)
                if i.mnemonic=='mov':state['reg',dest]=b
                elif i.mnemonic in ['movsx','movzx'] and ops[1].type==X86_OP_MEM:state['reg',dest]=('load:'+hex(i.address),1,0)
                elif i.mnemonic=='lea':state['reg',dest]=address(initial,ops[1])
                elif i.mnemonic in ['add','sub']:state['reg',dest]=add(a,b,1 if i.mnemonic=='add' else -1)
                elif i.mnemonic in ['shl','sal'] and b is not None and b[0] is None and 0<=b[2]<32:state['reg',dest]=scale(a,1<<b[2])
                elif i.mnemonic=='xor' and ops[1].type==X86_OP_REG and ops[0].reg==ops[1].reg:state['reg',dest]=const(0)
        elif len(ops)==1 and ops[0].type==X86_OP_REG and family(ops[0].reg):
            dest=family(ops[0].reg);a=operand(initial,ops[0],i.address)
            if i.mnemonic=='neg':state['reg',dest]=scale(a,-1)
            elif i.mnemonic in ['inc','dec']:state['reg',dest]=add(a,const(1 if i.mnemonic=='inc' else -1))
        return state
    states={ins[0].address:{}};todo=collections.deque([ins[0].address]);updates=0
    while todo:
        at=todo.popleft();after=step(by_at[at],states[at])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(after);todo.append(dest);continue
            old=states[dest];merged={}
            for k in set(old)|set(after):
                a,b=old.get(k),after.get(k)
                merged[k]=(a if a==b else ('phi:'+hex(dest)+':'+repr(k),a[1],a[2])
                    if a is not None and b is not None and a[0] is not None and b[0] is not None and a[1:]==b[1:] else None)
            if merged!=old:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    result=[]
    for at,state in states.items():
        i=by_at[at]
        if i.mnemonic=='lea':continue
        for op in i.operands:
            if op.type!=X86_OP_MEM:continue
            a=address(state,op)
            if a is None:continue
            for name,(base,width,oldscale) in GLOBALS.items():
                delta=a[2]-base
                if op.size!=width or not -oldscale<=delta<=oldscale:continue
                # Exclude neighboring globals, including hand_count at bits+32.
                if a[0] is None and not 0<=delta<(8 if name=='player_bits' else 128):continue
                if a[0] is not None and abs(a[1]) not in [4,oldscale]:continue
                if a[0] is not None and delta%abs(a[1]):continue
                if name=='player_bits' and a[0] is not None and delta not in [0,abs(a[1])]:continue
                result.append(dict(address=hex(at),global_name=name,selector_root=a[0],scale=a[1],bias_bytes=delta,
                    width=op.size,access=op.access,instruction=[i.mnemonic,i.op_str]))
    return sorted(result,key=lambda r:(int(r['address'],16),r['global_name']))


def inspect(image,debug,rows):
    owners=[r for r in rows if r['name']!=INLINE_HELPER]
    result=snapshots(image,debug,owners,with_effects=False)
    for row in result.values():row['global_addresses']=addresses(image,row['view'])
    return result


def flag_effects(image,snapshot):
    """Prove matching RMW masks and mutual exclusion of duplicated blocks."""
    md,ins=decode(snapshot['view'],image);graph=graph_for(ins);facts={int(f['address'],16):f for f in snapshot['global_addresses']}
    def family(reg):return FAMILIES.get(md.reg_name(reg))
    def defines(i,r):return any(family(w) and family(w)[0]==r for w in i.regs_access()[1])
    effects=[];covered=set()
    for index,store in enumerate(ins):
        f=facts.get(store.address)
        if not f or not f['access']&2:continue
        assert store.mnemonic=='mov' and store.operands[1].type==X86_OP_REG
        reg=family(store.operands[1].reg);assert reg and reg[1:]==(32,0)
        mask_index=load_index=None
        for k in range(index-1,max(-1,index-40),-1):
            i=ins[k]
            assert not (i.mnemonic.startswith('j') or i.mnemonic in ['call','ret']),hex(store.address)
            if not defines(i,reg[0]):continue
            if mask_index is None:
                assert i.mnemonic in ['or','and'] and i.operands[0].type==X86_OP_REG and i.operands[1].type==X86_OP_IMM
                assert family(i.operands[0].reg)[0]==reg[0];mask_index=k
            else:
                assert i.mnemonic=='mov' and i.operands[0].type==X86_OP_REG and family(i.operands[0].reg)==reg and i.operands[1].type==X86_OP_MEM
                load_index=k;break
        assert mask_index is not None and load_index is not None
        load,mask=ins[load_index],ins[mask_index];g=facts[load.address]
        assert g['access']==1 and f['access']==2
        assert all(f[k]==g[k] for k in ['global_name','selector_root','scale','bias_bytes','width'])
        assert f['global_name']=='player_bits' and f['width']==4
        # The interval has no call/branch or intervening write to this array.
        for i in ins[load_index+1:index]:
            assert i.address not in facts or not facts[i.address]['access']&2
            for op in i.operands:
                if op.type==X86_OP_MEM and op.access&2:
                    assert op.mem.base==X86_REG_EBP or not op.mem.base and not op.mem.index and not 0x4ef1a4<=op.mem.disp<0x4ef1ac
        _,bits,shift=family(mask.operands[0].reg);lane=((1<<bits)-1)<<shift
        literal=(mask.operands[1].imm&((1<<bits)-1))<<shift
        value=literal if mask.mnemonic=='or' else literal|(~lane&0xffffffff)
        assert (mask.mnemonic,value) in [('or',8),('and',0xfffffff7)]
        effects.append(dict(load=hex(load.address),operation=hex(mask.address),store=hex(store.address),
            global_name=f['global_name'],scale=f['scale'],bias=f['bias_bytes'],width=4,kind=mask.mnemonic,mask=value))
        covered.update([load.address,store.address])
    assert covered==set(facts)
    groups=collections.defaultdict(list)
    for e in effects:groups[(e['global_name'],e['scale'],e['bias'],e['width'],e['kind'],e['mask'])].append(e)
    def reachable(start,end):
        todo=list(graph[start]);seen=set()
        while todo:
            at=todo.pop()
            if at==end:return True
            if at not in seen:seen.add(at);todo.extend(graph[at])
        return False
    exclusions=[]
    for group in groups.values():
        for e in group:assert not reachable(int(e['store'],16),int(e['store'],16))
        for j,a in enumerate(group):
            for b in group[j+1:]:
                assert not reachable(int(a['store'],16),int(b['store'],16)) and not reachable(int(b['store'],16),int(a['store'],16))
                exclusions.append([a['store'],b['store']])
    return dict(effects=effects,mutually_unreachable_identical_store_pairs=exclusions,
        effect_classes=[list(k) for k in sorted(groups)])
