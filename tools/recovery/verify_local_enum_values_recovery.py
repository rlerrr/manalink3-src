"""Verify retained native local-enum values and the explicit restoration queue."""
import collections,json
from pathlib import Path
from canonicalize_helpers import ROOT,sha
from inspect_release import Image,value,inherited_value
from local_enum_analysis import validate_native,current_enums,source_declaration,match_compiled
from verify_inline_helpers import ranges


def verify(proof,image,root=ROOT):
    path=proof.get('native_local_enum_value_review')
    if not path:return {}
    record=json.loads((root/path).read_bytes());debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(image.data)==record['native_dll_sha256'] and sha(debug.data)==record['native_dbg_sha256']
    dw=debug.dwarf();rows=record['rows'];assert len(rows)==777
    assert len({r['native_enum_die'] for r in rows})==777
    artifact=record['compiled_artifact'];sealed_debug=Image((root/artifact['dbg_path']).read_bytes())
    assert sha(sealed_debug.data)==artifact['dbg_sha256']
    _,sealed=current_enums(sealed_debug,rows)
    _,current=current_enums(Image((root/'ManalinkEh.dbg').read_bytes()),rows)
    initial=collections.Counter();fresh=collections.Counter();cache={}
    for row in rows:
        _,owner=validate_native(dw,row)
        if row['name']=='dominaria_memorial_impl':
            concrete=dw.get_DIE_from_refaddr(int(row['concrete_owner_die'],16))
            origin=concrete.get_DIE_from_attribute('DW_AT_abstract_origin')
            assert origin.offset==owner.offset and ranges(dw,concrete)==[(0x22ca5c0,0x22ca999)]
        assert sha(row['source_review'].get('declaration','').encode())==row['source_review'].get('declaration_sha256',sha(b''))
        initial[row['source_review']['kind']]+=1
        frozen=match_compiled(row,sealed[row['source'],row['name']])
        assert frozen==row['compiled_enum']
        current_source=source_declaration(root,row,cache)
        checked=match_compiled(row,current[row['source'],row['name']])
        if row['source_review']['kind']=='missing_local_enum':
            # This receipt describes the baseline queue. A later restoration
            # may close it, but must emit the exact native names and values.
            assert (checked is None)==(current_source['kind']=='missing_local_enum')
        else:
            assert current_source==row['source_review'] and checked is not None
        fresh[current_source['kind']]+=1
    assert dict(initial)==dict(existing_native_names=486,missing_local_enum=290,existing_name_alias=1)
    alias=[r for r in rows if r.get('source_name_alias')]
    assert len(alias)==1 and alias[0]['name']=='select_a_subtype_impl'
    assert alias[0]['source_name_alias']=={'CHOICE_UNPRINTED':'CHOICE_TOKEN_ONLY'}
    assert alias[0]['enumerators'][-1]==['CHOICE_UNPRINTED',4]
    missing=[r for r in rows if r['source_review']['kind']=='missing_local_enum']
    return dict(native_local_enum_declarations=777,retained_exact_native_declarations=486,
        retained_enumerator_values=sum(len(r['enumerators']) for r in rows if r['source_review']['kind']=='existing_native_names'),
        existing_name_aliases=1,baseline_missing_enum_declarations=290,
        baseline_missing_enumerators=sum(len(r['enumerators']) for r in missing),
        current_source_counts=dict(fresh),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
