"""Check canonical action, protection and sacrifice domains against release DWARF."""
import json,re
from pathlib import Path
from unify_scalar_helpers import ROOT,read,sha,mask

EVIDENCE='docs/recovery-evidence/canonical-helper-domains.json'
DEBUG_SHA='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'

def verify_source(root=ROOT):
    path=root/EVIDENCE
    if not path.exists():return {}
    e=json.loads(path.read_text());assert e['native_debug_sha256']==DEBUG_SHA
    text=read(root/'defs.h');code=mask(text)
    result={}
    for row in e['domains']:
        canonical=row['canonical_definition'];assert text.count(canonical)==1,row['type']
        assert not re.search(r'\}\s*'+row['recovery_type']+r'\s*;',code)
        assert 'typedef '+row['type']+' '+row['recovery_type']+';' in text
        members=row['native'][0]['members']
        for native in row['native']:assert native['members']==members and native['bytes']==4
        parsed={n:int(v) for n,v in re.findall(r'\b(\w+)\s*=\s*(\d+)\s*,',canonical)}
        assert parsed==members,(row['type'],parsed)
        result[row['type']]=dict(bytes=4,members=members,recovery_type=row['recovery_type'])
    return result

def verify_native(root=ROOT):
    result=verify_source(root)
    if not result:return {}
    from inspect_release import Image,value
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();assert sha(data)==DEBUG_SHA
    info=Image(data).dwarf();e=json.loads((root/EVIDENCE).read_text())
    for row in e['domains']:
        for native in row['native']:
            die=info.get_DIE_from_refaddr(int(native['die_offset'],16))
            assert die.tag=='DW_TAG_enumeration_type' and value(die,'DW_AT_byte_size')==4
            assert value(die.cu.get_top_DIE(),'DW_AT_name')==native['source']
            members={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'}
            assert members==native['members']
        for storage in row.get('storage',[]):
            typedef=info.get_DIE_from_refaddr(int(storage['die_offset'],16));assert typedef.tag=='DW_TAG_typedef'
            assert value(typedef,'DW_AT_name')==row['type']
            target=typedef.get_DIE_from_attribute('DW_AT_type')
            assert target.tag==storage['target_tag']=='DW_TAG_enumeration_type'
            assert value(target,'DW_AT_byte_size')==storage['bytes']==4
            assert value(target,'DW_AT_encoding')==storage['encoding']==7
    return result

def verify_compiled(debug,root=ROOT,source=None):
    """The real GCC build must retain native enum members, width and encoding.

    The Microsoft ABI used by the Clang syntax checker forces signed enum
    storage. It cannot check GCC's unsigned action enum representation.
    """
    if source is None:source=verify_source(root)
    if not source:return {}
    from inspect_release import value
    wanted=set(source);found={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/manipulate_and_damage_all.c':continue
        for die in cu.get_top_DIE().iter_children():
            name=value(die,'DW_AT_name')
            if die.tag!='DW_TAG_typedef' or name not in wanted:continue
            target=die.get_DIE_from_attribute('DW_AT_type')
            assert target.tag=='DW_TAG_enumeration_type' and value(target,'DW_AT_byte_size')==4,name
            members={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in target.iter_children() if c.tag=='DW_TAG_enumerator'}
            assert members==source[name]['members'],(name,members)
            if name=='actions_t':assert value(target,'DW_AT_encoding')==7
            found[name]=dict(bytes=4,encoding=value(target,'DW_AT_encoding'),members=members)
        break
    assert set(found)==wanted,(found.keys(),wanted)
    return found

if __name__=='__main__':print(json.dumps(verify_native(),indent=2))
