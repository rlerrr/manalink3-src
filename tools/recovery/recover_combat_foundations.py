"""Recover combat legality, minimum-blocker enforcement/selection and speculative stat setup."""
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
from combat_foundation_bodies import BODIES

MINIMUM='26aa220';LEGAL2='26aa280';ENFORCE='26aa700';IMPL='26ab5a0';LEGAL='26ab6b0';HUMAN='26ab990';SETUP='26ac0a0'
ADDRESSES=[MINIMUM,LEGAL2,ENFORCE,IMPL,LEGAL,HUMAN,SETUP];CARDS=set();PRIVATE={LEGAL2,ENFORCE};HELPERS=set(ADDRESSES);PUBLIC_HELPERS=[a for a in ADDRESSES if a not in PRIVATE]
NAMES={MINIMUM:'minimum_blockers_2022',LEGAL2:'is_legal_block_impl2',ENFORCE:'enforce_minimum_blockers',IMPL:'is_legal_block_impl_2022',LEGAL:'is_legal_block_2022',HUMAN:'human_assign_blockers_2022',SETUP:'setup_ai_combat_data'}
LABEL='/* October 2022 combat legality, blocker selection and speculative stat setup. */'


def native_storage(runtime,debug,meta):
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
      'minimum_three_flag':('26aa22e','c744240800000800'),
      'minimum_two_flag':('26aa24d','c744240800000100'),
      'legality_blocker_tap_override':('26aa2d4','8b049d18727902'),
      'legality_conditional_illegal_state':('26aa2e3','83e0100508008000'),
      'legality_protection_mode':('26aa361','c744241004000000'),
      'legality_key_suppression':('26aa42f','c744240840000000'),
      'legality_blocked_extension_pointer':('26aa455','8b402c8b00'),
      'wrapper_unsigned_result_test':('26ab5d2','83f8010f8681000000'),
      'wrapper_signed_negative_test':('26ab5e2','0f8e80000000'),
      'human_illegal_target_state_zero':('26aba28','c7458c00000000'),
      'human_first_prompt':('26abb1b','c744240cac5c7100'),
      'human_second_prompt':('26abb5c','c744240cd85d7100'),
      'human_failed_block_prompt':('26abd2c','c70424045f7100'),
      'human_sleep_import':('26abd38','b8325d4d00'),
      'setup_table_base':('26ac0a1','ba64916000'),
      'setup_table_clear_words':('26ac0ad','b9b0040000'),
      'setup_table_clear':('26ac0e5','f3ab'),
      'setup_two_players':('26ac0f7','c745e002000000'),
      'setup_human_attacking_vigilant':('26ac2fd','83c804'),
      'setup_human_attacking_tapped':('26ac30b','83c914'),
      'setup_cop_mana_quantity':('26ac31b','c744240801000000'),
      'setup_cop_mana_color':('26ac323','c744240407000000')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,n
    arrays={};types={};filters={};table_declarations=[]
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/engine.c':continue
        for d in cu.iter_DIEs():
            n=value(d,'DW_AT_name')
            if d.tag=='DW_TAG_typedef' and n=='ai_combat_data_t':
                t=unqual(type_of(d));types[n]=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member'})
            if d.tag!='DW_TAG_variable':continue
            parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            owner=value(parent,'DW_AT_name') if parent is not None else None
            if owner not in {meta[a]['name'] for a in ADDRESSES}:continue
            if (owner,n)==('enforce_minimum_blockers','num_blocking'):
                t=unqual(type_of(d));arrays[owner+'::'+n]=dict(element_bytes=value(unqual(type_of(t)),'DW_AT_byte_size'),bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'])
            if owner=='human_assign_blockers' and n in {'td_choose_blocker','td_block_which_attacker'}:filters[n]=value(unqual(type_of(d)),'DW_AT_byte_size')
    assert types['ai_combat_data_t']==dict(bytes=16,members=dict(potential_power=0,potential_toughness=4,abils=8,opinion_of_game_state_without_this=12))
    assert arrays=={'enforce_minimum_blockers::num_blocking':dict(element_bytes=4,bounds=[150])}
    assert filters==dict(td_choose_blocker=84,td_block_which_attacker=84)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;literals={};native_symbols={};dynamic={};references={}
    exe_targets={0x499df0:2,0x472260:0,0x4350e0:4,0x435c80:0,0x435cd0:0,0x4d5d32:1}
    boundaries={};words={0x609164:'combat_data',0x7a5378:'speculating_during_combat',0x60a424:'check_abilities_enemiesagainstcolor',0x60a428:'check_abilities_cop_colors',0x60a42c:'cop_mana',0x7a3610:'activating_player',0x739b50:'check_abilities_power',0x7a371c:'check_abilities_toughness',0x78fef0:'check_abilities_keywords'}
    from capstone.x86_const import X86_OP_MEM
    for a in ADDRESSES:
        code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(code,int(a,16)))
        dynamic[a]=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),operand=i.op_str) for i in insns if i.mnemonic=='call' and i.operands[0].type!=X86_OP_IMM]
        boundaries[a]=[];references[a]=[]
        for i in insns:
            for op in i.operands:
                if op.type==X86_OP_MEM and op.mem.disp in words:references[a].append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{op.mem.disp:x}',width=op.size,name=words[op.mem.disp]))
                if op.type!=X86_OP_IMM:continue
                literal=readonly_string(runtime,op.imm)
                if literal is not None:literals[f'{op.imm:x}']=dict(bytes=(literal+b'\0').hex(),literal=byte_literal(literal))
                if i.mnemonic=='mov' and op.imm in exe_targets:boundaries[a].append(dict(load_address=f'{i.address:x}',load_bytes=i.bytes.hex(),target=f'{op.imm:x}',arguments=exe_targets[op.imm],calling_convention='stdcall' if op.imm==0x4d5d32 else 'cdecl'))
        syms=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(syms)==1 and syms[0]['storage']==(3 if a in PRIVATE else 2);native_symbols[a]=syms[0]
    assert len(dynamic[LEGAL])==1 and len(dynamic[HUMAN])==6
    assert len(boundaries[LEGAL])==1 and len(boundaries[HUMAN])==6
    return dict(arrays=arrays,types=types,target_bytes=filters,literals=literals,native_symbols=native_symbols,dynamic_exe_calls=dynamic,exe_boundaries=boundaries,exe_global_references=references,combat_table=dict(address='609164',players=2,cards_per_player=150,record_bytes=16,clear_bytes=4800),anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},retained_native_behavior=[
      'The private legality worker returns zero for illegal blocks, negative one for at most one blocker, or a positive minimum. The wrapper combines unsigned >1 with a signed <2 branch and treats other legal/potential blockers exactly as the native code does.',
      'A per-player flag permits tapped blockers. Protection uses mode four and KEY suppression; special keywords are read from the extension. Menace/three-blocker requirements combined with the one-blocker restriction are illegal.',
      'The enforcer keeps a 151-element count array, removes all blockers of an invalid group, clears both blocking/selection bits and retains the native consistency assertion. The count getter remains distinct from the retained older four-argument setter.',
      'Human selection checks current attackers and candidates repeatedly, retains attacker-chooses-blockers mode, zero target illegal-state mask, paid-block and blocker-chosen timing, band mapping, attacker state bit 0x4000, typed try-block/reassessment/Sleep calls and final enforcement.',
      'Speculative setup clears the 4800-byte EXE table, visits the acting player first then the opponent, sets temporary combat state and persistent recalculation bits, seeds and dispatches EVENT_CHECK_ABILITIES with the full saved activating-player word, combines indestructible/double-strike state, restores card state, preserves opponent COP colors and restores speculation flags only after recalculation.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore signed minimum/maximum blocker results, extension-backed legality, typed human target/callback/EXE frames, fixed blocker counts and speculative combat record/state accumulation',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('combat_foundation_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_helpers']:assert sha(retained_source_body(root,old['source'],old['name'],source_view=sources).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['combat_foundation_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-combat-engine-helpers' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    for old in component['retained_original_handlers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for a in PUBLIC_HELPERS:assert entries[a]['prototype'] in read(root/'manalink.h') or component.get('planning')
    ps=component['parameters'][LEGAL2];assert [p['location'][0]['expression'] for p in ps[:3]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    assert [p['name'] for p in ps]==['blocking_player','blocking_card','blocked_player','blocked_card','abils','landwalks']



def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta);(cache/'export-request-combat-foundations.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared seven combat foundations with two private workers and five public helpers.');return
    assert 'combat_foundations_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[('functions/engine.c',n) for n in ['is_legal_block_impl','is_legal_block','human_assign_blockers']]+[('functions/functions.c','minimum_blockers')]
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=n,source=source,body_sha256=sha(function_body(read(root/source),n).encode())) for source,n in retained],retained_original_handlers=[],
        scope='Recover seven combat foundations: a minimum-blocker getter, private legality/enforcement workers, qualified public legality frontends and human selection, and speculative combat-data setup. Restore unsigned/signed minimum/maximum semantics, extension keyword/KEY suppression/protection, the 151-element blocker array, both 84-byte targets, typed EXE frames and the 4800-byte combat table with native state/accumulator/COP behavior. Keep original legality/human bodies and the older setter. The large AI combination-search routines and downstream dispatch/gameplay migration remain separate recovery work.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-combat-engine-helpers'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],combat_foundation_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(next(e['prototype'] for e in entries if e['address']==a) for a in PUBLIC_HELPERS)+'\n#endif\n\n'+header[at:]
    # Match check_source.py's existing MSVC-host compatibility alias, only in
    # the disposable syntax-check copy. GCC's original source stays unchanged.
    syntax_sources={s:('#define strncasecmp _strnicmp\n'+t if s=='functions/rules_engine.c' else t) for s,t in sources.items()}
    compile_sources(root,cache/'combat-foundations-check',syntax_sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-combat-engine-helpers/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-combat-engine-helpers'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=7;m['helper_functions']+=7;m['shared_helper_interfaces']+=5;m['private_functions']+=2;m['private_helpers']+=2
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['combat_foundations_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'combat-foundations-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=7)))


if __name__=='__main__':main()
