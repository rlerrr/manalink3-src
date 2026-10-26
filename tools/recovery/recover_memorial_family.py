"""Recover the five Memorial lands and migrate their accepted resolver interfaces."""
import argparse,copy,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value,inherited_value
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings,readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from migrate_token_callback_records import source_body
from format_recovery import tokens
from audit_source import function_body,target_validation_input

WORKER='22ca5c0'
ADDRESSES='22ca5c0 22d07e0 22d0850 22d08c0 22d0930 22d09a0'.split()
CALLBACKS='22c9630 22c95d0 22c9680 22c94f0 22ca2c0'.split()
SOURCE='cards/dominaria.c'
NAME='dominaria_memorial_2022_impl'
WAR_GUARD={'22d09b0':'8d598e','22d09b3':'83fb01','22d09b6':'7668'}
AI_SITES={'22ca62e':'89f0','22ca630':'c1e005','22ca633':'8b80bcf44e00','22ca639':'83e806','22ca63c':'898514ffffff'}
INDIRECT_SITES={'22ca7a8':'8b4528','22ca7af':'89442408','22ca7b6':'ff5530'}
CONTROL_LITERALS=[dict(symbol='UNK_0273cd55',address='0x273cd55',bytes='0116',raw_occurrences=1,function_end='0x22ca999',materializations=[dict(address='22ca6f6',bytes='c78518ffffff55cd7302')]),
    dict(symbol='UNK_0273cd58',address='0x273cd58',bytes='0117',raw_occurrences=1,function_end='0x22ca999',materializations=[dict(address='22ca7c0',bytes='c78518ffffff58cd7302')])]


def historical_view(entry,body,binding=None):
    migration=entry['memorial_callback_migration_2022'];previous=migration['previous_record']
    body=body.replace('\r\n','\n');assert sha(body.encode())==migration['body_sha256']
    restored,_=rename(body,{new:old for old,new in migration['renames'].items()});assert sha(restored.encode())==migration['previous_body_sha256']
    if binding:assert entry[binding]==previous[binding]
    return previous,restored


def specialization(debug):
    cu=next(c for c in debug.dwarf().iter_CUs() if value(c.get_top_DIE(),'DW_AT_name')==SOURCE)
    die=next(d for d in cu.get_top_DIE().iter_children() if value(d,'DW_AT_low_pc')==int(WORKER,16))
    ps={inherited_value(p,'DW_AT_name'):p for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter'}
    assert 'DW_AT_location' in ps['ai'].attributes and value(ps['ai'],'DW_AT_const_value') is None
    names=['x','b','u','g','r','w','target_in_grave','label','td','prompt','impl']
    assert [value(ps[n],'DW_AT_location') for n in names]==[[145,i*4] for i in range(11)]
    symbols=[(n,s) for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(WORKER,16)]
    assert symbols==[('_dominaria_memorial.isra.0',3)],symbols
    return dict(native_symbol=symbols[0][0],physical_parameters=14,registers=['player','card','event'],stack=names,abstract_ai='DW_OP_GNU_parameter_ref; no physical entry storage',ai_formula='landsofcolor_controlled[player][7] - 6')


def reconstruct(a,raw):
    text=strip_comments(raw);changes=[];substitutions=[]
    if a==WORKER:
        substitutions=[('_func_void_int_int_target_definition_t_ptr *impl','void (*impl)(int,int,target_definition_t *)',INDIRECT_SITES),
            ('undefined *puStack_ec;','const char *puStack_ec;',{}),('char *label','const char *label',{})]
    if a=='22d09a0':substitutions=[('event - EVENT_RESOLVE_ACTIVATION < 2','(uint)(event - EVENT_RESOLVE_ACTIVATION) < 2',WAR_GUARD)]
    for old,new,sites in substitutions:
        assert text.count(old)==1,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=1,kind='recover native callback/string types or explicit unsigned event range',machine_sites=[dict(address=at,bytes=b) for at,b in sites.items()]));text=text.replace(old,new)
    return text,changes


