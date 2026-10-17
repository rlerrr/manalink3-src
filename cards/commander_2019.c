#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23b1fa0; original cards/commander_2019.c:2. */
static int goad_effect(int player, int card, event_t event)
{
  card_instance_t *instance;
  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_card)
  {
    instance = get_card_instance(player, card);
    attack_if_able((int)instance->damage_target_player, instance->damage_target_card, event);
  }
  if (event == EVENT_CAN_SKIP_TURN)
  {
    instance = get_card_instance(player, card);
    if (instance->damage_source_player == current_turn)
    {
      kill_card(player, card, KILL_EXILE);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23b2090; original cards/commander_2019.c:31. */
int card_anje_falkenrath(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  haste(player, card, event);
  result = discard_trigger(player, card, event, player, 2, 0);
  if ((result != 0) &&
      (result = get_original_internal_card_id(trigger_cause_controller, trigger_cause),
       cards_data[result].cc[2] == '\v'))
  {
    untap_card(player, card);
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      draw_a_card(player);
    }
  }
  result = generic_activated_ability(player, card, event, 0x21, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23b21b0; original cards/commander_2019.c:62. */
int card_apex_altisaur(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if ((event != EVENT_ETB) && (result = damage_dealt_to_me_arbitrary(
                                   player, card, event, DDBM_TRIGGER_OPTIONAL, player, card),
                               result == 0))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = 1 - player;
  result = can_target(&td);
  if ((result != 0) &&
      (result = new_pick_target(&td, "Select target creature you don\'t control.", 0, 0x80),
       result != 0))
  {
    instance = get_card_instance(player, card);
    result = instance->targets[0].card;
    instance->number_of_targets = '\0';
    fight(player, card, instance->targets[0].player, result);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23b28a0; original cards/commander_2019.c:384. */
int card_dockside_extortionist(int player, int card, event_t event)
{
  int howmany;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  howmany = count_permanents_by_type(1 - player, TYPE_ARTIFACT | TYPE_ENCHANTMENT);
  generate_tokens_by_id(player, card, 0x4998, howmany);
  return 0;
}

/* ManalinkEh 2022: 0x23b2cd0; original cards/commander_2019.c:486. */
int card_gerrard_weatherlight_hero(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  exile_from_owners_graveyard(player, card);
  reanimate_all_dead_this_turn(player, -0x7fffffbe);
  return 0;
}

/* ManalinkEh 2022: 0x23b2f70; original cards/commander_2019.c:572. */
int card_gift_of_doom(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      morph(player, card, EVENT_CAST_SPELL, 0, 0, 0, 0, 0, 0);
      result = get_id(player, card);
      if (result == 0x33c4)
      {
        return 0;
      }
      result = generic_aura(player, card, EVENT_CAST_SPELL, player, 0, 0, 0, 0xa00, 0, 0, 0);
      return result;
    }
  }
  else if ((((event == EVENT_CAN_CAST) || (event == EVENT_RESOLVE_SPELL)) ||
            (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
    goto LAB_023b2fac;
  if ((((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (2 < event - EVENT_POWER)) &&
        ((event != EVENT_RESOLVE_MOVING_AURA &&
          ((event != EVENT_CHANGE_TYPE && (event != EVENT_SET_COLOR)))))) &&
       (event != EVENT_GET_SELECTED_CARD)) &&
      (event != EVENT_STATIC_EFFECTS))
  {
    if (event == EVENT_CAN_UNMORPH)
    {
      default_test_definition(&this_test, 2);
      this_test.qty = 1;
      this_test.not_me = 1;
      result = new_can_sacrifice_as_cost(player, card, &this_test);
      return result;
    }
    if (event == EVENT_UNMORPH)
    {
      new_default_test_definition(&this_test, 2, "Select a creature to sacrifice.");
      this_test.not_me = 1;
      result = new_sacrifice(player, card, player, SAC_AS_COST, &this_test);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
    }
    else if (event == EVENT_TURNED_FACE_UP)
    {
      result = vanilla_aura(player, card, EVENT_CAN_CAST, player);
      if (result == 0)
      {
        kill_card(player, card, KILL_STATE_BASED_ACTION);
      }
      else
      {
        vanilla_aura(player, card, EVENT_CAST_SPELL, player);
        vanilla_aura(player, card, EVENT_RESOLVE_SPELL, player);
      }
    }
    result = morph(player, card, event, 0, 0, 0, 0, 0, 0);
    return result;
  }
LAB_023b2fac:
  morph(player, card, event, 0, 0, 0, 0, 0, 0);
  result = generic_aura(player, card, event, player, 0, 0, 0, 0xa00, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x23b2030; original cards/commander_2019.c:904. */
static char *commander_you_own(int who_chooses, int player, int card, int targeting_player,
                               int targeting_card)
{
  int result;
  char *pcVar2;
  result = get_owner(player, card);
  pcVar2 = "owner";
  if (result == targeting_player)
  {
    result = check_special_flags2(player, card, 0x1000000);
    pcVar2 = NULL;
    if (result == 0)
    {
      return "must be a commander";
    }
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x23b3dd0; original cards/commander_2019.c:983. */
int card_sevinnes_reclamation(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if ((event - EVENT_CAST_SPELL & 0xfffffff7) != 0)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = check_special_flags(player, card, 0x10000000);
      if (result != 0)
      {
        result = check_special_flags2(player, card, -0x80000000);
        if (result == 0)
        {
          copy_spell_from_stack(player, player, card);
        }
      }
      result = get_flashback(player, card);
      if (result != 0)
      {
        kill_card(player, card, KILL_EXILE);
      }
      result = validate_target_from_grave_source(player, card, player, 0);
      if (result != -1)
      {
        reanimate_permanent(player, card, player, result, REANIMATE_DEFAULT);
      }
      kill_card(player, card, KILL_DESTROY);
    }
    result = do_flashback(player, card, event, 4, 0, 0, 0, 0, 1);
    return result;
  }
  new_default_test_definition(&test, 0x47, "Select a permanent card with mana value 3 or less.");
  test.cmc = 4;
  test.cmc_flag = 3;
  result = generic_spell(player, card, event, 8, NULL, NULL, 0, &test);
  return result;
}
