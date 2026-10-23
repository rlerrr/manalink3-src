"""Recover reanimation consumers with exact prompt copies and Calix's meld pair."""
import argparse,csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from promote_recovery import strip_comments,UNRESOLVED
from recover_combat_mask_cards import cleaned
from recover_phase_prompt_cards import debug_evidence
from recover_test_consumers import read
from recover_spell_copy import compile_sources
from recover_literals import recover_strings,byte_literal
from recover_temporary_exile import save_json,sha
from audit_source import function_body,target_validation_input

PROMPTS={
 '2561cc0':dict(pointer=0x2753ed4,bytes=24,zero=0,variable='buf',ranges=[('2562153','2562169'),('256216e','2562184')]),
 '20328f0':dict(pointer=0x2724378,bytes=50,zero=50,variable='msg',ranges=[('203296f','20329bb')]),
 '20d3ad0':dict(pointer=0x272a0c0,bytes=43,zero=57,variable='msg',ranges=[('20d3ad0','20d3b38')]),
 '25c2250':dict(pointer=0x2757af8,bytes=43,zero=57,variable='msg',ranges=[('25c2250','25c22b8')])}
ADDRESSES=[*PROMPTS,'235b4c0']
CALIX_SITES={'235bac4':'8d951cffffff','235baca':'8d8d20ffffff','235bad0':'894c2408','235bad4':'89542404','235badb':'e8a07b3500','235bae0':'8b851cffffff','235ba02':'8b851cffffff'}


def transform(a,text,runtime):
    changes=[];prompt=None
    def replace(old,new,kind,sites=None,ranges=None):
        nonlocal text
        count=text.count(old);assert count,(a,old)
        change=dict(original=old,replacement=new,occurrences=count,kind=kind,machine_sites=sites or [])
        if ranges:change['machine_ranges']=ranges
        changes.append(change);text=text.replace(old,new)
    if a in PROMPTS:
        p=PROMPTS[a];data=runtime.pe.get_data(p['pointer']-0x2000000,p['bytes']);assert data.endswith(b'\0')
        ranges=[dict(start=s,end=e,bytes=int(e,16)-int(s,16),sha256=sha(runtime.pe.get_data(int(s,16)-0x2000000,int(e,16)-int(s,16)))) for s,e in p['ranges']]
        prompt=dict(address=f'{p["pointer"]:x}',bytes=data.hex(),copy_bytes=p['bytes'],zero_bytes=p['zero'],destination_bytes=100,ranges=ranges)
        replacement=f'memcpy({p["variable"]}, '+byte_literal(data[:-1])+f', {p["bytes"]});\n'
        if p['zero']:replacement+=f'memset({p["variable"]} + {p["bytes"]}, 0, {p["zero"]});\n'
        if a=='2561cc0':
            matches=list(re.finditer(r'puVar7 = \(undefined4 \*\)&UNK_02753ed4;.*?pcVar8 = pcVar8 \+ \(uint\)bVar9 \* -8 \+ 4;\s*\}',text,re.S));assert len(matches)==2 and matches[0][0]==matches[1][0]
            replace(matches[0][0],replacement,'exact 24-byte prompt copy',ranges=ranges)
            unused=['undefined4 *puVar7;','char *pcVar8;','byte bVar9;','bVar9 = 0;']
        else:
            pointer=f'&UNK_{p["pointer"]:08x}';start=text.rfind('\n',0,text.index(pointer))+1
            end=text.index('new_default_test_definition',start);old=text[start:end]
            replace(old,replacement,'exact 100-byte initialized prompt',ranges=ranges)
            unused=(['undefined4 *puVar4;','char *pcVar5;'] if a=='25c2250' else ['undefined4 *puVar3;','char *pcVar4;'])
            if a=='20328f0':unused+=['byte bVar5;','bVar5 = 0;']
        for item in unused:replace(item,'','remove dead compiler string-copy temporaries')
    else:
        sites=[dict(address=a,bytes=b) for a,b in CALIX_SITES.items()]
        replace('test_definition_t test;','test_definition_t test;\n  int csvids[2];','separate nonoverlapping lifetimes at native fbreg -236',sites)
        replace('unmeld(uVar5,(int *)&test,(int *)(test.message + 4));','unmeld(uVar5, &csvids[0], &csvids[1]);','native four-byte meld output stride',sites)
        # Both native loads are EBP-0xe4. Preserve that repeated first-element use.
        replace('test.message._0_4_','csvids[0]','native repeated first meld element, including second iteration',sites)
    return text,changes,prompt


