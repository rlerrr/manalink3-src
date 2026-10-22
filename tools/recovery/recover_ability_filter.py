"""Correct the native ability-suppression filter and migrate its two old-layout callers."""
import argparse,copy,csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from promote_recovery import clean,strip_comments,ABILITY_FILTER_API_NAMES
from recover_test_consumers import read,rename
from recover_spell_copy import compile_sources,format_c
from recover_temporary_exile import save_json,sha
from recover_phase_prompt_cards import debug_evidence
from audit_source import function_body

HELPER='26dda70'
CALLERS=['2191920','23c7630']
MAPPING={'test_definition_t':'test_definition_2022_t','default_test_definition':'default_test_definition_2022',**ABILITY_FILTER_API_NAMES}
HEADER_LABEL='/* October 2022 ability suppression with the native filter layout. */'
MASK_SITES={'26dda76':'837d0c34','26dda7c':'837d0c6c','26ddab5':'8b158c397a00','26ddabb':'85c9',
    '26ddabd':'742a','26ddaca':'894c240c','26ddad5':'e80611feff','26ddaf5':'8b9000010000',
    '26ddafb':'85d2','26ddafd':'7808','26ddb02':'09d1','26ddb0a':'898800010000'}


def historical_scalar_view(entry,body,binding):
    """Replay a later identifier-only migration before auditing an older binding."""
    migration=entry['ability_filter_caller_migration_2022'];previous=migration['previous_record']
    if migration['renames']!=MAPPING or binding not in previous or previous[binding]!=entry[binding]:
        raise ValueError('Ability-filter predecessor scalar proof mismatch')
    body=body.replace('\r\n','\n')
    if sha(body.encode())!=migration['body_sha256']:
        raise ValueError('Ability-filter current body changed before historical replay')
    restored,_=rename(body,{v:k for k,v in MAPPING.items()})
    if sha(restored.encode())!=migration['previous_body_sha256'] or previous.get('reconstruction_body_sha256')!=migration['previous_body_sha256']:
        raise ValueError('Ability-filter historical scalar body mismatch')
    return previous,restored


def audit_component(component,manifest,sources,machine_bytes):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions']};entry=entries[HELPER];detail=component['helper']
    require(entry['name']=='remove_abilities_by_test_2022' and not entry['static'] and entry['ability_filter_evidence']==detail,'Ability-filter native helper identity mismatch')
    require(entry['test_interface_2022']==dict(type='test_definition_2022_t',definition_present=True,calls={'new_make_test_in_play':'new_make_test_in_play_2022'}),
        'Ability-filter native test binding mismatch')
    old=component['previous_helper_record'];require(old['address']==HELPER and old['name']=='remove_abilities_by_test' and old['raw_sha256']==entry['raw_sha256'],
        'Ability-filter previous native record mismatch')
    source=sources[entry['source']].replace('\r\n','\n')
    require(sha(function_body(source,old['name']).encode())==component['legacy_body_sha256'],'Ability-filter retained 208-byte compatibility body changed')
    require(set(component['callers'])==set(CALLERS),'Ability-filter caller membership mismatch')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,migration in component['callers'].items():
        current=entries[a];previous=migration['previous_record'];body=function_body(sources[current['source']].replace('\r\n','\n'),current['name'])
        require(current['ability_filter_caller_migration_2022']==migration and migration['renames']==MAPPING,'Ability-filter caller migration mismatch')
        restored,_=rename(body,{v:k for k,v in MAPPING.items()})
        require(sha(restored.encode())==migration['previous_body_sha256'] and sha(body.encode())==migration['body_sha256'],
            'Ability-filter caller changed outside recorded identifiers')
        if previous.get('native_entry_callee_binding_2022'):
            require(historical_scalar_view(current,body,'native_entry_callee_binding_2022')==(previous,restored),'Ability-filter historical entry binding failed')
        native=machine_bytes(int(a,16),migration['machine_bytes']);require(sha(native)==migration['machine_sha256'],'Ability-filter caller native hash mismatch')
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        require({s['name'] for s in migration['canonical_calls']}=={'default_test_definition_2022','remove_abilities_by_test_2022'},'Ability-filter caller dependency coverage mismatch')
        for site in migration['canonical_calls']:
            ins=instructions[site['address']]
            require(ins.bytes.hex()==site['bytes'] and ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==int(site['target'],16) and
                re.search(r'\b'+site['name']+r'\s*\(',body),'Ability-filter caller native callee mismatch')
        require('test_definition_2022_t test;' in body and 'remove_abilities_by_test(' not in body,'Ability-filter caller native record layout missing')
        offset=-236 if a=='2191920' else -232
        require(any(v['name']=='test' and v['type']=='test_definition_t' and v['location']==f'(DW_OP_fbreg: {offset})' for v in component['locals'][a]),
            'Ability-filter caller DWARF stack object mismatch')
    require([p['name'] for p in component['parameters'][HELPER]]==['player','event','test','mode'],'Ability-filter physical argument order mismatch')
    require([p['location'] for p in component['parameters'][HELPER][:3]]==[f'(DW_OP_fbreg: {n})' for n in [0,4,8]] and
        all(v['expression']=='(DW_OP_fbreg: 12)' for v in component['parameters'][HELPER][3]['location']),'Ability-filter physical argument locations mismatch')
    require(component['layout']==dict(test_bytes=216,instance_bytes=300,ability_loss_offset=256,ability_loss_bytes=4),'Ability-filter record/slot layout mismatch')
    for a,data in MASK_SITES.items():require(machine_bytes(int(a,16),len(bytes.fromhex(data))).hex()==data,'Ability-filter native event/null/filter/mask instruction mismatch')


