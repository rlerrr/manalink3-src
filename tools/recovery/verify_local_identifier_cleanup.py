"""Verify function-local spelling edits and unchanged compiled runtime sections."""
import json,re
from pathlib import Path
from canonicalize_helpers import ROOT,read,mask,span,sha
from verify_source_spelling_edits import substitute
from inspect_release import Image,inherited_value
from global_initializers import inherited_type
from object_initializers import unqualify,sizeof
from verify_native_absolute_bindings import runtime_sections,stable_sections


def project(before,old):
    code=mask(before)
    assert re.fullmatch(r'[A-Za-z]*Var\d+',old)
    assert not re.search(r'\binstance\b',code)
    assert len(re.findall(r'\bcard_instance_t\s*\*\s*'+old+r'\s*(?:;|=)',code))==1
    assignments=re.findall(r'\b'+old+r'\s*=(?!=)\s*([^;]+);',code)
    assert assignments and all(re.fullmatch(r'get_card_instance\(\s*player\s*,\s*card\s*\)',v.strip()) for v in assignments)
    assert not re.search(r'(?:\.|->)\s*'+old+r'\b|&\s*'+old+r'\b',code)
    after=substitute(before,{old:'instance'})
    assert before.count('\n')==after.count('\n')
    return after,len(re.findall(r'\b'+old+r'\b',code))


def verify(proof,image,root=ROOT):
    path=proof.get('native_local_identifier_cleanup')
    if not path:return {}
    r=json.loads((root/path).read_bytes());assert sha(image.data)==r['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==r['native_dbg_sha256'];dw=debug.dwarf()
    identities=set();occurrences=0
    for row in r['rows']:
        key=row['source'],row['name'];assert key not in identities;identities.add(key)
        before,after=row['before_definition'],row['after_definition']
        actual,count=project(before,row['old_name']);assert actual==after and count==row['occurrences']
        assert sha(before.encode())==row['before_sha256'] and sha(after.encode())==row['after_sha256']
        text=read(root/row['source']);a,b=span(text,row['name']);assert text[a:b]==after
        candidates=row['native_candidates'];assert candidates
        for candidate in candidates:
            v=dw.get_DIE_from_refaddr(int(candidate['die'],16));assert inherited_value(v,'DW_AT_name')=='instance'
            t=unqualify(inherited_type(v));assert t.tag=='DW_TAG_pointer_type'
            target=unqualify(inherited_type(t));assert target.tag=='DW_TAG_structure_type' and sizeof(target)==300
        edits=[e for e in proof['source_edits'] if e.get('local_identifier_cleanup_extension')==path and (e['source'],e['name'])==key]
        assert len(edits)==1 and all(edits[0][k]==row[k] for k in ['before_sha256','after_sha256'])
        occurrences+=count
    for label in ['before','after']:
        a=r['runtime_'+label];im=Image((root/a['dll_path']).read_bytes())
        assert sha(im.data)==a['dll_sha256'] and sha((root/a['dbg_path']).read_bytes())==a['dbg_sha256']
        assert runtime_sections(im)==a['sections']
    expected=stable_sections(r['runtime_before']['sections'])
    assert expected==stable_sections(r['runtime_after']['sections'])
    # Runtime identity is a property of the two pinned naming builds. Later
    # independently verified changes elsewhere can legitimately alter the DLL.
    # Every naming-touched definition is still checked against live source above.
    assert len(identities)==r['counts']['functions'] and occurrences==r['counts']['identifier_occurrences']
    assert len({s for s,n in identities})==r['counts']['source_units']
    return dict(renamed_function_locals=len(identities),source_units=r['counts']['source_units'],identifier_occurrences=occurrences,
                unchanged_runtime_sections=len(expected),scope=r['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
