#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int unique_name(int unused1, int unused2, int player, int card);

/* ManalinkEh 2022: 0x25bb240; original cards/ravnica_allegiance.c:3. */
static int biomancers_familiar_legacy(int player, int card, event_t event)
{
  int result;
  result = eot_trigger(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x25bc420; original cards/ravnica_allegiance.c:198. */
int card_archway_angel(int player, int card, event_t event)
{
  int result;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  result = count_subtype(player, -1, 0x500c);
  gain_life(player, result * 2);
  return 0;
}

/* ManalinkEh 2022: 0x25bc490; original cards/ravnica_allegiance.c:241. */
int card_azorius_knight_arbiter(int player, int card, event_t event)
{
  vigilance(player, card, event);
  unblockable(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x25bc4d0; original cards/ravnica_allegiance.c:278. */
int card_basilica_bell_haunt(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  discard(1 - player, 0, player);
  gain_life(player, 3);
  return 0;
}

/* ManalinkEh 2022: 0x25bc550; original cards/ravnica_allegiance.c:316. */
int card_bedevil(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

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
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 0x142);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025bc5b6;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x142);
  get_card_instance(player, card);
LAB_025bc5b6:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target artifact, creature or planeswalker.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25bb290; original cards/ravnica_allegiance.c:827. */
static int effect_cry_of_the_carnarium(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_CAST_SPELL)
  {
    result = is_what(affected_card_controller, affected_card, 2);
    if (result != 0)
    {
      exile_if_would_be_put_into_graveyard(player, card, affected_card_controller, affected_card,
                                           1);
    }
  }
  result = eot_trigger(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x25be5f0; original cards/ravnica_allegiance.c:1321. */
int card_emergency_powers(int player, int card, event_t event)
{
  int result;
  unsigned int player_00;
  test_definition_t test;
  result = current_turn;
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
      ai_modifier = ai_modifier + (hand_count[1 - player] - hand_count[player]) * 0xc + 0x18;
    }
    else if (event != EVENT_CAN_CHANGE_TARGET)
    {
      if (event == EVENT_RESOLVE_SPELL)
      {
        reshuffle_hand_into_deck(current_turn, 1);
        reshuffle_grave_into_deck(result, 0);
        draw_cards(result, 7);
        player_00 = current_turn ^ 1;
        reshuffle_hand_into_deck(player_00, 1);
        reshuffle_grave_into_deck(player_00, 0);
        draw_cards(player_00, 7);
        if ((current_turn == player) && ((current_phase == 0x14 || (current_phase == 0x1e))))
        {
          new_default_test_definition(
              &test, 0x47, "Select a permanent card with converted mana cost 7 or less.");
          test.cmc = 8;
          test.cmc_flag = 3;
          new_global_tutor(player, player, 4, 4, 0, 2, &test);
        }
        kill_card(player, card, KILL_EXILE);
      }
      else if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25be7a0; original cards/ravnica_allegiance.c:1356. */
int card_end_raze_forerunners(int player, int card, event_t event)
{
  test_definition_t test;
  vigilance(player, card, event);
  haste(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_test_definition(&test, 2);
  test.not_me = 1;
  pump_creatures_until_eot(player, card, player, 1, 2, 2, 0x80, 8, &test);
  return 0;
}

/* ManalinkEh 2022: 0x25beba0; original cards/ravnica_allegiance.c:1426. */
int card_ethereal_absolution(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int whose_graveyard;
  token_generation_t token;
  test_definition_t test;
  boost_creature_type(player, card, event, ~SUBTYPE_INVALID, 1, 1, 0, BCT_CONTROLLER_ONLY);
  boost_creature_type(player, card, event, ~SUBTYPE_INVALID, -1, -1, 0, BCT_OPPONENT_ONLY);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_test_definition(&test, 0xff);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar1 = generic_activated_ability_with_target_in_grave(player, card, EVENT_CAN_ACTIVATE,
                                                             GAA_NONE, 2, 1, 0, 0, 0, 1, 0, NULL,
                                                             NULL, TYPE_ANY, &test, 0x800);
      return iVar1;
    }
    if (event == EVENT_ACTIVATE)
    {
      if (player != 1)
      {
        iVar1 = generic_activated_ability_with_target_in_grave(player, card, EVENT_ACTIVATE,
                                                               GAA_NONE, 2, 1, 0, 0, 0, 1, 0, NULL,
                                                               NULL, TYPE_ANY, &test, 0x800);
        return iVar1;
      }
      generic_activated_ability_with_target_in_grave(1, card, EVENT_ACTIVATE, GAA_NONE, 2, 1, 0, 0,
                                                     0, 1, 0, NULL, NULL, TYPE_CREATURE, NULL,
                                                     0x800);
      if (spell_fizzled != 1)
      {
        return 0;
      }
      spell_fizzled = 0;
      iVar1 = generic_activated_ability_with_target_in_grave(1, card, EVENT_ACTIVATE, GAA_NONE, 2,
                                                             1, 0, 0, 0, 1, 0, NULL, NULL, TYPE_ANY,
                                                             &test, 0x800);
      return iVar1;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      iVar1 = global_enchantment(player, card, event);
      return iVar1;
    }
    default_test_definition(&test, 0xff);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    whose_graveyard = 1 - player;
    iVar1 = validate_target_from_grave_source(player, card, whose_graveyard, 1);
    if (iVar1 != -1)
    {
      iVar2 = is_what(-1, graveyard_ptr_mutable[whose_graveyard][iVar1], 2);
      rfg_card_from_grave(whose_graveyard, iVar1);
      if (iVar2 != 0)
      {
        default_token_definition(player, card, 0x498d, &token);
        token.color_forced = '\"';
        token.key_plus = 0x20;
        generate_token(&token);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25befb0; original cards/ravnica_allegiance.c:1541. */
int card_footlight_fiend(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_ANY");
      if (result != 0)
      {
        damage_target_creature_or_player(instance->targets[0].player, instance->targets[0].card, 1,
                                         player, card);
      }
    }
  }
  result = hybrid(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25bf410; original cards/ravnica_allegiance.c:1662. */
int card_gates_ablaze(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&test, 2);
    result = count_subtype(player, 0x47, 0x500c);
    new_damage_all(player, card, 2, result, 1, &test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25bf500; original cards/ravnica_allegiance.c:1744. */
int card_growth_spiral(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
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
      draw_a_card(player);
      new_default_test_definition(&test, 1, "Select a land card.");
      new_global_tutor(player, player, 4, 4, 0, 1, &test);
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

/* ManalinkEh 2022: 0x25bb4e0; original cards/ravnica_allegiance.c:1815. */
static int unique_name(int unused1, int unused2, int player, int card)
{
  int result;
  result = get_card_name(player, card);
  result = count_permanents_by_name(player, TYPE_CREATURE, result);
  return (unsigned int)(result == 1);
}

/* ManalinkEh 2022: 0x25bf780; original cards/ravnica_allegiance.c:1820. */
int card_guardian_project(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 2);
    test.type_flag = 3;
    test.special_selection_function = unique_name;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    draw_a_card(player);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25bf840; original cards/ravnica_allegiance.c:1841. */
int card_gutterbones(int player, int card, event_t event)
{
  int result;
  comes_into_play_tapped(player, card, event);
  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = has_mana_multi(player, 1, 1, 0, 0, 0, 0);
    if (((result != 0) && (current_turn == player)) &&
        (result = get_trap_condition(1 - player, 2), 0 < result))
    {
      return 4;
    }
  }
  else if (event == EVENT_PAY_FLASHBACK_COSTS)
  {
    charge_mana_multi(player, 1, 1, 0, 0, 0, 0);
    return (unsigned int)(spell_fizzled != 1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25bf950; original cards/ravnica_allegiance.c:1867. */
int card_gyre_engineer(int player, int card, event_t event)
{
  int result;
  result = mana_producing_creature_multi(player, card, event, 0, 0, 0, 1, 1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25c0740; original cards/ravnica_allegiance.c:2282. */
int card_kayas_wrath(int player, int card, event_t event)
{
  int result;
  int amount;
  unsigned int t_player;
  test_definition_t test;
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
      default_test_definition(&test, 2);
      result = current_turn;
      amount = new_manipulate_all(player, card, current_turn, &test, ACT_KILL_DESTROY);
      if (player != result)
      {
        amount = 0;
      }
      t_player = current_turn ^ 1;
      result = new_manipulate_all(player, card, t_player, &test, ACT_KILL_DESTROY);
      if (player == t_player)
      {
        amount = amount + result;
      }
      gain_life(player, amount);
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

/* ManalinkEh 2022: 0x25c1410; original cards/ravnica_allegiance.c:2665. */
int card_precognitive_perception(int player, int card, event_t event)
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
      if ((current_turn == player) && ((current_phase == 0x14 || (current_phase == 0x1e))))
      {
        scry(player, 3);
      }
      draw_cards(player, 3);
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

/* ManalinkEh 2022: 0x25c1bd0; original cards/ravnica_allegiance.c:2853. */
int card_rakdos_firewheeler(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td_opp;
  target_definition_t td_creature_or_planeswalker;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td_opp, 0);
  td_opp.allowed_controller = 1 - player;
  td_opp.zone = 0x1000;
  result = can_target(&td_opp);
  if (result != 0)
  {
    result = pick_target(&td_opp, "TARGET_OPPONENT");
    if (result != 0)
    {
      instance = get_card_instance(player, card);
      default_target_definition(player, card, &td_creature_or_planeswalker, 0x102);
      result = can_target(&td_creature_or_planeswalker);
      if ((result == 0) || (result = new_pick_target(&td_creature_or_planeswalker,
                                                     "TARGET_CREATURE_OR_PLANESWALKER", 1, 0),
                            result == 0))
      {
        damage_target0(player, card, 2);
      }
      else
      {
        damage_target0(player, card, 2);
        damage_creature(instance->targets[1].player, instance->targets[1].card, 2, player, card);
      }
      instance->number_of_targets = '\0';
      return 0;
    }
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c2df0; original cards/ravnica_allegiance.c:3395. */
int card_shimmer_of_possibility(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = 0x18;
      if (1 - player == current_turn)
      {
        result = ((0x1d < current_phase) - 1 & 0xffffffe8) + 0x30;
      }
      ai_modifier = ai_modifier + result;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    impulse_effect(player, 4, 3);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25c34e0; original cards/ravnica_allegiance.c:3575. */
int card_smothering_tithe(int player, int card, event_t event)
{
  int result;
  result = card_drawn_trigger(player, card, event, 1 - player, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    charge_mana(1 - player, COLOR_ANY, 2);
    if (spell_fizzled == 1)
    {
      generate_token_by_id(player, card, 0x4998);
    }
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x25c37a0; original cards/ravnica_allegiance.c:3665. */
int card_sphinxs_insight(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    draw_cards(player, 2);
    if ((current_turn == player) && ((current_phase == 0x14 || (current_phase == 0x1e))))
    {
      gain_life(player, 2);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25c4680; original cards/ravnica_allegiance.c:4280. */
int card_wilderness_reclamation(int player, int card, event_t event)
{
  int result;
  if (current_turn == player)
  {
    result = eot_trigger(player, card, event);
    if (result != 0)
    {
      manipulate_type(player, card, player, TYPE_LAND, ACT_UNTAP);
    }
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}