def audit_card(component,manifest,sources,machine_bytes):
    from bind_delayed_callback_callers import audit_component as audit_bindings
    audit_bindings(component,manifest,sources,machine_bytes,expected={'21cfde0':HELPER},targets={HELPER:'remove_abilities_by_test_2022'})
    entry=next(f for f in manifest['accepted_functions'] if f['address']=='21cfde0')
    if entry.get('ability_filter_interface_2022')!={'calls':ABILITY_FILTER_API_NAMES} or not entry.get('wish_interface_2022',{}).get('dialog_selection_stubbed'):
        raise ValueError('Karn filter/Wish interface limitation mismatch')
    site=component['wish_site'];decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    data=bytes.fromhex(site['bytes']);ins=list(decoder.disasm(machine_bytes(int(site['address'],16),len(data)),int(site['address'],16)))
    if not (len(ins)==1 and ins[0].bytes==data and ins[0].mnemonic=='call' and ins[0].operands[0].type==X86_OP_IMM and ins[0].operands[0].imm==0x20590c0 and
        site['target']=='20590c0' and site['name']=='generic_wish_effect_2022' and 'generic_wish_effect_2022(' in function_body(sources[entry['source']],entry['name'])):
        raise ValueError('Karn native Wish callee mismatch')


def bind_card(root,apply):
    cache=root/'.recovery';m=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text());ip=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(ip.read_text())
    assert 'karn_ability_filter_2022' not in interfaces
    a='21cfde0';entry=next(f for f in m['accepted_functions'] if f['address']==a);meta=next(r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc']=='0x'+a)
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta['high_pc'],16)-int(a,16));decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    source=read(root/entry['source']);sites=[];wish=[]
    for ins in decoder.disasm(native,int(a,16)):
        if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==int(HELPER,16):
            sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=HELPER,name='remove_abilities_by_test_2022'))
        if ins.mnemonic=='call' and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==0x20590c0:
            wish.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target='20590c0',name='generic_wish_effect_2022'))
    proof=dict(functions={a:dict(machine_bytes=len(native),machine_sha256=sha(native),sites=sites,body_sha256=sha(function_body(source,entry['name']).encode()))},
        scope='Karn native static-ability filter connects to the corrected 216-byte helper; Wish dialog selection remains separately stubbed.')
    assert len(wish)==1;proof['wish_site']=wish[0]
    audit_card(proof,m,{entry['source']:source},lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if apply:interfaces['karn_ability_filter_2022']=proof;save_json(ip,interfaces)
    print(json.dumps(dict(applied=apply,karn_filter_call_sites=len(sites))))


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--bind-card',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    if args.bind_card:bind_card(root,args.apply);return
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text())
    assert 'ability_filter_2022' not in interfaces
    entries={f['address']:f for f in m['accepted_functions']};meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data);runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    proof=dict(debug_sha256=sha(debug_data),**debug_evidence(debug,meta,[HELPER]+CALLERS),previous_helper_record=copy.deepcopy(entries[HELPER]),callers={},
        layout=dict(test_bytes=216,instance_bytes=300,ability_loss_offset=256,ability_loss_bytes=4),
        scope='Correct one earlier native helper record and two earlier callers to use the recovered 216-byte filter API. Retain the 208-byte compatibility implementation. No new helper address is counted.')
    sources={entries[a]['source']:read(root/entries[a]['source']) for a in [HELPER]+CALLERS}
    entry=entries[HELPER];source=entry['source'];proof['legacy_body_sha256']=sha(function_body(sources[source],entry['name']).encode())
    marker=f'/* ManalinkEh 2022: 0x{HELPER}; original {source}:{entry["line"]}. */';assert sources[source].count(marker)==1
    sources[source]=sources[source].replace(marker,'/* Retained 208-byte test-layout compatibility implementation.\n * The native 2022 entry is remove_abilities_by_test_2022 below. */',1)
    raw=read(cache/'decompiled-abi'/f'{HELPER}.c');overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    body=format_c(clean(raw,overlap,use_test_api_2022=True,use_ability_filter_api_2022=True))
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']};targets[int(HELPER,16)]='remove_abilities_by_test_2022'
    native=runtime.pe.get_data(int(HELPER,16)-0x2000000,int(meta[HELPER]['high_pc'],16)-int(HELPER,16));calls=[]
    for ins in decoder.disasm(native,int(HELPER,16)):
        if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
            name=targets[ins.operands[0].imm];assert re.search(r'\b'+name+r'\s*\(',body),name
            calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=name))
    proof['helper']=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=[],canonical_calls=calls)
    entry.update(name='remove_abilities_by_test_2022',prototype=body[:body.index('{')].strip()+';',ability_filter_evidence=proof['helper'],
        reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,'remove_abilities_by_test_2022').encode()),
        test_interface_2022=dict(type='test_definition_2022_t',definition_present=True,calls={'new_make_test_in_play':'new_make_test_in_play_2022'}))
    sources[source]+='\n\n#ifndef SHANDALAR\n'+marker+'\n'+body+'\n#endif\n'
    for a in CALLERS:
        entry=entries[a];before=function_body(sources[entry['source']],entry['name']);after,count=rename(before,MAPPING);assert count>=3 and sources[entry['source']].count(before)==1
        sources[entry['source']]=sources[entry['source']].replace(before,after,1)
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));calls=[]
        for ins in decoder.disasm(native,int(a,16)):
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and targets.get(ins.operands[0].imm) in {'default_test_definition_2022','remove_abilities_by_test_2022'}:
                calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=targets[ins.operands[0].imm]))
        migration=dict(previous_record=copy.deepcopy(entry),previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),renames=MAPPING,
            machine_bytes=len(native),machine_sha256=sha(native),canonical_calls=calls);proof['callers'][a]=migration
        entry.update(ability_filter_caller_migration_2022=migration,reconstruction=proof['scope'],reconstruction_body_sha256=migration['body_sha256'],
            test_interface_2022=dict(type='test_definition_2022_t',definition_present=True,calls={'default_test_definition':'default_test_definition_2022'}),
            ability_filter_interface_2022=dict(calls=ABILITY_FILTER_API_NAMES))
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');prototype=entries[HELPER]['prototype']
    header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+prototype+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'ability-filter-check',sources,header);audit_component(proof,m,sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if args.apply:
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));save_json(mp,m);interfaces['ability_filter_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'ability-filter-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_implementations=0,corrected_native_helper_records=1,migrated_callers=2)))


if __name__=='__main__':main()
