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

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x230f290; original cards/zendikar_rising.c:3539. */
int card_thieving_skydiver(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t td;
  char buffer[100];

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  kicker(player, card, event, -1, 0, 0, 0, 0, 0);
  if (event == EVENT_ETB)
  {
    iVar1 = kicked(player, card);
    if (iVar1 != 0)
    {
      instance = get_card_instance(player, card);
      if ((instance != NULL) && (0 < instance->info_slot))
      {
        default_target_definition(player, card, &td, 0x40);
        td.extra = instance->info_slot;
        td.special = 0x800;
        iVar1 = can_target(&td);
        if (iVar1 != 0)
        {
          scnprintf(buffer, 100, " Select an artifact with mana value %d or less.",
                    instance->info_slot);
          iVar1 = new_pick_target(&td, buffer, 0, 0x81);
          if (iVar1 != 0)
          {
            iVar1 = gain_control_permanently(player, instance->targets[0].player,
                                             instance->targets[0].card);
            iVar3 = is_equipment(player, iVar1);
            if (iVar3 != 0)
            {
              equip_target_creature(player, iVar1, player, card);
            }
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x230fbc0; original cards/zendikar_rising.c:3839. */
int card_yasharn_implacable_earth(int player, int card, event_t event)
{
  const char *pcVar1;
  test_definition_t test;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  pcVar1 = get_hacked_land_text(player, card, "Select a basic %s.", 0x5001);
  new_default_test_definition(&test, 1, pcVar1);
  test.subtype = get_hacked_subtype(player, card, 0x5001);
  test.sub2 = 0x2000;
  test.subtype_flag = 4;
  test.no_shuffle = 1;
  new_global_tutor(player, player, 1, 1, 0, 1, &test);
  pcVar1 = get_hacked_land_text(player, card, "Select a basic %s.", 0x5007);
  new_default_test_definition(&test, 1, pcVar1);
  test.subtype = get_hacked_subtype(player, card, 0x5007);
  test.sub2 = 0x2000;
  test.subtype_flag = 4;
  new_global_tutor(player, player, 1, 1, 0, 1, &test);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2308450; original cards/zendikar_rising.c:619. */
int card_bloodchiefs_thirst(int player, int card, event_t event)
{
  int result;
  const char *prompt;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x102);
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x102);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        result = is_token(player, card);
        if ((result == 0) && (result = check_special_flags(player, card, 0x40000), result == 0))
        {
          do_kicker(player, card, 2, 1, 0, 0, 0, 0);
        }
        result = kicked(player, card);
        if (result == 0)
        {
          td.extra = 2;
          td.special = 0x800;
          result = can_target(&td);
          if (result == 0)
          {
            spell_fizzled = 1;
            ai_modifier = ai_modifier + -999;
            return 0;
          }
        }
        result = kicked(player, card);
        prompt = "Select target permanent or player";
        if (result == 0)
        {
          prompt = "Select target permanent or player with mana value 2 or less";
        }
        generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td, prompt, 1, NULL);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x102);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        kill_target0(player, card, KILL_DESTROY);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 0x102);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2308ff0; original cards/zendikar_rising.c:1091. */
int card_feed_the_swarm(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 6);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {

            kill_target0(player, card, KILL_DESTROY);
            instance = get_card_instance(player, card);
            result = get_cmc(instance->targets[0].player, instance->targets[0].card);
            lose_life(player, result);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02309045;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 6);
LAB_02309045:
  result = generic_spell(player, card, event, 0x81, &td, "Select target enchantment or creature.",
                         1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x230fe80; original cards/zendikar_rising.c:3889. */
int card_zareth_san_the_trickster(int player, int card, event_t event)
{
  unsigned int uVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  unsigned int uVar5;
  unsigned int *puVar6;
  int times_damaged[2];

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  check_legend_rule(player, card, event);
  if (1 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    ninjutsu(player, card, event, 2, 1, 1, 0, 0, 0, 0x5628);
    iVar2 = damage_dealt_by_me(player, card, event,
                               DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_BE_COMBAT_DAMAGE |
                                   DDBM_TRIGGER_OPTIONAL);
    if (iVar2 != 0)
    {

      puVar6 = (unsigned int *)times_damaged;
      default_test_definition(&recovered_stack_td.test, 0x47);
      iVar2 = 0;
      instance = get_card_instance(player, card);
      uVar1 = instance->targets[1].player;
      instance->targets[1].player = 0;
      uVar5 = uVar1 & 0xff;
      times_damaged[1] = uVar1 >> 8 & 0xff;
      times_damaged[0] = uVar5;
      while (true)
      {
        while (uVar5 != 0)
        {
          iVar4 = any_in_graveyard_by_type(iVar2, 0x47);
          if ((iVar4 == 0) || (iVar4 = new_select_target_from_grave(player, card, iVar2, SFG_0, 2,
                                                                    &recovered_stack_td.test, 0),
                               iVar4 < 0))
          {
            uVar5 = uVar5 - 1;
            *puVar6 = uVar5;
          }
          else
          {
            reanimate_permanent(player, card, iVar2, instance->targets[0].player,
                                REANIMATE_DEFAULT);
            uVar5 = uVar5 - 1;
            *puVar6 = uVar5;
          }
        }
        iVar2 = iVar2 + 1;
        puVar6 = puVar6 + 1;
        if (iVar2 == 2)
          break;
        uVar5 = *puVar6;
      }
    }
    iVar2 = flash(player, card, event);
    return iVar2;
  }
  default_target_definition(player, card, &recovered_stack_td.target, 2);
  recovered_stack_td.target.required_subtype = 0x9f;
  iVar2 = ninjutsu_custom_target(player, card, event, 2, 1, 1, 0, 0, 0, &recovered_stack_td.target,
                                 0x5628);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int is_instant_sorcery_or_wizard(int iid, int unused, int player, int card);
int mdfc_front(int player, int card, event_t event);
static int malakir_rebirth_effect(int player, int card, event_t event);
static int mdfc_front_impl(int player, int card, event_t event, int can_play_front,
                           int suppress_charging_back_cost);
int mdfc_back(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23077a0; original cards/zendikar_rising.c:169. */
int mdfc_back(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_COUNTERSPELL) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_token(player, card);
    if (result == 0)
    {

      result = get_id(player, card);
      result = dfc_back_to_front(result);
      mdfc_transform(player, card, result);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2306fe0; original cards/zendikar_rising.c:177. */
static int mdfc_front_impl(int player, int card, event_t event, int can_play_front,
                           int suppress_charging_back_cost)
{
  card_ptr_t *pcVar1;
  unsigned char bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  card_instance_t *instance;
  const char *pcVar6;
  int iVar7;
  unsigned int csvid;
  unsigned int uVar8;
  bool bVar9;
  unsigned int uStack_28;
  unsigned int uStack_24;
  const char *pcStack_1c;

  if (((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) &&
       event != EVENT_PLAY_FIRST_HALF) &&
      (event != EVENT_CAN_CAST))
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
    else if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
             (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  iVar3 = get_original_internal_card_id(player, card);
  csvid = (unsigned int)cards_data[iVar3].id;
  bVar2 = cards_data[iVar3].type;
  if ((bVar2 & 0x30) == 0)
  {
    iVar4 = can_sorcery_be_played(player, event);
    if (iVar4 != 0)
    {
      bVar2 = cards_data[iVar3].type;
      goto LAB_02307047;
    }
  LAB_0230705c:
    can_play_front = 0;
  }
  else
  {
  LAB_02307047:
    if (((bVar2 & 1) != 0) &&
        ((((unsigned char)land_can_be_played & 1) != 0 || (player != current_turn))))
      goto LAB_0230705c;
  }
  iVar4 = dfc_front_to_back(csvid);
  if (((stack_size == 1) && (card_on_stack_controller == player)) &&
      ((card_on_stack == card && (iVar5 = can_legally_play_csvid(player, iVar4), iVar5 != 0))))
  {
    bVar9 = true;
  }
  else
  {
    iVar5 = can_legally_play_csvid_now(player, iVar4, event);
    bVar9 = iVar5 != 0;
  }
  if (event == EVENT_MODIFY_COST)
  {
    if ((((suppress_charging_back_cost == 0) &&
          (iVar3 = has_mana_to_cast_id(player, EVENT_MODIFY_COST, iVar4), iVar3 == 0)) ||
         (!bVar9)) &&
        ((iVar3 = has_mana_to_cast_id(player, EVENT_MODIFY_COST, csvid),
          iVar3 == 0 || (can_play_front == 0))))
    {
      infinite_casting_cost();
      return 0;
    }

    iVar3 = get_id(player, card);
    pcVar1 = cards_ptr[iVar3];
    iVar3 = get_updated_casting_cost(player, card, -1, EVENT_MODIFY_COST, -1);
    COST_COLORLESS = COST_COLORLESS - (char)iVar3;
    iVar3 = get_id(player, card);
    bVar9 = false;
    if (iVar3 == 0x5560)
    {
      iVar3 = count_basic_lands(1 - player, -1);
      bVar9 = iVar3 == 0;
    }
    COST_COLORLESS = COST_COLORLESS - bVar9;
    COST_BLACK = COST_BLACK - pcVar1->req_black;
    COST_BLUE = COST_BLUE - pcVar1->req_blue;
    COST_GREEN = COST_GREEN - pcVar1->req_green;
    COST_RED = COST_RED - pcVar1->req_red;
    COST_WHITE = COST_WHITE - pcVar1->req_white;
    return 0;
  }
  instance = get_card_instance(player, card);
  if (event == EVENT_CHANGE_TYPE)
  {
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
    iVar3 = instance->targets[0xc].card;
    if (iVar3 < 0)
    {
      return 0;
    }
    event_result = iVar3;
    instance->regen_status = instance->regen_status | 0x9000000;
    return 0;
  }
  if (event == EVENT_CAN_CAST)
  {

    if ((suppress_charging_back_cost == 0) &&
        (iVar3 = has_mana_to_cast_id(player, EVENT_CAN_CAST, iVar4), iVar3 == 0))
    {
      uVar8 = 0;
    }
    else
    {
      uVar8 = (unsigned int)bVar9;
    }
    iVar3 = has_mana_to_cast_id(player, EVENT_CAN_CAST, csvid);
    if (can_play_front != 0)
    {
      if (iVar3 != 0)
      {
        return ((can_play_front == 99) - 1 & 0xffffff9e) + 99;
      }
      return uVar8;
    }
    return uVar8;
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
  iVar5 = check_special_flags3(player, card, 1);
  if (iVar5 != 0)
  {
    return 0;
  }

  iVar5 = played_for_free(player, card);
  if (iVar5 == 0)
  {
    iVar5 = is_token(player, card);
    uStack_24 = (unsigned int)(iVar5 != 0);
    iVar7 = check_special_flags2(player, card, -0x80000000);
    if (iVar7 == 0)
    {
      uStack_24 = uStack_24 | suppress_charging_back_cost;
      if (uStack_24 == 0)
      {
        iVar5 = has_mana_to_cast_id(player, EVENT_CAST_SPELL, iVar4);
        if (iVar5 == 0)
        {
          uStack_28 = 0;
        }
        else
        {
          uStack_28 = (unsigned int)bVar9;
        }
        pcStack_1c = get_card_name_by_id(iVar4);
      }
      else
      {
        uStack_28 = (unsigned int)bVar9;
        pcStack_1c = get_card_name_by_id(iVar4);
        if (iVar5 != 0)
          goto LAB_023073fc;
      }
      iVar5 = has_mana_to_cast_id(player, EVENT_CAST_SPELL, csvid);
      uStack_24 = 0;
      uVar8 = 0;
      if (iVar5 != 0)
        goto LAB_023073fc;
      goto LAB_02307406;
    }
  }
  else
  {
    iVar5 = check_special_flags2(player, card, -0x80000000);
    if (iVar5 != 0)
    {
      if (instance->mode != '\x01')
      {
        if (instance->mode != '\x02')
        {
          return 0;
        }
        instance->targets[0xd].player = csvid;
        instance->targets[0xd].card = csvid;
        mdfc_transform(player, card, iVar4);
        goto LAB_023074f5;
      }
      goto LAB_0230771c;
    }

    uStack_28 = (unsigned int)bVar9;
    pcStack_1c = get_card_name_by_id(iVar4);
    uStack_24 = 1;
  LAB_023073fc:
    uVar8 = (unsigned int)(can_play_front != 0);
  LAB_02307406:
    pcVar6 = get_card_name_by_instance(instance);
    iVar5 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033", "\001\016", pcVar6,
                        uVar8, 1, pcStack_1c, uStack_28, 1, 0, 0, 0, 0);
    if (iVar5 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
  }
  if (instance->mode != '\x01')
  {
    if (instance->mode != '\x02')
    {
      return 0;
    }
    instance->targets[0xd].player = csvid;
    instance->targets[0xd].card = csvid;
    mdfc_transform(player, card, iVar4);
    if (uStack_24 == 0 && suppress_charging_back_cost == 0)
    {
      charge_mana_from_id(player, card, EVENT_CAST_SPELL, iVar4);
    }
  LAB_023074f5:
    bVar9 = spell_fizzled != 1;
    instance->targets[1].card = x_value;
    if (bVar9)
    {

      iVar3 = get_internal_card_id_from_csv_id(iVar4);
      call_card_fn(cards_data[iVar3].code_pointer, instance, player, card, EVENT_CAST_SPELL);
      if (spell_fizzled != 1)
      {
        return 0;
      }
    }
    mdfc_transform(player, card, csvid);
    return 0;
  }
  if (uStack_24 == 0)
  {
    charge_mana_from_id(player, card, EVENT_CAST_SPELL, csvid);
    iVar4 = is_x_spell(player, card);
    if (iVar4 != 0)
    {
      set_x_for_x_spells(player, card, EVENT_CAST_SPELL, x_value);
    }
  }
LAB_0230771c:
  if (spell_fizzled == 1)
  {
    return 0;
  }

  call_card_fn(cards_data[iVar3].code_pointer, instance, player, card, EVENT_PLAY_FIRST_HALF);
  return 0;
}

/* ManalinkEh 2022: 0x2307820; original cards/zendikar_rising.c:270. */
int mdfc_front_conditional(int player, int card, event_t event, int can_play_front)
{
  int result;

  result = mdfc_front_impl(player, card, event, can_play_front, 0);
  return result;
}

/* ManalinkEh 2022: 0x2307840; original cards/zendikar_rising.c:273. */
int mdfc_front_suppress_charging_mana(int player, int card, event_t event)
{
  int result;

  result = mdfc_front_impl(player, card, event, 1, 1);
  return result;
}

/* ManalinkEh 2022: 0x2307860; original cards/zendikar_rising.c:277. */
int mdfc_front(int player, int card, event_t event)
{
  int result;

  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x2307880; original cards/zendikar_rising.c:291. */
int cmc_not_in_array(int iid, int arrptr, int player, int card)
{
  int iVar1;
  int iVar2;
  int iVar3;

  iVar3 = 0;
  do
  {
    iVar1 = get_cmc_by_id((unsigned int)cards_data[iid].id);
    iVar2 = get_cmc_by_id((unsigned int)cards_data[*(int *)arrptr].id);
    if (iVar1 == iVar2)
    {
      return 0;
    }
    iVar3 = iVar3 + 1;
    arrptr = arrptr + 4;
  } while ((iVar3 < 4) && (iVar3 != -1));
  return 1;
}

/* ManalinkEh 2022: 0x2307c90; original cards/zendikar_rising.c:377. */
int mdfc_back_etb_tapped_land(int player, int card, event_t event)
{
  int result;

  mdfc_back(player, card, event);
  result = plain_mana_producer_tapped(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2307d10; original cards/zendikar_rising.c:414. */
int card_angel_of_destiny(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_DEAL_DAMAGE)
  {
    instance = get_card_instance(affected_card_controller, affected_card);
    if (instance->internal_card_id != damage_card)
    {
      return 0;
    }
    if (instance->info_slot < 1)
    {
      return 0;
    }
    if (instance->damage_source_player != player)
    {
      return 0;
    }
    if ((instance->token_status & 0x140000) == 0)
    {
      return 0;
    }
    if ((instance->targets[3].player & 2) == 0)
    {
      return 0;
    }

    gain_life(current_turn, instance->info_slot);
    gain_life(current_turn ^ 1, instance->info_slot);
  }

  result = declare_attackers_trigger(
      player, card, event, DAT_ATTACKS_PLAYER | DAT_RESOLVE_TRIGGER_MANDATORY, player, card);
  if (result == 0)
  {
    return 0;
  }
  create_legacy_effect_for_opponent(player, card, angel_of_destiny_legacy, -1, -1);
  return 0;
}

/* ManalinkEh 2022: 0x2307f80; original cards/zendikar_rising.c:475. */
int card_archon_of_emeria(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if ((event == EVENT_CAST_SPELL) || (event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE))
  {
    default_test_definition(&test, 1);
    test.subtype = 0x2000;
    test.subtype_flag = 1;
    permanents_enters_battlefield_tapped(player, card, event, 1 - player, 0, &test);
  }
  if ((((event == EVENT_MODIFY_COST_GLOBAL) &&
        (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
       (result = is_what(affected_card_controller, affected_card, 1), result == 0)) &&
      (result = get_specific_storm_count(affected_card_controller), 0 < result))
  {
    infinite_casting_cost();
  }
  return 0;
}

/* ManalinkEh 2022: 0x2308080; original cards/zendikar_rising.c:521. */
int card_bela_ged_recovery(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    default_test_definition(&this_test, 0xff);
    this_test.ai_selection_mode = 1;
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      generic_spell(player, card, EVENT_CAST_SPELL, 8, NULL, NULL, 1, &this_test);
      if (spell_fizzled != 1)
      {

        instance = get_card_instance(player, card);
        ai_modifier =
            ai_modifier +
            cards_ptr[graveyard_ptr_mutable[player][instance->targets[0].player]]->ai_base_value;
      }
      return 0;
    }
  joined_r0x02308181:
    this_test.ai_selection_mode = 1;
    if (event == EVENT_RESOLVE_SPELL)
    {

      this_test.ai_selection_mode = 1;
      result = validate_target_from_grave_source(player, card, player, 0);
      if (result != -1)
      {
        from_grave_to_hand(player, result, 1);
      }
      kill_card(player, card, KILL_DESTROY);
    }
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_test_definition(&this_test, 0xff);
        goto joined_r0x02308181;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
    default_test_definition(&this_test, 0xff);
    this_test.ai_selection_mode = 1;
  }
  result = generic_spell(player, card, EVENT_CAN_CAST, 8, NULL, NULL, 1, &this_test);
  result = mdfc_front_impl(player, card, event, result, 0);
  return result;
}

/* ManalinkEh 2022: 0x23082c0; original cards/zendikar_rising.c:569. */
int card_beyeen_veil(int player, int card, event_t event)
{
  int result;

  if ((event == EVENT_PLAY_FIRST_HALF ||
       (event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE)) ||
      (event == EVENT_CAN_CAST))
  {
  LAB_023082f7:
    if (event != EVENT_RESOLVE_SPELL)
      goto LAB_023082fc;
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
      goto LAB_023082fc;
    }
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event != EVENT_CAN_CHANGE_TARGET)
      {
        if (event != EVENT_CHANGE_TARGET)
        {
          return 0;
        }
        goto LAB_023082fc;
      }
      goto LAB_023082f7;
    }
  }
  pump_subtype_until_eot(player, card, 1 - player, -1, -2, 0, 0, 0);
  kill_card(player, card, KILL_DESTROY);
LAB_023082fc:
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x23083c0; original cards/zendikar_rising.c:591. */
int card_blackbloom_rogue(int player, int card, event_t event)
{
  int result;

  menace(player, card, event);
  if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
  {
    result = count_graveyard(1 - player);
    if (7 < result)
    {
      event_result = event_result + 3;
    }
  }
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x23086d0; original cards/zendikar_rising.c:659. */
int mdfc_front_land(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_CAN_CAST)
  {
    mdfc_front_impl(player, card, event, 1, 0);
    result = plain_mana_producer(player, card, event);
    return result;
  }
  result = mdfc_front_impl(player, card, EVENT_CAN_CAST, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x2308740; original cards/zendikar_rising.c:691. */
int mdfc_back_land(int player, int card, event_t event)
{
  int result;

  mdfc_back(player, card, event);
  result = plain_mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2308780; original cards/zendikar_rising.c:726. */
int card_brushfire_elemental(int player, int card, event_t event)
{
  int result;

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
  result = card_steppe_lynx(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2308e40; original cards/zendikar_rising.c:1014. */
int card_emerias_call(int player, int card, event_t event)
{
  int result;
  pump_ability_t pump;
  test_definition_t test;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
  LAB_02308e74:
    if (event != EVENT_RESOLVE_SPELL)
      goto LAB_02308e79;
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
      goto LAB_02308e79;
    }
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event != EVENT_CAN_CHANGE_TARGET)
      {
        if (event != EVENT_CHANGE_TARGET)
        {
          return 0;
        }
        goto LAB_02308e79;
      }
      goto LAB_02308e74;
    }
  }
  generate_tokens_by_id(player, card, 0x5509, 2);
  default_pump_ability_definition(player, card, &pump, 0, 0, 0, 0x800);
  pump.paue_flags = 0x8000;
  default_test_definition(&test, 2);
  test.subtype = 2;
  test.subtype_flag = 1;
  pump_ability_by_test(player, card, player, &pump, &test);
  kill_card(player, card, KILL_DESTROY);
LAB_02308e79:
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x23092f0; original cards/zendikar_rising.c:1189. */
int card_glasspool_mimic(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_RESOLVE_SPELL)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.allow_cancel = 1;
    result = enters_the_battlefield_as_copy_of_any(player, card, EVENT_RESOLVE_SPELL, &td,
                                                   "ASHNODS_BATTLEGEAR");

    if ((result != 0) &&
        (result = add_a_subtype_permanently(player, card, player, card, SUBTYPE_ROGUE),
         -1 < result))
    {
      instance = get_card_instance(player, result);
      instance->targets[2].card = 0xac;
    }
    return 0;
  }
  enters_the_battlefield_as_copy_of_any(player, card, event, NULL, "ASHNODS_BATTLEGEAR");
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x23093f0; original cards/zendikar_rising.c:1262. */
int card_hagra_mauling(int player, int card, event_t event)
{
  int result;
  target_definition_t td1;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    default_target_definition(player, card, &td1, 2);
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      generic_spell(player, card, EVENT_CAST_SPELL, 1, &td1, "TARGET_CREATURE", 1, NULL);
      goto LAB_02309459;
    }
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
    }
    else if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
             (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
    default_target_definition(player, card, &td1, 2);
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td1);
    if (result != 0)
    {
      kill_target0(player, card, KILL_DESTROY);
    }
    kill_card(player, card, KILL_DESTROY);
  }
LAB_02309459:
  result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td1, "TARGET_CREATURE", 1, NULL);
  result = mdfc_front_impl(player, card, event, result, 0);
  return result;
}

/* ManalinkEh 2022: 0x230ade0; original cards/zendikar_rising.c:1645. */
int card_jwari_disruption(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      result = counterspell(player, card, EVENT_CAST_SPELL, NULL, 0);
      return result;
    }
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
      goto LAB_0230ae29;
    }
    if (event == EVENT_RESOLVE_SPELL)
      goto LAB_0230aeb0;
    if (event != EVENT_CAN_CHANGE_TARGET)
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      goto LAB_0230ae29;
    }
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
  LAB_0230aeb0:
    counterspell_resolve_unless_pay_x(player, card, NULL, 0, 1);
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
LAB_0230ae29:
  result = counterspell(player, card, EVENT_CAN_CAST, NULL, 0);
  result = mdfc_front_impl(player, card, event, result, 0);
  return result;
}

/* ManalinkEh 2022: 0x230af30; original cards/zendikar_rising.c:1676. */
int card_kabira_takedown(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    default_target_definition(player, card, &td, 0x102);
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      result = generic_spell(player, card, EVENT_CAST_SPELL, 1, &td,
                             "TARGET_CREATURE_OR_PLANESWALKER", 1, NULL);
      return result;
    }
  joined_r0x0230af93:
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        result = count_subtype(player, 2, -1);
        damage_target0(player, card, result);
      }
      kill_card(player, card, KILL_DESTROY);
    }
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 0x102);
        goto joined_r0x0230af93;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td, 0x102);
  }
  result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_CREATURE_OR_PLANESWALKER", 1,
                         NULL);
  result = mdfc_front_impl(player, card, event, result, 0);
  return result;
}

