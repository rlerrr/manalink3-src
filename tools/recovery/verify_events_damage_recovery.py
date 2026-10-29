"""Bind event/damage recovery receipts to native ABI and shared declarations."""
import json,re
from canonicalize_helpers import ROOT,read,sha,mask,span
from inspect_release import Image


def verify(proof,image,root=ROOT):
    path=proof.get('native_events_damage_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text())
    assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256'])
              for e in proof['source_edits']}
    for row in record['helpers']+record['callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
    flag=record['shared_enum'];header=read(root/'defs.h')
    assert re.search(r'\b'+flag['name']+r'\s*=\s*\(1\s*<<\s*6\)',mask(header))
    assert flag['value']==64 and flag['local_shadow_declarations_removed']==5
    for base in ['cards','functions']:
        for p in (root/base).glob('*.c'):
            text=mask(read(p))
            assert not re.search(r'\b'+flag['retired_name']+r'\b',text),p
            assert not re.search(r'\b'+flag['name']+r'\s*=',text),p
    counter=record['counter_enum']
    enum_text=re.search(r'COUNTER_P1_P1\s*=\s*0.*?COUNTER_invalid\s*=\s*255',mask(header),re.S)
    assert enum_text
    current={name:int(value) for name,value in re.findall(r'(COUNTER_\w+)\s*=\s*(\d+)',enum_text[0])}
    assert current==counter['values']
    from inspect_release import value
    original_debug=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original_debug.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    enum_die=original_debug.dwarf().get_DIE_from_refaddr(int(counter['native_die_offset'],16))
    original_values={value(c,'DW_AT_name'):value(c,'DW_AT_const_value')
                     for c in enum_die.iter_children() if c.tag=='DW_TAG_enumerator'}
    assert original_values==current and value(enum_die,'DW_AT_byte_size')==counter['storage_bytes']==4
    boundary=record['legacy_effect_boundary']
    assert any(s[0]==boundary['native_coff_symbol'] and s[1]==int(boundary['address'],16) and s[2]==-1 and s[4]==2
               for s in original_debug.symbols)
    debug=Image((root/'ManalinkEh.dbg').read_bytes())
    callback=record['equipment_callback']
    symbols=[s for s in debug.symbols if s[0]=='_'+callback['name']]
    assert len(symbols)==1 and symbols[0][2]>0 and symbols[0][3]==32 and symbols[0][4]==2,symbols
    assert 'int equipment_legacy(int player, int card, event_t event);' in read(root/'manalink.h')
    # The address guard must track the rebuilt linker marker; importing the
    # original DLL's upper .text address would reject valid rebuilt functions.
    events=read(root/'functions/events.c');a,b=span(events,'call_card_fn');guard=events[a:b]
    assert 'extern char etext __asm__("etext");' in guard and 'entry > (uintptr_t)&etext' in guard
    assert len([s for s in debug.symbols if s[0]=='etext' and s[2]>0 and s[4]==2])==1
    binding=record['optimized_damage_filter'];witness=binding['call_witness']
    lo,hi=int(witness['start'],16),int(witness['end'],16)
    data=image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo)
    assert sha(data)==witness['bytes_sha256'] and witness['type']==-1 and witness['test']==0
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    instructions=list(Cs(CS_ARCH_X86,CS_MODE_32).disasm(data,lo))
    assert instructions[-1].mnemonic=='call' and instructions[-1].op_str==binding['private_address']
    for operands in ['dword ptr [esp + 0xc], 0','dword ptr [esp + 8], 0xffffffff']:
        assert any(i.mnemonic=='mov' and i.op_str==operands for i in instructions),operands
    damage=read(root/'functions/functions.c');a,b=span(damage,'damage_dealt_by_me_arbitrary')
    assert re.search(r'check_damage_test\(player, card, event, mode, triggering_player, triggering_card, -1, NULL\)',damage[a:b])
    return dict(helper_definitions=record['reviewed_helper_definitions'],native_ranges=record['reviewed_native_helper_ranges'],
                changed_helpers=record['changed_helper_definitions'],callers=len(record['callers']),
                external_equipment_callback=True,shared_planeswalker_permission_flag=True,
                native_counter_members=len(current),counter_local_shadows_removed=counter['local_shadows_removed'],
                optimized_filter_call_verified=True,
                scope='Complete direct-body receipts, native optimized-call witness, shared enum and rebuilt callback/linker symbols. Transitive behavior and gameplay validation remain separate.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
