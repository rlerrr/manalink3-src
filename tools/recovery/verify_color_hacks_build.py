"""Inspect rebuilt color helpers, static storage and x86 variadic access."""
from inspect_release import value,inherited_value


def verify(debug,pe,interfaces):
    if not interfaces.get('color_hack_native_2022'):
        return {}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_REG_EBP
    from recover_color_hacks import NAMES,ADDRESSES,SUBTYPE,LAND_TEXT,TEXT,TEXT2,HACKED,SLEIGHTED,PROTECTION,FROM_TEXT,CONVERT,AI
    from color_hack_evidence import layout,table_values,COLOR_WORDS,LAND_WORDS,SUBTYPES,ENUMS
    from verify_native_build import literal_register_call_targets
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    symbols={};bodies={};ranges={};objects={};retained={};enums={};varargs=False;fields={};parts=[];unlocated={}
    wanted={NAMES[SUBTYPE]:{'subtypes'},NAMES[LAND_TEXT]:{'buf','words'},NAMES[TEXT]:{'buf','words'},NAMES[TEXT2]:{'buf','words'}}
    def coff(n):
        ss=[s for s in debug.symbols if s[0]=='_'+n];assert len(ss)==1,(n,ss)
        _,off,sec,kind,storage=ss[0]
        return (off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off),sec,kind,storage
    for a,n in NAMES.items():
        at,sec,kind,st=coff(n);assert sec>0 and kind==32 and st==2;symbols[n]=at
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/functions.c','cards/unlimited.c'}:continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_member' and n in {'color_id','hack_mode'} and layout(d)['bounds']:fields[n]=dict(offset=value(d,'DW_AT_data_member_location'),**layout(d))
            if d.tag=='DW_TAG_subprogram':
                if n==NAMES[LAND_TEXT] and any(c.tag=='DW_TAG_unspecified_parameters' for c in d.iter_children()):
                    assert len([c for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter'])==3;varargs=True
                lo=value(d,'DW_AT_low_pc')
                if lo is not None:
                    hi=d.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value;ranges[(n,lo)]=end
                    if n in NAMES.values():
                        code=list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo),lo))
                        if lo==symbols[n]:bodies[n]=code
                        else:
                            # GCC gives an outlined part its parent's abstract
                            # DWARF name. It is a distinct private COFF range.
                            assert n==NAMES[TEXT]
                            at,sec,kind,st=coff(n+'.part.0')
                            assert lo==at and sec>0 and kind==32 and st==3
                            assert sum(i.size for i in code)==end-lo and code[-1].mnemonic=='ret'
                            parts.append((n,lo,end,code))
            if d.tag!='DW_TAG_variable':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            owner=inherited_value(p,'DW_AT_name') if p is not None else None
            if owner not in wanted and (owner,n)!=('get_hacked_subtype','subtypes'):continue
            if owner in wanted and n not in wanted[owner]:continue
            loc=value(d,'DW_AT_location')
            if loc is None:
                if (owner,n)==(NAMES[TEXT],'words'):unlocated[owner+'::'+n]=layout(d)
                continue
            assert loc[0]==3 and len(loc)==5
            at=int.from_bytes(bytes(loc[1:]),'little');shape=layout(d);item=dict(address=hex(at),**shape)
            if n in {'words','subtypes'}:item['values']=table_values(pe,at,shape,n=='words')
            (objects if owner in wanted else retained)[owner+'::'+n]=item
    assert enums==ENUMS and fields==interfaces['color_hack_native_2022']['native_storage']['byte_fields']
    folded={}
    for n in set(NAMES.values())-set(bodies):
        assert n in {NAMES[a] for a in [HACKED,SLEIGHTED,SUBTYPE,PROTECTION]},n
        old=n.removesuffix('_2022');target=coff(old)[0];at=symbols[n]
        ins=list(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,5),at))
        assert len(ins)==1 and ins[0].mnemonic=='jmp' and ins[0].operands[0].type==X86_OP_IMM and ins[0].operands[0].imm==target
        end=ranges[(old,target)];bodies[n]=list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target),target))
        folded[n]=dict(retained_target=old,target=hex(target),bytes=ins[0].bytes.hex())
    subtype_key=NAMES[SUBTYPE]+'::subtypes'
    if subtype_key not in objects:
        assert NAMES[SUBTYPE] in folded;objects[subtype_key]=retained['get_hacked_subtype::subtypes']
    word_key=NAMES[TEXT]+'::words'
    if word_key not in objects:
        # Constant merging can remove this variable's DW_AT_location while
        # preserving its full DWARF type. Prove the emitted indexed load and
        # actual private COFF objects before assigning the runtime table view.
        assert word_key in unlocated and len(parts)<=1
        if parts:assert parts[0][0]==NAMES[TEXT]
        shape=unlocated[word_key];assert shape==dict(bounds=[7,1],element_bytes=4)
        at=int(objects[NAMES[TEXT2]+'::words']['address'],16)
        ss=[]
        for name,off,sec,kind,storage in debug.symbols:
            if name.startswith('_words.') and sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==at:
                assert storage==3;ss.append(name)
        assert len(ss)==2
        owner_code=parts[0][3] if parts else bodies[NAMES[TEXT]]
        loads=[i for i in owner_code if any(o.type==X86_OP_MEM and o.mem.base==0 and o.mem.scale==4 and o.mem.index!=0 and o.mem.disp==at for o in i.operands)]
        assert len(loads)==1
        objects[word_key]=dict(address=hex(at),**shape,values=table_values(pe,at,shape,True),
            address_evidence=dict(kind='merged private COFF tables and emitted indexed load; DWARF supplies type only',
                                  owner='outlined part' if parts else 'public body',
                                  coff_symbols=ss,load_address=hex(loads[0].address),load_bytes=loads[0].bytes.hex()))
    native=interfaces['color_hack_native_2022']['native_storage']['static_objects']
    assert len(objects)==7,objects
    for key,expected in native.items():
        a,n=key.split('::');actual=objects[NAMES[a]+'::'+n]
        assert {k:actual[k] for k in ['bounds','element_bytes']}=={k:expected[k] for k in ['bounds','element_bytes']}
        if 'values' in expected:assert actual['values']==expected['values']
    assert len({objects[NAMES[a]+'::buf']['address'] for a in [LAND_TEXT,TEXT,TEXT2]})==3
    assert varargs and any(i.mnemonic=='lea' and any(o.type==X86_OP_MEM and o.mem.base==X86_REG_EBP and o.mem.disp==20 for o in i.operands) for i in bodies[NAMES[LAND_TEXT]])
    converter=bodies[NAMES[CONVERT]]
    assert any(o.type==X86_OP_IMM and o.imm==7 for i in converter for o in i.operands) and any(o.type==X86_OP_IMM and o.imm==62 for i in converter for o in i.operands)
    assert any(i.mnemonic=='shl' for i in converter)
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}|literal_register_call_targets(insns)
    outlines=[]
    from hashlib import sha256
    for n,lo,end,code in parts:
        assert lo in targets(bodies[n])
        assert {coff('get_card_instance')[0],coff('sprintf')[0]}<=targets(code)
        assert {int(objects[n+'::buf']['address'],16),int(objects[n+'::words']['address'],16)}<={o.imm for i in code for o in i.operands if o.type==X86_OP_IMM}|{o.mem.disp for i in code for o in i.operands if o.type==X86_OP_MEM}
        outlines.append(dict(name=n+'.part.0',parent=n,address=hex(lo),end=hex(end),bytes=end-lo,
                             sha256=sha256(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo)).hexdigest(),
                             public_edge_verified=True,dependencies=['get_card_instance','sprintf']))
    assert len(parts)<=1
    assert coff(NAMES[FROM_TEXT])[0] in targets(bodies[NAMES[AI]])
    for n in ['check_special_flags3_2022','set_special_flags3_2022']:assert coff(n)[0] in targets(bodies[NAMES[AI]])
    assert coff('strs_words_to_sleight')[0]==0x618d4c and coff('num_words_to_sleight')[0]==0x620d58
    return dict(public_symbols={n:hex(a) for n,a in symbols.items()},folded_public_thunks=folded,outlined_parts=outlines,static_objects=objects,byte_fields=fields,enum_values=enums,
        color_any_result=62,variadic_fixed_parameters=3,first_variadic_ebp_offset=20,distinct_text_buffers=3,shared_rebuilt_color_words=objects[NAMES[TEXT]+'::words']['address']==objects[NAMES[TEXT2]+'::words']['address'],
        exe_word_views=dict(table='0x618d4c',count='0x620d58',table_bytes=30000,color_stride=5000,word_stride=100))
