#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void true_transform_to_csvid(int player, int card, int csvid);

/* ManalinkEh 2022: 0x25c98a0; original cards/throne_of_eldraine.c:5. */
void true_transform_to_csvid(int player, int card, int csvid)
{
  card_instance_t *instance;
  uint32_t new_iid;
  uint32_t original_or_display_iid;
  int csvid_00;

  instance = get_card_instance(player, card);
  csvid_00 = instance->targets[0xd].player;
  if (instance->targets[0xd].card == csvid_00)
  {
    csvid_00 = csvid;
  }
  original_or_display_iid = instance->original_internal_card_id;
  new_iid = get_internal_card_id_from_csv_id(csvid);
  if (new_iid != original_or_display_iid)
  {
    original_or_display_iid = get_internal_card_id_from_csv_id(csvid_00);
  }
  instance->targets[0xd].card = csvid_00;
  instance->targets[0xc].card = original_or_display_iid;
  instance->internal_card_id = original_or_display_iid;
  instance->backup_internal_card_id = (uint16_t)original_or_display_iid;
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
  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
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
  return generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
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
  char *error;
  int result;
  if ((player != targeting_player) || (error = "cannot target self", card != targeting_card))
  {
    result = get_owner(player, card);
    error = "owner";
    if (targeting_player == result)
    {
      return NULL;
    }
  }
  return error;
}

/* ManalinkEh 2022: 0x25ccdb0; original cards/throne_of_eldraine.c:1160. */
int card_claim_the_firstborn(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST && event != EVENT_CAST_SPELL && event != EVENT_RESOLVE_SPELL &&
      event != EVENT_CAN_CHANGE_TARGET && event != EVENT_CHANGE_TARGET)
    return 0;
  if (event == EVENT_CAST_SPELL && (card != affected_card || player != affected_card_controller))
    return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = 1 - player;
  td.extra = 3;
  td.special = 0x800;
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                  "Select target creature with mana value 3 or less", 1, NULL);
    if (spell_fizzled != 1)
      ai_modifier_for_act_of_treason(player, card);
    return 0;
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (valid_target(&td))
      effect_act_of_treason(player, card, instance->targets[0].player, instance->targets[0].card);
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
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
  int graveyard_size;
  int mana_value;
  char *error;
  graveyard_size = count_graveyard(player);
  mana_value = get_cmc(player, card);
  error = NULL;
  if (graveyard_size < mana_value)
  {
    error = "mana value must be less than or equal to the number of cards in its controller\'s "
            "graveyard";
  }
  return error;
}

/* ManalinkEh 2022: 0x25cdfd0; original cards/throne_of_eldraine.c:1616. */
int card_emry_lurker_of_the_loch(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
    mill(player, 4);
  else if (event == EVENT_MODIFY_COST)
  {
    int artifacts = count_permanents_by_type(player, TYPE_ARTIFACT);
    COST_COLORLESS -= (char)artifacts;
    return 0;
  }

  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION &&
      event != EVENT_CLEANUP)
    return 0;

  test_definition_t test;
  new_default_test_definition(&test, 0x40, "Select target artifact card.");
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    card_instance_t *instance = get_card_instance(player, card);
    int index = validate_target_from_grave_source(player, card, player, 0);
    if (index != -1)
      create_spell_has_flashback_legacy(player, card, index, FBL_PLAYING_PERMANENT_CARD);
    instance->number_of_targets = 0;
  }
  return generic_activated_ability_with_target_in_grave(
      player, card, event, GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, 8);
}

/* ManalinkEh 2022: 0x25ce1b0; original cards/throne_of_eldraine.c:1659. */
int card_epic_downfall(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
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
  return generic_spell(player, card, event, 0x81, &td,
                         "Select target creature with CMC 3 or greater.", 1, NULL);
}

/* ManalinkEh 2022: 0x25ce310; original cards/throne_of_eldraine.c:1686. */
int card_escape_to_the_wilds(int player, int card, event_t event)
{
  unsigned short csvid;
  int result;
  card_instance_t *instance;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = 5;
    do
    {
      if (*deck_ptr[player] != -1)
      {
        csvid = cards_data[*deck_ptr[player]].id;
        rfg_top_card_of_deck(player);
        create_may_play_card_from_exile_effect(player, card, player, (unsigned int)csvid,
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
  return generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
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
  unsigned int spell_color;
  unsigned int blue_color;
  if (event == EVENT_MODIFY_COST)
  {
    if (card_on_stack_controller != -1)
    {
      spell_color = get_color(card_on_stack_controller, card_on_stack);
      blue_color = get_sleighted_color_test(player, card, 4);
      if ((spell_color & blue_color) != 0)
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
  return generic_spell(player, card, event, 2, NULL, NULL, 1, NULL);
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
  if (!IS_GS_EVENT(player, card, event))
  {
    return 0;
  }
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
  return generic_spell(player, card, event, 1, &td, "TARGET_CREATURE_OR_PLANESWALKER", 1, NULL);
}

/* ManalinkEh 2022: 0x25d3c30; original cards/throne_of_eldraine.c:4331. */
int card_specters_shriek(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST && event != EVENT_CAST_SPELL && event != EVENT_RESOLVE_SPELL &&
      event != EVENT_CAN_CHANGE_TARGET && event != EVENT_CHANGE_TARGET)
    return 0;
  if (event == EVENT_CAST_SPELL && (card != affected_card || player != affected_card_controller))
    return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (valid_target(&td))
    {
      card_instance_t *instance = get_card_instance(player, card);
      bool exiled_from_own_hand = false;
      if (hand_count[instance->targets[0].player] != 0)
      {
        test_definition_t this_test;
        new_default_test_definition(&this_test, 1, "Opponent's hand: Select a card to exile.");
        this_test.type_flag = 1;
        int selected =
            new_global_tutor(player, instance->targets[0].player, 4, 6, 0, 1, &this_test);
        if (selected >= 0)
        {
          int iid = get_internal_card_id_from_csv_id(selected);
          unsigned int colors = get_color_by_internal_id(instance->targets[0].player, iid);
          unsigned int black = get_sleighted_color_test(player, card, 2);
          if (!(colors & black) && hand_count[player] != 0)
          {
            new_default_test_definition(&this_test, 0xff, "Your hand: Select a card to exile.");
            new_global_tutor(player, player, 4, 6, 1, 3, &this_test);
            exiled_from_own_hand = true;
          }
        }
      }
      if (!exiled_from_own_hand)
        spell_fizzled = 1;
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
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
  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    int creatures = count_graveyard_by_type(player, 2);
    COST_COLORLESS += (char)creatures * -2;
  }
  test_definition_t test;
  if (event == EVENT_GRAVEYARD_FROM_PLAY || trigger_condition == 0xd5)
  {
    default_test_definition(&test, 2);
    int deaths =
        store_and_trigger_graveyard_from_play(player, card, event, player, 2, &test, player, card);
    if (deaths > 11)
      deaths = 11;
    card_instance_t *instance = get_card_instance(player, card);
    for (int index = 0; index < 10; ++index)
      if (index < deaths && instance->targets[index].player != -1)
      {
        int owner, position;
        if (find_in_owners_graveyard(instance->targets[index].player, instance->targets[index].card,
                                     &owner, &position))
          from_graveyard_to_deck(owner, position, 2);
        instance->targets[index].card = -1;
        instance->targets[index].player = -1;
      }
  }
  if (event != EVENT_ACTIVATE && event - EVENT_RESOLVE_ACTIVATION > 1 && event != EVENT_CLEANUP)
    return 0;
  new_default_test_definition(&test, 2, "Select target creature card.");
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    int index = validate_target_from_grave_source(player, card, player, 0);
    if (index != -1)
      reanimate_permanent(player, card, player, index, REANIMATE_DEFAULT);
  }
  return generic_activated_ability_with_target_in_grave(
      player, card, event, GAA_CAN_SORCERY_BE_PLAYED | GAA_UNTAPPED, 2, 1, 0, 0, 0, 0, 2, NULL,
      NULL, TYPE_NONE, &test, 8);
}

/* ManalinkEh 2022: 0x25d5e90; original cards/throne_of_eldraine.c:5218. */
int card_vantress_gargoyle(int player, int card, event_t event)
{
  if (event == EVENT_ATTACK_LEGALITY)
  {
    if (card == affected_card && player == affected_card_controller &&
        count_graveyard(1 - current_turn) < 7)
      event_result |= 1;
    return 0;
  }
  if (event == EVENT_BLOCK_LEGALITY)
  {
    if (card == affected_card && player == affected_card_controller && hand_count[player] <= 3)
      event_result |= 1;
    return 0;
  }

  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION &&
      event != EVENT_CLEANUP)
    return 0;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    mill(current_turn, 1);
    mill(current_turn ^ 1, 1);
  }
  return generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
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

  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION &&
      event != EVENT_CLEANUP)
    return 0;
  if (event == EVENT_RESOLVE_ACTIVATION)
    draw_a_card(player);
  msg = get_sleighted_color_text(player, card, "Select another %s creature to sacrifice.", 1);
  new_default_test_definition(&test, 2, msg);
  test.color = get_sleighted_color_test(player, card, 2);
  test.not_me = 1;
  return generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, SARV_0);
}

