#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21c8920; original cards/war_of_the_spark.c:239. */
int card_arboreal_grazer(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  result = duh_mode(player);
  etb_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
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
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
      gain_life_2022(player, hand_count[player]);
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
  vigilance_2022(player, card, event);
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
  result = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  deathtouch_2022(player, card, event);
  lifelink(player, card, event);
  result = mana_producing_creature_all_one_color(player, card, event, 0, COLOR_TEST_ANY_COLORED, 1);
  return result;
}

/* ManalinkEh 2022: 0x21d1560; original cards/war_of_the_spark.c:3874. */
int card_mana_geode(int player, int card, event_t event)
{
  int result;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
      indestructible_2022(affected_card_controller, affected_card, EVENT_ABILITIES);
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
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  result = vanilla_instant_pump_2022(player, card, event, 2, player, 2, 0, 0, 0x800);
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
      vigilance_2022((int)instance->damage_target_player, instance->damage_target_card, EVENT_ABILITIES);
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
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  scry(player, 1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21cc9e0; original cards/war_of_the_spark.c:1923. */
int card_finale_of_devastation(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  char buffer[100];
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
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          iVar2 = instance->info_slot;
          scnprintf(buffer, 100, "Select a creature card with CMC %d or less", iVar2);
          new_default_test_definition(&test, 2, buffer);
          test.cmc = iVar2 + 1;
          test.cmc_flag = 3;
          iVar3 = new_special_count_grave(player, &test);
          if ((iVar3 == 0) ||
              (iVar3 = new_global_tutor(player, player, 2, 4, 0, 2, &test), iVar3 < 0))
          {
            new_global_tutor(player, player, 1, 4, 0, 2, &test);
          }
          if (9 < iVar2)
          {
            pump_subtype_until_eot_2022(player, card, player, -1, iVar2, iVar2, 0, 4);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021cca21;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  get_card_instance(player, card);
LAB_021cca21:
  iVar2 = generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x21ccbf0; original cards/war_of_the_spark.c:1960. */
int card_finale_of_eternity(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  card_instance_t *instance;
  int iVar4;
  unsigned int uVar5;
  int iVar6;
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
        instance = get_card_instance(player, card);
        default_target_definition(player, card, &td, 2);
        td.toughness_requirement = x_value | 0x2000;
        td.allow_cancel = 2;
        if (event == EVENT_RESOLVE_SPELL)
        {
          uVar5 = instance->info_slot;
          td.toughness_requirement = uVar5 | 0x2000;
          if (instance->number_of_targets != '\0')
          {
            iVar6 = 0;
            do
            {
              while (iVar4 = validate_target(player, card, &td, iVar6), iVar4 == 0)
              {
                iVar6 = iVar6 + 1;
                if ((int)(unsigned int)instance->number_of_targets <= iVar6)
                  goto LAB_021ccce6;
              }
              ptVar2 = instance->targets + iVar6;
              ptVar1 = instance->targets + iVar6;
              iVar6 = iVar6 + 1;
              kill_card(ptVar1->player, ptVar2->card, KILL_DESTROY);
            } while (iVar6 < (int)(unsigned int)instance->number_of_targets);
          LAB_021ccce6:
            uVar5 = instance->info_slot;
          }
          if (9 < (int)uVar5)
          {
            default_test_definition(&test, 2);
            new_reanimate_all(player, card, player, &test, REANIMATE_DEFAULT);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021ccd6b;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  get_card_instance(player, card);
  default_target_definition(player, card, &td, 2);
  td.toughness_requirement = x_value | 0x2000;
  td.allow_cancel = 2;
LAB_021ccd6b:
  iVar6 = generic_spell(player, card, event, 0x24, &td, "TARGET_CREATURE", 3, NULL);
  return iVar6;
}

/* ManalinkEh 2022: 0x21cce20; original cards/war_of_the_spark.c:1997. */
int card_finale_of_glory(int player, int card, event_t event)
{
  card_instance_t *instance;
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
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          default_token_definition(player, card, 0x4986, &token);
          token.qty = (unsigned char)instance->info_slot;
          token.s_key_plus = 8;
          generate_token(&token);
          if (9 < instance->info_slot)
          {
            default_token_definition(player, card, 0x48ff, &token);
            token.s_key_plus = 8;
            token.qty = (unsigned char)instance->info_slot;
            generate_token(&token);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021cce61;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  get_card_instance(player, card);
LAB_021cce61:
  result = generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21ccf80; original cards/war_of_the_spark.c:2028. */
int card_finale_of_promise(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  char buffer_i[100];
  char buffer_s[100];
  test_definition_t test_i;
  test_definition_t test_s;

  if (event == EVENT_CAN_CAST)
  {
    get_card_instance(player, card);
    scnprintf(buffer_i, 100, "Select an instant card with cmc %d or less", x_value);
    new_default_test_definition(&test_i, 0x30, buffer_i);
    scnprintf(buffer_s, 100, "Select a sorcery card with cmc %d or less", x_value);
    new_default_test_definition(&test_s, 8, buffer_s);
    iVar5 = generic_spell(player, card, EVENT_CAN_CAST, 0xc, NULL, NULL, 1, &test_i);
    iVar2 = 1;
    if (iVar5 == 0)
    {
      iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 0xc, NULL, NULL, 1, &test_s);
    }
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance = get_card_instance(player, card);
        scnprintf(buffer_i, 100, "Select an instant card with cmc %d or less", x_value);
        new_default_test_definition(&test_i, 0x30, buffer_i);
        scnprintf(buffer_s, 100, "Select a sorcery card with cmc %d or less", x_value);
        new_default_test_definition(&test_s, 8, buffer_s);
        if ((card == affected_card) && (player == affected_card_controller))
        {
          generic_spell(player, card, EVENT_CAST_SPELL, 4, NULL, NULL, 1, NULL);
          test_i.cmc = x_value + 1;
          test_i.cmc_flag = 3;
          test_s.cmc_flag = 3;
          test_s.cmc = test_i.cmc;
          iVar2 = new_special_count_grave(player, &test_i);
          if (iVar2 != 0)
          {
            iVar2 = new_select_a_card(player, player, 2, 0, 2, -1, &test_i);
            if (iVar2 != -1)
            {
              instance->targets[0].player = iVar2;
              iVar2 = turn_card_in_grave_face_down(player, iVar2);
              instance->targets[0].card = iVar2;
            }
          }
          iVar2 = new_special_count_grave(player, &test_s);
          if (iVar2 != 0)
          {
            iVar2 = new_select_a_card(player, player, 2, 0, 2, -1, &test_s);
            if (iVar2 != -1)
            {
              instance->targets[1].player = iVar2;
              instance->targets[1].card = graveyard_ptr_mutable[player][iVar2];
            }
          }
          iVar2 = instance->targets[0].player;
          if (iVar2 != -1)
          {
            turn_card_in_grave_face_up(player, iVar2, instance->targets[0].card);
          }
        }
      }
    }
    else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      instance = get_card_instance(player, card);
      scnprintf(buffer_i, 100, "Select an instant card with cmc %d or less", x_value);
      new_default_test_definition(&test_i, 0x30, buffer_i);
      scnprintf(buffer_s, 100, "Select a sorcery card with cmc %d or less", x_value);
      new_default_test_definition(&test_s, 8, buffer_s);
      if (event == EVENT_RESOLVE_SPELL)
      {
        iVar2 = instance->targets[0].card;
        iVar5 = instance->targets[1].card;
        if (iVar2 != -1)
        {
          iVar3 = can_legally_play_iid(player, iVar2);
          if (iVar3 != 0)
          {
            remove_card_from_grave(player, instance->targets[0].player);
            iVar3 = add_card_to_hand(player, iVar2);
            iVar4 = real_play_spell_for_free(player, iVar3, 0x10100000);
            if (iVar4 == 0)
            {
              obliterate_card(player, iVar3);
            }
            else if (9 < instance->info_slot)
            {
              copy_spell_2022(player, (unsigned int)cards_data[iVar2].id);
              copy_spell_2022(player, (unsigned int)cards_data[iVar2].id);
            }
          }
        }
        if (iVar5 != -1)
        {
          iVar2 = can_legally_play_iid(player, iVar5);
          if (iVar2 != 0)
          {
            remove_card_from_grave(player, instance->targets[1].player);
            iVar2 = add_card_to_hand(player, iVar5);
            iVar3 = real_play_spell_for_free(player, iVar2, 0x10100000);
            if (iVar3 == 0)
            {
              obliterate_card(player, iVar2);
            }
            else if (9 < instance->info_slot)
            {
              copy_spell_2022(player, (unsigned int)cards_data[iVar5].id);
              copy_spell_2022(player, (unsigned int)cards_data[iVar5].id);
            }
          }
        }
        kill_card(player, card, KILL_DESTROY);
        return 0;
      }
    }
    else if (event == EVENT_CHANGE_TARGET)
    {
      get_card_instance(player, card);
      scnprintf(buffer_i, 100, "Select an instant card with cmc %d or less", x_value);
      new_default_test_definition(&test_i, 0x30, buffer_i);
      scnprintf(buffer_s, 100, "Select a sorcery card with cmc %d or less", x_value);
      new_default_test_definition(&test_s, 8, buffer_s);
    }
    iVar2 = 0;
  }
  return iVar2;
}

/* ManalinkEh 2022: 0x21cd630; original cards/war_of_the_spark.c:2121. */
int card_finale_of_revelation(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

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
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          if (instance->info_slot < 10)
          {
            draw_cards(player, instance->info_slot);
          }
          else
          {
            reshuffle_grave_into_deck(player, 0);
            draw_cards(player, instance->info_slot);
            untap_lands(player, card, 5);
            create_legacy_effect(player, card, card_spellbook);
          }
          kill_card(player, card, KILL_EXILE);
        }
        goto LAB_021cd704;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  get_card_instance(player, card);
LAB_021cd704:
  result = generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21d8b10; original cards/war_of_the_spark.c:6820. */
int card_viviens_arkbow(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  char buffer[75];
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      pcVar1 = get_card_instance(player, card);
      pcVar2 = get_card_instance(player, card);
      scnprintf(buffer, 0x4b, " Select a creature card with converted mana cost %d or less.",
                pcVar2->info_slot);
      new_default_test_definition(&test, 2, buffer);
      test.cmc = pcVar1->info_slot + 1;
      test.cmc_flag = 3;
      reveal_top_cards_of_library_and_choose(pcVar1->parent_controller, pcVar1->parent_card, player,
                                             pcVar1->info_slot, 1, TUTOR_PLAY, 0,
                                             TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &test);
    }
  }
  result = generic_activated_ability(player, card, event, 0x21, -1, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21cb8e0; original cards/war_of_the_spark.c:1471. */
int card_dovins_veto(int player, int card, event_t event)
{
  int result;

  cannot_be_countered(player, card, event);
  result = card_negate(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21cdfe0; original cards/war_of_the_spark.c:2421. */
int charge_mana_from_id_minus_two(int player, int card, event_t event, int csvid)
{
  card_ptr_t *cp;
  bool bVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int number;
  unsigned int uVar5;
  unsigned int clred_artifact;
  unsigned int uVar6;
  unsigned int green;
  unsigned int white;
  unsigned int blue;
  int iStack_1c;
  unsigned int uStack_18;
  unsigned int uStack_14;

  if (csvid < 0)
  {
    csvid = get_id(player, card);
  }
  if (card == -1)
  {
    uStack_14 = get_internal_card_id_from_csv_id(csvid);
    if (csvid == 0x25ad)
      goto LAB_021ce247;
  }
  else
  {
    instance = get_card_instance(player, card);
    uStack_14 = instance->internal_card_id;
    if ((unsigned int)cards_data[uStack_14].id != csvid)
    {
      backtrace_and_abort("cards/war_of_the_spark.c:2435\ncards_data[iid].id == csvid");
    }
    if (csvid == 0x25ad)
    {

      if (-1 < card)
      {
        iVar3 = casting_reaper_king(player, card, event);
        return iVar3;
      }
    LAB_021ce247:
      iStack_1c = add_card_to_hand(player, uStack_14);
      iVar3 = casting_reaper_king(player, iStack_1c, event);
      goto LAB_021ce1dc;
    }
  }
  cp = cards_ptr[csvid];
  uVar6 = (unsigned int)cp->req_colorless;
  if (uVar6 == 0x28)
  {
    uVar6 = 0;
  }
  clred_artifact = (unsigned int)(cards_data[uStack_14].type & 0x40);
  iVar3 = is_phyrexian(csvid);
  if (iVar3 == 0)
  {

    if (card < 0)
    {
      iVar3 = get_updated_casting_cost(player, -1, uStack_14, event, uVar6);
      uStack_18 = iVar3 - 2U & (int)(iVar3 - 2U ^ 0xffffffff) >> 0x1f;
      iVar3 = is_normal_hybrid(cp);
      if (iVar3 == 0)
      {
        iVar3 = is_monocolor_hybrid(cp);
        if (iVar3 == 0)
        {
        LAB_021ce2db:

          if (csvid == 0x2ded)
          {
            iVar3 = count_graveyard_by_type(player, 2);
            uStack_18 = uStack_18 - iVar3 & (int)(uStack_18 - iVar3 ^ 0xffffffff) >> 0x1f;
          }
          uVar6 = uStack_18;
          if (cards_data[uStack_14].cc[1] == 0xff)
          {
            if (uStack_18 != 0)
            {
              uStack_14 = (unsigned int)cp->req_red;
              blue = (unsigned int)cp->req_blue;
              white = (unsigned int)cp->req_white;
              green = (unsigned int)cp->req_green;
              uVar5 = (unsigned int)cp->req_black;
              if (clred_artifact == 0)
              {
                uVar6 = 0;
                bVar1 = true;
              }
              else
              {
                bVar1 = true;
                uStack_18 = 0;
              }
              goto LAB_021ce32d;
            }
            uStack_18 = 0xffffffff;
          }
          uStack_14 = (unsigned int)cp->req_red;
          blue = (unsigned int)cp->req_blue;
          white = (unsigned int)cp->req_white;
          green = (unsigned int)cp->req_green;
          uVar5 = (unsigned int)cp->req_black;
          if (clred_artifact != 0)
          {
            iVar3 = charge_mana_multi_a(player, 0, uVar5, blue, green, uStack_14, white, uStack_18);
            return iVar3;
          }
          uVar6 = 0;
          bVar1 = false;
        LAB_021ce32d:
          iVar3 =
              charge_mana_multi_a(player, uStack_18, uVar5, blue, green, uStack_14, white, uVar6);
          if (iVar3 == 0)
          {
            return 0;
          }
          if (!bVar1)
          {
            return iVar3;
          }
          iVar3 = charge_mana_multi_a(player, -(unsigned int)(clred_artifact == 0), 0, 0, 0, 0, 0,
                                      -(unsigned int)(clred_artifact != 0));
          return iVar3;
        }
        card = add_card_to_hand(player, uStack_14);
        instance = get_card_instance(player, card);
        uVar6 = instance->state;
        hand_count[player] = hand_count[player] + -1;
        instance->state = uVar6 | 0x20;
        iStack_1c = card;
      LAB_021ce466:

        iVar3 = get_color_for_monocolored_hybrid(player, card);
        iVar4 = get_number_of_monocolored_hybrid_mana(player, card, 0);
        iVar3 = charge_mana_for_monocolor_hybrid(player, card, iVar4, iVar3, uStack_18);
        goto joined_r0x021ce1d6;
      }
      card = add_card_to_hand(player, uStack_14);
      instance = get_card_instance(player, card);
      instance->state = instance->state | 0x20;
      hand_count[player] = hand_count[player] + -1;
      iStack_1c = card;
    }
    else
    {
      iVar3 = get_updated_casting_cost(player, card, -1, event, uVar6);
      uStack_18 = iVar3 - 2U & (int)(iVar3 - 2U ^ 0xffffffff) >> 0x1f;
      iVar3 = is_normal_hybrid(cp);
      iStack_1c = -1;
      if (iVar3 == 0)
      {
        iVar3 = is_monocolor_hybrid(cp);
        if (iVar3 == 0)
          goto LAB_021ce2db;
        goto LAB_021ce466;
      }
    }

    iVar3 = get_colors_for_hybrid(player, card, 0, 0);
    iVar4 = get_colors_for_hybrid(player, card, iVar3, 0);
    number = get_number_of_hybrid_mana(player, card, 0, 0);
    iVar3 = charge_mana_hybrid(player, card, number, iVar3, iVar4, uStack_18);
  }
  else
  {

    iStack_1c = -1;
    if (card < 0)
    {
      card = add_card_to_hand(player, uStack_14);
      instance = get_card_instance(player, card);
      uVar5 = instance->state;
      hand_count[player] = hand_count[player] + -1;
      instance->state = uVar5 | 0x20;
      iStack_1c = card;
    }
    iVar3 = charge_phyrexian_mana_2022(
        player, card, event, uVar6 - 2 & (int)(uVar6 - 2 ^ 0xffffffff) >> 0x1f,
        (unsigned int)cp->req_black, (unsigned int)cp->req_blue, (unsigned int)cp->req_green,
        (unsigned int)cp->req_red, (unsigned int)cp->req_white, clred_artifact);
  }
joined_r0x021ce1d6:
  if (iStack_1c < 0)
  {
    return iVar3;
  }
LAB_021ce1dc:
  obliterate_card(player, iStack_1c);
  return iVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int massacre_girl_legacy(int player, int card, event_t event);
static int colors_to_guilds(int iid);
int card_return_to_nature(int player, int card, event_t event);

/* ManalinkEh 2022: 0x21c8a00; original cards/war_of_the_spark.c:268. */
int card_arlinn_voice_of_the_pack(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  test_definition_t test;

  check_legend_rule(player, card, event);
  instance = in_play(player, card);
  if ((((instance != NULL) &&
        (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
       ((event == EVENT_CAST_SPELL || (event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE)))) &&
      (affected_card_controller == player))
  {
    default_test_definition(&test, 2);
    test.subtype = 0xd7;
    test.sub2 = 0xd4;
    test.subtype_flag = 3;
    result = new_make_test_in_play(affected_card_controller, affected_card, -1, &test);
    if (result != 0)
    {
      enters_the_battlefield_with_counters(affected_card_controller, affected_card, event,
                                           COUNTER_P1_P1, 1);
    }
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = count_counters(player, card, COUNTER_LOYALTY);
      if (result < 2)
      {
        return 0;
      }
      goto LAB_021c8a7e;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003",
                           "\001\014", "\001\017", "Create a Wolf", 1, 5, 0xfffffffe, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_021c8a7e;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021c8a7e;
    get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    generate_token_by_id_2022(player, card, 0x49a8);
  }
LAB_021c8a7e:
  result = planeswalker(player, card, event, 7);
  return result;
}

/* ManalinkEh 2022: 0x21c8cd0; original cards/war_of_the_spark.c:333. */
int card_ashiok_dream_render(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);
  hybrid(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_target(&td);
      if (result == 0)
      {
        return 0;
      }
      goto LAB_021c8d57;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003",
                           "\001\014", "\001\017", "Mill 4, then exile opp's graveyard", result, 5,
                           0xffffffff, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      new_pick_target(&td, "TARGET_PLAYER", 0, 1);
      goto LAB_021c8d57;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021c8d57;
    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    mill(instance->targets[0].player, 4);
    rfg_whole_graveyard(1 - player);
  }
LAB_021c8d57:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x21c9c50; original cards/war_of_the_spark.c:727. */
int card_casualties_of_war(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int iVar3;
  target_t *ptVar4;
  int iVar5;
  target_t *ptVar6;
  bool bVar7;
  target_definition_t td;
  target_definition_t td1;
  target_definition_t td2;
  target_definition_t td3;
  target_definition_t td4;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x40);
    default_target_definition(player, card, &td1, 2);
    default_target_definition(player, card, &td2, 4);
    default_target_definition(player, card, &td3, 1);
    default_target_definition(player, card, &td4, 0x100);
    get_card_instance(player, card);
    iVar5 = can_target(&td);
    if (((iVar5 != 0) || (iVar5 = can_target(&td1), iVar5 != 0)) ||
        ((iVar5 = can_target(&td2),
          iVar5 != 0 ||
              ((iVar5 = can_target(&td3), iVar5 != 0 || (iVar5 = can_target(&td4), iVar5 != 0))))))
    {
      iVar5 = basic_spell(player, card, EVENT_CAN_CAST);
      return iVar5;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x40);
      default_target_definition(player, card, &td1, 2);
      default_target_definition(player, card, &td2, 4);
      default_target_definition(player, card, &td3, 1);
      default_target_definition(player, card, &td4, 0x100);
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        ptVar6 = instance->targets;
        instance->number_of_targets = '\0';
        ptVar4 = ptVar6;
        do
        {
          ptVar4->card = -1;
          ptVar4->player = -1;
          ptVar4 = ptVar4 + 1;
        } while (instance->targets + 10 != ptVar4);
        iVar5 = can_target(&td);
        if ((iVar5 != 0) && (iVar5 = new_pick_target(&td, "TARGET_ARTIFACT", 0, 0), iVar5 != 0))
        {

          state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
          instance->targets[instance->number_of_targets + 4].player = 1;
        }
        iVar5 = can_target(&td1);
        if ((iVar5 != 0) && (iVar5 = new_pick_target(&td1, "TARGET_CREATURE", -1, 0), iVar5 != 0))
        {

          state_untargettable(instance->targets[instance->number_of_targets - 1].player,
                              instance->targets[instance->number_of_targets - 1].card, 1);
          instance->targets[instance->number_of_targets + 4].player = 2;
        }
        iVar5 = can_target(&td2);
        if ((iVar5 != 0) &&
            (iVar5 = new_pick_target(&td2, "TARGET_ENCHANTMENT", -1, 0), iVar5 != 0))
        {

          instance->targets[instance->number_of_targets + 4].player = 4;
        }
        iVar5 = can_target(&td3);
        if ((iVar5 != 0) && (iVar5 = new_pick_target(&td3, "TARGET_LAND", -1, 0), iVar5 != 0))
        {

          instance->targets[instance->number_of_targets + 4].player = 8;
        }
        iVar5 = can_target(&td4);
        if ((iVar5 != 0) &&
            (iVar5 = new_pick_target(&td4, "TARGET_PLANESWALKER", -1, 0), iVar5 != 0))
        {

          instance->targets[instance->number_of_targets + 4].player = 0x10;
        }
        do
        {
          if ((ptVar6->player != -1) && (ptVar6->card != -1))
          {
            state_untargettable(ptVar6->player, ptVar6->card, 0);
          }
          ptVar6 = ptVar6 + 1;
        } while (instance->targets + 5 != ptVar6);
        if (instance->number_of_targets == '\0')
        {
          spell_fizzled = 1;
        }
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x40);
    default_target_definition(player, card, &td1, 2);
    default_target_definition(player, card, &td2, 4);
    default_target_definition(player, card, &td3, 1);
    default_target_definition(player, card, &td4, 0x100);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      if (instance->number_of_targets != '\0')
      {
        iVar5 = 0;
        do
        {
          while (uVar2 = instance->targets[iVar5 + 5].player, (int)uVar2 < 0)
          {
          LAB_021c9d9e:
            iVar5 = iVar5 + 1;
            if ((int)(unsigned int)instance->number_of_targets <= iVar5)
              goto LAB_021c9f14;
          }
          if ((uVar2 & 1) == 0)
          {

            bVar7 = false;
            if ((uVar2 & 2) != 0)
              goto LAB_021c9e00;
          LAB_021c9d78:
            if ((uVar2 & 4) == 0)
              goto LAB_021c9d80;
          LAB_021c9e40:
            iVar3 = validate_target(player, card, &td2, iVar5);
            uVar2 = instance->targets[iVar5 + 5].player;
            if (iVar3 == 0)
              goto LAB_021c9d80;
            bVar7 = true;
            if ((uVar2 & 8) != 0)
              goto LAB_021c9e80;
          LAB_021c9d88:
            if ((uVar2 & 0x10) != 0)
            {
            LAB_021c9ec0:
              iVar3 = validate_target(player, card, &td4, iVar5);
              if (iVar3 != 0)
                goto LAB_021c9ee5;
            }
            if (!bVar7)
              goto LAB_021c9d9e;
          }
          else
          {
            iVar3 = validate_target(player, card, &td, iVar5);
            bVar7 = iVar3 != 0;
            uVar2 = instance->targets[iVar5 + 5].player;
            if ((uVar2 & 2) == 0)
              goto LAB_021c9d78;
          LAB_021c9e00:
            iVar3 = validate_target(player, card, &td1, iVar5);
            uVar2 = instance->targets[iVar5 + 5].player;
            if (iVar3 == 0)
              goto LAB_021c9d78;
            bVar7 = true;
            if ((uVar2 & 4) != 0)
              goto LAB_021c9e40;
          LAB_021c9d80:
            if ((uVar2 & 8) == 0)
              goto LAB_021c9d88;
          LAB_021c9e80:
            iVar3 = validate_target(player, card, &td3, iVar5);
            if (iVar3 == 0)
            {
              uVar2 = instance->targets[iVar5 + 5].player;
              goto LAB_021c9d88;
            }
            if ((instance->targets[iVar5 + 5].player & 0x10) != 0)
            {
              bVar7 = true;
              goto LAB_021c9ec0;
            }
          }
        LAB_021c9ee5:
          ptVar4 = instance->targets + iVar5;
          ptVar6 = instance->targets + iVar5;
          iVar5 = iVar5 + 1;
          kill_card(ptVar6->player, ptVar4->card, KILL_DESTROY);
        } while (iVar5 < (int)(unsigned int)instance->number_of_targets);
      }
    LAB_021c9f14:
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21ca3e0; original cards/war_of_the_spark.c:873. */
int card_chandra_fire_artisan(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if (event == EVENT_COUNTERS)
  {
    if (((card == affected_card) && (player == affected_card_controller)) && (counters_added < 0))
    {
      opponent_or_pw_target_definition(player, card, &td);
      iVar3 = can_target(&td);
      if ((iVar3 != 0) &&
          (iVar3 = new_pick_target(&td, "TARGET_OPPONENT_OR_PLANESWALKER", 0, 1), iVar3 != 0))
      {
        damage_target0(player, card, -counters_added);
      }
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      instance = get_card_instance(player, card);
      if (event == EVENT_ACTIVATE)
      {

        instance->number_of_targets = '\0';
        instance->info_slot = 0;
        iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Exile one card",
                            1, 5, 1, "Exile seven cards", 1, 10, 0xfffffff9, 0, 0, 0, 0);
        if (iVar3 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        goto LAB_021ca43a;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_021ca43a;
      instance = get_card_instance(player, card);
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (instance->info_slot == 1)
      {

        iVar3 = *deck_ptr[player];
        if (iVar3 != -1)
        {
          rfg_top_card_of_deck(player);
          create_may_play_card_from_exile_effect(
              player, card, player, (unsigned int)cards_data[iVar3].id, MPCFE_UNTIL_EOT);
        }
      }
      else if (instance->info_slot == 2)
      {
        iVar3 = 7;
        do
        {

          iVar1 = *deck_ptr[player];
          if (iVar1 != -1)
          {
            rfg_top_card_of_deck(player);
            create_may_play_card_from_exile_effect(
                player, card, player, (unsigned int)cards_data[iVar1].id, MPCFE_UNTIL_EOT);
          }
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
      }
    }
  }
LAB_021ca43a:

  iVar3 = planeswalker(player, card, event, 4);
  return iVar3;
}

/* ManalinkEh 2022: 0x21cbca0; original cards/war_of_the_spark.c:1559. */
int card_dreadhorde_arcanist(int player, int card, event_t event)
{
  int result;
  char buffer[75];
  test_definition_t test;

  result = duh_mode(player);
  dat_ability(player, card, event, (result != 0) + DAT_RESOLVE_TRIGGER_OPTIONAL, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    result = get_power(player, card);
    scnprintf(buffer, 0x4b,
              " Select an instant or sorcery card with converted mana cost %d or less.",
              result + 1);
    new_default_test_definition(&test, 0x38, buffer);
    result = get_power(player, card);
    test.cmc = result + 1;
    test.cmc_flag = 3;
    result = new_special_count_grave(player, &test);
    if (result != 0)
    {

      result = new_select_a_card(player, player, 2, 0, 1, -1, &test);
      if (result != -1)
      {
        play_card_in_grave_for_free_and_exile_it(player, player, result);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21cd7f0; original cards/war_of_the_spark.c:2162. */
int card_flux_channeler(int player, int card, event_t event)
{
  prowess_mode(player, card, event, 2);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  proliferate(player, card);
  return 0;
}

/* ManalinkEh 2022: 0x21cf060; original cards/war_of_the_spark.c:2742. */
int card_huatli_the_suns_heart(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  hybrid(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = count_counters(player, card, COUNTER_LOYALTY);
      if (result < 3)
      {
        return 0;
      }
      goto LAB_021cf0be;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003",
                           "\001\014", "\001\017", "Gain life", 1, 5, 0xfffffffd, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_021cf0be;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021cf0be;
    get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    default_test_definition(&this_test, 2);
    result = check_battlefield_for_special_card(player, card, player, 0x100, &this_test);
    gain_life_2022(player, result);
  }
LAB_021cf0be:
  result = planeswalker(player, card, event, 7);
  return result;
}

/* ManalinkEh 2022: 0x21cf450; original cards/war_of_the_spark.c:2951. */
int card_jace_wielder_of_mysteries(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);

  if (((((trigger_condition == 0xcf) && (card == affected_card)) &&
        (player == affected_card_controller)) &&
       ((reason_for_trigger_controller == player && (suppress_draw == 0)))) &&
      ((result = is_humiliated_for_ability(player, card, AL_STATIC),
        result == 0 && (*deck_ptr[player] == -1))))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result = event_result | 2;
      goto LAB_021cf4db;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      lose_the_game_2022(1 - player);
      suppress_draw = 1;
      goto LAB_021cf4db;
    }
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Mill 3 and draw",
                           1, 5, 1, "Draw 7", 1, 0x14, 0xfffffff8, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      new_pick_target(&td, "TARGET_PLAYER", 0, 1);
      goto LAB_021cf4db;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021cf4db;
    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->info_slot == 1)
    {
      mill(instance->targets[0].player, 2);
      draw_a_card(player);
    }
    else if ((instance->info_slot == 2) && (draw_cards(player, 7), *deck_ptr[player] == -1))
    {
      lose_the_game_2022(1 - player);
    }
  }
LAB_021cf4db:
  result = planeswalker(player, card, event, 4);
  return result;
}

/* ManalinkEh 2022: 0x21cf710; original cards/war_of_the_spark.c:3036. */
int card_jaya_venerated_firemage(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *instance;
  int result;
  unsigned int uVar4;
  target_definition_t td;

  instance = damage_being_dealt(event);
  if ((((instance != NULL) && (instance->damage_source_player == player)) &&
       (instance->damage_source_card != card)) &&
      ((cVar1 = instance->initial_color, uVar4 = get_sleighted_color_test(player, card, 0x10),
        ((int)cVar1 & uVar4) != 0 &&
            (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))))
  {
    instance->info_slot_fn = instance->info_slot_fn + 1;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_target(&td);
      if (result == 0)
      {
        return 0;
      }
      result = count_counters(player, card, COUNTER_LOYALTY);
      if (result < 2)
      {
        return 0;
      }
      goto LAB_021cf79c;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003", "\001\014",
                      "\001\017", "Deal 2 damage", result, 5, 0xfffffffe, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      new_pick_target(&td, "TARGET_ANY", 0, 1);
      goto LAB_021cf79c;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021cf79c;
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 1;
    get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
  {
    damage_target0(player, card, 2);
  }
LAB_021cf79c:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x21c7d20; original cards/war_of_the_spark.c:3090. */
static char *has_a_counter_and_not_mana_src(int who_chooses, int player, int card)
{
  int result;

  result = count_counters(player, card, ~COUNTER_P1_P1);
  if (result != 0)
  {
    result = get_id(player, card);
    result = get_internal_card_id_from_csv_id(result);
    if (((cards_data[result].extra_ability & 0x1000) == 0) &&
        (result = is_humiliated_for_ability(player, card, AL_MANA), result == 0))
    {
      return NULL;
    }
  }
  return "Must have a counter.";
}

/* ManalinkEh 2022: 0x21d0210; original cards/war_of_the_spark.c:3356. */
int card_kayas_ghostform(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int result;
  target_definition_t td;

  pcVar2 = get_card_instance(player, card);
  pcVar3 = in_play(player, card);
  if (pcVar3 == NULL)
  {
  LAB_021d024f:
    if (event == EVENT_CAN_CAST)
      goto LAB_021d0268;
    if (event != EVENT_CAST_SPELL)
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
        goto LAB_021d0268;
      goto LAB_021d032b;
    }
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_021d0268;
  }
  else
  {
    cVar1 = pcVar2->damage_target_player;
    if ((event != EVENT_GRAVEYARD_FROM_PLAY) || (cVar1 == -1))
      goto LAB_021d024f;
    if (((pcVar2->damage_target_card == affected_card) && (cVar1 == affected_card_controller)) &&
        (result = check_for_death_trigger_removal((int)cVar1, affected_card), result == 0))
    {
      cVar1 = pcVar2->damage_target_player;
      result = pcVar2->damage_target_card;
      pcVar3 = in_play((int)cVar1, result);

      if ((pcVar3 != NULL) && (pcVar3 = get_card_instance((int)cVar1, result),
                               (unsigned char)(pcVar3->kill_code - 1) < 4))
      {

        result = create_legacy_effect(player, card, kayas_ghostform_effect);
        pcVar3 = get_card_instance(player, result);
        result = get_owner((int)pcVar2->damage_target_player, pcVar2->damage_target_card);
        pcVar3->targets[0].player = result;
        result = get_original_internal_card_id((int)pcVar2->damage_target_player,
                                               pcVar2->damage_target_card);
        pcVar3->targets[0xb].player = 1;
        pcVar3->targets[0].card = result;
      }
    }
  LAB_021d032b:
    if (event == EVENT_CHANGE_TARGET)
      goto LAB_021d0268;
  }
  if (((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
        (2 < event - EVENT_POWER)) &&
       ((event != EVENT_RESOLVE_MOVING_AURA &&
         (((event != EVENT_CHANGE_TYPE && (event != EVENT_SET_COLOR)) &&
           (event != EVENT_GET_SELECTED_CARD)))))) &&
      (event != EVENT_STATIC_EFFECTS))
  {
    return 0;
  }
LAB_021d0268:
  default_target_definition(player, card, &td, 0x102);
  td.allowed_controller = player;
  result = targeted_aura_custom_prompt(player, card, event, &td,
                                       "Select target creature or planeswalker you control.");
  return result;
}

/* ManalinkEh 2022: 0x21d0410; original cards/war_of_the_spark.c:3392. */
int card_kaya_bane_of_the_dead(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);
  hybrid(player, card, event);
  if (event == EVENT_ABILITIES)
  {
    if (((1 - player == affected_card_controller) &&
         (result = is_what(1 - player, affected_card, 0x47), result != 0)) &&
        (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
    {
      set_special_flags_2022(affected_card_controller, affected_card, 0x20000);
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition(player, card, &td, 2);
      td.allow_cancel = 1;
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        result = can_target(&td);
        if (result == 0)
        {
          return 0;
        }
        result = count_counters(player, card, COUNTER_LOYALTY);
        if (result < 3)
        {
          return 0;
        }
        goto LAB_021d04aa;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_021d04aa;
      default_target_definition(player, card, &td, 2);
      td.allow_cancel = 1;
      instance = get_card_instance(player, card);
    }
    if (event == EVENT_ACTIVATE)
    {

      result = can_target(&td);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003",
                           "\001\026", "\001\014", "\001\017", "Exile target creature.", result, 5,
                           0xfffffffd, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      new_pick_target(&td, "TARGET_CREATURE", 0, 1);
    }
    else if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
    }
  }
LAB_021d04aa:
  result = planeswalker(player, card, event, 7);
  return result;
}

/* ManalinkEh 2022: 0x21d06d0; original cards/war_of_the_spark.c:3452. */
int card_kiora_behemoth_beckoner(int player, int card, event_t event)
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
  hybrid(player, card, event);
  if ((((trigger_condition == 0xdb) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    default_test_definition(&recovered_stack_td.test, 2);
    etb_ability_global(player, event, 2, TYPE_NONE, &recovered_stack_td.test, player, card);
  }
  if (event == EVENT_ETB)
  {
    result = is_what(trigger_cause_controller, trigger_cause, 2);
    if (result != 0)
    {
      draw_cards(player, 1);
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      instance = get_card_instance(player, card);
      default_target_definition(player, card, &recovered_stack_td.target, 0x47);
      recovered_stack_td.target.allow_cancel = 1;
      if (event == EVENT_CAN_ACTIVATE)
      {
        result = can_target(&recovered_stack_td.target);
        if (result == 0)
        {
          return 0;
        }
        goto LAB_021d0794;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_021d0794;
      instance = get_card_instance(player, card);
      default_target_definition(player, card, &recovered_stack_td.target, 0x47);
    }
    recovered_stack_td.target.allow_cancel = 1;
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&recovered_stack_td.target);
      result =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003", "\001\014",
                      "\001\017", "Untap a permanent", result, 5, 0xffffffff, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      new_pick_target(&recovered_stack_td.target, "TARGET_PERMANENT", 0, 1);
    }
    else if ((event == EVENT_RESOLVE_ACTIVATION) &&
             (result = valid_target(&recovered_stack_td.target), result != 0))
    {
      untap_card(instance->targets[0].player, instance->targets[0].card);
    }
  }
LAB_021d0794:
  result = planeswalker(player, card, event, 7);
  return result;
}

/* ManalinkEh 2022: 0x21c7420; original cards/war_of_the_spark.c:3895. */
static int massacre_girl_legacy(int player, int card, event_t event)
{
  unsigned int player_00;
  int card_00;
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int player_01;
  unsigned int uVar4;
  int card_01;

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    count_for_gfp_ability(player, card, 0x77, 2, 2, NULL);
  }
  iVar1 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (iVar1 != 0)
  {

    uVar4 = 0;
    instance = get_card_instance(player, card);
    iVar1 = -iVar1;
    player_00 = instance->targets[1].player;
    card_00 = instance->targets[1].card;
    do
    {
      card_01 = 0;
      player_01 = current_turn ^ uVar4;
      if (0 < active_cards_count[player_01])
      {
        do
        {
          instance = in_play(player_01, card_01);
          if ((((instance != NULL) && (iVar3 = is_this_dying(player_01, card_01), iVar3 == 0)) &&
               (iVar3 = is_what(player_01, card_01, 2), iVar3 != 0)) &&
              ((player_00 != player_01 || (card_00 != card_01))))
          {
            if ((player_00 == 0xffffffff) ||
                ((card_00 == -1 || (instance = in_play(player_00, card_00), instance == NULL))))
            {
              pump_until_eot_merge_previous_2022(player, card, player_01, card_01, iVar1, iVar1);
            }
            else
            {
              pump_until_eot_merge_previous_2022(player_00, card_00, player_01, card_01, iVar1, iVar1);
            }
          }
          card_01 = card_01 + 1;
        } while (card_01 < active_cards_count[player_01]);
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 != 2);
  }

  iVar1 = eot_trigger(player, card, event);
  if (iVar1 != 0)
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21d15e0; original cards/war_of_the_spark.c:3920. */
int card_massacre_girl(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int player_00;
  unsigned int uStack_14;

  check_legend_rule(player, card, event);
  menace(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    iVar1 = create_legacy_effect(player, card, massacre_girl_legacy);
    if (iVar1 == -1)
    {
      backtrace_and_abort("cards/war_of_the_spark.c:3934\nlegacy != -1");
    }
    instance = get_card_instance(player, iVar1);
    instance->targets[1].player = player;
    instance = get_card_instance(player, iVar1);
    instance->targets[1].card = card;
    add_status(player, iVar1, 0x10000);

    uStack_14 = 0;
    do
    {
      player_00 = uStack_14 ^ current_turn;
      if (0 < active_cards_count[player_00])
      {
        iVar1 = 0;
        do
        {
          instance = in_play(player_00, iVar1);
          if (((instance != NULL) && (iVar3 = is_what(player_00, iVar1, 2), iVar3 != 0)) &&
              ((player != player_00 || (card != iVar1))))
          {
            pump_until_eot_merge_previous_2022(player, card, player_00, iVar1, -1, -1);
          }
          iVar1 = iVar1 + 1;
        } while (iVar1 < active_cards_count[player_00]);
      }
      uStack_14 = uStack_14 + 1;
    } while (uStack_14 != 2);
    return 0;
  }

  return 0;
}

/* ManalinkEh 2022: 0x21c7620; original cards/war_of_the_spark.c:4680. */
static int colors_to_guilds(int iid)
{
  unsigned int uVar1;
  int result;
  unsigned int v;

  v = (unsigned int)cards_data[iid].color;
  uVar1 = num_bits_set(v);
  if (uVar1 == 2)
  {
    if (v == 0x24)
    {
      return 1;
    }
    if (v == 6)
    {
      result = 2;
    }
    else if (v == 0x12)
    {
      result = 4;
    }
    else if (v == 0x18)
    {
      result = 8;
    }
    else if (v == 0x28)
    {
      result = 0x10;
    }
    else if (v == 0x22)
    {
      result = 0x20;
    }
    else if (v == 10)
    {
      result = 0x40;
    }
    else if (v == 0xc)
    {
      result = 0x80;
    }
    else
    {
      if (v != 0x14)
      {
        return (unsigned int)(v == 0x30) << 9;
      }
      result = 0x100;
    }
  }
  else
  {
    result = 0;
  }
  return result;
}

/* ManalinkEh 2022: 0x21c7f20; original cards/war_of_the_spark.c:4706. */
static int not_selected(int iid, int chosen, int unused1, int unused2)
{
  int result;
  unsigned int uVar2;

  result = colors_to_guilds(iid);
  if (result < 1)
  {
    return 0;
  }
  uVar2 = colors_to_guilds(iid);
  return (unsigned int)((chosen & uVar2) == 0);
}

/* ManalinkEh 2022: 0x21d3fc0; original cards/war_of_the_spark.c:4876. */
int card_parhelion_ii(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    default_token_definition(player, card, 0x48ff, &token);
    token.qty = '\x02';
    token.s_key_plus = 8;
    token.action = TOKEN_ACTION_ATTACKING_UNTAPPED;
    generate_token(&token);
  }
  result = vehicle(player, card, event, 4);
  return result;
}

/* ManalinkEh 2022: 0x21d4da0; original cards/war_of_the_spark.c:5302. */
int card_return_to_nature(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  target_definition_t *td_00;
  const char *prompt;
  target_definition_t td;
  target_definition_t td2;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x40);
    default_target_definition(player, card, &td2, 4);
    new_default_test_definition(&test, 0xff, "Select target card to exile.");
    get_card_instance(player, card);
    iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    if (iVar2 != 0)
    {
      return 1;
    }
    iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL);
    if (iVar2 == 0)
    {
      iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 0x1000, NULL, NULL, 1, &test);
      return iVar2;
    }
    return 1;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      td_00 = &td2;
      default_target_definition(player, card, &td, 0x40);
      default_target_definition(player, card, td_00, 4);
      new_default_test_definition(&test, 0xff, "Select target card to exile.");
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        iVar2 = check_special_flags2_2022(player, card, -0x80000000);
        if (iVar2 == 0)
        {

          iVar2 = can_target(&td);
          iVar3 = can_target(td_00);
          iVar4 = generic_spell(player, card, EVENT_CAN_CAST, 0x1000, NULL, NULL, 1, &test);
          iVar2 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "Destroy target artifact",
                              iVar2, 10, "Destroy target enchantment", iVar3, 10,
                              "Exile target card from a graveyard", iVar4, 3, 0, 0, 0, 0);
          if (iVar2 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
        }
        iVar2 = instance->info_slot;
        if (iVar2 == 1)
        {
          td_00 = &td;
          prompt = "TARGET_ARTIFACT";
        }
        else
        {
          if (iVar2 != 2)
          {
            if (iVar2 != 3)
            {
              return 0;
            }
            iVar2 = generic_spell(player, card, EVENT_CAST_SPELL, 0x1000, NULL, NULL, 1, &test);
            return iVar2;
          }
          prompt = "TARGET_ENCHANTMENT";
        }
        iVar2 = generic_spell(player, card, EVENT_CAST_SPELL, 1, td_00, prompt, 1, NULL);
        return iVar2;
      }
    }
    return 0;
  }
  if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
  {
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x40);
    default_target_definition(player, card, &td2, 4);
    new_default_test_definition(&test, 0xff, "Select target card to exile.");
    get_card_instance(player, card);
    return 0;
  }
  default_target_definition(player, card, &td, 0x40);
  default_target_definition(player, card, &td2, 4);
  new_default_test_definition(&test, 0xff, "Select target card to exile.");
  instance = get_card_instance(player, card);
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }
  iVar2 = instance->info_slot;
  if (iVar2 == 1)
  {
    iVar2 = valid_target(&td);
    if (iVar2 == 0)
    {
      iVar2 = instance->info_slot;
      goto LAB_021d4e5d;
    }
  LAB_021d51b0:
    kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
  }
  else
  {
  LAB_021d4e5d:
    if (iVar2 != 2)
      goto LAB_021d4e66;
    iVar2 = valid_target(&td2);
    if (iVar2 != 0)
      goto LAB_021d51b0;
  }
  iVar2 = instance->info_slot;
LAB_021d4e66:

  if ((iVar2 == 3) &&
      (iVar2 = validate_target_from_grave_source(player, card, instance->targets[0].player, 1),
       iVar2 != -1))
  {
    rfg_card_from_grave(instance->targets[0].player, iVar2);
  }
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

/* ManalinkEh 2022: 0x21d54d0; original cards/war_of_the_spark.c:5447. */
int card_saheeli_sublime_artificer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  target_definition_t *ptStack_c0;
  target_definition_t *ptStack_bc;
  target_definition_t td;
  target_definition_t td2;

  check_legend_rule(player, card, event);
  hybrid(player, card, event);
  prowess_mode(player, card, event, 2);
  if (event == EVENT_SPA)
  {
    generate_token_by_id_2022(player, card, 0x4980);
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition(player, card, &td, 0x40);
      td.allowed_controller = player;
      td.allow_cancel = 1;
      default_target_definition(player, card, &td2, 0x42);
      td2.allowed_controller = player;
      td2.allow_cancel = 1;
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        result = can_target(&td);
        if (((result == 0) || (result = can_target(&td2), result == 0)) ||
            (result = count_counters(player, card, COUNTER_LOYALTY), result < 2))
        {
          return 0;
        }
        goto LAB_021d55be;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_021d55be;
      default_target_definition(player, card, &td, 0x40);
      td.allowed_controller = player;
      td.allow_cancel = 1;
      default_target_definition(player, card, &td2, 0x42);
      td2.allowed_controller = player;
      td2.allow_cancel = 1;
      instance = get_card_instance(player, card);
    }
    ptStack_bc = &td;
    ptStack_c0 = &td2;
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(ptStack_bc);
      uVar3 = 0;
      if (result != 0)
      {
        result = can_target(ptStack_c0);
        uVar3 = (unsigned int)(result != 0);
      }
      result =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003", "\001\014",
                      "\001\017", "Copy artifact or creature", uVar3, 5, 0xfffffffe, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      result = new_pick_target(ptStack_bc, "TARGET_ARTIFACT", 0, 1);
      if (result != 0)
      {
        new_pick_target(ptStack_c0, "PUTREFY", 1, 1);
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {
      shapeshift_target(player, card, instance->targets[0].player, instance->targets[0].card,
                        instance->targets[1].player, instance->targets[1].card, 5);
    }
  }
LAB_021d55be:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x21d5850; original cards/war_of_the_spark.c:5513. */
int card_samut_tyrant_smasher(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);
  hybrid(player, card, event);
  boost_subtype(player, card, event, ~SUBTYPE_INVALID, 0, 0, 0, 4,
                BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allow_cancel = 1;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_target(&td);
      if (result == 0)
      {
        return 0;
      }
      goto LAB_021d5919;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003", "\001\014",
                      "\001\017", "Boost a creature", result, 5, 0xffffffff, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      goto LAB_021d5919;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021d5919;
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
  }
  td.preferred_controller = player;
  td.allow_cancel = 1;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    pump_ability_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card, 2,
                           1, 0, 4);
    scry(player, 1);
  }
LAB_021d5919:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x21d6dc0; original cards/war_of_the_spark.c:6083. */
int card_teferi_time_raveler(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  bool bVar3;
  target_definition_t *ptStack_68;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    if (((affected_card_controller != player) &&
         (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
        (result = can_sorcery_be_played(affected_card_controller, EVENT_MODIFY_COST_GLOBAL),
         result == 0))
    {
      infinite_casting_cost();
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition(player, card, &td, 0x46);
      td.allow_cancel = 1;
      instance = get_card_instance(player, card);
      if (event == EVENT_ACTIVATE)
      {

        instance->number_of_targets = '\0';
        instance->info_slot = 0;
        result = can_target(&td);
        result =
            dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                        "Cast sorceries with flash", 1, 5, 1, "Bounce and draw", 1,
                        (-(unsigned int)(result == 0) & 0xfffffff9) + 10, 0xfffffffd, 0, 0, 0, 0);
        if (result == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if ((result == 2) && (result = can_target(&td), result != 0))
        {
          new_pick_target(&td, "Select target artifact, creature or enchantment.", 0, 0x80);
        }
        goto LAB_021d6e41;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_021d6e41;
      default_target_definition(player, card, &td, 0x46);
      td.allow_cancel = 1;
      instance = get_card_instance(player, card);
    }
    ptStack_68 = &td;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      if (instance->info_slot == 1)
      {
        bVar3 = current_turn == player;
        result = create_legacy_activate(player, card, teferi_flash_effect);
        instance = get_card_instance(player, result);
        instance->targets[9].card = (unsigned int)bVar3;
      }
      else if (instance->info_slot == 2)
      {
        result = valid_target(ptStack_68);
        if (result != 0)
        {
          bounce_permanent(instance->targets[0].player, instance->targets[0].card);
        }
        draw_a_card(player);
      }
    }
  }
LAB_021d6e41:
  result = planeswalker(player, card, event, 4);
  return result;
}

/* ManalinkEh 2022: 0x21d7a50; original cards/war_of_the_spark.c:6364. */
int card_the_wanderer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);
  instance = noncombat_damage_being_prevented(event);
  if ((instance != NULL) && (instance->damage_target_player == player))
  {

    prevent_damage(affected_card_controller, affected_card, instance->info_slot);
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.power_requirement = 0x1004;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_target(&td);
      if (result == 0)
      {
        return 0;
      }
      result = count_counters(player, card, COUNTER_LOYALTY);
      if (result < 2)
      {
        return 0;
      }
      goto LAB_021d7ae7;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003",
                           "\001\014", "\001\017", "Exile target creature with power 4 or greater.",
                           result, 5, 0xfffffffe, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      new_pick_target(&td, "Select target creature with power 4 or greater.", 0, 0x81);
      goto LAB_021d7ae7;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021d7ae7;
    default_target_definition(player, card, &td, 2);
    td.power_requirement = 0x1004;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
  {
    kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
  }
LAB_021d7ae7:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x21d7cc0; original cards/war_of_the_spark.c:6443. */
int card_tibalt_rakish_instigator(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  token_generation_t token;

  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    *(unsigned int *)(&player_bits + (1 - player) * 4) =
        *(unsigned int *)(&player_bits + (1 - player) * 4) | 0x200000;
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        result = count_counters(player, card, COUNTER_LOYALTY);
        if (result < 2)
        {
          return 0;
        }
        goto LAB_021d7d23;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_021d7d23;
      instance = get_card_instance(player, card);
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003",
                           "\001\014", "\001\017", "Create a Devil", 1, 5, 0xfffffffe, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {
      default_token_definition(player, card, 0x491c, &token);
      token.special_infos = 0x42;
      generate_token(&token);
    }
  }
LAB_021d7d23:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x21d82d0; original cards/war_of_the_spark.c:6614. */
int card_tyrants_scorn(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t td1;
  target_definition_t td2;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td1, 2);
    td1.special = 0x800;
    td1.extra = 3;
    default_target_definition(player, card, &td2, 2);
    get_card_instance(player, card);
    iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td1, NULL, 1, NULL);
    if (iVar3 != 0)
    {
      return 1;
    }
    iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL);
    return iVar3;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td1, 2);
      td1.special = 0x800;
      td1.extra = 3;
      default_target_definition(player, card, &td2, 2);
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        iVar3 = check_special_flags2_2022(player, card, -0x80000000);
        if (iVar3 == 0)
        {

          iVar3 = can_target(&td2);
          iVar2 = can_target(&td1);
          iVar3 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002",
                              "Destroy target creature with CMC 3 or less", iVar2, 5,
                              "Bounce target creature", iVar3, 5, 0, 0, 0, 0);
          if (iVar3 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if (iVar3 == 1)
          {
            iVar3 = generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td1,
                                  "Select target creature with CMC 3 or less.", 1, NULL);
            return iVar3;
          }
          if (iVar3 == 2)
          {
            iVar3 =
                generic_spell(player, card, EVENT_CAST_SPELL, 1, &td2, "TARGET_CREATURE", 1, NULL);
            return iVar3;
          }
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td1, 2);
    td1.special = 0x800;
    td1.extra = 3;
    default_target_definition(player, card, &td2, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      if (instance->info_slot == 1)
      {
        iVar3 = valid_target(&td1);
        if (iVar3 != 0)
        {
          kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        }
      }
      else if ((instance->info_slot == 2) && (iVar3 = valid_target(&td2), iVar3 != 0))
      {
        bounce_permanent(instance->targets[0].player, instance->targets[0].card);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td1, 2);
    td1.special = 0x800;
    td1.extra = 3;
    default_target_definition(player, card, &td2, 2);
    get_card_instance(player, card);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int cruel_celebrant_effect(int player, int card, event_t event);
static int mayhem_devil_effect(int player, int card, event_t event);
static int liliana_dreadhorde_general_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x21c63f0; original cards/war_of_the_spark.c:1035. */
static int cruel_celebrant_effect(int player, int card, event_t event)
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
           result != 0x4d83))
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
  LAB_021c6424:
    uVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto joined_r0x021c6598;
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
      goto LAB_021c6424;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);
    if ((pcVar3 != NULL) &&
        (result = is_this_dying(affected_card_controller, affected_card), result != 0))
    {

      if ((pcVar1->damage_target_card == affected_card) &&
          (pcVar1->damage_target_player == affected_card_controller))
      {
        result = is_what((int)pcVar1->damage_target_player, pcVar1->damage_target_card, 2);
        if (result == 0)
        {
          count_for_gfp_ability(player, card, 0x77, player, 0x47, NULL);
        }
        pcVar1->damage_target_card = -1;
        pcVar1->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        if (pcVar1->targets[0xb].player < 0)
        {
          pcVar1->targets[0xb].player = 0;
        }
        *(uint8_t *)((int)&pcVar1->targets[0xb].player + 2) = 1;
      }
      count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
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

    uVar2 = uVar2 & 0xff;
    if (uVar2 != 0)
    {
      do
      {
        uVar2 = uVar2 - 1;
        lose_life(1 - player, 1);
        gain_life(player, 1);
      } while (0 < (int)uVar2);
    }
    kill_card(player, card, KILL_EXILE);
  }
