"""Native tap/untap/AI layouts, flags, dialog frames and behavior evidence."""
from inspect_release import Image,value,inherited_value


def inspect_native(runtime,debug,meta,root):
    from color_hack_evidence import layout
    from recover_tap_ai import ADDRESSES,PRIVATE,UPKEEP,LANDS,HAS,DISABLE,ENTER,TWIDDLE,CHOOSE,SICK,EXE_TARGETS
    from recover_temporary_exile import sha
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
    from export_abi import type_of
    from pack_native_tables import unqual
    symbols={};objects={};enums={};fields={}
    wanted={UPKEEP:{'td','prompt','typetext','types'},LANDS:{'td'}}
    for a in ADDRESSES:
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(ss)==1 and ss[0]['storage']==(3 if a in PRIVATE else 2);symbols[a]=ss[0]
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_structure_type' and n=='target_definition_t':
                assert value(d,'DW_AT_byte_size')==84
                fields={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in d.iter_children() if c.tag=='DW_TAG_member'}
            if d.tag!='DW_TAG_variable':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            lo=value(p,'DW_AT_low_pc') if p is not None else None
            a=f'{lo:x}' if lo is not None else None
            if a in wanted and n in wanted[a]:
                objects[a+'::'+n]=layout(d)
                if n=='td':fields={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in unqual(type_of(d)).iter_children() if c.tag=='DW_TAG_member'}
    assert enums==ENUMS
    expected={UPKEEP+'::td':dict(bounds=[],element_bytes=84),UPKEEP+'::prompt':dict(bounds=[199],element_bytes=1),UPKEEP+'::typetext':dict(bounds=[99],element_bytes=1),UPKEEP+'::types':dict(bounds=[16],element_bytes=4),LANDS+'::td':dict(bounds=[],element_bytes=84)}
    assert objects==expected,objects
    assert fields['extra']==40 and fields['special']==56 and fields['allow_cancel']==68 and fields['illegal_abilities']==28,fields
    strings={}
    for a in [0x27635d1,0x275f90e,0x275f914]:strings[f'{a:x}']=runtime.pe.get_data(a-0x2000000,512).split(b'\0')[0].hex()
    assert list(strings.values())==['0102','010c','0107']
    anchors={
        'sickness_guard_and_both_bit_clear':('268981b','f7c200000200740981e2fffffcff895008'),
        'ai_filter_callback_field':('26a7199','c7463800800000c7462830856702'),
        'optional_untap_ai_target_card_guard':('2687807','8b938000000085d2790c'),
    }
    # The complete entry disassembly pins the abilities event and absence of
    # the old enter/leaves-play bulk action. Keep byte anchors for the fields
    # where the source differs most from the retained implementation.
    for n,(a,b) in anchors.items():assert runtime.pe.get_data(int(a,16)-0x2000000,len(bytes.fromhex(b))).hex()==b,n
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def code(a):return list(decoder.disasm(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)),int(a,16)))
    assert ENUMS['EVENT_ABILITIES'] in {o.imm for i in code(DISABLE) for o in i.operands if o.type==X86_OP_IMM}
    assert ENUMS['EVENT_CAST_SPELL'] not in {o.imm for i in code(DISABLE) for o in i.operands if o.type==X86_OP_IMM}
    enter_targets=[i.operands[0].imm for i in code(ENTER) if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM]
    assert enter_targets==[0x26b2ab0,0x2693c00],enter_targets
    # DIALOG's x86 stack frame: three fixed args, three control pointers,
    # small-card pair, two three-word choices and four null sentinels.
    instructions=code(TWIDDLE);calls=[i for i in instructions if i.mnemonic=='call'];dialog=calls[2]
    before=[i for i in instructions if i.address<dialog.address]
    writes={}
    for i in before:
        if i.mnemonic=='mov' and len(i.operands)==2 and i.operands[0].type==X86_OP_MEM and i.reg_name(i.operands[0].mem.base)=='esp' and i.operands[1].type==X86_OP_IMM:
            writes[i.operands[0].mem.disp]=i.operands[1].imm
    expected_fixed={8:109,12:0x27635d1,16:0x275f90e,20:0x275f914,32:0x715dd8,36:1,44:0x715f04,48:1,56:0,60:0,64:0,68:0}
    assert all(writes.get(k)==v for k,v in expected_fixed.items()),writes
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,local_objects=objects,enum_values=enums,target_layout=dict(bytes=84,members=fields),source_state_alias=dict(source='STATE_SICKNESS_UNUSED',native='STATE_SUMMON_SICK_BACKUP',value=65536),dialog_control_strings=strings,
        twiddle_dialog=dict(call=hex(dialog.address),target=hex(dialog.operands[0].imm),stack_words=18,constant_words={str(k):v for k,v in expected_fixed.items()}),
        anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},
        behavior=['Suppression applies during EVENT_ABILITIES with AL_STATIC and excludes itself; entry creates only the name legacy and does not run the old bulk action or leaves-play reset. The predicate calls the full has_an_activated_ability API.',
        'Selection disables cancellation and illegal-ability filtering, installs the AI callback, temporarily makes its source untargettable, and uses the qualified card-name/random-name APIs. Human card-list selection still reaches the deferred typed chooser boundary.',
        'Keep all signed tap/untap/haste/land/draw/steal AI scores, phase weights, mana-land exceptions, ability checks and negative scores. Draw scoring uses AL_TRIG and intentionally reads life[player] even when scoring draws for the opponent.',
        'Act-of-Treason spells handle EVENT_CHANGE_TARGET and restore who_chooses from attacking_card_controller. Untap state retains per-type suppression, optional versus mandatory trigger results, callback decisions, both negative target guards and the once-per-opponent-untap 77 sentinel.',
        'Clearing sickness is guarded by bit 0x20000 and clears both 0x20000 and the native backup bit 0x10000, represented by the existing source alias. Combat removal preserves attacked/blocked history. Dialog code remains an external boundary and is never executed.'])

