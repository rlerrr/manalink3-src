"""Check native cast-trigger layout, copy filtering and retained legacy bodies."""
from audit_source import function_body
from recover_spell_trigger_native import ADDRESSES,CONTROL_SITES


def audit_component(component,manifest,sources,machine_bytes,root):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('spell_trigger_evidence')}
    require(set(entries)==set(component['functions'])==set(component['previous_records'])==set(ADDRESSES),
            'Native spell-trigger component membership mismatch')
    require({a for a,f in entries.items() if f['static']}=={'2684f40'},'Native spell-trigger private linkage mismatch')
    for address,detail in component['functions'].items():
        entry=entries[address];previous=component['previous_records'][address]
        require(entry['spell_trigger_evidence']==detail and entry['name']==previous['name']+'_2022',
                'Native spell-trigger function evidence/name mismatch')
        require('test_definition_2022_t *test' in entry['prototype'],'Native spell-trigger test prototype mismatch')
        require(entry['source']==previous['source']=='functions/functions.c','Native spell-trigger source mismatch')
    require(component['test_bytes']==216 and component['not_me_offset']==170 and
            component['copied_spell_word']==dict(extension_offset=184,mask=0x80000000),
            'Native spell-trigger test/extension layout mismatch')
    require(component['control_sites']==CONTROL_SITES,'Native spell-trigger control membership mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(data))==data,'Native spell-trigger storage/branch bytes mismatch')
    fragments=component['legacy_fragments']
    require({f['address'] for f in fragments}==set(ADDRESSES) and len(fragments)==2 and
            all(f in manifest['source_fragments'] for f in fragments),'Legacy spell-trigger registry mismatch')
    for fragment in fragments:
        previous=component['previous_records'][fragment['address']]
        require(fragment['original_function']==previous['name'] and fragment['raw_sha256']==previous['raw_sha256'] and
                fragment['raw_export']=='decompiled-support/'+fragment['address']+'-legacy-spell-trigger.c',
                'Legacy spell-trigger original export mismatch')
        from unify_scalar_helpers import retained_source_body
        body=retained_source_body(root,fragment['source'],fragment['original_function'])
        require('test_definition_t *test' in body and 'test_definition_2022_t' not in body,
                'Legacy spell-trigger test interface changed')
    body=function_body(sources['functions/functions.c'],'sp_ability_trigger_impl_2022')
    require('0x80000000U' in body and 'get_special_flags2_by_instance(' not in body and 'test->not_me' in body,
            'Native spell-trigger copied-spell/not-me projection mismatch')
