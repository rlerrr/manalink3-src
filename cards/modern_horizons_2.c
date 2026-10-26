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
      ;
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
  unsigned int uVar3;
  unsigned int uVar4;
  unsigned int player_00;
  unsigned int player_01;
  target_definition_t *ptStack_140;
  target_definition_t td2;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = get_trap_condition(player, 0x203);
      if (result != 0)
      {
        result = get_updated_casting_cost(player, card, -1, EVENT_MODIFY_COST, 0);
        result = has_mana_hybrid(player, 1, 1, 4, result);
        if (result != 0)
        {
          instance = get_card_instance(player, card);
          instance->info_slot = 1;
          null_casting_cost(player, card);
          goto LAB_0222db7c;
        }
      }
      infinite_casting_cost();
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
    }
  LAB_0222db7c:
    etb_ability(player, card, EVENT_MODIFY_COST, RESOLVE_TRIGGER_MANDATORY);
  LAB_0222db94:
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
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
        result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 0, 0, 0, 0, 0, 0, 0,
                                           &td2, NULL);
        return result;
      }
      goto LAB_0222dace;
    }
  }
  else
  {
    if (event != EVENT_CAST_SPELL)
    {
      etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
      if (event == EVENT_ETB)
      {
        new_default_test_definition(&test, 0xff, "Select a card named The Underworld Cookbook.");
        test.id = 0x5a8b;
        new_global_tutor(player, player, 1, 1, 0, 5, &test);
        return 0;
      }
      goto LAB_0222db94;
    }
    if (((((card == affected_card) && (player == affected_card_controller)) &&
          (result = check_special_flags(player, card, 0x40000), result == 0)) &&
         ((result = is_token(player, card),
           result == 0 && (result = check_special_flags(player, card, 0x80000), result == 0)))) &&
        (instance = get_card_instance(player, card), instance->info_slot == 1))
    {
      charge_mana_hybrid(player, card, 1, 1, 4, 0);
    }
    etb_ability(player, card, EVENT_CAST_SPELL, RESOLVE_TRIGGER_MANDATORY);
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
  default_target_definition(player, card, &td2, 2);
  instance = get_card_instance(player, card);
LAB_0222dace:
  ptStack_140 = &td2;
  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&test, 0x47, "Select a Food to sacrifice.");
    test.subtype = 0x3008;
    uVar3 =
        new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
    if (uVar3 != 0)
    {
      uVar4 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                            &test);
      player_00 = (int)uVar3 >> 0x10 & 0xff;
      state_untargettable(player_00, uVar3 >> 0x18, 0);
      if (uVar4 != 0)
      {
        player_01 = (int)uVar4 >> 0x10 & 0xff;
        state_untargettable(player_01, uVar4 >> 0x18, 0);
        if (spell_fizzled == 1)
        {
          return 0;
        }
        generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 0, 0, 0, 0, 0, 0, 0, ptStack_140,
                                  "TARGET_CREATURE");
        if (spell_fizzled == 1)
        {
          return 0;
        }
        kill_card(player_00, uVar3 >> 0x18, KILL_SACRIFICE);
        kill_card(player_01, uVar4 >> 0x18, KILL_SACRIFICE);
        return 0;
      }
    }
    spell_fizzled = 1;
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(ptStack_140), result != 0))
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
  unsigned int uVar2;
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
      result = new_can_sacrifice_as_cost(player, card, &test);
      return result;
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
        uVar2 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                              &test);
        if (uVar2 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_ANY", 1, NULL);
        card_00 = uVar2 >> 0x18;
        uVar2 = uVar2 >> 0x10 & 0xff;
        if (spell_fizzled == 1)
        {
          state_untargettable(uVar2, card_00, 0);
        }
        else
        {
          result = is_what(uVar2, card_00, 0x40);
          instance->info_slot = result;
          kill_card(uVar2, card_00, KILL_SACRIFICE);
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
  int result;
  card_instance_t *instance;
  uint8_t uVar3;
  target_definition_t td;

  store_num_colors_paid_in_eot_toughness0(player, card, event);
  if ((((trigger_condition == 0xd3) && (reason_for_trigger_controller == player)) &&
       (trigger_cause_controller == player)) &&
      ((trigger_cause == card && (event == EVENT_TRIGGER))))
  {
    result = 1;
    uVar3 = '\0';
    do
    {
      if (0 < mana_paid[result])
      {
        uVar3 = uVar3 + '\x01';
      }
      result = result + 1;
    } while (result != 6);
    instance = get_card_instance(player, card);
    instance->eot_toughness0 = uVar3;
  LAB_022386b8:
    if (event != EVENT_CHANGE_TARGET)
    {
    LAB_02238730:
      result = do_flashback(player, card, event, 4, 0, 0, 0, 1, 0);
      return result;
    }
  }
  else if (event != EVENT_CAN_CAST)
  {
    if (event != EVENT_CAST_SPELL)
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
            instance = get_card_instance(player, card);
            damage_target0(player, card, (unsigned int)instance->eot_toughness0);
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
        goto LAB_022386e3;
      }
      goto LAB_022386b8;
    }
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_02238730;
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
LAB_022386e3:
  result = generic_spell(player, card, event, 1, &td, "TARGET_ANY", 1, NULL);
  return result;
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
        goto LAB_0223b95f;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.illegal_type = 1;
LAB_0223b95f:
  result =
      generic_spell(player, card, event, 0x81, &td, "Select target nonland permanent.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x223bdf0; original cards/modern_horizons_2.c:5167. */
int card_ragavan_nimble_pilferer(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  card_instance_t *instance;
  unsigned int uVar4;
  int iStack_20;
  unsigned int *puStack_1c;
  int times_damaged[2];

  check_legend_rule(player, card, event);
  dash(player, card, event, 1, 0, 0, 0, 1, 0);
  result = damage_dealt_by_me(player, card, event,
                              DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                  DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result != 0)
  {
    instance = get_card_instance(player, card);
    iStack_20 = 0;
    uVar1 = instance->targets[1].player;
    instance->targets[1].player = 0;
    puStack_1c = (unsigned int *)times_damaged;
    uVar4 = uVar1 & 0xff;
    times_damaged[1] = uVar1 >> 8 & 0xff;
    times_damaged[0] = uVar4;
    while (true)
    {
      while (uVar4 != 0)
      {
        generate_token_by_id(player, card, 0x4998);
        exile_top_x_card_you_may_play_them(player, card, 1, NULL, iStack_20, MPCFE_UNTIL_EOT);
        uVar4 = uVar4 - 1;
        *puStack_1c = uVar4;
      }
      iStack_20 = iStack_20 + 1;
      puStack_1c = puStack_1c + 1;
      if (iStack_20 == 2)
        break;
      uVar4 = *puStack_1c;
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
  int iVar1;
  int iVar2;
  token_generation_t token;
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
          iVar1 = validate_target_from_grave_source(player, card, player, 0);
          if (iVar1 != -1)
          {
            default_token_definition(player, card, 0x4908, &token);
            token.tou = 1;
            token.pow = 1;
            token.color_forced = ' ';
            iVar2 = get_base_toughness_iid(player, graveyard_ptr_mutable[player][iVar1]);
            token.qty = (unsigned char)iVar2;
            rfg_card_from_grave(player, iVar1);
            generate_token(&token);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0223dbde;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  new_default_test_definition(&this_test, 2, "Select target creature card.");
  this_test.ai_selection_mode = 2;
LAB_0223dbde:
  iVar1 = generic_spell(player, card, event, 8, NULL, NULL, 0, &this_test);
  return iVar1;
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
  result = count_graveyard(player);
  if (result < 3)
  {
    if (player == 1)
    {
      return 0;
    }
    if (ai_is_speculating == 1)
    {
      return 0;
    }
    result = count_graveyard_by_type(player, 2);
    if (result == 0)
    {
    LAB_02243c33:
      if (ai_is_speculating == 1)
      {
        return 0;
      }
    }
    result = graveyard_has_shroud(player);
    if (result == 0)
      goto LAB_02243c51;
  }
  else
  {
    result = count_graveyard_by_type(player, 2);
    if (result == 0)
    {
      if (player == 1)
      {
        return 0;
      }
      goto LAB_02243c33;
    }
    result = graveyard_has_shroud(player);
    if (result == 0)
      goto LAB_02243c51;
    if (player == 1)
    {
      return 0;
    }
  }
  if (ai_is_speculating == 1)
  {
    return 0;
  }
LAB_02243c51:
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
  char *pcVar2;

  result = count_counters(player, card, COUNTER_P1_P1);
  pcVar2 = "must not have +1/+1 counters.";
  if (result == 0)
  {
    result = can_use_activated_abilities(player, card);
    pcVar2 = NULL;
    if (result == 0)
    {
      return "this can\'t use activated abilities.";
    }
  }
  return pcVar2;
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
  xtrigger_t xVar1;
  int result;

  enable_xtrigger_flags = enable_xtrigger_flags | ENABLE_XTRIGGER_1_1_COUNTERS;
  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if (xVar1 != XTRIGGER_1_1_COUNTERS)
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
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  unsigned int uVar5;
  char buffer[100];

  iVar1 = trigger_cause;
  iVar3 = trigger_cause_controller;
  iVar4 = spell_fizzled;
  if (trigger_condition != 0xd3)
  {
    return;
  }
  if (((card == affected_card) && (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    if (event == EVENT_TRIGGER)
    {
      iVar4 = is_what(trigger_cause_controller, trigger_cause, 1);
      if ((iVar4 == 0) && (1 - t_player == trigger_cause_controller))
      {

        instance = get_card_instance(1 - t_player, trigger_cause);
        uVar5 = 0;
        if (instance->number_of_targets != '\0')
        {
          iVar4 = 0;
          do
          {
            iVar3 = instance->targets[iVar4].player;
            if (((((t_player == iVar3) && (iVar1 = instance->targets[iVar4].card, iVar1 != -1)) &&
                  ((t_card == iVar1 || (t_card == -1)))) &&
                 (((player != iVar3 || card != iVar1 || (((unsigned int)flags >> 1 & 1) == 0)) &&
                   ((type == TYPE_NONE || (iVar3 = is_what(iVar3, iVar1, type), iVar3 != 0)))))) &&
                ((subtype == SUBTYPE_INVALID ||
                  (iVar3 = has_subtype(instance->targets[iVar4].player,
                                       instance->targets[iVar4].card, subtype),
                   iVar3 != 0))))
            {
              uVar5 = 2;
              break;
            }
            iVar4 = iVar4 + 1;
          } while (iVar4 < (int)(unsigned int)instance->number_of_targets);
        }
        event_result = event_result | uVar5;
      }
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {

      uVar5 = flags & 1;
      if (uVar5 == 0)
      {
        uVar5 = 1;
      }
      else if (-1 < hand_count[trigger_cause_controller])
      {

        discard(trigger_cause_controller, 6, player);
        uVar5 = (unsigned int)(spell_fizzled == 1);
      }
      spell_fizzled = iVar4;
      if ((life_to_pay != 0) && (iVar4 = can_pay_life(iVar3, life_to_pay), iVar4 != 0))
      {

        scnprintf(buffer, 100, "Pay %d life", life_to_pay);
        iVar4 = dialog_impl(player, card, EVENT_RESOLVE_TRIGGER, "\001\002", "\001\003", "\001\014",
                            "\001\010", iVar3, buffer, 1,
                            ((5 < life[iVar3] - life_to_pay) - 1 & 0xfffffff1) + 0x14, "Decline", 1,
                            10, 0, 0, 0, 0);
        if (iVar4 == 1)
        {
          uVar5 = 0;
          lose_life(iVar3, life_to_pay);
        }
      }
      if (((((((cless < 1) && (black < 1)) && (blue < 1)) && ((green < 1 && (red < 1)))) &&
            (white < 1)) ||
           ((iVar4 = has_mana_multi(iVar3, cless, black, blue, green, red, white),
             iVar4 == 0 || (iVar4 = charge_mana_multi(iVar3, cless, black, blue, green, red, white),
                            iVar4 == 0)))) &&
          (uVar5 != 0))
      {
        real_counter_a_spell(player, card, iVar3, iVar1);
      }
    }
  }
  return;
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
          goto LAB_0222c491;
        }
      }
    }
    gain_life(player, 1);
  }
LAB_0222c491:
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x222a8c0; original cards/modern_horizons_2.c:281. */
static int count_lands_in_play_and_in_hand(int player)
{
  int iVar1;
  card_instance_t *instance;
  int card;
  int iVar3;

  if (active_cards_count[player] < 1)
  {
    return 0;
  }
  card = 0;
  iVar3 = 0;
  do
  {
    while ((iVar1 = is_what(player, card, 1),
            iVar1 != 0 &&
                ((instance = in_hand(player, card),
                  instance != NULL || (instance = in_play(player, card), instance != NULL)))))
    {
      iVar3 = iVar3 + 1;
      card = card + 1;
      if (active_cards_count[player] <= card)
      {
        return iVar3;
      }
    }
    card = card + 1;
  } while (card < active_cards_count[player]);
  return iVar3;
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
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
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
      default_target_definition(player, card, &td, 0x47);
      td.zone = 0x1000;
      if ((card == affected_card) && (player == affected_card_controller))
      {
        if (current_turn == player)
        {
          td.preferred_controller = player;
          if (0x1d < current_phase)
          {
            iVar1 = count_graveyard(player);
            ai_modifier = ai_modifier + iVar1 * 5;
            goto LAB_0222e73b;
          }
        }
        else
        {
          iVar1 = 1 - player;
          td.preferred_controller = iVar1;
          if (current_phase < 0x19)
          {

            iVar3 = count_graveyard(player);
            ai_modifier = ai_modifier + iVar3 * 5;
            if (0 < active_cards_count[iVar1])
            {
              iVar3 = 0;
              do
              {
                instance = in_play(iVar1, iVar3);
                if ((instance != NULL) && (iVar4 = is_attacking(iVar1, iVar3), iVar4 != 0))
                {
                  ai_modifier = ai_modifier + 10;
                }
                iVar3 = iVar3 + 1;
              } while (iVar3 < active_cards_count[iVar1]);
            }
            goto LAB_0222e73b;
          }
        }
        ai_modifier = ai_modifier + -0x32;
      }
      goto LAB_0222e73b;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0x47);
      td.zone = 0x1000;
      if (event == EVENT_RESOLVE_SPELL)
      {
        iVar1 = valid_target(&td);
        if (iVar1 != 0)
        {

          instance = get_card_instance(player, card);
          reshuffle_grave_into_deck(instance->targets[0].player, 0);
          iVar1 = card_fog(player, card, EVENT_RESOLVE_SPELL);
          return iVar1;
        }
        kill_card(player, card, KILL_BURY);
      }
      goto LAB_0222e73b;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.zone = 0x1000;
LAB_0222e73b:
  iVar1 = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x222ea40; original cards/modern_horizons_2.c:959. */
int card_bone_shards(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
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
      result = generic_spell(player, card, EVENT_CAN_CAST, 0x81, &td,
                             "Select target creature or planeswalker.", 1, NULL);
      return result;
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
          uVar3 = new_sacrifice(player, card, player,
                                SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
          if (uVar3 == 0)
          {
            spell_fizzled = 1;
          }
          else
          {
            generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                          "Select target creature or planeswalker.", 1, NULL);
            if (-1 < (int)uVar3)
            {
              player_00 = (int)uVar3 >> 0x10 & 0xff;
              if (spell_fizzled == 1)
              {
                state_untargettable(player_00, uVar3 >> 0x18, 0);
              }
              else
              {
                kill_card(player_00, uVar3 >> 0x18, KILL_SACRIFICE);
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
  unsigned int uVar2;
  card_instance_t *instance;

  result = has_subtype(player, card, SUBTYPE_SNOW);
  uVar2 = 1;
  if (result == 0)
  {
    instance = get_card_instance(player, card);
    uVar2 = (unsigned int)((instance->card_color & 0x41) != 0);
  }
  return uVar2;
}

/* ManalinkEh 2022: 0x222b940; original cards/modern_horizons_2.c:1084. */
static char *target_is_snow_or_can_produce_colorless(int who_chooses, int player, int card)
{
  int result;
  char *pcVar2;

  result = is_snow_or_can_produce_colorless(-1, -1, player, card);
  pcVar2 = NULL;
  if (result == 0)
  {
    pcVar2 = "must be snow or should produce |C";
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x222f4f0; original cards/modern_horizons_2.c:1164. */
int card_breyas_apprentice(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int iVar3;
  int iVar4;
  unsigned int player_00;
  target_definition_t *ptStack_140;
  target_definition_t td;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_test;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x4995, &recovered_stack_test.token);
    recovered_stack_test.test.message[0x1c] = '\x01';
    recovered_stack_test.test.message[0x1d] = '\0';
    recovered_stack_test.test.message[0x1e] = '\0';
    recovered_stack_test.test.message[0x1f] = '\0';
    recovered_stack_test.test.message[0x18] = '\x01';
    recovered_stack_test.test.message[0x19] = '\0';
    recovered_stack_test.test.message[0x1a] = '\0';
    recovered_stack_test.test.message[0x1b] = '\0';
    recovered_stack_test.test.message[0x20] = ' ';
    recovered_stack_test.test.message[0x21] = '\0';
    recovered_stack_test.test.message[0x22] = '\0';
    recovered_stack_test.test.message[0x23] = '\0';
    generate_token(&recovered_stack_test.token);
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      new_default_test_definition(&recovered_stack_test.test, 0x40,
                                  "Select an artifact to sacrifice.");
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar3 = generic_activated_ability_with_sacrifice_as_cost(
            player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL,
            TYPE_ARTIFACT, &recovered_stack_test.test, SARV_REPORT_CMC);
        return iVar3;
      }
      if (event == EVENT_ACTIVATE)
      {

        instance->number_of_targets = '\0';
        instance->info_slot = 0;
        iVar3 = can_target(&td);
        iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\016",
                            "+2/+0 to target creature", iVar3,
                            -(unsigned int)(current_phase - 0x15U < 4) & 0x14, "Play from library",
                            1, 10, 0, 0, 0, 0);
        if (iVar3 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        uVar2 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                              &recovered_stack_test.test);
        if (uVar2 != 0)
        {
          if (iVar3 == 1)
          {
            new_pick_target(&td, "TARGET_CREATURE", 0, 1);
          }
          player_00 = (int)uVar2 >> 0x10 & 0xff;
          if ((spell_fizzled != 1) && (generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0,
                                                                 0, 0, 0, 0, 0, 0, NULL, NULL),
                                       spell_fizzled != 1))
          {
            if (iVar3 == 1)
            {
              iVar4 = is_attacking(instance->targets[0].player, instance->targets[0].card);
              if ((iVar4 != 0) ||
                  (iVar4 = is_blocking(instance->targets[0].player, instance->targets[0].card),
                   iVar4 != 0))
              {
                ai_modifier = ai_modifier + 0x28;
              }
            }
            else if (iVar3 == 2)
            {
              ai_modifier = ai_modifier + (hand_count[player] * 5 + 5) * -2 + 0x46;
            }
            kill_card(player_00, uVar2 >> 0x18, KILL_SACRIFICE);
            add_state(player, card, 0x10);
            tapped_for_mana_color = -1;
            instance->mode = (uint8_t)iVar3;
            return 0;
          }
          state_untargettable(player_00, uVar2 >> 0x18, 0);
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
      new_default_test_definition(&recovered_stack_test.test, 0x40,
                                  "Select an artifact to sacrifice.");
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      instance = get_card_instance(player, card);
    }
    ptStack_140 = &td;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (instance->mode == '\x01')
      {
        iVar3 = valid_target(ptStack_140);
        if (iVar3 != 0)
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
        result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x20, 0, 0, 0, 0, 0, 0,
                                           0, NULL, NULL);
        return result;
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
      goto LAB_0222fd66;
    }
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
  instance = get_card_instance(player, card);
LAB_0222fd66:
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
  card_instance_t *pcVar1;
  unsigned int uVar2;
  int result;
  card_instance_t *pcVar4;
  event_t event_00;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x59cd))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if (-1 < pcVar1->damage_target_player)
    {
      result = effect_follows_control_of_attachment(player, card, 0x4700);
      if (result != 0)
      {
        return 0;
      }
      goto LAB_0222afac;
    }
    event_00 = EVENT_STATIC_EFFECTS;
  LAB_0222b081:
    uVar2 = resolve_gfp_ability(player, card, event_00, RESOLVE_TRIGGER_MANDATORY);
    result = check_status(player, card, 0x10000);
    if (result != 0)
    {
      return 0;
    }
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, event), result != 0))
    {
      return 0;
    }
    if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {

      pcVar1 = get_card_instance(player, card);
      pcVar4 = in_play(affected_card_controller, affected_card);

      if ((pcVar4 != NULL) && (pcVar4 = get_card_instance(affected_card_controller, affected_card),
                               pcVar4->kill_code != '\0'))
      {

        if ((-1 < pcVar1->damage_target_player) &&
            ((pcVar1->damage_target_card == affected_card &&
              (pcVar1->damage_target_player == affected_card_controller))))
        {
          pcVar1->damage_target_card = -1;
          pcVar1->damage_target_player = -1;
          remove_status(player, card, 0x10000);
        }
        count_for_gfp_ability(player, card, 0x77, player, 0x100, NULL);
      }
      event_00 = EVENT_GRAVEYARD_FROM_PLAY;
      goto LAB_0222b081;
    }
  LAB_0222afac:
    uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    result = check_status(player, card, 0x10000);
    if (result != 0)
      goto LAB_0222b012;
  }

  uVar2 = uVar2 & 0xff;
  if (uVar2 != 0)
  {
    do
    {
      uVar2 = uVar2 - 1;
      carth_ability(player, card);
    } while (0 < (int)uVar2);
  }
  kill_card(player, card, KILL_EXILE);
LAB_0222b012:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x22303f0; original cards/modern_horizons_2.c:1538. */
int card_carth_the_lion(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  unsigned int uVar3;
  int iVar4;

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

      if (0 < active_cards_count[player])
      {
        iVar4 = 0;
        do
        {
          instance = in_play(player, iVar4);

          if ((((instance != NULL) && (iVar2 = is_what(player, iVar4, 0x80), iVar2 != 0)) &&
               (instance = get_card_instance(player, iVar4),
                instance->info_slot_fn == carth_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_02230471;
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[player]);
      }

      iVar4 = create_targetted_legacy_effect(player, card, carth_effect, player, card);
      instance = get_card_instance(player, iVar4);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar4, 0x10000);
    }
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    count_for_gfp_ability(player, card, 0x77, player, 0x100, NULL);
  }
LAB_02230471:
  instance = get_card_instance(player, card);
  if (instance->kill_code == '\0')
  {

    uVar3 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    uVar3 = uVar3 & 0xff;
    if (uVar3 != 0)
    {
      do
      {
        uVar3 = uVar3 - 1;
        carth_ability(player, card);
      } while (0 < (int)uVar3);
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
  unsigned char bVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  unsigned int uVar5;
  int iVar6;
  target_definition_t td;
  target_definition_t td2;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.required_abilities = 0x20;
    default_target_definition(player, card, &td2, 2);
    td2.preferred_controller = player;
    get_card_instance(player, card);
    iVar6 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    if ((iVar6 != 0) ||
        (iVar6 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL), iVar6 != 0))
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
        iVar6 = check_special_flags2(player, card, -0x80000000);
        if (iVar6 == 0)
        {

          instance->mode = '\0';
          iVar6 = can_target(&td);
          iVar3 = can_target(&td2);
          uVar5 = 0;
          if ((iVar3 != 0) && (iVar6 != 0))
          {
            iVar4 = has_mana_multi(player, 2, 0, 0, 1, 0, 0);
            uVar5 = (unsigned int)(iVar4 != 0);
          }
          iVar6 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033",
                              "5 damage to target creature with flying", iVar6, 10,
                              "Target creature gets +3/+3 until end of turn", iVar3, 5, "Entwine",
                              uVar5, 0xf, 0, 0, 0, 0);
          if (iVar6 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if ((iVar6 == 3) && (charge_mana_multi(player, 2, 0, 0, 1, 0, 0), spell_fizzled == 1))
          {
            return 0;
          }
        }
        bVar1 = instance->mode;
        if ((bVar1 & 1) == 0)
        {
          uVar5 = 0;
        }
        else
        {
          iVar6 = new_pick_target(&td, "Select target creature with flying.", 0, 0x81);
          uVar5 = (unsigned int)(iVar6 != 0);
          if (spell_fizzled == 1)
          {
            return 0;
          }
          bVar1 = instance->mode;
        }
        if ((bVar1 & 2) != 0)
        {
          new_pick_target(&td2, "Select target creature to get +3/+3", uVar5, 0x81);
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

      bVar1 = instance->mode;
      iVar6 = 0;
      if ((bVar1 & 1) != 0)
      {
        iVar6 = validate_target(player, card, &td, 0);
        if (iVar6 != 0)
        {
          damage_target0(player, card, 5);
        }
        bVar1 = instance->mode;
        iVar6 = 1;
      }
      if (((bVar1 & 2) != 0) && (iVar3 = validate_target(player, card, &td2, iVar6), iVar3 != 0))
      {
        pump_until_eot(player, card, instance->targets[iVar6].player, instance->targets[iVar6].card,
                       3, 3);
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
  unsigned int uVar3;
  unsigned int uVar4;

  if ((event == EVENT_MODIFY_COST_GLOBAL) && (affected_card_controller == player))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated(player, card, AL_STATIC);
      if (result == 0)
      {
        uVar3 = get_color(affected_card_controller, affected_card);
        uVar4 = get_sleighted_color_test(player, card, 0x18);
        if ((uVar3 & uVar4) != 0)
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
  int iVar1;
  int iVar2;
  int iVar3;
  token_generation_t token;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    iVar2 = 3;

    iVar3 = 0;
    do
    {
      iVar1 = flip_a_coin(player, card);
      iVar3 = iVar3 + iVar1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    if (iVar3 != 0)
    {
      default_token_definition(player, card, 0x493a, &token);
      token.qty = (unsigned char)iVar3;
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
  xtrigger_t xVar1;
  int result;

  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if (((event == EVENT_DECLARE_ATTACKERS) || (xVar1 == XTRIGGER_ATTACKING)) &&
      (result = delirium(player), result != 0))
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
  uint8_t uVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int uVar4;
  int iVar5;
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
    iVar5 = basic_spell(player, card, EVENT_CAN_CAST);
    return iVar5;
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

        iVar5 = new_special_count_grave(player, &this_test2);
        iVar3 = count_graveyard_by_type(player, 2);
        uVar4 = 0;
        if (iVar3 != 0)
        {
          iVar3 = graveyard_has_shroud(player);
          uVar4 = (unsigned int)(iVar3 == 0);
        }
        iVar5 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033", "\001\016",
                            "Reanimate one", uVar4, 10, "Reanimate two", iVar5, 0x14, 0, 0, 0, 0);
        if (iVar5 == 0)
        {
          spell_fizzled = 1;
          iVar5 = 0;
        }
        else if (iVar5 == 1)
        {
          iVar5 = generic_spell(player, card, EVENT_CAST_SPELL, 8, NULL, NULL, 0, &this_test);
        }
        else
        {
          if (iVar5 != 2)
          {
            return 0;
          }
          iVar5 = generic_spell(player, card, EVENT_CAST_SPELL, 0x28, NULL, NULL, 2, &this_test2);
        }
        return iVar5;
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
      uVar1 = instance->mode;
      if (uVar1 == '\x01')
      {

        iVar5 = validate_target_from_grave_source(player, card, player, 0);
        if (iVar5 != -1)
        {
          reanimate_permanent(player, card, player, iVar5, REANIMATE_PLUS1_PLUS1_COUNTER);
        }
        uVar1 = instance->mode;
      }
      if (uVar1 == '\x02')
      {

        for (iVar5 = 0; instance = get_card_instance(player, card), iVar5 < instance->info_slot;
             iVar5 = iVar5 + 1)
        {

          iVar3 = validate_target_from_grave(player, card, player, iVar5);
          if (iVar3 != -1)
          {
            reanimate_permanent(player, card, player, iVar3, REANIMATE_DEFAULT);
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
  int iVar2;
  int iVar3;
  int player_00;

  check_legend_rule(player, card, event);
  indestructible(player, card, event);
  instance = is_equipping(player, card);
  if (instance != NULL)
  {

    instance = get_card_instance(player, card);
    player_00 = (int)instance->damage_target_player;
    iVar2 = instance->damage_target_card;
    rfg_when_damage(player_00, iVar2, event);
    if (((event == EVENT_CHANGE_TYPE) && (iVar2 == affected_card)) &&
        (player_00 == affected_card_controller))
    {
      iVar3 = is_humiliated(player, card, AL_TRIG);
      if (iVar3 == 0)
      {
        instance = get_card_instance(player_00, iVar2);
        instance->destroys_if_blocked = instance->destroys_if_blocked | 0xc;
      }
    }
    iVar2 = vanilla_equipment(player, card, event, 7, 5, 5, 0x180, 0x804);
    return iVar2;
  }
  iVar2 = living_weapon(player, card, event, 7);
  return iVar2;
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
      result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
      return result;
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
                goto LAB_02239e8e;
              }
            }
            new_default_test_definition(&test, 0x47, "Select a Mountain to sacrifice.");
            test.subtype = 0x5006;
            new_sacrifice(player, card, player, SAC_AS_COST, &test);
            td.allow_cancel = 0;
          }
        LAB_02239e8e:
          result = generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                                 "Select target creature or planeswalker.", 1, NULL);
          return result;
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
  int iVar1;
  int iVar2;

  if (event - EVENT_CAN_PAY_MADNESS_COST < 2)
  {
    iVar1 = madness(player, card, event, 1, 2, 0, 0, 0, 0);
    return iVar1;
  }
  upkeep_trigger_ability(player, card, event, 2);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
    {
      iVar1 = count_graveyard_by_type(player, 2);
      iVar2 = count_graveyard_by_type(1 - player, 2);
      set_base_pt(iVar1 + iVar2);
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
  char cVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int card_00;
  target_definition_t td1;

  if (event == EVENT_MODIFY_COST)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      iVar3 = 1 - player;
      if (active_cards_count[iVar3] < 1)
      {
        cVar1 = '\0';
      }
      else
      {
        card_00 = 0;
        cVar1 = '\0';
        do
        {
          instance = in_play(iVar3, card_00);
          if ((instance != NULL) && (iVar4 = is_what(iVar3, card_00, 1), iVar4 != 0))
          {
            instance = get_card_instance(iVar3, card_00);
            cVar1 = (cVar1 + '\x01') - ((instance->card_color & 0x41) == 0);
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[iVar3]);
      }
      COST_COLORLESS = COST_COLORLESS - cVar1;
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
    iVar3 = can_target(&td1);
    if ((iVar3 != 0) && (iVar3 = new_pick_target(
                             &td1, "Select target nonbasic land an opponent controls.", 0, 0x80),
                         iVar3 != 0))
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
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  target_definition_t td;
  test_definition_t test;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    iVar2 = delirium(player);
    if ((iVar2 == 0) && (iVar2 = can_target(&td), iVar2 != 0))
    {
      iVar2 = hand_count[player];
      iVar3 = can_target(&td);
      uVar4 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\014", "\001\003", "\001\016",
                          "4 damage to any target", iVar3, 3, "Look at top four of library", 1,
                          7 - iVar2, 0, 0, 0, 0);
      if (((uVar4 & 1) != 0) && (iVar2 = new_pick_target(&td, "TARGET_ANY", 0, 1), iVar2 != 0))
      {
        damage_target0(player, card, 4);
        instance->number_of_targets = '\0';
      }
      if ((uVar4 & 2) == 0)
      {
        return 0;
      }
    }
    else
    {
      iVar2 = new_pick_target(&td, "TARGET_ANY", 0, 1);
      if (iVar2 != 0)
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
  card_instance_t *pcVar1;
  int iVar2;
  int iVar3;
  card_instance_t *pcVar4;
  unsigned int uVar5;
  int iStack_14;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    pcVar1 = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = delirium(player);
      if ((iVar2 != 0) && (iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 2,
                                                             0, 1, 0, 0, 0, 0, NULL, NULL),
                           iVar2 != 0))
      {
        return 1;
      }
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 1, 0, 0, 0, 0,
                                        NULL, NULL);
      return iVar2;
    }
    if (event != EVENT_ACTIVATE)
      goto LAB_0223c184;

    iVar2 = delirium(player);
    uVar5 = 0;
    if (iVar2 != 0)
    {
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 2, 0, 1, 0, 0, 0, 0,
                                        NULL, NULL);
      uVar5 = (unsigned int)(iVar2 != 0);
    }
    iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 1, 0, 0, 0, 0,
                                      NULL, NULL);
    iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\033",
                        "Draw & discard", iVar2, 5, "Draw a card", uVar5, 10, 0, 0, 0, 0);
    if (iVar2 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    iVar2 = 0;
    iStack_14 = 0;
    if (active_cards_count[player] < 1)
    {
    LAB_0223c4d0:
      ai_modifier = ai_modifier + 0x30;
    }
    else
    {
      do
      {
        pcVar4 = in_play(player, iVar2);
        if ((pcVar4 != NULL) || (pcVar4 = in_hand(player, iVar2), pcVar4 != NULL))
        {
          iVar3 = is_what(player, iVar2, 1);
          iStack_14 = (iStack_14 + 1) - (unsigned int)(iVar3 == 0);
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < active_cards_count[player]);
      if (4 < iStack_14 - 3U)
        goto LAB_0223c4d0;
    }
    if (pcVar1->mode == '\x01')
    {
      iVar2 = 0;
    }
    else
    {
      if (pcVar1->mode != '\x02')
        goto LAB_0223c18d;
      iVar2 = 2;
    }
    iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 1, iVar2, 0, 1, 0, 0, 0, 0,
                                      NULL, NULL);
  }
  else
  {
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      pcVar1 = get_card_instance(player, card);
    LAB_0223c184:
      if ((event == EVENT_RESOLVE_ACTIVATION) && (draw_a_card(player), pcVar1->mode == '\x01'))
      {
        discard(player, 0, player);
      }
    }
  LAB_0223c18d:
    iVar2 = 0;
  }
  return iVar2;
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
  unsigned int uVar2;
  unsigned int uVar3;

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

          uVar2 = get_color(affected_card_controller, affected_card);
          uVar3 = get_sleighted_color_test(player, card, 0x20);
          if ((uVar3 & uVar2) != 0)
          {
            vigilance(affected_card_controller, affected_card, EVENT_ABILITIES);
          }
          uVar3 = get_sleighted_color_test(player, card, 4);
          if ((uVar3 & uVar2) != 0)
          {
            hexproof(affected_card_controller, affected_card, EVENT_ABILITIES);
          }
          uVar3 = get_sleighted_color_test(player, card, 2);
          if ((uVar3 & uVar2) != 0)
          {
            lifelink(affected_card_controller, affected_card, EVENT_ABILITIES);
          }
          uVar3 = get_sleighted_color_test(player, card, 0x10);
          if ((uVar3 & uVar2) != 0)
          {
            event_result = event_result | 0x100;
          }
          uVar3 = get_sleighted_color_test(player, card, 8);
          if ((uVar3 & uVar2) != 0)
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
  int result;
  token_generation_t token;

  if (event - EVENT_POWER < 2)
  {
    if ((((affected_card_controller == player) &&
          (result = is_humiliated(player, card, AL_STATIC), result == 0)) &&
         (result = has_subtype(affected_card_controller, affected_card, SUBTYPE_CRAB),
          result != 0)) &&
        (result = delirium(player), result != 0))
    {
      event_result = event_result + 1;
    }
  LAB_0223dea8:
    result = global_enchantment(player, card, event);
    return result;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_0223dde5:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_0223ddea;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
      goto LAB_0223dea8;
    goto LAB_0223dde5;
  }
  default_token_definition(player, card, 0x59d6, &token);
  token.pow = 0;
  token.tou = 3;
  token.color_forced = '\x04';
  generate_token(&token);
LAB_0223ddea:
  result = generic_activated_ability(player, card, event, 0x20, 1, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
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
  int iVar1;
  int iVar2;

  check_legend_rule(player, card, event);
  if (event == EVENT_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar1 = count_subtype(player, 0x47, 0x74);
      iVar2 = has_subtype(player, card, SUBTYPE_MERFOLK);
      if ((int)((iVar2 != 0) + 2) <= iVar1)
      {
        iVar1 = is_humiliated(player, card, AL_STATIC);
        if (iVar1 == 0)
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
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  int player_00;
  int32_t targ_player;
  unsigned int uVar4;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_test;

  if (event - EVENT_POWER < 2)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar2 = count_domain(player, card);
      set_base_pt(iVar2);
    }
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    return 0;
  }
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }

  iVar2 = hand_count[player];
  iVar1 = count_graveyard(player);
  player_00 = 1 - player;
  if (iVar1 != 0)
  {
    iVar1 = graveyard_has_shroud(player);
    uVar4 = 1;
    if (iVar1 == 0)
      goto LAB_02240a95;
  }
  iVar1 = count_graveyard(player_00);
  uVar4 = 0;
  if (iVar1 != 0)
  {
    iVar1 = graveyard_has_shroud(player_00);
    uVar4 = (unsigned int)(iVar1 == 0);
  }
LAB_02240a95:
  iVar1 = count_graveyard(player_00);
  iVar2 =
      dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\016", "\001\003",
                  "Discard & draw", (unsigned int)(0 < iVar2), 10, "Exile a card in a graveyard",
                  uVar4, ((0x13 < iVar1) - 1 & 0xfffffff6) + 0xf, 0, 0, 0, 0);
  if (iVar2 == 1)
  {
    if (0 < hand_count[player])
    {
      discard(player, 0, player);
      draw_cards(player, 1);
    }
  }
  else if (iVar2 == 2)
  {

    iVar2 = count_graveyard(0);
    uVar4 = 0;
    if (iVar2 != 0)
    {
      iVar2 = graveyard_has_shroud(0);
      uVar4 = (unsigned int)(iVar2 == 0);
    }
    iVar2 = count_graveyard(1);
    if (((iVar2 == 0) || (iVar2 = graveyard_has_shroud(1), iVar2 != 0)) ||
        (uVar4 = uVar4 | 2, uVar4 != 3))
    {
      targ_player = (int)uVar4 >> 1;
    }
    else
    {

      default_target_definition(player, card, &recovered_stack_test.target, 2);
      recovered_stack_test.test.message[0xc] = '\0';
      recovered_stack_test.test.message[0xd] = '\x10';
      recovered_stack_test.test.message[0xe] = '\0';
      recovered_stack_test.test.message[0xf] = '\0';
      instance = get_card_instance(player, card);
      iVar2 = new_pick_target(&recovered_stack_test.target,
                              "Select the owner of the graveyard you want to look.", 0, 0x80);
      if (iVar2 == 0)
      {
        return 0;
      }
      targ_player = instance->targets[0].player;
      instance->number_of_targets = '\0';
    }
    new_default_test_definition(&recovered_stack_test.test, 0xff, "Select a card to exile.");
    iVar2 = new_select_a_card(player, targ_player, 2, 0, 1, -1, &recovered_stack_test.test);
    if (iVar2 != -1)
    {
      rfg_card_from_grave(targ_player, iVar2);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2240d80; original cards/modern_horizons_2.c:6709. */
int card_the_underworld_cookbook(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  unsigned int uVar5;
  test_definition_t test;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_test_definition(&test, 2);
    if (event == EVENT_CAN_ACTIVATE)
    {
      uVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x21, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      uVar5 = generic_activated_ability_with_target_in_grave(
          player, card, EVENT_CAN_ACTIVATE, GAA_SACRIFICE_ME | GAA_UNTAPPED, 4, 0, 0, 0, 0, 0, 0,
          NULL, NULL, TYPE_CREATURE, &test, 8);
      return uVar5 | uVar4;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->mode = '\0';
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x21, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar3 = generic_activated_ability_with_target_in_grave(
          player, card, EVENT_CAN_ACTIVATE, GAA_SACRIFICE_ME | GAA_UNTAPPED, 4, 0, 0, 0, 0, 0, 0,
          NULL, NULL, TYPE_CREATURE, &test, 8);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\033", "\001\016",
                          "Create a Food token", iVar2, 2, "Creature from grave to hand", iVar3, 1,
                          0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 != 1)
      {
        if (iVar2 != 2)
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
        (iVar2 = validate_target_from_grave_source(player, card, player, 0), iVar2 != -1))
    {

      add_card_to_hand(player, graveyard_ptr_mutable[player][iVar2]);
      remove_card_from_grave(player, iVar2);
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
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t *ptStack_68;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 0, 0, 0, 0, 1, 0, 0,
                                        &td, NULL);
      if (iVar2 != 0)
      {
        return 1;
      }
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 0, 1, 0,
                                        NULL, NULL);
      return iVar2;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->mode = '\0';
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 0, 0, 0, 0, 1, 0, 0,
                                        &td, NULL);
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 0, 1, 0,
                                        NULL, NULL);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\033",
                          "Destroy your artifact", iVar2, 10, "Zabaz gains flying", iVar3,
                          ((current_phase < 0x18) - 1 & 0xfffffff1) + 0x14, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 1)
      {
        generic_activated_ability(player, card, EVENT_ACTIVATE, 6, 0, 0, 0, 0, 1, 0, 0, &td,
                                  "Select target artifact you control.");
        if (spell_fizzled != 1)
        {
          iVar2 =
              check_special_flags(instance->targets[0].player, instance->targets[0].card, 0x8000);
          ai_modifier = ai_modifier + (-(unsigned int)(iVar2 == 0) & 0xffffffd0) + 0x18;
        }
      }
      else if (iVar2 == 2)
      {
        generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 0, 0, 0, 0, 0, 1, 0, NULL, NULL);
      }
      goto LAB_02243fa5;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02243fa5;
    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
  }
  ptStack_68 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->mode == '\x01')
    {
      iVar2 = valid_target(ptStack_68);
      if (iVar2 != 0)
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
LAB_02243fa5:
  iVar2 = modular(player, card, event, 1);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int magus_of_the_bridge_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2238d90; original cards/modern_horizons_2.c:4098. */
int card_lightning_spear(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t *ptStack_68;
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

      iVar2 = basic_equipment(player, card, EVENT_CAN_ACTIVATE, 1);
      if (iVar2 == 0)
      {
        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xc, 2, 0, 0, 0, 1, 0,
                                          0, &td, NULL);
        return iVar2;
      }
      return iVar2;
    }
    if (event == EVENT_ACTIVATE)
    {

      iVar2 = can_activate_basic_equipment(player, card, EVENT_ACTIVATE, 1);
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xc, 2, 0, 0, 0, 1, 0, 0,
                                        &td, NULL);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\004", "\001\016",
                          "\001\033", "Equip", iVar2, 5, "Sac & damage", iVar3, 10, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 != 1)
      {
        if (iVar2 != 2)
        {
          return 0;
        }
        generic_activated_ability(player, card, EVENT_ACTIVATE, 0xc, 2, 0, 0, 0, 1, 0, 0, &td,
                                  "TARGET_ANY");
        return 0;
      }
      iVar2 = basic_equipment(player, card, EVENT_ACTIVATE, 1);
      return iVar2;
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
  ptStack_68 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (instance->mode == '\x01')
    {
      iVar2 = basic_equipment(player, card, EVENT_RESOLVE_ACTIVATION, 1);
      return iVar2;
    }
    if (instance->mode == '\x02')
    {
      td.illegal_abilities = instance->targets[1].player;
      iVar2 = valid_target(ptStack_68);
      if (iVar2 != 0)
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
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_this_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x5a2e))
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
  LAB_0222abf8:
    result = check_status(player, card, 0x10000);
    if (result != 0)
      goto LAB_0222ac14;
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
      goto LAB_0222abf8;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);

    if ((pcVar3 != NULL) && (pcVar3 = get_card_instance(affected_card_controller, affected_card),
                             pcVar3->kill_code != '\0'))
    {
      default_test_definition(&recovered_stack_this_test.test, 2);
      recovered_stack_this_test.test.type_flag = 3;

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
        *(uint8_t *)((int)&pcVar1->targets[0xb].player + 2) = 1;
      }
      count_for_gfp_ability(player, card, 0x77, player, 0, &recovered_stack_this_test.test);
    }
    result = check_status(player, card, 0x10000);
    if (result != 0)
    {
      return 0;
    }
  }

  result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {

    default_token_definition(player, card, 0x49ac, &recovered_stack_this_test.token);
    recovered_stack_this_test.test.message[0x1c] = '\x02';
    recovered_stack_this_test.test.message[0x1d] = '\0';
    recovered_stack_this_test.test.message[0x1e] = '\0';
    recovered_stack_this_test.test.message[0x1f] = '\0';
    recovered_stack_this_test.test.message[0x18] = '\x02';
    recovered_stack_this_test.test.message[0x19] = '\0';
    recovered_stack_this_test.test.message[0x1a] = '\0';
    recovered_stack_this_test.test.message[0x1b] = '\0';
    recovered_stack_this_test.test.message[0x28] = '\x02';
    recovered_stack_this_test.test.message[0x2b] = (char)result;
    generate_token(&recovered_stack_this_test.token);
    kill_card(player, card, KILL_EXILE);
  }
