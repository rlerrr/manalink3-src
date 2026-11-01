"""Read-only discovery inventory, preserving inconsistent per-CU definitions."""
import collections,json,sys
from pathlib import Path
sys.path.insert(0,'tools/recovery')
from inspect_release import Image,value
from object_initializers import unqualify
from canonicalize_helpers import sha

debug_path,output=sys.argv[1:]
image=Image(Path(debug_path).read_bytes());dw=image.dwarf();groups=collections.defaultdict(dict);units=0
headers={'defs.h','manalink.h','subtypes.h','card_id.h','counters.h'}
for cu in dw.iter_CUs():
    top=cu.get_top_DIE();source=value(top,'DW_AT_name','').replace('\\','/')
    if not source.startswith(('cards/','functions/')):continue
    units+=1;line=dw.line_program_for_CU(cu)
    files=line.header['file_entry'] if line else []
    def filename(d):
        index=value(d,'DW_AT_decl_file')
        if not isinstance(index,int) or not 1<=index<=len(files):return None
        return files[index-1].name.decode('latin1').replace('\\','/').split('/')[-1]
    candidates={}
    for d in top.iter_children():
        if d.tag not in ['DW_TAG_typedef','DW_TAG_enumeration_type']:continue
        enum=unqualify(d)
        if enum is None or enum.tag!='DW_TAG_enumeration_type':continue
        name=value(d,'DW_AT_name');header=filename(d) or filename(enum)
        if not name or header not in headers:continue
        candidates[(header,name,enum.offset)]=enum
    for (header,name,offset),enum in candidates.items():
        values={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in enum.iter_children() if c.tag=='DW_TAG_enumerator'}
        if not values:continue
        signature=sha(json.dumps(values,sort_keys=True).encode());key=(header,name)
        variant=groups[key].setdefault(signature,dict(values=values,uses=[]))
        variant['uses'].append(dict(source=source,enum_die_offset=hex(offset)))
    cu._dielist.clear();cu._diemap.clear()
result=dict(debug_path=debug_path,debug_sha256=sha(image.data),extractor_sha256=sha(Path(__file__).read_bytes()),project_units=units,headers=sorted(headers),definitions=[dict(header=h,name=n,variants=list(variants.values())) for (h,n),variants in sorted(groups.items())],scope='Discovery-only top-level named header enum definitions. Inconsistent native compilation-unit definitions remain distinct; missing names or values require source and machine-code review before any change.')
Path(output).write_bytes((json.dumps(result,indent=2)+'\n').encode('utf8'))
print(json.dumps(dict(project_units=units,named_header_enums=len(groups),variants=sum(map(len,groups.values())),output=output)))