/* ManalinkEh 2022: 0x25cc840; original cards/throne_of_eldraine.c:1005. */
int card_cauldron_familiar(int player, int card, event_t event)
{
  int result;
  test_definition_t test_food;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  if (event != EVENT_CAN_CAST && event != EVENT_CAST_SPELL && event != EVENT_RESOLVE_SPELL &&
      event != EVENT_CAN_CHANGE_TARGET && event != EVENT_CHANGE_TARGET)
    return 0;
  if (event == EVENT_CAST_SPELL && (card != affected_card || player != affected_card_controller))
    return 0;

  target_definition_t td, spell_target;
  default_target_definition(player, card, &td, 2);
  td.special = 0x8000;
  td.extra = (int32_t)ditl_graveyard_count;
  counterspell_target_definition(player, card, &spell_target, 0xff);
  spell_target.special = 0x8000;
  spell_target.extra = (int32_t)ditl_graveyard_count;
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
  {
    if (counterspell(player, card, EVENT_CAN_CAST, &spell_target, 0))
      return 99;
    return generic_spell(player, card, EVENT_CAN_CAST, 0x81, &td, NULL, 1, NULL);
  }
  if (event == EVENT_CAST_SPELL)
  {
    if (card == affected_card && player == affected_card_controller)
    {
      instance->info_slot = 0;
      if (counterspell(player, card, EVENT_CAN_CAST, &spell_target, 0))
      {
        instance->info_slot = 0x42;
        return counterspell(player, card, EVENT_CAST_SPELL, &spell_target, 0);
      }
      generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                    "Select target creature with mana value less than or equal to the number of "
                    "cards in its controller's graveyard.",
                    1, NULL);
      if (spell_fizzled != 1)
        instance->info_slot = 0x43;
    }
    return 0;
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    unsigned char valid = 0;
    if (instance->info_slot == 0x42)
      valid = counterspell_validate(player, card, &spell_target, 0) != 0;
    if (instance->info_slot == 0x43)
      valid |= (valid_target(&td) != 0) * 2;
    if (valid & 1)
      real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
    if (valid & 2)
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    kill_card(player, card, KILL_DESTROY);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25cebb0; original cards/throne_of_eldraine.c:1847. */
int card_faeburrow_elder(int player, int card, event_t event)
{
  vigilance(player, card, event);
  int colors[6] = {0};
  bool count_colors;
  if (event == EVENT_COUNT_MANA)
    count_colors = card == affected_card && player == affected_card_controller;
  else
    count_colors =
        event == EVENT_ACTIVATE || (event == EVENT_CAN_ACTIVATE && player == 1) ||
        (event - EVENT_POWER < 2 && card == affected_card && player == affected_card_controller);
  if (count_colors)
  {
    card_instance_t *instance = get_card_instance(player, card);
    instance->info_slot = 0;
    target_definition_t td;
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.illegal_abilities = 0;
    int total = 0;
    for (int color = 1; color < 6; ++color)
    {
      unsigned int color_test = 1 << ((unsigned char)color & 0x1f);
      td.required_color = color_test;
      if (can_target(&td))
      {
        colors[color] = 1;
        instance = get_card_instance(player, card);
        ++total;
        instance->info_slot |= color_test;
      }
    }
    if (event - EVENT_POWER < 2)
      event_result += total;
  }
  return mana_producing_creature_multi(player, card, event, 0x18, 0, colors[1], colors[2],
                                       colors[3], colors[4], colors[5]);
}

/* ManalinkEh 2022: 0x25ced80; original cards/throne_of_eldraine.c:1910. */
int card_feasting_troll_king(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  test_definition_t test_food;

  vigilance(player, card, event);
  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    instance = get_card_instance(player, card);
    result = not_played_from_hand(player, card);
    bool no_etb_trigger = trigger_condition != 0xdb;
    instance->info_slot = result;
    if (no_etb_trigger)
      return 0;
  }
  if (trigger_condition == 0xdb && card == affected_card && player == affected_card_controller &&
      (instance = get_card_instance(player, card), instance->info_slot != 1))
    etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    generate_tokens_by_id(player, card, 0x5077, 3);
    return 0;
  }
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
  protection_from_multicolored(player, card, event);
  card_instance_t *instance = get_card_instance(player, card);
  int number;
  if (event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller)
  {
    set_special_flags2(player, card, 0x20000);
    if (!check_special_flags2(player, card, -0x80000000))
      instance->info_slot = 0;
    if (!played_for_free(player, card) && !is_token(player, card))
    {
      charge_mana(player, COLOR_COLORLESS, -1);
      set_x_for_x_spells(player, card, EVENT_CAST_SPELL, x_value);
      number = instance->info_slot;
    }
    else
    {
      instance->info_slot = 0;
      number = 0;
    }
  }
  else
    number = instance->info_slot;
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

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  unsigned int sacrificed;
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
    sacrificed = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST, &test);
    if (sacrificed == 0)
    {
      instance = get_card_instance(player, card);
      spell_fizzled = 1;
      instance->state = instance->state & 0xffffffef;
      result = 0;
    }
    else
    {
      result = get_toughness((int)sacrificed >> 0x10 & 0xff, sacrificed >> 0x18);
      instance = get_card_instance(player, card);
      instance->info_slot = ((3 < result) + 1);
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
  set_special_flags2(player, card, 0x800);
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
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION &&
      event != EVENT_CLEANUP)
    return 0;
  card_instance_t *instance = get_card_instance(player, card);
  int normal_timing = can_sorcery_be_played(player, event);
  unsigned int can_play_normally = 0;
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE)
  {
    if (normal_timing)
      can_play_normally = 1;
    else if (stack_size == 1 && card_on_stack_controller == player)
      can_play_normally = (unsigned int)(card_on_stack == card);
  }
  else
  {
    if (normal_timing || stack_size != 1)
      return 0;
    if (card_on_stack_controller == player)
      can_play_normally = (unsigned int)(card_on_stack == card);
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    unsigned int available = 0;
    for (int index = 0; index < 16; ++index)
      if (instance->targets[index].player >= 0 && instance->targets[index].card >= 0 &&
          has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, instance->targets[index].card) &&
          find_iid_in_rfg(instance->targets[index].player, instance->targets[index].card) >= 0 &&
          (can_play_normally || cards_data[instance->targets[index].card].id == 0x5037))
        available = 1;
    return available;
  }
  if (event == EVENT_ACTIVATE)
  {
    int castable[16] = {0};
    for (int index = 0; index < 16; ++index)
      if (instance->targets[index].player != -1 && instance->targets[index].card != -1 &&
          has_mana_to_cast_iid(player, EVENT_ACTIVATE, instance->targets[index].card) &&
          find_iid_in_rfg(instance->targets[index].player, instance->targets[index].card) >= 0)
      {
        int iid = instance->targets[index].card;
        if (can_play_normally || cards_data[iid].id == 0x5037)
          castable[index] = iid;
      }
    const char *names[16];
    for (int index = 15; index >= 0; --index)
      names[index] = get_card_name_by_id((unsigned int)cards_data[castable[index]].id);
    int choice = dialog_impl(
        player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\004", names[0], castable[0], 1,
        names[1], castable[1], 1, names[2], castable[2], 1, names[3], castable[3], 1, names[4],
        castable[4], 1, names[5], castable[5], 1, names[6], castable[6], 1, names[7], castable[7],
        1, names[8], castable[8], 1, names[9], castable[9], 1, names[10], castable[10], 1,
        names[11], castable[0xb], 1, names[12], castable[0xc], 1, names[13], castable[0xd], 1,
        names[14], castable[0xe], 1, names[15], castable[0xf], 1, 0, 0, 0, 0);
    if (!choice)
    {
      spell_fizzled = 1;
      return 0;
    }
    int iid = castable[choice - 1];
    int paid = charge_mana_from_id(player, -1, EVENT_ACTIVATE, (unsigned int)cards_data[iid].id);
    if (paid && play_card_from_adventure(player, instance->targets[choice - 1].player, iid))
    {
      instance->targets[choice - 1].player = -1;
      instance->targets[choice - 1].card = -1;
      cant_be_responded_to = 1;
      for (int index = 0; index < 16; ++index)
        if (instance->targets[index].player != -1 && instance->targets[index].card != -1 &&
            find_iid_in_rfg(instance->targets[index].player, instance->targets[index].card) >= 0)
          return 0;
      kill_card(player, card, KILL_EXILE);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25ca280; original cards/throne_of_eldraine.c:169. */
static int resolve_an_adventure(int player, int card, int csvid_creature);

int adventure(int player, int card, event_t event, int csvid_creature)
{
  if (event == EVENT_COUNTERSPELL)
  {
    if (card == affected_card && player == affected_card_controller && !is_token(player, card))
      adventure_transform(player, card, csvid_creature);
    return 0;
  }
  if (event != EVENT_RESOLVE_SPELL || is_token(player, card))
    return 0;
  return resolve_an_adventure(player, card, csvid_creature);
}

/* ManalinkEh 2022: 0x25ca460; original cards/throne_of_eldraine.c:178. */
int generic_adventurer(int player, int card, event_t event, int priority_creature,
                       int priority_adventure, int csvid_adventure)
{
  int value;
  unsigned int adventure_legal;
  card_instance_t *instance;
  const char *adventure_name;
  const char *creature_name;
  unsigned int csvid;
  int creature_legal;

  if ((event == EVENT_QUERY) && (attacking_card == 1))
  {
    return 1;
  }
  if ((((stack_size == 1) && (card_on_stack_controller == player)) && (card_on_stack == card)) &&
      (value = can_legally_play_csvid(player, csvid_adventure), value != 0))
  {
    adventure_legal = 1;
  }
  else
  {
    value = can_legally_play_csvid_now(player, csvid_adventure, event);
    adventure_legal = (unsigned int)(value != 0);
  }
  value = get_original_internal_card_id(player, card);
  creature_legal = 1;
  csvid = (unsigned int)cards_data[value].id;
  if ((cards_data[value].type & 0x10) == 0)
  {
    creature_legal = can_sorcery_be_played(player, event);
  }
  if (event == EVENT_MODIFY_COST)
  {
    value = has_mana_to_cast_id(player, EVENT_MODIFY_COST, csvid_adventure);
    if (((adventure_legal == 0) || (value == 0)) &&
        ((value = has_mana_to_cast_id(player, EVENT_MODIFY_COST, csvid),
          creature_legal == 0 || (value == 0))))
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
    value = instance->targets[0xc].card;
    if (value < 0)
    {
      return 0;
    }
    event_result = value;
    instance->regen_status = instance->regen_status | 0x9000000;
    return 0;
  }
  if (event == EVENT_CAN_CAST)
  {
    value = has_mana_to_cast_id(player, EVENT_CAN_CAST, csvid_adventure);
    if ((adventure_legal != 0) && (value != 0))
    {
      return adventure_legal;
    }
    value = has_mana_to_cast_id(player, EVENT_CAN_CAST, csvid);
    if (creature_legal == 0)
    {
      return 0;
    }
    if (value == 0)
    {
      return 0;
    }
    return creature_legal;
  }
  if (event != EVENT_CAST_SPELL)
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      return 0;
    }
    if (check_special_flags2(player, card, 0x800) == 0)
    {
      return 0;
    }
    remove_special_flags2(player, card, 0x800);
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
  if (check_special_flags2(player, card, 0x800) != 0)
  {
    return 0;
  }
  if (check_special_flags3(player, card, 1) != 0)
  {
    return 0;
  }

  bool skip_cost = played_for_free(player, card) != 0;
  if (!skip_cost)
    skip_cost = is_token(player, card) != 0;
  if (!check_special_flags2(player, card, -0x80000000))
  {
    instance->info_slot = 0;
    instance->number_of_targets = 0;
    int choice = 1;
    bool choose_mode = false;
    if (skip_cost)
    {
      if (adventure_legal)
      {
        if (!creature_legal)
          choice = 2;
        else
          choose_mode = true;
      }
    }
    else
    {
      int adventure_mana = has_mana_to_cast_id(player, EVENT_CAST_SPELL, csvid_adventure);
      if (adventure_mana && adventure_legal)
      {
        if (!has_mana_to_cast_id(player, EVENT_CAST_SPELL, csvid) || !creature_legal)
          choice = 2;
        else
          choose_mode = true;
      }
    }
    if (choose_mode)
    {
      adventure_name = get_card_name_by_id(csvid_adventure);
      creature_name = get_card_name_by_instance(instance);
      choice = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014", creature_name,
                           creature_legal, priority_creature, adventure_name, 1, priority_adventure,
                           0, 0, 0, 0);
      if (!choice)
      {
        spell_fizzled = 1;
        return 0;
      }
    }
    instance->info_slot = choice;
  }
  if (instance->info_slot == 1)
  {
    if (!skip_cost)
    {
      charge_mana_from_id(player, card, EVENT_CAST_SPELL, csvid);
      if (is_x_spell(player, card))
        instance->targets[1].card = x_value;
    }
  }
  else if (instance->info_slot == 2)
  {
    instance->targets[13].player = csvid;
    instance->targets[13].card = csvid;
    adventure_transform(player, card, csvid_adventure);
    if (!skip_cost)
      charge_mana_from_id(player, card, EVENT_CAST_SPELL, csvid_adventure);
    bool paid = spell_fizzled != 1;
    instance->targets[1].card = x_value;
    if (paid)
    {
      int iid = get_internal_card_id_from_csv_id(csvid_adventure);
      call_card_fn(cards_data[iid].code_pointer, instance, player, card, EVENT_CAST_SPELL);
      if (spell_fizzled != 1)
        return 0;
    }
    adventure_transform(player, card, csvid);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25cac80; original cards/throne_of_eldraine.c:351. */
int card_alter_fate(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST || event == EVENT_CAN_CHANGE_TARGET)
  {
    test_definition_t this_test;
    new_default_test_definition(&this_test, 2, "Select a creature card.");
    return generic_spell(player, card, event, 8, NULL, NULL, 1, &this_test);
  }
  if (event == EVENT_CAST_SPELL)
  {
    if (card == affected_card && player == affected_card_controller)
    {
      test_definition_t this_test;
      new_default_test_definition(&this_test, 2, "Select a creature card.");
      if (card == affected_card && player == affected_card_controller)
      {
        generic_spell(player, card, EVENT_CAST_SPELL, 8, NULL, NULL, 1, &this_test);
        if (spell_fizzled == 1)
          return 0;
        card_instance_t *instance = get_card_instance(player, card);
        ai_modifier +=
            cards_ptr[graveyard_ptr_mutable[player][instance->targets[0].player]]->ai_base_value;
      }
    }
    return 0;
  }
  if (event == EVENT_RESOLVE_SPELL || event == EVENT_CHANGE_TARGET)
  {
    test_definition_t this_test;
    new_default_test_definition(&this_test, 2, "Select a creature card.");
    if (event == EVENT_RESOLVE_SPELL)
    {
      int index = validate_target_from_grave_source(player, card, player, 0);
      if (index != -1)
        from_grave_to_hand(player, index, 1);
    }
  }
  if (event == EVENT_RESOLVE_SPELL ||
      (event & ~EVENT_SET_LEGACY_EFFECT_TEXT) == EVENT_CHANGE_TARGET ||
      event == EVENT_CHANGE_TYPE || event == EVENT_SET_COLOR)
    return adventure(player, card, event, 0x50d7);
  return 0;
}

/* ManalinkEh 2022: 0x25caf40; original cards/throne_of_eldraine.c:394. */
int card_arcane_signet(int player, int card, event_t event)
{
  card_instance_t *instance;
  int card_or_iid;
  card_instance_t *permanent;
  unsigned int colors;

  instance = get_card_instance(player, card);
  if (event == EVENT_CAST_SPELL)
  {

    if (((card == affected_card) && (player == affected_card_controller)) &&
        (0 < active_cards_count[player]))
    {
      card_or_iid = 0;
      do
      {
        permanent = in_play(player, card_or_iid);
        if ((permanent != NULL) && (get_id(player, card_or_iid) == 0x3953))
        {

          permanent = get_card_instance(player, card_or_iid);
          card_or_iid = permanent->targets[2].player;
          instance->info_slot = cards_data[card_or_iid].color & 0x3e;
          colors = get_color_from_remainder_text((unsigned int)cards_data[card_or_iid].id);
          colors = colors & 0x3e | instance->info_slot;
          instance->info_slot = colors;
          card_or_iid = permanent->targets[2].card;
          if (-1 < card_or_iid)
          {
            instance->info_slot = cards_data[card_or_iid].color & 0x3e | colors;
            colors = get_color_from_remainder_text((unsigned int)cards_data[card_or_iid].id);
            instance->info_slot = instance->info_slot | colors & 0x3e;
          }
          break;
        }
        card_or_iid = card_or_iid + 1;
      } while (card_or_iid < active_cards_count[player]);
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    play_land_sound_effect_force_color(player, card, instance->info_slot);
  }
  return plain_mana_producer(player, card, event);
}

/* ManalinkEh 2022: 0x25cb400; original cards/throne_of_eldraine.c:554. */
int card_battle_display(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE &&
      event != EVENT_SET_COLOR)
    return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, 0x40);
  if (event == EVENT_CAST_SPELL && (card != affected_card || player != affected_card_controller))
    return adventure(player, card, event, 0x505b);
  if (event == EVENT_CAN_CAST || event == EVENT_CAST_SPELL || event == EVENT_CAN_CHANGE_TARGET)
    return generic_spell(player, card, event, 1, &td, "TARGET_ARTIFACT", 1, NULL);
  if (event == EVENT_RESOLVE_SPELL && valid_target(&td))
  {
    card_instance_t *instance = get_card_instance(player, card);
    kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
  }
  return adventure(player, card, event, 0x505b);
}