LAB_0222ac14:
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
  int iVar2;
  int iVar3;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_this_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar3 = 0;
        do
        {
          instance = in_play(player, iVar3);

          if ((((instance != NULL) && (iVar2 = is_what(player, iVar3, 0x80), iVar2 != 0)) &&
               (instance = get_card_instance(player, iVar3),
                instance->info_slot_fn == magus_of_the_bridge_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_022399a5;
          iVar3 = iVar3 + 1;
        } while (iVar3 < active_cards_count[player]);
      }

      iVar3 =
          create_targetted_legacy_effect(player, card, magus_of_the_bridge_effect, player, card);
      instance = get_card_instance(player, iVar3);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar3, 0x10000);
    }
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    default_test_definition(&recovered_stack_this_test.test, 2);
    recovered_stack_this_test.test.type_flag = 3;
    count_for_gfp_ability_and_store_values(player, card, 0x77, 2, 0,
                                           &recovered_stack_this_test.test, 0x200, 0);
  }
LAB_022399a5:
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (iVar3 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY), iVar3 != 0))
  {

    default_token_definition(player, card, 0x49ac, &recovered_stack_this_test.token);
    recovered_stack_this_test.test.message[0x2b] = (char)iVar3;
    recovered_stack_this_test.test.message[0x1c] = '\x02';
    recovered_stack_this_test.test.message[0x1d] = '\0';
    recovered_stack_this_test.test.message[0x1e] = '\0';
    recovered_stack_this_test.test.message[0x1f] = '\0';
    recovered_stack_this_test.test.message[0x18] = '\x02';
    recovered_stack_this_test.test.message[0x19] = '\0';
    recovered_stack_this_test.test.message[0x1a] = '\0';
    recovered_stack_this_test.test.message[0x1b] = '\0';
    recovered_stack_this_test.test.message[0x28] = '\x02';
    generate_token(&recovered_stack_this_test.token);
    if ((char)((unsigned int)iVar3 >> 8) != '\0')
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
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

  if ((event == EVENT_ETB) && (0 < active_cards_count[player]))
  {
    iVar1 = 0;
    do
    {
      if (card != iVar1)
      {
        instance = in_play(player, iVar1);
        if (instance != NULL)
        {
          iVar3 = is_what(player, iVar1, 0x40);
          if (iVar3 != 0)
          {
            iVar3 = is_what(player, iVar1, 2);
            if (iVar3 != 0)
            {
              add_counters(player, card, player, iVar1, COUNTER_P1_P1, 1);
            }
          }
        }
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < active_cards_count[player]);
  }
  iVar1 = modular(player, card, event, 2);
  return iVar1;
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
  uint8_t uVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  uint32_t uVar5;
  unsigned int uVar6;
  int iVar7;
  card_instance_t *pcStack_68;
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
    pcStack_68 = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {

      iVar3 = can_sorcery_be_played(player, EVENT_CAN_ACTIVATE);
      if (iVar3 == 0)
      {
        return 0;
      }

      iVar3 = get_cost_mod_for_activated_abilities(player, card, 0, 0, 0, 1, 0, 0);
      iVar3 = has_mana_hybrid(player, 1, 3, 5, iVar3);
      uVar6 = 0;
      if (iVar3 != 0)
      {
        uVar6 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x801, 0, 0, 0, 0, 0, 0,
                                          0, NULL, NULL);
      }
      if (0 < active_cards_count[player])
      {
        iVar3 = 0;
        do
        {
          pcVar2 = in_play(player, iVar3);
          if ((((pcVar2 != NULL) && (iVar7 = is_what(player, iVar3, 2), iVar7 != 0)) &&
               (iVar7 = check_state(player, iVar3, 0x20010), iVar7 == 0)) &&
              (iVar7 = is_humiliated(player, iVar3, AL_NM_ACT), iVar7 == 0))
          {
            iVar7 = get_cost_mod_for_activated_abilities(player, iVar3, 0, 0, 0, 1, 0, 0);
            iVar7 = has_mana_hybrid(player, 1, 3, 5, iVar7);
            if (iVar7 != 0)
            {
              return uVar6 | 1;
            }
          }
          iVar3 = iVar3 + 1;
        } while (iVar3 < active_cards_count[player]);
        return uVar6;
      }
      return uVar6;
    }
    if (event == EVENT_ACTIVATE)
    {

      iVar7 = 0;
      pcStack_68->number_of_targets = '\0';
      pcStack_68->mode = '\0';
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x801, 0, 0, 0, 0, 0, 0,
                                        0, NULL, NULL);
      if (0 < active_cards_count[player])
      {
        do
        {
          pcVar2 = in_play(player, iVar7);
          if (((pcVar2 != NULL) && (iVar4 = is_what(player, iVar7, 2), iVar4 != 0)) &&
              ((iVar4 = check_state(player, iVar7, 0x20010),
                iVar4 == 0 &&
                    (iVar4 = is_humiliated(player, iVar7, AL_NM_ACT), iVar4 == 0))))
          {

            iVar4 = get_cost_mod_for_activated_abilities(player, iVar7, 0, 0, 0, 1, 0, 0);
            iVar4 = has_mana_hybrid(player, 1, 3, 5, iVar4);
            if (iVar4 != 0)
            {
              uVar5 = 1;
              goto LAB_0222d552;
            }
          }
          iVar7 = iVar7 + 1;
        } while (iVar7 < active_cards_count[player]);
      }
      uVar5 = 0;
    LAB_0222d552:
      iVar3 =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\016", "Outlast",
                      iVar3, 10, "Another creature outlasts", uVar5, 0x14, 0, 0, 0, 0);
      if (iVar3 == 1)
      {

        iVar7 = get_cost_mod_for_activated_abilities(player, card, 0, 0, 0, 1, 0, 0);
        iVar7 = charge_mana_hybrid(player, card, 1, 3, 5, iVar7);
        if (iVar7 != 0)
        {
          generic_activated_ability(player, card, EVENT_ACTIVATE, 0x801, 0, 0, 0, 0, 0, 0, 0, NULL,
                                    NULL);
        }
      }
      else if ((iVar3 == 2) &&
               (iVar7 = new_pick_target(
                    &td,
                    "Select anothe untapped, nonsick creature you control without +1/+1 counters.",
                    0, 0x80),
                iVar7 != 0))
      {

        iVar7 = get_cost_mod_for_activated_abilities(pcStack_68->targets[0].player,
                                                     pcStack_68->targets[0].card, 0, 0, 0, 1, 0, 0);
        iVar7 = charge_mana_hybrid(player, card, 1, 3, 5, iVar7);
        if (iVar7 != 0)
        {
          generic_activated_ability(pcStack_68->targets[0].player, pcStack_68->targets[0].card,
                                    EVENT_ACTIVATE, 0x801, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
          pcStack_68->number_of_targets = '\0';
        }
      }
      if (spell_fizzled == 1)
      {
        return 0;
      }
      pcStack_68->mode = (uint8_t)iVar3;
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
    pcStack_68 = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    uVar1 = pcStack_68->mode;
    if (uVar1 == '\x01')
    {
      pcVar2 = in_play(pcStack_68->parent_controller, pcStack_68->parent_card);
      if (pcVar2 != NULL)
      {
        add_counters(player, card, pcStack_68->parent_controller,
                                 pcStack_68->parent_card, COUNTER_P1_P1, 1);
      }
      uVar1 = pcStack_68->mode;
    }
    if ((uVar1 == '\x02') &&
        (pcVar2 = in_play(pcStack_68->targets[0].player, pcStack_68->targets[0].card),
         pcVar2 != NULL))
    {
      add_counters(player, card, pcStack_68->targets[0].player,
                               pcStack_68->targets[0].card, COUNTER_P1_P1, 1);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x222f3f0; original cards/modern_horizons_2.c:1146. */
int card_breathless_knight(int player, int card, event_t event)
{
  int result;

  lifelink(player, card, event);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = check_special_flags(trigger_cause_controller, trigger_cause, 0x10000000);
    if (result == 0)
    {
      result = check_special_flags3(trigger_cause_controller, trigger_cause, 1);
      if (result == 0)
        goto LAB_0222f41e;
    }
    etb_ability_global(player, event, 2, TYPE_CREATURE, NULL, player, card);
  }
LAB_0222f41e:
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
  int result;
  unsigned int uVar3;
  test_definition_t this_test;

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {

    instance = in_play(affected_card_controller, affected_card);
    if ((((instance != NULL) && (affected_card_controller == player)) &&
         (result = is_what(player, affected_card, 0x42), result != 0)) &&
        ((instance->kill_code == '\x03' &&
          (result = check_special_flags(affected_card_controller, affected_card, 0x800),
           result == 0))))
    {

      instance = get_card_instance(player, card);
      if (instance->targets[0xb].player < 0)
      {
        instance->targets[0xb].player = 0;
      }
      *(char *)&instance->targets[0xb].player = (char)instance->targets[0xb].player + '\x01';
    }
    instance = get_card_instance(player, card);
    if (instance->kill_code != '\0')
    {
      return 0;
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if (instance->kill_code != '\0')
      goto LAB_0223bf54;
  }

  uVar3 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (uVar3 != 0)
  {

    add_counters(player, card, player, card, COUNTER_P1_P1, uVar3 & 0xff);
  }
LAB_0223bf54:
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
  result = generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_NONE, 1, 1, 0,
                                                            1, 0, 0, 0, NULL, NULL, TYPE_NONE,
                                                            &this_test, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x2240320; original cards/modern_horizons_2.c:6497. */
int card_sword_of_hearth_and_home(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  int result;
  card_instance_t *instance;
  int iid;
  int card_00;
  int iStack_140;
  target_definition_t td;
  test_definition_t this_test;

  iStack_140 = equipped_creature_deals_damage(player, card, event,
                                              DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (iStack_140 != 0)
  {

    default_target_definition(player, card, &td, 2);
    td.special = 0x8000;
    td.extra = (int32_t)target_you_own;
    td.allow_cancel = 2;
    new_default_test_definition(&this_test, 1, "Select a basic land card.");
    this_test.subtype = 0x2000;
    instance = get_card_instance(player, card);
    if (0 < iStack_140)
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
        iStack_140 = iStack_140 + -1;
      } while (iStack_140 != 0);
    }
  }
  uVar1 = get_sleighted_protection(player, card, 0x8000);
  uVar2 = get_sleighted_protection(player, card, 0x2000);
  result = vanilla_equipment(player, card, event, 2, 2, 2, uVar1 | uVar2, 0);
  return result;
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
  unsigned char bVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    get_card_instance(player, card);
    iVar4 = basic_spell(player, card, EVENT_CAN_CAST);
    return iVar4;
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
        iVar4 = check_special_flags2(player, card, -0x80000000);
        if (iVar4 == 0)
        {

          instance->mode = '\0';
          iVar4 = has_mana_multi(player, 3, 0, 0, 0, 0, 1);
          iVar4 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033",
                              "Put a +1/+1 counter on each of up to two target creatures", 1, 0x14,
                              "Proliferate", 1, 10, "Entwine", iVar4, 0x1e, 0, 0, 0, 0);
          if (iVar4 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if ((iVar4 == 3) && (charge_mana_multi(player, 3, 0, 0, 0, 0, 1), spell_fizzled == 1))
          {
            return 0;
          }
        }
        if (((instance->mode & 1) != 0) &&
            (iVar4 = new_pick_target(&td, "TARGET_CREATURE", 0, 0), iVar4 != 0))
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
      bVar1 = instance->mode;

      if (((bVar1 & 1) != 0) && (instance->number_of_targets != '\0'))
      {
        iVar4 = 0;
        do
        {
          while (iVar3 = validate_target(player, card, &td, iVar4), iVar3 == 0)
          {
            iVar4 = iVar4 + 1;
            if ((int)(unsigned int)instance->number_of_targets <= iVar4)
              goto LAB_02241fec;
          }
          add_counters(player, card, instance->targets[iVar4].player,
                                   instance->targets[iVar4].card, COUNTER_P1_P1, 1);
          iVar4 = iVar4 + 1;
        } while (iVar4 < (int)(unsigned int)instance->number_of_targets);
      LAB_02241fec:
        bVar1 = instance->mode;
      }
      if ((bVar1 & 2) != 0)
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
  int32_t iVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  unsigned int uVar5;
  int card_00;
  test_definition_t this_test;
  int fake_hand[100][2];

  if (event == EVENT_MODIFY_COST)
  {

    pcVar2 = get_card_instance(player, card);
    if (0 < active_cards_count[player])
    {
      iVar3 = 0;
      do
      {
        pcVar4 = in_hand(player, iVar3);
        if (((card != iVar3) && (pcVar4 != NULL)) &&
            (uVar5 = get_color(player, iVar3), (color_test & uVar5) != 0))
        {
          null_casting_cost(player, card);
          pcVar2->eot_toughness0 = '\x01';
          return 0;
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 < active_cards_count[player]);
    }
    pcVar2->eot_toughness0 = '\0';
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

  pcVar2 = get_card_instance(player, card);
  iVar3 = played_for_free(player, card);
  if (iVar3 != 0)
  {
    return 0;
  }
  iVar3 = is_token(player, card);
  if (iVar3 != 0)
  {
    return 0;
  }
  if (pcVar2->eot_toughness0 == '\x01')
  {

    iVar3 = get_id(player, card);
    iVar3 = has_mana_to_cast_id(player, EVENT_CAST_SPELL, iVar3);
    if (iVar3 != 0)
    {
      iVar3 = do_dialog(player, player, card, -1, -1, " Play normally\n Evoke\n Cancel", 0);
      if (iVar3 == 0)
      {
        iVar3 = get_id(player, card);
        charge_mana_from_id(player, card, EVENT_CAST_SPELL, iVar3);
        return 0;
      }
      if (iVar3 != 1)
      {
        spell_fizzled = 1;
        return 0;
      }
    }

    card_00 = 0;
    iVar3 = 0;
    if (0 < active_cards_count[player])
    {
      do
      {
        pcVar4 = in_hand(player, card_00);
        if (((card != card_00) && (pcVar4 != NULL)) &&
            (uVar5 = get_color(player, card_00), (color_test & uVar5) != 0))
        {
          pcVar4 = get_card_instance(player, card_00);
          iVar1 = pcVar4->internal_card_id;
          fake_hand[1][iVar3] = card_00;
          fake_hand[0][iVar3] = iVar1;
          iVar3 = iVar3 + 1;
        }
        card_00 = card_00 + 1;
      } while (card_00 < active_cards_count[player]);
    }
    new_default_test_definition(&this_test, 0xff, "Select a card to exile.");
    iVar3 = select_card_from_zone(player, player, fake_hand[0], iVar3, 0, 3, -1, &this_test);
    if (iVar3 < 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    rfg_card_in_hand(player, fake_hand[1][iVar3]);
    pcVar2->eot_toughness1 = '\x01';
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2233bd0; original cards/modern_horizons_2.c:2513. */
int card_endurance(int player, int card, event_t event)
{
  int argument2;
  int iVar1;
  card_instance_t *instance;
  int position;
  int iVar3;
  int iStack_6c;
  target_definition_t td;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  evoke_with_cards_in_hand(player, card, event, 8);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    instance = get_card_instance(player, card);
    iVar1 = can_target(&td);
    if ((iVar1 != 0) && (iVar1 = pick_target(&td, "TARGET_PLAYER"), iVar1 != 0))
    {

      iVar1 = instance->targets[0].player;
      iStack_6c = count_graveyard(iVar1);
      position = iStack_6c + -1;
      if (position < 0)
      {
        iStack_6c = 0;
      }
      else
      {
        do
        {

          argument2 = graveyard_ptr_mutable[iVar1][position];
          iVar3 = position + -1;
          remove_card_from_grave(iVar1, position);
          raw_put_iid_on_top_of_deck(iVar1, argument2);
          position = iVar3;
        } while (iVar3 != -1);
      }
      put_top_x_on_bottom_in_random_order(iVar1, iStack_6c);
    }
    instance->number_of_targets = '\0';
    iVar1 = evoked(player, card);
    if (iVar1 != 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  iVar1 = flash(player, card, event);
  return iVar1;
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
  unsigned int uVar2;

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
  uVar2 = resolve_gfp_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (uVar2 != 0)
  {

    uVar2 = uVar2 & 0xff;
    while (true)
    {

      if (((uVar2 == 0) || (result = has_mana(player, 7, 1), result == 0)) ||
          (result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\003",
                                "Return Squirrel Sanctuary to its owner's hand", 1, 10, "Decline",
                                1, 5, 0, 0, 0, 0),
           result != 1))
        goto LAB_0223f3d8;
      result =
          charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player, COLOR_ANY, 1);
      if (result != 0)
        break;
      uVar2 = uVar2 - 1;
    }
    bounce_permanent(player, card);
  }
LAB_0223f3d8:
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2243df0; original cards/modern_horizons_2.c:7678. */
int card_yusri_fortunes_flame(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  int iStack_14;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    if (life[player] < 0x10)
    {
      iStack_14 = 4 - (0xfU - life[player] >> 1);
      if (iStack_14 < 1)
      {
        iStack_14 = 1;
      }
    }
    else
    {
      iStack_14 = 5;
    }
    do
    {
      result = choose_a_number(player, "How many coins you\'ll flip?", iStack_14);
      uVar2 = result - 1;
    } while (4 < uVar2);
    iStack_14 = 0;
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
        iStack_14 = iStack_14 + 1;
      }
      if (uVar2 == 0)
        break;
      uVar2 = uVar2 - 1;
    }
    if (iStack_14 == 5)
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
  int iVar1;
  int iVar2;
  int player_00;
  test_definition_t test;

  if ((trigger_condition == 0xd3) && (reason_for_trigger_controller == player))
  {
    player_00 = 1 - player;
    iVar1 = get_specific_storm_count(player_00);
    iVar2 = get_stormcreature_count(player_00);
    if (iVar1 == iVar2)
    {
      default_test_definition(&test, 2);
      test.type_flag = 1;
      sp_ability_trigger(player_00, event, 2, TYPE_NONE, &test, player, card);
    }
  }
  if (event == EVENT_SPA)
  {

    iVar1 = get_power(player, card);
    iVar2 = has_mana(1 - player, 0, iVar1);
    if ((iVar2 != 0) && (iVar1 = charge_mana(1 - player, COLOR_COLORLESS, iVar1), iVar1 != 0))
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
  card_instance_t *instance;
  int iVar2;
  unsigned int uVar3;
  int iVar4;
  int priority_first_half;
  int iVar5;
  union
  {
    int totals[2];
    test_definition_t this_test;
  } storage;

  instance = get_card_instance(player, card);
  iVar4 = current_turn;
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
      storage.totals[0] = 0;
      storage.totals[1] = 0;

      priority_first_half = hand_count[player] * -10 + 0x46;
      iVar5 = active_cards_count[current_turn] + -1;
      if (-1 < iVar5)
      {
        do
        {
          instance = in_play(iVar4, iVar5);
          if ((((instance != NULL) && (iVar2 = is_what(iVar4, iVar5, 2), iVar2 != 0)) &&
               (iVar2 = get_toughness(iVar4, iVar5), iVar2 < 4)) &&
              ((iVar2 = check_for_ability(iVar4, iVar5, 0x4000),
                iVar2 == 0 && (iVar2 = check_for_ability(iVar4, iVar5, 0x20), iVar2 == 0))))
          {
            storage.totals[iVar4] = storage.totals[iVar4] + 1;
          }
          iVar5 = iVar5 + -1;
        } while (iVar5 != -1);
      }
      iVar4 = 1 - current_turn;
      iVar5 = active_cards_count[iVar4] + -1;
      if (-1 < iVar5)
      {
        do
        {
          instance = in_play(iVar4, iVar5);
          if (((instance != NULL) && (iVar2 = is_what(iVar4, iVar5, 2), iVar2 != 0)) &&
              ((iVar2 = get_toughness(iVar4, iVar5),
                iVar2 < 4 &&
                    ((iVar2 = check_for_ability(iVar4, iVar5, 0x4000),
                      iVar2 == 0 && (iVar2 = check_for_ability(iVar4, iVar5, 0x20), iVar2 == 0))))))
          {
            storage.totals[iVar4] = storage.totals[iVar4] + 1;
          }
          iVar5 = iVar5 + -1;
        } while (iVar5 != -1);
      }
      iVar4 = storage.totals[player] * -10 + 0x14 + storage.totals[1 - player] * 10;
      goto LAB_0223431a;
    }
    if (event != EVENT_CAN_CHANGE_TARGET)
    {
      if (event == EVENT_RESOLVE_SPELL)
      {
        uVar3 = instance->info_slot;
        if ((uVar3 & 1) != 0)
        {
          discard(player, 0, 0);
          draw_cards(player, 2);
          uVar3 = instance->info_slot;
        }
        if ((uVar3 & 2) != 0)
        {
          default_test_definition(&storage.this_test, 2);
          new_damage_all(player, card, 2, 3, 0, &storage.this_test);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      else if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  iVar4 = 0x14;
  priority_first_half = 10;
LAB_0223431a:
  iVar4 = generic_split_card(player, card, event, 0x59f0, 1, priority_first_half, 0x59ff, 1,
                                  iVar4);
  return iVar4;
}

/* ManalinkEh 2022: 0x223d1d0; original cards/modern_horizons_2.c:5515. */
int card_said_done(int player, int card, event_t event)
{
  int can_play_second_half;
  int result;
  card_instance_t *pcVar2;
  unsigned int uVar3;
  card_instance_t *pcStack_140;
  target_definition_t td;
  test_definition_t this_test;

  if (event == EVENT_MODIFY_COST)
  {

    default_test_definition(&this_test, 0x38);
    result = new_special_count_grave(player, &this_test);
    if ((result == 0) || (result = graveyard_has_shroud(player), result != 0))
    {
      uVar3 = 0;
    }
    else
    {
      result = can_sorcery_be_played(player, EVENT_MODIFY_COST);
      uVar3 = (unsigned int)(result != 0);
    }
    default_target_definition(player, card, &td, 2);
    result = can_target(&td);
    generic_split_card(player, card, EVENT_MODIFY_COST, 0x5a5d, uVar3, 10, 0x59e2, result, 8);
    return 0;
  }
  if (event == EVENT_PLAY_FIRST_HALF)
  {
    pcVar2 = get_card_instance(player, card);
    pcVar2->number_of_targets = '\0';
    result = card_call_to_mind(player, card, EVENT_CAST_SPELL);
    return result;
  }
  if (event == EVENT_PLAY_SECOND_HALF)
  {
    pcVar2 = get_card_instance(player, card);
    pcVar2->number_of_targets = '\0';
    result = card_frost_breath(player, card, EVENT_CAST_SPELL);
    return result;
  }
  if (event == EVENT_CAN_CAST)
  {
  LAB_0223d250:
    pcStack_140 = get_card_instance(player, card);
    default_test_definition(&this_test, 0x38);
    result = new_special_count_grave(player, &this_test);
    if (result == 0)
    {
      default_target_definition(player, card, &td, 2);
      uVar3 = 0;
    }
    else
    {
    LAB_0223d408:
      uVar3 = 0;
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
      LAB_0223d41a:
        result = can_sorcery_be_played(player, event);
        uVar3 = (unsigned int)(result != 0);
      }
    LAB_0223d392:
      default_target_definition(player, card, &td, 2);
      if (event == EVENT_CAST_SPELL)
        goto LAB_0223d3c6;
    }
    if (event == EVENT_RESOLVE_SPELL)
    {
      if ((pcStack_140->info_slot & 1U) != 0)
      {
        result = card_call_to_mind(player, card, EVENT_RESOLVE_SPELL);
        return result;
      }
      if ((pcStack_140->info_slot & 2U) != 0)
      {
        result = card_frost_breath(player, card, EVENT_RESOLVE_SPELL);
        return result;
      }
      kill_card(player, card, KILL_DESTROY);
      result = 5;
    }
    else
    {
      result = 5;
    }
  }
  else
  {
    if (event != EVENT_CAST_SPELL)
    {
      if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
      {
        if (event != EVENT_CHANGE_TARGET)
        {
          return 0;
        }
        goto LAB_0223d250;
      }
      pcStack_140 = get_card_instance(player, card);
      default_test_definition(&this_test, 0x38);
      result = new_special_count_grave(player, &this_test);
      uVar3 = 0;
      if (result != 0)
        goto LAB_0223d408;
      goto LAB_0223d392;
    }
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
    pcStack_140 = get_card_instance(player, card);
    default_test_definition(&this_test, 0x38);
    result = new_special_count_grave(player, &this_test);
    if (result == 0)
    {
      default_target_definition(player, card, &td, 2);
      uVar3 = 0;
    }
    else
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
        goto LAB_0223d41a;
      uVar3 = 0;
      default_target_definition(player, card, &td, 2);
    }
  LAB_0223d3c6:
    result = 5;
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (current_turn == player))
    {
      result = ((0x17 < current_phase) - 1 & 0xfffffff6) + 0xf;
    }
  }
  can_play_second_half = can_target(&td);
  result = generic_split_card(player, card, event, 0x5a5d, uVar3, result, 0x59e2,
                                   can_play_second_half, 10);
  return result;
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
      if (result == 2)
      {
        new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      }
    }
    else if (result == 1)
    {
      surveil(player, 2);
    }
    else if (result == 2)
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
  unsigned int uVar1;
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
      uVar1 = instance->state;
      hand_count[player] = hand_count[player] + 1;
      instance->state = uVar1 & 0xffffffdf;
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
  int iVar1;
  type_t tVar2;
  unsigned int uVar3;
  int iVar4;
  card_instance_t *instance;
  target_definition_t td;
  test_definition_t test;

  iVar1 = beginning_of_combat(player, card, event, player);
  if (iVar1 != 0)
  {

    new_default_test_definition(&test, 0xff, "Select a card to discard.");
    iVar1 = new_select_a_card(player, player, 4, 0, 3, -1, &test);
    if (-1 < iVar1)
    {

      tVar2 = get_type(player, iVar1);
      uVar3 = num_bits_set(tVar2 & ~TYPE_INTERRUPT);
      iVar4 = is_tribal(player, iVar1);
      discard_card(player, iVar1);
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      td.allow_cancel = 0;
      iVar1 = can_target(&td);
      if (iVar1 != 0)
      {
        iVar1 = new_pick_target(&td, "TARGET_ANY", 0, 0);
        if (iVar1 != 0)
        {
          damage_target0(player, card, (uVar3 + 1) - (unsigned int)(iVar4 == 0));
          instance = get_card_instance(player, card);
          instance->number_of_targets = '\0';
        }
      }
    }
  }
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22305e0; original cards/modern_horizons_2.c:1596. */
int card_chatterfang_squirrel_general(int player, int card, event_t event)
{
  enum
  {
    SARV_REPORT_COUNT = 8
  };

  int iVar1;
  card_instance_t *instance;
  int iVar3;
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
      iVar1 = instance->info_slot;
      instance->info_slot = 0;
      iVar3 = valid_target(&td);
      if (iVar3 != 0)
      {
        pump_until_eot_merge_previous(player, card, instance->targets[0].player,
                                      instance->targets[0].card, iVar1, -iVar1);
      }
    }
  }
  iVar1 = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_CAN_TARGET, 0, 1, 0, 0, 0, 0, 0, &td, "TARGET_CREATURE", TYPE_NONE,
      &test, SARV_REPORT_CMC | SARV_REPORT_COUNT);
  return iVar1;
}

/* ManalinkEh 2022: 0x2230860; original cards/modern_horizons_2.c:1655. */
int card_chitterspitter(int player, int card, event_t event)
{
  enum
  {
    COUNTER_ACORN = 160
  };

  int result;
  card_instance_t *instance;
  unsigned int uVar3;
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

      uVar3 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_JUST_MARK, &test);
      if (uVar3 != 0)
      {
        add_counters(player, card, player, card, COUNTER_ACORN, 1);
        kill_card(player, uVar3 >> 0x18, KILL_SACRIFICE);
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
  result = generic_activated_ability(player, card, event, 1, 0, 0, 0, 1, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x222ba50; original cards/modern_horizons_2.c:1961. */
static int card_exiled_with_void_counter(int player, int card, event_t event)
{
  enum
  {
    COUNTER_VOID = 170
  };

  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int card_00;
  int iStack_14;

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
      iStack_14 = 0;
      iVar3 = instance->targets[0].card;
      if (0 < active_cards_count[player])
      {
        card_00 = 0;
        do
        {
          instance = in_play(player, card_00);
          if (instance != NULL)
          {
            iVar2 = is_what(player, card_00, 0x80);
            if (iVar2 != 0)
            {

              instance = get_card_instance(player, card_00);
              if (instance->info_slot_fn == card_exiled_with_void_counter)
              {
                iVar2 = count_counters(player, card_00, COUNTER_VOID);
                if ((iVar2 != 0) && (iVar3 == instance->targets[0].card))
                {
                  iStack_14 = iStack_14 + 1;
                }
              }
            }
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[player]);
      }
      iVar3 = count_iid_in_rfg(player, iVar3);
      if (iVar3 < iStack_14)
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
  bool bVar3;

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
    bVar3 = current_turn == player;
    instance = get_card_instance(player, result);
    instance->targets[1].card = (unsigned int)bVar3;
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x22419b0; original cards/modern_horizons_2.c:6955. */
int card_tizerus_charger(int player, int card, event_t event)
{
  enum
  {
    COUNTER_FLYING = 147
  };

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
  card_instance_t *instance;
  int result;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_test;

  if (event == EVENT_MODIFY_COST)
  {
    if ((card != affected_card) || (player != affected_card_controller))
    {
      if (trigger_condition != 0xdb)
      {
        return 0;
      }
      goto LAB_0223828c;
    }
    default_test_definition(&recovered_stack_test.test, 0x47);
    recovered_stack_test.test.type_flag = 6;
    result =
        check_battlefield_for_special_card(player, card, player, 4, &recovered_stack_test.test);
    COST_COLORLESS = COST_COLORLESS - (char)result;
    if (trigger_condition != 0xdb)
    {
      return 0;
    }
  }
  else if (trigger_condition != 0xdb)
    goto LAB_0223828c;
  if ((card == affected_card) && (affected_card_controller == player))
  {
    default_test_definition(&recovered_stack_test.test, 0x47);
    recovered_stack_test.test.type_flag = 6;
    etb_ability_global(player, event, 2, TYPE_NONE, &recovered_stack_test.test, player, card);
  }
LAB_0223828c:
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &recovered_stack_test.target, 0x47);
    recovered_stack_test.target.allowed_controller = (int32_t)(1 - player);
    recovered_stack_test.test.message[0x14] = '\x01';
    recovered_stack_test.test.message[0x15] = '\0';
    recovered_stack_test.test.message[0x16] = '\0';
    recovered_stack_test.test.message[0x17] = '\0';
    instance = get_card_instance(player, card);
    result = can_target(&recovered_stack_test.target);
    if ((result != 0) &&
        (result = new_pick_target(&recovered_stack_test.target,
                                  "Select target nonland permanent an opponent controls.", 0, 0x80),
         result != 0))
    {
      effect_frost_titan(player, card, instance->targets[0].player, instance->targets[0].card);
      instance->number_of_targets = '\0';
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
    COUNTER_CORRUPTION = 163
  };

  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  bool bVar6;
  int iStack_574;
  target_definition_t td;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    int value;
    target_definition_t target;
    uint8_t bytes[84];
  } recovered_stack_p;

  int aiStack_4d0[2];
  int c;
  int aiStack_4c4[301];
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
      get_protection_source('\0', &recovered_stack_p.value, &c);
      if ((((-1 < recovered_stack_p.value) && (-1 < c)) &&
           (iVar2 = is_what(recovered_stack_p.value, c, 0x47), iVar2 != 0)) &&
          (iVar2 = count_counters(recovered_stack_p.value, c, COUNTER_CORRUPTION), iVar2 != 0))
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
    default_target_definition(player, card, &recovered_stack_p.target, 0x102);
    iVar2 = can_target(&recovered_stack_p.target);
    iVar2 = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                        "Drain 2 and corrupt", 1, 0x14 - life[player], 1, "Borrow target", iVar2,
                        10, 0xfffffffd, "Steal all corrupted", 1, 0x1e, 0xfffffff9, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (iVar2 == 0)
      {
        return 0;
      }
    }
    else
    {
      instance->number_of_targets = '\0';
      if (iVar2 == 1)
      {
        td.allow_cancel = 2;
        new_pick_target(&td, "Select another target creature or planeswalker.", 0, 0x80);
      }
      else if (iVar2 == 2)
      {
        new_pick_target(&recovered_stack_p.target, "Select target creature or planeswalker.", 0,
                        0x81);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x102);
    td.special = 0x4000;
    default_target_definition(player, card, &recovered_stack_p.target, 0x102);
    iVar2 = can_target(&recovered_stack_p.target);
    iVar3 = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                        "Drain 2 and corrupt", 1, 0x14 - life[player], 1, "Borrow target", iVar2,
                        10, 0xfffffffd, "Steal all corrupted", 1, 0x1e, 0xfffffff9, 0, 0, 0, 0);
    iVar2 = current_turn;
    if (iVar3 == 1)
    {
      lose_life(1 - player, 2);
      gain_life(player, 2);
      iVar2 = validate_target(player, card, &td, 0);
      if (iVar2 != 0)
      {
        add_counters(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_CORRUPTION, 1);
      }
    }
    else if (iVar3 == 2)
    {
      iVar2 = validate_target(player, card, &recovered_stack_p.target, 0);
      if (iVar2 != 0)
      {
        add_counters(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_CORRUPTION, 1);
        effect_act_of_treason(player, card, instance->targets[0].player, instance->targets[0].card);
      }
    }
    else if (iVar3 == 3)
    {
      iVar3 = active_cards_count[current_turn] + -1;
      if (iVar3 < 0)
      {
        iStack_574 = 0;
        iVar3 = 1 - current_turn;
        iVar2 = active_cards_count[iVar3] + -1;
        if (-1 < iVar2)
          goto LAB_022366eb;
      }
      else
      {
        iStack_574 = 0;
        do
        {
          instance = in_play(iVar2, iVar3);
          if (((instance != NULL) && (iVar4 = is_what(iVar2, iVar3, 0x47), iVar4 != 0)) &&
              (iVar4 = count_counters(iVar2, iVar3, COUNTER_CORRUPTION), iVar4 != 0))
          {
            aiStack_4c4[iStack_574 * 2 + -1] = iVar2;
            aiStack_4c4[iStack_574 * 2] = iVar3;
            iStack_574 = iStack_574 + 1;
          }
          iVar3 = iVar3 + -1;
        } while (iVar3 != -1);
        iVar3 = 1 - current_turn;
        iVar2 = active_cards_count[iVar3];
        while (iVar2 = iVar2 + -1, -1 < iVar2)
        {
        LAB_022366eb:
          instance = in_play(iVar3, iVar2);
          if (((instance != NULL) && (iVar4 = is_what(iVar3, iVar2, 0x47), iVar4 != 0)) &&
              (iVar4 = count_counters(iVar3, iVar2, COUNTER_CORRUPTION), iVar4 != 0))
          {
            aiStack_4c4[iStack_574 * 2 + -1] = iVar3;
            aiStack_4c4[iStack_574 * 2] = iVar2;
            iStack_574 = iStack_574 + 1;
          }
        }
        if (0 < iStack_574)
        {
          piVar5 = aiStack_4d0 + iStack_574 * 2;
          do
          {
            gain_control_permanently(player, *piVar5, piVar5[1]);
            bVar6 = &c != piVar5;
            piVar5 = piVar5 + -2;
          } while (bVar6);
        }
      }
    }
  }
  iVar2 = planeswalker(player, card, event, 4);
  return iVar2;
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
  int iVar1;
  const char *pcVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  unsigned int uVar6;
  const char *puVar7;
  int iVar8;
  int *piVar9;
  bool bVar10;
  int iStack_404;
  target_definition_t td;
  char msg[100];
  test_definition_t test;
  int devoured[151];

  base_target_definition(player, card, &td, type);
  td.illegal_abilities = 0;
  td.allowed_controller = player;
  td.preferred_controller = player;
  default_test_definition(&test, type);
  pcVar2 = type_text(msg, 100, test.type, 0);
  if ((test.type == 4) || (test.type == 0x40))
  {
    puVar7 = "n";
  }
  else
  {
    puVar7 = "";
  }
  sprintf(test.message, "Select a%s %s to sacrifice.", puVar7, pcVar2);
  instance = get_card_instance(player, card);
  if ((ai_is_speculating == 1) ||
      ((iStack_404 = -1, player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    iStack_404 = count_subtype(player, type, -1);
    iStack_404 = iStack_404 / 3;
  }
  instance->targets[1].player = 0;
  instance->targets[1].card = 0;
  iVar4 = get_id(player, card);
  iVar8 = 0;
  do
  {
    iVar5 = can_target(&td);
    if ((iVar5 == 0) ||
        (uVar6 = new_sacrifice(player, card, player,
                               SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test),
         uVar6 == 0))
    {
      iVar5 = iVar8 + -1;
      iStack_404 = iVar8;
      break;
    }
    instance->targets[1].card = instance->targets[1].card + 1;
    if ((iVar4 == 0x27f5) &&
        (iVar5 = has_subtype(player, uVar6 >> 0x18, SUBTYPE_GOBLIN), iVar5 != 0))
    {
      instance->targets[1].player = instance->targets[1].player + 1;
    }
    iVar1 = iVar8 + 1;
    devoured[iVar8] = uVar6 >> 0x18;
    iVar5 = iVar8;
    iVar8 = iVar1;
  } while (iStack_404 != iVar1);
  if (-1 < iVar5)
  {
    piVar9 = devoured + iVar5;
    do
    {
      kill_card(player, *piVar9, KILL_SACRIFICE);
      bVar10 = piVar9 != devoured;
      piVar9 = piVar9 + -1;
    } while (bVar10);
  }
  return iStack_404;
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
  char cVar1;
  int iVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  unsigned char bVar5;
  uint32_t argument1;
  inst_info_t iVar6;
  uint32_t uStack_14;

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
  pcVar3 = get_card_instance(player, card);
  if (-1 < pcVar3->damage_target_player)
  {

    pcVar3 = get_card_instance(player, card);
    pcVar4 = get_card_instance((int)pcVar3->damage_target_player, pcVar3->damage_target_card);
    iVar2 = pcVar3->damage_target_card;
    cVar1 = pcVar3->damage_target_player;
    iVar6 = inst_info(pcVar3, 0x59dd);
    argument1 = iVar6.data;
    if ((((iVar2 == affected_card) && (affected_card_controller == cVar1)) &&
         (argument1 != 0xffffffff)) &&
        ((((unsigned int)land_can_be_played >> 16 & 0xff) & 2) == 0))
    {

      bVar5 = cards_data[argument1].type | 0x40;
      if (cards_data[argument1].type == bVar5)
      {
        pcVar3->targets[7].player = argument1;
        pcVar3->dummy3 = argument1;
        pcVar4->regen_status = pcVar4->regen_status | 0x1000000;
        uStack_14 = argument1;
      }
      else if ((pcVar3->targets[7].player != argument1) ||
               (uStack_14 = pcVar3->dummy3, cards_data[pcVar3->dummy3].type != bVar5))
      {

        uStack_14 = create_a_card_type(argument1);
        if (uStack_14 == 0xffffffff)
        {
          return 0;
        }
        cards_data[uStack_14].type = bVar5;
        pcVar3->dummy3 = uStack_14;
        pcVar3->targets[7].player = argument1;
        pcVar4->regen_status = pcVar4->regen_status | 0x1000000;
      }
      event_result = uStack_14;
    }
  }
  attached_is_additional_subtype(player, card, EVENT_CHANGE_TYPE, SUBTYPE_VEHICLE);
  return 0;
}

/* ManalinkEh 2022: 0x22378f0; original cards/modern_horizons_2.c:3666. */
int card_grist_the_hunger_tide(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  int iVar3;
  unsigned int uVar4;
  card_instance_t *inst;
  unsigned int player_00;
  int player_01;
  inst_info_t iVar5;
  uint32_t uVar6;
  target_definition_t td;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_this_test;

  check_legend_rule(player, card, event);
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE)
  {
    pcVar2 = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x102);
    iVar1 = life[1 - player];
    iVar3 = count_graveyard_by_type(1 - player, 2);
    if (iVar1 - iVar3 < 6)
    {
      iVar1 = can_target(&td);
      uVar6 = 100;
    }
    else
    {
      iVar1 = can_target(&td);
      uVar6 = 10;
    }
    iVar1 = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                        "Insect and mill a card", 1, 10, 1, "Destroy creature or PW", iVar1, 0x14,
                        0xfffffffe, "Opponent loses life", 1, uVar6, 0xfffffffb, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (iVar1 == 0)
      {
        return 0;
      }
    }
    else
    {
      pcVar2->number_of_targets = '\0';
      if (iVar1 == 2)
      {
        new_default_test_definition(&recovered_stack_this_test.test, 2,
                                    "Select a creature to sacrifice.");
        uVar4 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                              &recovered_stack_this_test.test);
        if (uVar4 != 0)
        {
          iVar1 = new_pick_target(&td, "Select target creature or planeswalker.", 0, 0x81);
          player_00 = (int)uVar4 >> 0x10 & 0xff;
          if (iVar1 == 0)
          {
            state_untargettable(player_00, uVar4 >> 0x18, 0);
          }
          else
          {
            kill_card(player_00, uVar4 >> 0x18, KILL_SACRIFICE);
          }
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {
    pcVar2 = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x102);
    player_01 = 1 - player;
    iVar1 = life[player_01];
    iVar3 = count_graveyard_by_type(player_01, 2);
    if (iVar1 - iVar3 < 6)
    {
      iVar1 = can_target(&td);
      uVar6 = 100;
    }
    else
    {
      iVar1 = can_target(&td);
      uVar6 = 10;
    }
    iVar1 = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                        "Insect and mill a card", 1, 10, 1, "Destroy creature or PW", iVar1, 0x14,
                        0xfffffffe, "Opponent loses life", 1, uVar6, 0xfffffffb, 0, 0, 0, 0);
    if (iVar1 == 1)
    {
      default_token_definition(player, card, 0x4952, &recovered_stack_this_test.token);
      recovered_stack_this_test.test.message[0x28] = '\n';
      generate_token(&recovered_stack_this_test.token);
      while (true)
      {
        inst = get_card_instance(pcVar2->parent_controller, pcVar2->parent_card);
        inst->info_slot = 0;
        special_mill(pcVar2->parent_controller, pcVar2->parent_card, 0x5a13, player, 1);
        iVar5 = inst_info(inst, 0x5a13);
        if (iVar5.data2 != 0x42)
          break;
        add_counters(player, card, pcVar2->parent_controller, pcVar2->parent_card,
                                 COUNTER_LOYALTY, 1);
      }
    }
    else if (iVar1 == 2)
    {
      iVar1 = validate_target(player, card, &td, 0);
      if (iVar1 != 0)
      {
        kill_card(pcVar2->targets[0].player, pcVar2->targets[0].card, KILL_DESTROY);
      }
    }
    else if (iVar1 == 3)
    {
      iVar1 = count_graveyard_by_type(player, 2);
      lose_life(player_01, iVar1);
    }
  }
  iVar1 = planeswalker(player, card, event, 3);
  return iVar1;
}

/* ManalinkEh 2022: 0x2239520; original cards/modern_horizons_2.c:4243. */
int card_lonis_cryptozoologist(int player, int card, event_t event)
{
  enum
  {
    SARV_REPORT_COUNT = 8
  };

  card_instance_t *instance;
  int iVar1;
  uint32_t num_cards;
  inst_info_t iVar2;
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
        iVar1 = generic_activated_ability_with_sacrifice_as_cost(
            player, card, EVENT_CAN_ACTIVATE, GAA_CAN_ONLY_TARGET_OPPONENT | GAA_UNTAPPED, 0, 0, 0,
            0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test2, SARV_REPORT_COUNT);
        return iVar1;
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
      iVar1 = valid_target(&td);
      if (iVar1 != 0)
      {

        iVar2 = inst_info(instance, 0x5a2b);
        num_cards = iVar2.data;
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
  actions_t aVar2;
  card_instance_t *instance;

  result = duh_mode(player);
  aVar2 = trigger_gain_life(player, card, event, player, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (aVar2 != 0)
  {
    instance = get_card_instance(player, card);
    if (instance->info_slot != 0x42)
    {
      instance = in_play(player, card);
      if (instance != NULL)
      {
        manipulate_type(player, card, 2, TYPE_CREATURE, aVar2 & 0xffff | ACT_ADD_COUNTERS_BASE);
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
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222ca50; original cards/modern_horizons_2.c:382. */
int card_arcbound_javelineer(int player, int card, event_t event)
{
  unsigned int uVar1;
  card_instance_t *inst;
  card_instance_t *pcVar2;
  uint32_t uVar3;
  int iVar4;
  uint32_t data;
  inst_info_t iVar5;
  target_definition_t td;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.required_state = 0x20;
    inst = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                        &td, NULL);
      return iVar4;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0x84, 0, 0, 0, 0, 0, 0, 0, &td,
                                "Select target attacking or blocking creature.");
      if (spell_fizzled == 1)
      {
        return 0;
      }

      iVar4 = get_toughness(inst->targets[0].player, inst->targets[0].card);
      pcVar2 = get_card_instance(inst->targets[0].player, inst->targets[0].card);

      uVar1 = (uint32_t)pcVar2->damage_on_card | ((uint32_t)(uint16_t)pcVar2->counter_power << 16);

      uVar3 = count_counters(player, card, COUNTER_P1_P1);
      if (player == 0)
      {
        iVar4 = count_counters(0, card, COUNTER_P1_P1);
        data = choose_a_number(0, "How many +1/+1 counters you want to remove?", iVar4);
      }
      else
      {
        data = iVar4 - (uVar1 & 0xffff);
        if ((int)uVar3 < (int)data)
        {
          data = uVar3;
        }
      }
      if ((-1 < (int)data) &&
          (iVar4 = count_counters(player, card, COUNTER_P1_P1), (int)data <= iVar4))
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
      iVar4 = modular(player, card, event, 1);
      return iVar4;
    }
    default_target_definition(player, card, &td, 2);
    td.required_state = 0x20;
    inst = get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (iVar4 = valid_target(&td), iVar4 != 0))
  {
    iVar5 = inst_info(inst, 0x59ac);
    damage_target0(player, card, iVar5.data);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2232c90; original cards/modern_horizons_2.c:2191. */
int card_dermotaxi(int player, int card, event_t event)
{
  target_t *ret_location;
  unsigned short uVar1;
  uint32_t data;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int32_t player_00;
  target_t *ptVar5;
  target_t *ptVar6;
  inst_info_t iVar7;
  int iStack_f4;
  unsigned int uStack_f0;
  card_instance_t *pcStack_ec;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  if (event == EVENT_RESOLVE_SPELL)
  {
    iVar2 = count_graveyard_by_type(player, 2);
    player_00 = 1 - player;
    iVar4 = count_graveyard_by_type(player_00, 2);
    pcVar3 = get_card_instance(player, card);
    if (iVar4 == 0)
    {
      if (iVar2 != 0)
      {
        player_00 = player;
      }
    }
    else if (iVar2 != 0)
    {
      default_target_definition(player, card, &recovered_stack_td.target, 0xff);
      recovered_stack_td.target.zone = 0x1000;
      recovered_stack_td.target.allow_cancel = 0;
      recovered_stack_td.target.illegal_abilities = 0;
      iVar2 = new_pick_target(&recovered_stack_td.target,
                              "Select the owner of the graveyard you want to search.", 0, 0x80);
      if (iVar2 != 0)
      {
        player_00 = pcVar3->targets[0].player;
        pcVar3->number_of_targets = '\0';
      }
    }
    new_default_test_definition(&recovered_stack_td.test, 2,
                                "Select a creature card to imprint on Dermotaxi.");
    iVar2 = new_select_a_card(player, player_00, 2, 1, 2, -1, &recovered_stack_td.test);
    data = graveyard_ptr_mutable[player_00][iVar2];
    rfg_card_from_grave(player_00, iVar2);
    set_inst_info(pcVar3, 0x59dd, data, (uint16_t)player_00);
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      pcStack_ec = get_card_instance(player, card);
      default_target_definition(player, card, &recovered_stack_td.target, 2);
      recovered_stack_td.target.illegal_abilities = 0;
      recovered_stack_td.target.allowed_controller = player;
      recovered_stack_td.target.preferred_controller = player;
      recovered_stack_td.target.illegal_state = 1;
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        if (iVar2 == 0)
        {
          return 0;
        }
        iVar2 = target_available(player, card, &recovered_stack_td.target);
        return (unsigned int)(1 < iVar2);
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        return 0;
      }
      pcStack_ec = get_card_instance(player, card);
      default_target_definition(player, card, &recovered_stack_td.target, 2);
    }
    recovered_stack_td.target.allowed_controller = player;
    recovered_stack_td.target.illegal_state = 1;
    recovered_stack_td.target.illegal_abilities = 0;
    recovered_stack_td.target.preferred_controller = recovered_stack_td.target.allowed_controller;
    if (event == EVENT_ACTIVATE)
    {
      pcStack_ec->number_of_targets = '\0';
      iVar2 = charge_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0);
      if (iVar2 != 0)
      {
        ret_location = pcStack_ec->targets;
        iStack_f4 = -1;
        iVar2 = select_target(player, card, &recovered_stack_td.target, "Select a creature to tap.",
                              ret_location);
        if (iVar2 != 0)
        {
          state_untargettable(pcStack_ec->targets[0].player, pcStack_ec->targets[0].card, 1);
          iVar2 = pcStack_ec->targets[0].card;
          iVar4 = select_target(player, card, &recovered_stack_td.target,
                                "Select a creature to tap.", pcStack_ec->targets + 1);
          if (iVar4 == 0)
          {
            uStack_f0 = 1;
          }
          else
          {
            state_untargettable(pcStack_ec->targets[1].player, pcStack_ec->targets[1].card, 1);
            iStack_f4 = pcStack_ec->targets[1].card;
            uStack_f0 = 2;
          }
          ptVar5 = ret_location;
          do
          {
            ptVar6 = ptVar5 + 1;
            state_untargettable(ptVar5->player, ptVar5->card, 0);
            ptVar5 = ptVar6;
          } while (ptVar6 != ret_location + uStack_f0);
          if (1 < uStack_f0)
          {
            pcStack_ec->number_of_targets = '\0';
            tap_card(player, iVar2);
            tap_card(player, iStack_f4);
            return 0;
          }
        }
        spell_fizzled = 1;
        return 0;
      }
    }
    else if ((event == EVENT_RESOLVE_ACTIVATION) &&
             (iVar7 = inst_info(pcStack_ec, 0x59dd), iVar7.data != 0))
    {
      iVar7 = inst_info(pcStack_ec, 0x59dd);
      uVar1 = cards_data[iVar7.data].id;
      iVar7 = inst_info(pcStack_ec, 0x59dd);
      iVar2 = check_rfg(((uint32_t)iVar7.id | ((uint32_t)iVar7.data2 << 16)) >> 0x10,
                        (unsigned int)uVar1);
      if (iVar2 != 0)
      {
        iVar2 = create_targetted_legacy_effect(
            player, card, dermotaxi_legacy, pcStack_ec->parent_controller, pcStack_ec->parent_card);
        pcVar3 = get_card_instance(player, iVar2);
        iVar7 = inst_info(pcStack_ec, 0x59dd);
        set_inst_info(pcVar3, 0x59dd, iVar7.data, 0);
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
    result = eot_trigger(player, card, EVENT_ETB);
    if (result == 0)
      goto LAB_02233568;
  LAB_02233604:
    kill_card(player, card, KILL_SACRIFICE);
  }
  else
  {
    result = eot_trigger(player, card, event);
    if (result != 0)
      goto LAB_02233604;
  }
  if ((event != EVENT_ABILITIES) && (event != EVENT_CAST_SPELL))
  {
  LAB_02233568:
    result = flash(player, card, event);
    return result;
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
        goto LAB_02233568;
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
  int iVar1;
  card_instance_t *instance;
  inst_info_t iVar3;

  check_legend_rule(player, card, event);
  haste(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    iVar1 = get_storm_count();
    COST_COLORLESS = COST_COLORLESS + (char)iVar1 * -3;
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
        iVar3 = inst_info(instance, 0x5a8f);
        if (iVar3.data2 == 0x42)
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
        goto LAB_0222e398;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.preferred_controller = player;
LAB_0222e398:
  result = generic_spell(player, card, event, 1, &td, "TARGET_PERMANENT", 1, NULL);
  return result;
}


/* October 2022 copied local table sources (.data, writable). */
static int garth_card_ids[7] = {-1, 64, 25, 242, 224, 202, 17};
static int serras_emissary_types[9] = {0, 2, 64, 4, 1, 256, 16, 8, 4105};

/* ManalinkEh 2022: 0x2235c00; original cards/modern_horizons_2.c:3161. */
int card_garth_one_eye(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *inst;
  card_instance_t *inst_00;
  int iVar2;
  unsigned int uVar3;
  unsigned int uVar4;
  int *piVar5;
  unsigned int uVar6;
  int *piVar7;
  unsigned int uVar8;
  unsigned char bVar9;
  inst_info_t iVar10;
  unsigned int uStack_40;
  unsigned int uStack_3c;
  unsigned int uStack_38;
  int csvids[7];
  bVar9 = 0;
  check_legend_rule(player, card, event);
  if (event != EVENT_RESOLVE_ACTIVATION)
    goto LAB_02235c2b;
  inst = get_card_instance(player, card);
  inst_00 = get_card_instance(inst->parent_controller, inst->parent_card);
  iVar10 = inst_info(inst, 0x5a04);
  uVar4 = ((uint32_t)iVar10.id | ((uint32_t)iVar10.data2 << 16)) >> 0x10;
  if ((iVar10.data2 & 0x3f) == 0x3f)
  {
    return 0;
  }
  piVar5 = garth_card_ids;
  piVar7 = csvids;
  for (iVar1 = 7; iVar1 != 0; iVar1 = iVar1 + -1)
  {
    *piVar7 = *piVar5;
    piVar5 = piVar5 + (unsigned int)bVar9 * -2 + 1;
    piVar7 = piVar7 + (unsigned int)bVar9 * -2 + 1;
  }
  if ((uVar4 & 2) == 0)
  {
    uVar8 = 1;
    if (player == 0)
      goto LAB_02235d12;
    iVar1 = has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0x40);
    uVar8 = (unsigned int)(iVar1 != 0);
    if ((((uint64_t)iVar10.id | ((uint64_t)iVar10.data2 << 16) | ((uint64_t)iVar10.data << 32)) &
         0x40000) == 0)
      goto LAB_02235fe8;
    uStack_3c = 0;
    if ((uVar4 & 8) != 0)
      goto LAB_02236020;
  LAB_02235f30:
    iVar1 = has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0xf2);
    uStack_40 = (unsigned int)(iVar1 != 0);
    if ((uVar4 & 0x10) != 0)
    {
    LAB_02235fd8:
      uStack_38 = 0;
      goto LAB_02235d5a;
    }
  LAB_02235f60:
    iVar1 = has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0xe0);
    uStack_38 = (unsigned int)(iVar1 != 0);
    if ((uVar4 & 0x20) == 0)
      goto LAB_02235f90;
  LAB_02235fd0:
    uVar6 = 0;
  LAB_02235d70:
    uVar3 = 0;
    if (((uVar4 & 0x40) == 0) && (uVar3 = 1, player != 0))
    {
    LAB_02236048:
      iVar1 = has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0x11);
      uVar3 = (unsigned int)(iVar1 != 0);
    }
  }
  else
  {
    uVar8 = 0;
  LAB_02235d12:
    if ((((uint64_t)iVar10.id | ((uint64_t)iVar10.data2 << 16) | ((uint64_t)iVar10.data << 32)) &
         0x40000) == 0)
    {
      uStack_3c = 1;
      if (player == 0)
        goto LAB_02235d2a;
    LAB_02235fe8:
      iVar1 = has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0x19);
      uStack_3c = (unsigned int)(iVar1 != 0);
      if ((uVar4 & 8) == 0)
        goto LAB_02235f30;
    LAB_02236020:
      uStack_40 = 0;
    }
    else
    {
      uStack_3c = 0;
    LAB_02235d2a:
      if ((uVar4 & 8) != 0)
        goto LAB_02236020;
      uStack_40 = 1;
      if (player != 0)
        goto LAB_02235f30;
    }
    if ((uVar4 & 0x10) != 0)
      goto LAB_02235fd8;
    uStack_38 = 1;
    if (player != 0)
      goto LAB_02235f60;
  LAB_02235d5a:
    if ((uVar4 & 0x20) != 0)
      goto LAB_02235fd0;
    uVar6 = 1;
    if (player == 0)
      goto LAB_02235d70;
  LAB_02235f90:
    iVar1 = has_mana_to_cast_id(player, EVENT_RESOLVE_ACTIVATION, 0xca);
    uVar6 = (unsigned int)(iVar1 != 0);
    if ((uVar4 & 0x40) == 0)
      goto LAB_02236048;
    uVar3 = 0;
  }
  iVar1 = count_graveyard(player);
  iVar1 = dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\002", "\001\014", "\001\004",
                      "\001\003", "Disenchant", uVar8, 5, "Braingeyser", uStack_3c,
                      hand_count[player] * -10 + 0x46, "Terror", uStack_40, 0xf, "Shivan Dragon",
                      uStack_38, 0x14, "Regrowth", uVar6, iVar1, "Black Lotus", uVar3, 0x32, 0, 0,
                      0, 0);
  set_inst_info(inst_00, 0x5a04, 0,
                (unsigned short)(1 << ((unsigned char)iVar1 & 0x1f)) | iVar10.data2);
  iVar1 = csvids[iVar1];
  iVar2 = charge_mana_from_id(player, -1, EVENT_RESOLVE_ACTIVATION, iVar1);
  if (iVar2 != 0)
  {
    copy_spell(player, iVar1);
  }
LAB_02235c2b:
  iVar1 = generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x223dfe0; original cards/modern_horizons_2.c:5774. */
int card_serras_emissary(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  uint32_t uVar3;
  int *piVar4;
  int *piVar5;
  inst_info_t iVar6;
  int type_array[9];
  if (event != EVENT_RESOLVE_SPELL)
  {
    if (event == EVENT_ABILITIES)
    {
      if ((affected_card_controller == player) &&
          (iVar2 = is_what(player, affected_card, 2), iVar2 != 0))
      {
        instance = get_card_instance(player, card);
        iVar6 = inst_info(instance, 0x5a67);
        uVar3 = iVar6.data;
        if (uVar3 == 2)
        {
          event_result = event_result | 0x200000;
        }
        else if (uVar3 == 0x40)
        {
          event_result = event_result | 0x10000;
        }
        else if (uVar3 == 1)
        {
          event_result = event_result | 0x400000;
        }
        else if (uVar3 == 0x10)
        {
          event_result = event_result | 0x40000;
        }
        else if (uVar3 == 8)
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
           (iVar2 = is_humiliated(player, card, AL_STATIC), iVar2 == 0)) &&
          (affected_card_controller == player))
      {
        iVar6 = inst_info(instance, 0x5a67);
        if (((-1 < affected_card) &&
             (iVar2 = is_what(affected_card_controller, affected_card, 2), iVar2 != 0)) ||
            (affected_card == -1))
        {
          if (((unsigned int)event_result >> 24 & 0xff) == '\x06')
          {
            event_result = event_result | 1;
          }
          if (iVar6.data == 0x1009)
          {
            iVar2 = is_tribal(attacking_card_controller, attacking_card);
          }
          else
          {
            iVar2 = is_what(attacking_card_controller, attacking_card, iVar6.data);
          }
          if (iVar2 != 0)
          {
            event_result = event_result | 1;
          }
        }
      }
    }
    return 0;
  }
  piVar4 = serras_emissary_types;
  piVar5 = type_array;
  for (iVar2 = 9; iVar2 != 0; iVar2 = iVar2 + -1)
  {
    *piVar5 = *piVar4;
    piVar4 = piVar4 + 1;
    piVar5 = piVar5 + 1;
  }
  iVar2 =
      dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\003", "\001\014", "Creature",
                  1, 10, "Artifact", 1, 10, "Enchantment", 1, 10, "Land", 1, 5, "Planeswalker", 1,
                  10, "Instant", 1, 10, "Sorcery", 1, 10, "Tribal", 1, 1, 0, 0, 0, 0);
  uVar3 = type_array[iVar2];
  instance = get_card_instance(player, card);
  set_inst_info(instance, 0x5a67, uVar3, 0);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x223fb20; original cards/modern_horizons_2.c:6351. */
int card_subtlety(int player, int card, event_t event)
{
  int to_counter_card;
  int iVar1;
  card_instance_t *instance;
  int iVar3;
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
        iVar1 = card_on_stack_controller;
        if (instance->info_slot == 1)
        {
          instance->number_of_targets = '\x01';
          instance->targets[0].player = iVar1;
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
          iVar1 = counterspell_validate(player, card, &td, 0);
          if (iVar1 != 0)
          {

            iVar1 = instance->targets[0].player;
            to_counter_card = instance->targets[0].card;
            iVar3 = pre_counterspell(player, card, iVar1, to_counter_card);
            if (iVar3 != 0)
            {

              iVar3 = do_dialog(iVar1, iVar1, to_counter_card, -1, -1,
                                " Put on top of deck\n Put on bottom of deck", 0);
              action_on_card(player, card, iVar1, to_counter_card, (iVar3 != 0) + ACT_PUT_ON_TOP);
            }
          }
          instance->number_of_targets = '\0';
          iVar1 = evoked(player, card);
          if (iVar1 != 0)
          {
            kill_card(player, card, KILL_SACRIFICE);
          }
        }
      }
    }
    iVar1 = evoke_with_cards_in_hand(player, card, event, 4);
    return iVar1;
  }
  counterspell_target_definition(player, card, &td, 0x102);
  iVar1 = counterspell(player, card, EVENT_CAN_CAST, &td, 1);
  if (iVar1 != 0)
  {
    instance = get_card_instance(player, card);
    instance->info_slot = 1;
    return 99;
  }
  instance = get_card_instance(player, card);
  instance->info_slot = 0;
  iVar1 = flash(player, card, EVENT_CAN_CAST);
  return iVar1;
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
  unsigned int uVar1;
  card_ptr_t *pcVar2;
  int result;

  uVar1 = (uint32_t)cards_data[iid].id | ((uint32_t)cards_data[iid].reserved2 << 16);
  result = has_no_mana_cost(-1, iid);
  if ((((result == 0) && (result = is_what(-1, iid, 1), result == 0)) &&
       (pcVar2 = cards_ptr[uVar1 & 0xffff], pcVar2->req_colorless < 2)) &&
      (((pcVar2->req_black == '\0' && (pcVar2->req_blue == '\0')) &&
        ((pcVar2->req_green == '\0' && (pcVar2->req_red == '\0'))))))
  {
    return (unsigned int)(pcVar2->req_white == '\0');
  }
  return 0;
}


