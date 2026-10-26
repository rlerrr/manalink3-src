"""Conservative native CFG register tracking for inline counter source arguments.

Only exact, path-invariant formal-parameter identities are admitted. Unsupported
instructions invalidate written registers. Unknown/unequal CFG values do not
become defaults. This is a narrow argument witness, not a function equivalence
claim or executable-code transformation.
"""
import collections,json,re,sys
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_AC_WRITE
from capstone.x86_const import X86_OP_REG,X86_OP_IMM,X86_OP_MEM
from canonicalize_helpers import ROOT,read,span,mask,sha
from inspect_release import Image
from unify_scalar_helpers import parameters

REGS=['eax','ecx','edx','ebx','esp','ebp','esi','edi']
INLINE={'add_counter':('add_counters',3),'remove_counter':('remove_counters',3),
 'add_1_1_counter':('add_counters',2),'add_1_1_counters':('add_counters',3),
 'remove_1_1_counter':('remove_counters',2),'remove_1_1_counters':('remove_counters',3),
 'add_minus1_minus1_counters':('add_counters',3)}
TARGETS={'add_counters':0x26d4840,'remove_counters':0x26d4c10}

def add(a,b):
    if a is None or b is None:return None
    if isinstance(a,int) and isinstance(b,int):return (a+b)&0xffffffff
    if isinstance(a,tuple) and a[0]=='sp' and isinstance(b,int):return ('sp',a[1]+(b if b<0x80000000 else b-0x100000000))
    if b==0:return a
    if a==0:return b
    return None

def address(ins,op,state):
    if op.type!=X86_OP_MEM or op.mem.segment:return None
    base=state.get(ins.reg_name(op.mem.base)) if op.mem.base else 0
    index=state.get(ins.reg_name(op.mem.index)) if op.mem.index else 0
    if not isinstance(index,int):return None
    return add(add(base,index*op.mem.scale),op.mem.disp)

def operand(ins,op,state):
    if op.type==X86_OP_IMM:return op.imm&0xffffffff
    if op.type==X86_OP_REG:return state.get(ins.reg_name(op.reg)) if op.size==4 else None
    if op.type==X86_OP_MEM and op.size==4:
        a=address(ins,op,state)
        return state.get(('mem',a)) if a is not None else None
    return None

def execute(ins,before):
    state=dict(before);ops=ins.operands;name=ins.mnemonic
    try:written=ins.regs_access()[1]
    except Exception:written=[]
    for reg in written:
        n=ins.reg_name(reg)
        full=next((r for r in REGS if n in {'e'+r[1:],r[1:],r[1]+'l',r[1]+'h'}),n)
        state.pop(full,None)
    # Drop stack words overwritten by this instruction, even partial writes.
    for op in ops:
        if op.type==X86_OP_MEM and op.access&CS_AC_WRITE:
            a=address(ins,op,before)
            if isinstance(a,tuple) and a[0]=='sp':
                for key in list(state):
                    if isinstance(key,tuple) and key[0]=='mem' and isinstance(key[1],tuple) and key[1][0]=='sp':
                        if a[1]<key[1][1]+4 and key[1][1]<a[1]+op.size:state.pop(key)
    value=None
    if name=='mov' and len(ops)==2 and ops[0].size==4:value=operand(ins,ops[1],before)
    elif name=='lea' and len(ops)==2:value=address(ins,ops[1],before)
    elif name in {'add','sub'} and len(ops)==2:
        a=operand(ins,ops[0],before);b=operand(ins,ops[1],before)
        value=add(a,-b if name=='sub' and isinstance(b,int) else b) if name=='add' or isinstance(b,int) else None
    elif name=='xor' and len(ops)==2 and ops[0].type==ops[1].type==X86_OP_REG and ops[0].reg==ops[1].reg:value=0
    elif name in {'inc','dec'} and len(ops)==1:value=add(operand(ins,ops[0],before),1 if name=='inc' else -1)
    if value is not None and ops:
        if ops[0].type==X86_OP_REG and ops[0].size==4:state[ins.reg_name(ops[0].reg)]=value
        elif ops[0].type==X86_OP_MEM and ops[0].size==4:
            a=address(ins,ops[0],before)
            if a is not None:state[('mem',a)]=value
    if name=='push' and len(ops)==1:
        sp=add(before.get('esp'),-4)
        if sp is not None:
            state['esp']=sp;v=operand(ins,ops[0],before)
            if v is not None:state[('mem',sp)]=v
    elif name=='pop' and len(ops)==1:
        sp=before.get('esp');v=before.get(('mem',sp));newsp=add(sp,4)
        if newsp is not None:state['esp']=newsp
        if ops[0].type==X86_OP_REG and v is not None:state[ins.reg_name(ops[0].reg)]=v
    elif name=='leave':
        sp=before.get('ebp');v=before.get(('mem',sp));newsp=add(sp,4)
        if newsp is not None:state['esp']=newsp
        if v is not None:state['ebp']=v
    elif name=='call':
        for reg in ['eax','ecx','edx']:state.pop(reg,None)
        # Callee sees only explicit outgoing arguments. Local stack values are
        # discarded across calls; incoming scalar argument slots remain valid.
        for key in list(state):
            if isinstance(key,tuple) and key[0]=='mem' and isinstance(key[1],tuple) and key[1][0]=='sp' and key[1][1]<4:state.pop(key)
    return state

