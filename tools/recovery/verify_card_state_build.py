"""Inspect rebuilt card-state helpers without loading the DLL."""
from inspect_release import value,inherited_value


def verify(debug,pe,interfaces):
    component=interfaces.get('card_state_native_2022')
    if not component:return {}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG
    from export_abi import type_of,type_name
    from recover_card_state import NAMES,TOKEN,X_SPELL,SICK,ANIMATED_SICK,ADD_STATE,REMOVE_STATE,TAP,UNTAP,CREATURE_DEAD,BASIC,BASIC_ID,MANA_LAND,DATA,NO_MANA
    from card_state_evidence import accessed_records
    from verify_native_build import literal_register_call_targets
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    native=component['native_storage'];symbols={};ranges={};bodies={};returns={};inlines=[];physical_returns={}
    def coff(n):
        ss=[s for s in debug.symbols if s[0]=='_'+n];assert len(ss)==1,(n,ss)
        _,off,sec,kind,storage=ss[0]
        return (off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off),sec,kind,storage
    for a,n in NAMES.items():
        at,sec,kind,storage=coff(n);assert sec>0 and kind==32 and storage==2,(n,sec,kind,storage)
        symbols[n]=at
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/functions.c','functions/deck.c','functions/play_spell_for_free.c'}:continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name');lo=value(d,'DW_AT_low_pc')
            if d.tag=='DW_TAG_subprogram' and lo is not None:
                hi=d.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value;ranges[(n,lo)]=end
                physical_returns[(n,lo)]=type_name(type_of(d))
                if n in symbols:
                    assert lo==symbols[n];returns[n]=type_name(type_of(d))
                    bodies[n]=list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo),lo))
            if d.tag=='DW_TAG_inlined_subroutine':
                p=d.get_parent()
                while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
                owner=inherited_value(p,'DW_AT_name') if p is not None else None
                if owner in symbols:
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
    records,enums=accessed_records(debug);assert records==native['records'] and enums==native['enum_values']
    folded={};physical={n:n for n in bodies}
    for n in set(symbols)-set(bodies):
        at=symbols[n];insns=list(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,5),at))
        assert len(insns)==1 and insns[0].mnemonic=='jmp' and insns[0].operands[0].type==X86_OP_IMM,(n,[(i.mnemonic,i.op_str) for i in insns])
        target=insns[0].operands[0].imm
        candidates=[owner for owner,lo in ranges if lo==target and (owner==n.removesuffix('_2022') or owner in symbols)]
        assert len(candidates)==1,(n,hex(target),candidates)
        owner=candidates[0];end=ranges[(owner,target)];physical[n]=owner
        if owner in symbols:
            assert {owner,n}=={NAMES[SICK],NAMES[ANIMATED_SICK]}
            from card_state_bodies import BODIES
            assert BODIES[SICK].replace(NAMES[SICK],'sickness')==BODIES[ANIMATED_SICK].replace(NAMES[ANIMATED_SICK],'sickness')
        bodies[n]=list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target),target))
        returns[n]=physical_returns[(owner,target)]
        folded[n]=dict(target_name=owner,target=hex(target),bytes=insns[0].bytes.hex(),target_length=end-target)
    assert set(bodies)==set(symbols)
    from unify_scalar_helpers import shared_recovered_names
    expected_folds={'is_animated_and_sick_2022','add_status_2022','remove_status_2022',
                    'untap_card_no_event_2022','get_card_data_2022','has_activation_on_stack_2022'}
    assert set(folded)==expected_folds-shared_recovered_names()
    assert returns=={NAMES[a]:t for a,t in native['return_types'].items()},returns
    for inline in inlines:
        parent=inline['parent']
        for lo,hi in inline['ranges']:assert symbols[parent]<=lo<hi<=ranges[(parent,symbols[parent])]
    def targets(code):
        return {i.operands[0].imm for i in code if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}|literal_register_call_targets(code)
    dependencies={
        TOKEN:['get_card_instance','check_special_flags_2022'],
        X_SPELL:[NAMES[DATA],'check_special_flags2_2022'],
        SICK:['is_what_2022','check_special_flags2_2022'],
        ANIMATED_SICK:['is_what_2022','check_special_flags2_2022'],
        ADD_STATE:['in_play','is_what_2022','get_card_instance'],
        REMOVE_STATE:['in_play','is_what_2022','get_card_instance'],
        TAP:['get_card_instance','dispatch_event'],
        UNTAP:['get_card_instance','dispatch_event','dispatch_event_with_attacker_to_one_card'],
        CREATURE_DEAD:[NAMES[DATA],'in_play','can_regenerate'],
        BASIC:[NAMES[BASIC_ID],'get_id'],MANA_LAND:['get_card_instance','is_what_2022'],
        NO_MANA:['get_id'],DATA:['get_card_instance'],
    }
    resolved={}
    for a,names in dependencies.items():
        n=NAMES[a];found=targets(bodies[n]);resolved[n]={}
        for dep in names:
            at=coff(dep)[0];possible={at}
            if dep in folded:possible.add(int(folded[dep]['target'],16))
            instances=[x for x in inlines if x['parent']==physical[n] and x['name']==dep]
            assert possible & found or instances,(n,dep,[hex(t) for t in found])
            resolved[n][dep]=[hex(t) for t in sorted(possible & found)] if possible & found else 'verified inline instance'
    flags=[]
    for inline in inlines:
        if inline['name'] not in {'check_special_flags_2022','check_special_flags2_2022'}:continue
        owner=inline['parent'];code=[i for i in bodies[owner] if any(lo<=i.address<hi for lo,hi in inline['ranges'])]
        assert coff('get_card_instance')[0] in targets(code)
        field=180 if inline['name']=='check_special_flags_2022' else 184
        disps={o.mem.disp for i in code for o in i.operands if o.type==X86_OP_MEM and o.mem.base}
        assert 44 in disps
        result=dict(name=inline['name'],parent=owner,ranges=[[hex(lo),hex(hi)] for lo,hi in inline['ranges']],extension_pointer_offset=44,flag_offset=field)
        if not any(field<=d<field+4 for d in disps):
            last=code[-1];following=next(i for i in bodies[owner] if i.address==last.address+last.size)
            assert last.mnemonic=='mov' and last.operands[0].type==X86_OP_REG and last.operands[1].type==X86_OP_MEM and last.operands[1].mem.disp==44
            reads=[o for o in following.operands if o.type==X86_OP_MEM and o.mem.base==last.operands[0].reg and field<=o.mem.disp<field+4]
            assert len(reads)==1 and following.mnemonic in {'test','mov','and'},(inline,following.mnemonic,following.op_str)
            result['result_consumer']=dict(address=hex(following.address),bytes=following.bytes.hex(),field_offset=reads[0].mem.disp)
        flags.append(result)
    # Once both sickness spellings are shared aliases, GCC emits each
    # recovered definition directly, including its own verified flag inline.
    own_animated=NAMES[ANIMATED_SICK] in shared_recovered_names()
    expected_parents={NAMES[TOKEN],NAMES[X_SPELL],NAMES[SICK]}
    if own_animated:expected_parents.add(NAMES[ANIMATED_SICK])
    assert {x['parent'] for x in flags}==expected_parents
    assert len(flags)==3+own_animated and sum(len(x['ranges']) for x in flags)==4+own_animated,flags
    assert sum('result_consumer' in x for x in flags)==2+own_animated
    x_code=bodies[NAMES[X_SPELL]]
    assert any(i.mnemonic=='and' and i.operands[0].type==X86_OP_REG and
               i.operands[1].type==X86_OP_IMM and i.operands[1].imm==0x20000 for i in x_code)
    # Exact masks may be tested at byte granularity by GCC.
    for a in [ADD_STATE,REMOVE_STATE]:
        expected=2 if a==ADD_STATE else 0x47
        assert expected in {o.imm for i in bodies[NAMES[a]] for o in i.operands if o.type==X86_OP_IMM}
    assert 99 in {o.imm for i in bodies[NAMES[CREATURE_DEAD]] for o in i.operands if o.type==X86_OP_IMM}
    assert coff('dispatch_event')[0]==0x4359b0
    untap=bodies[NAMES[UNTAP]]
    assert any(i.mnemonic=='test' and any(o.type==X86_OP_IMM and o.imm==4 for o in i.operands) for i in untap)
    assert coff('dispatch_event_with_attacker_to_one_card')[0] in targets(untap)
    return dict(public_symbols={n:hex(a) for n,a in symbols.items()},standalone_bodies=len(symbols)-len(folded),folded_thunks=folded,
                return_types=returns,records=records,enum_values=enums,resolved_dependencies=resolved,
                inline_flag_queries=flags,inline_instances=[{**x,'ranges':[[hex(lo),hex(hi)] for lo,hi in x['ranges']]} for x in inlines],
                no_mana_cost_ids=native['no_mana_cost_classifier']['accepted_ids'],creature_death_result=99,
                bulk_add_type=2,bulk_remove_type=0x47,untap_dispatch_targets=['0x4359b0',hex(coff('dispatch_event_with_attacker_to_one_card')[0])])
