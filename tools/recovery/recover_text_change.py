"""Recover the native four-argument color and land-word replacement helpers."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image,value
from promote_recovery import clean,strip_comments,UNRESOLVED,TEXT_CHANGE_API_NAMES
from recover_test_consumers import read
from recover_literals import recover_strings
from recover_spell_copy import compile_sources,format_c
from recover_temporary_exile import save_json,sha
from recover_phase_prompt_cards import debug_evidence
from audit_source import function_body

ADDRESSES=['267f930','267fdc0']
HEADER_LABEL='/* October 2022 color and basic-land word replacement APIs. */'
ZERO_DECL='\tSF3_0 = 0, /* Recovered October 2022 zero-valued flag name. */\n'
ORIGINAL='uStack_30._0_1_ = (uint8_t)iVar5;'
REPLACEMENT='uStack_30 = (uint)iVar5;'
SITES={
 '267f930':{'267fb9b':'e8b0a00600','267fba7':'8945d4','267fbf3':'8a4dd4','267fbf7':'884c033f','267fd5f':'8a55d4','267fd62':'88540140'},
 '267fdc0':{'268002b':'e8209c0600','2680037':'8945d4','2680086':'8a4dd4','268008a':'888c0313010000','26801ff':'8a55d4','2680202':'88940114010000'}}
CARD='23f08c0'
PROMPT_ORIGINAL='generic_activated_ability(player,card,event,7,0,0,0,0,0,0,0,&td,prompt)'
PROMPT_REPLACEMENT=PROMPT_ORIGINAL.replace('&td,prompt)','&td,(char *)prompt)')
PROMPT_SITES={'23f0945':'e826f92800','23f094e':'89442430','23f09a0':'e85bd12a00'}


def audit_card(component,manifest,sources,machine_bytes,root=None):
    if root is None:root=Path(__file__).resolve().parents[2]
    def require(ok,message):
        if not ok:raise ValueError(message)
    entry=next(f for f in manifest['accepted_functions'] if f['address']==CARD);detail=component['function']
    require(entry['name']=='card_balduvian_shaman' and not entry['static'] and entry['text_change_card_evidence']==detail,'Shaman native evidence/linkage mismatch')
    require(detail['changes']==[dict(original=PROMPT_ORIGINAL,replacement=PROMPT_REPLACEMENT,occurrences=1,
        kind='local pointer qualification at native prompt boundary',machine_sites=[dict(address=a,bytes=b) for a,b in PROMPT_SITES.items()])],
        'Shaman native prompt bridge mismatch')
    for a,data in PROMPT_SITES.items():require(machine_bytes(int(a,16),len(bytes.fromhex(data))).hex()==data,'Shaman native prompt transfer mismatch')
    require(any(v['name']=='td' and v['type']=='target_definition_t' and v['location']=='(DW_OP_fbreg: -104)' for v in component['locals'][CARD]),
        'Shaman target-definition local storage mismatch')
    body=function_body(sources[entry['source']],entry['name'])
    require('replace_all_instances_of_one_color_word_with_another_2022(' in body and '(char *)prompt' in body,'Shaman native text/prompt interfaces missing')
    references=detail['function_references'];require({r['name'] for r in references}=={'doesnt_have_cumulative_upkeep','effect_basha'} and len(references)==2,
        'Shaman native predicate/legacy references mismatch')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for ref in references:
        data=bytes.fromhex(ref['bytes']);ins=list(decoder.disasm(machine_bytes(int(ref['address'],16),len(data)),int(ref['address'],16)))
        require(len(ins)==1 and ins[0].bytes==data and any(o.type==X86_OP_IMM and o.imm==int(ref['target'],16) for o in ins[0].operands) and
            re.search(r'\b'+ref['name']+r'\b',body),'Shaman native callback operand mismatch')
    for support in component['source_interfaces']:
        from unify_scalar_helpers import retained_source_body
        require(sha(retained_source_body(root,support['source'],support['name'],source_view=sources).encode())==support['body_sha256'],
            'Shaman retained source interface changed')


def recover_card(root,apply):
    cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'text_change_helpers_2022' in interfaces and 'shaman_text_change_2022' not in interfaces
    assert CARD not in {f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};row=meta[CARD]
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data)
    proof=dict(debug_sha256=sha(debug_data),**debug_evidence(debug,meta,[CARD]),source_interfaces=[],
        scope='Native Shaman using the four-argument text-changing helper, recovered target/activation APIs and one pointer-qualification bridge at prompt argument 12.',
        integration_pending='Native linking and gameplay verification remain pending.')
    raw=read(cache/'decompiled-text-change'/f'{CARD}.c');text=strip_comments(raw);assert text.count(PROMPT_ORIGINAL)==1;text=text.replace(PROMPT_ORIGINAL,PROMPT_REPLACEMENT)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());text,literals=recover_strings(text,runtime,{})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    body=format_c(clean(text,overlap,use_keyword_accessors_2022=True,use_extension_flags_2022=True,use_test_api_2022=True,use_target_api_2022=True,
        use_activated_api_2022=True,use_text_change_api_2022=True))
    assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body)
    symbols=[s for n,o,sec,k,s in debug.functions() if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+o==int(CARD,16)];assert symbols==[2]
    native=runtime.pe.get_data(int(CARD,16)-0x2000000,int(row['high_pc'],16)-int(CARD,16));decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};callbacks={a:n for a,n in targets.items() if n in {'doesnt_have_cumulative_upkeep','effect_basha'}};calls=[];references=[]
    for ins in decoder.disasm(native,int(CARD,16)):
        if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
            target=targets[ins.operands[0].imm];assert re.search(r'\b'+target+r'\s*\(',body),target
            calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=target))
        for operand in ins.operands:
            if operand.type==X86_OP_IMM and operand.imm in callbacks:
                references.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{operand.imm:x}',name=callbacks[operand.imm]))
    detail=dict(machine_bytes=len(native),machine_sha256=sha(native),canonical_calls=calls,function_references=references,changes=[dict(original=PROMPT_ORIGINAL,replacement=PROMPT_REPLACEMENT,occurrences=1,
        kind='local pointer qualification at native prompt boundary',machine_sites=[dict(address=a,bytes=b) for a,b in PROMPT_SITES.items()])]);proof['function']=detail
    e=dict(address=CARD,name=row['name'],source=row['source'],line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),
        recovered_literals=literals,text_change_card_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,row['name']).encode()))
    sources={row['source']:read(root/row['source']),'functions/functions.c':read(root/'functions/functions.c')}
    for source,name in [('functions/functions.c',n) for n in ['get_sleighted_color_text','generic_activated_ability_2022','granted_generic_activated_ability_2022']]+[(row['source'],n) for n in callbacks.values()]:
        proof['source_interfaces'].append(dict(source=source,name=name,body_sha256=sha(function_body(sources[source],name).encode())))
    sources[row['source']]+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{CARD}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    compile_sources(root,cache/'shaman-text-check',{row['source']:sources[row['source']]})
    audit_card(proof,{**m,'accepted_functions':m['accepted_functions']+[e]},sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if apply:
        (root/row['source']).write_bytes(sources[row['source']].replace('\n','\r\n').encode('latin1'));m['accepted_functions'].append(e);m['accepted']+=1;m['card_handler_functions']+=1;m['source_files'][row['source']]+=1
        registrations=list(csv.DictReader((cache/'2022-registrations.csv').open()));names={f['name'] for f in m['accepted_functions']}
        m['registered_handler_symbols']=len({r['symbol'].lstrip('_') for r in registrations if r['symbol'].lstrip('_') in names})
        m['registered_handler_symbols_marked_coded']=len({r['symbol'].lstrip('_') for r in registrations if r['coded']=='True' and r['symbol'].lstrip('_') in names})
        save_json(mp,m);interfaces['shaman_text_change_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'shaman-text-plan.json',proof);print(json.dumps(dict(applied=apply,new_card_functions=1)))


def audit_component(component,manifest,sources,machine_bytes,root,header=None,definitions=None):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('text_change_evidence')}
    require(set(entries)==set(component['functions'])==set(ADDRESSES),'Text-change helper membership mismatch')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a]
        require(not entry['static'] and entry['name'] in TEXT_CHANGE_API_NAMES.values() and entry['text_change_evidence']==detail,'Text-change helper linkage/evidence mismatch')
        require([p['name'] for p in component['parameters'][a]]==['player','card','t_player','t_card'] and
            [p['location'] for p in component['parameters'][a]]==[f'(DW_OP_fbreg: {n})' for n in range(0,16,4)],'Text-change physical parameters mismatch')
        require(any(v['name']=='options' and v['type']=='int[6]' and v['location']=='(DW_OP_fbreg: -44)' for v in component['locals'][a]),'Text-change option-array storage mismatch')
        require(len(detail['changes'])==1 and detail['changes'][0]['original']==ORIGINAL and detail['changes'][0]['replacement']==REPLACEMENT and
            {s['address']:s['bytes'] for s in detail['changes'][0]['machine_sites']}==SITES[a],'Text-change native full-word result projection mismatch')
        native=machine_bytes(int(a,16),detail['machine_bytes']);require(sha(native)==detail['machine_sha256'],'Text-change native body hash mismatch')
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        for site,data in SITES[a].items():require(instructions[site].bytes.hex()==data,'Text-change result instruction mismatch')
        call=instructions[next(iter(SITES[a]))]
        require(call.mnemonic=='call' and call.operands[0].imm==0x26e9c50,'Text-change chooser result source mismatch')
        store=instructions[list(SITES[a])[1]]
        require(store.mnemonic=='mov' and store.operands[0].type==X86_OP_MEM and store.operands[0].size==4 and
            decoder.reg_name(store.operands[0].mem.base)=='ebp' and store.operands[0].mem.disp==-44 and decoder.reg_name(store.operands[1].reg)=='eax',
            'Text-change full chooser result stack store mismatch')
        body=function_body(sources[entry['source']],entry['name'])
        member='color_id' if a=='267f930' else 'hack_mode'
        require(body.count(member+'[uVar1] = (uint8_t)uStack_30;')==1 and body.count(member+'[iVar5] = (uint8_t)uStack_30;')==1,
            'Text-change byte result consumers mismatch')
        require(body.count('options[instance->'+member+'[iVar5]] = 1;')==1 and 't_player, t_card' in body,'Text-change chained mapping/target identity missing')
    for support in component['retained_source_helpers']:
        from unify_scalar_helpers import retained_source_body
        require(sha(retained_source_body(root,'functions/functions.c',support['name']).encode())==support['body_sha256'],
            'Text-change retained source helper body changed')
    require(component['layout']==dict(color_offset=64,hack_offset=276,mapping_bytes=6,options_bytes=24,result_store_bytes=4,result_read_bytes=1),
        'Text-change native storage widths mismatch')
    header=read(root/'manalink.h') if header is None else header
    for e in entries.values():
        prototype=re.search(r'\bvoid\s+'+e['name']+r'\s*\([^;{}]*\);',header)
        require(prototype is not None and re.sub(r'\s','',prototype[0])==re.sub(r'\s','',e['prototype']),'Text-change public prototype mismatch')
    definitions=read(root/'defs.h') if definitions is None else definitions
    require(component['zero_flags']==dict(SF3_0=0) and definitions.count(ZERO_DECL)==1,'Text-change native zero-valued flag mismatch')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--card',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';directory=cache/'decompiled-text-change'
    if args.card:recover_card(root,args.apply);return
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'text_change_helpers_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data)
    proof=dict(debug_sha256=sha(debug_data),**debug_evidence(debug,meta,ADDRESSES),functions={},retained_source_helpers=[],
        layout=dict(color_offset=64,hack_offset=276,mapping_bytes=6,options_bytes=24,result_store_bytes=4,result_read_bytes=1),
        scope='Four native parameters separate the acting card from the target. Preserve native chained mappings, AI choices, cached text masks and byte writes. Replace the decompiler partial result assignment with the complete native EAX stack-word store.',
        integration_pending='Native linking and gameplay verification remain pending.')
    zero=[]
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='functions/functions.c':continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                zero.extend(value(e,'DW_AT_const_value') for e in die.iter_children() if value(e,'DW_AT_name')=='SF3_0')
    assert zero==[0];proof['zero_flags']=dict(SF3_0=0)
    definitions=read(root/'defs.h');anchor='\tSF3_REANIMATED';assert definitions.count(anchor)==1 and 'SF3_0' not in definitions
    definitions=definitions.replace(anchor,ZERO_DECL+anchor,1)
    original_source=read(root/'functions/functions.c')
    for name in [*TEXT_CHANGE_API_NAMES,'get_colors_to_hack_from_type_text','get_colors_to_hack_from_rules_text','get_colors_to_sleight_from_text']:
        proof['retained_source_helpers'].append(dict(name=name,body_sha256=sha(function_body(original_source,name).encode())))
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets.update({int(a,16):TEXT_CHANGE_API_NAMES[meta[a]['name']] for a in ADDRESSES})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    sources={'functions/functions.c':original_source};entries=[]
    for a in ADDRESSES:
        row=meta[a];name=TEXT_CHANGE_API_NAMES[row['name']];raw=read(directory/f'{a}.c');text=strip_comments(raw)
        assert text.count(ORIGINAL)==1;text=text.replace(ORIGINAL,REPLACEMENT)
        text,literals=recover_strings(text,runtime,{})
        body=format_c(clean(text,overlap,use_keyword_accessors_2022=True,use_extension_flags_2022=True,use_text_change_api_2022=True))
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),a
        symbols=[s for n,o,sec,k,s in debug.functions() if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+o==int(a,16)];assert symbols==[2]
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(row['high_pc'],16)-int(a,16));instructions=list(decoder.disasm(native,int(a,16)));by_site={f'{i.address:x}':i for i in instructions}
        assert all(by_site[s].bytes.hex()==data for s,data in SITES[a].items())
        calls=[]
        for ins in instructions:
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
                target=targets[ins.operands[0].imm];assert re.search(r'\b'+target+r'\s*\(',body),(a,target)
                calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=target))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),canonical_calls=calls,changes=[dict(original=ORIGINAL,replacement=REPLACEMENT,occurrences=1,
            kind='native full-word dialog result store with later low-byte reads',machine_sites=[dict(address=s,bytes=data) for s,data in SITES[a].items()])])
        proof['functions'][a]=detail
        e=dict(address=a,name=name,source=row['source'],line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),
            recovered_literals=literals,text_change_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode()))
        entries.append(e);sources[row['source']]+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n'+header[at:]
    scratch=cache/'text-change-check';scratch.mkdir(exist_ok=True);(scratch/'defs.h').write_bytes(definitions.encode('latin1'))
    compile_sources(root,scratch,sources,header)
    audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n),root,header,definitions)
    if args.apply:
        citations=m['source_fragments']+m['accepted_functions']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not set(ADDRESSES)&{c['address'] for c in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')}
        focused={r['address']:r for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
        for a in ADDRESSES:rows[a]=focused[a];shutil.copyfile(directory/f'{a}.c',cache/'decompiled-abi'/f'{a}.c')
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        (root/'defs.h').write_bytes(definitions.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=2;m['helper_functions']+=2;m['shared_helper_interfaces']+=2;m['source_files']['functions/functions.c']+=2
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()})
        save_json(mp,m);interfaces['text_change_helpers_2022']=proof;save_json(ip,interfaces)
        audit_component(proof,m,sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n),root)
    save_json(cache/'text-change-plan.json',proof)
    print(json.dumps(dict(applied=args.apply,new_implementations=2,new_public_helpers=2)))


if __name__=='__main__':main()
