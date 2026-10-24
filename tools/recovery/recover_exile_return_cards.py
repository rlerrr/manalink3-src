"""Recover Sands, Chemister, Livio, Voidwalker and its private legacy constructor."""
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
from exile_return_card_bodies import BODIES

CREATE='222b230';SANDS='20d6320';CHEMISTER='228e160';LIVIO='25abb10';DAUTHI='2231790'
ADDRESSES=[CREATE,SANDS,CHEMISTER,LIVIO,DAUTHI];PRIVATE={CREATE};CARDS=set(ADDRESSES)-PRIVATE
LITERAL_ADDRESSES=[0x2729b76,0x2729cba,0x272a22f,0x272a21e,0x272a208,0x272a1f2,0x273a539,0x273a55c,0x273a59d,0x273a586,0x273a574,0x273a5b8,0x275676b,0x2756c15,0x2756a63,0x2756e9d,0x2756e85,0x2756765,0x2756768,0x2756e41,0x2756ec0,0x2756e2d,0x2736d2e,0x2736d34,0x273710f,0x2736d31,0x2737101,0x2737103,0x2737112]


def arguments(body,name):
    masked=re.sub(LITERAL,lambda m:' '*len(m[0]),body);calls=[]
    for match in re.finditer(r'\b'+name+r'\s*\(',masked):
        start=match.end();depth=0;args=[]
        for i in range(start,len(masked)):
            c=masked[i]
            if c in '([':depth+=1
            elif c in ')]':
                if c==')' and depth==0:args.append(body[start:i].strip());break
                depth-=1
            if c==',' and depth==0:args.append(body[start:i].strip());start=i+1
        calls.append(args)
    return calls


