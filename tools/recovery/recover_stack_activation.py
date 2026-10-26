"""Recover stack proxy copies, cast finalization and forced mana activation."""
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
from stack_activation_bodies import BODIES

WORKER='26a8540';PUSH='26ade00';RECOPY='26ade40';RESPONSE='26adc90';FINALIZE='26add20';SPELL='26adfa0';TAPPABLE='26b0480';FORCE='26b0500';TAP='26b0650'
ADDRESSES=[WORKER,PUSH,RECOPY,RESPONSE,FINALIZE,SPELL,TAPPABLE,FORCE,TAP];PRIVATE={WORKER};PUBLIC_HELPERS=[a for a in ADDRESSES if a not in PRIVATE]
NAMES={WORKER:'put_card_or_activation_onto_stack_impl_2022',PUSH:'put_card_or_activation_onto_stack_2022',RECOPY:'recopy_card_onto_stack_2022',RESPONSE:'allow_response_to_activation_2022',FINALIZE:'finalize_activation_2022',SPELL:'put_card_on_stack3_2022',TAPPABLE:'is_a_tappable_mana_source_2022',FORCE:'force_activation_for_mana_2022',TAP:'tap_card_for_mana_2022'}
LABEL='/* October 2022 stack proxies, cast finalization and mana activation. */'
EXE_TARGETS=[0x439690,0x436a20,0x436740,0x435c80,0x435cd0,0x477070,0x477a90,0x471d70,0x4d5d32,0x472260,0x4359b0,0x436930,0x4e81fe]
ENUMS={'STATE_BEING_CAST':0x80000000,'ENABLE_XTRIGGER_REPLACE_ETB':0x80,'XTRIGGER_REPLACE_ETB':0x1000e00,'SF_NOT_CAST':0x40000,'EVENT_SPELL_CAST':0x6200,'EVENT_LAND_PLAYED':0x6300,'EVENT_THIS_SPELL_CAST':0x6400,'QUERY_STORM':4,'QUERY_GRAVESTORM':5,'WAV_SNOW':84}


