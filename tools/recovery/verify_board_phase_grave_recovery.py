"""Audit board/phase/grave receipts and the paired 22-entry Shirei storage."""
import csv,json
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value
from verify_land_hybrid_core_recovery import variables,array_shape

def verify(proof,image,root=ROOT):
    path=proof.get('native_board_phase_grave_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['helpers']+record['support_helpers']+record['card_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:
            lo=int(part['address'],16);hi=int(part['end'],16)
            assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
            assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dwarf=original.dwarf();enum=record['native_enum'];die=dwarf.get_DIE_from_refaddr(int(enum['die_offset'],16))
    assert value(die,'DW_AT_name')==enum['name'] and value(die,'DW_AT_const_value')==32
    assert 'GFPC_INTO_WHOSE_GRAVE\t\t= 1<<5' in read(root/'defs.h')
    row=next(r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']=='0x256ec30')
    fn=dwarf.get_DIE_from_refaddr(int(row['die_offset'],16))
    expected={name:[22] for name in ['dead','to_reanimate']}
    native_arrays={inherited_value(v,'DW_AT_name'):array_shape(v.get_DIE_from_attribute('DW_AT_type'),dwarf)
      for v in variables(fn) if inherited_value(v,'DW_AT_name') in expected}
    assert native_arrays==expected,native_arrays
    layouts=[]
    for d in fn.cu.iter_DIEs():
        if d.tag=='DW_TAG_structure_type' and value(d,'DW_AT_name')=='card_instance_struct':
            offsets={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in d.iter_children() if c.tag=='DW_TAG_member'}
            layouts.append((offsets['color_id'],offsets['targets']))
    assert layouts and all(x==(64,116) for x in layouts),layouts
    # Ghidra's color_id +8*i -32 store at i>=11 is targets[i-11].card.
    assert all(64+8*i-32==116+8*(i-11)+4 for i in range(11,22))
    assert value(dwarf.get_DIE_from_refaddr(0x6986921),'DW_AT_external') is True
    rebuilt=Image((root/'ManalinkEh.dbg').read_bytes());dw=rebuilt.dwarf();arrays={}
    external=[]
    for cu in dw.iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/').removeprefix('./')
        if source=='functions/multiblock.c':
            for f in cu.iter_DIEs():
                if f.tag=='DW_TAG_subprogram' and inherited_value(f,'DW_AT_name')=='is_multiblocker_card_copying' and value(f,'DW_AT_low_pc'):
                    external.append(value(f,'DW_AT_external'))
        if source!='cards/betrayers_of_kamigawa.c':continue
        for f in cu.iter_DIEs():
            if f.tag=='DW_TAG_subprogram' and inherited_value(f,'DW_AT_name')=='card_shirei_shizos_caretaker' and value(f,'DW_AT_low_pc'):
                arrays={inherited_value(v,'DW_AT_name'):array_shape(v.get_DIE_from_attribute('DW_AT_type'),dw)
                        for v in variables(f) if inherited_value(v,'DW_AT_name') in expected}
    assert arrays==expected,arrays
    assert external and all(x is True for x in external),external
    assert len(record['support_helpers'])==1
    assert len(record['helpers'])==record['helper_definitions']==record['new_native_helper_ranges']==35
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==18
    assert len(record['card_callers'])==record['changed_card_callers']==1
    return dict(helper_definitions=35,native_helper_ranges=35,changed_helpers=18,changed_card_callers=1,
      native_and_rebuilt_shirei_arrays=arrays,storage_target_pairs=11,native_graveyard_owner_mode=32,native_and_rebuilt_shared_predicate_external=True,
      scope='Complete source/native-range receipts, original/rebuilt 22-entry Shirei arrays and native offset translation. Built-in dialog backends and gameplay tests remain downstream.')

if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
