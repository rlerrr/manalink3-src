"""Verify native specific enters-play filters, wrappers and private worker."""
import re
from audit_source import function_body
from recover_specific_cip import ADDRESSES, PRIVATE, CONTROL_SITES


def audit_component(component,manifest,sources,machine_bytes):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('specific_cip_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']),'Specific enters-play membership mismatch')
    require({a for a,f in entries.items() if f['static']}==PRIVATE,'Specific enters-play private linkage mismatch')
    for address,detail in component['functions'].items():
        require(entries[address]['specific_cip_evidence']==detail,'Specific enters-play function evidence mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES]==[7,15,7,6],'Specific enters-play parameter count mismatch')
    worker=component['parameters']['2684210']
    require([p['name'] for p in worker]==['controller_of_trigger','event','trigger_mode','type','test','triggering_player','triggering_card'],
            'Specific enters-play worker parameter order mismatch')
    for p,expr in zip(worker,['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)','DW_OP_fbreg: 0','DW_OP_fbreg: 4','DW_OP_fbreg: 8','DW_OP_fbreg: 12']):
        require(expr in str(p['location']),'Specific enters-play worker physical argument mismatch')
    require(any(v['name']=='test' and v['type']=='test_definition_t' and v['location']=='(DW_OP_fbreg: -228)'
                for v in component['locals']['2684410']),'Specific enters-play scalar test local mismatch')
    require(component['test_filter']==dict(bytes=216,not_me_offset=170,not_me_bytes=2) and
            component['scalar_flag_offsets']==dict(type_flag=108,subtype_flag=110,color_flag=124,id_flag=126,cmc_flag=136),
            'Specific enters-play filter layout mismatch')
    require(component['trigger_context']==dict(guard_address='7a2d78',guard_value=219,regen_status_offset=40,
            mask=0x6000000,target_player_offset=124,target_card_offset=128),'Specific enters-play context mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES,'Specific enters-play native site membership mismatch')
    sites={s['address']:s['bytes'] for s in component['control_sites']}
    expected={'268421f':'39351c7e7300','26842e0':'6683b8aa00000001','2684328':'81ca00000006',
              '26843f7':'89507c','26843fa':'898880000000','2684412':'813d782d7a00db000000',
              '2684484':'66895590','2684491':'66895592','268449e':'668955a0','26844ab':'668955a2',
              '26844c2':'668955ac','26844f1':'e81afdffff','2684513':'813d782d7a00db000000',
              '268454c':'e9bffcffff','2684565':'813d782d7a00db000000','268459b':'e970fcffff'}
    require(sites==expected,'Specific enters-play native filter/guard/store semantics mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(data))==data,'Specific enters-play instruction mismatch')
    for address in ['2684410','2684500','2684560']:
        detail=component['functions'][address]
        require(any(c['target']=='2684210' and c['name']=='new_specific_cip_impl_2022' for c in detail['canonical_calls']),
                'Specific enters-play private call binding mismatch')
        body=function_body(sources[entries[address]['source']],entries[address]['name'])
        require('trigger_condition' in body,'Specific enters-play wrapper guard missing')
        if address!='2684500':
            require(len(re.findall(r'\bt_player\b',body))==1,'Specific enters-play unused source parameter changed')
