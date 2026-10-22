#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2372ff0; original cards/jumpstart.c:37. */
int card_archaeomender(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  etb_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
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
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  gain_life_2022(player, 2);
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
    pump_subtype_until_eot_2022(player, card, player, 0x77, 1, 0, 0, 0x10004);
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
    lose_life_2022(player, 1);
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) && (hand_count[1 - player] == 0))
    {
      lose_life_2022(1 - player, 10);
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
  deathtouch_2022(player, card, event);
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

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23730a0; original cards/jumpstart.c:56. */
int card_blessed_sanctuary(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  test_definition_t this_test;

  instance = noncombat_damage_being_prevented(event);
  if ((instance != NULL) && (instance->damage_target_player == player))
  {
    prevent_damage(affected_card_controller, affected_card, instance->info_slot);
  }
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&this_test, 2);
    this_test.type_flag = 3;
    this_test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &this_test, player, card);
  }
  if (event == EVENT_ETB)
  {
    generate_token_by_id_2022(player, card, 0x54f2);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23750b0; original cards/jumpstart.c:740. */
int card_scholar_of_the_lost_trove(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  result = duh_mode(player);
  etb_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    default_test_definition(&test, 0x78);
    result = new_select_target_from_grave(player, card, player, SFG_0, 2, &test, 0);
    if (result != -1)
    {
      play_card_in_grave_for_free_and_exile_it(player, player, result);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int allosaurus_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2372700; original cards/jumpstart.c:3. */
static int allosaurus_legacy(int player, int card, event_t event)
{
  int result;

  attached_set_pt(player, card, event, 5, 5);
  attached_is_additional_subtype(player, card, event, SUBTYPE_DINOSAUR);
  result = eot_trigger(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x2372e70; original cards/jumpstart.c:11. */
int card_allosaurus_shepherd(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  test_definition_t test;

  cannot_be_countered(player, card, event);
  default_test_definition(&test, 0xff);
  test.color = get_sleighted_color_test(player, card, 8);
  type_uncounterable(player, card, event, player, 0, &test);
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }

    if ((event == EVENT_RESOLVE_ACTIVATION) && (0 < active_cards_count[player]))
    {
      iVar2 = 0;
      do
      {
        while ((iVar1 = is_what(player, iVar2, 2),
                iVar1 != 0 && (iVar1 = has_subtype(player, iVar2, SUBTYPE_ELF), iVar1 != 0)))
        {
          iVar1 = iVar2 + 1;
          create_targetted_legacy_effect(player, card, allosaurus_legacy, player, iVar2);
          iVar2 = iVar1;
          if (active_cards_count[player] <= iVar1)
            goto LAB_02372f80;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < active_cards_count[player]);
    }
  }
LAB_02372f80:
  iVar2 = generic_activated_ability(player, card, event, 0, 4, 0, 0, 2, 0, 0, 0, NULL, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x2373860; original cards/jumpstart.c:225. */
int card_inniaz_the_gale_force(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int target_number;
  unsigned int ret_location;
  unsigned int uVar3;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (event == EVENT_DECLARE_ATTACKERS)
  {
    default_test_definition(&test, 2);
    test.keyword = 0x20;
    test.qty = 3;
    store_attackers(player, card, EVENT_DECLARE_ATTACKERS, DAT_RESOLVE_TRIGGER_MANDATORY, player,
                    -1, &test);
    dat_ability(player, card, EVENT_DECLARE_ATTACKERS, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  }
  else
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {

      uVar3 = 0;
      default_target_definition(player, card, &td, 0x47);
      td.illegal_type = 1;
      td.illegal_abilities = 0;
      instance = get_card_instance(player, card);
      do
      {
        td.allow_cancel = 0;
        ret_location = current_turn ^ uVar3;
        td.allowed_controller = ret_location;
        if (ai_is_speculating == 1)
        {
          test.message[0] = '\0';
        }
        else if (player == ret_location)
        {
          scnprintf(test.message, 0x96, "Select a nonland permanent you control");
        }
        else
        {
          scnprintf(test.message, 0x96, "Select a nonland permanent controlled by %s",
                    opponent_name);
        }
        result = can_target(&td);
        if (result != 0)
        {
          new_pick_target(&td, test.message, ret_location, 0x80);
        }
        uVar3 = uVar3 + 1;
      } while (uVar3 != 2);

      uVar3 = 0;
      do
      {
        target_number = 1 - (current_turn ^ uVar3);
        td.allowed_controller = target_number;
        result = validate_target(player, card, &td, target_number);
        if (result != 0)
        {
          give_control(player, card, *(int *)((int)instance + target_number * 8 + 0x74),
                       *(int *)((int)instance + target_number * 8 + 0x78));
        }
        uVar3 = uVar3 + 1;
      } while (uVar3 != 2);
      instance->number_of_targets = '\0';
      return 0;
    }
    if (event == EVENT_CAN_ACTIVATE)
    {

      result = get_cost_mod_for_activated_abilities_2022(player, card, 2, 0, 0, 0, 0, 1);
      result = has_mana_hybrid(player, 1, 5, 2, result);
      if (result == 0)
      {
        return 0;
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x800, 0, 0, 0, 0, 0, 0,
                                         0, NULL, NULL);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {

      result = get_cost_mod_for_activated_abilities_2022(player, card, 2, 0, 0, 0, 0, 1);
      charge_mana_hybrid(player, card, 1, 5, 2, result);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x800, 0, 0, 0, 0, 0, 0, 0,
                                         NULL, NULL);
      return result;
    }
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    new_default_test_definition(&test, 2, "");
    test.keyword = 0x20;
    test.state = 4;
    pump_creatures_until_eot_merge_pt(player, card, current_turn, 1, 1, &test);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2375750; original cards/jumpstart.c:942. */
int card_steel_plume_marshal(int player, int card, event_t event)
{
  test_definition_t test;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  default_test_definition(&test, 2);
  test.keyword = 0x20;
  test.state = 4;
  test.not_me = 1;
  pump_creatures_until_eot_merge_pt(player, card, player, 2, 2, &test);
  return 0;
}

/* ManalinkEh 2022: 0x2375800; original cards/jumpstart.c:963. */
int card_stone_haven_pilgrim(int player, int card, event_t event)
{
  int result;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    result = check_battlefield_for_subtype(player, 0x44, -1);
    if (result != 0)
    {
      pump_ability_until_eot_2022(player, card, player, card, 1, 1, 0, 2);
    }
  }
  else if ((((event - EVENT_POW_BOOST < 2) && (current_turn == player)) &&
            (current_phase < 0x16)) &&
           (result = check_battlefield_for_subtype(player, 0x44, -1), result != 0))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    return (unsigned int)(result == 0);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2375900; original cards/jumpstart.c:986. */
int thriving_land(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  color_test_t cVar3;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    instance = get_card_instance(player, card);
    result = get_deck_color(player, player);
    cVar3 = get_sleighted_color_test(player, card, (unsigned int)instance->card_color);
    result = choose_a_color_from(player, result, (cVar3 ^ ~COLOR_TEST_0) & COLOR_TEST_ANY_COLORED);
    create_card_name_legacy(player, card, result + 0x615a);
    result = color_to_color_test(result);
    instance->info_slot = result;
  }
  result = plain_mana_producer_tapped(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int kels_fight_fixer_effect(int player, int card, event_t event);
static int spiteful_prankster_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2372790; original cards/jumpstart.c:306. */
static int kels_fight_fixer_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  unsigned int uVar3;
  card_instance_t *pcVar4;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x54da))
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
      if ((pcVar4 != NULL) &&
          (result = is_this_dying(affected_card_controller, affected_card), result != 0))
      {

        if ((pcVar1->damage_target_card == affected_card) &&
            (affected_card_controller == pcVar1->damage_target_player))
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
        pcVar1 = get_card_instance(affected_card_controller, affected_card);
        if (pcVar1->kill_code == '\x03')
        {
          default_test_definition(&test, 2);
          count_for_gfp_ability(player, card, 0x77, player, 0, &test);
        }
      }
    }
  }
  result = duh_mode(player);
  uVar3 = resolve_gfp_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if ((uVar3 != 0) && (result = check_status(player, card, 0x10000), result == 0))
  {

    uVar3 = uVar3 & 0xff;
    while (((uVar3 != 0 && (result = has_mana_hybrid(player, 1, 2, 1, 0), result != 0)) &&
            (result = charge_mana_hybrid_while_resolving(player, card, event, 1, COLOR_BLUE,
                                                         COLOR_BLACK, 0),
             result != 0)))
    {
      draw_a_card(player);
      uVar3 = uVar3 - 1;
    }
    kill_card(player, card, KILL_EXILE);
  }
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2373d40; original cards/jumpstart.c:366. */
int card_kels_fight_fixer(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  unsigned int uVar3;
  int iVar4;
  card_instance_t *pcStack_f0;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar1 = 0;
        do
        {
          pcVar2 = in_play(player, iVar1);

          if ((((pcVar2 != NULL) && (iVar4 = is_what(player, iVar1, 0x80), iVar4 != 0)) &&
               (pcVar2 = get_card_instance(player, iVar1),
                pcVar2->info_slot_fn == kels_fight_fixer_effect)) &&
              (((pcVar2->damage_target_player == player && (pcVar2->damage_target_card == card)) ||
                ((pcVar2->targets[0].player == player && (pcVar2->targets[0].card == card))))))
            goto LAB_02373e5c;
          iVar1 = iVar1 + 1;
        } while (iVar1 < active_cards_count[player]);
      }

      iVar1 = create_targetted_legacy_effect(player, card, kels_fight_fixer_effect, player, card);
      pcVar2 = get_card_instance(player, iVar1);
      pcVar2->targets[0].player = player;
      pcVar2->targets[0].card = card;
      pcVar2->number_of_targets = '\x01';
      add_status(player, iVar1, 0x10000);
    }
  LAB_02373e5c:
    pcStack_f0 = get_card_instance(player, card);
    if (pcStack_f0->kill_code == '\0')
      goto LAB_02373e78;
  LAB_02373f38:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_02373d94;
    }
  }
  else
  {
    if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {
      pcVar2 = get_card_instance(affected_card_controller, affected_card);
      if (pcVar2->kill_code == '\x03')
      {
        default_test_definition(&test, 2);
        count_for_gfp_ability(player, card, 0x77, player, 0, &test);
      }
      pcStack_f0 = get_card_instance(player, card);
      if (pcStack_f0->kill_code != '\0')
      {
        return 0;
      }
    LAB_02373e78:

      iVar1 = duh_mode(player);
      uVar3 = resolve_gfp_ability(player, card, event, (iVar1 != 0) + RESOLVE_TRIGGER_OPTIONAL);
      uVar3 = uVar3 & 0xff;
      while (((uVar3 != 0 && (iVar1 = has_mana_hybrid(player, 1, 2, 1, 0), iVar1 != 0)) &&
              (iVar1 = charge_mana_hybrid_while_resolving(player, card, event, 1, COLOR_BLUE,
                                                          COLOR_BLACK, 0),
               iVar1 != 0)))
      {
        draw_a_card(player);
        uVar3 = uVar3 - 1;
      }
    }
    else
    {
      pcStack_f0 = get_card_instance(player, card);
      if (pcStack_f0->kill_code == '\0')
        goto LAB_02373e78;
    }
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_02373f38;
  LAB_02373d94:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02373d9d;
  }
  pump_ability_until_eot_no_repeat_2022(pcStack_f0->parent_controller, pcStack_f0->parent_card,
                                   pcStack_f0->parent_controller, pcStack_f0->parent_card, 0,
                                   0x800);
LAB_02373d9d:
  iVar1 = generic_activated_ability(player, card, event, 0x200, 1, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x2372ae0; original cards/jumpstart.c:807. */
static int spiteful_prankster_legacy(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  unsigned int uVar3;
  int result;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_this_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x54e6))
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
  LAB_02372c28:
    uVar3 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar3 == 0)
      goto LAB_02372c49;
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
      goto LAB_02372c28;

    pcVar1 = get_card_instance(player, card);
    pcVar2 = in_play(affected_card_controller, affected_card);

    if ((pcVar2 != NULL) && (pcVar2 = get_card_instance(affected_card_controller, affected_card),
                             pcVar2->kill_code != '\0'))
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
        *(uint8_t *)((int)&pcVar1->targets[0xb].player + 2) = 1;
        return 0;
      }
      default_test_definition(&recovered_stack_this_test.test, 2);
      count_for_gfp_ability(player, card, 0x77, 2, 0, &recovered_stack_this_test.test);
    }
    uVar3 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar3 == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {

    default_target_definition(player, card, &recovered_stack_this_test.target, 0x102);
    recovered_stack_this_test.test.message[0xc] = '\0';
    recovered_stack_this_test.test.message[0xd] = '\x12';
    recovered_stack_this_test.test.message[0xe] = '\0';
    recovered_stack_this_test.test.message[0xf] = '\0';
    recovered_stack_this_test.test.message[0x44] = '\0';
    recovered_stack_this_test.test.message[0x45] = '\0';
    recovered_stack_this_test.test.message[0x46] = '\0';
    recovered_stack_this_test.test.message[0x47] = '\0';
    pcVar1 = get_card_instance(player, card);
    for (uVar3 = uVar3 & 0xff; uVar3 != 0; uVar3 = uVar3 - 1)
    {
      result = can_target(&recovered_stack_this_test.target);
      if ((result != 0) &&
          (result = new_pick_target(&recovered_stack_this_test.target, "TARGET_ANY", 1, 0),
           result != 0))
      {
        damage_creature(pcVar1->targets[1].player, pcVar1->targets[1].card, 1, player, card);
        pcVar1->number_of_targets = '\0';
      }
    }
    kill_card(player, card, KILL_EXILE);
  }
