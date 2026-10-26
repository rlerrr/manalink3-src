"""Recover turn phases, attack eligibility, display/death checks and untap phasing."""
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
from turn_flow_bodies import BODIES

ATTACK='26ab730';DISPLAY='26b0680';WINNER='26b07b0';DEAD='26b0830';UNTAP='26b08d0';UPKEEP='26b0ca0';COLOR='26b0f60';PHASING='2377ac0'
ADDRESSES=[ATTACK,DISPLAY,WINNER,DEAD,UNTAP,UPKEEP,COLOR,PHASING];PRIVATE=set();PUBLIC_HELPERS=ADDRESSES
NAMES={ATTACK:'can_attack_2022',DISPLAY:'card_instances_should_be_displayed_identically_2022',WINNER:'determine_start_duel_winner_2022',DEAD:'is_anyone_dead_2022',UNTAP:'untap_phase_2022',UPKEEP:'upkeep_phase_2022',COLOR:'sub_495BE0_2022',PHASING:'untap_phasing_2022'}
LABEL='/* October 2022 turn flow, combat eligibility, display comparison and untap phasing. */'
EXE_TARGETS=[0x435c80,0x435cd0,0x4359b0,0x472260,0x4a7d80,0x43a700,0x439c90,0x43a060,0x435140,0x498ed0,0x436a20,0x477070]


