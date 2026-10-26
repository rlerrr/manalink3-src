"""Native prevention layouts, compiler-worker ABI and shield side effects."""
from inspect_release import value,inherited_value

ENUMS=dict(EVENT_PREVENT_DAMAGE=33,EVENT_CLEANUP=34,EVENT_GRAVEYARD_FROM_PLAY=119,
           EVENT_AFTER_DAMAGE=167,EVENT_SET_LEGACY_EFFECT_NAME=256,KILL_EXILE=4,
           STATUS_COMBAT_DAMAGE=0x40000,STATUS_FIRST_STRIKE_DAMAGE=0x100000,
           STATE_CANNOT_TARGET=0x100000,STATE_TAPPED=16,STATE_OWNED_BY_OPPONENT=0x1000,
           TYPE_CREATURE=2,TYPE_EFFECT=128,ANYBODY=2,KEYWORD_TRAMPLE=128,
           SF_ATTACKING_PWALKER=64,SF_WONT_RECEIVE_DAMAGE=2,
           LCBP_PENDING_DAMAGE_CARDS=2,LCBP_DAMAGE_PREVENTION=4,
           DDBM_MUST_BE_COMBAT_DAMAGE=4,DDBM_MUST_DAMAGE_PLAYER=16,DDBM_MUST_DAMAGE_OPPONENT=2,
           PREVENT_ADD_1_1_COUNTER=1,PREVENT_GAIN_LIFE=2,PREVENT_REDIRECT=4,PREVENT_INFINITE=8,
           FOG_COLOR_MASK=63,FOG_INVERSE_COLOR_CHECK=128,FOG_COMBAT_DAMAGE_ONLY=256,
           FOG_UNBLOCKED_CREATURES_ONLY=512,FOG_CREATURES_WITHOUT_TRAMPLE=1024,
           FOG_BY_CONTROLLED_BY_PLAYER=2048,FOG_BY_NON_SUBTYPE=4096,FOG_BY_ANY_TYPE=8192,
           FOG_ONLY_DAMAGE_TO_T_PLAYER=16384,SUBTYPE_WOLF=215,SUBTYPE_WEREWOLF=212,COUNTER_P1_P1=0)
FIELDS={
 'card_instance_t':{'state','token_status','internal_card_id','info_slot','initial_color','targets',
                    'number_of_targets','damage_target_card','damage_target_player','damage_source_card',
                    'damage_source_player','counter_toughness','untap_status','display_pic_csv_id','eot_toughness'},
 'card_ptr_t':{'name'},'card_ext_t':{'special_flags'},
}


def accessed_records(debug):
    from export_abi import type_of
    from pack_native_tables import unqual
    from color_hack_evidence import layout
    records={};enums={};signed={}
    def fields(t,wanted,base=0):
        result={}
        for c in t.iter_children():
            if c.tag!='DW_TAG_member':continue
            n=value(c,'DW_AT_name');off=base+(value(c,'DW_AT_data_member_location') or 0);ty=unqual(type_of(c))
            if n in wanted:
                result[n]=dict(offset=off,**layout(c))
                if n in {'counter_toughness','initial_color','damage_target_player','damage_source_player'}:
                    signed[n]=value(ty,'DW_AT_encoding');assert signed[n] in {5,6}
            if ty.tag in {'DW_TAG_union_type','DW_TAG_structure_type'}:
                result.update(fields(ty,wanted,off))
        return result
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_typedef' and n in FIELDS:
                t=unqual(d);records[n]=dict(bytes=value(t,'DW_AT_byte_size'),members=fields(t,FIELDS[n]))
    assert enums==ENUMS,(enums,ENUMS)
    assert {n:r['bytes'] for n,r in records.items()}==dict(card_instance_t=300,card_ptr_t=152,card_ext_t=192)
    assert all(set(records[n]['members'])==wanted for n,wanted in FIELDS.items())
    assert signed==dict(damage_source_player=6,counter_toughness=5,initial_color=6,damage_target_player=6),signed
    return records,enums,signed


