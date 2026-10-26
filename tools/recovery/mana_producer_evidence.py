"""Native layouts, literals and instruction anchors for mana-production helpers."""
from inspect_release import Image, value, inherited_value


ENUMS = dict(COLOR_COLORLESS=0,COLOR_BLACK=1,COLOR_BLUE=2,COLOR_GREEN=3,COLOR_RED=4,COLOR_WHITE=5,COLOR_ARTIFACT=6,COLOR_TEST_ANY=63,COLOR_TEST_ARTIFACT=64,TYPE_PERMANENT=71,GS_LITERAL_PROMPT=128,PM_FIXED=0,PM_ALL_ONE=1,PM_ANY_COMBO=2,EVENT_COUNT_MANA=127,EVENT_TAP_CARD=129)
LITERALS = {b'What kind of mana?', b'%d mana left', b'%d mana left (producing ', b'%d', b'%s (%s): What kind of mana?', b'Select target %s.', b'%s', b'land'}


def inspect_native(runtime,debug,meta,root):
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_mana_producers import ADDRESSES, PRIVATE, CHOICE, COMBO, FLARE, WGA, DECLARE_WORKER, ONE_WORKER, EXE_TARGETS
    from recover_temporary_exile import sha
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    bodies={a:list(decoder.disasm(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)),int(a,16))) for a in ADDRESSES}
    symbols={}
    for a in ADDRESSES:
        matches=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(matches)==1 and matches[0]['storage']==(3 if a in PRIVATE else 2);symbols[a]=matches[0]
    assert symbols[DECLARE_WORKER]['name'].endswith('.part.0') and symbols[ONE_WORKER]['name'].endswith('.part.0')
    arrays={};types={};enums={};inline_choice=False
    wanted={(CHOICE,'buf'),(CHOICE,'charging'),(COMBO,'cols'),(FLARE,'prompt'),(WGA,'buffer')}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/produce_mana.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_inlined_subroutine' and n=='choose_combination_of_colors_of_mana_to_produce_onechoice':
                p=d.get_parent()
                while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
                if p is not None and value(p,'DW_AT_low_pc')==int(CHOICE,16):inline_choice=True
            if d.tag!='DW_TAG_variable':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            owner=f"{value(p,'DW_AT_low_pc'):x}" if p is not None and value(p,'DW_AT_low_pc') is not None else None
            if (owner,n)==(WGA,'td'):
                t=unqual(type_of(d));types['target_definition_t']=value(t,'DW_AT_byte_size')
            if (owner,n) not in wanted:continue
            t=unqual(type_of(d));bounds=[]
            while t.tag=='DW_TAG_array_type':
                bounds.extend(value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type');t=unqual(type_of(t))
            arrays[owner+'::'+n]=dict(bounds=bounds,element_bytes=value(t,'DW_AT_byte_size'))
    assert enums==ENUMS and inline_choice
    assert arrays=={CHOICE+'::buf':dict(bounds=[299],element_bytes=1),CHOICE+'::charging':dict(bounds=[6],element_bytes=4),COMBO+'::cols':dict(bounds=[6],element_bytes=4),FLARE+'::prompt':dict(bounds=[299],element_bytes=1),WGA+'::buffer':dict(bounds=[99],element_bytes=1)}
    anchors={
        'charging_seven_slots':('266a743','83f807'),
        'charging_x_sentinel1000':('266a750','c70486e8030000'),
        'pre_pool_adjustment':('266a72d','8b1c8101da'),
        'pool_row_adjustment':('266a738','2b9483e0f44e00'),
        'availability_seven_bits':('266ab93','83e07f'),
        'availability_unsigned_division':('266abbd','f7f7'),
        'availability_public_guard':('266abfc','85c97f08'),
        'one_color_public_guard':('266b536','85c97f0e'),
        'one_color_cancel_guard':('266b18d','833d94f14e0001'),
        'wga_mode_branch':('266b479','837d0c01'),
        'wga_known_positive_worker':('266b48f','e8ecf6ffff'),
        'wga_clear_chosen_colors':('266b4e4','c705e448770200000000'),
        'wga_one_color_worker':('266b4f0','e86bfcffff'),
    }
    for n,(a,b) in anchors.items():assert runtime.pe.get_data(int(a,16)-0x2000000,len(bytes.fromhex(b))).hex()==b,n
    literals={}
    for instructions in bodies.values():
        for ins in instructions:
            for op in ins.operands:
                if op.type!=X86_OP_IMM or not 0x2720000<=op.imm<0x2770000:continue
                data=runtime.pe.get_data(op.imm-0x2000000,512).split(b'\0')[0]
                if data in LITERALS:literals[f'{op.imm:x}']=dict(bytes=(data+b'\0').hex(),text=data.decode('ascii'))
    assert {bytes.fromhex(x['bytes'])[:-1] for x in literals.values()}==LITERALS
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,arrays=arrays,types=types,enum_values=enums,inlined_onechoice=True,literals=literals,
        private_physical_prefix=['eax','edx','ecx'],anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},
        behavior=['Keep the full inlined one-choice logic: seven charging words, X sentinel 1000, pre-pool minus current-pool adjustment, artifact/generic excess application, highest-demand/default selection, prompt lifecycle and cancellation.',
        'Availability counts only seven color bits and distributes unsigned (amount+i)/num_bits values through the positive-amount wrapper. The one-color wrapper clears chosen_colors before checking the amount; its private worker preserves cancel and chosen-color updates.',
        'Mana Flare maps artifact-restricted mana to colorless, preserves the native affected-controller versus player recipients and cancellation fallback. Subtype triggers keep the native permissive permanent-mask check. Wild Growth keeps only the native PM_ALL_ONE versus other branch, positive-amount worker calls, attachment/target/AI behavior and count-mana checks.'])
