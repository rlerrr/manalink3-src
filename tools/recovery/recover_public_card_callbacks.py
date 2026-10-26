"""Recover shared bounce/damage callbacks and four card consumers."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body

BOUNCE='25fb3e0'
ARTIFACT='22444f0'
WIND='2608540'
WALL='2058980'
QUICKSILVER='22549e0'
RUNE='2056200'
HOMONYM='212ace0'
ADDRESSES=[BOUNCE,ARTIFACT,WIND,WALL,QUICKSILVER,RUNE]
NAMES={BOUNCE:'bounce_at_end_of_combat_2022',ARTIFACT:'damage_from_artifact_source_2022'}
HEADER_LABEL='/* October 2022 public card callbacks and shared Windscouter entry point. */'
LEGACY=[('cards/stronghold.c','bounce_at_end_of_combat'),('cards/stronghold.c','wot_effect'),('cards/antiquities.c','damage_from_artifact_source')]


def aliases(a):
    if a in {BOUNCE,WIND,WALL}:return {'bounce_at_end_of_combat':NAMES[BOUNCE]}
    if a in {ARTIFACT,RUNE}:return {'damage_from_artifact_source':NAMES[ARTIFACT]}
    return {}


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('public_card_callback_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert len(component['retained_legacy_bodies'])==3
    for detail in component['retained_legacy_bodies']:
        assert (detail['source'],detail['name']) in LEGACY
        from unify_scalar_helpers import retained_source_body
        assert sha(retained_source_body(root,detail['source'],detail['name']).encode())==detail['body_sha256']
    wrong=component['excluded_homonym'];assert wrong['address']==HOMONYM and wrong['source']=='cards/stronghold.c' and wrong['native_coff_storage']==3 and wrong['physical_arguments']==4
    assert sha(machine_bytes(int(HOMONYM,16),wrong['machine_bytes']))==wrong['machine_sha256']
    for a,params in component['parameters'].items():
        assert len(params)==(4 if a==HOMONYM else 3)
        assert [p['location'] if isinstance(p['location'],str) else p['location'][0]['expression'] for p in params]==[f'(DW_OP_fbreg: {i*4})' for i in range(len(params))]
    assert component['unit_layouts']['cards/urza_saga.c']['target_definition_t']['bytes']==84
    assert any(v['name']=='td' and v['type']=='target_definition_t' and v['location']=='(DW_OP_fbreg: -104)' for v in component['locals'][RUNE])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['public_card_callback_evidence']==detail and not entry['static']
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-shared-callbacks' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,literals=recover_strings(strip_comments(raw),runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,aliases(a))
        assert function_body(cleaned(text,overlap),entry['name'])==body and detail['translations']==aliases(a)
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==2
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body) and site['target']!=HOMONYM
        if a==ARTIFACT:
            assert all(literal in body for literal in ['"must be a damage card"','"must be damage from artifact source"','"must be damage dealt to you"'])
            assert 'targets[3].player' in body and 'check_special_flags_2022(' in body
        if a==BOUNCE:assert 'trigger_condition == 0xcc' in body and 'EVENT_END_TRIGGER' in body and 'KILL_EXILE' in body
        if a==RUNE:assert 'target_definition_2022_t td;' in body and body.count(NAMES[ARTIFACT])==2
        if a in {BOUNCE,ARTIFACT,WIND}:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    for a,target in [(WIND,BOUNCE),(WALL,BOUNCE),(RUNE,ARTIFACT)]:
        refs=component['functions'][a]['callback_references'];assert len(refs)==(2 if a==RUNE else 1) and all(r['target']==target for r in refs)
    assert any(r['target']==WIND for r in component['functions'][QUICKSILVER]['canonical_calls'])


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'public_card_callbacks_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    size=int(meta[HOMONYM]['high_pc'],16)-int(HOMONYM,16)
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),**debug_evidence(debug,meta,ADDRESSES+[HOMONYM]),unit_layouts=native_layouts(debug,{'cards/urza_saga.c'}),functions={},
        retained_legacy_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],
        excluded_homonym=dict(address=HOMONYM,source='cards/stronghold.c',physical_arguments=4,native_coff_storage=3,machine_bytes=size,machine_sha256=sha(get(int(HOMONYM,16),size))),
        scope='Recover the public three-argument end-of-combat bounce callback and artifact-damage predicate, plus Windscouter, Wall of Junk, Quicksilver Behemoth and Rune of Protection: Artifacts. Qualify names only in functions with verified native bindings; preserve the separate private four-argument Stronghold wrapper, its effect and the old private artifact predicate. Preserve native bounce cleanup, missing-source artifact snapshot fallback, planeswalker-damage rejection, cycling and prevention behavior.')
    wrong_storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(HOMONYM,16)]
    assert wrong_storage==[3]
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(a,meta[a]['name']),source=meta[a]['source'],static=False) for a in ADDRESSES})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=NAMES.get(a,row['name']);raw=read(cache/'decompiled-shared-callbacks'/f'{a}.c');text,literals=recover_strings(strip_comments(raw),runtime,{});text,_=rename(text,aliases(a));body=cleaned(text,overlap)
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[2],(a,storage)
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=[],translations=aliases(a),canonical_calls=[],callback_references=[],native_coff_storage=2)
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail
        entries.append(dict(address=a,name=name,source=source,line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,public_card_callback_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in {BOUNCE,ARTIFACT,WIND})+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'public-callbacks-check',{**sources,'cards/stronghold.c':read(root/'cards/stronghold.c')},header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-shared-callbacks/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-shared-callbacks'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=6;m['card_handler_functions']+=4;m['helper_functions']+=2;m['shared_helper_interfaces']+=2
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['public_card_callbacks_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'public-card-callbacks-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=4,new_helpers=2,private_helpers=0)))


if __name__=='__main__':main()
