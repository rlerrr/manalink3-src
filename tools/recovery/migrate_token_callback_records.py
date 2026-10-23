"""Migrate native token callbacks and their accepted consumers to the 76-byte ABI."""
import argparse,copy,csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value
from export_abi import type_of,type_name
from promote_recovery import TOKEN_API_NAMES,strip_comments,LITERAL
from recover_test_consumers import read,rename
from recover_phase_prompt_cards import debug_evidence
from recover_spell_copy import compile_sources
from recover_temporary_exile import save_json,sha
from audit_source import function_body

CALLBACKS={'22c95e0':'give_token_haste','2619110':'haste_and_must_attack_until_eot',
           '23c76d0':'add_hofry_legacy','21c7140':'ugins_token_processing'}
CALLERS={'22ce550':['22c95e0'],'23c6dc0':['23c76d0'],'23d0750':['23c76d0'],'21d8740':['21c7140']}
ADDRESSES=set(CALLBACKS)|set(CALLERS)


def source_body(text,name):
    """Keep inline recovery comments while locating one definition's exact span."""
    masked=re.sub(LITERAL+r'|/\*.*?\*/|//[^\n]*',lambda m:re.sub(r'[^\n]',' ',m[0]),text,flags=re.S)
    match=re.search(r'\b'+re.escape(name)+r'\s*\([^;{}]*\)\s*\{',masked)
    assert match,name
    depth=1
    for at in range(match.end(),len(masked)):
        if masked[at]=='{':depth+=1
        elif masked[at]=='}':
            depth-=1
            if depth==0:return text[match.start():at+1]
    raise ValueError('Unclosed token consumer definition: '+name)


def historical_view(entry,body,binding=None):
    migration=entry['token_record_migration_2022'];previous=migration['previous_record']
    body=body.replace('\r\n','\n')
    if sha(body.encode())!=migration['body_sha256']:raise ValueError('Token migration current body mismatch')
    restored,_=rename(body,{new:old for old,new in migration['renames'].items()})
    if sha(restored.encode())!=migration['previous_body_sha256']:raise ValueError('Token migration exact predecessor mismatch')
    if binding and (binding not in previous or entry[binding]!=previous[binding]):raise ValueError('Token migration predecessor interface mismatch')
    return previous,restored