/* ManalinkEh 2022: 0x230b140; original cards/zendikar_rising.c:1724. */
int card_kazandu_mammoth(int player, int card, event_t event)
{
  int result;

  card_steppe_lynx(player, card, event);
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x230b4a0; original cards/zendikar_rising.c:1800. */
int card_khalni_ambush(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int can_play_front;
  target_definition_t *ptStack_bc;
  target_definition_t td;
  target_definition_t td1;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = 1 - player;
    td1.preferred_controller = td1.allowed_controller;
    instance = get_card_instance(player, card);
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      instance->number_of_targets = '\0';
      result = new_pick_target(&td, "Select target creature you control.", 0, 0x81);
      if (result != 0)
      {
        new_pick_target(&td1, "Select target creature opponent controls.", 1, 0x81);
      }
      goto LAB_0230b55a;
    }
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
    }
    else if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
             (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = 1 - player;
    td1.preferred_controller = td1.allowed_controller;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    ptStack_bc = &td1;
    result = validate_target(player, card, &td, 0);
    if ((result != 0) && (result = validate_target(player, card, ptStack_bc, 1), result != 0))
    {
      fight(instance->targets[0].player, instance->targets[0].card, instance->targets[1].player,
            instance->targets[1].card);
    }
    kill_card(player, card, KILL_DESTROY);
  }
