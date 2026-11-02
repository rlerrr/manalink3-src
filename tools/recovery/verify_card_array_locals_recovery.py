"""Verify restored native arrays/scoped records in ten original card units."""
import collections,json
from pathlib import Path
from canonicalize_helpers import ROOT,read,span,sha
from verify_additional_local_names import source_hash_matches
from inspect_release import Image
from card_array_locals_analysis import inspect,source_changes,NAMES,TEST_NAMES,TOKEN_NAMES
from verify_array_role_recovery import replay,variable
from verify_scoped_array_recovery import initial_bytes,disjoint_reused_storage

ARRAYS={'card_mana_cache':('found',[2],4,5),'card_gusthas_scepter':('buffer',[100],1,6),
    'card_slaughter_the_strong':('msg',[100],1,6),'card_vraska_relic_seeker':('buffer',[50],1,6),
    'card_fast_furious':('totals',[2],4,5),'card_kayas_guile':('priority',[6],4,5),
    'card_merrow_bonegnawer':('msg',[100],1,6),'card_reign_of_terror':('buffer',[100],1,6),
    'card_clone_shell':('msg',[100],1,6),'card_lord_windgrace':('dead',[10],4,5)}
TEST_COUNTS={'card_mana_cache':1,'card_gusthas_scepter':1,'card_slaughter_the_strong':1,'card_vraska_relic_seeker':0,
    'card_fast_furious':1,'card_kayas_guile':1,'card_merrow_bonegnawer':2,'card_reign_of_terror':1,'card_clone_shell':1,'card_lord_windgrace':2}


