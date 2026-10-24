"""Recover native spell-cost payment, Improvise and two blocked card handlers."""
import argparse,copy,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value
from export_abi import type_of
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from migrate_token_callback_records import source_body
from format_recovery import tokens
from audit_source import function_body,target_validation_input

COST='26af860'
CAST='20bbea0'
FRONT='20bc670'
STATUARY='20c3ac0'
SHADE='230e7c0'
MODIFIER='20ba6c0'
ADDRESSES=[COST,CAST,FRONT,STATUARY,SHADE]
NAMES={'charge_spell_cost':'charge_spell_cost_2022','cast_spell_with_improvise':'cast_spell_with_improvise_2022_impl','improvise':'improvise_2022'}
HEADER_LABEL='/* October 2022 spell-cost payment and native Improvise frontend. */'
TD_SITES={'20bbef0':'e84b435a00','20bbf14':'895da4','20bbfb0':'e88b425a00','20bbfd4':'895da4'}
ROLLBACK_SITES={'26afb5b':'0fbe05f0ce5500','26afb62':'890424','26afb65':'b860024300','26afb6a':'ffd0'}
ENGINE_RANGES=[('26af861','26af88a'),('26af944','26af98a'),('26afa28','26afa6a'),('26afa85','26afb9e'),('26afba3','26afbe5'),('26afc03','26afc3c')]
ABI_ASSERTS='STATIC_ASSERT(offsetof(card_data_t, cc) + 1 == 0x2c, spell_cost_x_byte_2022);\nSTATIC_ASSERT(offsetof(card_data_t, type) == 0x28, spell_cost_type_byte_2022);\n'
NATIVE_NOTE='/* Native 2022 behavior: snapshot copies seven words but rollback reads eight signed bytes.\n * The rollback call receives the sign-extended first byte cast to a pointer (0x26afb5b).\n * The summary byte address advances by player, independently of the 32-byte mana row. */\n'
EXE_BOUNDARIES=[dict(address='4302c0',end='430324',return_type='int',parameters=['int','int','int','int']),
    dict(address='4301a0',end='4301d5',return_type='int',parameters=['int','int']),
    dict(address='430260',end='4302ba',return_type='int',parameters=['int *'])]


def reconstruct(a,raw):
    text=strip_comments(raw);changes=[]
    def replace(old,new,kind,sites=None,count=None):
        nonlocal text
        found=text.count(old);assert found and (count is None or found==count),(a,old,found)
        changes.append(dict(original=old,replacement=new,occurrences=found,kind=kind,machine_sites=[dict(address=at,bytes=b) for at,b in (sites or {}).items()]));text=text.replace(old,new)
    if a==COST:
        replace('*(int *)((int)pcVar5 * 4 + -0x1084860)','pay_mana_xbugrwaU[(uintptr_t)pcVar5 - 0x55cf18]','native six signed cost bytes to 32-bit mana words',count=1)
        replace('pcVar5 = &COST_COLORLESS;','pcVar5 = EXE_BYTE_PTR(0x55cf18);','use contiguous six-byte EXE cost view',count=1)
        replace('*(int *)(&stack0xfea8c350 + (int)psVar8 * 4)','v7[(uintptr_t)psVar8 - 0x55cf20]','native eight-int local snapshot at frame -52',count=2)
        replace('*(undefined4 *)(&stack0xfea8c34c + (int)psVar8 * 4)','(uint32_t)v7[(uintptr_t)psVar8 - 0x55cf21]','native previous-byte snapshot index after cursor increment',count=2)
        replace('*(int *)(player * 0x20 + -0x10847a4 + (int)psVar8 * 4)','mana_pool[player][(uintptr_t)psVar8 - 0x55cf21]','native 32-byte mana row and shifted byte cursor')
        replace('(int *)(player * 0x20 + -0x10847a4 + (int)psVar8 * 4)','&mana_pool[player][(uintptr_t)psVar8 - 0x55cf21]','native 32-byte mana row pointer',count=1)
        replace('*(int *)(player * 0x20 + -0x10846e0 + (int)psVar8 * 4)','mana_pool[player][(uintptr_t)psVar8 - 0x55cef0]','native eight-byte rollback indexing',count=2)
        replace('((int)mana_pool[0] + player + 0x1c)','(EXE_BYTE_PTR(0x4ef4fc) + player)','preserve native one-byte-per-player EXE summary address')
        replace('spell_cost_adjustment_bytes_2022','((int8_t *)0x55cf20)','native signed eight-byte adjustment view')
        replace('spell_cost_paid_snapshot_bytes_2022','((int8_t *)0x55cef0)','native snapshot byte view; retain seven-word copy and eight-byte reads')
        replace('sbyte','int8_t','native MOVSX signed byte semantics')
        for old,new in [('charge_spell_cost_exe_2022','EXE_FN(int, 0x4302c0, int, int, int, int)'),('record_mana_spent_exe_2022','EXE_FN(int, 0x4301a0, int, int)'),('rollback_mana_spent_exe_2022','EXE_FN(int, 0x430260, int *)'),('_DAT_007a2fe4','EXE_DWORD(0x7a2fe4)'),('sub_498F20','EXE_FN(void, 0x498f20, void)'),('sub_499050','EXE_FN(int, 0x499050, void)')]:
            replace(old,new,'preserve typed EXE call or branch-value binding')
    if a==CAST:
        replace('  td.allowed_controller = player;\n','','remove hoisted target field store before initializer',TD_SITES,count=1)
        replace('    td.illegal_state = 1;','    td.allowed_controller = player;\n    td.illegal_state = 1;','restore allowed controller after each native target initializer',TD_SITES,count=2)
    return text,changes


