"""Recover Vivien and native flash casting with distinct private event contexts."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image,value
from export_abi import type_of,type_name
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

WAR='cards/war_of_the_spark.c'
FUNCTIONS='functions/functions.c'
CARD='21d8c90'
PUBLIC='2696b60'
ADDRESSES=['21c7a50','21c78a0','2691090',PUBLIC,CARD]
NAMES={'21c7a50':'vivien_has_mana_to_cast_target_2022','21c78a0':'vivien_flash_ability',
       '2691090':'flash_has_mana_to_cast_target_2022',PUBLIC:'can_play_cards_as_though_they_had_flash_2022',CARD:'card_vivien_champion_of_the_wilds'}
GLOBALS={WAR:('vivien_current_event_2022','2774010'),FUNCTIONS:('flash_current_event_2022','2775a74')}
HEADER_LABEL='/* October 2022 casting-as-flash API with native target definitions. */'
CARD_SITES={'21d8d54':'e817d05000','21d8d5d':'8985c4feffff','21d8d99':'83f803',
            '21d8d9e':'c785c4feffff03000000','21d8db6':'668945de','21d8e8a':'8b85c4feffff','21d8e90':'48'}


def reconstruct(a,raw):
    text=strip_comments(raw);changes=[]
    if a==CARD:
        replacements=[('  int iVar3;','  int iVar3;\n  int amount;'),
            ('ptStack_140 = (target_definition_t *)count_deck(player)','amount = count_deck(player)'),
            ('ptStack_140 != (target_definition_t *)0x0','amount != 0'),
            ('3 < (int)ptStack_140','3 < amount'),
            ('ptStack_140 = (target_definition_t *)0x3','amount = 3'),
            ('(ushort)ptStack_140','(ushort)amount'),('(int)ptStack_140 + -1','amount + -1')]
        for old,new in replacements:
            count=text.count(old);assert count==(2 if old=='(int)ptStack_140 + -1' else 1)
            changes.append(dict(original=old,replacement=new,occurrences=count,kind='split reused target pointer from signed deck count',machine_sites=[dict(address=a,bytes=b) for a,b in CARD_SITES.items()]))
            text=text.replace(old,new)
    return text,changes


def translations(a,source):
    return {'hack_current_event':GLOBALS[source][0],
            'has_mana_to_cast_target':NAMES['21c7a50' if source==WAR else '2691090'],
            'can_play_cards_as_though_they_had_flash':NAMES[PUBLIC]}


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('flash_casting_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    assert component['runtime_sha256']=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    for unit,layout in component['unit_layouts'].items():
        assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
        assert layout['test_definition_t']['members']['no_shuffle']['offset']==192 and layout['test_definition_t']['members']['create_minideck']['offset']==194
    assert any(v['name']=='td' and v['type']=='target_definition_t' and v['location']=='(DW_OP_fbreg: -104)' for v in component['locals']['21c78a0'])
    assert all(any(v['name']==name and v['type']==type_ and v['location']==location for v in component['locals'][CARD]) for name,type_,location in [
        ('td_creature','target_definition_t','(DW_OP_fbreg: -320)'),('test','test_definition_t','(DW_OP_fbreg: -236)')])
    assert any(v['name']=='amount' and v['type']=='int' and 'DW_OP_lit3' in str(v['location']) for v in component['locals'][CARD])
    for old,digest in component['retained_bodies'].items():assert sha(function_body(sources[FUNCTIONS].replace('\r\n','\n'),old).encode())==digest
    for unit,detail in component['globals'].items():
        name,address=GLOBALS[unit];assert detail['name']==name and detail['target_address']==address and detail['native_coff_storage']==3
        assert detail['dwarf_location']==[3,*int(address,16).to_bytes(4,'little')] and detail['type']=='event_t' and detail['bytes']==4
        assert detail['native_section']=='.bss' and detail['zero_initialized'] and detail['linkage']=='private'
        assert detail['declaration']==f'static event_t {name};' and sources[unit].count(detail['declaration'])==1
        assert {k:v for k,v in detail.items() if k in {'source','name','declaration','target_address','zero_initialized','linkage'}} in manifest['source_globals']
    worker_params=component['parameters'][PUBLIC]
    assert [p['name'] for p in worker_params]==['player','card','event','td','prompt','literal']
    assert [p['location'] if isinstance(p['location'],str) else p['location'][0]['expression'] for p in worker_params]==[f'(DW_OP_fbreg: {i*4})' for i in range(6)]
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes())
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['flash_casting_evidence']==detail
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-flash' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'];text,_=rename(text,translations(a,entry['source']))
        assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256']
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        assert detail['native_coff_storage']==(2 if a in {PUBLIC,CARD} else 3)
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes']
            assert any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body)
        for site in detail['global_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes']
            assert any(o.type==X86_OP_MEM and o.mem.disp==int(site['target'],16) for o in ins.operands)
            assert site['target']==GLOBALS[entry['source']][1] and site['name']==GLOBALS[entry['source']][0] and re.search(r'\b'+site['name']+r'\b',body)
        assert len(detail['global_references'])=={'21c7a50':1,'21c78a0':1,'2691090':1,PUBLIC:3,CARD:0}[a]
        if a in {'21c7a50','2691090'}:
            assert '"not enough mana"' in body and len(component['parameters'][a])==5
            assert ('get_internal_card_id_from_csv_id(' in body)==(a=='2691090')
        if a==CARD:
            assert 'int amount;' in body and 'test_definition_2022_t test;' in body and 'target_definition_2022_t td_creature;' in body
            assert '(target_definition_2022_t *)count_deck' not in body
    assert {r['target'] for r in component['functions']['21c78a0']['callback_references']}=={'21c7a50'}
    assert {r['target'] for r in component['functions'][PUBLIC]['callback_references']}=={'2691090'}
    assert any(r['target']=='21c78a0' for r in component['functions'][CARD]['callback_references'])
    assert entries[PUBLIC]['prototype'] in read(root/'manalink.h') or component.get('planning')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'flash_casting_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes())
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{WAR,FUNCTIONS}),globals={},functions={},
        retained_bodies={n:sha(function_body(read(root/FUNCTIONS),n).encode()) for n in ['has_mana_to_cast_target','can_play_cards_as_though_they_had_flash']},
        scope='Recover native flash casting and Vivien. Keep separate native private event contexts and same-named callbacks, their distinct ID/IID mana checks, 84-byte target interfaces and native callback addresses. Split a decompiler-reused target pointer from a signed, capped deck count. Preserve native chooser fallback, face-down exile behavior and resolution branches; retain the legacy helper and callback.')
    declarations=[]
    for cu in debug.dwarf().iter_CUs():
        source=value(cu.get_top_DIE(),'DW_AT_name')
        if source not in GLOBALS:continue
        name,address=GLOBALS[source]
        die=next(d for d in cu.get_top_DIE().iter_children() if d.tag=='DW_TAG_variable' and value(d,'DW_AT_name')=='hack_current_event')
        symbols=[s for s in debug.symbols if s[0]=='_hack_current_event' and s[2]>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[s[2]-1].VirtualAddress+s[1]==int(address,16)];assert len(symbols)==1 and symbols[0][4]==3
        section=next((name,s) for name,s in runtime.sections.items() if s.VirtualAddress<=int(address,16)-0x2000000<s.VirtualAddress+s.Misc_VirtualSize)
        assert section[0]=='.bss' and section[1].SizeOfRawData==0
        d=dict(source=source,name=name,declaration=f'static event_t {name};',target_address=address,zero_initialized=True,linkage='private');declarations.append(d)
        t=type_of(die)
        while t.tag=='DW_TAG_typedef':t=type_of(t)
        proof['globals'][source]=dict(d,dwarf_location=value(die,'DW_AT_location'),type=type_name(type_of(die)),bytes=value(t,'DW_AT_byte_size'),native_coff_storage=3,native_section=section[0])
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES[a],source=meta[a]['source'],static=a not in {CARD,PUBLIC}) for a in ADDRESSES})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={s:read(root/s) for s in GLOBALS};entries=[];bodies={}
    for source,(name,address) in GLOBALS.items():sources[source]+=f'\n\n#ifndef SHANDALAR\n/* October 2022 private event context at 0x{address}. */\nstatic event_t {name};\n#endif\n'
    for a in ADDRESSES:
        row=meta[a];source=row['source'];raw=read(cache/'decompiled-flash'/f'{a}.c');text,changes=reconstruct(a,raw);text,literals=recover_strings(text,runtime,{});text,_=rename(text,translations(a,source));body=cleaned(text,overlap)
        if a not in {PUBLIC,CARD}:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==([2] if a in {PUBLIC,CARD} else [3]),(a,storage)
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],global_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type==X86_OP_IMM and op.imm in targets:
                    target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                    site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
                if op.type==X86_OP_MEM and op.mem.disp==int(GLOBALS[source][1],16):detail['global_references'].append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=GLOBALS[source][1],name=GLOBALS[source][0]))
        proof['functions'][a]=detail;bodies[a]=body
        entries.append(dict(address=a,name=NAMES[a],source=source,line=row['line'],static=a not in {PUBLIC,CARD},prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,flash_casting_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,NAMES[a]).encode())))
        sources[source]+=f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER_LABEL+'\n#ifndef SHANDALAR\n'+next(e['prototype'] for e in entries if e['address']==PUBLIC)+'\n#endif\n\n'+header[at:]
    compile_sources(root,cache/'flash-casting-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries,'source_globals':m['source_globals']+declarations},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-flash/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-flash'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['source_globals']+=declarations;m['accepted']+=5;m['card_handler_functions']+=1;m['helper_functions']+=4;m['private_functions']+=3;m['private_helpers']+=3;m['shared_helper_interfaces']+=1
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['flash_casting_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'flash-casting-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=1,new_helpers=4,private_helpers=3)))


if __name__=='__main__':main()
