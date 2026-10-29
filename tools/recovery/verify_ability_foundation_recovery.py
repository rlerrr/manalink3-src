"""Verify ability recovery receipts, native worker contracts and source moves."""
import json,re
from canonicalize_helpers import ROOT,read,sha,mask,span
from inspect_release import Image,value,inherited_value


def source_units_for(image,names):
    found={name:set() for name in names}
    for cu in image.dwarf().iter_CUs():
        top=cu.get_top_DIE();source=value(top,'DW_AT_name','').replace('\\','/')
        if not source.endswith(('functions/abilities.c','functions/functions.c','functions/has_creature_type.c')):continue
        for die in top.iter_children():
            if die.tag!='DW_TAG_subprogram' or not value(die,'DW_AT_low_pc'):continue
            name=inherited_value(die,'DW_AT_name','')
            if name in found:found[name].add(source)
    return found


def verify(proof,image,root=ROOT):
    path=proof.get('native_ability_foundation_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256'])
              for e in proof['source_edits']}
    for row in record['helpers']+record['callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
    inline=record['inline_helpers'][0]
    assert any(e['source']==inline['source'] and e['after_sha256']==inline['definition_sha256']
               and e.get('native_inline_worker_reconstruction',{}).get('source_function')==inline['name']
               for e in proof['source_edits'])
    retired=record['retired_predecessors'];header=mask(read(root/'manalink.h'))
    for base in ['cards','functions']:
        for p in (root/base).glob('*.c'):
            text=mask(read(p))
            assert not any(re.search(r'\b'+name+r'\b',text) for name in retired),p
    assert not any(re.search(r'\b'+name+r'\b',header) for name in retired)
    debug=Image((root/'ManalinkEh.dbg').read_bytes())
    original_debug=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original_debug.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    moved=[r for r in record['helpers'] if r['source_moved']]
    assert len(moved)==record['moved_definitions']==2
    original_units=source_units_for(original_debug,[r['name'] for r in moved])
    rebuilt_units=source_units_for(debug,[r['name'] for r in moved])
    for row in moved:
        name=row['name'];source=row['source']
        assert original_units[name]==rebuilt_units[name]=={source},(name,original_units[name],rebuilt_units[name])
        assert span(read(root/source),name) is not None and span(read(root/row['previous_source']),name) is None
        symbols=[s for s in debug.symbols if s[0]=='_'+name]
        assert len(symbols)==1 and symbols[0][2]>0 and symbols[0][3]==32 and symbols[0][4]==2,(name,symbols)
    for name in retired:
        assert not any(s[0]=='_'+name or s[0].startswith('_'+name+'.') for s in debug.symbols),name
    # The public frontend loads color bits into ECX. The optimized worker has
    # no event argument; Ghidra's raw fourth parameter is not source ABI.
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    decoder=Cs(CS_ARCH_X86,CS_MODE_32)
    worker=list(decoder.disasm(image.pe.get_data(0x6dd300,57),0x26dd300))
    frontend=list(decoder.disasm(image.pe.get_data(0x6e1000,52),0x26e1000))
    assert any(i.mnemonic=='mov' and i.op_str=='ecx, dword ptr [ebp + 0x14]' for i in frontend)
    assert any(i.mnemonic=='call' and i.op_str=='0x26dd300' for i in frontend)
    assert any(i.mnemonic=='and' and i.op_str=='ecx, 0x3e' for i in worker)
    assert any(i.mnemonic=='and' and i.op_str=='esi, 0x40' for i in worker)
    assert len([i for i in worker if i.mnemonic=='call'])==1
    haste=list(decoder.disasm(image.pe.get_data(0x6df3b0,42),0x26df3b0))
    assert any(i.mnemonic=='and' and i.op_str=='edx, 0xfffcffff' for i in haste)
    return dict(helper_definitions=record['helper_definitions'],new_native_helper_ranges=record['new_native_helper_ranges'],
                changed_helpers=record['changed_helper_definitions'],moved_definitions=len(moved),
                callers=len(record['callers']),retired_predecessors=len(retired),
                optimized_color_worker_verified=True,native_haste_bits_verified=True,
                scope='Complete direct-body replay receipts, native/rebuilt source-unit and public-symbol identity for moved definitions, retired-symbol absence, and physical color-worker/haste witnesses. Gameplay validation remains downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
