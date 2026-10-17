#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int doublecast_legacy_mana_value_2_or_less(int player, int card, event_t event);
int triplecast_legacy_mana_value_2_or_less(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2306e80; original cards/zendikar_rising.c:143. */
void mdfc_transform(int player, int card, int csvid)
{
  int result;
  card_instance_t *instance;

  true_transform_to_csvid(player, card, csvid);
  result = check_special_flags3(player, card, 0x200);
  if (result == 0)
  {
    set_special_flags3(player, card, 0x200);
  }
  else
  {
    remove_special_flags3(player, card, 0x200);
  }
  instance = get_card_instance(player, card);
  instance->regen_status = instance->regen_status | 0x1f000000;
  get_abilities(player, card, EVENT_CHANGE_TYPE, -1);
  get_abilities(player, card, EVENT_SET_COLOR, -1);
  result = is_what(player, card, 2);
  if (result != 0)
  {
    get_abilities(player, card, EVENT_POWER, -1);
    get_abilities(player, card, EVENT_TOUGHNESS, -1);
    get_abilities(player, card, EVENT_ABILITIES, -1);
  }
  if (csvid == instance->original_internal_card_id)
  {
    instance->targets[0xc].card = -1;
    instance->targets[0xd].player = -1;
    instance->targets[0xd].card = -1;
  }
  return;
}

/* ManalinkEh 2022: 0x2306b30; original cards/zendikar_rising.c:405. */
static int angel_of_destiny_legacy(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  iVar2 = eot_trigger(player, card, event);
  if ((iVar2 != 0) && (iVar2 = 1 - player, iVar2 == current_turn))
  {
    iVar1 = life[iVar2];
    iVar2 = get_starting_life_total(iVar2);
    if (iVar2 <= iVar1)
    {
      lose_the_game(player);
    }
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2307e10; original cards/zendikar_rising.c:444. */
int card_anowon_the_ruin_thief(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;
  check_legend_rule(player, card, event);
  boost_subtype(player, card, event, SUBTYPE_ROGUE, 1, 1, 0, 0, BCT_CONTROLLER_ONLY);
  if (event == EVENT_DEAL_DAMAGE)
  {
    default_test_definition(&test, 0xff);
    test.subtype = 0x9f;
    check_damage_test(player, -1, EVENT_DEAL_DAMAGE,
                      DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                          DDBM_REPORT_DAMAGE_DEALT | DDBM_MUST_BE_COMBAT_DAMAGE,
                      player, card, 0, &test);
  }
  result = resolve_damage_trigger(player, card, event,
                                  DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                      DDBM_REPORT_DAMAGE_DEALT | DDBM_MUST_BE_COMBAT_DAMAGE,
                                  player, card);
  if (result == 0)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  special_mill(player, card, 0x550c, 0,
               (unsigned int)(unsigned char)instance->targets[0x10].player);
  special_mill(player, card, 0x550c, 1,
               (unsigned int)*(unsigned char *)((int)&instance->targets[0x10].player + 1));
  return 0;
}

/* ManalinkEh 2022: 0x2308800; original cards/zendikar_rising.c:756. */
int card_canyon_jerboa(int player, int card, event_t event)
{
  landfall(player, card, event);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  pump_creatures_until_eot_merge_pt(player, card, player, 1, 1, NULL);
  return 0;
}

/* ManalinkEh 2022: 0x2308a50; original cards/zendikar_rising.c:851. */
int card_confounding_conundrum(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  result = get_trap_condition(1 - player, 0x402);
  if (1 < result)
  {
    etb_ability_global(1 - player, event, 2, TYPE_LAND, NULL, player, card);
  }
  if (event == EVENT_ETB)
  {
    if ((trigger_cause_controller == player) && (trigger_cause == card))
    {
      draw_a_card(player);
    }
    else
    {
      default_target_definition(player, card, &td, 1);
      td.illegal_abilities = 0;
      td.allow_cancel = 0;
      td.allowed_controller = trigger_cause_controller;
      td.preferred_controller = trigger_cause_controller;
      td.who_chooses = trigger_cause_controller;
      instance = get_card_instance(player, card);
      instance->number_of_targets = '\0';
      result = can_target(&td);
      if (result != 0)
      {
        result = new_pick_target(&td, "Select a land you control to return to your hand.", 0, 0x80);
        if (result != 0)
        {
          bounce_permanent(instance->targets[0].player, instance->targets[0].card);
        }
      }
      instance->number_of_targets = '\0';
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x230b7a0; original cards/zendikar_rising.c:1863. */
int card_legion_angel(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  result = add_card_to_hand(player, instance->internal_card_id);
  reveal_card(player, card, player, result);
  return 0;
}

/* ManalinkEh 2022: 0x230ba50; original cards/zendikar_rising.c:1963. */
int card_maddening_cacophony(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  int player_00;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
      kicker(player, card, EVENT_CAST_SPELL, 3, 0, 1, 0, 0, 0);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        ai_modifier = ai_modifier + 0x60;
      }
      goto LAB_0230bac9;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      kicker(player, card, event, 3, 0, 1, 0, 0, 0);
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = kicked(player, card);
        player_00 = 1 - player;
        uVar1 = 8;
        if (result != 0)
        {
          uVar1 = count_deck(player_00);
          uVar1 = round_up_value(uVar1);
        }
        mill(player_00, uVar1);
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_0230bac9;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  kicker(player, card, event, 3, 0, 1, 0, 0, 0);
LAB_0230bac9:
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x230bde0; original cards/zendikar_rising.c:2011. */
int card_makindi_ox(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  landfall(player, card, event);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = 1 - player;
  td.allow_cancel = 0;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = pick_target(&td, "TARGET_CREATURE_OPPONENT_CONTROLS");
    if (result != 0)
    {
      tap_card(instance->targets[0].player, instance->targets[0].card);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x2306820; original cards/zendikar_rising.c:2059. */
static int malakir_rebirth_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  result = instance->targets[0].player;
  seek_grave_for_id_to_reanimate(
      result, card, result, (unsigned int)cards_data[instance->targets[0].card].id, REANIMATE_TAP);
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x230d550; original cards/zendikar_rising.c:2768. */
int card_ruin_crab(int player, int card, event_t event)
{
  landfall(player, card, event);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  mill(1 - player, 3);
  return 0;
}

/* ManalinkEh 2022: 0x230d980; original cards/zendikar_rising.c:2856. */
int card_scute_swarm(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  landfall(player, card, event);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  result = count_permanents_by_type(player, TYPE_LAND);
  if (result < 6)
  {
    default_token_definition(player, card, 0x4952, &token);
  }
  else
  {
    copy_token_definition(player, card, &token, player, card);
  }
  generate_token(&token);
  return 0;
}

/* ManalinkEh 2022: 0x23068c0; original cards/zendikar_rising.c:2906. */
int doublecast_legacy_mana_value_2_or_less(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = check_special_flags(trigger_cause_controller, trigger_cause, 0x40000);
    if (result == 0)
    {
      default_test_definition(&test, 0x38);
      test.cmc = 3;
      test.cmc_flag = 3;
      result = new_specific_spell_played(player, card, event, player, 2, &test);
      if (result != 0)
      {
        copy_spell_from_stack(player, trigger_cause_controller, trigger_cause);
        kill_card(player, card, KILL_EXILE);
      }
    }
  }
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x23069f0; original cards/zendikar_rising.c:2923. */
int triplecast_legacy_mana_value_2_or_less(int player, int card, event_t event)
{
  int t_card;
  int iVar1;
  int iVar2;
  test_definition_t test;
  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    iVar1 = check_special_flags(trigger_cause_controller, trigger_cause, 0x40000);
    if (iVar1 == 0)
    {
      default_test_definition(&test, 0x38);
      test.cmc = 3;
      test.cmc_flag = 3;
      iVar2 = new_specific_spell_played(player, card, event, player, 2, &test);
      t_card = trigger_cause;
      iVar1 = trigger_cause_controller;
      if (iVar2 != 0)
      {
        copy_spell_from_stack(player, trigger_cause_controller, trigger_cause);
        copy_spell_from_stack(player, iVar1, t_card);
        kill_card(player, card, KILL_EXILE);
      }
    }
  }
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x230db10; original cards/zendikar_rising.c:2942. */
int card_sea_gate_stormcaller(int player, int card, event_t event)
{
  int result;
  kicker(player, card, event, 1, 0, 1, 0, 0, 0);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  result = kicked(player, card);
  if (result != 0)
  {
    create_legacy_effect(player, card, triplecast_legacy_mana_value_2_or_less);
    return 0;
  }
  create_legacy_effect(player, card, doublecast_legacy_mana_value_2_or_less);
  return 0;
}

/* ManalinkEh 2022: 0x230e8e0; original cards/zendikar_rising.c:3240. */
int card_skyclave_squid(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  landfall(player, card, event);
  if (event == EVENT_ETB)
  {
    card_00 = create_targetted_legacy_effect_no_repeat(
        player, card, effect_defender_can_attack_until_eot, player, card);
    if (card_00 != -1)
    {
      instance = get_card_instance(player, card_00);
      instance->token_status = instance->token_status | 0x10000;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x230f070; original cards/zendikar_rising.c:3459. */
int card_tajuru_paragon(int player, int card, event_t event)
{
  int result;
  reported_subtypes_t *prVar2;
  unsigned char bVar3;
  reported_subtypes_t my_stypes;
  test_definition_t test;
  bVar3 = 0;
  kicker(player, card, event, 3, 0, 0, 0, 0, 0);
  result = kicked(player, card);
  etb_ability(player, card, event, (unsigned int)(result != 0) * 2);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  prVar2 = &my_stypes;
  for (result = 0x1a; result != 0; result = result + -1)
  {
    prVar2->reported_subtypes[0] = 0;
    prVar2 = (reported_subtypes_t *)((int)prVar2 + ((unsigned int)bVar3 * -2 + 1) * 4);
  }
  get_subtypes(player, card, &my_stypes);
  default_test_definition(&test, 0xff);
  test.subtype = my_stypes.reported_subtypes[0];
  test.sub2 = my_stypes.reported_subtypes[1];
  test.sub3 = my_stypes.reported_subtypes[2];
  test.sub4 = my_stypes.reported_subtypes[3];
  test.subtype_flag = 3;
  test.sub5 = my_stypes.reported_subtypes[4];
  reveal_top_cards_of_library_and_choose(player, card, player, 6, 0, TUTOR_HAND, 1,
                                         TUTOR_BOTTOM_OF_DECK_RANDOM, 1, &test);
  return 0;
}