/* ManalinkEh 2022: 0x25cb600; original cards/throne_of_eldraine.c:578. */
int card_beanstalk_giant(int player, int card, event_t event)
{
  if (event - EVENT_POWER < 2 && card == affected_card && player == affected_card_controller)
  {
    int lands = count_subtype(player, 1, -1);
    set_characteristic_pt(lands);
  }

  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE &&
      event != EVENT_QUERY)
    return 0;

  return generic_adventurer(player, card, event, 5, 5, 0x502d);
}

/* ManalinkEh 2022: 0x25cb6f0; original cards/throne_of_eldraine.c:601. */
int card_beloved_princess(int player, int card, event_t event)
{
  int result;

  lifelink(player, card, event);
  if (((event == EVENT_BLOCK_LEGALITY) && (attacking_card_controller == player)) &&
      (attacking_card == card))
  {
    result = is_humiliated(player, card, AL_STATIC);
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
  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE &&
      event != EVENT_SET_COLOR)
    return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, 2);
  if (event == EVENT_CAST_SPELL && (card != affected_card || player != affected_card_controller))
    return adventure(player, card, event, 0x50f0);
  if (event == EVENT_CAN_CAST || event == EVENT_CAST_SPELL || event == EVENT_CAN_CHANGE_TARGET)
    return generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  if (event == EVENT_RESOLVE_SPELL && valid_target(&td))
  {
    card_instance_t *instance = get_card_instance(player, card);
    pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 2, 0);
  }
  return adventure(player, card, event, 0x50f0);
}

/* ManalinkEh 2022: 0x25cbb30; original cards/throne_of_eldraine.c:714. */
int card_brazen_borrower(int player, int card, event_t event)
{

  if (event == EVENT_BLOCK_LEGALITY)
  {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (is_humiliated(player, card, AL_STATIC) == 0)) &&
        (check_for_ability(attacking_card_controller, attacking_card, 0x20) == 0))
    {
      event_result = 1;
    }
  }
  else
  {
    if (event == EVENT_CAN_CAST)
      return generic_adventurer(player, card, event, 5, 5, 0x5038);
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        return generic_adventurer(player, card, event, 5, 5, 0x5038);
    }
    else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
             (event == EVENT_CHANGE_TARGET))
      return generic_adventurer(player, card, event, 5, 5, 0x5038);
  }
  if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
  {
    return 0;
  }
  return generic_adventurer(player, card, event, 5, 5, 0x5038);
}

/* ManalinkEh 2022: 0x25cbc30; original cards/throne_of_eldraine.c:766. */
int card_cast_off(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE &&
      event != EVENT_SET_COLOR)
    return 0;

  if (event == EVENT_CAN_CAST || event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CAST_SPELL)
    return basic_spell(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    test_definition_t test;
    default_test_definition(&test, 2);
    test.subtype = 0x48;
    test.subtype_flag = 1;
    new_manipulate_all(player, card, 2, &test, ACT_KILL_DESTROY);
  }
  return adventure(player, card, event, 0x50e7);
}

/* ManalinkEh 2022: 0x25cca30; original cards/throne_of_eldraine.c:1064. */
int card_charming_prince(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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

  if (event == EVENT_CAN_CAST || event == EVENT_RESOLVE_SPELL ||
      event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CHANGE_TARGET ||
      (event == EVENT_CAST_SPELL && card == affected_card &&
       player == affected_card_controller) ||
      event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE || event == EVENT_QUERY)
  {
    return generic_adventurer(player, card, event, 4, 5, 0x504e);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25cd560; original cards/throne_of_eldraine.c:1353. */
int card_deafening_silence(int player, int card, event_t event)
{
  int spells_cast;
  int creatures_cast;

  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    if (is_humiliated(player, card, AL_STATIC) == 0)
    {
      if (is_what(affected_card_controller, affected_card, 3) == 0)
      {
        spells_cast = get_specific_storm_count(affected_card_controller);
        creatures_cast = get_stormcreature_count(affected_card_controller);
        if (spells_cast != creatures_cast && -1 < spells_cast - creatures_cast)
        {
          infinite_casting_cost();
        }
      }
    }
  }
  return global_enchantment(player, card, event);
}

/* ManalinkEh 2022: 0x25cdf30; original cards/throne_of_eldraine.c:1596. */
int card_embereth_shieldbreaker(int player, int card, event_t event)
{

  if (event == EVENT_CAN_CAST || event == EVENT_RESOLVE_SPELL ||
      event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CHANGE_TARGET ||
      (event == EVENT_CAST_SPELL && card == affected_card &&
       player == affected_card_controller) ||
      event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE || event == EVENT_QUERY)
  {
    return generic_adventurer(player, card, event, 1, 8, 0x505c);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25ce960; original cards/throne_of_eldraine.c:1812. */
int card_fae_of_wishes(int player, int card, event_t event)
{
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE)
  {
    get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 1, 0, 1, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled != 1)
        multidiscard(player, 2, 0);
      return 0;
    }
    if (hand_count[player] > 1)
      return generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 1, 0, 1, 0, 0, 0, 0,
                                       NULL, NULL);
  }
  else if (event == EVENT_RESOLVE_ACTIVATION || event == EVENT_CLEANUP)
  {
    card_instance_t *activation = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION &&
        in_play(activation->parent_controller, activation->parent_card))
      bounce_permanent(activation->parent_controller, activation->parent_card);
    return 0;
  }

  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE &&
      event != EVENT_QUERY)
    return 0;

  return generic_adventurer(player, card, event, 5, 10, 0x5065);
}

/* ManalinkEh 2022: 0x25cf040; original cards/throne_of_eldraine.c:1968. */
int card_fertile_footsteps(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE &&
      event != EVENT_SET_COLOR)
    return 0;

  if (event == EVENT_CAN_CAST)
    return basic_spell(player, card, EVENT_CAN_CAST);
  if (event == EVENT_CAST_SPELL)
  {
    if (player == affected_card_controller)
    {
      int lands = get_total_available_lands(player);
      if (1 < lands - 4U)
        ai_modifier += 0x30;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
    global_tutor(player, player, 1, 4, 0, 4, 1, 0, 0x2000, 0, 0, 0, 0, 0, -1, 0);
  return adventure(player, card, event, 0x502c);
}

/* ManalinkEh 2022: 0x25cf6c0; original cards/throne_of_eldraine.c:2171. */
int card_foulmire_knight(int player, int card, event_t event)
{

  deathtouch(player, card, event);
  if (event == EVENT_CAN_CAST || event == EVENT_RESOLVE_SPELL ||
      event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CHANGE_TARGET ||
      (event == EVENT_CAST_SPELL && card == affected_card &&
       player == affected_card_controller) ||
      event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE || event == EVENT_QUERY)
  {
    return generic_adventurer(player, card, event, 5, 5, 0x507c);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25cf800; original cards/throne_of_eldraine.c:2245. */
int card_garruk_cursed_huntsman(int player, int card, event_t event)
{
  enum
  {
    CHOICE_WOLVES = 1,
    CHOICE_DESTROY = 2,
    CHOICE_EMBLEM = 3
  };

  card_instance_t *instance;
  int result;
  target_definition_t *creature_target;
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
      if (result == CHOICE_DESTROY)
      {
        new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      }
      return planeswalker(player, card, event, 5);
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      return planeswalker(player, card, event, 5);
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
  }
  creature_target = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    result = instance->info_slot;
    if (result == CHOICE_WOLVES)
    {
      default_token_definition(player, card, 0x49a8, &token);
      token.qty = '\x02';
      token.tou = 2;
      token.pow = 2;
      token.special_infos = 0x43;
      token.color_forced = '\n';
      generate_token(&token);
    }
    else if (result == CHOICE_DESTROY)
    {
      result = valid_target(creature_target);
      if (result != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        draw_a_card(player);
      }
    }
    else if (result == CHOICE_EMBLEM)
    {
      generate_token_by_id(player, card, 0x5085);
    }
  }
  return planeswalker(player, card, event, 5);
}

/* ManalinkEh 2022: 0x25d0770; original cards/throne_of_eldraine.c:2606. */
int card_haggle(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  if (event == EVENT_CAN_CAST)
  {
    return basic_spell(player, card, EVENT_CAN_CAST);
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
      return adventure(player, card, event, 0x50bd);
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
      return adventure(player, card, event, 0x50bd);
    }
    if ((event == EVENT_CAN_CHANGE_TARGET) ||
        ((event & ~EVENT_SET_LEGACY_EFFECT_TEXT) == EVENT_CHANGE_TARGET))
      return adventure(player, card, event, 0x50bd);
  }
  if ((event != EVENT_CHANGE_TYPE) && (event != EVENT_SET_COLOR))
  {
    return 0;
  }
  return adventure(player, card, event, 0x50bd);
}

/* ManalinkEh 2022: 0x25d08c0; original cards/throne_of_eldraine.c:2658. */
int card_hearts_desire(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE &&
      event != EVENT_SET_COLOR)
    return 0;

  if (event == EVENT_CAN_CAST || event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CAST_SPELL)
    return basic_spell(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    generate_token_by_id(player, card, 0x494c);
  }
  return adventure(player, card, event, 0x50b3);
}

/* ManalinkEh 2022: 0x25c9120; original cards/throne_of_eldraine.c:2972. */
static int korvold_fae_cursed_king_effect(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (event == EVENT_STATIC_EFFECTS && card == affected_card && player == affected_card_controller)
  {
    instance = get_card_instance(player, card);
    if (instance->damage_target_player >= 0 &&
        get_id(instance->damage_target_player, instance->damage_target_card) != 0x50a9)
      kill_card(player, card, KILL_EXILE);
  }
  instance = get_card_instance(player, card);
  if (instance->damage_target_player >= 0 &&
      effect_follows_control_of_attachment(player, card, event))
    return 0;
  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    instance = get_card_instance(player, card);
    if (in_play(affected_card_controller, affected_card) &&
        get_card_instance(affected_card_controller, affected_card)->kill_code)
    {
      if (instance->damage_target_player >= 0 && instance->damage_target_card == affected_card &&
          instance->damage_target_player == affected_card_controller)
      {
        instance->damage_target_card = -1;
        instance->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        if (instance->targets[11].player < 0)
          instance->targets[11].player = 0;
        ++*((uint8_t *)&instance->targets[11].player + 1);
      }
      instance = get_card_instance(affected_card_controller, affected_card);
      if (instance->kill_code == 3)
      {
        test_definition_t test;
        default_test_definition(&test, 0x47);
        count_for_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, player, 0, &test);
      }
    }
  }
  unsigned int deaths = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (deaths && !check_status(player, card, 0x10000))
  {
    draw_cards(player, deaths & 0xff);
    kill_card(player, card, KILL_EXILE);
  }
  int status = check_status(player, card, 0x10000);
  if (event == EVENT_CLEANUP && !status)
    kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x25d1790; original cards/throne_of_eldraine.c:3097. */
