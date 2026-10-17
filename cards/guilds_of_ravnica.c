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
      gain_life(player, result);
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
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  target_definition_t td;
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
      default_target_definition(player, card, &td, 0x47);
      td.required_subtype = 0x500c;
      td.preferred_controller = player;
      td.allowed_controller = player;
      td.illegal_abilities = 0;
      td.illegal_state = 1;
      td.allow_cancel = 3;
      iStack_ec = 0;
      while (
          (result = can_target(&td),
           result != 0 && (result = new_pick_target(&td, "Choose an untapped Gate to tap", 0, 0x80),
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
      default_test_definition((test_definition_t *)&td, 0x47);
      etb_ability_global(player, event, 2, TYPE_NONE, (test_definition_t *)&td, player, card);
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
    pump_until_eot(player, card, player, card, result, 0);
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
  pump_ability_until_eot(token->s_player, token->s_card, token->t_player, beast, 0, 0, 0, 0x404);
  return;
}

/* ManalinkEh 2022: 0x261e850; original cards/guilds_of_ravnica.c:2380. */
int card_maximize_altitude(int player, int card, event_t event)
{
  int result;
  if ((event != EVENT_GRAVEYARD_ABILITY) && (event != EVENT_PAY_FLASHBACK_COSTS))
  {
    result = vanilla_instant_pump(player, card, event, 2, player, 1, 1, 0x20, 0);
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
    result = vanilla_instant_pump(player, card, event, 2, player, 1, 1, 0, 4);
    return result;
  }
  result = jumpstart(player, card, event, 0x4be7);
  return result;
}

/* ManalinkEh 2022: 0x261eea0; original cards/guilds_of_ravnica.c:2658. */
int card_nightveil_predator(int player, int card, event_t event)
{
  deathtouch(player, card, event);
  hexproof(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x261f880; original cards/guilds_of_ravnica.c:3018. */
int card_pitiless_gorgon(int player, int card, event_t event)
{
  int result;
  deathtouch(player, card, event);
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
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
            copy_token(player, card, player, instance->targets[0].card);
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
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
    pump_ability_until_eot(iVar1, card_00, iVar1, card_00, 0, 0, 0, 0x800);
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
      lose_the_game(0);
    }
    if (uVar4 != 0)
    {
      do
      {
        lose_the_game(1);
        uVar4 = uVar4 - 1;
      } while (uVar4 != 0);
      return 0;
    }
  }
  return 0;
}
