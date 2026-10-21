#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void true_transform_to_csvid(int player, int card, int csvid);

/* ManalinkEh 2022: 0x25c98a0; original cards/throne_of_eldraine.c:5. */
void true_transform_to_csvid(int player, int card, int csvid)
{
  card_instance_t *instance;
  uint32_t uVar2;
  uint32_t uVar3;
  int csvid_00;

  instance = get_card_instance(player, card);
  csvid_00 = instance->targets[0xd].player;
  if (instance->targets[0xd].card == csvid_00)
  {
    csvid_00 = csvid;
  }
  uVar3 = instance->original_internal_card_id;
  uVar2 = get_internal_card_id_from_csv_id(csvid);
  if (uVar2 != uVar3)
  {
    uVar3 = get_internal_card_id_from_csv_id(csvid_00);
  }
  instance->targets[0xd].card = csvid_00;
  instance->targets[0xc].card = uVar3;
  instance->internal_card_id = uVar3;
  instance->backup_internal_card_id = (uint16_t)uVar3;
  return;
}

/* ManalinkEh 2022: 0x25c9910; original cards/throne_of_eldraine.c:20. */
void adventure_transform(int player, int card, int csvid)
{
  int result;
  card_instance_t *instance;

  true_transform_to_csvid(player, card, csvid);
  result = check_special_flags3_2022(player, card, 0x200);
  if (result == 0)
  {
    set_special_flags3_2022(player, card, 0x200);
  }
  else
  {
    remove_special_flags3_2022(player, card, 0x200);
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

/* ManalinkEh 2022: 0x25cb2a0; original cards/throne_of_eldraine.c:497. */
int card_bake_into_a_pie(int player, int card, event_t event)
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
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            generate_token_by_id(player, card, 0x5077);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025cb2f5;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
LAB_025cb2f5:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25cb780; original cards/throne_of_eldraine.c:619. */
int card_blacklance_paragon(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 2);
    td.required_subtype = 0x67;
    td.preferred_controller = player;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Select target Knight", 0, 0x81);
      if (result != 0)
      {
        pump_ability_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card,
                               0, 0, 0, 0x202);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = flash(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25c9860; original cards/throne_of_eldraine.c:1058. */
static char *another_you_own(int who_chooses, int player, int card, int targeting_player,
                             int targeting_card)
{
  char *pcVar1;
  int result;
  if ((player != targeting_player) || (pcVar1 = "cannot target self", card != targeting_card))
  {
    result = get_owner(player, card);
    pcVar1 = "owner";
    if (targeting_player == result)
    {
      return NULL;
    }
  }
  return pcVar1;
}

/* ManalinkEh 2022: 0x25ccdb0; original cards/throne_of_eldraine.c:1160. */
int card_claim_the_firstborn(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        default_target_definition(player, card, &td, 2);
        td.preferred_controller = 1 - player;
        td.extra = 3;
        td.special = 0x800;
        get_card_instance(player, card);
        if ((card != affected_card) || (player != affected_card_controller))
          goto LAB_025cce32;
        generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                      "Select target creature with mana value 3 or less", 1, NULL);
        if (spell_fizzled != 1)
        {
          ai_modifier_for_act_of_treason(player, card);
        }
      }
      return 0;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = 1 - player;
      td.extra = 3;
      td.special = 0x800;
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = valid_target(&td);
        if (result != 0)
        {
          effect_act_of_treason(player, card, instance->targets[0].player,
                                instance->targets[0].card);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_025cce32;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = 1 - player;
  td.extra = 3;
  td.special = 0x800;
  get_card_instance(player, card);
LAB_025cce32:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25cd600; original cards/throne_of_eldraine.c:1377. */
int card_didnt_say_please(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = counterspell_validate(player, card, NULL, 0);
    if (result != 0)
    {
      instance = get_card_instance(player, card);
      real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
      mill(instance->targets[0].player, 3);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 2, NULL, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25c90a0; original cards/throne_of_eldraine.c:1438. */
static char *ditl_graveyard_count(int who_chooses, int player, int card, int targeting_player,
                                  int targeting_card)
{
  int iVar1;
  int iVar2;
  char *pcVar3;
  iVar1 = count_graveyard(player);
  iVar2 = get_cmc(player, card);
  pcVar3 = NULL;
  if (iVar1 < iVar2)
  {
    pcVar3 = "mana value must be less than or equal to the number of cards in its controller\'s "
             "graveyard";
  }
  return pcVar3;
}

/* ManalinkEh 2022: 0x25cdfd0; original cards/throne_of_eldraine.c:1616. */
int card_emry_lurker_of_the_loch(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;
  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    mill(player, 4);
  }
  else
  {
    if (event == EVENT_MODIFY_COST)
    {
      result = count_permanents_by_type(player, TYPE_ARTIFACT);
      COST_COLORLESS = COST_COLORLESS - (char)result;
      return 0;
    }
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      goto LAB_025ce03d;
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
LAB_025ce03d:
  new_default_test_definition(&test, 0x40, "Select target artifact card.");
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    result = validate_target_from_grave_source(player, card, player, 0);
    if (result != -1)
    {
      create_spell_has_flashback_legacy(player, card, result, FBL_PLAYING_PERMANENT_CARD);
    }
    instance->number_of_targets = '\0';
  }
  result = generic_activated_ability_with_target_in_grave(
      player, card, event, GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, 8);
  return result;
}

/* ManalinkEh 2022: 0x25ce1b0; original cards/throne_of_eldraine.c:1659. */
int card_epic_downfall(int player, int card, event_t event)
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
        td.extra = 3;
        td.special = 0x1000000;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            instance = get_card_instance(player, card);
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025ce213;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.extra = 3;
  td.special = 0x1000000;
LAB_025ce213:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target creature with CMC 3 or greater.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25ce310; original cards/throne_of_eldraine.c:1686. */
int card_escape_to_the_wilds(int player, int card, event_t event)
{
  unsigned short uVar1;
  int result;
  card_instance_t *instance;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = 5;
    do
    {
      if (*deck_ptr[player] != -1)
      {
        uVar1 = cards_data[*deck_ptr[player]].id;
        rfg_top_card_of_deck(player);
        create_may_play_card_from_exile_effect(player, card, player, (unsigned int)uVar1,
                                               MPCFE_UNTIL_END_OF_YOUR_NEXT_TURN);
      }
      result = result + -1;
    } while (result != 0);
    result = create_legacy_effect(player, card, check_playable_lands_legacy);
    instance = get_card_instance(player, result);
    result = get_id(player, card);
    instance->targets[2].card = result;
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25cf780; original cards/throne_of_eldraine.c:2229. */
int garruk_cursed_huntsman_emblem(int player, int card, event_t event)
{
  int result;
  if (event - EVENT_POWER < 2)
  {
    if (affected_card_controller == player)
    {
      event_result = event_result + 3;
    }
  }
  else if (event == EVENT_ABILITIES)
  {
    result = is_what(affected_card_controller, affected_card, 2);
    if ((result != 0) && (player == affected_card_controller))
    {
      event_result = event_result | 0x80;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25cfac0; original cards/throne_of_eldraine.c:2320. */
int card_giant_opportunity(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
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
      new_default_test_definition(&test, 0x47, "Select a food to sacrifice.");
      test.subtype = 0x3008;
      test.qty = 2;
      result = new_sacrifice(player, card, player, SAC_ALL_OR_NONE | SAC_DONE, &test);
      if (result == 0)
      {
        generate_tokens_by_id(player, card, 0x5077, 3);
      }
      else
      {
        default_token_definition(player, card, 0x4936, &token);
        token.tou = 7;
        token.pow = 7;
        token.color_forced = '\b';
        generate_token(&token);
      }
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

/* ManalinkEh 2022: 0x25d09c0; original cards/throne_of_eldraine.c:2737. */
int card_inspiring_veteran(int player, int card, event_t event)
{
  if (1 < event - EVENT_POWER)
  {
    return 0;
  }
  boost_creature_type(player, card, event, SUBTYPE_KNIGHT, 1, 1, 0, BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x25d0a20; original cards/throne_of_eldraine.c:2750. */
int card_into_the_story(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_MODIFY_COST)
  {
    result = count_graveyard(1 - player);
    if (result < 7)
    {
      return 0;
    }
    COST_COLORLESS = COST_COLORLESS + -3;
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
    else if (event == EVENT_RESOLVE_SPELL)
    {
      draw_cards(player, 4);
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

/* ManalinkEh 2022: 0x25d0b00; original cards/throne_of_eldraine.c:2776. */
int card_irencrag_pyromancer(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (cards_drawn_this_turn[player] == 2)
  {
    result = card_drawn_trigger(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
    if (result != 0)
    {
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      td.allow_cancel = 0;
      instance = get_card_instance(player, card);
      instance->number_of_targets = '\0';
      result = can_target(&td);
      if (result != 0)
      {
        result = pick_target(&td, "TARGET_ANY");
        if (result != 0)
        {
          damage_target0(player, card, 3);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d0bd0; original cards/throne_of_eldraine.c:2804. */
int card_jousting_dummy(int player, int card, event_t event)
{
  int result;
  result = generic_shade_merge_pt(player, card, event, 0, 3, 0, 0, 0, 0, 0, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x25d0c40; original cards/throne_of_eldraine.c:2814. */
int card_keeper_of_fables(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_DEAL_DAMAGE)
  {
    new_default_test_definition(&test, 2, "");
    test.subtype = 0x5b;
    test.subtype_flag = 1;
    check_damage_test(player, -1, EVENT_DEAL_DAMAGE,
                      DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE, player, card, 0, &test);
  }
  result = resolve_damage_trigger(
      player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE, player, card);
  if (result == 0)
  {
    return 0;
  }
  draw_cards(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x25d2250; original cards/throne_of_eldraine.c:3392. */
int card_mirrormade(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  default_target_definition(player, card, &td, 0x44);
  enters_the_battlefield_as_copy_of_any(player, card, event, &td, "TARGET_ARTIFACT_ENCHANTMENT");
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25d2580; original cards/throne_of_eldraine.c:3492. */
int card_mystical_dispute(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  unsigned int uVar3;
  if (event == EVENT_MODIFY_COST)
  {
    if (card_on_stack_controller != -1)
    {
      uVar2 = get_color(card_on_stack_controller, card_on_stack);
      uVar3 = get_sleighted_color_test(player, card, 4);
      if ((uVar2 & uVar3) != 0)
      {
        COST_COLORLESS = COST_COLORLESS + -2;
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    counterspell_resolve_unless_pay_x(player, card, NULL, 0, 3);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 2, NULL, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25d2be0; original cards/throne_of_eldraine.c:3677. */
int card_once_upon_a_time(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_MODIFY_COST)
  {
    result = get_trap_condition(player, 0x301);
    if (result == 0)
    {
      null_casting_cost(player, card);
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
    else if (event == EVENT_RESOLVE_SPELL)
    {
      new_default_test_definition(&test, 3, "Select a creature or land card.");
      reveal_top_cards_of_library_and_choose(player, card, player, 5, 0, TUTOR_HAND, 1,
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

/* ManalinkEh 2022: 0x25d3760; original cards/throne_of_eldraine.c:3989. */
int card_redcap_melee(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x102);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            result = damage_target0(player, card, 4);
            instance = get_card_instance(player, result);
            instance->targets[10].card = 0x50eb;
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025d37b5;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x102);
LAB_025d37b5:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE_OR_PLANESWALKER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25d3c30; original cards/throne_of_eldraine.c:4331. */
int card_specters_shriek(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;
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
        default_target_definition(player, card, &td, 0);
        td.zone = 0x1000;
        if (event != EVENT_RESOLVE_SPELL)
          goto LAB_025d3c95;
        result = valid_target(&td);
        if (result != 0)
        {
          instance = get_card_instance(player, card);
          if (hand_count[instance->targets[0].player] != 0)
          {
            new_default_test_definition(&this_test, 1, "Opponent\'s hand: Select a card to exile.");
            this_test.type_flag = 1;
            result = new_global_tutor(player, instance->targets[0].player, 4, 6, 0, 1, &this_test);
            if (-1 < result)
            {
              result = get_internal_card_id_from_csv_id(result);
              uVar3 = get_color_by_internal_id(instance->targets[0].player, result);
              uVar4 = get_sleighted_color_test(player, card, 2);
              if (((uVar3 & uVar4) == 0) && (hand_count[player] != 0))
              {
                new_default_test_definition(&this_test, 0xff, "Your hand: Select a card to exile.");
                new_global_tutor(player, player, 4, 6, 1, 3, &this_test);
                goto LAB_025d3d3c;
              }
            }
          }
          spell_fizzled = 1;
        }
      LAB_025d3d3c:
        kill_card(player, card, KILL_DESTROY);
        goto LAB_025d3c95;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
LAB_025d3c95:
  result = generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25d4820; original cards/throne_of_eldraine.c:4525. */
int card_stormfist_crusader(int player, int card, event_t event)
{
  int player_00;
  unsigned int player_01;
  menace(player, card, event);
  upkeep_trigger_ability(player, card, event, player);
  player_00 = current_turn;
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    draw_a_card(current_turn);
    lose_life(player_00, 1);
    player_01 = current_turn ^ 1;
    draw_a_card(player_01);
    lose_life(player_01, 1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d52b0; original cards/throne_of_eldraine.c:4804. */
int card_the_cauldron_of_eternity(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int owner;
  int position;
  test_definition_t test;
  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    iVar1 = count_graveyard_by_type(player, 2);
    COST_COLORLESS = COST_COLORLESS + (char)iVar1 * -2;
  LAB_025d52ea:
    if (trigger_condition != 0xd5)
      goto joined_r0x025d52fd;
  }
  else if (event != EVENT_GRAVEYARD_FROM_PLAY)
    goto LAB_025d52ea;
  default_test_definition(&test, 2);
  iVar1 =
      store_and_trigger_graveyard_from_play(player, card, event, player, 2, &test, player, card);
  if (0xb < iVar1)
  {
    iVar1 = 0xb;
  }
  instance = get_card_instance(player, card);
  iVar4 = 0;
  do
  {
    if ((iVar4 < iVar1) && (iVar3 = instance->targets[iVar4].player, iVar3 != -1))
    {
      iVar3 = find_in_owners_graveyard(iVar3, instance->targets[iVar4].card, &owner, &position);
      if (iVar3 != 0)
      {
        from_graveyard_to_deck(owner, position, 2);
      }
      instance->targets[iVar4].card = -1;
      instance->targets[iVar4].player = -1;
    }
    iVar4 = iVar4 + 1;
  } while (iVar4 != 10);
joined_r0x025d52fd:
  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&test, 2, "Select target creature card.");
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 2, "Select target creature card.");
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (iVar1 = validate_target_from_grave_source(player, card, player, 0), iVar1 != -1))
    {
      reanimate_permanent(player, card, player, iVar1, REANIMATE_DEFAULT);
    }
  }
  iVar1 = generic_activated_ability_with_target_in_grave(
      player, card, event, GAA_CAN_SORCERY_BE_PLAYED | GAA_UNTAPPED, 2, 1, 0, 0, 0, 0, 2, NULL,
      NULL, TYPE_NONE, &test, 8);
  return iVar1;
}

/* ManalinkEh 2022: 0x25d5e90; original cards/throne_of_eldraine.c:5218. */
int card_vantress_gargoyle(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_ATTACK_LEGALITY)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (result = count_graveyard(1 - current_turn), result < 7))
    {
      event_result = event_result | 1;
    }
  LAB_025d5f50:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_025d5ecd;
    }
  }
  else
  {
    if (event == EVENT_BLOCK_LEGALITY)
    {
      if (card != affected_card)
      {
        return 0;
      }
      if (player != affected_card_controller)
      {
        return 0;
      }
      if (3 < hand_count[player])
      {
        return 0;
      }
      event_result = event_result | 1;
      return 0;
    }
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_025d5f50;
  LAB_025d5ecd:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_025d5ed6;
  }
  mill(current_turn, 1);
  mill(current_turn ^ 1, 1);
LAB_025d5ed6:
  result = generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25d6280; original cards/throne_of_eldraine.c:5360. */
int card_wishclaw_talisman(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  test_definition_t test;
  enters_the_battlefield_with_counters(player, card, event, COUNTER_WISH, 3);
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      new_default_test_definition(&test, 0xff, "Select a card.");
      new_global_tutor(player, player, 1, 1, 0, 1, &test);
      instance = get_card_instance(player, card);
      result = instance->parent_card;
      instance = get_card_instance(player, card);
      give_control_of_self(instance->parent_controller, result);
    }
  }
  result = generic_activated_ability(player, card, event, 0x10001, 1, 0, 0, 0, 0, 0, 0x17a0100,
                                     NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25cb0a0; original cards/throne_of_eldraine.c:461. */
int card_ayara_first_of_locthwain(int player, int card, event_t event)
{
  const char *msg;
  int result;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 2);
    test.color = get_sleighted_color_test(player, card, 2);
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    lose_life(1 - player, 1);
    gain_life(player, 1);
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_025cb109:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_025cb10e;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_025cb109;
  }
  draw_a_card(player);
LAB_025cb10e:
  msg = get_sleighted_color_text(player, card, "Select another %s creature to sacrifice.", 1);
  new_default_test_definition(&test, 2, msg);
  test.color = get_sleighted_color_test(player, card, 2);
  test.not_me = 1;
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x25cc840; original cards/throne_of_eldraine.c:1005. */
int card_cauldron_familiar(int player, int card, event_t event)
{
  int result;
  test_definition_t test_food;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    lose_life(1 - player, 1);
    gain_life(player, 1);
    return 0;
  }
  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    new_default_test_definition(&test_food, 0xff, "Select a Food to sacrifice");
    test_food.subtype = 0x3008;
    if (event == EVENT_GRAVEYARD_ABILITY)
    {
      result = generic_activated_ability_with_sacrifice_as_cost(
          player, card, EVENT_CAN_ACTIVATE, GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
          &test_food, SARV_0);
      if (result != 0)
      {
        return 8;
      }
    }
    else if (event == EVENT_PAY_FLASHBACK_COSTS)
    {
      generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_ACTIVATE, GAA_NONE, 0, 0,
                                                       0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
                                                       &test_food, SARV_0);
      return (unsigned int)(spell_fizzled != 1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25cd820; original cards/throne_of_eldraine.c:1442. */
int card_drown_in_the_loch(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned char bVar3;
  target_definition_t td;
  target_definition_t td1;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.special = 0x8000;
    td.extra = (int32_t)ditl_graveyard_count;
    counterspell_target_definition(player, card, &td1, 0xff);
    td1.special = 0x8000;
    td1.extra = (int32_t)ditl_graveyard_count;
    get_card_instance(player, card);
    result = counterspell(player, card, EVENT_CAN_CAST, &td1, 0);
    if (result == 0)
    {
      result = generic_spell(player, card, EVENT_CAN_CAST, 0x81, &td, NULL, 1, NULL);
      return result;
    }
    return 99;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.special = 0x8000;
      td.extra = (int32_t)ditl_graveyard_count;
      counterspell_target_definition(player, card, &td1, 0xff);
      td1.special = 0x8000;
      td1.extra = (int32_t)ditl_graveyard_count;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->info_slot = 0;
        result = counterspell(player, card, EVENT_CAN_CAST, &td1, 0);
        if (result != 0)
        {
          instance->info_slot = 0x42;
          result = counterspell(player, card, EVENT_CAST_SPELL, &td1, 0);
          return result;
        }
        generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                      "Select target creature with mana value less than or equal to the number of "
                      "cards in its controller\'s graveyard.",
                      1, NULL);
        if (spell_fizzled != 1)
        {
          instance->info_slot = 0x43;
        }
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
    default_target_definition(player, card, &td, 2);
    td.special = 0x8000;
    td.extra = (int32_t)ditl_graveyard_count;
    counterspell_target_definition(player, card, &td1, 0xff);
    td1.special = 0x8000;
    td1.extra = (int32_t)ditl_graveyard_count;
    get_card_instance(player, card);
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  td.special = 0x8000;
  td.extra = (int32_t)ditl_graveyard_count;
  counterspell_target_definition(player, card, &td1, 0xff);
  td1.special = 0x8000;
  td1.extra = (int32_t)ditl_graveyard_count;
  instance = get_card_instance(player, card);
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }
  if (instance->info_slot == 0x42)
  {
    result = counterspell_validate(player, card, &td1, 0);
    bVar3 = result != 0;
    if (instance->info_slot == 0x43)
      goto LAB_025cdb92;
  }
  else
  {
    if (instance->info_slot != 0x43)
      goto LAB_025cd8e1;
    bVar3 = false;
  LAB_025cdb92:
    result = valid_target(&td);
    bVar3 = bVar3 | (result != 0) * '\x02';
  }
  if (bVar3 != 0)
  {
    if ((bVar3 & 1) != 0)
    {
      real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
    }
    if ((bVar3 & 2) != 0)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
  }
LAB_025cd8e1:
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

/* ManalinkEh 2022: 0x25cebb0; original cards/throne_of_eldraine.c:1847. */
int card_faeburrow_elder(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  int *piVar5;
  unsigned char bVar6;
  int iStack_80;
  int cols[6];
  target_definition_t td;

  bVar6 = 0;
  vigilance_2022(player, card, event);
  piVar5 = cols;
  for (iVar3 = 6; iVar3 != 0; iVar3 = iVar3 + -1)
  {
    *piVar5 = 0;
    piVar5 = piVar5 + (unsigned int)bVar6 * -2 + 1;
  }
  if (event == EVENT_COUNT_MANA)
  {
    if (card != affected_card)
      goto LAB_025ced2a;
    if (player != affected_card_controller)
    {
      cols[1] = 0;
      cols[2] = 0;
      cols[4] = 0;
      cols[5] = 0;
      cols[3] = 0;
      goto LAB_025cecc2;
    }
  }
  else if (((event != EVENT_ACTIVATE) && ((event != EVENT_CAN_ACTIVATE || (player != 1)))) &&
           ((1 < event - EVENT_POWER ||
             ((card != affected_card || (player != affected_card_controller))))))
  {
  LAB_025ced2a:
    cols[1] = 0;
    cols[2] = 0;
    cols[3] = 0;
    cols[4] = 0;
    cols[5] = 0;
    goto LAB_025cecc2;
  }
  iVar3 = 1;
  instance = get_card_instance(player, card);
  instance->info_slot = 0;
  default_target_definition(player, card, &td, 0x47);
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  iStack_80 = 0;
  do
  {
    while (true)
    {
      uVar4 = 1 << ((unsigned char)iVar3 & 0x1f);
      td.required_color = uVar4;
      iVar2 = can_target(&td);
      if (iVar2 == 0)
        break;
      cols[iVar3] = 1;
      instance = get_card_instance(player, card);
      iVar3 = iVar3 + 1;
      iStack_80 = iStack_80 + 1;
      instance->info_slot = instance->info_slot | uVar4;
      if (iVar3 == 6)
        goto LAB_025cecad;
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 != 6);
LAB_025cecad:
  if (event - EVENT_POWER < 2)
  {
    event_result = event_result + iStack_80;
  }
LAB_025cecc2:
  iVar3 = mana_producing_creature_multi(player, card, event, 0x18, 0, cols[1], cols[2], cols[3],
                                        cols[4], cols[5]);
  return iVar3;
}

/* ManalinkEh 2022: 0x25ced80; original cards/throne_of_eldraine.c:1910. */
int card_feasting_troll_king(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  bool bVar3;
  test_definition_t test_food;

  vigilance_2022(player, card, event);
  if (event == EVENT_CAST_SPELL)
  {
    if (card != affected_card)
      goto LAB_025cedca;
    if (player == affected_card_controller)
    {
      instance = get_card_instance(player, card);
      result = not_played_from_hand(player, card);
      bVar3 = trigger_condition != 0xdb;
      instance->info_slot = result;
      if (bVar3)
      {
        return 0;
      }
      goto LAB_025ceef0;
    }
    if (trigger_condition != 0xdb)
      goto LAB_025cedca;
  }
  else if (trigger_condition == 0xdb)
  {
  LAB_025ceef0:
    if (((card == affected_card) && (affected_card_controller == player)) &&
        (instance = get_card_instance(player, card), instance->info_slot != 1))
    {
      etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    }
  }
  if (event == EVENT_ETB)
  {
    generate_tokens_by_id(player, card, 0x5077, 3);
    return 0;
  }
LAB_025cedca:
  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    new_default_test_definition(&test_food, 0xff, "Select a Food to sacrifice");
    test_food.subtype = 0x3008;
    test_food.qty = 3;
    if (event == EVENT_GRAVEYARD_ABILITY)
    {
      result = generic_activated_ability_with_sacrifice_as_cost(
          player, card, EVENT_CAN_ACTIVATE, GAA_IN_YOUR_TURN, 0, 0, 0, 0, 0, 0, 0, NULL, NULL,
          TYPE_NONE, &test_food, SARV_0);
      if (result != 0)
      {
        return 8;
      }
    }
    else if (event == EVENT_PAY_FLASHBACK_COSTS)
    {
      generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_ACTIVATE,
                                                       GAA_IN_YOUR_TURN, 0, 0, 0, 0, 0, 0, 0, NULL,
                                                       NULL, TYPE_NONE, &test_food, SARV_0);
      return (unsigned int)(spell_fizzled != 1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d46f0; original cards/throne_of_eldraine.c:4495. */
int card_stonecoil_serpent(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int32_t number;

  protection_from_multicolored(player, card, event);
  instance = get_card_instance(player, card);
  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    set_special_flags2_2022(player, card, 0x20000);
    result = check_special_flags2_2022(player, card, -0x80000000);
    if (result == 0)
    {
      instance->info_slot = 0;
    }
    result = played_for_free(player, card);
    if (result == 0)
    {
      result = is_token(player, card);
      if (result == 0)
      {
        charge_mana(player, COLOR_COLORLESS, -1);
        set_x_for_x_spells(player, card, EVENT_CAST_SPELL, x_value);
        goto LAB_025d4725;
      }
    }
    instance->info_slot = 0;
    number = 0;
  }
  else
  {
  LAB_025d4725:
    number = instance->info_slot;
  }
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, number);
  return 0;
}

/* ManalinkEh 2022: 0x25d5110; original cards/throne_of_eldraine.c:4771. */
int card_tempting_witch(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  test_definition_t test;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    if ((((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
         (event == EVENT_RESOLVE_ACTIVATION)) ||
        (event == EVENT_CLEANUP))
    {
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      new_default_test_definition(&test, 0x47, "Select a Food.");
      test.subtype = 0x3008;
      if (event == EVENT_RESOLVE_ACTIVATION)
      {
        result = valid_target(&td);
        if (result != 0)
        {
          instance = get_card_instance(player, card);
          lose_life(instance->targets[0].player, 3);
        }
      }
      result = generic_activated_ability_with_sacrifice_as_cost(
          player, card, event, GAA_CAN_TARGET | GAA_UNTAPPED, 2, 0, 0, 0, 0, 0, 0, &td,
          "TARGET_PLAYER", TYPE_NONE, &test, SARV_0);
    }
    else
    {
      result = 0;
    }
    return result;
  }
  generate_token_by_id(player, card, 0x5077);
  return 0;
}

/* ManalinkEh 2022: 0x25d63e0; original cards/throne_of_eldraine.c:5397. */
int card_witchs_oven(int player, int card, event_t event)
{
  unsigned int uVar1;
  card_instance_t *instance;
  int result;
  test_definition_t test;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x201, 0, 0, 0, 0, 0, 0, 0,
                                       NULL, NULL);
  }
  else if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
    instance = get_card_instance(player, card);
    instance->state = instance->state | 0x10;
    uVar1 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST, &test);
    if (uVar1 == 0)
    {
      instance = get_card_instance(player, card);
      spell_fizzled = 1;
      instance->state = instance->state & 0xffffffef;
      result = 0;
    }
    else
    {
      result = get_toughness((int)uVar1 >> 0x10 & 0xff, uVar1 >> 0x18);
      instance = get_card_instance(player, card);
      instance->info_slot_fn = (CardFunction)((3 < result) + 1);
      result = 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    result = 0;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      generate_tokens_by_id(player, card, 0x5077, instance->info_slot);
      return 0;
    }
  }
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int play_card_from_adventure(int player, int t_player, int iid);
static int adventure_effect(int player, int card, event_t event);
int adventure(int player, int card, event_t event, int csvid_creature);
int generic_adventurer(int player, int card, event_t event, int priority_creature,
                       int priority_adventure, int csvid_adventure);

/* ManalinkEh 2022: 0x25c9a70; original cards/throne_of_eldraine.c:46. */
int play_card_from_adventure(int player, int t_player, int iid)
{
  int card;
  int result;

  remove_card_from_rfg(t_player, (unsigned int)cards_data[iid].id);
  card = add_card_to_hand(player, iid);
  set_special_flags2_2022(player, card, 0x800);
  if (t_player != player)
  {
    if ((player == 0) && (t_player == 1))
    {
      add_state(0, card, 0x1000);
    }
    else
    {
      remove_state(player, card, 0x1000);
    }
  }
  result = real_play_spell_for_free(player, card, -0x80000000);
  if (result == 0)
  {
    obliterate_card(player, card);
    card = -1;
  }
  return card;
}

/* ManalinkEh 2022: 0x25c9b30; original cards/throne_of_eldraine.c:65. */
static int adventure_effect(int player, int card, event_t event)
{
  unsigned int uVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  const char *pcVar5;
  const char *pcVar6;
  const char *pcVar7;
  const char *pcVar8;
  const char *pcVar9;
  const char *pcVar10;
  const char *pcVar11;
  const char *pcVar12;
  const char *pcVar13;
  const char *pcVar14;
  const char *pcVar15;
  const char *pcVar16;
  const char *pcVar17;
  const char *pcVar18;
  const char *pcVar19;
  const char *pcVar20;
  int iVar21;
  unsigned int uVar22;
  target_t *ptVar23;
  int32_t *piVar24;
  int *piVar25;
  unsigned char bVar26;
  unsigned int uStack_54;
  int castable[16];

  bVar26 = 0;
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    iVar3 = can_sorcery_be_played(player, event);
    if (iVar3 != 0)
    {
      uVar22 = 1;
      goto joined_r0x025c9bd5;
    }
    uVar22 = 0;
    if (stack_size != 1)
      goto joined_r0x025c9bd5;
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    iVar3 = can_sorcery_be_played(player, event);
    if (iVar3 != 0)
    {
      return 0;
    }
    if (stack_size != 1)
    {
      return 0;
    }
  }
  uVar22 = 0;
  if (card_on_stack_controller == player)
  {
    uVar22 = (unsigned int)(card_on_stack == card);
  }
joined_r0x025c9bd5:
  if (event == EVENT_CAN_ACTIVATE)
  {
    piVar24 = &instance->targets[0].card;

    uStack_54 = 0;
    do
    {
      uVar1 = uStack_54;
      if ((((-1 < ((target_t *)(piVar24 + -1))->player) && (-1 < *piVar24)) &&
           (iVar3 = has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, *piVar24), iVar3 != 0)) &&
          (((iVar3 = find_iid_in_rfg(((target_t *)(piVar24 + -1))->player, *piVar24),
             -1 < iVar3 && (uVar1 = uVar22, uVar22 == 0)) &&
            (uVar1 = uStack_54, cards_data[*piVar24].id == 0x5037))))
      {
        uStack_54 = 1;
        uVar1 = uStack_54;
      }
      uStack_54 = uVar1;
      piVar24 = piVar24 + 2;
    } while (&instance->targets[0x10].card != piVar24);
    return uStack_54;
  }
  if (event == EVENT_ACTIVATE)
  {
    piVar25 = castable;

    for (iVar3 = 0x10; iVar3 != 0; iVar3 = iVar3 + -1)
    {
      *piVar25 = 0;
      piVar25 = piVar25 + (unsigned int)bVar26 * -2 + 1;
    }
    iVar3 = 0;
    do
    {
      if (((instance->targets[iVar3].player != -1) &&
           (iVar4 = instance->targets[iVar3].card, iVar4 != -1)) &&
          (((iVar4 = has_mana_to_cast_iid(player, EVENT_ACTIVATE, iVar4),
             iVar4 != 0 && (iVar4 = find_iid_in_rfg(instance->targets[iVar3].player,
                                                    instance->targets[iVar3].card),
                            -1 < iVar4)) &&
            ((iVar4 = instance->targets[iVar3].card,
              uVar22 != 0 || (cards_data[iVar4].id == 0x5037))))))
      {
        castable[iVar3] = iVar4;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 != 0x10);
    pcVar5 = get_card_name_by_id((unsigned int)cards_data[castable[0xf]].id);
    pcVar6 = get_card_name_by_id((unsigned int)cards_data[castable[0xe]].id);
    pcVar7 = get_card_name_by_id((unsigned int)cards_data[castable[0xd]].id);
    pcVar8 = get_card_name_by_id((unsigned int)cards_data[castable[0xc]].id);
    pcVar9 = get_card_name_by_id((unsigned int)cards_data[castable[0xb]].id);
    pcVar10 = get_card_name_by_id((unsigned int)cards_data[castable[10]].id);
    pcVar11 = get_card_name_by_id((unsigned int)cards_data[castable[9]].id);
    pcVar12 = get_card_name_by_id((unsigned int)cards_data[castable[8]].id);
    pcVar13 = get_card_name_by_id((unsigned int)cards_data[castable[7]].id);
    pcVar14 = get_card_name_by_id((unsigned int)cards_data[castable[6]].id);
    pcVar15 = get_card_name_by_id((unsigned int)cards_data[castable[5]].id);
    pcVar16 = get_card_name_by_id((unsigned int)cards_data[castable[4]].id);
    pcVar17 = get_card_name_by_id((unsigned int)cards_data[castable[3]].id);
    pcVar18 = get_card_name_by_id((unsigned int)cards_data[castable[2]].id);
    pcVar19 = get_card_name_by_id((unsigned int)cards_data[castable[1]].id);
    pcVar20 = get_card_name_by_id((unsigned int)cards_data[castable[0]].id);
    iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\004", pcVar20,
                        castable[0], 1, pcVar19, castable[1], 1, pcVar18, castable[2], 1, pcVar17,
                        castable[3], 1, pcVar16, castable[4], 1, pcVar15, castable[5], 1, pcVar14,
                        castable[6], 1, pcVar13, castable[7], 1, pcVar12, castable[8], 1, pcVar11,
                        castable[9], 1, pcVar10, castable[10], 1, pcVar9, castable[0xb], 1, pcVar8,
                        castable[0xc], 1, pcVar7, castable[0xd], 1, pcVar6, castable[0xe], 1,
                        pcVar5, castable[0xf], 1, 0, 0, 0, 0);
    if (iVar3 == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      iVar4 = castable[iVar3 + -1];
      iVar21 = charge_mana_from_id(player, -1, EVENT_ACTIVATE, (unsigned int)cards_data[iVar4].id);
      if ((iVar21 != 0) &&
          (iVar4 = play_card_from_adventure(player, instance->targets[iVar3 + -1].player, iVar4),
           iVar4 != 0))
      {
        instance->targets[iVar3 + -1].player = -1;
        instance->targets[iVar3 + -1].card = -1;
        cant_be_responded_to = 1;
        ptVar23 = instance->targets;
        do
        {
          if (((ptVar23->player != -1) && (ptVar23->card != -1)) &&
              (iVar3 = find_iid_in_rfg(ptVar23->player, ptVar23->card), -1 < iVar3))
          {
            return 0;
          }
          ptVar23 = ptVar23 + 1;
        } while (ptVar23 != instance->targets + 0x10);
        kill_card(player, card, KILL_EXILE);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25ca280; original cards/throne_of_eldraine.c:169. */
int adventure(int player, int card, event_t event, int csvid_creature)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;

  if (event == EVENT_COUNTERSPELL)
  {
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
    iVar1 = is_token(player, card);
    if (iVar1 != 0)
    {
      return 0;
    }
    adventure_transform(player, card, csvid_creature);
    return 0;
  }
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }
  iVar1 = is_token(player, card);
  if (iVar1 != 0)
  {
    return 0;
  }

  iVar1 = 0;
  if (0 < active_cards_count[player])
  {
    do
    {
      instance = in_play(player, iVar1);
      if ((((instance != NULL) && (iVar2 = is_what(player, iVar1, 0x80), iVar2 != 0)) &&
           (instance->info_slot_fn == adventure_effect)) &&
          ((instance->targets[0xf].player == -1 && (instance->targets[0xf].card == -1))))
      {
        if (iVar1 != -1)
        {

          instance = get_card_instance(player, iVar1);
          iVar1 = 1;
          goto LAB_025ca35a;
        }
        break;
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < active_cards_count[player]);
  }

  iVar1 = create_legacy_activate(player, card, adventure_effect);
  instance = get_card_instance(player, iVar1);
  instance->display_pic_csv_id = 0x616c;
  instance->display_pic_num = 0;
  iVar1 = get_owner(player, card);
LAB_025ca40b:
  instance->targets[0].player = iVar1;
  iVar1 = get_original_internal_card_id(player, card);
  instance->targets[0].card = iVar1;
  goto LAB_025ca420;
  while (iVar1 = iVar1 + 1, iVar1 != 0x10)
  {
  LAB_025ca35a:
    if ((instance->targets[iVar1].player == -1) && (instance->targets[iVar1].card == -1))
    {
      instance = (card_instance_t *)(instance->color_id + iVar1 * 8 + -0x40);
      iVar1 = get_owner(player, card);
      goto LAB_025ca40b;
    }
  }
LAB_025ca420:
  obliterate_card(player, card);
  iVar1 = get_owner(player, card);
  add_csvid_to_rfg(iVar1, csvid_creature);
  return 0;
}

/* ManalinkEh 2022: 0x25ca460; original cards/throne_of_eldraine.c:178. */
int generic_adventurer(int player, int card, event_t event, int priority_creature,
                       int priority_adventure, int csvid_adventure)
{
  int iVar1;
  unsigned int uVar2;
  card_instance_t *instance;
  int iVar3;
  const char *pcVar4;
  const char *pcVar5;
  unsigned int csvid;
  bool bVar6;
  int iStack_14;

  if ((event == EVENT_QUERY) && (attacking_card == 1))
  {
    return 1;
  }
  if ((((stack_size == 1) && (card_on_stack_controller == player)) && (card_on_stack == card)) &&
      (iVar1 = can_legally_play_csvid(player, csvid_adventure), iVar1 != 0))
  {
    uVar2 = 1;
  }
  else
  {
    iVar1 = can_legally_play_csvid_now(player, csvid_adventure, event);
    uVar2 = (unsigned int)(iVar1 != 0);
  }
  iVar1 = get_original_internal_card_id(player, card);
  iStack_14 = 1;
  csvid = (unsigned int)cards_data[iVar1].id;
  if ((cards_data[iVar1].type & 0x10) == 0)
  {
    iStack_14 = can_sorcery_be_played(player, event);
  }
  if (event == EVENT_MODIFY_COST)
  {
    iVar1 = has_mana_to_cast_id(player, EVENT_MODIFY_COST, csvid_adventure);
    if (((uVar2 == 0) || (iVar1 == 0)) &&
        ((iVar1 = has_mana_to_cast_id(player, EVENT_MODIFY_COST, csvid),
          iStack_14 == 0 || (iVar1 == 0))))
    {
      infinite_casting_cost();
      return 0;
    }
    null_casting_cost(player, card);
    return 0;
  }
  if (event == EVENT_QUERY)
  {
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
    iVar1 = instance->targets[0xc].card;
    if (iVar1 < 0)
    {
      return 0;
    }
    event_result = iVar1;
    instance->regen_status = instance->regen_status | 0x9000000;
    return 0;
  }
  if (event == EVENT_CAN_CAST)
  {
    iVar1 = has_mana_to_cast_id(player, EVENT_CAN_CAST, csvid_adventure);
    if ((uVar2 != 0) && (iVar1 != 0))
    {
      return uVar2;
    }
    iVar1 = has_mana_to_cast_id(player, EVENT_CAN_CAST, csvid);
    if (iStack_14 == 0)
    {
      return 0;
    }
    if (iVar1 == 0)
    {
      return 0;
    }
    return iStack_14;
  }
  if (event != EVENT_CAST_SPELL)
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      return 0;
    }
    iVar1 = check_special_flags2_2022(player, card, 0x800);
    if (iVar1 == 0)
    {
      return 0;
    }
    remove_special_flags2_2022(player, card, 0x800);
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
  iVar1 = check_special_flags2_2022(player, card, 0x800);
  if (iVar1 != 0)
  {
    return 0;
  }
  iVar1 = check_special_flags3_2022(player, card, 1);
  if (iVar1 != 0)
  {
    return 0;
  }

  iVar1 = played_for_free(player, card);
  if (iVar1 == 0)
  {
    iVar1 = is_token(player, card);
    bVar6 = iVar1 != 0;
    iVar3 = check_special_flags2_2022(player, card, -0x80000000);
    if (iVar3 != 0)
      goto LAB_025ca795;
    instance->info_slot = 0;
    instance->number_of_targets = '\0';
    if (iVar1 != 0)
      goto LAB_025ca785;
    iVar1 = has_mana_to_cast_id(player, EVENT_CAST_SPELL, csvid_adventure);
    if ((iVar1 != 0) && (uVar2 != 0))
    {
      iVar1 = has_mana_to_cast_id(player, EVENT_CAST_SPELL, csvid);
      if (iVar1 != 0)
        goto LAB_025ca955;
      goto LAB_025ca94b;
    }
  LAB_025ca78d:
    iVar1 = 1;
  }
  else
  {
    iVar1 = check_special_flags2_2022(player, card, -0x80000000);
    bVar6 = true;
    if (iVar1 != 0)
      goto LAB_025ca795;

    instance->info_slot = 0;
    instance->number_of_targets = '\0';
    bVar6 = true;
  LAB_025ca785:
    if (uVar2 == 0)
      goto LAB_025ca78d;
  LAB_025ca955:
    if (iStack_14 == 0)
    {
    LAB_025ca94b:
      iVar1 = 2;
    }
    else
    {
      pcVar4 = get_card_name_by_id(csvid_adventure);
      pcVar5 = get_card_name_by_instance(instance);
      iVar1 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014", pcVar5, iStack_14,
                          priority_creature, pcVar4, 1, priority_adventure, 0, 0, 0, 0);
      if (iVar1 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
    }
  }
  instance->info_slot = iVar1;
LAB_025ca795:
  if (instance->info_slot == 1)
  {
    if (!bVar6)
    {
      charge_mana_from_id(player, card, EVENT_CAST_SPELL, csvid);
      iVar1 = is_x_spell(player, card);
      if (iVar1 != 0)
      {
        instance->targets[1].card = x_value;
      }
    }
  }
  else if (instance->info_slot == 2)
  {
    instance->targets[0xd].player = csvid;
    instance->targets[0xd].card = csvid;
    adventure_transform(player, card, csvid_adventure);
    if (!bVar6)
    {
      charge_mana_from_id(player, card, EVENT_CAST_SPELL, csvid_adventure);
    }
    bVar6 = spell_fizzled != 1;
    instance->targets[1].card = x_value;
    if (bVar6)
    {

      iVar1 = get_internal_card_id_from_csv_id(csvid_adventure);
      call_card_fn(cards_data[iVar1].code_pointer, instance, player, card, EVENT_CAST_SPELL);
      if (spell_fizzled != 1)
      {
        return 0;
      }
    }
    adventure_transform(player, card, csvid);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25cac80; original cards/throne_of_eldraine.c:351. */
int card_alter_fate(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;

  if (event == EVENT_CAN_CAST)
  {
    new_default_test_definition(&this_test, 2, "Select a creature card.");
  LAB_025caeee:
    result = generic_spell(player, card, event, 8, NULL, NULL, 1, &this_test);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      new_default_test_definition(&this_test, 2, "Select a creature card.");
      if ((card == affected_card) && (player == affected_card_controller))
      {
        generic_spell(player, card, EVENT_CAST_SPELL, 8, NULL, NULL, 1, &this_test);
        if (spell_fizzled == 1)
        {
          return 0;
        }

        instance = get_card_instance(player, card);
        ai_modifier =
            ai_modifier +
            cards_ptr[graveyard_ptr_mutable[player][instance->targets[0].player]]->ai_base_value;
        return 0;
      }
    }
  LAB_025cad50:
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_025cad19;
  }
  else
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      new_default_test_definition(&this_test, 2, "Select a creature card.");
      if (event == EVENT_CAN_CHANGE_TARGET)
        goto LAB_025caeee;

      result = validate_target_from_grave_source(player, card, player, 0);
      if (result != -1)
      {
        from_grave_to_hand(player, result, 1);
      }
      if (event == EVENT_CAST_SPELL)
        goto LAB_025cad50;
    }
    else if (event == EVENT_CHANGE_TARGET)
    {
      new_default_test_definition(&this_test, 2, "Select a creature card.");
    }
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        ((event & ~EVENT_SET_LEGACY_EFFECT_TEXT) == EVENT_CHANGE_TARGET))
      goto LAB_025cad19;
  }
  if ((event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
  {
    return 0;
  }
LAB_025cad19:
  result = adventure(player, card, event, 0x50d7);
  return result;
}

/* ManalinkEh 2022: 0x25caf40; original cards/throne_of_eldraine.c:394. */
int card_arcane_signet(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  unsigned int uVar5;

  pcVar1 = get_card_instance(player, card);
  if (event == EVENT_CAST_SPELL)
  {

    if (((card == affected_card) && (player == affected_card_controller)) &&
        (0 < active_cards_count[player]))
    {
      iVar2 = 0;
      do
      {
        pcVar3 = in_play(player, iVar2);
        if ((pcVar3 != NULL) && (iVar4 = get_id(player, iVar2), iVar4 == 0x3953))
        {

          pcVar3 = get_card_instance(player, iVar2);
          iVar2 = pcVar3->targets[2].player;
          pcVar1->info_slot = cards_data[iVar2].color & 0x3e;
          uVar5 = get_color_from_remainder_text((unsigned int)cards_data[iVar2].id);
          uVar5 = uVar5 & 0x3e | pcVar1->info_slot;
          pcVar1->info_slot = uVar5;
          iVar2 = pcVar3->targets[2].card;
          if (-1 < iVar2)
          {
            pcVar1->info_slot = cards_data[iVar2].color & 0x3e | uVar5;
            uVar5 = get_color_from_remainder_text((unsigned int)cards_data[iVar2].id);
            pcVar1->info_slot = pcVar1->info_slot | uVar5 & 0x3e;
          }
          break;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < active_cards_count[player]);
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    play_land_sound_effect_force_color(player, card, pcVar1->info_slot);
  }
  iVar2 = plain_mana_producer(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x25cb400; original cards/throne_of_eldraine.c:554. */
int card_battle_display(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x40);
    goto LAB_025cb4ae;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025cb4fc;
    default_target_definition(player, card, &td, 0x40);
  LAB_025cb53d:
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025cb469;
  }
  else
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0x40);
    }
    else
    {
      if (event == EVENT_CHANGE_TARGET)
      {
        default_target_definition(player, card, &td, 0x40);
        goto LAB_025cb469;
      }
    LAB_025cb4fc:
      if ((event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
      {
        return 0;
      }
      default_target_definition(player, card, &td, 0x40);
      if (event == EVENT_CAST_SPELL)
        goto LAB_025cb53d;
    }
    if (event != EVENT_CAN_CHANGE_TARGET)
    {
      if ((event == EVENT_RESOLVE_SPELL) && (result = valid_target(&td), result != 0))
      {

        instance = get_card_instance(player, card);
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
    LAB_025cb469:
      result = adventure(player, card, event, 0x505b);
      return result;
    }
  }
LAB_025cb4ae:
  result = generic_spell(player, card, event, 1, &td, "TARGET_ARTIFACT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25cb600; original cards/throne_of_eldraine.c:578. */
int card_beanstalk_giant(int player, int card, event_t event)
{
  int result;

  if (event - EVENT_POWER < 2)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = count_subtype(player, 1, -1);
      set_characteristic_pt(result);
    }
  LAB_025cb688:
    if (event == EVENT_CHANGE_TARGET)
      goto LAB_025cb650;
  }
  else
  {
    if (event == EVENT_CAN_CAST)
      goto LAB_025cb650;
    if (event != EVENT_CAST_SPELL)
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
        goto LAB_025cb650;
      goto LAB_025cb688;
    }
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_025cb650;
  }
  if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
  {
    return 0;
  }
LAB_025cb650:
  result = generic_adventurer(player, card, event, 5, 5, 0x502d);
  return result;
}

/* ManalinkEh 2022: 0x25cb6f0; original cards/throne_of_eldraine.c:601. */
int card_beloved_princess(int player, int card, event_t event)
{
  int result;

  lifelink(player, card, event);
  if (((event == EVENT_BLOCK_LEGALITY) && (attacking_card_controller == player)) &&
      (attacking_card == card))
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      result = get_power(affected_card_controller, affected_card);
      if (2 < result)
      {
        event_result = 1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25cb920; original cards/throne_of_eldraine.c:683. */
int card_boulder_rush(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    goto LAB_025cb9ce;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025cba1c;
    default_target_definition(player, card, &td, 2);
  LAB_025cba5d:
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025cb989;
  }
  else
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 2);
    }
    else
    {
      if (event == EVENT_CHANGE_TARGET)
      {
        default_target_definition(player, card, &td, 2);
        goto LAB_025cb989;
      }
    LAB_025cba1c:
      if ((event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
      {
        return 0;
      }
      default_target_definition(player, card, &td, 2);
      if (event == EVENT_CAST_SPELL)
        goto LAB_025cba5d;
    }
    if (event != EVENT_CAN_CHANGE_TARGET)
    {
      if ((event == EVENT_RESOLVE_SPELL) && (result = valid_target(&td), result != 0))
      {

        instance = get_card_instance(player, card);
        pump_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card, 2, 0);
      }
    LAB_025cb989:
      result = adventure(player, card, event, 0x50f0);
      return result;
    }
  }
LAB_025cb9ce:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25cbb30; original cards/throne_of_eldraine.c:714. */
int card_brazen_borrower(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_BLOCK_LEGALITY)
  {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
        (result = check_for_ability(attacking_card_controller, attacking_card, 0x20), result == 0))
    {
      event_result = 1;
    }
  }
  else
  {
    if (event == EVENT_CAN_CAST)
      goto LAB_025cbb68;
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_025cbb68;
    }
    else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
             (event == EVENT_CHANGE_TARGET))
      goto LAB_025cbb68;
  }
  if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
  {
    return 0;
  }
LAB_025cbb68:
  result = generic_adventurer(player, card, event, 5, 5, 0x5038);
  return result;
}

/* ManalinkEh 2022: 0x25cbc30; original cards/throne_of_eldraine.c:766. */
int card_cast_off(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
    goto LAB_025cbc64;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025cbc8c;
  LAB_025cbcb3:
    if (player != affected_card_controller)
    {
    LAB_025cbcbb:
      result = adventure(player, card, event, 0x50e7);
      return result;
    }
  }
  else
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event == EVENT_CAN_CHANGE_TARGET)
        goto LAB_025cbc64;
      if (event == EVENT_CHANGE_TARGET)
        goto LAB_025cbcbb;
    LAB_025cbc8c:
      if ((event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
      {
        return 0;
      }
      if (event == EVENT_CAST_SPELL)
      {
        if (affected_card != card)
          goto LAB_025cbcbb;
        goto LAB_025cbcb3;
      }
    }
    if (event != EVENT_CAN_CHANGE_TARGET)
    {
      if (event == EVENT_RESOLVE_SPELL)
      {
        default_test_definition(&test, 2);
        test.subtype = 0x48;
        test.subtype_flag = 1;
        new_manipulate_all(player, card, 2, &test, ACT_KILL_DESTROY);
      }
      goto LAB_025cbcbb;
    }
  }
LAB_025cbc64:
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25cca30; original cards/throne_of_eldraine.c:1064. */
int card_charming_prince(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 2);
    td.special = 0x8000;
    td.extra = (int32_t)another_you_own;
    td.allow_cancel = 0;
    result = can_target(&td);
    result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Scry 2", 1, 8, "Gain 3 life", 1,
                         3, "Blink", result, 8, 0, 0, 0, 0);
    if (result == 2)
    {
      gain_life(player, 3);
    }
    else if (result == 3)
    {
      result = can_target(&td);
      if (result != 0)
      {
        result = new_pick_target(&td, "Select another target creature you own", 0, 0x81);
        if (result != 0)
        {
          instance = get_card_instance(player, card);
          result = instance->targets[0].card;
          instance = get_card_instance(player, card);
          remove_until_eot(player, card, instance->targets[0].player, result);
        }
      }
    }
    else if (result == 1)
    {
      scry(player, 2);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25ccfe0; original cards/throne_of_eldraine.c:1233. */
int card_curious_pair(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_025cd008;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_025cd008;
    if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
    {
      return 0;
    }
  }
LAB_025cd008:
  result = generic_adventurer(player, card, event, 4, 5, 0x504e);
  return result;
}

/* ManalinkEh 2022: 0x25cd560; original cards/throne_of_eldraine.c:1353. */
int card_deafening_silence(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    iVar1 = is_humiliated_for_ability(player, card, AL_STATIC);
    if (iVar1 == 0)
    {
      iVar1 = is_what(affected_card_controller, affected_card, 3);
      if (iVar1 == 0)
      {
        iVar1 = get_specific_storm_count(affected_card_controller);
        iVar2 = get_stormcreature_count(affected_card_controller);
        if (iVar1 != iVar2 && -1 < iVar1 - iVar2)
        {
          infinite_casting_cost();
        }
      }
    }
  }
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x25cdf30; original cards/throne_of_eldraine.c:1596. */
int card_embereth_shieldbreaker(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_025cdf58;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_025cdf58;
    if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
    {
      return 0;
    }
  }
LAB_025cdf58:
  result = generic_adventurer(player, card, event, 1, 8, 0x505c);
  return result;
}

/* ManalinkEh 2022: 0x25ce960; original cards/throne_of_eldraine.c:1812. */
int card_fae_of_wishes(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    get_card_instance(player, card);
    if (event != EVENT_CAN_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 1, 0, 1, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled != 1)
      {
        multidiscard(player, 2, 0);
      }
      goto LAB_025ce996;
    }
    if (1 < hand_count[player])
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 1, 0, 1, 0, 0, 0, 0,
                                         NULL, NULL);
      return result;
    }
  LAB_025ce9a0:
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
      goto LAB_025ce9b5;
  }
  else if (((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP)) &&
           (pcVar2 = get_card_instance(player, card), event == EVENT_RESOLVE_ACTIVATION))
  {
    pcVar3 = in_play(pcVar2->parent_controller, pcVar2->parent_card);
    if (pcVar3 != NULL)
    {
      bounce_permanent(pcVar2->parent_controller, pcVar2->parent_card);
    }
  }
  else
  {
  LAB_025ce996:

    if (event == EVENT_CAN_CAST)
      goto LAB_025ce9b5;
    if (event != EVENT_CAST_SPELL)
      goto LAB_025ce9a0;
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_025ce9b5;
  }
  if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
  {
    return 0;
  }
LAB_025ce9b5:
  result = generic_adventurer(player, card, event, 5, 10, 0x5065);
  return result;
}

/* ManalinkEh 2022: 0x25cf040; original cards/throne_of_eldraine.c:1968. */
int card_fertile_footsteps(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAN_CAST)
  {
    result = basic_spell(player, card, EVENT_CAN_CAST);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
    {
    LAB_025cf0a8:
      if ((event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
      {
        return 0;
      }
      if (event != EVENT_CAST_SPELL)
        goto LAB_025cf075;
      if (affected_card != card)
        goto LAB_025cf07e;
    }

    if ((player == affected_card_controller) &&
        (result = get_total_available_lands(player), 1 < result - 4U))
    {
      ai_modifier = ai_modifier + 0x30;
    }
  }
  else
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event != EVENT_CAN_CHANGE_TARGET)
      {
        if (event == EVENT_CHANGE_TARGET)
          goto LAB_025cf07e;
        goto LAB_025cf0a8;
      }
    LAB_025cf075:
      if (event != EVENT_RESOLVE_SPELL)
        goto LAB_025cf07e;
    }
    global_tutor(player, player, 1, 4, 0, 4, 1, 0, 0x2000, 0, 0, 0, 0, 0, -1, 0);
  }
LAB_025cf07e:
  result = adventure(player, card, event, 0x502c);
  return result;
}

/* ManalinkEh 2022: 0x25cf6c0; original cards/throne_of_eldraine.c:2171. */
int card_foulmire_knight(int player, int card, event_t event)
{
  int result;

  deathtouch_2022(player, card, event);
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_025cf6f9;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_025cf6f9;
    if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
    {
      return 0;
    }
  }
LAB_025cf6f9:
  result = generic_adventurer(player, card, event, 5, 5, 0x507c);
  return result;
}

/* ManalinkEh 2022: 0x25cf800; original cards/throne_of_eldraine.c:2245. */
int card_garruk_cursed_huntsman(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t *ptStack_b4;
  token_generation_t token;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Wolves", 1, 5, 0,
                           "Destroy a creature & draw", result, 10, 0xfffffffd, "Emblem", 1, 0x14,
                           0xfffffffa, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 2)
      {
        new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      }
      goto LAB_025cf875;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_025cf875;
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
  }
  ptStack_b4 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    result = instance->info_slot;
    if (result == 1)
    {
      default_token_definition(player, card, 0x49a8, &token);
      token.qty = '\x02';
      token.tou = 2;
      token.pow = 2;
      token.special_infos = 0x43;
      token.color_forced = '\n';
      generate_token(&token);
    }
    else if (result == 2)
    {
      result = valid_target(ptStack_b4);
      if (result != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        draw_a_card(player);
      }
    }
    else if (result == 3)
    {
      generate_token_by_id(player, card, 0x5085);
    }
  }
LAB_025cf875:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x25d0770; original cards/throne_of_eldraine.c:2606. */
int card_haggle(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  if (event == EVENT_CAN_CAST)
  {
    result = basic_spell(player, card, EVENT_CAN_CAST);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_025d07a6;
  }
  else
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      if (0 < hand_count[player])
      {

        default_test_definition(&this_test, 0);
        result = new_select_a_card(player, player, 4, 0, 3, -1, &this_test);
        if (result != -1)
        {
          discard_card(player, result);
          draw_a_card(player);
        }
      }
      goto LAB_025d07a6;
    }
    if ((event == EVENT_CAN_CHANGE_TARGET) ||
        ((event & ~EVENT_SET_LEGACY_EFFECT_TEXT) == EVENT_CHANGE_TARGET))
      goto LAB_025d07a6;
  }
  if ((event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
  {
    return 0;
  }
LAB_025d07a6:
  result = adventure(player, card, event, 0x50bd);
  return result;
}

/* ManalinkEh 2022: 0x25d08c0; original cards/throne_of_eldraine.c:2658. */
int card_hearts_desire(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAN_CAST)
    goto LAB_025d08f1;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025d0914;
  LAB_025d093d:
    if (player == affected_card_controller)
    {
    LAB_025d08f1:
      result = basic_spell(player, card, event);
      return result;
    }
  }
  else
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event == EVENT_CAN_CHANGE_TARGET)
        goto LAB_025d08f1;
      if (event == EVENT_CHANGE_TARGET)
        goto LAB_025d0948;
    LAB_025d0914:
      if ((event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
      {
        return 0;
      }
      if (event == EVENT_CAST_SPELL)
      {
        if (affected_card != card)
          goto LAB_025d0948;
        goto LAB_025d093d;
      }
    }
    if (event == EVENT_CAN_CHANGE_TARGET)
      goto LAB_025d08f1;
    if (event == EVENT_RESOLVE_SPELL)
    {
      generate_token_by_id(player, card, 0x494c);
    }
  }
LAB_025d0948:
  result = adventure(player, card, event, 0x50b3);
  return result;
}

/* ManalinkEh 2022: 0x25c9120; original cards/throne_of_eldraine.c:2972. */
static int korvold_fae_cursed_king_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  int result;
  card_instance_t *pcVar4;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x50a9))
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
  LAB_025c915c:
    uVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto LAB_025c917a;
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
      goto LAB_025c915c;

    pcVar1 = get_card_instance(player, card);
    pcVar4 = in_play(affected_card_controller, affected_card);
    if ((pcVar4 != NULL) && (pcVar4 = get_card_instance(affected_card_controller, affected_card),
                             pcVar4->kill_code != '\0'))
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
        *(char *)((int)&pcVar1->targets[0xb].player + 1) =
            *(char *)((int)&pcVar1->targets[0xb].player + 1) + '\x01';
      }
      pcVar1 = get_card_instance(affected_card_controller, affected_card);
      if (pcVar1->kill_code == '\x03')
      {
        default_test_definition(&test, 0x47);
        count_for_gfp_ability(player, card, 0x77, player, 0, &test);
      }
    }
    uVar2 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
    {
      check_status(player, card, 0x10000);
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {

    draw_cards(player, uVar2 & 0xff);
    kill_card(player, card, KILL_EXILE);
  }
LAB_025c917a:
  result = check_status(player, card, 0x10000);
  if ((event == EVENT_CLEANUP) && (result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d1790; original cards/throne_of_eldraine.c:3097. */
int card_linden_the_steadfast_queen(int player, int card, event_t event)
{
  test_definition_t test;

  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  if (event == EVENT_DECLARE_ATTACKERS)
  {
    default_test_definition(&test, 2);
    test.color = get_sleighted_color_test(player, card, 0x20);
    store_attackers(player, card, EVENT_DECLARE_ATTACKERS,
                    DAT_SEPARATE_TRIGGERS | DAT_RESOLVE_TRIGGER_MANDATORY, player, -1, &test);
    dat_ability(player, card, EVENT_DECLARE_ATTACKERS,
                DAT_SEPARATE_TRIGGERS | DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    return 0;
  }
  dat_ability(player, card, event, DAT_SEPARATE_TRIGGERS | DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  gain_life(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x25d1f10; original cards/throne_of_eldraine.c:3256. */
int card_lovestruck_beast(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (event == EVENT_ABILITIES)
  {
    default_test_definition(&test, 2);
    test.toughness = 1;
    test.power = 1;
    result = check_battlefield_for_special_card(player, card, player, 0, &test);
    if (result == 0)
    {
      cannot_attack_2022(player, card, EVENT_ABILITIES);
    }
  }
  else
  {
    if (event == EVENT_CAN_CAST)
      goto LAB_025d1f3d;
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_025d1f3d;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_025d1f3d;
  }
  if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
  {
    return 0;
  }
LAB_025d1f3d:
  result = generic_adventurer(player, card, event, 5, 8, 0x50b4);
  return result;
}

/* ManalinkEh 2022: 0x25d2110; original cards/throne_of_eldraine.c:3350. */
int card_merchant_of_the_vale(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      if (event != EVENT_CAN_CAST)
      {
        if (event == EVENT_CAST_SPELL)
        {
          if ((card == affected_card) && (player == affected_card_controller))
            goto LAB_025d215b;
        }
        else if ((event == EVENT_RESOLVE_SPELL) ||
                 ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
          goto LAB_025d215b;
        if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
        {
          return 0;
        }
      }
    LAB_025d215b:
      result = generic_adventurer(player, card, event, 5, 7, 0x50be);
      return result;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      draw_cards(player, 1);
    }
  }
  result = generic_activated_ability(player, card, event, 0x20, 2, 0, 0, 0, 1, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25d22c0; original cards/throne_of_eldraine.c:3419. */
int card_murderous_rider(int player, int card, event_t event)
{
  int result;
  int owner;
  int position;

  lifelink(player, card, event);
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if ((result != 0) &&
      (result = find_in_owners_graveyard(player, card, &owner, &position), result != 0))
  {

    result = graveyard_ptr_mutable[owner][position];
    remove_card_from_grave(owner, position);
    put_iid_under_the_first_x_cards_of_library(owner, result, 500);
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_025d231d;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_025d231d;
    if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
    {
      return 0;
    }
  }
LAB_025d231d:
  result = generic_adventurer(player, card, event, 5, 10, 0x50c7);
  return result;
}

/* ManalinkEh 2022: 0x25d2410; original cards/throne_of_eldraine.c:3450. */
int card_mystic_sanctuary(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  subtype_t subtype;
  int iVar3;
  int iStack_ec;
  test_definition_t this_test;

  if (((event != EVENT_CAST_SPELL) || (card != affected_card)) ||
      (player != affected_card_controller))
    goto LAB_025d2428;
  iVar1 = check_state(player, card, 0x10);
  if (iVar1 == 0)
  {

    iVar1 = 0;
    iStack_ec = 0;
    if (0 < active_cards_count[player])
    {
      do
      {
        instance = in_play(player, iVar1);
        if (instance != NULL)
        {
          subtype = get_hacked_subtype(player, card, 0x5002);
          iVar3 = has_subtype(player, iVar1, subtype);
          if ((iVar3 != 0) && (iStack_ec = iStack_ec + 1, iStack_ec == 3))
            goto LAB_025d2508;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player]);
      if (2 < iStack_ec)
        goto LAB_025d2508;
    }
    instance = get_card_instance(player, card);
    instance->state = instance->state | 0x10;
  }
  else
  {
  LAB_025d2508:
    new_default_test_definition(&this_test, 0x38, "Select target instant or sorcery card.");
    iVar1 = new_special_count_grave(player, &this_test);
    if (iVar1 != 0)
    {
      iVar1 = graveyard_has_shroud(player);
      if (iVar1 == 0)
      {
        new_global_tutor(player, player, 2, 2, 0, 1, &this_test);
      }
    }
  }
LAB_025d2428:
  iVar1 = mana_producer(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x25d2680; original cards/throne_of_eldraine.c:3566. */
int card_oko_thief_of_crowns(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  target_definition_t *ptStack_1f0;
  target_definition_t *ptStack_1ec;
  target_definition_t *ptStack_1e8;
  target_definition_t td_elk;
  target_definition_t td_player;
  target_definition_t td_opp;
  test_definition_t hc;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td_elk, 0x42);
    default_target_definition(player, card, &td_player, 0x42);
    td_player.allowed_controller = player;
    default_target_definition(player, card, &td_opp, 2);
    td_opp.allowed_controller = 1 - player;
    td_opp.power_requirement = 0x2003;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td_player);
      uVar3 = 0;
      if (result != 0)
      {
        result = can_target(&td_opp);
        uVar3 = (unsigned int)(result != 0);
      }
      result = can_target(&td_elk);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Food", 1, 3, 2,
                           "Elk", result, 8, 1, "Trade", uVar3, 8, 0xfffffffb, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 2)
      {
        result = can_target(&td_elk);
        if (result != 0)
        {
          new_pick_target(&td_elk, "PUTREFY", 0, 1);
        }
      }
      else if (result == 3)
      {
        instance->number_of_targets = '\0';
        result =
            new_pick_target(&td_player, "Select target artifact or creature you control.", 0, 0x81);
        if ((result != 0) &&
            (result = new_pick_target(
                 &td_opp, "Select target creature an opponent controls with power 3 or less.", 1,
                 0x81),
             result != 0))
        {
          instance->number_of_targets = '\x02';
        }
      }
      goto LAB_025d2761;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_025d2761;
    default_target_definition(player, card, &td_elk, 0x42);
    default_target_definition(player, card, &td_player, 0x42);
    td_player.allowed_controller = player;
    default_target_definition(player, card, &td_opp, 2);
    td_opp.allowed_controller = 1 - player;
    td_opp.power_requirement = 0x2003;
    instance = get_card_instance(player, card);
  }
  ptStack_1e8 = &td_elk;
  ptStack_1ec = &td_player;
  ptStack_1f0 = &td_opp;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    result = instance->info_slot;
    if (result == 1)
    {
      generate_token_by_id(player, card, 0x5077);
    }
    else if (result == 2)
    {
      result = valid_target(ptStack_1e8);
      if (result != 0)
      {
        result = is_what(instance->targets[0].player, instance->targets[0].card, 2);
        if (result == 0)
        {
          turn_into_creature(player, card, instance->targets[0].player, instance->targets[0].card,
                             0, 0, 0);
        }
        default_test_definition(&hc, 0);
        hc.subtype = 0x3d;
        hc.toughness = 3;
        hc.power = 3;
        humiliate_and_set_pt_abilities(player, card, instance->targets[0].player,
                                       instance->targets[0].card, 2, &hc);
      }
    }
    else if (((result == 3) && (result = valid_target(ptStack_1ec), result != 0)) &&
             (result = validate_target(player, card, ptStack_1f0, 1), result != 0))
    {
      exchange_control_of_target_permanents(instance->parent_controller, instance->parent_card,
                                            instance->targets[0].player, instance->targets[0].card,
                                            instance->targets[1].player, instance->targets[1].card);
      instance->number_of_targets = '\0';
    }
  }
LAB_025d2761:
  result = planeswalker(player, card, event, 4);
  return result;
}

/* ManalinkEh 2022: 0x25d2d30; original cards/throne_of_eldraine.c:3717. */
int card_order_of_midnight(int player, int card, event_t event)
{
  int result;

  cannot_block(player, card, event);
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_025d2d69;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_025d2d69;
    if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
    {
      return 0;
    }
  }
LAB_025d2d69:
  result = generic_adventurer(player, card, event, 5, 8, 0x50d8);
  return result;
}

/* ManalinkEh 2022: 0x25d2df0; original cards/throne_of_eldraine.c:3763. */
int card_petty_theft(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = 1 - player;
    td.illegal_type = 1;
    td.preferred_controller = td.allowed_controller;
    get_card_instance(player, card);
    goto LAB_025d2ef1;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025d2f3c;
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = 1 - player;
    td.illegal_type = 1;
    td.preferred_controller = td.allowed_controller;
    get_card_instance(player, card);
  LAB_025d2fa2:
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025d2e8f;
  }
  else
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0x47);
      td.allowed_controller = 1 - player;
      td.illegal_type = 1;
      td.preferred_controller = td.allowed_controller;
      instance = get_card_instance(player, card);
    }
    else
    {
      if (event == EVENT_CHANGE_TARGET)
      {
        default_target_definition(player, card, &td, 0x47);
        td.allowed_controller = 1 - player;
        td.illegal_type = 1;
        td.preferred_controller = td.allowed_controller;
        get_card_instance(player, card);
        goto LAB_025d2e8f;
      }
    LAB_025d2f3c:
      if ((event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
      {
        return 0;
      }
      default_target_definition(player, card, &td, 0x47);
      td.allowed_controller = 1 - player;
      td.illegal_type = 1;
      td.preferred_controller = td.allowed_controller;
      instance = get_card_instance(player, card);
      if (event == EVENT_CAST_SPELL)
        goto LAB_025d2fa2;
    }
    if (event != EVENT_CAN_CHANGE_TARGET)
    {
      if ((event == EVENT_RESOLVE_SPELL) && (result = valid_target(&td), result != 0))
      {
        bounce_permanent(instance->targets[0].player, instance->targets[0].card);
      }
    LAB_025d2e8f:
      result = adventure(player, card, event, 0x5037);
      return result;
    }
  }
LAB_025d2ef1:
  result =
      generic_spell(player, card, event, 0x81, &td, "Select target nonland permanent.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25d3080; original cards/throne_of_eldraine.c:3807. */
int card_profane_insight(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAN_CAST)
    goto LAB_025d30b1;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025d30d4;
  LAB_025d30fd:
    if (player == affected_card_controller)
    {
    LAB_025d30b1:
      result = basic_spell(player, card, event);
      return result;
    }
  }
  else
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event == EVENT_CAN_CHANGE_TARGET)
        goto LAB_025d30b1;
      if (event == EVENT_CHANGE_TARGET)
        goto LAB_025d3108;
    LAB_025d30d4:
      if ((event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
      {
        return 0;
      }
      if (event == EVENT_CAST_SPELL)
      {
        if (affected_card != card)
          goto LAB_025d3108;
        goto LAB_025d30fd;
      }
    }
    if (event == EVENT_CAN_CHANGE_TARGET)
      goto LAB_025d30b1;
    if (event == EVENT_RESOLVE_SPELL)
    {
      draw_a_card(player);
      lose_life(player, 1);
    }
  }
LAB_025d3108:
  result = adventure(player, card, event, 0x507b);
  return result;
}

/* ManalinkEh 2022: 0x25d3180; original cards/throne_of_eldraine.c:3839. */
int card_questing_beast(int player, int card, event_t event)
{
  unsigned int uVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  int times_damaged[2];
  int damages[2];
  target_definition_t td;

  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  deathtouch_2022(player, card, event);
  haste(player, card, event);
  iVar2 = is_humiliated_for_ability(player, card, AL_STATIC);
  if ((event == EVENT_BLOCK_LEGALITY) && (iVar2 == 0))
  {
    if ((attacking_card_controller == player) && (attacking_card == card))
    {
      iVar2 = get_power(affected_card_controller, affected_card);
      if (iVar2 < 3)
      {
        event_result = 1;
      }
    }
  }
  else if (event == EVENT_PREVENT_DAMAGE)
  {

    instance = get_card_instance(affected_card_controller, affected_card);
    if (((instance->internal_card_id == damage_card) &&
         (instance->damage_source_player == player)) &&
        ((instance->token_status & 0x140000) != 0))
    {
      state_untargettable(affected_card_controller, affected_card, 1);
    }
  }
  iVar2 = damage_dealt_by_me(player, card, event,
                             DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                 DDBM_REPORT_DAMAGE_DEALT | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (iVar2 != 0)
  {

    instance = get_card_instance(player, card);
    uVar1 = instance->targets[1].player;
    instance->targets[1].player = 0;
    times_damaged[1] = uVar1 >> 8 & 0xff;
    times_damaged[0] = uVar1 & 0xff;
    uVar1 = instance->targets[0x10].player;
    instance->targets[0x10].player = 0;
    damages[1] = uVar1 >> 8 & 0xff;
    damages[0] = uVar1 & 0xff;
    default_target_definition(player, card, &td, 0x100);
    iVar2 = 1 - player;
    td.allow_cancel = 0;
    td.allowed_controller = iVar2;
    if (times_damaged[iVar2] != 0)
    {
      iVar4 = can_target(&td);
      if (iVar4 != 0)
      {
        iVar4 = new_pick_target(&td, "Select target planeswalker an opponent controls.", 0, 0x80);
        if (iVar4 != 0)
        {
          damage_target0(player, card, damages[iVar2]);
          instance->number_of_targets = '\0';
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d33c0; original cards/throne_of_eldraine.c:3915. */
int card_rankle_master_of_pranks(int player, int card, event_t event)
{
  int player_00;
  unsigned int uVar1;
  unsigned int uVar2;
  int result;
  unsigned int uVar4;
  test_definition_t test;

  check_legend_rule(player, card, event);
  haste(player, card, event);

  if (((((event == EVENT_DEAL_DAMAGE) || (event == EVENT_GRAVEYARD_FROM_PLAY)) ||
        (trigger_condition == 0xd7)) ||
       (event == EVENT_CHANGE_TYPE)) &&
      (uVar1 = whenever_i_deal_damage_to_a_player(
           player, card, event, DDBM_MUST_BE_COMBAT_DAMAGE | DDBM_TRIGGER_OPTIONAL),
       uVar1 != 0))
  {

    for (result = (uVar1 >> 8 & 0xff) + (uVar1 & 0xff); result != 0; result = result + -1)
    {
      uVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Discard", 1, 5,
                          "Draw & lose 1", 1, 5, "Sacrifice", 1, 5, "Discard & Draw", 1, 5,
                          "Discard & Sac", 1, 5, "Draw & Sac", 1, 5, "All three", 1, 5, 0, 0, 0, 0);
      if (uVar1 == 3)
      {
      LAB_025d35cf:
        new_default_test_definition(&test, 2, "Select a creature to sacrifice.");

        new_sacrifice(player, card, current_turn, SAC_CAUSED | SAC_NO_CANCEL, &test);
        new_sacrifice(player, card, current_turn ^ 1, SAC_CAUSED | SAC_NO_CANCEL, &test);
      }
      else
      {

        if (uVar1 == 4)
        {
          uVar4 = 0;
          uVar2 = 2;
        LAB_025d364f:

          discard(current_turn, 0, player);
          discard(current_turn ^ 1, 0, player);
        }
        else
        {
          uVar2 = uVar1 & 2;
          uVar4 = uVar1 & 4;
          if ((uVar1 & 1) != 0)
            goto LAB_025d364f;
        }
        player_00 = current_turn;
        if (uVar2 != 0)
        {

          lose_life(current_turn, 1);
          draw_a_card(player_00);
          uVar1 = current_turn ^ 1;
          lose_life(uVar1, 1);
          draw_a_card(uVar1);
        }
        if (uVar4 != 0)
          goto LAB_025d35cf;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d36a0; original cards/throne_of_eldraine.c:3969. */
int card_realm_cloaked_giant(int player, int card, event_t event)
{
  int result;

  vigilance_2022(player, card, event);
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_025d36d9;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_025d36d9;
    if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
    {
      return 0;
    }
  }
LAB_025d36d9:
  result = generic_adventurer(player, card, event, 5, 8, 0x50e8);
  return result;
}

/* ManalinkEh 2022: 0x25d38b0; original cards/throne_of_eldraine.c:4051. */
int card_rimrock_knight(int player, int card, event_t event)
{
  int result;

  cannot_block(player, card, event);
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_025d38e9;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_025d38e9;
    if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
    {
      return 0;
    }
  }
LAB_025d38e9:
  result = generic_adventurer(player, card, event, 3, 5, 0x50f1);
  return result;
}

/* ManalinkEh 2022: 0x25d3a50; original cards/throne_of_eldraine.c:4105. */
int card_roving_keep(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    pcVar2 = get_card_instance(player, card);
    pcVar3 = in_play(pcVar2->parent_controller, pcVar2->parent_card);
    if (pcVar3 != NULL)
    {

      pump_ability_until_eot_2022(player, card, pcVar2->parent_controller, pcVar2->parent_card, 2, 0,
                             0x80, 0);
      result = create_targetted_legacy_effect(player, card, effect_defender_can_attack_until_eot,
                                              pcVar2->parent_controller, pcVar2->parent_card);
      pcVar2 = get_card_instance(player, result);
      pcVar2->token_status = 0x10000;
    }
  }
  else if ((event == EVENT_POW_BOOST) ||
           (((event == EVENT_CHECK_ABILITIES && (card == affected_card)) &&
             (player == affected_card_controller))))
  {
    result = generic_shade(player, card, event, 0, 7, 0, 0, 0, 0, 0, 2, 0, 0x80, 0);
    return result;
  }
  result = generic_activated_ability(player, card, event, 0, 7, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25d3ec0; original cards/throne_of_eldraine.c:4389. */
int card_steelclaw_lance(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;
  unsigned int uVar4;
  unsigned int uVar5;
  target_definition_t *td_00;
  const char *prompt;
  target_definition_t *ptStack_c0;
  target_definition_t td;
  target_definition_t td1;

  pcVar1 = get_card_instance(player, card);
  if (event - EVENT_POWER < 2)
  {
    pcVar3 = in_play(player, card);
    if ((((pcVar3 != NULL) && (pcVar1->damage_target_card == affected_card)) &&
         (pcVar1->damage_target_player == affected_card_controller)) &&
        (pcVar1 = is_equipping(player, card), pcVar1 != NULL))
    {
      event_result = event_result + 2;
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    td_00 = &td;
    default_target_definition(player, card, td_00, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    default_target_definition(player, card, &td1, 2);
    td1.required_subtype = 0x67;
    td1.allowed_controller = player;
    td1.preferred_controller = player;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x100, 3, 0, 0, 0, 0, 0,
                                         0, td_00, NULL);
      if (result != 0)
      {
        return 1;
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x100, 1, 0, 0, 0, 0, 0,
                                         0, &td1, NULL);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {

      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 3, 0, 0, 0, 0, 0, 0,
                                         td_00, NULL);
      uVar5 = 0;
      if (result != 0)
      {
        result = can_sorcery_be_played(player, EVENT_ACTIVATE);
        uVar5 = (unsigned int)(result != 0);
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 1, 0, 0, 0, 0, 0, 0,
                                         &td1, NULL);
      uVar4 = 0;
      if (result != 0)
      {
        result = can_sorcery_be_played(player, EVENT_ACTIVATE);
        uVar4 = (unsigned int)(result != 0);
      }
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "Normal equip",
                           uVar5, 1, "Knight equip", uVar4, 5, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 1)
      {
        prompt = "Select target creature you control.";
        result = 3;
      LAB_025d4208:
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x102, result, 0, 0, 0, 0,
                                           0, 0, td_00, prompt);
        return result;
      }
      if (result == 2)
      {
        prompt = "Select target Knight you control.";
        result = 1;
        td_00 = &td1;
        goto LAB_025d4208;
      }
      goto LAB_025d3f79;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    default_target_definition(player, card, &td1, 2);
  }
  ptStack_c0 = &td1;
  td1.allowed_controller = player;
  td1.required_subtype = 0x67;
  td1.preferred_controller = td1.allowed_controller;
  if (event != EVENT_RESOLVE_ACTIVATION)
    goto LAB_025d3f79;
  result = pcVar1->info_slot;
  if (result == 1)
  {
    result = validate_target(player, card, &td, 0);
    if (result == 0)
    {
      result = pcVar1->info_slot;
      goto LAB_025d436a;
    }
  }
  else
  {
  LAB_025d436a:
    if ((result != 2) || (result = validate_target(player, card, ptStack_c0, 0), result == 0))
      goto LAB_025d3f79;
  }
  equip_target_creature(pcVar1->parent_controller, pcVar1->parent_card, pcVar1->targets[0].player,
                        pcVar1->targets[0].card);
LAB_025d3f79:
  result = generic_activated_ability(player, card, event, 0x102, 3, 0, 0, 0, 0, 0, 0, &td,
                                     "Select target creature you control.");
  return result;
}

/* ManalinkEh 2022: 0x25d44d0; original cards/throne_of_eldraine.c:4469. */
int card_stomp(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x102);
    goto LAB_025d458d;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025d45dc;
    default_target_definition(player, card, &td, 0x102);
  LAB_025d4624:
    td.zone = 0x1200;
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025d4544;
  }
  else
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0x102);
    }
    else
    {
      if (event == EVENT_CHANGE_TARGET)
      {
        default_target_definition(player, card, &td, 0x102);
        td.zone = 0x1200;
        goto LAB_025d4544;
      }
    LAB_025d45dc:
      if ((event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
      {
        return 0;
      }
      default_target_definition(player, card, &td, 0x102);
      if (event == EVENT_CAST_SPELL)
        goto LAB_025d4624;
    }
    td.zone = 0x1200;
    if (event != EVENT_CAN_CHANGE_TARGET)
    {
      if ((event == EVENT_RESOLVE_SPELL) && (result = valid_target(&td), result != 0))
      {
        damage_cannot_be_prevented_until_eot(player, card);
        damage_target0(player, card, 2);
      }
    LAB_025d4544:
      result = adventure(player, card, event, 0x5034);
      return result;
    }
  }
LAB_025d458d:
  td.zone = 0x1200;
  result = generic_spell(player, card, event, 1, &td, "TARGET_ANY", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25d48b0; original cards/throne_of_eldraine.c:4547. */
int card_swift_end(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x102);
    goto LAB_025d495e;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025d49ac;
    default_target_definition(player, card, &td, 0x102);
  LAB_025d49ed:
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025d4919;
  }
  else
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0x102);
    }
    else
    {
      if (event == EVENT_CHANGE_TARGET)
      {
        default_target_definition(player, card, &td, 0x102);
        goto LAB_025d4919;
      }
    LAB_025d49ac:
      if ((event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
      {
        return 0;
      }
      default_target_definition(player, card, &td, 0x102);
      if (event == EVENT_CAST_SPELL)
        goto LAB_025d49ed;
    }
    if (event != EVENT_CAN_CHANGE_TARGET)
    {
      if ((event == EVENT_RESOLVE_SPELL) && (result = valid_target(&td), result != 0))
      {

        instance = get_card_instance(player, card);
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        lose_life(player, 2);
      }
    LAB_025d4919:
      result = adventure(player, card, event, 0x50c6);
      return result;
    }
  }
LAB_025d495e:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE_OR_PLANESWALKER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25d4ac0; original cards/throne_of_eldraine.c:4572. */
int card_syr_alin_the_lions_claw(int player, int card, event_t event)
{
  test_definition_t test;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  new_default_test_definition(&test, 2, "");
  test.not_me = 1;
  pump_creatures_until_eot(player, card, player, 0, 1, 1, 0, 0, &test);
  return 0;
}

/* ManalinkEh 2022: 0x25d4e10; original cards/throne_of_eldraine.c:4662. */
int card_syr_faren_the_hengehammer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }

  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.allow_cancel = 0;
  td.required_state = 2;
  td.special = 0x4000;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "Select another target attacking creature.", 0, 0x80);
    if (result != 0)
    {

      result = get_power(player, card);
      pump_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card, result,
                     result);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x25d59f0; original cards/throne_of_eldraine.c:5000. */
int card_torbran_thane_of_red_fell(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *instance;
  unsigned int uVar3;
  int result;

  check_legend_rule(player, card, event);
  instance = damage_being_dealt(event);
  if ((instance != NULL) && (result = (int)instance->damage_source_player, result == player))
  {
    cVar1 = instance->initial_color;
    uVar3 = get_sleighted_color_test(result, card, 0x10);
    if (((int)cVar1 & uVar3) != 0)
    {
      result = is_humiliated_for_ability(result, card, AL_STATIC);
      if (result == 0)
      {
        instance->info_slot_fn = instance->info_slot_fn + 2;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d5d90; original cards/throne_of_eldraine.c:5164. */
int card_treats_to_share(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAN_CAST)
    goto LAB_025d5dc1;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_025d5de4;
  LAB_025d5e0d:
    if (player == affected_card_controller)
    {
    LAB_025d5dc1:
      result = basic_spell(player, card, event);
      return result;
    }
  }
  else
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event == EVENT_CAN_CHANGE_TARGET)
        goto LAB_025d5dc1;
      if (event == EVENT_CHANGE_TARGET)
        goto LAB_025d5e18;
    LAB_025d5de4:
      if ((event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
      {
        return 0;
      }
      if (event == EVENT_CAST_SPELL)
      {
        if (affected_card != card)
          goto LAB_025d5e18;
        goto LAB_025d5e0d;
      }
    }
    if (event == EVENT_CAN_CHANGE_TARGET)
      goto LAB_025d5dc1;
    if (event == EVENT_RESOLVE_SPELL)
    {
      generate_token_by_id(player, card, 0x5077);
    }
  }
LAB_025d5e18:
  result = adventure(player, card, event, 0x504d);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25d18c0; original cards/throne_of_eldraine.c:3131. */
int card_lochmere_serpent(int player, int card, event_t event)
{
  int32_t *piVar1;
  card_instance_t *instance;
  const char *pcVar3;
  int iVar4;
  int iVar5;
  uint32_t uVar6;
  int iVar7;
  target_t *ret_location;
  test_definition_t t_is;
  test_definition_t t_sw;

  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    iVar4 = has_mana_multi(player, 0, 1, 1, 0, 0, 0);
    if (((iVar4 != 0) &&
         (iVar4 = can_sorcery_be_played(player, EVENT_GRAVEYARD_ABILITY), iVar4 != 0)) &&
        (iVar4 = count_graveyard(1 - player), 4 < iVar4))
    {
      return 4;
    }
  }
  else
  {
    if (event == EVENT_PAY_FLASHBACK_COSTS)
    {

      new_default_test_definition(&t_sw, 0xff, "Select a card to exile.");
      instance = get_card_instance(player, card);
      charge_mana_for_activated_ability(player, card, 0, 1, 1, 0, 0, 0);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      ret_location = instance->targets;
      select_multiple_cards_from_graveyard(player, 1 - player, -1, 1, &t_sw, 5, ret_location);
      if (spell_fizzled != 1)
      {
        do
        {
          piVar1 = &ret_location->player;

          ret_location = ret_location + 1;
          rfg_card_from_grave(1 - player, *piVar1);
        } while (ret_location != instance->targets + 5);
        return 1;
      }
      goto LAB_025d1c84;
    }
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      goto LAB_025d18fc;
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {
  LAB_025d18fc:
    if (stack_size == 0)
    {
      instance = get_card_instance(player, card);
      instance->targets[4].player = 0;
    }
    instance = get_card_instance(player, card);
    pcVar3 = get_hacked_land_text(player, card, "Select %a to sacrifice.", 0x5002);
    new_default_test_definition(&t_is, 0xff, pcVar3);
    t_is.subtype = 0x5002;
    pcVar3 = get_hacked_land_text(player, card, "Select %a to sacrifice.", 0x5009);
    new_default_test_definition(&t_sw, 0xff, pcVar3);
    t_sw.subtype = 0x5009;
    iVar4 = generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_CAN_ACTIVATE,
                                                             GAA_NONE, 0, 0, 1, 0, 0, 0, 0, NULL,
                                                             NULL, TYPE_NONE, &t_is, SARV_0);
    iVar5 = generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_CAN_ACTIVATE,
                                                             GAA_NONE, 0, 1, 0, 0, 0, 0, 0, NULL,
                                                             NULL, TYPE_NONE, &t_sw, SARV_0);
    if (((current_turn == player) && (current_phase < 0x17)) &&
        ((iVar7 = check_for_special_ability(player, card, 0x10),
          iVar7 == 0 && ((instance->targets[4].player & 1) == 0))))
    {
      uVar6 = 5;
    }
    else
    {
      uVar6 = 0;
    }
    iVar4 = dialog_impl(player, card, event, "\001\002", "Unblockable", iVar4, uVar6,
                        "Gain life & draw", iVar5, 5, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      return iVar4;
    }
    if (event == EVENT_ACTIVATE)
    {
      if (iVar4 == 1)
      {
        generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_ACTIVATE, GAA_NONE, 0,
                                                         0, 1, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
                                                         &t_is, SARV_0);
        if (spell_fizzled != 1)
        {
          instance->targets[4].player = instance->targets[4].player | 1;
          return 0;
        }
      }
      else if (iVar4 == 2)
      {
        generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_ACTIVATE, GAA_NONE, 0,
                                                         1, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
                                                         &t_sw, SARV_0);
        return 0;
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (iVar4 == 1)
      {
        pump_ability_until_eot_no_repeat_2022(player, card, instance->parent_controller,
                                         instance->parent_card, 0, 0x10);
        return 0;
      }
      if (iVar4 == 2)
      {
        gain_life(player, 1);
        draw_a_card(player);
        return 0;
      }
    }
    return 0;
  }
LAB_025d1c84:
  iVar4 = flash(player, card, event);
  return iVar4;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25d0d20; original cards/throne_of_eldraine.c:2845. */
int card_kenrith_the_returned_king(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint32_t uVar6;
  card_instance_t *instance;
  target_definition_t *td;
  const char *prompt;
  target_definition_t td_crit;
  target_definition_t td_play;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (((event != EVENT_ACTIVATE) && (1 < event - EVENT_RESOLVE_ACTIVATION)) &&
      (event != EVENT_CLEANUP))
    goto LAB_025d0d5c;
  if (stack_size == 0)
  {
    instance = get_card_instance(player, card);
    instance->targets[4].player = 0;
  }
  default_target_definition(player, card, &td_crit, 2);
  default_target_definition(player, card, &td_play, 0);
  td_play.zone = 0x1000;
  new_default_test_definition(&test, 2, "Select a creature card.");
  iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 1, 0, 0, NULL,
                                    NULL);
  iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 1, 0, 0, 1, 0, 0, 0,
                                    &td_crit, "TARGET_CREATURE");
  iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 2, 0, 0, 0, 0, 1, 0,
                                    &td_play, "TARGET_PLAYER");
  iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 3, 0, 1, 0, 0, 0, 0,
                                    &td_play, "TARGET_PLAYER");
  iVar5 = generic_activated_ability_with_target_in_grave(player, card, EVENT_CAN_ACTIVATE, GAA_NONE,
                                                         4, 1, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
                                                         &test, 0x1000);
  uVar6 = 0xffffffec;
  if (current_turn == player)
  {
    if ((current_phase < 0x15) &&
        (instance = get_card_instance(player, card), (instance->targets[4].player & 1) == 0))
    {
      uVar6 = 5;
    }
    else
    {
      uVar6 = 0;
    }
  }
  iVar1 = dialog_impl(player, card, event, "\001\002", "Trample & Haste", iVar1, uVar6,
                      "+1/+1 counter", iVar2, 5, "Gain 5 life", iVar3, 5, "Target player draws",
                      iVar4, 5, "Reanimate", iVar5, 5, 0, 0, 0, 0);
  if (event == EVENT_CAN_ACTIVATE)
  {
    return iVar1;
  }
  if (event == EVENT_ACTIVATE)
  {
    switch (iVar1)
    {
    default:
      goto LAB_025d0d5c;
    case 1:
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 0, 0, 0, 0, 1, 0, 0, NULL, NULL);
      instance = get_card_instance(player, card);
      instance->targets[4].player = instance->targets[4].player | 1;
      return 0;
    case 2:
      td = &td_crit;
      prompt = "TARGET_CREATURE";
      iVar4 = 0;
      iVar3 = 1;
      iVar2 = 0;
      iVar1 = 1;
      break;
    case 3:
      td = &td_play;
      prompt = "TARGET_PLAYER";
      iVar4 = 1;
      iVar3 = 0;
      iVar2 = 0;
      iVar1 = 2;
      break;
    case 4:
      td = &td_play;
      prompt = "TARGET_PLAYER";
      iVar4 = 0;
      iVar3 = 0;
      iVar2 = 1;
      iVar1 = 3;
      break;
    case 5:
      generic_activated_ability_with_target_in_grave(player, card, EVENT_ACTIVATE, GAA_NONE, 4, 1,
                                                     0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test,
                                                     0x1000);
      return 0;
    }
    generic_activated_ability(player, card, EVENT_ACTIVATE, 4, iVar1, 0, iVar2, iVar3, 0, iVar4, 0,
                              td, prompt);
    return 0;
  }
  if (event != EVENT_RESOLVE_ACTIVATION)
    goto LAB_025d0d5c;

  instance = get_card_instance(player, card);
  iVar2 = instance->targets[0].player;
  instance = get_card_instance(player, card);
  switch (iVar1)
  {
  case 1:
    pump_creatures_until_eot(player, card, 2, 0, 0, 0, 0x80, 4, NULL);
    break;
  case 2:
    add_counters_from_source(player, card, iVar2, instance->targets[0].card, COUNTER_P1_P1, 1);
    break;
  case 3:
    gain_life(iVar2, 5);
    break;
  case 4:
    draw_a_card(iVar2);
    break;
  case 5:

    iVar1 = validate_target_from_grave_source(player, card, iVar2, 1);
    if (iVar1 != -1)
    {
      reanimate_permanent(player, card, iVar2, iVar1, REANIMATE_UNDER_OWNER_CONTROL);
    }
  default:
  LAB_025d0d5c:
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d14b0; original cards/throne_of_eldraine.c:3028. */
int card_korvold_fae_cursed_king(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int iVar3;
  int iVar4;
  test_definition_t test;

  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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

          if ((((instance != NULL) && (iVar3 = is_what(player, iVar4, 0x80), iVar3 != 0)) &&
               (instance = get_card_instance(player, iVar4),
                instance->info_slot_fn == korvold_fae_cursed_king_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_025d160c;
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[player]);
      }

      iVar4 = create_targetted_legacy_effect(player, card, korvold_fae_cursed_king_effect, player,
                                             card);
      instance = get_card_instance(player, iVar4);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar4, 0x10000);
    }
  LAB_025d160c:
    dat_ability(player, card, EVENT_STATIC_EFFECTS, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  }
  else
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    if ((event == EVENT_ETB) || (event == EVENT_DAT_ABILITY))
    {
      new_default_test_definition(&test, 0x47, "Select a permanent to sacrifice.");
      test.not_me = 1;
      new_sacrifice(player, card, player, SAC_NO_CANCEL, &test);
    }
    if ((event == EVENT_GRAVEYARD_FROM_PLAY) &&
        (instance = get_card_instance(affected_card_controller, affected_card),
         instance->kill_code == '\x03'))
    {
      count_for_gfp_ability(player, card, 0x77, player, 0x47, NULL);
    }
  }
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar2 != 0))
  {

    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, uVar2 & 0xff);
    draw_cards(player, uVar2 & 0xff);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d57c0; original cards/throne_of_eldraine.c:4894. */
