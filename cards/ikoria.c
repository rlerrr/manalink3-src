#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22888b0; original cards/ikoria.c:191. */
int card_colossification(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    instance = get_card_instance(player, card);
    tap_card((int)instance->damage_target_player, instance->damage_target_card);
  }
  else
  {
    if (event == EVENT_CAN_CAST)
      goto LAB_02288900;
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_02288900;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_02288900;
  }
  if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
       (((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
         (event != EVENT_CHANGE_TYPE)))) &&
      (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
        (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_02288900:
  result = generic_aura_2022(player, card, event, player, 0x14, 0x14, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x22894f0; original cards/ikoria.c:564. */
int card_fire_prophecy(int player, int card, event_t event)
{
  int result;
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
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            damage_target0(player, card, 3);
            default_test_definition(&test, 0xff);
            result = new_global_tutor(player, player, 4, 10, 0, 3, &test);
            if (result != -2)
            {
              draw_a_card(player);
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0228954b;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
LAB_0228954b:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2289b70; original cards/ikoria.c:753. */
int card_glimmerbell(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      untap_card(instance->parent_controller, instance->parent_card);
    }
  }
  result = generic_activated_ability(player, card, event, 0, 1, 0, 1, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2289d70; original cards/ikoria.c:888. */
int card_imposing_vantasaur(int player, int card, event_t event)
{
  int result;
  vigilance_2022(player, card, event);
  result = cycling(player, card, event, 1, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2289de0; original cards/ikoria.c:919. */
int card_inspired_ultimatum(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td_any;
  target_definition_t td_player;
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td_any, 0x102);
    td_any.zone = 0x1200;
    default_target_definition(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td_any, NULL, 2, NULL);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td_any, 0x102);
      td_any.zone = 0x1200;
      default_target_definition(player, card, &td_player, 0);
      td_player.zone = 0x1000;
      if ((card == affected_card) &&
          ((player == affected_card_controller &&
            (result = new_pick_target(&td_player, "Select target player to gain life.", 0, 0x81),
             result != 0))))
      {
        new_pick_target(&td_any, "Select target for damage.", 1, 0x81);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td_any, 0x102);
    td_any.zone = 0x1200;
    default_target_definition(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    if (event == EVENT_RESOLVE_SPELL)
    {
      instance = get_card_instance(player, card);
      result = valid_target(&td_player);
      if (result != 0)
      {
        gain_life(instance->targets[0].player, 5);
      }
      result = validate_target(player, card, &td_any, 1);
      if (result != 0)
      {
        damage_creature(instance->targets[1].player, instance->targets[1].card, 5, player, card);
      }
      result = valid_target(&td_player);
      if ((result != 0) || (result = validate_target(player, card, &td_any, 1), result != 0))
      {
        draw_cards(player, 5);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td_any, 0x102);
    td_any.zone = 0x1200;
    default_target_definition(player, card, &td_player, 0);
  }
  return 0;
}

/* ManalinkEh 2022: 0x228acb0; original cards/ikoria.c:1231. */
int card_lava_serpent(int player, int card, event_t event)
{
  int result;
  haste(player, card, event);
  result = cycling(player, card, event, 2, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x228bee0; original cards/ikoria.c:1955. */
int card_ruinous_ultimatum(int player, int card, event_t event)
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
      default_test_definition(&test, 1);
      test.type_flag = 1;
      new_manipulate_all(player, card, 1 - player, &test, ACT_KILL_DESTROY);
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

/* ManalinkEh 2022: 0x22884c0; original cards/ikoria.c:2222. */
static int has_counter(int unused1, int unused2, int player, int card)
{
  card_instance_t *instance;
  int result;
  instance = get_card_instance(player, card);
  if (instance->internal_card_id == activation_card)
  {
    instance = get_card_instance(instance->parent_controller, instance->parent_card);
  }
  result = (unsigned int)instance->special_counters + (unsigned int)instance->counters +
           (unsigned int)instance->counters_m1m1;
  if ((char)instance->targets[0x12].player != -1)
  {
    result = result + (unsigned int)instance->counters2;
  }
  if (*(char *)((int)&instance->targets[0x12].player + 1) != -1)
  {
    result = result + (unsigned int)instance->counters3;
  }
  if (*(char *)((int)&instance->targets[0x12].player + 2) != -1)
  {
    result = result + (unsigned int)instance->counters4;
  }
  if (*(char *)((int)&instance->targets[0x12].player + 3) != -1)
  {
    result = result + (unsigned int)instance->counters5;
  }
  if (*(char *)((int)&instance->targets[0x12].card + 2) != -1)
  {
    result = result + (unsigned int)(unsigned char)instance->targets[0x12].card;
  }
  if (*(char *)((int)&instance->targets[0x12].card + 3) == -1)
  {
    return result;
  }
  return result + (unsigned int)*(unsigned char *)((int)&instance->targets[0x12].card + 1);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2288b90; original cards/ikoria.c:270. */
int card_dire_tactics(int player, int card, event_t event)
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
            result = check_battlefield_for_subtype(player, 0x47, 0x5b);
            if (result == 0)
            {
              result = get_toughness(instance->targets[0].player, instance->targets[0].card);
              lose_life(player, result);
            }
            kill_target0(player, card, KILL_EXILE);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02288bf6;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_02288bf6:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2289690; original cards/ikoria.c:653. */
int card_general_kudro_of_drannith(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  boost_creature_type(player, card, event, SUBTYPE_HUMAN, 1, 1, 0, BCT_CONTROLLER_ONLY);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0x47);
    test.subtype = 0x5b;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    iVar1 = 1 - player;
    if ((*graveyard_ptr_mutable[iVar1] != -1) && (iVar2 = graveyard_has_shroud(iVar1), iVar2 == 0))
    {

      new_default_test_definition(&test, 0xff, "Select target card.");
      iVar2 = new_select_target_from_grave(player, card, iVar1, SFG_CANNOT_CANCEL, 1, &test, 0);
      if (iVar2 != -1)
      {
        rfg_card_from_grave(iVar1, iVar2);
      }
    }
  }
  else if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
           ((event == EVENT_RESOLVE_ACTIVATION || (event == EVENT_CLEANUP))))
  {
    default_target_definition(player, card, &td, 2);
    td.power_requirement = 0x1004;
    if ((event == EVENT_RESOLVE_ACTIVATION) && (iVar1 = valid_target(&td), iVar1 != 0))
    {
      kill_target0(player, card, KILL_DESTROY);
    }
    new_default_test_definition(&test, 0x47, "Select a Human to sacrifice.");
    test.subtype = 0x5b;
    test.qty = 2;
    iVar1 = generic_activated_ability_with_sacrifice_as_cost(
        player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, 2, 0, 0, 0, 0, 0, 0, &td,
        "Select target creature with power 4 or greater.", TYPE_NONE, &test, SARV_0);
    return iVar1;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int iid_has_cycling(int iid, int me, int player, int card);

/* ManalinkEh 2022: 0x2288800; original cards/ikoria.c:2. */
int triome(int player, int card, event_t event)
{
  int result;

  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result == 0)
  {
    comes_into_play_tapped(player, card, event);
  }
  if (2 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    result = plain_mana_producer(player, card, event);
    return result;
  }
  result = cycling(player, card, event, 3, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2288a10; original cards/ikoria.c:232. */
int card_cunning_nightbonder(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
  LAB_02288a5c:
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result != 0)
      goto LAB_02288a38;

    if (((affected_card_controller == player) &&
         (result = is_what(player, affected_card, 0x47), result != 0)) &&
        ((result = get_original_internal_card_id(affected_card_controller, affected_card),
          (cards_data[result].type & 0x10) != 0 ||
              ((result = is_what(affected_card_controller, affected_card, 2),
                result != 0 && (result = check_battlefield_for_id(affected_card_controller, 0x21f5),
                                result != 0))))))
    {
      if (event == EVENT_MODIFY_COST_GLOBAL)
      {
        COST_COLORLESS = COST_COLORLESS + -1;
        goto LAB_02288a38;
      }
      if (event == EVENT_CAST_SPELL)
      {
        cannot_be_countered(affected_card_controller, affected_card, EVENT_CAST_SPELL);
        goto LAB_02288a38;
      }
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_02288a38;
    goto LAB_02288a5c;
  }
  if (event == EVENT_CAN_CAST)
  {
    result = flash(player, card, EVENT_CAN_CAST);
    return result;
  }
LAB_02288a38:
  result = hybrid(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2288d20; original cards/ikoria.c:309. */
int card_drannith_magistrate(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if ((event == EVENT_MODIFY_COST_GLOBAL) && (affected_card_controller != player))
  {
    instance = in_hand(affected_card_controller, affected_card);
    if (instance != NULL)
    {
      result = check_state(affected_card_controller, affected_card, 0x20);
      if (result == 0)
      {
        infinite_casting_cost();
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2289c30; original cards/ikoria.c:800. */
int card_gyruda_doom_of_depths(int player, int card, event_t event)
{
  int *piVar1;
  int result;
  unsigned int uVar3;
  int *piVar4;

  if (event == EVENT_QUERY)
  {
    if (attacking_card == 2)
    {

      piVar4 = deck_ptr[player];
      piVar1 = piVar4 + 500;
      do
      {
        if (*piVar4 == -1)
          break;
        uVar3 = get_cmc_by_internal_id(*piVar4);
        if ((uVar3 & 1) != 0)
        {
          return 0;
        }
        piVar4 = piVar4 + 1;
      } while (piVar4 != piVar1);
      result = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          uVar3 = get_cmc(player, result);
          if ((uVar3 & 1) != 0)
          {
            return 0;
          }
          result = result + 1;
        } while (result < active_cards_count[player]);
      }
      return 1;
    }
    check_legend_rule(player, card, EVENT_QUERY);
    etb_ability_2022(player, card, EVENT_QUERY, RESOLVE_TRIGGER_MANDATORY);
  }
  else
  {
    check_legend_rule(player, card, event);
    etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (event == EVENT_ETB)
    {
      special_mill(player, card, 0x5337, 2, 4);
    }
  }
  result = hybrid(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x228a4f0; original cards/ikoria.c:1043. */
int card_keruga_the_macrosage(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  test_definition_t test;

  if (event == EVENT_QUERY)
  {
    if (attacking_card == 2)
    {

      piVar4 = deck_ptr[player];
      piVar1 = piVar4 + 500;
      do
      {
        iVar2 = *piVar4;
        if (iVar2 == -1)
          break;
        if (((cards_data[iVar2].type & 1) == 0) &&
            (iVar2 = get_cmc_by_internal_id(iVar2), iVar2 < 3))
        {
          return 0;
        }
        piVar4 = piVar4 + 1;
      } while (piVar1 != piVar4);
      iVar2 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          iVar3 = is_what(player, iVar2, 1);
          if ((iVar3 == 0) && (iVar3 = get_cmc(player, iVar2), iVar3 < 3))
          {
            return 0;
          }
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }
      return 1;
    }
    check_legend_rule(player, card, EVENT_QUERY);
    etb_ability_2022(player, card, EVENT_QUERY, RESOLVE_TRIGGER_MANDATORY);
  }
  else
  {
    check_legend_rule(player, card, event);
    etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (event == EVENT_ETB)
    {
      default_test_definition(&test, 0x47);
      test.cmc = 2;
      test.cmc_flag = 2;
      iVar2 = check_battlefield_for_special_card(player, card, player, 4, &test);
      draw_cards(player, iVar2);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x228a9b0; original cards/ikoria.c:1171. */
int card_kogla_the_titan_ape(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  check_legend_rule(player, card, event);
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = 1 - player;
    result = can_target(&td);
    if ((result != 0) &&
        (result = new_pick_target(&td, "Select target creature you don\'t control.", 0, 0x80),
         result != 0))
    {

      instance = get_card_instance(player, card);
      result = instance->targets[0].card;
      instance->number_of_targets = '\0';
      fight(player, card, instance->targets[0].player, result);
    }
    dat_ability(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  }
  else
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    if (event == EVENT_DAT_ABILITY)
    {
      default_target_definition(player, card, &td, 0x44);
      td.allowed_controller = 1 - player;
      td.allow_cancel = 0;
      result = can_target(&td);
      if ((result != 0) && (result = pick_target(&td, "TARGET_ARTIFACT_ENCHANTMENT"), result != 0))
      {
        kill_target0(player, card, KILL_DESTROY);
      }
      instance = get_card_instance(player, card);
      instance->number_of_targets = '\0';
      return 0;
    }
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      goto LAB_0228aa4c;
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
LAB_0228aa4c:
  default_target_definition(player, card, &td, 2);
  td.required_subtype = 0x5b;
  td.allowed_controller = player;
  if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
  {

    instance = get_card_instance(player, card);
    bounce_permanent(instance->targets[0].player, instance->targets[0].card);
    pump_ability_until_eot_no_repeat_2022(player, card, instance->parent_controller,
                                     instance->parent_card, 0, 0x800);
  }
  result = generic_activated_ability(player, card, event, 6, 1, 0, 0, 1, 0, 0, 0, &td,
                                     "Select target Human you control.");
  return result;
}

/* ManalinkEh 2022: 0x228bc70; original cards/ikoria.c:1763. */
int card_obosh_the_preypiercer(int player, int card, event_t event)
{
  int *piVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int uVar4;
  int iVar5;
  int *piVar6;

  if ((event != EVENT_QUERY) || (attacking_card != 2))
  {
    check_legend_rule(player, card, event);
    instance = damage_being_dealt(event);
    if ((instance != NULL) &&
        ((instance->damage_source_player == player &&
          (uVar4 = get_cmc(player, instance->damage_source_card), (uVar4 & 1) != 0))))
    {
      instance->info_slot = instance->info_slot * 2;
    }
    iVar3 = hybrid(player, card, event);
    return iVar3;
  }

  piVar6 = deck_ptr[player];
  piVar1 = piVar6 + 500;
  do
  {
    iVar3 = *piVar6;
    if (iVar3 == -1)
      break;
    if ((cards_data[iVar3].type == '\0') &&
        (uVar4 = get_cmc_by_internal_id(iVar3), (uVar4 & 1) == 0))
    {
      return 0;
    }
    piVar6 = piVar6 + 1;
  } while (piVar1 != piVar6);
  iVar3 = 0;
  if (0 < active_cards_count[player])
  {
    do
    {
      iVar5 = is_what(player, iVar3, 1);
      if ((iVar5 == 0) && (uVar4 = get_cmc(player, iVar3), (uVar4 & 1) == 0))
      {
        return 0;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < active_cards_count[player]);
  }
  return 1;
}

/* ManalinkEh 2022: 0x2288060; original cards/ikoria.c:1927. */
static int iid_has_cycling(int iid, int me, int player, int card)
{
  return (unsigned int)(cards_data[iid].cc[2] == '\x04');
}

/* ManalinkEh 2022: 0x228bda0; original cards/ikoria.c:1933. */
int card_rooting_moloch(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  test_definition_t test;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  new_default_test_definition(&test, 0xff, "Select a card with Cycling.");
  test.special_selection_function = iid_has_cycling;
  if (event == EVENT_ETB)
  {
    iVar2 = new_special_count_grave(player, &test);
    if (iVar2 != 0)
    {

      iVar2 = new_select_a_card(player, player, 2, 1, 1, 0, &test);
      if (iVar2 != -1)
      {

        iVar1 = graveyard_ptr_mutable[player][iVar2];
        rfg_card_from_grave(player, iVar2);
        create_may_play_card_from_exile_effect(player, card, player,
                                               (unsigned int)cards_data[iVar1].id,
                                               MPCFE_UNTIL_END_OF_YOUR_NEXT_TURN);
      }
    }
  }
  iVar2 = cycling(player, card, event, 2, 0, 0, 0, 0, 0);
  return iVar2;
}

/* ManalinkEh 2022: 0x228cb30; original cards/ikoria.c:2542. */
int card_whirlwind_of_thought(int player, int card, event_t event)
{
  int result;

  prowess_mode(player, card, event, 2);
  if (event == EVENT_SPA)
  {
    draw_a_card(player);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x228cb90; original cards/ikoria.c:2582. */
int card_winota_joiner_of_forces(int player, int card, event_t event)
{
  int t_card;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (event != EVENT_DECLARE_ATTACKERS)
  {
    dat_ability(player, card, event, DAT_SEPARATE_TRIGGERS | DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {

      default_test_definition(&test, 2);
      test.subtype = 0x5b;
      test.subtype_flag = 0;
      t_card =
          reveal_top_cards_of_library_and_choose(player, card, player, 6, 0, TUTOR_PLAY_ATTACKING,
                                                 0, TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &test);
      if (-1 < t_card)
      {
        pump_ability_until_eot_no_repeat_2022(player, card, player, t_card, 0, 0x800);
      }
    }
    return 0;
  }
  default_test_definition(&test, 2);
  test.subtype = 0x5b;
  test.subtype_flag = 1;
  store_attackers(player, card, EVENT_DECLARE_ATTACKERS,
                  DAT_SEPARATE_TRIGGERS | DAT_RESOLVE_TRIGGER_MANDATORY, player, -1, &test);
  dat_ability(player, card, EVENT_DECLARE_ATTACKERS,
              DAT_SEPARATE_TRIGGERS | DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22884e0; original cards/ikoria.c:2380. */
static int vivien_library_ability(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  unsigned int uVar3;
  uint32_t uVar4;

  if (event == EVENT_CAN_ACTIVATE)
  {

    iVar1 = *deck_ptr[player];
    if (iVar1 != -1)
    {
      iVar2 = is_what(-1, iVar1, 0x40);
      if (((iVar2 != 0) &&
           (iVar2 = has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, iVar1), iVar2 != 0)) &&
          (iVar1 = can_legally_play_iid_now(player, iVar1, EVENT_CAN_ACTIVATE), iVar1 != 0))
      {
        return iVar1;
      }
      iVar1 = *deck_ptr[player];
    }
    return (unsigned int)(iVar1 != -1);
  }
  if (event != EVENT_ACTIVATE)
  {
    return 0;
  }

  iVar1 = *deck_ptr[player];
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    uVar4 = 0;
    uVar3 = 0;
    if (iVar1 == -1)
      goto LAB_0228856c;
  }
  else
  {
    uVar4 = 1;
    if (iVar1 == -1)
    {
      uVar4 = 0;
      uVar3 = 0;
      goto LAB_0228856c;
    }
  }
  iVar1 = is_what(-1, iVar1, 2);
  uVar3 = 0;
  if (iVar1 != 0)
  {
    iVar1 = can_legally_play_iid_now(player, *deck_ptr[player], EVENT_ACTIVATE);
    uVar3 = 0;
    if (iVar1 != 0)
    {
      iVar1 = has_mana_to_cast_iid(player, EVENT_ACTIVATE, *deck_ptr[player]);
      uVar3 = (unsigned int)(iVar1 != 0);
    }
  }
  iVar1 = *deck_ptr[player];
LAB_0228856c:
  iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\014", "\001\021",
                      iVar1, "Check top card of deck", uVar4, 0, "Play top card", uVar3, 10, 0, 0,
                      0, 0);
  if (iVar1 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (iVar1 != 1)
  {
    if ((iVar1 == 2) &&
        (iVar1 = charge_mana_from_id(player, -1, EVENT_ACTIVATE,
                                     (unsigned int)cards_data[*deck_ptr[player]].id),
         iVar1 != 0))
    {
      play_card_in_deck_for_free(player, player, 0);
      cant_be_responded_to = 1;
      spell_fizzled = 1;
    }
    return 0;
  }
  show_deck(player, deck_ptr[player], 1, "Here\'s the top card of your deck", 0, ENGINE_OK_LABEL);
  spell_fizzled = 1;
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x228a680; original cards/ikoria.c:1090. */
int card_kinnan_bonder_prodigy(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  color_test_t col;
  test_definition_t test;

  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      iVar1 = is_humiliated_for_ability(player, card, AL_TRIG);
      if (iVar1 != 0)
      {
        return 0;
      }
      if (event == EVENT_COUNT_MANA)
      {
        iVar1 = is_what(affected_card_controller, affected_card, 1);
        if (iVar1 != 0)
        {
          return 0;
        }
        instance = in_play(player, card);
        if (instance == NULL)
        {
          return 0;
        }
        iVar1 = is_tapped(affected_card_controller, affected_card);
        if (iVar1 != 0)
        {
          return 0;
        }
        iVar1 = is_animated_and_sick(affected_card_controller, affected_card);
        if (iVar1 != 0)
        {
          return 0;
        }
        iVar1 = can_produce_mana_2022(affected_card_controller, affected_card);
        if (iVar1 == 0)
        {
          return 0;
        }
        instance = get_card_instance(affected_card_controller, affected_card);
        iVar3 = 0;
        col = (color_test_t)instance->card_color;
        iVar1 = 0;
        do
        {
          iVar1 =
              (iVar1 + 1) - (unsigned int)(((int)col >> ((unsigned char)iVar3 & 0x1f) & 1U) == 0);
          iVar3 = iVar3 + 1;
        } while (iVar3 != 7);
        if (0 < iVar1)
        {
          declare_mana_available_hex(affected_card_controller, col, 1);
          return 0;
        }
        iVar1 = single_color_test_bit_to_color(col);
        declare_mana_available(affected_card_controller, iVar1, 1);
        return 0;
      }
      if (EVENT_COUNT_MANA < event)
      {
        if (event != EVENT_TAP_CARD)
        {
          return 0;
        }

        iVar1 = is_what(affected_card_controller, affected_card, 1);
        if (iVar1 != 0)
        {
          return 0;
        }
        instance = in_play(player, card);
        if (instance == NULL)
        {
          return 0;
        }
        produce_mana_of_any_type_tapped_for(player, card, 1);
        return 0;
      }
      if (event != EVENT_CAST_SPELL)
      {
        return (unsigned int)(event == EVENT_CAN_CAST);
      }
      if (card == affected_card)
      {
        if (player == affected_card_controller)
        {
          ai_modifier = ai_modifier + 0x30;
          return 0;
        }
        return 0;
      }
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      new_default_test_definition(&test, 2, "Select a non-Human creature card.");
      test.subtype = 0x5b;
      test.subtype_flag = 1;
      reveal_top_cards_of_library_and_choose(player, card, player, 5, 0, TUTOR_PLAY, 0,
                                             TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &test);
    }
  }
  iVar1 = generic_activated_ability(player, card, event, 0, 5, 0, 1, 1, 0, 0, 0, NULL, NULL);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x228bfc0; original cards/ikoria.c:2029. */
int card_shark_typhoon(int player, int card, event_t event)
{
  int32_t iVar1;
  int iVar2;
  card_instance_t *instance;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_token;

  if (event == EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    iVar2 = cycling(player, card, EVENT_CAN_ACTIVATE_FROM_HAND, 1, 0, 1, 0, 0, 0);
    return iVar2;
  }
  if (event == EVENT_ACTIVATE_FROM_HAND)
  {

    iVar2 = cycling(player, card, EVENT_ACTIVATE_FROM_HAND, 1, 0, 1, 0, 0, 0);
    charge_mana(player, COLOR_COLORLESS, -1);
    if (spell_fizzled == 1)
    {
      return iVar2;
    }
    instance = get_card_instance(player, card);
    instance->info_slot = x_value;
    return iVar2;
  }
  if (event == EVENT_RESOLVE_ACTIVATION_FROM_HAND)
  {
    if ((spell_fizzled == 0) && ((((unsigned int)player_bits[player] >> 24 & 0xff) & 2) == 0))
    {

      instance = get_card_instance(player, card);
      iVar1 = instance->info_slot;
      default_token_definition(player, card, 0x5394, &recovered_stack_token.token);
      recovered_stack_token.token.key_plus = 0x20;
      recovered_stack_token.token.pow = iVar1;
      recovered_stack_token.token.tou = iVar1;
      generate_token(&recovered_stack_token.token);
      iVar2 = cycling(player, card, EVENT_RESOLVE_ACTIVATION_FROM_HAND, 1, 0, 1, 0, 0, 0);
      return iVar2;
    }
    if ((trigger_condition != 0xd3) || (card != affected_card))
      goto LAB_0228c00d;
  LAB_0228c068:
    if (player == affected_card_controller)
    {
      default_test_definition(&recovered_stack_token.test, 2);
      sp_ability_trigger(player, event, 2, TYPE_NONE, &recovered_stack_token.test, player, card);
    }
  }
  else if ((trigger_condition == 0xd3) && (card == affected_card))
    goto LAB_0228c068;
  if (event == EVENT_SPA)
  {

    iVar2 = get_cmc(trigger_cause_controller, trigger_cause);
    default_token_definition(player, card, 0x5394, &recovered_stack_token.token);
    recovered_stack_token.token.key_plus = 0x20;
    recovered_stack_token.token.pow = iVar2;
    recovered_stack_token.token.tou = iVar2;
    generate_token(&recovered_stack_token.token);
  }
LAB_0228c00d:
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x228c2c0; original cards/ikoria.c:2169. */
int card_sprite_dragon(int player, int card, event_t event)
{
  int result;

  haste(player, card, event);
  result = card_deeproot_champion(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x22883c0; original cards/ikoria.c:2425. */
static int vivien_doubling_legacy(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  sp_ability_trigger(player, event, 2, TYPE_CREATURE, NULL, player, card);
  if (event == EVENT_SPA)
  {

    result = get_cmc(trigger_cause_controller, trigger_cause);
    default_test_definition(&test, 2);
    test.cmc_flag = 3;
    test.cmc = result;
    sprintf(test.message, "Select a creature card with mana value %d or less.", result + -1);
    new_global_tutor(player, player, 1, 4, 1, 2, &test);
  }
  else if (event != EVENT_CLEANUP)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x228cf40; original cards/ikoria.c:2700. */
int card_zirda_the_dawnwaker(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  int *piVar5;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  if (event == EVENT_QUERY)
  {
    if (attacking_card == 2)
    {

      piVar5 = deck_ptr[player];
      piVar1 = piVar5 + 500;
      do
      {
        iVar2 = *piVar5;
        if (iVar2 == -1)
          break;
        if (((cards_data[iVar2].type & 0x47) != 0) &&
            (iVar2 = has_an_activated_ability(iVar2), iVar2 == 0))
        {
          return 0;
        }
        piVar5 = piVar5 + 1;
      } while (piVar5 != piVar1);
      iVar2 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {

          iVar4 = get_original_internal_card_id(player, iVar2);
          if (((cards_data[iVar4].type & 0x47) != 0) &&
              (iVar4 = has_an_activated_ability(iVar4), iVar4 == 0))
          {
            return 0;
          }
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }
      return 1;
    }
    check_legend_rule(player, card, EVENT_QUERY);
  LAB_0228cf82:
    leaves_play_2022(player, card, event, 0, 0);
    if (event == EVENT_LEAVES_PLAY_ABILITY)
    {
      default_test_definition(&recovered_stack_td.test, 0x47);
      remove_cost_mod_for_activated_abilities(player, -1, 2, &recovered_stack_td.test);
      goto LAB_0228d158;
    }
  LAB_0228cfae:
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      goto LAB_0228cfc6;
  }
  else
  {
    check_legend_rule(player, card, event);
    if (event == EVENT_RESOLVE_SPELL)
    {
      default_test_definition(&recovered_stack_td.test, 0x47);
      set_cost_mod_for_activated_abilities(player, -1, 2, &recovered_stack_td.test);
      leaves_play_2022(player, card, EVENT_RESOLVE_SPELL, 0, 0);
      goto LAB_0228cfae;
    }
    if (event != EVENT_CAST_SPELL)
      goto LAB_0228cf82;
    iVar2 = is_what(affected_card_controller, affected_card, 0x47);
    if (iVar2 != 0)
    {
      set_cost_mod_for_activated_abilities(affected_card_controller, affected_card, 2, NULL);
    }
    leaves_play_2022(player, card, EVENT_CAST_SPELL, 0, 0);
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
  LAB_0228d158:
    iVar2 = hybrid(player, card, event);
    return iVar2;
  }
LAB_0228cfc6:
  default_target_definition(player, card, &recovered_stack_td.target, 2);
  if ((event == EVENT_RESOLVE_ACTIVATION) &&
      (iVar2 = valid_target(&recovered_stack_td.target), iVar2 != 0))
  {

    instance = get_card_instance(player, card);
    iVar2 = pump_ability_until_eot_no_repeat(player, card, instance->targets[0].player,
                                             instance->targets[0].card, 0, 0x20);
    alternate_legacy_text(2, player, iVar2);
  }
  iVar2 = generic_activated_ability(player, card, event, 5, 1, 0, 0, 0, 0, 0, 0,
                                    &recovered_stack_td.target, "TARGET_CREATURE");
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x228a120; original cards/ikoria.c:966. */
int card_kaheera_the_orphanguard(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  int *piVar5;

  if (event == EVENT_QUERY)
  {
    if (attacking_card == 2)
    {

      piVar5 = deck_ptr[player];
      piVar1 = piVar5 + 500;
      do
      {
        iVar2 = *piVar5;
        if (iVar2 == -1)
          break;
        if ((((((cards_data[iVar2].type & 2) != 0) &&
               (iVar2 =
                    has_subtype_by_id_2022(player, (unsigned int)cards_data[iVar2].id, SUBTYPE_CAT),
                iVar2 == 0)) &&
              (iVar2 = has_subtype_by_id_2022(player, (unsigned int)cards_data[*piVar5].id,
                                              SUBTYPE_ELEMENTAL),
               iVar2 == 0)) &&
             ((iVar2 = has_subtype_by_id_2022(player, (unsigned int)cards_data[*piVar5].id,
                                              SUBTYPE_NIGHTMARE),
               iVar2 == 0 && (iVar2 = has_subtype_by_id_2022(
                                  player, (unsigned int)cards_data[*piVar5].id, SUBTYPE_DINOSAUR),
                              iVar2 == 0)))) &&
            (iVar2 = has_subtype_by_id_2022(player, (unsigned int)cards_data[*piVar5].id,
                                            SUBTYPE_BEAST),
             iVar2 == 0))
        {
          return 0;
        }
        piVar5 = piVar5 + 1;
      } while (piVar5 != piVar1);
      iVar2 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          iVar4 = is_what(player, iVar2, 2);
          if (((iVar4 != 0) && (iVar4 = has_subtype(player, iVar2, SUBTYPE_CAT), iVar4 == 0)) &&
              ((iVar4 = has_subtype(player, iVar2, SUBTYPE_ELEMENTAL),
                iVar4 == 0 &&
                    (((iVar4 = has_subtype(player, iVar2, SUBTYPE_NIGHTMARE),
                       iVar4 == 0 &&
                           (iVar4 = has_subtype(player, iVar2, SUBTYPE_DINOSAUR), iVar4 == 0)) &&
                      (iVar4 = has_subtype(player, iVar2, SUBTYPE_BEAST), iVar4 == 0))))))
          {
            return 0;
          }
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }
      return 1;
    }
    check_legend_rule(player, card, EVENT_QUERY);
    vigilance_2022(player, card, EVENT_QUERY);
  }
  else
  {
    check_legend_rule(player, card, event);
    vigilance_2022(player, card, event);
    if ((((event - EVENT_POWER < 3) && (instance = in_play(player, card), instance != NULL)) &&
         (iVar2 = is_humiliated_for_ability(player, card, AL_STATIC), iVar2 == 0)) &&
        ((((affected_card_controller == player && (affected_card != card)) &&
           (iVar2 = is_what(player, affected_card, 2), iVar2 != 0)) &&
          (((iVar2 = has_subtype(affected_card_controller, affected_card, SUBTYPE_CAT),
             iVar2 != 0 ||
                 (iVar2 = has_subtype(affected_card_controller, affected_card, SUBTYPE_ELEMENTAL),
                  iVar2 != 0)) ||
            ((iVar2 = has_subtype(affected_card_controller, affected_card, SUBTYPE_NIGHTMARE),
              iVar2 != 0 ||
                  ((iVar2 = has_subtype(affected_card_controller, affected_card, SUBTYPE_DINOSAUR),
                    iVar2 != 0 || (iVar2 = has_subtype(affected_card_controller, affected_card,
                                                       SUBTYPE_BEAST),
                                   iVar2 != 0))))))))))
    {
      if (event < EVENT_ABILITIES)
      {
        event_result = event_result + 1;
      }
      else
      {
        vigilance_2022(affected_card_controller, affected_card, EVENT_ABILITIES);
      }
    }
  }
  iVar2 = hybrid(player, card, event);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int luminous_broodmoth_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x228ad20; original cards/ikoria.c:1262. */
int card_lukka_coppercoat_outcast(int player, int card, event_t event)
{
  enum
  {
    MPCFE_LUKKA = 512
  };

  card_instance_t *instance;
  int result;
  target_definition_t *ptStack_140;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Exile top 3", 1,
                           10, 1, "Exile a creature", result, 10, 0xfffffffe, "Creature smash", 1,
                           0x14, 0xfffffff9, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 2)
      {
        new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      }
      goto LAB_0228ad9e;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_0228ad9e;
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
  }
  ptStack_140 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    result = instance->info_slot;
    if (result == 1)
    {
      default_test_definition(&test, 2);
      exile_top_x_card_you_may_play_them(instance->parent_controller, instance->parent_card, 3,
                                         &test, player, MPCFE_LUKKA);
    }
    else if (result == 2)
    {
      result = valid_target(ptStack_140);
      if (result != 0)
      {

        result = get_cmc(instance->targets[0].player, instance->targets[0].card);
        kill_target0(player, card, KILL_EXILE);
        default_test_definition(&test, 2);
        test.cmc_flag = 2;
        test.cmc = result;
        reveal_card_until_you_reveal_test(player, player, &test, 0x44);
      }
    }
    else
    {

      if ((result == 3) && (ptStack_140 = (target_definition_t *)(active_cards_count[player] + -1),
                            -1 < (int)ptStack_140))
      {
        do
        {
          instance = in_play(player, (int)ptStack_140);
          if ((instance != NULL) && (result = is_what(player, (int)ptStack_140, 2), result != 0))
          {
            result = get_power(player, (int)ptStack_140);
            damage_player(1 - player, result, player, (int)ptStack_140);
          }
          ptStack_140 = (target_definition_t *)((int)ptStack_140 + -1);
        } while (ptStack_140 != (target_definition_t *)0xffffffff);
      }
    }
  }
LAB_0228ad9e:

  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x2288080; original cards/ikoria.c:1339. */
static int luminous_broodmoth_effect(int player, int card, event_t event)
{
  enum
  {
    COUNTER_FLYING = 147
  };

  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int t_card;
  target_t *ptVar5;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (iVar2 = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           iVar2 != 0x5357))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (iVar2 = effect_follows_control_of_attachment(player, card, 0x4700), iVar2 != 0))
    {
      return 0;
    }
  LAB_022880b7:
    iVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
      goto LAB_022880dc;
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (iVar2 = effect_follows_control_of_attachment(player, card, event), iVar2 != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_022880b7;

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
      }
      default_test_definition(&test, 2);
      test.keyword = 0x20;
      test.keyword_flag = 1;
      count_for_gfp_ability_and_store_values(player, card, 0x77, player, 0, &test, 0x90, 0);
    }
    iVar2 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
    {
      return 0;
    }
  }
  iVar4 = check_status(player, card, 0x10000);
  if (iVar4 == 0)
  {

    pcVar1 = get_card_instance(player, card);
    if (0 < iVar2)
    {
      ptVar5 = pcVar1->targets + 1;
      do
      {

        iVar4 = ptVar5->player;
        t_card = seek_grave_for_id_to_reanimate(
            iVar4, -1, iVar4, (unsigned int)cards_data[ptVar5->card].id, REANIMATE_DEFAULT);
        if (t_card != -1)
        {
          add_counters_from_source(player, card, iVar4, t_card, COUNTER_FLYING, 1);
        }
        ptVar5 = ptVar5 + 1;
      } while (ptVar5 != pcVar1->targets + iVar2 + 1);
    }
    kill_card(player, card, KILL_EXILE);
  }
LAB_022880dc:
  if ((event == EVENT_CLEANUP) && (iVar2 = check_status(player, card, 0x10000), iVar2 == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x228b0d0; original cards/ikoria.c:1398. */
int card_luminous_broodmoth(int player, int card, event_t event)
{
  enum
  {
    COUNTER_FLYING = 147
  };

  int player_00;
  card_instance_t *instance;
  int iVar2;
  int t_card;
  int iVar3;
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
                instance->info_slot_fn == luminous_broodmoth_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_0228b0f5;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, luminous_broodmoth_effect, player, card);
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
    test.keyword = 0x20;
    test.keyword_flag = 1;
    count_for_gfp_ability_and_store_values(player, card, 0x77, player, 0, &test, 0x90, 0);
  }
LAB_0228b0f5:
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (iVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY), 0 < iVar2))
  {
    iVar3 = 0;
    do
    {

      instance = get_card_instance(player, card);
      player_00 = instance->targets[iVar3].player;
      t_card = seek_grave_for_id_to_reanimate(
          player_00, -1, player_00, (unsigned int)cards_data[instance->targets[iVar3].card].id,
          REANIMATE_DEFAULT);
      if (t_card != -1)
      {
        add_counters_from_source(player, card, player_00, t_card, COUNTER_FLYING, 1);
      }
      iVar3 = iVar3 + 1;
    } while (iVar2 != iVar3);
    return 0;
  }
  return 0;
}


/* ManalinkEh 2022: 0x22887d0; original cards/ikoria.c:2619. */
static const char *permanent_you_control_and_own(int who_chooses, int player, int card,
                                                 int targeting_player, int targeting_card)
{
  int result;
  result = get_owner(player, card);
  if (result != targeting_player)
  {
    return "owner";
  }
  return NULL;
}


/* ManalinkEh 2022: 0x228cd10; original cards/ikoria.c:2625. */
int card_yorion_sky_nomad(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  target_definition_t td;
  char flicker_array[2][151];

  if (event == EVENT_QUERY)
  {
    if (attacking_card == 2)
    {
      if (singleplayer_mode == 2)
      {
        iVar1 = 0x3b;
      }
      else
      {
        iVar1 = ((other_game_mode == OGM_EDH) - 1 & 0xffffffda) + 0x75;
      }
      iVar2 = count_deck(player);
      return (unsigned int)(iVar1 < iVar2);
    }
    check_legend_rule(player, card, EVENT_QUERY);
    etb_ability(player, card, EVENT_QUERY, RESOLVE_TRIGGER_MANDATORY);
  }
  else
  {
    check_legend_rule(player, card, event);
    etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (event == EVENT_ETB)
    {

      default_target_definition(player, card, &td, 0x47);
      td.allowed_controller = player;
      td.preferred_controller = player;
      td.special = 0xc000;
      td.extra = (int32_t)permanent_you_control_and_own;
      td.illegal_abilities = 0;
      td.allow_cancel = 3;
      memset(flicker_array, 0, sizeof(flicker_array));
      iVar1 = mark_up_to_n_targets_noload(&td, "Select a nonland permanent you own and control.",
                                          -1, flicker_array);
      if ((0 < iVar1) && (0 < active_cards_count[player]))
      {
        iVar1 = 0;
        do
        {
          if (flicker_array[player][iVar1] != '\0')
          {
            iVar2 = remove_until_eot(player, card, player, iVar1);
            alternate_legacy_text(2, player, iVar2);
          }
          iVar1 = iVar1 + 1;
        } while (iVar1 < active_cards_count[player]);
      }
    }
  }
  iVar1 = hybrid(player, card, event);
  return iVar1;
}
