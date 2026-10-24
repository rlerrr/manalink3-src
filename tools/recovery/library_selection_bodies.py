"""Structured native VLA/call-frame reconstructions; literals are resolved from release bytes."""
BODIES={
'20d0ed0':'''int card_hazorets_undying_fury(int player, int card, event_t event)
{
  if (event == EVENT_RESOLVE_SPELL) {
    shuffle(player);
    if (deck_ptr[player][0] != -1) {
      int amount = count_deck(player);
      if (amount > 0) {
        if (amount > 4) amount = 4;
        int *deck = deck_ptr[player];
        show_deck(0, deck, amount, "Cards exiled by Hazoret's Undying Fury", 0, str_Done);
        int haz[amount];
        int haz_count = 0;
        for (int i = amount - 1; i != -1; --i) {
          if (!is_what(-1, deck[i], TYPE_LAND) &&
              get_cmc_by_id((uint16_t)cards_data[deck[i]].id) < 6 &&
              can_legally_play_iid(player, deck[i])) {
            haz[haz_count++] = deck[i];
          }
          rfg_card_in_deck(player, i);
        }
        while (haz_count > 0) {
          test_definition_t this_test;
          new_default_test_definition(&this_test, TYPE_ANY, LIT_02729e54);
          int selected = select_card_from_zone(player, player, haz, haz_count, 0, 1, -1, &this_test);
          if (selected == -1) break;
          unsigned int csvid = (uint16_t)cards_data[haz[selected]].id;
          /* Native compaction reads haz[haz_count], one extra entry. */
          for (int k = selected; k < haz_count; ++k) haz[k] = haz[k + 1];
          --haz_count;
          play_card_in_exile_for_free(player, 1 - player, csvid);
        }
      }
    }
    int legacy = create_legacy_effect(player, card, hou_finality_effect);
    get_card_instance(player, legacy)->targets[1].card = 1;
    kill_card(player, card, KILL_DESTROY);
  }
  return basic_spell(player, card, event);
}
''',
'2215840':'''int card_sunbirds_invocation(int player, int card, event_t event)
{
  if (trigger_condition == 0xd3 && card == affected_card &&
      player == affected_card_controller && reason_for_trigger_controller == player) {
    if (!not_played_from_hand(trigger_cause_controller, trigger_cause))
      sp_ability_trigger(player, event, 2, TYPE_ANY, NULL, player, card);
  }
  if (event == EVENT_SPA) {
    int amount = get_cmc(trigger_cause_controller, trigger_cause);
    int num_cards = count_deck(player);
    if (amount <= num_cards) num_cards = amount;
    if (num_cards == 0) return 0;
    show_deck(player, deck_ptr[player], num_cards, "Cards revealed by Sunbird's Invocation.", 0, str_Done);
    int selection_array[2][num_cards];
    int eligible = 0;
    if (num_cards > 0) {
      for (int i = 0; i != num_cards; ++i) {
        int iid = deck_ptr[player][i];
        selection_array[0][i] = iid;
        selection_array[1][i] = iid;
        if (!can_legally_play_iid(player, iid) ||
            get_cmc_by_internal_id(deck_ptr[player][i]) > get_cmc(trigger_cause_controller, trigger_cause) ||
            (cards_data[deck_ptr[player][i]].type & TYPE_LAND)) {
          selection_array[1][i] = iid_draw_a_card;
        } else ++eligible;
      }
      for (int i = 0; i != num_cards; ++i) remove_card_from_deck(player, 0);
    }
    int selected_iid = -1;
    if (eligible != 0) {
      test_definition_t this_test;
      new_default_test_definition(&this_test, TYPE_LAND, LIT_02735f30);
      this_test.type_flag = 1;
      this_test.id = 0x388;
      this_test.id_flag = 1;
      int s = select_card_from_zone(player, player, selection_array[1], num_cards, 0, 2, -1, &this_test);
      if (s != -1) {
        selected_iid = selection_array[0][s];
        selection_array[0][s] = -1;
        selection_array[1][s] = iid_draw_a_card;
      }
    }
    for (int i = num_cards - 1; i >= 0; --i)
      if (selection_array[0][i] != -1) raw_put_iid_on_top_of_deck(player, selection_array[0][i]);
    int bottom_count = num_cards;
    if (selected_iid != -1) {
      fake_play_card_in_zone_for_free(player, player, selected_iid, 0x10000);
      bottom_count = num_cards - 1;
    }
    put_top_x_on_bottom_in_random_order(player, bottom_count);
  }
  return global_enchantment(player, card, event);
}
''',
'2217170':'''int card_gishath_suns_avatar(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  vigilance(player, card, event);
  haste(player, card, event);
  if (damage_dealt_by_me(player, card, event,
       DDBM_MUST_DAMAGE_PLAYER | DDBM_REPORT_DAMAGE_DEALT | DDBM_MUST_BE_COMBAT_DAMAGE)) {
    int amount = get_card_instance(player, card)->targets[16].player;
    if (amount == -1) return 0;
    int count = count_deck(player);
    if (count < amount) amount = count;
    if (amount != 0) {
      int gc_flag = grafdiggers_cages(player, TYPE_CREATURE, TUTOR_FROM_DECK);
      show_deck(0, deck_ptr[player], amount, "Cards revealed by Gishath, Sun's Avatar", 0, str_Done);
      show_deck(1, deck_ptr[player], amount, "Cards revealed by Gishath, Sun's Avatar", 0, str_Done);
      if (gc_flag == 0) {
        int deck_copy[amount];
        for (int i = 0; i < amount; ++i) {
          deck_copy[i] = deck_ptr[player][0];
          remove_card_from_deck(player, 0);
        }
        char msg[100];
        scnprintf(msg, 100, LIT_0273607c);
        test_definition_t test;
        new_default_test_definition(&test, TYPE_CREATURE, msg);
        test.subtype = 0xf0;
        int remaining = amount;
        do {
          int selected = select_card_from_zone(player, player, deck_copy, remaining, 0, 1, -1, &test);
          if (selected == -1) {
            if (remaining != 0) {
              for (int i = 0; i < remaining; ++i) raw_put_iid_on_top_of_deck(player, deck_copy[i]);
              put_top_x_on_bottom_in_random_order(player, remaining);
            }
            break;
          }
          if (deck_copy[selected] != -1)
            fake_play_card_in_zone_for_free(player, player, deck_copy[selected], 0x10000);
          --remaining;
          for (int i = selected; i < remaining; ++i) deck_copy[i] = deck_copy[i + 1];
        } while (remaining != 0);
      }
    }
  }
  if (event == EVENT_PHASE_CHANGED && current_phase == 0x22)
    get_card_instance(player, card)->targets[16].player = 0;
  return 0;
}
''',
'21d3830':'''int card_niv_mizzet_reborn(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB) {
    int num_cards = count_deck(player);
    if (num_cards != 0) {
      if (num_cards > 10) num_cards = 10;
      show_deck(current_turn, deck_ptr[player], num_cards, "Cards revealed by Niv-Mizzet Reborn", 0, str_Done);
      show_deck(current_turn ^ 1, deck_ptr[player], num_cards, "Cards revealed by Niv-Mizzet Reborn", 0, str_Done);
      int deck_copy[num_cards];
      for (int i = 0; i < num_cards; ++i) {
        deck_copy[i] = deck_ptr[player][0];
        remove_card_from_deck(player, 0);
      }
      char msg[100];
      const char *prompt = LIT_02732eec;
      /* The native call supplies this extra argument even though the format has no conversion. */
      scnprintf(msg, 100, prompt, num_cards);
      test_definition_t test;
      new_default_test_definition(&test, TYPE_ANY, msg);
      int chosen = 0;
      int remaining = num_cards;
      do {
        test.special_selection_function = not_selected;
        test.value_for_special_selection_function = chosen;
        int selected = select_card_from_zone(player, player, deck_copy, remaining, 0, 1, -1, &test);
        if (selected == -1) {
          if (remaining == 0) return 0;
          for (int i = 0; i < remaining; ++i) raw_put_iid_on_top_of_deck(player, deck_copy[i]);
          put_top_x_on_bottom_in_random_order(player, remaining);
          return 0;
        }
        --remaining;
        add_card_to_hand(player, deck_copy[selected]);
        chosen |= colors_to_guilds(deck_copy[selected]);
        for (int i = selected; i < remaining; ++i) deck_copy[i] = deck_copy[i + 1];
      } while (remaining != 0);
    }
  }
  return 0;
}
''',
'22cefa0':'''int card_kamahls_druidic_vow(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST) {
    if (check_battlefield_for_subtype(player, TYPE_CREATURE, 0x2001)) return 1;
    return check_battlefield_for_subtype(player, 0x100, -1) != 0;
  }
  if (event == EVENT_CAST_SPELL) {
    if (card == affected_card && player == affected_card_controller) {
      generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
      if (spell_fizzled != 1) {
        card_instance_t *instance = get_card_instance(player, card);
        ai_modifier += instance->info_slot > 5 ? instance->info_slot * 12 : -48;
      }
    }
    return 0;
  }
  if (event == EVENT_RESOLVE_SPELL) {
    card_instance_t *instance = get_card_instance(player, card);
    int num_cards = count_deck(player);
    if (instance->info_slot < num_cards) num_cards = instance->info_slot;
    if (num_cards != 0) {
      int gc_flag = grafdiggers_cages(player, TYPE_CREATURE, TUTOR_FROM_DECK);
      show_deck(player, deck_ptr[player], num_cards, "Cards revealed by Kamahl's Druidic Vow", 0, str_Done);
      if (gc_flag == 0) {
        int deck_copy[num_cards];
        for (int i = 0; i < num_cards; ++i) {
          deck_copy[i] = deck_ptr[player][0];
          remove_card_from_deck(player, 0);
        }
        char msg[100];
        scnprintf(msg, 100, LIT_0273d068, num_cards);
        test_definition_t test;
        new_default_test_definition(&test, 0x47, msg);
        test.cmc = num_cards + 1;
        test.special_selection_function = is_land_or_legendary_by_internal_id;
        test.cmc_flag = 3;
        int remaining = num_cards;
        do {
          int selected = select_card_from_zone(player, player, deck_copy, remaining, 0, 1, -1, &test);
          if (selected == -1) {
            if (remaining != 0) {
              for (int i = 0; i < remaining; ++i) raw_put_iid_on_top_of_deck(player, deck_copy[i]);
              mill(player, remaining);
            }
            break;
          }
          int added = add_card_to_hand(player, deck_copy[selected]);
          set_special_flags(player, added, 0x10000);
          put_into_play(player, added);
          --remaining;
          for (int i = selected; i < remaining; ++i) deck_copy[i] = deck_copy[i + 1];
        } while (remaining != 0);
      }
      kill_card(player, card, KILL_DESTROY);
    }
  } else if (event != EVENT_CAN_CHANGE_TARGET && event != EVENT_CHANGE_TARGET) return 0;
  return generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
}
''',
'2289140':'''int card_emergent_ultimatum(int player, int card, event_t event)
{
  if (event != EVENT_CAN_CAST) {
    if (event == EVENT_CAST_SPELL) {
      if (card != affected_card || player != affected_card_controller) return 0;
    } else if (event == EVENT_RESOLVE_SPELL) {
      int number = count_deck(player);
      if (number > 0) {
        if (number > 3) number = 3;
        iid_t searched[3] = {-1, -1, -1};
        test_definition_t test;
        new_default_test_definition(&test, TYPE_ANY, "Select a monocolored card.");
        test.value_for_special_selection_function = (int)searched;
        test.color_flag = 3;
        test.special_selection_function = test_not_in_array;
        int num_chosen = 0;
        do {
          int selected = new_select_a_card(player, player, 1, 0, 2, -1, &test);
          if (selected < 0) {
            if (num_chosen == 0) return 0;
            break;
          }
          int iid = deck_ptr[player][selected];
          searched[num_chosen++] = iid;
          add_csvid_to_rfg(player, (uint16_t)cards_data[iid].id);
          remove_card_from_deck(player, selected);
        } while (num_chosen < number);
        test.special_selection_function = NULL;
        memcpy(test.message, LIT_0273a174, 55);
        test.value_for_special_selection_function = -1;
        int selected = select_card_from_zone(1 - player, player, searched, num_chosen, 1, 2, 0, &test);
        remove_card_from_rfg(player, (uint16_t)cards_data[searched[selected]].id);
        put_iid_under_the_first_x_cards_of_library(player, searched[selected], 0);
        searched[selected] = -1;
        shuffle(player);
        --num_chosen;
        if (num_chosen == 0) return 0;
        int to_cast[num_chosen];
        int cast_count = 0;
        for (int i = 0; i != 3; ++i) if (searched[i] != -1) to_cast[cast_count++] = searched[i];
        memcpy(test.message, LIT_0273a1ac, 31);
        while (cast_count > 0) {
          test_definition_t this_test2;
          new_default_test_definition(&this_test2, TYPE_ANY, LIT_0273a1ac);
          /* Native chooser uses test, while initializing the separate second test object. */
          int s = select_card_from_zone(player, player, to_cast, cast_count, 0, 2, -1, &test);
          if (s == -1) break;
          unsigned int csvid = (uint16_t)cards_data[to_cast[s]].id;
          /* Native compaction reads one entry beyond the current casting queue. */
          for (int k = s; k < cast_count; ++k) to_cast[k] = to_cast[k + 1];
          --cast_count;
          play_card_in_exile_for_free(player, player, csvid);
        }
      }
      kill_card(player, card, KILL_EXILE);
    } else if (event != EVENT_CAN_CHANGE_TARGET && event != EVENT_CHANGE_TARGET) return 0;
  }
  return basic_spell(player, card, event);
}
'''
}
