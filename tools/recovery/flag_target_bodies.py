"""Native dynamic-selection frame used by the flag/target consumer batch."""

SCOUTING='''int card_scouting_trek(int player, int card, event_t event)
{
  if (event == EVENT_RESOLVE_SPELL) {
    test_definition_t this_test;
    new_default_test_definition(&this_test, TYPE_LAND, "Select a basic land card.");
    this_test.subtype = 0x2000;
    this_test.no_shuffle = 1;
    int deck_count = count_deck(player);
    int tutored[deck_count];
    int count = 0;
    for (;;) {
      int selected = new_select_a_card(player, player, 1, 0, 1, -1, &this_test);
      if (selected == -1) break;
      tutored[count] = deck_ptr[player][selected];
      ++count;
      remove_card_from_deck(player, selected);
    }
    shuffle(player);
    if (count > 0) {
      /* Native loop puts count + 1 entries back; retain the extra read for downstream review. */
      for (int i = 0; i != count + 1; ++i) raw_put_iid_on_top_of_deck(player, tutored[i]);
    }
    if (count > 1) rearrange_top_x(player, player, count);
    kill_card(player, card, KILL_DESTROY);
  }
  return basic_spell(player, card, event);
}
'''