def native_storage(runtime,debug,meta):
    arrays={};counters={};units={meta[a]['source'] for a in ADDRESSES};owners={meta[a]['name'] for a in ADDRESSES}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in units:continue
        for d in cu.iter_DIEs():
            n=value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumerator' and n in {'COUNTER_VOID','COUNTER_AEGIS'}:counters[n]=value(d,'DW_AT_const_value')
            if d.tag!='DW_TAG_variable':continue
            parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            owner=value(parent,'DW_AT_name') if parent is not None else None
            if owner not in owners:continue
            t=unqual(type_of(d))
            if t.tag=='DW_TAG_array_type':arrays[owner+'::'+n]=dict(bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'],element_bytes=value(unqual(type_of(t)),'DW_AT_byte_size'),location=value(d,'DW_AT_location'))
    assert counters==dict(COUNTER_VOID=170,COUNTER_AEGIS=173)
    assert arrays['card_dauthi_voidwalker::exiled']['bounds']==[30,2]
    assert arrays['card_livio_oathsworn_sentinel::marked']['bounds']==[99,1]
    assert arrays['card_livio_oathsworn_sentinel::buffer']['bounds']==[99]
    assert arrays['card_izzet_chemister::buffer']['bounds']==[49] and arrays['card_izzet_chemister::ic']['element_bytes']==4
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;dialogs={};expected={SANDS:[21],CHEMISTER:[17],LIVIO:[16,21],DAUTHI:[59,59]}
    for a,counts in expected.items():
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(native,int(a,16)))
        sites=[i for i in insns if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==0x26e9c50]
        args=arguments(BODIES[a],'dialog_impl');assert len(sites)==len(args)==len(counts)
        dialogs[a]=[]
        for ins,actual,n in zip(sites,args,counts):
            assert len(actual)==n,(a,len(actual),n)
            assert actual[-4:]==['0']*4
            dialogs[a].append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),words=n,arguments=actual))
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    assert get(0x228e580,3).hex()=='c1e002' and get(0x228e589,5).hex()=='e842dc4600'
    assert get(0x228e61c,6).hex()=='8d851cffffff' and get(0x228e656,6).hex()=='8b8500feffff'
    assert get(0x228e6a8,12).hex()=='4039f08b1487895487fc75f4'
    assert get(0x22318fe,9).hex()=='83ff1e0f8f19020000' and get(0x22319b0,1)==b'\x47'
    literals={f'{a:x}':dict(bytes=(readonly_string(runtime,a)+b'\0').hex(),literal=byte_literal(readonly_string(runtime,a))) for a in LITERAL_ADDRESSES}
    return dict(arrays=arrays,counters=counters,dialogs=dialogs,literals=literals,
        chemister_vla=dict(shift_address='228e580',shift_bytes='c1e002',probe_address='228e589',probe_bytes='e842dc4600',element_bytes=4),
        chemister_distinct_filters=dict(initial_filter_ebp=-444,unused_initialized_filter_ebp=-228,initializer_pointer_address='228e61c',initializer_pointer_bytes='8d851cffffff',selector_pointer_address='228e656',selector_pointer_bytes='8b8500feffff'),
        packed_exile=dict(owner_bit=31,iid_mask=0x7fffffff,record_bytes=4,card_data_bytes=72,id_offset=36,name_offset=1),
        retained_native_behavior=['Chemister initializes its second filter but passes the first filter to selection.',
        'Chemister removes a selection by copying ic[k+1] through k<count, retaining the native final-slot read.',
        'Chemister checks the recorded owner but casts from 1-player, and skips detached cleanup when its recorded amount is not positive.',
        'Voidwalker initializes exactly 93 integer words, searches sentinel row zero, and can fill index 31 despite the 31-row declaration.',
        'Voidwalker scores inactive zero rows, preserves both 59-word dialog pages, and does not add a selected-row guard.',
        'Livio gathers at most its declared 100 pair slots without adding a new bounds check, in descending AP/NAP order; return processing reverses that order.',
        'Endless Sands retains the differing can-activate/actual payment costs and its native AI/regeneration and cancellation paths.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore typed packed exile words, native fixed/VLA arrays, separate filters, full modal frames, callback identity and native state/order/bounds behavior',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('exile_return_card_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    assert component['unit_layouts']==native_layouts(debug,{e['source'] for e in entries.values()})
    for old in component['retained_private_callbacks']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for record in component['native_storage']['literals'].values():assert record['literal'] in ''.join(BODIES.values()),record
    for a,e in entries.items():
        detail=component['functions'][a];assert e['exile_return_card_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-exile-return-cards' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    assert [p['location'][0]['expression'] for p in component['parameters'][CREATE]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    assert len({(p['location'][0]['start'],p['location'][0]['end']) for p in component['parameters'][CREATE]})==1


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta);(cache/'export-request-exile-return-cards.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared four exile/return handlers and one private constructor.');return
    assert 'exile_return_cards_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta),unit_layouts=native_layouts(debug,{meta[a]['source'] for a in ADDRESSES}),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_private_callbacks=[dict(name=n,source=s,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in [('cards/modern_horizons_2.c','card_exiled_with_void_counter'),('cards/commander_legends.c','card_exiled_with_aegis_counter')]],
        scope='Recover Endless Sands, Izzet Chemister, Livio and Dauthi Voidwalker with the private register-argument Void-counter legacy constructor in their original source units. Restore packed owner/IID words, native fixed and variable arrays, two distinct Chemister filters, all six complete dialog frames, attachment callback identities, display/counter metadata and exact native state/iteration/payment/cleanup rules. Preserve and document native unchecked bounds and other quirks; gameplay integration remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=meta[a]['name'],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=row['name'];raw=read(cache/'decompiled-exile-return-cards'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],exile_return_card_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    compile_sources(root,cache/'exile-return-cards-check',sources)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-exile-return-cards/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-exile-return-cards'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=5;m['card_handler_functions']+=4;m['helper_functions']+=1;m['private_functions']+=1;m['private_helpers']+=1
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['exile_return_cards_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'exile-return-cards-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=4,new_helpers=1)))


if __name__=='__main__':main()
