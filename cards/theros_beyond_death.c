#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x235a610; original cards/theros_beyond_death.c:180. */
int card_alseid_of_lifes_bounty(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  target_definition_t td;

  lifelink(player, card, event);
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 6);
    td.preferred_controller = player;
    td.allowed_controller = player;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = global_enchantment(player, card, event);
      return result;
    }
    default_target_definition(player, card, &td, 6);
    td.preferred_controller = player;
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        uVar3 = select_a_protection(player);
        pump_ability_until_eot_2022(instance->parent_controller, instance->parent_card,
                               instance->targets[0].player, instance->targets[0].card, 0, 0,
                               uVar3 | 0x10000000, 0);
      }
    }
  }
  result = generic_activated_ability(player, card, event, 0xe, 1, 0, 0, 0, 0, 0, 0, &td,
                                     "Select target creature or enchantment you control.");
  return result;
}

/* ManalinkEh 2022: 0x235c4a0; original cards/theros_beyond_death.c:1264. */
int card_dryad_of_the_ilysian_grove(int player, int card, event_t event)
{
  check_playable_lands(player);
  all_lands_are_basiclandtype(player, card, event, player, 1, 0x5009);
  all_lands_are_basiclandtype(player, card, event, player, 2, 0x5002);
  all_lands_are_basiclandtype(player, card, event, player, 3, 0x5001);
  all_lands_are_basiclandtype(player, card, event, player, 4, 0x5006);
  all_lands_are_basiclandtype(player, card, event, player, 5, 0x5007);
  if (event == EVENT_RESOLVE_SPELL)
  {
    recalculate_all_cards_in_play();
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x235d700; original cards/theros_beyond_death.c:1839. */
int card_funeral_rites(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    draw_cards(player, 2);
    lose_life_2022(player, 2);
    mill(player, 2);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x235e7c0; original cards/theros_beyond_death.c:2338. */
int card_kunoros_hound_of_athreos(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  menace(player, card, event);
  lifelink(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x235ea20; original cards/theros_beyond_death.c:2481. */
int card_mire_triton(int player, int card, event_t event)
{
  deathtouch_2022(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  mill(player, 2);
  gain_life(player, 2);
  return 0;
}

/* ManalinkEh 2022: 0x2360e70; original cards/theros_beyond_death.c:3761. */
int card_thassas_oracle(int player, int card, event_t event)
{
  int num_cards;
  int result;
  test_definition_t test;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    num_cards = devotion(player, card, 2, 0);
    default_test_definition(&test, 0xff);
    reveal_top_cards_of_library_and_choose(player, card, player, num_cards, 1, TUTOR_DECK, 0,
                                           TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &test);
    result = count_deck(player);
    if (result <= num_cards)
    {
      lose_the_game(1 - player);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23598e0; original cards/theros_beyond_death.c:3834. */
static int akroan_war_legacy(int player, int card, event_t event)
{
  all_must_attack_if_able(1 - player, event, -1);
  if ((event == EVENT_CAN_SKIP_TURN) && (player == current_turn))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x235a360; original cards/theros_beyond_death.c:115. */
int card_agonizing_remorse(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
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
        instance = get_card_instance(player, card);
        if (event != EVENT_RESOLVE_SPELL)
          goto LAB_0235a3dc;
        iVar3 = valid_target(&td);
        if (iVar3 != 0)
        {
          iVar3 = hand_count[instance->targets[0].player];
          iVar4 = -1;
        LAB_0235a4b0:
          if (0 < iVar3)
            goto LAB_0235a506;
          do
          {
            if (-1 < iVar4)
            {
            LAB_0235a4b8:
              lose_life_2022(player, 1);
              break;
            }
            iVar3 = instance->targets[0].player;
            if (*graveyard_ptr_mutable[iVar3] == -1)
            {
              if (hand_count[iVar3] < 1)
                goto LAB_0235a5f0;
            }
            else
            {
              new_default_test_definition(&this_test, 1,
                                          "Opponent\'s graveyard: Select a card to exile.");
              this_test.type_flag = 1;
              iVar4 = new_global_tutor(player, instance->targets[0].player, 2, 6, 0, 1, &this_test);
              if (-1 < iVar4)
                goto LAB_0235a4b8;
              iVar1 = instance->targets[0].player;
              iVar3 = hand_count[iVar1];
              if (iVar3 < 1)
                goto code_r0x0235a5df;
            }
          LAB_0235a506:
            new_default_test_definition(&this_test, 1, "Opponent\'s hand: Select a card to exile.");
            this_test.type_flag = 1;
            iVar4 = new_global_tutor(player, instance->targets[0].player, 4, 6, 0, 1, &this_test);
          } while (true);
        }
        goto LAB_0235a4c8;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  get_card_instance(player, card);
  goto LAB_0235a3dc;
code_r0x0235a5df:
  if (*graveyard_ptr_mutable[iVar1] == -1)
    goto LAB_0235a5f0;
  goto LAB_0235a4b0;
LAB_0235a5f0:
  spell_fizzled = 1;
LAB_0235a4c8:
  kill_card(player, card, KILL_DESTROY);
LAB_0235a3dc:
  iVar3 = generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x235f030; original cards/theros_beyond_death.c:2711. */
int card_nyx_lotus(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int clr1;

  comes_into_play_tapped(player, card, event);
  if (event == EVENT_COUNT_MANA)
  {
    clr1 = 1;
    iVar2 = 0;
    do
    {
      iVar3 = devotion(player, -1, clr1, 0);
      if (iVar2 < iVar3)
      {
        iVar2 = iVar3;
      }
      clr1 = clr1 + 1;
    } while (clr1 != 6);
    instance = get_card_instance(player, card);
    instance->info_slot = iVar2;
  }
  instance = get_card_instance(player, card);
  iVar2 = artifact_mana_all_one_color(player, card, event, instance->info_slot, 0);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int escape(int player, int card, event_t event, int colorless, int black, int blue, int green,
           int red, int white, int num_exile);

static int escape_part_2022(int player, int card, event_t event, int colorless, int black, int blue,
                            int green, int red, int white, int num_exile);

/* ManalinkEh 2022: 0x235a2e0; original cards/theros_beyond_death.c:4. */
int escape(int player, int card, event_t event, int colorless, int black, int blue, int green,
           int red, int white, int num_exile)
{
  int result;

  if ((event != EVENT_GRAVEYARD_ABILITY) && (event != EVENT_PAY_FLASHBACK_COSTS))
  {
    return 0;
  }
  result = escape_part_2022(player, card, event, colorless, black, blue, green, red, white, num_exile);
  return result;
}

/* ManalinkEh 2022: 0x235bb10; original cards/theros_beyond_death.c:946. */
int card_cling_to_dust(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int whose_graveyard;
  test_definition_t test;

  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    iVar1 = escape(player, card, event, 3, 1, 0, 0, 0, 0, 5);
    return iVar1;
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
        new_default_test_definition(&test, 0xff, "Select target card to exile.");
        if (event == EVENT_RESOLVE_SPELL)
        {

          whose_graveyard = 1 - player;
          iVar1 = validate_target_from_grave_source(player, card, whose_graveyard, 1);
          if (iVar1 != -1)
          {

            iVar2 = is_what(-1, graveyard_ptr_mutable[whose_graveyard][iVar1], 2);
            rfg_card_from_grave(whose_graveyard, iVar1);
            if (iVar2 == 0)
            {
              draw_a_card(player);
            }
            else
            {
              gain_life_2022(player, 3);
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0235bb7d;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  new_default_test_definition(&test, 0xff, "Select target card to exile.");
LAB_0235bb7d:
  iVar1 = generic_spell(player, card, event, 0x1000, NULL, NULL, 1, &test);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x235aaf0; original cards/theros_beyond_death.c:408. */
int card_ashiok_nightmare_muse(int player, int card, event_t event)
{
  uint8_t uVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int iStack_148;
  int iStack_144;
  target_definition_t *ptStack_140;
  target_definition_t td;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_token;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->mode = '\0';
      iVar3 = can_target(&td);
      iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "\001\033",
                          "Nightmare token", 1, 5, 1, "Bounce & exile from hand", iVar3, 10,
                          0xfffffffd, "Cast from exile", 1, 0x14, 0xfffffff9, 0, 0, 0, 0);
      if (iVar3 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar3 == 2)
      {
        new_pick_target(&td, "TARGET_NONLAND_PERMANENT", 0, 1);
      }
      goto LAB_0235ab76;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      iVar3 = planeswalker(player, card, event, 5);
      return iVar3;
    }
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 1;
    instance = get_card_instance(player, card);
  }
  ptStack_140 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    uVar1 = instance->mode;
    if (uVar1 == '\x01')
    {
      default_token_definition(player, card, 0x56d9, &recovered_stack_token.token);
      recovered_stack_token.token.pow = 2;
      recovered_stack_token.token.tou = 3;
      recovered_stack_token.token.special_infos = 0x42;
      generate_token(&recovered_stack_token.token);
    }
    else if (uVar1 == '\x02')
    {
      iVar3 = valid_target(ptStack_140);
      if ((iVar3 != 0) && (bounce_permanent(instance->targets[0].player, instance->targets[0].card),
                           hand_count[instance->targets[0].player] != 0))
      {

        default_test_definition(&recovered_stack_token.test, 0xff);
        iVar3 = instance->targets[0].player;
        iVar3 = new_select_a_card(iVar3, iVar3, 4, 1, 3, -1, &recovered_stack_token.test);
        rfg_card_in_hand(instance->targets[0].player, iVar3);
      }
    }
    else if (uVar1 == '\x03')
    {
      iVar3 = 1 - player;
      iVar4 = count_rfg(iVar3);
      if (iVar4 != 0)
      {

        new_default_test_definition(&recovered_stack_token.test, 1,
                                    "Select a card to cast for free.");

        iVar4 = special_count_zone(player, rfg_ptr[iVar3], &recovered_stack_token.test);
        iStack_144 = iVar4;
        if (3 < iVar4)
        {
          iStack_144 = 3;
        }
        iStack_148 = 0;
        if (0 < iVar4)
        {
          do
          {

            iVar4 = new_select_a_card(player, iVar3, 3, 0, 2, 0, &recovered_stack_token.test);
            if ((iVar4 == -1) &&
                (iVar4 = new_select_a_card(player, iVar3, 3, 0, 2, 0, &recovered_stack_token.test),
                 iVar4 == -1))
              break;
            play_card_in_exile_for_free(player, iVar3,
                                        (unsigned int)cards_data[rfg_ptr[iVar3][iVar4]].id);
            iStack_148 = iStack_148 + 1;
          } while (iStack_148 < iStack_144);
        }
      }
    }
  }
LAB_0235ab76:
  iVar3 = planeswalker(player, card, event, 4);
  return iVar3;
}

/* ManalinkEh 2022: 0x235c010; original cards/theros_beyond_death.c:1142. */
int card_destiny_spinner(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  result = is_humiliated_for_ability(player, card, AL_STATIC);
  if (result == 0)
  {
    type_uncounterable(player, card, event, player, 6, NULL);
  }
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 1);
    td.allowed_controller = player;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = global_enchantment(player, card, event);
      return result;
    }
    default_target_definition(player, card, &td, 1);
    td.allowed_controller = player;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        result = count_permanents_by_type(player, TYPE_ENCHANTMENT);
        result = land_animation2(instance->parent_controller, instance->parent_card,
                                 instance->targets[0].player, instance->targets[0].card, 1, result,
                                 result, 0x80, 4, 0, NULL);
        instance = get_card_instance(player, result);
        instance->targets[9].card = 0x3a;
      }
    }
  }
  result = generic_activated_ability(player, card, event, 6, 3, 0, 0, 1, 0, 0, 0, &td,
                                     "Select target land you control.");
  return result;
}

/* ManalinkEh 2022: 0x235c9d0; original cards/theros_beyond_death.c:1428. */
int card_elspeth_suns_nemesis(int player, int card, event_t event)
{
  target_t *ptVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  target_t *ptVar5;
  target_t *ptVar6;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    iVar3 = escape(player, card, event, 4, 0, 0, 0, 0, 2, 4);
    return iVar3;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allow_cancel = 3;
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Pump creatures", 1,
                          10, 0xffffffff, "Create tokens", 1, 10, 0xfffffffe, "Gain life", 1, 10,
                          0xfffffffd, 0, 0, 0, 0);
      if (iVar3 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }

      if ((iVar3 == 1) && (iVar3 = can_target(&td), iVar3 != 0))
      {
        ptVar1 = instance->targets;
        iVar3 = select_target(player, card, &td, " Select target creature.", ptVar1);
        if (iVar3 != 0)
        {
          state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
          iVar4 = can_target(&td);
          iVar3 = 1;
          if ((iVar4 != 0) && (iVar4 = select_target(player, card, &td, " Select target creature.",
                                                     instance->targets + 1),
                               iVar4 != 0))
          {
            state_untargettable(instance->targets[1].player, instance->targets[1].card, 1);
            iVar3 = 2;
          }
          ptVar5 = ptVar1;
          do
          {
            ptVar6 = ptVar5 + 1;
            state_untargettable(ptVar5->player, ptVar5->card, 0);
            ptVar5 = ptVar6;
          } while (ptVar6 != ptVar1 + iVar3);
        }
      }
      goto LAB_0235ca6b;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_0235ca6b;
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
  }
  td.preferred_controller = player;
  td.allow_cancel = 3;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar3 = instance->info_slot;
    if (iVar3 == 1)
    {

      if (instance->number_of_targets != '\0')
      {
        iVar3 = 0;
        do
        {
          ptVar5 = instance->targets + iVar3;
          ptVar1 = instance->targets + iVar3;
          iVar3 = iVar3 + 1;
          pump_until_eot_merge_previous_2022(player, card, ptVar1->player, ptVar5->card, 2, 1);
        } while (iVar3 < (int)(unsigned int)instance->number_of_targets);
      }
      instance->number_of_targets = '\0';
    }
    else if (iVar3 == 2)
    {
      generate_tokens_by_id(player, card, 0x4986, 2);
    }
    else if (iVar3 == 3)
    {
      gain_life(player, 5);
    }
  }
LAB_0235ca6b:
  iVar3 = planeswalker(player, card, event, 5);
  return iVar3;
}

/* ManalinkEh 2022: 0x235d970; original cards/theros_beyond_death.c:1951. */
int card_heliods_intervention(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  uint32_t uVar3;
  int iVar4;
  int iVar5;
  target_definition_t td;
  target_definition_t td1;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x44);
    default_target_definition(player, card, &td1, 0);
    td1.zone = 0x1000;
    get_card_instance(player, card);
    iVar2 = can_target(&td);
    if ((iVar2 != 0) || (iVar2 = can_target(&td1), iVar2 != 0))
    {
      iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 4, NULL, NULL, 0, NULL);
      return iVar2;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x44);
      default_target_definition(player, card, &td1, 0);
      td1.zone = 0x1000;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        iVar2 = can_target(&td1);
        iVar5 = can_target(&td);
        uVar3 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\016", "\001\014",
                            "Destroy artifacts & enchantments", iVar5, 5, "Gain life", iVar2, 5, 0,
                            0, 0, 0);
        iVar2 = max_x_value;
        if (uVar3 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (uVar3 == 1)
        {

          max_x_value = target_available(player, card, &td);
          generic_spell(player, card, EVENT_CAST_SPELL, 5, &td, "TARGET_ARTIFACT_ENCHANTMENT",
                        x_value, NULL);
          max_x_value = iVar2;
        }
        else
        {
          max_x_value = iVar2;
          if (uVar3 == 2)
          {
            generic_spell(player, card, EVENT_CAST_SPELL, 5, &td1, "TARGET_PLAYER", 1, NULL);
          }
        }
        instance->eot_toughness = uVar3;
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x44);
    default_target_definition(player, card, &td1, 0);
    td1.zone = 0x1000;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      iVar2 = instance->info_slot;
      if (instance->eot_toughness == 1)
      {

        if (0 < iVar2)
        {
          iVar5 = 0;
          do
          {
            iVar4 = validate_target(player, card, &td, iVar5);
            if (iVar4 != 0)
            {
              kill_card(instance->targets[iVar5].player, instance->targets[iVar5].card,
                        KILL_DESTROY);
            }
            iVar5 = iVar5 + 1;
          } while (iVar2 != iVar5);
        }
      }
      else if ((instance->eot_toughness == 2) && (iVar5 = valid_target(&td1), iVar5 != 0))
      {
        gain_life_2022(instance->targets[0].player, iVar2 * 2);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x235e4d0; original cards/theros_beyond_death.c:2294. */
int card_kroxa_titan_of_deaths_hunger(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  test_definition_t test_land;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  new_default_test_definition(&test_land, 1, "Select a land card.");
  test_land.zone = 0x100;
  if (event != EVENT_ETB)
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    if (event == EVENT_DAT_ABILITY)
    {

      iVar2 = 1 - player;
      new_default_test_definition(&this_test, 0xff, "Select a card to discard.");
      iVar1 = new_select_a_card(iVar2, iVar2, 4, 1, 3, -1, &this_test);
      if (iVar1 != -1)
      {
        new_discard_card(iVar2, iVar1, player, 2);
        iVar1 = is_what(iVar2, iVar1, 1);
        if (iVar1 == 0)
        {
          return 0;
        }
      }
      lose_life(iVar2, 3);
    }
    else if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
    {
      iVar1 = escape(player, card, event, 0, 2, 0, 0, 2, 0, 5);
      return iVar1;
    }
    return 0;
  }

  new_default_test_definition(&this_test, 0xff, "Select a card to discard.");
  iVar2 = 1 - player;
  iVar1 = new_select_a_card(iVar2, iVar2, 4, 1, 3, -1, &this_test);
  if (iVar1 != -1)
  {
    new_discard_card(iVar2, iVar1, player, 2);
    iVar1 = is_what(iVar2, iVar1, 1);
    if (iVar1 == 0)
      goto LAB_0235e6c0;
  }
  lose_life(iVar2, 3);
LAB_0235e6c0:
  iVar1 = check_special_flags3_2022(player, card, 0x80000);
  if (iVar1 == 0)
  {
    kill_card(player, card, KILL_SACRIFICE);
  }
  dat_ability(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  return 0;
}

/* ManalinkEh 2022: 0x235eaa0; original cards/theros_beyond_death.c:2613. */
int card_nyleas_intervention(int player, int card, event_t event)
{
  card_instance_t *instance;
  uint32_t uVar2;
  int result;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    get_card_instance(player, card);
    result = generic_spell(player, card, EVENT_CAN_CAST, 4, NULL, NULL, 0, NULL);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (instance = get_card_instance(player, card), card == affected_card)) &&
        (player == affected_card_controller))
    {

      uVar2 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014",
                          "Search for lands", 1, 5, "Damage flyers", 1, 5, 0, 0, 0, 0);
      if (uVar2 == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        generic_spell(player, card, EVENT_CAST_SPELL, 4, NULL, NULL, 0, NULL);
        instance->eot_toughness = uVar2;
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      result = instance->info_slot;
      if ((instance->eot_toughness != 1) || (result < 1))
      {
        if ((instance->eot_toughness == 2) && (result >= 1))
        {
          default_test_definition(&test, 2);
          test.keyword = 0x20;
          new_damage_all(player, card, 2, result * 2, 0, &test);
        }
      }
      else
      {
        new_default_test_definition(&test, 1, "Select a land card");
        test.qty = (unsigned short)result;
        new_global_tutor(player, player, 1, 1, 0, 1, &test);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x235f990; original cards/theros_beyond_death.c:3053. */
int card_purphoross_intervention(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  uint32_t uVar3;
  int iVar4;
  token_generation_t token;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x102);
    get_card_instance(player, card);
    iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 4, NULL, NULL, 0, NULL);
    return iVar2;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x102);
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        iVar2 = can_target(&td);
        uVar3 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\016", "\001\014",
                            "Create a token", 1, 5, "Damage target", iVar2, 5, 0, 0, 0, 0);
        if (uVar3 == 0)
        {
          spell_fizzled = 1;
        }
        else
        {
          if (uVar3 == 1)
          {
            generic_spell(player, card, EVENT_CAST_SPELL, 4, NULL, NULL, 0, NULL);
          }
          else if (uVar3 == 2)
          {
            generic_spell(player, card, EVENT_CAST_SPELL, 5, &td, "TARGET_CREATURE_OR_PLANESWALKER",
                          1, NULL);
          }
          instance->eot_toughness = uVar3;
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x102);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      iVar2 = instance->info_slot;
      if ((instance->eot_toughness != 1) || (iVar2 < 1))
      {
        if ((instance->eot_toughness == 2) &&
            ((iVar2 >= 1 && (iVar4 = valid_target(&td), iVar4 != 0))))
        {
          damage_target0(player, card, iVar2 * 2);
        }
      }
      else
      {
        default_token_definition(player, card, 0x4926, &token);
        token.tou = 1;
        token.key_plus = 0x80;
        token.special_code_on_generation = haste_and_sacrifice_token_at_end_of_next_turn;
        token.pow = iVar2;
        generate_token(&token);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 0x102);
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23596d0; original cards/theros_beyond_death.c:3154. */
static int purphoros_test(int iid, int color, int player, int card)
{
  unsigned int uVar1;
  unsigned int uVar2;
  int result;

  uVar1 = get_color_by_internal_id(player, iid);
  uVar2 = 1;
  if ((color & uVar1) == 0)
  {
    result = is_artifact_creature_by_internal_id(iid, 0, 0, 0);
    uVar2 = (unsigned int)(result != 0);
  }
  return uVar2;
}

