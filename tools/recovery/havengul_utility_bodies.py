"""Havengul native permission/borrowed-ability state and small utility boundaries."""

BODIES = {
'210a700': r'''
int can_cast_this_havengul_2022(int iid, int unused, int player, int card)
{
  return has_mana_to_cast_iid(player, EVENT_CAN_CAST, iid);
}
''',
'210b160': r'''
int can_cast_creature_havengul_2022(int player, int card)
{
  if (generic_activated_ability_with_target_in_grave_2022(player, card, EVENT_CAN_ACTIVATE,
      GAA_NONE, 1, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_CREATURE, NULL, 0x1000)) {
    for (int p = 0; p != 2; ++p) {
      int cg = count_graveyard(p);
      const int *grave = get_grave(p);
      for (; cg >= 0; --cg) {
        if (is_what(-1, grave[cg], 2)) {
          ++havengul_lich_hack;
          int result = has_mana_to_cast_id(player, EVENT_CAN_CAST, (uint16_t)cards_data[grave[cg]].id);
          --havengul_lich_hack;
          if (result) return 1;
        }
      }
    }
  }
  return 0;
}
''',
'210b2a0': r'''
int find_attached_havengul_legacy_2022(int player, int card, int target_player, int target_card)
{
  for (int eff = 0; eff < active_cards_count[player]; ++eff) {
    card_instance_t *inst = in_play(player, eff);
    if (inst && inst->internal_card_id == LEGACY_EFFECT_CUSTOM && inst->info_slot_fn == havengul_lich_legacy &&
        inst->damage_source_player == player && inst->damage_source_card == card &&
        inst->damage_target_player == target_player && inst->damage_target_card == target_card) return eff;
  }
  return -1;
}
''',
'210b320': r'''
int spell_has_flashback_legacy_for_hl(int player, int card, event_t event)
{
  if (event == EVENT_CLEANUP || event == EVENT_STATIC_EFFECTS) {
    if (event == EVENT_STATIC_EFFECTS && get_card_instance(player, card)->targets[1].card != -1) return 0;
    kill_card(player, card, KILL_EXILE);
  }
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE && event != EVENT_RESOLVE_ACTIVATION) return 0;
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE) {
    int pos = find_in_graveyard_by_source(instance->targets[2].card, instance->targets[1].player, instance->targets[1].card);
    instance->targets[1].card = pos;
    if (pos != -1) {
      if ((instance->targets[2].player & 1) == 0) {
        if (!has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, instance->targets[0].card)) return 0;
        if (has_no_mana_cost(-1, instance->targets[0].card)) return 0;
      }
      int rval = can_legally_play_iid(player, instance->targets[0].card);
      if (rval != 0 && ((cards_data[instance->targets[0].card].type & 0x30) != 0 || can_sorcery_be_played(player, EVENT_CAN_ACTIVATE))) return rval;
    }
  } else if (event == EVENT_ACTIVATE) {
    uint32_t mode = instance->targets[2].player;
    int iid = instance->targets[0].card;
    if ((mode & 1) == 0) {
      if (!charge_mana_from_id(player, -1, EVENT_ACTIVATE, (uint16_t)cards_data[iid].id)) return 0;
      iid = instance->targets[0].card;
      mode = instance->targets[2].player;
    }
    int hlp = instance->targets[3].player, hlc = instance->targets[3].card;
    int source = instance->targets[1].player, hint = instance->targets[1].card, gyp = instance->targets[2].card;
    kill_card(player, card, KILL_EXILE);
    int pos = find_in_graveyard_by_source(gyp, source, hint);
    if ((mode & 2) == 0) play_card_in_grave_for_free_and_exile_it(player, gyp, pos);
    else {
      int played = play_card_in_grave_for_free(player, gyp, pos);
      if (player != gyp) get_card_instance(player, played)->state |= 0x1000;
      int legacy = find_attached_havengul_legacy_2022(hlp, hlc, hlp, hlc);
      card_instance_t *leg = legacy < 0 ? get_card_instance(hlp, hlc) : get_card_instance(player, legacy);
      if ((cards_data[iid].extra_ability & 0x1001) != 0) {
        for (int i = 1; i != 19; ++i) {
          if (leg->targets[i].card == -1) { leg->targets[i].card = iid; cant_be_responded_to = 1; return 0; }
          if (leg->targets[i].player == -1) { leg->targets[i].player = iid; cant_be_responded_to = 1; return 0; }
        }
      }
    }
    cant_be_responded_to = 1;
  }
  return 0;
}
''',
'210b650': r'''
int can_use_stolen_havengul_2022(int player, int card)
{
  int legacy = find_attached_havengul_legacy_2022(player, card, player, card);
  card_instance_t *leg = get_card_instance(player, legacy < 0 ? card : legacy);
  if (player != 1) {
    for (int i = 1; i != 19; ++i) {
      if (leg->targets[i].card != -1 && cards_data[leg->targets[i].card].code_pointer(player, card, EVENT_CAN_ACTIVATE)) return 1;
    }
  }
  return 0;
}
''',
'2114ea0': r'''
int card_havengul_lich_2022(int player, int card, event_t event)
{
  if (event == EVENT_STATIC_EFFECTS) {
    if (affect_me(player, card)) {
      int found = 0;
      for (int c = 0; c < active_cards_count[player]; ++c) {
        if (in_play(player, c) && is_what(player, c, 0x80)) {
          card_instance_t *inst = get_card_instance(player, c);
          if (inst->info_slot_fn == havengul_lich_legacy &&
              ((inst->damage_target_player == player && inst->damage_target_card == card) ||
               (inst->targets[0].player == player && inst->targets[0].card == card))) { found = 1; break; }
        }
      }
      if (!found) {
        int legacy = create_targetted_legacy_effect(player, card, havengul_lich_legacy, player, card);
        card_instance_t *inst = get_card_instance(player, legacy);
        inst->targets[0].player = player; inst->targets[0].card = card;
        inst->number_of_targets = 1;
        add_status(player, legacy, 0x10000);
      }
    }
    get_card_instance(player, card);
    test_definition_2022_t test;
    new_default_test_definition_2022(&test, 2, "Select target creature card.");
    return 0;
  }
  card_instance_t *instance = get_card_instance(player, card);
  test_definition_2022_t test;
  new_default_test_definition_2022(&test, 2, "Select target creature card.");
  test.ai_selection_mode = 0x101;
  if (event == EVENT_CAN_ACTIVATE) {
    if (!can_use_activated_abilities_2022(player, card)) return 0;
    if (player != 1) {
      if (generic_activated_ability_with_target_in_grave_2022(player, card, EVENT_CAN_ACTIVATE, GAA_NONE, 1, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, 0x1000)) return 1;
      return can_use_stolen_havengul_2022(player, card) != 0;
    }
    if (!has_mana_for_activated_ability_2022(1, card, 1, 0, 0, 0, 0, 0)) return 0;
    if (!graveyard_has_shroud(2)) return can_cast_creature_havengul_2022(1, card);
    return 0;
  }
  if (event == EVENT_RESOLVE_ACTIVATION) {
    uint32_t mode = instance->eot_toughness;
    if (mode == 0x42) {
      int selected = validate_target_from_grave_source(player, card, instance->targets[0].player, 1);
      if (selected >= 0) {
        int hlp = instance->parent_controller, hlc = instance->parent_card;
        int legacy = create_legacy_activate(hlp, hlc, spell_has_flashback_legacy_for_hl);
        card_instance_t *leg = get_card_instance(hlp, legacy);
        card_instance_t *parent = get_card_instance(hlp, hlc);
        int gyp = parent->targets[0].player;
        const int *grave = get_grave(gyp);
        leg->targets[0].card = grave[selected];
        leg->targets[1].card = selected;
        leg->targets[1].player = graveyard_source[gyp][selected];
        leg->targets[2].player = 2; leg->targets[2].card = gyp;
        leg->targets[3].player = hlp; leg->targets[3].card = hlc;
        create_card_name_legacy(hlp, legacy, (uint16_t)cards_data[grave[selected]].id);
      }
      mode = instance->eot_toughness;
    }
    if ((int32_t)mode < 0 && mode != 0x80000000) {
      cards_data[mode & 0x0fffffff].code_pointer(player, card, EVENT_RESOLVE_ACTIVATION);
      instance->eot_toughness = 0;
    }
    return 0;
  }
  if (event != EVENT_ACTIVATE) return 0;
  int use_grave = 0;
  if (player == 0) {
    if (generic_activated_ability_with_target_in_grave_2022(0, card, EVENT_CAN_ACTIVATE, GAA_NONE, 1, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, 0x1000)) {
      if (!can_use_stolen_havengul_2022(0, card)) use_grave = 1;
      else {
        int choice = do_dialog(0, 0, card, -1, -1, " Cast a creature from grave\n Use a stolen ability\n Cancel", 0);
        if (choice == 0) use_grave = 1;
        else if (choice != 1) { spell_fizzled = 1; return 0; }
      }
    }
  } else if (player == 1 && can_cast_creature_havengul_2022(1, card)) {
    if (!can_use_stolen_havengul_2022(1, card)) use_grave = 1;
    else {
      int choice = do_dialog(1, 1, card, -1, -1, " Cast a creature from grave\n Use a stolen ability\n Cancel", 0);
      if (choice == 0) use_grave = 1;
      else if (choice != 1) { spell_fizzled = 1; return 0; }
    }
    if (use_grave) test.special_selection_function = can_cast_this_havengul_2022;
  }
  if (use_grave) {
    generic_activated_ability_with_target_in_grave_2022(player, card, EVENT_ACTIVATE, GAA_NONE, 1, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, 0x1000);
    if (spell_fizzled != 1) instance->eot_toughness = 0x42;
    return 0;
  }
  int legacy = find_attached_havengul_legacy_2022(player, card, player, card);
  card_instance_t *leg = legacy < 0 ? instance : get_card_instance(player, legacy);
  int act_array[8];
  int aa_count = 0;
  for (int i = 1; i != 19; ++i) {
    if (leg->targets[i].card != -1 && cards_data[leg->targets[i].card].code_pointer(player, card, EVENT_CAN_ACTIVATE)) act_array[aa_count++] = leg->targets[i].card;
    if (leg->targets[i].player != -1 && cards_data[leg->targets[i].player].code_pointer(player, card, EVENT_CAN_ACTIVATE)) act_array[aa_count++] = leg->targets[i].player;
  }
  test_definition_2022_t this_test;
  new_default_test_definition_2022(&this_test, 2, "Select an ability to activate.");
  int selected = select_card_from_zone_2022(player, player, act_array, aa_count, 0, 1, -1, &this_test);
  if (selected == -1) { spell_fizzled = 1; return 0; }
  cards_data[act_array[selected]].code_pointer(player, card, EVENT_ACTIVATE);
  if (spell_fizzled != 1) instance->eot_toughness = (uint32_t)act_array[selected] | 0x80000000;
  return 0;
}
''',
'22c6540': r'''
int is_commander_in_command_zone_2022(int player)
{
  int edh_card = locate_edh_card_2022(player);
  if (edh_card != -1) {
    int32_t state = get_card_instance(player, edh_card)->targets[3].player;
    if (state >= 0) return ((state >> 8) | state) & 1;
  }
  return 0;
}
''',
'2650ca0': r'''
int show_deck_force_expansion_2022(int player, const int *deck, int num_cards, const char *prompt,
                                  int suppress_done_label, const char *done_label, int expansion)
{
  call_sub_437E20_unless_ai_is_speculating();
  hack_show_deck_force_expansion = expansion;
  int result = show_deck_exe(player, deck, num_cards, prompt, suppress_done_label, (int32_t)(intptr_t)done_label);
  hack_show_deck_force_expansion = 0;
  return result;
}
''',
'266d3f0': r'''
int mana_producer_fixed_2022(int player, int card, event_t event, int base_color)
{
  if (event == EVENT_ACTIVATE) {
    if (is_what(player, card, 1)) {
      int added = mana_colors_added[player];
      if (added != 0) return produce_mana_tapped_all_one_color_with_default_impl(player, card, added | color_to_color_test(base_color), 1, COLOR_TEST_COLORLESS, 0, 1);
      if (multi_mana_transmutation[player][0] != 0 && mana_doublers(player, TYPE_LAND) > 1) {
        color_t color = single_color_test_bit_to_color(multi_mana_transmutation[player][0]);
        produce_mana_tapped_impl(player, card, color, multi_mana_transmutation[player][1], 1, 1);
        return 0;
      }
    }
    int override = check_special_flags2_2022(player, card, 0x40);
    if (card >= 0 && override && mana_quantity_override[player][base_color] != 0) {
      produce_mana_tapped_impl(player, card, mana_color_override[player][base_color], mana_quantity_override[player][base_color], 1, 1);
      return 0;
    }
  }
  return mana_producer_fixed(player, card, event, base_color);
}
'''
}
