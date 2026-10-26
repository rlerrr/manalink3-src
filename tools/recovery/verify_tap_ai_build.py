"""Inspect rebuilt tap/untap, ability-event suppression and AI-scoring helpers."""
from inspect_release import value,inherited_value


def verify(debug,pe,interfaces):
    if not interfaces.get('tap_ai_native_2022'):
        return {}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_tap_ai import NAMES,PRIVATE,PUBLIC_HELPERS,HAS,SELECT,ENTER,DISABLE,LAND,HASTE,DRAW,TREASON,AOT,TAP,UNTAP,TWIDDLE,TWIDDLE_AI,UPKEEP,DURING,CHOOSING,CHOOSE,LANDS,SICK,COMBAT,EXE_TARGETS
    from tap_ai_evidence import ENUMS
    from color_hack_evidence import layout
    from verify_native_build import literal_register_call_targets
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    wanted=set(NAMES.values());symbols={};bodies={};ranges={};objects={};enums={};fields={}
    native=interfaces['tap_ai_native_2022']['native_storage']
    worker='type_text_2022_impl';worker_object=None
    def coff(n):
        ss=[s for s in debug.symbols if s[0]=='_'+n];assert len(ss)==1,(n,ss)
        _,off,sec,kind,st=ss[0]
        return (off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off),sec,kind,st
    for a,n in NAMES.items():
        at,sec,kind,st=coff(n);assert sec>0 and kind==32 and st==(3 if a in PRIVATE else 2),(n,sec,kind,st)
        symbols[n]=at
    expected_enums={**ENUMS};expected_enums['STATE_SICKNESS_UNUSED']=expected_enums.pop('STATE_SUMMON_SICK_BACKUP')
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in expected_enums:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_subprogram' and value(d,'DW_AT_low_pc') is not None:
                lo=value(d,'DW_AT_low_pc');hi=d.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value;ranges[(n,lo)]=end
                if n in wanted:
                    assert lo==symbols[n];bodies[n]=list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo),lo))
            if d.tag!='DW_TAG_variable':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            owner=inherited_value(p,'DW_AT_name') if p is not None else None
            if owner==worker and n=='types':worker_object=layout(d)
            key=next((a+'::'+n for a in NAMES if NAMES[a]==owner and a+'::'+n in native['local_objects']),None)
            if key is not None:
                objects[key]=layout(d)
                if n=='td':fields={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in unqual(type_of(d)).iter_children() if c.tag=='DW_TAG_member'}
    assert enums==expected_enums
    assert worker_object==dict(bounds=[16],element_bytes=4)
    # The original UPKEEP's inline text array is now in the exact recovered
    # shared worker. Its own concrete bounds and this caller are checked below.
    objects[UPKEEP+'::types']=worker_object
    assert objects==native['local_objects'] and fields==native['target_layout']['members']
    folded={}
    for n in set(symbols)-set(bodies):
        old=n.removesuffix('_2022');target=coff(old)[0];at=symbols[n];end=ranges[(old,target)]
        insns=list(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,5),at))
        assert len(insns)==1 and insns[0].mnemonic=='jmp' and insns[0].operands[0].type==X86_OP_IMM and insns[0].operands[0].imm==target,(n,insns)
        bodies[n]=list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target),target));folded[n]=dict(retained_target=old,target=hex(target),bytes=insns[0].bytes.hex())
    from unify_scalar_helpers import shared_recovered_names
    assert set(folded)=={NAMES[a] for a in [LAND,HASTE,TWIDDLE_AI,CHOOSING,COMBAT]}-shared_recovered_names()
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}|literal_register_call_targets(insns)
    dependencies={HAS:['has_an_activated_ability'],SELECT:['can_target_2022','new_pick_target_2022','new_card_from_list_2022','get_card_name_2022','ai_name_a_random_csvid_2022'],ENTER:['create_card_name_legacy_2022'],DISABLE:['is_humiliated_for_ability','action_on_card_2022'],DRAW:['is_humiliated_for_ability','get_total_available_lands'],TREASON:['check_for_special_ability_2022'],AOT:['generic_spell_2022','valid_target_2022',NAMES[TREASON]],TAP:['check_for_special_ability_2022'],UNTAP:['can_use_activated_abilities_2022'],TWIDDLE:['load_text_2022','dialog_impl'],UPKEEP:['default_target_definition_2022','can_target_2022','pick_next_target_noload_2022',worker],DURING:['manipulate_all_2022'],CHOOSE:['set_special_flags2_2022'],LANDS:['default_target_definition_2022','can_target_2022','select_target_2022']}
    resolved={}
    # Qualified get_card_name is a verified five-byte jump to its retained
    # counterpart. GCC may call that known target directly after ICF.
    extra_folded={}
    n='get_card_name_2022';at=coff(n)[0];insns=list(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,5),at))
    if n in shared_recovered_names():
        assert at==coff('get_card_name')[0]
    else:
        assert len(insns)==1 and insns[0].mnemonic=='jmp' and insns[0].operands[0].type==X86_OP_IMM and insns[0].operands[0].imm==coff('get_card_name')[0]
        extra_folded[n]=insns[0].operands[0].imm
    for a,nn in dependencies.items():
        found=targets(bodies[NAMES[a]]);resolved[NAMES[a]]={}
        for n in nn:
            possible={coff(n)[0]}
            if n in folded:possible.add(int(folded[n]['target'],16))
            if n in extra_folded:possible.add(extra_folded[n])
            assert possible & found,(a,n,possible,found)
            resolved[NAMES[a]][n]=[hex(x) for x in sorted(possible & found)]
    callback=symbols[NAMES[HAS]]
    assert any(i.mnemonic=='mov' and len(i.operands)==2 and i.operands[0].type==X86_OP_MEM and i.operands[0].mem.disp==40 and i.operands[1].type==X86_OP_IMM and i.operands[1].imm==callback for i in bodies[NAMES[SELECT]])
    assert set(EXE_TARGETS)<=targets(bodies[NAMES[SELECT]])
    assert targets(bodies[NAMES[ENTER]])=={coff(n)[0] for n in ['get_card_instance','create_card_name_legacy_2022']}
    imms=lambda a:{o.imm & 0xffffffff for i in bodies[NAMES[a]] for o in i.operands if o.type==X86_OP_IMM}
    assert {0x20000,0xfffcffff}<=imms(SICK)
    assert 52 in imms(DISABLE) and 108 not in imms(DISABLE)
    assert 21504 in imms(AOT) and 77 in imms(DURING)
    # Recover the unchanged eighteen-word variadic frame in rebuilt x86 code.
    insns=bodies[NAMES[TWIDDLE]];dialog=next(i for i in insns if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==coff('dialog_impl')[0])
    writes={}
    for i in insns:
        if i.address>=dialog.address:break
        if i.mnemonic=='mov' and len(i.operands)==2 and i.operands[0].type==X86_OP_MEM and i.reg_name(i.operands[0].mem.base)=='esp' and i.operands[1].type==X86_OP_IMM:writes[i.operands[0].mem.disp]=i.operands[1].imm
    frame={int(k):v for k,v in native['twiddle_dialog']['constant_words'].items()}
    controls={12:b'\x01\x02',16:b'\x01\x0c',20:b'\x01\x07'}
    for offset,expected in controls.items():
        at=writes[offset];actual=pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,3);assert actual==expected+b'\0'
    assert all(writes.get(k)==v for k,v in frame.items() if k not in controls),writes
    worker_at,sec,kind,st=coff(worker);assert sec>0 and kind==32 and st==3
    worker_end=ranges[(worker,worker_at)]
    return dict(public_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in PUBLIC_HELPERS},private_symbols={NAMES[HAS]:hex(callback)},folded_public_thunks=folded,local_objects=objects,target_layout=native['target_layout'],enum_values=enums,source_state_alias=native['source_state_alias'],resolved_dependencies=resolved,ai_target_predicate=dict(address=hex(callback),extra_offset=40),ability_event=52,enter_creates_label_only=True,clear_sickness_mask='0xfffcffff',twiddle_dialog=dict(call=hex(dialog.address),stack_words=18,control_strings={str(k):v.hex() for k,v in controls.items()}),shared_text_worker=dict(name=worker,address=hex(worker_at),length=worker_end-worker_at,caller=NAMES[UPKEEP],array=worker_object),typed_exe_boundaries=[hex(x) for x in EXE_TARGETS],chooser_ui_still_deferred=True)
