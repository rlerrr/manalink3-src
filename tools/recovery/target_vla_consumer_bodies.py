"""Typed local arrays for native target display, random selection and discard frames."""
AETHER_HAND='''
    int hand_slots = hand_count[player];
    int original_hand[2][hand_slots];
    int ohc = 0;
    int discard_slots = hand_slots;
    if (active_cards_count[player] > 0) {
      for (int c = 0; c < active_cards_count[player]; ++c) {
        if (in_hand(player, c) != NULL) {
          original_hand[0][ohc] = get_original_internal_card_id(player, c);
          original_hand[1][ohc] = c;
          ++ohc;
        }
      }
      discard_slots = hand_count[player];
    }
    int to_discard[discard_slots];
    int tdc = 0;
    while (tdc < (unsigned int)pcVar4->number_of_targets) {
      int selected = select_card_from_zone(player, player, original_hand[0], ohc, 0, 3, -1, &this_test);
      if (selected == -1) {
        if (tdc < (unsigned int)pcVar4->number_of_targets) {
          spell_fizzled = 1;
          return 0;
        }
        break;
      }
      to_discard[tdc++] = original_hand[1][selected];
      /* Native compaction reads one extra entry in each row. */
      for (int k = selected; k < ohc; ++k) {
        original_hand[0][k] = original_hand[0][k + 1];
        original_hand[1][k] = original_hand[1][k + 1];
      }
      --ohc;
    }
    charge_mana(player, 0, (int)((uint32_t)pcVar4->number_of_targets - (uint32_t)pcVar4->info_slot));
    if (spell_fizzled != 1) {
      for (int i = 0; i < (unsigned int)pcVar4->number_of_targets; ++i) discard_card(player, to_discard[i]);
    }
'''

METZALI_RANDOM='''
            int capacity = target_available(player, card, &td);
            int targ_array[capacity];
            memset(targ_array, 0xff, capacity * sizeof(int));
            int count = 0;
            for (int c = 0; c < active_cards_count[current_turn]; ++c) {
              pcVar2 = in_play(current_turn, c);
              if (pcVar2 != NULL && would_validate_arbitrary_target(&td, current_turn, c)) targ_array[count++] = c;
            }
            /* Native random bound is a fresh availability count, not the filled count. */
            int selected = internal_rand(target_available(player, card, &td));
            kill_card(current_turn, targ_array[selected], KILL_DESTROY);
'''

METZALI_DIALOG='''
      int abil_damage = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1,
                         1, 0, 0, 0, 1, 0, 0, NULL, NULL);
      int abil_destroy = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1,
                          2, 0, 0, 0, 0, 1, 0, NULL, NULL);
      int priority_destroy = -100;
      if (current_turn != player) priority_destroy = target_available(player, card, &td) ? 15 : 0;
      iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, LIT_027306ab, LIT_0273093c,
                LIT_0273092f, 1, 8, LIT_0273091f, abil_damage, 10,
                LIT_02730900, abil_destroy, priority_destroy, 0, 0, 0, 0);
'''

def display_body(instance,caption):
    return f'''
      int count = (unsigned int){instance}->number_of_targets;
      int t_array[count];
      int tac = 0;
      if ({instance}->number_of_targets != 0) {{
        do {{
          int c = {instance}->targets[tac].card;
          int p = {instance}->targets[tac].player;
          ++tac;
          if (c == -1) t_array[tac - 1] = get_internal_card_id_from_csv_id(p == player ? 0x615a : 0x6161);
          else t_array[tac - 1] = get_card_instance(p, c)->internal_card_id;
        }} while (tac < (unsigned int){instance}->number_of_targets);
      }}
      return show_deck(player, t_array, tac, {caption}, 0, str_Done);
'''
