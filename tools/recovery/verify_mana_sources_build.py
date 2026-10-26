"""Inspect rebuilt mana source/control routines and the bit-count argument flow."""
from inspect_release import value,inherited_value


def verify(debug,pe,interfaces):
    if not interfaces.get('mana_source_native_2022'):
        return {}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG,X86_REG_ESP,X86_REG_EAX,X86_REG_ECX,X86_REG_EDX
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_mana_sources import NAMES,ADDRESSES,TAP,GRANT_ONE,GRANT,ENCHANTED,QUERY,LANDS,CREATURE,CREATURE_MULTI,TWO,SAC,SAC_TAPPED,PRODUCER,TAPPED,EXE_TARGETS
    from mana_source_evidence import ENUMS,LITERALS
    from verify_native_build import literal_register_call_targets
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    symbols={};bodies={};ranges={};types={};enums={};card_data={}
    def coff(n):
        matches=[s for s in debug.symbols if s[0]=='_'+n];assert len(matches)==1,(n,matches)
        _,off,sec,kind,st=matches[0]
        return (off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off),sec,kind,st
    for a,n in NAMES.items():
        at,sec,kind,st=coff(n);assert sec>0 and kind==32 and st==2
        symbols[n]=at
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/produce_mana.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_typedef' and n=='card_data_t':
                t=unqual(type_of(d));card_data['bytes']=value(t,'DW_AT_byte_size')
                for member in t.iter_children():
                    if member.tag=='DW_TAG_member' and value(member,'DW_AT_name')=='id':
                        card_data['id_offset']=value(member,'DW_AT_data_member_location');card_data['id_bytes']=value(unqual(type_of(member)),'DW_AT_byte_size')
            if d.tag=='DW_TAG_subprogram' and value(d,'DW_AT_low_pc') is not None:
                lo=value(d,'DW_AT_low_pc');hi=d.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value
                ranges[(n,lo)]=end
                if n in set(NAMES.values())|{'single_color_test_bit_to_color_2022'}:
                    assert lo==coff(n)[0];bodies[n]=list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo),lo))
            if d.tag!='DW_TAG_variable' or n!='td':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            owner=inherited_value(p,'DW_AT_name') if p is not None else None
            if owner in {NAMES[a] for a in [TAP,GRANT_ONE,GRANT,ENCHANTED]}:types[owner]=value(unqual(type_of(d)),'DW_AT_byte_size')
    assert set(NAMES.values())<=set(bodies) and enums==ENUMS
    assert types=={NAMES[a]:84 for a in [TAP,GRANT_ONE,GRANT,ENCHANTED]}
    assert card_data==dict(bytes=72,id_offset=36,id_bytes=2)
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}|literal_register_call_targets(insns)
    dependencies={GRANT_ONE:['produce_mana_tapped_all_one_color_with_default_impl','declare_mana_available_any_combination_of_colors_impl_2022'],GRANT:['produce_mana_tapped_impl'],CREATURE:['produce_mana_tapped_impl',NAMES[CREATURE]],CREATURE_MULTI:['produce_mana_tapped_multi_impl',NAMES[CREATURE_MULTI]],TWO:['produce_mana_tapped2_impl','declare_mana_available_maybe_hex_2022','single_color_test_bit_to_color_2022',NAMES[TWO]],SAC:['produce_mana_tapped2_impl','mana_producer_fixed_2022','produce_mana_tapped_all_one_color_with_default_impl'],SAC_TAPPED:[NAMES[SAC],'is_humiliated_for_ability'],TAPPED:[NAMES[PRODUCER],'is_humiliated_for_ability'],ENCHANTED:[NAMES[PRODUCER],'produce_mana_tapped_all_one_color_with_default_impl','declare_mana_available_maybe_hex_2022','single_color_test_bit_to_color_2022'],PRODUCER:['produce_mana_tapped_impl','produce_mana_tapped_all_one_color_with_default_impl','declare_mana_available_maybe_hex_2022','single_color_test_bit_to_color_2022']}
    for a,names in dependencies.items():assert {coff(n)[0] for n in names}<=targets(bodies[NAMES[a]]),(a,names)
    assert set(EXE_TARGETS)<=set().union(*(targets(bodies[NAMES[a]]) for a in ADDRESSES))
    query_word=[dict(address=hex(i.address),bytes=i.bytes.hex()) for i in bodies[NAMES[QUERY]] if i.mnemonic=='mov' and len(i.operands)==2 and i.operands[1].type==X86_OP_MEM and i.operands[1].mem.base and i.operands[1].mem.disp==36 and i.operands[1].size==4]
    assert query_word
    # Follow the final popcount result to the amount stack slot across the
    # single-color helper. Inspect that callee's real register writes instead
    # of assuming it destroys every caller-saved register.
    producer=bodies[NAMES[PRODUCER]];popcount=coff('num_bits_set')[0];single=coff('single_color_test_bit_to_color_2022')[0];produce=coff('produce_mana_tapped_impl')[0]
    at=next(j for j,i in enumerate(producer) if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==produce)
    start=max(j for j,i in enumerate(producer[:at]) if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==popcount)
    window=producer[start+1:at];assert not any(i.mnemonic.startswith('j') for i in window)
    tainted={X86_REG_EAX};amount=None;single_calls=[]
    for i in window:
        ops=i.operands
        if i.mnemonic=='call':
            assert ops[0].type==X86_OP_IMM and ops[0].imm==single
            writes=set().union(*(set(x.regs_access()[1]) for x in bodies['single_color_test_bit_to_color_2022']))
            tainted-=(writes & {X86_REG_EAX,X86_REG_ECX,X86_REG_EDX});single_calls.append(hex(i.address))
        elif i.mnemonic=='mov' and len(ops)==2:
            has=ops[1].type==X86_OP_REG and ops[1].reg in tainted
            if ops[0].type==X86_OP_REG:
                tainted.discard(ops[0].reg)
                if has:tainted.add(ops[0].reg)
            elif ops[0].type==X86_OP_MEM and ops[0].mem.base==X86_REG_ESP and ops[0].mem.disp==12:
                assert has;amount=dict(address=hex(i.address),bytes=i.bytes.hex(),stack_offset=12)
        else:
            tainted-=set(i.regs_access()[1])
    assert amount and len(single_calls)==1
    for a in [CREATURE,CREATURE_MULTI]:assert any(i.mnemonic=='idiv' for i in bodies[NAMES[a]])
    strings={}
    for a in ADDRESSES:
        for ins in bodies[NAMES[a]]:
            for op in ins.operands:
                if op.type!=X86_OP_IMM:continue
                rva=op.imm-pe.OPTIONAL_HEADER.ImageBase;section=pe.get_section_by_rva(rva) if rva>=0 else None
                if section is None or section.Name.rstrip(b'\0')!=b'.rdata':continue
                text=pe.get_data(rva,512).split(b'\0')[0]
                if text in LITERALS:strings[hex(op.imm)]=text.decode('ascii')
    assert set(strings.values())=={x.decode('ascii') for x in LITERALS}
    return dict(public_symbols={n:hex(at) for n,at in symbols.items()},target_definition_sizes=types,card_data=card_data,enum_values=enums,literals=strings,query_id_word_reads=query_word,
        bitcount_amount=dict(popcount_call=hex(producer[start].address),single_color_calls=single_calls,amount_store=amount,producer_call=hex(producer[at].address)),dependencies={NAMES[a]:names for a,names in dependencies.items()},signed_ai_rating_division=True,exe_boundaries=[hex(a) for a in EXE_TARGETS])