def dataflow(image,row):
    if len(row['native_ranges'])!=1:return None,'no single concrete range'
    r=row['native_ranges'][0];lo,hi=int(r['low_pc'],16),int(r['high_pc'],16)
    if r['frame_base']!=[dict(operation='DW_OP_call_frame_cfa',arguments=[])]:return None,'unsupported native frame base'
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    insns=list(decoder.disasm(image.pe.get_data(lo-0x2000000,hi-lo),lo));by={i.address:i for i in insns}
    if not insns or insns[-1].address+insns[-1].size!=hi:return None,'incomplete instruction decoding'
    init={'esp':('sp',0)}
    for p in r['parameters']:
        loc=p['location'];v=('parameter',p['name'])
        if p['constant'] is not None:continue
        if not isinstance(loc,list) or len(loc)!=1:continue
        op=loc[0]
        if op['operation']=='DW_OP_fbreg':init[('mem',('sp',4+op['arguments'][0]))]=v
        elif re.fullmatch(r'DW_OP_reg[0-7]',op['operation']):init[REGS[int(op['operation'][9:])]]=v
    states={lo:init};work=collections.deque([lo]);steps=0
    while work:
        at=work.popleft();ins=by[at];before=states[at];after=execute(ins,before);steps+=1
        if steps>100000:return None,'CFG iteration limit'
        fall=at+ins.size;successors=[]
        if ins.mnemonic.startswith('j'):
            if ins.operands[0].type!=X86_OP_IMM:return None,'indirect branch'
            target=ins.operands[0].imm
            if target in by:successors.append(target)
            if ins.mnemonic!='jmp' and fall in by:successors.append(fall)
        elif ins.mnemonic.startswith(('ret','iret')) or ins.mnemonic in {'ud2','hlt'}:pass
        elif ins.mnemonic.startswith('loop'):return None,'unsupported loop instruction'
        elif fall in by:successors.append(fall)
        for target in successors:
            if target not in states:states[target]=dict(after);work.append(target)
            else:
                old=states[target];merged={k:v for k,v in old.items() if k in after and after[k]==v}
                if merged!=old:states[target]=merged;work.append(target)
    return (states,by),'ok'

def evaluate(ops,state):
    if not isinstance(ops,list) or len(ops)!=1:return None
    op=ops[0];name=op['operation'];args=op['arguments']
    if re.fullmatch(r'DW_OP_breg[0-7]',name):return add(state.get(REGS[int(name[10:])]),args[0])
    if name.startswith('DW_OP_lit'):return int(name[9:])
    if name.startswith('DW_OP_const'):return args[0]&0xffffffff
    return None

def argument(site,index):
    want=[dict(operation='DW_OP_breg4',arguments=[index*4])]
    rows=[a for a in site['arguments'] if a['location']==want]
    return rows[0]['value'] if len(rows)==1 else None

def inspect_row(image,row):
    computed,status=dataflow(image,row)
    if computed is None:return dict(status=status)
    states,by=computed;out=[]
    for site in row['native_sites']:
        if site['helper'] not in TARGETS:continue
        pc=int(site['return_pc'],16);at=pc-5;ins=by.get(at)
        if not ins or ins.mnemonic!='call' or ins.size!=5 or ins.operands[0].type!=X86_OP_IMM or ins.operands[0].imm!=TARGETS[site['helper']]:
            out.append(dict(site=site['die_offset'],status='not an anchored direct call'));continue
        state=states.get(at)
        if state is None:out.append(dict(site=site['die_offset'],status='unreached CFG node'));continue
        values=[evaluate(argument(site,i),state) for i in [0,1]]
        out.append(dict(site=site['die_offset'],return_pc=site['return_pc'],callee=site['helper'],values=values,
            expressions=[argument(site,i) for i in [0,1]],bytes_hex=ins.bytes.hex(),status='known source pair' if all(v is not None for v in values) else 'unknown source pair'))
    annotated={int(site['return_pc'],16)-5 for site in row['native_sites'] if site['helper'] in TARGETS}
    for ins in by.values():
        if ins.mnemonic=='call' and len(ins.operands)==1 and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in TARGETS.values() and ins.address not in annotated:
            out.append(dict(site=None,return_pc=hex(ins.address+ins.size),status='direct counter call lacks DWARF argument annotation'))
    return dict(status='analyzed',sites=out,range_bytes_sha256=sha(image.pe.get_data(int(row['native_ranges'][0]['low_pc'],16)-0x2000000,int(row['native_ranges'][0]['high_pc'],16)-int(row['native_ranges'][0]['low_pc'],16))))

def main():
    path=ROOT/'.recovery/native-inline-counter-caller-arguments-731.json';inventory=json.loads(path.read_text())
    image=Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes());assert sha(image.data)==inventory['dll_sha256']
    output=[]
    for row in inventory['callers']:
        result=inspect_row(image,row);output.append(dict(source=row['source'],caller=row['caller'],**result))
    counts=collections.Counter(r['status'] for r in output);known=collections.Counter()
    for row in output:
        if row['status']!='analyzed':continue
        sites=row['sites'];pairs=[s.get('values') for s in sites]
        if sites and all(p==[['parameter','player'],['parameter','card']] or p==[('parameter','player'),('parameter','card')] for p in pairs):known['uniform caller player/card']+=1
        known['known sites']+=sum(s['status']=='known source pair' for s in sites)
        known['sites']+=len(sites)
    out=dict(format=1,inventory=str(path.relative_to(ROOT)).replace('\\','/'),inventory_sha256=sha(path.read_bytes()),analyzer_sha256=sha(Path(__file__).read_bytes()),callers=output,counts=dict(counts),known=dict(known))
    target=ROOT/'.recovery/inline-counter-source-pairs-731-v2.json';assert not target.exists();target.write_text(json.dumps(out,indent=2)+'\n')
    print(dict(statuses=dict(counts),known=dict(known)))

if __name__=='__main__':main()