LAB_0230b55a:
  ptStack_bc = &td1;
  result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
  can_play_front = 0;
  if (result != 0)
  {
    result = can_target(ptStack_bc);
    can_play_front = (unsigned int)(result != 0);
  }
  result = mdfc_front_impl(player, card, event, can_play_front, 0);
  return result;
}

/* ManalinkEh 2022: 0x230b830; original cards/zendikar_rising.c:1878. */
int card_leyline_tyrant(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  if (event == EVENT_MANA_POOL_DRAINING)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      mana_doesnt_drain_from_pool[player][4] = mana_doesnt_drain_from_pool[player][4] | 0x80;
    }
  }
  result = duh_mode(player);
  result = this_dies_trigger(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (result != 0)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    result = new_pick_target(&td, "TARGET_ANY", 0, 1);
    if (result != 0)
    {

      result =
          charge_mana_for_activated_ability_while_resolving(player, card, event, 0, 0, 0, 0, -1, 0);
      if (0 < result)
      {
        damage_target0(player, card, result);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x230bea0; original cards/zendikar_rising.c:2034. */
int card_makindi_stampede(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
  LAB_0230bed4:
    if (event != EVENT_RESOLVE_SPELL)
      goto LAB_0230bed9;
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
      goto LAB_0230bed9;
    }
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event != EVENT_CAN_CHANGE_TARGET)
      {
        if (event != EVENT_CHANGE_TARGET)
        {
          return 0;
        }
        goto LAB_0230bed9;
      }
      goto LAB_0230bed4;
    }
  }
  pump_creatures_until_eot(player, card, player, 0, 2, 2, 0, 0, NULL);
  kill_card(player, card, KILL_DESTROY);
LAB_0230bed9:
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x2306700; original cards/zendikar_rising.c:2070. */
static int malakir_rebirth_effect(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *pcVar2;
  int result;
  card_instance_t *pcVar4;

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {

    pcVar2 = in_play(player, card);
    if ((((pcVar2 != NULL) && (cVar1 = pcVar2->damage_target_player, cVar1 != -1)) &&
         (pcVar2->damage_target_card == affected_card)) &&
        (cVar1 == affected_card_controller))
    {
      result = is_this_dying((int)cVar1, pcVar2->damage_target_card);
      if (result != 0)
      {
        result = check_for_death_trigger_removal(affected_card_controller, affected_card);
        if (result == 0)
        {

          result = create_legacy_effect(player, card, malakir_rebirth_legacy);
          pcVar4 = get_card_instance(player, result);
          result = get_owner((int)pcVar2->damage_target_player, pcVar2->damage_target_card);
          pcVar4->targets[0].player = result;
          result = get_original_internal_card_id((int)pcVar2->damage_target_player,
                                                 pcVar2->damage_target_card);
          pcVar4->targets[0xb].player = 1;
          pcVar4->targets[0].card = result;
        }
      }
    }
  }
  else if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x230bfa0; original cards/zendikar_rising.c:2089. */
int card_malakir_rebirth(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    default_target_definition(player, card, &td, 2);
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      result = generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL);
      return result;
    }
  joined_r0x0230c003:
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        create_targetted_legacy_effect(player, card, malakir_rebirth_effect,
                                       instance->targets[0].player, instance->targets[0].card);
      }
      kill_card(player, card, KILL_DESTROY);
    }
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 2);
        goto joined_r0x0230c003;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td, 2);
  }
  result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_CREATURE", 1, NULL);
  result = mdfc_front_impl(player, card, event, result, 0);
  return result;
}

/* ManalinkEh 2022: 0x230c650; original cards/zendikar_rising.c:2317. */
int card_nighthawk_scavenger(int player, int card, event_t event)
{
  int result;

  deathtouch_2022(player, card, event);
  lifelink(player, card, event);
  if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
  {
    result = count_types_in_grave(1 - player);
    set_base_pt(result + 1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x230d0f0; original cards/zendikar_rising.c:2542. */
int card_ondu_inversion(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
  LAB_0230d123:
    if (event != EVENT_RESOLVE_SPELL)
      goto LAB_0230d128;
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
      goto LAB_0230d128;
    }
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event != EVENT_CAN_CHANGE_TARGET)
      {
        if (event != EVENT_CHANGE_TARGET)
        {
          return 0;
        }
        goto LAB_0230d128;
      }
      goto LAB_0230d123;
    }
  }
  default_test_definition(&this_test, 1);
  this_test.type_flag = 1;
  new_manipulate_all(player, card, 2, &this_test, ACT_KILL_DESTROY);
  kill_card(player, card, KILL_DESTROY);
LAB_0230d128:
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x230d1f0; original cards/zendikar_rising.c:2584. */
int card_pelakka_predation(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  ec_definition_t ec;
  target_definition_t td;
  test_definition_t this_test;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      target_opponent(player, card);
      goto LAB_0230d239;
    }
  LAB_0230d234:
    if (event != EVENT_RESOLVE_SPELL)
      goto LAB_0230d239;
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
      goto LAB_0230d239;
    }
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event != EVENT_CAN_CHANGE_TARGET)
      {
        if (event != EVENT_CHANGE_TARGET)
        {
          return 0;
        }
        goto LAB_0230d239;
      }
      goto LAB_0230d234;
    }
  }
  default_target_definition(player, card, &td, 2);
  td.zone = 0x1000;
  result = valid_target(&td);
  if (result != 0)
  {

    instance = get_card_instance(player, card);
    default_ec_definition(instance->targets[0].player, player, &ec);
    new_default_test_definition(&this_test, 0xff, "Select a card with CMC 3 or greater.");
    this_test.cmc = 2;
    this_test.cmc_flag = 2;
    new_effect_coercion(&ec, &this_test);
  }
  kill_card(player, card, KILL_DESTROY);
LAB_0230d239:
  result = opponent_is_valid_target(player, card);
  result = mdfc_front_impl(player, card, event, result, 0);
  return result;
}

/* ManalinkEh 2022: 0x230d850; original cards/zendikar_rising.c:2839. */
int card_scourge_of_the_skyclaves(int player, int card, event_t event)
{
  int iVar1;
  unsigned int uVar2;
  int iVar3;
  unsigned int player_00;

  kicker(player, card, event, 4, 1, 0, 0, 0, 0);
  if (event - EVENT_POWER < 2)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar3 = life[1 - player];
      if (life[1 - player] < life[player])
      {
        iVar3 = life[player];
      }
      set_base_pt(0x14 - iVar3);
    }
  }
  else if ((((event != EVENT_CAN_CAST) && (event == EVENT_CAST_SPELL)) &&
            (card == affected_card)) &&
           (player == affected_card_controller))
  {
    iVar1 = kicked(player, card);
    iVar3 = current_turn;
    if (iVar1 != 0)
    {

      uVar2 = round_up_value(life[current_turn]);
      lose_life(iVar3, uVar2);
      player_00 = current_turn ^ 1;
      uVar2 = round_up_value(life[player_00]);
      lose_life(player_00, uVar2);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x230da20; original cards/zendikar_rising.c:2883. */
int card_sea_gate_restoration(int player, int card, event_t event)
{
  int result;

  if ((event == EVENT_PLAY_FIRST_HALF ||
       (event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE)) ||
      (event == EVENT_CAN_CAST))
  {
  LAB_0230da57:
    if (event != EVENT_RESOLVE_SPELL)
      goto LAB_0230da5c;
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
      goto LAB_0230da5c;
    }
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event != EVENT_CAN_CHANGE_TARGET)
      {
        if (event != EVENT_CHANGE_TARGET)
        {
          return 0;
        }
        goto LAB_0230da5c;
      }
      goto LAB_0230da57;
    }
  }
  draw_cards(player, hand_count[player] + 1);
  create_legacy_effect(player, card, card_spellbook);
  kill_card(player, card, KILL_DESTROY);
LAB_0230da5c:
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x230dbe0; original cards/zendikar_rising.c:2964. */
int card_sejiri_shelter(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  target_definition_t td;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      result = generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL);
      return result;
    }
  joined_r0x0230dd08:
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {

        uVar3 = select_a_protection(player);
        pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card,
                               0, 0, uVar3 | 0x10000000, 0);
      }
      kill_card(player, card, KILL_DESTROY);
    }
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 2);
        td.allowed_controller = player;
        td.preferred_controller = player;
        instance = get_card_instance(player, card);
        goto joined_r0x0230dd08;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    get_card_instance(player, card);
  }
  result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_CREATURE", 1, NULL);
  result = mdfc_front_impl(player, card, event, result, 0);
  return result;
}

/* ManalinkEh 2022: 0x230e2d0; original cards/zendikar_rising.c:3085. */
int card_silundi_vision(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
  LAB_0230e303:
    if (event != EVENT_RESOLVE_SPELL)
      goto LAB_0230e308;
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
      goto LAB_0230e308;
    }
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event != EVENT_CAN_CHANGE_TARGET)
      {
        if (event != EVENT_CHANGE_TARGET)
        {
          return 0;
        }
        goto LAB_0230e308;
      }
      goto LAB_0230e303;
    }
  }
  new_default_test_definition(&test, 0x38, "Select an instant or sorcery card.");
  reveal_top_cards_of_library_and_choose(player, card, player, 6, 0, TUTOR_HAND, 1,
                                         TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &test);
  kill_card(player, card, KILL_DESTROY);
LAB_0230e308:
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x230e5d0; original cards/zendikar_rising.c:3147. */
int card_skyclave_cleric(int player, int card, event_t event)
{
  int result;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    gain_life(player, 2);
    return 0;
  }
  if (((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) &&
       event != EVENT_PLAY_FIRST_HALF) &&
      (event != EVENT_CAN_CAST))
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
    }
    else if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
             (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x230e6a0; original cards/zendikar_rising.c:3183. */
int card_skyclave_relic(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  indestructible_2022(player, card, event);
  kicker(player, card, event, 3, 0, 0, 0, 0, 0);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_token(player, card);
    if (result == 0)
    {
      etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    }
  }
  if (event == EVENT_ETB)
  {
    result = kicked(player, card);
    if (result != 0)
    {
      copy_token_definition(player, card, &token, player, card);
      token.action = TOKEN_ACTION_TAPPED;
      token.qty = '\x02';
      generate_token(&token);
    }
  }
  result = plain_mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x230e960; original cards/zendikar_rising.c:3269. */
int card_song_mad_treachery(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 2;
    instance = get_card_instance(player, card);
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL);
      if (spell_fizzled != 1)
      {
        ai_modifier_for_act_of_treason(player, card);
      }
      return 0;
    }
  joined_r0x0230ea89:
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        effect_act_of_treason(player, card, instance->targets[0].player, instance->targets[0].card);
      }
      kill_card(player, card, KILL_DESTROY);
    }
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 2);
        td.preferred_controller = 2;
        instance = get_card_instance(player, card);
        goto joined_r0x0230ea89;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 2;
    get_card_instance(player, card);
  }
  result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_CREATURE", 1, NULL);
  result = mdfc_front_impl(player, card, event, result, 0);
  return result;
}

/* ManalinkEh 2022: 0x230eba0; original cards/zendikar_rising.c:3306. */
int card_soul_shatter(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int iVar5;
  int32_t player_00;
  int32_t card_00;
  int player_01;
  int iStack_d0;
  int iStack_cc;
  target_definition_t td;
  char msg[100];

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

      player_01 = 1 - player;
      pcVar2 = get_card_instance(player, card);
      iVar1 = active_cards_count[player_01] + -1;
      pcVar2->targets[0].card = -1;
      pcVar2->targets[0].player = -1;
      if (iVar1 < 0)
      {
        player_00 = -1;
        card_00 = -1;
      }
      else
      {
        iStack_d0 = 1;
        iStack_cc = -1;
        do
        {
          while (pcVar3 = in_play(player_01, iVar1), pcVar3 == NULL)
          {
          LAB_0230ec80:
            iVar1 = iVar1 + -1;
            if (iVar1 == -1)
              goto LAB_0230ed08;
          }
          iVar4 = is_what(player_01, iVar1, 2);
          iVar5 = is_planeswalker(player_01, iVar1);
          if (iVar4 == 0 && iVar5 == 0)
            goto LAB_0230ec80;
          iVar4 = get_cmc(player_01, iVar1);
          if (iVar4 <= iStack_cc)
          {
            iVar4 = get_cmc(player_01, iVar1);
            iStack_d0 = iStack_d0 + (unsigned int)(iVar4 == iStack_cc);
            goto LAB_0230ec80;
          }
          pcVar2->targets[0].card = iVar1;
          pcVar2->targets[0].player = player_01;
          iStack_cc = get_cmc(player_01, iVar1);
          iVar1 = iVar1 + -1;
          iStack_d0 = 1;
        } while (iVar1 != -1);
      LAB_0230ed08:
        if (1 < iStack_d0)
        {
          default_target_definition(player, card, &td, 0x102);
          td.illegal_abilities = 0;
          td.allow_cancel = 0;
          td.extra = iStack_cc;
          td.special = 0x1000000;
          td.who_chooses = player_01;
          td.allowed_controller = player_01;
          td.preferred_controller = player_01;
          scnprintf(msg, 100, "Select a creature you control with CMC %d.", iStack_cc);
          pcVar2->number_of_targets = '\0';
          new_pick_target(&td, msg, 0, 0x80);
        }
        card_00 = pcVar2->targets[0].card;
        player_00 = pcVar2->targets[0].player;
      }
      kill_card(player_00, card_00, KILL_SACRIFICE);
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x230ee40; original cards/zendikar_rising.c:3363. */
int card_spikefield_hazard(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      result = generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_ANY", 1, NULL);
      return result;
    }
  joined_r0x0230eeaa:
    td.zone = 0x1200;
    if (event == EVENT_RESOLVE_SPELL)
    {
      td.zone = 0x1200;
      result = valid_target(&td);
      if (result != 0)
      {

        result = damage_target0(player, card, 1);
        instance = get_card_instance(player, result);
        instance->targets[3].card = instance->targets[3].card | 0x80;
      }
      kill_card(player, card, KILL_DESTROY);
    }
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 0x102);
        goto joined_r0x0230eeaa;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
  }
  result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_ANY", 1, NULL);
  result = mdfc_front_impl(player, card, event, result, 0);
  return result;
}

