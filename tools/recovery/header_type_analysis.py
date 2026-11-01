"""Compare common-CU named DWARF types without mistaking aliases for ABI bugs."""
import collections,json
from inspect_release import value
from global_initializers import inherited_type
from object_initializers import sizeof,unqualify,array_count,member_offset


def descriptor(d,seen=()):
    d=unqualify(d)
    if d is None:return ['void']
    name=value(d,'DW_AT_name')
    try:size=sizeof(d)
    except ValueError:size=None
    if d.offset in seen:return [d.tag,name,size]
    seen=seen+(d.offset,)
    if d.tag=='DW_TAG_base_type':return [d.tag,size,value(d,'DW_AT_encoding')]
    if d.tag in ['DW_TAG_pointer_type','DW_TAG_reference_type']:
        target=unqualify(inherited_type(d))
        return [d.tag,size,target.tag if target else 'void',value(target,'DW_AT_name') if target else None]
    if d.tag=='DW_TAG_array_type':
        return [d.tag,size,array_count(d),descriptor(inherited_type(d),seen)]
    if d.tag=='DW_TAG_enumeration_type':
        return [d.tag,size,[[value(c,'DW_AT_name'),value(c,'DW_AT_const_value')]
                          for c in d.iter_children() if c.tag=='DW_TAG_enumerator']]
    if d.tag in ['DW_TAG_structure_type','DW_TAG_class_type','DW_TAG_union_type']:
        fields=[]
        for c in d.iter_children():
            if c.tag not in ['DW_TAG_member','DW_TAG_inheritance'] or value(c,'DW_AT_declaration',False):continue
            fields.append([value(c,'DW_AT_name'),member_offset(c),value(c,'DW_AT_bit_size'),
                           value(c,'DW_AT_bit_offset'),descriptor(inherited_type(c),seen)])
        return [d.tag,size,fields]
    return [d.tag,name,size]


def inventory(image,source='functions/functions.c'):
    result=collections.defaultdict(dict);found=False;dw=image.dwarf()
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!=source:continue
        assert not found;found=True;line=dw.line_program_for_CU(cu)
        for d in cu.iter_DIEs():
            if d.tag not in ['DW_TAG_typedef','DW_TAG_enumeration_type','DW_TAG_structure_type','DW_TAG_union_type']:continue
            name=value(d,'DW_AT_name');t=unqualify(d)
            if not name or t is None or value(t,'DW_AT_declaration',False):continue
            shape=descriptor(t);file_index=value(d,'DW_AT_decl_file')
            filename=line['file_entry'][file_index-1].name.decode('utf-8') if file_index else None
            result[(d.tag,name)][json.dumps(shape,sort_keys=True)]=dict(source=source,die_offset=hex(d.offset),
                decl_file=filename,decl_line=value(d,'DW_AT_decl_line'),layout=shape)
        break
    assert found
    return [dict(tag=tag,name=name,definitions=list(shapes.values())) for (tag,name),shapes in sorted(result.items())]


def promoted_fields(shape,base=0):
    result=[]
    for f in shape[2]:
        if f[0] is None and f[4][0] in ['DW_TAG_structure_type','DW_TAG_class_type','DW_TAG_union_type']:
            result.extend(promoted_fields(f[4],base+f[1]))
        else:result.append([f[0],base+f[1],*f[2:]])
    return result


