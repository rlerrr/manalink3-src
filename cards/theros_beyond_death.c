#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x235a610; original cards/theros_beyond_death.c:180. */
int card_alseid_of_lifes_bounty(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  target_definition_t td;

  lifelink(player, card, event);
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 6);
    td.preferred_controller = player;
    td.allowed_controller = player;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = global_enchantment(player, card, event);
      return result;
    }
    default_target_definition(player, card, &td, 6);
    td.preferred_controller = player;
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        uVar3 = select_a_protection(player);
        pump_ability_until_eot(instance->parent_controller, instance->parent_card,
                               instance->targets[0].player, instance->targets[0].card, 0, 0,
                               uVar3 | 0x10000000, 0);
      }
    }
  }
  result = generic_activated_ability(player, card, event, 0xe, 1, 0, 0, 0, 0, 0, 0, &td,
                                     "Select target creature or enchantment you control.");
  return result;
}

/* ManalinkEh 2022: 0x235c4a0; original cards/theros_beyond_death.c:1264. */
int card_dryad_of_the_ilysian_grove(int player, int card, event_t event)
{
  check_playable_lands(player);
  all_lands_are_basiclandtype(player, card, event, player, 1, 0x5009);
  all_lands_are_basiclandtype(player, card, event, player, 2, 0x5002);
  all_lands_are_basiclandtype(player, card, event, player, 3, 0x5001);
  all_lands_are_basiclandtype(player, card, event, player, 4, 0x5006);
  all_lands_are_basiclandtype(player, card, event, player, 5, 0x5007);
  if (event == EVENT_RESOLVE_SPELL)
  {
    recalculate_all_cards_in_play();
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x235d700; original cards/theros_beyond_death.c:1839. */
int card_funeral_rites(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    draw_cards(player, 2);
    lose_life(player, 2);
    mill(player, 2);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x235e7c0; original cards/theros_beyond_death.c:2338. */
int card_kunoros_hound_of_athreos(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  vigilance(player, card, event);
  menace(player, card, event);
  lifelink(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x235ea20; original cards/theros_beyond_death.c:2481. */
int card_mire_triton(int player, int card, event_t event)
{
  deathtouch(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  mill(player, 2);
  gain_life(player, 2);
  return 0;
}

/* ManalinkEh 2022: 0x2360e70; original cards/theros_beyond_death.c:3761. */
int card_thassas_oracle(int player, int card, event_t event)
{
  int num_cards;
  int result;
  test_definition_t test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    num_cards = devotion(player, card, 2, 0);
    default_test_definition(&test, 0xff);
    reveal_top_cards_of_library_and_choose(player, card, player, num_cards, 1, TUTOR_DECK, 0,
                                           TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &test);
    result = count_deck(player);
    if (result <= num_cards)
    {
      lose_the_game(1 - player);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23598e0; original cards/theros_beyond_death.c:3834. */
static int akroan_war_legacy(int player, int card, event_t event)
{
  all_must_attack_if_able(1 - player, event, -1);
  if ((event == EVENT_CAN_SKIP_TURN) && (player == current_turn))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}
