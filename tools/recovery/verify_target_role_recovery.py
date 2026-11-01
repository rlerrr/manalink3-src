"""Verify native marker/prompt storage, record copies and typed target roles."""
import json
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image
from target_role_analysis import inspect
from verify_array_role_recovery import variable,footprint,replay


PROMPTS={'card_ovinomancer':('msg','_new_pick_target',1),'card_shivan_emissary':('msg','_new_pick_target',1),
         'card_spell_queller':('prompt','_dialog_impl',10)}
COPIES={'card_nesting_grounds':('td_from','td_to'),'card_flesh_blood':('td','td2')}
LOOPS={'card_blim_comedic_genius':('times_damaged',2),'card_verdurous_gearhulk':('marked',4),
       'card_verdant_confluence':('choices',3),'card_catch_release':('typs',5)}
SCALARS={'card_sorin_vengeful_bloodlord':'loyalty_to_remove','card_ruins_of_oran_rief':'mana_result',
         'card_lukka_coppercoat_outcast':'creature_index','card_verdant_confluence':'ret_location'}


def validate(snapshot,row,*,native=False):
    name=row['name'];result={}
    if name=='card_eye_of_doom':
        v=variable(snapshot,'marked')
        assert v['shape']==dict(kind='DW_TAG_array_type',size=1208,dimensions=[2,151],element_size=4,element_encoding=5)
        assert v['frame_offset']+1208<=-8
        result['marker_clear']=footprint(snapshot,v['frame_offset'],b'\0'*1208)
    if name in PROMPTS:
        var,consumer,index=PROMPTS[name];v=variable(snapshot,var)
        assert v['shape']==dict(kind='DW_TAG_array_type',size=100,dimensions=[100],element_size=1,element_encoding=6)
        assert v['frame_offset']+100<=-8
        ptr=['frame',v['frame_offset']+8]
        formatter=[c for c in snapshot['effects']['calls'] if c['name']=='_scnprintf' and c['arguments'][0]==ptr and c['arguments'][1]==['const',0xffffffff,100]]
        assert len(formatter)==1,(name,formatter)
        consumers=[c for c in snapshot['effects']['calls'] if c['name']==consumer and c['arguments'][index]==ptr]
        # Ovinomancer's loop loads its prompt via a nonescaping pointer spill;
        # the conservative call-memory kill intentionally leaves that unknown.
        if name!='card_ovinomancer':assert consumers,(name,consumer)
        result['prompt']=dict(bytes=100,formatter=formatter[0]['address'],definite_consumer_sites=[c['address'] for c in consumers],
            consumer_scope='Scnprintf destination/100-byte bound plus typed source operand; unknown Ovinomancer pointer-spill values are not asserted equal.' if name=='card_ovinomancer' else 'Exact consumer frame operand')
    if name in COPIES:
        src,dst=COPIES[name];copies=snapshot['effects']['frame_copies'];assert len(copies)==2
        for copy in copies:
            assert copy['size']==84
            assert any(v['name']==src and v['frame_offset']==copy['source_frame_offset'] and v['shape']==dict(kind='DW_TAG_structure_type',size=84) for v in snapshot['variables'])
            assert any(v['name']==dst and v['frame_offset']==copy['destination_frame_offset'] and v['shape']==dict(kind='DW_TAG_structure_type',size=84) for v in snapshot['variables'])
        result['target_copies']=dict(bytes_each=84,sites=[c['address'] for c in copies],source=src,destination=dst)
    if name in LOOPS:
        var,count=LOOPS[name];v=variable(snapshot,var)
        assert v['shape']['dimensions']==[count] and v['shape']['element_size']==4
        assert v['frame_offset']+count*4<=-8
        if native:
            assert any(t['name']=='td' and t['shape']==dict(kind='DW_TAG_structure_type',size=84) and t['frame_offset']==v['frame_offset']+4*count for t in snapshot['variables'])
        result['typed_array_endpoint']=dict(name=var,elements=count,bytes=count*4)
    if not native and name in SCALARS:
        found=[v for v in snapshot['scalar_types'] if v['name']==SCALARS[name]]
        assert len(found)==1 and found[0]['kind']=='DW_TAG_base_type' and found[0]['size']==4 and found[0]['encoding']==5,(name,found)
        result['integer_role']=SCALARS[name]
    return result


