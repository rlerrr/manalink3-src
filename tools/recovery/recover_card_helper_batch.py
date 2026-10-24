"""Recover seven handlers together with their Escape, counter and targeting helpers."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_REG_ESP
from inspect_release import Image,value,inherited_value
from export_abi import type_of
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

WORKER='235a200'
ESCAPE_CARDS='235e820 235f610 2362150 23623d0 2362540'.split()
TARGET='2661a70'
COUNTERS='227ed50'
ADDRESSES=[WORKER]+ESCAPE_CARDS+[TARGET,'215b1f0',COUNTERS,'235ffa0']
HELPERS={WORKER,TARGET,COUNTERS}
NAMES={'escape':'escape_part_2022',
       'becomes_target_of_subtype_spell':'becomes_target_of_subtype_spell_2022',
       'test_enters_the_battlefield_with_counters':'test_enters_the_battlefield_with_counters_2022'}
GLOBAL=dict(source='functions/targets.c',name='targeted_subtype_spell_result_2022',
    declaration='static target_t targeted_subtype_spell_result_2022;',target_address='27748c8',zero_initialized=True,linkage='private')
ENUMS=dict(EVENT_CAST_SPELL=108,EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE=2304,
    EVENT_GRAVEYARD_ABILITY=176,EVENT_PAY_FLASHBACK_COSTS=181,EVENT_TRIGGER=125,
    EVENT_RESOLVE_TRIGGER=126,TRIGGER_SPELL_CAST=211,TYPE_LAND=1,TYPE_EFFECT=128,SF3_ESCAPE=524288)
SITES={
    WORKER:{'235a209':'83f96c','235a20e':'81f900090000','235a216':'81f9b0000000','235a220':'81f9b5000000',
            '235a233':'c744241400000000','235a25a':'c744240800000000','235a264':'e8d7f6ffff',
            '235a284':'c744240800000800','235a2a8':'8b451c','235a2b3':'89442410','235a2c2':'e899733200'},
    TARGET:{'2661a70':'813d782d7a00d3000000','2661ac1':'83e87d','2661ac4':'83f801','2661ac7':'77c3',
            '2661af6':'c744240881000000','2661b47':'a3c8487702','2661b51':'a3cc487702','2661b56':'b8c8487702'},
    '2362540':{'2362677':'8d438e','236267a':'83f801','236267d':'7607'},
    COUNTERS:{'227ed91':'e84afe4300','227ed9f':'897d18','227eda2':'89450c','227edaa':'897514',
              '227edad':'895d10','227edb0':'894508','227edba':'e9a1284000'}}


def specialization(debug):
    result={};enums={};global_proof=None
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in {'cards/theros_beyond_death.c','functions/targets.c'}:continue
        for d in cu.get_top_DIE().iter_children():
            if d.tag=='DW_TAG_enumeration_type':
                for e in d.iter_children():
                    n=value(e,'DW_AT_name')
                    if n in ENUMS:enums[n]=value(e,'DW_AT_const_value')
            if value(d,'DW_AT_low_pc')==int(WORKER,16):
                params={inherited_value(p,'DW_AT_name'):p for p in d.iter_children() if p.tag=='DW_TAG_formal_parameter'}
                result={n:dict(location=value(p,'DW_AT_location'),constant=value(p,'DW_AT_const_value')) for n,p in params.items()}
                assert set(params)=={'player','card','event','colorless','black','blue','green','red','white','num_exile','num_counters'}
                assert all(result[n]==dict(location=None,constant=0) for n in ['blue','white'])
                assert [result[n]['location'] for n in ['colorless','black','green','red','num_exile','num_counters']]==[[145,i*4] for i in range(6)]
            if value(d,'DW_AT_low_pc')==int(TARGET,16):
                v=next(x for x in d.iter_children() if x.tag=='DW_TAG_variable' and value(x,'DW_AT_name')=='rval')
                t=type_of(v)
                while t.tag=='DW_TAG_typedef':t=type_of(t)
                members={value(x,'DW_AT_name'):value(x,'DW_AT_data_member_location') for x in t.iter_children() if x.tag=='DW_TAG_member'}
                symbols=[s for s in debug.symbols if s[2]>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s[2]-1].VirtualAddress+s[1]==int(GLOBAL['target_address'],16)]
                assert len(symbols)==1 and symbols[0][4]==3 and debug.pe.sections[symbols[0][2]-1].Name.rstrip(b'\0')==b'.bss'
                global_proof=dict(GLOBAL,dwarf_location=value(v,'DW_AT_location'),native_name=symbols[0][0],native_storage=3,
                    bytes=value(t,'DW_AT_byte_size'),members=members)
                assert global_proof['bytes']==8 and members==dict(player=0,card=4)
                assert global_proof['dwarf_location']==[3,*int(GLOBAL['target_address'],16).to_bytes(4,'little')]
    assert enums==ENUMS and result and global_proof
    return dict(escape_parameters=result,enums=enums,result_storage=global_proof)


def reconstruct(a,raw):
    text=strip_comments(raw);changes=[]
    substitutions=[]
    if a==WORKER:
        substitutions=[('int event,int colorless','event_t event,int colorless'),
            ('event == 0x6c','event == EVENT_CAST_SPELL'),('event == 0x900','event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE'),
            ('event != 0xb0','event != EVENT_GRAVEYARD_ABILITY'),('event != 0xb5','event != EVENT_PAY_FLASHBACK_COSTS')]
    if a==TARGET:
        substitutions=[('_trigger_condition != 0xd3','trigger_condition != TRIGGER_SPELL_CAST'),
            ('event - EVENT_TRIGGER < 2','(unsigned int)event - (unsigned int)EVENT_TRIGGER < 2U'),
            ('trigger_cause,0x81','trigger_cause,TYPE_LAND | TYPE_EFFECT'),
            ('becomes_target_of_subtype_spell::rval','targeted_subtype_spell_result_2022')]
    if a=='2362540':substitutions=[('1 < event - EVENT_RESOLVE_ACTIVATION','1U < (unsigned int)event - (unsigned int)EVENT_RESOLVE_ACTIVATION')]
    for old,new in substitutions:
        count=text.count(old);assert count==(3 if '::rval' in old else 1),(a,old,count)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind='restore concrete native types, event range or private storage',
            machine_sites=[dict(address=at,bytes=b) for at,b in SITES[a].items()]))
        text=text.replace(old,new)
    return text,changes


def worker_arguments(code,a,body):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;instructions=list(decoder.disasm(code,int(a,16)))
    indexes=[i for i,x in enumerate(instructions) if x.mnemonic in {'call','jmp'} and x.operands[0].type==X86_OP_IMM and x.operands[0].imm==int(WORKER,16)]
    assert len(indexes)==1;index=indexes[0];stores={};sites=[]
    for ins in reversed(instructions[:index]):
        if ins.mnemonic in {'call','ret'}:break
        if ins.mnemonic=='mov' and len(ins.operands)==2 and ins.operands[0].type==X86_OP_MEM and ins.operands[0].mem.base==X86_REG_ESP and ins.operands[1].type==X86_OP_IMM:
            offset=ins.operands[0].mem.disp
            if offset in range(0,24,4) and offset not in stores:
                stores[offset]=ins.operands[1].imm;sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex()))
    assert set(stores)==set(range(0,24,4)),(a,stores)
    match=re.search(r'escape_with_counters_2022_impl\(player,\s*card,\s*event,\s*([^;]+?)\)',body);assert match,a
    args=[int(x.strip(),0) for x in match[1].split(',')];assert args==[stores[i] for i in range(0,24,4)]
    call=instructions[index];return dict(arguments=dict(zip(['colorless','black','green','red','num_exile','num_counters'],args)),stack_stores=sites,call=dict(address=f'{call.address:x}',bytes=call.bytes.hex()))


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('card_helper_batch_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['specialization']==specialization(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()))
    assert GLOBAL in manifest['source_globals'] and sources[GLOBAL['source']].count(GLOBAL['declaration'])==1
    for retained in component['retained_bodies']:
        assert sha(function_body(read(root/retained['source']),retained['name']).encode())==retained['body_sha256']
    assert component['unit_layouts']['cards/theros_beyond_death.c']['test_definition_t']['bytes']==216
    for a in ['2362540','235ffa0']:
        assert any(v['name']=='test' and v['type']=='test_definition_t' and v['location']=='(DW_OP_fbreg: -236)' for v in component['locals'][a])
    for a,sites in SITES.items():
        for at,b in sites.items():assert machine_bytes(int(at,16),len(bytes.fromhex(b))).hex()==b
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['card_helper_batch_evidence']==detail and entry['static']==(a==WORKER)
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-card-helper-batch' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,NAMES)
        assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a==WORKER else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body)
        if a in ESCAPE_CARDS:assert detail['worker_arguments']==worker_arguments(code,a,body)
        if a==WORKER:
            assert entry['prototype'].count(',')==8 and 'escape_part_2022(player, card, event, colorless, black, 0, green, red, 0, num_exile)' in body
            assert 'COUNTER_P1_P1, num_counters' in body and '0x80000' in body
            assert [component['parameters'][a][i]['location'][0]['expression'] for i in range(3)]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))','(DW_OP_reg1 (r1))']
        if a==COUNTERS:
            assert 'new_make_test_in_play_2022(' in body and 'enters_the_battlefield_with_counters(affected_card_controller, affected_card, event, type, number)' in ' '.join(body.split())
        if a in HELPERS-{WORKER}:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len(entries)==10 and sum(e['name'].startswith('card_') for e in entries.values())==7


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text())
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        specialization(debug)
        (cache/'escape-clone-interface.tsv').write_text(WORKER+'\tescape_with_counters_2022_impl\tint\t__regparm3\t'+'\t'.join('int:'+n for n in ['player','card','event','colorless','black','green','red','num_exile','num_counters'])+'\n')
        (cache/'export-request-card-helper-batch.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES))
        print('Prepared nine-physical-parameter Escape signature and ten exports.');return
    assert 'card_helper_batch_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),specialization=specialization(debug),**debug_evidence(debug,meta,ADDRESSES),
        unit_layouts=native_layouts(debug,{'cards/theros_beyond_death.c','cards/morningtide.c'}),functions={},
        retained_bodies=[dict(source=s,name=n,body_sha256=sha(function_body(read(root/s),n).encode())) for s,n in [
            ('cards/morningtide.c','test_enters_the_battlefield_with_counters'),('cards/theros_beyond_death.c','escape'),('cards/theros_beyond_death.c','escape_part_2022')]],
        scope='Batch seven handlers with three shared helpers: five Escape consumers, Renata and Fugitive Druid. Restore the nine-physical-argument Escape clone with blue/white constant zero, validate thirty caller stack constants, retain direct native worker binding, use 216-byte tests, preserve unsigned event gates and the eight-byte private targeting result. Existing compatibility bodies and all prior records remain unchanged.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name='escape_with_counters_2022_impl' if a==WORKER else NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a==WORKER) for a in ADDRESSES})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    sources[GLOBAL['source']]=read(root/GLOBAL['source'])+'\n\n#ifndef SHANDALAR\n/* October 2022 private targeting result at 0x27748c8 (native function-local static). */\n'+GLOBAL['declaration']+'\n#endif\n'
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-card-helper-batch'/f'{a}.c');text,changes=reconstruct(a,raw);text,literals=recover_strings(text,runtime,{});text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a==WORKER:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        symbols=[(n,s) for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)]
        assert len(symbols)==1 and symbols[0][1]==(3 if a==WORKER else 2)
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=symbols[0][1],native_coff_name=symbols[0][0])
        if a in ESCAPE_CARDS:detail['worker_arguments']=worker_arguments(code,a,body)
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a==WORKER,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,card_helper_batch_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+'/* October 2022 counter-entry and subtype-target trigger helpers. */\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS-{WORKER})+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'card-helper-batch-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries,'source_globals':m['source_globals']+[GLOBAL]},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-card-helper-batch/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-card-helper-batch'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['source_globals'].append(GLOBAL);m['accepted']+=10;m['card_handler_functions']+=7;m['helper_functions']+=3;m['private_functions']+=1;m['private_helpers']+=1;m['shared_helper_interfaces']+=2
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['card_helper_batch_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'card-helper-batch-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=7,new_helpers=3,private_helpers=1)))


if __name__=='__main__':main()
