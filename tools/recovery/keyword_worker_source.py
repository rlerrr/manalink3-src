"""Readable keyword workers with their existing private/public C interfaces."""
from ability_query_source import canonical

EXPECTED={'deathtouch_part': '2618e445b9ec60119c40a7c50d088b5dee721160922952907939dfbac7c78bd1', 'hexproof_part': 'fa0f7b4c10428dfb847d96b7c0ec5f6c9d97fadc28c1bf80dcec315213cffdf8', 'indestructible_part': '3690a730d10c7dae84d7d8c322caaec6f77997b37cb6afd4ea0931ab28ba4ec1', 'attack_if_able_part': '243573a67f23ec074c0fd03c89f09a39410af74ba394ed369d72dd14523d6b11', 'vigilance_part': 'f5ae9ea4268b7895fec20c3afe8c8e3468a1e1466a2eb6b27558eb734b061b80', 'block_if_able': '53b9aee5cc951c467f30944cbbffe429a84bfe27fd22d5a5c3e7bc75dbcdc8d9'}
BODIES={
'deathtouch_part':'''static void deathtouch_part(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  card_ext_t *extension = instance->ext;
  if (player_bits[player] & PB_CANT_HAVE_OR_GAIN_DEATHTOUCH)
    extension->sp_keywords &= ~SP_KEYWORD_DEATHTOUCH;
  else
    extension->sp_keywords |= SP_KEYWORD_DEATHTOUCH;
}''',
'hexproof_part':'''static void hexproof_part(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  card_ext_t *extension = instance->ext;
  if (player_bits[player] & PB_CANT_HAVE_OR_GAIN_HEXPROOF)
    extension->sp_keywords &= ~SP_KEYWORD_HEXPROOF;
  else
    extension->sp_keywords |= SP_KEYWORD_HEXPROOF;
}''',
'indestructible_part':'''static void indestructible_part(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  instance->ext->sp_keywords |= SP_KEYWORD_INDESTRUCTIBLE;
}''',
'attack_if_able_part':'''static int attack_if_able_part(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (!(instance->state & STATE_UNKNOWN8000))
  {
    instance->state |= STATE_UNKNOWN8000;
    if (can_attack(player, card))
      must_attack = 1;
  }
  return player;
}''',
'vigilance_part':'''static void vigilance_part(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  instance->ext->sp_keywords |= SP_KEYWORD_VIGILANCE;
  if (player != -1 && card != -1 && is_humiliated(player, card, AL_KEY))
    return;
  instance->state |= STATE_VIGILANCE;
}''',
'block_if_able':'''int block_if_able(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  unsigned int keyword = instance->ext->sp_keywords;
  if (!(keyword & SP_KEYWORD_MUST_BLOCK))
    instance->ext->sp_keywords = keyword | SP_KEYWORD_MUST_BLOCK;
  if (current_turn != player && event == EVENT_DECLARE_BLOCKERS)
    select_blocker(player, card, player);
  return 0;
}'''
}


def project(before,name):
    assert canonical(before)==EXPECTED[name]
    return BODIES[name]
