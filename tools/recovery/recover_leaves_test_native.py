"""Upgrade accepted leaves-play entries/callers to native tests; retain old APIs."""
import argparse,copy,csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value
from export_abi import type_of,type_name
from promote_recovery import LEAVES_TEST_API_NAMES,TEST_API_NAMES,TARGET_API_NAMES,strip_comments,UNRESOLVED
from recover_combat_mask_cards import cleaned
from recover_phase_prompt_cards import debug_evidence
from recover_test_consumers import read,rename
from recover_spell_copy import compile_sources
from recover_temporary_exile import save_json,sha
from migrate_token_callback_records import source_body
from format_recovery import definition_span
from audit_source import function_body

HELPERS=['26a3790','26a3a80']
CALLERS=['25acd50','25ace40','233cad0']
ADDRESSES=HELPERS+CALLERS
SOURCE='functions/functions.c'
HEADER_LABEL='/* October 2022 leaves-play trigger API with native test definitions. */'


def historical_view(entry,body,binding=None):
    migration=entry['leaves_test_caller_migration_2022'];previous=migration['previous_record']
    body=body.replace('\r\n','\n');assert sha(body.encode())==migration['body_sha256']
    restored,_=rename(body,{new:old for old,new in migration['renames'].items()})
    assert sha(restored.encode())==migration['previous_body_sha256']
    if binding:assert binding in previous and entry[binding]==previous[binding]
    return previous,restored


