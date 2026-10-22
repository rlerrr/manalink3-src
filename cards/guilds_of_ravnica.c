#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int jumpstart(int player, int card, event_t event, int id);

/* ManalinkEh 2022: 0x2619aa0; original cards/guilds_of_ravnica.c:41. */
int jumpstart(int player, int card, event_t event, int id)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = has_mana_to_cast_id(player, EVENT_GRAVEYARD_ABILITY, id);
    if ((result != 0) && (hand_count[player] != 0))
    {
      result = get_internal_card_id_from_csv_id(id);
      result = can_legally_play_iid_now(player, result, EVENT_GRAVEYARD_ABILITY);
      if (result != 0)
      {
        return 2;
      }
    }
  }
  else if ((event == EVENT_PAY_FLASHBACK_COSTS) &&
           (charge_mana_from_id(player, -1, EVENT_PAY_FLASHBACK_COSTS, id), spell_fizzled != 1))
  {
    new_default_test_definition(&this_test, 0xff, "Select a card to discard.");
    result = new_select_a_card(player, player, 4, 0, 3, -1, &this_test);
    if (result == -1)
    {
      spell_fizzled = 1;
      return 0;
    }
    discard_card(player, result);
    return 2;
  }
  return 0;
}

/* ManalinkEh 2022: 0x261a6e0; original cards/guilds_of_ravnica.c:449. */
int card_camaraderie(int player, int card, event_t event)
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
      result = count_permanents_by_type(player, TYPE_CREATURE);
      gain_life_2022(player, result);
      draw_cards(player, result);
      pump_creatures_until_eot_merge_pt(player, card, player, 1, 1, NULL);
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

