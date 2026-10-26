"""Recover stack placement, activation legacies, trigger response and opponent/deck utilities."""
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
from stack_utility_bodies import BODIES

LEGACY='26a9420';ACTIVATION='26adc00';STACK='26afd30';WRAPPER='26ae510';TRIGGER='26cbf50';LIMIT='26c7890';ROGUE='26c78f0'
ADDRESSES=[LEGACY,ACTIVATION,STACK,WRAPPER,TRIGGER,LIMIT,ROGUE];CARDS=set();PRIVATE=set();HELPERS=set(ADDRESSES);PUBLIC_HELPERS=ADDRESSES
NAMES={LEGACY:'create_legacy_activate_from_iid',ACTIVATION:'fx_activation_906',STACK:'put_card_on_stack_2022',WRAPPER:'put_card_on_stack2_2022',TRIGGER:'allow_response_to_trigger',LIMIT:'limit_allowed_in_deck',ROGUE:'random_rogue'}
LABEL='/* October 2022 stack, activation, trigger-response and deck/opponent utilities. */'
EXISTING_DECL='int compute_and_check_casting_cost(int player, int unused, int card);'


def native_storage(runtime,debug,meta):
    import struct
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
      'stack_trace_old_counter':('26b02c5','89442408'),
      'stack_trace_name':('26b02db','8d9111707e00'),
      'stack_trace_card':('26b02c9','894c2414'),
      'stack_reset_selected':('26afd9d','c70530fa7800ffffffff'),
      'stack_can_cast_types':('26afdbb','f604c538707e003c'),
      'stack_capacity':('26afe03','833da85750001f'),
      'stack_play_state':('26afe45','0d20000300'),
      'stack_hand_decrement_before_player_test':('26b0047','4883feff'),
      'stack_response_event':('26b0264','c744240cd3000000'),
      'stack_response_first_argument':('26b0274','c70424feffffff'),
      'stack_response_sleep_milliseconds':('26b0296','c70424b80b0000'),
      'stack_response_sleep_stdcall_compensation':('26b02a4','51'),
      'stack_ai_penalty_argument':('26b0423','c7042401000000'),
      'stack_undo_payment_player':('26b03da','893424'),
      'stack_discard_owner_result':('26afeb9','ffd0'),
      'stack_failure_mask':('26aff09','81e25dfffcff'),
      'stack_second_call_state':('26affd3','250000c0ff0580004000'),
      'stack_wrapper_interrupt_gate':('26ae519','813de8bc6200d3000000'),
      'legacy_self_source_card':('26a94a1','895648'),
      'legacy_target_recalc':('26a950d','81ca0000001f'),
      'activation_parent_player':('26adc35','8b900c010000'),
      'activation_parent_card':('26adc41','8b9010010000'),
      'rogue_count_snapshot':('26c78f9','8b35b8707300'),
      'rogue_vla_elements':('26c78ff','8d04b500000000'),
      'rogue_stack_probe':('26c7906','e8c5480300'),
      'rogue_vla_allocation':('26c790b','29c4'),
      'rogue_stride':('26c7951','05a8120000'),
      'rogue_image_offset':('26c7935','80b8f401000000'),
      'rogue_difficulty_offset':('26c793e','39b800030000'),
      'rogue_abort_terminal':('26c79b4','e8ffa40300'),
      'limit_seventeen_entries':('26c78a1','83f811'),
      'limit_default_seven_or_four':('26c78c4','8d444004')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,n
    arrays={};rogue_layout={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/engine.c','functions/rules_engine.c'}:continue
        for d in cu.iter_DIEs():
            n=value(d,'DW_AT_name')
            if d.tag=='DW_TAG_typedef' and n=='rogue_t':
                t=unqual(type_of(d));rogue_layout=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member'})
            if d.tag!='DW_TAG_variable':continue
            parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            owner=value(parent,'DW_AT_name') if parent is not None else None
            if (owner,n) not in {('put_card_on_stack','str'),('random_rogue','ok_rogues'),('limit_allowed_in_deck','unlimited_list')}:continue
            t=unqual(type_of(d));assert t.tag=='DW_TAG_array_type'
            arrays.setdefault(owner+'::'+n,[]).append(dict(element_bytes=value(unqual(type_of(t)),'DW_AT_byte_size'),bounds=[dict(form=c.attributes['DW_AT_upper_bound'].form,value=value(c,'DW_AT_upper_bound')) for c in t.iter_children() if c.tag=='DW_TAG_subrange_type']))
    assert rogue_layout==dict(bytes=4776,members=dict(name=0,quote=200,imgname=500,difficulty=768,padding=772))
    assert sorted(x['bounds'][0]['value'] for x in arrays['put_card_on_stack::str'])==[299,499],arrays
    assert arrays['limit_allowed_in_deck::unlimited_list'][0]['bounds'][0]['value']==16
    assert arrays['random_rogue::ok_rogues'][0]['bounds'][0]['form'].startswith('DW_FORM_ref')
    unlimited=list(struct.unpack('<17i',get(0x2767dc0,68)))
    assert unlimited==[91,126,164,188,239,2100,2101,2102,2103,2104,20819,15719,22636,19685,18489,7107,13335]
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;literals={};native_symbols={};dynamic={}
    exe_targets={0x430150:0,0x4301e0:0,0x430200:1,0x435940:1,0x436700:0,0x4373b0:2,0x439690:2,0x436a20:4,0x471b60:6,0x4779f0:3,0x4a7d80:1,0x4d5d32:1}
    boundaries={}
    for a in ADDRESSES:
        code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(code,int(a,16)))
        dynamic[a]=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),operand=i.op_str) for i in insns if i.mnemonic=='call' and i.operands[0].type!=X86_OP_IMM]
        boundaries[a]=[]
        for i in insns:
            for op in i.operands:
                if op.type!=X86_OP_IMM:continue
                literal=readonly_string(runtime,op.imm)
                if literal is not None:literals[f'{op.imm:x}']=dict(bytes=(literal+b'\0').hex(),literal=byte_literal(literal))
                if i.mnemonic=='mov' and op.imm in exe_targets:boundaries[a].append(dict(load_address=f'{i.address:x}',load_bytes=i.bytes.hex(),target=f'{op.imm:x}',arguments=exe_targets[op.imm],calling_convention='stdcall' if op.imm==0x4d5d32 else 'cdecl'))
        syms=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(syms)==1 and syms[0]['storage']==2;native_symbols[a]=syms[0]
    assert len(dynamic[STACK])==15 and len(dynamic[TRIGGER])==2,{a:len(v) for a,v in dynamic.items()}
    abort_symbols=[n for n,v,sec,k,st in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==0x2701eb8]
    assert abort_symbols==['_abort']
    return dict(arrays=arrays,rogue_layout=rogue_layout,unlimited_list=unlimited,literals=literals,native_symbols=native_symbols,dynamic_exe_calls=dynamic,exe_boundaries=boundaries,abort_symbol=abort_symbols[0],anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},retained_native_behavior=[
      'Stack placement retains both phases, counter/hand bookkeeping, stack state snapshots, all response/payment/trace/AI/cleanup calls and original branch order. Token failure uses the EXE discard result as the player index.',
      'The wrapper sets the interrupt-only flag for event 0xd3 and clears it on every return from that branch, rather than restoring its old value. Original EXE stack linker bindings remain available.',
      'The activation proxy clears activating globals to -1, rather than restoring their old values. The legacy constructor sources itself and marks an attached target for full recalculation.',
      'The opponent count and VLA allocation precede settings refresh. Selection first requires matching difficulty, then falls back to all named/image-bearing entries. The no-opponent path pops up an error and aborts; the raw decompiler continuation belongs to the following function.',
      'The native 17-entry unlimited deck list returns 5000; Seven Dwarves returns seven and other cards four. This does not replace the retained older unlimited predicate or migrate deck-legality callers.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore both stack phases, complete typed EXE call frames, activation proxy/legacy state, 500-byte prompt, 17-entry deck table, dynamic opponent storage and terminal abort boundary',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('stack_utility_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    for old in component['retained_helpers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['stack_utility_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-stack-utility-helpers' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    for old in component['retained_original_handlers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for a in PUBLIC_HELPERS:assert entries[a]['prototype'] in read(root/'manalink.h') or component.get('planning')
    header=read(root/'manalink.h');assert EXISTING_DECL in header or component.get('planning')
    linker=read(root/'manalink.lds');assert '_put_card_on_stack = 0x4338F0;' in linker and '_put_card_on_stack2 = 0x433850;' in linker



def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta);(cache/'export-request-stack-utility-helpers.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared seven public stack/activation, trigger and deck/opponent helpers.');return
    assert 'stack_utility_helpers_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[('functions/engine.c',n) for n in ['put_card_on_stack3','put_card_or_activation_onto_stack','recopy_card_onto_stack']]+[('functions/events.c',n) for n in ['call_card_fn','dispatch_event_to_single_card']]+[('functions/rules_engine.c',n) for n in ['unlimited_allowed_in_deck','read_and_adjust_settings']]+[('functions/play_spell_for_free.c','compute_and_check_casting_cost')]
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=n,source=source,body_sha256=sha(function_body(read(root/source),n).encode())) for source,n in retained],retained_original_handlers=[],
        scope='Recover seven public helpers in their original units: both phases of stack placement and wrapper, activation dispatch proxy, activated legacy constructor, trigger response, deck limits and random opponent selection. Preserve existing source APIs and EXE stack linker bindings; qualify the two new stack entries until downstream migration. Verify complete EXE frames, 500-byte prompt, seventeen table values, 4776-byte opponent layout, native VLA allocation, error/abort boundary and all native state/cleanup behavior. Downstream dispatch/gameplay integration remains separate.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-stack-utility-helpers'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],stack_utility_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+EXISTING_DECL+'\n'+'\n'.join(next(e['prototype'] for e in entries if e['address']==a) for a in PUBLIC_HELPERS)+'\n#endif\n\n'+header[at:]
    # Match check_source.py's existing MSVC-host compatibility alias, only in
    # the disposable syntax-check copy. GCC's original source stays unchanged.
    syntax_sources={s:('#define strncasecmp _strnicmp\n'+t if s=='functions/rules_engine.c' else t) for s,t in sources.items()}
    compile_sources(root,cache/'stack-utility-helpers-check',syntax_sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-stack-utility-helpers/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-stack-utility-helpers'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=7;m['helper_functions']+=7;m['shared_helper_interfaces']+=7
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['stack_utility_helpers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'stack-utility-helpers-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=7)))


if __name__=='__main__':main()
