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
  unsigned int uVar1;
  unsigned int uVar2;
  uVar1 = modular(player, card, event, 2);
  uVar2 = generic_shade_merge_pt(player, card, event, 0, 0, 0, 0, 0, 1, 0, 1, 0);
  return uVar2 | uVar1;
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
      *(unsigned int *)(&player_bits + player * 4) =
          *(unsigned int *)(&player_bits + player * 4) | 0x20000;
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
    result = hand_count[player];
    if (result < 3)
      goto LAB_0222efdd;
    result = 2;
    do
    {
      card_00 = new_select_a_card(player, player, 4, 1, 3, -1, &this_test);
      if (card_00 != -1)
      {
        put_on_top_of_deck(player, card_00);
        result = result + -1;
      }
    LAB_0222efdd:
    } while (0 < result);
  }
  result = generic_activated_ability(player, card, event, 9, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
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
      LAB_02230038:
        result = do_flashback(player, card, event, 3, 0, 0, 0, 2, 0);
        return result;
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
        goto LAB_0222ffeb;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_02230038;
    }
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
LAB_0222ffeb:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target creature, planeswalker or player.", 1, NULL);
  return result;
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
        goto LAB_02231026;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x44);
  get_card_instance(player, card);
LAB_02231026:
  result = generic_spell(player, card, event, 1, &td, "DISENCHANT", 1, NULL);
  return result;
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
  short sVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  test_definition_t this_test;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        new_default_test_definition(&this_test, 2, "Select a creature card.");
        if ((card != affected_card) || (player != affected_card_controller))
          goto LAB_0223399b;
        generic_spell(player, card, EVENT_CAST_SPELL, 8, NULL, NULL, 1, &this_test);
        if ((spell_fizzled != 1) && (ai_is_speculating == 1))
        {
          piVar2 = graveyard_ptr_mutable[player];
          iVar3 = validate_target_from_grave_source(player, card, player, 0);
          this_test.id = (int)cards_data[piVar2[iVar3]].id;
          sVar1 = cards_ptr[piVar2[iVar3]]->ai_base_value;
          iVar3 = new_special_count_grave(player, &this_test);
          ai_modifier = ai_modifier + sVar1 * iVar3;
        }
      }
      return 0;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      new_default_test_definition(&this_test, 2, "Select a creature card.");
      if (event == EVENT_RESOLVE_SPELL)
      {
        iVar3 = validate_target_from_grave_source(player, card, player, 0);
        if (iVar3 != -1)
        {
          iVar3 = graveyard_ptr_mutable[player][iVar3];
          iVar4 = count_graveyard(player);
          iVar4 = iVar4 + -1;
          if (-1 < iVar4)
          {
            do
            {
              if (graveyard_ptr_mutable[player][iVar4] == iVar3)
              {
                from_grave_to_hand(player, iVar4, 1);
              }
              iVar4 = iVar4 + -1;
            } while (iVar4 != -1);
          }
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_0223399b;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  new_default_test_definition(&this_test, 2, "Select a creature card.");
LAB_0223399b:
  iVar3 = generic_spell(player, card, event, 8, NULL, NULL, 1, &this_test);
  return iVar3;
}

/* ManalinkEh 2022: 0x2233f70; original cards/modern_horizons_2.c:2614. */
int card_fae_offering(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  iVar1 = eot_trigger(player, card, event);
  if (iVar1 != 0)
  {
    iVar1 = get_stormcreature_count(player);
    if (iVar1 != 0)
    {
      iVar2 = get_specific_storm_count(player);
      if (iVar1 < iVar2)
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
        goto LAB_02234a26;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 3);
  get_card_instance(player, card);
LAB_02234a26:
  result = generic_spell(player, card, event, 0x81, &td, "Select target creature or planeswalker.",
                         1, NULL);
  return result;
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
  unsigned int uVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int card_00;
  card_instance_t *pcVar4;
  int iVar5;
  target_definition_t td;

  if (event == EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    iVar3 = has_mana_multi(player, 0, 0, 1, 0, 0, 0);
    if (iVar3 != 0)
    {
      return 1;
    }
  LAB_022368b6:
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      goto LAB_022368c9;
  }
  else
  {
    if (event != EVENT_ACTIVATE_FROM_HAND)
    {
      if (event == EVENT_RESOLVE_ACTIVATION_FROM_HAND)
      {
        if (spell_fizzled == 1)
        {
          return 0;
        }
        iVar3 = get_internal_card_id_from_csv_id(0x5a07);
        card_00 = add_card_to_hand(player, iVar3);
        pcVar2 = get_card_instance(player, card_00);
        uVar1 = pcVar2->state;
        hand_count[player] = hand_count[player] + -1;
        pcVar2->state = uVar1 | 0x20;
        default_target_definition(player, card_00, &td, 2);
        td.preferred_controller = player;
        pcVar4 = get_card_instance(player, card);
        iVar3 = pcVar4->targets[0].player;
        while ((iVar3 != 0 && (iVar5 = new_pick_target(&td, "TARGET_CREATURE", 0, 0), iVar5 != 0)))
        {
          pump_ability_until_eot(player, card_00, pcVar2->targets[0].player,
                                 pcVar2->targets[0].card, 0, 0, 0x20, 0);
          iVar3 = iVar3 + -1;
          pcVar2->number_of_targets = '\0';
        }
        obliterate_card(player, card_00);
        return 0;
      }
      goto LAB_022368b6;
    }
    charge_mana_multi(player, -1, 0, 1, 0, 0, 0);
    if (spell_fizzled != 1)
    {
      pcVar2 = get_card_instance(player, card);
      pcVar2->targets[0].player = x_value;
      discard_card(player, card);
      return 1;
    }
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
LAB_022368c9:
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.special = 0x4000;
  pcVar2 = get_card_instance(player, card);
  if ((event == EVENT_RESOLVE_ACTIVATION) && (iVar3 = valid_target(&td), iVar3 != 0))
  {
    pump_ability_until_eot(player, card, pcVar2->targets[0].player, pcVar2->targets[0].card, 0, 0,
                           0x20, 0);
  }
  iVar3 = generic_activated_ability(player, card, event, 4, 2, 0, 1, 0, 0, 0, 0, &td,
                                    "Select another target creature.");
  return iVar3;
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
    goto LAB_02236c06;
  }
  if (event == EVENT_ACTIVATE)
  {
  LAB_02236c01:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02236c06;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_02236c01;
  }
  draw_cards(player, 1);
LAB_02236c06:
  result = generic_activated_ability(player, card, event, 1, 1, 0, 0, 0, 0, 0, 1, NULL, NULL);
  return result;
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
        goto LAB_02238a6e;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  new_default_test_definition(&this_test, 2, "Select target creature card.");
  this_test.ai_selection_mode = 2;
LAB_02238a6e:
  result = generic_spell(player, card, event, 8, NULL, NULL, 0, &this_test);
  return result;
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
        goto LAB_0223b26b;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  new_default_test_definition(&this_test, 2, "Select target nonlegendary creature card.");
  this_test.ai_selection_mode = 2;
  this_test.subtype = 0x2001;
  this_test.subtype_flag = 1;
LAB_0223b26b:
  result = generic_spell(player, card, event, 8, NULL, NULL, 0, &this_test);
  return result;
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
        goto LAB_0223c626;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_0223c626:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
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
    if (event != EVENT_ACTIVATE)
      goto LAB_0223cdce;
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
  LAB_0223cdce:
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        tap_card(instance->targets[0].player, instance->targets[0].card);
      }
    }
    result =
        generic_activated_ability(player, card, event, 5, 1, 0, 0, 0, 0, 0, 0, &td, "TARGET_LAND");
    return result;
  }
  return 0;
}

