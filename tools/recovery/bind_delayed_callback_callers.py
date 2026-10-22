"""Record direct calls and callback operands for the delayed-effect recovery batch."""
import argparse,csv,hashlib,json,re
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from audit_source import function_body
from recover_test_consumers import read

EXPECTED={'23761b0':'2377110','218eda0':'218f5b0','212ad10':'212c130','261a7d0':'21b8dc0',
          '2390c80':'23eb810','233b5c0':'2335b80','2192290':'218eda0','21357f0':'212ad10','2403af0':'24bc200'}
TARGETS={'2377110':'effect_sacrifice_at_next_eot','218f5b0':'remove_eot_unless_cb','212c130':'effect_destroy_at_next_eot',
         '21b8dc0':'last_chance_legacy_2022','23eb810':'cantrip_legacy','2335b80':'sac_at_eot',
         '218eda0':'remove_token_at_end_of_next_turn_unless_cb','212ad10':'destroy_token_at_next_eot','24bc200':'effect_nettling_imp_2022'}


def sha(data):return hashlib.sha256(data).hexdigest()


def audit_component(component,manifest,sources,machine_bytes,expected=EXPECTED,targets=TARGETS):
    def require(ok,message):
        if not ok:raise ValueError(message)
    require(set(component['functions'])==set(expected),'Native callback/caller membership mismatch')
    entries={f['address']:f for f in manifest['accepted_functions']};decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    for address,detail in component['functions'].items():
        entry=entries[address];body=function_body(sources[entry['source']].replace('\r\n','\n'),entry['name'])
        require(sha(body.encode())==detail['body_sha256'],'Delayed effect caller body mismatch')
        data=machine_bytes(int(address,16),detail['machine_bytes']);require(sha(data)==detail['machine_sha256'],'Delayed effect caller native hash mismatch')
        require(len(detail['sites'])==1,'Delayed effect caller must have one selected helper reference')
        instructions={f'{i.address:x}':i for i in decoder.disasm(data,int(address,16))}
        for site in detail['sites']:
            ins=instructions[site['address']];target=expected[address]
            require(site['target']==target and site['name']==targets[target] and ins.bytes.hex()==site['bytes'] and
                any(o.type==X86_OP_IMM and o.imm==int(target,16) for o in ins.operands),'Delayed effect native callback/call operand mismatch')
            require(re.search(r'\b'+targets[target]+r'\b',body),'Native source callback/callee mismatch')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';m=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    ip=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(ip.read_text());assert 'delayed_callback_callers_2022' not in interfaces
    entries={f['address']:f for f in m['accepted_functions']};meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    sources={entries[a]['source']:read(root/entries[a]['source']) for a in EXPECTED};proof=dict(functions={},scope='Nine native helper operands for three constructors and six card handlers. Earlier callers are unchanged.')
    for address,target in EXPECTED.items():
        data=image.pe.get_data(int(address,16)-0x2000000,int(meta[address]['high_pc'],16)-int(address,16));sites=[]
        for ins in decoder.disasm(data,int(address,16)):
            if any(o.type==X86_OP_IMM and o.imm==int(target,16) for o in ins.operands):
                sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=target,name=TARGETS[target]))
        entry=entries[address];proof['functions'][address]=dict(machine_bytes=len(data),machine_sha256=sha(data),sites=sites,
            body_sha256=sha(function_body(sources[entry['source']],entry['name']).encode()))
    audit_component(proof,m,sources,lambda a,n:image.pe.get_data(a-0x2000000,n))
    if args.apply:
        interfaces['delayed_callback_callers_2022']=proof
        temporary=ip.with_suffix('.tmp')
        temporary.write_text(json.dumps(interfaces,indent=2))
        assert json.loads(temporary.read_text())==interfaces
        temporary.replace(ip)
    print(json.dumps(dict(applied=args.apply,bound_functions=len(proof['functions']),native_sites=sum(len(d['sites']) for d in proof['functions'].values()))))


if __name__=='__main__':main()