def verify_superseded_definition(proof,image,root,source,name,old_sha):
    path=proof.get('native_target_role_recovery');assert path
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    found=[r for r in record['rows'] if (r['source'],r['name'],r['before_sha256'])==(source,name,old_sha)]
    assert len(found)==1;row=found[0]
    assert sha(row['before_definition'].encode())==old_sha and replay(row['before_definition'],row['changes'])==row['after_definition']
    text=read(root/source);a,b=span(text,name)
    assert text[a:b]==row['after_definition'] and sha(text[a:b].encode())==row['after_sha256']
    receipts=[e for e in proof['source_edits'] if e.get('native_target_role_extension')==path and e['source']==source and e['before_sha256']==old_sha]
    assert len(receipts)==1 and receipts[0]['after_sha256']==row['after_sha256']
    view=row['native_snapshot']['view'];lo,hi=int(view['address'],16),int(view['end'],16)
    assert sha(image.pe.get_data(lo-image.pe.OPTIONAL_HEADER.ImageBase,hi-lo))==view['bytes_sha256']


def verify(proof,image,root=ROOT):
    path=proof.get('native_target_role_recovery')
    if not path:return {}
    record=json.loads((root/path).read_bytes());assert sha(image.data)==record['native_dll_sha256']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==record['native_dbg_sha256']
    rows=record['rows'];assert len(rows)==13
    native=inspect(image,debug,rows);builds=[]
    for label in ['before','after']:
        artifact=record['runtime_'+label];im=Image((root/artifact['dll_path']).read_bytes());dbg=Image((root/artifact['dbg_path']).read_bytes())
        assert sha(im.data)==artifact['dll_sha256'] and sha(dbg.data)==artifact['dbg_sha256']
        builds.append(inspect(im,dbg,rows))
    current=inspect(Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes()),rows)
    for row in rows:
        key=(row['source'],row['name']);assert native[key]==row['native_snapshot']
        assert builds[0][key]==row['before_compiled'] and builds[1][key]==row['after_compiled']
        assert validate(native[key],row,native=True)==row['native_validation']
        assert validate(builds[1][key],row)==row['compiled_validation'];validate(current[key],row)
        verify_superseded_definition(proof,image,root,*key,row['before_sha256'])
        assert '(target_definition_t *)' not in row['after_definition']
        endpoints={'card_blim_comedic_genius':['times_damaged + 2'],
            'card_verdurous_gearhulk':['marked + 4'],
            'card_verdant_confluence':['choices + 3'],
            'card_catch_release':['typs + 5']}
        for endpoint in endpoints.get(row['name'],[]):assert endpoint in row['after_definition']
    assert len(record['retained'])==1
    retained=record['retained'][0];assert retained['name']=='strive'
    text=read(root/retained['source']);a,b=span(text,'strive');assert text[a:b]==retained['before_definition']
    assert 'target_definition_t *td, const char *prompt' in text[a:b]
    before,after=record['before_link_inputs'],record['after_link_inputs'];assert set(before)==set(after)
    expected={'.recovery/build-gcc93/obj/'+r['source'][:-2]+'.o' for r in rows}
    assert {p for p in before if before[p]!=after[p]}==expected
    return dict(recovered_card_definitions=13,retained_typed_helpers=1,restored_marker_bytes=1208,
        restored_prompt_buffers=3,prompt_bytes_each=100,exact_target_record_copies=4,target_record_bytes=84,
        typed_array_capacities=[2,3,4,5],integer_roles_restored=4,changed_linker_objects=len(expected),
        unchanged_linker_objects=len(before)-len(expected),scope=record['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
