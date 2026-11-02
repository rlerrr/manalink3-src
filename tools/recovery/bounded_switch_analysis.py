"""Direct CFGs with readonly indexed tables protected by checked CMP/JA guards."""
import collections
from capstone.x86 import X86_OP_IMM,X86_OP_MEM,X86_OP_REG,X86_REG_EBP
from canonicalize_helpers import sha


def graph(ins,image):
    by={i.address:i for i in ins};edges={};tables=[]
    for j,i in enumerate(ins):
        nexts=[];fall=i.address+i.size
        assert not i.mnemonic.startswith('loop'),'LOOP needs separate analysis'
        if i.mnemonic.startswith('ret'):pass
        elif i.mnemonic.startswith('j'):
            op=i.operands[0]
            if op.type==X86_OP_IMM:
                if op.imm in by:nexts.append(op.imm)
                if i.mnemonic!='jmp' and fall in by:nexts.append(fall)
            else:
                assert i.mnemonic=='jmp' and op.type==X86_OP_MEM and op.size==4 and j>=2
                m=op.mem;assert not m.base and m.index and m.scale==4
                load=None;cmp,guard=ins[j-2:j]
                if guard.mnemonic=='mov':
                    assert j>=3;cmp,guard,load=ins[j-3:j]
                    assert load.operands[0].type==X86_OP_REG and load.operands[0].size==4 and load.operands[0].reg==m.index
                    a=cmp.operands[0];b=load.operands[1]
                    assert a.type==b.type==X86_OP_MEM and a.size==b.size==4
                    assert a.mem.base==b.mem.base==X86_REG_EBP and not a.mem.index and not b.mem.index and a.mem.disp==b.mem.disp
                else:
                    assert cmp.operands[0].type==X86_OP_REG and cmp.operands[0].size==4 and cmp.operands[0].reg==m.index
                assert cmp.mnemonic=='cmp' and cmp.operands[1].type==X86_OP_IMM and 0<=cmp.operands[1].imm<=255
                assert guard.mnemonic=='ja' and guard.operands[0].type==X86_OP_IMM and guard.operands[0].imm!=i.address
                chain=[cmp,guard]+([load] if load is not None else [])+[i]
                assert all(a.address+a.size==b.address for a,b in zip(chain,chain[1:]))
                count=cmp.operands[1].imm+1;rva=m.disp-image.pe.OPTIONAL_HEADER.ImageBase
                section=image.pe.get_section_by_rva(rva);assert section is not None and not section.Characteristics&0x80000000
                assert rva+4*count<=section.VirtualAddress+section.Misc_VirtualSize
                raw=image.pe.get_data(rva,count*4);assert len(raw)==count*4
                targets=[int.from_bytes(raw[k:k+4],'little') for k in range(0,len(raw),4)];assert all(a in by for a in targets)
                nexts=list(set(targets));tables.append(dict(jump=hex(i.address),guard_chain=[hex(x.address) for x in chain],
                    table_address=hex(m.disp),entries=count,bytes_sha256=sha(raw),targets=[hex(a) for a in targets]))
        elif fall in by:nexts.append(fall)
        edges[i.address]=nexts
    pred=collections.defaultdict(set)
    for a,bs in edges.items():
        for b in bs:pred[b].add(a)
    for t in tables:
        chain=[int(a,16) for a in t['guard_chain']]
        for a,b in zip(chain,chain[1:]):assert pred[b]=={a},'Switch guard can be bypassed'
    return edges,tables