int card_the_great_henge(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    result = get_highest_power(player);
    COST_COLORLESS = COST_COLORLESS - (char)result;
    if (trigger_condition != 0xdb)
      goto LAB_025d580a;
  LAB_025d5872:
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_test_definition(&this_test, 2);
      this_test.type_flag = 3;
      this_test.not_me = 1;
      etb_ability_global(player, event, 2, TYPE_NONE, &this_test, player, card);
    }
  }
  else if (trigger_condition == 0xdb)
    goto LAB_025d5872;
  if (event == EVENT_ETB)
  {
    add_counters_from_source(player, card, trigger_cause_controller, trigger_cause, COUNTER_P1_P1,
                             1);
    draw_a_card(player);
  }
  else if (event == EVENT_ACTIVATE)
  {
    gain_life(player, 2);
  }
LAB_025d580a:
  result = artifact_mana_all_one_color(player, card, event, 2, 0);
  return result;
}

/* ManalinkEh 2022: 0x25d5920; original cards/throne_of_eldraine.c:4927. */
int card_the_magic_mirror(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_MODIFY_COST)
  {
    result = count_graveyard_by_type(player, 0x38);
    COST_COLORLESS = COST_COLORLESS - (char)result;
  }
  else if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    add_counters_from_source(player, card, player, card, COUNTER_LORE, 1);
    result = count_counters(player, card, COUNTER_LORE);
    draw_cards(player, result);
  }
  result = card_spellbook(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25d65d0; original cards/throne_of_eldraine.c:5470. */
int card_yorvo_lord_of_garenbrig(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  test_definition_t test;

  check_legend_rule(player, card, event);
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, 4);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 2);
    test.color = get_sleighted_color_test(player, card, 8);
    test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if ((event == EVENT_ETB) && (trigger_cause != card))
  {
    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    iVar1 = get_power(trigger_cause_controller, trigger_cause);
    iVar2 = get_power(player, card);
    if (iVar2 < iVar1)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int trail_of_crumbs_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25cbd70; original cards/throne_of_eldraine.c:787. */
int card_castle_ardenvale(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_CAST_SPELL)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (result = check_state(player, card, 0x10), result == 0))
    {
      result = get_hacked_subtype(player, card, 0x5007);
      result = check_battlefield_for_subtype(player, -1, result);
      if (result == 0)
      {
        instance = get_card_instance(player, card);
        instance->state = instance->state | 0x10;
      }
    }
  }
  else if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
      result = paying_mana();
      if (((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1,
                                                                2, 0, 0, 0, 0, 2, 0, NULL, NULL),
                             result != 0)) &&
          (result = do_dialog(player, player, card, -1, -1,
                              " Generate mana\n Create a Human\n Cancel", 1),
           result != 0))
      {
        if (result != 1)
        {
          return 0;
        }
        add_state(player, card, 0x10);
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 2, 0, 0, 0, 0, 2, 0,
                                           NULL, NULL);
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
        generate_token_by_id(player, card, 0x494c);
      }
      return 0;
    }
  }

  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25cc020; original cards/throne_of_eldraine.c:839. */
