"""Native color precedence, printed-color exceptions and word/deck layouts."""
from inspect_release import Image,value,inherited_value

ENUMS=dict(CARD_ID_CELESTIAL_DAWN=2426,CARD_ID_MYCOSYNTH_LATTICE=6920,CARD_ID_DRYAD_ARBOR=8916,CARD_ID_PAINTERS_SERVANT=9622,COLOR_TEST_COLORLESS=1,COLOR_TEST_ANY_COLORED=62,EVENT_SET_COLOR=54,KEYWORD_SWAMPWALK=1,KEYWORD_BASIC_LANDWALK=31,EA_MANA_SOURCE=4096,WAV_CHANGEC=39,CHANGE_COLOR_ADD=1,CHANGE_COLOR_SET=2,CHANGE_COLOR_END_AT_EOT=4,CHANGE_COLOR_NO_SOUND=8,CHANGE_COLOR_NO_SLEIGHT=16,AL_STATIC=4)


def inspect_native(runtime,debug,meta,root):
    import csv
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM
    from recover_color_core import ADDRESSES,PRIVATE,GLOBAL,COLOR,NO_HACK,IID,TYPE,RULES,DECK,MY,EXE_TARGETS
    from color_hack_evidence import layout
    from recover_temporary_exile import sha
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    symbols={};arrays={};enums={};word_array=None;word_count=None
    for a in ADDRESSES:
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(ss)==1 and ss[0]['storage']==(3 if a in PRIVATE else 2);symbols[a]=ss[0]
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag!='DW_TAG_variable':continue
            if n=='strs_words_to_hack':word_array=layout(d)
            if n=='num_words_to_hack':word_count=layout(d)
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            owner=inherited_value(p,'DW_AT_name') if p is not None else None
            if (owner,n) not in {(meta[DECK]['name'],'clrs'),(meta[DECK]['name'],'clrs_res'),(meta[MY]['name'],'clrs')}:continue
            arrays[owner+'::'+n]=dict(**layout(d),location=value(d,'DW_AT_location'))
    assert enums==ENUMS
    expected={'get_deck_color::clrs':dict(bounds=[4],element_bytes=4),'get_deck_color::clrs_res':dict(bounds=[1],element_bytes=4),'has_my_colors::clrs':dict(bounds=[1],element_bytes=4)}
    assert {n:{k:v[k] for k in ['bounds','element_bytes']} for n,v in arrays.items()}==expected
    assert word_array==dict(bounds=[5,49,99],element_bytes=1) and word_count==dict(bounds=[],element_bytes=4)
    def absolute(n):
        ss=[s for s in debug.symbols if s[0]=='_'+n];assert len(ss)==1 and ss[0][2]==-1
        return ss[0][1]
    assert absolute('strs_words_to_hack')==0x620d9c and absolute('num_words_to_hack')==0x738c80
    rows=list(csv.DictReader((root/'.recovery/2022-registrations.csv').open()));jas=[r for r in rows if r['csvid']=='20870']
    assert len(jas)==1 and jas[0]['name']=='Jasconian Isle' and jas[0]['coded']=='False'
    anchors={
        'color_query_set_color_event':('267ea87','c744240836000000'),
        'split_color_overwrites_nonzero':('267ead9','89d085c074038945f0'),
    }
    for n,(a,b) in anchors.items():assert runtime.pe.get_data(int(a,16)-0x2000000,len(bytes.fromhex(b))).hex()==b,n
    code={a:list(decoder.disasm(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16)),int(a,16))) for a in ADDRESSES}
    direct=lambda a:{i.operands[0].imm for i in code[a] if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}
    assert 0x97a not in {o.imm for i in code[GLOBAL] for o in i.operands if o.type==X86_OP_IMM}
    assert {0x97a,0x2596,0x1b08}<={o.imm for i in code[IID] for o in i.operands if o.type==X86_OP_IMM}
    assert 0x5186 in {o.imm for i in code[NO_HACK] for o in i.operands if o.type==X86_OP_IMM}
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,arrays=arrays,enum_values=enums,word_array=dict(address='620d9c',bytes=30000,**word_array),word_count=dict(address='738c80',**word_count),word_strides=dict(color=5000,word=100),scanner_physical_parameter='eax:csvid',
        jasconian_isle=dict(csvid=20870,name=jas[0]['name'],iid=int(jas[0]['iid']),coded=False,source='.recovery/2022-registrations.csv',sha256=sha((root/'.recovery/2022-registrations.csv').read_bytes())),
        anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},
        behavior=['The global shortcut scans Painter and Lattice only, with unsigned strict-newer timestamps and accumulated Painter OR masks. The full IID query additionally gates on static humiliation and handles the owner-only sleighted Dawn/nonland override; preserve its exact changer-dependent result.',
        'Concrete color queries take the SET_COLOR result then use the split-card ID lookup and replace it only with a nonzero database color. Printed color keeps Dryad Arbor and blue Jasconian Isle, colorless artifacts and conditional mana-source masking, including the colorless sentinel. count_colors retains the separate Dryad-only land exception.',
        'Color changes use no-repeat targeted legacy creation, preserve the add/set/EOT/no-sleight/no-sound flags and recover their callback. Hack-word scanners take csvid in EAX and traverse |H words in the 6x50x100 EXE table; retain the two earlier cached exports.',
        'Preserve walk/color masks, shared-color masks, five-bit popcounts, hybrid syntax and color containment. Deck choices hide opposing colors from humans, ignore lands, preserve first-on-tie and two-color selection, and keep chooser retry loops and COLOR_ANY conversion. Dialog code is a typed EXE boundary and is never executed.'])
