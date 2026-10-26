"""Recover mana drain and AI decision/choice bookkeeping with native quirks."""
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
from recover_literals import readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input
from mana_ai_phase_bodies import BODIES

BURN='2671530';NONDRAIN='266bfd0';MINIMIZE='266bf60';WORKER='26b1240';REMEMBER='26b2190';RAND='26b21c0';TIMER='26b1280';DECISION='26b2240'
ADDRESSES=[BURN,NONDRAIN,MINIMIZE,WORKER,REMEMBER,RAND,TIMER,DECISION];PRIVATE={WORKER};PUBLIC_HELPERS=[a for a in ADDRESSES if a not in PRIVATE]
NAMES={BURN:'mana_burn_2022',NONDRAIN:'mana_doesnt_drain_until_eot_2022',MINIMIZE:'minimize_nondraining_mana_2022',WORKER:'remember_ai_value_impl_2022',REMEMBER:'remember_ai_value_2022',RAND:'recorded_rand_2022',TIMER:'check_timer_for_ai_speculation_2022',DECISION:'ai_decision_phase_2022'}
LABEL='/* October 2022 mana draining and AI phase/choice bookkeeping. */'
EXE_TARGETS=[0x434040,0x470250,0x4359b0,0x4a7d80,0x477070,0x477a90,0x499160,0x4990b0,0x439c90,0x439a80,0x44e050,0x472c90,0x498f20,0x499050]


