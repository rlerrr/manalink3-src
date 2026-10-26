"""Inspect native snapshot storage and the ordered save/load frames, without execution."""
from inspect_release import value, inherited_value


def stack_frames(instructions, target):
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG, X86_REG_ESP, X86_REG_EAX, X86_REG_ECX, X86_REG_EDX
    slots = {}; calls = []; registers = {}
    for ins in instructions:
        ops = ins.operands
        if ins.mnemonic == 'mov' and len(ops) == 2 and ops[0].type == X86_OP_REG:
            registers[ops[0].reg] = ops[1].imm if ops[1].type == X86_OP_IMM else None
        if ins.mnemonic == 'mov' and len(ops) == 2 and ops[0].type == X86_OP_MEM:
            mem = ops[0].mem
            if mem.base == X86_REG_ESP and not mem.index:
                slots[mem.disp] = ops[1].imm if ops[1].type == X86_OP_IMM else None
        if ins.mnemonic == 'call':
            destination = ops[0].imm if ops[0].type == X86_OP_IMM else registers.get(ops[0].reg) if ops[0].type == X86_OP_REG else None
            if destination == target:
                calls.append(dict(address=hex(ins.address), pointer=slots.get(0), size=slots.get(4)))
            for reg in [X86_REG_EAX, X86_REG_ECX, X86_REG_EDX]:
                registers.pop(reg, None)
        elif ins.mnemonic != 'mov':
            for reg in ins.regs_access()[1]:
                registers.pop(reg, None)
    return calls


