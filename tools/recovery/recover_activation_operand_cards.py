"""Recover four handlers blocked by address-like activation flag/cost words."""
import argparse,csv,hashlib,json,re,subprocess,collections
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image,value
from promote_recovery import clean,strip_comments,UNRESOLVED,rewrite_argument
from recover_test_consumers import read,format_c
from recover_literals import recover_strings
from audit_source import function_body

CONFIG={
 '20c1380':(10,0x2020800,'GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 8)',dict(PCOUNTER_ENERGY=2)),
 '22e9800':(3,0x400000,'GAA_CAN_ONLY_TARGET_OPPONENT',dict(GAA_CAN_ONLY_TARGET_OPPONENT=0x400000)),
 '2109f60':(3,0x400001,'(GAA_UNTAPPED | GAA_CAN_ONLY_TARGET_OPPONENT)',dict(GAA_UNTAPPED=1,GAA_CAN_ONLY_TARGET_OPPONENT=0x400000)),
 '2025030':(3,0x400018,'(GAA_SACRIFICE_ME | GAA_CAN_SORCERY_BE_PLAYED | GAA_CAN_ONLY_TARGET_OPPONENT)',dict(GAA_SACRIFICE_ME=8,GAA_CAN_SORCERY_BE_PLAYED=16,GAA_CAN_ONLY_TARGET_OPPONENT=0x400000))}


def sha(data):return hashlib.sha256(data).hexdigest()