joined_r0x021c6598:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x21caa00; original cards/war_of_the_spark.c:1090. */
int card_cruel_celebrant(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  unsigned int uVar3;
  int iVar4;

  iVar1 = is_humiliated_for_ability(player, card, AL_TRIG);
  if (iVar1 == 0)
  {
    if (event == EVENT_STATIC_EFFECTS)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {

        iVar1 = 0;
        if (0 < active_cards_count[player])
        {
          do
          {
            instance = in_play(player, iVar1);

            if ((((instance != NULL) && (iVar4 = is_what(player, iVar1, 0x80), iVar4 != 0)) &&
                 (instance = get_card_instance(player, iVar1),
                  instance->info_slot_fn == cruel_celebrant_effect)) &&
                (((instance->damage_target_player == player &&
                   (instance->damage_target_card == card)) ||
                  ((instance->targets[0].player == player &&
                    (instance->targets[0].card == card))))))
              goto LAB_021caa3e;
            iVar1 = iVar1 + 1;
          } while (iVar1 < active_cards_count[player]);
        }

        iVar1 = create_targetted_legacy_effect(player, card, cruel_celebrant_effect, player, card);
        instance = get_card_instance(player, iVar1);
        instance->targets[0].player = player;
        instance->targets[0].card = card;
        instance->number_of_targets = '\x01';
        add_status(player, iVar1, 0x10000);
      }
    }
    else if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {
      count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
    }
  LAB_021caa3e:
    instance = get_card_instance(player, card);

    if (((instance->kill_code == '\0') &&
         (uVar3 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY),
          uVar3 != 0)) &&
        (uVar3 = uVar3 & 0xff, uVar3 != 0))
    {
      do
      {
        lose_life(1 - player, 1);
        uVar3 = uVar3 - 1;
        gain_life(player, 1);
      } while (0 < (int)uVar3);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c6d10; original cards/war_of_the_spark.c:3641. */
static int liliana_dreadhorde_general_effect(int player, int card, event_t event)
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
           result != 0x4de5))
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
  LAB_021c6d45:
    uVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto LAB_021c6d67;
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
      goto LAB_021c6d45;

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
      count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
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

    draw_cards(player, uVar2 & 0xff);
    kill_card(player, card, KILL_EXILE);
  }
