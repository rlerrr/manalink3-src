"""Recover native activation costs, permission queries and granted ability core."""
import argparse,csv,hashlib,json,re,shutil,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from elftools.dwarf.locationlists import LocationParser,LocationExpr,BaseAddressEntry
from elftools.dwarf.descriptions import describe_DWARF_expr
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import clean,strip_comments,UNRESOLVED
from recover_test_consumers import read,format_c,rename
from recover_literals import recover_strings
from audit_source import function_body
from activated_core_bodies import STORAGE,reconstruct

ADDRESSES='269c8c0 269db00 269e690 269e810 26901c0 268e850 268e9e0 26dd340 26dd380 26dd3a0 269e2b0 2690310 26888a0'.split()
ADDED_FLAG_MEMBERS=dict(GAA_DURING_COMBAT=4096,GAA_MANA_ABILITY=2097152)

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    ap=argparse.ArgumentParser(__doc__);ap.add_argument('--apply',action='store_true');args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';m=json.loads(mp.read_text());assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    directories={a:cache/('decompiled-activation-cost' if a=='26888a0' else 'decompiled-activated-core') for a in ADDRESSES}
    names={a:meta[a]['name']+'_2022' for a in ADDRESSES};mapping={meta[a]['name']:n for a,n in names.items()}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);dw=debug.dwarf();lp=LocationParser(dw.location_lists());parameters={};locals_by_address={};enums={};flags=None;mana_payment=None
    for global_cu in dw.iter_CUs():
        for die in global_cu.get_top_DIE().iter_children():
            if die.tag!='DW_TAG_variable' or inherited_value(die,'DW_AT_name')!='pay_mana_xbugrwaU':continue
            typ=type_of(die);assert type_name(typ)=='int[8]' and value(type_of(typ),'DW_AT_byte_size')==4
            symbols=[dict(name=n,address=f'{a:x}',storage=s) for n,a,sec,k,s in debug.symbols if sec==-1 and n=='_pay_mana_xbugrwaU']
            assert symbols==[dict(name='_pay_mana_xbugrwaU',address='4ef400',storage=2)]
            mana_payment=dict(type='int[8]',elements=8,element_bytes=4,bytes=32,symbols=symbols)
            break
        if mana_payment:break
    for cu in dw.iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name') not in {'functions/functions.c','functions/abilities.c'}:continue
        def location(p):
            attr=p.attributes.get('DW_AT_location')
            if attr is None:return None
            loc=lp.parse_from_attribute(attr,cu['version'],die=p)
            if isinstance(loc,LocationExpr):return describe_DWARF_expr(loc.loc_expr,dw.structs)
            return [dict(base=hex(x.base_address)) if isinstance(x,BaseAddressEntry) else dict(start=hex(x.begin_offset),end=hex(x.end_offset),expression=describe_DWARF_expr(x.loc_expr,dw.structs)) for x in loc]
        def locals(die):
            out=[]
            for c in die.iter_children():
                if c.tag=='DW_TAG_variable':out.append(dict(name=inherited_value(c,'DW_AT_name'),type=type_name(type_of(c)),location=location(c)))
                elif c.tag in {'DW_TAG_lexical_block','DW_TAG_inlined_subroutine'}:out.extend(locals(c))
            return out
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_variable' and inherited_value(die,'DW_AT_name')=='pay_mana_xbugrwaU':
                typ=type_of(die);assert type_name(typ)=='int[8]' and value(type_of(typ),'DW_AT_byte_size')==4
                symbols=[dict(name=n,address=f'{a:x}',storage=s) for n,a,sec,k,s in debug.symbols if sec==-1 and n=='_pay_mana_xbugrwaU']
                assert symbols==[dict(name='_pay_mana_xbugrwaU',address='4ef400',storage=2)]
                mana_payment=dict(type='int[8]',elements=8,element_bytes=4,bytes=32,symbols=symbols)
            if die.tag=='DW_TAG_enumeration_type':
                members={value(c,'DW_AT_name'):value(c,'DW_AT_const_value') for c in die.iter_children() if c.tag=='DW_TAG_enumerator'};enums.update(members)
                if 'GAA_CAN_TARGET' in members:
                    item=dict(bytes=value(die,'DW_AT_byte_size'),members=members);assert flags is None or flags==item;flags=item
            pc=value(die,'DW_AT_low_pc');address=f'{pc:x}' if pc else ''
            if address not in ADDRESSES:continue
            parameters[address]=[dict(name=inherited_value(p,'DW_AT_name'),type=type_name(type_of(p)),location=location(p),constant=value(p,'DW_AT_const_value')) for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter']
            locals_by_address[address]=locals(die)
    assert set(parameters)==set(ADDRESSES) and len(parameters['269c8c0'])==15 and len(parameters['269db00'])==13 and mana_payment
    local={(p['name'],p['type'],p['location']) for p in locals_by_address['269c8c0'] if isinstance(p['location'],str)}
    assert {('td_default','target_definition_t','(DW_OP_fbreg: -236)'),('test','test_definition_t','(DW_OP_fbreg: -236)'),('tgt','target_t','(DW_OP_fbreg: -236)')}<=local
    assert flags['bytes']==4 and flags['members']['GAA_NONE']==0 and flags['members']['GAA_NONSICK']==0 and enums['SF_0']==enums['SF2_0']==0
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    def code(a):return runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16))
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets.update({int(a,16):n for a,n in names.items()})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    assert {n:flags['members'][n] for n in ADDED_FLAG_MEMBERS}==ADDED_FLAG_MEMBERS
    proof=dict(debug_sha256=sha(debug_data),parameters=parameters,locals=locals_by_address,flags=flags,added_flag_members=ADDED_FLAG_MEMBERS,zero_flags=dict(SF_0=0,SF2_0=0),enum_values={},storage_declaration=STORAGE,mana_payment_array=mana_payment,functions={},
        scope='Native granted and generic activated-ability core, attachment/equipment wrappers, 84-byte targets, shared 216-byte test/target/selected-pair storage, activation permission and cost/mana queries, resolving-cost wrapper, creature pump frontend and shade amount helper. Reuse the verified unchanged GAA flag domain.',
        integration_pending='Specialized sacrifice/graveyard/tap-subtype cost wrappers, earlier aggregate callers and engine/lifecycle integration remain pending. No native DLL build or gameplay execution performed.')
    entries=[];bodies={}
    for address,name in names.items():
        raw=(directories[address]/(address+'.c')).read_text();native=code(address);instructions=list(decoder.disasm(native,int(address,16)))
        assert any(s==2 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+off==int(address,16) for n,off,sec,k,s in debug.functions()),address
        text,changes=reconstruct(address,strip_comments(raw),instructions)
        proof['enum_values'].update({n:enums[n] for n in set(re.findall(r'\b(?:EVENT|AL|SF|SF2|COUNTER)_\w+\b',text)) if n in enums})
        text,_=rename(text,mapping);text,literals=recover_strings(text,runtime,{})
        body=clean(text,overlap,use_test_api_2022=True,use_target_api_2022=True,use_sacrifice_api_2022=True,use_spell_api_2022=True,use_extension_flags_2022=True,use_keyword_accessors_2022=True,use_ability_api_2022=True,use_pump_api_2022=True)
        assert not UNRESOLVED.search(body) and not re.search(r'\._\d|stack0x',body),address
        body=format_c(body);bodies[address]=body;calls=[]
        for i in instructions:
            if i.mnemonic in {'call','jmp'} and i.operands[0].type==X86_OP_IMM and i.operands[0].imm in targets:
                destination=targets[i.operands[0].imm];assert re.search(r'\b'+destination+r'\s*\(',body),(address,destination)
                calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{i.operands[0].imm:x}',name=destination))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls);proof['functions'][address]=detail
        entries.append(dict(name=name,address=address,source=meta[address]['source'],line=meta[address]['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,
            activated_core_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    sources={s:read(root/s) for s in {e['source'] for e in entries}}
    for source in sources:
        sources[source]+='\n\n#ifndef SHANDALAR\n/* Recovered October 2022 activated-ability core. */\n'
        if source=='functions/functions.c':sources[source]+=STORAGE+'\n'
        for e in entries:
            if e['source']==source:sources[source]+=f"\n/* ManalinkEh 2022: 0x{e['address']}; original {source}:{e['line']}. */\n"+bodies[e['address']]+'\n'
        sources[source]+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+'/* October 2022 activated-ability core APIs. */\n#ifndef SHANDALAR\nextern int pay_mana_xbugrwaU[8];\n'+'\n'.join(e['prototype'] for e in entries)+'\n#endif\n\n'+header[at:]
    defs=read(root/'defs.h')
    for name,value,anchor in [('GAA_DURING_COMBAT',12,'GAA_ONCE_PER_TURN'),('GAA_MANA_ABILITY',21,'GAA_CAN_ONLY_TARGET_OPPONENT')]:
        if re.search(r'\b'+name+r'\s*=',defs):continue
        at=defs.index('\t'+anchor);defs=defs[:at]+f'\t{name} = 1<<{value}, /* October 2022 DWARF; previously unnamed bit. */\n'+defs[at:]
    scratch=cache/'activated-core-check';scratch.mkdir(exist_ok=True);(scratch/'manalink.h').write_bytes(header.encode('latin1'));(scratch/'defs.h').write_bytes(defs.encode('latin1'))
    for source,content in sources.items():
        target=scratch/Path(source).name;target.write_bytes(content.encode('latin1'))
        r=subprocess.run(['C:/Program Files/LLVM/bin/clang.exe','-target','i686-pc-windows-msvc','-std=gnu99','-fsyntax-only','-ferror-limit=0','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-Werror=return-type','-Werror=uninitialized','-Werror=sometimes-uninitialized','-Werror=format','-I',str(scratch),'-I',str(root),str(target)],capture_output=True,text=True)
        (scratch/(Path(source).stem+'.log')).write_text(r.stderr)
        if r.returncode:print(r.stderr)
        r.check_returncode()
    export_path=cache/'decompiled-abi/export.tsv';rows=list(csv.DictReader(export_path.open(),delimiter='\t'));existing={r['address'] for r in rows};focused={r['address']:r for directory in set(directories.values()) for r in csv.DictReader((directory/'export.tsv').open(),delimiter='\t')}
    rows=[focused[r['address']] if r['address'] in names else r for r in rows];rows.extend(focused[a] for a in names if a not in existing)
    if args.apply:
        citations=m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        for citation in citations:
            a=citation['address']
            if a not in names or citation.get('raw_export'):continue
            old=export_path.parent/(a+'.c')
            if sha(old.read_text().encode())==sha((directories[a]/(a+'.c')).read_text().encode()):continue
            support=cache/'decompiled-support'/(a+'-'+citation['raw_sha256']+'.c');support.parent.mkdir(exist_ok=True);shutil.copyfile(old,support);citation['raw_export']=support.relative_to(cache).as_posix()
        for source,content in sources.items():(root/source).write_bytes(content.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        (root/'defs.h').write_bytes(defs.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=len(entries);m['helper_functions']+=len(entries);m['shared_helper_interfaces']+=len(entries)
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows});mp.write_text(json.dumps(m,indent=2))
        path=root/'docs/recovery-evidence/interface-recovery.json';i=json.loads(path.read_text());i['activated_core_2022']=proof;path.write_text(json.dumps(i,indent=2))
        with export_path.open('w',newline='') as stream:
            w=csv.DictWriter(stream,fieldnames=rows[0],delimiter='\t');w.writeheader();w.writerows(rows)
        for a in names:shutil.copyfile(directories[a]/(a+'.c'),export_path.parent/(a+'.c'))
    print(json.dumps(dict(applied=args.apply,new_helpers=len(entries),private_helpers=0,shared_storage_bytes=216)))

if __name__=='__main__':main()
