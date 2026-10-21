"""Audit physical specialization, native poison bytes and hidden-debt storage."""
from audit_source import function_body
from recover_cannot_lose import ADDRESSES,PRIVATE,CONTROL_SITES


def audit_component(component,manifest,sources,machine_bytes,interfaces):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('cannot_lose_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']),'Cannot-lose component membership mismatch')
    require({a for a,f in entries.items() if f['static']}==PRIVATE,'Cannot-lose private linkage mismatch')
    for address,detail in component['functions'].items():
        require(entries[address]['cannot_lose_evidence']==detail,'Cannot-lose function evidence mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES]==[7,4,4],'Cannot-lose source parameter count mismatch')
    parameters=component['parameters']['26a3060']
    require([p['name'] for p in parameters]==['player','card','event','who_cant_lose','for_poison','for_draw_on_empty_deck','for_life'],
            'Cannot-lose specialized parameter order mismatch')
    for p,expression in zip(parameters,['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)','DW_OP_fbreg: 0','DW_OP_fbreg: 4','DW_OP_fbreg: 8']):
        require(expression in str(p['location']),'Cannot-lose physical register/stack argument mismatch')
    require(parameters[6]['constant']==1 and parameters[6]['location'] is None and
            component['specialized_constant']==dict(name='for_life',value=1,has_storage=False,physical_arguments=6),
            'Cannot-lose optimized constant parameter mismatch')
    body=function_body(sources['functions/functions.c'],'cannot_lose_the_game_impl_2022')
    require('for_life' not in body and len(entries['26a3060']['prototype'].split('(',1)[1].split(')',1)[0].split(','))==6,
            'Cannot-lose worker retains a nonphysical argument')
    require(component['counter_layout']==interfaces['dependency_repairs_2022']['player_counters']==
            dict(address='4ef530',bytes=8,stride=4,fields=dict(poison_=0,experience_=1,energy_=2)),
            'Cannot-lose poison byte layout mismatch')
    require(component['storage_offsets']==dict(life_debt=156,poison_debt=160,protected_player=164,leaving_marker=168,leaving_value=17),
            'Cannot-lose debt/protected-player storage mismatch')
    require(component['public_modes']=={'26a3390':dict(for_poison=0,for_draw_on_empty_deck=0),'26a33c0':dict(for_poison=1,for_draw_on_empty_deck=1)},
            'Cannot-lose public wrapper modes mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES,'Cannot-lose native site membership mismatch')
    expected={'26a3177':'8a1cbd30f54e00','26a3214':'8a148d30f54e00','26a31f3':'c782a800000011000000',
              '26a310b':'2b829c000000','26a316a':'898a9c000000','26a33a4':'c7451000000000','26a33ab':'c7450c00000000',
              '26a33d4':'c7451001000000','26a33db':'c7450c01000000','26a33b3':'e9a8fcffff','26a33e3':'e978fcffff'}
    require({s['address']:s['bytes'] for s in component['control_sites']}==expected,'Cannot-lose native field/mode semantics mismatch')
    for site in component['control_sites']:
        code=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(code))==code,'Cannot-lose instruction mismatch')
    for address,mode in [('26a3390',0),('26a33c0',1)]:
        wrapper=function_body(sources['functions/functions.c'],entries[address]['name'])
        require(f'who_cant_lose, {mode}, {mode})' in wrapper and
                any(c['target']=='26a3060' and c['name']=='cannot_lose_the_game_impl_2022' for c in component['functions'][address]['canonical_calls']),
                'Cannot-lose private wrapper binding mismatch')
