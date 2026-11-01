"""Read-only CFG proof for two native quantity stores versus one rebuilt store."""
import argparse,collections,json,sys
from pathlib import Path
sys.path.insert(0,'tools/recovery')
from capstone.x86 import X86_OP_REG,X86_OP_MEM,X86_OP_IMM,X86_REG_EBP
from array_role_analysis import snapshots
from array_census_analysis import symbol_map
from canonicalize_helpers import sha
from inspect_release import Image,value,inherited_value
from shared_stack_test_analysis import FAMILIES,frame_offset
from verify_wide_stack_test_recovery import decode,graph_for

def analyze(image,debug):
    source='functions/token_generation.c';name='generate_custom_token_from_subtype';s=snapshots(image,debug,[dict(source=source,name=name)],with_effects=False)[source,name]
    dw=debug.dwarf();d=dw.get_DIE_from_refaddr(int(s['die_offset'],16));params=[p for p in d.iter_children() if p.tag=='DW_TAG_formal_parameter']
    assert len(params)==7 and inherited_value(params[6],'DW_AT_name')=='amount';argument=frame_offset(dw,params[6])+8;assert argument==32
    token=next(v for v in s['variables'] if v['name']=='token');assert token['shape']['size']==76;qty=token['frame_offset']+8+43
    md,ins=decode(s['view'],image);graph=graph_for(ins);by={i.address:i for i in ins};sm=symbol_map(image,debug)
    def family(r):return FAMILIES.get(md.reg_name(r))
    def operand(state,op):
        if op.type==X86_OP_REG:
            f=family(op.reg);return state.get(f[0]) if f and f[2]==0 else None
        if op.type==X86_OP_MEM and op.mem.base==X86_REG_EBP and not op.mem.index and op.mem.disp==argument:return 'amount_low8'
    def step(i,initial):
        state=dict(initial);ops=i.operands;store=False
        for r in i.regs_access()[1]:
            f=family(r)
            if f and f[2]==0:state[f[0]]=None
        if i.mnemonic=='call':
            for r in ['eax','ecx','edx']:state[r]=None
        elif i.mnemonic in ['mov','movzx','movsx'] and len(ops)==2:
            v=operand(initial,ops[1])
            if ops[0].type==X86_OP_REG:
                f=family(ops[0].reg)
                if f and f[2]==0:state[f[0]]=v
            if ops[0].type==X86_OP_MEM and ops[0].mem.base==X86_REG_EBP and not ops[0].mem.index:
                assert not ops[0].mem.disp<=argument<ops[0].mem.disp+ops[0].size,'Formal amount stack word is modified'
                if ops[0].mem.disp==qty:
                    assert ops[0].size==1 and v=='amount_low8';store=True
        return state,store
    states={ins[0].address:{}};todo=collections.deque(states);updates=0
    while todo:
        at=todo.popleft();after,_=step(by[at],states[at])
        for dest in graph[at]:
            if dest not in states:states[dest]=dict(after);todo.append(dest);continue
            old=states[dest];merged={k:old.get(k) if old.get(k)==after.get(k) else None for k in set(old)|set(after)}
            if merged!=old:states[dest]=merged;todo.append(dest)
        updates+=1;assert updates<100000
    stores={at for at,state in states.items() if step(by[at],state)[1]}
    generators={i.address for i in ins if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and '_generate_token' in sm.get(i.operands[0].imm,set())};assert len(generators)==1
    def reachable(start,target,forbid=frozenset()):
        todo=[start];seen=set()
        while todo:
            at=todo.pop()
            if at in forbid or at in seen:continue
            if at==target:return True
            seen.add(at);todo.extend(graph[at])
        return False
    gen=next(iter(generators));assert not reachable(ins[0].address,gen,stores)
    for a in stores:
        for b in stores:
            if a!=b:assert not reachable(a,b)
        assert not any(reachable(n,a) for n in graph[a])
    return dict(view=s['view'],argument_number=6,amount_ebp_displacement=argument,token_frame=token['frame_offset'],qty_field_offset=43,
        stores=[dict(address=hex(a),instruction=[by[a].mnemonic,by[a].op_str],value='low8(formal amount)') for a in sorted(stores)],
        generator=hex(gen),every_generator_path_has_qty_store=True,stores_mutually_unreachable=True,no_store_site_repeats=True,
        instructions=[[hex(i.address),i.mnemonic,i.op_str] for i in ins])

parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--output',default='docs/recovery-evidence/token-quantity-clone-review.json');args=parser.parse_args()
ni=Image(Path('.recovery/2022-ManalinkEh.dll').read_bytes());nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());ci=Image(Path('ManalinkEh.dll').read_bytes());cd=Image(Path('ManalinkEh.dbg').read_bytes())
n=analyze(ni,nd);c=analyze(ci,cd);assert len(n['stores'])==2 and len(c['stores'])==1
r=dict(native_dll_sha256=sha(ni.data),native_dbg_sha256=sha(nd.data),current_dll_sha256=sha(ci.data),current_dbg_sha256=sha(cd.data),tool_sha256=sha(Path(__file__).read_bytes()),native=n,current=c,
    scope='Read-only complete direct-CFG quantity-store comparison for generate_custom_token_from_subtype. Both native stores and the one rebuilt store write the low8 bits of immutable formal amount7 at EBP+32 into token.qty offset43. Every direct path reaching the generator passes a quantity store; native store sites are mutually unreachable and no site repeats. Caller-save register definitions and unknown operations are conservative. This proves the duplicated byte-store value/path behavior, not arbitrary aliases, all token fields or gameplay.')
p=Path(args.output);assert not p.exists();p.write_bytes((json.dumps(r,indent=2)+'\n').encode());print('Verified native2/current1 quantity stores: same amount low8, mandatory generator-path coverage, mutually exclusive native sites, no repeats.')
