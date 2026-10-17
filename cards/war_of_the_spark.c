#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21c8920; original cards/war_of_the_spark.c:239. */
int card_arboreal_grazer(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 1, "Select a land card.");
    test.zone = 0x100;
    result = check_battlefield_for_special_card(player, card, player, 0, &test);
    if (result != 0)
    {
      new_global_tutor(player, player, 4, 5, 0, 1, &test);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c8c50; original cards/war_of_the_spark.c:323. */
int card_ashioks_skulker(int player, int card, event_t event)
{
  int result;
  result = generic_shade(player, card, event, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, 0x10);
  return result;
}

/* ManalinkEh 2022: 0x21cae80; original cards/war_of_the_spark.c:1229. */
int card_deathsprout(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td1;

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
        default_target_definition(player, card, &td1, 2);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td1);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            tutor_basic_land(player, 1, 1);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021caee6;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td1, 2);
  get_card_instance(player, card);
LAB_021caee6:
  result = generic_spell(player, card, event, 1, &td1, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21cafe0; original cards/war_of_the_spark.c:1264. */
int card_despark(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x47);
        td.special = 0x1000000;
        td.extra = 4;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021cb058;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.special = 0x1000000;
  td.extra = 4;
  get_card_instance(player, card);
LAB_021cb058:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target permanent with converted mana cost 4 or greater.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c6700; original cards/war_of_the_spark.c:1373. */
int insist_until_eot(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_CAST_SPELL)
  {
    if (card != affected_card)
    {
      if ((player == affected_card_controller) &&
          (result = is_what(player, affected_card, 2), result != 0))
      {
        state_untargettable(affected_card_controller, affected_card, 1);
      }
      goto LAB_021c6717;
    }
    if (player != affected_card_controller)
    {
      if (trigger_condition != 0xdb)
        goto LAB_021c6727;
      goto LAB_021c67c9;
    }
    if (trigger_condition != 0xdb)
      goto LAB_021c6727;
  }
  else
  {
  LAB_021c6717:
    if ((trigger_condition != 0xdb) || (card != affected_card))
      goto LAB_021c6727;
  LAB_021c67c9:
    if (player != affected_card_controller)
      goto LAB_021c6727;
  }
  if (((reason_for_trigger_controller == affected_card_controller) &&
       (result = is_what(trigger_cause_controller, trigger_cause, 2), result != 0)) &&
      (result = check_state(trigger_cause_controller, trigger_cause, 0x100000), result != 0))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result = event_result | 2;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      state_untargettable(trigger_cause_controller, trigger_cause, 0);
    }
  }
LAB_021c6727:
  result = eot_trigger(player, card, event);
  if (result != 0)
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21cc0f0; original cards/war_of_the_spark.c:1682. */
int card_elite_guardmage(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  gain_life(player, 3);
  draw_a_card(player);
  return 0;
}

