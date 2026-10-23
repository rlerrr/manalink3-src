"""Recover artifact mana frontends, shared target predicates and three consumers."""
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
from audit_source import function_body,target_validation_input

ONE='266c7a0'
DIFFERENT='26712f0'
LAND='2043f20'
DAMAGE='216cda0'
VESSEL='21cd780'
STREAM='21bfed0'
CIRCLE='238f920'
HELPERS=[ONE,DIFFERENT,LAND,DAMAGE]
ADDRESSES=HELPERS+[VESSEL,STREAM,CIRCLE]
NAMES={n:n+'_2022' for n in ['artifact_mana_all_one_color','artifact_mana_all_different_colors','is_land_or_nonblack_creature','colored_damage_source_that_damages_me']}
NAMES['produce_mana_tapped_all_different_colors_impl']='produce_mana_tapped_all_different_colors_2022_impl'
HEADER_LABEL='/* October 2022 artifact mana helpers and shared target predicates. */'
LEGACY=[('functions/produce_mana.c','artifact_mana_all_one_color'),('cards/urza_saga.c','is_land_or_nonblack_creature'),('cards/mercadian_masques.c','colored_damage_source_that_damages_me')]
WORKER_SITES={'267133a':'31c9','267133c':'8b550c','267133f':'8a4858','2671342':'8b4514','2671345':'890424','2671348':'c744240400000000','2671350':'8b4508','2671353':'e8a8faffff',
              '267141a':'31c9','267141c':'8b550c','267141f':'8a4858','2671422':'8b4514','2671425':'890424','2671428':'c744240400000000','2671430':'8b4508','2671433':'e8c8f9ffff'}
SHIFT_SITES={'238fa83':'ba01000000','238fa88':'d3e2','238fa8a':'895038'}
ERROR_SITE=dict(address='216cdc6',bytes='b860907300')