def native_storage(runtime,debug,meta,root):
    import struct
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
      'attack_state_mask':('26ab78f','a910008200'),
      'attack_attacker_repeated_lookup':('26ab892','c7042401000000'),
      'attack_attacker_repeated_call':('26ab8c4','c744240401000000'),
      'attack_global_event':('26ab962','c744240879000000'),
      'display_state_mask':('26b06a6','81e31010b210'),
      'display_token_mask':('26b0729','81e300000200'),
      'death_poison0_byte':('26b07cb','803d30f54e0009'),
      'death_poison1_byte':('26b07e5','803d34f54e0009'),
      'untap_skip_callback':('26b09ce','81783800926702'),
      'untap_record_skip_after_kill':('26b0a02','c7858cfdffff01000000'),
      'untap_shimmer_id':('26b098a','6681f9310a'),
      'untap_sickness_clear':('26b0a53','81e1fffffeff'),
      'untap_curse_id':('26b0a68','6681f9440a'),
      'untap_cloak_id':('26b0a6f','6681f98309'),
      'untap_phasing_third_arg':('26b0af8','89442408'),
      'untap_phasing_call':('26b0b0c','e8af6fccff'),
      'untap_151_words':('26b0b1d','b997000000'),
      'untap_zero_array':('26b0b2e','f3ab'),
      'untap_status_byte_clear':('26b0b76','c6801c01000000'),
      'untap_trace_preincrement':('26b0c2b','40'),
      'upkeep_speculation_budget':('26b0d32','c74424040a000000'),
      'upkeep_speculation_reason':('26b0d3a','c7042411000000'),
      'upkeep_current_phase2':('26b0d7f','c70584f14e0002000000'),
      'upkeep_current_phase4':('26b0dcd','c70584f14e0004000000'),
      'upkeep_response_event':('26b0dd7','c744240c04000000'),
      'color_deck_mask':('26b106c','f7c200800800'),
      'color_deck_iid_mask':('26b1074','81e2ff7f0000'),
      'color_deck_500':('26b10b9','3df4010000'),
      'phasing_state_mask':('2377b06','2522008000'),
      'phasing_curse_argument':('2377b24','8b4510'),
      'phasing_event':('2377b51','c744240800410000'),
      'phasing_result_magic':('2377b65','3dca62a903'),
      'phasing_mark_out':('2377c68','c744240c01000000'),
      'phasing_mark_in':('2377c9c','c744240c01000000'),
      'phasing_complete_out':('2377cec','c744240c00000000')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,(n,get(int(a,16),len(bytes.fromhex(b))).hex())
    arrays={};table=None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/engine.c','cards/mirage.c'}:continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_variable':continue
            n=inherited_value(d,'DW_AT_name');p=d.get_parent()
            while p is not None and p.tag!='DW_TAG_subprogram':p=p.get_parent()
            owner=inherited_value(p,'DW_AT_name') if p is not None else None
            if (owner,n) not in {('untap_phase','untapped'),('untap_phase','buf'),('upkeep_phase','str'),('sub_495BE0','num_per_col'),('untap_phasing','shimmer_subtypes')}:continue
            t=unqual(type_of(d));assert t.tag=='DW_TAG_array_type'
            arrays[owner+'::'+n]=dict(element_bytes=value(unqual(type_of(t)),'DW_AT_byte_size'),bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'],location=value(d,'DW_AT_location'))
            if owner=='untap_phasing':
                loc=value(d,'DW_AT_location');assert loc==[3,160,71,116,2]
                table=dict(address='27447a0',values=list(struct.unpack('<14i',get(0x27447a0,56))),bytes=get(0x27447a0,56).hex())
    assert {k:(v['element_bytes'],v['bounds']) for k,v in arrays.items()}=={'untap_phase::untapped':(4,[150]),'untap_phase::buf':(1,[199]),'upkeep_phase::str':(1,[99]),'sub_495BE0::num_per_col':(4,[5]),'untap_phasing::shimmer_subtypes':(4,[13])}
    assert arrays['untap_phase::untapped']['location']==arrays['untap_phase::buf']['location'] and table['values'][-1]==-1 and all(x>=0 for x in table['values'][:-1])
    symbols={};literals={};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a in ADDRESSES:
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert len(ss)==1 and ss[0]['storage']==2;symbols[a]=ss[0]
        insns=list(decoder.disasm(get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16)),int(a,16)))
        assert not any(o.type==X86_OP_IMM and o.imm==0x437620 for i in insns for o in i.operands)
        for i in insns:
            for o in i.operands:
                if o.type==X86_OP_IMM and o.imm in {0x2764aab,0x2764ac6}:
                    data=readonly_string(runtime,o.imm);assert data is not None;literals[f'{o.imm:x}']=dict(bytes=(data+b'\0').hex(),literal=byte_literal(data))
    assert set(literals)=={'2764aab','2764ac6'}
    assert readonly_string(runtime,0x2764aab)==b'%d: Entering Untap Phase.\n' and readonly_string(runtime,0x2764ac6)==b'%d: Entering UpKeep Phase.\n'
    # The native callback has been folded to the same trivial return-zero body.
    callback=[s for s in debug.symbols if s[0]=='_target_player_skips_untap'];assert len(callback)==1 and callback[0][4]==2
    sy=callback[0];at=debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sy[2]-1].VirtualAddress+sy[1];assert at==0x2679200 and get(at,3)==bytes.fromhex('31c0c3')
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,arrays=arrays,shimmer_table=table,retained_skip_callback=dict(address=f'{at:x}',entry_bytes=get(at,8).hex(),coff_storage=2),display_masks=dict(state=0x10b21010,token=0x20000),poison_view=dict(address='4ef530',players=2,row_bytes=4,poison_bytes=1,threshold=10),deck_view=dict(max_entries=500,exclusion_mask=0x88000,iid_mask=0x7fff,initial_entries=200,initial_row_bytes=1600,initial_record_bytes=8,tie_order=[1,2,3,4,5]),exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},literals=literals,anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},
        behavior=['Retain attack state/callback save/restore, AI-only per-card scan and its repeated attacker lookup/call quirk, conditional opponent legality dispatch and all display comparison fields/masks. Death checks use byte poison fields at four-byte strides; speculation converts lethal poison to -99 life and returns zero.',
        'Untap begins the turn and scans all in-play cards before honoring a one-shot skip. Clear active-player no-attack sickness, consume at most one skip callback, aggregate Shimmer and opposing Teferi Curse/Cloak attachments, then phase with three arguments. Preserve 151 untap markers, 200-byte trace buffer at a separate lifetime, byte-only status clearing, mana recount, untap events, full recalculation/stops/end-event and mana burn.',
        'Untap phasing preserves the fourteen-entry subtype table, simultaneous two-pass marks, opposing attachment phasing callbacks, magic result, and native mode zero for both completion workers. Original two-argument phasing remains beside the new qualified API.',
        'Upkeep retains the 100-byte trace buffer, postincremented trace index, phase-stop/speculation reason 17/budget 10, repeated AI loop, both upkeep triggers and all cleanup. The obsolete EXE upkeep-flag reset call is absent in 2022 and omitted here.',
        'The color helper counts all applicable color bits across 500 Deck entries or 200 weighted initial-library entries, preserves exclusion/IID masks and first-in-enum-order tie breaking, including the empty-deck black result.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore complete turn/untap/phasing flow, native attack/display/death/color behavior and typed state storage',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('turn_flow_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_helpers']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta,root)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['turn_flow_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-turn-flow' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    for old in component['retained_original_handlers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for a in PUBLIC_HELPERS:assert entries[a]['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert component['retained_untap_fragments']==[f for f in manifest['source_fragments'] if f['address']==UNTAP]



def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta,root);(cache/'export-request-turn-flow.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared eight public turn/combat/display/phasing helpers.');return
    assert 'turn_flow_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[(meta[a]['source'],meta[a]['name']) for a in ADDRESSES]+[('functions/engine.c','check_attack_legality_if_enchanting_affected_card'),('functions/functions.c','target_player_skips_untap'),('functions/produce_mana.c','mana_burn'),('functions/ai.c','ai_decision_phase')]
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta,root),**debug_evidence(debug,meta,ADDRESSES),functions={},retained_untap_fragments=[f for f in m['source_fragments'] if f['address']==UNTAP],
        retained_helpers=[dict(name=n,source=source,body_sha256=sha(function_body(read(root/source),n).encode())) for source,n in retained],retained_original_handlers=[],
        scope='Recover eight public turn/combat/display/death/deck-color/phasing helpers in their original units. Preserve native untap skip scanning, sickness clearing, attachment-driven phasing, three-argument/two-pass phasing, upkeep speculation and missing obsolete reset call, exact attack quirks, comparison fields, byte poison checks and color ties. Retain all older bodies, source fragments, accepted records and EXE bindings. Downstream caller/dispatch/gameplay integration remains work.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-turn-flow'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],turn_flow_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(next(e['prototype'] for e in entries if e['address']==a) for a in PUBLIC_HELPERS)+'\n#endif\n\n'+header[at:]
    # Match check_source.py's existing MSVC-host compatibility alias, only in
    # the disposable syntax-check copy. GCC's original source stays unchanged.
    syntax_sources={s:('#define strncasecmp _strnicmp\n'+t if s=='functions/rules_engine.c' else t) for s,t in sources.items()}
    compile_sources(root,cache/'turn-flow-check',syntax_sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-turn-flow/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:
            if row['address']==UNTAP:
                assert read(cache/'decompiled-turn-flow'/f'{UNTAP}.c')==read(cache/'decompiled-abi'/f'{UNTAP}.c')
                continue
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-turn-flow'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=8;m['helper_functions']+=8;m['shared_helper_interfaces']+=8
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['turn_flow_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'turn-flow-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=8)))


if __name__=='__main__':main()