int card_linden_the_steadfast_queen(int player, int card, event_t event)
{
  test_definition_t test;

  check_legend_rule(player, card, event);
  vigilance(player, card, event);
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
      cannot_attack(player, card, EVENT_ABILITIES);
    }
  }
  else
  {
    if (event == EVENT_CAN_CAST)
      return generic_adventurer(player, card, event, 5, 8, 0x50b4);
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        return generic_adventurer(player, card, event, 5, 8, 0x50b4);
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      return generic_adventurer(player, card, event, 5, 8, 0x50b4);
  }
  if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
  {
    return 0;
  }
  return generic_adventurer(player, card, event, 5, 8, 0x50b4);
}

/* ManalinkEh 2022: 0x25d2110; original cards/throne_of_eldraine.c:3350. */
int card_merchant_of_the_vale(int player, int card, event_t event)
{

  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      if (event != EVENT_CAN_CAST)
      {
        if (event == EVENT_CAST_SPELL)
        {
          if ((card == affected_card) && (player == affected_card_controller))
            return generic_adventurer(player, card, event, 5, 7, 0x50be);
        }
        else if ((event == EVENT_RESOLVE_SPELL) ||
                 ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
          return generic_adventurer(player, card, event, 5, 7, 0x50be);
        if ((event != EVENT_MODIFY_COST && event != EVENT_CHANGE_TYPE) && (event != EVENT_QUERY))
        {
          return 0;
        }
      }
      return generic_adventurer(player, card, event, 5, 7, 0x50be);
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      draw_cards(player, 1);
    }
  }
  return generic_activated_ability(player, card, event, 0x20, 2, 0, 0, 0, 1, 0, 0, NULL, NULL);
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
  if (event == EVENT_CAN_CAST || event == EVENT_RESOLVE_SPELL ||
      event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CHANGE_TARGET ||
      (event == EVENT_CAST_SPELL && card == affected_card &&
       player == affected_card_controller) ||
      event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE || event == EVENT_QUERY)
  {
    return generic_adventurer(player, card, event, 5, 10, 0x50c7);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d2410; original cards/throne_of_eldraine.c:3450. */
int card_mystic_sanctuary(int player, int card, event_t event)
{
  if (event != EVENT_CAST_SPELL || card != affected_card || player != affected_card_controller)
    return mana_producer(player, card, event);
  bool can_tutor = check_state(player, card, 0x10) != 0;
  if (!can_tutor)
  {
    int islands = 0;
    for (int permanent = 0; permanent < active_cards_count[player]; ++permanent)
      if (in_play(player, permanent))
      {
        subtype_t subtype = get_hacked_subtype(player, card, 0x5002);
        if (has_subtype(player, permanent, subtype) && ++islands == 3)
          break;
      }
    if (islands > 2)
      can_tutor = true;
    else
    {
      card_instance_t *instance = get_card_instance(player, card);
      instance->state |= 0x10;
    }
  }
  if (can_tutor)
  {
    test_definition_t this_test;
    new_default_test_definition(&this_test, 0x38, "Select target instant or sorcery card.");
    if (new_special_count_grave(player, &this_test) && !graveyard_has_shroud(player))
      new_global_tutor(player, player, 2, 2, 0, 1, &this_test);
  }
  return mana_producer(player, card, event);
}

/* ManalinkEh 2022: 0x25d2680; original cards/throne_of_eldraine.c:3566. */
int card_oko_thief_of_crowns(int player, int card, event_t event)
{
  enum
  {
    CHOICE_FOOD = 1,
    CHOICE_ELK = 2,
    CHOICE_TRADE = 3
  };

  card_instance_t *instance;
  int result;
  unsigned int can_trade;
  target_definition_t *opponent_target;
  target_definition_t *own_target;
  target_definition_t *elk_target;
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
      can_trade = 0;
      if (result != 0)
      {
        result = can_target(&td_opp);
        can_trade = (unsigned int)(result != 0);
      }
      result = can_target(&td_elk);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Food", 1, 3, 2,
                           "Elk", result, 8, 1, "Trade", can_trade, 8, 0xfffffffb, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == CHOICE_ELK)
      {
        result = can_target(&td_elk);
        if (result != 0)
        {
          new_pick_target(&td_elk, "PUTREFY", 0, 1);
        }
      }
      else if (result == CHOICE_TRADE)
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
      return planeswalker(player, card, event, 4);
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      return planeswalker(player, card, event, 4);
    default_target_definition(player, card, &td_elk, 0x42);
    default_target_definition(player, card, &td_player, 0x42);
    td_player.allowed_controller = player;
    default_target_definition(player, card, &td_opp, 2);
    td_opp.allowed_controller = 1 - player;
    td_opp.power_requirement = 0x2003;
    instance = get_card_instance(player, card);
  }
  elk_target = &td_elk;
  own_target = &td_player;
  opponent_target = &td_opp;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    result = instance->info_slot;
    if (result == CHOICE_FOOD)
    {
      generate_token_by_id(player, card, 0x5077);
    }
    else if (result == CHOICE_ELK)
    {
      result = valid_target(elk_target);
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
    else if (((result == CHOICE_TRADE) && (result = valid_target(own_target), result != 0)) &&
             (result = validate_target(player, card, opponent_target, 1), result != 0))
    {
      exchange_control_of_target_permanents(instance->parent_controller, instance->parent_card,
                                            instance->targets[0].player, instance->targets[0].card,
                                            instance->targets[1].player, instance->targets[1].card);
      instance->number_of_targets = '\0';
    }
  }
  return planeswalker(player, card, event, 4);
}

/* ManalinkEh 2022: 0x25d2d30; original cards/throne_of_eldraine.c:3717. */
int card_order_of_midnight(int player, int card, event_t event)
{

  cannot_block(player, card, event);
  if (event == EVENT_CAN_CAST || event == EVENT_RESOLVE_SPELL ||
      event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CHANGE_TARGET ||
      (event == EVENT_CAST_SPELL && card == affected_card &&
       player == affected_card_controller) ||
      event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE || event == EVENT_QUERY)
  {
    return generic_adventurer(player, card, event, 5, 8, 0x50d8);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d2df0; original cards/throne_of_eldraine.c:3763. */
int card_petty_theft(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE &&
      event != EVENT_SET_COLOR)
    return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, 0x47);
  td.allowed_controller = 1 - player;
  td.illegal_type = 1;
  td.preferred_controller = td.allowed_controller;
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_CAST_SPELL && (card != affected_card || player != affected_card_controller))
    return adventure(player, card, event, 0x5037);
  if (event == EVENT_CAN_CAST || event == EVENT_CAST_SPELL || event == EVENT_CAN_CHANGE_TARGET)
    return generic_spell(player, card, event, 0x81, &td, "Select target nonland permanent.", 1,
                         NULL);
  if (event == EVENT_RESOLVE_SPELL && valid_target(&td))
  {
    bounce_permanent(instance->targets[0].player, instance->targets[0].card);
  }
  return adventure(player, card, event, 0x5037);
}

/* ManalinkEh 2022: 0x25d3080; original cards/throne_of_eldraine.c:3807. */
int card_profane_insight(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE &&
      event != EVENT_SET_COLOR)
    return 0;

  if (event == EVENT_CAN_CAST || event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CAST_SPELL)
    return basic_spell(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    draw_a_card(player);
    lose_life(player, 1);
  }
  return adventure(player, card, event, 0x507b);
}

