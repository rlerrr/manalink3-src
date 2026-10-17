#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2372ff0; original cards/jumpstart.c:37. */
int card_archaeomender(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 0x40, "Select target artifact card.");
    result = count_graveyard_by_type(player, 0x40);
    if (result != 0)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        new_global_tutor(player, player, 2, 1, 1, 1, &test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23731b0; original cards/jumpstart.c:91. */
int card_brightmare(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = 1 - player;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "TARGET_CREATURE", 0, 0);
    if (result != 0)
    {
      tap_card(instance->targets[0].player, instance->targets[0].card);
      result = get_toughness(instance->targets[0].player, instance->targets[0].card);
      gain_life(player, result);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x2373370; original cards/jumpstart.c:136. */
int card_chained_brute(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  does_not_untap(player, card, event);
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    untap_card(instance->parent_controller, instance->parent_card);
  }
  result = generic_activated_ability(player, card, event, 0x50200, 1, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2373430; original cards/jumpstart.c:153. */
int card_corsair_captain(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    generate_token_by_id(player, card, 0x4998);
  }
  boost_subtype(player, card, event, SUBTYPE_PIRATE, 1, 1, 0, 0, BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x2373780; original cards/jumpstart.c:208. */
int card_immolating_gyre(int player, int card, event_t event)
{
  int result;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      result = count_graveyard_by_type(player, 0x38);
      new_damage_all(player, card, 1 - player, result, 0x21, NULL);
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2374220; original cards/jumpstart.c:467. */
int card_lightning_core_excavator(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      damage_target0(player, card, 3);
    }
  }
  result =
      generic_activated_ability(player, card, event, 0xd, 5, 0, 0, 0, 0, 0, 0, &td, "TARGET_ANY");
  return result;
}

/* ManalinkEh 2022: 0x2374350; original cards/jumpstart.c:488. */
int card_living_lightning(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    result = count_graveyard_by_type(player, 0x38);
    if (result != 0)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        new_default_test_definition(&this_test, 0x38, "Select an instant or sorcery card.");
        new_global_tutor(player, player, 2, 1, 0, 1, &this_test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2374990; original cards/jumpstart.c:599. */
int card_nocturnal_feeder(int player, int card, event_t event)
{
  int result;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  lose_life(1 - player, 2);
  gain_life(player, 2);
  return 0;
}

/* ManalinkEh 2022: 0x2374fe0; original cards/jumpstart.c:720. */
int card_release_the_dogs(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if (card != affected_card)
      {
        return 0;
      }
      if (player != affected_card_controller)
      {
        return 0;
      }
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      default_token_definition(player, card, 0x494b, &token);
      token.color_forced = ' ';
      token.qty = '\x04';
      generate_token(&token);
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2375170; original cards/jumpstart.c:758. */
int card_sethron_hurloon_general(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0xff);
    test.subtype = 0x77;
    test.type_flag = 3;
    test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    generate_token_by_id(player, card, 0x4964);
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    result = get_cost_mod_for_activated_abilities(player, card, 2, 1, 0, 0, 0, 0);
    result = has_mana_hybrid(player, 1, 1, 4, result);
    if (result != 0)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x800, 0, 0, 0, 0, 0, 0,
                                         0, NULL, NULL);
      return result;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = get_cost_mod_for_activated_abilities(player, card, 2, 1, 0, 0, 0, 0);
    charge_mana_hybrid(player, card, 1, 1, 4, result);
    if (spell_fizzled != 1)
    {
      result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x800, 0, 0, 0, 0, 0, 0, 0,
                                         NULL, NULL);
      return result;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    pump_subtype_until_eot(player, card, player, 0x77, 1, 0, 0, 0x10004);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23759d0; original cards/jumpstart.c:1039. */
int card_tinybones_trinket_thief(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  result = eot_trigger(player, card, event);
  if ((result != 0) && (result = get_trap_condition(1 - player, 0x203), result != 0))
  {
    draw_a_card(player);
    lose_life(player, 1);
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) && (hand_count[1 - player] == 0))
    {
      lose_life(1 - player, 10);
    }
  }
  result = generic_activated_ability(player, card, event, 0, 4, 2, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2375df0; original cards/jumpstart.c:1118. */
int card_witch_of_the_moors(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  deathtouch(player, card, event);
  result = eot_trigger(player, card, event);
  if ((result != 0) && (current_turn == player))
  {
    result = get_trap_condition(player, 1);
    if (result != 0)
    {
      new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
      new_sacrifice(player, card, 1 - player, SAC_CAUSED | SAC_NO_CANCEL, &test);
      result = any_in_graveyard_by_type(player, 2);
      if (result != 0)
      {
        new_global_tutor(player, player, 2, 1, 1, 1, &test);
      }
    }
  }
  return 0;
}