def control_literals(a,text,runtime):
    if a!=WORKER:return text,[]
    for literal in CONTROL_LITERALS:
        data=readonly_string(runtime,int(literal['address'],16));assert data==bytes.fromhex(literal['bytes'])
        old='&'+literal['symbol'];assert text.count(old)==1;text=text.replace(old,byte_literal(data))
    return text,copy.deepcopy(CONTROL_LITERALS)


def native_sites(code,a,body,targets):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;calls=[];refs=[]
    for ins in decoder.disasm(code,int(a,16)):
        for op in ins.operands:
            if op.type!=X86_OP_IMM or op.imm not in targets:continue
            target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==SOURCE
            site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(calls if ins.mnemonic in {'call','jmp'} else refs).append(site)
    return calls,refs


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('memorial_family_evidence')}
    assert set(entries)==set(ADDRESSES+CALLBACKS)==set(component['functions'])
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data) and component['specialization']==specialization(debug)
    p=component['parameters'][WORKER];assert len(p)==15 and p[-1]['name']=='ai' and all('DW_OP_GNU_parameter_ref' in v['expression'] for v in p[-1]['location'])
    assert [v['location'][0]['expression'] for v in p[:3]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    assert [v['location'] for v in p[3:-1]]==[f'(DW_OP_fbreg: {i*4})' for i in range(11)]
    for at,b in {**WAR_GUARD,**AI_SITES,**INDIRECT_SITES}.items():assert machine_bytes(int(at,16),len(bytes.fromhex(b))).hex()==b
    for literal in CONTROL_LITERALS:
        for site in literal['materializations']:assert machine_bytes(int(site['address'],16),len(bytes.fromhex(site['bytes']))).hex()==site['bytes']
    layout=component['unit_layouts'][SOURCE];assert layout['target_definition_t']['bytes']==84 and layout['test_definition_t']['bytes']==216
    assert any(v['name']=='test' and v['location']=='(DW_OP_fbreg: -236)' for v in component['locals'][WORKER])
    assert any(v['name']=='td' and v['location']=='(DW_OP_fbreg: -96)' for v in component['locals']['22d09a0'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    # Replay the target catalog recorded for these accepted caller bodies.
    # Recovering a callee later does not migrate this component's callers;
    # the new callee component records those pending bindings separately.
    recorded_targets={int(site['target'],16) for detail in component['functions'].values() for site in detail['canonical_calls']+detail['callback_references']}
    targets={int(f['address'],16):f for f in manifest['accepted_functions'] if int(f['address'],16) in recorded_targets|{int(a,16) for a in entries}}
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['memorial_family_evidence']==detail and entry['static']==(a==WORKER or a in CALLBACKS)
        body=function_body(sources[SOURCE].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-memorial-family' if component.get('planning') and a in ADDRESSES else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,constants=control_literals(a,text,runtime);assert constants==entry.get('reconstructed_literal_constants',[])
        text,literals=recover_strings(text,runtime,{});assert literals==entry.get('recovered_literals',[])
        expected=function_body(cleaned(text,overlap),entry['name'])
        matches=tokens(expected)==tokens(body) if a in CALLBACKS else expected==body
        if not matches:
            import difflib
            raise AssertionError(a+'\n'+'\n'.join(difflib.unified_diff(expected.splitlines(),body.splitlines())))
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if entry['static'] else 2)
        calls,refs=native_sites(code,a,body,targets);assert calls==detail['canonical_calls'] and refs==detail['callback_references']
        if a in CALLBACKS:
            migration=entry['memorial_callback_migration_2022'];assert migration==component['migrations'][a]
            previous,restored=historical_view(entry,body);assert previous==component['previous_records'][a] and len(component['parameters'][a])==3
            assert 'target_definition_2022_t *td' in body and 'target_definition_t' not in body
        elif a!=WORKER:
            assert len(component['parameters'][a])==3 and body.count(NAME+'(')==1
            assert {r['target'] for r in refs}=={CALLBACKS[ADDRESSES[1:].index(a)]}
    worker=function_body(sources[SOURCE],NAME)
    assert '(*impl)(player, card, td)' in worker and 'test_definition_2022_t test;' in worker and 'spell_fizzled != 1' in worker
    assert 'x + 1' in worker and 'iVar1 + -6' in worker and 'tapped_for_mana_color = -1;' in worker
    assert 'new_global_tutor_2022(' in function_body(sources[SOURCE],entries['22c94f0']['name'])
    assert 'valid_target_2022(td)' in function_body(sources[SOURCE],entries['22ca2c0']['name'])


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'memorial_family_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());spec=specialization(debug)
    previous={f['address']:copy.deepcopy(f) for f in m['accepted_functions'] if f['address'] in CALLBACKS};assert set(previous)==set(CALLBACKS)
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),specialization=spec,**debug_evidence(debug,meta,ADDRESSES+CALLBACKS),unit_layouts=native_layouts(debug,{SOURCE}),functions={},previous_records=previous,migrations={},
        scope='Recover the concrete fourteen-parameter Memorial helper and five land handlers; the abstract ai parameter has no physical storage and native AI priority uses the player land count minus six. Migrate five existing resolver interfaces to native 84-byte targets and the Unity resolver to a 216-byte test/native tutor. Retain each exact previous record and reversible source view. Preserve native dialog controls, mana/target eligibility, tap-before-payment, fizzle/sacrifice/untap paths, graveyard selection and callback dispatch; qualify the unsigned War event range.')
    source=read(root/SOURCE);entries={f['address']:copy.deepcopy(f) for f in m['accepted_functions']};new=[]
    overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    for a in CALLBACKS:
        entry=entries[a];before_raw=source_body(source,entry['name']);before=function_body(before_raw,entry['name']);mapping={'target_definition_t':'target_definition_2022_t'}
        if a=='22c94f0':mapping.update(test_definition_t='test_definition_2022_t',new_default_test_definition='new_default_test_definition_2022',new_global_tutor='new_global_tutor_2022')
        if a=='22ca2c0':mapping['valid_target']='valid_target_2022'
        after_raw,_=rename(before_raw,mapping);after=function_body(after_raw,entry['name']);source=source.replace(before_raw,after_raw,1)
        migration=dict(previous_record=previous[a],previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),renames=mapping)
        entry.update(prototype=after_raw[:after_raw.index('{')].strip()+';',memorial_callback_migration_2022=migration,reconstruction=proof['scope'],reconstruction_body_sha256=migration['body_sha256']);proof['migrations'][a]=migration
    for a in ADDRESSES:
        row=meta[a];assert row['source']==SOURCE;raw=read(cache/'decompiled-memorial-family'/f'{a}.c');text,changes=reconstruct(a,raw);text,constants=control_literals(a,text,runtime);text,literals=recover_strings(text,runtime,{});body=cleaned(text,overlap)
        if a==WORKER:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict',body),a
        name=NAME if a==WORKER else row['name'];entry=dict(address=a,name=name,source=SOURCE,line=row['line'],static=a==WORKER,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode()))
        if constants:entry['reconstructed_literal_constants']=constants
        entries[a]=entry;new.append(entry);source+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {SOURCE}:{row["line"]}. */\n'+body+'\n#endif\n'
    targets={int(a,16):f for a,f in entries.items()}
    for a in ADDRESSES+CALLBACKS:
        entry=entries[a];raw=read(cache/('decompiled-memorial-family' if a in ADDRESSES else 'decompiled-abi')/f'{a}.c');_,changes=reconstruct(a,raw)
        code=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));symbols=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert symbols==[3 if entry['static'] else 2]
        calls,refs=native_sites(code,a,function_body(source,entry['name']),targets);detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=calls,callback_references=refs,native_coff_storage=symbols[0]);entry['memorial_family_evidence']=detail;proof['functions'][a]=detail
    compile_sources(root,cache/'memorial-family-check',{SOURCE:source});get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':list(entries.values())},{SOURCE:source},get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-memorial-family/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-memorial-family'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        (root/SOURCE).write_bytes(source.replace('\n','\r\n').encode('latin1'));m['accepted_functions']=list(entries.values());m['accepted']+=6;m['card_handler_functions']+=5;m['helper_functions']+=1;m['private_functions']+=1;m['private_helpers']+=1;m['source_files'][SOURCE]+=6
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['memorial_family_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'memorial-family-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=5,new_helpers=1,migrated_existing_callbacks=5)))


if __name__=='__main__':main()
