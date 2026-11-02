"""Check structured shared-tail dispatch against exact source and native enums."""
import json,re
from pathlib import Path
from canonicalize_helpers import ROOT,read,sha,span
from inspect_release import Image,value,inherited_value
from verify_inline_helpers import ranges
from verify_final_helper_locals import event_values
from spell_dispatch_source import project,trace_proof


def views(image,debug,rows):
    wanted={v['name'] for v in rows};out={};dw=debug.dwarf()
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name','').replace('\\','/')!='cards/throne_of_eldraine.c':continue
        for d in cu.get_top_DIE().iter_children():
            name=inherited_value(d,'DW_AT_name')
            if d.tag!='DW_TAG_subprogram' or name not in wanted or not ranges(dw,d):continue
            assert name not in out;parts=ranges(dw,d);assert len(parts)==1;a,b=parts[0]
            out[name]=dict(owner_die=hex(d.offset),address=hex(a),end=hex(b),bytes_sha256=sha(image.pe.get_data(a-image.pe.OPTIONAL_HEADER.ImageBase,b-a)))
        cu._dielist.clear();cu._diemap.clear()
    assert set(out)==wanted;return out


def verify(proof,image,root=ROOT):
    path=proof.get('native_spell_dispatch_cleanup')
    if not path:return {}
    r=json.loads((root/path).read_bytes());assert sha(image.data)==r['native_dll_sha256'];nd=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(nd.data)==r['native_dbg_sha256'];native=views(image,nd,r['rows']);builds=[]
    for label in ['before','after']:
        a=r['runtime_'+label];im,db=Image((root/a['dll_path']).read_bytes()),Image((root/a['dbg_path']).read_bytes());assert sha(im.data)==a['dll_sha256'] and sha(db.data)==a['dbg_sha256'];builds.append(views(im,db,r['rows']))
    ci,cd=Image((root/'ManalinkEh.dll').read_bytes()),Image((root/'ManalinkEh.dbg').read_bytes());current=views(ci,cd,r['rows']);cases=dead=0
    for row in r['rows']:
        name=row['name'];after,facts=project(row['before_definition']);assert facts==row['facts'] and after==row['after_definition'];assert sha(after.encode())==row['after_sha256'] and sha(row['before_definition'].encode())==row['before_sha256']
        text=read(root/row['source']);a,b=span(text,name);assert text[a:b]==after
        assert native[name]==row['native_view'] and builds[0][name]==row['before_view'] and builds[1][name]==row['after_view'];events=event_values(nd,native[name]);assert events==r['native_event_values'];compiled_events=event_values(cd,current[name]);used=set(re.findall(r'\bEVENT_\w+\b',facts['old_dispatch']+facts['new_dispatch']));assert all(events[n]==compiled_events[n] for n in used)
        trace=trace_proof(facts,events);assert trace==row['trace_proof'];cases+=len(trace['cases']);dead+=facts['removed_dead_result_local']
        edits=[e for e in proof['source_edits'] if e.get('spell_dispatch_cleanup_extension')==path and (e['source'],e.get('name'))==(row['source'],name)];assert len(edits)==1 and edits[0]['before_sha256']==row['before_sha256'] and edits[0]['after_sha256']==row['after_sha256']
    a,b=r['before_link_inputs'],r['after_link_inputs'];assert set(a)==set(b) and {p for p in a if a[p]!=b[p]}=={'.recovery/build-gcc93/obj/cards/throne_of_eldraine.o'};assert len(r['rows'])==7 and cases==252 and dead==6
    return dict(structured_card_handlers=7,removed_lab_gotos=14,removed_lab_labels=7,removed_dead_result_locals=6,parsed_dispatch_cases=252,native_event_values=8,source_units=1,changed_linker_objects=1,unchanged_linker_objects=len(a)-1,scope=r['scope'])


if __name__=='__main__':
    from canonicalize_helpers import load
    print(json.dumps(verify(load(),Image((ROOT/'.recovery/2022-ManalinkEh.dll').read_bytes())),indent=2))
