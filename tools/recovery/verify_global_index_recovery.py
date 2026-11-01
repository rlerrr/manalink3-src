"""Verify typed player_bits/stack_data indexing against native address events."""
import collections,json,re
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,inherited_value
from global_index_analysis import inspect,global_types,flag_effects,INLINE_HELPER,GLOBALS
from verify_array_role_recovery import replay
from verify_inline_helpers import ranges


def event_counts(snapshot,before=False):
    result=collections.Counter()
    for a in snapshot['global_addresses']:
        factor=(8 if a['global_name']=='player_bits' else 128) if before else 1
        assert a['scale']%factor==a['bias_bytes']%factor==0
        for bit in [1,2]:
            if a['access']&bit:result[(a['global_name'],a['scale']//factor,a['bias_bytes']//factor,a['width'],bit)]+=1
    return result


def validate(snapshot,row,before=False):
    names={c['global_name'] for c in row['changes']};assert len(names)==1
    facts=snapshot['global_addresses'];assert facts and {f['global_name'] for f in facts}==names
    for f in facts:
        expected=GLOBALS[f['global_name']][2] if before else 4
        assert abs(f['scale'])==expected,(row['name'],before,f)
        assert f['width']==(4 if f['global_name']=='player_bits' else 2)
        if f['global_name']=='stack_data':assert f['instruction'][0]=='movsx'
    counts=event_counts(snapshot,before)
    return dict(global_name=next(iter(names)),effective_scale=next(iter({abs(f['scale']) for f in facts})),
        address_events=[dict(global_name=k[0],scale=k[1],bias=k[2],width=k[3],access=k[4],count=v)
            for k,v in sorted(counts.items())],machine_sites=[f['address'] for f in facts])


def comparison(row,native,compiled,native_image,compiled_image):
    if event_counts(native)==event_counts(compiled):return dict(kind='exact_address_event_multisets')
    assert row['name']=='card_worms_of_the_earth'
    a,b=flag_effects(native_image,native),flag_effects(compiled_image,compiled)
    assert a['effect_classes']==b['effect_classes']
    assert len(a['effects'])==4 and len(b['effects'])==6
    assert not a['mutually_unreachable_identical_store_pairs'] and len(b['mutually_unreachable_identical_store_pairs'])==2
    return dict(kind='exclusive_identical_flag_effects',native=a,compiled=b,
        scope='Identical4-byte address classes and OR8/AND-not8 effects. Both extra compiled OR copies are mutually unreachable and no effect site repeats on a direct CFG path. Source replay preserves predicates; arbitrary aliasing and full function equivalence are not asserted.')


def has_source_repair(proof,root,source,name,old_sha):
    path=proof.get('native_global_index_recovery')
    if not path:return False
    record=json.loads((root/path).read_bytes())
    return any((r['source'],r['name'],r['before_sha256'])==(source,name,old_sha) for r in record['rows'])


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    path=proof.get('native_global_index_recovery');assert path
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    matches=[r for r in record['rows'] if (r['source'],r['name'],r['before_sha256'])==(source,name,old_sha)]
    assert len(matches)==1;row=matches[0]
    assert sha(row['before_definition'].encode())==old_sha
    assert replay(row['before_definition'],row['changes'])==row['after_definition']
    text=read(root/source);a,b=span(text,name)
    assert text[a:b]==row['after_definition'] and sha(text[a:b].encode())==row['after_sha256']
    receipts=[e for e in proof['source_edits'] if e.get('native_global_index_extension')==path
        and e['source']==source and e['before_sha256']==old_sha]
    assert len(receipts)==1 and receipts[0]['after_sha256']==row['after_sha256']
    owner=next(r for r in record['rows'] if r['name']==('card_nimble_obstructionist' if name==INLINE_HELPER else name))
    view=owner['native_snapshot']['view'];lo,hi=int(view['address'],16),int(view['end'],16)
    assert sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo))==view['bytes_sha256']