def sites(code,a,body,targets):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;calls=[];refs=[]
    for ins in decoder.disasm(code,int(a,16)):
        for op in ins.operands:
            if op.type!=X86_OP_IMM or op.imm not in targets:continue
            target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==targets[int(a,16)]['source']
            site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(calls if ins.mnemonic in {'call','jmp'} else refs).append(site)
    return calls,refs


def card_layout(debug):
    cu=next(c for c in debug.dwarf().iter_CUs() if value(c.get_top_DIE(),'DW_AT_name')=='functions/engine.c')
    d=next(d for d in cu.get_top_DIE().iter_children() if d.tag=='DW_TAG_typedef' and value(d,'DW_AT_name')=='card_data_t');t=type_of(d)
    members={value(x,'DW_AT_name'):value(x,'DW_AT_data_member_location') for x in t.iter_children() if x.tag=='DW_TAG_member'}
    assert value(t,'DW_AT_byte_size')==72 and members['type']==40 and members['cc']==43
    return dict(bytes=72,type_offset=40,cc_offset=43,x_cost_byte_offset=44)


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('spell_cost_evidence')};assert set(entries)==set(ADDRESSES+[MODIFIER])==set(component['functions'])
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert sha(debug.data)==component['debug_sha256'] and component['card_data_layout']==card_layout(debug)
    assert any(v['name']=='v7' and v['type']=='int[8]' and v['location']=='(DW_OP_fbreg: -52)' for v in component['locals'][COST])
    assert [p['name'] for p in component['parameters'][COST]]==['cd','player','card']
    assert component['parameters'][COST][0]['location']=='(DW_OP_fbreg: 0)'
    for i,p in enumerate(component['parameters'][COST][1:],1):assert all(x['expression']==f'(DW_OP_fbreg: {i*4})' for x in p['location'])
    assert [p['location'][0]['expression'] for p in component['parameters'][CAST]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    assert any(v['name']=='art_to_tap' and v['type']=='int[15]' for v in component['locals'][CAST])
    for layout in component['unit_layouts'].values():assert layout['target_definition_t']['bytes']==84 and layout['test_definition_t']['bytes']==216
    for at,b in {**TD_SITES,**ROLLBACK_SITES}.items():assert machine_bytes(int(at,16),len(bytes.fromhex(b))).hex()==b
    for block,(start,end) in zip(component['index_ranges'],ENGINE_RANGES):
        data=machine_bytes(int(start,16),int(end,16)-int(start,16));assert block==dict(start=start,end=end,bytes=len(data),sha256=sha(data))
    assert component['index_maps']==dict(cost_base='55cf18',cost_words=6,adjustment_base='55cf20',adjustment_bytes=8,snapshot_bytes_base='55cef0',snapshot_copy_words=7,rollback_read_bytes=8,mana_pool_base='4ef4e0',mana_row_bytes=32,summary_base='4ef4fc',summary_player_stride=1,local_snapshot_bytes=32)
    assert 0x55cf18*4-0x1084860==0x4ef400 and 0x55cf21*4-0x10847a4==0x4ef4e0 and 0x55cef0*4-0x10846e0==0x4ef4e0
    assert 0x55cf20*4-0x1573c80==0 and 0x55cf21*4-0x1573c84==0
    exe=Image((root/'.recovery/2022-Magic.exe').read_bytes());assert sha(exe.data)==component['exe_sha256']
    assert [dict((k,v) for k,v in e.items() if k!='machine_sha256') for e in component['exe_boundaries']]==EXE_BOUNDARIES
    for e in component['exe_boundaries']:assert sha(exe.pe.get_data(int(e['address'],16)-exe.pe.OPTIONAL_HEADER.ImageBase,int(e['end'],16)-int(e['address'],16)))==e['machine_sha256']
    for n,digest in component['retained_bodies'].items():assert sha(function_body(read(root/'cards/aether_revolt.c'),n).encode())==digest
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];targets={int(f['address'],16):f for f in manifest['accepted_functions']}
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['spell_cost_evidence']==detail and entry['static']==(a in {CAST,MODIFIER})
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-spell-cost' if component.get('planning') and a!=MODIFIER else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry.get('recovered_literals',[]);text,_=rename(text,NAMES)
        expected=function_body(cleaned(text,overlap),entry['name']);assert (tokens(expected)==tokens(body) if a==MODIFIER else expected==body),a
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if entry['static'] else 2)
        calls,refs=sites(code,a,body,targets);assert calls==detail['canonical_calls'] and refs==detail['callback_references']
        if a==MODIFIER:
            migration=entry['spell_cost_modifier_migration_2022'];assert migration==component['modifier_migration']
            restored,_=rename(body,{new:old for old,new in migration['renames'].items()});assert sha(restored.encode())==migration['previous_body_sha256']
            assert migration['previous_record']==component['previous_modifier_record']
        if a in {COST,FRONT}:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    cost=function_body(sources['functions/engine.c'],NAMES['charge_spell_cost'])
    assert 'EXE_FN(int, 0x430260, int *)((int *)(int)((int8_t *)0x55cef0)[0])' in cost
    assert 'for (iVar15 = 7;' in cost and 'int v7[8];' in cost and 'return iVar15;' in cost
    cast=function_body(sources['cards/aether_revolt.c'],NAMES['cast_spell_with_improvise'])
    assert cast.count('td.allowed_controller = player;')==2 and 'target_definition_2022_t td;' in cast and 'int art_to_tap[15];' in cast
    assert ABI_ASSERTS in read(root/'functions/engine.c') or component.get('planning')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'spell_cost_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());exe=Image((cache/'2022-Magic.exe').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    previous=copy.deepcopy(next(f for f in m['accepted_functions'] if f['address']==MODIFIER))
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),exe_sha256=sha(exe.data),exe_boundaries=[dict(e,machine_sha256=sha(exe.pe.get_data(int(e['address'],16)-exe.pe.OPTIONAL_HEADER.ImageBase,int(e['end'],16)-int(e['address'],16)))) for e in EXE_BOUNDARIES],
        **debug_evidence(debug,meta,ADDRESSES+[MODIFIER]),unit_layouts=native_layouts(debug,{'cards/aether_revolt.c'}),card_data_layout=card_layout(debug),functions={},previous_modifier_record=previous,
        retained_bodies={n:sha(function_body(read(root/'cards/aether_revolt.c'),n).encode()) for n in ['improvise','improvise_impl']},
        index_ranges=[dict(start=a,end=b,bytes=int(b,16)-int(a,16),sha256=sha(get(int(a,16),int(b,16)-int(a,16)))) for a,b in ENGINE_RANGES],
        index_maps=dict(cost_base='55cf18',cost_words=6,adjustment_base='55cf20',adjustment_bytes=8,snapshot_bytes_base='55cef0',snapshot_copy_words=7,rollback_read_bytes=8,mana_pool_base='4ef4e0',mana_row_bytes=32,summary_base='4ef4fc',summary_player_stride=1,local_snapshot_bytes=32),
        scope='Recover native spell-cost payment, the private Improvise worker and public frontend, Inspiring Statuary and Skyclave Shade. Reconstruct signed EXE byte views, local int[8] indexing and typed EXE calls from native operands; preserve seven-word snapshots with eight-byte reads, one-byte-per-player summary stride, native rollback pointer materialized from a sign-extended byte, AI branches and return-register behavior. Restore Improvise controller stores after both initializers, retain its native int[15] artifact array, and migrate the accepted cost modifier to native targeting. Preserve legacy Improvise bodies and the exact modifier predecessor; gameplay corrections are not inferred.')
    entries={f['address']:copy.deepcopy(f) for f in m['accepted_functions']};sources={s:read(root/s) for s in {meta[a]['source'] for a in ADDRESSES}}
    entry=entries[MODIFIER];before_raw=source_body(sources[entry['source']],entry['name']);before=function_body(before_raw,entry['name']);mapping=dict(target_definition_t='target_definition_2022_t',default_target_definition='default_target_definition_2022',target_available='target_available_2022');after_raw,_=rename(before_raw,mapping);after=function_body(after_raw,entry['name']);sources[entry['source']]=sources[entry['source']].replace(before_raw,after_raw,1)
    migration=dict(previous_record=previous,previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),renames=mapping);entry.update(spell_cost_modifier_migration_2022=migration,reconstruction=proof['scope'],reconstruction_body_sha256=migration['body_sha256']);proof['modifier_migration']=migration
    overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];new=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];raw=read(cache/'decompiled-spell-cost'/f'{a}.c');text,changes=reconstruct(a,raw);text,literals=recover_strings(text,runtime,{});text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a==CAST:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict',body),a
        name=NAMES.get(row['name'],row['name']);entry=dict(address=a,name=name,source=source,line=row['line'],static=a==CAST,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode()));entries[a]=entry;new.append(entry)
        sources[source]+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+(ABI_ASSERTS+NATIVE_NOTE if a==COST else '')+body+'\n#endif\n'
    targets={int(a,16):f for a,f in entries.items()}
    for a in ADDRESSES+[MODIFIER]:
        entry=entries[a];raw=read(cache/('decompiled-spell-cost' if a!=MODIFIER else 'decompiled-abi')/f'{a}.c');_,changes=reconstruct(a,raw);code=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16))
        symbols=[(n,s) for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert len(symbols)==1 and symbols[0][1]==(3 if entry['static'] else 2),(a,symbols)
        calls,refs=sites(code,a,function_body(sources[entry['source']],entry['name']),targets);detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=calls,callback_references=refs,native_coff_storage=symbols[0][1],native_coff_name=symbols[0][0]);entry['spell_cost_evidence']=detail;proof['functions'][a]=detail
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in new if e['address'] in {COST,FRONT})+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'spell-cost-check',sources,header);proof['planning']=True;audit_component(proof,{**m,'accepted_functions':list(entries.values())},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-spell-cost/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-spell-cost'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']=list(entries.values());m['accepted']+=5;m['card_handler_functions']+=2;m['helper_functions']+=3;m['private_functions']+=1;m['private_helpers']+=1;m['shared_helper_interfaces']+=2
        for e in new:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['spell_cost_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'spell-cost-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=2,new_helpers=3,migrated_cost_modifiers=1)))


if __name__=='__main__':main()
