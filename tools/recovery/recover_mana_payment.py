"""Recover full 2022 mana payment and seven resolving/multi-color frontends."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
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
from mana_payment_bodies import BODIES

CORE='26ae610';MULTI='267bc50';ARTIFACT='267bcb0';RESOLVE='267bd20';CSV='267bda0';MULTI_RESOLVE='267be70';MULTI_CSV='267bf30';HYBRID='267c030'
ADDRESSES=[CORE,MULTI,ARTIFACT,RESOLVE,CSV,MULTI_RESOLVE,MULTI_CSV,HYBRID];PRIVATE=set();PUBLIC_HELPERS=ADDRESSES
NAMES={CORE:'charge_mana_2022',MULTI:'charge_mana_multi_2022',ARTIFACT:'charge_mana_multi_a_2022',RESOLVE:'charge_mana_while_resolving_2022',CSV:'charge_mana_while_resolving_csvid_2022',MULTI_RESOLVE:'charge_mana_multi_while_resolving_2022',MULTI_CSV:'charge_mana_multi_while_resolving_csvid_2022',HYBRID:'charge_mana_hybrid_while_resolving_2022'}
LABEL='/* October 2022 complete mana-payment engine and resolving frontends. */'
PRIVATE_TYPES='''#define payment_amt_to_apply_to_cost EXE_FN(int, 0x4300d0, int *, int, int *, int, int, int, int)
#define payment_apply_mana_in_pool_to_colorless_cost EXE_FN(void, 0x42ef80, int, int *, int *, int, int *, int)
#define payment_apply_mana_in_pool_to_cost EXE_FN(void, 0x42ec40, int, int *, int *, int)
#define payment_apply_mana_in_pool_to_x_cost EXE_FN(void, 0x42edc0, int, int *, int *, int, int *, int)
#define payment_apply_mana_to_cost EXE_FN(void, 0x430060, int *, int, int, int *, int, int, color_t, int *, int *)
#define payment_calculate_needed_mana_colors EXE_FN(int, 0x42f1a0, int *)
#define payment_copy_mana_pool_to_display EXE_FN(void, 0x4388d0, void)
#define payment_TENTATIVE_reassess_all_cards EXE_FN(void, 0x472260, void)
#define payment_resolve_top_card_on_stack EXE_FN(void, 0x436740, void)
#define payment_single_color_test_bit_to_color_t EXE_FN(color_t, 0x4358f0, color_test_t)
#define payment_TENTATIVE_update_mana_spent EXE_FN(void, 0x430260, int *)
#define payment_TENTATIVE_color_clicked_in_mana_summary_pane EXE_DWORD(0x60a52c)
#define payment_global_all_purpose_buffer ((char *)0x60a690)
#define payment_WILDGUESS_phase_to_run_to EXE_DWORD(0x4ef1ac)
#define payment_raw_mana_may_be_spent_as_color ((int (*)[11])0x60a4b4)
#define payment_WILDGUESS_whose_phase_to_run_to EXE_DWORD(0x4ef1b0)
'''
EXE_TARGETS=[0x4300d0,0x42ef80,0x42ec40,0x42edc0,0x430060,0x42f1a0,0x4388d0,0x472260,0x436740,0x4358f0,0x430260,0x498f20,0x499050,0x49c8d0,0x42f130,0x42f4a0,0x49d490,0x49d510,0x4359b0,0x436930]


def review_cpp_candidates(debug,runtime,root):
    wanted={0x270d580:'stl_heap.h',0x270d6b0:'stl_algo.h',0x27096c0:'hashtable.h',0x270d810:'stl_algo.h',0x270d8c0:'stl_algo.h',0x26eea80:'utility.cpp'};result=[]
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/utility.cpp':continue
        hdr=debug.dwarf().line_program_for_CU(cu).header
        for d in cu.iter_DIEs():
            a=value(d,'DW_AT_low_pc')
            if d.tag!='DW_TAG_subprogram' or a not in wanted:continue
            entry=hdr['file_entry'][inherited_value(d,'DW_AT_decl_file')-1];name=entry.name.decode();di=entry.dir_index;directory=hdr['include_directory'][di-1].decode() if di else value(cu.get_top_DIE(),'DW_AT_comp_dir');path=str(directory)+'/'+name
            assert Path(name).name==wanted[a]
            high=d.attributes['DW_AT_high_pc'];end=high.value if high.form=='DW_FORM_addr' else a+high.value;code=runtime.pe.get_data(a-0x2000000,end-a)
            result.append(dict(address=f'{a:x}',name=inherited_value(d,'DW_AT_name'),decl_file=path,line=inherited_value(d,'DW_AT_decl_line'),linkage=inherited_value(d,'DW_AT_linkage_name'),native_bytes=len(code),machine_sha256=sha(code),review='GCC standard-library instantiation; existing sort/map source providers retained' if a!=0x26eea80 else 'Manalink csvid_t::iid member; still requires implementation; only observed native callers are deferred chooser routines'))
    assert len(result)==6 and sum('/include/c++/bits/' in r['decl_file'] for r in result)==5
    providers=[dict(name=n,source='functions/utility.cpp',body_sha256=sha(function_body(read(root/'functions/utility.cpp'),n).encode())) for n in ['sort_ArrayConstCharPtr','less_const_char','UnorderedMapIntInt_set']]
    return dict(candidates=result,providers=providers,scope='Declaration-file and provider review only. No additional recovered implementations and no byte-matching claim. The ID-conversion member remains an implementation candidate, and modified original bodies remain outside this name inventory.')


def native_storage(runtime,debug,meta,root):
    arrays=[];pointer_slots={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/engine.c':continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_variable':continue
            parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            if parent is None or value(parent,'DW_AT_low_pc')!=int(CORE,16):continue
            name=inherited_value(d,'DW_AT_name');t=unqual(type_of(d));loc=value(d,'DW_AT_location')
            if t.tag=='DW_TAG_array_type':arrays.append(dict(name=name,element_bytes=value(unqual(type_of(t)),'DW_AT_byte_size'),bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'],location=loc))
            if name in {'old_charge_mana_addr_of_pre_mana_pool','old_charge_mana_addr_of_pay_mana_xbugrw','old_charge_mana_addr_of_x_value','old_charge_mana_addr_of_max_x_value'}:
                assert t.tag=='DW_TAG_pointer_type' and value(t,'DW_AT_byte_size')==4;pointer_slots[name]=dict(bytes=4,location=loc)
    assert {x['name']:(x['element_bytes'],x['bounds']) for x in arrays}==dict(amtspaid=(4,[6]),buffer=(1,[149]),old_mana_pool=(4,[7]),old_pay_mana=(4,[7]))
    assert next(x for x in arrays if x['name']=='buffer')['location']==next(x for x in arrays if x['name']=='old_pay_mana')['location'] and len(pointer_slots)==4
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
        'seven_paid_words':('26ae64b','b907000000'),'seven_paid_clear':('26ae66a','f3ab'),
        'copy_mode_flag':('26ae677','2500040000'),'rollback_target':('26ae75b','b860024300'),
        'cost_reset_eight_words':('26ae6cf','c7051cf44e0000000000'),
        'pool_snapshot_eight_words':('26aee33','83f808'),'paid_snapshot_eight_words':('26aee1b','83f808'),
        'stack_capacity_guard':('26aef6c','833da85750001f'),'stack_guard_branch':('26aef79','0f8e03030000'),
        'tap_state_test':('26af027','f6400810'),'tap_event':('26af031','c744240881000000'),
        'tap_trigger':('26af065','c7442404d2000000'),'tap_sound':('26af075','c7042412000000'),
        'krrik_can_pay_life':('26af2e9','c744240402000000'),'krrik_signed_half':('26af48d','c1eb1f01c3d1fb'),
        'krrik_ai_rejection':('26af7f8','812da8317a00e7030000'),
        'raw_conversion_masked_shift':('26af67f','d3e0'),'raw_conversion_stride_44':('26af692','39b50cffffff')}
    for name,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,(name,get(int(a,16),len(bytes.fromhex(b))).hex())
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;symbols={};frames={};settlement=[]
    for a in ADDRESSES:
        code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(code,int(a,16)))
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert len(ss)==1 and ss[0]['storage']==2;symbols[a]=ss[0]
        if a in {MULTI,ARTIFACT,MULTI_RESOLVE,MULTI_CSV}:
            writes=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),destination=f'{i.operands[0].mem.disp:x}',bytes_written=i.operands[0].size) for i in insns if i.mnemonic=='mov' and i.operands[0].type==X86_OP_MEM and i.operands[0].mem.base==i.operands[0].mem.index==0 and 0x4ef400<=i.operands[0].mem.disp<0x4ef420]
            count=7 if a==ARTIFACT else 6;assert len(writes)==count and all(x['bytes_written']==4 for x in writes) and {int(x['destination'],16) for x in writes}==set(range(0x4ef400,0x4ef400+4*count,4));frames[a]=writes
        if a in {RESOLVE,CSV,MULTI_RESOLVE,MULTI_CSV,HYBRID}:
            pop=next(i.address for i in insns if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==0x436930)
            condition=next((i.address for i in insns if i.mnemonic=='cmp' and i.operands[0].type==X86_OP_MEM and i.operands[0].mem.disp==0x4ef194),None)
            if a in {RESOLVE,CSV}:assert pop<condition
            elif a in {MULTI_RESOLVE,MULTI_CSV}:assert condition<pop
            settlement.append(dict(address=a,pop_address=f'{pop:x}',fizzle_test_address=f'{condition:x}' if condition is not None else None,return_timing='after cleanup' if a in {RESOLVE,CSV} else 'captured before cleanup'))
    assert len(frames)==4 and len(settlement)==5
    literals={0x2764a2c:b' Pay 2 life for how much black mana? (max %d, min %d)',0x2764a62:b'PROMPT_GRABMANA_DUALUSE'}
    for a,s in literals.items():assert readonly_string(runtime,a)==s
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(arrays=arrays,pointer_slots=pointer_slots,native_symbols=symbols,payment_request_stores=frames,resolving_return_timing=settlement,
        conversion_table=dict(address='60a4b4',players=2,row_elements=11,element_bytes=4,scanned_elements=10,row_bytes=44,shift_count_mask=31),
        exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},
        literals={f'{a:x}':dict(bytes=(s+b'\0').hex(),literal=byte_literal(s)) for a,s in literals.items()},anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},
        behavior=['Recover the full payment algorithm while preserving the older charge_mana body and its three fragment records. Keep seven paid-mana slots, eight-word cost/pool snapshots and four saved callback pointers. The 150-byte Krrik prompt and eight-word cost snapshot have different lifetimes at one native stack location.',
        'Preserve X speculation/replay, automatic/manual pool payment, six AI autotap flag sets, interactive source selection, the stack-capacity guard, newly-tapped event/trigger/sound dispatch and activation snapshot restoration.',
        'Keep native Krrik minimum only in the prompt, upper-bound validation, early cancellation/AI-rejection returns and final life settlement even after the mana rollback path. Restore all state/global resets and total-paid result.',
        'Four multi-color frontends write six/seven full-width request words without clamping. Resolving scalar variants inspect fizzle after cleanup; multi-color variants capture it before cleanup, and the hybrid variant captures its helper result. Both CSV variants balance the temporary invisible card and hand count.',
        'The raw conversion table has eleven-int rows but scans ten entries. Native x86 shift-count masking is explicit; all typed EXE call frames and original provider bodies are retained. Downstream caller migration remains separate work.'])


def reconstruct(a,raw,runtime,meta):
    body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=strip_comments(raw),replacement=body,occurrences=1,kind='restore the typed full payment flow, separate prompt and snapshot objects, native X/Krrik/activation/tap/rollback behavior and resolving frontend stores/return timing',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('mana_payment_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_helpers']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta,root) and component['cpp_candidate_review']==review_cpp_candidates(debug,runtime,root)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['mana_payment_evidence']==detail and not e['static']
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-mana-payment' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==2
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    assert PRIVATE_TYPES in sources['functions/engine.c'].replace('\r\n','\n') and component['private_declarations']==PRIVATE_TYPES
    header=read(root/'manalink.h') if not component.get('planning') else component['planned_header']
    for a in ADDRESSES:assert entries[a]['prototype'] in header
    assert component['retained_charge_fragments']==[f for f in manifest['source_fragments'] if f.get('address')==CORE]


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta,root);review_cpp_candidates(debug,runtime,root);print('Prepared the complete mana engine and seven typed frontends; reviewed five library instantiations and the remaining C++ member candidate.');return
    assert 'mana_payment_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[('functions/engine.c','charge_mana')]+[('functions/functions.c',meta[a]['name']) for a in ADDRESSES if a!=CORE]+[('functions/ai.c','human_autotap_for_mana'),('functions/engine.c','put_card_or_activation_onto_stack')]
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta,root),cpp_candidate_review=review_cpp_candidates(debug,runtime,root),**debug_evidence(debug,meta,ADDRESSES),functions={},private_declarations=PRIVATE_TYPES,
        retained_helpers=[dict(name=n,source=s,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in retained],retained_charge_fragments=[f for f in m['source_fragments'] if f.get('address')==CORE],
        scope='Recover complete October 2022 charge_mana plus seven multi-color/resolving frontends as qualified entries in their original units. Restore typed arrays, pointer snapshots, EXE frames, Krrik/X/autotap/manual payment, stack capacity, tap trigger/sound, rollback and settlement and exact cleanup/return timing. Preserve the older engine body, its three existing fragment records and ten providers/frontends. Review five standard-library instantiations without counting new implementations; the C++ ID-conversion member remains a candidate. Downstream callers/gameplay migration remains separate work.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=False) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={'functions/engine.c':read(root/'functions/engine.c')+'\n\n#ifndef SHANDALAR\n'+PRIVATE_TYPES+'\n#endif\n'};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=NAMES[a];raw=read(cache/'decompiled-mana-payment'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=2)
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],mana_payment_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'mana-payment-check',sources,header)
    proof['planning']=True;proof['planned_header']=header;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning'];del proof['planned_header']
    if args.apply:
        # CORE is already represented by three fragments with a dedicated raw
        # export. Preserve those records/files while adding its first full body.
        citations=m['accepted_functions']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};assert len(proof['retained_charge_fragments'])==3
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-mana-payment/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-mana-payment'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=8;m['helper_functions']+=8;m['shared_helper_interfaces']+=8
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['mana_payment_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'mana-payment-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=8)))


if __name__=='__main__':main()
