"""Native tutor reconstruction: dynamic selection storage and all destination arms."""

BODY = r'''int new_global_tutor_2022(int player, int targ_player, int search_location,
    int destination, int must_select, int ai_selection_mode, test_definition_2022_t *this_test)
{
  int avoid_shuffle = this_test->no_shuffle;
  int test_type = new_get_test_score_2022(this_test);
  int capacity = this_test->qty;
  int tutored[capacity];
  int selector = 1 - player;
  int opposition_agent = check_battlefield_for_id(selector, 0x56a2);
  int self_search = player == targ_player;
  int shuffle_after = avoid_shuffle == 0 && search_location == TUTOR_FROM2022_DECK;
  int shuffle_before;
  int zone;
  if (!opposition_agent || player != targ_player || search_location != TUTOR_FROM2022_DECK)
  {
    shuffle_before = destination == TUTOR2022_DECK && shuffle_after;
    shuffle_after = shuffle_after && destination != TUTOR2022_DECK;
    selector = player;
    zone = search_location == TUTOR_FROM2022_GRAVE_NOTARGET ? TUTOR_FROM2022_GRAVE : search_location;
  }
  else
  {
    shuffle_before = 0;
    self_search = selector == targ_player;
    destination = TUTOR2022_OPPOSITION_AGENT;
    zone = TUTOR_FROM2022_DECK;
  }

  int tc = 0;
  int cancelled = 0;
  int card_added = -1;
  int really_tutored = 0;
  if (this_test->qty != 0)
  {
    unsigned int q = 0;
    do
    {
      int selected = -1;
      if (zone == TUTOR_FROM2022_DECK_AUTO)
      {
        if (!get_tutoring_denial_2022(selector))
        {
          unsigned int starting_card = 0;
          if (check_battlefield_for_id(1 - targ_player, 0x22b9) && count_deck(targ_player) > 3)
            starting_card = count_deck(targ_player) - 4;
          while ((int)starting_card < count_deck(selector))
          {
            if (new_make_test_2022(targ_player, deck_ptr[targ_player][starting_card], test_type, this_test))
            {
              selected = starting_card;
              break;
            }
            ++starting_card;
          }
        }
        zone = TUTOR_FROM2022_DECK;
      }
      else
        selected = new_select_a_card_2022(selector, targ_player, search_location,
            must_select, ai_selection_mode, test_type, this_test);

      if (selected == -1)
      {
        if (this_test->qty > 1)
          break;
        cancelled = -2;
      }
      else
      {
        if (zone == TUTOR_FROM2022_GRAVE || zone == TUTOR_FROM2022_DECK)
        {
          int *s_location = zone == TUTOR_FROM2022_GRAVE ? graveyard_ptr_mutable[targ_player] : deck_ptr[targ_player];
          if (zone == TUTOR_FROM2022_DECK && selector == targ_player)
            set_trap_condition(targ_player, 0x600, 1);
          int from = zone == TUTOR_FROM2022_GRAVE ? 2 : 1;
          if (!grafdiggers_cages(targ_player, (type_t)cards_data[s_location[selected]].type, from) ||
              ((unsigned int)(destination - TUTOR2022_PLAY) > 1 && destination != TUTOR2022_PLAY_ATTACKING))
          {
            tutored[tc++] = s_location[selected];
            if (zone == TUTOR_FROM2022_GRAVE)
              remove_card_from_grave(targ_player, selected);
            else
              remove_card_from_deck(targ_player, selected);
          }
          else
            tutored[tc] = -1;
        }
        else if (zone == TUTOR_FROM2022_RFG)
        {
          int iid = rfg_ptr[targ_player][selected];
          tutored[tc++] = iid;
          remove_card_from_rfg(targ_player, cards_data[iid].id);
        }
        else if (zone == TUTOR_FROM2022_HAND)
        {
          if (destination == TUTOR2022_GET_ID)
            selected = get_id(targ_player, selected);
          tutored[tc++] = selected;
          get_card_instance(targ_player, selected)->state |= 0x20;
        }
        if (this_test->create_minideck > 1)
          --this_test->create_minideck;
      }
      ++q;
    } while (q < this_test->qty);
  }

  if (shuffle_before)
    shuffle(targ_player);
  else if (search_location == TUTOR_FROM2022_HAND)
    for (int q = 0; q < tc; ++q)
      get_card_instance(targ_player, tutored[q])->state &= ~0x20U;

  int storage_loc = 0;
  for (int q = 0; q < tc; ++q)
  {
    int iid = tutored[q];
    if (iid == -1)
      continue;
    ++really_tutored;
    switch (destination)
    {
    case TUTOR2022_HAND:
    case TUTOR2022_TPLAYER_HAND:
    case TUTOR2022_HAND_AND_MARK:
    {
      int dest_player = destination == TUTOR2022_TPLAYER_HAND ? targ_player : selector;
      card_added = add_card_to_hand(dest_player, iid);
      int reveal = selector == (destination == TUTOR2022_HAND) && avoid_shuffle != 2;
      if (reveal && ((unsigned int)(zone - TUTOR_FROM2022_GRAVE) < 2 ||
          (test_type != 0 && (test_type != 1 || this_test->type != 0xff))))
        reveal_card(selector, card_added, -1, -1);
      if (destination == TUTOR2022_HAND_AND_MARK)
        add_state(dest_player, card_added, 0x200000);
      if (zone == TUTOR_FROM2022_GRAVE)
        from_grave_to_hand_triggers(dest_player, card_added);
      break;
    }
    case TUTOR2022_DECK:
      if (zone == TUTOR_FROM2022_HAND)
        card_added = iid;
      if ((test_type == 0 || (test_type == 1 && this_test->type == 0xff)) || selector != 1)
      {
        if (zone != TUTOR_FROM2022_HAND)
        {
          put_iid_under_the_first_x_cards_of_library(targ_player, iid, 0);
          break;
        }
      }
      else if (zone == TUTOR_FROM2022_HAND)
        reveal_card(1, card_added, -1, -1);
      else
      {
        card_added = add_card_to_hand_invisible(targ_player, iid);
        reveal_card(1, card_added, -1, -1);
      }
      put_on_top_of_deck(targ_player, card_added);
      break;
    case TUTOR2022_GRAVE:
      if (zone == TUTOR_FROM2022_HAND)
      {
        new_discard_card(targ_player, iid, selector, 0);
        card_added = iid;
      }
      else
      {
        card_added = add_card_to_hand(targ_player, iid);
        if (zone == TUTOR_FROM2022_DECK)
        {
          put_on_top_of_deck(targ_player, card_added);
          mill(targ_player, 1);
        }
        else
          kill_card(targ_player, card_added, KILL_DESTROY);
      }
      break;
    case TUTOR2022_PLAY:
    case TUTOR2022_PLAY_TAPPED:
    case TUTOR2022_PLAY_ATTACKING:
    {
      card_added = zone == TUTOR_FROM2022_HAND ? iid : add_card_to_hand(targ_player, iid);
      card_instance_t *instance = get_card_instance(targ_player, card_added);
      if (targ_player != selector)
      {
        if (selector == 0)
          add_state(targ_player, card_added, 0x1000);
        else
          remove_state(targ_player, card_added, 0x1000);
      }
      if (destination == TUTOR2022_PLAY_TAPPED || destination == TUTOR2022_PLAY_ATTACKING)
        instance->state |= 0x10;
      if (zone == TUTOR_FROM2022_GRAVE)
        set_special_flags3_2022(targ_player, card_added, 1);
      else if (zone == TUTOR_FROM2022_DECK)
        set_special_flags_2022(targ_player, card_added, 0x10000);
      put_into_play(targ_player, card_added);
      if (targ_player != selector)
      {
        card_added = gain_control_impl_2022(targ_player, card_added);
        instance = get_card_instance(selector, card_added);
      }
      if (destination == TUTOR2022_PLAY_ATTACKING)
      {
        instance->state |= 4;
        choose_who_attack(selector, card_added);
      }
      break;
    }
    case TUTOR2022_RFG:
      if (zone == TUTOR_FROM2022_HAND)
      {
        card_added = get_id(targ_player, iid);
        rfg_card_in_hand(targ_player, iid);
      }
      else
      {
        card_added = cards_data[iid].id;
        add_card_to_rfg(targ_player, iid);
      }
      play_sound_effect((wav_t)0x17);
      break;
    case TUTOR2022_BOTTOM_OF_DECK:
    case TUTOR2022_BOTTOM_OF_DECK_RANDOM:
      if (zone == TUTOR_FROM2022_HAND)
      {
        int position = count_deck(targ_player);
        deck_ptr[targ_player][position] = get_original_internal_card_id(targ_player, iid);
        obliterate_card(targ_player, iid);
      }
      else
      {
        card_added = cards_data[iid].id;
        deck_ptr[targ_player][count_deck(targ_player)] = iid;
      }
      break;
    case TUTOR2022_OPPOSITION_AGENT:
    {
      card_added = add_card_to_hand(selector, iid);
      card_instance_t *instance = get_card_instance(selector, card_added);
      instance->state |= 0x20;
      --hand_count[selector];
      reveal_card(selector, card_added, -1, -1);
      add_card_to_rfg(player, iid);
      create_may_play_card_from_exile_effect(selector, card_added, player, cards_data[iid].id, 1);
      obliterate_card(selector, card_added);
      break;
    }
    default:
      break;
    }
    if (this_test->storage >= 0)
      get_card_instance(selector, this_test->storage)->targets[storage_loc++].card = iid;
  }
  if (search_location == TUTOR_FROM2022_DECK_AUTO && self_search)
    side_effects_to_tutor_deck_from_permanents_2022(selector, targ_player);
  if (shuffle_after)
    shuffle(targ_player);
  if (cancelled == -2)
    return -2;
  return this_test->qty < 2 ? card_added : really_tutored;
}
'''

