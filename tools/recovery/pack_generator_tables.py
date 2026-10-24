"""Decode pack collation weights and the generator's local initialization image."""
import struct
from inspect_release import value,inherited_value
from export_abi import type_of
from pack_native_tables import unqual
from recover_temporary_exile import sha


def decode(debug,runtime):
    tables={};layouts={};once=None;choice_array=None
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/packs.c':continue
        for d in cu.iter_DIEs():
            name=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_structure_type' and name=='count_and_card_t':
                layouts[name]=dict(bytes=value(d,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in d.iter_children() if c.tag=='DW_TAG_member'})
            if d.tag!='DW_TAG_variable':continue
            if name=='once' and inherited_value(d.get_parent(),'DW_AT_name')=='test_packs':
                once=dict(bytes=value(unqual(type_of(d)),'DW_AT_byte_size'),location=d.attributes['DW_AT_location'].value)
            if name=='choices' and inherited_value(d.get_parent(),'DW_AT_name')=='make_pack_normal':
                t=unqual(type_of(d));assert t.tag=='DW_TAG_array_type';bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'];assert bounds==[43]
                choice_array=dict(elements=44,element_bytes=value(unqual(type_of(t)),'DW_AT_byte_size'),location=d.attributes['DW_AT_location'].value)
            if d.get_parent().tag!='DW_TAG_compile_unit' or 'DW_AT_location' not in d.attributes:continue
            t=unqual(type_of(d))
            if t.tag!='DW_TAG_array_type':continue
            elem=unqual(type_of(t))
            if value(elem,'DW_AT_name')!='count_and_card_t':continue
            bound=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'];assert len(bound)==1 and isinstance(bound[0],int)
            loc=d.attributes['DW_AT_location'].value;assert len(loc)==5 and loc[0]==3
            address=struct.unpack('<I',bytes(loc[1:]))[0];count=bound[0]+1;data=get(address,count*8);assert len(data)==count*8
            tables[name]=dict(address=f'{address:x}',count=count,element_bytes=8,bytes=data.hex(),sha256=sha(data))
    assert len(tables)==18 and layouts['count_and_card_t']==dict(bytes=8,members=dict(count=0,csvid=4))
    assert once==dict(bytes=4,location=[3,184,82,121,2]) and choice_array['element_bytes']==8
    pointers={int(r['address'],16):n for n,r in tables.items()}
    image=get(0x2721d80,152);rows=list(struct.iter_unpack('<II',image));assert len(rows)==19 and rows[-1]==(0,0) and all(p in pointers for r,p in rows[:-1])
    varying=dict(address='2721d80',bytes=image.hex(),entries=[dict(rarity=r,table=pointers[p]) for r,p in rows[:-1]]+[dict(rarity=0,table=None)])
    contraptions=get(0x276c714,12);assert struct.unpack('<3i',contraptions)==(16,7,2)
    return dict(tables=tables,layouts=layouts,choices=choice_array,once=once,varying=varying,contraptions=dict(address='276c714',bytes=contraptions.hex(),values=[16,7,2]))


def render(data):
    prefix='typedef struct count_and_card_t { int count; int csvid; } count_and_card_t;\n'
    prefix+='typedef struct varying_copies_entry_t { draftrarity_t rarity; const count_and_card_t *counts_and_cards; } varying_copies_entry_t;\n'
    prefix+='_Static_assert(sizeof(count_and_card_t)==8 && sizeof(varying_copies_entry_t)==8, "native collation record widths");\n\n'
    globals_=[]
    for n,r in data['tables'].items():
        rows=list(struct.iter_unpack('<ii',bytes.fromhex(r['bytes'])));assert rows[-1][0]<=0 and all(c>=0 for c,i in rows[:-1])
        declaration=f'static const count_and_card_t {n}[{r["count"]}] = {{\n'+''.join(f'  {{{c}, {i}}},\n' for c,i in rows)+'};'
        globals_.append(dict(source='functions/packs.c',name=n,declaration=declaration,target_address=r['address'],linkage='private',native_bytes=r['count']*8))
    local='  varying_copies_entry_t varying_copies[19] = {\n'+''.join('    {'+str(r['rarity'])+', '+(r['table'] or 'NULL')+'},\n' for r in data['varying']['entries'])+'  };'
    return prefix+'\n\n'.join(g['declaration'] for g in globals_)+'\n',local,globals_
