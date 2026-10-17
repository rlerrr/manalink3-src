#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25e6a60; original cards/commander_2020.c:24. */
int card_bonders_ornament(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int player_00;

  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 1;
      result = paying_mana();
      if ((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1,
                                                               5, 0, 0, 0, 0, 0, 0, NULL, NULL),
                            result != 0))
      {
        result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Produce mana", 1, 1,
                             "Draw a card", 1, 10, 0, 0, 0, 0);
        if (result == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (result != 1)
        {
          if (result != 2)
          {
            return 0;
          }
          add_state(player, card, 0x10);
          result = charge_mana_for_activated_ability(player, card, 4, 0, 0, 0, 0, 0);
          if (result == 0)
          {
            remove_state(player, card, 0x10);
          }
          tapped_for_mana_color = -1;
          return 0;
        }
      }
      event = EVENT_ACTIVATE;
    }
    else
    {
      if (event == EVENT_RESOLVE_ACTIVATION)
      {

        instance = get_card_instance(player, card);
        if (instance->info_slot != 2)
        {
          return 0;
        }

        uVar3 = 0;
        do
        {
          player_00 = current_turn ^ uVar3;
          result = get_id(instance->parent_controller, instance->parent_card);
          result = check_battlefield_for_id(player_00, result);
          if (result != 0)
          {
            draw_a_card(player_00);
          }
          uVar3 = uVar3 + 1;
        } while (uVar3 != 2);
        return 0;
      }
      if (event == EVENT_CLEANUP)
      {
        return 0;
      }
    }
  }

  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25e6dd0; original cards/commander_2020.c:280. */
int card_jirina_kudro(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  token_generation_t token;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  boost_creature_type(player, card, event, SUBTYPE_HUMAN, 2, 0, 0, BCT_CONTROLLER_ONLY);

  if ((event == EVENT_ETB) && (0 < active_cards_count[player]))
  {
    iVar3 = 0;
    do
    {
      instance = in_play(player, iVar3);
      if ((instance != NULL) && (iVar2 = get_id(player, iVar3), iVar2 == 0x3953))
      {
        iVar3 = count_counters(player, iVar3, COUNTER_VORTEX);
        if (iVar3 / 2 == 0)
        {
          return 0;
        }
        default_token_definition(player, card, 0x494c, &token);
        token.qty = (unsigned char)(iVar3 / 2);
        generate_token(&token);
        return 0;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < active_cards_count[player]);
  }
  return 0;
}