GAIN_CONTROL_BODY = r'''int gain_control_impl_2022(int player, int card)
{
  card_instance_t *src = get_card_instance(player, card);
  uint32_t old_timestamp = src->timestamp;
  int new_player = 1 - player;
  int new_card = add_card_to_hand_exe(new_player, src->internal_card_id);
  if (new_card != -1)
  {
    card_instance_t *dest = get_card_instance(new_player, new_card);
    memcpy_inst_and_ext(dest, src);
    dest->state = (dest->state & 0xfffffff3U) | 0x30000U;
    timestamp_card[old_timestamp] = new_card;
    timestamp_player[old_timestamp] = new_player;
    for (int p = 0; p < 2; ++p)
      for (int c = 0; c < active_cards_count[p]; ++c)
      {
        card_instance_t *instance = get_card_instance(p, c);
        if (instance->damage_target_player == player && instance->damage_target_card == card)
        {
          instance->damage_target_player = new_player;
          instance->damage_target_card = new_card;
        }
        if (instance->damage_source_player == player && instance->damage_source_card == card)
        {
          instance->damage_source_player = new_player;
          instance->damage_source_card = new_card;
        }
        unsigned int count = instance->number_of_targets;
        for (unsigned int i = 0; i < count; ++i)
          if (instance->targets[i].player == player && instance->targets[i].card == card)
          {
            instance->targets[i].player = new_player;
            instance->targets[i].card = new_card;
          }
      }
  }
  uint8_t type = cards_data[src->internal_card_id].type;
  if ((type & 2) != 0)
    ++creature_count[new_player];
  if ((type & 0x40) != 0)
    ++artifact_count[new_player];
  if ((type & 4) != 0)
    ++enchantment_count[new_player];
  types_of_cards_on_bf[new_player] |= type;
  redraw_libraries();
  src->token_status |= 0x40000008U;
  kill_card_exe(player, card, KILL_EXILE);
  if (((uint32_t)event_flags & 0x10000000U) != 0)
    dispatch_event(new_player, new_card, (event_t)0x94);
  return new_card;
}
'''
