"""Verify native Dihada selection storage and typed adventure-slot writes."""
import json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from selection_storage_analysis import inspect,target_geometry
from verify_array_role_recovery import variable,replay


def validate(snapshot,row,native=False):
    if row['name']=='card_geyadrone_dihada':
        v=variable(snapshot,'marked')
        assert v['shape']==dict(kind='DW_TAG_array_type',size=1208,dimensions=[151,2],element_size=4,element_encoding=5)
        assert v['frame_offset']+1208<=-8
        for name in ['td','td2']:assert variable(snapshot,name)['shape']==dict(kind='DW_TAG_structure_type',size=84)
        for name in ['p','c']:
            found=[t for t in snapshot['local_types'] if t['name']==name]
            assert found and all(t['kind']=='DW_TAG_base_type' and t['size']==4 and t['encoding']==5 for t in found)
        stores=snapshot['effects']['indexed_accesses'];fields=[]
        for offset in [0,4]:
            sites=[s['address'] for s in stores if s['frame_offset']==v['frame_offset']+offset
                and s['scale']==8 and s['size']==4 and s['access']&2]
            assert len(sites)==2,(row['name'],offset,sites)
            fields.append(dict(offset=offset,sites=sites))
        return dict(marker_bytes=1208,rows=151,columns=2,row_bytes=8,collection_fields=fields,
            reverse_order_scope='Native instruction window review plus exact typed source replay: rows mc-1 through zero. Dynamic values and complete function equivalence are not inferred.')
    assert row['name']=='adventure'
    writes=snapshot['adventure_writes']
    assert [(w['slot_relative_field_offset'],w['result']) for w in writes['writes']]==[(116,'owner'),(120,'iid')]
    assert len(writes['slot_advances'])==1 and writes['slot_advances'][0]['bytes']==8
    if not native:
        slots=[t for t in snapshot['local_types'] if t['name']=='slot']
        assert slots and all(t['kind']=='DW_TAG_pointer_type' and t['target']['kind']=='DW_TAG_structure_type' and t['target']['size']==8 for t in slots)
    return dict(target_owner_field=116,target_iid_field=120,target_stride=8,definite_writes=writes,
        scope='Definite get_card_instance provenance, eight-byte indexed target-slot advance, and owner/IID result field identities. Chosen index, bounds and general aliasing are outside this machine model.')


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    path=proof.get('native_selection_storage_recovery');assert path
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    matches=[r for r in record['rows'] if (r['source'],r['name'],r['before_sha256'])==(source,name,old_sha)]
    assert len(matches)==1;row=matches[0]
    assert sha(row['before_definition'].encode())==old_sha
    assert replay(row['before_definition'],row['changes'])==row['after_definition']
    text=read(root/source);a,b=span(text,name)
    assert text[a:b]==row['after_definition'] and sha(text[a:b].encode())==row['after_sha256']
    receipts=[e for e in proof['source_edits'] if e.get('native_selection_storage_extension')==path
        and e['source']==source and e['before_sha256']==old_sha]
    assert len(receipts)==1 and receipts[0]['after_sha256']==row['after_sha256']
    owner=next(r for r in record['rows'] if r['name']==('adventure' if name=='resolve_an_adventure' else name))
    view=owner['native_snapshot']['view'];lo,hi=int(view['address'],16),int(view['end'],16)
    assert sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo))==view['bytes_sha256']


def verify(proof,image,root=ROOT):
    path=proof.get('native_selection_storage_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    rows=record['rows'];assert len(rows)==3
    native=inspect(image,debug,rows);builds=[]
    for label in ['before','after']:
        artifact=record['runtime_'+label];im=Image((root/artifact['dll_path']).read_bytes());dbg=Image((root/artifact['dbg_path']).read_bytes())
        assert sha(im.data)==artifact['dll_sha256'] and sha(dbg.data)==artifact['dbg_sha256']
        builds.append(inspect(im,dbg,rows))
    current_debug=Image((root/'ManalinkEh.dbg').read_bytes())
    current=inspect(Image((root/'ManalinkEh.dll').read_bytes()),current_debug,rows)
    expected=dict(targets_offset=116,target_bytes=8,fields=dict(player=0,card=4))
    assert target_geometry(debug)==target_geometry(current_debug)==record['target_geometry']==expected
    for row in rows:
        verify_superseded_definition(proof,image,root,row['source'],row['name'],row['before_sha256'])
        if row['name']=='resolve_an_adventure':continue
        key=(row['source'],row['name']);assert native[key]==row['native_snapshot']
        assert builds[0][key]==row['before_compiled'] and builds[1][key]==row['after_compiled']
        assert validate(native[key],row,native=True)==row['native_validation']
        assert validate(builds[1][key],row)==row['compiled_validation'];validate(current[key],row)
    inline=json.loads((root/proof['native_inline_final_definitions_recovery']).read_bytes())
    helper=next(h for h in inline['helpers'] if h['name']=='resolve_an_adventure')
    row=next(r for r in rows if r['name']=='resolve_an_adventure')
    assert helper['definition']==row['before_definition'] and helper['definition_sha256']==row['before_sha256']
    assert helper['native_origins']==record['inline_origins']
    assert inline['unresolved_semantic_limits']==record['retained_inline_semantic_limits']
    for r in rows:
        body=r['after_definition']
        if r['name']=='card_geyadrone_dihada':
            assert 'marked_index = iStack_574 - 1;' in body and 'while (marked_index >= 0);' in body
            assert body.count('marked[iStack_574][0]')==body.count('marked[iStack_574][1]')==2
            assert 'recovered_stack_p' not in body and 'aiStack_4c4' not in body
        else:
            assert 'slot = instance->targets;' in body and 'slot = &instance->targets[iVar1];' in body
            assert 'slot->player = iVar1;' in body and 'slot->card = iVar1;' in body
            assert 'iVar1 != 0x10' in body and '(card_instance_t *)' not in body
            assert body.endswith('  return 0;\n}')
    # Replay the exact native window reviewed for collection and reverse order.
    d=next(r for r in rows if r['name']=='card_geyadrone_dihada')
    window=[i for i in d['native_snapshot']['instructions'] if 0x2236748<=int(i[0],16)<0x223678d]
    assert window==record['native_reverse_loop_review']['instructions']
    assert ['0x2236759','lea','esi, [ebp + eax*8 - 0x4cc]'] in window
    assert ['0x2236771','sub','edi, 8'] in window and ['0x2236789','cmp','eax, esi'] in window
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    changed={p for p in before if before[p]!=after[p]}
    assert changed=={'.recovery/build-gcc93/obj/cards/modern_horizons_2.o','.recovery/build-gcc93/obj/cards/throne_of_eldraine.o'}
    return dict(recovered_card_definitions=1,recovered_helper_definitions=2,marker_rows=151,marker_columns=2,
        marker_bytes=1208,typed_adventure_source_paths=2,compiled_adventure_field_stores=2,
        target_stride=8,retained_inline_semantic_limits=len(record['retained_inline_semantic_limits']),
        changed_linker_objects=2,unchanged_linker_objects=len(before)-2,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
