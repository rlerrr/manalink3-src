"""Typed reconstructions of mana draining and AI phase/choice bookkeeping."""

BODIES = {
'2671530': r'''
void mana_burn_2022(void)
{

  if (!(trace_mode & 2))
  {
  restart:;
    int c;
    card_instance_t *instance;
    if (mana_pool[AI][7] && current_phase > PHASE_NORMAL_COMBAT_DAMAGE &&
        (unsigned int)(active_cards_count[AI] - 1) < 149)
      for (c = 0; c < active_cards_count[AI]; ++c)
        if ((instance = in_play(AI, c)) &&
            dispatch_event_with_attacker_to_one_card(AI, c, EVENT_CAN_WASTE_MANA, 1 - AI, -1))
        {
          if (EXE_FN(int, 0x434040, int, int, int)(AI, AI, c))
            finalize_activation_2022(AI, c);
          goto restart;
        }
  }

  int p, c;
  for (p = 0; p <= 1; ++p)
    for (c = 0; c < 7; ++c)
      mana_doesnt_drain_from_pool[p][c] &= MANADRAIN_AMT_MASK;
  dispatch_event(0, 0, EVENT_MANA_POOL_DRAINING);

  for (p = 0; p < 2; ++p)
  {
    int amt_drained = 0, amt_left = 0;
    int clr;
    if (mana_pool[p][COLOR_ANY] > 0)
    {
      for (clr = COLOR_ARTIFACT; clr >= COLOR_COLORLESS; --clr)
        if (mana_doesnt_drain_from_pool[p][clr] & MANADRAIN_DOESNT_DRAIN)
          amt_left += mana_pool[p][clr];
        else
        {
          uint8_t nondraining;
          if ((nondraining = (mana_doesnt_drain_from_pool[p][clr] & MANADRAIN_AMT_MASK)))
          {
            if (nondraining > mana_pool[p][clr])
            {
              nondraining = mana_pool[p][clr];
              mana_doesnt_drain_from_pool[p][clr] &= ~MANADRAIN_AMT_MASK;
              mana_doesnt_drain_from_pool[p][clr] |= nondraining;
            }
            mana_pool[p][clr] -= nondraining;
          }

          if (mana_doesnt_drain_from_pool[p][clr] & MANADRAIN_BECOMES_COLORLESS)
          {
            amt_left += mana_pool[p][clr];
            mana_pool[p][COLOR_COLORLESS] += mana_pool[p][clr];
          }
          else
            amt_drained += mana_pool[p][clr];

          mana_pool[p][clr] = nondraining;
          amt_left += nondraining;
        }
    }

    mana_pool[p][COLOR_ANY] = amt_left;

    int setting_mana_burn;
    if (amt_drained > 0 && (setting_mana_burn = get_setting(SETTING_MANA_BURN)) &&
        !(setting_mana_burn == 2 && IS_AI(p)))
    {
      if (ai_is_speculating != 1)
      {
        play_sound_effect(WAV_MANABURN);
        EXE_FN(void, 0x470250, int, int)(p, amt_drained);
      }
      life[p] -= amt_drained;
    }
  }
}
''',

'266bfd0': r'''
void mana_doesnt_drain_until_eot_2022(int player, color_t color, int amt)
{
  if (amt < 0)
    return;

  int nondraining = mana_doesnt_drain_from_pool[player][color] & MANADRAIN_AMT_MASK;

  nondraining += amt;
  nondraining = MIN(nondraining, MANADRAIN_AMT_MASK);
  nondraining = MIN(nondraining, mana_pool[player][color]);

  mana_doesnt_drain_from_pool[player][color] &= ~MANADRAIN_AMT_MASK;
  mana_doesnt_drain_from_pool[player][color] |= nondraining;
}
''',

'266bf60': r'''
void minimize_nondraining_mana_2022(void)
{
  int p, c, nondraining;
  for (p = 0; p <= 1; ++p)
    for (c = 0; c < 7; ++c)
      if ((nondraining = (mana_doesnt_drain_from_pool[p][c] & MANADRAIN_AMT_MASK)) &&
          mana_pool[p][c] < nondraining)
        mana_doesnt_drain_from_pool[p][c] =
            mana_pool[p][c] | (mana_doesnt_drain_from_pool[p][c] & ~MANADRAIN_AMT_MASK);
}
''',

'26b1240': r'''
int remember_ai_value_impl_2022(int value)
{
  if (ai_is_speculating == 1)
  {
    if (value >= 99)
      ++value;
    EXE_DWORD(0x7a2fe4) = value;
    EXE_FN(void, 0x498f20, void)();
  }
  else
    EXE_FN(int, 0x499050, void)();
  value = EXE_DWORD(0x7a2fe4);
  if (value >= 99)
    --value;
  return value;
}
''',

'26b2190': r'''
int remember_ai_value_2022(int player, int value)
{
  EXE_DWORD(0x7a2fe4) = 0;
  if (player == AI && !(trace_mode & 2))
    return remember_ai_value_impl_2022(value);
  return value;
}
''',

'26b21c0': r'''
int recorded_rand_2022(int player, int upperbound)
{
  int value = internal_rand(upperbound);
  EXE_DWORD(0x7a2fe4) = 0;
  if (player == AI && !(trace_mode & 2))
    return remember_ai_value_impl_2022(value);
  return value;
}
''',

'26b1280': r'''
int check_timer_for_ai_speculation_2022(void)
{
  int decision_time = get_setting(SETTING_AI_DECISION_TIME);
  if (decision_time <= 0)
  {
    decision_time = 5405;
  }
  return (100 * (get_usertime_of_current_thread_in_ms() - start_usertime_of_current_thread_in_ms)) /
         decision_time;
}
''',

'26b2240': r'''
int ai_decision_phase_2022(int player, int *phase_code_to_go_to,
                           int *becomes_second_arg_of_main_phase_and_discard_phase,
                           int *becomes_third_arg_of_main_phase)
{

  int upkeep_speculation = hack_ai_decision_phase_upkeep;
  hack_ai_decision_phase_upkeep = 0;

  int store_raw_mana_available[2][8];

  if (trace_mode & 2)
  {
    char str[100];
    sprintf(str, "%d: Entering AI Decision Phase.\n", EXE_DWORD(0x60EC40)++);
    EXE_FN(void, 0x4a7d80, const char *)(str);
  }

  *phase_code_to_go_to = -1;

  if (ai_is_speculating == 1)
  {
    int p = 1 - player;
    int col;
    for (col = 0; col <= 7; ++col)
    {
      store_raw_mana_available[p][col] = raw_mana_available[p][col];
      raw_mana_available[p][col] = landsofcolor_controlled[p][col];
    }

    dispatch_event_raw(EVENT_SHOULD_AI_PLAY);

    card_instance_t *inst;
    int c;
    for (c = 0; c < active_cards_count[AI]; ++c)
      if ((inst = get_card_instance(AI, c))->internal_card_id != -1)
        call_card_function_i(inst, AI, c, EVENT_CAN_COUNTER);

    EXE_FN(void, 0x477070, void)();
    EXE_FN(void, 0x477a90, void)();

    for (p = 0; p <= 1; ++p)
      for (col = 0; col <= 7; ++col)
        raw_mana_available[p][col] = store_raw_mana_available[p][col];

    recalculate_all_cards_in_play_2022();

    int ai_opinion = EXE_FN(int, 0x499160, int)(AI) + ai_modifier;

    if (life[AI] > 0)
      EXE_DWORD(0x60E9FC) |= 4u;

    int v16 = 0;
    if (EXE_DWORD(0x715B30) < ai_opinion && !EXE_DWORD(0x7A372C))
    {
      EXE_DWORD(0x715B30) = ai_opinion;
      EXE_FN(void, 0x4990B0, void)();
      v16 = EXE_DWORD(0x60E9FC);
    }

    if (EXE_DWORD(0x7282F4) == 999)
      EXE_DWORD(0x7282F4) = -1;

    EXE_DWORD(0x7282EC) = 0;
    *becomes_third_arg_of_main_phase = 0;
    EXE_DWORD(0x7A372C) = 0;

    if (check_timer_for_ai_speculation_2022() > EXE_DWORD(0x786DCC) / 2 &&
        (5 * (5 * opponent_skill + 5) <= EXE_DWORD(0x738B48) ||
         ((EXE_DWORD(0x60E9FC) & 4) < 1 ? 200 : 50) < check_timer_for_ai_speculation_2022()))
    {
      ai_is_speculating = 0;
      EXE_DWORD(0x7282F4) = -1;
      EXE_DWORD(0x60E9FC) = v16;
    }

    ++EXE_DWORD(0x738B48);

    if (EXE_DWORD(0x60EC3C) == 1)
    {
      if (land_can_be_played & TENTATIVE_LCBP_DURING_SECOND_MAIN_PHASE)
        current_phase = PHASE_MAIN2;
      else
        current_phase = PHASE_MAIN1;
      *becomes_second_arg_of_main_phase_and_discard_phase = 1;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 2)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *becomes_second_arg_of_main_phase_and_discard_phase = 2;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 3)
    {
      *becomes_second_arg_of_main_phase_and_discard_phase = 1;
      *phase_code_to_go_to = 7;
    }

    if (EXE_DWORD(0x60EC3C) == 4)
    {
      if (land_can_be_played & TENTATIVE_LCBP_DURING_COMBAT)
        current_phase = PHASE_MAIN2;
      else
        current_phase = PHASE_MAIN1;
      *becomes_third_arg_of_main_phase = 0;
      *becomes_second_arg_of_main_phase_and_discard_phase = 4;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 5)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *becomes_second_arg_of_main_phase_and_discard_phase = 5;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 6)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *becomes_second_arg_of_main_phase_and_discard_phase = 6;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 7)
    {
      if (land_can_be_played & TENTATIVE_LCBP_DURING_COMBAT)
        current_phase = PHASE_MAIN2;
      else
        current_phase = PHASE_MAIN1;
      *becomes_third_arg_of_main_phase = 0;
      *becomes_second_arg_of_main_phase_and_discard_phase = 7;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 8)
    {
      current_phase = PHASE_NORMAL_COMBAT_DAMAGE;
      *becomes_second_arg_of_main_phase_and_discard_phase = 8;
      *phase_code_to_go_to = 6;
    }

    if (EXE_DWORD(0x60EC3C) == 17)
      *phase_code_to_go_to = 4;
  }

  if (upkeep_speculation)
    return 0;

  if (*phase_code_to_go_to == -1)
  {
    current_phase = PHASE_CLEANUP2;

    EXE_FN(int, 0x439C90, int)(PHASE_CLEANUP2);
    mana_burn_2022();

    if (is_anyone_dead_2022())
      return 1;

    if (current_turn == EXE_DWORD(0x4EF1B0) && EXE_DWORD(0x4EF1AC) == PHASE_CLEANUP2)
    {
      EXE_DWORD(0x4EF1B0) = -1;
      EXE_DWORD(0x4EF1AC) = -1;
    }

    EXE_FN(int, 0x439A80, void)();
    play_sound_effect(WAV_ENDTURN);
  }

  return 0;
}
''',

}
