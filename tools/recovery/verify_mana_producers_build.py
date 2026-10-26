"""Inspect rebuilt mana chooser, physical workers and production/aura helpers."""
from inspect_release import value,inherited_value
import hashlib,re


def verify(debug,pe,interfaces):
    if not interfaces.get('mana_producer_native_2022'):
        return {}
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86_const import X86_OP_IMM
    from export_abi import type_of
    from pack_native_tables import unqual
    from recover_mana_producers import NAMES,PRIVATE,PUBLIC_HELPERS,CHOICE,DECLARE_WORKER,DECLARE,MULTI,SINGLE,MAYBE,COMBO,ONE_WORKER,ONE,WGA,FLARE,SUBTYPE,AURA,AURA_ONE,AURA_COMBO,EXE_TARGETS
    from mana_producer_evidence import ENUMS,LITERALS
    from verify_native_build import literal_register_call_targets
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    wanted=set(NAMES.values());symbols={};ranges={};bodies={};arrays={};types={};enums={};parts={}
    native=interfaces['mana_producer_native_2022']['native_storage']
    def coff(n):
        matches=[s for s in debug.symbols if s[0]=='_'+n];assert len(matches)==1,(n,matches)
        _,off,sec,kind,st=matches[0]
        return (off if sec==-1 else debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off),sec,kind,st
    for a,n in NAMES.items():
        at,sec,kind,st=coff(n);assert sec>0 and kind==32 and st==(3 if a in PRIVATE else 2),(n,sec,kind,st)
        symbols[n]=at
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/produce_mana.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in ENUMS:enums[n]=value(d,'DW_AT_const_value')
            if d.tag=='DW_TAG_subprogram' and value(d,'DW_AT_low_pc') is not None:
                lo=value(d,'DW_AT_low_pc');hi=d.attributes['DW_AT_high_pc'];end=hi.value if hi.form=='DW_FORM_addr' else lo+hi.value
                ranges[(n,lo)]=end
                if n in wanted:
                    code=pe.get_data(lo-pe.OPTIONAL_HEADER.ImageBase,end-lo)
                    insns=list(decoder.disasm(code,lo))
                    assert insns and insns[-1].address+insns[-1].size==end
                    if lo==symbols[n]:bodies[n]=insns
                    else:
                        # GCC can outline a guarded public body after its old
                        # callers start using the same recovered implementation.
                        matches=[s for s in debug.symbols if re.fullmatch('_'+re.escape(n)+r'\.part\.\d+',s[0]) and s[2]>0 and coff(s[0][1:])[0]==lo]
                        assert len(matches)==1 and matches[0][3:]==(32,3),(n,hex(lo),matches)
                        parts.setdefault(n,[]).append(dict(name=matches[0][0][1:],address=lo,end=end,bytes=len(code),sha256=hashlib.sha256(code).hexdigest(),instructions=insns))
            if d.tag!='DW_TAG_variable':continue
            p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            owner=inherited_value(p,'DW_AT_name') if p is not None else None
            if owner not in wanted:continue
            if n=='td' and owner==NAMES[WGA]:types['target_definition_t']=value(unqual(type_of(d)),'DW_AT_byte_size')
            key=next((a+'::'+n for a in NAMES if NAMES[a]==owner and a+'::'+n in native['arrays']),None)
            if key is None:continue
            t=unqual(type_of(d));bounds=[]
            while t.tag=='DW_TAG_array_type':
                bounds.extend(value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type');t=unqual(type_of(t))
            arrays[key]=dict(bounds=bounds,element_bytes=value(t,'DW_AT_byte_size'))
    assert arrays==native['arrays'] and types==native['types'] and enums==ENUMS,(arrays,types,enums)
    folded={}
    for n in wanted-set(bodies):
        assert n==NAMES[MULTI],n
        old=n.removesuffix('_2022');target=coff(old)[0];at=symbols[n]
        insns=list(decoder.disasm(pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,5),at))
        assert len(insns)==1 and insns[0].mnemonic=='jmp' and insns[0].operands[0].type==X86_OP_IMM and insns[0].operands[0].imm==target
        end=ranges[(old,target)];bodies[n]=list(decoder.disasm(pe.get_data(target-pe.OPTIONAL_HEADER.ImageBase,end-target),target))
        folded[n]=dict(retained_target=old,target=hex(target),bytes=insns[0].bytes.hex())
    assert set(bodies)==wanted
    def targets(insns):
        return {i.operands[0].imm for i in insns if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM}|literal_register_call_targets(insns)
    for name,blocks in parts.items():
        assert {p['address'] for p in blocks}<=targets(bodies[name]),('Outlined blocks are not reached by their canonical entry',name)
    def complete(name):return bodies[name]+[i for p in parts.get(name,[]) for i in p['instructions']]
    dependencies={DECLARE:[DECLARE_WORKER],MAYBE:[SINGLE],COMBO:[CHOICE],ONE:[ONE_WORKER],WGA:[COMBO,ONE_WORKER,DECLARE_WORKER,MAYBE],FLARE:[COMBO],AURA:[WGA],AURA_ONE:[WGA],AURA_COMBO:[WGA]}
    verified={}
    for a,deps in dependencies.items():
        actual=targets(complete(NAMES[a]));assert {symbols[NAMES[b]] for b in deps}<=actual,(a,actual)
        verified[NAMES[a]]=[NAMES[b] for b in deps]
    assert coff('choose_mana_all_one_color_2022')[0] in targets(bodies[NAMES[ONE_WORKER]])
    assert set(EXE_TARGETS)<=set().union(*(targets(complete(n)) for n in bodies))
    assert any(i.mnemonic=='div' for i in bodies[NAMES[DECLARE_WORKER]])
    assert not any(i.mnemonic=='idiv' for i in bodies[NAMES[DECLARE_WORKER]])
    strings={}
    for name in bodies:
        insns=complete(name)
        for ins in insns:
            for op in ins.operands:
                if op.type!=X86_OP_IMM:continue
                rva=op.imm-pe.OPTIONAL_HEADER.ImageBase
                section=pe.get_section_by_rva(rva) if rva>=0 else None
                if section is None or section.Name.rstrip(b'\0')!=b'.rdata':continue
                text=pe.get_data(rva,512).split(b'\0')[0]
                if text in LITERALS:strings[hex(op.imm)]=text.decode('ascii')
    assert set(strings.values())=={x.decode('ascii') for x in LITERALS},strings
    return dict(public_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in PUBLIC_HELPERS},private_symbols={NAMES[a]:hex(symbols[NAMES[a]]) for a in PRIVATE},folded_public_thunks=folded,
        outlined_parts={n:[{k:(hex(v) if k in {'address','end'} else v) for k,v in p.items() if k!='instructions'} for p in blocks] for n,blocks in parts.items()},
        array_shapes=arrays,type_sizes=types,enum_values=enums,dependencies=verified,unsigned_availability_division=True,literals=strings,exe_boundaries=[hex(a) for a in EXE_TARGETS])
