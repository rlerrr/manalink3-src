"""Native type/animation/copy layouts, flags and lifecycle evidence."""
from inspect_release import Image,value,inherited_value


def inspect_native(runtime,debug,meta,root):
    from color_hack_evidence import layout
    from recover_type_animation import ADDRESSES,PRIVATE,GLOBAL_LEGACY,ENTERS_COPY,GLOBAL,LEGACY,CREATURE,ARTIFACT,LAND2,CARD,SHARED,EXE_TARGETS
    from recover_temporary_exile import sha
    from export_abi import type_of,type_name
    from pack_native_tables import unqual
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
    symbols={};objects={};enums={};records={};callback=None
    for a in ADDRESSES:
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(ss)==1 and ss[0]['storage']==(3 if a in PRIVATE else 2);symbols[a]=ss[0]
    wanted={GLOBAL_LEGACY:{'this_test'},ENTERS_COPY:{'td'}}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_typedef' and n in {'card_data_t','test_definition_t'}:
                t=unqual(d);records[n]=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):dict(offset=value(c,'DW_AT_data_member_location'),**layout(c)) for c in t.iter_children() if c.tag=='DW_TAG_member'})
            if d.tag!='DW_TAG_variable':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            lo=value(p,'DW_AT_low_pc') if p is not None else None
            a=f'{lo:x}' if lo is not None else None
            if a in wanted and n in wanted[a]:objects[a+'::'+n]=layout(d)
            if a==CARD and n=='ptFunction':
                t=unqual(type_of(d));assert t.tag=='DW_TAG_pointer_type';t=unqual(type_of(t));assert t.tag=='DW_TAG_subroutine_type'
                callback=dict(result=type_name(type_of(t)),parameters=[type_name(type_of(c)) for c in t.iter_children() if c.tag=='DW_TAG_formal_parameter'])
    assert enums==ENUMS
    assert objects=={GLOBAL_LEGACY+'::this_test':dict(bounds=[],element_bytes=216),ENTERS_COPY+'::td':dict(bounds=[],element_bytes=84)},objects
    assert records['card_data_t']['bytes']==72 and records['test_definition_t']['bytes']==216
    fields=records['card_data_t']['members']
    assert {n:(fields[n]['offset'],fields[n]['element_bytes']) for n in ['id','type','power','toughness','code_pointer','static_ability']}==dict(id=(36,2),type=(40,1),power=(46,2),toughness=(48,2),code_pointer=(52,4),static_ability=(56,4))
    assert fields['cc']==dict(offset=43,bounds=[2],element_bytes=1)
    fields=records['test_definition_t']['members']
    assert {n:(fields[n]['offset'],fields[n]['element_bytes']) for n in ['type','type_flag','subtype','subtype_flag','color','color_flag','cmc','cmc_flag']}==dict(type=(100,4),type_flag=(108,2),subtype=(104,4),subtype_flag=(110,2),color=(112,4),color_flag=(124,2),cmc=(128,4),cmc_flag=(136,2))
    assert callback==dict(result='int',parameters=['int','int','event_t']),callback
    anchors={
        'dynamic_iid_pool_and_sixteen_slots':('26962ed','8b3d80537a008b35b4056200895ddc894dd88d4710'),
        'artifact_uses_ordinary_legacy':('26938fc','e8ff71feff'),
        'creature_uses_no_repeat_legacy':('2694e1f','e85c65feff'),
        'global_filter_color_flag_in_color_slot':('2694fec','668b537c8b4dec8990a4000000'),
        'global_filter_cmc_flag_only':('269502d','8990b0000000'),
        'begin_turn_expiry_mode_bit':('268cb86','a820'),
    }
    for n,(a,b) in anchors.items():assert runtime.pe.get_data(int(a,16)-0x2000000,len(bytes.fromhex(b))).hex()==b,n
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def code(a):return list(decoder.disasm(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)),int(a,16)))
    # The global constructor leaves these default legacy slots uninitialized;
    # do not silently repair the old color/cmc packing mistakes.
    stores=[i.operands[0].mem.disp for i in code(LAND2) if i.mnemonic=='mov' and i.operands[0].type==X86_OP_MEM and i.reg_name(i.operands[0].mem.base)=='eax']
    assert 164 in stores and 176 in stores and not {168,172}&set(stores)
    # Concrete fifteen-word granted-ability frames; three calls carry the
    # native sign-bit type-change flag and three trailing zero words.
    insns=code(SHARED);frames=[]
    for k,i in enumerate(insns):
        if i.mnemonic!='call' or i.operands[0].type!=X86_OP_IMM:continue
        target=i.operands[0].imm
        native_name=next((r['name'] for a,r in meta.items() if int(a,16)==target),None)
        if native_name!='granted_generic_activated_ability':continue
        writes={}
        for j in insns[:k]:
            if j.mnemonic=='mov' and len(j.operands)==2 and j.operands[0].type==X86_OP_MEM and j.reg_name(j.operands[0].mem.base)=='esp' and j.operands[1].type==X86_OP_IMM:writes[j.operands[0].mem.disp]=j.operands[1].imm & 0xffffffff
        assert all(writes.get(off)==val for off,val in {20:0x80000000,48:0,52:0,56:0}.items()),writes
        frames.append(dict(call=hex(i.address),target=hex(target),stack_words=15,constant_words={str(off):writes[off] for off in [20,48,52,56]}))
    assert len(frames)==3
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,local_objects=objects,enum_values=enums,records=records,animated_land_callback=callback,anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},manland_activation_frames=frames,dynamic_iid_pool=dict(address='0x7a5380',word_bytes=4,slots=16),global_filter_slots=dict(color_flag_destination=164,cmc_flag_destination=176,untouched=[168,172]),exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},
        behavior=['Animation retains forced/additional subtype helpers, color set/OR slots, all expiry modes including next begin turn, Genju lifelink, Chimeric sacrifice and removal without the old reset_subtypes call. Creature creation reuses the no-repeat legacy; artifact creation does not.',
        'Generic animated lands set base power/toughness, preserve display/color/ability fields and dispatch a typed three-argument callback under the native signed nonnegative-address guard.',
        'Global type changes retain the sixteen-slot dynamic-IID search, second-pass request, CMC-based 0x4000 sentinel, narrow database writes, filtering, subtype add and sleighted color. The old leaves-play subtype reset is absent.',
        'Retain malformed global-animation filter packing: color_flag overwrites the color slot; color_flag and cmc value slots remain untouched, with cmc_flag alone stored. Local tests use 216 bytes and the copy-entry target uses 84 bytes; that target is deliberately initialized only on RESOLVE_SPELL.',
        'Uncounterable spells retain static humiliation, type/test union, an invisible death cleanup legacy and entering/cleanup state clearing. Cloning keeps Copy Artifact/Metamorph type exceptions, animated original identities, backup IID, token-characteristic copying, recalc and legend checking.',
        'Manlands retain all three fifteen-word granted-activation calls, mana-payment gates, threshold restriction, animation marker, parent transform, cleanup and color; dialog and card-type allocation remain typed EXE boundaries and are never executed.'])