def inspect_native(runtime, debug, meta, root):
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_ai_snapshots import ADDRESSES, PRIVATE, BACKUP, RESTORE, BACKUP0, RESTORE0, SAVE, AUTOTAP, WORKER, COMBAT, EXE_TARGETS, GLOBALS
    from ai_snapshot_bodies import ITEMS
    from recover_literals import readonly_string
    from recover_temporary_exile import sha
    from inspect_release import Image
    decoder = Cs(CS_ARCH_X86, CS_MODE_32); decoder.detail = True
    bodies = {a:list(decoder.disasm(runtime.pe.get_data(int(a,16)-0x2000000, int(meta[a]['high_pc'],16)-int(a,16)),int(a,16))) for a in ADDRESSES}
    def coff(name):
        ss = [s for s in debug.symbols if s[0] == '_'+name]; assert len(ss) == 1, (name,ss)
        _, off, sec, kind, storage = ss[0]
        return dict(address=hex(off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off), section=sec, kind=kind, storage=storage)
    symbols = {a:coff(meta[a]['name']) for a in ADDRESSES}
    assert all(int(symbols[a]['address'],16)==int(a,16) and symbols[a]['storage']==(3 if a in PRIVATE else 2) and symbols[a]['kind']==32 for a in ADDRESSES)
    storage = {}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') != 'functions/ai.c':continue
        for die in cu.iter_DIEs():
            n=inherited_value(die,'DW_AT_name')
            if die.tag!='DW_TAG_variable' or n not in {p+k for p in ['backup_','backup0_'] for k,_,_ in ITEMS}:continue
            t=unqual(type_of(die)); count=1; bounds=[]
            while t.tag=='DW_TAG_array_type':
                for c in t.iter_children():
                    if c.tag=='DW_TAG_subrange_type':
                        upper=value(c,'DW_AT_upper_bound');assert isinstance(upper,int);bounds.append(upper);count*=upper+1
                t=unqual(type_of(t))
            loc=value(die,'DW_AT_location');assert loc[0]==3 and len(loc)==5
            symbol=coff(n);assert int(symbol['address'],16)==int.from_bytes(bytes(loc[1:]),'little') and symbol['storage']==3
            storage[n]=dict(**symbol,bytes=count*value(t,'DW_AT_byte_size'),element_bytes=value(t,'DW_AT_byte_size'),bounds=bounds)
    assert len(storage)==46
    for prefix in ['backup_','backup0_']:
        for n,size,_ in ITEMS:assert storage[prefix+n]['bytes']==size,(prefix,n)
    for g in GLOBALS:
        s=storage[g['name']];assert s['address']==hex(int(g['target_address'],16)) and s['bytes']==4 and s['element_bytes']==4
        section=debug.pe.sections[s['section']-1];assert section.Name.rstrip(b'\0')==b'.bss'
    frames=stack_frames(bodies[SAVE],0x49f500)
    expected=[dict(name=n,pointer=int(coff(n)['address'],16),size=size) for n,size,_ in ITEMS]
    assert [(f['pointer'],f['size']) for f in frames]==[(f['pointer'],f['size']) for f in expected]
    assert not any(i.mnemonic.startswith('j') for i in bodies[SAVE][:next(k for k,i in enumerate(bodies[SAVE]) if hex(i.address)==frames[-1]['address'])])
    snapshots={}
    for a,bank,region,cards,size in [(BACKUP,'backup_',0x50be70,0x5265b0,0x1a740),(RESTORE,'backup_',0x50be70,0x5265b0,0x1a740),(BACKUP0,'backup0_',0x526a30,0x543730,0x1cd00),(RESTORE0,'backup0_',0x526a30,0x543730,0x1cd00)]:
        imm={o.imm for i in bodies[a] for o in i.operands if o.type==X86_OP_IMM}
        absolute={o.mem.disp for i in bodies[a] for o in i.operands if o.type==X86_OP_MEM and not o.mem.base and not o.mem.index}
        assert {region,cards,size,0x480,0xe100,4000}<=imm
        assert int(storage[bank+'counters_added_type']['address'],16) in absolute
        assert int(coff('counters_added_type')['address'],16) in absolute
        snapshots[a]=dict(bank=bank,region=hex(region),region_bytes=size,card_data=hex(cards),card_data_bytes=0x480,card_data_slots=16,card_data_stride=72)
    anchors={
        'bank0_clear':('26b1b2c','c70544bb5b0000000000'),
        'bank0_stack_test':('26b1b55','a1a857500085c00f88a6010000'),
        'bank0_abort_continuation':('26b1d14','e949feffff'),
        'combat_swap_low_bit':('26b216f','8d1400d1f883e20283e00109d0'),
    }
    for n,(a,b) in anchors.items():assert runtime.pe.get_data(int(a,16)-0x2000000,len(bytes.fromhex(b))).hex()==b,n
    abort=bodies[BACKUP0][-3:];literal_frames=[]
    for i in bodies[BACKUP0]:
        if i.mnemonic=='mov' and len(i.operands)==2 and i.operands[1].type==X86_OP_IMM:
            ptr=i.operands[1].imm
            if 0x2700000<ptr<0x2770000:
                try:lit=readonly_string(runtime,ptr)
                except (ValueError,AssertionError):continue
                if lit==b'functions/ai.c:247\nstack_size >= 0':literal_frames.append(dict(address=hex(ptr),bytes=(lit+b'\0').hex()))
    assert len(literal_frames)==1
    flags=[o.imm for i in bodies[AUTOTAP] if i.mnemonic=='mov' and len(i.operands)==2 and i.operands[0].type==X86_OP_MEM and i.operands[0].mem.disp==12 for o in [i.operands[1]] if o.type==X86_OP_IMM]
    assert sorted(flags)==[4,12,20,28,30]
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,snapshot_storage=storage,snapshots=snapshots,save_items=expected,save_frames=frames,unconditional_save_calls=23,autotap_flags=flags,
        worker_physical_parameters=['eax:lethal_player','edx:lethal_card','ecx:other_player','stack:other_card'],assertion_literal=literal_frames[0],
        anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},
        behavior=['Copy complete EXE snapshot regions and sixteen 72-byte card-data slots; bank0 clears the native control flag before the card-data copy and checks stack_size afterwards. The abort call can return to supplemental copying in the machine code.',
        'Both banks preserve all 23 supplemental fields, including four-byte counters_added_type, one-byte always_prompt_for_color and the existing extension/cache fragments. Keep all 23 save/load calls unconditional and in native order, combine status with bitwise AND and decrement turn_count after the calls when the native mode flag is nonzero.',
        'Human autotap retains five flag combinations, artifact/creature settings and human/trace/speculation gating. The private lethal-blocking helper has three register arguments and one stack argument; retain byte flags, wall/protection checks, both callback paths and result-bit swapping.'])
