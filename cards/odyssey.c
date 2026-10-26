#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x250a4a0; original cards/odyssey.c:2626. */
int card_fervent_denial(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    result = do_flashback(player, card, event, 5, 0, 2, 0, 0, 0);
    return result;
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
      if (event == EVENT_RESOLVE_SPELL)
      {
        instance = get_card_instance(player, card);
        result = counterspell_validate(player, card, NULL, 0);
        if (result != 0)
        {
          real_counter_a_spell(player, card, instance->targets[0].player,
                               instance->targets[0].card);
        }
        instance->number_of_targets = '\0';
        result = get_flashback(player, card);
        if (result != 0)
        {
          kill_card(player, card, KILL_EXILE);
          return 0;
        }
        kill_card(player, card, KILL_DESTROY);
        return 0;
      }
      if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
      {
        return 0;
      }
    }
  }
  result = counterspell(player, card, event, NULL, 0);
  return result;
}

/* ManalinkEh 2022: 0x250a820; original cards/odyssey.c:2687. */
int card_flame_burst(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
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
          iVar1 = valid_target(&td);
          if (iVar1 != 0)
          {
            iVar1 = get_id(player, card);
            iVar1 = count_graveyard_by_id(2, iVar1);
            iVar2 = count_graveyard_by_id(2, 0x160d);
            damage_target0(player, card, iVar1 + 2 + iVar2);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0250a87c;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
LAB_0250a87c:
  iVar1 = generic_spell(player, card, event, 1, &td, "TARGET_ANY", 1, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x2510090; original cards/odyssey.c:4402. */
int card_predict(int player, int card, event_t event)
{
  int t_player;
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  target_definition_t td;
  test_definition_t test;
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  if (event == EVENT_RESOLVE_SPELL)
  {
    iVar2 = valid_target(&td);
    if (iVar2 != 0)
    {
      new_default_test_definition(&test, 0xff, "Choose a card name.");
      if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
      {
        iVar2 = ai_name_a_random_csvid(player, card, 1 - player, TYPE_NONE, &test);
      }
      else
      {
        iVar2 = new_card_from_list(player, 3, &test);
      }
      if (-1 < iVar2)
      {
        instance = get_card_instance(player, card);
        t_player = instance->targets[0].player;
        iVar1 = *deck_ptr[t_player];
        if (iVar1 != -1)
        {
          mill(t_player, 1);
          iVar2 = has_same_name(-1, iVar2, -1, (unsigned int)cards_data[iVar1].id);
          if (iVar2 == 0)
          {
            draw_a_card(player);
          }
          else
          {
            draw_cards(player, 2);
          }
        }
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  iVar2 = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2510270; original cards/odyssey.c:4444. */
int card_price_of_glory(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = in_play(player, card);
  if ((((event == EVENT_TAP_CARD) && (instance != NULL)) && (-1 < tapped_for_mana_color)) &&
      (current_turn != affected_card_controller))
  {
    result = is_what(affected_card_controller, affected_card, 1);
    if (result != 0)
    {
      result = is_humiliated_for_ability(player, card, AL_TRIG);
      if (result == 0)
      {
        kill_card(affected_card_controller, affected_card, KILL_DESTROY);
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2511ec0; original cards/odyssey.c:4960. */
int card_seize_the_day(int player, int card, event_t event)
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
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_0251206e;
    }
    else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      if (event == EVENT_RESOLVE_SPELL)
      {
        instance = get_card_instance(player, card);
        result = instance->targets[0].card;
        instance = get_card_instance(player, card);
        untap_card(instance->targets[0].player, result);
        extra_combat_effect(player, card, EVENT_RESOLVE_SPELL, 0, NULL, 1);
        result = get_flashback(player, card);
        if (result == 0)
        {
          kill_card(player, card, KILL_DESTROY);
        }
        else
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    else
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
    }
    result = do_flashback(player, card, event, 2, 0, 0, 0, 1, 0);
    return result;
  }
  default_target_definition(player, card, &td, 2);
LAB_0251206e:
  td.allowed_controller = player;
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2501eb0; original cards/odyssey.c:5852. */
static int verdant_succession_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  target_t *ptVar5;
  int (*paiVar6)[2];
  int iStack_198;
  int added[9][2];
  char msg[100];
  test_definition_t this_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (iVar2 = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           iVar2 != 0x1673))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (iVar2 = effect_follows_control_of_attachment(player, card, 0x4700), iVar2 != 0))
    {
      return 0;
    }
  LAB_02501ee7:
    iVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
      goto LAB_02501f10;
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (iVar2 = effect_follows_control_of_attachment(player, card, event), iVar2 != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_02501ee7;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);
    if ((pcVar3 != NULL) &&
        (iVar2 = is_this_dying(affected_card_controller, affected_card), iVar2 != 0))
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
      default_test_definition(&this_test, 2);
      this_test.color = get_sleighted_color_test(player, card, 8);
      count_for_gfp_ability_and_store_values(player, card, 0x77, 2, 0, &this_test, 0x10, 1);
    }
    iVar2 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
    {
      return 0;
    }
  }
  iVar2 = check_status(player, card, 0x10000);
  if (iVar2 == 0)
  {

    pcVar1 = get_card_instance(player, card);
    ptVar5 = pcVar1->targets + 1;
    iVar2 = 0;
    do
    {
      if (ptVar5->player != -1)
      {
        added[iVar2][0] = ptVar5->player;
        iVar4 = get_card_name(-1, ptVar5->card);
        ptVar5->card = -1;
        added[iVar2][1] = iVar4;
        ptVar5->player = -1;
        iVar2 = iVar2 + 1;
      }
      ptVar5 = ptVar5 + 1;
    } while (ptVar5 != pcVar1->targets + 10);

    if (0 < iVar2)
    {

      paiVar6 = added;
      do
      {
        iVar4 = (*paiVar6)[1];
        iStack_198 = card_counts[player][5] + 1;
        do
        {
          if (iVar4 == -1)
          {
            scnprintf(msg, 100,
                      "You won\'t be able to tutor any card as you\'re searching for a card with "
                      "no name.");
          }
          else
          {

            scnprintf(msg, 100, "Select a card named %s", cards_ptr[iVar4]->name);
          }
          new_default_test_definition(&this_test, 0xff, msg);
          this_test.id = iVar4;
          new_global_tutor((*paiVar6)[0], (*paiVar6)[0], 1, 4, 0, 5, &this_test);
          iStack_198 = iStack_198 + -1;
        } while (iStack_198 != 0);
        paiVar6 = paiVar6 + 1;
      } while (paiVar6 != added + iVar2);
    }
    kill_card(player, card, KILL_EXILE);
  }
LAB_02501f10:
  if ((event == EVENT_CLEANUP) && (iVar2 = check_status(player, card, 0x10000), iVar2 == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2512e50; original cards/odyssey.c:5362. */
int card_spellbane_centaur(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  unsigned int uVar4;

  if (event == EVENT_ABILITIES)
  {
    result = is_what(affected_card_controller, affected_card, 2);
    if ((result != 0) && (affected_card_controller == player))
    {
      special_abilities2(player, affected_card, EVENT_ABILITIES, SP_KEYWORD2_GRANTED_PROT_MISC);
    }
  }
  else if (event == EVENT_PROT_MISC)
  {
    instance = get_card_instance(affected_card_controller, affected_card);
    if (instance != NULL)
    {
      result = is_what(affected_card_controller, affected_card, 2);
      if (((result != 0) && (affected_card_controller == player)) &&
          (((unsigned int)event_result >> 24 & 0xff) == '\x05'))
      {
        uVar3 = get_color(attacking_card_controller, attacking_card);
        uVar4 = get_sleighted_color_test(player, card, 4);
        if ((uVar3 & uVar4) != 0)
        {
          event_result = event_result | 1;
        }
      }
    }
  }
  return 0;
}
