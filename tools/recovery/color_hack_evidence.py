"""Native static tables, buffers and byte mappings for color/subtype helpers."""
from inspect_release import Image,value,inherited_value

COLOR_WORDS=[[s,s.upper()] for s in ['colorless','black','blue','green','red','white','artifact','any']]
LAND_WORDS=[[s,s.capitalize(),s.upper()] for s in ['land','swamp','island','forest','mountain','plains','land','land']]
SUBTYPES=[[0,0,0,0],[0x5009,0x90,1,239],[0x5002,0x89,2,126],[0x5001,0x8e,3,91],[0x5006,0x6f,4,164],[0x5007,0x94,5,188]]
ENUMS=dict(COLOR_ANY=7,COLOR_TEST_ANY_COLORED=62,COLOR_BLACK=1,COLOR_WHITE=5,KEYWORD_PROT_BLACK=0x800,KEYWORD_PROT_COLORED=0xf800,SUBTYPE_SWAMP=0x5009,SUBTYPE_ISLAND=0x5002,SUBTYPE_FOREST=0x5001,SUBTYPE_MOUNTAIN=0x5006,SUBTYPE_PLAINS=0x5007,SF3_HARDCODED_SLEIGHT_WORDS_REPLACED=0x2000)


def layout(die):
    from export_abi import type_of
    from pack_native_tables import unqual
    t=unqual(type_of(die));bounds=[]
    while t.tag=='DW_TAG_array_type':
        bounds.extend(value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type');t=unqual(type_of(t))
    return dict(bounds=bounds,element_bytes=value(t,'DW_AT_byte_size'))


def table_values(pe,address,shape,strings=False):
    import struct
    count=1
    for upper in shape['bounds']:count*=upper+1
    data=pe.get_data(address-pe.OPTIONAL_HEADER.ImageBase,count*4)
    vals=list(struct.unpack('<'+'I'*count,data))
    if strings:vals=[pe.get_data(p-pe.OPTIONAL_HEADER.ImageBase,128).split(b'\0')[0].decode('ascii') for p in vals]
    columns=shape['bounds'][-1]+1
    return [vals[i:i+columns] for i in range(0,count,columns)]


def inspect_native(runtime,debug,meta,root):
    from recover_color_hacks import ADDRESSES,SUBTYPE,LAND_TEXT,TEXT,TEXT2,CONVERT
    from recover_temporary_exile import sha
    symbols={};objects={};enums={};fields={};word_array=None;word_count=None
    wanted={SUBTYPE:{'subtypes'},LAND_TEXT:{'buf','words'},TEXT:{'buf','words'},TEXT2:{'buf','words'}}
    for a in ADDRESSES:
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(ss)==1 and ss[0]['storage']==2;symbols[a]=ss[0]
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_member' and n in {'color_id','hack_mode'} and layout(d)['bounds']:fields[n]=dict(offset=value(d,'DW_AT_data_member_location'),**layout(d))
            if d.tag!='DW_TAG_variable':continue
            if n=='strs_words_to_sleight':word_array=layout(d)
            if n=='num_words_to_sleight':word_count=layout(d)
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            lo=value(p,'DW_AT_low_pc') if p is not None else None
            owner=next((a for a in wanted if inherited_value(p,'DW_AT_name')==meta[a]['name']),None) if p is not None else None
            if owner not in wanted or n not in wanted[owner]:continue
            loc=value(d,'DW_AT_location');assert loc[0]==3 and len(loc)==5
            address=int.from_bytes(bytes(loc[1:]),'little');shape=layout(d);item=dict(address=hex(address),**shape)
            if n=='words':item['values']=table_values(runtime.pe,address,shape,True)
            if n=='subtypes':item['values']=table_values(runtime.pe,address,shape)
            objects[owner+'::'+n]=item
    assert enums==ENUMS
    assert fields==dict(color_id=dict(offset=64,bounds=[5],element_bytes=1),hack_mode=dict(offset=276,bounds=[5],element_bytes=1)),fields
    assert word_array==dict(bounds=[5,49,99],element_bytes=1) and word_count==dict(bounds=[],element_bytes=4)
    assert len(objects)==7,objects
    for a in [LAND_TEXT,TEXT,TEXT2]:assert {k:objects[a+'::buf'][k] for k in ['bounds','element_bytes']}==dict(bounds=[499],element_bytes=1)
    assert len({objects[a+'::buf']['address'] for a in [LAND_TEXT,TEXT,TEXT2]})==3
    assert objects[LAND_TEXT+'::words']['values']==LAND_WORDS and objects[SUBTYPE+'::subtypes']['values']==SUBTYPES
    assert objects[TEXT+'::words']['values']==objects[TEXT2+'::words']['values']==COLOR_WORDS
    assert objects[TEXT+'::words']['address']==objects[TEXT2+'::words']['address']
    def absolute(n):
        ss=[s for s in debug.symbols if s[0]=='_'+n];assert len(ss)==1 and ss[0][2]==-1
        return ss[0][1]
    assert absolute('strs_words_to_sleight')==0x618d4c and absolute('num_words_to_sleight')==0x620d58
    anchors={
        'any_color_compare':('2680da6','83f907'),
        'any_color_result62':('2680db8','b83e000000'),
        'other_colors_x86_shift':('2680db1','d3e0'),
        'varargs_first_stack_word':('26808dc','8d4514'),
        'land_text_speculation':('26808c9','833d7485720001'),
        'color_text_speculation':('26802f9','833d7485720001'),
    }
    for name,(a,b) in anchors.items():assert runtime.pe.get_data(int(a,16)-0x2000000,len(bytes.fromhex(b))).hex()==b,name
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,static_objects=objects,byte_fields=fields,enum_values=enums,word_array=dict(address='618d4c',bytes=30000,**word_array),word_count=dict(address='620d58',**word_count),word_strides=dict(color=5000,word=100),varargs=dict(fixed_words=3,first_argument_ebp_offset=20,argument_bytes=4),
        anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},exe_sha256=sha(exe.data),behavior=['COLOR_ANY converts to the five colored bits; all other inputs retain the native low-five-bit x86 shift count.',
        'Keep six-byte hack/sleight maps, zero-entry fallback, subtype/color/hardcoded/card-ID input domains, negative-reference guards, five-color/protection masks and unrelated-bit retention.',
        'Keep three distinct 500-byte buffers and all subtype/land/color word table values. Native one/two-color text tables share an address. Speculation returns the empty literal for color text but clears/returns the land formatter buffer. Preserve variadic int consumption, %s/%a/%l, a/an, case selection, and unbounded sprintf behavior.',
        'The rules-text scanner consumes |S markers and a 6x50x100 EXE text table, skipping colorless, with 5000/100-byte strides. The AI helper preserves flag-before-scan order, repeated get_id calls and card-definition cache update.'])