/* ManalinkEh 2022: 0x223d690; original cards/modern_horizons_2.c:5576. */
int card_sanctifier_en_vec(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int grave_id;
  test_definition_t this_test;
  default_test_definition(&this_test, 0xff);
  this_test.color = get_sleighted_color_test(player, card, 0x12);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    iVar1 = 0;
    do
    {
      iVar2 = count_graveyard(iVar1);
      grave_id = iVar2 + -1;
      if (-1 < iVar2 + -1)
      {
        do
        {
          while (iVar2 =
                     new_make_test(iVar1, graveyard_ptr_mutable[iVar1][grave_id], -1, &this_test),
                 iVar2 != 0)
          {
            iVar2 = grave_id + -1;
            rfg_card_from_grave(iVar1, grave_id);
            grave_id = iVar2;
            if (iVar2 == -1)
              goto LAB_0223d7c6;
          }
          grave_id = grave_id + -1;
        } while (grave_id != -1);
      }
    LAB_0223d7c6:
      iVar1 = iVar1 + 1;
    } while (iVar1 != 2);
  }
  iVar1 = if_a_card_would_be_put_into_graveyard_from_library_do_something_instead(
      player, card, event, 2, RESOLVE_TRIGGER_MANDATORY, &this_test);
  if (iVar1 != 0)
  {
    rfg_top_card_of_deck(trigger_cause_controller);
    replace_milled = 1;
  }
  iVar1 = if_a_card_would_be_put_into_graveyard_from_anywhere_but_library_do_something_instead(
      player, card, event, 2, RESOLVE_TRIGGER_MANDATORY, &this_test);
  if (iVar1 != 0)
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
      LAB_0223fa58:
        result = do_flashback(player, card, event, 2, 0, 0, 0, 1, 0);
        return result;
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
      goto LAB_0223fa58;
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
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
        goto LAB_0223ff16;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_0223ff16:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x222b780; original cards/modern_horizons_2.c:6494. */
static char *target_you_own(int who_chooses, int player, int card, int targeting_player,
                            int targeting_card)
{
  int result;
  char *pcVar2;
  result = get_owner(player, card);
  pcVar2 = NULL;
  if (result != targeting_player)
  {
    pcVar2 = "owner";
  }
  return pcVar2;
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
        goto LAB_02241bfd;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.zone = 0x1000;
  get_card_instance(player, card);
LAB_02241bfd:
  result = generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
  return result;
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
        goto LAB_022423d5;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x102);
LAB_022423d5:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE_OR_PLANESWALKER", 1, NULL);
  return result;
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
