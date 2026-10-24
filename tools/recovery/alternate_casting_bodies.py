"""Typed native two-row hand/graveyard selection arrays and per-card state."""

def hand_legal(action):
    return '''int legal = can_legally_play_card_in_hand(player, i);
        if ((legal == 99 && (is_what(player, i, 0x20) || is_what(player, i, 0x22))) ||
            (legal == 1 && (is_what(player, i, 0x10) || can_sorcery_be_played(player, EVENT_CAN_ACTIVATE) ||
                           is_what(player, i, 2) || can_sorcery_be_played(player, EVENT_CAN_ACTIVATE) ||
                           flash(player, i, EVENT_CAN_ACTIVATE)))) {
          '''+action+'''
        }'''

HAND_ADD='playable[0][count] = get_card_instance(player, i)->internal_card_id; playable[1][count++] = i;'
BODIES = {
'22d9770':r'''
int card_as_foretold(int player, int card, event_t event)
{
  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY) {
    add_counters_from_source(player, card, player, card, COUNTER_TIME, 1);
  } else if (event == EVENT_CAN_ACTIVATE) {
    for (int i = 0; i < active_cards_count[player]; ++i) {
      if (in_hand(player, i) && !is_what(player, i, 1) &&
          get_cmc(player, i) <= count_counters(player, card, COUNTER_TIME) &&
          get_card_instance(player, card)->targets[3].player != 0x42) {
        @CAN@
      }
    }
    return 0;
  } else if (event == EVENT_ACTIVATE) {
    unsigned capacity = (unsigned)hand_count[player];
    int playable[2][capacity];
    int count = 0;
    for (int i = 0; i < active_cards_count[player]; ++i) {
      if (in_hand(player, i) && !is_what(player, i, 1) &&
          get_cmc(player, i) <= count_counters(player, card, COUNTER_TIME) &&
          get_card_instance(player, card)->targets[3].player != 0x42) {
        @ADD@
      }
    }
    test_definition_2022_t test;
    _Static_assert(sizeof(test) == 216, "native As Foretold filter");
    new_default_test_definition_2022(&test, 1, "Select a spell to cast.");
    test.type_flag = 1;
    int selected = select_card_from_zone_2022(player, player, playable[0], count, 0, 2, -1, &test);
    if (selected == -1) { spell_fizzled = 1; return 0; }
    play_card_in_hand_for_free(player, playable[1][selected]);
    cant_be_responded_to = 1;
    get_card_instance(player, card)->targets[3].player = 0x42;
  } else if (event == EVENT_CLEANUP) {
    get_card_instance(player, card)->targets[3].player = 0;
  }
  return global_enchantment(player, card, event);
}
'''.replace('@CAN@',hand_legal('return legal;')).replace('@ADD@',hand_legal(HAND_ADD)),
'25cf1b0':r'''
int card_fires_of_invention(int player, int card, event_t event)
{
  if (!is_humiliated_for_ability(player, card, AL_STATIC)) {
    if (event == EVENT_MODIFY_COST_GLOBAL) {
      if (affected_card_controller == player && !is_what(player, affected_card, 1) &&
          (get_specific_storm_count_2022(affected_card_controller) > 1 || current_turn != player))
        infinite_casting_cost();
    } else if (event == EVENT_CAN_ACTIVATE) {
      if (get_specific_storm_count_2022(player) > 1 || current_turn != player) return 0;
      int lands = count_permanents_by_type(player, TYPE_LAND);
      for (int i = 0; i < active_cards_count[player]; ++i) {
        if (in_hand(player, i) && !is_what(player, i, 1) && get_cmc(player, i) <= lands) {
          @CAN@
        }
      }
      return 0;
    } else if (event == EVENT_ACTIVATE && get_specific_storm_count_2022(player) < 2 && current_turn == player) {
      unsigned capacity = (unsigned)hand_count[player];
      int playable[2][capacity];
      int lands = count_permanents_by_type(player, TYPE_LAND);
      int count = 0;
      for (int i = 0; i < active_cards_count[player]; ++i) {
        if (in_hand(player, i) && !is_what(player, i, 1) && get_cmc(player, i) <= lands) {
          @ADD@
        }
      }
      char buffer[100];
      scnprintf(buffer, 100, "Select a non-land card with mana value less than or equal to %d to play.", lands);
      test_definition_2022_t test;
      _Static_assert(sizeof(test) == 216, "native Fires of Invention filter");
      new_default_test_definition_2022(&test, 1, buffer);
      test.cmc = lands + 1;
      test.cmc_flag = 3;
      test.type_flag = 1;
      int selected = select_card_from_zone_2022(player, player, playable[0], count, 0, 2, -1, &test);
      if (selected == -1) { spell_fizzled = 1; return 0; }
      play_card_in_hand_for_free(player, playable[1][selected]);
      cant_be_responded_to = 1;
    }
  }
  return global_enchantment(player, card, event);
}
'''.replace('@CAN@',hand_legal('return legal;')).replace('@ADD@',hand_legal(HAND_ADD)),
'22ceb70':r'''
int card_jodah_archmage_eternal(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  if (event == EVENT_CAN_ACTIVATE) {
    if (has_mana_multi(player, 0, 1, 1, 1, 1, 1)) {
      int result = 0;
      for (int i = 0; i < active_cards_count[player]; ++i) {
        if (in_hand(player, i) && !is_what(player, i, 1)) {
          int legal = can_legally_play_iid(player, get_original_internal_card_id(player, i));
          if (legal != 0 && (player == 0 ||
              (player == 1 && !has_mana_to_cast_id(1, EVENT_CAN_CAST, get_id(1, i))))) {
            result = legal;
            if (legal == 99) return 99;
          }
        }
      }
      return result;
    }
  } else if (event == EVENT_ACTIVATE) {
    charge_mana_multi(player, 0, 1, 1, 1, 1, 1);
    if (spell_fizzled != 1) {
      unsigned capacity = (unsigned)hand_count[player];
      int playable[2][capacity];
      int count = 0;
      for (int i = 0; i < active_cards_count[player]; ++i) {
        if (in_hand(player, i) && !is_what(player, i, 1)) {
          int legal = can_legally_play_iid(player, get_original_internal_card_id(player, i));
          if (legal != 0 && (player == 0 ||
              (player == 1 && !has_mana_to_cast_id(1, EVENT_CAN_CAST, get_id(1, i))))) {
            playable[0][count] = get_original_internal_card_id(player, i);
            playable[1][count++] = i;
          }
        }
      }
      test_definition_2022_t test;
      _Static_assert(sizeof(test) == 216, "native Jodah filter");
      new_default_test_definition_2022(&test, 0xff, "Select a card to play.");
      test.zone = 0x100;
      int selected = select_card_from_zone_2022(player, player, playable[0], count, 0, 2, -1, &test);
      if (selected == -1) spell_fizzled = 1;
      else {
        play_card_in_hand_for_free(player, playable[1][selected]);
        cant_be_responded_to = 1;
      }
    }
  }
  return 0;
}
''',
'20d5330':r'''
int card_abandoned_sarcophagus(int player, int card, event_t event)
{
  enable_xtrigger_flags |= 0xa;
  if (event == EVENT_GRAVEYARD_FROM_PLAY) {
    if (card_is_being_cycled != 0) return 0;
    test_definition_2022_t test;
    default_test_definition_2022(&test, 0xff);
    test.special_selection_function = has_cycling;
    if_a_card_would_be_put_into_graveyard_from_anywhere_exile_it_instead_2022(player, card, event, player, &test);
  } else if ((xtrigger_condition() == XTRIGGER_REPLACE_CARD_TO_GY_FROM_ANYWHERE_BUT_LIBRARY ||
              xtrigger_condition() == 8) && card_is_being_cycled == 0) {
    test_definition_2022_t test;
    default_test_definition_2022(&test, 0xff);
    test.special_selection_function = has_cycling;
    if_a_card_would_be_put_into_graveyard_from_anywhere_exile_it_instead_2022(player, card, event, player, &test);
  }
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE &&
      event != EVENT_RESOLVE_ACTIVATION && event != EVENT_CLEANUP) return 0;
  if (is_humiliated_for_ability(player, card, AL_STATIC)) return 0;
  if (event == EVENT_CAN_ACTIVATE) {
    for (int i = 0; get_grave(player)[i] != -1; ++i) {
      int iid = get_grave(player)[i];
      if (cards_data[iid].cc[2] == 4 && !is_what(-1, iid, 1)) {
        int legal = can_legally_play_iid(player, get_grave(player)[i]);
        if (legal != 0 && has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, get_grave(player)[i])) {
          if (legal == 99) return 99;
          if (can_sorcery_be_played(player, EVENT_CAN_ACTIVATE) ||
              (cards_data[get_grave(player)[i]].type & 0x30) != 0) return 1;
        }
      }
    }
    return 0;
  }
  if (event != EVENT_ACTIVATE) return 0;
  unsigned capacity = (unsigned)count_graveyard(player);
  int playable[2][capacity];
  int count = 0;
  for (int i = 0; get_grave(player)[i] != -1; ++i) {
    int iid = get_grave(player)[i];
    if (cards_data[iid].cc[2] == 4 && !is_what(-1, iid, 1) &&
        can_legally_play_iid(player, get_grave(player)[i]) &&
        has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, get_grave(player)[i]) &&
        (can_sorcery_be_played(player, EVENT_CAN_ACTIVATE) ||
         (cards_data[get_grave(player)[i]].type & 0x30) != 0)) {
      playable[0][count] = get_grave(player)[i];
      playable[1][count++] = i;
    }
  }
  test_definition_2022_t test;
  _Static_assert(sizeof(test) == 216, "native Sarcophagus filter");
  new_default_test_definition_2022(&test, 0xff, "Select a card to play.");
  int selected = select_card_from_zone_2022(player, player, playable[0], count, 0, 1, -1, &test);
  if (selected != -1 && charge_mana_from_id(player, -1, EVENT_ACTIVATE, (uint16_t)cards_data[playable[0][selected]].id)) {
    play_card_in_grave_for_free(player, player, playable[1][selected]);
    cant_be_responded_to = 1;
    return 0;
  }
  spell_fizzled = 1;
  return 0;
}
''',
'228b340':r'''
int card_lurrus_of_the_dream_den(int player, int card, event_t event)
{
  if (event == EVENT_QUERY && attacking_card == 2) {
    const int *deck = deck_ptr[player];
    int lurrus_count = 0;
    for (int i = 0; i < 500 && deck[i] != -1; ++i) {
      if (is_what(-1, deck[i], 0x47) && get_cmc_by_internal_id(deck[i]) > 2) {
        int iid = deck[i];
        int lurrus_iid = get_internal_card_id_from_csv_id(0x5359);
        if (lurrus_count != 0 || iid != lurrus_iid) return 0;
        lurrus_count = 1;
      }
    }
    for (int i = 0; i < active_cards_count[player]; ++i) {
      if (!is_what(player, i, 0x47) || get_cmc(player, i) < 3) return 0;
      int id = get_id(player, i);
      if (lurrus_count != 0 || id != 0x5359) return 0;
      lurrus_count = 1;
    }
    return 1;
  }
  check_legend_rule(player, card, event);
  lifelink(player, card, event);
  if (event == EVENT_QUERY) return 0;
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE &&
      event != EVENT_RESOLVE_ACTIVATION && event != EVENT_CLEANUP) return 0;
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE) {
    if (is_humiliated_for_ability(player, card, AL_STATIC) || current_turn != player || instance->info_slot == 0x42)
      return 0;
    const int *grave = get_grave(player);
    int result = 0;
    for (int i = 0; i < count_graveyard(player); ++i) {
      if (is_what(-1, grave[i], 0x47) && !is_what(-1, grave[i], 1) &&
          get_cmc_by_internal_id(grave[i]) <= 2 && has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, grave[i]) &&
          (is_what(-1, grave[i], 0x30) || can_sorcery_be_played(player, EVENT_CAN_ACTIVATE))) {
        int legal = can_legally_play_iid(player, grave[i]);
        if (legal == 99) return 99;
        if (legal != 0) result = legal;
      }
    }
    return result;
  }
  if (event == EVENT_ACTIVATE) {
    unsigned capacity = (unsigned)count_graveyard(player);
    int playable[2][capacity];
    const int *grave = get_grave(player);
    int count = 0;
    for (int i = 0; i < count_graveyard(player); ++i) {
      if (is_what(-1, grave[i], 0x47) && !is_what(-1, grave[i], 1) &&
          get_cmc_by_internal_id(grave[i]) < 3 && has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, grave[i]) &&
          (is_what(-1, grave[i], 0x30) || can_sorcery_be_played(player, EVENT_CAN_ACTIVATE)) &&
          can_legally_play_iid(player, grave[i])) {
        playable[0][count] = grave[i];
        playable[1][count++] = i;
      }
    }
    test_definition_2022_t test;
    _Static_assert(sizeof(test) == 216, "native Lurrus filter");
    new_default_test_definition_2022(&test, 0xff, "Select a permanent card to play.");
    int selected = select_card_from_zone_2022(player, player, playable[0], count, 0, 1, -1, &test);
    if (selected != -1 && charge_mana_from_id(player, -1, EVENT_ACTIVATE, (uint16_t)cards_data[playable[0][selected]].id)) {
      instance->info_slot = 0x42;
      play_card_in_grave_for_free(player, player, playable[1][selected]);
      cant_be_responded_to = 1;
      return 0;
    }
    spell_fizzled = 1;
  } else if (event == EVENT_CLEANUP) instance->info_slot = 0;
  return 0;
}
''',
'22d0bd0':r'''
int card_muldrotha_the_gravetide(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  card_instance_t *instance = get_card_instance(player, card);
  for (int i = 0; i < active_cards_count[player]; ++i) {
    card_instance_t *leg = in_play(player, i);
    if (leg && leg->internal_card_id == LEGACY_EFFECT_CUSTOM &&
        leg->info_slot == (int32_t)(intptr_t)muldrotha_legacy &&
        leg->damage_source_player == player && leg->damage_source_card == card &&
        leg->damage_target_player == player && leg->damage_target_card == card) {
      instance = get_card_instance(player, i);
      break;
    }
  }
  if (event == EVENT_STATIC_EFFECTS && affect_me(player, card)) {
    int found = 0;
    for (int i = 0; i < active_cards_count[player]; ++i) {
      if (in_play(player, i) && is_what(player, i, 0x80)) {
        card_instance_t *leg = get_card_instance(player, i);
        if (leg->info_slot == (int32_t)(intptr_t)muldrotha_legacy &&
            ((leg->damage_target_player == player && leg->damage_target_card == card) ||
             (leg->targets[0].player == player && leg->targets[0].card == card))) {
          found = 1;
          break;
        }
      }
    }
    if (!found) {
      int legacy = create_targetted_legacy_effect(player, card, muldrotha_legacy, player, card);
      card_instance_t *leg = get_card_instance(player, legacy);
      leg->targets[0].player = player;
      leg->targets[0].card = card;
      leg->number_of_targets = 1;
      add_status(player, legacy, 0x10000);
    }
  }
  if (!in_play(player, card) || is_humiliated_for_ability(player, card, AL_STATIC)) return 0;
  if (event == EVENT_CAN_ACTIVATE) {
    if (current_turn == player) {
      int used_types = instance->targets[1].player;
      if (used_types == -1) instance->targets[1].player = used_types = 0;
      for (int i = 0; get_grave(player)[i] != -1; ++i) {
        int types = get_type_with_iid(-1, -1, get_grave(player)[i]);
        if ((types & 0x147) != 0 && (types | used_types) != used_types) {
          int legal = can_legally_play_iid_now(player, get_grave(player)[i], EVENT_CAN_ACTIVATE);
          if (legal != 0 && has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, get_grave(player)[i])) return legal;
        }
      }
    }
  } else if (event == EVENT_ACTIVATE) {
    int used_types = instance->targets[1].player;
    unsigned capacity = (unsigned)count_graveyard(player);
    int playable[2][capacity];
    int count = 0;
    for (int i = 0; get_grave(player)[i] != -1; ++i) {
      int types = get_type_with_iid(-1, -1, get_grave(player)[i]);
      if ((types & 0x147) != 0 && (types | used_types) != used_types &&
          can_legally_play_iid_now(player, get_grave(player)[i], EVENT_ACTIVATE) &&
          has_mana_to_cast_iid(player, EVENT_ACTIVATE, get_grave(player)[i])) {
        playable[0][count] = get_grave(player)[i];
        playable[1][count++] = i;
      }
    }
    test_definition_2022_t test;
    _Static_assert(sizeof(test) == 216, "native Muldrotha filter");
    new_default_test_definition_2022(&test, 0xff, "Select a permanent card to play.");
    int selected = select_card_from_zone_2022(player, player, playable[0], count, 0, 1, -1, &test);
    if (selected != -1 && charge_mana_from_id(player, -1, EVENT_ACTIVATE, (uint16_t)cards_data[playable[0][selected]].id)) {
      int types = get_type_with_iid(-1, -1, playable[0][selected]);
      if (num_bits_set(types) < 2) instance->targets[1].player |= types;
      else {
        int choice = dialog_impl(player, card, EVENT_ACTIVATE,
            "\001\002", "\001\014", "\001\003", "\001\004", "\001\016",
            "Artifact", (types & 0x40) != 0 && (used_types & 0x40) == 0, 1,
            "Creature", (types & 2) != 0 && (used_types & 2) == 0, 1,
            "Enchantment", (types & 4) != 0 && (used_types & 4) == 0, 1,
            "Land", (types & 1) != 0 && (used_types & 1) == 0, 1,
            "Planeswalker", (types & 0x100) != 0 && (used_types & 0x100) == 0, 1,
            0, 0, 0, 0);
        switch (choice) {
          case 1: instance->targets[1].player |= 0x40; break;
          case 2: instance->targets[1].player |= 2; break;
          case 3: instance->targets[1].player |= 4; break;
          case 4: instance->targets[1].player |= 1; break;
          case 5: instance->targets[1].player |= 0x100; break;
          default: spell_fizzled = 1; return 0;
        }
      }
      play_card_in_grave_for_free(player, player, playable[1][selected]);
      cant_be_responded_to = 1;
      return 0;
    }
    spell_fizzled = 1;
  }
  return 0;
}
'''
}
