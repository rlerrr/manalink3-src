"""Audit scoped energy-card operand decoding against native x86 instructions."""
import re
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
from audit_source import function_body
from recover_energy_cards import ADDRESSES, ENUMS


def audit_component(component,manifest,sources,machine_bytes,interfaces):
    def require(condition,message):
        if not condition:
            raise ValueError(message)
    entries={f['address']:f for f in manifest['accepted_functions'] if f.get('energy_card_evidence')}
    require(set(entries)==set(ADDRESSES)==set(component['functions']) and not any(f['static'] for f in entries.values()),
            'Energy-card membership/linkage mismatch')
    require(component['enum_values']==ENUMS,'Energy-card native enums mismatch')
    require(component['player_counters']==interfaces['dependency_repairs_2022']['player_counters']==
            dict(address='4ef530',bytes=8,stride=4,fields=dict(poison_=0,experience_=1,energy_=2)),
            'Energy-card player-counter layout mismatch')
    require(component['cost_word']==dict(bytes=4,kind_shift=24,counter_shift=16,quantity_shift=8,
            life_shift=0,player_kind=2,energy_counter=2,physical_argument=10,stack_offset=40),
            'Energy-card packed-cost layout mismatch')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True
    total=0
    for address,detail in component['functions'].items():
        require(entries[address]['energy_card_evidence']==detail,'Energy-card function evidence mismatch')
        code=machine_bytes(int(address,16),detail['machine_bytes'])
        instructions=list(decoder.disasm(code,int(address,16)))
        by_site={f'{i.address:x}':i for i in instructions}
        body=function_body(sources[entries[address]['source']],entries[address]['name'])
        for site in detail['operand_sites']:
            total+=1
            ins=by_site[site['address']]
            require(ins.bytes.hex()==site['bytes'],'Energy-card native operand bytes mismatch')
            if site['kind'] in {'packed_cost','activation_mode'}:
                offset=40 if site['kind']=='packed_cost' else 12
                require(ins.mnemonic=='mov' and len(ins.operands)==2 and ins.operands[0].type==X86_OP_MEM and
                        ins.reg_name(ins.operands[0].mem.base)=='esp' and ins.operands[0].mem.disp==offset and
                        ins.operands[0].size==4 and ins.operands[1].type==X86_OP_IMM and ins.operands[1].imm==site['value'],
                        'Energy-card outgoing argument store mismatch')
                call=next(i for i in instructions if i.address>ins.address and i.mnemonic=='call')
                require(call.operands[0].type==X86_OP_IMM and call.operands[0].imm==0x269db00,
                        'Energy-card argument does not lead to native activated helper')
                if site['kind']=='packed_cost':
                    require(site['value'] in {0x2020100,0x2020200,0x2020300} and
                            f'GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, {(site["value"]>>8)&255})' in body,
                            'Energy-card symbolic cost mismatch')
                else:
                    require(site['value']==0x400001 and '(GAA_UNTAPPED | GAA_CAN_ONLY_TARGET_OPPONENT)' in body,
                            'Energy-card activation mode mismatch')
            else:
                require(site['kind']=='energy_byte' and any(o.type==X86_OP_MEM and o.size==1 and
                        o.mem.disp==0x4ef532 and o.mem.scale==4 for o in ins.operands) and
                        'ENERGY_COUNTERS(player)' in body,'Energy-card unsigned byte access mismatch')
    require(total==14,'Energy-card operand count mismatch')
    prompt=component['mutable_prompt_local']
    require(prompt['address']=='20bf370' and prompt['name']=='prompt' and prompt['type']=='char *' and
            re.search(r'(?m)^\s*char\s*\*prompt;',function_body(sources['cards/aether_revolt.c'],'card_consulate_turret')),
            'Energy-card mutable prompt interface mismatch')
