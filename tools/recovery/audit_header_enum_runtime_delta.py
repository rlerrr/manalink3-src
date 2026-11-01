"""Check the enum batch changes only the established crew-event immediates."""
import json
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from canonicalize_helpers import ROOT,sha
from inspect_release import Image
from verify_native_absolute_bindings import runtime_sections
from verify_native_header_enums import crew_ranges,immediate_sites


def main():
    proof=json.loads((ROOT/'docs/recovery-evidence/helper-canonicalization.json').read_bytes())
    absolute=json.loads((ROOT/proof['native_absolute_bindings_recovery']).read_bytes())
    sealed=absolute['linker_only_build'];before=Image((ROOT/sealed['path']).read_bytes())
    assert sha(before.data)==sealed['dll_sha256']
    after=Image((ROOT/'ManalinkEh.dll').read_bytes());debug=Image((ROOT/'ManalinkEh.dbg').read_bytes())
    original=runtime_sections(before);current=runtime_sections(after)
    assert original.keys()==current.keys()
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True;sites=[];authorized={}
    for name,parts in crew_ranges(debug).items():
        assert len(parts)==1
        for row in immediate_sites(after,*parts[0],0x1d00):
            at=int(row['address'],16)
            new=next(md.disasm(bytes.fromhex(row['bytes']),at,count=1))
            old=next(md.disasm(before.pe.get_data(at-0x2000000,new.size),at,count=1))
            assert old.size==new.size and new.imm_size==old.imm_size==4
            expected=bytearray(new.bytes);offset=new.imm_offset
            assert int.from_bytes(expected[offset:offset+4],'little')==0x1d00
            expected[offset:offset+4]=(0x4f00).to_bytes(4,'little')
            assert bytes(expected)==old.bytes
            for i,(x,y) in enumerate(zip(old.bytes,new.bytes)):
                if x!=y:authorized[at+i]=(x,y)
            sites.append(dict(function=name,**row,before_bytes=old.bytes.hex()))
    differences=[]
    for name,record in original.items():
        other=current[name]
        for key in ['rva','virtual_size','raw_size','characteristics']:assert record[key]==other[key],(name,key)
        a=before.sections[name];b=after.sections[name]
        old=bytearray(before.data[a.PointerToRawData:a.PointerToRawData+a.SizeOfRawData])
        new=bytearray(after.data[b.PointerToRawData:b.PointerToRawData+b.SizeOfRawData])
        for data,section in [(old,record),(new,other)]:
            for field in section['volatile_fields']:data[field['offset']:field['offset']+field['size']]=b'\0'*field['size']
        for i,(x,y) in enumerate(zip(old,new)):
            if x==y:continue
            at=0x2000000+record['rva']+i
            assert authorized.get(at)==(x,y),(name,hex(at),hex(x),hex(y))
            differences.append(dict(address=hex(at),before=x,after=y))
    assert len(differences)==len(authorized) and sites
    report=dict(status='Every normalized runtime-section difference is an established crew-event immediate.',before_dll_sha256=sha(before.data),after_dll_sha256=sha(after.data),after_dbg_sha256=sha(debug.data),runtime_sections=len(original),crew_consumers=len(crew_ranges(debug)),event_sites=sites,changed_runtime_bytes=differences,inputs={path:sha((ROOT/path).read_bytes()) for path in [proof['native_absolute_bindings_recovery'],proof['native_header_enums_recovery'],'defs.h','tools/recovery/audit_header_enum_runtime_delta.py']},scope='The sealed linker-only build and current build have identical runtime-section layouts. Every changed payload byte belongs to a decoded immediate changing0x4f00 to0x1d00 in the3 reviewed crew consumers. Export timestamp is normalized; debug sections and debuglink are excluded. This is a batch delta check, not a whole-game equivalence claim.')
    output=ROOT/'docs/recovery-evidence/header-enum-runtime-delta.json'
    output.write_bytes((json.dumps(report,indent=2)+'\n').encode('utf8'))
    print(json.dumps(dict(runtime_sections=len(original),crew_consumers=report['crew_consumers'],event_sites=len(sites),changed_runtime_bytes=len(differences))))


if __name__=='__main__':main()