def native_layouts(debug,units):
    layouts={}
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in units:continue
        layouts[source]={}
        for d in cu.get_top_DIE().iter_children():
            name=value(d,'DW_AT_name')
            if d.tag!='DW_TAG_typedef' or name not in {'test_definition_t','target_definition_t'}:continue
            t=type_of(d);layouts[source][name]=dict(bytes=value(t,'DW_AT_byte_size'),members={value(x,'DW_AT_name'):dict(offset=value(x,'DW_AT_data_member_location'),type=type_name(type_of(x))) for x in t.iter_children() if x.tag=='DW_TAG_member'})
    assert set(layouts)==units
    for unit in layouts.values():
        assert unit['test_definition_t']['bytes']==216 and unit['target_definition_t']['bytes']==84
        assert unit['test_definition_t']['members']['special_selection_function']['offset']==196
        assert unit['test_definition_t']['members']['value_for_special_selection_function']['offset']==200
    return layouts


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions']}
    assert set(component['functions'])==set(ADDRESSES) and set(component['previous_records'])==set(ADDRESSES)
    assert len(component['legacy_fragments'])==2 and all(f in manifest['source_fragments'] for f in component['legacy_fragments'])
    for layout in component['unit_layouts'].values():
        assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
        assert layout['test_definition_t']['members']['special_selection_function']['offset']==196
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a in ADDRESSES:
        entry=entries[a];detail=component['functions'][a];previous=component['previous_records'][a]
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        assert sha(body.encode())==entry['reconstruction_body_sha256'] and entry['raw_sha256']==previous['raw_sha256'] and not entry['static']
        assert 'test_definition_t' not in body and 'test_definition_2022_t' in body
        assert entry['source']==previous['source']
        assert entry['prototype'] in read(root/'manalink.h') or a in CALLERS or component.get('planning')
        native=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(native)==detail['machine_sha256']
        insns={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        for call in detail['canonical_calls']:
            ins=insns[call['address']];assert ins.bytes.hex()==call['bytes'] and ins.mnemonic in {'call','jmp'}
            assert ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==int(call['target'],16)
            assert re.search(r'\b'+call['name']+r'\s*\(',body)
        if a in HELPERS:
            assert entry['leaves_test_evidence']==detail and entry['name']==previous['name']+'_2022'
            assert [p['location'] for p in component['parameters'][a]]==[f'(DW_OP_fbreg: {i*4})' for i in range(8 if a==HELPERS[0] else 7)]
            assert 'test_definition_2022_t *test' in entry['prototype']
            assert sha(function_body(sources[SOURCE].replace('\r\n','\n'),previous['name']).encode())==component['retained_body_hashes'][a]
        else:
            assert entry['leaves_test_caller_migration_2022']==detail and detail['previous_record']==previous
            restored_entry,restored=historical_view(entry,body);assert restored_entry==previous
            if previous.get('stack_storage_reconstruction'):
                assert sha(restored.encode())==previous['stack_storage_reconstruction']['body_sha256']
                assert 'target_definition_2022_t target;' in body and 'uint8_t bytes[216];' in body
                assert 'target_definition_t' not in body
            assert any(v['name']=='test' and v['type']=='test_definition_t' for v in component['locals'][a])
    soul=component['locals']['233cad0']
    assert all(any(v['name']==name and v['location']=='(DW_OP_fbreg: -236)' for v in soul) for name in ['td','test'])


def bind_card(root,apply):
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'ozolith_native_callers_2022' not in interfaces
    a='228c300';entry=next(f for f in m['accepted_functions'] if f['address']==a);meta=next(r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc']=='0x'+a)
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta['high_pc'],16)-int(a,16));decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    targets={int(f['address'],16):f for f in m['accepted_functions']};sites=[]
    for ins in decoder.disasm(native,int(a,16)):
        for op in ins.operands:
            if op.type==X86_OP_IMM and op.imm in targets:
                target=targets[op.imm];assert not target['static'] or target['source']==entry['source']
                sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=target['address'],name=target['name'],kind='call' if ins.mnemonic in {'call','jmp'} else 'reference'))
    proof=dict(address=a,source=entry['source'],machine_bytes=len(native),machine_sha256=sha(native),sites=sites,body_sha256=sha(function_body(read(root/entry['source']),entry['name']).encode()))
    audit_card(proof,m,{entry['source']:read(root/entry['source'])},lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if apply:interfaces['ozolith_native_callers_2022']=proof;save_json(ip,interfaces)
    print(json.dumps(dict(applied=apply,bound_cards=1,native_sites=len(sites))))


def audit_card(component,manifest,sources,machine_bytes):
    entries={f['address']:f for f in manifest['accepted_functions']};entry=entries[component['address']];assert entry['address']=='228c300'
    body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==component['body_sha256']
    native=machine_bytes(0x228c300,component['machine_bytes']);assert sha(native)==component['machine_sha256']
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;insns={f'{i.address:x}':i for i in decoder.disasm(native,0x228c300)}
    for site in component['sites']:
        ins=insns[site['address']];target=entries[site['target']]
        assert ins.bytes.hex()==site['bytes'] and site['name']==target['name'] and re.search(r'\b'+target['name']+r'\b',body)
        assert any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
        assert not target['static'] or target['source']==entry['source']
    assert {s['target'] for s in component['sites'] if s['kind']=='reference'}=={'22884c0'}
    assert any(s['target']=='26a3a80' and s['kind']=='call' for s in component['sites'])
    assert 'test_definition_2022_t test;' in body and 'test_leaves_play_2022(' in body


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--bind-card',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    if args.bind_card:bind_card(root,args.apply);return
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'leaves_test_native_2022' not in interfaces
    entries={f['address']:f for f in m['accepted_functions']};previous={a:copy.deepcopy(entries[a]) for a in ADDRESSES}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data)
    proof=dict(debug_sha256=sha(data),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{entries[a]['source'] for a in ADDRESSES}|{'cards/ikoria.c'}),functions={},previous_records=previous,legacy_fragments=[],retained_body_hashes={},
        scope='Native leaves-play mode/frontend with 216-byte tests; keep exact earlier 208-byte compatibility implementations as uncounted fragments. Migrate three accepted card callers by reversible identifier/type substitutions, preserving exact predecessor records and earlier scalar/stack evidence. No native address is counted again.')
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;sources={entries[a]['source']:read(root/entries[a]['source']) for a in ADDRESSES};header=read(root/'manalink.h');backups={};bodies={}
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets.update({int(a,16):entries[a]['name']+'_2022' for a in HELPERS})
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    for a in HELPERS:
        old=previous[a];source=sources[SOURCE];marker=f'/* ManalinkEh 2022: 0x{a};';at=source.index(marker);marker_end,start,end=definition_span(source[at:],old['name']);payload=source[at+marker_end:at+end].strip()
        begin=f'/* Retained legacy leaves-play test interface: {old["name"]} @ 0x{a}. */';finish=f'/* End retained legacy leaves-play test interface: {a}. */'
        backup='decompiled-support/'+a+'-legacy-leaves-test.c';raw=read(cache/'decompiled-abi'/f'{a}.c');assert sha(raw.encode())==old['raw_sha256'];backups[backup]=raw
        fragment=dict(source=SOURCE,original_function=old['name'],address=a,raw_export=backup,raw_sha256=old['raw_sha256'],start_marker=begin,end_marker=finish,body_sha256=sha(payload.encode()),scope='Exact earlier 208-byte test interface; excluded from native implementation counts.')
        proof['legacy_fragments'].append(fragment);proof['retained_body_hashes'][a]=sha(function_body(source,old['name']).encode());sources[SOURCE]=source[:at]+begin+source[at+marker_end:at+end]+'\n'+finish+source[at+end:]
        body=cleaned(raw,overlap);assert not UNRESOLVED.search(body) and old['name']+'_2022' in body
        native=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));calls=[]
        for ins in decoder.disasm(native,int(a,16)):
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
                name=targets[ins.operands[0].imm];assert re.search(r'\b'+name+r'\s*\(',body),(a,name)
                calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=name))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=[],canonical_calls=calls);proof['functions'][a]=detail;bodies[a]=body
        entry=dict(name=old['name']+'_2022',address=a,source=SOURCE,line=old['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=old['raw_sha256'],recovered_literals=[],leaves_test_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,old['name']+'_2022').encode()))
        entries[a]=entry
    new=[entries[a] for a in HELPERS];at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in new)+'\n#endif\n\n'+header[at:]
    sources[SOURCE]+='\n\n#ifndef SHANDALAR\n/* Recovered native October 2022 leaves-play test interfaces. */\n'+'\n'.join(f'\n/* ManalinkEh 2022: 0x{a}; original {SOURCE}:{previous[a]["line"]}. */\n'+bodies[a] for a in HELPERS)+'\n#endif\n'
    for a in CALLERS:
        entry=entries[a];old=previous[a];source=entry['source'];before_raw=source_body(sources[source],entry['name']);before=function_body(before_raw,entry['name'])
        mapping={'test_definition_t':'test_definition_2022_t'}
        mapping.update({n:v for n,v in {**TEST_API_NAMES,**LEAVES_TEST_API_NAMES}.items() if re.search(r'\b'+n+r'\s*\(',before)})
        if a=='233cad0':mapping.update({'target_definition_t':'target_definition_2022_t',**{n:v for n,v in TARGET_API_NAMES.items() if re.search(r'\b'+n+r'\s*\(',before)}})
        after_raw,_=rename(before_raw,mapping);after=function_body(after_raw,entry['name']);sources[source]=sources[source].replace(before_raw,after_raw,1)
        native=get(int(a,16),int(meta[a]['high_pc'],16)-int(a,16));calls=[]
        mapped_targets={int(f['address'],16):(original,f['name']) for f in entries.values() for original,name in mapping.items() if name==f['name']}
        for ins in decoder.disasm(native,int(a,16)):
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in mapped_targets:
                original,name=mapped_targets[ins.operands[0].imm];calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=name,original=original))
        assert {c['original'] for c in calls}==set(mapping)-{'test_definition_t','target_definition_t'}
        migration=dict(previous_record=old,previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),renames=mapping,machine_bytes=len(native),machine_sha256=sha(native),canonical_calls=calls)
        entry.update(leaves_test_caller_migration_2022=migration,reconstruction=proof['scope'],reconstruction_body_sha256=migration['body_sha256'],leaves_test_interface_2022=dict(translations={n:v for n,v in mapping.items() if n in LEAVES_TEST_API_NAMES}),test_interface_2022=dict(type='test_definition_2022_t',definition_present=True,calls={n:v for n,v in mapping.items() if n in TEST_API_NAMES}))
        if a=='233cad0':entry['target_interface_2022']=dict(type='target_definition_2022_t',definition_present=True,calls={n:v for n,v in mapping.items() if n in TARGET_API_NAMES})
        proof['functions'][a]=migration
    combined={**m,'accepted_functions':list(entries.values()),'source_fragments':m['source_fragments']+proof['legacy_fragments']}
    compile_sources(root,cache/'leaves-test-native-check',sources,header)
    proof['planning']=True;audit_component(proof,combined,sources,get,root);del proof['planning']
    if args.apply:
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'))
        for backup,text in backups.items():p=cache/backup;p.parent.mkdir(exist_ok=True);p.write_text(text)
        m['accepted_functions']=list(entries.values());m['source_fragments']+=proof['legacy_fragments'];save_json(mp,m);interfaces['leaves_test_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'leaves-test-native-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_implementations=0,upgraded_helpers=2,migrated_existing_cards=3,retained_legacy_interfaces=2)))


if __name__=='__main__':main()
