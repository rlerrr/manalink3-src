"""Recover fifteen native flag/target consumers using typed storage and accepted interfaces."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image,value
from export_abi import type_of
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings,readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

NIMBLE='20ceab0';HEALER='24044c0';SCOUT='249bde0';VORTEX='238cdd0';CHARM='2391ca0';TIME='2024970';SAMITE='249b3a0';HOURGLASS='23fd2e0';SHADRIX='23daf20';PLATE='25a5e60';PATH='2290a90';QUANDRIX='23d8240';GAMBIT='22f72b0';FUNGUS='22f78e0';ZONE='21cc160';CAN_ACT='2692dd0';CAN_TRIG='2693060'
HOUR_LEGACY="23ec740"
ATTACHED="25b2290"
FLASH="21c7b60"
ADDRESSES=[FLASH,ATTACHED,HOUR_LEGACY,CAN_ACT,CAN_TRIG,NIMBLE,HEALER,SCOUT,VORTEX,CHARM,TIME,SAMITE,HOURGLASS,SHADRIX,PLATE,PATH,QUANDRIX,GAMBIT,FUNGUS,ZONE]
HELPERS={CAN_ACT,CAN_TRIG,HOUR_LEGACY,ATTACHED,FLASH};PRIVATE={HOUR_LEGACY,FLASH};LEGACY=[('cards/urza_legacy.c','target_is_attached_to_creature'),('functions/functions.c','can_counter_activated_ability'),('functions/functions.c','can_counter_triggered_ability')];LABEL='/* October 2022 counter-ability eligibility and cycling state. */';ALIASES={}
GLOBAL=dict(source='functions/functions.c',name='card_is_being_cycled',declaration='int card_is_being_cycled = 0;',target_address='2775470',zero_initialized=True,linkage='public')
CHOICES={'CHOICE_INVALID_TARGET':0,'CHOICE_BOUNCE':1,'CHOICE_COUNTERSPELL':2,'CHOICE_P1_P1_COUNTERS':3,'CHOICE_RECYCLE':4}
NAMES={'effect_enable_flash_land':'effect_enable_flash_land_2022','target_is_attached_to_creature':'target_is_attached_to_creature_2022','can_counter_activated_ability':'can_counter_activated_ability_2022','can_counter_triggered_ability':'can_counter_triggered_ability_2022','phase_in_impl':'phase_in_impl_2022','phase_out_impl':'phase_out_impl_2022'}
FIXED={FLASH:{'21c7b7c':'83e872','21c7b7f':'83f801','21c7b82':'760a'},NIMBLE:{'20ceac2':'8d874fffffff','20ceac8':'83f802','20ceacb':'7623'},HEALER:{'24044da':'8d478e','24044dd':'83f801','24044e0':'7616','2404be4':'8d5590','2404be7':'891424','2404bea':'8944240c','2404bee':'c74424080d8a7402','2404bf6':'c744240464000000','2404bfe':'e8ad96feff'},
 PLATE:{'25a5e7c':'8d70ce','25a5e7f':'83fe02','25a5e82':'0f8708010000','25a61e8':'89d8','25a61ec':'e88fc5ffff'},
 VORTEX:{'238ce36':'c7042400000000','238ce3d':'b810904900','238ce42':'ffd0'},HOURGLASS:{'23fd670':'a108687300','23fd678':'a308687300'},
 SHADRIX:{'23db369':'e802a12800','23db374':'c745a802000000','23db37b':'c745ac01000000','23db382':'c745b020000000','23db389':'c645b822','23db38d':'897da4','23db390':'e83ba62800'},
 HOUR_LEGACY:{'23ec9d4':'a108687300','23ec9dc':'a308687300'},SCOUT:{'249becb':'8d4601','249bee0':'8b04b7','249beea':'46','249beeb':'e890defdfd','249bef0':'3bb518ffffff','249bef6':'75e8'}}


def native_storage(debug):
    result={}
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {'cards/strixhaven.c','functions/functions.c'}:continue
        if source=='cards/strixhaven.c':
            for d in cu.iter_DIEs():
                if d.tag!='DW_TAG_typedef' or value(d,'DW_AT_name')!='CommandChoices':continue
                parent=d.get_parent().get_parent()
                if value(parent,'DW_AT_low_pc')!=int(QUANDRIX,16):continue
                t=type_of(d);members={value(x,'DW_AT_name'):value(x,'DW_AT_const_value') for x in t.iter_children()}
                assert value(t,'DW_AT_byte_size')==4 and members==CHOICES
                result['CommandChoices']=dict(bytes=4,members=members)
        for d in cu.get_top_DIE().iter_children():
            name=value(d,'DW_AT_name')
            if source=='functions/functions.c' and d.tag=='DW_TAG_variable' and name=='card_is_being_cycled' and value(d,'DW_AT_location'):
                assert value(type_of(d),'DW_AT_byte_size')==4 and value(d,'DW_AT_location')==[3,112,84,119,2]
                symbols=[x for x in debug.symbols if x[0]=='_card_is_being_cycled'];assert len(symbols)==1 and symbols[0][4]==2
                symbol=symbols[0];section=debug.pe.sections[symbol[2]-1]
                assert debug.pe.OPTIONAL_HEADER.ImageBase+section.VirtualAddress+symbol[1]==0x2775470 and section.Name.rstrip(b'\0')==b'.bss' and section.SizeOfRawData==0
                result['cycling_global']=dict(GLOBAL,dwarf_location=value(d,'DW_AT_location'),native_storage=2,bytes=4,native_section='.bss')
            if d.tag=='DW_TAG_typedef' and ((source=='cards/strixhaven.c' and name in {'token_generation_t','card_instance_t'}) or (source=='functions/functions.c' and name=='stack_data_t')):
                t=type_of(d)
                while t.tag=='DW_TAG_typedef':t=type_of(t)
                result[name]=dict(bytes=value(t,'DW_AT_byte_size'),members={value(x,'DW_AT_name'):value(x,'DW_AT_data_member_location') for x in t.iter_children() if x.tag=='DW_TAG_member' and value(x,'DW_AT_name') is not None})
    assert result['token_generation_t']['bytes']==76 and all(result['token_generation_t']['members'][n]==o for n,o in [('t_player',20),('pow',24),('tou',28),('key_plus',32),('color_forced',40)])
    assert result['stack_data_t']['bytes']==4 and result['stack_data_t']['members']['internal_card_id']==0 and result['stack_data_t']['members']['generating_event']==2
    assert result['card_instance_t']['bytes']==300
    return result


def field_sites(runtime,meta,a):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    code=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));sites=[]
    for i in decoder.disasm(code,int(a,16)):
        if any(o.type==X86_OP_MEM and (o.mem.disp in {56,0x4ef1a7} or 0x509620<=o.mem.disp<0x5097b0 or (a==QUANDRIX and -580<=o.mem.disp<=-564)) for o in i.operands):sites.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex()))
    return sites


def reconstruct(a,raw,runtime,meta):
    from flag_target_bodies import SCOUTING
    text=strip_comments(raw);changes=[];pairs=[];sites=[dict(address=at,bytes=b) for at,b in FIXED.get(a,{}).items()]+field_sites(runtime,meta,a)
    if a==QUANDRIX:
        declaration='  typedef enum { '+', '.join(f'{name} = {number}' for name,number in CHOICES.items())+' } CommandChoices;\n'
        text=text.replace('  CommandChoices CVar1;',declaration+'  CommandChoices CVar1;')
        changes.append(dict(original='  CommandChoices CVar1;',replacement=declaration+'  CommandChoices CVar1;',occurrences=1,kind='restore function-local four-byte CommandChoices enum from owning DWARF lexical block',machine_sites=[]))
    if a==SCOUT:return SCOUTING,[dict(original=text,replacement=SCOUTING,occurrences=1,kind='restore native deck-sized VLA and call frames including extra-entry read',machine_sites=sites,machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=int(meta[a]['high_pc'],16)-int(a,16),machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16))))]
    if a==NIMBLE:pairs=[('2 < event - EVENT_CAN_ACTIVATE_FROM_HAND','2U < (unsigned int)event - (unsigned int)EVENT_CAN_ACTIVATE_FROM_HAND'),('(*(byte *)(player * 4 + 0x4ef1a7) & 2)','(player_bits[player] & PB_GY_ABILITIES_DISABLED)'),('*(char *)((stack_size + -2) * 4 + 0x509632)','(char)stack_data[stack_size - 2].generating_event'),('*(char *)(stack_size * 4 + 0x50962a)','(char)stack_data[stack_size - 2].generating_event'),('*(int32_t *)(stack_size * 8 + 0x5096a0)','stack_cards[stack_size - 2].player'),('*(int32_t *)(stack_size * 8 + 0x5096a4)','stack_cards[stack_size - 2].card')]
    if a==FLASH:pairs=[('1 < event - EVENT_RESOLVE_ACTIVATION','1U < (unsigned int)event - (unsigned int)EVENT_RESOLVE_ACTIVATION')]
    if a==CAN_ACT:pairs=[('event - EVENT_CAST_SPELL < 2','(unsigned int)event - (unsigned int)EVENT_CAST_SPELL < 2U'),('*(char *)((stack_size - iVar2) * 4 + 0x509632)','(char)stack_data[stack_size - iVar2].generating_event'),('*(short *)(&stack_data + (stack_size - iVar2) * 4)','stack_data[stack_size - iVar2].internal_card_id')]
    if a==HEALER:pairs=[('event - EVENT_RESOLVE_ACTIVATION < 2','(unsigned int)event - (unsigned int)EVENT_RESOLVE_ACTIVATION < 2U'),('  size_t __n_1;\n',''),('      __n_1 = 3;\n',''),('snprintf(msg,100,"%s",__format_1,__n_1);','snprintf(msg,100,"%s",__format_1);')]
    if a==VORTEX:pairs=[('(*(code *)0x499010)(0)','EXE_FN(int, 0x499010, int)(0)')]
    if a in {HOURGLASS,HOUR_LEGACY}:pairs=[('_DAT_00736808','EXE_DWORD(0x736808)')]
    if a in {SAMITE,GAMBIT}:pairs=[('*(undefined1 *)&pcVar1->field_26','((unsigned char *)&pcVar1->info_slot)[0]')]
    if a==GAMBIT:pairs.append(('  code *func_ptr;','  int (*func_ptr)(int, int, event_t);'))
    if a==FUNGUS:pairs=[('((uint)pcVar2->field_26 & 2)','((uint)pcVar2->info_slot & 2)')]
    if a==PLATE:pairs=[('td_00,prompt);','td_00,(char *)prompt);'),('  int in_stack_ffffff08;\n',''),('get_color_identity_of_commander(player,in_stack_ffffff08)','get_color_identity_of_all_commanders_2022(player)'),('event - EVENT_POWER < 3','(unsigned int)event - (unsigned int)EVENT_POWER < 3U'),('event - EVENT_POWER < 2','(unsigned int)event - (unsigned int)EVENT_POWER < 2U')]
    if a==PATH:
        decl=re.search(r'(?m)^  anon_union_\w+ aVar6;\n',text);assert decl
        pairs=[(decl[0],''),('          aVar6.info_slot._1_3_ = 0;\n          aVar6.info_slot._0_1_ = cards_data[iVar2].color;\n          pcVar1->field_26 = aVar6;','          pcVar1->info_slot = (unsigned int)cards_data[iVar2].color;')]
    if a==QUANDRIX:pairs=[('choices[0] = CONCAT31(choices[0]._1_3_,1);','((unsigned char *)&choices[0])[0] = 1;'),('choices[0]._0_2_ = CONCAT11(1,(undefined1)choices[0]);','((unsigned char *)&choices[0])[1] = 1;'),('choices[0] = CONCAT31(choices[0]._1_3_,(undefined1)choices[0] + CHOICE_BOUNCE);','((unsigned char *)&choices[0])[0] += CHOICE_BOUNCE;'),('(target_definition_t *)pCStack_248 == ptStack_244','pCStack_248 == choices + 2'),('(target_definition_t *)pCVar7 == ptStack_244','pCVar7 == choices + 2')]
    if a==SHADRIX:
        pairs=[('  undefined *puVar3;','  const char *puVar3;')]
        for at in [0x2747e24,0x2747e28]:pairs.append((f'&UNK_{at:08x}',byte_literal(readonly_string(runtime,at))))
        start=text.index('            default_token_definition(');end=text.index('            generate_token((token_generation_t *)buffer3);',start)+len('            generate_token((token_generation_t *)buffer3);')
        pairs.append((text[start:end],'''            token_generation_t token;
            default_token_definition(player, card, 0x58a7, &token);
            token.pow = 2;
            token.tou = 1;
            token.key_plus = KEYWORD_FLYING;
            token.color_forced = 34;
            token.t_player = iVar2;
            generate_token(&token);'''))
    if a==ZONE:pairs=[('(int32_t)&UNK_027329a8','(int32_t)"SELECT_CARD"')]
    for old,new in pairs:
        count=text.count(old);assert count>0,(a,old)
        change=dict(original=old,replacement=new,occurrences=count,kind='restore native field/stack view, fixed interface, typed phase storage or unsigned event gate',machine_sites=sites)
        if (a==SHADRIX and old.startswith('&UNK_')) or a==ZONE:
            at=int(old[5:],16) if a==SHADRIX else 0x27329a8;literal=readonly_string(runtime,at)
            change.update(native_literal=dict(address=f'{at:x}',bytes=(literal+b'\0').hex(),copy_bytes=len(literal)+1),literal=literal.decode())
        changes.append(change);text=text.replace(old,new)
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('flag_target_consumer_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['native_storage']==native_storage(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()))
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert component['exe_sha256']==sha(exe.data)=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    assert component['exe_boundary']==dict(address='499010',bytes=exe.pe.get_data(0x99010,0x38).hex(),arguments=1,return_value=0,convention='cdecl')
    assert component['exe_boundary']['bytes'].endswith('33c0c9c3')
    assert entries[CAN_TRIG]['previous_recovery']==component['migrated_previous_record'] and entries[CAN_TRIG]['previous_recovery']['name']=='can_counter_triggered_ability'
    assert GLOBAL in manifest['source_globals'] and sources[GLOBAL['source']].count(GLOBAL['declaration'])==1
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_bodies']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    for layout in component['unit_layouts'].values():assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
    def local(a,n):return next(v for v in component['locals'][a] if v['name']==n)
    assert local(SHADRIX,'token')['location']==local(SHADRIX,'buffer3')['location']=='(DW_OP_fbreg: -120)'
    assert local(QUANDRIX,'choices')['location']=='(DW_OP_fbreg: -580)' and local(QUANDRIX,'td')['location']=='(DW_OP_fbreg: -572)'
    assert local(QUANDRIX,'choices')['type']=='CommandChoices[2]' and local(SCOUT,'tutored')['type'].startswith('int[')
    linker=read(root/'manalink.lds');assert re.search(r'_stack_cards\s*=\s*0x5096B0;',linker) and re.search(r'_stack_data\s*=\s*0x509630;',linker)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['flag_target_consumer_evidence']==detail and entry['static']==(a in PRIVATE)
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-flag-target-consumers' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    assert len(entries)==20 and sum(e['name'].startswith('card_') for e in entries.values())==15

def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(debug);(cache/'export-request-flag-target-consumers.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared twenty flag/target consumer and eligibility-helper requests.');return
    assert 'flag_target_consumers_native_2022' not in interfaces and not (set(ADDRESSES)-{CAN_TRIG})&{f['address'] for f in m['accepted_functions']}
    proof=dict(migrated_previous_record=next(f for f in m['accepted_functions'] if f['address']==CAN_TRIG),runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(debug),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{'cards/strixhaven.c','cards/commander_legends.c','cards/commander_2017.c','cards/ice_age.c','cards/invasion.c','cards/weatherlight.c','cards/hour_of_devastation.c','cards/war_of_the_spark.c','cards/urza_legacy.c'}),exe_sha256=sha((cache/'2022-Magic.exe').read_bytes()),exe_boundary=dict(address='499010',bytes=Image((cache/'2022-Magic.exe').read_bytes()).pe.get_data(0x99010,0x38).hex(),arguments=1,return_value=0,convention='cdecl'),functions={},retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],
        scope='Recover fifteen flag/target consumers, two native eligibility helpers, the private Hourglass callback, the public attached-Aura predicate, the private flash-land callback and shared cycling state together in their original source files. Restore typed info-slot and stack views, unsigned event gates, commander specialization binding, Command choice bytes and array endpoint, Shadrix phase-specific token object and label pointers, Scouting VLA/call frames and native extra-entry read, live EXE control word and exact one-argument engine boundary. Retain legacy eligibility and attached-Aura predicate bodies and all prior records, interfaces, globals, fragments, storage/mask registries and dialog stubs; add only native declarations and the verified zero-initialized cycling word. Gameplay integration remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={GLOBAL['source']:read(root/GLOBAL['source']).replace('/* ManalinkEh 2022: 0x2693060; original functions/functions.c:15042. */','/* Retained legacy target-layout compatibility body for 0x2693060. */')+'\n\n#ifndef SHANDALAR\n/* October 2022 shared cycling state at 0x2775470. */\n'+GLOBAL['declaration']+'\n#endif\n'};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-flag-target-consumers'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,ALIASES);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,flag_target_consumer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    migrated=next(e for e in entries if e['address']==CAN_TRIG);migrated['previous_recovery']=proof['migrated_previous_record']
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS and not e['static'])+'\nextern int card_is_being_cycled;\n#endif\n\n'+header[at:];checking=dict(sources);source='cards/invasion.c';prefix=read(root/source)
    assert prefix.count('snprintf(msg, 100, "%s", __format_1, __n_1);')==2
    checking[source]='#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored "-Wformat-extra-args"\n'+prefix+'\n#pragma GCC diagnostic pop\n'+sources[source][len(prefix):]
    compile_sources(root,cache/'flag-target-consumers-check',checking,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':[f for f in m['accepted_functions'] if f['address']!=CAN_TRIG]+entries,'source_globals':m['source_globals']+[GLOBAL]},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not (set(ADDRESSES)-{CAN_TRIG})&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-flag-target-consumers/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-flag-target-consumers'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']=[migrated if f['address']==CAN_TRIG else f for f in m['accepted_functions']]+[e for e in entries if e['address']!=CAN_TRIG];m['source_globals'].append(GLOBAL);m['accepted']+=19;m['card_handler_functions']+=15;m['helper_functions']+=4;m['private_functions']+=2;m['private_helpers']+=2;m['shared_helper_interfaces']+=2
        for e in entries:
            if e['address']!=CAN_TRIG:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['flag_target_consumers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'flag-target-consumers-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=15,new_helpers=4,migrated_helpers=1)))


if __name__=='__main__':main()
