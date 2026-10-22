"""Verify the temporary-exile wrappers and two transformation card call chains."""
import argparse,csv,json
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
from inspect_release import Image
from audit_source import function_body
from recover_test_consumers import read
from recover_temporary_exile import save_json,sha
from bind_delayed_callback_callers import audit_component as audit_bindings

EXPECTED={'2685c10':'2685a40','2685c50':'2685a40','22128b0':'20d9140','22182e0':'22128b0','2209f00':'20d9fd0'}
TARGETS={'2685a40':'remove_until_upkeep_impl','20d9140':'return_to_bf_transformed_impl',
         '22128b0':'transform_at_end_of_combat','20d9fd0':'exile_and_return_to_bf_transformed'}
MODES={'2685c10':0,'2685c50':1}


def audit_component(component,manifest,sources,machine_bytes):
    audit_bindings(component,manifest,sources,machine_bytes,expected=EXPECTED,targets=TARGETS)
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    if set(component['wrapper_modes'])!=set(MODES):raise ValueError('Temporary-exile wrapper mode membership mismatch')
    for address,mode in MODES.items():
        detail=component['functions'][address];site=component['wrapper_modes'][address]
        if site['value']!=mode:raise ValueError('Temporary-exile wrapper discard mode mismatch')
        instructions={f'{i.address:x}':i for i in decoder.disasm(machine_bytes(int(address,16),detail['machine_bytes']),int(address,16))}
        ins=instructions[site['address']]
        if not (ins.bytes.hex()==site['bytes'] and ins.mnemonic=='mov' and ins.operands[0].type==X86_OP_MEM and
                ins.reg_name(ins.operands[0].mem.base)=='esp' and ins.operands[0].mem.disp==20 and ins.operands[0].size==4 and
                ins.operands[1].type==X86_OP_IMM and ins.operands[1].imm==mode):
            raise ValueError('Temporary-exile native outgoing discard argument mismatch')


def main():
    parser=argparse.ArgumentParser(__doc__);parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery';m=json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    ip=root/'docs/recovery-evidence/interface-recovery.json';interfaces=json.loads(ip.read_text());assert 'temporary_exile_callers_2022' not in interfaces
    entries={f['address']:f for f in m['accepted_functions']};meta={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    sources={entries[a]['source']:read(root/entries[a]['source']) for a in EXPECTED};proof=dict(functions={},wrapper_modes={},
        scope='Five native callee/callback operands connect the two wrappers and two cards to their worker/transform helpers; outgoing argument 5 distinguishes the discard-on-return wrapper.')
    for address,target in EXPECTED.items():
        data=image.pe.get_data(int(address,16)-0x2000000,int(meta[address]['high_pc'],16)-int(address,16));instructions=list(decoder.disasm(data,int(address,16)));sites=[]
        for ins in instructions:
            if any(o.type==X86_OP_IMM and o.imm==int(target,16) for o in ins.operands):
                sites.append(dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),target=target,name=TARGETS[target]))
            if address in MODES and ins.mnemonic=='mov' and len(ins.operands)==2 and ins.operands[0].type==X86_OP_MEM and ins.reg_name(ins.operands[0].mem.base)=='esp' and ins.operands[0].mem.disp==20 and ins.operands[1].type==X86_OP_IMM:
                assert address not in proof['wrapper_modes'];proof['wrapper_modes'][address]=dict(address=f'{ins.address:x}',bytes=ins.bytes.hex(),value=ins.operands[1].imm)
        entry=entries[address];proof['functions'][address]=dict(machine_bytes=len(data),machine_sha256=sha(data),sites=sites,
            body_sha256=sha(function_body(sources[entry['source']],entry['name']).encode()))
    audit_component(proof,m,sources,lambda a,n:image.pe.get_data(a-0x2000000,n))
    if args.apply:interfaces['temporary_exile_callers_2022']=proof;save_json(ip,interfaces)
    print(json.dumps(dict(applied=args.apply,bound_functions=5,native_call_or_callback_sites=5,wrapper_mode_stores=2)))


if __name__=='__main__':main()
