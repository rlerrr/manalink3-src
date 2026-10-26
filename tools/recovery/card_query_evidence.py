"""Native card-query layouts, type/CMC guards and Grist metadata."""
from inspect_release import Image,value,inherited_value


def inspect_native(runtime,debug,meta,root):
    import csv
    from color_hack_evidence import layout
    from recover_card_queries import ADDRESSES,PRIVATE,PT,SPLIT,IS_WHAT,CMC,CMC_IID,COMMON_HAND,COMMON_NONLAND,SHRINES
    from recover_temporary_exile import sha
    from export_abi import type_of
    from pack_native_tables import unqual
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
    symbols={};objects={};enums={};card_fields={}
    wanted={COMMON_HAND:{'cmc'},COMMON_NONLAND:{'cmc'},SHRINES:{'td'}}
    members={'id','type','cc','power','toughness','code_pointer'}
    for a in ADDRESSES:
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(ss)==1 and ss[0]['storage']==(3 if a in PRIVATE else 2);symbols[a]=ss[0]
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_typedef' and n=='card_data_t':
                t=unqual(d);assert value(t,'DW_AT_byte_size')==72
                card_fields={value(c,'DW_AT_name'):dict(offset=value(c,'DW_AT_data_member_location'),**layout(c)) for c in t.iter_children() if c.tag=='DW_TAG_member' and value(c,'DW_AT_name') in members}
            if d.tag!='DW_TAG_variable':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            lo=value(p,'DW_AT_low_pc') if p is not None else None
            a=f'{lo:x}' if lo is not None else None
            if a in wanted and n in wanted[a]:objects[a+'::'+n]=layout(d)
    assert enums==ENUMS
    assert objects=={COMMON_HAND+'::cmc':dict(bounds=[16],element_bytes=4),COMMON_NONLAND+'::cmc':dict(bounds=[16],element_bytes=4),SHRINES+'::td':dict(bounds=[],element_bytes=84)},objects
    assert card_fields==dict(id=dict(offset=36,bounds=[],element_bytes=2),type=dict(offset=40,bounds=[],element_bytes=1),cc=dict(offset=43,bounds=[2],element_bytes=1),power=dict(offset=46,bounds=[],element_bytes=2),toughness=dict(offset=48,bounds=[],element_bytes=2),code_pointer=dict(offset=52,bounds=[],element_bytes=4))
    rows=list(csv.DictReader((root/'.recovery/2022-registrations.csv').open()));grist=[r for r in rows if r['csvid']=='23059']
    assert len(grist)==1 and grist[0]['full_name']=='Grist, the Hunger Tide' and grist[0]['coded']=='True'
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def code(a):return list(decoder.disasm(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)),int(a,16)))
    for a in [PT,IS_WHAT]:assert 0x5a13 in {o.imm for i in code(a) for o in i.operands if o.type==X86_OP_IMM}
    anchors={
        'grist_base_pt_guard':('2677b7c','66813cdd34707e00135a7430'),
        'grist_type_guard':('2681da0','66817924135a'),
        'split_popcount_unsigned_one_or_two':('267ed86','4a83fa01764c'),
        'colored_mana_count_sentinel':('267ecd3','3c10'),
        'pt_context_restoration':('2677c27','8935848c7300893d8c397a00891db4056200'),
    }
    for n,(a,b) in anchors.items():assert runtime.pe.get_data(int(a,16)-0x2000000,len(bytes.fromhex(b))).hex()==b,n
    pt=code(PT);calls=[i for i in pt if i.mnemonic=='call'];assert len(calls)==1 and calls[0].operands[0].imm==0x26caf40
    callback=calls[0];restores={}
    for label,address in [('affected_card_controller',0x738c84),('affected_card',0x7a398c),('event_result',0x6205b4)]:
        sites=[i for i in pt if i.address>callback.address and i.mnemonic=='mov' and i.operands[0].type==X86_OP_MEM and not i.operands[0].mem.base and i.operands[0].mem.disp==address]
        assert len(sites)==1;restores[label]=dict(address=hex(sites[0].address),bytes=sites[0].bytes.hex())
    cmc=code(CMC);x_calls=[i for i in cmc if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==0x26940a0]
    assert len(x_calls)==2
    return dict(native_symbols=symbols,local_objects=objects,enum_values=enums,card_data=dict(bytes=72,members=card_fields),
        grist=dict(csvid=23059,iid=int(grist[0]['iid']),name=grist[0]['full_name'],coded=True,source='.recovery/2022-registrations.csv',sha256=sha((root/'.recovery/2022-registrations.csv').read_bytes())),
        anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},pt_callback=dict(call=hex(callback.address),target=hex(callback.operands[0].imm),stack_words=5,context_restores=restores),cmc_x_spell_calls=[hex(i.address) for i in x_calls],
        private_parameters={PT:['eax:player','edx:iid','ecx:event'],SPLIT:['eax:csvid','edx:half_played']},
        behavior=['Grist is a creature for off-battlefield is_what queries and has base 1/1. Do not force that exception into get_type_with_iid, which retains its separate printed/current planeswalker behavior. Preserve flash/interrupt normalization and Enchanted Evening/Lattice type flags.',
        'Replace the old fixed split-card CMC table with recursive half-card IID queries. An unsigned 32-bit population count of one or two selects via the supplied mask; every other count sums both halves. Non-split cards return -1 from that private helper. No-mana-cost cards return zero, and zero-valued split results override the printed fallback.',
        'Runtime mana value keeps token zero, backup IID, flipped DFC identity, invisible X costs, Crime/Punishment and Supply/Demand target-slot X values. The second native is_x_spell call is retained even though its return is unused; Ghidra pointer-shaped temporaries are scalar IID/result words.',
        'The variable-PT worker preserves signed 16-bit database values and positive 0x4000 callback markers, passes NULL instance/card -1, and restores both affected globals and event_result. Four public wrappers retain power/toughness events and backup IDs.',
        'Retain seventeen unchecked CMC bins, first-on-tie mode selection and -1 for empty modes. Highest power now supports ANYBODY and floors at zero; power sums stay signed. Preserve ascending player/slot loops, strict comparison modes, type/effect exclusions, the color counter\'s type-query-before-in_play ordering, subtype-check versus subtype-count distinction, AL_STATIC ID counting and Shrine ability-filter clearing.'])

ENUMS={'SUBTYPE_SHRINE': 16416, 'CARD_ID_CRIME_PUNISHMENT': 8189, 'CARD_ID_SUPPLY_DEMAND': 8301, 'EVENT_POWER': 50, 'EVENT_TOUGHNESS': 51, 'STATE_INVISIBLE': 32, 'TYPE_LAND': 1, 'TYPE_CREATURE': 2, 'TYPE_ENCHANTMENT': 4, 'TYPE_SORCERY': 8, 'TYPE_INSTANT': 16, 'TYPE_INTERRUPT': 32, 'TYPE_ARTIFACT': 64, 'TYPE_EFFECT': 128, 'TYPE_PERMANENT': 71, 'TARGET_TYPE_PLANESWALKER': 256, 'KEYWORD_DEFENDER': 8388608, 'ANYBODY': 2, 'SF2_MYCOSYNTH_LATTICE': 4, 'SF2_ENCHANTED_EVENING': 128, 'SF3_CARD_IS_FLIPPED': 512, 'AL_STATIC': 4}
