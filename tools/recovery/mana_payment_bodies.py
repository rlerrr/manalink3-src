"""Typed reconstruction of the 2022 mana payment engine and resolving frontends."""

BODIES = {
'26ae610': r'''
int charge_mana_2022(int player, color_t color, int amount)
{
  int old_affected_card_controller = affected_card_controller;
  int old_affected_card = affected_card;

  int life_to_pay_for_krrik_2022 = 0;

  if (color == COLOR_ANY)
    color = COLOR_COLORLESS;

  unknown62BCEC = 0;
  EXE_DWORD(0x4d7cb0) = 200;

  int amtspaid[COLOR_ARTIFACT + 1] = {0};
  int total_paid = 0;

  if (!(land_can_be_played & LCBP_CARD_BEING_COPIED))
  {
    int old_land_can_be_played = land_can_be_played;
    land_can_be_played |= LCBP_CHARGING_MANA;

    pay_mana_xbugrwaU[color] += amount;

    int amt_of_nonartifact_mana =
        has_mana(player, COLOR_ARTIFACT, 1) - has_mana(player, COLOR_ANY, 1);
    int autotap_matching_colors_in_pool = 1;
    int autotap_colored_in_pool_for_colorless, autotap_pool_for_x, autotap_sources_human,
        autotap_sources_ai;
    if (!ldoubleclicked && !IS_AI(player))
    {
      autotap_colored_in_pool_for_colorless = autotap_pool_for_x = autotap_sources_human = 0;
      autotap_sources_ai = 0;
    }
    else
    {

      autotap_colored_in_pool_for_colorless = autotap_pool_for_x = autotap_sources_human = 1;
      if (!IS_AI(player))
        autotap_sources_ai = 0;
      else
        autotap_sources_ai = 1;
    }

    if (player == AI && !(trace_mode & 2))
    {
      int any_x_cost = 0;
      int x = 0;

      color_t c;
      for (c = COLOR_COLORLESS; c <= COLOR_ARTIFACT; ++c)
        if (pay_mana_xbugrwaU[c] == -1)
        {
          any_x_cost = 1;
          if (ai_is_speculating == 1)
          {
            int avail = has_mana(player, c, 1);
            if (c == COLOR_COLORLESS)
            {
              color_t d;
              for (d = COLOR_COLORLESS; d <= COLOR_ARTIFACT; ++d)
                if (pay_mana_xbugrwaU[d] > 0)
                  avail -= pay_mana_xbugrwaU[d];
            }

            if (avail > 0)
              x += avail;
          }
        }

      if (any_x_cost)
      {
        if (ai_is_speculating == 1)
        {
          if (x > 1 && internal_rand(3))
            x = internal_rand(x - 1) + 1;

          if (x > max_x_value && max_x_value != -1)
            x = max_x_value;

          EXE_DWORD(0x7a2fe4) = x;
          EXE_FN(void, 0x498f20, void)();
        }
        else
        {
          EXE_FN(int, 0x499050, void)();

          if (EXE_DWORD(0x7a2fe4) == 99)
            EXE_DWORD(0x7a2fe4) = 0;

          x = EXE_DWORD(0x7a2fe4);
        }
      }

      max_x_value = x;
    }

    x_value = 0;

    if (max_x_value == 0)
    {
      color_t c;
      for (c = COLOR_COLORLESS; c <= COLOR_ARTIFACT; ++c)
        if (pay_mana_xbugrwaU[c] == -1)
          pay_mana_xbugrwaU[c] = 0;
    }

    int pay_mana_has_x = 0;
    color_t c;
    for (c = COLOR_COLORLESS; c <= COLOR_ARTIFACT; ++c)
      if (pay_mana_xbugrwaU[c] == -1)
        pay_mana_has_x = 1;

    if ((card_flags[player] & 2) != 0 && pay_mana_xbugrwaU[COLOR_BLACK] != 0 &&
        charge_mana_called_from_pay_phyrexian_mana == 0)
    {
      int can_pay = can_pay_life(player, 2);
      int max_life = how_much_life_can_i_pay(player);
      if (can_pay)
      {
        int maximum = max_life / 2;
        if (pay_mana_xbugrwaU[COLOR_BLACK] < maximum)
          maximum = pay_mana_xbugrwaU[COLOR_BLACK];
        int minimum = pay_mana_xbugrwaU[COLOR_BLACK] - has_mana(player, COLOR_BLACK, 1);
        if (minimum < 0)
          minimum = 0;
        char buffer[150];
        sprintf(buffer, " Pay 2 life for how much black mana? (max %d, min %d)", maximum, minimum);
        int chosen;
        while (1)
        {
          chosen = choose_a_number(player, buffer, maximum);
          if (chosen == -1)
          {
            spell_fizzled = 1;
            return 0;
          }

          if (chosen <= maximum)
            break;
          if (player == 1)
          {
            spell_fizzled = 1;
            ai_modifier -= 100;
            return 0;
          }
        }
        if ((max_life - 6) / 2 <= chosen && player == 1)
        {
          ai_modifier -= 999;
          spell_fizzled = 1;
          return 0;
        }
        life_to_pay_for_krrik_2022 = chosen * 2;
        pay_mana_xbugrwaU[COLOR_BLACK] -= chosen;
      }
    }

    if (autotap_matching_colors_in_pool &&
        !pay_mana_maximally_satisfied(pay_mana_xbugrwaU, x_value, max_x_value))
    {
      payment_apply_mana_in_pool_to_cost(player, amtspaid, &total_paid, amt_of_nonartifact_mana);
      payment_copy_mana_pool_to_display();
    }

    if (autotap_colored_in_pool_for_colorless &&
        !pay_mana_maximally_satisfied(pay_mana_xbugrwaU, x_value, max_x_value))
    {
      payment_apply_mana_in_pool_to_colorless_cost(player, amtspaid, &total_paid,
                                                   amt_of_nonartifact_mana, &x_value, max_x_value);
      payment_copy_mana_pool_to_display();
    }

    if (autotap_pool_for_x && pay_mana_has_x &&
        !pay_mana_maximally_satisfied(pay_mana_xbugrwaU, x_value, max_x_value))
    {
      payment_apply_mana_in_pool_to_x_cost(player, amtspaid, &total_paid, amt_of_nonartifact_mana,
                                           &x_value, max_x_value);
      payment_copy_mana_pool_to_display();
    }

    if (autotap_sources_human)
      human_autotap_for_mana(player, amtspaid, &total_paid, 1);

    if (autotap_sources_ai &&
        !pay_mana_maximally_satisfied(pay_mana_xbugrwaU, x_value, max_x_value))
    {
      try_to_pay_for_mana_by_autotapping(player, amtspaid, &total_paid,
                                         AUTOTAP_NO_CREATURES | AUTOTAP_NO_DONT_AUTO_TAP, 1);

      if (!pay_mana_maximally_satisfied(pay_mana_xbugrwaU, x_value, max_x_value))
        try_to_pay_for_mana_by_autotapping(player, amtspaid, &total_paid, AUTOTAP_NO_DONT_AUTO_TAP,
                                           1);

      if (!pay_mana_maximally_satisfied(pay_mana_xbugrwaU, x_value, max_x_value))
        try_to_pay_for_mana_by_autotapping(
            player, amtspaid, &total_paid,
            AUTOTAP_NO_CREATURES | AUTOTAP_NO_ARTIFACTS | AUTOTAP_NO_NONBASIC_LANDS, 1);

      if (!pay_mana_maximally_satisfied(pay_mana_xbugrwaU, x_value, max_x_value))
        try_to_pay_for_mana_by_autotapping(player, amtspaid, &total_paid,
                                           AUTOTAP_NO_CREATURES | AUTOTAP_NO_ARTIFACTS, 1);

      if (!pay_mana_maximally_satisfied(pay_mana_xbugrwaU, x_value, max_x_value))
        try_to_pay_for_mana_by_autotapping(player, amtspaid, &total_paid, AUTOTAP_NO_CREATURES, 1);

      if (!pay_mana_maximally_satisfied(pay_mana_xbugrwaU, x_value, max_x_value))
        try_to_pay_for_mana_by_autotapping(player, amtspaid, &total_paid, 0, 1);
    }

    if (IS_AI(player))
    {
      if (!pay_mana_maximally_satisfied(pay_mana_xbugrwaU, x_value, max_x_value) && !pay_mana_has_x)
        spell_fizzled = 1;
    }
    else
      while (!pay_mana_maximally_satisfied(pay_mana_xbugrwaU, x_value, max_x_value))
      {
        int any_nonx_mana = 0;
        for (c = COLOR_COLORLESS; c <= COLOR_ARTIFACT; ++c)
          if (pay_mana_xbugrwaU[c] > 0)
          {
            any_nonx_mana = 1;
            break;
          }

        format_manacost_into_global_allpurpose_buffer(0, pay_mana_xbugrwaU, x_value, max_x_value);
        int card = EXE_FN(int, 0x49C8D0, int, int, int, int, int, char *, int)(
            player, player, player, 0, 0, payment_global_all_purpose_buffer, any_nonx_mana ? 1 : 3);
        if (EXE_DWORD(0x60a550) != -1 || (card != -1 && card != -2))
        {
          if (EXE_DWORD(0x60a550) == -1 || card != -1)
          {
            card_instance_t *inst = get_card_instance(player, card);
            int iid = inst->internal_card_id;
            if (iid >= 0 && (cards_data[iid].extra_ability & EA_MANA_SOURCE) &&
                ((cards_data[iid].type & TYPE_INTERRUPT) ||
                 (inst->state & (STATE_IN_PLAY | STATE_OUBLIETTED)) == STATE_IN_PLAY))
            {
              int old_mana_pool[8], old_pay_mana[8], old_x_value, old_max_x_value, i;
              for (i = 0; i < 8; ++i)
                old_mana_pool[i] = mana_pool[player][i];

              int (*old_charge_mana_addr_of_pre_mana_pool)[8] = charge_mana_addr_of_pre_mana_pool;
              charge_mana_addr_of_pre_mana_pool = &old_mana_pool;
              int *old_charge_mana_addr_of_x_value = charge_mana_addr_of_x_value;
              charge_mana_addr_of_x_value = &old_x_value;
              int (*old_charge_mana_addr_of_pay_mana_xbugrw)[8] =
                  charge_mana_addr_of_pay_mana_xbugrw;
              charge_mana_addr_of_pay_mana_xbugrw = &old_pay_mana;
              int *old_charge_mana_addr_of_max_x_value = charge_mana_addr_of_max_x_value;
              charge_mana_addr_of_max_x_value = &old_max_x_value;

              old_x_value = x_value;
              x_value = 0;
              old_max_x_value = max_x_value;
              max_x_value = -1;
              for (i = 0; i < 8; ++i)
              {
                old_pay_mana[i] = pay_mana_xbugrwaU[i];
                pay_mana_xbugrwaU[i] = 0;
              }

              if ((inst->state & (STATE_OUBLIETTED | STATE_IN_PLAY)) != STATE_IN_PLAY)
              {
                put_card_on_stack2_2022(player, card);
                payment_TENTATIVE_reassess_all_cards();
              }
              else if (dispatch_event_to_single_card(player, card, EVENT_CAN_ACTIVATE, 1 - player,
                                                     -1))
              {
                if (stack_size < 32)
                  put_card_or_activation_onto_stack(player, card, EVENT_RESOLVE_ACTIVATION, player,
                                                    0);
                needed_mana_colors = payment_calculate_needed_mana_colors(old_pay_mana);
                tapped_for_mana_color = -1;
                int was_tapped = inst->state & STATE_TAPPED;
                event_activate_then_duplicate_into_stack(player, card, EVENT_ACTIVATE, 1 - player,
                                                         -1);

                needed_mana_colors = 0;

                if (spell_fizzled == 1)
                {
                  spell_fizzled = 0;
                  obliterate_top_card_of_stack();
                }
                else
                {
                  land_can_be_played &= ~LCBP_CHARGING_MANA;

                  if (!was_tapped && (inst->state & STATE_TAPPED))
                  {
                    dispatch_event(player, card, EVENT_TAP_CARD);
                    dispatch_trigger2(player, TRIGGER_TAP_CARD, EXE_STR(0x78fba8), 1, player, card);
                    play_sound_effect(WAV_TAP);
                  }
                  payment_resolve_top_card_on_stack();

                  land_can_be_played |= LCBP_CHARGING_MANA;

                  payment_TENTATIVE_reassess_all_cards();
                }
              }

              charge_mana_addr_of_pre_mana_pool = old_charge_mana_addr_of_pre_mana_pool;
              charge_mana_addr_of_x_value = old_charge_mana_addr_of_x_value;
              charge_mana_addr_of_pay_mana_xbugrw = old_charge_mana_addr_of_pay_mana_xbugrw;
              charge_mana_addr_of_max_x_value = old_charge_mana_addr_of_max_x_value;

              x_value = old_x_value;
              max_x_value = old_max_x_value;

              for (i = 0; i < 8; ++i)
                pay_mana_xbugrwaU[i] = old_pay_mana[i];

              for (i = 0; i < 8; ++i)
                mana_pool[player][i] -= old_mana_pool[i];

              amt_of_nonartifact_mana = 0;
              payment_apply_mana_in_pool_to_cost(player, amtspaid, &total_paid,
                                                 amt_of_nonartifact_mana);
              payment_apply_mana_in_pool_to_colorless_cost(
                  player, amtspaid, &total_paid, amt_of_nonartifact_mana, &x_value, max_x_value);
              payment_apply_mana_in_pool_to_x_cost(player, amtspaid, &total_paid,
                                                   amt_of_nonartifact_mana, &x_value, max_x_value);

              for (i = 0; i < 8; ++i)
                mana_pool[player][i] += old_mana_pool[i];

              payment_copy_mana_pool_to_display();
            }
          }
        }
        else if (EXE_DWORD(0x60a564) == -2)
        {
          if (payment_WILDGUESS_whose_phase_to_run_to == -1 &&
              payment_WILDGUESS_phase_to_run_to == -1)
          {
            if (card == -1)
              spell_fizzled = 1;
            break;
          }
          else if (!any_nonx_mana)
            break;
        }
        else if (EXE_DWORD(0x60a564) == -3 && EXE_DWORD(0x60a434) == player &&
                 payment_TENTATIVE_color_clicked_in_mana_summary_pane != -1)
        {
          if (mana_pool[player][payment_TENTATIVE_color_clicked_in_mana_summary_pane] > 0)
          {

            if (payment_TENTATIVE_color_clicked_in_mana_summary_pane != COLOR_ARTIFACT ||
                pay_mana_xbugrwaU[COLOR_ARTIFACT])
            {
              int col_to_apply_to = -1;
              if (pay_mana_xbugrwaU[payment_TENTATIVE_color_clicked_in_mana_summary_pane])
                col_to_apply_to = payment_TENTATIVE_color_clicked_in_mana_summary_pane;
              else if (pay_mana_xbugrwaU[COLOR_ARTIFACT])
                col_to_apply_to = COLOR_ARTIFACT;
              else if (pay_mana_xbugrwaU[COLOR_COLORLESS])
                col_to_apply_to = COLOR_COLORLESS;

              if (col_to_apply_to != -1)
              {
                payment_apply_mana_to_cost(
                    pay_mana_xbugrwaU, col_to_apply_to,
                    payment_amt_to_apply_to_cost(
                        pay_mana_xbugrwaU, col_to_apply_to, mana_pool[player],
                        payment_TENTATIVE_color_clicked_in_mana_summary_pane, ldoubleclicked,
                        max_x_value, x_value),
                    &x_value, max_x_value, player,
                    payment_TENTATIVE_color_clicked_in_mana_summary_pane, amtspaid, &total_paid);
                payment_copy_mana_pool_to_display();
                continue;
              }
            }

            {
              color_test_t colors = 0;
              int n;
              for (n = 0; n < 10 && payment_raw_mana_may_be_spent_as_color[player][n] != -1; ++n)
                if (payment_TENTATIVE_color_clicked_in_mana_summary_pane ==
                    LOWORD(payment_raw_mana_may_be_spent_as_color[player][n]))
                  colors |= 1u << (BYTE3(payment_raw_mana_may_be_spent_as_color[player][n]) & 31);

              color_test_t paymask = 0;
              for (c = COLOR_COLORLESS; c <= COLOR_ARTIFACT; ++c)
                if (pay_mana_xbugrwaU[c])
                  paymask |= 1 << c;
              colors &= paymask;

              if (colors)
              {
                int num_colors = num_bits_set(colors);

                color_t col_to_apply_to;
                if (num_colors == 1)
                  col_to_apply_to = payment_single_color_test_bit_to_color_t(colors);
                else
                {
                  load_text_2022(0, "PROMPT_GRABMANA_DUALUSE");
                  col_to_apply_to = choose_a_color_exe(
                      player, text_lines[0], 1,
                      payment_TENTATIVE_color_clicked_in_mana_summary_pane, colors);
                }

                payment_apply_mana_to_cost(
                    pay_mana_xbugrwaU, col_to_apply_to,
                    payment_amt_to_apply_to_cost(
                        pay_mana_xbugrwaU, col_to_apply_to, mana_pool[player],
                        payment_TENTATIVE_color_clicked_in_mana_summary_pane, ldoubleclicked,
                        max_x_value, x_value),
                    &x_value, max_x_value, player,
                    payment_TENTATIVE_color_clicked_in_mana_summary_pane, amtspaid, &total_paid);
                payment_copy_mana_pool_to_display();
              }
            }
          }
        }
      }

    if (!(old_land_can_be_played & LCBP_CHARGING_MANA))
      land_can_be_played &= ~LCBP_CHARGING_MANA;
  }

  if (spell_fizzled == 1)
  {
    payment_TENTATIVE_update_mana_spent(amtspaid);
    color_t c;
    for (c = COLOR_COLORLESS; c <= COLOR_ARTIFACT; ++c)
    {
      produce_mana(player, c, amtspaid[c]);
      amtspaid[c] = 0;
    }
    total_paid = 0;
    x_value = 0;
  }

  int c;
  for (c = 0; c < 7; ++c)
  {
    mana_paid[c] = amtspaid[c];
    pay_mana_xbugrwaU[c] = 0;
  }
  pay_mana_xbugrwaU[7] = 0;

  if ((card_flags[player] & 2) != 0)
    lose_life_2022(player, life_to_pay_for_krrik_2022);

  max_x_value = -1;
  unknown62BCEC = -1;

  affected_card_controller = old_affected_card_controller;
  affected_card = old_affected_card;

  return total_paid;
}
''',
}

