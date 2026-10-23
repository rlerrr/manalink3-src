"""Recover the specialized native Embalm worker and its ten missing consumers."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_REG_ESP
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

WORKER='22d74b0'
ADDRESSES='22d74b0 22d94b0 22d9520 22d9c50 22da5b0 22dafd0 22db120 22dee30 22e08c0 22e2560 22e35a0'.split()
NAME='embalm_2022_impl'
SOURCE='cards/amonkhet.c'
ENUMS=dict(EVENT_GRAVEYARD_ABILITY=176,EVENT_PAY_FLASHBACK_COSTS=181,
    EVENT_RESOLVE_ACTIVATED_GRAVEYARD_ABILITY=3328,EVENT_CAN_CAST=116,
    GA_EMBALM=2048,GA_BASIC=32,GAPAID_EXILE=2,SF3_EMBALMED_OR_ETERNALIZED=262144,COLOR_TEST_WHITE=32)
EVENT_SITES={'22d74bd':'81f9b0000000','22d74c5':'81f9b5000000','22d74d1':'81f9000d0000','22d74db':'83fb74'}
TOKEN_SITES={'22d7548':'8d7da8','22d7565':'66c745d02001','22d756b':'c745f000000400'}
ZERO_SITES={'22d74f3':'c744240800000000','22d7583':'c744240800000000'}


def native_specialization(debug):
    cu=next(c for c in debug.dwarf().iter_CUs() if value(c.get_top_DIE(),'DW_AT_name')==SOURCE)
    die=next(d for d in cu.get_top_DIE().iter_children() if value(d,'DW_AT_low_pc')==int(WORKER,16))
    params={inherited_value(p,'DW_AT_name'):p for p in die.iter_children() if p.tag=='DW_TAG_formal_parameter'}
    assert value(params['b'],'DW_AT_const_value')==0 and value(params['b'],'DW_AT_location') is None
    result={n:dict(location=value(p,'DW_AT_location'),constant=value(p,'DW_AT_const_value')) for n,p in params.items()}
    assert [result[n]['location'] for n in ['cid','x','u','g','r','w']]==[[145,i*4] for i in range(6)]
    assert set(params)=={'player','card','event','cid','x','b','u','g','r','w'}
    typedef=next(d for d in cu.get_top_DIE().iter_children() if d.tag=='DW_TAG_typedef' and value(d,'DW_AT_name')=='token_generation_t')
    t=type_of(typedef);token=dict(bytes=value(t,'DW_AT_byte_size'),members={value(x,'DW_AT_name'):dict(offset=value(x,'DW_AT_data_member_location'),type=type_name(type_of(x))) for x in t.iter_children() if x.tag=='DW_TAG_member'})
    assert token['bytes']==76 and [token['members'][n]['offset'] for n in ['color_forced','no_sleight','special_flags3']]==[40,41,72]
    enums={value(x,'DW_AT_name'):value(x,'DW_AT_const_value') for d in cu.get_top_DIE().iter_children() if d.tag=='DW_TAG_enumeration_type' for x in d.iter_children() if value(x,'DW_AT_name') in ENUMS}
    assert enums==ENUMS
    return dict(parameters=result,token_layout=token,enums=enums,native_symbol='_embalm.constprop.0',physical_parameters=9,register_parameters=['player','card','event'],stack_parameters=['cid','x','u','g','r','w'],black_mana=0)


def reconstruct(a,raw):
    text=strip_comments(raw);changes=[]
    if a==WORKER:
        substitutions=[('int event,int cid','event_t event,int cid',EVENT_SITES),
            ('event == 0xb0','event == EVENT_GRAVEYARD_ABILITY',{'22d74bd':EVENT_SITES['22d74bd']}),
            ('event == 0xb5','event == EVENT_PAY_FLASHBACK_COSTS',{'22d74c5':EVENT_SITES['22d74c5']}),
            ('event == 0xd00','event == EVENT_RESOLVE_ACTIVATED_GRAVEYARD_ABILITY',{'22d74d1':EVENT_SITES['22d74d1']}),
            ('event == 0x74','event == EVENT_CAN_CAST',{'22d74db':EVENT_SITES['22d74db']}),
            ('return 0x820;','return GA_EMBALM | GA_BASIC;',{'22d753b':'b820080000'}),
            ('return 2;','return GAPAID_EXILE;',{'22d75c0':'b802000000'}),
            ('token.special_flags3 = 0x40000;','token.special_flags3 = SF3_EMBALMED_OR_ETERNALIZED;',{'22d756b':TOKEN_SITES['22d756b']})]
        for old,new,sites in substitutions:
            assert text.count(old)==1,(a,old)
            changes.append(dict(original=old,replacement=new,occurrences=1,kind='restore native event/flag types and values after concrete clone signature',machine_sites=[dict(address=at,bytes=b) for at,b in sites.items()]));text=text.replace(old,new)
    return text,changes


def caller_arguments(code,a,body):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;insns=list(decoder.disasm(code,int(a,16)))
    calls=[i for i,x in enumerate(insns) if x.mnemonic=='call' and x.operands[0].type==X86_OP_IMM and x.operands[0].imm==int(WORKER,16)]
    assert len(calls)==1
    index=calls[0];stores={};sites=[]
    for ins in reversed(insns[:index]):
        if ins.mnemonic in {'call','ret'}:break
        if ins.mnemonic=='mov' and len(ins.operands)==2 and ins.operands[0].type==X86_OP_MEM and ins.operands[0].mem.base==X86_REG_ESP and ins.operands[1].type==X86_OP_IMM:
            offset=ins.operands[0].mem.disp
            if offset in range(0,24,4) and offset not in stores:
                stores[offset]=ins.operands[1].imm;sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex()))
    assert set(stores)==set(range(0,24,4)),(a,stores)
    match=re.search(NAME+r'\(player,\s*card,\s*event,\s*([^;]+?)\)',body);assert match,a
    args=[int(x.strip(),0) for x in match[1].split(',')];assert args==[stores[i] for i in range(0,24,4)]
    call=insns[index];return dict(arguments=dict(zip(['cid','x','u','g','r','w'],args)),stack_stores=sites,call=dict(address=f'{call.address:x}',bytes=call.bytes.hex(),target=WORKER))


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('embalm_family_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['specialization']==native_specialization(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()))
    for n,digest in component['retained_bodies'].items():assert sha(function_body(read(root/SOURCE),n).encode())==digest
    assert [p['location'][0]['expression'] for p in component['parameters'][WORKER][:3]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
    for at,b in {**EVENT_SITES,**TOKEN_SITES,**ZERO_SITES}.items():assert machine_bytes(int(at,16),len(bytes.fromhex(b))).hex()==b
    assert any(v['name']=='token' and v['location']=='(DW_OP_fbreg: -96)' for v in component['locals'][WORKER])
    assert component['unit_layouts'][SOURCE]['target_definition_t']['bytes']==84 and component['unit_layouts'][SOURCE]['test_definition_t']['bytes']==216
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['embalm_family_evidence']==detail and entry['static']==(a==WORKER)
        body=function_body(sources[SOURCE].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-embalm-family' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'] and function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a==WORKER else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body)
        if a==WORKER:
            assert entry['prototype'].count(',')==8 and 'token_generation_2022_t token;' in body
            assert 'has_mana_multi(player, x, 0, u, g, r, w)' in body and 'charge_mana_multi(player, x, 0, u, g, r, w)' in body
            assert 'spell_fizzled != 1' in body and 'SF3_EMBALMED_OR_ETERNALIZED' in body and 'special_code_on_generation' not in body
        else:
            assert detail['worker_arguments']==caller_arguments(code,a,body)
            assert len(component['parameters'][a])==3 and body.count(NAME+'(')==1
    assert 'target_definition_2022_t td2;' in function_body(sources[SOURCE],entries['22dee30']['name'])
    assert 'test_definition_2022_t test;' in function_body(sources[SOURCE],entries['22dee30']['name'])
    assert 'target_definition_2022_t td;' in function_body(sources[SOURCE],entries['22e35a0']['name'])


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'embalm_family_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());spec=native_specialization(debug)
    if args.prepare:
        (cache/'embalm-clone-interface.tsv').write_text(WORKER+'\t'+NAME+'\tint\t__regparm3\t'+'\t'.join('int:'+n for n in spec['register_parameters']+spec['stack_parameters'])+'\n')
        (cache/'export-request-embalm-family.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared verified nine-physical-parameter Embalm signature and eleven exports.');return
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),specialization=spec,**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{SOURCE}),functions={},
        retained_bodies={n:sha(function_body(read(root/SOURCE),n).encode()) for n in ['embalm','cb_embalm']},
        scope='Recover the nine-physical-parameter Embalm worker specialized to black mana zero and its ten missing native consumers. Preserve sorcery-speed/graveyard events, mana costs, fizzle gate, white nonsleighted token and direct embalmed flag; use native 76-byte token, 216-byte test and 84-byte target layouts. Check all sixty caller stack constants and native calls/callbacks; retain legacy worker and token callback unchanged.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAME if a==WORKER else meta[a]['name'],source=SOURCE,static=a==WORKER) for a in ADDRESSES})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];source=read(root/SOURCE);entries=[]
    for a in ADDRESSES:
        row=meta[a];assert row['source']==SOURCE;name=NAME if a==WORKER else row['name'];raw=read(cache/'decompiled-embalm-family'/f'{a}.c');text,changes=reconstruct(a,raw);text,literals=recover_strings(text,runtime,{});body=cleaned(text,overlap)
        if a==WORKER:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict',body),a
        symbols=[(n,s) for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert len(symbols)==1 and symbols[0][1]==(3 if a==WORKER else 2)
        if a==WORKER:assert symbols[0][0]==spec['native_symbol']
        code=runtime.pe.get_data(int(a,16)-0x2000000,int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=symbols[0][1])
        if a!=WORKER:detail['worker_arguments']=caller_arguments(code,a,body)
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==SOURCE
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=SOURCE,line=row['line'],static=a==WORKER,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,embalm_family_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        source+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {SOURCE}:{row["line"]}. */\n'+body+'\n#endif\n'
    compile_sources(root,cache/'embalm-family-check',{SOURCE:source})
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n);proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},{SOURCE:source},get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-embalm-family/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-embalm-family'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        (root/SOURCE).write_bytes(source.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=11;m['card_handler_functions']+=10;m['helper_functions']+=1;m['private_functions']+=1;m['private_helpers']+=1;m['source_files'][SOURCE]+=11
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['embalm_family_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'embalm-family-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=10,new_helpers=1,private_helpers=1)))


if __name__=='__main__':main()
