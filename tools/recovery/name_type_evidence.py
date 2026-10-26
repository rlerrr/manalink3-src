"""Native storage, assertion continuations and name/type behavior evidence."""
from inspect_release import Image,value,inherited_value

ENUMS=dict(CARD_ID_LIGHTNING_BOLT=145,CARD_ID_SPLINTER=4194,CARD_ID_SAKASHIMA_THE_IMPOSTOR=7731,CARD_ID_LAZAV_DIMIR_MASTERMIND=12992,CARD_ID_FACE_DOWN_CREATURE=13252,CARD_ID_WASTES=15719,CARD_ID_DARKSTEEL_MUTATION_INSECT=18713,CARD_ID_RIPTIDE_REPLICATOR_TOKEN=18809,CARD_ID_SPLINTER_TOKEN=18831,EVENT_CLEANUP=34,EVENT_SET_LEGACY_EFFECT_NAME=256,EVENT_STATIC_EFFECTS=18176,TYPE_PERMANENT=71,TYPE_NONEFFECT=127,TYPE_ANY=255,TYPETEXT_NO_ABBREV=1,TYPETEXT_INVERT=2,AI_FIRST_FOUND=5)


def inspect_native(runtime,debug,meta,root):
    from color_hack_evidence import layout
    from recover_name_type import ADDRESSES,PRIVATE,TYPE_IMPL,ALT_IMPL,RANDOM,TUTOR,DAMAGE,TYPE
    from recover_temporary_exile import sha
    symbols={};objects={};enums={};tables={}
    wanted={RANDOM:{'buffer'},TUTOR:{'msg','this_test'},DAMAGE:{'this_test'},TYPE_IMPL:{'types'}}
    for a in ADDRESSES:
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(ss)==1 and ss[0]['storage']==(3 if a in PRIVATE else 2);symbols[a]=ss[0]
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag!='DW_TAG_variable':continue
            if n in {'strs_hunting_subtypenames','text_lines'}:tables[n]=layout(d)
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            lo=value(p,'DW_AT_low_pc') if p is not None else None
            if lo is None:continue
            a=f'{lo:x}'
            if a in wanted and n in wanted[a]:objects[a+'::'+n]=layout(d)
    assert enums==ENUMS
    assert objects=={RANDOM+'::buffer':dict(bounds=[99],element_bytes=1),TUTOR+'::msg':dict(bounds=[99],element_bytes=1),TUTOR+'::this_test':dict(bounds=[],element_bytes=216),DAMAGE+'::this_test':dict(bounds=[],element_bytes=216),TYPE_IMPL+'::types':dict(bounds=[16],element_bytes=4)},objects
    assert tables==dict(strs_hunting_subtypenames=dict(bounds=[249,299],element_bytes=1),text_lines=dict(bounds=[248,299],element_bytes=1))
    def absolute(n):
        ss=[s for s in debug.symbols if s[0]=='_'+n];assert len(ss)==1 and ss[0][2]==-1
        return hex(ss[0][1])
    table_addresses={n:absolute(n) for n in tables}
    assert table_addresses==dict(strs_hunting_subtypenames='0x79066c',text_lines='0x715cac')
    strings={}
    for a in [0x275fa73,0x275fa77,0x275f920,0x275f9e8]:
        strings[f'{a:x}']=runtime.pe.get_data(a-0x2000000,512).split(b'\0')[0].decode('ascii')
    assert strings['275fa73']=='non' and strings['275fa77']==''
    assert strings['275f920']=='functions/functions.c:15713\n(cards_data[instance->internal_card_id].id == 902 || cards_data[instance->internal_card_id].id == 903) && "alternate_legacy_text only works on csvid 902 or 903 effects"'
    assert strings['275f9e8']=='functions/functions.c:15714\ninstance->internal_card_id != LEGACY_EFFECT_PUMP && "alternate_legacy_text doesn\'t work on LEGACY_EFFECT_PUMP"'
    anchors={
        'first_loop_in_play_unconditional_after_owner_gate':('269fd36','84db74f689742404893c24e88a35010085c074e6'),
        'second_loop_in_play_unconditional_after_owner_gate':('269fdb6','84db74f6897c2404893424e80a35010085c074e6'),
        'first_assert_returns_to_pump_check':('2678c88','e8238504008b436c390584057900'),
        'second_assert_returns_to_flag_write':('2678cb7','e8f48404008b434c80e4fc09f089434c'),
        'public_alternate_reloads_return_card':('2693bd3','e87850feff8b45105dc3'),
        'public_type_reloads_return_buffer':('26965ef','e81c27feff8b4508c9c3'),
    }
    for n,(a,b) in anchors.items():assert runtime.pe.get_data(int(a,16)-0x2000000,len(bytes.fromhex(b))).hex()==b,n
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,local_objects=objects,enum_values=enums,table_shapes=tables,table_addresses=table_addresses,strings=strings,anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},name_buffer_bytes=51,subtype_name_upper_bound=232,exe_sha256=sha(exe.data),
        worker_contracts={ALT_IMPL:dict(parameters=['eax:textnum','edx:player','ecx:legacycard'],return_value='unused; public wrapper reloads legacycard'),TYPE_IMPL:dict(parameters=['eax:buf','edx:maxlen','ecx:type','stack:flags'],return_value='unused final scnprintf EAX; public wrapper reloads buf')},
        behavior=['Use exact original-ID exceptions, nameless face-down cards, stored token subtypes, the Splinter token remap and nameless tutor prompt. Damage uses one ANYBODY call with a 216-byte test.',
        'Count in current-turn order, descending slots. Both loops always check in_play after the owner/ANYBODY gate; Ghidra incorrectly removes the first check. AI naming makes at most ten random attempts, type takes precedence over an optional test, and no filter never selects a random card; fallback is Lightning Bolt.',
        'Human naming delegates to the existing typed new_card_from_list_2022 boundary; chooser UI remains deferred. AI searches library, graveyard then exile, retaining IID/CSVID mode conversion.',
        'Name/subtype strings retain the 233-entry accessible window into the 250x300 EXE table and complemented negative card IDs. Naming callbacks retain the 51-byte output bound, cleanup and exile checks.',
        'Type text retains a 17-pointer array, abbreviations, sorting before Effect, empty None fallback, non prefixes and final comma/or formatting. Public speculation clears and returns buf. Alternate text accepts csvid 902/903, retains exact assertions, continues if backtrace_and_abort returns, and clears only bits 0x300.'])
