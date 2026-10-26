"""Read-only GCC/DWARF verification of damage-prevention recovery."""
from inspect_release import value,inherited_value


def verify(debug,pe,interfaces):
    component=interfaces.get('damage_prevention_native_2022')
    if not component:return {}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG
    from export_abi import type_of,type_name
    from recover_damage_prevention import NAMES,PRIVATE,WORKER,SHIELD,NEXT,MY,DEALT_LEGACY,FOG_LEGACY,FOG_SPECIAL2,DAMAGE,NEGATE_COMBAT
    from damage_prevention_evidence import accessed_records
    from verify_native_build import literal_register_call_targets
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    native=component['native_storage'];symbols={};bodies={};ranges={};returns={};inlines=[];physical_returns={}
    def coff(n):
        ss=[s for s in debug.symbols if s[0]=='_'+n];assert len(ss)==1,(n,ss)
        _,off,sec,kind,storage=ss[0]
        return (off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off),sec,kind,storage
    missing=[]
    for a,n in NAMES.items():
        if not any(s[0]=='_'+n for s in debug.symbols):missing.append(n);continue
        at,sec,kind,storage=coff(n);assert sec>0 and kind==32 and storage==(3 if a in PRIVATE else 2)
        symbols[n]=at
    assert not set(missing)-{NAMES[WORKER]}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name');lo=value(d,'DW_AT_low_pc')
            if d.tag=='DW_TAG_subprogram' and lo is not None:
                hi=d.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value;ranges[(n,lo)]=end
                physical_returns[(n,lo)]=type_name(type_of(d))
                if n in symbols:
                    assert lo==symbols[n];returns[n]=physical_returns[(n,lo)]
                    bodies[n]=list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo),lo))
                    if n==NAMES[WORKER]:
                        assert [inherited_value(c,'DW_AT_name') for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']==['player','card','flags']
            if d.tag=='DW_TAG_inlined_subroutine':
                p=d.get_parent()
                while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
                owner=inherited_value(p,'DW_AT_name') if p is not None else None
                if owner in set(NAMES.values()):
                    spans=[]
                    if lo is not None:
                        hi=d.attributes['DW_AT_high_pc'];spans=[(lo,hi.value if hi.form=='DW_FORM_addr' else lo+hi.value)]
                    else:
                        base=value(cu.get_top_DIE(),'DW_AT_low_pc')
                        for r in debug.dwarf().range_lists().get_range_list_at_offset(value(d,'DW_AT_ranges'),cu=cu):
                            if hasattr(r,'base_address'):base=r.base_address
                            else:
                                lo=r.begin_offset+(0 if r.is_absolute else base);end=r.end_offset+(0 if r.is_absolute else base)
                                if end>lo:spans.append((lo,end))
                    assert spans;inlines.append(dict(name=n,parent=owner,ranges=spans))
    records,enums,signed=accessed_records(debug)
    assert records==native['records'] and enums==native['enum_values'] and signed==native['signed_fields']
    folded={};physical={n:n for n in bodies}
    for n in set(symbols)-set(bodies):
        old=n.removesuffix('_2022');target=coff(old)[0];at=symbols[n];end=ranges[(old,target)]
        branch=next(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,5),at,count=1))
        assert branch.mnemonic=='jmp' and branch.operands[0].type==X86_OP_IMM and branch.operands[0].imm==target,(n,branch.mnemonic,branch.op_str)
        physical[n]=old;bodies[n]=list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target),target));returns[n]=physical_returns[(old,target)]
        folded[n]=dict(target_name=old,target=hex(target),bytes=branch.bytes.hex(),target_length=end-target)
    for a,n in NAMES.items():
        if n not in missing:assert returns[n]==native['return_types'][a],(n,returns[n],native['return_types'][a])
    from unify_scalar_helpers import shared_recovered_names
    assert set(folded)=={'damage_target_creature_or_player_2022','prevent_all_damage_2022'}-shared_recovered_names()
    def targets(code):
        return {i.operands[0].imm for i in code if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}|literal_register_call_targets(code)
    for inline in inlines:
        parent=inline['parent']
        for lo,hi in inline['ranges']:assert symbols[parent]<=lo<hi<=ranges[(parent,symbols[parent])]
    flags=[i for i in inlines if i['name']=='check_special_flags_2022']
    assert len(flags)==1 and flags[0]['parent']==NAMES[WORKER] and len(flags[0]['ranges'])==2
    flag_code=[i for i in bodies[NAMES[WORKER]] if any(lo<=i.address<hi for lo,hi in flags[0]['ranges'])]
    assert coff('get_card_instance')[0] in targets(flag_code)
    last=flag_code[-1]
    assert last.mnemonic=='mov' and last.operands[0].type==X86_OP_REG and last.operands[1].type==X86_OP_MEM and last.operands[1].mem.disp==44
    consumer=next(i for i in bodies[NAMES[WORKER]] if i.address==last.address+last.size)
    assert consumer.mnemonic=='test' and consumer.operands[0].type==X86_OP_MEM and consumer.operands[0].mem.base==last.operands[0].reg and consumer.operands[0].mem.disp==180 and consumer.operands[1].type==X86_OP_IMM and consumer.operands[1].imm==64
    if missing:
        instances=[i for i in inlines if i['name']==NAMES[WORKER]]
        assert {i['parent'] for i in instances}=={NAMES[MY],NAMES[DEALT_LEGACY]} and len(instances)==2
        for instance in instances:
            code=[i for i in bodies[instance['parent']] if any(lo<=i.address<hi for lo,hi in instance['ranges'])]
            assert coff('get_card_instance')[0] in targets(code)
            assert any(o.type==X86_OP_IMM and o.imm==0x140000 for i in code for o in i.operands)
    dependencies={}
    for a,detail in component['functions'].items():
        n=NAMES[a]
        if n in missing:continue
        owner=physical[n];found=targets(bodies[n]);dependencies[n]={}
        for dep in dict.fromkeys(c['name'] for c in detail['canonical_calls']):
            instances=[x for x in inlines if x['parent']==owner and x['name']==dep]
            if dep in missing:
                assert instances,(n,dep);dependencies[n][dep]='verified inline instance';continue
            if not any(s[0]=='_'+dep for s in debug.symbols):
                assert instances,(n,dep);dependencies[n][dep]='verified inline instance';continue
            at=coff(dep)[0];possible={at}
            if dep in folded:possible.add(int(folded[dep]['target'],16))
            assert possible & found or instances,(n,dep,[hex(t) for t in found])
            dependencies[n][dep]=[hex(t) for t in sorted(possible & found)] if possible & found else 'verified inline instance'
    callbacks={}
    for a,detail in component['functions'].items():
        for site in detail['callback_references']:
            n=NAMES[a];dep=site['name'];at=coff(dep)[0];possible={at}
            if dep in folded:possible.add(int(folded[dep]['target'],16))
            assert any(o.type==X86_OP_IMM and o.imm in possible for i in bodies[n] for o in i.operands),(n,dep)
            callbacks.setdefault(n,[]).append(dep)
    shield=bodies[NAMES[SHIELD]]
    counters=coff('add_counters_from_source')[0];calls=[i for i in shield if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==counters]
    assert len(calls)==1
    before=shield[:shield.index(calls[0])]
    assert any(i.mnemonic=='mov' and len(i.operands)==2 and i.operands[0].type==X86_OP_MEM and i.reg_name(i.operands[0].mem.base)=='esp' and i.operands[0].mem.disp==16 and i.operands[1].type==X86_OP_IMM and i.operands[1].imm==0 for i in before)
    # A 32-bit signed load would change shields whose truncated capacity is negative.
    assert any(i.mnemonic=='movsx' and any(o.type==X86_OP_MEM and o.size==2 and o.mem.disp==28 for o in i.operands) for i in shield)
    assert any(o.type==X86_OP_MEM and o.size==1 and o.mem.disp==284 for i in shield for o in i.operands)
    formats=[]
    for i in shield:
        for o in i.operands:
            if o.type!=X86_OP_IMM or not pe.OPTIONAL_HEADER.ImageBase<=o.imm<pe.OPTIONAL_HEADER.ImageBase+pe.OPTIONAL_HEADER.SizeOfImage:continue
            if pe.get_data(o.imm-pe.OPTIONAL_HEADER.ImageBase,8)==b'%s (%d)\0':formats.append(dict(address=hex(o.imm),instruction=hex(i.address)))
    assert len(formats)==1
    assert 51 in {o.imm for i in shield for o in i.operands if o.type==X86_OP_IMM}
    assert coff('dispatch_event_arbitrary_to_one_card')[0] in targets(bodies[NAMES[NEXT]])
    fog=bodies[NAMES[FOG_LEGACY]]
    assert 0x2efd in {o.imm for i in fog for o in i.operands if o.type==X86_OP_IMM}
    assert coff('damage_creature')[0] in targets(bodies[NAMES[DAMAGE]]) and 0x4a68a0 in targets(bodies[NAMES[DAMAGE]])
    return dict(public_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in NAMES if a not in PRIVATE},
                private_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in PRIVATE if NAMES[a] in symbols},
                standalone_bodies=len(bodies)-len(folded),folded_thunks=folded,fully_inlined_private_helpers=missing,
                return_types=returns,records=records,enum_values=enums,signed_fields=signed,resolved_dependencies=dependencies,callbacks=callbacks,
                inline_instances=[{**x,'ranges':[[hex(lo),hex(hi)] for lo,hi in x['ranges']]} for x in inlines],
                shield_counter_call=dict(address=hex(calls[0].address),target=hex(counters),stack_words=6,source_aware=True),
                shield_format=formats[0],shield_capacity_signed_bits=16,shield_redraw_byte_offset=284,moonmist_csvid=0x2efd,
                typed_exe_boundary='0x4a68a0')
