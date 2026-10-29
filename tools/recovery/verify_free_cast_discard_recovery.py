"""Verify free-casting/discard receipts, layouts and formatting API bindings."""
import json
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value


def address(image,name):
    values=set()
    for symbol,val,section,kind,storage in image.symbols:
        if symbol==name and section>0:
            values.add(image.pe.OPTIONAL_HEADER.ImageBase+image.pe.sections[section-1].VirtualAddress+val)
    assert len(values)==1,(name,values)
    return values.pop()


def calls_backend(binary,start,backend):
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);calls=[];ended=False
    for ins in decoder.disasm(binary.pe.get_data(start-binary.pe.OPTIONAL_HEADER.ImageBase,256),start):
        if ins.mnemonic=='call':calls.append(ins.op_str)
        if ins.mnemonic=='ret':ended=True;break
    assert ended and calls==[hex(backend)],(hex(start),calls,hex(backend))


def record_fields(die):
    return dict(byte_size=value(die,'DW_AT_byte_size'),fields={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in die.iter_children() if c.tag=='DW_TAG_member'})


def verify(proof,image,root=ROOT):
    path=proof.get('native_free_cast_discard_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['helpers']+record['support_helpers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
    for row in record['inline_support_helpers']:
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        assert any(e['source']==row['source'] and e['after_sha256']==row['definition_sha256'] and
                   e.get('native_inline_source_reconstruction',{}).get('owner')==row['native_owner'] for e in proof['source_edits'])
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    dwarf=original.dwarf();flags=record['native_discard_flags'];native=dwarf.get_DIE_from_refaddr(int(flags['die_offset'],16))
    enum={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in native.iter_children() if c.tag=='DW_TAG_enumerator'}
    assert enum==flags['values']==dict(DISC_RANDOM=1,DISC_CAUSED=2,DISC_OPTIONAL=4,DISC_JUST_MARK=8)
    assert value(native,'DW_AT_byte_size')==flags['byte_size']==4
    layout=record['native_rules_layout'];native=dwarf.get_DIE_from_refaddr(int(layout['die_offset'],16))
    expected=dict(byte_size=layout['byte_size'],fields=layout['fields']);assert record_fields(native)==expected
    debug=Image((root/'ManalinkEh.dbg').read_bytes());built=Image((root/'ManalinkEh.dll').read_bytes());enums=[];layouts=[]
    for cu in debug.dwarf().iter_CUs():
        unit=value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/').removeprefix('./')
        if unit not in ['functions/functions.c','functions/rules_engine.c']:continue
        for die in cu.iter_DIEs():
            if unit=='functions/functions.c' and die.tag=='DW_TAG_enumeration_type':
                vals={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'}
                if 'DISC_RANDOM' in vals:enums.append((vals,value(die,'DW_AT_byte_size')))
            if unit=='functions/rules_engine.c' and die.tag=='DW_TAG_typedef' and value(die,'DW_AT_name')=='rules_engine_globals_t':
                layouts.append(record_fields(die.get_DIE_from_attribute('DW_AT_type')))
    assert enums==[(enum,4)],enums
    assert layouts==[expected],layouts
    for metadata,binary in [(original,image),(debug,built)]:
        backend=address(metadata,'___ms_vsnprintf')
        for name in ['_scnprintf','_vscnprintf']:calls_backend(binary,address(metadata,name),backend)
    token=record['native_contracts']['native_dialog_token']
    assert image.pe.get_data(int(token['address'],16)-0x2000000,3)==bytes.fromhex(token['bytes'])
    assert len(record['helpers'])==record['helper_definitions']==record['new_native_helper_ranges']==52
    assert len(record['support_helpers'])==3 and len(record['inline_support_helpers'])==2
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==12
    return dict(helper_definitions=52,new_manual_ranges=52,changed_helpers=12,previously_retained_support_ranges=3,
                inline_support_definitions=2,native_discard_flags=enum,rules_record_bytes=152,
                native_and_rebuilt_rules_layout_verified=True,native_and_rebuilt_ms_formatting_calls_verified=True,
                scope='Complete body/source/native-range receipts, original/rebuilt discard enum and rules record offsets, MS formatting API calls. Built-in chooser bodies, library backend implementations and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
