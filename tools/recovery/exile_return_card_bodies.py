"""Typed native exile records, fixed pair tables, VLA and complete modal calls."""


def dauthi_dialog(page):
    base=1 if page==0 else 16
    options=',\n          '.join(f'labels[{i}], count > {base+i-1}, weights[{i}]' for i in range(15))
    tail='"Continue >>", count > 15, (best_index < 16 ? 0 : 999)' if page==0 else '"<<Back", allow_back, -999'
    return '''dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION,
          "\\001\\014", "\\001\\002", "\\001\\004", "\\001\\003",
          '''+options+',\n          '+tail+', 0, 0, 0, 0)'


BODIES={
'222b230':r'''
int create_dauthi_voidwalker_legacy(int player, int card, int iid)
{
  enum { COUNTER_VOID = 170 };
  int legacy = create_legacy_effect_for_opponent(player, card, card_exiled_with_void_counter, -1, -1);
  if (legacy != -1) {
    int owner = 1 - player;
    add_counters_from_source(player, card, owner, legacy, COUNTER_VOID, 1);
    card_instance_t *leg = get_card_instance(owner, legacy);
    int source_card = leg->damage_source_card;
    unsigned csvid = (uint16_t)cards_data[iid].id;
    leg->display_pic_csv_id = csvid;
    leg->targets[0].card = iid;
    leg->targets[0].player = owner;
    leg->eot_toughness = 0x300;
    leg->display_pic_num = (uint16_t)get_card_image_number(csvid, leg->damage_source_player, source_card);
  }
  return legacy;
}
''',
'20d6320':r'''
int card_endless_sands(int player, int card, event_t event)
{
  if (event == EVENT_RESOLVE_SPELL) {
    play_land_sound_effect(player, card);
    return mana_producer(player, card, event);
  }
  if (event == EVENT_CAST_SPELL) {
    if (affect_me(player, card) && !is_humiliated_for_ability(player, card, AL_TRIG))
      ai_modifier += count_subtype(player, 1, -1) * 12;
    return mana_producer(player, card, event);
  }
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE &&
      event != EVENT_RESOLVE_ACTIVATION && event != EVENT_CLEANUP)
    return mana_producer(player, card, event);
  card_instance_t *instance = get_card_instance(player, card);
  target_definition_2022_t td;
  default_target_definition_2022(player, card, &td, 2);
  td.allowed_controller = td.preferred_controller = player;
  if (player == 1) {
    td.required_state = 0x4000;
    if (event == EVENT_CAN_ACTIVATE) {
      if (generic_activated_ability_2022(1, card, event, 0x800005, 2, 0, 0, 0, 0, 0, 0, &td, "TARGET_CREATURE")) return 99;
      if (generic_activated_ability_2022(player, card, event, 9, 5, 0, 0, 0, 0, 0, 0, NULL, NULL)) return 1;
      return mana_producer(player, card, event);
    }
  } else if (event == EVENT_CAN_ACTIVATE) {
    if (generic_activated_ability_2022(player, card, event, 5, 3, 0, 0, 0, 0, 0, 0, &td, "SELECT_A_CREATURE_YOU_CONTROL")) return 1;
    if (exiledby_count(player, card, 2)) return 1;
    if (generic_activated_ability_2022(player, card, event, 9, 5, 0, 0, 0, 0, 0, 0, NULL, NULL)) return 1;
    return mana_producer(player, card, event);
  }
  if (event == EVENT_RESOLVE_ACTIVATION) {
    if (instance->info_slot == 2) {
      if (valid_target_2022(&td)) {
        int target_card = instance->targets[0].card;
        int target_player = instance->targets[0].player;
        if (player == 1) {
          regenerate_target(target_player, target_card);
          target_card = instance->targets[0].card;
          target_player = instance->targets[0].player;
        }
        if (!is_token(target_player, target_card)) {
          exile_card_and_remember_it_on_exiledby(instance->parent_controller, instance->parent_card,
                                                instance->targets[0].player, instance->targets[0].card);
          card_instance_t *parent = get_card_instance(instance->parent_controller, instance->parent_card);
          int previous = parent->targets[1].player;
          int count = previous == -1 ? 1 : previous + 1;
          get_card_instance(instance->parent_controller, instance->parent_card)->targets[1].player = count;
        } else kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
      }
    }
    if (instance->info_slot == 3) {
      int leg = 0, idx = 0;
      int *location;
      while ((location = exiledby_find_any(player - 2, instance->targets[1].card, &leg, &idx)) != NULL) {
        uint32_t packed = (uint32_t)*location;
        *location = -1;
        int owner = packed >> 31;
        int iid = packed & 0x7fffffff;
        if (check_rfg(owner, (uint16_t)cards_data[iid].id)) {
          int added = add_card_to_hand(player, iid);
          remove_card_from_rfg(owner, (uint16_t)cards_data[iid].id);
          put_into_play(player, added);
        }
      }
    }
    return 0;
  }
  if (event != EVENT_ACTIVATE) return 0;
  int choice = 1;
  if (!paying_mana()) {
    int can_exile = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 4, 3, 0, 0, 0, 0, 0, 0, &td, "TARGET_CREATURE");
    if (can_exile) choice = 6;
    else if (generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 8, 5, 0, 0, 0, 0, 0, 0, NULL, NULL)) choice = 6;
  }
  instance->info_slot = choice;
  int can_mana = ((instance->state >> 4) ^ 1) & 1;
  int can_exile = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 5, 2, 0, 0, 0, 0, 0, 0, &td, "SELECT_A_CREATURE_YOU_CONTROL");
  int can_return = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 9, 4, 0, 0, 0, 0, 0, 0, NULL, NULL);
  int can_show = player == 0 && exiledby_count(0, card, 2) != 0;
  if (choice == 6) {
    if (ai_is_speculating == 1 || (player == 1 && (trace_mode & 2) == 0)) { spell_fizzled = 1; return 0; }
    int previous = instance->targets[1].player;
    int return_weight = previous < 0 ? 0 : previous * 5;
    choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014",
        "Get mana", can_mana, 10, "Exile a creature", can_exile, 10,
        "Free buried creatures", can_return, return_weight, "Show buried creatures", can_show, 1, 0, 0, 0, 0);
    if (choice == 6) { spell_fizzled = 1; return 0; }
  }
  if (choice == 1) return mana_producer(player, card, EVENT_ACTIVATE);
  if (choice == 2) {
    add_state(player, card, 0x10);
    if (charge_mana_for_activated_ability_2022(player, card, 2, 0, 0, 0, 0, 0)) pick_target_2022(&td, "TARGET_CREATURE");
    tapped_for_mana_color = -1;
    if (spell_fizzled == 1) remove_state(player, card, 0x10);
  } else {
    if (choice == 3) {
      add_state(player, card, 0x10);
      if (charge_mana_for_activated_ability_2022(player, card, 4, 0, 0, 0, 0, 0)) {
        instance->targets[1].card = exiledby_detach(player, card);
        kill_card(player, card, KILL_SACRIFICE);
      }
      tapped_for_mana_color = -1;
      if (spell_fizzled != 1) { instance->info_slot = choice; return 0; }
    }
    if (spell_fizzled == 1) remove_state(player, card, 0x10);
    if (choice == 4) {
      exiledby_choose(player, card, 0x42e9, EXBY_CHOOSE, 0, "Creature", 0);
      spell_fizzled = 1;
      return 0;
    }
  }
  if (spell_fizzled != 1) instance->info_slot = choice;
  return 0;
}
''',
'228e160':r'''
int card_izzet_chemister(int player, int card, event_t event)
{
  haste(player, card, event);
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE &&
      event != EVENT_RESOLVE_ACTIVATION && event != EVENT_CLEANUP) return 0;
  char buffer[50];
  scnprintf(buffer, 50, "Select a card to exile");
  test_definition_2022_t test;
  new_default_test_definition_2022(&test, 0x38, buffer);
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE) {
    if (generic_activated_ability_2022(player, card, event, 9, 1, 0, 0, 0, 1, 0, 0, NULL, NULL) &&
        exiledby_count(player, card, player) > 0 && exiledby_choose(player, card, 0x4398, EXBY_CAN_CAST, 1, "", 1))
      return exiledby_choose(player, card, 0x4398, EXBY_CAN_CAST, 1, "", 1);
    if (generic_activated_ability_with_target_in_grave_2022(player, card, event, GAA_UNTAPPED,
          0, 0, 0, 0, 1, 0, 0, NULL, NULL, TYPE_NONE, &test, 8)) return 1;
    return player == 0 && exiledby_count(0, card, 0) > 0;
  }
  if (event == EVENT_ACTIVATE) {
    int have_exile = exiledby_count(player, card, player) > 0;
    int abilities[5];
    abilities[0] = generic_activated_ability_with_target_in_grave_2022(player, card, EVENT_CAN_ACTIVATE,
        GAA_UNTAPPED, 0, 0, 0, 0, 1, 0, 0, NULL, NULL, TYPE_NONE, &test, 8);
    abilities[1] = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 9, 1, 0, 0, 0, 1, 0, 0, NULL, NULL);
    abilities[2] = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 9, 1, 0, 0, 0, 1, 0, 0, NULL, NULL);
    abilities[3] = player == 0 && have_exile;
    abilities[4] = 0;
    int weight = exiledby_count(player, card, player) * 2;
    int can_cast = player == 1 ? abilities[1] != 0 && have_exile : abilities[2];
    int choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002",
        "Exile from graveyard", abilities[0], 6, "Cast spells from exile", can_cast, weight,
        "Show exiled cards", abilities[3], 0, 0, 0, 0, 0);
    if (choice == 0) { spell_fizzled = 1; return 0; }
    if (choice == 3) {
      exiledby_choose(player, card, 0x4398, EXBY_CHOOSE, 0, "Card", 1);
      spell_fizzled = 1;
      return 0;
    }
    if (choice == 1) {
      instance->number_of_targets = 0;
      generic_activated_ability_with_target_in_grave_2022(player, card, EVENT_ACTIVATE,
          GAA_UNTAPPED, 0, 0, 0, 0, 1, 0, 0, NULL, NULL, TYPE_NONE, &test, 8);
      return 0;
    }
    if (choice != 2) return 0;
    instance->number_of_targets = 0;
    add_state(player, card, 0x10);
    if (charge_mana_for_activated_ability_2022(player, card, 1, 0, 0, 0, 1, 0)) {
      instance->targets[1].player = exiledby_count(player, card, player);
      instance->targets[1].card = exiledby_detach(player, card);
      kill_card(player, card, KILL_SACRIFICE);
    }
    if (spell_fizzled == 1) remove_state(player, card, 0x10);
    tapped_for_mana_color = -1;
    return 0;
  }
  if (event == EVENT_RESOLVE_ACTIVATION) {
    if (instance->info_slot == 1) {
      int selected = validate_target_from_grave_source(player, card, player, 0);
      if (selected >= 0) {
        int leg = 0, idx = 0;
        exiledby_remember(player, instance->parent_card, player, get_grave(player)[selected], &leg, &idx);
        rfg_card_from_grave(player, selected);
      }
    }
    if (instance->info_slot == 2) {
      int leg = 0, idx = 0;
      int amount = instance->targets[1].player;
      if (amount > 0) {
        int ic[amount];
        int count = 0;
        int *location;
        while ((location = exiledby_find_any(player - 2, instance->targets[1].card, &leg, &idx)) != NULL) {
          uint32_t packed = (uint32_t)*location;
          *location = -1;
          int iid = packed & 0x7fffffff;
          if (check_rfg(packed >> 31, (uint16_t)cards_data[iid].id)) ic[count++] = iid;
        }
        test_definition_2022_t this_test;
        while (count > 0) {
          new_default_test_definition_2022(&this_test, 0xff, "Select a card to play for free");
          int selected = select_card_from_zone_2022(player, player, ic, count, 0, 1, -1, &test);
          if (selected == -1) break;
          unsigned csvid = (uint16_t)cards_data[ic[selected]].id;
          for (int k = selected; k < count; ++k) ic[k] = ic[k + 1];
          --count;
          play_card_in_exile_for_free(player, 1 - player, csvid);
        }
        exiledby_destroy_detached(player, instance->targets[1].card);
      }
    }
  }
  return 0;
}
''',
'25abb10':r'''
int card_livio_oathsworn_sentinel(int player, int card, event_t event)
{
  enum { COUNTER_AEGIS = 173 };
  check_legend_rule(player, card, event);
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE &&
      event != EVENT_RESOLVE_ACTIVATION && event != EVENT_CLEANUP) return 0;
  target_definition_2022_t td;
  default_target_definition_2022(player, card, &td, 2);
  td.preferred_controller = player;
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE) {
    if (generic_activated_ability_2022(player, card, event, 0x40004, 1, 0, 0, 0, 0, 1, 0, &td, NULL)) return 1;
    return generic_activated_ability_2022(player, card, event, 1, 2, 0, 0, 0, 0, 1, 0, NULL, NULL);
  }
  if (event == EVENT_ACTIVATE) {
    instance->number_of_targets = 0;
    int abils[2];
    abils[0] = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 0x40004, 1, 0, 0, 0, 0, 1, 0, &td, NULL);
    abils[1] = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 1, 2, 0, 0, 0, 0, 1, 0, NULL, NULL);
    int choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\033", "\001\016",
        "Exile with Aegis counter", abils[0], 10, "Return the exiled cards", abils[1], 20, 0, 0, 0, 0);
    if (choice == 0) { spell_fizzled = 1; return 0; }
    if (choice == 1) generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 0x40004, 1, 0, 0, 0, 0, 1, 0, &td, "TARGET_CREATURE");
    else if (choice == 2) generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 1, 2, 0, 0, 0, 0, 1, 0, NULL, NULL);
    return 0;
  }
  if (event == EVENT_RESOLVE_ACTIVATION) {
    if (instance->mode == 1) {
      if (valid_target_2022(&td)) {
        card_instance_t *target = get_card_instance(instance->targets[0].player, instance->targets[0].card);
        char buffer[100];
        scnprintf(buffer, 100, " Exile %s", get_card_name_by_instance(target));
        int controller = instance->targets[0].player;
        int choice = dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION,
            "\001\002", "\001\014", "\001\003", "\001\010", controller,
            "\001\007", controller, instance->targets[0].card,
            buffer, 1, controller == player ? 20 : -20, "Decline", 1, 10, 0, 0, 0, 0);
        if (choice == 1) {
          int target_card = instance->targets[0].card;
          int target_player = instance->targets[0].player;
          int iid = get_original_internal_card_id(target_player, target_card);
          int owner = get_owner(target_player, target_card);
          int legacy;
          if (player == owner) legacy = create_legacy_effect(player, card, card_exiled_with_aegis_counter);
          else legacy = create_legacy_effect_for_opponent(player, card, card_exiled_with_aegis_counter, -1, -1);
          if (legacy != -1) {
            add_counters_from_source(player, card, owner, legacy, COUNTER_AEGIS, 1);
            card_instance_t *leg = get_card_instance(owner, legacy);
            unsigned csvid = (uint16_t)cards_data[iid].id;
            leg->targets[0].card = iid;
            leg->display_pic_csv_id = csvid;
            leg->targets[0].player = owner;
            leg->eot_toughness = 0x300;
            leg->display_pic_num = (uint16_t)get_card_image_number(csvid, target_player, target_card);
          }
          kill_card(target_player, target_card, KILL_EXILE);
        }
      }
    } else if (instance->mode == 2) {
      int marked[100][2];
      int count = 0;
      for (int phase = 0; phase < 2; ++phase) {
        int p = phase == 0 ? current_turn : 1 - current_turn;
        for (int c = active_cards_count[p] - 1; c >= 0; --c) {
          if (in_play(p, c) && is_what(p, c, 0x80) && count_counters(p, c, COUNTER_AEGIS) &&
              get_card_instance(p, c)->info_slot_fn == card_exiled_with_aegis_counter) {
            marked[count][0] = p;
            marked[count++][1] = c;
          }
        }
      }
      for (int i = count - 1; i >= 0; --i) {
        int p = marked[i][0], c = marked[i][1];
        card_instance_t *effect = get_card_instance(p, c);
        int owner = effect->targets[0].player, iid = effect->targets[0].card;
        kill_card(p, c, KILL_EXILE);
        int position = find_iid_in_rfg(owner, iid);
        if (position >= 0) {
          remove_card_from_rfg_by_position(owner, position);
          int added = add_card_to_hand(owner, iid);
          put_into_play(owner, added);
        }
      }
    }
  }
  return 0;
}
''',
'2231790':r'''
int card_dauthi_voidwalker(int player, int card, event_t event)
{
  enum { COUNTER_VOID = 170 };
  shadow(player, card, event);
  int opponent = 1 - player;
  if (if_a_card_would_be_put_into_graveyard_from_library_do_something_instead_2022(
        player, card, event, opponent, RESOLVE_TRIGGER_MANDATORY, NULL)) {
    create_dauthi_voidwalker_legacy(player, card, deck_ptr[trigger_cause_controller][trigger_cause]);
    rfg_card_in_deck(trigger_cause_controller, trigger_cause);
    replace_milled = 1;
  }
  if (if_a_card_would_be_put_into_graveyard_from_anywhere_but_library_do_something_instead_2022(
        player, card, event, opponent, RESOLVE_TRIGGER_MANDATORY, NULL)) {
    create_dauthi_voidwalker_legacy(player, card, trigger_cause);
    add_card_to_rfg(trigger_cause_controller, trigger_cause);
    play_sound_effect(WAV_DESTROY);
    replace_milled = 1;
  }
  if (event == EVENT_ACTIVATE) {
    int count = 0;
    for (int c = 0; c < active_cards_count[opponent]; ++c)
      if (in_play(opponent, c) && is_what(opponent, c, 0x80) && count_counters(opponent, c, COUNTER_VOID)) ++count;
    ai_modifier += count * 10;
  } else if (event == EVENT_RESOLVE_ACTIVATION) {
    int exiled[31][3] = {{0}};
    if (active_cards_count[opponent] > 0) {
      int best_index = -1, max_value = -99, count = 0;
      for (int c = 0; c < active_cards_count[opponent] && count < 31; ++c) {
        if (in_play(opponent, c) && is_what(opponent, c, 0x80) && count_counters(opponent, c, COUNTER_VOID)) {
          card_instance_t *leg = get_card_instance(opponent, c);
          int iid = leg->targets[0].card, owner = leg->targets[0].player;
          if (check_rfg(owner, (uint16_t)cards_data[iid].id)) {
            int found = 0;
            for (int d = 0; d <= count; ++d) if (exiled[d][1] == iid) { found = 1; break; }
            if (!found) {
              ++count;
              exiled[count][0] = owner;
              exiled[count][1] = iid;
              exiled[count][2] = c;
              int value = my_base_value_by_id((uint16_t)cards_data[iid].id);
              if (value > max_value) { max_value = value; best_index = count; }
            }
          }
        }
      }
      if (count != 0) {
        const char *labels[15];
        int weights[15];
        for (int i = 14; i >= 0; --i) labels[i] = count > i ? cards_data[exiled[i + 1][1]].name : " ";
        int choice;
        for (;;) {
          for (int i = 14; i >= 0; --i) weights[i] = my_base_value_by_id((uint16_t)cards_data[exiled[i + 1][1]].id);
          choice = @PAGE1@;
          if (choice != 16) break;
          int allow_back = ai_is_speculating != 1 && (player != 1 || ((uint32_t)trace_mode >> 1 & 1));
          const char *saved_labels[15];
          for (int i = 0; i < 15; ++i) saved_labels[i] = labels[i];
          for (int i = 14; i >= 0; --i) {
            int iid = exiled[i + 16][1];
            weights[i] = my_base_value_by_id((uint16_t)cards_data[iid].id);
            labels[i] = count > i + 15 ? cards_data[iid].name : " ";
          }
          int second = @PAGE2@;
          for (int i = 0; i < 15; ++i) labels[i] = saved_labels[i];
          if (second != 16) { choice = second + 15; break; }
        }
        create_may_play_card_from_exile_effect_2022(player, card, exiled[choice][0],
            (uint16_t)cards_data[exiled[choice][1]].id, MPCFE_UNTIL_EOT | MPCFE_FOR_FREE);
      }
    }
  }
  return generic_activated_ability_2022(player, card, event, 9, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
}
'''.replace('@PAGE1@',dauthi_dialog(0)).replace('@PAGE2@',dauthi_dialog(1))
}
