#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222cd80; original cards/modern_horizons_2.c:427. */
int card_arcbound_mouser(int player, int card, event_t event)
{
  int result;
  lifelink(player, card, event);
  result = modular(player, card, event, 1);
  return result;
}

/* ManalinkEh 2022: 0x222cdd0; original cards/modern_horizons_2.c:437. */
int card_arcbound_prototype(int player, int card, event_t event)
{
  int result;
  result = modular(player, card, event, 2);
  return result;
}

/* ManalinkEh 2022: 0x222d020; original cards/modern_horizons_2.c:497. */
int card_arcbound_whelp(int player, int card, event_t event)
{
  unsigned int modular_result;
  unsigned int shade_result;
  modular_result = modular(player, card, event, 2);
  shade_result = generic_shade_merge_pt(player, card, event, 0, 0, 0, 0, 0, 1, 0, 1, 0);
  return shade_result | modular_result;
}

/* ManalinkEh 2022: 0x222d0c0; original cards/modern_horizons_2.c:507. */
int card_archfiend_of_sorrows(int player, int card, event_t event)
{
  int result;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    pump_creatures_until_eot(player, card, 1 - player, 0, -2, -2, 0, 0, NULL);
  }
  result = unearth(player, event, 3, 2, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x222e0a0; original cards/modern_horizons_2.c:761. */
int card_barbed_spike(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x4995, &token);
    token.tou = 1;
    token.pow = 1;
    token.key_plus = 0x20;
    token.action = TOKEN_ACTION_EQUIP;
    generate_token(&token);
  }
  result = vanilla_equipment(player, card, event, 2, 1, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x222e160; original cards/modern_horizons_2.c:782. */
int card_batterbone(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  instance = is_equipping(player, card);
  if (instance != NULL)
  {
    result = vanilla_equipment(player, card, event, 5, 1, 1, 0, 10);
    return result;
  }
  result = living_weapon(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x222e500; original cards/modern_horizons_2.c:851. */
int card_blazing_rootwalla(int player, int card, event_t event)
{
  int result;
  if (event - EVENT_CAN_PAY_MADNESS_COST < 2)
  {
    result = madness(player, card, event, 0, 0, 0, 0, 0, 0);
    return result;
  }
  if (event - EVENT_POW_BOOST < 2)
  {
    result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x2000, 0, 0, 0, 0, 1, 0,
                                       0, NULL, NULL);
    if (result != 0)
    {
      return 2;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    result = generic_shade(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0);
    return result;
  }
  result = generic_activated_ability(player, card, event, 0x2000, 0, 0, 0, 0, 1, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x222b980; original cards/modern_horizons_2.c:936. */
int blossoming_calm_effect(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      player_bits[player] =
          player_bits[player] | 0x20000;
    }
  }
  else if ((event == EVENT_CAN_SKIP_TURN) && (player == current_turn))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x222ee70; original cards/modern_horizons_2.c:1019. */
int card_bottle_golems(int player, int card, event_t event)
{
  int result;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  result = get_power(player, card);
  gain_life(player, result);
  return 0;
}

/* ManalinkEh 2022: 0x222eed0; original cards/modern_horizons_2.c:1030. */
int card_brainstone(int player, int card, event_t event)
{
  int result;
  int card_00;
  test_definition_t this_test;
  if (event == EVENT_ACTIVATE)
  {
    ai_modifier = ai_modifier + hand_count[player] * -10 + 0x46;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_cards(player, 3);
    new_default_test_definition(&this_test, 0xff, "Select a card to put on top.");
    result = hand_count[player] < 3 ? hand_count[player] : 2;
    while (result > 0)
    {
      card_00 = new_select_a_card(player, player, 4, 1, 3, -1, &this_test);
      if (card_00 != -1)
      {
        put_on_top_of_deck(player, card_00);
        --result;
      }
    }
  }
  return generic_activated_ability(player, card, event, 9, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
}

/* ManalinkEh 2022: 0x222ff10; original cards/modern_horizons_2.c:1326. */
int card_calibrated_blast(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  test_definition_t this_test;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return do_flashback(player, card, event, 3, 0, 0, 0, 2, 0);
      }
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 0x102);
        td.zone = 0x1200;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            default_test_definition(&this_test, 1);
            this_test.type_flag = 1;
            result = reveal_card_until_you_reveal_test(player, player, &this_test, 0x204);
            if (-1 < result)
            {
              result = get_cmc_by_internal_id(result);
              damage_target0(player, card, result);
            }
          }
          result = get_flashback(player, card);
          if (result == 0)
          {
            kill_card(player, card, KILL_DESTROY);
          }
          else
          {
            kill_card(player, card, KILL_EXILE);
          }
        }
        return generic_spell(player, card, event, 0x81, &td,
                             "Select target creature, planeswalker or player.", 1, NULL);
      }
      if (event != EVENT_CHANGE_TARGET)
        return do_flashback(player, card, event, 3, 0, 0, 0, 2, 0);
    }
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
  return generic_spell(player, card, event, 0x81, &td,
                       "Select target creature, planeswalker or player.", 1, NULL);
}

/* ManalinkEh 2022: 0x222aef0; original cards/modern_horizons_2.c:1484. */
static void carth_ability(int player, int card)
{
  test_definition_t test;
  new_default_test_definition(&test, 0x100, "Select a planeswalker card.");
  reveal_top_cards_of_library_and_choose(player, card, player, 7, 0, TUTOR_HAND, 1,
                                         TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &test);
  return;
}