/* ManalinkEh 2022: 0x23604d0; original cards/theros_beyond_death.c:3482. */
int card_soul_guide_lantern(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcStack_ec;
  test_definition_t test;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    if ((*graveyard_ptr_mutable[player] == -1) ||
        (iVar1 = graveyard_has_shroud(player), iVar1 != 0))
    {
      iVar1 = count_graveyard_by_type(1 - player, 2);
      if (iVar1 == 0)
      {
        return 0;
      }
      iVar1 = graveyard_has_shroud(1 - player);
      if (iVar1 != 0)
      {
        return 0;
      }
    }
    new_default_test_definition(&test, 0xff, "Select target card.");
    iVar1 = select_target_from_either_grave(player, card, SFG_CANNOT_CANCEL, 3, 1, &test, 0, 1);
    if (iVar1 != -1)
    {

      pcVar3 = get_card_instance(player, card);
      iVar1 = validate_target_from_grave_source(player, card, pcVar3->targets[0].player, 1);
      if (iVar1 != -1)
      {
        rfg_card_from_grave(pcVar3->targets[0].player, iVar1);
      }
    }
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      pcStack_ec = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 0, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        if (iVar1 == 0)
        {
          iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 1, 0, 0, 0, 0, 0,
                                            0, NULL, NULL);
          return (unsigned int)(iVar1 != 0);
        }
        return 1;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        return 0;
      }
      pcStack_ec = get_card_instance(player, card);
    }
    iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 0, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 1, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    if (event == EVENT_ACTIVATE)
    {

      iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Exile opponent's grave", iVar1,
                          5, "Draw a card", iVar2, 5, 0, 0, 0, 0);
      if (iVar1 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar1 == 1)
      {
        iVar1 = 0;
      }
      else
      {
        if (iVar1 != 2)
        {
          return 0;
        }
        iVar1 = 1;
      }
      iVar1 = generic_activated_ability(player, card, EVENT_ACTIVATE, 9, iVar1, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      return iVar1;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      iVar1 = pcStack_ec->info_slot;
      if (iVar1 == 1)
      {
        rfg_whole_graveyard(1 - player);
        iVar1 = pcStack_ec->info_slot;
      }
      if (iVar1 == 2)
      {
        draw_a_card(player);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23609c0; original cards/theros_beyond_death.c:3687. */
int card_thassas_intervention(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  uint32_t uVar4;
  int iVar5;
  int iVar6;
  target_definition_t td_spell;
  test_definition_t this_test;

  if (event == EVENT_CAN_COUNTER)
  {
    if (1 < raw_mana_available[player][2])
    {
      ai_modifier = ai_modifier + 0x18;
    }
  }
  else if (event == EVENT_CAN_CAST)
  {
    counterspell_target_definition(player, card, &td_spell, 0);
    get_card_instance(player, card);
    iVar2 = counterspell(player, card, EVENT_CAN_CAST, &td_spell, -1);
    if (player == 0)
    {
      return (-(unsigned int)(iVar2 == 0) & 0xffffff9e) + 99;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      counterspell_target_definition(player, card, &td_spell, 0);
      instance = get_card_instance(player, card);
      iVar2 = counterspell(player, card, EVENT_CAN_CAST, &td_spell, -1);
      if ((card == affected_card) &&
          ((player == affected_card_controller &&
            (generic_spell(player, card, EVENT_CAST_SPELL, 4, NULL, NULL, 0, NULL),
             spell_fizzled != 1))))
      {
        instance->number_of_targets = '\0';
        iVar3 = check_special_flags2_2022(player, card, -0x80000000);
        if (iVar3 == 0)
        {

          uVar4 =
              dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014", "Counter a spell",
                          iVar2, 1000000, "Browse top of library", 1, 1, 0, 0, 0, 0);
          if (uVar4 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if (uVar4 == 1)
          {
            counterspell(player, card, EVENT_CAST_SPELL, &td_spell, 0);
          }
          instance->eot_toughness = uVar4;
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    counterspell_target_definition(player, card, &td_spell, 0);
    instance = get_card_instance(player, card);
    counterspell(player, card, EVENT_CAN_CAST, &td_spell, -1);
    if (event == EVENT_RESOLVE_SPELL)
    {
      uVar4 = instance->eot_toughness;
      if (uVar4 == 1)
      {
        counterspell_resolve_unless_pay_x(player, card, &td_spell, 0, instance->info_slot * 2);
        uVar4 = instance->eot_toughness;
      }
      if (uVar4 == 2)
      {

        iVar3 = count_deck(player);
        iVar2 = instance->info_slot;
        if (iVar3 < instance->info_slot)
        {
          iVar2 = iVar3;
        }
        if (iVar2 != 0)
        {

          iVar3 = iVar2;
          if (2 < iVar2)
          {
            iVar3 = 2;
          }
          do
          {
            iVar5 = iVar2;
            iVar6 = iVar3;
            new_default_test_definition(&this_test, 0xff, "Select a card to add to your hand");
            this_test.create_minideck = (unsigned short)iVar5;
            this_test.no_shuffle = 1;
            new_global_tutor(player, player, 1, 1, 1, 1, &this_test);
            iVar3 = iVar6 + -1;
            if (iVar3 == 0)
              break;
            iVar2 = iVar5 + -1;
          } while (iVar5 + -1 != 0);
          iVar5 = iVar5 - iVar6;
          if (0 < iVar5)
          {
            put_top_x_on_bottom_in_random_order(player, iVar5);
          }
        }
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    counterspell_target_definition(player, card, &td_spell, 0);
    get_card_instance(player, card);
    counterspell(player, card, EVENT_CAN_CAST, &td_spell, -1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23621b0; original cards/theros_beyond_death.c:4411. */
int card_uro_titan_of_natures_wrath(int player, int card, event_t event)
{
  int result;
  test_definition_t test_land;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  new_default_test_definition(&test_land, 1, "Select a land card.");
  test_land.zone = 0x100;
  if (event == EVENT_ETB)
  {
    gain_life(player, 3);
    draw_a_card(player);
    new_global_tutor(player, player, 4, 4, 0, 1, &test_land);
    result = check_special_flags3_2022(player, card, 0x80000);
    if (result == 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
    dat_ability(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  }
  else
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    if (event == EVENT_DAT_ABILITY)
    {
      gain_life(player, 3);
      draw_a_card(player);
      new_global_tutor(player, player, 4, 4, 0, 1, &test_land);
      return 0;
    }
    if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
    {
      result = escape(player, card, event, 0, 0, 2, 2, 0, 0, 5);
      return result;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int daxos_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2358f60; original cards/theros_beyond_death.c:1003. */
static int daxos_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  unsigned int uVar4;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x51f5))
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
  LAB_02359080:
    uVar4 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 == 0)
      goto joined_r0x023590a5;
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
      goto LAB_02359080;

    pcVar1 = get_card_instance(player, card);
    pcVar2 = in_play(affected_card_controller, affected_card);
    if ((pcVar2 != NULL) &&
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

    for (uVar4 = uVar4 & 0xff; uVar4 != 0; uVar4 = uVar4 - 1)
    {
      gain_life(player, 1);
    }
    kill_card(player, card, KILL_EXILE);
  }
joined_r0x023590a5:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x235bd50; original cards/theros_beyond_death.c:1054. */
int card_daxos_blessed_by_the_sun(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (event == EVENT_TOUGHNESS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar1 = devotion(player, card, 5, 0);
      set_characteristic_pt(iVar1);
    }
    default_test_definition(&test, 2);
    test.not_me = 1;
    etb_ability_global(player, EVENT_TOUGHNESS, 2, TYPE_NONE, &test, player, card);
  }
  else
  {
    default_test_definition(&test, 2);
    test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
    if (event == EVENT_ETB)
    {
      gain_life(player, 1);
    }
    else if (event == EVENT_STATIC_EFFECTS)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {

        if (0 < active_cards_count[player])
        {
          iVar1 = 0;
          do
          {
            instance = in_play(player, iVar1);

            if ((((instance != NULL) && (iVar3 = is_what(player, iVar1, 0x80), iVar3 != 0)) &&
                 (instance = get_card_instance(player, iVar1),
                  instance->info_slot_fn == daxos_effect)) &&
                (((instance->damage_target_player == player &&
                   (instance->damage_target_card == card)) ||
                  ((instance->targets[0].player == player &&
                    (instance->targets[0].card == card))))))
              goto LAB_0235bde8;
            iVar1 = iVar1 + 1;
          } while (iVar1 < active_cards_count[player]);
        }

        iVar1 = create_targetted_legacy_effect(player, card, daxos_effect, player, card);
        instance = get_card_instance(player, iVar1);
        instance->targets[0].player = player;
        instance->targets[0].card = card;
        instance->number_of_targets = '\x01';
        add_status(player, iVar1, 0x10000);
      }
    }
    else if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {
      count_for_gfp_ability(player, card, 0x77, player, 0, &test);
    }
  }
LAB_0235bde8:
  iVar1 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (0 < iVar1)
  {
    do
    {
      gain_life(player, 1);
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x2359220; original cards/theros_beyond_death.c:1616. */
static int erebos_thb_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (iVar3 = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           iVar3 != 0x5211))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (iVar3 = effect_follows_control_of_attachment(player, card, 0x4700), iVar3 != 0))
    {
      return 0;
    }
  LAB_02359348:
    iVar3 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (iVar3 == 0)
      goto joined_r0x02359369;
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (iVar3 = effect_follows_control_of_attachment(player, card, event), iVar3 != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_02359348;

    pcVar1 = get_card_instance(player, card);
    pcVar2 = in_play(affected_card_controller, affected_card);
    if ((pcVar2 != NULL) &&
        (iVar3 = is_this_dying(affected_card_controller, affected_card), iVar3 != 0))
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
        return 0;
      }
      count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
    }
    iVar3 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (iVar3 == 0)
    {
      return 0;
    }
  }
  iVar4 = check_status(player, card, 0x10000);
  if (iVar4 == 0)
  {

    if ((char)iVar3 != '\0')
    {
      do
      {
        while (true)
        {
          iVar3 = can_pay_life(player, 2);
          if (iVar3 == 0)
            goto LAB_023594ad;

          iVar3 =
              dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\003", "\001\014",
                          "Pay 2 life", 1, life[player], "Decline", 1, 5, "Done", 1, 5, 0, 0, 0, 0);
          if (iVar3 != 1)
            break;
          lose_life(player, 2);
          draw_a_card(player);
        }
      } while (iVar3 != 3);
    }
  LAB_023594ad:
    kill_card(player, card, KILL_EXILE);
  }
joined_r0x02359369:
  if ((event == EVENT_CLEANUP) && (iVar3 = check_status(player, card, 0x10000), iVar3 == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2361c60; original cards/theros_beyond_death.c:4170. */
int card_tymaret_chosen_from_death(int player, int card, event_t event)
{
  card_instance_t *instance;
  int grave_id;
  int iVar2;
  unsigned int uVar3;
  unsigned int uVar4;
  target_t *ptVar5;
  int iVar6;
  bool bVar7;
  unsigned char bVar8;
  int iStack_ec;
  test_definition_t test;

  bVar8 = 0;
  check_legend_rule(player, card, event);
  if (event != EVENT_TOUGHNESS)
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      instance = get_card_instance(player, card);
      default_test_definition(&test, 0xff);
      test.ai_selection_mode = 3;
      test.qty = 2;
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar6 = generic_activated_ability_with_target_in_grave(player, card, EVENT_CAN_ACTIVATE,
                                                               GAA_NONE, 1, 1, 0, 0, 0, 0, 0, NULL,
                                                               NULL, TYPE_NONE, &test, 0x2000);
        return iVar6;
      }
      if (event == EVENT_ACTIVATE)
      {
        ptVar5 = instance->targets;
        uVar4 = 0x98;
        bVar7 = ((unsigned int)instance & 1) != 0;
        if (bVar7)
        {
          *(uint8_t *)&instance->targets[0].player = 0xff;
          ptVar5 = (target_t *)((int)&instance->targets[0].player + 1);
          uVar4 = 0x97;
        }
        if (((unsigned int)ptVar5 & 2) != 0)
        {
          *(uint16_t *)&ptVar5->player = 0xffff;
          uVar4 = uVar4 - 2;
          ptVar5 = (target_t *)((int)&ptVar5->player + 2);
        }
        for (uVar3 = uVar4 >> 2; uVar3 != 0; uVar3 = uVar3 - 1)
        {
          ptVar5->player = -1;
          ptVar5 = (target_t *)&ptVar5[-(unsigned int)bVar8].card;
        }
        if ((uVar4 & 2) != 0)
        {
          *(uint16_t *)&ptVar5->player = 0xffff;
          ptVar5 = (target_t *)((int)&ptVar5->player + 2);
        }
        if (bVar7)
        {
          *(uint8_t *)&ptVar5->player = 0xff;
        }
        iVar6 = generic_activated_ability_with_target_in_grave(player, card, EVENT_ACTIVATE,
                                                               GAA_NONE, 1, 1, 0, 0, 0, 0, 0, NULL,
                                                               NULL, TYPE_NONE, &test, 0x2000);
        return iVar6;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        goto LAB_02361dfe;
      instance = get_card_instance(player, card);
      default_test_definition(&test, 0xff);
    }
    test.ai_selection_mode = 3;
    test.qty = 2;
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      return 0;
    }

    iStack_ec = 0;
    iVar6 = 0;
    do
    {

      grave_id = validate_target_from_grave_source(player, card,
                                                   instance->targets[iVar6].player >> 0x10, iVar6);
      if (grave_id != -1)
      {
        iVar2 = is_what(
            -1, graveyard_ptr_mutable[instance->targets[iVar6].player >> 0x10][grave_id], 2);
        iStack_ec = (iStack_ec + 1) - (unsigned int)(iVar2 == 0);
        rfg_card_from_grave(instance->targets[iVar6].player >> 0x10, grave_id);
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 != 2);
    gain_life_2022(player, iStack_ec);
    return 0;
  }
  if (((card == affected_card) && (affected_card_controller == player)) && (player != -1))
  {
    iVar6 = devotion(player, card, 1, 0);
    set_characteristic_pt(iVar6);
  }
LAB_02361dfe:
  iVar6 = global_enchantment(player, card, event);
  return iVar6;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static void elspeth_counters(int s_player, int s_card, int t_player, int t_card);

/* ManalinkEh 2022: 0x235b430; original cards/theros_beyond_death.c:786. */
int find_attached_legacy_effect_not_full(int player, int card, CardFunction func_ptr,
                                         int target_player_id, int target_card)
{
  card_instance_t *instance;
  int card_00;

  if (0 < active_cards_count[player])
  {
    card_00 = 0;
    do
    {
      instance = in_play(player, card_00);
      if (((((instance != NULL) && (instance->internal_card_id == LEGACY_EFFECT_CUSTOM)) &&
            (func_ptr == (CardFunction)instance->info_slot)) &&
           ((instance->damage_source_player == player &&
             (instance->damage_source_card == card)))) &&
          ((instance->damage_target_player == target_player_id &&
            ((instance->damage_target_card == target_card &&
              (instance->targets[0x12].player == -1))))))
      {
        return card_00;
      }
      card_00 = card_00 + 1;
    } while (card_00 < active_cards_count[player]);
  }
  return -1;
}

/* ManalinkEh 2022: 0x2359750; original cards/theros_beyond_death.c:1319. */
static void elspeth_counters(int s_player, int s_card, int t_player, int t_card)
{
  int result;
  counter_t type;

  result = is_planeswalker(t_player, t_card);
  if (result == 0)
  {
    result = is_what(t_player, t_card, 2);
    if (result == 0)
    {
      return;
    }
    type = COUNTER_P1_P1;
  }
  else
  {
    type = COUNTER_LOYALTY;
  }
  hack_silent_counters = 1;
  add_counters_from_source(s_player, s_card, t_player, t_card, type, 1);
  hack_silent_counters = 0;
  return;
}

/* ManalinkEh 2022: 0x235c590; original cards/theros_beyond_death.c:1331. */
int card_elspeth_conquers_death(int player, int card, event_t event)
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

  if (event == EVENT_CAN_SKIP_TURN)
  {
    if (current_turn == player)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
    }
  }
  else if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    if (1 - player == affected_card_controller)
    {
      instance = get_card_instance(player, card);
      if (instance->info_slot == 0x42)
      {
        result = is_what(affected_card_controller, affected_card, 1);
        if (result == 0)
        {
          result = is_what(affected_card_controller, affected_card, 2);
          if (result == 0)
          {
            COST_COLORLESS = COST_COLORLESS + '\x02';
          }
        }
      }
    }
  }
  else if (event == EVENT_CHAPTER1)
  {
    default_target_definition(player, card, &recovered_stack_td.target, 0x47);
    recovered_stack_td.target.allowed_controller = 1 - player;
    recovered_stack_td.target.special = 0x1000000;
    recovered_stack_td.target.extra = 3;
    recovered_stack_td.target.allow_cancel = 0;
    result = can_target(&recovered_stack_td.target);
    if (result != 0)
    {
      result = new_pick_target(
          &recovered_stack_td.target,
          "Select target permanent an opponent controls with converted mana cost 3 or greater.", 0,
          0x81);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
      }
    }
  }
  else if (event == EVENT_CHAPTER2)
  {
    instance = get_card_instance(player, card);
    instance->info_slot = 0x42;
  }
  else if (event == EVENT_CHAPTER3)
  {
    new_default_test_definition(&recovered_stack_td.test, 0x102,
                                "Select target creature or planeswalker card.");
    result = select_target_from_grave_source(player, card, player, SFG_0, 2,
                                             &recovered_stack_td.test, 0);
    if (result != 0)
    {

      result = validate_target_from_grave_source(player, card, player, 0);
      if (result != -1)
      {
        reanimate_permanent_with_function(player, card, player, result, REANIMATE_DEFAULT,
                                          elspeth_counters);
      }
    }
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x235c830; original cards/theros_beyond_death.c:1386. */
int card_elspeths_nightmare(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  ec_definition_t ec;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  if (event == EVENT_CHAPTER1)
  {
    default_target_definition(player, card, &recovered_stack_td.target, 2);
    recovered_stack_td.target.allowed_controller = 1 - player;
    recovered_stack_td.target.power_requirement = 0x2002;
    recovered_stack_td.target.allow_cancel = 0;
    result = can_target(&recovered_stack_td.target);
    if (result != 0)
    {
      result = new_pick_target(&recovered_stack_td.target,
                               "Select target creature an opponent controls with power 2 or less.",
                               0, 0x81);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
    }
  }
  else if (event == EVENT_CHAPTER2)
  {
    result = target_opponent(player, card);
    if (result != 0)
    {
      default_ec_definition(1 - player, player, &ec);
      new_default_test_definition(&recovered_stack_td.test, 3,
                                  "Select a nonland, noncreature card.");
      new_effect_coercion(&ec, &recovered_stack_td.test);
    }
  }
  else if (event == EVENT_CHAPTER3)
  {
    result = target_opponent(player, card);
    if (result != 0)
    {
      rfg_whole_graveyard(1 - player);
    }
  }
  result = saga(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x23616b0; original cards/theros_beyond_death.c:3957. */
int card_the_first_iroan_games(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_CHAPTER1)
  {
    generate_token_by_id_2022(player, card, 0x494e);
  }
  else if (event == EVENT_CHAPTER2)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "TARGET_CREATURE_YOU_CONTROL", 0, 1);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 3);
      }
    }
  }
  else if (event == EVENT_CHAPTER3)
  {
    result = ferocious(player, card);
    if (result != 0)
    {
      draw_cards(player, 2);
    }
  }
  else if (event == EVENT_CHAPTER4)
  {
    generate_token_by_id_2022(player, card, 0x493f);
  }
  result = saga(player, card, event, 4);
  return result;
}

/* ManalinkEh 2022: 0x2361800; original cards/theros_beyond_death.c:3995. */
int card_the_triumph_of_anax(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td0;
  target_definition_t td;

  if (((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CHAPTER1) || (event == EVENT_CHAPTER3))
  {
    default_target_definition(player, card, &td, 2);
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_CREATURE");
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        result = count_counters(player, card, COUNTER_LORE);
        pump_ability_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card,
                               result, 0, 0x80, 0);
      }
    }
  }
  if (event == EVENT_CHAPTER4)
  {

    default_target_definition(player, card, &td0, 2);
    td0.allowed_controller = player;
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = 1 - player;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
    instance->number_of_targets = '\0';
    result = new_pick_target(&td0, "Select target creature you control.", 0, 0x81);
    if (result != 0)
    {
      new_pick_target(&td, "Select target creature opponent controls.", 1, 0x81);
    }
    result = valid_target(&td0);
    if (result != 0)
    {
      result = validate_target(player, card, &td, 1);
      if (result != 0)
      {
        fight(instance->targets[0].player, instance->targets[0].card, instance->targets[1].player,
              instance->targets[1].card);
      }
    }
  }
  result = saga(player, card, event, 4);
  return result;
}

