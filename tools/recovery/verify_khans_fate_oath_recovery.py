"""Audit Khans/Fate/Oath complete bodies, inline helpers and optimized parameters."""
import csv,json,re
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value


def verify(proof,image,root=ROOT):
    path=proof.get('native_khans_fate_oath_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    def native_range(part):
        lo=int(part['address'],16);hi=int(part['end'],16)
        assert sha(image.pe.get_data(lo-0x2000000,hi-lo))==part['bytes_sha256']
        assert sha((root/part['raw_export']).read_bytes())==part['raw_export_sha256']
    for row in record['helpers']+record['card_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        for part in row['native_ranges']:native_range(part)
    for row in record['inline_support_helpers']:
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256']
        assert any(e['source']==row['source'] and e['after_sha256']==row['definition_sha256'] and
          e.get('native_inline_source_reconstruction',{}).get('function')==row['name'] for e in proof['source_edits'])
        for part in row['native_owner_ranges']:native_range(part)
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dw=original.dwarf();inventory={r['low_pc']:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']}
    def die(at):return dw.get_DIE_from_refaddr(int(inventory[at]['die_offset'],16))
    for at in ['0x2199890','0x2199920']:assert inherited_value(die(at),'DW_AT_type') is None
    constants={};stack={}
    for p in die('0x225a450').iter_children():
        if p.tag!='DW_TAG_formal_parameter':continue
        name=inherited_value(p,'DW_AT_name');constant=value(p,'DW_AT_const_value')
        if constant is not None:constants[name]=constant
        loc=p.attributes.get('DW_AT_location')
        if loc and loc.form=='DW_FORM_exprloc':stack[name]=loc.value
    assert constants=={'white':0,'green':0,'black':0}
    assert stack=={'cless':[145,0],'blue':[145,4],'red':[145,8]}
    assert image.pe.get_data(0x199904,8)==bytes.fromhex('c744240802000000')
    assert image.pe.get_data(0x7389be,3)==bytes.fromhex('010c00')
    expected=[1]*29
    for i,v in [(0,2),(4,3),(12,4),(28,5)]:expected[i]=v
    table=image.pe.get_data(0x731500,116)
    assert [int.from_bytes(table[i:i+4],'little') for i in range(0,116,4)]==expected
    # This engine global is a macro, so it has no original COFF variable entry.
    assert image.pe.get_data(0x31b94e,5)==bytes.fromhex('a1f0fe7800')
    assert image.pe.get_data(0x31b956,5)==bytes.fromhex('a3f0fe7800')
    assert re.search(r'#define\s+check_abilities_keywords\s+EXE_DWORD\(0x78fef0\)',read(root/'manalink.h'))
    rebuilt=Image((root/'ManalinkEh.dbg').read_bytes());void_entries=set()
    for cu in rebuilt.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/').removeprefix('./')
        if source!='cards/khans_of_tarkir.c':continue
        for fn in cu.iter_DIEs():
            if fn.tag!='DW_TAG_subprogram' or not value(fn,'DW_AT_low_pc'):continue
            name=inherited_value(fn,'DW_AT_name')
            if name in {'prowess_mode','prowess'}:
                assert inherited_value(fn,'DW_AT_type') is None
                void_entries.add(name)
    assert void_entries=={'prowess_mode','prowess'}
    assert len(record['helpers'])==record['helper_definitions']==35
    assert sum(len(r['native_ranges']) for r in record['helpers'])==record['native_helper_ranges']==35
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==15
    assert len(record['card_callers'])==31 and sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==22
    assert len(record['inline_support_helpers'])==2
    return dict(helper_definitions=35,native_helper_ranges=35,changed_helpers=15,complete_card_callers=31,
      changed_card_callers=22,inline_support_helpers=2,native_and_rebuilt_void_prowess=True,
      optimized_surge_constants=constants,optimized_surge_stack_parameters=stack,native_color_lookup=True,
      scope='Complete source/native-range receipts, inline Raid/chooser ownership, native/rebuilt void Prowess interfaces, mandatory dispatch literal, Surge optimized parameter locations, native dialog token/color table, and combat-estimator global binding. Built-in dialog backends and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
