"""Prove the two remaining test-field conditional expressions in emitted code."""
import argparse,collections,json,sys
from pathlib import Path
sys.path.insert(0,'tools/recovery')
from capstone.x86 import X86_OP_IMM,X86_OP_MEM,X86_OP_REG,X86_REG_EBP,X86_REG_ESP
from array_census_analysis import symbol_map
from canonicalize_helpers import read,span,sha
from inspect_release import Image
from verify_wide_stack_test_recovery import decode,graph_for
from shared_stack_test_analysis import FAMILIES as BASE_FAMILIES

LANES=dict(BASE_FAMILIES)
for base,small in [('esi','si'),('edi','di'),('ebp','bp'),('esp','sp')]:LANES[small]=(base,16,0)


def bits(v,n=32):return tuple(v>>j&1 for j in range(n))
def number(v):return sum(x<<j for j,x in enumerate(v)) if all(x in [0,1] for x in v) else None


def stack_at_calls(image,view):
    md,ins=decode(view,image);graph=graph_for(ins);by={i.address:i for i in ins}
    frame=[i for i in ins[:8] if i.mnemonic=='mov' and i.op_str=='ebp, esp'];assert len(frame)==1
    start=frame[0].address+frame[0].size;states={start:0};todo=collections.deque([start])
    while todo:
        at=todo.popleft();i=by[at];off=states[at];ops=i.operands
        if i.mnemonic=='push':off=None if off is None else off-4
        elif i.mnemonic=='pop':off=None if off is None else off+4
        elif i.mnemonic=='leave':off=4
        elif i.mnemonic=='mov' and i.op_str=='esp, ebp':off=0
        elif ops and ops[0].type==X86_OP_REG and ops[0].reg==X86_REG_ESP and ops[0].access&2:
            if i.mnemonic in ['sub','add'] and ops[1].type==X86_OP_IMM:
                off=None if off is None else off+ops[1].imm*(1 if i.mnemonic=='add' else -1)
            else:off=None
        for nxt in graph[at]:
            new=off if nxt not in states or states[nxt]==off else None
            if nxt not in states or new!=states[nxt]:states[nxt]=new;todo.append(nxt)
    return states