int card_castle_embereth(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_CAST_SPELL)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (result = check_state(player, card, 0x10), result == 0))
    {
      result = get_hacked_subtype(player, card, 0x5006);
      result = check_battlefield_for_subtype(player, -1, result);
      if (result == 0)
      {
        instance = get_card_instance(player, card);
        instance->state = instance->state | 0x10;
      }
    }
  }
  else if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
      result = paying_mana();
      if (((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1,
                                                                1, 0, 0, 0, 2, 0, 0, NULL, NULL),
                             result != 0)) &&
          (result = do_dialog(player, player, card, -1, -1,
                              " Generate mana\n Pump your creatures\n Cancel", 1),
           result != 0))
      {
        if (result != 1)
        {
          return 0;
        }
        add_state(player, card, 0x10);
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 1, 0, 0, 0, 2, 0, 0,
                                           NULL, NULL);
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
        pump_creatures_until_eot_merge_pt(player, card, player, 1, 0, NULL);
      }
      return 0;
    }
  }

  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25cc2e0; original cards/throne_of_eldraine.c:900. */
int card_castle_locthwain(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_CAST_SPELL)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (result = check_state(player, card, 0x10), result == 0))
    {
      result = get_hacked_subtype(player, card, 0x5009);
      result = check_battlefield_for_subtype(player, -1, result);
      if (result == 0)
      {
        instance = get_card_instance(player, card);
        instance->state = instance->state | 0x10;
      }
    }
  }
  else if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
      result = paying_mana();
      if (((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1,
                                                                1, 2, 0, 0, 0, 0, 0, NULL, NULL),
                             result != 0)) &&
          (result =
               do_dialog(player, player, card, -1, -1, " Generate mana\n Draw a card\n Cancel", 1),
           result != 0))
      {
        if (result != 1)
        {
          return 0;
        }
        add_state(player, card, 0x10);
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 1, 2, 0, 0, 0, 0, 0,
                                           NULL, NULL);
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
        draw_a_card(player);
        lose_life(player, hand_count[player]);
      }
      return 0;
    }
  }

  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25cc5a0; original cards/throne_of_eldraine.c:953. */
