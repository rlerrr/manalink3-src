"""Inspect rebuilt full AI snapshot/save and support helper bodies, without loading them."""
from inspect_release import value, inherited_value


def verify(debug, pe, interfaces):
    if not interfaces.get('ai_snapshot_native_2022'):
        return {}
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_AC_READ, CS_AC_WRITE
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_ai_snapshots import NAMES, PUBLIC_HELPERS, WORKER, BACKUP, RESTORE, BACKUP0, RESTORE0, SAVE, AUTOTAP, COMBAT, EXE_TARGETS
    from ai_snapshot_bodies import ITEMS
    from ai_snapshot_evidence import stack_frames
    from verify_native_build import literal_register_call_targets
    decoder=Cs(CS_ARCH_X86, CS_MODE_32);decoder.detail=True
    wanted=set(NAMES.values());symbols={};ranges={};bodies={};storage={};enums={}
    expected_enums=dict(EVENT_CHECK_DESTROY_IF_BLOCKED=0x4500,DIFB_ASK_CARD=1,DIFB_ASK_ALL_CARDS=2,DIFB_DESTROYS_NONWALLS=4,DIFB_DESTROYS_WALLS=8,DIFB_DESTROYS_ALL=12,DIFB_DESTROYS_UNPROTECTED=16,AUTOTAP_NO_NONBASIC_LANDS=2,AUTOTAP_NO_DONT_AUTO_TAP=4,AUTOTAP_NO_ARTIFACTS=8,AUTOTAP_NO_CREATURES=16,SETTING_AUTOTAP_ARTIFACTS=17,SETTING_AUTOTAP_CREATURES=18,IPF2022_DAMAGE=1)
    from pathlib import Path
    if (Path(__file__).resolve().parents[2]/'docs/recovery-evidence/canonical-helper-domains.json').exists():
        expected_enums['IPF_DAMAGE']=expected_enums.pop('IPF2022_DAMAGE')
    def coff(n):
        matches=[s for s in debug.symbols if s[0]=='_'+n];assert len(matches)==1,(n,matches)
        _,off,sec,kind,st=matches[0]
        return (off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off),sec,kind,st
    for a,n in NAMES.items():
        at,sec,kind,st=coff(n);assert sec>0 and kind==32 and st==(3 if a==WORKER else 2),(n,sec,kind,st)
        symbols[n]=at
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/ai.c':continue
        for die in cu.iter_DIEs():
            n=inherited_value(die,'DW_AT_name')
            if die.tag=='DW_TAG_enumerator' and n in expected_enums:
                enums[n]=value(die,'DW_AT_const_value')
            if die.tag=='DW_TAG_subprogram' and value(die,'DW_AT_low_pc') is not None:
                lo=value(die,'DW_AT_low_pc');hi=die.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value
                ranges[(n,lo)]=end
                if n in wanted:
                    assert lo==symbols[n];bodies[n]=list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo),lo))
            if die.tag!='DW_TAG_variable' or n not in {p+k for p in ['backup_','backup0_'] for k,_,_ in ITEMS}:continue
            t=unqual(type_of(die));count=1;bounds=[]
            while t.tag=='DW_TAG_array_type':
                for c in t.iter_children():
                    if c.tag=='DW_TAG_subrange_type':
                        upper=value(c,'DW_AT_upper_bound');assert isinstance(upper,int);bounds.append(upper);count*=upper+1
                t=unqual(type_of(t))
            loc=value(die,'DW_AT_location');assert loc[0]==3 and len(loc)==5
            at=int.from_bytes(bytes(loc[1:]),'little');assert coff(n)[0]==at and coff(n)[3]==3
            storage[n]=dict(address=hex(at),bytes=count*value(t,'DW_AT_byte_size'),element_bytes=value(t,'DW_AT_byte_size'),bounds=bounds)
    assert len(storage)==46
    assert enums==expected_enums
    for p in ['backup_','backup0_']:
        for n,size,_ in ITEMS:assert storage[p+n]['bytes']==size
        _,sec,_,st=coff(p+'counters_added_type')
        assert st==3 and debug.pe.sections[sec-1].Name.rstrip(b'\0')==b'.bss'
    folded={}
    for n in wanted-set(bodies):
        old=n.removesuffix('_2022');target=coff(old)[0];at=symbols[n]
        insns=list(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,5),at))
        assert len(insns)==1 and insns[0].mnemonic=='jmp' and insns[0].operands[0].type==X86_OP_IMM and insns[0].operands[0].imm==target
        end=ranges[(old,target)];bodies[n]=list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target),target))
        folded[n]=dict(retained_target=old,target=hex(target),bytes=insns[0].bytes.hex())
    assert set(bodies)==wanted
    frames=stack_frames(bodies[NAMES[SAVE]],0x49f500)
    expected=[(coff(n)[0],size) for n,size,_ in ITEMS]
    assert [(f['pointer'],f['size']) for f in frames]==expected,frames
    last=next(k for k,i in enumerate(bodies[NAMES[SAVE]]) if hex(i.address)==frames[-1]['address'])
    assert not any(i.mnemonic.startswith('j') for i in bodies[NAMES[SAVE]][:last])
    bulk={};counter_access={}
    for a in [BACKUP,RESTORE,BACKUP0,RESTORE0]:
        insns=bodies[NAMES[a]];native=interfaces['ai_snapshot_native_2022']['native_storage']['snapshots'][a]
        imms={o.imm for i in insns for o in i.operands if o.type==X86_OP_IMM}
        assert {int(native['region'],16),native['region_bytes'],int(native['card_data'],16),0x480,57600,4000}<=imms,(a,imms)
        bank=native['bank'];source=coff('counters_added_type')[0];target=int(storage[bank+'counters_added_type']['address'],16)
        access={at:[dict(address=hex(i.address),bytes=i.bytes.hex(),width=o.size,read=bool(o.access&CS_AC_READ),write=bool(o.access&CS_AC_WRITE)) for i in insns for o in i.operands if o.type==X86_OP_MEM and not o.mem.base and not o.mem.index and o.mem.disp==at] for at in [source,target]}
        assert all(len(v)==1 and v[0]['width']==4 for v in access.values()),(a,access)
        restoring=a in {RESTORE,RESTORE0}
        assert access[source][0]['write']==restoring and access[target][0]['write']!=restoring
        counter_access[NAMES[a]]={hex(k):v for k,v in access.items()};bulk[NAMES[a]]=native
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}|literal_register_call_targets(insns)
    all_targets=set().union(*(targets(v) for v in bodies.values()))
    assert set(EXE_TARGETS)<=all_targets,[hex(a) for a in set(EXE_TARGETS)-all_targets]
    assert symbols[NAMES[WORKER]] in targets(bodies[NAMES[COMBAT]])
    assert coff('is_protected_from_2022')[0] in targets(bodies[NAMES[WORKER]])
    flags=[i.operands[1].imm for i in bodies[NAMES[AUTOTAP]] if i.mnemonic=='mov' and len(i.operands)==2 and i.operands[0].type==X86_OP_MEM and i.operands[0].mem.disp==12 and i.operands[1].type==X86_OP_IMM]
    assert sorted(flags)==[4,12,20,28,30],flags
    return dict(public_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in PUBLIC_HELPERS},private_worker_symbol=hex(symbols[NAMES[WORKER]]),folded_public_thunks=folded,enum_values=enums,autotap_flags=flags,
        snapshot_storage=storage,bulk_snapshot_regions=bulk,counter_type_accesses=counter_access,unconditional_save_frames=frames,exe_boundaries=[hex(a) for a in EXE_TARGETS])