LAB_021c6d67:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x21d0e20; original cards/war_of_the_spark.c:3692. */
int card_liliana_dreadhorde_general(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  int iVar3;
  card_instance_t *pcVar4;
  unsigned int uVar5;
  test_definition_t *ptVar6;
  int iVar7;
  unsigned char bVar8;
  type_t types[5];
  char *prompts[5];
  target_definition_t td;
  test_definition_t this_test;
  test_definition_t test;

  bVar8 = 0;
  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      iVar2 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          pcVar1 = in_play(player, iVar2);

          if ((((pcVar1 != NULL) && (iVar7 = is_what(player, iVar2, 0x80), iVar7 != 0)) &&
               (pcVar1 = get_card_instance(player, iVar2),
                pcVar1->info_slot_fn == liliana_dreadhorde_general_effect)) &&
              (((pcVar1->damage_target_player == player && (pcVar1->damage_target_card == card)) ||
                ((pcVar1->targets[0].player == player && (pcVar1->targets[0].card == card))))))
            goto LAB_021d0e5c;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, liliana_dreadhorde_general_effect,
                                             player, card);
      pcVar1 = get_card_instance(player, iVar2);
      pcVar1->targets[0].player = player;
      pcVar1->targets[0].card = card;
      pcVar1->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  LAB_021d0e5c:
    pcVar1 = get_card_instance(player, card);
    if (pcVar1->kill_code == '\0')
      goto LAB_021d10d6;
  LAB_021d0e72:
    if (event == EVENT_CAN_ACTIVATE)
      goto LAB_021d1118;
  LAB_021d0e7c:
    if (event == EVENT_ACTIVATE)
      goto LAB_021d1118;
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021d0ea4;
    pcVar1 = get_card_instance(player, card);
  }
  else
  {
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_021d0e5c;
    count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
    pcVar1 = get_card_instance(player, card);
    if (pcVar1->kill_code != '\0')
      goto LAB_021d0ea4;
  LAB_021d10d6:

    uVar5 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar5 == 0)
      goto LAB_021d0e72;

    draw_cards(player, uVar5 & 0xff);
    if (event != EVENT_CAN_ACTIVATE)
      goto LAB_021d0e7c;
  LAB_021d1118:
    pcVar1 = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      pcVar1->number_of_targets = '\0';
      pcVar1->info_slot = 0;
      iVar2 = count_permanents_by_type(1 - player, TYPE_PERMANENT);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Create a Zombie",
                          1, 5, 1, "Each player sacs", 1, 5, 0xfffffffc, "Opponent chooses & sacs",
                          1, iVar2, 0xfffffff7, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_021d0ea4;
    }
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar2 = pcVar1->info_slot;
    if (iVar2 == 1)
    {
      generate_token_by_id(player, card, 0x49ac);
    }
    else if (iVar2 == 2)
    {
      new_default_test_definition(&test, 2, "Select a creature to sacrifice.");

      test.qty = 2;
      new_sacrifice(player, card, current_turn, SAC_CAUSED | SAC_NO_CANCEL, &test);
      new_sacrifice(player, card, current_turn ^ 1, SAC_CAUSED | SAC_NO_CANCEL, &test);
    }
    else if (iVar2 == 3)
    {
      ptVar6 = &test;

      for (iVar2 = 0x4b; iVar2 != 0; iVar2 = iVar2 + -1)
      {
        ptVar6->message[0] = '\0';
        ptVar6->message[1] = '\0';
        ptVar6->message[2] = '\0';
        ptVar6->message[3] = '\0';
        ptVar6 = (test_definition_t *)((int)ptVar6 + (unsigned int)bVar8 * -8 + 4);
      }
      ptVar6->message[0] = '\0';
      ptVar6->message[1] = '\0';
      iVar7 = 0;
      default_target_definition(player, card, &td, 0);
      iVar2 = 1 - player;

      td.allow_cancel = 0;
      td.illegal_abilities = 0;
      types[0] = TYPE_ARTIFACT;
      types[1] = TYPE_CREATURE;
      types[2] = TYPE_ENCHANTMENT;
      types[3] = TYPE_LAND;
      types[4] = TARGET_TYPE_PLANESWALKER;
      prompts[0] = "TARGET_ARTIFACT";
      prompts[1] = "TARGET_CREATURE";
      prompts[2] = "TARGET_ENCHANTMENT";
      prompts[3] = "TARGET_LAND";
      prompts[4] = "TARGET_PLANESWALKER";
      td.required_type = TYPE_ARTIFACT;
      td.who_chooses = iVar2;
      td.allowed_controller = iVar2;
      while (true)
      {
        iVar3 = can_target(&td);
        if ((iVar3 != 0) && (iVar3 = new_pick_target(&td, prompts[iVar7], 0, 0), iVar3 != 0))
        {
          iVar3 = pcVar1->targets[0].card;
          *(uint8_t *)((int)&test + iVar3 + iVar2 * 0x97) = 1;
          pcVar4 = get_card_instance(iVar2, iVar3);
          uVar5 = pcVar4->state;
          pcVar4->state = uVar5 | 0x200000;
          if ((player == 0) || (ai_is_speculating == 1))
          {
            pcVar4->state = uVar5 | 0x300000;
          }
        }
        iVar7 = iVar7 + 1;
        if (iVar7 == 5)
          break;
        td.required_type = types[iVar7];
      }
      td.required_type = 0x47;
      td.required_subtype = 0x1009;
      iVar7 = can_target(&td);
      if ((iVar7 != 0) &&
          (iVar7 = new_pick_target(&td, "Select a tribal card", 0, 0x80), iVar7 != 0))
      {

        iVar7 = pcVar1->targets[0].card;
        *(uint8_t *)((int)&test + iVar7 + iVar2 * 0x97) = 1;
        pcVar1 = get_card_instance(iVar2, iVar7);
        uVar5 = pcVar1->state;
        pcVar1->state = uVar5 | 0x200000;
        if ((player == 0) || (ai_is_speculating == 1))
        {
          pcVar1->state = uVar5 | 0x300000;
        }
      }
      if (0 < active_cards_count[iVar2])
      {
        iVar7 = 0;
        do
        {
          if (*(char *)((int)&test + iVar7 + iVar2 * 0x97) != '\0')
          {
            pcVar1 = get_card_instance(iVar2, iVar7);
            pcVar1->state = pcVar1->state & 0xffcfffff;
          }
          iVar7 = iVar7 + 1;
        } while (iVar7 < active_cards_count[iVar2]);
      }
      iVar7 = can_sacrifice(player, iVar2, 1, TYPE_PERMANENT, SUBTYPE_INVALID);
      if (iVar7 != 0)
      {
        default_test_definition(&this_test, 0x47);
        this_test.value_for_special_selection_function = (int)&test;
        this_test.special_selection_function = not_marked;
        new_manipulate_all(player, card, iVar2, &this_test, ACT_KILL_SACRIFICE);
      }
    }
  }
