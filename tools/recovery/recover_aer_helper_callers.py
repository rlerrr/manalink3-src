"""Recover the two-register Expertise helper and bind fifteen native consumers."""
import argparse,copy,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_REG,X86_REG_EDX
from inspect_release import Image,value,inherited_value
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from migrate_token_callback_records import source_body
from audit_source import function_body

HELPER='20bc560'
IMPROVISE='20bc670'
FREE_CALLERS={'20c4190':2,'20c8930':5,'20ca3b0':3,'20ccd20':3}
IMPROVISE_CALLERS='20c0840 20c0ef0 20c1000 20c11a0 20c1c90 20c4ec0 20c5720 20c87d0 20c9e40 20ca520 20cc910'.split()
CALLERS=list(FREE_CALLERS)+IMPROVISE_CALLERS
SOURCE='cards/aether_revolt.c'
NAME='av_free_spell_2022'
FORWARD='#ifndef SHANDALAR\nstatic void av_free_spell_2022(int player, int max_cmc);\n#endif\n'
HELPER_SITES={'20bc565':'89c6','20bc574':'89d3','20bc576':'43','20bc5b2':'66c745880100','20bc5b8':'895d9c','20bc5bb':'66c745a40300'}


def historical_view(entry,body,binding=None):
    migration=entry['aer_helper_caller_migration_2022'];previous=migration['previous_record'];body=body.replace('\r\n','\n')
    assert sha(body.encode())==migration['body_sha256']
    for change in reversed(migration['source_changes']):
        assert body.count(change['replacement'])==change['occurrences'];body=body.replace(change['replacement'],change['original'])
    assert sha(body.encode())==migration['previous_body_sha256']
    if binding:assert entry[binding]==previous[binding]
    return previous,body