BODIES.update({
'267bc50': r'''
int charge_mana_multi_2022(int player, int colorless, int black, int blue, int green, int red, int white)
{
  pay_mana_xbugrwaU[0] = colorless;
  pay_mana_xbugrwaU[1] = black;
  pay_mana_xbugrwaU[2] = blue;
  pay_mana_xbugrwaU[3] = green;
  pay_mana_xbugrwaU[4] = red;
  pay_mana_xbugrwaU[5] = white;
  charge_mana_2022(player, COLOR_COLORLESS, 0);
  return spell_fizzled != 1;
}
''',
'267bcb0': r'''
int charge_mana_multi_a_2022(int player, int colorless, int black, int blue, int green, int red, int white, int artifact)
{
  pay_mana_xbugrwaU[0] = colorless;
  pay_mana_xbugrwaU[1] = black;
  pay_mana_xbugrwaU[2] = blue;
  pay_mana_xbugrwaU[3] = green;
  pay_mana_xbugrwaU[4] = red;
  pay_mana_xbugrwaU[5] = white;
  pay_mana_xbugrwaU[6] = artifact;
  charge_mana_2022(player, COLOR_COLORLESS, 0);
  return spell_fizzled != 1;
}
''',
'267bd20': r'''
int charge_mana_while_resolving_2022(int player, int card, event_t event, int charged_player, color_t color, int amount)
{
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    event = EVENT_RESOLVE_TRIGGER;
  put_card_or_activation_onto_stack(player, card, event, 0, 0);
  spell_fizzled = 0;
  ldoubleclicked = 0;
  charge_mana_2022(charged_player, color, amount);
  obliterate_top_card_of_stack();
  return spell_fizzled != 1;
}
''',
'267bda0': r'''
int charge_mana_while_resolving_csvid_2022(int csvid, event_t event, int charged_player, color_t color, int amount)
{
  int card_added = add_card_to_hand(charged_player, get_internal_card_id_from_csv_id(csvid));
  card_instance_t *instance = get_card_instance(charged_player, card_added);
  unsigned state = instance->state;
  --hand_count[charged_player];
  instance->state = state | STATE_INVISIBLE;
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    event = EVENT_RESOLVE_TRIGGER;
  put_card_or_activation_onto_stack(charged_player, card_added, event, 0, 0);
  spell_fizzled = 0;
  ldoubleclicked = 0;
  charge_mana_2022(charged_player, color, amount);
  obliterate_top_card_of_stack();
  obliterate_card(charged_player, card_added);
  return spell_fizzled != 1;
}
''',
'267be70': r'''
int charge_mana_multi_while_resolving_2022(int player, int card, event_t event, int charged_player, int colorless, int black, int blue, int green, int red, int white)
{
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    event = EVENT_RESOLVE_TRIGGER;
  put_card_or_activation_onto_stack(player, card, event, 0, 0);
  pay_mana_xbugrwaU[0] = colorless;
  pay_mana_xbugrwaU[1] = black;
  pay_mana_xbugrwaU[2] = blue;
  pay_mana_xbugrwaU[3] = green;
  pay_mana_xbugrwaU[4] = red;
  pay_mana_xbugrwaU[5] = white;
  spell_fizzled = 0;
  ldoubleclicked = 0;
  charge_mana_2022(charged_player, COLOR_COLORLESS, 0);
  int result = spell_fizzled != 1;
  obliterate_top_card_of_stack();
  return result;
}
''',
'267bf30': r'''
int charge_mana_multi_while_resolving_csvid_2022(int csvid, event_t event, int charged_player, int colorless, int black, int blue, int green, int red, int white)
{
  int card_added = add_card_to_hand(charged_player, get_internal_card_id_from_csv_id(csvid));
  card_instance_t *instance = get_card_instance(charged_player, card_added);
  unsigned state = instance->state;
  --hand_count[charged_player];
  instance->state = state | STATE_INVISIBLE;
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    event = EVENT_RESOLVE_TRIGGER;
  put_card_or_activation_onto_stack(charged_player, card_added, event, 0, 0);
  pay_mana_xbugrwaU[0] = colorless;
  pay_mana_xbugrwaU[1] = black;
  pay_mana_xbugrwaU[2] = blue;
  pay_mana_xbugrwaU[3] = green;
  pay_mana_xbugrwaU[4] = red;
  spell_fizzled = 0;
  ldoubleclicked = 0;
  pay_mana_xbugrwaU[5] = white;
  charge_mana_2022(charged_player, COLOR_COLORLESS, 0);
  int result = spell_fizzled != 1;
  obliterate_top_card_of_stack();
  obliterate_card(charged_player, card_added);
  return result;
}
''',
'267c030': r'''
int charge_mana_hybrid_while_resolving_2022(int player, int card, event_t event, int amt_colored, color_t first_color, color_t second_color, int amt_colorless)
{
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    event = EVENT_RESOLVE_TRIGGER;
  put_card_or_activation_onto_stack(player, card, event, 0, 0);
  spell_fizzled = 0;
  ldoubleclicked = 0;
  int result = charge_mana_hybrid(player, card, amt_colored, first_color, second_color, amt_colorless);
  obliterate_top_card_of_stack();
  return result;
}
'''
})