LAB_021d0ea4:
  iVar2 = planeswalker(player, card, event, 6);
  return iVar2;
}

/* ManalinkEh 2022: 0x21c69c0; original cards/war_of_the_spark.c:3950. */
static int mayhem_devil_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  unsigned int uVar3;
  int result;
  target_definition_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x4ded))
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
  LAB_021c6af0:
    uVar3 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar3 == 0)
      goto LAB_021c6b15;
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
      goto LAB_021c6af0;

    pcVar1 = get_card_instance(player, card);
    pcVar2 = in_play(affected_card_controller, affected_card);
    if (pcVar2 != NULL)
    {

      pcVar2 = get_card_instance(affected_card_controller, affected_card);
      if (pcVar2->kill_code != '\0')
      {

        if (((-1 < pcVar1->damage_target_player) &&
             (pcVar1->damage_target_card == affected_card)) &&
            (pcVar1->damage_target_player == affected_card_controller))
        {
          pcVar1->damage_target_card = -1;
          pcVar1->damage_target_player = -1;
          remove_status(player, card, 0x10000);
          if (pcVar1->targets[0xb].player < 0)
          {
            pcVar1->targets[0xb].player = 0;
          }
          *(uint8_t *)((int)&pcVar1->targets[0xb].player + 1) = 1;
          return 0;
        }
        if (pcVar2->kill_code == '\x03')
        {
          count_for_gfp_ability(player, card, 0x77, 2, 0x47, NULL);
        }
      }
    }
    uVar3 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar3 == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {

    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    for (uVar3 = uVar3 & 0xff; uVar3 != 0; uVar3 = uVar3 - 1)
    {
      result = can_target(&td);
      if ((result != 0) && (result = pick_target(&td, "TARGET_ANY"), result != 0))
      {
        damage_target0(player, card, 1);
      }
      pcVar1 = get_card_instance(player, card);
      pcVar1->number_of_targets = '\0';
    }
    kill_card(player, card, KILL_EXILE);
  }
LAB_021c6b15:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x21d1770; original cards/war_of_the_spark.c:4014. */
int card_mayhem_devil(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  unsigned int uVar3;
  int iVar4;
  target_definition_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar4 = 0;
        do
        {
          instance = in_play(player, iVar4);

          if ((((instance != NULL) && (iVar2 = is_what(player, iVar4, 0x80), iVar2 != 0)) &&
               (instance = get_card_instance(player, iVar4),
                instance->info_slot_fn == mayhem_devil_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_021d178e;
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[player]);
      }

      iVar4 = create_targetted_legacy_effect(player, card, mayhem_devil_effect, player, card);
      instance = get_card_instance(player, iVar4);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar4, 0x10000);
    }
  }
  else if ((event == EVENT_GRAVEYARD_FROM_PLAY) &&
           (instance = get_card_instance(affected_card_controller, affected_card),
            instance->kill_code == '\x03'))
  {
    count_for_gfp_ability(player, card, 0x77, 2, 0x47, NULL);
  }
LAB_021d178e:
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (uVar3 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar3 != 0))
  {

    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    for (uVar3 = uVar3 & 0xff; uVar3 != 0; uVar3 = uVar3 - 1)
    {
      iVar4 = can_target(&td);
      if ((iVar4 != 0) && (iVar4 = pick_target(&td, "TARGET_ANY"), iVar4 != 0))
      {
        damage_target0(player, card, 1);
      }
      instance = get_card_instance(player, card);
      instance->number_of_targets = '\0';
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21c8110; original cards/war_of_the_spark.c:66. */
int card_ajani_the_greathearted(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcStack_18;
  int iStack_14;

  check_legend_rule(player, card, event);
  boost_subtype(player, card, event, ~SUBTYPE_INVALID, 0, 0, 0, 8,
                BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    pcStack_18 = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      pcStack_18->number_of_targets = '\0';
      pcStack_18->info_slot = 0;
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Gain 3 life", 1,
                           5, 1, "Add counters", 1, 10, 0xfffffffe, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_021c81a2;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021c81a2;
    pcStack_18 = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (pcStack_18->info_slot == 1)
    {
      gain_life_2022(player, 3);
    }
    else
    {

      if ((pcStack_18->info_slot == 2) &&
          (iStack_14 = active_cards_count[player] + -1, -1 < iStack_14))
      {
        do
        {
          pcVar2 = in_play(player, iStack_14);
          if (pcVar2 != NULL)
          {
            result = is_what(player, iStack_14, 2);
            if (result != 0)
            {
              add_counters_from_source(player, card, player, iStack_14, COUNTER_P1_P1, 1);
            }
            result = is_planeswalker(player, iStack_14);
            if ((result != 0) && (pcStack_18->parent_card != iStack_14))
            {
              add_counters_from_source(player, card, player, iStack_14, COUNTER_LOYALTY, 1);
            }
          }
          iStack_14 = iStack_14 + -1;
        } while (iStack_14 != -1);
      }
    }
  }
LAB_021c81a2:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x21c8ed0; original cards/war_of_the_spark.c:411. */
int card_battlefield_promotion(int player, int card, event_t event)
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
        td.preferred_controller = player;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {

            instance = get_card_instance(player, card);
            pump_ability_until_eot_2022(player, card, instance->targets[0].player,
                                   instance->targets[0].card, 0, 0, 0x100, 0);
            add_counters_from_source(player, card, instance->targets[0].player,
                                     instance->targets[0].card, COUNTER_P1_P1, 1);
            gain_life(player, 2);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c8f28;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
LAB_021c8f28:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c95b0; original cards/war_of_the_spark.c:565. */
int card_bolass_citadel(int player, int card, event_t event)
{
  bool bVar1;
  const char *pcVar2;
  int iVar3;
  card_instance_t *instance;
  int iVar5;
  unsigned int uVar6;
  test_definition_t *ptVar7;
  test_definition_t *ptVar8;
  int *piVar9;
  test_definition_t *ptVar10;
  unsigned char bVar11;
  unsigned int uStack_118;
  uint32_t uStack_114;
  int sac[10];
  test_definition_t test;

  bVar11 = 0;
  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 1, "Select an nonland permanent to sacrifice.");
  LAB_021c9600:
    test.qty = 10;
    test.type_flag = 1;
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (instance = get_card_instance(player, card), instance->info_slot == 3))
    {
      lose_life_2022(1 - player, 10);
      return 0;
    }
    return 0;
  }
  new_default_test_definition(&test, 1, "Select an nonland permanent to sacrifice.");
  test.qty = 10;
  test.type_flag = 1;
  if (event == EVENT_CAN_ACTIVATE)
  {

    iVar5 = *deck_ptr[player];
    if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
    {
      if (iVar5 == -1)
        goto LAB_021c9712;
    }
    else
    {
      if (iVar5 == -1)
        goto LAB_021c9712;
      iVar3 = can_legally_play_iid_now(player, iVar5, EVENT_CAN_ACTIVATE);
      if (iVar3 < 99)
      {
        return 1;
      }
    }
    iVar3 = is_what(-1, iVar5, 1);
    if (iVar3 == 0)
    {
      iVar3 = get_cmc_by_internal_id(iVar5);
      iVar3 = can_pay_life_as_cost_for_spells_or_activated_abilities(player, iVar3);
      if (iVar3 == 0)
      {
      LAB_021c9712:
        iVar5 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        if (iVar5 == 0)
        {
          return 0;
        }
        iVar5 = new_can_sacrifice_as_cost(player, card, &test);
        return (unsigned int)(iVar5 != 0);
      }
    }
    iVar5 = can_legally_play_iid_now(player, iVar5, EVENT_CAN_ACTIVATE);
    return iVar5;
  }
  if (event != EVENT_ACTIVATE)
    goto LAB_021c9600;

  iVar5 = *deck_ptr[player];
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    uStack_114 = 0;
    if (iVar5 == -1)
    {
      uStack_118 = 0;
      goto LAB_021c9830;
    }
  }
  else
  {
    uStack_114 = 1;
    if (iVar5 == -1)
    {
      uStack_114 = 0;
      uStack_118 = 0;
      goto LAB_021c9830;
    }
  }
  iVar3 = is_what(-1, iVar5, 1);
  if (iVar3 == 0)
  {
    iVar3 = get_cmc_by_internal_id(iVar5);
    iVar3 = can_pay_life_as_cost_for_spells_or_activated_abilities(player, iVar3);
    if (iVar3 == 0)
    {
      uStack_118 = 0;
      goto LAB_021c9830;
    }
  }
  iVar3 = can_legally_play_iid_now(player, *deck_ptr[player], EVENT_ACTIVATE);
  uStack_118 = (unsigned int)(iVar3 != 0);
LAB_021c9830:
  iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL,
                                    NULL);
  uVar6 = 0;
  if (iVar3 != 0)
  {
    iVar3 = new_can_sacrifice_as_cost(player, card, &test);
    uVar6 = (unsigned int)(iVar3 != 0);
  }
  iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016",
                      "Check top card of deck", uStack_114, 0, "Play top card", uStack_118, 10,
                      "Sac 10 nonland permanents", uVar6, 10, 0, 0, 0, 0);
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
    return 0;
  }
  if (iVar3 == 2)
  {
    iVar5 = get_cmc_by_internal_id(iVar5);
    lose_life_2022(player, iVar5);
    play_card_in_deck_for_free(player, player, 0);
    spell_fizzled = 1;
    cant_be_responded_to = 1;
    return 0;
  }
  if (iVar3 != 3)
  {
    return 0;
  }
  iVar5 =
      generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  if (iVar5 == 0)
  {
    return 0;
  }

  test.qty = 1;
  piVar9 = sac;
  for (iVar5 = 10; iVar5 != 0; iVar5 = iVar5 + -1)
  {
    *piVar9 = 0;
    piVar9 = piVar9 + (unsigned int)bVar11 * -2 + 1;
  }
  ptVar10 = (test_definition_t *)sac;
  ptVar7 = ptVar10;
  do
  {
    iVar5 =
        new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
    *(int *)ptVar7->message = iVar5;
    ptVar8 = ptVar10;
    if (iVar5 == 0)
    {
      remove_state(player, card, 0x10);
      spell_fizzled = 1;
      bVar1 = false;
      goto LAB_021c9a33;
    }
    ptVar7 = (test_definition_t *)(ptVar7->message + 4);
  } while (ptVar7 != &test);
  bVar1 = true;
LAB_021c9a33:
  do
  {
    uVar6 = *(unsigned int *)ptVar8->message;
    if (uVar6 != 0)
    {
      spell_fizzled = 1;
      state_untargettable((int)uVar6 >> 0x10 & 0xff, uVar6 >> 0x18, 0);
    }
    pcVar2 = ptVar8->message;
    ptVar8 = (test_definition_t *)(pcVar2 + 4);
  } while ((test_definition_t *)(pcVar2 + 4) != &test);
  if (!bVar1)
  {
    return 0;
  }
  do
  {
    uVar6 = *(unsigned int *)ptVar10->message;
    if (uVar6 != 0)
    {
      kill_card((int)uVar6 >> 0x10 & 0xff, uVar6 >> 0x18, KILL_SACRIFICE);
    }
    ptVar10 = (test_definition_t *)(ptVar10->message + 4);
  } while (ptVar10 != &test);
  return 0;
}

/* ManalinkEh 2022: 0x21cb150; original cards/war_of_the_spark.c:1321. */
int card_domris_ambush(int player, int card, event_t event)
{
  int player_00;
  card_instance_t *instance;
  int result;
  int amt;
  target_definition_t td_control;
  target_definition_t td_dont;

  if (event == EVENT_CHECK_PUMP)
  {
    instance = get_card_instance(player, card);
    result = can_play_iid(player, 0x3b, instance->internal_card_id);
    if (result != 0)
    {
      pumpable_power[player] = pumpable_power[player] + 1;
      pumpable_toughness[player] = pumpable_toughness[player] + 1;
    }
  }
  else if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td_control, 2);
    td_control.preferred_controller = player;
    td_control.allowed_controller = player;
    default_target_definition(player, card, &td_dont, 0x102);
    td_dont.allowed_controller = 1 - player;
    td_dont.preferred_controller = td_dont.allowed_controller;
    result = can_target(&td_control);
    if (result != 0)
    {
      result = can_target(&td_dont);
      return (unsigned int)(result != 0);
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td_control, 2);
      td_control.preferred_controller = player;
      td_control.allowed_controller = player;
      default_target_definition(player, card, &td_dont, 0x102);
      td_dont.allowed_controller = 1 - player;
      td_dont.preferred_controller = td_dont.allowed_controller;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        result = new_pick_target(&td_control, "ASHNODS_BATTLEGEAR", 0, 1);
        if (result != 0)
        {
          new_pick_target(&td_dont, "Select target creature or planeswalker you don\'t control.", 1,
                          0x81);
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL || event == EVENT_CAN_CHANGE_TARGET) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td_control, 2);
    td_control.preferred_controller = player;
    td_control.allowed_controller = player;
    default_target_definition(player, card, &td_dont, 0x102);
    td_dont.allowed_controller = 1 - player;
    td_dont.preferred_controller = td_dont.allowed_controller;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = validate_target(player, card, &td_control, 0);
      if (result != 0)
      {
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 1);
        result = validate_target(player, card, &td_dont, 1);
        if (result != 0)
        {
          result = instance->targets[0].card;
          player_00 = instance->targets[0].player;
          amt = get_power(player_00, result);
          damage_creature(instance->targets[1].player, instance->targets[1].card, amt, player_00,
                          result);
        }
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21d0a00; original cards/war_of_the_spark.c:3520. */
int card_krenko_tin_street_kingpin(int player, int card, event_t event)
{
  int howmany;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  howmany = get_power(player, card);
  generate_tokens_by_id_2022(player, card, 0x493a, howmany);
  return 0;
}

