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
        pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card,
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
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
