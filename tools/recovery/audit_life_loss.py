"""Audit the native life-loss saved-ID matrix and coordinated gain/loss binding."""
import hashlib
from audit_source import function_body
from recover_test_consumers import rename
from recover_life_loss import ADDRESSES,CONTROL_SITES


def audit_component(component,manifest,sources,machine_bytes,interfaces):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('life_loss_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']) and not any(f['static'] for f in entries.values()),
            'Life-loss engine membership/public linkage mismatch')
    entry=entries['268f1b0'];detail=component['functions']['268f1b0']
    require(entry['life_loss_evidence']==detail,'Life-loss engine evidence mismatch')
    parameters=component['parameters']['268f1b0']
    require([p['name'] for p in parameters]==['player','amount'] and len(parameters)==2,'Life-loss engine parameter mismatch')
    marked=next(v for v in component['locals']['268f1b0'] if v['name']=='marked')
    require((marked['type'],marked['location'],marked['bounds'])==('int[2][151]','(DW_OP_fbreg: -1228)',[1,150]),
            'Life-loss saved callback ID array mismatch')
    require(component['marked_layout']==dict(rows=2,columns=151,element_bytes=4,bytes=1208,zero_words=302,frame_offset=-1228),
            'Life-loss saved callback ID matrix layout mismatch')
    body=function_body(sources[entry['source']],entry['name'])
    require('memset(marked, 0, sizeof(marked));' in body and 'paiVar8' not in body and 'bVar9' not in body,
            'Life-loss native matrix clearing reconstruction mismatch')
    require(component['routes']==dict(gain_worker='268f730',mastery_callback='22d0100',oath_callback='24040a0',sacrifice='26e5080'),
            'Life-loss callback route identities mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES,'Life-loss native site membership mismatch')
    sites={s['address']:s['bytes'] for s in component['control_sites']}
    expected={'268f1ef':'e8bcffffff','268f203':'83fa02','268f22b':'e840feffff','268f234':'31c0',
              '268f236':'b92e010000','268f23b':'8dbd3cfbffff','268f241':'f3ab','268f3e5':'e8069d0300',
              '268f404':'e85794fcff','268f4d0':'e8ab5b0500','268f56f':'e8bc010000','268f59a':'e8919e0200',
              '268f5c1':'e8da4ad7ff','268f611':'e8ca4c0400','268f63a':'e8f1000000','268f656':'e8a50ac4ff'}
    require(sites==expected,'Life-loss native clear/recursion/callback semantics mismatch')
    for site in component['control_sites']:
        code=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(code))==code,'Life-loss native instruction mismatch')
    for target,name in [('268f1b0','lose_life_2022'),('268f730','gain_life_impl_2022'),
                         ('22d0100','lichs_mastery_lifeloss_trigger_2022'),('26e5080','impose_sacrifice_2022')]:
        require(any(c['target']==target and c['name']==name for c in detail['canonical_calls']),
                'Life-loss recovered callback binding mismatch')
    binding=component['gain_binding'];gain=next(f for f in manifest['accepted_functions'] if f['address']=='268f730')
    require(gain['life_loss_gain_binding_2022']=={k:v for k,v in binding.items() if k!='previous_record'} and
            gain['reconstruction_body_sha256']==binding['body_sha256'],'Life-loss coordinated gain record mismatch')
    gain_body=function_body(sources[gain['source']],gain['name']).replace('\r\n','\n')
    restored,count=rename(gain_body,{'lose_life_2022':'lose_life'})
    require(count==2 and hashlib.sha256(gain_body.encode()).hexdigest()==binding['body_sha256'] and
            hashlib.sha256(restored.encode()).hexdigest()==binding['previous_body_sha256']==binding['previous_record']['reconstruction_body_sha256'],
            'Life-loss gain worker changed outside two recorded callee names')
    require(hashlib.sha256(machine_bytes(0x268f730,binding['machine_bytes'])).hexdigest()==binding['machine_sha256'],
            'Life-loss gain worker native body mismatch')
    require([c['address'] for c in binding['canonical_calls']]==['268f809','268f8a8'] and
            [c['bytes'] for c in binding['canonical_calls']]==['e8a2f9ffff','e803f9ffff'],
            'Life-loss gain worker native loss call sites mismatch')
    updated=gain['life_core_evidence'];previous=binding['previous_record']['life_core_evidence']
    require(updated==interfaces['life_core_2022']['functions']['268f730'] and
            {k:v for k,v in updated.items() if k!='canonical_calls'}=={k:v for k,v in previous.items() if k!='canonical_calls'} and
            updated['canonical_calls']==sorted(previous['canonical_calls']+binding['canonical_calls'],key=lambda c:int(c['address'],16)),
            'Life-loss earlier gain component changed outside recorded callback evidence')
