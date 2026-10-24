"""Recover the final five missing named card handlers with native state, storage and modal call frames."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value,inherited_value
from export_abi import type_of
from pack_native_tables import unqual
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read
from recover_combat_mask_cards import cleaned
from recover_literals import readonly_string,byte_literal,LITERAL
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input
from final_card_handler_bodies import BODIES
from recover_exile_return_cards import arguments

PHARAOH='20d20a0';TAMIYO='21d6820';DRAGON='21d25c0';KEFNET='21ce5c0';ARYEL='22cb2d0'
ADDRESSES=[PHARAOH,TAMIYO,DRAGON,KEFNET,ARYEL];PRIVATE=set();CARDS=set(ADDRESSES);SUPPORT=[]


def native_storage(runtime,debug,meta):
    arrays={};units={meta[a]['source'] for a in ADDRESSES};owners={meta[a]['name'] for a in ADDRESSES}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in units:continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_variable':continue
            parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            owner=value(parent,'DW_AT_name') if parent is not None else None
            if owner not in owners:continue
            t=unqual(type_of(d))
            if t.tag=='DW_TAG_array_type':arrays[owner+'::'+value(d,'DW_AT_name')]=dict(bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'],element_bytes=value(unqual(type_of(t)),'DW_AT_byte_size'),location=value(d,'DW_AT_location'))
    assert arrays['card_tamiyo_collector_of_tales::counts']['bounds']==[500] and arrays['card_tamiyo_collector_of_tales::counts']['element_bytes']==8
    assert arrays['card_nicol_bolas_dragon_god::activable']['bounds']==[13] and arrays['card_nicol_bolas_dragon_god::activable']['element_bytes']==4
    assert arrays['card_aryel_knight_of_windgrace::buffer']['bounds']==[74] and arrays['card_aryel_knight_of_windgrace::buffer']['element_bytes']==1
    for key in ['card_nicol_bolas_god_pharaoh::removed','card_aryel_knight_of_windgrace::to_tap']:assert arrays[key]['element_bytes']==4
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
       'pharaoh_vla_bytes':('20d24a4','c1e002'),
       'pharaoh_vla_probe':('20d24a7','e8249d6200'),
       'aryel_vla_bytes':('22cb665','c1e002'),
       'aryel_vla_probe':('22cb668','e8630b4300'),
       'aryel_vla_subtract':('22cb66d','29c4'),
       'tamiyo_table_word_count':('21d6acc','b9ea030000'),
       'tamiyo_table_fill':('21d6ad7','f3ab'),
       'tamiyo_second_filter_pointer':('21d6b70','8d854cf0ffff'),
       'tamiyo_first_filter_chooser_pointer':('21d6b9e','897c2404'),
       'aryel_format_count':('22cb9ec','8974240c'),
       'aryel_format_bound':('22cb9f8','c74424044b000000'),
       'aryel_token_keyword_word':('22cb924','c78524ffffff08000000'),
       'dragon_god_native_capacity':('21d2ca6','83ff0e7f7e'),
       'dragon_god_mode_byte':('21d2a27','885052'),
       'dragon_god_selected_iid_word':('21d31d3','89584c')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,(n,get(int(a,16),len(bytes.fromhex(b))).hex())
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;dialogs={};literals={};dynamic_dispatch={}
    expected={PHARAOH:[25],TAMIYO:[17],DRAGON:[26,23,53],KEFNET:[14],ARYEL:[15]}
    for a in ADDRESSES:
        code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(code,int(a,16)))
        sites=[i for i in insns if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==0x26e9c50]
        actual=arguments(BODIES[a],'dialog_impl');counts=[len(args) for args in actual];assert sorted(counts)==sorted(expected[a]) and len(sites)==len(actual),(a,counts)
        # Code layout places Dragon-God's resolution dialog before its nested chooser.
        source_counts=expected[a]
        dialogs[a]=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),words=n) for i,n in zip(sites,source_counts)]
        for args in actual:assert args[-4:]==['0']*4,(a,args[-4:])
        dynamic_dispatch[a]=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),operand=i.op_str) for i in insns if i.mnemonic=='call' and i.operands[0].type!=X86_OP_IMM]
        for i in insns:
            for op in i.operands:
                if op.type!=X86_OP_IMM:continue
                literal=readonly_string(runtime,op.imm)
                if literal is not None:literals[f'{op.imm:x}']=dict(bytes=(literal+b'\0').hex(),literal=byte_literal(literal))
    assert len(dynamic_dispatch[DRAGON])==5 and all(not dynamic_dispatch[a] for a in ADDRESSES if a!=DRAGON)
    return dict(arrays=arrays,filter_bytes=216,target_bytes=84,token_bytes=76,dialogs=dialogs,literals=literals,dynamic_dispatch=dynamic_dispatch,
       anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},
       retained_native_behavior=[
         'God-Pharaoh retains its unchecked activation target, count_deck-sized exile VLA, AP/NAP reveal and mode-9 exile permission.',
         'Tamiyo retains the 501-pair nonland frequency table, first-maximum choice and forward removal loop that advances even after compaction. Its human branch initializes test2 but passes test, not test2, to the deferred chooser.',
         'Dragon-God retains all five dynamic handler dispatches, the 14-IID table with native >14 capacity check, reverse name lookup including inactive zero entries, full 53-word nested chooser and unchecked selected index.',
         'Dragon-God first sets and clears player-bit 0x40000, then saves/restores complete words for the nested scan; the persisted mode is byte 0x52 and selected IID is word 0x4c.',
         'Kefnet retains separate first-draw replacement/drawn triggers, repeated instance reads, all artifact/Phyrexian/hybrid/Reaper-King/cost-reduction branches, its EVENT_RESOLVE_ACTIVATION dialog inside the draw trigger, and cleanup timing.',
         'Aryel retains target-count-sized four-byte tap storage, repeated availability checks, untargettable/tap/payment ordering, both cancellation cleanup phases, exact formatted target count and separate 75-byte buffer/76-byte token lifetimes.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore native typed fixed/VLA storage, complete modal frames, dynamic handler dispatch and event/payment/AI/cleanup behavior',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('final_card_handler_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    assert component['unit_layouts']==native_layouts(debug,{e['source'] for e in entries.values()})
    for old in component['retained_helpers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['final_card_handler_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-final-card-handlers' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    assert entries[TAMIYO]['integration_dependency']==component['integration_dependency']
    assert manifest['source_stubs']==component['retained_source_stubs']


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta);(cache/'export-request-final-card-handlers.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES+SUPPORT));print('Prepared the final five missing named card handlers.');return
    assert 'final_card_handlers_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta),unit_layouts=native_layouts(debug,{meta[a]['source'] for a in ADDRESSES}),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=f['name'],source=f['source'],body_sha256=sha(function_body(read(root/f['source']),f['name']).encode())) for f in m['accepted_functions'] if f['name'] in {'planeswalker','war_god_eternal','charge_mana_from_id_minus_two'}],
        retained_source_stubs=m['source_stubs'],
        integration_dependency='Tamiyo interactive named-card selection awaits the existing choose_a_card_test_ct stub, which returns -1. Native unconditional uses and distinct initialized/passed filters are retained; interactive gameplay is not verified.',
        scope='Recover the final five missing named card handlers: Nicol Bolas God-Pharaoh, Tamiyo Collector of Tales, Nicol Bolas Dragon-God, God-Eternal Kefnet and Aryel Knight of Windgrace. Restore fixed/VLA tables, full modal frames, callback dispatch, state bytes/words, cost/selection/payment/AI/order/cleanup behavior. Preserve explicit native unchecked bounds, inactive choices, mismatched filters and forward removal quirks; gameplay integration remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=meta[a]['name'],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=row['name'];raw=read(cache/'decompiled-final-card-handlers'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],final_card_handler_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    for e in entries:
        if e['address']==TAMIYO:e['integration_dependency']=proof['integration_dependency']
    compile_sources(root,cache/'final-card-handlers-check',sources)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-final-card-handlers/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-final-card-handlers'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=5;m['card_handler_functions']+=5
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['final_card_handlers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'final-card-handlers-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=5,new_helpers=0)))


if __name__=='__main__':main()