def audit_component(component,manifest,sources,machine_bytes):
    entries={f['address']:f for f in manifest['accepted_functions']}
    assert set(component['functions'])==ADDRESSES
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,migration in component['functions'].items():
        entry=entries[a];body=function_body(sources[entry['source']],entry['name'])
        assert entry['token_record_migration_2022']==migration
        previous,restored=historical_view(entry,body)
        assert entry['raw_sha256']==previous['raw_sha256'] and entry['static']==previous['static']
        assert entry['token_interface_2022']['type']=='token_generation_2022_t'
        assert 'token_generation_t' not in body and 'token_generation_2022_t' in body
        assert entry['reconstruction_body_sha256']==migration['body_sha256']
        native=machine_bytes(int(a,16),migration['machine_bytes']);assert sha(native)==migration['machine_sha256']
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        for call in migration['canonical_calls']:
            ins=instructions[call['address']]
            assert ins.bytes.hex()==call['bytes'] and ins.mnemonic in {'call','jmp'}
            assert ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==int(call['target'],16)
            assert re.search(r'\b'+call['name']+r'\s*\(',body)
        assert set(entry['token_interface_2022']['calls'])=={c['original'] for c in migration['canonical_calls']}
        assert component['unit_layouts'][entry['source']]['bytes']==76
        if a in CALLBACKS:
            assert entry['prototype']==previous['prototype'].replace('token_generation_t','token_generation_2022_t')
            ps=component['parameters'][a]
            assert [p['type'] for p in ps]==['token_generation_t *','int','int']
            for p,expected in zip(ps,['(DW_OP_fbreg: 0)','(DW_OP_fbreg: 4)','(DW_OP_fbreg: 8)']):
                location=p['location']
                assert location==expected or (isinstance(location,list) and location and all(v['expression']==expected for v in location))
        else:
            assert entry['prototype']==previous['prototype']
            assert any(v['name']=='token' and v['type']=='token_generation_t' for v in component['locals'][a])
        for reference in migration['callback_references']:
            ins=instructions[reference['address']];target=entries[reference['target']]
            assert ins.bytes.hex()==reference['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(reference['target'],16) for o in ins.operands)
            assert target['source']==entry['source'] and target['static'] and target['name'] in body
        assert {r['target'] for r in migration['callback_references']}==set(CALLERS.get(a,[]))
        if previous.get('stack_storage_reconstruction'):
            assert sha(restored.encode())==previous['stack_storage_reconstruction']['body_sha256']
        for declaration in migration['forward_declarations']:
            source=sources[entry['source']].replace('\r\n','\n')
            assert declaration['replacement'] in source and declaration['original'] not in source


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'token_record_migration_2022' not in interfaces
    entries={f['address']:f for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data)
    proof=dict(debug_sha256=sha(data),**debug_evidence(debug,meta,sorted(ADDRESSES)),unit_layouts={},functions={},
        scope='Identifier-only migration of four native token callbacks and four accepted consumers to the already recovered 76-byte token layout/API. Preserve complete predecessor records, exact body replay and earlier scalar/stack/local-enum evidence. No implementation counted again; legacy token interfaces remain available elsewhere.')
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {entries[a]['source'] for a in ADDRESSES}:continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_typedef' and value(die,'DW_AT_name')=='token_generation_t':
                record=type_of(die)
                members={value(c,'DW_AT_name'):dict(offset=value(c,'DW_AT_data_member_location'),type=type_name(type_of(c))) for c in record.iter_children() if c.tag=='DW_TAG_member'}
                assert value(record,'DW_AT_byte_size')==76 and len(members)==22
                assert {n:members[n]['offset'] for n in ['s_player','s_card','t_player','action_argument','special_code_on_generation','special_flags3']}==dict(s_player=4,s_card=8,t_player=20,action_argument=52,special_code_on_generation=64,special_flags3=72)
                proof['unit_layouts'][source]=dict(bytes=76,members=members)
        cu._dielist.clear();cu._diemap.clear()
    assert set(proof['unit_layouts'])=={entries[a]['source'] for a in ADDRESSES}
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    sources={entries[a]['source']:read(root/entries[a]['source']) for a in ADDRESSES}
    targets={int(f['address'],16):(n,f['name']) for f in m['accepted_functions'] for n,new in TOKEN_API_NAMES.items() if new==f['name']}
    for a in sorted(ADDRESSES):
        entry=entries[a];source=entry['source'];raw_before=source_body(sources[source],entry['name']);before=function_body(raw_before,entry['name'])
        mapping={'token_generation_t':'token_generation_2022_t'}
        mapping.update({n:new for n,new in TOKEN_API_NAMES.items() if re.search(r'\b'+n+r'\s*\(',before)})
        actions=sorted(set(re.findall(r'\bTOKEN_ACTION_\w+',before)))
        mapping.update({n:n.replace('TOKEN_ACTION_','TOKEN2022_ACTION_') for n in actions})
        raw_after,count=rename(raw_before,mapping);after=function_body(raw_after,entry['name']);assert count>0 and sources[source].count(raw_before)==1
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));calls=[];references=[]
        for ins in decoder.disasm(native,int(a,16)):
            if ins.mnemonic in {'call','jmp'} and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm in targets:
                original,name=targets[ins.operands[0].imm]
                assert mapping.get(original)==name
                calls.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=name,original=original))
            for target in CALLERS.get(a,[]):
                if any(o.type==X86_OP_IMM and o.imm==int(target,16) for o in ins.operands):
                    references.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=target,name=CALLBACKS[target]))
        assert {c['original'] for c in calls}==set(mapping)&set(TOKEN_API_NAMES)
        migration=dict(previous_record=copy.deepcopy(entry),previous_body_sha256=sha(before.encode()),body_sha256=sha(after.encode()),renames=mapping,
            machine_bytes=len(native),machine_sha256=sha(native),canonical_calls=calls,callback_references=references,forward_declarations=[])
        sources[source]=sources[source].replace(raw_before,raw_after,1)
        if a in CALLBACKS:
            for match in list(re.finditer(r'(?m)^static void '+entry['name']+r'\([^;{}]*\);',sources[source])):
                original=match[0];replacement=original.replace('token_generation_t','token_generation_2022_t')
                if original!=replacement:
                    sources[source]=sources[source].replace(original,replacement,1)
                    migration['forward_declarations'].append(dict(original=original,replacement=replacement))
            entry['prototype']=entry['prototype'].replace('token_generation_t','token_generation_2022_t')
        entry.update(token_record_migration_2022=migration,reconstruction=proof['scope'],reconstruction_body_sha256=migration['body_sha256'],
            token_interface_2022=dict(type='token_generation_2022_t',calls={n:new for n,new in mapping.items() if n in TOKEN_API_NAMES},actions=actions))
        proof['functions'][a]=migration
    compile_sources(root,cache/'token-record-migration-check',sources)
    audit_component(proof,m,sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if args.apply:
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        save_json(mp,m);interfaces['token_record_migration_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'token-record-migration-plan.json',proof)
    print(json.dumps(dict(applied=args.apply,migrated_callbacks=4,migrated_existing_consumers=4,new_implementations=0)))


if __name__=='__main__':main()
