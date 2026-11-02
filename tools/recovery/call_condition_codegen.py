"""Bound the two manually reviewed GCC scheduling changes in this cleanup."""
import collections,json
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from canonicalize_helpers import sha
from verify_native_absolute_bindings import stable_sections,runtime_sections


def verify(before,before_debug,after,after_debug,review):
    a,b=stable_sections(runtime_sections(before)),stable_sections(runtime_sections(after));assert set(a)==set(b)
    changed=[n for n in a if a[n]!=b[n]];assert changed==['.text','.reloc']
    for name in changed:assert {k:v for k,v in a[name].items() if k!='normalized_bytes_sha256'}=={k:v for k,v in b[name].items() if k!='normalized_bytes_sha256'}
    def functions(im):
        out=collections.defaultdict(set)
        for name,v,s,k,c in im.symbols:
            if k==32 and s>0:out[im.pe.OPTIONAL_HEADER.ImageBase+im.pe.sections[s-1].VirtualAddress+v].add(name)
        return dict(out)
    assert functions(before_debug)==functions(after_debug)
    md=Cs(CS_ARCH_X86,CS_MODE_32);ranges=[];assert {v['name'] for v in review}=={'card_otherworld_atlas','liliana_reanimation_legacy'}
    for row in review:
        assert row['ranges'][0]==row['ranges'][1];lo,hi=map(lambda v:int(v,16),row['ranges'][0]);ranges.append((lo,hi))
        assert '_'+row['name'] in functions(before_debug)[lo]
        for key,im in [('before',before),('after',after)]:
            data=im.pe.get_data(lo-im.pe.OPTIONAL_HEADER.ImageBase,hi-lo);ins=list(md.disasm(data,lo));assert sum(i.size for i in ins)==hi-lo
            assert [[hex(i.address),i.mnemonic,i.op_str] for i in ins]==row[key]
    def inside(va):return any(lo<=va<hi for lo,hi in ranges)
    left=bytearray(before.section_data(before.sections['.text']));right=bytearray(after.section_data(after.sections['.text']));base=before.pe.OPTIONAL_HEADER.ImageBase+before.sections['.text'].VirtualAddress;diffs=sum(x!=y for x,y in zip(left,right))
    for lo,hi in ranges:left[lo-base:hi-base]=right[lo-base:hi-base]=b'\0'*(hi-lo)
    assert left==right and diffs==176
    reloc=[]
    for im in [before,after]:
        im.pe.parse_data_directories(directories=[5]);outside=[];values=collections.Counter()
        for block in im.pe.DIRECTORY_ENTRY_BASERELOC:
            for e in block.entries:
                if not e.type:continue
                va=im.pe.OPTIONAL_HEADER.ImageBase+e.rva
                if inside(va):
                    assert e.type==3;values[im.pe.get_data(e.rva,4).hex()]+=1
                else:outside.append((e.type,e.rva))
        reloc.append((outside,values))
    assert reloc[0]==reloc[1]
    return dict(identical_normalized_runtime_sections=24,different_runtime_sections=changed,changed_code_owners=2,unchanged_cleaned_code_owners=716,differing_text_bytes=176,all_function_addresses_unchanged=True,relocation_targets_unchanged=True,
        scope='All bytes outside two reviewed function ranges are identical. Their instruction snapshots bind a manual review: Atlas reschedules the same14 dialog arguments (including20-5*first_count and5*second_count) and retains its calls/branches; Liliana reschedules the same player/card-id/Zombie arguments before an unchanged subtype call. Relocation-site motion is confined to those ranges and preserves target words. This is bounded codegen corroboration; source/expanded CFG and compiler-type checks establish the local rewrite, not a general machine-code equivalence proof.')
