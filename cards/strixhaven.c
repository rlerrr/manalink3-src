#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *is_monocolored(int who_chooses, int player, int card, int targeting_player,
                            int targeting_card);

/* ManalinkEh 2022: 0x23c6490; original cards/strixhaven.c:113. */
static int academic_probation_legacy(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int player_00;

  if (event == EVENT_ABILITIES)
  {
    instance = get_card_instance(player, card);
    if (((-1 < instance->damage_target_player) &&
         (instance->damage_target_card == affected_card)) &&
        (instance->damage_target_player == affected_card_controller))
    {
      instance = get_card_instance(player, card);
      player_00 = (int)instance->damage_target_player;
      card_00 = instance->damage_target_card;
      disable_all_activated_abilities_2022(player_00, card_00, 1);
      cannot_attack_2022(player_00, card_00, EVENT_ABILITIES);
      cannot_block(player_00, card_00, EVENT_ABILITIES);
    }
  }
  else if ((event == EVENT_CAN_SKIP_TURN) && (current_turn == player))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23c9e00; original cards/strixhaven.c:496. */
int card_basic_conjuration(int player, int card, event_t event)
{
  int result;
  test_definition_t test3;
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&test3, 2, "Select a creature card to reveal.");
    reveal_top_cards_of_library_and_choose(player, card, player, 6, 0, TUTOR_HAND, 1,
                                           TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &test3);
    gain_life_2022(player, 3);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23ca7a0; original cards/strixhaven.c:698. */
int card_blood_age_general(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  test_definition_t test3;
  if (event == EVENT_ACTIVATE)
  {
    if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
    {
      default_test_definition(&test3, 2);
      test3.state = 4;
      test3.subtype = 0xbc;
      iVar2 = check_battlefield_for_special_card(player, card, player, 4, &test3);
      iVar1 = iVar2 * 8;
      if ((current_turn != player) || (0x18 < current_phase))
      {
        iVar1 = iVar2 * -8;
      }
      ai_modifier = ai_modifier + iVar1;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    default_test_definition(&test3, 2);
    test3.state = 4;
    test3.subtype = 0xbc;
    pump_creatures_until_eot(player, card, 2, 0, 1, 0, 0, 0, &test3);
  }
  iVar1 = generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x23c68c0; original cards/strixhaven.c:891. */
static int codie_effect(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if ((((trigger_condition == 0xd3) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (result = check_special_flags_2022(trigger_cause_controller, trigger_cause, 0x40000), result == 0))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result = event_result | 2;
      return 0;
    }
    if (event == EVENT_RESOLVE_TRIGGER)
    {
      default_test_definition(&test, 0xff);
      result = new_specific_spell_played(player, card, EVENT_RESOLVE_TRIGGER, player, 2, &test);
      if (result == 0)
      {
        return 0;
      }
      default_test_definition(&test, 0x38);
      test.cmc = get_cmc(trigger_cause_controller, trigger_cause);
      test.cmc_flag = 3;
      result = reveal_card_until_you_reveal_test(player, player, &test, 0xa04);
      if (-1 < result)
      {
        create_may_play_card_from_exile_effect(player, card, player,
                                               (unsigned int)cards_data[result].id,
                                               MPCFE_UNTIL_END_OF_YOUR_NEXT_TURN | MPCFE_FOR_FREE);
      }
      goto LAB_023c6a02;
    }
  }
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
LAB_023c6a02:
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x23cba70; original cards/strixhaven.c:1160. */
int card_crushing_disappointment(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    lose_life_2022(player, 2);
    lose_life_2022(1 - player, 2);
    draw_cards(player, 2);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23cbaf0; original cards/strixhaven.c:1175. */
int card_culling_ritual(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  test_definition_t test;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_test_definition(&test, 1);
      test.type_flag = 1;
      test.cmc = 3;
      test.cmc_flag = 3;
      iVar1 = check_battlefield_for_special_card(player, card, 1 - player, 4, &test);
      iVar2 = check_battlefield_for_special_card(player, card, player, 4, &test);
      ai_modifier = ai_modifier + iVar1 * 8 + iVar2 * -10;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&test, 1);
    test.type_flag = 1;
    test.cmc = 3;
    test.cmc_flag = 3;
    iVar1 = new_manipulate_all(player, card, 2, &test, ACT_KILL_DESTROY);
    produce_mana_any_combination_of_colors(player, COLOR_TEST_GREEN | COLOR_TEST_BLACK, iVar1,
                                           "Select any combination of B and/or G");
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x23cbdd0; original cards/strixhaven.c:1238. */
int card_curate(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  test_definition_t test3;
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&test3, 0xff, "Select a card to put into your graveyard.");
    iVar2 = count_deck(player);
    iVar1 = iVar2;
    if (0 < iVar2)
    {
      iVar1 = 1;
    }
    do
    {
      if (iVar2 == 0)
        break;
      test3.create_minideck = (unsigned short)iVar1;
      iVar3 = new_global_tutor(player, player, 1, 3, 0, 3, &test3);
    } while (iVar3 != -1);
    draw_a_card(player);
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x23cc050; original cards/strixhaven.c:1297. */
int card_deadly_brew(int player, int card, event_t event)
{
  int result;
  test_definition_t test3;
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&test3, 0x102, "Select a creature or planeswalker to sacrifice.");
    result = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_CAUSED, &test3);
    new_sacrifice(player, card, 1 - player, SAC_CAUSED, &test3);
    if (result != 0)
    {
      new_default_test_definition(&test3, 0x47, "Select a permanent card.");
      new_global_tutor(player, player, 2, 1, 0, 1, &test3);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23cd240; original cards/strixhaven.c:1608. */
int card_double_major(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td2;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        counterspell_target_definition(player, card, &td2, 2);
        td2.allowed_controller = player;
        get_card_instance(player, card);
        if ((card == affected_card) && (player == affected_card_controller))
          goto LAB_023cd380;
      }
    }
    else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      counterspell_target_definition(player, card, &td2, 2);
      td2.allowed_controller = player;
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = counterspell_validate(player, card, &td2, 0);
        if (result != 0)
        {
          result =
              copy_spell_from_stack(player, instance->targets[0].player, instance->targets[0].card);
          if (-1 < result)
          {
            set_special_flags3_2022(player, result, 0x1000000);
          }
        }
        kill_card(player, card, KILL_DESTROY);
      }
    }
    else if (event == EVENT_CHANGE_TARGET)
    {
      counterspell_target_definition(player, card, &td2, 2);
      td2.allowed_controller = player;
      get_card_instance(player, card);
      return 0;
    }
    return 0;
  }
  counterspell_target_definition(player, card, &td2, 2);
  td2.allowed_controller = player;
  get_card_instance(player, card);
LAB_023cd380:
  result = counterspell(player, card, event, &td2, 0);
  return result;
}