int card_castle_vantress(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_CAST_SPELL)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (result = check_state(player, card, 0x10), result == 0))
    {
      result = get_hacked_subtype(player, card, 0x5002);
      result = check_battlefield_for_subtype(player, -1, result);
      if (result == 0)
      {
        instance = get_card_instance(player, card);
        instance->state = instance->state | 0x10;
      }
    }
  }
  else if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
      result = paying_mana();
      if (((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1,
                                                                2, 0, 2, 0, 0, 0, 0, NULL, NULL),
                             result != 0)) &&
          (result = do_dialog(player, player, card, -1, -1, " Generate mana\n Scry 2\n Cancel", 1),
           result != 0))
      {
        if (result != 1)
        {
          return 0;
        }
        add_state(player, card, 0x10);
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 2, 0, 2, 0, 0, 0, 0,
                                           NULL, NULL);
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
        scry(player, 2);
      }
      return 0;
    }
  }

  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25cfc20; original cards/throne_of_eldraine.c:2366. */
int card_gilded_goose(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  test_definition_t test_food;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
  LAB_025cfdd8:
    generate_token_by_id(player, card, 0x5077);
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    if (instance->info_slot != 1)
    {
      new_default_test_definition(&test_food, 0xff, "Select a Food to sacrifice");
      goto LAB_025cfc9a;
    }
    goto LAB_025cfdd8;
  }
  new_default_test_definition(&test_food, 0xff, "Select a Food to sacrifice");
  test_food.subtype = 0x3008;
  if (event == EVENT_COUNT_MANA)
  {
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
    iVar2 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
    if (iVar2 == 0)
    {
      return 0;
    }
    iVar2 = new_can_sacrifice_as_cost(player, card, &test_food);
    if (iVar2 == 0)
    {
      return 0;
    }
    declare_mana_available_hex(player, 0x3f, 1);
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      if ((iVar2 != 0) && (iVar2 = new_can_sacrifice_as_cost(player, card, &test_food), iVar2 != 0))
      {
        return 1;
      }
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 1, 0, 0, 1, 0, 0, 0,
                                        NULL, NULL);
      return (unsigned int)(iVar2 != 0);
    }
    if (event != EVENT_ACTIVATE)
    {
      return 0;
    }

    instance->info_slot = 0;
    iVar2 = paying_mana();
    if (iVar2 == 0)
    {

      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 1, 0, 0, 1, 0, 0, 0,
                                        NULL, NULL);
      iVar3 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      uVar4 = 0;
      if (iVar3 != 0)
      {
        iVar3 = new_can_sacrifice_as_cost(player, card, &test_food);
        uVar4 = (unsigned int)(iVar3 != 0);
      }
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Make a Food", iVar2, 5,
                          "Get Mana", uVar4, 5, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 1)
      {
        instance->state = instance->state | 0x10;
        generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 1, 0, 0, 1, 0, 0, 0, NULL, NULL);
        if (spell_fizzled != 1)
        {
          return 0;
        }
        instance->state = instance->state & 0xffffffef;
        return 0;
      }
      if (iVar2 != 2)
      {
        return 0;
      }
    }
    instance->state = instance->state | 0x10;
    iVar2 = new_sacrifice(player, card, player, SAC_AS_COST, &test_food);
    if (iVar2 != 0)
    {
      do
      {
        spell_fizzled = 0;
        produce_mana_all_one_color(player, COLOR_TEST_ANY_COLORED, 1);
      } while (spell_fizzled == 1);
      return 0;
    }
    spell_fizzled = 1;
    instance->state = instance->state & 0xffffffef;
    return 0;
  }