/* ManalinkEh 2022: 0x2242640; original cards/modern_horizons_2.c:7224. */
int card_urzas_saga(int player, int card, event_t event)
{
  int iVar1;
  unsigned int uVar2;
  card_instance_t *instance;
  unsigned int uVar4;
  inst_info_t iVar5;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_test;

  if (event == EVENT_CHAPTER1)
  {
    instance = get_card_instance(player, card);
    iVar5 = inst_info(instance, 0x5a9e);
    set_inst_info(instance, 0x5a9e, 0, iVar5.data2 | 1);
  LAB_0224267a:
    if (event == EVENT_CAN_ACTIVATE)
      goto LAB_0224267f;
  }
  else
  {
    if (event == EVENT_CHAPTER2)
    {
      instance = get_card_instance(player, card);
      iVar5 = inst_info(instance, 0x5a9e);
      set_inst_info(instance, 0x5a9e, 0, iVar5.data2 | 2);
      goto LAB_0224269d;
    }
    if (event == EVENT_CHAPTER3)
    {
      new_default_test_definition(&recovered_stack_test.test, 0x40,
                                  "Select an artifact card with mana cost |0 or |1.");
      recovered_stack_test.test.special_selection_function = has_mana_cost_0_or_1;
      new_global_tutor(player, player, 1, 4, 0, 1, &recovered_stack_test.test);
      goto LAB_0224269d;
    }
    if (event != EVENT_COUNT_MANA)
      goto LAB_0224267a;
  LAB_0224267f:
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (instance = in_play(player, card), instance != NULL)) &&
        (iVar5 = inst_info(instance, 0x5a9e),
         (((uint64_t)iVar5.id | ((uint64_t)iVar5.data2 << 16) | ((uint64_t)iVar5.data << 32)) &
          0x10000) != 0))
    {
      iVar1 = mana_producer(player, card, event);
      return iVar1;
    }
  }
  if (event == EVENT_ACTIVATE)
  {
    instance = in_play(player, card);
    instance->mode = '\0';
    iVar5 = inst_info(instance, 0x5a9e);
    uVar4 = 0;
    if ((((uint64_t)iVar5.id | ((uint64_t)iVar5.data2 << 16) | ((uint64_t)iVar5.data << 32)) &
         0x10000) != 0)
    {
      iVar1 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      uVar4 = (unsigned int)(iVar1 != 0);
    }
    iVar1 = paying_mana();
    if (iVar1 == 0)
    {
      uVar2 = 0;
      if ((((uint32_t)iVar5.id | ((uint32_t)iVar5.data2 << 16)) >> 0x10 & 2) != 0)
      {
        iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 3, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        uVar2 = (unsigned int)(iVar1 != 0);
      }
    }
    else
    {
      uVar2 = 0;
    }
    iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\033",
                        "Produce mana", uVar4, 5, "Create a token", uVar2, 10, 0, 0, 0, 0);
    if (iVar1 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    if (instance->mode == '\x01')
    {
      iVar1 = mana_producer(player, card, EVENT_ACTIVATE);
      return iVar1;
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
        if (((current_turn != player) || (iVar1 = 0x30, 0x14 < current_phase)) &&
            (iVar1 = -0x30, current_turn == 1 - player))
        {
          iVar1 = ((current_phase < 0x17) - 1 & 0xffffffa0) + 0x30;
        }
        ai_modifier = ai_modifier + iVar1;
      }
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) &&
           (instance = get_card_instance(player, card), instance->mode == '\x02'))
  {
    default_token_definition(player, card, 0x4918, &recovered_stack_test.token);
    recovered_stack_test.token.tou = 0;
    recovered_stack_test.token.pow = 0;
    recovered_stack_test.token.special_infos = 66;
    generate_token(&recovered_stack_test.token);
  }