/* ManalinkEh 2022: 0x230f1d0; original cards/zendikar_rising.c:3489. */
int card_tangled_florahedron(int player, int card, event_t event)
{
  int result;

  if ((event != EVENT_PLAY_FIRST_HALF &&
       (event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE)) &&
      (event != EVENT_CAN_CAST))
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_0230f258;
    }
    else if ((event != EVENT_RESOLVE_SPELL) &&
             ((event != EVENT_CAN_CHANGE_TARGET && (event != EVENT_CHANGE_TARGET))))
    {
    LAB_0230f258:
      result = mana_producing_creature(player, card, event, 0x18, COLOR_GREEN, 1);
      return result;
    }
  }
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x23066a0; original cards/zendikar_rising.c:3646. */
static int is_instant_sorcery_or_wizard(int iid, int unused, int player, int card)
{
  int result;
  unsigned int uVar2;

  result = is_what(player, card, 0x38);
  uVar2 = 1;
  if (result == 0)
  {
    result = has_subtype(player, card, SUBTYPE_WIZARD);
    uVar2 = (unsigned int)(result != 0);
  }
  return uVar2;
}

/* ManalinkEh 2022: 0x230f550; original cards/zendikar_rising.c:3654. */
int card_umara_wizard(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0xff);
    test.special_selection_function = is_instant_sorcery_or_wizard;
    result = new_specific_spell_played(player, card, event, player, 2, &test);
    if (result != 0)
    {
      pump_ability_until_eot_no_repeat(player, card, player, card, 0x20, 0);
    }
  }
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x23100f0; original cards/zendikar_rising.c:3925. */
int card_zof_consumption(int player, int card, event_t event)
{
  int result;

  if ((event == EVENT_PLAY_FIRST_HALF ||
       (event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE)) ||
      (event == EVENT_CAN_CAST))
  {
  LAB_02310127:
    if (event != EVENT_RESOLVE_SPELL)
      goto LAB_0231012c;
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
      goto LAB_0231012c;
    }
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event != EVENT_CAN_CHANGE_TARGET)
      {
        if (event != EVENT_CHANGE_TARGET)
        {
          return 0;
        }
        goto LAB_0231012c;
      }
      goto LAB_02310127;
    }
  }
  lose_life(1 - player, 4);
  gain_life(player, 4);
  kill_card(player, card, KILL_DESTROY);
LAB_0231012c:
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2308bb0; original cards/zendikar_rising.c:906. */
int card_crawling_barrens(int player, int card, event_t event)
{
  unsigned int uVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  unsigned int uVar5;
  uint32_t uVar6;
  uint32_t uVar7;
  uint32_t uVar8;
  uint32_t uVar9;
  uint32_t uVar10;
  uint32_t uVar11;
  uint32_t uVar12;
  uint32_t uVar13;
  uint32_t uVar14;
  uint32_t uVar15;

  iVar2 = is_humiliated_for_ability(player, card, AL_TRIG);
  if (iVar2 == 0)
  {
    comes_into_play_tapped(player, card, event);
  }
  if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_ACTIVATE))
  {
  LAB_02308beb:
    has_subtype_if_animated_self(player, card, event, SUBTYPE_ELEMENTAL);
    iVar2 = mana_producer(player, card, event);
    return iVar2;
  }

  instance = get_card_instance(player, card);
  uVar1 = instance->token_status;
  iVar2 = paying_mana();
  uVar5 = 0;
  if (iVar2 == 0)
  {
    iVar2 = can_use_activated_abilities(player, card);
    uVar5 = (unsigned int)(iVar2 != 0);
  }
  iVar2 = paying_mana();
  iVar4 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
  uVar15 = 0;
  uVar14 = 0;
  uVar13 = 0;
  uVar12 = 0;
  uVar11 = 0;
  uVar10 = 0;
  uVar9 = 0;
  uVar8 = 0;
  uVar7 = 0;
  uVar6 = 4;
  iVar2 = dialog_impl(player, card, event, "\001\016", "\001\026", "Produce mana", iVar4, iVar2,
                      "Add counters", uVar5, (uVar1 >> 0xc ^ 1) & 1, "\001\020", 4, 0, 0, 0, 0, 0,
                      0, 0, 0, 0);
  if (event != EVENT_CAN_ACTIVATE)
  {
    if (iVar2 != 0)
    {
      if (iVar2 != 2)
        goto LAB_02308beb;
      if (event == EVENT_RESOLVE_ACTIVATION)
      {

        instance = get_card_instance(player, card);
        add_counters_from_source(player, card, instance->parent_controller, instance->parent_card,
                                 COUNTER_P1_P1, 2);
        iVar2 = dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\002", "\001\026",
                            "Animate", 1, 1, 0, 0, 0, 0, uVar6, uVar7, uVar8, uVar9, uVar10, uVar11,
                            uVar12, uVar13, uVar14, uVar15);
        if (iVar2 == 1)
        {
          animate_self(player, card, 0, 0, KEYWORD_0, SP_KEYWORD_0, COLOR_TEST_0, 0);
          return 0;
        }
      }
    }
    iVar2 = 0;
  }
  return iVar2;
}

/* ManalinkEh 2022: 0x2309150; original cards/zendikar_rising.c:1112. */
int card_felidar_retreat(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  int iStack_14;

  landfall(player, card, event);
  if (event == EVENT_ETB)
  {

    result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\003", "\001\014",
                         "Cat Beast", 1, 1, "+1/+1 counters", 1, 1, 0, 0, 0, 0);
    if (result == 1)
    {
      generate_token_by_id(player, card, 0x5527);
    }
    else
    {

      if ((result == 2) && (iStack_14 = active_cards_count[player] + -1, -1 < iStack_14))
      {
        do
        {
          instance = in_play(player, iStack_14);
          if ((instance != NULL) && (result = is_what(player, iStack_14, 2), result != 0))
          {
            add_counters_from_source(player, card, player, iStack_14, COUNTER_P1_P1, 1);
            pump_ability_until_eot_no_repeat(player, card, player, iStack_14, 0, 8);
          }
          iStack_14 = iStack_14 + -1;
        } while (iStack_14 != -1);
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x230c6d0; original cards/zendikar_rising.c:2348. */
int card_nissa_of_shadowed_boughs(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  card_instance_t *pcVar5;
  unsigned int uVar6;
  target_definition_t *ptStack_144;
  test_definition_t *ptStack_140;
  target_definition_t td;
  test_definition_t test;

  landfall(player, card, event);
  if (event == EVENT_ETB)
  {
    if ((trigger_cause_controller != player) || (trigger_cause != card))
    {
      add_counters_from_source(player, card, player, card, COUNTER_LOYALTY, 1);
    }
    check_legend_rule(player, card, EVENT_ETB);
  }
  else
  {
    check_legend_rule(player, card, event);
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition(player, card, &td, 1);
      td.allowed_controller = player;
      default_test_definition(&test, 2);
      iVar1 = count_permanents_by_type(player, TYPE_LAND);
      test.cmc = iVar1 + 1;
      test.cmc_flag = 3;
      test.zone = 0x100;
      pcVar2 = get_card_instance(player, card);
      if (event == EVENT_ACTIVATE)
      {

        pcVar2->number_of_targets = '\0';
        pcVar2->info_slot = 0;
        iVar1 = new_special_count_grave(player, &test);
        uVar6 = 1;
        if (iVar1 == 0)
        {
          iVar1 = check_battlefield_for_special_card(player, card, player, 0, &test);
          uVar6 = (unsigned int)(iVar1 != 0);
        }
        iVar1 = can_target(&td);
        iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                            "Untap & animate a land", iVar1, 5, 1, "Put a creature into play",
                            uVar6, 10, 0xfffffffb, 0, 0, 0, 0);
        if (iVar1 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (iVar1 == 1)
        {
          new_pick_target(&td, "Select a land you control.", 0, 0x81);
        }
        goto LAB_0230c7c0;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_0230c7c0;
      default_target_definition(player, card, &td, 1);
      td.allowed_controller = player;
      default_test_definition(&test, 2);
      iVar1 = count_permanents_by_type(player, TYPE_LAND);
      test.cmc = iVar1 + 1;
      test.cmc_flag = 3;
      test.zone = 0x100;
      pcVar2 = get_card_instance(player, card);
    }
    ptStack_140 = &test;
    ptStack_144 = &td;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      if (pcVar2->info_slot == 1)
      {
        iVar1 = valid_target(ptStack_144);
        if (iVar1 != 0)
        {
          untap_card(pcVar2->targets[0].player, pcVar2->targets[0].card);
          iVar1 = dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\002", "\001\003",
                              "Animate", 1, 10, "Do not animate", 1, 1, 0, 0, 0, 0);
          if (iVar1 == 1)
          {

            pcVar2 = get_card_instance(player, card);
            iVar1 = land_animation2(player, card, pcVar2->targets[0].player,
                                    pcVar2->targets[0].card, 1, 3, 3, 0, 0x10004, 0, NULL);
            pcVar5 = get_card_instance(player, iVar1);
            pcVar5->targets[9].card = 0x3a;
            pcVar5 = get_card_instance(player, iVar1);
            pcVar5->targets[9].player = 1;
            pcVar2 = get_card_instance(pcVar2->targets[0].player, pcVar2->targets[0].card);
            pcVar2->regen_status = pcVar2->regen_status | 0x1f000000;
          }
        }
      }
      else if (pcVar2->info_slot == 2)
      {

        iVar1 = check_battlefield_for_special_card(player, card, player, 0, ptStack_140);
        iVar3 = new_special_count_grave(player, ptStack_140);
        if (iVar1 == 0 && iVar3 == 0)
        {
          return 0;
        }
        do
        {
          if ((iVar1 != 0) &&
              (iVar4 = new_select_a_card(player, player, 4, 0, 2, -1, ptStack_140), -1 < iVar4))
          {
            put_into_play(player, iVar4);
            goto LAB_0230ca8c;
          }
        } while ((iVar3 == 0) ||
                 (iVar4 = new_select_a_card(player, player, 2, 0, 2, -1, ptStack_140), iVar4 < 0));

        iVar4 = reanimate_permanent(player, card, player, iVar4, REANIMATE_DEFAULT);
      LAB_0230ca8c:
        hack_silent_counters = 1;
        add_counters_from_source(player, card, player, iVar4, COUNTER_P1_P1, 2);
        hack_silent_counters = 0;
      }
    }
  }
LAB_0230c7c0:
  iVar1 = planeswalker(player, card, event, 4);
  return iVar1;
}

/* ManalinkEh 2022: 0x230d390; original cards/zendikar_rising.c:2622. */
int card_phylath_world_sculptor(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  landfall(player, card, event);
  if (event == EVENT_ETB)
  {
    if ((trigger_cause_controller == player) && (trigger_cause == card))
    {
      result = count_basic_lands(player, -1);
      generate_tokens_by_id(player, card, 0x4973, result);
    }
    else
    {
      default_target_definition(player, card, &td, 0xff);
      td.required_subtype = 0x96;
      td.allowed_controller = player;
      td.allow_cancel = 0;
      result = can_target(&td);
      if (result != 0)
      {
        result = new_pick_target(&td, "Select a plant you control.", 0, 0x81);
        if (result != 0)
        {

          instance = get_card_instance(player, card);
          add_counters_from_source(player, card, instance->targets[0].player,
                                   instance->targets[0].card, COUNTER_P1_P1, 4);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x230d4d0; original cards/zendikar_rising.c:2655. */
int card_prowling_felidar(int player, int card, event_t event)
{
  vigilance(player, card, event);
  landfall(player, card, event);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  return 0;
}

/* ManalinkEh 2022: 0x230d5b0; original cards/zendikar_rising.c:2780. */
int card_scale_the_heights(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
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
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      td.allow_cancel = 3;
      if ((card == affected_card) && (player == affected_card_controller))
      {
        result = can_target(&td);
        if (result != 0)
        {
          new_pick_target(&td, "TARGET_CREATURE", 0, 0);
        }
        default_test_definition(&test, 1);
        test.zone = 0x100;
        result = check_battlefield_for_special_card(player, card, player, 4, &test);
        ai_modifier = ai_modifier + result * 0xc;
      }
      goto LAB_0230d61e;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      td.allow_cancel = 3;
      if (event == EVENT_RESOLVE_SPELL)
      {

        instance = get_card_instance(player, card);
        if ((instance->number_of_targets != '\0') && (result = valid_target(&td), result != 0))
        {
          add_counters_from_source(player, card, instance->targets[0].player,
                                   instance->targets[0].card, COUNTER_P1_P1, 1);
        }
        gain_life(player, 2);
        result = create_legacy_effect(player, card, check_playable_lands_legacy);
        instance = get_card_instance(player, result);
        result = get_id(player, card);
        instance->targets[2].card = result;
        draw_cards(player, 1);
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_0230d61e;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.allow_cancel = 3;
LAB_0230d61e:
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2306bc0; original cards/zendikar_rising.c:3608. */
static int three_counters_for_mv_less_than_three(int player, int card)
{
  int result;

  result = get_cmc(player, card);
  if (3 < result)
  {
    return 0;
  }
  hack_silent_counters = hack_silent_counters + 1;
  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 3);
  hack_silent_counters = hack_silent_counters + -1;
  return 0;
}

/* ManalinkEh 2022: 0x230f940; original cards/zendikar_rising.c:3757. */
int card_vastwood_fortification(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    default_target_definition(player, card, &td, 2);
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      result = generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL);
      return result;
    }
  joined_r0x0230f9a3:
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 1);
      }
      kill_card(player, card, KILL_DESTROY);
    }
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 2);
        goto joined_r0x0230f9a3;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td, 2);
  }
  result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_ANY", 1, NULL);
  result = mdfc_front_impl(player, card, event, result, 0);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2308870; original cards/zendikar_rising.c:790. */
