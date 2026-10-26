"""Recover native draft orchestration/deck output and K'rrik-aware mana frontends."""
import argparse,collections,csv,json,re,shutil
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
from draft_mana_bodies import BODIES

WRITE='2662180';DRAFT='26623d0';LIFE='268fdf0';ARTIFACT='268fe30';MANA='268ff90'
ADDRESSES=[WRITE,DRAFT,LIFE,ARTIFACT,MANA];PRIVATE={WRITE,LIFE};CARDS={DRAFT};PUBLIC_HELPERS=[ARTIFACT,MANA]
NAMES={WRITE:'write_deck_file',DRAFT:'card_draft_2022',LIFE:'how_much_life_can_i_pay_for_mana_2022',ARTIFACT:'has_mana_multi_a_2022',MANA:'has_mana_multi_2022'}
LABEL='/* October 2022 draft orchestration and life-as-black-mana checks. */'
PRIVATE_TYPES='''#include <errno.h>
#define draft_get_path_base_2022 EXE_FN(void, 0x466020, char *)
#define DRAFT_IS_BASIC_2022(id) ((id) == CARD_ID_SWAMP || (id) == CARD_ID_ISLAND || (id) == CARD_ID_MOUNTAIN || (id) == CARD_ID_FOREST || (id) == CARD_ID_PLAINS)
'''
LITERALS={0x275df24:b'booster',0x275df2c:b'starter',0x275df34:b'\\PlayDeck\\%s.dck',0x275df45:b'w',0x275df48:b'Couldn\'t open "%s" for writing: %s',0x275df6b:b'EXPANSIONNAMES',0x275df7a:b'Menus.txt',0x275df84:b's',0x275df88:b';%s\n;Draft deck\n;\n;\n;%s\n;1\n;4th Edition\n;Drafted from %d pack%s',0x275dfc8:b'%c %s %s',0x275dfd1:b'\n\n',0x275dfd4:b'.%d\t1\t%s\n',0x275dfde:b'',0x275dfdf:b'booster pack',0x275dfec:b'starter deck',0x275dff9:b'an',0x275dffc:b'a',0x275dffe:b'How many opponents?',0x275e012:b'How many packs?',0x275e024:b'could not malloc %d bytes for picks[%d]',0x275e04c:b'Could not create %s %s %s',0x275e066:b'View prev',0x275e070:b'Draft a card',0x275e07d:b'Drafted so far',0x275e09c:b'functions/draft.c:207\nnum_in_iids < iids_size - 1',0x275e0d0:b'functions/draft.c:233\nnum_in_iids < iids_size - 1',0x275e102:b'Draft Opponent %d',0x275e114:b'could not malloc %d bytes for packs[%d]'}