LAB_025cfc9a:
  test_food.subtype = 0x3008;
  if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d00f0; original cards/throne_of_eldraine.c:2478. */
int card_golden_egg(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int argument7;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_cards(player, 1);
    return 0;
  }
  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    instance = get_card_instance(player, card);
  LAB_025d0148:
    if ((event == EVENT_RESOLVE_ACTIVATION) && (instance->targets[1].player != 0))
    {
      gain_life(player, 3);
      return 0;
    }
    return 0;
  }
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = is_tapped(player, card);
    if ((((result == 0) && (result = is_animated_and_sick(player, card), result == 0)) &&
         (result = can_produce_mana(player, card), result != 0)) &&
        (result = has_mana(player, 7, 1), result != 0))
    {
      return 1;
    }
    result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 2, 0, 0, 0, 0, 0, 0,
                                       NULL, NULL);
    return result;
  }
  if (event != EVENT_ACTIVATE)
    goto LAB_025d0148;

  instance->targets[1].player = 0;
  result = is_tapped(player, card);
  if (((result != 0) || (result = is_animated_and_sick(player, card), result != 0)) ||
      ((result = can_produce_mana(player, card),
        result == 0 || (result = has_mana(player, 7, 1), result == 0))))
  {
  LAB_025d01b1:
    spell_fizzled = 1;
    ai_modifier = ai_modifier + -100;
    return 0;
  }
  result = paying_mana();
  if ((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 2,
                                                           0, 0, 0, 0, 0, 0, NULL, NULL),
                        result != 0))
  {

    argument7 = 0;
    if (life[player] < 6)
    {
      result = count_subtype(player, 1, -1);
      argument7 = (unsigned int)(5 < result);
    }
    result = do_dialog(player, player, card, -1, -1, " Generate mana\n Sac & gain life\n Cancel",
                       argument7);
    if (result != 0)
    {
      if (result != 1)
      {
        if (result != 2)
        {
          return 0;
        }
        goto LAB_025d01b1;
      }
      add_state(player, card, 0x10);
      charge_mana(player, COLOR_COLORLESS, 2);
      if (spell_fizzled != 1)
      {
        tapped_for_mana_color = -1;
        instance->targets[1].player = 1;
        goto LAB_025d048e;
      }
      goto LAB_025d036a;
    }
  }
  add_state(player, card, 0x10);
  charge_mana(player, COLOR_COLORLESS, 1);
  if ((spell_fizzled != 1) &&
      (result = produce_mana_tapped_all_one_color_2022(player, card, COLOR_TEST_ANY_COLORED, 1),
       result != 0))
  {
  LAB_025d048e:
    kill_card(player, card, KILL_SACRIFICE);
    return 0;
  }
