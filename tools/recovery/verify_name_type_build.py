"""Inspect rebuilt name/action APIs, callbacks, folded entries and text workers."""
from inspect_release import value,inherited_value


def verify(debug,pe,interfaces):
    if not interfaces.get('name_type_native_2022'):
        return {}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM
    from recover_name_type import NAMES,PRIVATE,PUBLIC_HELPERS,GET_BY_ID,GET_BY_INSTANCE,GET_OR_SUBTYPE,SUBTYPE_LEGACY,RFG_LEGACY,CARD_LEGACY,HARD_SUBTYPE,CREATE_SUBTYPE,CREATE_RFG,GET,SAME,DAMAGE,TUTOR,COUNT,RANDOM,NAME,ALT_IMPL,ALT,TYPE_IMPL,TYPE
    from name_type_evidence import ENUMS
    from color_hack_evidence import layout
    from verify_native_build import literal_register_call_targets
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    wanted=set(NAMES.values());symbols={};bodies={};ranges={};objects={};enums={};inline=[]
    native=interfaces['name_type_native_2022']['native_storage']
    inlined={ALT_IMPL:ALT,TYPE_IMPL:TYPE}
    # Pass 132 adds a second real caller of the same recovered text worker.
    # GCC now outlines it instead of inlining its sole call. The earlier
    # source/ledger remains exact; verify the changed compiler representation.
    outlined_type=bool(interfaces.get('tap_ai_native_2022'))
    # Pass 136 adds another caller of the alternate-text worker as well.
    # Verify its actual outline and both call sites; keep the old source and
    # recovery records intact rather than requiring the earlier inline form.
    outlined_alt=bool(interfaces.get('damage_prevention_native_2022'))
    outlined_workers=({TYPE_IMPL} if outlined_type else set())|({ALT_IMPL} if outlined_alt else set())
    def coff(n):
        ss=[s for s in debug.symbols if s[0]=='_'+n];assert len(ss)==1,(n,ss)
        _,off,sec,kind,st=ss[0]
        return (off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off),sec,kind,st
    for a,n in NAMES.items():
        if a in inlined and a not in outlined_workers:
            assert not any(s[0]=='_'+n for s in debug.symbols)
            continue
        at,sec,kind,st=coff(n);assert sec>0 and kind==32 and st==(3 if a in PRIVATE else 2),(n,sec,kind,st)
        symbols[n]=at
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_subprogram' and value(d,'DW_AT_low_pc') is not None:
                lo=value(d,'DW_AT_low_pc');hi=d.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value;ranges[(n,lo)]=end
                if n in wanted:
                    assert lo==symbols[n];bodies[n]=list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo),lo))
            if d.tag=='DW_TAG_inlined_subroutine' and n in {NAMES[a] for a in inlined}:
                a=next(a for a in inlined if NAMES[a]==n);parent=NAMES[inlined[a]]
                p=d.get_parent()
                while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
                assert inherited_value(p,'DW_AT_name')==parent
                origin=d.get_DIE_from_attribute('DW_AT_abstract_origin')
                ps=[inherited_value(c,'DW_AT_name') for c in origin.iter_children() if c.tag=='DW_TAG_formal_parameter']
                assert ps==(['textnum','player','legacycard'] if a==ALT_IMPL else ['buf','maxlen','type','flags'])
                assert 'DW_AT_type' not in origin.attributes # void worker; incidental EAX is not consumed
                lo=value(d,'DW_AT_low_pc');spans=[]
                if lo is not None:
                    hi=d.attributes['DW_AT_high_pc'];spans=[(lo,hi.value if hi.form=='DW_FORM_addr' else lo+hi.value)]
                else:
                    base=value(cu.get_top_DIE(),'DW_AT_low_pc')
                    for r in debug.dwarf().range_lists().get_range_list_at_offset(value(d,'DW_AT_ranges'),cu=cu):
                        if hasattr(r,'base_address'):base=r.base_address
                        else:
                            lo=r.begin_offset+(0 if r.is_absolute else base);end=r.end_offset+(0 if r.is_absolute else base)
                            if end>lo:spans.append((lo,end))
                assert spans;inline.append(dict(name=n,parent=parent,ranges=spans,parameters=ps))
            if d.tag!='DW_TAG_variable':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            owner=inherited_value(p,'DW_AT_name') if p is not None else None
            key=next((a+'::'+n for a in NAMES if NAMES[a]==owner and a+'::'+n in native['local_objects']),None)
            if key is not None:objects[key]=layout(d)
    assert objects==native['local_objects'] and enums==ENUMS,(objects,enums)
    folded={};missing_dwarf={}
    for n in set(symbols)-set(bodies):
        old=n.removesuffix('_2022');target=coff(old)[0];at=symbols[n];end=ranges[(old,target)]
        if n==NAMES[GET_BY_ID]:
            expected=pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target)
            actual=pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,end-target);assert actual==expected
            insns=list(decoder.disasm(actual,at));assert insns[-1].mnemonic=='ret' and not any(i.mnemonic=='call' or i.mnemonic.startswith('j') for i in insns)
            missing_dwarf[n]=dict(retained_equivalent=old,address=hex(at),length=len(actual),bytes=actual.hex());bodies[n]=insns
            continue
        insns=list(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,5),at))
        assert len(insns)==1 and insns[0].mnemonic=='jmp' and insns[0].operands[0].type==X86_OP_IMM and insns[0].operands[0].imm==target,(n,insns)
        bodies[n]=list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target),target));folded[n]=dict(retained_target=old,target=hex(target),bytes=insns[0].bytes.hex())
    from unify_scalar_helpers import shared_recovered_names
    shared=shared_recovered_names()
    expected_folds={NAMES[a] for a in [GET_BY_INSTANCE,GET_OR_SUBTYPE,GET,SAME]}-shared
    if {NAMES[SUBTYPE_LEGACY],NAMES[RFG_LEGACY]}<=shared:
        expected_folds|={NAMES[CREATE_SUBTYPE],NAMES[CREATE_RFG]}-shared
    assert set(folded)==expected_folds,folded
    assert set(missing_dwarf)=={NAMES[GET_BY_ID]}-shared
    shared_getter={}
    if NAMES[GET_BY_ID] in shared:
        # Its retained duplicate formerly owned the sole DWARF body. With a
        # source alias, the recovered name now owns that complete range.
        n=NAMES[GET_BY_ID];at=symbols[n];end=ranges[(n,at)]
        assert coff(n.removesuffix('_2022'))[0]==at
        code=bodies[n]
        assert sum(i.size for i in code)==end-at and code[-1].mnemonic=='ret'
        assert not any(i.mnemonic=='call' or i.mnemonic.startswith('j') for i in code)
        from hashlib import sha256
        shared_getter=dict(name=n,address=hex(at),length=end-at,sha256=sha256(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,end-at)).hexdigest(),
                           representation='same-address alias with complete recovered-name DWARF range')
    expected_inline=set(inlined)-outlined_workers
    assert len(inline)==len(expected_inline) and {x['name'] for x in inline}=={NAMES[a] for a in expected_inline}
    for instance in inline:
        parent=instance['parent'];lo=symbols[parent];end=ranges[(parent,lo)]
        assert all(lo<=a<b<=end for a,b in instance['ranges'])
        instance['ranges']=[[hex(a),hex(b)] for a,b in instance['ranges']]
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}|literal_register_call_targets(insns)
    outlines={}
    if outlined_type:
        at=symbols[NAMES[TYPE_IMPL]];end=ranges[(NAMES[TYPE_IMPL],at)]
        assert at in targets(bodies[NAMES[TYPE]])
        assert {coff(n)[0] for n in ['load_text_2022','sort_ArrayConstCharPtr','scnprintf']}<=targets(bodies[NAMES[TYPE_IMPL]])
        outlines[NAMES[TYPE_IMPL]]=dict(address=hex(at),length=end-at,public_caller=NAMES[TYPE],reason='Second recovered caller prevents sole-call inlining.')
    if outlined_alt:
        from recover_damage_prevention import NAMES as damage_names,NEGATE_COMBAT
        at=symbols[NAMES[ALT_IMPL]];end=ranges[(NAMES[ALT_IMPL],at)]
        assert end-at==125 and at in targets(bodies[NAMES[ALT]])
        caller=damage_names[NEGATE_COMBAT];caller_at=coff(caller)[0];caller_end=ranges[(caller,caller_at)]
        caller_code=list(decoder.disasm(pe.get_data(caller_at-pe.OPTIONAL_HEADER.ImageBase,caller_end-caller_at),caller_at))
        assert at in targets(caller_code)
        outlines[NAMES[ALT_IMPL]]=dict(address=hex(at),length=end-at,public_caller=NAMES[ALT],second_caller=caller,reason='Second recovered caller prevents sole-call inlining.')
    # GCC splits the exile-check branch, then aliases that generated worker
    # to the retained callback's identical part. Validate the shared address
    # and its independent old DWARF bounds before following its dependencies.
    old_part='legacy_name_with_rfg_check.part.0';new_part=NAMES[RFG_LEGACY]+'.part.0'
    if NAMES[RFG_LEGACY] in shared and not any(s[0]=='_'+new_part for s in debug.symbols):
        assert not any(s[0]=='_'+old_part for s in debug.symbols)
        callback_at=symbols[NAMES[RFG_LEGACY]];callback_end=ranges[(NAMES[RFG_LEGACY],callback_at)]
        callback_code=bodies[NAMES[RFG_LEGACY]]
        assert callback_end-callback_at==247 and sum(i.size for i in callback_code)==247
        from collections import Counter
        actual=Counter(i.operands[0].imm for i in callback_code if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM)
        expected=Counter({coff('get_card_instance')[0]:2,coff('check_rfg')[0]:1,coff('kill_card')[0]:1,coff('scnprintf')[0]:1})
        assert actual==expected
        part_code=[]
        generated_part=dict(representation='unsplit callback',name=NAMES[RFG_LEGACY],address=hex(callback_at),length=247,verified_call_counts=dict(get_card_instance=2,check_rfg=1,kill_card=1,scnprintf=1))
    else:
        part_at,sec,kind,st=coff(new_part);assert (sec,kind,st)==(1,32,3)
        assert part_at in targets(bodies[NAMES[RFG_LEGACY]])
        assert part_at==coff(old_part)[0]
        part_end=ranges[('legacy_name_with_rfg_check',part_at)]
        part_code=list(decoder.disasm(pe.get_data(part_at-pe.OPTIONAL_HEADER.ImageBase,part_end-part_at),part_at))
        assert part_end-part_at==101 and part_code[-1].mnemonic=='jmp'
        generated_part=dict(name=new_part,retained_alias=old_part,address=hex(part_at),length=part_end-part_at)
    dependencies={CARD_LEGACY:['create_legacy_effect_exe_2022'],HARD_SUBTYPE:['create_legacy_effect_exe_2022'],DAMAGE:[NAMES[GET],'default_test_definition_2022','new_damage_all_2022'],TUTOR:[NAMES[GET],'new_default_test_definition_2022','new_global_tutor_2022'],COUNT:[NAMES[GET],'in_play'],RANDOM:['new_make_test_2022','count_deck','internal_rand'],NAME:['new_card_from_list_2022','new_select_a_card_2022'],TYPE:['load_text_2022','sort_ArrayConstCharPtr','scnprintf'],ALT:['backtrace_and_abort'],SUBTYPE_LEGACY:['raw_get_subtype_text'],RFG_LEGACY:['check_rfg']}
    resolved={}
    for a,nn in dependencies.items():
        found=targets(bodies[NAMES[a]])
        if a==RFG_LEGACY:found|=targets(part_code)
        if a==TYPE and outlined_type:found|=targets(bodies[NAMES[TYPE_IMPL]])
        if a==ALT and outlined_alt:found|=targets(bodies[NAMES[ALT_IMPL]])
        resolved[NAMES[a]]={}
        for n in nn:
            possible={coff(n)[0]}
            if n in folded:possible.add(int(folded[n]['target'],16))
            assert possible & found,(a,n,possible,found)
            resolved[NAMES[a]][n]=[hex(x) for x in sorted(possible & found)]
    callbacks={}
    for a,b in [(CREATE_SUBTYPE,SUBTYPE_LEGACY),(CREATE_RFG,RFG_LEGACY)]:
        target=symbols[NAMES[b]]
        assert any(o.type==X86_OP_IMM and o.imm==target for i in bodies[NAMES[a]] for o in i.operands)
        callbacks[NAMES[a]]=dict(name=NAMES[b],address=hex(target))
    strings={}
    for a in [ALT,TYPE]+([TYPE_IMPL] if outlined_type else [])+([ALT_IMPL] if outlined_alt else []):
        for i in bodies[NAMES[a]]:
            for o in i.operands:
                if o.type!=X86_OP_IMM:continue
                rva=o.imm-pe.OPTIONAL_HEADER.ImageBase
                sec=pe.get_section_by_rva(rva) if rva>=0 else None
                if sec is None or sec.Name.rstrip(b'\0')!=b'.rdata':continue
                txt=pe.get_data(rva,512).split(b'\0')[0].decode('ascii',errors='replace')
                if txt in native['strings'].values():strings[hex(o.imm)]=txt
    assert {native['strings']['275f920'],native['strings']['275f9e8'],'non',''}<=set(strings.values())
    abort=coff('backtrace_and_abort')[0];assertions=[];code=bodies[NAMES[ALT_IMPL if outlined_alt else ALT]]
    for index,i in enumerate(code):
        if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==abort:
            following=code[index+1];assert following.mnemonic not in {'ret','ud2','int3'}
            assertions.append(dict(call=hex(i.address),continuation=hex(following.address),bytes=following.bytes.hex()))
    assert len(assertions)==2
    # Two separately emitted loops both call in_play, matching native control flow.
    in_play=coff('in_play')[0]
    assert sum(i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==in_play for i in bodies[NAMES[COUNT]])==2
    assert {n:hex(coff(n)[0]) for n in native['table_addresses']}==native['table_addresses']
    return dict(public_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in PUBLIC_HELPERS},private_callback_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in [SUBTYPE_LEGACY,RFG_LEGACY]},private_worker_inline_instances=inline,private_worker_outlines=outlines,folded_public_thunks=folded,public_without_concrete_dwarf=missing_dwarf,shared_name_getter=shared_getter,generated_shared_part=generated_part,local_objects=objects,enum_values=enums,resolved_dependencies=resolved,callbacks=callbacks,strings=strings,returning_assertion_continuations=assertions,both_count_loops_check_in_play=True,table_addresses=native['table_addresses'],chooser_ui_still_deferred=True)
