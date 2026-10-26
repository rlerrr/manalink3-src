"""Recover engine state, extension subtype reset, legacy and mana helpers."""
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
from engine_state_bodies import BODIES

LANDS='26a8700';PHASE='26a8bb0';ASTERISK='26a8c00';LEGACY='26a9140';FROM_IID='26a9320';FORMAT='26a9520';AUTOTAP='26a98c0';RECALC='26a9e40';RESET='2672860'
ADDRESSES=[LANDS,PHASE,ASTERISK,LEGACY,FROM_IID,FORMAT,AUTOTAP,RECALC,RESET];PRIVATE=set();PUBLIC_HELPERS=ADDRESSES
NAMES={LANDS:'count_colors_of_lands_in_play_2022',PHASE:'phase_changed_2022',ASTERISK:'effect_asterisk_2022',LEGACY:'create_legacy_effect_exe_2022',FROM_IID:'create_legacy_effect_from_iid_2022',FORMAT:'format_manacost_into_global_allpurpose_buffer_2022',AUTOTAP:'autotap_mana_source_2022',RECALC:'recalculate_all_cards_in_play_2022',RESET:'reset_subtypes_from_instance_2022'}
LABEL='/* October 2022 engine state, legacy construction and mana formatting/autotapping. */'
DECLARATION='int override_xx;'
GLOBAL=dict(source='functions/engine.c',name='override_xx',declaration=DECLARATION,target_address='2775a80',linkage='public',native_bytes=4,zero_initialized=True)
EXE_TARGETS=[0x4377e0,0x4a09a0,0x44e050,0x436980,0x42f1a0,0x436740,0x436930,0x4359b0,0x47d2f0]