LAB_025d036a:
  untap_card_no_event(player, card);
  return 0;
}

/* ManalinkEh 2022: 0x25c9450; original cards/throne_of_eldraine.c:5026. */
static int trail_of_crumbs_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  unsigned int uVar3;
  card_instance_t *pcVar4;
  test_definition_t test_food;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x5130))
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
          default_test_definition(&test_food, 0xff);
          test_food.subtype = 0x3008;
          count_for_gfp_ability(player, card, 0x77, player, 0, &test_food);
        }
      }
    }
  }
  result = duh_mode(player);
  uVar3 = resolve_gfp_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if ((uVar3 != 0) && (result = check_status(player, card, 0x10000), result == 0))
  {

    uVar3 = uVar3 & 0xff;
    while (((uVar3 != 0 && (result = has_mana(player, 0, 1), result != 0)) &&
            (result = charge_mana_while_resolving(player, card, event, player, COLOR_COLORLESS, 1),
             result != 0)))
    {
      new_default_test_definition(&test_food, 0x47, "Select a permanent card.");
      reveal_top_cards_of_library_and_choose(player, card, player, 2, 1, TUTOR_HAND, 1,
                                             TUTOR_BOTTOM_OF_DECK, 0, &test_food);
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

/* ManalinkEh 2022: 0x25d5a80; original cards/throne_of_eldraine.c:5089. */
int card_trail_of_crumbs(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  test_definition_t test_food;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    generate_token_by_id(player, card, 0x5077);
  }
  else if (event == EVENT_STATIC_EFFECTS)
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
                instance->info_slot_fn == trail_of_crumbs_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_025d5acd;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, trail_of_crumbs_effect, player, card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  }
  else if ((event == EVENT_GRAVEYARD_FROM_PLAY) &&
           (instance = get_card_instance(affected_card_controller, affected_card),
            instance->kill_code == '\x03'))
  {
    default_test_definition(&test_food, 0xff);
    test_food.subtype = 0x3008;
    count_for_gfp_ability(player, card, 0x77, player, 0, &test_food);
  }
LAB_025d5acd:
  instance = get_card_instance(player, card);
  if (instance->kill_code == '\0')
  {

    iVar2 = duh_mode(player);
    uVar4 = resolve_gfp_ability(player, card, event, (iVar2 != 0) + RESOLVE_TRIGGER_OPTIONAL);
    uVar4 = uVar4 & 0xff;
    while (((uVar4 != 0 && (iVar2 = has_mana(player, 0, 1), iVar2 != 0)) &&
            (iVar2 = charge_mana_while_resolving(player, card, event, player, COLOR_COLORLESS, 1),
             iVar2 != 0)))
    {
      new_default_test_definition(&test_food, 0x47, "Select a permanent card.");
      reveal_top_cards_of_library_and_choose(player, card, player, 2, 1, TUTOR_HAND, 1,
                                             TUTOR_BOTTOM_OF_DECK, 0, &test_food);
      uVar4 = uVar4 - 1;
    }
  }
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25cab30; original cards/throne_of_eldraine.c:319. */
int card_alela_artful_provocateur(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  deathtouch_2022(player, card, event);
  lifelink(player, card, event);
  if (event == EVENT_POWER)
  {
    if (affected_card_controller == player)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        instance = in_play(player, card);
        if (instance != NULL)
        {
          result = check_for_ability(affected_card_controller, affected_card, 0x20);
          if (result != 0)
          {
            event_result = event_result + 1;
          }
        }
      }
    }
    sp_ability_trigger(player, EVENT_POWER, 2, TYPE_ARTIFACT | TYPE_ENCHANTMENT, NULL, player,
                       card);
    return 0;
  }
  sp_ability_trigger(player, event, 2, TYPE_ARTIFACT | TYPE_ENCHANTMENT, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  generate_token_by_id(player, card, 0x492f);
  return 0;
}

