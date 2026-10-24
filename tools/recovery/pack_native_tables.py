"""Decode native pack descriptions with DWARF array bounds and pointer identity."""
import struct,re
from inspect_release import value
from export_abi import type_of,type_name
from recover_temporary_exile import sha


def unqual(t):
    while t.tag in {'DW_TAG_const_type','DW_TAG_typedef','DW_TAG_volatile_type'}:t=type_of(t)
    return t


def decode(debug,runtime):
    records={};enums={};layouts={};get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/packs.c':continue
        for d in cu.get_top_DIE().iter_children():
            name=value(d,'DW_AT_name')
            if d.tag=='DW_TAG_typedef' and name in {'expansion_t','draftrarity_t'}:
                t=unqual(d);assert t.tag=='DW_TAG_enumeration_type' and value(t,'DW_AT_byte_size')==4
                enums[name]=[[value(c,'DW_AT_name'),value(c,'DW_AT_const_value')] for c in t.iter_children() if c.tag=='DW_TAG_enumerator']
            if d.tag=='DW_TAG_structure_type' and name in {'rarity_num_t','booster_t','csvid_choices_t'}:
                layouts[name]=dict(bytes=value(d,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in d.iter_children() if c.tag=='DW_TAG_member'})
            if d.tag!='DW_TAG_variable' or 'DW_AT_location' not in d.attributes:continue
            t=unqual(type_of(d))
            if t.tag!='DW_TAG_array_type':continue
            elem=unqual(type_of(t));elemname=value(elem,'DW_AT_name')
            if elemname not in {'rarity_num_t','booster_t'}:continue
            bound=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'];assert len(bound)==1 and isinstance(bound[0],int)
            loc=d.attributes['DW_AT_location'].value;assert len(loc)==5 and loc[0]==3
            address=struct.unpack('<I',bytes(loc[1:]))[0];count=bound[0]+1;width=value(elem,'DW_AT_byte_size');data=get(address,count*width);assert len(data)==count*width
            records[name]=dict(address=f'{address:x}',count=count,element_bytes=width,element_type=elemname,bytes=data.hex(),sha256=sha(data))
    assert layouts['rarity_num_t']==dict(bytes=8,members=dict(rarity=0,num=4))
    assert layouts['booster_t']==dict(bytes=12,members=dict(expansion=0,booster=4,starter=8))
    assert layouts['csvid_choices_t']==dict(bytes=8,members=dict(num=0,csvids=4))
    assert records['pack_contents']['count']==198
    pools={n:r for n,r in records.items() if r['element_type']=='rarity_num_t'}
    pointers={}
    for n,r in pools.items():
        for offset in range(r['count']):
            address=int(r['address'],16)+offset*8;assert address not in pointers;pointers[address]=(n,offset)
    rows=list(struct.iter_unpack('<III',bytes.fromhex(records['pack_contents']['bytes'])))
    for exp,b,s in rows:assert b in pointers and s in pointers
    records['pack_contents']['rows']=[dict(expansion=e,booster=list(pointers[b]),starter=list(pointers[s])) for e,b,s in rows]
    return dict(enums=enums,layouts=layouts,tables=records)


EXPANSION_BEGIN='/* October 2022 native expansion identifiers: begin. */'
EXPANSION_END='/* October 2022 native expansion identifiers: end. */'


def render(data,expansion_header):
    private='typedef enum {\n'+''.join(f'  {n} = {v},\n' for n,v in data['enums']['draftrarity_t'])+'} draftrarity_t;\n\n'
    private+='typedef struct rarity_num_t { draftrarity_t rarity; int num; } rarity_num_t;\n'
    private+='typedef struct booster_t { EXPANSION_t expansion; const rarity_num_t *booster; const rarity_num_t *starter; } booster_t;\n'
    private+='typedef struct csvid_choices_t { int num; int *csvids; } csvid_choices_t;\n'
    private+='_Static_assert(sizeof(rarity_num_t)==8 && sizeof(booster_t)==12 && sizeof(csvid_choices_t)==8, "native pack description layouts");\n\n'
    declarations=[]
    for n,r in data['tables'].items():
        if r['element_type']!='rarity_num_t':continue
        rows=list(struct.iter_unpack('<ii',bytes.fromhex(r['bytes'])));assert rows[-1][0]==0 and all(0<=x<=43 for x,y in rows)
        decl=f'static const rarity_num_t {n}[{r["count"]}] = {{\n'+''.join(f'  {{{x}, {y}}},\n' for x,y in rows)+'};'
        declarations.append(dict(source='functions/packs.c',name=n,declaration=decl,target_address=r['address'],linkage='private',native_bytes=len(bytes.fromhex(r['bytes']))))
    def ptr(item):
        name,offset=item;return name if not offset else f'{name} + {offset}'
    r=data['tables']['pack_contents'];decl='static booster_t pack_contents[198] = {\n'+''.join('  {'+str(row['expansion'])+', '+ptr(row['booster'])+', '+ptr(row['starter'])+'},\n' for row in r['rows'])+'};'
    declarations.append(dict(source='functions/packs.c',name='pack_contents',declaration=decl,target_address=r['address'],linkage='private',native_bytes=198*12))
    base=expansion_header
    if EXPANSION_BEGIN in base:
        start=base.index(EXPANSION_BEGIN);end=base.index(EXPANSION_END,start)+len(EXPANSION_END)+1;base=base[:start]+base[end:]
    existing={n:int(v) for n,v in re.findall(r'\b(EXPANSION_\w+)\s*=\s*(\d+)',base)}
    native=dict(data['enums']['expansion_t']);assert all(native[n]==v for n,v in existing.items() if n in native)
    missing=[(n,v) for n,v in data['enums']['expansion_t'] if n not in existing];assert len(missing)==30
    public=EXPANSION_BEGIN+'\n'+''.join(f'  {n} = {v},\n' for n,v in missing)+EXPANSION_END+'\n'

    return public,private+'\n\n'.join(d['declaration'] for d in declarations)+'\n',declarations
