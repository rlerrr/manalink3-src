"""Audit complete convoke/suspend/upkeep receipts and native enum/callback contracts."""
import json
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value

def verify(proof,image,root=ROOT):
    path=proof.get('native_convoke_suspend_upkeep_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['helpers']+record['card_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16)
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dwarf=original.dwarf()
    import re
    header=read(root/'card_id.h')
    for row in record['native_card_ids']:
        die=dwarf.get_DIE_from_refaddr(int(row['die_offset'],16))
        assert die.tag=='DW_TAG_enumerator' and value(die,'DW_AT_name')==row['name']
        assert value(die,'DW_AT_const_value')==row['value']
        assert re.search(r'\b'+row['name']+r'\s*=\s*'+str(row['value'])+r'\b',header)
    rebuilt=Image((root/'ManalinkEh.dbg').read_bytes());found=[]
    for cu in rebuilt.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/').removeprefix('./')
        if source!='cards/ravnica.c':continue
        for die in cu.iter_DIEs():
            if die.tag=='DW_TAG_subprogram' and inherited_value(die,'DW_AT_name')=='savra_trigger' and not value(die,'DW_AT_declaration',False):
                names=[inherited_value(c,'DW_AT_name') for c in die.iter_children() if c.tag=='DW_TAG_formal_parameter']
                if names:found.append(names)
    assert found and all(names==['player','card','amount'] for names in found),found
    assert len(record['helpers'])==record['helper_definitions']==record['new_native_helper_ranges']==47
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==18
    assert len(record['card_callers'])==record['changed_card_callers']==2
    assert len(record['native_card_ids'])==8
    return dict(helper_definitions=47,native_helper_ranges=47,changed_helpers=18,changed_card_callers=2,native_card_ids=8,
      rebuilt_savra_callback_parameters=found,
      scope='Complete source/native-range receipts, native card-id enum values and rebuilt three-parameter Savra callback. Built-in dialog backends and gameplay tests remain downstream.')

if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
