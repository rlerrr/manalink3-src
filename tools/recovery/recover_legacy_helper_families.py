"""Recover four modified card/legacy families while preserving their 2020 entry points."""
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
from legacy_helper_family_bodies import BODIES
from recover_exile_return_cards import arguments

BRUNA='2340190';BRUNA_CARD='234ffc0';DAMAGE='24bad70';PERSONAL='24bc2e0';PERSONAL_CARD='24ca360';TOSHIRO='25686b0';TOSHIRO_CARD='25705a0';SEHTS='2571c90';SEHTS_CARD='257a9d0'
ADDRESSES=[BRUNA,BRUNA_CARD,DAMAGE,PERSONAL,PERSONAL_CARD,TOSHIRO,TOSHIRO_CARD,SEHTS,SEHTS_CARD]
CARDS={BRUNA_CARD,PERSONAL_CARD,TOSHIRO_CARD,SEHTS_CARD};PRIVATE={BRUNA,DAMAGE,TOSHIRO,SEHTS};HELPERS=set(ADDRESSES)-CARDS;SUPPORT=['233ff50','24badd0']
LABEL='/* October 2022 Personal Incarnation owner-controlled legacy. */'


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
    assert set(arrays)=={'toshiro_effect::playable','card_toshiro_umezawa::playable'}
    for a in arrays.values():assert a['bounds'][0]==1 and len(a['bounds'])==2 and a['element_bytes']==4
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
      'toshiro_effect_low_byte':('25687dc','25ff000000'),
      'toshiro_effect_vla_bytes':('25687ed','c1e003'),
      'toshiro_effect_vla_probe':('2568804','e8c7391900'),
      'toshiro_card_vla_bytes':('257073b','c1e003'),
      'toshiro_card_vla_probe':('257075c','e86fba1800'),
      'toshiro_effect_post_cast_decrement':('2568a17','4e'),
      'toshiro_card_post_cast_decrement':('25708cb','4e'),
      'toshiro_detached_marker_byte':('2568b44','c683ce00000001'),
      'personal_activated_legacy_global':('24ca674','a15c9d7300'),
      'personal_legacy_callback':('24ca6a3','c74038e0c24b02'),
      'bruna_two_live_register_arguments':('2340195','89d6'),
      'bruna_aura_subtype':('23401ea','c78548ffffff02400000'),
      'sehts_player_bit':('2571cef','0d00000004'),
      'sehts_protection_tag':('2571d15','803db705620006')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,(n,get(int(a,16),len(bytes.fromhex(b))).hex())
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;literals={};native_symbols={}
    for a in ADDRESSES:
        code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16))
        for i in decoder.disasm(code,int(a,16)):
            for op in i.operands:
                if op.type!=X86_OP_IMM:continue
                literal=readonly_string(runtime,op.imm)
                if literal is not None:literals[f'{op.imm:x}']=dict(bytes=(literal+b'\0').hex(),literal=byte_literal(literal))
        syms=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(syms)==1 and syms[0]['storage']==(3 if a in PRIVATE else 2);native_symbols[a]=syms[0]
    assert native_symbols[BRUNA]['name']=='_bruna_ability.isra.0'
    return dict(arrays=arrays,filter_bytes=216,target_bytes=84,literals=literals,native_symbols=native_symbols,
      anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},
      source_binding=dict(qualified_card_suffix='_2022',existing_2020_entry_points_retained=True,downstream_dispatch_migration=False),
      retained_native_behavior=[
        'Bruna tests the existing selector result against zero, preserving its -1 behavior, and reaches the grave/hand phases after human cancellation. Its specialized worker has only EAX/EDX live arguments; the logical event is elided.',
        'Personal Incarnation retains the positive-life death gate, inclusive active-card-count scan, ownership-transfer fake-card/EXE-legacy construction, two separate damage callbacks and repeated state/owner reads.',
        'Toshiro attaches a persistent legacy, clears attachment/status and writes byte 206 on source death. The detached effect takes only the low byte of the resolve count; the main handler uses the full result.',
        'Both Toshiro loops retain two-row four-byte VLAs, retries with no candidates or failed payment, original grave pointers, cancellation and post-cast candidate-count decrement. No empty-list exit or array guard is added.',
        'Sehts Tiger retains the player protection bit, high-byte event tag 6, turn-start expiry, cleanup slot and x86-masked color shift.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore typed target/filter lifetimes and VLA rows, physical specialized worker signature, legacy callbacks, owner/control/protection state and native lifecycle/retry behavior',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('legacy_helper_family_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    assert component['unit_layouts']==native_layouts(debug,{e['source'] for e in entries.values()})
    for old in component['retained_helpers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['legacy_helper_family_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-legacy-helper-families' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    for old in component['retained_original_handlers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    assert [p['location'][0]['expression'] for p in component['parameters'][BRUNA]][:2]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))']
    assert component['parameters'][BRUNA][2]['location'][0]['expression'].startswith('(DW_OP_GNU_parameter_ref:')
    for e in entries.values():
        for support in e.get('supporting_exports',[]):assert sha(read(root/'.recovery'/support['raw_export']).encode())==support['raw_sha256']
    assert entries[PERSONAL]['prototype'] in read(root/'manalink.h') or component.get('planning')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta);(cache/'legacy-helper-family-clones.tsv').write_text('2340190\tbruna_ability\tvoid\t__regparm2\tint:player\tint:card\n');(cache/'export-request-legacy-helper-families.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES+SUPPORT));print('Prepared four modified card/legacy families and two retained dependencies.');return
    assert 'legacy_helper_families_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta),unit_layouts=native_layouts(debug,{meta[a]['source'] for a in ADDRESSES}),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=n,source=source,body_sha256=sha(function_body(read(root/source),n).encode())) for source,n in [('cards/avacyn_restored.c','check_targets_for_bruna'),('cards/unlimited.c','is_damaging_attached_creature')]],
        retained_original_handlers=[dict(name=meta[a]['name'],source=meta[a]['source'],body_sha256=sha(function_body(read(root/meta[a]['source']),meta[a]['name']).encode())) for a in sorted(CARDS)],
        scope='Recover Bruna, Personal Incarnation, Toshiro and Sehts Tiger with five native legacy/selection helpers. Append qualified _2022 card handlers in their original set files, preserving the older public implementations and downstream dispatch. Restore Bruna physical two-register signature, distinct 84/216-byte target/filter lifetimes, Toshiro two-row VLAs and detached death marker, Personal Incarnation owner-control damage callbacks and Sehts protection state. Preserve explicit native retry, bounds, ordering and cleanup quirks; gameplay integration remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=meta[a]['name']+('_2022' if a in CARDS else ''),source=meta[a]['source'],static=a in PRIVATE or a in SUPPORT) for a in ADDRESSES+SUPPORT});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-legacy-helper-families'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],legacy_helper_family_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    for e in entries:
        support = '233ff50' if e['address']==BRUNA else '24badd0' if e['address']==PERSONAL_CARD else None
        if support:e['supporting_exports']=[dict(address=support,name=meta[support]['name'],source=meta[support]['source'],raw_export=f'decompiled-legacy-helper-families/{support}.c',raw_sha256=sha(read(cache/'decompiled-legacy-helper-families'/f'{support}.c').encode()),purpose='Existing original source body is retained and hash-pinned; native export validates the dependency without counting a new implementation.')]
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+next(e['prototype'] for e in entries if e['address']==PERSONAL)+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'legacy-helper-families-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-legacy-helper-families/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-legacy-helper-families'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=9;m['card_handler_functions']+=4;m['helper_functions']+=5;m['private_functions']+=4;m['private_helpers']+=4;m['shared_helper_interfaces']+=1
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['legacy_helper_families_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'legacy-helper-families-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=4,new_helpers=5)))


if __name__=='__main__':main()