def audit_component(component,manifest,sources,machine_bytes):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('activation_operand_evidence')}
    require(set(entries)==set(CONFIG)==set(component['functions']),'Activation operand card membership mismatch')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for address,(argument,number,expression,enums) in CONFIG.items():
        detail=component['functions'][address];entry=entries[address]
        require(entry['activation_operand_evidence']==detail and not entry['static'],'Activation operand evidence/linkage mismatch')
        require(component['unit_enums'][entry['source']]==enums,'Activation operand native enum mismatch')
        require(detail['argument']==argument and detail['value']==number and detail['replacement']==expression,
                'Activation operand decoding mismatch')
        body=function_body(sources[entry['source']],entry['name'])
        require(expression in body,'Activation operand source expression missing')
        instructions=list(decoder.disasm(machine_bytes(int(address,16),detail['machine_bytes']),int(address,16)))
        by_site={f'{i.address:x}':i for i in instructions}
        require(bool(detail['changes'][0]['machine_sites']),'Activation operand proof missing')
        for site in detail['changes'][0]['machine_sites']:
            ins=by_site[site['address']]
            require(ins.bytes.hex()==site['bytes'] and ins.mnemonic=='mov' and ins.operands[0].type==X86_OP_MEM and
                    ins.reg_name(ins.operands[0].mem.base)=='esp' and ins.operands[0].mem.disp==argument*4 and
                    ins.operands[0].size==4 and ins.operands[1].type==X86_OP_IMM and ins.operands[1].imm==number,
                    'Activation operand native argument store mismatch')
            call=next(i for i in instructions if i.address>ins.address and i.mnemonic=='call')
            require(call.operands[0].type==X86_OP_IMM and call.operands[0].imm==0x269db00,
                    'Activation operand native consumer mismatch')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text())
    assert 'activation_operand_cards_2022' not in interfaces and not set(CONFIG)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data)
    wanted={meta[a]['source']:set(c[3]) for a,c in CONFIG.items()};unit_enums={s:{} for s in wanted}
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in wanted:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for child in die.iter_children():
                    name=value(child,'DW_AT_name')
                    if name in wanted[source]:unit_enums[source][name]=value(child,'DW_AT_const_value')
    assert all(unit_enums[meta[a]['source']]==c[3] for a,c in CONFIG.items())
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    proof=dict(debug_sha256=sha(debug_data),unit_enums=unit_enums,functions={},
        scope='Four native handlers with verified outgoing activation mode/cost argument stores. Preserve every flag and packed cost byte, use recovered native aggregate frontends, and retain all native branches and returns.',
        integration_pending='Native linking and gameplay verification remain pending.')
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']}
    overlap=[x.split('\t')[1] for x in (cache/'absolute-interfaces.tsv').read_text().splitlines() if x.split('\t')[2]=='global']
    entries=[];sources={}
    for address,(argument,number,expression,enums) in CONFIG.items():
        row=meta[address];name=row['name'];source=row['source'];raw=(cache/'decompiled-abi'/(address+'.c')).read_text();text=strip_comments(raw)
        token=f'0x{number:x}';count=text.count(token)
        changed=rewrite_argument(text,'generic_activated_ability',13,argument,{token:expression})
        assert count>0 and changed.count(token)==0 and changed.count(expression)==count,address
        native=runtime.pe.get_data(int(address,16)-0x2000000,int(row['high_pc'],16)-int(address,16))
        instructions=list(decoder.disasm(native,int(address,16)))
        sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in instructions if i.mnemonic=='mov' and len(i.operands)==2 and
            i.operands[0].type==X86_OP_MEM and i.reg_name(i.operands[0].mem.base)=='esp' and i.operands[0].mem.disp==argument*4 and
            i.operands[0].size==4 and i.operands[1].type==X86_OP_IMM and i.operands[1].imm==number]
        assert len(sites)==count,(address,sites,count)
        changes=[dict(original=token,replacement=expression,occurrences=count,kind='verified native activation argument',machine_sites=sites)]
        changed,literals=recover_strings(changed,runtime,{})
        body=format_c(clean(changed,overlap,use_test_api_2022=True,use_target_api_2022=True,use_extension_flags_2022=True,
            use_keyword_accessors_2022=True,use_ability_api_2022=True,use_pump_api_2022=True,use_activated_api_2022=True,
            use_attack_api_2022=True,use_entry_api_2022=True,use_specific_cip_api_2022=True))
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        calls=[]
        for ins in instructions:
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
                target=targets[ins.operands[0].imm];assert re.search(r'\b'+target+r'\s*\(',body),(address,target)
                calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=target))
        symbols=[s for n,off,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16)]
        assert symbols==[2],(address,symbols)
        detail=dict(argument=argument,value=number,replacement=expression,machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls)
        proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=source,line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',
            raw_sha256=sha(raw.encode()),recovered_literals=literals,activation_operand_evidence=detail,reconstruction=proof['scope'],
            reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources.setdefault(source,read(root/source));sources[source]+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{address}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    scratch=cache/'activation-operand-cards-check';scratch.mkdir(exist_ok=True)
    for source,text in sources.items():
        path=scratch/Path(source).name;path.write_bytes(text.encode('latin1'))
        r=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0',
            '-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized',
            '-Werror=sometimes-uninitialized','-Werror=format','-I',str(root),str(path)],capture_output=True,text=True)
        path.with_suffix('.log').write_text(r.stderr)
        if r.returncode:print(r.stderr[-6000:])
        r.check_returncode()
    audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,
                    lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if args.apply:
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=4;m['card_handler_functions']+=4
        for entry in entries:
            m['source_files'][entry['source']]=m['source_files'].get(entry['source'],0)+1
            m.get('deferred',{}).pop(entry['name'],None)
        m['deferred_reason_counts']=dict(collections.Counter(reason.split(':')[0] for reason in m.get('deferred',{}).values()))
        registrations=list(csv.DictReader((cache/'2022-registrations.csv').open()));names={f['name'] for f in m['accepted_functions']}
        m['registered_handler_symbols']=len({r['symbol'].lstrip('_') for r in registrations if r['symbol'].lstrip('_') in names})
        m['registered_handler_symbols_marked_coded']=len({r['symbol'].lstrip('_') for r in registrations if r['coded']=='True' and r['symbol'].lstrip('_') in names})
        mp.write_text(json.dumps(m,indent=2));interfaces['activation_operand_cards_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
    (cache/'activation-operand-cards-plan.json').write_text(json.dumps(proof,indent=2));print(json.dumps(dict(applied=args.apply,new_card_functions=4)))


if __name__=='__main__':main()