def native_storage(runtime,debug,meta,root):
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
      'format_intro_clear':('26a952c','c685c8feffff00'),
      'format_str_clear':('26a9575','c68564feffff00'),
      'format_symbol_word':('26a95c1','66c78561feffff7c42'),
      'format_symbol_terminator':('26a95ca','c68563feffff00'),
      'format_stack_event_byte':('26a9555','8a148532965000'),
      'format_override_load':('26a969b','a1805a7702'),
      'format_override_after_cost_probe':('26a9809','8b1d805a7702'),
      'format_double_x_divisor':('26a9732','b902000000'),
      'format_triple_x_divisor':('26a9838','b903000000'),
      'format_signed_divide_max':('26a9862','f7f9'),
      'format_signed_divide_x':('26a9879','f7f9'),
      'autotap_snapshot_eight':('26a9976','83f808'),
      'autotap_restore_eight':('26a9a5b','83f808'),
      'autotap_stack_guard':('26a997b','833da85750001f'),
      'autotap_private_push':('26a9a8d','e8aeeaffff'),
      'autotap_new_tap_test':('26a9a01','f6460810'),
      'autotap_tap_event':('26a9ae3','c744240881000000'),
      'autotap_tap_trigger':('26a9b12','c7442404d2000000'),
      'autotap_mandatory':('26a9b02','c744240c01000000'),
      'recalc_player_mask0':('26a9e62','250f00c0f0'),
      'recalc_player_mask1':('26a9e7b','250f00c0f0'),
      'recalc_subtype_fifteen':('26a9ef7','83f80f'),
      'recalc_out_of_play_reset':('26a9f2f','e82c89fcff'),
      'recalc_instance_stride':('26a9f35','81c32c010000'),
      'recalc_in_play_mask':('26a9f53','2502008000'),
      'recalc_change_type_event':('26a9f7d','c74424083c000000'),
      'recalc_set_color_event':('26a9faa','c744240836000000'),
      'recalc_abilities_event':('26a9fc6','c744240834000000'),
      'recalc_land_count':('26a9ff2','e809e7ffff'),
      'recalc_mana_count':('26a9ff9','e832fbffff'),
      'recalc_static_event':('26aa05d','c744240800470000'),
      'legacy_parent_source_player':('26a9210','8b830c010000'),
      'reset_types_read':('26728a9','668bbc4684000000'),
      'reset_filter_shift':('2672892','d3e7'),
      'reset_filter_mask':('2672894','81e75f030000'),
      'reset_main_seven':('26728a4','83f807'),
      'reset_tail4':('26728d6','c7421600000000'),
      'reset_tail8':('26728dd','c7421a00000000'),
      'reset_tail10':('26728e4','66c7421e0000')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,(n,get(int(a,16),len(bytes.fromhex(b))).hex())
    arrays={};pointers={};global_type=None;types={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/engine.c','functions/has_creature_type.c'}:continue
        for d in cu.iter_DIEs():
            n=inherited_value(d,'DW_AT_name')
            if d.tag=='DW_TAG_variable' and n=='override_xx' and value(d,'DW_AT_location') is not None:
                t=unqual(type_of(d));assert value(t,'DW_AT_byte_size')==4 and value(d,'DW_AT_external')
                loc=value(d,'DW_AT_location');assert loc==[3,128,90,119,2];global_type=dict(bytes=4,location=loc,external=True)
            if d.tag=='DW_TAG_typedef' and n in {'card_ext_t','card_ptr_t'}:
                t=unqual(type_of(d));members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member'}
                if n=='card_ext_t':
                    field=next(c for c in t.iter_children() if value(c,'DW_AT_name')=='subtype');arr=unqual(type_of(field));types[n]=dict(bytes=value(t,'DW_AT_byte_size'),subtype_offset=members['subtype'],element_bytes=value(unqual(type_of(arr)),'DW_AT_byte_size'),upper_bounds=[value(c,'DW_AT_upper_bound') for c in arr.iter_children() if c.tag=='DW_TAG_subrange_type'])
                else:types[n]=dict(types_offset=members['types'])
            if d.tag!='DW_TAG_variable':continue
            parent=d.get_parent()
            while parent is not None and parent.tag!='DW_TAG_subprogram':parent=parent.get_parent()
            owner=inherited_value(parent,'DW_AT_name') if parent is not None else None
            if owner not in {'autotap_mana_source','format_manacost_into_global_allpurpose_buffer'}:continue
            t=unqual(type_of(d))
            if t.tag=='DW_TAG_array_type':arrays[owner+'::'+n]=dict(element_bytes=value(unqual(type_of(t)),'DW_AT_byte_size'),bounds=[value(c,'DW_AT_upper_bound') for c in t.iter_children() if c.tag=='DW_TAG_subrange_type'])
            if str(n).startswith('old_charge_mana_addr_of_'):
                assert t.tag=='DW_TAG_pointer_type' and value(t,'DW_AT_byte_size')==4;pointers[n]=4
    assert arrays=={'autotap_mana_source::old_pay_mana':dict(element_bytes=4,bounds=[7]),'format_manacost_into_global_allpurpose_buffer::intro':dict(element_bytes=1,bounds=[299]),'format_manacost_into_global_allpurpose_buffer::str':dict(element_bytes=1,bounds=[99]),'format_manacost_into_global_allpurpose_buffer::mana_symbol':dict(element_bytes=1,bounds=[2])}
    assert len(pointers)==3 and global_type and types['card_ext_t']==dict(bytes=192,subtype_offset=8,element_bytes=2,upper_bounds=[11]) and types['card_ptr_t']['types_offset']==132
    syms=[s for s in debug.symbols if s[0]=='_override_xx'];assert len(syms)==1
    symbol=syms[0];section=debug.pe.sections[symbol[2]-1];assert symbol[4]==2 and section.Name.rstrip(b'\0')==b'.bss' and section.SizeOfRawData==0
    assert debug.pe.OPTIONAL_HEADER.ImageBase+section.VirtualAddress+symbol[1]==0x2775a80
    symbols={};literal={}
    for a in ADDRESSES:
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert len(ss)==1 and ss[0]['storage']==2;symbols[a]=ss[0]
    for a in [0x2764940,0x2764945,0x276494c,0x276494f,0x2764953,0x276495a,0x276495d]:
        data=readonly_string(runtime,a);assert data is not None;literal[f'{a:x}']=dict(bytes=(data+b'\0').hex(),literal=byte_literal(data))
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,arrays=arrays,saved_pointer_bytes=pointers,types=types,override_global=global_type,exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},literals=literal,anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},
        behavior=['Land-color counts include per-player added mana colors but retain separate basic-land subtype counts, Prismatic Omen/Contamination behavior, all eight table columns and the land-play flag update.',
        'Recalculation keeps the exact native player mask, all recovered card/dice/subtype resets and their existing fragments, rebuilds extension subtypes for non-battlefield instances and retains change-type/color/ability then mana, toughness/power/counter/static ordering and hand-visibility update.',
        'Subtype reset copies seven signed database type words, clamps negatives, filters the native 0x1000..0x1009 bit set and 0x2005, then clears five more extension subtype slots. It is a different two-argument API from the older mode-based reset.',
        'The 300-byte intro, 100-byte mana string and three-byte symbol are distinct. Explicit nonzero override_xx wins over both hack_xx and double/triple-X text detection; signed division and native generic/colored formatting remain exact.',
        'Autotap snapshots eight request words, X/max-X and three four-byte pointers; retains the capacity guard, conditional mandatory tap trigger, unconditional successful tap sound/resolution and complete rollback/restore. Legacy frontends retain all five EXE arguments, source remapping, activation/effect/face-down IDs and signed/word image fields.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore typed engine recalculation, subtype reconstruction, legacy frames and mana formatting/autotap state',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('engine_state_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_helpers']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta,root)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['engine_state_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-engine-state' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    for old in component['retained_original_handlers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for a in PUBLIC_HELPERS:assert entries[a]['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert component['override_global']==GLOBAL and (GLOBAL in manifest['source_globals'] or component.get('planning'))
    assert DECLARATION in sources['functions/engine.c']
    assert component['retained_recalc_fragments']==[f for f in manifest['source_fragments'] if f['address']==RECALC]



def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta,root);(cache/'export-request-engine-state.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared nine public engine, subtype, legacy and mana helpers.');return
    assert 'engine_state_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[('functions/engine.c',meta[a]['name']) for a in ADDRESSES if a!=RESET]+[('functions/has_creature_type.c','reset_subtypes')]
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta,root),**debug_evidence(debug,meta,ADDRESSES),functions={},override_global=GLOBAL,retained_recalc_fragments=[f for f in m['source_fragments'] if f['address']==RECALC],
        retained_helpers=[dict(name=n,source=source,body_sha256=sha(function_body(read(root/source),n).encode())) for source,n in retained],retained_original_handlers=[],
        scope='Recover nine public engine/state, legacy, formatting/autotap and extension subtype-reset helpers in their original units, plus the four-byte override_xx global. Preserve all existing source bodies, accepted records and four recalculation fragments. Verify exact masks, field/array/pointer storage, formatting precedence, native EXE frames, subtype filter and full recalculation/activation behavior. Downstream caller/dispatch/gameplay integration remains work.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={'functions/engine.c':read(root/'functions/engine.c')+'\n\n#ifndef SHANDALAR\n/* October 2022 public X-format override; native .bss @ 0x2775a80. */\n'+DECLARATION+'\n#endif\n'};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-engine-state'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],engine_state_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\nextern int override_xx;\n'+'\n'.join(next(e['prototype'] for e in entries if e['address']==a) for a in PUBLIC_HELPERS)+'\n#endif\n\n'+header[at:]
    # Match check_source.py's existing MSVC-host compatibility alias, only in
    # the disposable syntax-check copy. GCC's original source stays unchanged.
    syntax_sources={s:('#define strncasecmp _strnicmp\n'+t if s=='functions/rules_engine.c' else t) for s,t in sources.items()}
    compile_sources(root,cache/'engine-state-check',syntax_sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-engine-state/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:
            if row['address']==RECALC:
                assert read(cache/'decompiled-engine-state'/f'{RECALC}.c')==read(cache/'decompiled-abi'/f'{RECALC}.c')
                continue
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-engine-state'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=9;m['helper_functions']+=9;m['shared_helper_interfaces']+=9;m['source_globals'].append(GLOBAL)
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['engine_state_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'engine-state-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=9)))


if __name__=='__main__':main()