/* ManalinkEh 2022: 0x25d3180; original cards/throne_of_eldraine.c:3839. */
int card_questing_beast(int player, int card, event_t event)
{
  unsigned int damage_record;
  int result_or_power;
  card_instance_t *instance;
  int times_damaged[2];
  int damages[2];
  target_definition_t td;

  check_legend_rule(player, card, event);
  vigilance(player, card, event);
  deathtouch(player, card, event);
  haste(player, card, event);
  result_or_power = is_humiliated(player, card, AL_STATIC);
  if ((event == EVENT_BLOCK_LEGALITY) && (result_or_power == 0))
  {
    if ((attacking_card_controller == player) && (attacking_card == card))
    {
      result_or_power = get_power(affected_card_controller, affected_card);
      if (result_or_power < 3)
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
  if (damage_dealt_by_me(player, card, event,
                         DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                             DDBM_REPORT_DAMAGE_DEALT | DDBM_MUST_BE_COMBAT_DAMAGE) != 0)
  {

    instance = get_card_instance(player, card);
    damage_record = instance->targets[1].player;
    instance->targets[1].player = 0;
    times_damaged[1] = damage_record >> 8 & 0xff;
    times_damaged[0] = damage_record & 0xff;
    damage_record = instance->targets[0x10].player;
    instance->targets[0x10].player = 0;
    damages[1] = damage_record >> 8 & 0xff;
    damages[0] = damage_record & 0xff;
    default_target_definition(player, card, &td, 0x100);
    result_or_power = 1 - player;
    td.allow_cancel = 0;
    td.allowed_controller = result_or_power;
    if (times_damaged[result_or_power] != 0)
    {
      if (can_target(&td) != 0)
      {
        if (new_pick_target(&td, "Select target planeswalker an opponent controls.", 0, 0x80) != 0)
        {
          damage_target0(player, card, damages[result_or_power]);
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
  check_legend_rule(player, card, event);
  haste(player, card, event);
  if (event == EVENT_DEAL_DAMAGE || event == EVENT_GRAVEYARD_FROM_PLAY ||
      trigger_condition == 0xd7 || event == EVENT_CHANGE_TYPE)
  {
    unsigned int hits = whenever_i_deal_damage_to_a_player(
        player, card, event, DDBM_MUST_BE_COMBAT_DAMAGE | DDBM_TRIGGER_OPTIONAL);
    for (int remaining = ((hits >> 8) & 0xff) + (hits & 0xff); remaining; --remaining)
    {
      unsigned int choice =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Discard", 1, 5, "Draw & lose 1", 1,
                      5, "Sacrifice", 1, 5, "Discard & Draw", 1, 5, "Discard & Sac", 1, 5,
                      "Draw & Sac", 1, 5, "All three", 1, 5, 0, 0, 0, 0);
      bool discard_cards = choice == 4 || (choice != 3 && (choice & 1));
      unsigned int draw_cards_mode = choice == 4 ? 2 : choice == 3 ? 0 : choice & 2;
      unsigned int sacrifice = choice == 3 ? 1 : choice == 4 ? 0 : choice & 4;
      if (discard_cards)
      {
        discard(current_turn, 0, player);
        discard(current_turn ^ 1, 0, player);
      }
      int first_player = current_turn;
      if (draw_cards_mode)
      {
        lose_life(current_turn, 1);
        draw_a_card(first_player);
        unsigned int second_player = current_turn ^ 1;
        lose_life(second_player, 1);
        draw_a_card(second_player);
      }
      if (sacrifice)
      {
        test_definition_t test;
        new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
        new_sacrifice(player, card, current_turn, SAC_CAUSED | SAC_NO_CANCEL, &test);
        new_sacrifice(player, card, current_turn ^ 1, SAC_CAUSED | SAC_NO_CANCEL, &test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d36a0; original cards/throne_of_eldraine.c:3969. */
int card_realm_cloaked_giant(int player, int card, event_t event)
{

  vigilance(player, card, event);
  if (event == EVENT_CAN_CAST || event == EVENT_RESOLVE_SPELL ||
      event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CHANGE_TARGET ||
      (event == EVENT_CAST_SPELL && card == affected_card &&
       player == affected_card_controller) ||
      event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE || event == EVENT_QUERY)
  {
    return generic_adventurer(player, card, event, 5, 8, 0x50e8);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d38b0; original cards/throne_of_eldraine.c:4051. */
int card_rimrock_knight(int player, int card, event_t event)
{

  cannot_block(player, card, event);
  if (event == EVENT_CAN_CAST || event == EVENT_RESOLVE_SPELL ||
      event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CHANGE_TARGET ||
      (event == EVENT_CAST_SPELL && card == affected_card &&
       player == affected_card_controller) ||
      event == EVENT_MODIFY_COST || event == EVENT_CHANGE_TYPE || event == EVENT_QUERY)
  {
    return generic_adventurer(player, card, event, 3, 5, 0x50f1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d3a50; original cards/throne_of_eldraine.c:4105. */
int card_roving_keep(int player, int card, event_t event)
{
  int result;
  card_instance_t *activation;
  card_instance_t *source;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    activation = get_card_instance(player, card);
    source = in_play(activation->parent_controller, activation->parent_card);
    if (source != NULL)
    {

      pump_ability_until_eot(player, card, activation->parent_controller, activation->parent_card,
                             2, 0, 0x80, 0);
      result =
          create_targetted_legacy_effect(player, card, effect_defender_can_attack_until_eot,
                                         activation->parent_controller, activation->parent_card);
      activation = get_card_instance(player, result);
      activation->token_status = 0x10000;
    }
  }
  else if ((event == EVENT_POW_BOOST) ||
           (((event == EVENT_CHECK_ABILITIES && (card == affected_card)) &&
             (player == affected_card_controller))))
  {
    return generic_shade(player, card, event, 0, 7, 0, 0, 0, 0, 0, 2, 0, 0x80, 0);
  }
  return generic_activated_ability(player, card, event, 0, 7, 0, 0, 0, 0, 0, 0, NULL, NULL);
}

/* ManalinkEh 2022: 0x25d3ec0; original cards/throne_of_eldraine.c:4389. */
int card_steelclaw_lance(int player, int card, event_t event)
{
  card_instance_t *equipment;
  int result;
  card_instance_t *instance;
  unsigned int can_equip_knight;
  unsigned int can_equip_creature;
  target_definition_t *td_00;
  const char *prompt;
  target_definition_t *knight_target;
  target_definition_t td;
  target_definition_t td1;

  equipment = get_card_instance(player, card);
  if (event - EVENT_POWER < 2)
  {
    instance = in_play(player, card);
    if ((((instance != NULL) && (equipment->damage_target_card == affected_card)) &&
         (equipment->damage_target_player == affected_card_controller)) &&
        (equipment = is_equipping(player, card), equipment != NULL))
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
      return generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x100, 1, 0, 0, 0, 0, 0, 0,
                                       &td1, NULL);
    }
    if (event == EVENT_ACTIVATE)
    {

      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 3, 0, 0, 0, 0, 0, 0,
                                         td_00, NULL);
      can_equip_creature = 0;
      if (result != 0)
      {
        result = can_sorcery_be_played(player, EVENT_ACTIVATE);
        can_equip_creature = (unsigned int)(result != 0);
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 1, 0, 0, 0, 0, 0, 0,
                                         &td1, NULL);
      can_equip_knight = 0;
      if (result != 0)
      {
        result = can_sorcery_be_played(player, EVENT_ACTIVATE);
        can_equip_knight = (unsigned int)(result != 0);
      }
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "Normal equip",
                           can_equip_creature, 1, "Knight equip", can_equip_knight, 5, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 1)
      {
        prompt = "Select target creature you control.";
        result = 3;
        return generic_activated_ability(player, card, EVENT_ACTIVATE, 0x102, result, 0, 0, 0, 0, 0,
                                         0, td_00, prompt);
      }
      if (result == 2)
      {
        prompt = "Select target Knight you control.";
        result = 1;
        td_00 = &td1;
        return generic_activated_ability(player, card, EVENT_ACTIVATE, 0x102, result, 0, 0, 0, 0, 0,
                                         0, td_00, prompt);
      }
      return generic_activated_ability(player, card, event, 0x102, 3, 0, 0, 0, 0, 0, 0, &td,
                                       "Select target creature you control.");
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
  knight_target = &td1;
  td1.allowed_controller = player;
  td1.required_subtype = 0x67;
  td1.preferred_controller = td1.allowed_controller;
  if (!(event != EVENT_RESOLVE_ACTIVATION))
  {
    int mode = equipment->info_slot;
    bool valid = false;
    if (mode == 1)
    {
      valid = validate_target(player, card, &td, 0) != 0;
      if (!valid)
        mode = equipment->info_slot;
    }
    if (!valid && mode == 2)
      valid = validate_target(player, card, knight_target, 0) != 0;
    if (valid)
      equip_target_creature(equipment->parent_controller, equipment->parent_card,
                            equipment->targets[0].player, equipment->targets[0].card);
  }
  return generic_activated_ability(player, card, event, 0x102, 3, 0, 0, 0, 0, 0, 0, &td,
                                   "Select target creature you control.");
}

/* ManalinkEh 2022: 0x25d44d0; original cards/throne_of_eldraine.c:4469. */
int card_stomp(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE &&
      event != EVENT_SET_COLOR)
    return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
  if (event == EVENT_CAST_SPELL && (card != affected_card || player != affected_card_controller))
    return adventure(player, card, event, 0x5034);
  if (event == EVENT_CAN_CAST || event == EVENT_CAST_SPELL || event == EVENT_CAN_CHANGE_TARGET)
    return generic_spell(player, card, event, 1, &td, "TARGET_ANY", 1, NULL);
  if (event == EVENT_RESOLVE_SPELL && valid_target(&td))
  {
    damage_cannot_be_prevented_until_eot(player, card);
    damage_target0(player, card, 2);
  }
  return adventure(player, card, event, 0x5034);
}

/* ManalinkEh 2022: 0x25d48b0; original cards/throne_of_eldraine.c:4547. */
int card_swift_end(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE &&
      event != EVENT_SET_COLOR)
    return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, 0x102);
  if (event == EVENT_CAST_SPELL && (card != affected_card || player != affected_card_controller))
    return adventure(player, card, event, 0x50c6);
  if (event == EVENT_CAN_CAST || event == EVENT_CAST_SPELL || event == EVENT_CAN_CHANGE_TARGET)
    return generic_spell(player, card, event, 1, &td, "TARGET_CREATURE_OR_PLANESWALKER", 1, NULL);
  if (event == EVENT_RESOLVE_SPELL && valid_target(&td))
  {
    card_instance_t *instance = get_card_instance(player, card);
    kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    lose_life(player, 2);
  }
  return adventure(player, card, event, 0x50c6);
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
      pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, result,
                     result);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x25d59f0; original cards/throne_of_eldraine.c:5000. */
int card_torbran_thane_of_red_fell(int player, int card, event_t event)
{
  char source_color;
  card_instance_t *instance;
  unsigned int red_color;
  int result;

  check_legend_rule(player, card, event);
  instance = damage_being_dealt(event);
  if ((instance != NULL) && (result = (int)instance->damage_source_player, result == player))
  {
    source_color = instance->initial_color;
    red_color = get_sleighted_color_test(result, card, 0x10);
    if (((int)source_color & red_color) != 0)
    {
      result = is_humiliated(result, card, AL_STATIC);
      if (result == 0)
      {
        instance->info_slot = instance->info_slot + 2;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d5d90; original cards/throne_of_eldraine.c:5164. */
int card_treats_to_share(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE &&
      event != EVENT_SET_COLOR)
    return 0;

  if (event == EVENT_CAN_CAST || event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CAST_SPELL)
    return basic_spell(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    generate_token_by_id(player, card, 0x5077);
  }
  return adventure(player, card, event, 0x504d);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25d18c0; original cards/throne_of_eldraine.c:3131. */
int card_lochmere_serpent(int player, int card, event_t event)
{
  enum
  {
    CHOICE_UNBLOCKABLE = 1,
    CHOICE_DRAW = 2
  };

  int32_t *grave_position;
  card_instance_t *instance;
  const char *sacrifice_prompt;
  int mode_or_available;
  int can_draw;
  uint32_t unblockable_priority;
  int already_unblockable;
  target_t *ret_location;
  test_definition_t t_is;
  test_definition_t t_sw;

  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    mode_or_available = has_mana_multi(player, 0, 1, 1, 0, 0, 0);
    if (((mode_or_available != 0) &&
         (mode_or_available = can_sorcery_be_played(player, EVENT_GRAVEYARD_ABILITY),
          mode_or_available != 0)) &&
        (mode_or_available = count_graveyard(1 - player), 4 < mode_or_available))
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
          grave_position = &ret_location->player;

          ret_location = ret_location + 1;
          rfg_card_from_grave(1 - player, *grave_position);
        } while (ret_location != instance->targets + 5);
        return 1;
      }
      return flash(player, card, event);
    }
  }
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION ||
      event == EVENT_CLEANUP)
  {
    if (stack_size == 0)
    {
      instance = get_card_instance(player, card);
      instance->targets[4].player = 0;
    }
    instance = get_card_instance(player, card);
    sacrifice_prompt = get_hacked_land_text(player, card, "Select %a to sacrifice.", 0x5002);
    new_default_test_definition(&t_is, 0xff, sacrifice_prompt);
    t_is.subtype = 0x5002;
    sacrifice_prompt = get_hacked_land_text(player, card, "Select %a to sacrifice.", 0x5009);
    new_default_test_definition(&t_sw, 0xff, sacrifice_prompt);
    t_sw.subtype = 0x5009;
    mode_or_available = generic_activated_ability_with_sacrifice_as_cost(
        player, card, EVENT_CAN_ACTIVATE, GAA_NONE, 0, 0, 1, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
        &t_is, SARV_0);
    can_draw = generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_CAN_ACTIVATE,
                                                                GAA_NONE, 0, 1, 0, 0, 0, 0, 0, NULL,
                                                                NULL, TYPE_NONE, &t_sw, SARV_0);
    if (((current_turn == player) && (current_phase < 0x17)) &&
        ((already_unblockable = check_for_special_ability(player, card, 0x10),
          already_unblockable == 0 && ((instance->targets[4].player & 1) == 0))))
    {
      unblockable_priority = 5;
    }
    else
    {
      unblockable_priority = 0;
    }
    mode_or_available =
        dialog_impl(player, card, event, "\001\002", "Unblockable", mode_or_available,
                    unblockable_priority, "Gain life & draw", can_draw, 5, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      return mode_or_available;
    }
    if (event == EVENT_ACTIVATE)
    {
      if (mode_or_available == CHOICE_UNBLOCKABLE)
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
      else if (mode_or_available == CHOICE_DRAW)
      {
        generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_ACTIVATE, GAA_NONE, 0,
                                                         1, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
                                                         &t_sw, SARV_0);
        return 0;
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (mode_or_available == CHOICE_UNBLOCKABLE)
      {
        pump_ability_until_eot_no_repeat(player, card, instance->parent_controller,
                                         instance->parent_card, 0, 0x10);
        return 0;
      }
      if (mode_or_available == CHOICE_DRAW)
      {
        gain_life(player, 1);
        draw_a_card(player);
        return 0;
      }
    }
    return 0;
  }
  return flash(player, card, event);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25d0d20; original cards/throne_of_eldraine.c:2845. */
int card_kenrith_the_returned_king(int player, int card, event_t event)
{
  enum
  {
    CHOICE_RED = 1,
    CHOICE_GREEN = 2,
    CHOICE_WHITE = 3,
    CHOICE_BLUE = 4,
    CHOICE_BLACK = 5
  };

  check_legend_rule(player, card, event);
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION &&
      event != EVENT_CLEANUP)
    return 0;
  card_instance_t *instance;
  if (stack_size == 0)
  {
    instance = get_card_instance(player, card);
    instance->targets[4].player = 0;
  }
  target_definition_t creature_target, player_target;
  test_definition_t test;
  default_target_definition(player, card, &creature_target, 2);
  default_target_definition(player, card, &player_target, 0);
  player_target.zone = 0x1000;
  new_default_test_definition(&test, 2, "Select a creature card.");
  int can_haste = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 1, 0,
                                            0, NULL, NULL);
  int can_counter = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 1, 0, 0, 1, 0, 0,
                                              0, &creature_target, "TARGET_CREATURE");
  int can_gain_life = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 2, 0, 0, 0, 0,
                                                1, 0, &player_target, "TARGET_PLAYER");
  int can_draw = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 3, 0, 1, 0, 0, 0, 0,
                                           &player_target, "TARGET_PLAYER");
  int can_reanimate = generic_activated_ability_with_target_in_grave(
      player, card, EVENT_CAN_ACTIVATE, GAA_NONE, 4, 1, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test,
      0x1000);
  uint32_t haste_priority = 0xffffffec;
  if (current_turn == player)
  {
    if (current_phase < 0x15 &&
        (instance = get_card_instance(player, card), (instance->targets[4].player & 1) == 0))
      haste_priority = 5;
    else
      haste_priority = 0;
  }
  int mode =
      dialog_impl(player, card, event, "\001\002", "Trample & Haste", can_haste, haste_priority,
                  "+1/+1 counter", can_counter, 5, "Gain 5 life", can_gain_life, 5,
                  "Target player draws", can_draw, 5, "Reanimate", can_reanimate, 5, 0, 0, 0, 0);
  if (event == EVENT_CAN_ACTIVATE)
    return mode;
  if (event == EVENT_ACTIVATE)
  {
    switch (mode)
    {
    case CHOICE_RED:
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 0, 0, 0, 0, 1, 0, 0, NULL, NULL);
      instance = get_card_instance(player, card);
      instance->targets[4].player |= 1;
      break;
    case CHOICE_GREEN:
      generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 1, 0, 0, 1, 0, 0, 0,
                                &creature_target, "TARGET_CREATURE");
      break;
    case CHOICE_WHITE:
      generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 2, 0, 0, 0, 0, 1, 0,
                                &player_target, "TARGET_PLAYER");
      break;
    case CHOICE_BLUE:
      generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 3, 0, 1, 0, 0, 0, 0,
                                &player_target, "TARGET_PLAYER");
      break;
    case CHOICE_BLACK:
      generic_activated_ability_with_target_in_grave(player, card, EVENT_ACTIVATE, GAA_NONE, 4, 1,
                                                     0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test,
                                                     0x1000);
      break;
    }
    return 0;
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    int target_player = instance->targets[0].player;
    instance = get_card_instance(player, card);
    switch (mode)
    {
    case CHOICE_RED:
      pump_creatures_until_eot(player, card, 2, 0, 0, 0, 0x80, 4, NULL);
      break;
    case CHOICE_GREEN:
      add_counters(player, card, target_player, instance->targets[0].card, COUNTER_P1_P1, 1);
      break;
    case CHOICE_WHITE:
      gain_life(target_player, 5);
      break;
    case CHOICE_BLUE:
      draw_a_card(target_player);
      break;
    case CHOICE_BLACK:
    {
      int index = validate_target_from_grave_source(player, card, target_player, 1);
      if (index != -1)
        reanimate_permanent(player, card, target_player, index, REANIMATE_UNDER_OWNER_CONTROL);
      break;
    }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d14b0; original cards/throne_of_eldraine.c:3028. */
int card_korvold_fae_cursed_king(int player, int card, event_t event)
{
  int c;
  card_instance_t *instance;
  unsigned int deaths;
  int legacy_card;
  test_definition_t test;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      bool found = false;
      if (0 < active_cards_count[player])
      {
        c = 0;
        do
        {
          instance = in_play(player, c);

          if ((((instance != NULL) && (is_what(player, c, 0x80) != 0)) &&
               (instance = get_card_instance(player, c),
                instance->info_slot_fn == korvold_fae_cursed_king_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
          {
            found = true;
            break;
          }
          c = c + 1;
        } while (c < active_cards_count[player]);
      }

      if (!found)
      {
        legacy_card = create_targetted_legacy_effect(player, card, korvold_fae_cursed_king_effect,
                                                     player, card);
        instance = get_card_instance(player, legacy_card);
        instance->targets[0].player = player;
        instance->targets[0].card = card;
        instance->number_of_targets = '\x01';
        add_status(player, legacy_card, 0x10000);
      }
    }
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
      (deaths = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY), deaths != 0))
  {

    add_counters(player, card, player, card, COUNTER_P1_P1, deaths & 0xff);
    draw_cards(player, deaths & 0xff);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25d57c0; original cards/throne_of_eldraine.c:4894. */
int card_the_great_henge(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    int power = get_highest_power(player);
    COST_COLORLESS -= (char)power;
  }
  if (trigger_condition == 0xdb && card == affected_card && player == affected_card_controller)
  {
    test_definition_t this_test;
    default_test_definition(&this_test, 2);
    this_test.type_flag = 3;
    this_test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &this_test, player, card);
  }
  if (event == EVENT_ETB)
  {
    add_counters(player, card, trigger_cause_controller, trigger_cause, COUNTER_P1_P1, 1);
    draw_a_card(player);
  }
  else if (event == EVENT_ACTIVATE)
    gain_life(player, 2);
  return artifact_mana_all_one_color(player, card, event, 2, 0);
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
    add_counters(player, card, player, card, COUNTER_LORE, 1);
    result = count_counters(player, card, COUNTER_LORE);
    draw_cards(player, result);
  }
  result = card_spellbook(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25d65d0; original cards/throne_of_eldraine.c:5470. */
int card_yorvo_lord_of_garenbrig(int player, int card, event_t event)
{
  int entering_power;
  int own_power;
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
    add_counters(player, card, player, card, COUNTER_P1_P1, 1);
    entering_power = get_power(trigger_cause_controller, trigger_cause);
    own_power = get_power(player, card);
    if (own_power < entering_power)
    {
      add_counters(player, card, player, card, COUNTER_P1_P1, 1);
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
  enum
  {
    CHOICE_FOOD = 1,
    CHOICE_MANA = 2
  };

  int choice;
  card_instance_t *instance;
  int can_make_food;
  int mana_available;
  unsigned int can_sacrifice_food;
  test_definition_t test_food;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
    generate_token_by_id(player, card, 0x5077);
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    if (instance->info_slot == CHOICE_FOOD)
      generate_token_by_id(player, card, 0x5077);
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
    if (mana_producer(player, card, EVENT_CAN_ACTIVATE) == 0)
    {
      return 0;
    }
    if (new_can_sacrifice_as_cost(player, card, &test_food) == 0)
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
      can_make_food = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      if ((can_make_food != 0) &&
          (can_make_food = new_can_sacrifice_as_cost(player, card, &test_food), can_make_food != 0))
      {
        return 1;
      }
      can_make_food = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 1, 0, 0, 1, 0,
                                                0, 0, NULL, NULL);
      return (unsigned int)(can_make_food != 0);
    }
    if (event != EVENT_ACTIVATE)
    {
      return 0;
    }

    instance->info_slot = 0;
    if (paying_mana() == 0)
    {

      can_make_food = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 1, 0, 0, 1, 0,
                                                0, 0, NULL, NULL);
      mana_available = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      can_sacrifice_food = 0;
      if (mana_available != 0)
      {
        mana_available = new_can_sacrifice_as_cost(player, card, &test_food);
        can_sacrifice_food = (unsigned int)(mana_available != 0);
      }
      choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Make a Food", can_make_food,
                           5, "Get Mana", can_sacrifice_food, 5, 0, 0, 0, 0);
      if (choice == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (choice == CHOICE_FOOD)
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
      if (choice != CHOICE_MANA)
      {
        return 0;
      }
    }
    instance->state = instance->state | 0x10;
    if (new_sacrifice(player, card, player, SAC_AS_COST, &test_food) != 0)
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
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_cards(player, 1);
    return 0;
  }
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION &&
      event != EVENT_CLEANUP)
    return 0;
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_ACTIVATION || event == EVENT_CLEANUP)
  {
    if (event == EVENT_RESOLVE_ACTIVATION && instance->targets[1].player != 0)
      gain_life(player, 3);
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (!is_tapped(player, card) && !is_animated_and_sick(player, card) &&
        can_produce_mana(player, card) && has_mana(player, 7, 1))
      return 1;
    return generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 2, 0, 0, 0, 0, 0, 0, NULL,
                                     NULL);
  }
  instance->targets[1].player = 0;
  if (is_tapped(player, card) || is_animated_and_sick(player, card) ||
      !can_produce_mana(player, card) || !has_mana(player, 7, 1))
  {
    spell_fizzled = 1;
    ai_modifier -= 100;
    return 0;
  }
  if (!paying_mana() && generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 2, 0, 0, 0,
                                                  0, 0, 0, NULL, NULL))
  {
    unsigned int priority = 0;
    if (life[player] < 6)
      priority = (unsigned int)(count_subtype(player, 1, -1) > 5);
    int choice = do_dialog(player, player, card, -1, -1,
                           " Generate mana\n Sac & gain life\n Cancel", priority);
    if (choice == 1)
    {
      add_state(player, card, 0x10);
      charge_mana(player, COLOR_COLORLESS, 2);
      if (spell_fizzled != 1)
      {
        tapped_for_mana_color = -1;
        instance->targets[1].player = 1;
        kill_card(player, card, KILL_SACRIFICE);
      }
      else
        untap_card_no_event(player, card);
      return 0;
    }
    if (choice == 2)
    {
      spell_fizzled = 1;
      ai_modifier -= 100;
      return 0;
    }
    if (choice != 0)
      return 0;
  }
  add_state(player, card, 0x10);
  charge_mana(player, COLOR_COLORLESS, 1);
  if (spell_fizzled != 1 &&
      produce_mana_tapped_all_one_color(player, card, COLOR_TEST_ANY_COLORED, 1))
    kill_card(player, card, KILL_SACRIFICE);
  else
    untap_card_no_event(player, card);
  return 0;
}