def native_storage(runtime,debug,meta,root):
    arrays=[]
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/draft.c':continue
        for d in cu.iter_DIEs():
            if d.tag!='DW_TAG_variable':continue
            parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            if parent is None or value(parent,'DW_AT_low_pc') not in {int(WRITE,16),int(DRAFT,16)}:continue
            t=unqual(type_of(d))
            if t.tag!='DW_TAG_array_type':continue
            bounds=[c.attributes.get('DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'];assert len(bounds)==1
            arrays.append(dict(owner=inherited_value(parent,'DW_AT_name'),name=inherited_value(d,'DW_AT_name'),element_bytes=value(unqual(type_of(t)),'DW_AT_byte_size'),bounds=[dict(form=b.form,value=b.value) if b else None for b in bounds],location=value(d,'DW_AT_location')))
    assert len(arrays)==11
    fixed=[x for x in arrays if x['name'] in {'exps','is_booster','buf','basics','path'}]
    assert collections.Counter((x['name'],x['element_bytes'],x['bounds'][0]['value']) for x in fixed)==collections.Counter([('exps',4,5),('is_booster',4,5),('buf',1,599),('buf',1,49),('basics',4,4),('path',1,499)])
    dynamic=[x for x in arrays if x not in fixed];assert {x['name']:x['element_bytes'] for x in dynamic}==dict(picks=4,colors=4,packs=4,csvids=4,deck=8)
    assert next(x for x in dynamic if x['name']=='picks')['bounds'][0]['form']=='DW_FORM_exprloc'
    assert all(x['bounds']==[None] for x in dynamic if x['name']!='picks')
    reused=[x['location'] for x in fixed if x['name'] in {'buf','basics'}];assert len(reused)==3 and reused[0]==reused[1]==reused[2]
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
        'draft_reason_controller':('266241d','39051c7e7300'),'picks_count_plus_one':('26624f9','40'),
        'picks_pointer_stride':('2662506','c1e002'),'picks_capacity_plus_41':('2662533','8d4629'),
        'picks_int_stride':('2662544','c1e002'),'csvid_int_stride':('2662639','c1e002'),
        'pack_heap_capacity':('266268f','8d0c8504000000'),'zero_opponents_skip_rotation':('266276a','85c07ebd'),
        'rotation_loop':('26629d8','4039f08b1c82895c82fc75f4'),
        'error_vowel_unshifted_name':('2662be4','8b4410e48a00'),'error_loss_continues_malloc':('2662c6d','e89efb02008b855cfdffff890424'),
        'target_pair_stride':('2662ded','c1e003'),'basic_score_penalty':('2662f34','83e864'),
        'off_color_penalty':('2662ea6','83e932'),'matching_color_bonus':('2662f8c','83c20a'),
        'mana_source_quadratic_bonus':('2662fbc','8d4440020fafc0'),'nonlands_limit_23':('2663051','83fb16'),
        'basics_cycle_modulo':('2663125','f7ff'),'deck_minimum_40':('266312b','be28000000'),
        'clone_player_eax':('268fdf5','89c3890424'),'clone_life_limit_id':('268fe03','ba10300000'),
        'artifact_krrik_flag':('268fe45','f604851872790202'),'mana_krrik_flag':('268ffa5','f604951872790202'),
        'artifact_signed_half':('268ff2f','c1ea1f01d0d1f8'),'mana_signed_half':('2690052','c1e91f01c8'),
        'ordinary_mana_tail':('269003b','e9a02bd7fd'),'artifact_boolean':('268ff64','0f95c0')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,(n,get(int(a,16),len(bytes.fromhex(b))).hex())
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;probes=[];mana_words={};symbols={};clone_stack_reads=[]
    for a in ADDRESSES:
        code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(code,int(a,16)))
        if a==DRAFT:
            probes=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target='26fc1d0') for i in insns if i.mnemonic=='call' and i.operands[0].type==X86_OP_IMM and i.operands[0].imm==0x26fc1d0];assert len(probes)==5
        if a in {ARTIFACT,MANA}:
            writes=[]
            for i in insns:
                if i.mnemonic=='mov' and i.operands[0].type==X86_OP_MEM and i.operands[0].mem.base==i.operands[0].mem.index==0 and 0x55cf10<=i.operands[0].mem.disp<0x55cf18:
                    assert i.operands[0].size==1;writes.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),destination=f'{i.operands[0].mem.disp:x}',bytes_written=1))
            assert len(writes)==8 and {int(x['destination'],16) for x in writes}==set(range(0x55cf10,0x55cf18));mana_words[a]=writes
        if a==LIFE:
            clone_stack_reads=[i.op_str for i in insns for o in i.operands if o.type==X86_OP_MEM and i.reg_name(o.mem.base)=='ebp' and o.mem.disp>0];assert not clone_stack_reads
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert len(ss)==1 and ss[0]['storage']==(3 if a in PRIVATE else 2);symbols[a]=ss[0]
    assert '.constprop.' in symbols[LIFE]['name']
    for a,s in LITERALS.items():assert readonly_string(runtime,a)==s,(hex(a),readonly_string(runtime,a),s)
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    targets=[0x466020,0x472120,0x471eb0,0x44e050,0x402be0,0x49d510]
    return dict(arrays=arrays,stack_probe_sites=probes,mana_byte_stores=mana_words,native_symbols=symbols,life_clone_physical_parameters=['eax:player'],life_clone_flags_optimized_out=True,card_flags=dict(address='2797218',element_bytes=4,mask=2),basic_ids=[239,126,164,91,188],
        exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in targets},
        literals={f'{a:x}':dict(bytes=(s+b'\0').hex(),literal=byte_literal(s)) for a,s in LITERALS.items()},anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},
        behavior=['The draft supports zero through eight opponents and one through six packs. Cancellation retries the native preceding choice. Rotation occurs only with at least one opponent. Arrays retain five variable bounds and separate 600/50-byte and five-int scratch lifetimes.',
        'Empty-pack diagnostics test the first character of expansion names at the unshifted expansion index, but print the index plus seven. Losing the human continues native allocation/cleanup rather than returning immediately.',
        'AI picks retain random score tie behavior, fallback indices, two-color preference and land/artifact/basic exceptions. Opponent construction keeps signed base scores, basic/off-color penalties, quadratic mana-source bonus, twenty-three nonlands and forty-card basic fill.',
        'The writer preserves the 500-byte path, exact deck metadata/pack punctuation/card records and terminal fopen/malloc failures. No files are written by recovery verification.',
        'The life clone has one physical register argument; DWARF flags is optimized out. Both mana checks retain Krrik flag bit two, signed halving without subtracting one life, nonnegative requests and eight byte stores. Artifact splitting returns a normalized Boolean; ordinary mana checks return the raw EXE result.'])


