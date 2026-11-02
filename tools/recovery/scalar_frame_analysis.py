"""Conservative direct scalar frame stores, including16-bit index-register lanes."""
from capstone.x86 import X86_OP_MEM,X86_OP_REG,X86_OP_IMM,X86_REG_EBP
from canonicalize_helpers import sha
from verify_inline_helpers import ranges
from verify_wide_stack_test_recovery import decode
from shared_stack_test_analysis import FAMILIES as BASE_FAMILIES
FAMILIES=dict(BASE_FAMILIES)
for base,short in [('esi','si'),('edi','di'),('ebp','bp'),('esp','sp')]:FAMILIES[short]=(base,16,0)


def collect(image,dw,die,frame,fields,*,start=0,end=216):
    """Basic-block constants with partial lanes and all explicit/implicit kills."""
    parts=ranges(dw,die);assert len(parts)==1;lo,hi=parts[0]
    raw=image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo)
    view=dict(address=hex(lo),end=hex(hi),bytes_sha256=sha(raw))
    md,ins=decode(view,image)
    assert any(i.mnemonic=='mov' and i.op_str=='ebp, esp' for i in ins[:8])
    leaders={lo};known={};stores=[]
    for i in ins:
        if i.mnemonic.startswith('j'):
            if i.operands[0].type==X86_OP_IMM:leaders.add(i.operands[0].imm)
            leaders.add(i.address+i.size)
    def get(reg):
        f=FAMILIES.get(md.reg_name(reg))
        if not f:return None
        name,bits,shift=f;mask=((1<<bits)-1)<<shift;old=known.get(name,(0,0))
        return old[1]>>shift&((1<<bits)-1) if old[0]&mask==mask else None
    def put(reg,val):
        f=FAMILIES.get(md.reg_name(reg))
        if not f:return
        name,bits,shift=f;mask=((1<<bits)-1)<<shift;old=known.get(name,(0,0))
        known[name]=((old[0]&~mask)|(mask if val is not None else 0),(old[1]&~mask)|((val or 0)<<shift&mask))
    def operand(op):
        return op.imm if op.type==X86_OP_IMM else get(op.reg) if op.type==X86_OP_REG else None
    for i in ins:
        if i.address in leaders:known.clear()
        ops=i.operands
        if i.mnemonic=='call':
            for n in ['eax','ecx','edx']:known.pop(n,None)
            continue
        if len(ops)==2 and ops[0].type==X86_OP_MEM and i.mnemonic in ['mov','and','or','add','sub']:
            m=ops[0].mem;off=m.disp-8-frame
            if m.base==X86_REG_EBP and not m.index and start<=off<end:
                val=operand(ops[1]) if i.mnemonic=='mov' else None
                stores.append(dict(address=hex(i.address),offset=off,size=ops[0].size,
                    value=None if val is None else val&((1<<(8*ops[0].size))-1),instruction=[i.mnemonic,i.op_str]))
        answer=None;recognized=False
        if len(ops)==2 and ops[0].type==X86_OP_REG:
            if i.mnemonic=='mov':answer=operand(ops[1]);recognized=True
            elif i.mnemonic=='xor' and ops[1].type==X86_OP_REG and ops[0].reg==ops[1].reg:answer=0;recognized=True
        for reg in i.regs_access()[1]:put(reg,None)
        if recognized:put(ops[0].reg,answer)
    return dict(view,stores=stores)