def reconstruct(a,raw):
    text=strip_comments(raw);changes=[]
    if a==LAND:
        old='pcVar4 = get_sleighted_color_text';new='pcVar4 = (char *)get_sleighted_color_text';assert text.count(old)==1
        changes.append(dict(original=old,replacement=new,occurrences=1,kind='adapt existing const-qualified string API to native char-pointer result without writes',machine_sites=[]));text=text.replace(old,new)
    if a==DAMAGE:
        old='return (char *)0x739060;';new='return (char *)EXE_STR(0x739060);';assert text.count(old)==1
        changes.append(dict(original=old,replacement=new,occurrences=1,kind='preserve native engine color-error pointer',machine_sites=[ERROR_SITE]));text=text.replace(old,new)
    if a==CIRCLE:
        old='1 << ((byte)iVar2 & 0x1f)';new='1U << ((byte)iVar2 & 0x1f)';assert text.count(old)==1
        changes.append(dict(original=old,replacement=new,occurrences=1,kind='explicit native 32-bit unsigned chosen-color shift',machine_sites=[dict(address=a,bytes=b) for a,b in SHIFT_SITES.items()]));text=text.replace(old,new)
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('mana_predicate_card_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    for retained in component['retained_bodies']:
        assert (retained['source'],retained['name']) in LEGACY
        assert sha(function_body(read(root/retained['source']),retained['name']).encode())==retained['body_sha256']
    worker=next(f for f in manifest['accepted_functions'] if f['address']=='2670e00');assert worker==component['retained_worker_record'] and worker['static'] and worker['source']=='functions/produce_mana.c'
    spec=component['worker_specialization'];assert spec['mca']['constant']==1 and spec['mca']['location']==[]
    assert [spec[n]['location'] for n in ['player','card','available','num_mana','prompt']]==[[80],[82],[81],[145,0],[145,4]]
    assert worker['prototype'].count(',')==4 and worker['name']=='produce_mana_tapped_all_different_colors_2022_impl'
    for at,expected in WORKER_SITES.items():assert machine_bytes(int(at,16),len(bytes.fromhex(expected))).hex()==expected
    assert component['engine_error_pointer']==dict(address='739060',binding='(char *)EXE_STR(0x739060)',operand=ERROR_SITE,scope='Engine-owned runtime color-error buffer; no on-disk message is assumed.')
    for source,layout in component['unit_layouts'].items():assert layout['target_definition_t']['bytes']==84
    for a in [STREAM,CIRCLE]:assert any(v['name']=='td' and v['type']=='target_definition_t' and v['location']=='(DW_OP_fbreg: -104)' for v in component['locals'][a])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['mana_predicate_card_evidence']==detail and not entry['static']
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-mana-predicates' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,NAMES)
        assert function_body(cleaned(text,overlap),entry['name'])==body
        params=component['parameters'][a];assert len(params)==(5 if a in HELPERS else 3)
        assert [p['location'] if isinstance(p['location'],str) else p['location'][0]['expression'] for p in params]==[f'(DW_OP_fbreg: {i*4})' for i in range(len(params))]
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==2
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body)
        if a==DIFFERENT:
            assert body.count('produce_mana_tapped_all_different_colors_2022_impl(')==2 and 'in_stack_' not in body
            assert body.count('artifact_mana_all_one_color_2022(')==1
        if a==ONE:
            assert 'artifact_mana_all_one_color_2022(player, card, EVENT_CAN_ACTIVATE, amount, sac)' in body
            assert 'spell_fizzled != 1' in body
        if a in {ONE,DIFFERENT}:assert 'KILL_SACRIFICE' in body and 'declare_mana_available_maybe_hex(' in body
        if a==DAMAGE:assert '(char *)EXE_STR(0x739060)' in body and 'initial_color' in body and '"damaging a Planeswalker."' in body
        if a==CIRCLE:assert '1U << ((unsigned char)' in body and 'target_definition_2022_t td;' in body
        if a in HELPERS:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len([s for s in component['functions'][STREAM]['callback_references'] if s['target']==LAND])==2
    assert len([s for s in component['functions'][CIRCLE]['callback_references'] if s['target']==DAMAGE])==1
    assert any(s['target']==DIFFERENT for s in component['functions'][VESSEL]['canonical_calls'])
    assert any(s['target']==ONE for s in component['functions'][ONE]['canonical_calls'])


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'mana_predicate_cards_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{'cards/portal_1_2_3k.c','cards/mirage.c'}),functions={},
        retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in LEGACY],retained_worker_record=next(f for f in m['accepted_functions'] if f['address']=='2670e00'),
        worker_specialization=interfaces['mana_production_2022']['specialized_different_colors'],engine_error_pointer=dict(address='739060',binding='(char *)EXE_STR(0x739060)',operand=ERROR_SITE,scope='Engine-owned runtime color-error buffer; no on-disk message is assumed.'),
        scope='Recover two artifact mana frontends, two shared target predicates and Firemind Vessel, Stream of Acid and Prismatic Circle. Reuse the accepted five-physical-argument distinct-color worker with mca=1; preserve native tap/sickness/sacrifice checks, AI costs, mana declaration, fizzle gates and same-entry recursion. Keep live-source and damage-snapshot color paths, the native engine error pointer, sleight-aware targeting and native 32-bit color shift. Retain three legacy bodies and all previous recovery records.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=False) for a in ADDRESSES})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=NAMES.get(row['name'],row['name']);raw=read(cache/'decompiled-mana-predicates'/f'{a}.c');text,changes=reconstruct(a,raw);text,literals=recover_strings(text,runtime,{});text,_=rename(text,NAMES);body=cleaned(text,overlap)
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[2],(a,storage)
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=2)
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,mana_predicate_card_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS)+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'mana-predicate-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-mana-predicates/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-mana-predicates'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=7;m['card_handler_functions']+=3;m['helper_functions']+=4;m['shared_helper_interfaces']+=4
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['mana_predicate_cards_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'mana-predicate-cards-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=3,new_helpers=4,private_helpers=0)))


if __name__=='__main__':main()
