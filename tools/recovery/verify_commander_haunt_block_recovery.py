"""Audit Commander/haunt/multiblock source receipts and native part boundaries."""
import json
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image


def verify(proof,image,root=ROOT):
    path=proof.get('native_commander_haunt_block_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['helpers']+record['support_helpers']+record['card_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name'])
        current_hash=sha(text[a:b].encode())
        if current_hash!=row['definition_sha256']:
            # Preserve the older receipt when a later audit restores external linkage only.
            refinements=[e for e in proof['source_edits'] if e['source']==row['source'] and
                e.get('native_linkage_refinement',{}).get('function')==row['name'] and
                e['before_sha256']==row['definition_sha256'] and e['after_sha256']==current_hash]
            assert len(refinements)==1,row['name']
            change=refinements[0]
            assert change['before'].startswith('static ')
            assert change['after']==change['before'].removeprefix('static ')
    for row in record['inline_support_helpers']:
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        assert any(e['source']==row['source'] and e['after_sha256']==row['definition_sha256'] and
                   e.get('native_inline_source_reconstruction',{}).get('owner')==row['native_owner'] for e in proof['source_edits'])
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);helpers={r['name']:r for r in record['helpers']}
    for part in record['native_parts']:
        ranges={r['address']:r for r in helpers[part['function']]['native_ranges']}
        assert set(ranges)=={part['private'],part['public']}
        lo=int(part['public'],16);hi=int(ranges[part['public']]['end'],16)
        branches=[i.op_str for i in decoder.disasm(image.pe.get_data(lo-0x2000000,hi-lo),lo) if i.mnemonic in ['call','jmp']]
        assert part['private'] in branches,(part,branches)
    original=Image((root/'G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes())
    assert sha(original.data)=='7cdd6bfea74652b900250faae275c754ef1d32966b9f314a729e46b980337b85'
    debug=Image((root/'ManalinkEh.dbg').read_bytes());pointer=record['native_contracts']['tasigur_cost_pointer']
    base=int(pointer['cards_ptr_base'],16);assert base+4*pointer['csv_id']==int(pointer['pointer_cell'],16)
    for metadata in [original,debug]:
        assert [(v,s) for n,v,s,k,c in metadata.symbols if n=='_cards_ptr']==[(base,-1)]
    row=helpers['get_random_card_special']['native_ranges'][0];lo=int(row['address'],16);hi=int(row['end'],16)
    ops=[i.op_str for i in decoder.disasm(image.pe.get_data(lo-0x2000000,hi-lo),lo)]
    assert any('0x124' in op for op in ops) and any('0x2e1' in op for op in ops)
    assert len(record['helpers'])==record['helper_definitions']==43
    assert sum(len(r['native_ranges']) for r in record['helpers'])==record['new_native_helper_ranges']==45
    assert len(record['support_helpers'])==1 and len(record['card_callers'])==2 and len(record['inline_support_helpers'])==3
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==5
    assert sum(r['source_changed'] for r in record['card_callers'])==record['changed_card_callers']==1
    return dict(helper_definitions=43,new_manual_ranges=45,changed_helpers=5,changed_card_callers=1,
                support_ranges=1,complete_card_callers=2,inline_support_definitions=3,native_public_private_pairs=2,
                specialized_random_bounds_verified=True,native_and_rebuilt_tasigur_pointer_binding_verified=True,
                scope='Complete native-owner/source/range receipts, public/private pairs, specialized bounds and fixed pointer-cell binding. Built-in choosers, independent callees and gameplay tests remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