def native_storage(runtime,debug,meta,root):
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    anchors={
      'worker_stack_record_event':('26a8572','88148532965000'),
      'worker_stack_record_aux':('26a857c','88148533965000'),
      'worker_stack_record_iid':('26a8589','66890c8530965000'),
      'worker_extension_copy':('26a8664','e8a7a30000'),
      'worker_signed_target_player':('26a85b3','0fbe5350'),
      'wrapper_capacity':('26ade05','833da85750001f'),
      'wrapper_worker_tail':('26ade31','e90aa7ffff'),
      'recopy_extension_copy':('26adf02','e8094b0000'),
      'recopy_no_recopy_mask':('26adecc','f7c100000020'),
      'recopy_damage_id_last':('26adf53','83c22e'),
      'recopy_image_word':('26adf64','66894364'),
      'spell_being_cast_set':('26adfc6','81ca00000080'),
      'spell_not_cast_mask':('26adff2','c744240800000400'),
      'spell_replicate_byte':('26ae443','8a909f000000'),
      'spell_storm_query':('26ae44c','c744240c04000000'),
      'spell_gravestorm_query':('26ae47b','c744240c05000000'),
      'spell_copy_loop':('26ae4b3','e818a2fbff'),
      'spell_cast_event':('26ae4c0','c744240800620000'),
      'spell_land_event':('26ae270','c744240800630000'),
      'spell_this_cast_event':('26ae01e','c744240800640000'),
      'spell_cast_trigger':('26ae04f','c7442404d3000000'),
      'spell_being_cast_clear':('26ae183','25ffffff7f'),
      'spell_replace_flag':('26ae188','f6058052790280'),
      'spell_replace_trigger':('26ae328','c7442404000e0001'),
      'spell_snow_subtype':('26ae164','c744240803200000'),
      'spell_snow_sound':('26ae3e0','c7042454000000'),
      'spell_fizzle_sleep':('26ae362','c70424d0070000'),
      'spell_fizzle_stdcall_compensation':('26ae370','52'),
      'force_stack_capacity':('26b0509','833da85750001f'),
      'force_worker_call':('26b052f','e80c80ffff'),
      'force_tapped_event':('26b05e0','c744240881000000'),
      'force_ability_event':('26b05f4','c7442408000c0000'),
      'force_tap_trigger':('26b0620','c7442404d2000000'),
      'force_tap_trigger_mandatory':('26b0610','c744240c01000000'),
      'force_play_ability_event':('26b0594','c744240891000000'),
      'tap_colorless':('26b0659','c744240801000000')}
    for n,(a,b) in anchors.items():assert get(int(a,16),len(bytes.fromhex(b))).hex()==b,n
    enums={};types={};field=None
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/engine.c':continue
        for d in cu.iter_DIEs():
            n=value(d,'DW_AT_name')
            if d.tag=='DW_TAG_enumeration_type':
                for c in d.iter_children():
                    key=value(c,'DW_AT_name')
                    if key in ENUMS:enums[key]=value(c,'DW_AT_const_value')
            if d.tag=='DW_TAG_typedef' and n in {'stack_data_t','card_ext_t'}:
                t=unqual(type_of(d));types[n]=dict(bytes=value(t,'DW_AT_byte_size'),members={value(c,'DW_AT_name'):value(c,'DW_AT_data_member_location') for c in t.iter_children() if c.tag=='DW_TAG_member'})
                if n=='card_ext_t':
                    f=next(c for c in t.iter_children() if value(c,'DW_AT_name')=='times_replicate_paid');field=dict(offset=value(f,'DW_AT_data_member_location'),bytes=value(unqual(type_of(f)),'DW_AT_byte_size'))
    assert enums==ENUMS and types['stack_data_t']==dict(bytes=4,members=dict(internal_card_id=0,generating_event=2,unknown_sdt=3))
    assert types['card_ext_t']['bytes']==192 and field==dict(offset=159,bytes=1)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;symbols={};literal={};boundaries={}
    for a in ADDRESSES:
        code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(code,int(a,16)))
        ss=[dict(name=n,storage=st) for n,v,sec,k,st in debug.functions() if sec>0 and k==32 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert len(ss)==1 and ss[0]['storage']==(3 if a in PRIVATE else 2);symbols[a]=ss[0]
        boundaries[a]=[]
        for i in insns:
            for o in i.operands:
                if o.type!=X86_OP_IMM:continue
                if o.imm in EXE_TARGETS:boundaries[a].append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{o.imm:x}'))
                if o.imm in {0x2764a04,0x2764a1b,0x27649a7}:
                    data=readonly_string(runtime,o.imm);assert data is not None;literal[f'{o.imm:x}']=dict(bytes=(data+b'\0').hex(),literal=byte_literal(data))
    assert set(literal)=={'2764a04','2764a1b','27649a7'}
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    return dict(native_symbols=symbols,types=types,replicate_field=field,enums=enums,exe_sha256=sha(exe.data),exe_boundaries={f'{a:x}':exe.pe.get_data(a-0x400000,48).hex() for a in EXE_TARGETS},exe_operands=boundaries,literals=literal,
        worker_physical_parameters=['eax:player','edx:card','ecx:event','stack+4:a4','stack+8:a5'],anchors={n:dict(address=a,bytes=b) for n,(a,b) in anchors.items()},
        behavior=['Preserve the public 32-entry stack guard and private worker, four-byte packed stack records, signed damage-target player, activation proxy allocation failure and hand bookkeeping, saved timestamp and original/backup IDs. Copy the full instance and extension, clear dying state and preserve the X-value handoff.',
        'Recopy the instance and extension unless the parent has DONT_RECOPY; preserve parent/timestamp, damage-ID range, image word widths and signed damage target. Final activation resolves and dispatches its tap trigger in native order.',
        'Spell finalization sets BEING_CAST, skips cast/copy events for SF_NOT_CAST, uses the extension Replicate byte, lets Storm override Replicate and Gravestorm override Storm, and loops the unsigned amount without clamping. Land/spell/this-spell/trigger ordering and post-trigger counter checks are exact.',
        'Preserve sound order including Snow, clear BEING_CAST before the replacement trigger and resolution, retain native 0x80 enable bit and 0x01000e00 xtrigger independently from old header values, and retain all early returns, damage/regeneration calls, land count and fizzle cleanup.',
        'Forced mana activation keeps the capacity guard but continues activation even if full. Newly tapped cards dispatch tap and tapped-for-ability events then a mandatory player tap trigger; already-tapped/untapped cards dispatch play-ability. The tap sound, resolution and reassessment remain unconditional on the successful path.'])


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore extension-aware stack proxies, native cast/copy/replacement ordering and forced mana activation/tap behavior',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('stack_activation_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_helpers']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta,root)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['stack_activation_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-stack-activation' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    for old in component['retained_original_handlers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for a in PUBLIC_HELPERS:assert entries[a]['prototype'] in read(root/'manalink.h') or component.get('planning')
    ps=component['parameters'][WORKER];assert [p['location'][0]['expression'] for p in ps[:3]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    assert [p['name'] for p in ps]==['player','card','event','a4','a5']



def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta,root);(cache/'export-request-stack-activation.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared nine stack/activation helpers with one private worker and eight public interfaces.');return
    assert 'stack_activation_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[('functions/engine.c',n) for n in ['put_card_or_activation_onto_stack','recopy_card_onto_stack','allow_response_to_activation','finalize_activation','put_card_on_stack3','comes_into_play_trigger','is_a_tappable_mana_source','force_activation_for_mana','tap_card_for_mana']]
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta,root),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=n,source=source,body_sha256=sha(function_body(read(root/source),n).encode())) for source,n in retained],retained_original_handlers=[],
        scope='Recover nine stack/activation functions in the original engine unit, with one private part and eight qualified public APIs. Restore instance/extension copies, native stack record widths, cast-event and Storm/Gravestorm/Replicate ordering, entry-replacement trigger values, Snow sound and forced mana tap triggers. Retain all older source bodies, accepted records and EXE bindings. Downstream caller/dispatch/gameplay integration remains work.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-stack-activation'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],stack_activation_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(next(e['prototype'] for e in entries if e['address']==a) for a in PUBLIC_HELPERS)+'\n#endif\n\n'+header[at:]
    # Match check_source.py's existing MSVC-host compatibility alias, only in
    # the disposable syntax-check copy. GCC's original source stays unchanged.
    syntax_sources={s:('#define strncasecmp _strnicmp\n'+t if s=='functions/rules_engine.c' else t) for s,t in sources.items()}
    compile_sources(root,cache/'stack-activation-check',syntax_sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-stack-activation/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-stack-activation'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=9;m['helper_functions']+=9;m['shared_helper_interfaces']+=8;m['private_functions']+=1;m['private_helpers']+=1
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['stack_activation_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'stack-activation-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=9)))


if __name__=='__main__':main()
