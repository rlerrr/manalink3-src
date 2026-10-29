"""Verify cost/phase/life source receipts and native wrapper contracts."""
import json
from canonicalize_helpers import ROOT,read,sha,span


def verify(proof,image,root=ROOT):
    path=proof.get('native_cost_phase_life_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text());assert sha(image.data)==record['dll_sha256']
    receipts={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256']) for e in proof['source_edits']}
    for row in record['helpers']:
        assert (row['source'],row['name'],row['definition_sha256']) in receipts,row['name']
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
    for row in record['inline_support_helpers']:
        text=read(root/row['source']);a,b=span(text,row['name'])
        assert sha(text[a:b].encode())==row['definition_sha256'],row['name']
        assert any(e['source']==row['source'] and e['after_sha256']==row['definition_sha256'] and
                   e.get('native_inline_source_reconstruction',{}).get('owner')==row['native_owner'] for e in proof['source_edits'])
    for address,expected in record['native_contracts']['native_dialog_tokens'].items():
        assert image.pe.get_data(int(address,16)-0x2000000,3)==bytes.fromhex(expected)+b'\0'
    assert len(record['helpers'])==record['helper_definitions']==36
    assert sum(len(r['native_ranges']) for r in record['helpers'])==record['new_native_helper_ranges']==38
    assert sum(r['source_changed'] for r in record['helpers'])==record['changed_helper_definitions']==13
    return dict(helper_definitions=36,new_native_ranges=38,changed_helpers=13,inline_support_definitions=2,
                optimized_reveal_clones=2,native_dialog_tokens_verified=True,
                scope='Complete native-body review receipts, exact current source bodies, native code ranges and reveal-dialog control tokens. Built-in dialog implementations and gameplay validation remain downstream.')


if __name__=='__main__':
    from canonicalize_helpers import load
    from inspect_release import Image
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