int card_cleansing_wildfire(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
  LAB_0230891c:
    default_target_definition(player, card, &td, 1);
    if (player != 1)
      goto LAB_023088d0;
  LAB_0230893f:
    td.required_subtype = 0x2000;
    td.special = 0x1000;
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
      goto LAB_0230891c;
    }
    if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      goto LAB_0230891c;
    }
    default_target_definition(player, card, &td, 1);
    if (player == 1)
      goto LAB_0230893f;
  }
  if (event != EVENT_RESOLVE_SPELL)
    goto LAB_023088d0;
  result = valid_target(&td);
  if (result != 0)
  {

    kill_target0(player, card, KILL_DESTROY);
    instance = get_card_instance(player, card);
    result = instance->targets[0].player;
    if (result == 0)
    {
      result = do_dialog(0, player, card, -1, -1, " Tutor a basic land\n Pass", 0);
      if (result != 1)
      {
        result = instance->targets[0].player;
        goto LAB_02308a1a;
      }
    }
    else
    {
    LAB_02308a1a:
      tutor_basic_land(result, 1, 0);
    }
    draw_a_card(player);
  }
  kill_card(player, card, KILL_DESTROY);
LAB_023088d0:
  result = generic_spell(player, card, event, 1, &td, "TARGET_LAND", 1, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x230aab0; original cards/zendikar_rising.c:1560. */
int card_jace_mirror_mage(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    token_generation_t token;
    uint8_t bytes[84];
  } recovered_stack_td;

  kicker(player, card, event, 2, 0, 0, 0, 0, 0);
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = kicked(player, card);
    if (result != 0)
    {
      copy_token_definition(player, card, &recovered_stack_td.token, player, card);
      recovered_stack_td.target.power_requirement = 0x180;
      generate_token(&recovered_stack_td.token);
    }
  LAB_0230ab90:
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_0230ac98;
    default_target_definition(player, card, &recovered_stack_td.target, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Scry 2", 1, 5, 1,
                           "Draw and reveal", 1, 6, 0, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_0230ab27;
    }
  }
  else
  {
    if (event != EVENT_STATIC_EFFECTS)
    {
      if (event == EVENT_CAST_SPELL)
      {
        if ((card == affected_card) && (player == affected_card_controller))
        {

          instance = get_card_instance(player, card);
          result = planeswalker(player, card, EVENT_CAST_SPELL,
                                (unsigned int)(instance->damage_source_card != 0x5568) * 3 + 1);
          return result;
        }
        goto LAB_0230ab27;
      }
      goto LAB_0230ab90;
    }
    result = is_legendary(player, card);
    if (result == 0)
    {
      check_legend_rule(player, card, EVENT_STATIC_EFFECTS);
    }
  LAB_0230ac98:
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_0230ab27;
    default_target_definition(player, card, &recovered_stack_td.target, 2);
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->info_slot == 1)
    {
      scry(player, 2);
    }
    else if (instance->info_slot == 2)
    {

      if (*graveyard_ptr_mutable[player] != -1)
      {
        reveal_card_iid(player, card, *graveyard_ptr_mutable[player]);
      }
      result = draw_a_card(player);
      if (result != -1)
      {

        instance = get_card_instance(player, card);
        result = get_cmc(player, result);
        remove_counters_from_source(player, card, instance->parent_controller,
                                    instance->parent_card, COUNTER_LOYALTY, result);
      }
    }
  }
LAB_0230ab27:
  result = planeswalker(player, card, event, 4);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x230b970; original cards/zendikar_rising.c:1941. */
