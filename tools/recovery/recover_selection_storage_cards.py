"""Recover six selection/storage handlers while retaining the deferred chooser boundary."""
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
from selection_storage_card_bodies import BODIES

ORMOS='2374a00';PELT='261f430';BLAZE='24bf650';DISPOSSESS='22dbd60';HOT='25c8380';MISE='2304e30'
ADDRESSES=[ORMOS,PELT,BLAZE,DISPOSSESS,HOT,MISE];PRIVATE=set();CARDS=set(ADDRESSES)
SUPPORT=['26ef720','26ef820']


def native_storage(runtime,debug,meta):
    arrays={};units={meta[a]['source'] for a in ADDRESSES};owners={meta[a]['name'] for a in ADDRESSES}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in units:continue
        for d in cu.iter_DIEs():
            n=value(d,'DW_AT_name')
            if d.tag!='DW_TAG_variable':continue
            parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            owner=value(parent,'DW_AT_name') if parent is not None else None
            if owner not in owners:continue
            t=unqual(type_of(d))
            if t.tag=='DW_TAG_array_type':arrays[owner+'::'+n]=dict(bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'],element_bytes=value(unqual(type_of(t)),'DW_AT_byte_size'),location=value(d,'DW_AT_location'))
    assert arrays['card_ormos_archive_keeper::searched']['bounds']==[1,2]
    for name in ['card_ormos_archive_keeper::hand_copy','card_pelt_collector::array']:assert arrays[name]['element_bytes']==4
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
        'ormos_vla_row_bytes':('2374cd2','8d0c8500000000'),
        'ormos_vla_total_bytes':('2374cd9','c1e003'),
        'ormos_vla_probe':('2374ce2','e8e9743800'),
        'pelt_vla_bytes':('261f50c','8d04b500000000'),
        'pelt_pending_low_byte':('261f7eb','fe86cc000000'),
        'blaze_phase_signed_limit':('24bf768','833d84f14e0016'),
        'blaze_legacy_exe_global':('24bf7b9','a168837200'),
        'hot_fix_elapsed_signed_limit':('25c844a','3d10270000'),
        'hot_fix_send_packet_type':('25c846d','c74424041c000000'),
        'hot_fix_send_direction':('25c8475','c7042400000000'),
        'hot_fix_receive_packet_type':('25c84d9','c74424041c000000'),
        'hot_fix_receive_direction':('25c84e1','c7042401000000'),
        'dispossess_artifact_type':('22dbe93','c744240840000000'),
        'dispossess_all_colors':('22dbe9b','c7442404ffffffff'),
        'dispossess_dll_chooser_call':('22dbeaa','e871384100'),
        'mise_typed_chooser_call':('2304eff','e81ca93e00')}
    for name,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,(name,get(int(a,16),len(bytes.fromhex(b))).hex())
    assert get(0x2721180,24)==b'\xff'*24
    wrappers={
        '26ef720':'5589e583ec108b4510c744240cff000000890424c74424083f000000c7442404000000008b4d0c8b550831c0e86ffbffffc9c3',
        '26ef820':'5589e583ec108b4514c744240cff000000890424c74424083f000000c7442404000000008b4d108b55088b450ce86efaffffc9c3'}
    for a,b in wrappers.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b
    literal_values={0x273d980:b'Choose an artifact card name.',0x273d9a4:b'\x01\x0c',0x273d9a1:b'\x01\x03',0x273d99e:b'\x01\x11',0x273f201:b'Choose a nonland card name.'}
    for a,b in literal_values.items():assert readonly_string(runtime,a)==b
    return dict(arrays=arrays,filter_bytes=216,target_bytes=84,
        anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},
        ormos_initializer=dict(address='2721180',bytes=get(0x2721180,24).hex(),rows=2,columns=3),
        pelt_storage=dict(pending_byte_offset=204,resolved_byte_offset=208,first_power_slots=10,second_power_slots=11,cleared_target_pairs=9),
        chooser_wrapper_equivalence=dict(wrappers=wrappers,implementation='26ef2c0',registers=['EAX: constraints','EDX: prompt','ECX: colors'],stack_words=['types',0,63,255],delegation='choose_a_card(prompt, colors, types) == choose_a_card_test_ct(prompt, NULL, colors, types)',ui_implementation_recovered=False),
        literals={f'{a:x}':dict(bytes=(b+b'\0').hex(),literal=byte_literal(b)) for a,b in literal_values.items()},
        retained_native_behavior=[
          'Ormos stores the first distinct IID in searched[0][1], compares selected zone indices with saved hand positions, and pays only after three selections.',
          'Pelt Collector increments only the pending low byte before finding a free power slot, reads the resolved low byte, sorts its native VLA and clears only nine target pairs; no capacity guard is added.',
          'Hot Fix preserves the AI/speculation branches, wrapped 32-bit clock subtraction, packet 28 directions and EXE payload/tag globals.',
          'Blaze of Glory uses the signed phase limit and runtime EXE legacy global, then retains generic-spell dispatch after destroy.',
          'Dispossess retains the unchecked cards_data[-1] fallback and all fourteen dialog words. The DLL chooser delegates to the existing typed stub with a NULL filter.',
          'Mise snapshots the top IID before choosing, preserves AI fallback 0x2ee2 and unconditional use of the chooser return. Mise and Dispossess await deferred interactive chooser integration.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore typed fixed/VLA storage, packed target slots, native event/order/clock/network behavior and verified deferred chooser delegation',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('selection_storage_card_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    assert component['unit_layouts']==native_layouts(debug,{e['source'] for e in entries.values()})
    for old in component['retained_helpers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['selection_storage_card_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-selection-storage-cards' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    for a in [MISE,DISPOSSESS]:assert entries[a]['integration_dependency']==component['integration_dependency']
    support=entries[DISPOSSESS]['supporting_exports'];assert len(support)==1 and support[0]['address']=='26ef720'
    assert sha(read(root/'.recovery'/support[0]['raw_export']).encode())==support[0]['raw_sha256']
    assert manifest['source_stubs']==component['retained_source_stubs']


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta);(cache/'export-request-selection-storage-cards.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES+SUPPORT));print('Prepared six cards and two supporting chooser wrapper exports.');return
    assert 'selection_storage_cards_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta),unit_layouts=native_layouts(debug,{meta[a]['source'] for a in ADDRESSES}),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=f['name'],source=f['source'],body_sha256=sha(function_body(read(root/f['source']),f['name']).encode())) for f in m['accepted_functions'] if f['name']=='test_not_in_array'],
        retained_source_stubs=m['source_stubs'],
        integration_dependency='Interactive named-card selection awaits the existing choose_a_card_test_ct stub, which returns -1. Native unconditional uses are retained; interactive gameplay is not verified.',
        scope='Recover Ormos, Pelt Collector, Blaze of Glory, Dispossess, Hot Fix and Mise in their original set files. Restore native fixed and variable arrays, split target power slots and low-byte counters, signed phase/clock rules, packet-specific EXE network frames and the runtime legacy-effect global. Verify the DLL chooser wrapper is the NULL-filter specialization of the existing typed boundary without recovering deferred dialog internals. Preserve native event, ordering, fallback and unchecked-bound quirks; gameplay integration remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=meta[a]['name'],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=row['name'];raw=read(cache/'decompiled-selection-storage-cards'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],selection_storage_card_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    for e in entries:
        if e['address'] in [DISPOSSESS,MISE]:e['integration_dependency']=proof['integration_dependency']
        if e['address']==DISPOSSESS:e['supporting_exports']=[dict(address='26ef720',name='choose_a_card',source='functions/choose_a_card.cpp',raw_export='decompiled-selection-storage-cards/26ef720.c',raw_sha256=sha(read(cache/'decompiled-selection-storage-cards/26ef720.c').encode()),purpose='Native thin wrapper proves NULL-filter delegation to the existing typed chooser stub; no recovered implementation is counted.')]
    compile_sources(root,cache/'selection-storage-cards-check',sources)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-selection-storage-cards/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-selection-storage-cards'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=6;m['card_handler_functions']+=6
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['selection_storage_cards_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'selection-storage-cards-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=6,new_helpers=0)))


if __name__=='__main__':main()
