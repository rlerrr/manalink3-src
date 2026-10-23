"""Eerie Ultimatum: native selection, restore, VLA sort and reanimation loops."""

EERIE = r'''int card_eerie_ultimatum(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL &&
      (card != affected_card || player != affected_card_controller))
    return 0;
  if (event == EVENT_RESOLVE_SPELL)
  {
    test_definition_t test;
    /* Native selection storage is not initialized before the predicate reads it. */
    int chosen[2][151];
    new_default_test_definition(&test, 0x47, "Select a permanent card.");
    test.value_for_special_selection_function = (int)chosen[1];
    test.no_shuffle = 1;
    test.special_selection_function = test_not_in_array;
    test.id = 0x388;
    test.id_flag = 1;
    int shroud = graveyard_has_shroud(player);
    if (shroud == 0)
    {
      int num_chosen = 0;
      while (1)
      {
        int selected = new_select_a_card(player, player, 5, 0, 5, 0, &test);
        if (selected == -1)
          break;
        int iid = graveyard_ptr_mutable[player][selected];
        if (has_subtype_by_id_2022(player, (unsigned int)cards_data[iid].id, SUBTYPE_AURA))
        {
          int temporary = add_card_to_hand(player, iid);
          if (temporary < 0)
            continue;
          add_state(player, temporary, 0x20);
          set_special_flags3(player, card, 0x40);
          --hand_count[player];
          int can_cast = call_card_function(player, temporary, EVENT_CAN_CAST);
          obliterate_card_and_recycle(player, temporary);
          if (can_cast == 0)
            continue;
        }
        chosen[0][num_chosen] = selected;
        chosen[1][num_chosen] = turn_card_in_grave_face_down(player, selected);
        ++num_chosen;
      }
      for (int i = 0; i < num_chosen; ++i)
        turn_card_in_grave_face_up(player, chosen[0][i], chosen[1][i]);

      /* The zero-selection branch also allocates a zero-length GNU VLA and sorts it. */
      int to_reanimate[num_chosen];
      for (int i = 0; i < num_chosen; ++i)
        to_reanimate[i] = chosen[0][i];
      descending_qsort(num_chosen, to_reanimate);
      if (num_chosen > 0)
      {
        int reanimated[num_chosen];
        for (int i = 0; i < num_chosen; ++i)
        {
          reanimated[i] = reanimate_permanent(player, card, player, to_reanimate[i], REANIMATE_DEFAULT);
          /* No native guard skips a failed (-1) reanimation result. */
          state_untargettable(player, reanimated[i], 1);
        }
        for (int i = shroud; i < num_chosen; ++i)
          state_untargettable(player, reanimated[i], 0);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  if (event == EVENT_CAN_CAST || event == EVENT_CAST_SPELL ||
      event == EVENT_RESOLVE_SPELL || event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CHANGE_TARGET)
    return basic_spell(player, card, event);
  return 0;
}
'''
