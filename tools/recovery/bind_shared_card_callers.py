"""Pin native direct calls and Kari Zev's callback address for six unlocked cards."""
import argparse
import csv
import hashlib
import json
import re
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from audit_source import function_body
from recover_test_consumers import read

EXPECTED = {'20c4400': ['2073940'], '20c5540': ['2672840'], '2236f60': ['244d530'],
            '223bd60': ['244d530', '268f1b0', '268f900'], '25b1670': ['23e1320'], '20f6010': ['24bd8f0']}
TARGETS = {'2073940': 'exile_at_end_of_combat_2022', '2672840': 'is_creature_type', '244d530': 'converge',
           '268f1b0': 'lose_life_2022', '268f900': 'gain_life_2022', '23e1320': 'vow_2022', '24bd8f0': 'lace_2022'}


def sha(data):
    return hashlib.sha256(data).hexdigest()


def audit_component(component, manifest, sources, machine_bytes):
    def require(ok, message):
        if not ok:
            raise ValueError(message)
    entries = {f['address']: f for f in manifest['accepted_functions']}
    require(set(component['functions']) == set(EXPECTED), 'Shared card caller membership mismatch')
    decoder = Cs(CS_ARCH_X86, CS_MODE_32); decoder.detail = True
    for address, detail in component['functions'].items():
        entry = entries[address]
        body = function_body(sources[entry['source']].replace('\r\n','\n'), entry['name'])
        require(sha(body.encode()) == detail['body_sha256'], 'Shared card caller body mismatch')
        data = machine_bytes(int(address,16), detail['machine_bytes'])
        require(sha(data) == detail['machine_sha256'], 'Shared card caller native hash mismatch')
        instructions = {f'{i.address:x}':i for i in decoder.disasm(data,int(address,16))}
        require([s['target'] for s in detail['sites']] == EXPECTED[address], 'Shared card caller target sequence mismatch')
        for site in detail['sites']:
            i = instructions[site['address']]
            require(i.bytes.hex() == site['bytes'] and any(o.type == X86_OP_IMM and o.imm == int(site['target'],16) for o in i.operands),
                    'Shared card native callee/callback operand mismatch')
            require(site['name'] == TARGETS[site['target']] and re.search(r'\b'+site['name']+r'\b',body), 'Shared card source callee/callback mismatch')
    require(component['functions']['20c4400']['sites'] == [dict(address='20c449c',bytes='c745e440390702',target='2073940',name='exile_at_end_of_combat_2022')],
            'Kari Zev callback must bind the Innistrad helper')


def main():
    parser = argparse.ArgumentParser(__doc__); parser.add_argument('--apply',action='store_true'); args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]; cache = root/'.recovery'
    manifest = json.loads((root/'docs/recovery-evidence/recovered-functions.json').read_text())
    ip = root/'docs/recovery-evidence/interface-recovery.json'; interfaces = json.loads(ip.read_text())
    assert 'shared_card_callers_2022' not in interfaces
    entries = {f['address']:f for f in manifest['accepted_functions']}
    meta = {r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    image = Image((cache/'2022-ManalinkEh.dll').read_bytes()); decoder = Cs(CS_ARCH_X86,CS_MODE_32); decoder.detail = True
    sources = {entries[a]['source']:read(root/entries[a]['source']) for a in EXPECTED}
    proof = dict(functions={},scope='Whole native function hashes and selected helper/callback operands for six newly recovered cards. No earlier callers changed.')
    for address, targets in EXPECTED.items():
        data = image.pe.get_data(int(address,16)-0x2000000,int(meta[address]['high_pc'],16)-int(address,16))
        sites = []
        for i in decoder.disasm(data,int(address,16)):
            for o in i.operands:
                if o.type == X86_OP_IMM and f'{o.imm:x}' in targets:
                    target = f'{o.imm:x}'
                    sites.append(dict(address=f'{i.address:x}',bytes=i.bytes.hex(),target=target,name=TARGETS[target]))
        entry = entries[address]
        proof['functions'][address] = dict(machine_bytes=len(data),machine_sha256=sha(data),sites=sites,
            body_sha256=sha(function_body(sources[entry['source']],entry['name']).encode()))
    audit_component(proof,manifest,sources,lambda a,n:image.pe.get_data(a-0x2000000,n))
    if args.apply:
        interfaces['shared_card_callers_2022'] = proof; ip.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,new_card_call_bindings=6,selected_native_sites=sum(len(d['sites']) for d in proof['functions'].values()))))


if __name__ == '__main__':
    main()