/* ManalinkEh 2022: 0x21d19b0; original cards/war_of_the_spark.c:4075. */
int card_merfolk_skydiver(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 0;
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if ((result != 0) && (result = pick_target(&td, "TARGET_CREATURE"), result != 0))
    {
      add_counters_from_source(player, card, instance->targets[0].player, instance->targets[0].card,
                               COUNTER_P1_P1, 1);
    }
    instance->number_of_targets = '\0';
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_021d1a11:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_021d1a16;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_021d1a11;
  }
  proliferate(player, card);
LAB_021d1a16:
  result = generic_activated_ability(player, card, event, 0, 3, 0, 1, 1, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21d2370; original cards/war_of_the_spark.c:4328. */
int card_neoform(int player, int card, event_t event)
{
  unsigned int uVar1;
  card_instance_t *instance;
  int result;
  unsigned int player_00;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    result = generic_spell(player, card, EVENT_CAN_CAST, 0x400, NULL, NULL, 1, NULL);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (result = check_special_flags2_2022(player, card, -0x80000000), result == 0))
    {

      new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
      uVar1 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                            &test);
      if (uVar1 == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        player_00 = uVar1 >> 0x10 & 0xff;
        result = get_cmc(player_00, uVar1 >> 0x18);
        instance = get_card_instance(player, card);
        instance->info_slot_fn = (CardFunction)(result + 1);
        kill_card(player_00, uVar1 >> 0x18, KILL_SACRIFICE);
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {

    default_test_definition(&test, 2);
    instance = get_card_instance(player, card);
    test.cmc = instance->info_slot;
    instance = get_card_instance(player, card);
    scnprintf(test.message, 100, "Select a creature card with CMC %d.", instance->info_slot);
    result = new_global_tutor(player, player, 1, 4, 0, 2, &test);
    if (-1 < result)
    {
      hack_silent_counters = hack_silent_counters + 1;
      add_counters_from_source(player, card, player, result, COUNTER_P1_P1, 1);
      hack_silent_counters = hack_silent_counters + -1;
    }
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21d5390; original cards/war_of_the_spark.c:5389. */
int card_roalesk_apex_hybrid(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 0;
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.special = 0x4000;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_CREATURE");
      if (result != 0)
      {
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 2);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  proliferate(player, card);
  proliferate(player, card);
  return 0;
}

/* ManalinkEh 2022: 0x21d6710; original cards/war_of_the_spark.c:5847. */
int card_spark_double(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int iVar2;
  counter_t type;
  int iVar3;
  target_definition_t td;

  base_target_definition(player, card, &td, 0x102);
  enters_the_battlefield_as_copy_of_any(player, card, event, &td,
                                        "TARGET_CREATURE_OR_PLANESWALKER");
  if (event == EVENT_RESOLVE_SPELL)
  {

    instance = get_card_instance(player, card);
    iVar3 = instance->targets[0].player;
    instance = get_card_instance(player, card);
    card_00 = instance->targets[0].card;
    if ((iVar3 != -1) && (card_00 != -1))
    {
      hack_silent_counters = hack_silent_counters + 1;
      iVar2 = is_what(iVar3, card_00, 2);
      type = COUNTER_P1_P1;
      if (iVar2 == 0)
      {
        iVar3 = is_what(iVar3, card_00, 0x100);
        type = (-(unsigned int)(iVar3 == 0) & 0xbd) + COUNTER_LOYALTY;
      }
      add_counters_from_source(player, card, player, card, type, 1);
      hack_silent_counters = hack_silent_counters + -1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21d7790; original cards/war_of_the_spark.c:6313. */
int card_the_elderspell(int player, int card, event_t event)
{
  target_t *ptVar1;
  unsigned int uVar2;
  target_t *ptVar3;
  int iVar4;
  card_instance_t *pcVar5;
  int iVar6;
  card_instance_t *pcStack_68;
  target_definition_t td;

  if (event == EVENT_RESOLVE_GENERAL_EFFECT)
  {

    pcVar5 = get_card_instance(player, card);
    uVar2 = pcVar5->targets[10].player;
    iVar4 = (uVar2 >> 0x10) + (uVar2 & 0xffff);
    default_target_definition(player, card, &td, 0x100);
    td.allowed_controller = player;
    td.allow_cancel = 0;
    if (((iVar4 != 0) && (iVar6 = can_target(&td), iVar6 != 0)) &&
        (iVar6 = new_pick_target(&td, "TARGET_PLANESWALKER", 0, 0), iVar6 != 0))
    {
      add_counters_from_source(player, card, pcVar5->targets[0].player, pcVar5->targets[0].card,
                               COUNTER_LOYALTY, iVar4 * 2);
    }
    return 0;
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
        default_target_definition(player, card, &td, 0x100);
        td.preferred_controller = 2;
        td.allow_cancel = 3;
        if (event == EVENT_RESOLVE_SPELL)
        {

          pcVar5 = get_card_instance(player, card);
          if (pcVar5->number_of_targets != '\0')
          {
            iVar4 = 0;
            pcStack_68 = NULL;
            do
            {
              while (iVar6 = validate_target(player, card, &td, iVar4), iVar6 == 0)
              {
                iVar4 = iVar4 + 1;
                if ((int)(unsigned int)pcVar5->number_of_targets <= iVar4)
                  goto LAB_021d7962;
              }
              pcStack_68 = legacy_permanents_destroyed_this_way_accumulate(
                  player, card, pcStack_68, pcVar5->targets[iVar4].player,
                  pcVar5->targets[iVar4].card);
              ptVar3 = pcVar5->targets + iVar4;
              ptVar1 = pcVar5->targets + iVar4;
              iVar4 = iVar4 + 1;
              kill_card(ptVar1->player, ptVar3->card, KILL_DESTROY);
            } while (iVar4 < (int)(unsigned int)pcVar5->number_of_targets);
          }
        LAB_021d7962:
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021d7806;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x100);
  td.preferred_controller = 2;
  td.allow_cancel = 3;
LAB_021d7806:
  iVar4 = generic_spell(player, card, event, 0x20, &td, "TARGET_PLANESWALKER", 0xb, NULL);
  return iVar4;
}

/* ManalinkEh 2022: 0x21c7ac0; original cards/war_of_the_spark.c:6872. */
static int look_at_card(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int iid;

  instance = get_card_instance(player, card);
  iid = instance->targets[0].card;
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (ai_is_speculating == 1)
    {
      uVar2 = 0;
    }
    else
    {
      uVar2 = 1;
      if (player == 1)
      {
        uVar2 = (unsigned int)trace_mode >> 1 & 1;
      }
    }
  }
  else
  {

    uVar2 = 0;
    if (event == EVENT_ACTIVATE)
    {
      show_deck(player, &iid, 1, "Face-down card in exile", 0, ENGINE_OK_LABEL);
      spell_fizzled = 1;
    }
  }
  return uVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21c9080; original cards/war_of_the_spark.c:449. */
int card_blast_zone(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  unsigned int argument3;
  int iVar4;
  test_definition_t test;

  enters_the_battlefield_with_counters(player, card, event, COUNTER_CHARGE, 1);
  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      instance = get_card_instance(player, card);

      instance->info_slot = 0;
      iVar1 = paying_mana();
      if (iVar1 == 0)
      {

        iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 4, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "Produce mana", 1,
                            8, "Add charge counter", iVar1, 1, "Detonate", iVar2, 1, 0, 0, 0, 0);
        if (iVar1 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        instance->info_slot = iVar1;
        if (iVar1 != 1)
        {
          if (iVar1 == 2)
          {

            iVar1 = has_mana(player, 7, 1);
            argument3 = round_down_value(iVar1 - 1);
            iVar1 = choose_a_number(player, "Add how many counters?", argument3);
            if (iVar1 < 0)
            {
              spell_fizzled = 1;
            }
            else
            {
              add_state(player, card, 0x10);
              tapped_for_mana_color = -1;
            }
            iVar2 = charge_mana_for_activated_ability_2022(player, card, iVar1 * 2, 0, 0, 0, 0, 0);
            if (iVar2 == 0)
            {
              remove_state(player, card, 0x10);
              spell_fizzled = 1;
            }
            else
            {
              add_state(player, card, 0x10);
              tapped_for_mana_color = -1;
              instance->targets[9].card = iVar1;
            }
            if (ai_is_speculating == 1)
            {

              iVar2 = get_most_common_cmc_nonland(1 - player);
              iVar4 = count_counters(player, card, COUNTER_CHARGE);
              iVar1 = 1;
              if (player != 1)
              {
                iVar1 = -1;
              }
              ai_modifier =
                  ai_modifier + (((iVar4 <= iVar2) - 1 & 0xffffffe4) + 0x20) * iVar1 * iVar4;
            }
            tapped_for_mana_color = -1;
            return 0;
          }
          if (iVar1 != 3)
          {
            return 0;
          }
          add_state(player, card, 0x10);
          iVar1 = generic_activated_ability(player, card, EVENT_ACTIVATE, 8, 3, 0, 0, 0, 0, 0, 0,
                                            NULL, NULL);
          if (iVar1 != 0)
          {
            return 0;
          }
          remove_state(player, card, 0x10);
          return 0;
        }
      }
      else
      {
        instance->info_slot = 1;
      }
      iVar1 = mana_producer(player, card, EVENT_ACTIVATE);
      return iVar1;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_ACTIVATION)
      {
        iVar1 = instance->info_slot;
        if (iVar1 == 2)
        {
          add_counters_from_source(player, card, instance->parent_controller, instance->parent_card,
                                   COUNTER_CHARGE, instance->targets[9].card);
          iVar1 = instance->info_slot;
        }
        if (iVar1 == 3)
        {
          default_test_definition(&test, 1);
          test.type_flag = 1;
          test.cmc = count_counters(player, card, COUNTER_CHARGE);
          new_manipulate_all(player, card, 2, &test, ACT_KILL_DESTROY);
          return 0;
        }
      }
      return 0;
    }
  }
  iVar1 = mana_producer(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x21cabd0; original cards/war_of_the_spark.c:1167. */
int card_davriel_rogue_shadowmage(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    instance = get_card_instance(player, card);
    instance->targets[1].player = 1 - player;
  }
  instance = get_card_instance(player, card);
  if (-1 < instance->targets[1].player)
  {
    instance = get_card_instance(player, card);
    upkeep_trigger_ability(player, card, event, instance->targets[0].player);
    if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    {

      instance = get_card_instance(player, card);
      result = instance->targets[1].player;
      if (hand_count[result] < 2)
      {
        damage_player(result, 2, player, card);
      }
      goto LAB_021cac7e;
    }
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_target(&td);
      if (result == 0)
      {
        return 0;
      }
      goto LAB_021cac7e;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021cac7e;
    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_ACTIVATE)
  {

    instance->number_of_targets = '\0';
    instance->info_slot = 0;
    result = can_target(&td);
    result =
        dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003", "\001\014",
                    "\001\017", "Target player discards", result, 5, 0xffffffff, 0, 0, 0, 0);
    if (result == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    new_pick_target(&td, "TARGET_PLAYER", 0, 1);
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    discard(instance->targets[0].player, 2, player);
  }
LAB_021cac7e:
  result = planeswalker(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x21cb4d0; original cards/war_of_the_spark.c:1397. */
int card_domri_anarch_of_bolas(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  target_definition_t *ptStack_c4;
  target_definition_t *ptStack_c0;
  target_definition_t td;
  target_definition_t td1;

  check_legend_rule(player, card, event);
  boost_subtype(player, card, event, ~SUBTYPE_INVALID, 1, 0, 0, 0,
                BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  instance = get_card_instance(player, card);
  if (event == EVENT_COUNT_MANA)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (0 < instance->targets[9].card))
    {
      declare_mana_available_hex(player, 0x18, 1);
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      td.preferred_controller = player;
      default_target_definition(player, card, &td1, 2);
      td1.allowed_controller = 1 - player;
      td1.preferred_controller = td1.allowed_controller;
      if (event == EVENT_ACTIVATE)
      {

        instance->number_of_targets = '\0';
        instance->info_slot = 0;
        result = can_target(&td);
        uVar3 = 0;
        if (result != 0)
        {
          result = can_target(&td1);
          uVar3 = (unsigned int)(result != 0);
        }
        result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                             "Mana & uncounterable", 1, 5, 1, "Fight!", uVar3, 7, 0xfffffffe, 0, 0,
                             0, 0);
        if (result == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (result == 1)
        {
          produce_mana_any_combination_of_colors(player, COLOR_TEST_RED | COLOR_TEST_GREEN, 1,
                                                 NULL);
          tapped_for_mana_color = -2;
          create_legacy_effect(player, card, insist_until_eot);
        }
        else if (result == 2)
        {
          instance->number_of_targets = '\0';
          result = new_pick_target(&td, "Select target creature you control.", 0, 0x81);
          if (result != 0)
          {
            new_pick_target(&td1, "Select target creature opponent controls.", 1, 0x81);
          }
        }
        goto LAB_021cb5d3;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_021cb5d3;
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      td.preferred_controller = player;
      default_target_definition(player, card, &td1, 2);
      td1.preferred_controller = 1 - player;
    }
    ptStack_c0 = &td;
    ptStack_c4 = &td1;
    td1.allowed_controller = td1.preferred_controller;

    if ((((event == EVENT_RESOLVE_ACTIVATION) && (instance->info_slot == 2)) &&
         (result = validate_target(player, card, ptStack_c0, 0), result != 0)) &&
        (result = validate_target(player, card, ptStack_c4, 1), result != 0))
    {
      fight(instance->targets[0].player, instance->targets[0].card, instance->targets[1].player,
            instance->targets[1].card);
    }
  }
LAB_021cb5d3:
  result = planeswalker(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x21cd850; original cards/war_of_the_spark.c:2184. */
int card_gideon_blackblade(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  uint32_t uVar3;
  target_definition_t *ptStack_c0;
  target_definition_t *ptStack_bc;
  target_definition_t td1;
  target_definition_t td2;

  check_legend_rule(player, card, event);
  if ((((event == EVENT_CAN_SKIP_TURN) || (event == EVENT_RESOLVE_SPELL)) &&
       (current_turn == player)) &&
      (instance = in_play(player, card), instance != NULL))
  {
    result = prevent_all_damage_to_target(player, card, player, card, 1);
    instance = get_card_instance(player, result);
    instance->token_status = instance->token_status | 0x10000;
    animate_self(player, card, 4, 4, KEYWORD_0, SP_KEYWORD_INDESTRUCTIBLE, COLOR_TEST_0, 0);
  }
  has_subtype_if_animated_self(player, card, event, SUBTYPE_HUMAN);
  has_subtype_if_animated_self(player, card, event, SUBTYPE_SOLDIER);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td1, 2);
    td1.preferred_controller = player;
    td1.allowed_controller = player;
    td1.allow_cancel = 1;
    default_target_definition(player, card, &td2, 0x47);
    td2.illegal_type = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_target(&td1);
      if (result == 0)
      {
        result = count_counters(player, card, COUNTER_LOYALTY);
        if (result < 6)
        {
          return 0;
        }
        result = can_target(&td2);
        if (result == 0)
        {
          return 0;
        }
      }
      goto LAB_021cd960;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td2);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                           "Pump a creature you control", 1, 5, 1, "Exile target nonland permanent",
                           result, 10, 0xfffffffa, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 1)
      {
        result = can_target(&td1);
        if (result != 0)
        {
          new_pick_target(&td1, "TARGET_CREATURE_YOU_CONTROL", 0, 0);
        }
      }
      else if ((result == 2) && (result = can_target(&td2), result != 0))
      {
        new_pick_target(&td2, "TARGET_NONLAND_PERMANENT", 0, 1);
      }
      goto LAB_021cd960;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021cd960;
    default_target_definition(player, card, &td1, 2);
    td1.preferred_controller = player;
    td1.allowed_controller = player;
    td1.allow_cancel = 1;
    default_target_definition(player, card, &td2, 0x47);
    td2.illegal_type = 1;
    instance = get_card_instance(player, card);
  }
  ptStack_bc = &td2;
  ptStack_c0 = &td1;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->info_slot == 1)
    {
      result = valid_target(ptStack_c0);
      if (result != 0)
      {

        result = is_tapped(instance->targets[0].player, instance->targets[0].card);
        if (result == 0)
        {
          uVar3 = 0;
          if (current_phase < 0x15)
          {
            uVar3 = 5;
          }
        }
        else
        {
          uVar3 = 0;
        }
        result =
            dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\002", "\001\014", "Vigilance",
                        1, uVar3, "Lifelink", 5, 1, "Indestructible", 5, 1, 0, 0, 0, 0);
        if (result == 1)
        {
          result = 8;
        }
        else if (result != 2)
        {
          result = 0x800;
        }
        pump_ability_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card,
                               0, 0, 0, result);
      }
    }
    else if ((instance->info_slot == 2) && (result = valid_target(ptStack_bc), result != 0))
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
    }
  }
LAB_021cd960:
  result = planeswalker(player, card, event, 4);
  return result;
}

/* ManalinkEh 2022: 0x21cfbf0; original cards/war_of_the_spark.c:3187. */
int card_karns_bastion(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
      result = paying_mana();
      if (((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1,
                                                                4, 0, 0, 0, 0, 0, 0, NULL, NULL),
                             result != 0)) &&
          (result =
               do_dialog(player, player, card, -1, -1, " Generate mana\n Proliferate\n Cancel", 1),
           result != 0))
      {
        if (result != 1)
        {
          return 0;
        }
        add_state(player, card, 0x10);
        result = charge_mana_for_activated_ability_2022(player, card, 4, 0, 0, 0, 0, 0);
        if (result == 0)
        {
          remove_state(player, card, 0x10);
        }
        else
        {
          instance->info_slot = 1;
        }
        tapped_for_mana_color = -1;
        return 0;
      }
      event = EVENT_ACTIVATE;
    }
    else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      instance = get_card_instance(player, card);
      if ((event == EVENT_RESOLVE_ACTIVATION) && (0 < instance->info_slot))
      {
        proliferate(player, card);
      }
      return 0;
    }
  }

  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x21d3290; original cards/war_of_the_spark.c:4576. */
int card_nissa_who_shakes_the_world(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  const char *msg;
  subtype_t subtype;
  target_definition_t *ptStack_140;
  target_definition_t td_land;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  if ((((event - EVENT_COUNT_MANA & 0xfffffffd) == 0) &&
       (result = is_what(affected_card_controller, affected_card, 1), result != 0)) &&
      (affected_card_controller == player))
  {
    subtype = get_hacked_subtype(player, card, 0x5001);
    result = has_subtype(affected_card_controller, affected_card, subtype);
    if ((result != 0) && (result = is_humiliated_for_ability(player, card, AL_TRIG), result == 0))
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
        result = is_animated_and_sick(affected_card_controller, affected_card);
        if (result != 0)
        {
          return 0;
        }
        result = can_produce_mana_2022(affected_card_controller, affected_card);
        if (result == 0)
        {
          return 0;
        }
        declare_mana_available(affected_card_controller, 3, 1);
        goto LAB_021d332a;
      }
      if (-1 < tapped_for_mana_color)
      {
        produce_mana(affected_card_controller, 3, 1);
      }
    }
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td_land, 1);
    td_land.illegal_type = 2;
    td_land.preferred_controller = player;
    td_land.allowed_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Awaken", 1, 5, 1,
                           "Indestructible forests", 1, 10, 0xfffffff8, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if ((result == 1) && (result = can_target(&td_land), result != 0))
      {
        new_pick_target(&td_land, "Select target noncreature land.", 0, 0x80);
      }
      goto LAB_021d332a;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021d332a;
    default_target_definition(player, card, &td_land, 1);
    td_land.illegal_type = 2;
    td_land.preferred_controller = player;
    td_land.allowed_controller = player;
    instance = get_card_instance(player, card);
  }
  ptStack_140 = &td_land;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->info_slot == 1)
    {
      result = validate_target(player, card, ptStack_140, 0);
      if (result != 0)
      {

        add_counters_from_source(instance->parent_controller, instance->parent_card,
                                 instance->targets[0].player, instance->targets[0].card,
                                 COUNTER_P1_P1, 3);
        untap_card(instance->targets[0].player, instance->targets[0].card);
        result = animate_other(instance->parent_controller, instance->parent_card,
                               instance->targets[0].player, instance->targets[0].card, 0, 0,
                               KEYWORD_0, SP_KEYWORD_VIGILANCE | SP_KEYWORD_HASTE, COLOR_TEST_0, 1);
        instance = get_card_instance(player, result);
        instance->targets[9].card = 0x3a;
      }
    }
    else if (instance->info_slot == 2)
    {
      generate_token_by_id(player, card, 0x4dfc);
      msg = get_hacked_land_text(player, card, "Select %a card.", 0x5001);
      new_default_test_definition(&this_test, 1, msg);
      this_test.subtype = get_hacked_subtype(player, card, 0x5001);
      result = count_deck_by_type(player, 1);
      this_test.qty = (unsigned short)result;
      new_global_tutor(player, player, 1, 5, 0, 1, &this_test);
    }
  }
