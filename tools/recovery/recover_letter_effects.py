"""Recover letter-name card handlers and private text-effect helpers."""
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

CLONE='2302cb0';LETTERS='2302a30';SPARK_FX='2302d00';TOGGLE_FX='2302d80';SPARK='2303c00';TOGGLE='2306480';ABCS='2304fc0';CHOOSE='25c4e40';STAFF='25c8c00'
ADDRESSES=[CLONE,LETTERS,SPARK_FX,TOGGLE_FX,SPARK,TOGGLE,ABCS,CHOOSE,STAFF]
PRIVATE={CLONE,LETTERS,SPARK_FX,TOGGLE_FX,CHOOSE};HELPERS=PRIVATE;NAMES={};ALIASES={}
LABEL='/* October 2022 letter-name and text-effect helpers. */'


def native_tables(runtime):
    get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    fold=get(0x273f300,256);assert len(fold)==256
    assert fold[ord('A')]==ord('a') and fold[0xc6]==0xc6 and fold[0xdf]==0xdf
    ptrs=struct.unpack('<17I',get(0x27211c0,68));choices=[readonly_string(runtime,p) for p in ptrs]
    assert choices==[bytes([c]) for c in b'BCDFGHJKLMPQVWXYZ']
    return dict(character_fold=dict(address='273f300',bytes=fold.hex(),sha256=sha(fold)),consonants=dict(address='27211c0',bytes=get(0x27211c0,68).hex(),pointers=[f'{p:x}' for p in ptrs],choices=[s.decode() for s in choices]))


