"""Recover card type, mana-value, PT and battlefield queries."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value,inherited_value
from export_abi import type_of
from pack_native_tables import unqual
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read
from recover_combat_mask_cards import cleaned
from recover_literals import readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input
from card_query_bodies import BODIES

PLANESWALKER='2681af0';WITH_IID='2681b50';TYPE='2681c50';IS_WHAT='2681ca0';PT='2677b70';BASE_POWER='2696aa0';BASE_POWER_IID='2696ae0';BASE_TOUGHNESS='2696b00';BASE_TOUGHNESS_IID='2696b40';SPLIT='267ed20';CMC_IID='267ecb0';CMC='2694100';HIGHEST_POWER='2692070';POWER_SUM='2691ae0';DEFENDERS='2691a40';BY_TOUGHNESS='268c7a0';BY_POWER='268c6e0';HIGHEST_NONLAND='26944f0';HIGHEST_CMC='2694440';COMMON_HAND='2694390';COMMON_NONLAND='26942c0';BY_TYPE='2683b50';BY_COLOR='2683e60';BY_ID='267bbb0';CHECK_SUBTYPE='2682d30';COUNT_SUBTYPE='26828d0';SHRINES='267a900'
ADDRESSES=list(BODIES)
PRIVATE={PT,SPLIT};PUBLIC_FUNCTIONS=[a for a in ADDRESSES if a not in PRIVATE];PUBLIC_HELPERS=PUBLIC_FUNCTIONS
NAMES={a:body.split('(')[0].strip().split()[-1].lstrip('*') for a,body in BODIES.items()}
LABEL='/* October 2022 card type, mana-value, PT and battlefield queries. */'
PRESERVED_RAW=set()
EXE_TARGETS=[]


def raw_path(cache,a,planning=False):
    return cache/('decompiled-card-queries' if planning and a not in PRESERVED_RAW else 'decompiled-abi')/(a+'.c')


def retained_body(source,name):
    # Older names occur in conditions before their definitions. Anchor the
    # actual C definition so those calls cannot be mistaken for a body.
    at=re.search(r'(?m)^[ \t]*(?:static\s+)?(?:int|type_t)\s+'+re.escape(name)+r'\s*\([^;{}]*\)\s*\{',source)
    if at is None:
        from unify_scalar_helpers import retained_alias_body
        return retained_alias_body(source,name)
    return function_body(source[at.start():],name)




def native_storage(runtime,debug,meta,root):
    from card_query_evidence import inspect_native
    return inspect_native(runtime,debug,meta,root)


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore printed/current type and flags, Grist creature/PT exception, generic recursive split-card mana value, token/DFC/X handling, PT callback context restoration, histograms and exact battlefield filters',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('card_queries_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    for old in component['retained_helpers']:assert sha(retained_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta,root)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['card_queries_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256']
        raw=read(raw_path(root/'.recovery',a,component.get('planning',False)));assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    assert [p['location'][0]['expression'] for p in component['parameters'][PT]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    assert [p['location'][0]['expression'] for p in component['parameters'][SPLIT]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))']
    cmc_locals={v['name']:v['type'] for v in component['locals'][CMC]}
    assert all(cmc_locals[n]=='int' for n in ['iid','result','result_split_card','csvid','half_played'])
    for old in component['retained_original_handlers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for a in PUBLIC_HELPERS:assert entries[a]['prototype'] in read(root/'manalink.h') or component.get('planning')




def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta,root);(cache/'export-request-card-queries.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared twenty-seven card-query helpers.');return
    assert 'card_queries_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[(meta[a]['source'],meta[a]['name']) for a in ADDRESSES]
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta,root),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=n,source=source,body_sha256=sha(retained_body(read(root/source),n).encode())) for source,n in retained],retained_original_handlers=[],
        scope='Recover twenty-seven card-query helpers in functions/functions.c with twenty-five public interfaces and two private physical workers. Retain every old body. Preserve Grist off-battlefield creature and 1/1 PT exceptions, printed/current planeswalker and type overrides, generic recursive split-card mana values and popcount selector, token/DFC/X-cost handling and zero-valued splits, callback global restoration, seventeen-bin unchecked histograms and first-on-tie behavior, signed counts/zero floors, and exact type/color/subtype/ability-loss filters. No runtime or downstream integration is claimed.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(raw_path(cache,a,True));text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],card_queries_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(next(e['prototype'] for e in entries if e['address']==a) for a in PUBLIC_HELPERS)+'\n#endif\n\n'+header[at:]
    # Match check_source.py's existing MSVC-host compatibility alias, only in
    # the disposable syntax-check copy. GCC's original source stays unchanged.
    syntax_sources={s:('#define strncasecmp _strnicmp\n'+t if s=='functions/rules_engine.c' else t) for s,t in sources.items()}
    compile_sources(root,cache/'card-queries-check',syntax_sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-card-queries/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES and r['address'] not in PRESERVED_RAW]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-card-queries'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=27;m['helper_functions']+=27;m['shared_helper_interfaces']+=25;m['private_functions']+=2;m['private_helpers']+=2
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['card_queries_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'card-queries-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=27)))


if __name__=='__main__':main()