int card_luminarch_aspirant(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  result = beginning_of_combat(player, card, event, player);
  if (result != 0)
  {

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 0;
    instance->number_of_targets = '\0';
    td.preferred_controller = player;
    td.allowed_controller = player;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "TARGET_CREATURE_YOU_CONTROL", 0, 0);
      if (result != 0)
      {
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 1);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x230ccf0; original cards/zendikar_rising.c:2464. */
int card_obuun_mul_daya_ancestor(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  target_definition_t td;

  check_legend_rule(player, card, event);
  landfall(player, card, event);
  result = beginning_of_combat(player, card, event, player);
  if (result != 0)
  {

    default_target_definition(player, card, &td, 1);
    td.allowed_controller = player;
    pcVar2 = get_card_instance(player, card);
    result = can_target(&td);
    if ((result != 0) &&
        (result = new_pick_target(&td, "Select a land you control.", 0, 0x80), result != 0))
    {

      result = get_power(player, card);
      result = land_animation2(player, card, pcVar2->targets[0].player, pcVar2->targets[0].card, 1,
                               result, result, 0x80, 4, 0, NULL);
      pcVar3 = get_card_instance(player, result);
      pcVar3->targets[9].card = 0x3a;
      pcVar3 = get_card_instance(player, result);
      pcVar3->targets[9].player = 1;
      pcVar3 = get_card_instance(pcVar2->targets[0].player, pcVar2->targets[0].card);
      pcVar3->regen_status = pcVar3->regen_status | 0x1f000000;
      get_abilities(pcVar2->targets[0].player, pcVar2->targets[0].card, EVENT_CHANGE_TYPE, -1);
    }
    pcVar2->number_of_targets = '\0';
  }
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allow_cancel = 0;
    pcVar2 = get_card_instance(player, card);
    result = can_target(&td);
    if ((result != 0) && (result = new_pick_target(&td, "TARGET_CREATURE", 0, 0), result != 0))
    {
      add_counters_from_source(player, card, pcVar2->targets[0].player, pcVar2->targets[0].card,
                               COUNTER_P1_P1, 1);
    }
    pcVar2->number_of_targets = '\0';
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x230bc20; original cards/zendikar_rising.c:1987. */
int card_magmatic_channeler(int player, int card, event_t event)
{
  enum
  {
    TUTOR_EXILE = 6
  };

  int result;
  int iids[2];
  test_definition_t test;

  if (event - EVENT_POWER < 2)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (result = count_graveyard_by_type(player, 0x38), 3 < result))
    {
      event_result = event_result + (unsigned int)(event == EVENT_POWER) * 2 + 1;
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_0230bc4b:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_0230bc50;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_0230bc4b;
  }

  default_test_definition(&test, 0xff);
  iids[0] = *deck_ptr[player];
  iids[1] = deck_ptr[player][1];
  result = reveal_top_cards_of_library_and_choose(player, card, player, 2, 1, TUTOR_EXILE, 1,
                                                  TUTOR_EXILE, 0, &test);
  create_may_play_card_from_exile_effect(
      player, card, player, (unsigned int)cards_data[iids[result]].id, MPCFE_UNTIL_EOT);
LAB_0230bc50:
  result = generic_activated_ability(player, card, event, 0x21, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x230c310; original cards/zendikar_rising.c:2245. */
int card_nahiri_heir_of_the_ancients(int player, int card, event_t event)
{
  enum
  {
    TOKEN_ACTION_EQUIP_ANY = 8
  };

  int result;
  card_instance_t *instance;
  test_definition_t *ptStack_190;
  target_definition_t *ptStack_18c;
  token_generation_t token;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x102);
    default_test_definition(&test, 0xff);
    test.subtype = 0xd2;
    test.sub2 = 0x3001;
    test.subtype_flag = 3;
    if (event == EVENT_ACTIVATE)
    {

      instance = get_card_instance(player, card);
      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Kor token", 1, 5,
                           1, "Dig for a warrior or equpment", 1, 10, 0xfffffffe,
                           "Damage creature or planeswalker", result, 0x14, 0xfffffffd, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 3)
      {
        new_pick_target(&td, "TARGET_CREATURE_OR_PLANESWALKER", 0, 1);
      }
      goto LAB_0230c3ae;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_0230c3ae;
    default_target_definition(player, card, &td, 0x102);
    default_test_definition(&test, 0xff);
  }
  ptStack_18c = &td;
  ptStack_190 = &test;
  test.sub2 = 0x3001;
  test.subtype_flag = 3;
  test.subtype = 0xd2;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    instance = get_card_instance(player, card);
    result = instance->info_slot;
    if (result == 1)
    {
      default_token_definition(player, card, 0x557e, &token);
      token.action = TOKEN_ACTION_EQUIP_ANY;
      generate_token(&token);
    }
    else if (result == 2)
    {
      reveal_top_cards_of_library_and_choose(player, card, player, 6, 0, TUTOR_HAND, 1,
                                             TUTOR_BOTTOM_OF_DECK_RANDOM, 0, ptStack_190);
    }
    else if ((result == 3) && (result = valid_target(ptStack_18c), result != 0))
    {
      result = count_subtype(player, 0xff, 0x3001);
      damage_target0(player, card, result * 2);
    }
  }
LAB_0230c3ae:
  result = planeswalker(player, card, event, 4);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x230c1b0; original cards/zendikar_rising.c:2212. */
int card_myriad_construct(int player, int card, event_t event)
{
  int iVar1;
  const target_t *ptVar2;
  card_instance_t *instance;
  int iVar4;
  token_generation_t token;

  kicker(player, card, event, 3, 0, 0, 0, 0, 0);
  iVar1 = kicked(player, card);
  iVar4 = 0;
  if (iVar1 != 0)
  {
    iVar4 = count_nonbasic_lands(1 - player);
  }
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, iVar4);
  ptVar2 = becomes_target_of_spell(player, card, event, player, card, 2, RESOLVE_TRIGGER_MANDATORY);
  if (ptVar2 != NULL)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      iVar4 = is_humiliated_for_ability(player, card, AL_TRIG);
      if (iVar4 == 0)
      {

        iVar4 = get_power(player, card);
        kill_card(player, card, KILL_SACRIFICE);
        default_token_definition(player, card, 0x4918, &token);
        token.tou = 1;
        token.pow = 1;
        token.qty = (unsigned char)iVar4;
        generate_token(&token);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022 private Party bit-count cache @ 0x2774020; initialized by party_size. */
static int number_bits_set[16];

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int party_size(int player);

/* ManalinkEh 2022: 0x2306c30; original cards/zendikar_rising.c:107. */
int party_size(int player)
{
  int iVar1;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  unsigned int uVar8;
  int iStack_5c;
  int iStack_54;
  int candidates_by_class[16];

  piVar7 = candidates_by_class;

  for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1)
  {
    *piVar7 = 0;
    piVar7 = piVar7 + 1;
  }

  if (active_cards_count[player] < 1)
  {
    iStack_54 = 0;
    iVar5 = 0;
  }
  else
  {
    iVar6 = 0;
    iStack_54 = 0;
    do
    {

      while (((instance = in_play(player, iVar6),
               instance == NULL ||
                   (instance = get_card_instance(player, iVar6), instance->internal_card_id < 0)) ||
              (iVar5 = is_what(player, iVar6, 2), iVar5 == 0)))
      {
      LAB_02306c60:
        iVar6 = iVar6 + 1;
        iVar5 = candidates_by_class[0];
        if (active_cards_count[player] <= iVar6)
          goto LAB_02306d42;
      }
      iVar5 = has_subtype(player, iVar6, SUBTYPE_CLERIC);
      uVar8 = (unsigned int)(iVar5 != 0);
      iVar5 = has_subtype(player, iVar6, SUBTYPE_ROGUE);
      if (iVar5 != 0)
      {
        uVar8 = uVar8 | 4;
      }
      iVar5 = has_subtype(player, iVar6, SUBTYPE_WARRIOR);
      if (iVar5 == 0)
      {
        iVar5 = has_subtype(player, iVar6, SUBTYPE_WIZARD);
        if (iVar5 != 0)
          goto LAB_02306d1b;
        if (uVar8 == 0)
          goto LAB_02306c60;
        candidates_by_class[uVar8] = candidates_by_class[uVar8] + 1;
      }
      else
      {
        uVar8 = uVar8 | 8;
        iVar5 = has_subtype(player, iVar6, SUBTYPE_WIZARD);
        if (iVar5 != 0)
        {
        LAB_02306d1b:
          uVar8 = uVar8 | 2;
        }
        candidates_by_class[uVar8] = candidates_by_class[uVar8] + 1;
      }
      iStack_54 = iStack_54 + 1;
      iVar6 = iVar6 + 1;
      iVar5 = candidates_by_class[0];
    } while (iVar6 < active_cards_count[player]);
  }
LAB_02306d42:

  uVar8 = 0;
  do
  {
    uVar3 = num_bits_set(uVar8);
    number_bits_set[uVar8] = uVar3;
    uVar8 = uVar8 + 1;
  } while (uVar8 != 0x10);
  iVar6 = 0;
  while (true)
  {
    iVar1 = number_bits_set[iVar6];
    if (iVar1 < iVar5)
    {
      candidates_by_class[iVar6] = iVar1;
      iStack_54 = iStack_54 - (iVar5 - iVar1);
    }
    iVar6 = iVar6 + 1;
    if (iVar6 == 0x10)
      break;
    iVar5 = candidates_by_class[iVar6];
  }
  iStack_5c = 3;
  uVar8 = 1;
  do
  {
    uVar3 = uVar8 * 2;
    if ((int)uVar3 < 9)
    {
      uVar4 = uVar3;
      do
      {
        while (true)
        {
          iVar5 = candidates_by_class[uVar8];
          iVar6 = candidates_by_class[uVar8 | uVar4];
          if ((iVar5 == 0) || (candidates_by_class[uVar4] == 0))
            break;
          if (iVar6 != 0)
          {
            iStack_54 = iStack_54 - iVar6;
            candidates_by_class[uVar8 | uVar4] = 0;
          }
        LAB_02306dbb:
          uVar4 = uVar4 * 2;
          if (8 < (int)uVar4)
            goto LAB_02306dfb;
        }
        if (iVar6 < 2)
          goto LAB_02306dbb;
        piVar7 = candidates_by_class + uVar4;
        candidates_by_class[uVar8] = 0;
        uVar4 = uVar4 * 2;
        iStack_54 = (iStack_54 - iVar5) - *piVar7;
        *piVar7 = 0;
      } while ((int)uVar4 < 9);
    }
  LAB_02306dfb:
    iStack_5c = iStack_5c + -1;
    uVar8 = uVar3;
    if (iStack_5c == 0)
    {
      uVar3 = 0;
      uVar8 = 0;
      do
      {
        if (candidates_by_class[uVar8] != 0)
        {
          uVar3 = uVar3 | uVar8;
        }
        uVar8 = uVar8 + 1;
      } while (uVar8 != 0x10);

      iVar5 = number_bits_set[uVar3];
      if (iStack_54 < number_bits_set[uVar3])
      {
        iVar5 = iStack_54;
      }
      return iVar5;
    }
  } while (true);
}

/* ManalinkEh 2022: 0x230fb60; original cards/zendikar_rising.c:3801. */
int card_veteran_adventurer(int player, int card, event_t event)
{
  int result;

  vigilance(player, card, event);
  if (event != EVENT_MODIFY_COST)
  {
    return 0;
  }
  result = party_size(player);
  COST_COLORLESS = COST_COLORLESS - (char)result;
  return 0;
}

/* ManalinkEh 2022: 0x230fd40; original cards/zendikar_rising.c:3867. */
int card_zagras_thief_of_heartbeats(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  deathtouch_2022(player, card, event);
  haste(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    result = party_size(player);
    COST_COLORLESS = COST_COLORLESS - (char)result;
  }
  else if (((event == EVENT_ABILITIES) && (affected_card_controller == player)) &&
           (affected_card != card))
  {
    result = is_what(player, affected_card, 2);
    if (result != 0)
    {
      deathtouch_2022(affected_card_controller, affected_card, EVENT_ABILITIES);
    }
  }
  instance = combat_damage_being_dealt(event);
  if (instance != NULL)
  {
    result = damage_is_to_planeswalker(instance);
    if ((result != 0) && (instance->damage_source_player == player))
    {
      result = is_what(player, instance->damage_source_card, 2);
      if (result != 0)
      {
        kill_card(instance->targets[4].player, instance->targets[4].card, KILL_DESTROY);
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2307cd0; original cards/zendikar_rising.c:393. */
int card_ancient_greenwarden(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_STATIC_EFFECTS)
  {
    card_counts[player][1] = card_counts[player][1] + '\x01';
  }
  result = card_crucible_of_worlds(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x230b190; original cards/zendikar_rising.c:1750. */
int card_kazuuls_fury(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar1;
  unsigned int uVar2;
  uint32_t data;
  unsigned int card_00;
  inst_info_t iVar3;
  target_definition_t td1;
  test_definition_t test;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    default_target_definition(player, card, &td1, 0x102);
    td1.zone = 0x1200;
    instance = get_card_instance(player, card);
    if (event == EVENT_PLAY_FIRST_HALF)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {

        instance->number_of_targets = '\0';
        new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
        uVar2 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                              &test);
        if (uVar2 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        iVar1 = pick_target(&td1, "TARGET_ANY");
        card_00 = uVar2 >> 0x18;
        uVar2 = (int)uVar2 >> 0x10 & 0xff;
        if (iVar1 == 0)
        {
          state_untargettable(uVar2, card_00, 1);
        }
        else
        {
          data = get_power(uVar2, card_00);
          set_inst_info(instance, 0x5577, data, 0);
          kill_card(uVar2, card_00, KILL_SACRIFICE);
        }
      }
      goto LAB_0230b216;
    }
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
    }
    else if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
             (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
    default_target_definition(player, card, &td1, 0x102);
    td1.zone = 0x1200;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    iVar1 = valid_target(&td1);
    if (iVar1 != 0)
    {
      iVar3 = inst_info(instance, 0x5577);
      damage_target0(player, card, iVar3.data);
    }
    kill_card(player, card, KILL_DESTROY);
  }
LAB_0230b216:
  iVar1 = generic_spell(player, card, EVENT_CAN_CAST, 0x401, &td1, NULL, 1, NULL);
  iVar1 = mdfc_front_impl(player, card, event, iVar1, 0);
  return iVar1;
}

/* ManalinkEh 2022: 0x230cf60; original cards/zendikar_rising.c:2504. */
int card_omnath_locus_of_creation(int player, int card, event_t event)
{
  uint16_t uVar1;
  card_instance_t *instance;
  inst_info_t result;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  landfall(player, card, event);
  if (event == EVENT_ETB)
  {
    if ((trigger_cause_controller == player) && (trigger_cause == card))
    {
      draw_a_card(player);
    }
    else
    {

      instance = get_card_instance(player, card);
      increment_data2(instance, 0x55ae);
      result = inst_info(instance, 0x55ae);
      uVar1 = result.data2;
      if (uVar1 == 2)
      {
        produce_mana_multi(player, 0, 0, 1, 1, 1, 1);
      }
      else if (uVar1 == 3)
      {
        new_damage_all(player, card, 1 - player, 4, 0x22, NULL);
      }
      else if (uVar1 == 1)
      {
        gain_life(player, 4);
      }
    }
  }
  else if (event == EVENT_CLEANUP)
  {

    instance = get_card_instance(player, card);
    clear_info(instance, 0x55ae);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2307900; original cards/zendikar_rising.c:302. */
int card_agadeems_awakening(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int *piVar5;
  int *piVar6;
  inst_info_t iVar7;
  int iStack_18c;
  int chosen[2][19];
  test_definition_t test;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    new_default_test_definition(&test, 0x47, "Select a permanent card.");
    if (event == EVENT_PLAY_FIRST_HALF)
    {

      instance = get_card_instance(player, card);
      iStack_18c = instance->info_slot;
      test.cmc_flag = 3;
      test.cmc = iStack_18c + 1;
      test.no_shuffle = 1;
      test.value_for_special_selection_function = (int)(chosen + 1);
      test.special_selection_function = test_not_in_array;

      iVar1 = graveyard_has_shroud(player);
      if (iVar1 == 0)
      {
        iVar1 = 0;
        if (0x13 < iStack_18c)
        {
          iStack_18c = 0x13;
        }

        while ((
            (iVar3 = new_select_a_card(player, player, 5, 0, 5, 0, &test),
             iVar3 != -1 && (iVar4 = new_special_count_grave(player, &test), iVar1 < iStack_18c)) &&
            (iVar4 != 0)))
        {
          instance->targets[iVar1].player = iVar3;
          chosen[0][iVar1] = iVar3;
          iVar3 = turn_card_in_grave_face_down(player, iVar3);
          instance->targets[iVar1].card = iVar3;
          chosen[1][iVar1] = iVar3;
          iVar1 = iVar1 + 1;
        }
        if (0 < iVar1)
        {
          piVar5 = chosen[0];
          do
          {
            piVar6 = piVar5 + 1;
            turn_card_in_grave_face_up(player, *piVar5, piVar5[0x13]);
            piVar5 = piVar6;
          } while (piVar6 != chosen[0] + iVar1);
        }
        descending_qsort_on_player(iVar1, instance->targets);
        set_inst_info(instance, 0x5501, 0, (uint16_t)iVar1);
      }
      goto LAB_0230796f;
    }
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
    }
    else if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
             (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
    new_default_test_definition(&test, 0x47, "Select a permanent card.");
  }
  if (event == EVENT_RESOLVE_SPELL)
  {

    instance = get_card_instance(player, card);
    for (iVar1 = 0; iVar7 = inst_info(instance, 0x5501),
        iVar1 < (int)(((uint32_t)iVar7.id | ((uint32_t)iVar7.data2 << 16)) >> 0x10);
         iVar1 = iVar1 + 1)
    {

      iVar3 = validate_target_from_grave(player, card, player, iVar1);
      if (iVar3 != -1)
      {
        reanimate_permanent(player, card, player, iVar3, REANIMATE_DEFAULT);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
LAB_0230796f:
  iVar1 = generic_spell(player, card, EVENT_CAN_CAST, 0xc, NULL, NULL, 1, &test);
  iVar1 = mdfc_front_impl(player, card, event, iVar1, 0);
  return iVar1;
}

/* ManalinkEh 2022: 0x230de40; original cards/zendikar_rising.c:3022. */
int card_shatterskull_smashing(int player, int card, event_t event)
{
  const char *pcVar1;
  int iVar2;
  card_instance_t *instance;
  uint32_t data;
  unsigned int uVar4;
  uint32_t argument3;
  unsigned int uVar5;
  inst_info_t iVar6;
  uint16_t uStack_164;
  target_definition_t td;
  char buffer[250];

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
    default_target_definition(player, card, &td, 0x102);
    td.allow_cancel = 3;
    if (event == EVENT_PLAY_FIRST_HALF)
    {

      pick_up_to_n_targets(&td, "TARGET_CREATURE_OR_PLANESWALKER", 2);
      instance = get_card_instance(player, card);
      if (instance->number_of_targets == '\0')
      {
        argument3 = instance->info_slot;
        spell_fizzled = 1;
        if (5 < (int)argument3)
        {
        LAB_0230e040:
          argument3 = argument3 * 2;
          goto LAB_0230e042;
        }
        uStack_164 = 0;
        data = argument3;
      }
      else
      {
        argument3 = instance->info_slot;
        if (5 < (int)argument3)
          goto LAB_0230e040;
      LAB_0230e042:
        uStack_164 = 0;
        data = argument3;
        if (instance->number_of_targets == '\x02')
        {
          iVar2 = get_id(instance->targets[0].player, instance->targets[0].card);
          scnprintf(buffer, 0xfa, "Deal how much damage to %s?", cards_ptr[iVar2]->name);
          data = choose_a_number(player, buffer, argument3);
          iVar2 = get_id(instance->targets[1].player, instance->targets[1].card);
          pcVar1 = cards_ptr[iVar2]->name;
          iVar2 = get_id(instance->targets[0].player, instance->targets[0].card);
          scnprintf(buffer, 0xfa, "%d damage will be dealt to\n %s\n and %d will be dealt to\n %s.",
                    data, cards_ptr[iVar2]->name, argument3 - data, pcVar1);

          uVar5 = 0;
          do
          {
            uVar4 = current_turn ^ uVar5;
            uVar5 = uVar5 + 1;
            dialog_impl(player, card, EVENT_PLAY_FIRST_HALF, "\001\011", "", "\001\014", "\001\003",
                        "\001\010", uVar4, buffer, 1, 1, 0, 0, 0, 0);
          } while (uVar5 != 2);
          uStack_164 = (uint16_t)(argument3 - data);
        }
      }
      set_inst_info(instance, 0x55dd, data, uStack_164);
      goto LAB_0230debc;
    }
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
    }
    else if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
             (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x102);
  }
  td.allow_cancel = 3;
  if (event == EVENT_RESOLVE_SPELL)
  {

    td.allow_cancel = 3;
    instance = get_card_instance(player, card);
    iVar2 = valid_target(&td);
    if (iVar2 != 0)
    {
      iVar6 = inst_info(instance, 0x55dd);
      damage_target0(player, card, iVar6.data);
    }
    iVar2 = validate_target(player, card, &td, 1);
    if (iVar2 != 0)
    {
      iVar6 = inst_info(instance, 0x55dd);
      damage_creature(instance->targets[1].player, instance->targets[1].card,
                      ((uint32_t)iVar6.id | ((uint32_t)iVar6.data2 << 16)) >> 0x10, player, card);
    }
    kill_card(player, card, KILL_DESTROY);
  }
LAB_0230debc:
  iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
  iVar2 = mdfc_front_impl(player, card, event, iVar2, 0);
  return iVar2;
}

/* ManalinkEh 2022: 0x230e400; original cards/zendikar_rising.c:3114. */
int card_skyclave_apparition(int player, int card, event_t event)
{
  int iVar1;
  uint32_t data;
  card_instance_t *instance;
  inst_info_t iVar3;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    token_generation_t token;
    uint8_t bytes[84];
  } recovered_stack_td;

  iVar1 = duh_mode(player);
  etb_ability(player, card, event, (iVar1 != 0) + RESOLVE_TRIGGER_OPTIONAL);
  leaves_play_2022(player, card, event, 1, 1);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &recovered_stack_td.target, 0x47);
    recovered_stack_td.target.allowed_controller = 1 - trigger_cause_controller;
    recovered_stack_td.target.illegal_type = 0x1001;
    recovered_stack_td.target.extra = 4;
    recovered_stack_td.target.special = 0x800;
    iVar1 = can_target(&recovered_stack_td.target);
    if (iVar1 != 0)
    {
      iVar1 = new_pick_target(
          &recovered_stack_td.target,
          "Select a nonland, nontoken permanent you don\'t control with mana value 4 or less.", 0,
          0x80);
      if (iVar1 != 0)
      {
        instance = get_card_instance(player, card);
        iVar1 = get_owner(instance->targets[0].player, instance->targets[0].card);
        data = get_cmc(instance->targets[0].player, instance->targets[0].card);
        set_inst_info(instance, 0x55e4, data, (uint16_t)iVar1);
        kill_target0(player, card, KILL_EXILE);
      }
    }
  }
  else if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    instance = get_card_instance(player, card);
    default_token_definition(player, card, 0x4951, &recovered_stack_td.token);
    iVar3 = inst_info(instance, 0x55e4);
    recovered_stack_td.target.required_abilities = iVar3.data;
    recovered_stack_td.target.illegal_abilities = recovered_stack_td.target.required_abilities;
    iVar3 = inst_info(instance, 0x55e4);
    recovered_stack_td.target.required_subtype = 0x42;
    recovered_stack_td.target.illegal_type =
        ((uint32_t)iVar3.id | ((uint32_t)iVar3.data2 << 16)) >> 0x10;
    generate_token(&recovered_stack_td.token);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2309d50; original cards/zendikar_rising.c:1381. */
int card_inscription_of_insight(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  unsigned int uVar3;
  card_instance_t *instance;
  unsigned int uVar5;
  inst_info_t iVar6;
  unsigned int uStack_108;
  token_generation_t token;
  target_definition_t td_creature;
  target_definition_t td_player;

  kicker(player, card, event, 2, 0, 2, 0, 0, 0);
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
      default_target_definition(player, card, &td_creature, 2);
      default_target_definition(player, card, &td_player, 0);
      td_player.zone = 0x1000;
      if ((card == affected_card) && (player == affected_card_controller))
      {
        uStack_108 = 0;
        do
        {
          iVar1 = kicked(player, card);
          iVar2 = can_target(&td_player);
          uVar5 = 0;
          if (iVar2 != 0)
          {
            uVar5 = ((unsigned char)uStack_108 >> 3 ^ 1) & 1;
          }
          if (((ai_is_speculating == 1) ||
               ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))) &&
              (iVar2 = can_target(&td_creature), iVar2 == 0))
          {
            uVar3 = 0;
          }
          else
          {
            uVar3 = (uStack_108 >> 1 ^ 1) & 1;
          }
          iVar1 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\004",
                              "Bounce creatures", uVar3, 1, "Scry 2 & draw",
                              ((unsigned char)uStack_108 >> 2 ^ 1) & 1, 1, "Illusion token", uVar5,
                              1, "Done", iVar1, 0xfffffffb, 0, 0, 0, 0);
          if (2 < iVar1 - 1U)
          {
            iVar2 = kicked(player, card);
            if (iVar2 != 0)
            {
              if (iVar1 == 0)
              {
                spell_fizzled = 1;
                return 0;
              }
              if (iVar1 < 4)
                goto LAB_02309eeb;
            }
            if (iVar1 == 0)
            {
              spell_fizzled = 1;
              return 0;
            }
            break;
          }

          uStack_108 =
              (unsigned int)(unsigned char)((unsigned char)uStack_108 |
                                            (unsigned char)(1 << ((unsigned char)iVar1 & 0x1f)));
          iVar1 = kicked(player, card);
          if (iVar1 == 0)
            break;
        LAB_02309eeb:
          uVar5 = num_bits_set((int)(char)(unsigned char)uStack_108);
        } while (uVar5 < 3);
        if ((uStack_108 & 2) != 0)
        {

          instance = get_card_instance(player, card);
          iVar1 = new_pick_target(&td_creature, "Select a creature to bounce.", 0, 0x80);
          if (iVar1 != 0)
          {
            state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
            iVar1 = can_target(&td_creature);
            if (iVar1 != 0)
            {
              new_pick_target(&td_creature, "Select a creature to bounce.", 1, 0x80);
            }
            state_untargettable(instance->targets[0].player, instance->targets[0].card, 0);
          }
        }
        if ((uStack_108 & 8) != 0)
        {
          new_pick_target(&td_player, "TARGET_PLAYER", 3, 1);
        }
        instance = get_card_instance(player, card);
        set_inst_info(instance, 0x5565, 0, (short)(char)(unsigned char)uStack_108);
      }
      goto LAB_02309e19;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td_creature, 2);
      default_target_definition(player, card, &td_player, 0);
      td_player.zone = 0x1000;
      if (event == EVENT_RESOLVE_SPELL)
      {

        instance = get_card_instance(player, card);
        iVar6 = inst_info(instance, 0x5565);
        uVar5 = ((uint32_t)iVar6.id | ((uint32_t)iVar6.data2 << 16)) >> 0x10;
        if (((((uint64_t)iVar6.id | ((uint64_t)iVar6.data2 << 16) | ((uint64_t)iVar6.data << 32)) &
              0x20000) != 0) &&
            (iVar1 = validate_arbitrary_target(&td_creature, instance->targets[0].player,
                                               instance->targets[0].card),
             iVar1 != 0))
        {
          bounce_permanent(instance->targets[0].player, instance->targets[0].card);
          iVar1 = validate_arbitrary_target(&td_creature, instance->targets[1].player,
                                            instance->targets[1].card);
          if (iVar1 != 0)
          {
            bounce_permanent(instance->targets[1].player, instance->targets[1].card);
          }
        }
        if ((uVar5 & 4) != 0)
        {
          scry(player, 2);
          draw_cards(player, 2);
        }
        if (((uVar5 & 8) != 0) &&
            (iVar1 = validate_arbitrary_target(&td_player, instance->targets[3].player,
                                               instance->targets[3].card),
             iVar1 != 0))
        {
          default_token_definition(player, card, 0x4951, &token);
          token.t_player = instance->targets[3].player;
          token.pow = hand_count[token.t_player];
          token.special_infos = 0x42;
          token.tou = token.pow;
          generate_token(&token);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_02309e19;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td_creature, 2);
  default_target_definition(player, card, &td_player, 0);
  td_player.zone = 0x1000;
LAB_02309e19:
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x230a390; original cards/zendikar_rising.c:1457. */
int card_inscription_of_ruin(int player, int card, event_t event)
{
  int32_t iVar1;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  card_instance_t *instance;
  unsigned char bVar6;
  unsigned int uVar7;
  unsigned int uVar8;
  inst_info_t iVar9;
  target_definition_t td_creature;
  target_definition_t td_player;
  test_definition_t test_grave;

  kicker(player, card, event, 2, 2, 0, 0, 0, 0);
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td_creature, 2);
    td_creature.extra = 3;
    td_creature.special = 0x800;
    default_target_definition(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    new_default_test_definition(&test_grave, 2, "Select a creature card.");
    test_grave.cmc = 3;
    test_grave.cmc_flag = 3;
    iVar2 = opponent_is_valid_target(player, card);
    if (((iVar2 == 0) && (iVar2 = new_special_count_grave(player, &test_grave), iVar2 == 0)) &&
        (iVar2 = can_target(&td_creature), iVar2 == 0))
    {
      return 0;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
    default_target_definition(player, card, &td_creature, 2);
    td_creature.extra = 3;
    td_creature.special = 0x800;
    default_target_definition(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    new_default_test_definition(&test_grave, 2, "Select a creature card.");
    test_grave.cmc = 3;
    test_grave.cmc_flag = 3;
    if ((card == affected_card) && (player == affected_card_controller))
    {
      bVar6 = 0;
      do
      {
        iVar2 = kicked(player, card);
        iVar3 = can_target(&td_creature);
        uVar7 = 0;
        if (iVar3 != 0)
        {
          uVar7 = (bVar6 >> 3 ^ 1) & 1;
        }
        iVar3 = new_special_count_grave(player, &test_grave);
        uVar8 = 0;
        if (iVar3 != 0)
        {
          uVar8 = (bVar6 >> 2 ^ 1) & 1;
        }
        iVar3 = opponent_is_valid_target(player, card);
        uVar4 = 0;
        if (iVar3 != 0)
        {
          uVar4 = (bVar6 >> 1 ^ 1) & 1;
        }
        iVar2 =
            dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\004", "Opponent discards",
                        uVar4, 1, "Return creature from grave", uVar8, 1, "Kill creature cmc <= 3",
                        uVar7, 1, "Done", iVar2, 0xfffffffb, 0, 0, 0, 0);
        if (2 < iVar2 - 1U)
        {
          iVar3 = kicked(player, card);
          if (iVar3 != 0)
          {
            if (iVar2 == 0)
            {
              spell_fizzled = 1;
              return 0;
            }
            if (iVar2 < 4)
              goto LAB_0230a5d3;
          }
          if (iVar2 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          break;
        }

        bVar6 = bVar6 | (unsigned char)(1 << ((unsigned char)iVar2 & 0x1f));
        iVar2 = kicked(player, card);
        if (iVar2 == 0)
          break;
      LAB_0230a5d3:
        uVar7 = num_bits_set((int)(char)bVar6);
      } while (uVar7 < 3);
      instance = get_card_instance(player, card);
      if ((bVar6 & 2) != 0)
      {
        instance->number_of_targets = '\0';
        target_opponent(player, card);
        iVar1 = instance->targets[0].card;
        instance->targets[1].player = instance->targets[0].player;
        instance->targets[1].card = iVar1;
      }
      if ((bVar6 & 4) != 0)
      {
        instance->number_of_targets = '\0';
        generic_spell(player, card, EVENT_CAST_SPELL, 8, NULL, NULL, 1, &test_grave);
        if (spell_fizzled != 1)
        {

          ai_modifier =
              ai_modifier +
              cards_ptr[graveyard_ptr_mutable[player][instance->targets[0].player]]->ai_base_value;
        }
      }
      if ((bVar6 & 8) != 0)
      {
        new_pick_target(&td_creature, "Select a creature with mana value 3 or less.", 3, 0x81);
      }
      set_inst_info(instance, 0x5566, 0, (short)(char)bVar6);
    }
  }
  else
  {
    if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
        (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
    default_target_definition(player, card, &td_creature, 2);
    td_creature.extra = 3;
    td_creature.special = 0x800;
    default_target_definition(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    new_default_test_definition(&test_grave, 2, "Select a creature card.");
    test_grave.cmc = 3;
    test_grave.cmc_flag = 3;
    if (event == EVENT_RESOLVE_SPELL)
    {

      instance = get_card_instance(player, card);
      iVar9 = inst_info(instance, 0x5566);
      uVar7 = ((uint32_t)iVar9.id | ((uint32_t)iVar9.data2 << 16)) >> 0x10;
      if (((((uint64_t)iVar9.id | ((uint64_t)iVar9.data2 << 16) | ((uint64_t)iVar9.data << 32)) &
            0x20000) != 0) &&
          (iVar2 = validate_arbitrary_target(&td_player, instance->targets[1].player,
                                             instance->targets[1].card),
           iVar2 != 0))
      {
        discard(instance->targets[1].player, 2, 2);
      }

      if (((uVar7 & 4) != 0) &&
          (iVar2 = validate_target_from_grave_source(player, card, player, 0), iVar2 != -1))
      {
        reanimate_permanent(player, card, player, iVar2, REANIMATE_DEFAULT);
      }
      if (((uVar7 & 8) != 0) &&
          (iVar2 = validate_arbitrary_target(&td_creature, instance->targets[3].player,
                                             instance->targets[3].card),
           iVar2 != 0))
      {
        kill_card(instance->targets[3].player, instance->targets[3].card, KILL_DESTROY);
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  iVar2 = basic_spell(player, card, event);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2308fb0; original cards/zendikar_rising.c:1042. */
int mdfc_back_boltland(int player, int card, event_t event)
{
  int result;

  card_generic_boltland(player, card, event);
  result = mdfc_back(player, card, event);
  return result;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x230f420; original cards/zendikar_rising.c:3616. */
int card_turntimber_symbiosis(int player, int card, event_t event)
{
  int result;
  test_definition_2022_t test;

  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
  LAB_0230f453:
    if (event != EVENT_RESOLVE_SPELL)
      goto LAB_0230f458;
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
      goto LAB_0230f458;
    }
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event != EVENT_CAN_CHANGE_TARGET)
      {
        if (event != EVENT_CHANGE_TARGET)
        {
          return 0;
        }
        goto LAB_0230f458;
      }
      goto LAB_0230f453;
    }
  }
  default_test_definition(&test, 2);
  reveal_top_cards_of_library_and_choose_fn_2022(player, card, player, 7, 0, TUTOR2022_PLAY, 1,
                                                 TUTOR2022_BOTTOM_OF_DECK_RANDOM, 0, &test,
                                                 three_counters_for_mv_less_than_three);
  kill_card(player, card, KILL_DESTROY);
LAB_0230f458:
  result = mdfc_front_impl(player, card, event, 1, 0);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x230e7c0; original cards/zendikar_rising.c:3214. */
int card_skyclave_shade(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_data_t *cd;
  cannot_block(player, card, event);
  kicker(player, card, event, 2, 1, 0, 0, 0, 0);
  iVar1 = kicked(player, card);
  iVar2 = 0;
  if (iVar1 != 0)
  {
    iVar2 = 2;
  }
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, iVar2);
  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    iVar2 = get_id(player, card);
    iVar2 = has_mana_to_cast_id(player, EVENT_GRAVEYARD_ABILITY, iVar2);
    if (((iVar2 != 0) && (current_turn == player)) && (0 < lands_played))
    {
      return 2;
    }
  }
  else if (event == EVENT_PAY_FLASHBACK_COSTS)
  {
    cd = get_card_data(player, card);
    iVar2 = charge_spell_cost_2022(cd, player, card);
    return (unsigned int)(iVar2 != 0);
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x230f7b0; original cards/zendikar_rising.c:3723. */
int card_valakut_exploration(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iStack_18;
  landfall(player, card, event);
  if ((event == EVENT_ETB) && (iVar1 = *deck_ptr[player], iVar1 != -1))
  {
    rfg_top_card_of_deck(player);
    iVar1 = create_may_play_card_from_exile_effect(player, card, player,
                                                        (unsigned int)cards_data[iVar1].id, 0);
    instance = get_card_instance(player, iVar1);
    instance->eot_toughness = 0x561c;
  }
  iVar1 = eot_trigger(player, card, event);
  if ((iVar1 != 0) && (0 < active_cards_count[player]))
  {
    iStack_18 = 0;
    iVar1 = 0;
    do
    {
      instance = in_play(player, iVar1);
      if ((((instance != NULL) && (iVar3 = is_what(player, iVar1, 0x80), iVar3 != 0)) &&
           (instance->info_slot_fn == effect_may_play_card_from_exile)) &&
          (instance->eot_toughness == 0x561c))
      {
        iStack_18 = iStack_18 + 1;
        iVar3 = get_internal_card_id_from_csv_id(instance->targets[0].card);
        iVar3 = find_iid_in_rfg(player, iVar3);
        if (iVar3 != -1)
        {
          from_exile_to_graveyard(player, iVar3);
        }
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < active_cards_count[player]);
    if (iStack_18 != 0)
    {
      damage_player(1 - player, iStack_18, player, card);
    }
  }
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x230f630; original cards/zendikar_rising.c:3682. */
int card_valakut_awakening(int player, int card, event_t event)
{
  int iVar1;
  int iVar3;
  char msg[100];
  test_definition_2022_t this_test;
  if (((event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE) ||
       event == EVENT_PLAY_FIRST_HALF) ||
      (event == EVENT_CAN_CAST))
  {
  LAB_0230f666:
    if (event != EVENT_RESOLVE_SPELL)
      goto LAB_0230f66c;
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
      goto LAB_0230f66c;
    }
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event != EVENT_CAN_CHANGE_TARGET)
      {
        if (event != EVENT_CHANGE_TARGET)
        {
          return 0;
        }
        goto LAB_0230f66c;
      }
      goto LAB_0230f666;
    }
  }
  memset(msg, 0, sizeof msg);
  memcpy(msg, "Select a card to put on the bottom of your library.", 52);
  iVar3 = 0;
  new_default_test_definition(&this_test, 0xff, msg);
  if (0 < hand_count[player])
  {
    do
    {
      iVar1 = new_select_a_card(player, player, 4, 0, 3, -1, &this_test);
      if (iVar1 == -1)
        break;
      put_on_bottom_of_deck(player, iVar1);
      iVar3 = iVar3 + 1;
    } while (0 < hand_count[player]);
    if (0 < iVar3)
    {
      draw_cards(player, iVar3 + 1);
    }
  }
  kill_card(player, card, KILL_DESTROY);
LAB_0230f66c:
  iVar1 = mdfc_front_impl(player, card, event, 1, 0);
  return iVar1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x23095c0; original cards/zendikar_rising.c:1306. */
int card_inscription_of_abundance(int player, int card, event_t event)
{
  int iVar1;
  unsigned int uVar2;
  card_instance_t *instance;
  int iVar4;
  unsigned int uVar5;
  unsigned int uVar8;
  inst_info_t iVar10;
  unsigned int uStack_17c;
  unsigned int uStack_16c;
  target_definition_2022_t td_creature;
  target_definition_2022_t td_control;
  target_definition_2022_t td_dont_control;
  target_definition_2022_t td_player;
  kicker(player, card, event, 2, 0, 0, 1, 0, 0);
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td_creature, 2);
    td_dont_control = td_creature;
    td_control = td_dont_control;
    td_dont_control.allowed_controller = 1 - player;
    td_player = td_control;
    td_control.allowed_controller = player;
    td_player.zone = 0x1000;
    iVar4 = can_target(&td_creature);
    if (((iVar4 != 0) || (iVar4 = can_target(&td_player), iVar4 != 0)) ||
        ((iVar4 = can_target(&td_control),
          iVar4 != 0 && (iVar4 = can_target(&td_dont_control), iVar4 != 0))))
      goto LAB_023096c9;
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td_creature, 2);
      td_dont_control = td_creature;
      td_control = td_dont_control;
      td_dont_control.allowed_controller = 1 - player;
      td_player = td_control;
      td_control.allowed_controller = player;
      td_player.zone = 0x1000;
      if ((card == affected_card) && (player == affected_card_controller))
      {
        uStack_16c = 0;
        do
        {
          uVar8 = 1;
          iVar4 = kicked(player, card);
          iVar1 = kicked(player, card);
          if (iVar1 != 0)
          {
            uVar8 = uStack_16c & 1;
          }
          iVar1 = can_target(&td_control);
          if ((iVar1 == 0) || (iVar1 = can_target(&td_dont_control), iVar1 == 0))
          {
            uStack_17c = 0;
          }
          else
          {
            uStack_17c = ((unsigned char)uStack_16c >> 3 ^ 1) & 1;
          }
          iVar1 = can_target(&td_player);
          uVar5 = 0;
          if (iVar1 != 0)
          {
            uVar5 = ((unsigned char)uStack_16c >> 2 ^ 1) & 1;
          }
          iVar1 = can_target(&td_creature);
          uVar2 = 0;
          if (iVar1 != 0)
          {
            uVar2 = ((unsigned char)uStack_16c >> 1 ^ 1) & 1;
          }
          iVar4 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\004",
                              "Two +1/+1 counters", uVar2, 1, "Gain life", uVar5, 1, "Fight",
                              uStack_17c, uVar8, "Done", iVar4, 0xfffffffb, 0, 0, 0, 0);
          if (2 < iVar4 - 1U)
          {
            iVar1 = kicked(player, card);
            if (iVar1 != 0)
            {
              if (iVar4 == 0)
              {
                spell_fizzled = 1;
                return 0;
              }
              if (iVar4 < 4)
                goto LAB_023097fe;
            }
            if (iVar4 == 0)
            {
              spell_fizzled = 1;
              return 0;
            }
            break;
          }
          uStack_16c =
              (unsigned int)(unsigned char)((unsigned char)uStack_16c |
                                            (unsigned char)(1 << ((unsigned char)iVar4 & 0x1f)));
          iVar4 = kicked(player, card);
          if (iVar4 == 0)
            break;
        LAB_023097fe:
          uVar8 = num_bits_set((int)(char)(unsigned char)uStack_16c);
        } while (uVar8 < 3);
        if ((uStack_16c & 2) != 0)
        {
          new_pick_target(&td_creature, "Select a creature to gain counters.", 1, 0x81);
        }
        if ((uStack_16c & 4) != 0)
        {
          new_pick_target(&td_player, "TARGET_PLAYER", 2, 1);
        }
        if (((uStack_16c & 8) != 0) &&
            (iVar4 = new_pick_target(&td_control, "TARGET_CREATURE_YOU_CONTROL", 3, 1),
             iVar4 != 0))
        {
          new_pick_target(&td_dont_control, "Select target creature you don\'t control.", 4,
                               0x81);
        }
        instance = get_card_instance(player, card);
        set_inst_info(instance, 0x5564, 0, (short)(char)(unsigned char)uStack_16c);
      }
      goto LAB_023096c9;
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td_creature, 2);
    td_dont_control = td_creature;
    td_control = td_dont_control;
    td_dont_control.allowed_controller = 1 - player;
    td_player = td_control;
    td_control.allowed_controller = player;
    td_player.zone = 0x1000;
    if (event == EVENT_RESOLVE_SPELL)
    {
      instance = get_card_instance(player, card);
      iVar10 = inst_info(instance, 0x5564);
      uVar8 = ((uint32_t)iVar10.id | ((uint32_t)iVar10.data2 << 16)) >> 0x10;
      if (((((uint64_t)iVar10.id | ((uint64_t)iVar10.data2 << 16) | ((uint64_t)iVar10.data << 32)) &
            0x20000) != 0) &&
          (iVar4 = validate_arbitrary_target(&td_creature, instance->targets[1].player,
                                                  instance->targets[1].card),
           iVar4 != 0))
      {
        add_counters_from_source(player, card, instance->targets[1].player,
                                 instance->targets[1].card, COUNTER_P1_P1, 2);
      }
      if (((uVar8 & 4) != 0) &&
          (iVar4 = validate_arbitrary_target(&td_player, instance->targets[2].player,
                                                  instance->targets[2].card),
           iVar4 != 0))
      {
        iVar4 = get_highest_power(instance->targets[2].player);
        gain_life(instance->targets[2].player, iVar4);
      }
      if ((((uVar8 & 8) != 0) &&
           (iVar4 = validate_arbitrary_target(&td_control, instance->targets[3].player,
                                                   instance->targets[3].card),
            iVar4 != 0)) &&
          (iVar4 = validate_arbitrary_target(&td_dont_control, instance->targets[4].player,
                                                  instance->targets[4].card),
           iVar4 != 0))
      {
        fight(instance->targets[3].player, instance->targets[3].card, instance->targets[4].player,
              instance->targets[4].card);
      }
      kill_card(player, card, KILL_DESTROY);
    }
  LAB_023096c9:
    iVar4 = basic_spell(player, card, event);
    return iVar4;
  }
  return 0;
}

#endif
