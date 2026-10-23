"""Recover the five Disciple frontends and their four-argument native worker."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from migrate_token_callback_records import source_body
from audit_source import function_body,target_validation_input

SOURCE='cards/apocalypse.c'
WORKER='205ffb0'
CARDS={'2061850':('ana',6,'COLOR_TEST_BLUE | COLOR_TEST_BLACK','COLOR_BLACK, COLOR_BLUE'),
       '2062680':('ceta',24,'COLOR_TEST_RED | COLOR_TEST_GREEN','COLOR_GREEN, COLOR_RED'),
       '2063de0':('dega',18,'COLOR_TEST_RED | COLOR_TEST_BLACK','COLOR_BLACK, COLOR_RED'),
       '2066c10':('necra',40,'COLOR_TEST_WHITE | COLOR_TEST_GREEN',None),
       '20680a0':('raka',36,'COLOR_TEST_WHITE | COLOR_TEST_BLUE',None)}
ADDRESSES=[WORKER,*CARDS]
NAMES={'ap_disciples':'ap_disciples_2022'}
CONTROLLER_SITES={'2060005':'e836026000','2060029':'899d00ffffff',
                  '20600d5':'e866016000','20600f9':'899d00ffffff'}
RDATA_SITE={'address':'20602fb','bytes':'c744244488587202'}


def reconstruct(raw):
    text=strip_comments(raw);changes=[]
    old='  td.preferred_controller = player;\n';assert text.count(old)==1
    changes.append(dict(original=old,replacement='',occurrences=1,
        kind='remove decompiler-hoisted target preference',machine_sites=[dict(address=a,bytes=b) for a,b in CONTROLLER_SITES.items()]))
    text=text.replace(old,'')
    for call in ['default_target_definition(player,card,td_00,2);','default_target_definition(player,card,&td,2);']:
        assert text.count(call)==1
        replacement=call+'\n    td.preferred_controller = player;'
        changes.append(dict(original=call,replacement=replacement,occurrences=1,
            kind='restore target preference after initializer',machine_sites=[dict(address=a,bytes=b) for a,b in CONTROLLER_SITES.items()]))
        text=text.replace(call,replacement)
    return text,changes


def original_call(a):
    return 'return ap_disciples(player, card, event, '+CARDS[a][3]+');'


def native_call(a):
    return 'return ap_disciples_2022(player, card, event, '+CARDS[a][2]+');'


def replacement_call(a):
    return '#ifndef SHANDALAR\n\t'+native_call(a)+'\n#else\n\t'+original_call(a)+'\n#endif'


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('disciple_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    source=sources[SOURCE].replace('\r\n','\n')
    assert sha(function_body(source,'ap_disciples').encode())==component['retained_legacy_body_sha256']
    layout=component['unit_layouts'][SOURCE]['target_definition_t']
    assert layout['bytes']==84 and layout['members']['preferred_controller']['offset']==8
    assert [(v['name'],v['location']) for v in component['locals'][WORKER] if v['name'] in {'td','td2','td3'}]==[
        ('td','(DW_OP_fbreg: -272)'),('td2','(DW_OP_fbreg: -188)'),('td3','(DW_OP_fbreg: -104)')]
    parameters=component['parameters'][WORKER]
    assert [p['name'] for p in parameters]==['player','card','event','colors']
    assert [p['location'][0]['expression'] for p in parameters[:3]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    assert parameters[3]['location']=='(DW_OP_fbreg: 0)'
    assert component['enum_values']=={'COLOR_TEST_BLACK':2,'COLOR_TEST_BLUE':4,'COLOR_TEST_GREEN':8,'COLOR_TEST_RED':16,'COLOR_TEST_WHITE':32}
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['disciple_evidence']==detail
        body=function_body(source,entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256']
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        assert detail['native_coff_storage']==(3 if a==WORKER else 2)
        for call in detail['canonical_calls']:
            ins=insns[call['address']];assert ins.bytes.hex()==call['bytes'] and ins.mnemonic in {'call','jmp'}
            assert ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==int(call['target'],16)
            assert re.search(r'\b'+call['name']+r'\s*\(',body)
        raw_dir='decompiled-disciples' if component.get('planning') else 'decompiled-abi'
        raw=read(root/'.recovery'/raw_dir/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        if a==WORKER:
            text,changes=reconstruct(raw);assert changes==detail['changes']
            assert target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
            assert machine_bytes(int(RDATA_SITE['address'],16),8).hex()==RDATA_SITE['bytes']
            literal=next(x for x in entry['recovered_literals'] if x['symbol']=='_rdata')
            assert literal['address']=='0x2725888' and bytes.fromhex(literal['bytes'])==b'Prevent damage'
            runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes())
            text,literals=recover_strings(text,runtime,{'_rdata':0x2725888});assert literals==entry['recovered_literals']
            text,_=rename(text,NAMES)
            overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
            assert function_body(cleaned(text,overlap),entry['name'])==body
            assert body.count('td.preferred_controller = player;')==2
            assert 'target_definition_t' not in body and 'target_definition_2022_t' in body
        else:
            assert detail['machine_bytes']==25 and len(detail['canonical_calls'])==1
            instructions=list(insns.values());assert instructions[-1].mnemonic=='jmp'
            assert [i.bytes.hex() for i in instructions[:5]]==['55','89e5','8b4508','8b4d10','8b550c']
            assert instructions[5].bytes.hex()=='c74508'+CARDS[a][1].to_bytes(4,'little').hex()
            assert instructions[6].bytes.hex()=='5d' and native_call(a) in body
            if CARDS[a][3]:
                restored=body.replace(replacement_call(a),original_call(a))
                assert sha(restored.encode())==component['previous_wrapper_body_sha256'][a]


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'disciple_native_2022' not in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());source=read(root/SOURCE)
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{SOURCE}),functions={},
        retained_legacy_body_sha256=sha(function_body(source,'ap_disciples').encode()),previous_wrapper_body_sha256={},
        scope='Recover the four-argument color-mask worker and five Disciple tail wrappers. Preserve legacy helper and the three existing handlers\' SHANDALAR calls. Restore target controller preference after initialization using native stores; preserve native activation priority, mana handling and resolution control flow.')
    proof['enum_values']={}
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!=SOURCE:continue
        for die in cu.iter_DIEs():
            name=value(die,'DW_AT_name')
            if die.tag=='DW_TAG_enumerator' and name in {'COLOR_TEST_BLACK','COLOR_TEST_BLUE','COLOR_TEST_GREEN','COLOR_TEST_RED','COLOR_TEST_WHITE'}:
                proof['enum_values'][name]=value(die,'DW_AT_const_value')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets[int(WORKER,16)]=dict(name='ap_disciples_2022',source=SOURCE,static=True)
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n);decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];entries=[];bodies={}
    for a in ADDRESSES:
        raw=read(cache/'decompiled-disciples'/f'{a}.c');row=meta[a];name=NAMES.get(row['name'],row['name']);literals=[];changes=[]
        if a==WORKER:
            text,changes=reconstruct(raw);text,literals=recover_strings(text,runtime,{'_rdata':0x2725888});text,_=rename(text,NAMES);body='static '+cleaned(text,overlap)
        elif CARDS[a][3]:
            old=source_body(source,name);proof['previous_wrapper_body_sha256'][a]=sha(function_body(old,name).encode());assert old.count(original_call(a))==1
            body='int '+old.replace(original_call(a),replacement_call(a))
        else:body=f'int {name}(int player, int card, event_t event)\n{{\n  {native_call(a)}\n}}'
        assert not UNRESOLVED.search(strip_comments(body)),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert storage==([3] if a==WORKER else [2]),(a,storage)
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            if ins.mnemonic not in {'call','jmp'} or ins.operands[0].type!=X86_OP_IMM or ins.operands[0].imm not in targets:continue
            target=targets[ins.operands[0].imm];assert re.search(r'\b'+target['name']+r'\s*\(',body),(a,target['name'])
            assert not target['static'] or target['source']==SOURCE
            detail['canonical_calls'].append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{ins.operands[0].imm:x}',name=target['name']))
        proof['functions'][a]=detail;bodies[a]=body
        entries.append(dict(address=a,name=name,source=SOURCE,line=row['line'],static=a==WORKER,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,disciple_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
    def marker(a):return f'/* ManalinkEh 2022: 0x{a}; original {SOURCE}:{meta[a]["line"]}. */\n'
    first=source.index('int card_ana_disciple(')
    source=source[:first]+'#ifndef SHANDALAR\n'+marker(WORKER)+bodies[WORKER]+'\n#endif\n\n'+source[first:]
    for a in CARDS:
        if CARDS[a][3]:
            old='int '+source_body(source,meta[a]['name']);assert source.count(old)==1
            source=source.replace(old,marker(a)+bodies[a])
        else:source+='\n\n#ifndef SHANDALAR\n'+marker(a)+bodies[a]+'\n#endif\n'
    compile_sources(root,cache/'disciples-check',{SOURCE:source})
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},{SOURCE:source},get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-disciples/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-disciples'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        (root/SOURCE).write_bytes(source.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=6;m['card_handler_functions']+=5;m['helper_functions']+=1;m['private_functions']+=1;m['private_helpers']+=1;m['source_files'][SOURCE]+=6
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['disciple_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'disciples-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=5,new_helpers=1)))


if __name__=='__main__':main()