/* ManalinkEh 2022: 0x25ccbf0; original cards/throne_of_eldraine.c:1118. */
int card_chulane_teller_of_tales(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  check_legend_rule(player, card, event);
  vigilance(player, card, event);
  sp_ability_trigger(player, event, 2, TYPE_CREATURE, NULL, player, card);
  if (event == EVENT_SPA)
  {
    draw_a_card(player);
    new_default_test_definition(&recovered_stack_td.test, 1, "Select a land card.");
    new_global_tutor(player, player, 4, 4, 0, 1, &recovered_stack_td.test);
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_025cccb1:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_025cccb6;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_025cccb1;
  }
  instance = get_card_instance(player, card);
  result = instance->targets[0].card;
  instance = get_card_instance(player, card);
  bounce_permanent(instance->targets[0].player, result);
LAB_025cccb6:
  default_target_definition(player, card, &recovered_stack_td.target, 2);
  recovered_stack_td.target.allowed_controller = player;
  result = generic_activated_ability(player, card, event, 5, 3, 0, 0, 0, 0, 0, 0,
                                     &recovered_stack_td.target, "TARGET_CREATURE_YOU_CONTROL");
  return result;
}

/* ManalinkEh 2022: 0x25d2040; original cards/throne_of_eldraine.c:3275. */
int card_lucky_clover(int player, int card, event_t event)
{
  test_definition_t test;

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0x38);
    test.subtype = 0x6002;
    sp_ability_trigger(1 - player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event != EVENT_SPA)
  {
    return 0;
  }
  copy_spell_from_stack(player, trigger_cause_controller, trigger_cause);
  return 0;
}

/* ManalinkEh 2022: 0x25d5540; original cards/throne_of_eldraine.c:4850. */
int card_the_circle_of_loyalty(int player, int card, event_t event)
{
  int result;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_test;

  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    result = count_subtype(player, -1, 0x67);
    COST_COLORLESS = COST_COLORLESS - (char)result;
    boost_creature_type(player, card, EVENT_MODIFY_COST, ~SUBTYPE_INVALID, 1, 1, 0,
                        BCT_CONTROLLER_ONLY);
    if (trigger_condition == 0xd3)
      goto LAB_025d5700;
  LAB_025d56b6:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_025d55d3;
    }
  }
  else
  {
    boost_creature_type(player, card, event, ~SUBTYPE_INVALID, 1, 1, 0, BCT_CONTROLLER_ONLY);
    if (trigger_condition == 0xd3)
    {
    LAB_025d5700:
      if ((card == affected_card) && (player == affected_card_controller))
      {
        default_test_definition(&recovered_stack_test.test, 0xff);
        recovered_stack_test.test.subtype = 0x2001;
        sp_ability_trigger(player, event, 2, TYPE_NONE, &recovered_stack_test.test, player, card);
      }
    }
    if (event == EVENT_SPA)
    {
      default_token_definition(player, card, 0x4958, &recovered_stack_test.token);
      recovered_stack_test.test.message[0x24] = '\b';
      recovered_stack_test.test.message[0x25] = '\0';
      recovered_stack_test.test.message[0x26] = '\0';
      recovered_stack_test.test.message[0x27] = '\0';
      generate_token(&recovered_stack_test.token);
      return 0;
    }
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_025d56b6;
  LAB_025d55d3:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_025d55dc;
  }
  default_token_definition(player, card, 0x4958, &recovered_stack_test.token);
  recovered_stack_test.test.message[0x24] = '\b';
  recovered_stack_test.test.message[0x25] = '\0';
  recovered_stack_test.test.message[0x26] = '\0';
  recovered_stack_test.test.message[0x27] = '\0';
  generate_token(&recovered_stack_test.token);