ENUMS={'CARD_ID_COPY_ARTIFACT': 47, 'CARD_ID_STALKING_STONES_ANIMATED': 3300, 'CARD_ID_NANTUKO_MONASTERY': 5979, 'CARD_ID_CHIMERIC_COILS': 7026, 'CARD_ID_GENJU_OF_THE_FIELDS': 7473, 'CARD_ID_PHYREXIAN_METAMORPH': 11655, 'EVENT_CLEANUP': 34, 'EVENT_POWER': 50, 'EVENT_TOUGHNESS': 51, 'EVENT_ABILITIES': 52, 'EVENT_SET_COLOR': 54, 'EVENT_CHANGE_TYPE': 60, 'EVENT_CAST_SPELL': 108, 'EVENT_ACTIVATE': 109, 'EVENT_RESOLVE_SPELL': 113, 'EVENT_RESOLVE_ACTIVATION': 114, 'EVENT_CAN_ACTIVATE': 115, 'EVENT_TRIGGER': 125, 'EVENT_RESOLVE_TRIGGER': 126, 'EVENT_BEGIN_TURN': 17920, 'EVENT_STATIC_EFFECTS': 18176, 'TRIGGER_COMES_INTO_PLAY': 219, 'RESOLVE_TRIGGER_MANDATORY': 2, 'KILL_SACRIFICE': 3, 'KILL_EXILE': 4, 'STATE_CANNOT_TARGET': 1048576, 'STATUS_PERMANENT': 32, 'STATUS_ANIMATED': 64, 'STATUS_INVISIBLE_FX': 65536, 'TYPE_LAND': 1, 'TYPE_CREATURE': 2, 'TYPE_ENCHANTMENT': 4, 'TYPE_ARTIFACT': 64, 'TYPE_EFFECT': 128, 'TYPE_PERMANENT': 71, 'KEYWORD_RECALC_CHANGE_TYPE': 16777216, 'LCBP_NEED_EVENT_CHANGE_TYPE_SECOND_PASS': 65536, 'LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS': 131072, 'ANYBODY': 2, 'SF_TYPE_ALREADY_CHANGED': 4, 'SF_MANA_PRODUCER_DISABLED_FOR_AI': 4194304, 'GAA_TYPE_CHANGE': -2147483648, 'AL_STATIC': 4}
