"""Inspect rebuilt card type, mana-value, PT and battlefield-query helpers."""
from inspect_release import value,inherited_value


def verify(debug,pe,interfaces):
    if not interfaces.get('card_queries_native_2022'):
        return {}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_card_queries import NAMES,PRIVATE,PUBLIC_HELPERS,PLANESWALKER,WITH_IID,TYPE,IS_WHAT,PT,BASE_POWER,BASE_POWER_IID,BASE_TOUGHNESS,BASE_TOUGHNESS_IID,SPLIT,CMC_IID,CMC,HIGHEST_POWER,POWER_SUM,DEFENDERS,BY_TOUGHNESS,BY_POWER,HIGHEST_NONLAND,HIGHEST_CMC,COMMON_HAND,COMMON_NONLAND,BY_TYPE,BY_COLOR,BY_ID,CHECK_SUBTYPE,COUNT_SUBTYPE,SHRINES
    from card_query_evidence import ENUMS
    from color_hack_evidence import layout
    from verify_native_build import literal_register_call_targets
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    wanted=set(NAMES.values());symbols={};bodies={};ranges={};objects={};enums={};card_fields={};scalar_locals={};inline_flags=[]
    native=interfaces['card_queries_native_2022']['native_storage']
    def coff(n):
        ss=[s for s in debug.symbols if s[0]=='_'+n];assert len(ss)==1,(n,ss)
        _,off,sec,kind,st=ss[0]
        return (off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off),sec,kind,st
    for a,n in NAMES.items():
        at,sec,kind,st=coff(n);assert sec>0 and kind==32 and st==(3 if a in PRIVATE else 2),(n,sec,kind,st)
        symbols[n]=at
    members=set(native['card_data']['members'])
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_typedef' and n=='card_data_t':
                t=unqual(d);assert value(t,'DW_AT_byte_size')==72
                card_fields={value(c,'DW_AT_name'):dict(offset=value(c,'DW_AT_data_member_location'),**layout(c)) for c in t.iter_children() if c.tag=='DW_TAG_member' and value(c,'DW_AT_name') in members}
            if d.tag=='DW_TAG_subprogram' and value(d,'DW_AT_low_pc') is not None:
                lo=value(d,'DW_AT_low_pc');hi=d.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value;ranges[(n,lo)]=end
                if n in wanted:
                    assert lo==symbols[n];bodies[n]=list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo),lo))
            if d.tag=='DW_TAG_inlined_subroutine' and n in {'check_special_flags2_2022','check_special_flags3_2022'}:
                p=d.get_parent()
                while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
                owner=inherited_value(p,'DW_AT_name') if p is not None else None
                if owner in {NAMES[WITH_IID],NAMES[IS_WHAT],NAMES[CMC]}:
                    lo=value(d,'DW_AT_low_pc');assert lo is not None
                    hi=d.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value
                    inline_flags.append(dict(name=n,parent=owner,start=lo,end=end))
            if d.tag!='DW_TAG_variable':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            owner=inherited_value(p,'DW_AT_name') if p is not None else None
            key=next((a+'::'+n for a in NAMES if NAMES[a]==owner and a+'::'+n in native['local_objects']),None)
            if key is not None:objects[key]=layout(d)
            if owner==NAMES[CMC] and n in {'iid','result','split'}:
                t=unqual(type_of(d));assert t.tag=='DW_TAG_base_type' and value(t,'DW_AT_byte_size')==4 and value(t,'DW_AT_encoding')==5
                scalar_locals[n]=4
    # Shared is_what bindings allow GCC to fold seven now-identical recovered
    # callers into their retained bodies. Verify each actual jump and complete
    # target range, then apply the same dependency/layout checks to that body.
    folded={}
    for name in wanted-set(bodies):
        at=symbols[name]
        first=next(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,5),at,count=1))
        assert first.mnemonic=='jmp' and first.operands[0].type==X86_OP_IMM
        target=first.operands[0].imm;owner=name.removesuffix('_2022')
        assert (owner,target) in ranges,(name,owner,hex(target))
        end=ranges[(owner,target)]
        bodies[name]=list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target),target))
        folded[name]=dict(target_name=owner,target=hex(target),bytes=first.bytes.hex(),target_length=end-target)
    expected_folds={'get_power_sum_2022','count_creatures_by_toughness_2022','count_defenders_2022',
                    'count_creatures_by_power_2022','count_permanents_by_type_2022',
                    'check_battlefield_for_subtype_2022','count_subtype_2022'}
    from unify_scalar_helpers import shared_recovered_names
    shared=shared_recovered_names()
    assert set(folded)==(expected_folds-shared if 'is_what_2022' in shared else set())
    assert set(bodies)==wanted
    assert objects==native['local_objects'] and enums==ENUMS and card_fields==native['card_data']['members']
    assert scalar_locals==dict(iid=4,result=4,split=4)
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}|literal_register_call_targets(insns)
    assert len(inline_flags)==4
    for instance in inline_flags:
        parent=instance['parent'];at=symbols[parent];end=ranges[(parent,at)]
        lo=instance['start'];hi=instance['end'];assert at<=lo<hi<=end
        code=[i for i in bodies[parent] if lo<=i.address<hi]
        assert coff('get_card_instance')[0] in targets(code)
        disps={o.mem.disp for i in code for o in i.operands if o.type==X86_OP_MEM and o.mem.base}
        field=184 if instance['name']=='check_special_flags2_2022' else 188
        assert 44 in disps
        if not any(field<=off<field+4 for off in disps):
            # GCC ends the inline range after obtaining the extension pointer;
            # its caller consumes the masked result in the very next test.
            last=code[-1];following=next(i for i in bodies[parent] if i.address==hi)
            assert last.mnemonic=='mov' and last.operands[0].type==X86_OP_REG and last.operands[1].type==X86_OP_MEM and last.operands[1].mem.disp==44
            reads=[o for o in following.operands if o.type==X86_OP_MEM and o.mem.base==last.operands[0].reg and field<=o.mem.disp<field+4]
            assert len(reads)==1 and following.mnemonic=='test',(instance,following.op_str)
            instance['result_consumer']=dict(address=hex(following.address),bytes=following.bytes.hex(),field_offset=reads[0].mem.disp)
        instance['start']=hex(lo);instance['end']=hex(hi);instance['extension_pointer_offset']=44;instance['flag_offset']=field
    dependencies={WITH_IID:[NAMES[PLANESWALKER],'check_special_flags2_2022'],TYPE:[NAMES[WITH_IID]],IS_WHAT:[NAMES[PLANESWALKER]],PT:['call_card_fn'],BASE_POWER:[NAMES[PT]],BASE_POWER_IID:[NAMES[PT]],BASE_TOUGHNESS:[NAMES[PT]],BASE_TOUGHNESS_IID:[NAMES[PT]],SPLIT:['split_cards_id_table',NAMES[CMC_IID]],CMC_IID:[NAMES[SPLIT],'has_no_mana_cost'],CMC:[NAMES[CMC_IID],NAMES[SPLIT],'is_token','is_x_spell'],HIGHEST_POWER:[NAMES[IS_WHAT]],POWER_SUM:[NAMES[IS_WHAT]],DEFENDERS:[NAMES[IS_WHAT]],BY_TOUGHNESS:[NAMES[IS_WHAT]],BY_POWER:[NAMES[IS_WHAT]],HIGHEST_NONLAND:[NAMES[IS_WHAT],NAMES[CMC]],HIGHEST_CMC:[NAMES[IS_WHAT],NAMES[CMC]],COMMON_HAND:[NAMES[IS_WHAT],NAMES[CMC]],COMMON_NONLAND:[NAMES[IS_WHAT],NAMES[CMC]],BY_TYPE:[NAMES[IS_WHAT]],BY_COLOR:[NAMES[IS_WHAT],'get_color_2022'],BY_ID:['is_humiliated_for_ability'],CHECK_SUBTYPE:[NAMES[IS_WHAT]],COUNT_SUBTYPE:[NAMES[IS_WHAT]],SHRINES:['default_target_definition_2022','target_available_2022']}
    resolved={}
    for a,nn in dependencies.items():
        found=targets(bodies[NAMES[a]]);resolved[NAMES[a]]={}
        for n in nn:
            target=coff(n)[0]
            inlined=[x for x in inline_flags if x['name']==n and x['parent']==NAMES[a]]
            assert target in found or inlined,(a,n,hex(target),found)
            resolved[NAMES[a]][n]=hex(target) if target in found else 'verified inline instance'
    for a in [PT,IS_WHAT]:
        assert 23059 in {o.imm for i in bodies[NAMES[a]] for o in i.operands if o.type==X86_OP_IMM}
    # The PT callback's effects on these three global words are temporary.
    pt=bodies[NAMES[PT]];call=next(i for i in pt if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==coff('call_card_fn')[0])
    restores={}
    for n,address in [('affected_card_controller',0x738c84),('affected_card',0x7a398c),('event_result',0x6205b4)]:
        assert coff(n)[0]==address
        sites=[i for i in pt if i.address>call.address and i.mnemonic=='mov' and i.operands[0].type==X86_OP_MEM and not i.operands[0].mem.base and i.operands[0].mem.disp==address]
        assert len(sites)==1;restores[n]=dict(address=hex(sites[0].address),bytes=sites[0].bytes.hex())
    # The native split selector is an unsigned population-count test. Record
    # its mask constants and the emitted unsigned branch after subtracting one.
    split=bodies[NAMES[SPLIT]];imms={o.imm & 0xffffffff for i in split for o in i.operands if o.type==X86_OP_IMM}
    assert {0x55555555,0x33333333,0x0f0f0f0f}<=imms
    assert any(i.mnemonic in {'ja','jbe'} for i in split)
    assert sum(i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==symbols[NAMES[CMC_IID]] for i in split)==3
    cmc=bodies[NAMES[CMC]]
    x_calls=[i for i in cmc if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==coff('is_x_spell')[0]]
    assert len(x_calls)==2
    return dict(public_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in PUBLIC_HELPERS},private_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in PRIVATE},standalone_bodies=len(bodies)-len(folded),folded_helpers=folded,local_objects=objects,enum_values=enums,card_data=dict(bytes=72,members=card_fields),cmc_scalar_locals=scalar_locals,resolved_dependencies=resolved,inline_flag_queries=inline_flags,grist_csvid=23059,pt_callback=dict(call=hex(call.address),stack_words=5,context_restores=restores),split_card_recursive_sites=3,split_popcount_unsigned=True,cmc_x_spell_calls=[hex(i.address) for i in x_calls])