ENUMS={'CARD_ID_MANA_VAULT': 157, 'CARD_ID_SOL_RING': 230, 'CARD_ID_MISHRAS_WORKSHOP': 510, 'CARD_ID_UNDERWORLD_DREAMS': 829, 'CARD_ID_GRIM_MONOLITH': 1509, 'CARD_ID_TOLARIAN_ACADEMY': 1512, 'CARD_ID_GAEAS_CRADLE': 1513, 'CARD_ID_SERRAS_SANCTUM': 3894, 'CARD_ID_PHYREXIAN_TYRANNY': 5274, 'CARD_ID_SPITEFUL_VISIONS': 9684, 'CARD_ID_NEKUSAR_THE_MINDRAZER': 13627, 'CARD_ID_FATE_UNRAVELER': 13704, 'CARD_ID_OB_NIXILIS_REIGNITED_EMBLEM': 15388, 'EVENT_ABILITIES': 52, 'EVENT_CAST_SPELL': 108, 'EVENT_ACTIVATE': 109, 'EVENT_RESOLVE_SPELL': 113, 'EVENT_TRIGGER': 125, 'EVENT_RESOLVE_TRIGGER': 126, 'EVENT_UNTAP': 130, 'EVENT_UPKEEP_TRIGGER_ABILITY': 2560, 'EVENT_CHOOSING_TO_UNTAP': 18688, 'EVENT_CHANGE_TARGET': 21504, 'KILL_DESTROY': 2, 'STATE_ATTACKING': 4, 'STATE_BLOCKING': 8, 'STATE_ATTACKED': 64, 'STATE_BLOCKED': 16384, 'STATE_SUMMON_SICK_BACKUP': 65536, 'STATE_SUMMON_SICK': 131072, 'TYPE_LAND': 1, 'TYPE_CREATURE': 2, 'TYPE_ENCHANTMENT': 4, 'TYPE_ARTIFACT': 64, 'KEYWORD_FLYING': 32, 'KEYWORD_FIRST_STRIKE': 256, 'KEYWORD_DEFENDER': 8388608, 'KEYWORD_INFECT': 536870912, 'KEYWORD_DOUBLE_STRIKE': 1073741824, 'PHASE_UNTAP': 1, 'PHASE_DECLARE_ATTACKERS': 21, 'PHASE_DECLARE_BLOCKERS': 23, 'EA_ACT_ABILITY': 1, 'EA_ACT_INTERRUPT': 2, 'AI': 1, 'TARGET_STATE_TAPPED': 1, 'TARGET_SPECIAL_EXTRA_FUNCTION': 32768, 'TYPETEXT_0': 0, 'SF2_COULD_NOT_UNTAP': 33554432, 'SP_KEYWORD_UNBLOCKABLE': 16, 'SP_KEYWORD_CANNOT_BLOCK': 32, 'SP_KEYWORD_SHADOW': 256, 'SP_KEYWORD_DEATHTOUCH': 512, 'ACT_UNTAP': 7, 'MATCH': 0, 'GS_CAN_TARGET': 1, 'GS_LITERAL_PROMPT': 128, 'AL_TRIG': 2, 'AL_STATIC': 4}
