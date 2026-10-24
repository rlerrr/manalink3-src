"""Typed Ormos/Pelt storage and native named-card/legacy boundaries."""

BODIES={
'2374a00':r'''
int card_ormos_archive_keeper(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  if (trigger_condition == TRIGGER_REPLACE_CARD_DRAW && affect_me(player, card) &&
      reason_for_trigger_controller == player && suppress_draw == 0 &&
      !is_humiliated_for_ability(player, card, AL_STATIC) && deck_ptr[player][0] == -1) {
    if (event == EVENT_TRIGGER) { event_result |= 2; return 0; }
    if (event == EVENT_RESOLVE_TRIGGER) {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 5);
      suppress_draw = 1;
      return 0;
    }
  }
  if (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE &&
      event != EVENT_RESOLVE_ACTIVATION && event != EVENT_CLEANUP) return 0;
  int searched[2][3] = {{-1,-1,-1},{-1,-1,-1}};
  if (event == EVENT_CAN_ACTIVATE) {
    if (!generic_activated_ability_2022(player, card, event, 0, 1, 0, 2, 0, 0, 0, 0, NULL, NULL)) return 0;
    int count = 0;
    for (int i = 0; i < active_cards_count[player]; ++i) {
      if (in_hand(player, i) && test_not_in_array(get_original_internal_card_id(player, i),
                                                (int32_t)(intptr_t)searched, -1, -1)) {
        ++count;
        if (count == 3) return 1;
        searched[0][count] = get_original_internal_card_id(player, i);
      }
    }
    return 0;
  }
  if (event == EVENT_ACTIVATE) {
    test_definition_2022_t test;
    new_default_test_definition_2022(&test, 0xff, "Select a card to discard.");
    test.value_for_special_selection_function = (int32_t)(intptr_t)searched;
    test.special_selection_function = test_not_in_array;
    test.zone = 0x100;
    test.id_flag = 1;
    test.id = (uint16_t)cards_data[iid_draw_a_card].id;
    unsigned capacity = (unsigned)hand_count[player];
    int hand_copy[2][capacity];
    int count = 0;
    for (int phase = 0; phase < 2; ++phase) {
      int p = phase == 0 ? current_turn : 1 - current_turn;
      for (int c = active_cards_count[p] - 1; c >= 0; --c) {
        if (player == p && in_hand(player, c)) {
          hand_copy[0][count] = get_original_internal_card_id(player, c);
          hand_copy[1][count++] = c;
        }
      }
    }
    if (count != 0) {
      int chosen = 0;
      do {
        int selected = select_card_from_zone_2022(player, player, hand_copy[0], count, 0, 5, -1, &test);
        if (selected < 0 || selected == searched[1][0] || selected == searched[1][1]) {
          if (chosen < 3) { spell_fizzled = 1; return 0; }
          break;
        }
        searched[0][chosen] = hand_copy[0][selected];
        searched[1][chosen] = hand_copy[1][selected];
        ++chosen;
        hand_copy[0][selected] = iid_draw_a_card;
      } while (chosen != 3);
      generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 0, 1, 0, 2, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled != 1) {
        discard_card(player, searched[1][0]);
        discard_card(player, searched[1][1]);
        discard_card(player, searched[1][2]);
        return 1;
      }
    }
    return 0;
  }
  if (event == EVENT_RESOLVE_ACTIVATION) draw_cards(player, 5);
  return 0;
}
''',
'261f430':r'''
int card_pelt_collector(int player, int card, event_t event)
{
  _Static_assert(offsetof(card_instance_t, targets) == 116 && sizeof(target_t) == 8, "native split power slots");
  if (trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card)) {
    test_definition_2022_t test;
    default_test_definition_2022(&test, 2);
    test.power = get_power(player, card);
    test.power_flag = 2;
    test.not_me = 1;
    etb_ability_global_2022(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB) add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  else if (event == EVENT_GRAVEYARD_FROM_PLAY) {
    if (is_this_dying(affected_card_controller, affected_card) && affected_card_controller == player &&
        affected_card != card && is_what(player, affected_card, 2)) {
      int dying_power = get_power(affected_card_controller, affected_card);
      if (get_power(player, card) < dying_power && !check_for_death_trigger_removal(affected_card_controller, affected_card)) {
        card_instance_t *instance = get_card_instance(player, card);
        if (instance->targets[11].player < 0) instance->targets[11].player = 0;
        uint8_t *pending = (uint8_t *)&instance->targets[11].player;
        *pending = (uint8_t)(*pending + 1);
        for (int pos = 0; pos != 21; ++pos) {
          int32_t *slot = pos < 10 ? &instance->targets[pos].player : &instance->targets[pos - 10].card;
          if (*slot == -1) { *slot = get_power(affected_card_controller, affected_card); break; }
        }
      }
    }
  }
  int resolved = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (!resolved && (event == EVENT_ETB || event == EVENT_GRAVEYARD_FROM_PLAY)) return 0;
  if (resolved) {
    card_instance_t *instance = get_card_instance(player, card);
    unsigned amount = (uint32_t)instance->targets[11].card & 0xff;
    int array[amount];
    for (unsigned pos = 0; pos < amount; ++pos)
      array[pos] = pos < 10 ? instance->targets[pos].player : instance->targets[pos - 10].card;
    for (unsigned i = 1; i < amount; ++i) {
      for (unsigned j = i; j < amount; ++j) {
        if (array[j] < array[i - 1]) { int temp = array[i - 1]; array[i - 1] = array[j]; array[j] = temp; }
      }
    }
    for (unsigned i = 0; i < amount; ++i)
      if (array[i] > get_power(player, card)) add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    instance->targets[11].card = 0;
    for (int i = 0; i != 9; ++i) { instance->targets[i].player = -1; instance->targets[i].card = -1; }
  }
  if (event == EVENT_ABILITIES && !is_humiliated_for_ability(player, card, AL_STATIC) &&
      count_counters(player, card, COUNTER_P1_P1) > 2) event_result |= 0x80;
  return 0;
}
''',
'24bf650':r'''
int card_blaze_of_glory(int player, int card, event_t event)
{
  if (event == EVENT_CAN_CAST) { if ((int32_t)current_phase > 0x16) return 0; }
  else if (event == EVENT_CAST_SPELL) { if (!affect_me(player, card)) return 0; }
  else if (event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET && event != EVENT_CHANGE_TARGET) return 0;
  target_definition_2022_t td;
  default_target_definition_2022(player, card, &td, 2);
  td.allowed_controller = 1 - current_turn;
  if (event == EVENT_RESOLVE_SPELL) {
    if (valid_target_2022(&td)) {
      card_instance_t *instance = get_card_instance(player, card);
      create_legacy_effect_exe(player, card, EXE_DWORD(0x728368), instance->targets[0].player, instance->targets[0].card);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell_2022(player, card, event, 1, &td, "BLAZE_OF_GLORY", 1, NULL);
}
''',
'22dbd60':r'''
int card_dispossess(int player, int card, event_t event)
{
  if (event == EVENT_CAST_SPELL) { if (!affect_me(player, card)) return 0; }
  else if (event != EVENT_CAN_CAST && event != EVENT_RESOLVE_SPELL &&
           event != EVENT_CAN_CHANGE_TARGET && event != EVENT_CHANGE_TARGET) return 0;
  int opponent = 1 - player;
  target_definition_2022_t td;
  default_target_definition_2022(player, card, &td, 0);
  td.zone = 0x1000;
  td.allowed_controller = opponent;
  if (event == EVENT_RESOLVE_SPELL) {
    if (valid_target_2022(&td)) {
      uint32_t csvid;
      if (ai_is_speculating == 1 || (player == 1 && (trace_mode & 2) == 0)) {
        int iid = -1;
        for (int i = 0; i < 500 && deck_ptr[opponent][i] != -1; ++i)
          if (is_what(-1, deck_ptr[opponent][i], 0x40)) { iid = deck_ptr[opponent][i]; break; }
        if (iid == -1) {
          const int *grave = get_grave(opponent);
          for (int i = 0; i < 500 && grave[i] != -1; ++i)
            if (is_what(-1, grave[i], 0x40)) { iid = grave[i]; break; }
        }
        if (player == 1) ai_modifier += iid == -1 ? -100 : 100;
        csvid = (uint16_t)cards_data[iid].id;
        const char *name = get_card_name_by_id(csvid);
        dialog_impl(player, card, EVENT_ACTIVATE, "\001\014", "\001\003", "\001\021", iid, name, 1, 1, 0, 0, 0, 0);
      } else {
        csvid = choose_a_card_test_ct("Choose an artifact card name.", NULL, (color_test_t)-1, TYPE_ARTIFACT);
      }
      lobotomy_effect_2022(player, opponent, csvid, -1, 0);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell_2022(player, card, event, 0x40, &td, "TARGET_OPPONENT", 1, NULL);
}
''',
'25c8380':r'''
int card_hot_fix(int player, int card, event_t event)
{
  if (event != EVENT_RESOLVE_SPELL) return basic_spell(player, card, event);
  int32_t started;
  uint32_t needs_shuffle;
  if (ai_is_speculating == 1) { started = 0; goto rearrange; }
  if (player == 1 && (trace_mode & 2) == 0) {
    rearrange_top_x(1, 1, 500);
    started = 0;
    if (ai_is_speculating != 1) goto check_ai_clock;
    if ((trace_mode & 2) != 0) goto receive_result;
    goto finish;
  }
  started = time_in_ms();
rearrange:
  rearrange_top_x(player, player, 500);
  if (ai_is_speculating == 1) needs_shuffle = started < -10000;
  else {
check_ai_clock:
    if (player == 1 && (trace_mode & 2) == 0) needs_shuffle = started < -10000;
    else {
      int32_t ended = time_in_ms();
      needs_shuffle = (int32_t)((uint32_t)ended - (uint32_t)started) > 10000;
    }
  }
  if ((trace_mode & 2) != 0) {
    if (player == 1) goto receive_result;
    EXE_DWORD(0x78fee8) = needs_shuffle;
    EXE_BYTE(0x78fee4) = 28;
    EXE_FN(int, 0x4a6bc0, int, int)(0, 28);
  }
check_shuffle:
  if (needs_shuffle != 0) shuffle(player);
finish:
  kill_card(player, card, KILL_DESTROY);
  return basic_spell(player, card, event);
receive_result:
  EXE_FN(int, 0x4a70c0, int, int)(1, 28);
  needs_shuffle = EXE_DWORD(0x78fee8);
  goto check_shuffle;
}
''',
'2304e30':r'''
int card_mise(int player, int card, event_t event)
{
  if (event == EVENT_RESOLVE_SPELL) {
    int top = deck_ptr[player][0];
    uint32_t csvid;
    if (ai_is_speculating == 1 || (player == 1 && (trace_mode & 2) == 0)) {
      csvid = 0x2ee2;
      if (top != -1 && !is_what(-1, top, 1)) csvid = (uint16_t)cards_data[top].id;
    } else {
      test_definition_2022_t test;
      new_default_test_definition_2022(&test, 1, "");
      test.type_flag = 1;
      csvid = choose_a_card_test_ct("Choose a nonland card name.", &test, (color_test_t)-1, (type_t)~TYPE_LAND);
    }
    if (player == 1 || (trace_mode & 2) != 0)
      look_at_iid(player, card, get_internal_card_id_from_csv_id(csvid), "chooses:");
    if (top != -1) {
      reveal_card_iid(player, card, top);
      if ((uint16_t)cards_data[top].id == csvid) draw_cards(player, 3);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return basic_spell(player, card, event);
}
'''
}
