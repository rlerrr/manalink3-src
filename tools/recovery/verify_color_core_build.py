"""Inspect rebuilt color queries/changes, folded APIs and inlined word scanners."""
from inspect_release import value,inherited_value


def verify(debug,pe,interfaces):
    if not interfaces.get('color_core_native_2022'):
        return {}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM
    from recover_color_core import NAMES,ADDRESSES,PRIVATE,PUBLIC_HELPERS,GLOBAL,COLOR,NO_HACK,IID,REAL,CHANGE,LEGACY,WALK,KEYWORD,TYPE,RULES,CSVID,COLORLESS,SHARED,MY,REAL_COUNT,COUNT,EVALUATE,HYBRID,DECK,SELECT,CHOOSE_FROM,CHOOSE,EXE_TARGETS
    from color_core_evidence import ENUMS
    from color_hack_evidence import layout
    from verify_native_build import literal_register_call_targets
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    symbols={};bodies={};ranges={};arrays={};enums={};inline=[];private_symbols={};private_parameters={}
    def coff(n):
        ss=[s for s in debug.symbols if s[0]=='_'+n];assert len(ss)==1,(n,ss)
        _,off,sec,kind,st=ss[0]
        return (off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off),sec,kind,st
    for a in PUBLIC_HELPERS:
        n=NAMES[a];at,sec,kind,st=coff(n);assert sec>0 and kind==32 and st==2;symbols[n]=at
    for a in PRIVATE:
        n=NAMES[a]
        if any(s[0]=='_'+n for s in debug.symbols):
            at,sec,kind,st=coff(n);assert sec>0 and kind==32 and st==3
            private_symbols[n]=at
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_subprogram' and value(d,'DW_AT_low_pc') is not None:
                lo=value(d,'DW_AT_low_pc');hi=d.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value;ranges[(n,lo)]=end
                if n in symbols:
                    assert lo==symbols[n];bodies[n]=list(decoder.disasm(pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo),lo))
                for name,at in private_symbols.items():
                    if lo==at and n in {name,name.removesuffix('_2022')}:
                        from export_abi import type_of
                        params=[c for c in d.iter_children() if c.tag=='DW_TAG_formal_parameter']
                        assert len(params)==1 and inherited_value(params[0],'DW_AT_name')=='csvid'
                        t=type_of(params[0]);assert value(t,'DW_AT_byte_size')==4 and value(t,'DW_AT_encoding')==5
                        private_parameters[name]=dict(name='csvid',bytes=4,signed=True)
            if d.tag=='DW_TAG_inlined_subroutine' and n in {NAMES[a] for a in PRIVATE}:
                p=d.get_parent()
                while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
                assert inherited_value(p,'DW_AT_name')==NAMES[CSVID]
                origin=d.get_DIE_from_attribute('DW_AT_abstract_origin');params=[inherited_value(c,'DW_AT_name') for c in origin.iter_children() if c.tag=='DW_TAG_formal_parameter'];assert params==['csvid']
                spans=[];lo=value(d,'DW_AT_low_pc')
                if lo is not None:
                    hi=d.attributes['DW_AT_high_pc'];spans=[(lo,hi.value if hi.form=='DW_FORM_addr' else lo+hi.value)]
                else:
                    base=value(cu.get_top_DIE(),'DW_AT_low_pc')
                    for r in debug.dwarf().range_lists().get_range_list_at_offset(value(d,'DW_AT_ranges'),cu=cu):
                        if hasattr(r,'base_address'):base=r.base_address
                        else:
                            lo=r.begin_offset+(0 if r.is_absolute else base);end=r.end_offset+(0 if r.is_absolute else base)
                            if end>lo:spans.append((lo,end))
                assert spans;inline.append(dict(name=n,parent=NAMES[CSVID],ranges=spans))
            if d.tag!='DW_TAG_variable' or n not in {'clrs','clrs_res'}:continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            owner=inherited_value(p,'DW_AT_name') if p is not None else None
            if (owner,n) in {(NAMES[DECK],'clrs'),(NAMES[DECK],'clrs_res'),(NAMES[MY],'clrs')}:arrays[owner+'::'+n]=layout(d)
    assert enums==ENUMS
    native=interfaces['color_core_native_2022']['native_storage']
    expected={n.split('::')[0]+'_2022::'+n.split('::')[1]:{k:v[k] for k in ['bounds','element_bytes']} for n,v in native['arrays'].items()}
    assert arrays==expected,arrays
    folded={};missing_dwarf={}
    for n in set(symbols)-set(bodies):
        old=n.removesuffix('_2022');target=coff(old)[0];at=symbols[n]
        end=ranges[(old,target)]
        if n==NAMES[SHARED]:
            # GCC emits this external body but supplies only its abstract DIE
            # and inline instance. Its whole COFF body equals the retained
            # helper, whose independent DWARF range supplies a checked length.
            original=pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target)
            actual=pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,end-target)
            assert actual==original
            decoded=list(decoder.disasm(actual,at))
            assert decoded[-1].mnemonic=='ret' and not any(i.mnemonic=='call' or i.mnemonic.startswith('j') for i in decoded)
            missing_dwarf[n]=dict(retained_equivalent=old,bytes=actual.hex(),length=len(actual),address=hex(at))
            bodies[n]=decoded
            continue
        ins=list(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,5),at))
        assert len(ins)==1 and ins[0].mnemonic=='jmp' and ins[0].operands[0].type==X86_OP_IMM and ins[0].operands[0].imm==target,(n,hex(target),[(i.mnemonic,i.op_str) for i in ins])
        bodies[n]=list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target),target))
        folded[n]=dict(retained_target=old,target=hex(target),bytes=ins[0].bytes.hex())
    from unify_scalar_helpers import shared_recovered_names
    assert set(folded)=={NAMES[a] for a in [WALK,KEYWORD,EVALUATE,HYBRID,DECK,CHOOSE_FROM,CHOOSE]}-shared_recovered_names()
    outlines={}
    required={coff(n)[0] for n in ['strstr','strlen','strncmp']}
    if private_symbols:
        # GCC shares each outlined scanner with its retained implementation.
        # COFF supplies the qualified entry; the retained-name DWARF DIE at
        # that exact address supplies its complete concrete range/type.
        assert set(private_symbols)=={NAMES[a] for a in PRIVATE} and not inline
        assert set(private_parameters)==set(private_symbols)
        for n,at in private_symbols.items():
            owner=n if (n,at) in ranges else n.removesuffix('_2022');end=ranges[(owner,at)]
            assert end-at==254
            raw=pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,end-at)
            code=list(decoder.disasm(raw,at));assert sum(i.size for i in code)==end-at
            calls=[i.operands[0].imm for i in code if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM]
            assert len(calls)==3 and set(calls)==required
            assert any(i.mnemonic=='ret' for i in code)
            caller_targets={i.operands[0].imm for i in bodies[NAMES[CSVID]] if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM}
            assert at in caller_targets
            import hashlib
            outlines[n]=dict(address=hex(at),retained_dwarf_name=owner,length=end-at,
                             sha256=hashlib.sha256(raw).hexdigest(),parameter=private_parameters[n],
                             string_calls=[hex(x) for x in sorted(calls)])
    else:
        assert len(inline)==2 and {x['name'] for x in inline}=={NAMES[a] for a in PRIVATE}
    owner=bodies[NAMES[CSVID]];lo=symbols[NAMES[CSVID]];end=ranges[(NAMES[CSVID],lo)]
    required={coff(n)[0] for n in ['strstr','strlen','strncmp']}
    for instance in inline:
        assert all(lo<=a<b<=end for a,b in instance['ranges'])
        calls={i.operands[0].imm for i in owner if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and any(a<=i.address<b for a,b in instance['ranges'])}
        assert calls==required,(instance,calls,required)
        instance['string_calls']=[hex(a) for a in sorted(calls)];instance['ranges']=[[hex(a),hex(b)] for a,b in instance['ranges']]
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}|literal_register_call_targets(insns)
    dependencies={COLOR:['split_cards_id_table'],IID:[NAMES[NO_HACK],'get_sleighted_color_test_2022','is_humiliated_for_ability'],REAL:[NAMES[COLOR],NAMES[IID]],CHANGE:['create_targetted_legacy_effect_no_repeat','get_sleighted_color_test_2022'],COLORLESS:[NAMES[COLOR]],MY:[NAMES[COLOR]],COUNT:[NAMES[COLOR]],SELECT:[NAMES[DECK]]}
    resolved={}
    for a,nn in dependencies.items():
        found=targets(bodies[NAMES[a]]);resolved[NAMES[a]]={}
        for n in nn:
            possible={coff(n)[0]}
            if n in folded:possible.add(int(folded[n]['target'],16))
            assert possible & found,(a,n,possible,found)
            resolved[NAMES[a]][n]=[hex(x) for x in sorted(possible & found)]
    callback=coff(NAMES[LEGACY])[0]
    assert any(o.type==X86_OP_IMM and o.imm==callback for i in bodies[NAMES[CHANGE]] for o in i.operands)
    assert set(EXE_TARGETS)<=set().union(*(targets(b) for b in bodies.values()))
    imms=lambda a:{o.imm for i in bodies[NAMES[a]] for o in i.operands if o.type==X86_OP_IMM}
    assert 0x97a not in imms(GLOBAL) and {0x97a,0x2596,0x1b08}<=imms(IID)
    assert 0x5186 in imms(NO_HACK)
    assert coff('strs_words_to_hack')[0]==0x620d9c and coff('num_words_to_hack')[0]==0x738c80
    return dict(public_symbols={n:hex(at) for n,at in symbols.items()},folded_public_thunks=folded,private_scanner_inline_instances=inline,private_scanner_outlines=outlines,public_without_concrete_dwarf=missing_dwarf,local_array_views=arrays,enum_values=enums,
        dependencies={NAMES[a]:nn for a,nn in dependencies.items()},resolved_dependencies=resolved,legacy_callback=hex(callback),global_shortcut_excludes_dawn=True,jasconian_isle_csvid=20870,exe_boundaries=[hex(a) for a in EXE_TARGETS],
        exe_word_views=dict(table='0x620d9c',count='0x738c80',bytes=30000,color_stride=5000,word_stride=100))
