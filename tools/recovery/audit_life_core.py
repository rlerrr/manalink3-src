"""Verify native life-state globals, flag projections and private/public routes."""
import re
from recover_life_core import ADDRESSES,PRIVATE,CONTROL_SITES,ENUM_DECLARATION,CONTEXT_DECLARATIONS


def audit_component(component,manifest,sources,machine_bytes,root):
    def require(condition,message):
        if not condition:raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('life_core_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']),'Life core membership mismatch')
    require({a for a,f in entries.items() if f['static']}==PRIVATE,'Life core private linkage mismatch')
    for address,detail in component['functions'].items():
        require(entries[address]['life_core_evidence']==detail,'Life core function evidence mismatch')
    require([len(component['parameters'][a]) for a in ADDRESSES]==[1,1,1,3,2,2,1,5],'Life core parameter count mismatch')
    for address,registers in [('2685c90',['DW_OP_reg0 (r0)']),('268f730',['DW_OP_reg0 (r0)','DW_OP_reg2 (r2)','DW_OP_reg1 (r1)'])]:
        for parameter,expression in zip(component['parameters'][address],registers):
            require(expression in str(parameter['location']),'Life core private physical argument mismatch')
    require(component['native_replacement_trigger']==dict(name='XTRIGGER_REPLACE_GAIN_LIFE',value=0x1000600,
            source_name='XTRIGGER2022_REPLACE_GAIN_LIFE',legacy_value=0x1000800),'Life replacement event domain mismatch')
    definitions=(root/'defs.h').read_text()
    require(definitions.count(ENUM_DECLARATION)==1 and re.search(r'\bXTRIGGER_REPLACE_GAIN_LIFE\s*=\s*0x01000800',definitions),
            'Life replacement event declaration/legacy value mismatch')
    require(component['event_flag']==dict(address='4ef188',bytes=4,mask=0x200000) and
            component['player_flag']==dict(address='4ef1a4',stride=4,bytes=4,mask=0x200000),
            'Life core event/player flag projection mismatch')
    require(component['draw_counter']==dict(name='life_gain_draw_count_2022',address='4ef538',elements=2,
            element_bytes=4,name_inferred=True),'Life core executable draw counter mismatch')
    require(component['context']==dict(push_address='435c80',pop_address='435cd0',declarations=CONTEXT_DECLARATIONS),
            'Life core executable context helper mismatch')
    require(component['game_loss_frontend']==dict(real_lose_address='4791f0',gain_trigger=224,gain_amount_address='73b6ac'),
            'Life core game-loss/gain-notification bindings mismatch')
    require(sources['functions/functions.c'].replace('\r\n','\n').count(CONTEXT_DECLARATIONS)==1,
            'Life core context macro declarations mismatch')
    events=sources['functions/events.c']
    for name,address in [('push_affected_card_stack','435c80'),('pop_affected_card_stack','435cd0')]:
        require(re.search(r'#define\s+'+name+r'\s+EXE_FN\(void,\s*0x'+address+r',\s*void\)',events,re.I),
                'Life core existing executable context signature mismatch')
    linker=(root/'manalink.lds').read_text()
    for name,address in [('life_gain_draw_count_2022','4ef538'),('player_bits','4ef1a4'),('event_flags','4ef188'),('real_lose_the_game','4791f0')]:
        require(re.search(r'(?im)^_'+name+r'\s*=\s*0x'+address+r';',linker),'Life core executable linker binding mismatch')
    header=(root/'manalink.h').read_text()
    require('extern int life_gain_draw_count_2022[2];' in header,'Life core executable draw-counter declaration missing')
    require(set(component['globals'])=={'life_to_lose_on_gain_life','suspend_cannot_lose_the_game'},'Life core state membership mismatch')
    for name,address in [('life_to_lose_on_gain_life','2797120'),('suspend_cannot_lose_the_game','2775460')]:
        evidence=component['globals'][name]
        require((evidence['address'],evidence['bytes'],evidence['coff_storage'],evidence['section'],evidence['zero_initialized'])==
                (address,4,2,'.bss',True) and evidence['type_evidence']['type']=='int' and evidence['type_evidence']['bytes']==4,
                'Life core external BSS state type/address mismatch')
        declaration=dict(source='functions/functions.c',name=name,declaration='int '+name+';',target_address=address,zero_initialized=True,linkage='external')
        require(declaration in component['global_declarations'] and declaration in manifest['source_globals'] and
                'extern int '+name+';' in header,'Life core state source/header declaration mismatch')
    require([s['address'] for s in component['control_sites']]==CONTROL_SITES,'Life core native site membership mismatch')
    sites={s['address']:s['bytes'] for s in component['control_sites']}
    expected={'268f0b9':'f60495a6f14e0020','26927f1':'8b1560547702','2692801':'e98a34ffff',
              '268f74b':'f60485a6f14e0020','268f76c':'f6058af14e0020','268f779':'b8805c4300','268f77e':'ffd0',
              '268f7b1':'8b3d20717902','268f7c0':'c7052071790200000000','268f7a6':'c744240400060001',
              '268f7d8':'893d20717902','268f7e4':'b8d05c4300','268f7e9':'ffd0','268f83c':'f6058af14e0020',
              '268f845':'8b049d38f54e00','268f90c':'e91ffeffff','268f9ff':'e92cfdffff','268f9f1':'b901000000',
              '268f901':'31c9','268f86c':'e88f000000','268f885':'e876000000','268f8f0':'e87bc00300','268f7ca':'e831c20300',
              '269283d':'a160547702','2692848':'e84334ffff','268be67':'813d782d7a00e0000000','268be8b':'8b1dacb67300',
              '2692885':'e96669defd','26928b9':'e93269defd'}
    require(sites==expected,'Life core native state/flags/callback semantics mismatch')
    for site in component['control_sites']:
        data=bytes.fromhex(site['bytes'])
        require(machine_bytes(int(site['address'],16),len(data))==data,'Life core instruction mismatch')
    for caller,target,name in [('26927f0','2685c90','can_lose_the_game_impl_2022'),('268f900','268f730','gain_life_impl_2022'),
                               ('268f9f0','268f730','gain_life_impl_2022'),('268f730','268f900','gain_life_2022'),
                               ('2692810','2685c90','can_lose_the_game_impl_2022')]:
        require(any(c['target']==target and c['name']==name for c in component['functions'][caller]['canonical_calls']),
                'Life core private/public route mismatch')
