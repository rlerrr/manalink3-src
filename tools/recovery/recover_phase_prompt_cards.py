"""Recover signed phase-AI comparisons and local prompt qualification boundaries."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read
from recover_literals import recover_strings
from recover_spell_copy import compile_sources,format_c
from recover_temporary_exile import save_json,sha
from audit_source import function_body

PHASE_CARDS=['25a9da0','25aa4f0']
PROMPTS={
 '2120600':dict(function='get_sleighted_color_text',site='212064f',target='2680270',argument=None,stores=[]),
 '2124160':dict(function='generic_activated_ability',site='21242b7',target='269db00',argument=12,stores=['2124260']),
 '23897c0':dict(function='generic_activated_ability',site='23898c8',target='269db00',argument=12,stores=['2389876']),
 '2602b50':dict(function='granted_generic_activated_ability',site='2602e06',target='269c8c0',argument=14,stores=['2602d9c','2602e10']),
 '2607b60':dict(function='granted_generic_activated_ability',site='2607d39',target='269c8c0',argument=14,stores=['2607cd0','2607d64'])}
PHASE_SITES={
 '25a9da0':{'25aa1a7':'390570866200','25aa1ad':'0f840d030000','25aa1b3':'833d84f14e0016','25aa1ba':'0f9ec0',
            '25aa1c8':'48','25aa1c9':'43','25aa1ca':'83e0d8','25aa1d0':'83c014','25aa1d5':'8915a8317a00',
            '25aa4c0':'833d84f14e0014','25aa4c7':'e9eefcffff'},
 '25aa4f0':{'25aa62a':'391d70866200','25aa630':'a184f14e00','25aa635':'0f845d010000','25aa63b':'83f816','25aa63e':'0f9ec0',
            '25aa64c':'48','25aa64d':'83e0d8','25aa650':'83c014','25aa655':'8915a8317a00','25aa798':'83f814','25aa79b':'e99efeffff'}}
PHASE_EXPR='ai_modifier = ai_modifier + (current_phase <= (current_turn == player ? PHASE_MAIN1 : PHASE_BEFORE_BLOCKING) ? 20 : -20);'
CALLBACK_CARDS=['2124160','2125a30']


def audit_callbacks(component,manifest,sources,machine_bytes):
    entries={f['address']:f for f in manifest['accepted_functions']}
    assert set(component['functions'])==set(CALLBACK_CARDS)
    predicate=entries['2120600']
    assert predicate['static'] and predicate['source']=='cards/homelands.c'
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,detail in component['functions'].items():
        entry=entries[a];body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        assert entry['source']==predicate['source'] and sha(body.encode())==detail['body_sha256']
        native=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(native)==detail['machine_sha256']
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        assert len(detail['sites'])==2 and body.count('damage_dealt_to_white_creature')==2
        for site in detail['sites']:
            ins=instructions[site['address']]
            assert ins.bytes.hex()==site['bytes'] and ins.mnemonic=='mov' and ins.operands[0].type==X86_OP_MEM and ins.operands[0].size==4
            assert ins.operands[1].type==X86_OP_IMM and ins.operands[1].imm==0x2120600
            assert site['target']=='2120600' and site['name']==predicate['name']


def bind_callbacks(root):
    cache=root/'.recovery';m=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    ip=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(ip.read_text());assert 'white_damage_predicate_callers_2022' not in interfaces
    entries={f['address']:f for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    sources={'cards/homelands.c':read(root/'cards/homelands.c')};proof=dict(functions={},scope='Two private damage-predicate pointer stores in each of Daughter of Autumn and Hazduhr the Abbot; all functions retain their native unit linkage.')
    for a in CALLBACK_CARDS:
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16))
        sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target='2120600',name='damage_dealt_to_white_creature') for i in decoder.disasm(native,int(a,16))
            if any(o.type==X86_OP_IMM and o.imm==0x2120600 for o in i.operands)]
        proof['functions'][a]=dict(machine_bytes=len(native),machine_sha256=sha(native),sites=sites,body_sha256=sha(function_body(sources[entries[a]['source']],entries[a]['name']).encode()))
    audit_callbacks(proof,m,sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    interfaces['white_damage_predicate_callers_2022']=proof;save_json(ip,interfaces)
    print(json.dumps(dict(bound_functions=2,native_callback_pointer_stores=4)))


def debug_evidence(debug,meta,addresses):
    dw=debug.dwarf();lp=LocationParser(dw.location_lists());parameters={};locals_={};enums={}
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {meta[a]['source'] for a in addresses}:continue
        def location(die):
            at=die.attributes.get('DW_AT_location')
            if at is None:return None
            x=lp.parse_from_attribute(at,cu['version'],die=die)
            if isinstance(x,LocationExpr):return describe_DWARF_expr(x.loc_expr,dw.structs)
            return [dict(base=hex(y.base_address)) if isinstance(y,BaseAddressEntry) else dict(start=hex(y.begin_offset),end=hex(y.end_offset),expression=describe_DWARF_expr(y.loc_expr,dw.structs)) for y in x]
        def variables(die):
            result=[]
            for v in die.iter_children():
                if v.tag=='DW_TAG_variable':result.append(dict(name=inherited_value(v,'DW_AT_name'),type=type_name(type_of(v)),location=location(v)))
                elif v.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:result+=variables(v)
            return result
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for child in die.iter_children():
                    n=value(child,'DW_AT_name')
                    if n in {'PHASE_MAIN1','PHASE_BEFORE_BLOCKING'}:enums[n]=value(child,'DW_AT_const_value')
            pc=value(die,'DW_AT_low_pc');a=f'{pc:x}' if pc else ''
            if a not in addresses:continue
            parameters[a]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p)) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_[a]=variables(die)
    assert set(parameters)==set(addresses)
    return dict(parameters=parameters,locals=locals_,enum_values=enums)


def audit_phase(component,manifest,sources,machine_bytes,root):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('phase_ai_evidence')}
    require(set(entries)==set(PHASE_CARDS)==set(component['functions']),'Phase-AI card membership mismatch')
    require(component['enum_values']==dict(PHASE_MAIN1=20,PHASE_BEFORE_BLOCKING=22),'Phase-AI native thresholds mismatch')
    for a,entry in entries.items():
        detail=component['functions'][a];require(entry['phase_ai_evidence']==detail and not entry['static'],'Phase-AI evidence/linkage mismatch')
        first=detail['changes'][0]
        require(first['original'].count('SBORROW4')==2 and first['kind']=='signed phase comparison and plus/minus 20 AI adjustment',
                'Phase-AI CPU-flag reconstruction mismatch')
        expected=('iVar5 = iVar5 + 1;\n' if a=='25a9da0' else '')+PHASE_EXPR
        require(first['replacement']==expected and {s['address']:s['bytes'] for s in first['machine_sites']}==PHASE_SITES[a],
                'Phase-AI comparison/control/store proof mismatch')
        for address,data in PHASE_SITES[a].items():require(machine_bytes(int(address,16),len(bytes.fromhex(data))).hex()==data,'Phase-AI native instruction mismatch')
        body=function_body(sources[entry['source']],entry['name'])
        require('SBORROW4' not in body and 'PHASE_MAIN1 : PHASE_BEFORE_BLOCKING' in body,'Phase-AI signed source comparison missing')
    require(any(v['name']=='marked' and v['type']=='int[151]' and v['location']=='(DW_OP_fbreg: -624)' for v in component['locals']['25a9da0']),
            'Kamahl Will damage-source snapshot mismatch')
    for a,expected in {'25a9da0':{'td':-792,'td2':-708},'25aa4f0':{'td':-104}}.items():
        for name,offset in expected.items():
            require(any(v['name']==name and v['type']=='target_definition_t' and v['location']==f'(DW_OP_fbreg: {offset})' for v in component['locals'][a]),
                    'Kamahl target-definition stack storage mismatch')
    bindings={n:int(v,16) for n,v in re.findall(r'(?m)^_(\w+)\s*=\s*(0x[0-9a-fA-F]+);',read(root/'manalink.lds'))}
    require(all(bindings[n]==v for n,v in dict(current_turn=0x628670,current_phase=0x4ef184,ai_modifier=0x7a31a8).items()),'Phase-AI retained EXE global binding mismatch')


def audit_prompts(component,manifest,sources,machine_bytes):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('prompt_boundary_evidence')}
    require(set(entries)==set(PROMPTS)==set(component['functions']),'Prompt-boundary membership mismatch')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,cfg in PROMPTS.items():
        entry=entries[a];detail=component['functions'][a]
        require(entry['prompt_boundary_evidence']==detail and entry['static']==(a=='2120600') and len(detail['changes'])==1,'Prompt-boundary evidence/linkage mismatch')
        boundary=detail['boundary'];require(boundary['argument']==cfg['argument'] and boundary['target']==cfg['target'] and boundary['call_address']==cfg['site'],
                'Prompt-boundary native ABI role mismatch')
        instructions={f'{i.address:x}':i for i in decoder.disasm(machine_bytes(int(a,16),detail['machine_bytes']),int(a,16))}
        call=instructions[cfg['site']]
        require(call.bytes.hex()==boundary['call_bytes'] and call.mnemonic in {'call','jmp'} and call.operands[0].type==X86_OP_IMM and call.operands[0].imm==int(cfg['target'],16),
                'Prompt-boundary native callee mismatch')
        require([s['address'] for s in boundary['prompt_stores']]==cfg['stores'],'Prompt-boundary store membership mismatch')
        for site in boundary['prompt_stores']:
            ins=instructions[site['address']]
            require(ins.bytes.hex()==site['bytes'] and ins.mnemonic=='mov' and ins.operands[0].type==X86_OP_MEM and ins.reg_name(ins.operands[0].mem.base)=='esp' and
                    ins.operands[0].mem.disp==cfg['argument']*4 and ins.operands[0].size==4,'Prompt-boundary outgoing pointer store mismatch')
        change=detail['changes'][0]
        require(change['kind']=='local pointer qualification at native boundary' and change['occurrences']==1 and '(char *)' in change['replacement'],
                'Prompt-boundary source qualifier projection mismatch')
    require([p['name'] for p in component['parameters']['2120600']]==['who_chooses','player','card','target_player','target_card'],
            'Damage-selection predicate parameter order mismatch')
    for support in component['source_interfaces']:
        require(sha(function_body(sources[support['source']].replace('\r\n','\n'),support['name']).encode())==support['body_sha256'],'Prompt-boundary supporting source interface changed')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prompts',action='store_true');parser.add_argument('--bind-callbacks',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';directory=cache/'decompiled-phase-prompt-cards'
    if args.bind_callbacks:
        assert args.apply and not args.prompts
        bind_callbacks(root);return
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());addresses=list(PROMPTS) if args.prompts else PHASE_CARDS
    component='prompt_boundary_consumers_2022' if args.prompts else 'phase_ai_cards_2022';assert component not in interfaces
    assert not set(addresses)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data)
    proof=dict(debug_sha256=sha(debug_data),**debug_evidence(debug,meta,addresses),functions={},
        scope='Local native CPU-flag or pointer-qualification reconstruction with unchanged control flow, arguments and storage. Use recovered native aggregate/extension APIs.',
        integration_pending='Native linking and gameplay verification remain pending.')
    if args.prompts:
        proof['source_interfaces']=[]
        for name in ['get_sleighted_color_text','generic_activated_ability_2022','granted_generic_activated_ability_2022']:
            proof['source_interfaces'].append(dict(source='functions/functions.c',name=name,body_sha256=sha(function_body(read(root/'functions/functions.c'),name).encode())))
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets.update({int(a,16):meta[a]['name'] for a in addresses})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    sources={};entries=[]
    for a in addresses:
        row=meta[a];raw=read(directory/f'{a}.c');text=strip_comments(raw);native=runtime.pe.get_data(int(a,16)-0x2000000,int(row['high_pc'],16)-int(a,16))
        instructions=list(decoder.disasm(native,int(a,16)));by_site={f'{i.address:x}':i for i in instructions};changes=[];boundary=None
        if args.prompts:
            cfg=PROMPTS[a]
            if a=='2120600':original='pcVar2 = get_sleighted_color_text';replacement='pcVar2 = (char *)get_sleighted_color_text'
            else:
                matches=list(re.finditer(r'\b'+cfg['function']+r'\s*\([^;{}]*,\s*prompt\s*\)',text));assert len(matches)==1,(a,len(matches))
                original=matches[0][0];replacement=re.sub(r',\s*prompt\s*\)$',',(char *)prompt)',original)
            assert text.count(original)==1 and replacement!=original;text=text.replace(original,replacement,1)
            site_addresses=[cfg['site']]+cfg['stores'];sites=[dict(address=s,bytes=by_site[s].bytes.hex()) for s in site_addresses]
            changes=[dict(original=original,replacement=replacement,occurrences=1,kind='local pointer qualification at native boundary',machine_sites=sites)]
            boundary=dict(argument=cfg['argument'],target=cfg['target'],call_address=cfg['site'],call_bytes=by_site[cfg['site']].bytes.hex(),prompt_stores=sites[1:])
        else:
            start=text.index('if (current_turn == player) {');end=text.index(';',text.index('ai_modifier = ai_modifier + ((',start))+1
            original=text[start:end];assert original.count('SBORROW4')==2
            replacement=('iVar5 = iVar5 + 1;\n' if a=='25a9da0' else '')+PHASE_EXPR
            sites=[dict(address=s,bytes=by_site[s].bytes.hex()) for s in PHASE_SITES[a]]
            assert {s['address']:s['bytes'] for s in sites}==PHASE_SITES[a]
            changes.append(dict(original=original,replacement=replacement,occurrences=1,kind='signed phase comparison and plus/minus 20 AI adjustment',machine_sites=sites))
            text=text[:start]+replacement+text[end:]
            for variable in (['bVar12'] if a=='25a9da0' else ['bVar3','bVar4']):
                declaration='  bool '+variable+';';assert text.count(declaration)==1 and len(re.findall(r'\b'+variable+r'\b',text))==1
                text=text.replace(declaration,'',1);changes.append(dict(original=declaration,replacement='',occurrences=1,kind='remove dead CPU-flag temporary',machine_sites=[]))
        text,literals=recover_strings(text,runtime,{})
        body=format_c(clean(text,overlap,use_keyword_accessors_2022=True,use_extension_flags_2022=True,use_mana_api_2022=True,
            use_test_api_2022=True,use_target_api_2022=True,use_sacrifice_api_2022=True,use_spell_api_2022=True,use_ability_api_2022=True,use_pump_api_2022=True,
            use_activated_api_2022=True,use_attack_api_2022=True,use_entry_api_2022=True,use_spell_trigger_api_2022=True,use_life_api_2022=True,use_life_loss_api_2022=True))
        assert not UNRESOLVED.search(body) and not re.search(r'SBORROW|\._\d|stack0x',body),a
        private=a=='2120600'
        symbols=[s for n,o,sec,k,s in debug.functions() if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+o==int(a,16)];assert symbols==[3 if private else 2],(a,symbols)
        if private:body='static '+body
        calls=[]
        for ins in instructions:
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
                target=targets[ins.operands[0].imm];assert re.search(r'\b'+target+r'\s*\(',body),(a,target)
                calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=target))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls)
        if boundary is not None:detail['boundary']=boundary
        proof['functions'][a]=detail
        e=dict(address=a,name=row['name'],source=row['source'],line=row['line'],static=private,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),
            recovered_literals=literals,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,row['name']).encode()))
        e['prompt_boundary_evidence' if args.prompts else 'phase_ai_evidence']=detail;entries.append(e)
        sources.setdefault(row['source'],read(root/row['source']));sources[row['source']]+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=None
    compile_sources(root,cache/('prompt-boundary-check' if args.prompts else 'phase-ai-cards-check'),sources,header)
    check_sources={**sources,'functions/functions.c':read(root/'functions/functions.c')}
    if args.prompts:audit_prompts(proof,{**m,'accepted_functions':m['accepted_functions']+entries},check_sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    else:audit_phase(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n),root)
    if args.apply:
        citations=m['source_fragments']+m['accepted_functions']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(addresses)&{c['address'] for c in citations}
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        if header is not None:(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=len(entries);m['card_handler_functions']+=4 if args.prompts else 2
        if args.prompts:m['helper_functions']+=1;m['private_functions']+=1;m['private_helpers']+=1
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        registrations=list(csv.DictReader((cache/'2022-registrations.csv').open()));names={f['name'] for f in m['accepted_functions']}
        m['registered_handler_symbols']=len({r['symbol'].lstrip('_') for r in registrations if r['symbol'].lstrip('_') in names})
        m['registered_handler_symbols_marked_coded']=len({r['symbol'].lstrip('_') for r in registrations if r['coded']=='True' and r['symbol'].lstrip('_') in names})
        save_json(mp,m);interfaces[component]=proof;save_json(ip,interfaces)
        for a in addresses:shutil.copyfile(directory/f'{a}.c',cache/'decompiled-abi'/f'{a}.c')
    save_json(cache/('prompt-boundary-plan.json' if args.prompts else 'phase-ai-cards-plan.json'),proof)
    print(json.dumps(dict(applied=args.apply,new_implementations=len(entries),card_functions=4 if args.prompts else 2,helpers=1 if args.prompts else 0)))


if __name__=='__main__':main()
