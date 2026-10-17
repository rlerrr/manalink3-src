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
