"""Check native death-trigger filters, packed counts and optimized worker ABI."""
from recover_death_trigger_core import ADDRESSES,PRIVATE,CONTROL_SITES


def audit_component(component,manifest,machine_bytes):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('death_trigger_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']),'Death-trigger component membership mismatch')
    require({a for a,f in entries.items() if f['static']}==PRIVATE,'Death-trigger private linkage mismatch')
    for address,detail in component['functions'].items():
        require(entries[address]['death_trigger_evidence']==detail,'Death-trigger function evidence mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES]==[9,8,8,6,4],'Death-trigger physical parameter count mismatch')
    private=component['parameters']['2694580']
    require([p['name'] for p in private]==['player','card','event','t_player','type','this_test','extra_check','mode','storage_location'],
            'Death-trigger private parameter order mismatch')
    for p,expression in zip(private[:3],['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)']):
        require(expression in str(p['location']),'Death-trigger private register argument mismatch')
    require(any(v['name']=='csvids' and v['type']=='int[3]' and v['location']=='(DW_OP_fbreg: -32)'
                for v in component['locals']['2694580']),'Native meld ID array layout mismatch')
    require(component['test_filter']==dict(bytes=216,not_me_offset=170,not_me_bytes=2) and
            component['card_count_cache']==dict(address='2797200',players=2,row_bytes=7,element_bytes=1,used_columns=[5]) and
            component['stored_counters']==dict(word_offset=204,previous_word_offset=208,element_bytes=1),
            'Native death-trigger test/cache/packed storage mismatch')
    require(component['enum_values']['SF2_0']==0 and component['enum_values']['SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE']==0x10000000,
            'Native death-trigger replacement flag mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES,'Death-trigger native instruction membership mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(data))==data,'Death-trigger native operand mismatch')
    sites={s['address']:s['bytes'] for s in component['control_sites']}
    require(sites['269465e']=='6683bfaa00000001' and sites['2694800']=='8a8005727902' and
            sites['26949e3']=='8d55f0' and sites['26949ed']=='8d55ec' and sites['2694ad1']=='8b448de8',
            'Native death-trigger filter, multiplier or meld ID indexing mismatch')
    for address in ['2694c90','2694ce0','2694d30']:
        require(any(c['target']=='2694580' and c['name']=='count_for_gfp_ability_and_store_values_impl_2022'
                    for c in component['functions'][address]['canonical_calls']),'Native death-trigger wrapper binding mismatch')
