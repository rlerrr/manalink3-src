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
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  vigilance_2022(player, card, event);
  unblockable(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x25bc4d0; original cards/ravnica_allegiance.c:278. */
int card_basilica_bell_haunt(int player, int card, event_t event)
{
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  vigilance_2022(player, card, event);
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
      gain_life_2022(player, amount);
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
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
      gain_life_2022(player, 2);
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

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static void afterlife(int player, int card, event_t event, int qty);

/* ManalinkEh 2022: 0x25bbd20; original cards/ravnica_allegiance.c:59. */
static void afterlife(int player, int card, event_t event, int qty)
{
  int result;
  token_generation_t token;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return;
  }
  default_token_definition(player, card, 0x498d, &token);
  token.color_forced = '\"';
  token.no_sleight = '\x01';
  token.key_plus = 0x20;
  token.qty = (unsigned char)qty;
  generate_token(&token);
  return;
}

/* ManalinkEh 2022: 0x25bd690; original cards/ravnica_allegiance.c:891. */
int card_debtors_transport(int player, int card, event_t event)
{
  afterlife(player, card, event, 2);
  return 0;
}

/* ManalinkEh 2022: 0x25be390; original cards/ravnica_allegiance.c:1274. */
int card_electrodominance(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  char buffer[75];
  target_definition_t td;
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
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 0x102);
        td.zone = 0x1200;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            damage_target0(player, card, instance->info_slot);
            scnprintf(buffer, 0x4b, " Choose a card with converted mana cost %d or less to cast.",
                      instance->info_slot);
            new_default_test_definition(&test, 1, buffer);
            test.type_flag = 1;
            test.cmc = instance->info_slot + 1;
            test.cmc_flag = 3;
            test.zone = 0x100;
            if (((0 < hand_count[player]) &&
                 (result = check_battlefield_for_special_card(player, card, player, 0, &test),
                  result != 0)) &&
                (result = new_select_a_card(player, player, 4, 0, 1, -1, &test), -1 < result))
            {
              play_card_in_hand_for_free(player, result);
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025be40d;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
  get_card_instance(player, card);
LAB_025be40d:
  result = generic_spell(player, card, event, 5, &td, "TARGET_ANY", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25bfc00; original cards/ravnica_allegiance.c:1947. */
int card_hydroid_krasis(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    generic_spell(player, card, EVENT_CAST_SPELL, 4, NULL, NULL, 0, NULL);
    enters_the_battlefield_with_counters(player, card, EVENT_CAST_SPELL, COUNTER_P1_P1,
                                         instance->info_slot);
    result = is_token(player, card);
    if (result == 0)
    {
      result = check_special_flags_2022(player, card, 0x40000);
      if (result == 0)
      {
        uVar3 = round_down_value(instance->info_slot);
        gain_life(player, uVar3);
        uVar3 = round_down_value(instance->info_slot);
        draw_cards(player, uVar3);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25bfd20; original cards/ravnica_allegiance.c:1996. */
int card_imperious_oligarch(int player, int card, event_t event)
{
  vigilance_2022(player, card, event);
  afterlife(player, card, event, 1);
  return 0;
}

/* ManalinkEh 2022: 0x25c1300; original cards/ravnica_allegiance.c:2579. */
int card_orzhov_enforcer(int player, int card, event_t event)
{
  deathtouch_2022(player, card, event);
  afterlife(player, card, event, 1);
  return 0;
}

/* ManalinkEh 2022: 0x25c18c0; original cards/ravnica_allegiance.c:2748. */
int card_prime_speaker_vannifar(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int result;
  unsigned int player_00;
  test_definition_t test;
  test_definition_t test2;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
    test.not_me = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x40a11, 0, 0, 0, 0, 0,
                                         0, 0, NULL, NULL);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      uVar2 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                            &test);
      if (uVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      add_state(player, card, 0x10);
      player_00 = (int)uVar2 >> 0x10 & 0xff;
      result = get_cmc(player_00, uVar2 >> 0x18);
      instance->info_slot = result;
      if (result < 6)
      {
        ai_modifier = ai_modifier + result * 8;
      }
      kill_card(player_00, uVar2 >> 0x18, KILL_SACRIFICE);
      tapped_for_mana_color = -1;
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
    test.not_me = 1;
    instance = get_card_instance(player, card);
  }
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  result = instance->info_slot;
  default_test_definition(&test2, 2);
  test2.cmc = result + 1;
  scnprintf(test2.message, 100, "Select a creature card with CMC %d.", test2.cmc);
  new_global_tutor(player, player, 1, 4, 0, 1, &test2);
  return 0;
}

/* ManalinkEh 2022: 0x25c39a0; original cards/ravnica_allegiance.c:3805. */
int card_syndicate_messenger(int player, int card, event_t event)
{
  afterlife(player, card, event, 1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25c1340; original cards/ravnica_allegiance.c:2639. */
int card_plaza_of_harmony(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = count_subtype(player, -1, 0x500c);
    if (1 < result)
    {
      gain_life(player, 3);
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {

    result = get_id(player, card);
    uVar2 = get_color_of_mana_produced_by_id(result, 0x3e, player);
    play_land_sound_effect_force_color(player, card, uVar2 & (int)(uVar2 ^ 0xffffffff) >> 0x1f);
    return 0;
  }
  result = mana_producer(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25bb1c0; original cards/ravnica_allegiance.c:9. */
static int find_bf_legacy_effect(int target_player_id, int target_card)
{
  card_instance_t *instance;
  int card;

  if (0 < active_cards_count[target_player_id])
  {
    card = 0;
    do
    {
      while ((((instance = in_play(target_player_id, card),
                instance == NULL || (instance->internal_card_id != LEGACY_EFFECT_CUSTOM)) ||
               (instance->info_slot_fn != biomancers_familiar_legacy)) ||
              (instance->damage_target_player != target_player_id)))
      {
        card = card + 1;
        if (active_cards_count[target_player_id] <= card)
        {
          return -1;
        }
      }
      if (instance->damage_target_card == target_card)
      {
        return card;
      }
      card = card + 1;
    } while (card < active_cards_count[target_player_id]);
  }
  return -1;
}

/* ManalinkEh 2022: 0x25bc930; original cards/ravnica_allegiance.c:460. */
int card_captive_audience(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  unsigned int uVar4;
  unsigned int uVar5;
  token_generation_t token;

  instance = get_card_instance(player, card);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (instance->info_slot < 7)
  {
    upkeep_trigger_ability(player, card, event, player);
  }
  if (event == EVENT_ETB)
  {
    give_control_of_self(player, card);
    instance->info_slot = 0;
    cant_be_responded_to = 1;
  }
  else if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {

    uVar3 = instance->info_slot;
    if (uVar3 == 0xffffffff)
    {
      instance->info_slot = 0;
      uVar3 = 1;
      uVar5 = 1;
      uVar4 = 1;
    }
    else
    {
      uVar4 = (unsigned int)((uVar3 & 4) == 0);
      uVar5 = (unsigned int)((uVar3 & 2) == 0);
      uVar3 = (unsigned int)((uVar3 & 1) == 0);
    }
    result =
        dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\003", "\001\014",
                    "Your life total becomes 4", uVar3, 0xfffffff6, "Discard your hand", uVar5,
                    0xfffffffb, "Opponent creates five Zombies", uVar4, 0xfffffff1, 0, 0, 0, 0);
    if (result == 1)
    {
      set_life_total(player, 4);
      instance->info_slot = instance->info_slot | 1;
    }
    else if (result == 2)
    {
      new_discard_all(player, player);
      instance->info_slot = instance->info_slot | 2;
    }
    else if (result == 3)
    {
      default_token_definition(player, card, 0x49ac, &token);
      token.t_player = 1 - player;
      token.qty = '\x05';
      generate_token(&token);
      instance->info_slot = instance->info_slot | 4;
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25bf230; original cards/ravnica_allegiance.c:1623. */
int card_gate_colossus(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_MODIFY_COST)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if ((((event == EVENT_BLOCK_LEGALITY) && (result == 0)) &&
         (attacking_card_controller == player)) &&
        (attacking_card == card))
    {
      result = get_power(affected_card_controller, affected_card);
      if (result < 3)
      {
        event_result = 1;
      }
    }
    return 0;
  }
  result = count_subtype(player, -1, 0x500c);
  COST_COLORLESS = COST_COLORLESS - (char)result;
  is_humiliated_for_ability(player, card, AL_STATIC);
  return 0;
}

/* ManalinkEh 2022: 0x25bf300; original cards/ravnica_allegiance.c:1645. */
int card_gatebreaker_ram(int player, int card, event_t event)
{
  int result;

  if (event - EVENT_POWER < 2)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        result = count_subtype(player, 0x47, 0x500c);
        event_result = event_result + result;
      }
    }
  }
  else if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      result = count_subtype(player, 0x47, 0x500c);
      if (1 < result)
      {
        event_result = event_result | 0x80;
        vigilance_2022(player, card, EVENT_ABILITIES);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25bfa70; original cards/ravnica_allegiance.c:1914. */
int card_high_alert(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_ABILITIES)
  {
    if (affected_card_controller == player)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        instance = get_card_instance(affected_card_controller, affected_card);
        instance->token_status = instance->token_status | 0x800;
      }
    }
  }
  else if ((((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
            (event == EVENT_RESOLVE_ACTIVATION)) ||
           (event == EVENT_CLEANUP))
  {
    default_target_definition(player, card, &td, 2);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        result = instance->targets[0].card;
        instance = get_card_instance(player, card);
        untap_card(instance->targets[0].player, result);
      }
    }
    result = generic_activated_ability(player, card, event, 4, 2, 0, 1, 0, 0, 1, 0, &td,
                                       "TARGET_CREATURE");
    return result;
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25c1280; original cards/ravnica_allegiance.c:2548. */
int card_nikya_of_the_old_ways(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if ((result == 0) && (affected_card_controller == player))
    {
      result = is_what(player, affected_card, 3);
      if (result == 0)
      {
        infinite_casting_cost();
      }
    }
  }
  result = card_mana_flare(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25c1d30; original cards/ravnica_allegiance.c:2908. */
int card_rakdos_the_showstopper(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  unsigned int player_00;
  int iVar3;
  int *piVar4;
  int *piVar5;
  unsigned int player_01;
  unsigned int uVar6;
  int count[2];
  int marked[2][150];

  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    count[0] = 0;
    count[1] = 0;
    uVar6 = 0;
    player_00 = current_turn;
    do
    {
      player_01 = uVar6 ^ player_00;

      iVar3 = active_cards_count[player_01] + -1;
      if (-1 < iVar3)
      {
        do
        {
          instance = in_play(player_01, iVar3);
          if ((((instance != NULL) && (iVar2 = is_what(player_01, iVar3, 2), iVar2 != 0)) &&
               (iVar2 = has_subtype(player_01, iVar3, SUBTYPE_DEMON), iVar2 == 0)) &&
              ((iVar2 = has_subtype(player_01, iVar3, SUBTYPE_DEVIL),
                iVar2 == 0 && (iVar2 = has_subtype(player_01, iVar3, SUBTYPE_IMP), iVar2 == 0))))
          {
            add_status(player_01, iVar3, 0x20000);
            iVar2 = flip_a_coin(player, card);
            if (iVar2 != 0)
            {
              iVar2 = count[player_01];
              count[player_01] = iVar2 + 1;
              marked[player_01][iVar2] = iVar3;
            }
            remove_status(player_01, iVar3, 0x20000);
          }
          iVar3 = iVar3 + -1;
          player_00 = current_turn;
        } while (iVar3 != -1);
      }
      uVar6 = uVar6 + 1;
    } while (uVar6 != 2);

    iVar3 = count[player_00];

    uVar6 = player_00;
    if (0 < iVar3)
    {
      piVar5 = marked[player_00];
      do
      {
        piVar4 = piVar5 + 1;
        kill_card(player_00, *piVar5, KILL_DESTROY);
        piVar5 = piVar4;
        uVar6 = current_turn;
      } while (piVar4 != marked[player_00] + iVar3);
    }
    uVar6 = uVar6 ^ 1;
    iVar3 = count[uVar6];
    if (0 < iVar3)
    {
      piVar5 = marked[uVar6];
      do
      {
        piVar4 = piVar5 + 1;
        kill_card(uVar6, *piVar5, KILL_DESTROY);
        piVar5 = piVar4;
      } while (piVar4 != marked[uVar6] + iVar3);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c46f0; original cards/ravnica_allegiance.c:4293. */
int card_windstorm_drake(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (((event == EVENT_POWER) && (affected_card_controller == player)) && (affected_card != card))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      instance = in_play(affected_card_controller, affected_card);
      if (instance != NULL)
      {
        result = is_humiliated_for_ability(player, card, AL_STATIC);
        if (result == 0)
        {
          result = check_for_ability(affected_card_controller, affected_card, 0x20);
          if (result != 0)
          {
            event_result = event_result + 1;
          }
        }
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int judith_the_scourge_diva_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25bb520; original cards/ravnica_allegiance.c:2153. */
static int judith_the_scourge_diva_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  card_instance_t *pcVar3;
  int result;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x4cd0))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, 0x4700), result != 0))
    {
      return 0;
    }
  LAB_025bb558:
    uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto LAB_025bb57a;
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, event), result != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_025bb558;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);

    if ((pcVar3 != NULL) && (pcVar3 = get_card_instance(affected_card_controller, affected_card),
                             pcVar3->kill_code != '\0'))
    {

      if ((-1 < pcVar1->damage_target_player) &&
          ((pcVar1->damage_target_card == affected_card &&
            (pcVar1->damage_target_player == affected_card_controller))))
      {
        pcVar1->damage_target_card = -1;
        pcVar1->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        if (pcVar1->targets[0xb].player < 0)
        {
          pcVar1->targets[0xb].player = 0;
        }
        *(uint8_t *)((int)&pcVar1->targets[0xb].player + 1) = 1;
      }
      default_test_definition(&recovered_stack_test.test, 2);
      recovered_stack_test.test.type_flag = 3;
      count_for_gfp_ability_and_store_values(player, card, 0x77, player, 0,
                                             &recovered_stack_test.test, 0x40, 0);
    }
    uVar2 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {
    default_target_definition(player, card, &recovered_stack_test.target, 0x102);
    recovered_stack_test.test.message[0xc] = '\0';
    recovered_stack_test.test.message[0xd] = '\x12';
    recovered_stack_test.test.message[0xe] = '\0';
    recovered_stack_test.test.message[0xf] = '\0';
    recovered_stack_test.test.message[0x44] = '\0';
    recovered_stack_test.test.message[0x45] = '\0';
    recovered_stack_test.test.message[0x46] = '\0';
    recovered_stack_test.test.message[0x47] = '\0';

    for (uVar2 = uVar2 & 0xff; uVar2 != 0; uVar2 = uVar2 - 1)
    {
      result = can_target(&recovered_stack_test.target);
      if ((result != 0) &&
          (result = pick_target(&recovered_stack_test.target, "TARGET_ANY"), result != 0))
      {
        damage_target0(player, card, 1);
      }
      pcVar1 = get_card_instance(player, card);
      pcVar1->number_of_targets = '\0';
    }
    kill_card(player, card, KILL_EXILE);
  }
LAB_025bb57a:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c04f0; original cards/ravnica_allegiance.c:2217. */
int card_judith_the_scourge_diva(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  unsigned int uVar4;
  target_definition_t td;

  if (((event == EVENT_STATIC_EFFECTS) && (card == affected_card)) &&
      (player == affected_card_controller))
  {

    if (0 < active_cards_count[player])
    {
      iVar1 = 0;
      do
      {
        instance = in_play(player, iVar1);

        if ((((instance != NULL) && (iVar2 = is_what(player, iVar1, 0x80), iVar2 != 0)) &&
             (instance = get_card_instance(player, iVar1),
              instance->info_slot_fn == judith_the_scourge_diva_effect)) &&
            (((instance->damage_target_player == player &&
               (instance->damage_target_card == card)) ||
              ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
          goto LAB_025c050c;
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player]);
    }

    iVar1 =
        create_targetted_legacy_effect(player, card, judith_the_scourge_diva_effect, player, card);
    instance = get_card_instance(player, iVar1);
    instance->targets[0].player = player;
    instance->targets[0].card = card;
    instance->number_of_targets = '\x01';
    add_status(player, iVar1, 0x10000);
  }
LAB_025c050c:
  boost_creature_type(player, card, event, ~SUBTYPE_INVALID, 1, 0, 0, BCT_CONTROLLER_ONLY);
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
  td.allow_cancel = 0;
  count_for_gfp_ability_and_store_values(player, card, event, player, 2, NULL, 0x40, 0);
  iVar1 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (iVar1 != 0)
  {

    instance = get_card_instance(player, card);
    for (uVar4 = (unsigned int)(unsigned char)instance->targets[0xb].card; uVar4 != 0;
         uVar4 = uVar4 - 1)
    {
      iVar1 = can_target(&td);
      if ((iVar1 != 0) && (iVar1 = pick_target(&td, "TARGET_ANY"), iVar1 != 0))
      {
        damage_target0(player, card, 1);
      }
      instance->number_of_targets = '\0';
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c28d0; original cards/ravnica_allegiance.c:3325. */
int card_seraph_of_the_scales(int player, int card, event_t event)
{
  card_instance_t *instance;
  uint32_t uVar2;
  unsigned int uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iStack_1c;

  afterlife(player, card, event, 2);
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 0, 1, 0,
                                      NULL, NULL);
    if (iVar4 != 0)
    {
      return 1;
    }
    iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 1, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    return (unsigned int)(iVar4 != 0);
  }
  if (event != EVENT_ACTIVATE)
  {
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      return 0;
    }

    instance = get_card_instance(player, card);
    if (instance->info_slot == 1)
    {
      iVar6 = 8;
      iVar5 = 1;
      iVar4 = 0;
    }
    else
    {
      if (instance->info_slot != 2)
      {
        return 0;
      }
      iVar6 = 0x200;
      iVar5 = 0;
      iVar4 = 1;
    }
    iVar4 = generic_shade(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0, iVar4, 0, 0, 0, iVar5, 0, 0,
                          0, iVar6);
    return iVar4;
  }

  iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 0, 1, 0, NULL,
                                    NULL);
  iVar5 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 1, 0, 0, 0, 0, 0, NULL,
                                    NULL);
  instance = get_card_instance(player, card);
  uVar3 = 0;
  if (-1 < instance->targets[1].card)
  {
    instance = get_card_instance(player, card);
    uVar3 = instance->targets[1].card;
  }
  iStack_1c = 1;
  if (current_phase < 0x15)
  {
    iStack_1c = 1;
    iVar6 = check_for_special_ability(player, card, 8);
    if (iVar6 == 0)
    {
      iStack_1c = (-(unsigned int)((uVar3 & 8) == 0) & 9) + 1;
    }
  }
  if (((current_phase == 0x18) && (iVar6 = is_unblocked(player, card), iVar6 == 0)) &&
      (iVar6 = check_for_special_ability(player, card, 0x200), (uVar3 & 0x200) == 0 && iVar6 == 0))
  {
    uVar2 = 10;
  }
  else
  {
    uVar2 = 1;
  }
  iVar4 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "Vigilance", iVar4,
                      iStack_1c, "Deathtouch", iVar5, uVar2, 0, 0, 0, 0);
  if (iVar4 != 0)
  {
    if (iVar4 == 1)
    {
      iVar5 = 1;
      iVar4 = 0;
    }
    else
    {
      if (iVar4 != 2)
      {
        return 0;
      }
      iVar5 = 0;
      iVar4 = 1;
    }
    iVar4 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 0, iVar4, 0, 0, 0, iVar5, 0,
                                      NULL, NULL);
    return iVar4;
  }
  spell_fizzled = 1;
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int dovin_grand_arbiter_legacy(int player, int card, event_t event);
void riot(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25bc240; original cards/ravnica_allegiance.c:71. */
void riot(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\003", "\001\014",
                         "+1/+1 counter", 1, 5, "Haste", 1, 5, 0, 0, 0, 0);
    if (result == 1)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    }
    else if (result == 2)
    {

      result = pump_ability_until_eot_no_repeat_2022(player, card, player, card, 0, -0x7ffffffc);
      if (result != -1)
      {
        instance = get_card_instance(player, result);
        instance->token_status = instance->token_status | 0x10000;
      }
      instance = get_card_instance(player, card);
      result = is_humiliated_for_ability(player, card, AL_KEY);
      if (result == 0)
      {
        instance->state = instance->state & 0xfffdffff;
      }
    }
  }
  return;
}

/* ManalinkEh 2022: 0x25bb340; original cards/ravnica_allegiance.c:1117. */
int dovin_grand_arbiter_legacy(int player, int card, event_t event)
{
  int card_00;
  int player_00;
  card_instance_t *instance;
  int result;

  instance = combat_damage_being_dealt(event);
  if ((((instance != NULL) && (instance->damage_source_player == player)) &&
       ((instance->targets[3].player & 2) != 0)) &&
      ((instance->damage_target_card == -1 &&
        (result = check_special_flags_2022(player, instance->damage_source_card, 0x40), result == 0))))
  {
    instance = get_card_instance(player, card);
    if (instance->targets[0x10].player < 0)
    {
      instance = get_card_instance(player, card);
      instance->targets[0x10].player = 0;
    }
    instance = get_card_instance(player, card);
    instance->targets[0x10].player = instance->targets[0x10].player + 1;
  }
  if (((trigger_condition == 0xd7) && (card == affected_card)) &&
      ((player == affected_card_controller && (reason_for_trigger_controller == player))))
  {

    instance = get_card_instance(player, card);
    result = instance->targets[0x10].player;
    if (result < 1)
    {
      return 0;
    }
    if (event == EVENT_TRIGGER)
    {
      event_result = event_result | 2;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      card_00 = instance->targets[0].card;
      player_00 = instance->targets[0].player;
      add_counters_from_source(player_00, card_00, player_00, card_00, COUNTER_LOYALTY, result);
      instance->targets[0x10].player = 0;
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

/* ManalinkEh 2022: 0x25bdea0; original cards/ravnica_allegiance.c:1147. */
int card_dovin_grand_arbiter(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  bool bVar5;
  int iVar6;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  bVar5 = event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE;
  if ((event == EVENT_RESOLVE_ACTIVATION) || (bVar5))
  {

    instance = get_card_instance(player, card);
    iVar1 = count_counters(player, card, COUNTER_LOYALTY);
    iVar4 = ((7 < iVar1) - 1 & 0xffffffed) + 0x14;
    iVar1 = iVar4;
    iVar3 = count_counters(player, card, COUNTER_LOYALTY);
    iVar1 = dialog_impl(player, card, event, "\001\002", "\001\017",
                        "Add counters for combat damage", 1, 5, 1, "Create a thopter & gain life",
                        1, ((1 < iVar3) - 1 & 0xfffffff2) + 0xf, 0xffffffff, "Dig ten cards deep",
                        1, iVar4, 0xfffffff9, 0, 0, 0, 0, iVar1);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (iVar1 == 0)
      {
        return 0;
      }
    }
    else if (bVar5)
    {
      instance->number_of_targets = '\0';
    }
    else if (iVar1 == 2)
    {
      generate_token_by_id(player, card, 0x4995);
      gain_life_2022(player, 1);
    }
    else if (iVar1 == 3)
    {

      iVar1 = count_deck(player);
      if (iVar1 != 0)
      {
        if (10 < iVar1)
        {
          iVar1 = 10;
        }

        iVar3 = iVar1;
        if (3 < iVar1)
        {
          iVar3 = 3;
        }
        do
        {
          iVar6 = iVar3;
          iVar4 = iVar1;
          new_default_test_definition(&this_test, 0xff, "Select a card to add to your hand");
          this_test.create_minideck = (unsigned short)iVar4;
          this_test.no_shuffle = 1;
          new_global_tutor(player, player, 1, 1, 1, 1, &this_test);
          iVar3 = iVar6 + -1;
          if (iVar3 == 0)
            break;
          iVar1 = iVar4 + -1;
        } while (iVar4 + -1 != 0);
        iVar4 = iVar4 - iVar6;
        if (0 < iVar4)
        {
          put_top_x_on_bottom_in_random_order(player, iVar4);
        }
      }
    }
    else if (iVar1 == 1)
    {
      create_my_legacy(instance->parent_controller, instance->parent_card,
                       dovin_grand_arbiter_legacy);
    }
  }
  iVar1 = planeswalker(player, card, event, 3);
  return iVar1;
}

/* ManalinkEh 2022: 0x25be870; original cards/ravnica_allegiance.c:1383. */
int card_essence_capture(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  target_definition_t td1;

  if (event == EVENT_CAN_CAST)
  {
    counterspell_target_definition(player, card, &td, 2);
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = player;
    get_card_instance(player, card);
    result = counterspell(player, card, EVENT_CAN_CAST, &td, 0);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      counterspell_target_definition(player, card, &td, 2);
      default_target_definition(player, card, &td1, 2);
      td1.allowed_controller = player;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        counterspell(player, card, EVENT_CAST_SPELL, &td, 0);
        result = can_target(&td1);
        if (result != 0)
        {
          new_pick_target(&td1, "TARGET_CREATURE_YOU_CONTROL", 1, 0);
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    counterspell_target_definition(player, card, &td, 2);
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = counterspell_validate(player, card, &td, 0);
      if (result != 0)
      {
        real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
      }
      result = validate_target(player, card, &td1, 1);
      if (result != 0)
      {
        add_counters_from_source(player, card, instance->targets[1].player,
                                 instance->targets[1].card, COUNTER_P1_P1, 1);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    counterspell_target_definition(player, card, &td, 2);
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = player;
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25bf4c0; original cards/ravnica_allegiance.c:1694. */
int card_ghor_clan_wrecker(int player, int card, event_t event)
{
  menace(player, card, event);
  riot(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x25bf710; original cards/ravnica_allegiance.c:1798. */
int card_gruul_spellbreaker(int player, int card, event_t event)
{
  riot(player, card, event);
  if (current_turn != player)
  {
    return 0;
  }
  hexproof_2022(player, card, event);
  *(unsigned int *)(&player_bits + player * 4) =
      *(unsigned int *)(&player_bits + player * 4) | 0x20000;
  return 0;
}

/* ManalinkEh 2022: 0x25c25a0; original cards/ravnica_allegiance.c:3133. */
int card_rhythm_of_the_wild(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;

  type_uncounterable(player, card, event, player, 2, NULL);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 2);
    test.type_flag = 3;
    result = new_specific_cip(player, card, event, player, 2, &test);
    if (result != 0)
    {

      result = dialog_impl(trigger_cause_controller, trigger_cause, EVENT_ACTIVATE, "\001\002",
                           "\001\003", "+1/+1 counter", 1, 5, "Haste", 1, 5, 0, 0, 0, 0);
      if (result == 1)
      {
        add_counters_from_source(trigger_cause_controller, trigger_cause, trigger_cause_controller,
                                 trigger_cause, COUNTER_P1_P1, 1);
      }
      else if (result == 2)
      {

        instance = get_card_instance(trigger_cause_controller, trigger_cause);
        pump_ability_until_eot_no_repeat_2022(player, card, trigger_cause_controller, trigger_cause, 0,
                                         -0x7ffffffc);
        result = is_humiliated_for_ability(trigger_cause_controller, trigger_cause, AL_KEY);
        if (result == 0)
        {
          instance->state = instance->state & 0xfffdffff;
        }
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25c2eb0; original cards/ravnica_allegiance.c:3421. */
int card_simic_ascendancy(int player, int card, event_t event)
{
  xtrigger_t xVar1;
  int result;
  card_instance_t *instance;
  target_definition_t td;

  upkeep_trigger_ability(player, card, event, player);
  enable_xtrigger_flags = enable_xtrigger_flags | ENABLE_XTRIGGER_1_1_COUNTERS;
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    result = count_counters(player, card, COUNTER_GROWTH);
    if (0x13 < result)
    {
      lose_the_game_2022(1 - player);
    }
  }
  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if ((((xVar1 == XTRIGGER_1_1_COUNTERS) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      ((trigger_cause_controller == player && (reason_for_trigger_controller == player))))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result != 0)
      goto LAB_025c2f0e;
    if (event != EVENT_TRIGGER)
    {
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        add_counters_from_source(player, card, player, card, COUNTER_GROWTH, counters_added);
        goto LAB_025c304b;
      }
      goto LAB_025c2f0e;
    }
    event_result = event_result | 2;
  }
  else
  {
  LAB_025c2f0e:
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      goto LAB_025c2f26;
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
  LAB_025c304b:
    result = global_enchantment(player, card, event);
    return result;
  }
LAB_025c2f26:
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.allow_cancel = 0;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    result = instance->targets[0].card;
    instance = get_card_instance(player, card);
    add_counters_from_source(player, card, instance->targets[0].player, result, COUNTER_P1_P1, 1);
  }
  result = generic_activated_ability(player, card, event, 4, 1, 0, 1, 1, 0, 0, 0, &td,
                                     "TARGET_CREATURE_YOU_CONTROL");
  return result;
}

/* ManalinkEh 2022: 0x25c3100; original cards/ravnica_allegiance.c:3467. */
int card_skarrgan_hellkite(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  riot(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = count_counters(player, card, COUNTER_P1_P1);
      if (result == 0)
      {
        return 0;
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 3, 0, 0, 0, 1, 0, 0,
                                         &td, "TARGET_ANY");
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 3, 0, 0, 0, 1, 0, 0, &td,
                                "TARGET_ANY");
      if (spell_fizzled == 1)
      {
        return 0;
      }
      new_pick_target(&td, "TARGET_ANY", 1, 1);
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x102);
  }
  td.zone = 0x1200;
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  divide_damage(player, card, &td);
  return 0;
}

/* ManalinkEh 2022: 0x25c40d0; original cards/ravnica_allegiance.c:4054. */
int card_unbreakable_formation(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  bool bVar4;
  bool bVar5;
  bool bVar6;

  if (event == EVENT_RESOLVE_SPELL)
  {

    bVar4 = current_turn != player;
    bVar5 = current_phase != 0x14;
    bVar6 = current_phase != 0x1e;
    if (0 < active_cards_count[player])
    {
      iVar1 = 0;
    LAB_025c41aa:
      do
      {
        instance = in_play(player, iVar1);
        if ((instance != NULL) && (iVar3 = is_what(player, iVar1, 2), iVar3 != 0))
        {
          if (bVar4 || bVar6 && bVar5)
          {
            iVar3 = iVar1 + 1;
            pump_ability_until_eot_2022(player, card, player, iVar1, 0, 0, 0, 0x800);
            iVar1 = iVar3;
            if (active_cards_count[player] <= iVar3)
              break;
            goto LAB_025c41aa;
          }
          pump_ability_until_eot_2022(player, card, player, iVar1, 0, 0, 0, 0x808);
          add_counters_from_source(player, card, player, iVar1, COUNTER_P1_P1, 1);
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player]);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x25c4900; original cards/ravnica_allegiance.c:4343. */
int card_zhur_taa_goblin(int player, int card, event_t event)
{
  riot(player, card, event);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int vindictive_vampire_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25bf090; original cards/ravnica_allegiance.c:1579. */
int card_frilled_mystic(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  const char *pcVar3;
  char buffer[100];

  result = duh_mode(player);
  etb_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
  {
    result = card_counterspell(player, card, EVENT_CAN_CAST);
    if (result == 0)
    {
      instance->info_slot = 0;
      return 1;
    }
    instance->info_slot = 1;
    result = 99;
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if (((card == affected_card) && (player == affected_card_controller)) &&
          (current_turn == player))
      {
        ai_modifier = ai_modifier + -10;
      }
    }
    else if (event == EVENT_ETB)
    {
      result = get_id(card_on_stack_controller, card_on_stack);
      pcVar3 = get_card_name_by_id(result);
      scnprintf(buffer, 100, " Counter %s\n Pass", pcVar3);
      if (instance->info_slot == 1)
      {
        result = do_dialog(player, player, card, card_on_stack_controller, card_on_stack, buffer,
                           (unsigned int)(card_on_stack_controller == player));
        if (result == 0)
        {
          real_counter_a_spell(player, card, card_on_stack_controller, card_on_stack);
        }
      }
    }
    result = 0;
  }
  return result;
}

/* ManalinkEh 2022: 0x25c14e0; original cards/ravnica_allegiance.c:2684. */
int card_priest_of_forgotten_gods(int player, int card, event_t event)
{
  target_t *ptVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  test_definition_t *ptStack_144;
  target_definition_t *ptStack_140;
  target_definition_t td;
  test_definition_t test;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    td.allow_cancel = 3;
    new_default_test_definition(&test, 2, "Select another creature to sacrifice.");
    test.not_me = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      test.qty = 2;
      iVar4 = new_can_sacrifice_as_cost(player, card, &test);
      if (iVar4 == 0)
      {
        return 0;
      }
      iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                        &td, NULL);
      return (unsigned int)(iVar4 != 0);
    }
    if (event == EVENT_ACTIVATE)
    {
      instance->number_of_targets = '\0';
      iVar4 = new_sacrifice(player, card, player, SAC_AS_COST, &test);
      if (iVar4 == 0)
      {
        return 0;
      }
      iVar4 = new_sacrifice(player, card, player, SAC_AS_COST | SAC_NO_CANCEL, &test);
      if (iVar4 == 0)
      {
        return 0;
      }
      instance->number_of_targets = '\0';
      while (true)
      {
        iVar4 = can_target(&td);
        if (iVar4 == 0)
        {
          return 0;
        }
        iVar4 = new_pick_target(&td, "TARGET_PLAYER", -1, 0);
        if (iVar4 == 0)
        {
          if (instance->targets[instance->number_of_targets].card != -1)
          {
            return 0;
          }
          spell_fizzled = 1;
          return 0;
        }
        if (td.allowed_controller != 2)
        {
          return 0;
        }
        iVar4 = instance->targets[0].player;
        if (((ai_is_speculating == 1) ||
             ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))) &&
            (1 - player == iVar4))
          break;
        td.allowed_controller = 1 - iVar4;
        td.preferred_controller = td.allowed_controller;
      }
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    td.allow_cancel = 3;
    new_default_test_definition(&test, 2, "Select another creature to sacrifice.");
    test.not_me = 1;
    instance = get_card_instance(player, card);
  }
  ptStack_140 = &td;
  ptStack_144 = &test;
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }

  iVar4 = 0;
  if (instance->number_of_targets != '\0')
  {
    do
    {
      while (iVar3 = validate_target(player, card, ptStack_140, iVar4), iVar3 != 0)
      {
        new_sacrifice(player, card, instance->targets[iVar4].player, SAC_AS_COST | SAC_NO_CANCEL,
                      ptStack_144);
        ptVar1 = instance->targets + iVar4;
        iVar4 = iVar4 + 1;
        lose_life_2022(ptVar1->player, 2);
        if ((int)(unsigned int)instance->number_of_targets <= iVar4)
          goto LAB_025c161a;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)(unsigned int)instance->number_of_targets);
  }
LAB_025c161a:
  produce_mana(player, 1, 2);
  draw_a_card(player);
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

/* ManalinkEh 2022: 0x25bb9c0; original cards/ravnica_allegiance.c:4098. */
static int vindictive_vampire_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  unsigned int uVar4;
  test_definition_t this_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x4d4b))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, 0x4700), result != 0))
    {
      return 0;
    }
  LAB_025bbb00:
    uVar4 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 == 0)
      goto joined_r0x025bbb25;
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, event), result != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_025bbb00;

    pcVar1 = get_card_instance(player, card);
    pcVar2 = in_play(affected_card_controller, affected_card);
    if ((pcVar2 != NULL) &&
        (result = is_this_dying(affected_card_controller, affected_card), result != 0))
    {

      if ((pcVar1->damage_target_card == affected_card) &&
          (pcVar1->damage_target_player == affected_card_controller))
      {
        pcVar1->damage_target_card = -1;
        pcVar1->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        if (pcVar1->targets[0xb].player < 0)
        {
          pcVar1->targets[0xb].player = 0;
        }
        *(uint8_t *)((int)&pcVar1->targets[0xb].player + 2) = 1;
        return 0;
      }
      default_test_definition(&this_test, 2);
      this_test.not_me = 1;
      count_for_gfp_ability(player, card, 0x77, player, 0, &this_test);
    }
    uVar4 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {

    for (uVar4 = uVar4 & 0xff; uVar4 != 0; uVar4 = uVar4 - 1)
    {
      damage_player(1 - player, 1, player, card);
      gain_life(player, 1);
    }
    kill_card(player, card, KILL_EXILE);
  }
