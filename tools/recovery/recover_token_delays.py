"""Recover native haste/delayed-removal workers and 76-byte token callbacks."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image,value
from promote_recovery import TOKEN_DELAY_API_NAMES,strip_comments,UNRESOLVED
from recover_combat_mask_cards import cleaned
from recover_phase_prompt_cards import debug_evidence
from recover_test_consumers import read,rename
from recover_spell_copy import compile_sources
from recover_temporary_exile import save_json,sha
from audit_source import function_body

ADDRESSES='2678910 2678a80 2678aa0 2678ac0 26a0080 26a01a0 26a01e0'.split()
PRIVATE={'2678910','2678ac0'}
NAMES={**TOKEN_DELAY_API_NAMES,'haste_and_kill_at_eot':'haste_and_kill_at_eot_2022',
       'generic_haste_and_kill_at_eot_legacy':'generic_haste_and_kill_at_eot_legacy_2022'}
SOURCE='functions/functions.c'
HEADER_LABEL='/* October 2022 haste and delayed-removal APIs, including native token callbacks. */'


def bind_card(root,apply):
    from bind_delayed_callback_callers import audit_component as audit_callers
    cache=root/'.recovery';m=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    ip=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(ip.read_text());assert 'token_delay_card_callers_2022' not in interfaces
    a,target='2395ad0','26a01a0';entry=next(f for f in m['accepted_functions'] if f['address']==a)
    row=next(r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']=='0x'+a)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());native=runtime.pe.get_data(int(a,16)-0x2000000,int(row['high_pc'],16)-int(a,16))
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;name=NAMES['haste_and_sacrifice_token_at_end_of_next_turn']
    sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=target,name=name) for i in decoder.disasm(native,int(a,16)) if any(o.type==X86_OP_IMM and o.imm==int(target,16) for o in i.operands)]
    source=read(root/entry['source']);proof=dict(functions={a:dict(machine_bytes=len(native),machine_sha256=sha(native),sites=sites,body_sha256=sha(function_body(source,entry['name']).encode()))},scope='Chandra, Acolyte of Flame native token callback pointer binds to the recovered 76-byte haste/sacrifice frontend.')
    audit_callers(proof,m,{entry['source']:source},lambda a,n:runtime.pe.get_data(a-0x2000000,n),expected={a:target},targets={target:name})
    if apply:interfaces['token_delay_card_callers_2022']=proof;save_json(ip,interfaces)
    print(json.dumps(dict(applied=apply,bound_card_handlers=1,native_callback_operands=len(sites))))


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('token_delay_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert {a for a,f in entries.items() if f['static']}==PRIVATE
    assert component['enum_values']==dict(TRIGGER_EOT=205,STATE_OUBLIETTED=0x800000)
    worker_parameters=component['parameters']['2678910']
    assert [p['name'] for p in worker_parameters]==['player','card','event','kill_mode']
    assert [p['location'][0]['expression'] for p in worker_parameters[:3]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    assert worker_parameters[3]['location']=='(DW_OP_fbreg: 0)'
    assert [p['location'] for p in component['parameters']['26a0080']]==[f'(DW_OP_fbreg: {i})' for i in [0,4,8,12,16]]
    for name,digest in component['retained_bodies'].items():
        assert sha(function_body(sources[SOURCE].replace('\r\n','\n'),name).encode())==digest
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];body=function_body(sources[SOURCE].replace('\r\n','\n'),entry['name'])
        assert entry['token_delay_evidence']==detail and sha(body.encode())==entry['reconstruction_body_sha256']
        native=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(native)==detail['machine_sha256']
        insns={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        assert entry['name']==NAMES[component['native_names'][a]]
        assert len(component['parameters'][a])==({'2678910':4,'26a0080':5}.get(a,3))
        for change in detail['changes']:
            assert change['original'] in {'0xcd','0x800000'}
            for site in change['machine_sites']:
                ins=insns[site['address']];assert ins.bytes.hex()==site['bytes']
                expected=int(change['original'],16)
                assert any(o.type==X86_OP_IMM and o.imm==expected for o in ins.operands) or (
                    expected==0x800000 and ins.mnemonic=='test' and any(o.type==X86_OP_IMM and o.imm==0x80 for o in ins.operands)
                    and any(o.type==X86_OP_MEM and o.size==1 and o.mem.disp==10 for o in ins.operands))
        for call in detail['canonical_calls']:
            ins=insns[call['address']];assert ins.bytes.hex()==call['bytes'] and ins.mnemonic in {'call','jmp'}
            assert ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==int(call['target'],16)
            assert re.search(r'\b'+call['name']+r'\s*\(',body)
        for ref in detail['callback_references']:
            ins=insns[ref['address']];assert ins.bytes.hex()==ref['bytes']
            assert any(o.type==X86_OP_IMM and o.imm==int(ref['target'],16) for o in ins.operands)
            assert ref['name'] in body
        if a in {'26a01a0','26a01e0'}:
            assert 'token_generation_2022_t *token' in body
            assert [p['type'] for p in component['parameters'][a]]==['token_generation_t *','int','int']
        if a not in PRIVATE:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    constructor=function_body(sources[SOURCE],entries['26a0080']['name'])
    assert 'get_card_instance(player, -1)' in constructor and 'return -1;' in constructor
    assert {r['name'] for r in component['functions']['26a0080']['callback_references']}=={
        NAMES['haste_and_remove_eot'],NAMES['haste_and_sacrifice_eot'],NAMES['generic_haste_and_kill_at_eot_legacy']}


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--bind-card',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';directory=cache/'decompiled-token-delays'
    if args.bind_card:bind_card(root,args.apply);return
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'token_delay_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data)
    proof=dict(debug_sha256=sha(data),**debug_evidence(debug,meta,ADDRESSES),functions={},native_names={a:meta[a]['name'] for a in ADDRESSES},
        retained_bodies={n:sha(function_body(read(root/SOURCE),n).encode()) for n in NAMES},
        scope='Native delay/haste worker, three effect frontends, effect constructor and two 76-byte token callbacks under separate compatibility names. Preserve countdown/controller/phasing guards and the constructor branch which discards a legacy ID and uses -1; no gameplay corrections inferred.')
    enums={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=SOURCE:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for child in die.iter_children():
                    name=value(child,'DW_AT_name')
                    if name in {'TRIGGER_EOT','STATE_OUBLIETTED'}:enums[name]=value(child,'DW_AT_const_value')
    proof['enum_values']=enums;assert enums==dict(TRIGGER_EOT=205,STATE_OUBLIETTED=0x800000)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']}
    targets.update({int(a,16):NAMES[meta[a]['name']] for a in ADDRESSES})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    entries=[];bodies={}
    for a in ADDRESSES:
        raw=read(directory/f'{a}.c');text=strip_comments(raw);native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));insns=list(decoder.disasm(native,int(a,16)));changes=[]
        if a=='2678910':
            for literal,name in [('0xcd','TRIGGER_EOT'),('0x800000','STATE_OUBLIETTED')]:
                sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in insns if
                    any(o.type==X86_OP_IMM and o.imm==int(literal,16) for o in i.operands) or (
                    literal=='0x800000' and i.mnemonic=='test' and any(o.type==X86_OP_IMM and o.imm==0x80 for o in i.operands)
                    and any(o.type==X86_OP_MEM and o.size==1 and o.mem.disp==10 for o in i.operands))]
                assert text.count(literal)==1 and sites
                changes.append(dict(original=literal,replacement=name,occurrences=1,kind='native delay event/state masks',machine_sites=sites));text=text.replace(literal,name)
        text,_=rename(text,NAMES);body=cleaned(text,overlap)
        assert not UNRESOLVED.search(body) and not re.search(r'stack0x|\._\d|_conflict',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert storage==([3] if a in PRIVATE else [2]),(a,storage)
        if a in PRIVATE:body='static '+body
        calls=[];refs=[]
        for i in insns:
            for o in i.operands:
                if o.type!=X86_OP_IMM or o.imm not in targets:continue
                name=targets[o.imm]
                assert re.search(r'\b'+name+r'\b',body),(a,name)
                site=dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{o.imm:x}',name=name)
                (calls if i.mnemonic in {'call','jmp'} else refs).append(site)
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=calls,callback_references=refs)
        proof['functions'][a]=detail;bodies[a]=body
        entries.append(dict(address=a,name=NAMES[meta[a]['name']],source=SOURCE,line=meta[a]['line'],static=a in PRIVATE,
            prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],token_delay_evidence=detail,
            reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,NAMES[meta[a]['name']]).encode())))
    source=read(root/SOURCE)+'\n\n#ifndef SHANDALAR\n/* Recovered October 2022 haste/delayed-removal family. */\n'+'\n'.join(f['prototype'] for f in entries)+'\n'
    for entry in entries:source+=f'\n/* ManalinkEh 2022: 0x{entry["address"]}; original {SOURCE}:{entry["line"]}. */\n'+bodies[entry['address']]+'\n'
    source+='#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus')
    block=HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(f['prototype'] for f in entries if not f['static'])+'\n#endif\n\n'
    header=header[:at]+block+header[at:]
    scratch='#include "manalink.h"\n'+'\n'.join(f['prototype'] for f in entries)+'\n'+'\n'.join(bodies.values())
    compile_sources(root,cache/'token-delays-check',{SOURCE:scratch},header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},{SOURCE:source},lambda a,n:runtime.pe.get_data(a-0x2000000,n),root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])]
        assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')}
        for row in csv.DictReader((directory/'export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(directory/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        (root/SOURCE).write_bytes(source.replace('\n','\r\n').encode('latin1'));(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=7;m['helper_functions']+=7;m['private_functions']+=2;m['private_helpers']+=2;m['shared_helper_interfaces']+=5;m['source_files'][SOURCE]+=7
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()})
        save_json(mp,m);interfaces['token_delay_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'token-delays-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_helpers=7,private_helpers=2,public_helpers=5)))


if __name__=='__main__':main()
