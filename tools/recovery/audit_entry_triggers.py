"""Verify native entry-trigger interfaces, cache addressing and private calls."""
from recover_entry_triggers import ADDRESSES, PRIVATE, CONTROL_SITES


def audit_component(component,manifest,sources,machine_bytes):
    def require(condition,message):
        if not condition:raise ValueError(message)

    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('entry_trigger_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']), 'Entry-trigger component membership mismatch')
    require({a for a,f in entries.items() if f['static']}==PRIVATE, 'Entry-trigger private linkage mismatch')
    for address,detail in component['functions'].items():
        require(entries[address]['entry_trigger_evidence']==detail, 'Entry-trigger function evidence mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES]==[4,7,3,4,3], 'Entry-trigger parameter count mismatch')
    private=component['parameters']['2319fb0']
    require([p['name'] for p in private]==['player','card','event','mode'], 'Exploit private source parameters mismatch')
    for p,expression in zip(private,['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)','DW_OP_fbreg: 0']):
        require(expression in str(p['location']), 'Exploit private physical register/stack parameter mismatch')
    require(any(v['name']=='test' and v['type']=='test_definition_t' and v['location']=='(DW_OP_fbreg: -236)'
                for v in component['locals']['2319fb0']), 'Exploit native test local mismatch')
    require(component['card_count_cache']==dict(address='2797200',players=2,row_bytes=7,element_bytes=1,used_columns=[1,2,3,4]),
            'Entry-trigger count-cache layout mismatch')
    require(component['test_filter']==dict(bytes=216,not_me_offset=170,not_me_bytes=2), 'Entry-trigger test filter layout mismatch')
    require(component['trigger_context']==dict(regen_status_offset=40,mask=0x6000000,ability_loss_mask=2),
            'Entry-trigger context/ability category mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES, 'Entry-trigger native site membership mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(data))==data, 'Entry-trigger native instruction mismatch')
    sites={s['address']:s['bytes'] for s in component['control_sites']}
    expected={'26a5424':'6b450807','26a542a':'8a9804727902','26a5455':'8a8802727902',
              '26a5483':'8a9001727902','26a553d':'8a8803727902','26a5b22':'6683b8aa00000001',
              '26a5b5f':'81482800000006','231a03a':'c744240c40000000','231a469':'e942fbffff',
              '2516d13':'81ca00000006'}
    require(all(sites[a]==b for a,b in expected.items()), 'Entry-trigger native cache/filter/call semantics mismatch')
    require('count_harmonic_prodigy(' in sources['functions/functions.c'], 'Entry-trigger repetition dependency missing')
    require(any(call['target']=='2319fb0' and call['name']=='exploit_impl_2022'
                for call in component['functions']['231a430']['canonical_calls']), 'Exploit private tail-call binding mismatch')