/* ManalinkEh 2022: 0x2230a90; original cards/modern_horizons_2.c:1690. */
int card_chrome_courier(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 0xff, "Select a card.");
    result = reveal_top_cards_of_library_and_choose(player, card, player, 2, 1, TUTOR_HAND, 1,
                                                    TUTOR_GRAVE, 0, &test);
    if (-1 < result)
    {
      result = is_what(player, result, 0x40);
      if (result != 0)
      {
        gain_life(player, 3);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2230b70; original cards/modern_horizons_2.c:1709. */
int card_clattering_augur(int player, int card, event_t event)
{
  int result;
  cannot_block(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_a_card(player);
    lose_life(player, 1);
  }
  else if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = has_mana_multi(player, 2, 2, 0, 0, 0, 0);
    if (result != 0)
    {
      return 4;
    }
  }
  else if (event == EVENT_PAY_FLASHBACK_COSTS)
  {
    charge_mana_multi(player, 2, 2, 0, 0, 0, 0);
    return (unsigned int)(spell_fizzled != 1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2230fc0; original cards/modern_horizons_2.c:1807. */
int card_crack_open(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 0x44);
  instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      generate_token_by_id(player, card, 0x4998);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 1, &td, "DISENCHANT", 1, NULL);
}

/* ManalinkEh 2022: 0x2232bb0; original cards/modern_horizons_2.c:2121. */
int card_deepwood_denizen(int player, int card, event_t event)
{
  int result;
  vigilance(player, card, event);
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
  result = count_counters_by_counter_and_card_type(player, COUNTER_P1_P1, TYPE_CREATURE);
  result = generic_activated_ability(player, card, event, 1,
                                     5U - result & (int)(5U - result ^ 0xffffffff) >> 0x1f, 0, 0, 1,
                                     0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2233220; original cards/modern_horizons_2.c:2291. */
int card_dihadas_ploy(int player, int card, event_t event)
{
  int result;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      draw_cards(player, 2);
      discard(player, 0, player);
      result = get_trap_condition(player, 0x203);
      gain_life(player, result);
      kill_card(player, card, KILL_DESTROY);
    }
    result = jumpstart(player, card, event, 0x59df);
    return result;
  }
  result = generic_spell(player, card, EVENT_CAN_CAST, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22332d0; original cards/modern_horizons_2.c:2317. */
int card_disciple_of_the_sun(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  lifelink(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 0x47,
                                "Select target target permanent card with mana value 3 or less.");
    test.cmc = 4;
    test.cmc_flag = 3;
    result = new_special_count_grave(player, &test);
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

/* ManalinkEh 2022: 0x2233690; original cards/modern_horizons_2.c:2410. */
int card_drey_keeper(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    generate_tokens_by_id(player, card, 0x4991, 2);
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    default_test_definition(&this_test, 2);
    this_test.subtype = 0xc0;
    pump_creatures_until_eot(player, card, player, 0, 1, 0, 0, 0x10000, &this_test);
  }
  result = generic_activated_ability(player, card, event, 0, 3, 1, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2233940; original cards/modern_horizons_2.c:2472. */
int card_echoing_return(int player, int card, event_t event)
{
  int count;
  short base_value;
  int *graveyard;
  int iid_or_count;
  int grave_index;
  test_definition_t this_test;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        new_default_test_definition(&this_test, 2, "Select a creature card.");
        if ((card != affected_card) || (player != affected_card_controller))
          return generic_spell(player, card, event, 8, NULL, NULL, 1, &this_test);
        generic_spell(player, card, EVENT_CAST_SPELL, 8, NULL, NULL, 1, &this_test);
        if ((spell_fizzled != 1) && (ai_is_speculating == 1))
        {
          graveyard = graveyard_ptr_mutable[player];
          iid_or_count = validate_target_from_grave_source(player, card, player, 0);
          this_test.id = (int)cards_data[graveyard[iid_or_count]].id;
          base_value = cards_ptr[graveyard[iid_or_count]]->ai_base_value;
          iid_or_count = new_special_count_grave(player, &this_test);
          ai_modifier = ai_modifier + base_value * iid_or_count;
        }
      }
      return 0;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      new_default_test_definition(&this_test, 2, "Select a creature card.");
      if (event == EVENT_RESOLVE_SPELL)
      {
        iid_or_count = validate_target_from_grave_source(player, card, player, 0);
        if (iid_or_count != -1)
        {
          iid_or_count = graveyard_ptr_mutable[player][iid_or_count];
          count = count_graveyard(player);
          grave_index = count + -1;
          if (-1 < grave_index)
          {
            do
            {
              if (graveyard_ptr_mutable[player][grave_index] == iid_or_count)
              {
                from_grave_to_hand(player, grave_index, 1);
              }
              grave_index = grave_index + -1;
            } while (grave_index != -1);
          }
        }
        kill_card(player, card, KILL_DESTROY);
      }
      return generic_spell(player, card, event, 8, NULL, NULL, 1, &this_test);
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  new_default_test_definition(&this_test, 2, "Select a creature card.");
  return generic_spell(player, card, event, 8, NULL, NULL, 1, &this_test);
}

/* ManalinkEh 2022: 0x2233f70; original cards/modern_horizons_2.c:2614. */
int card_fae_offering(int player, int card, event_t event)
{
  int creature_spells;
  int all_spells;
  if (eot_trigger(player, card, event) != 0)
  {
    creature_spells = get_stormcreature_count(player);
    if (creature_spells != 0)
    {
      all_spells = get_specific_storm_count(player);
      if (creature_spells < all_spells)
      {
        generate_token_by_id(player, card, 0x4917);
        generate_token_by_id(player, card, 0x5077);
        generate_token_by_id(player, card, 0x4998);
      }
    }
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x2234010; original cards/modern_horizons_2.c:2632. */
int card_fairgrounds_patrol(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = has_mana_multi(player, 1, 0, 0, 0, 0, 1);
    if ((result != 0) &&
        (result = can_sorcery_be_played(player, EVENT_GRAVEYARD_ABILITY), result != 0))
    {
      return 0x20;
    }
  }
  else if (event == EVENT_PAY_FLASHBACK_COSTS)
  {
    charge_mana_multi(player, 1, 0, 0, 0, 0, 1);
    if (spell_fizzled != 1)
    {
      return 2;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATED_GRAVEYARD_ABILITY)
  {
    default_token_definition(player, card, 0x4995, &token);
    token.tou = 1;
    token.pow = 1;
    token.key_plus = 0x20;
    token.action = TOKEN_ACTION_EQUIP;
    generate_token(&token);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2234550; original cards/modern_horizons_2.c:2727. */
int card_feast_of_sanity(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result = discard_trigger(player, card, event, player, 2, 0);
  if (result != 0)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Select target creature, planeswalker or player.", 0, 0x80);
      if (result != 0)
      {
        damage_target0(player, card, 1);
        gain_life(player, 1);
      }
    }
    instance = get_card_instance(player, card);
    instance->number_of_targets = '\0';
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2234690; original cards/modern_horizons_2.c:2762. */
int card_flame_blitz(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    if (current_turn == player)
    {
      result = eot_trigger(player, card, event);
      if (result != 0)
      {
        default_test_definition(&this_test, 0x100);
        new_damage_all(player, card, 2, 5, 0, &this_test);
      }
    }
    result = global_enchantment(player, card, event);
    return result;
  }
  result = cycling(player, card, event, 2, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x22349c0; original cards/modern_horizons_2.c:2819. */
int card_flay_essence(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 3);
  instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      result = count_counters(instance->targets[0].player, instance->targets[0].card,
                              ~COUNTER_P1_P1);
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
      gain_life(player, result);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 0x81, &td, "Select target creature or planeswalker.",
                         1, NULL);
}

/* ManalinkEh 2022: 0x2235190; original cards/modern_horizons_2.c:2942. */
int card_fodder_tosser(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x100);
    td.zone = 0x1200;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x100);
    td.zone = 0x1200;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      damage_target0(player, card, 2);
    }
  }
  result = generic_activated_ability(player, card, event, 0x25, 0, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_PLAYER_OR_PLANESWALKER");
  return result;
}

/* ManalinkEh 2022: 0x2235350; original cards/modern_horizons_2.c:2980. */
int card_foundation_breaker(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x44);
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "DISENCHANT", 0, 0);
      if (result != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
    }
    instance->number_of_targets = '\0';
    result = evoked(player, card);
    if (result != 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = evoke(player, card, event, 1, 0, 0, 1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2235800; original cards/modern_horizons_2.c:3055. */
int card_fractured_sanity(int player, int card, event_t event)
{
  int result;
  if (1 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    if (event == EVENT_RESOLVE_ACTIVATION_FROM_HAND)
    {
      if (spell_fizzled != 1)
      {
        mill(1 - player, 4);
      }
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      mill(1 - player, 0xe);
      kill_card(player, card, KILL_DESTROY);
    }
    result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
    return result;
  }
  result = cycling(player, card, event, 1, 0, 1, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2235b90; original cards/modern_horizons_2.c:3152. */
int card_gargadon(int player, int card, event_t event)
{
  int result;
  result = suspend(player, card, event, 4, 1, 0, 0, 0, 1, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2236880; original cards/modern_horizons_2.c:3331. */
int card_ghost_lit_drifter(int player, int card, event_t event)
{
  unsigned int saved_state;
  card_instance_t *instance;
  int targets_or_result;
  int card_00;
  card_instance_t *activation;
  target_definition_t td;

  if (event == EVENT_CAN_ACTIVATE_FROM_HAND)
    return has_mana_multi(player, 0, 0, 1, 0, 0, 0) != 0;
  if (event == EVENT_ACTIVATE_FROM_HAND)
  {
    charge_mana_multi(player, -1, 0, 1, 0, 0, 0);
    if (spell_fizzled != 1)
    {
      instance = get_card_instance(player, card);
      instance->targets[0].player = x_value;
      discard_card(player, card);
      return 1;
    }
    return 0;
  }
  if (event == EVENT_RESOLVE_ACTIVATION_FROM_HAND)
  {

    if (spell_fizzled == 1)
    {
      return 0;
    }
    card_00 = add_card_to_hand(player, get_internal_card_id_from_csv_id(0x5a07));
    instance = get_card_instance(player, card_00);
    saved_state = instance->state;
    hand_count[player] = hand_count[player] + -1;
    instance->state = saved_state | 0x20;
    default_target_definition(player, card_00, &td, 2);
    td.preferred_controller = player;
    activation = get_card_instance(player, card);
    targets_or_result = activation->targets[0].player;
    while ((targets_or_result != 0 && (new_pick_target(&td, "TARGET_CREATURE", 0, 0) != 0)))
    {
      pump_ability_until_eot(player, card_00, instance->targets[0].player,
                             instance->targets[0].card, 0, 0, 0x20, 0);
      targets_or_result = targets_or_result + -1;
      instance->number_of_targets = '\0';
    }
    obliterate_card(player, card_00);
    return 0;
  }
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION &&
      event != EVENT_CLEANUP)
    return 0;
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.special = 0x4000;
  instance = get_card_instance(player, card);
  if ((event == EVENT_RESOLVE_ACTIVATION) &&
      (targets_or_result = valid_target(&td), targets_or_result != 0))
  {
    pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 0,
                           0, 0x20, 0);
  }
  return generic_activated_ability(player, card, event, 4, 2, 0, 1, 0, 0, 0, 0, &td,
                                   "Select another target creature.");
}

/* ManalinkEh 2022: 0x2236bc0; original cards/modern_horizons_2.c:3398. */
int card_gilt_blade_prowler(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = get_trap_condition(player, 0x203);
    if (result == 0)
    {
      return 0;
    }
    return generic_activated_ability(player, card, event, 1, 1, 0, 0, 0, 0, 0, 1, NULL, NULL);
  }

  if (event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION && event != EVENT_CLEANUP)
    return 0;
  if (event == EVENT_RESOLVE_ACTIVATION)
    draw_cards(player, 1);
  return generic_activated_ability(player, card, event, 1, 1, 0, 0, 0, 0, 0, 1, NULL, NULL);
}

/* ManalinkEh 2022: 0x22381f0; original cards/modern_horizons_2.c:3858. */
int card_jewel_eyed_cobra(int player, int card, event_t event)
{
  int result;
  deathtouch(player, card, event);
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  generate_token_by_id(player, card, 0x4998);
  return 0;
}

/* ManalinkEh 2022: 0x22387d0; original cards/modern_horizons_2.c:3978. */
int card_kitchen_imp(int player, int card, event_t event)
{
  int result;
  haste(player, card, event);
  if (1 < event - EVENT_CAN_PAY_MADNESS_COST)
  {
    return 0;
  }
  result = madness(player, card, event, 0, 1, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2238a10; original cards/modern_horizons_2.c:4030. */
int card_late_to_dinner(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
  new_default_test_definition(&this_test, 2, "Select target creature card.");
  this_test.ai_selection_mode = 2;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = validate_target_from_grave_source(player, card, player, 0);
    if (result != -1)
    {
      reanimate_permanent(player, card, player, result, REANIMATE_DEFAULT);
      generate_token_by_id(player, card, 0x5077);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 8, NULL, NULL, 0, &this_test);
}

/* ManalinkEh 2022: 0x2239920; original cards/modern_horizons_2.c:4311. */
int card_lucid_dreams(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = count_types_in_grave(player);
    draw_cards(player, result);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x223a330; original cards/modern_horizons_2.c:4607. */
int card_monoskelion(int player, int card, event_t event)
{
  int result;
  result = triskelion_like_creature(player, card, event, 1, 1, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x223a780; original cards/modern_horizons_2.c:4695. */
int card_mystic_redaction(int player, int card, event_t event)
{
  int result;
  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    scry(player, 1);
  }
  result = discard_trigger(player, card, event, player, 2, 0);
  if (result != 0)
  {
    mill(1 - player, 2);
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x223b200; original cards/modern_horizons_2.c:4917. */
int card_persist2(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
  new_default_test_definition(&this_test, 2, "Select target nonlegendary creature card.");
  this_test.ai_selection_mode = 2;
  this_test.subtype = 0x2001;
  this_test.subtype_flag = 1;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = validate_target_from_grave_source(player, card, player, 0);
    if (result != -1)
    {
      reanimate_permanent(player, card, player, result, REANIMATE_MINUS1_MINUS1_COUNTER);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 8, NULL, NULL, 0, &this_test);
}

/* ManalinkEh 2022: 0x223b560; original cards/modern_horizons_2.c:4991. */
int card_piru_the_volatile(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  check_legend_rule(player, card, event);
  lifelink(player, card, event);
  if ((trigger_condition == 0xc9) && (current_turn == player))
  {
    upkeep_trigger_ability(player, card, event, player);
  }
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    charge_mana_multi(player, 0, 1, 0, 0, 1, 1);
    if (spell_fizzled == 1)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  default_test_definition(&this_test, 2);
  this_test.subtype = 0x2001;
  this_test.subtype_flag = 1;
  new_damage_all(player, card, 2, 7, 0, &this_test);
  return 0;
}

/* ManalinkEh 2022: 0x223c5c0; original cards/modern_horizons_2.c:5303. */
int card_recalibrate(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      bounce_permanent(instance->targets[0].player, instance->targets[0].card);
      result = get_trap_condition(player, 0x203);
      if (result != 0)
      {
        draw_a_card(player);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
}

/* ManalinkEh 2022: 0x223c800; original cards/modern_horizons_2.c:5353. */
int card_rift_sower(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  color_test_t available;
  color_t color;
  if ((event != EVENT_CAN_CAST) && (event != EVENT_MODIFY_COST))
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto card_generic_noncombat_1_mana_producing_creature;
    }
    else if ((event != EVENT_RESOLVE_SPELL) &&
             ((event != EVENT_CAN_CHANGE_TARGET && (event != EVENT_CHANGE_TARGET))))
    {
    card_generic_noncombat_1_mana_producing_creature:
      if (event == EVENT_CAN_CAST)
      {
        return 1;
      }
      instance = get_card_instance(player, card);
      available = instance->mana_color & COLOR_TEST_ANY;
      if ((instance->mana_color & 0x3f) != 0)
      {
        if ((available - COLOR_TEST_COLORLESS & available) != COLOR_TEST_0)
        {
          result = mana_producing_creature_all_one_color(player, card, event, 0x18, available, 1);
          return result;
        }
        color = single_color_test_bit_to_color(available);
        available = mana_producing_creature(player, card, event, 0x18, color, 1);
      }
      return available;
    }
  }
  result = suspend(player, card, event, 2, 0, 0, 0, 1, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x223cd70; original cards/modern_horizons_2.c:5434. */
int card_rishadan_dockhand(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 1);
    if (event == EVENT_CAN_ACTIVATE)
      return generic_activated_ability(player, card, event, 5, 1, 0, 0, 0, 0, 0, 0, &td,
                                       "TARGET_LAND");
    generic_activated_ability(player, card, EVENT_ACTIVATE, 5, 1, 0, 0, 0, 0, 0, 0, &td,
                              "TARGET_LAND");
    if (spell_fizzled != 1)
    {
      ai_mod_for_tap_effects(player, card, 0);
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {
    default_target_definition(player, card, &td, 1);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        tap_card(instance->targets[0].player, instance->targets[0].card);
      }
    }
    return generic_activated_ability(player, card, event, 5, 1, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_LAND");
  }
  return 0;
}

/* ManalinkEh 2022: 0x223d690; original cards/modern_horizons_2.c:5576. */
int card_sanctifier_en_vec(int player, int card, event_t event)
{
  test_definition_t this_test;
  default_test_definition(&this_test, 0xff);
  this_test.color = get_sleighted_color_test(player, card, 0x12);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    for (int grave_owner = 0; grave_owner < 2; ++grave_owner)
      for (int index = count_graveyard(grave_owner) - 1; index >= 0; --index)
        if (new_make_test(grave_owner, graveyard_ptr_mutable[grave_owner][index], -1, &this_test))
          rfg_card_from_grave(grave_owner, index);
  }
  if (if_a_card_would_be_put_into_graveyard_from_library_do_something_instead(
          player, card, event, 2, RESOLVE_TRIGGER_MANDATORY, &this_test) != 0)
  {
    rfg_top_card_of_deck(trigger_cause_controller);
    replace_milled = 1;
  }
  if (if_a_card_would_be_put_into_graveyard_from_anywhere_but_library_do_something_instead(
          player, card, event, 2, RESOLVE_TRIGGER_MANDATORY, &this_test) != 0)
  {
    add_card_to_rfg(trigger_cause_controller, trigger_cause);
    play_sound_effect(WAV_DESTROY);
    replace_milled = 1;
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x223d910; original cards/modern_horizons_2.c:5635. */
int card_sanctum_weaver(int player, int card, event_t event)
{
  int result;
  if ((event != EVENT_CAN_CAST) && (event != EVENT_SHOULD_AI_PLAY))
  {
    result = count_permanents_by_type(player, TYPE_ENCHANTMENT);
    result = mana_producing_creature_all_one_color(player, card, event, 0x18,
                                                   COLOR_TEST_ANY_COLORED, result);
    return result;
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x223e9b0; original cards/modern_horizons_2.c:5978. */
int card_smell_fear(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  target_definition_t td1;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = 1 - player;
    td1.allow_cancel = 2;
    td1.preferred_controller = td1.allowed_controller;
    get_card_instance(player, card);
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    if (result != 0)
    {
      result = can_target(&td1);
      return result;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      td.preferred_controller = player;
      default_target_definition(player, card, &td1, 2);
      td1.allowed_controller = 1 - player;
      td1.allow_cancel = 2;
      td1.preferred_controller = td1.allowed_controller;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        result = new_pick_target(&td, "Select target creature you control.", 0, 0x81);
        if (result != 0)
        {
          new_pick_target(&td1, "Select target creature opponent controls.", 1, 0x80);
        }
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = 1 - player;
    td1.allow_cancel = 2;
    td1.preferred_controller = td1.allowed_controller;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = validate_target(player, card, &td, 0);
      if (result != 0)
      {
        proliferate(player, card);
        result = validate_target(player, card, &td1, 1);
        if (result != 0)
        {
          fight(instance->targets[0].player, instance->targets[0].card, instance->targets[1].player,
                instance->targets[1].card);
        }
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x223f180; original cards/modern_horizons_2.c:6138. */
int card_soul_of_migration(int player, int card, event_t event)
{
  int result;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    generate_tokens_by_id(player, card, 0x4908, 2);
    result = evoked(player, card);
    if (result != 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = evoke(player, card, event, 3, 0, 0, 0, 0, 1);
  return result;
}

/* ManalinkEh 2022: 0x223f250; original cards/modern_horizons_2.c:6156. */
int card_specimen_collector(int player, int card, event_t event)
{
  int result;
  target_t tgt;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    generate_token_by_id(player, card, 0x4991);
    generate_token_by_id(player, card, 0x59d6);
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    default_target_definition(player, card, &td, 0x1000);
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.allow_cancel = 0;
    result = can_target(&td);
    if (result != 0)
    {
      result = select_target(player, card + -1000, &td, "Select target token you control.", &tgt);
      if (result != 0)
      {
        copy_token(player, card, tgt.player, tgt.card);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x223f570; original cards/modern_horizons_2.c:6228. */
int card_squirrel_sovereign(int player, int card, event_t event)
{
  boost_creature_type(player, card, event, SUBTYPE_SQUIRREL, 1, 1, 0, BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x223f720; original cards/modern_horizons_2.c:6269. */
int card_steelfin_whale(int player, int card, event_t event)
{
  affinity(player, card, event, 0x40, -1);
  etb_ability_global(player, event, 2, TYPE_ARTIFACT, NULL, player, card);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  untap_card(player, card);
  return 0;
}

/* ManalinkEh 2022: 0x223f8a0; original cards/modern_horizons_2.c:6304. */
int card_storm_gods_oracle(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "TARGET_ANY", 0, 0);
      if (result != 0)
      {
        damage_target0(player, card, 3);
      }
    }
  }
  if (((event != EVENT_ACTIVATE) && (1 < event - EVENT_RESOLVE_ACTIVATION)) &&
      (event != EVENT_CLEANUP))
  {
    result = global_enchantment(player, card, event);
    return result;
  }
  result = generic_shade(player, card, event, 0, 1, 0, 0, 0, 0, 0, 1, -1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x223fa00; original cards/modern_horizons_2.c:6327. */
int card_strike_it_rich(int player, int card, event_t event)
{
  int result;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return do_flashback(player, card, event, 2, 0, 0, 0, 1, 0);
      }
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      generate_token_by_id(player, card, 0x4998);
      result = get_flashback(player, card);
      if (result == 0)
      {
        kill_card(player, card, KILL_DESTROY);
      }
      else
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
      return do_flashback(player, card, event, 2, 0, 0, 0, 1, 0);
  }
  return generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
}

/* ManalinkEh 2022: 0x223fdd0; original cards/modern_horizons_2.c:6406. */
int card_sudden_edict(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  if (event == EVENT_CAN_CAST)
  {
    result = card_diabolic_edict(player, card, EVENT_CAN_CAST);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    add_state(player, card, 0x100000);
    default_target_definition(player, card, &td, 0xff);
    td.zone = 0x1000;
    result = can_target(&td);
    if ((result != 0) && (result = new_pick_target(&td, "TARGET_PLAYER", 0, 1), result != 0))
    {
      card_diabolic_edict(player, card, EVENT_RESOLVE_SPELL);
    }
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x223feb0; original cards/modern_horizons_2.c:6427. */
int card_suspend(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      suspend_a_card(player, card, instance->targets[0].player, instance->targets[0].card, 2);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
}

/* ManalinkEh 2022: 0x222b780; original cards/modern_horizons_2.c:6494. */
static char *target_you_own(int who_chooses, int player, int card, int targeting_player,
                            int targeting_card)
{
  int result;
  char *error;
  result = get_owner(player, card);
  error = NULL;
  if (result != targeting_player)
  {
    error = "owner";
  }
  return error;
}

/* ManalinkEh 2022: 0x22405d0; original cards/modern_horizons_2.c:6546. */
int card_sylvan_anthem(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  boost_creature_by_color(player, card, event, 8, 1, 1, 0, BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&this_test, 2);
    this_test.color = get_sleighted_color_test(player, card, 8);
    etb_ability_global(player, event, 2, TYPE_NONE, &this_test, player, card);
  }
  if (event == EVENT_ETB)
  {
    scry(player, 1);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2241280; original cards/modern_horizons_2.c:6774. */
int card_thought_monitor(int player, int card, event_t event)
{
  affinity(player, card, event, 0x40, -1);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  draw_cards(player, 2);
  return 0;
}

/* ManalinkEh 2022: 0x2241b80; original cards/modern_horizons_2.c:6989. */
int card_tourachs_canticle(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  ec_definition_t ec;
  target_definition_t td;
  test_definition_t this_test;

  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  td.zone = 0x1000;
  instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      default_ec_definition(instance->targets[0].player, player, &ec);
      new_default_test_definition(&this_test, 0xff, "Select a card to discard.");
      new_effect_coercion(&ec, &this_test);
      discard(1 - player, 3, player);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
}

/* ManalinkEh 2022: 0x2241ea0; original cards/modern_horizons_2.c:7047. */
int card_tragic_fall(int player, int card, event_t event)
{
  int result;
  result = (-(unsigned int)(hand_count[player] == 0) & 0xfffffff6) - 3;
  result = vanilla_instant_pump(player, card, event, 2, 1 - player, result, result, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2242300; original cards/modern_horizons_2.c:7126. */
int card_underworld_hermit(int player, int card, event_t event)
{
  int howmany;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  howmany = devotion(player, card, 1, 0);
  generate_tokens_by_id(player, card, 0x4991, howmany);
  return 0;
}

/* ManalinkEh 2022: 0x2242380; original cards/modern_horizons_2.c:7138. */
int card_unholy_heat(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 0x102);
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      result = delirium(player);
      damage_target0(player, card, (-(unsigned int)(result == 0) & 0xfffffffc) + 6);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 1, &td, "TARGET_CREATURE_OR_PLANESWALKER", 1, NULL);
}

/* ManalinkEh 2022: 0x22424c0; original cards/modern_horizons_2.c:7162. */
int card_unmarked_grave(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_CAN_CAST)
  {
    result = basic_spell(player, card, EVENT_CAN_CAST);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      ai_modifier = ai_modifier + 0x60;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&this_test, 0xff,
                                "Select a nonlegendary card to put into your graveyard.");
    this_test.subtype = 0x2001;
    this_test.subtype_flag = 1;
    new_global_tutor(player, player, 1, 3, 0, 6, &this_test);
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x22435b0; original cards/modern_horizons_2.c:7497. */
int card_vermin_gorger(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    lose_life(1 - player, 2);
    gain_life(player, 2);
  }
  result = generic_activated_ability(player, card, event, 0x40201, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2243660; original cards/modern_horizons_2.c:7511. */
int card_viashino_lashclaw(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    pump_creatures_until_eot_no_repeat(player, card, player, 0, 0, 4, NULL);
  }
  result = generic_activated_ability(player, card, event, 0x21, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2243720; original cards/modern_horizons_2.c:7523. */
int card_vile_entomber(int player, int card, event_t event)
{
  test_definition_t this_test;
  deathtouch(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  new_default_test_definition(&this_test, 0xff, "Select a card to put into your graveyard.");
  new_global_tutor(player, player, 1, 3, 0, 6, &this_test);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222c420; original cards/modern_horizons_2.c:247. */
int enrage(int player, int card, event_t event)
{
  int result;

  result = damage_dealt_to_me_arbitrary(player, card, event, DDBM_0, player, card);
  return result;
}

/* ManalinkEh 2022: 0x222da30; original cards/modern_horizons_2.c:646. */
int card_asmoranomardicadaistinaculdacar(int player, int card, event_t event)
{
  int tgt_player;
  card_instance_t *instance;
  int result;
  unsigned int first_food;
  unsigned int second_food;
  unsigned int player_00;
  unsigned int player_01;
  target_definition_t *creature_target;
  target_definition_t td2;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      bool payable = false;
      if (get_trap_condition(player, 0x203))
      {
        int generic = get_updated_casting_cost(player, card, -1, EVENT_MODIFY_COST, 0);
        if (has_mana_hybrid(player, 1, 1, 4, generic))
        {
          instance = get_card_instance(player, card);
          instance->info_slot = 1;
          null_casting_cost(player, card);
          payable = true;
        }
      }
      if (!payable)
      {
        infinite_casting_cost();
        instance = get_card_instance(player, card);
        instance->info_slot = 0;
      }
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if (((((card == affected_card) && (player == affected_card_controller)) &&
          (result = check_special_flags(player, card, 0x40000), result == 0)) &&
         ((result = is_token(player, card),
           result == 0 && (result = check_special_flags(player, card, 0x80000), result == 0)))) &&
        (instance = get_card_instance(player, card), instance->info_slot == 1))
    {
      charge_mana_hybrid(player, card, 1, 1, 4, 0);
    }
  }
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 0xff, "Select a card named The Underworld Cookbook.");
    test.id = 0x5a8b;
    new_global_tutor(player, player, 1, 1, 0, 5, &test);
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td2, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      default_test_definition(&test, 0x47);
      test.subtype = 0x3008;
      test.qty = 2;
      result = new_can_sacrifice_as_cost(player, card, &test);
      if (result == 0)
      {
        return 0;
      }
      return generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 0, 0, 0, 0, 0, 0, 0,
                                       &td2, NULL);
    }
  }
  else
  {
    if (event != EVENT_RESOLVE_ACTIVATION && event != EVENT_CLEANUP)
      return 0;
    default_target_definition(player, card, &td2, 2);
    instance = get_card_instance(player, card);
  }
  creature_target = &td2;
  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&test, 0x47, "Select a Food to sacrifice.");
    test.subtype = 0x3008;
    first_food =
        new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
    if (first_food != 0)
    {
      second_food = new_sacrifice(player, card, player,
                                  SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
      player_00 = (int)first_food >> 0x10 & 0xff;
      state_untargettable(player_00, first_food >> 0x18, 0);
      if (second_food != 0)
      {
        player_01 = (int)second_food >> 0x10 & 0xff;
        state_untargettable(player_01, second_food >> 0x18, 0);
        if (spell_fizzled == 1)
        {
          return 0;
        }
        generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 0, 0, 0, 0, 0, 0, 0,
                                  creature_target, "TARGET_CREATURE");
        if (spell_fizzled == 1)
        {
          return 0;
        }
        kill_card(player_00, first_food >> 0x18, KILL_SACRIFICE);
        kill_card(player_01, second_food >> 0x18, KILL_SACRIFICE);
        return 0;
      }
    }
    spell_fizzled = 1;
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) &&
           (result = valid_target(creature_target), result != 0))
  {
    result = instance->targets[0].card;
    tgt_player = instance->targets[0].player;
    damage_creature(tgt_player, result, 6, tgt_player, result);
  }
  return 0;
}

/* ManalinkEh 2022: 0x222dfe0; original cards/modern_horizons_2.c:745. */
int card_bannerhide_krushok(int player, int card, event_t event)
{
  int result;

  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    result = scavenge(player, card, event, 5, 0, 0, 2, 0, 0);
    return result;
  }
  result = reinforce(player, card, event, 2, 1, 0, 0, 1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x222e9d0; original cards/modern_horizons_2.c:943. */
int card_blossoming_calm(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_effect(player, card, blossoming_calm_effect);
    gain_life(player, 2);
    rebound(player, card);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2234150; original cards/modern_horizons_2.c:2660. */
int card_faithless_salvaging(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    discard(player, 0, 0);
    draw_a_card(player);
    rebound(player, card);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2234790; original cards/modern_horizons_2.c:2782. */
int card_flametongue_yearling(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
      result = is_token(player, card);
      if (result == 0)
      {
        result = check_special_flags(player, card, 0x40000);
        if ((result == 0) && (spell_fizzled != 1))
        {
          result = do_multikicker(player, card, EVENT_CAST_SPELL, 2, 0, 0, 0, 0, 0);
          instance->info_slot = result;
        }
      }
    }
    instance = get_card_instance(player, card);
    enters_the_battlefield_with_counters(player, card, EVENT_CAST_SPELL, COUNTER_P1_P1,
                                         instance->info_slot);
    etb_ability(player, card, EVENT_CAST_SPELL, RESOLVE_TRIGGER_MANDATORY);
    return 0;
  }
  instance = get_card_instance(player, card);
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, instance->info_slot);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  td.allow_cancel = 0;
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "TARGET_CREATURE", 0, 0);
    if (result != 0)
    {
      result = get_power(player, card);
      damage_target0(player, card, result);
    }
  }
  instance = get_card_instance(player, card);
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x2234b40; original cards/modern_horizons_2.c:2844. */
int card_floodhound(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    ai_modifier = ai_modifier + hand_count[player] * -10 + 0x46;
    investigate(player, card, player);
  }
  result = generic_activated_ability(player, card, event, 1, 3, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22354a0; original cards/modern_horizons_2.c:3005. */
int card_foundry_helix(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int sacrificed_or_owner;
  int result;
  unsigned int card_00;
  target_definition_t td;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    new_default_test_definition(&test, 0x47, "Select a permanent to sacrifice.");
    get_card_instance(player, card);
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_ANY", 1, NULL);
    if (result != 0)
    {
      return new_can_sacrifice_as_cost(player, card, &test);
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      new_default_test_definition(&test, 0x47, "Select a permanent to sacrifice.");
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        sacrificed_or_owner = new_sacrifice(player, card, player,
                                            SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
        if (sacrificed_or_owner == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_ANY", 1, NULL);
        card_00 = sacrificed_or_owner >> 0x18;
        sacrificed_or_owner = sacrificed_or_owner >> 0x10 & 0xff;
        if (spell_fizzled == 1)
        {
          state_untargettable(sacrificed_or_owner, card_00, 0);
        }
        else
        {
          result = is_what(sacrificed_or_owner, card_00, 0x40);
          instance->info_slot = result;
          kill_card(sacrificed_or_owner, card_00, KILL_SACRIFICE);
        }
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    new_default_test_definition(&test, 0x47, "Select a permanent to sacrifice.");
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if ((result != 0) && (damage_target0(player, card, 4), instance->info_slot == 1))
      {
        gain_life(player, 4);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22358e0; original cards/modern_horizons_2.c:3079. */
int card_funnel_web_recluse(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if ((event == EVENT_ETB) && (0 < creatures_dead_this_turn))
  {
    investigate(player, card, player);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2236cb0; original cards/modern_horizons_2.c:3420. */
int card_glimmer_bairn(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  test_definition_t test;

  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&test, 0x47, "Select a token to sacrifice.");
    test.type_flag = 6;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 0x47, "Select a token to sacrifice.");
    test.type_flag = 6;
    if (event == EVENT_CHECK_PUMP)
    {
      result = generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_CAN_ACTIVATE,
                                                                GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL,
                                                                NULL, TYPE_NONE, &test, SARV_0);
      if (result != 0)
      {
        pumpable_power[player] = pumpable_power[player] + 2;
        pumpable_toughness[player] = pumpable_toughness[player] + 2;
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      pump_until_eot_merge_previous(instance->parent_controller, instance->parent_card,
                                    instance->parent_controller, instance->parent_card, 2, 2);
    }
  }
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x2237ea0; original cards/modern_horizons_2.c:3749. */
int card_hard_evidence(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    generate_token_by_id(player, card, 0x59d6);
    investigate(player, card, player);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2238590; original cards/modern_horizons_2.c:3940. */
int card_kaleidoscorch(int player, int card, event_t event)
{
  card_instance_t *instance;
  target_definition_t td;

  store_num_colors_paid_in_eot_toughness0(player, card, event);

  bool counted_colors = trigger_condition == 0xd3 && reason_for_trigger_controller == player &&
                        trigger_cause_controller == player && trigger_cause == card &&
                        event == EVENT_TRIGGER;
  if (counted_colors)
  {
    unsigned char colors = 0;
    for (int color = 1; color != 6; ++color)
      if (mana_paid[color] > 0)
        ++colors;
    instance = get_card_instance(player, card);
    instance->eot_toughness0 = colors;
  }
  else if (event == EVENT_CAN_CAST || event == EVENT_CHANGE_TARGET ||
           (event == EVENT_CAST_SPELL && card == affected_card &&
            player == affected_card_controller) ||
           event == EVENT_RESOLVE_SPELL || event == EVENT_CAN_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    if (event == EVENT_RESOLVE_SPELL)
    {
      if (valid_target(&td))
      {
        instance = get_card_instance(player, card);
        damage_target0(player, card, (unsigned int)instance->eot_toughness0);
      }
      int flashback = get_flashback(player, card);
      kill_card(player, card, flashback ? KILL_EXILE : KILL_DESTROY);
    }
    return generic_spell(player, card, event, 1, &td, "TARGET_ANY", 1, NULL);
  }
  return do_flashback(player, card, event, 4, 0, 0, 0, 1, 0);
}

/* ManalinkEh 2022: 0x223aad0; original cards/modern_horizons_2.c:4759. */
int card_nested_shambler(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  default_token_definition(player, card, 0x4991, &token);
  token.action = TOKEN_ACTION_TAPPED;
  result = get_power(player, card);
  token.qty = (unsigned char)result;
  generate_token(&token);
  return 0;
}

/* ManalinkEh 2022: 0x223b380; original cards/modern_horizons_2.c:4950. */
int card_piercing_rays(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    result = card_excoriate(player, card, event);
    return result;
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.illegal_state = 1;
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    result = has_mana_multi(player, 2, 0, 0, 0, 0, 1);
    if ((((result != 0) && (result = can_target(&td), result != 0)) &&
         (instance->info_slot != 1)) &&
        (current_phase == 4))
    {
      return 1;
    }
  }
  else if (event == EVENT_ACTIVATE_FROM_HAND)
  {
    instance->number_of_targets = '\0';
    charge_mana_multi(player, 2, 0, 0, 0, 0, 1);
    if ((spell_fizzled != 1) &&
        (result = new_pick_target(&td, "Select target untapped creature.", 0, 0x80), result != 0))
    {
      return 1;
    }
  }
  else if (((event == EVENT_RESOLVE_ACTIVATION_FROM_HAND) && (spell_fizzled != 1)) &&
           (result = valid_target(&td), result != 0))
  {
    tap_card(instance->targets[0].player, instance->targets[0].card);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x223b6b0; original cards/modern_horizons_2.c:5031. */
int card_priest_of_fell_rites(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = count_graveyard_by_type(player, 2);
    if ((0 < result) && (result = graveyard_has_shroud(player), result == 0))
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x19, 0, 0, 0, 0, 0, 0,
                                         3, NULL, NULL);
      return result;
    }
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      new_default_test_definition(&this_test, 2, "Select target creature card.");
      instance = get_card_instance(player, card);
      instance->number_of_targets = '\0';
      result = select_target_from_grave_source(player, card, player, SFG_0, 2, &this_test, 0);
      if (result != -1)
      {
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x19, 0, 0, 0, 0, 0, 0, 3,
                                           NULL, NULL);
        return result;
      }
      spell_fizzled = 1;
      return 0;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (result = validate_target_from_grave_source(player, card, player, 0), result != -1))
    {
      reanimate_permanent(player, card, player, result, REANIMATE_DEFAULT);
    }
  }
  result = unearth(player, event, 3, 1, 0, 0, 0, 1);
  return result;
}

/* ManalinkEh 2022: 0x223b8f0; original cards/modern_horizons_2.c:5067. */
int card_prismatic_ending(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  store_num_colors_paid_in_eot_toughness0(player, card, event);
  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 0x47);
  td.illegal_type = 1;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      instance = get_card_instance(player, card);
      result = get_cmc(instance->targets[0].player, instance->targets[0].card);
      if (result <= (int)(unsigned int)instance->eot_toughness0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 0x81, &td, "Select target nonland permanent.", 1, NULL);
}

/* ManalinkEh 2022: 0x223bdf0; original cards/modern_horizons_2.c:5167. */
int card_ragavan_nimble_pilferer(int player, int card, event_t event)
{
  unsigned int packed_damage_counts;
  int result;
  card_instance_t *instance;
  unsigned int remaining;
  int damaged_player;
  unsigned int *damage_count;
  int times_damaged[2];

  check_legend_rule(player, card, event);
  dash(player, card, event, 1, 0, 0, 0, 1, 0);
  result = damage_dealt_by_me(player, card, event,
                              DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                  DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result != 0)
  {
    instance = get_card_instance(player, card);
    damaged_player = 0;
    packed_damage_counts = instance->targets[1].player;
    instance->targets[1].player = 0;
    damage_count = (unsigned int *)times_damaged;
    remaining = packed_damage_counts & 0xff;
    times_damaged[1] = packed_damage_counts >> 8 & 0xff;
    times_damaged[0] = remaining;
    while (true)
    {
      while (remaining != 0)
      {
        generate_token_by_id(player, card, 0x4998);
        exile_top_x_card_you_may_play_them(player, card, 1, NULL, damaged_player, MPCFE_UNTIL_EOT);
        remaining = remaining - 1;
        *damage_count = remaining;
      }
      damaged_player = damaged_player + 1;
      damage_count = damage_count + 1;
      if (damaged_player == 2)
        break;
      remaining = *damage_count;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x223cf40; original cards/modern_horizons_2.c:5463. */
int card_road_ruin(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = can_sorcery_be_played(player, EVENT_GRAVEYARD_ABILITY);
    if (result == 0)
    {
      return 0;
    }
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    if (result != 0)
    {
      result = aftermath(player, card, EVENT_GRAVEYARD_ABILITY, 1, 0, 0, 0, 2, 0, 0);
      return result;
    }
  }
  else
  {
    if (event == EVENT_CAN_CAST)
    {
      result = basic_spell(player, card, EVENT_CAN_CAST);
      return result;
    }
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      result = basic_spell(player, card, EVENT_CAST_SPELL);
      return result;
    }
    if (event == EVENT_PLAY_SECOND_HALF)
    {
      result = generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL);
      return result;
    }
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = get_aftermath(player, card);
      if (result == 0)
      {
        result = card_rampant_growth(player, card, EVENT_RESOLVE_SPELL);
        return result;
      }
      result = get_aftermath(player, card);
      if ((result != 0) && (result = valid_target(&td), result != 0))
      {
        result = count_subtype(player, 1, -1);
        damage_target0(player, card, result);
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  result = aftermath(player, card, event, 1, 0, 0, 0, 2, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x223db80; original cards/modern_horizons_2.c:5676. */
int card_scour_the_desert(int player, int card, event_t event)
{
  int grave_index;
  int toughness;
  token_generation_t token;
  test_definition_t this_test;

  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
  new_default_test_definition(&this_test, 2, "Select target creature card.");
  this_test.ai_selection_mode = 2;
  if (event == EVENT_RESOLVE_SPELL)
  {
    grave_index = validate_target_from_grave_source(player, card, player, 0);
    if (grave_index != -1)
    {
      default_token_definition(player, card, 0x4908, &token);
      token.tou = 1;
      token.pow = 1;
      token.color_forced = ' ';
      toughness = get_base_toughness_iid(player, graveyard_ptr_mutable[player][grave_index]);
      token.qty = (unsigned char)toughness;
      rfg_card_from_grave(player, grave_index);
      generate_token(&token);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 8, NULL, NULL, 0, &this_test);
}

/* ManalinkEh 2022: 0x223e840; original cards/modern_horizons_2.c:5949. */
int card_slag_strider(int player, int card, event_t event)
{
  int result;
  target_definition_t td_creature_or_player;

  affinity(player, card, event, 0x40, -1);
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td_creature_or_player, 0x102);
    td_creature_or_player.zone = 0x1200;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td_creature_or_player, 0x102);
    td_creature_or_player.zone = 0x1200;
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (result = valid_target(&td_creature_or_player), result != 0))
    {
      damage_target0(player, card, 1);
    }
  }
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_CAN_TARGET, 1, 0, 0, 0, 0, 0, 0, &td_creature_or_player,
      "TARGET_ANY", TYPE_ARTIFACT, NULL, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x223f7b0; original cards/modern_horizons_2.c:6285. */
int card_step_through(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (1 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    if (event == EVENT_RESOLVE_ACTIVATION_FROM_HAND)
    {
      new_default_test_definition(&test, 0xff, "Select a Wizard card.");
      test.subtype = 0xd6;
      new_global_tutor(player, player, 1, 1, 0, 1, &test);
    }
    result = card_undo(player, card, event);
    return result;
  }
  result = cycling(player, card, event, 2, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2240230; original cards/modern_horizons_2.c:6473. */
int card_sweep_the_skies(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  store_num_colors_paid_in_eot_toughness0(player, card, event);
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
      instance = get_card_instance(player, card);
      generate_tokens_by_id(player, card, 0x4995, (unsigned int)instance->eot_toughness0);
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  result = generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2240950; original cards/modern_horizons_2.c:6632. */
int card_terramorph2(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = count_subtype(player, 1, -1);
      ai_modifier = ai_modifier + (6 - result) * 8;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    tutor_basic_land(player, 1, 0);
    rebound(player, card);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x22425d0; original cards/modern_horizons_2.c:7188. */
int card_urban_daggertooth(int player, int card, event_t event)
{
  int result;

  vigilance(player, card, event);
  result = damage_dealt_to_me_arbitrary(player, card, event, DDBM_0, player, card);
  if (result == 0)
  {
    return 0;
  }
  proliferate(player, card);
  return 0;
}

/* ManalinkEh 2022: 0x22437e0; original cards/modern_horizons_2.c:7547. */
int card_wavesifter(int player, int card, event_t event)
{
  int result;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    investigate(player, card, player);
    investigate(player, card, player);
    result = evoked(player, card);
    if (result != 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = evoke(player, card, event, 0, 0, 1, 1, 0, 0);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2237f10; original cards/modern_horizons_2.c:3778. */
int card_hell_mongrel(int player, int card, event_t event)
{
  int result;

  if (1 < event - EVENT_CAN_PAY_MADNESS_COST)
  {
    result = card_noose_constrictor(player, card, event);
    return result;
  }
  result = madness(player, card, event, 2, 1, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2238d40; original cards/modern_horizons_2.c:4088. */
int card_lens_flare(int player, int card, event_t event)
{
  int result;

  affinity(player, card, event, 0x40, -1);
  result = card_sandblast(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x223ab50; original cards/modern_horizons_2.c:4775. */
int card_nettlecyst(int player, int card, event_t event)
{
  int result;
  int power;

  if (event - EVENT_POWER < 2)
  {
    result = count_permanents_by_type(player, TYPE_ARTIFACT | TYPE_ENCHANTMENT);
    power = count_permanents_by_type(player, TYPE_ARTIFACT | TYPE_ENCHANTMENT);
    attached_gets_pt(player, card, event, power, result);
  }
  result = living_weapon(player, card, event, 2);
  return result;
}

/* ManalinkEh 2022: 0x223c790; original cards/modern_horizons_2.c:5341. */
int card_revolutionist(int player, int card, event_t event)
{
  int result;

  if (1 < event - EVENT_CAN_PAY_MADNESS_COST)
  {
    result = card_archaeomancer(player, card, event);
    return result;
  }
  result = madness(player, card, event, 3, 0, 0, 0, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x22408e0; original cards/modern_horizons_2.c:6620. */
int card_terminal_agony(int player, int card, event_t event)
{
  int result;

  if (1 < event - EVENT_CAN_PAY_MADNESS_COST)
  {
    result = card_murder2(player, card, event);
    return result;
  }
  result = madness(player, card, event, 0, 1, 0, 0, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x2241b20; original cards/modern_horizons_2.c:6976. */
int card_tormods_cryptkeeper(int player, int card, event_t event)
{
  int result;

  vigilance(player, card, event);
  if (((event != EVENT_ACTIVATE) && (1 < event - EVENT_RESOLVE_ACTIVATION)) &&
      (event != EVENT_CLEANUP))
  {
    return 0;
  }
  result = card_tormods_crypt(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2243bb0; original cards/modern_horizons_2.c:7635. */
int card_young_necromancer(int player, int card, event_t event)
{
  int result;
  target_t exiled[2];
  test_definition_t this_test;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
    result = count_graveyard(player);
    if (((2 < result) && (result = count_graveyard_by_type(player, 2), result != 0)) &&
        (result = graveyard_has_shroud(player), result == 0))
    {
      return 0;
    }
    ai_modifier = ai_modifier + -0x30;
    return 0;
  }
  if (event != EVENT_ETB)
  {
    return 0;
  }
  int grave_count = count_graveyard(player);
  if (grave_count < 3 && (player == 1 || ai_is_speculating == 1))
    return 0;
  int creatures = count_graveyard_by_type(player, 2);
  if (creatures == 0 && (player == 1 || ai_is_speculating == 1))
    return 0;
  int shrouded = graveyard_has_shroud(player);
  if (shrouded && (player == 1 || ai_is_speculating == 1))
    return 0;
  new_default_test_definition(&this_test, 0xff, "Select a card to exile");
  select_multiple_cards_from_graveyard_to_array(player, player, -1, 3, &this_test, 2, exiled);
  if (spell_fizzled != 1)
  {
    rfg_card_from_grave(player, exiled[0].player);
    rfg_card_from_grave(player, exiled[1].player);
    result = count_graveyard_by_type(player, 2);
    if ((result != 0) && (result = graveyard_has_shroud(player), result == 0))
    {
      new_default_test_definition(&this_test, 2, "Select target creature card.");
      new_global_tutor(player, player, 2, 4, 1, 1, &this_test);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x222b840; original cards/modern_horizons_2.c:7671. */
int yusri_effect(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
  }
  result = card_omniscience(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222b9f0; original cards/modern_horizons_2.c:550. */
static char *doesnt_have_p1_p1_counter_and_can_be_activated(int who_chooses, int player, int card)
{
  int result;
  char *error;

  result = count_counters(player, card, COUNTER_P1_P1);
  error = "must not have +1/+1 counters.";
  if (result == 0)
  {
    result = can_use_activated_abilities(player, card);
    error = NULL;
    if (result == 0)
    {
      return "this can\'t use activated abilities.";
    }
  }
  return error;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int is_artifact_land(int iid, int unused, int unused2, int unused3);
static int is_snow_or_can_produce_colorless(int iid, int val, int player, int card);
static int count_lands_in_play_and_in_hand(int player);
static int carth_effect(int player, int card, event_t event);
static int tide_shaper_effect(int player, int card, event_t event);
int one_or_more_p1_p1_counters_are_putted_on_me(int player, int card, event_t event);
void ward_granted(int player, int card, event_t event, int t_player, int t_card, type_t type,
                  subtype_t subtype, int flags, int cless, int black, int blue, int green, int red,
                  int white, int life_to_pay);
void ward_life(int player, int card, event_t event, int life_to_pay);

/* ManalinkEh 2022: 0x222bce0; original cards/modern_horizons_2.c:67. */
int one_or_more_p1_p1_counters_are_putted_on_me(int player, int card, event_t event)
{
  xtrigger_t trigger;
  int result;

  enable_xtrigger_flags = enable_xtrigger_flags | ENABLE_XTRIGGER_1_1_COUNTERS;
  trigger = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    trigger = xtrigger_impl_value_dont_use_directly;
  }
  if (trigger != XTRIGGER_1_1_COUNTERS)
  {
    return 0;
  }
  if ((((card == affected_card) && (player == affected_card_controller)) &&
       (reason_for_trigger_controller == player)) &&
      ((trigger_cause_controller == player && (trigger_cause == card))))
  {
    result = is_humiliated(player, card, AL_TRIG);
    if (result == 0)
    {
      if (event == EVENT_TRIGGER)
      {
        event_result = event_result | 2;
      }
      return (unsigned int)(event == EVENT_RESOLVE_TRIGGER);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x222bdd0; original cards/modern_horizons_2.c:202. */
void ward_granted(int player, int card, event_t event, int t_player, int t_card, type_t type,
                  subtype_t subtype, int flags, int cless, int black, int blue, int green, int red,
                  int white, int life_to_pay)
{
  int spell_or_target_card;
  card_instance_t *instance;
  int spell_player_or_result;
  int saved_fizzle_or_result;
  unsigned int counter_unless_paid;
  char buffer[100];

  spell_or_target_card = trigger_cause;
  spell_player_or_result = trigger_cause_controller;
  saved_fizzle_or_result = spell_fizzled;
  if (trigger_condition != 0xd3)
  {
    return;
  }
  if (((card == affected_card) && (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    if (event == EVENT_TRIGGER)
    {
      saved_fizzle_or_result = is_what(trigger_cause_controller, trigger_cause, 1);
      if ((saved_fizzle_or_result == 0) && (1 - t_player == trigger_cause_controller))
      {

        instance = get_card_instance(1 - t_player, trigger_cause);
        counter_unless_paid = 0;
        if (instance->number_of_targets != '\0')
        {
          saved_fizzle_or_result = 0;
          do
          {
            spell_player_or_result = instance->targets[saved_fizzle_or_result].player;
            if (((((t_player == spell_player_or_result) &&
                   (spell_or_target_card = instance->targets[saved_fizzle_or_result].card,
                    spell_or_target_card != -1)) &&
                  ((t_card == spell_or_target_card || (t_card == -1)))) &&
                 (((player != spell_player_or_result || card != spell_or_target_card ||
                    (((unsigned int)flags >> 1 & 1) == 0)) &&
                   ((type == TYPE_NONE || (spell_player_or_result = is_what(
                                               spell_player_or_result, spell_or_target_card, type),
                                           spell_player_or_result != 0)))))) &&
                ((subtype == SUBTYPE_INVALID ||
                  (spell_player_or_result =
                       has_subtype(instance->targets[saved_fizzle_or_result].player,
                                   instance->targets[saved_fizzle_or_result].card, subtype),
                   spell_player_or_result != 0))))
            {
              counter_unless_paid = 2;
              break;
            }
            saved_fizzle_or_result = saved_fizzle_or_result + 1;
          } while (saved_fizzle_or_result < (int)(unsigned int)instance->number_of_targets);
        }
        event_result = event_result | counter_unless_paid;
      }
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {

      counter_unless_paid = flags & 1;
      if (counter_unless_paid == 0)
      {
        counter_unless_paid = 1;
      }
      else if (-1 < hand_count[trigger_cause_controller])
      {

        discard(trigger_cause_controller, 6, player);
        counter_unless_paid = (unsigned int)(spell_fizzled == 1);
      }
      spell_fizzled = saved_fizzle_or_result;
      if ((life_to_pay != 0) &&
          (saved_fizzle_or_result = can_pay_life(spell_player_or_result, life_to_pay),
           saved_fizzle_or_result != 0))
      {

        scnprintf(buffer, 100, "Pay %d life", life_to_pay);
        saved_fizzle_or_result =
            dialog_impl(player, card, EVENT_RESOLVE_TRIGGER, "\001\002", "\001\003", "\001\014",
                        "\001\010", spell_player_or_result, buffer, 1,
                        ((5 < life[spell_player_or_result] - life_to_pay) - 1 & 0xfffffff1) + 0x14,
                        "Decline", 1, 10, 0, 0, 0, 0);
        if (saved_fizzle_or_result == 1)
        {
          counter_unless_paid = 0;
          lose_life(spell_player_or_result, life_to_pay);
        }
      }
      if (((((((cless < 1) && (black < 1)) && (blue < 1)) && ((green < 1 && (red < 1)))) &&
            (white < 1)) ||
           ((saved_fizzle_or_result =
                 has_mana_multi(spell_player_or_result, cless, black, blue, green, red, white),
             saved_fizzle_or_result == 0 ||
                 (saved_fizzle_or_result = charge_mana_multi(spell_player_or_result, cless, black,
                                                             blue, green, red, white),
                  saved_fizzle_or_result == 0)))) &&
          (counter_unless_paid != 0))
      {
        real_counter_a_spell(player, card, spell_player_or_result, spell_or_target_card);
      }
    }
  }
  return;
}

void ward(int player, int card, event_t event, int flags, int cless, int black, int blue, int green,
          int red, int white, int life_to_pay)
{
  ward_granted(player, card, event, player, card, TYPE_NONE, SUBTYPE_INVALID, flags, cless, black,
               blue, green, red, white, life_to_pay);
}

/* ManalinkEh 2022: 0x222c220; original cards/modern_horizons_2.c:231. */
void ward_mana(int player, int card, event_t event, int cless, int black, int blue, int green,
               int red, int white)
{
  ward_granted(player, card, event, player, card, TYPE_NONE, SUBTYPE_INVALID, 0, cless, black, blue,
               green, red, white, 0);
  return;
}

/* ManalinkEh 2022: 0x222c2a0; original cards/modern_horizons_2.c:235. */
void ward_life(int player, int card, event_t event, int life_to_pay)
{
  ward_granted(player, card, event, player, card, TYPE_NONE, SUBTYPE_INVALID, 0, 0, 0, 0, 0, 0, 0,
               life_to_pay);
  return;
}

/* ManalinkEh 2022: 0x222c320; original cards/modern_horizons_2.c:239. */
void ward_discard(int player, int card, event_t event)
{
  ward_granted(player, card, event, player, card, TYPE_NONE, SUBTYPE_INVALID, 1, 0, 0, 0, 0, 0, 0,
               0);
  return;
}

/* ManalinkEh 2022: 0x222c3a0; original cards/modern_horizons_2.c:243. */
void ward_multidiscard(int player, int card, event_t event, int number_to_discard)
{
  ward_granted(player, card, event, player, card, TYPE_NONE, SUBTYPE_INVALID,
               number_to_discard << 0x18 | 1, 0, 0, 0, 0, 0, 0, 0);
  return;
}

/* ManalinkEh 2022: 0x222c460; original cards/modern_horizons_2.c:253. */
int card_abiding_grace(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  result = eot_trigger(player, card, event);
  if ((result != 0) && (current_turn == player))
  {

    new_default_test_definition(&test, 2, "Select a creature with CMC 1 or less.");
    test.cmc = 2;
    test.cmc_flag = 3;
    result = new_special_count_grave(player, &test);
    if (result != 0)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\003", "\001\014",
                             "Gain 1 life", 1, ((life[player] < 6) - 1 & 0xffffffec) + 0x1e,
                             "Reanimate", 1, 0x14, 0, 0, 0, 0);
        if (result != 1)
        {
          if (result == 2)
          {
            new_global_tutor(player, player, 2, 4, 1, 1, &test);
          }
          return global_enchantment(player, card, event);
        }
      }
    }
    gain_life(player, 1);
  }
  return global_enchantment(player, card, event);
}

/* ManalinkEh 2022: 0x222a8c0; original cards/modern_horizons_2.c:281. */
static int count_lands_in_play_and_in_hand(int player)
{
  int land;
  card_instance_t *instance;
  int card;
  int count;

  if (active_cards_count[player] < 1)
  {
    return 0;
  }
  card = 0;
  count = 0;
  do
  {
    while (
        (land = is_what(player, card, 1),
         land != 0 && ((instance = in_hand(player, card),
                        instance != NULL || (instance = in_play(player, card), instance != NULL)))))
    {
      count = count + 1;
      card = card + 1;
      if (active_cards_count[player] <= card)
      {
        return count;
      }
    }
    card = card + 1;
  } while (card < active_cards_count[player]);
  return count;
}

/* ManalinkEh 2022: 0x222c600; original cards/modern_horizons_2.c:289. */
int card_abundant_harvest(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      ai_modifier = ai_modifier + 0x14;
      result = count_lands_in_play_and_in_hand(player);
      if (result < 6)
      {
        ai_modifier = ai_modifier + 0x1e;
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {

    result = count_lands_in_play_and_in_hand(player);
    result =
        dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\003", "\001\014", "Land",
                    1, ((result < 6) - 1 & 0xffffffec) + 0x1e, "Nonland", 1, 0x14, 0, 0, 0, 0);
    default_test_definition(&this_test, 1);
    if (result == 2)
    {
      this_test.type_flag = 1;
    }
    reveal_card_until_you_reveal_test(player, player, &this_test, 0x24);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x222c940; original cards/modern_horizons_2.c:361. */
int card_altar_of_the_goyf(int player, int card, event_t event)
{
  card_instance_t *instance;
  int power;
  unsigned int card_00;

  store_attackers(player, card, event, DAT_ATTACKS_ALONE | DAT_TRACK, player, -1, NULL);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  if (event == EVENT_DAT_ABILITY)
  {

    instance = get_card_instance(player, card);
    card_00 = (unsigned int)(unsigned char)instance->targets[2].player;
    power = count_types_in_grave(2);
    instance = in_play(player, card_00);
    if (instance != NULL)
    {
      pump_until_eot(player, card, player, card_00, power, power);
    }
  }
  boost_creature_type(player, card, event, SUBTYPE_LHURGOYF, 0, 0, 0x80,
                      BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x222d190; original cards/modern_horizons_2.c:533. */
int card_archon_of_cruelty(int player, int card, event_t event)
{
  int result;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if ((event == EVENT_DAT_ABILITY) || (event == EVENT_ETB))
  {
    result = opponent_is_valid_target(player, card);
    if (result != 0)
    {
      result = 1 - player;
      player_sacrifices_a_permanent(player, card, result, 0x102, SAC_CAUSED);
      new_multidiscard(result, 1, player, 2);
      lose_life(result, 3);
      draw_cards(player, 1);
      gain_life(player, 3);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x222e6e0; original cards/modern_horizons_2.c:875. */
int card_blessed_respite(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST && event != EVENT_CAST_SPELL && event != EVENT_RESOLVE_SPELL &&
      event != EVENT_CAN_CHANGE_TARGET && event != EVENT_CHANGE_TARGET)
    return 0;
  if (event == EVENT_CAST_SPELL && (card != affected_card || player != affected_card_controller))
    return 0;
  target_definition_t td;
  default_target_definition(player, card, &td, 0x47);
  td.zone = 0x1000;
  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    if (current_turn == player)
    {
      td.preferred_controller = player;
      if (current_phase > 0x1d)
      {
        int grave_count = count_graveyard(player);
        ai_modifier += grave_count * 5;
      }
      else
        ai_modifier -= 0x32;
    }
    else
    {
      int opponent = 1 - player;
      td.preferred_controller = opponent;
      if (current_phase < 0x19)
      {
        int grave_count = count_graveyard(player);
        ai_modifier += grave_count * 5;
        for (int creature = 0; creature < active_cards_count[opponent]; ++creature)
          if (in_play(opponent, creature) && is_attacking(opponent, creature))
            ai_modifier += 10;
      }
      else
        ai_modifier -= 0x32;
    }
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (valid_target(&td))
    {
      card_instance_t *instance = get_card_instance(player, card);
      reshuffle_grave_into_deck(instance->targets[0].player, 0);
      return card_fog(player, card, event);
    }
    kill_card(player, card, KILL_BURY);
  }
  return generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
}

/* ManalinkEh 2022: 0x222ea40; original cards/modern_horizons_2.c:959. */
int card_bone_shards(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int sacrificed;
  unsigned int player_00;
  target_definition_t td;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x102);
    new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
    get_card_instance(player, card);
    if ((1 < hand_count[player]) ||
        (result = new_can_sacrifice_as_cost(player, card, &test), result != 0))
    {
      return generic_spell(player, card, EVENT_CAN_CAST, 0x81, &td,
                           "Select target creature or planeswalker.", 1, NULL);
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x102);
      new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
      get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        result = count_subtype(player, 2, -1);
        result =
            dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014", "\001\016",
                        "Sac a creature", 1, result * 10, "Discard a card", 1, 0x1e, 0, 0, 0, 0);
        if (result == 0)
        {
          spell_fizzled = 1;
        }
        else if (result == 1)
        {
          sacrificed = new_sacrifice(player, card, player,
                                     SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
          if (sacrificed == 0)
          {
            spell_fizzled = 1;
          }
          else
          {
            generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                          "Select target creature or planeswalker.", 1, NULL);
            if (-1 < (int)sacrificed)
            {
              player_00 = (int)sacrificed >> 0x10 & 0xff;
              if (spell_fizzled == 1)
              {
                state_untargettable(player_00, sacrificed >> 0x18, 0);
              }
              else
              {
                kill_card(player_00, sacrificed >> 0x18, KILL_SACRIFICE);
              }
            }
          }
        }
        else
        {
          if (result == 2)
          {
            discard(player, 0, 0);
          }
          generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                        "Select target creature or planeswalker.", 1, NULL);
        }
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x102);
    new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = validate_target(player, card, &td, 0);
      if (result != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x222f020; original cards/modern_horizons_2.c:1058. */
int card_break_ties(int player, int card, event_t event)
{
  int result;

  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    result = card_return_to_nature(player, card, event);
    return result;
  }
  result = reinforce(player, card, event, 1, 0, 0, 0, 0, 0, 1);
  return result;
}

/* ManalinkEh 2022: 0x222b8f0; original cards/modern_horizons_2.c:1077. */
static int is_snow_or_can_produce_colorless(int iid, int val, int player, int card)
{
  int result;
  unsigned int eligible;
  card_instance_t *instance;

  result = has_subtype(player, card, SUBTYPE_SNOW);
  eligible = 1;
  if (result == 0)
  {
    instance = get_card_instance(player, card);
    eligible = (unsigned int)((instance->card_color & 0x41) != 0);
  }
  return eligible;
}

/* ManalinkEh 2022: 0x222b940; original cards/modern_horizons_2.c:1084. */
static char *target_is_snow_or_can_produce_colorless(int who_chooses, int player, int card)
{
  int result;
  char *error;

  result = is_snow_or_can_produce_colorless(-1, -1, player, card);
  error = NULL;
  if (result == 0)
  {
    error = "must be snow or should produce |C";
  }
  return error;
}

/* ManalinkEh 2022: 0x222f4f0; original cards/modern_horizons_2.c:1164. */
int card_breyas_apprentice(int player, int card, event_t event)
{
  int choice;
  card_instance_t *instance;
  unsigned int sacrificed;
  int target_available;
  int in_combat;
  unsigned int player_00;
  target_definition_t *target;
  target_definition_t td;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    token_generation_t token;
    default_token_definition(player, card, 0x4995, &token);
    token.tou = 1;
    token.pow = 1;
    token.key_plus = 32;
    generate_token(&token);
  }
  else
  {
    test_definition_t test;
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      new_default_test_definition(&test, 0x40, "Select an artifact to sacrifice.");
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        return generic_activated_ability_with_sacrifice_as_cost(
            player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL,
            TYPE_ARTIFACT, &test, SARV_REPORT_CMC);
      }
      if (event == EVENT_ACTIVATE)
      {

        instance->number_of_targets = '\0';
        instance->info_slot = 0;
        target_available = can_target(&td);
        choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\016",
                             "+2/+0 to target creature", target_available,
                             -(unsigned int)(current_phase - 0x15U < 4) & 0x14, "Play from library",
                             1, 10, 0, 0, 0, 0);
        if (choice == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        sacrificed = new_sacrifice(player, card, player,
                                   SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
        if (sacrificed != 0)
        {
          if (choice == 1)
          {
            new_pick_target(&td, "TARGET_CREATURE", 0, 1);
          }
          player_00 = (int)sacrificed >> 0x10 & 0xff;
          if ((spell_fizzled != 1) && (generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0,
                                                                 0, 0, 0, 0, 0, 0, NULL, NULL),
                                       spell_fizzled != 1))
          {
            if (choice == 1)
            {
              in_combat = is_attacking(instance->targets[0].player, instance->targets[0].card);
              if ((in_combat != 0) ||
                  (in_combat = is_blocking(instance->targets[0].player, instance->targets[0].card),
                   in_combat != 0))
              {
                ai_modifier = ai_modifier + 0x28;
              }
            }
            else if (choice == 2)
            {
              ai_modifier = ai_modifier + (hand_count[player] * 5 + 5) * -2 + 0x46;
            }
            kill_card(player_00, sacrificed >> 0x18, KILL_SACRIFICE);
            add_state(player, card, 0x10);
            tapped_for_mana_color = -1;
            instance->mode = (uint8_t)choice;
            return 0;
          }
          state_untargettable(player_00, sacrificed >> 0x18, 0);
          spell_fizzled = 1;
          return 0;
        }
        spell_fizzled = 1;
        return 0;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        return 0;
      }
      new_default_test_definition(&test, 0x40, "Select an artifact to sacrifice.");
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      instance = get_card_instance(player, card);
    }
    target = &td;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (instance->mode == '\x01')
      {
        if (valid_target(target) != 0)
        {
          pump_until_eot_merge_previous(instance->parent_controller, instance->parent_card,
                                        instance->targets[0].player, instance->targets[0].card, 2,
                                        0);
        }
      }
      else if (instance->mode == '\x02')
      {
        exile_top_x_card_you_may_play_them(player, card, 1, NULL, player,
                                           MPCFE_UNTIL_END_OF_YOUR_NEXT_TURN);
        return 0;
      }
    }
  }

  return 0;
}

/* ManalinkEh 2022: 0x222fab0; original cards/modern_horizons_2.c:1252. */
int card_burdened_aerialist(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    generate_token_by_id(player, card, 0x4998);
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {

    instance = in_play(affected_card_controller, affected_card);
    if ((instance != NULL) && (affected_card_controller == player))
    {
      if (affected_card != card)
      {
        result = is_what(player, affected_card, 0x47);
        if ((result != 0) && (instance->kill_code == '\x03'))
        {
          result = is_token(affected_card_controller, affected_card);
          if (result != 0)
          {
            result = check_special_flags(affected_card_controller, affected_card, 0x800);
            if (result == 0)
            {

              instance = get_card_instance(player, card);
              if (instance->targets[0xb].player < 0)
              {
                instance->targets[0xb].player = 0;
              }
              *(char *)&instance->targets[0xb].player =
                  (char)instance->targets[0xb].player + '\x01';
            }
          }
        }
      }
    }
  }
  result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  pump_ability_until_eot_no_repeat(player, card, player, card, 0x20, 0);
  return 0;
}

/* ManalinkEh 2022: 0x222fc40; original cards/modern_horizons_2.c:1288. */
int card_cabal_initiate(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_POWER)
  {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (result = has_threshold(player), result != 0)) &&
        (result = is_humiliated(player, card, AL_STATIC), result == 0))
    {
      event_result = event_result + 1;
    }
  }
  else
  {
    if (event == EVENT_TOUGHNESS)
    {
      if (card != affected_card)
      {
        return 0;
      }
      if (player != affected_card_controller)
      {
        return 0;
      }
      result = has_threshold(player);
      if (result == 0)
      {
        return 0;
      }
      result = is_humiliated(player, card, AL_STATIC);
      if (result != 0)
      {
        return 0;
      }
      event_result = event_result + 2;
      return 0;
    }
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        return generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x20, 0, 0, 0, 0, 0, 0,
                                         0, NULL, NULL);
      }
      if (event == EVENT_ACTIVATE)
      {
        generic_activated_ability(player, card, EVENT_ACTIVATE, 0x20, 0, 0, 0, 0, 0, 0, 0, NULL,
                                  NULL);
        if (spell_fizzled == 1)
        {
          return 0;
        }
        result = check_for_special_ability(player, card, 2);
        if (result != 0)
        {
          ai_modifier = ai_modifier + -0xc;
          return 0;
        }
        result = is_attacking(player, card);
        if ((result == 0) && (result = is_blocking(player, card), result == 0))
        {
          return 0;
        }
        ai_modifier = ai_modifier + life[player] * -5 + 100;
        return 0;
      }
      return 0;
    }
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  pump_ability_until_eot_no_repeat(player, card, instance->parent_controller, instance->parent_card,
                                   0, 2);
  return 0;
}

/* ManalinkEh 2022: 0x222af70; original cards/modern_horizons_2.c:1489. */
static int carth_effect(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (event == EVENT_STATIC_EFFECTS && card == affected_card && player == affected_card_controller)
  {
    instance = get_card_instance(player, card);
    if (instance->damage_target_player >= 0 &&
        get_id(instance->damage_target_player, instance->damage_target_card) != 0x59cd)
      kill_card(player, card, KILL_EXILE);
  }
  instance = get_card_instance(player, card);
  if (instance->damage_target_player >= 0 &&
      effect_follows_control_of_attachment(player, card, event))
    return 0;
  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    instance = get_card_instance(player, card);
    card_instance_t *dying = in_play(affected_card_controller, affected_card);
    if (dying && get_card_instance(affected_card_controller, affected_card)->kill_code != 0)
    {
      if (instance->damage_target_player >= 0 && instance->damage_target_card == affected_card &&
          instance->damage_target_player == affected_card_controller)
      {
        instance->damage_target_card = -1;
        instance->damage_target_player = -1;
        remove_status(player, card, 0x10000);
      }
      count_for_gfp_ability(player, card, 0x77, player, 0x100, NULL);
    }
  }
  unsigned int deaths = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (!check_status(player, card, 0x10000))
  {
    for (deaths &= 0xff; deaths != 0;)
    {
      --deaths;
      carth_ability(player, card);
    }
    kill_card(player, card, KILL_EXILE);
  }
  if (event == EVENT_CLEANUP && !check_status(player, card, 0x10000))
    kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x22303f0; original cards/modern_horizons_2.c:1538. */
int card_carth_the_lion(int player, int card, event_t event)
{
  int c;
  card_instance_t *instance;
  unsigned int deaths;
  int legacy_card;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    carth_ability(player, card);
  }
  else if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      bool found = false;
      if (0 < active_cards_count[player])
      {
        c = 0;
        do
        {
          instance = in_play(player, c);

          if ((((instance != NULL) && (is_what(player, c, 0x80) != 0)) &&
               (instance = get_card_instance(player, c), instance->info_slot_fn == carth_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
          {
            found = true;
            break;
          }
          c = c + 1;
        } while (c < active_cards_count[player]);
      }

      if (!found)
      {
        legacy_card = create_targetted_legacy_effect(player, card, carth_effect, player, card);
        instance = get_card_instance(player, legacy_card);
        instance->targets[0].player = player;
        instance->targets[0].card = card;
        instance->number_of_targets = '\x01';
        add_status(player, legacy_card, 0x10000);
      }
    }
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    count_for_gfp_ability(player, card, 0x77, player, 0x100, NULL);
  }
  instance = get_card_instance(player, card);
  if (instance->kill_code == '\0')
  {

    deaths = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    deaths = deaths & 0xff;
    if (deaths != 0)
    {
      do
      {
        deaths = deaths - 1;
        carth_ability(player, card);
      } while (0 < (int)deaths);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22307c0; original cards/modern_horizons_2.c:1626. */
int card_chatterstorm(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  if (event == EVENT_QUERY)
  {
    if (attacking_card == 4)
    {
      return 1;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    default_token_definition(player, card, 0x4991, &token);
    token.tou = 1;
    token.pow = 1;
    token.color_forced = '\b';
    generate_token(&token);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2230c90; original cards/modern_horizons_2.c:1742. */
int card_combine_chrysalis(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  token_generation_t token;
  test_definition_t test;

  if (event == EVENT_ABILITIES)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated(player, card, AL_STATIC);
      if ((result == 0) && (affected_card_controller == player))
      {
        result = is_what(player, affected_card, 2);
        if (result != 0)
        {
          result = is_token(affected_card_controller, affected_card);
          if (result != 0)
          {
            event_result = event_result | 0x20;
          }
        }
      }
    }
  }
  else if ((((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
            (event == EVENT_RESOLVE_ACTIVATION)) ||
           (event == EVENT_CLEANUP))
  {
    new_default_test_definition(&test, 0x47, "Select a token to sacrifice.");
    test.type_flag = 6;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      default_token_definition(player, card, 0x4907, &token);
      token.tou = 4;
      token.pow = 4;
      token.color_forced = '\b';
      generate_token(&token);
    }
    result = generic_activated_ability_with_sacrifice_as_cost(
        player, card, event, GAA_CAN_SORCERY_BE_PLAYED | GAA_UNTAPPED, 2, 0, 1, 1, 0, 0, 0, NULL,
        NULL, TYPE_NONE, &test, SARV_REPORT_CMC);
    return result;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2231740; original cards/modern_horizons_2.c:1949. */
int etb_tapped_indestructible_land(int player, int card, event_t event)
{
  int result;

  comes_into_play_tapped(player, card, event);
  indestructible(player, card, event);
  result = plain_mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2234640; original cards/modern_horizons_2.c:2749. */
int card_filigree_attendant(int player, int card, event_t event)
{
  int newval;

  if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
  {
    newval = count_permanents_by_type(player, TYPE_ARTIFACT);
    set_base_pt(newval);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2234c00; original cards/modern_horizons_2.c:2858. */
int card_flourishing_strike(int player, int card, event_t event)
{
  int choice;
  unsigned char selected_mode;
  card_instance_t *instance;
  int pump_available_or_valid;
  int entwine_available;
  unsigned int entwine_or_target_index;
  int damage_available_or_index;
  target_definition_t td;
  target_definition_t td2;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.required_abilities = 0x20;
    default_target_definition(player, card, &td2, 2);
    td2.preferred_controller = player;
    get_card_instance(player, card);
    damage_available_or_index = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    if ((damage_available_or_index != 0) ||
        (damage_available_or_index =
             generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL),
         damage_available_or_index != 0))
    {
      return 1;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.required_abilities = 0x20;
      default_target_definition(player, card, &td2, 2);
      td2.preferred_controller = player;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        instance->number_of_targets = '\0';
        if (check_special_flags2(player, card, -0x80000000) == 0)
        {

          instance->mode = '\0';
          damage_available_or_index = can_target(&td);
          pump_available_or_valid = can_target(&td2);
          entwine_or_target_index = 0;
          if ((pump_available_or_valid != 0) && (damage_available_or_index != 0))
          {
            entwine_available = has_mana_multi(player, 2, 0, 0, 1, 0, 0);
            entwine_or_target_index = (unsigned int)(entwine_available != 0);
          }
          choice =
              dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033",
                          "5 damage to target creature with flying", damage_available_or_index, 10,
                          "Target creature gets +3/+3 until end of turn", pump_available_or_valid,
                          5, "Entwine", entwine_or_target_index, 0xf, 0, 0, 0, 0);
          if (choice == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if ((choice == 3) && (charge_mana_multi(player, 2, 0, 0, 1, 0, 0), spell_fizzled == 1))
          {
            return 0;
          }
        }
        selected_mode = instance->mode;
        if ((selected_mode & 1) == 0)
        {
          entwine_or_target_index = 0;
        }
        else
        {
          damage_available_or_index =
              new_pick_target(&td, "Select target creature with flying.", 0, 0x81);
          entwine_or_target_index = (unsigned int)(damage_available_or_index != 0);
          if (spell_fizzled == 1)
          {
            return 0;
          }
          selected_mode = instance->mode;
        }
        if ((selected_mode & 2) != 0)
        {
          new_pick_target(&td2, "Select target creature to get +3/+3", entwine_or_target_index,
                          0x81);
        }
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.required_abilities = 0x20;
    default_target_definition(player, card, &td2, 2);
    td2.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      selected_mode = instance->mode;
      damage_available_or_index = 0;
      if ((selected_mode & 1) != 0)
      {
        if (validate_target(player, card, &td, 0) != 0)
        {
          damage_target0(player, card, 5);
        }
        selected_mode = instance->mode;
        damage_available_or_index = 1;
      }
      if (((selected_mode & 2) != 0) &&
          (pump_available_or_valid = validate_target(player, card, &td2, damage_available_or_index),
           pump_available_or_valid != 0))
      {
        pump_until_eot(player, card, instance->targets[damage_available_or_index].player,
                       instance->targets[damage_available_or_index].card, 3, 3);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22370b0; original cards/modern_horizons_2.c:3496. */
int card_goblin_anarchomancer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int spell_color;
  unsigned int sleighted_colors;

  if ((event == EVENT_MODIFY_COST_GLOBAL) && (affected_card_controller == player))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated(player, card, AL_STATIC);
      if (result == 0)
      {
        spell_color = get_color(affected_card_controller, affected_card);
        sleighted_colors = get_sleighted_color_test(player, card, 0x18);
        if ((spell_color & sleighted_colors) != 0)
        {
          COST_COLORLESS = COST_COLORLESS + -1;
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2237150; original cards/modern_horizons_2.c:3510. */
int card_goblin_traprunner(int player, int card, event_t event)
{
  int coin_result;
  int i;
  int wins;
  token_generation_t token;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    i = 3;

    wins = 0;
    do
    {
      coin_result = flip_a_coin(player, card);
      wins = wins + coin_result;
      i = i + -1;
    } while (i != 0);
    if (wins != 0)
    {
      default_token_definition(player, card, 0x493a, &token);
      token.qty = (unsigned char)wins;
      token.tou = 1;
      token.pow = 1;
      token.color_forced = '\x10';
      token.action = TOKEN_ACTION_ATTACKING;
      generate_token(&token);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2237210; original cards/modern_horizons_2.c:3545. */
int card_gouged_zealot(int player, int card, event_t event)
{
  xtrigger_t trigger;

  trigger = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    trigger = xtrigger_impl_value_dont_use_directly;
  }
  if (((event == EVENT_DECLARE_ATTACKERS) || (trigger == XTRIGGER_ATTACKING)) &&
      (delirium(player) != 0))
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  }
  if (event == EVENT_DAT_ABILITY)
  {
    new_damage_all(player, card, 1 - player, 1, 1, NULL);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x22372d0; original cards/modern_horizons_2.c:3564. */
int card_graceful_restoration(int player, int card, event_t event)
{
  enum
  {
    CHOICE_REANIMATE_P1_P1 = 1,
    CHOICE_REANIMATE_2 = 2
  };

  int i;
  int choice;
  uint8_t selected_mode;
  card_instance_t *instance;
  int grave_index_or_available;
  unsigned int one_available;
  int result_or_available;
  test_definition_t this_test;
  test_definition_t this_test2;

  if (event == EVENT_CAN_CAST)
  {
    new_default_test_definition(&this_test, 2, "Select target creature card.");
    this_test.ai_selection_mode = 2;
    new_default_test_definition(&this_test2, 2,
                                "Select target creature card with power 2 or less.");
    this_test2.power = 3;
    this_test2.power_flag = 3;
    this_test2.ai_selection_mode = 1;
    get_card_instance(player, card);
    return basic_spell(player, card, EVENT_CAN_CAST);
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      new_default_test_definition(&this_test, 2, "Select target creature card.");
      this_test.ai_selection_mode = 2;
      new_default_test_definition(&this_test2, 2,
                                  "Select target creature card with power 2 or less.");
      this_test2.power = 3;
      this_test2.power_flag = 3;
      this_test2.ai_selection_mode = 1;
      get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        result_or_available = new_special_count_grave(player, &this_test2);
        grave_index_or_available = count_graveyard_by_type(player, 2);
        one_available = 0;
        if (grave_index_or_available != 0)
        {
          grave_index_or_available = graveyard_has_shroud(player);
          one_available = (unsigned int)(grave_index_or_available == 0);
        }
        choice = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033", "\001\016",
                             "Reanimate one", one_available, 10, "Reanimate two",
                             result_or_available, 0x14, 0, 0, 0, 0);
        if (choice == 0)
        {
          spell_fizzled = 1;
          result_or_available = 0;
        }
        else if (choice == CHOICE_REANIMATE_P1_P1)
        {
          result_or_available =
              generic_spell(player, card, EVENT_CAST_SPELL, 8, NULL, NULL, 0, &this_test);
        }
        else
        {
          if (choice != CHOICE_REANIMATE_2)
          {
            return 0;
          }
          result_or_available =
              generic_spell(player, card, EVENT_CAST_SPELL, 0x28, NULL, NULL, 2, &this_test2);
        }
        return result_or_available;
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    new_default_test_definition(&this_test, 2, "Select target creature card.");
    this_test.ai_selection_mode = 2;
    new_default_test_definition(&this_test2, 2,
                                "Select target creature card with power 2 or less.");
    this_test2.power = 3;
    this_test2.power_flag = 3;
    this_test2.ai_selection_mode = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      selected_mode = instance->mode;
      if (selected_mode == '\x01')
      {

        result_or_available = validate_target_from_grave_source(player, card, player, 0);
        if (result_or_available != -1)
        {
          reanimate_permanent(player, card, player, result_or_available,
                              REANIMATE_PLUS1_PLUS1_COUNTER);
        }
        selected_mode = instance->mode;
      }
      if (selected_mode == '\x02')
      {

        for (i = 0; instance = get_card_instance(player, card), i < instance->info_slot; i = i + 1)
        {

          grave_index_or_available = validate_target_from_grave(player, card, player, i);
          if (grave_index_or_available != -1)
          {
            reanimate_permanent(player, card, player, grave_index_or_available, REANIMATE_DEFAULT);
          }
        }
      }
      kill_card(player, card, KILL_SACRIFICE);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    new_default_test_definition(&this_test, 2, "Select target creature card.");
    this_test.ai_selection_mode = 2;
    new_default_test_definition(&this_test2, 2,
                                "Select target creature card with power 2 or less.");
    this_test2.power = 3;
    this_test2.power_flag = 3;
    this_test2.ai_selection_mode = 1;
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2237f80; original cards/modern_horizons_2.c:3793. */
int card_herd_baloth(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  result = one_or_more_p1_p1_counters_are_putted_on_me(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  default_token_definition(player, card, 0x4907, &token);
  token.tou = 4;
  token.pow = 4;
  generate_token(&token);
  return 0;
}

/* ManalinkEh 2022: 0x2238460; original cards/modern_horizons_2.c:3910. */
int card_kaldra_compleat(int player, int card, event_t event)
{
  card_instance_t *instance;
  int equipped_card;
  int player_00;

  check_legend_rule(player, card, event);
  indestructible(player, card, event);
  instance = is_equipping(player, card);
  if (instance != NULL)
  {

    instance = get_card_instance(player, card);
    player_00 = (int)instance->damage_target_player;
    equipped_card = instance->damage_target_card;
    rfg_when_damage(player_00, equipped_card, event);
    if (((event == EVENT_CHANGE_TYPE) && (equipped_card == affected_card)) &&
        (player_00 == affected_card_controller))
    {
      if (is_humiliated(player, card, AL_TRIG) == 0)
      {
        instance = get_card_instance(player_00, equipped_card);
        instance->destroys_if_blocked = instance->destroys_if_blocked | 0xc;
      }
    }
    return vanilla_equipment(player, card, event, 7, 5, 5, 0x180, 0x804);
  }
  return living_weapon(player, card, event, 7);
}

/* ManalinkEh 2022: 0x2239d80; original cards/modern_horizons_2.c:4499. */
int card_mine_collapse(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  test_definition_t test;

  if (event == EVENT_MODIFY_COST)
  {
    default_test_definition(&test, 0x47);
    test.subtype = 0x5006;
    if ((current_turn == player) &&
        (result = new_can_sacrifice_as_cost(player, card, &test), result != 0))
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 1;
      null_casting_cost(player, card);
    }
    else
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
    }
  }
  else
  {
    if (event == EVENT_CAN_CAST)
    {
      default_target_definition(player, card, &td, 0x102);
      get_card_instance(player, card);
      return generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    }
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        default_target_definition(player, card, &td, 0x102);
        instance = get_card_instance(player, card);
        if ((card == affected_card) && (player == affected_card_controller))
        {
          result = played_for_free(player, card);
          if (((result == 0) && (result = is_token(player, card), result == 0)) &&
              (instance->info_slot == 1))
          {

            result = has_mana_to_cast_iid(player, EVENT_CAST_SPELL, instance->internal_card_id);
            if (result != 0)
            {
              result = count_subtype(player, 1, -1);
              result = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014",
                                   "Sacrifice a mountain", 1, result * 2, "Cast normally", 1, 5, 0,
                                   0, 0, 0);
              if (result != 1)
              {
                if (result == 2)
                {
                  result = get_id(player, card);
                  charge_mana_from_id(player, card, EVENT_CAST_SPELL, result);
                }
                else
                {
                  spell_fizzled = 1;
                }
                return generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                                     "Select target creature or planeswalker.", 1, NULL);
              }
            }
            new_default_test_definition(&test, 0x47, "Select a Mountain to sacrifice.");
            test.subtype = 0x5006;
            new_sacrifice(player, card, player, SAC_AS_COST, &test);
            td.allow_cancel = 0;
          }
          return generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                               "Select target creature or planeswalker.", 1, NULL);
        }
      }
    }
    else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0x102);
      get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = valid_target(&td);
        if (result != 0)
        {
          damage_target0(player, card, 5);
        }
        kill_card(player, card, KILL_DESTROY);
      }
    }
    else if (event == EVENT_CHANGE_TARGET)
    {
      default_target_definition(player, card, &td, 0x102);
      get_card_instance(player, card);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x223a830; original cards/modern_horizons_2.c:4712. */
int card_necrogoyf(int player, int card, event_t event)
{
  int own_creatures;
  int opponent_creatures;

  if (event - EVENT_CAN_PAY_MADNESS_COST < 2)
  {
    return madness(player, card, event, 1, 2, 0, 0, 0, 0);
  }
  upkeep_trigger_ability(player, card, event, 2);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
    {
      own_creatures = count_graveyard_by_type(player, 2);
      opponent_creatures = count_graveyard_by_type(1 - player, 2);
      set_base_pt(own_creatures + opponent_creatures);
    }
    return 0;
  }
  new_multidiscard(current_turn, 1, 2, player);
  return 0;
}

/* ManalinkEh 2022: 0x223a960; original cards/modern_horizons_2.c:4735. */
int card_necromancers_familiar(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_ABILITIES)
  {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (result = is_humiliated(player, card, AL_STATIC), result == 0)) &&
        (hand_count[player] == 0))
    {
      lifelink(player, card, EVENT_ABILITIES);
    }
  }
  else if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
           ((event == EVENT_RESOLVE_ACTIVATION || (event == EVENT_CLEANUP))))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      pump_ability_until_eot_no_repeat(player, card, instance->parent_controller,
                                       instance->parent_card, 0, 0x800);
      tap_card(instance->parent_controller, instance->parent_card);
    }
    result = generic_activated_ability(player, card, event, 0x20, 0, 1, 0, 0, 0, 0, 0, NULL, NULL);
    return result;
  }
  return 0;
}

/* ManalinkEh 2022: 0x223ace0; original cards/modern_horizons_2.c:4806. */
int card_obsidian_charmaw(int player, int card, event_t event)
{
  char colorless_lands;
  card_instance_t *instance;
  int opponent_or_result;
  int card_00;
  target_definition_t td1;

  if (event == EVENT_MODIFY_COST)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      opponent_or_result = 1 - player;
      if (active_cards_count[opponent_or_result] < 1)
      {
        colorless_lands = '\0';
      }
      else
      {
        card_00 = 0;
        colorless_lands = '\0';
        do
        {
          instance = in_play(opponent_or_result, card_00);
          if ((instance != NULL) && (is_what(opponent_or_result, card_00, 1) != 0))
          {
            instance = get_card_instance(opponent_or_result, card_00);
            colorless_lands = (colorless_lands + '\x01') - ((instance->card_color & 0x41) == 0);
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[opponent_or_result]);
      }
      COST_COLORLESS = COST_COLORLESS - colorless_lands;
    }
    etb_ability(player, card, EVENT_MODIFY_COST, RESOLVE_TRIGGER_MANDATORY);
    return 0;
  }
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td1, 1);
    td1.allowed_controller = 1 - player;
    td1.required_subtype = 0x2000;
    td1.special = 0x1000;
    td1.allow_cancel = 0;
    td1.preferred_controller = td1.allowed_controller;
    instance = get_card_instance(player, card);
    opponent_or_result = can_target(&td1);
    if ((opponent_or_result != 0) &&
        (opponent_or_result =
             new_pick_target(&td1, "Select target nonbasic land an opponent controls.", 0, 0x80),
         opponent_or_result != 0))
    {
      action_on_target(player, card, 0, ACT_KILL_DESTROY);
      instance->number_of_targets = '\0';
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x223bae0; original cards/modern_horizons_2.c:5108. */
int card_prophetic_titan(int player, int card, event_t event)
{
  card_instance_t *instance;
  int value;
  unsigned int selected_modes;
  target_definition_t td;
  test_definition_t test;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    value = delirium(player);
    if ((value == 0) && (value = can_target(&td), value != 0))
    {
      value = hand_count[player];
      selected_modes = dialog_impl(player, card, EVENT_ACTIVATE, "\001\014", "\001\003", "\001\016",
                                   "4 damage to any target", can_target(&td), 3,
                                   "Look at top four of library", 1, 7 - value, 0, 0, 0, 0);
      if (((selected_modes & 1) != 0) &&
          (value = new_pick_target(&td, "TARGET_ANY", 0, 1), value != 0))
      {
        damage_target0(player, card, 4);
        instance->number_of_targets = '\0';
      }
      if ((selected_modes & 2) == 0)
      {
        return 0;
      }
    }
    else
    {
      if (new_pick_target(&td, "TARGET_ANY", 0, 1) != 0)
      {
        damage_target0(player, card, 4);
        instance->number_of_targets = '\0';
      }
    }
    new_default_test_definition(&test, 0xff, "Select a card to add to your hand.");
    reveal_top_cards_of_library_and_choose(player, card, player, 4, 1, TUTOR_HAND, 0,
                                           TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &test);
  }
  return 0;
}

/* ManalinkEh 2022: 0x223c150; original cards/modern_horizons_2.c:5244. */
int card_raving_visionary(int player, int card, event_t event)
{
  int c;
  card_instance_t *instance;
  int result_or_cost;
  card_instance_t *permanent;
  unsigned int draw_available;
  int land_count;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result_or_cost = delirium(player);
      if ((result_or_cost != 0) &&
          (result_or_cost = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 2, 0, 1,
                                                      0, 0, 0, 0, NULL, NULL),
           result_or_cost != 0))
      {
        return 1;
      }
      return generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 1, 0, 0, 0, 0,
                                       NULL, NULL);
    }

    result_or_cost = delirium(player);
    draw_available = 0;
    if (result_or_cost != 0)
    {
      result_or_cost = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 2, 0, 1, 0, 0,
                                                 0, 0, NULL, NULL);
      draw_available = (unsigned int)(result_or_cost != 0);
    }
    result_or_cost = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 1, 0, 0,
                                               0, 0, NULL, NULL);
    if (dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\033",
                    "Draw & discard", result_or_cost, 5, "Draw a card", draw_available, 10, 0, 0, 0,
                    0) == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    c = 0;
    land_count = 0;
    bool no_active_cards = active_cards_count[player] < 1;
    if (!no_active_cards)
    {
      do
      {
        permanent = in_play(player, c);
        if (permanent != NULL || (permanent = in_hand(player, c), permanent != NULL))
          land_count += is_what(player, c, 1) != 0;
        ++c;
      } while (c < active_cards_count[player]);
    }
    if (no_active_cards || land_count - 3U > 4)
      ai_modifier += 0x30;
    if (instance->mode == '\x01')
    {
      result_or_cost = 0;
    }
    else
    {
      if (instance->mode != '\x02')
        return 0;
      result_or_cost = 2;
    }
    result_or_cost = generic_activated_ability(player, card, EVENT_ACTIVATE, 1, result_or_cost, 0,
                                               1, 0, 0, 0, 0, NULL, NULL);
  }
  else
  {
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      instance = get_card_instance(player, card);
      if ((event == EVENT_RESOLVE_ACTIVATION) && (draw_a_card(player), instance->mode == '\x01'))
      {
        discard(player, 0, player);
      }
    }
    result_or_cost = 0;
  }
  return result_or_cost;
}

/* ManalinkEh 2022: 0x223d840; original cards/modern_horizons_2.c:5617. */
int card_sanctuary_raptor(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    default_test_definition(&this_test, 0x47);
    this_test.type_flag = 6;
    result = check_battlefield_for_special_card(player, card, player, 4, &this_test);
    if (2 < result)
    {
      pump_ability_until_eot(player, card, player, card, 2, 0, 0x100, 0);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x223d990; original cards/modern_horizons_2.c:5647. */
int card_scion_of_draco(int player, int card, event_t event)
{
  int result;
  unsigned int creature_color;
  unsigned int sleighted_color;

  if (event != EVENT_MODIFY_COST)
  {
    if ((event == EVENT_ABILITIES) && (affected_card_controller == player))
    {
      result = is_what(player, affected_card, 2);
      if (result != 0)
      {
        result = is_humiliated(player, card, AL_STATIC);
        if (result == 0)
        {

          creature_color = get_color(affected_card_controller, affected_card);
          sleighted_color = get_sleighted_color_test(player, card, 0x20);
          if ((sleighted_color & creature_color) != 0)
          {
            vigilance(affected_card_controller, affected_card, EVENT_ABILITIES);
          }
          sleighted_color = get_sleighted_color_test(player, card, 4);
          if ((sleighted_color & creature_color) != 0)
          {
            hexproof(affected_card_controller, affected_card, EVENT_ABILITIES);
          }
          sleighted_color = get_sleighted_color_test(player, card, 2);
          if ((sleighted_color & creature_color) != 0)
          {
            lifelink(affected_card_controller, affected_card, EVENT_ABILITIES);
          }
          sleighted_color = get_sleighted_color_test(player, card, 0x10);
          if ((sleighted_color & creature_color) != 0)
          {
            event_result = event_result | 0x100;
          }
          sleighted_color = get_sleighted_color_test(player, card, 8);
          if ((sleighted_color & creature_color) != 0)
          {
            event_result = event_result | 0x80;
          }
        }
      }
    }
    return 0;
  }
  result = count_domain(player, card);
  COST_COLORLESS = COST_COLORLESS + (char)result * -2;
  return 0;
}

/* ManalinkEh 2022: 0x223ddc0; original cards/modern_horizons_2.c:5726. */
int card_scuttletide(int player, int card, event_t event)
{
  token_generation_t token;

  if (event - EVENT_POWER < 2)
  {
    if ((((affected_card_controller == player) && (is_humiliated(player, card, AL_STATIC) == 0)) &&
         (has_subtype(affected_card_controller, affected_card, SUBTYPE_CRAB) != 0)) &&
        (delirium(player) != 0))
    {
      event_result = event_result + 1;
    }
    return global_enchantment(player, card, event);
  }
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_CLEANUP)
    return generic_activated_ability(player, card, event, 0x20, 1, 0, 0, 0, 0, 0, 0, NULL, NULL);
  if (event != EVENT_RESOLVE_ACTIVATION)
    return global_enchantment(player, card, event);
  default_token_definition(player, card, 0x59d6, &token);
  token.pow = 0;
  token.tou = 3;
  token.color_forced = '\x04';
  generate_token(&token);
  return generic_activated_ability(player, card, event, 0x20, 1, 0, 0, 0, 0, 0, 0, NULL, NULL);
}

/* ManalinkEh 2022: 0x223df40; original cards/modern_horizons_2.c:5754. */
int card_search_the_premises(int player, int card, event_t event)
{
  int result;

  store_attackers(player, card, event, 0, 1 - player, -1, NULL);
  dat_ability(player, card, event, DAT_SEPARATE_TRIGGERS | DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  if (event == EVENT_DAT_ABILITY)
  {
    investigate(player, card, player);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x223e590; original cards/modern_horizons_2.c:5896. */
int card_skophos_reaver(int player, int card, event_t event)
{
  int result;

  if (event - EVENT_CAN_PAY_MADNESS_COST < 2)
  {
    result = madness(player, card, event, 1, 0, 0, 0, 1, 0);
    return result;
  }
  if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
  {
    result = is_humiliated(player, card, AL_STATIC);
    if ((result == 0) && (current_turn == player))
    {
      event_result = event_result + 2;
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x223ecd0; original cards/modern_horizons_2.c:6025. */
int card_so_shiny(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;

  if (event == EVENT_UNTAP)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated(player, card, AL_STATIC);
      if (result == 0)
      {

        instance = get_card_instance(player, card);
        if ((-1 < instance->damage_target_player) && (-1 < instance->damage_target_card))
        {
          does_not_untap((int)instance->damage_target_player, instance->damage_target_card,
                         EVENT_UNTAP);
        }
      }
    }
    etb_ability(player, card, EVENT_UNTAP, RESOLVE_TRIGGER_MANDATORY);
  }
  else
  {
    etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (event == EVENT_ETB)
    {
      default_test_definition(&test, 0x47);
      test.type_flag = 6;
      result = check_battlefield_for_special_card(player, card, player, 0, &test);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        if ((-1 < instance->damage_target_player) && (-1 < instance->damage_target_card))
        {
          tap_card((int)instance->damage_target_player, instance->damage_target_card);
        }
        scry(player, 2);
      }
    }
  }
  result = disabling_aura(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x222b890; original cards/modern_horizons_2.c:6060. */
static int is_artifact_land(int iid, int unused, int unused2, int unused3)
{
  int result;

  result = is_what(-1, iid, 0x40);
  if (result == 0)
  {
    return 0;
  }
  result = is_what(-1, iid, 1);
  return (unsigned int)(result != 0);
}

/* ManalinkEh 2022: 0x223ee40; original cards/modern_horizons_2.c:6067. */
int card_sojourners_companion(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  affinity(player, card, event, 0x40, -1);
  if (event - EVENT_CAN_ACTIVATE_FROM_HAND < 2)
  {
    result = cycling(player, card, event, 2, 0, 0, 0, 0, 0);
    return result;
  }
  if (event != EVENT_RESOLVE_ACTIVATION_FROM_HAND)
  {
    return 0;
  }
  new_default_test_definition(&test, 0x47, "Select an artifact land card.");
  test.special_selection_function = is_artifact_land;
  new_global_tutor(player, player, 1, 1, 0, 1, &test);
  return 0;
}

/* ManalinkEh 2022: 0x2240000; original cards/modern_horizons_2.c:6450. */
int card_svyelun_of_sea_and_sky(int player, int card, event_t event)
{
  int merfolk_count;
  int is_merfolk;

  check_legend_rule(player, card, event);
  if (event == EVENT_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      merfolk_count = count_subtype(player, 0x47, 0x74);
      is_merfolk = has_subtype(player, card, SUBTYPE_MERFOLK);
      if ((int)((is_merfolk != 0) + 2) <= merfolk_count)
      {
        if (is_humiliated(player, card, AL_STATIC) == 0)
        {
          indestructible(player, card, EVENT_ABILITIES);
        }
      }
    }
    ward_granted(player, card, EVENT_ABILITIES, player, -1, TYPE_PERMANENT, SUBTYPE_MERFOLK, 2, 1,
                 0, 0, 0, 0, 0, 0);
    dat_ability(player, card, EVENT_ABILITIES, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    return 0;
  }
  ward_granted(player, card, event, player, -1, TYPE_PERMANENT, SUBTYPE_MERFOLK, 2, 1, 0, 0, 0, 0,
               0, 0);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  draw_cards(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x2240780; original cards/modern_horizons_2.c:6593. */
int card_tavern_scoundrel(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;

  if (event == EVENT_COIN_FLIP_WON_OR_LOST)
  {
    if (affected_card == 1)
    {
      generate_tokens_by_id(player, card, 0x4998, 2);
    }
  }
  else if ((((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
            (event == EVENT_RESOLVE_ACTIVATION)) ||
           (event == EVENT_CLEANUP))
  {
    new_default_test_definition(&test, 0x47, "Select another permanent to sacrifice.");
    test.not_me = 1;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      instance = get_card_instance(player, card);
      flip_a_coin(instance->parent_controller, instance->parent_card);
    }
    result = generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_UNTAPPED, 1,
                                                              0, 0, 0, 0, 0, 0, NULL, NULL,
                                                              TYPE_NONE, &test, SARV_0);
    return result;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2240a00; original cards/modern_horizons_2.c:6650. */
int card_territorial_kavu(int player, int card, event_t event)
{
  int choice;
  int count_or_result;
  int hand_count_or_grave_index;
  card_instance_t *instance;
  int player_00;
  int32_t targ_player;
  unsigned int grave_available;

  if (event - EVENT_POWER < 2)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      set_base_pt(count_domain(player, card));
    }
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    return 0;
  }
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }

  hand_count_or_grave_index = hand_count[player];
  count_or_result = count_graveyard(player);
  player_00 = 1 - player;
  grave_available = 0;
  if (count_or_result != 0 && !graveyard_has_shroud(player))
    grave_available = 1;
  else
  {
    count_or_result = count_graveyard(player_00);
    if (count_or_result != 0)
      grave_available = graveyard_has_shroud(player_00) == 0;
  }
  count_or_result = count_graveyard(player_00);
  choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\016", "\001\003",
                       "Discard & draw", (unsigned int)(0 < hand_count_or_grave_index), 10,
                       "Exile a card in a graveyard", grave_available,
                       ((0x13 < count_or_result) - 1 & 0xfffffff6) + 0xf, 0, 0, 0, 0);
  if (choice == 1)
  {
    if (0 < hand_count[player])
    {
      discard(player, 0, player);
      draw_cards(player, 1);
    }
  }
  else if (choice == 2)
  {
    test_definition_t test;

    hand_count_or_grave_index = count_graveyard(0);
    grave_available = 0;
    if (hand_count_or_grave_index != 0)
    {
      hand_count_or_grave_index = graveyard_has_shroud(0);
      grave_available = (unsigned int)(hand_count_or_grave_index == 0);
    }
    hand_count_or_grave_index = count_graveyard(1);
    if (((hand_count_or_grave_index == 0) ||
         (hand_count_or_grave_index = graveyard_has_shroud(1), hand_count_or_grave_index != 0)) ||
        (grave_available = grave_available | 2, grave_available != 3))
    {
      targ_player = (int)grave_available >> 1;
    }
    else
    {
      target_definition_t td1;

      default_target_definition(player, card, &td1, 2);
      td1.zone = 4096;
      instance = get_card_instance(player, card);
      if (new_pick_target(&td1, "Select the owner of the graveyard you want to look.", 0, 0x80) ==
          0)
      {
        return 0;
      }
      targ_player = instance->targets[0].player;
      instance->number_of_targets = '\0';
    }
    new_default_test_definition(&test, 0xff, "Select a card to exile.");
    hand_count_or_grave_index = new_select_a_card(player, targ_player, 2, 0, 1, -1, &test);
    if (hand_count_or_grave_index != -1)
    {
      rfg_card_from_grave(targ_player, hand_count_or_grave_index);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2240d80; original cards/modern_horizons_2.c:6709. */
int card_the_underworld_cookbook(int player, int card, event_t event)
{
  int choice;
  card_instance_t *instance;
  int food_available_or_grave_index;
  unsigned int food_available;
  unsigned int return_available;
  test_definition_t test;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_test_definition(&test, 2);
    if (event == EVENT_CAN_ACTIVATE)
    {
      food_available = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x21, 0, 0, 0, 0,
                                                 0, 0, 0, NULL, NULL);
      return_available = generic_activated_ability_with_target_in_grave(
          player, card, EVENT_CAN_ACTIVATE, GAA_SACRIFICE_ME | GAA_UNTAPPED, 4, 0, 0, 0, 0, 0, 0,
          NULL, NULL, TYPE_CREATURE, &test, 8);
      return return_available | food_available;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->mode = '\0';
      food_available_or_grave_index = generic_activated_ability(
          player, card, EVENT_CAN_ACTIVATE, 0x21, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
      choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\033", "\001\016",
                           "Create a Food token", food_available_or_grave_index, 2,
                           "Creature from grave to hand",
                           generic_activated_ability_with_target_in_grave(
                               player, card, EVENT_CAN_ACTIVATE, GAA_SACRIFICE_ME | GAA_UNTAPPED, 4,
                               0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_CREATURE, &test, 8),
                           1, 0, 0, 0, 0);
      if (choice == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (choice != 1)
      {
        if (choice != 2)
        {
          return 0;
        }
        generic_activated_ability_with_target_in_grave(
            player, card, EVENT_ACTIVATE, GAA_SACRIFICE_ME | GAA_UNTAPPED, 4, 0, 0, 0, 0, 0, 0,
            NULL, NULL, TYPE_CREATURE, &test, 8);
        return 0;
      }
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0x21, 0, 0, 0, 0, 0, 0, 0, NULL,
                                NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      ai_modifier = ai_modifier + ((life[player] < 6) - 1 & 0xfffffff2) + 0x18;
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    default_test_definition(&test, 2);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->mode == '\x01')
    {
      generate_token_by_id(player, card, 0x5077);
      return 0;
    }

    if ((instance->mode == '\x02') &&
        (food_available_or_grave_index = validate_target_from_grave_source(player, card, player, 0),
         food_available_or_grave_index != -1))
    {

      add_card_to_hand(player, graveyard_ptr_mutable[player][food_available_or_grave_index]);
      remove_card_from_grave(player, food_available_or_grave_index);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2241300; original cards/modern_horizons_2.c:6793. */
int card_thraben_watcher(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  vigilance(player, card, event);
  if (event - EVENT_POWER < 3)
  {
    instance = in_play(player, card);
    if ((instance != NULL) && (affected_card_controller == player))
    {
      result = is_humiliated(player, card, AL_STATIC);
      if (result == 0)
      {
        result = is_token(affected_card_controller, affected_card);
        if (result == 0)
        {
          if ((event == EVENT_POWER) || (event == EVENT_TOUGHNESS))
          {
            event_result = event_result + 1;
          }
          else if (event == EVENT_ABILITIES)
          {
            vigilance(affected_card_controller, affected_card, EVENT_ABILITIES);
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x222bc00; original cards/modern_horizons_2.c:6845. */
static int tide_shaper_effect(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_CHANGE_TYPE)
  {

    instance = get_card_instance(player, card);
    if ((instance->damage_target_card == affected_card) &&
        (instance->damage_target_player == affected_card_controller))
    {
      result = get_hacked_subtype(player, card, 0x5002);
      event_result = land_into_new_basic_land_type(event_result, result);
    }
  }
  else if (event == EVENT_STATIC_EFFECTS)
  {

    instance = get_card_instance(player, card);
    result = instance->targets[0].player;
    if (-1 < result)
    {
      instance = in_play(result, instance->targets[0].card);
      if (instance == NULL)
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2241530; original cards/modern_horizons_2.c:6863. */
int card_tide_shaper(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  kicker(player, card, event, 1, 0, 0, 0, 0, 0);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = kicked(player, card);
    if (result != 0)
    {
      default_target_definition(player, card, &td, 1);
      if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
      {
        td.required_subtype = get_hacked_subtype(player, card, 0x5002);
        td.special = 0x1000;
      }
      td.allow_cancel = 0;
      result = can_target(&td);
      if (result != 0)
      {
        result = new_pick_target(&td, "TARGET_LAND", 0, 0);
        if (result != 0)
        {

          instance = get_card_instance(player, card);
          result = create_targetted_legacy_effect(player, card, tide_shaper_effect,
                                                  instance->targets[0].player,
                                                  instance->targets[0].card);
          instance = get_card_instance(player, result);
          instance->targets[0].player = player;
          instance->targets[0].card = card;
          instance->number_of_targets = '\x01';
        }
      }
    }
  }
  instance = in_play(player, card);
  if (instance != NULL)
  {
    result = is_humiliated(player, card, AL_STATIC);
    if ((((result == 0) && (event - EVENT_POWER < 2)) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      result = check_battlefield_for_subtype(1 - player, 0x47, 0x5002);
      if (result != 0)
      {
        event_result = event_result + 1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x22418c0; original cards/modern_horizons_2.c:6936. */
int card_tireless_provisioner(int player, int card, event_t event)
{
  int result;

  landfall(player, card, event);
  if (event == EVENT_ETB)
  {

    result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\003",
                         "Food token", 1, ((life[player] < 6) - 1 & 0xffffffec) + 0x1e,
                         "Treasure token", 1, 0x14, 0, 0, 0, 0);
    generate_token_by_id(player, card, ((result == 1) - 1 & 0xfffff921) + 0x5077);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2242cb0; original cards/modern_horizons_2.c:7323. */
int card_vedalken_infiltrator(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  unblockable(player, card, event);
  if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
  {
    result = metalcraft(player, card);
    if (result != 0)
    {
      instance = in_play(player, card);
      if (instance != NULL)
      {
        event_result = event_result + 1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2243b30; original cards/modern_horizons_2.c:7619. */
int card_yavimaya_cradle_of_growth(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    play_land_sound_effect(player, card);
  }
  all_lands_are_basiclandtype(player, card, event, 2, 3, 0x5001);
  result = plain_mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2243f30; original cards/modern_horizons_2.c:7713. */
int card_zabaz_the_glimmerwasp(int player, int card, event_t event)
{
  int choice;
  card_instance_t *instance;
  int destroy_available_or_flags;
  int flying_available;
  target_definition_t *target;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 0, 0, 0, 0, 1, 0, 0, &td,
                                    NULL) != 0)
      {
        return 1;
      }
      return generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 0, 1, 0,
                                       NULL, NULL);
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->mode = '\0';
      destroy_available_or_flags = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 0,
                                                             0, 0, 0, 1, 0, 0, &td, NULL);
      flying_available = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0,
                                                   0, 1, 0, NULL, NULL);
      choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\033",
                           "Destroy your artifact", destroy_available_or_flags, 10,
                           "Zabaz gains flying", flying_available,
                           ((current_phase < 0x18) - 1 & 0xfffffff1) + 0x14, 0, 0, 0, 0);
      if (choice == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (choice == 1)
      {
        generic_activated_ability(player, card, EVENT_ACTIVATE, 6, 0, 0, 0, 0, 1, 0, 0, &td,
                                  "Select target artifact you control.");
        if (spell_fizzled != 1)
        {
          destroy_available_or_flags =
              check_special_flags(instance->targets[0].player, instance->targets[0].card, 0x8000);
          ai_modifier =
              ai_modifier + (-(unsigned int)(destroy_available_or_flags == 0) & 0xffffffd0) + 0x18;
        }
      }
      else if (choice == 2)
      {
        generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 0, 0, 0, 0, 0, 1, 0, NULL, NULL);
      }
      return modular(player, card, event, 1);
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      return modular(player, card, event, 1);
    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
  }
  target = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->mode == '\x01')
    {
      if (valid_target(target) != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
    }
    else if (instance->mode == '\x02')
    {
      pump_ability_until_eot_no_repeat(player, card, instance->parent_controller,
                                       instance->parent_card, 0x20, 0);
    }
  }
  return modular(player, card, event, 1);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int magus_of_the_bridge_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2238d90; original cards/modern_horizons_2.c:4098. */
int card_lightning_spear(int player, int card, event_t event)
{
  enum
  {
    CHOICE_EQUIP = 1,
    CHOICE_SAC_DAMAGE = 2
  };

  int choice;
  card_instance_t *instance;
  int equip_available;
  target_definition_t *target;
  target_definition_t td;

  attached_gets_pt(player, card, event, 1, 0);
  attached_gets_kw(player, card, event, KEYWORD_TRAMPLE, SP_KEYWORD_0);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {

      equip_available = basic_equipment(player, card, EVENT_CAN_ACTIVATE, 1);
      if (equip_available == 0)
      {
        return generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xc, 2, 0, 0, 0, 1, 0, 0,
                                         &td, NULL);
      }
      return equip_available;
    }
    if (event == EVENT_ACTIVATE)
    {

      equip_available = can_activate_basic_equipment(player, card, EVENT_ACTIVATE, 1);
      choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\004", "\001\016",
                           "\001\033", "Equip", equip_available, 5, "Sac & damage",
                           generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xc, 2, 0, 0,
                                                     0, 1, 0, 0, &td, NULL),
                           10, 0, 0, 0, 0);
      if (choice == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (choice != CHOICE_EQUIP)
      {
        if (choice != CHOICE_SAC_DAMAGE)
        {
          return 0;
        }
        generic_activated_ability(player, card, EVENT_ACTIVATE, 0xc, 2, 0, 0, 0, 1, 0, 0, &td,
                                  "TARGET_ANY");
        return 0;
      }
      return basic_equipment(player, card, EVENT_ACTIVATE, 1);
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    instance = get_card_instance(player, card);
  }
  target = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (instance->mode == '\x01')
    {
      return basic_equipment(player, card, EVENT_RESOLVE_ACTIVATION, 1);
    }
    if (instance->mode == '\x02')
    {
      td.illegal_abilities = instance->targets[1].player;
      if (valid_target(target) != 0)
      {
        damage_target0(player, card, 3);
        return 0;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x222abc0; original cards/modern_horizons_2.c:4325. */
static int magus_of_the_bridge_effect(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  card_instance_t *dying;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      if ((-1 < instance->damage_target_player) &&
          (result = get_id((int)instance->damage_target_player, instance->damage_target_card),
           result != 0x5a2e))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    instance = get_card_instance(player, card);
    if ((-1 < instance->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, 0x4700), result != 0))
    {
      return 0;
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if ((-1 < instance->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, event), result != 0))
    {
      return 0;
    }
    if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {
      instance = get_card_instance(player, card);
      dying = in_play(affected_card_controller, affected_card);

      if ((dying != NULL) && (dying = get_card_instance(affected_card_controller, affected_card),
                              dying->kill_code != '\0'))
      {
        test_definition_t this_test;
        default_test_definition(&this_test, 2);
        this_test.type_flag = 3;

        if ((-1 < instance->damage_target_player) &&
            ((instance->damage_target_card == affected_card &&
              (instance->damage_target_player == affected_card_controller))))
        {
          instance->damage_target_card = -1;
          instance->damage_target_player = -1;
          remove_status(player, card, 0x10000);
          if (instance->targets[0xb].player < 0)
          {
            instance->targets[0xb].player = 0;
          }
          *(uint8_t *)((int)&instance->targets[0xb].player + 2) = 1;
        }
        count_for_gfp_ability(player, card, 0x77, player, 0, &this_test);
      }
    }
  }
  if (!check_status(player, card, 0x10000))
  {

    result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (result != 0)
    {
      token_generation_t token;

      default_token_definition(player, card, 0x49ac, &token);
      token.tou = 2;
      token.pow = 2;
      token.color_forced = '\x02';
      token.qty = (char)result;
      generate_token(&token);
      kill_card(player, card, KILL_EXILE);
    }
  }
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2239980; original cards/modern_horizons_2.c:4387. */
int card_magus_of_the_bridge(int player, int card, event_t event)
{
  card_instance_t *instance;
  int deaths_or_legacy;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      bool found = false;
      if (0 < active_cards_count[player])
      {
        deaths_or_legacy = 0;
        do
        {
          instance = in_play(player, deaths_or_legacy);

          if ((((instance != NULL) && (is_what(player, deaths_or_legacy, 0x80) != 0)) &&
               (instance = get_card_instance(player, deaths_or_legacy),
                instance->info_slot_fn == magus_of_the_bridge_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
          {
            found = true;
            break;
          }
          deaths_or_legacy = deaths_or_legacy + 1;
        } while (deaths_or_legacy < active_cards_count[player]);
      }

      if (!found)
      {
        deaths_or_legacy =
            create_targetted_legacy_effect(player, card, magus_of_the_bridge_effect, player, card);
        instance = get_card_instance(player, deaths_or_legacy);
        instance->targets[0].player = player;
        instance->targets[0].card = card;
        instance->number_of_targets = '\x01';
        add_status(player, deaths_or_legacy, 0x10000);
      }
    }
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    test_definition_t this_test;
    default_test_definition(&this_test, 2);
    this_test.type_flag = 3;
    count_for_gfp_ability_and_store_values(player, card, 0x77, 2, 0, &this_test, 0x200, 0);
  }
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (deaths_or_legacy = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY),
       deaths_or_legacy != 0))
  {
    token_generation_t token;

    default_token_definition(player, card, 0x49ac, &token);
    token.qty = (char)deaths_or_legacy;
    token.tou = 2;
    token.pow = 2;
    token.color_forced = '\x02';
    generate_token(&token);
    if ((char)((unsigned int)deaths_or_legacy >> 8) != '\0')
    {
      kill_card(player, card, KILL_EXILE);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222ce00; original cards/modern_horizons_2.c:445. */
int card_arcbound_shikari(int player, int card, event_t event)
{
  int c;
  card_instance_t *instance;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

  if ((event == EVENT_ETB) && (0 < active_cards_count[player]))
  {
    c = 0;
    do
    {
      if (card != c)
      {
        instance = in_play(player, c);
        if (instance != NULL)
        {
          if (is_what(player, c, 0x40) != 0)
          {
            if (is_what(player, c, 2) != 0)
            {
              add_counters(player, card, player, c, COUNTER_P1_P1, 1);
            }
          }
        }
      }
      c = c + 1;
    } while (c < active_cards_count[player]);
  }
  return modular(player, card, event, 2);
}

/* ManalinkEh 2022: 0x222cee0; original cards/modern_horizons_2.c:465. */
int card_arcbound_slasher(int player, int card, event_t event)
{
  int result;

  riot(player, card, event);
  result = modular(player, card, event, 4);
  return result;
}

/* ManalinkEh 2022: 0x222d290; original cards/modern_horizons_2.c:560. */
int card_arcus_acolyte(int player, int card, event_t event)
{
  int c;
  int choice;
  uint8_t selected_mode;
  card_instance_t *creature;
  int self_available;
  int eligible;
  uint32_t another_available;
  unsigned int self_result;
  int index_or_cost;
  card_instance_t *instance;
  target_definition_t td;

  lifelink(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.illegal_state = 0x1001;
    td.special = 0xc000;
    td.extra = (int32_t)doesnt_have_p1_p1_counter_and_can_be_activated;
    td.illegal_abilities = 0;
    td.preferred_controller = player;
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {

      if (can_sorcery_be_played(player, EVENT_CAN_ACTIVATE) == 0)
      {
        return 0;
      }

      self_available = has_mana_hybrid(
          player, 1, 3, 5, get_cost_mod_for_activated_abilities(player, card, 0, 0, 0, 1, 0, 0));
      self_result = 0;
      if (self_available != 0)
      {
        self_result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x801, 0, 0, 0, 0,
                                                0, 0, 0, NULL, NULL);
      }
      if (0 < active_cards_count[player])
      {
        c = 0;
        do
        {
          creature = in_play(player, c);
          if ((((creature != NULL) &&
                (index_or_cost = is_what(player, c, 2), index_or_cost != 0)) &&
               (index_or_cost = check_state(player, c, 0x20010), index_or_cost == 0)) &&
              (index_or_cost = is_humiliated(player, c, AL_NM_ACT), index_or_cost == 0))
          {
            index_or_cost = has_mana_hybrid(
                player, 1, 3, 5, get_cost_mod_for_activated_abilities(player, c, 0, 0, 0, 1, 0, 0));
            if (index_or_cost != 0)
            {
              return self_result | 1;
            }
          }
          c = c + 1;
        } while (c < active_cards_count[player]);
        return self_result;
      }
      return self_result;
    }
    if (event == EVENT_ACTIVATE)
    {

      index_or_cost = 0;
      instance->number_of_targets = '\0';
      instance->mode = '\0';
      self_available = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x801, 0, 0, 0,
                                                 0, 0, 0, 0, NULL, NULL);
      another_available = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          creature = in_play(player, index_or_cost);
          if (((creature != NULL) &&
               (eligible = is_what(player, index_or_cost, 2), eligible != 0)) &&
              ((eligible = check_state(player, index_or_cost, 0x20010),
                eligible == 0 &&
                    (eligible = is_humiliated(player, index_or_cost, AL_NM_ACT), eligible == 0))))
          {

            eligible = has_mana_hybrid(
                player, 1, 3, 5,
                get_cost_mod_for_activated_abilities(player, index_or_cost, 0, 0, 0, 1, 0, 0));
            if (eligible != 0)
            {
              another_available = 1;
              break;
            }
          }
          index_or_cost = index_or_cost + 1;
        } while (index_or_cost < active_cards_count[player]);
      }
      choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\016",
                           "Outlast", self_available, 10, "Another creature outlasts",
                           another_available, 0x14, 0, 0, 0, 0);
      if (choice == 1)
      {

        index_or_cost = get_cost_mod_for_activated_abilities(player, card, 0, 0, 0, 1, 0, 0);
        if (charge_mana_hybrid(player, card, 1, 3, 5, index_or_cost) != 0)
        {
          generic_activated_ability(player, card, EVENT_ACTIVATE, 0x801, 0, 0, 0, 0, 0, 0, 0, NULL,
                                    NULL);
        }
      }
      else if ((choice == 2) &&
               (index_or_cost = new_pick_target(
                    &td,
                    "Select anothe untapped, nonsick creature you control without +1/+1 counters.",
                    0, 0x80),
                index_or_cost != 0))
      {

        index_or_cost = get_cost_mod_for_activated_abilities(
            instance->targets[0].player, instance->targets[0].card, 0, 0, 0, 1, 0, 0);
        if (charge_mana_hybrid(player, card, 1, 3, 5, index_or_cost) != 0)
        {
          generic_activated_ability(instance->targets[0].player, instance->targets[0].card,
                                    EVENT_ACTIVATE, 0x801, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
          instance->number_of_targets = '\0';
        }
      }
      if (spell_fizzled == 1)
      {
        return 0;
      }
      instance->mode = (uint8_t)choice;
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.illegal_state = 0x1001;
    td.special = 0xc000;
    td.extra = (int32_t)doesnt_have_p1_p1_counter_and_can_be_activated;
    td.illegal_abilities = 0;
    td.preferred_controller = player;
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    selected_mode = instance->mode;
    if (selected_mode == '\x01')
    {
      creature = in_play(instance->parent_controller, instance->parent_card);
      if (creature != NULL)
      {
        add_counters(player, card, instance->parent_controller, instance->parent_card,
                     COUNTER_P1_P1, 1);
      }
      selected_mode = instance->mode;
    }
    if ((selected_mode == '\x02') &&
        (creature = in_play(instance->targets[0].player, instance->targets[0].card),
         creature != NULL))
    {
      add_counters(player, card, instance->targets[0].player, instance->targets[0].card,
                   COUNTER_P1_P1, 1);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x222f3f0; original cards/modern_horizons_2.c:1146. */
int card_breathless_knight(int player, int card, event_t event)
{

  lifelink(player, card, event);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    if (check_special_flags(trigger_cause_controller, trigger_cause, 0x10000000) ||
        check_special_flags3(trigger_cause_controller, trigger_cause, 1))
      etb_ability_global(player, event, 2, TYPE_CREATURE, NULL, player, card);
  }
  if (event != EVENT_ETB)
  {
    return 0;
  }
  add_counters(player, card, player, card, COUNTER_P1_P1, 1);
  return 0;
}

/* ManalinkEh 2022: 0x22337d0; original cards/modern_horizons_2.c:2440. */
int card_duskshell_crawler(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    if (event == EVENT_ABILITIES)
    {
      result = is_what(affected_card_controller, affected_card, 2);
      if (result != 0)
      {
        result = is_humiliated(player, card, AL_STATIC);
        if (result == 0)
        {
          result = count_counters(affected_card_controller, affected_card, COUNTER_P1_P1);
          if (0 < result)
          {
            event_result = event_result | 0x80;
          }
        }
      }
    }
    return 0;
  }

  default_target_definition(player, card, &td, 2);
  td.allow_cancel = 0;
  td.preferred_controller = player;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = pick_target(&td, "TARGET_CREATURE");
    if (result != 0)
    {
      add_counters(player, card, instance->targets[0].player, instance->targets[0].card,
                               COUNTER_P1_P1, 1);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x223a620; original cards/modern_horizons_2.c:4671. */
int card_myr_scrapling(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      add_counters(player, card, instance->targets[0].player, instance->targets[0].card,
                               COUNTER_P1_P1, 1);
    }
  }
  result = generic_activated_ability(player, card, event, 0xc, 0, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x223bf20; original cards/modern_horizons_2.c:5201. */
int card_ravenous_squirrel(int player, int card, event_t event)
{
  card_instance_t *instance;
  test_definition_t this_test;

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {

    instance = in_play(affected_card_controller, affected_card);
    if ((((instance != NULL) && (affected_card_controller == player)) &&
         (is_what(player, affected_card, 0x42) != 0)) &&
        ((instance->kill_code == '\x03' &&
          (check_special_flags(affected_card_controller, affected_card, 0x800) == 0))))
    {

      instance = get_card_instance(player, card);
      if (instance->targets[0xb].player < 0)
      {
        instance->targets[0xb].player = 0;
      }
      *(char *)&instance->targets[0xb].player = (char)instance->targets[0xb].player + '\x01';
    }
  }
  instance = get_card_instance(player, card);
  if (instance->kill_code == 0)
  {
    unsigned int sacrifices = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (sacrifices != 0)
      add_counters(player, card, player, card, COUNTER_P1_P1, sacrifices & 0xff);
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
    return 0;
  if (((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE)) &&
      ((event != EVENT_RESOLVE_ACTIVATION && (event != EVENT_CLEANUP))))
  {
    return 0;
  }
  new_default_test_definition(&this_test, 0x42, "Select a creature or an artifact to sacrifice.");
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_cards(player, 1);
    gain_life(player, 1);
  }
  return generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_NONE, 1, 1, 0, 1,
                                                          0, 0, 0, NULL, NULL, TYPE_NONE,
                                                          &this_test, SARV_0);
}

/* ManalinkEh 2022: 0x2240320; original cards/modern_horizons_2.c:6497. */
int card_sword_of_hearth_and_home(int player, int card, event_t event)
{
  unsigned int first_protection;
  unsigned int second_protection;
  int result;
  card_instance_t *instance;
  int iid;
  int card_00;
  int damage_triggers;
  target_definition_t td;
  test_definition_t this_test;

  damage_triggers = equipped_creature_deals_damage(
      player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (damage_triggers != 0)
  {

    default_target_definition(player, card, &td, 2);
    td.special = 0x8000;
    td.extra = (int32_t)target_you_own;
    td.allow_cancel = 2;
    new_default_test_definition(&this_test, 1, "Select a basic land card.");
    this_test.subtype = 0x2000;
    instance = get_card_instance(player, card);
    if (0 < damage_triggers)
    {
      do
      {
        instance->number_of_targets = '\0';
        result = can_target(&td);
        if ((result == 0) ||
            (result = new_pick_target(&td, "Select up to one target creature you own.", 0, 0x80),
             result == 0))
        {

          result = new_global_tutor(player, player, 1, 1, 0, 5, &this_test);
        }
        else
        {
          iid =
              get_original_internal_card_id(instance->targets[0].player, instance->targets[0].card);
          action_on_target(player, card, 0, ACT_KILL_REMOVE);
          result = new_global_tutor(player, player, 1, 1, 0, 5, &this_test);
          if (-1 < iid)
          {
            card_00 = add_card_to_hand(player, iid);
            remove_card_from_rfg(player, iid);
            set_special_flags(player, card_00, -0x80000000);
            if (-1 < card_00)
            {
              put_into_play(player, card_00);
            }
          }
        }
        if (-1 < result)
        {
          put_into_play(player, result);
        }
        damage_triggers = damage_triggers + -1;
      } while (damage_triggers != 0);
    }
  }
  first_protection = get_sleighted_protection(player, card, 0x8000);
  second_protection = get_sleighted_protection(player, card, 0x2000);
  return vanilla_equipment(player, card, event, 2, 2, 2, first_protection | second_protection, 0);
}

/* ManalinkEh 2022: 0x2241d50; original cards/modern_horizons_2.c:7021. */
int card_tourach_dread_cantor(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  kicker(player, card, event, 0, 2, 0, 0, 0, 0);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = kicked(player, card);
    if (result != 0)
    {
      result = opponent_is_valid_target(player, card);
      if (result != 0)
      {
        new_multidiscard(1 - player, 2, 3, player);
      }
    }
  }
  if (((trigger_condition == 0xd1) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = discard_trigger(player, card, event, 1 - player, 2, 0);
    if (result != 0)
    {
      add_counters(player, card, player, card, COUNTER_P1_P1, 1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2241f00; original cards/modern_horizons_2.c:7057. */
int card_unbounded_potential(int player, int card, event_t event)
{
  int i;
  unsigned char selected_mode;
  card_instance_t *instance;
  int result_or_choice;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    get_card_instance(player, card);
    return basic_spell(player, card, EVENT_CAN_CAST);
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        result_or_choice = check_special_flags2(player, card, -0x80000000);
        if (result_or_choice == 0)
        {

          instance->mode = '\0';
          result_or_choice = dialog_impl(
              player, card, EVENT_CAST_SPELL, "\001\002", "\001\033",
              "Put a +1/+1 counter on each of up to two target creatures", 1, 0x14, "Proliferate",
              1, 10, "Entwine", has_mana_multi(player, 3, 0, 0, 0, 0, 1), 0x1e, 0, 0, 0, 0);
          if (result_or_choice == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if ((result_or_choice == 3) &&
              (charge_mana_multi(player, 3, 0, 0, 0, 0, 1), spell_fizzled == 1))
          {
            return 0;
          }
        }
        if (((instance->mode & 1) != 0) &&
            (result_or_choice = new_pick_target(&td, "TARGET_CREATURE", 0, 0),
             result_or_choice != 0))
        {
          state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
          new_pick_target(&td, "TARGET_CREATURE", 1, 0);
          state_untargettable(instance->targets[0].player, instance->targets[0].card, 0);
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      selected_mode = instance->mode;

      if (((selected_mode & 1) != 0) && (instance->number_of_targets != '\0'))
      {
        i = 0;
        do
        {
          if (validate_target(player, card, &td, i) == 0)
          {
            i = i + 1;
            continue;
          }
          add_counters(player, card, instance->targets[i].player, instance->targets[i].card,
                       COUNTER_P1_P1, 1);
          i = i + 1;
        } while (i < (int)(unsigned int)instance->number_of_targets);
        selected_mode = instance->mode;
      }
      if ((selected_mode & 2) != 0)
      {
        proliferate(player, card);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2243980; original cards/modern_horizons_2.c:7581. */
int card_wrens_run_hydra(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    result = card_ivy_elemental(player, card, event);
    return result;
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    result = has_mana_multi(player, 0, 0, 0, 2, 0, 0);
    if (result != 0)
    {
      result = can_target(&td);
      return result;
    }
  }
  else if (event == EVENT_ACTIVATE_FROM_HAND)
  {
    charge_mana_multi(player, -1, 0, 0, 2, 0, 0);
    if ((spell_fizzled != 1) && (result = pick_target(&td, "TARGET_CREATURE"), result != 0))
    {
      discard_card(player, card);
      return 1;
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION_FROM_HAND) && (spell_fizzled != 1))
  {
    add_counters(player, card, instance->targets[0].player, instance->targets[0].card,
                             COUNTER_P1_P1, x_value);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int evoke_with_cards_in_hand(int player, int card, event_t event, int color_test);

/* ManalinkEh 2022: 0x222b4b0; original cards/modern_horizons_2.c:4. */
static int evoke_with_cards_in_hand(int player, int card, event_t event, int color_test)
{
  int choice;
  int32_t iid;
  card_instance_t *instance;
  int index_or_csvid;
  card_instance_t *hand_card;
  unsigned int hand_card_color;
  int card_00;
  test_definition_t this_test;
  int fake_hand[100][2];

  if (event == EVENT_MODIFY_COST)
  {

    instance = get_card_instance(player, card);
    if (0 < active_cards_count[player])
    {
      index_or_csvid = 0;
      do
      {
        hand_card = in_hand(player, index_or_csvid);
        if (((card != index_or_csvid) && (hand_card != NULL)) &&
            (hand_card_color = get_color(player, index_or_csvid),
             (color_test & hand_card_color) != 0))
        {
          null_casting_cost(player, card);
          instance->eot_toughness0 = '\x01';
          return 0;
        }
        index_or_csvid = index_or_csvid + 1;
      } while (index_or_csvid < active_cards_count[player]);
    }
    instance->eot_toughness0 = '\0';
    return 0;
  }
  if (event != EVENT_CAST_SPELL)
  {
    return 0;
  }
  if (card != affected_card)
  {
    return 0;
  }
  if (player != affected_card_controller)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  if (played_for_free(player, card) != 0)
  {
    return 0;
  }
  if (is_token(player, card) != 0)
  {
    return 0;
  }
  if (instance->eot_toughness0 == '\x01')
  {

    index_or_csvid = get_id(player, card);
    if (has_mana_to_cast_id(player, EVENT_CAST_SPELL, index_or_csvid) != 0)
    {
      choice = do_dialog(player, player, card, -1, -1, " Play normally\n Evoke\n Cancel", 0);
      if (choice == 0)
      {
        charge_mana_from_id(player, card, EVENT_CAST_SPELL, get_id(player, card));
        return 0;
      }
      if (choice != 1)
      {
        spell_fizzled = 1;
        return 0;
      }
    }

    card_00 = 0;
    index_or_csvid = 0;
    if (0 < active_cards_count[player])
    {
      do
      {
        hand_card = in_hand(player, card_00);
        if (((card != card_00) && (hand_card != NULL)) &&
            (hand_card_color = get_color(player, card_00), (color_test & hand_card_color) != 0))
        {
          hand_card = get_card_instance(player, card_00);
          iid = hand_card->internal_card_id;
          fake_hand[1][index_or_csvid] = card_00;
          fake_hand[0][index_or_csvid] = iid;
          index_or_csvid = index_or_csvid + 1;
        }
        card_00 = card_00 + 1;
      } while (card_00 < active_cards_count[player]);
    }
    new_default_test_definition(&this_test, 0xff, "Select a card to exile.");
    index_or_csvid =
        select_card_from_zone(player, player, fake_hand[0], index_or_csvid, 0, 3, -1, &this_test);
    if (index_or_csvid < 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    rfg_card_in_hand(player, fake_hand[1][index_or_csvid]);
    instance->eot_toughness1 = '\x01';
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2233bd0; original cards/modern_horizons_2.c:2513. */
int card_endurance(int player, int card, event_t event)
{
  int argument2;
  int target_or_valid;
  card_instance_t *instance;
  int position;
  int previous_position;
  int grave_count;
  target_definition_t td;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  evoke_with_cards_in_hand(player, card, event, 8);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    instance = get_card_instance(player, card);
    target_or_valid = can_target(&td);
    if ((target_or_valid != 0) &&
        (target_or_valid = pick_target(&td, "TARGET_PLAYER"), target_or_valid != 0))
    {

      target_or_valid = instance->targets[0].player;
      grave_count = count_graveyard(target_or_valid);
      position = grave_count + -1;
      if (position < 0)
      {
        grave_count = 0;
      }
      else
      {
        do
        {

          argument2 = graveyard_ptr_mutable[target_or_valid][position];
          previous_position = position + -1;
          remove_card_from_grave(target_or_valid, position);
          raw_put_iid_on_top_of_deck(target_or_valid, argument2);
          position = previous_position;
        } while (previous_position != -1);
      }
      put_top_x_on_bottom_in_random_order(target_or_valid, grave_count);
    }
    instance->number_of_targets = '\0';
    if (evoked(player, card) != 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  return flash(player, card, event);
}

/* ManalinkEh 2022: 0x2235950; original cards/modern_horizons_2.c:3095. */
int card_fury2(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x102);
    td.allow_cancel = 0;
    target_and_divide_damage(player, card, &td, "TARGET_CREATURE_OR_PLANESWALKER", 4);
    instance = get_card_instance(player, card);
    instance->number_of_targets = '\0';
    result = evoked(player, card);
    if (result != 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = evoke_with_cards_in_hand(player, card, event, 0x10);
  return result;
}

/* ManalinkEh 2022: 0x2237770; original cards/modern_horizons_2.c:3633. */
int card_grief2(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  int card_00;
  target_definition_t td;
  test_definition_t test;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "TARGET_PLAYER", 0, 0);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        result = instance->targets[0].player;
        new_default_test_definition(&test, 1, "Select a nonland card.");
        test.type_flag = 1;
        card_00 = new_select_a_card(player, result, 4, 1, 1, -1, &test);
        if (card_00 != -1)
        {
          new_discard_card(result, card_00, player, 2);
        }
      }
    }
    result = evoked(player, card);
    if (result != 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = evoke_with_cards_in_hand(player, card, event, 2);
  return result;
}

/* ManalinkEh 2022: 0x2238c50; original cards/modern_horizons_2.c:4069. */
int card_legion_vanguard(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      explore(player, card, EVENT_RESOLVE_ACTIVATION);
    }
  }
  new_default_test_definition(&test, 2, "Select another creature to sacrifice.");
  test.not_me = 1;
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_NONE, 1, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x2239140; original cards/modern_horizons_2.c:4165. */
int card_liquimetal_torque(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      default_target_definition(player, card, &td, 0x47);
      td.illegal_type = 1;
      td.preferred_controller = player;
      instance = get_card_instance(player, card);

      instance->number_of_targets = '\0';
      instance->mode = '\0';
      result = paying_mana();
      if (((result != 0) || (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5,
                                                                2, 0, 0, 0, 0, 0, 0, &td, NULL),
                             result == 0)) ||
          (result = do_dialog(player, player, card, -1, -1,
                              " Produce mana\n Turn nonland permanent into artifact\n Cancel", 1),
           result == 0))
      {
        result = mana_producer(player, card, EVENT_ACTIVATE);
        return result;
      }
      if (result == 1)
      {
        add_state(player, card, 0x10);
        generic_activated_ability(player, card, EVENT_ACTIVATE, 7, 1, 0, 0, 0, 0, 0, 0, &td,
                                  "Select target nonland permanent.");
        if (spell_fizzled != 1)
        {
          result = is_what(instance->targets[0].player, instance->targets[0].card, 0x40);
          if (result != 0)
          {
            ai_modifier = ai_modifier + -100;
          }
          instance->mode = '\x01';
          return 0;
        }
        remove_state(player, card, 0x10);
        return 0;
      }
      if (result != 2)
      {
        return 0;
      }
      spell_fizzled = 1;
      return 0;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      default_target_definition(player, card, &td, 0x47);
      td.illegal_type = 1;
      td.preferred_controller = player;
      instance = get_card_instance(player, card);
      if ((event == EVENT_RESOLVE_ACTIVATION) &&
          ((instance->mode == '\x01' && (result = valid_target(&td), result != 0))))
      {
        turn_into_artifact(player, card, instance->targets[0].player, instance->targets[0].card, 1);
        return 0;
      }
      return 0;
    }
  }
  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x223f000; original cards/modern_horizons_2.c:6102. */
int card_solitude(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event != EVENT_CAN_CAST)
  {
    lifelink(player, card, event);
    etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (event == EVENT_ETB)
    {

      default_target_definition(player, card, &td, 2);
      td.allow_cancel = 2;
      instance = get_card_instance(player, card);
      result = can_target(&td);
      if (result != 0)
      {
        result = new_pick_target(&td, "TARGET_CREATURE", 0, 0);
        if (result != 0)
        {

          result = get_power(instance->targets[0].player, instance->targets[0].card);
          kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
          gain_life(instance->targets[0].player, result);
        }
      }
      instance->number_of_targets = '\0';
      result = evoked(player, card);
      if (result != 0)
      {
        kill_card(player, card, KILL_SACRIFICE);
      }
    }
    result = evoke_with_cards_in_hand(player, card, event, 0x20);
    return result;
  }
  result = flash(player, card, EVENT_CAN_CAST);
  return result;
}

/* ManalinkEh 2022: 0x223f360; original cards/modern_horizons_2.c:6188. */
int card_squirrel_sanctuary(int player, int card, event_t event)
{
  int result;
  unsigned int deaths;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    generate_token_by_id(player, card, 0x4991);
  }
  else if (((event == EVENT_GRAVEYARD_FROM_PLAY) && (affected_card_controller == player)) &&
           (result = is_token(player, affected_card), result == 0))
  {
    count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
  }
  result = duh_mode(player);
  deaths = resolve_gfp_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (deaths != 0)
  {

    deaths = deaths & 0xff;
    while (true)
    {

      if (((deaths == 0) || (result = has_mana(player, 7, 1), result == 0)) ||
          (result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\003",
                                "Return Squirrel Sanctuary to its owner's hand", 1, 10, "Decline",
                                1, 5, 0, 0, 0, 0),
           result != 1))
        return global_enchantment(player, card, event);
      result =
          charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player, COLOR_ANY, 1);
      if (result != 0)
        break;
      deaths = deaths - 1;
    }
    bounce_permanent(player, card);
  }
  return global_enchantment(player, card, event);
}

/* ManalinkEh 2022: 0x2243df0; original cards/modern_horizons_2.c:7678. */
int card_yusri_fortunes_flame(int player, int card, event_t event)
{
  int result;
  unsigned int remaining;
  int suggested_flips_or_wins;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    if (life[player] < 0x10)
    {
      suggested_flips_or_wins = 4 - (0xfU - life[player] >> 1);
      if (suggested_flips_or_wins < 1)
      {
        suggested_flips_or_wins = 1;
      }
    }
    else
    {
      suggested_flips_or_wins = 5;
    }
    do
    {
      result = choose_a_number(player, "How many coins you\'ll flip?", suggested_flips_or_wins);
      remaining = result - 1;
    } while (4 < remaining);
    suggested_flips_or_wins = 0;
    while (true)
    {
      result = flip_a_coin(player, card);
      if (result == 0)
      {
        damage_player(player, 2, player, card);
      }
      else
      {
        draw_cards(player, 1);
        suggested_flips_or_wins = suggested_flips_or_wins + 1;
      }
      if (remaining == 0)
        break;
      remaining = remaining - 1;
    }
    if (suggested_flips_or_wins == 5)
    {
      create_legacy_activate(player, card, yusri_effect);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222cf30; original cards/modern_horizons_2.c:475. */
int card_arcbound_tracker(int player, int card, event_t event)
{
  int result;

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_humiliated(player, card, AL_TRIG);
    if (result == 0)
    {
      result = get_specific_storm_count(player);
      if (0 < result)
      {
        sp_ability_trigger(player, event, 2, TYPE_ANY, NULL, player, card);
      }
    }
  }
  if (event == EVENT_SPA)
  {
    add_counters(player, card, player, card, COUNTER_P1_P1, 1);
  }
  result = modular(player, card, event, 2);
  return result;
}

/* ManalinkEh 2022: 0x2230250; original cards/modern_horizons_2.c:1442. */
int card_captain_ripley_vance(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((trigger_condition == 0xd3) && (trigger_cause_controller == player))
  {
    result = get_specific_storm_count(player);
    if (result == 2)
    {
      sp_ability_trigger(player, event, 2, TYPE_ANY, NULL, player, card);
    }
  }
  if (event != EVENT_SPA)
  {
    return 0;
  }
  add_counters(player, card, player, card, COUNTER_P1_P1, 1);
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "Select target creature, planeswalker or player.", 0, 0x80);
    if (result != 0)
    {
      result = get_power(player, card);
      damage_target0(player, card, result);
    }
  }
  instance = get_card_instance(player, card);
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x2233d30; original cards/modern_horizons_2.c:2553. */
int card_esper_sentinel(int player, int card, event_t event)
{
  int storm_count_or_power;
  int creature_spells_or_payment;
  int player_00;
  test_definition_t test;

  if ((trigger_condition == 0xd3) && (reason_for_trigger_controller == player))
  {
    player_00 = 1 - player;
    storm_count_or_power = get_specific_storm_count(player_00);
    creature_spells_or_payment = get_stormcreature_count(player_00);
    if (storm_count_or_power == creature_spells_or_payment)
    {
      default_test_definition(&test, 2);
      test.type_flag = 1;
      sp_ability_trigger(player_00, event, 2, TYPE_NONE, &test, player, card);
    }
  }
  if (event == EVENT_SPA)
  {

    storm_count_or_power = get_power(player, card);
    creature_spells_or_payment = has_mana(1 - player, 0, storm_count_or_power);
    if ((creature_spells_or_payment != 0) &&
        (storm_count_or_power = charge_mana(1 - player, COLOR_COLORLESS, storm_count_or_power),
         storm_count_or_power != 0))
    {
      return 0;
    }
    draw_cards(player, 1);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2233e60; original cards/modern_horizons_2.c:2581. */
int card_etherium_spinner(int player, int card, event_t event)
{
  test_definition_t test;

  if ((trigger_condition == 0xd3) && (trigger_cause_controller == player))
  {
    default_test_definition(&test, 0xff);
    test.cmc = 3;
    test.cmc_flag = 2;
    sp_ability_trigger(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event != EVENT_SPA)
  {
    return 0;
  }
  generate_token_by_id(player, card, 0x4995);
  return 0;
}

/* ManalinkEh 2022: 0x223a190; original cards/modern_horizons_2.c:4576. */
int card_moderation(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    result = is_humiliated(player, card, AL_STATIC);
    if ((result == 0) && (affected_card_controller == player))
    {
      result = is_what(player, affected_card, 1);
      if (result == 0)
      {
        result = get_specific_storm_count(affected_card_controller);
        if (0 < result)
        {
          infinite_casting_cost();
        }
      }
    }
    sp_ability_trigger(player, EVENT_MODIFY_COST_GLOBAL, 2, TYPE_ANY, NULL, player, card);
    return 0;
  }
  sp_ability_trigger(player, event, 2, TYPE_ANY, NULL, player, card);
  if (event == EVENT_SPA)
  {
    draw_cards(player, 1);
    return 0;
  }
  if (event == EVENT_SHOULD_AI_PLAY)
  {

    if (player == 0)
    {
      ai_modifier = ai_modifier + -8;
    }
    else
    {
      ai_modifier = ai_modifier + ((hand_count[player] < 3) - 1 & 0xfffffff0) + 8;
    }
    return 0;
  }
  if (event != EVENT_CAN_CAST)
  {
    return 0;
  }
  result = basic_spell(player, card, EVENT_CAN_CAST);
  return result;
}

/* ManalinkEh 2022: 0x22406e0; original cards/modern_horizons_2.c:6568. */
int card_sythis_harvests_hand(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  sp_ability_trigger(player, event, 2, TYPE_ENCHANTMENT, NULL, player, card);
  if (event == EVENT_SPA)
  {
    draw_cards(player, 1);
    gain_life(player, 1);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2238850; original cards/modern_horizons_2.c:3994. */
int card_knighted_myr(int player, int card, event_t event)
{
  int result;

  result = one_or_more_p1_p1_counters_are_putted_on_me(player, card, event);
  if (result != 0)
  {
    pump_ability_until_eot_no_repeat(player, card, player, card, 0x40000000, 0);
  }
  result = adapt(player, card, event, 1, 2, 0, 0, 0, 0, 1);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2235a30; original cards/modern_horizons_2.c:3118. */
int card_gaeas_will(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_activate(player, card, effect_yawgmoths_will);
    kill_card(player, card, KILL_DESTROY);
  }
  result = suspend_only(player, card, event, 4, 0, 0, 0, 1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2236ec0; original cards/modern_horizons_2.c:3449. */
int card_glimpse_of_tomorrow(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    warp_world_effect(player, card, player, 0xc);
    kill_card(player, card, KILL_DESTROY);
  }
  result = suspend_only(player, card, event, 3, 0, 0, 0, 0, 2, 0);
  return result;
}

/* ManalinkEh 2022: 0x223ba70; original cards/modern_horizons_2.c:5094. */
int card_profane_tutor(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_RESOLVE_SPELL)
  {
    result = suspend_only(player, card, event, 2, 1, 1, 0, 0, 0, 0);
    return result;
  }
  result = card_demonic_tutor(player, card, EVENT_RESOLVE_SPELL);
  return result;
}

/* ManalinkEh 2022: 0x223c720; original cards/modern_horizons_2.c:5328. */
int card_resurgent_belief(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_RESOLVE_SPELL)
  {
    result = suspend_only(player, card, event, 2, 1, 0, 0, 0, 0, 1);
    return result;
  }
  result = card_replenish(player, card, EVENT_RESOLVE_SPELL);
  return result;
}

/* ManalinkEh 2022: 0x223ef60; original cards/modern_horizons_2.c:6089. */
int card_sol_talisman(int player, int card, event_t event)
{
  int result;

  if (((event != EVENT_ACTIVATE) &&
       (1 < event - EVENT_RESOLVE_ACTIVATION && event != EVENT_CLEANUP)) &&
      (event != EVENT_COUNT_MANA))
  {
    result = suspend_only(player, card, event, 3, 1, 0, 0, 0, 0, 0);
    return result;
  }
  result = card_sol_ring(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22341b0; original cards/modern_horizons_2.c:2675. */
int card_fast_furious(int player, int card, event_t event)
{
  int p;
  card_instance_t *instance;
  int eligible;
  unsigned int selected_modes;
  int second_priority;
  int priority_first_half;
  int c;

  instance = get_card_instance(player, card);
  p = current_turn;
  if (event == EVENT_MODIFY_COST)
  {
    generic_split_card(player, card, EVENT_MODIFY_COST, 0x59f0, 1, 10, 0x59ff, 1, 0x14);
    return 0;
  }
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
      int totals[2];
      totals[0] = 0;
      totals[1] = 0;

      priority_first_half = hand_count[player] * -10 + 0x46;
      c = active_cards_count[current_turn] + -1;
      if (-1 < c)
      {
        do
        {
          instance = in_play(p, c);
          if ((((instance != NULL) && (eligible = is_what(p, c, 2), eligible != 0)) &&
               (eligible = get_toughness(p, c), eligible < 4)) &&
              ((eligible = check_for_ability(p, c, 0x4000),
                eligible == 0 && (eligible = check_for_ability(p, c, 0x20), eligible == 0))))
          {
            totals[p] = totals[p] + 1;
          }
          c = c + -1;
        } while (c != -1);
      }
      p = 1 - current_turn;
      c = active_cards_count[p] + -1;
      if (-1 < c)
      {
        do
        {
          instance = in_play(p, c);
          if (((instance != NULL) && (eligible = is_what(p, c, 2), eligible != 0)) &&
              ((eligible = get_toughness(p, c),
                eligible < 4 && ((eligible = check_for_ability(p, c, 0x4000),
                                  eligible == 0 &&
                                      (eligible = check_for_ability(p, c, 0x20), eligible == 0))))))
          {
            totals[p] = totals[p] + 1;
          }
          c = c + -1;
        } while (c != -1);
      }
      second_priority = totals[player] * -10 + 0x14 + totals[1 - player] * 10;
      return generic_split_card(player, card, event, 0x59f0, 1, priority_first_half, 0x59ff, 1,
                                second_priority);
    }
    if (event != EVENT_CAN_CHANGE_TARGET)
    {
      if (event == EVENT_RESOLVE_SPELL)
      {
        selected_modes = instance->info_slot;
        if ((selected_modes & 1) != 0)
        {
          discard(player, 0, 0);
          draw_cards(player, 2);
          selected_modes = instance->info_slot;
        }
        if ((selected_modes & 2) != 0)
        {
          test_definition_t this_test;
          default_test_definition(&this_test, 2);
          this_test.keyword = KEYWORD_FLYING;
          this_test.keyword_flag = DOESNT_MATCH;
          new_damage_all(player, card, 2, 3, 0, &this_test);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      else if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  second_priority = 0x14;
  priority_first_half = 10;
  return generic_split_card(player, card, event, 0x59f0, 1, priority_first_half, 0x59ff, 1,
                            second_priority);
}

/* ManalinkEh 2022: 0x223d1d0; original cards/modern_horizons_2.c:5515. */
int card_said_done(int player, int card, event_t event)
{
  if (event == EVENT_MODIFY_COST)
  {
    test_definition_t test;
    default_test_definition(&test, 0x38);
    unsigned int first_available = 0;
    if (new_special_count_grave(player, &test) && !graveyard_has_shroud(player))
      first_available = can_sorcery_be_played(player, event) != 0;
    target_definition_t td;
    default_target_definition(player, card, &td, 2);
    int second_available = can_target(&td);
    generic_split_card(player, card, event, 0x5a5d, first_available, 10, 0x59e2, second_available,
                       8);
    return 0;
  }
  if (event == EVENT_PLAY_FIRST_HALF || event == EVENT_PLAY_SECOND_HALF)
  {
    card_instance_t *instance = get_card_instance(player, card);
    instance->number_of_targets = 0;
    if (event == EVENT_PLAY_FIRST_HALF)
      return card_call_to_mind(player, card, EVENT_CAST_SPELL);
    return card_frost_breath(player, card, EVENT_CAST_SPELL);
  }
  if (event != EVENT_CAN_CAST && event != EVENT_CAST_SPELL && event != EVENT_RESOLVE_SPELL &&
      event != EVENT_CAN_CHANGE_TARGET && event != EVENT_CHANGE_TARGET)
    return 0;
  if (event == EVENT_CAST_SPELL && (card != affected_card || player != affected_card_controller))
    return 0;
  card_instance_t *instance = get_card_instance(player, card);
  test_definition_t test;
  default_test_definition(&test, 0x38);
  unsigned int first_available = 0;
  if (new_special_count_grave(player, &test) && !graveyard_has_shroud(player))
    first_available = can_sorcery_be_played(player, event) != 0;
  target_definition_t td;
  default_target_definition(player, card, &td, 2);
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (instance->info_slot & 1U)
      return card_call_to_mind(player, card, event);
    if (instance->info_slot & 2U)
      return card_frost_breath(player, card, event);
    kill_card(player, card, KILL_DESTROY);
  }
  int priority = 5;
  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller &&
      current_turn == player)
    priority = current_phase > 0x17 ? 15 : 5;
  int second_available = can_target(&td);
  return generic_split_card(player, card, event, 0x5a5d, first_available, priority, 0x59e2,
                            second_available, 10);
}

/* ManalinkEh 2022: 0x222bd80; original cards/modern_horizons_2.c:82. */
int basic_landcycling(int player, int card, event_t event, int cless, int black, int blue,
                           int green, int red, int white)
{
  int result;
  if (event - EVENT_CAN_ACTIVATE_FROM_HAND < 2)
  {
    result = cycling(player, card, event, cless, black, blue, green, red, white);
    return result;
  }
  if (event != EVENT_RESOLVE_ACTIVATION_FROM_HAND)
  {
    return 0;
  }
  tutor_basic_land(player, 0, 0);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22388f0; original cards/modern_horizons_2.c:4008. */
int card_landscaper_colos(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (event == EVENT_ETB)
    {
      result = count_graveyard(1 - player);
      if ((result != 0) && (result = graveyard_has_shroud(player), result == 0))
      {
        new_default_test_definition(&this_test, 0xff,
                                    "Select a card to put on the bottom of opponent\'s library.");
        new_global_tutor(player, 1 - player, 2, 10, 1, 1, &this_test);
      }
    }
    return 0;
  }
  result = basic_landcycling(player, card, event, 1, 0, 0, 0, 0, 1);
  return result;
}

/* ManalinkEh 2022: 0x2238b80; original cards/modern_horizons_2.c:4055. */
int card_lazotep_chancellor(int player, int card, event_t event)
{
  int result;

  if (trigger_condition != 0xd1)
  {
    return 0;
  }
  if ((card == affected_card) && (player == affected_card_controller))
  {
    result = duh_mode(player);
    result = discard_trigger(player, card, event, player, (result != 0) + 1, 0);
    if (result != 0)
    {
      result = charge_mana_while_resolving(player, card, event, player, COLOR_COLORLESS, 1);
      if (result != 0)
      {
        amass(player, card, 2);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2239cd0; original cards/modern_horizons_2.c:4482. */
int card_mental_journey(int player, int card, event_t event)
{
  int result;

  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      draw_cards(player, 3);
      kill_card(player, card, KILL_DESTROY);
    }
    result = basic_spell(player, card, event);
    return result;
  }
  result = basic_landcycling(player, card, event, 1, 0, 1, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x223aeb0; original cards/modern_horizons_2.c:4844. */
int card_orchard_strider(int player, int card, event_t event)
{
  int result;

  if (event - EVENT_CAN_ACTIVATE_FROM_HAND < 3)
  {
    result = basic_landcycling(player, card, event, 1, 0, 0, 1, 0, 0);
    return result;
  }
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  generate_tokens_by_id(player, card, 0x5077, 2);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2231120; original cards/modern_horizons_2.c:1832. */
int card_dakkon_shadow_slayer(int player, int card, event_t event)
{
  enum
  {
    CHOICE_SURVEIL = 1,
    CHOICE_EXILE = 2,
    CHOICE_TUTOR_ARTIFACT = 3
  };

  card_instance_t *instance;
  int result;
  target_definition_t td;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = count_permanents_by_type(player, TYPE_LAND);
      enters_the_battlefield_with_counters(player, card, EVENT_CAST_SPELL, COUNTER_LOYALTY, result);
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) ||
           (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE))
  {

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    result = can_target(&td);
    result = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033", "Surveil 2", 1,
                         10, 1, "Exile target creature", result, 0x14, 0xfffffffd,
                         "Tutor artifact from hand or graveyard", 1, 5, 0xfffffffa, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (result == 0)
      {
        return 0;
      }
    }
    else if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE)
    {
      instance->number_of_targets = '\0';
      if (result == CHOICE_EXILE)
      {
        new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      }
    }
    else if (result == CHOICE_SURVEIL)
    {
      surveil(player, 2);
    }
    else if (result == CHOICE_EXILE)
    {
      result = validate_target(player, card, &td, 0);
      if (result != 0)
      {
        kill_target0(player, card, KILL_EXILE);
      }
    }
    else
    {

      new_default_test_definition(&this_test, 0x40, "Select an artifact card.");
      result = new_global_tutor(player, player, 4, 4, 0, 2, &this_test);
      if (result < 0)
      {
        new_global_tutor(player, player, 2, 4, 0, 2, &this_test);
      }
    }
  }
  result = planeswalker(player, card, event, 0);
  return result;
}

/* ManalinkEh 2022: 0x22333b0; original cards/modern_horizons_2.c:2341. */
int card_dragons_rage_channeler(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  if ((trigger_condition == 0xd3) && (trigger_cause_controller == player))
  {
    default_test_definition(&this_test, 2);
    this_test.type_flag = 1;
    sp_ability_trigger(player, event, 2, TYPE_ANY, &this_test, player, card);
  }
  if (event != EVENT_SPA)
  {
    if ((event - EVENT_POWER < 3) || (event == EVENT_BEGIN_COMBAT))
    {
      result = is_humiliated(player, card, AL_STATIC);
      if (result == 0)
      {
        result = delirium(player);
        if (result != 0)
        {
          if ((card == affected_card) && (player == affected_card_controller))
          {
            if ((event == EVENT_POWER) || (event == EVENT_TOUGHNESS))
            {
              event_result = event_result + 2;
            }
            else if (event == EVENT_ABILITIES)
            {
              event_result = event_result | 0x20;
            }
          }
          attack_if_able(player, card, event);
        }
      }
    }
    return 0;
  }
  surveil(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x22352a0; original cards/modern_horizons_2.c:2961. */
int card_foul_watcher(int player, int card, event_t event)
{
  int result;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
    {
      result = is_humiliated(player, card, AL_STATIC);
      if (result == 0)
      {
        result = delirium(player);
        if (result != 0)
        {
          event_result = event_result + 1;
        }
      }
    }
    return 0;
  }
  surveil(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x2239bc0; original cards/modern_horizons_2.c:4452. */
int card_master_of_death(int player, int card, event_t event)
{
  unsigned int saved_state;
  int result;
  card_instance_t *instance;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    surveil(player, 2);
    return 0;
  }
  if (event == EVENT_GRAVEYARD_ABILITY_UPKEEP)
  {

    result = can_pay_life_as_cost_for_spells_or_activated_abilities(player, 1);
    if ((result != 0) && (result = do_dialog(player, player, card, -1, -1,
                                             " Return Master of Death to your hand\n decline",
                                             (unsigned int)(life[player] < 7)),
                          result == 0))
    {

      lose_life(player, 1);
      instance = get_card_instance(player, card);
      saved_state = instance->state;
      hand_count[player] = hand_count[player] + 1;
      instance->state = saved_state & 0xffffffdf;
      return -1;
    }
    return -2;
  }
  return 0;
}

/* ManalinkEh 2022: 0x223e4f0; original cards/modern_horizons_2.c:5884. */
int card_sinister_starfish(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    surveil(player, 1);
  }
  result = generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222e1f0; original cards/modern_horizons_2.c:797. */
int card_battle_plan(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    result = beginning_of_combat(player, card, event, player);
    if (result != 0)
    {

      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      td.allowed_controller = player;
      td.allow_cancel = 0;
      instance = get_card_instance(player, card);
      instance->number_of_targets = '\0';
      result = can_target(&td);
      if (result != 0)
      {
        result = new_pick_target(&td, "Select target creature you control.", 0, 0x81);
        if (result != 0)
        {
          pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 2,
                         0);
        }
      }
    }
    result = global_enchantment(player, card, event);
    return result;
  }
  result = basic_landcycling(player, card, event, 1, 0, 0, 0, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x222e960; original cards/modern_horizons_2.c:921. */
int card_bloodbraid_marauder(int player, int card, event_t event)
{
  int result;

  cannot_block(player, card, event);
  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = delirium(player);
    if (result != 0)
    {
      cascade(player, card, EVENT_CAST_SPELL);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2233f20; original cards/modern_horizons_2.c:2602. */
int card_ethersworn_sphinx(int player, int card, event_t event)
{
  affinity(player, card, event, 0x40, -1);
  cascade(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x2237020; original cards/modern_horizons_2.c:3481. */
int card_glorious_enforcer(int player, int card, event_t event)
{
  int result;

  lifelink(player, card, event);
  result = beginning_of_combat(player, card, event, 2);
  if ((result != 0) && (life[1 - player] < life[player]))
  {
    pump_ability_until_eot_no_repeat(player, card, player, card, 0x40000000, 0);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x223a390; original cards/modern_horizons_2.c:4616. */
int card_mount_velus_manticore(int player, int card, event_t event)
{
  int discarded_card;
  type_t discarded_type;
  unsigned int type_count;
  int tribal;
  card_instance_t *instance;
  target_definition_t td;
  test_definition_t test;

  if (beginning_of_combat(player, card, event, player) != 0)
  {

    new_default_test_definition(&test, 0xff, "Select a card to discard.");
    discarded_card = new_select_a_card(player, player, 4, 0, 3, -1, &test);
    if (-1 < discarded_card)
    {

      discarded_type = get_type(player, discarded_card);
      type_count = num_bits_set(discarded_type & ~TYPE_INTERRUPT);
      tribal = is_tribal(player, discarded_card);
      discard_card(player, discarded_card);
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      td.allow_cancel = 0;
      if (can_target(&td) != 0)
      {
        if (new_pick_target(&td, "TARGET_ANY", 0, 0) != 0)
        {
          damage_target0(player, card, (type_count + 1) - (unsigned int)(tribal == 0));
          instance = get_card_instance(player, card);
          instance->number_of_targets = '\0';
        }
      }
    }
  }
  return global_enchantment(player, card, event);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22305e0; original cards/modern_horizons_2.c:1596. */
int card_chatterfang_squirrel_general(int player, int card, event_t event)
{
  enum
  {
    SARV_REPORT_COUNT = 8
  };

  int sacrificed_count;
  card_instance_t *instance;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&test, 0x47, "Select a Squirrel to sacrifice.");
    test.subtype = 0xc0;
    default_target_definition(player, card, &td, 2);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 0x47, "Select a Squirrel to sacrifice.");
    test.subtype = 0xc0;
    default_target_definition(player, card, &td, 2);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      instance = get_card_instance(player, card);
      sacrificed_count = instance->info_slot;
      instance->info_slot = 0;
      if (valid_target(&td) != 0)
      {
        pump_until_eot_merge_previous(player, card, instance->targets[0].player,
                                      instance->targets[0].card, sacrificed_count,
                                      -sacrificed_count);
      }
    }
  }
  return generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_CAN_TARGET, 0, 1, 0, 0, 0, 0, 0, &td, "TARGET_CREATURE", TYPE_NONE,
      &test, SARV_REPORT_CMC | SARV_REPORT_COUNT);
}

/* ManalinkEh 2022: 0x2230860; original cards/modern_horizons_2.c:1655. */
int card_chitterspitter(int player, int card, event_t event)
{

  int result;
  card_instance_t *instance;
  unsigned int sacrificed;
  test_definition_t test;

  result = duh_mode(player);
  upkeep_trigger_ability_mode(player, card, event, player,
                              (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    new_default_test_definition(&test, 0x47, "Select a token to sacrifice.");
    test.type_flag = 6;
    result = new_can_sacrifice(player, card, player, &test);
    if (result != 0)
    {

      sacrificed = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_JUST_MARK, &test);
      if (sacrificed != 0)
      {
        add_counters(player, card, player, card, COUNTER_ACORN, 1);
        kill_card(player, sacrificed >> 0x18, KILL_SACRIFICE);
      }
    }
  }
  else if (event - EVENT_POWER < 2)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated(player, card, AL_STATIC);
      if ((result == 0) && (affected_card_controller == player))
      {
        result = has_subtype(player, affected_card, SUBTYPE_SQUIRREL);
        if (result != 0)
        {
          result = count_counters(player, card, COUNTER_ACORN);
          event_result = event_result + result;
        }
      }
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    generate_token_by_id(player, card, 0x4991);
  }
  return generic_activated_ability(player, card, event, 1, 0, 0, 0, 1, 0, 0, 0, NULL, NULL);
}

/* ManalinkEh 2022: 0x222ba50; original cards/modern_horizons_2.c:1961. */
static int card_exiled_with_void_counter(int player, int card, event_t event)
{

  card_instance_t *instance;
  int counters;
  int iid_or_exiled_count;
  int card_00;
  int matching_legacies;

  if (event == EVENT_SET_LEGACY_EFFECT_NAME)
  {
    instance = get_card_instance(player, card);
    if (-1 < instance->targets[0].card)
    {

      instance = get_card_instance(player, card);
      scnprintf(set_legacy_effect_name_addr, 0x33, "%s",
                cards_ptr[cards_data[instance->targets[0].card].id]->name);
    }
  }
  else
  {
    if (event == EVENT_SET_LEGACY_EFFECT_TEXT)
    {
      scnprintf(set_legacy_effect_name_addr, 0x33, "Exiled with a void counter.");
      return 0;
    }
    if (event == EVENT_STATIC_EFFECTS)
    {

      instance = get_card_instance(player, card);
      matching_legacies = 0;
      iid_or_exiled_count = instance->targets[0].card;
      if (0 < active_cards_count[player])
      {
        card_00 = 0;
        do
        {
          instance = in_play(player, card_00);
          if (instance != NULL)
          {
            if (is_what(player, card_00, 0x80) != 0)
            {

              instance = get_card_instance(player, card_00);
              if (instance->info_slot_fn == card_exiled_with_void_counter)
              {
                counters = count_counters(player, card_00, COUNTER_VOID);
                if ((counters != 0) && (iid_or_exiled_count == instance->targets[0].card))
                {
                  matching_legacies = matching_legacies + 1;
                }
              }
            }
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[player]);
      }
      iid_or_exiled_count = count_iid_in_rfg(player, iid_or_exiled_count);
      if (iid_or_exiled_count < matching_legacies)
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2235ad0; original cards/modern_horizons_2.c:3134. */
int card_galvanic_relay(int player, int card, event_t event)
{
  enum
  {
    MPCFE_DURING_YOUR_NEXT_TURN = 1024
  };

  int result;
  card_instance_t *instance;
  bool own_turn;

  if (event == EVENT_QUERY)
  {
    if (attacking_card == 4)
    {
      return 1;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {

    result = exile_top_x_card_you_may_play_them(player, card, 1, NULL, player,
                                                MPCFE_DURING_YOUR_NEXT_TURN |
                                                    MPCFE_UNTIL_END_OF_YOUR_NEXT_TURN);
    own_turn = current_turn == player;
    instance = get_card_instance(player, result);
    instance->targets[1].card = (unsigned int)own_turn;
    kill_card(player, card, KILL_DESTROY);
  }
  return basic_spell(player, card, event);
}

/* ManalinkEh 2022: 0x22419b0; original cards/modern_horizons_2.c:6955. */
int card_tizerus_charger(int player, int card, event_t event)
{

  int result;

  if ((((event == EVENT_CAST_SPELL) || (event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE)) &&
       (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = check_special_flags3(player, card, 0x80000);
    if (result != 0)
    {

      result = dialog_impl(player, card, event, "\001\002", "\001\014", "\001\003", "+1/+1 counter",
                           1, 10, "Flying counter", 1, 0x14, 0, 0, 0, 0);
      if (result == 1)
      {
        enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, 1);
      }
      else if (result == 2)
      {
        enters_the_battlefield_with_counters(player, card, event, COUNTER_FLYING, 1);
      }
    }
  }
  result = escape(player, card, event, 4, 1, 0, 0, 0, 0, 5);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x223f5c0; original cards/modern_horizons_2.c:6239. */
int card_steel_dromedary(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  comes_into_play_tapped(player, card, event);
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, 2);
  if (event == EVENT_UNTAP)
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
  else if ((event != EVENT_CHANGE_TYPE) && (event != EVENT_BEGIN_COMBAT))
  {
    return 0;
  }
  result = count_counters(player, card, COUNTER_P1_P1);
  if (result != 0)
  {
    does_not_untap(player, card, event);
    result = beginning_of_combat(player, card, event, player);
    if (result != 0)
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      result = can_target(&td);
      if ((result != 0) &&
          (result = new_pick_target(&td, "Select target creature which will get a +1/+1 counter.",
                                    0, 0x80),
           result != 0))
      {

        instance = get_card_instance(player, card);
        move_counters(player, card, instance->targets[0].player,
                                  instance->targets[0].card, player, card, COUNTER_P1_P1, 1);
        instance->number_of_targets = '\0';
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2238260; original cards/modern_horizons_2.c:3874. */
int card_junk_winder(int player, int card, event_t event)
{
  if (event == EVENT_MODIFY_COST)
  {
    if (card == affected_card && player == affected_card_controller)
    {
      test_definition_t test;
      default_test_definition(&test, 0x47);
      test.type_flag = 6;
      int tokens = check_battlefield_for_special_card(player, card, player, 4, &test);
      COST_COLORLESS -= (char)tokens;
    }
    if (trigger_condition != 0xdb)
      return 0;
  }
  if (trigger_condition == 0xdb && card == affected_card && affected_card_controller == player)
  {
    test_definition_t test;
    default_test_definition(&test, 0x47);
    test.type_flag = 6;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    target_definition_t td;
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = 1 - player;
    td.illegal_type = 1;
    card_instance_t *instance = get_card_instance(player, card);
    if (can_target(&td) &&
        new_pick_target(&td, "Select target nonland permanent an opponent controls.", 0, 0x80))
    {
      effect_frost_titan(player, card, instance->targets[0].player, instance->targets[0].card);
      instance->number_of_targets = 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22361d0; original cards/modern_horizons_2.c:3234. */
int card_geyadrone_dihada(int player, int card, event_t event)
{
  enum
  {
    CHOICE_OPP_LOSES_2_LIFE = 1,
    CHOICE_STEAL_PERMANENT_UNTIL_EOT = 2,
    CHOICE_STEAL_ALL_PERMANENTS = 3
  };


  card_instance_t *instance;
  int value;
  int choice;
  target_definition_t td;

  target_definition_t td2;
  int p;
  int c;
  int marked[151][2];
  check_legend_rule(player, card, event);
  misc_protection(player, card, event);
  if (event == EVENT_PROT_MISC)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      if (((unsigned int)event_result >> 24 & 0xff) == '\x06')
      {
        event_result = event_result | 1;
        return 0;
      }
      get_protection_source('\0', &p, &c);
      if ((((-1 < p) && (-1 < c)) && (value = is_what(p, c, 0x47), value != 0)) &&
          (value = count_counters(p, c, COUNTER_CORRUPTION), value != 0))
      {
        event_result = event_result | 1;
      }
    }
  }
  else if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x102);
    td.special = 0x4000;
    default_target_definition(player, card, &td2, 0x102);
    value = can_target(&td2);
    value = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                        "Drain 2 and corrupt", 1, 0x14 - life[player], 1, "Borrow target", value,
                        10, 0xfffffffd, "Steal all corrupted", 1, 0x1e, 0xfffffff9, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (value == 0)
      {
        return 0;
      }
    }
    else
    {
      instance->number_of_targets = '\0';
      if (value == CHOICE_OPP_LOSES_2_LIFE)
      {
        td.allow_cancel = 2;
        new_pick_target(&td, "Select another target creature or planeswalker.", 0, 0x80);
      }
      else if (value == CHOICE_STEAL_PERMANENT_UNTIL_EOT)
      {
        new_pick_target(&td2, "Select target creature or planeswalker.", 0, 0x81);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x102);
    td.special = 0x4000;
    default_target_definition(player, card, &td2, 0x102);
    value = can_target(&td2);
    choice = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                         "Drain 2 and corrupt", 1, 0x14 - life[player], 1, "Borrow target", value,
                         10, 0xfffffffd, "Steal all corrupted", 1, 0x1e, 0xfffffff9, 0, 0, 0, 0);
    value = current_turn;
    if (choice == CHOICE_OPP_LOSES_2_LIFE)
    {
      lose_life(1 - player, 2);
      gain_life(player, 2);
      if (validate_target(player, card, &td, 0) != 0)
      {
        add_counters(player, card, instance->targets[0].player, instance->targets[0].card,
                     COUNTER_CORRUPTION, 1);
      }
    }
    else if (choice == CHOICE_STEAL_PERMANENT_UNTIL_EOT)
    {
      if (validate_target(player, card, &td2, 0) != 0)
      {
        add_counters(player, card, instance->targets[0].player, instance->targets[0].card,
                     COUNTER_CORRUPTION, 1);
        effect_act_of_treason(player, card, instance->targets[0].player, instance->targets[0].card);
      }
    }
    else if (choice == CHOICE_STEAL_ALL_PERMANENTS)
    {
      int marked_count = 0;
      int scan_player = value;
      for (int scan_card = active_cards_count[current_turn] - 1; scan_card >= 0; --scan_card)
      {
        instance = in_play(scan_player, scan_card);
        if (instance && is_what(scan_player, scan_card, 0x47) &&
            count_counters(scan_player, scan_card, COUNTER_CORRUPTION))
        {
          marked[marked_count][0] = scan_player;
          marked[marked_count][1] = scan_card;
          ++marked_count;
        }
      }
      scan_player = 1 - current_turn;
      for (int scan_card = active_cards_count[scan_player] - 1; scan_card >= 0; --scan_card)
      {
        instance = in_play(scan_player, scan_card);
        if (instance && is_what(scan_player, scan_card, 0x47) &&
            count_counters(scan_player, scan_card, COUNTER_CORRUPTION))
        {
          marked[marked_count][0] = scan_player;
          marked[marked_count][1] = scan_card;
          ++marked_count;
        }
      }
      for (int index = marked_count - 1; index >= 0; --index)
        gain_control_permanently(player, marked[index][0], marked[index][1]);
    }
  }
  return planeswalker(player, card, event, 4);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2237ff0; original cards/modern_horizons_2.c:3814. */
int card_inevitable_betrayal(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {

    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    instance = get_card_instance(player, card);
    if (instance->info_slot == 1)
    {
      result = basic_spell(player, card, EVENT_CAN_CAST);
      return result;
    }
    result = generic_spell(player, card, EVENT_CAN_CAST, 0x40, &td, NULL, 1, NULL);
    return result;
  }
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
    result = played_for_free(player, card);
    if ((result != 0) || (result = is_token(player, card), result != 0))
    {
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      result = generic_spell(player, card, EVENT_CAST_SPELL, 0x40, &td, NULL, 1, NULL);
      return result;
    }
  }
  else
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = card_bribery(player, card, EVENT_RESOLVE_SPELL);
      return result;
    }
    if (((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET)) &&
        (event != EVENT_MODIFY_COST))
    {
      return 0;
    }
  }
  result = suspend_only(player, card, event, 3, 1, 0, 2, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x222a950; original cards/modern_horizons_2.c:1364. */
static int devouring_type(int player, int card, int type)
{
  int count;
  int next_index;
  const char *permanent_type_text;
  card_instance_t *instance;
  int csvid;
  int last_index_or_result;
  unsigned int sacrificed;
  const char *article_suffix;
  int devoured_count;
  int *devoured_cursor;
  bool more_devoured;
  int desired_count;
  target_definition_t td;
  char msg[100];
  test_definition_t test;
  int devoured[151];

  base_target_definition(player, card, &td, type);
  td.illegal_abilities = 0;
  td.allowed_controller = player;
  td.preferred_controller = player;
  default_test_definition(&test, type);
  permanent_type_text = type_text(msg, 100, test.type, 0);
  if ((test.type == 4) || (test.type == 0x40))
  {
    article_suffix = "n";
  }
  else
  {
    article_suffix = "";
  }
  sprintf(test.message, "Select a%s %s to sacrifice.", article_suffix, permanent_type_text);
  instance = get_card_instance(player, card);
  if ((ai_is_speculating == 1) ||
      ((desired_count = -1, player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    count = count_subtype(player, type, -1);
    desired_count = count / 3;
  }
  instance->targets[1].player = 0;
  instance->targets[1].card = 0;
  csvid = get_id(player, card);
  devoured_count = 0;
  do
  {
    last_index_or_result = can_target(&td);
    if ((last_index_or_result == 0) ||
        (sacrificed = new_sacrifice(player, card, player,
                                    SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test),
         sacrificed == 0))
    {
      last_index_or_result = devoured_count + -1;
      desired_count = devoured_count;
      break;
    }
    instance->targets[1].card = instance->targets[1].card + 1;
    if ((csvid == 0x27f5) &&
        (last_index_or_result = has_subtype(player, sacrificed >> 0x18, SUBTYPE_GOBLIN),
         last_index_or_result != 0))
    {
      instance->targets[1].player = instance->targets[1].player + 1;
    }
    next_index = devoured_count + 1;
    devoured[devoured_count] = sacrificed >> 0x18;
    last_index_or_result = devoured_count;
    devoured_count = next_index;
  } while (desired_count != next_index);
  if (-1 < last_index_or_result)
  {
    devoured_cursor = devoured + last_index_or_result;
    do
    {
      kill_card(player, *devoured_cursor, KILL_SACRIFICE);
      more_devoured = devoured_cursor != devoured;
      devoured_cursor = devoured_cursor + -1;
    } while (more_devoured);
  }
  return desired_count;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void devour_type(int player, int card, event_t event, int type, int number);

/* ManalinkEh 2022: 0x2230120; original cards/modern_horizons_2.c:1409. */
void devour_type(int player, int card, event_t event, int type, int number)
{
  int result;
  card_instance_t *instance;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = can_sacrifice_type_as_cost(player, 1, type);
    if (result != 0)
    {
      instance = get_card_instance(player, card);
      instance->state = instance->state | STATE_OUBLIETTED;
      result = devouring_type(player, card, type);
      instance->state = instance->state & 0xff7fffff;
      add_counters(player, card, player, card, COUNTER_P1_P1, number * result);
      if (0 < result)
      {
        instance = get_card_instance(player, card);
        instance->targets[2].player = 0x42;
      }
    }
  }
  return;
}

/* ManalinkEh 2022: 0x22301f0; original cards/modern_horizons_2.c:1430. */
int card_caprichrome(int player, int card, event_t event)
{
  int result;

  vigilance(player, card, event);
  devour_type(player, card, event, 0x40, 1);
  result = flash(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222b300; original cards/modern_horizons_2.c:2140. */
int dermotaxi_legacy(int player, int card, event_t event)
{
  char attached_player;
  int t_card;
  card_instance_t *effect;
  card_instance_t *attached;
  unsigned char artifact_type;
  uint32_t argument1;
  inst_info_t imprint_record;
  uint32_t card_type;

  if (event != EVENT_CHANGE_TYPE)
  {
    attached_is_additional_subtype(player, card, event, SUBTYPE_VEHICLE);
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  effect = get_card_instance(player, card);
  if (-1 < effect->damage_target_player)
  {

    effect = get_card_instance(player, card);
    attached = get_card_instance((int)effect->damage_target_player, effect->damage_target_card);
    t_card = effect->damage_target_card;
    attached_player = effect->damage_target_player;
    imprint_record = inst_info(effect, 0x59dd);
    argument1 = imprint_record.data;
    if ((((t_card == affected_card) && (affected_card_controller == attached_player)) &&
         (argument1 != 0xffffffff)) &&
        ((((unsigned int)land_can_be_played >> 16 & 0xff) & 2) == 0))
    {

      artifact_type = cards_data[argument1].type | 0x40;
      if (cards_data[argument1].type == artifact_type)
      {
        effect->targets[7].player = argument1;
        effect->dummy3 = argument1;
        attached->regen_status = attached->regen_status | 0x1000000;
        card_type = argument1;
      }
      else if ((effect->targets[7].player != argument1) ||
               (card_type = effect->dummy3, cards_data[effect->dummy3].type != artifact_type))
      {

        card_type = create_a_card_type(argument1);
        if (card_type == 0xffffffff)
        {
          return 0;
        }
        cards_data[card_type].type = artifact_type;
        effect->dummy3 = card_type;
        effect->targets[7].player = argument1;
        attached->regen_status = attached->regen_status | 0x1000000;
      }
      event_result = card_type;
    }
  }
  attached_is_additional_subtype(player, card, EVENT_CHANGE_TYPE, SUBTYPE_VEHICLE);
  return 0;
}

/* ManalinkEh 2022: 0x22378f0; original cards/modern_horizons_2.c:3666. */
int card_grist_the_hunger_tide(int player, int card, event_t event)
{
  enum
  {
    CHOICE_INSECT = 1,
    CHOICE_SAC_AND_KILL = 2,
    CHOICE_OPP_LOSE_LIFE = 3
  };

  int choice_or_result;
  card_instance_t *instance;
  int grave_creatures;
  unsigned int sacrificed;
  card_instance_t *inst;
  unsigned int player_00;
  int player_01;
  inst_info_t ability_record;
  uint32_t ultimate_priority;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE)
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x102);
    choice_or_result = life[1 - player];
    grave_creatures = count_graveyard_by_type(1 - player, 2);
    if (choice_or_result - grave_creatures < 6)
    {
      choice_or_result = can_target(&td);
      ultimate_priority = 100;
    }
    else
    {
      choice_or_result = can_target(&td);
      ultimate_priority = 10;
    }
    choice_or_result = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                                   "Insect and mill a card", 1, 10, 1, "Destroy creature or PW",
                                   choice_or_result, 0x14, 0xfffffffe, "Opponent loses life", 1,
                                   ultimate_priority, 0xfffffffb, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (choice_or_result == 0)
      {
        return 0;
      }
    }
    else
    {
      instance->number_of_targets = '\0';
      if (choice_or_result == CHOICE_SAC_AND_KILL)
      {
        test_definition_t this_test;
        new_default_test_definition(&this_test, 2, "Select a creature to sacrifice.");
        sacrificed = new_sacrifice(player, card, player,
                                   SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &this_test);
        if (sacrificed != 0)
        {
          choice_or_result =
              new_pick_target(&td, "Select target creature or planeswalker.", 0, 0x81);
          player_00 = (int)sacrificed >> 0x10 & 0xff;
          if (choice_or_result == 0)
          {
            state_untargettable(player_00, sacrificed >> 0x18, 0);
          }
          else
          {
            kill_card(player_00, sacrificed >> 0x18, KILL_SACRIFICE);
          }
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x102);
    player_01 = 1 - player;
    choice_or_result = life[player_01];
    grave_creatures = count_graveyard_by_type(player_01, 2);
    if (choice_or_result - grave_creatures < 6)
    {
      choice_or_result = can_target(&td);
      ultimate_priority = 100;
    }
    else
    {
      choice_or_result = can_target(&td);
      ultimate_priority = 10;
    }
    choice_or_result = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                                   "Insect and mill a card", 1, 10, 1, "Destroy creature or PW",
                                   choice_or_result, 0x14, 0xfffffffe, "Opponent loses life", 1,
                                   ultimate_priority, 0xfffffffb, 0, 0, 0, 0);
    if (choice_or_result == CHOICE_INSECT)
    {
      token_generation_t token;
      default_token_definition(player, card, 0x4952, &token);
      token.color_forced = '\n';
      generate_token(&token);
      while (true)
      {
        inst = get_card_instance(instance->parent_controller, instance->parent_card);
        inst->info_slot = 0;
        special_mill(instance->parent_controller, instance->parent_card, 0x5a13, player, 1);
        ability_record = inst_info(inst, 0x5a13);
        if (ability_record.data2 != 0x42)
          break;
        add_counters(player, card, instance->parent_controller, instance->parent_card,
                     COUNTER_LOYALTY, 1);
      }
    }
    else if (choice_or_result == CHOICE_SAC_AND_KILL)
    {
      if (validate_target(player, card, &td, 0) != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
    }
    else if (choice_or_result == CHOICE_OPP_LOSE_LIFE)
    {
      lose_life(player_01, count_graveyard_by_type(player, 2));
    }
  }
  return planeswalker(player, card, event, 3);
}

/* ManalinkEh 2022: 0x2239520; original cards/modern_horizons_2.c:4243. */
int card_lonis_cryptozoologist(int player, int card, event_t event)
{
  enum
  {
    SARV_REPORT_COUNT = 8
  };

  card_instance_t *instance;
  uint32_t num_cards;
  inst_info_t clue_record;
  target_definition_t td;
  char buffer[100];
  test_definition_t test2;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((((trigger_condition == 0xdb) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      ((trigger_cause_controller != player || (trigger_cause != card))))
  {
    default_test_definition(&test, 2);
    test.type_flag = 3;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    investigate(player, card, player);
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      new_default_test_definition(&test2, 0x47, "Select a Clue to sacrifice,");
      test2.subtype = 0x3004;
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        return generic_activated_ability_with_sacrifice_as_cost(
            player, card, EVENT_CAN_ACTIVATE, GAA_CAN_ONLY_TARGET_OPPONENT | GAA_UNTAPPED, 0, 0, 0,
            0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test2, SARV_REPORT_COUNT);
      }
      if (event == EVENT_ACTIVATE)
      {
        generic_activated_ability_with_sacrifice_as_cost(
            player, card, EVENT_ACTIVATE, GAA_CAN_ONLY_TARGET_OPPONENT | GAA_UNTAPPED, 0, 0, 0, 0,
            0, 0, 0, NULL, NULL, TYPE_NONE, &test2, SARV_REPORT_COUNT);
        set_inst_info(instance, 0x5a2b, instance->info_slot, 0);
        instance->info_slot = 0;
        return 0;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        return 0;
      }
      new_default_test_definition(&test2, 0x47, "Select a Clue to sacrifice,");
      test2.subtype = 0x3004;
      instance = get_card_instance(player, card);
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      td.allowed_controller = 1 - player;
      if (valid_target(&td) != 0)
      {

        clue_record = inst_info(instance, 0x5a2b);
        num_cards = clue_record.data;
        clear_info(instance, 0x5a2b);
        scnprintf(buffer, 100, "Select a nonland permanent with mana value %d or less.", num_cards);
        new_default_test_definition(&test, 0x39, buffer);
        test.cmc = num_cards + 1;
        test.type_flag = 1;
        test.cmc_flag = 3;
        reveal_top_cards_of_library_and_choose(player, card, 1 - player, num_cards, 0, TUTOR_PLAY,
                                               0, TUTOR_BOTTOM_OF_DECK_RANDOM, 1, &test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x223a530; original cards/modern_horizons_2.c:4645. */
int card_murktide_regent(int player, int card, event_t event)
{
  card_instance_t *instance;
  inst_info_t result;

  delve(player, card, event);
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      result = inst_info(instance, 0x5a37);
      enters_the_battlefield_with_counters(player, card, EVENT_CAST_SPELL, COUNTER_P1_P1,
                                           result.data);
      clear_info(instance, 0x5a37);
    }
  }
  else if ((event == EVENT_REMOVED_FROM_GRAVEYARD) && (affected_card_controller == player))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      add_counters(player, card, player, card, COUNTER_P1_P1,
                               (unsigned int)((unsigned int)event_result >> 8 & 0xff));
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x223abd0; original cards/modern_horizons_2.c:4789. */
int card_nykthos_paragon(int player, int card, event_t event)
{
  int result;
  actions_t life_gained;
  card_instance_t *instance;

  result = duh_mode(player);
  life_gained =
      trigger_gain_life(player, card, event, player, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (life_gained != 0)
  {
    instance = get_card_instance(player, card);
    if (instance->info_slot != 0x42)
    {
      instance = in_play(player, card);
      if (instance != NULL)
      {
        manipulate_type(player, card, 2, TYPE_CREATURE,
                        life_gained & 0xffff | ACT_ADD_COUNTERS_BASE);
        instance = get_card_instance(player, card);
        set_inst_info(instance, 0x5a3e, 0, 0x42);
      }
    }
  }
  if (event == EVENT_CLEANUP)
  {
    instance = get_card_instance(player, card);
    clear_info(instance, 0x5a3e);
  }
  return global_enchantment(player, card, event);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222ca50; original cards/modern_horizons_2.c:382. */
int card_arcbound_javelineer(int player, int card, event_t event)
{
  unsigned int packed_damage_and_counter_power;
  card_instance_t *inst;
  card_instance_t *target;
  uint32_t available_counters;
  int value;
  uint32_t data;
  inst_info_t damage_record;
  target_definition_t td;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.required_state = 0x20;
    inst = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      return generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                       &td, NULL);
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0x84, 0, 0, 0, 0, 0, 0, 0, &td,
                                "Select target attacking or blocking creature.");
      if (spell_fizzled == 1)
      {
        return 0;
      }

      value = get_toughness(inst->targets[0].player, inst->targets[0].card);
      target = get_card_instance(inst->targets[0].player, inst->targets[0].card);

      packed_damage_and_counter_power =
          (uint32_t)target->damage_on_card | ((uint32_t)(uint16_t)target->counter_power << 16);

      available_counters = count_counters(player, card, COUNTER_P1_P1);
      if (player == 0)
      {
        value = count_counters(0, card, COUNTER_P1_P1);
        data = choose_a_number(0, "How many +1/+1 counters you want to remove?", value);
      }
      else
      {
        data = value - (packed_damage_and_counter_power & 0xffff);
        if ((int)available_counters < (int)data)
        {
          data = available_counters;
        }
      }
      if ((-1 < (int)data) &&
          (value = count_counters(player, card, COUNTER_P1_P1), (int)data <= value))
      {
        set_inst_info(inst, 0x59ac, data, 0);
        remove_counters(player, card, player, card, COUNTER_P1_P1, data);
        return 0;
      }
      spell_fizzled = 1;
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return modular(player, card, event, 1);
    }
    default_target_definition(player, card, &td, 2);
    td.required_state = 0x20;
    inst = get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (value = valid_target(&td), value != 0))
  {
    damage_record = inst_info(inst, 0x59ac);
    damage_target0(player, card, damage_record.data);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2232c90; original cards/modern_horizons_2.c:2191. */
int card_dermotaxi(int player, int card, event_t event)
{
  target_t *ret_location;
  unsigned short exiled_csvid;
  uint32_t data;
  int value;
  card_instance_t *effect_or_card;
  int opponent_creatures;
  int32_t player_00;
  target_t *target_cursor;
  target_t *next_target;
  inst_info_t imprint_record;
  int second_creature;
  unsigned int selected_count;
  card_instance_t *instance;

  if (event == EVENT_RESOLVE_SPELL)
  {
    test_definition_t test;
    value = count_graveyard_by_type(player, 2);
    player_00 = 1 - player;
    opponent_creatures = count_graveyard_by_type(player_00, 2);
    effect_or_card = get_card_instance(player, card);
    if (opponent_creatures == 0)
    {
      if (value != 0)
      {
        player_00 = player;
      }
    }
    else if (value != 0)
    {
      target_definition_t td;
      default_target_definition(player, card, &td, 0xff);
      td.zone = 0x1000;
      td.allow_cancel = 0;
      td.illegal_abilities = 0;
      if (new_pick_target(&td, "Select the owner of the graveyard you want to search.", 0, 0x80) !=
          0)
      {
        player_00 = effect_or_card->targets[0].player;
        effect_or_card->number_of_targets = '\0';
      }
    }
    new_default_test_definition(&test, 2, "Select a creature card to imprint on Dermotaxi.");
    value = new_select_a_card(player, player_00, 2, 1, 2, -1, &test);
    data = graveyard_ptr_mutable[player_00][value];
    rfg_card_from_grave(player_00, value);
    set_inst_info(effect_or_card, 0x59dd, data, (uint16_t)player_00);
  }
  else
  {
    target_definition_t td2;
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      instance = get_card_instance(player, card);
      default_target_definition(player, card, &td2, 2);
      td2.illegal_abilities = 0;
      td2.allowed_controller = player;
      td2.preferred_controller = player;
      td2.illegal_state = 1;
      if (event == EVENT_CAN_ACTIVATE)
      {
        if (generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL) == 0)
        {
          return 0;
        }
        value = target_available(player, card, &td2);
        return (unsigned int)(1 < value);
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        return 0;
      }
      instance = get_card_instance(player, card);
      default_target_definition(player, card, &td2, 2);
    }
    td2.allowed_controller = player;
    td2.illegal_state = 1;
    td2.illegal_abilities = 0;
    td2.preferred_controller = td2.allowed_controller;
    if (event == EVENT_ACTIVATE)
    {
      instance->number_of_targets = '\0';
      if (charge_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0) != 0)
      {
        ret_location = instance->targets;
        second_creature = -1;
        if (select_target(player, card, &td2, "Select a creature to tap.", ret_location) != 0)
        {
          state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
          value = instance->targets[0].card;
          if (select_target(player, card, &td2, "Select a creature to tap.",
                            instance->targets + 1) == 0)
          {
            selected_count = 1;
          }
          else
          {
            state_untargettable(instance->targets[1].player, instance->targets[1].card, 1);
            second_creature = instance->targets[1].card;
            selected_count = 2;
          }
          target_cursor = ret_location;
          do
          {
            next_target = target_cursor + 1;
            state_untargettable(target_cursor->player, target_cursor->card, 0);
            target_cursor = next_target;
          } while (next_target != ret_location + selected_count);
          if (1 < selected_count)
          {
            instance->number_of_targets = '\0';
            tap_card(player, value);
            tap_card(player, second_creature);
            return 0;
          }
        }
        spell_fizzled = 1;
        return 0;
      }
    }
    else if ((event == EVENT_RESOLVE_ACTIVATION) &&
             (imprint_record = inst_info(instance, 0x59dd), imprint_record.data != 0))
    {
      imprint_record = inst_info(instance, 0x59dd);
      exiled_csvid = cards_data[imprint_record.data].id;
      imprint_record = inst_info(instance, 0x59dd);
      if (check_rfg(((uint32_t)imprint_record.id | ((uint32_t)imprint_record.data2 << 16)) >> 0x10,
                    (unsigned int)exiled_csvid) != 0)
      {
        effect_or_card =
            get_card_instance(player, create_targetted_legacy_effect(player, card, dermotaxi_legacy,
                                                                     instance->parent_controller,
                                                                     instance->parent_card));
        imprint_record = inst_info(instance, 0x59dd);
        set_inst_info(effect_or_card, 0x59dd, imprint_record.data, 0);
        return 0;
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2233510; original cards/modern_horizons_2.c:2374. */
int card_dress_down(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_cards(player, 1);
    if (!eot_trigger(player, card, EVENT_ETB))
      return flash(player, card, event);
    kill_card(player, card, KILL_SACRIFICE);
  }
  else if (eot_trigger(player, card, event))
    kill_card(player, card, KILL_SACRIFICE);
  if ((event != EVENT_ABILITIES) && (event != EVENT_CAST_SPELL))
  {
    return flash(player, card, event);
  }
  instance = in_play(player, card);
  if (instance != NULL)
  {
    instance = in_play(affected_card_controller, affected_card);
    if (instance != NULL)
    {
      result = is_what(affected_card_controller, affected_card, 2);
      if (result != 0)
      {

        instance = get_card_instance(affected_card_controller, affected_card);
        event_result = event_result & 0x9f080000;
        instance->state = instance->state & 0xffffdfff;
        instance->ext->sp_keywords = instance->ext->sp_keywords & 0x993c00c0;
        humiliate(player, card, affected_card_controller, affected_card, 1);
        return flash(player, card, event);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2236070; original cards/modern_horizons_2.c:3203. */
int card_general_ferrous_rokiric(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_humiliated(player, card, AL_TRIG);
    if (result == 0)
    {
      result = count_colors(trigger_cause_controller, trigger_cause);
      if (1 < result)
      {
        sp_ability_trigger(player, event, 2, TYPE_ANY, NULL, player, card);
      }
    }
  }
  if (event != EVENT_SPA)
  {
    misc_hexproof(player, card, event);
    if (((event == EVENT_PROT_MISC) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      result = count_colors(attacking_card_controller, attacking_card);
      if (result == 1)
      {
        event_result = event_result | 1;
      }
    }
    return 0;
  }
  default_token_definition(player, card, 0x4940, &token);
  token.tou = 4;
  token.pow = 4;
  token.color_forced = '0';
  generate_token(&token);
  misc_hexproof(player, card, EVENT_SPA);
  return 0;
}

/* ManalinkEh 2022: 0x22413d0; original cards/modern_horizons_2.c:6815. */
int card_thrasta_tempests_roar(int player, int card, event_t event)
{
  int storm_count;
  card_instance_t *instance;
  inst_info_t entry_record;

  check_legend_rule(player, card, event);
  haste(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    storm_count = get_storm_count();
    COST_COLORLESS = COST_COLORLESS + (char)storm_count * -3;
    trample_over_planeswalkers(player, card, EVENT_MODIFY_COST);
    return 0;
  }
  trample_over_planeswalkers(player, card, event);
  if (event != EVENT_RESOLVE_SPELL)
  {
    if (event == EVENT_ABILITIES)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance = get_card_instance(player, card);
        entry_record = inst_info(instance, 0x5a8f);
        if (entry_record.data2 == 0x42)
        {
          hexproof(player, card, EVENT_ABILITIES);
        }
      }
    }
    else if (event == EVENT_CLEANUP)
    {
      instance = get_card_instance(player, card);
      clear_info(instance, 0x5a8f);
      return 0;
    }
    return 0;
  }
  instance = get_card_instance(player, card);
  set_inst_info(instance, 0x5a8f, 0, 0x42);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222e340; original cards/modern_horizons_2.c:824. */
int card_blacksmiths_skill(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 0x47);
  td.preferred_controller = player;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      instance = get_card_instance(player, card);
      result = is_what(instance->targets[0].player, instance->targets[0].card, 2);
      if ((result == 0) ||
          (result = is_what(instance->targets[0].player, instance->targets[0].card, 0x40),
           result == 0))
      {
        result = 0;
      }
      else
      {
        result = 2;
      }
      pump_ability_until_eot(player, card, instance->targets[0].player,
                             instance->targets[0].card, result, result, 0,
                             (SP_KEYWORD_INDESTRUCTIBLE | SP_KEYWORD_HEXPROOF));
    }
    kill_card(player, card, KILL_BURY);
  }
  return generic_spell(player, card, event, 1, &td, "TARGET_PERMANENT", 1, NULL);
}


/* October 2022 copied local table sources (.data, writable). */
static int garth_card_ids[7] = {-1, 64, 25, 242, 224, 202, 17};
static int serras_emissary_types[9] = {0, 2, 64, 4, 1, 256, 16, 8, 4105};

/* ManalinkEh 2022: 0x2235c00; original cards/modern_horizons_2.c:3161. */
int card_garth_one_eye(int player, int card, event_t event)
{
  int choice;
  int csvid_or_count;
  card_instance_t *inst;
  card_instance_t *inst_00;
  unsigned int lotus_available;
  unsigned int used_spells;
  int *card_id_cursor;
  unsigned int regrowth_available;
  int *copy_cursor;
  unsigned int disenchant_available;
  inst_info_t used_record;
  unsigned int terror_available;
  unsigned int braingeyser_available;
  unsigned int dragon_available;
  int csvids[7];
  check_legend_rule(player, card, event);
  if (event != EVENT_RESOLVE_ACTIVATION)
    return generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  inst = get_card_instance(player, card);
  inst_00 = get_card_instance(inst->parent_controller, inst->parent_card);
  used_record = inst_info(inst, 0x5a04);
  used_spells = ((uint32_t)used_record.id | ((uint32_t)used_record.data2 << 16)) >> 0x10;
  if ((used_record.data2 & 0x3f) == 0x3f)
  {
    return 0;
  }
  card_id_cursor = garth_card_ids;
  copy_cursor = csvids;
  for (csvid_or_count = 7; csvid_or_count != 0; csvid_or_count = csvid_or_count + -1)
  {
    *copy_cursor = *card_id_cursor;
    card_id_cursor = card_id_cursor + 1u;
    copy_cursor = copy_cursor + 1u;
  }
  disenchant_available =
      (used_spells & 2) == 0 &&
      (player == 0 || has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0x40));
  braingeyser_available =
      (used_spells & 4) == 0 &&
      (player == 0 || has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0x19));
  terror_available = (used_spells & 8) == 0 &&
                     (player == 0 || has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0xf2));
  dragon_available = (used_spells & 0x10) == 0 &&
                     (player == 0 || has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0xe0));
  regrowth_available = (used_spells & 0x20) == 0 &&
                       (player == 0 || has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0xca));
  lotus_available = (used_spells & 0x40) == 0 &&
                    (player == 0 || has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0x11));
  csvid_or_count = count_graveyard(player);
  choice = dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\002", "\001\014", "\001\004",
                       "\001\003", "Disenchant", disenchant_available, 5, "Braingeyser",
                       braingeyser_available, hand_count[player] * -10 + 0x46, "Terror",
                       terror_available, 0xf, "Shivan Dragon", dragon_available, 0x14, "Regrowth",
                       regrowth_available, csvid_or_count, "Black Lotus", lotus_available, 0x32, 0,
                       0, 0, 0);
  set_inst_info(inst_00, 0x5a04, 0,
                (unsigned short)(1 << ((unsigned char)choice & 0x1f)) | used_record.data2);
  csvid_or_count = csvids[choice];
  if (charge_mana_from_id(player, -1, EVENT_RESOLVE_ACTIVATION, csvid_or_count) != 0)
  {
    copy_spell(player, csvid_or_count);
  }
  return generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
}

/* ManalinkEh 2022: 0x223dfe0; original cards/modern_horizons_2.c:5774. */
int card_serras_emissary(int player, int card, event_t event)
{
  int choice;
  card_instance_t *instance;
  int result_or_count;
  uint32_t chosen_type;
  int *type_cursor;
  int *copy_cursor;
  inst_info_t protection_record;
  int type_array[9];
  if (event != EVENT_RESOLVE_SPELL)
  {
    if (event == EVENT_ABILITIES)
    {
      if ((affected_card_controller == player) &&
          (result_or_count = is_what(player, affected_card, 2), result_or_count != 0))
      {
        instance = get_card_instance(player, card);
        protection_record = inst_info(instance, 0x5a67);
        chosen_type = protection_record.data;
        if (chosen_type == 2)
        {
          event_result = event_result | 0x200000;
        }
        else if (chosen_type == 0x40)
        {
          event_result = event_result | 0x10000;
        }
        else if (chosen_type == 1)
        {
          event_result = event_result | 0x400000;
        }
        else if (chosen_type == 0x10)
        {
          event_result = event_result | 0x40000;
        }
        else if (chosen_type == 8)
        {
          event_result = event_result | 0x100000;
        }
        else
        {
          instance = get_card_instance(affected_card_controller, affected_card);
          instance->ext->sp_keywords2 = instance->ext->sp_keywords2 | 2;
        }
      }
    }
    else if (event == EVENT_STATIC_EFFECTS)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        player_bits[player] = player_bits[player] | 0x4000000;
      }
    }
    else
    {
      if ((((event == EVENT_PROT_MISC) && (instance = in_play(player, card), instance != NULL)) &&
           (result_or_count = is_humiliated(player, card, AL_STATIC), result_or_count == 0)) &&
          (affected_card_controller == player))
      {
        protection_record = inst_info(instance, 0x5a67);
        if (((-1 < affected_card) &&
             (result_or_count = is_what(affected_card_controller, affected_card, 2),
              result_or_count != 0)) ||
            (affected_card == -1))
        {
          if (((unsigned int)event_result >> 24 & 0xff) == '\x06')
          {
            event_result = event_result | 1;
          }
          if (protection_record.data == 0x1009)
          {
            result_or_count = is_tribal(attacking_card_controller, attacking_card);
          }
          else
          {
            result_or_count =
                is_what(attacking_card_controller, attacking_card, protection_record.data);
          }
          if (result_or_count != 0)
          {
            event_result = event_result | 1;
          }
        }
      }
    }
    return 0;
  }
  type_cursor = serras_emissary_types;
  copy_cursor = type_array;
  for (result_or_count = 9; result_or_count != 0; result_or_count = result_or_count + -1)
  {
    *copy_cursor = *type_cursor;
    type_cursor = type_cursor + 1;
    copy_cursor = copy_cursor + 1;
  }
  choice =
      dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\003", "\001\014", "Creature",
                  1, 10, "Artifact", 1, 10, "Enchantment", 1, 10, "Land", 1, 5, "Planeswalker", 1,
                  10, "Instant", 1, 10, "Sorcery", 1, 10, "Tribal", 1, 1, 0, 0, 0, 0);
  chosen_type = type_array[choice];
  instance = get_card_instance(player, card);
  set_inst_info(instance, 0x5a67, chosen_type, 0);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x223fb20; original cards/modern_horizons_2.c:6351. */
int card_subtlety(int player, int card, event_t event)
{
  int to_counter_card;
  int spell_player;
  card_instance_t *instance;
  int choice;
  target_definition_t td;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {

        if (current_turn == player)
        {
          ai_modifier = ai_modifier + -10;
        }
        instance = get_card_instance(player, card);
        spell_player = card_on_stack_controller;
        if (instance->info_slot == 1)
        {
          instance->number_of_targets = '\x01';
          instance->targets[0].player = spell_player;
          instance->targets[0].card = card_on_stack;
        }
      }
      etb_ability(player, card, EVENT_CAST_SPELL, RESOLVE_TRIGGER_MANDATORY);
    }
    else
    {
      etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
      if (event == EVENT_ETB)
      {
        instance = get_card_instance(player, card);
        if (instance->info_slot == 1)
        {

          counterspell_target_definition(player, card, &td, 0x102);
          instance = get_card_instance(player, card);
          if (counterspell_validate(player, card, &td, 0) != 0)
          {

            spell_player = instance->targets[0].player;
            to_counter_card = instance->targets[0].card;
            if (pre_counterspell(player, card, spell_player, to_counter_card) != 0)
            {

              choice = do_dialog(spell_player, spell_player, to_counter_card, -1, -1,
                                 " Put on top of deck\n Put on bottom of deck", 0);
              action_on_card(player, card, spell_player, to_counter_card,
                             (choice != 0) + ACT_PUT_ON_TOP);
            }
          }
          instance->number_of_targets = '\0';
          if (evoked(player, card) != 0)
          {
            kill_card(player, card, KILL_SACRIFICE);
          }
        }
      }
    }
    return evoke_with_cards_in_hand(player, card, event, 4);
  }
  counterspell_target_definition(player, card, &td, 0x102);
  if (counterspell(player, card, EVENT_CAN_CAST, &td, 1) != 0)
  {
    instance = get_card_instance(player, card);
    instance->info_slot = 1;
    return 99;
  }
  instance = get_card_instance(player, card);
  instance->info_slot = 0;
  return flash(player, card, EVENT_CAN_CAST);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2241730; original cards/modern_horizons_2.c:6903. */
int card_timeless_dragon(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_TIMELESS_DRAGON = 23185
  };

  int result;

  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    result = eternalize(player, card, event, CARD_ID_TIMELESS_DRAGON, 2, 0, 0, 0, 0, 2);
    return result;
  }
  result = landcycling(player, card, event, 2, 0x5007, -1);
  return result;
}

/* ManalinkEh 2022: 0x22417d0; original cards/modern_horizons_2.c:6917. */
int card_timeless_witness(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_TIMELESS_WITNESS = 23186
  };

  int result;
  test_definition_t test;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 0, "Select target card.");
    new_global_tutor(player, player, 2, 1, 1, 1, &test);
  }
  result = eternalize(player, card, event, CARD_ID_TIMELESS_WITNESS, 5, 0, 0, 2, 0, 0);
  return result;
}


/* ManalinkEh 2022: 0x222b7b0; original cards/modern_horizons_2.c:7203. */
static int has_mana_cost_0_or_1(int iid, int val, int player, int card)
{
  unsigned int packed_card_identity;
  card_ptr_t *printed_card;
  int result;

  packed_card_identity = (uint32_t)cards_data[iid].id | ((uint32_t)cards_data[iid].reserved2 << 16);
  result = has_no_mana_cost(-1, iid);
  if ((((result == 0) && (result = is_what(-1, iid, 1), result == 0)) &&
       (printed_card = cards_ptr[packed_card_identity & 0xffff],
        printed_card->req_colorless < 2)) &&
      (((printed_card->req_black == '\0' && (printed_card->req_blue == '\0')) &&
        ((printed_card->req_green == '\0' && (printed_card->req_red == '\0'))))))
  {
    return (unsigned int)(printed_card->req_white == '\0');
  }
  return 0;
}


/* ManalinkEh 2022: 0x2242640; original cards/modern_horizons_2.c:7224. */
int card_urzas_saga(int player, int card, event_t event)
{
  int choice_or_result;
  unsigned int token_available;
  card_instance_t *instance;
  unsigned int mana_available;
  inst_info_t chapter_record;

  if (event == EVENT_CHAPTER1 || event == EVENT_CHAPTER2)
  {
    instance = get_card_instance(player, card);
    chapter_record = inst_info(instance, 0x5a9e);
    set_inst_info(instance, 0x5a9e, 0, chapter_record.data2 | (event == EVENT_CHAPTER1 ? 1 : 2));
    if (event == EVENT_CHAPTER2)
      return saga(player, card, event, 3);
  }
  else if (event == EVENT_CHAPTER3)
  {
    test_definition_t test;
    new_default_test_definition(&test, 0x40, "Select an artifact card with mana cost |0 or |1.");
    test.special_selection_function = has_mana_cost_0_or_1;
    new_global_tutor(player, player, 1, 4, 0, 1, &test);
    return saga(player, card, event, 3);
  }
  else if (event == EVENT_CAN_ACTIVATE || event == EVENT_COUNT_MANA)
  {
    if (card == affected_card && player == affected_card_controller &&
        (instance = in_play(player, card), instance != NULL))
    {
      chapter_record = inst_info(instance, 0x5a9e);
      if (chapter_record.data2 & 1)
        return mana_producer(player, card, event);
    }
  }
  if (event == EVENT_ACTIVATE)
  {
    instance = in_play(player, card);
    instance->mode = '\0';
    chapter_record = inst_info(instance, 0x5a9e);
    mana_available = 0;
    if ((((uint64_t)chapter_record.id | ((uint64_t)chapter_record.data2 << 16) |
          ((uint64_t)chapter_record.data << 32)) &
         0x10000) != 0)
    {
      choice_or_result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      mana_available = (unsigned int)(choice_or_result != 0);
    }
    if (paying_mana() == 0)
    {
      token_available = 0;
      if ((((uint32_t)chapter_record.id | ((uint32_t)chapter_record.data2 << 16)) >> 0x10 & 2) != 0)
      {
        choice_or_result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 3, 0, 0,
                                                     0, 0, 0, 0, NULL, NULL);
        token_available = (unsigned int)(choice_or_result != 0);
      }
    }
    else
    {
      token_available = 0;
    }
    choice_or_result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\033",
                                   "Produce mana", mana_available, 5, "Create a token",
                                   token_available, 10, 0, 0, 0, 0);
    if (choice_or_result == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    if (instance->mode == '\x01')
    {
      return mana_producer(player, card, EVENT_ACTIVATE);
    }
    if (instance->mode == '\x02')
    {
      add_state(player, card, 0x10);
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        remove_state(player, card, 0x10);
      }
      else
      {
        if (((current_turn != player) || (choice_or_result = 0x30, 0x14 < current_phase)) &&
            (choice_or_result = -0x30, current_turn == 1 - player))
        {
          choice_or_result = ((current_phase < 0x17) - 1 & 0xffffffa0) + 0x30;
        }
        ai_modifier = ai_modifier + choice_or_result;
      }
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) &&
           (instance = get_card_instance(player, card), instance->mode == '\x02'))
  {
    token_generation_t token;
    default_token_definition(player, card, 0x4918, &token);
    token.tou = 0;
    token.pow = 0;
    token.special_infos = 66;
    generate_token(&token);
  }
  return saga(player, card, event, 3);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222c7a0; original cards/modern_horizons_2.c:326. */
int card_aeve_progenitor_ooze(int player, int card, event_t event)
{
  int c;
  int token_or_flags;
  card_instance_t *instance;
  int is_ooze;
  int ooze_count;

  if (event == EVENT_QUERY)
  {
    if (attacking_card == 4)
    {
      return 1;
    }
    check_legend_rule(player, card, EVENT_QUERY);
  }
  else
  {
    check_legend_rule(player, card, event);
    if (event == EVENT_CHANGE_TYPE)
    {
      if (((card == affected_card) && (player == affected_card_controller)) &&
          (token_or_flags = is_token(player, card), token_or_flags != 0))
      {
        remove_a_subtype(player, card, SUBTYPE_LEGEND);
      }
    }
    else if ((((event == EVENT_THIS_SPELL_CAST) ||
               (event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE)) &&
              (card == affected_card)) &&
             ((player == affected_card_controller &&
               (token_or_flags = check_special_flags2(player, card, 0x100), token_or_flags == 0))))
    {

      if (active_cards_count[player] < 1)
      {
        ooze_count = 0;
      }
      else
      {
        c = 0;
        ooze_count = 0;
        do
        {
          if ((card != c) && (instance = in_play(player, c), instance != NULL))
          {
            is_ooze = has_subtype(player, c, SUBTYPE_OOZE);
            ooze_count = (ooze_count + 1) - (unsigned int)(is_ooze == 0);
          }
          c = c + 1;
        } while (c < active_cards_count[player]);
      }
      hack_silent_counters = hack_silent_counters + 1;
      add_counters(player, card, player, card, COUNTER_P1_P1, ooze_count);
      hack_silent_counters = hack_silent_counters + -1;
    }
  }
  return 0;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x223e3d0; original cards/modern_horizons_2.c:5853. */
int card_shattered_ego(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  attached_gets_pt(player, card, event, -3, 0);
  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      instance = get_card_instance(player, card);
      put_permanent_under_the_first_x_card_of_its_owners_library(
          (int)instance->damage_target_player, instance->damage_target_card, 3);
    }
    result = aura(player, card, event, NULL, NULL);
    disabling_aura_ai(player, card, event);
    return result;
  }
  result = generic_activated_ability(player, card, event, 0, 3, 0, 2, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x223e650; original cards/modern_horizons_2.c:5913. */
int card_skyblades_boon(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = has_mana_multi(player, 2, 0, 0, 0, 0, 1);
    if (result != 0)
    {
      return 4;
    }
  }
  else
  {
    if (event == EVENT_PAY_FLASHBACK_COSTS)
    {
      charge_mana_multi(player, 2, 0, 0, 0, 0, 1);
      if (spell_fizzled != 1)
      {
        return 1;
      }
      attached_gets_pt(player, card, event, 1, 1);
      attached_gets_kw(player, card, event, KEYWORD_FLYING, SP_KEYWORD_0);
      return aura(player, card, event, NULL, NULL);
    }
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      return generic_activated_ability(player, card, event, 0, 2, 0, 0, 0, 0, 1, 0, NULL, NULL);
    }
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    instance = get_card_instance(player, card);
    bounce_permanent(instance->parent_controller, instance->parent_card);
  }
  attached_gets_pt(player, card, event, 1, 1);
  attached_gets_kw(player, card, event, KEYWORD_FLYING, SP_KEYWORD_0);
  return aura(player, card, event, NULL, NULL);
}

/* ManalinkEh 2022: 0x22438b0; original cards/modern_horizons_2.c:7566. */
int card_world_weary(int player, int card, event_t event)
{
  int result;

  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    attached_gets_pt(player, card, event, -4, -4);
    result = aura(player, card, event, NULL, NULL);
    disabling_aura_ai(player, card, event);
    return result;
  }
  result = basic_landcycling(player, card, event, 1, 0, 0, 0, 1, 0);
  return result;
}
#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2230390; original cards/modern_horizons_2.c:1468. */
int card_captured_by_lagacs(int player, int card, event_t event)
{
  int result;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    support(player, card, 2);
  }
  result = card_pacifism(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2230e60; original cards/modern_horizons_2.c:1773. */
int card_constable_of_the_realm(int player, int card, event_t event)
{
  xtrigger_t trigger;
  int result;
  card_instance_t *instance;
  target_definition_t td;

  return_from_oblivion(player, card, event);
  renown(player, card, event, 2);
  enable_xtrigger_flags = enable_xtrigger_flags | ENABLE_XTRIGGER_1_1_COUNTERS;
  trigger = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    trigger = xtrigger_impl_value_dont_use_directly;
  }
  if ((((trigger == XTRIGGER_1_1_COUNTERS) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (((reason_for_trigger_controller == player && (trigger_cause_controller == player)) &&
        (trigger_cause == card))))
  {
    result = is_humiliated(player, card, AL_TRIG);
    if (result == 0)
    {
      if (event == EVENT_TRIGGER)
      {
        result = duh_mode(player);
        event_result = event_result | (result != 0) + 1;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {
        default_target_definition(player, card, &td, 0x47);
        td.illegal_type = 1;
        result = new_pick_target(&td, "TARGET_NONLAND_PERMANENT", 0, 0);
        if (result != 0)
        {

          instance = get_card_instance(player, card);
          obliviation(player, card, instance->targets[0].player, instance->targets[0].card);
          instance->number_of_targets = '\0';
        }
      }
    }
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2239450; original cards/modern_horizons_2.c:4218. */
int card_loathsome_curator(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  menace(player, card, event);
  result = exploit(player, card, event);
  if (result != 0)
  {
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = 1 - player;
    td.extra = 3;
    td.special = 0x800;
    td.allow_cancel = 0;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(
          &td, "Select target creature you don\'t control with mana value 3 or less.", 0, 0x80);
      if (result != 0)
      {
        kill_target0(player, card, KILL_DESTROY);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x223dd40; original cards/modern_horizons_2.c:5706. */
int card_scurry_oak(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  evolve(player, card, event);
  result = one_or_more_p1_p1_counters_are_putted_on_me(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  default_token_definition(player, card, 0x4991, &token);
  token.tou = 1;
  token.pow = 1;
  token.color_forced = '\b';
  generate_token(&token);
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2236f60; original cards/modern_horizons_2.c:3463. */
int card_glinting_creeper(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  converge(player, card, event);
  instance = get_card_instance(player, card);
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, instance->info_slot);
  if (((event == EVENT_BLOCK_LEGALITY) && (current_turn == player)) && (attacking_card == card))
  {
    result = is_humiliated(player, card, AL_STATIC);
    if (result == 0)
    {
      result = get_power(affected_card_controller, affected_card);
      if (result < 3)
      {
        event_result = event_result | 1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x223bd60; original cards/modern_horizons_2.c:5148. */
int card_radiant_epicure(int player, int card, event_t event)
{
  int amount;
  card_instance_t *instance;

  converge(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  amount = instance->info_slot;
  lose_life(1 - player, amount);
  gain_life(player, amount);
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x223af70; original cards/modern_horizons_2.c:4868. */
int card_out_of_time(int player, int card, event_t event)
{
  int p;
  card_instance_t *instance;
  int creature_count;
  int marked[151][2];

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      creature_count = count_subtype(1 - player, 2, -1);
      ai_modifier = ai_modifier + creature_count * 5;
      creature_count = count_subtype(player, 2, -1);
      ai_modifier = ai_modifier + creature_count * -5;
    }
    etb_ability(player, card, EVENT_CAST_SPELL, RESOLVE_TRIGGER_MANDATORY);
    vanishing(player, card, event, 0);
    return 0;
  }
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  p = current_turn;
  if (event == EVENT_ETB)
  {
    int marked_count = 0;
    for (int scan_card = active_cards_count[current_turn] - 1; scan_card >= 0; --scan_card)
    {
      instance = in_play(p, scan_card);
      if (instance && is_what(p, scan_card, 2))
      {
        marked[marked_count][0] = p;
        marked[marked_count][1] = scan_card;
        ++marked_count;
      }
    }
    int other_player = 1 - current_turn;
    for (int scan_card = active_cards_count[other_player] - 1; scan_card >= 0; --scan_card)
    {
      instance = in_play(other_player, scan_card);
      if (instance && is_what(other_player, scan_card, 2))
      {
        marked[marked_count][1] = scan_card;
        marked[marked_count][0] = other_player;
        ++marked_count;
      }
    }
    for (int index = marked_count - 1; index >= 0; --index)
    {
      int target_card = marked[index][1];
      int target_player = marked[index][0];
      untap_card(target_player, target_card);
      phase_out_target_until_source_leaves_play(player, card, target_player, target_card);
    }
    add_counters(player, card, player, card, COUNTER_TIME, marked_count);
  }
  vanishing(player, card, event, 0);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x222f0a0; original cards/modern_horizons_2.c:1089. */
int card_break_the_ice(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int overload_result;
  target_definition_t td;
  test_definition_t this_test;
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 1);
    td.special = 0x8000;
    td.extra = (int32_t)target_is_snow_or_can_produce_colorless;
    instance = get_card_instance(player, card);
    result = basic_spell(player, card, EVENT_CAN_CAST);
    if (result != 0)
    {
      if (instance->info_slot == 1)
      {
        return 1;
      }
      return can_target(&td);
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 1);
      td.special = 0x8000;
      td.extra = (int32_t)target_is_snow_or_can_produce_colorless;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        result = check_special_flags2(player, card, -0x80000000);
        if ((result != 0) ||
            (overload_result = pay_overload(player, card, 4, 2, 0, 0, 0, 0), overload_result == 1))
        {
          new_pick_target(&td, "Select land that is snow or could produce |C.", 0, 0x81);
          if (spell_fizzled == 1)
          {
            return 0;
          }
          overload_result = 1;
        }
        instance->info_slot = instance->info_slot | overload_result;
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 1);
    td.special = 0x8000;
    td.extra = (int32_t)target_is_snow_or_can_produce_colorless;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      if (((unsigned int)instance->info_slot & 2) == 0)
      {
        result = valid_target(&td);
        if (result != 0)
        {
          kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        }
      }
      else
      {
        default_test_definition(&this_test, 1);
        this_test.special_selection_function = is_snow_or_can_produce_colorless;
        new_manipulate_all(player, card, 2, &this_test, ACT_KILL_DESTROY);
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  return overload(player, card, event, 4, 2, 0, 0, 0, 0);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2231430; original cards/modern_horizons_2.c:1895. */
int card_damn2(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int overload_result;
  target_definition_t td;
  test_definition_t this_test;
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    result = basic_spell(player, card, EVENT_CAN_CAST);
    if (result != 0)
    {
      if (instance->info_slot == 1)
      {
        return 1;
      }
      return can_target(&td);
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        result = check_special_flags2(player, card, -0x80000000);
        if ((result != 0) ||
            (overload_result = pay_overload(player, card, 2, 0, 0, 0, 0, 2), overload_result == 1))
        {
          new_pick_target(&td, "TARGET_CREATURE", 0, 1);
          if (spell_fizzled == 1)
          {
            return 0;
          }
          overload_result = 1;
        }
        instance->info_slot = instance->info_slot | overload_result;
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      if (((unsigned int)instance->info_slot & 2) == 0)
      {
        result = valid_target(&td);
        if (result != 0)
        {
          kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
        }
      }
      else
      {
        default_test_definition(&this_test, 2);
        new_manipulate_all(player, card, 2, &this_test, ACT_KILL_BURY);
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  return overload(player, card, event, 2, 0, 0, 0, 0, 2);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2242b50; original cards/modern_horizons_2.c:7296. */
int card_vectis_gloves(int player, int card, event_t event)
{
  int p;
  int humiliated;
  card_instance_t *instance;
  unsigned int nonbasic_land;
  int card_00;

  if (((((event == EVENT_BLOCK_LEGALITY) &&
         (instance = is_equipping(player, card), instance != NULL)) &&
        (humiliated = is_humiliated(player, card, AL_STATIC), humiliated == 0)) &&
       (((player_bits[player] & PB_NONSTANDARD_LANDWALK_DISABLED) == 0 &&
         (instance = get_card_instance(player, card),
          attacking_card_controller == instance->damage_target_player)))) &&
      (attacking_card == instance->damage_target_card))
  {
    p = 1 - player;
    nonbasic_land = 0;
    if (0 < active_cards_count[p])
    {
      card_00 = 0;
      do
      {
        instance = in_play(p, card_00);
        if (((instance != NULL) && (is_what(p, card_00, 1) != 0)) &&
            (is_what(p, card_00, 0x40) != 0))
        {
          nonbasic_land = 1;
          break;
        }
        card_00 = card_00 + 1;
      } while (card_00 < active_cards_count[p]);
    }
    event_result = event_result | nonbasic_land;
  }
  return vanilla_equipment(player, card, event, 2, 2, 0, 0, 0);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x223c8c0; original cards/modern_horizons_2.c:5364. */
int card_rise_and_shine(int player, int card, event_t event)
{
  int c;
  card_instance_t *instance;
  unsigned int overload_result;
  int result_or_artifact;
  int *marked_cursor;
  bool more_marked;
  int marked_count;
  target_definition_t td;
  int marked[151];
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x40);
    td.illegal_type = 2;
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
    if (basic_spell(player, card, EVENT_CAN_CAST) != 0)
    {
      if (instance->info_slot == 1)
      {
        return 1;
      }
      return generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x40);
      td.illegal_type = 2;
      td.allowed_controller = player;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        result_or_artifact = check_special_flags2(player, card, -0x80000000);
        if ((result_or_artifact != 0) ||
            (overload_result = pay_overload(player, card, 4, 0, 2, 0, 0, 0), overload_result == 1))
        {
          new_pick_target(&td, "Select targer noncreature artifact", 0, 0x81);
          if (spell_fizzled == 1)
          {
            return 0;
          }
          overload_result = 1;
        }
        instance->info_slot = instance->info_slot | overload_result;
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x40);
    td.illegal_type = 2;
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      if (((unsigned int)instance->info_slot & 2) == 0)
      {
        if (valid_target(&td) != 0)
        {
          add_counters(player, card, instance->targets[0].player, instance->targets[0].card,
                       COUNTER_P1_P1, 4);
          artifact_animation(player, card, instance->targets[0].player, instance->targets[0].card,
                             0, 0, 0, 0, 0);
        }
      }
      else
      {
        if (0 < active_cards_count[player])
        {
          marked_count = 0;
          for (c = 0; c < active_cards_count[player]; ++c)
            if ((instance = in_play(player, c), instance != NULL) && is_what(player, c, 0x40) &&
                !is_what(player, c, 2))
              marked[marked_count++] = c;
          if (0 < marked_count)
          {
            marked_cursor = marked + marked_count + -1;
            do
            {
              result_or_artifact = *marked_cursor;
              add_counters(player, card, player, result_or_artifact, COUNTER_P1_P1, 4);
              artifact_animation(player, card, player, result_or_artifact, 0, 0, 0, 0, 0);
              more_marked = marked != marked_cursor;
              marked_cursor = marked_cursor + -1;
            } while (more_marked);
          }
        }
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  return overload(player, card, event, 4, 0, 2, 0, 0, 0);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2242d30; original cards/modern_horizons_2.c:7339. */
int card_verdant_command(int player, int card, event_t event)
{
  typedef enum
  {
    CHOICE_INVALID_TARGET = 0,
    CHOICE_SQUIRREL = 1,
    CHOICE_COUNTER_PW_ABIL = 2,
    CHOICE_EXILE_CARD = 3,
    CHOICE_3_LIFE = 4
  } CommandChoices;
  CommandChoices choice;
  int result;
  int opponent;
  const char *prompt;
  card_instance_t *instance;
  test_definition_t *grave_test;
  target_definition_t *player_target;
  int counter_available;
  int priority3;
  CommandChoices choices[2];
  int abils[5];
  // Native 0x022430df-0x0224311f reads these without initialization on the human path.
  int priority[5];
  token_generation_t token;
  target_definition_t td_spell;
  target_definition_t td_player;
  test_definition_t test;

  if (event != EVENT_CAN_CAST && event != EVENT_CAST_SPELL && event != EVENT_RESOLVE_SPELL &&
      event != EVENT_CAN_CHANGE_TARGET && event != EVENT_CHANGE_TARGET)
    return 0;
  if (event == EVENT_CAST_SPELL && (card != affected_card || player != affected_card_controller))
    return 0;
  counter_activated_target_definition(player, card, &td_spell, 0x100);
  counter_available = can_counter_activated_ability(player, card, event, &td_spell);
  default_target_definition(player, card, &td_player, 0);
  td_player.zone = 0x1000;
  new_default_test_definition(&test, 0xff, "Select target card to exile.");
  instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    abils[0] = 0x1010101;
    abils[1] = 0x1010101;
    abils[2] = 0x1010101;
    abils[3] = 0x1010101;
    abils[4] = 0x1010101;
    choices[0] =
        dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\005", choices + 1, "\001\015",
                    "\001\002", "2 tapped Squirrels", 0x1010101, 0x1010101,
                    "Counter PW loyalty ability", 0x1010101, 0x1010101, "Exile card in grave",
                    0x1010101, 0x1010101, "Gain 3 life", 0x1010101, 0x1010101, 0, 0, 0, 0);

    int target_index = 0;
    for (int choice_index = 0; choice_index < 2; ++choice_index)
    {
      switch (choices[choice_index])
      {
      case CHOICE_INVALID_TARGET:
        ++target_index;
        break;
      case CHOICE_SQUIRREL:
        if (validate_target(player, card, &td_player, target_index))
        {
          default_token_definition(player, card, 0x4991, &token);
          token.t_player = instance->targets[target_index].player;
          token.tou = 1;
          token.pow = 1;
          token.color_forced = 8;
          token.qty = 2;
          token.action = TOKEN_ACTION_TAPPED;
          generate_token(&token);
        }
        ++target_index;
        break;
      case CHOICE_COUNTER_PW_ABIL:
        resolve_counter_activated_ability(player, card, &td_spell, target_index);
        ++target_index;
        break;
      case CHOICE_EXILE_CARD:
      {
        int grave_index = validate_target_from_grave_source(
            player, card, instance->targets[target_index].player, target_index + 1);
        if (grave_index != -1)
          rfg_card_from_grave(instance->targets[target_index].player, grave_index);
        target_index += 2;
        break;
      }
      case CHOICE_3_LIFE:
        if (validate_target(player, card, &td_player, target_index))
          gain_life(instance->targets[target_index].player, 3);
        // The native fourth mode does not advance the target index.
        break;
      }
    }
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  grave_test = &test;
  player_target = &td_player;
  abils[1] = generic_spell(player, card, EVENT_CAN_CAST, 1, player_target, NULL, 1, NULL);
  abils[2] = (int)(counter_available != 0);
  abils[3] = generic_spell(player, card, EVENT_CAN_CAST, 0x1000, NULL, NULL, 1, grave_test);
  abils[4] = abils[1];
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    opponent = 1 - player;
    result = count_subtype(opponent, 2, -1);
    priority[1] = result * 5;
    priority[2] = (-(unsigned int)(abils[2] == 0) & 0xfff0bdc1) + 1000000;
    result = graveyard_has_shroud(opponent);
    priority3 = 5;
    if (result == 0)
    {
      priority3 = count_graveyard(opponent);
    }
    priority[4] = ((life[player] < 6) - 1 & 0xffffffd8) + 0x32;
  }
  else
  {
    priority3 = priority[3];
  }
  choice = dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                       "2 tapped Squirrels", abils[1], priority[1], "Counter PW loyalty ability",
                       abils[2], priority[2], "Exile card in grave", abils[3], priority3,
                       "Gain 3 life", abils[4], priority[4], 0, 0, 0, 0);
  if (event == EVENT_CAN_CAST)
  {
    if (counter_available != 0)
    {
      choice = 99;
    }
    return choice;
  }
  choices[0] = choice;

  if (event != EVENT_CAST_SPELL || card != affected_card || player != affected_card_controller)
    return 0;
  instance->number_of_targets = 0;
  int target_index = 0;
  for (int choice_index = 0; choice_index < 2; ++choice_index)
    switch (choices[choice_index])
    {
    case CHOICE_INVALID_TARGET:
      spell_fizzled = 1;
      break;
    case CHOICE_SQUIRREL:
    case CHOICE_3_LIFE:
      prompt = choices[choice_index] == CHOICE_SQUIRREL ? "Select target player to get squirrels."
                                                        : "Select target player to gain life.";
      td_player.preferred_controller = player;
      if (!new_pick_target(player_target, prompt, target_index, 0x81))
        return 0;
      ++target_index;
      break;
    case CHOICE_COUNTER_PW_ABIL:
      cast_counter_activated_ability(player, card, target_index);
      ++target_index;
      break;
    case CHOICE_EXILE_CARD:
      if (select_target_from_either_grave(player, card, SFG_0, 3, 1, grave_test, target_index,
                                          target_index + 1) == -1)
      {
        spell_fizzled = 1;
        return 0;
      }
      target_index += 2;
      break;
    }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x222b230; original cards/modern_horizons_2.c:1987. */
static int create_dauthi_voidwalker_legacy(int player, int card, int iid)
{
  int legacy =
      create_legacy_effect_for_opponent(player, card, card_exiled_with_void_counter, -1, -1);
  if (legacy != -1)
  {
    int owner = 1 - player;
    add_counters(player, card, owner, legacy, COUNTER_VOID, 1);
    card_instance_t *leg = get_card_instance(owner, legacy);
    int source_card = leg->damage_source_card;
    unsigned csvid = (uint16_t)cards_data[iid].id;
    leg->display_pic_csv_id = csvid;
    leg->targets[0].card = iid;
    leg->targets[0].player = owner;
    leg->eot_toughness = 0x300;
    leg->display_pic_num =
        (uint16_t)get_card_image_number(csvid, leg->damage_source_player, source_card);
  }
  return legacy;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2231790; original cards/modern_horizons_2.c:2001. */
int card_dauthi_voidwalker(int player, int card, event_t event)
{
  shadow(player, card, event);
  int opponent = 1 - player;
  if (if_a_card_would_be_put_into_graveyard_from_library_do_something_instead(
          player, card, event, opponent, RESOLVE_TRIGGER_MANDATORY, NULL))
  {
    create_dauthi_voidwalker_legacy(player, card,
                                    deck_ptr[trigger_cause_controller][trigger_cause]);
    rfg_card_in_deck(trigger_cause_controller, trigger_cause);
    replace_milled = 1;
  }
  if (if_a_card_would_be_put_into_graveyard_from_anywhere_but_library_do_something_instead(
          player, card, event, opponent, RESOLVE_TRIGGER_MANDATORY, NULL))
  {
    create_dauthi_voidwalker_legacy(player, card, trigger_cause);
    add_card_to_rfg(trigger_cause_controller, trigger_cause);
    play_sound_effect(WAV_DESTROY);
    replace_milled = 1;
  }
  if (event == EVENT_ACTIVATE)
  {
    int count = 0;
    for (int c = 0; c < active_cards_count[opponent]; ++c)
      if (in_play(opponent, c) && is_what(opponent, c, 0x80) &&
          count_counters(opponent, c, COUNTER_VOID))
        ++count;
    ai_modifier += count * 10;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    int exiled[31][3] = {{0}};
    if (active_cards_count[opponent] > 0)
    {
      int best_index = -1, max_value = -99, count = 0;
      for (int c = 0; c < active_cards_count[opponent] && count < 31; ++c)
      {
        if (in_play(opponent, c) && is_what(opponent, c, 0x80) &&
            count_counters(opponent, c, COUNTER_VOID))
        {
          card_instance_t *leg = get_card_instance(opponent, c);
          int iid = leg->targets[0].card, owner = leg->targets[0].player;
          if (check_rfg(owner, (uint16_t)cards_data[iid].id))
          {
            int found = 0;
            for (int d = 0; d <= count; ++d)
              if (exiled[d][1] == iid)
              {
                found = 1;
                break;
              }
            if (!found)
            {
              ++count;
              exiled[count][0] = owner;
              exiled[count][1] = iid;
              exiled[count][2] = c;
              int value = my_base_value_by_id((uint16_t)cards_data[iid].id);
              if (value > max_value)
              {
                max_value = value;
                best_index = count;
              }
            }
          }
        }
      }
      if (count != 0)
      {
        const char *labels[15];
        int weights[15];
        for (int i = 14; i >= 0; --i)
          labels[i] = count > i ? cards_data[exiled[i + 1][1]].name : " ";
        int choice;
        for (;;)
        {
          for (int i = 14; i >= 0; --i)
            weights[i] = my_base_value_by_id((uint16_t)cards_data[exiled[i + 1][1]].id);
          choice = dialog_impl(
              player, card, EVENT_RESOLVE_ACTIVATION, "\001\014", "\001\002", "\001\004",
              "\001\003", labels[0], count > 0, weights[0], labels[1], count > 1, weights[1],
              labels[2], count > 2, weights[2], labels[3], count > 3, weights[3], labels[4],
              count > 4, weights[4], labels[5], count > 5, weights[5], labels[6], count > 6,
              weights[6], labels[7], count > 7, weights[7], labels[8], count > 8, weights[8],
              labels[9], count > 9, weights[9], labels[10], count > 10, weights[10], labels[11],
              count > 11, weights[11], labels[12], count > 12, weights[12], labels[13], count > 13,
              weights[13], labels[14], count > 14, weights[14], "Continue >>", count > 15,
              (best_index < 16 ? 0 : 999), 0, 0, 0, 0);
          if (choice != 16)
            break;
          int allow_back =
              ai_is_speculating != 1 && (player != 1 || ((uint32_t)trace_mode >> 1 & 1));
          const char *saved_labels[15];
          for (int i = 0; i < 15; ++i)
            saved_labels[i] = labels[i];
          for (int i = 14; i >= 0; --i)
          {
            int iid = exiled[i + 16][1];
            weights[i] = my_base_value_by_id((uint16_t)cards_data[iid].id);
            labels[i] = count > i + 15 ? cards_data[iid].name : " ";
          }
          int second = dialog_impl(
              player, card, EVENT_RESOLVE_ACTIVATION, "\001\014", "\001\002", "\001\004",
              "\001\003", labels[0], count > 15, weights[0], labels[1], count > 16, weights[1],
              labels[2], count > 17, weights[2], labels[3], count > 18, weights[3], labels[4],
              count > 19, weights[4], labels[5], count > 20, weights[5], labels[6], count > 21,
              weights[6], labels[7], count > 22, weights[7], labels[8], count > 23, weights[8],
              labels[9], count > 24, weights[9], labels[10], count > 25, weights[10], labels[11],
              count > 26, weights[11], labels[12], count > 27, weights[12], labels[13], count > 28,
              weights[13], labels[14], count > 29, weights[14], "<<Back", allow_back, -999, 0, 0, 0,
              0);
          for (int i = 0; i < 15; ++i)
            labels[i] = saved_labels[i];
          if (second != 16)
          {
            choice = second + 15;
            break;
          }
        }
        create_may_play_card_from_exile_effect(player, card, exiled[choice][0],
                                                    (uint16_t)cards_data[exiled[choice][1]].id,
                                                    MPCFE_UNTIL_EOT | MPCFE_FOR_FREE);
      }
    }
  }
  return generic_activated_ability(player, card, event, 9, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 inline-only helpers; native code remains in their callers. */
static int can_produce_colorless(int player, int card)
{
  return get_card_instance(player, card)->card_color & 0x41;
}
#endif


#ifndef SHANDALAR
/* Recovered October 2022 inline helpers; existing native callers contain the expansions. */
static void archon_of_cruelty_effect(int player, int card)
{
  if (opponent_is_valid_target(player, card))
  {
    int opponent = 1 - player;
    player_sacrifices_a_permanent(player, card, opponent, 0x102, SAC_CAUSED);
    new_multidiscard(opponent, 1, player, 2);
    lose_life(opponent, 3);
    draw_cards(player, 1);
    gain_life(player, 3);
  }
}
#endif


#ifndef SHANDALAR
/* Recovered October 2022 private inline definitions. See per-helper limits in the recovery evidence. */
static int check_targets_for_ward(int player, int card, int ctc_player, int ctc_card, int t_player, int t_card, type_t type, subtype_t subtype, int flags)
{
  card_instance_t *instance = get_card_instance(ctc_player, ctc_card);
  for (int i = 0; i < (int)(unsigned int)instance->number_of_targets; ++i)
  {
    int p = instance->targets[i].player;
    int c = instance->targets[i].card;
    if (p == t_player && c != -1 && (t_card == c || t_card == -1) &&
        (player != p || card != c || !(flags & 2)) &&
        (type == TYPE_NONE || is_what(p, c, type)) &&
        (subtype == SUBTYPE_INVALID || has_subtype(p, c, subtype)))
      return 2;
  }
  return 0;
}

static void ward_action(int player, int card, event_t event, int tcp, int tcc, int mode, int flags, int cless, int black, int blue, int green, int red, int white, int life_to_pay)
{
  int result;
  unsigned int must_counter;
  int saved_fizzle = spell_fizzled;
  char buffer[100];
  (void)mode;
  must_counter = flags & 1;
  if (must_counter == 0)
  {
    must_counter = 1;
  }
  else if (-1 < hand_count[trigger_cause_controller])
  {

    discard(trigger_cause_controller, 6, player);
    must_counter = (unsigned int)(spell_fizzled == 1);
  }
  spell_fizzled = saved_fizzle;
  if ((life_to_pay != 0) && (result = can_pay_life(tcp, life_to_pay), result != 0))
  {

    scnprintf(buffer, 100, "Pay %d life", life_to_pay);
    result = dialog_impl(player, card, event, "\001\002", "\001\003", "\001\014",
                        "\001\010", tcp, buffer, 1,
                        ((5 < life[tcp] - life_to_pay) - 1 & 0xfffffff1) + 0x14, "Decline", 1,
                        10, 0, 0, 0, 0);
    if (result == 1)
    {
      must_counter = 0;
      lose_life(tcp, life_to_pay);
    }
  }
  if (((((((cless < 1) && (black < 1)) && (blue < 1)) && ((green < 1 && (red < 1)))) &&
        (white < 1)) ||
       ((result = has_mana_multi(tcp, cless, black, blue, green, red, white),
         result == 0 || (result = charge_mana_multi(tcp, cless, black, blue, green, red, white),
                        result == 0)))) &&
      (must_counter != 0))
  {
    real_counter_a_spell(player, card, tcp, tcc);
  }
}
#endif