def reconstruct(a,raw,runtime,meta):
    text=strip_comments(raw);changes=[];size=int(meta[a]['high_pc'],16)-int(a,16)
    block=dict(machine_sites=[],machine_range=[a,meta[a]['high_pc'][2:]],machine_bytes=size,machine_sha256=sha(runtime.pe.get_data(int(a,16)-0x2000000,size)))
    def change(old,new,kind):
        nonlocal text
        count=text.count(old);assert count>0,(a,old)
        changes.append(dict(original=old,replacement=new,occurrences=count,kind=kind,**block));text=text.replace(old,new)
    tables=native_tables(runtime)
    if a in {LETTERS,STAFF}:
        fold=bytes.fromhex(tables['character_fold']['bytes'])
        initializer='  static const unsigned char native_name_fold[256] = {\n'+''.join('    '+', '.join(f'0x{c:02x}' for c in fold[i:i+16])+',\n' for i in range(0,256,16))+'  };\n'
        change('  byte bVar1;\n',initializer+'  byte bVar1;\n','restore full native 256-byte character-fold table; indexed loads are not an empty string')
        variable='bVar1' if a==LETTERS else 'uVar7'
        change('""['+variable+']','native_name_fold['+variable+']','restore zero-extended character index and native table operand')
    if a==LETTERS:
        change('  undefined1 auStackY_87 [79];\n','', 'remove false pre-array stack alias')
        change('auStackY_87[bVar1] = 1;','letter[bVar1 - 0x61] = 1;','restore alphabet bitmap at ebp-0x22: indexed store ebp+character-0x83')
        start=text.index('  pcVar6 = letter;');end=text.index('  return iVar5;',start)
        change(text[start:end],'  iVar5 = 0;\n  for (iVar3 = 0; iVar3 < 26; ++iVar3) {\n    iVar5 += letter[iVar3] != 0;\n  }\n','restore 26-byte bitmap reduction; native first byte plus 25 remaining bytes, ending at ebp-8')
        change('  undefined4 uStackY_38;\n','', 'remove unused synthetic return-PC declaration')
        for match in list(re.finditer(r'(?m)^ *uStackY_38 = 0x[0-9a-f]+;\n',text)):change(match[0],'','remove unused synthetic return-PC assignment')
    if a==CHOOSE:
        start=text.index('  puVar3 = (undefined4 *)&_data;');end=text.index('  if ((ai_is_speculating',start)
        change(text[start:end],'','replace native seventeen-word pointer copy with exact local pointer initializer')
        change('  char *choices [17];','  char *choices[17] = {'+', '.join(byte_literal(s.encode()) for s in tables['consonants']['choices'])+'};','restore native 17-element consonant choice table and order')
        for decl in ['  undefined4 *puVar3;\n','  char **ppcVar4;\n']:change(decl,'','remove resolved table-copy scaffolding')
    if a==STAFF:
        for addr in ['02757fa8','02758038']:
            data=readonly_string(runtime,int(addr,16));assert data
            change('&UNK_'+addr,'(byte *)'+byte_literal(data),'restore exact native long-name spelling, including case and first/second-byte fast paths')
            changes[-1].update(native_literal=dict(address=addr,bytes=(data+b'\0').hex(),copy_bytes=len(data)+1),literal=data.decode())
    return text,changes


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('letter_effect_evidence')};assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['native_tables']==native_tables(runtime)
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];meta={r['low_pc'][2:]:r for r in csv.DictReader((root/'.recovery/2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['letter_effect_evidence']==detail and entry['static']==(a in PRIVATE)
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-letter-effects' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw,runtime,meta);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,ALIASES);assert literals==entry['recovered_literals'];text,_=rename(text,NAMES);assert function_body(cleaned(text,overlap),entry['name'])==body
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands);assert re.search(r'\b'+site['name']+r'\b',body)
    assert len(entries)==9 and sum(e['name'].startswith('card_') for e in entries.values())==4
    for a,site in [(LETTERS,'2302b05'),(STAFF,'25c8da7')]:
        code=machine_bytes(int(site,16),6);assert code[-4:]==struct.pack('<I',0x273f300)
    assert machine_bytes(0x25c4e45,5)==bytes.fromhex('bec0117202') and machine_bytes(0x25c4e61,7)==bytes.fromhex('b911000000f3a5')
    assert machine_bytes(0x2302d18,5)==bytes.fromhex('b9c4450000') and machine_bytes(0x2302d98,5)==bytes.fromhex('b9d8450000')
    assert machine_bytes(0x2302cb7,2)==bytes.fromhex('89cb')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json';m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']};debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        native_tables(runtime)
        (cache/'letter-effects-clone.tsv').write_text('2302cb0\ttxtfx_name_2022\tint\t__regparm3\tint:player\tint:card\tint:marker\n')
        (cache/'export-request-letter-effects.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],('txtfx_name_2022' if a==CLONE else meta[a]['name']),meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared four card handlers and five private letter/text-effect helpers.');return
    assert 'letter_effects_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),native_tables=native_tables(runtime),**debug_evidence(debug,meta,ADDRESSES),functions={},
        scope='Recover four letter-name/dice/toggle card handlers and five private helpers in original Unsets source files. Restore the event-specialized text-name compiler part, complete 256-byte character-fold tables, 26-byte alphabet bitmap and count loop, seventeen consonant pointer choices, two exact long-name literals and original callback/state/AI paths. Existing interfaces, source bodies and GUI chooser stubs remain unchanged; gameplay integration is downstream.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=('txtfx_name_2022' if a==CLONE else meta[a]['name']),source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES});decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=targets[int(a,16)]['name'];raw=read(cache/'decompiled-letter-effects'/f'{a}.c');text,changes=reconstruct(a,raw,runtime,meta);text,literals=recover_strings(text,runtime,ALIASES);text,_=rename(text,NAMES);body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==[3 if a in PRIVATE else 2]
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,letter_effect_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())));sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    compile_sources(root,cache/'letter-effects-check',sources)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations};rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-letter-effects/export.tsv').open(),delimiter='\t'):rows[row['address']]=row;shutil.copyfile(cache/'decompiled-letter-effects'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=9;m['card_handler_functions']+=4;m['helper_functions']+=5;m['private_functions']+=5;m['private_helpers']+=5
        for e in entries:m['source_files'][e['source']]=m['source_files'].get(e['source'],0)+1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['letter_effects_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'letter-effects-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=4,new_helpers=5)))


if __name__=='__main__':main()
