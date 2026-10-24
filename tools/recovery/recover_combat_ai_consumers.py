"""Recover the thirty cached consumers of three native combat-AI accumulators."""
import argparse,csv,json,re,shutil
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image
from promote_recovery import strip_comments,UNRESOLVED
from recover_test_consumers import read
from recover_combat_mask_cards import cleaned
from recover_literals import recover_strings
from recover_spell_copy import compile_sources
from recover_phase_prompt_cards import debug_evidence
from recover_leaves_test_native import native_layouts
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

ADDRESSES='22d8f10 2643f00 23fa6e0 2409280 249e010 217f970 238a780 238ed30 209ba10 21c3d10 21c48d0 21c4dc0 25fa4f0 25fa8d0 26021a0 2603010 2603370 215a450 215acc0 215adf0 215afa0 215dda0 21095e0 2056030 20227c0 2022fb0 2023bf0 22eeb00 22f68b0 22f8980'.split()
PRIVATE={'25fa4f0','25fa8d0'}
GLOBALS={0x739b50:'check_abilities_power_2022',0x7a371c:'check_abilities_toughness_2022',0x78fef0:'check_abilities_keywords_2022'}
HEADER='/* October 2022 EXE-owned combat-AI accumulators (32-bit lvalues). */\n#ifndef SHANDALAR\n'+''.join(f'#define {name} EXE_DWORD(0x{a:x})\n' for a,name in GLOBALS.items())+'#endif\n\n'
RANGES={'2409280':{'24093da':'8d43ce','24093dd':'83f802','24093e0':'76b5'},
        '22f8980':{'22f89e7':'8d43ce','22f89ea':'83f802','22f89ed':'7629'}}
SETUP_RANGES=[(0x26ac14f,0x26ac170),(0x26ac170,0x26ac191),(0x26ac191,0x26ac1b2),
              (0x26ac1b2,0x26ac1d5),(0x26ac1de,0x26ac203)]


def initializer_proof(runtime,magic):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    result=[]
    for start,end in SETUP_RANGES:
        code=runtime.pe.get_data(start-0x2000000,end-start);insns=list(decoder.disasm(code,start))
        assert sum(i.size for i in insns)==len(code)
        result.append(dict(address=f'{start:x}',bytes=code.hex()))
    for (start,end),a,event in zip(SETUP_RANGES,list(GLOBALS),[50,51,52]):
        insns=list(decoder.disasm(runtime.pe.get_data(start-0x2000000,end-start),start))
        assert any(i.mnemonic=='mov' and i.operands[0].type==X86_OP_MEM and i.operands[0].mem.disp==8 and i.operands[1].type==X86_OP_IMM and i.operands[1].imm==event for i in insns)
        assert insns[-2].mnemonic=='call' and insns[-2].operands[0].imm==0x26cc810
        assert insns[-1].mnemonic=='mov' and insns[-1].operands[0].mem.disp==a and insns[-1].operands[0].size==4
    insns=list(decoder.disasm(bytes.fromhex(result[3]['bytes']),int(result[3]['address'],16)))
    assert insns[0].mnemonic=='mov' and insns[0].operands[1].imm==140
    assert insns[-1].mnemonic=='call' and insns[-1].operands[0].imm==0x4359b0
    globals_=[]
    for a,name in GLOBALS.items():
        section=next(s for s in magic.pe.sections if s.VirtualAddress<=a-magic.pe.OPTIONAL_HEADER.ImageBase<s.VirtualAddress+s.Misc_VirtualSize)
        assert section.Characteristics & 0x80000000
        globals_.append(dict(name=name,address=f'{a:x}',bytes=4,owner='Magic.exe',section=section.Name.rstrip(b'\0').decode(),writable=True))
    return dict(executable_sha256=sha(magic.data),ranges=result,globals=globals_,
        scope='Native setup seeds three 32-bit EXE lvalues from EVENT_POWER/TOUGHNESS/ABILITIES, dispatches EVENT_CHECK_ABILITIES and adds power/toughness or ORs keywords into the per-card combat record. The initializer itself is supporting machine evidence, not counted as recovered.')


def reconstruct(a,raw):
    text=strip_comments(raw);changes=[]
    if a in RANGES:
        old='2 < event - EVENT_POWER';new='2U < (unsigned int)event - (unsigned int)EVENT_POWER';assert text.count(old)==1
        changes.append(dict(original=old,replacement=new,occurrences=1,kind='preserve native unsigned three-event Aura range',machine_sites=[dict(address=at,bytes=b) for at,b in RANGES[a].items()]));text=text.replace(old,new)
    return text,changes