/* ManalinkEh 2022: 0x23ce2b0; original cards/strixhaven.c:1960. */
int card_efreet_flamepainter(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  result = damage_dealt_by_me(player, card, event,
                              DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE |
                                  DDBM_TRIGGER_OPTIONAL);
  if (result != 0)
  {
    new_default_test_definition(&this_test, 0x38, "Select an instant or sorcery card.");
    result = new_special_count_grave(player, &this_test);
    if (result != 0)
    {
      result = new_select_a_card(player, player, 2, 0, 2, -1, &this_test);
      if (result != -1)
      {
        play_card_in_grave_for_free_and_exile_it(player, player, result);
        return 0;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23ce830; original cards/strixhaven.c:2126. */
int card_environmental_sciences(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    tutor_basic_land(player, 0, 0);
    gain_life_2022(player, 2);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23ce8a0; original cards/strixhaven.c:2146. */
int card_eureka_moment(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    draw_cards(player, 2);
    new_default_test_definition(&this_test, 1, "Select a land card.");
    new_global_tutor(player, player, 4, 4, 0, 1, &this_test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23ced10; original cards/strixhaven.c:2248. */
int card_expressive_iteration(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  test_definition_t test;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      ai_modifier = ai_modifier + 0x60;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    iVar2 = count_deck(player);
    iVar1 = iVar2;
    if (3 < iVar2)
    {
      iVar1 = 3;
    }
    if (iVar2 != 0)
    {
      iVar2 = 1;
      do
      {
        while (iVar2 != 1)
        {
          if (iVar2 == 2)
          {
            new_default_test_definition(&test, 0xff,
                                        "Select a card to put on the bottom of your library.");
            test.no_shuffle = 1;
            test.create_minideck = (unsigned short)iVar1;
            new_global_tutor(player, player, 1, 10, (unsigned int)(iVar1 == 3), 1, &test);
          }
          else if (iVar2 == 3)
          {
            exile_top_x_card_you_may_play_them(player, card, 1, NULL, player, MPCFE_UNTIL_EOT);
          }
          iVar2 = iVar2 + 1;
          iVar1 = iVar1 + -1;
          if (iVar1 == 0)
            goto LAB_023cee00;
        }
        new_default_test_definition(&test, 0xff, "Select a card to add to your hand.");
        test.no_shuffle = 1;
        iVar2 = 2;
        test.create_minideck = (unsigned short)iVar1;
        new_global_tutor(player, player, 1, 1, (unsigned int)(iVar1 == 3), 1, &test);
        iVar1 = iVar1 + -1;
      } while (iVar1 != 0);
    }
  LAB_023cee00:
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x23cfdd0; original cards/strixhaven.c:2642. */
int card_fracture2(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x144);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_023cfe36;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x144);
  get_card_instance(player, card);
LAB_023cfe36:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target artifact, enchantment, or planeswalker.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23d0410; original cards/strixhaven.c:2790. */
int card_hall_monitor(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  haste(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event != EVENT_ACTIVATE)
      goto LAB_023d048f;
    generic_activated_ability(player, card, EVENT_ACTIVATE, 5, 1, 0, 0, 0, 1, 0, 0, &td,
                              "TARGET_CREATURE");
    if (spell_fizzled != 1)
    {
      ai_modifier = ai_modifier + 0x18;
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
  LAB_023d048f:
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        pump_ability_until_eot_no_repeat_2022(instance->parent_controller, instance->parent_card,
                                         instance->targets[0].player, instance->targets[0].card, 0,
                                         0x20);
      }
    }
    result = generic_activated_ability(player, card, event, 5, 1, 0, 0, 0, 1, 0, 0, &td,
                                       "TARGET_CREATURE");
    return result;
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d1390; original cards/strixhaven.c:3259. */
int card_introduction_to_annihilation(int player, int card, event_t event)
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
        td.illegal_type = 1;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
            draw_cards(instance->targets[0].player, 1);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_023d13fd;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.illegal_type = 1;
  get_card_instance(player, card);
LAB_023d13fd:
  result = generic_spell(player, card, event, 1, &td, "TARGET_NONLAND_PERMANENT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23d4f50; original cards/strixhaven.c:4497. */
int card_mortality_spear(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_MODIFY_COST)
  {
    result = get_trap_condition(player, 1);
    if (result != 0)
    {
      COST_COLORLESS = COST_COLORLESS + -2;
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
        default_target_definition(player, card, &td, 0x47);
        td.illegal_type = 1;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_023d4fc5;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.illegal_type = 1;
  get_card_instance(player, card);
LAB_023d4fc5:
  result = generic_spell(player, card, event, 1, &td, "TARGET_NONLAND_PERMANENT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23d72b0; original cards/strixhaven.c:5127. */
int card_practical_research(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event != EVENT_RESOLVE_SPELL)
    goto LAB_023d72c7;
  draw_cards(player, 4);
  new_default_test_definition(&test, 0x38, "Select an instant or sorcery card to discard");
  test.zone = 0x100;
  result = check_battlefield_for_special_card(player, card, player, 0, &test);
  if (result == 0)
  {
  LAB_023d7340:
    multidiscard(player, 2, 0);
  }
  else
  {
    result = new_select_a_card(player, player, 4, 0, 3, -1, &test);
    if (result == -1)
      goto LAB_023d7340;
    discard_card(player, result);
  }
  kill_card(player, card, KILL_SACRIFICE);
LAB_023d72c7:
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23d9370; original cards/strixhaven.c:5813. */
int card_reduce_to_memory(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  token_generation_t token;
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
        td.illegal_type = 1;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            default_token_definition(player, card, 0x498d, &token);
            token.t_player = instance->targets[0].player;
            token.pow = 3;
            token.tou = 2;
            token.color_forced = '0';
            generate_token(&token);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_023d93e7;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.illegal_type = 1;
  get_card_instance(player, card);
LAB_023d93e7:
  result = generic_spell(player, card, event, 1, &td, "TARGET_NONLAND_PERMANENT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23d9540; original cards/strixhaven.c:5871. */
int card_resculpt(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  token_generation_t token;
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
        default_target_definition(player, card, &td, 0x42);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            result = instance->targets[0].player;
            kill_card(result, instance->targets[0].card, KILL_EXILE);
            default_token_definition(player, card, 0x4926, &token);
            token.tou = 4;
            token.pow = 4;
            token.color_forced = '\x14';
            token.t_player = result;
            generate_token(&token);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_023d95ac;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x42);
  get_card_instance(player, card);
LAB_023d95ac:
  result =
      generic_spell(player, card, event, 0x81, &td, "Select target artifact or creature.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23d9b90; original cards/strixhaven.c:5986. */
int card_rootha_mercurial_artist(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  check_legend_rule(player, card, event);
  if (event == EVENT_ACTIVATE)
  {
    counterspell_target_definition(player, card, &td, 0x38);
    td.allowed_controller = player;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    counterspell_target_definition(player, card, &td, 0x38);
    td.allowed_controller = player;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      result = counterspell_validate(player, card, &td, 0);
      if (result != 0)
      {
        copy_spell_from_stack_2022(player, instance->targets[0].player, instance->targets[0].card);
      }
    }
  }
  result = generic_activated_ability(player, card, event, 0x20400, 2, 0, 0, 0, 0, 0, 0, &td, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23da640; original cards/strixhaven.c:6264. */
int card_secret_rendezvous(int player, int card, event_t event)
{
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
    else if (event == EVENT_RESOLVE_SPELL)
    {
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      result = valid_target(&td);
      if (result != 0)
      {
        draw_cards(player, 3);
        draw_cards(1 - player, 3);
      }
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  result = generic_spell(player, card, event, 0x40, NULL, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23daa60; original cards/strixhaven.c:6352. */
int card_semesters_end(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  target_definition_t *ptStack_68;
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
        td.preferred_controller = player;
        td.allowed_controller = player;
        td.allow_cancel = 3;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          if (instance->number_of_targets != '\0')
          {
            iVar5 = 0;
            do
            {
              while (iVar4 = validate_target(player, card, &td, iVar5), iVar4 == 0)
              {
                iVar5 = iVar5 + 1;
                if ((int)(unsigned int)instance->number_of_targets <= iVar5)
                  goto LAB_023dab39;
              }
              ptVar2 = instance->targets + iVar5;
              ptVar1 = instance->targets + iVar5;
              iVar5 = iVar5 + 1;
              remove_until_eot(player, card, ptVar1->player, ptVar2->card);
            } while (iVar5 < (int)(unsigned int)instance->number_of_targets);
          }
        LAB_023dab39:
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_023daba2;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x102);
  td.preferred_controller = player;
  td.allowed_controller = player;
  td.allow_cancel = 3;
  get_card_instance(player, card);
LAB_023daba2:
  ptStack_68 = &td;
  iVar5 = generic_spell(player, card, event, 0xa0, ptStack_68,
                        "Select target creature or planeswalker you control.", 10, NULL);
  return iVar5;
}

/* ManalinkEh 2022: 0x23c8540; original cards/strixhaven.c:6513. */
static int player_is_valid_target(int player, int card, int t_player)
{
  uint8_t uVar1;
  int32_t iVar2;
  int32_t iVar3;
  card_instance_t *instance;
  int iVar5;
  target_definition_t td;

  instance = get_card_instance(player, card);
  default_target_definition(player, card, &td, 0);
  td.allow_cancel = 0;
  iVar2 = instance->targets[0].player;
  iVar3 = instance->targets[0].card;
  uVar1 = instance->number_of_targets;
  instance->targets[0].player = t_player;
  instance->targets[0].card = -1;
  instance->number_of_targets = '\x01';
  td.zone = 0x1000;
  td.preferred_controller = t_player;
  iVar5 = would_valid_target(&td);
  instance->targets[0].player = iVar2;
  instance->targets[0].card = iVar3;
  instance->number_of_targets = uVar1;
  return iVar5;
}

/* ManalinkEh 2022: 0x23dc7f0; original cards/strixhaven.c:7043. */
int card_solve_the_equation(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&this_test, 0x38, "Select an instant or sorcery card.");
    new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23dd6a0; original cards/strixhaven.c:7412. */
int card_teach_by_example(int player, int card, event_t event)
{
  int result;
  if (event != EVENT_CAN_CAST)
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      if (event == EVENT_MODIFY_COST)
      {
        modify_cost_for_hybrid_spells(player, card, EVENT_MODIFY_COST, 0);
        return 0;
      }
      if ((((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
           (player == affected_card_controller)) &&
          (result = hybrid_casting(player, card, 0), result == 0))
      {
        spell_fizzled = 1;
      }
      return (unsigned int)(event == EVENT_CAN_CAST);
    }
    create_legacy_effect(player, card, doublecast_legacy);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23c85d0; original cards/strixhaven.c:8002. */
static char *is_monocolored(int who_chooses, int player, int card, int targeting_player,
                            int targeting_card)
{
  int result;
  result = count_colors(player, card);
  if (result != 1)
  {
    return "must be monocolored.";
  }
  return NULL;
}

/* ManalinkEh 2022: 0x23df040; original cards/strixhaven.c:8007. */
int card_vanishing_verse(int player, int card, event_t event)
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
        td.special = 0x8000;
        td.extra = (int32_t)is_monocolored;
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
        goto LAB_023df0b8;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.special = 0x8000;
  td.extra = (int32_t)is_monocolored;
  get_card_instance(player, card);
LAB_023df0b8:
  result =
      generic_spell(player, card, event, 0x81, &td, "Select target monocolored permanent", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23df310; original cards/strixhaven.c:8068. */
int card_venerable_warsinger(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;
  vigilance_2022(player, card, event);
  result = damage_dealt_by_me(player, card, event,
                              DDBM_MUST_DAMAGE_PLAYER | DDBM_REPORT_DAMAGE_DEALT |
                                  DDBM_MUST_BE_COMBAT_DAMAGE | DDBM_TRIGGER_OPTIONAL);
  if (result == 0)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  result = instance->targets[0x10].player;
  instance = get_card_instance(player, card);
  instance->targets[0x10].player = 0;
  default_test_definition(&test, 2);
  test.cmc = result + 1;
  test.cmc_flag = 3;
  scnprintf(test.message, 100, "Choose a creature card with MV %d or less", result);
  new_global_tutor(player, player, 2, 4, 0, 3, &test);
  return 0;
}

/* ManalinkEh 2022: 0x23e0890; original cards/strixhaven.c:8492. */
int card_zephyr_boots(int player, int card, event_t event)
{
  int result;

  result = equipped_creature_deals_damage(player, card, event,
                                          DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (0 < result)
  {
    do
    {
      draw_a_card(player);
      discard(player, 0, player);
      result = result + -1;
    } while (result != 0);
  }
  result = vanilla_equipment(player, card, event, 2, 0, 0, 0x20, 0);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static void make_pest(int player, int card, int amount);
static void make_inkling(int player, int card, int amount, int action);

/* ManalinkEh 2022: 0x23c6810; original cards/strixhaven.c:8. */
static void make_pest(int player, int card, int amount)
{
  token_generation_t token;

  default_token_definition(player, card, 0x4970, &token);
  token.qty = (unsigned char)amount;
  token.color_forced = '\n';
  token.tou = 1;
  token.pow = 1;
  token.special_infos = 0x42;
  generate_token(&token);
  return;
}

/* ManalinkEh 2022: 0x23c6860; original cards/strixhaven.c:29. */
static void make_inkling(int player, int card, int amount, int action)
{
  token_generation_t token;

  default_token_definition(player, card, 0x58a7, &token);
  token.qty = (unsigned char)amount;
  token.pow = 2;
  token.tou = 1;
  token.key_plus = 0x20;
  token.color_forced = '\"';
  token.action = action;
  generate_token(&token);
  return;
}

/* ManalinkEh 2022: 0x23ca980; original cards/strixhaven.c:737. */
int card_blot_out_the_sky(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;

  if (event == EVENT_RESOLVE_SPELL)
  {
    instance = get_card_instance(player, card);
    make_inkling(player, card, instance->info_slot, 1);
    if (5 < instance->info_slot)
    {
      default_test_definition(&this_test, 3);
      this_test.type_flag = 1;
      new_manipulate_all(player, card, 2, &this_test, ACT_KILL_DESTROY);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23cb610; original cards/strixhaven.c:1075. */
int card_containment_breach(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x44);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            result = get_cmc(instance->targets[0].player, instance->targets[0].card);
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            if (result < 3)
            {
              make_pest(player, card, 1);
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_023cb676;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x44);
  get_card_instance(player, card);
LAB_023cb676:
  result = generic_spell(player, card, event, 1, &td, "DISENCHANT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23d1330; original cards/strixhaven.c:3244. */
int card_inkling_summoning(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    make_inkling(player, card, 1, 0);
    kill_card(player, card, KILL_DESTROY);
  }
  if (event != EVENT_MODIFY_COST)
  {
    if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      result = hybrid_casting(player, card, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
      }
    }
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  modify_cost_for_hybrid_spells(player, card, EVENT_MODIFY_COST, 0);
  return 0;
}

/* ManalinkEh 2022: 0x23d5d30; original cards/strixhaven.c:4746. */
int card_pest_summoning(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    make_pest(player, card, 2);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23d8120; original cards/strixhaven.c:5432. */
int card_professor_of_zoomancy(int player, int card, event_t event)
{
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  make_pest(player, card, 1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23ca940; original cards/strixhaven.c:727. */
int card_blood_researcher(int player, int card, event_t event)
{
  int result;

  menace(player, card, event);
  result = card_ajanis_pridemate(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23cb790; original cards/strixhaven.c:1102. */
int card_crackle_with_power(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 3;
    get_card_instance(player, card);
    iVar4 = generic_spell(player, card, EVENT_CAN_CAST, 4, NULL, NULL, 0, NULL);
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        default_target_definition(player, card, &td, 0x102);
        td.zone = 0x1200;
        td.allow_cancel = 3;
        get_card_instance(player, card);
        if ((card == affected_card) && (player == affected_card_controller))
        {
          multi_x_spell(player, card, EVENT_CAST_SPELL, 3);
          iVar4 =
              generic_spell(player, card, EVENT_CAST_SPELL, 0x25, &td, "TARGET_ANY", x_value, NULL);
          return iVar4;
        }
      }
    }
    else if ((event == EVENT_RESOLVE_SPELL || event == EVENT_CAN_CHANGE_TARGET) ||
             (event == EVENT_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      td.allow_cancel = 3;
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {

        iVar4 = 0;
        if (instance->number_of_targets != '\0')
        {
          do
          {
            while (iVar5 = validate_target(player, card, &td, iVar4), iVar5 == 0)
            {
              iVar4 = iVar4 + 1;
              if ((int)(unsigned int)instance->number_of_targets <= iVar4)
                goto LAB_023cb883;
            }
            ptVar2 = instance->targets + iVar4;
            ptVar1 = instance->targets + iVar4;
            iVar4 = iVar4 + 1;
            damage_creature(ptVar1->player, ptVar2->card, instance->info_slot * 5, player, card);
          } while (iVar4 < (int)(unsigned int)instance->number_of_targets);
        }
      LAB_023cb883:
        kill_card(player, card, KILL_DESTROY);
      }
    }
    iVar4 = 0;
  }
  return iVar4;
}

/* ManalinkEh 2022: 0x23ceb00; original cards/strixhaven.c:2210. */
int card_exponential_growth(int player, int card, event_t event)
{
  CardFunction p_Var1;
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    get_card_instance(player, card);
    result = generic_spell(player, card, EVENT_CAN_CAST, 5, &td, NULL, 1, NULL);
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        default_target_definition(player, card, &td, 2);
        td.preferred_controller = player;
        get_card_instance(player, card);
        if ((card == affected_card) && (player == affected_card_controller))
        {
          multi_x_spell(player, card, EVENT_CAST_SPELL, 2);
          result =
              generic_spell(player, card, EVENT_CAST_SPELL, 5, &td, "TARGET_CREATURE", 1, NULL);
          return result;
        }
      }
    }
    else if ((event == EVENT_RESOLVE_SPELL || event == EVENT_CAN_CHANGE_TARGET) ||
             (event == EVENT_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = valid_target(&td);
        if (result != 0)
        {

          for (p_Var1 = instance->info_slot_fn; p_Var1 != (CardFunction)0x0; p_Var1 = p_Var1 + -1)
          {

            result = get_power(instance->targets[0].player, instance->targets[0].card);
            pump_until_eot_merge_previous_2022(player, card, instance->targets[0].player,
                                          instance->targets[0].card, result, 0);
          }
        }
        kill_card(player, card, KILL_DESTROY);
      }
    }
    result = 0;
  }
  return result;
}

/* ManalinkEh 2022: 0x23d5410; original cards/strixhaven.c:4590. */
int card_necrotic_fumes(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int result;
  target_definition_t td;
  target_definition_t td2;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x102);
    default_target_definition(player, card, &td2, 2);
    td2.allowed_controller = player;
    td2.illegal_abilities = 0;
    get_card_instance(player, card);
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    if (result != 0)
    {
      result = can_target(&td2);
      return result;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x102);
      default_target_definition(player, card, &td2, 2);
      td2.allowed_controller = player;
      td2.illegal_abilities = 0;
      instance = get_card_instance(player, card);

      if ((card == affected_card) &&
          ((player == affected_card_controller &&
            (new_pick_target(&td2, "Select a creature you control", 0, 0x81), spell_fizzled != 1))))
      {
        result = instance->targets[0].player;
        card_00 = instance->targets[0].card;
        instance->number_of_targets = '\0';
        generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE_OR_PLANESWALKER", 1,
                      NULL);
        if (spell_fizzled != 1)
        {
          kill_card(result, card_00, KILL_EXILE);
        }
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x102);
    default_target_definition(player, card, &td2, 2);
    td2.allowed_controller = player;
    td2.illegal_abilities = 0;
    get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        kill_target0(player, card, KILL_EXILE);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23ddc60; original cards/strixhaven.c:7501. */
int card_tend_the_pests(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_023ddca0;
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      instance = get_card_instance(player, card);
      make_pest(player, card, instance->info_slot);
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
    return 0;
  }
LAB_023ddca0:
  result = card_carrion(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int ris_legacy(int player, int card, event_t event);
static int card_exiled_with_study_counter(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23c8bb0; original cards/strixhaven.c:128. */
int card_academic_probation(int player, int card, event_t event)
{
  unsigned char bVar1;
  card_instance_t *pcVar2;
  int result;
  card_instance_t *pcVar4;
  target_definition_t td;
  test_definition_t this_test;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 1;
    get_card_instance(player, card);
    result = basic_spell(player, card, EVENT_CAN_CAST);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x47);
      td.illegal_type = 1;
      pcVar2 = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        pcVar2->number_of_targets = '\0';
        result = check_special_flags2_2022(player, card, -0x80000000);
        if (result == 0)
        {

          result = can_target(&td);
          result =
              dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033", "Name a card", 1,
                          10, "Target a permanent", result, 0x14, 0, 0, 0, 0);
          if (result == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
        }
        if ((pcVar2->mode & 2) != 0)
        {
          new_pick_target(&td, "Select target nonland permanent.", 0, 0x81);
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 1;
    pcVar2 = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      bVar1 = pcVar2->mode;
      if ((bVar1 & 1) != 0)
      {

        new_default_test_definition(&this_test, 1, "Select nonland card name.");
        this_test.type_flag = 1;
        do
        {
          result = name_a_card(player, 1 - player, &this_test, 3);
        } while (result == -1);
        result = target_player_cant_cast_csvid(player, card, 1 - player, result);
        if (-1 < result)
        {

          pcVar4 = get_card_instance(player, result);
          add_flag_to_special_effect(player, result, 0x20);
          pcVar4->targets[0].card = pcVar4->targets[0].card & 0xfffffffe;
        }
        bVar1 = pcVar2->mode;
      }
      if (((bVar1 & 2) != 0) && (result = valid_target(&td), result != 0))
      {
        disable_all_activated_abilities(pcVar2->targets[0].player, pcVar2->targets[0].card, 1);
        create_targetted_legacy_effect(player, card, academic_probation_legacy,
                                       pcVar2->targets[0].player, pcVar2->targets[0].card);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 1;
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23c8f80; original cards/strixhaven.c:192. */
int card_access_tunnel(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  bool bVar3;
  target_definition_t td;

  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      td.power_requirement = 0x2003;
      instance = get_card_instance(player, card);

      result = paying_mana();
      if ((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5,
                                                               4, 0, 0, 0, 0, 0, 0, &td, NULL),
                            result != 0))
      {
        bVar3 = ai_is_speculating != 1;
        instance->mode = '\x02';
        if ((bVar3) && ((player != 1 || (((unsigned char)trace_mode & 2) != 0))))
        {
          result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                               "Give 'Unblockable' to a creature with power 3 or less", 1, 10, 0, 0,
                               0, 0);
          if (result == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if (result == 1)
            goto LAB_023c902a;
          if (result != 2)
          {
            return 0;
          }
        }
        if (current_turn == player)
        {
          result = ((current_phase < 0x17) - 1 & 0xffffffa0) + 0x30;
        }
        else
        {
          result = -0x30;
        }
        ai_modifier = ai_modifier + result;
        add_state(player, card, 0x10);
        generic_activated_ability(player, card, EVENT_ACTIVATE, 7, 3, 0, 0, 0, 0, 0, 0, &td,
                                  "Select target creature with power 3 or less.");
        if (spell_fizzled == 1)
        {
          remove_state(player, card, 0x10);
        }
        return 0;
      }
      instance->mode = '\x01';
    LAB_023c902a:
      result = mana_producer(player, card, EVENT_ACTIVATE);
      return result;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      td.power_requirement = 0x2003;
      instance = get_card_instance(player, card);
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }
      if (instance->mode != '\x02')
      {
        return 0;
      }
      result = valid_target(&td);
      if (result == 0)
      {
        return 0;
      }
      pump_ability_until_eot_2022(instance->parent_controller, instance->parent_card,
                             instance->targets[0].player, instance->targets[0].card, 0, 0, 0, 0x10);
      return 0;
    }
  }
  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23c9310; original cards/strixhaven.c:255. */
int card_accomplished_alchemist(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    if (event == EVENT_COUNT_MANA)
    {
    LAB_023c93e5:
      if (card == affected_card)
        goto LAB_023c9460;
      goto LAB_023c93ed;
    }
    iVar1 = get_trap_condition(player, 1);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar1 = mana_producing_creature_all_one_color(player, card, EVENT_CAN_ACTIVATE, 0xc,
                                                    COLOR_TEST_ANY_COLORED, iVar1);
      return (unsigned int)(iVar1 != 0);
    }
  }
  else
  {
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      if (event == EVENT_COUNT_MANA)
        goto LAB_023c93e5;
    }
    else
    {
      if (((event != EVENT_COUNT_MANA) || (card != affected_card)) ||
          (player != affected_card_controller))
      {
        iVar1 = mana_producing_creature_all_one_color(player, card, event, 0xc,
                                                      COLOR_TEST_ANY_COLORED, 1);
        return iVar1;
      }
    LAB_023c9460:
      if (player == affected_card_controller)
      {

        iVar1 = get_trap_condition(player, 1);
        iVar2 = 1;
        if (iVar1 != 0)
        {
          iVar2 = get_trap_condition(player, 1);
        }
        iVar1 = mana_producing_creature_all_one_color(player, card, EVENT_COUNT_MANA, 0xc,
                                                      COLOR_TEST_ANY_COLORED, iVar2);
        return iVar1;
      }
    }
  LAB_023c93ed:
    iVar1 = get_trap_condition(player, 1);
  }
  if (event != EVENT_ACTIVATE)
  {
    return 0;
  }

  if (1 < iVar1)
  {
    if (player == 1)
      goto LAB_023c9422;
    iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "Get mana", 1, 1,
                        "Add X mana", 1, 1, 0, 0, 0, 0);
    if (iVar2 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    if (iVar2 != 1)
    {
      if (iVar2 != 2)
      {
        return 0;
      }
      goto LAB_023c9422;
    }
  }
  iVar1 = 1;
LAB_023c9422:
  iVar1 = mana_producing_creature_all_one_color(player, card, EVENT_ACTIVATE, 0xc,
                                                COLOR_TEST_ANY_COLORED, iVar1);
  return iVar1;
}

/* ManalinkEh 2022: 0x23c9760; original cards/strixhaven.c:375. */
int card_augmenter_pugilist(int player, int card, event_t event)
{
  int result;

  mdfc_front(player, card, event);
  if (((event - EVENT_POWER < 2) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      result = count_subtype(player, 1, -1);
      if (7 < result)
      {
        event_result = event_result + 5;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23c9ed0; original cards/strixhaven.c:513. */
int card_bayou_groff(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  unsigned int player_00;
  test_definition_t this_test;

  if (event == EVENT_MODIFY_COST)
  {

    result = get_updated_casting_cost(player, card, -1, EVENT_MODIFY_COST, 1);
    result = has_mana_multi(player, 1, 0, 0, result, 0, 0);
    if ((result != 0) ||
        (result = can_sacrifice_type_as_cost(player, 1, TYPE_CREATURE), result != 0))
    {
      null_casting_cost(player, card);
      instance = get_card_instance(player, card);
      instance->info_slot = 1;
      return 0;
    }
    infinite_casting_cost();
    instance = get_card_instance(player, card);
    instance->info_slot = 0;
  }
  else if ((((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
            (player == affected_card_controller)) &&
           ((instance = get_card_instance(player, card),
             instance->info_slot == 1 && (result = is_token(player, card), result == 0))))
  {

    result = can_sacrifice_type_as_cost(player, 1, TYPE_CREATURE);
    if (result != 0)
    {
      result = count_subtype(player, 2, -1);
      result = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014",
                           "Sacrifice a creature", 1, 0x14, "Pay 3", 1, result * 10, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 1)
      {

        new_default_test_definition(&this_test, 2, "Select a creature to sacrifice.");
        uVar3 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                              &this_test);
        if (uVar3 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        charge_mana_from_id(player, -1, EVENT_CAST_SPELL, 0x583d);
        player_00 = (int)uVar3 >> 0x10 & 0xff;
        if (spell_fizzled != 1)
        {
          kill_card(player_00, uVar3 >> 0x18, KILL_SACRIFICE);
          return 0;
        }
        state_untargettable(player_00, uVar3 >> 0x18, 0);
        return 0;
      }
      if (result != 2)
      {
        return 0;
      }
    }

    result = get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, 3);
    charge_mana_multi(player, 1, 0, 0, result, 0, 0);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23ca200; original cards/strixhaven.c:573. */
int card_beledros_witherbloom(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  upkeep_trigger_ability(player, card, event, 2);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    make_pest(player, card, 1);
  }
  else
  {

    if ((event == EVENT_RESOLVE_ACTIVATION) && (0 < active_cards_count[player]))
    {
      iVar1 = 0;
      do
      {
        iVar2 = is_what(player, iVar1, 1);
        if (iVar2 != 0)
        {
          instance = in_play(player, iVar1);
          if (instance != NULL)
          {
            action_on_card_2022(player, card, player, iVar1, ACT_UNTAP);
          }
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player]);
    }
  }
  iVar1 = generic_activated_ability(player, card, event, 0x2000, 0, 0, 0, 0, 0, 0, 10, NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x23ca3e0; original cards/strixhaven.c:629. */
int card_blade_historian(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if ((event == EVENT_ABILITIES) && (affected_card_controller == player))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        result = is_what(affected_card_controller, affected_card, 2);
        if (result != 0)
        {
          result = is_attacking(affected_card_controller, affected_card);
          if (result != 0)
          {
            event_result = event_result | 0x40000000;
          }
        }
      }
    }
  }
  result = hybrid(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23ca4a0; original cards/strixhaven.c:642. */
int card_blex_vexing_pest(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  mdfc_front(player, card, event);
  if (((((event - EVENT_POWER < 2) && (affected_card_controller == player)) &&
        (instance = in_play(player, card), instance != NULL)) &&
       (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
      (((result = has_subtype(affected_card_controller, affected_card, SUBTYPE_PEST),
         result != 0 || (result = has_subtype(affected_card_controller, affected_card, SUBTYPE_BAT),
                         result != 0)) ||
        ((result = has_subtype(affected_card_controller, affected_card, SUBTYPE_INSECT),
          result != 0 ||
              ((result = has_subtype(affected_card_controller, affected_card, SUBTYPE_SNAKE),
                result != 0 ||
                    (result = has_subtype(affected_card_controller, affected_card, SUBTYPE_SPIDER),
                     result != 0))))))))
  {
    event_result = event_result + 1;
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    gain_life_2022(player, 4);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x23ca620; original cards/strixhaven.c:666. */
int card_search_for_blex(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  test_definition_t test3;

  mdfc_back(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {

    iVar2 = count_deck(player);
    iVar1 = iVar2;
    if (5 < iVar2)
    {
      iVar1 = 5;
    }
    new_default_test_definition(&test3, 0xff, "Select a card to add to your hand.");
    test3.create_minideck = (unsigned short)iVar1;
    test3.no_shuffle = 1;
    if (iVar2 == 0)
    {
      iVar1 = 0;
      iVar2 = 0;
    }
    else
    {
      iVar2 = 0;
      do
      {

        if ((((ai_is_speculating == 1) ||
              ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))) &&
             (life[player] - (iVar2 * 3 + 3) < 6)) ||
            (iVar3 = new_global_tutor(player, player, 1, 1, 0, 1, &test3), iVar3 < 0))
          break;
        iVar2 = iVar2 + 1;
        iVar1 = iVar1 + -1;
      } while (iVar1 != 0);
      iVar2 = iVar2 * 3;
    }
    mill(player, iVar1);
    lose_life_2022(player, iVar2);
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x23caac0; original cards/strixhaven.c:800. */
int card_callous_bloodmage(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int what_player;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  target_definition_t td1;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }

  default_target_definition(player, card, &td1, 0);
  td1.zone = 0x1000;
  td1.allow_cancel = 0;
  instance = get_card_instance(player, card);
  iVar2 = can_target(&td1);
  what_player = 1 - player;
  iVar3 = count_subtype(what_player, 2, -1);
  iVar4 = count_subtype(player, 2, -1);
  iVar7 = 0;
  if (6 < life[player])
  {
    iVar7 = hand_count[player] * -10 + 0x46;
  }
  iVar5 = would_validate_arbitrary_target(&td1, what_player, -1);
  iVar6 = 0;
  if (iVar5 != 0)
  {
    iVar6 = count_graveyard(what_player);
  }
  iVar2 =
      dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\003",
                  "Create a Pest token", 1, (iVar3 - iVar4) * 10, "Draw a card and you lose 1 life",
                  1, iVar7, "Exile target player's graveyard", iVar2, iVar6, 0, 0, 0, 0);
  if (iVar2 == 1)
  {
    make_pest(player, card, 1);
  }
  else if (iVar2 == 2)
  {
    draw_cards(player, 1);
    lose_life(player, 1);
  }
  else if (iVar2 == 3)
  {
    iVar2 = can_target(&td1);
    if (iVar2 != 0)
    {
      iVar2 = new_pick_target(&td1, "TARGET_PLAYER", 0, 0);
      if (iVar2 != 0)
      {
        rfg_whole_graveyard(instance->targets[0].player);
      }
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x23cbeb0; original cards/strixhaven.c:1261. */
int card_daemogoth_titan(int player, int card, event_t event)
{
  int result;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    result = blocking(player, card, event);
    if (result == 0)
      goto LAB_023cbefe;
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result != 0)
      goto LAB_023cbefe;
  }
  player_sacrifices_a_permanent(player, card, player, 2, SAC_CAUSED | SAC_NO_CANCEL);
LAB_023cbefe:
  result = hybrid(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23cc160; original cards/strixhaven.c:1318. */
int card_decisive_denial(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  target_definition_t td;
  target_definition_t td1;
  target_definition_t td2;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = 1 - player;
    td1.preferred_controller = td1.allowed_controller;
    counterspell_target_definition(player, card, &td2, 0xff);
    td2.illegal_type = 2;
    get_card_instance(player, card);
    result = counterspell(player, card, EVENT_CAN_CAST, &td2, 0);
    if (result == 0)
    {
      result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
      if (result == 0)
      {
        return 0;
      }
      result = can_target(&td1);
    }
    else
    {
      result = 99;
    }
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      td.preferred_controller = player;
      default_target_definition(player, card, &td1, 2);
      td1.allowed_controller = 1 - player;
      td1.preferred_controller = td1.allowed_controller;
      counterspell_target_definition(player, card, &td2, 0xff);
      td2.illegal_type = 2;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        result = check_special_flags2_2022(player, card, -0x80000000);
        if (result == 0)
        {

          result = generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, NULL, 1, NULL);
          uVar3 = 0;
          if (result != 0)
          {
            result = can_target(&td1);
            uVar3 = (unsigned int)(result != 0);
          }
          result = counterspell(player, card, EVENT_CAN_CAST, &td2, 0);
          result = dialog_impl(
              player, card, EVENT_CAST_SPELL, "\001\002", "\001\016", "\001\033",
              "Target creature you control fights target creature you don't control", uVar3, 5,
              "Counter target noncreature spell unless its controller pays |3", result, 10, 0, 0, 0,
              0);
          if (result == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
        }
        if (instance->mode == '\x01')
        {
          generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CRETURE_YOU_CONTROL", 1,
                        NULL);
          if (spell_fizzled != 1)
          {
            new_pick_target(&td1, "Select target creature opponent controls.", 1, 0x81);
          }
        }
        else if (instance->mode == '\x02')
        {
          result = counterspell(player, card, EVENT_CAST_SPELL, &td2, 0);
          return result;
        }
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = 1 - player;
    td1.preferred_controller = td1.allowed_controller;
    counterspell_target_definition(player, card, &td2, 0xff);
    td2.illegal_type = 2;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      if (instance->mode == '\x01')
      {
        result = validate_target(player, card, &td, 0);
        if ((result != 0) && (result = validate_target(player, card, &td1, 1), result != 0))
        {
          fight(instance->targets[0].player, instance->targets[0].card, instance->targets[1].player,
                instance->targets[1].card);
        }
      }
      else if (instance->mode == '\x02')
      {
        counterspell_resolve_unless_pay_x(player, card, &td2, 0, 3);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23cf000; original cards/strixhaven.c:2311. */
int card_awaken_the_blood_avatar(int player, int card, event_t event)
{
  int iVar1;
  unsigned int uVar2;
  unsigned int uVar3;
  int iVar4;
  unsigned int uVar5;
  card_instance_t *instance;
  char sac[151];

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_test;

  if (event == EVENT_CAN_CAST)
  {

    default_test_definition(&recovered_stack_test.test, 2);
    iVar1 = max_can_sacrifice_as_cost(player, card, &recovered_stack_test.test);

    iVar4 = get_updated_casting_cost(player, card, -1, EVENT_CAN_CAST, -1);
    uVar2 = iVar4 + iVar1 * -2;
    iVar1 = has_mana_multi(player, uVar2 & (int)(uVar2 ^ 0xffffffff) >> 0x1f, 1, 0, 0, 1, 0);
    return (unsigned int)(iVar1 != 0);
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      new_default_test_definition(&recovered_stack_test.test, 2, "Select a creature to sacrifice");
      uVar2 = max_can_sacrifice_as_cost(player, card, &recovered_stack_test.test);
      uVar3 = get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, -1);
      iVar1 = has_mana_multi(player, uVar3, 1, 0, 0, 1, 0);
      iVar1 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\016", "\001\014",
                          "Sacrifice creatures", uVar2, 5, "Cast Normally", iVar1, 10, 0, 0, 0, 0);
      if (iVar1 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar1 == 1)
      {

        uVar5 = round_up_value(uVar3);

        if ((int)uVar2 < (int)uVar5)
        {
          uVar5 = uVar2;
        }
        recovered_stack_test.test.qty = (unsigned short)uVar5;
        iVar1 = mark_sacrifice(player, card, player, SAC_AS_COST | SAC_JUST_MARK | SAC_DONE,
                               &recovered_stack_test.test, sac);
        if (spell_fizzled != 1)
        {

          uVar3 = uVar3 + iVar1 * -2;
          charge_mana_multi(player, uVar3 & (int)(uVar3 ^ 0xffffffff) >> 0x1f, 1, 0, 0, 1, 0);
        }
        if (spell_fizzled != 1)
        {

          if (0 < active_cards_count[player])
          {
            iVar1 = 0;
            do
            {
              if (sac[iVar1] != '\0')
              {
                instance = get_card_instance(player, iVar1);
                instance->state = instance->state & 0xffefffff;
                kill_card(player, iVar1, KILL_SACRIFICE);
              }
              iVar1 = iVar1 + 1;
            } while (iVar1 < active_cards_count[player]);
          }
          iVar1 = get_id(player, card);
          return iVar1;
        }
        ai_modifier = ai_modifier + -99;
        return 0;
      }
      if (iVar1 == 2)
      {
        charge_mana_multi(player, uVar3, 1, 0, 0, 1, 0);
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    player_sacrifices_a_permanent(player, card, 1 - player, 2, SAC_CAUSED);
    default_token_definition(player, card, 0x4904, &recovered_stack_test.token);
    recovered_stack_test.test.message[0x18] = '\x03';
    recovered_stack_test.test.message[0x19] = '\0';
    recovered_stack_test.test.message[0x1a] = '\0';
    recovered_stack_test.test.message[0x1b] = '\0';
    recovered_stack_test.test.message[0x1c] = '\x06';
    recovered_stack_test.test.message[0x1d] = '\0';
    recovered_stack_test.test.message[0x1e] = '\0';
    recovered_stack_test.test.message[0x1f] = '\0';
    recovered_stack_test.test.message[0x28] = '\x12';
    recovered_stack_test.test.message[0x2c] = 'C';
    recovered_stack_test.test.message[0x2d] = '\0';
    recovered_stack_test.test.message[0x2e] = '\0';
    recovered_stack_test.test.message[0x2f] = '\0';
    generate_token(&recovered_stack_test.token);
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x23cf490; original cards/strixhaven.c:2410. */
int card_fervent_mastery(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int card_00;
  int iVar4;
  unsigned int uStack_ec;
  test_definition_t this_test;

  if (event == EVENT_MODIFY_COST)
  {
    iVar2 = has_mana_multi(player, 2, 0, 0, 0, 2, 0);
    if (iVar2 != 0)
    {
      null_casting_cost(player, card);
      pcVar1 = get_card_instance(player, card);
      pcVar1->info_slot = 1;
      return 0;
    }
    pcVar1 = get_card_instance(player, card);
    pcVar1->info_slot = 0;
    return 0;
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
      pcVar1 = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        pcVar1->targets[1].player = 0;
        pcVar3 = get_card_instance(player, card);
        if ((pcVar3->info_slot == 1) && (iVar2 = played_for_free(player, card), iVar2 == 0))
        {
          iVar2 = is_token(player, card);
          uStack_ec = (unsigned int)(iVar2 == 0);
        }
        else
        {
          uStack_ec = 0;
        }
        iVar2 = has_mana_to_cast_id(player, EVENT_CAST_SPELL, 0x5887);
        iVar2 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014", "\001\016",
                            "Pay alternate mana cost", uStack_ec, 10, "Pay full mana cost", iVar2,
                            0xf, 0, 0, 0, 0);
        if (iVar2 == 0)
        {
          spell_fizzled = 1;
          iVar4 = spell_fizzled;
        }
        else if (iVar2 == 1)
        {
          charge_mana_multi(player, 2, 0, 0, 0, 2, 0);
          iVar4 = spell_fizzled;
          if (spell_fizzled == 1)
          {
            return 0;
          }
          pcVar1->targets[1].player = 0x42;
        }
        else
        {
          iVar4 = spell_fizzled;
          if (iVar2 == 2)
          {
            charge_mana_from_id(player, card, EVENT_CAST_SPELL, 0x5887);
            iVar4 = spell_fizzled;
          }
        }
        if (iVar4 != 0)
        {
          return 0;
        }
      }
      goto LAB_023cf585;
    }
    if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
        (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  pcVar1 = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (pcVar1->targets[1].player == 0x42)
    {

      iVar4 = 1 - player;
      new_default_test_definition(&this_test, 0xff, "Select a card to discard.");
      iVar2 = 0;
      if (hand_count[iVar4] != 0)
      {
        iVar2 = 0;

        while (card_00 = new_select_a_card(iVar4, iVar4, 4, 0, 3, -1, &this_test), -1 < card_00)
        {
          iVar2 = iVar2 + 1;
          discard_card(iVar4, card_00);
          if (((iVar2 == 3) && ((ai_is_speculating == 1 ||
                                 ((iVar4 == 1 && (((unsigned char)trace_mode & 2) == 0)))))) ||
              (hand_count[iVar4] == 0))
            break;
        }
      }
      draw_cards(iVar4, iVar2);
    }
    new_default_test_definition(&this_test, 0xff, "Select a card.");
    this_test.qty = 3;
    new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
    new_multidiscard(player, 3, 1, player);
    kill_card(player, card, KILL_DESTROY);
  }
LAB_023cf585:
  iVar2 = basic_spell(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x23c7630; original cards/strixhaven.c:2571. */
int ris_legacy(int player, int card, event_t event)
{
  test_definition_2022_t test;

  if ((event == EVENT_ABILITIES) || (event == EVENT_CAST_SPELL))
  {
    default_test_definition_2022(&test, 0x100);
    remove_abilities_by_test_2022(1 - player, event, &test, AL_LOYALTY);
  }
  if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x23cfbe0; original cards/strixhaven.c:2584. */
int card_revel_in_silence(int player, int card, event_t event)
{
  int result;

  mdfc_back(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {

    target_player_cant_cast_type(player, card, 1 - player, 0x1fe);
    result = create_legacy_effect(player, card, ris_legacy);
    add_status(player, result, 0x10000);
    kill_card(player, card, KILL_EXILE);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23cff20; original cards/strixhaven.c:2672. */
int card_frostboil_snarl(int player, int card, event_t event)
{
  int result;

  soi_need_subtype_land(player, card, event, SUBTYPE_ISLAND, SUBTYPE_MOUNTAIN);
  result = plain_mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23cff70; original cards/strixhaven.c:2687. */
int card_furycalm_snarl(int player, int card, event_t event)
{
  int result;

  soi_need_subtype_land(player, card, event, SUBTYPE_PLAINS, SUBTYPE_MOUNTAIN);
  result = plain_mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23d0010; original cards/strixhaven.c:2723. */
int card_golden_ratio(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  unsigned int uVar3;
  int iVar4;
  int8_t *piVar5;
  bool bVar6;
  int iStack_118;
  int8_t pws[255];

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      piVar5 = pws + 5;
      pws[0] = '\0';
      pws[1] = '\0';
      pws[2] = '\0';
      pws[3] = '\0';
      pws[4] = '\0';
      uVar3 = 0xfa;
      bVar6 = ((unsigned int)piVar5 & 2) != 0;
      if (bVar6)
      {
        pws[5] = '\0';
        pws[6] = '\0';
        uVar3 = 0xf8;
        piVar5 = pws + 7;
      }
      for (uVar3 = uVar3 >> 2; uVar3 != 0; uVar3 = uVar3 - 1)
      {
        piVar5[0] = '\0';
        piVar5[1] = '\0';
        piVar5[2] = '\0';
        piVar5[3] = '\0';
        piVar5 = piVar5 + 4;
      }
      if (!bVar6)
      {
        piVar5[0] = '\0';
        piVar5[1] = '\0';
      }
      if (active_cards_count[player] < 1)
      {
        iStack_118 = 0;
      }
      else
      {
        iVar4 = 0;
        iStack_118 = 0;
        do
        {
          instance = in_play(player, iVar4);

          if (((instance != NULL) && (iVar2 = is_what(player, iVar4, 2), iVar2 != 0)) &&
              (iVar2 = get_power(player, iVar4), pws[iVar2] == '\0'))
          {
            pws[iVar2] = '\x01';
            iStack_118 = iStack_118 + 1;
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[player]);
        iStack_118 = iStack_118 * 10;
      }
      ai_modifier = ai_modifier + iStack_118;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {

    piVar5 = pws + 5;
    pws[0] = '\0';
    pws[1] = '\0';
    pws[2] = '\0';
    pws[3] = '\0';
    pws[4] = '\0';
    uVar3 = 0xfa;
    bVar6 = ((unsigned int)piVar5 & 2) != 0;
    if (bVar6)
    {
      pws[5] = '\0';
      pws[6] = '\0';
      uVar3 = 0xf8;
      piVar5 = pws + 7;
    }
    for (uVar3 = uVar3 >> 2; uVar3 != 0; uVar3 = uVar3 - 1)
    {
      piVar5[0] = '\0';
      piVar5[1] = '\0';
      piVar5[2] = '\0';
      piVar5[3] = '\0';
      piVar5 = piVar5 + 4;
    }
    if (!bVar6)
    {
      piVar5[0] = '\0';
      piVar5[1] = '\0';
    }
    if (active_cards_count[player] < 1)
    {
      iStack_118 = 0;
    }
    else
    {
      iVar4 = 0;
      iStack_118 = 0;
      do
      {

        while (((instance = in_play(player, iVar4),
                 instance != NULL && (iVar2 = is_what(player, iVar4, 2), iVar2 != 0)) &&
                (iVar2 = get_power(player, iVar4), pws[iVar2] == '\0')))
        {
          iVar4 = iVar4 + 1;
          iStack_118 = iStack_118 + 1;
          pws[iVar2] = '\x01';
          if (active_cards_count[player] <= iVar4)
            goto LAB_023d0108;
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 < active_cards_count[player]);
    }
  LAB_023d0108:
    draw_cards(player, iStack_118);
    kill_card(player, card, KILL_DESTROY);
  }
  iVar4 = basic_spell(player, card, event);
  return iVar4;
}

/* ManalinkEh 2022: 0x23d0620; original cards/strixhaven.c:2828. */
int card_harness_infinity(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int *piVar5;
  bool bVar6;
  int ids[151];

  if (event == EVENT_RESOLVE_SPELL)
  {

    iVar1 = active_cards_count[player] + -1;
    if (iVar1 < 0)
    {
      iVar4 = -1;
      iVar1 = count_graveyard(player);
      iVar1 = iVar1 + -1;
      if (-1 < iVar1)
        goto LAB_023d06c8;
    }
    else
    {
      iVar4 = 0;
      do
      {
        instance = in_hand(player, iVar1);
        if (instance != NULL)
        {
          iVar3 = get_original_internal_card_id(player, iVar1);
          ids[iVar4] = iVar3;
          obliterate_card(player, iVar1);
          iVar4 = iVar4 + 1;
        }
        iVar1 = iVar1 + -1;
      } while (iVar1 != -1);
      iVar4 = iVar4 + -1;
      iVar1 = count_graveyard(player);
      while (iVar1 = iVar1 + -1, -1 < iVar1)
      {
      LAB_023d06c8:

        add_card_to_hand(player, graveyard_ptr_mutable[player][iVar1]);
        remove_card_from_grave(player, iVar1);
      }
      if (-1 < iVar4)
      {
        piVar5 = ids + iVar4;
        do
        {
          raw_put_iid_on_top_of_graveyard(player, *piVar5);
          bVar6 = ids != piVar5;
          piVar5 = piVar5 + -1;
        } while (bVar6);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x23d0f90; original cards/strixhaven.c:3172. */
int card_ingenious_mastery(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  token_generation_t token;

  if (event == EVENT_MODIFY_COST)
  {
    iVar2 = has_mana_multi(player, 2, 0, 1, 0, 0, 0);
    if (iVar2 != 0)
    {
      null_casting_cost(player, card);
      pcVar1 = get_card_instance(player, card);
      pcVar1->info_slot = 1;
      return 0;
    }
    pcVar1 = get_card_instance(player, card);
    pcVar1->info_slot = 0;
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
      pcVar1 = get_card_instance(player, card);
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_023d1016;
      pcVar1->targets[1].player = 0;
      pcVar3 = get_card_instance(player, card);
      if ((pcVar3->info_slot != 1) || (iVar2 = is_token(player, card), iVar2 != 0))
        goto LAB_023d1016;

      iVar2 = has_mana_to_cast_id(player, EVENT_CAST_SPELL, 0x58a6);
      if (iVar2 == 0)
      {
      LAB_023d1280:
        charge_mana_multi(player, 2, 0, 1, 0, 0, 0);
        iVar4 = spell_fizzled;
        if (spell_fizzled == 1)
        {
          return 0;
        }
        pcVar1->targets[1].player = 0x42;
      }
      else
      {
        iVar2 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014",
                            "Pay alternate mana cost", 1, 0x14, "Pay full mana cost", 1, 10, 0, 0,
                            0, 0);
        if (iVar2 == 0)
        {
          spell_fizzled = 1;
          iVar4 = spell_fizzled;
        }
        else
        {
          if (iVar2 == 1)
            goto LAB_023d1280;
          iVar4 = spell_fizzled;
          if (iVar2 == 2)
          {
            charge_mana_from_id(player, -1, EVENT_CAST_SPELL, 0x58a6);
            if (spell_fizzled == 1)
            {
              return 0;
            }
            charge_mana(player, COLOR_COLORLESS, -1);
            iVar4 = spell_fizzled;
          }
        }
      }
      if (iVar4 != 0)
      {
        return 0;
      }
      goto LAB_023d1016;
    }
    if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
        (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  pcVar1 = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (pcVar1->targets[1].player == 0x42)
    {
      draw_cards(player, 3);
      default_token_definition(player, card, 0x4998, &token);
      token.qty = '\x02';
      token.t_player = 1 - player;
      generate_token(&token);
      scry(1 - player, 2);
    }
    else
    {
      draw_cards(player, pcVar1->info_slot);
    }
    kill_card(player, card, KILL_DESTROY);
  }
LAB_023d1016:
  iVar2 = generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x23d17f0; original cards/strixhaven.c:3339. */
int card_journey_to_the_oracle(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int zone_count;
  int iVar4;
  test_definition_t this_test;
  int lands[151];
  int iids[151];

  mdfc_back(player, card, event);
  if (event != EVENT_RESOLVE_SPELL)
    goto LAB_023d181b;

  new_default_test_definition(&this_test, 1, "Select a Land card to put into play.");
  this_test.id = 0x388;
  this_test.id_flag = 1;
  iVar1 = get_internal_card_id_from_csv_id(0x388);
  if (active_cards_count[player] < 1)
  {
    zone_count = 0;
  LAB_023d1983:
    do
    {
      if (iids[zone_count] == iVar1)
      {
        put_into_play(player, lands[zone_count]);
        zone_count = zone_count + -1;
        if (zone_count < 0)
          break;
        goto LAB_023d1983;
      }
      zone_count = zone_count + -1;
    } while (-1 < zone_count);
  }
  else
  {
    iVar4 = 0;
    zone_count = 0;
    do
    {
      while ((instance = in_hand(player, iVar4),
              instance != NULL && (iVar3 = is_what(player, iVar4, 1), iVar3 != 0)))
      {
        lands[zone_count] = iVar4;
        iVar3 = get_original_internal_card_id(player, iVar4);
        iVar4 = iVar4 + 1;
        iids[zone_count] = iVar3;
        zone_count = zone_count + 1;
        if (active_cards_count[player] <= iVar4)
          goto LAB_023d18f0;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < active_cards_count[player]);
  LAB_023d18f0:
    if (0 < zone_count)
    {
      iVar4 = 0;
      do
      {
        iVar3 = select_card_from_zone(player, player, iids, zone_count, 0, 1, -1, &this_test);
        if (iVar3 == -1)
          break;

        iVar4 = iVar4 + 1;
        iids[iVar3] = iVar1;
      } while (zone_count != iVar4);
      goto LAB_023d1983;
    }
    if (zone_count == 0)
      goto LAB_023d1983;
  }
  if ((hand_count[player] != 0) && (iVar1 = count_subtype(player, 1, -1), 7 < iVar1))
  {

    new_default_test_definition(&this_test, 0xff, "Select a card to discard.");
    iVar1 = new_select_a_card(player, player, 4, 0, 3, -1, &this_test);
    if (iVar1 != -1)
    {
      new_discard_card(player, iVar1, 2, player);
      bounce_permanent(player, card);
      return 0;
    }
  }
  kill_card(player, card, KILL_BURY);
LAB_023d181b:
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x23c7760; original cards/strixhaven.c:3395. */
static int kasmina_effect(int player, int card, event_t event)
{
  CardFunction address;
  card_instance_t *pcVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  card_instance_t *pcStack_6c;
  target_definition_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (iVar3 = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           iVar3 != 0x58ae))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if (pcVar1->damage_target_player < '\0')
    {
      return 0;
    }
    iVar3 = effect_follows_control_of_attachment(player, card, 0x4700);
    if (iVar3 != 0)
    {
      return 0;
    }
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (iVar3 = effect_follows_control_of_attachment(player, card, event), iVar3 != 0))
    {
      return 0;
    }
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      pcStack_6c = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {

        if (active_cards_count[player] < 1)
        {
          return 0;
        }
        iVar3 = 0;
        do
        {

          iVar2 = pcStack_6c->damage_target_card;
          pcVar1 = in_play(player, iVar3);
          if (((pcVar1 != NULL) && (iVar4 = is_planeswalker(player, iVar3), iVar2 != iVar3)) &&
              (iVar4 != 0))
          {

            iVar2 = get_internal_card_id_from_csv_id(0x58ae);
            iVar2 = call_card_fn(cards_data[iVar2].code_pointer, pcVar1, player, iVar3,
                                 EVENT_CAN_ACTIVATE);
            if (iVar2 != 0)
            {
              return 1;
            }
          }
          iVar3 = iVar3 + 1;
        } while (iVar3 < active_cards_count[player]);
        return 0;
      }
      goto LAB_023c77b3;
    }
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
  pcStack_6c = get_card_instance(player, card);
LAB_023c77b3:
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x100);
    td.illegal_abilities = 0;
    td.allowed_controller = player;
    iVar3 = can_target(&td);
    if ((iVar3 != 0) &&
        (iVar3 = new_pick_target(&td, "Select a planeswalker to activate.", 1, 0x81), iVar3 != 0))
    {

      iVar3 = pcStack_6c->targets[1].player;
      iVar2 = pcStack_6c->targets[1].card;
      if ((pcStack_6c->damage_target_player == iVar3) && (pcStack_6c->damage_target_card == iVar2))
      {
        spell_fizzled = 1;
        return 0;
      }
      pcVar1 = get_card_instance(iVar3, iVar2);
      iVar4 = get_internal_card_id_from_csv_id(0x58ae);
      call_card_fn(cards_data[iVar4].code_pointer, pcVar1, iVar3, iVar2, EVENT_ACTIVATE);
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar3 = get_internal_card_id_from_csv_id(0x58ae);
    address = cards_data[iVar3].code_pointer;
    iVar3 = pcStack_6c->targets[1].player;
    iVar2 = pcStack_6c->targets[1].card;
    pcVar1 = get_card_instance(iVar3, iVar2);
    call_card_fn(address, pcVar1, iVar3, iVar2, EVENT_RESOLVE_ACTIVATION);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d20e0; original cards/strixhaven.c:3564. */
int card_kelpie_guide(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  target_definition_t *ptStack_bc;
  target_definition_t td;
  target_definition_t td2;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x47);
    td.preferred_controller = player;
    td.special = 0x4000;
    default_target_definition(player, card, &td2, 0x47);
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                        &td, NULL);
      if (iVar2 == 0)
      {
        iVar2 = count_permanents_by_type(player, TYPE_LAND);
        if (iVar2 < 8)
        {
          return 0;
        }
        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                          &td2, NULL);
        if (iVar2 == 0)
        {
          return 0;
        }
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->info_slot = 0;
      instance->number_of_targets = '\0';
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                        &td, NULL);
      iVar3 = count_subtype(player, 1, -1);
      uVar4 = 0;
      if (7 < iVar3)
      {
        iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                          &td2, NULL);
        uVar4 = (unsigned int)(iVar3 != 0);
      }
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\033", "\001\016",
                          "Untap another target permanent", iVar2,
                          ((current_turn == player) - 1 & 0xffffffec) + 0x1e,
                          "Tap target permanent", uVar4, 0x14, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 1)
      {
        generic_activated_ability(player, card, EVENT_ACTIVATE, 7, 0, 0, 0, 0, 0, 0, 0, &td,
                                  "Select another target permanent you control.");
        if (spell_fizzled == 1)
        {
          return 0;
        }
        ai_mod_for_untap_effects(player, card, 0, 0);
        return 0;
      }
      if (iVar2 != 2)
      {
        return 0;
      }
      generic_activated_ability(player, card, EVENT_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0, &td2,
                                "TARGET_PERMANENT");
      if (spell_fizzled == 1)
      {
        return 0;
      }

      iVar3 = get_cmc(instance->targets[0].player, instance->targets[0].card);
      iVar2 = iVar3 * 5;
      if (instance->targets[0].player != 1 - player)
      {
        iVar2 = iVar3 * -5;
      }
      ai_modifier = ai_modifier + iVar2;
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x47);
    td.preferred_controller = player;
    td.special = 0x4000;
    default_target_definition(player, card, &td2, 0x47);
    instance = get_card_instance(player, card);
  }
  ptStack_bc = &td2;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (instance->mode == '\x01')
    {
      iVar2 = valid_target(&td);
      if (iVar2 != 0)
      {
        untap_card(instance->targets[0].player, instance->targets[0].card);
      }
    }
    else if ((instance->mode == '\x02') && (iVar2 = valid_target(ptStack_bc), iVar2 != 0))
    {
      tap_card(instance->targets[0].player, instance->targets[0].card);
      return 0;
    }
  }

  return 0;
}

/* ManalinkEh 2022: 0x23c86a0; original cards/strixhaven.c:3632. */
static int card_exiled_with_study_counter(int player, int card, event_t event)
{
  int player_00;
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int card_00;
  int iStack_14;

  if (event == EVENT_SET_LEGACY_EFFECT_NAME)
  {
    instance = get_card_instance(player, card);
    if (-1 < instance->targets[0].card)
    {

      instance = get_card_instance(player, card);
      scnprintf(set_legacy_effect_name_addr, 0x33, "%s",
                cards_ptr[cards_data[instance->targets[0].card].id]->name);
    }
  }
  else
  {
    if (event == EVENT_SET_LEGACY_EFFECT_TEXT)
    {
      scnprintf(set_legacy_effect_name_addr, 0x33, "Exiled with a study counter.");
      return 0;
    }
    if (event == EVENT_STATIC_EFFECTS)
    {

      instance = get_card_instance(player, card);
      iVar3 = instance->targets[0].card;
      instance = get_card_instance(player, card);
      iStack_14 = 0;
      player_00 = instance->targets[0].player;
      if (0 < active_cards_count[player])
      {
        card_00 = 0;
        do
        {
          instance = in_play(player, card_00);
          if (instance != NULL)
          {
            iVar2 = is_what(player, card_00, 0x80);
            if (iVar2 != 0)
            {

              instance = get_card_instance(player, card_00);
              if (instance->info_slot_fn == card_exiled_with_study_counter)
              {
                iVar2 = count_counters(player, card_00, COUNTER_STUDY);
                if ((iVar2 != 0) && (iVar3 == instance->targets[0].card))
                {
                  iStack_14 = iStack_14 + 1;
                }
              }
            }
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[player]);
      }
      iVar3 = count_iid_in_rfg(player_00, iVar3);
      if (iVar3 < iStack_14)
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23c6a40; original cards/strixhaven.c:3672. */
static int count_different_mana_values_among_card_exiled_with_study_counter(int player)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int card;
  int *piVar4;
  int manacosts[20];

  piVar4 = manacosts;

  for (iVar3 = 0x14; iVar3 != 0; iVar3 = iVar3 + -1)
  {
    *piVar4 = 0;
    piVar4 = piVar4 + 1;
  }
  if (active_cards_count[player] < 1)
  {
    iVar3 = 0;
  }
  else
  {
    card = 0;
    iVar3 = 0;
    do
    {
      instance = in_play(player, card);
      if (instance != NULL)
      {
        iVar2 = is_what(player, card, 0x80);
        if (iVar2 != 0)
        {

          instance = get_card_instance(player, card);
          if (instance->info_slot_fn == card_exiled_with_study_counter)
          {
            iVar2 = count_counters(player, card, COUNTER_STUDY);
            if (iVar2 != 0)
            {

              iVar2 = get_cmc_by_internal_id(instance->targets[0].card);
              if (manacosts[iVar2] == 0)
              {
                manacosts[iVar2] = 1;
                iVar3 = iVar3 + 1;
              }
            }
          }
        }
      }
      card = card + 1;
    } while (card < active_cards_count[player]);
  }
  return iVar3;
}

/* ManalinkEh 2022: 0x23d3d20; original cards/strixhaven.c:4118. */
int card_lorehold_excavation(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  token_generation_t token;
  test_definition_t test;

  result = eot_trigger(player, card, event);
  if (((result != 0) && (current_turn == player)) &&
      (result = is_humiliated_for_ability(player, card, AL_TRIG), result == 0))
  {
    special_mill(player, card, 0x58bb, player, 1);
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    new_default_test_definition(&test, 2, "Select a creature card to exile from your graveyard.");
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 5, 0, 0, 0, 0, 0, 0,
                                         NULL, NULL);
      if (result == 0)
      {
        return 0;
      }
      result = new_special_count_grave(player, &test);
      return (unsigned int)(result != 0);
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 5, 0, 0, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      result = new_global_tutor(player, player, 2, 6, 0, 3, &test);
      if (-1 < result)
      {
        return 0;
      }
      spell_fizzled = 1;
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = global_enchantment(player, card, event);
      return result;
    }
    new_default_test_definition(&test, 2, "Select a creature card to exile from your graveyard.");
  }
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  default_token_definition(instance->parent_controller, instance->parent_card, 0x498d, &token);
  token.pow = 3;
  token.tou = 2;
  token.color_forced = '0';
  token.action = TOKEN_ACTION_TAPPED;
  generate_token(&token);
  return 0;
}

/* ManalinkEh 2022: 0x23d4290; original cards/strixhaven.c:4290. */
int card_mercurial_transformation(int player, int card, event_t event)
{
  uint8_t uVar1;
  int iVar2;
  int iVar3;
  card_instance_t *instance;
  int32_t iVar5;
  target_definition_t td;
  test_definition_t test;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
      default_target_definition(player, card, &td, 0x47);
      td.illegal_type = 1;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_NONLAND_PERMANENT", 1, NULL);
        if (spell_fizzled == 1)
        {
          return 0;
        }
        dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033", "Frog", 1, 1, "Octopus",
                    1, 1, 0, 0, 0, 0);
        if (spell_fizzled == 1)
        {
          return 0;
        }
        if (ai_is_speculating != 1)
        {
          if (player != 1)
          {
            return 0;
          }
          if (((unsigned char)trace_mode & 2) != 0)
          {
            return 0;
          }
        }

        iVar2 = instance->targets[0].player;
        iVar3 = instance->targets[0].card;
        if (iVar2 == 1 - player)
        {
          iVar2 = get_power(iVar2, iVar3);
          iVar3 = is_what(instance->targets[0].player, instance->targets[0].card, 2);
          if (iVar3 == 0)
          {
            iVar2 = get_cmc(instance->targets[0].player, instance->targets[0].card);
          }
          ai_modifier = ai_modifier + iVar2 * 8;
          return 0;
        }
        iVar2 = get_power(iVar2, iVar3);
        iVar3 = is_what(instance->targets[0].player, instance->targets[0].card, 2);
        if (iVar3 != 0)
        {
          if (3 < iVar2)
          {
            ai_modifier = ai_modifier + iVar2 * -8;
            return 0;
          }
          ai_modifier = ai_modifier + (4 - iVar2) * 8;
          return 0;
        }
        iVar2 = get_cmc(instance->targets[0].player, instance->targets[0].card);
        ai_modifier = ai_modifier + (4 - iVar2) * 8;
        return 0;
      }
      goto LAB_023d430a;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0x47);
      td.illegal_type = 1;
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        iVar2 = valid_target(&td);
        if (iVar2 != 0)
        {

          uVar1 = instance->mode;
          iVar2 = is_what(instance->targets[0].player, instance->targets[0].card, 2);
          if (iVar2 == 0)
          {

            iVar2 = instance->targets[0].card;
            iVar3 = instance->targets[0].player;
            if (uVar1 == '\x01')
            {
              iVar2 = turn_into_creature(player, card, iVar3, iVar2, 1, 1, 1);
              instance = get_card_instance(player, iVar2);
              iVar2 = get_sleighted_color_test(player, card, 4);
              instance->targets[6].player = iVar2;
              instance->targets[9].player = 1;
              iVar5 = 0x44;
            }
            else
            {
              iVar2 = turn_into_creature(player, card, iVar3, iVar2, 1, 4, 4);
              instance = get_card_instance(player, iVar2);
              iVar2 = get_sleighted_color_test(player, card, 4);
              instance->targets[6].player = iVar2;
              instance->targets[9].player = 1;
              iVar5 = 0x86;
            }
            instance->targets[9].card = iVar5;
          }
          else
          {
            default_test_definition(&test, 0);
            if (uVar1 == '\x01')
            {
              test.toughness = 1;
              test.power = 1;
              test.subtype = 0x44;
            }
            else
            {
              test.toughness = 4;
              test.power = 4;
              test.subtype = 0x86;
            }
            test.color = get_sleighted_color_test(player, card, 4);
            humiliate_and_set_pt_abilities(player, card, instance->targets[0].player,
                                           instance->targets[0].card, 4, &test);
          }
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_023d430a;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.illegal_type = 1;
  get_card_instance(player, card);
LAB_023d430a:
  iVar2 = generic_spell(player, card, event, 1, &td, NULL, 1, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x23d53c0; original cards/strixhaven.c:4580. */
int card_necroblossom_snarl(int player, int card, event_t event)
{
  int result;

  soi_need_subtype_land(player, card, event, SUBTYPE_SWAMP, SUBTYPE_FOREST);
  result = plain_mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23d5d00; original cards/strixhaven.c:4736. */
int card_owlin_shieldmage(int player, int card, event_t event)
{
  ward_life(player, card, event, 3);
  return 0;
}

/* ManalinkEh 2022: 0x23d6550; original cards/strixhaven.c:4853. */
int card_restorative_burst(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  mdfc_back(player, card, event);
  if (event != EVENT_CAN_CAST)
  {
    if (event != EVENT_CAST_SPELL)
    {
      if ((event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET) &&
          (event != EVENT_CHANGE_TARGET))
      {
        return 0;
      }
      new_default_test_definition(&this_test, 0x103,
                                  "Select a creature, land, and/or planeswalker card.");
      if (event != EVENT_RESOLVE_SPELL)
        goto LAB_023d65bd;

      result = validate_target_from_grave(player, card, player, 0);
      if (result == -1)
      {
        result = validate_target_from_grave(player, card, player, 1);
        if (result != -1)
        {
        LAB_023d66f4:
          from_grave_to_hand(player, result, 1);
          goto LAB_023d6708;
        }
      }
      else
      {
        from_grave_to_hand(player, result, 1);
        result = validate_target_from_grave(player, card, player, 1);
        if (result != -1)
          goto LAB_023d66f4;
      LAB_023d6708:
        gain_life_2022(player, 4);
        gain_life_2022(1 - player, 4);
      }
      kill_card(player, card, KILL_EXILE);
      goto LAB_023d65bd;
    }
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
  }
  new_default_test_definition(&this_test, 0x103,
                              "Select a creature, land, and/or planeswalker card.");
LAB_023d65bd:
  result = generic_spell(player, card, event, 0x28, NULL, NULL, 2, &this_test);
  return result;
}

/* ManalinkEh 2022: 0x23d9770; original cards/strixhaven.c:5920. */
int card_rip_apart(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t *td;
  const char *prompt;
  target_definition_t td1;
  target_definition_t td2;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td1, 0x102);
    default_target_definition(player, card, &td2, 0x44);
    get_card_instance(player, card);
    iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td1, NULL, 1, NULL);
    if (iVar3 == 0)
    {
      iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL);
      return iVar3;
    }
    return 1;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      td = &td2;
      default_target_definition(player, card, &td1, 0x102);
      default_target_definition(player, card, td, 0x44);
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        iVar3 = check_special_flags2_2022(player, card, -0x80000000);
        if (iVar3 == 0)
        {

          iVar3 = can_target(td);
          iVar2 = can_target(&td1);
          iVar3 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\016", "\001\033",
                              "Damage target creature or planeswalker", iVar2, 5,
                              "Destroy target artifact or enchantment", iVar3, 5, 0, 0, 0, 0);
          if (iVar3 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if (iVar3 == 1)
          {
            td = &td1;
            prompt = "TARGET_CREATURE_OR_PLANESWALKER";
          }
          else
          {
            if (iVar3 != 2)
            {
              return 0;
            }
            prompt = "DISENCHANT";
          }
          iVar3 = generic_spell(player, card, EVENT_CAST_SPELL, 1, td, prompt, 1, NULL);
          return iVar3;
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td1, 0x102);
    default_target_definition(player, card, &td2, 0x44);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      if (instance->mode == '\x01')
      {
        iVar3 = valid_target(&td1);
        if (iVar3 != 0)
        {
          damage_target0(player, card, 3);
        }
      }
      else if ((instance->mode == '\x02') && (iVar3 = valid_target(&td2), iVar3 != 0))
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td1, 0x102);
    default_target_definition(player, card, &td2, 0x44);
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d9ee0; original cards/strixhaven.c:6059. */
int card_will_scholar_of_frost(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  int valid[5];
  token_generation_t token;
  target_definition_t td;
  target_definition_t td2;

  mdfc_back(player, card, event);
  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    instance = in_play(player, card);
    if ((((instance != NULL) &&
          (iVar4 = is_humiliated_for_ability(player, card, AL_STATIC), iVar4 == 0)) &&
         (affected_card_controller == player)) &&
        (iVar4 = is_what(player, affected_card, 0x38), iVar4 != 0))
    {
      COST_COLORLESS = COST_COLORLESS + -1;
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) ||
           (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE))
  {

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 3;
    default_target_definition(player, card, &td2, 0x47);
    iVar4 = can_target(&td);
    iVar4 = dialog_impl(player, card, event, "\001\002", "\001\017", "Target creature is 0/2",
                        iVar4, 10, 1, "Draw 2 cards", 1, 0x14, 0xfffffffd, "Exile 5 permanents", 1,
                        0x32, 0xfffffff9, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (iVar4 == 0)
      {
        return 0;
      }
    }
    else if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE)
    {
      instance->number_of_targets = '\0';
      if (iVar4 == 1)
      {
        new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      }
      else if (iVar4 == 3)
      {

        iVar4 = 0;
        do
        {
          iVar5 = new_pick_target(&td2, "TARGET_PERMANENT", iVar4, 0);
          if (iVar5 != 0)
          {
            state_untargettable(instance->targets[iVar4].player, instance->targets[iVar4].card, 1);
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 != 5);
        if (instance->number_of_targets != '\0')
        {
          iVar4 = 0;
          do
          {
            ptVar2 = instance->targets + iVar4;
            ptVar1 = instance->targets + iVar4;
            iVar4 = iVar4 + 1;
            state_untargettable(ptVar1->player, ptVar2->card, 0);
          } while (iVar4 < (int)(unsigned int)instance->number_of_targets);
        }
      }
    }
    else if (iVar4 == 1)
    {
      iVar4 = validate_target(player, card, &td, 0);
      if (iVar4 != 0)
      {
        set_pt_and_abilities_until_eot(player, card, instance->targets[0].player,
                                       instance->targets[0].card, 0, 2, 0, 0, 8);
      }
    }
    else if (iVar4 == 2)
    {
      draw_cards(player, 2);
    }
    else
    {

      valid[0] = 0;
      valid[1] = 0;
      valid[2] = 0;
      valid[3] = 0;
      valid[4] = 0;
      if (instance->number_of_targets != '\0')
      {
        iVar4 = 0;
        do
        {
          iVar5 = validate_target(player, card, &td2, iVar4);
          if (iVar5 != 0)
          {
            valid[iVar4] = 1;
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 < (int)(unsigned int)instance->number_of_targets);
        if (instance->number_of_targets != 0)
        {
          iVar4 = 0;
          do
          {
            while (true)
            {
              if (valid[iVar4] != 0)
              {
                default_token_definition(player, card, 0x4926, &token);
                token.tou = 4;
                token.pow = 4;
                token.color_forced = '\x14';
                token.t_player = instance->targets[iVar4].player;
                generate_token(&token);
              }
              iVar5 = validate_target(player, card, &td2, iVar4);
              if (iVar5 != 0)
                break;
              iVar4 = iVar4 + 1;
              if ((int)(unsigned int)instance->number_of_targets <= iVar4)
                goto LAB_023da1f8;
            }
            ptVar2 = instance->targets + iVar4;
            ptVar1 = instance->targets + iVar4;
            iVar4 = iVar4 + 1;
            kill_card(ptVar1->player, ptVar2->card, KILL_EXILE);
          } while (iVar4 < (int)(unsigned int)instance->number_of_targets);
        }
      }
    }
  }
LAB_023da1f8:
  iVar4 = planeswalker(player, card, event, 4);
  return iVar4;
}

/* ManalinkEh 2022: 0x23da7f0; original cards/strixhaven.c:6305. */
int card_selfless_glyphweaver(int player, int card, event_t event)
{
  int result;

  mdfc_front(player, card, event);
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    pump_creatures_until_eot_no_repeat(player, card, player, 0, 0, 0x800, NULL);
  }
  result = generic_activated_ability(player, card, event, 0x4000, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23da8c0; original cards/strixhaven.c:6317. */
int card_deadly_vanity(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int card_00;
  unsigned int player_00;
  unsigned int uStack_68;
  target_definition_t td;

  mdfc_back(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {

    default_target_definition(player, card, &td, 0x102);
    td.preferred_controller = player;
    td.illegal_abilities = 0;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    iVar1 = can_target(&td);
    if (iVar1 != 0)
    {
      new_pick_target(&td, "Select a creature or planeswalker.", 0, 0x80);
      instance->number_of_targets = '\0';
    }

    uStack_68 = 0;
    do
    {
      player_00 = uStack_68 ^ current_turn;

      if (0 < active_cards_count[player_00])
      {
        card_00 = 0;
        do
        {
          while (iVar3 = is_what(player_00, card_00, 0x102), iVar3 == 0)
          {
          LAB_023da9a4:
            card_00 = card_00 + 1;
            if (active_cards_count[player_00] <= card_00)
              goto LAB_023da9e8;
          }
          if (((iVar1 == 0) || (instance->targets[0].player != player_00)) ||
              (instance->targets[0].card != card_00))
          {
            kill_card(player_00, card_00, KILL_DESTROY);
            goto LAB_023da9a4;
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[player_00]);
      }
    LAB_023da9e8:
      uStack_68 = uStack_68 + 1;
    } while (uStack_68 != 2);
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x23c8280; original cards/strixhaven.c:6645. */
static int embrose_dean_of_shadow_effect(int player, int card, event_t event)
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
           result != 0x590f))
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
  LAB_023c82b5:
    uVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto LAB_023c82d7;
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
      goto LAB_023c82b5;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);

    if ((pcVar3 != NULL) && (pcVar3 = get_card_instance(affected_card_controller, affected_card),
                             pcVar3->kill_code != '\0'))
    {

      if ((-1 < pcVar1->damage_target_player) &&
          ((affected_card == pcVar1->damage_target_card &&
            (pcVar1->damage_target_player == affected_card_controller))))
      {
        pcVar1->damage_target_card = -1;
        pcVar1->damage_target_player = -1;
        remove_status(player, card, 0x10000);
      }
      result = count_counters(affected_card_controller, affected_card, COUNTER_P1_P1);
      if (result != 0)
      {
        count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
      }
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
LAB_023c82d7:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23db9f0; original cards/strixhaven.c:6771. */
int card_shineshadow_snarl(int player, int card, event_t event)
{
  int result;

  soi_need_subtype_land(player, card, event, SUBTYPE_PLAINS, SUBTYPE_SWAMP);
  result = plain_mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23dca70; original cards/strixhaven.c:7095. */
int card_spectacle_mage(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if ((result == 0) && (affected_card_controller == player))
      {
        result = is_what(player, affected_card, 0x38);
        if (result != 0)
        {
          result = get_cmc(affected_card_controller, affected_card);
          if (4 < result)
          {
            COST_COLORLESS = COST_COLORLESS + -1;
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23dcf60; original cards/strixhaven.c:7230. */
int card_stonebound_mentor(int player, int card, event_t event)
{
  card_instance_t *instance;

  if (event == EVENT_REMOVED_FROM_GRAVEYARD)
  {
    if (affected_card_controller == player)
    {
      instance = get_card_instance(player, card);
      if (instance->info_slot != 0x42)
      {
        scry(player, 1);
        instance = get_card_instance(player, card);
        instance->info_slot = 0x42;
      }
    }
  }
  else if (event == EVENT_STATIC_EFFECTS)
  {
    instance = get_card_instance(player, card);
    if (instance->info_slot == 0x42)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23de590; original cards/strixhaven.c:7711. */
int card_umbral_juke(int player, int card, event_t event)
{
  unsigned char bVar1;
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    get_card_instance(player, card);
    result = basic_spell(player, card, EVENT_CAN_CAST);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        result = check_special_flags2_2022(player, card, -0x80000000);
        if (result == 0)
        {

          instance->mode = '\0';
          result = can_target(&td);
          result = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\016", "\001\033",
                               "Target player sac a creature or a planeswalker", result, 10,
                               "Create a 2/1 Inkling", 1, 5, 0, 0, 0, 0);
          if (result == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
        }
        if ((instance->mode & 1) != 0)
        {
          new_pick_target(&td, "TARGET_PLAYER", 0, 1);
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      bVar1 = instance->mode;
      if ((bVar1 & 1) != 0)
      {
        result = validate_target(player, card, &td, 0);
        if (result != 0)
        {
          player_sacrifices_a_permanent(player, card, instance->targets[0].player, 0x102,
                                        SAC_CAUSED);
        }
        bVar1 = instance->mode;
      }
      if ((bVar1 & 2) != 0)
      {
        make_inkling(player, card, 1, 0);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23df1b0; original cards/strixhaven.c:8032. */
int card_velomachus_lorehold(int player, int card, event_t event)
{
  int result;
  int amount;
  test_definition_t test;

  check_legend_rule(player, card, event);
  special_abilities_2022(player, card, event, 0xc, player, card);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    result = get_power(player, card);

    amount = count_deck(player);
    if (amount != 0)
    {
      if (7 < amount)
      {
        amount = 7;
      }
      default_test_definition(&test, 0x38);
      test.cmc = result + 1;
      test.cmc_flag = 3;
      scnprintf(test.message, 100, "Choose an instant or sorcery card with MV %d or less", result);
      test.create_minideck = (unsigned short)amount;
      result = new_select_a_card(player, player, 1, 0, 1, -1, &test);
      if (-1 < result)
      {
        amount = amount + -1;
        play_card_in_deck_for_free(player, player, result);
      }
      put_top_x_on_bottom_in_random_order(player, amount);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23dfaf0; original cards/strixhaven.c:8208. */
int card_vineglimmer_snarl(int player, int card, event_t event)
{
  int result;

  soi_need_subtype_land(player, card, event, SUBTYPE_ISLAND, SUBTYPE_FOREST);
  result = plain_mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23e0930; original cards/strixhaven.c:8510. */
int card_zimone_quandrix_prodigy(int player, int card, event_t event)
{
  uint8_t uVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int iVar5;
  event_t event_00;
  int iStack_f0;
  int iStack_ec;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    instance = get_card_instance(player, card);
  LAB_023e097a:
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      uVar1 = instance->mode;
      if (uVar1 == '\x01')
      {
        new_default_test_definition(&this_test, 1, "Select a land card to put into play.");
        new_global_tutor(player, player, 4, 5, 0, 1, &this_test);
        uVar1 = instance->mode;
      }
      if (uVar1 == '\x02')
      {
        iVar3 = count_permanents_by_type(player, TYPE_LAND);
        draw_cards(player, (7 < iVar3) + 1);
      }
    }
    return 0;
  }
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar3 = 1;
    event_00 = EVENT_CAN_ACTIVATE;
    goto LAB_023e0c28;
  }
  if (event != EVENT_ACTIVATE)
    goto LAB_023e097a;

  instance->mode = '\0';
  iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 4, 0, 0, 0, 0, 0, 0, NULL,
                                    NULL);
  iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 1, 0, 0, 0, 0, 0, 0, NULL,
                                    NULL);
  iVar3 =
      dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\033",
                  "Put a land into play from hand", iVar4, 5, "Draw a card", iVar3, 10, 0, 0, 0, 0);
  if (iVar3 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (active_cards_count[player] < 1)
  {
    if (iVar3 == 2)
    {
    LAB_023e0dd0:
      ai_modifier = ai_modifier + 0x30;
      goto LAB_023e0d7d;
    }
  LAB_023e0d49:
    if (iVar3 != 1)
    {
      return 0;
    }
  LAB_023e0c79:
    iVar3 = 1;
  }
  else
  {
    iStack_ec = 0;
    iStack_f0 = 0;
    iVar4 = 0;
    do
    {
      iVar5 = is_what(player, iVar4, 1);
      if (iVar5 != 0)
      {
        instance = in_hand(player, iVar4);
        iStack_ec = (iStack_ec + 1) - (unsigned int)(instance == NULL);
        instance = in_play(player, iVar4);
        iStack_f0 = (iStack_f0 + 1) - (unsigned int)(instance == NULL);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < active_cards_count[player]);
    if (iVar3 != 2)
    {
      if ((iStack_ec == 0) || (iVar3 != 1))
        goto LAB_023e0d49;
      ai_modifier = ai_modifier + 0x18;
      goto LAB_023e0c79;
    }
    if (4 < (unsigned int)(iStack_f0 + -3 + iStack_ec))
      goto LAB_023e0dd0;
  LAB_023e0d7d:
    iVar3 = 4;
  }
  event_00 = EVENT_ACTIVATE;
LAB_023e0c28:
  iVar3 = generic_activated_ability(player, card, event_00, 1, iVar3, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static void learn(int player, int card);

/* ManalinkEh 2022: 0x23c6560; original cards/strixhaven.c:60. */
static void learn(int player, int card)
{
  int *piVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  test_definition_t test;

  if ((((unsigned int)player_bits[player] >> 24 & 0xff) & 2) == 0)
  {

    piVar1 = graveyard_ptr_mutable[player];
    iVar3 = *piVar1;
    if (iVar3 != -1)
    {
      bVar2 = false;
      iVar4 = 0;
      while (true)
      {

        if ((cards_data[iVar3].id == 0x58fc) &&
            (iVar3 = dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\014",
                                 "Retrieve Retriever Phoenix", 1, 0x14, "Learn as normal", 1, 5, 0,
                                 0, 0, 0),
             iVar3 == 1))
        {

          add_card_to_hand(player, graveyard_ptr_mutable[player][iVar4]);
          remove_card_from_grave(player, iVar4);
          bVar2 = true;
          iVar3 = piVar1[iVar4 + 1];
        }
        else
        {
          iVar3 = piVar1[iVar4 + 1];
        }
        if (iVar3 == -1)
          break;
        iVar3 = graveyard_ptr_mutable[player][iVar4 + 1];
        iVar4 = iVar4 + 1;
      }
      if (bVar2)
      {
        return;
      }
    }
  }
  iVar3 = dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\014", "Get a Lesson", 1,
                      10, "Discard & Draw", hand_count[player], 10, "Do nothing", 1, 1, 0, 0, 0, 0);
  if (iVar3 == 1)
  {

    new_default_test_definition(&test, 0xff, "Choose a Lesson card.");
    test.subtype = 0x6003;
    iVar3 = new_card_from_list(player, 5, &test);
    iVar4 = add_card_to_hand(player, iVar3);
    reveal_card(player, card, player, iVar4);
    iVar3 = check_card_for_rules_engine(iVar3);
    update_rules_engine(iVar3);
    return;
  }
  if (iVar3 != 2)
  {
    return;
  }
  discard(player, 0, player);
  draw_cards(player, 1);
  return;
}

/* ManalinkEh 2022: 0x23c9580; original cards/strixhaven.c:317. */
int card_arcane_subtraction(int player, int card, event_t event)
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
            pump_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card, -4,
                           0);
            learn(player, card);
          }
          kill_card(player, card, KILL_DESTROY);
          return 0;
        }
        goto LAB_023c95e4;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_023c95e4:
  result = vanilla_instant_pump_2022(player, card, event, 2, 1 - player, -4, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x23cba10; original cards/strixhaven.c:1139. */
int card_cram_session(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    gain_life_2022(player, 4);
    learn(player, card);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23c6b10; original cards/strixhaven.c:1752. */
static int dramatic_finale_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x586e))
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
  LAB_023c6b45:
    result = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (result == 0)
      goto LAB_023c6b65;
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
      goto LAB_023c6b45;

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
      count_for_gfp_ability_and_store_values(player, card, 0x77, player, 2, NULL, 0x40, 0);
    }
    result =
        resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (result == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {
    make_inkling(player, card, 1, 0);
    kill_card(player, card, KILL_EXILE);
  }
LAB_023c6b65:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23cdd10; original cards/strixhaven.c:1852. */
int card_dream_strix(int player, int card, event_t event)
{
  int result;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    learn(player, card);
  }
  result = card_skulking_ghost(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23ce6b0; original cards/strixhaven.c:2100. */
int card_enthusiastic_study(int player, int card, event_t event)
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
            pump_ability_until_eot_2022(player, card, instance->targets[0].player,
                                   instance->targets[0].card, 3, 1, 0x80, 0);
            learn(player, card);
          }
          kill_card(player, card, KILL_DESTROY);
          return 0;
        }
        goto LAB_023ce780;
      }
      if ((event != EVENT_CHANGE_TARGET) && (event != EVENT_CHECK_PUMP))
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_023ce780:
  result = vanilla_instant_pump_2022(player, card, event, 2, player, 3, 1, 0x80, 0);
  return result;
}

/* ManalinkEh 2022: 0x23cf450; original cards/strixhaven.c:2397. */
int card_eyetwitch(int player, int card, event_t event)
{
  int result;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  learn(player, card);
  return 0;
}

/* ManalinkEh 2022: 0x23cf8f0; original cards/strixhaven.c:2487. */
int card_field_trip(int player, int card, event_t event)
{
  int result;
  const char *msg;
  test_definition_t this_test;

  if (event == EVENT_RESOLVE_SPELL)
  {
    msg = get_hacked_land_text(player, card, "Select a basic  %s card.", 0x5001);
    new_default_test_definition(&this_test, 0xff, msg);
    this_test.subtype = get_hacked_subtype(player, card, 0x5001);
    this_test.sub2 = 0x2000;
    this_test.subtype_flag = 4;
    new_global_tutor(player, player, 1, 5, 0, 5, &this_test);
    learn(player, card);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23cffc0; original cards/strixhaven.c:2704. */
int card_gnarled_professor(int player, int card, event_t event)
{
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  learn(player, card);
  return 0;
}

/* ManalinkEh 2022: 0x23d0c90; original cards/strixhaven.c:3086. */
int card_hunt_for_specimens(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    make_pest(player, card, 1);
    learn(player, card);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23d0cf0; original cards/strixhaven.c:3102. */
int card_igneous_inspiration(int player, int card, event_t event)
{
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
        default_target_definition(player, card, &td, 0x102);
        td.zone = 0x1200;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            damage_target0(player, card, 3);
            learn(player, card);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_023d0d4c;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
LAB_023d0d4c:
  result = generic_spell(player, card, event, 1, &td, "TARGET_ANY", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23d58a0; original cards/strixhaven.c:4678. */
int card_overgrown_arch(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if ((iVar2 == 0) && (iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 8, 2,
                                                             0, 0, 0, 0, 0, 0, NULL, NULL),
                           iVar2 == 0))
      {
        return 0;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 8, 2, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar4 = 5;
      if (5 < life[player])
      {
        iVar4 = (6 - hand_count[player]) * 8;
      }
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\033", "\001\016",
                          "Gain 1 life", iVar2, 0x1e, "Learn", iVar3, iVar4, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 != 1)
      {
        if (iVar2 != 2)
        {
          return 0;
        }
        generic_activated_ability(player, card, EVENT_ACTIVATE, 8, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
        if (spell_fizzled == 1)
        {
          return 0;
        }
        iVar2 = -0x32;
        if (5 < life[player])
        {
          iVar2 = (6 - hand_count[player]) * 8;
        }
        ai_modifier = ai_modifier + iVar2;
        return 0;
      }
      generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      if (1 - player != current_turn)
      {
        return 0;
      }
      if (current_phase < 0x18)
      {
        return 0;
      }
      ai_modifier = ai_modifier + 0x30;
      return 0;
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
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->mode == '\x01')
    {
      gain_life_2022(player, 1);
    }
    else if (instance->mode == '\x02')
    {
      learn(player, card);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d7120; original cards/strixhaven.c:5085. */
int card_poets_quill(int player, int card, event_t event)
{
  int result;
  int ec;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    attached_gets_pt(player, card, event, 1, 1);
    attached_gets_kw(player, card, event, KEYWORD_0, SP_KEYWORD_LIFELINK);
    if ((((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
         (event == EVENT_RESOLVE_ACTIVATION)) ||
        (event == EVENT_CLEANUP))
    {
      result = basic_equipment(player, card, event, 0x10101);
    }
    else
    {
      result = 0;
    }
    return result;
  }
  learn(player, card);
  attached_gets_pt(player, card, EVENT_ETB, 1, 1);
  attached_gets_kw(player, card, EVENT_ETB, KEYWORD_0, SP_KEYWORD_LIFELINK);
  return 0;
}

/* ManalinkEh 2022: 0x23d7250; original cards/strixhaven.c:5112. */
int card_pop_quiz(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    draw_cards(player, 1);
    learn(player, card);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23d96f0; original cards/strixhaven.c:5895. */
int card_retriever_phoenix(int player, int card, event_t event)
{
  int result;

  haste(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = check_special_flags_2022(player, card, 0x40000);
    if (result == 0)
    {
      learn(player, card);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int shadewing_laureate_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23c9a40; original cards/strixhaven.c:432. */
int card_baleful_mastery(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  target_definition_t td;

  if (event == EVENT_MODIFY_COST)
  {
    iVar2 = has_mana_multi(player, 1, 1, 0, 0, 0, 0);
    if (iVar2 != 0)
    {
      null_casting_cost(player, card);
      pcVar1 = get_card_instance(player, card);
      pcVar1->info_slot = 1;
      return 0;
    }
    pcVar1 = get_card_instance(player, card);
    pcVar1->info_slot = 0;
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
      default_target_definition(player, card, &td, 0x102);
      pcVar1 = get_card_instance(player, card);
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_023c9abe;
      pcVar1->targets[1].player = 0;
      pcVar3 = get_card_instance(player, card);
      if ((pcVar3->info_slot != 1) || (iVar2 = is_token(player, card), iVar2 != 0))
        goto LAB_023c9abe;

      iVar2 = has_mana_to_cast_id(player, EVENT_CAST_SPELL, 0x583b);
      if (iVar2 == 0)
      {
      LAB_023c9d54:

        iVar2 = get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, 1);
        charge_mana_multi(player, iVar2, 1, 0, 0, 0, 0);
        iVar4 = spell_fizzled;
        if (spell_fizzled == 1)
        {
          return 0;
        }
        pcVar1->targets[1].player = 0x42;
      }
      else
      {
        iVar2 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014",
                            "Pay alternate mana cost", 1, 0x14, "Pay full mana cost", 1, 10, 0, 0,
                            0, 0);
        if (iVar2 == 0)
        {
          spell_fizzled = 1;
          iVar4 = spell_fizzled;
        }
        else
        {
          if (iVar2 == 1)
            goto LAB_023c9d54;
          iVar4 = spell_fizzled;
          if (iVar2 == 2)
          {
            charge_mana_from_id(player, -1, EVENT_CAST_SPELL, 0x583b);
            iVar4 = spell_fizzled;
          }
        }
      }
      if (iVar4 != 0)
      {
        return 0;
      }
      goto LAB_023c9abe;
    }
    if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
        (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 0x102);
  pcVar1 = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    iVar2 = valid_target(&td);
    if (iVar2 != 0)
    {
      if (pcVar1->targets[1].player == 0x42)
      {
        draw_cards(1 - player, 1);
      }
      action_on_target(player, card, 0, ACT_KILL_REMOVE);
    }
    kill_card(player, card, KILL_DESTROY);
  }
LAB_023c9abe:
  iVar2 = generic_spell(player, card, event, 0x81, &td, "Select target creature or planeswalker.",
                        1, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x23ce530; original cards/strixhaven.c:2075. */
int card_emergent_sequence(int player, int card, event_t event)
{
  int result;
  int num;
  card_instance_t *instance;
  test_definition_t this_test;

  if (event == EVENT_RESOLVE_SPELL)
  {

    new_default_test_definition(&this_test, 1, "Select a basic land card.");
    this_test.subtype = 0x2000;
    result = new_global_tutor(player, player, 1, 5, 1, 5, &this_test);
    if (-1 < result)
    {

      num = get_trap_condition(player, 0x402);
      add_counters_from_source(player, card, player, result, COUNTER_P1_P1, num);
      result = turn_into_creature(player, card, player, result, 0, 0, 0);
      instance = get_card_instance(player, result);
      result = get_id(player, card);
      instance->targets[2].card = result;
      result = get_sleighted_color_test(player, card, 0xc);
      instance->targets[9].card = 0x10a;
      instance->targets[6].card = result;
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23ce960; original cards/strixhaven.c:2175. */
int card_expanded_anatomy(int player, int card, event_t event)
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
            add_counters_from_source(player, card, instance->targets[0].player,
                                     instance->targets[0].card, COUNTER_P1_P1, 2);
            pump_ability_until_eot_2022(player, card, instance->targets[0].player,
                                   instance->targets[0].card, 0, 0, 0, 8);
          }
          kill_card(player, card, KILL_DESTROY);
          return 0;
        }
        goto LAB_023ce9c4;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_023ce9c4:
  result = vanilla_instant_pump_2022(player, card, event, 2, player, 2, 2, 0, 8);
  return result;
}

/* ManalinkEh 2022: 0x23c7530; original cards/strixhaven.c:2509. */
static int fdoc_legacy(int player, int card, event_t event)
{
  etb_ability_global(player, event, 2, TYPE_CREATURE, NULL, player, card);
  if (event == EVENT_ETB)
  {
    add_counters_from_source(player, card, trigger_cause_controller, trigger_cause, COUNTER_P1_P1,
                             1);
    pump_ability_until_eot_2022(player, card, trigger_cause_controller, trigger_cause, 0, 0, 0, 4);
    return 0;
  }
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x23d02b0; original cards/strixhaven.c:2764. */
int card_guiding_voice(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_023d0377;
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
            add_counters_from_source(player, card, instance->targets[0].player,
                                     instance->targets[0].card, COUNTER_P1_P1, 1);
            learn(player, card);
          }
          kill_card(player, card, KILL_DESTROY);
          return 0;
        }
        goto LAB_023d0377;
      }
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_023d0377;
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_023d0377:
  result = vanilla_instant_pump_2022(player, card, event, 2, player, 1, 1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x23d56f0; original cards/strixhaven.c:4653. */
int card_oriq_loremage(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  test_definition_t this_test;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {
      ai_modifier = ai_modifier + 0x30;
      goto LAB_023d573a;
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
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    new_default_test_definition(&this_test, 0xff, "Select a card to put into your graveyard.");
    result = new_global_tutor(player, player, 1, 3, 0, 6, &this_test);
    if ((-1 < result) && (result = is_what(player, result, 0x38), result != 0))
    {
      add_counters_from_source(instance->parent_controller, instance->parent_card,
                               instance->parent_controller, instance->parent_card, COUNTER_P1_P1,
                               1);
    }
  }
LAB_023d573a:
  result = generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23c7f10; original cards/strixhaven.c:6398. */
static int shadewing_laureate_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  unsigned int uVar4;
  target_definition_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x590c))
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
  LAB_023c8060:
    uVar4 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 == 0)
      goto LAB_023c8081;
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
      goto LAB_023c8060;

    pcVar1 = get_card_instance(player, card);
    pcVar2 = in_play(affected_card_controller, affected_card);

    if (((pcVar2 != NULL) && (pcVar2 = get_card_instance(affected_card_controller, affected_card),
                              pcVar2->kill_code != '\0')) &&
        (result = check_for_ability(affected_card_controller, affected_card, 0x20), result != 0))
    {

      if (((-1 < pcVar1->damage_target_player) && (pcVar1->damage_target_card == affected_card)) &&
          (pcVar1->damage_target_player == affected_card_controller))
      {
        pcVar1->damage_target_card = -1;
        pcVar1->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        return 0;
      }
      count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
    }
    uVar4 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {

    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allow_cancel = 0;
    pcVar1 = get_card_instance(player, card);
    uVar4 = uVar4 & 0xff;
    if (uVar4 != 0)
    {
      do
      {
        result = can_target(&td);
        if (result == 0)
          break;
        result = new_pick_target(&td, "TARGET_CREATURE", 1, 0);
        if (result != 0)
        {
          add_counters_from_source(player, card, pcVar1->targets[1].player, pcVar1->targets[1].card,
                                   COUNTER_P1_P1, 1);
        }
        uVar4 = uVar4 - 1;
        pcVar1->number_of_targets = '\0';
      } while (0 < (int)uVar4);
    }
    kill_card(player, card, KILL_EXILE);
  }
LAB_023c8081:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23dac90; original cards/strixhaven.c:6456. */
int card_shadewing_laureate(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  target_definition_t td;

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
                instance->info_slot_fn == shadewing_laureate_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_023dacb2;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, shadewing_laureate_effect, player, card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  }
  else if ((event == EVENT_GRAVEYARD_FROM_PLAY) &&
           (iVar2 = check_for_ability(affected_card_controller, affected_card, 0x20), iVar2 != 0))
  {
    count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
  }
LAB_023dacb2:
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (uVar4 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar4 != 0))
  {

    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    uVar4 = uVar4 & 0xff;
    if (uVar4 != 0)
    {
      do
      {
        iVar2 = can_target(&td);
        if (iVar2 == 0)
          break;
        iVar2 = new_pick_target(&td, "TARGET_CREATURE", 0, 0);
        if (iVar2 != 0)
        {
          add_counters_from_source(player, card, instance->targets[0].player,
                                   instance->targets[0].card, COUNTER_P1_P1, 1);
        }
        uVar4 = uVar4 - 1;
        instance->number_of_targets = '\0';
      } while (0 < (int)uVar4);
    }
  }
  iVar2 = hybrid(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x23db3a0; original cards/strixhaven.c:6613. */
int card_shaile_dean_of_radiance(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  mdfc_front(player, card, event);
  check_legend_rule(player, card, event);
  vigilance(player, card, event);
  if (event == EVENT_ACTIVATE)
  {

    iVar3 = active_cards_count[player] + -1;
    if (-1 < iVar3)
    {
      do
      {
        instance = in_play(player, iVar3);
        if (((instance != NULL) && (iVar2 = is_what(player, iVar3, 2), iVar2 != 0)) &&
            (iVar2 = check_special_flags_2022(player, iVar3, 0x1000), iVar2 != 0))
        {
          ai_modifier = ai_modifier + 10;
        }
        iVar3 = iVar3 + -1;
      } while (iVar3 != -1);
    }
  }
  else
  {

    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (iVar3 = active_cards_count[player] + -1, -1 < active_cards_count[player] + -1))
    {
      do
      {
        while ((
            instance = in_play(player, iVar3),
            instance == NULL ||
                ((iVar2 = is_what(player, iVar3, 2),
                  iVar2 == 0 || (iVar2 = check_special_flags_2022(player, iVar3, 0x1000), iVar2 == 0)))))
        {
          iVar3 = iVar3 + -1;
          if (iVar3 == -1)
            goto LAB_023db47b;
        }
        iVar2 = iVar3 + -1;
        add_counters_from_source(player, card, player, iVar3, COUNTER_P1_P1, 1);
        iVar3 = iVar2;
      } while (iVar2 != -1);
    }
  }
LAB_023db47b:
  iVar3 = generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x23db560; original cards/strixhaven.c:6693. */
int card_embrose_dean_of_shadow(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *pcVar2;
  int iVar3;
  unsigned int uVar4;
  card_instance_t *pcVar5;
  int iVar6;
  target_definition_t td;

  mdfc_back(player, card, event);
  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar3 = 0;
        do
        {
          pcVar2 = in_play(player, iVar3);

          if ((((pcVar2 != NULL) && (iVar6 = is_what(player, iVar3, 0x80), iVar6 != 0)) &&
               (pcVar2 = get_card_instance(player, iVar3),
                pcVar2->info_slot_fn == embrose_dean_of_shadow_effect)) &&
              (((pcVar2->damage_target_player == player && (pcVar2->damage_target_card == card)) ||
                ((pcVar2->targets[0].player == player && (pcVar2->targets[0].card == card))))))
            goto LAB_023db5aa;
          iVar3 = iVar3 + 1;
        } while (iVar3 < active_cards_count[player]);
      }

      iVar3 =
          create_targetted_legacy_effect(player, card, embrose_dean_of_shadow_effect, player, card);
      pcVar2 = get_card_instance(player, iVar3);
      pcVar2->targets[0].player = player;
      pcVar2->targets[0].card = card;
      pcVar2->number_of_targets = '\x01';
      add_status(player, iVar3, 0x10000);
    }
  LAB_023db5aa:
    pcVar2 = get_card_instance(player, card);
    if (pcVar2->kill_code == '\0')
      goto LAB_023db6d0;
  }
  else
  {
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_023db5aa;
    iVar3 = count_counters(affected_card_controller, affected_card, COUNTER_P1_P1);
    if (iVar3 != 0)
    {
      count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
    }
    pcVar2 = get_card_instance(player, card);
    if (pcVar2->kill_code != '\0')
    {
      return 0;
    }
  LAB_023db6d0:

    uVar4 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 != 0)
    {

      draw_cards(player, uVar4 & 0xff);
    }
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 2;
    pcVar2 = get_card_instance(player, card);
    if (event != EVENT_ACTIVATE)
      goto LAB_023db616;
    generic_activated_ability(player, card, EVENT_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0, &td,
                              "TARGET_CREATURE");
    if (spell_fizzled != 1)
    {
      iVar3 = get_toughness(pcVar2->targets[0].player, pcVar2->targets[0].card);
      pcVar5 = get_card_instance(pcVar2->targets[0].player, pcVar2->targets[0].card);
      iVar3 = (iVar3 + 1) - (unsigned int)pcVar5->damage_on_card;
      if (pcVar2->targets[0].player == player)
      {
        bVar1 = 2 < iVar3;
      }
      else
      {
        bVar1 = iVar3 < 3;
      }
      ai_modifier = ai_modifier + (bVar1 - 1 & 0xffffffa0) + 0x30;
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 2;
    pcVar2 = get_card_instance(player, card);
  LAB_023db616:
    if ((event == EVENT_RESOLVE_ACTIVATION) && (iVar3 = valid_target(&td), iVar3 != 0))
    {
      add_counters_from_source(player, card, pcVar2->targets[0].player, pcVar2->targets[0].card,
                               COUNTER_P1_P1, 1);
      damage_target0(player, card, 2);
    }
    iVar3 = generic_activated_ability(player, card, event, 5, 0, 0, 0, 0, 0, 0, 0, &td,
                                      "TARGET_CREATURE");
    return iVar3;
  }
  return 0;
}

/* ManalinkEh 2022: 0x23dc8a0; original cards/strixhaven.c:7064. */
int card_sparring_regimen(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td1;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    learn(player, card);
    store_attackers(player, card, EVENT_ETB, 0, player, -1, NULL);
    dat_ability(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  }
  else
  {
    store_attackers(player, card, event, 0, player, -1, NULL);
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {

      default_target_definition(player, card, &td1, 2);
      td1.required_state = 2;
      td1.allow_cancel = 0;
      instance = get_card_instance(player, card);
      result = can_target(&td1);
      if (result != 0)
      {
        result = new_pick_target(&td1, "Select target attacking creature.", 0, 0x80);
        if (result != 0)
        {
          add_counters_from_source(player, card, instance->targets[0].player,
                                   instance->targets[0].card, COUNTER_P1_P1, 1);
          untap_card(instance->targets[0].player, instance->targets[0].card);
        }
      }
      instance->number_of_targets = '\0';
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int valentin_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23cada0; original cards/strixhaven.c:915. */
int card_codie_vociferous_codex(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    instance = in_play(player, card);
    if ((((instance != NULL) &&
          (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
         (affected_card_controller == player)) &&
        (result = is_what(player, affected_card, 0x146), result != 0))
    {
      infinite_casting_cost();
    }
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    result = can_produce_mana_2022(player, card);
    if ((((result != 0) && (result = is_tapped(player, card), result == 0)) &&
         ((result = is_animated_and_sick(player, card),
           result == 0 && ((instance = get_card_instance(player, card),
                            instance->targets[1].player != 0x42 &&
                                (result = has_mana(player, 0, 9), result != 0)))))) &&
        (result = can_produce_mana_2022(player, card), result != 0))
    {
      return 1;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {

    instance = get_card_instance(player, card);
    instance->targets[1].player = 0x42;
    charge_mana(player, COLOR_COLORLESS, 4);
    instance->targets[1].player = 0;
    if (spell_fizzled != 1)
    {
      add_state(player, card, 0x10);
      produce_mana_multi(player, 0, 1, 1, 1, 1, 1);
      create_legacy_effect(instance->parent_controller, instance->parent_card, codie_effect);
    }
  }
  else if (((((event == EVENT_COUNT_MANA) && (card == affected_card)) &&
             (player == affected_card_controller)) &&
            ((result = is_tapped(player, card),
              result == 0 && (result = is_animated_and_sick(player, card), result == 0)))) &&
           ((result = can_produce_mana_2022(player, card),
             result != 0 && (instance = get_card_instance(player, card),
                             instance->targets[1].player != 0x42))))
  {
    declare_mana_available(player, 1, 1);
    declare_mana_available(player, 2, 1);
    declare_mana_available(player, 3, 1);
    declare_mana_available(player, 4, 1);
    declare_mana_available(player, 5, 1);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x23cbc70; original cards/strixhaven.c:1206. */
int card_culmination_of_studies(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_RESOLVE_SPELL)
  {

    instance = get_card_instance(player, card);
    result = instance->info_slot;
    reveal_top_cards_of_library(player, result);
    if (result != 0)
    {
      do
      {

        result = *deck_ptr[player];
        rfg_top_card_of_deck(player);
        is_what(-1, result, 1);
        get_color_by_internal_id(player, result);
        get_sleighted_color_test(player, card, 4);
        get_color_by_internal_id(player, result);
        get_sleighted_color_test(player, card, 0x10);
      } while (true);
    }
    generate_tokens_by_id_2022(player, card, 0x4998, 0);
    draw_cards(player, 0);
    damage_player(1 - player, 0, player, card);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23cd680; original cards/strixhaven.c:1684. */
int card_dragons_approach(int player, int card, event_t event)
{
  int iVar1;
  unsigned int uVar2;
  int iVar3;
  test_definition_t test;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_test_definition(&test, 0xff);
      test.id = get_id(player, card);
      iVar1 = new_special_count_grave(player, &test);
      if (3 < iVar1)
      {
        ai_modifier = ai_modifier + 0x60;
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    damage_player(1 - player, 3, player, card);
    default_test_definition(&test, 0xff);
    test.id = 0x586c;
    iVar1 = new_special_count_grave(player, &test);

    if ((3 < iVar1) &&
        (iVar1 = do_dialog(player, player, card, -1, -1, " Tutor a Dragon\n Decline", 0),
         iVar1 == 0))
    {

      uVar2 = get_id(player, card);
      iVar1 = count_graveyard(player);
      iVar1 = iVar1 + -1;
      if (-1 < iVar1)
      {
        iVar3 = 0;
        do
        {
          if (cards_data[graveyard_ptr_mutable[player][iVar1]].id == uVar2)
          {
            rfg_card_from_grave(player, iVar1);
            iVar3 = iVar3 + 1;
          }

          iVar1 = iVar1 + -1;
        } while ((-1 < iVar1) && (iVar3 < 4));
      }
      new_default_test_definition(&test, 2, "Select a Dragon creature card.");
      test.subtype = 0x30;
      new_global_tutor(player, player, 1, 4, 1, 2, &test);
      kill_card(player, card, KILL_EXILE);
      return 0;
    }
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x23cf9e0; original cards/strixhaven.c:2523. */
int card_first_day_of_class(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar1 = 1;
      do
      {

        iVar2 = has_mana(player, 7, iVar1);
        if (iVar2 != 0)
        {
          ai_modifier = ai_modifier + 8;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 != 0x10);
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_effect(player, card, fdoc_legacy);
    learn(player, card);
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x23cfaa0; original cards/strixhaven.c:2550. */
int card_flamescroll_celebrant(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  mdfc_front(player, card, event);
  if (((event == EVENT_PLAY_ABILITY) || (event == EVENT_TAPPED_TO_PLAY_ABILITY)) &&
      (1 - player == affected_card_controller))
  {
    instance = get_card_instance(1 - player, affected_card);
    if (((cards_data[instance->internal_card_id].extra_ability & 0x1000) == 0) ||
        (tapped_for_mana_color < 0))
    {
      instance = in_play(player, card);
      if (instance != NULL)
      {
        result = is_humiliated_for_ability(player, card, AL_TRIG);
        if (result == 0)
        {
          damage_player(affected_card_controller, 1, player, card);
        }
      }
    }
  }
  result = generic_shade(player, card, event, 0, 1, 0, 0, 0, 1, 0, 2, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x23d2e30; original cards/strixhaven.c:3845. */
int card_letter_of_acceptance(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      if ((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9,
                                                               3, 0, 0, 0, 0, 0, 0, NULL, NULL),
                            result == 0))
      {
        return 0;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->mode = '\0';
      result = paying_mana();
      if ((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9,
                                                               3, 0, 0, 0, 0, 0, 0, NULL, NULL),
                            result != 0))
      {

        result = count_permanents_by_type(player, TYPE_LAND);
        result = do_dialog(player, player, card, -1, -1, " Produce mana\n Sac and draw 1\n Cancel",
                           (unsigned int)(6 < result));
        if (result != 0)
        {
          if (result == 1)
          {
            add_state(player, card, 0x10);
            generic_activated_ability(player, card, EVENT_ACTIVATE, 8, 2, 0, 0, 0, 0, 0, 0, NULL,
                                      NULL);
            if (spell_fizzled != 1)
            {
              result = hand_count[player];
              instance->mode = '\x01';
              ai_modifier = ai_modifier + result * -10 + 0x46;
              return 0;
            }
            remove_state(player, card, 0x10);
            return 0;
          }
          if (result != 2)
          {
            return 0;
          }
          spell_fizzled = 1;
          return 0;
        }
      }
      event = EVENT_ACTIVATE;
      goto LAB_023d2f43;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
    LAB_023d2f43:
      result = mana_producer(player, card, event);
      return result;
    }
    instance = get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (instance->mode == '\x01'))
  {
    draw_cards(player, 1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d3290; original cards/strixhaven.c:3943. */
int card_lorehold_campus(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int argument7;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      if ((iVar2 == 0) && (iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 5,
                                                             0, 0, 0, 0, 0, 0, NULL, NULL),
                           iVar2 == 0))
      {
        return 0;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->mode = '\0';
      iVar2 = paying_mana();
      if ((iVar2 == 0) && (iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 5,
                                                             0, 0, 0, 0, 0, 0, NULL, NULL),
                           iVar2 != 0))
      {

        argument7 = 0;
        if (hand_count[player] < 3)
        {
          argument7 = (unsigned int)(current_turn != player);
        }
        iVar2 =
            do_dialog(player, player, card, -1, -1, " Produce mana\n Scry 1\n Cancel", argument7);
        if (iVar2 != 0)
        {
          if (iVar2 == 1)
          {
            add_state(player, card, 0x10);
            generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 4, 0, 0, 0, 0, 0, 0, NULL,
                                      NULL);
            if (spell_fizzled != 1)
            {
              iVar3 = -0x30;
              iVar2 = hand_count[player];
              instance->mode = '\x01';
              if (iVar2 < 3)
              {
                iVar3 = ((current_turn != player) - 1 & 0xffffffa0) + 0x30;
              }
              ai_modifier = ai_modifier + iVar3;
              return 0;
            }
            remove_state(player, card, 0x10);
            return 0;
          }
          if (iVar2 != 2)
          {
            return 0;
          }
          spell_fizzled = 1;
          return 0;
        }
      }
      iVar2 = mana_producer(player, card, EVENT_ACTIVATE);
      return iVar2;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      iVar2 = mana_producer_tapped(player, card, event);
      return iVar2;
    }
    instance = get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (instance->mode == '\x01'))
  {
    scry(player, 1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d6740; original cards/strixhaven.c:4907. */
int card_plargg_dean_of_chaos(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  char buffer[100];
  test_definition_t test;

  check_legend_rule(player, card, event);
  mdfc_front(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x21, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if ((iVar2 == 0) && (iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 4,
                                                             0, 0, 0, 1, 0, 0, NULL, NULL),
                           iVar2 == 0))
      {
        return 0;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x21, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 4, 0, 0, 0, 1, 0, 0,
                                        NULL, NULL);
      iVar3 = count_deck(player);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\033",
                          "Discard & Draw", iVar2, 0x14, "Reveal cards", iVar4, iVar3, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 1)
      {
        generic_activated_ability(player, card, EVENT_ACTIVATE, 0x21, 0, 0, 0, 0, 0, 0, 0, NULL,
                                  NULL);
        if (spell_fizzled == 1)
        {
          return 0;
        }
        if (1 - player != current_turn)
        {
          return 0;
        }
        if (current_phase < 0x18)
        {
          return 0;
        }
        ai_modifier = ai_modifier + (7 - hand_count[player]) * 8;
        return 0;
      }
      if (iVar2 != 2)
      {
        return 0;
      }
      generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 4, 0, 0, 0, 1, 0, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      iVar2 = count_deck(player);
      ai_modifier = ai_modifier + iVar2;
      return 0;
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
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->mode == '\x01')
    {
      draw_a_card(player);
    }
    else if (instance->mode == '\x02')
    {

      default_test_definition(&test, 1);
      test.subtype = 0x2001;
      test.type_flag = 1;
      test.subtype_flag = 1;
      test.cmc = 4;
      test.cmc_flag = 3;
      iVar2 = reveal_card_until_you_reveal_test(player, player, &test, 0x400);
      if (iVar2 != 0)
      {
        iVar4 = deck_ptr[player][iVar2 + -1];
        if ((iVar4 != -1) && (iVar3 = new_make_test(player, iVar4, -1, &test), iVar3 != 0))
        {

          scnprintf(buffer, 100, " Play %s\n Decline", cards_ptr[cards_data[iVar4].id]->name);
          iVar4 = do_dialog(player, player, card, -1, -1, buffer, 0);
          if (iVar4 == 0)
          {
            iVar2 = iVar2 + -1;
            play_card_in_deck_for_free(player, player, iVar2);
          }
        }
        put_top_x_on_bottom_in_random_order(player, iVar2);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d6d00; original cards/strixhaven.c:4991. */
int card_augusta_dean_of_order(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int *piVar4;
  bool bVar5;
  target_definition_t td;
  test_definition_t test;
  int marked[151];

  mdfc_back(player, card, event);
  check_legend_rule(player, card, event);
  instance = in_play(player, card);
  if ((instance == NULL) &&
      (iVar3 = is_humiliated_for_ability(player, card, AL_STATIC), iVar3 != 0))
  {
    return 0;
  }
  if (((card == affected_card) || (player != affected_card_controller)) ||
      (iVar3 = is_what(player, affected_card, 2), iVar3 == 0))
  {
  LAB_023d6d5a:
    store_attackers(player, card, event, 0, player, -1, NULL);
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {

      default_test_definition(&test, 2);
      new_manipulate_all(player, card, player, &test, ACT_UNTAP);
      default_target_definition(player, card, &td, 2);
      td.illegal_abilities = 0;
      td.preferred_controller = player;
      td.allowed_controller = player;
      td.allow_cancel = 3;
      instance = get_card_instance(player, card);
      iVar3 = 0;
      while ((iVar2 = can_target(&td),
              iVar2 != 0 &&
                  (iVar2 = new_pick_target(&td, "Select a creature to tap.", 0, 0x80), iVar2 != 0)))
      {
        iVar2 = instance->targets[0].card;
        marked[iVar3] = iVar2;
        state_untargettable(player, iVar2, 1);
        iVar3 = iVar3 + 1;
      }
      if (0 < iVar3)
      {
        piVar4 = marked + iVar3 + -1;
        do
        {
          iVar3 = *piVar4;
          state_untargettable(player, iVar3, 0);
          if (spell_fizzled != 1)
          {
            tap_card(player, iVar3);
          }
          bVar5 = piVar4 != marked;
          piVar4 = piVar4 + -1;
        } while (bVar5);
        return 0;
      }
    }
  }
  else
  {
    if (event == EVENT_POWER)
    {
      iVar3 = is_tapped(affected_card_controller, affected_card);
      if (iVar3 != 0)
      {
      LAB_023d6e04:
        event_result = event_result + 1;
      }
    }
    else
    {
      if (event != EVENT_TOUGHNESS)
        goto LAB_023d6d5a;
      iVar3 = is_tapped(affected_card_controller, affected_card);
      if (iVar3 == 0)
        goto LAB_023d6e04;
    }
    store_attackers(player, card, event, 0, player, -1, NULL);
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d9cf0; original cards/strixhaven.c:6010. */
int card_rowan_scholar_of_sparks(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  mdfc_front(player, card, event);
  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    instance = in_play(player, card);
    if ((((instance != NULL) &&
          (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
         (affected_card_controller == player)) &&
        (result = is_what(player, affected_card, 0x38), result != 0))
    {
      COST_COLORLESS = COST_COLORLESS + -1;
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) ||
           (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE))
  {

    instance = get_card_instance(player, card);
    result = dialog_impl(player, card, event, "\001\002", "\001\017", "Damage opponent", 1, 10, 1,
                         "Emblem", 1, 0x32, 0xfffffffc, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (result == 0)
      {
        return 0;
      }
    }
    else if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE)
    {
      instance->number_of_targets = '\0';
    }
    else if (result == 1)
    {

      damage_player(1 - player,
                    (-(unsigned int)(cards_drawn_this_turn[player] < 3) & 0xfffffffe) + 3, player,
                    card);
    }
    else
    {
      generate_token_by_id_2022(player, card, 0x5903);
    }
  }
  result = planeswalker(player, card, event, 2);
  return result;
}

/* ManalinkEh 2022: 0x23de0e0; original cards/strixhaven.c:7596. */
int card_the_biblioplex(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  bool bVar4;
  char buffer[100];

  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((hand_count[player] == 7) || (hand_count[player] == 0))
    {
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 3, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if (iVar3 != 0)
      {
        return 1;
      }
    }
    iVar3 = mana_producer(player, card, EVENT_CAN_ACTIVATE);
    return iVar3;
  }
  if (event == EVENT_ACTIVATE)
  {

    instance->mode = '\0';
    iVar3 = paying_mana();
    if (((iVar3 != 0) || (((hand_count[player] != 7 && (hand_count[player] != 0)) ||
                           (iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1,
                                                              3, 0, 0, 0, 0, 0, 0, NULL, NULL),
                            iVar3 == 0)))) ||
        (iVar3 = do_dialog(player, player, card, -1, -1,
                           " Produce mana\n Look at top card\n Cancel", 1),
         iVar3 == 0))
    {
      iVar3 = mana_producer(player, card, EVENT_ACTIVATE);
      return iVar3;
    }
    if (iVar3 == 1)
    {
      add_state(player, card, 0x10);
      generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        remove_state(player, card, 0x10);
      }
      else
      {
        bVar4 = current_turn != player;
        instance->mode = '\x01';
        ai_modifier = ai_modifier + (bVar4 - 1 & 0xffffffa0) + 0x30;
      }
    }
    else
    {
      spell_fizzled = 1;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (instance->mode == '\0')
    {
      iVar3 = mana_producer(player, card, EVENT_RESOLVE_ACTIVATION);
      return iVar3;
    }

    iVar3 = *deck_ptr[player];
    if (iVar3 != -1)
    {
      iVar2 = is_what(-1, iVar3, 0x38);
      if (iVar2 != 0)
      {

        scnprintf(buffer, 100, " %s to hand\n Decline", cards_ptr[cards_data[iVar3].id]->name);
        iVar2 = do_dialog(player, player, card, -1, -1, buffer, 0);
        if (iVar2 == 0)
        {
          reveal_card_iid(player, card, iVar3);
          add_card_to_hand(player, iVar3);
          remove_card_from_deck(player, 0);
          return 0;
        }
      }
      scnprintf(buffer, 100, " %s to grave\n Leave it on top of deck",
                cards_ptr[cards_data[iVar3].id]->name);
      iVar3 = internal_rand(0x32);
      iVar3 = do_dialog(player, player, card, -1, -1, buffer, (unsigned int)(0x19 < iVar3));
      if (iVar3 == 0)
      {
        mill(player, 1);
      }
    }
  }
  else if (event != EVENT_CLEANUP)
  {
    iVar3 = mana_producer(player, card, event);
    return iVar3;
  }
  return 0;
}

/* ManalinkEh 2022: 0x23c7140; original cards/strixhaven.c:7853. */
static int valentin_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  int result;
  card_instance_t *pcVar4;
  int t_player;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x3d07))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if (-1 < pcVar1->damage_target_player)
    {
      result = effect_follows_control_of_attachment(player, card, 0x4700);
      if (result != 0)
      {
        return 0;
      }
      goto LAB_023c717f;
    }
    uVar2 = resolve_gfp_ability(player, card, EVENT_STATIC_EFFECTS, RESOLVE_TRIGGER_MANDATORY);
    result = check_status(player, card, 0x10000);
    if (result != 0)
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
        }
        default_test_definition(&test, 2);
        t_player = 1 - player;
        test.type_flag = 3;
        count_for_gfp_ability_and_store_values(player, card, 0x77, t_player, 0, &test, 0x40, 0);
        result = is_token(affected_card_controller, affected_card);
        if ((((result == 0) &&
              (result = is_this_dying(affected_card_controller, affected_card), result != 0)) &&
             (t_player == affected_card_controller)) &&
            (result = is_what(t_player, affected_card, 2), result != 0))
        {
          pcVar1 = get_card_instance(affected_card_controller, affected_card);
          pcVar1->kill_code = '\x04';
          set_special_flags2_2022(affected_card_controller, affected_card, 0x10000000);
        }
      }
      uVar2 =
          resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
      result = check_status(player, card, 0x10000);
      if (result != 0)
      {
        return 0;
      }
      uVar2 = uVar2 & 0xff;
      if (uVar2 != 0)
        goto LAB_023c71ed;
      return 0;
    }
  LAB_023c717f:
    uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    result = check_status(player, card, 0x10000);
    if (result != 0)
      goto LAB_023c7209;
  }

  for (uVar2 = uVar2 & 0xff; uVar2 != 0; uVar2 = uVar2 - 1)
  {
  LAB_023c71ed:
    result = has_mana(player, 0, 2);
    if ((result == 0) || (result = charge_mana(player, COLOR_COLORLESS, 2), result == 0))
      break;
    make_pest(player, card, 1);
  }
LAB_023c7209:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23debd0; original cards/strixhaven.c:7915. */
int card_valentin_dean_of_the_vein(int player, int card, event_t event)
{
  unsigned int uVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  test_definition_t test;

  mdfc_front(player, card, event);
  check_legend_rule(player, card, event);
  menace(player, card, event);
  lifelink(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
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
                instance->info_slot_fn == valentin_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_023dec3a;
          iVar3 = iVar3 + 1;
        } while (iVar3 < active_cards_count[player]);
      }

      iVar3 = create_targetted_legacy_effect(player, card, valentin_effect, player, card);
      instance = get_card_instance(player, iVar3);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar3, 0x10000);
    }
  }
  else if ((event == EVENT_GRAVEYARD_FROM_PLAY) &&
           (instance = in_play(affected_card_controller, affected_card), instance != NULL))
  {
    default_test_definition(&test, 2);
    iVar4 = 1 - player;
    test.type_flag = 3;
    count_for_gfp_ability_and_store_values(player, card, 0x77, iVar4, 0, &test, 0x40, 0);
    iVar3 = is_token(affected_card_controller, affected_card);
    if ((iVar3 == 0) && (((iVar3 = is_this_dying(affected_card_controller, affected_card),
                           iVar3 != 0 && (iVar4 == affected_card_controller)) &&
                          (iVar3 = is_what(iVar4, affected_card, 2), iVar3 != 0))))
    {
      instance = get_card_instance(affected_card_controller, affected_card);
      instance->kill_code = '\x04';
      set_special_flags2_2022(affected_card_controller, affected_card, 0x10000000);
    }
  }
LAB_023dec3a:
  uVar1 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  instance = get_card_instance(player, card);
  if ((instance->kill_code == '\0') && (uVar1 != 0))
  {

    uVar1 = uVar1 & 0xff;
    while (((uVar1 != 0 && (iVar3 = has_mana(player, 0, 2), iVar3 != 0)) &&
            (iVar3 = charge_mana(player, COLOR_COLORLESS, 2), iVar3 != 0)))
    {
      make_pest(player, card, 1);
      uVar1 = uVar1 - 1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23def30; original cards/strixhaven.c:7981. */
int card_lisette_dean_of_the_root(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  mdfc_back(player, card, event);
  check_legend_rule(player, card, event);
  result = trigger_gain_life_2022(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = has_mana(player, 0, 1);
      if (result != 0)
      {
        result = charge_mana(player, COLOR_COLORLESS, 1);
        if (result != 0)
        {
          manipulate_type(player, card, player, TYPE_CREATURE,
                          ACT_ADD_COUNTERS_BASE | ACT_KILL_BURY);
          pump_creatures_until_eot(player, card, player, 0, 0, 0, 0x80, 0, NULL);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23df410; original cards/strixhaven.c:8092. */
int card_verdant_mastery(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  int iStack_1d8;
  int *piStack_1d4;
  int chosen[4];
  test_definition_t test;
  test_definition_t test2;

  if (event == EVENT_MODIFY_COST)
  {
    iVar2 = has_mana_multi(player, 3, 0, 0, 1, 0, 0);
    if (iVar2 != 0)
    {
      null_casting_cost(player, card);
      instance = get_card_instance(player, card);
      instance->mode = '\x01';
      return 0;
    }
    instance = get_card_instance(player, card);
    instance->mode = '\0';
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
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->targets[1].player = 0;
        instance = get_card_instance(player, card);
        if ((instance->mode == '\x01') && (iVar2 = is_token(player, card), iVar2 == 0))
        {

          iVar2 = has_mana_to_cast_id(player, EVENT_CAST_SPELL, 0x5947);
          iVar2 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033", "\001\016",
                              "Pay alternate mana cost", 1, 0x14, "Pay full mana cost", iVar2, 10,
                              0, 0, 0, 0);
          if (iVar2 == 0)
          {
            spell_fizzled = 1;
          }
          else if (iVar2 == 1)
          {
            charge_mana_multi(player, 3, 0, 0, 1, 0, 0);
          }
          else if (iVar2 == 2)
          {
            charge_mana_from_id(player, -1, EVENT_CAST_SPELL, 0x5947);
          }
          if (spell_fizzled != 0)
          {
            return 0;
          }
        }
      }
      goto LAB_023df46b;
    }
    if (((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_RESOLVE_SPELL)) &&
        (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  instance = get_card_instance(player, card);
  if (event != EVENT_RESOLVE_SPELL)
    goto LAB_023df46b;

  chosen[0] = -1;
  chosen[1] = -1;
  chosen[2] = -1;
  chosen[3] = -1;
  default_test_definition(&test, 1);
  test.subtype = 0x2000;
  test.no_shuffle = 1;
  if (ai_is_speculating != 1)
  {
    scnprintf(test.message, 100, "Select four basic land cards: #%d", 1);
  }
  iVar2 = 0;
  do
  {

    iVar3 = new_select_a_card(player, player, 1, 0, 1, -1, &test);
    if (iVar3 == -1)
    {
      shuffle(player);
      if ((instance->mode != '\x01') || (iVar2 == 0))
        goto LAB_023df9f0;
      goto LAB_023df8d6;
    }
    chosen[iVar2] = deck_ptr[player][iVar3];
    iVar2 = iVar2 + 1;
    remove_card_from_deck(player, iVar3);
  } while (iVar2 != 4);
  shuffle(player);
  if (instance->mode == '\x01')
  {
  LAB_023df8d6:

    piStack_1d4 = chosen;
    new_default_test_definition(&test2, 0xff,
                                "Select a land card to put into play under opponent\'s control.");
    iVar3 = select_card_from_zone(player, player, piStack_1d4, iVar2, 1, 1, -1, &test2);
    if (-1 < iVar3)
    {

      iVar5 = 1 - player;
      iVar4 = add_card_to_hand(iVar5, chosen[iVar3]);
      add_state(iVar5, iVar4, 0x10);
      if (player == 0)
      {
        add_state(1, iVar4, 0x1000);
      }
      else
      {
        remove_state(iVar5, iVar4, 0x1000);
      }
      put_into_play(iVar5, iVar4);
      if (iVar3 < iVar2)
      {
        piVar6 = piStack_1d4 + iVar3;
        do
        {
          piVar7 = piVar6 + 1;
          *piVar6 = piVar6[1];
          piVar6 = piVar7;
        } while (piVar7 != piStack_1d4 + iVar2);
      }
      iVar2 = iVar2 + -1;
      chosen[iVar2] = -1;
    }
  LAB_023df9f0:
    new_default_test_definition(&test, 1, "Select a land card to put into play.");
    iStack_1d8 = iVar2;
    if (2 < iVar2)
    {
      iStack_1d8 = 2;
    }
    if (0 < iVar2)
      goto LAB_023df64e;
  }
  else
  {
    new_default_test_definition(&test, 1, "Select a land card to put into play.");

    iStack_1d8 = 2;
  LAB_023df64e:
    piStack_1d4 = chosen;
    iVar3 = 0;
    do
    {

      while (iVar4 = select_card_from_zone(player, player, piStack_1d4, iVar2, 1, 1, -1, &test),
             iVar4 < 0)
      {
        iVar3 = iVar3 + 1;
        if (iStack_1d8 <= iVar3)
          goto LAB_023df72b;
      }

      iVar5 = add_card_to_hand(player, chosen[iVar4]);
      add_state(player, iVar5, 0x10);
      put_into_play(player, iVar5);
      if (iVar4 < 3)
      {
        piVar6 = piStack_1d4 + iVar4;
        do
        {
          *piVar6 = piVar6[1];
          piVar6 = piVar6 + 1;
        } while (piVar6 != chosen + 3);
      }
      iVar2 = iVar2 + -1;
      iVar3 = iVar3 + 1;
      chosen[iVar2] = -1;
    } while (iVar3 < iStack_1d8);
  LAB_023df72b:
    if (0 < iVar2)
    {
      piVar6 = piStack_1d4 + iVar2;
      do
      {
        if (*piStack_1d4 != -1)
        {
          add_card_to_hand(player, *piStack_1d4);
        }
        piStack_1d4 = piStack_1d4 + 1;
      } while (piStack_1d4 != piVar6);
    }
  }
  kill_card(player, card, KILL_DESTROY);
LAB_023df46b:
  iVar2 = basic_spell(player, card, event);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23c96f0; original cards/strixhaven.c:343. */
int card_archmage_emeritus(int player, int card, event_t event)
{
  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  draw_a_card(player);
  return 0;
}

/* ManalinkEh 2022: 0x23cad20; original cards/strixhaven.c:870. */
int card_clever_lumimancer(int player, int card, event_t event)
{
  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  pump_until_eot_merge_previous_2022(player, card, player, card, 2, 2);
  return 0;
}

/* ManalinkEh 2022: 0x23cd8d0; original cards/strixhaven.c:1732. */
int card_dragonsguard_elite(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {

    instance = get_card_instance(player, card);
    result = count_counters(instance->parent_controller, instance->parent_card, COUNTER_P1_P1);
    add_counters_from_source(player, card, instance->parent_controller, instance->parent_card,
                             COUNTER_P1_P1, result);
  }
  result = generic_activated_ability(player, card, event, 0, 4, 0, 0, 2, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23cef00; original cards/strixhaven.c:2286. */
int card_extus_oriq_overlord(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  mdfc_front_suppress_charging_mana(player, card, event);
  check_legend_rule(player, card, event);
  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    new_default_test_definition(&this_test, 2, "Select target nonlegendary creature,");
    this_test.subtype = 0x2001;
    this_test.subtype_flag = 1;
    result = new_special_count_grave(player, &this_test);
    if (result != 0)
    {
      result = graveyard_has_shroud(player);
      if (result == 0)
      {
        new_global_tutor(player, player, 2, 1, 1, 1, &this_test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d1500; original cards/strixhaven.c:3290. */
int card_jadzi_oracle_of_arcavios(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  char buffer[100];

  mdfc_front(player, card, event);
  check_legend_rule(player, card, event);
  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {

    iVar1 = *deck_ptr[player];
    reveal_card_iid(player, card, iVar1);
    iVar2 = is_what(-1, iVar1, 1);
    if (iVar2 != 0)
    {
      put_into_play_a_card_from_deck(player, player, 0);
      return 0;
    }

    iVar2 = can_legally_play_iid(player, iVar1);
    if (iVar2 == 0)
    {
      return 0;
    }
    scnprintf(buffer, 100, "Cast %s", cards_ptr[cards_data[iVar1].id]->name);
    iVar1 = has_mana(player, 0, 1);
    iVar1 = dialog_impl(player, card, EVENT_SPA, "\001\002", "\001\003", "\001\014", "\001\016",
                        buffer, iVar1, 10, "Decline", 1, 1, 0, 0, 0, 0);
    if (iVar1 == 2)
    {
      return 0;
    }
    charge_mana(player, COLOR_COLORLESS, 1);
    if (spell_fizzled == 1)
    {
      return 0;
    }
    play_card_in_deck_for_free(player, player, 0);
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_023d1589:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_023d1592;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_023d1589;
  }

  pcVar3 = get_card_instance(player, card);
  pcVar4 = in_play(pcVar3->parent_controller, pcVar3->parent_card);
  if (pcVar4 != NULL)
  {
    bounce_permanent(pcVar3->parent_controller, pcVar3->parent_card);
  }
LAB_023d1592:
  iVar1 = generic_activated_ability(player, card, event, 0x20, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x23d2da0; original cards/strixhaven.c:3830. */
int card_leonin_lightscribe(int player, int card, event_t event)
{
  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  pump_creatures_until_eot_merge_pt(player, card, player, 1, 1, NULL);
  return 0;
}

/* ManalinkEh 2022: 0x23d3190; original cards/strixhaven.c:3912. */
int card_lorehold_apprentice(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {

    if (0 < active_cards_count[player])
    {
      result = 0;
      do
      {
        while ((((instance = in_play(player, result),
                  instance == NULL || (instance->internal_card_id != LEGACY_EFFECT_ACTIVATED)) ||
                 (instance->info_slot_fn != effect_flame_fusillade)) ||
                (instance->targets[2].player != 2)))
        {
          result = result + 1;
          if (active_cards_count[player] <= result)
            goto LAB_023d3240;
        }
        if (instance->targets[2].card == 0xbc)
        {
          return 0;
        }
        result = result + 1;
      } while (result < active_cards_count[player]);
    }
  LAB_023d3240:
    result = create_legacy_activate(player, card, effect_flame_fusillade);
    if (-1 < result)
    {

      instance = get_card_instance(player, result);
      instance->targets[2].player = 2;
      instance->targets[2].card = 0xbc;
      instance->eot_toughness = 0x300;
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d4000; original cards/strixhaven.c:4191. */
int card_mage_hunter(int player, int card, event_t event)
{
  magecraft_trigger(1 - player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  lose_life_2022(1 - player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x23d73c0; original cards/strixhaven.c:5155. */
int card_prismari_apprentice(int player, int card, event_t event)
{
  int result;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    pump_ability_until_eot_no_repeat_2022(player, card, player, card, 0, 0x10);
    result = get_cmc(trigger_cause_controller, trigger_cause);
    if (4 < result)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d7cd0; original cards/strixhaven.c:5336. */
int card_prismari_pledgemage(int player, int card, event_t event)
{
  int result;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    create_targetted_legacy_effect(player, card, effect_defender_can_attack_until_eot, player,
                                   card);
  }
  result = hybrid(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23d7d50; original cards/strixhaven.c:5351. */
int card_professor_onyx(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iStack_f4;
  test_definition_t test3;

  check_legend_rule(player, card, event);
  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    lose_life(1 - player, 2);
    gain_life_2022(player, 2);
    goto LAB_023d7e98;
  }
  if ((event == EVENT_ACTIVATE) || (event == EVENT_CAN_ACTIVATE))
  {
    iVar2 = (unsigned int)(6 < life[player]) * 5 + 5;
  LAB_023d7ed1:
    iVar2 = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                        "Lose 1 life and reval 3 cards", 1, iVar2, 1,
                        "Opp. sac the biggest creature", 1, 0x14, 0xfffffffd,
                        "Opp. discards or loses life", 1, 0x32, 0xfffffff8, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (iVar2 == 0)
      {
        return 0;
      }
      goto LAB_023d7e98;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_023d7e98;

    iVar2 = 5;
    if (life[player] < 7)
      goto LAB_023d7ed1;
    iVar2 = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                        "Lose 1 life and reval 3 cards", 1, 10, 1, "Opp. sac the biggest creature",
                        1, 0x14, 0xfffffffd, "Opp. discards or loses life", 1, 0x32, 0xfffffff8, 0,
                        0, 0, 0);
  }
  iVar1 = spell_fizzled;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (iVar2 == 1)
    {
      lose_life(player, 1);
      new_default_test_definition(&test3, 0xff, "Select a card to add to your hand.");
      reveal_top_cards_of_library_and_choose(player, card, player, 3, 1, TUTOR_HAND, 0, TUTOR_GRAVE,
                                             0, &test3);
    }
    else if (iVar2 == 2)
    {
      player_sacrifices_the_creature_with_highest_power(player, card, 1 - player);
    }
    else if (iVar2 == 3)
    {

      iStack_f4 = 0x15;
      iVar2 = 1 - player;
      while (hand_count[iVar2] != 0)
      {

        spell_fizzled = 0;
        discard(iVar2, 6, player);
        if ((spell_fizzled == 1) || (iStack_f4 = iStack_f4 + -3, iStack_f4 == 0))
          break;
      }
      spell_fizzled = iVar1;
      lose_life(iVar2, iStack_f4);
    }
  }
LAB_023d7e98:
  iVar2 = planeswalker(player, card, event, 5);
  return iVar2;
}

/* ManalinkEh 2022: 0x23d8170; original cards/strixhaven.c:5458. */
int card_quandrix_apprentice(int player, int card, event_t event)
{
  test_definition_t test3;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  new_default_test_definition(&test3, 1, "Select a land card to reveal.");
  reveal_top_cards_of_library_and_choose(player, card, player, 3, 0, TUTOR_HAND, 1,
                                         TUTOR_BOTTOM_OF_DECK, 0, &test3);
  return 0;
}

/* ManalinkEh 2022: 0x23d8e10; original cards/strixhaven.c:5688. */
int card_quandrix_pledgemage(int player, int card, event_t event)
{
  int result;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  }
  result = hybrid(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23da3d0; original cards/strixhaven.c:6155. */
int card_rowan_scholar_of_sparks_emblem(int player, int card, event_t event)
{
  int result;

  sp_ability_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    result = has_mana(player, 0, 2);
    if (result != 0)
    {
      charge_mana(player, COLOR_COLORLESS, 2);
      if (spell_fizzled != 1)
      {
        copy_spell_from_stack_2022(player, trigger_cause_controller, trigger_cause);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23da760; original cards/strixhaven.c:6286. */
int card_sedgemoor_witch(int player, int card, event_t event)
{
  ward_life(player, card, event, 3);
  menace(player, card, event);
  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  make_pest(player, card, 1);
  return 0;
}

/* ManalinkEh 2022: 0x23dba40; original cards/strixhaven.c:6787. */
int card_silverquill_apprentice(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    base_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "TARGET_CREATURE", 0, 0);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        pump_until_eot_merge_previous_2022(player, card, instance->targets[0].player,
                                      instance->targets[0].card, 1, 0);
        instance->number_of_targets = '\0';
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23dd010; original cards/strixhaven.c:7253. */
int card_storm_kiln_artist(int player, int card, event_t event)
{
  int result;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    if (((event == EVENT_POWER) && (card == affected_card)) && (player == affected_card_controller))
    {
      result = count_permanents_by_type(player, TYPE_ARTIFACT);
      event_result = event_result + result;
    }
    return 0;
  }
  generate_token_by_id_2022(player, card, 0x4998);
  return 0;
}

/* ManalinkEh 2022: 0x23dd240; original cards/strixhaven.c:7324. */
int card_symmetry_sage(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t td;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 0;
    iVar2 = can_target(&td);
    if (iVar2 != 0)
    {
      iVar2 = new_pick_target(&td, "TARGET_CREATURE", 0, 0);
      if (iVar2 != 0)
      {
        set_pt_and_abilities_until_eot(player, card, instance->targets[0].player,
                                       instance->targets[0].card, 2, -1, 0, 0, 8);
        iVar2 = instance->targets[0].player;
        if (iVar2 == player)
        {

          iVar3 = get_power(iVar2, instance->targets[0].card);
          iVar2 = instance->targets[0].player;
          ai_modifier = ai_modifier + ((iVar3 < 2) - 1 & 0xffffffa0) + 0x30;
        }
        if (1 - player == iVar2)
        {

          iVar2 = get_power(iVar2, instance->targets[0].card);
          ai_modifier = ai_modifier + ((2 < iVar2) - 1 & 0xffffffa0) + 0x30;
        }
        instance->number_of_targets = '\0';
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23dea60; original cards/strixhaven.c:7821. */
int card_nassari_dean_of_expression(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  mdfc_back(player, card, event);
  check_legend_rule(player, card, event);
  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    default_test_definition(&test, 0x7e);
    exile_top_x_card_you_may_play_them(player, card, 1, &test, 1 - player,
                                       MPCFE_UNTIL_EOT | MPCFE_FOR_CMC);
    if (trigger_condition != 0xd3)
    {
      return 0;
    }
  }
  else if (trigger_condition != 0xd3)
    goto LAB_023deabd;
  if (((card == affected_card) && (player == affected_card_controller)) &&
      (result = check_special_flags_2022(trigger_cause_controller, trigger_cause, -0x80000000),
       result != 0))
  {
    sp_ability_trigger(player, event, 2,
                       TYPE_ARTIFACT | TYPE_SPELL | TYPE_ENCHANTMENT | TYPE_CREATURE, NULL, player,
                       card);
  }
LAB_023deabd:
  if (event != EVENT_SPA)
  {
    return 0;
  }
  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  return 0;
}

/* ManalinkEh 2022: 0x23dfb40; original cards/strixhaven.c:8223. */
int card_wandering_archaic(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  mdfc_front(player, card, event);
  iVar2 = 1 - player;
  sp_ability_trigger(iVar2, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }

  iVar1 = has_mana(iVar2, 0, 2);
  if ((iVar1 != 0) && (iVar2 = charge_mana(iVar2, COLOR_COLORLESS, 2), iVar2 != 0))
  {
    return 0;
  }
  copy_spell_from_stack_2022(player, trigger_cause_controller, trigger_cause);
  return 0;
}

/* ManalinkEh 2022: 0x23dfdf0; original cards/strixhaven.c:8285. */
int card_witherbloom_apprentice(int player, int card, event_t event)
{
  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  lose_life(1 - player, 1);
  gain_life_2022(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x23e0810; original cards/strixhaven.c:8473. */
int card_witherbloom_pledgemage(int player, int card, event_t event)
{
  int result;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    gain_life_2022(player, 1);
  }
  result = hybrid(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23c8990; original cards/strixhaven.c:50. */
/* Compiler specialization: quantity is always one in this binary. */
static void make_one_spirit_strixhaven(int player, int card)
{
  token_generation_t token;
  default_token_definition(player, card, 0x498d, &token);
  token.pow = 3;
  token.tou = 2;
  token.color_forced = '0';
  token.qty = '\x01';
  generate_token(&token);
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23d0e40; original cards/strixhaven.c:3127. */
int card_illuminate_history(int player, int card, event_t event)
{
  int result;
  int card_00;
  test_definition_t test;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      ai_modifier = ai_modifier + 0x18;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {

    new_default_test_definition(&test, 0xff, "Select a card to discard.");
    if (hand_count[player] != 0)
    {
      result = 0;
      while (true)
      {

        card_00 = new_select_a_card(player, player, 4, 0, 3, -1, &test);
        if (card_00 < 0)
          break;
        result = result + 1;
        discard_card(player, card_00);
        if (((result == 3) && ((ai_is_speculating == 1 ||
                                ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))))) ||
            (hand_count[player] == 0))
          break;
      }
    }
    result = count_graveyard(player);
    if (6 < result)
    {
      make_one_spirit_strixhaven(player, card);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23d8ea0; original cards/strixhaven.c:5702. */
int card_quintorius_field_historian(int player, int card, event_t event)
{
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  boost_creature_type(player, card, event, SUBTYPE_SPIRIT, 1, 0, 0,
                      BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  if (event == EVENT_REMOVED_FROM_GRAVEYARD)
  {
    if (affected_card_controller == player)
    {
      instance = get_card_instance(player, card);
      if (instance->info_slot != 0x42)
      {
        make_one_spirit_strixhaven(player, card);
        instance = get_card_instance(player, card);
        instance->info_slot = 0x42;
      }
    }
  }
  else if (event == EVENT_STATIC_EFFECTS)
  {
    instance = get_card_instance(player, card);
    if (instance->info_slot == 0x42)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23dcf00; original cards/strixhaven.c:7182. */
int card_spirit_summoning(int player, int card, event_t event)
{
  int result;

  hybrid(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    make_one_spirit_strixhaven(player, card);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23c8b60; original cards/strixhaven.c:18. */
/* Compiler specialization: quantity is always one in this binary. */
static void make_one_fractal(int player, int card, int number_of_p1_p1_counters)
{
  token_generation_t token;
  default_token_definition(player, card, 0x588e, &token);
  token.color_forced = '\f';
  token.qty = '\x01';
  token.action = TOKEN_ACTION_ADD_COUNTERS;
  token.action_argument = number_of_p1_p1_counters & 0xffff;
  generate_token(&token);
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23ca340; original cards/strixhaven.c:610. */
int card_biomathematician(int player, int card, event_t event)
{
  test_definition_t this_test;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  make_one_fractal(player, card, 0);
  default_test_definition(&this_test, 0x47);
  this_test.subtype = 0x10a;
  new_manipulate_all(player, card, player, &this_test, ACT_ADD_COUNTERS_BASE | ACT_KILL_BURY);
  return 0;
}

/* ManalinkEh 2022: 0x23caa60; original cards/strixhaven.c:760. */
int card_body_of_research(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    result = count_deck(player);
    make_one_fractal(player, card, result);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23cfc80; original cards/strixhaven.c:2611. */
int card_fractal_summoning(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  modify_cost_for_hybrid_spells(player, card, event, 0);
  if (event == EVENT_CAN_CAST)
  {
    result = generic_spell(player, card, EVENT_CAN_CAST, 4, NULL, NULL, 1, NULL);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = hybrid_casting(player, card, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        charge_mana(player, COLOR_COLORLESS, -1);
        if (spell_fizzled != 1)
        {
          set_x_for_x_spells(player, card, EVENT_CAST_SPELL, x_value);
        }
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    instance = get_card_instance(player, card);
    make_one_fractal(player, card, instance->info_slot);
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d3120; original cards/strixhaven.c:3899. */
int card_leyline_invocation(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    result = count_subtype(player, 1, -1);
    make_one_fractal(player, card, result);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23d4080; original cards/strixhaven.c:4222. */
int card_manifestation_sage(int player, int card, event_t event)
{
  int result;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    make_one_fractal(player, card, hand_count[player]);
  }
  result = hybrid(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23c89e0; original cards/strixhaven.c:41. */
/* Concrete compiler clone: only EAX/EDX are ABI arguments. */
static void make_one_elemental_strixhaven(int player, int card)
{
  token_generation_t token;
  default_token_definition(player, card, 0x4926, &token);
  token.tou = 4;
  token.pow = 4;
  token.color_forced = '\x14';
  token.qty = '\x01';
  generate_token(&token);
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int ee_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23c8a30; original cards/strixhaven.c:1982. */
int ee_legacy(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;

  if ((((trigger_condition == 0xd4) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {

    pcVar3 = get_card_instance(player, card);
    iVar1 = pcVar3->targets[0].card;
    if ((trigger_cause == iVar1) &&
        (iVar2 = pcVar3->targets[0].player, trigger_cause_controller == iVar2))
    {
      if (event == EVENT_TRIGGER)
      {

        pcVar4 = get_card_instance(iVar2, iVar1);
        event_result = event_result | 2;
        pcVar4->kill_code = '\x04';
        pcVar3->damage_target_card = -1;
        pcVar3->damage_target_player = -1;
        return 0;
      }
      if (event == EVENT_RESOLVE_TRIGGER)
      {
        make_one_elemental_strixhaven(player, card);
        return 0;
      }
      if (event == EVENT_END_TRIGGER)
        goto LAB_023c8b00;
    }
  }
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }

  pcVar3 = get_card_instance(player, card);
  iVar1 = pcVar3->targets[0].player;
  iVar2 = pcVar3->targets[0].card;
  remove_special_flags_2022(iVar1, iVar2, 0x4000);
  remove_special_flags2_2022(iVar1, iVar2, 0x10000000);
LAB_023c8b00:
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x23ce370; original cards/strixhaven.c:2019. */
int card_elemental_expressionist(int player, int card, event_t event)
{
  int32_t iVar1;
  int iVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  target_definition_t td;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    iVar2 = can_target(&td);
    if (iVar2 != 0)
    {
      iVar2 = new_pick_target(&td, "Select target creature you control.", 0, 0x80);
      if (iVar2 != 0)
      {

        pcVar3 = get_card_instance(player, card);
        set_special_flags2_2022(pcVar3->targets[0].player, pcVar3->targets[0].card, 0x10000000);
        set_special_flags_2022(pcVar3->targets[0].player, pcVar3->targets[0].card, 0x4000);
        iVar2 = create_targetted_legacy_effect(player, card, ee_legacy, pcVar3->targets[0].player,
                                               pcVar3->targets[0].card);
        pcVar4 = get_card_instance(player, iVar2);
        iVar1 = pcVar3->targets[0].player;
        pcVar4->targets[0].card = pcVar3->targets[0].card;
        pcVar4->targets[0].player = iVar1;
        pcVar3->number_of_targets = '\0';
      }
    }
  }
  iVar2 = hybrid(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x23ce4d0; original cards/strixhaven.c:2053. */
int card_elemental_summoning(int player, int card, event_t event)
{
  int result;

  hybrid(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    make_one_elemental_strixhaven(player, card);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23d40e0; original cards/strixhaven.c:4236. */
int card_mascot_exhibition(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    make_inkling(player, card, 1, 0);
    make_one_spirit_strixhaven(player, card);
    make_one_elemental_strixhaven(player, card);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23d50f0; original cards/strixhaven.c:4529. */
int card_multiple_choice(int player, int card, event_t event)
{
  unsigned int uVar1;
  card_instance_t *instance;
  int result;
  unsigned int uVar4;
  unsigned int uStack_bc;
  target_definition_t td;
  target_definition_t td2;

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
        if (event != EVENT_RESOLVE_SPELL)
          goto LAB_023d51c8;

        uVar1 = instance->info_slot;
        if (uVar1 == 4)
        {
          uStack_bc = 2;
          uVar4 = 4;
        LAB_023d5232:
          scry(player, 1);
          draw_cards(player, 1);
        LAB_023d517b:
          if (uStack_bc != 0)
          {
            default_target_definition(player, card, &td, 0);
            td.zone = 0x1000;
            td.illegal_abilities = 0;
            result = new_pick_target(&td, "Select a player", 0, 0x80);
            if (result != 0)
            {
              default_target_definition(player, card, &td2, 2);
              td2.allow_cancel = 0;
              td2.illegal_abilities = 0;
              td2.who_chooses = instance->targets[0].player;
              td2.allowed_controller = td2.who_chooses;
              result = can_target(&td2);
              if ((result != 0) &&
                  (result = new_pick_target(&td2, "Select a creature you control.", 1, 0x80),
                   result != 0))
              {
                action_on_target(player, card, 1, ACT_BOUNCE);
              }
            }
          }
          if (uVar4 != 0)
            goto LAB_023d53a0;
        }
        else
        {
          if (uVar1 != 3)
          {
            uStack_bc = uVar1 & 2;
            uVar4 = uVar1 & 4;
            if ((uVar1 & 1) != 0)
              goto LAB_023d5232;
            goto LAB_023d517b;
          }
        LAB_023d53a0:
          make_one_elemental_strixhaven(player, card);
        }
        kill_card(player, card, KILL_DESTROY);
        goto LAB_023d51c8;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  get_card_instance(player, card);
LAB_023d51c8:
  result = generic_spell(player, card, event, 4, NULL, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23dac10; original cards/strixhaven.c:6382. */
int card_serpentine_curve(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  if (event == EVENT_RESOLVE_SPELL)
  {

    iVar1 = count_graveyard_by_type(player, 0x38);
    iVar2 = count_rfg_by_type(player, 0x38);
    make_one_fractal(player, card, iVar1 + 1 + iVar2);
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23d35a0; original cards/strixhaven.c:3998. */
int card_lorehold_command(int player, int card, event_t event)
{
  typedef enum
  {
    CHOICE_INVALID_TARGET = 0,
    CHOICE_SPIRIT = 1,
    CHOICE_P1_AND_INDESTRUCTIBLE = 2,
    CHOICE_DAMAGE_AND_GAIN = 3,
    CHOICE_SAC_AND_DRAW = 4
  } CommandChoices;

  CommandChoices CVar1;
  int result;
  unsigned int uVar3;
  CommandChoices *pCVar4;
  bool bVar5;
  uint32_t uStack_1b4;
  int iStack_1b0;
  int iStack_1ac;
  card_instance_t *instance;
  target_definition_t *ptStack_1a4;
  int iStack_1a0;
  target_definition_t *ptStack_19c;
  CommandChoices choices[2];
  target_definition_t td;
  target_definition_t td2;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    default_target_definition(player, card, &td2, 0);
    td2.zone = 0x1000;
    td2.preferred_controller = player;
    new_default_test_definition(&test, 0x47, "Select a permanent to sacrifice.");
    get_card_instance(player, card);
    result = basic_spell(player, card, EVENT_CAN_CAST);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_023d38a4;
  LAB_023d35d8:

    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    default_target_definition(player, card, &td2, 0);
    td2.zone = 0x1000;
    td2.preferred_controller = player;
    new_default_test_definition(&test, 0x47, "Select a permanent to sacrifice.");
    instance = get_card_instance(player, card);
    if (ai_is_speculating == 1)
    {
    LAB_023d3845:
      if (event == EVENT_RESOLVE_SPELL)
      {
      LAB_023d3695:
        ptStack_19c = &td;
        ptStack_1a4 = &td2;
        CVar1 = dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                            "Create a 3/2 Spirit", 0, 1, "Pump your creatures", 0, 1,
                            "Damage & gain life", 0, 1, "Sac & draw", 0, 1, 0, 0, 0, 0);
        pCVar4 = choices;
        bVar5 = false;
        choices[0] = CVar1;

        if (CVar1 == CHOICE_DAMAGE_AND_GAIN)
          goto LAB_023d37ce;
        do
        {
          if (CVar1 < CHOICE_DAMAGE_AND_GAIN || bVar5)
          {
            if (CVar1 == CHOICE_SPIRIT)
            {
              make_one_spirit_strixhaven(player, card);
            }
            else if (CVar1 == CHOICE_P1_AND_INDESTRUCTIBLE)
            {
              pump_creatures_until_eot(player, card, player, 0, 1, 0, 0, 0x800, NULL);
            }
          }
          else if (CVar1 == CHOICE_SAC_AND_DRAW)
          {
            player_sacrifices_a_permanent(player, card, player, 0x47, SAC_CAUSED | SAC_NO_CANCEL);
            draw_cards(player, 2);
          }
          while (true)
          {
            pCVar4 = pCVar4 + 1;
            if (pCVar4 == choices + 2)
            {
              kill_card(player, card, KILL_DESTROY);
              goto LAB_023d38a4;
            }
            CVar1 = *pCVar4;
            bVar5 = CVar1 == CHOICE_DAMAGE_AND_GAIN;
            if (!bVar5)
              break;
          LAB_023d37ce:
            result = valid_target(ptStack_19c);
            if (result != 0)
            {
              damage_target0(player, card, 3);
              result = validate_target(player, card, ptStack_1a4, 1);
              if (result != 0)
              {
                gain_life_2022(instance->targets[1].player, 3);
              }
            }
          }
        } while (true);
      }
      goto LAB_023d3993;
    }
  LAB_023d3683:
    if ((player == 1) && (((unsigned char)trace_mode & 2) == 0))
      goto LAB_023d3845;
    if (event == EVENT_RESOLVE_SPELL)
      goto LAB_023d3695;
    iStack_1b0 = 1;
    uStack_1b4 = 1;
    iStack_1ac = 1;
    iStack_1a0 = 1;
  }
  else
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      goto LAB_023d35d8;
    if (event != EVENT_CHANGE_TARGET)
      goto LAB_023d38a4;
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    default_target_definition(player, card, &td2, 0);
    td2.zone = 0x1000;
    td2.preferred_controller = player;
    new_default_test_definition(&test, 0x47, "Select a permanent to sacrifice.");
    instance = get_card_instance(player, card);
    if (ai_is_speculating != 1)
      goto LAB_023d3683;
  LAB_023d3993:

    result = count_subtype(1 - player, 2, -1);
    iStack_1a0 = result * 8;
    iStack_1ac = result * -8 + 0x20;
    if ((current_turn == player) && (current_phase != 0x18))
    {
      iStack_1a0 = 5;
    }
    uStack_1b4 = 0x14;
    iStack_1b0 = hand_count[player] * -10 + 0x46;
  }
  ptStack_19c = &td;
  ptStack_1a4 = &td2;
  result = generic_spell(player, card, EVENT_CAN_CAST, 1, ptStack_19c, NULL, 1, &test);
  uVar3 = 0;
  if (result != 0)
  {
    result = can_target(ptStack_1a4);
    uVar3 = (unsigned int)(result != 0);
  }
  CVar1 =
      dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                  "Create a 3/2 Spirit", 1, iStack_1ac, "Pump your creatures", 1, iStack_1a0,
                  "Damage & gain life", uVar3, uStack_1b4, "Sac & draw", 1, iStack_1b0, 0, 0, 0, 0);
  choices[0] = CVar1;
  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
      (player == affected_card_controller))
  {

    pCVar4 = choices;
    instance->number_of_targets = '\0';
    if (CVar1 == CHOICE_INVALID_TARGET)
      goto LAB_023d3bd1;
    do
    {
      if ((CVar1 == CHOICE_DAMAGE_AND_GAIN) &&
          (generic_spell(player, card, EVENT_CAST_SPELL, 0x81, ptStack_19c,
                         "Select target to damage.", 1, NULL),
           spell_fizzled != 1))
      {
        new_pick_target(ptStack_1a4, "Select target player to gain life.", 1, 0x81);
      }
      while (true)
      {
        pCVar4 = pCVar4 + 1;
        if (pCVar4 == choices + 2)
          goto LAB_023d38a4;
        CVar1 = *pCVar4;
        if (CVar1 != CHOICE_INVALID_TARGET)
          break;
      LAB_023d3bd1:
        spell_fizzled = 1;
      }
    } while (true);
  }
LAB_023d38a4:
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x23d7480; original cards/strixhaven.c:5179. */
int card_prismari_command(int player, int card, event_t event)
{
  typedef enum
  {
    CHOICE_INVALID_TARGET = 0,
    CHOICE_DAMAGE = 1,
    CHOICE_DRAW_DISCARD = 2,
    CHOICE_TREASURE = 3,
    CHOICE_KILL_ARTIFACT = 4
  } CommandChoices;

  card_instance_t *instance;
  CommandChoices CVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  const char *prompt;
  unsigned int target_number;
  CommandChoices *pCVar6;
  uint32_t uStack_17c;
  uint32_t uStack_178;
  uint32_t uStack_174;
  uint32_t uStack_170;
  CommandChoices choices[2];
  token_generation_t token;
  target_definition_t td;
  target_definition_t td2;
  target_definition_t td3;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    default_target_definition(player, card, &td2, 0);
    td2.zone = 0x1000;
    td2.preferred_controller = player;
    default_target_definition(player, card, &td3, 0x40);
    get_card_instance(player, card);
    iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    iVar4 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL);
    if (iVar4 == 0)
    {
      iVar4 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL);
      if (iVar4 == 0)
      {
        return 0;
      }
      if (iVar3 == 0)
      {
        return 0;
      }
    }
    else
    {
      generic_spell(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL);
    }
    iVar3 = basic_spell(player, card, EVENT_CAN_CAST);
    return iVar3;
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
  }
  else if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
           (event != EVENT_CHANGE_TARGET))
  {
    return 0;
  }

  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
  default_target_definition(player, card, &td2, 0);
  td2.zone = 0x1000;
  td2.preferred_controller = player;
  default_target_definition(player, card, &td3, 0x40);
  instance = get_card_instance(player, card);
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    if (event == EVENT_RESOLVE_SPELL)
      goto LAB_023d755a;
    uStack_17c = 0x14;
    uStack_178 = 10;
    uStack_174 = 0x1e;
    uStack_170 = 0x28;
  }
  else
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
    LAB_023d755a:
      CVar2 = dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                          "\001\033", "2 damage to any target", 0, 1, "Draw 2 & discard 2", 0, 1,
                          "Create a Treasure", 0, 1, "Destroy target artifact", 0, 1, 0, 0, 0, 0);

      pCVar6 = choices;
      target_number = 0;
      choices[0] = CVar2;
      do
      {
        switch (CVar2)
        {
        case CHOICE_INVALID_TARGET:
          break;
        case CHOICE_DAMAGE:
          iVar3 = validate_target(player, card, &td, target_number);
          if (iVar3 != 0)
          {
            damage_creature(instance->targets[target_number].player,
                            instance->targets[target_number].card, 2, player, card);
          }
          break;
        case CHOICE_DRAW_DISCARD:
          iVar3 = validate_target(player, card, &td2, target_number);
          if (iVar3 != 0)
          {
            draw_cards(instance->targets[target_number].player, 2);
            new_multidiscard(instance->targets[target_number].player, 2, 2, player);
          }
          break;
        case CHOICE_TREASURE:
          iVar3 = validate_target(player, card, &td2, target_number);
          if (iVar3 != 0)
          {
            default_token_definition(player, card, 0x4998, &token);
            token.t_player = instance->targets[target_number].player;
            generate_token(&token);
          }
          break;
        case CHOICE_KILL_ARTIFACT:
          iVar3 = validate_target(player, card, &td3, target_number);
          if (iVar3 != 0)
          {
            action_on_target(player, card, target_number, ACT_KILL_DESTROY);
          }
        default:
          goto LAB_023d7889;
        }
        target_number = target_number + 1;
      LAB_023d7889:
        pCVar6 = pCVar6 + 1;
        if (pCVar6 == choices + 2)
        {
          kill_card(player, card, KILL_DESTROY);
          return 0;
        }
        CVar2 = *pCVar6;
      } while (true);
    }
    uStack_17c = 1;
    uStack_178 = 1;
    uStack_174 = 1;
    uStack_170 = 1;
  }
  iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
  iVar4 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL);
  iVar5 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL);
  CVar2 = dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                      "\001\033", "2 damage to any target", iVar3, uStack_174, "Draw 2 & discard 2",
                      iVar4, uStack_170, "Create a Treasure", iVar4, uStack_178,
                      "Destroy target artifact", iVar5, uStack_17c, 0, 0, 0, 0);
  if (((event != EVENT_CAST_SPELL) || (card != affected_card)) ||
      (player != affected_card_controller))
  {

    return 0;
  }

  pCVar6 = choices;
  iVar3 = 0;
  instance->number_of_targets = '\0';
  choices[0] = CVar2;
  do
  {
    if (CVar2 < CHOICE_KILL_ARTIFACT)
    {
      if (CVar2 < CHOICE_DRAW_DISCARD)
      {
        if (CVar2 == CHOICE_INVALID_TARGET)
        {
          spell_fizzled = 1;
          goto LAB_023d7c39;
        }
        iVar4 = new_pick_target(&td, "Select a target to damage", iVar3, 0x81);
        if (iVar4 == 0)
        {
          return 0;
        }
      }
      else
      {
        if (CVar2 == CHOICE_DRAW_DISCARD)
        {
          prompt = "Select target player to draw & discard.";
        }
        else
        {
          prompt = "Select target player to create a treasure.";
        }
        iVar4 = new_pick_target(&td2, prompt, iVar3, 0x81);
        if (iVar4 == 0)
        {
          return 0;
        }
      }
    LAB_023d7c38:
      iVar3 = iVar3 + 1;
    }
    else if (CVar2 == CHOICE_KILL_ARTIFACT)
    {
      iVar4 = new_pick_target(&td3, "TARGET_ARTIFACT", iVar3, 1);
      if (iVar4 == 0)
      {
        return 0;
      }
      goto LAB_023d7c38;
    }
  LAB_023d7c39:
    pCVar6 = pCVar6 + 1;
    if (pCVar6 == choices + 2)
    {
      return 0;
    }
    CVar2 = *pCVar6;
  } while (true);
}

/* ManalinkEh 2022: 0x23dbb20; original cards/strixhaven.c:6815. */
int card_silverquill_command(int player, int card, event_t event)
{
  typedef enum
  {
    CHOICE_INVALID_TARGET = 0,
    CHOICE_PUMP = 1,
    CHOICE_REANIMATE = 2,
    CHOICE_DRAW1_LOSE1 = 3,
    CHOICE_OPP_SAC = 4
  } CommandChoices;

  CommandChoices CVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  CommandChoices *pCVar8;
  uint32_t uStack_1c4;
  uint32_t uStack_1c0;
  int iStack_1a8;
  card_instance_t *instance;
  target_definition_t *ptStack_1a0;
  target_definition_t *ptStack_19c;
  CommandChoices choices[2];
  target_definition_t td;
  target_definition_t td3;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    new_default_test_definition(&test, 2, "Select target creature card with MV 2 or less.");
    test.cmc = 3;
    test.cmc_flag = 3;
    default_target_definition(player, card, &td3, 0);
    td3.zone = 0x1000;
    get_card_instance(player, card);

    iVar7 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    iVar6 = generic_spell(player, card, EVENT_CAN_CAST, 8, NULL, NULL, 1, &test);
    iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL);
    iVar3 = would_validate_arbitrary_target(&td3, 1 - player, -1);
    return (unsigned int)(1 < (int)(((((unsigned int)(iVar7 != 0) - (unsigned int)(iVar6 == 0)) -
                                      (unsigned int)(iVar2 == 0)) +
                                     3) -
                                    (unsigned int)(iVar3 == 0)));
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
    {
      return 0;
    }
  LAB_023dbb59:

    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    new_default_test_definition(&test, 2, "Select target creature card with MV 2 or less.");
    test.cmc = 3;
    test.cmc_flag = 3;
    default_target_definition(player, card, &td3, 0);
    td3.zone = 0x1000;
    instance = get_card_instance(player, card);
    if (ai_is_speculating != 1)
    {
    LAB_023dbbfb:
      if ((player != 1) || (((unsigned char)trace_mode & 2) != 0))
      {
        if (event == EVENT_RESOLVE_SPELL)
          goto LAB_023dbc0e;
        iVar7 = 1;
        iStack_1a8 = 1 - player;
        iVar6 = 1;
        uStack_1c4 = 1;
        uStack_1c0 = 1;
        goto LAB_023dbed7;
      }
    }
    if (event == EVENT_RESOLVE_SPELL)
    {
    LAB_023dbc0e:
      ptStack_19c = &td3;
      ptStack_1a0 = &td;
      CVar1 = dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                          "\001\033", "Pump a creature", 0, 1, "Reanimate", 0, 1,
                          "Draw & lose 1 life", 0, 1, "Opponent sacrifices", 0, 1, 0, 0, 0, 0);

      pCVar8 = choices;
      iVar7 = 0;
      choices[0] = CVar1;
      do
      {
        switch (CVar1)
        {
        case CHOICE_INVALID_TARGET:
          break;
        case CHOICE_PUMP:
          iVar6 = validate_target(player, card, ptStack_1a0, iVar7);
          if (iVar6 != 0)
          {
            pump_ability_until_eot_2022(player, card, instance->targets[iVar7].player,
                                   instance->targets[iVar7].card, 3, 3, 0x20, 0);
          }
          break;
        case CHOICE_REANIMATE:
          iVar6 = validate_target_from_grave_source(player, card, player, iVar7);
          if (iVar6 != -1)
          {
            reanimate_permanent(player, card, player, iVar6, REANIMATE_DEFAULT);
          }
          break;
        case CHOICE_DRAW1_LOSE1:
          iVar6 = validate_target(player, card, ptStack_19c, iVar7);
          if (iVar6 != 0)
          {
            draw_cards(instance->targets[iVar7].player, 1);
            lose_life(instance->targets[iVar7].player, 1);
          }
          break;
        case CHOICE_OPP_SAC:
          iVar6 = validate_target(player, card, ptStack_19c, iVar7);
          if (iVar6 != 0)
          {
            player_sacrifices_a_permanent(player, card, instance->targets[iVar7].player, 2,
                                          SAC_CAUSED);
          }
        default:
          goto LAB_023dbd51;
        }
        iVar7 = iVar7 + 1;
      LAB_023dbd51:
        pCVar8 = pCVar8 + 1;
        if (pCVar8 == choices + 2)
        {
          kill_card(player, card, KILL_DESTROY);
          return 0;
        }
        CVar1 = *pCVar8;
      } while (true);
    }
  }
  else
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      goto LAB_023dbb59;
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    new_default_test_definition(&test, 2, "Select target creature card with MV 2 or less.");
    test.cmc = 3;
    test.cmc_flag = 3;
    default_target_definition(player, card, &td3, 0);
    td3.zone = 0x1000;
    instance = get_card_instance(player, card);
    if (ai_is_speculating != 1)
      goto LAB_023dbbfb;
  }
  iStack_1a8 = 1 - player;
  iVar6 = ((5 < life[player]) - 1 & 0xffffffdd) + 0x28;
  iVar7 = count_subtype(iStack_1a8, 2, -1);
  uStack_1c4 = 10;
  uStack_1c0 = 0x14;
  iVar7 = (-(unsigned int)(iVar7 == 0) & 0xffffffe3) + 0x1e;
LAB_023dbed7:
  ptStack_19c = &td3;
  ptStack_1a0 = &td;
  iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, ptStack_1a0, NULL, 1, NULL);
  iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 8, NULL, NULL, 1, &test);
  iVar4 = generic_spell(player, card, EVENT_CAN_CAST, 1, ptStack_19c, NULL, 1, NULL);
  iVar5 = would_validate_arbitrary_target(ptStack_19c, iStack_1a8, -1);
  CVar1 = dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                      "\001\033", "Pump a creature", iVar2, uStack_1c4, "Reanimate", iVar3,
                      uStack_1c0, "Draw & lose 1 life", iVar4, iVar6, "Opponent sacrifices", iVar5,
                      iVar7, 0, 0, 0, 0);
  if (((event != EVENT_CAST_SPELL) || (card != affected_card)) ||
      (player != affected_card_controller))
  {

    return 0;
  }
  instance->number_of_targets = '\0';
  pCVar8 = choices;
  iVar7 = 0;
  choices[0] = CVar1;
  do
  {
    switch (CVar1)
    {
    case CHOICE_INVALID_TARGET:
      spell_fizzled = 1;
      break;
    case CHOICE_PUMP:
      iVar6 = new_pick_target(ptStack_1a0, "Select target creature to get +3/+3 and flying.", iVar7,
                              0x81);
      if (iVar6 == 0)
      {
        return 0;
      }
      iVar7 = iVar7 + 1;
      break;
    case CHOICE_REANIMATE:
      select_target_from_grave_source(player, card, player, SFG_0, 2, &test, iVar7);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      iVar7 = iVar7 + 1;
      break;
    case CHOICE_DRAW1_LOSE1:
      if ((life[iStack_1a8] != 1) ||
          (iVar6 = would_validate_arbitrary_target(ptStack_19c, iStack_1a8, -1), iVar6 == 0))
      {
        td3.preferred_controller = player;
      }
      iVar6 = new_pick_target(ptStack_19c, "Select target player to draw a card and lose 1 life.",
                              iVar7, 0x81);
      if (iVar6 == 0)
      {
        return 0;
      }
      iVar7 = iVar7 + 1;
      break;
    case CHOICE_OPP_SAC:
      instance->targets[iVar7].player = iStack_1a8;
      instance->targets[iVar7].card = -1;
      instance->number_of_targets = instance->number_of_targets + '\x01';
      iVar7 = iVar7 + 1;
    }
    pCVar8 = pCVar8 + 1;
    if (pCVar8 == choices + 2)
    {
      return 0;
    }
    CVar1 = *pCVar8;
  } while (true);
}

/* ManalinkEh 2022: 0x23dfe70; original cards/strixhaven.c:8307. */
int card_witherbloom_command(int player, int card, event_t event)
{
  typedef enum
  {
    CHOICE_INVALID_TARGET = 0,
    CHOICE_MILL = 1,
    CHOICE_KIL = 2,
    CHOICE_PUMP = 3,
    CHOICE_OPP_LOSE2 = 4
  } CommandChoices;

  CommandChoices CVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  CommandChoices *pCVar6;
  unsigned int target_number;
  uint32_t uStack_21c;
  uint32_t uStack_218;
  uint32_t uStack_214;
  uint32_t uStack_210;
  int iStack_200;
  card_instance_t *instance;
  target_definition_t *ptStack_1f8;
  target_definition_t *ptStack_1f4;
  target_definition_t *ptStack_1f0;
  CommandChoices choices[2];
  target_definition_t td;
  target_definition_t td2;
  target_definition_t td3;
  test_definition_t this_test;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    default_target_definition(player, card, &td2, 0x47);
    td2.illegal_type = 3;
    td2.extra = 2;
    td2.special = 0x800;
    default_target_definition(player, card, &td3, 2);
    td3.preferred_controller = 1 - player;
    get_card_instance(player, card);

    iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL);
    iVar4 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL);
    iVar5 = opponent_is_valid_target(player, card);
    return (unsigned int)(1 < (int)(((((unsigned int)(iVar2 != 0) - (unsigned int)(iVar3 == 0)) -
                                      (unsigned int)(iVar4 == 0)) +
                                     3) -
                                    (unsigned int)(iVar5 == 0)));
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card != affected_card) || (player != affected_card_controller))
    {
      return 0;
    }
  LAB_023e0190:
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    default_target_definition(player, card, &td2, 0x47);
    td2.illegal_type = 3;
    td2.extra = 2;
    td2.special = 0x800;
    default_target_definition(player, card, &td3, 2);
    iStack_200 = 1 - player;
    td3.preferred_controller = iStack_200;
    instance = get_card_instance(player, card);
    if (ai_is_speculating != 1)
    {
    LAB_023dff6e:
      if ((player != 1) || (((unsigned char)trace_mode & 2) != 0))
      {
        if (event == EVENT_RESOLVE_SPELL)
          goto LAB_023dff80;
        uStack_21c = 1;
        uStack_218 = 1;
        uStack_214 = 1;
        uStack_210 = 1;
        goto LAB_023e027e;
      }
      goto LAB_023e0175;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      goto LAB_023e0190;
    }

    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    default_target_definition(player, card, &td2, 0x47);
    td2.illegal_type = 3;
    td2.extra = 2;
    td2.special = 0x800;
    default_target_definition(player, card, &td3, 2);
    iStack_200 = 1 - player;
    td3.preferred_controller = iStack_200;
    instance = get_card_instance(player, card);
    if (ai_is_speculating != 1)
      goto LAB_023dff6e;
  LAB_023e0175:
    if (event == EVENT_RESOLVE_SPELL)
    {
    LAB_023dff80:
      ptStack_1f0 = &td;
      ptStack_1f4 = &td3;
      ptStack_1f8 = &td2;
      CVar1 = dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                          "Mill 3 & recycle land", 0, 1, "Destroy a permanent", 0, 1,
                          "Shrink creature", 0, 1, "Drain 2 life", 0, 1, 0, 0, 0, 0);

      pCVar6 = choices;
      target_number = 0;
      choices[0] = CVar1;
      do
      {
        switch (CVar1)
        {
        case CHOICE_INVALID_TARGET:
          break;
        case CHOICE_MILL:
          iVar2 = validate_target(player, card, ptStack_1f0, target_number);
          if (iVar2 != 0)
          {
            mill(instance->targets[target_number].player, 3);
            new_default_test_definition(&this_test, 1, "Select a land card.");
            iVar2 = new_special_count_grave(player, &this_test);
            if (iVar2 != 0)
            {
              new_global_tutor(player, player, 2, 1, 0, 1, &this_test);
            }
          }
          break;
        case CHOICE_KIL:
          iVar2 = validate_target(player, card, ptStack_1f8, target_number);
          if (iVar2 != 0)
          {
            action_on_target(player, card, target_number, ACT_KILL_DESTROY);
          }
          break;
        case CHOICE_PUMP:
          iVar2 = validate_target(player, card, ptStack_1f4, target_number);
          if (iVar2 != 0)
          {
            pump_until_eot_2022(player, card, instance->targets[target_number].player,
                           instance->targets[target_number].card, -3, -1);
          }
          break;
        case CHOICE_OPP_LOSE2:
          td.allowed_controller = iStack_200;
          iVar2 = validate_target(player, card, ptStack_1f0, target_number);
          if (iVar2 != 0)
          {
            lose_life(instance->targets[target_number].player, 2);
            gain_life(player, 2);
          }
        default:
          goto LAB_023e0109;
        }
        target_number = target_number + 1;
      LAB_023e0109:
        pCVar6 = pCVar6 + 1;
        if (pCVar6 == choices + 2)
        {
          kill_card(player, card, KILL_DESTROY);
          return 0;
        }
        CVar1 = *pCVar6;
      } while (true);
    }
  }
  uStack_21c = 5;
  uStack_218 = 0x14;
  uStack_214 = 10;
  uStack_210 = 0x1e;
LAB_023e027e:
  ptStack_1f0 = &td;
  ptStack_1f4 = &td3;
  ptStack_1f8 = &td2;
  iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, ptStack_1f0, NULL, 1, NULL);
  iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, ptStack_1f8, NULL, 1, NULL);
  iVar4 = generic_spell(player, card, EVENT_CAN_CAST, 1, ptStack_1f4, NULL, 1, NULL);
  iVar5 = opponent_is_valid_target(player, card);
  CVar1 = dialog_impl(player, card, event, "\001\005", choices + 1, "\001\015", "\001\002",
                      "Mill 3 & recycle land", iVar2, uStack_214, "Destroy a permanent", iVar3,
                      uStack_210, "Shrink creature", iVar4, uStack_218, "Drain 2 life", iVar5,
                      uStack_21c, 0, 0, 0, 0);
  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    pCVar6 = choices;
    iVar2 = 0;
    instance->number_of_targets = '\0';
    choices[0] = CVar1;
    while (true)
    {
      switch (CVar1)
      {
      case CHOICE_INVALID_TARGET:
        spell_fizzled = 1;
        break;
      case CHOICE_MILL:
        iVar3 = new_pick_target(ptStack_1f0, "Select target player to mill 3.", iVar2, 0x81);
        if (iVar3 == 0)
        {
          return 0;
        }
        iVar2 = iVar2 + 1;
        break;
      case CHOICE_KIL:
        iVar3 = new_pick_target(
            ptStack_1f8, "Select target noncreature, nonland permanent with mana value 2 or less",
            iVar2, 0x81);
        if (iVar3 == 0)
        {
          return 0;
        }
        iVar2 = iVar2 + 1;
        break;
      case CHOICE_PUMP:
        iVar3 = new_pick_target(ptStack_1f4, "Select target creature to get -3/-1.", iVar2, 0x81);
        if (iVar3 == 0)
        {
          return 0;
        }
        iVar2 = iVar2 + 1;
        break;
      case CHOICE_OPP_LOSE2:
        instance->targets[iVar2].player = iStack_200;
        instance->targets[iVar2].card = -1;
        instance->number_of_targets = instance->number_of_targets + '\x01';
        iVar2 = iVar2 + 1;
      }
      pCVar6 = pCVar6 + 1;
      if (pCVar6 == choices + 2)
        break;
      CVar1 = *pCVar6;
    }
    return 0;
  }

  return 0;
}

/* ManalinkEh 2022: 0x23c7a80; original cards/strixhaven.c:3659. */
static void create_exile_with_study_counter_legacy(int player, int card, int iid, int owner)
{
  unsigned int uVar1;
  int result;
  card_instance_t *instance;

  result = create_legacy_effect(player, card, card_exiled_with_study_counter);
  if (result != -1)
  {
    add_counters_from_source(player, card, player, result, COUNTER_STUDY, 1);
    instance = get_card_instance(player, result);
    uVar1 = cards_data[iid].id;
    instance->targets[0].player = owner;
    result = instance->damage_source_card;
    instance->display_pic_csv_id = (uint16_t)uVar1;
    instance->targets[0].card = iid;
    instance->eot_toughness = 0x300;
    result = get_card_image_number(uVar1 & 0xffff, (int)instance->damage_source_player, result);
    instance->display_pic_num = (uint16_t)result;
  }
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23d26a0; original cards/strixhaven.c:3690. */
int card_kianne_dean_of_substance(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;

  check_legend_rule(player, card, event);
  mdfc_front(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if ((iVar2 == 0) && (iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 4,
                                                             0, 0, 1, 0, 0, 0, NULL, NULL),
                           iVar2 == 0))
      {
        return 0;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      iVar2 = count_different_mana_values_among_card_exiled_with_study_counter(player);
      instance->info_slot = 0;
      instance->number_of_targets = '\0';
      iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 4, 0, 0, 1, 0, 0, 0,
                                        NULL, NULL);
      iVar4 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\033", "\001\016",
                          "Tap and exile top card", iVar4, 0x14, "Create a Fractal", iVar3,
                          iVar2 * 10, 0, 0, 0, 0);
      if (iVar4 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar4 == 1)
      {
        generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
        if (spell_fizzled == 1)
        {
          return 0;
        }
        ai_modifier = ai_modifier + 0x18;
        return 0;
      }
      if (iVar4 != 2)
      {
        return 0;
      }
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 4, 0, 0, 1, 0, 0, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      ai_modifier = ai_modifier + iVar2 * 10;
      return 0;
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
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (instance->mode == '\x01')
    {

      iVar2 = *deck_ptr[player];
      if (iVar2 != -1)
      {
        iVar4 = is_what(-1, iVar2, 1);
        if (iVar4 == 0)
        {
          create_exile_with_study_counter_legacy(player, card, iVar2, player);
          rfg_top_card_of_deck(player);
        }
        else
        {
          put_into_play_a_card_from_deck(player, player, 0);
        }
      }
    }
    else if (instance->mode == '\x02')
    {
      iVar2 = count_different_mana_values_among_card_exiled_with_study_counter(player);
      make_one_fractal(player, card, iVar2);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d2b40; original cards/strixhaven.c:3759. */
int card_imbraham_dean_of_theory(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  CardFunction p_Var3;
  int iVar4;
  int iStack_348;
  test_definition_t test;
  int iids[151];

  mdfc_back(player, card, event);
  check_legend_rule(player, card, event);
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      instance = get_card_instance(player, card);
      p_Var3 = instance->info_slot_fn;
      instance->info_slot = 0;
      while ((p_Var3 != (CardFunction)0x0 && (*deck_ptr[player] != -1)))
      {

        create_exile_with_study_counter_legacy(player, card, *deck_ptr[player], player);
        rfg_top_card_of_deck(player);
        p_Var3 = p_Var3 + -1;
      }
      iVar4 = 0;
      iStack_348 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          instance = in_play(player, iVar4);
          if ((((instance != NULL) && (iVar2 = is_what(player, iVar4, 0x80), iVar2 != 0)) &&
               (instance->info_slot_fn == card_exiled_with_study_counter)) &&
              (iVar2 = count_counters(player, iVar4, COUNTER_STUDY), iVar2 != 0))
          {
            iids[iStack_348] = instance->targets[0].card;
            iStack_348 = iStack_348 + 1;
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[player]);
      }
      new_default_test_definition(&test, 0xff, "Select a card to put into your hand.");
      iVar4 = select_card_from_zone(player, player, iids, iStack_348, 0, 1, -1, &test);
      if (iVar4 == -1)
      {
        return 0;
      }
      iVar4 = iids[iVar4];
      iVar2 = find_iid_in_rfg(player, iVar4);
      if (-1 < iVar2)
      {
        add_card_to_hand(player, iVar4);
        remove_card_from_rfg_by_position(player, iVar2);
      }
    }
  }
  iVar4 = generic_activated_ability(player, card, event, 1, -1, 0, 2, 0, 0, 0, 0, NULL, NULL);
  return iVar4;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23cb090; original cards/strixhaven.c:975. */
int card_confront_the_past(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  target_definition_t td;
  char buffer[100];
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x100);
    td.allowed_controller = 1 - player;
    get_card_instance(player, card);
    iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    if (iVar3 != 0)
    {
      return 1;
    }
    iVar3 = graveyard_has_shroud(player);

    if ((iVar3 == 0) && (iVar3 = *graveyard_ptr_mutable[player], iVar3 != -1))
    {
      iVar4 = 4;
      do
      {
        iVar2 = is_what(-1, iVar3, 0x100);
        if (iVar2 != 0)
        {
          iVar3 = get_cmc_by_internal_id(iVar3);
          iVar3 = has_mana(player, 0, iVar3);
          if (iVar3 != 0)
          {
            return 1;
          }
        }
        iVar3 = *(int *)((int)graveyard_ptr_mutable[player] + iVar4);
        iVar4 = iVar4 + 4;
      } while (iVar3 != -1);
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x100);
      td.allowed_controller = 1 - player;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        iVar3 = check_special_flags2_2022(player, card, -0x80000000);
        if (iVar3 == 0)
        {

          instance->number_of_targets = '\0';
          instance->info_slot = 0;
          instance->targets[1].player = 0;
          generic_spell(player, card, EVENT_CAST_SPELL, 4, NULL, NULL, 1, NULL);
          iVar4 = graveyard_has_shroud(player);
          iVar3 = 0;
          if (iVar4 == 0)
          {
            default_test_definition(&test, 0x100);
            test.cmc = instance->info_slot + 1;
            test.cmc_flag = 3;
            iVar3 = new_special_count_grave(player, &test);
          }
          iVar4 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
          iVar3 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\016", "\001\033",
                              "Reanimate a Planewalke with MV X or less", iVar3, 5,
                              "Remove 2*X loyalty from an oppoent's planeswalker", iVar4, 10, 0, 0,
                              0, 0);
          if (iVar3 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
        }
        if (instance->mode == '\x01')
        {
          scnprintf(buffer, 100, "Select a planeswalker card with mana value %d or less.",
                    instance->info_slot);
          new_default_test_definition(&test, 0x100, buffer);
          test.cmc = instance->info_slot + 1;
          test.cmc_flag = 3;
          iVar3 = generic_spell(player, card, EVENT_CAST_SPELL, 8, NULL, NULL, 1, &test);
          return iVar3;
        }
        if (instance->mode == '\x02')
        {
          iVar3 = generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                                "Select target planeswalker an opponent controls.", 1, NULL);
          return iVar3;
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL || event == EVENT_CAN_CHANGE_TARGET) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x100);
    td.allowed_controller = 1 - player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      if (instance->mode == '\x01')
      {

        iVar3 = validate_target_from_grave_source(player, card, player, 0);
        if (iVar3 != -1)
        {
          reanimate_permanent(player, card, player, iVar3, REANIMATE_DEFAULT);
        }
      }
      else if ((instance->mode == '\x02') && (iVar3 = valid_target(&td), iVar3 != 0))
      {
        remove_counters_from_source(player, card, instance->targets[0].player,
                                    instance->targets[0].card, COUNTER_LOYALTY,
                                    instance->info_slot * 2);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d1ab0; original cards/strixhaven.c:3456. */
int card_kasmina_enigma_sage(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int clr;
  const char *pcVar4;
  target_definition_t td;
  char buffer[100];
  char buffer3[100];
  char buffer2[100];
  test_definition_t this_test;

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
                instance->info_slot_fn == kasmina_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_023d1d1c;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_activate(player, card, kasmina_effect, player, card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
    }
  LAB_023d1d1c:
    check_legend_rule(player, card, EVENT_STATIC_EFFECTS);
  }
  else
  {
    check_legend_rule(player, card, event);
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      instance = get_card_instance(player, card);
      default_target_definition(player, card, &td, 2);
      iVar2 = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033", "Scry 2", 1, 10,
                          2, "Create a Fractal", 1, 0x14, 0,
                          "Exile a spell from your deck and you can cast it", 1, 0x32, 0xfffffff8,
                          0, 0, 0, 0);
      if (event == EVENT_CAN_ACTIVATE)
      {
        if (iVar2 == 0)
        {
          return 0;
        }
      }
      else
      {
        instance->number_of_targets = '\0';
        if (iVar2 == 2)
        {

          iVar2 = count_counters(player, card, COUNTER_LOYALTY);
          if (iVar2 < 1)
          {
            iVar2 = 1;
          }
          if ((ai_is_speculating != 1) && ((player != 1 || (((unsigned char)trace_mode & 2) != 0))))
          {
            do
            {
              do
              {
                iVar2 = choose_a_number(player, "How many Loyalty you\'ll remove?", 1);
              } while (iVar2 < 0);
              iVar3 = count_counters(player, card, COUNTER_LOYALTY);
            } while (iVar3 < iVar2);
          }
          remove_counters_from_source(player, card, player, card, COUNTER_LOYALTY, iVar2);
          instance->targets[1].player = iVar2;
        }
      }
    }
    else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {

      instance = get_card_instance(player, card);
      default_target_definition(player, card, &td, 2);
      iVar2 = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033", "Scry 2", 1, 10,
                          2, "Create a Fractal", 1, 0x14, 0,
                          "Exile a spell from your deck and you can cast it", 1, 0x32, 0xfffffff8,
                          0, 0, 0, 0);
      if (iVar2 == 1)
      {
        scry(player, 2);
      }
      else if (iVar2 == 2)
      {
        make_one_fractal(player, card, instance->targets[1].player);
      }
      else if (iVar2 == 3)
      {

        clr = get_color(instance->parent_controller, instance->parent_card);
        pcVar4 = get_color_text(player, buffer3, clr);
        scnprintf(buffer, 100, "Select a %s instant or sorcery card.", pcVar4);
        new_default_test_definition(&this_test, 0x38, buffer);
        this_test.color = clr;
        iVar2 = new_select_a_card(player, player, 1, 0, 2, -1, &this_test);
        if (-1 < iVar2)
        {

          iVar3 = deck_ptr[player][iVar2];
          rfg_card_in_deck(player, iVar2);
          iVar2 = can_legally_play_iid(player, iVar3);
          if (iVar2 != 0)
          {

            scnprintf(buffer2, 100, "Play %s", cards_ptr[cards_data[iVar3].id]->name);
            iVar2 = dialog_impl(player, card, event, "\001\002", "\001\003", "\001\014", buffer2, 1,
                                0x14, "Decline", 1, 10, 0, 0, 0, 0);
            if (iVar2 == 1)
            {
              play_card_in_exile_for_free(player, player, (unsigned int)cards_data[iVar3].id);
            }
          }
        }
      }
    }
  }
  iVar2 = planeswalker(player, card, event, 2);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23cdd70; original cards/strixhaven.c:1876. */
int card_ecological_appreciation(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  int zone_count;
  int *piStack_12c;
  int *piStack_128;
  int iStack_11c;
  int chosen[4];
  int cards[4];
  int iids[4];
  test_definition_t this_test;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
      set_special_flags_2022(player, card, 0x20000);
      iVar2 = check_special_flags2_2022(player, card, -0x80000000);
      if (iVar2 != 0)
      {
        return 0;
      }
      charge_mana(player, COLOR_COLORLESS, -1);
      if (spell_fizzled != 1)
      {
        set_x_for_x_spells(player, card, EVENT_CAST_SPELL, x_value);
        return 0;
      }
      return 0;
    }
    if (event == EVENT_RESOLVE_SPELL)
    {

      zone_count = 0;
      instance = get_card_instance(player, card);
      iVar2 = instance->info_slot;
      default_test_definition(&this_test, 2);
      this_test.cmc = iVar2 + 1;
      this_test.no_shuffle = 1;
      this_test.value_for_special_selection_function = (int)chosen;
      this_test.cmc_flag = 3;
      this_test.special_selection_function = test_not_in_array;
      chosen[0] = -1;
      chosen[1] = -1;
      chosen[2] = -1;
      chosen[3] = -1;
      cards[0] = -1;
      cards[1] = -1;
      cards[2] = -1;
      cards[3] = -1;
      iids[0] = -1;
      iids[1] = -1;
      iids[2] = -1;
      iids[3] = -1;
      do
      {

        if ((ai_is_speculating == 1) || ((player == 1 && ((trace_mode & 2U) == 0))))
        {
          this_test.message[0] = '\0';
        }
        else
        {
          sprintf(this_test.message, "Select a creature card with CMC %d or less (%d of 4)", iVar2,
                  zone_count + 1);
        }
        iVar4 = new_global_tutor(player, player, 1, 1, 0, 2, &this_test);
        if ((iVar4 < 0) &&
            ((iVar4 = new_special_count_grave(player, &this_test),
              iVar4 == 0 ||
                  (iVar4 = new_global_tutor(player, player, 2, 1, 0, 2, &this_test), iVar4 < 0))))
        {
          iStack_11c = zone_count;
          if (2 < zone_count)
          {
            iStack_11c = 2;
          }
          default_test_definition(&this_test, 0xff);
          this_test.id = 0x388;
          this_test.id_flag = 1;
          this_test.message[0] = '\0';
          if (0 < zone_count)
            goto LAB_023ce071;
          shuffle(player);
          goto LAB_023ce17e;
        }
        iVar5 = get_original_internal_card_id(player, iVar4);
        chosen[zone_count] = iVar5;
        add_state(player, iVar4, 0x20);
        cards[zone_count] = iVar4;
        iVar4 = get_original_internal_card_id(player, iVar4);
        iids[zone_count] = iVar4;
        zone_count = zone_count + 1;
      } while (zone_count != 4);

      default_test_definition(&this_test, 0xff);
      iStack_11c = 2;
    LAB_023ce071:
      piStack_128 = iids;
      piStack_12c = cards;
      this_test.id_flag = 1;
      this_test.id = 0x388;
      this_test.message[0] = '\0';
      iVar2 = 0;
      do
      {

        iVar2 = iVar2 + 1;
        if ((ai_is_speculating != 1) && ((trace_mode & 2U) != 0 || player != 0))
        {
          sprintf(this_test.message, "Select a creature card (%d of %d)", iVar2, iStack_11c);
        }
        iVar4 =
            select_card_from_zone(1 - player, player, piStack_128, zone_count, 1, 2, 0, &this_test);
        if (-1 < iVar4)
        {
          iVar5 = get_internal_card_id_from_csv_id(0x388);
          iids[iVar4] = iVar5;
          put_on_top_of_deck(player, cards[iVar4]);
          cards[iVar4] = -1;
        }
      } while (iVar2 < iStack_11c);
      shuffle(player);
      piVar1 = piStack_12c + zone_count;
      do
      {
        if (*piStack_12c != -1)
        {
          put_into_play(player, *piStack_12c);
        }
        piStack_12c = piStack_12c + 1;
      } while (piStack_12c != piVar1);
    LAB_023ce17e:
      kill_card(player, card, KILL_EXILE);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  iVar2 = generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23dc500; original cards/strixhaven.c:6979. */
int card_silverquill_pledgemage(int player, int card, event_t event)
{
  int result;
  unsigned int argument7;
  int ability;
  int sp_ability;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {

    argument7 = 1;
    if (6 < life[player])
    {
      result = check_for_ability(player, card, 0x20);
      argument7 = (unsigned int)(result != 0);
    }
    result = do_dialog(player, player, card, -1, -1, " This gains flying\n This gains lifelink",
                       argument7);
    if (result == 0)
    {
      sp_ability = 0;
      ability = 0x20;
    }
    else
    {
      ability = 0;
      sp_ability = (unsigned int)(result == 1) * 2;
    }
    pump_ability_until_eot_no_repeat_alt(player, card, player, card, ability, sp_ability,
                                         result + 1);
  }
  result = hybrid(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static void add_hofry_legacy(token_generation_2022_t *token, int card_added, int number);
int rushed_rebirth_legacy(int player, int card, event_t event);
static int hofri_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23c8600; original cards/strixhaven.c:2865. */
static int hofri_legacy(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  int position_in_exile;

  result = attached_leaves_play(player, card, event, 1, 0);
  if (result == 0)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  result = instance->targets[1].player;
  position_in_exile = find_iid_in_rfg(result, instance->targets[1].card);
  if (-1 < position_in_exile)
  {
    from_exile_to_graveyard(result, position_in_exile);
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x23c76d0; original cards/strixhaven.c:2877. */
static void add_hofry_legacy(token_generation_2022_t *token, int card_added, int number)
{
  unsigned int uVar1;
  int card;
  card_instance_t *instance;

  card = create_targetted_legacy_effect(token->s_player, token->s_card, hofri_legacy,
                                        token->t_player, card_added);
  instance = get_card_instance(token->s_player, card);
  uVar1 = token->action_argument;
  instance->targets[1].player = uVar1 & 0xf;
  instance->targets[1].card = (int)uVar1 >> 4;
  add_a_subtype_permanently(token->s_player, token->s_card, token->t_player, card_added,
                            SUBTYPE_SPIRIT);
  return;
}

/* ManalinkEh 2022: 0x23c6dc0; original cards/strixhaven.c:2892. */
static int hofri_effect(int player, int card, event_t event)
{
  unsigned int uVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  unsigned int uVar4;
  int iVar5;
  int iVar6;
  int grave_id;
  target_t *ptVar7;
  unsigned int player_00;
  token_generation_2022_t token;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar2 = get_card_instance(player, card);
      if ((-1 < pcVar2->damage_target_player) &&
          (iVar5 = get_id((int)pcVar2->damage_target_player, pcVar2->damage_target_card),
           iVar5 != 0x589e))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar2 = get_card_instance(player, card);
    if ((-1 < pcVar2->damage_target_player) &&
        (iVar5 = effect_follows_control_of_attachment(player, card, 0x4700), iVar5 != 0))
    {
      return 0;
    }
  LAB_023c6f08:
    uVar4 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 == 0)
      goto LAB_023c6f2c;
  }
  else
  {
    pcVar2 = get_card_instance(player, card);
    if ((-1 < pcVar2->damage_target_player) &&
        (iVar5 = effect_follows_control_of_attachment(player, card, event), iVar5 != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_023c6f08;

    pcVar2 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);

    if ((pcVar3 != NULL) && (pcVar3 = get_card_instance(affected_card_controller, affected_card),
                             pcVar3->kill_code != '\0'))
    {

      if ((-1 < pcVar2->damage_target_player) &&
          ((affected_card == pcVar2->damage_target_card &&
            (pcVar2->damage_target_player == affected_card_controller))))
      {
        pcVar2->damage_target_card = -1;
        pcVar2->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        return 0;
      }
      count_for_gfp_ability_and_store_values(player, card, 0x77, player, 2, NULL, 0x10d0, 0);
    }
    uVar4 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 == 0)
    {
      return 0;
    }
  }
  iVar5 = check_status(player, card, 0x10000);
  if (iVar5 == 0)
  {

    pcVar2 = get_card_instance(player, card);
    if ((uVar4 & 0xff) != 0)
    {
      ptVar7 = pcVar2->targets;

      do
      {
        uVar1 = ptVar7->player;
        iVar5 = ptVar7->card;
        ptVar7->card = 0;
        player_00 = uVar1 & 0xf;
        ptVar7->player = 0;
        iVar6 = *graveyard_ptr_mutable[player_00];
        if (iVar6 != -1)
        {
          grave_id = 0;
          do
          {
            if (iVar5 == iVar6)
            {
              rfg_card_from_grave(player_00, grave_id);
              default_token_definition_2022(player, card, (unsigned int)cards_data[iVar5].id, &token);
              token.special_code_on_generation = add_hofry_legacy;
              token.action_argument = uVar1;
              generate_token_2022(&token);
              break;
            }
            grave_id = grave_id + 1;
            iVar6 = graveyard_ptr_mutable[player_00][grave_id];
          } while (iVar6 != -1);
        }
        ptVar7 = ptVar7 + 1;
      } while (ptVar7 != pcVar2->targets + (uVar4 & 0xff));
    }
    kill_card(player, card, KILL_EXILE);
  }
LAB_023c6f2c:
  if ((event == EVENT_CLEANUP) && (iVar5 = check_status(player, card, 0x10000), iVar5 == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23c7cd0; original cards/strixhaven.c:6173. */
int rushed_rebirth_legacy(int player, int card, event_t event)
{
  char cVar1;
  int iVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int32_t iVar5;
  card_instance_t *pcVar6;
  int iVar7;
  char buffer[100];
  test_definition_t test;

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {

    pcVar6 = get_card_instance(player, card);
    iVar2 = pcVar6->damage_target_card;

    if ((((iVar2 == affected_card) &&
          (iVar7 = (int)pcVar6->damage_target_player, iVar7 == affected_card_controller)) &&
         (iVar3 = check_for_death_trigger_removal(iVar7, iVar2), iVar3 == 0)) &&
        (pcVar4 = get_card_instance(iVar7, iVar2), (unsigned char)(pcVar4->kill_code - 1) < 3))
    {
      cVar1 = pcVar6->damage_target_player;
      iVar2 = pcVar6->damage_target_card;
      pcVar6->targets[0xb].player = 0x42;
      pcVar6->targets[0].card = iVar2;
      pcVar6->targets[0].player = (int)cVar1;
      iVar2 = get_cmc((int)cVar1, iVar2);
      pcVar6->targets[1].player = iVar2;
      iVar2 = is_what((int)pcVar6->damage_target_player, pcVar6->damage_target_card, 2);
      iVar5 = 1;
      if (iVar2 != 0)
      {
        iVar5 = card_counts[player][5] + 1;
      }
      pcVar6->targets[0xb].player = iVar5;
      pcVar6->damage_target_player = -1;
      pcVar6->damage_target_card = -1;
    }
    iVar2 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
    {
      return 0;
    }
  }
  else
  {
    iVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_023c7ee4;
    }
  }

  pcVar6 = get_card_instance(player, card);
  iVar7 = pcVar6->targets[1].player;
  scnprintf(buffer, 100, "Select a creature card with mana value %d or less.", iVar7 + -1);
  new_default_test_definition(&test, 2, buffer);
  test.cmc_flag = 3;
  test.cmc = iVar7;
  do
  {
    new_global_tutor(player, player, 1, 5, 0, 1, &test);
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  kill_card(player, card, KILL_EXILE);
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
LAB_023c7ee4:
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x23da480; original cards/strixhaven.c:6221. */
int card_rushed_rebirth(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_CHECK_PUMP)
  {
    instance = in_hand(player, card);
    if ((instance != NULL) &&
        (result = has_mana_to_cast_iid(player, EVENT_CAN_CAST, instance->internal_card_id),
         result != 0))
    {
      pumpable_toughness[player] = pumpable_toughness[player] + 99;
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
        default_target_definition(player, card, &td, 2);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            create_targetted_legacy_effect(player, card, rushed_rebirth_legacy,
                                           instance->targets[0].player, instance->targets[0].card);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_023da4f4;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_023da4f4:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int is_basic_forest_or_island(int iid, int unused2, int player, int card);

/* ManalinkEh 2022: 0x23c7c00; original cards/strixhaven.c:5659. */
static int is_basic_forest_or_island(int iid, int unused2, int player, int card)
{
  subtype_t subtype;
  subtype_t subtype_00;
  int iVar1;
  int iVar2;

  subtype = get_hacked_subtype(player, card, 0x5001);
  subtype_00 = get_hacked_subtype(player, card, 0x5002);
  iVar1 = has_subtype_by_id_2022(player, (unsigned int)cards_data[iid].id, SUBTYPE_BASIC);
  iVar2 = 0;
  if (iVar1 != 0)
  {
    iVar1 = has_subtype_by_id_2022(player, (unsigned int)cards_data[iid].id, subtype);
    iVar2 = 1;
    if (iVar1 == 0)
    {
      iVar2 = has_subtype_by_id_2022(player, (unsigned int)cards_data[iid].id, subtype_00);
      return (unsigned int)(iVar2 != 0);
    }
  }
  return iVar2;
}

/* ManalinkEh 2022: 0x23d8cc0; original cards/strixhaven.c:5668. */
int card_quandrix_cultivator(int player, int card, event_t event)
{
  int result;
  const char *pcVar2;
  char buffer[100];
  test_definition_t test3;

  result = duh_mode(player);
  etb_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {

    pcVar2 = get_hacked_land_text(player, card, "Select a %s ", 0x5001);
    result = scnprintf(buffer, 100, "%s", pcVar2);
    pcVar2 = get_hacked_land_text(player, card, "or %s card.", 0x5002);
    scnprintf(buffer + result, 100 - result, "%s", pcVar2);
    new_default_test_definition(&test3, 0xff, buffer);
    test3.special_selection_function = is_basic_forest_or_island;
    new_global_tutor(player, player, 1, 4, 0, 5, &test3);
  }
  result = hybrid(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23d0b50; original cards/strixhaven.c:3047. */
int card_honor_troll(int player, int card, event_t event)
{
  enum
  {
    XTRIGGER_REPLACE_GAIN_LIFE = 16778752
  };

  xtrigger_t xVar1;
  int result;

  vigilance_2022(player, card, event);
  xVar1 = trigger_condition;
  if (trigger_condition == 0xe5)
  {
    xVar1 = xtrigger_impl_value_dont_use_directly;
  }
  if (xVar1 == XTRIGGER_REPLACE_GAIN_LIFE)
  {
    if (card != affected_card)
    {
      return 0;
    }
    if (player == affected_card_controller)
    {
      if (((reason_for_trigger_controller != player) || (trigger_cause_controller != player)) ||
          (life_gained < 1))
      {
        if (1 < event - EVENT_POWER)
        {
          return 0;
        }
        goto LAB_023d0bb1;
      }
      result = is_humiliated_for_ability(player, card, AL_TRIG);
      if (result == 0)
      {
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
          return 0;
        }
        if (event == EVENT_RESOLVE_TRIGGER)
        {
          life_gained = life_gained + 1;
          return 0;
        }
      }
      goto LAB_023d0b8a;
    }
    if (1 < event - EVENT_POWER)
    {
      return 0;
    }
  }
  else
  {
  LAB_023d0b8a:
    if (1 < event - EVENT_POWER)
    {
      return 0;
    }
    if (card != affected_card)
    {
      return 0;
    }
  }
  if (player != affected_card_controller)
  {
    return 0;
  }
LAB_023d0bb1:
  result = is_humiliated_for_ability(player, card, AL_STATIC);
  if ((result == 0) && (0x18 < life[player]))
  {
    if (event == EVENT_POWER)
    {
      event_result = event_result + 2;
    }
    else if (event == EVENT_TOUGHNESS)
    {
      event_result = event_result + 1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d8f80; original cards/strixhaven.c:5724. */
int card_radiant_scrollwielder(int player, int card, event_t event)
{
  enum
  {
    MPCFE_EXILE_ON_RESOLUTION = 128
  };

  unsigned short uVar1;
  int *piVar2;
  int iVar3;
  int argument1;
  int iVar4;
  int found[151];

  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {

    piVar2 = graveyard_ptr_mutable[player];
    iVar3 = *piVar2;
    if (iVar3 != -1)
    {
      argument1 = 0;
      iVar4 = 0;
      do
      {
        iVar3 = is_what(-1, iVar3, 0x38);
        if (iVar3 != 0)
        {
          found[argument1] = iVar4;
          argument1 = argument1 + 1;
        }
        iVar4 = iVar4 + 1;
        iVar3 = piVar2[iVar4];
      } while (iVar3 != -1);
      if (argument1 != 0)
      {
        iVar3 = internal_rand(argument1);
        uVar1 = cards_data[piVar2[found[iVar3]]].id;
        rfg_card_from_grave(player, found[iVar3]);
        create_may_play_card_from_exile_effect(player, card, player, (unsigned int)uVar1,
                                               MPCFE_EXILE_ON_RESOLUTION | MPCFE_UNTIL_EOT);
        return 0;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23dcb20; original cards/strixhaven.c:7116. */
int card_spell_satchel(int player, int card, event_t event)
{
  enum
  {
    COUNTER_BOOK = 161
  };

  card_instance_t *instance;
  int result;

  magecraft_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event == EVENT_SPA)
  {
    add_counters_from_source(player, card, player, card, COUNTER_BOOK, 1);
  LAB_023dcccf:

    result = mana_producer(player, card, event);
    return result;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = count_counters(player, card, COUNTER_BOOK);
      if ((result == 0) || (result = mana_producer(player, card, EVENT_CAN_ACTIVATE), result == 0))
      {
        result = count_counters(player, card, COUNTER_BOOK);
        if (result < 3)
        {
          return 0;
        }
        result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 4, 0, 0, 0, 0, 0, 0,
                                           NULL, NULL);
        if (result == 0)
        {
          return 0;
        }
      }
      return 1;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_023dcccf;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_ACTIVATE)
  {
    instance->mode = '\0';
    result = paying_mana();
    if ((((result != 0) || (result = count_counters(player, card, COUNTER_BOOK), result == 0)) ||
         (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 4, 0, 0, 0, 0, 0,
                                             0, NULL, NULL),
          result == 0)) ||
        (result =
             do_dialog(player, player, card, -1, -1, " Produce mana\n Draw a card\n Cancel", 1),
         result == 0))
    {
      remove_counters_from_source(player, card, player, card, COUNTER_BOOK, 1);
      event = EVENT_ACTIVATE;
      goto LAB_023dcccf;
    }
    if (result == 1)
    {
      add_state(player, card, 0x10);
      generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 3, 0, 0, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        remove_state(player, card, 0x10);
      }
      else
      {
        remove_counters_from_source(player, card, player, card, COUNTER_BOOK, 3);
        result = hand_count[player];
        instance->mode = '\x01';
        ai_modifier = ai_modifier + result * -10 + 0x46;
      }
    }
    else if (result == 2)
    {
      spell_fizzled = 1;
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) && (instance->mode == '\x01'))
  {
    draw_a_card(player);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x23dd0c0; original cards/strixhaven.c:7277. */
int card_strixhaven_stadium(int player, int card, event_t event)
{
  enum
  {
    COUNTER_POINT = 169
  };

  unsigned int uVar1;
  int iVar2;
  unsigned int num;
  int iVar3;
  int times_damaged[2];

  uVar1 = damage_dealt_to_me_arbitrary(
      2, -1, event, DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_BE_COMBAT_DAMAGE, player, card);
  if (uVar1 != 0)
  {

    iVar3 = 0;
    num = uVar1 & 0xff;
    times_damaged[1] = uVar1 >> 8 & 0xff;
    times_damaged[0] = num;
    while (true)
    {
      if (num != 0)
      {
        if (1 - player == iVar3)
        {
          add_counters_from_source(player, card, player, card, COUNTER_POINT, num);
          iVar2 = count_counters(player, card, COUNTER_POINT);
          if (9 < iVar2)
          {
            lose_the_game_2022(1 - player);
          }
        }
        if (player == iVar3)
        {
          remove_counters_from_source(player, card, player, card, COUNTER_POINT, num);
        }
      }
      iVar3 = iVar3 + 1;
      if (iVar3 == 2)
        break;
      num = times_damaged[iVar3];
    }
  }
  if (event == EVENT_ACTIVATE)
  {
    add_counters_from_source(player, card, player, card, COUNTER_POINT, 1);
    ai_modifier = ai_modifier + 0x100;
  }
  iVar3 = mana_producer(player, card, event);
  return iVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23cbf60; original cards/strixhaven.c:1273. */
int card_daemogoth_woe_eater(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
    new_sacrifice(player, card, player, SAC_AS_COST, &test);
  }
  result = this_is_sacrificed_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    discard(1 - player, 0, player);
    draw_a_card(player);
    gain_life_2022(player, 2);
  }
  result = hybrid(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23ddf40; original cards/strixhaven.c:7567. */
int card_test_of_talents(int player, int card, event_t event)
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
        counterspell_target_definition(player, card, &td, 0x38);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {

            result = get_id(instance->targets[0].player, instance->targets[0].card);
            real_counter_a_spell(player, card, instance->targets[0].player,
                                 instance->targets[0].card);
            result = lobotomy_effect_2022(player, instance->targets[0].player, result, -1, 0);
            draw_cards(player, result);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_023ddfa6;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  counterspell_target_definition(player, card, &td, 0x38);
  get_card_instance(player, card);
LAB_023ddfa6:
  result = generic_spell(player, card, event, 2, &td, NULL, 1, NULL);
  return result;
}


/* ManalinkEh 2022: 0x23ddcf0; original cards/strixhaven.c:7520. */
int card_tenured_inkcaster(int player, int card, event_t event)
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

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &recovered_stack_td.target, 2);
    recovered_stack_td.target.preferred_controller = player;
    recovered_stack_td.target.allow_cancel = 0;
    instance = get_card_instance(player, card);
    result = can_target(&recovered_stack_td.target);
    if (result != 0)
    {
      result = new_pick_target(&recovered_stack_td.target, "TARGET_CREATURE", 0, 0);
      if (result != 0)
      {
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 1);
      }
    }
    instance->number_of_targets = '\0';
    dat_ability(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    return 0;
  }
  if (event != EVENT_DECLARE_ATTACKERS)
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {
      instance = get_card_instance(player, card);
      result = instance->info_slot + -1;
      if (-1 < result)
      {
        do
        {
          lose_life(1 - player, 1);
          result = result + -1;
          gain_life(player, 1);
        } while (result != -1);
        return 0;
      }
    }
    return 0;
  }
  new_default_test_definition(&recovered_stack_td.test, 2, "");
  store_attackers(player, card, EVENT_DECLARE_ATTACKERS, 0, player, -1, &recovered_stack_td.test);
  dat_ability(player, card, EVENT_DECLARE_ATTACKERS, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23d4150; original cards/strixhaven.c:4258. */
int card_master_symmetrist(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  int iVar5;

  store_attackers(player, card, event, DAT_TRACK, player, -1, NULL);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  if (event == EVENT_DAT_ABILITY)
  {

    pcVar1 = get_card_instance(player, card);
    iVar5 = pcVar1->info_slot + -1;
    if (-1 < iVar5)
    {
      do
      {
        pcVar2 = in_play(current_turn,
                         (unsigned int)*(unsigned char *)((int)&pcVar1->targets[2].player + iVar5));
        if (pcVar2 != NULL)
        {
          iVar3 =
              get_power(current_turn,
                        (unsigned int)*(unsigned char *)((int)&pcVar1->targets[2].player + iVar5));
          iVar4 = get_toughness(
              current_turn,
              (unsigned int)*(unsigned char *)((int)&pcVar1->targets[2].player + iVar5));
          if (iVar3 == iVar4)
          {
            pump_ability_until_eot_no_repeat_2022(
                player, card, current_turn,
                (unsigned int)*(unsigned char *)((int)&pcVar1->targets[2].player + iVar5), 0x80, 0);
          }
        }
        iVar5 = iVar5 + -1;
      } while (iVar5 != -1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23d4840; original cards/strixhaven.c:4367. */
int card_mila_crafty_companion(int player, int card, event_t event)
{
  int iVar1;
  const target_t *ptVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  int iVar5;
  test_definition_t test;

  check_legend_rule(player, card, event);
  mdfc_front(player, card, event);
  store_attackers(player, card, event, DAT_TRACK, 1 - player, -1, NULL);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  if (event == EVENT_DAT_ABILITY)
  {

    pcVar3 = get_card_instance(player, card);
    iVar1 = pcVar3->info_slot + -1;
    if (-1 < iVar1)
    {
      do
      {
        pcVar4 = in_play(current_turn,
                         (unsigned int)*(unsigned char *)((int)&pcVar3->targets[2].player + iVar1));
        if ((pcVar4 != NULL) &&
            (iVar5 = planeswalker_its_attacking(
                 current_turn,
                 (unsigned int)*(unsigned char *)((int)&pcVar3->targets[2].player + iVar1)),
             -1 < iVar5))
        {
          default_test_definition(&test, 0x100);
          new_manipulate_all(player, card, player, &test, 0x1420001);
          break;
        }
        iVar1 = iVar1 + -1;
      } while (iVar1 != -1);
    }
  }
  iVar1 = duh_mode(player);
  ptVar2 = any_becomes_target(player, card, event, player, -1, TYPE_PERMANENT, -1, TYPE_ANY,
                              1 - player, (iVar1 != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (((ptVar2 != NULL) && (pcVar3 = in_play(player, card), pcVar3 != NULL)) &&
      (iVar1 = is_humiliated_for_ability(player, card, AL_TRIG), iVar1 == 0))
  {
    draw_cards(player, 1);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23cc710; original cards/strixhaven.c:1412. */
int card_devastating_mastery(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int iStack_ec;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_this_test;

  if (event == EVENT_MODIFY_COST)
  {
    iVar2 = has_mana_multi(player, 2, 0, 0, 0, 0, 2);
    if (iVar2 != 0)
    {
      null_casting_cost(player, card);
      pcVar1 = get_card_instance(player, card);
      pcVar1->info_slot = 1;
      return 0;
    }
    pcVar1 = get_card_instance(player, card);
    pcVar1->info_slot = 0;
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
      pcVar1 = get_card_instance(player, card);
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_023cc7c9;
      pcVar1->targets[1].player = 0;
      pcVar3 = get_card_instance(player, card);
      if ((pcVar3->info_slot != 1) || (iVar2 = is_token(player, card), iVar2 != 0))
        goto LAB_023cc7c9;
      iVar2 = has_mana_to_cast_id(player, EVENT_CAST_SPELL, 0x5866);
      if (iVar2 == 0)
      {
      LAB_023cca7e:
        iVar2 = get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, 2);
        charge_mana_multi(player, iVar2, 0, 0, 0, 0, 2);
        iVar4 = spell_fizzled;
        if (spell_fizzled == 1)
        {
          return 0;
        }
        pcVar1->targets[1].player = 0x42;
      }
      else
      {
        iVar2 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014",
                            "Pay alternate mana cost", 1, 0x14, "Pay full mana cost", 1, 10, 0, 0,
                            0, 0);
        if (iVar2 == 0)
        {
          spell_fizzled = 1;
          iVar4 = spell_fizzled;
        }
        else
        {
          if (iVar2 == 1)
            goto LAB_023cca7e;
          iVar4 = spell_fizzled;
          if (iVar2 == 2)
          {
            charge_mana_from_id(player, -1, EVENT_CAST_SPELL, 0x5866);
            iVar4 = spell_fizzled;
          }
        }
      }
      if (iVar4 != 0)
      {
        return 0;
      }
      goto LAB_023cc7c9;
    }
    if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
        (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  pcVar1 = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (pcVar1->targets[1].player == 0x42)
    {
      default_target_definition(player, card, &recovered_stack_this_test.target, 0x47);
      recovered_stack_this_test.target.who_chooses = (int32_t)(1 - player);
      recovered_stack_this_test.test.message[0x14] = '\x01';
      recovered_stack_this_test.test.message[0x15] = '\0';
      recovered_stack_this_test.test.message[0x16] = '\0';
      recovered_stack_this_test.test.message[0x17] = '\0';
      recovered_stack_this_test.test.message[0x1c] = '\0';
      recovered_stack_this_test.test.message[0x1d] = '\0';
      recovered_stack_this_test.test.message[0x1e] = '\0';
      recovered_stack_this_test.test.message[0x1f] = '\0';
      recovered_stack_this_test.test.message[0x44] = '\x03';
      recovered_stack_this_test.test.message[0x45] = '\0';
      recovered_stack_this_test.test.message[0x46] = '\0';
      recovered_stack_this_test.test.message[0x47] = '\0';
      iStack_ec = 2;
      recovered_stack_this_test.target.allowed_controller =
          (int32_t)((uint32_t)(recovered_stack_this_test.target.who_chooses));
      do
      {
        iVar2 = can_target(&recovered_stack_this_test.target);
        if ((iVar2 == 0) ||
            (iVar2 = new_pick_target(&recovered_stack_this_test.target,
                                     "Select a nonland permanent you control.", 0, 0x80),
             iVar2 == 0))
          break;
        action_on_target(player, card, 0, ACT_BOUNCE);
        iStack_ec = iStack_ec + -1;
      } while (iStack_ec != 0);
    }
    default_test_definition(&recovered_stack_this_test.test, 1);
    recovered_stack_this_test.test.type_flag = 1;
    new_manipulate_all(player, card, 2, &recovered_stack_this_test.test, ACT_KILL_DESTROY);
    kill_card(player, card, KILL_DESTROY);
  }
LAB_023cc7c9:
  iVar2 = basic_spell(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x23d0750; original cards/strixhaven.c:2957. */
int card_hofri_ghostforge(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  int grave_id;
  unsigned int player_00;
  target_t *ptVar6;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_2022_t token;
    uint8_t bytes[216];
  } recovered_stack_test;

  check_legend_rule(player, card, event);
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
          if ((((instance != NULL) && (iVar5 = is_what(player, iVar4, 0x80), iVar5 != 0)) &&
               (instance = get_card_instance(player, iVar4),
                instance->info_slot_fn == hofri_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_023d0982;
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[player]);
      }
      iVar4 = create_targetted_legacy_effect(player, card, hofri_effect, player, card);
      instance = get_card_instance(player, iVar4);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar4, 0x10000);
    }
  LAB_023d0982:
    uVar2 = resolve_gfp_ability(player, card, EVENT_STATIC_EFFECTS, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
    {
      return 0;
    }
  LAB_023d09aa:
    instance = get_card_instance(player, card);
    if (instance->kill_code != '\0')
      goto LAB_023d07b4;
  }
  else
  {
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
    {
      uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
      if (uVar2 == 0)
        goto LAB_023d07b4;
      goto LAB_023d09aa;
    }
    default_test_definition(&recovered_stack_test.test, 2);
    recovered_stack_test.test.not_me = 1;
    count_for_gfp_ability_and_store_values(player, card, 0x77, player, 2,
                                           &recovered_stack_test.test, 0x10d0, 0);
    uVar2 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    if (instance->kill_code != '\0')
    {
      return 0;
    }
  }
  if ((uVar2 & 0xff) != 0)
  {
    ptVar6 = instance->targets;
    do
    {
      uVar1 = ptVar6->player;
      iVar4 = ptVar6->card;
      ptVar6->card = 0;
      player_00 = uVar1 & 0xf;
      ptVar6->player = 0;
      iVar5 = *graveyard_ptr_mutable[player_00];
      if (iVar5 != -1)
      {
        grave_id = 0;
        do
        {
          if (iVar4 == iVar5)
          {
            rfg_card_from_grave(player_00, grave_id);
            default_token_definition_2022(player, card, (unsigned int)cards_data[iVar4].id,
                                     &recovered_stack_test.token);
            recovered_stack_test.token.special_code_on_generation = add_hofry_legacy;
            recovered_stack_test.token.action_argument = (int32_t)(uVar1);
            generate_token_2022(&recovered_stack_test.token);
            break;
          }
          grave_id = grave_id + 1;
          iVar5 = graveyard_ptr_mutable[player_00][grave_id];
        } while (iVar5 != -1);
      }
      ptVar6 = ptVar6 + 1;
    } while (ptVar6 != instance->targets + (uVar2 & 0xff));
  }
LAB_023d07b4:
  if ((((event - EVENT_POWER < 3) && (instance = in_play(player, card), instance != NULL)) &&
       ((iVar4 = is_humiliated_for_ability(player, card, AL_STATIC),
         iVar4 == 0 && (((affected_card_controller == player && (affected_card != card)) &&
                         (instance = in_play(player, affected_card), instance != NULL)))))) &&
      (iVar4 = has_subtype(affected_card_controller, affected_card, SUBTYPE_SPIRIT), iVar4 != 0))
  {
    if (event < EVENT_ABILITIES)
    {
      event_result = event_result + 1;
    }
    else
    {
      haste(affected_card_controller, affected_card, EVENT_ABILITIES);
      event_result = event_result | 0x80;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23dd6d0; original cards/strixhaven.c:7432. */
int card_team_pennant(int player, int card, event_t event)
{
  uint8_t uVar1;
  card_instance_t *instance;
  int result;
  unsigned int uVar4;
  unsigned int uVar5;
  target_definition_t *td_00;
  const char *prompt;
  target_definition_t *ptStack_c0;
  target_definition_t *ptStack_bc;
  target_definition_t td;
  target_definition_t td1;

  attached_gets_pt(player, card, event, 1, 1);
  attached_gets_kw(player, card, event, KEYWORD_TRAMPLE, SP_KEYWORD_VIGILANCE);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    td_00 = &td;
    default_target_definition(player, card, td_00, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    default_target_definition(player, card, &td1, 2);
    td1.special = 0x8000;
    td1.extra = (int32_t)target_is_token_too_2022;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x100, 3, 0, 0, 0, 0, 0,
                                         0, td_00, NULL);
      if (result == 0)
      {
        result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x100, 1, 0, 0, 0, 0,
                                           0, 0, &td1, NULL);
        return result;
      }
      return 1;
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
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "\001\033",
                           "Normal equip", uVar5, 1, "Token equip", uVar4, 5, 0, 0, 0, 0);
      if (result != 0)
      {
        if (result == 1)
        {
          prompt = "Select target creature you control.";
          result = 3;
        }
        else
        {
          if (result != 2)
          {
            return 0;
          }
          prompt = "Select target creature token you control.";
          result = 1;
          td_00 = &td1;
        }
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x102, result, 0, 0, 0, 0,
                                           0, 0, td_00, prompt);
        return result;
      }
      spell_fizzled = 1;
      return 0;
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
    td1.special = 0x8000;
    td1.extra = (int32_t)target_is_token_too_2022;
    instance = get_card_instance(player, card);
  }
  ptStack_bc = &td;
  ptStack_c0 = &td1;
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  uVar1 = instance->mode;
  if (uVar1 == '\x01')
  {
    result = validate_target(player, card, ptStack_bc, 0);
    if (result != 0)
      goto LAB_023dda65;
    uVar1 = instance->mode;
  }
  if (uVar1 != '\x02')
  {
    return 0;
  }
  result = validate_target(player, card, ptStack_c0, 0);
  if (result == 0)
  {
    return 0;
  }
LAB_023dda65:
  equip_target_creature(instance->parent_controller, instance->parent_card,
                        instance->targets[0].player, instance->targets[0].card);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23ccb40; original cards/strixhaven.c:1496. */
int card_dina_soul_steeper(int player, int card, event_t event)
{
  int iVar1;
  unsigned int uVar2;
  card_instance_t *instance;
  uint32_t data;
  unsigned int card_00;
  inst_info_t iVar4;
  test_definition_t test;

  check_legend_rule(player, card, event);
  iVar1 = trigger_gain_life(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
  if ((iVar1 != 0) && (instance = in_play(player, card), instance != NULL))
  {
    iVar1 = check_battlefield_for_id(player, 0x304d);
    if (iVar1 == 0)
    {
      lose_life(1 - player, 1);
    }
    else
    {
      lose_the_game(1 - player);
    }
  }
  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event != EVENT_ACTIVATE)
    {
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }

      instance = get_card_instance(player, card);
      iVar4 = inst_info(instance, 0x5868);
      pump_until_eot_2022(instance->parent_controller, instance->parent_card,
                     instance->parent_controller, instance->parent_card, iVar4.data, 0);
      return 0;
    }

    new_default_test_definition(&test, 2, "Select another creature to sacrifice.");
    test.not_me = 1;
    uVar2 =
        new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
    if (uVar2 == 0)
    {
      spell_fizzled = 1;
    }
    else
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 1, 0, 0, 0, 0, 0, 0, NULL, NULL);
      card_00 = uVar2 >> 0x18;
      uVar2 = (int)uVar2 >> 0x10 & 0xff;
      if (spell_fizzled == 1)
      {
        state_untargettable(uVar2, card_00, 0);
        return 0;
      }
      data = get_power(uVar2, card_00);
      instance = get_card_instance(player, card);
      set_inst_info(instance, 0x5868, data, 0);
      kill_card(uVar2, card_00, KILL_SACRIFICE);
    }
    return 0;
  }
  iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x40200, 0, 1, 0, 0, 0, 0, 0,
                                    NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x23cd420; original cards/strixhaven.c:1638. */
int card_draconic_intervention(int player, int card, event_t event)
{
  int id;
  card_instance_t *instance;
  int iVar2;
  uint32_t data;
  inst_info_t iVar3;
  test_definition_t test;
  test_definition_t test2;

  if (event == EVENT_CAN_CAST)
  {
    new_default_test_definition(&test, 0x38, "Select an instant or sorcery card.");
    get_card_instance(player, card);
    iVar2 = new_special_count_grave(player, &test);
    if (iVar2 != 0)
    {
      iVar2 = basic_spell(player, card, EVENT_CAN_CAST);
      return iVar2;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      new_default_test_definition(&test, 0x38, "Select an instant or sorcery card.");
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        instance->info_slot = 0;
        iVar2 = new_select_a_card(player, player, 2, 0, 2, -1, &test);
        if (iVar2 < 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        id = graveyard_ptr_mutable[player][iVar2];
        rfg_card_from_grave(player, iVar2);
        data = get_cmc_by_internal_id(id);
        set_inst_info(instance, 0x586b, data, 0);
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    new_default_test_definition(&test, 0x38, "Select an instant or sorcery card.");
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      default_test_definition(&test2, 2);
      test2.subtype = 0x30;
      test2.subtype_flag = 1;
      iVar3 = inst_info(instance, 0x586b);
      new_damage_all(player, card, 2, iVar3.data, 8, &test2);
      kill_card(player, card, KILL_EXILE);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23cda20; original cards/strixhaven.c:1799. */
int card_dramatic_finale(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  inst_info_t iVar4;

  if (event - EVENT_POWER < 2)
  {
    if ((((affected_card_controller == player) &&
          (iVar2 = is_token(player, affected_card), iVar2 != 0)) &&
         (instance = in_play(player, card), instance != NULL)) &&
        (iVar2 = is_humiliated_for_ability(player, card, AL_STATIC), iVar2 == 0))
    {
      event_result = event_result + 1;
    }
  }
  else
  {
    if ((event == EVENT_CAN_CAST) || (event == EVENT_SHOULD_AI_PLAY))
    {
      iVar2 = global_enchantment(player, card, event);
      return iVar2;
    }
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
                instance->info_slot_fn == dramatic_finale_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_023cda67;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, dramatic_finale_effect, player, card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  }
LAB_023cda67:
  count_for_gfp_ability_and_store_values(player, card, event, player, 2, NULL, 0x40, 0);
  instance = get_card_instance(player, card);
  iVar4 = inst_info(instance, 0x586e);
  iVar2 = resolve_gfp_ability_2022(player, card, event, (unsigned int)(iVar4.data2 != 0x42) * 2);
  if ((iVar2 != 0) && (instance = get_card_instance(player, card), instance->kill_code == '\0'))
  {
    instance = get_card_instance(player, card);
    set_inst_info(instance, 0x586e, 0, 0x42);
    make_inkling(player, card, 1, 0);
  }
  if (event == EVENT_CLEANUP)
  {
    instance = get_card_instance(player, card);
    clear_info(instance, 0x586e);
  }
  iVar2 = hybrid(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x23dc610; original cards/strixhaven.c:6996. */
int card_silverquill_silencer(int player, int card, event_t event)
{
  uint32_t data;
  card_instance_t *instance;
  inst_info_t result;
  test_definition_t test;

  if (event == EVENT_RESOLVE_SPELL)
  {

    instance = get_card_instance(player, card);
    new_default_test_definition(&test, 1, "Select a nonland card.");
    test.type_flag = 1;
    if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
    {
      data = ai_name_a_random_csvid(player, card, 1 - player, TYPE_NONE, &test);
    }
    else
    {
      data = new_card_from_list(player, 3, &test);
    }
    if (-1 < (int)data)
    {
      set_inst_info(instance, 0x5916, data, 0);
      create_card_name_legacy(player, card, data);
    }
    if (trigger_condition != 0xd3)
    {
      return 0;
    }
  }
  else if (trigger_condition != 0xd3)
    goto LAB_023dc635;
  if ((card == affected_card) && (player == affected_card_controller))
  {
    default_test_definition(&test, 0xff);
    instance = get_card_instance(player, card);
    result = inst_info(instance, 0x5916);
    test.id = result.data;
    sp_ability_trigger(1 - player, event, 2, TYPE_ANY, &test, player, card);
  }
LAB_023dc635:
  if (event != EVENT_SPA)
  {
    return 0;
  }
  lose_life_2022(1 - player, 3);
  draw_a_card(player);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23d9080; original cards/strixhaven.c:5762. */
int card_reconstruct_history(int player, int card, event_t event)
{
  unsigned int ret_location;
  const char *pcVar1;
  card_instance_t *instance;
  int grave_position;
  int *piVar3;
  int iVar4;
  inst_info_t iVar5;
  int types[5];
  char msg[100];
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    iVar4 = basic_spell(player, card, EVENT_CAN_CAST);
    return iVar4;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      clear_info(instance, 0x58f6);
      piVar3 = types;
      types[0] = 0x40;
      types[1] = 4;
      types[2] = 0x10;
      types[3] = 8;
      types[4] = 0x100;
      iVar4 = 0x40;
      while (true)
      {
        default_test_definition(&test, iVar4);
        iVar4 = new_special_count_grave(player, &test);
        if (iVar4 != 0)
        {

          iVar5 = inst_info(instance, 0x58f6);
          ret_location = ((uint32_t)iVar5.id | ((uint32_t)iVar5.data2 << 16)) >> 0x10;
          pcVar1 = type_text(msg, 100, test.type, TYPETEXT_0);
          sprintf(test.message, "Select target %s card.", pcVar1);
          iVar4 =
              select_target_from_grave_source(player, card, player, SFG_0, 1, &test, ret_location);
          if (-1 < iVar4)
          {
            iVar4 = turn_card_in_grave_face_down(player, instance->targets[ret_location].player);
            instance->targets[ret_location].card = iVar4;
            increment_data2(instance, 0x58f6);
          }
        }
        piVar3 = piVar3 + 1;
        if (piVar3 == types + 5)
          break;
        iVar4 = *piVar3;
      }
      for (iVar4 = 0; iVar5 = inst_info(instance, 0x58f6),
          iVar4 < (int)(((uint32_t)iVar5.id | ((uint32_t)iVar5.data2 << 16)) >> 0x10);
           iVar4 = iVar4 + 1)
      {
        turn_card_in_grave_face_up(player, instance->targets[iVar4].player,
                                   instance->targets[iVar4].card);
        instance->targets[iVar4].card = graveyard_source[player][instance->targets[iVar4].player];
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    instance = get_card_instance(player, card);

    for (iVar4 = 0; iVar5 = inst_info(instance, 0x58f6),
        iVar4 < (int)(((uint32_t)iVar5.id | ((uint32_t)iVar5.data2 << 16)) >> 0x10);
         iVar4 = iVar4 + 1)
    {

      grave_position = validate_target_from_grave_source(player, card, player, iVar4);
      if (grave_position != -1)
      {
        from_grave_to_hand(player, grave_position, 1);
      }
    }
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23cce60; original cards/strixhaven.c:1544. */
int card_divide_by_zero(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  target_definition_t td2;

  if (event == EVENT_CAN_CAST)
  {
    counterspell_target_definition(player, card, &td, 0xff);
    td.extra = 1;
    td.special = 0x1000000;
    default_target_definition(player, card, &td2, 0x47);
    td2.extra = 1;
    td2.special = 0x1000000;
    result = counterspell(player, card, EVENT_CAN_CAST, &td, 0);
    if (result != 0)
    {
      instance = get_card_instance(player, card);
      instance->mode = '\x01';
      return 99;
    }
    instance = get_card_instance(player, card);
    instance->mode = '\0';
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL);
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
    counterspell_target_definition(player, card, &td, 0xff);
    td.extra = 1;
    td.special = 0x1000000;
    default_target_definition(player, card, &td2, 0x47);
    td2.extra = 1;
    td2.special = 0x1000000;
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    if (instance->mode != '\x01')
    {
      result = generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td2,
                             "Select target spell or permanent with CMC 1 or greater.", 1, NULL);
      return result;
    }
    result = counterspell(player, card, EVENT_CAST_SPELL, &td, 0);
    return result;
  }
  if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
  {
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
    counterspell_target_definition(player, card, &td, 0xff);
    td.extra = 1;
    td.special = 0x1000000;
    default_target_definition(player, card, &td2, 0x47);
    return 0;
  }
  counterspell_target_definition(player, card, &td, 0xff);
  td.extra = 1;
  td.special = 0x1000000;
  default_target_definition(player, card, &td2, 0x47);
  td2.extra = 1;
  td2.special = 0x1000000;
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  if (instance->mode == '\x01')
  {
    result = counterspell_validate(player, card, &td, 0);
    if (result == 0)
      goto LAB_023ccf29;
    result = pre_counterspell(player, card, instance->targets[0].player, instance->targets[0].card);
    if (result != 0)
      goto LAB_023cd200;
    kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
  }
  else
  {
    result = valid_target(&td2);
    if (result == 0)
      goto LAB_023ccf29;
  LAB_023cd200:
    bounce_permanent(instance->targets[0].player, instance->targets[0].card);
  }
  learn(player, card);
LAB_023ccf29:
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23d6ff0; original cards/strixhaven.c:5058. */
int card_plumb_the_forbidden(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_2022_t test;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      result = is_token(player, card);
      if (result != 0)
      {
        return 0;
      }
      new_default_test_definition_2022(&test, 2, "Select a creature to sacrifice.");
      test.qty = 0x97;
      result = new_sacrifice_2022(player, card, player,
                                  SAC2022_OPTIONAL | SAC2022_AS_COST | SAC2022_DONE, &test);
      if ((spell_fizzled != 1) && (0 < result))
      {
        instance = get_card_instance(player, card);
        instance->ext->times_replicate_paid = (uint8_t)result;
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    draw_cards(player, 1);
    lose_life_2022(player, 1);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int uvilda_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23c8860; original cards/strixhaven.c:7771. */
int uvilda_legacy(int player, int card, event_t event)
{
  enum
  {
    COUNTER_HONE = 167
  };

  int iVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int csvid;

  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    remove_counters_from_source(player, card, player, card, COUNTER_HONE, 1);
    return 0;
  }
  if ((event == EVENT_STATIC_EFFECTS) &&
      (iVar1 = count_counters(player, card, COUNTER_HONE), iVar1 == 0))
  {

    instance = get_card_instance(player, card);
    iVar1 = instance->targets[1].player;
    if (iVar1 != -1)
    {
      csvid = (unsigned int)cards_data[iVar1].id;
      iVar3 = check_rfg(player, csvid);
      if ((iVar3 != 0) && (iVar1 = can_legally_play_iid(player, iVar1), iVar1 != 0))
      {
        iVar1 = charge_mana_from_id_with_cost_mod(player, card, EVENT_STATIC_EFFECTS, csvid, -4);
        if (iVar1 == 0)
        {
          return 0;
        }
        play_card_in_exile_for_free(player, player, csvid);
        return 0;
      }
      kill_card(player, card, KILL_EXILE);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23de890; original cards/strixhaven.c:7793. */
int card_uvilda_dean_of_perfection(int player, int card, event_t event)
{
  enum
  {
    COUNTER_HONE = 167
  };

  int iVar1;
  int iVar2;
  card_instance_t *instance;
  test_definition_2022_t test;

  mdfc_front(player, card, event);
  check_legend_rule(player, card, event);
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      new_default_test_definition_2022(&test, 0x38, "Select an instant or sorcery to exile.");
      iVar1 = new_select_a_card_2022(player, player, 4, 0, 2, -1, &test);
      if (iVar1 == -1)
      {
        return 0;
      }
      iVar2 = get_original_internal_card_id(player, iVar1);
      if (iVar2 == -1)
      {
        return 0;
      }
      rfg_card_in_hand(player, iVar1);
      iVar1 = create_legacy_effect(player, card, uvilda_legacy);
      add_counters_from_source(player, card, player, iVar1, COUNTER_HONE, 3);
      instance = get_card_instance(player, iVar1);
      instance->targets[1].player = iVar2;
    }
  }
  iVar1 = generic_activated_ability_2022(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar1;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23d5d90; original cards/strixhaven.c:4759. */
int card_pestilent_cauldron(int player, int card, event_t event)
{
  uint8_t uVar1;
  unsigned char bVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iStack_ec;
  test_definition_2022_t test;

  mdfc_front(player, card, event);
  if (((event == EVENT_ACTIVATE) || (event - EVENT_RESOLVE_ACTIVATION < 2)) ||
      (event == EVENT_CLEANUP))
  {
    instance = get_card_instance(player, card);
    iVar4 = graveyard_has_shroud(player);
    bVar2 = false;
    if (iVar4 == 0)
    {
      iVar4 = count_graveyard(player);
      bVar2 = 3 < iVar4;
    }
    iVar4 = 1 - player;
    iVar5 = graveyard_has_shroud(iVar4);
    if ((iVar5 == 0) && (iVar5 = count_graveyard(iVar4), 3 < iVar5))
    {
      bVar2 = bVar2 | 2;
    }
    new_default_test_definition_2022(&test, 0xff, "Select target card to exile.");
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar4 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 0x21, 0, 0, 0, 0, 0,
                                             0, 0, NULL, NULL);
      if (iVar4 != 0)
      {
        return 1;
      }
      iVar4 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 1, 1, 0, 0, 0, 0, 0,
                                             0, NULL, NULL);
      if (iVar4 != 0)
      {
        return 1;
      }
      iVar4 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 1, 4, 0, 0, 0, 0, 0,
                                             0, NULL, NULL);
      if (iVar4 != 0)
      {
        return (unsigned int)(bVar2 != 0);
      }
    }
    else if (event == EVENT_ACTIVATE)
    {

      iVar5 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 0x21, 0, 0, 0, 0, 0,
                                             0, 0, NULL, NULL);
      iVar7 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 1, 1, 0, 0, 0, 0, 0,
                                             0, NULL, NULL);
      generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 1, 4, 0, 0, 0, 0, 0, 0, NULL,
                                     NULL);
      iVar6 = get_trap_condition(player, 1);
      iVar5 =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\004", "\001\016", "\001\033",
                      "Create a Pest", iVar5, 0x14, "Mill", iVar7, iVar6 << 3, "Exile and draw",
                      iVar7, (-(unsigned int)((bVar2 & 2) == 0) & 0xffffffe7) + 0x1e, 0, 0, 0, 0);
      if (iVar5 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar5 == 1)
      {
        generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 0x21, 0, 0, 0, 0, 0, 0, 0,
                                       NULL, NULL);
        if (spell_fizzled != 1)
        {
          iVar4 = count_subtype(iVar4, 2, -1);
          ai_modifier = ai_modifier + iVar4 * 8;
        }
      }
      else if (iVar5 == 2)
      {
        generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 1, 1, 0, 0, 0, 0, 0, 0, NULL,
                                       NULL);
        if (spell_fizzled != 1)
        {
          iVar4 = get_trap_condition(player, 1);
          ai_modifier = ai_modifier + iVar4 * 8;
        }
      }
      else if (((iVar5 == 3) &&
                (select_multiple_cards_from_either_graveyard_2022(
                     player, card, SFG_CANNOT_CANCEL, 3, 1, NULL, 4, instance->targets + 1),
                 spell_fizzled != 1)) &&
               (generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 1, 4, 0, 0, 0, 0, 0, 0,
                                               NULL, NULL),
                spell_fizzled != 1))
      {
        ai_modifier = ai_modifier + 0x18;
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {

      uVar1 = instance->mode;
      if (uVar1 == '\x01')
      {
        make_pest(player, card, 1);
      }
      else
      {
        if (uVar1 == '\x02')
        {
          iVar5 = get_trap_condition(player, 1);
          mill(iVar4, iVar5);
          return 0;
        }
        if (uVar1 == '\x03')
        {

          iStack_ec = 0;
          iVar4 = 1;
          do
          {

            iVar7 = instance->targets[iVar4].player >> 0x10;
            iVar5 = validate_target_from_grave_source(player, card, iVar7, iVar4);
            if (iVar5 != -1)
            {
              rfg_card_from_grave(iVar7, iVar5);
              iStack_ec = iStack_ec + 1;
            }
            iVar4 = iVar4 + 1;
          } while (iVar4 != 5);
          if (iStack_ec != 0)
          {
            draw_cards(player, 1);
            return 0;
          }
        }
      }
    }
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23dfc10; original cards/strixhaven.c:8243. */
int card_explore_the_vastlands(int player, int card, event_t event)
{
  enum
  {
    TUTOR_0 = 0
  };

  int result;
  unsigned int t_player;
  const char *msg;
  unsigned int uStack_ec;
  test_definition_2022_t test;

  mdfc_back(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition_2022(&test, 0x39, "Select an instant, sorcery or land card.");
    test.ai_selection_mode = 1;

    uStack_ec = 0;
    do
    {
      t_player = uStack_ec ^ current_turn;

      result = reveal_top_cards_of_library_and_choose_who_chooses_2022(
          player, card, t_player, t_player, 5, 0, TUTOR2022_TPLAYER_HAND, 1, TUTOR2022_0, 0, &test);
      if (result == -1)
      {
        put_top_x_on_bottom_in_random_order(t_player, 5);
      }
      else
      {
        result = is_what(t_player, result, 1);
        if (result == 0)
        {
          msg = "Select a land card.";
          result = 1;
        }
        else
        {
          msg = "Select an instant or sorcery card.";
          result = 0x38;
        }
        new_default_test_definition_2022(&test, result, msg);
        reveal_top_cards_of_library_and_choose_who_chooses_2022(
            player, card, t_player, t_player, 4, 0, TUTOR2022_TPLAYER_HAND, 1,
            TUTOR2022_BOTTOM_OF_DECK_RANDOM, 0, &test);
      }
      gain_life_2022(t_player, 3);
      uStack_ec = uStack_ec + 1;
    } while (uStack_ec != 2);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 native scalar-word handlers. */
int lukka_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23c7b30; original cards/strixhaven.c:4404. */
int lukka_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  result = (int)instance->damage_target_player;
  if (-1 < instance->damage_target_player)
  {
    haste(result, instance->damage_target_card, event);
    result = (int)instance->damage_target_player;
  }
  if (result == current_turn)
  {
    result = upkeep_trigger(player, card, event);
    if (result != 0)
    {
      result = check_state((int)instance->damage_target_player, instance->damage_target_card,
                           STATE_OUBLIETTED);
      if (result == 0)
      {
        kill_card((int)instance->damage_target_player, instance->damage_target_card, KILL_EXILE);
      }
      kill_card(player, card, KILL_EXILE);
    }
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23d4a50; original cards/strixhaven.c:4419. */
int card_lukka_wayward_bonder(int player, int card, event_t event)
{
  bool bVar1;
  int iVar2;
  int iVar3;
  card_instance_t *instance;
  test_definition_2022_t *ptStack_1c8;
  test_definition_2022_t this_test;
  test_definition_2022_t this_test2;

  check_legend_rule(player, card, event);
  mdfc_back(player, card, event);
  bVar1 = event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE;
  if (bVar1)
  {
    instance = get_card_instance(player, card);
    new_default_test_definition_2022(&this_test, 2, "Select target creature card.");
    iVar2 = graveyard_has_shroud(player);
    if (iVar2 == 0)
    {
    LAB_023d4cf8:
      ptStack_1c8 = &this_test;
      iVar2 = new_special_count_grave_2022(player, ptStack_1c8);
      iVar2 = dialog_impl(player, card, event, "\001\002", "\001\017", "Discard & draw", 1, 10, 1,
                          "Reanimate target creature", (unsigned int)(iVar2 != 0), 0x14, 0xfffffffe,
                          "Emblem", 1, 5, 0xfffffff9, 0, 0, 0, 0);
      if (event != EVENT_CAN_ACTIVATE)
      {
        if (bVar1)
          goto LAB_023d4dc6;
        goto LAB_023d4bb6;
      }
    }
    else
    {
      iVar2 = dialog_impl(player, card, event, "\001\002", "\001\017", "Discard & draw", 1, 10, 1,
                          "Reanimate target creature", 0, 0x14, 0xfffffffe, "Emblem", 1, 5,
                          0xfffffff9, 0, 0, 0, 0);
      if (event != EVENT_CAN_ACTIVATE)
      {
      LAB_023d4dc6:
        ptStack_1c8 = &this_test;
        instance->number_of_targets = '\0';

        if ((iVar2 == 2) && (iVar2 = select_multiple_cards_from_graveyard_2022(
                                 player, player, 0, 2, ptStack_1c8, 1, instance->targets),
                             iVar2 == -1))
        {
          spell_fizzled = 1;
          return 0;
        }
        goto LAB_023d4aa9;
      }
    }
    if (iVar2 == 0)
    {
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_023d4aa9;

    instance = get_card_instance(player, card);
    new_default_test_definition_2022(&this_test, 2, "Select target creature card.");
    iVar2 = graveyard_has_shroud(player);
    if (iVar2 == 0)
      goto LAB_023d4cf8;
    iVar2 = dialog_impl(player, card, event, "\001\002", "\001\017", "Discard & draw", 1, 10, 1,
                        "Reanimate target creature", 0, 0x14, 0xfffffffe, "Emblem", 1, 5,
                        0xfffffff9, 0, 0, 0, 0);
  LAB_023d4bb6:
    if (iVar2 == 1)
    {

      new_default_test_definition_2022(&this_test2, 0xff, "Select a card to discard.");
      iVar2 = new_select_a_card_2022(player, player, 4, 0, 3, -1, &this_test2);
      if (iVar2 == -1)
      {
        return 0;
      }
      iVar3 = is_what(player, iVar2, 2);
      new_discard_card(player, iVar2, 2, player);
      draw_cards(player, iVar3 + 1);
    }
    else if (iVar2 == 2)
    {

      iVar2 = validate_target_from_grave_source(player, card, player, 0);

      if ((iVar2 != -1) &&
          (iVar2 = reanimate_permanent(player, card, player, iVar2, REANIMATE_DEFAULT), -1 < iVar2))
      {
        create_targetted_legacy_effect(player, card, lukka_legacy, player, iVar2);
      }
    }
    else if (iVar2 == 3)
    {
      generate_token_by_id_2022(player, card, 0x58bd);
    }
  }
LAB_023d4aa9:
  iVar2 = planeswalker(player, card, event, 5);
  return iVar2;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x23dd3c0; original cards/strixhaven.c:7355. */
int card_tanazir_quandrix(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  int mc;
  int marked[151];
  target_definition_2022_t td;
  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition_2022(player, card, &td, 2);
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
    iVar1 = new_pick_target_2022(&td, "TARGET_CREATURE_YOU_CONTROL", 0, 0);
    if (iVar1 != 0)
    {
      iVar1 = count_counters(instance->targets[0].player, instance->targets[0].card, COUNTER_P1_P1);
      if (iVar1 == 0)
      {
        ai_modifier = ai_modifier + -0x60;
      }
      add_counters_from_source(player, card, instance->targets[0].player, instance->targets[0].card,
                               COUNTER_P1_P1, iVar1);
    }
    dat_ability(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    return 0;
  }
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);

  if (((event == EVENT_DAT_ABILITY) &&
       (iVar1 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014", "\001\003",
                            "Set P/T of your creatures", 1, 10, "Decline", 1, 5, 0, 0, 0, 0),
        iVar1 == 1)) &&
      (iVar1 = active_cards_count[player] + -1, -1 < iVar1))
  {
    mc = 0;
    do
    {
      instance = in_play(player, iVar1);
      if (instance != (card_instance_t *)0 && is_what(player, iVar1, 2))
        marked[mc++] = iVar1;
      --iVar1;
    } while (iVar1 != -1);
    if (mc > 0)
    {
      int *cursor = marked + mc - 1;
      do
      {
        int toughness = get_toughness(player, card);
        int power = get_power(player, card);
        set_pt_and_abilities_until_eot(player, card, player, *cursor, power, toughness, 0, 0, 8);
        if (cursor == marked)
          break;
        --cursor;
      } while (1);
    }
  }
  return 0;
}

#endif
