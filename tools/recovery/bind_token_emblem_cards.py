"""Verify token IDs returned to Ajani and Mu Yanling, and their emblem writes."""
import argparse,csv,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image
from recover_test_consumers import read
from recover_temporary_exile import save_json,sha
from recover_phase_prompt_cards import debug_evidence
from audit_source import function_body

CONFIG={
 '2206a90':dict(csvid=0x49bb,argument='2206b8d',call='2206b9c',result='2206ba4',instance='2206ba8',
    writes={'2206bc6':('898780000000',128,4)},locals={'td1':('target_definition_t',-320),'test_resurrect':('test_definition_t',-236)}),
 '23982c0':dict(csvid=0x4f99,argument='239838a',call='239839f',result='23983a7',instance='23983ab',
    writes={'23983e8':('89417c',124,4),'23983f1':('888116010000',278,1)},locals={'td':('target_definition_t',-104),'token':('token_generation_t',-180)})}


def audit_component(component,manifest,sources,machine_bytes):
    def require(ok,message):
        if not ok:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions']}
    require(set(component['functions'])==set(CONFIG),'Token emblem card membership mismatch')
    require(entries['26666b0']['name']=='generate_token_by_id_2022' and entries['26666b0']['prototype'].startswith('int '),'Native token result helper mismatch')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for a,cfg in CONFIG.items():
        entry=entries[a];detail=component['functions'][a];body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        require(not entry['static'] and entry['token_frontend_interface_2022']['calls']=={'generate_token_by_id':'generate_token_by_id_2022'},'Token emblem frontend binding mismatch')
        require(sha(body.encode())==detail['body_sha256'],'Token emblem source body mismatch')
        native=machine_bytes(int(a,16),detail['machine_bytes']);require(sha(native)==detail['machine_sha256'],'Token emblem native body mismatch')
        instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        require(set(detail['sites'])=={cfg[k] for k in ['argument','call','result','instance']}|set(cfg['writes']),'Token emblem instruction membership mismatch')
        for site,data in detail['sites'].items():require(instructions[site].bytes.hex()==data,'Token emblem instruction bytes mismatch')
        arg=instructions[cfg['argument']]
        require(arg.mnemonic=='mov' and arg.operands[0].type==X86_OP_MEM and decoder.reg_name(arg.operands[0].mem.base)=='esp' and
            arg.operands[0].mem.disp==8 and arg.operands[0].size==4 and arg.operands[1].type==X86_OP_IMM and arg.operands[1].imm==cfg['csvid'],
            'Token emblem native card-ID argument mismatch')
        for key,target in [('call',0x26666b0),('instance',0x26b2ab0)]:
            ins=instructions[cfg[key]];require(ins.mnemonic=='call' and ins.operands[0].type==X86_OP_IMM and ins.operands[0].imm==target,'Token emblem native callee mismatch')
        result=instructions[cfg['result']]
        require(result.mnemonic=='mov' and result.operands[0].type==X86_OP_MEM and decoder.reg_name(result.operands[0].mem.base)=='esp' and
            result.operands[0].mem.disp==4 and result.operands[0].size==4 and decoder.reg_name(result.operands[1].reg)=='eax','Token emblem returned ID transfer mismatch')
        for site,(data,offset,width) in cfg['writes'].items():
            ins=instructions[site];require(ins.mnemonic=='mov' and ins.bytes.hex()==data and ins.operands[0].type==X86_OP_MEM and
                ins.operands[0].mem.disp==offset and ins.operands[0].size==width,'Token emblem native field store mismatch')
        for name,(typename,offset) in cfg['locals'].items():
            require(any(v['name']==name and v['type']==typename and v['location']==f'(DW_OP_fbreg: {offset})' for v in component['locals'][a]),'Token emblem aggregate local mismatch')
        require(re.search(r'\biVar2 = generate_token_by_id_2022\(player, card, '+hex(cfg['csvid'])+r'\);',body) and
            re.search(r'\bget_card_instance\(player, iVar2\)',body),'Token emblem source returned-ID use mismatch')
        expected=['instance->targets[1].card = iVar2;'] if a=='2206a90' else ['pcVar3->targets[1].player = iVar2;','pcVar3->hack_mode[2] = pcVar1->hack_mode[2];']
        require(all(text in body for text in expected),'Token emblem source payload stores missing')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    m=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text());ip=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(ip.read_text())
    assert 'token_emblem_cards_2022' not in interfaces
    entries={f['address']:f for f in m['accepted_functions']};meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    debug_data=Path('G:/Manalink/ManalinkEh_10_22_Hotfix.dbg').read_bytes();debug=Image(debug_data)
    proof=dict(debug_sha256=sha(debug_data),**debug_evidence(debug,meta,list(CONFIG)),functions={},
        scope='Two native integer token results pass unchanged from EAX to get_card_instance card argument 1. Preserve exact emblem IDs, color/subtype words and the one-byte hacked-Island mapping copy. New native helpers are already recovered; this component adds no implementations.')
    runtime=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    sources={entries[a]['source']:read(root/entries[a]['source']) for a in CONFIG}
    for a,cfg in CONFIG.items():
        native=runtime.pe.get_data(int(a,16)-0x2000000,int(meta[a]['high_pc'],16)-int(a,16));instructions={f'{i.address:x}':i for i in decoder.disasm(native,int(a,16))}
        sites={cfg[k]:instructions[cfg[k]].bytes.hex() for k in ['argument','call','result','instance']}
        sites.update({s:instructions[s].bytes.hex() for s in cfg['writes']});entry=entries[a]
        proof['functions'][a]=dict(machine_bytes=len(native),machine_sha256=sha(native),sites=sites,body_sha256=sha(function_body(sources[entry['source']],entry['name']).encode()))
    audit_component(proof,m,sources,lambda a,n:runtime.pe.get_data(a-0x2000000,n))
    if args.apply:interfaces['token_emblem_cards_2022']=proof;save_json(ip,interfaces)
    print(json.dumps(dict(applied=args.apply,bound_functions=2,returned_token_id_transfers=2,emblem_field_stores=3)))


if __name__=='__main__':main()
