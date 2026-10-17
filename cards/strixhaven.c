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
      disable_all_activated_abilities(player_00, card_00, 1);
      cannot_attack(player_00, card_00, EVENT_ABILITIES);
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
    gain_life(player, 3);
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
      (result = check_special_flags(trigger_cause_controller, trigger_cause, 0x40000), result == 0))
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
    lose_life(player, 2);
    lose_life(1 - player, 2);
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
            set_special_flags3(player, result, 0x1000000);
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
    gain_life(player, 2);
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
        pump_ability_until_eot_no_repeat(instance->parent_controller, instance->parent_card,
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
        copy_spell_from_stack(player, instance->targets[0].player, instance->targets[0].card);
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
  vigilance(player, card, event);
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