LAB_0224269d:
  iVar1 = saga(player, card, event, 3);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x222c7a0; original cards/modern_horizons_2.c:326. */
int card_aeve_progenitor_ooze(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iStack_18;

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
          (iVar1 = is_token(player, card), iVar1 != 0))
      {
        remove_a_subtype(player, card, SUBTYPE_LEGEND);
      }
    }
    else if ((((event == EVENT_THIS_SPELL_CAST) ||
               (event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE)) &&
              (card == affected_card)) &&
             ((player == affected_card_controller &&
               (iVar1 = check_special_flags2(player, card, 0x100), iVar1 == 0))))
    {

      if (active_cards_count[player] < 1)
      {
        iStack_18 = 0;
      }
      else
      {
        iVar1 = 0;
        iStack_18 = 0;
        do
        {
          if ((card != iVar1) && (instance = in_play(player, iVar1), instance != NULL))
          {
            iVar3 = has_subtype(player, iVar1, SUBTYPE_OOZE);
            iStack_18 = (iStack_18 + 1) - (unsigned int)(iVar3 == 0);
          }
          iVar1 = iVar1 + 1;
        } while (iVar1 < active_cards_count[player]);
      }
      hack_silent_counters = hack_silent_counters + 1;
      add_counters(player, card, player, card, COUNTER_P1_P1, iStack_18);
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
      goto LAB_0223e73a;
    }
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      result = generic_activated_ability(player, card, event, 0, 2, 0, 0, 0, 0, 1, 0, NULL, NULL);
      return result;
    }
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    instance = get_card_instance(player, card);
    bounce_permanent(instance->parent_controller, instance->parent_card);
  }
