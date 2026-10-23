"""Recover Legion Warboss and Tidal Wave with verified native token callback types."""
import argparse,csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM,X86_OP_REG
from inspect_release import Image,value
from promote_recovery import strip_comments,UNRESOLVED
from recover_combat_mask_cards import cleaned,ENGINE_RAW,ENGINE_NAME
from recover_phase_prompt_cards import debug_evidence
from recover_test_consumers import read
from recover_spell_copy import compile_sources
from recover_temporary_exile import save_json,sha
from audit_source import function_body

CONFIG={'2392f60':'23761b0','261e2f0':'2619110'}


def audit_component(component,manifest,sources,machine_bytes):
    entries={f['address']:f for f in manifest['accepted_functions']}
    assert set(component['functions'])==set(CONFIG)
    assert component['keyword_defender']==0x800000
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,target in CONFIG.items():
        entry,callback=entries[a],entries[target];detail=component['functions'][a]
        body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        assert entry['callback_token_card_evidence']==detail and sha(body.encode())==entry['reconstruction_body_sha256']
        assert not entry['static'] and callback['static'] and callback['source']==entry['source']
        assert 'token_generation_2022_t *' in callback['prototype']
        assert any(v['name']=='token' and v['type']=='token_generation_t' for v in component['locals'][a])
        native=machine_bytes(int(a,16),detail['machine_bytes']);assert sha(native)==detail['machine_sha256']
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        assert len(detail['callback_references'])==1
        ref=detail['callback_references'][0];ins=instructions[ref['address']]
        assert ins.bytes.hex()==ref['bytes'] and any(o.type==X86_OP_IMM and o.imm==int(target,16) for o in ins.operands)
        assert ref['name']==callback['name'] and f'token.special_code_on_generation = {callback["name"]};' in body
        for call in detail['canonical_calls']:
            ins=instructions[call['address']];assert ins.bytes.hex()==call['bytes'] and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==int(call['target'],16)
            assert re.search(r'\b'+call['name']+r'\s*\(',body)
        if a=='2392f60':
            assert [c['replacement'] for c in detail['changes']]==['KEYWORD_DEFENDER']
            assert 'token.key_plus = KEYWORD_DEFENDER;' in body
            for site in detail['changes'][0]['machine_sites']:
                ins=instructions[site['address']];assert ins.bytes.hex()==site['bytes'] and ins.mnemonic=='mov'
                assert ins.operands[0].type==X86_OP_MEM and ins.operands[1].type==X86_OP_IMM and ins.operands[1].imm==0x800000
        else:
            assert [c['replacement'] for c in detail['changes']]==[ENGINE_NAME]
            load,call=detail['engine_load'],detail['engine_call'];li,ci=instructions[load['address']],instructions[call['address']]
            assert li.bytes.hex()==load['bytes'] and ci.bytes.hex()==call['bytes']
            assert li.mnemonic=='mov' and li.operands[1].type==X86_OP_IMM and li.operands[1].imm==0x434be0
            assert li.operands[0].type==ci.operands[0].type==X86_OP_REG and li.operands[0].reg==ci.operands[0].reg
            assert li.address+li.size==ci.address and ci.mnemonic=='call' and ENGINE_NAME+'(player)' in body


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    m=json.loads(mp.read_text());interfaces=json.loads(ip.read_text());assert 'callback_token_cards_2022' not in interfaces
    assert not set(CONFIG)&{f['address'] for f in m['accepted_functions']}
    entries_by_address={f['address']:f for f in m['accepted_functions']}
    meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(data)
    proof=dict(debug_sha256=sha(data),**debug_evidence(debug,meta,list(CONFIG)),keyword_defender=None,functions={})
    for cu in debug.dwarf().iter_CUs():
        if value(cu.get_top_DIE(),'DW_AT_name')!='cards/mirage.c':continue
        for die in cu.get_top_DIE().iter_children():
            if die.tag=='DW_TAG_enumeration_type':
                for child in die.iter_children():
                    if value(child,'DW_AT_name')=='KEYWORD_DEFENDER':proof['keyword_defender']=value(child,'DW_AT_const_value')
    assert proof['keyword_defender']==0x800000
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    targets={int(f['address'],16):f['name'] for f in m['accepted_functions']}
    overlap=[line.split('\t')[1] for line in (cache/'absolute-interfaces.tsv').read_text().splitlines() if line.split('\t')[2]=='global']
    entries=[];bodies={};sources={}
    for a,target in CONFIG.items():
        row=meta[a];raw=read(cache/'decompiled-abi'/f'{a}.c');text=strip_comments(raw)
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(row['high_pc'],16)-int(a,16));insns=list(decoder.disasm(native,int(a,16)))
        detail=dict(machine_bytes=len(native),machine_sha256=sha(native),changes=[],canonical_calls=[],callback_references=[])
        if a=='2392f60':
            original,replacement='0x800000','KEYWORD_DEFENDER'
            sites=[dict(address=f'{i.address:x}',bytes=i.bytes.hex()) for i in insns if i.mnemonic=='mov' and i.operands[0].type==X86_OP_MEM and i.operands[1].type==X86_OP_IMM and i.operands[1].imm==0x800000]
            assert text.count(original)==1 and len(sites)==1
        else:
            original,replacement=ENGINE_RAW,ENGINE_NAME
            matches=[(load,call) for load,call in zip(insns,insns[1:]) if load.mnemonic=='mov' and load.operands[1].type==X86_OP_IMM and load.operands[1].imm==0x434be0 and call.mnemonic=='call']
            assert text.count(original)==1 and len(matches)==1
            load,call=matches[0];detail['engine_load']=dict(address=f'{load.address:x}',bytes=load.bytes.hex());detail['engine_call']=dict(address=f'{call.address:x}',bytes=call.bytes.hex());sites=[detail['engine_load'],detail['engine_call']]
        detail['changes'].append(dict(original=original,replacement=replacement,occurrences=1,kind='native token callback consumer',machine_sites=sites));text=text.replace(original,replacement)
        body=cleaned(text,overlap);assert not UNRESOLVED.search(body),a
        for i in insns:
            for o in i.operands:
                if o.type!=X86_OP_IMM or o.imm not in targets:continue
                name=targets[o.imm];assert re.search(r'\b'+name+r'\b',body),(a,name)
                site=dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=f'{o.imm:x}',name=name)
                (detail['canonical_calls'] if i.mnemonic in {'call','jmp'} else detail['callback_references']).append(site)
        proof['functions'][a]=detail;bodies[a]=body
        entry=dict(address=a,name=row['name'],source=row['source'],line=row['line'],static=False,prototype=body[:body.index('{')].strip()+';',
            raw_sha256=sha(raw.encode()),recovered_literals=[],callback_token_card_evidence=detail,
            reconstruction='Native token constructors/generator and typed callback pointer, plus exact defender-mask or verified EXE query binding.',reconstruction_body_sha256=sha(function_body(body,row['name']).encode()))
        entries.append(entry)
        retained=[f['prototype'] for f in m['accepted_functions'] if f['source']==row['source'] and f['static'] and re.search(r'\b'+f['name']+r'\b',body)]
        compile_sources(root,cache/'callback-token-cards-check',{row['source']:'#include "manalink.h"\n'+'\n'.join(retained)+'\n'+body})
        sources[row['source']]=read(root/row['source'])+f'\n\n#ifndef SHANDALAR\n/* ManalinkEh 2022: 0x{a}; original {row["source"]}:{row["line"]}. */\n'+body+'\n#endif\n'
    audit_component(proof,{**m,'accepted_functions':m['accepted_functions']+entries},sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if args.apply:
        for source,text in sources.items():(root/source).write_bytes(text.replace('\n','\r\n').encode('latin1'))
        m['accepted_functions']+=entries;m['accepted']+=2;m['card_handler_functions']+=2
        for entry in entries:m['source_files'][entry['source']]+=1
        save_json(mp,m);interfaces['callback_token_cards_2022']=proof;save_json(ip,interfaces)
    save_json(cache/'callback-token-cards-plan.json',proof);print(json.dumps(dict(applied=args.apply,new_card_functions=2,native_callback_operands=2)))


if __name__=='__main__':main()
