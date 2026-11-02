"""Restore the linear keyword-dispatch sequence without caching engine globals."""
import re
from ability_query_source import canonical

EXPECTED='551d976f94fe3794f578bb964e9e4c5a450e8fa520139484e9bb7fb5244bf783'
PLAIN=[('SP_KEYWORD_FEAR',1),('SP_KEYWORD_LIFELINK',2),('SP_KEYWORD_HASTE',4)]


def project(before,*,expanded=False):
    assert canonical(re.sub(r'\b_Bool\b','bool',before))==EXPECTED
    lines=['void special_abilities(int player, int card, event_t event, int sp_abilities,','                            int source_player, int source_card)','{','  card_instance_t *instance;','  card_instance_t *target;','','  instance = get_card_instance(player, card);']
    def guard(name):return '  if ((sp_abilities & '+name+') && event == EVENT_ABILITIES &&\n      card == affected_card && player == affected_card_controller)'
    def store(name,late=False):
        variable='instance' if late else 'target';lines.extend([guard(name),'  {','    '+variable+' = get_card_instance(player, card);','    '+variable+'->ext->sp_keywords |= '+name+';','  }'])
    def part(name,callee):lines.extend([guard(name),'  {','    '+callee+'(player, card);','  }'])
    for name,_ in PLAIN:store(name)
    part('SP_KEYWORD_VIGILANCE','vigilance_part');store('SP_KEYWORD_UNBLOCKABLE');store('SP_KEYWORD_CANNOT_BLOCK');store('SP_KEYWORD_SHADOW');part('SP_KEYWORD_DEATHTOUCH','deathtouch_part')
    lines.extend(['  if ((sp_abilities & SP_KEYWORD_MUST_ATTACK) && event == EVENT_BEGIN_COMBAT &&','      player == current_turn && forbid_attack == 0)','  {','    attack_if_able_part(player, card);','  }'])
    part('SP_KEYWORD_INDESTRUCTIBLE','indestructible_part')
    lines.extend(['  if (sp_abilities & SP_KEYWORD_MUST_BE_BLOCKED)','  {','    lure_effect(source_player, source_card, event, player, card, 0, -1, 0, NULL);','  }','  if (sp_abilities & SP_KEYWORD_FLANKING)','  {','    granted_flanking(source_player, source_card, player, card, event);','  }'])
    store('SP_KEYWORD_WITHER')
    lines.extend(['  if (sp_abilities & SP_KEYWORD_LURE)','  {','    lure_effect(source_player, source_card, event, player, card, 1, -1, 0, NULL);','  }'])
    part('SP_KEYWORD_HEXPROOF','hexproof_part');store('SP_KEYWORD_INTIMIDATE')
    lines.extend(['  if (sp_abilities & SP_KEYWORD_MUST_BLOCK)','  {','    block_if_able(player, card, event);','  }','  if (sp_abilities & SP_KEYWORD_SHADOW_HOSER)','  {','    instance->ext->sp_keywords |= SP_KEYWORD_SHADOW_HOSER;','  }'])
    for name in ['SP_KEYWORD_MENACE','SP_KEYWORD_MINBLOCKERS_3','SP_KEYWORD_MAXBLOCKERS_1','SP_KEYWORD_SKULK','SP_KEYWORD_SQUIRRELLINK','SP_KEYWORD_HORSEMANSHIP']:store(name,True)
    lines.append('}');after='\n'.join(lines)
    return after.replace('NULL','((void *)0)') if expanded else after