def validate(snapshot,row):
    name=row['name'];role,dims,width,encoding=ARRAYS[name];size=width
    for d in dims:size*=d
    decl=[v for v in snapshot['array_declarations'] if v['name']==role]
    assert len(decl)==1 and decl[0]['shape']==dict(kind='DW_TAG_array_type',size=size,dimensions=dims,element_size=width,element_encoding=encoding)
    located=[v for v in snapshot['variables'] if v['name']==role]
    if located:
        assert len(located)==1 and located[0]['frame_offset']+size<=-8
    else:
        # Kaya's copied priority/scalar values make this source array dead
        # after reconstruction. Do not invent volatile memory to force it.
        assert name=='card_kayas_guile' and decl[0]['frame_offset'] is None
    disjoint_reused_storage(snapshot)
    tests=[v for v in snapshot['typed_records'] if v['bytes']==216];assert len(tests)==TEST_COUNTS[name]
    test_facts=[]
    for t in tests:
        assert t['frame_offset']+216<=-8
        test_facts.append(dict(name=t['name'],initializers=sorted(t['initializers']),layout=t['layout'],parameters=t['normalized_parameters']))
    tokens=[v for v in snapshot['typed_records'] if v['bytes']==76];expected=2 if name=='card_vraska_relic_seeker' else 1 if name in ['card_kayas_guile','card_lord_windgrace'] else 0
    assert len(tokens)==expected;token_facts=[]
    for t in tokens:
        assert t['initializer_csvid'] and t['initializer_csvid'][0]=='const' and t['frame_offset']+76<=-8
        events=collections.Counter()
        for w in t['constant_writes']:
            for j,b in enumerate(bytes.fromhex(w['bytes'])):events[w['offset']+j,b]+=1
        calls=[c for c in snapshot['storage_calls'] if c['name']=='_generate_token'
            and any(lo<=int(c['address'],16)<hi for lo,hi in t['scope_ranges'])]
        assert len(calls)==1
        assert calls[0]['arguments'][0]==['frame',t['frame_offset']+8] or name=='card_lord_windgrace' and calls[0]['arguments'][0] is None
        token_facts.append(dict(csvid=t['initializer_csvid'][2],constant_bytes=[[o,b,n] for (o,b),n in sorted(events.items())]))
    result=dict(array=role,dimensions=dims,bytes=size,tests=sorted(test_facts,key=lambda t:t['name']),tokens=sorted(token_facts,key=lambda t:t['csvid']))
    if name in ['card_merrow_bonegnawer','card_clone_shell']:
        msg=located[0];literal=b'Select a card to exile.\0' if name=='card_merrow_bonegnawer' else b'Select a card.\0'
        initial_bytes(snapshot,msg,literal+b'\0'*(100-len(literal)))
        calls=[c for c in snapshot['storage_calls'] if c['name']=='_new_default_test_definition' and c['arguments'][2]==['frame',msg['frame_offset']+8]]
        assert len(calls)==(1 if name=='card_merrow_bonegnawer' else 3)
        result['initialized_message']=dict(bytes=100,prefix=literal.hex(),consumers=len(calls))
    if name in ['card_gusthas_scepter','card_slaughter_the_strong','card_vraska_relic_seeker','card_reign_of_terror']:
        frame=located[0]['frame_offset'];calls=[c for c in snapshot['storage_calls'] if c['name']=='_scnprintf']
        assert len(calls)==(2 if name=='card_reign_of_terror' else 1)
        base=[c for c in calls if c['arguments'][0]==['frame',frame+8] and c['arguments'][1]==['const',0xffffffff,size]];assert len(base)==1
        target,arg=('_new_default_test_definition',2) if name=='card_gusthas_scepter' else ('_do_dialog',5) if name=='card_reign_of_terror' else ('_new_pick_target',1)
        consumers=[c for c in snapshot['storage_calls'] if c['name']==target and
            (c['arguments'][arg]==['frame',frame+8] or name=='card_reign_of_terror' and c['arguments'][arg] is None)];assert len(consumers)==1
        assert consumers[0]['arguments'][arg]==['frame',frame+8] or name=='card_reign_of_terror' and consumers[0]['arguments'][arg] is None
        result['prompt']=dict(bytes=size,formatter_sites=len(calls),fixed_base_formatters=1,consumer_sites=1,
            pointer_evidence='Typed source operand; rebuilt pointer-spill value may be unknown.' if name=='card_reign_of_terror' else 'Definite native/rebuilt frame operand.')
    if name=='card_mana_cache':initial_bytes(snapshot,located[0],b'\xff'*8)
    if name=='card_fast_furious':initial_bytes(snapshot,located[0],b'\0'*8)
    if name=='card_slaughter_the_strong':
        assert variable(snapshot,'marked')['shape']==dict(kind='DW_TAG_array_type',size=302,dimensions=[2,151],element_size=1,element_encoding=6)
    if name=='card_gusthas_scepter':
        for callee,first in [('_exiledby_find_any',2),('_exiledby_remember',4)]:
            calls=[c for c in snapshot['storage_calls'] if c['name']==callee];assert len(calls)==1
            for j,local in enumerate(['leg','idx']):
                assert any(v['name']==local and v['size']==4 and v['encoding']==5 and calls[0]['arguments'][first+j]==['frame',v['frame_offset']+8] for v in snapshot['scalar_frame_locals'])
        result['integer_cursor_api_pairs']=2
    if name=='card_clone_shell':
        ids=[v for v in snapshot['variables'] if v['name']=='stored_iid'];assert len(ids)==2
        for v in ids:
            assert v['shape']==dict(kind='DW_TAG_array_type',size=4,dimensions=[1],element_size=4,element_encoding=5)
            calls=[c for c in snapshot['storage_calls'] if c['name']=='_show_deck' and c['arguments'][1]==['frame',v['frame_offset']+8] and any(lo<=int(c['address'],16)<hi for lo,hi in v['scope_ranges'])]
            assert len(calls)==1 and calls[0]['arguments'][2]==['const',0xffffffff,1]
        result['single_iid_arrays']=2
    return result


