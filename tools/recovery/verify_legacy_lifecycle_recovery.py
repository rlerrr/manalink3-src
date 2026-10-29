"""Verify legacy/lifecycle receipts and the native void/status-bit contract."""
import json
from canonicalize_helpers import ROOT,read,sha,span,declarations
from inspect_release import Image,value,inherited_value


def void_functions(image):
    result=[]
    for cu in image.dwarf().iter_CUs():
        top=cu.get_top_DIE();unit=value(top,'DW_AT_name','').replace('\\','/')
        if not unit.endswith('functions/functions.c'):continue
        for die in cu.iter_DIEs():
            if die.tag!='DW_TAG_subprogram' or not value(die,'DW_AT_low_pc'):continue
            if inherited_value(die,'DW_AT_name','')=='cannot_be_countered':
                assert inherited_value(die,'DW_AT_type') is None,'Native uncounterability helper must return void'
                result.append(value(die,'DW_AT_low_pc'))
    return result


def verify(proof,image,root=ROOT):
    path=proof.get('native_legacy_lifecycle_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256'])
              for e in proof['source_edits']}
    for row in record['helpers']+record['support_helpers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
    source=read(root/'functions/functions.c');header=read(root/'manalink.h')
    assert 'void cannot_be_countered(int player, int card, event_t event);' in header
    assert len(declarations(source,'find_obliviation_legacy'))==1
    assert source.index(declarations(source,'find_obliviation_legacy')[0])<span(source,'true_return_from_oblivion')[0]
    for name in ['create_legacy_effect_for_opponent','effect_follows_control_of_attachment']:
        a,b=span(source,name);body=source[a:b]
        assert 'memcpy(' in body and 'sizeof(card_instance_t)' in body and 'memcpy_inst_and_ext(' not in body
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    debug=Image((root/'ManalinkEh.dbg').read_bytes())
    assert void_functions(original)==[0x267f650]
    assert len(void_functions(debug))==1
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    instructions=list(Cs(CS_ARCH_X86,CS_MODE_32).disasm(image.pe.get_data(0x67f650,65),0x267f650))
    tail=[(i.mnemonic,i.op_str) for i in instructions if 0x267f686<=i.address<=0x267f68c]
    assert tail==[('mov','edx, dword ptr [eax + 0x18]'),('or','dh, 2'),('mov','dword ptr [eax + 0x18], edx')]
    assert len(record['helpers'])==record['helper_definitions']==record['new_native_helper_ranges']==33
    return dict(helper_definitions=33,new_native_ranges=33,changed_helpers=record['changed_helper_definitions'],
                support_helpers=len(record['support_helpers']),native_and_rebuilt_void_contract=True,
                native_uncounterability_status_write=True,private_finder_declaration_moved=True,native_shallow_copies_retained=True,
                scope='Complete direct-body replay receipts, original/rebuilt return type, physical native status write, private declaration placement, and retained native shallow-copy contracts. Gameplay validation remains downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