LAB_025d55dc:
  result = generic_activated_ability(player, card, event, 1, 3, 0, 0, 0, 0, 1, 0, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int acclaimed_contender_fn(int iid, int unused, int player, int unused3);

/* ManalinkEh 2022: 0x25c8fd0; original cards/throne_of_eldraine.c:284. */
static int acclaimed_contender_fn(int iid, int unused, int player, int unused3)
{
  int result;

  result = has_subtype_by_id_2022(player, (unsigned int)cards_data[iid].id, SUBTYPE_KNIGHT);
  if (((result == 0) &&
       (result = has_subtype_by_id_2022(player, (unsigned int)cards_data[iid].id, SUBTYPE_AURA),
        result == 0)) &&
      (result = has_subtype_by_id_2022(player, (unsigned int)cards_data[iid].id, SUBTYPE_EQUIPMENT),
       result == 0))
  {
    result = is_what(-1, iid, 0x40);
    if (result == 0)
    {
      return 0;
    }
    result = is_legendary(-1, iid);
    return (unsigned int)(result != 0);
  }
  return 1;
}

/* ManalinkEh 2022: 0x25caa30; original cards/throne_of_eldraine.c:296. */
int card_acclaimed_contender(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  test_definition_t search_test;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_test_definition(&test, 0x47);
    test.subtype = 0x67;
    test.not_me = 1;
    result = check_battlefield_for_special_card(player, card, player, 0, &test);
    if (result != 0)
    {
      default_test_definition(&search_test, 0xff);
      search_test.special_selection_function = acclaimed_contender_fn;
      reveal_top_cards_of_library_and_choose(player, card, player, 5, 0, TUTOR_HAND, 1,
                                             TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &search_test);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25c8f40; original cards/throne_of_eldraine.c:1253. */
static int dotm_selection(int iid, int value, int player, int card)
{
  int iVar1;
  int iVar2;

  iVar1 = is_what(-1, iid, 0x40);
  iVar2 = 1;
  if (iVar1 == 0)
  {
    iVar1 = is_what(-1, iid, 4);
    iVar2 = 0;
    if (iVar1 != 0)
    {
      iVar2 = has_subtype_by_id_2022(player, (unsigned int)cards_data[iid].id, SUBTYPE_AURA);
      return (unsigned int)(iVar2 == 0);
    }
  }
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25d0610; original cards/throne_of_eldraine.c:2576. */
int card_grumgully_the_generous(int player, int card, event_t event)
{
  enum
  {
    ENABLE_XTRIGGER_REPLACE_ETB = 128,
    XTRIGGER_REPLACE_ETB = 16780800
  };

  xtrigger_t xVar1;
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  enable_xtrigger_flags = enable_xtrigger_flags | ENABLE_XTRIGGER_REPLACE_ETB;
  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if ((((xVar1 == XTRIGGER_REPLACE_ETB) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    instance = in_play(player, card);
    if (((instance != NULL) && (trigger_cause_controller == player)) && (trigger_cause != card))
    {
      result = is_what(player, trigger_cause, 2);
      if (result != 0)
      {
        result = has_subtype(trigger_cause_controller, trigger_cause, SUBTYPE_HUMAN);
        if (result == 0)
        {
          result = check_special_flags2_2022(trigger_cause_controller, trigger_cause, 0x100);
          if (result == 0)
          {
            result = is_humiliated_for_ability(player, card, AL_STATIC);
            if (result == 0)
            {
              if (event == EVENT_TRIGGER)
              {
                event_result = event_result | 2;
              }
              else if (event == EVENT_RESOLVE_TRIGGER)
              {
                hack_silent_counters = hack_silent_counters + 1;
                add_counters_from_source(player, card, trigger_cause_controller, trigger_cause,
                                         COUNTER_P1_P1, 1);
                hack_silent_counters = hack_silent_counters + -1;
              }
            }
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d3970; original cards/throne_of_eldraine.c:4064. */
int card_robber_of_the_rich(int player, int card, event_t event)
{
  enum
  {
    MPCFE_ATTACK_WITH_ROGUE = 256
  };

  int result;
  int player_00;

  haste(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);

  if ((((event == EVENT_DAT_ABILITY) &&
        (player_00 = 1 - current_turn, hand_count[current_turn] < hand_count[player_00])) &&
       (result = *deck_ptr[player_00], result != -1)) &&
      ((cards_data[result].type & 1) == 0))
  {
    rfg_top_card_of_deck(player_00);
    create_may_play_card_from_exile_effect(player, card, 1 - current_turn,
                                           (unsigned int)cards_data[result].id,
                                           MPCFE_ATTACK_WITH_ROGUE | MPCFE_FOR_CMC);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int has_adventure(int iid, int unused, int player, int card);

/* ManalinkEh 2022: 0x25c90e0; original cards/throne_of_eldraine.c:1518. */
static int has_adventure(int iid, int unused, int player, int card)
{
  enum
  {
    QUERY_ADVENTURER = 1
  };

  int result;

  result = query(player, card, iid, QUERY_ADVENTURER);
  return (unsigned int)(result != 0);
}

/* ManalinkEh 2022: 0x25cdca0; original cards/throne_of_eldraine.c:1526. */
int card_edgewall_innkeeper(int player, int card, event_t event)
{
  test_definition_t test;

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 2);
    test.special_selection_function = has_adventure;
    sp_ability_trigger(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event != EVENT_SPA)
  {
    return 0;
  }
  draw_a_card(player);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25cb890; original cards/throne_of_eldraine.c:669. */
int card_bonecrusher_giant(int player, int card, event_t event)
{
  const target_t *ptVar1;
  int result;

  ptVar1 = becomes_target_of_spell(player, card, event, player, card, 2, RESOLVE_TRIGGER_MANDATORY);
  if (ptVar1 != NULL)
  {
    damage_player(ptVar1->player, 2, player, card);
  }
  result = generic_adventurer(player, card, event, 5, 10, 0x5035);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25d4f20; original cards/throne_of_eldraine.c:4701. */
int card_syr_konrad_the_grim(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  instance = in_play(player, card);
  if ((instance == NULL) ||
      (result = is_humiliated_for_ability(player, card, AL_TRIG), result != 0))
  {
  LAB_025d4f71:
    if (event == EVENT_ACTIVATE)
      goto LAB_025d4fbb;
  }
  else
  {

    default_test_definition(&this_test, 2);
    this_test.not_me = 1;
    uVar3 = whenever_type_is_put_into_graveyard_from_anywhere(player, card, event, player, 0,
                                                              &this_test);

    if ((uVar3 != 0) && (uVar3 = uVar3 & 0xff, uVar3 != 0))
    {
      do
      {
        damage_player(1 - player, 1, player, card);
        uVar3 = uVar3 - 1;
      } while (0 < (int)uVar3);
    }
    if (event != EVENT_REMOVED_FROM_GRAVEYARD)
      goto LAB_025d4f71;
    if ((affected_card_controller == player) && (((unsigned int)event_result >> 16 & 0xff) != '\0'))
    {
      damage_player(1 - player, (unsigned int)event_result >> 0x10, player, card);
    }
  }
  if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    mill(current_turn, 1);
    mill(current_turn ^ 1, 1);
  }
LAB_025d4fbb:
  result = generic_activated_ability(player, card, event, 0, 1, 1, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25d6010; original cards/throne_of_eldraine.c:5299. */
int card_wicked_wolf(int player, int card, event_t event)
{
  int t_card;
  int result;
  card_instance_t *instance;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_test_food;

  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &recovered_stack_test_food.target, 2);
    recovered_stack_test_food.target.allowed_controller = (int32_t)(1 - player);
    result = can_target(&recovered_stack_test_food.target);
    if ((result != 0) &&
        (result = new_pick_target(&recovered_stack_test_food.target,
                                  "Select target creature you don\'t control.", 0, 0x80),
         result != 0))
    {
      instance = get_card_instance(player, card);
      result = instance->targets[0].card;
      instance->number_of_targets = '\0';
      fight(player, card, instance->targets[0].player, result);
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_025d6121:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_025d612a;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_025d6121;
  }
  instance = get_card_instance(player, card);
  result = instance->parent_controller;
  instance = get_card_instance(player, card);
  t_card = instance->parent_card;
  add_counters_from_source(player, card, result, t_card, COUNTER_P1_P1, 1);
  pump_ability_until_eot_no_repeat(player, card, result, t_card, 0, 0x800);
  tap_card(result, t_card);
LAB_025d612a:
  new_default_test_definition(&recovered_stack_test_food.test, 0xff, "Select a Food to sacrifice");
  recovered_stack_test_food.test.subtype = 0x3008;
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
      &recovered_stack_test_food.test, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x25d4b90; original cards/throne_of_eldraine.c:4592. */
/* The prefix at 0x738f34 is an engine-owned runtime string buffer. */
int card_syr_carah_the_bold(int player, int card, event_t event)
{
  target_t *ptVar1;
  int32_t *piVar2;
  int32_t iVar3;
  target_t *ptVar4;
  card_instance_t *pcVar5;
  card_instance_t *pcVar6;
  int iVar7;
  int iVar8;
  int amts[10];
  target_definition_t td;
  char prompt[200];

  pcVar5 = damage_being_dealt(event);
  if ((((pcVar5 != NULL) && (pcVar5->damage_source_player == player)) &&
       (((pcVar5->targets[3].player & 0x18) != 0 || (pcVar5->damage_source_card == card)))) &&
      ((pcVar5->damage_target_card == -1 &&
        (iVar8 = damage_is_to_planeswalker(pcVar5), iVar8 == 0))))
  {
    pcVar6 = get_card_instance(player, card);
    iVar8 = pcVar6->info_slot;
    pcVar6->number_of_targets = '\0';
    if (iVar8 < 10)
    {
      pcVar6->info_slot = iVar8 + 1;
      pcVar6->targets[iVar8].card = pcVar5->info_slot;
    }
  }
  if ((((trigger_condition == 0xd7) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      ((reason_for_trigger_controller == player &&
        (pcVar5 = get_card_instance(player, card), 0 < pcVar5->info_slot))))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result = event_result | 2;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      iVar8 = 0;
      default_target_definition(player, card, &td, 2);
      td.allow_cancel = 0;
      td.allowed_controller = 1 - player;
      pcVar5->number_of_targets = '\0';
      if (0 < pcVar5->info_slot)
      {
        do
        {
          iVar7 = can_target(&td);
          if (iVar7 == 0)
            break;
          iVar3 = pcVar5->targets[iVar8].card;
          if (ai_is_speculating == 1)
          {
            prompt[0] = '\0';
          }
          else
          {
            load_text_2022(NULL, "TARGET_CREATURE_OPPONENT_CONTROLS");
            iVar7 = get_id(player, card);
            scnprintf(prompt, 200, "%s %s (%d): %s", (const char *)0x738f34,
                      cards_ptr[iVar7]->full_name, iVar3, text_lines[0]);
          }
          iVar7 = pick_next_target_noload(&td, prompt);
          if (iVar7 != 0)
          {
            amts[pcVar5->number_of_targets - 1] = iVar3;
          }
          iVar8 = iVar8 + 1;
        } while (iVar8 < pcVar5->info_slot);
      }
      iVar8 = 0;
      pcVar5->info_slot = 0;
      if (pcVar5->number_of_targets != '\0')
      {
        do
        {
          piVar2 = amts + iVar8;
          ptVar4 = pcVar5->targets + iVar8;
          ptVar1 = pcVar5->targets + iVar8;
          iVar8 = iVar8 + 1;
          damage_creature(ptVar1->player, ptVar4->card, *piVar2, player, card);
        } while (iVar8 < (int)(unsigned int)pcVar5->number_of_targets);
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25ce410; original cards/throne_of_eldraine.c:1720. */
int card_fabled_passage(int player, int card, event_t event)
{
  color_test_t col;
  unsigned int uVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  color_t color;
  bool bVar5;
  test_definition_t this_test;

  if (event == EVENT_RESOLVE_SPELL)
  {
    play_land_sound_effect(player, card);
    iVar4 = mana_colors_added[player];
  joined_r0x025ce6ad:
    if (iVar4 == 0)
    {
      return 0;
    }
    iVar4 = mana_producer(player, card, event);
  }
  else
  {
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (mana_colors_added[player] != 0)
      {
        iVar4 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
        return iVar4;
      }
      in_play(player, card);
    }
    else if (event == EVENT_ACTIVATE)
    {
      instance = in_play(player, card);
      ai_modifier = ai_modifier + 100;
      if (mana_colors_added[player] == 0)
      {
        instance->info_slot = 2;
        iVar4 = generic_activated_ability(player, card, EVENT_ACTIVATE, 9, 0, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        return iVar4;
      }
      iVar4 = paying_mana();
      if ((iVar4 != 0) || (iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 1,
                                                             0, 0, 0, 0, 0, 0, NULL, NULL),
                           iVar4 == 0))
      {
        instance->info_slot = 1;
      LAB_025ce4ce:
        iVar4 = can_produce_mana_2022(player, card);
        if (iVar4 == 0)
        {
          return 0;
        }
        iVar4 = check_special_flags2_2022(player, card, 0x40);
        if ((iVar4 != 0) && (iVar4 = is_what(player, card, 1), iVar4 != 0))
        {
          produce_mana_tapped_2022(player, card, COLOR_BLACK, 1);
          return 0;
        }

        col = mana_colors_added[player];
        uVar1 = num_bits_set(col);
        if (uVar1 == 1)
        {
          color = single_color_test_bit_to_color(col);
          produce_mana_tapped_2022(player, card, color, 1);
          return 0;
        }
        produce_mana_tapped_all_one_color_2022(player, card, col, 1);
        return 0;
      }
      bVar5 = ai_is_speculating != 1;
      instance->info_slot = 2;
      if ((bVar5) && ((player != 1 || (((unsigned char)trace_mode & 2) != 0))))
      {
        iVar4 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                            "Use Ability", 1, 1, 0, 0, 0, 0);
        if (iVar4 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (iVar4 == 1)
          goto LAB_025ce4ce;
        if (iVar4 != 2)
          goto LAB_025ce563;
      }
      generic_activated_ability(player, card, EVENT_ACTIVATE, 9, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        iVar4 = mana_colors_added[player];
        goto joined_r0x025ce6ad;
      }
      instance = in_play(player, card);
      if ((event == EVENT_RESOLVE_ACTIVATION) && (instance->info_slot == 2))
      {

        new_default_test_definition(&this_test, 1, "Select a basic land card.");
        this_test.subtype = 0x2000;
        iVar4 = new_global_tutor(player, player, 1, 5, 0, 5, &this_test);
        if ((-1 < iVar4) && (iVar3 = count_permanents_by_type(player, TYPE_LAND), 3 < iVar3))
        {
          if (ai_is_speculating == 1)
          {
            ai_modifier = ai_modifier + 0xf;
          }
          else
          {
            untap_card(player, iVar4);
          }
        }
      }
    }
  LAB_025ce563:
    iVar4 = generic_activated_ability(player, card, event, 9, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  }
  return iVar4;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int nonland_nontoken_permanent(int iid, int value, int player, int card);

/* ManalinkEh 2022: 0x25c9800; original cards/throne_of_eldraine.c:1402. */
static int nonland_nontoken_permanent(int iid, int value, int player, int card)
{
  int result;

  result = is_what(player, card, 1);
  if (result != 0)
  {
    return 0;
  }
  result = is_token(player, card);
  return (unsigned int)(result == 0);
}

/* ManalinkEh 2022: 0x25cd6e0; original cards/throne_of_eldraine.c:1410. */
int card_doom_foretold(int player, int card, event_t event)
{
  int result;
  token_generation_2022_t token;
  test_definition_2022_t test;

  upkeep_trigger_ability(player, card, event, 2);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    new_default_test_definition_2022(&test, 0x47,
                                     "Select a nonland, nontoken permanent to sacrifice.");
    test.special_selection_function = nonland_nontoken_permanent;
    result = new_sacrifice_2022(player, card, current_turn,
                                SAC2022_RETURN_CHOICE | SAC2022_CAUSED | SAC2022_NO_CANCEL, &test);
    if (result == 0)
    {
      discard(current_turn, 2, player);
      lose_life(current_turn, 2);
      draw_a_card(player);
      gain_life(player, 2);
      default_token_definition_2022(player, card, 0x4958, &token);
      token.s_key_plus = 8;
      generate_token_2022(&token);
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

#endif
