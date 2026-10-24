"""Recover Havengul Lich and specialized helpers plus commander/viewer/mana utility boundaries."""
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
from havengul_utility_bodies import BODIES
from recover_exile_return_cards import arguments

FILTER='210a700';CAST='210b160';FIND='210b2a0';LEGACY='210b320';STOLEN='210b650';CARD='2114ea0';COMMANDER='22c6540';VIEW='2650ca0';MANA='266d3f0'
ADDRESSES=[FILTER,CAST,FIND,LEGACY,STOLEN,CARD,COMMANDER,VIEW,MANA];CARDS={CARD};PRIVATE={FILTER,CAST,FIND,LEGACY,STOLEN};HELPERS=set(ADDRESSES)-CARDS;SUPPORT=[];PUBLIC_HELPERS=[COMMANDER,VIEW,MANA]
NAMES={FILTER:'can_cast_this_havengul_2022',CAST:'can_cast_creature_havengul_2022',FIND:'find_attached_havengul_legacy_2022',LEGACY:'spell_has_flashback_legacy_for_hl',STOLEN:'can_use_stolen_havengul_2022',CARD:'card_havengul_lich_2022',COMMANDER:'is_commander_in_command_zone_2022',VIEW:'show_deck_force_expansion_2022',MANA:'mana_producer_fixed_2022'}
LABEL='/* October 2022 commander, expansion-viewer and fixed-mana helpers. */'
CLONES='210b160\tcan_cast_creature_in_grave_with_havengul_lich\tint\t__regparm2\tint:player\tint:card\n210b2a0\tfind_attached_legacy_effect\tint\t__regparm3\tint:player\tint:card\tint:target_player\tint:target_card\n'


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
    assert arrays['card_havengul_lich::act_array']['bounds']==[7] and arrays['card_havengul_lich::act_array']['element_bytes']==4
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
      'stolen_card_only_first_slot':('210b67c','8d9880000000'),
      'stolen_card_only_end':('210b682','0510010000'),
      'main_target_pair_first_slot':('2114f39','8d707c'),
      'main_target_pair_end':('2114f3c','050c010000'),
      'main_ai_filter_mode':('2114eef','66c785f0feffff0101'),
      'main_borrowed_iid_tag':('211507c','0d00000080'),
      'main_borrowed_iid_mask':('2115418','25ffffff0f'),
      'legacy_inherit_abilities_mask':('210b5ae','f704d54c707e0001100000'),
      'legacy_slot_limit':('210b5d0','83fa13'),
      'viewer_preparation_exe_call':('2650ca6','e8b510e2fd'),
      'viewer_exe_call':('2650cdc','e87f86e2fd'),
      'viewer_reset_expansion':('2650ce1','c7056441770200000000'),
      'fixed_mana_exe_tail':('266d41d','e99ee2e0fd')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,(n,get(int(a,16),len(bytes.fromhex(b))).hex())
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;literals={};native_symbols={};dynamic={}
    for a in ADDRESSES:
        code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(code,int(a,16)))
        dynamic[a]=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),operand=i.op_str) for i in insns if i.mnemonic=='call' and i.operands[0].type!=X86_OP_IMM]
        for i in insns:
            for op in i.operands:
                if op.type!=X86_OP_IMM:continue
                literal=readonly_string(runtime,op.imm)
                if literal is not None:literals[f'{op.imm:x}']=dict(bytes=(literal+b'\0').hex(),literal=byte_literal(literal))
        syms=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(syms)==1 and syms[0]['storage']==(3 if a in PRIVATE else 2);native_symbols[a]=syms[0]
    assert len(dynamic[CARD])==4 and len(dynamic[STOLEN])==1
    return dict(arrays=arrays,filter_bytes=216,target_bytes=84,literals=literals,native_symbols=native_symbols,dynamic_dispatch=dynamic,
      anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},
      exe_boundaries=dict(viewer=dict(address='479360',arguments=6),prepare_viewer=dict(address='471d60',arguments=0),fixed_mana=dict(address='47b6c0',arguments=4,original_linker_binding_retained=True)),
      retained_native_behavior=[
        'The creature-query specialization has two live registers and tests both graveyards; its logical t_player parameter is removed. The attached-legacy specialization fixes its callback and retains EAX/EDX/ECX plus one stack argument.',
        'The selector predicate uses the callback player directly, rather than reading the old targets[0].player. Grave positions are relocated by source ID before permission activation.',
        'The main handler scans both words of targets 1..18 into an eight-IID act_array without adding a capacity guard. The can-use helper scans only card words in those same eighteen slots.',
        'Borrowed IID state uses tag 0x80000000, excludes exactly that sentinel and resolves through mask 0x0fffffff. Return values and native reads after external calls are retained.',
        'The flashback legacy snapshots state before destroying itself, relocates the grave entry, keeps the returned-owner/control and callback storage behavior, and does not add a missing-position or played-card guard.',
        'The commander helper accepts nonnegative state and tests bits 0/8. The expansion viewer resets the global to zero instead of restoring its old value. The fixed-mana wrapper retains land color/transmutation/override precedence and falls through to the existing four-argument EXE binding.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore specialized physical interfaces, 216-byte filters, fixed borrowed-ability storage, grave-source/legacy state, full callback dispatch and typed viewer/mana EXE boundaries',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('havengul_utility_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    assert component['unit_layouts']==native_layouts(debug,{'cards/dark_ascension.c'})
    for old in component['retained_helpers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['havengul_utility_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-havengul-utility-helpers' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    for old in component['retained_original_handlers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    ps=component['parameters']
    for a in [CAST,STOLEN]:assert [p['location'][0]['expression'] for p in ps[a][:2]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))']
    assert ps[CAST][2]['location'] is None
    fp={p['name']:p for p in ps[FIND]};assert fp['func_ptr']['location']=='(DW_OP_addr: 210a600; DW_OP_stack_value)'
    assert fp['target_player_id']['location'][0]['expression']=='(DW_OP_reg1 (r1))' and fp['target_card']['location']=='(DW_OP_fbreg: 0)'
    for a in PUBLIC_HELPERS:assert entries[a]['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert '_mana_producer_fixed = 0x47B6C0;' in read(root/'manalink.lds')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta);(cache/'havengul-utility-clones.tsv').write_text(CLONES);(cache/'export-request-havengul-utility-helpers.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared Havengul and three utility helpers; apply the two physical clone interfaces before export.');return
    assert 'havengul_utility_helpers_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta),unit_layouts=native_layouts(debug,{'cards/dark_ascension.c'}),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=n,source=source,body_sha256=sha(function_body(read(root/source),n).encode())) for source,n in [('cards/dark_ascension.c','havengul_lich_legacy'),('cards/dark_ascension.c','can_cast_creature_in_grave_with_havengul_lich'),('cards/dark_ascension.c','can_use_stolen_ability_with_havengul_lich'),('cards/dark_ascension.c','can_cast_this')]],
        retained_original_handlers=[dict(name='card_havengul_lich',source='cards/dark_ascension.c',body_sha256=sha(function_body(read(root/'cards/dark_ascension.c'),'card_havengul_lich').encode()))],
        scope='Recover Havengul Lich with five permission/selection/borrowed-ability helpers, plus commander-state, expansion-viewer and fixed-mana wrappers. Keep the original Havengul handler and helpers beside qualified 2022 versions. Restore physical compiler-specialized parameters, grave-source identity, both 216-byte filters, eight-IID storage with eighteen target pairs, all dynamic dispatch and native payment/control/state/cleanup behavior. Keep the existing EXE mana/viewer boundaries and explicit unchecked-storage/native fallback quirks; downstream dispatch/gameplay integration remains separate.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});targets.update({0x479360:dict(name='show_deck_exe',source='',static=False),0x471d60:dict(name='call_sub_437E20_unless_ai_is_speculating',source='',static=False),0x47b6c0:dict(name='mana_producer_fixed',source='',static=False)});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-havengul-utility-helpers'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],havengul_utility_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(next(e['prototype'] for e in entries if e['address']==a) for a in PUBLIC_HELPERS)+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'havengul-utility-helpers-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-havengul-utility-helpers/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-havengul-utility-helpers'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=9;m['card_handler_functions']+=1;m['helper_functions']+=8;m['private_functions']+=5;m['private_helpers']+=5;m['shared_helper_interfaces']+=3
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['havengul_utility_helpers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'havengul-utility-helpers-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=1,new_helpers=8)))


if __name__=='__main__':main()