def specialization(debug):
    cu=next(c for c in debug.dwarf().iter_CUs() if value(c.get_top_DIE(),'DW_AT_name')==SOURCE)
    die=next(d for d in cu.get_top_DIE().iter_children() if value(d,'DW_AT_low_pc')==int(HELPER,16));card=next(p for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter' and inherited_value(p,'DW_AT_name')=='card')
    assert value(card,'DW_AT_location') is None and value(card,'DW_AT_const_value') is None
    symbols=[(n,s) for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(HELPER,16)]
    assert symbols==[('_av_free_spell.isra.0.part.0',3)],symbols
    return dict(native_symbol=symbols[0][0],physical_parameters=2,player='EAX',max_cmc='EDX',abstract_card='no physical location',test_bytes=216)


def native_calls(code,a,target,name,body):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;instructions=list(decoder.disasm(code,int(a,16)));calls=[]
    for at,i in enumerate(instructions):
        if i.mnemonic not in {'call','jmp'} or i.operands[0].type!=X86_OP_IMM or i.operands[0].imm!=int(target,16):continue
        site=dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=target,name=name)
        if a in FREE_CALLERS:
            assert i.mnemonic=='call';setup=instructions[at-2:at]
            assert len(setup)==2 and setup[0].mnemonic=='mov' and setup[0].operands[0].type==X86_OP_REG and setup[0].operands[0].reg==X86_REG_EDX and setup[0].operands[1].type==X86_OP_IMM and setup[0].operands[1].imm==FREE_CALLERS[a]
            site['argument_setup']=[dict(address=f'{x.address:x}',bytes=x.bytes.hex()) for x in setup]
        calls.append(site)
    assert len(calls)==body.count(name+'(') and calls,(a,len(calls))
    return calls


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions']};helper=entries[HELPER]
    assert set(component['callers'])==set(CALLERS) and set(component['previous_records'])==set(CALLERS)
    assert helper['static'] and helper['name']==NAME and helper['source']==SOURCE and helper['prototype'].count(',')==1
    assert entries[IMPROVISE]['name']=='improvise_2022' and not entries[IMPROVISE]['static']
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==component['debug_sha256'] and specialization(debug)==component['specialization']
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    params=component['parameters'][HELPER];assert [p['name'] for p in params]==['player','max_cmc','card'] and params[-1]['location'] is None
    assert [p['location'][0]['expression'] for p in params[:2]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))']
    assert component['unit_layouts'][SOURCE]['test_definition_t']['bytes']==216
    assert any(v['name']=='test' and v['location']=='(DW_OP_fbreg: -236)' for v in component['locals'][HELPER])
    for at,b in HELPER_SITES.items():assert machine_bytes(int(at,16),len(bytes.fromhex(b))).hex()==b
    source=sources[SOURCE].replace('\r\n','\n');body=function_body(source,NAME);assert sha(body.encode())==helper['reconstruction_body_sha256']
    raw=read(root/'.recovery'/('decompiled-aer-helpers' if component.get('planning') else 'decompiled-abi')/f'{HELPER}.c');assert sha(raw.encode())==helper['raw_sha256']
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());text,literals=recover_strings(strip_comments(raw),runtime,{});assert literals==helper['recovered_literals']
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];assert function_body(cleaned(text,overlap),NAME)==body
    assert 'test_definition_2022_t test;' in body and 'test.cmc = max_cmc + 1;' in body and 'play_card_in_hand_for_free(player, card)' in body
    detail=component['helper'];assert helper['aer_free_helper_evidence']==detail and sha(machine_bytes(int(HELPER,16),detail['machine_bytes']))==detail['machine_sha256']
    assert component['helper_calls']==[s for s in detail['canonical_calls']]
    for n,digest in component['retained_bodies'].items():assert sha(function_body(source,n).encode())==digest
    assert source.count(FORWARD)==1
    for a,migration in component['callers'].items():
        entry=entries[a];assert entry['aer_helper_caller_migration_2022']==migration
        current=function_body(source,entry['name']);previous,restored=historical_view(entry,current);assert previous==component['previous_records'][a]
        assert entry['raw_sha256']==previous['raw_sha256'] and entry['reconstruction_body_sha256']==migration['body_sha256']
        native=machine_bytes(int(a,16),migration['machine_bytes']);assert sha(native)==migration['machine_sha256']
        target,name=(HELPER,NAME) if a in FREE_CALLERS else (IMPROVISE,'improvise_2022')
        assert migration['canonical_calls']==native_calls(native,a,target,name,current)
        assert not re.search(r'\b(?:av_free_spell|improvise)\s*\(',current)
        assert {k:v for k,v in entry.items() if k not in {'aer_helper_caller_migration_2022','reconstruction','reconstruction_body_sha256','supporting_exports'}}=={k:v for k,v in previous.items() if k not in {'reconstruction','reconstruction_body_sha256','supporting_exports'}}
        if a in FREE_CALLERS:
            support=migration['support'];assert support in entry['supporting_exports'] and entry['supporting_exports']==previous.get('supporting_exports',[])+[support]
            raw=read(root/'.recovery'/('decompiled-aer-helpers/'+a+'.c' if component.get('planning') else support['raw_export']));assert sha(raw.encode())==support['raw_sha256']
            assert re.search(NAME+r'\(player,\s*'+str(FREE_CALLERS[a])+r'\)',strip_comments(raw))
            assert len(migration['source_changes'])==1 and migration['source_changes'][0]['replacement']==f'{NAME}(player, {FREE_CALLERS[a]})'
        else:assert entry.get('supporting_exports')==previous.get('supporting_exports')
    assert sum(len(v['canonical_calls']) for v in component['callers'].values())==18


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text())
    if not args.prepare:assert 'aer_helper_callers_native_2022' not in interfaces and HELPER not in {f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        specialization(debug);params=debug_evidence(debug,meta,[HELPER])['parameters'][HELPER]
        assert [p['location'][0]['expression'] for p in params[:2]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))'] and params[-1]['location'] is None
        (cache/'aer-free-spell-clone.tsv').write_text(HELPER+'\t'+NAME+'\tvoid\t__regparm2\tint:player\tint:max_cmc\n')
        (cache/'export-request-aer-helpers.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in [HELPER]+list(FREE_CALLERS)))
        print('Prepared verified two-register Expertise signature and five export requests.');return
    entries={f['address']:copy.deepcopy(f) for f in m['accepted_functions']};previous={a:copy.deepcopy(entries[a]) for a in CALLERS};source=read(root/SOURCE)
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),specialization=specialization(debug),**debug_evidence(debug,meta,[HELPER]+CALLERS),unit_layouts=native_layouts(debug,{SOURCE}),previous_records=previous,callers={},
        retained_bodies={n:sha(function_body(source,n).encode()) for n in ['av_free_spell','improvise','improvise_impl']},
        scope='Recover the private Expertise free-cast specialization with player/max_cmc in two registers and no physical card parameter. Bind four native Expertise callers to this two-argument API and eleven accepted handlers to the recovered native Improvise frontend. Preserve all prior records, raw hashes and reversible source views; pin four fresh caller exports and verify eighteen direct native calls/tail jumps, argument constants and native 216-byte test layout. Legacy helper bodies remain unchanged.')
    for a in CALLERS:
        entry=entries[a];assert entry['source']==SOURCE;before_raw=source_body(source,entry['name']);before=function_body(before_raw,entry['name']);changes=[]
        if a in FREE_CALLERS:
            pattern=r'\bav_free_spell\(player,\s*'+str(FREE_CALLERS[a])+r',\s*[^()]+\)';matches=list(re.finditer(pattern,before));assert len(matches)==1,a
            old=matches[0][0];new=f'{NAME}(player, {FREE_CALLERS[a]})';changes.append(dict(original=old,replacement=new,occurrences=1));after_raw=before_raw.replace(old,new)
        else:
            count=len(re.findall(r'\bimprovise\(',before));assert count
            changes.append(dict(original='improvise(',replacement='improvise_2022(',occurrences=count));after_raw,_=rename(before_raw,{'improvise':'improvise_2022'})
        after=function_body(after_raw,entry['name']);source=source.replace(before_raw,after_raw,1);native=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));target,name=(HELPER,NAME) if a in FREE_CALLERS else (IMPROVISE,'improvise_2022')
        migration=dict(previous_record=previous[a],previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),source_changes=changes,machine_bytes=len(native),machine_sha256=sha(native),canonical_calls=native_calls(native,a,target,name,after),changes=[])
        if a in FREE_CALLERS:
            raw=read(cache/'decompiled-aer-helpers'/f'{a}.c');support=dict(address=a,name=entry['name'],source=SOURCE,raw_export=f'decompiled-support/{a}-aer-free-call.c',raw_sha256=sha(raw.encode()));migration['support']=support;entry['supporting_exports']=entry.get('supporting_exports',[])+[support]
        proof['callers'][a]=migration;entry.update(aer_helper_caller_migration_2022=migration,reconstruction=proof['scope'],reconstruction_body_sha256=migration['body_sha256'])
    raw=read(cache/'decompiled-aer-helpers'/f'{HELPER}.c');text,literals=recover_strings(strip_comments(raw),runtime,{});overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];body='static '+cleaned(text,overlap);assert not UNRESOLVED.search(strip_comments(body))
    anchor='static int aer_implement(int player, int card, event_t event);';assert source.count(anchor)==1;source=source.replace(anchor,anchor+'\n'+FORWARD,1);source+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{HELPER}; original {SOURCE}:{meta[HELPER]["line"]}. */\n'+body+'\n#endif\n'
    native=get(int(HELPER,16),int(meta[HELPER]['high_pc'],16)-int(HELPER,16));decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;targets={int(a,16):f for a,f in entries.items()};calls=[]
    for i in decoder.disasm(native,int(HELPER,16)):
        if i.mnemonic not in {'call','jmp'} or i.operands[0].type!=X86_OP_IMM or i.operands[0].imm not in targets:continue
        target=targets[i.operands[0].imm];assert not target['static'] and re.search(r'\b'+target['name']+r'\b',body);calls.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=target['address'],name=target['name']))
    detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=[],canonical_calls=calls,native_coff_storage=3);proof['helper']=detail;proof['helper_calls']=calls
    entry=dict(address=HELPER,name=NAME,source=SOURCE,line=meta[HELPER]['line'],static=True,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,aer_free_helper_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,NAME).encode()));entries[HELPER]=entry
    compile_sources(root,cache/'aer-helper-check',{SOURCE:source});proof['planning']=True;audit_component(proof,{**m,'accepted_functions':list(entries.values())},{SOURCE:source},get,root);del proof['planning']
    if args.apply:
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};row=next(r for r in csv.DictReader((cache/'decompiled-aer-helpers/export.tsv').open(),delimiter='\t') if r['address']==HELPER);assert HELPER in rows;rows[HELPER]=row;shutil.copyfile(cache/'decompiled-aer-helpers'/f'{HELPER}.c',cache/'decompiled-abi'/f'{HELPER}.c')
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for a in FREE_CALLERS:
            support=proof['callers'][a]['support'];p=cache/support['raw_export'];assert not p.exists();p.parent.mkdir(exist_ok=True);shutil.copyfile(cache/'decompiled-aer-helpers'/f'{a}.c',p)
        (root/SOURCE).write_bytes(source.replace('\n','\r\n').encode('latin1'));m['accepted_functions']=list(entries.values());m['accepted']+=1;m['helper_functions']+=1;m['private_functions']+=1;m['private_helpers']+=1;m['source_files'][SOURCE]+=1;save_json(mp,m);interfaces['aer_helper_callers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'aer-helper-callers-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_private_helpers=1,migrated_card_handlers=15,native_caller_sites=18)))


if __name__=='__main__':main()
