"""Recover native weighted pack generation, wrapper and original self-check entry point."""
import argparse,csv,json,re,shutil,struct
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

from pack_generator_tables import decode,render
NORMAL='26eb8e0';MAKE='26ec9b0';TEST='26eca90'
ADDRESSES=[NORMAL,MAKE,TEST];HELPERS=set(ADDRESSES);PRIVATE={NORMAL};ALIASES={};LEGACY=[]
NAMES={'make_pack_normal':'make_pack_normal_2022','make_pack':'make_pack_2022','expansion_t':'EXPANSION_t','csvid_t':'int','card_id_t':'int','anon_struct_8_2_06904ed2':'varying_copies_entry_t'}
LABEL='/* October 2022 native weighted pack generation and self-check entry point. */'


def native_interfaces(runtime,debug,root):
    from recover_pack_utilities import native_interfaces as utilities
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    utilities(runtime,debug)
    runtime.pe.parse_data_directories();imports={s.address:dict(dll=d.dll.decode(),name=s.name.decode()) for d in runtime.pe.DIRECTORY_ENTRY_IMPORT for s in d.imports if s.name}
    assert imports[0x27b5494]==dict(dll='msvcrt.dll',name='fflush') and get(0x2701e98,6)==bytes.fromhex('ff2594547b02')
    assert get(0x26ec9c6,3)==bytes.fromhex('8b4d10') and get(0x26ec9d8,2)==bytes.fromhex('85c9')
    assert get(0x26ec0ae,14)==bytes.fromhex('be801d7202b92600000089c7f3a5')
    return dict(stderr_pointer_slot='2722d04',fprintf_thunk='2701e80',fflush=dict(thunk='2701e98',bytes=get(0x2701e98,6).hex(),iat='27b5494',dll='msvcrt.dll'),varying_initializer_copy=dict(start='26ec0ae',bytes=get(0x26ec0ae,14).hex(),words=38),wrapper_output_pointer_preserved_in_ecx=True,
        retained_legacy_draft_helper=dict(source='cards/draft.c',name='make_pack',body_sha256=sha(function_body(read(root/'cards/draft.c'),'make_pack').encode())),rarity_layout=dict(bytes=5,default_expansion=0,default_rarity=1,dfc_symbol=3,flags=4,expansion_rarity_offset=5),subtype_array=dict(offset=132,elements=7,element_bytes=2,end_offset=146))


def reconstruct(a,raw,runtime,meta):
    tables=decode_tables_cached(runtime)
    text=strip_comments(raw);changes=[];size=int(meta[a]['high_pc'],16)-int(a,16)
    block=dict(machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))
    def change(old,new,kind):
        nonlocal text
        count=text.count(old);assert count>0,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,**block));text=text.replace(old,new)
    if a==NORMAL:
        unused,local,globals_=render(tables)
        change('        case 0x341:\n        }','        case 0x341:\n          ;\n        }','retain native no-op final switch arm with a C statement before the closing brace')
        change('  anon_struct_8_2_06904ed2 varying_copies [19];',local,'restore nineteen-entry native collation-weight initializer with all eighteen pointers and the null sentinel')
        change('  Rarities *pRVar12;','  const Rarities *pRVar12;','retain const Drawcardlib rarity view')
        change('rarity_num_t *','const rarity_num_t *','retain const native pack-description views')
        change('count_and_card_t *','const count_and_card_t *','retain immutable native collation-weight views')
        change('(&pRVar12[1].default_expansion)[expansion]','pRVar12->rarity_in_expansion[expansion]','restore native five-byte rarity header and flexible expansion-rarity byte tail')
        change('(int16_t *)&cards_ptr[uVar19]->mana_source_colors','cards_ptr[uVar19]->types + 7','restore exact native seven-entry subtype-array end at record offset 146')
        change('(csvid_choices_t *)&stack0xfffffff0','choices + 44','restore end of native 44-element choice-pool initialization')
        start=text.index('  paVar2 = varying_copies;');end=text.index('  while (pcVar25 !=',start)
        change(text[start:end],'  paVar2 = varying_copies;\n  pcVar25 = paVar2->counts_and_cards;\n','replace lowered 38-word initializer copy with typed initialized array and native first pointer')
        change('  draftrarity_t *pdVar20;\n','','remove resolved initializer-copy cursor')
        start=text.index('  ppcVar22 = &choices[0].csvids;',text.index('LAB_026ec3e0:'));end=text.index('  if ((testing_mode !=',start)
        change(text[start:end],'  for (iVar14 = 0; iVar14 < 44; ++iVar14) {\n    if (choices[iVar14].csvids != NULL) free(choices[iVar14].csvids);\n  }\n','restore all 44 native non-null pool frees without fictitious stack endpoint')
        change('  card_id_t **ppcVar22;','  int **ppcVar22;','restore int-pointer alias for the choice-pool field, matching helper physical interfaces')
        change('  uint8_t uVar3;','  static const int contraption_copies[3] = {16, 7, 2};\n  uint8_t uVar3;','restore exact native 16/7/2 contraption weight table')
        change('*(int *)(&_CSWTCH_173 + (uint)bVar6 * 4)','contraption_copies[(uint)bVar6]','restore unsigned byte-indexed native contraption lookup')
        change('  byte bVar26;','  byte bVar26;\n  _Static_assert(sizeof(Rarities) == 5 && offsetof(Rarities, rarity_in_expansion) == 5, "native rarity byte tail");\n  _Static_assert(offsetof(card_ptr_t, types) == 132 && offsetof(card_ptr_t, mana_source_colors) == 146, "native seven subtypes");','verify exported rarity and native subtype field views')
    if a==MAKE:
        change('  csvid_t *out_csvids_01;\n','','remove fictitious ECX-clobber output-pointer local')
        change('out_csvids_01','out_csvids','restore caller output pointer retained in ECX across validity predicate')
    if a==TEST:
        change('  expansion_t eVar1;','  static int once = 0;\n  expansion_t eVar1;','restore native once-only persistent zero-initialized gate')
        change('test_packs::once','once','restore static local scope')
        change('(float)(iVar7 - iStack_18)','(float)(int32_t)((uint32_t)iVar7 - (uint32_t)iStack_18)','preserve native 32-bit elapsed-counter subtraction before float conversion')
        change('func_0x02701e98','fflush','restore native msvcrt fflush import')
    if '(*_imp____acrt_iob_func)(2)' in text:change('(*_imp____acrt_iob_func)(2)','stderr','restore native stderr shim: _iob + 2 * 32')
    if 'func_0x02701e80' in text:change('func_0x02701e80','fprintf','restore native msvcrt fprintf import')
    for symbol in sorted(set(re.findall(r'&UNK_([0-9a-f]{8})',text))):
        data=readonly_string(runtime,int(symbol,16));assert data is not None,(a,symbol)
        change('&UNK_'+symbol,byte_literal(data),'restore byte-exact native diagnostic format')
        changes[-1].update(native_literal=dict(address=symbol,bytes=(data+b'\0').hex(),copy_bytes=len(data)+1),literal=data.decode())
    return text,changes