def pending_spell_cost_bindings(runtime,meta,manifest,root):
    """Track exact deferred edges without silently migrating older caller bodies."""
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;result=[]
    aliases={int(ARTIFACT,16):('has_mana_multi_a',NAMES[ARTIFACT]),int(MANA,16):('has_mana_multi',NAMES[MANA])}
    for f in manifest['accepted_functions']:
        if not f.get('spell_cost_evidence'):continue
        a=f['address'];body=function_body(read(root/f['source']),f['name']);code=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));sites=[]
        for i in decoder.disasm(code,int(a,16)):
            if i.mnemonic not in {'call','jmp'} or i.operands[0].type!=X86_OP_IMM or i.operands[0].imm not in aliases:continue
            target=i.operands[0].imm;old,new=aliases[target]
            assert re.search(r'\b'+old+r'\s*\(',body) and not re.search(r'\b'+new+r'\s*\(',body)
            sites.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{target:x}',current_source_callee=old,recovered_native_callee=new))
        if sites:result.append(dict(address=a,name=f['name'],source=f['source'],body_sha256=sha(body.encode()),sites=sites))
    return result


def reconstruct(a,raw,runtime,meta):
    body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=strip_comments(raw),replacement=body,occurrences=1,kind='restore dynamic draft arrays/ownership, AI pick and deck scoring, exact deck output and typed failure frames; remove the optimized-out clone argument and recover Krrik-aware byte-sized mana checks',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('draft_mana_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_helpers']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta,root)
    assert component['pending_spell_cost_mana_bindings']==pending_spell_cost_bindings(runtime,meta,manifest,root)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['draft_mana_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-draft-mana' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    assert PRIVATE_TYPES in sources['functions/draft.c'].replace('\r\n','\n') and component['private_declarations']==PRIVATE_TYPES
    ps=component['parameters'][LIFE];assert [p['name'] for p in ps]==['player','flags'] and ps[0]['location'][0]['expression']=='(DW_OP_reg0 (r0))' and ps[1]['location'] is None
    assert '(int player)' in entries[LIFE]['prototype']
    header=read(root/'manalink.h') if not component.get('planning') else component['planned_header']
    for a in PUBLIC_HELPERS+[DRAFT]:assert entries[a]['prototype'] in header


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta,root);print('Prepared one modified draft handler, its deck writer, the private life clone and both modified mana checks.');return
    assert 'draft_mana_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[('cards/draft.c','card_draft')]+[('functions/functions.c',n) for n in ['has_mana_multi','has_mana_multi_a','how_much_life_can_i_pay','life_total_can_change']]+[('functions/draft.c','pick_card')]+[('functions/packs.c',n) for n in ['pack_size','make_pack_2022','choose_packs']]+[('functions/tutors.c','show_deck_force_expansion_2022'),('functions/windows.c','localtime_date')]
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta,root),pending_spell_cost_mana_bindings=pending_spell_cost_bindings(runtime,meta,m,root),**debug_evidence(debug,meta,ADDRESSES),functions={},private_declarations=PRIVATE_TYPES,
        retained_helpers=[dict(name=n,source=s,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in retained],
        scope='Recover the native 2022 draft handler and deck writer in functions/draft.c, and the private life-payment clone plus both modified multi-color mana checks in functions/functions.c. Preserve five VLA objects, all fixed/scratch arrays, native draft choices, scores, card-data widths, heap ownership, exact deck output and terminal errors, register/optimized-out parameters, signed life math and byte-sized mana globals. Preserve the old cards/draft.c handler, old mana checks and eleven retained providers. Downstream caller/dispatch/gameplay migration remains separate work.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={'functions/draft.c':read(root/'functions/draft.c')+'\n\n#ifndef SHANDALAR\n'+PRIVATE_TYPES+'\n#endif\n'};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=NAMES[a];raw=read(cache/'decompiled-draft-mana'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=3 if a in PRIVATE else 2)
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],draft_mana_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in PUBLIC_HELPERS+[DRAFT])+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'draft-mana-check',sources,header)
    proof['planning']=True;proof['planned_header']=header;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning'];del proof['planned_header']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-draft-mana/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-draft-mana'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=5;m['card_handler_functions']+=1;m['helper_functions']+=4;m['shared_helper_interfaces']+=2;m['private_functions']+=2;m['private_helpers']+=2
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['draft_mana_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'draft-mana-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=1,new_helpers=4)))


if __name__=='__main__':main()