def inspect_native(runtime,debug,meta,root):
    import csv
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
    from export_abi import type_of,type_name
    from damage_prevention_bodies import BODIES
    from recover_damage_prevention import PRIVATE,WORKER,SHIELD,NEXT,MY,DEALT_LEGACY
    from recover_literals import readonly_string
    from recover_temporary_exile import sha
    records,enums,signed=accessed_records(debug);symbols={};returns={};parameters={}
    for a in BODIES:
        ss=[dict(name=n,storage=s) for n,v,sec,k,s in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(ss)==1 and ss[0]['storage']==(3 if a in PRIVATE else 2);symbols[a]=ss[0]
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_subprogram':continue
            lo=value(d,'DW_AT_low_pc');a=f'{lo:x}' if lo is not None else None
            if a in BODIES:returns[a]=type_name(type_of(d))
            if a==WORKER:
                parameters={inherited_value(c,'DW_AT_name'):value(c,'DW_AT_location') for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter'}
    assert set(returns)==set(BODIES) and set(parameters)=={'player','card','flags','event'}
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def code(a):return list(decoder.disasm(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)),int(a,16)))
    worker=code(WORKER)
    # Three physical register arguments; event has only a GNU parameter-ref
    # location, and both native callers guard it before entering this worker.
    assert not any(o.type==X86_OP_MEM and i.reg_name(o.mem.base)=='ebp' and o.mem.disp>=8 for i in worker for o in i.operands)
    gated={}
    for a in [MY,DEALT_LEGACY]:
        ins=code(a);calls=[i for i in ins if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==int(WORKER,16)]
        compares=[i for i in ins if i.mnemonic=='cmp' and any(o.type==X86_OP_IMM and o.imm==33 for o in i.operands)]
        assert len(calls)==len(compares)==1 and compares[0].address<calls[0].address
        gated[a]=dict(compare=hex(compares[0].address),compare_bytes=compares[0].bytes.hex(),call=hex(calls[0].address),call_bytes=calls[0].bytes.hex())
    literal=readonly_string(runtime,0x275fa92);assert literal==b'%s (%d)'
    title=[i for i in code(SHIELD) if any(o.type==X86_OP_IMM and o.imm==0x275fa92 for o in i.operands)];assert len(title)==1
    shield=code(SHIELD);counter_calls=[i for i in shield if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==0x26d4840]
    assert len(counter_calls)==1
    # The counter API has six stack words, including the shield's source.
    before=shield[:shield.index(counter_calls[0])]
    zero=[i for i in before if i.mnemonic=='mov' and len(i.operands)==2 and i.operands[0].type==X86_OP_MEM and i.reg_name(i.operands[0].mem.base)=='esp' and i.operands[0].mem.disp==16 and i.operands[1].type==X86_OP_IMM and i.operands[1].imm==0]
    assert zero
    names=[r for r in csv.DictReader((root/'.recovery/2022-registrations.csv').open()) if r['csvid']==str(0x2efd)];assert len(names)==1 and names[0]['full_name']=='Moonmist'
    return dict(native_symbols=symbols,return_types=returns,records=records,enum_values=enums,signed_fields=signed,
                worker_physical_parameters=['eax:player','edx:card','ecx:flags'],worker_event_optimized_away=True,worker_event_gates=gated,
                shield_title=dict(address='275fa92',literal=literal.decode(),sha256=sha(literal),instruction=hex(title[0].address),bytes=title[0].bytes.hex(),buffer_bytes=51),
                counter_award=dict(call=hex(counter_calls[0].address),target='26d4840',stack_words=6,counter_type=0,source_aware=True),
                moonmist=dict(csvid=0x2efd,name=names[0]['full_name'],metadata_sha256=sha((root/'.recovery/2022-registrations.csv').read_bytes())),
                behavior=['Separate the compiler worker\'s three physical player/card/flags parameters from the optimized-away event argument. Its two callers gate EVENT_PREVENT_DAMAGE.',
                          'Fog retains every source/player/combat/unblocked/trample/color/inverse-color/subtype filter, the Moonmist Wolf/Werewolf exception and the separate planeswalker setup.',
                          'Shields retain signed 16-bit capacity, source-aware counters, non-lifelink life gain, redirection validation, opponent ownership toggle, immediate damage redispatch and low-byte redraw rollover.',
                          'Preserve selective unpreventable-damage guards, unconditional legacy lookup after creation where native has no failure check, detachment while damage is pending, EOT/cleanup distinctions and the damage-until-EOT source/target pairing quirk. Negative prevention values other than -1 can increase the damage amount as in the native helper.'])
