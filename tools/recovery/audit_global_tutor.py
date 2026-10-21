"""Audit native tutor domains, VLA field decoding and control-transfer stores."""
import re
from recover_global_tutor import ADDRESSES,CONTROL_SITES,definitions
from audit_source import function_body


def audit_component(component,manifest,sources,machine_bytes,root):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('global_tutor_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']) and not any(f['static'] for f in entries.values()),
            'Global tutor membership/public linkage mismatch')
    for address,detail in component['functions'].items():
        require(entries[address]['global_tutor_evidence']==detail,'Global tutor function evidence mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES]==[2,7,16],'Global tutor physical parameter count mismatch')
    require(component['test_layout']==dict(bytes=216,qty_offset=174,qty_bytes=2,no_shuffle_offset=192,create_minideck_offset=194,storage_offset=212),
            'Native tutor test field layout mismatch')
    require(component['vla_bound']['expression_bytes']==[119,174,1,148,2,10,255,255,26,49,28] and
            any(v['name']=='tutored' for v in component['locals']['2651990']),'Native tutor VLA bound expression mismatch')
    require(component['destinations']==dict(TUTOR_0=0,TUTOR_HAND=1,TUTOR_DECK=2,TUTOR_GRAVE=3,TUTOR_PLAY=4,TUTOR_PLAY_TAPPED=5,
            TUTOR_RFG=6,TUTOR_EXILE=6,TUTOR_GET_ID=7,TUTOR_PLAY_ATTACKING=8,TUTOR_GET_POSITION=9,TUTOR_BOTTOM_OF_DECK=10,
            TUTOR_TPLAYER_HAND=11,TUTOR_BOTTOM_OF_DECK_RANDOM=12,TUTOR_HAND_AND_MARK=13,TUTOR_OPPOSITION_AGENT=14) and
            component['origins']==dict(TUTOR_FROM_DECK=1,TUTOR_FROM_GRAVE=2,TUTOR_FROM_RFG=3,TUTOR_FROM_HAND=4,TUTOR_FROM_GRAVE_NOTARGET=5,TUTOR_FROM_DECK_AUTO=6),
            'Native tutor enum values mismatch')
    require((root/'defs.h').read_text().count(definitions(component['destinations'],component['origins']))==1,
            'Native tutor domain definitions mismatch')
    require(component['timestamp_arrays']==dict(card_address='5076f0',player_address='507ec0',elements=500,element_bytes=4),
            'Native control-transfer timestamp layout mismatch')
    linker=(root/'manalink.lds').read_text()
    for name,address in [('timestamp_card','5076f0'),('timestamp_player','507ec0')]:
        require(re.search('_'+name+r'\s*=\s*0x'+address+r';',linker,re.IGNORECASE) and
                'extern int '+name+'[500];' in sources['functions/gain_control_and_legend_rule.c'],
                'Existing timestamp source/linker binding mismatch')
    helper=component['parameters']['2650b80']
    require(component['side_effect_helper']==dict(address='2650b80',physical_arguments=['player'],retained_unused_source_argument='targ_player') and
            [p['name'] for p in helper]==['player','targ_player'] and
            all('DW_OP_GNU_parameter_ref' in x['expression'] for x in helper[1]['location']),
            'Native tutor side-effect helper unused argument mismatch')
    helper_body=function_body(sources['functions/tutors.c'],'side_effects_to_tutor_deck_from_permanents_2022').split('{',1)[1]
    require(not re.search(r'\btarg_player\b',helper_body),'Retained side-effect helper now consumes its nonphysical argument')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES,'Global tutor instruction membership mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(data))==data,'Global tutor native operand mismatch')
    sites={s['address']:s['bytes'] for s in component['control_sites']}
    expected={'26519a1':'668bb7c0000000','26519b8':'668b87ae000000','26519bf':'c1e002','26519c7':'29c4',
              '26519cc':'8d44241c','265214f':'89c6','2663a51':'81ce08000040','2663ab1':'0d00000300',
              '2663abc':'890495f0765000','2663aca':'893495c07e5000'}
    require(all(sites[a]==b for a,b in expected.items()),'Native tutor VLA, hand-ID or control-transfer semantics mismatch')
    require(component['native_edge_cases']==dict(single_cancel_returns_minus_two=True,multi_cancel_keeps_selected=True,
            cage_failure_does_not_advance_count=True,hand_id_used_for_instance_marking=True,failed_control_allocation_still_runs_tail=True),
            'Native tutor edge-case scope changed')
    require(any(c['target']=='2651990' and c['name']=='new_global_tutor_2022' for c in component['functions']['26524d0']['canonical_calls']) and
            any(c['target']=='2663990' and c['name']=='gain_control_impl_2022' for c in component['functions']['2651990']['canonical_calls']),
            'Native tutor wrapper/control-transfer callee mismatch')
