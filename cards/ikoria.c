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
  result = generic_aura(player, card, event, player, 0x14, 0x14, 0, 0, 0, 0, 0);
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
  vigilance(player, card, event);
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
