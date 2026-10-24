"""Native final missing-handler projections, including retained native quirks."""

BODIES = {
'20d20a0': r'''
int card_nicol_bolas_god_pharaoh(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION || event == EVENT_CLEANUP) {
    card_instance_t *instance = get_card_instance(player, card);
    int opponent = 1 - player;
    target_definition_2022_t td1, td3;
    default_target_definition_2022(player, card, &td1, 0);
    td1.zone = 0x1000;
    td1.allowed_controller = opponent;
    td1.preferred_controller = opponent;
    default_target_definition_2022(player, card, &td3, 0x102);
    td3.zone = 0x1200;
    td3.allowed_controller = opponent;
    td3.preferred_controller = opponent;
    if (event == EVENT_ACTIVATE) {
      instance->number_of_targets = 0;
      instance->info_slot = 0;
      int damage_available = can_target_2022(&td3);
      int steal_available = can_target_2022(&td1);
      int choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
          "Steal a spell", steal_available, 10, 2,
          "Opponents exile from hand", 1, 10, 1,
          "Deal 7 damage", damage_available, 10, -4,
          "Exile opponents stuff", 1, 100, -12, 0, 0, 0, 0);
      if (choice == 0) { spell_fizzled = 1; return 0; }
      if (choice == 1) new_pick_target_2022(&td1, "TARGET_OPPONENT", 0, 1);
      else if (choice == 3) new_pick_target_2022(&td3, "Choose an opponent, creature an opponent controls, or planeswalker an opponent controls", -1, 0x80);
    } else if (event == EVENT_RESOLVE_ACTIVATION) {
      int choice = instance->info_slot;
      if (choice == 1) {
        int t_player = instance->targets[0].player;
        int *deck = deck_ptr[t_player];
        int removed[count_deck(t_player)];
        int z = 0;
        while (*deck != -1) {
          removed[z] = *deck;
          ++z;
          rfg_top_card_of_deck(t_player);
          if ((cards_data[removed[z - 1]].type & 1) == 0) {
            show_deck_2022(current_turn, removed, z, "These cards were revealed", 0, str_Done);
            show_deck_2022(current_turn ^ 1, removed, z, "These cards were revealed", 0, str_Done);
            create_may_play_card_from_exile_effect_2022(player, card, t_player, (uint16_t)cards_data[removed[z - 1]].id, 9);
            break;
          }
        }
      } else if (choice == 2) {
        test_definition_2022_t test;
        new_default_test_definition_2022(&test, 0xff, "Select a card to exile.");
        new_global_tutor_2022(opponent, opponent, 4, 6, 1, 3, &test);
        new_global_tutor_2022(opponent, opponent, 4, 6, 1, 3, &test);
      } else if (choice == 3) damage_target0(player, card, 7);
      else if (choice == 4) {
        test_definition_2022_t test;
        default_test_definition_2022(&test, 1);
        test.type_flag = 1;
        new_manipulate_all_2022(player, card, opponent, &test, ACT_KILL_EXILE);
      }
    }
  }
  return planeswalker(player, card, event, 7);
}
''',
'21d6820': r'''
int card_tamiyo_collector_of_tales(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION || event == EVENT_CLEANUP) {
    test_definition_2022_t test;
    default_test_definition_2022(&test, 0xff);
    test.ai_selection_mode = 1;
    card_instance_t *instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE) {
      instance->number_of_targets = 0;
      instance->info_slot = 0;
      int choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
          "Reveal top 4", 1, 5, 1,
          "Return a card from your graveyard to your hand", get_grave(player)[0] != -1, 10, -3, 0, 0, 0, 0);
      if (choice == 0) { spell_fizzled = 1; return 0; }
      if (choice == 2) generic_activated_ability_with_target_in_grave_2022(player, card, EVENT_ACTIVATE, GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, 8);
    } else if (event == EVENT_RESOLVE_ACTIVATION) {
      if (instance->info_slot == 2) {
        int selected = validate_target_from_grave_source(player, card, player, 0);
        if (selected != -1) from_grave_to_hand(player, selected, 1);
      } else if (instance->info_slot == 1) {
        int on_top = deck_ptr[player][0];
        int chosen;
        if (ai_is_speculating == 1 || (player == 1 && (trace_mode & 2) == 0)) {
          chosen = get_internal_card_id_from_csv_id(0x2ee2);
          if (on_top != -1) {
            target_t counts[501];
            for (int i = 0; i != 501; ++i) { counts[i].player = -1; counts[i].card = -1; }
            int highest_count = 1;
            for (int i = 0; deck_ptr[player][i] != -1; ++i) {
              if (!is_what(-1, deck_ptr[player][i], 1)) {
                int iid = deck_ptr[player][i];
                int j = 0;
                while (counts[j].card != iid && counts[j].card != -1) ++j;
                if (counts[j].card == -1) { counts[j].card = iid; counts[j].player = 1; }
                else {
                  ++counts[j].player;
                  if (counts[j].player > highest_count) highest_count = counts[j].player;
                }
              }
            }
            for (int i = 0; i != 501; ++i)
              if (counts[i].player == highest_count) { chosen = counts[i].card; break; }
          }
        } else {
          test_definition_2022_t test2;
          new_default_test_definition_2022(&test2, 1, "");
          test2.type_flag = 1;
          int csvid = choose_a_card_test_ct("Choose a nonland card name.", &test, (color_test_t)-1, (type_t)~TYPE_LAND);
          chosen = get_internal_card_id_from_csv_id(csvid);
        }
        if (player == 1 || (trace_mode & 2) != 0) look_at_iid(player, card, chosen, "chooses:");
        int qty = count_deck(player);
        if (qty != 0) {
          int amount = qty > 4 ? 4 : qty;
          show_deck_2022(current_turn, deck_ptr[player], amount, "Cards revealed by Tamiyo, Teller of Tales", 0, str_Done);
          show_deck_2022(current_turn ^ 1, deck_ptr[player], amount, "Cards revealed by Tamiyo, Teller of Tales", 0, str_Done);
          int remaining = amount;
          if (qty > 0) {
            for (int i = 0; i < amount; ++i) {
              if (deck_ptr[player][i] == chosen) {
                add_card_to_hand(player, chosen);
                remove_card_from_deck(player, i);
                --remaining;
              }
            }
          }
          if (remaining != 0) mill(player, remaining);
        }
      }
    }
  }
  return planeswalker(player, card, event, 5);
}
''',
'21d25c0': r'''
int card_nicol_bolas_dragon_god(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  if (event == EVENT_CAN_ACTIVATE) {
    for (int p = 0; p != 2; ++p) {
      for (int c = 0; c < active_cards_count[p]; ++c) {
        if (in_play(p, c) && is_what(p, c, 0x100) && get_id(p, c) != 0x4df9) {
          int iid = get_card_instance(p, c)->internal_card_id;
          if (cards_data[iid].code_pointer(player, card, EVENT_CAN_ACTIVATE) == 99) return 99;
        }
      }
    }
    return planeswalker(player, card, EVENT_CAN_ACTIVATE, 4);
  }
  if (event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION || event == EVENT_CLEANUP) {
    target_definition_2022_t td;
    default_target_definition_2022(player, card, &td, 0x102);
    card_instance_t *instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE) {
      instance->number_of_targets = 0;
      instance->info_slot = 0;
      player_bits[0] |= 0x40000;
      player_bits[1] |= 0x40000;
      int ab1 = 0;
      for (int p = 0; p != 2; ++p) {
        for (int c = 0; c < active_cards_count[p]; ++c) {
          if (in_play(p, c) && is_planeswalker(p, c) && get_id(p, c) != 0x4df9) {
            int iid = get_card_instance(p, c)->internal_card_id;
            int act = cards_data[iid].code_pointer(player, card, EVENT_CAN_ACTIVATE);
            if (act != 0) { ab1 = act; break; }
          }
        }
      }
      player_bits[0] &= 0xfffbffff;
      player_bits[1] &= 0xfffbffff;
      int destroy_available = can_target_2022(&td);
      int choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "\001\014",
          "Use another pw's ability", ab1, 10, 0,
          "Draw and opp chooses", 1, 5, 1,
          "Destroy", destroy_available, 10, -3,
          "Opp needs a hero", 1, 20, -8, 0, 0, 0, 0);
      if (choice == 0) { spell_fizzled = 1; return 0; }
      if (choice == 1) {
        instance->number_of_targets = 0;
        uint32_t oldpb0 = player_bits[0], oldpb1 = player_bits[1];
        int activable[14] = {0};
        player_bits[0] |= 0x40000;
        player_bits[1] |= 0x40000;
        int ac = 0;
        for (int p = 0; p != 2; ++p) {
          for (int c = 0; c < active_cards_count[p]; ++c) {
            if (ac > 14) break;
            if (in_play(p, c) && is_planeswalker(p, c) && get_id(p, c) != 0x4df9) {
              int iid = get_card_instance(p, c)->internal_card_id;
              if (cards_data[iid].code_pointer(player, card, EVENT_CAN_ACTIVATE)) activable[ac++] = iid;
            }
          }
        }
        if (ac == 0) {
          spell_fizzled = 1;
          player_bits[0] = oldpb0; player_bits[1] = oldpb1;
          return 0;
        }
        player_bits[0] = oldpb0; player_bits[1] = oldpb1;
        const char *names[14];
        for (int i = 13; i >= 0; --i) names[i] = get_card_name_by_id((uint16_t)cards_data[activable[i]].id);
        int choice2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "\001\004", "\001\026",
            names[0], activable[0], 1, names[1], activable[1], 1,
            names[2], activable[2], 1, names[3], activable[3], 1,
            names[4], activable[4], 1, names[5], activable[5], 1,
            names[6], activable[6], 1, names[7], activable[7], 1,
            names[8], activable[8], 1, names[9], activable[9], 1,
            names[10], activable[10], 1, names[11], activable[11], 1,
            names[12], activable[12], 1, names[13], activable[13], 1, 0, 0, 0, 0);
        if (choice2 == 0) { spell_fizzled = 1; return 0; }
        uint32_t iid = activable[choice2 - 1];
        cards_data[iid].code_pointer(player, card, EVENT_ACTIVATE);
        if (spell_fizzled != 1) {
          get_card_instance(player, card)->eot_toughness = iid;
          ++instance->targets[9].card;
        }
      } else if (choice == 3) new_pick_target_2022(&td, "TARGET_CREATURE_OR_PLANESWALKER", 0, 1);
      instance->unk52 = (uint8_t)choice;
    } else if (event == EVENT_RESOLVE_ACTIVATION) {
      unsigned choice = instance->unk52;
      if (choice == 1) {
        uint32_t iid = get_card_instance(player, card)->eot_toughness;
        cards_data[iid].code_pointer(player, card, EVENT_RESOLVE_ACTIVATION);
      } else if (choice == 2) {
        int opponent = 1 - player;
        draw_a_card(player);
        unsigned nothing_available = 0;
        if (hand_count[opponent] == 0) nothing_available = can_sacrifice_2022(player, opponent, 1, TYPE_PERMANENT, SUBTYPE_INVALID) == 0;
        int sacrifice_available = can_sacrifice_2022(player, opponent, 1, TYPE_PERMANENT, SUBTYPE_INVALID);
        int choice2 = dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\014", "\001\003", "\001\010", opponent,
            "\001\026", "\001\016", "\001\004",
            "Discard a card", hand_count[opponent], 10,
            "Sac a permanent", sacrifice_available, 5,
            "Do nothing", nothing_available, 0, 0, 0, 0, 0);
        if (choice2 == 1) discard(opponent, 0, player);
        else if (choice2 == 2) impose_sacrifice_2022(player, card, opponent, 1, 0x47, 0, 0, 0, 0, 0, 0, 0, -1, 0);
      } else if (choice == 3) {
        if (valid_target_2022(&td)) kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      } else if (choice == 4) {
        int opponent = 1 - player;
        for (int c = active_cards_count[opponent] - 1; c >= 0; --c) {
          if (in_play(opponent, c) && (is_what(opponent, c, 0x100) ||
              (is_what(opponent, c, 2) && is_legendary(opponent, c)))) return planeswalker(player, card, event, 4);
        }
        lose_the_game_2022(opponent);
      }
    }
  }
  return planeswalker(player, card, event, 4);
}
''',
'21ce5c0': r'''
int card_god_eternal_kefnet(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  if (trigger_condition == 0xcf && cards_drawn_this_turn[player] == 0 && affect_me(player, card) &&
      reason_for_trigger_controller == player && trigger_cause_controller == player && suppress_draw == 0 &&
      deck_ptr[player][0] != -1 && !is_humiliated_for_ability(player, card, AL_TRIG)) {
    if (event == EVENT_TRIGGER) event_result |= (duh_mode(player) != 0) + 1;
    else if (event == EVENT_RESOLVE_TRIGGER) {
      show_deck_2022(current_turn, deck_ptr[player], 1, "God-Eternal Kefnet reveals:", 0, str_Done);
      show_deck_2022(current_turn ^ 1, deck_ptr[player], 1, "God-Eternal Kefnet reveals:", 0, str_Done);
      int *deck = deck_ptr[player];
      card_instance_t *instance = get_card_instance(player, card);
      instance->info_slot = *deck;
    }
  }
  if (trigger_condition == 0xd0 && cards_drawn_this_turn[player] == 1 &&
      (cards_data[get_card_instance(player, card)->info_slot].type & 0x38) != 0 &&
      (cards_data[get_card_instance(player, card)->info_slot].type & 2) == 0 && affect_me(player, card) &&
      reason_for_trigger_controller == player && trigger_cause_controller == player && suppress_draw == 0 &&
      !is_humiliated_for_ability(player, card, AL_TRIG)) {
    if (event == EVENT_TRIGGER) event_result |= 2;
    else if (event == EVENT_RESOLVE_TRIGGER) {
      int iid = get_card_instance(player, card)->info_slot;
      uint32_t id = (uint16_t)cards_data[iid].id;
      int available;
      if (id == 0x25ad) available = can_cast_reaper_king(player, -1, EVENT_RESOLVE_TRIGGER);
      else {
        card_ptr_t *cp = cards_ptr[id];
        uint32_t original_cless = cp->req_colorless;
        if (original_cless == 0x28) original_cless = 0;
        int cless = get_updated_casting_cost(player, -1, iid, EVENT_RESOLVE_TRIGGER, original_cless) - 2;
        if (cless < 0) cless = 0;
        if (id == 0x2ded) {
          cless -= count_graveyard_by_type(player, 2);
          if (cless < 0) cless = 0;
        }
        uint32_t artifact = cards_data[iid].type & 0x40;
        if (is_phyrexian(id)) available = has_phyrexian_mana_2022(player, cless, cp->req_black, cp->req_blue, cp->req_green, cp->req_red, cp->req_white, artifact);
        else if (is_normal_hybrid(cp)) {
          int c1 = get_colors_for_hybrid(-1, iid, 0, 0);
          int c2 = get_colors_for_hybrid(-1, iid, c1, 0);
          int number = get_number_of_hybrid_mana(-1, iid, 0, 0);
          available = has_mana_hybrid(player, number, c1, c2, cless);
        } else if (is_monocolor_hybrid(cp)) {
          int c1 = get_color_for_monocolored_hybrid(-1, iid);
          int number = get_number_of_monocolored_hybrid_mana(-1, iid, 0);
          available = has_mana_for_monocolor_hybrid(player, number, c1, 0);
        } else if (artifact) available = has_mana_multi_a(player, cless, cp->req_black, cp->req_blue, cp->req_green, cp->req_red, cp->req_white);
        else available = has_mana_multi(player, cless, cp->req_black, cp->req_blue, cp->req_green, cp->req_red, cp->req_white);
      }
      if (available && dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\002", "\001\014", "\001\021", iid, "Copy top card", 1, 10, 0, 0, 0, 0) == 1) {
        if (charge_mana_from_id_minus_two(player, -1, EVENT_RESOLVE_TRIGGER, (uint16_t)cards_data[iid].id)) copy_spell_2022(player, (uint16_t)cards_data[iid].id);
      }
      get_card_instance(player, card)->info_slot = 0;
    } else if (event == EVENT_CLEANUP) get_card_instance(player, card)->info_slot = 0;
  } else if (event == EVENT_CLEANUP) get_card_instance(player, card)->info_slot = 0;
  return war_god_eternal(player, card, event);
}
''',
'22cb2d0': r'''
int card_aryel_knight_of_windgrace(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION && event != EVENT_CLEANUP) return 0;
  target_definition_2022_t td1, td;
  default_target_definition_2022(player, card, &td1, 2);
  td1.allowed_controller = player; td1.preferred_controller = player;
  td1.special = 0x4000; td1.illegal_abilities = 0; td1.illegal_state = 1; td1.allow_cancel = 3;
  default_target_definition_2022(player, card, &td, 2);
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE) {
    td.power_requirement = target_available_2022(player, card, &td1) | 0x2000;
    if (generic_activated_ability_2022(player, card, event, 1, 2, 0, 0, 0, 0, 1, 0, NULL, NULL)) return 1;
    return generic_activated_ability_2022(player, card, event, 5, 0, 1, 0, 0, 0, 0, 0, &td, NULL) != 0;
  }
  if (event == EVENT_ACTIVATE) {
    td.power_requirement = target_available_2022(player, card, &td1) | 0x2000;
    instance->info_slot = 0;
    int ab1 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 1, 2, 0, 0, 0, 0, 1, 0, NULL, NULL);
    int ab2 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 5, 0, 1, 0, 0, 0, 0, 0, &td, NULL);
    int choice = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "Knight token", ab1, 5, "Destroy a creature", ab2, 10, 0, 0, 0, 0);
    if (choice == 0) { spell_fizzled = 1; return 0; }
    if (choice == 1) return generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 1, 2, 0, 0, 0, 0, 1, 0, NULL, NULL);
    if (choice != 2) return 0;
    if (!charge_mana_for_activated_ability_2022(player, card, 0, 1, 0, 0, 0, 0)) {
      remove_state(player, card, 0x10); spell_fizzled = 1; return 0;
    }
    add_state(player, card, 0x10);
    tapped_for_mana_color = -1;
    int to_tap[target_available_2022(player, card, &td1)];
    int ttc = 0;
    while (ttc < target_available_2022(player, card, &td1) && can_target_2022(&td1)) {
      instance->number_of_targets = 0;
      if (!new_pick_target_2022(&td1, "Select a Knight you control to tap.", 0, 0x80)) break;
      to_tap[ttc++] = instance->targets[0].card;
      state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
      instance->number_of_targets = 0;
    }
    for (int i = 0; i < ttc; ++i) {
      state_untargettable(player, to_tap[i], 0);
      if (spell_fizzled == 1) remove_state(player, card, 0x10);
      else tap_card(player, to_tap[i]);
    }
    instance->number_of_targets = 0;
    td.power_requirement = ttc | 0x2000;
    char buffer[75];
    scnprintf(buffer, 75, "Select target creature with power less than or equal to %d", ttc);
    new_pick_target_2022(&td, buffer, 0, 0x81);
    instance->eot_toughness = ttc;
    if (spell_fizzled != 1) return 0;
    for (int i = 0; i < ttc; ++i) remove_state(player, to_tap[i], 0x10);
    remove_state(player, card, 0x10);
    return 0;
  }
  if (event == EVENT_RESOLVE_ACTIVATION) {
    if (instance->info_slot == 1) {
      token_generation_2022_t token;
      default_token_definition_2022(player, card, 0x4958, &token);
      _Static_assert(offsetof(token_generation_2022_t, s_key_plus) == 36, "native token keyword word");
      token.s_key_plus = 8;
      generate_token_2022(&token);
    }
    if (instance->info_slot == 2) {
      td.power_requirement = instance->eot_toughness | 0x2000;
      if (validate_target_2022(player, card, &td, 0)) kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
    instance->eot_toughness = 0xffffffff;
  }
  return 0;
}
'''
}