def native_storage(runtime,debug,meta,root):
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
      'burn_capacity_lower_upper':('267155f','483d94000000'),
      'burn_waste_event':('2671594','c74424088f000000'),
      'burn_clear_flags0':('26715c7','83e13f'),
      'burn_clear_flags1':('26715df','83e23f'),
      'burn_drain_event':('26715ed','c744240800480000'),
      'burn_player_row32':('2671626','83c620'),
      'burn_nondrain_mask':('267166c','83e13f'),
      'burn_to_colorless_flag':('2671697','83e240'),
      'burn_keep_flag_signed_byte':('26716c9','84d2'),
      'nondrain_lowbits':('266c002','83e63f'),
      'nondrain_max63':('266c005','83fa3f'),
      'nondrain_keep_highbits':('266c010','83e0c0'),
      'ai_upkeep_capture':('26b2249','8b35a05a7702'),
      'ai_upkeep_reset':('26b2256','c705a05a770200000000'),
      'ai_one_row_stride':('26b229d','c1e005'),
      'ai_one_row_save':('26b22ae','891407'),
      'ai_one_row_replace':('26b22b7','8990c0f34e00'),
      'ai_play_query_event':('26b22c4','c70424c7000000'),
      'ai_counter_event':('26b22f6','c744240c38000000'),
      'ai_restore_row0':('26b2333','891485c0f34e00'),
      'ai_restore_row1':('26b234c','891485e0f34e00'),
      'ai_v16_zero':('26b238d','31ff'),
      'ai_timer_signed_half':('26b23f5','c1e81f01c8d1f8'),
      'ai_survival_threshold_lowbyte':('26b2421','80e26a'),
      'ai_timer_upper_threshold':('26b2424','81c2c8000000'),
      'ai_upkeep_return_code':('26b268d','c70004000000'),
      'random_before_record':('26b21ce','e87dbed9fd'),
      'random_clear_record':('26b21d3','c705e42f7a0000000000'),
      'random_private_tail':('26b21ec','e94ff0ffff'),
      'timer_default':('26b1285','bb1d150000'),
      'timer_setting13':('26b128a','c704240d000000'),
      'timer_signed_division':('26b12b8','f7fb'),
      'remember_decode99':('26b125a','83f862'),
      'remember_encode99':('26b126a','83f862')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,(n,get(int(a,16),len(bytes.fromhex(b))).hex())
    arrays={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/ai.c':continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_variable':continue
            n=inherited_value(d,'DW_AT_name');p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            if p is None or value(p,'DW_AT_low_pc')!=int(DECISION,16) or n not in {'store_raw_mana_available','str'}:continue
            t=unqual(type_of(d));bounds=[]
            while t.tag=='DW_TAG_array_type':
                bounds.extend(value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type');t=unqual(type_of(t))
            arrays[n]=dict(bounds=bounds,element_bytes=value(t,'DW_AT_byte_size'),location=value(d,'DW_AT_location'))
    assert {n:(x['bounds'],x['element_bytes']) for n,x in arrays.items()}==dict(store_raw_mana_available=([1,7],4),str=([99],1))
    assert arrays['str']['location']==arrays['store_raw_mana_available']['location']
    symbols={}
    for a in ADDRESSES:
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert len(ss)==1 and ss[0]['storage']==(3 if a in PRIVATE else 2);symbols[a]=ss[0]
    text=readonly_string(runtime,0x2764b08);assert text==b'%d: Entering AI Decision Phase.\n'
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,arrays=arrays,worker_physical_parameters=['eax:value'],worker_player_abstract_only=True,
        mana_views=dict(nondraining=dict(address='4ef200',players=2,row_bytes=7,element_bytes=1,amount_mask=63,convert_flag=64,keep_flag=128),pool=dict(address='4ef4e0',players=2,row_bytes=32,element_bytes=4),raw_available=dict(address='4ef3c0',players=2,row_bytes=32,element_bytes=4)),
        partial_snapshot=dict(saved_player='1-player',saved_words=8,restored_players=[0,1],restored_words=16,other_row_uninitialized=True,trace_bytes=100,snapshot_bytes=64),
        exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},trace_literal=dict(address='2764b08',bytes=(text+b'\0').hex(),literal=byte_literal(text)),anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},
        behavior=['Mana draining retains the AI waste loop and activation capacity range, byte amount/convert/keep flags, reverse color traversal, nondraining caps, colorless conversion, total recomputation and exact setting/speculation/sound/dialog/life behavior. The burn EXE dialog is a typed boundary, never executed.',
        'AI decision saves only the opposing eight-word mana row but restores both rows. Preserve this native partial initialization instead of filling the other row. The 64-byte snapshot and 100-byte trace buffer have separate source lifetimes at one native stack location; no stack-byte equivalence claim is made.',
        'Retain upkeep flag capture/reset, should-play/can-counter dispatch, damage/regeneration, qualified recalculation, opinion/path capture, signed timer limits, survival thresholds 200/50, phase routing for reasons 1..8/17, cleanup stops, mana drain, death check and end-turn sound.',
        'The choice worker takes only value in EAX; player exists only as an optimized abstract parameter. Public wrappers clear the record and gate on AI/non-trace, and recorded_rand generates its random value before clearing/gating. Encode and decode values >=99 around native path-record/replay calls. Timer uses positive configured milliseconds or 5405, current-thread user time, factor 100 and signed division.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore mana drain flags/settlement, native partial AI snapshot, phase routing and physical choice-worker interface',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('mana_ai_phase_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_helpers']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta,root)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['mana_ai_phase_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-mana-ai-phase' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    for old in component['retained_original_handlers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for a in PUBLIC_HELPERS:assert entries[a]['prototype'] in read(root/'manalink.h') or component.get('planning')
    ps=component['parameters'][WORKER]
    assert [p['name'] for p in ps]==['value','player'] and ps[0]['location'][0]['expression']=='(DW_OP_reg0 (r0))'
    assert all('DW_OP_GNU_parameter_ref' in x['expression'] for x in ps[1]['location'])



def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta,root);(cache/'export-request-mana-ai-phase.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared eight mana/AI helpers with one physical-value private worker.');return
    assert 'mana_ai_phase_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[(meta[a]['source'],meta[a]['name']) for a in ADDRESSES if a!=WORKER]
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta,root),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=n,source=source,body_sha256=sha(function_body(read(root/source),n).encode())) for source,n in retained],retained_original_handlers=[],
        scope='Recover eight mana-drain and AI phase/choice helpers in the original units, with seven qualified public APIs and one private physical-value worker. Preserve native byte drain flags, settlement and settings, partial AI snapshot initialization and all phase/opinion/timer/cleanup behavior. Retain all old bodies and earlier records; no zero-fill fix, stack-byte equivalence or gameplay integration is claimed.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-mana-ai-phase'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],mana_ai_phase_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(next(e['prototype'] for e in entries if e['address']==a) for a in PUBLIC_HELPERS)+'\n#endif\n\n'+header[at:]
    # Match check_source.py's existing MSVC-host compatibility alias, only in
    # the disposable syntax-check copy. GCC's original source stays unchanged.
    syntax_sources={s:('#define strncasecmp _strnicmp\n'+t if s=='functions/rules_engine.c' else t) for s,t in sources.items()}
    compile_sources(root,cache/'mana-ai-phase-check',syntax_sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-mana-ai-phase/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-mana-ai-phase'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=8;m['helper_functions']+=8;m['shared_helper_interfaces']+=7;m['private_functions']+=1;m['private_helpers']+=1
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['mana_ai_phase_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'mana-ai-phase-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=8)))


if __name__=='__main__':main()
