"""Recover specialized Spectacle/Amplify and native hand/counter helper consumers."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_REG_ESP,X86_REG_EBP
from inspect_release import Image,value,inherited_value
from export_abi import type_of
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings,readonly_string
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

SPECTACLE='25bbda0'
CONSUMERS='25be180 25c0a00 25c27a0 25c32f0 25c3580 25c3830'.split()
REVEAL='2691e10'
AMPLIFY='24a6470'
BOOST='2199b20'
DISCARD='2699e10'
COERCION='2699f60'
EXILE='269fe40'
ADDRESSES=[SPECTACLE]+CONSUMERS+[REVEAL,AMPLIFY,'24a67c0',BOOST,'25c47a0',DISCARD,COERCION,EXILE]
PRIVATE={SPECTACLE,AMPLIFY,DISCARD}
PUBLIC={REVEAL,BOOST,COERCION,EXILE}
NAMES={n:n+'_2022' for n in ['reveal_cards_from_your_hand','boost_creatures_by_counters','new_effect_coercion','exile_top_x_card_you_may_play_them']}
NAMES['discard_card_by_int_id']='discard_card_by_int_id_2022_impl'
LABEL='/* October 2022 native hand reveal and counter-based continuous boosts. */'
ENUMS=dict(TRAP_LIFE_LOST=2,TRIGGER_UPKEEP=201,COUNTER_P1_P1=0,BCT_CONTROLLER_ONLY=1,BCT_INCLUDE_SELF=2)
SITES={SPECTACLE:{'25bbdab':'81f9a4000000','25bbdb7':'83f96c','25bbf3d':'c744240402000000'},
    AMPLIFY:{'24a6480':'898d18ffffff','24a64f2':'c745e0505c4a02','24a6507':'0fafc8',
        '24a655e':'c74424085ce87402','24a6566':'8944240c','24a656a':'c744240464000000','24a6575':'e8c6feffff',
        '24a6446':'8d4514','24a6449':'c74424085ce87402','24a6458':'c744240464000000','24a6463':'e8a86c2500'},
    BOOST:{'2199b56':'8d47ce','2199b5f':'83f802','2199b62':'760c'},
    '25c3580':{'25c3588':'813d782d7a00c9000000'},
    '25c27a0':{'25c280c':'f6403802','25c2858':'c7403800000000'},
    COERCION:{'269a30a':'8d4fff','269a30d':'8b94857cfdffff','269a325':'8d84bd78fdffff','269a330':'8b7904','269a333':'83c104','269a336':'8979fc','269a339':'39c1','269a33b':'75f3'},
    '24a67c0':{'24a67c3':'837d1071','24a67d6':'b901000000','24a67db':'e890fcffff'}}
LEGACY=[('functions/functions.c',n) for n in ['reveal_cards_from_your_hand','new_effect_coercion','discard_card_by_int_id','exile_top_x_card_you_may_play_them']]+[('cards/khans_of_tarkir.c','boost_creatures_by_counters'),('cards/legions.c','amplify')]
EC_LAYOUT='typedef char recovery_ec_definition_2022_layout[(sizeof(ec_definition_t) == 28 && offsetof(ec_definition_t, target_player) == 0 && offsetof(ec_definition_t, who_choses) == 4 && offsetof(ec_definition_t, effect) == 8 && offsetof(ec_definition_t, cards_to_reveal) == 12 && offsetof(ec_definition_t, qty) == 16 && offsetof(ec_definition_t, ai_selection_mode) == 20 && offsetof(ec_definition_t, special_selection_function) == 24) ? 1 : -1];'


def native_specialization(debug):
    clones={};enums={};ec_layout=None
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {'cards/ravnica_allegiance.c','cards/legions.c','functions/functions.c'}:continue
        for d in cu.get_top_DIE().iter_children():
            a=value(d,'DW_AT_low_pc')
            if a in [int(SPECTACLE,16),int(AMPLIFY,16)]:
                clones[f'{a:x}']={inherited_value(p,'DW_AT_name'):dict(location=value(p,'DW_AT_location'),constant=value(p,'DW_AT_const_value')) for p in d.iter_children() if p.tag=='DW_TAG_formal_parameter'}
            if source=='cards/ravnica_allegiance.c' and d.tag=='DW_TAG_enumeration_type':
                for x in d.iter_children():
                    n=value(x,'DW_AT_name')
                    if n in ENUMS:enums[n]=value(x,'DW_AT_const_value')
            if source=='functions/functions.c' and d.tag=='DW_TAG_typedef' and value(d,'DW_AT_name')=='ec_definition_t':
                t=type_of(d)
                while t.tag=='DW_TAG_typedef':t=type_of(t)
                ec_layout=dict(bytes=value(t,'DW_AT_byte_size'),members={value(x,'DW_AT_name'):value(x,'DW_AT_data_member_location') for x in t.iter_children() if x.tag=='DW_TAG_member'})
    assert enums==ENUMS
    sp=clones[SPECTACLE];assert set(sp)=={'player','card','event','colorless','black','blue','green','red','white'}
    assert all(sp[n]==dict(location=None,constant=0) for n in ['blue','green','white'])
    assert [sp[n]['location'] for n in ['colorless','black','red']]==[[145,i*4] for i in range(3)]
    assert set(clones[AMPLIFY])=={'player','card','event','amount'}
    assert ec_layout==dict(bytes=28,members=dict(target_player=0,who_choses=4,effect=8,cards_to_reveal=12,qty=16,ai_selection_mode=20,special_selection_function=24))
    return dict(clones=clones,enums=enums,ec_layout=ec_layout,spectacle_physical_arguments=6,amplify_physical_arguments=3)


def reconstruct(a,raw):
    text=strip_comments(raw);changes=[];pairs=[]
    if a==SPECTACLE:pairs=[('int event,int colorless','event_t event,int colorless'),('event == 0xa4','event == EVENT_MODIFY_COST'),('event == 0x6c','event == EVENT_CAST_SPELL'),('get_trap_condition(1 - player,2)','get_trap_condition(1 - player,TRAP_LIFE_LOST)')]
    if a==AMPLIFY:pairs=[('  size_t in_stack_ffffff08;\n',''),(',__format_1,\n               in_stack_ffffff08)',',__format_1)')]
    if a==BOOST:pairs=[('event - EVENT_POWER < 3','(unsigned int)event - (unsigned int)EVENT_POWER < 3U')]
    if a=='25c3580':pairs=[('_trigger_condition == 0xc9','trigger_condition == TRIGGER_UPKEEP')]
    if a=='25c27a0':pairs=[('((uint)pcVar2->field_26 & 2)','((uint)pcVar2->info_slot & 2)')]
    if a==COERCION:pairs=[('initial_hand + iStack_354 + 0x31','ai_hand + iStack_354 - 1')]
    for old,new in pairs:
        assert text.count(old)==1,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=1,kind='restore native clone types, bounded variadic frame, enum or unsigned event range',machine_sites=[dict(address=at,bytes=b) for at,b in SITES[a].items()]));text=text.replace(old,new)
    return text,changes


def spectacle_arguments(code,a,body):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;insns=list(decoder.disasm(code,int(a,16)))
    indexes=[i for i,x in enumerate(insns) if x.mnemonic in {'call','jmp'} and x.operands[0].type==X86_OP_IMM and x.operands[0].imm==int(SPECTACLE,16)]
    arguments=[]
    for i in indexes:
        call=insns[i];tail=call.mnemonic=='jmp';base=X86_REG_EBP if tail else X86_REG_ESP;bias=8 if tail else 0;stores={};sites=[]
        for ins in reversed(insns[:i]):
            if ins.mnemonic in {'call','ret'}:break
            if ins.mnemonic=='mov' and len(ins.operands)==2 and ins.operands[0].type==X86_OP_MEM and ins.operands[0].mem.base==base and ins.operands[1].type==X86_OP_IMM:
                offset=ins.operands[0].mem.disp-bias
                if offset in [0,4,8] and offset not in stores:
                    stores[offset]=ins.operands[1].imm;sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex()))
        assert set(stores)=={0,4,8},(a,stores)
        arguments.append(dict(arguments=[stores[n] for n in [0,4,8]],stores=sites,tail_call=tail,call=dict(address=f'{call.address:x}',bytes=call.bytes.hex())))
    matches=re.findall(r'spectacle_2022_impl\(player,\s*card,\s*event,\s*([^;]+?)\)',body)
    assert len(matches)==len(arguments)>0
    assert {tuple(int(x.strip(),0) for x in args.split(',')) for args in matches}=={tuple(item['arguments']) for item in arguments}
    return arguments


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('casting_reveal_family_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes())
    assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['specialization']==native_specialization(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()))
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_bodies']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    for layout in component['unit_layouts'].values():assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
    locals_=component['locals'][REVEAL]
    assert all(any(v['name']==n and v['type']=='int[100]' and v['location']==f'(DW_OP_fbreg: {offset})' for v in locals_) for n,offset in [('revealed',-820),('your_hand',-420)])
    assert all(any(v['name']==n and v['type']=='int[50]' and v['location']==f'(DW_OP_fbreg: {offset})' for v in component['locals'][COERCION]) for n,offset in [('initial_hand',-852),('ai_hand',-652)])
    assert EC_LAYOUT in read(root/'manalink.h') or component.get('planning')
    assert [component['parameters'][AMPLIFY][i]['location'][0]['expression'] for i in range(3)]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    assert 'DW_OP_GNU_parameter_ref' in component['parameters'][AMPLIFY][3]['location'][0]['expression']
    assert [component['parameters'][DISCARD][i]['location'][0]['expression'] for i in range(3)]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    prompt=component['format_wrapper'];assert prompt['address']=='24a6440' and prompt['bytes']==machine_bytes(0x24a6440,42).hex()
    assert prompt['literal']==readonly_string(runtime,0x274e85c).hex() and bytes.fromhex(prompt['literal'])==b'Select a card that shares a subtype with %s.'
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['casting_reveal_family_evidence']==detail and entry['static']==(a in PRIVATE)
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-casting-reveal-family' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        for at,b in SITES.get(a,{}).items():assert machine_bytes(int(at,16),len(bytes.fromhex(b))).hex()==b
        if a in CONSUMERS:assert detail['spectacle_arguments']==spectacle_arguments(code,a,body)
        if a in PUBLIC:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert entries[SPECTACLE]['prototype'].count(',')==5 and entries[AMPLIFY]['prototype'].count(',')==2
    assert 'reveal_cards_from_your_hand_2022(player, card, &test)' in function_body(sources['cards/legions.c'],entries[AMPLIFY]['name'])
    assert len(entries)==15 and sum(e['name'].startswith('card_') for e in entries.values())==8


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_specialization(debug)
        (cache/'casting-reveal-clones.tsv').write_text(SPECTACLE+'\tspectacle_2022_impl\tint\t__regparm3\tint:player\tint:card\tint:event\tint:colorless\tint:black\tint:red\n'+AMPLIFY+'\tamplify_2022_impl\tvoid\t__regparm3\tint:player\tint:card\tint:amount\n')
        (cache/'export-request-casting-reveal-family.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared fifteen exports and two verified physical signatures.');return
    assert 'casting_reveal_family_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),specialization=native_specialization(debug),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{'cards/ravnica_allegiance.c','cards/legions.c','functions/functions.c'}),functions={},
        format_wrapper=dict(address='24a6440',bytes=get(0x24a6440,42).hex(),literal=readonly_string(runtime,0x274e85c).hex(),scope='Native snprintf specialization consumes the first variadic argument at EBP+20; caller supplies exactly four words, fixed size 100 and one %s. Remove the invented fifth argument; wrapper return is unused.'),
        retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],
        scope='Recover six Spectacle handlers, Aven Warhawk and Zegana with seven native helper implementations. Restore six-argument Spectacle and three-register Amplify parts, zero mana constants and eight caller frames; use native test/target domains, fixed hand arrays, subtype callback, four-word formatted prompt and unsigned counter-boost gate. Recover native coercion/discard and filtered top-deck exile; express the adjacent-array end pointer within its owning array. Preserve native payment, fizzle, trigger and manual-selection oddities; retain earlier legacy bodies and records.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name={SPECTACLE:'spectacle_2022_impl',AMPLIFY:'amplify_2022_impl'}.get(a,NAMES.get(meta[a]['name'],meta[a]['name'])),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-casting-reveal-family'/f'{a}.c');text,changes=reconstruct(a,raw);text,literals=recover_strings(text,runtime,{});text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==([3] if a in PRIVATE else [2])
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        if a in CONSUMERS:detail['spectacle_arguments']=spectacle_arguments(code,a,body)
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,casting_reveal_family_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+EC_LAYOUT+'\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in PUBLIC)+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'casting-reveal-family-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-casting-reveal-family/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-casting-reveal-family'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=15;m['card_handler_functions']+=8;m['helper_functions']+=7;m['private_functions']+=3;m['private_helpers']+=3;m['shared_helper_interfaces']+=4
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['casting_reveal_family_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'casting-reveal-family-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=8,new_helpers=7,private_helpers=3)))


if __name__=='__main__':main()