/* ManalinkEh 2022: 0x2361b20; original cards/theros_beyond_death.c:4138. */
int card_tymaret_calls_the_dead(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CHAPTER1)
  {
    mill(player, 3);
    new_default_test_definition(&test, 6, "Select a creature or enchantment card to exile.");
    result = select_target_from_grave_source(player, card, player, SFG_NOTARGET, 3, &test, 0);
    if (result != -1)
    {

      result = validate_target_from_grave_source(player, card, player, 0);
      if (result != -1)
      {
        rfg_card_from_grave(player, result);
        generate_token_by_id(player, card, 0x49ac);
      }
    }
  }
  else if (event == EVENT_CHAPTER3)
  {

    result = count_subtype(player, -1, 0xde);
    gain_life_2022(player, result);
    scry(player, result);
  }
  result = saga(player, card, event, 3);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23611a0; original cards/theros_beyond_death.c:3840. */
int card_the_akroan_war(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int card_00;
  int iVar4;
  target_definition_t td;

  if (event == EVENT_CHAPTER1)
  {

    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    iVar3 = can_target(&td);
    if ((iVar3 != 0) && (iVar3 = pick_target(&td, "TARGET_CREATURE"), iVar3 != 0))
    {
      gain_control_until_source_is_in_play_and_tapped(player, card, instance->targets[0].player,
                                                      instance->targets[0].card,
                                                      GCUS_SOURCE_IN_PLAY);
    }
    instance->number_of_targets = '\0';
  }
  else if (event == EVENT_CHAPTER2)
  {
    iVar3 = create_legacy_effect(player, card, akroan_war_legacy);
    alternate_legacy_text(2, player, iVar3);
  }
  else
  {

    iVar3 = 0;
    if (event == EVENT_CHAPTER3)
    {
      do
      {
        card_00 = 0;
        if (0 < active_cards_count[iVar3])
        {
          do
          {
            while ((iVar1 = is_what(iVar3, card_00, 2),
                    iVar1 == 0 || (iVar1 = is_tapped(iVar3, card_00), iVar1 == 0)))
            {
              card_00 = card_00 + 1;
              if (active_cards_count[iVar3] <= card_00)
                goto LAB_02361240;
            }
            iVar1 = get_power(iVar3, card_00);
            iVar4 = card_00 + 1;
            damage_creature(iVar3, card_00, iVar1, iVar3, card_00);
            card_00 = iVar4;
          } while (iVar4 < active_cards_count[iVar3]);
        }
      LAB_02361240:
        iVar3 = iVar3 + 1;
      } while (iVar3 != 2);
    }
  }
  iVar3 = saga(player, card, event, 3);
  return iVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int medomais_prophecy_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23595a0; original cards/theros_beyond_death.c:2404. */
static int medomais_prophecy_legacy(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t test;

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0xff);
    instance = get_card_instance(player, card);
    test.id = instance->targets[0].card;
    sp_ability_trigger(1 - player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_SPA)
  {
    draw_cards(player, 2);
    kill_card(player, card, KILL_EXILE);
  }
  result = eot_trigger(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x235e870; original cards/theros_beyond_death.c:2423. */
int card_medomais_prophecy(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  test_definition_t test;

  if (event == EVENT_CHAPTER1)
  {
    scry(player, 2);
  }
  else if (event == EVENT_CHAPTER2)
  {
    default_test_definition(&test, 0xff);
    pcVar2 = get_card_instance(player, card);
    result = new_card_from_list(player, 3, &test);
    pcVar2->targets[0].card = result;
  }
  else if (event == EVENT_CHAPTER3)
  {

    result = create_legacy_effect(player, card, medomais_prophecy_legacy);
    pcVar2 = get_card_instance(player, card);
    pcVar3 = get_card_instance(player, result);
    pcVar3->targets[0].card = pcVar2->targets[0].card;
    pcVar2 = get_card_instance(player, card);
    pcVar2->targets[0].card = -1;
  }
  else if (event == EVENT_CHAPTER4)
  {
    show_deck(player, deck_ptr[player], 1, "Here\'s the top card of your library.", 0,
              ENGINE_DONE_LABEL);
    show_deck(player, deck_ptr[1 - player], 1, "Here\'s the top card of your opponent\'s library.",
              0, ENGINE_DONE_LABEL);
  }
  result = saga(player, card, event, 4);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23597f0; original cards/theros_beyond_death.c:3111. */
static int sacrifice_eot(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int player_00;

  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_player)
  {

    instance = get_card_instance(player, card);
    if ((((trigger_condition == 0xcd) && (card == affected_card)) &&
         (player == affected_card_controller)) &&
        (player == reason_for_trigger_controller))
    {
      iVar1 = instance->targets[1].player;
      if (iVar1 < 1)
      {
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
        }
        else if (event == EVENT_RESOLVE_TRIGGER)
        {
          player_00 = (int)instance->damage_target_player;
          iVar1 = instance->damage_target_card;
          iVar3 = check_state(player_00, iVar1, STATE_OUBLIETTED);
          if (iVar3 == 0)
          {
            if (player_00 == instance->targets[0].player)
            {
              kill_card(player_00, iVar1, KILL_SACRIFICE);
            }
          }
          else
          {
            kill_card(player, card, KILL_EXILE);
          }
        }
      }
      else
      {
        instance->targets[1].player = iVar1 + -1;
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x235f6b0; original cards/theros_beyond_death.c:3004. */
int card_polukranos_unchained(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  check_legend_rule(player, card, event);
  result = check_special_flags3_2022(player, card, 0x80000);
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1,
                                       (-(unsigned int)(result == 0) & 0xfffffffa) + 0xc);
  if (event == EVENT_DEAL_DAMAGE)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);

    if (((((result == 0) && (result = count_counters(player, card, COUNTER_P1_P1), result != 0)) &&
          (instance = get_card_instance(affected_card_controller, affected_card),
           instance->internal_card_id == damage_card)) &&
         ((instance->damage_target_card == card && (instance->damage_target_player == player)))) &&
        (0 < instance->info_slot))
    {

      result = count_counters(player, card, COUNTER_P1_P1);
      result = prevent_damage(affected_card_controller, affected_card, result);
      remove_counters_from_source(player, card, player, card, COUNTER_P1_P1, result);
    }
  }
  else
  {
    if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
    {
      result = escape(player, card, event, 4, 1, 0, 1, 0, 0, 6);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      default_target_definition(player, card, &td, 2);
      td.special = 0x4000;
      goto LAB_0235f762;
    }
  }
  if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  td.special = 0x4000;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    instance = get_card_instance(player, card);
    fight(instance->parent_controller, instance->parent_card, instance->targets[0].player,
          instance->targets[0].card);
  }
LAB_0235f762:
  result = generic_activated_ability(player, card, event, 4, 1, 1, 0, 1, 0, 0, 0, &td,
                                     "SELECT_ANOTHER_CREATURE");
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int thb_god(int player, int card, event_t event, int color1, int color2, int amount);

/* ManalinkEh 2022: 0x2358bf0; original cards/theros_beyond_death.c:61. */
static int thb_god(int player, int card, event_t event, int color1, int color2, int amount)
{
  unsigned int uVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  card_instance_t *pcVar5;
  card_instance_t *pcVar6;
  unsigned char bVar7;

  if (((event == EVENT_CAN_CAST) || (event == EVENT_SHOULD_AI_PLAY)) &&
      ((iVar3 = is_what(player, card, 2),
        iVar3 == 0 || (pcVar2 = in_hand(player, card), pcVar2 != NULL))))
  {
    iVar3 = global_enchantment(player, card, event);
    return iVar3;
  }
  if ((event != EVENT_CHANGE_TYPE) &&
      (event != EVENT_CAST_SPELL && event != EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE))
  {
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
  if ((((unsigned int)land_can_be_played >> 16 & 0xff) & 2) != 0)
  {
    return 0;
  }

  pcVar2 = get_card_instance(player, card);
  if (event == EVENT_CAST_SPELL || event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE)
  {
    iVar3 = get_original_internal_card_id(player, card);
    pcVar2->targets[7].card = iVar3;
    iVar3 = devotion(player, card, color1, color2);
    if (iVar3 < amount)
      goto LAB_02358cf2;
    if (event != EVENT_CHANGE_TYPE)
    {
      return 0;
    }
  }
  else
  {
    iVar3 = devotion(player, card, color1, color2);
    if (iVar3 < amount)
    {
    LAB_02358cf2:

      iVar3 = pcVar2->internal_card_id;
      bVar7 = cards_data[iVar3].type & 0xfd;
      if ((cards_data[iVar3].type & 2) == 0)
      {
        return 0;
      }
      if ((pcVar2->targets[7].player != iVar3) ||
          (iVar4 = pcVar2->dummy3, cards_data[iVar4].type != bVar7))
      {

        iVar4 = create_a_card_type(iVar3);
        if (iVar4 == -1)
        {
          return 0;
        }
        cards_data[iVar4].type = bVar7;
        uVar1 = pcVar2->regen_status;
        pcVar2->targets[7].player = iVar3;
        pcVar2->dummy3 = iVar4;
        pcVar2->regen_status = uVar1 | 0x1000000;
        pcVar2->internal_card_id = iVar4;
        pcVar2->backup_internal_card_id = (uint16_t)iVar4;
      }
      if (event == EVENT_CHANGE_TYPE)
      {
        event_result = iVar4;
        return 0;
      }
      return 0;
    }
  }
  pcVar5 = get_card_instance(player, card);
  pcVar6 = get_card_instance(player, card);
  uVar1 = pcVar5->targets[7].card;
  pcVar6->targets[7].player = uVar1;
  pcVar2->internal_card_id = uVar1;
  pcVar2->backup_internal_card_id = (uint16_t)uVar1;
  event_result = uVar1 & 0xffff;
  return 0;
}

/* ManalinkEh 2022: 0x235d270; original cards/theros_beyond_death.c:1683. */
int card_erebos_bleak_hearted(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  indestructible_2022(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar1 = 0;
        do
        {
          instance = in_play(player, iVar1);

          if ((((instance != NULL) && (iVar3 = is_what(player, iVar1, 0x80), iVar3 != 0)) &&
               (instance = get_card_instance(player, iVar1),
                instance->info_slot_fn == erebos_thb_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_0235d2ba;
          iVar1 = iVar1 + 1;
        } while (iVar1 < active_cards_count[player]);
      }

      iVar1 = create_targetted_legacy_effect(player, card, erebos_thb_effect, player, card);
      instance = get_card_instance(player, iVar1);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar1, 0x10000);
    }
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    default_test_definition(&test, 2);
    test.not_me = 1;
    count_for_gfp_ability(player, card, 0x77, player, 0, &test);
  }
LAB_0235d2ba:
  iVar1 = duh_mode(player);
  iVar1 = resolve_gfp_ability(player, card, event, (iVar1 != 0) + RESOLVE_TRIGGER_OPTIONAL);

  if (((iVar1 != 0) && (instance = get_card_instance(player, card), instance->kill_code == '\0')) &&
      ((char)iVar1 != '\0'))
  {
    do
    {
      while (true)
      {
        iVar1 = can_pay_life(player, 2);
        if (iVar1 == 0)
          goto LAB_0235d2e9;

        iVar1 =
            dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\003", "\001\014",
                        "Pay 2 life", 1, life[player], "Decline", 1, 5, "Done", 1, 5, 0, 0, 0, 0);
        if (iVar1 != 1)
          break;
        lose_life(player, 2);
        draw_a_card(player);
      }
    } while (iVar1 != 3);
  }
LAB_0235d2e9:
  if (event == EVENT_ACTIVATE)
  {
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      iVar1 = thb_god(player, card, event, 1, 0, 5);
      return iVar1;
    }
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      pump_until_eot_merge_previous(instance->parent_controller, instance->parent_card,
                                    instance->targets[0].player, instance->targets[0].card, -2, -1);
    }
  }
  default_target_definition(player, card, &td, 2);
  new_default_test_definition(&test, 2, "Select a creature to sacrifice.");
  test.not_me = 1;
  iVar1 = generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_CAN_TARGET, 1,
                                                           1, 0, 0, 0, 0, 0, &td, "TARGET_CREATURE",
                                                           TYPE_NONE, &test, SARV_0);
  return iVar1;
}

