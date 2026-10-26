#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2394930; original cards/m20.c:3. */
int card_aerial_assault(int player, int card, event_t event)
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 2);
        td.required_state = 1;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            instance = get_card_instance(player, card);
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            new_default_test_definition(&test, 2, "");
            test.keyword = 0x20;
            result = check_battlefield_for_special_card(player, card, player, 4, &test);
            gain_life(player, result);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02394995;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.required_state = 1;
LAB_02394995:
  result = generic_spell(player, card, event, 1, &td, "ROYAL_ASSASSIN", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2395180; original cards/m20.c:348. */
int card_cavalier_of_dawn(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  token_generation_t token;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  vigilance(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &recovered_stack_td.target, 0x47);
    recovered_stack_td.target.illegal_type = 1;
    recovered_stack_td.target.allow_cancel = 1;
    result = can_target(&recovered_stack_td.target);
    if (result != 0)
    {
      result = new_pick_target(&recovered_stack_td.target, "TARGET_NONLAND_PERMANENT", 0, 0);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        default_token_definition(player, card, 0x4940, &token);
        token.t_player = instance->targets[0].player;
        generate_token(&token);
      }
    }
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    result = count_graveyard_by_type(player, 0x44);
    if (result != 0)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        new_default_test_definition(&recovered_stack_td.test, 0x44,
                                    "Select an Artifact or Enchantment card.");
        new_global_tutor(player, player, 2, 1, 0, 1, &recovered_stack_td.test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2395360; original cards/m20.c:385. */
int card_cavalier_of_flame(int player, int card, event_t event)
{
  int result;
  int card_00;
  test_definition_t test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 0xff, "Select a card to discard.");
    result = 0;
    if (hand_count[player] != 0)
    {
      result = 0;
      while (card_00 = new_select_a_card(player, player, 4, 0, 3, -1, &test), -1 < card_00)
      {
        result = result + 1;
        discard_card(player, card_00);
        if (((result == 3) && ((ai_is_speculating == 1 ||
                                ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))))) ||
            (hand_count[player] == 0))
          break;
      }
    }
    draw_cards(player, result);
    result = this_dies_trigger(player, card, EVENT_ETB, RESOLVE_TRIGGER_MANDATORY);
    if (result == 0)
    {
      return 0;
    }
  LAB_02395533:
    default_test_definition(&test, 0x100);
    result = count_graveyard_by_type(player, 1);
    new_damage_all(player, card, 1 - player, result, 0x22, &test);
  }
  else
  {
    result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (result != 0)
      goto LAB_02395533;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_023953e6:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_023953f0;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_023953e6;
  }
  pump_creatures_until_eot(player, card, player, 0, 1, 0, 0, 4, NULL);
