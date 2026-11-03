#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25e6a60; original cards/commander_2020.c:24. */
int card_bonders_ornament(int player, int card, event_t event)
{
  enum
  {
    CHOICE_MANA = 1,
    CHOICE_DRAW = 2
  };

  int result;
  card_instance_t *instance;
  unsigned int turn_order_index;
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
        if (result != CHOICE_MANA)
        {
          if (result != CHOICE_DRAW)
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

        turn_order_index = 0;
        do
        {
          player_00 = current_turn ^ turn_order_index;
          result = get_id(instance->parent_controller, instance->parent_card);
          result = check_battlefield_for_id(player_00, result);
          if (result != 0)
          {
            draw_a_card(player_00);
          }
          turn_order_index = turn_order_index + 1;
        } while (turn_order_index != 2);
        return 0;
      }
      if (event == EVENT_CLEANUP)
      {
        return 0;
      }
    }
  }

  return mana_producer(player, card, event);
}

/* ManalinkEh 2022: 0x25e6dd0; original cards/commander_2020.c:280. */
int card_jirina_kudro(int player, int card, event_t event)
{
  int count;
  card_instance_t *instance;
  int commander_marker_card;
  token_generation_t token;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  boost_creature_type(player, card, event, SUBTYPE_HUMAN, 2, 0, 0, BCT_CONTROLLER_ONLY);

  if ((event == EVENT_ETB) && (0 < active_cards_count[player]))
  {
    commander_marker_card = 0;
    do
    {
      instance = in_play(player, commander_marker_card);
      if ((instance != NULL) && (get_id(player, commander_marker_card) == 0x3953))
      {
        count = count_counters(player, commander_marker_card, COUNTER_VORTEX);
        if (count / 2 == 0)
        {
          return 0;
        }
        default_token_definition(player, card, 0x494c, &token);
        token.qty = (unsigned char)(count / 2);
        generate_token(&token);
        return 0;
      }
      commander_marker_card = commander_marker_card + 1;
    } while (commander_marker_card < active_cards_count[player]);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25e6ee0; original cards/commander_2020.c:374. */
int card_nesting_grounds(int player, int card, event_t event)
{
  enum
  {
    CHOICE_MANA = 1,
    CHOICE_COUNTER = 2
  };

  int result;
  card_instance_t *instance;
  counter_t chosen_counter_type;
  bool not_speculating;
  target_definition_t *destination_definition;
  target_definition_t *source_definition;
  target_definition_t td_from;
  target_definition_t td_to;

  if (event == EVENT_CAN_ACTIVATE)
  {
    return mana_producer(player, card, event);
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      instance = get_card_instance(player, card);
      default_target_definition(player, card, &td_from, 0x47);
      td_to = td_from;
      source_definition = &td_from;
      destination_definition = &td_to;
      td_from.special = 0x40000;
      td_from.allowed_controller = player;
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }
      if (instance->info_slot != 2)
      {
        return 0;
      }
      result = valid_target(source_definition);
      if (result != 0)
      {
        result = validate_target(player, card, destination_definition, 1);
        if (result != 0)
        {
          move_counters(player, card, instance->targets[1].player, instance->targets[1].card,
                        instance->targets[0].player, instance->targets[0].card,
                        instance->targets[2].card, 1);
          return 0;
        }
        return 0;
      }
      return 0;
    }
    return mana_producer(player, card, event);
  }
  instance = get_card_instance(player, card);
  default_target_definition(player, card, &td_from, 0x47);
  td_to = td_from;
  td_from.allowed_controller = player;
  td_from.special = 0x40000;

  result = paying_mana();
  if ((result != 0) || (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 2,
                                                           0, 0, 0, 0, 0, 0, &td_from, NULL),
                        result == 0))
  {
    instance->info_slot = 1;
    return mana_producer(player, card, EVENT_ACTIVATE);
  }
  result = target_available(player, card, &td_to);
  if (result < 2)
  {
    instance->info_slot = 1;
    return mana_producer(player, card, EVENT_ACTIVATE);
  }
  not_speculating = ai_is_speculating != 1;
  instance->info_slot = 2;
  if ((not_speculating) && ((player != 1 || (((unsigned char)trace_mode & 2) != 0))))
  {
    result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                         "Move a counter", 1, 1, 0, 0, 0, 0);
    if (result == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    if (result == CHOICE_MANA)
      return mana_producer(player, card, EVENT_ACTIVATE);
    if (result != CHOICE_COUNTER)
    {
      return 0;
    }
  }
  add_state(player, card, 0x10);
  instance->number_of_targets = '\0';
  result = generic_activated_ability(player, card, EVENT_ACTIVATE, 7, 1, 0, 0, 0, 0, 0, 0, &td_from,
                                     "Select target permanent with a counter.");
  if (!(result == 0))
  {
    state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
    result = can_target(&td_to);
    if (result == 0)
    {

      spell_fizzled = 1;
    }
    else
    {
      pick_next_target_noload(&td_to, "Select target perment to move a counter to.");
      if (spell_fizzled != 1)
      {
        chosen_counter_type = choose_existing_counter_type(
            player, player, card, instance->targets[0].player, instance->targets[0].card, CECT_MOVE,
            instance->targets[1].player, instance->targets[1].card);
        if (chosen_counter_type == COUNTER_invalid)
          spell_fizzled = 1;
        else
        {
          instance->info_slot = 2;
          instance->targets[2].card = chosen_counter_type;
        }
      }
    }
    state_untargettable(instance->targets[0].player, instance->targets[0].card, 0);
  }
  if (spell_fizzled == 1)
  {
    remove_state(player, card, 0x10);
    instance->number_of_targets = '\0';
  }
  return 0;
}


#ifndef SHANDALAR
/* Recovered October 2022 native scalar-word handlers. */
int card_fierce_guardianship(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25e6cd0; original cards/commander_2020.c:201. */
int card_fierce_guardianship(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if (event == EVENT_MODIFY_COST)
  {
    if ((player_bits[player] & PB_COMMANDER_IN_PLAY) != 0)
    {
      null_casting_cost(player, card);
      instance->info_slot = 1;
      return 0;
    }
    instance->info_slot = 0;
  }
  else
  {
    if (event == EVENT_CAN_CAST)
    {
      return card_negate(player, card, event);
    }
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        result = nemesis_freespell(player, card, 0x6c);
        if (result != 0)
          return card_negate(player, card, event);
        spell_fizzled = 1;
      }
    }
    else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
             (event == EVENT_CHANGE_TARGET))
      return card_negate(player, card, event);
  }
  return 0;
}

#endif
