"""Verify native database/extension declarations and the bounded type census."""
import collections,json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha
from inspect_release import Image
from header_type_analysis import inventory,compare_inventories
from verify_native_absolute_bindings import runtime_sections,stable_sections


def named(rows,tag,name):
    row=next(r for r in rows if (r['tag'],r['name'])==(tag,name))
    assert len(row['definitions'])==1
    return row['definitions'][0]['layout']


def declarations(rows):
    card=named(rows,'DW_TAG_typedef','card_data_t');assert card[:2]==['DW_TAG_structure_type',72]
    assert next(f for f in card[2] if f[0]=='name')==['name',1,None,None,['DW_TAG_array_type',35,35,['DW_TAG_base_type',1,6]]]
    assert not any(f[0]=='reserved1' for f in card[2])
    extension=named(rows,'DW_TAG_structure_type','card_ext_t');assert extension[:2]==['DW_TAG_structure_type',192]
    assert named(rows,'DW_TAG_typedef','card_ext_t')==extension


def verify(proof,image,root=ROOT):
    path=proof.get('native_header_type_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    original=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)==record['native_dbg_sha256']
    native=inventory(original);assert native==record['native_types'];declarations(native)
    built=[]
    for label in ['runtime_before','runtime_after']:
        artifact=record[label];binary=(root/artifact['path']).read_bytes();debug=(root/artifact['debug_path']).read_bytes()
        assert sha(binary)==artifact['dll_sha256'] and sha(debug)==artifact['dbg_sha256']
        built.append((Image(binary),Image(debug)))
    before=inventory(built[0][1]);after=inventory(built[1][1])
    assert before==record['before_types'] and after==record['after_types'];declarations(after)
    old=named(before,'DW_TAG_typedef','card_data_t')
    assert next(f for f in old[2] if f[0]=='name')==['name',1,None,None,['DW_TAG_array_type',18,18,['DW_TAG_base_type',1,6]]]
    assert next(f for f in old[2] if f[0]=='reserved1')==['reserved1',19,None,None,['DW_TAG_array_type',17,17,['DW_TAG_base_type',1,8]]]
    assert not any((r['tag'],r['name'])==('DW_TAG_structure_type','card_ext_t') for r in before)
    assert named(before,'DW_TAG_structure_type','card_ext_struct')[:2]==['DW_TAG_structure_type',192]
    comparisons=compare_inventories(native,after);assert comparisons==record['comparisons']
    current=inventory(Image((root/'ManalinkEh.dbg').read_bytes()));declarations(current)
    compare_inventories(native,current)
    assert stable_sections(runtime_sections(built[0][0]))==stable_sections(runtime_sections(built[1][0]))
    edits=[e for e in proof['source_edits'] if e.get('native_header_type_extension')==path]
    assert len(edits)==len(record['source_edits'])==3
    header=read(root/'defs.h')
    for row in record['source_edits']:
        assert row['source']=='defs.h' and sha(row['before'].encode())==row['before_sha256'] and sha(row['after'].encode())==row['after_sha256']
        assert header.count(row['after'])==1 and row['before'] not in header
        assert any(e==row for e in edits)
    for label in ['before','after']:
        saved=root/('.recovery/header-types-'+label+'/defs.h')
        assert sha(read(saved).encode())==record['header_'+label+'_sha256']
    assert len(record['compiler_probes'])==2
    for row in record['compiler_probes']:
        assert row['exit_code']==0 and row['language'] in ['C','C++']
        assert sha((root/row['source']).read_bytes())==row['source_sha256']
        assert sha((root/row['object']).read_bytes())==row['object_sha256']
        assert row['header_sha256']==record['header_after_sha256']
    strengthened=record['strengthened_native_record_verifier']
    assert sha((root/strengthened['path']).read_bytes())==strengthened['after_sha256']
    assert sha((root/'.recovery/header-types-before'/strengthened['path']).read_bytes())==strengthened['before_sha256']
    counts=dict(collections.Counter(r['kind'] for r in comparisons))
    assert counts==dict(exact_definition=74,compatible_with_explicit_views=22,external_sdk_alias_not_emitted_in_this_cu=3)
    return dict(reviewed_common_cu_types=99,**counts,native_name_field_bytes=35,
                restored_native_extension_tag='card_ext_t',runtime_sections_unchanged=26,
                compiler_probe_languages=['C','C++'],scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