LAB_023953f0:
  result = generic_activated_ability(player, card, event, 0, 1, 0, 0, 0, 1, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2395750; original cards/m20.c:468. */
int card_cavalier_of_night(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  card_instance_t *instance;
  target_definition_t td2;
  test_definition_t test;
  lifelink(player, card, event);
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 2, "Select another creature to sacrifice.");
    test.not_me = 1;
    default_target_definition(player, card, &td2, 2);
    td2.allowed_controller = 1 - player;
    uVar2 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_JUST_MARK | SAC_NO_CANCEL,
                          &test);
    if (uVar2 != 0)
    {
      result = can_target(&td2);
      if (result != 0)
      {
        result = new_pick_target(&td2, "TARGET_CREATURE_OPPONENT_CONTROLS", 0, 0);
        if (result != 0)
        {
          instance = get_card_instance(player, card);
          result = instance->targets[0].card;
          instance = get_card_instance(player, card);
          kill_card(instance->targets[0].player, result, KILL_DESTROY);
        }
      }
      kill_card((int)uVar2 >> 0x10 & 0xff, uVar2 >> 0x18, KILL_SACRIFICE);
    }
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    new_default_test_definition(&test, 2, "Select a creature card with CMC 3 or less.");
    test.cmc = 4;
    test.cmc_flag = 3;
    result = new_special_count_grave(player, &test);
    if (0 < result)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        new_global_tutor(player, player, 2, 4, 1, 1, &test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23959a0; original cards/m20.c:514. */
int card_cavalier_of_thorns(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    reveal_top_cards_of_library_and_choose_type(player, card, player, 5, 1, TUTOR_PLAY_TAPPED, 1,
                                                TUTOR_GRAVE, 1, TYPE_LAND);
  }
  result = duh_mode(player);
  result = this_dies_trigger(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (result != 0)
  {
    result = exile_from_owners_graveyard(player, card);
    if (result != 0)
    {
      default_test_definition(&test, 0xff);
      new_global_tutor(player, player, 2, 2, 0, 1, &test);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2396950; original cards/m20.c:877. */
int card_colossus_hammer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = is_equipping(player, card);
  if ((((event == EVENT_ABILITIES) && (instance != NULL)) &&
       (instance->damage_target_card == affected_card)) &&
      (instance->damage_target_player == affected_card_controller))
  {
    event_result = event_result & 0xffffffdf;
  }
  result = vanilla_equipment(player, card, event, 8, 10, 10, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x23969f0; original cards/m20.c:892. */
int card_corpse_knight(int player, int card, event_t event)
{
  test_definition_t test;
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 2);
    test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event != EVENT_ETB)
  {
    return 0;
  }
  lose_life(1 - player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x2396ab0; original cards/m20.c:990. */
int card_drawn_from_dreams(int player, int card, event_t event)
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
      new_default_test_definition(&test, 0xff, "Select a card.");
      test.qty = 2;
      reveal_top_cards_of_library_and_choose(player, card, player, 7, 1, TUTOR_HAND, 0,
                                             TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &test);
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

/* ManalinkEh 2022: 0x2397090; original cards/m20.c:1240. */
int card_golos_tireless_pilgrim(int player, int card, event_t event)
{
  unsigned short uVar1;
  int result;
  test_definition_t test;
  check_legend_rule(player, card, event);
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 1, "Choose a land card.");
    new_global_tutor(player, player, 1, 5, 0, 1, &test);
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_02397161:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02397166;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_02397161;
  }
  result = 3;
  do
  {
    if (*deck_ptr[player] != -1)
    {
      uVar1 = cards_data[*deck_ptr[player]].id;
      rfg_top_card_of_deck(player);
      create_may_play_card_from_exile_effect(player, card, player, (unsigned int)uVar1,
                                             MPCFE_UNTIL_EOT | MPCFE_FOR_FREE);
    }
    result = result + -1;
  } while (result != 0);
LAB_02397166:
  result = generic_activated_ability(player, card, event, 0, 2, 1, 1, 1, 1, 1, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2397240; original cards/m20.c:1274. */
int card_gorging_vulture(int player, int card, event_t event)
{
  int source_csvid;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  source_csvid = get_id(player, card);
  special_mill(player, card, source_csvid, player, 4);
  return 0;
}

/* ManalinkEh 2022: 0x23972c0; original cards/m20.c:1359. */
int card_infuriate(int player, int card, event_t event)
{
  int result;
  result = vanilla_instant_pump(player, card, event, 2, player, 3, 2, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2397a20; original cards/m20.c:1534. */
int card_leafkin_druid(int player, int card, event_t event)
{
  int result;
  result = count_permanents_by_type(player, TYPE_CREATURE);
  result = mana_producing_creature(player, card, event, 0x14, COLOR_GREEN, (3 < result) + 1);
  return result;
}

/* ManalinkEh 2022: 0x2397a80; original cards/m20.c:1544. */
int card_legions_end(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
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
        td.allowed_controller = 1 - player;
        td.power_requirement = 0x2002;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          iVar2 = validate_target(player, card, &td, 0);
          if (iVar2 != 0)
          {
            iVar2 = get_id(instance->targets[0].player, instance->targets[0].card);
            echoing_effect(player, card, instance->targets[0].player, instance->targets[0].card,
                           &td, 4);
            new_default_test_definition(&this_test, 0, "");
            this_test.no_shuffle = 1;
            this_test.id = iVar2;
            if (ai_is_speculating != 1)
            {
              scnprintf(this_test.message, 100, "Hand: Select a card named %s to exile.",
                        cards_ptr[iVar2]->full_name);
            }
            do
            {
              iVar3 = instance->targets[0].player;
              if ((hand_count[iVar3] < 1) ||
                  (iVar3 = new_global_tutor(player, iVar3, 4, 6, 1, 5, &this_test), iVar3 < 0))
                break;
              iVar3 = is_id_in_hand(instance->targets[0].player, iVar2);
            } while (iVar3 != 0);
            if (ai_is_speculating != 1)
            {
              scnprintf(this_test.message, 100, "Graveyard: Select a card named %s to exile.",
                        cards_ptr[iVar2]->full_name);
            }
            do
            {
              iVar2 = new_special_count_grave(instance->targets[0].player, &this_test);
              if (iVar2 < 1)
                break;
              iVar2 = new_global_tutor(player, instance->targets[0].player, 2, 6, 1, 5, &this_test);
            } while (-1 < iVar2);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02397b09;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = 1 - player;
  td.power_requirement = 0x2002;
  get_card_instance(player, card);
LAB_02397b09:
  iVar2 = generic_spell(player, card, event, 0x81, &td,
                        "Select target creature an opponent controls with CMC 2 or less", 1, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x23942a0; original cards/m20.c:1844. */
static int fx_yanling_debuff(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (event == EVENT_POWER)
  {
    instance = get_card_instance(player, card);
    if ((instance->damage_target_card == affected_card) &&
        (instance->damage_target_player == affected_card_controller))
    {
      event_result = event_result + -2;
    }
  }
  else if (event == EVENT_ABILITIES)
  {
    instance = get_card_instance(player, card);
    if ((instance->damage_target_card == affected_card) &&
        (instance->damage_target_player == affected_card_controller))
    {
      event_result = event_result & 0xffffffdf;
    }
  }
  else if ((event == EVENT_BEGIN_TURN) && (current_turn == player))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23986c0; original cards/m20.c:1943. */
int card_mu_yanling_sky_dancer_emblem(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 1);
    instance = get_card_instance(player, card);
    td.illegal_state = 1;
    td.required_subtype = instance->targets[1].player;
    td.allowed_controller = player;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 1);
    instance = get_card_instance(player, card);
    td.allowed_controller = player;
    td.illegal_state = 1;
    td.required_subtype = instance->targets[1].player;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      draw_a_card(player);
    }
  }
  result = generic_activated_ability_with_tapsubtype_as_cost(player, card, event, GAA_NONE, 0, 0, 0,
                                                             0, 0, 0, 0, NULL, NULL, &td, 1);
  return result;
}

/* ManalinkEh 2022: 0x2399100; original cards/m20.c:2156. */
int card_pyroclastic_elemental(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      damage_target0(player, card, 1);
    }
  }
  result =
      generic_activated_ability(player, card, event, 4, 1, 0, 0, 0, 2, 0, 0, &td, "TARGET_PLAYER");
  return result;
}

/* ManalinkEh 2022: 0x23993f0; original cards/m20.c:2261. */
int card_rotting_regisaur(int player, int card, event_t event)
{
  upkeep_trigger_ability(player, card, event, player);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    return 0;
  }
  discard(player, 0, player);
  return 0;
}

/* ManalinkEh 2022: 0x2394640; original cards/m20.c:2606. */
static char *is_legendary_by_iid(int who_chooses, int player, int card)
{
  int result;
  char *pcVar2;
  result = is_legendary(player, card);
  pcVar2 = NULL;
  if (result == 0)
  {
    pcVar2 = "must be a Legendary spell";
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x239a2b0; original cards/m20.c:2734. */
int card_tomebound_lich(int player, int card, event_t event)
{
  int result;
  deathtouch(player, card, event);
  lifelink(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if ((event != EVENT_ETB) &&
      (result = damage_dealt_by_me(player, card, event,
                                   DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE),
       result == 0))
  {
    return 0;
  }
  draw_a_card(player);
  discard(player, 0, player);
  return 0;
}

/* ManalinkEh 2022: 0x239a4f0; original cards/m20.c:2861. */
int card_vilis_broker_of_blood(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if (event == EVENT_ACTIVATE)
  {
    get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      pump_until_eot_merge_previous(player, instance->parent_card, instance->targets[0].player,
                                    instance->targets[0].card, -1, -1);
    }
  }
  result = generic_activated_ability(player, card, event, 4, 0, 1, 0, 0, 0, 0, 2, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2396bc0; original cards/m20.c:1027. */
int card_elvish_reclaimer(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (event - EVENT_POWER < 2)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (result = count_graveyard_by_type(player, 1), 2 < result))
    {
      event_result = event_result + 2;
    }
  }
  else if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
           ((event == EVENT_RESOLVE_ACTIVATION || (event == EVENT_CLEANUP))))
  {
    new_default_test_definition(&test, 1, "Select a land card");
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      new_global_tutor(player, player, 1, 5, 0, 5, &test);
    }
    result = generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_UNTAPPED, 2,
                                                              0, 0, 0, 0, 0, 0, NULL, NULL,
                                                              TYPE_LAND, NULL, SARV_0);
    return result;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2396f70; original cards/m20.c:1158. */
int card_fry(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  const char *prompt;
  int result;
  target_definition_t td;

  cannot_be_countered(player, card, event);
  default_target_definition(player, card, &td, 0x102);
  uVar1 = get_sleighted_color_test(player, card, 0x20);
  uVar2 = get_sleighted_color_test(player, card, 4);
  td.required_color = uVar2 | uVar1;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      damage_target0(player, card, 5);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  prompt = get_sleighted_color_text2(
      player, card, "Select target creature or planeswalker that\'s %s or %s.", 5, 2);
  result = generic_spell(player, card, event, 0x81, &td, prompt, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2398dd0; original cards/m20.c:2045. */
int card_noxious_grasp(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  const char *prompt;
  int result;
  card_instance_t *instance;
  target_definition_t td;

  default_target_definition(player, card, &td, 0x102);
  uVar1 = get_sleighted_color_test(player, card, 8);
  uVar2 = get_sleighted_color_test(player, card, 0x20);
  td.required_color = uVar2 | uVar1;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      instance = get_card_instance(player, card);
      result = instance->targets[0].card;
      instance = get_card_instance(player, card);
      kill_card(instance->targets[0].player, result, KILL_DESTROY);
      gain_life(player, 1);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  prompt =
      get_sleighted_color_text2(player, card, "Select target creature that\'s %s or %s .", 3, 5);
  result = generic_spell(player, card, event, 0x81, &td, prompt, 1, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2394af0; original cards/m20.c:42. */
int card_agent_of_treachery(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  target_definition_t td;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x47);
    td.preferred_controller = 1 - player;
    td.allow_cancel = 0;
    iVar1 = can_target(&td);
    if ((iVar1 != 0) && (iVar1 = new_pick_target(&td, "TARGET_PERMANENT", 0, 0), iVar1 != 0))
    {
      instance = get_card_instance(player, card);
      iVar1 = instance->targets[0].card;
      instance = get_card_instance(player, card);
      gain_control_permanently(player, instance->targets[0].player, iVar1);
    }
  }
  iVar1 = eot_trigger(player, card, event);

  if (((iVar1 != 0) && (current_turn == player)) &&
      (iVar1 = active_cards_count[player] + -1, -1 < iVar1))
  {
    iVar4 = 0;
    do
    {
      instance = in_play(player, iVar1);
      if ((instance != NULL) && (iVar3 = is_what(player, iVar1, 0x47), iVar3 != 0))
      {
        iVar3 = is_stolen(player, iVar1);
        iVar4 = (iVar4 + 1) - (unsigned int)(iVar3 == 0);
      }
      iVar1 = iVar1 + -1;
      if (iVar1 < 0)
      {
        if (iVar4 < 3)
        {
          return 0;
        }
        break;
      }
    } while (iVar4 < 3);
    draw_cards(player, 3);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2394f20; original cards/m20.c:247. */
int card_blood_for_bones(int player, int card, event_t event)
{
  int iVar1;
  int iid;
  int iVar2;
  unsigned int uVar3;
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

      new_default_test_definition(&test, 2, "Select a creature to return to the battlefield.");
      iVar1 = new_select_a_card(player, player, 2, 0, 2, -1, &test);
      if (iVar1 == -1)
      {
        new_default_test_definition(&test, 2, "Select a creature to return to your hand.");
        test.id = 0x388;
        test.id_flag = 1;
        iVar2 = new_select_a_card(player, player, 2, 0, 1, -1, &test);
        uVar3 = 0xffffffff;
      }
      else
      {
        iid = turn_card_in_grave_face_down(player, iVar1);
        new_default_test_definition(&test, 2, "Select a creature to return to your hand.");
        test.id = 0x388;
        test.id_flag = 1;
        iVar2 = new_select_a_card(player, player, 2, 0, 1, -1, &test);
        if (iid != -1)
        {
          turn_card_in_grave_face_up(player, iVar1, iid);
        }
        uVar3 = reanimate_permanent(player, card, player, iVar1, REANIMATE_DEFAULT);
      }
      if (iVar2 != -1)
      {
        from_grave_to_hand(
            player, iVar2 - ((uVar3 ^ 0xffffffff) >> 0x1f & (unsigned int)(iVar1 < iVar2)), 1);
      }
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  iVar1 = generic_spell(player, card, event, 0x400, NULL, NULL, 0, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x2399450; original cards/m20.c:2320. */
int card_scorch_spitter(int player, int card, event_t event)
{
  int tgt_card;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    tgt_card = planeswalker_its_attacking(player, card);
    if (tgt_card != -2)
    {
      damage_creature(1 - current_turn, tgt_card, 1, player, card);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x239a0e0; original cards/m20.c:2694. */
int card_thunderkin_awakener(int player, int card, event_t event)
{
  xtrigger_t xVar1;
  int result;
  test_definition_t this_test;

  store_attackers(player, card, event, 0, player, card, NULL);
  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if ((((xVar1 == XTRIGGER_ATTACKING) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    result = graveyard_has_shroud(player);
    if (result != 0)
    {
      return 0;
    }
    default_test_definition(&this_test, 2);
    this_test.toughness = get_toughness(player, card);
    this_test.toughness_flag = 3;
    result = new_special_count_grave(player, &this_test);
    if (result == 0)
    {
      return 0;
    }
    result = duh_mode(player);
    dat_ability(player, card, event, (result != 0) + DAT_RESOLVE_TRIGGER_OPTIONAL, 0);
  }
  if (event == EVENT_DAT_ABILITY)
  {

    result = get_toughness(player, card);
    default_test_definition(&this_test, 2);
    scnprintf(this_test.message, 100,
              "Select an Elemental creature card with toughness %d or less.", result + -1);
    this_test.toughness_flag = 3;
    this_test.toughness = result;
    result = new_special_count_grave(player, &this_test);
    if (result != 0)
    {
      result = new_global_tutor(player, player, 2, 8, 1, 2, &this_test);
      create_targetted_legacy_effect(player, card, haste_and_sacrifice_eot, player, result);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2394670; original cards/m20.c:2787. */
int vos_legacy(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  char cVar3;

  if (event == EVENT_CAST_SPELL)
  {
    if (card != affected_card)
    {
      if (player == affected_card_controller)
      {
        state_untargettable(player, affected_card, 1);
      }
    LAB_0239476a:
      if (trigger_condition != 0xdb)
      {
        return 0;
      }
      if (card != affected_card)
      {
        return 0;
      }
      goto LAB_0239473e;
    }
    if (player != affected_card_controller)
      goto LAB_0239476a;
    if (trigger_condition != 0xdb)
    {
      return 0;
    }
  }
  else
  {
    if ((trigger_condition != 0xdb) || (card != affected_card))
      goto LAB_02394697;
  LAB_0239473e:
    if (player != affected_card_controller)
      goto LAB_02394697;
  }
  if ((reason_for_trigger_controller == affected_card_controller) &&
      (result = check_state(trigger_cause_controller, trigger_cause, 0x100000), result != 0))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result = event_result | 2;
      return 0;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      state_untargettable(trigger_cause_controller, trigger_cause, 0);
      return 0;
    }
  }
LAB_02394697:
  if (event == EVENT_STATIC_EFFECTS)
  {
    *(unsigned int *)(&player_bits + player * 4) =
        *(unsigned int *)(&player_bits + player * 4) | 0x4000000;
  }
  else if (event == EVENT_PROT_MISC)
  {
    cVar3 = (char)((unsigned int)event_result >> 0x18);
    if ((cVar3 == '\x06') ||
        ((((cVar3 == '\x05' && (affected_card_controller == player)) && (affected_card == -1)) &&
          ((1 - player == attacking_card_controller &&
            (uVar1 = get_color(1 - player, attacking_card), (uVar1 & 6) != 0))))))
    {
      event_result = event_result | 1;
      return 0;
    }
  }
  else if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int moldervine_reclamation_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2396d30; original cards/m20.c:1095. */
int card_field_of_the_dead(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int *piVar5;
  unsigned char bVar6;
  int iStack_108;
  int array[7];
  test_definition_t test;

  bVar6 = 0;
  if (((((trigger_condition == 0xdb) && (card == affected_card)) &&
        (player == affected_card_controller)) &&
       ((trigger_cause_controller == player && (reason_for_trigger_controller == player)))) &&
      (iVar1 = is_humiliated(player, card, AL_TRIG), iVar1 == 0))
  {

    iVar1 = is_what(trigger_cause_controller, trigger_cause, 1);
    if ((iVar1 != 0) && (iVar1 = count_permanents_by_type(player, TYPE_LAND), 6 < iVar1))
    {

      default_test_definition(&test, 1);
      piVar5 = array + 1;
      for (iVar1 = 0x18; iVar1 != 0; iVar1 = iVar1 + -1)
      {
        *(uint8_t *)piVar5 = 0xff;
        piVar5 = (int *)((int)piVar5 + (unsigned int)bVar6 * -2 + 1);
      }
      array[0] = get_card_name(trigger_cause_controller, trigger_cause);
      if (0 < active_cards_count[player])
      {
        iVar1 = 0;
        do
        {
          instance = in_play(player, iVar1);
          if ((instance != NULL) && (iVar3 = is_what(player, iVar1, 1), iVar3 != 0))
          {
            iVar3 = 0;
            do
            {
              iVar4 = get_card_name(player, iVar1);
              if (iVar4 == array[iVar3])
                break;
              if (array[iVar3] == -1)
              {
                iVar4 = get_card_name(player, iVar1);
                array[iVar3] = iVar4;
                if (iVar3 == 6)
                {
                  default_test_definition(&test, 1);
                  iStack_108 = 2;
                  goto LAB_02396df3;
                }
                break;
              }
              iVar3 = iVar3 + 1;
            } while (iVar3 != 7);
          }
          iVar1 = iVar1 + 1;
        } while (iVar1 < active_cards_count[player]);
      }
    }
    default_test_definition(&test, 1);
    iStack_108 = 0;
  LAB_02396df3:
    etb_ability_global(player, event, iStack_108, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    generate_token_by_id(player, card, 0x49ac);
  }
  iVar1 = plain_mana_producer_tapped(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x2394390; original cards/m20.c:1737. */
static int moldervine_reclamation_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  card_instance_t *pcVar3;
  int result;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x4f96))
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
  LAB_023943c4:
    uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto joined_r0x023943ed;
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
      goto LAB_023943c4;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);
    if ((pcVar3 != NULL) &&
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
      }
      count_for_gfp_ability_and_store_values(player, card, 0x77, player, 2, NULL, 0, 0);
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

    for (uVar2 = uVar2 & 0xff; uVar2 != 0; uVar2 = uVar2 - 1)
    {
      gain_life(player, 1);
    }
    draw_cards(player, 0);
    kill_card(player, card, KILL_EXILE);
  }
joined_r0x023943ed:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23980d0; original cards/m20.c:1788. */
int card_moldervine_reclamation(int player, int card, event_t event)
{
  unsigned int uVar1;
  int iVar2;
  int iVar3;
  card_instance_t *instance;

  if (((event == EVENT_STATIC_EFFECTS) && (card == affected_card)) &&
      (player == affected_card_controller))
  {

    if (0 < active_cards_count[player])
    {
      iVar2 = 0;
      do
      {
        instance = in_play(player, iVar2);

        if ((((instance != NULL) && (iVar3 = is_what(player, iVar2, 0x80), iVar3 != 0)) &&
             (instance = get_card_instance(player, iVar2),
              instance->info_slot_fn == moldervine_reclamation_effect)) &&
            (((instance->damage_target_player == player &&
               (instance->damage_target_card == card)) ||
              ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
          goto LAB_023980ea;
        iVar2 = iVar2 + 1;
      } while (iVar2 < active_cards_count[player]);
    }

    iVar2 =
        create_targetted_legacy_effect(player, card, moldervine_reclamation_effect, player, card);
    instance = get_card_instance(player, iVar2);
    instance->targets[0].player = player;
    instance->targets[0].card = card;
    instance->number_of_targets = '\x01';
    add_status(player, iVar2, 0x10000);
  }
LAB_023980ea:
  count_for_gfp_ability_and_store_values(player, card, event, player, 2, NULL, 0, 0);
  uVar1 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if ((uVar1 != 0) && (instance = get_card_instance(player, card), instance->kill_code == '\0'))
  {

    for (uVar1 = uVar1 & 0xff; uVar1 != 0; uVar1 = uVar1 - 1)
    {
      gain_life(player, 1);
    }
    draw_cards(player, 0);
  }
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x23994e0; original cards/m20.c:2362. */
int card_shifting_ceratops(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  card_instance_t *instance;
  int32_t player_00;
  uint32_t uVar4;
  int32_t card_00;
  uint32_t uVar5;
  unsigned int uVar6;
  int iVar7;
  int iVar8;
  uint32_t uStack_1c;

  cannot_be_countered(player, card, event);
  if (event == EVENT_CLEANUP)
  {

    instance = get_card_instance(player, card);
    instance->targets[1].card = 0;
    instance->targets[1].player = 0;
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar7 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 1, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        return iVar7;
      }
      if (event == EVENT_ACTIVATE)
      {

        iVar7 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 1, 0, 0, 0,
                                          NULL, NULL);
        uVar1 = get_abilities(player, card, EVENT_ABILITIES, -1);
        uVar6 = instance->targets[1].player;
        uVar1 = uVar6 & (int)(uVar6 ^ 0xffffffff) >> 0x1f | uVar1;
        uVar2 = get_special_abilities(player, card);
        uVar6 = instance->targets[1].card;
        if (((((uVar1 & 0x400) == 0) && (current_turn != player)) && (current_phase == 0x16)) &&
            (iVar8 = blocking(player, card, EVENT_ACTIVATE), iVar8 == 0))
        {
          uStack_1c = 10;
        }
        else
        {
          uStack_1c = 1;
        }
        if ((((uVar1 & 0x80) == 0) && (current_phase == 0x18)) &&
            (iVar8 = is_unblocked(player, card), iVar8 == 0))
        {
          uVar5 = 10;
        }
        else
        {
          uVar5 = 1;
        }
        if (((((uVar2 | uVar6 & (int)(uVar6 ^ 0xffffffff) >> 0x1f) & 4) == 0) &&
             (current_phase < 0x15)) &&
            ((current_turn == player && (iVar8 = is_sick(player, card), iVar8 != 0))))
        {
          uVar4 = 10;
        }
        else
        {
          uVar4 = 1;
        }
        iVar7 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Reach", iVar7, uStack_1c,
                            "Trample", iVar7, uVar5, "Haste", iVar7, uVar4, 0, 0, 0, 0);
        if (iVar7 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (2 < iVar7 - 1U)
        {
          return 0;
        }
        iVar8 = charge_mana_for_activated_ability(player, card, 0, 0, 0, 1, 0, 0);
        if (iVar8 == 0)
        {
          return 0;
        }
        uVar6 = instance->targets[1].player;
        if ((int)uVar6 < -1)
        {
          instance->targets[1].player = 0;
          uVar6 = 0;
        }
        uVar1 = instance->targets[1].card;
        if ((int)uVar1 < -1)
        {
          uVar1 = 0;
        }
        if (iVar7 == 1)
        {
          uVar1 = uVar1 | 0x400;
        }
        else if (iVar7 == 2)
        {
          instance->targets[1].player = uVar6 | 0x80;
        }
        else if (iVar7 == 3)
        {
          uVar1 = uVar1 | 4;
        }
        instance->targets[1].card = uVar1;
        return 0;
      }
      goto LAB_023996a3;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      return 0;
    }
  }
  instance = get_card_instance(player, card);
LAB_023996a3:
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar7 = instance->info_slot;
    if (iVar7 == 1)
    {
      card_00 = instance->parent_card;
      player_00 = instance->parent_controller;
      iVar8 = 0;
      iVar7 = 0x400;
    }
    else if (iVar7 == 2)
    {
      card_00 = instance->parent_card;
      player_00 = instance->parent_controller;
      iVar8 = 0;
      iVar7 = 0x80;
    }
    else
    {
      if (iVar7 != 3)
      {
        return 0;
      }
      card_00 = instance->parent_card;
      player_00 = instance->parent_controller;
      iVar8 = 4;
      iVar7 = 0;
    }
    pump_ability_until_eot_no_repeat(player_00, card_00, player_00, card_00, iVar7, iVar8);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2394c80; original cards/m20.c:95. */
int card_ajani_strength_of_the_pride(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      instance->targets[0].card = -1;
      instance->targets[0].player = -1;
      iVar2 = get_starting_life_total(player);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Gain life", 1, 10,
                          1, "Pridemate token", 1, 10, 0xfffffffe, "Mega-exile", 1,
                          ((iVar2 + 0xe < life[player]) - 1 & 0xffffffce) + 0x19, 0, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_02394cd2;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02394cd2;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar2 = instance->info_slot;
    if (iVar2 == 1)
    {
      iVar2 = count_permanents_by_type(player, TYPE_CREATURE);
      iVar3 = count_permanents_by_type(player, TARGET_TYPE_PLANESWALKER);
      gain_life(player, iVar3 + iVar2);
    }
    else if (iVar2 == 2)
    {
      generate_token_by_id(player, card, 0x2b4f);
    }
    else if ((iVar2 == 3) && (iVar2 = get_starting_life_total(player), iVar2 + 0xe < life[player]))
    {
      instance = get_card_instance(player, card);
      iVar2 = instance->parent_card;
      instance = get_card_instance(player, card);
      kill_card(instance->parent_controller, iVar2, KILL_EXILE);
      default_test_definition(&this_test, 0x42);
      new_manipulate_all(player, card, 1 - player, &this_test, ACT_KILL_REMOVE);
    }
  }
LAB_02394cd2:
  iVar2 = planeswalker(player, card, event, 5);
  return iVar2;
}

/* ManalinkEh 2022: 0x2397690; original cards/m20.c:1449. */
int card_knight_of_the_ebon_legion(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  result = eot_trigger(player, card, event);
  if (((result != 0) && (current_turn == player)) &&
      ((result = get_trap_condition(player, 2),
        3 < result || (result = get_trap_condition(1 - player, 2), 3 < result))))
  {
    add_counters(player, card, player, card, COUNTER_P1_P1, 1);
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      instance = get_card_instance(player, card);
      pump_ability_until_eot(player, card, instance->parent_controller, instance->parent_card, 3, 3,
                             0, 0x200);
    }
  }
  result = generic_activated_ability(player, card, event, 0, 2, 1, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2398800; original cards/m20.c:1968. */
int card_mystic_forge(int player, int card, event_t event)
{
  unsigned int uVar1;
  int iVar2;
  int iVar3;
  card_instance_t *instance;
  uint32_t uVar5;
  uint32_t uVar6;

  if (event == EVENT_CAN_ACTIVATE)
  {

    iVar3 = *deck_ptr[player];
    if (iVar3 != -1)
    {
      iVar2 = is_what(-1, iVar3, 0x40);
      if ((((iVar2 != 0) ||
            ((iVar2 = get_color_by_internal_id(player, *deck_ptr[player]),
              iVar2 == 1 && (iVar2 = is_what(-1, *deck_ptr[player], 1), iVar2 == 0)))) &&
           (iVar2 = has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, iVar3), iVar2 != 0)) &&
          (iVar3 = can_legally_play_iid_now(player, iVar3, EVENT_CAN_ACTIVATE), iVar3 != 0))
      {
        return iVar3;
      }
      if (*deck_ptr[player] != -1)
      {
        return 1;
      }
    }
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 1,
                                      NULL, NULL);
    return (unsigned int)(iVar3 != 0);
  }
  if (event != EVENT_ACTIVATE)
  {
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event == EVENT_CLEANUP)
      {
        return 0;
      }
      iVar3 = global_enchantment(player, card, event);
      return iVar3;
    }
    instance = get_card_instance(player, card);
    if (instance->info_slot != 3)
    {
      return 0;
    }
    rfg_top_card_of_deck(player);
    return 0;
  }

  iVar3 = *deck_ptr[player];
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    uVar6 = 0;
    if (iVar3 != -1)
      goto LAB_02398883;
  LAB_02398ae0:
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 1,
                                      NULL, NULL);
    uVar1 = 0;
  }
  else
  {
    uVar6 = 1;
    if (iVar3 == -1)
    {
      uVar6 = 0;
      goto LAB_02398ae0;
    }
  LAB_02398883:
    iVar3 = is_what(-1, iVar3, 0x40);
    if (((iVar3 == 0) &&
         (((uVar1 = get_color_by_internal_id(player, *deck_ptr[player]),
            (uVar1 & 1) == 0 &&
                (iVar3 = get_color_by_internal_id(player, *deck_ptr[player]), iVar3 != 0)) ||
           (iVar3 = is_what(-1, *deck_ptr[player], 1), iVar3 != 0)))) ||
        (iVar3 = can_legally_play_iid_now(player, *deck_ptr[player], EVENT_ACTIVATE), iVar3 == 0))
      goto LAB_02398ae0;
    iVar2 = has_mana_to_cast_iid(player, EVENT_ACTIVATE, *deck_ptr[player]);
    uVar1 = (unsigned int)(iVar2 != 0);
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 1,
                                      NULL, NULL);
    if (iVar2 != 0)
    {
      uVar1 = 1;
      uVar5 = 0xfffffff6;
      goto LAB_02398b4b;
    }
  }
  uVar5 = 5;
LAB_02398b4b:
  iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016",
                      "Check top card of deck", uVar6, 0, "Play top card", uVar1, 10,
                      "Exile top of library", iVar3, uVar5, 0, 0, 0, 0);
  if (iVar3 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (iVar3 == 1)
  {
    show_deck(player, deck_ptr[player], 1, "Here\'s the top card of your deck", 0,
              ENGINE_DONE_LABEL);
    spell_fizzled = 1;
  }
  else if (iVar3 == 2)
  {
    iVar3 = charge_mana_from_id(player, -1, EVENT_ACTIVATE,
                                (unsigned int)cards_data[*deck_ptr[player]].id);
    if (iVar3 == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      play_card_in_deck_for_free(player, player, 0);
      cant_be_responded_to = 1;
    }
  }
  else if (iVar3 == 3)
  {
    iVar3 =
        generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 1, NULL, NULL);
    return iVar3;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2398f10; original cards/m20.c:2081. */
int card_omnath_locus_of_the_roil(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  bool bVar3;
  target_definition_t td;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  landfall(player, card, event);
  if ((event == EVENT_ETB) && (trigger_cause_controller == player))
  {
    if (trigger_cause == card)
    {
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      td.allow_cancel = 0;
      result = can_target(&td);
      if ((result != 0) && (result = pick_target(&td, "TARGET_ANY"), result != 0))
      {
        result = count_subtype(player, -1, 0x3a);
        damage_target0(player, card, result);
      }
      instance = get_card_instance(player, card);
      bVar3 = player != trigger_cause_controller;
      instance->number_of_targets = '\0';
      if (bVar3)
      {
        return 0;
      }
    }
    if (trigger_cause != card)
    {

      default_target_definition(player, card, &td, 0x47);
      td.allowed_controller = player;
      td.preferred_controller = player;
      td.required_subtype = 0x3a;
      td.allow_cancel = 0;
      instance = get_card_instance(player, card);
      result = can_target(&td);
      if ((result != 0) &&
          (result = new_pick_target(&td, "Select target Elemental.", 0, 0x80), result != 0))
      {
        add_counters(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 1);
      }
      instance->number_of_targets = '\0';
      result = count_permanents_by_type(player, TYPE_LAND);
      if (7 < result)
      {
        draw_a_card(player);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2399990; original cards/m20.c:2481. */
int card_sorin_imperious_bloodlord(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  uint32_t uVar3;
  target_definition_t *ptStack_19c;
  int iStack_198;
  target_definition_t *ptStack_194;
  target_definition_t td1;
  target_definition_t td2;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = player;
    td1.preferred_controller = player;
    default_target_definition(player, card, &td2, 0x102);
    td2.zone = 0x1200;
    new_default_test_definition(&test, 2, "Select a Vampire creture card.");
    test.subtype = 0xcd;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      instance->targets[0].card = -1;
      instance->targets[0].player = -1;
      iStack_198 = count_subtype_in_hand(player, SUBTYPE_VAMPIRE);
      if (iStack_198 != 0)
      {
        iStack_198 = 0xf;
      }
      result = count_subtype(player, 2, 0xcd);
      uVar3 = 0;
      if (result != 0)
      {
        uVar3 = 10;
      }
      result = can_target(&td1);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Pump a creature",
                           result, 10, 1, "Sac for damage", 1, uVar3, 1, "Put a Vampire into play",
                           1, iStack_198, 0xfffffffd, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 1)
      {
        new_pick_target(&td1, "TARGET_CREATURE_YOU_CONTROL", 0, 1);
      }
      goto LAB_02399a98;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02399a98;
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = player;
    td1.preferred_controller = player;
    default_target_definition(player, card, &td2, 0x102);
    td2.zone = 0x1200;
    new_default_test_definition(&test, 2, "Select a Vampire creture card.");
    test.subtype = 0xcd;
    instance = get_card_instance(player, card);
  }
  ptStack_194 = &td1;
  ptStack_19c = &td2;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    result = instance->info_slot;
    if (result == 1)
    {
      result = valid_target(ptStack_194);
      if (result != 0)
      {
        pump_ability_until_eot_no_repeat(instance->parent_controller, instance->parent_card,
                                         instance->targets[0].player, instance->targets[0].card, 0,
                                         0x202);
        result =
            has_subtype(instance->targets[0].player, instance->targets[0].card, SUBTYPE_VAMPIRE);
        if (result != 0)
        {
          add_counters(instance->parent_controller, instance->parent_card,
                                   instance->targets[0].player, instance->targets[0].card,
                                   COUNTER_P1_P1, 1);
        }
      }
    }
    else if (result == 2)
    {
      result = new_sacrifice(player, card, player, 0, &test);
      if (result == 0)
      {
        return 0;
      }
      result = pick_target(ptStack_19c, "TARGET_ANY");
      if (result != 0)
      {
        damage_target0(player, card, 3);
        gain_life(player, 3);
      }
    }
    else if (result == 3)
    {
      new_global_tutor(player, player, 4, 4, 0, 2, &test);
    }
  }
LAB_02399a98:
  result = planeswalker(player, card, event, 4);
  return result;
}

/* ManalinkEh 2022: 0x239acb0; original cards/m20.c:3014. */
int card_voracious_hydra(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event != EVENT_CAST_SPELL)
  {
    etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (event == EVENT_ETB)
    {
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = 1 - player;
      td.allow_cancel = 0;
      td.preferred_controller = td.allowed_controller;
      result = can_target(&td);
      result =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\003", "\001\016",
                      "Double +1/+1 counters", 1, 10, "Fight", result, 8, 0, 0, 0, 0);
      if (result == 1)
      {
        result = count_counters(player, card, COUNTER_P1_P1);
        add_counters(player, card, player, card, COUNTER_P1_P1, result);
      }
      else
      {
        result = can_target(&td);
        if (result != 0)
        {
          result = new_pick_target(&td, "TARGET_CREATURE_OPPONENT_CONTROLS", 0, 0);
          if (result != 0)
          {
            instance = get_card_instance(player, card);
            result = instance->targets[0].card;
            instance = get_card_instance(player, card);
            fight(player, card, instance->targets[0].player, result);
          }
        }
      }
    }
    return 0;
  }
  if ((card == affected_card) && (player == affected_card_controller))
  {
    result = is_token(player, card);
    if (result == 0)
    {
      generic_spell(player, card, EVENT_CAST_SPELL, 4, NULL, NULL, 0, NULL);
      instance = get_card_instance(player, card);
      enters_the_battlefield_with_counters(player, card, EVENT_CAST_SPELL, COUNTER_P1_P1,
                                           instance->info_slot);
    }
  }
  etb_ability(player, card, EVENT_CAST_SPELL, RESOLVE_TRIGGER_MANDATORY);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2396660; original cards/m20.c:801. */
int card_chandra_novice_pyromancer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  target_definition_t *ptStack_140;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      iVar2 = can_target(&td);
      iVar3 = can_produce_mana(player, card);
      iVar4 = count_subtype(player, 2, 0x3a);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Pump elementals",
                          1, iVar4, 1, "Add mana", iVar3, 2, 0xffffffff, "Deal 2 damage", iVar2, 2,
                          0xfffffffe, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 2)
      {
        produce_mana(player, 4, 2);
        tapped_for_mana_color = -2;
      }
      else if (iVar2 == 3)
      {
        new_pick_target(&td, "TARGET_ANY", 0, 1);
      }
      goto LAB_023966e0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_023966e0;
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    instance = get_card_instance(player, card);
  }
  ptStack_140 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->info_slot == 1)
    {
      default_test_definition(&test, 2);
      test.subtype = 0x3a;
      pump_creatures_until_eot_merge_pt(player, card, player, 2, 0, &test);
    }
    else if ((instance->info_slot == 3) && (iVar2 = valid_target(ptStack_140), iVar2 != 0))
    {
      damage_target0(player, card, 2);
    }
  }
LAB_023966e0:
  iVar2 = planeswalker(player, card, event, 5);
  return iVar2;
}

/* ManalinkEh 2022: 0x2397800; original cards/m20.c:1471. */
int card_kykar_winds_fury(int player, int card, event_t event)
{
  int result;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_token;

  prowess_mode(player, card, event, 2);
  if (event == EVENT_SPA)
  {
    default_token_definition(player, card, 0x498d, &recovered_stack_token.token);
    recovered_stack_token.token.pow = 1;
    recovered_stack_token.token.tou = 1;
    recovered_stack_token.token.color_forced = ' ';
    recovered_stack_token.token.key_plus = 0x20;
    generate_token(&recovered_stack_token.token);
  }
  else if (event == EVENT_COUNT_MANA)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_test_definition(&recovered_stack_token.test, 0x47);
      result = can_produce_mana(player, card);
      if ((result != 0) &&
          (result = new_can_sacrifice_as_cost(player, card, &recovered_stack_token.test),
           result != 0))
      {
        result = count_subtype(player, -1, 0xbc);
        declare_mana_available(player, 4, result);
      }
    }
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    default_test_definition(&recovered_stack_token.test, 0x47);
    result = can_produce_mana(player, card);
    if ((result != 0) &&
        (result = new_can_sacrifice_as_cost(player, card, &recovered_stack_token.test),
         result != 0))
    {
      return 1;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&recovered_stack_token.test, 0x47, "Select a spirit to sacrifice.");
    result = new_sacrifice(player, card, player, SAC_AS_COST, &recovered_stack_token.test);
    if (result == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      produce_mana(player, 4, 1);
      tapped_for_mana_color = -2;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2397df0; original cards/m20.c:1605. */
int card_leyline_of_abundance(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;

  result = is_humiliated(player, card, AL_TRIG);
  if ((((event - EVENT_COUNT_MANA & 0xfffffffd) == 0 && result == 0) &&
       (affected_card_controller == player)) &&
      (result = is_what(player, affected_card, 2), result != 0))
  {
    instance = in_play(player, card);
    if (instance == NULL)
    {
      return 0;
    }
    if (event == EVENT_COUNT_MANA)
    {
      result = is_tapped(affected_card_controller, affected_card);
      if (result != 0)
      {
        return 0;
      }
      instance = get_card_instance(affected_card_controller, affected_card);
      if ((instance->state & 0x20000) != 0)
      {
        return 0;
      }
      result = can_produce_mana(affected_card_controller, affected_card);
      if (result == 0)
      {
        return 0;
      }
      declare_mana_available(affected_card_controller, 3, 1);
      goto LAB_02397f3a;
    }
    if (-1 < tapped_for_mana_color)
    {
      produce_mana(affected_card_controller, 3, 1);
    }
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_02397e4d:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02397e56;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
    LAB_02397f3a:
      result = global_enchantment(player, card, event);
      return result;
    }
    goto LAB_02397e4d;
  }
  default_test_definition(&this_test, 2);
  new_manipulate_all(player, card, player, &this_test, ACT_ADD_COUNTERS_BASE | ACT_KILL_BURY);
LAB_02397e56:
  result = generic_activated_ability(player, card, event, 0, 6, 0, 0, 2, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2399230; original cards/m20.c:2219. */
int card_risen_reef(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  int iVar3;
  card_instance_t *instance;
  test_definition_t test;

  if ((((trigger_condition == 0xdb) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (iVar2 = is_humiliated(player, card, AL_TRIG), iVar2 == 0))
  {
    default_test_definition(&test, 0x47);
    test.subtype = 0x3a;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {

    piVar1 = deck_ptr[player];
    iVar2 = *piVar1;
    if (iVar2 != -1)
    {
      if ((player == 1) || (ai_is_speculating == 1))
      {
        iVar2 = is_what(-1, iVar2, 1);
      }
      else
      {
        iVar2 = is_what(-1, iVar2, 1);
        if (iVar2 == 0)
        {
          reveal_card_iid(player, card, *piVar1);
          iVar2 = *piVar1;
          remove_card_from_deck(player, 0);
          add_card_to_hand(player, iVar2);
          return 0;
        }
        iVar2 = reveal_card_optional_iid(player, card, *piVar1, "Put on the battlefield tapped");
      }
      iVar3 = *piVar1;
      remove_card_from_deck(player, 0);
      iVar3 = add_card_to_hand(player, iVar3);
      if (iVar2 != 0)
      {
        instance = get_card_instance(player, iVar3);
        instance->state = instance->state | 0x10;
        put_into_play(player, iVar3);
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2395f20; original cards/m20.c:683. */
int card_chandra_awakened_inferno(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  unsigned int uVar3;
  int iVar4;
  int iVar5;
  int iStack_194;
  target_definition_t *ptStack_190;
  card_instance_t *pcStack_18c;
  token_generation_t token;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  cannot_be_countered(player, card, event);
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02395fdf;
    default_test_definition(&test, 2);
    test.subtype = 0x3a;
    test.subtype_flag = 1;
    default_target_definition(player, card, &td, 0x102);
    if (player == 1)
    {
      uVar3 = count_counters(1, card, COUNTER_LOYALTY);
      td.toughness_requirement = uVar3 | 0x2000;
      pcStack_18c = get_card_instance(1, card);
    }
    else
    {
      pcStack_18c = get_card_instance(player, card);
      if (event == EVENT_ACTIVATE)
        goto LAB_0239606a;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      iVar1 = pcStack_18c->info_slot;
      if (iVar1 == 1)
      {
        default_token_definition(player, card, 0x4f53, &token);
        token.t_player = 1 - player;
        generate_token(&token);
      }
      else if (iVar1 == 2)
      {
        new_damage_all(player, card, 2, 3, 0, &test);
      }
      else if ((iVar1 == 3) && (iVar1 = valid_target(&td), iVar1 != 0))
      {

        iVar1 = damage_target0(player, card, pcStack_18c->targets[1].player);
        pcVar2 = get_card_instance(player, iVar1);
        pcVar2->targets[3].card = pcVar2->targets[3].card | 0x80;
      }
    }
    goto LAB_02395fdf;
  }
  default_test_definition(&test, 2);
  test.subtype = 0x3a;
  test.subtype_flag = 1;
  default_target_definition(player, card, &td, 0x102);
  if (player == 1)
  {
    uVar3 = count_counters(1, card, COUNTER_LOYALTY);
    td.toughness_requirement = uVar3 | 0x2000;
    pcStack_18c = get_card_instance(1, card);
    pcStack_18c->number_of_targets = '\0';
    pcStack_18c->info_slot = 0;
    iVar5 = count_counters(1, card, COUNTER_LOYALTY);
    iVar1 = current_turn;
    if (iVar5 < 2)
      goto LAB_0239607b;

    iStack_194 = 0;
    iVar5 = active_cards_count[current_turn] + -1;
    if (-1 < iVar5)
    {
      do
      {
        pcVar2 = in_play(iVar1, iVar5);
        if (((pcVar2 != NULL) &&
             (iVar4 = has_subtype(iVar1, iVar5, SUBTYPE_ELEMENTAL), iVar4 == 0)) &&
            (iVar4 = is_what(iVar1, iVar5, 2), iVar4 != 0))
        {
          iVar4 = get_toughness(iVar1, iVar5);
          pcVar2 = get_card_instance(iVar1, iVar5);
          if (((int)(iVar4 - (unsigned int)pcVar2->damage_on_card) < 4) &&
              (iVar4 = check_for_special_ability(iVar1, iVar5, 0x800), iVar4 == 0))
          {
            if (iVar1 == 1)
            {
              iStack_194 = iStack_194 + -5;
            }
            else
            {
              iStack_194 = iStack_194 + 5;
            }
          }
        }
        iVar5 = iVar5 + -1;
      } while (iVar5 != -1);
    }
    iVar1 = 1 - current_turn;
    iVar5 = active_cards_count[iVar1] + -1;
    if (-1 < iVar5)
    {
      do
      {
        pcVar2 = in_play(iVar1, iVar5);
        if (((pcVar2 != NULL) &&
             (iVar4 = has_subtype(iVar1, iVar5, SUBTYPE_ELEMENTAL), iVar4 == 0)) &&
            (iVar4 = is_what(iVar1, iVar5, 2), iVar4 != 0))
        {
          iVar4 = get_toughness(iVar1, iVar5);
          pcVar2 = get_card_instance(iVar1, iVar5);
          if (((int)(iVar4 - (unsigned int)pcVar2->damage_on_card) < 4) &&
              (iVar4 = check_for_special_ability(iVar1, iVar5, 0x800), iVar4 == 0))
          {
            if (iVar1 == 1)
            {
              iStack_194 = iStack_194 + -5;
            }
            else
            {
              iStack_194 = iStack_194 + 5;
            }
          }
        }
        iVar5 = iVar5 + -1;
      } while (iVar5 != -1);
    }
  }
  else
  {
    pcStack_18c = get_card_instance(player, card);
  LAB_0239606a:

    pcStack_18c->number_of_targets = '\0';
    pcStack_18c->info_slot = 0;
  LAB_0239607b:
    iStack_194 = 0;
  }
  ptStack_190 = &td;
  iVar1 = can_target(ptStack_190);
  iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Emblem", 1,
                      0xf - life[1 - player], 2, "Damage non-Elementals", 1, iStack_194, 0xfffffffd,
                      "Damage creature or planeswalker", iVar1, 0xf, 0, 0, 0, 0, 0);
  if (iVar1 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (iVar1 != 3)
    goto LAB_02395fdf;

  iVar1 = pick_target(ptStack_190, "TARGET_CREATURE_OR_PLANESWALKER");
  if (iVar1 == 0)
  {
    if (spell_fizzled == 1)
      goto LAB_02395fdf;
  }
  else
  {

    iVar1 = count_counters(player, card, COUNTER_LOYALTY);
    iVar5 = is_planeswalker(pcStack_18c->targets[0].player, pcStack_18c->targets[0].card);
    if (iVar5 == 0)
    {
      iVar5 = get_toughness(pcStack_18c->targets[0].player, pcStack_18c->targets[0].card);
    }
    else
    {
      iVar5 = count_counters(pcStack_18c->targets[0].player, pcStack_18c->targets[0].card,
                             COUNTER_LOYALTY);
    }
    if (iVar1 < iVar5)
    {
      iVar5 = iVar1;
    }
    if (player == 0)
    {
      iVar5 = choose_a_number(0, "Pay how much Loyalty ?", iVar5);
    }
    if (spell_fizzled == 1)
      goto LAB_02395fdf;
    if ((0 < iVar5) && (iVar1 = count_counters(player, card, COUNTER_LOYALTY), iVar5 <= iVar1))
    {
      remove_counters(player, card, player, card, COUNTER_LOYALTY, iVar5);
      pcStack_18c->targets[1].player = iVar5;
      goto LAB_02395fdf;
    }
  }
  spell_fizzled = 1;
LAB_02395fdf:
  iVar1 = planeswalker(player, card, event, 6);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2398010; original cards/m20.c:1648. */
int card_leyline_of_combustion(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  const target_t *ptVar3;

  instance = in_play(player, card);
  if (instance != NULL)
  {
    result = is_humiliated(player, card, AL_TRIG);
    if (result == 0)
    {
      ptVar3 = any_becomes_target(player, card, event, player, -1, TYPE_PERMANENT, -1, TYPE_ANY,
                                  1 - player, RESOLVE_TRIGGER_MANDATORY);
      if (ptVar3 != NULL)
      {
        damage_player(1 - player, 2, player, card);
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x239af40; original cards/m20.c:3124. */
int card_yarok_the_desecrated(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  deathtouch(player, card, event);
  lifelink(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    card_counts[player][4] = card_counts[player][4] + '\x01';
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2397310; original cards/m20.c:1377. */
int card_kaalia_zenith_seeker(int player, int card, event_t event)
{
  unsigned char bVar1;
  bool bVar2;
  int amount;
  int iVar3;
  unsigned int uVar4;
  int iVar5;
  bool bVar6;
  test_definition_t test;

  check_legend_rule(player, card, event);
  vigilance(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if ((event == EVENT_ETB) && (*deck_ptr[player] != -1))
  {

    show_deck(0, deck_ptr[player], 4, "Revealed", 0, ENGINE_DONE_LABEL);

    amount = count_deck(player);
    if (4 < amount)
    {
      amount = 4;
    }
    if (0 < amount + -1)
    {
      iVar5 = 0;
      bVar1 = 0;
      bVar2 = false;
      bVar6 = false;
      do
      {
        iVar3 = has_subtype_by_id(
            player, (unsigned int)cards_data[*(int *)((int)deck_ptr[player] + iVar5)].id,
            SUBTYPE_ANGEL);
        if (iVar3 != 0)
        {
          bVar6 = true;
        }
        iVar3 = has_subtype_by_id(
            player, (unsigned int)cards_data[*(int *)((int)deck_ptr[player] + iVar5)].id,
            SUBTYPE_DEMON);
        if (iVar3 != 0)
        {
          bVar2 = true;
        }
        iVar3 = has_subtype_by_id(
            player, (unsigned int)cards_data[*(int *)((int)deck_ptr[player] + iVar5)].id,
            SUBTYPE_DRAGON);
        if (iVar3 != 0)
        {
          bVar1 = 1;
        }
        iVar5 = iVar5 + 4;
      } while (amount * 4 + -4 != iVar5);
      if (bVar6)
      {
        new_default_test_definition(&test, 0xff, "Select an Angel card.");
        test.subtype = 2;
        test.no_shuffle = 2;
        test.create_minideck = (unsigned short)amount;
        iVar5 = new_global_tutor(player, player, 1, 1, 0, 1, &test);
        if (-1 < iVar5)
        {
          amount = amount + -1;
        }
      }
      bVar6 = *deck_ptr[player] != -1;
      if ((bVar2) && (bVar6))
      {
        new_default_test_definition(&test, 0xff, "Select a Demon card.");
        test.subtype = 0x2c;
        test.no_shuffle = 2;
        test.create_minideck = (unsigned short)amount;
        iVar5 = new_global_tutor(player, player, 1, 1, 0, 1, &test);
        if (-1 < iVar5)
        {
          amount = amount + -1;
        }
        bVar6 = *deck_ptr[player] != -1;
      }
      if ((bool)(bVar1 & bVar6))
      {
        new_default_test_definition(&test, 0xff, "Select a Dragon card.");
        test.create_minideck = (unsigned short)amount;
        test.subtype = 0x30;
        test.no_shuffle = 2;
        uVar4 = new_global_tutor(player, player, 1, 1, 0, 1, &test);
        amount = amount - (unsigned int)(uVar4 < 0x80000000);
      }
    }
    put_top_x_on_bottom_in_random_order(player, amount);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2399e40; original cards/m20.c:2613. */
int card_tales_end(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td_spell;

  instance = get_card_instance(player, card);
  counterspell_target_definition(player, card, &td_spell, 0);
  td_spell.special = 0x8000;
  td_spell.extra = (int32_t)is_legendary_by_iid;
  if (event == EVENT_CAN_CAST)
  {
    result = counterspell(player, card, EVENT_CAN_CAST, &td_spell, 0);
    if (result != 0)
    {
      instance->info_slot = 1;
      return 99;
    }
    result = can_counter_activated_ability(player, card, EVENT_CAN_CAST, NULL);
    if (result != 0)
    {
      instance->info_slot = 2;
      return 99;
    }
    result = can_counter_triggered_ability(player, card, EVENT_CAN_CAST, NULL);
    if (result != 0)
    {
      instance->info_slot = 3;
      return 99;
    }
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        result = instance->info_slot;
        instance->number_of_targets = '\0';
        if (result == 1)
        {
          counterspell(player, card, EVENT_CAST_SPELL, &td_spell, 0);
          return 0;
        }
        if (result != 2)
        {
          if (result != 3)
          {
            return 0;
          }
          cast_counter_triggered_ability(player, card, 0);
          return 0;
        }
        cast_counter_activated_ability(player, card, 0);
        return 0;
      }
    LAB_02399ed4:
      result = counterspell(player, card, event, &td_spell, 0);
      return result;
    }
    if (event != EVENT_RESOLVE_SPELL)
      goto LAB_02399ed4;
    result = instance->info_slot;
    if (result == 1)
    {
      counterspell(player, card, EVENT_RESOLVE_SPELL, &td_spell, 0);
      return 0;
    }
    if (result == 2)
    {
      result = resolve_counter_activated_ability(player, card, NULL, 0);
      if (result != 0)
        goto LAB_0239a024;
      result = instance->info_slot;
    }
    if ((result == 3) &&
        (result = resolve_counter_triggered_ability(player, card, NULL, 0), result != 0))
    {
    LAB_0239a024:
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int hexpr_ub_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2394850; original cards/m20.c:2819. */
static int hexpr_ub_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  char cVar3;

  attached_gets_sp_kw2(player, card, event, SP_KEYWORD2_GRANTED_PROT_MISC);
  instance = get_card_instance(player, card);
  if (event == EVENT_PROT_MISC)
  {
    cVar3 = (char)((unsigned int)event_result >> 0x18);
    if ((cVar3 == '\x06') ||
        ((((cVar3 == '\x05' && (attacking_card_controller != player)) &&
           (instance->damage_target_card == affected_card)) &&
          ((instance->damage_target_player == affected_card_controller &&
            (uVar2 = get_color(attacking_card_controller, attacking_card), (uVar2 & 6) != 0))))))
    {
      event_result = event_result | 1;
      return 0;
    }
  }
  else if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x239a350; original cards/m20.c:2831. */
int card_veil_of_summer(int player, int card, event_t event)
{
  int iVar1;
  color_t cVar2;
  card_instance_t *instance;
  int iVar4;

  if (event == EVENT_CAN_CAST)
    goto LAB_0239a381;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
    {
      return 0;
    }
    goto LAB_0239a381;
  }
  if (event != EVENT_RESOLVE_SPELL)
  {
    if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
    goto LAB_0239a381;
  }

  cVar2 = get_sleighted_color(player, card, 2);
  iVar1 = get_stormcolor_count(1 - player, cVar2);
  if (iVar1 == 0)
  {
    cVar2 = get_sleighted_color(player, card, 1);
    iVar1 = get_stormcolor_count(1 - player, cVar2);
    if (iVar1 != 0)
      goto LAB_0239a4e0;
  }
  else
  {
  LAB_0239a4e0:
    draw_a_card(player);
  }
  create_legacy_effect(player, card, vos_legacy);
  if (0 < active_cards_count[player])
  {
    iVar1 = 0;
    do
    {
      while ((instance = in_play(player, iVar1),
              instance == NULL || (iVar4 = is_what(player, iVar1, 0x47), iVar4 == 0)))
      {
        iVar1 = iVar1 + 1;
        if (active_cards_count[player] <= iVar1)
          goto LAB_0239a4c0;
      }
      iVar4 = iVar1 + 1;
      create_targetted_legacy_effect(player, card, hexpr_ub_legacy, player, iVar1);
      iVar1 = iVar4;
    } while (iVar4 < active_cards_count[player]);
  }
LAB_0239a4c0:
  kill_card(player, card, KILL_DESTROY);
LAB_0239a381:
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2394370; original cards/m20.c:591. */
static int can_cast_this(int iid, int unused, int player, int card)
{
  int result;

  result = has_mana_to_cast_iid(player, EVENT_CAN_CAST, iid);
  return result;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23982c0; original cards/m20.c:1863. */
int card_mu_yanling_sky_dancer(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  target_definition_t *ptStack_b4;
  token_generation_t token;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    td.allow_cancel = 3;
    pcVar1 = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      pcVar1->number_of_targets = '\0';
      pcVar1->info_slot = 0;
      pcVar1->targets[0].card = -1;
      pcVar1->targets[0].player = -1;
      iVar2 = check_battlefield_for_id(player, 0x4f99);
      iVar4 = -0x19;
      if (iVar2 == 0)
      {
        iVar2 = get_hacked_subtype(player, card, 0x5002);
        iVar2 = count_subtype(player, 1, iVar2);
        iVar4 = count_counters(player, card, COUNTER_LOYALTY);
        iVar4 = iVar2 * 5 + iVar4 * 5 + -0x19;
      }
      iVar2 = can_target(&td);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Debuff a creature",
                          1, iVar2 * 10, 2, "Elemental Bird token", 1, 10, 0xfffffffd, "Emblem", 1,
                          iVar4, 0xfffffff8, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 1)
      {
        pcVar1->number_of_targets = '\0';
        new_pick_target(&td, "TARGET_CREATURE", 0, 0);
        if (pcVar1->targets[0].card == -1)
        {
          spell_fizzled = 1;
        }
      }
      goto LAB_0239834e;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_0239834e;
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    td.allow_cancel = 3;
    pcVar1 = get_card_instance(player, card);
  }
  ptStack_b4 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar2 = pcVar1->info_slot;
    if (iVar2 == 1)
    {
      iVar2 = valid_target(ptStack_b4);
      if (iVar2 != 0)
      {
        create_targetted_legacy_effect(player, card, fx_yanling_debuff, pcVar1->targets[0].player,
                                       pcVar1->targets[0].card);
      }
    }
    else if (iVar2 == 2)
    {
      default_token_definition(player, card, 0x4f65, &token);
      token.tou = 4;
      token.pow = 4;
      token.color_forced = '\x04';
      generate_token(&token);
    }
    else if (iVar2 == 3)
    {

      iVar2 = generate_token_by_id(player, card, 0x4f99);
      pcVar3 = get_card_instance(player, iVar2);
      iVar2 = get_hacked_subtype(pcVar1->parent_controller, pcVar1->parent_card, 0x5002);
      pcVar3->targets[1].player = iVar2;
      pcVar3->hack_mode[2] = pcVar1->hack_mode[2];
    }
  }
LAB_0239834e:
  iVar2 = planeswalker(player, card, event, 2);
  return iVar2;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x239a660; original cards/m20.c:2897. */
int card_vivien_arkbow_ranger(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int amt;
  unsigned int uVar4;
  target_definition_t *ptStack_1f0;
  target_definition_t *ptStack_1ec;
  target_definition_t *ptStack_1e8;
  target_definition_t td;
  target_definition_t td_control;
  target_definition_t td_cp;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    default_target_definition(player, card, &td_control, 2);
    td_control.preferred_controller = player;
    td_control.allowed_controller = player;
    default_target_definition(player, card, &td_cp, 0x102);
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      instance->targets[0].card = -1;
      instance->targets[0].player = -1;
      iVar2 = can_target(&td_control);
      uVar4 = 0;
      if (iVar2 != 0)
      {
        iVar2 = can_target(&td_cp);
        uVar4 = (unsigned int)(iVar2 != 0);
      }
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                          "Add two +1/+1 counters", 1, 10, 1, "A creature deals damage", uVar4, 10,
                          0xfffffffd, "Living Wish", 1, 0x14, 0xfffffffb, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 1)
      {
        iVar2 = target_available(player, card, &td);
        if (iVar2 == 1)
        {
          pick_target(&td, "ASHNODS_BATTLEGEAR");
        }
        else
        {

          iVar2 = new_pick_target(&td, "ASHNODS_BATTLEGEAR", 0, 0);
          if ((iVar2 == 0) ||
              (iVar2 = new_pick_target(&td, "ASHNODS_BATTLEGEAR", 1, 0), iVar2 == 0))
          {
            spell_fizzled = 1;
          }
        }
      }
      else if ((iVar2 == 2) &&
               ((iVar2 = new_pick_target(&td_control, "ASHNODS_BATTLEGEAR", 0, 1),
                 iVar2 == 0 ||
                     (iVar2 = new_pick_target(&td_cp, "TARGET_CREATURE_OR_PLANESWALKER", 1, 1),
                      iVar2 == 0))))
      {
        instance->number_of_targets = '\0';
      }
      goto LAB_0239a736;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_0239a736;
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    default_target_definition(player, card, &td_control, 2);
    td_control.preferred_controller = player;
    td_control.allowed_controller = player;
    default_target_definition(player, card, &td_cp, 0x102);
    instance = get_card_instance(player, card);
  }
  ptStack_1e8 = &td_control;
  ptStack_1ec = &td;
  ptStack_1f0 = &td_cp;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar2 = instance->info_slot;
    if (iVar2 == 1)
    {

      if (instance->number_of_targets == '\x01')
      {
        iVar2 = valid_target(ptStack_1ec);
        if (iVar2 != 0)
        {
          add_counters(player, card, instance->targets[0].player,
                                   instance->targets[0].card, COUNTER_P1_P1, 2);
          pump_ability_until_eot_no_repeat(instance->parent_controller, instance->parent_card,
                                                instance->targets[0].player,
                                                instance->targets[0].card, 0x80, 0);
        }
      }
      else
      {
        iVar2 = 0;
        do
        {
          iVar3 = validate_target(player, card, ptStack_1ec, iVar2);
          if (iVar3 != 0)
          {
            add_counters(player, card, instance->targets[iVar2].player,
                                     instance->targets[iVar2].card, COUNTER_P1_P1, 1);
            pump_ability_until_eot_no_repeat(
                instance->parent_controller, instance->parent_card, instance->targets[iVar2].player,
                instance->targets[iVar2].card, 0x80, 0);
          }
          iVar2 = iVar2 + 1;
        } while (iVar2 != 2);
      }
    }
    else if (iVar2 == 2)
    {
      iVar2 = valid_target(ptStack_1e8);
      if ((iVar2 != 0) && (iVar2 = validate_target(player, card, ptStack_1f0, 1), iVar2 != 0))
      {
        iVar2 = instance->targets[0].card;
        iVar3 = instance->targets[0].player;
        amt = get_power(iVar3, iVar2);
        damage_creature(instance->targets[1].player, instance->targets[1].card, amt, iVar3, iVar2);
      }
    }
    else if (iVar2 == 3)
    {
      default_test_definition(&test, 2);
      generic_wish_effect(player, card, &test, 0x4fb7);
    }
  }
LAB_0239a736:
  iVar2 = planeswalker(player, card, event, 4);
  return iVar2;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2395ad0; original cards/m20.c:594. */
int card_chandra_acolyte_of_flame(int player, int card, event_t event)
{
  int *piVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  card_instance_t *pcVar5;
  unsigned int uVar6;
  unsigned int uVar7;
  uint32_t uVar8;
  token_generation_t token;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    new_default_test_definition(
        &test, 0x38, "Select an instant or sorcery card with converted mana cost 3 or less.");
    test.cmc = 4;
    test.cmc_flag = 3;
    test.type_flag = 2;
    test.special_selection_function = can_cast_this;
    pcVar2 = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      pcVar2->number_of_targets = '\0';
      pcVar2->info_slot = 0;

      iVar3 = count_graveyard(player);
      piVar1 = graveyard_ptr_mutable[player];
      if (-1 < iVar3)
      {
        do
        {
          iVar4 = is_what(-1, piVar1[iVar3], 0x38);
          if (((iVar4 != 0) && (iVar4 = get_cmc_by_internal_id(piVar1[iVar3]), iVar4 < 4)) &&
              (iVar4 = has_mana_to_cast_id(player, EVENT_CAN_CAST,
                                           (unsigned int)cards_data[piVar1[iVar3]].id),
               iVar4 != 0))
          {
            uVar8 = 0x14;
            goto LAB_02395dad;
          }
          iVar3 = iVar3 + -1;
        } while (iVar3 != -1);
      }
      uVar8 = 0xffffffec;
    LAB_02395dad:
      iVar3 = new_special_count_grave(player, &test);
      iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Add loyalty", 1, 5,
                          0, "Elemental tokens", 1, 10, 0, "Cast from graveyard", iVar3, uVar8,
                          0xfffffffe, 0, 0, 0, 0);
      if ((iVar3 == 0) || ((iVar3 == 3 && (iVar3 = select_target_from_grave_source(
                                               player, card, player, SFG_0, 2, &test, 1),
                                           iVar3 == -1))))
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_02395b64;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_02395b64;
    new_default_test_definition(
        &test, 0x38, "Select an instant or sorcery card with converted mana cost 3 or less.");
    test.cmc = 4;
    test.cmc_flag = 3;
    test.type_flag = 2;
    test.special_selection_function = can_cast_this;
    pcVar2 = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar3 = pcVar2->info_slot;
    if (iVar3 == 1)
    {

      iVar3 = active_cards_count[player] + -1;
      if (-1 < iVar3)
      {
        do
        {
          pcVar5 = in_play(player, iVar3);
          if ((pcVar5 != NULL) && (iVar4 = is_planeswalker(player, iVar3), iVar4 != 0))
          {
            uVar6 = get_color(player, iVar3);
            uVar7 = get_sleighted_color_test(pcVar2->parent_controller, pcVar2->parent_card, 0x10);
            if ((uVar6 & uVar7) != 0)
            {
              add_counters(player, card, player, iVar3, COUNTER_LOYALTY, 1);
            }
          }
          iVar3 = iVar3 + -1;
        } while (iVar3 != -1);
      }
    }
    else if (iVar3 == 2)
    {
      default_token_definition(player, card, 0x4926, &token);
      token.tou = 1;
      token.pow = 1;
      token.qty = '\x02';
      token.special_code_on_generation = haste_and_sacrifice_token_at_end_of_next_turn;
      generate_token(&token);
    }
    else
    {

      if (((iVar3 == 3) &&
           (iVar3 = validate_target_from_grave_source(player, card, player, 1), iVar3 != -1)) &&
          (iVar4 = charge_mana_from_id(
               player, -1, EVENT_RESOLVE_ACTIVATION,
               (unsigned int)cards_data[graveyard_ptr_mutable[player][iVar3]].id),
           iVar4 != 0))
      {
        play_card_in_grave_for_free_and_exile_it(player, player, iVar3);
      }
    }
  }
LAB_02395b64:
  iVar3 = planeswalker(player, card, event, 4);
  return iVar3;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x23955e0; original cards/m20.c:431. */
int card_cavalier_of_gales(int player, int card, event_t event)
{
  int result;
  int card_00;
  int owner;
  int position;
  test_definition_t this_test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_cards(player, 3);
    new_default_test_definition(&this_test, 0xff, "Select a card to put on top.");
    result = hand_count[player];
    if (2 < result)
    {
      result = 2;
    }
    while (0 < result)
    {
      card_00 = new_select_a_card(player, player, 4, 1, 3, -1, &this_test);
      if (card_00 != -1)
      {
        put_on_top_of_deck(player, card_00);
        result = result + -1;
      }
    }
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if ((result != 0) &&
      (result = find_in_owners_graveyard(player, card, &owner, &position), result != 0))
  {
    from_graveyard_to_deck(owner, position, 3);
    scry(player, 2);
    return 0;
  }
  return 0;
}

#endif
