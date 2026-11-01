"""Verify native reverse-array origins, capacities and paired consumers."""
import collections,json,re
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from reverse_array_analysis import inspect,ARRAYS,CONSUMERS
from verify_array_role_recovery import variable,replay


def validate(snapshot,row):
    name=row['name'];role,dims,width,encoding=ARRAYS[name];v=variable(snapshot,role);size=width
    for d in dims:size*=d
    assert v['shape']==dict(kind='DW_TAG_array_type',size=size,dimensions=dims,element_size=width,element_encoding=encoding)
    assert v['frame_offset']+size<=-8 and snapshot['array_declaration']['has_emitted_location']
    if name=='card_out_of_time':assert snapshot['element_layout']==dict(kind='DW_TAG_base_type',size=4,encoding=5)
    else:
        assert snapshot['element_layout']==dict(kind='DW_TAG_structure_type',size=8,encoding=None,
            fields=[dict(name='player',offset=0,size=4,encoding=5),dict(name='card',offset=4,size=4,encoding=5)])
    flow=snapshot['row_flow'];assert flow
    expected={'_untap_card','_phase_out_target_until_source_leaves_play'} if name=='card_out_of_time' else {'_find_in_owners_graveyard'}
    assert collections.Counter(c['name'] for c in flow['consumer_calls'])==collections.Counter({n:1 for n in expected})
    assert len(flow['same_row_pairs'])==len(flow['consumer_calls'])
    for call in flow['consumer_calls']:
        a,b=CONSUMERS[call['name']]
        assert call['arguments'][a][:2]==['column',0] and call['arguments'][b][:2]==['column',4]
    assert len(flow['indexed_row_origins'])==1 and flow['indexed_row_origins'][0]['row_bytes']==8
    assert flow['indexed_row_origins'][0]['relative_frame_bias']==-8
    assert len(flow['reverse_row_steps'])==1 and flow['reverse_row_steps'][0]['bytes']==-8
    return dict(array=role,bytes=size,dimensions=dims,row_bytes=8,
        consumer_names=sorted(expected),paired_consumer_calls=len(flow['same_row_pairs']),
        row_origin=flow['indexed_row_origins'][0],reverse_step=flow['reverse_row_steps'][0])


def verify(proof,image,root=ROOT):
    path=proof.get('native_reverse_array_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    rows=record['rows'];assert len(rows)==3 and {r['name'] for r in rows}==set(ARRAYS)
    native=inspect(image,debug,rows);builds=[]
    for label in ['before','after']:
        artifact=record['runtime_'+label];im=Image((root/artifact['dll_path']).read_bytes());dbg=Image((root/artifact['dbg_path']).read_bytes())
        assert sha(im.data)==artifact['dll_sha256'] and sha(dbg.data)==artifact['dbg_sha256']
        builds.append(inspect(im,dbg,rows))
    current=inspect(Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()),rows)
    source_forms={
        'pias_revolution_effect':('ptVar7','dead_array + result - 1','if (bVar8)','} while (bVar8);'),
        'card_pias_revolution':('ptVar8','dead_array + iVar4 - 1','if (bVar9)','} while (bVar9);'),
        'card_out_of_time':('paiVar5','marked + iVar4 - 1','if (paiStack_4cc != marked)','} while (paiStack_4cc != marked);')}
    for row in rows:
        key=(row['source'],row['name']);assert native[key]==row['native_snapshot']
        assert builds[0][key]==row['before_compiled'] and builds[1][key]==row['after_compiled']
        assert validate(native[key],row)==row['native_validation']
        assert validate(builds[1][key],row)==row['compiled_validation'];validate(current[key],row)
        # A missing emitted location does not assert that no physical storage
        # existed; it is the exact before-artifact DWARF observation.
        assert builds[0][key]['row_flow'] is None and not builds[0][key]['array_declaration']['has_emitted_location']
        assert builds[0][key]['array_declaration']['bytes']==native[key]['array_declaration']['bytes']
        assert builds[0][key]['element_layout']==native[key]['element_layout']
        assert sha(row['before_definition'].encode())==row['before_sha256']
        assert replay(row['before_definition'],row['changes'])==row['after_definition'] and len(row['changes'])==2
        text=read(root/row['source']);a,b=span(text,row['name']);assert text[a:b]==row['after_definition'] and sha(text[a:b].encode())==row['after_sha256']
        ptr,initializer,guard,end=source_forms[row['name']];body=row['after_definition']
        assert ptr+' = '+initializer+';' in body and guard in body
        assert not re.search(r'\b'+ptr+r'\b',body.split(end,1)[1])
        assert '(&owner +' not in body and '(&iStack_4d0 +' not in body
        edits=[e for e in proof['source_edits'] if e.get('native_reverse_array_extension')==path
            and e['source']==row['source'] and e['before_sha256']==row['before_sha256']]
        assert len(edits)==1 and edits[0]['after_sha256']==row['after_sha256']
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+r['source'][:-2]+'.o' for r in rows}
    assert {p for p in before if before[p]!=after[p]}==expected
    return dict(recovered_card_definitions=2,recovered_private_helpers=1,fixed_array_locations=3,
        native_array_bytes=[160,176,1208],native_array_rows=[20,22,151],row_bytes=8,
        paired_consumer_calls=4,guarded_final_cursor_steps=3,changed_linker_objects=2,
        unchanged_linker_objects=len(before)-2,scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
