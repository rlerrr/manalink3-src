"""Record native calls and callback addresses for a newly promoted dependency batch."""
import argparse
import csv
import hashlib
import json
import re
from pathlib import Path

from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from capstone.x86_const import X86_OP_IMM
from inspect_release import Image
from audit_source import function_body
from promote_recovery import code_only
from recover_test_consumers import read


def main():
    ap=argparse.ArgumentParser(__doc__)
    ap.add_argument('--snapshot',required=True)
    ap.add_argument('--apply',action='store_true')
    args=ap.parse_args()
    root=Path(__file__).resolve().parents[2];cache=root/'.recovery'
    mp=root/'docs/recovery-evidence/recovered-functions.json';ip=root/'docs/recovery-evidence/interface-recovery.json'
    manifest=json.loads(mp.read_text());interfaces=json.loads(ip.read_text())
    assert 'dependency_frontier_2022' not in interfaces
    previous=json.loads((root/args.snapshot/'docs/recovery-evidence/recovered-functions.json').read_text())
    old={f['address'] for f in previous['accepted_functions']}
    entries=[f for f in manifest['accepted_functions'] if f['address'] not in old]
    assert entries
    targets={int(f['address'],16):f for f in manifest['accepted_functions']}
    metadata={r['low_pc'][2:]:r for r in csv.DictReader((cache/'2022-functions.csv').open()) if r['low_pc'] and r['high_pc']}
    image=Image((cache/'2022-ManalinkEh.dll').read_bytes());decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    sources={f['source']:read(root/f['source']) for f in entries}
    proof=dict(functions={},scope='Direct calls/jumps and immediate callback/function-pointer addresses to recovered targets in this newly promoted dependency batch. Private references must stay in their owning source unit. This is not a complete whole-program call-graph audit.')
    call_count=reference_count=0
    for entry in entries:
        address=int(entry['address'],16)
        native=image.pe.get_data(address-0x2000000,int(metadata[entry['address']]['high_pc'],16)-address)
        body=code_only(function_body(sources[entry['source']],entry['name'])).split('{',1)[1]
        calls=[];references=[]
        for instruction in decoder.disasm(native,address):
            for operand in instruction.operands:
                if operand.type!=X86_OP_IMM or operand.imm not in targets:continue
                target=targets[operand.imm]
                assert re.search(r'\b'+re.escape(target['name'])+r'\b',body),(entry['name'],target['name'])
                assert not target['static'] or target['source']==entry['source'],(entry,target)
                site=dict(address=f'{instruction.address:x}',bytes=instruction.bytes.hex(),target=target['address'],name=target['name'])
                (calls if instruction.mnemonic in {'call','jmp'} else references).append(site)
        detail=dict(machine_bytes=len(native),machine_sha256=hashlib.sha256(native).hexdigest(),changes=[],canonical_calls=calls,function_references=references)
        entry['native_dependency_bindings_2022']=detail
        proof['functions'][entry['address']]=detail
        call_count+=len(calls);reference_count+=len(references)
    if args.apply:
        mp.write_text(json.dumps(manifest,indent=2));interfaces['dependency_frontier_2022']=proof;ip.write_text(json.dumps(interfaces,indent=2))
    print(json.dumps(dict(applied=args.apply,functions=len(entries),native_calls=call_count,native_function_references=reference_count)))


if __name__=='__main__':main()
