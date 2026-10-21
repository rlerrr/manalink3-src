"""Verify private mechanic parameters, discarded return and shared prompt view."""
from recover_level_buddy import ADDRESSES, PRIVATE, CONTROL_SITES, direct_reference_sites


def audit_component(component,manifest,sources,machine_bytes,image):
    def require(condition,message):
        if not condition:raise ValueError(message)

    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('level_buddy_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']), 'Level Up/Buddy membership mismatch')
    require({a for a,f in entries.items() if f['static']}==PRIVATE, 'Level Up/Buddy private linkage mismatch')
    for address,detail in component['functions'].items():
        require(entries[address]['level_buddy_evidence']==detail, 'Level Up/Buddy function evidence mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES]==[15,21,5,5], 'Mechanic source parameter count mismatch')
    level=component['parameters']['24ac850']
    expected=['player','card','event','l2','l2power','l2toughness','l2key','l2sp','l3','l3power','l3toughness','l3key','l3sp']
    require([p['name'] for p in level[:13]]==component['level_physical_parameters']==expected,
            'Level Up private physical parameter ordering mismatch')
    require({p['name'] for p in level[13:]}==set(component['level_nonphysical_parameters'])=={'l1power','l1toughness'} and
            all(all('DW_OP_GNU_parameter_ref' in x['expression'] for x in p['location']) for p in level[13:]),
            'Level Up nonphysical base-power/toughness parameter mismatch')
    for address in PRIVATE:
        for p,expression in zip(component['parameters'][address][:3],['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)']):
            require(p['location'][0]['expression']=='('+expression+')', 'Mechanic private register parameter mismatch')
    result=component['buddy_private_return']
    require(result==dict(source_type='int',modeled_type='void',result_discarded=True,caller='258f6c0',
            call='258f70c',zero_return='258f6d5',native_direct_references=['258f70c']) and
            direct_reference_sites(image,0x258e270)==result['native_direct_references'],
            'Buddy private discarded return/caller evidence mismatch')
    require(entries['258e270']['prototype'].startswith('static void buddy_creature_part_2022'),
            'Buddy private source result type mismatch')
    require(component['shared_prompt']==dict(test_bytes=216,message_offset=0,message_bytes=100,frame_offset=-236),
            'Buddy test/prompt prefix layout mismatch')
    fixed={(v['name'],v['type'],v['location']) for v in component['locals']['258e270'] if isinstance(v['location'],str)}
    require({('test','test_definition_t','(DW_OP_fbreg: -236)'),('msg2','char[100]','(DW_OP_fbreg: -236)'),
             ('msg','char[100]','(DW_OP_fbreg: -336)')}<=fixed, 'Buddy overlapping prompt/test location mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES, 'Mechanic control evidence membership mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(data))==data, 'Mechanic native instruction mismatch')
    sites={s['address']:s['bytes'] for s in component['control_sites']}
    expected_sites={'258f70c':'e85febffff','258f711':'ebc2','258f6d5':'31c0',
                    '258e43b':'8985b4feffff','258e482':'89442420','24ace97':'e8b4f9ffff'}
    require(all(sites[a]==b for a,b in expected_sites.items()), 'Mechanic native parameter/result/prompt semantics mismatch')
    for address,private_name in [('24ace40','level_up_effects_part_2022'),('258f6c0','buddy_creature_part_2022')]:
        require(any(c['name']==private_name for c in component['functions'][address]['canonical_calls']),
                'Mechanic public/private call binding mismatch')
