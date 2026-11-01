"""Bounded CFG/controller and bit-lane proofs for compiler store transformations."""
import collections
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86 import X86_OP_REG,X86_OP_MEM,X86_OP_IMM,X86_REG_EBP,X86_REG_ESP
from canonicalize_helpers import sha
from inspect_release import value,inherited_value
from shared_stack_test_analysis import FAMILIES,walk
from verify_wide_stack_test_recovery import graph_for,symbol_at
from elftools.dwarf.locationlists import LocationParser,LocationExpr
from elftools.dwarf.dwarf_expr import DWARFExprParser


def decode(view,image):
    lo,hi=int(view['address'],16),int(view['end'],16);md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    ins=list(md.disasm(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo),lo))
    assert sum(i.size for i in ins)==hi-lo
    return md,ins


def controllers(view,image,debug,cfa):
    """Prove the player-valued controller fields at every target consumer."""
    md,ins=decode(view,image);graph=graph_for(ins);by_at={i.address:i for i in ins}
    unknown=frozenset({'unknown'});player=frozenset({'player'});states={ins[0].address:{('ebp',8,4):player}}
    todo=collections.deque(states);symbols={};base=cfa+8
    for i in ins:
        if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM:
            try:symbols[i.address]=symbol_at(image,debug,i.operands[0].imm)
            except AssertionError:pass
    def regkey(reg):
        f=FAMILIES.get(md.reg_name(reg));return ('reg',f[0]) if f else None
    def memkey(op):
        if op.type==X86_OP_MEM and not op.mem.index and op.mem.base in [X86_REG_EBP,X86_REG_ESP]:
            return ('ebp' if op.mem.base==X86_REG_EBP else 'esp',op.mem.disp,op.size)
    def operand(state,op):
        if op.type==X86_OP_REG:return state.get(regkey(op.reg),unknown)
        if op.type==X86_OP_MEM:return state.get(memkey(op),unknown)
        if op.type==X86_OP_IMM:return frozenset({'constant:'+str(op.imm)})
        return unknown
    steps=0
    while todo:
        at=todo.popleft();i=by_at[at];state=dict(states[at]);ops=i.operands
        if i.mnemonic=='call':
            if symbols.get(at)=='_default_target_definition' and state.get(('esp',8,4))==frozenset({'frame:'+str(cfa)}):
                state[('target_initialized',)]=frozenset({'yes'})
                for off in [4,8]:state[('ebp',base+off,4)]=unknown
            for r in ['eax','ecx','edx']:state[('reg',r)]=unknown
            state={k:v for k,v in state.items() if k[0]!='esp'}
        elif i.mnemonic=='mov' and len(ops)==2:
            key=regkey(ops[0].reg) if ops[0].type==X86_OP_REG else memkey(ops[0])
            if key:state[key]=operand(state,ops[1]) if ops[0].size==4 else unknown
        elif i.mnemonic=='lea' and len(ops)==2 and ops[0].type==X86_OP_REG:
            p=ops[1];state[regkey(ops[0].reg)]=frozenset({'frame:'+str(p.mem.disp-8)}) if (
                p.type==X86_OP_MEM and p.mem.base==X86_REG_EBP and not p.mem.index) else unknown
        else:
            for reg in i.regs_access()[1]:
                k=regkey(reg)
                if k:state[k]=unknown
            if ops and ops[0].type==X86_OP_MEM and ops[0].access&2:
                k=memkey(ops[0])
                if k:state[k]=unknown
        for target in graph[at]:
            old=states.get(target)
            merged=state if old is None else {k:old.get(k,unknown)|state.get(k,unknown) for k in old.keys()|state.keys()}
            if old!=merged:
                states[target]=dict(merged);todo.append(target);steps+=1;assert steps<100000
    stores=[]
    for i in ins:
        ops=i.operands
        if i.mnemonic=='mov' and len(ops)==2 and ops[0].type==X86_OP_MEM and ops[0].mem.base==X86_REG_EBP and not ops[0].mem.index and ops[0].mem.disp in [base+4,base+8]:
            assert ops[0].size==4 and operand(states[i.address],ops[1])==player
            stores.append(hex(i.address))
    consumers=[]
    for i in ins:
        name=symbols.get(i.address)
        if name not in ['_targeted_aura','_generic_spell']:continue
        state=states[i.address];arg=12 if name=='_targeted_aura' else 16
        assert state.get(('esp',arg,4))==frozenset({'frame:'+str(cfa)})
        assert state.get(('target_initialized',))==frozenset({'yes'})
        assert state.get(('ebp',base+4,4))==state.get(('ebp',base+8,4))==player
        consumers.append(dict(address=hex(i.address),callee=name,allowed_controller='player',preferred_controller='player'))
    assert {c['callee'] for c in consumers}=={'_generic_spell','_targeted_aura'}
    return dict(kind='controller_values_at_all_consumers',consumers=consumers,player_store_sites=stores,
                method='CFG reaching values with unknown-preserving joins and call clobbers')