def references(code,a):
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;sites=[]
    for ins in decoder.disasm(code,int(a,16)):
        for op in ins.operands:
            if op.type==X86_OP_MEM and op.mem.disp in GLOBALS:
                assert op.size==4 and not op.mem.base and not op.mem.index
                sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.mem.disp:x}',name=GLOBALS[op.mem.disp],operand_bytes=op.size))
    assert sites,a
    return sites


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('combat_ai_consumer_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    runtime=Image((root/'.recovery/2022-ManalinkEh.dll').read_bytes());magic=Image((root/'.recovery/2022-Magic.exe').read_bytes())
    assert component['runtime_sha256']==sha(runtime.data)=='52793f60124a3b83d8b2728d145e918101a395b4e8d1c0af69cddcfc87ef10e5'
    assert component['initializer']==initializer_proof(runtime,magic)
    assert component['initializer']['executable_sha256']=='0170f02b1c381a20a97ca0c12e99c8d338453ce4edbfc99148e726d565bee0de'
    assert HEADER in read(root/'manalink.h') or component.get('planning')
    for layout in component['unit_layouts'].values():assert layout['test_definition_t']['bytes']==216 and layout['target_definition_t']['bytes']==84
    overlap=[s.split('\t')[1] for s in (root/'.recovery/absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global']
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];assert entry['combat_ai_consumer_evidence']==detail and entry['static']==(a in PRIVATE)
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name']);assert sha(body.encode())==entry['reconstruction_body_sha256']
        raw=read(root/'.recovery'/('decompiled-combat-ai-consumers' if component.get('planning') else 'decompiled-abi')/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        text,changes=reconstruct(a,raw);assert changes==detail['changes'] and target_validation_input(strip_comments(raw),detail,a,machine_bytes)==text
        text,literals=recover_strings(text,runtime,{});assert literals==entry['recovered_literals'] and function_body(cleaned(text,overlap),entry['name'])==body
        assert len(component['parameters'][a])==3
        code=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(code)==detail['machine_sha256'] and detail['native_coff_storage']==(3 if a in PRIVATE else 2)
        assert detail['global_references']==references(code,a)
        assert {n for n in GLOBALS.values() if re.search(r'\b'+n+r'\b',body)}=={s['name'] for s in detail['global_references']}
        insns={f'{i.address:x}':i for i in decoder.disasm(code,int(a,16))}
        for site in detail['canonical_calls']+detail['callback_references']:
            ins=insns[site['address']];assert ins.bytes.hex()==site['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(site['target'],16) for o in ins.operands)
            assert re.search(r'\b'+site['name']+r'\b',body)
        for at,b in RANGES.get(a,{}).items():assert machine_bytes(int(at,16),len(bytes.fromhex(b))).hex()==b
    assert len(entries)==30 and sum(e['name'].startswith('card_') for e in entries.values())==28


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');parser.add_argument('--prepare',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug=Image(Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes());runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());magic=Image((cache/'2022-Magic.exe').read_bytes());get=lambda a,n:runtime.pe.get_data(a-0x2000000,n)
    if args.prepare:
        initializer_proof(runtime,magic)
        (cache/'export-request-combat-ai-consumers.tsv').write_text(''.join('\t'.join([a,meta[a]['source'],meta[a]['name'],meta[a]['high_pc'][2:]])+'\n' for a in ADDRESSES));print('Prepared thirty combat-AI consumers; apply ApplyRecoveryCombatGlobals.java before exporting.');return
    assert 'combat_ai_consumers_native_2022' not in interfaces and not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    proof=dict(runtime_sha256=sha(runtime.data),debug_sha256=sha(debug.data),initializer=initializer_proof(runtime,magic),**debug_evidence(debug,meta,ADDRESSES),
        unit_layouts=native_layouts(debug,{'cards/ice_age.c','cards/tempest.c','cards/visions.c','cards/prophecy.c'}),functions={},
        scope='Recover all thirty cached consumers of the three EXE-owned EVENT_CHECK_ABILITIES accumulators together. Bind exact 32-bit runtime lvalues from native initializer events/stores and every consumer operand; retain native phase, player, activation, trigger and failure behavior. Use native target/test/pump interfaces, retain unsigned Aura event ranges and private callback linkage. Existing source and recovery records remain unchanged.')
    targets={int(f['address'],16):f for f in m['accepted_functions']};targets.update({int(a,16):dict(name=meta[a]['name'],source=meta[a]['source'],static=a in PRIVATE) for a in ADDRESSES})
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;overlap=[s.split('\t')[1] for s in (cache/'absolute-interfaces.tsv').read_text().splitlines() if s.split('\t')[2]=='global'];sources={};entries=[]
    for a in ADDRESSES:
        row=meta[a];source=row['source'];name=row['name'];raw=read(cache/'decompiled-combat-ai-consumers'/f'{a}.c');text,changes=reconstruct(a,raw);text,literals=recover_strings(text,runtime,{});body=cleaned(text,overlap)
        if a in PRIVATE:body='static '+body
        assert not UNRESOLVED.search(strip_comments(body)) and not re.search(r'stack0x|\._\d|_conflict|::',body),a
        storage=[s for n,v,sec,k,s in debug.functions() if sec>0 and debug.pe.OPTIONAL_HEADER.ImageBase+debug.pe.sections[sec-1].VirtualAddress+v==int(a,16)];assert storage==([3] if a in PRIVATE else [2])
        code=get(int(a,16),int(row['high_pc'],16)-int(a,16));detail=dict(machine_bytes=len(code),machine_sha256=sha(code),changes=changes,canonical_calls=[],callback_references=[],global_references=references(code,a),native_coff_storage=storage[0])
        for ins in decoder.disasm(code,int(a,16)):
            for op in ins.operands:
                if op.type!=X86_OP_IMM or op.imm not in targets:continue
                target=targets[op.imm];assert re.search(r'\b'+target['name']+r'\b',body),(a,target['name']);assert not target['static'] or target['source']==source
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{op.imm:x}',name=target['name']);(detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;entries.append(dict(address=a,name=name,source=source,line=row['line'],static=a in PRIVATE,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,combat_ai_consumer_evidence=detail,reconstruction=proof['scope'],reconstruction_body_sha256=sha(function_body(body,name).encode())))
        sources[source]=sources.get(source,read(root/source))+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {source}:{row["line"]}. */\n'+body+'\n#endif\n'
    header=read(root/'manalink.h');at=header.rfind('#ifdef __cplusplus');header=header[:at]+HEADER+header[at:]
    # Two retained Invasion bodies pass an unused extra snprintf argument.
    # Suppress only that diagnostic in the old prefix of the scratch file;
    # every new body still receives the complete strict format check.
    checking=dict(sources);source='cards/invasion.c';prefix=read(root/source)
    assert prefix.count('snprintf(msg, 100, "%s", __format_1, __n_1);')==2
    checking[source]='#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored "-Wformat-extra-args"\n'+prefix+'\n#pragma GCC diagnostic pop\n'+sources[source][len(prefix):]
    compile_sources(root,cache/'combat-ai-consumers-check',checking,header)
    proof['planning']=True;audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,get,root);del proof['planning']
    if args.apply:
        citations=m['accepted_functions']+m['source_fragments']+[s for f in m['accepted_functions'] for s in f.get('supporting_exports',[])];assert not set(ADDRESSES)&{f['address'] for f in citations}
        rows={r['address']:r for r in csv.DictReader((cache/'decompiled-abi/export.tsv').open(),delimiter='\t')};before=len(rows)
        for row in csv.DictReader((cache/'decompiled-combat-ai-consumers/export.tsv').open(),delimiter='\t'):
            rows[row['address']]=row;shutil.copyfile(cache/'decompiled-combat-ai-consumers'/f'{row["address"]}.c',cache/'decompiled-abi'/f'{row["address"]}.c')
        proof['new_canonical_raw_entries']=len(rows)-before
        with (cache/'decompiled-abi/export.tsv').open('w',newline='') as stream:
            writer=csv.DictWriter(stream,fieldnames=['address','source','name','status','prototype'],delimiter='\t');writer.writeheader();writer.writerows(rows.values())
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        (root/'manalink.h').write_bytes(header.replace('\n','\r\n').encode('latin1'));m['accepted_functions']+=entries;m['accepted']+=30;m['card_handler_functions']+=28;m['helper_functions']+=2;m['private_functions']+=2;m['private_helpers']+=2
        for e in entries:m['source_files'][e['source']]+=1
        m['raw_export_entries']=len(rows);m['exported_functions']=len({r['name'] for r in rows.values()});save_json(mp,m);interfaces['combat_ai_consumers_native_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'combat-ai-consumers-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_cards=28,new_helpers=2,private_helpers=2)))


if __name__=='__main__':main()