/* ManalinkEh 2022: 0x25c9450; original cards/throne_of_eldraine.c:5026. */
static int trail_of_crumbs_effect(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int deaths;
  card_instance_t *dying;
  test_definition_t test_food;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      if ((-1 < instance->damage_target_player) &&
          (result = get_id((int)instance->damage_target_player, instance->damage_target_card),
           result != 0x5130))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    instance = get_card_instance(player, card);
    if ((-1 < instance->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, 0x4700), result != 0))
    {
      return 0;
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if ((-1 < instance->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, event), result != 0))
    {
      return 0;
    }
    if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {

      instance = get_card_instance(player, card);
      dying = in_play(affected_card_controller, affected_card);
      if ((dying != NULL) &&
          (result = is_this_dying(affected_card_controller, affected_card), result != 0))
      {

        if ((instance->damage_target_card == affected_card) &&
            (affected_card_controller == instance->damage_target_player))
        {
          instance->damage_target_card = -1;
          instance->damage_target_player = -1;
          remove_status(player, card, 0x10000);
          if (instance->targets[0xb].player < 0)
          {
            instance->targets[0xb].player = 0;
          }
          *(uint8_t *)((int)&instance->targets[0xb].player + 2) = 1;
        }
        instance = get_card_instance(affected_card_controller, affected_card);
        if (instance->kill_code == '\x03')
        {
          default_test_definition(&test_food, 0xff);
          test_food.subtype = 0x3008;
          count_for_gfp_ability(player, card, 0x77, player, 0, &test_food);
        }
      }
    }
  }
  result = duh_mode(player);
  deaths = resolve_gfp_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if ((deaths != 0) && (result = check_status(player, card, 0x10000), result == 0))
  {

    deaths = deaths & 0xff;
    while (((deaths != 0 && (result = has_mana(player, 0, 1), result != 0)) &&
            (result = charge_mana_while_resolving(player, card, event, player, COLOR_COLORLESS, 1),
             result != 0)))
    {
      new_default_test_definition(&test_food, 0x47, "Select a permanent card.");
      reveal_top_cards_of_library_and_choose(player, card, player, 2, 1, TUTOR_HAND, 1,
                                             TUTOR_BOTTOM_OF_DECK, 0, &test_food);
      deaths = deaths - 1;
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
  int c;
  card_instance_t *instance;
  int legacy_or_result;
  unsigned int deaths;
  test_definition_t test_food;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    generate_token_by_id(player, card, 0x5077);
  }
  else if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      bool found = false;
      if (0 < active_cards_count[player])
      {
        c = 0;
        do
        {
          instance = in_play(player, c);

          if ((((instance != NULL) && (is_what(player, c, 0x80) != 0)) &&
               (instance = get_card_instance(player, c),
                instance->info_slot_fn == trail_of_crumbs_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
          {
            found = true;
            break;
          }
          c = c + 1;
        } while (c < active_cards_count[player]);
      }

      if (!found)
      {
        legacy_or_result =
            create_targetted_legacy_effect(player, card, trail_of_crumbs_effect, player, card);
        instance = get_card_instance(player, legacy_or_result);
        instance->targets[0].player = player;
        instance->targets[0].card = card;
        instance->number_of_targets = '\x01';
        add_status(player, legacy_or_result, 0x10000);
      }
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
  instance = get_card_instance(player, card);
  if (instance->kill_code == '\0')
  {

    legacy_or_result = duh_mode(player);
    deaths = resolve_gfp_ability(player, card, event,
                                 (legacy_or_result != 0) + RESOLVE_TRIGGER_OPTIONAL);
    deaths = deaths & 0xff;
    while (((deaths != 0 && (legacy_or_result = has_mana(player, 0, 1), legacy_or_result != 0)) &&
            (legacy_or_result =
                 charge_mana_while_resolving(player, card, event, player, COLOR_COLORLESS, 1),
             legacy_or_result != 0)))
    {
      new_default_test_definition(&test_food, 0x47, "Select a permanent card.");
      reveal_top_cards_of_library_and_choose(player, card, player, 2, 1, TUTOR_HAND, 1,
                                             TUTOR_BOTTOM_OF_DECK, 0, &test_food);
      deaths = deaths - 1;
    }
  }
  return global_enchantment(player, card, event);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25cab30; original cards/throne_of_eldraine.c:319. */
int card_alela_artful_provocateur(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  deathtouch(player, card, event);
  lifelink(player, card, event);
  if (event == EVENT_POWER)
  {
    if (affected_card_controller == player)
    {
      result = is_humiliated(player, card, AL_STATIC);
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
  target_definition_t td;
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  vigilance(player, card, event);
  sp_ability_trigger(player, event, 2, TYPE_CREATURE, NULL, player, card);
  if (event == EVENT_SPA)
  {
    test_definition_t test2;
    draw_a_card(player);
    new_default_test_definition(&test2, 1, "Select a land card.");
    new_global_tutor(player, player, 4, 4, 0, 1, &test2);
    return 0;
  }

  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION &&
      event != EVENT_CLEANUP)
    return 0;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    result = instance->targets[0].card;
    instance = get_card_instance(player, card);
    bounce_permanent(instance->targets[0].player, result);
  }
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = player;
  return generic_activated_ability(player, card, event, 5, 3, 0, 0, 0, 0, 0, 0, &td,
                                   "TARGET_CREATURE_YOU_CONTROL");
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
  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    int knights = count_subtype(player, -1, 0x67);
    COST_COLORLESS -= (char)knights;
  }
  boost_creature_type(player, card, event, ~SUBTYPE_INVALID, 1, 1, 0, BCT_CONTROLLER_ONLY);
  if (trigger_condition == 0xd3 && card == affected_card && player == affected_card_controller)
  {
    test_definition_t test;
    default_test_definition(&test, 0xff);
    test.subtype = 0x2001;
    sp_ability_trigger(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_SPA)
  {
    token_generation_t token;
    default_token_definition(player, card, 0x4958, &token);
    token.s_key_plus = SP_KEYWORD_VIGILANCE;
    generate_token(&token);
    return 0;
  }

  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION &&
      event != EVENT_CLEANUP)
    return 0;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    token_generation_t token;
    default_token_definition(player, card, 0x4958, &token);
    token.s_key_plus = SP_KEYWORD_VIGILANCE;
    generate_token(&token);
  }
  return generic_activated_ability(player, card, event, 1, 3, 0, 0, 0, 0, 1, 0, NULL, NULL);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int acclaimed_contender_fn(int iid, int unused, int player, int unused3);

/* ManalinkEh 2022: 0x25c8fd0; original cards/throne_of_eldraine.c:284. */
static int acclaimed_contender_fn(int iid, int unused, int player, int unused3)
{
  int result;

  result = has_subtype_by_id(player, (unsigned int)cards_data[iid].id, SUBTYPE_KNIGHT);
  if (((result == 0) &&
       (result = has_subtype_by_id(player, (unsigned int)cards_data[iid].id, SUBTYPE_AURA),
        result == 0)) &&
      (result = has_subtype_by_id(player, (unsigned int)cards_data[iid].id, SUBTYPE_EQUIPMENT),
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

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  int artifact_or_enchantment;
  int ineligible;

  artifact_or_enchantment = is_what(-1, iid, 0x40);
  ineligible = 1;
  if (artifact_or_enchantment == 0)
  {
    artifact_or_enchantment = is_what(-1, iid, 4);
    ineligible = 0;
    if (artifact_or_enchantment != 0)
    {
      ineligible = has_subtype_by_id(player, (unsigned int)cards_data[iid].id, SUBTYPE_AURA);
      return (unsigned int)(ineligible == 0);
    }
  }
  return ineligible;
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

  xtrigger_t trigger;
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  enable_xtrigger_flags = enable_xtrigger_flags | ENABLE_XTRIGGER_REPLACE_ETB;
  trigger = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    trigger = xtrigger_impl_value_dont_use_directly;
  }
  if ((((trigger == XTRIGGER_REPLACE_ETB) && (card == affected_card)) &&
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
          result = check_special_flags2(trigger_cause_controller, trigger_cause, 0x100);
          if (result == 0)
          {
            result = is_humiliated(player, card, AL_STATIC);
            if (result == 0)
            {
              if (event == EVENT_TRIGGER)
              {
                event_result = event_result | 2;
              }
              else if (event == EVENT_RESOLVE_TRIGGER)
              {
                hack_silent_counters = hack_silent_counters + 1;
                add_counters(player, card, trigger_cause_controller, trigger_cause, COUNTER_P1_P1,
                             1);
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
  const target_t *spell_target;

  spell_target =
      becomes_target_of_spell(player, card, event, player, card, 2, RESOLVE_TRIGGER_MANDATORY);
  if (spell_target != NULL)
  {
    damage_player(spell_target->player, 2, player, card);
  }
  return generic_adventurer(player, card, event, 5, 10, 0x5035);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25d4f20; original cards/throne_of_eldraine.c:4701. */
int card_syr_konrad_the_grim(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  card_instance_t *instance = in_play(player, card);
  if (instance && !is_humiliated(player, card, AL_TRIG))
  {
    test_definition_t this_test;
    default_test_definition(&this_test, 2);
    this_test.not_me = 1;
    unsigned int deaths = whenever_type_is_put_into_graveyard_from_anywhere(player, card, event,
                                                                            player, 0, &this_test);
    if (deaths)
      for (deaths &= 0xff; deaths; --deaths)
        damage_player(1 - player, 1, player, card);
    if (event == EVENT_REMOVED_FROM_GRAVEYARD && affected_card_controller == player &&
        ((unsigned int)event_result >> 16 & 0xff) != 0)
      damage_player(1 - player, (unsigned int)event_result >> 16, player, card);
  }

  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION &&
      event != EVENT_CLEANUP)
    return 0;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    mill(current_turn, 1);
    mill(current_turn ^ 1, 1);
  }
  return generic_activated_ability(player, card, event, 0, 1, 1, 0, 0, 0, 0, 0, NULL, NULL);
}

/* ManalinkEh 2022: 0x25d6010; original cards/throne_of_eldraine.c:5299. */
int card_wicked_wolf(int player, int card, event_t event)
{
  test_definition_t test_food;
  int t_card;
  int result;
  card_instance_t *instance;

  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    target_definition_t td;
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = (int32_t)(1 - player);
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
    return 0;
  }

  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION &&
      event != EVENT_CLEANUP)
    return 0;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    result = instance->parent_controller;
    instance = get_card_instance(player, card);
    t_card = instance->parent_card;
    add_counters(player, card, result, t_card, COUNTER_P1_P1, 1);
    pump_ability_until_eot_no_repeat(player, card, result, t_card, 0, 0x800);
    tap_card(result, t_card);
  }
  new_default_test_definition(&test_food, 0xff, "Select a Food to sacrifice");
  test_food.subtype = 0x3008;
  return generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_NONE, 0, 0, 0, 0,
                                                          0, 0, 0, NULL, NULL, TYPE_NONE,
                                                          &test_food, SARV_0);
}

/* ManalinkEh 2022: 0x25d4b90; original cards/throne_of_eldraine.c:4592. */
/* The prefix at 0x738f34 is an engine-owned runtime string buffer. */
int card_syr_carah_the_bold(int player, int card, event_t event)
{
  int i;
  target_t *damage_target;
  int32_t *amount;
  int32_t target_or_amount;
  target_t *selected_target;
  card_instance_t *damage_or_source;
  card_instance_t *instance;
  int csvid;
  int index_or_result;
  int amts[10];
  target_definition_t td;
  char prompt[200];

  damage_or_source = damage_being_dealt(event);
  if ((((damage_or_source != NULL) && (damage_or_source->damage_source_player == player)) &&
       (((damage_or_source->targets[3].player & 0x18) != 0 ||
         (damage_or_source->damage_source_card == card)))) &&
      ((damage_or_source->damage_target_card == -1 &&
        (index_or_result = damage_is_to_planeswalker(damage_or_source), index_or_result == 0))))
  {
    instance = get_card_instance(player, card);
    index_or_result = instance->info_slot;
    instance->number_of_targets = '\0';
    if (index_or_result < 10)
    {
      instance->info_slot = index_or_result + 1;
      instance->targets[index_or_result].card = damage_or_source->info_slot;
    }
  }
  if ((((trigger_condition == 0xd7) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      ((reason_for_trigger_controller == player &&
        (damage_or_source = get_card_instance(player, card), 0 < damage_or_source->info_slot))))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result = event_result | 2;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      i = 0;
      default_target_definition(player, card, &td, 2);
      td.allow_cancel = 0;
      td.allowed_controller = 1 - player;
      damage_or_source->number_of_targets = '\0';
      if (0 < damage_or_source->info_slot)
      {
        do
        {
          if (can_target(&td) == 0)
            break;
          target_or_amount = damage_or_source->targets[i].card;
          if (ai_is_speculating == 1)
          {
            prompt[0] = '\0';
          }
          else
          {
            load_text(NULL, "TARGET_CREATURE_OPPONENT_CONTROLS");
            csvid = get_id(player, card);
            scnprintf(prompt, 200, "%s %s (%d): %s", (const char *)0x738f34,
                      cards_ptr[csvid]->full_name, target_or_amount, text_lines[0]);
          }
          if (pick_next_target_noload(&td, prompt) != 0)
          {
            amts[damage_or_source->number_of_targets - 1] = target_or_amount;
          }
          i = i + 1;
        } while (i < damage_or_source->info_slot);
      }
      i = 0;
      damage_or_source->info_slot = 0;
      if (damage_or_source->number_of_targets != '\0')
      {
        do
        {
          amount = amts + i;
          selected_target = damage_or_source->targets + i;
          damage_target = damage_or_source->targets + i;
          i = i + 1;
          damage_creature(damage_target->player, selected_target->card, *amount, player, card);
        } while (i < (int)(unsigned int)damage_or_source->number_of_targets);
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25ce410; original cards/throne_of_eldraine.c:1720. */
int card_fabled_passage(int player, int card, event_t event)
{
  enum
  {
    CHOICE_MANA = 1,
    CHOICE_ABILITY = 2
  };

  if (event == EVENT_RESOLVE_SPELL)
  {
    play_land_sound_effect(player, card);
    if (mana_colors_added[player])
      return mana_producer(player, card, event);
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (mana_colors_added[player])
      return mana_producer(player, card, EVENT_CAN_ACTIVATE);
    in_play(player, card);
  }
  else if (event == EVENT_ACTIVATE)
  {
    card_instance_t *instance = in_play(player, card);
    ai_modifier += 100;
    if (!mana_colors_added[player])
    {
      instance->info_slot = 2;
      return generic_activated_ability(player, card, EVENT_ACTIVATE, 9, 0, 0, 0, 0, 0, 0, 0, NULL,
                                       NULL);
    }
    bool use_mana = false;
    if (paying_mana() || !generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 1, 0, 0, 0,
                                                    0, 0, 0, NULL, NULL))
    {
      instance->info_slot = 1;
      use_mana = true;
    }
    else
    {
      bool human = ai_is_speculating != 1;
      instance->info_slot = 2;
      if (human && (player != 1 || ((unsigned char)trace_mode & 2) != 0))
      {
        int choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                                 "Use Ability", 1, 1, 0, 0, 0, 0);
        if (choice == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (choice == CHOICE_MANA)
          use_mana = true;
        else if (choice != CHOICE_ABILITY)
          return generic_activated_ability(player, card, event, 9, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
      }
      if (!use_mana)
        generic_activated_ability(player, card, EVENT_ACTIVATE, 9, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
    }
    if (use_mana)
    {
      if (!can_produce_mana(player, card))
        return 0;
      if (check_special_flags2(player, card, 0x40) && is_what(player, card, 1))
      {
        produce_mana_tapped(player, card, COLOR_BLACK, 1);
        return 0;
      }
      color_test_t colors = mana_colors_added[player];
      unsigned int color_count = num_bits_set(colors);
      if (color_count == 1)
      {
        color_t color = single_color_test_bit_to_color(colors);
        produce_mana_tapped(player, card, color, 1);
      }
      else
        produce_mana_tapped_all_one_color(player, card, colors, 1);
      return 0;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION || event == EVENT_CLEANUP)
  {
    card_instance_t *instance = in_play(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION && instance->info_slot == 2)
    {
      test_definition_t this_test;
      new_default_test_definition(&this_test, 1, "Select a basic land card.");
      this_test.subtype = 0x2000;
      int land = new_global_tutor(player, player, 1, 5, 0, 5, &this_test);
      if (land >= 0 && count_permanents_by_type(player, TYPE_LAND) > 3)
      {
        if (ai_is_speculating == 1)
          ai_modifier += 0xf;
        else
          untap_card(player, land);
      }
    }
  }
  else
  {
    if (mana_colors_added[player])
      return mana_producer(player, card, event);
    return 0;
  }
  return generic_activated_ability(player, card, event, 9, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
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
  token_generation_t token;
  test_definition_t test;

  upkeep_trigger_ability(player, card, event, 2);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    new_default_test_definition(&test, 0x47,
                                     "Select a nonland, nontoken permanent to sacrifice.");
    test.special_selection_function = nonland_nontoken_permanent;
    result = new_sacrifice(player, card, current_turn,
                                SAC_RETURN_CHOICE | SAC_CAUSED | SAC_NO_CANCEL, &test);
    if (result == 0)
    {
      discard(current_turn, 2, player);
      lose_life(current_turn, 2);
      draw_a_card(player);
      gain_life(player, 2);
      default_token_definition(player, card, 0x4958, &token);
      token.s_key_plus = 8;
      generate_token(&token);
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25d04e0; original cards/throne_of_eldraine.c:2556. */
int card_granted(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST &&
      !(event == EVENT_CAST_SPELL && card == affected_card && player == affected_card_controller) &&
      event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET &&
      event != EVENT_CHANGE_TARGET && event != EVENT_COUNTERSPELL && event != EVENT_CHANGE_TYPE &&
      event != EVENT_SET_COLOR)
    return 0;

  if (event == EVENT_CAN_CAST || event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CAST_SPELL)
    return basic_spell(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    test_definition_t test;
    default_test_definition(&test, 2);
    test.type_flag = 1;
    generic_wish_effect(player, card, &test, 0x50ca);
  }
  return adventure(player, card, event, 0x5064);
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 flag and combat-query consumers. */
int card_embercleave(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25cdd60; original cards/throne_of_eldraine.c:1554. */
int card_embercleave(int player, int card, event_t event)
{
  int value;
  card_instance_t *instance;
  target_definition_t td;
  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    if (event == EVENT_CAN_CAST)
    {
      return flash(player, card, event);
    }
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        return flash(player, card, event);
    }
    else if (event == EVENT_MODIFY_COST)
    {
      value = count_attackers(player);
      COST_COLORLESS = COST_COLORLESS - (char)value;
    }
    return vanilla_equipment(player, card, event, 3, 1, 1,
                             (KEYWORD_DOUBLE_STRIKE | KEYWORD_TRAMPLE), 0);
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.allowed_controller = player;
  td.allow_cancel = 0;
  instance = get_card_instance(player, card);
  value = can_target(&td);
  if ((value != 0) &&
      (value = new_pick_target(&td, "Select target creature you control.", 0, 0x80), value != 0))
  {
    value = -0x2a;
    if (!is_sick(instance->targets[0].player, instance->targets[0].card) &&
        !check_for_ability(instance->targets[0].player, instance->targets[0].card,
                           KEYWORD_DEFENDER))
      value = 0x2a;
    ai_modifier = ai_modifier + value;
    equip_target_creature(player, card, instance->targets[0].player, instance->targets[0].card);
  }
  instance->number_of_targets = '\0';
  return vanilla_equipment(player, card, event, 3, 1, 1, (KEYWORD_DOUBLE_STRIKE | KEYWORD_TRAMPLE),
                           0);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25cd080; original cards/throne_of_eldraine.c:1261. */
int card_dance_of_the_manse(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST)
  {
    get_card_instance(player, card);
    return generic_spell(player, card, EVENT_CAN_CAST, 4, NULL, NULL, 0, NULL);
  }
  if (event != EVENT_CAST_SPELL)
  {
    if (event == EVENT_CHANGE_TARGET)
    {
      get_card_instance(player, card);
      return 0;
    }
    if (event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET)
      return 0;
    card_instance_t *instance = get_card_instance(player, card);
    if (event != EVENT_RESOLVE_SPELL)
      return 0;
    int x = instance->info_slot;
    for (int index = 0; index < x; ++index)
    {
      int grave_index = validate_target_from_grave(player, card, player, index);
      if (grave_index != -1)
      {
        int permanent = reanimate_permanent(player, card, player, grave_index, REANIMATE_DEFAULT);
        if (x >= 6)
          turn_into_creature(player, card, player, permanent, 0, 4, 4);
      }
    }
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  if (card != affected_card || player != affected_card_controller)
    return 0;
  card_instance_t *instance = get_card_instance(player, card);
  if (card != affected_card || player != affected_card_controller)
    return 0;
  if (check_special_flags2(player, card, -0x80000000))
    return 0;
  int available_mana = has_mana(player, 7, 1);
  test_definition_t test;
  default_test_definition(&test, 0xff);
  test.cmc = available_mana + 1;
  test.special_selection_function = dotm_selection;
  test.cmc_flag = 3;
  int available_cards = new_special_count_grave(player, &test);
  int maximum = available_mana;
  if (available_cards < maximum)
    maximum = available_cards;
  if (maximum < 1 || graveyard_has_shroud(player))
  {
    instance->info_slot = 0;
    ai_modifier -= 0x30;
    return 0;
  }
  int limit = maximum > 19 ? 19 : maximum;
  char buffer[100];
  if (ai_is_speculating == 1)
    buffer[0] = 0;
  else if (maximum == 1)
    scnprintf(buffer, 100, "Select artifact or non-Aura ench. card with cmc %d or less.",
              available_mana);
  else
    sprintf(buffer, "Select up to %d art. or non-Aura ench. with cmc %d or less.", limit,
            available_mana);
  new_default_test_definition(&test, 0xff, buffer);
  int selected =
      select_multiple_cards_from_graveyard(player, player, 0, 1, &test, limit, instance->targets);
  int highest_cmc = 0;
  for (int index = 0; index < selected; ++index)
  {
    int cmc = get_cmc_by_internal_id(instance->targets[index].card);
    if (highest_cmc < cmc)
      highest_cmc = cmc;
  }
  int required = selected < highest_cmc ? highest_cmc : selected;
  int paid;
  if (player == 1)
  {
    int desired = required < 6 ? 6 : required;
    paid = charge_mana(1, COLOR_COLORLESS, desired);
    if (spell_fizzled == 1)
    {
      spell_fizzled = 0;
      paid = charge_mana(1, COLOR_COLORLESS, required);
    }
  }
  else
    paid = charge_mana(player, COLOR_COLORLESS, -1);
  if (paid < required)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (spell_fizzled == 1)
    return 0;
  set_x_for_x_spells(player, card, EVENT_CAST_SPELL, paid);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25cf1b0; original cards/throne_of_eldraine.c:2027. */
int card_fires_of_invention(int player, int card, event_t event)
{
  if (!is_humiliated(player, card, AL_STATIC))
  {
    if (event == EVENT_MODIFY_COST_GLOBAL)
    {
      if (affected_card_controller == player && !is_what(player, affected_card, 1) &&
          (get_specific_storm_count(affected_card_controller) > 1 || current_turn != player))
        infinite_casting_cost();
    }
    else if (event == EVENT_CAN_ACTIVATE)
    {
      if (get_specific_storm_count(player) > 1 || current_turn != player)
        return 0;
      int lands = count_permanents_by_type(player, TYPE_LAND);
      for (int i = 0; i < active_cards_count[player]; ++i)
      {
        if (in_hand(player, i) && !is_what(player, i, 1) && get_cmc(player, i) <= lands)
        {
          int legal = can_legally_play_card_in_hand(player, i);
          if ((legal == 99 && (is_what(player, i, 0x20) || is_what(player, i, 0x22))) ||
              (legal == 1 &&
               (is_what(player, i, 0x10) || can_sorcery_be_played(player, EVENT_CAN_ACTIVATE) ||
                is_what(player, i, 2) || can_sorcery_be_played(player, EVENT_CAN_ACTIVATE) ||
                flash(player, i, EVENT_CAN_ACTIVATE))))
          {
            return legal;
          }
        }
      }
      return 0;
    }
    else if (event == EVENT_ACTIVATE && get_specific_storm_count(player) < 2 &&
             current_turn == player)
    {
      unsigned capacity = (unsigned)hand_count[player];
      int playable[2][capacity];
      int lands = count_permanents_by_type(player, TYPE_LAND);
      int count = 0;
      for (int i = 0; i < active_cards_count[player]; ++i)
      {
        if (in_hand(player, i) && !is_what(player, i, 1) && get_cmc(player, i) <= lands)
        {
          int legal = can_legally_play_card_in_hand(player, i);
          if ((legal == 99 && (is_what(player, i, 0x20) || is_what(player, i, 0x22))) ||
              (legal == 1 &&
               (is_what(player, i, 0x10) || can_sorcery_be_played(player, EVENT_CAN_ACTIVATE) ||
                is_what(player, i, 2) || can_sorcery_be_played(player, EVENT_CAN_ACTIVATE) ||
                flash(player, i, EVENT_CAN_ACTIVATE))))
          {
            playable[0][count] = get_card_instance(player, i)->internal_card_id;
            playable[1][count++] = i;
          }
        }
      }
      char buffer[100];
      scnprintf(buffer, 100,
                "Select a non-land card with mana value less than or equal to %d to play.", lands);
      test_definition_t test;
      _Static_assert(sizeof(test) == 216, "native Fires of Invention filter");
      new_default_test_definition(&test, 1, buffer);
      test.cmc = lands + 1;
      test.cmc_flag = 3;
      test.type_flag = 1;
      int selected =
          select_card_from_zone(player, player, playable[0], count, 0, 2, -1, &test);
      if (selected == -1)
      {
        spell_fizzled = 1;
        return 0;
      }
      play_card_in_hand_for_free(player, playable[1][selected]);
      cant_be_responded_to = 1;
    }
  }
  return global_enchantment(player, card, event);
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 private inline definitions. See per-helper limits in the recovery evidence. */
static int resolve_an_adventure(int player, int card, int csvid_creature)
{
  card_instance_t *instance = NULL;
  int legacy = -1;
  for (int index = 0; index < active_cards_count[player]; ++index)
  {
    instance = in_play(player, index);
    if (instance && is_what(player, index, 0x80) && instance->info_slot_fn == adventure_effect &&
        instance->targets[15].player == -1 && instance->targets[15].card == -1)
    {
      legacy = index;
      break;
    }
  }
  target_t *slot = NULL;
  if (legacy >= 0)
  {
    instance = get_card_instance(player, legacy);
    for (int index = 1; index < 16; ++index)
      if (instance->targets[index].player == -1 && instance->targets[index].card == -1)
      {
        slot = &instance->targets[index];
        break;
      }
  }
  else
  {
    instance = get_card_instance(player, create_legacy_activate(player, card, adventure_effect));
    instance->display_pic_csv_id = 0x616c;
    instance->display_pic_num = 0;
    slot = instance->targets;
  }
  if (slot)
  {
    slot->player = get_owner(player, card);
    slot->card = get_original_internal_card_id(player, card);
  }
  obliterate_card(player, card);
  add_csvid_to_rfg(get_owner(player, card), csvid_creature);
  return 0;
}
#endif
