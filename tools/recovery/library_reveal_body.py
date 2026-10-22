"""Native library chooser: restore the VLA, overlapping text views and call frames."""

STORAGE='''typedef union
{
  char display[100];
  char diagnostic[200];
} library_reveal_text_2022_t;'''

WORKER=r'''int reveal_top_cards_of_library_and_choose_fn_who_chooses(
    int src_player, int src_card, int t_player, int who_chooses, int num_cards,
    int must_select, tutor_destination_2022_t destination_chosen, int reveal_chosen,
    tutor_destination_2022_t destination_rest, int reveal_rest, test_definition_2022_t *test,
    library_reveal_callback_2022_t fn)
{
  library_reveal_text_2022_t text;
  if (reveal_rest)
    num_cards = reveal_top_cards_of_library(t_player, num_cards);
  else
  {
    int count = count_deck(t_player);
    if (num_cards > count)
      num_cards = count;
  }
  if (num_cards <= 0)
    return -1;

  int num_to_pick = test->qty;
  if (num_to_pick > num_cards)
    num_to_pick = num_cards;
  if (num_to_pick <= 0)
  {
    scnprintf(text.display, 100, LITERAL_TITLE, cards_ptr[get_id(src_player, src_card)]->name);
    call_sub_437E20_unless_ai_is_speculating();
    hack_show_deck_force_expansion = 0;
    show_deck_exe(who_chooses, deck_ptr[t_player], num_cards, text.display, 0, 0x7375b0);
    return -1;
  }

  test->qty = 1;
  test->create_minideck = (uint16_t)num_cards;
  int chosen_iids[num_to_pick];
  int num_chosen = 0;
  int selected = -1;
  int cage = 0;
  if (destination_chosen == TUTOR2022_PLAY || destination_chosen == TUTOR2022_PLAY_TAPPED ||
      destination_chosen == TUTOR2022_PLAY_ATTACKING)
    cage = grafdiggers_cages(t_player, TYPE_CREATURE, 1) != 0;

  for (int i = 0; i < num_to_pick; ++i)
  {
    selected = select_card_from_zone(who_chooses, t_player, deck_ptr[t_player], num_cards,
                                    must_select, test->ai_selection_mode, -1, test);
    if (selected == -1)
      break;
    --num_cards;
    int iid = deck_ptr[t_player][selected];
    if (reveal_chosen)
      reveal_card_iid(src_player, src_card, iid);

    switch (destination_chosen)
    {
      case TUTOR2022_HAND:
      case TUTOR2022_HAND_AND_MARK:
        remove_card_from_deck(t_player, selected);
        selected = add_card_to_hand(src_player, iid);
        if (destination_chosen == TUTOR2022_HAND_AND_MARK && selected >= 0)
          add_state(src_player, selected, STATE_TARGETTED);
        break;
      case TUTOR2022_GRAVE:
        remove_card_from_deck(t_player, selected);
        chosen_iids[num_chosen++] = iid;
        break;
      case TUTOR2022_PLAY:
      case TUTOR2022_PLAY_TAPPED:
      case TUTOR2022_PLAY_ATTACKING:
        if (!(cage && is_what(-1, deck_ptr[t_player][selected], TYPE_CREATURE)))
        {
          remove_card_from_deck(t_player, selected);
          chosen_iids[num_chosen++] = iid;
          break;
        }
        /* Native cage failure shares the deck-preserving rotation arm. */
      case TUTOR2022_DECK:
        for (int n = selected; n < num_cards; ++n)
        {
          int temporary = deck_ptr[t_player][n];
          deck_ptr[t_player][n] = deck_ptr[t_player][n + 1];
          deck_ptr[t_player][n + 1] = temporary;
        }
        break;
      case TUTOR2022_EXILE:
        rfg_card_in_deck(t_player, selected);
        break;
      case TUTOR2022_GET_ID:
        selected = (uint16_t)cards_data[iid].id;
        /* Intentionally fall through to the native quantity restriction. */
      case TUTOR2022_GET_POSITION:
        if (num_to_pick > 1)
        {
          sprintf(text.diagnostic, LITERAL_QTY, LITERAL_SOURCE, 884,
                  (int)destination_chosen, num_to_pick);
          backtrace_and_abort(text.diagnostic);
        }
        break;
      case TUTOR2022_BOTTOM_OF_DECK:
        put_card_in_deck_to_bottom(t_player, selected);
        break;
      case TUTOR2022_TPLAYER_HAND:
        remove_card_from_deck(t_player, selected);
        selected = add_card_to_hand(t_player, iid);
        break;
      case TUTOR2022_BOTTOM_OF_DECK_RANDOM:
        sprintf(text.diagnostic, LITERAL_CHOSEN, LITERAL_SOURCE, 902, (int)destination_chosen);
        backtrace_and_abort(text.diagnostic);
        break;
    }
  }

  if (num_cards > 0)
    switch (destination_rest)
    {
      case TUTOR2022_HAND:
      case TUTOR2022_HAND_AND_MARK:
        for (int i = 0; i < num_cards; ++i)
        {
          int card_added = add_card_to_hand(t_player, deck_ptr[t_player][i]);
          if (card_added >= 0 && destination_rest == TUTOR2022_HAND_AND_MARK)
            add_state(t_player, card_added, STATE_TARGETTED);
        }
        obliterate_top_n_cards_of_deck(t_player, num_cards);
        break;
      case TUTOR2022_DECK:
        rearrange_top_x(t_player, t_player, num_cards);
        break;
      case TUTOR2022_GRAVE:
        mill(t_player, num_cards);
        break;
      case TUTOR2022_EXILE:
        rfg_top_n_cards_of_deck(t_player, num_cards);
        break;
      case TUTOR2022_BOTTOM_OF_DECK:
        put_top_x_on_bottom(t_player, t_player, num_cards);
        break;
      case TUTOR2022_TPLAYER_HAND:
        for (int i = 0; i < num_cards; ++i)
          add_card_to_hand(1 - t_player, deck_ptr[t_player][i]);
        obliterate_top_n_cards_of_deck(t_player, num_cards);
        break;
      case TUTOR2022_BOTTOM_OF_DECK_RANDOM:
        put_top_x_on_bottom_in_random_order(t_player, num_cards);
        break;
      case TUTOR2022_PLAY:
      case TUTOR2022_PLAY_TAPPED:
      case TUTOR2022_GET_ID:
      case TUTOR2022_PLAY_ATTACKING:
      case TUTOR2022_GET_POSITION:
        sprintf(text.diagnostic, LITERAL_REST, LITERAL_SOURCE, 945, (int)destination_rest);
        backtrace_and_abort(text.diagnostic);
        break;
    }

  if (num_chosen <= 0)
    return selected;
  if (destination_chosen == TUTOR2022_GRAVE)
  {
    for (int i = num_chosen - 1; i >= 0; --i)
      raw_put_iid_on_top_of_deck(t_player, chosen_iids[i]);
    mill(t_player, num_chosen);
    return num_chosen;
  }
  if (destination_chosen == TUTOR2022_PLAY || destination_chosen == TUTOR2022_PLAY_TAPPED ||
      destination_chosen == TUTOR2022_PLAY_ATTACKING)
  {
    int result = num_chosen;
    for (int i = 0; i < num_chosen; ++i)
    {
      int card_added = add_card_to_hand(src_player, chosen_iids[i]);
      set_special_flags(src_player, card_added, SF_PLAYED_FROM_DECK);
      card_instance_t *instance = get_card_instance(src_player, card_added);
      if (t_player != src_player)
        instance->state ^= STATE_OWNED_BY_OPPONENT;
      if (fn)
        fn(src_player, card_added);
      if (destination_chosen == TUTOR2022_PLAY_ATTACKING)
      {
        instance->state |= STATE_TAPPED | STATE_ATTACKING;
        put_into_play(src_player, card_added);
        choose_who_attack(src_player, card_added);
      }
      else
      {
        if (destination_chosen == TUTOR2022_PLAY_TAPPED)
          instance->state |= STATE_TAPPED;
        put_into_play(src_player, card_added);
        if (num_chosen == 1)
          result = card_added;
      }
    }
    return result;
  }
  sprintf(text.diagnostic, LITERAL_BATCH, LITERAL_SOURCE, 999,
          (int)destination_chosen, num_chosen);
  backtrace_and_abort(text.diagnostic);
  /* Native physical epilogue; backtrace_and_abort normally does not return. */
  return num_chosen;
}'''

TYPE_FRONTEND=r'''int reveal_top_cards_of_library_and_choose_type(int src_player, int src_card,
    int t_player, int num_cards, int must_select, tutor_destination_2022_t destination_chosen,
    int reveal_chosen, tutor_destination_2022_t destination_rest, int reveal_rest,
    type_t legal_types)
{
  test_definition_2022_t test;
  if ((legal_types & ~TYPE_EFFECT) == TYPE_NONEFFECT || legal_types == TYPE_NONE)
    default_test_definition(&test, TYPE_ANY);
  else
  {
    new_default_test_definition(&test, legal_types, "Select a ");
    if (ai_is_speculating != 1)
    {
      int length = strlen(test.message);
      type_text(test.message + length, 100 - length, legal_types, TYPETEXT_0);
      length = strlen(test.message);
      scnprintf(test.message + length, 100 - length, " card.");
    }
  }
  return reveal_top_cards_of_library_and_choose_fn_who_chooses(src_player, src_card,
      t_player, src_player, num_cards, must_select, destination_chosen, reveal_chosen,
      destination_rest, reveal_rest, &test, NULL);
}'''
