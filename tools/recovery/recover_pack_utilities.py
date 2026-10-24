"""Recover native pack tables, availability/sizing/announcement and private choice helpers."""
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

from pack_native_tables import decode,render
APPEND='26eb550';ADD='26eb660';VALID='26ec840';SIZE='26ec8a0';ANNOUNCE='26ed870'
ADDRESSES=[APPEND,ADD,VALID,SIZE,ANNOUNCE];HELPERS=set(ADDRESSES);PRIVATE={APPEND,ADD};ALIASES={};LEGACY=[]
NAMES={'add_to_csvid_choices':'add_to_csvid_choices_2022','add_card_to_pack':'add_card_to_pack_2022','expansion_t':'EXPANSION_t'}
LABEL='/* October 2022 native pack expansion identifiers and public utilities. */'
CLONE_INTERFACES='26eb550\tadd_to_csvid_choices_2022\tvoid\t__regparm3\tint_ptr:choice_num\tint_ptr_ptr:choice_csvids\tint:csvid\n26eb660\tadd_card_to_pack_2022\tint\t__regparm3\tint_ptr:out_csvids\tint:pos\tint_ptr:choice_num\tint_ptr_ptr:choice_csvids\tint:num\tint:rarity\tint:expansion\tint:booster\tint:testing_mode\n'


def native_interfaces(runtime,debug):
    runtime.pe.parse_data_directories();imports={s.address:dict(dll=d.dll.decode(),name=s.name.decode()) for d in runtime.pe.DIRECTORY_ENTRY_IMPORT for s in d.imports if s.name}
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    assert imports[0x27b54a0]==dict(dll='msvcrt.dll',name='fprintf') and get(0x2701e80,6)==bytes.fromhex('ff25a0547b02')
    assert imports[0x27b546c]==dict(dll='msvcrt.dll',name='_iob')
    assert get(0x2722d04,4)==struct.pack('<I',0x27025f0) and get(0x27025f0,14)==bytes.fromhex('8b442404c1e00503056c547b02c3')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    from capstone.x86_const import X86_REG_ECX
    insns=list(decoder.disasm(get(0x26ec840,81),0x26ec840));assert all(X86_REG_ECX not in i.regs_access()[1] for i in insns) and not any(i.mnemonic=='call' for i in insns)
    assert get(0x26ec8a9,3)==bytes.fromhex('8b4d08') and get(0x26ec8c3,3)==bytes.fromhex('8d0449')
    return dict(stderr=dict(pointer_slot='2722d04',helper='27025f0',helper_bytes=get(0x27025f0,14).hex(),iob_import='27b546c',file_bytes=32,index=2),fprintf=dict(thunk='2701e80',bytes=get(0x2701e80,6).hex(),iat='27b54a0',dll='msvcrt.dll'),valid_preserves_ecx=True,
                clone_interfaces=CLONE_INTERFACES,clones=[dict(address=a,coff_name=n,register_words=3,stack_words=0 if a==APPEND else 6) for a,n in [(APPEND,'_add_to_csvid_choices.isra.0'),(ADD,'_add_card_to_pack.isra.0')]])


