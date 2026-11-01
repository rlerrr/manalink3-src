"""Verify restored original header identities against native and rebuilt DWARF."""
import json
from pathlib import Path
from canonicalize_helpers import ROOT,sha
from inspect_release import Image,value
from verify_native_absolute_bindings import runtime_sections,stable_sections


def global_enum_values(debug,wanted):
    result={name:set() for name in wanted};dw=debug.dwarf()
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')
        if not source.startswith(('cards/','functions/')):continue
        for d in cu.get_top_DIE().iter_children():
            if d.tag!='DW_TAG_enumeration_type':continue
            for child in d.iter_children():
                if child.tag!='DW_TAG_enumerator':continue
                name=value(child,'DW_AT_name')
                if name in wanted:result[name].add(value(child,'DW_AT_const_value'))
        cu._dielist.clear();cu._diemap.clear()
    return result


def verify(proof,image,root=ROOT):
    path=proof.get('native_header_names_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    original=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(original.data)==record['native_dbg_sha256']
    dw=original.dwarf();cache={}
    expected={r['name']:r['value'] for r in record['enumerators']}
    assert len(expected)==5111
    for row in record['enumerators']:
        offset=int(row['native_enum_die_offset'],16)
        if offset not in cache:
            enum=dw.get_DIE_from_refaddr(offset);assert enum.tag=='DW_TAG_enumeration_type'
            assert value(enum.cu.get_top_DIE(),'DW_AT_name').replace('\\','/')==row['native_source']
            cache[offset]={value(d,'DW_AT_name'):value(d,'DW_AT_const_value') for d in enum.iter_children() if d.tag=='DW_TAG_enumerator'}
        assert cache[offset][row['name']]==row['value']
    compiled=Image((root/'ManalinkEh.dbg').read_bytes());actual=global_enum_values(compiled,expected)
    assert all(actual[n]=={v} for n,v in expected.items()),[(n,actual[n],v) for n,v in expected.items() if actual[n]!={v}][:20]
    edits=[r for r in proof['source_edits'] if r.get('native_header_names_extension')==path]
    assert len(edits)==len(record['source_edits'])
    for row in record['source_edits']:
        assert any(all(r[k]==row[k] for k in ['source','before','after']) for r in edits)
    artifacts=[]
    for key in ['runtime_before','runtime_after']:
        row=record[key];artifact=Image((root/row['path']).read_bytes());assert sha(artifact.data)==row['dll_sha256'];artifacts.append(artifact)
    assert stable_sections(runtime_sections(artifacts[0]))==stable_sections(runtime_sections(artifacts[1]))
    assert record['added_card_ids']==5100 and record['added_other_names']==10
    return dict(restored_card_id_names=5100,restored_other_header_names=10,corrected_dfc_symbol_limit=11,verified_compiled_enumerators=len(expected),unchanged_sealed_runtime_sections=len(runtime_sections(artifacts[1])),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
