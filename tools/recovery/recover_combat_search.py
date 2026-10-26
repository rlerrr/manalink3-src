"""Recover recursive blocker scoring, blocker assignment and attacker combination search."""
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
from combat_search_bodies import BODIES

POWER='26a8440';RECURSE='26aa890';ASSIGN='26abd60';ATTACK='26ac340'
ADDRESSES=[POWER,RECURSE,ASSIGN,ATTACK];CARDS=set();PRIVATE={POWER,RECURSE};HELPERS=set(ADDRESSES);PUBLIC_HELPERS=[ASSIGN,ATTACK]
NAMES={POWER:'mod_pow_unblocked',RECURSE:'ai_assign_one_blocker',ASSIGN:'ai_assign_blockers_2022',ATTACK:'ai_choose_attackers'}
LABEL='/* October 2022 combat search and EXE-owned defending-player word. */'
EXISTING_DECL='void dispatch_event_raw(event_t event);'
MACRO='#define ai_combat_defending_player_2022 EXE_DWORD(0x607c2c)'


def native_storage(runtime,debug,meta):
    from capstone.x86_const import X86_OP_MEM
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
      'power_physical_registers':('26a8445','89cf5389d3'),
      'recursion_call_one':('26aa9c1','e8cafeffff'),
      'recursion_call_two':('26aaa1c','e86ffeffff'),
      'leaf_vla_elements_plus_one':('26aaa45','8d0c9504000000'),
      'leaf_vla_probe':('26aaa66','e865170500'),
      'leaf_vla_allocation':('26aaa6b','29c4'),
      'leaf_vla_clear':('26aaa81','f3ab'),
      'rampage_initialization_before_attackers':('26aaa96','c78530ffffff00000000'),
      'leaf_rating_signed_product':('26aae6e','f72c95148c6200'),
      'leaf_life_signed_product':('26ab263','f72c95148c6200'),
      'leaf_life_signed_division':('26ab290','e89b0f0500'),
      'leaf_best_score_signed_test':('26ab2a3','0f8df6f6ffff'),
      'commander_damage_counter':('26ab49d','c74424081c000000'),
      'commander_damage_limit':('26ab4c6','83f814'),
      'commander_damage_life_penalty':('26ab4e0','8d98f1d8ffff'),
      'fast_attacker_preincrement':('26ad6be','418b143a'),
      'fast_attacker_indexed_store':('26ad6c2','89948dd4f4ffff'),
      'fast_attacker_limit':('26ad6a3','83f90f'),
      'potential_attacker_qsort_stride':('26ad675','c74424080c000000'),
      'activating_player_sign_extension':('26ad390','0fbe8574f4ffff'),
      'activating_player_full_restore':('26ad397','a310367a00'),
      'assassin_initial_outer_zero':('26ad3e8','31c0'),
      'assassin_mapping_clobbers_outer_zero':('26ad44e','31c0'),
      'assassin_mapping_loop_counter_increment':('26ad495','83c002'),
      'assassin_outer_test':('26ad498','39c80f8c50ffffff')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,(n,get(int(a,16),len(bytes.fromhex(b))).hex())
    arrays=[];types={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/engine.c':continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_typedef' and n in {'PotentialAttacker','ai_combat_data_t'}:
                t=unqual(type_of(d));types[n]=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member'})
            if d.tag!='DW_TAG_variable' or n is None:continue
            parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
            if owner not in {'ai_assign_one_blocker','ai_choose_attackers'}:continue
            if value(parent,'DW_AT_low_pc') not in {int(RECURSE,16),int(ATTACK,16)}:continue
            t=unqual(type_of(d))
            if t.tag!='DW_TAG_array_type':continue
            element=unqual(type_of(t));bounds=[dict(form=c.attributes['DW_AT_upper_bound'].form,value=value(c,'DW_AT_upper_bound')) for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']
            arrays.append(dict(owner=owner,name=n,location=value(d,'DW_AT_location'),element_bytes=value(element,'DW_AT_byte_size'),bounds=bounds))
    assert len(arrays)==23 and sum(x['owner']=='ai_choose_attackers' for x in arrays)==20,[(x['owner'],x['name']) for x in arrays]
    vla=next(x for x in arrays if x['name']=='outflanked');assert vla['element_bytes']==4 and vla['bounds'][0]['form']=='DW_FORM_ref4'
    pot=next(x for x in arrays if x['name']=='pot_atck');assert pot['element_bytes']==12 and pot['bounds'][0]['value']==149
    pairs=next(x for x in arrays if x['name']=='cardpow');assert pairs['element_bytes']==8 and pairs['bounds'][0]['value']==7
    assert types['PotentialAttacker']==dict(bytes=12,members=dict(card=0,effectiveness=4,bands=8,one_of_best_7=9))
    assert types['ai_combat_data_t']['bytes']==16
    slots={x['name']:x['location'] for x in arrays if x['name'] in {'pot_atck','card_to_attacker_pt_idx','assassin_tgt','cardpow'}}
    assert slots['pot_atck']==slots['card_to_attacker_pt_idx'] and slots['assassin_tgt']==slots['cardpow']
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;literals={};native_symbols={};dynamic={};references={};boundaries={};word_refs={}
    exe_targets={0x4af6f0:1,0x4b4350:2,0x498e30:0,0x498e70:0,0x435c80:0,0x435cd0:0,0x437e20:0}
    for a in ADDRESSES:
        code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(code,int(a,16)))
        dynamic[a]=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),operand=i.op_str) for i in insns if i.mnemonic=='call' and i.operands[0].type!=X86_OP_IMM]
        boundaries[a]=[];word_refs[a]=[]
        for i in insns:
            for op in i.operands:
                if op.type==X86_OP_MEM and 0x607000<=op.mem.disp<0x60a500:word_refs[a].append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{op.mem.disp:x}',width=op.size))
                if op.type!=X86_OP_IMM:continue
                literal=readonly_string(runtime,op.imm)
                if literal is not None:literals[f'{op.imm:x}']=dict(bytes=(literal+b'\0').hex(),literal=byte_literal(literal))
                if i.mnemonic=='mov' and op.imm in exe_targets:boundaries[a].append(dict(load_address=f'{i.address:x}',load_bytes=i.bytes.hex(),target=f'{op.imm:x}',arguments=exe_targets[op.imm],calling_convention='cdecl'))
        syms=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(syms)==1 and syms[0]['storage']==(3 if a in PRIVATE else 2);native_symbols[a]=syms[0]
    division=[n for n,v,sec,k,st in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==0x26fc230]
    assert len(division)==1 and 'divdi3' in division[0]
    magic=Image((Path(__file__).resolve().parents[2]/'.recovery/2022-Magic.exe').read_bytes())
    assert sha(magic.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    section=next(s for s in magic.pe.sections if s.VirtualAddress<=0x607c2c-0x400000<s.VirtualAddress+s.Misc_VirtualSize);assert section.Characteristics&0x80000000
    return dict(arrays=arrays,types=types,stack_slot_reuse=slots,native_symbols=native_symbols,literals=literals,dynamic_exe_calls=dynamic,exe_boundaries=boundaries,combat_word_references=word_refs,defending_player=dict(address='607c2c',bytes=4,owner='Magic.exe',macro=MACRO),division_runtime=dict(address='26fc230',name=division[0],operation='signed 64-bit quotient; truncate toward zero'),anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},retained_native_behavior=[
      'The power helper retains all six ID cases and the Rogue bonus. Recursive blocker assignment retains fixed assignments, minimum/maximum restrictions, allowed-block masks and original recursion/backtracking updates.',
      'Terminal scoring restores the blockernum+1 VLA, both sixteen-int scratch arrays, flanking/first-strike/indestructibility and banding allocation, signed 64-bit score weighting/division, commander damage and Circle-of-Protection benefit selection. Rampage is initialized before the attacker loop and retains its native cross-attacker lifetime and repeated multiplication.',
      'PotentialAttacker[150] and the later 150-byte card map have different lifetimes at the same native stack address. CardPower[8] and assassin_tgt[16] also share a native address. Their source scopes remain distinct rather than writing through the shorter Ghidra arrays.',
      'Fast attacker preselection increments the index before storing into attack_indices[16], including the native unchecked sixteenth-index write. The assassin ranking performs one comparison pass before the mapping counter terminates its outer loop. Neither quirk is repaired.',
      'The callback boost path saves the activating player as int8_t and sign-extends it on restore. All sixteen-word snapshots, byte maps, combination masks, forced-attacker rules, score/penalty comparisons, callback/EXE calls, band assignment and final legality retries retain the native behavior. Existing old AI entry and accepted callbacks/workers survive unchanged.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore the recursive blocker/VLA scoring algorithm, distinct attacker temporary lifetimes, signed 64-bit math, exact snapshots/callback frames, candidate combination search and original unchecked/native state behavior',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('combat_search_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    for old in component['retained_helpers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['combat_search_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-combat-search' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    for old in component['retained_original_handlers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for a in PUBLIC_HELPERS:assert entries[a]['prototype'] in read(root/'manalink.h') or component.get('planning')
    for a,expected in [(POWER,['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']),(RECURSE,['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))'])]:
        ps=component['parameters'][a];assert [p['location'][0]['expression'] for p in ps]==expected
    header=read(root/'manalink.h');assert MACRO in header and EXISTING_DECL in header or component.get('planning')



def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta);(cache/'export-request-combat-search.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared four combat-search implementations with two private workers and two public helpers.');return
    assert 'combat_search_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[('functions/ai.c','ai_assign_blockers')]+[('functions/engine.c',n) for n in ['qsort_compare_attacker_effectiveness','qsort_cardvalue_ascending','enforce_minimum_blockers','is_legal_block_impl_2022','human_assign_blockers_2022','minimum_blockers_2022','setup_ai_combat_data']]+[('functions/events.c','dispatch_event_raw')]
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=n,source=source,body_sha256=sha(function_body(read(root/source),n).encode())) for source,n in retained],retained_original_handlers=[],
        scope='Recover four combat-search implementations in functions/engine.c: unblocked-power adjustment, recursive blocker assignment/scoring, qualified public blocker assignment and attacker-combination search. Restore register interfaces, the candidate VLA, all twenty-three native array objects and distinct physical slot lifetimes, typed signed 64-bit scoring, commander/COP/banding/first-strike handling, all callback/EXE frames, native score comparisons, preincrement and ranking quirks, state snapshots and final legality retries. Preserve the older functions/ai.c entry and nine existing providers/callbacks. Downstream caller/dispatch/gameplay integration remains separate work.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-combat-search'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],combat_search_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+MACRO+'\n'+EXISTING_DECL+'\n'+'\n'.join(next(e['prototype'] for e in entries if e['address']==a) for a in PUBLIC_HELPERS)+'\n#endif\n\n'+header[at:]
    # Match check_source.py's existing MSVC-host compatibility alias, only in
    # the disposable syntax-check copy. GCC's original source stays unchanged.
    syntax_sources={s:('#define strncasecmp _strnicmp\n'+t if s=='functions/rules_engine.c' else t) for s,t in sources.items()}
    compile_sources(root,cache/'combat-search-check',syntax_sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-combat-search/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-combat-search'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=4;m['helper_functions']+=4;m['shared_helper_interfaces']+=2;m['private_functions']+=2;m['private_helpers']+=2
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['combat_search_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'combat-search-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=4)))


if __name__=='__main__':main()