joined_r0x025bbb25:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c4240; original cards/ravnica_allegiance.c:4152. */
int card_vindictive_vampire(int player, int card, event_t event)
{
  unsigned int uVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  test_definition_t this_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      iVar4 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          instance = in_play(player, iVar4);

          if ((((instance != NULL) && (iVar2 = is_what(player, iVar4, 0x80), iVar2 != 0)) &&
               (instance = get_card_instance(player, iVar4),
                instance->info_slot_fn == vindictive_vampire_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_025c4265;
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[player]);
      }

      iVar4 = create_targetted_legacy_effect(player, card, vindictive_vampire_effect, player, card);
      instance = get_card_instance(player, iVar4);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar4, 0x10000);
    }
  }
  else if ((event == EVENT_GRAVEYARD_FROM_PLAY) &&
           (instance = in_play(player, card), instance != NULL))
  {
    default_test_definition(&this_test, 2);
    this_test.not_me = 1;
    count_for_gfp_ability(player, card, 0x77, player, 0, &this_test);
  }
LAB_025c4265:
  uVar1 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);

  if (((uVar1 != 0) && (instance = get_card_instance(player, card), instance->kill_code == '\0')) &&
      (uVar1 = uVar1 & 0xff, uVar1 != 0))
  {
    do
    {
      damage_player(1 - player, 1, player, card);
      gain_life(player, 1);
      uVar1 = uVar1 - 1;
    } while (uVar1 != 0);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25bcce0; original cards/ravnica_allegiance.c:594. */
int card_cindervines(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&recovered_stack_td.test, 2);
    sp_ability_trigger(1 - player, event, 2, TYPE_NONE, &recovered_stack_td.test, player, card);
  }
  if (event == EVENT_SPA)
  {
    damage_player(trigger_cause_controller, 1, player, card);
  }
  else if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
           ((event == EVENT_RESOLVE_ACTIVATION || (event == EVENT_CLEANUP))))
  {
    default_target_definition(player, card, &recovered_stack_td.target, 0x44);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&recovered_stack_td.target);
      if (result != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        damage_player(instance->targets[0].player, 2, player, card);
      }
      ai_modifier = ai_modifier + -0xc;
    }
    result = generic_activated_ability(player, card, event, 0xc, 1, 0, 0, 0, 0, 0, 0,
                                       &recovered_stack_td.target, "DISENCHANT");
    return result;
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25bf9b0; original cards/ravnica_allegiance.c:1893. */
int card_hero_of_precinct_one(int player, int card, event_t event)
{
  test_definition_t test;

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0xff);
    test.color_flag = 2;
    sp_ability_trigger(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event != EVENT_SPA)
  {
    return 0;
  }
  generate_token_by_id(player, card, 0x494c);
  return 0;
}

