"""Audit attacker integer/byte storage, Support copies and dispatcher binding."""
import hashlib

from audit_source import function_body
from recover_attacker_core import ADDRESSES, DAT_ADDRESS, CONTROL_SITES
from recover_literals import readonly_string
from recover_test_consumers import rename


def audit_component(component, manifest, sources, machine_bytes, image):
    def require(condition, message):
        if not condition:
            raise ValueError(message)

    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('attacker_core_evidence')}
    require(set(entries)==set(ADDRESSES)|{DAT_ADDRESS} and set(component['functions'])==set(ADDRESSES),
            'Attacker component membership mismatch')
    require(not any(f['static'] for f in entries.values()),'Attacker component public linkage mismatch')
    for address, detail in component['functions'].items():
        require(entries[address]['attacker_core_evidence']==detail,'Attacker function evidence mismatch')
    require(component['storage']==dict(word_bytes=4,count_offsets=[56,124,140],attacker_bytes_offset=132,
            tracked_capacity=64,separate_trigger_uses_count_before_decrement=True),
            'Attacker scalar count/packed byte storage mismatch')
    require(component['support_copy']==dict(declared_pairs=152,array_bytes=1216,copied_bytes=152,
            target_bytes=8,saved_count_bytes=1),'Native Support copy extent mismatch')
    for address in ['268d340','268d6a0']:
        require(any(v['name']=='storage' and v['type']=='int32_t *' for v in component['locals'][address]) and
                any(v['name']=='attackers' and v['type']=='unsigned char *' for v in component['locals'][address]),
                'Attacker native scalar/byte pointer type mismatch')
    require(any(v['name']=='targs' and v['type']=='target_t[152]' and v['location']=='(DW_OP_fbreg: -1236)'
                for v in component['locals']['225a8c0']), 'Native Support saved target-array layout mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES,
            'Attacker/Support instruction membership mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(data))==data,'Attacker/Support native operand mismatch')
    sites={s['address']:s['bytes'] for s in component['control_sites']}
    expected={'268d403':'8bb8a0000000','268d43e':'668b97ae000000','268d497':'8d8384000000',
              '268d571':'8d5038','268d57d':'8d507c','268d582':'8d908c000000',
              '268d609':'83f83f','268d614':'880c02','268d61a':'8906',
              '268d74a':'8a8c1884000000','268d75b':'ff0a','225a948':'b926000000',
              '225a966':'f3a5','225aa7a':'b898000000','225aa98':'f3a5','225a9d3':'74b3',
              '225a99b':'c7442408c4897302'}
    require(all(sites[a]==b for a,b in expected.items()),'Attacker/Support instruction semantics mismatch')
    prompt=component['support_prompt']
    require(prompt['address']=='27389c4' and
            readonly_string(image,int(prompt['address'],16))==bytes.fromhex(prompt['bytes'])==
            b'Select another target creature. (%d of %d)', 'Native Support prompt mismatch')
    require(component['enum_values']['SF_0']==0 and component['flags']['DAT_TRACK']==64 and
            component['flags']['DAT_STORE_IN_INFO_SLOT']==2048 and
            component['flags']['DAT_BATTALION']==16384, 'Attacker native flag semantics mismatch')
    require(set(component['repairs'])=={DAT_ADDRESS},'Attacker dispatcher repair membership mismatch')
    repair=component['repairs'][DAT_ADDRESS]
    entry=entries[DAT_ADDRESS]
    body=function_body(sources[entry['source']],entry['name']).replace('\r\n','\n')
    previous=rename(body,{v:k for k,v in repair['calls'].items()})[0]
    require(hashlib.sha256(body.encode()).hexdigest()==repair['body_sha256']==entry['reconstruction_body_sha256'] and
            hashlib.sha256(previous.encode()).hexdigest()==repair['previous_body_sha256'] and
            repair['evidence']==entry['attacker_core_evidence'], 'Attacker dispatcher body/binding mismatch')
    require(entry['raw_sha256']==repair['previous_record']['raw_sha256'] and len(repair['evidence']['canonical_calls'])==2,
            'Attacker dispatcher raw provenance/call count mismatch')