LAB_0223e73a:
  attached_gets_pt(player, card, event, 1, 1);
  attached_gets_kw(player, card, event, KEYWORD_FLYING, SP_KEYWORD_0);
  result = aura(player, card, event, NULL, NULL);
  return result;
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
  xtrigger_t xVar1;
  int result;
  card_instance_t *instance;
  target_definition_t td;

  return_from_oblivion(player, card, event);
  renown(player, card, event, 2);
  enable_xtrigger_flags = enable_xtrigger_flags | ENABLE_XTRIGGER_1_1_COUNTERS;
  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if ((((xVar1 == XTRIGGER_1_1_COUNTERS) && (card == affected_card)) &&
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
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int (*paiVar5)[2];
  int iVar6;
  int iStack_4d0;
  int (*paiStack_4cc)[2];
  int marked[151][2];

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar3 = count_subtype(1 - player, 2, -1);
      ai_modifier = ai_modifier + iVar3 * 5;
      iVar3 = count_subtype(player, 2, -1);
      ai_modifier = ai_modifier + iVar3 * -5;
    }
    etb_ability(player, card, EVENT_CAST_SPELL, RESOLVE_TRIGGER_MANDATORY);
    goto LAB_0223b0f4;
  }
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  iVar3 = current_turn;
  if (event != EVENT_ETB)
    goto LAB_0223b0f4;

  iVar6 = active_cards_count[current_turn] + -1;
  if (iVar6 < 0)
  {
    iVar4 = 0;
    iVar6 = 1 - current_turn;
    iVar3 = active_cards_count[iVar6] + -1;
    if (-1 < iVar3)
      goto LAB_0223b02b;
  LAB_0223b1f0:
    iVar3 = 0;
  }
  else
  {
    iVar4 = 0;
    do
    {
      instance = in_play(iVar3, iVar6);
      if ((instance != NULL) && (iVar2 = is_what(iVar3, iVar6, 2), iVar2 != 0))
      {
        marked[iVar4][0] = iVar3;
        marked[iVar4][1] = iVar6;
        iVar4 = iVar4 + 1;
      }
      iVar6 = iVar6 + -1;
    } while (iVar6 != -1);
    iVar6 = 1 - current_turn;
    iVar3 = active_cards_count[iVar6];
    while (iVar3 = iVar3 + -1, -1 < iVar3)
    {
    LAB_0223b02b:
      while ((instance = in_play(iVar6, iVar3),
              instance != NULL && (iVar2 = is_what(iVar6, iVar3, 2), iVar2 != 0)))
      {
        marked[iVar4][1] = iVar3;
        marked[iVar4][0] = iVar6;
        iVar4 = iVar4 + 1;
        iVar3 = iVar3 + -1;
        if (iVar3 < 0)
          goto LAB_0223b065;
      }
    }
  LAB_0223b065:
    if (iVar4 < 1)
      goto LAB_0223b1f0;
    paiVar5 = (int (*)[2])(&iStack_4d0 + iVar4 * 2);
    iStack_4d0 = iVar4;
    do
    {
      iVar3 = (*paiVar5)[1];
      iVar6 = (*paiVar5)[0];
      paiStack_4cc = paiVar5;
      untap_card(iVar6, iVar3);
      phase_out_target_until_source_leaves_play(player, card, iVar6, iVar3);
      paiVar5 = paiVar5 + -1;
      iVar3 = iStack_4d0;
    } while (paiStack_4cc != marked);
  }
  add_counters(player, card, player, card, COUNTER_TIME, iVar3);
