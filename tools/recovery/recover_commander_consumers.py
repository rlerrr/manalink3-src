"""Recover the all-commanders specialization, command-zone wrapper and consumers."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value,inherited_value
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

SPECIAL='25a2780'
WRAPPER='22c69b0'
WAR_ROOM='25b16c0'
HELLKITE='25a88e0'
ADDRESSES=[SPECIAL,WRAPPER,WAR_ROOM,HELLKITE]
NAME='get_color_identity_of_all_commanders_2022'
WRAPPER_NAME='put_commander_from_command_zone_to_2022'
HEADER_LABEL='/* October 2022 command-zone destination wrapper. */'
SPECIAL_SITES={'25a2788':'89c6','25a278d':'bb01000000','25a27ad':'83fb03'}
CALL_SITES={'25b1738':'89f0','25b173a':'e84110ffff','25b17d9':'89f0','25b17db':'e8a00fffff'}
LIFE_SITES={'25b17fc':'83ff05','25b17ff':'0f9fc0','25b1809':'29c2','25b1884':'83ff05','25b1887':'0f9fc2','25b18d1':'29d1','25b18d3':'890cb528f54e00'}


def reconstruct(a,raw):
    text=strip_comments(raw);changes=[]
    substitutions=[]
    if a==SPECIAL:substitutions=[('int __regparm1 get_color_identity_of_commander(int player,int which_commander)',f'int {NAME}(int player)')]
    if a==WAR_ROOM:substitutions=[('get_color_identity_of_commander(player,iVar2)',NAME+'(player)'),('  iVar2 = player;\n','')]
    if a==WRAPPER:substitutions=[('int put_commander_from_command_zone_to(int player,int where)',f'int {WRAPPER_NAME}(int player,int where)')]
    if a==HELLKITE:substitutions=[('put_commander_from_command_zone_to(player,4)',WRAPPER_NAME+'(player,4)')]
    for old,new in substitutions:
        count=text.count(old);assert count==(2 if a==WAR_ROOM and old.startswith('get_') else 1)
        sites=SPECIAL_SITES if a==SPECIAL else CALL_SITES if a==WAR_ROOM else {'22c69c3':'e9c8fbffff'} if a==WRAPPER else {'25a892b':'e880e0d1ff'}
        changes.append(dict(original=old,replacement=new,occurrences=count,kind='recover specialized commander interfaces while retaining legacy private functions',machine_sites=[dict(address=a,bytes=b) for a,b in sites.items()]))
        text=text.replace(old,new)
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('commander_consumer_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['specialization']==dict(native_symbol='_get_color_identity_of_commander.constprop.0',which_commander=3,physical_player='EAX',source_name=NAME)
    p=component['parameters'][SPECIAL];assert p[0]['name']=='player' and p[0]['location'][0]['expression']=='(DW_OP_reg0 (r0))' and p[1]['name']=='which_commander' and p[1]['location'] is None
    assert len(component['parameters'][WRAPPER])==2
    assert component['retained_core']==dict(address='22c6590',name='put_commander_from_command_zone_to_impl',source='cards/vanguard.c',static=True)
    core=next(f for f in manifest['accepted_functions'] if f['address']=='22c6590');assert all(core[k]==v for k,v in component['retained_core'].items())
    legacy=component['legacy_wrapper'];assert legacy['source']=='cards/commander_2015.c' and legacy['name']=='put_commander_from_command_zone_to'
    assert sha(function_body(read(root/legacy['source']),legacy['name']).encode())==legacy['body_sha256']
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['commander_consumer_evidence']==detail
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-commander-consumers' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'] and function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256']
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        assert detail['native_coff_storage']==(3 if a==SPECIAL else 2)
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body)
        if a==SPECIAL:assert body.startswith(NAME+'(int player)') and 'which_commander' not in body and 'commander_code != 3' in body
        if a==WAR_ROOM:
            assert body.count(NAME+'(player)')==2 and '(5 < (int)uVar1)' in body
            for at,expected in LIFE_SITES.items():assert machine_bytes(int(at,16),len(bytes.fromhex(expected))).hex()==expected
        if a==WRAPPER:
            assert detail['machine_bytes']==24 and 'put_commander_from_command_zone_to_impl(player, where, 0, NULL)' in body
            assert machine_bytes(0x22c69b1,2).hex()=='31c9' and machine_bytes(0x22c69bb,7).hex()=='c7450800000000'
    assert {r['target'] for r in component['functions'][WRAPPER]['canonical_calls']}=={'22c6590'}
    assert any(r['target']==WRAPPER for r in component['functions'][HELLKITE]['canonical_calls'])
    assert any(r['target']=='25a2020' for r in component['functions'][HELLKITE]['callback_references'])
    assert entries[WRAPPER]['prototype'] in read(root/'manalink.h') or component.get('planning')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'commander_consumers_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),**debug_evidence(debug,meta,ADDRESSES),functions={},
        specialization=dict(native_symbol='_get_color_identity_of_commander.constprop.0',which_commander=3,physical_player='EAX',source_name=NAME),
        retained_core=dict(address='22c6590',name='put_commander_from_command_zone_to_impl',source='cards/vanguard.c',static=True),
        legacy_wrapper=dict(source='cards/commander_2015.c',name='put_commander_from_command_zone_to',body_sha256=sha(function_body(read(root/'cards/commander_2015.c'),'put_commander_from_command_zone_to').encode())),
        scope='Recover the native both-commanders color specialization with one physical input, its two War Room callers, the public command-zone wrapper and Hellkite Courser. Preserve native War Room mana costs, mode bytes, AI branches and the extra life subtraction of colors > 5. Reuse the already recovered command-zone core and Hellkite delayed callback.')
    cu=next(c for c in debug.dwarf().iter_CUs() if value(c.get_top_DIE(),'DW_AT_name')=='cards/commander_legends.c');die=next(d for d in cu.get_top_DIE().iter_children() if value(d,'DW_AT_low_pc')==int(SPECIAL,16));param=next(p for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter' and inherited_value(p,'DW_AT_name')=='which_commander');assert value(param,'DW_AT_const_value')==3
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAME if a==SPECIAL else WRAPPER_NAME if a==WRAPPER else meta[a]['name'],source=meta[a]['source'],static=a==SPECIAL) for a in ADDRESSES})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=NAME if a==SPECIAL else WRAPPER_NAME if a==WRAPPER else row['name'];raw=read(cache/'decompiled-commander-consumers'/f'{a}.c');text,changes=reconstruct(a,raw);text,literals=recover_strings(text,runtime,{});body=cleaned(text,overlap)
        if a==SPECIAL:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict',body),a
        symbols=[(n,s) for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert len(symbols)==1 and symbols[0][1]==(3 if a==SPECIAL else 2),(a,symbols)
        if a==SPECIAL:assert symbols[0][0]==proof['specialization']['native_symbol']
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=symbols[0][1])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail
        entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a==SPECIAL,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,commander_consumer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+next(e['prototype'] for e in entries if e['address']==WRAPPER)+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'commander-consumers-check',{**sources,'cards/commander_2015.c':read(root/'cards/commander_2015.c')},header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-commander-consumers/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-commander-consumers'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=4;m['card_handler_functions']+=2;m['helper_functions']+=2;m['private_functions']+=1;m['private_helpers']+=1;m['shared_helper_interfaces']+=1
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['commander_consumers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'commander-consumers-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=2,new_helpers=2,private_helpers=1)))


if __name__=='__main__':main()
