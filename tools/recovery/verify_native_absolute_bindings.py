"""Verify the append-only recovery of native fixed EXE linker names."""
import json,re,copy
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha
from inspect_release import Image


def absolute_symbols(image):
    result={}
    for name,value,section,kind,storage in image.symbols:
        if section==-1:result.setdefault(name,set()).add(value)
    return {name:sorted(values) for name,values in result.items()}


def canonical_digest(value):return sha(json.dumps(value,sort_keys=True,separators=(',',':')).encode())


def runtime_sections(image):
    """Hash all raw runtime section bytes, identifying only export timestamps."""
    volatile=[]
    image.pe.parse_data_directories(directories=[0])
    export=getattr(image.pe,'DIRECTORY_ENTRY_EXPORT',None)
    if export:volatile.append((export.struct.get_field_absolute_offset('TimeDateStamp'),4,'IMAGE_EXPORT_DIRECTORY.TimeDateStamp'))
    result={}
    for name,section in image.sections.items():
        if name.startswith('.debug') or name=='.gnu_debuglink':continue
        start=section.PointerToRawData;data=bytearray(image.data[start:start+section.SizeOfRawData]);masked=[]
        for at,size,reason in volatile:
            if start<=at and at+size<=start+len(data):
                masked.append(dict(offset=at-start,size=size,reason=reason,value=bytes(data[at-start:at-start+size]).hex()))
                data[at-start:at-start+size]=b'\0'*size
        result[name]=dict(rva=section.VirtualAddress,virtual_size=section.Misc_VirtualSize,raw_size=section.SizeOfRawData,characteristics=section.Characteristics,normalized_bytes_sha256=sha(bytes(data)),volatile_fields=masked)
    return result


def stable_sections(sections):
    return {name:{k:v for k,v in row.items() if k!='volatile_fields'} for name,row in sections.items()}


def verify(proof,image,root=ROOT,binary_only=False):
    path=proof.get('native_absolute_bindings_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());source=(root/record['source']).read_bytes();n=record['prefix_bytes']
    assert sha(source[:n])==record['prefix_sha256'] and source[n:]==record['append'].replace('\n','\r\n').encode('ascii')
    assert sha(source)==record['source_sha256']
    original=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(original.data)==record['native_dbg_sha256']
    assert sha(image.data)==record['native_dll_sha256']
    native_abs=absolute_symbols(original);native_dll_abs=absolute_symbols(image)
    current=Image((root/'ManalinkEh.dbg').read_bytes());current_abs=absolute_symbols(current)
    text=read(root/record['source'])
    for row in record['bindings']:
        name=row['name'];value=int(row['address'],16)
        assert value and native_abs[name]==native_dll_abs[name]==current_abs[name]==[value]
        matches=re.findall(r'(?m)^'+re.escape(name)+r'\s*=\s*(0x[0-9a-fA-F]+)\s*;',text)
        assert len(matches)==1 and int(matches[0],16)==value
    for name,values in record['previous_absolute_symbols'].items():assert current_abs[name]==values,name
    for row in record['excluded_zero_entries']:
        assert native_abs[row['name']]==[0]
        assert not re.search(r'(?m)^'+re.escape(row['name'])+r'\s*=',text)
    # Every old receipt is otherwise unchanged. Reconstruct its previous
    # canonical JSON shape by restoring its one historical full-file pin.
    for migration in record['receipt_migrations']:
        receipt=json.loads((root/migration['path']).read_text())
        assert canonical_digest(receipt)==migration['after_canonical_sha256']
        restored=copy.deepcopy(receipt);rows=[r for r in restored['physical_interface_inputs'] if r['path']==record['source']]
        assert len(rows)==1 and rows[0]['sha256']==record['source_sha256']
        rows[0]['sha256']=record['prefix_sha256']
        assert canonical_digest(restored)==migration['before_canonical_sha256']
    # Preserve the linker-only build as historical evidence. Later source
    # recoveries can legitimately change the current runtime code/data.
    linked=record['linker_only_build'];artifact=Image((root/linked['path']).read_bytes())
    assert sha(artifact.data)==linked['dll_sha256']
    actual=runtime_sections(artifact)
    assert stable_sections(actual)==stable_sections(record['runtime_sections_before']), 'Runtime section contents/layout changed'
    assert len(record['bindings'])==54 and len(record['excluded_zero_entries'])==2
    return dict(restored_native_absolute_names=54,unchanged_existing_absolute_names=len(record['previous_absolute_symbols']),archived_linker_only_runtime_sections_verified=len(actual),migrated_linker_receipts=len(record['receipt_migrations']),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
