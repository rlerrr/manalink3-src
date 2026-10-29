"""Bind control/legend recovery to native enums, source placement and ABI bytes."""
import json,re
from canonicalize_helpers import ROOT,read,sha,span,mask
from inspect_release import Image,value,inherited_value


def rebuilt_declarations(image,markers):
    enums={};units={}
    for cu in image.dwarf().iter_CUs():
        top=cu.get_top_DIE();unit=value(top,'DW_AT_name','').replace('\\','/')
        if not unit.endswith(('functions/gain_control_and_legend_rule.c','functions/functions.c')):continue
        for die in cu.iter_DIEs():
            if die.tag=='DW_TAG_enumeration_type' and unit.endswith('functions/gain_control_and_legend_rule.c'):
                vals={value(c,'DW_AT_name'):value(c,'DW_AT_const_value')
                      for c in die.iter_children() if c.tag=='DW_TAG_enumerator'}
                for marker in markers:
                    if marker in vals:enums[marker]=(vals,value(die,'DW_AT_byte_size'))
            if die.tag=='DW_TAG_subprogram' and value(die,'DW_AT_low_pc'):
                name=inherited_value(die,'DW_AT_name','')
                if name=='is_legendary':units.setdefault(name,set()).add(unit)
    return enums,units


def verify(proof,image,root=ROOT):
    path=proof.get('native_control_legend_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256'])
              for e in proof['source_edits']}
    for row in record['helpers']+record['callers']+record['support_helpers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
    for p in list((root/'cards').glob('*.c'))+list((root/'functions').glob('*.c'))+[root/'defs.h',root/'manalink.h']:
        text=mask(read(p))
        assert not any(re.search(r'\b'+name+r'\b',text) for name in record['retired_predecessors']),p
    source='functions/gain_control_and_legend_rule.c'
    text=read(root/source);a,b=span(text,'switch_control')
    assert 'u4585f0(player, legacy_card);' in text[a:b]
    assert span(read(root/'functions/functions.c'),'is_legendary') is None
    assert span(text,'is_legendary') is not None
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    debug=Image((root/'ManalinkEh.dbg').read_bytes())
    enums,units=rebuilt_declarations(debug,record['native_enums'])
    for marker,row in record['native_enums'].items():
        die=original.dwarf().get_DIE_from_refaddr(int(row['die_offset'],16))
        vals={value(c,'DW_AT_name'):value(c,'DW_AT_const_value')
              for c in die.iter_children() if c.tag=='DW_TAG_enumerator'}
        assert vals==row['values']==enums[marker][0],marker
        assert value(die,'DW_AT_byte_size')==row['byte_size']==enums[marker][1]==4,marker
    assert units['is_legendary']=={source},units
    assert len([s for s in debug.symbols if s[0]=='_is_legendary' and s[2]>0 and s[3]==32 and s[4]==2])==1
    assert not any(s[0]=='_real_switch_control' or s[0].startswith('_real_switch_control.') for s in debug.symbols)
    assert len([s for s in debug.symbols if s[0]=='_gain_control_impl' and s[2]>0 and s[3]==32 and s[4]==2])==1
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    decoder=Cs(CS_ARCH_X86,CS_MODE_32)
    call=list(decoder.disasm(image.pe.get_data(0x664373,12),0x2664373))
    assert [(i.mnemonic,i.op_str) for i in call]==[
        ('mov','dword ptr [esp], esi'),('mov','dword ptr [esp + 4], ebx'),('call','0x4585f0')]
    assert len(record['helpers'])==record['helper_definitions']==24
    assert sum(len(r['native_ranges']) for r in record['helpers'])==record['new_native_helper_ranges']==25
    return dict(helper_definitions=24,new_native_ranges=25,changed_helpers=record['changed_helper_definitions'],
                card_callers=len(record['callers']),support_helpers=len(record['support_helpers']),
                native_and_rebuilt_enum_values=sum(len(r['values']) for r in record['native_enums'].values()),
                moved_legendary_predicate=True,retired_transfer_worker=True,control_magic_argument_writes_verified=True,
                scope='Direct-body replay, complete native/rebuilt shared enum values and widths, source-unit/public-symbol identity, retired API absence, and physical engine-call ABI witness. Gameplay validation remains downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