def audit_component(component,manifest,sources,machine_bytes,root):
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('reanimation_card_evidence')}
    assert set(entries)==set(ADDRESSES)==set(component['functions'])
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,entry in entries.items():
        detail=component['functions'][a];body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        assert entry['reanimation_card_evidence']==detail and sha(body.encode())==entry['reconstruction_body_sha256']
        assert not entry['static'] and 'reanimate_' in body
        raw=read(root/'.recovery/decompiled-abi'/f'{a}.c');assert sha(raw.encode())==entry['raw_sha256']
        transformed=target_validation_input(strip_comments(raw),detail,a,machine_bytes)
        native=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(native)==detail['machine_sha256']
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        for call in detail['canonical_calls']+detail['callback_references']:
            ins=instructions[call['address']];assert ins.bytes.hex()==call['bytes']
            assert any(o.type==X86_OP_IMM and o.imm==int(call['target'],16) for o in ins.operands)
            assert re.search(r'\b'+call['name']+r'\b',body),(a,call)
        if a in PROMPTS:
            p=detail['prompt'];config=PROMPTS[a];data=bytes.fromhex(p['bytes'])
            assert machine_bytes(int(p['address'],16),len(data))==data and data.endswith(b'\0')
            assert (p['copy_bytes'],p['zero_bytes'],p['destination_bytes'])==(config['bytes'],config['zero'],100)
            assert 'memcpy('+config['variable'] in body and 'UNK_' not in transformed
            assert any(v['name']==config['variable'] and v['type']=='char[100]' for v in component['locals'][a])
            for block in p['ranges']:
                start,end=int(block['start'],16),int(block['end'],16);code=machine_bytes(start,end-start)
                assert sha(code)==block['sha256'] and len(code)==block['bytes']
                ins=list(decoder.disasm(code,start));assert any(i.mnemonic=='rep movsd' for i in ins)
                assert any(o.type==X86_OP_IMM and o.imm==config['pointer'] for i in ins for o in i.operands)
                if config['zero']:assert any(i.mnemonic=='rep stosd' for i in ins)
        else:
            for at,data in CALIX_SITES.items():assert machine_bytes(int(at,16),len(bytes.fromhex(data))).hex()==data
            assert any(v['name']=='csvids' and v['type']=='int[2]' and v['location']=='(DW_OP_fbreg: -236)' for v in component['locals'][a])
            assert any(v['name']=='test' and v['type']=='test_definition_t' and v['location']=='(DW_OP_fbreg: -236)' for v in component['locals'][a])
            assert 'int csvids[2];' in body and body.count('get_internal_card_id_from_csv_id(csvids[0])')==2
            ref=next(f for f in manifest['accepted_functions'] if f['address']=='2358e10')
            assert ref['static'] and ref['source']==entry['source']


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'reanimation_cards_2022' not in interfaces and 'reanimation_2022' in interfaces
    assert not set(ADDRESSES)&{f['address'] for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();proof=dict(debug_sha256=sha(data),**debug_evidence(Image(data),meta,ADDRESSES),functions={})
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    targets={int(f['address'],16):f for f in m['accepted_functions']}
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    entries=[];sources={}
    for a in ADDRESSES:
        row=meta[a];raw=read(cache/'decompiled-abi'/f'{a}.c');text,changes,prompt=transform(a,strip_comments(raw),runtime)
        text,literals=recover_strings(text,runtime,{});body=cleaned(text,overlap)
        assert not UNRESOLVED.search(body) and not re.search(r'stack0x|\._\d|_conflict',body),a
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(row['high_pc'],16)-int(a,16))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=changes,canonical_calls=[],callback_references=[])
        if prompt:detail['prompt']=prompt
        for ins in decoder.disasm(native,int(a,16)):
            for o in ins.operands:
                if o.type!=X86_OP_IMM or o.imm not in targets:continue
                target=targets[o.imm];name=target['name'];assert re.search(r'\b'+name+r'\b',body),(a,name)
                assert not target['static'] or target['source']==row['source']
                site=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=f'{o.imm:x}',name=name)
                (detail['canonical_calls'] if ins.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail
        entry=dict(address=a,name=row['name'],source=row['source'],line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',raw_sha256=sha(raw.encode()),recovered_literals=literals,reanimation_card_evidence=detail,
            reconstruction='Verified native prompt copy/fill or Calix meld-array lifetimes, preserving native control flow and repeated first meld element; bind all recovered callees and private callback operands.',reconstruction_body_sha256=sha(function_body(body,row['name']).encode()))
        entries.append(entry)
        retained=[f['prototype'] for f in m['accepted_functions'] if f['source']==row['source'] and f['static'] and re.search(r'\b'+f['name']+r'\b',body)]
        compile_sources(root,cache/'reanimation-cards-check',{row['source']:'#include "manalink.h"\n'+'\n'.join(retained)+'\n'+body})
        source=sources.get(row['source'],read(root/row['source']))
        sources[row['source']]=source+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n),root)
    if args.apply:
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=5;m['card_handler_functions']+=5
        for entry in entries:m['source_files'][entry['source']]+=1
        save_json(mp,m);interfaces['reanimation_cards_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'reanimation-cards-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_card_functions=5,initialized_prompt_bytes=348)))


if __name__=='__main__':main()