/* ManalinkEh 2022: 0x25c0870; original cards/ravnica_allegiance.c:2331. */
int card_lavinia_azorius_renegade(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    iVar1 = affected_card_controller;
    if (affected_card_controller == -1)
    {
      iVar1 = event_result;
    }
    if ((1 - player == iVar1) &&
        (iVar1 = is_what(affected_card_controller, affected_card, 2), iVar1 == 0))
    {
      iVar1 = get_cmc(affected_card_controller, affected_card);
      iVar2 = count_permanents_by_type(affected_card_controller, TYPE_LAND);
      if ((iVar2 < iVar1) &&
          (iVar1 = is_humiliated_for_ability(player, card, AL_STATIC), iVar1 == 0))
      {
        infinite_casting_cost();
        return 0;
      }
    }
    if (trigger_condition != 0xd3)
    {
      return 0;
    }
  }
  else if (trigger_condition != 0xd3)
    goto LAB_025c08a6;
  if (((card == affected_card) && (player == affected_card_controller)) &&
      (iVar1 = played_for_free(trigger_cause_controller, trigger_cause), iVar1 != 0))
  {
    sp_ability_trigger(1 - player, event, 2, TYPE_ANY, NULL, player, card);
  }
LAB_025c08a6:
  if (event != EVENT_SPA)
  {
    return 0;
  }
  real_counter_a_spell(player, card, trigger_cause_controller, trigger_cause);
  return 0;
}