LAB_0223b0f4:
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
  unsigned int uVar3;
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
      result = can_target(&td);
      return result;
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
            (uVar3 = pay_overload(player, card, 4, 2, 0, 0, 0, 0), uVar3 == 1))
        {
          new_pick_target(&td, "Select land that is snow or could produce |C.", 0, 0x81);
          if (spell_fizzled == 1)
          {
            return 0;
          }
          uVar3 = 1;
        }
        instance->info_slot = instance->info_slot | uVar3;
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
  result = overload(player, card, event, 4, 2, 0, 0, 0, 0);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2231430; original cards/modern_horizons_2.c:1895. */
int card_damn2(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
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
      result = can_target(&td);
      return result;
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
            (uVar3 = pay_overload(player, card, 2, 0, 0, 0, 0, 2), uVar3 == 1))
        {
          new_pick_target(&td, "TARGET_CREATURE", 0, 1);
          if (spell_fizzled == 1)
          {
            return 0;
          }
          uVar3 = 1;
        }
        instance->info_slot = instance->info_slot | uVar3;
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
  result = overload(player, card, event, 2, 0, 0, 0, 0, 2);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2242b50; original cards/modern_horizons_2.c:7296. */
int card_vectis_gloves(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int uVar4;
  int card_00;

  if (((((event == EVENT_BLOCK_LEGALITY) &&
         (instance = is_equipping(player, card), instance != NULL)) &&
        (iVar1 = is_humiliated(player, card, AL_STATIC), iVar1 == 0)) &&
       (((player_bits[player] & PB_NONSTANDARD_LANDWALK_DISABLED) == 0 &&
         (instance = get_card_instance(player, card),
          attacking_card_controller == instance->damage_target_player)))) &&
      (attacking_card == instance->damage_target_card))
  {
    iVar1 = 1 - player;
    uVar4 = 0;
    if (0 < active_cards_count[iVar1])
    {
      card_00 = 0;
      do
      {
        instance = in_play(iVar1, card_00);
        if (((instance != NULL) && (iVar3 = is_what(iVar1, card_00, 1), iVar3 != 0)) &&
            (iVar3 = is_what(iVar1, card_00, 0x40), iVar3 != 0))
        {
          uVar4 = 1;
          break;
        }
        card_00 = card_00 + 1;
      } while (card_00 < active_cards_count[iVar1]);
    }
    event_result = event_result | uVar4;
  }
  iVar1 = vanilla_equipment(player, card, event, 2, 2, 0, 0, 0);
  return iVar1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x223c8c0; original cards/modern_horizons_2.c:5364. */
int card_rise_and_shine(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  unsigned int uVar3;
  int iVar4;
  int *piVar5;
  bool bVar6;
  int iStack_2c4;
  target_definition_t td;
  int marked[151];
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x40);
    td.illegal_type = 2;
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
    iVar4 = basic_spell(player, card, EVENT_CAN_CAST);
    if (iVar4 != 0)
    {
      if (instance->info_slot == 1)
      {
        return 1;
      }
      iVar4 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
      return iVar4;
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
        iVar4 = check_special_flags2(player, card, -0x80000000);
        if ((iVar4 != 0) || (uVar3 = pay_overload(player, card, 4, 0, 2, 0, 0, 0), uVar3 == 1))
        {
          new_pick_target(&td, "Select targer noncreature artifact", 0, 0x81);
          if (spell_fizzled == 1)
          {
            return 0;
          }
          uVar3 = 1;
        }
        instance->info_slot = instance->info_slot | uVar3;
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
        iVar4 = valid_target(&td);
        if (iVar4 != 0)
        {
          add_counters(player, card, instance->targets[0].player,
                                   instance->targets[0].card, COUNTER_P1_P1, 4);
          artifact_animation(player, card, instance->targets[0].player, instance->targets[0].card,
                             0, 0, 0, 0, 0);
        }
      }
      else
      {
        if (0 < active_cards_count[player])
        {
          iVar4 = 0;
          iStack_2c4 = 0;
          do
          {
            while (((instance = in_play(player, iVar4),
                     instance != NULL && (iVar2 = is_what(player, iVar4, 0x40), iVar2 != 0)) &&
                    (iVar2 = is_what(player, iVar4, 2), iVar2 == 0)))
            {
              marked[iStack_2c4] = iVar4;
              iStack_2c4 = iStack_2c4 + 1;
              iVar4 = iVar4 + 1;
              if (active_cards_count[player] <= iVar4)
                goto LAB_0223c9d0;
            }
            iVar4 = iVar4 + 1;
          } while (iVar4 < active_cards_count[player]);
        LAB_0223c9d0:
          if (0 < iStack_2c4)
          {
            piVar5 = marked + iStack_2c4 + -1;
            do
            {
              iVar4 = *piVar5;
              add_counters(player, card, player, iVar4, COUNTER_P1_P1, 4);
              artifact_animation(player, card, player, iVar4, 0, 0, 0, 0, 0);
              bVar6 = marked != piVar5;
              piVar5 = piVar5 + -1;
            } while (bVar6);
          }
        }
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  iVar4 = overload(player, card, event, 4, 0, 2, 0, 0, 0);
  return iVar4;
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
  CommandChoices CVar1;
  int iVar2;
  int iVar3;
  CommandChoices *pCVar4;
  const char *prompt;
  card_instance_t *instance;
  CommandChoices *pCStack_220;
  test_definition_t *ptStack_21c;
  target_definition_t *ptStack_218;
  int iStack_214;
  int priority3;
  target_definition_t *ptStack_210;
  CommandChoices choices[2];
  int abils[5];
  int priority[5];
  token_generation_t token;
  target_definition_t td_spell;
  target_definition_t td_player;
  test_definition_t test;
  if (event == EVENT_CAN_CAST)
  {
  LAB_02242f74:
    counter_activated_target_definition(player, card, &td_spell, 0x100);
    iStack_214 = can_counter_activated_ability(player, card, event, &td_spell);
    default_target_definition(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    new_default_test_definition(&test, 0xff, "Select target card to exile.");
    instance = get_card_instance(player, card);
  }
  else
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
      goto LAB_02242f74;
    }
    if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      goto LAB_02242f74;
    }
    counter_activated_target_definition(player, card, &td_spell, 0x100);
    iStack_214 = can_counter_activated_ability(player, card, event, &td_spell);
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
      goto LAB_02242f45;
    }
  }
  ptStack_21c = &test;
  ptStack_218 = &td_player;
  abils[1] = generic_spell(player, card, EVENT_CAN_CAST, 1, ptStack_218, NULL, 1, NULL);
  abils[2] = (int)(iStack_214 != 0);
  abils[3] = generic_spell(player, card, EVENT_CAN_CAST, 0x1000, NULL, NULL, 1, ptStack_21c);
  abils[4] = abils[1];
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    iVar3 = 1 - player;
    iVar2 = count_subtype(iVar3, 2, -1);
    priority[1] = iVar2 * 5;
    priority[2] = (-(unsigned int)(abils[2] == 0) & 0xfff0bdc1) + 1000000;
    iVar2 = graveyard_has_shroud(iVar3);
    priority3 = 5;
    if (iVar2 == 0)
    {
      priority3 = count_graveyard(iVar3);
    }
    priority[4] = ((life[player] < 6) - 1 & 0xffffffd8) + 0x32;
  }
  else
  {
    priority3 = priority[3];
  }
  pCVar4 = choices;
  CVar1 = dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                      "2 tapped Squirrels", abils[1], priority[1], "Counter PW loyalty ability",
                      abils[2], priority[2], "Exile card in grave", abils[3], priority3,
                      "Gain 3 life", abils[4], priority[4], 0, 0, 0, 0);
  if (event == EVENT_CAN_CAST)
  {
    if (iStack_214 != 0)
    {
      CVar1 = 99;
    }
    return CVar1;
  }
  choices[0] = CVar1;
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
    instance->number_of_targets = '\0';
    iVar3 = 0;
    do
    {
      switch (CVar1)
      {
      case CHOICE_INVALID_TARGET:
        spell_fizzled = 1;
        break;
      case CHOICE_SQUIRREL:
        prompt = "Select target player to get squirrels.";
        goto code_r0x02243558;
      case CHOICE_COUNTER_PW_ABIL:
        cast_counter_activated_ability(player, card, iVar3);
        iVar3 = iVar3 + 1;
        break;
      case CHOICE_EXILE_CARD:
        iVar2 = select_target_from_either_grave(player, card, SFG_0, 3, 1, ptStack_21c, iVar3,
                                                     iVar3 + 1);
        if (iVar2 == -1)
        {
          spell_fizzled = 1;
          return 0;
        }
        iVar3 = iVar3 + 2;
        break;
      case CHOICE_3_LIFE:
        prompt = "Select target player to gain life.";
      code_r0x02243558:
        td_player.preferred_controller = player;
        iVar2 = new_pick_target(ptStack_218, prompt, iVar3, 0x81);
        if (iVar2 == 0)
        {
          return 0;
        }
        iVar3 = iVar3 + 1;
      }
      pCVar4 = pCVar4 + 1;
      if (pCVar4 == choices + 2)
      {
        return 0;
      }
      CVar1 = *pCVar4;
    } while (true);
  }
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }
LAB_02242f45:
  ptStack_210 = &td_spell;
  ptStack_218 = &td_player;
  pCStack_220 = choices;
  iVar3 = 0;
