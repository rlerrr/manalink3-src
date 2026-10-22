"""Recover Norritt with one explicit prompt-qualification bridge at the native ABI."""
import argparse,csv,hashlib,json,re,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value,inherited_value
from elftools.dwarf.descriptions import describe_DWARF_expr
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,format_c
from recover_literals import recover_strings
from audit_source import function_body

ADDRESS='2403af0'
ORIGINAL='generic_activated_ability(player,card,EVENT_ACTIVATE,7,0,0,0,0,0,0,0,&td2,prompt)'
REPLACEMENT=ORIGINAL.replace('&td2,prompt)','&td2,(char *)prompt)')
SITES={'240400f':'e85cc22700','240401c':'89442430','2404071':'e88a9a2900'}


def sha(data):return hashlib.sha256(data).hexdigest()


def audit_component(component,manifest,sources,machine_bytes):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entry=next(f for f in manifest['accepted_functions'] if f['address']==ADDRESS)
    detail=component['function']
    require(entry['norritt_prompt_evidence']==detail and entry['name']=='card_norritt' and not entry['static'],'Norritt evidence/linkage mismatch')
    require(component['target_locals']==[dict(name='td',type='target_definition_t',location='(DW_OP_fbreg: -188)'),
            dict(name='td2',type='target_definition_t',location='(DW_OP_fbreg: -104)')],'Norritt native target locals mismatch')
    require(len(detail['changes'])==1 and detail['changes'][0]['original']==ORIGINAL and detail['changes'][0]['replacement']==REPLACEMENT,
            'Norritt prompt bridge mismatch')
    require({s['address']:s['bytes'] for s in detail['changes'][0]['machine_sites']}==SITES,'Norritt native prompt transfer evidence mismatch')
    body=function_body(sources[entry['source']],entry['name'])
    require(body.count('(char *)prompt')==1 and 'const char *prompt;' in body,'Norritt prompt qualifier bridge missing')
    for support in component['source_interfaces']:
        require(sha(function_body(sources[support['source']].replace('\r\n','\n'),support['name']).encode())==support['body_sha256'],
                'Norritt supporting source interface changed')
    # The native helper return in EAX is placed unchanged in outgoing argument 12.
    for address,data in SITES.items():require(machine_bytes(int(address,16),len(bytes.fromhex(data))).hex()==data,'Norritt prompt instruction mismatch')
    require(len(detail['function_references'])==1 and detail['function_references'][0]['target']=='24bc200' and
            detail['function_references'][0]['name']=='effect_nettling_imp_2022','Norritt callback binding mismatch')
    for reference in detail['function_references']:
        data=bytes.fromhex(reference['bytes'])
        require(machine_bytes(int(reference['address'],16),len(data))==data and 'effect_nettling_imp_2022' in body,'Norritt callback pointer instruction mismatch')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'norritt_prompt_2022' not in interfaces
    assert ADDRESS not in {f['address'] for f in m['accepted_functions']}
    row=next(r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']=='0x'+ADDRESS)
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dw=debug.dwarf();locals_=[]
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=row['source']:continue
        for die in cu.get_top_DIE().iter_children():
            if value(die,'DW_AT_low_pc')!=int(ADDRESS,16):continue
            locals_=[dict(name=inherited_value(v,'DW_AT_name'),type=type_name(type_of(v)),location=describe_DWARF_expr(v.attributes['DW_AT_location'].value,dw.structs))
                for v in die.iter_children() if v.tag=='DW_TAG_variable' and inherited_value(v,'DW_AT_name') in {'td','td2'}]
    symbols=[s for n,o,sec,k,s in debug.functions() if debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+o==int(ADDRESS,16)];assert symbols==[2]
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());native=runtime.pe.get_data(int(ADDRESS,16)-0x2000000,int(row['high_pc'],16)-int(ADDRESS,16))
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;instructions=list(decoder.disasm(native,int(ADDRESS,16)))
    raw=(cache/'decompiled-abi'/f'{ADDRESS}.c').read_text();text=strip_comments(raw);assert text.count(ORIGINAL)==1
    text=text.replace(ORIGINAL,REPLACEMENT);text,literals=recover_strings(text,runtime,{})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    body=format_c(clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,use_activated_api_2022=True,
        use_delayed_callback_api_2022=True,use_keyword_accessors_2022=True,use_extension_flags_2022=True))
    assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body)
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};calls=[];references=[]
    for ins in instructions:
        if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
            target=targets[ins.operands[0].imm];assert re.search(r'\b'+target+r'\s*\(',body),target
            calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=target))
        if any(o.type==X86_OP_IMM and o.imm==0x24bc200 for o in ins.operands):
            references.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target='24bc200',name='effect_nettling_imp_2022'))
    detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=[dict(original=ORIGINAL,replacement=REPLACEMENT,occurrences=1,
        kind='explicit pointer qualifier at native prompt ABI boundary',machine_sites=[dict(address=a,bytes=b) for a,b in SITES.items()])],canonical_calls=calls,function_references=references)
    source=row['source'];sources={source:read(root/source),'functions/functions.c':read(root/'functions/functions.c')}
    proof=dict(debug_sha256=sha(debug_data),target_locals=locals_,function=detail,source_interfaces=[],
        scope='Native Norritt with recovered target/activation APIs and the public Nettling Imp callback. Cast only the color-text result at outgoing prompt argument 12, preserving the original pointer without copying or modifying characters. The source facade returns const char*, while the recovered native activation API has char*.',
        integration_pending='Native linking and gameplay verification remain pending.')
    for name in ['get_sleighted_color_text','generic_activated_ability_2022','granted_generic_activated_ability_2022']:
        proof['source_interfaces'].append(dict(name=name,source='functions/functions.c',body_sha256=sha(function_body(sources['functions/functions.c'],name).encode())))
    entry=dict(address=ADDRESS,name='card_norritt',source=source,line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),
        recovered_literals=literals,norritt_prompt_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,'card_norritt').encode()))
    sources[source]+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{ADDRESS}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    scratch=cache/'norritt-check';scratch.mkdir(exist_ok=True);path=scratch/'ice_age.c';path.write_bytes(sources[source].encode('latin1'))
    result=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0',
        '-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format',
        '-I',str(root),str(path)],capture_output=True,text=True);path.with_suffix('.log').write_text(result.stderr)
    if result.returncode:print(result.stderr[-6000:])
    result.check_returncode();audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+[entry]},sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if args.apply:
        (root/source).write_bytes(sources[source].replace('\n','\r\n').encode('latin1'));m['accepted_functions'].append(entry);m['accepted']+=1;m['card_handler_functions']+=1;m['source_files'][source]+=1
        registrations=list(csv.DictReader((cache/'2022-registrations.csv').open()));names={f['name'] for f in m['accepted_functions']}
        m['registered_handler_symbols']=len({r['symbol'].lstrip('_') for r in registrations if r['symbol'].lstrip('_') in names})
        m['registered_handler_symbols_marked_coded']=len({r['symbol'].lstrip('_') for r in registrations if r['coded']=='True' and r['symbol'].lstrip('_') in names})
        mp.write_text(json.dumps(m,indent=2));interfaces['norritt_prompt_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
    (cache/'norritt-plan.json').write_text(json.dumps(proof,indent=2));print(json.dumps(dict(applied=args.apply,new_card_functions=1)))


if __name__=='__main__':main()