/* ManalinkEh 2022: 0x261a8f0; original cards/guilds_of_ravnica.c:539. */
int card_chemisters_insight(int player, int card, event_t event)
{
  int result;
  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    result = jumpstart(player, card, event, 0x4b6e);
    return result;
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
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      draw_cards(player, 2);
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

/* ManalinkEh 2022: 0x261a9c0; original cards/guilds_of_ravnica.c:565. */
int card_circuitous_route(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = get_total_available_lands(player);
      ai_modifier = ai_modifier + ((result < 6) - 1 & 0xffffffb8) + 0x30;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&test, 1, "Choose a basic land card or Gate card.");
    test.subtype = 0x2000;
    test.sub2 = 0x500c;
    test.subtype_flag = 3;
    test.qty = 2;
    test.no_shuffle = 1;
    new_global_tutor(player, player, 1, 5, 0, 1, &test);
    shuffle(player);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x261aae0; original cards/guilds_of_ravnica.c:638. */
int card_conclave_tribunal(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  return_from_oblivion(player, card, event);
  generic_spell_with_convoke(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = 1 - player;
    td.illegal_type = 1;
    result = can_target(&td);
    if (result != 0)
    {
      result =
          new_pick_target(&td, "Select target nonland permanent an opponent controls.", 0, 0x80);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        obliviation(player, card, instance->targets[0].player, instance->targets[0].card);
        instance->number_of_targets = '\0';
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x261b640; original cards/guilds_of_ravnica.c:956. */
int card_direct_current(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    result = jumpstart(player, card, event, 0x4b89);
    return result;
  }
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_CREATURE", 1, NULL);
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
  }
  else
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 2);
      if (event == EVENT_RESOLVE_SPELL)
      {
        damage_target0(player, card, 2);
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_0261b6a9;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 2);
LAB_0261b6a9:
  result = generic_spell(player, card, event, 1, &td, "TARGET_ANY", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x261d010; original cards/guilds_of_ravnica.c:1594. */
int card_glowspore_shaman(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    mill(player, 3);
    new_default_test_definition(&this_test, 1, "Select a land card.");
    result = new_special_count_grave(player, &this_test);
    if (result != 0)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        new_global_tutor(player, player, 2, 2, 0, 1, &this_test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x261d540; original cards/guilds_of_ravnica.c:1690. */
int card_golgari_findbroker(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&this_test, 0x47, "Select target permanent card.");
    result = new_special_count_grave(player, &this_test);
    if (result != 0)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        new_global_tutor(player, player, 2, 1, 1, 1, &this_test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x261d5f0; original cards/guilds_of_ravnica.c:1749. */
int card_guild_summit(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  int iStack_ec;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    if (trigger_cause_controller != player)
    {
    LAB_0261d6f8:
      if (trigger_condition == 0xdb)
      {
      LAB_0261d704:
        if (card == affected_card)
          goto LAB_0261d668;
      }
      goto LAB_0261d6c8;
    }
    if (trigger_cause == card)
    {
      default_target_definition(player, card, &recovered_stack_td.target, 0x47);
      recovered_stack_td.target.required_subtype = 0x500c;
      recovered_stack_td.target.preferred_controller = player;
      recovered_stack_td.target.allowed_controller = player;
      recovered_stack_td.target.illegal_abilities = 0;
      recovered_stack_td.target.illegal_state = 1;
      recovered_stack_td.target.allow_cancel = 3;
      iStack_ec = 0;
      while ((result = can_target(&recovered_stack_td.target),
              result != 0 && (result = new_pick_target(&recovered_stack_td.target,
                                                       "Choose an untapped Gate to tap", 0, 0x80),
                              result != 0)))
      {
        instance = get_card_instance(player, card);
        result = instance->targets[0].card;
        instance = get_card_instance(player, card);
        tap_card(instance->targets[0].player, result);
        iStack_ec = iStack_ec + 1;
      }
      if (iStack_ec != 0)
      {
        draw_cards(player, iStack_ec);
      }
      goto LAB_0261d6f8;
    }
    if (trigger_condition == 0xdb)
      goto LAB_0261d704;
  }
  else
  {
    if ((trigger_condition != 0xdb) || (affected_card != card))
      goto LAB_0261d644;
  LAB_0261d668:
    if (player == affected_card_controller)
    {
      default_test_definition(&recovered_stack_td.test, 0x47);
      etb_ability_global(player, event, 2, TYPE_NONE, &recovered_stack_td.test, player, card);
    }
    if (event != EVENT_ETB)
      goto LAB_0261d644;
  LAB_0261d6c8:
    if (trigger_cause == card)
      goto LAB_0261d644;
  }
  draw_cards(player, 1);
LAB_0261d644:
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x261d840; original cards/guilds_of_ravnica.c:1856. */
int card_hunted_witness(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  default_token_definition(player, card, 0x4986, &token);
  token.s_key_plus = 2;
  generate_token(&token);
  return 0;
}

/* ManalinkEh 2022: 0x261e080; original cards/guilds_of_ravnica.c:2130. */
int card_kraul_harpooner(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = 1 - player;
    td.required_abilities = 0x20;
    td.allow_cancel = 2;
    result = count_graveyard_by_type(player, 2);
    pump_until_eot_2022(player, card, player, card, result, 0);
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(
          &td, "Choose up to one target creature with flying you don\'t control.", 0, 0x80);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        result = instance->targets[0].card;
        instance = get_card_instance(player, card);
        fight(player, card, instance->targets[0].player, result);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x261e190; original cards/guilds_of_ravnica.c:2167. */
int card_lava_coil(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 2);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            instance = get_card_instance(player, card);
            exile_if_would_be_put_into_graveyard(player, card, instance->targets[0].player,
                                                 instance->targets[0].card, 1);
            damage_target0(player, card, 4);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0261e1e5;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
LAB_0261e1e5:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2619110; original cards/guilds_of_ravnica.c:2237. */
static void haste_and_must_attack_until_eot(token_generation_t *token, int beast, int number)
{
  pump_ability_until_eot_2022(token->s_player, token->s_card, token->t_player, beast, 0, 0, 0, 0x404);
  return;
}

/* ManalinkEh 2022: 0x261e850; original cards/guilds_of_ravnica.c:2380. */
int card_maximize_altitude(int player, int card, event_t event)
{
  int result;
  if ((event != EVENT_GRAVEYARD_ABILITY) && (event != EVENT_PAY_FLASHBACK_COSTS))
  {
    result = vanilla_instant_pump_2022(player, card, event, 2, player, 1, 1, 0x20, 0);
    return result;
  }
  result = jumpstart(player, card, event, 0x4be6);
  return result;
}

/* ManalinkEh 2022: 0x261e8d0; original cards/guilds_of_ravnica.c:2393. */
int card_maximize_velocity(int player, int card, event_t event)
{
  int result;
  if ((event != EVENT_GRAVEYARD_ABILITY) && (event != EVENT_PAY_FLASHBACK_COSTS))
  {
    result = vanilla_instant_pump_2022(player, card, event, 2, player, 1, 1, 0, 4);
    return result;
  }
  result = jumpstart(player, card, event, 0x4be7);
  return result;
}

/* ManalinkEh 2022: 0x261eea0; original cards/guilds_of_ravnica.c:2658. */
int card_nightveil_predator(int player, int card, event_t event)
{
  deathtouch_2022(player, card, event);
  hexproof_2022(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x261f880; original cards/guilds_of_ravnica.c:3018. */
int card_pitiless_gorgon(int player, int card, event_t event)
{
  int result;
  deathtouch_2022(player, card, event);
  if (event != EVENT_MODIFY_COST)
  {
    if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      result = hybrid_casting(player, card, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
      }
    }
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  modify_cost_for_hybrid_spells(player, card, EVENT_MODIFY_COST, 0);
  return 0;
}

/* ManalinkEh 2022: 0x261f8c0; original cards/guilds_of_ravnica.c:3029. */
int card_plaguecrafter(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  unsigned int t_player;
  int sac[2];
  test_definition_t test;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 0x102, "Select a creature or planeswalker to sacrifice.");
    iVar2 = current_turn;
    sac[0] = 0;
    sac[1] = 0;
    iVar1 = new_sacrifice(player, card, current_turn, SAC_CAUSED | SAC_NO_CANCEL, &test);
    sac[iVar2] = iVar1;
    t_player = current_turn ^ 1;
    iVar2 = new_sacrifice(player, card, t_player, SAC_CAUSED | SAC_NO_CANCEL, &test);
    sac[t_player] = iVar2;
    if (sac[current_turn] == 0)
    {
      discard(current_turn, 2, player);
    }
    if (sac[current_turn ^ 1U] == 0)
    {
      discard(current_turn ^ 1U, 2, player);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x261fb10; original cards/guilds_of_ravnica.c:3091. */
int card_quasiduplicate(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    result = jumpstart(player, card, event, 0x4c06);
    return result;
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 2);
        td.allowed_controller = player;
        td.preferred_controller = player;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            copy_token_2022(player, card, player, instance->targets[0].card);
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
        goto LAB_0261fb99;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = player;
  td.preferred_controller = player;
  get_card_instance(player, card);
LAB_0261fb99:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE_YOU_CONTROL", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x261fcf0; original cards/guilds_of_ravnica.c:3128. */
int card_radical_idea(int player, int card, event_t event)
{
  int result;
  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    result = jumpstart(player, card, event, 0x4c07);
    return result;
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
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      draw_a_card(player);
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
    {
      return 0;
    }
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x26205b0; original cards/guilds_of_ravnica.c:3394. */
int card_ritual_of_soot(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&test, 2);
    test.cmc = 4;
    test.cmc_flag = 3;
    new_manipulate_all(player, card, 2, &test, ACT_KILL_DESTROY);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2620970; original cards/guilds_of_ravnica.c:3477. */
int card_selective_snare(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  int iVar3;
  card_instance_t *instance;
  int iVar5;
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
      default_target_definition(player, card, &td, 2);
      instance = get_card_instance(player, card);
      td.required_subtype = select_a_subtype(player, card);
      instance->targets[0x11].card = td.required_subtype;
      instance->number_of_targets = instance->number_of_targets + 0xff;
      goto LAB_026209dc;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 2);
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        td.required_subtype = instance->targets[0x11].card;
        if (instance->number_of_targets != '\0')
        {
          iVar3 = 0;
          do
          {
            while (iVar5 = validate_target(player, card, &td, iVar3), iVar5 != 0)
            {
              ptVar2 = instance->targets + iVar3;
              ptVar1 = instance->targets + iVar3;
              iVar3 = iVar3 + 1;
              bounce_permanent(ptVar1->player, ptVar2->card);
              if ((int)(unsigned int)instance->number_of_targets <= iVar3)
                goto LAB_02620aae;
            }
            iVar3 = iVar3 + 1;
          } while (iVar3 < (int)(unsigned int)instance->number_of_targets);
        }
      LAB_02620aae:
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_026209dc;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_026209dc:
  iVar3 = x_spell_with_x_targets(player, card, event, &td, "TARGET_CREATURE", 0);
  return iVar3;
}

/* ManalinkEh 2022: 0x2620c30; original cards/guilds_of_ravnica.c:3696. */
int card_thief_of_sanity(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  int pos;
  unsigned int uVar5;
  unsigned int uVar6;
  int times_damaged[2];
  test_definition_t test;
  iVar2 = damage_dealt_by_me(player, card, event,
                             DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                 DDBM_MUST_BE_COMBAT_DAMAGE);
  if (iVar2 != 0)
  {
    uVar5 = 0;
    instance = get_card_instance(player, card);
    uVar6 = instance->targets[1].player;
    instance->targets[1].player = 0;
    times_damaged[0] = uVar6 & 0xff;
    times_damaged[1] = uVar6 >> 8 & 0xff;
    do
    {
      uVar6 = current_turn ^ uVar5;
      iVar2 = times_damaged[uVar6];
      if (iVar2 != 0)
      {
        iVar4 = count_deck(uVar6);
        if (iVar4 != 0)
        {
          new_default_test_definition(&test, 0xff, "Select a card to exile.");
          if (player == 1)
          {
            test.type = 1;
            test.type_flag = 1;
          }
          if (3 < iVar4)
          {
            iVar4 = 3;
          }
          test.create_minideck = (unsigned short)iVar4;
          test.no_shuffle = 1;
          pos = new_select_a_card(player, uVar6, 1, (unsigned int)(player == 0), 1, -1, &test);
          if (pos == -1)
          {
            new_default_test_definition(&test, 0xff, "Select a card to exile.");
            pos = new_select_a_card(player, uVar6, 1, 1, 1, -1, &test);
          }
          iVar1 = deck_ptr[uVar6][pos];
          rfg_card_in_deck(uVar6, pos);
          remove_card_from_rfg(uVar6, (unsigned int)cards_data[iVar1].id);
          create_may_play_card_from_exile_effect(player, card, uVar6,
                                                 (unsigned int)cards_data[iVar1].id,
                                                 MPCFE_FACE_DOWN | MPCFE_FOR_CMC);
          if (iVar4 + -1 != 0)
          {
            put_top_x_on_bottom_in_random_order(uVar6, iVar4 + -1);
          }
        }
        times_damaged[uVar6] = iVar2 + -1;
      }
      uVar5 = uVar5 + 1;
    } while (uVar5 != 2);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2621120; original cards/guilds_of_ravnica.c:3813. */
int card_trostani_discordant(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  check_legend_rule(player, card, event);
  boost_creature_type(player, card, event, ~SUBTYPE_INVALID, 1, 1, 0, BCT_CONTROLLER_ONLY);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x4986, &token);
    token.s_key_plus = 2;
    token.qty = '\x02';
    generate_token(&token);
  }
  if ((current_turn == player) && (result = eot_trigger(player, card, event), result != 0))
  {
    get_back_your_permanents(current_turn, -1, TYPE_CREATURE);
    get_back_your_permanents(current_turn ^ 1, -1, TYPE_CREATURE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2621250; original cards/guilds_of_ravnica.c:3860. */
int card_underrealm_lich(int player, int card, event_t event)
{
  int card_00;
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  test_definition_t this_test;
  if ((((trigger_condition == 0xcf) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      ((suppress_draw == 0 && (reason_for_trigger_controller == player))))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result = event_result | 2;
      goto LAB_02621275;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      iVar1 = count_deck(player);
      if (iVar1 < 3)
      {
        iVar1 = count_deck(player);
        if (iVar1 != 0)
        {
          default_test_definition(&this_test, 0xff);
          this_test.no_shuffle = 1;
          this_test.create_minideck = 3;
          new_global_tutor(player, player, 1, 1, 1, 1, &this_test);
          iVar1 = iVar1 + -1;
          if (0 < iVar1)
            goto LAB_0262139c;
        }
      }
      else
      {
        default_test_definition(&this_test, 0xff);
        this_test.no_shuffle = 1;
        this_test.create_minideck = 3;
        new_global_tutor(player, player, 1, 1, 1, 1, &this_test);
        iVar1 = 2;
      LAB_0262139c:
        mill(player, iVar1);
      }
      suppress_draw = 1;
      goto LAB_02621275;
    }
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    iVar1 = instance->parent_controller;
    instance = get_card_instance(player, card);
    card_00 = instance->parent_card;
    pump_ability_until_eot_2022(iVar1, card_00, iVar1, card_00, 0, 0, 0, 0x800);
    tap_card(iVar1, card_00);
    iVar3 = check_for_special_ability(iVar1, card_00, 0x800);
    if (iVar3 != 0)
    {
      ai_modifier = ai_modifier + -0x30;
    }
    iVar3 = is_this_dying(iVar1, card_00);
    if ((iVar3 != 0) && (iVar1 = check_for_special_ability(iVar1, card_00, 0x800), iVar1 == 0))
    {
      ai_modifier = ai_modifier + life[player] * 2 + -10;
    }
  }
LAB_02621275:
  iVar1 = generic_activated_ability(player, card, event, 0, 0, 0, 0, 0, 0, 0, 4, NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x2621b60; original cards/guilds_of_ravnica.c:4138. */
int card_vraska_golgari_queen_emblem(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;
  test_definition_t test;
  default_test_definition(&test, 2);
  check_damage_test(player, -1, event,
                    DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                        DDBM_MUST_BE_COMBAT_DAMAGE,
                    player, card, 0, &test);
  result = resolve_damage_trigger(player, card, event,
                                  DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                      DDBM_MUST_BE_COMBAT_DAMAGE,
                                  player, card);
  if (result != 0)
  {
    instance = get_card_instance(player, card);
    uVar3 = instance->targets[1].player;
    instance->targets[1].player = 0;
    uVar4 = uVar3 >> 8 & 0xff;
    for (uVar3 = uVar3 & 0xff; uVar3 != 0; uVar3 = uVar3 - 1)
    {
      lose_the_game_2022(0);
    }
    if (uVar4 != 0)
    {
      do
      {
        lose_the_game_2022(1);
        uVar4 = uVar4 - 1;
      } while (uVar4 != 0);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x261a000; original cards/guilds_of_ravnica.c:171. */
int card_arboretum_elemental(int player, int card, event_t event)
{
  card_ptr_t *pcVar1;
  card_instance_t *instance;
  int result;

  hexproof_2022(player, card, event);
  instance = get_card_instance(player, card);
  if (event == EVENT_MODIFY_COST)
  {
    result = get_id(player, card);
    pcVar1 = cards_ptr[result];
    result = has_convoked_mana_extended(
        player, card, EVENT_MODIFY_COST, (unsigned int)pcVar1->req_colorless,
        (unsigned int)pcVar1->req_black, (unsigned int)pcVar1->req_blue,
        (unsigned int)pcVar1->req_green, (unsigned int)pcVar1->req_red,
        (unsigned int)pcVar1->req_white);
    if (result != 2)
    {
    code_r0x021aa704:
      instance->info_slot = 0;
      return 0;
    }
    null_casting_cost(player, card);
    instance->info_slot = 1;
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    result = cast_spell_with_convoke(player, card, EVENT_CAST_SPELL);
    if (result != 0)
      goto code_r0x021aa704;
    spell_fizzled = 1;
  }
  return 0;
}

/* ManalinkEh 2022: 0x261d8b0; original cards/guilds_of_ravnica.c:1880. */
int card_impervious_greatwurm(int player, int card, event_t event)
{
  card_ptr_t *pcVar1;
  card_instance_t *instance;
  int result;

  indestructible_2022(player, card, event);
  instance = get_card_instance(player, card);
  if (event == EVENT_MODIFY_COST)
  {
    result = get_id(player, card);
    pcVar1 = cards_ptr[result];
    result = has_convoked_mana_extended(
        player, card, EVENT_MODIFY_COST, (unsigned int)pcVar1->req_colorless,
        (unsigned int)pcVar1->req_black, (unsigned int)pcVar1->req_blue,
        (unsigned int)pcVar1->req_green, (unsigned int)pcVar1->req_red,
        (unsigned int)pcVar1->req_white);
    if (result != 2)
    {
    code_r0x021aa704:
      instance->info_slot = 0;
      return 0;
    }
    null_casting_cost(player, card);
    instance->info_slot = 1;
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    result = cast_spell_with_convoke(player, card, EVENT_CAST_SPELL);
    if (result != 0)
      goto code_r0x021aa704;
    spell_fizzled = 1;
  }
  return 0;
}

/* ManalinkEh 2022: 0x261ddb0; original cards/guilds_of_ravnica.c:2037. */
int card_izoni_thousand_eyed(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x4952, &token);
    token.color_forced = '\n';
    result = count_graveyard_by_type(player, 2);
    token.qty = (unsigned char)result;
    generate_token(&token);
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_0261de29:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_0261de2e;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_0261de29;
  }
  gain_life(player, 1);
  draw_a_card(player);
LAB_0261de2e:
  result = generic_activated_ability(player, card, event, 0x40200, 0, 1, 0, 1, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x261e380; original cards/guilds_of_ravnica.c:2289. */
int card_march_of_the_multitudes(int player, int card, event_t event)
{
  card_ptr_t *pcVar1;
  card_instance_t *instance;
  int result;
  token_generation_t token;

  if (event == EVENT_MODIFY_COST)
  {
    result = get_id(player, card);
    pcVar1 = cards_ptr[result];
    result = has_convoked_mana_extended(
        player, card, EVENT_MODIFY_COST, (unsigned int)pcVar1->req_colorless,
        (unsigned int)pcVar1->req_black, (unsigned int)pcVar1->req_blue,
        (unsigned int)pcVar1->req_green, (unsigned int)pcVar1->req_red,
        (unsigned int)pcVar1->req_white);
    if (result == 2)
    {
      null_casting_cost(player, card);
      instance = get_card_instance(player, card);
      instance->targets[1].player = 1;
    }
    else
    {
      instance = get_card_instance(player, card);
      instance->targets[1].player = 0;
    }
  }
  else
  {
    if (event == EVENT_CAN_CAST)
    {
      get_card_instance(player, card);
      result = generic_spell(player, card, EVENT_CAN_CAST, 4, NULL, NULL, 0, NULL);
      return result;
    }
    if (event == EVENT_CAST_SPELL)
    {
      if ((((card == affected_card) && (player == affected_card_controller)) &&
           (instance = get_card_instance(player, card), card == affected_card)) &&
          (player == affected_card_controller))
      {
        result = check_special_flags2_2022(player, card, -0x80000000);
        if (result == 0)
        {
          result = played_for_free(player, card);
          if ((result == 0) && (result = is_token(player, card), result == 0))
          {
            if (instance->targets[1].player == 1)
            {
              result =
                  charge_convoked_mana_extended(player, card, EVENT_CAST_SPELL, 0, 0, 0, 1, 0, 2);
              if (result != 0)
              {
                x_value = charge_convoked_mana_extended_generic(player, card, EVENT_CAST_SPELL, -1,
                                                                0, 0, 0, 0, 0, 0);
                if (x_value < 0)
                {
                  x_value = x_value & 0x7fffffff;
                }
                instance->info_slot = x_value;
              }
            }
            else
            {
              instance->info_slot = x_value;
            }
          }
          else
          {
            instance->info_slot = 0;
          }
        }
        else
        {
          instance->info_slot = 0;
        }
      }
    }
    else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        x_value = 0;
        default_token_definition(player, card, 0x4986, &token);
        token.s_key_plus = 2;
        token.qty = (unsigned char)instance->info_slot;
        generate_token(&token);
        kill_card(player, card, KILL_DESTROY);
        return 0;
      }
    }
    else if (event == EVENT_CHANGE_TARGET)
    {
      get_card_instance(player, card);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x261e6e0; original cards/guilds_of_ravnica.c:2355. */
int card_mausoleum_secrets(int player, int card, event_t event)
{
  int result;
  const char *pcVar2;
  test_definition_t test;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = count_graveyard_by_type(player, 2);
      ai_modifier = ai_modifier + result * 0xc;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    result = count_graveyard_by_type(player, 2);
    default_test_definition(&test, 0xff);
    test.color = get_sleighted_color_test(player, card, 2);
    pcVar2 = get_sleighted_color_text(player, card, "%s", 1);
    scnprintf(test.message, 100, "Select a %s card with CMC %d or less.", pcVar2, result);
    test.cmc_flag = 3;
    test.cmc = result + 1;
    new_global_tutor(player, player, 1, 1, 0, 1, &test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int effect_nullhide_ferox(int player, int card, event_t event);
static int mnemonic_betrayal_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2619bf0; original cards/guilds_of_ravnica.c:69. */
int gnr_locket(int player, int card, event_t event)
{
  unsigned char bVar1;
  int iVar2;
  card_instance_t *instance;
  unsigned int uVar4;
  int iVar5;
  bool bVar6;
  int iStack_20;
  int iStack_1c;

  if ((event != EVENT_CAN_ACTIVATE) &&
      (((event == EVENT_ACTIVATE || (event == EVENT_RESOLVE_ACTIVATION)) ||
        (event == EVENT_CLEANUP))))
  {
    iVar2 = 1;
    instance = get_card_instance(player, card);
    iStack_20 = -1;
    iStack_1c = -1;
    bVar1 = instance->mana_color;
    do
    {
      uVar4 = color_to_color_test(iVar2);
      iVar5 = iStack_1c;
      if (((bVar1 & 0x3e & uVar4) != 0) && (iVar5 = iVar2, iStack_1c != -1))
      {
        iStack_20 = iVar2;
        iVar5 = iStack_1c;
      }
      iStack_1c = iVar5;
      iVar2 = iVar2 + 1;
    } while (iVar2 != 5);
    if (event != EVENT_ACTIVATE)
    {
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }
      if (instance->info_slot != 2)
      {
        return 0;
      }
      draw_cards(player, 2);
      return 0;
    }
    iVar2 = get_cost_mod_for_activated_abilities_2022(player, card, 0, 0, 0, 0, 0, 0);
    iVar5 = paying_mana();
    if (((iVar5 == 0) &&
         (iVar5 = has_mana_hybrid(player, 5, iStack_1c, iStack_20, iVar2), iVar5 != 0)) &&
        (iVar5 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 0, 0, 0, 0, 0, 0, 0,
                                           NULL, NULL),
         iVar5 != 0))
    {
      bVar6 = ai_is_speculating == 1;
      instance->info_slot = 2;
      if ((bVar6) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
      {
      LAB_02619e6c:
        instance->state = instance->state | 0x10;
        iVar2 = charge_mana_hybrid(player, card, 4, iStack_1c, iStack_20, iVar2);
        if (iVar2 != 0)
        {
          generic_activated_ability(player, card, EVENT_ACTIVATE, 8, 0, 0, 0, 0, 0, 0, 0, NULL,
                                    NULL);
        }
        if (spell_fizzled == 1)
        {
          untap_card_no_event(player, card);
        }
        tapped_for_mana_color = -1;
        return 0;
      }
      iVar5 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                          "Draw 2 cards", 1, 1, 0, 0, 0, 0);
      if (iVar5 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (instance->info_slot != 1)
      {
        if (instance->info_slot != 2)
        {
          return 0;
        }
        goto LAB_02619e6c;
      }
    }
    else
    {
      instance->info_slot = 1;
    }
    event = EVENT_ACTIVATE;
  }

  iVar2 = mana_producer(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x261b340; original cards/guilds_of_ravnica.c:868. */
int card_deafening_clarion(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    result = dialog_impl(player, card, EVENT_CAN_CAST, "\001\002", "Damage all creatures", 1, 5,
                         "Your creatures gain lifelink", 1, 5, "Both", 1, 7, 0, 0, 0, 0);
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
    uVar1 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "Damage all creatures", 1, 5,
                        "Your creatures gain lifelink", 1, 5, "Both", 1, 7, 0, 0, 0, 0);
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = basic_spell(player, card, EVENT_CAST_SPELL);
      return result;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET) &&
        (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }

    uVar1 = dialog_impl(player, card, event, "\001\002", "Damage all creatures", 1, 5,
                        "Your creatures gain lifelink", 1, 5, "Both", 1, 7, 0, 0, 0, 0);
  }
  if ((uVar1 & 1) != 0)
  {
    default_test_definition(&test, 2);
    new_damage_all(player, card, 2, 3, 1, &test);
  }
  if ((uVar1 & 2) != 0)
  {
    pump_subtype_until_eot_2022(player, card, player, -1, 0, 0, 0, 2);
  }
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

/* ManalinkEh 2022: 0x261bb10; original cards/guilds_of_ravnica.c:1058. */
int card_disinformation_campaign(int player, int card, event_t event)
{
  int result;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_a_card(player);
    discard(1 - player, 2, player);
  }
  else if (event == EVENT_SURVEIL)
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      bounce_permanent(player, card);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x261bce0; original cards/guilds_of_ravnica.c:1160. */
int card_emmara_soul_of_the_accord(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  check_legend_rule(player, card, event);
  if (((event == EVENT_TAP_CARD) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      default_token_definition(player, card, 0x4986, &token);
      token.s_key_plus = 2;
      generate_token(&token);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x261d0d0; original cards/guilds_of_ravnica.c:1623. */
int card_goblin_cratermaker(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t *td;
  const char *prompt;
  target_definition_t td1;
  target_definition_t td2;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    td = &td1;
    default_target_definition(player, card, td, 2);
    default_target_definition(player, card, &td2, 0x47);
    td2.illegal_type = 1;
    td2.illegal_color = 0x3e;
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xc, 1, 0, 0, 0, 0, 0, 0,
                                        td, NULL);
      if (iVar3 == 0)
      {
        iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xc, 1, 0, 0, 0, 0, 0,
                                          0, &td2, NULL);
        return iVar3;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xc, 1, 0, 0, 0, 0, 0, 0,
                                        &td2, NULL);
      iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xc, 1, 0, 0, 0, 0, 0, 0,
                                        td, NULL);
      iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "2 damage to target creature",
                          iVar1, 5, "Destroy a colorless nonland permanent", iVar3, 5, 0, 0, 0, 0);
      if (iVar3 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar3 == 1)
      {
        prompt = "TARGET_CREATURE";
        iVar3 = 0xc;
      }
      else
      {
        if (iVar3 != 2)
        {
          return 0;
        }
        prompt = "Select target colorless nonland permanent.";
        iVar3 = 0xe;
        td = &td2;
      }
      iVar3 = generic_activated_ability(player, card, EVENT_ACTIVATE, iVar3, 1, 0, 0, 0, 0, 0, 0,
                                        td, prompt);
      return iVar3;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td1, 2);
    default_target_definition(player, card, &td2, 0x47);
  }
  td2.illegal_color = 0x3e;
  td2.illegal_type = 1;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    if (instance->info_slot == 1)
    {
      iVar3 = valid_target(&td1);
      if (iVar3 != 0)
      {
        damage_target0(player, card, 2);
      }
    }
    else if ((instance->info_slot == 2) && (iVar3 = valid_target(&td2), iVar3 != 0))
    {
      kill_target0(player, card, KILL_DESTROY);
      return 0;
    }
  }

  return 0;
}

/* ManalinkEh 2022: 0x261d8f0; original cards/guilds_of_ravnica.c:1900. */
int card_inspiring_unicorn(int player, int card, event_t event)
{
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  pump_creatures_until_eot(player, card, player, 0, 1, 1, 0, 0, NULL);
  return 0;
}

/* ManalinkEh 2022: 0x2619800; original cards/guilds_of_ravnica.c:2547. */
static int mnemonic_betrayal_legacy(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  iVar1 = eot_trigger(player, card, event);
  if (iVar1 != 0)
  {

    iVar1 = active_cards_count[player] + -1;
    if (-1 < iVar1)
    {
      do
      {
        instance = in_play(player, iVar1);
        if (instance != NULL)
        {
          iVar3 = is_what(player, iVar1, 0x80);
          if ((iVar3 != 0) && (instance->targets[9].player == 0x4beb))
          {
            iVar3 = find_iid_in_rfg(1 - player, instance->targets[9].card);
            if (-1 < iVar3)
            {
              from_exile_to_graveyard(1 - player, iVar3);
              kill_card(player, iVar1, KILL_EXILE);
            }
          }
        }
        iVar1 = iVar1 + -1;
      } while (iVar1 != -1);
    }
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x261ec50; original cards/guilds_of_ravnica.c:2565. */
int card_mnemonic_betrayal(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  int iVar3;
  card_instance_t *instance;
  int iVar5;
  int *piVar6;
  int *piVar7;

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
      iVar2 = 1 - player;
      iVar3 = count_graveyard(iVar2);
      if ((iVar3 != 0) && (iVar3 = graveyard_has_shroud(iVar2), iVar3 == 0))
      {

        piVar1 = graveyard_ptr_mutable[iVar2];
        piVar7 = piVar1;
        do
        {
          iVar3 = *piVar7;
          if (iVar3 == -1)
            break;
          if ((cards_data[iVar3].type & 1) == 0)
          {

            iVar3 = create_may_play_card_from_exile_effect(
                player, card, iVar2, (unsigned int)cards_data[iVar3].id, MPCFE_FOR_CMC);
          }
          else
          {
            iVar3 = create_legacy_effect(player, card, empty);
          }
          piVar6 = piVar7 + 1;
          set_legacy_image(player, (unsigned int)cards_data[*piVar7].id, iVar3);
          instance = get_card_instance(player, iVar3);
          iVar5 = get_id(player, card);
          instance->targets[9].player = iVar5;
          instance = get_card_instance(player, iVar3);
          instance->targets[9].card = *piVar7;
          piVar7 = piVar6;
        } while (piVar6 != piVar1 + 500);
        rfg_whole_graveyard(iVar2);
        create_legacy_effect(player, card, mnemonic_betrayal_legacy);
      }
      kill_card(player, card, KILL_EXILE);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  iVar2 = basic_spell(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x2619460; original cards/guilds_of_ravnica.c:2734. */
static int effect_nullhide_ferox(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar2 = instance->targets[0].player;
      if (-1 < iVar2)
      {
        iVar2 = get_id(iVar2, instance->targets[0].card);
        if (iVar2 != 0x4bf6)
        {
          kill_card(player, card, KILL_EXILE);
        }
        instance = in_play(instance->targets[0].player, instance->targets[0].card);
        if (instance == NULL)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      get_card_instance(player, card);
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {

      instance = get_card_instance(player, card);
      iVar2 = instance->targets[0].player;
      card_00 = instance->targets[0].card;
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar3 = can_use_activated_abilities_2022(iVar2, card_00);
        if (iVar3 != 0)
        {

          iVar2 = get_cost_mod_for_activated_abilities_2022(iVar2, card_00, 2, 0, 0, 0, 0, 0);
          iVar2 = has_mana_multi(player, iVar2, 0, 0, 0, 0, 0);
          if (iVar2 != 0)
          {
            return 1;
          }
        }
      }
      else
      {
        if (event == EVENT_ACTIVATE)
        {

          iVar2 = get_cost_mod_for_activated_abilities_2022(iVar2, card_00, 2, 0, 0, 0, 0, 0);
          charge_mana_multi(player, iVar2, 0, 0, 0, 0, 0);
          return 0;
        }
        if (event == EVENT_RESOLVE_ACTIVATION)
        {
          humiliate_and_set_pt_abilities(player, card, iVar2, card_00, 4, NULL);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x261f0f0; original cards/guilds_of_ravnica.c:2774. */
int card_nullhide_ferox(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int iVar6;

  hexproof_2022(player, card, event);
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    iVar3 = is_humiliated_for_ability(player, card, AL_STATIC);
    if (((iVar3 == 0) && (affected_card_controller == player)) &&
        (iVar3 = is_what(player, affected_card, 3), iVar3 == 0))
    {
      infinite_casting_cost();
    }
    get_card_instance(player, card);
  }
  else
  {
    pcVar2 = get_card_instance(player, card);
    if (event == EVENT_STATIC_EFFECTS)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        bVar1 = false;
        iVar3 = 0;
        do
        {

          iVar5 = 0;
          if (-1 < active_cards_count[iVar3])
          {
            do
            {
              pcVar4 = in_play(iVar3, iVar5);

              if (((pcVar4 != NULL) && (iVar6 = is_what(iVar3, iVar5, 0x80), iVar6 != 0)) &&
                  ((pcVar4 = get_card_instance(iVar3, iVar5),
                    pcVar4->info_slot_fn == effect_nullhide_ferox &&
                        ((pcVar4->targets[0].player == player &&
                          (pcVar4->targets[0].card == card))))))
              {
                if (player == iVar3)
                {
                  kill_card(player, iVar5, KILL_EXILE);
                }
                else
                {
                  if (iVar3 == 1)
                    goto LAB_0261f174;
                  bVar1 = true;
                }
              }
              iVar5 = iVar5 + 1;
            } while (iVar5 <= active_cards_count[iVar3]);
          }
          iVar3 = iVar3 + 1;
        } while (iVar3 != 2);
        if (!bVar1)
        {

          iVar6 = 1 - player;
          iVar3 = add_card_to_hand(iVar6, pcVar2->internal_card_id);
          iVar5 = create_legacy_activate(iVar6, iVar3, effect_nullhide_ferox);
          pcVar2 = get_card_instance(iVar6, iVar5);
          pcVar2->targets[0].player = player;
          pcVar2->targets[0].card = card;
          pcVar2->number_of_targets = '\x01';
          obliterate_card(iVar6, iVar3);
        }
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {
      humiliate_and_set_pt_abilities(player, card, pcVar2->parent_controller, pcVar2->parent_card,
                                     4, NULL);
    }
  }
LAB_0261f174:
  iVar3 = is_humiliated_for_ability(player, card, AL_TRIG);
  if ((iVar3 != 0) && (iVar3 = eot_trigger(player, card, event), iVar3 != 0))
  {
    humiliate(player, card, player, card, 0);
  }
  iVar3 = generic_activated_ability(player, card, event, 0, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x26218b0; original cards/guilds_of_ravnica.c:4048. */
int card_vraska_golgari_queen(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((event - EVENT_RESOLVE_ACTIVATION < 2) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x47);
    td.special = 0x800;
    td.extra = 3;
    td.illegal_type = 1;
    iVar2 = count_counters(player, card, COUNTER_LOYALTY);
    iVar3 = can_target(&td);
    iVar2 = dialog_impl(player, card, event, "\001\002", "\001\017", "Sac to draw and gain life", 1,
                        5, 2, "Destroy target nonland", iVar3, ((3 < iVar2) - 1 & 0xfffffff2) + 0xf,
                        0xfffffffd, "Emblem", 1, 0x14, 0xfffffff7, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (iVar2 == 0)
      {
        return 0;
      }
    }
    else if (event == EVENT_ACTIVATE)
    {
      instance->number_of_targets = '\0';
      if (iVar2 == 2)
      {
        new_pick_target(&td, "Select target nonland permanent with converted mana cost 3 or less.",
                        0, 0x81);
      }
    }
    else if (iVar2 == 2)
    {
      iVar2 = valid_target(&td);
      if (iVar2 != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
    }
    else if (iVar2 == 3)
    {
      generate_token_by_id(player, card, 0x4c45);
    }
    else if (iVar2 == 1)
    {
      new_default_test_definition(&test, 0x47, "Select a permanent to sacrifice.");
      test.not_me = 1;
      iVar2 = new_sacrifice(player, card, player, SAC_DONE, &test);
      if (iVar2 != 0)
      {
        gain_life_2022(player, 1);
        draw_a_card(player);
      }
    }
  }
  iVar2 = planeswalker(player, card, event, 4);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x261ad20; original cards/guilds_of_ravnica.c:740. */
int card_crush_contraband(int player, int card, event_t event)
{
  int iVar1;
  unsigned int uVar2;
  card_instance_t *instance;
  uint32_t uVar4;
  uint32_t uVar5;
  int iVar6;
  bool bVar7;
  target_definition_t *ptStack_11c;
  target_definition_t *ptStack_118;
  unsigned int uStack_114;
  int iStack_110;
  target_definition_t td_a;
  target_definition_t td_e;
  target_definition_t td_opp_a;

  if (event == EVENT_CAN_CAST)
  {
  LAB_0261aec1:
    default_target_definition(player, card, &td_a, 0x40);
    default_target_definition(player, card, &td_e, 4);
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
      goto LAB_0261aec1;
    }
    if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      goto LAB_0261aec1;
    }

    default_target_definition(player, card, &td_a, 0x40);
    default_target_definition(player, card, &td_e, 4);
    if (event == EVENT_RESOLVE_SPELL)
    {
      uVar4 = 3;
      iVar6 = 0;
      iStack_110 = 0;
      uStack_114 = 0;
      uVar5 = 1;
      iVar1 = 1;
      goto LAB_0261add0;
    }
  }
  ptStack_118 = &td_e;
  ptStack_11c = &td_a;
  iStack_110 = can_target(ptStack_11c);
  iVar6 = can_target(ptStack_118);
  if ((iStack_110 == 0) || (player != 1))
  {
    bVar7 = false;
  }
  else
  {
    default_target_definition(1, card, &td_opp_a, 0x40);
    td_opp_a.allowed_controller = 0;
    iVar1 = can_target(&td_opp_a);
    bVar7 = iVar1 != 0;
  }
  uStack_114 = (unsigned int)(iVar6 != 0 && iStack_110 != 0);
  if ((iVar6 != 0) && (player == 1))
  {
    default_target_definition(1, card, &td_opp_a, 4);
    td_opp_a.allowed_controller = 0;
    iVar1 = can_target(&td_opp_a);
    if (iVar1 != 0)
    {
      if (bVar7)
      {
        uVar4 = 0x19;
        uVar5 = 5;
        iVar1 = 5;
        goto LAB_0261add0;
      }
      if (iVar1 != 0)
      {
        uVar4 = 3;
        uVar5 = 5;
        iVar1 = 1;
        goto LAB_0261add0;
      }
    }
  }
  uVar4 = 3;
  uVar5 = 1;
  iVar1 = (-(unsigned int)!bVar7 & 0xfffffffc) + 5;
LAB_0261add0:
  ptStack_118 = &td_e;
  ptStack_11c = &td_a;
  uVar2 = dialog_impl(player, card, event, "\001\002", "Exile artifact", iStack_110, iVar1,
                      "Exile enchantment", iVar6, uVar5, "Both", uStack_114, uVar4, 0, 0, 0, 0);
  if (event != EVENT_CAN_CAST)
  {
    if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
        (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      instance->number_of_targets = '\0';
      if ((uVar2 & 1) != 0)
      {
        new_pick_target(ptStack_11c, "TARGET_ARTIFACT", 0, 1);
      }
      if ((uVar2 & 2) != 0)
      {
        new_pick_target(ptStack_118, "TARGET_ENCHANTMENT", uVar2 & 1, 1);
      }
    }
    else
    {

      instance = get_card_instance(player, card);
      if (((uVar2 & 1) != 0) && (iVar1 = validate_target(player, card, ptStack_11c, 0), iVar1 != 0))
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
      }
      if ((uVar2 & 2) != 0)
      {
        uVar2 = uVar2 & 1;
        iVar1 = validate_target(player, card, ptStack_118, uVar2);
        if (iVar1 != 0)
        {
          kill_card(instance->targets[uVar2].player, instance->targets[uVar2].card, KILL_EXILE);
        }
      }
      kill_card(player, card, KILL_DESTROY);
    }
    return 0;
  }
  return uVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static void mentor(int player, int card, event_t event);

/* ManalinkEh 2022: 0x26196c0; original cards/guilds_of_ravnica.c:133. */
static void mentor(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  char buffer[75];
  target_definition_t td;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    default_target_definition(player, card, &td, 2);
    td.required_state = 2;
    result = get_power(player, card);
    td.power_requirement = result - 1U | 0x2000;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    instance->number_of_targets = '\0';
    result = get_power(player, card);
    scnprintf(buffer, 0x4b, " Choose target attacking creature with power %d or less.",
              result + -1);
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, buffer, 0, 0x80);
      if (result != 0)
      {
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 1);
      }
    }
  }
  return;
}

/* ManalinkEh 2022: 0x261a3a0; original cards/guilds_of_ravnica.c:295. */
int card_barging_sergeant(int player, int card, event_t event)
{
  haste(player, card, event);
  mentor(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x261a630; original cards/guilds_of_ravnica.c:377. */
int card_blade_instructor(int player, int card, event_t event)
{
  mentor(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x261a650; original cards/guilds_of_ravnica.c:405. */
int card_boros_challenger(int player, int card, event_t event)
{
  int result;

  mentor(player, card, event);
  result = generic_shade(player, card, event, 0, 2, 0, 0, 0, 1, 1, 1, 1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x261c0e0; original cards/guilds_of_ravnica.c:1267. */
int card_experimental_frenzy(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  uint32_t uVar4;
  unsigned int uVar5;
  uint32_t uVar6;

  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    if (((affected_card_controller == player) &&
         (instance = in_hand(player, affected_card), instance != NULL)) &&
        (iVar1 = check_state(affected_card_controller, affected_card, 0x20), iVar1 == 0))
    {
      infinite_casting_cost();
    }
  LAB_0261c3e8:
    iVar1 = global_enchantment(player, card, event);
    return iVar1;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {

    iVar1 = *deck_ptr[player];
    if (iVar1 != -1)
    {
      iVar2 = has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, iVar1);
      if (((iVar2 != 0) &&
           (iVar1 = can_legally_play_iid_now(player, iVar1, EVENT_CAN_ACTIVATE), iVar1 != 0)) &&
          (ai_is_speculating == 0))
      {
        return iVar1;
      }
      if (*deck_ptr[player] != -1)
        goto LAB_0261c34c;
    }
    iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    if (iVar1 == 0)
    {
      return 0;
    }
  LAB_0261c34c:
    instance = has_activation_on_stack(player, card);
    return (unsigned int)(instance == NULL);
  }
  if (event != EVENT_ACTIVATE)
  {
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      if (instance->info_slot != 3)
      {
        return 0;
      }
      instance = get_card_instance(player, card);
      iVar1 = instance->parent_card;
      instance = get_card_instance(player, card);
      kill_card(instance->parent_controller, iVar1, KILL_DESTROY);
      return 0;
    }
    if (event == EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_0261c3e8;
  }

  iVar1 = *deck_ptr[player];
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    uVar6 = 0;
    if (iVar1 != -1)
      goto LAB_0261c14f;
  LAB_0261c16b:
    iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 1, 0, 0,
                                      NULL, NULL);
    uVar5 = 0;
  }
  else
  {
    uVar6 = 1;
    if (iVar1 == -1)
    {
      uVar6 = 0;
      goto LAB_0261c16b;
    }
  LAB_0261c14f:
    iVar1 = can_legally_play_iid_now(player, iVar1, EVENT_ACTIVATE);
    if ((iVar1 == 0) ||
        (iVar1 = has_mana_to_cast_iid(player, EVENT_ACTIVATE, *deck_ptr[player]), iVar1 == 0))
      goto LAB_0261c16b;
    instance = has_activation_on_stack(player, card);
    uVar5 = (unsigned int)(instance == NULL);
    iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 1, 0, 0,
                                      NULL, NULL);
    if (instance == NULL)
    {
      uVar5 = 1;
      uVar4 = 0xfffffff6;
      goto LAB_0261c1d6;
    }
  }
  uVar4 = 5;
LAB_0261c1d6:
  iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016",
                      "Check top card of deck", uVar6, 0, "Play top card", uVar5, 10,
                      "Destroy Experimental Frenzy", iVar1, uVar4, 0, 0, 0, 0);
  if (iVar1 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (iVar1 == 1)
  {
    show_deck(player, deck_ptr[player], 1, "Here\'s the top card of your deck", 0,
              ENGINE_DONE_LABEL);
    spell_fizzled = 1;
  }
  else if (iVar1 == 2)
  {
    iVar1 = charge_mana_from_id(player, -1, EVENT_ACTIVATE,
                                (unsigned int)cards_data[*deck_ptr[player]].id);
    if (iVar1 != 0)
    {
      play_card_in_deck_for_free(player, player, 0);
      cant_be_responded_to = 1;
    }
  }
  else if (iVar1 == 3)
  {
    iVar1 =
        generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 3, 0, 0, 0, 1, 0, 0, NULL, NULL);
    return iVar1;
  }
  return 0;
}

/* ManalinkEh 2022: 0x261dee0; original cards/guilds_of_ravnica.c:2086. */
int card_knight_of_autumn(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 0x44);
    td.allow_cancel = 0;
    result = can_target(&td);
    result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Add two +1/+1 counters", 1, 5,
                         "Destroy an artifact or enchantment", result, 8, "Gain 4 life", 1, 3, 0, 0,
                         0, 0);
    if (result == 2)
    {
      result = can_target(&td);
      if (result != 0)
      {
        result = pick_target(&td, "DISENCHANT");
        if (result != 0)
        {
          action_on_target(player, card, 0, ACT_KILL_DESTROY);
        }
      }
    }
    else if (result == 3)
    {
      gain_life(player, 4);
    }
    else if (result == 1)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 2);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x261f3f0; original cards/guilds_of_ravnica.c:2868. */
int card_parhelion_patrol(int player, int card, event_t event)
{
  vigilance_2022(player, card, event);
  mentor(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x26217e0; original cards/guilds_of_ravnica.c:3990. */
int card_venerated_loxodon(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    result = 0;
    do
    {
      instance = get_card_instance(player, card);
      if (-1 < instance->targets[result].card)
      {
        instance = get_card_instance(player, card);
        add_counters_from_source(player, card, player, instance->targets[result].card,
                                 COUNTER_P1_P1, 1);
        instance = get_card_instance(player, card);
        instance->targets[result].card = -1;
      }
      result = result + 1;
    } while (result != 0x12);
  }
  result = generic_spell_with_convoke(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int midnight_reaper_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x261a040; original cards/guilds_of_ravnica.c:200. */
int card_assassins_trophy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
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
        default_target_definition(player, card, &td, 0x47);
        td.allowed_controller = 1 - player;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {

            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            result = instance->targets[0].player;
            if ((result == 1) ||
                (result = do_dialog(result, player, card, -1, -1, " Search a basic land\n Pass", 0),
                 result == 0))
            {
              new_default_test_definition(&this_test, 1, "Select a basic land card.");
              this_test.subtype = 0x2000;
              result = instance->targets[0].player;
              new_global_tutor(result, result, 1, 4, 0, 5, &this_test);
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0261a0c0;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.allowed_controller = 1 - player;
  get_card_instance(player, card);
LAB_0261a0c0:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target permanent an opponent controls.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x261ac90; original cards/guilds_of_ravnica.c:719. */
int card_creeping_chill(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAN_CAST)
  {
    result = basic_spell(player, card, EVENT_CAN_CAST);
    return result;
  }
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }
  damage_player(1 - player, 3, player, card);
  gain_life_2022(player, 3);
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

/* ManalinkEh 2022: 0x261b1b0; original cards/guilds_of_ravnica.c:828. */
int card_dawn_of_hope(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  result = duh_mode(player);
  result = trigger_gain_life_2022(player, card, event, player, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (result != 0)
  {
    result = has_mana(player, 7, 2);
    if (result != 0)
    {
      result =
          charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player, COLOR_ANY, 2);
      if (result != 0)
      {
        draw_cards(player, 1);
      }
    }
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = global_enchantment(player, card, event);
      return result;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      default_token_definition(player, card, 0x4986, &token);
      token.s_key_plus = 2;
      generate_token(&token);
    }
  }
  result = generic_activated_ability(player, card, event, 0, 3, 0, 0, 0, 0, 1, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x261dc70; original cards/guilds_of_ravnica.c:2009. */
int card_ionize(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  counterspell_target_definition(player, card, &td, 0xff);
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {

      instance = get_card_instance(player, card);
      result = counterspell_validate(player, card, &td, 0);
      if (result != 0)
      {
        real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
      }
      damage_player(instance->targets[0].player, 2, player, card);
      instance->number_of_targets = '\0';
      kill_card(player, card, KILL_DESTROY);
    }
    result = counterspell(player, card, event, NULL, 0);
    return result;
  }
  td.illegal_abilities = 0;
  result = counterspell(player, card, EVENT_CAN_CAST, NULL, 0);
  return result;
}

/* ManalinkEh 2022: 0x2619180; original cards/guilds_of_ravnica.c:2414. */
static int midnight_reaper_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  card_instance_t *pcVar3;
  int result;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x4be9))
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
  LAB_026191b7:
    uVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto joined_r0x026191e0;
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
      goto LAB_026191b7;

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
      default_test_definition(&test, 2);
      test.type_flag = 3;
      count_for_gfp_ability(player, card, 0x77, player, 0, &test);
    }
    uVar2 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {

    for (uVar2 = uVar2 & 0xff; uVar2 != 0; uVar2 = uVar2 - 1)
    {
      damage_player(player, 1, player, card);
    }
    draw_cards(player, 0);
    kill_card(player, card, KILL_EXILE);
  }
joined_r0x026191e0:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x261e950; original cards/guilds_of_ravnica.c:2471. */
int card_midnight_reaper(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar2 = 0;
        do
        {
          instance = in_play(player, iVar2);

          if ((((instance != NULL) && (iVar3 = is_what(player, iVar2, 0x80), iVar3 != 0)) &&
               (instance = get_card_instance(player, iVar2),
                instance->info_slot_fn == midnight_reaper_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_0261e975;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, midnight_reaper_effect, player, card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    default_test_definition(&test, 2);
    test.type_flag = 3;
    count_for_gfp_ability(player, card, 0x77, player, 0, &test);
  }
LAB_0261e975:
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (uVar4 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar4 != 0))
  {

    damage_player(player, uVar4 & 0xff, player, card);
    draw_cards(player, uVar4 & 0xff);
  }
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x2620390; original cards/guilds_of_ravnica.c:3352. */
int card_risk_factor(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t td;

  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    iVar3 = jumpstart(player, card, event, 0x4c12);
    return iVar3;
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 0);
        iVar3 = 1 - player;
        td.zone = 0x1000;
        td.allowed_controller = iVar3;
        if (event == EVENT_RESOLVE_SPELL)
        {
          iVar1 = valid_target(&td);
          if (iVar1 != 0)
          {

            instance = get_card_instance(player, card);
            iVar3 =
                do_dialog(instance->targets[0].player, player, card, -1, -1,
                          " Take 4 damage\n Opponent draws 3", (unsigned int)(life[iVar3] < 0xb));
            if (iVar3 == 0)
            {
              damage_player(instance->targets[0].player, 4, player, card);
            }
            else if (iVar3 == 1)
            {
              draw_cards(player, 3);
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02620415;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0);
  td.allowed_controller = 1 - player;
  td.zone = 0x1000;
LAB_02620415:
  iVar3 = generic_spell(player, card, event, 1, &td, "TARGET_OPPONENT", 1, NULL);
  return iVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x261a5c0; original cards/guilds_of_ravnica.c:360. */
int card_beast_whisperer(int player, int card, event_t event)
{
  sp_ability_trigger(player, event, 2, TYPE_CREATURE, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  draw_cards(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x261bc60; original cards/guilds_of_ravnica.c:1137. */
int card_electrostatic_field(int player, int card, event_t event)
{
  sp_ability_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  damage_player(1 - player, 1, player, card);
  return 0;
}

/* ManalinkEh 2022: 0x261c980; original cards/guilds_of_ravnica.c:1414. */
int card_fireminds_research(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t *ptStack_68;
  target_definition_t td;

  sp_ability_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 1, 0, 1, 0, 0, 0,
                                          0x17b0200, NULL, NULL);
        if (iVar2 == 0)
        {
          iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 1, 0, 0, 0, 1, 0,
                                            0x17b0500, &td, "TARGET_ANY");
          return iVar2;
        }
        return 1;
      }
      if (event == EVENT_ACTIVATE)
      {

        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 1, 0, 1, 0, 0, 0,
                                          0x17b0200, NULL, NULL);
        iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 1, 0, 0, 0, 1, 0,
                                          0x17b0500, &td, "TARGET_ANY");
        iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Draw a card", iVar2, 5,
                            "Deal 5 damage", iVar3, 10, 0, 0, 0, 0);
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
          iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 1, 0, 0, 0, 1, 0,
                                            0x17b0500, &td, "TARGET_ANY");
          return iVar2;
        }
        iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 1, 0, 1, 0, 0, 0,
                                          0x17b0200, NULL, NULL);
        return iVar2;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_0261cbb4;
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      instance = get_card_instance(player, card);
    }
    ptStack_68 = &td;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      iVar2 = instance->info_slot;
      if (iVar2 == 1)
      {
        draw_a_card(player);
        iVar2 = instance->info_slot;
      }
      if ((iVar2 == 2) && (iVar2 = valid_target(ptStack_68), iVar2 != 0))
      {
        damage_target0(player, card, 5);
      }
    }
    return 0;
  }
  add_counters_from_source(player, card, player, card, COUNTER_CHARGE, 1);
LAB_0261cbb4:
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x261ee30; original cards/guilds_of_ravnica.c:2620. */
int card_murmuring_mystic(int player, int card, event_t event)
{
  sp_ability_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  generate_token_by_id_2022(player, card, 0x4b5e);
  return 0;
}

/* ManalinkEh 2022: 0x261eee0; original cards/guilds_of_ravnica.c:2679. */
int card_niv_mizzet_parun(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  check_legend_rule(player, card, event);
  cannot_be_countered(player, card, event);
  result = card_drawn_trigger(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {

    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    instance->number_of_targets = '\0';
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_ANY");
      if (result != 0)
      {
        damage_target0(player, card, 1);
      }
    }
  }
  sp_ability_trigger(2, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  draw_a_card(player);
  return 0;
}

/* ManalinkEh 2022: 0x2620070; original cards/guilds_of_ravnica.c:3259. */
int card_ral_izzet_viceroy_emblem(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  sp_ability_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
  td.allow_cancel = 0;
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "TARGET_ANY", 0, 0);
    if (result != 0)
    {
      damage_target0(player, card, 4);
      draw_cards(player, 2);
    }
  }
  instance = get_card_instance(player, card);
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x2620680; original cards/guilds_of_ravnica.c:3436. */
int card_runaway_steam_kin(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if ((((trigger_condition == 0xd3) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (result = count_counters(player, card, COUNTER_P1_P1), result < 3))
  {
    default_test_definition(&test, 0xff);
    test.color = get_sleighted_color_test(player, card, 0x10);
    sp_ability_trigger(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_SPA)
  {
    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  }
  else
  {
    if (event == EVENT_COUNT_MANA)
    {
      if (card != affected_card)
      {
        return 0;
      }
      if (player != affected_card_controller)
      {
        return 0;
      }
      result = can_produce_mana_2022(player, card);
      if (result == 0)
      {
        return 0;
      }
      result = count_counters(player, card, COUNTER_P1_P1);
      if (result < 3)
      {
        return 0;
      }
      result = count_counters(player, card, COUNTER_P1_P1);
      declare_mana_available(player, 4, (result / 3) * 3);
      return 0;
    }
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_produce_mana_2022(player, card);
      if (result == 0)
      {
        return 0;
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 0, 0,
                                         0x1000300, NULL, NULL);
      return (unsigned int)(result != 0);
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 0, 0, 0, 0, 0, 0, 0x1000300, NULL,
                                NULL);
      tapped_for_mana_color = -2;
      return 0;
    }
  }
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  produce_mana(player, 4, 3);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x261a3e0; original cards/guilds_of_ravnica.c:324. */
int card_beacon_bolt(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  target_definition_t td;

  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    iVar1 = jumpstart(player, card, event, 0x4b5b);
    return iVar1;
  }
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    iVar1 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_CREATURE", 1, NULL);
    return iVar1;
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
  }
  else
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 2);
      if (event == EVENT_RESOLVE_SPELL)
      {

        iVar1 = count_rfg_by_type(player, 0x38);
        iVar2 = count_graveyard_by_type(player, 0x38);
        damage_target0(player, card, iVar2 + iVar1);
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_0261a449;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 2);
LAB_0261a449:
  iVar1 = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x261abf0; original cards/guilds_of_ravnica.c:699. */
int card_crackling_drake(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
    {
      iVar1 = count_rfg_by_type(player, 0x38);
      iVar2 = count_graveyard_by_type(player, 0x38);
      set_characteristic_pt(iVar1 + iVar2);
    }
    return 0;
  }
  draw_a_card(player);
  return 0;
}

/* ManalinkEh 2022: 0x261fdf0; original cards/guilds_of_ravnica.c:3179. */
int card_ral_izzet_viceroy(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  bool bVar5;
  target_definition_t td;

  check_legend_rule(player, card, event);
  bVar5 = event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE;
  if ((event == EVENT_RESOLVE_ACTIVATION) || (bVar5))
  {

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    iVar1 = count_counters(player, card, COUNTER_LOYALTY);
    iVar3 = count_counters(player, card, COUNTER_LOYALTY);
    iVar4 = can_target(&td);
    iVar1 = dialog_impl(player, card, event, "\001\002", "\001\017",
                        "Check the first two cards of your deck", 1, 5, 1, "Damage target creature",
                        iVar4, ((3 < iVar3) - 1 & 0xfffffff2) + 0xf, 0xfffffffd, "Emblem", 1,
                        ((8 < iVar1) - 1 & 0xffffffed) + 0x14, 0xfffffff8, 0, 0, 0, 0);
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
      if (iVar1 == 2)
      {
        new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      }
    }
    else if (iVar1 == 2)
    {
      iVar1 = valid_target(&td);
      if (iVar1 != 0)
      {

        iVar1 = count_rfg_by_type(player, 0x38);
        iVar3 = count_graveyard_by_type(player, 0x38);
        damage_target0(player, card, iVar3 + iVar1);
      }
    }
    else if (iVar1 == 3)
    {
      generate_token_by_id_2022(player, card, 0x4c0c);
    }
    else if (iVar1 == 1)
    {
      select_one_and_mill_the_rest(player, player, 2, 0xff);
    }
  }
  iVar1 = planeswalker(player, card, event, 5);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int hybrid_split_card(int player, int card, event_t event, int card_id_first_half,
                      int can_play_first_half, int priority_first_half, int id_second_half,
                      int can_play_second_half, int priority_second_half);

/* ManalinkEh 2022: 0x2619f70; original cards/guilds_of_ravnica.c:157. */
int hybrid_split_card(int player, int card, event_t event, int card_id_first_half,
                      int can_play_first_half, int priority_first_half, int id_second_half,
                      int can_play_second_half, int priority_second_half)
{
  int result;

  modify_cost_for_hybrid_spells(player, card, event, 0);
  result = generic_split_card_2022(player, card, event, card_id_first_half, can_play_first_half,
                                   priority_first_half, id_second_half, can_play_second_half,
                                   priority_second_half);
  return result;
}

/* ManalinkEh 2022: 0x261bd70; original cards/guilds_of_ravnica.c:1215. */
int card_expansion_explosion(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int result;
  int can_play_first_half;
  target_definition_t td_spell;
  target_definition_t td_any;
  target_definition_t td_player;

  counterspell_target_definition(player, card, &td_spell, 0x18);
  td_spell.special = td_spell.special | 0x800;
  td_spell.extra = 4;
  default_target_definition(player, card, &td_any, 0x102);
  td_any.zone = 0x1200;
  default_target_definition(player, card, &td_player, 0);
  td_player.zone = 0x1000;
  if (event == EVENT_PLAY_FIRST_HALF)
  {
    result = twincast(player, card, 0x6c, &td_spell, NULL);
    return result;
  }
  if (event == EVENT_PLAY_SECOND_HALF)
  {
    new_pick_target(&td_any, "Select target for damage", 0, 0x81);
    new_pick_target(&td_player, "Select target player to draw cards", 1, 0x81);
    result = generic_spell(player, card, EVENT_CAST_SPELL, 4, NULL, NULL, 0, NULL);
    return result;
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_0261be7f;
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
            twincast(player, card, 0x71, &td_spell, NULL);
            uVar2 = instance->info_slot;
          }
          if (((uVar2 & 2) != 0) &&
              ((result = validate_target(player, card, &td_any, 0),
                result != 0 ||
                    (result = validate_target(player, card, &td_player, 1), result != 0))))
          {
            damage_target0(player, card, x_value);
            draw_cards(instance->targets[1].player, x_value);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0261be7f;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_0261be7f;
    }
  }
  get_card_instance(player, card);
LAB_0261be7f:
  result = can_target(&td_any);
  can_play_first_half = counterspell(player, card, EVENT_CAN_CAST, &td_spell, 0);
  result =
      hybrid_split_card(player, card, event, 0x4b9b, can_play_first_half, 5, 0x4b9e, result, 8);
  return result;
}

/* ManalinkEh 2022: 0x261c660; original cards/guilds_of_ravnica.c:1358. */
int card_find_finality(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int result;
  target_definition_t td;
  test_definition_t test;

  new_default_test_definition(&test, 2, "Select up to two target creature cards.");
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = player;
  td.illegal_abilities = 0;
  td.allow_cancel = 2;
  if (event == EVENT_PLAY_FIRST_HALF)
  {
    result = spell_return_up_to_n_target_cards_from_your_graveyard_to_your_hand(
        player, card, EVENT_CAST_SPELL, 2, &test, 0);
    return result;
  }
  if (event == EVENT_PLAY_SECOND_HALF)
  {
    new_pick_target(&td, "SELECT_A_CREATURE_YOU_CONTROL", 0, 0);
    return 0;
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_0261c744;
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
            spell_return_up_to_n_target_cards_from_your_graveyard_to_your_hand(
                player, card, EVENT_RESOLVE_SPELL, 2, &test, 0);
            uVar2 = instance->info_slot;
          }
          if ((uVar2 & 2) != 0)
          {
            result = valid_target(&td);
            if (result != 0)
            {
              add_counters_from_source(player, card, instance->targets[0].player,
                                       instance->targets[0].card, COUNTER_P1_P1, 2);
            }

            pump_subtype_until_eot_2022(player, card, current_turn, -1, -4, -4, 0, 0);
            pump_subtype_until_eot_2022(player, card, current_turn ^ 1, -1, -4, -4, 0, 0);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0261c744;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_0261c744;
    }
  }
  get_card_instance(player, card);
LAB_0261c744:
  result = spell_return_up_to_n_target_cards_from_your_graveyard_to_your_hand(
      player, card, EVENT_CAN_CAST, 2, &test, 0);
  result = hybrid_split_card(player, card, event, 0x4ba1, result, 10, 0x4ba0, 1, 8);
  return result;
}

/* ManalinkEh 2022: 0x261d990; original cards/guilds_of_ravnica.c:1961. */
int card_invert_invent(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  target_definition_t td;
  test_definition_t test_instant;
  test_definition_t test_sorcery;

  default_target_definition(player, card, &td, 2);
  td.allow_cancel = 3;
  new_default_test_definition(&test_instant, 0x10, "Select an instant card.");
  test_instant.no_shuffle = 1;
  new_default_test_definition(&test_sorcery, 8, "Select a sorcery card.");
  if (event == EVENT_PLAY_FIRST_HALF)
  {
    iVar3 = generic_spell(player, card, EVENT_CAST_SPELL, 0x20, &td, "TARGET_CREATURE", 2, NULL);
    return iVar3;
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_0261da82;
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

            pcVar4 = get_card_instance(player, card);
            if (pcVar4->number_of_targets != '\0')
            {
              iVar3 = 0;
              do
              {
                iVar5 = validate_target(player, card, &td, iVar3);
                if (iVar5 != 0)
                {
                  switch_power_and_toughness_until_eot(player, card, pcVar1->targets[iVar3].player,
                                                       pcVar1->targets[iVar3].card);
                }
                iVar3 = iVar3 + 1;
              } while (iVar3 < (int)(unsigned int)pcVar4->number_of_targets);
            }
            uVar2 = pcVar1->info_slot;
          }
          if ((uVar2 & 2) != 0)
          {
            new_global_tutor(player, player, 1, 1, 0, 1, &test_instant);
            new_global_tutor(player, player, 1, 1, 0, 1, &test_sorcery);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0261da82;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_0261da82;
    }
  }
  get_card_instance(player, card);
LAB_0261da82:
  iVar3 = can_target(&td);
  iVar3 = hybrid_split_card(player, card, event, 0x4bcd, iVar3, 8, 0x4bcc, 1, 10);
  return iVar3;
}

/* ManalinkEh 2022: 0x2620160; original cards/guilds_of_ravnica.c:3300. */
int card_response_resurgence(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int result;
  int can_play_first_half;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  td.required_state = 0x20;
  if (event == EVENT_PLAY_FIRST_HALF)
  {
    result = generic_spell(player, card, EVENT_CAST_SPELL, 1, &td,
                           "TARGET_ATTACKING_BLOCKING_CREATURE", 1, NULL);
    return result;
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_02620203;
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
              damage_target0(player, card, 5);
            }
            uVar2 = instance->info_slot;
          }
          if ((uVar2 & 2) != 0)
          {
            pump_subtype_until_eot_2022(player, card, player, -1, 0, 0, 0x100, 8);
            extra_combat_effect(player, card, EVENT_RESOLVE_SPELL, 0, NULL, 1);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02620203;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_02620203;
    }
  }
  get_card_instance(player, card);
LAB_02620203:
  result = can_sorcery_be_played(player, event);
  can_play_first_half = can_target(&td);
  result =
      hybrid_split_card(player, card, event, 0x4c0e, can_play_first_half, 5, 0x4c10, result, 8);
  return result;
}

/* ManalinkEh 2022: 0x26198e0; original cards/guilds_of_ravnica.c:4. */
void surveil(int player, int number)
{
  int to_look = count_deck(player);
  if (number < to_look)
    to_look = number;
  if (to_look != 0)
  {
    int added[to_look];
    int ac = 0;
    do
    {
      test_definition_t this_test;
      new_default_test_definition(&this_test, TYPE_ANY, "Select a card to put into your graveyard");
      this_test.create_minideck = (unsigned short)to_look;
      int selected =
          new_select_a_card(player, player, TUTOR_FROM_DECK, 0, AI_MIN_VALUE, -1, &this_test);
      if (selected == -1)
        break;
      added[ac] = deck_ptr[player][selected];
      remove_card_from_deck(player, selected);
      ++ac;
      --to_look;
    } while (to_look != 0);
    if (ac != 0)
    {
      for (int i = 0; i < ac; ++i)
      {
        int card_added = add_card_to_hand(player, added[i]);
        put_on_top_of_deck(player, card_added);
      }
      mill(player, ac);
    }
    if (to_look != 0)
      rearrange_top_x(player, player, to_look);
  }
  if (number > 0)
    dispatch_event(player, -1, EVENT_SURVEIL);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x261b800; original cards/guilds_of_ravnica.c:991. */
int card_discovery_dispersal(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int iVar5;
  int player_00;
  int iStack_d4;
  int iStack_d0;
  target_definition_t td;
  char msg[100];

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_0261ba38;
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
            surveil(player, 2);
            draw_a_card(player);
            uVar2 = pcVar1->info_slot;
          }
          if ((uVar2 & 2) != 0)
          {

            player_00 = 1 - player;
            iStack_d4 = 1;
            pcVar1->targets[0].card = -1;
            pcVar1->targets[0].player = -1;
            iStack_d0 = -1;
            iVar5 = active_cards_count[player_00] + -1;
            if (-1 < iVar5)
            {
              do
              {
                while ((pcVar3 = in_play(player_00, iVar5),
                        pcVar3 == NULL || (iVar4 = is_what(player_00, iVar5, 2), iVar4 == 0)))
                {
                LAB_0261b8a8:
                  iVar5 = iVar5 + -1;
                  if (iVar5 == -1)
                    goto LAB_0261b920;
                }
                iVar4 = get_cmc(player_00, iVar5);
                if (iVar4 <= iStack_d0)
                {
                  iVar4 = get_cmc(player_00, iVar5);
                  iStack_d4 = iStack_d4 + (unsigned int)(iVar4 == iStack_d0);
                  goto LAB_0261b8a8;
                }
                pcVar1->targets[0].card = iVar5;
                pcVar1->targets[0].player = player_00;
                iStack_d0 = get_cmc(player_00, iVar5);
                iVar5 = iVar5 + -1;
                iStack_d4 = 1;
              } while (iVar5 != -1);
            LAB_0261b920:
              if (1 < iStack_d4)
              {
                default_target_definition(player, card, &td, 2);
                td.illegal_abilities = 0;
                td.allow_cancel = 0;
                td.extra = iStack_d0;
                td.special = 0x1000000;
                td.who_chooses = player_00;
                td.allowed_controller = player_00;
                td.preferred_controller = player_00;
                scnprintf(msg, 100, "Select a creature you control with CMC %d.", iStack_d0);
                pcVar1->number_of_targets = '\0';
                new_pick_target(&td, msg, 0, 0x80);
              }
              iVar5 = pcVar1->targets[0].player;
              if ((iVar5 != -1) && (iVar4 = pcVar1->targets[0].card, iVar4 != -1))
              {
                bounce_permanent(iVar5, iVar4);
              }
            }
            discard(player_00, 0, player);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0261ba38;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_0261ba38;
    }
  }
  get_card_instance(player, card);
LAB_0261ba38:
  iVar5 = can_sorcery_be_played(player, event);
  iVar5 = hybrid_split_card(player, card, event, 0x4b8a, iVar5, 6, 0x4b8d, 1, 10);
  return iVar5;
}

/* ManalinkEh 2022: 0x261bbb0; original cards/guilds_of_ravnica.c:1098. */
int card_doom_whisperer(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      surveil(player, 2);
    }
  }
  result = generic_activated_ability(player, card, event, 0, 0, 0, 0, 0, 0, 0, 2, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x261eb60; original cards/guilds_of_ravnica.c:2525. */
int card_mission_briefing(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (event == EVENT_RESOLVE_SPELL)
  {
    surveil(player, 2);
    result = count_graveyard_by_type(player, 0x38);
    if (result != 0)
    {

      new_default_test_definition(&test, 0x38, "Select target instant or sorcery card.");
      result = new_select_a_card(player, player, 2, 1, 1, -1, &test);
      if (-1 < result)
      {
        create_spell_has_flashback_legacy(player, card, result, 0);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x261f010; original cards/guilds_of_ravnica.c:2715. */
int card_notion_rain(int player, int card, event_t event)
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
      surveil(player, 2);
      draw_cards(player, 2);
      damage_player(player, 2, player, card);
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

/* ManalinkEh 2022: 0x2620b60; original cards/guilds_of_ravnica.c:3538. */
int card_sinister_sabotage(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_RESOLVE_SPELL)
  {
    result = counterspell_validate(player, card, NULL, 0);
    if (result != 0)
    {

      instance = get_card_instance(player, card);
      real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
      surveil(player, 1);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 2, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2620eb0; original cards/guilds_of_ravnica.c:3742. */
int card_thought_erasure(int player, int card, event_t event)
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
            new_default_test_definition(&this_test, 1, "Select a nonland card.");
            this_test.type_flag = 1;
            new_effect_coercion(&ec, &this_test);
            surveil(player, 1);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02620f2d;
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
LAB_02620f2d:
  result = generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x261a290; original cards/guilds_of_ravnica.c:268. */
int card_aurelia_exemplar_of_justice(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  target_definition_t td;

  check_legend_rule(player, card, event);
  mentor(player, card, event);
  result = beginning_of_combat_2022(player, card, event, player);
  if (result != 0)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.allow_cancel = 2;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "TARGET_CREATURE_YOU_CONTROL", 0, 0);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        uVar3 = get_color(instance->targets[0].player, instance->targets[0].card);
        pump_ability_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card,
                               2, 0, (uVar3 & 0x10) << 3, uVar3 >> 2 & 8);
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2621530; original cards/guilds_of_ravnica.c:3913. */
int card_unmoored_ego(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int csvid;
  int iVar4;
  int32_t player_00;
  int iVar5;
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
        default_target_definition(player, card, &td, 0);
        td.zone = 0x1000;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          iVar3 = valid_target(&td);
          if (iVar3 != 0)
          {

            if (player == 1)
            {
              iVar3 = count_deck(instance->targets[0].player);
              iVar3 = internal_rand(iVar3);
              player_00 = instance->targets[0].player;
              iVar5 = 0;
              do
              {
                iVar1 = deck_ptr[player_00][iVar3];
                if (iVar1 == -1)
                {
                  iVar5 = iVar5 + 1;
                  iVar3 = 0;
                }
                else
                {
                  iVar4 =
                      has_subtype_by_id_2022(1, (unsigned int)cards_data[iVar1].id, SUBTYPE_BASIC);
                  if (iVar4 == 0)
                  {

                    csvid = (unsigned int)cards_data[iVar1].id;
                    goto LAB_026216e7;
                  }
                  iVar3 = iVar3 + 1;
                  player_00 = instance->targets[0].player;
                }
              } while (iVar5 < 2);
              iVar3 = 0;
            }
            else
            {
              new_default_test_definition(&test, 0xff, "Choose a card name.");
              csvid = new_card_from_list(player, 3, &test);
              if ((int)csvid < 0)
              {
                player_00 = instance->targets[0].player;
                iVar3 = 0;
              }
              else
              {
              LAB_026216e7:
                iVar3 = lobotomy_effect_2022(player, instance->targets[0].player, csvid, 4, 4);
                player_00 = instance->targets[0].player;
              }
            }
            draw_cards(player_00, iVar3);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_026215ad;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  get_card_instance(player, card);
LAB_026215ad:
  iVar3 = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return iVar3;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2619160; original cards/guilds_of_ravnica.c:1502. */
static int is_basic_land_by_internal_id(int iid, int unused, int unused1, int unused2)
{
  int result;

  result = is_basic_land_by_id((unsigned int)cards_data[iid].id);
  return result;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x261ce20; original cards/guilds_of_ravnica.c:1505. */
int card_flower_flourish(int player, int card, event_t event)
{
  const char *msg;
  card_instance_t *instance;
  unsigned int uVar2;
  int result;
  test_definition_2022_t this_test;

  msg = get_hacked_land_text(player, card, "Select a basic %s or %s card.", 0x5001, 0x5007);
  new_default_test_definition_2022(&this_test, 1, msg);
  this_test.subtype = get_hacked_subtype(player, card, 0x5001);
  this_test.sub2 = get_hacked_subtype(player, card, 0x5007);
  this_test.subtype_flag = 3;
  this_test.special_selection_function = is_basic_land_by_internal_id;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_0261cf1d;
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
            new_global_tutor_2022(player, player, 1, 1, 0, 1, &this_test);
            uVar2 = instance->info_slot;
          }
          if ((uVar2 & 2) != 0)
          {
            pump_subtype_until_eot_2022(player, card, player, -1, 2, 2, 0, 0);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0261cf1d;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_0261cf1d;
    }
  }
  get_card_instance(player, card);
LAB_0261cf1d:
  result = hybrid_split_card(player, card, event, 0x4ba7, 1, 10, 0x4ba6, 1, 8);
  return result;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x261a7d0; original cards/guilds_of_ravnica.c:504. */
int card_chance_for_glory(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event != EVENT_RESOLVE_SPELL)
  {
    result = basic_spell(player, card, event);
    return result;
  }
  pump_creatures_until_eot_2022(player, card, player, 1, 0, 0, 0, -0x7ffff800, NULL);
  result = check_battlefield_for_id(1 - player, 0x2ddf);
  if (result == 0)
  {

    result = create_legacy_effect(player, card, last_chance_legacy_2022);
    alternate_legacy_text(2, player, result);
    instance = get_card_instance(player, result);
    instance->targets[1].player = 2;
    result = time_walk_effect(player, card);
    alternate_legacy_text(2, player, result);
  }
  else
  {
    kill_card(player, card, KILL_DESTROY);
  }
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 native scalar-word handlers. */
int card_portcullis_vine(int player, int card, event_t event);

/* ManalinkEh 2022: 0x261f9f0; original cards/guilds_of_ravnica.c:3054. */
int card_portcullis_vine(int player, int card, event_t event)
{
  int result;
  test_definition_2022_t test;
  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition_2022(&test, 2, "Select a creature with defender to sacrifice.");
    test.keyword = KEYWORD_DEFENDER;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition_2022(&test, 2, "Select a creature with defender to sacrifice.");
    test.keyword = KEYWORD_DEFENDER;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      draw_a_card(player);
    }
  }
  result = generic_activated_ability_with_sacrifice_as_cost_2022(
      player, card, event, GAA_LITERAL_PROMPT | GAA_UNTAPPED, 2, 0, 0, 0, 0, 0, 0, NULL,
      "Select a creature with defender to sacrifice.", ~TYPE_NONE, &test, SARV_0);
  return result;
}

#endif
