"""Read-only bit-vector proof for the two packed token-argument differences."""
import argparse,json,sys
from pathlib import Path
sys.path.insert(0,'tools/recovery')
from capstone.x86 import X86_OP_IMM,X86_OP_MEM,X86_OP_REG,X86_REG_EBP
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from array_role_analysis import snapshots,effects
from array_census_analysis import symbol_map
from canonicalize_helpers import sha,read,span
from inspect_release import Image,value,inherited_value
from verify_wide_stack_test_recovery import decode
from shared_stack_test_analysis import FAMILIES
from verify_inline_helpers import ranges

def const(v,n=32):return tuple((v>>j)&1 for j in range(n))
X=tuple('X'+str(j) for j in range(32))
def opbit(a,b,op):
    if op=='and':
        if a==0 or b==0:return 0
        if a==1:return b
        if b==1 or a==b:return a
    else:
        if a==1 or b==1:return 1
        if a==0:return b
        if b==0 or a==b:return a
    return None

def entry_location(dw,die,param,pc):
    attr=param.attributes['DW_AT_location'];loc=LocationParser(dw.location_lists()).parse_from_attribute(attr,die.cu['version'],die=param)
    if isinstance(loc,LocationExpr):return loc.loc_expr
    base=value(die.cu.get_top_DIE(),'DW_AT_low_pc',0)
    for e in loc:
        if isinstance(e,BaseAddressEntry):base=e.base_address;continue
        a=e.begin_offset if e.is_absolute else base+e.begin_offset;b=e.end_offset if e.is_absolute else base+e.end_offset
        if a<=pc<b:return e.loc_expr
    raise AssertionError('No parameter entry location')

