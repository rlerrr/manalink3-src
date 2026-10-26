"""Inspect rebuilt type/animation/copy APIs, callbacks, layouts and call frames."""
from inspect_release import value,inherited_value


def verify(debug,pe,interfaces):
    if not interfaces.get('type_animation_native_2022'):
        return {}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG
    from export_abi import type_of,type_name
    from pack_native_tables import unqual
    from recover_type_animation import NAMES,PRIVATE,PUBLIC_FUNCTIONS,PUBLIC_HELPERS,CLEANUP,UNCOUNTERABLE,IS_ARTIFACT,GLOBAL,LEGACY,ARTIFACT,CREATURE,ANIMATE,GLOBAL_LEGACY,REAL_LAND,LAND2,CARD,LAND,TRANSFORM,SHARED,ANIMATED,NORMAL,CLONE,CLONE_LEGEND,ENTERS_COPY,SHAPESHIFT,EXE_TARGETS
    from type_animation_evidence import ENUMS
    from color_hack_evidence import layout
    from verify_native_build import literal_register_call_targets
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    wanted=set(NAMES.values());symbols={};bodies={};ranges={};objects={};enums={};records={};callback=None;death_worker=None
    native=interfaces['type_animation_native_2022']['native_storage']
    def coff(n):
        ss=[s for s in debug.symbols if s[0]=='_'+n]
        if not ss and n=='this_dies_trigger_impl_2022':
            import re
            ss=[s for s in debug.symbols if re.fullmatch('_'+n+r'\.constprop\.\d+',s[0])]
        assert len(ss)==1,(n,ss)
        _,off,sec,kind,st=ss[0]
        return (off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off),sec,kind,st
    for a,n in NAMES.items():
        at,sec,kind,st=coff(n);assert sec>0 and kind==32 and st==(3 if a in PRIVATE else 2),(n,sec,kind,st)
        symbols[n]=at
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_typedef' and n in {'card_data_t','test_definition_2022_t'}:
                t=unqual(d);records['test_definition_t' if n=='test_definition_2022_t' else n]=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):dict(offset=value(c,'DW_AT_data_member_location'),**layout(c)) for c in t.iter_children() if c.tag=='DW_TAG_member'})
            if d.tag=='DW_TAG_subprogram' and value(d,'DW_AT_low_pc') is not None:
                lo=value(d,'DW_AT_low_pc');hi=d.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value;ranges[(n,lo)]=end
                if n=='this_dies_trigger_impl_2022' and lo==coff(n)[0]:
                    assert coff(n)[1:]==(1,32,3)
                    parameters={inherited_value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter'}
                    attached=parameters.pop('attached')
                    assert attached in {None,0} and set(parameters)=={'player','card','event','trigger_mode','who_gets_trigger','required_kill_code'} and all(x is None for x in parameters.values())
                    # Consolidated native callers also use attached targets, so
                    # GCC now emits the full seven-argument worker. A build
                    # using only attached=0 can still produce a checked clone.
                    cloned=any(s[0].startswith('_'+n+'.constprop.') for s in debug.symbols)
                    assert cloned==(attached==0),('Unexpected worker specialization',n,attached)
                    death_worker=dict(name=n,address=hex(lo),length=end-lo,constant_arguments={} if attached is None else dict(attached=0))
                if n in wanted:
                    assert lo==symbols[n];bodies[n]=list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo),lo))
            if d.tag!='DW_TAG_variable':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            owner=inherited_value(p,'DW_AT_name') if p is not None else None
            key=next((a+'::'+n for a in NAMES if NAMES[a]==owner and a+'::'+n in native['local_objects']),None)
            if key is not None:objects[key]=layout(d)
            if owner==NAMES[CARD] and n=='ptFunction':
                t=unqual(type_of(d));assert t.tag=='DW_TAG_pointer_type';t=unqual(type_of(t));assert t.tag=='DW_TAG_subroutine_type'
                callback=dict(result=type_name(type_of(t)),parameters=[type_name(type_of(c)) for c in t.iter_children() if c.tag=='DW_TAG_formal_parameter'])
    assert objects==native['local_objects'] and enums==ENUMS
    # The retained database view divides the unused 35-byte native name field
    # into an 18-byte name plus seventeen reserved bytes. Every accessed field
    # has the exact native offset/width; verify that sole partition explicitly.
    import copy
    expected_records=copy.deepcopy(native['records'])
    expected_records['card_data_t']['members']['name']=dict(offset=1,bounds=[17],element_bytes=1)
    expected_records['card_data_t']['members']['reserved1']=dict(offset=19,bounds=[16],element_bytes=1)
    assert records==expected_records,records
    name_view=dict(native_name_bytes=35,source_name_bytes=18,source_reserved_bytes=17,byte_range=[1,36],accessed_by_this_batch=False)
    assert callback==native['animated_land_callback']
    assert death_worker and death_worker['length']>0
    # Check the complete emitted range and outgoing calls for the actual
    # specialization, including both attached-target in_play checks when the
    # attached argument remains dynamic.
    lo=int(death_worker['address'],16)
    worker_code=list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,death_worker['length']),lo))
    assert sum(i.size for i in worker_code)==death_worker['length']
    from collections import Counter
    expected_worker_calls={'in_play':1 if death_worker['constant_arguments'] else 3,'get_card_instance':3,'check_for_death_trigger_removal':1,
                           'is_humiliated_for_ability':1,'is_what':2,'backtrace_and_abort':2,
                           'create_legacy_effect_for_opponent':1,'get_abilities':2,'create_legacy_effect':1}
    actual_worker_calls=Counter(i.operands[0].imm for i in worker_code
                                if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM
                                and not lo<=i.operands[0].imm<lo+death_worker['length'])
    assert actual_worker_calls==Counter({coff(n)[0]:count for n,count in expected_worker_calls.items()})
    death_worker['verified_external_call_counts']=expected_worker_calls
    folded={}
    for n in set(symbols)-set(bodies):
        old=n.removesuffix('_2022');target=coff(old)[0];at=symbols[n];end=ranges[(old,target)]
        insns=list(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,5),at))
        assert len(insns)==1 and insns[0].mnemonic=='jmp' and insns[0].operands[0].type==X86_OP_IMM and insns[0].operands[0].imm==target,(n,insns)
        bodies[n]=list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target),target));folded[n]=dict(retained_target=old,target=hex(target),bytes=insns[0].bytes.hex())
    from unify_scalar_helpers import shared_recovered_names
    assert set(folded)=={NAMES[a] for a in [CLEANUP,LAND,TRANSFORM,IS_ARTIFACT]}-shared_recovered_names()
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}|literal_register_call_targets(insns)
    dependencies={UNCOUNTERABLE:['is_humiliated_for_ability','this_dies_trigger_impl_2022','new_make_test_2022'],GLOBAL:['new_make_test_in_play_2022','new_make_test_2022','get_sleighted_color_test_2022','add_a_subtype_2022'],LEGACY:['special_abilities_2022','attached_is_forced_subtype','attached_is_additional_subtype','upkeep_trigger_mode_impl_2022','remove_special_flags_2022','recalculate_all_cards_in_play_2022'],ARTIFACT:['create_targetted_legacy_effect'],CREATURE:['create_targetted_legacy_effect_no_repeat'],ANIMATE:[NAMES[CREATURE]],GLOBAL_LEGACY:['default_test_definition_2022',NAMES[GLOBAL]],REAL_LAND:[NAMES[CREATURE]],LAND2:[NAMES[REAL_LAND]],CARD:['set_base_pt','special_abilities_2022'],SHARED:['mana_producer_2022','granted_generic_activated_ability_2022','set_special_flags_2022','remove_special_flags_2022',NAMES[TRANSFORM]],ANIMATED:[NAMES[SHARED],'remove_special_flags_2022',NAMES[TRANSFORM]],NORMAL:[NAMES[SHARED]],CLONE:['copy_token_characteristics_for_clone_2022'],CLONE_LEGEND:[NAMES[CLONE]],ENTERS_COPY:['base_target_definition_2022','enters_the_battlefield_as_copy_of_any_2022'],SHAPESHIFT:['real_shapeshift_target_2022']}
    resolved={}
    for a,nn in dependencies.items():
        found=targets(bodies[NAMES[a]]);resolved[NAMES[a]]={}
        for n in nn:
            possible={coff(n)[0]}
            if n in folded:possible.add(int(folded[n]['target'],16))
            assert possible & found,(a,n,possible,found)
            resolved[NAMES[a]][n]=[hex(x) for x in sorted(possible & found)]
    callbacks={}
    for a,b in [(UNCOUNTERABLE,CLEANUP),(ARTIFACT,LEGACY),(CREATURE,LEGACY),(LAND2,GLOBAL_LEGACY)]:
        at=symbols[NAMES[b]]
        assert any(o.type==X86_OP_IMM and o.imm==at for i in bodies[NAMES[a]] for o in i.operands),(a,b,hex(at))
        callbacks[NAMES[a]]=dict(name=NAMES[b],address=hex(at))
    assert set(EXE_TARGETS)<=set().union(*(targets(v) for v in bodies.values()))
    imms=lambda a:{o.imm & 0xffffffff for i in bodies[NAMES[a]] for o in i.operands if o.type==X86_OP_IMM}
    assert {0x10000,0x20000,0x4000}<=imms(GLOBAL)
    assert 0x4600 in imms(LEGACY) and 0x20 in imms(LEGACY)
    assert 0x10000 in imms(UNCOUNTERABLE)
    from canonicalize_helpers import load as canonical_proof,span
    consolidation=canonical_proof() or {}
    retired_reset=any(e['source']=='functions/has_creature_type.c' and not e['after'].strip()
                      and span(e['before'],'reset_subtypes') for e in consolidation.get('source_edits',[]))
    if retired_reset:
        assert not any(s[0]=='_reset_subtypes' or s[0].startswith('_reset_subtypes.') for s in debug.symbols)
    else:
        assert coff('reset_subtypes')[0] not in targets(bodies[NAMES[GLOBAL]])|targets(bodies[NAMES[LEGACY]])
    ctor=bodies[NAMES[LAND2]]
    stores={i.operands[0].mem.disp for i in ctor if i.mnemonic=='mov' and i.operands[0].type==X86_OP_MEM and i.operands[0].mem.base and i.reg_name(i.operands[0].mem.base) not in {'esp','ebp'}}
    assert {164,176}<=stores and not {168,172}&stores,stores
    cardcode=bodies[NAMES[CARD]]
    indirect=[i for i in cardcode if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_REG]
    assert len(indirect)==1
    assert any(i.mnemonic=='mov' and len(i.operands)==2 and i.operands[1].type==X86_OP_MEM and i.operands[1].mem.disp==168 for i in cardcode)
    # Verify the complete same-ABI granted-activation frames after rebuilding.
    insns=bodies[NAMES[SHARED]];grant=coff('granted_generic_activated_ability_2022')[0];frames=[]
    for k,i in enumerate(insns):
        if i.mnemonic!='call' or i.operands[0].type!=X86_OP_IMM or i.operands[0].imm!=grant:continue
        writes={}
        for j in insns[:k]:
            if j.mnemonic=='mov' and len(j.operands)==2 and j.operands[0].type==X86_OP_MEM and j.reg_name(j.operands[0].mem.base)=='esp' and j.operands[1].type==X86_OP_IMM:writes[j.operands[0].mem.disp]=j.operands[1].imm & 0xffffffff
        assert all(writes.get(off)==val for off,val in {20:0x80000000,48:0,52:0,56:0}.items()),writes
        frames.append(dict(call=hex(i.address),stack_words=15,constant_words={str(off):writes[off] for off in [20,48,52,56]}))
    assert len(frames)==3
    return dict(public_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in PUBLIC_FUNCTIONS},public_helpers=len(PUBLIC_HELPERS),card_callback=NAMES[CARD],private_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in PRIVATE},folded_thunks=folded,local_objects=objects,records=records,card_name_layout_projection=name_view,enum_values=enums,resolved_dependencies=resolved,callbacks=callbacks,animated_land_callback=dict(**callback,indirect_branch=hex(indirect[0].address)),manland_activation_frames=frames,death_trigger_worker=death_worker,global_filter_slots=native['global_filter_slots'],old_subtype_reset_absent=True,typed_exe_boundaries=[hex(x) for x in EXE_TARGETS])