LAB_02242f52:
  do
  {
    switch (*pCStack_220)
    {
    case CHOICE_INVALID_TARGET:
      goto code_r0x022432f5;
    case CHOICE_SQUIRREL:
      iVar2 = validate_target(player, card, ptStack_218, iVar3);
      if (iVar2 == 0)
        goto code_r0x022432f5;
      default_token_definition(player, card, 0x4991, &token);
      token.t_player = instance->targets[iVar3].player;
      token.tou = 1;
      token.pow = 1;
      token.color_forced = '\b';
      token.qty = '\x02';
      token.action = TOKEN_ACTION_TAPPED;
      generate_token(&token);
      goto code_r0x022432f6;
    case CHOICE_COUNTER_PW_ABIL:
      resolve_counter_activated_ability(player, card, ptStack_210, iVar3);
      goto code_r0x022432f5;
    case CHOICE_EXILE_CARD:
      iVar2 = validate_target_from_grave_source(player, card, instance->targets[iVar3].player,
                                                iVar3 + 1);
      if (iVar2 != -1)
      {
        rfg_card_from_grave(instance->targets[iVar3].player, iVar2);
      }
      iVar3 = iVar3 + 2;
      break;
    case CHOICE_3_LIFE:
      iVar2 = validate_target(player, card, ptStack_218, iVar3);
      if (iVar2 != 0)
      {
        gain_life(instance->targets[iVar3].player, 3);
      }
    }
    pCStack_220 = pCStack_220 + 1;
  } while (pCStack_220 != choices + 2);
code_r0x02243242:
  kill_card(player, card, KILL_DESTROY);
  return 0;
code_r0x022432f5:
code_r0x022432f6:
  iVar3 = iVar3 + 1;
  pCStack_220 = pCStack_220 + 1;
  if (pCStack_220 == choices + 2)
    goto code_r0x02243242;
  goto LAB_02242f52;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x222b230; original cards/modern_horizons_2.c:1987. */
static int create_dauthi_voidwalker_legacy(int player, int card, int iid)
{
  enum
  {
    COUNTER_VOID = 170
  };
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
  enum
  {
    COUNTER_VOID = 170
  };
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