def analyze(image,debug,source,name,kind):
    snap=snapshots(image,debug,[dict(source=source,name=name)],with_effects=False)[source,name]
    token=next(v for v in snap['variables'] if v['name']=='token');assert token['shape']['size']==76
    md,ins=decode(snap['view'],image);sm=symbol_map(image,debug)
    def iscall(i,n):return i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and n in sm.get(i.operands[0].imm,set())
    initializers=[j for j,i in enumerate(ins) if iscall(i,'_default_token_definition')];generators=[j for j,i in enumerate(ins) if iscall(i,'_generate_token')]
    assert len(initializers)==len(generators)==1;init,gen=initializers[0],generators[0];assert init<gen
    start=init+1 if kind=='counter_choice' else 0
    assert not any(i.mnemonic.startswith('j') for i in ins[start:gen])
    frame_disp=token['frame_offset']+8+52;regs={};roles={};target=[None]*32;writes=[];seed_loads=[];origin={}
    def family(r):return FAMILIES.get(md.reg_name(r))
    if kind=='counter_choice':
        table=next(v for v in snap['variables'] if v['name']=='counters');assert table['shape']['dimensions']==[4] and table['shape']['element_size']==4 and table['shape']['size']==16
        table_disp=table['frame_offset']+8;selector_disp=table_disp-4;values={}
        for i in ins[:init]:
            ops=i.operands
            if i.mnemonic=='mov' and len(ops)==2 and ops[0].type==X86_OP_MEM and ops[0].mem.base==X86_REG_EBP and not ops[0].mem.index and ops[0].size==4 and ops[1].type==X86_OP_IMM:
                relative=ops[0].mem.disp-table_disp
                if relative in [0,4,8,12]:values[relative//4]=ops[1].imm
        assert values=={0:0,1:154,2:152,3:153}
        slots=[j for j,i in enumerate(ins[:init]) if i.mnemonic=='mov' and i.operands[0].type==X86_OP_MEM and i.operands[0].mem.base==X86_REG_EBP and not i.operands[0].mem.index and i.operands[0].mem.disp==selector_disp and i.operands[1].type==X86_OP_REG and md.reg_name(i.operands[1].reg)=='eax'];assert slots
        slot=slots[-1];dialogs=[j for j,i in enumerate(ins[:slot]) if iscall(i,'_dialog_impl')];assert dialogs;dialog=dialogs[-1]
        for i in ins[dialog+1:slot]:
            assert i.mnemonic!='call' and not i.mnemonic.startswith('j')
            assert not any(family(r) and family(r)[0]=='eax' for r in i.regs_access()[1])
        origin=dict(kind='same_dialog_result_indexed_int_table',table_values=[values[j] for j in range(4)],table_bytes=16,
            selector_slot_relative_to_table=-4,dialog_call=hex(ins[dialog].address),selector_store=hex(ins[slot].address))
    else:
        dw=debug.dwarf();die=dw.get_DIE_from_refaddr(int(snap['die_offset'],16));all_params=[p for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
        folded={inherited_value(p,'DW_AT_name'):value(p,'DW_AT_const_value') for p in all_params if value(p,'DW_AT_const_value') is not None}
        assert folded in [{},{'amount':1}]
        params=[p for p in all_params if value(p,'DW_AT_const_value') is None];assert len(params)==3
        assert entry_location(dw,die,params[2],ins[0].address)==[0x51] # x86 ECX
        regs['ecx']=X;origin=dict(kind='third_physical_int_parameter_in_ecx',parameter=2,bits=32,folded_parameters=folded)
        table_disp=selector_disp=None
    def getreg(r):
        f=family(r)
        if not f:return (None,)*32
        return regs.get(f[0],(None,)*32)[f[2]:f[2]+f[1]]
    def setreg(r,bits,role=None):
        f=family(r)
        if not f:return
        old=list(regs.get(f[0],(None,)*32));old[f[2]:f[2]+f[1]]=list(bits[:f[1]]);regs[f[0]]=tuple(old);roles[f[0]]=role if f[1:]==(32,0) else None
    def operand(op,site):
        if op.type==X86_OP_IMM:return const(op.imm,op.size*8),None
        if op.type==X86_OP_REG:return getreg(op.reg),roles.get(family(op.reg)[0]) if family(op.reg) else None
        if op.type==X86_OP_MEM and kind=='counter_choice':
            m=op.mem
            if m.base==X86_REG_EBP and not m.index and m.disp==selector_disp and op.size==4:return (None,)*32,'dialog_index'
            if m.base==X86_REG_EBP and m.index and m.scale==4 and m.disp==table_disp and op.size==4:
                assert family(m.index) and roles.get(family(m.index)[0])=='dialog_index';seed_loads.append(hex(site));return X,'selected_counter_word'
        return (None,)*(op.size*8),None
    for i in ins[start:gen+1]:
        if iscall(i,'_generate_token'):break
        if i.mnemonic=='call':
            assert iscall(i,'_default_token_definition');target=[None]*32
            for reg in ['eax','ecx','edx']:regs[reg]=(None,)*32;roles[reg]=None
            continue
        ops=i.operands;answer=None;role=None
        if i.mnemonic=='mov' and len(ops)==2:answer,role=operand(ops[1],i.address)
        elif i.mnemonic in ['and','or'] and len(ops)==2:
            a,_=operand(ops[0],i.address);b,_=operand(ops[1],i.address);answer=tuple(opbit(x,y,i.mnemonic) for x,y in zip(a,b))
        elif i.mnemonic=='shl' and len(ops)==2 and ops[1].type==X86_OP_IMM:
            a,_=operand(ops[0],i.address);n=ops[1].imm&31;answer=(0,)*n+a[:len(a)-n]
        for reg in i.regs_access()[1]:
            f=family(reg)
            if f:
                old=list(regs.get(f[0],(None,)*32));old[f[2]:f[2]+f[1]]=[None]*f[1];regs[f[0]]=tuple(old);roles[f[0]]=None
        if ops and ops[0].type==X86_OP_REG and ops[0].access&2 and answer is not None:setreg(ops[0].reg,answer,role)
        if ops and ops[0].type==X86_OP_MEM and ops[0].access&2:
            m=ops[0].mem
            if m.base==X86_REG_EBP and not m.index and max(m.disp,frame_disp)<min(m.disp+ops[0].size,frame_disp+4):
                assert i.mnemonic=='mov' and answer is not None
                for byte in range(ops[0].size):
                    relative=m.disp+byte-frame_disp
                    if 0<=relative<4:target[relative*8:relative*8+8]=answer[byte*8:byte*8+8]
                writes.append(dict(address=hex(i.address),offset=m.disp-frame_disp,bytes=ops[0].size,instruction=[i.mnemonic,i.op_str]))
    assert all(b is not None for b in target)
    if kind=='counter_choice':assert len(seed_loads)==1 and tuple(target)==const(1,16)+X[:16]
    else:assert tuple(target)==X[:16]+const(0,16)
    eff=effects(image,debug,snap['view']);generation=next(c for c in eff['calls'] if c['address']==hex(ins[gen].address));assert generation['arguments'][0]==['frame',token['frame_offset']+8]
    return dict(view=snap['view'],die_offset=snap['die_offset'],origin=origin,token_frame=token['frame_offset'],action_argument_offset=52,
        target_bits=target,store_sites=writes,seed_loads=seed_loads,generator=hex(ins[gen].address),
        instruction_window=[[hex(i.address),i.mnemonic,i.op_str] for i in ins[start:gen+1]])

parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--output',default='docs/recovery-evidence/token-packed-value-review.json');args=parser.parse_args()
ni=Image(Path('.recovery/2022-ManalinkEh.dll').read_bytes());nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());ci=Image(Path('ManalinkEh.dll').read_bytes());cd=Image(Path('ManalinkEh.dbg').read_bytes());rows=[]
for source,native_name,current_name,kind in [('cards/ikoria.c','card_vivien_monsters_advocate','card_vivien_monsters_advocate','counter_choice'),('cards/strixhaven.c','make_fractal','make_one_fractal','counter_number')]:
    n=analyze(ni,nd,source,native_name,kind);c=analyze(ci,cd,source,current_name,kind);assert n['target_bits']==c['target_bits']
    text=read(source);a,b=span(text,current_name);rows.append(dict(source=source,native_name=native_name,current_name=current_name,kind=kind,native=n,current=c,source_definition_sha256=sha(text[a:b].encode())))
report=dict(native_dll_sha256=sha(ni.data),native_dbg_sha256=sha(nd.data),current_dll_sha256=sha(ci.data),current_dbg_sha256=sha(cd.data),tool_sha256=sha(Path(__file__).read_bytes()),rows=rows,
    scope='Read-only targeted bit-vector equivalence for two differing direct store sequences. Tracks each bit of32-bit values through actual MOV,AND,OR,SHL and partial writes; unsupported definitions become unknown, calls kill caller-save registers and token memory. Native/current generation sees identical action_argument bits for all32-bit input words. Vivien binds the indexed four-int table, constants0/154/152/153 and the selector spill to a direct dialog_impl result; chosen index/bounds and dialog control semantics are not proven. Fractal binds formal parameter3 to ECX at each function entry. This checks the packed field and its generator operand, not all token contents, arbitrary aliasing or gameplay.')
out=Path(args.output);assert not out.exists();out.write_bytes((json.dumps(report,indent=2)+'\n').encode());print('Verified2 packed token arguments over all32-bit words: Vivien low16=1/high16=selected counter; Fractal low16=amount/high16=0.')