def reconstruct(a,raw,runtime,meta):
    text=strip_comments(raw);changes=[];size=int(meta[a]['high_pc'],16)-int(a,16)
    block=dict(machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))
    def change(old,new,kind):
        nonlocal text
        count=text.count(old);assert count>0,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,**block));text=text.replace(old,new)
    if a in {VALID,SIZE}:
        change('rarity_num_t *','const rarity_num_t *','restore native immutable rarity-description pointer views')
    if a==VALID:
        change('expansion < (EXPANSION_STREETS_OF_NEW_CAPENNA|EXPANSION_LIMITED_EDITION_ALPHA)','(uint32_t)expansion < 198U','make the native unsigned expansion range explicit, including rejection of negative values')
    if a==SIZE:
        change('  int extraout_ECX;\n','','remove false call-clobbered register local; native validity predicate never writes ECX')
        change('pack_contents[extraout_ECX]','pack_contents[expansion]','restore expansion operand retained in ECX across native is_valid_pack')
    if a==ADD:
        change('(*_imp____acrt_iob_func)(2)','stderr','restore diagnostic stream from native iob shim: _iob + 2 * 32')
        change('func_0x02701e80','fprintf','restore native msvcrt fprintf import')
        for address in ['0276bda4','0276be14']:
            data=readonly_string(runtime,int(address,16));assert data
            change('&UNK_'+address,byte_literal(data),'restore byte-exact native pack diagnostic format')
            changes[-1].update(native_literal=dict(address=address,bytes=(data+b'\0').hex(),copy_bytes=len(data)+1),literal=data.decode())
    if a==ANNOUNCE:change('  char **ppcVar1;','  const char **ppcVar1;','restore borrowed immutable expansion-name table returned by existing text API')
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('pack_utility_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());assert component['native_tables']==decode(debug,runtime) and component['native_interfaces']==native_interfaces(runtime,debug)
    public,private,globals_=render(component['native_tables'],read(root/'Expansions.h'));assert component['public_types']==public and component['private_types']==private and component['globals']==globals_ and len(globals_)==44
    assert public in read(root/'Expansions.h') or component.get('planning')
    for g in globals_:assert g in manifest['source_globals'] and sources[g['source']].replace('\r\n','\n').count(g['declaration'])==1,(g['name'],'native table declaration mismatch')
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['pack_utility_evidence']==detail and entry['static']==(a in PRIVATE)
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-pack-utilities' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,ALIASES);assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
        if a not in PRIVATE:assert entry['prototype'] in read(root/'manalink.h') or component.get('planning')
    assert len(entries)==5 and not any(e['name'].startswith('card_') for e in entries.values())


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    from pack_native_tables import decode
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        (cache/'pack-utility-clones.tsv').write_text(CLONE_INTERFACES)
        decode(debug,runtime);(cache/'export-request-pack-utilities.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],NAMES.get(meta[a]['name'],meta[a]['name']),meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared five pack utilities and two concrete clone interfaces.');return
    assert 'pack_utilities_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    from pack_native_tables import decode,render
    tables=decode(debug,runtime);public_types,private_types,globals_=render(tables,read(root/'Expansions.h'))
    assert not (root/'functions/packs.c').exists()
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_tables=tables,globals=globals_,public_types=public_types,private_types=private_types,native_interfaces=native_interfaces(runtime,debug),**debug_evidence(debug,meta,ADDRESSES),functions={},
        scope='Recover five pack utilities and all 198 native booster/starter descriptions in a new original-source functions/packs.c unit. Restore the 199 native expansion enumerator names, 44 private initialized table objects, two field-split private helper interfaces, preserved-ECX expansion operand, const pool views, typed diagnostic stderr/fprintf boundary and native announcement controls. Preserve allocation/growth caps, duplicate handling, random draws, validity exclusions, conditional maximum sizing and original unchecked paths. Pack generation, its card consumers and pack-selection GUI remain pending; no substitute implementation or new GUI stub is introduced.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=NAMES.get(meta[a]['name'],meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={'functions/packs.c':'#include "manalink.h"\n\n#ifndef SHANDALAR\n'+private_types+'\n#endif\n'};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-pack-utilities'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,ALIASES);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,pack_utility_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources[source]+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+LABEL+'\n#ifndef SHANDALAR\n'+'\n'.join(e['prototype'] for e in entries if e['address'] in HELPERS-PRIVATE)+'\n#endif\n\n'+header[at:];scratch=cache/'pack-utilities-check';scratch.mkdir(exist_ok=True);(scratch/'defs.h').write_bytes((root/'defs.h').read_bytes());expansion_header=read(root/'Expansions.h');expansion_header=expansion_header.replace('\t// Beware!',public_types+'\t// Beware!');(scratch/'Expansions.h').write_bytes(expansion_header.encode('latin1'));compile_sources(root,scratch,sources,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries,'source_globals':m['source_globals']+globals_},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-pack-utilities/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-pack-utilities'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'Expansions.h').write_bytes(expansion_header.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=5;m['helper_functions']+=5;m['private_functions']+=2;m['private_helpers']+=2;m['shared_helper_interfaces']+=3;m['source_globals']+=globals_
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['pack_utilities_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'pack-utilities-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=0,new_helpers=5,new_global_tables=44)))


if __name__=='__main__':main()
