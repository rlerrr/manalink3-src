"""Native Saheeli's Directive control flow and recovered variable-length buffer."""

BODY=r'''int card_saheelis_directive(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL)
  {
    if (card != affected_card || player != affected_card_controller)
      goto native_improvise;
    improvise_2022(player, card, EVENT_CAST_SPELL);
    generic_spell(player, card, EVENT_CAST_SPELL, GS_X_SPELL, NULL, NULL, 0, NULL);
    if (spell_fizzled == 1)
      return 0;
    card_instance_t *instance = get_card_instance(player, card);
    int adjustment = -48;
    if (instance->info_slot > 5)
      adjustment = instance->info_slot * 12;
    ai_modifier += adjustment;
    return 0;
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    card_instance_t *instance = get_card_instance(player, card);
    int deck_count = count_deck(player);
    int max = instance->info_slot;
    if (deck_count < max)
      max = deck_count;
    if (max != 0)
    {
      int cage = grafdiggers_cages(player, TYPE_CREATURE, TUTOR_FROM_DECK);
      show_deck(player, deck_ptr[player], max, "Cards revealed by Saheeli's Directive", 0, str_Done);
      if (cage == 0)
      {
        int deck_copy[max];
        for (int i = 0; i < max; ++i)
        {
          deck_copy[i] = deck_ptr[player][0];
          remove_card_from_deck(player, 0);
        }
        char msg[100];
        scnprintf(msg, sizeof(msg), &UNK_02754194, max);
        test_definition_t test;
        new_default_test_definition(&test, TYPE_ARTIFACT, msg);
        test.cmc_flag = F5_CMC_LESSER_THAN_VALUE;
        test.cmc = max + 1;
        int amount = max;
        do
        {
          int selected = select_card_from_zone(player, player, deck_copy, amount, 0, 1, -1, &test);
          if (selected == -1)
          {
            if (amount != 0)
            {
              if (amount > 0)
                for (int i = cage; i < amount; ++i)
                  raw_put_iid_on_top_of_deck(player, deck_copy[i]);
              mill(player, amount);
            }
            break;
          }
          is_what(-1, deck_copy[selected], TYPE_CREATURE);
          int card_added = add_card_to_hand(player, deck_copy[selected]);
          set_special_flags(player, card_added, SF_PLAYED_FROM_DECK);
          put_into_play(player, card_added);
          --amount;
          for (int i = selected; i < amount; ++i)
            deck_copy[i] = deck_copy[i + 1];
        } while (amount != 0);
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  else if (event == EVENT_CAN_CAST || event == EVENT_CHANGE_TARGET)
    improvise_2022(player, card, event);
  else if (event != EVENT_CAN_CHANGE_TARGET)
    goto native_improvise;
  return generic_spell(player, card, event, GS_X_SPELL, NULL, NULL, 0, NULL);

native_improvise:
  return improvise_2022(player, card, event);
}
'''
