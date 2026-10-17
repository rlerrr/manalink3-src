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
        pump_ability_until_eot(instance->parent_controller, instance->parent_card,
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
    lose_life(player, 2);
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
  vigilance(player, card, event);
  menace(player, card, event);
  lifelink(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x235ea20; original cards/theros_beyond_death.c:2481. */
int card_mire_triton(int player, int card, event_t event)
{
  deathtouch(player, card, event);
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
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
              lose_life(player, 1);
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

/* ManalinkEh 2022: 0x235a2e0; original cards/theros_beyond_death.c:4. */
int escape(int player, int card, event_t event, int colorless, int black, int blue, int green,
           int red, int white, int num_exile)
{
  int result;

  if ((event != EVENT_GRAVEYARD_ABILITY) && (event != EVENT_PAY_FLASHBACK_COSTS))
  {
    return 0;
  }
  result = escape(player, card, event, colorless, black, blue, green, red, white, num_exile);
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
              gain_life(player, 3);
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
  token_generation_t token;

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
      default_token_definition(player, card, 0x56d9, &token);
      token.pow = 2;
      token.tou = 3;
      token.special_infos = 0x42;
      generate_token(&token);
    }
    else if (uVar1 == '\x02')
    {
      iVar3 = valid_target(ptStack_140);
      if ((iVar3 != 0) && (bounce_permanent(instance->targets[0].player, instance->targets[0].card),
                           hand_count[instance->targets[0].player] != 0))
      {

        default_test_definition((test_definition_t *)&token, 0xff);
        iVar3 = instance->targets[0].player;
        iVar3 = new_select_a_card(iVar3, iVar3, 4, 1, 3, -1, (test_definition_t *)&token);
        rfg_card_in_hand(instance->targets[0].player, iVar3);
      }
    }
    else if (uVar1 == '\x03')
    {
      iVar3 = 1 - player;
      iVar4 = count_rfg(iVar3);
      if (iVar4 != 0)
      {

        new_default_test_definition((test_definition_t *)&token, 1,
                                    "Select a card to cast for free.");

        iVar4 = special_count_zone(player, rfg_ptr[iVar3], (test_definition_t *)&token);
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

            iVar4 = new_select_a_card(player, iVar3, 3, 0, 2, 0, (test_definition_t *)&token);
            if ((iVar4 == -1) &&
                (iVar4 = new_select_a_card(player, iVar3, 3, 0, 2, 0, (test_definition_t *)&token),
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
          pump_until_eot_merge_previous(player, card, ptVar1->player, ptVar5->card, 2, 1);
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
        gain_life(instance->targets[0].player, iVar2 * 2);
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
  iVar1 = check_special_flags3(player, card, 0x80000);
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

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
        iVar3 = check_special_flags2(player, card, -0x80000000);
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
    result = check_special_flags3(player, card, 0x80000);
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
    uVar4 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
    uVar4 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
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
  iVar1 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
    iVar3 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
    iVar3 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
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
    gain_life(player, iStack_ec);
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
  target_definition_t td;

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
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = 1 - player;
    td.special = 0x1000000;
    td.extra = 3;
    td.allow_cancel = 0;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(
          &td,
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
    new_default_test_definition((test_definition_t *)&td, 0x102,
                                "Select target creature or planeswalker card.");
    result = select_target_from_grave_source(player, card, player, SFG_0, 2,
                                             (test_definition_t *)&td, 0);
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
  target_definition_t td;

  if (event == EVENT_CHAPTER1)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = 1 - player;
    td.power_requirement = 0x2002;
    td.allow_cancel = 0;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(
          &td, "Select target creature an opponent controls with power 2 or less.", 0, 0x81);
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
      new_default_test_definition((test_definition_t *)&td, 3,
                                  "Select a nonland, noncreature card.");
      new_effect_coercion(&ec, (test_definition_t *)&td);
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
    generate_token_by_id(player, card, 0x494e);
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
    generate_token_by_id(player, card, 0x493f);
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
        pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card,
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
    gain_life(player, result);
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
