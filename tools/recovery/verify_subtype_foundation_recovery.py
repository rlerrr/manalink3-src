"""Verify native subtype definitions, all report consumers, and lookup data."""
import json,re,struct
from canonicalize_helpers import ROOT,read,sha,span,mask,declarations
from inspect_release import Image,value


def array_count(die):
    while die.tag in {'DW_TAG_typedef','DW_TAG_const_type','DW_TAG_volatile_type'}:
        die=die.get_DIE_from_attribute('DW_AT_type')
    assert die.tag=='DW_TAG_array_type'
    dims=[c for c in die.iter_children() if c.tag=='DW_TAG_subrange_type'];assert len(dims)==1
    count=value(dims[0],'DW_AT_count')
    return count if count is not None else value(dims[0],'DW_AT_upper_bound')+1


def declarations_in(image):
    result=dict(enums=[],reports=[],variables={})
    for cu in image.dwarf().iter_CUs():
        unit=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if unit not in ['functions/has_creature_type.c','cards/mirrodin.c','cards/zendikar_rising.c']:continue
        for die in cu.iter_DIEs():
            if die.tag=='DW_TAG_enumeration_type' and unit=='functions/has_creature_type.c':
                vals={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'}
                if 'SUBTYPE_MAX_USED_CREATURE_SUBTYPE' in vals:
                    result['enums'].append((vals,value(die,'DW_AT_byte_size')))
            if die.tag=='DW_TAG_structure_type':
                for child in die.iter_children():
                    if child.tag=='DW_TAG_member' and value(child,'DW_AT_name')=='reported_subtypes':
                        result['reports'].append((unit,value(die,'DW_AT_byte_size'),array_count(child.get_DIE_from_attribute('DW_AT_type')),
                                                  value(child,'DW_AT_data_member_location')))
            if die.tag=='DW_TAG_variable' and unit=='functions/has_creature_type.c':
                name=value(die,'DW_AT_name','')
                if name.startswith('strs_') or name in ['arr_from_hardcoded_subtype_to_subtype','more_types']:
                    attr=die.attributes.get('DW_AT_location')
                    if attr and len(attr.value)==5 and attr.value[0]==3:
                        result['variables'][name]=dict(address=int.from_bytes(bytes(attr.value[1:]),'little'),
                                                       count=array_count(die.get_DIE_from_attribute('DW_AT_type')))
    return result


def verify(proof,image,root=ROOT):
    path=proof.get('native_subtype_foundation_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['helpers']+record['support_helpers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    types=record['native_types'];dwarf=original.dwarf()
    die=dwarf.get_DIE_from_refaddr(int(types['subtype_t']['die_offset'],16))
    vals={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'}
    assert len(vals)==495 and vals==types['subtype_t']['values'] and value(die,'DW_AT_byte_size')==4
    report=dwarf.get_DIE_from_refaddr(int(types['reported_subtypes_t']['die_offset'],16))
    assert value(report,'DW_AT_byte_size')==104
    field=next(c for c in report.iter_children() if c.tag=='DW_TAG_member' and value(c,'DW_AT_name')=='reported_subtypes')
    assert array_count(field.get_DIE_from_attribute('DW_AT_type'))==26 and value(field,'DW_AT_data_member_location')==0
    debug=Image((root/'ManalinkEh.dbg').read_bytes());rebuilt=declarations_in(debug)
    built=Image((root/'ManalinkEh.dll').read_bytes())
    assert rebuilt['enums']==[(vals,4)]
    assert {row[0] for row in rebuilt['reports']}=={'functions/has_creature_type.c','cards/mirrodin.c','cards/zendikar_rising.c'}
    assert all(row[1:]==(104,26,0) for row in rebuilt['reports']),rebuilt['reports']
    variables=rebuilt['variables']
    for name,count in record['native_contracts']['text_counts'].items():
        assert variables['strs_'+name+'_type']['count']==count,(name,variables)
    tables=record['native_tables']
    for key,symbol in [('hardcoded_table','arr_from_hardcoded_subtype_to_subtype'),('token_more_table','more_types')]:
        row=tables[key];at=int(row['address'],16);data=image.pe.get_data(at-0x2000000,row['element_count']*4)
        assert sha(data)==row['bytes_sha256'] and list(struct.unpack('<'+str(row['element_count'])+'I',data))==row['values']
        assert built.pe.get_data(variables[symbol]['address']-built.pe.OPTIONAL_HEADER.ImageBase,len(data))==data,symbol
    assert variables['more_types']['count']==9
    consumers=record['report_consumers'];assert len(consumers)==9
    expected={}
    for row in consumers:expected.setdefault(row['source'],[]).append(row['function'])
    pattern=re.compile(r'\breported_subtypes(?:_t)?\b')
    new_names='|'.join(re.escape(n) for n in record['enum_added_names'])
    for p in list((root/'cards').glob('*.c'))+list((root/'functions').glob('*.c')):
        text=read(p);code=mask(text);rel=p.relative_to(root).as_posix()
        assert not re.search(r'\bSUBTYPE_HOUND\b',code),rel
        assert not re.search(r'\b(?:'+new_names+r')\s*=',code),rel
        spans=[span(text,name) for name in expected.get(rel,[])];assert all(s is not None for s in spans),rel
        for proto in declarations(text,'get_subtypes_by_csvid'):
            start=text.index(proto);spans.append((start,start+len(proto)))
        for m in pattern.finditer(code):
            assert any(a<=m.start()<b for a,b in spans),(rel,m.start(),'unreviewed subtype-report consumer')
    assert len(record['helpers'])==record['helper_definitions']==record['new_native_helper_ranges']==30
    return dict(helper_definitions=30,new_native_ranges=30,changed_helpers=record['changed_helper_definitions'],
                support_definitions=len(record['support_helpers']),native_enum_members=495,report_bytes=104,report_entries=26,
                report_consumers=9,shared_enum_shadows_removed=len(record['local_enum_shadows_removed']),
                native_and_rebuilt_lookup_tables_verified=True,text_cache_counts_verified=True,
                scope='Full-body replay, complete native/rebuilt enum and record layout in all consumer units, complete source consumer inventory, original/rebuilt table bytes and text cache bounds. Built-in dialog implementations and gameplay validation remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
