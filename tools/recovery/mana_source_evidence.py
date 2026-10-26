"""Inspect land/creature mana source layouts, replacement events and native quirks."""
from inspect_release import Image,value,inherited_value

ENUMS=dict(SUBTYPE_FOREST=0x5001,SUBTYPE_ISLAND=0x5002,SUBTYPE_MOUNTAIN=0x5006,SUBTYPE_PLAINS=0x5007,SUBTYPE_SWAMP=0x5009,COLOR_ARTIFACT=6,COLOR_ANY=7,EVENT_CHANGE_TYPE=60,EVENT_MODIFY_MANA_PROD=166,EVENT_STATIC_EFFECTS=0x4700,TYPE_LAND=1,TYPE_CREATURE=2,TYPE_ARTIFACT=64,EA_MANA_SOURCE=0x1000,TARGET_STATE_TAPPED=1,TARGET_STATE_SUMMONING_SICK=0x1000,SF2_KATABATIC_WINDS=16,SF2_CONTAMINATION=64,SF2_QUARUM_TRENCH_GNOMES=0x800000,SF2_DEEP_WATER=0x20000000,AL_TRIG=2,AL_STATIC=4)
LITERALS={b'TARGET_CREATURE',b'TARGET_LAND',b'TARGET_ARTIFACT',b'TARGET_PERMANENT',b' One Mana Ability\n Sac Ability\n Cancel',b'Select target land you control.'}


def inspect_native(runtime,debug,meta,root):
    import struct
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_mana_sources import ADDRESSES,TAP,GRANT_ONE,GRANT,ENCHANTED,QUERY,EXE_TARGETS
    from recover_temporary_exile import sha
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    symbols={};bodies={};enums={};types={};card_data={}
    for a in ADDRESSES:
        matches=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(matches)==1 and matches[0]['storage']==2;symbols[a]=matches[0]
        bodies[a]=list(decoder.disasm(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)),int(a,16)))
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
            if d.tag!='DW_TAG_variable' or n!='td':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            lo=value(p,'DW_AT_low_pc') if p is not None else None
            if lo is not None and f'{lo:x}' in {TAP,GRANT_ONE,GRANT,ENCHANTED}:types[f'{lo:x}::td']=value(unqual(type_of(d)),'DW_AT_byte_size')
    assert enums==ENUMS and types=={a+'::td':84 for a in [TAP,GRANT_ONE,GRANT,ENCHANTED]}
    assert card_data==dict(bytes=72,id_offset=36,id_bytes=2),card_data
    anchors={
        'query_native_four_byte_id_word':('266f697','8b5824'),
        'query_low_word_switch1_bound':('266f936','6683fa2c'),
        'query_low_word_switch2_bound':('266f97c','6683fa26'),
        'query_shift_mask1':('266fc14','81e201000050'),
        'land_type_shift_by_converted_color':('266bd61','ba0100000088c1d3e2395638'),
        'producer_quirky_quorum_bit_and':('266ede2','21f8a820'),
        'producer_final_bitcount':('266efbb','e880a30000'),
        'producer_bitcount_saved_in_ecx':('266efc3','89c1'),
        'producer_bitcount_amount_argument':('266efd2','894c240c'),
        'creature_katabatic_guard':('266d202','c744240810000000'),
        'aura_controller_stores_after_default':('266f21a','895da8895da4'),
        'aura_transmutation_land_quantity':('266f5cc','8b04fd84717902'),
        'aura_override_mask_as_index':('266f5ee','8b04fd807179028d04f88b048540717902'),
    }
    for n,(a,b) in anchors.items():assert runtime.pe.get_data(int(a,16)-0x2000000,len(bytes.fromhex(b))).hex()==b,n
    jump_tables={}
    for at,count in [(0x275ea20,45),(0x275ead4,39)]:
        data=runtime.pe.get_data(at-0x2000000,4*count);entries=list(struct.unpack('<'+'I'*count,data))
        assert all(int(QUERY,16)<=x<int(meta[QUERY]['high_pc'],16) for x in entries)
        jump_tables[f'{at:x}']=dict(entries=[f'{x:x}' for x in entries],bytes=data.hex(),sha256=sha(data))
    literals={}
    for insns in bodies.values():
        for i in insns:
            for op in i.operands:
                if op.type!=X86_OP_IMM or not 0x2720000<=op.imm<0x2770000:continue
                data=runtime.pe.get_data(op.imm-0x2000000,512).split(b'\0')[0]
                if data in LITERALS:literals[f'{op.imm:x}']=dict(bytes=(data+b'\0').hex(),text=data.decode('ascii'))
    assert {bytes.fromhex(x['bytes'])[:-1] for x in literals.values()}==LITERALS
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,type_sizes=types,card_data=card_data,enum_values=enums,query_jump_tables=jump_tables,literals=literals,
        anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},
        behavior=['Basic-land conversion contributes subtype and hacked color bits to mana_colors_added during EVENT_MODIFY_MANA_PROD, and refreshes subtypes during EVENT_CHANGE_TYPE. Keep the native info_slot shift by the converted color-test value; the old leaves-play reset and granted-mana fallback are absent.',
        'Preserve the exact four-byte card-data ID word and low-16-bit decision tree, including both jump tables, transmutation/count/life special cases, -1 results and added-color/info-slot/mana-color fallback combinations. Do not collapse these to the old short card list.',
        'Generic producer uses its final bit count as the direct activation amount, including zero. Keep repeated bit-count calls, the bitwise Quorum flag test, sequential color remapping, transmutation/doubler gates and exact worker mca/contaminate arguments.',
        'Creature producers retain Katabatic Winds restrictions and signed AI ratings. Land/creature grants preserve target states, self-versus-other tap dispatch, added colors and quantity/transmutation overrides. Tapped wrappers use trigger humiliation, while granted any-color activation uses static humiliation.',
        'The Aura uses controller stores after target initialization, the native extended Aura-event set, mixed Aura-player versus land-player cache rows, first nonnegative quantity override and the transmutation mask as an override index. Preserve these native quirks without normalization; gameplay correctness is not established.'])
