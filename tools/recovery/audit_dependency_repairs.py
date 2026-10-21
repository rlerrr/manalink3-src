"""Verify the field operands and physical signature behind four native helpers."""
import re
from recover_dependency_repairs import ADDRESSES,PRIVATE,SITES


def audit_component(component,manifest,machine_bytes,root):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('dependency_repair_evidence')}
    require(set(entries)==set(component['functions'])==set(ADDRESSES),'Dependency repair membership mismatch')
    require({a for a,f in entries.items() if f['static']}==PRIVATE,'Dependency repair private linkage mismatch')
    for address,detail in component['functions'].items():
        require(entries[address]['dependency_repair_evidence']==detail,'Dependency repair function evidence mismatch')
        expected=3 if address in PRIVATE else 2
        require(len(component['symbols'][address])==1 and component['symbols'][address][0]['storage']==expected,
                'Dependency repair native linkage mismatch')
    require(component['symbols']['25a22a0']==[dict(name='_miara_trigger.isra.0',storage=3)],'Miara optimized worker identity mismatch')
    parameters=component['parameters']['25a22a0']
    require([p['name'] for p in parameters]==['player','amount','card'] and
            [p['location'][0]['expression'] for p in parameters[:2]]==['(DW_OP_reg0 (r0))','(DW_OP_reg2 (r2))'] and
            'DW_OP_GNU_parameter_ref' in parameters[2]['location'][0]['expression'],
            'Miara physical/nonphysical parameter mismatch')
    require(component['miara_physical']==['player','amount'] and component['miara_nonphysical']==['card'] and
            entries['25a22a0']['prototype']=='static void miara_trigger(int player, int amount);',
            'Miara recovered physical signature mismatch')
    require(any(v['name']=='counter_types' and v['type']=='counter_t[9]' for v in component['locals']['26d4f30']),
            'Native counter-type array bound mismatch')
    require(component['player_counters']==dict(address='4ef530',bytes=8,stride=4,fields=dict(poison_=0,experience_=1,energy_=2)) and
            component['player_bits']==dict(address='4ef1a4',bytes=8,stride=4,legend_rule_mask=0x8000000),
            'Native player word/byte layout mismatch')
    linker=(root/'manalink.lds').read_text()
    require(re.search(r'_player_counters\s*=\s*0x4EF530;',linker) and re.search(r'_player_bits\s*=\s*0x4ef1a4;',linker),
            'Native player word/byte linker binding mismatch')
    require([s['address'] for s in component['control_sites']]==SITES,'Dependency repair instruction membership mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(data))==data,'Dependency repair native operand mismatch')
    controls={s['address']:s['bytes'] for s in component['control_sites']}
    expected={'26d4f5c':'803c9d32f54e0000','26d4f6a':'803c9d30f54e0000','26d4f78':'803c9d31f54e0000',
              '26a80ef':'81ca00000008','26a8109':'0d00000008','21c7ec0':'c744240800008000',
              '25a22a0':'85d2','25a22ad':'89d6','25a22b2':'89c3','25a246b':'e830feffff','25aca41':'e85a58ffff'}
    require(all(controls[a]==b for a,b in expected.items()),'Dependency repair predicate/parameter encoding mismatch')