/* ManalinkEh 2022: 0x21cc800; original cards/war_of_the_spark.c:1851. */
int card_erratic_visionary(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_cards(player, 1);
    discard(player, 0, player);
  }
  result = generic_activated_ability(player, card, event, 1, 1, 0, 1, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21cc8b0; original cards/war_of_the_spark.c:1882. */
int card_evolution_sage(int player, int card, event_t event)
{
  landfall(player, card, event);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  proliferate(player, card);
  return 0;
}

/* ManalinkEh 2022: 0x21cef20; original cards/war_of_the_spark.c:2692. */
int card_heartwarming_redemption(int player, int card, event_t event)
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
      result = hand_count[player] + 1;
      discard_all(player);
      draw_cards(player, result);
      gain_life(player, hand_count[player]);
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

/* ManalinkEh 2022: 0x21ceff0; original cards/war_of_the_spark.c:2726. */
int card_huatlis_raptor(int player, int card, event_t event)
{
  vigilance(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  proliferate(player, card);
  return 0;
}

/* ManalinkEh 2022: 0x21cf210; original cards/war_of_the_spark.c:2784. */
int card_ignite_the_beacon(int player, int card, event_t event)
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
    else if (event == EVENT_RESOLVE_SPELL)
    {
      new_default_test_definition(&this_test, 0x100, "Select a planeswalker card.");
      this_test.qty = 2;
      new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
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

/* ManalinkEh 2022: 0x21c7380; original cards/war_of_the_spark.c:3345. */
static int kayas_ghostform_effect(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  seek_grave_for_id_to_reanimate(player, card, instance->targets[0].player,
                                 (unsigned int)cards_data[instance->targets[0].card].id,
                                 REANIMATE_DEFAULT);
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x21d0ab0; original cards/war_of_the_spark.c:3545. */
int card_law_rune_enforcer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.special = 0x1000000;
    td.extra = 2;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.special = 0x1000000;
    td.extra = 2;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      result = instance->targets[0].card;
      instance = get_card_instance(player, card);
      tap_card(instance->targets[0].player, result);
    }
  }
  result = generic_activated_ability(player, card, event, 5, 1, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x21c68a0; original cards/war_of_the_spark.c:3572. */
static int lazotep_plating_effect(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_STATIC_EFFECTS)
  {
    *(unsigned int *)(&player_bits + player * 4) =
        *(unsigned int *)(&player_bits + player * 4) | 0x20000;
  }
  result = eot_trigger(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x21d0db0; original cards/war_of_the_spark.c:3620. */
int card_leyline_prowler(int player, int card, event_t event)
{
  int result;
  deathtouch(player, card, event);
  lifelink(player, card, event);
  result = mana_producing_creature_all_one_color(player, card, event, 0, COLOR_TEST_ANY_COLORED, 1);
  return result;
}

/* ManalinkEh 2022: 0x21d1560; original cards/war_of_the_spark.c:3874. */
int card_mana_geode(int player, int card, event_t event)
{
  int result;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    scry(player, 1);
  }
  result = artifact_mana_all_one_color(player, card, event, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x21d37d0; original cards/war_of_the_spark.c:4667. */
int card_nissa_who_shakes_the_world_emblem(int player, int card, event_t event)
{
  int result;
  if ((event == EVENT_ABILITIES) && (affected_card_controller == player))
  {
    result = is_what(affected_card_controller, affected_card, 1);
    if (result != 0)
    {
      indestructible(affected_card_controller, affected_card, EVENT_ABILITIES);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21d49e0; original cards/war_of_the_spark.c:5156. */
int card_prison_realm(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (event == EVENT_CAN_CAST)
  {
    result = basic_spell(player, card, EVENT_CAN_CAST);
    return result;
  }
  return_from_oblivion(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 0x102);
  td.allowed_controller = 1 - player;
  td.allow_cancel = 0;
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "TARGET_CREATURE_OR_PLANESWALKER", 0, 0);
    if (result != 0)
    {
      instance = get_card_instance(player, card);
      obliviation(player, card, instance->targets[0].player, instance->targets[0].card);
      goto LAB_021d4aa2;
    }
  }
  if (player == 1)
  {
    ai_modifier = ai_modifier + -0x30;
  }
LAB_021d4aa2:
  scry(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x21c7c40; original cards/war_of_the_spark.c:6064. */
static int teferi_flash_effect(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_CAN_SKIP_TURN)
  {
    instance = get_card_instance(player, card);
    if (instance->targets[9].card < 1)
    {
      kill_card(player, card, KILL_EXILE);
    }
    else
    {
      instance = get_card_instance(player, card);
      instance->targets[9].card = instance->targets[9].card + -1;
    }
  }
  else if ((event - EVENT_RESOLVE_ACTIVATION < 2) || (event == EVENT_ACTIVATE))
  {
    base_target_definition(player, card, &td, 8);
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.zone = 0x100;
    result = can_play_cards_as_though_they_had_flash(player, card, event, &td,
                                                     "Select a sorcery card", 1);
    return result;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21d7e70; original cards/war_of_the_spark.c:6489. */
int card_time_wipe(int player, int card, event_t event)
{
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
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      td.preferred_controller = player;
      td.illegal_abilities = 0;
      td.allow_cancel = 0;
      if ((card == affected_card) && (player == affected_card_controller))
      {
        result = can_target(&td);
        if (result != 0)
        {
          pick_target(&td, "SELECT_A_CREATURE_YOU_CONTROL");
        }
        result = basic_spell(player, card, EVENT_CAST_SPELL);
        return result;
      }
      goto LAB_021d7ef2;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      td.preferred_controller = player;
      td.illegal_abilities = 0;
      td.allow_cancel = 0;
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = valid_target(&td);
        if (result != 0)
        {
          action_on_target(player, card, 0, ACT_BOUNCE);
        }
        default_test_definition(&this_test, 2);
        new_manipulate_all(player, card, 2, &this_test, ACT_KILL_DESTROY);
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_021d7ef2;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  td.allow_cancel = 0;
LAB_021d7ef2:
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x21d8ac0; original cards/war_of_the_spark.c:6796. */
int card_unlikely_aid(int player, int card, event_t event)
{
  int result;
  result = vanilla_instant_pump(player, card, event, 2, player, 2, 0, 0, 0x800);
  return result;
}

/* ManalinkEh 2022: 0x21c7da0; original cards/war_of_the_spark.c:6856. */
static int fx_vigilance_reach_until_your_next_turn(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (event == EVENT_ABILITIES)
  {
    instance = get_card_instance(player, card);
    if ((instance->damage_target_card == affected_card) &&
        (instance->damage_target_player == affected_card_controller))
    {
      vigilance((int)instance->damage_target_player, instance->damage_target_card, EVENT_ABILITIES);
      event_result = event_result | 0x400;
    }
  }
  else if ((event == EVENT_BEGIN_TURN) && (player == current_turn))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c7a90; original cards/war_of_the_spark.c:7035. */
static int has_deathtouch(int unused, int unused2, int player, int card)
{
  int result;
  result = check_for_special_ability(player, card, 0x200);
  return (unsigned int)(result != 0);
}

/* ManalinkEh 2022: 0x21d93c0; original cards/war_of_the_spark.c:7101. */
int card_wall_of_runes(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  scry(player, 1);
  return 0;
}