def compare_shape(native,current,path='',notes=None):
    """Require native members/enum values, with three explicit storage views.

    Pointer target identities are checked, not full subroutine signatures or
    pointer graph reachability. Anonymous records promote members as C does.
    Native raw unused words have independently bounded existing byte views.
    """
    if notes is None:notes=[]
    if native==current:return notes
    assert native[0]==current[0] and native[1]==current[1],(path,native[:2],current[:2])
    tag=native[0]
    if tag=='DW_TAG_enumeration_type':
        left=dict(native[2]);right=dict(current[2])
        assert len(left)==len(native[2]) and len(right)==len(current[2])
        assert all(n in right and right[n]==v for n,v in left.items()),path
        notes.append(dict(path=path,kind='enum_order_or_extra_source_names',
                          extra_names=sorted(set(right)-set(left))))
    elif tag=='DW_TAG_array_type':
        assert native[2]==current[2],path
        compare_shape(native[3],current[3],path+'[]',notes)
    elif tag in ['DW_TAG_structure_type','DW_TAG_class_type','DW_TAG_union_type']:
        left=promoted_fields(native);right=promoted_fields(current)
        used=set();is_instance=path in ['card_instance_t','card_instance_struct']
        for f in left:
            matches=[(i,g) for i,g in enumerate(right) if f[:4]==g[:4]]
            if not matches and is_instance and f[0] in ['unused0x124','unused0x128','unused0x12a']:
                if f[0]=='unused0x128':
                    assert f==['unused0x128',296,None,None,['DW_TAG_base_type',2,7]]
                    parts=[(i,g) for i,g in enumerate(right) if g[0] in ['upkeep_red','upkeep_white']]
                    assert [g for i,g in parts]==[
                        ['upkeep_red',296,None,None,['DW_TAG_base_type',1,8]],
                        ['upkeep_white',297,None,None,['DW_TAG_base_type',1,8]]]
                    used.update(i for i,g in parts)
                    notes.append(dict(path=path+'.'+f[0],kind='native_uint16_as_existing_two_uint8_fields',byte_range=[296,298]))
                    continue
                alias='token_special_keywords' if f[0]=='unused0x124' else 'upkeep_artmana'
                matches=[(i,g) for i,g in enumerate(right) if g[:4]==[alias,*f[1:4]]]
                notes.append(dict(path=path+'.'+f[0],kind='existing_named_storage_view',source_member=alias))
            assert len(matches)==1,(path,f[:4],len(matches))
            i,g=matches[0];used.add(i);compare_shape(f[4],g[4],path+'.'+str(f[0]),notes)
        extras=[g for i,g in enumerate(right) if i not in used]
        if extras:
            assert is_instance,(path,extras)
            expected=[
                ['extension_init_bytes',44,None,None,['DW_TAG_array_type',8,8,['DW_TAG_base_type',1,8]]],
                ['unknown0x37',55,None,None,['DW_TAG_base_type',1,8]],
                ['upkeep_colorless',292,None,None,['DW_TAG_base_type',1,8]],
                ['upkeep_black',293,None,None,['DW_TAG_base_type',1,8]],
                ['upkeep_blue',294,None,None,['DW_TAG_base_type',1,8]],
                ['upkeep_green',295,None,None,['DW_TAG_base_type',1,8]]]
            assert extras==expected,(path,extras)
            notes.append(dict(path=path,kind='existing_overlapping_initializer_and_byte_views',members=[f[0] for f in extras]))
    else:
        assert native==current,(path,native,current)
    return notes


SDK_ALIASES={'DWORD_PTR':('basetsd.h',4),'ULONG_PTR':('basetsd.h',4),'WORD':('minwindef.h',2)}


def compare_inventories(native,current):
    actual={(r['tag'],r['name']):r for r in current};rows=[]
    for r in native:
        key=(r['tag'],r['name']);new=actual.get(key)
        assert len(r['definitions'])==1,key
        if new is None:
            assert r['tag']=='DW_TAG_typedef' and r['name'] in SDK_ALIASES,key
            filename,size=SDK_ALIASES[r['name']];d=r['definitions'][0]
            assert d['decl_file']==filename and d['layout'][0]=='DW_TAG_base_type' and d['layout'][1]==size
            rows.append(dict(tag=r['tag'],name=r['name'],kind='external_sdk_alias_not_emitted_in_this_cu',
                             native_bytes=size,decl_file=filename));continue
        assert len(new['definitions'])==1,key
        old_shape=r['definitions'][0]['layout'];new_shape=new['definitions'][0]['layout']
        notes=compare_shape(old_shape,new_shape,r['name'])
        rows.append(dict(tag=r['tag'],name=r['name'],kind='exact_definition' if old_shape==new_shape else 'compatible_with_explicit_views',notes=notes))
    assert len(native)==99 and len(rows)==99
    return rows
