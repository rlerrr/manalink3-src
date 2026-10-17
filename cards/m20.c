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
  target_definition_t td;
  vigilance(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 1;
    td.allow_cancel = 1;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "TARGET_NONLAND_PERMANENT", 0, 0);
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
        new_default_test_definition((test_definition_t *)&td, 0x44,
                                    "Select an Artifact or Enchantment card.");
        new_global_tutor(player, player, 2, 1, 0, 1, (test_definition_t *)&td);
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