def paths(image,view,start,target_disp,expected,*,esp_offset,protected_start,return_seed=None,memory_seed=None,excluded=None,other_fields=None):
    """Explore every local branch until its first field store; calls/exits fail.

    Symbolic words are only compared with the explicitly excluded constant.
    Unsupported register/flag definitions become unknown. Partial writes keep
    their actual lanes. This is a bounded expression proof, not whole execution.
    """
    md,ins=decode(view,image);graph=graph_for(ins);by={i.address:i for i in ins};unknown=(None,)*32
    regs={'eax':return_seed} if return_seed is not None else {};memory={}
    if memory_seed is not None:
        disp,seed=memory_seed
        for j in range(len(seed)//8):memory[disp+j]=seed[j*8:j*8+8]
    def getreg(state,r):
        f=LANES.get(md.reg_name(r))
        return state.get(f[0],unknown)[f[2]:f[2]+f[1]] if f else unknown
    def putreg(state,r,v):
        f=LANES.get(md.reg_name(r))
        if f:
            old=list(state.get(f[0],unknown));old[f[2]:f[2]+f[1]]=v[:f[1]];state[f[0]]=tuple(old)
    def operand(state,mem,op):
        if op.type==X86_OP_IMM:return bits(op.imm,op.size*8)
        if op.type==X86_OP_REG:return getreg(state,op.reg)
        if op.type==X86_OP_MEM and op.mem.base==X86_REG_EBP and not op.mem.index:
            return tuple(x for j in range(op.size) for x in mem.get(op.mem.disp+j,(None,)*8))
        return (None,)*(op.size*8)
    def equal(a,b):
        na,nb=number(a),number(b)
        if na is not None and nb is not None:return na==nb
        if excluded is not None:
            sym,val=excluded
            if (a==sym and nb==val) or (b==sym and na==val):return False
        if any(x in [0,1] and y in [0,1] and x!=y for x,y in zip(a,b)):return False
        return None
    pending=[(start,regs,memory,None,[])];stores={};instructions={};iterations=0
    while pending:
        at,initial,oldmem,zf,path=pending.pop();assert at not in path,'Cycle before first expression store'
        i=by[at];instructions[at]=[i.mnemonic,i.op_str];ops=i.operands;state=dict(initial);mem=dict(oldmem)
        assert i.mnemonic!='call','Another call intervenes before the expression store'
        if ops and ops[0].type==X86_OP_MEM and ops[0].access&2 and ops[0].mem.base==X86_REG_EBP and not ops[0].mem.index:
            dest=ops[0].mem.disp
            if dest==target_disp:
                assert i.mnemonic=='mov';val=number(operand(initial,oldmem,ops[1]));assert val==expected,(hex(at),val,expected)
                stores[at]=dict(address=hex(at),bytes=ops[0].size,value=val,instruction=[i.mnemonic,i.op_str]);continue
            if other_fields and dest in other_fields:
                assert i.mnemonic=='mov' and number(operand(initial,oldmem,ops[1]))==other_fields[dest]
        answer=None;new_zf=zf
        if i.mnemonic=='mov' and len(ops)==2:answer=operand(initial,oldmem,ops[1])
        elif i.mnemonic in ['movzx','movsx']:
            a=operand(initial,oldmem,ops[1]);answer=a+((0,)*(ops[0].size*8-len(a)) if i.mnemonic=='movzx' else (a[-1],)*(ops[0].size*8-len(a)))
        elif i.mnemonic in ['sete','setz','setne','setnz']:
            answer=bits(int(zf if i.mnemonic in ['sete','setz'] else not zf),8) if zf is not None else (None,)*8
        elif i.mnemonic in ['cmp','test']:
            a,b=[operand(initial,oldmem,o) for o in ops]
            if i.mnemonic=='cmp':new_zf=equal(a,b)
            elif a==b:new_zf=equal(a,bits(0,len(a)))
            else:new_zf=None
        elif i.mnemonic in ['and','or','xor'] and len(ops)==2:
            a,b=[operand(initial,oldmem,o) for o in ops];out=[]
            for x,y in zip(a,b):
                if i.mnemonic=='and':out.append(0 if x==0 or y==0 else y if x==1 else x if y==1 else None)
                elif i.mnemonic=='or':out.append(1 if x==1 or y==1 else y if x==0 else x if y==0 else None)
                else:out.append(0 if x==y and x is not None else x^y if x in [0,1] and y in [0,1] else None)
            answer=tuple(out);new_zf=equal(answer,bits(0,len(answer)))
        elif i.mnemonic in ['inc','dec']:
            old=operand(initial,oldmem,ops[0]);n=number(old)
            answer=bits(n+(1 if i.mnemonic=='inc' else -1),len(old)) if n is not None else (None,)*len(old)
            new_zf=equal(answer,bits(0,len(answer)))
        elif any(md.reg_name(r)=='eflags' for r in i.regs_access()[1]):new_zf=None
        for r in i.regs_access()[1]:
            f=LANES.get(md.reg_name(r))
            if f:putreg(state,r,(None,)*f[1])
        if ops and ops[0].type==X86_OP_REG and ops[0].access&2 and answer is not None:putreg(state,ops[0].reg,answer)
        assert X86_REG_ESP not in i.regs_access()[1],'Stack pointer changes inside expression window'
        if ops and ops[0].type==X86_OP_MEM and ops[0].access&2:
            m=ops[0].mem;assert not m.index
            if m.base==X86_REG_ESP:
                assert esp_offset is not None and 0<=m.disp and esp_offset+m.disp+ops[0].size<=protected_start
            else:
                assert m.base==X86_REG_EBP,'Unmodelled memory write before expression store'
                value=answer if answer is not None else (None,)*(8*ops[0].size)
                for j in range(ops[0].size):mem[m.disp+j]=value[j*8:j*8+8]
        nexts=graph[at]
        if i.mnemonic in ['je','jz','jne','jnz'] and zf is not None:
            take=zf if i.mnemonic in ['je','jz'] else not zf
            nexts=[ops[0].imm if take else at+i.size]
        assert nexts,'Expression path exits without a field store'
        for nxt in nexts:assert nxt in by;pending.append((nxt,state,mem,new_zf,path+[at]))
        iterations+=1;assert iterations<10000
    assert stores
    return dict(stores=[stores[a] for a in sorted(stores)],esp_ebp_displacement=esp_offset,local_instructions=[[hex(a),*instructions[a]] for a in sorted(instructions)])


def analyze(image,debug,row,label):
    cmp=row['comparisons'][0][label];frame=cmp['frame_offset'];view=row['whole_frame_review'][label+'_owner_view']
    md,ins=decode(view,image);sm=symbol_map(image,debug);name=row['name'];cases=[];stack=stack_at_calls(image,view);excluded_calls=[]
    if name=='card_stitch_together':
        calls=[i for i in ins if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and '_has_threshold' in sm.get(i.operands[0].imm,set())]
        assert calls
        setting_sites={int(s['address'],16) for s in cmp['all_stores'] if s['offset']==172}
        graph=graph_for(ins);by={i.address:i for i in ins}
        for call in calls:
            todo=[call.address+call.size];seen=set();reachable_setting=set()
            while todo:
                at=todo.pop()
                if at in seen:continue
                seen.add(at)
                if at in setting_sites:reachable_setting.add(at);continue
                if by[at].mnemonic!='call':todo.extend(graph[at])
            if not reachable_setting:
                excluded_calls.append(dict(call=hex(call.address),reason='No AI-setting store reachable before another call/exit in the complete direct CFG.'))
                continue
            for nonzero in [False,True]:
                seed=tuple('H'+str(j) for j in range(32)) if nonzero else bits(0)
                proof=paths(image,view,call.address+call.size,frame+8+172,2 if nonzero else 1,
                    esp_offset=stack[call.address],protected_start=frame+8,return_seed=seed,excluded=(seed,0) if nonzero else None)
                cases.append(dict(call=hex(call.address),opaque_return='nonzero32' if nonzero else 'zero32',**proof))
        target=172;expected_sites={s['address'] for s in cmp['all_stores'] if s['offset']==target}
    else:
        assert name=='card_mercurial_transformation'
        calls=[i for i in ins if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and '_default_test_definition' in sm.get(i.operands[0].imm,set())];assert len(calls)==1
        call=calls[0];idx=ins.index(call)
        compare=next(i for i in ins[idx+1:] if i.mnemonic=='cmp')
        assert compare.operands[0].type==X86_OP_MEM and compare.operands[0].mem.base==X86_REG_EBP and not compare.operands[0].mem.index
        assert compare.operands[1].type==X86_OP_IMM and compare.operands[1].imm==1
        slot,size=compare.operands[0].mem.disp,compare.operands[0].size
        for one in [True,False]:
            seed=bits(1,size*8) if one else tuple('M'+str(j) for j in range(size*8))
            proof=paths(image,view,call.address+call.size,frame+8+104,0x44 if one else 0x86,
                esp_offset=stack[call.address],protected_start=min(frame+8,slot),memory_seed=(slot,seed),excluded=None if one else (seed,1),other_fields={frame+8+148:1 if one else 4,frame+8+152:1 if one else 4})
            cases.append(dict(initializer=hex(call.address),selector_frame_displacement=slot,selector_bytes=size,
                opaque_selector='one' if one else 'not_one',**proof))
        target=104;expected_sites={s['address'] for s in cmp['all_stores'] if s['offset']==target}
    assert expected_sites=={s['address'] for c in cases for s in c['stores']},'A field store lacks an exhaustive local expression proof'
    return dict(view=view,field_offset=target,cases=cases,excluded_resolution_only_calls=excluded_calls)


parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--output',default='docs/recovery-evidence/test-conditional-value-review.json');args=parser.parse_args()
review_path=Path('docs/recovery-evidence/test-outlier-scope-review.json');review=json.loads(review_path.read_bytes())
ni=Image(Path('.recovery/2022-ManalinkEh.dll').read_bytes());nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
ci=Image(Path('ManalinkEh.dll').read_bytes());cd=Image(Path('ManalinkEh.dbg').read_bytes());rows=[]
for label,im,dbg in [('native',ni,nd),('current',ci,cd)]:assert sha(im.data)==review[label+'_dll_sha256'] and sha(dbg.data)==review[label+'_dbg_sha256']
for row in review['rows']:
    if row['name'] not in ['card_stitch_together','card_mercurial_transformation']:continue
    source=read(row['source']);a,b=span(source,row['name']);assert sha(source[a:b].encode())==row['source_definition_sha256']
    rows.append(dict(source=row['source'],name=row['name'],source_definition_sha256=row['source_definition_sha256'],
        native=analyze(ni,nd,row,'native'),current=analyze(ci,cd,row,'current')))
assert len(rows)==2
record=dict(native_dll_sha256=sha(ni.data),native_dbg_sha256=sha(nd.data),current_dll_sha256=sha(ci.data),current_dbg_sha256=sha(cd.data),
    source_review=dict(path=review_path.as_posix(),sha256=sha(review_path.read_bytes())),tool_sha256=sha(Path(__file__).read_bytes()),rows=rows,
    scope='Independent bounded conditional-expression proof. Stitch assigns AI selection1 for zero and2 for nonzero opaque has_threshold returns; every AI-setting store is covered. Mercurial assigns subtype68 (Frog) for an opaque selector equal to1, otherwise134 (Octopus); encountered power/toughness stores agree with1/4. Every direct path from the credited call return to its first field store is explored; intervening calls, exits, cycles or unsupported values fail. Partial bit lanes and unsupported definitions are conservative. These proofs are conditional on the same opaque helper return/selector input: argument provenance, selector origin, full dispatch/control semantics, arbitrary aliases and gameplay are not established.')
out=Path(args.output);assert not out.exists();out.write_bytes((json.dumps(record,indent=2)+'\n').encode());print('Verified both conditional field expressions over zero/nonzero32 and selector==1/!=1 domains; all setting stores covered.')