LAB_02372c49:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23754b0; original cards/jumpstart.c:874. */
int card_spiteful_prankster(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int iVar3;
  int iVar4;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_this_test;

  if (event == EVENT_ABILITIES)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (current_turn == player))
    {
      event_result = event_result | 0x100;
    }
  }
  else if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar3 = 0;
        do
        {
          instance = in_play(player, iVar3);

          if ((((instance != NULL) && (iVar4 = is_what(player, iVar3, 0x80), iVar4 != 0)) &&
               (instance = get_card_instance(player, iVar3),
                instance->info_slot_fn == spiteful_prankster_legacy)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_023754e1;
          iVar3 = iVar3 + 1;
        } while (iVar3 < active_cards_count[player]);
      }

      iVar3 = create_targetted_legacy_effect(player, card, spiteful_prankster_legacy, player, card);
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
    recovered_stack_this_test.test.not_me = 1;
    count_for_gfp_ability(player, card, 0x77, 2, 0, &recovered_stack_this_test.test);
  }
LAB_023754e1:
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar2 != 0))
  {

    default_target_definition(player, card, &recovered_stack_this_test.target, 0x102);
    recovered_stack_this_test.test.message[0xc] = '\0';
    recovered_stack_this_test.test.message[0xd] = '\x12';
    recovered_stack_this_test.test.message[0xe] = '\0';
    recovered_stack_this_test.test.message[0xf] = '\0';
    recovered_stack_this_test.test.message[0x44] = '\0';
    recovered_stack_this_test.test.message[0x45] = '\0';
    recovered_stack_this_test.test.message[0x46] = '\0';
    recovered_stack_this_test.test.message[0x47] = '\0';
    instance = get_card_instance(player, card);
    for (uVar2 = uVar2 & 0xff; uVar2 != 0; uVar2 = uVar2 - 1)
    {
      iVar3 = can_target(&recovered_stack_this_test.target);
      if ((iVar3 != 0) &&
          (iVar3 = pick_target(&recovered_stack_this_test.target, "TARGET_ANY"), iVar3 != 0))
      {
        damage_target0(player, card, 1);
        instance->number_of_targets = '\0';
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23734d0; original cards/jumpstart.c:168. */
int card_emiel_the_blessed(int player, int card, event_t event)
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

  check_legend_rule(player, card, event);
  if ((((trigger_condition == 0xdb) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    default_test_definition(&recovered_stack_td.test, 2);
    result = duh_mode(player);
    etb_ability_global(player, event, (result != 0) + 1, TYPE_NONE, &recovered_stack_td.test,
                       player, card);
  }
  if (event == EVENT_ETB)
  {
    result = has_mana_hybrid(player, 1, 3, 5, 0);
    if (result != 0)
    {
      result = charge_mana_hybrid_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, 1,
                                                  COLOR_GREEN, COLOR_WHITE, 0);
      if (result != 0)
      {
        result = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_UNICORN);
        add_counters_from_source(player, card, trigger_cause_controller, trigger_cause,
                                 COUNTER_P1_P1, (result != 0) + 1);
      }
    }
  }
  else if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
           ((event == EVENT_RESOLVE_ACTIVATION || (event == EVENT_CLEANUP))))
  {
    default_target_definition(player, card, &recovered_stack_td.target, 2);
    recovered_stack_td.target.allowed_controller = player;
    recovered_stack_td.target.preferred_controller = player;
    recovered_stack_td.target.special = 0x4000;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      instance = get_card_instance(player, card);
      result = valid_target(&recovered_stack_td.target);
      if (result != 0)
      {
        blink_effect(instance->targets[0].player, instance->targets[0].card, NULL);
      }
    }
    result = generic_activated_ability(player, card, event, 4, 3, 0, 0, 0, 0, 0, 0,
                                       &recovered_stack_td.target, "ASHNODS_BATTLEGEAR");
    return result;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2375c30; original cards/jumpstart.c:1082. */
int card_trusty_retriever(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  test_definition_t this_test;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    result = count_graveyard_by_type(player, 0x44);
    uVar2 = 0;
    if (result != 0)
    {
      result = graveyard_has_shroud(player);
      uVar2 = (unsigned int)(result == 0);
    }
    result =
        dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\003", "\001\016",
                    "Add a +1/+1 counter", 1, 5, "Retrieve a card", uVar2, 5, 0, 0, 0, 0);
    if (result == 1)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    }
    else if (result == 2)
    {
      new_default_test_definition(&this_test, 0x44, "Select target artifact or enchantment card.");
      result = new_special_count_grave(player, &this_test);
      if (0 < result)
      {
        result = graveyard_has_shroud(player);
        if (result == 0)
        {
          new_global_tutor(player, player, 2, 1, 1, 1, &this_test);
        }
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2374400; original cards/jumpstart.c:504. */
int card_muxus_goblin_grandee(int player, int card, event_t event)
{
  int *deck;
  int iVar1;
  int iVar2;
  int iVar3;
  int iStack_f0;
  test_definition_t test;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    deck = deck_ptr[player];

    iStack_f0 = count_deck(player);
    if (0 < iStack_f0)
    {
      if (6 < iStack_f0)
      {
        iStack_f0 = 6;
      }

      show_deck(0, deck, iStack_f0, "Here\'s the first 6 card of your deck", 0, ENGINE_DONE_LABEL);
      iVar1 = iStack_f0 + -1;
      do
      {

        iVar3 = deck[iVar1];
        iVar2 = is_what(-1, iVar3, 2);
        if (iVar2 != 0)
        {
          iVar2 =
              has_subtype_by_id_2022(player, (unsigned int)cards_data[iVar3].id, SUBTYPE_GOBLIN);
          if (iVar2 != 0)
          {
            iVar2 = get_cmc_by_internal_id(iVar3);
            if (iVar2 < 6)
            {

              iVar3 = add_card_to_hand(player, iVar3);
              put_into_play(player, iVar3);
              remove_card_from_deck(player, iVar1);
              iStack_f0 = iStack_f0 + -1;
            }
          }
        }
        iVar1 = iVar1 + -1;
      } while (iVar1 != -1);
      put_top_x_on_bottom_in_random_order(player, iStack_f0);
    }
    dat_ability(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    return 0;
  }
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }

  default_test_definition(&test, 0xff);
  test.subtype = 0x4b;
  test.not_me = 1;
  iVar1 = check_battlefield_for_special_card(player, card, player, 4, &test);
  pump_until_eot_2022(player, card, player, card, iVar1, iVar1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23732b0; original cards/jumpstart.c:116. */
int card_bruvac_the_grandiloquent(int player, int card, event_t event)
{
  enum
  {
    XTRIGGER_REPLACE_MILL_CARDS = 16778496
  };

  xtrigger_t xVar1;
  int result;

  check_legend_rule(player, card, event);
  enable_xtrigger_flags = enable_xtrigger_flags | ENABLE_XTRIGGER_REPLACE_MILL;
  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if ((((xVar1 == XTRIGGER_REPLACE_MILL_CARDS) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (((reason_for_trigger_controller == player && (1 - player == trigger_cause_controller)) &&
        (0 < replace_milled))))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      if (event == EVENT_TRIGGER)
      {
        event_result = event_result | 2;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {
        replace_milled = replace_milled << 1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23740f0; original cards/jumpstart.c:433. */
int card_lightning_phoenix(int player, int card, event_t event)
{
  enum
  {
    EVENT_GRAVEYARD_ABILITY_EOT = 1280
  };

  int result;

  haste(player, card, event);
  cannot_block(player, card, event);

  if ((((event == EVENT_GRAVEYARD_ABILITY_EOT) &&
        (result = get_trap_condition(1 - player, 0), 2 < result)) &&
       (result = has_mana(player, 4, 1), result != 0)) &&
      (((result = do_dialog(player, player, card, -1, -1,
                            " Return Lightning Phoenix\n Do not return Lightning Phoenix", 0),
         result == 0 && (charge_mana(player, COLOR_RED, 1), spell_fizzled != 1)) &&
        (result = grafdiggers_cages(player, TYPE_CREATURE, TUTOR_FROM_GRAVE), result == 0))))
  {
    remove_state(player, card, 0x20);
    put_into_play(player, card);
    return -1;
  }
  return 0;
}
