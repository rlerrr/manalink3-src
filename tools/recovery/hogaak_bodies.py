"""Typed native Convoke trial/payment frames; retain native edge cases and call order."""

PREDICATE='''int has_convoked_mana_for_hogaak_impl(int player, int cless, int black, int blue,
                                              int green, int red, int white)
{
  int size = active_cards_count[player];
  int tapped[size];
  int tapped_for_color[size][5];
  memset(tapped_for_color, 0, sizeof tapped_for_color);
  int attempt = 0;
  int scan_size = size;
  for (;;) {
    int clrs[6] = {cless, black, blue, green, red, white};
    int result = 0;
    int tc = 0;
    if (scan_size > 0) {
      int i = 0;
      do {
        if (in_play(player, i) && !is_tapped(player, i) && is_what(player, i, TYPE_CREATURE)) {
          unsigned int color = get_color(player, i);
          for (int k = 1; k != 6; ++k) {
            if ((color >> k & 1U) && clrs[k] > 0 && tapped_for_color[i][k - 1] != 1) {
              --clrs[k];
              tapped_for_color[i][k - 1] = 1;
              add_state(player, i, STATE_TAPPED);
              tapped[tc++] = i;
              goto after_creature;
            }
          }
          if (clrs[0] != 0) {
            --clrs[0];
            add_state(player, i, STATE_TAPPED);
            tapped[tc++] = i;
          }
        }
after_creature:
        if (clrs[0] + clrs[1] + clrs[2] + clrs[3] + clrs[4] + clrs[5] == 0) result = 1;
        ++i;
      } while (i < active_cards_count[player]);
      for (int i = 0; i < tc; ++i) remove_state(player, tapped[i], STATE_TAPPED);
      if (tc > 0 && result != 0) return 2;
    }
    ++attempt;
    if (attempt > 49 || result != 0) return result;
    scan_size = active_cards_count[player];
  }
}
'''


PAYMENT='''int charge_mana_hybrid_to_hogaak_convoke(int player, int card, int number,
                                           int first_color, int second_color, int colorless)
{
  int mana[6] = {0, 0, 0, 0, 0, 0};
  int mana2[10];
  char buffer[500];
  test_definition_t this_test;
  int pos = scnprintf(buffer, 500, " Cancel\\n");
  int options = 1;
  if (number >= 0) {
    int i = number;
    do {
      mana[first_color] = i;
      mana[second_color] = number - i;
      if (has_convoked_mana_for_hogaak_impl(player, colorless, mana[1], mana[2], mana[3],
                                            mana[4], mana[5])) {
        mana2[options] = 0;
        mana_into_string(colorless, mana[1], mana[2], mana[3], mana[4], mana[5], &this_test);
        pos += scnprintf(buffer + pos, 500 - pos, " Pay %s\\n", this_test.message);
        for (int color = 1; color != 6; ++color) {
          for (int k = 0; k < mana[color]; ++k) {
            mana2[options] |= 1U << (((color - 1) * 6 + k) & 31);
          }
        }
        ++options;
      }
      --i;
    } while (i != -1);
  }
  int choice = 1;
  if (number >= 0 && options >= 3 && player == 0) {
    choice = do_dialog(0, 0, card, -1, -1, buffer, 1);
    if (choice == 0) {
      spell_fizzled = 1;
      return 0;
    }
  }
  unsigned int encoded = mana2[choice];
  for (int i = 0; i != 6; ++i) mana[i] = 0;
  for (int color = 1; color != 6; ++color) {
    for (int k = 0; k != 6; ++k) {
      if (encoded >> ((color - 1) * 6 + k) & 1U) ++mana[color];
    }
  }
  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_CREATURE);
  td.allowed_controller = player;
  td.preferred_controller = player;
  int size = active_cards_count[player];
  td.illegal_state = 1;
  td.illegal_abilities = 0;
  td.allow_cancel = 1;
  int tapped[size];
  memset(tapped, 0xff, sizeof tapped);
  card_instance_t *instance = get_card_instance(player, card);
  if (player != 0) {
    int color_size = active_cards_count[player];
    int tapped_for_color[color_size][5];
    memset(tapped_for_color, 0, sizeof tapped_for_color);
    int attempts_left = 50;
    int scan_size = color_size;
    for (;;) {
      int clrs[6] = {colorless, mana[1], mana[2], mana[3], mana[4], mana[5]};
      int tc = 0;
      if (scan_size > 0) {
        int i = 0;
        do {
          if (in_play(player, i) && !is_tapped(player, i) && is_what(player, i, TYPE_CREATURE)) {
            unsigned int color = get_color(player, i);
            for (int k = 1; k != 6; ++k) {
              if ((color >> k & 1U) && clrs[k] > 0 && tapped_for_color[i][k - 1] != 1) {
                --clrs[k];
                tapped_for_color[i][k - 1] = 1;
                add_state(player, i, STATE_TAPPED);
                tapped[tc++] = i;
                goto after_creature;
              }
            }
            if (clrs[0] != 0) {
              add_state(player, i, STATE_TAPPED);
              tapped[tc++] = i;
              --clrs[0];
            }
          }
after_creature:
          if (clrs[0] + clrs[1] + clrs[2] + clrs[3] + clrs[4] + clrs[5] == 0) {
            if (spell_fizzled != 1) {
              for (int j = 0; j < tc; ++j) tap_card(player, tapped[j]);
              return (unsigned int)(spell_fizzled != 1);
            }
            break;
          }
          ++i;
        } while (i < active_cards_count[player]);
        for (int j = 0; j < tc; ++j) remove_state(player, tapped[j], STATE_TAPPED);
      }
      --attempts_left;
      if (attempts_left == 0) {
        spell_fizzled = 1;
        return (unsigned int)(spell_fizzled != 1);
      }
      scan_size = active_cards_count[player];
    }
  }
  int clrs[6] = {colorless, mana[1], mana[2], mana[3], mana[4], mana[5]};
  int tc = 0;
  do {
    if (!can_target(&td)) break;
    if (!select_target(0, card, &td, "Select a creature to tap for Convoke.", NULL)) {
      spell_fizzled = 1;
      goto rollback;
    }
    instance->number_of_targets = 0;
    if (will_tap_permanent_for_convoke(instance->targets[0].player, instance->targets[0].card,
                                      clrs)) {
      add_state(instance->targets[0].player, instance->targets[0].card, STATE_TAPPED);
      tapped[tc++] = instance->targets[0].card;
    }
  } while (clrs[1] + clrs[0] + clrs[2] + clrs[3] + clrs[4] + clrs[5] != 0);
  if (spell_fizzled == 1) {
rollback:
    for (int i = 0; i < tc; ++i) remove_state(0, tapped[i], STATE_TAPPED);
  } else {
    for (int i = 0; i < tc; ++i) tap_card(0, tapped[i]);
  }
  return (unsigned int)(spell_fizzled != 1);
}
'''
