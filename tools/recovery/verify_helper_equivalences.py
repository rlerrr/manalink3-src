"""Verify complete original DWARF bounds for shared native helper bodies."""
import json
from pathlib import Path
from inspect_release import Image,value,inherited_value,sha

def verify_bounds(root,entries):
    if not entries:return {}
    proof=json.loads((root/'docs/recovery-evidence/attachment-predicate-equivalence.json').read_text())
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()
    assert sha(data)==proof['debug_sha256']=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dwarf=Image(data).dwarf();checked={}
    for entry in entries:
        for label in ['canonical','duplicate']:
            address=entry[label+'_address'];record=proof['functions'][address]
            die=dwarf.get_DIE_from_refaddr(int(record['die_offset'],16))
            assert die.tag=='DW_TAG_subprogram'
            assert inherited_value(die,'DW_AT_name')==entry['canonical_name']
            assert value(die.cu.get_top_DIE(),'DW_AT_name')==record['source']==entry[label+'_source']
            low=value(die,'DW_AT_low_pc');high=die.attributes['DW_AT_high_pc']
            end=high.value if high.form=='DW_FORM_addr' else low+high.value
            assert low==int(address,16) and end==int(record['end'],16)
            assert end-low==record['bytes']==entry['bytes']
            checked[address]=record
    return dict(debug_sha256=proof['debug_sha256'],whole_function_bounds=checked)