def location_expressions(dw,die,name):
    variables=[v for v in walk(die) if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name')==name]
    assert len(variables)==1,(name,len(variables))
    attr=variables[0].attributes.get('DW_AT_location');assert attr
    loc=LocationParser(dw.location_lists()).parse_from_attribute(attr,die.cu['version'],variables[0])
    raw=[loc.loc_expr] if isinstance(loc,LocationExpr) else [x.loc_expr for x in loc if hasattr(x,'loc_expr')]
    return [DWARFExprParser(dw.structs).parse_expr(x) for x in raw]


def packed_choice(view,image,debug,die,dw,cfa,native=False):
    """Prove count16 | player8<<16 | card8<<24 for every input bit."""
    md,ins=decode(view,image);by_at={i.address:i for i in ins};base=cfa+8
    def offsets(name):
        return {o.args[0] for ops in location_expressions(dw,die,name) for o in ops if o.op_name=='DW_OP_fbreg'}
    if native:
        count_offsets=offsets('num_picked');assert len(count_offsets)==1;count=next(iter(count_offsets))
        pair=offsets('last_picked');assert len(pair)==2;player,card=sorted(pair)
        # Both original target pieces are four bytes, in player/card order.
        assert any([(o.op_name,o.args) for o in ops]==[
            ('DW_OP_fbreg',[player]),('DW_OP_piece',[4]),('DW_OP_fbreg',[card]),('DW_OP_piece',[4])]
            for ops in location_expressions(dw,die,'last_picked'))
    else:
        vals=[]
        for name in ['iStack_218','iStack_23c','iStack_22c']:
            found=offsets(name);assert len(found)==1,(name,found);vals.append(next(iter(found)))
        count,player,card=vals
    stores=[(j,i) for j,i in enumerate(ins) if i.mnemonic=='mov' and len(i.operands)==2 and
            i.operands[0].type==X86_OP_MEM and i.operands[0].mem.base==X86_REG_EBP and not i.operands[0].mem.index]
    if native:
        byte2=[j for j,i in stores if i.operands[0].mem.disp==base+2 and i.operands[0].size==1];assert len(byte2)==1
        at=byte2[0];first=max(j for j,i in stores if j<at and i.operands[0].mem.disp==base and i.operands[0].size==4)
        assert ins[first-2].mnemonic=='mov' and ins[first-1].mnemonic=='test'
        start=first-2;branch=first+1;end=at+4
        assert ins[branch].mnemonic=='je' and ins[end].mnemonic=='mov' and ins[end].operands[0].mem.disp==count+8
        # The byte updates, scratch reload and count copy are straight-line.
        assert ins[at+2].operands[0].mem.disp==base+3
    else:
        candidates=[j for j,i in stores if i.operands[0].mem.disp==base and i.operands[0].size==4 and
                    any(x.mnemonic=='shl' and x.op_str.endswith(', 0x18') for x in ins[max(0,j-12):j])]
        assert len(candidates)==1;end=candidates[0]
        branch=max(j for j in range(end) if ins[j].mnemonic=='je');start=branch+1
        assert ins[branch-1].mnemonic=='test'
    test=ins[branch-1] if not native else ins[first-1]
    assert test.op_str=='byte ptr [ebp + 8], 0x40'
    false_target=ins[branch].operands[0].imm
    assert false_target==ins[end].address+ins[end].size
    memory={};registers={}
    for label,offset in [('count',count),('player',player),('card',card)]:
        for bit in range(32):memory[(offset+8)*8+bit]=label+str(bit)
    def getreg(reg):
        family,width,shift=FAMILIES[md.reg_name(reg)];return registers.get(family,['?']*32)[shift:shift+width]
    def putreg(reg,bits):
        family,width,shift=FAMILIES[md.reg_name(reg)];assert len(bits)==width
        old=registers.setdefault(family,['?']*32);old[shift:shift+width]=bits
    def operand(op):
        if op.type==X86_OP_REG:return getreg(op.reg)
        if op.type==X86_OP_IMM:return ['1' if op.imm>>b&1 else '0' for b in range(op.size*8)]
        assert op.type==X86_OP_MEM and op.mem.base==X86_REG_EBP and not op.mem.index
        return [memory.get(op.mem.disp*8+b,'?') for b in range(op.size*8)]
    window=ins[start:end+1]
    for i in window:
        ops=i.operands
        if i.mnemonic in ['test','je']:continue # condition is true on the packing path
        if i.mnemonic=='mov':bits=operand(ops[1])
        elif i.mnemonic=='xor' and ops[0].reg==ops[1].reg:bits=['0']*(ops[0].size*8)
        elif i.mnemonic=='shl':
            shift=ops[1].imm;bits=(['0']*shift+operand(ops[0]))[:ops[0].size*8]
        elif i.mnemonic in ['or','and']:
            left,right=operand(ops[0]),operand(ops[1]);bits=[]
            for a,b in zip(left,right):
                if i.mnemonic=='or':
                    assert a==b or a=='0' or b=='0' or a=='1' or b=='1',(a,b)
                    bits.append(a if b=='0' or a==b else b if a=='0' else '1')
                else:
                    assert a==b or a in ['0','1'] or b in ['0','1'],(a,b)
                    bits.append('0' if '0' in [a,b] else a if b=='1' or a==b else b)
        else:raise AssertionError((i.mnemonic,i.op_str))
        if ops[0].type==X86_OP_REG:putreg(ops[0].reg,bits)
        else:
            assert ops[0].mem.base==X86_REG_EBP and not ops[0].mem.index
            for j,b in enumerate(bits):memory[ops[0].mem.disp*8+j]=b
    expected=['count'+str(i) for i in range(16)]+['player'+str(i) for i in range(8)]+['card'+str(i) for i in range(8)]
    assert [memory.get(base*8+i) for i in range(32)]==expected
    assert [memory.get((count+8)*8+i) for i in range(32)]==expected
    # Native's unconditional temporary write on the flag-off path is dead:
    # no direct reread or taking the scratch address before returning.
    if native:
        graph=graph_for(ins);todo=[false_target];seen=set()
        while todo:
            at=todo.pop()
            if at in seen:continue
            seen.add(at);i=by_at[at]
            for op in i.operands:
                if op.type==X86_OP_MEM and op.mem.base==X86_REG_EBP and not op.mem.index:
                    if op.access&1 or i.mnemonic=='lea':assert not (op.mem.disp<base+4 and base<op.mem.disp+op.size),(hex(at),i.op_str)
            todo.extend(graph[at])
    return dict(kind='packed_choice_bit_lanes',guard=hex(test.address),false_target=hex(false_target),
        input_frame_offsets=dict(count=count,player=player,card=card),
        result_bit_lanes=expected,instruction_window=[[hex(i.address),i.mnemonic,i.op_str] for i in window],
        native_dead_temporary_no_direct_reread=native)