def has_source_repair(proof,root,source,name,old_sha):
    path=proof.get('native_card_array_locals_recovery')
    if not path:return False
    record=json.loads((root/path).read_bytes());rows=[r for r in record['rows'] if (r['source'],r['name'],r['before_sha256'])==(source,name,old_sha)]
    if not rows:return False
    assert len(rows)==1;row=rows[0];changes,after=source_changes(name,row['before_definition'])
    assert sha(row['before_definition'].encode())==old_sha and changes==row['changes'] and after==row['after_definition']
    text=read(root/source);a,b=span(text,name);assert source_hash_matches(proof,root,source,name,sha(text[a:b].encode()),row['after_sha256']) and sha(after.encode())==row['after_sha256']
    edits=[e for e in proof['source_edits'] if e.get('native_card_array_locals_extension')==path and (e['source'],e['before_sha256'])==(source,old_sha)]
    assert len(edits)==1 and edits[0]['after_sha256']==row['after_sha256']
    return True


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    assert has_source_repair(proof,root,source,name,old_sha)
    record=json.loads((root/proof['native_card_array_locals_recovery']).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    row=next(r for r in record['rows'] if r['source']==source and r['name']==name);v=row['native_snapshot']['view'];lo,hi=int(v['address'],16),int(v['end'],16)
    assert sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo))==v['bytes_sha256']


def verify(proof,image,root=ROOT):
    path=proof.get('native_card_array_locals_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    rows=record['rows'];assert len(rows)==10 and {r['name'] for r in rows}==NAMES
    native=inspect(image,debug,rows);built=[]
    for label in ['before','after']:
        a=record['runtime_'+label];im=Image((root/a['dll_path']).read_bytes());dbg=Image((root/a['dbg_path']).read_bytes())
        assert sha(im.data)==a['dll_sha256'] and sha(dbg.data)==a['dbg_sha256'];built.append(inspect(im,dbg,rows))
    current=inspect(Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()),rows)
    for row in rows:
        key=row['source'],row['name'];assert native[key]==row['native_snapshot']
        assert built[0][key]==row['before_compiled'] and built[1][key]==row['after_compiled']
        left=validate(native[key],row);assert left==row['native_validation']==row['compiled_validation']
        assert validate(built[1][key],row)==validate(current[key],row)==left
        changes,after=source_changes(row['name'],row['before_definition']);assert changes==row['changes'] and after==row['after_definition']==replay(row['before_definition'],changes)
        assert sha(row['before_definition'].encode())==row['before_sha256']
        text=read(root/row['source']);a,b=span(text,row['name']);assert source_hash_matches(proof,root,row['source'],row['name'],sha(text[a:b].encode()),row['after_sha256']) and sha(after.encode())==row['after_sha256']
        assert has_source_repair(proof,root,*key,row['before_sha256'])
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+r['source'][:-2]+'.o' for r in rows};assert {p for p in before if before[p]!=after[p]}==expected
    fix=record['effect_model_fix'];old=(root/fix['before_path']).read_bytes();new=(root/fix['path']).read_bytes()
    assert sha(old)==fix['before_sha256'] and sha(new)==fix['after_sha256']
    assert old.replace(b"i.mnemonic=='rep stosd' and number(initial",b"i.mnemonic=='rep stosd' and n is not None and number(initial")==new
    optimized=[r['name'] for r in rows if not any(v['name']==ARRAYS[r['name']][0] for v in r['after_compiled']['variables'])]
    return dict(restored_card_definitions=10,restored_primary_arrays=10,restored_scoped_tests=11,restored_scoped_tokens=4,
        initialized_message_arrays=2,initialized_message_bytes=200,restored_single_iid_arrays=2,restored_integer_cursor_api_pairs=2,
        removed_source_unions=9,optimized_source_arrays=optimized,changed_linker_objects=10,unchanged_linker_objects=len(before)-10,
        restored_kaya_token_field_writes=2,
        scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
