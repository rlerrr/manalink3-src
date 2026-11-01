"""Read-only initializer screen, separate from function-local recovery scores."""
import sys,json,collections
from pathlib import Path
sys.path.insert(0,'tools/recovery')
from inspect_release import Image,value,inherited_value
from canonicalize_helpers import sha

def inherited_type(d):
    seen=set()
    while d and d.offset not in seen:
        seen.add(d.offset)
        if 'DW_AT_type' in d.attributes:return d.get_DIE_from_attribute('DW_AT_type')
        attr=next((x for x in ['DW_AT_abstract_origin','DW_AT_specification'] if x in d.attributes),None)
        d=d.get_DIE_from_attribute(attr) if attr else None
    return None

def shape(d,seen=()):
    if d is None or d.offset in seen:return None
    seen=seen+(d.offset,)
    if d.tag in ['DW_TAG_typedef','DW_TAG_const_type','DW_TAG_volatile_type','DW_TAG_restrict_type']:
        return shape(inherited_type(d),seen)
    if d.tag in ['DW_TAG_base_type','DW_TAG_enumeration_type']:
        size=value(d,'DW_AT_byte_size');encoding=value(d,'DW_AT_encoding')
        if size and (d.tag=='DW_TAG_enumeration_type' or encoding in [2,5,6,7,8]):return dict(kind='integer',size=size,encoding=encoding)
        return None
    if d.tag=='DW_TAG_array_type':
        inner=shape(inherited_type(d),seen)
        if not inner:return None
        counts=[]
        for c in d.iter_children():
            if c.tag!='DW_TAG_subrange_type':continue
            count=value(c,'DW_AT_count');upper=value(c,'DW_AT_upper_bound');lower=value(c,'DW_AT_lower_bound',0)
            if count is None and isinstance(upper,int) and isinstance(lower,int):count=upper-lower+1
            if not isinstance(count,int) or count<0:return None
            counts.append(count)
        if not counts:return None
        size=inner['size']
        for count in counts:size*=count
        if 0<size<=1000000:return dict(kind='array',counts=counts,element=inner,size=size)
    return None

def load_bytes(pe,at,size):
    rva=at-pe.OPTIONAL_HEADER.ImageBase;section=pe.get_section_by_rva(rva)
    if section is None or rva+size>section.VirtualAddress+section.Misc_VirtualSize:return None
    offset=rva-section.VirtualAddress
    available=max(0,min(size,section.SizeOfRawData-offset))
    return pe.get_data(rva,available)+b'\0'*(size-available)

def inventory(debug_path,dll_path,output):
    db=Image(Path(debug_path).read_bytes());im=Image(Path(dll_path).read_bytes());dw=db.dwarf();out=[];counts=collections.Counter()
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if not source.startswith(('cards/','functions/')):continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_variable' or value(d,'DW_AT_declaration',False):continue
            loc=d.attributes.get('DW_AT_location');name=inherited_value(d,'DW_AT_name')
            if not loc or not isinstance(loc.value,list) or len(loc.value)!=5 or loc.value[0]!=3 or not name:continue
            at=int.from_bytes(bytes(loc.value[1:]),'little');s=shape(inherited_type(d))
            if s is None:counts['non_arithmetic']+=1;continue
            data=load_bytes(im.pe,at,s['size'])
            if data is None:counts['outside_dll_or_invalid_extent']+=1;continue
            row=dict(source=source,name=name,die_offset=hex(d.offset),address=hex(at),shape=s,size=len(data),bytes=data.hex(),sha256=sha(data))
            parent=d.get_parent();scopes=[]
            while parent and parent.tag!='DW_TAG_compile_unit':
                n=inherited_value(parent,'DW_AT_name')
                if n:scopes.append(n)
                parent=parent.get_parent()
            row['scopes']=list(reversed(scopes));out.append(row)
        cu._dielist.clear();cu._diemap.clear()
    report=dict(debug_path=debug_path,dll_path=dll_path,debug_sha256=sha(db.data),dll_sha256=sha(im.data),extractor_sha256=sha(Path(__file__).read_bytes()),globals=out,excluded=dict(counts),scope='Defined direct-address integer/enum arrays and scalars in all project CUs, including the resumed C++ picker. Excludes pointers, aggregates, dynamic locations and EXE-owned storage. Compares load-time bytes only, not runtime values or complete declaration ABI.')
    Path(output).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(dict(output=output,globals=len(out),excluded=report['excluded'])))

if __name__=='__main__':inventory(*sys.argv[1:])
