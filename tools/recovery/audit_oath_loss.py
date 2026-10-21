"""Verify the retained Oath source template, native VLA and engine callback binding."""
import hashlib,re
from audit_source import function_body
from inspect_release import Image
from recover_literals import readonly_string
from recover_test_consumers import rename
from recover_oath_loss import ADDRESSES,CONTROL_SITES


def audit_component(component,manifest,sources,machine_bytes,image,root,interfaces):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('oath_life_loss_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']) and not any(f['static'] for f in entries.values()),
            'Oath life-loss membership/public linkage mismatch')
    entry=entries['24040a0'];detail=component['functions']['24040a0']
    require(entry['oath_life_loss_evidence']==detail and len(component['parameters']['24040a0'])==3,
            'Oath life-loss function/parameter evidence mismatch')
    template=component['source_template'];source=sources['cards/ice_age.c']
    require(template['source']=='cards/ice_age.c' and template['name']=='oath_of_lim_dul_lifeloss_trigger' and
            hashlib.sha256(function_body(source,template['name']).replace('\r\n','\n').encode()).hexdigest()==template['body_sha256'],
            'Oath life-loss original source template changed')
    predicate=component['existing_predicate'];pred_body=function_body(source,'target_only_permanents_unless_in_hand')
    require(predicate['source']=='cards/ice_age.c' and predicate['name']=='target_only_permanents_unless_in_hand' and
            hashlib.sha256(pred_body.replace('\r\n','\n').encode()).hexdigest()==predicate['body_sha256'],
            'Oath life-loss existing predicate source changed')
    require(predicate['address']=='23ede20' and hashlib.sha256(machine_bytes(0x23ede20,predicate['machine_bytes'])).hexdigest()==predicate['machine_sha256'] and
            any(s['address']=='23ede20' and s['raw_sha256']==predicate['raw_sha256'] for s in entry['supporting_exports']),
            'Oath life-loss existing predicate native/supporting export mismatch')
    require(re.search(r'\bin_hand\s*\(',pred_body) and re.search(r'\bis_what\s*\(',pred_body) and
            'TYPE_PERMANENT' in pred_body and re.search(r'EXE_STR\(0x728f6c\)',pred_body,re.I),
            'Oath life-loss existing hand/permanent predicate contract mismatch')
    require(component['vla']==dict(element_bytes=8,extra_elements=1,upper_bound=[145,8,6]) and
            component['target_layout']==dict(bytes=84,frame_offset=-104,flags=0xc000),
            'Oath life-loss target/VLA layout mismatch')
    target=next(v for v in component['locals']['24040a0'] if v['name']=='tgts')
    require(target['upper_bound']==[[145,8,6]],'Oath life-loss native amount-plus-one bound mismatch')
    body=function_body(source,entry['name'])
    require('target_t tgts[amount + 1];' in body and 'target_definition_2022_t td;' in body and
            'target_only_permanents_unless_in_hand;' in body,'Oath life-loss source target layout/predicate mismatch')
    prompt=component['prompt']
    require(prompt['address']=='2749380' and readonly_string(image,0x2749380)==bytes.fromhex(prompt['bytes'])==
            b'Select a permanent to sacrifice or card to discard.' and '"Select a permanent to sacrifice or card to discard."' in body,
            'Oath life-loss native selection prompt mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES,'Oath life-loss native site membership mismatch')
    expected={'24040a6':'8d7da0','24040c5':'e876c12500','24040eb':'8d04c508000000','24040f2':'c745d800c00000',
              '24040f9':'c745c820de3e02','2404100':'e8cb802f00','2404105':'29c4','2404114':'8d742414',
              '240413f':'e87cc32500','240415b':'c744240c80937402','240416a':'e841ca2500','240418e':'e80d3c2800',
              '24041c7':'e8d43b2800','24041e7':'e814622900','24041fe':'e89df12a00','2404207':'c744240803000000','240421e':'e8dd172b00'}
    require({s['address']:s['bytes'] for s in component['control_sites']}==expected,'Oath life-loss native VLA/selection/discard/sacrifice semantics mismatch')
    for site in component['control_sites']:
        code=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(code))==code,'Oath life-loss native instruction mismatch')
    binding=component['loss_binding'];loss=next(f for f in manifest['accepted_functions'] if f['address']=='268f1b0')
    require(loss['oath_loss_binding_2022']=={k:v for k,v in binding.items() if k!='previous_record'} and
            loss['reconstruction_body_sha256']==binding['body_sha256'],'Oath coordinated loss-engine record mismatch')
    loss_body=function_body(sources[loss['source']],loss['name']).replace('\r\n','\n')
    restored,count=rename(loss_body,{'oath_of_lim_dul_lifeloss_trigger_2022':'oath_of_lim_dul_lifeloss_trigger'})
    require(count==1 and hashlib.sha256(loss_body.encode()).hexdigest()==binding['body_sha256'] and
            hashlib.sha256(restored.encode()).hexdigest()==binding['previous_body_sha256']==binding['previous_record']['reconstruction_body_sha256'],
            'Oath loss engine changed outside one recorded callee name')
    require(binding['canonical_calls']==[dict(address='268f5c1',bytes='e8da4ad7ff',target='24040a0',name='oath_of_lim_dul_lifeloss_trigger_2022')],
            'Oath loss engine native callback destination mismatch')
    updated=loss['life_loss_evidence'];previous=binding['previous_record']['life_loss_evidence']
    require(updated==interfaces['life_loss_2022']['functions']['268f1b0'] and
            {k:v for k,v in updated.items() if k!='canonical_calls'}=={k:v for k,v in previous.items() if k!='canonical_calls'} and
            updated['canonical_calls']==sorted(previous['canonical_calls']+binding['canonical_calls'],key=lambda c:int(c['address'],16)),
            'Oath earlier loss component changed outside callback evidence')