_TABLES=None

def decode_tables_cached(runtime):
    global _TABLES
    if _TABLES is None:_TABLES=decode(Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes()),runtime)
    return _TABLES


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('pack_generator_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['native_tables']==decode(debug,runtime) and component['native_interfaces']==native_interfaces(runtime,debug,root)
    private,local,globals_=render(component['native_tables']);assert component['private_types']==private and component['local_initializer']==local and component['globals']==globals_ and len(globals_)==18
    for g in globals_:assert g in manifest['source_globals'] and sources[g['source']].replace('\r\n','\n').count(g['declaration'])==1,g['name']
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['pack_generator_evidence']==detail and entry['static']==(a in PRIVATE)
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-pack-generator' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,ALIASES);assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        if a not in PRIVATE:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len(entries)==3 and not any(e['name'].startswith('card_') for e in entries.values())


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        decode(debug,runtime);(cache/'export-request-pack-generator.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared generator, native wrapper and original self-check function.');return
    assert 'pack_generator_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    tables=decode(debug,runtime);private_types,local_initializer,globals_=render(tables)
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_tables=tables,globals=globals_,private_types=private_types,local_initializer=local_initializer,native_interfaces=native_interfaces(runtime,debug,root),**debug_evidence(debug,meta,ADDRESSES),functions={},
        scope='Recover the full weighted pack generator, public native pack wrapper and original self-check entry point in functions/packs.c. Restore eighteen native count/card arrays, all 878 rows including mixed nonpositive sentinels, the nineteen-entry local pointer initializer, 44 choice pools and exact release retry/duplicate/DFC/land/rarity special cases. Recover const rarity views, seven-subtype bounds, three-entry contraption weights, preserved-ECX output pointer, static once gate and native stderr/fprintf/fflush boundaries. The old two-argument draft helper is retained with a qualified native public wrapper. No test entry point or game code is executed; GUI/card consumers remain pending.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={'functions/packs.c':read(root/'functions/packs.c')+'\n\n#ifndef SHANDALAR\n'+private_types+'\n#endif\n'};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-pack-generator'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,ALIASES);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,pack_generator_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS-PRIVATE)+'\n#endif\n\n'+header[at:];compile_sources(root,cache/'pack-generator-check',sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries,'source_globals':m['source_globals']+globals_},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-pack-generator/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-pack-generator'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=3;m['helper_functions']+=3;m['private_functions']+=1;m['private_helpers']+=1;m['shared_helper_interfaces']+=2;m['source_globals']+=globals_
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['pack_generator_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'pack-generator-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=3,new_global_tables=18)))


if __name__=='__main__':main()
