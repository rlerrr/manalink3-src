"""Recover the remaining land and creature mana-source helpers."""
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
from mana_source_bodies import BODIES

LANDS='266bc20';QUERY='266f670';TAP='266b9d0';GRANT_ONE='266ca00';GRANT='266d250';SAC_TAPPED='266de60';TWO='266d9b0';CREATURE_MULTI='266e5e0';CREATURE='266d050';ENCHANTED='266f1a0';TAPPED='266f100';PRODUCER='266ed00';SAC='266dca0'
ADDRESSES=[LANDS,QUERY,TAP,GRANT_ONE,GRANT,SAC_TAPPED,TWO,CREATURE_MULTI,CREATURE,PRODUCER,ENCHANTED,TAPPED,SAC]
PRIVATE=set();PUBLIC_HELPERS=ADDRESSES
NAMES={a:body.split('(')[0].strip().split()[-1] for a,body in BODIES.items()}
LABEL='/* October 2022 land and creature mana sources and replacement queries. */'
EXE_TARGETS=[0x4359b0,0x471eb0,0x49d240,0x49d430,0x49d490,0x4e81fe]




def native_storage(runtime,debug,meta,root):
    from mana_source_evidence import inspect_native
    return inspect_native(runtime,debug,meta,root)


def reconstruct(a,raw,runtime,meta):
    original=strip_comments(raw);body=BODIES[a].strip()+'\n';size=int(meta[a]['high_pc'],16)-int(a,16)
    return body,[dict(original=original,replacement=body,occurrences=1,kind='restore land-color replacement/cache events, native ID decision tree, tap source/control rules, flags and mana-count paths',machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))]


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('mana_source_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['debug_sha256']==sha(debug.data)
    from unify_scalar_helpers import retained_source_body
    for old in component['retained_helpers']:assert sha(retained_source_body(root,old['source'],old['name']).encode())==old['body_sha256']
    meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};assert component['native_storage']==native_storage(runtime,debug,meta,root)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,e in entries.items():
        detail=component['functions'][a];assert e['mana_source_evidence']==detail and e['static']==(a in PRIVATE)
        body=function_body(sources[e['source']].replace('\r\n','\n'),e['name']);assert sha(body.encode())==e['reconstruction_body_sha256'],(a,e['name'],body[:120])
        raw=read(root/'.recovery'/('decompiled-mana-sources' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==e['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        assert function_body(cleaned(text,overlap),e['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    for pending in component.get('pending_memorial_callee_bindings',[]):
        caller=next(f for f in manifest['accepted_functions'] if f['address']==pending['caller'])
        assert caller['source']==pending['source'] and caller['name']==pending['caller_name']
        body=function_body(sources.get(caller['source'],read(root/caller['source'])).replace('\r\n','\n'),caller['name'])
        assert re.search(r'\b'+pending['legacy_name']+r'\b',body)
        assert pending['recovered_name']==NAMES[pending['target']]
        assert machine_bytes(int(pending['address'],16),len(bytes.fromhex(pending['bytes']))).hex()==pending['bytes']
    for old in component['retained_original_handlers']:assert sha(function_body(read(root/old['source']),old['name']).encode())==old['body_sha256']
    for a in PUBLIC_HELPERS:assert entries[a]['prototype'] in read(root/'manalink.h') or component.get('planning')




def pending_memorial_bindings(manifest,interfaces,runtime,root):
    entries={f['address']:f for f in manifest['accepted_functions']};pending=[]
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,detail in interfaces['memorial_family_native_2022']['functions'].items():
        code=runtime.pe.get_data(int(a,16)-0x2000000,detail['machine_bytes'])
        caller=entries[a];body=function_body(read(root/caller['source']),caller['name'])
        for ins in decoder.disasm(code,int(a,16)):
            if ins.mnemonic not in {'call','jmp'} or ins.operands[0].type!=X86_OP_IMM:continue
            target=f'{ins.operands[0].imm:x}'
            if target not in ADDRESSES:continue
            legacy=NAMES[target].removesuffix('_2022');assert re.search(r'\b'+legacy+r'\b',body)
            assert not any(site['address']==f'{ins.address:x}' for site in detail['canonical_calls'])
            pending.append(dict(caller=a,source=caller['source'],caller_name=caller['name'],legacy_name=legacy,target=target,recovered_name=NAMES[target],address=f'{ins.address:x}',bytes=ins.bytes.hex()))
    assert len(pending)==2 and {p['target'] for p in pending}=={TAPPED,PRODUCER}
    return pending


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_storage(runtime,debug,meta,root);(cache/'export-request-mana-sources.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared thirteen public land and creature mana-source helpers.');return
    assert 'mana_source_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    retained=[('cards/avacyn_restored.c' if a==ENCHANTED else 'functions/produce_mana.c',meta[a]['name']) for a in ADDRESSES]+[('functions/produce_mana.c','basic_subtype_to_color_test')]
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_storage=native_storage(runtime,debug,meta,root),pending_memorial_callee_bindings=pending_memorial_bindings(m,interfaces,runtime,root),**debug_evidence(debug,meta,ADDRESSES),functions={},
        retained_helpers=[dict(name=n,source=source,body_sha256=sha(function_body(read(root/source),n).encode())) for source,n in retained],retained_original_handlers=[],
        scope='Recover thirteen public land/creature mana sources and replacement queries in functions/produce_mana.c, preserving old providers including the Aura helper retained in cards/avacyn_restored.c. Preserve native added-color/subtype event flow, ID decision tree, count-derived activation amount, quantity/color overrides, transmutation, mixed-controller Aura indices, trigger/static humiliation modes, creature restrictions and exact tap/target/cancellation bookkeeping. All previous records and fragments remain unchanged; downstream caller/gameplay integration remains work.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-mana-sources'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=[],mana_source_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(next(e['prototype'] for e in entries if e['address']==a) for a in PUBLIC_HELPERS)+'\n#endif\n\n'+header[at:]
    # Match check_source.py's existing MSVC-host compatibility alias, only in
    # the disposable syntax-check copy. GCC's original source stays unchanged.
    syntax_sources={s:('#define strncasecmp _strnicmp\n'+t if s=='functions/rules_engine.c' else t) for s,t in sources.items()}
    compile_sources(root,cache/'mana-sources-check',syntax_sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in [r for r in csv.DictReader((cache/'decompiled-mana-sources/export.tsv').open(),delimiter='\t') if r['address'] in ADDRESSES]:rows[row['address']]=row;shutil.copyfile(cache/'decompiled-mana-sources'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=13;m['helper_functions']+=13;m['shared_helper_interfaces']+=13
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);(root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));interfaces['mana_source_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'mana-sources-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=13)))


if __name__=='__main__':main()
