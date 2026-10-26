"""Check retained executable fallbacks separately from canonical DLL functions."""
import json,re
from canonicalize_helpers import ROOT,read,mask,sha,span
from inspect_release import Image


def records(proof,root):
    receipts={r['path']:r['sha256'] for r in proof.get('interface_supporting_inputs',[])}
    for row in proof.get('source_domain_migrations',[]):
        if row['kind']!='typed_executable_fallback':continue
        assert sha((root/row['path']).read_bytes())==row['sha256']==receipts[row['path']]
        yield json.loads((root/row['path']).read_text())


def verify(proof,root=ROOT,source_root=None):
    source_root=root if source_root is None else source_root
    rows=list(records(proof,root))
    if not rows:return {}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_AC_READ
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_REG_EBP
    dll=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes())
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes())
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    result={}
    for row in rows:
        assert sha(dll.data)==row['dll_sha256'] and sha(exe.data)==row['exe_sha256']
        start=int(row['native_start'],16);end=int(row['native_end'],16)
        data=dll.pe.get_data(start-0x2000000,end-start)
        assert sha(data)==row['native_sha256']
        ins=list(decoder.disasm(data,start))
        assert [(i.mnemonic,i.op_str) for i in ins]==[tuple(r) for r in row['native_instructions']]
        target=int(row['target'],16)
        assert ins[-1].mnemonic=='jmp' and ins[-1].operands[0].type==X86_OP_IMM
        assert ins[-1].operands[0].imm==target and ins[-1].address+ins[-1].size==end
        # The release restores its original frame and tail-jumps with the same
        # four incoming arguments; the executable independently reads all four.
        expected_loads={8:'ebx',12:'edi',16:'esi',20:'edx'}
        for offset,reg in expected_loads.items():
            assert any(i.mnemonic=='mov' and i.op_str==f'{reg}, dword ptr [ebp + {hex(offset) if offset>9 else offset}]' for i in ins)
            assert any(i.mnemonic=='mov' and i.op_str==f'dword ptr [ebp + {hex(offset) if offset>9 else offset}], {reg}' for i in ins)
        assert [i.mnemonic for i in ins[-6:]]==['add','pop','pop','pop','pop','jmp']
        assert ins[-2].op_str=='ebp' and ins[-6].op_str=='esp, 0x24'
        offsets=set()
        for anchor in row['callee_argument_reads']:
            at=int(anchor['address'],16);raw=bytes.fromhex(anchor['bytes_hex'])
            assert exe.pe.get_data(at-0x400000,len(raw))==raw
            decoded=list(decoder.disasm(raw,at));assert len(decoded)==1
            op=next(o for o in decoded[0].operands if o.type==X86_OP_MEM and o.mem.base==X86_REG_EBP)
            assert op.access&CS_AC_READ and op.size==4 and op.mem.disp==anchor['offset']
            offsets.add(op.mem.disp)
        assert offsets==set(expected_loads)
        name=row['import_name'];caller=row['caller']
        header=read(source_root/'manalink.h');linker=read(source_root/'manalink.lds')
        assert header.count(row['prototype'])==1
        assert len(re.findall(r'(?mi)^_'+re.escape(name)+r'\s*=\s*0x'+format(target,'x')+r';',linker))==1
        assert not re.search(r'(?m)^_'+re.escape(caller)+r'\s*=',linker)
        source=read(source_root/row['source']);bounds=span(source,caller);assert bounds
        body=mask(source[bounds[0]:bounds[1]])
        assert body.count(row['fallback_call'])==1
        assert len(re.findall(r'\b'+re.escape(caller)+r'\s*\(',body))==1,'Accidental recursive fallback'
        result[caller]=dict(import_name=name,target=hex(target),arguments=4,tail_jump=hex(ins[-1].address))
    return result


def verify_compiled(debug,proof,root=ROOT):
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM
    from inspect_release import value,inherited_value
    from verify_inline_helpers import ranges
    physical=getattr(debug,'physical_image',debug)
    info=None;code=None
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    result={}
    for row in records(proof,root):
        name='_'+row['import_name'];symbols=[s for s in physical.symbols if s[0]==name]
        assert len(symbols)==1 and symbols[0][1]==int(row['target'],16) and symbols[0][2]==-1
        entries=[s for s in physical.symbols if s[0]=='_'+row['caller']]
        assert len(entries)==1 and entries[0][2]>0 and entries[0][3:]==(32,2)
        _,offset,section,_,_=entries[0]
        at=physical.pe.OPTIONAL_HEADER.ImageBase+physical.pe.sections[section-1].VirtualAddress+offset
        if info is None:info=physical.dwarf();code=Image((root/'ManalinkEh.dll').read_bytes())
        bodies=[]
        for cu in info.iter_CUs():
            if value(cu.get_top_DIE(),'DW_AT_name')!=row['source']:continue
            for die in cu.get_top_DIE().iter_children():
                if die.tag=='DW_TAG_subprogram' and inherited_value(die,'DW_AT_name')==row['caller'] and value(die,'DW_AT_low_pc')==at:
                    bodies.append(ranges(info,die))
        assert len(bodies)==1 and bodies[0]
        targets=set();sites=[]
        for low,end in bodies[0]:
            ins=list(decoder.disasm(code.pe.get_data(low-code.pe.OPTIONAL_HEADER.ImageBase,end-low),low))
            assert ins and ins[-1].address+ins[-1].size==end
            for i in ins:
                if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM:
                    targets.add(i.operands[0].imm)
                    if i.operands[0].imm==int(row['target'],16):sites.append(dict(address=hex(i.address),bytes_hex=i.bytes.hex()))
        assert sites and at not in targets,'Missing executable fallback or recursive DLL call'
        result[row['import_name']]=dict(target=row['target'],caller=hex(at),sites=sites)
    return result