/* ManalinkEh 2022: 0x25bbcd0; original cards/ravnica_allegiance.c:23. */
/* Concrete compiler clone: only EAX/EDX are ABI arguments. */
static int can_adapt_without_fx(int player, int card)
{
  int result;
  result = count_counters(player, card, COUNTER_P1_P1);
  if (result < 1)
  {
    return 1;
  }
  result = find_bf_legacy_effect(player, card);
  return (unsigned int)(result != -1);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int adapt(int player, int card, event_t event, int number, int cl, int b, int u, int g, int r,
          int w);

/* ManalinkEh 2022: 0x25bc030; original cards/ravnica_allegiance.c:34. */
int adapt(int player, int card, event_t event, int number, int cl, int b, int u, int g, int r,
          int w)
{
  int target_card;
  int iVar1;
  card_instance_t *instance;
  int card_00;
  int iVar3;

  if ((event == EVENT_CAN_ACTIVATE) && (player == 1))
  {
    iVar1 = can_adapt_without_fx(1, card);
    if ((iVar1 == 0) || (instance = has_activation_on_stack(1, card), instance != NULL))
    {
      ai_modifier = ai_modifier + -99;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {

    instance = get_card_instance(player, card);
    iVar1 = instance->parent_controller;
    instance = get_card_instance(player, card);
    target_card = instance->parent_card;
    card_00 = find_bf_legacy_effect(iVar1, target_card);
    iVar3 = count_counters(iVar1, target_card, COUNTER_P1_P1);
    if (((card_00 != -1) || (iVar3 < 1)) &&
        (add_counters_from_source(iVar1, target_card, iVar1, target_card, COUNTER_P1_P1, number),
         card_00 != -1))
    {
      kill_card(iVar1, card_00, KILL_EXILE);
    }
  }
  else if (((event - EVENT_POW_BOOST < 2) &&
            (iVar1 = can_adapt_without_fx(player, card), iVar1 != 0)) &&
           (iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, cl, b, u, g, r,
                                              w, 0, NULL, NULL),
            iVar1 != 0))
  {
    return number;
  }
  iVar1 = generic_activated_ability(player, card, event, 0, cl, b, u, g, r, w, 0, NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x25bc3c0; original cards/ravnica_allegiance.c:154. */
int card_aeromunculus(int player, int card, event_t event)
{
  int result;

  result = adapt(player, card, event, 1, 2, 0, 1, 1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25bf5f0; original cards/ravnica_allegiance.c:1764. */
int card_growth_chamber_guardian(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  if (((event == EVENT_COUNTERS) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&this_test, 2);
    this_test.id = get_card_name(player, card);
    result = get_card_name(player, card);
    scnprintf(this_test.message, 100, "Select a card named %s.", cards_ptr[result]->name);
    new_global_tutor(player, player, 1, 1, 0, 5, &this_test);
  }
  result = adapt(player, card, event, 2, 2, 0, 0, 1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25c1b40; original cards/ravnica_allegiance.c:2815. */
int card_pteramander(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_ACTIVATE) || (event - EVENT_RESOLVE_ACTIVATION < 2)) ||
      (result = 0, event == EVENT_CLEANUP))
  {
    result = count_graveyard_by_type(player, 0x38);
    result = adapt(player, card, event, 4, 7 - result, 0, 1, 0, 0, 0);
  }
  return result;
}

/* ManalinkEh 2022: 0x25c2870; original cards/ravnica_allegiance.c:3250. */
int card_sauroform_hybrid(int player, int card, event_t event)
{
  int result;

  result = adapt(player, card, event, 4, 4, 0, 0, 2, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25c3480; original cards/ravnica_allegiance.c:3541. */
int card_skitter_eel(int player, int card, event_t event)
{
  int result;

  result = adapt(player, card, event, 2, 2, 0, 1, 0, 0, 0);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25bb8b0; original cards/ravnica_allegiance.c:2999. */
static int can_counter_activated_or_triggered_ability(int player, int card, event_t event)
{
  int result;

  result = can_counter_activated_ability(player, card, event, NULL);
  if (result == 0)
  {
    result = can_counter_triggered_ability(player, card, event, NULL);
    if (result == 0)
    {
      return 0;
    }
  }
  return 99;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25bcf00; original cards/ravnica_allegiance.c:717. */
int card_consecrate_consume(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  int iVar3;
  int iVar4;
  card_instance_t *pcVar5;
  int32_t player_00;
  int card_00;
  int iStack_26c;
  target_definition_t td_player;
  target_definition_t td;
  test_definition_t test;
  test_definition_t this_test;

  new_default_test_definition(&test, 0xff, "Select target card to exile.");
  default_target_definition(player, card, &td_player, 0);
  td_player.zone = 0x1000;
  if (event == EVENT_PLAY_FIRST_HALF)
  {
    iVar3 = generic_spell(player, card, EVENT_CAST_SPELL, 0x1000, NULL, NULL, 1, &test);
    return iVar3;
  }
  if (event == EVENT_PLAY_SECOND_HALF)
  {
    new_pick_target(&td_player, "TARGET_PLAYER", 0, 0);
    return 0;
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_025bcfd4;
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        pcVar1 = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          uVar2 = pcVar1->info_slot;
          if ((uVar2 & 1) != 0)
          {

            iVar3 = validate_target_from_grave_source(player, card, pcVar1->targets[0].player, 1);
            if (iVar3 != -1)
            {
              rfg_card_from_grave(pcVar1->targets[0].player, iVar3);
              draw_cards(player, 1);
            }
            uVar2 = pcVar1->info_slot;
          }
          if (((uVar2 & 2) != 0) && (iVar3 = can_sacrifice(player, pcVar1->targets[0].player, 1,
                                                           TYPE_CREATURE, SUBTYPE_INVALID),
                                     iVar3 != 0))
          {

            default_test_definition(&this_test, 2);
            iVar4 = check_battlefield_for_special_card(player, card, pcVar1->targets[0].player,
                                                       0x80, &this_test);
            player_00 = pcVar1->targets[0].player;
            iStack_26c = 0;
            iVar3 = active_cards_count[player_00];
            if (0 < iVar3)
            {
              card_00 = 0;
              do
              {
                pcVar5 = in_play(player_00, card_00);
                if ((pcVar5 != NULL) &&
                    (iVar3 = is_what(pcVar1->targets[0].player, card_00, 2), iVar3 != 0))
                {
                  iVar3 = get_power(pcVar1->targets[0].player, card_00);
                  if (iVar3 == iVar4)
                  {
                    iStack_26c = iStack_26c + 1;
                  }
                  else
                  {
                    state_untargettable(pcVar1->targets[0].player, card_00, 1);
                  }
                }
                player_00 = pcVar1->targets[0].player;
                card_00 = card_00 + 1;
                iVar3 = active_cards_count[player_00];
              } while (card_00 < iVar3);
              if (1 < iStack_26c)
              {
                default_target_definition(player, card, &td, 2);
                td.who_chooses = pcVar1->targets[0].player;
                td.allow_cancel = 0;
                td.allowed_controller = td.who_chooses;
                iVar3 = new_pick_target(&td, "Select a creature to sacrifice.", 0, 0x80);
                if (iVar3 != 0)
                {
                  add_state(pcVar1->targets[0].player, pcVar1->targets[0].card, 0x200000);
                }
                player_00 = pcVar1->targets[0].player;
                iVar3 = active_cards_count[player_00];
              }
            }
            iVar3 = iVar3 + -1;
            if (-1 < iVar3)
            {
              while (true)
              {
                pcVar5 = in_play(player_00, iVar3);
                if ((pcVar5 != NULL) &&
                    (iVar4 = is_what(pcVar1->targets[0].player, iVar3, 2), iVar4 != 0))
                {
                  iVar4 = check_state(pcVar1->targets[0].player, iVar3, 0x100000);
                  if (iVar4 == 0)
                  {
                    if ((iStack_26c == 1) ||
                        ((1 < iStack_26c &&
                          (iVar4 = check_state(pcVar1->targets[0].player, iVar3, 0x200000),
                           iVar4 != 0))))
                    {

                      iVar4 = get_power(pcVar1->targets[0].player, iVar3);
                      kill_card(pcVar1->targets[0].player, iVar3, KILL_SACRIFICE);
                      gain_life_2022(player, iVar4);
                    }
                  }
                  else
                  {
                    state_untargettable(pcVar1->targets[0].player, iVar3, 0);
                  }
                }
                iVar3 = iVar3 + -1;
                if (iVar3 == -1)
                  break;
                player_00 = pcVar1->targets[0].player;
              }
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025bcfd4;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_025bcfd4;
    }
  }
  get_card_instance(player, card);
LAB_025bcfd4:
  iVar3 = can_sorcery_be_played(player, event);
  uVar2 = 0;
  if (iVar3 != 0)
  {
    iVar3 = can_target(&td_player);
    uVar2 = (unsigned int)(iVar3 != 0);
  }
  iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 0x1000, NULL, NULL, 1, &test);
  iVar3 = hybrid_split_card(player, card, event, 0x4c82, iVar3, 4, 0x4c85, uVar2, 8);
  return iVar3;
}

/* ManalinkEh 2022: 0x25bd6b0; original cards/ravnica_allegiance.c:926. */
int card_depose_deploy(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  int result;
  card_instance_t *pcVar4;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  if (event == EVENT_PLAY_FIRST_HALF)
  {
    result = generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL);
    return result;
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_025bd749;
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        pcVar1 = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          uVar2 = pcVar1->info_slot;
          if ((uVar2 & 1) != 0)
          {
            result = valid_target(&td);
            if (result != 0)
            {

              pcVar4 = get_card_instance(player, card);
              if ((player == 1) &&
                  (pcVar4 = get_card_instance(pcVar4->targets[0].player, pcVar4->targets[0].card),
                   (pcVar4->state & 0x10) != 0))
              {
                ai_modifier = ai_modifier + -0x30;
              }
              tap_card(pcVar1->targets[0].player, pcVar1->targets[0].card);
              draw_a_card(player);
            }
            uVar2 = pcVar1->info_slot;
          }
          if ((uVar2 & 2) != 0)
          {
            generate_tokens_by_id(player, card, 0x4995, 2);
            result = count_permanents_by_type(player, TYPE_CREATURE);
            gain_life_2022(player, result);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025bd749;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_025bd749;
    }
  }
  get_card_instance(player, card);
LAB_025bd749:
  result = can_target(&td);
  result = hybrid_split_card(player, card, event, 0x4c8e, result, 3, 0x4c8d, 1, 8);
  return result;
}

/* ManalinkEh 2022: 0x25c3aa0; original cards/ravnica_allegiance.c:3890. */
int card_thrash_threat(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  int amt;
  token_generation_t token;
  target_definition_t td_control;
  target_definition_t td_dont_control;

  default_target_definition(player, card, &td_control, 2);
  td_control.preferred_controller = player;
  td_control.allowed_controller = player;
  default_target_definition(player, card, &td_dont_control, 0x102);
  td_dont_control.allowed_controller = 1 - player;
  td_dont_control.preferred_controller = td_dont_control.allowed_controller;
  if (event == EVENT_PLAY_FIRST_HALF)
  {

    instance = get_card_instance(player, card);
    iVar2 = pick_target(&td_control, "ASHNODS_BATTLEGEAR");
    if ((iVar2 == 0) ||
        (iVar2 = pick_next_target_noload(
             &td_dont_control, "Select target creature or planeswalker you don\'t control."),
         iVar2 == 0))
    {
      instance->number_of_targets = '\0';
    }
    goto LAB_025c3b86;
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_025c3b86;
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          uVar4 = instance->info_slot;
          if ((uVar4 & 1) != 0)
          {
            iVar2 = valid_target(&td_control);
            if ((iVar2 != 0) &&
                (iVar2 = validate_target(player, card, &td_dont_control, 1), iVar2 != 0))
            {
              iVar2 = instance->targets[0].card;
              iVar3 = instance->targets[0].player;
              amt = get_power(iVar3, iVar2);
              damage_creature(instance->targets[1].player, instance->targets[1].card, amt, iVar3,
                              iVar2);
            }
            uVar4 = instance->info_slot;
          }
          if ((uVar4 & 2) != 0)
          {
            default_token_definition(player, card, 0x4907, &token);
            token.color_forced = '\x18';
            token.tou = 4;
            token.pow = 4;
            token.key_plus = 0x80;
            generate_token(&token);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025c3b86;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_025c3b86;
    }
  }
  get_card_instance(player, card);
LAB_025c3b86:
  iVar2 = can_sorcery_be_played(player, event);
  iVar3 = can_target(&td_control);
  uVar4 = 0;
  if (iVar3 != 0)
  {
    iVar3 = can_target(&td_dont_control);
    uVar4 = (unsigned int)(iVar3 != 0);
  }
  iVar2 = hybrid_split_card(player, card, event, 0x4d3e, uVar4, 5, 0x4d40, iVar2, 8);
  return iVar2;
}

/* ManalinkEh 2022: 0x25c4470; original cards/ravnica_allegiance.c:4231. */
int card_warrant_warden(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int result;
  int can_play_first_half;
  token_generation_t token;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  td.required_state = 0x20;
  if (event == EVENT_PLAY_FIRST_HALF)
  {
    generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_ATTACKING_BLOCKING_CREATURE", 1,
                  NULL);
    goto LAB_025c4527;
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_025c4527;
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          uVar2 = instance->info_slot;
          if ((uVar2 & 1) != 0)
          {
            result = valid_target(&td);
            if (result != 0)
            {
              put_on_top_of_deck(instance->targets[0].player, instance->targets[0].card);
            }
            uVar2 = instance->info_slot;
          }
          if ((uVar2 & 2) != 0)
          {
            default_token_definition(player, card, 0x498a, &token);
            token.color_forced = '$';
            token.key_plus = 0x20;
            token.s_key_plus = 8;
            generate_token(&token);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025c4527;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_025c4527;
    }
  }
  get_card_instance(player, card);
LAB_025c4527:
  result = can_sorcery_be_played(player, event);
  can_play_first_half = can_target(&td);
  result =
      hybrid_split_card(player, card, event, 0x4d4f, can_play_first_half, 5, 0x4d4e, result, 8);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25bc6a0; original cards/ravnica_allegiance.c:366. */
int card_biomancers_familiar(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&recovered_stack_td.test, 2);
    set_cost_mod_for_activated_abilities(player, -1, 2, &recovered_stack_td.test);
    leaves_play_2022(player, card, EVENT_RESOLVE_SPELL, 0, 0);
  LAB_025bc6ef:
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      goto LAB_025bc73e;
  }
  else
  {
    if (event != EVENT_CAST_SPELL)
    {
      leaves_play_2022(player, card, event, 0, 0);
      if (event == EVENT_LEAVES_PLAY_ABILITY)
      {
        default_test_definition(&recovered_stack_td.test, 2);
        remove_cost_mod_for_activated_abilities(player, -1, 2, &recovered_stack_td.test);
        return 0;
      }
      goto LAB_025bc6ef;
    }
    if ((affected_card_controller == player) &&
        (result = is_what(player, affected_card, 2), result != 0))
    {
      set_cost_mod_for_activated_abilities(affected_card_controller, affected_card, 2, NULL);
    }
    leaves_play_2022(player, card, EVENT_CAST_SPELL, 0, 0);
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
LAB_025bc73e:
  default_target_definition(player, card, &recovered_stack_td.target, 2);
  recovered_stack_td.target.preferred_controller = player;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    result = instance->targets[0].card;
    instance = get_card_instance(player, card);
    create_targetted_legacy_effect(player, card, biomancers_familiar_legacy,
                                   instance->targets[0].player, result);
  }
  result = generic_activated_ability(player, card, event, 5, 0, 0, 0, 0, 0, 0, 0,
                                     &recovered_stack_td.target, "TARGET_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x25bd8e0; original cards/ravnica_allegiance.c:969. */
int card_deputy_of_detention(int player, int card, event_t event)
{
  int iVar1;
  unsigned int *puVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int iVar5;
  card_instance_t *pcVar6;
  int iVar7;
  int iVar8;
  unsigned int player_00;
  unsigned int uVar9;
  int (*paiVar10)[151];
  unsigned char bVar11;
  unsigned int uStack_52c;
  int leg;
  int idx;
  target_definition_t td;
  int marked[2][151];

  bVar11 = 0;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    leaves_play_2022(player, card, event, 1, 0);
    if (event == EVENT_LEAVES_PLAY_ABILITY)
    {

      td.who_chooses = 0;
      marked[0][0] = 0;
      while (puVar2 = (unsigned int *)exiledby_find_any(player, card, &td.who_chooses, marked[0]),
             puVar2 != NULL)
      {

        uVar9 = *puVar2;
        *puVar2 = 0xffffffff;
        player_00 = uVar9 >> 0x1f;
        iVar4 = (uVar9 & 0x7fffffff) + uVar9 * 8;
        iVar1 = check_rfg(player_00,
                          (unsigned int)*(unsigned short *)(cards_data[0].cc + iVar4 * 8 + -7));
        if (iVar1 != 0)
        {

          iVar1 = add_card_to_hand(player_00, uVar9 & 0x7fffffff);
          remove_card_from_rfg(
              player_00, (unsigned int)*(unsigned short *)(cards_data[0].cc + iVar4 * 8 + -7));
          put_into_play(player_00, iVar1);
        }
      }
    }
    return 0;
  }

  default_target_definition(player, card, &td, 0x47);
  iVar1 = 1 - player;
  td.illegal_type = 1;
  td.allow_cancel = 0;
  td.allowed_controller = iVar1;
  pcVar3 = get_card_instance(player, card);
  iVar4 = can_target(&td);
  if ((iVar4 != 0) &&
      (iVar4 = new_pick_target(&td, "Select a nonland permanent an opponent controls.", 0, 0x80),
       iVar4 != 0))
  {

    iVar5 = get_card_name(pcVar3->targets[0].player, pcVar3->targets[0].card);
    iVar4 = current_turn;
    paiVar10 = marked;
    for (iVar8 = 0x12e; iVar8 != 0; iVar8 = iVar8 + -1)
    {
      (*paiVar10)[0] = 0;
      paiVar10 = (int (*)[151])((int)paiVar10 + ((unsigned int)bVar11 * -2 + 1) * 4);
    }
    if (iVar5 == -1)
    {
      iVar4 = get_original_internal_card_id(pcVar3->targets[0].player, pcVar3->targets[0].card);
      uStack_52c = (unsigned int)cards_data[iVar4].id;
      exile_card_and_remember_it_on_exiledby(player, card, pcVar3->targets[0].player,
                                             pcVar3->targets[0].card);
    }
    else
    {

      leg = 0;
      idx = 0;
      iVar5 = active_cards_count[current_turn] + -1;
      if (iVar5 < 0)
      {
        uStack_52c = 0xffffffff;
      }
      else
      {

        uStack_52c = 0xffffffff;
        do
        {
          if ((((iVar1 == iVar4) && (pcVar6 = in_play(iVar1, iVar5), pcVar6 != NULL)) &&
               (iVar8 = is_what(iVar1, iVar5, 0x47), iVar8 != 0)) &&
              (iVar8 =
                   has_same_name(pcVar3->targets[0].player, pcVar3->targets[0].card, iVar1, iVar5),
               iVar8 != 0))
          {
            iVar8 = is_token(iVar1, iVar5);
            if (iVar8 == 0)
            {
              iVar8 = get_owner(iVar1, iVar5);
              iVar7 = get_original_internal_card_id(iVar1, iVar5);
              uStack_52c = (unsigned int)cards_data[iVar7].id;
              exiledby_remember(player, card, iVar8, iVar7, &leg, &idx);
            }
            marked[iVar4][iVar5] = 1;
          }
          iVar5 = iVar5 + -1;
        } while (iVar5 != -1);
      }
      iVar4 = current_turn;
      iVar1 = 1 - current_turn;
      iVar5 = active_cards_count[iVar1] + -1;
      if (-1 < iVar5)
      {
        do
        {
          if (((player == iVar4) && (pcVar6 = in_play(iVar1, iVar5), pcVar6 != NULL)) &&
              ((iVar8 = is_what(iVar1, iVar5, 0x47),
                iVar8 != 0 && (iVar8 = has_same_name(pcVar3->targets[0].player,
                                                     pcVar3->targets[0].card, iVar1, iVar5),
                               iVar8 != 0))))
          {
            iVar8 = is_token(iVar1, iVar5);
            if (iVar8 == 0)
            {
              iVar8 = get_owner(iVar1, iVar5);
              iVar7 = get_original_internal_card_id(iVar1, iVar5);
              uStack_52c = (unsigned int)cards_data[iVar7].id;
              exiledby_remember(player, card, iVar8, iVar7, &leg, &idx);
            }
            marked[iVar1][iVar5] = 1;
          }
          iVar5 = iVar5 + -1;
        } while (iVar5 != -1);
      }

      iVar4 = current_turn;
      iVar1 = 0x96;
      do
      {
        if (marked[iVar4][iVar1] != 0)
        {
          kill_card(iVar4, iVar1, KILL_EXILE);
        }
        iVar1 = iVar1 + -1;
      } while (iVar1 != -1);
      uVar9 = current_turn ^ 1;
      iVar4 = 0x96;
      do
      {
        if (marked[uVar9][iVar4] != 0)
        {
          kill_card(uVar9, iVar4, KILL_EXILE);
        }
        iVar4 = iVar4 + -1;
      } while (iVar4 != -1);
      if ((int)uStack_52c < 0)
        goto LAB_025bda90;
    }
    create_card_name_legacy(player, card, uStack_52c);
  }
LAB_025bda90:
  leaves_play_2022(player, card, EVENT_ETB, 1, 0);
  return 0;
}

/* ManalinkEh 2022: 0x25c39c0; original cards/ravnica_allegiance.c:3832. */
int card_teysa_karlov(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  if (event != EVENT_STATIC_EFFECTS)
  {
    if (event == EVENT_ABILITIES)
    {
      result = is_token(affected_card_controller, affected_card);
      if (result != 0)
      {
        result = is_what(affected_card_controller, affected_card, 2);
        if (result != 0)
        {
          vigilance_2022(affected_card_controller, affected_card, EVENT_ABILITIES);
          lifelink(affected_card_controller, affected_card, EVENT_ABILITIES);
        }
      }
    }
    return 0;
  }
  card_counts[player][5] = card_counts[player][5] + '\x01';
  return 0;
}

/* ManalinkEh 2022: 0x25c3e80; original cards/ravnica_allegiance.c:3991. */
int card_tithe_taker(int player, int card, event_t event)
{
  int result;
  card_data_t *pcVar2;
  test_definition_t this_test;

  if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_SKIP_TURN))
  {
    if (current_turn == player)
    {
      default_test_definition(&this_test, 0x47);
      set_cost_mod_for_activated_abilities(1 - player, -1, 5, &this_test);
      goto joined_r0x025c40bc;
    }
    if (event == EVENT_CAST_SPELL)
      goto LAB_025c3fdd;
  }
  else
  {
  joined_r0x025c40bc:
    if (event == EVENT_CAST_SPELL)
    {
      if (((current_turn == player) && (1 - player == affected_card_controller)) &&
          (result = is_what(1 - player, affected_card, 0x47), result != 0))
      {
        set_cost_mod_for_activated_abilities(affected_card_controller, affected_card, 5, NULL);
      }
    LAB_025c3fdd:
      leaves_play_2022(player, card, EVENT_CAST_SPELL, 0, 0);
      goto LAB_025c3ee4;
    }
  }
  leaves_play_2022(player, card, event, 0, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    if (current_turn != player)
      goto LAB_025c3ee4;
    result = 1 - player;
  LAB_025c3f1e:
    default_test_definition(&this_test, 0x47);
    remove_cost_mod_for_activated_abilities(result, -1, 3, &this_test);
  }
  else if (event == EVENT_CAN_SKIP_TURN)
  {
    result = current_turn;
    if (1 - player != current_turn)
      goto LAB_025c3ee4;
    goto LAB_025c3f1e;
  }

  if (((event == EVENT_MODIFY_COST_GLOBAL) &&
       (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
      ((1 - player == affected_card_controller &&
        ((current_turn == player &&
          (pcVar2 = get_card_data(1 - player, affected_card), (pcVar2->type & 1) == 0))))))
  {
    COST_COLORLESS = COST_COLORLESS + '\x01';
  }
LAB_025c3ee4:
  afterlife(player, card, event, 1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25bcb70; original cards/ravnica_allegiance.c:546. */
int card_cavalcade_of_calamity(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  int tgt_card;
  test_definition_t test;

  if (event == EVENT_DECLARE_ATTACKERS)
  {
    default_test_definition(&test, 2);
    test.power = 2;
    test.power_flag = 3;
    store_attackers(player, card, EVENT_DECLARE_ATTACKERS, DAT_TRACK, player, -1, &test);
    dat_ability(player, card, EVENT_DECLARE_ATTACKERS, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  }
  else
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {

      instance = get_card_instance(player, card);
      result = instance->info_slot + -1;
      if (-1 < result)
      {
        do
        {

          tgt_card = planeswalker_its_attacking(
              player, (unsigned int)*(unsigned char *)((int)&instance->targets[2].player + result));
          if (tgt_card != -2)
          {
            damage_creature(1 - current_turn, tgt_card, 1, player, card);
          }
          result = result + -1;
        } while (result != -1);
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25bfd60; original cards/ravnica_allegiance.c:2023. */
int card_incubation_incongruity(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int iVar3;
  int iVar4;
  target_definition_t td;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_this_test;

  default_target_definition(player, card, &td, 2);
  if (event == EVENT_PLAY_SECOND_HALF)
  {
    generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL);
    goto LAB_025bfe0e;
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_025bfe0e;
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        instance = get_card_instance(player, card);
        if (event != EVENT_RESOLVE_SPELL)
          goto LAB_025bfe0e;
        uVar2 = instance->info_slot;
        if ((uVar2 & 1) != 0)
        {
          iVar3 = count_deck(player);
          if (iVar3 != 0)
          {
            if (5 < iVar3)
            {
              iVar3 = 5;
            }
            new_default_test_definition(&recovered_stack_this_test.test, 2,
                                        "Select a creature card.");
            recovered_stack_this_test.test.create_minideck = (unsigned short)iVar3;
            recovered_stack_this_test.test.no_shuffle = 1;
            iVar4 = new_global_tutor(player, player, 1, 1, 0, 1, &recovered_stack_this_test.test);
            if ((ai_is_speculating == 1) ||
                ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
            {
              if (iVar4 < 0)
              {
                recovered_stack_this_test.test.type = 0xff;
                iVar4 =
                    new_global_tutor(player, player, 1, 1, 0, 1, &recovered_stack_this_test.test);
                goto LAB_025c0046;
              }
            LAB_025c007c:
              reveal_card(player, card, player, iVar4);
              iVar3 = iVar3 + -1;
            }
            else
            {
            LAB_025c0046:
              if (-1 < iVar4)
                goto LAB_025c007c;
            }
            if (0 < iVar3)
            {
              put_top_x_on_bottom_in_random_order(player, iVar3);
            }
          }
          uVar2 = instance->info_slot;
        }
        if (((uVar2 & 2) != 0) && (iVar3 = valid_target(&td), iVar3 != 0))
        {
          kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
          default_token_definition(player, card, 0x4933, &recovered_stack_this_test.token);
          recovered_stack_this_test.token.t_player = (int32_t)(instance->targets[0].player);
          recovered_stack_this_test.test.message[0x18] = '\x03';
          recovered_stack_this_test.test.message[0x19] = '\0';
          recovered_stack_this_test.test.message[0x1a] = '\0';
          recovered_stack_this_test.test.message[0x1b] = '\0';
          recovered_stack_this_test.test.message[0x1c] = '\x03';
          recovered_stack_this_test.test.message[0x1d] = '\0';
          recovered_stack_this_test.test.message[0x1e] = '\0';
          recovered_stack_this_test.test.message[0x1f] = '\0';
          generate_token(&recovered_stack_this_test.token);
        }
        kill_card(player, card, KILL_DESTROY);
        goto LAB_025bfe0e;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_025bfe0e;
    }
  }
  get_card_instance(player, card);
LAB_025bfe0e:
  iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_CREATURE", 1, NULL);
  iVar4 = can_sorcery_be_played(player, event);
  iVar3 = hybrid_split_card(player, card, event, 0x4ccd, iVar4, 5, 0x4ccc, iVar3, 10);
  return iVar3;
}

/* ManalinkEh 2022: 0x25c1150; original cards/ravnica_allegiance.c:2515. */
int card_mirror_march(int player, int card, event_t event)
{
  int result;
  char cVar2;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_test;

  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&recovered_stack_test.test, 2);
    recovered_stack_test.test.type_flag = 3;
    etb_ability_global(player, event, 2, TYPE_NONE, &recovered_stack_test.test, player, card);
  }
  if (event == EVENT_ETB)
  {
    cVar2 = '\0';
    do
    {
      result = flip_a_coin(player, card);
      if (result == 0)
        break;
      cVar2 = cVar2 + '\x01';
    } while (result == 1);
    copy_token_definition(player, card, &recovered_stack_test.token, trigger_cause_controller,
                          trigger_cause);
    recovered_stack_test.test.message[0x2a] = '\x01';
    recovered_stack_test.token.special_code_for_legacy = haste_and_remove_eot;
    recovered_stack_test.test.message[0x2b] = cVar2;
    generate_token(&recovered_stack_test.token);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25c1f60; original cards/ravnica_allegiance.c:3004. */
int card_repudiate_replicate(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  td.allowed_controller = player;
  td.preferred_controller = player;
  instance = get_card_instance(player, card);
  if (event == EVENT_PLAY_FIRST_HALF)
  {
    result = instance->targets[9].card;
    if (result == 1)
    {
      cast_counter_activated_ability(player, card, 0);
    }
    else if (result == 2)
    {
      cast_counter_triggered_ability(player, card, 0);
    }
  }
  else
  {
    if (event == EVENT_PLAY_SECOND_HALF)
    {
      result = generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                             "Select target creature you control.", 1, NULL);
      return result;
    }
    if (event == EVENT_CAN_CAST)
    {
      result = can_counter_activated_ability(player, card, EVENT_CAN_CAST, NULL);
      if (result != 0)
      {
        instance->targets[9].card = 1;
      }
      result = can_counter_triggered_ability(player, card, EVENT_CAN_CAST, NULL);
      if (result != 0)
      {
        instance->targets[9].card = 2;
      }
    }
    else if ((event != EVENT_CAST_SPELL) && (event == EVENT_RESOLVE_SPELL))
    {
      uVar3 = instance->info_slot;
      if ((uVar3 & 1) != 0)
      {
        result = instance->targets[9].card;
        if (result == 1)
        {
          resolve_counter_activated_ability(player, card, NULL, 0);
          uVar3 = instance->info_slot;
        }
        else if (result == 2)
        {
          resolve_counter_triggered_ability(player, card, NULL, 0);
          uVar3 = instance->info_slot;
        }
      }
      if (((uVar3 & 2) != 0) && (result = valid_target(&td), result != 0))
      {
        copy_token(player, card, player, instance->targets[0].card);
      }
      kill_card(player, card, KILL_DESTROY);
      result = can_target(&td);
      goto joined_r0x025c20f2;
    }
  }
  result = can_target(&td);
joined_r0x025c20f2:
  uVar3 = 0;
  if (result != 0)
  {
    result = can_sorcery_be_played(player, event);
    uVar3 = (unsigned int)(result != 0);
  }
  result = can_counter_activated_or_triggered_ability(player, card, event);
  result = hybrid_split_card(player, card, event, 0x4cfe, result, 10, 0x4cfd, uVar3, 8);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25c3720; original cards/ravnica_allegiance.c:3649. */
int card_sphinx_of_the_guildpact(int player, int card, event_t event)
{
  unsigned int uVar1;

  misc_hexproof(player, card, event);
  if (((event == EVENT_PROT_MISC) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    uVar1 = get_color(attacking_card_controller, attacking_card);
    uVar1 = num_bits_set(uVar1);
    if (uVar1 == 1)
    {
      event_result = event_result | 1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c3860; original cards/ravnica_allegiance.c:3739. */
int card_sunder_shaman(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;
  const char *prompt;
  int times_damaged[2];
  target_definition_t td;

  maxblockers_1(player, card, event);
  result = damage_dealt_by_me(player, card, event,
                              DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                  DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result != 0)
  {

    instance = get_card_instance(player, card);

    uVar4 = 0;
    uVar3 = instance->targets[1].player;
    instance->targets[1].player = 0;
    times_damaged[0] = uVar3 & 0xff;
    times_damaged[1] = uVar3 >> 8 & 0xff;
    do
    {
      uVar3 = current_turn ^ uVar4;
      if (times_damaged[uVar3] != 0)
      {
        default_target_definition(player, card, &td, 0x44);
        td.allow_cancel = 0;
        td.allowed_controller = uVar3;
        td.preferred_controller = uVar3;
        result = can_target(&td);
        if (result != 0)
        {
          if (player == uVar3)
          {
            prompt = "Select target artifact or enchantment you control.";
          }
          else
          {
            prompt = "Select target artifact or enchantment controlled by opponent.";
          }
          result = new_pick_target(&td, prompt, 0, 0x80);
          if (result != 0)
          {
            action_on_target(player, card, 0, ACT_KILL_DESTROY);
          }
        }
      }
      uVar4 = uVar4 + 1;
      instance->number_of_targets = '\0';
    } while (uVar4 != 2);
  }
  return 0;
}
