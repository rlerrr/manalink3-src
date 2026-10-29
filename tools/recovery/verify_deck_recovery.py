"""Check the native templates and fixed banks used by recovered deck helpers."""
import json,re
from pathlib import Path
from inspect_release import Image
from canonicalize_helpers import ROOT,read,sha


def verify(proof,image,root=ROOT):
    path=proof.get('native_deck_recovery')
    if not path:return {}
    record=json.loads((root/path).read_text())
    assert sha(image.data)==record['dll_sha256']
    # These templates contain no relocated pointers. Compare every byte in the
    # rebuilt PE, rather than accepting matching structure sizes or field names.
    rebuilt=Image((root/'ManalinkEh.dll').read_bytes())
    debug=Image((root/'ManalinkEh.dbg').read_bytes())
    templates={}
    for row in record['templates']:
        original=image.pe.get_data(int(row['native_address'],16)-image.pe.OPTIONAL_HEADER.ImageBase,row['bytes'])
        assert len(original)==row['bytes'] and sha(original)==row['sha256'],row['name']
        symbols=[s for s in debug.symbols if s[0]=='_'+row['name'] and s[2]>0]
        assert len(symbols)==1,(row['name'],symbols)
        _,offset,section,_,_=symbols[0]
        rva=debug.pe.sections[section-1].VirtualAddress+offset
        assert rebuilt.pe.get_data(rva,row['bytes'])==original,('Native template changed',row['name'])
        templates[row['name']]=dict(bytes=row['bytes'],sha256=row['sha256'])
    linker=read(root/'manalink.lds');source=read(root/'functions/deck.c')
    for name,row in record['fixed_banks'].items():
        assert re.search(r'(?mi)^_'+re.escape(name)+r'\s*=\s*'+row['address']+r';',linker),name
        assert 'extern card_instance_t '+name+'[2][151];' in source,name
        assert row['players']==2 and row['slots']==151 and row['instance_bytes']==300
    # Whole-function receipts are immutable evidence. Later explicit source
    # edits may refine a body, so bind these records to their replayed edits
    # rather than silently overwriting an earlier body hash.
    body_records={(e['source'],e.get('native_source_reconstruction',{}).get('function'),e['after_sha256'])
                  for e in proof['source_edits']}
    for row in record['helpers']:
        assert ('functions/deck.c',row['name'],row['definition_sha256']) in body_records,row['name']
    for row in record['counterspell_callers']:
        assert (row['source'],row['name'],row['definition_sha256']) in body_records,row['name']
    return dict(helper_bodies=record['helper_definitions'],changed_bodies=record['changed_helper_definitions'],
                counterspell_callers=len(record['counterspell_callers']),templates=templates,
                fixed_banks=list(record['fixed_banks']),
                scope='Complete template byte identity, original fixed-bank addresses/dimensions, and explicit full-body replay receipts. Dependency closure remains separate review work.')


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
