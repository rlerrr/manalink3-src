"""Recover native rules-text parsing, two card consumers and Bloodletter character folding."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image,value,inherited_value
from export_abi import type_of,type_name
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read,rename
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings,readonly_string,byte_literal
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

RULES='2302de0';PUNCT='2305a70';CAPITAL='25c6880';BLOOD='2305090'
ADDRESSES=[RULES,PUNCT,CAPITAL,BLOOD];HELPERS={RULES};PRIVATE=set();ALIASES={};LEGACY=[];NAMES={}
LABEL='/* October 2022 native rules-text normalization. */'


def native_tables(runtime):
    from recover_letter_effects import native_tables as letter_tables
    runtime.pe.parse_data_directories();imports=[(d.dll.decode(),s.name.decode(),s.address) for d in runtime.pe.DIRECTORY_ENTRY_IMPORT for s in d.imports if s.name and s.name==b'strpbrk']
    assert imports==[('msvcrt.dll','strpbrk',0x27b5514)]
    assert runtime.pe.get_data(0x701da8,6)==bytes.fromhex('ff2514557b02')
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    punctuation=get(0x273f29a,100).split(b'\0')[0]
    assert punctuation==b'!?,;:\x96\x97()/"\x94\x93\'\x91\x92&.'
    assert get(0x273ef1b,11)==b'\n--FLIP--\n\0' and get(0x273ef26,7)==b'0BUGRW\0' and get(0x273ef2d,6)==b'BUGRW\0'
    return dict(character_fold=letter_tables(runtime)['character_fold'],punctuation=dict(address='273f29a',bytes=(punctuation+b'\0').hex()),strpbrk=dict(thunk='2701da8',iat='27b5514',dll='msvcrt.dll',name='strpbrk',bytes='ff2514557b02'),pattern_bytes=300,rules_vla_element_bytes=1,card_pointer_table='73bb00',name_offset=8,rules_offset=116,next_card_name_delta=4,flip_marker=dict(address='273ef1b',bytes=get(0x273ef1b,11).hex()),generic_two_members=dict(address='273ef26',bytes=get(0x273ef26,7).hex()),color_members=dict(address='273ef2d',bytes=get(0x273ef2d,6).hex()))


def reconstruct(a,raw,runtime,meta):
    from rules_text_bodies import RULES_PREFIX
    text=strip_comments(raw);changes=[];size=int(meta[a]['high_pc'],16)-int(a,16)
    block=dict(machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))
    def change(old,new,kind):
        nonlocal text
        count=text.count(old);assert count>0,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,**block));text=text.replace(old,new)
    tables=native_tables(runtime)
    if a==RULES:
        start=text.index('  uStackY_280 = 0x2302dfe;');end=text.index('  cVar8 = *pcVar12;',start)
        change(text[start:end],RULES_PREFIX,'restore strlen+1 char VLA, exact copy and flip marker, two 300-byte pattern buffers and next-card name at native table base+4')
        start=text.index('        pcVar12 = dfcbackpat1 + iVar2 + -0x10;');end=text.index('        if (pcVar5 ==',start)
        change(text[start:end],"        pcVar5 = strchr(pcVar7, ')');\n",'restore reminder-close search physical frame')
        start=text.index('    *(int *)(dfcbackpat1 + iVar2 + -0x10) = (int)cVar8;');end=text.index('    if (pcVar12 !=',start)
        change(text[start:end],'    saved_code = cVar8;\n    pcVar12 = strchr("0BUGRW", (int)cVar8);\n    cVar8 = saved_code;\n','restore generic-two hybrid membership frame and saved low-byte character')
        start=text.index('      *(int *)(dfcbackpat1 + iVar2 + -0x10) = (int)cVar1;');end=text.index('    }\n    pcVar12 = pcVar7 + iVar3;',start)
        change(text[start:end],'      pcVar12 = strchr("BUGRW", (int)cVar1);\n      iVar3 = (pcVar12 != (char *)0) + 1;\n','restore colored/hybrid membership frame with unchanged source pointer')
        start=text.index('    *(undefined **)(dfcbackpat1 + iVar2 + -0x14) = &UNK_0273ef2d;');end=text.index('    if (pcVar7 !=',start)
        change(text[start:end],'    pcVar12 = pcVar7;\n    pcVar7 = strchr("BUGRW", (int)cVar1);\n','restore phyrexian membership frame and source cursor without synthetic spills')
        for decl in ['  int iVar2;\n','  uint uVar4;\n','  uint uVar6;\n','  uint *puVar9;\n','  uint *puVar10;\n','  uint uStackY_280;\n','  char acStack_270 [4];\n','  char *pcStack_26c;\n']:change(decl,'','remove resolved allocation/string-length/stack-copy scaffolding')
        change('  char cVar1;','  char cVar1;\n  char saved_code;','give reused low-byte character spill its own typed local')
        change('  char *pcVar13;','  char *pcVar13 = out_buf;','retain writable output cursor alias; assigned before every native use')
    if a==PUNCT:
        punct=byte_literal(bytes.fromhex(tables['punctuation']['bytes'])[:-1])
        change('  int iVar4;','  char *punct;','restore pointer return type of native strpbrk import')
        change('while (iVar4 = func_0x02701da8(pcVar3,&UNK_0273f29a), iVar4 != 0)', 'while ((punct = strpbrk(pcVar3, '+punct+')) != NULL)','restore native strpbrk import and all punctuation bytes, including Windows quote/dash values')
        change('pcVar3 = (char *)(iVar4 + 1);','pcVar3 = punct + 1;','restore one-byte step after each punctuation match')
    if a==CAPITAL:
        change('  char *pcVar3;','  char *pcVar3;\n  char current_char;','restore register character separate from the local rules buffer')
        change("    if (rules[0] != '\\0')", "    current_char = rules[0];\n    if (current_char != '\\0')",'restore initial character load after text normalization')
        change('rules[0] + 0xbfU','current_char + 0xbfU','preserve unsigned ASCII uppercase range check')
        change('rules[0] = *pcVar3;','current_char = *pcVar3;','restore next-byte register load rather than a fictitious write to rules[0]')
        change("while (rules[0] != '\\0')", "while (current_char != '\\0')",'terminate at native byte cursor NUL')
    if a==BLOOD:
        fold=bytes.fromhex(tables['character_fold']['bytes']);initializer='  static const unsigned char native_name_fold[256] = {\n'+''.join('    '+', '.join(f'0x{c:02x}' for c in fold[i:i+16])+',\n' for i in range(0,256,16))+'  };\n'
        change('  byte bVar6;\n',initializer+'  byte bVar6;\n','restore complete native character-fold table without the empty-string lookup artifact')
        change('""[*pbVar4]','native_name_fold[*pbVar4]','restore native folded first-letter load')
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('rules_text_consumer_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['native_tables']==native_tables(runtime)
    assert entries[BLOOD]['previous_recovery']==component['retained_previous_record']
    previous=component['retained_previous_record'];assert previous['address']==BLOOD and previous['name']=='card_bloodletter' and not previous.get('reconstruction_body_sha256')
    assert '""[*pbVar4]' in component['retained_previous_body']
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['rules_text_consumer_evidence']==detail and not entry['static']
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-rules-text' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,ALIASES);assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==2
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        if a==RULES:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len(entries)==4 and sum(e['name'].startswith('card_') for e in entries.values())==3
    assert machine_bytes(0x2302e69,7)==bytes.fromhex('8b04bd04bb7300')
    assert machine_bytes(0x2302edc,6)==bytes.fromhex('c702202d2000') and machine_bytes(0x2302f15,5)==bytes.fromhex('66c7022900')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        (cache/'export-request-rules-text.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared rules-text helper, two card consumers and one card repair.');return
    assert 'rules_text_consumers_native_2022' not in interfaces and not (set(ADDRESSES)-{BLOOD})&{f['address'] for f in m['accepted_functions']}
    previous=next(f for f in m['accepted_functions'] if f['address']==BLOOD)
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_tables=native_tables(runtime),**debug_evidence(debug,meta,ADDRESSES),unit_layouts=native_layouts(debug,{'cards/unglued_unhinged.c','cards/unstable.c'}),functions={},
        retained_previous_record=previous,retained_previous_body=function_body(read(root/previous['source']),previous['name']),
        scope='Recover the native rules-text normalizer, Punctuate and Capital Offense in original Unsets files. Restore character VLA, both 300-byte back-face patterns, next-card name lookup, physical string frames, original pipe-token walker and exact punctuation byte set. Repair Bloodletter folding and two native dependency bindings, retaining its exact previous record/body. Preserve parser output-pointer quirks, malformed-input behavior, all legacy source and out-of-scope GUI stubs; gameplay integration remains downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-rules-text'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,ALIASES);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,rules_text_consumer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));
        if a==BLOOD:
            entries[-1]['previous_recovery']=previous
            original=function_body(sources.get(source,read(root/source)),row['name'])
            sources[source]=sources.get(source,read(root/source)).replace(original,function_body(body,row['name']))
        else:sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS-PRIVATE)+'\n#endif\n\n'+header[at:];compile_sources(root,cache/'rules-text-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not (set(ADDRESSES)-{BLOOD})&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-rules-text/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-rules-text'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']=[next(e for e in entries if e['address']==BLOOD) if f['address']==BLOOD else f for f in m['accepted_functions']]+[e for e in entries if e['address']!=BLOOD];m['accepted']+=3;m['card_handler_functions']+=2;m['helper_functions']+=1;m['shared_helper_interfaces']+=1
        for e in [e for e in entries if e['address']!=BLOOD]:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['rules_text_consumers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'rules-text-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=2,new_helpers=1,repaired_cards=1)))


if __name__=='__main__':main()