def verify(proof,image,root=ROOT):
    path=proof.get('native_global_index_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    rows=record['rows'];assert len(rows)==20 and len({r['source'] for r in rows})==17
    assert sum(c['occurrences'] for r in rows for c in r['changes'])==53
    native=inspect(image,debug,rows);builds=[];images=[]
    for label in ['before','after']:
        artifact=record['runtime_'+label];im=Image((root/artifact['dll_path']).read_bytes());dbg=Image((root/artifact['dbg_path']).read_bytes())
        assert sha(im.data)==artifact['dll_sha256'] and sha(dbg.data)==artifact['dbg_sha256']
        assert global_types(dbg)==record['global_types']
        builds.append(inspect(im,dbg,rows));images.append(im)
    current_debug=Image((root/'ManalinkEh.dbg').read_bytes())
    current_image=Image((root/'ManalinkEh.dll').read_bytes());current=inspect(current_image,current_debug,rows);kinds=collections.Counter()
    expected=dict(player_bits=dict(count=2,element_bytes=4,bytes=8,address='0x4ef1a4'),
        stack_data=dict(count=32,element_bytes=4,bytes=128,internal_card_id=dict(offset=0,bytes=2,encoding=5),address='0x509630'))
    assert global_types(debug)==global_types(current_debug)==record['global_types']==expected
    for row in rows:
        verify_superseded_definition(proof,image,root,row['source'],row['name'],row['before_sha256'])
        for change in row['changes']:
            name=change['global_name']
            pattern=r'\*\(unsigned int \*\)\(&player_bits \+ (.*?) \* 4\)' if name=='player_bits' else r'\*\(short \*\)\(&stack_data \+ (.*?) \* 4\)'
            match=re.fullmatch(pattern,change['before']);assert match
            replacement='player_bits['+match[1]+']' if name=='player_bits' else 'stack_data['+match[1]+'].internal_card_id'
            assert change['after']==replacement
        if row['name']==INLINE_HELPER:continue
        key=(row['source'],row['name']);assert native[key]==row['native_snapshot']
        assert builds[0][key]==row['before_compiled'] and builds[1][key]==row['after_compiled']
        assert validate(native[key],row)==row['native_validation']
        assert validate(builds[0][key],row,before=True)==row['before_validation']
        assert validate(builds[1][key],row)==row['compiled_validation'];validate(current[key],row)
        assert event_counts(native[key])==event_counts(builds[0][key],True)
        checked=comparison(row,native[key],builds[1][key],image,images[1]);assert checked==row['compiled_comparison'];kinds[checked['kind']]+=1
        comparison(row,native[key],current[key],image,current_image)
    inline=json.loads((root/proof['native_inline_final_definitions_recovery']).read_bytes())
    helper=next(h for h in inline['helpers'] if h['name']==INLINE_HELPER)
    row=next(r for r in rows if r['name']==INLINE_HELPER)
    assert helper['definition']==row['before_definition'] and helper['definition_sha256']==row['before_sha256']
    assert helper['native_origins']==record['inline_origins']
    dw=debug.dwarf()
    for origin in record['inline_origins']:
        d=dw.get_DIE_from_refaddr(int(origin['die_offset'],16));assert inherited_value(d,'DW_AT_name')==INLINE_HELPER
        for witness in origin['instances']:
            assert witness['owner']=='card_nimble_obstructionist'
            child=dw.get_DIE_from_refaddr(int(witness['die_offset'],16));assert child.get_DIE_from_attribute('DW_AT_abstract_origin').offset==d.offset
            assert ranges(dw,child)==[(int(p['address'],16),int(p['end'],16)) for p in witness['ranges']]
            for part in witness['ranges']:
                lo,hi=int(part['address'],16),int(part['end'],16)
                assert sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo))==part['sha256']
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+r['source'][:-2]+'.o' for r in rows}
    assert {p for p in before if before[p]!=after[p]}==expected
    assert dict(kinds)==dict(exact_address_event_multisets=18,exclusive_identical_flag_effects=1)
    return dict(recovered_card_definitions=13,recovered_helper_definitions=7,source_units=17,
        corrected_array_expressions=53,active_compiled_functions=19,inline_only_source_helpers=1,
        player_bits_old_stride=32,stack_data_old_stride=512,native_and_rebuilt_stride=4,
        exact_address_event_comparisons=18,exclusive_flag_clone_comparisons=1,
        changed_linker_objects=len(expected),unchanged_linker_objects=len(before)-len(expected),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