/* ManalinkEh 2022: 0x235ddf0; original cards/theros_beyond_death.c:2026. */
int card_heliod_sun_crowned(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  target_definition_t *ptStack_68;
  target_definition_t td;

  check_legend_rule(player, card, event);
  indestructible_2022(player, card, event);
  result = trigger_gain_life(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {

    pcVar2 = get_card_instance(player, card);
    pcVar2->number_of_targets = '\0';
    default_target_definition(player, card, &td, 6);
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.allow_cancel = 0;
    result = can_target(&td);
    if ((result != 0) &&
        (result = new_pick_target(&td, "TARGET_CREATURE_ENCHANTMENT", 0, 1), result != 0))
    {
      add_counters_from_source(player, card, pcVar2->targets[0].player, pcVar2->targets[0].card,
                               COUNTER_P1_P1, 1);
    }
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.special = 0x4000;
    td.preferred_controller = player;
    pcVar2 = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      result = check_for_special_ability(pcVar2->targets[0].player, pcVar2->targets[0].card, 2);
      if ((result == 0) &&
          (((pcVar3 = has_activation_on_stack(player, card),
             pcVar3 == NULL || (pcVar3->targets[0].player != pcVar2->targets[0].player)) ||
            (pcVar3->targets[0].card != pcVar2->targets[0].card))))
      {
        ai_modifier = ai_modifier + 0x28;
      }
      else
      {
        ai_modifier = ai_modifier + -0x28;
      }
      goto LAB_0235dea8;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = thb_god(player, card, event, 5, 0, 5);
      return result;
    }
    default_target_definition(player, card, &td, 2);
    td.special = 0x4000;
    td.preferred_controller = player;
    pcVar2 = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    pump_ability_until_eot_no_repeat(pcVar2->parent_controller, pcVar2->parent_card,
                                     pcVar2->targets[0].player, pcVar2->targets[0].card, 0, 2);
  }
LAB_0235dea8:
  ptStack_68 = &td;
  result = generic_activated_ability(player, card, event, 4, 1, 0, 0, 0, 0, 1, 0, ptStack_68,
                                     "TARGET_ANOTHER_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x235e310; original cards/theros_beyond_death.c:2259. */
int card_klothys_god_of_destiny(int player, int card, event_t event)
{
  int player_00;
  int result;
  card_instance_t *instance;
  unsigned char bStack_ec;
  test_definition_t test;

  check_legend_rule(player, card, event);
  indestructible_2022(player, card, event);
  result = beginning_of_phase(player, card, event, player, card, 0x14);
  if (result != 0)
  {
    new_default_test_definition(&test, 0xff, "Select target card to exile.");
    result = select_target_from_either_grave(player, card, SFG_0, 3, 1, &test, 0, 1);
    if (result != -1)
    {

      instance = get_card_instance(player, card);
      result = validate_target_from_grave_source(player, card, instance->targets[0].player, 1);
      if (result != -1)
      {

        player_00 = instance->targets[0].player;
        bStack_ec = cards_data[graveyard_ptr_mutable[player_00][result]].type & 1;
        rfg_card_from_grave(player_00, result);
        if (bStack_ec == 0)
        {
          gain_life(player, 2);
          damage_player(1 - player, 2, player, card);
        }
        else
        {
          produce_mana_all_one_color(player, COLOR_TEST_RED | COLOR_TEST_GREEN, 1);
        }
      }
    }
  }
  result = thb_god(player, card, event, 4, 3, 7);
  return result;
}

/* ManalinkEh 2022: 0x235ed80; original cards/theros_beyond_death.c:2663. */
int card_nylea_keen_eyed(int player, int card, event_t event)
{
  int *deck;
  int result;

  check_legend_rule(player, card, event);
  indestructible_2022(player, card, event);
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    if ((affected_card_controller == player) &&
        (result = is_what(player, affected_card, 2), result != 0))
    {
      COST_COLORLESS = COST_COLORLESS + -1;
    }
  LAB_0235ef0c:
    result = thb_god(player, card, event, 3, 0, 5);
    return result;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_0235edc8:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_0235edcd;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
      goto LAB_0235ef0c;
    goto LAB_0235edc8;
  }

  deck = deck_ptr[player];
  if (*deck != -1)
  {

    show_deck(current_turn, deck, 1, "Here\'s the first card of deck.", 0, ENGINE_DONE_LABEL);
    show_deck(current_turn ^ 1, deck, 1, "Here\'s the first card of deck.", 0, ENGINE_DONE_LABEL);
    result = is_what(-1, *deck, 2);
    if (result == 0)
    {
      result = dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\002", "\001\003",
                           "\001\014", "\001\021", *deck, "Put the card in your graveyard,", 1, 5,
                           "Pass", 1, 5, 0, 0, 0, 0);
      if (result == 1)
      {
        mill(player, 1);
      }
    }
    else
    {
      add_card_to_hand(player, *deck);
      remove_card_from_deck(player, 0);
    }
  }
LAB_0235edcd:
  result = generic_activated_ability(player, card, event, 0, 2, 0, 0, 1, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x235f370; original cards/theros_beyond_death.c:2870. */
int card_one_with_the_stars(int player, int card, event_t event)
{
  unsigned char bVar1;
  int argument1;
  card_data_t *pcVar2;
  unsigned char bVar3;
  card_instance_t *pcVar4;
  int iVar5;
  card_instance_t *pcVar6;
  int iVar7;
  int player_00;
  target_definition_t td;

  pcVar4 = in_play(player, card);
  if (((pcVar4 == NULL) ||
       (pcVar4 = get_card_instance(player, card), pcVar4->damage_target_player < '\0')) ||
      (iVar5 = is_humiliated_for_ability(player, card, AL_STATIC), iVar5 != 0))
  {
  LAB_0235f3a5:

    if (event == EVENT_CAN_CAST)
      goto LAB_0235f3c8;
    iVar7 = event_result;
    if (event != EVENT_CAST_SPELL)
      goto LAB_0235f3b1;
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_0235f3c8;
  }
  else
  {
    pcVar4 = get_card_instance(player, card);
    player_00 = (int)pcVar4->damage_target_player;
    iVar5 = pcVar4->damage_target_card;
    pcVar4 = get_card_instance(player_00, iVar5);
    if (event != EVENT_CHANGE_TYPE)
    {
      if (event == EVENT_RESOLVE_SPELL)
      {
        pcVar4->regen_status = pcVar4->regen_status | 0x1000000;
        goto LAB_0235f3c8;
      }
      goto LAB_0235f3a5;
    }
    pcVar6 = in_play(player, card);
    iVar7 = event_result;
    if (((pcVar6 != NULL) &&
         ((iVar5 == affected_card && (player_00 == affected_card_controller)))) &&
        ((((unsigned int)land_can_be_played >> 16 & 0xff) & 2) == 0))
    {
      argument1 = pcVar4->internal_card_id;
      bVar1 = cards_data[argument1].type;
      bVar3 = bVar1 & 0xb8 | 4;
      iVar5 = is_planeswalker(player_00, iVar5);
      if ((bVar1 == bVar3) && (iVar5 == 0))
      {
        return 0;
      }
      if ((pcVar4->targets[7].player != argument1) ||
          (iVar7 = pcVar4->dummy3, cards_data[pcVar4->dummy3].type != bVar3))
      {
        iVar7 = create_a_card_type(argument1);
        if (iVar7 == -1)
        {
          return 0;
        }
        pcVar2 = cards_at_7c7000[iVar7];
        pcVar2->type = bVar3;
        if (iVar5 != 0)
        {
          pcVar2->cc[2] = '\0';
        }
        pcVar4->regen_status = pcVar4->regen_status | 0x1000000;
        pcVar4->targets[7].player = argument1;
        pcVar4->dummy3 = iVar7;
      }
    }
  LAB_0235f3b1:
    event_result = iVar7;
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
      goto LAB_0235f3c8;
  }
  if (((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
        (2 < event - EVENT_POWER)) &&
       ((event != EVENT_RESOLVE_MOVING_AURA &&
         ((event != EVENT_CHANGE_TYPE && (event != EVENT_SET_COLOR)))))) &&
      ((event != EVENT_GET_SELECTED_CARD && (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_0235f3c8:
  default_target_definition(player, card, &td, 6);
  iVar5 = disabling_targeted_aura(player, card, event, &td,
                                  "Select target creature or enchantment.", 0);
  return iVar5;
}

/* ManalinkEh 2022: 0x235fd40; original cards/theros_beyond_death.c:3162. */
int card_purphoros_bronze_blooded(int player, int card, event_t event)
{
  int player_00;
  int result;
  const char *msg;
  card_instance_t *instance;
  bool bVar3;
  test_definition_t test;

  check_legend_rule(player, card, event);
  indestructible_2022(player, card, event);
  boost_subtype(player, card, event, ~SUBTYPE_INVALID, 0, 0, 0, 4, BCT_CONTROLLER_ONLY);
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = thb_god(player, card, event, 4, 0, 5);
      return result;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      msg = get_sleighted_color_text(player, card,
                                     "Select a %s creature card or artifact creature card", 4);
      new_default_test_definition(&test, 2, msg);
      test.special_selection_function = purphoros_test;
      test.value_for_special_selection_function = get_sleighted_color_test(player, card, 0x10);
      result = new_global_tutor(player, player, 4, 4, 0, 2, &test);
      if (-1 < result)
      {

        instance = get_card_instance(player, card);

        player_00 = instance->parent_controller;
        result = create_targetted_legacy_effect(player_00, instance->parent_card, sacrifice_eot,
                                                player_00, result);
        instance = get_card_instance(player_00, result);
        bVar3 = current_phase == 0x1f;
        instance->number_of_targets = '\0';
        if (bVar3)
        {
          if (-1 < result)
          {
            instance = get_card_instance(player_00, result);
            instance->targets[1].player = 1;
          }
        }
      }
    }
  }
  result = generic_activated_ability(player, card, event, 0, 2, 0, 0, 0, 1, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2360f60; original cards/theros_beyond_death.c:3780. */
int card_thassa_deep_dwelling(int player, int card, event_t event)
{
  int result;
  resolve_trigger_t trig_mode;
  card_instance_t *instance;
  target_definition_t td;

  check_legend_rule(player, card, event);
  indestructible_2022(player, card, event);
  if ((((trigger_condition == 0xcd) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (current_turn == player))
  {

    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.special = 0x4000;
    instance = get_card_instance(player, card);
    instance->number_of_targets = '\0';
    result = can_target(&td);
    trig_mode = RESOLVE_TRIGGER_0;
    if (result != 0)
    {
      result = duh_mode(player);
      trig_mode = (result != 0) + RESOLVE_TRIGGER_OPTIONAL;
    }
    result = eot_trigger_mode(player, card, event, player, trig_mode);
    if (result != 0)
    {
      result = pick_target(&td, "ASHNODS_BATTLEGEAR");
      if (result != 0)
      {
        blink_effect_controller(instance->targets[0].player, instance->targets[0].card, NULL);
      }
    }
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = thb_god(player, card, event, 2, 0, 5);
      return result;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      instance = get_card_instance(player, card);
      tap_card(instance->targets[0].player, instance->targets[0].card);
    }
  }
  default_target_definition(player, card, &td, 2);
  td.special = 0x4000;
  result = generic_activated_ability(player, card, event, 4, 3, 0, 1, 0, 0, 0, 0, &td,
                                     "TARGET_ANOTHER_CREATURE");
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2362420; original cards/theros_beyond_death.c:4506. */
int counter_activated_ability_resolve_unless_pay_x(int player, int card, target_definition_t *td,
                                                   int target_num, int x)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  char options[40];

  iVar1 = validate_counter_activated_ability(player, card, td, target_num);
  iVar2 = 0;

  if ((iVar1 == 0) || (iVar2 = 1, x < 1))
  {
    return iVar2;
  }
  sprintf(options, " Pay %d\n Decline", x);
  instance = get_card_instance(player, card);
  iVar2 = has_mana(instance->targets[target_num].player, 7, x);
  if ((iVar2 != 0) &&
      ((iVar2 = instance->targets[target_num].player,
        iVar2 =
            do_dialog(iVar2, player, card, iVar2, instance->targets[target_num].card, options, 0),
        iVar2 == 0 && (iVar2 = charge_mana_while_resolving(player, card, EVENT_RESOLVE_SPELL,
                                                           instance->targets[target_num].player,
                                                           COLOR_COLORLESS, x),
                       iVar2 != 0))))
  {
    return 2;
  }
  raw_counter_activated_ability(instance->targets[target_num].player,
                                instance->targets[target_num].card);
  return 1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int anax_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23582f0; original cards/theros_beyond_death.c:219. */
static int anax_effect(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iStack_ec;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      if ((-1 < instance->damage_target_player) &&
          (iVar2 = get_id((int)instance->damage_target_player, instance->damage_target_card),
           iVar2 != 0x51d8))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    instance = get_card_instance(player, card);
    if ((-1 < instance->damage_target_player) &&
        (iVar2 = effect_follows_control_of_attachment(player, card, 0x4700), iVar2 != 0))
    {
      return 0;
    }
  LAB_02358328:
    iVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
      goto LAB_02358346;
  }
  else
  {
    instance = get_card_instance(player, card);
    if ((-1 < instance->damage_target_player) &&
        (iVar2 = effect_follows_control_of_attachment(player, card, event), iVar2 != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_02358328;
    instance = in_play(affected_card_controller, affected_card);

    if (((instance != NULL) && (affected_card_controller == player)) &&
        (instance = get_card_instance(player, affected_card), instance->kill_code != '\0'))
    {

      instance = get_card_instance(player, card);
      if (instance->damage_target_player < '\0')
      {
      LAB_023586d8:
        iStack_ec = affected_card_controller;
      }
      else
      {

        iVar2 = instance->damage_target_card;
        iStack_ec = affected_card_controller;
        if ((iVar2 == affected_card) &&
            (iVar3 = (int)instance->damage_target_player, iVar3 == affected_card_controller))
        {
          iVar4 = is_what(iVar3, iVar2, 2);
          if ((iVar4 == 0) && ((iVar4 = is_this_dying(iVar3, iVar2),
                                iVar4 == 0 && (iVar2 = is_token(iVar3, iVar2), iVar2 == 0))))
          {
            if (instance->targets[0xb].player < 0)
            {
              instance->targets[0xb].player = 0;
            }
            *(uint8_t *)&instance->targets[0xb].player =
                (char)instance->targets[0xb].player + card_counts[player][5] + '\x01';
          }
          instance->damage_target_card = -1;
          instance->damage_target_player = -1;
          remove_status(player, card, 0x10000);
          goto LAB_023586d8;
        }
      }
      affected_card_controller = iStack_ec;
      iVar2 = is_what(iStack_ec, affected_card, 2);
      if (((iVar2 != 0) &&
           (iVar2 = is_this_dying(affected_card_controller, affected_card), iVar2 != 0)) &&
          (iVar2 = is_token(affected_card_controller, affected_card), iVar2 == 0))
      {
        default_test_definition(&recovered_stack_test.test, 2);
        recovered_stack_test.test.type_flag = 3;
        count_for_gfp_ability(player, card, 0x77, player, -1, &recovered_stack_test.test);
        iVar2 = get_power(affected_card_controller, affected_card);
        if (3 < iVar2)
        {
          instance = get_card_instance(player, card);
          instance->targets[0xb].player =
              instance->targets[0xb].player + card_counts[player][5] + 1;
        }
      }
    }
    iVar2 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
    {
      return 0;
    }
  }
  iVar3 = check_status(player, card, 0x10000);
  if (iVar3 == 0)
  {
    default_token_definition(player, card, 0x497e, &recovered_stack_test.token);
    recovered_stack_test.test.message[0x2c] = 'B';
    recovered_stack_test.test.message[0x2d] = '\0';
    recovered_stack_test.test.message[0x2e] = '\0';
    recovered_stack_test.test.message[0x2f] = '\0';
    recovered_stack_test.test.message[0x1c] = '\x01';
    recovered_stack_test.test.message[0x1d] = '\0';
    recovered_stack_test.test.message[0x1e] = '\0';
    recovered_stack_test.test.message[0x1f] = '\0';
    recovered_stack_test.test.message[0x18] = '\x01';
    recovered_stack_test.test.message[0x19] = '\0';
    recovered_stack_test.test.message[0x1a] = '\0';
    recovered_stack_test.test.message[0x1b] = '\0';
    recovered_stack_test.test.message[0x2b] = (char)iVar2;
    generate_token(&recovered_stack_test.token);
    kill_card(player, card, KILL_EXILE);
  }
LAB_02358346:
  if ((event == EVENT_CLEANUP) &&
      (instance = get_card_instance(player, card), instance->damage_target_player == -1))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x235a7d0; original cards/theros_beyond_death.c:285. */
int card_anax_hardened_in_the_forge(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_test;

  check_legend_rule(player, card, event);
  if (event == EVENT_POWER)
  {
    if (((card == affected_card) && (affected_card_controller == player)) && (player != -1))
    {
      iVar2 = devotion(player, card, 4, 0);
      set_characteristic_pt(iVar2);
    }
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
                instance->info_slot_fn == anax_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_0235a814;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, anax_effect, player, card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  }
  else if ((event == EVENT_GRAVEYARD_FROM_PLAY) &&
           (instance = in_play(affected_card_controller, affected_card), instance != NULL))
  {
    default_test_definition(&recovered_stack_test.test, 2);
    recovered_stack_test.test.type_flag = 3;
    count_for_gfp_ability(player, card, 0x77, player, -1, &recovered_stack_test.test);
    iVar2 = get_power(affected_card_controller, affected_card);
    if (3 < iVar2)
    {
      instance = get_card_instance(player, card);
      instance->targets[0xb].player = instance->targets[0xb].player + card_counts[player][5] + 1;
    }
  }
LAB_0235a814:
  instance = get_card_instance(player, card);

  if (((instance->kill_code == '\0') &&
       (iVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY), iVar2 != 0)) &&
      ((instance = in_play(player, card),
        instance != NULL && (instance = in_play(player, card), instance->kill_code == '\0'))))
  {
    default_token_definition(player, card, 0x497e, &recovered_stack_test.token);
    recovered_stack_test.test.message[0x2c] = 'B';
    recovered_stack_test.test.message[0x2d] = '\0';
    recovered_stack_test.test.message[0x2e] = '\0';
    recovered_stack_test.test.message[0x2f] = '\0';
    recovered_stack_test.test.message[0x1c] = '\x01';
    recovered_stack_test.test.message[0x1d] = '\0';
    recovered_stack_test.test.message[0x1e] = '\0';
    recovered_stack_test.test.message[0x1f] = '\0';
    recovered_stack_test.test.message[0x18] = '\x01';
    recovered_stack_test.test.message[0x19] = '\0';
    recovered_stack_test.test.message[0x1a] = '\0';
    recovered_stack_test.test.message[0x1b] = '\0';
    recovered_stack_test.test.message[0x2b] = (char)iVar2;
    generate_token(&recovered_stack_test.token);
  }
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2358e10; original cards/theros_beyond_death.c:752. */
static int calix_obliviation_legacy(int player, int card, event_t event)
{
  int player_00;
  int iid;
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_t *ptVar4;

  instance = get_card_instance(player, card);
  other_leaves_play_2022(player, card, (int)instance->damage_target_player,
                         instance->damage_target_card, event, 0, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    ptVar4 = instance->targets;
    do
    {
      player_00 = ptVar4->player;
      if (player_00 == -1)
      {
        return 0;
      }

      iid = ptVar4->card;
      iVar2 = check_rfg(player_00, (unsigned int)cards_data[iid].id);
      if (iVar2 != 0)
      {

        iVar2 = add_card_to_hand(player_00, iid);
        iVar3 = has_subtype(player_00, iVar2, SUBTYPE_AURA);
        if (iVar3 != 0)
        {
          iVar3 = call_card_function(player_00, iVar2, EVENT_CAN_CAST);
          if (iVar3 == 0)
          {
            obliterate_card(player_00, iVar2);
            goto LAB_02358e88;
          }
          set_special_flags3_2022(player_00, iVar2, 0x40);
        }
        remove_card_from_rfg(player_00, (unsigned int)cards_data[iid].id);
        put_into_play(player_00, iVar2);
      }
    LAB_02358e88:

      ptVar4 = ptVar4 + 1;
    } while (ptVar4 != instance->targets + 0x12);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int athreos_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2358760; original cards/theros_beyond_death.c:532. */
static int athreos_effect(int player, int card, event_t event)
{
  enum
  {
    COUNTER_COIN = 142
  };

  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  target_t *ptVar6;
  unsigned int uVar7;
  event_t event_00;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (iVar3 = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           iVar3 != 0x51e4))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if (-1 < pcVar1->damage_target_player)
    {
      iVar3 = effect_follows_control_of_attachment(player, card, 0x4700);
      if (iVar3 != 0)
      {
        return 0;
      }
      goto LAB_02358981;
    }
    event_00 = EVENT_STATIC_EFFECTS;
  LAB_02358860:
    iVar3 = resolve_gfp_ability_2022(player, card, event_00, RESOLVE_TRIGGER_MANDATORY);
    iVar4 = check_status(player, card, 0x10000);
    if (iVar4 != 0)
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
    if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {

      pcVar1 = get_card_instance(player, card);
      pcVar2 = in_play(affected_card_controller, affected_card);

      if ((pcVar2 != NULL) && (pcVar2 = get_card_instance(affected_card_controller, affected_card),
                               pcVar2->kill_code != '\0'))
      {

        if ((-1 < pcVar1->damage_target_player) &&
            ((pcVar1->damage_target_card == affected_card &&
              (pcVar1->damage_target_player == affected_card_controller))))
        {
          pcVar1->damage_target_card = -1;
          pcVar1->damage_target_player = -1;
          remove_status(player, card, 0x10000);
        }
        iVar3 = is_what(affected_card_controller, affected_card, 2);
        if (((iVar3 != 0) &&
             (iVar3 = count_counters(affected_card_controller, affected_card, COUNTER_COIN),
              iVar3 != 0)) &&
            (iVar3 = is_token(affected_card_controller, affected_card), iVar3 == 0))
        {

          iVar3 = is_this_dying(affected_card_controller, affected_card);
          if ((iVar3 != 0) &&
              (iVar3 = check_for_death_trigger_removal(affected_card_controller, affected_card),
               iVar3 != 0))
          {
            return 0;
          }
          uVar7 = pcVar1->targets[0xb].player;
          if ((int)uVar7 < 0)
          {
            pcVar1->targets[0xb].player = 0;
            uVar7 = 0;
          LAB_02358b7a:
            iVar3 = get_owner(affected_card_controller, affected_card);
            *(char *)&pcVar1->targets[uVar7].player = (char)iVar3;
            iVar3 = is_this_dying(affected_card_controller, affected_card);
            if (iVar3 == 0)
            {
              *(uint8_t *)((int)&pcVar1->targets[uVar7].player + 1) = 4;
            }
            iVar3 = get_original_internal_card_id(affected_card_controller, affected_card);
            pcVar1->targets[uVar7].card = iVar3;
          }
          else
          {
            uVar7 = uVar7 & 0xff;
            if (uVar7 < 10)
              goto LAB_02358b7a;
          }
          *(char *)&pcVar1->targets[0xb].player = (char)pcVar1->targets[0xb].player + '\x01';
        }
      }
      event_00 = EVENT_GRAVEYARD_FROM_PLAY;
      goto LAB_02358860;
    }
  LAB_02358981:
    iVar3 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    iVar4 = check_status(player, card, 0x10000);
    if (iVar4 != 0)
      goto joined_r0x023589c4;
  }

  pcVar1 = get_card_instance(player, card);
  if (1 < iVar3)
  {
    ptVar6 = pcVar1->targets + 1;
    do
    {
      iVar4 = ptVar6->card;
      if ((char)((unsigned int)ptVar6->player >> 8) == '\x04')
      {
        iVar5 = check_rfg(player, (unsigned int)cards_data[iVar4].id);
        if (iVar5 != 0)
        {

          iVar4 = add_card_to_hand(player, iVar4);
          put_into_play(player, iVar4);
        }
      }
      else
      {

        seek_grave_for_id_to_reanimate(player, -1, ptVar6->player & 0xff,
                                       (unsigned int)cards_data[iVar4].id, REANIMATE_DEFAULT);
      }
      ptVar6->card = -1;
      ptVar6->player = -1;
      ptVar6 = ptVar6 + 1;
    } while (ptVar6 != pcVar1->targets + iVar3);
  }
  kill_card(player, card, KILL_EXILE);
joined_r0x023589c4:
  if ((event == EVENT_CLEANUP) && (iVar3 = check_status(player, card, 0x10000), iVar3 == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x235afb0; original cards/theros_beyond_death.c:609. */
int card_athreos_shroud_veiled(int player, int card, event_t event)
{
  enum
  {
    COUNTER_COIN = 142
  };

  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  target_t *ptVar5;
  unsigned int uVar6;
  target_definition_t td;

  check_legend_rule(player, card, event);
  indestructible_2022(player, card, event);
  iVar1 = eot_trigger(player, card, event);
  if ((iVar1 != 0) && (current_turn == player))
  {
    default_target_definition(player, card, &td, 2);
    td.special = 0x4000;
    iVar1 = can_target(&td);
    if ((iVar1 != 0) && (iVar1 = pick_target(&td, "TARGET_ANOTHER_CREATURE"), iVar1 != 0))
    {

      instance = get_card_instance(player, card);
      add_counters_from_source(player, card, instance->targets[0].player, instance->targets[0].card,
                               COUNTER_COIN, 1);
      instance->number_of_targets = '\0';
    }
  }
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
                instance->info_slot_fn == athreos_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_0235b043;
          iVar1 = iVar1 + 1;
        } while (iVar1 < active_cards_count[player]);
      }

      iVar1 = create_targetted_legacy_effect(player, card, athreos_effect, player, card);
      instance = get_card_instance(player, iVar1);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar1, 0x10000);
    }
    goto LAB_0235b043;
  }
  if (((event != EVENT_GRAVEYARD_FROM_PLAY) ||
       (iVar1 = is_what(affected_card_controller, affected_card, 2), iVar1 == 0)) ||
      ((iVar1 = count_counters(affected_card_controller, affected_card, COUNTER_COIN),
        iVar1 == 0 || (iVar1 = is_token(affected_card_controller, affected_card), iVar1 != 0))))
    goto LAB_0235b043;

  iVar1 = is_this_dying(affected_card_controller, affected_card);
  if ((iVar1 != 0) &&
      (iVar1 = check_for_death_trigger_removal(affected_card_controller, affected_card),
       iVar1 != 0))
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  uVar6 = instance->targets[0xb].player;
  if ((int)uVar6 < 0)
  {
    instance->targets[0xb].player = 0;
    uVar6 = 0;
  LAB_0235b3c3:
    iVar1 = get_owner(affected_card_controller, affected_card);
    *(char *)&instance->targets[uVar6].player = (char)iVar1;
    iVar1 = is_this_dying(affected_card_controller, affected_card);
    if (iVar1 == 0)
    {
      *(uint8_t *)((int)&instance->targets[uVar6].player + 1) = 4;
    }
    iVar1 = get_original_internal_card_id(affected_card_controller, affected_card);
    instance->targets[uVar6].card = iVar1;
  }
  else
  {
    uVar6 = uVar6 & 0xff;
    if (uVar6 < 10)
      goto LAB_0235b3c3;
  }
  *(char *)&instance->targets[0xb].player = (char)instance->targets[0xb].player + '\x01';
LAB_0235b043:
  instance = get_card_instance(player, card);
  if (instance->kill_code == '\0')
  {

    instance = get_card_instance(player, card);
    iVar1 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

    if (0 < iVar1)
    {
      ptVar5 = instance->targets;
      do
      {
        iVar4 = ptVar5->card;
        if ((char)((unsigned int)ptVar5->player >> 8) == '\x04')
        {
          iVar3 = check_rfg(player, (unsigned int)cards_data[iVar4].id);
          if (iVar3 != 0)
          {

            iVar4 = add_card_to_hand(player, iVar4);
            put_into_play(player, iVar4);
          }
        }
        else
        {

          seek_grave_for_id_to_reanimate(player, -1, ptVar5->player & 0xff,
                                         (unsigned int)cards_data[iVar4].id, REANIMATE_DEFAULT);
        }
        ptVar5->card = -1;
        ptVar5->player = -1;
        ptVar5 = ptVar5 + 1;
      } while (ptVar5 != instance->targets + iVar1);
    }
  }
  iVar1 = thb_god(player, card, event, 5, 1, 7);
  return iVar1;
}

/* ManalinkEh 2022: 0x2361a10; original cards/theros_beyond_death.c:4071. */
int card_towering_wave_mystic(int player, int card, event_t event)
{
  enum
  {
    DDBM_MAY_DAMAGE_PLANESWALKER = 64
  };

  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t td;

  check_damage(player, card, event, DDBM_MAY_DAMAGE_PLANESWALKER | DDBM_REPORT_DAMAGE_DEALT, player,
               card);
  iVar1 = resolve_damage_trigger(
      player, card, event, DDBM_MAY_DAMAGE_PLANESWALKER | DDBM_REPORT_DAMAGE_DEALT, player, card);
  if (iVar1 == 0)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  iVar1 = instance->targets[0x10].player;
  instance->targets[0x10].player = 0;
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  td.allow_cancel = 0;
  iVar3 = can_target(&td);
  if (iVar3 != 0)
  {
    iVar3 = new_pick_target(&td, "TARGET_PLAYER", 0, 0);
    if (iVar3 != 0)
    {
      mill(instance->targets[0].player, iVar1);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int is_basic_land_by_internal_id(int iid, int unused, int unused1, int unused2);

/* ManalinkEh 2022: 0x2359730; original cards/theros_beyond_death.c:3921. */
static int is_basic_land_by_internal_id(int iid, int unused, int unused1, int unused2)
{
  int result;

  result = is_basic_land_by_id((unsigned int)cards_data[iid].id);
  return result;
}

/* ManalinkEh 2022: 0x2361560; original cards/theros_beyond_death.c:3924. */
int card_the_birth_of_meletis(int player, int card, event_t event)
{
  int result;
  const char *msg;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_this_test;

  if (event == EVENT_CHAPTER1)
  {
    msg = get_hacked_land_text(player, card, "Select a basic %s card.", 0x5007);
    new_default_test_definition(&recovered_stack_this_test.test, 1, msg);
    recovered_stack_this_test.test.subtype = get_hacked_subtype(player, card, 0x5007);
    recovered_stack_this_test.test.special_selection_function = is_basic_land_by_internal_id;
    new_global_tutor(player, player, 1, 1, 0, 5, &recovered_stack_this_test.test);
  }
  else if (event == EVENT_CHAPTER2)
  {
    default_token_definition(player, card, 0x49a3, &recovered_stack_this_test.token);
    recovered_stack_this_test.test.message[0x18] = '\0';
    recovered_stack_this_test.test.message[0x19] = '\0';
    recovered_stack_this_test.test.message[0x1a] = '\0';
    recovered_stack_this_test.test.message[0x1b] = '\0';
    recovered_stack_this_test.test.message[0x1c] = '\x04';
    recovered_stack_this_test.test.message[0x1d] = '\0';
    recovered_stack_this_test.test.message[0x1e] = '\0';
    recovered_stack_this_test.test.message[0x1f] = '\0';
    generate_token(&recovered_stack_this_test.token);
  }
  else if (event == EVENT_CHAPTER3)
  {
    gain_life(player, 2);
  }
  result = saga(player, card, event, 3);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x235d7b0; original cards/theros_beyond_death.c:1911. */
int card_haktos_the_unscarred(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  char buffer[100];

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    instance = get_card_instance(player, card);
    result = internal_rand(2);
    instance->info_slot = result + 2;
    sprintf(buffer, " Haktos is susceptible\n to mana cost: %d ", result + 2);
    do_dialog(player, player, card, -1, -1, buffer, 0);
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result != 0)
    {
      special_abilities2(player, card, EVENT_ETB, SP_KEYWORD2_PROT_MISC);
      return 0;
    }
  }
  else
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result != 0)
      goto LAB_0235d815;
  }
  attack_if_able_2022(player, card, event);
LAB_0235d815:
  special_abilities2(player, card, event, SP_KEYWORD2_PROT_MISC);
  if (((event == EVENT_PROT_MISC) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    if (((unsigned int)event_result >> 24 & 0xff) != '\x06')
    {
      result = get_cmc(attacking_card_controller, attacking_card);
      instance = get_card_instance(player, card);
      if (result == instance->info_slot)
      {
        return 0;
      }
    }
    event_result = event_result | 1;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x235c210; original cards/theros_beyond_death.c:1205. */
int card_dream_trawler(int player, int card, event_t event)
{
  int t_card;
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  lifelink(player, card, event);
  iVar1 = card_drawn_trigger(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
  if (iVar1 != 0)
  {
    pump_until_eot_merge_previous(player, card, player, card, 1, 0);
  }
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    draw_a_card(player);
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_0235c399:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (((event == EVENT_STATIC_EFFECTS) &&
           (instance = get_card_instance(player, card), instance->info_slot == 0x42)) &&
          (stack_size == 0))
      {
        instance = get_card_instance(player, card);
        instance->info_slot = 0;
      }
      goto LAB_0235c3ae;
    }
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0x42;
      goto LAB_0235c3ae;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_0235c399;
    }
  }
  instance = get_card_instance(player, card);
  iVar1 = instance->parent_controller;
  t_card = instance->parent_card;
  pump_ability_until_eot_no_repeat(player, card, iVar1, t_card, 0, (SP_KEYWORD_HEXPROOF));
  tap_card(iVar1, t_card);
  instance = get_card_instance(iVar1, t_card);
  if (instance->info_slot == 0x42)
  {
    ai_modifier = ai_modifier + -0x14;
  }
  iVar3 = check_for_special_ability(iVar1, t_card, (SP_KEYWORD_HEXPROOF));
  if (iVar3 != 0)
  {
    ai_modifier = ai_modifier + -0x32;
  }
  instance = get_card_instance(iVar1, t_card);
  instance->info_slot = 0;
LAB_0235c3ae:
  iVar1 = generic_activated_ability(player, card, event, 0x20, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x235f0e0; original cards/theros_beyond_death.c:2732. */
int card_nyxbloom_ancient(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = in_play(player, card);
  if ((instance != NULL) &&
      (result = is_humiliated_for_ability(player, card, AL_STATIC), result != 0))
  {
    return 0;
  }
  if (event == EVENT_TAP_CARD)
  {
    if (affected_card_controller == player)
    {

      result = is_what(player, affected_card, 1);
      if ((result != 0) && (multi_mana_transmutation[player][0] != 0))
      {
        return 0;
      }
      if (6 < tapped_for_mana_color)
      {
        if (tapped_for_mana_color != 0x100)
        {
          return 0;
        }
        result = 0;
        do
        {
          if (0 < tapped_for_mana[result])
          {
            produce_mana(affected_card_controller, result, tapped_for_mana[result] * 2);
            tapped_for_mana[result] = tapped_for_mana[result] * 3;
          }
          result = result + 1;
        } while (result != 7);
        return 0;
      }
      if (tapped_for_mana_color < 0)
      {
        return 0;
      }
      produce_mana(affected_card_controller, tapped_for_mana_color, 2);
      result = tapped_for_mana_color;
      tapped_for_mana[0] = 0;
      tapped_for_mana[1] = 0;
      tapped_for_mana[2] = 0;
      tapped_for_mana[3] = 0;
      tapped_for_mana[4] = 0;
      tapped_for_mana[5] = 0;
      tapped_for_mana[6] = 0;
      tapped_for_mana_color = 0x100;
      tapped_for_mana[result] = 3;
      return 0;
    }
  }
  else if ((((event == EVENT_COUNT_MANA) && (affected_card_controller == player)) &&
            (result = is_tapped(player, affected_card), result == 0)) &&
           ((result = is_animated_and_sick(affected_card_controller, affected_card),
             result == 0 && (result = can_produce_mana_2022(affected_card_controller, affected_card),
                             result != 0))))
  {

    result = is_what(affected_card_controller, affected_card, 1);
    if ((result != 0) && (multi_mana_transmutation[player][0] != 0))
    {
      return 0;
    }
    instance = get_card_instance(affected_card_controller, affected_card);
    if ((cards_at_7c7000[instance->internal_card_id]->extra_ability & 0x1000) != 0)
    {
      call_card_function(affected_card_controller, affected_card, EVENT_COUNT_MANA);
      call_card_function(affected_card_controller, affected_card, EVENT_COUNT_MANA);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}


/* ManalinkEh 2022: 0x235e100; original cards/theros_beyond_death.c:2212. */
int card_kiora_bests_the_sea_god(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  int card_00;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    token_generation_2022_t token;
    uint8_t bytes[84];
  } recovered_stack_token;

  if (event == EVENT_CHAPTER1)
  {
    default_token_definition_2022(player, card, 0x495c, &recovered_stack_token.token);
    recovered_stack_token.token.tou = 8;
    recovered_stack_token.token.pow = 8;
    recovered_stack_token.token.s_key_plus = 0x800000;
    generate_token_2022(&recovered_stack_token.token);
  }
  else if (event == EVENT_CHAPTER2)
  {
    iVar1 = 1 - player;
    if (0 < active_cards_count[iVar1])
    {
      card_00 = 0;
      do
      {
        instance = in_play(iVar1, card_00);
        if (instance != NULL)
        {
          iVar2 = is_what(iVar1, card_00, 0x47);
          if (iVar2 != 0)
          {
            iVar2 = is_what(iVar1, card_00, 1);
            if (iVar2 == 0)
            {
              effect_frost_titan(player, card, iVar1, card_00);
            }
          }
        }
        card_00 = card_00 + 1;
      } while (card_00 < active_cards_count[iVar1]);
    }
  }
  else if (event == EVENT_CHAPTER3)
  {
    default_target_definition(player, card, &recovered_stack_token.target, 0x47);
    recovered_stack_token.target.allowed_controller = 1 - player;
    recovered_stack_token.target.allow_cancel = 0;
    iVar1 = can_target(&recovered_stack_token.target);
    if (iVar1 != 0)
    {
      iVar1 = new_pick_target(&recovered_stack_token.target,
                              "Select target permanent an opponent controls.", 0, 0x80);
      if (iVar1 != 0)
      {
        instance = get_card_instance(player, card);
        iVar1 = instance->targets[0].card;
        instance = get_card_instance(player, card);
        gain_control_permanently(player, instance->targets[0].player, iVar1);
        instance = get_card_instance(player, card);
        iVar1 = instance->targets[0].card;
        instance = get_card_instance(player, card);
        untap_card(instance->targets[0].player, iVar1);
      }
    }
  }
  iVar1 = saga(player, card, event, 3);
  return iVar1;
}


#ifndef SHANDALAR
/* Recovered October 2022 typed consumers and compiler parts. */

/* ManalinkEh 2022: 0x2359940; original cards/theros_beyond_death.c:4. */
static int escape_part_2022(int player, int card, event_t event, int colorless, int black, int blue,
                            int green, int red, int white, int num_exile)
{
  int32_t *piVar1;
  card_instance_t *instance;
  int result;
  int position;
  int iid;
  target_t *ptVar4;
  test_definition_2022_t this_test;
  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = has_mana_multi(player, colorless, black, blue, green, red, white);
    if ((result != 0) && (result = count_graveyard(player), num_exile < result))
    {
      result = get_id(player, card);
      result = can_legally_play_csvid_now(player, result, EVENT_GRAVEYARD_ABILITY);
      if (result != 0)
      {
        return ((result == 99) - 1 & 0xfffffc00) + 0x2400;
      }
      if ((hack_allow_sorcery_if_rules_engine_is_only_effect == 1) && (stack_size == 1))
      {
        return 0x2000;
      }
    }
  }
  else if (event == EVENT_PAY_FLASHBACK_COSTS)
  {
    instance = get_card_instance(player, card);
    new_default_test_definition_2022(&this_test, 0xff, "Select a card to exile for escape.");
    this_test.id = 0x388;
    this_test.id_flag = 1;
    result = charge_mana_multi(player, colorless, black, blue, green, red, white);
    if (result != 0)
    {
      result = instance->targets[3].card;
      position = validate_target_from_grave_source(player, card, player, 3);
      if (position == -1)
      {
        return -1;
      }
      iid = turn_card_in_grave_face_down(player, position);
      select_multiple_cards_from_graveyard_2022(player, player, -1, 3, &this_test, num_exile,
                                                instance->targets);
      result = find_in_graveyard_by_source(player, result, position);
      turn_card_in_grave_face_up(player, result, iid);
    }
    if (spell_fizzled == 1)
    {
      return 0;
    }
    if (0 < num_exile)
    {
      ptVar4 = instance->targets;
      do
      {
        piVar1 = &ptVar4->player;
        ptVar4 = ptVar4 + 1;
        rfg_card_from_grave(player, *piVar1);
      } while (ptVar4 != instance->targets + num_exile);
    }
    return 1;
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x235cdd0; original cards/theros_beyond_death.c:1548. */
int card_ereboss_intervention(int player, int card, event_t event)
{
  target_t *ptVar1;
  card_instance_t *instance;
  int iVar3;
  uint32_t uVar4;
  int grave_id;
  unsigned int uVar5;
  int iVar6;
  target_definition_2022_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition_2022(player, card, &td, 2);
    get_card_instance(player, card);
    iVar3 = generic_spell_2022(player, card, EVENT_CAN_CAST, 4, NULL, NULL, 0, NULL);
    return iVar3;
  }
  if (event != EVENT_CAST_SPELL)
  {
    if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      default_target_definition_2022(player, card, &td, 2);
      get_card_instance(player, card);
      return 0;
    }
    default_target_definition_2022(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event != EVENT_RESOLVE_SPELL)
    {
      return 0;
    }

    iVar3 = instance->info_slot;
    if (instance->eot_toughness == 1)
    {
      pump_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card,
                          -iVar3, -iVar3);
      gain_life_2022(player, iVar3);
    }
    else
    {

      if ((instance->eot_toughness == 2) && (0 < iVar3 * 2))
      {
        iVar6 = 0;
        do
        {
          grave_id = validate_target_from_grave_source(
              player, card, instance->targets[iVar6].player >> 0x10, iVar6);
          if (grave_id == -1)
            break;
          ptVar1 = instance->targets + iVar6;

          iVar6 = iVar6 + 1;
          rfg_card_from_grave(ptVar1->player >> 0x10, grave_id);
        } while (iVar6 != iVar3 * 2);
      }
    }
    kill_card(player, card, KILL_DESTROY);
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
  default_target_definition_2022(player, card, &td, 2);
  instance = get_card_instance(player, card);
  if (card != affected_card)
  {
    return 0;
  }
  if (player != affected_card_controller)
  {
    return 0;
  }

  iVar3 = count_graveyard(player);
  if (iVar3 != 0)
  {
    uVar5 = 1;
    iVar3 = graveyard_has_shroud(player);
    if (iVar3 != 0)
      goto LAB_0235cf67;
  }
  iVar3 = count_graveyard(1 - player);
  uVar5 = 0;
  if (iVar3 != 0)
  {
    iVar3 = graveyard_has_shroud(1 - player);
    uVar5 = (unsigned int)(iVar3 == 0);
  }
LAB_0235cf67:
  iVar3 = can_target_2022(&td);
  uVar4 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\016", "\001\014",
                      "Debuff creature & gain life", iVar3, 5, "Exile cards from graveyards", uVar5,
                      5, 0, 0, 0, 0);
  iVar3 = max_x_value;
  if (uVar4 == 0)
  {
    spell_fizzled = 1;
  }
  else
  {
    if (uVar4 == 1)
    {
      generic_spell_2022(player, card, EVENT_CAST_SPELL, 5, &td, "TARGET_CREATURE", 1, NULL);
      iVar3 = max_x_value;
    }
    else if (uVar4 == 2)
    {

      max_x_value = 9;
      iVar6 = generic_spell_2022(player, card, EVENT_CAST_SPELL, 4, NULL, NULL, 0, NULL);
      if (iVar6 != 0)
      {
        select_multiple_cards_from_either_graveyard_2022(
            player, card, SFG_0, 3, 1, NULL, instance->info_slot * 2, instance->targets);
      }
    }
    max_x_value = iVar3;
    instance->eot_toughness = uVar4;
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2361340; original cards/theros_beyond_death.c:3883. */
int card_the_binding_of_the_titans(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int grave_id;
  int iVar3;
  int target_location;
  int iStack_f4;
  test_definition_2022_t test;

  if (event == EVENT_CHAPTER1)
  {

    mill(current_turn, 3);
    mill(current_turn ^ 1, 3);
  }
  else if (event == EVENT_CHAPTER2)
  {

    instance = get_card_instance(player, card);
    iVar1 = select_multiple_cards_from_either_graveyard_2022(player, card, SFG_0, 3, 1, NULL, 2,
                                                             instance->targets);
    iStack_f4 = 0;
    if (0 < iVar1)
    {
      target_location = 0;
      do
      {

        grave_id = validate_target_from_grave_source(
            player, card, instance->targets[target_location].player >> 0x10, target_location);
        if (grave_id != -1)
        {
          iVar3 = is_what(
              -1,
              graveyard_ptr_mutable[instance->targets[target_location].player >> 0x10][grave_id],
              2);
          iStack_f4 = (iStack_f4 + 1) - (unsigned int)(iVar3 == 0);
          rfg_card_from_grave(instance->targets[target_location].player >> 0x10, grave_id);
        }
        target_location = target_location + 1;
      } while (iVar1 != target_location);
    }
    gain_life_2022(player, iStack_f4);
  }
  else if (event == EVENT_CHAPTER3)
  {
    new_default_test_definition_2022(&test, 3, "Select target creature or land card.");
    iVar1 = new_special_count_grave_2022(player, &test);
    if (iVar1 != 0)
    {
      iVar1 = graveyard_has_shroud(player);
      if (iVar1 == 0)
      {
        new_global_tutor_2022(player, player, 2, 1, 1, 1, &test);
      }
    }
  }
  iVar1 = saga(player, card, event, 3);
  return iVar1;
}

#endif