LAB_021d332a:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x21d3ae0; original cards/war_of_the_spark.c:4769. */
int card_oath_of_kaya(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  target_definition_t td;

  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  iVar1 = 1 - player;
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    iVar2 = can_target(&td);
    if (iVar2 != 0)
    {
      iVar2 = pick_target(&td, "TARGET_ANY");
      if (iVar2 != 0)
      {
        damage_target0(player, card, 3);
      }
    }
    gain_life(player, 3);
    store_attackers(player, card, EVENT_ETB, DAT_ATTACKS_PLANESWALKER, iVar1, -1, NULL);
    dat_ability(player, card, EVENT_ETB, DAT_ATTACKS_PLANESWALKER | DAT_RESOLVE_TRIGGER_MANDATORY,
                0);
  }
  else
  {
    store_attackers(player, card, event, DAT_ATTACKS_PLANESWALKER, iVar1, -1, NULL);
    dat_ability(player, card, event, DAT_ATTACKS_PLANESWALKER | DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    if (event == EVENT_DAT_ABILITY)
    {
      damage_player(iVar1, 2, player, card);
      gain_life(player, 2);
    }
  }
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x21d3cb0; original cards/war_of_the_spark.c:4810. */
int card_ob_nixilis_the_hate_twisted(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  check_legend_rule(player, card, event);
  result = card_drawn_trigger(player, card, event, 1 - player, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    damage_player(1 - player, 1, player, card);
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_target(&td);
      if (result == 0)
      {
        return 0;
      }
      result = count_counters(player, card, COUNTER_LOYALTY);
      if (result < 2)
      {
        return 0;
      }
      goto LAB_021d3d56;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003", "\001\014",
                      "\001\017", "Destroy a creature", result, 5, 0xfffffffe, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      goto LAB_021d3d56;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021d3d56;
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
  {
    kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    draw_cards(instance->targets[0].player, 2);
  }
LAB_021d3d56:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x21d3f40; original cards/war_of_the_spark.c:4861. */
int card_paradise_druid(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_tapped(player, card);
    if (result == 0)
    {
      hexproof_2022(player, card, EVENT_ABILITIES);
    }
  }
  result = mana_producing_creature_all_one_color(player, card, event, 5, COLOR_TEST_ANY_COLORED, 1);
  return result;
}

/* ManalinkEh 2022: 0x21d4760; original cards/war_of_the_spark.c:5112. */
int card_prismite(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_COUNT_MANA)
  {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (result = can_produce_mana_2022(player, card), result != 0)) &&
        (result = has_mana_for_activated_ability_2022(player, card, 2, 0, 0, 0, 0, 0), result != 0))
    {
      result = has_mana_for_activated_ability_2022(player, card, 4, 0, 0, 0, 0, 0);
      if (result == 0)
      {
        declare_mana_available_hex(player, 0x3e, 1);
        declare_mana_available(player, 0, -1);
      }
      else
      {
        result = has_mana_for_activated_ability_2022(player, card, 6, 0, 0, 0, 0, 0);
        if (result == 0)
        {
          declare_mana_available_hex(player, 0x3e, 2);
          declare_mana_available(player, 0, -2);
        }
        else
        {
          declare_mana_available_hex(player, 0x3e, 3);
          declare_mana_available(player, 0, -3);
        }
      }
    }
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    result = has_mana(player, 0, 2);
    if (result != 0)
    {
      result = can_produce_mana_2022(player, card);
      return result;
    }
  }
  else if ((event == EVENT_ACTIVATE) &&
           (result = charge_mana_for_activated_ability_2022(player, card, 2, 0, 0, 0, 0, 0),
            result != 0))
  {
    do
    {
      spell_fizzled = 0;
      produce_mana_all_one_color(player, COLOR_TEST_ANY_COLORED, 1);
    } while (spell_fizzled == 1);
    tapped_for_mana_color = -2;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21d7260; original cards/war_of_the_spark.c:6212. */
int card_tezzeret_master_of_the_bridge(int player, int card, event_t event)
{
  int *piVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  unsigned int uVar5;
  int *piVar6;
  unsigned char bVar7;
  int iStack_118;
  int iStack_114;
  int iids[10];
  test_definition_t test;

  bVar7 = 0;
  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    if ((affected_card_controller == player) &&
        (((iVar3 = is_what(player, affected_card, 2),
           iVar3 != 0 ||
               (iVar3 = is_what(affected_card_controller, affected_card, 0x100), iVar3 != 0)) &&
          (iVar3 = is_humiliated_for_ability(player, card, AL_STATIC), iVar3 == 0))))
    {
      iVar3 = count_subtype(player, 0x40, -1);
      COST_COLORLESS = COST_COLORLESS - (char)iVar3;
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_test_definition(&test, 0x40);
      instance = get_card_instance(player, card);
      if (event == EVENT_ACTIVATE)
      {

        instance->number_of_targets = '\0';
        instance->info_slot = 0;
        iVar3 = new_special_count_grave(player, &test);
        uVar5 = 0;
        if (0 < iVar3)
        {
          iVar3 = graveyard_has_shroud(player);
          uVar5 = (unsigned int)(iVar3 == 0);
        }
        iVar3 = dialog_impl(
            player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Damage opponent & gain life", 1,
            10, 2, "Recur an artifact from your graveyard", uVar5, 10, 0xfffffffd,
            "Put into play artifacts from your library", 1, 0x14, 0xfffffff8, 0, 0, 0, 0);
        if (iVar3 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if ((iVar3 == 2) &&
            (iVar3 = select_target_from_grave_source(player, card, player, SFG_0, 1, &test, 0),
             iVar3 == -1))
        {
          spell_fizzled = 1;
          return 0;
        }
        goto LAB_021d72d3;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_021d72d3;
      default_test_definition(&test, 0x40);
      instance = get_card_instance(player, card);
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      iVar3 = instance->info_slot;
      if (iVar3 == 1)
      {

        iVar3 = count_permanents_by_type(player, TYPE_ARTIFACT);
        damage_player(1 - player, iVar3, player, card);
        gain_life_2022(player, iVar3);
      }
      else if (iVar3 == 2)
      {

        iVar3 = validate_target_from_grave_source(player, card, player, 0);
        if (iVar3 != -1)
        {
          from_grave_to_hand(player, iVar3, 1);
        }
      }
      else
      {

        if ((iVar3 == 3) && (iVar3 = count_deck(player), iVar3 != 0))
        {
          iStack_114 = iVar3;
          if (10 < iVar3)
          {
            iStack_114 = 10;
          }

          show_deck(current_turn, deck_ptr[player], iStack_114,
                    "Cards revealed by Tezzeret, Master of the Bridge", 0, ENGINE_DONE_LABEL);
          show_deck(current_turn ^ 1, deck_ptr[player], iStack_114,
                    "Cards revealed by Tezzeret, Master of the Bridge", 0, ENGINE_DONE_LABEL);
          piVar6 = iids;
          for (iVar4 = 10; iVar4 != 0; iVar4 = iVar4 + -1)
          {
            *piVar6 = -1;
            piVar6 = piVar6 + (unsigned int)bVar7 * -2 + 1;
          }
          if (iVar3 < 1)
          {
            rfg_top_n_cards_of_deck(player, iStack_114);
          }
          else
          {
            iStack_118 = 0;
            iVar3 = 0;
            do
            {
              iVar4 = is_what(-1, deck_ptr[player][iVar3], 0x40);
              if (iVar4 != 0)
              {
                iids[iStack_118] = deck_ptr[player][iVar3];
                iStack_118 = iStack_118 + 1;
              }
              iVar3 = iVar3 + 1;
            } while (iVar3 < iStack_114);
            rfg_top_n_cards_of_deck(player, iStack_114);
            if (0 < iStack_118)
            {
              piVar6 = iids;
              piVar1 = piVar6 + iStack_118;
              do
              {

                iVar3 = find_iid_in_rfg(player, *piVar6);
                if (-1 < iVar3)
                {

                  iVar4 = add_card_to_hand(player, *piVar6);
                  remove_card_from_rfg_by_position(player, iVar3);
                  put_into_play(player, iVar4);
                }
                piVar6 = piVar6 + 1;
              } while (piVar1 != piVar6);
            }
          }
        }
      }
    }
  }
LAB_021d72d3:
  iVar3 = planeswalker(player, card, event, 5);
  return iVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21d1b30; original cards/war_of_the_spark.c:4147. */
int card_nahiri_storm_of_stone(int player, int card, event_t event)
{
  int iVar1;
  unsigned int uVar2;
  int iVar3;
  card_instance_t *instance;
  target_definition_t *ptStack_68;
  target_definition_t td;

  check_legend_rule(player, card, event);
  hybrid(player, card, event);
  if (current_turn == player)
  {
    boost_subtype(player, card, event, ~SUBTYPE_INVALID, 0, 0, 0x100, 0,
                  BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  }
  if (event == EVENT_ETB)
  {
    draw_cards(player, 1);
    goto LAB_021d1bea;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.required_state = 1;
    if (player == 1)
    {
    LAB_021d1ce8:
      td.required_state = 1;
      uVar2 = count_counters(1, card, COUNTER_LOYALTY);
      td.toughness_requirement = uVar2 | 0x2000;
    }
    ptStack_68 = &td;
    td.allow_cancel = 1;
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar1 = can_target(ptStack_68);
      if (iVar1 == 0)
      {
        return 0;
      }
      goto LAB_021d1bea;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021d1bea;
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.required_state = 1;
    if (player == 1)
      goto LAB_021d1ce8;
  }
  ptStack_68 = &td;
  td.allow_cancel = 1;
  if (event != EVENT_ACTIVATE)
  {
    if ((event == EVENT_RESOLVE_ACTIVATION) && (iVar1 = valid_target(ptStack_68), iVar1 != 0))
    {
      damage_target0(player, card, instance->targets[1].player);
    }
    goto LAB_021d1bea;
  }

  instance->number_of_targets = '\0';
  instance->info_slot = 0;
  iVar1 = can_target(ptStack_68);
  iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003", "\001\014",
                      "\001\017", "Damage a creature", iVar1, 5, 0, 0, 0, 0, 0);
  if (iVar1 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }

  iVar1 = pick_target(ptStack_68, "ROYAL_ASSASSIN");
  if (iVar1 == 0)
  {
    if (spell_fizzled == 1)
      goto LAB_021d1bea;
  }
  else
  {

    iVar1 = count_counters(player, card, COUNTER_LOYALTY);
    iVar3 = get_toughness(instance->targets[0].player, instance->targets[0].card);
    if (iVar3 < iVar1)
    {
      iVar1 = iVar3;
    }
    if (player == 0)
    {
      iVar1 = choose_a_number(0, "Pay how much Loyalty?", iVar1);
    }
    if (spell_fizzled == 1)
      goto LAB_021d1bea;
    if ((0 < iVar1) && (iVar3 = count_counters(player, card, COUNTER_LOYALTY), iVar1 <= iVar3))
    {
      remove_counters_from_source(player, card, player, card, COUNTER_LOYALTY, iVar1);
      instance->targets[1].player = iVar1;
      goto LAB_021d1bea;
    }
  }
  spell_fizzled = 1;
LAB_021d1bea:
  iVar1 = planeswalker(player, card, event, 6);
  return iVar1;
}

/* ManalinkEh 2022: 0x21c7f50; original cards/war_of_the_spark.c:2. */
void amass(int player, int card, int amount)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  target_definition_t td;
  token_generation_t token;
  enum
  {
    SUBTYPE_ARMY = 254
  };

  iVar1 = count_subtype(player, -1, 0xfe);
  if (iVar1 == 1)
  {
    iVar1 = 0;
    if (0 < active_cards_count[player])
    {
      do
      {
        instance = in_play(player, iVar1);
        if (((instance != NULL) && (iVar2 = is_what(player, iVar1, 2), iVar2 != 0)) &&
            (iVar2 = has_subtype(player, iVar1, SUBTYPE_ARMY), iVar2 != 0))
        {
          add_counters_from_source(player, card, player, iVar1, COUNTER_P1_P1, amount);
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player]);
    }
  }
  else
  {
    if (iVar1 < 2)
    {
      default_token_definition(player, card, 0x4e58, &token);
      token.action = TOKEN_ACTION_ADD_COUNTERS;
      token.action_argument = amount;
      generate_token(&token);
      return;
    }
    default_target_definition(player, card, &td, 2);
    td.required_subtype = 0xfe;
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.allow_cancel = 0;
    iVar1 = can_target(&td);
    if ((iVar1 != 0) &&
        (iVar1 = new_pick_target(&td, "Select target Army you control.", 0, 0x80), iVar1 != 0))
    {
      instance = get_card_instance(player, card);
      iVar1 = instance->targets[0].card;
      instance = get_card_instance(player, card);
      add_counters_from_source(player, card, instance->targets[0].player, iVar1, COUNTER_P1_P1,
                               amount);
    }
  }
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21c8760; original cards/war_of_the_spark.c:199. */
int card_angrath_captain_of_chaos(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  hybrid(player, card, event);
  boost_subtype(player, card, event, ~SUBTYPE_INVALID, 0, 0, 0, 0x10000,
                BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = count_counters(player, card, COUNTER_LOYALTY);
      if (result < 2)
      {
        return 0;
      }
      goto LAB_021c87fb;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003",
                           "\001\014", "\001\017", "Amass 2", 1, 5, 0xfffffffe, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_021c87fb;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021c87fb;
    get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    amass(player, card, 2);
  }
LAB_021c87fb:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x21cc520; original cards/war_of_the_spark.c:1798. */
int card_enter_the_god_eternals(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  target_definition_t td1;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    default_target_definition(player, card, &td1, 0x100);
    td1.zone = 0x1200;
    get_card_instance(player, card);
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    if (result == 0)
    {
      return 0;
    }
    result = can_target(&td1);
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
    default_target_definition(player, card, &td, 2);
    default_target_definition(player, card, &td1, 0x100);
    td1.zone = 0x1200;
    instance = get_card_instance(player, card);
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
    instance->number_of_targets = '\0';
    result = pick_target(&td, "TARGET_CREATURE");
    if (result == 0)
    {
      return 0;
    }
    new_pick_target(&td1, "TARGET_PLAYER_OR_PLANESWALKER", 1, 1);
    return 0;
  }
  if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
      (event != EVENT_CHANGE_TARGET))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  default_target_definition(player, card, &td1, 0x100);
  td1.zone = 0x1200;
  instance = get_card_instance(player, card);
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }

  result = valid_target(&td);
  if (result == 0)
  {
    result = validate_target(player, card, &td1, 1);
    if (result == 0)
      goto LAB_021cc6c6;
  LAB_021cc7c8:
    mill(instance->targets[1].player, 4);
  }
  else
  {
    damage_target0(player, card, 4);
    gain_life_2022(player, 4);
    result = validate_target(player, card, &td1, 1);
    if (result != 0)
      goto LAB_021cc7c8;
  }
  amass(player, card, 4);
LAB_021cc6c6:
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

/* ManalinkEh 2022: 0x21cdde0; original cards/war_of_the_spark.c:2314. */
int card_gleaming_overseer(int player, int card, event_t event)
{
  int result;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    if ((event == EVENT_ABILITIES) && (affected_card_controller == player))
    {
      result = has_subtype(player, affected_card, SUBTYPE_ZOMBIE);
      if (result != 0)
      {
        result = is_token(affected_card_controller, affected_card);
        if (result != 0)
        {
          hexproof_2022(affected_card_controller, affected_card, EVENT_ABILITIES);
          menace(affected_card_controller, affected_card, EVENT_ABILITIES);
        }
      }
    }
    return 0;
  }
  amass(player, card, 1);
  return 0;
}

/* ManalinkEh 2022: 0x21d0d50; original cards/war_of_the_spark.c:3606. */
int card_lazotep_reaver(int player, int card, event_t event)
{
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  amass(player, card, 1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int war_god_eternal(int player, int card, event_t event);

/* ManalinkEh 2022: 0x21c6fc0; original cards/war_of_the_spark.c:29. */
static int war_god_eternal(int player, int card, event_t event)
{
  unsigned char bVar1;
  card_instance_t *instance;
  int result;
  uint32_t iid;
  int owner;
  int position;

  leaves_play_2022(player, card, event, 1, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    instance = get_card_instance(player, card);
    bVar1 = instance->kill_code;
    if (bVar1 != 0)
    {

      if (bVar1 < 4)
      {
        result = find_in_owners_graveyard(player, card, &owner, &position);
        if (result == 0)
        {
          return 0;
        }
        result = do_dialog(player, player, card, -1, -1, " Put into library\n Pass", 0);
        if (result != 0)
        {
          return 0;
        }
        iid = graveyard_ptr_mutable[owner][position];
        remove_card_from_grave(owner, position);
      }
      else
      {
        if (bVar1 != 4)
        {
          return 0;
        }
        iid = instance->original_internal_card_id;
        owner = get_owner(player, card);
        position = find_iid_in_rfg(owner, iid);
        if (position < 0)
        {
          return 0;
        }
        result = do_dialog(player, player, card, -1, -1, " Put into library\n Pass", 0);
        if (result != 0)
        {
          return 0;
        }
        remove_card_from_rfg_by_position(owner, position);
      }
      put_iid_under_the_first_x_cards_of_library(owner, iid, 2);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21cebb0; original cards/war_of_the_spark.c:2568. */
int card_god_eternal_oketra(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  check_legend_rule(player, card, event);
  sp_ability_trigger(player, event, 2, TYPE_CREATURE, NULL, player, card);
  if (event == EVENT_SPA)
  {
    default_token_definition(player, card, 0x4e59, &token);
    token.tou = 4;
    token.pow = 4;
    token.s_key_plus = 8;
    generate_token(&token);
  }
  result = war_god_eternal(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x21cec60; original cards/war_of_the_spark.c:2592. */
int card_god_eternal_rhonas(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  check_legend_rule(player, card, event);
  deathtouch_2022(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

  if ((event == EVENT_ETB) && (0 < active_cards_count[player]))
  {
    iVar1 = 0;
    do
    {
      instance = in_play(player, iVar1);
      if (((instance != NULL) && (iVar3 = is_what(player, iVar1, 2), card != iVar1)) &&
          (iVar3 != 0))
      {
        iVar3 = get_power(player, iVar1);
        pump_ability_until_eot(player, card, player, iVar1, iVar3, 0, 0, 8);
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < active_cards_count[player]);
  }
  iVar1 = war_god_eternal(player, card, event);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static void ugins_token_processing(token_generation_t *token, int beast, int number);
static int ugin_the_ineffable_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x21c76e0; original cards/war_of_the_spark.c:6686. */
static int ugin_the_ineffable_effect(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int iid;

  instance = get_card_instance(player, card);
  other_leaves_play_2022(player, card, (int)instance->damage_target_player,
                         instance->damage_target_card, event, 1, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    result = instance->targets[9].card;
    if (result != -1)
    {

      add_card_to_hand(player, result);
    }
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (ai_is_speculating == 1)
    {
      return 0;
    }
    if ((player == 1) && (((unsigned char)trace_mode & 2) == 0))
    {
      return 0;
    }
    if (instance->targets[9].card != -1)
    {
      return 0;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = instance->targets[9].card;
    if (result != -1)
    {
      iid = result;
      show_deck(player, &iid, 1, "Card exiled by Ugin, the Ineffable", 0, ENGINE_OK_LABEL);
      spell_fizzled = 1;
    }
  }
  else if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
  result = generic_activated_ability(player, card, event, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c7140; original cards/war_of_the_spark.c:6711. */
static void ugins_token_processing(token_generation_t *token, int beast, int number)
{
  int *piVar1;
  int card;
  card_instance_t *instance;

  if (*deck_ptr[token->s_player] == -1)
  {
    return;
  }

  card = create_targetted_legacy_activate(token->s_player, token->s_card, ugin_the_ineffable_effect,
                                          token->t_player, beast);
  if (card == -1)
  {
    backtrace_and_abort("cards/war_of_the_spark.c:6714\nlegacy != -1");
  }
  piVar1 = deck_ptr[token->s_player];
  instance = get_card_instance(token->t_player, card);
  instance->targets[9].card = *piVar1;
  return;
}

/* ManalinkEh 2022: 0x21d8740; original cards/war_of_the_spark.c:6719. */
int card_ugin_the_ineffable(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  target_definition_t *ptStack_b4;
  token_generation_t token;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    uVar3 = get_color_real_or_iid(event_result, affected_card_controller, affected_card);
    if (((uVar3 & 0x3e) == 0) &&
        (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
    {
      COST_COLORLESS = COST_COLORLESS + -2;
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition(player, card, &td, 0x47);
      td.required_color = 0x3e;
      instance = get_card_instance(player, card);
      if (event == EVENT_ACTIVATE)
      {

        instance->number_of_targets = '\0';
        instance->info_slot = 0;
        result =
            dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Create a Spirit", 1,
                        5, 1, "Destroy a colored permanent", 1, 10, 0xfffffffd, 0, 0, 0, 0);
        if (result == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (result == 2)
        {
          new_pick_target(&td, "Select target permanent that\'s one or more colors.", 0, 0x81);
        }
        goto LAB_021d87c7;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_021d87c7;
      default_target_definition(player, card, &td, 0x47);
      td.required_color = 0x3e;
      instance = get_card_instance(player, card);
    }
    ptStack_b4 = &td;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      if (instance->info_slot == 1)
      {

        if (*deck_ptr[player] == -1)
        {
          default_token_definition(player, card, 0x498d, &token);
          token.tou = 2;
          token.pow = 2;
          token.color_forced = '\x01';
          token.no_sleight = '\x01';
          generate_token(&token);
        }
        else
        {
          show_deck(player, deck_ptr[player], 1, "Here\'s the top card of your deck", 0,
                    ENGINE_DONE_LABEL);
          default_token_definition(player, card, 0x498d, &token);
          token.tou = 2;
          token.pow = 2;
          token.color_forced = '\x01';
          token.no_sleight = '\x01';
          token.special_code_on_generation = ugins_token_processing;
          generate_token(&token);
          obliterate_top_card_of_deck(player);
          play_sound_effect(WAV_DESTROY);
        }
      }
      else if ((instance->info_slot == 2) && (result = valid_target(ptStack_b4), result != 0))
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
    }
  }
LAB_021d87c7:
  result = planeswalker(player, card, event, 4);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21cbdd0; original cards/war_of_the_spark.c:1588. */
int card_dreadhorde_butcher(int player, int card, event_t event)
{
  enum
  {
    DDBM_MAY_DAMAGE_PLANESWALKER = 64
  };

  int result;
  card_instance_t *instance;
  target_definition_t td;

  haste(player, card, event);
  result = damage_dealt_by_me(player, card, event,
                              DDBM_MAY_DAMAGE_PLANESWALKER | DDBM_MUST_DAMAGE_PLAYER);
  if (result != 0)
  {
    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {

    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_ANY");
      if (result != 0)
      {
        result = get_power(player, card);
        damage_target_creature_or_player(instance->targets[0].player, instance->targets[0].card,
                                         result, player, card);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21d2250; original cards/war_of_the_spark.c:4294. */
int card_neheb_dreadhorde_champion(int player, int card, event_t event)
{
  enum
  {
    DDBM_MAY_DAMAGE_PLANESWALKER = 64
  };

  int result;
  int card_00;
  int amount;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  result = damage_dealt_by_me(player, card, event,
                              DDBM_MAY_DAMAGE_PLANESWALKER | DDBM_MUST_DAMAGE_PLAYER);
  if (result == 0)
  {
    return 0;
  }

  new_default_test_definition(&this_test, 0xff, "Select a card to discard.");
  result = hand_count[player];
  amount = 0;
  this_test.zone = 0x100;
  if (0 < result)
  {
    do
    {
      card_00 = new_select_a_card(player, player, 4, 0, 3, -1, &this_test);
      if (card_00 == -1)
        break;

      amount = amount + 1;
      discard_card(player, card_00);
    } while (result != amount);
  }
  draw_cards(player, amount);
  produce_mana(player, 4, amount);
  mana_doesnt_drain_until_eot(player, COLOR_RED, amount);
  return 0;
}

/* ManalinkEh 2022: 0x21d9130; original cards/war_of_the_spark.c:7043. */
int card_vraska_swarms_eminence(int player, int card, event_t event)
{
  enum
  {
    DDBM_MAY_DAMAGE_PLANESWALKER = 64
  };

  card_instance_t *instance;
  int result;
  target_t *ptVar3;
  target_t *ptVar4;
  token_generation_t token;
  test_definition_t test;

  check_legend_rule(player, card, event);
  hybrid(player, card, event);
  default_test_definition(&test, 2);
  test.special_selection_function = has_deathtouch;
  instance = get_card_instance(player, card);
  check_damage_test(player, -1, event,
                    DDBM_TRACE_DAMAGING_SOURCES | DDBM_MAY_DAMAGE_PLANESWALKER |
                        DDBM_MUST_DAMAGE_PLAYER,
                    player, card, 0, &test);
  result = resolve_damage_trigger(player, card, event, DDBM_TRACE_DAMAGING_SOURCES, player, card);

  if (0 < result)
  {
    ptVar3 = instance->targets;
    do
    {
      ptVar4 = ptVar3 + 1;
      add_counters_from_source(player, card, ptVar3->player, ptVar3->card, COUNTER_P1_P1, 1);
      ptVar3 = ptVar4;
    } while (ptVar4 != instance->targets + result);
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = count_counters(player, card, COUNTER_LOYALTY);
    if (result < 2)
    {
      return 0;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {

    instance->number_of_targets = '\0';
    instance->info_slot = 0;
    result =
        dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003", "\001\014",
                    "\001\017", "Create an assassin", 1, 5, 0xfffffffe, 0, 0, 0, 0);
    if (result == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    default_token_definition(player, card, 0x4903, &token);
    token.s_key_plus = 0x200;
    token.special_infos = 0x43;
    generate_token(&token);
  }
  result = planeswalker(player, card, event, 5);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21cc900; original cards/war_of_the_spark.c:1896. */
int card_fblthp_the_lost(int player, int card, event_t event)
{
  const target_t *ptVar1;
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = check_special_flags_2022(player, card, 0x10000);
    draw_cards(player, (result != 0) + 1);
  }
  ptVar1 = becomes_target_of_spell(player, card, event, player, card, 2, RESOLVE_TRIGGER_MANDATORY);
  if (ptVar1 != NULL)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated_for_ability(player, card, AL_TRIG);
      if (result == 0)
      {
        shuffle_into_library(player, card);
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21cbef0; original cards/war_of_the_spark.c:1618. */
int card_dreadhorde_invasion(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  test_definition_t this_test;

  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    lose_life(player, 1);
    amass(player, card, 1);
    dat_ability(player, card, EVENT_UPKEEP_TRIGGER_ABILITY, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  }
  else if (event == EVENT_DECLARE_ATTACKERS)
  {
    default_test_definition(&this_test, 2);
    this_test.subtype = 0xde;
    this_test.type_flag = 6;
    this_test.power = 5;
    this_test.power_flag = 2;
    store_attackers(player, card, EVENT_DECLARE_ATTACKERS,
                    DAT_TRACK | DAT_RESOLVE_TRIGGER_MANDATORY, player, -1, &this_test);
    dat_ability(player, card, EVENT_DECLARE_ATTACKERS, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  }
  else
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {

      pcVar2 = get_card_instance(player, card);
      result = pcVar2->info_slot + -1;
      if (-1 < result)
      {
        do
        {
          pcVar3 =
              in_play(current_turn,
                      (unsigned int)*(unsigned char *)((int)&pcVar2->targets[2].player + result));
          if (pcVar3 != NULL)
          {
            pump_ability_until_eot_2022(
                player, card, current_turn,
                (unsigned int)*(unsigned char *)((int)&pcVar2->targets[2].player + result), 0, 0, 0,
                2);
          }
          result = result + -1;
        } while (result != -1);
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x21d5ac0; original cards/war_of_the_spark.c:5564. */
int card_sarkhan_the_masterless(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  token_generation_t token;

  check_legend_rule(player, card, event);
  iVar5 = 1 - player;
  if (event == EVENT_CHECK_PUMP)
  {
    if ((current_turn == iVar5) && (current_phase < 0x16))
    {
      iVar4 = count_subtype(player, 2, 0x30);
      pumpable_toughness[current_turn] = pumpable_toughness[current_turn] - iVar4;
    }
    store_attackers(player, card, EVENT_CHECK_PUMP, DAT_TRACK, iVar5, -1, NULL);
    dat_ability(player, card, EVENT_CHECK_PUMP, DAT_TRACK | DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  LAB_021d5d6e:
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021d5d8f;
    pcVar1 = get_card_instance(player, card);
  }
  else
  {
    store_attackers(player, card, event, DAT_TRACK, iVar5, -1, NULL);
    dat_ability(player, card, event, DAT_TRACK | DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {

      pcVar1 = get_card_instance(player, card);
      iVar5 = pcVar1->info_slot + -1;
      if (-1 < iVar5)
      {
        do
        {
          pcVar2 =
              in_play(current_turn,
                      (unsigned int)*(unsigned char *)((int)&pcVar1->targets[2].player + iVar5));

          if ((pcVar2 != NULL) && (iVar4 = active_cards_count[player] + -1, -1 < iVar4))
          {
            do
            {
              pcVar2 = in_play(player, iVar4);
              if ((pcVar2 != NULL) &&
                  ((iVar3 = is_what(player, iVar4, 2),
                    iVar3 != 0 &&
                        (iVar3 = has_subtype(player, iVar4, SUBTYPE_DRAGON), iVar3 != 0))))
              {
                damage_creature(
                    current_turn,
                    (unsigned int)*(unsigned char *)((int)&pcVar1->targets[2].player + iVar5), 1,
                    player, iVar4);
              }
              iVar4 = iVar4 + -1;
            } while (iVar4 != -1);
          }
          iVar5 = iVar5 + -1;
        } while (iVar5 != -1);
      }
      goto LAB_021d5d8f;
    }
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_021d5d6e;
    pcVar1 = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      pcVar1->number_of_targets = '\0';
      pcVar1->info_slot = 0;
      iVar5 =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "PWs become dragons", 1,
                      5, 1, "Create a dragon token", 1, 5, 0xfffffffd, 0, 0, 0, 0);
      if (iVar5 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_021d5d8f;
    }
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (pcVar1->info_slot == 1)
    {

      if (0 < active_cards_count[player])
      {
        iVar5 = 0;
        do
        {
          pcVar1 = in_play(player, iVar5);
          if ((pcVar1 != NULL) && (iVar4 = is_what(player, iVar5, 0x100), iVar4 != 0))
          {

            iVar4 = animate_other(player, card, player, iVar5, 4, 4, KEYWORD_FLYING, SP_KEYWORD_0,
                                  COLOR_TEST_RED, 0);
            pcVar1 = get_card_instance(player, iVar4);
            pcVar1->targets[9].card = 0x30;
          }
          iVar5 = iVar5 + 1;
        } while (iVar5 < active_cards_count[player]);
      }
    }
    else if (pcVar1->info_slot == 2)
    {
      default_token_definition(player, card, 0x491f, &token);
      token.tou = 4;
      token.pow = 4;
      generate_token(&token);
    }
  }
LAB_021d5d8f:
  iVar5 = planeswalker(player, card, event, 5);
  return iVar5;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21d5ef0; original cards/war_of_the_spark.c:5670. */
int card_single_combat(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  unsigned int uVar5;
  unsigned int t_player;
  bool bVar6;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_this_test;

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
      pcVar3 = get_card_instance(player, card);
      uVar5 = 0;
      do
      {
        t_player = current_turn ^ uVar5;
        iVar2 = can_sacrifice(player, t_player, 1, TARGET_TYPE_PLANESWALKER | TYPE_CREATURE,
                              SUBTYPE_INVALID);
        if (iVar2 != 0)
        {
          default_target_definition(player, card, &recovered_stack_this_test.target, 0x102);
          recovered_stack_this_test.test.message[0x1c] = '\0';
          recovered_stack_this_test.test.message[0x1d] = '\0';
          recovered_stack_this_test.test.message[0x1e] = '\0';
          recovered_stack_this_test.test.message[0x1f] = '\0';
          recovered_stack_this_test.target.who_chooses = (int32_t)(t_player);
          recovered_stack_this_test.target.allowed_controller = (int32_t)(t_player);
          iVar2 = can_target(&recovered_stack_this_test.target);
          if ((iVar2 != 0) && (iVar2 = new_pick_target(&recovered_stack_this_test.target,
                                                       "Select a creature or planeswalker to keep.",
                                                       t_player, 0x80),
                               iVar2 != 0))
          {
            state_untargettable(pcVar3->targets[t_player].player, pcVar3->targets[t_player].card,
                                1);
          }
        }
        iVar2 = target_player_cant_cast_type(player, card, t_player, 0x102);
        bVar6 = current_turn == player;
        pcVar4 = get_card_instance(player, iVar2);
        uVar5 = uVar5 + 1;
        pcVar4->targets[2].player = bVar6 + 1;
      } while (uVar5 != 2);
      default_test_definition(&recovered_stack_this_test.test, 0x102);
      new_manipulate_all(player, card, 2, &recovered_stack_this_test.test, ACT_KILL_SACRIFICE);
      iVar2 = pcVar3->targets[0].player;
      if ((iVar2 != -1) && (iVar1 = pcVar3->targets[0].card, iVar1 != -1))
      {
        state_untargettable(iVar2, iVar1, 0);
      }
      iVar2 = pcVar3->targets[1].player;
      if ((iVar2 != -1) && (iVar1 = pcVar3->targets[1].card, iVar1 != -1))
      {
        state_untargettable(iVar2, iVar1, 0);
      }
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  iVar2 = basic_spell(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x21d80a0; original cards/war_of_the_spark.c:6542. */
int card_tolsimir_friend_to_wolves(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_test;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    if ((trigger_cause_controller == player) && (trigger_cause == card))
    {
      generate_token_by_id(player, card, 0x4e4f);
    }
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if ((result != 0) || (trigger_condition != 0xdb))
      goto LAB_021d8160;
  }
  else
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result != 0)
    {
      return 0;
    }
    if (trigger_condition != 0xdb)
    {
      return 0;
    }
  }
  if ((card == affected_card) && (player == affected_card_controller))
  {
    default_test_definition(&recovered_stack_test.test, 2);
    recovered_stack_test.test.subtype = 0xd7;
    etb_ability_global(player, event, 2, TYPE_NONE, &recovered_stack_test.test, player, card);
  }
  if (event != EVENT_ETB)
  {
    return 0;
  }
LAB_021d8160:
  if (trigger_cause != card)
  {
    gain_life(player, 3);
    default_target_definition(trigger_cause_controller, trigger_cause, &recovered_stack_test.target,
                              2);
    recovered_stack_test.target.allowed_controller = (int32_t)(1 - player);
    recovered_stack_test.test.message[0x44] = '\x01';
    recovered_stack_test.test.message[0x45] = '\0';
    recovered_stack_test.test.message[0x46] = '\0';
    recovered_stack_test.test.message[0x47] = '\0';
    recovered_stack_test.target.who_chooses = (int32_t)(player);
    recovered_stack_test.target.preferred_controller =
        (int32_t)((uint32_t)(recovered_stack_test.target.allowed_controller));
    result = can_target(&recovered_stack_test.target);
    if ((result != 0) &&
        (result = new_pick_target(&recovered_stack_test.target, "TARGET_CREATURE", 0, 0),
         result != 0))
    {
      instance = get_card_instance(trigger_cause_controller, trigger_cause);
      fight(trigger_cause_controller, trigger_cause, instance->targets[0].player,
            instance->targets[0].card);
      instance->number_of_targets = '\0';
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21cded0; original cards/war_of_the_spark.c:2348. */
int card_god_eternal_bontu(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  int iStack_ec;
  test_definition_t test;
  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test, 0x47, "");
    test.not_me = 1;
    instance = get_card_instance(player, card);
    iStack_ec = 0;
    instance->state = instance->state | STATE_OUBLIETTED;
    while (true)
    {
      result = new_sacrifice(player, card, player, SAC_DONE, &test);
      if (result == 0)
        break;
      iStack_ec = iStack_ec + 1;
    }
    instance->state = instance->state & 0xff7fffff;
    draw_cards(player, iStack_ec);
  }
  result = war_god_eternal(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21d6150; original cards/war_of_the_spark.c:5736. */
int card_sorin_vengeful_bloodlord(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  unsigned int uVar5;
  target_definition_t *ptStack_140;
  target_definition_t td;
  test_definition_t test_resurrect;

  check_legend_rule(player, card, event);
  lifelink(player, card, event);
  if (event == EVENT_ABILITIES)
  {
    if (((((current_turn == player) && (instance = in_play(player, card), instance != NULL)) &&
          (affected_card_controller == player)) &&
         (instance = in_play(player, affected_card), instance != NULL)) &&
        (((iVar1 = is_what(affected_card_controller, affected_card, 2),
           iVar1 != 0 ||
               (iVar1 = is_planeswalker(affected_card_controller, affected_card), iVar1 != 0)) &&
          ((iVar1 = is_humiliated_for_ability(player, card, AL_STATIC),
            iVar1 == 0 &&
                (iVar1 = is_humiliated_for_ability(affected_card_controller, affected_card, AL_KEY),
                 iVar1 == 0))))))
    {

      instance = get_card_instance(affected_card_controller, affected_card);
      instance->ext->sp_keywords = instance->ext->sp_keywords | 2;
    }
    goto LAB_021d6223;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x100);
    td.zone = 0x1200;
    default_test_definition(&test_resurrect, 2);
    iVar1 = count_counters(player, card, COUNTER_LOYALTY);
    test_resurrect.cmc = iVar1 + 1;
    test_resurrect.cmc_flag = 3;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      iVar1 = new_special_count_grave(player, &test_resurrect);
      uVar5 = 0;
      if (0 < iVar1)
      {
        iVar1 = graveyard_has_shroud(player);
        uVar5 = (unsigned int)(iVar1 == 0);
      }
      iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Deal 1 damage", 1,
                          5, 2, "Reanimate", uVar5, 10, 0, 0, 0, 0, 0);
      if (iVar1 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar1 == 1)
      {
        pick_target(&td, "TARGET_PLAYER_OR_PLANESWALKER");
        goto LAB_021d6223;
      }
      if (iVar1 != 2)
        goto LAB_021d6223;

      if (player == 1)
      {

        ptStack_140 = NULL;
        iVar1 = 0;
        for (iVar3 = 0; iVar4 = count_counters(1, card, COUNTER_LOYALTY), iVar3 <= iVar4;
             iVar3 = iVar3 + 1)
        {

          test_resurrect.cmc = iVar3;
          iVar4 = new_special_count_grave(1, &test_resurrect);
          if (iVar1 < iVar4)
          {
            iVar1 = iVar4;
            ptStack_140 = (target_definition_t *)iVar3;
          }
        }
      LAB_021d6674:
        iVar1 = (int)ptStack_140 + 1;
        if (-1 < (int)ptStack_140)
          goto LAB_021d63cb;
      LAB_021d63fd:
        spell_fizzled = 1;
      }
      else
      {
        if (player == 0)
        {
          iVar1 = count_counters(0, card, COUNTER_LOYALTY);
          ptStack_140 =
              (target_definition_t *)choose_a_number(0, "Remove how much Loyalty?", iVar1);
          goto LAB_021d6674;
        }
        iVar1 = 1;
        ptStack_140 = NULL;
      LAB_021d63cb:
        iVar3 = count_counters(player, card, COUNTER_LOYALTY);
        if (iVar3 < (int)ptStack_140)
          goto LAB_021d63fd;
        remove_counters_from_source(player, card, player, card, COUNTER_LOYALTY, (int)ptStack_140);
        instance->targets[1].player = (int32_t)ptStack_140;
      }
      test_resurrect.cmc = iVar1;
      select_target_from_grave_source(player, card, player, SFG_0, 1, &test_resurrect, 0);
      goto LAB_021d6223;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021d6223;
    default_target_definition(player, card, &td, 0x100);
    td.zone = 0x1200;
    default_test_definition(&test_resurrect, 2);
    iVar1 = count_counters(player, card, COUNTER_LOYALTY);
    test_resurrect.cmc = iVar1 + 1;
    test_resurrect.cmc_flag = 3;
    instance = get_card_instance(player, card);
  }
  ptStack_140 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->info_slot == 1)
    {
      iVar1 = valid_target(ptStack_140);
      if (iVar1 != 0)
      {
        damage_target0(player, card, 1);
      }
    }
    else
    {

      if ((instance->info_slot == 2) &&
          (iVar1 = validate_target_from_grave_source(player, card, player, 0), -1 < iVar1))
      {

        iVar1 = reanimate_permanent(player, card, player, iVar1, REANIMATE_DEFAULT);
        add_a_subtype_permanently(player, card, player, iVar1, SUBTYPE_VAMPIRE);
      }
    }
  }
LAB_021d6223:
  iVar1 = planeswalker(player, card, event, 4);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21d0bf0; original cards/war_of_the_spark.c:3581. */
int card_lazotep_plating(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
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
      amass(player, card, 1);
      if (0 < active_cards_count[player])
      {
        iVar1 = 0;
        do
        {
          while ((instance = in_play(player, iVar1),
                  instance == NULL || (iVar3 = is_what(player, iVar1, 0x47), iVar3 == 0)))
          {
            iVar1 = iVar1 + 1;
            if (active_cards_count[player] <= iVar1)
              goto LAB_021d0d18;
          }
          iVar3 = iVar1 + 1;
          pump_ability_until_eot(player, card, player, iVar1, 0, 0, 0, (SP_KEYWORD_HEXPROOF));
          iVar1 = iVar3;
        } while (iVar3 < active_cards_count[player]);
      }
    LAB_021d0d18:
      create_legacy_effect(player, card, lazotep_plating_effect);
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

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21c8380; original cards/war_of_the_spark.c:128. */
int card_angraths_rampage(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  type_t type;
  const char *pcVar3;
  target_definition_t td;
  char typetext[100];
  char prompt[200];
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    get_card_instance(player, card);
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_PLAYER", 1, NULL);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.zone = 0x1000;
      get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        result = check_special_flags2_2022(player, card, -0x80000000);

        if ((result == 0) &&
            (result =
                 dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002",
                             "Target sacrifices an artifact", 1, 5, "Target sacrifices a creature",
                             1, 5, "Target sacrifices a planeswalker", 1, 5, 0, 0, 0, 0),
             result == 0))
        {
          spell_fizzled = 1;
        }
        else
        {
          generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_PLAYER", 1, NULL);
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {

        if (instance->info_slot == 1)
        {
          result = 0x40;
          type = TYPE_ARTIFACT;
        }
        else if (instance->info_slot == 2)
        {
          type = TYPE_CREATURE;
          result = 2;
        }
        else
        {
          result = 0x100;
          type = TARGET_TYPE_PLANESWALKER;
        }
        if (ai_is_speculating == 1)
        {
          prompt[0] = '\0';
        }
        else
        {
          pcVar3 = type_text(typetext, 100, type, TYPETEXT_0);
          scnprintf(prompt, 200, "Select a %s to_sacrifice.", pcVar3);
        }
        new_default_test_definition(&test, result, prompt);
        new_sacrifice(player, card, instance->targets[0].player, SAC_CAUSED | SAC_NO_CANCEL, &test);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    get_card_instance(player, card);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int effect_yanggu_mana(int player, int card, event_t event);

/* ManalinkEh 2022: 0x21ced60; original cards/war_of_the_spark.c:2637. */
int card_guild_globe(int player, int card, event_t event)
{
  int result;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_a_card(player);
  }
  else if (event == EVENT_COUNT_MANA)
  {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (result = mana_producer(player, card, EVENT_CAN_ACTIVATE), result != 0)) &&
        ((result = can_sacrifice_this_as_cost(player, card),
          result != 0 && (result = has_mana(player, 7, 2), result != 0))))
    {
      declare_mana_available_any_combination_of_colors(player, COLOR_TEST_ANY_COLORED, 2);
    }
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
    if (((result != 0) && (result = can_sacrifice_this_as_cost(player, card), result != 0)) &&
        (result = has_mana(player, 7, 2), result != 0))
    {
      return 1;
    }
  }
  else if ((event == EVENT_ACTIVATE) &&
           (result = charge_mana_for_activated_ability_2022(player, card, 2, 0, 0, 0, 0, 0),
            result != 0))
  {
    produce_mana_tapped_all_different_colors_2022(player, card, COLOR_TEST_ANY_COLORED, 2, NULL);
    kill_card(player, card, KILL_SACRIFICE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c71e0; original cards/war_of_the_spark.c:3096. */
static int effect_yanggu_mana(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.allowed_controller = player;
  td.allow_cancel = 2;
  td.illegal_state = 0x1001;
  td.illegal_abilities = 0;
  td.special = 0x8000;
  td.extra = (int32_t)has_a_counter_and_not_mana_src;
  if (event == EVENT_COUNT_MANA)
  {

    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
        (result = target_available(player, card, &td), 0 < result))
    {
      declare_mana_available_hex(player, 0x3e, result);
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        result = is_humiliated_for_ability((int)instance->damage_target_player,
                                           instance->damage_target_card, AL_STATIC);
        if (result != 0)
        {
          return 0;
        }
        result = can_target(&td);
        return result;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        return 0;
      }
      instance = get_card_instance(player, card);
    }
    if (event == EVENT_ACTIVATE)
    {
      result = pick_target(&td, "TARGET_CREATURE");
      if (result != 0)
      {
        instance->number_of_targets = '\x01';
        produce_mana_tapped_any_combination_of_colors_impl(instance->targets[0].player,
                                                           instance->targets[0].card,
                                                           COLOR_TEST_ANY_COLORED, 1, NULL, 0);
      }
      spell_fizzled = 1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21cf9b0; original cards/war_of_the_spark.c:3134. */
int card_jiang_yanggu_wildcrafter(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    create_targetted_legacy_activate(player, card, effect_yanggu_mana, player, card);
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition(player, card, &td, 2);
      td.allow_cancel = 1;
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        result = can_target(&td);
        if (result == 0)
        {
          return 0;
        }
        goto LAB_021cfa35;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_021cfa35;
      default_target_definition(player, card, &td, 2);
      td.allow_cancel = 1;
      instance = get_card_instance(player, card);
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\014",
                           "\001\017", "Add a +1/+1 counter", result, 5, 0xffffffff, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      new_pick_target(&td, "TARGET_CREATURE", 0, 1);
    }
    else if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      add_counters_from_source(player, card, instance->targets[0].player, instance->targets[0].card,
                               COUNTER_P1_P1, 1);
    }
  }
LAB_021cfa35:
  result = planeswalker(player, card, event, 3);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21d1ee0; original cards/war_of_the_spark.c:4222. */
int card_narsets_reversal(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_RESOLVE_SPELL)
  {
    result = counterspell_validate(player, card, NULL, 0);
    if (result != 0)
    {

      instance = get_card_instance(player, card);
      result =
          pre_counterspell(player, card, instance->targets[0].player, instance->targets[0].card);
      if (result == 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
      }
      else
      {
        copy_spell_from_stack_2022(player, instance->targets[0].player, instance->targets[0].card);
        bounce_permanent(instance->targets[0].player, instance->targets[0].card);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 2, NULL, NULL, 1, NULL);
  return result;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21d0180; original cards/war_of_the_spark.c:3311. */
int card_kasminas_transmutation(int player, int card, event_t event)
{
  int result;

  attached_set_pt(player, card, event, 1, 1);
  disable_attached(player, card, event, 0x7e);
  result = aura(player, card, event, NULL, NULL);
  disabling_aura_ai(player, card, event);
  return result;
}
#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int effect_prevent_damage_to_or_from_until_my_next_turn(int player, int card, event_t event);

/* ManalinkEh 2022: 0x21c6910; original cards/war_of_the_spark.c:1484. */
static int effect_prevent_damage_to_or_from_until_my_next_turn(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;

  pcVar1 = damage_being_prevented(event);
  if (((pcVar1 != NULL) && (pcVar2 = get_card_instance(player, card), pcVar2 != NULL)) &&
      (((pcVar1->damage_source_card == pcVar2->damage_target_card &&
         (pcVar1->damage_source_player == pcVar2->damage_target_player)) ||
        ((pcVar2->damage_target_card == pcVar1->damage_target_card &&
          (pcVar1->damage_target_player == pcVar2->damage_target_player))))))
  {

    prevent_damage(affected_card_controller, affected_card, pcVar1->info_slot);
  }
  if ((event == EVENT_BEGIN_TURN) && (current_turn == player))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x21cb920; original cards/war_of_the_spark.c:1499. */
int card_dovin_hand_of_control(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_2022_t td;

  check_legend_rule(player, card, event);
  hybrid(player, card, event);
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    instance = in_play(player, card);
    if ((((instance != NULL) &&
          (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
         (1 - player == affected_card_controller)) &&
        ((result = is_what(1 - player, affected_card, 0x78),
          result != 0 &&
              (result = is_what(affected_card_controller, affected_card, 1), result == 0))))
    {
      COST_COLORLESS = COST_COLORLESS + '\x01';
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition_2022(player, card, &td, 0x47);
      td.allowed_controller = 1 - player;
      td.allow_cancel = 1;
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        result = can_target_2022(&td);
        if (result == 0)
        {
          return 0;
        }
        goto LAB_021cb9ca;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_021cb9ca;
      default_target_definition_2022(player, card, &td, 0x47);
      td.allowed_controller = 1 - player;
      td.allow_cancel = 1;
      instance = get_card_instance(player, card);
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target_2022(&td);
      result =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003", "\001\014",
                      "\001\017", "Prevent damage", result, 5, 0xffffffff, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      new_pick_target_2022(&td, "Select target permanent an opponent controls.", 0, 0x81);
    }
    else if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target_2022(&td), result != 0))
    {
      result = create_targetted_legacy_effect(
          player, card, effect_prevent_damage_to_or_from_until_my_next_turn,
          instance->targets[0].player, instance->targets[0].card);
      alternate_legacy_text(1, instance->parent_controller, result);
      if (((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))) &&
          (result = is_what(instance->targets[0].player, instance->targets[0].card, 2),
           result != 0))
      {
        result = get_power(instance->targets[0].player, instance->targets[0].card);
        ai_modifier = ai_modifier + result * 0xc;
      }
    }
  }
LAB_021cb9ca:
  result = planeswalker(player, card, event, 5);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x21c7e40; original cards/war_of_the_spark.c:2806. */
int bounce_at_eot(int player, int card, event_t event)
{
  char cVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_player)
  {
    instance = get_card_instance(player, card);
    if ((((trigger_condition == 0xcd) && (card == affected_card)) &&
         (player == affected_card_controller)) &&
        (player == reason_for_trigger_controller))
    {
      iVar2 = instance->targets[1].player;
      if (iVar2 < 1)
      {
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
        }
        else if (event == EVENT_RESOLVE_TRIGGER)
        {
          cVar1 = instance->damage_target_player;
          iVar2 = instance->damage_target_card;
          iVar4 = check_state((int)cVar1, iVar2, STATE_OUBLIETTED);
          if (iVar4 == 0)
          {
            bounce_permanent((int)cVar1, iVar2);
          }
          else
          {
            kill_card(player, card, KILL_EXILE);
          }
        }
      }
      else
      {
        instance->targets[1].player = iVar2 + -1;
      }
    }
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21cf300; original cards/war_of_the_spark.c:2835. */
int card_ilharg_the_raze_boar(int player, int card, event_t event)
{
  int result;
  int card_00;
  card_instance_t *instance;
  test_definition_2022_t test;

  result = duh_mode(player);
  dat_ability(player, card, event, (result != 0) + DAT_RESOLVE_TRIGGER_OPTIONAL, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    new_default_test_definition_2022(&test, 2, "Select a creature card");
    test.zone = 0x100;
    result = check_battlefield_for_special_card_2022(player, card, player, 0, &test);
    if (result != 0)
    {

      result = new_select_a_card_2022(player, player, 4, 0, 2, -1, &test);
      if (result != 0)
      {

        add_state(player, result, 0x14);
        card_00 = create_targetted_legacy_effect(player, card, bounce_at_eot, player, result);
        if ((current_phase == 0x1f) && (-1 < card_00))
        {
          instance = get_card_instance(player, card_00);
          instance->targets[1].player = 1;
        }
        put_into_play(player, result);
      }
    }
  }
  result = war_god_eternal(player, card, event);
  return result;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21cfde0; original cards/war_of_the_spark.c:3231. */
int card_karn_the_great_creator(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;
  target_definition_2022_t td;
  test_definition_2022_t test;

  check_legend_rule(player, card, event);
  pcVar1 = in_play(player, card);
  if ((pcVar1 != NULL) &&
      (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
  {
    default_test_definition_2022(&test, 0x40);
    remove_abilities_by_test_2022(1 - player, event, &test, AL_ALL_ACT);
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition_2022(player, card, &td, 0x40);
    td.preferred_controller = player;
    td.illegal_type = 2;
    pcVar1 = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      pcVar1->number_of_targets = '\0';
      pcVar1->info_slot = 0;
      result =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Animate an artifact",
                      1, 5, 1, "Wish for an artifact", 1, 10, 0xfffffffe, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if ((result == 1) && (result = can_target_2022(&td), result != 0))
      {
        new_pick_target_2022(&td, "Select target noncreature artifact.", 0, 0x80);
      }
      goto LAB_021cfe8e;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021cfe8e;
    default_target_definition_2022(player, card, &td, 0x40);
    td.preferred_controller = player;
    td.illegal_type = 2;
    pcVar1 = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (pcVar1->info_slot == 1)
    {

      result = get_cmc(pcVar1->targets[0].player, pcVar1->targets[0].card);
      result =
          turn_into_creature(pcVar1->parent_controller, pcVar1->parent_card,
                             pcVar1->targets[0].player, pcVar1->targets[0].card, 8, result, result);
      pcVar3 = get_card_instance(player, result);
      pcVar3->targets[4].player = pcVar1->parent_controller;
    }
    else if (pcVar1->info_slot == 2)
    {

      result = count_rfg_by_type(player, 0x40);
      if (result != 0)
      {
        default_test_definition_2022(&test, 0x40);
        result = new_global_tutor_2022(player, player, 3, 1, 0, 1, &test);
        if (-1 < result)
          goto LAB_021cfe8e;
      }
      default_test_definition_2022(&test, 0x40);
      generic_wish_effect_2022(player, card, &test, 0x1ac2);
    }
  }
LAB_021cfe8e:
  result = planeswalker(player, card, event, 5);
  return result;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 native scalar-word handlers. */
int card_narset_parter_of_veils(int player, int card, event_t event);
int card_teyo_the_shieldmage(int player, int card, event_t event);

/* ManalinkEh 2022: 0x21d1ff0; original cards/war_of_the_spark.c:4246. */
int card_narset_parter_of_veils(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_2022_t test;

  check_legend_rule(player, card, event);
  result = -player + 1;
  if ((((cards_drawn_this_turn[result] != 0) && (trigger_condition == TRIGGER_REPLACE_CARD_DRAW)) &&
       (card == affected_card)) &&
      ((player == affected_card_controller && (result == reason_for_trigger_controller))))
  {
    suppress_draw = 1;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    new_default_test_definition_2022(&test, 3, "Select a nonland, noncreature card.");
    test.type_flag = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = count_counters(player, card, COUNTER_LOYALTY);
      if (result < 2)
      {
        return 0;
      }
      goto LAB_021d2090;
    }
    if (event == EVENT_ACTIVATE)
    {
      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003",
                           "\001\014", "\001\017", "Look at top 4", 1, 5, 0xfffffffe, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_021d2090;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021d2090;
    new_default_test_definition_2022(&test, 3, "Select a nonland, noncreature card.");
    test.type_flag = 1;
    get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    reveal_top_cards_of_library_and_choose_2022(player, card, player, 4, 0, TUTOR2022_HAND, 1,
                                                TUTOR2022_BOTTOM_OF_DECK_RANDOM, 0, &test);
  }
LAB_021d2090:
  result = planeswalker(player, card, event, 5);
  return result;
}


/* ManalinkEh 2022: 0x21d7080; original cards/war_of_the_spark.c:6163. */
int card_teyo_the_shieldmage(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  token_generation_2022_t token;

  check_legend_rule(player, card, event);
  instance = in_play(player, card);
  if ((instance != NULL) &&
      (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
  {
    give_hexproof_to_player(player, card, event);
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = count_counters(player, card, COUNTER_LOYALTY);
      if (result < 2)
      {
        return 0;
      }
      goto LAB_021d70fa;
    }
    if (event == EVENT_ACTIVATE)
    {
      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\003",
                           "\001\014", "\001\017", "Create a wall", 1, 5, 0xfffffffe, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_021d70fa;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_021d70fa;
    get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    default_token_definition_2022(player, card, 0x49a3, &token);
    token.pow = 0;
    token.tou = 3;
    token.color_forced = ' ';
    token.key_plus = KEYWORD_DEFENDER;
    generate_token_2022(&token);
  }
LAB_021d70fa:
  result = planeswalker(player, card, event, 5);
  return result;
}

#endif
