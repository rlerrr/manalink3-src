"""Two bounded, non-DWARF runtime entries used by retained card bodies.

These are compiler/import facades, not recovered authored C functions. Never
infer identity from a COFF name alone: compare the complete fixed stack probe,
or decode the entire import thunk and resolve its actual PE import slot.
"""
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_MEM
import hashlib


def reviewed_runtime_entries(original,rebuilt,original_pe,rebuilt_pe):
    def entry(image,name,kind):
        matches=[(v,s,k,c) for n,v,s,k,c in image.symbols if n==name]
        assert len(matches)==1,(name,matches)
        value,section,actual_kind,storage=matches[0]
        assert section>0 and storage==2 and actual_kind==kind,(name,matches)
        sec=image.pe.sections[section-1]
        assert sec.Characteristics&0x20000000 and not sec.Characteristics&0x80000000,name
        return image.pe.OPTIONAL_HEADER.ImageBase+sec.VirtualAddress+value
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    result=[]
    for name,kind,size in [('___chkstk_ms',0,42),('_sprintf',32,6)]:
        pair=[]
        for image,pe in ((original,original_pe),(rebuilt,rebuilt_pe)):
            at=entry(image,name,kind);raw=pe.get_data(at-pe.OPTIONAL_HEADER.ImageBase,size)
            ins=list(md.disasm(raw,at));assert sum(i.size for i in ins)==size
            row=dict(address=hex(at),bytes=raw.hex(),bytes_sha256=hashlib.sha256(raw).hexdigest(),coff_name=name,coff_storage=2,coff_kind=kind)
            if name=='___chkstk_ms':
                # Complete GCC i386 probe: preserve EAX/ECX, touch each page,
                # touch the last partial page, return without changing ESP.
                expected='51503d001000008d4c240c721581e9001000008309002d001000003d0010000077eb29c18309005859c3'
                assert raw.hex()==expected
                assert ins[-1].mnemonic=='ret' and not any(i.mnemonic=='call' for i in ins)
                row['contract']='GCC i386 stack probe: EAX byte count; EAX/ECX/ESP preserved; page touches only'
            else:
                assert len(ins)==1 and ins[0].mnemonic=='jmp' and len(ins[0].operands)==1
                op=ins[0].operands[0]
                assert op.type==X86_OP_MEM and not op.mem.base and not op.mem.index and op.size==4
                imports=[(d.dll.lower().decode('ascii'),i.name.decode('ascii') if i.name else None,i.ordinal)
                         for d in pe.DIRECTORY_ENTRY_IMPORT for i in d.imports if i.address==op.mem.disp]
                assert imports==[('msvcrt.dll','sprintf',None)],imports
                row.update(import_slot=hex(op.mem.disp),import_identity=list(imports[0]),contract='cdecl variadic msvcrt.dll!sprintf import tail thunk')
            pair.append(row)
        if name=='___chkstk_ms':assert pair[0]['bytes']==pair[1]['bytes']
        result.append(dict(name=name,native=pair[0],rebuilt=pair[1],scope='Complete local runtime facade only; operating-system CRT internals are not decompiled or claimed equivalent.'))
    return result
