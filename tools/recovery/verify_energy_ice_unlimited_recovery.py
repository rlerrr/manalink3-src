"""Audit energy/legacy helper receipts and optimized native parameter contracts."""
import csv,json
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value

def verify(proof,image,root=ROOT):
    path=proof.get('native_energy_ice_unlimited_recovery')
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
    for row in record['inline_support_helpers']:
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256']
        assert any(e['source']==row['source'] and e['after_sha256']==row['definition_sha256'] and
          e.get('native_inline_source_reconstruction',{}).get('owner')==row['native_owner'] for e in proof['source_edits'])
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw=original.dwarf();inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    fn=dw.get_DIE_from_refaddr(int(inventory['0x239bdd0']['die_offset'],16))
    constants={};stack={}
    for p in fn.iter_children():
        if p.tag!='DW_TAG_formal_parameter':continue
        name=inherited_value(p,'DW_AT_name');constant=value(p,'DW_AT_const_value')
        if constant is not None:constants[name]=constant
        loc=p.attributes.get('DW_AT_location')
        if loc and loc.form=='DW_FORM_exprloc':stack[name]=loc.value
    assert constants==record['native_contracts']['boost']['fixed_parameters']=={'pow':1,'key':0},constants
    assert stack==record['native_contracts']['boost']['stack_parameters']=={'tou':[145,0],'s_key':[145,4]},stack
    for address in ['0x239bd80','0x239c350']:
        die=dw.get_DIE_from_refaddr(int(inventory[address]['die_offset'],16))
        assert inherited_value(die,'DW_AT_type') is None,address
    assert image.pe.get_data(0x744eac,3)==bytes.fromhex('010c00')
    assert image.pe.get_data(0x744ea9,3)==bytes.fromhex('010300')
    rebuilt=Image((root/'ManalinkEh.dbg').read_bytes());void_entries=[]
    for metadata in [original,rebuilt]:
        assert [(v,s) for n,v,s,k,c in metadata.symbols if n=='_LEGACY_EFFECT_ACTIVATED_PROTECT']==[(0x739d5c,-1)]
    for cu in rebuilt.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/').removeprefix('./')
        if source!='cards/kaladesh.c':continue
        for fn in cu.iter_DIEs():
            if fn.tag=='DW_TAG_subprogram' and inherited_value(fn,'DW_AT_name')=='attack_trigger_with_energy' and value(fn,'DW_AT_low_pc'):
                assert inherited_value(fn,'DW_AT_type') is None
                void_entries.append(hex(value(fn,'DW_AT_low_pc')))
    assert void_entries
    assert len(record['helpers'])==record['helper_definitions']==40
    assert sum(len(r['native_ranges']) for r in record['helpers'])==record['new_native_helper_ranges']==41
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==19
    assert len(record['card_callers'])==12 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==9
    assert len(record['inline_support_helpers'])==1
    return dict(helper_definitions=40,native_helper_ranges=41,changed_helpers=19,complete_card_callers=12,changed_card_callers=9,
      inline_crew_predicate=True,native_boost_fixed_parameters=constants,native_boost_stack_parameters=stack,
      native_and_rebuilt_void_energy_dispatch=True,
      scope='Complete source/native-range receipts, compiler-specialized boost parameter locations/constants, native dialog tokens and native/rebuilt void energy dispatcher. Built-in dialogs, CRT thunk and gameplay tests remain downstream.')

if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
