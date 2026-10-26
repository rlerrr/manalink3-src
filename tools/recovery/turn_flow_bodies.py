"""Typed reconstructions of turn flow, combat checks, display and phasing."""

BODIES = {
'26ab730': r'''
int can_attack_2022(int player, int card)
{

  card_instance_t *instance = get_card_instance(player, card);

  if (instance->internal_card_id >= 0 &&
      (!(instance->regen_status & KEYWORD_DEFENDER) ||
       (instance->token_status & STATUS_WALL_CAN_ATTACK)) &&
      (is_what(player, card, TYPE_CREATURE) || (instance->state & STATE_NONCREATURE_CAN_ATTACK)) &&
      !(instance->state & (STATE_OUBLIETTED | STATE_SUMMON_SICK | STATE_TAPPED)) &&
      !(instance->token_status & STATUS_CANT_ATTACK))
  {
    int old_cancel = cancel;
    int old_event_result = event_result;
    int old_affected_card_controller = affected_card_controller;
    int old_affected_card = affected_card;

    event_result = 0;
    affected_card_controller = player;
    affected_card = card;

    call_card_function_i(instance, player, card, EVENT_ATTACK_LEGALITY);
    int new_event_result = event_result;

    affected_card = old_affected_card;
    affected_card_controller = old_affected_card_controller;
    event_result = old_event_result;
    cancel = old_cancel;

    if (new_event_result)
      return 0;

    if (player != AI || (trace_mode & 2) ||
        !check_attack_legality_if_enchanting_affected_card(player, card))
      return ((!(player_bits[1 - player] & PB_OPPONENT_CHECKS_ATTACK_LEGALITY) &&
               !(event_flags & EA_DECLARE_ATTACK)) ||
              !dispatch_event(player, card, EVENT_ATTACK_LEGALITY));
  }

  return 0;
}
''',

'26b0680': r'''
int card_instances_should_be_displayed_identically_2022(card_instance_t *inst1,
                                                        card_instance_t *inst2)
{

  return (inst1 == inst2 ||
          !(inst1->internal_card_id != inst2->internal_card_id ||
            ((inst1->state ^ inst2->state) &
             (STATE_SUMMON_SICK | STATE_TAPPED | STATE_OWNED_BY_OPPONENT | STATE_TARGETTED |
              STATE_CANNOT_TARGET | STATE_OUBLIETTED | STATE_IS_TRIGGERING)) ||
            inst1->damage_on_card != inst2->damage_on_card || inst1->power != inst2->power ||
            inst1->toughness != inst2->toughness || inst1->mana_color != inst2->mana_color ||
            inst1->color != inst2->color || inst1->blocking != inst2->blocking ||
            inst1->info_slot != inst2->info_slot ||
            inst1->special_counters != inst2->special_counters ||
            inst1->counters2 != inst2->counters2 || inst1->counters3 != inst2->counters3 ||
            inst1->counters4 != inst2->counters4 || inst1->unknown0x11a != inst2->unknown0x11a ||
            inst1->counters != inst2->counters || inst1->counters5 != inst2->counters5 ||
            inst1->unknown0x122 != inst2->unknown0x122 ||
            ((inst1->token_status ^ inst2->token_status) & STATUS_RED_BORDER) ||
            inst1->display_pic_csv_id != inst2->display_pic_csv_id ||
            inst1->display_pic_num != inst2->display_pic_num ||
            inst1->untap_status != inst2->untap_status || inst1->kill_code != inst2->kill_code ||
            inst1->counters_m1m1 != inst2->counters_m1m1 ||
            inst1->targets[18].player != inst2->targets[18].player ||
            inst1->targets[18].card != inst2->targets[18].card));
}
''',

'26b07b0': r'''
int determine_start_duel_winner_2022(void)
{

  int lost0 = life[0] <= 0 || POISON_COUNTERS(0) >= 10;
  int lost1 = life[1] <= 0 || POISON_COUNTERS(1) >= 10;

  if (!lost0 && lost1)
    return 1;
  else if (!lost1 && lost0)
    return 0;
  else
    return -1;
}
''',

'26b0830': r'''
int is_anyone_dead_2022(void)
{
  if (life[0] <= 0 || life[1] <= 0 || POISON_COUNTERS(0) >= 10 || POISON_COUNTERS(1) >= 10)
  {
    if (ai_is_speculating != 1)
    {
      EXE_FN(void, 0x472260, void)();
      return 1;
    }
    else
    {
      if (POISON_COUNTERS(0) >= 10)
        life[0] = -99;

      if (POISON_COUNTERS(1) >= 10)
        life[1] = -99;
    }
  }
  return 0;
}
''',

'26b08d0': r'''
int untap_phase_2022(int player)
{
  enum { STATE_SUMMONSICK_NOATTACK_2022 = 0x10000 };

  memset(your_spells_cant_be_countered_by_color, 0, sizeof(your_spells_cant_be_countered_by_color));

  player_bits[0] &= ~PB_SEND_EVENT_UNTAP_CARD_TO_ALL;
  player_bits[1] &= ~PB_SEND_EVENT_UNTAP_CARD_TO_ALL;
  event_flags &= ~EF_ATTACKER_CHOOSES_BLOCKERS;
  dispatch_event(player, 0, EVENT_BEGIN_TURN);

  if (trace_mode & 2)
  {
    char buf[200];
    sprintf(buf, "%d: Entering Untap Phase.\n", ++EXE_DWORD(0x60EC40));
    EXE_FN(void, 0x4A7D80, const char *)(buf);
  }

  card_instance_t *instance;

  int shimmering_lands = 0;
  int curse_flag = 0;
  int skip = 0;
  int p, c;
  for (p = 0; p < 2; ++p)
    for (c = 0; c < active_cards_count[p]; ++c)
      if ((instance = in_play(p, c)))
      {
        if (p == player)
          instance->state &= ~STATE_SUMMONSICK_NOATTACK_2022;
        if (instance->internal_card_id == LEGACY_EFFECT_CUSTOM &&
            instance->info_slot == (int)target_player_skips_untap &&
            instance->targets[0].player == player && !skip)
        {
          kill_card(p, c, KILL_EXILE);
          skip = 1;
        }
        else if (cards_data[instance->internal_card_id].id == CARD_ID_SHIMMER)
        {
          shimmering_lands |= instance->info_slot;
        }
        else if (p == 1 - player &&
                 (cards_data[instance->internal_card_id].id == CARD_ID_TEFERIS_CURSE ||
                  cards_data[instance->internal_card_id].id == CARD_ID_CLOAK_OF_INVISIBILITY) &&
                 instance->damage_target_player == player)
          curse_flag = 1;
      }

  if (skip || check_battlefield_for_id(2, CARD_ID_STASIS) ||
      check_battlefield_for_id(2, CARD_ID_THE_EON_FOG) ||
      check_battlefield_for_id(2, CARD_ID_SANDS_OF_TIME))
    return 0;

  untap_phasing_2022(player, shimmering_lands, curse_flag);

  EXE_FN(int, 0x43A700, int)(player);

  int untapped[151] = {0};
  for (c = 0; c < active_cards_count[player]; ++c)
    if ((instance = in_play(player, c)))
    {
      if ((instance->untap_status & 3) == 3)
      {

        untapped[c] = 1;

        instance->state &= ~STATE_TAPPED;
      }

      SET_BYTE0(instance->untap_status) = 0;
    }

  count_colors_of_lands_in_play_2022();
  count_mana_2022();

  for (c = 0; c < active_cards_count[player]; ++c)
    if (untapped[c])
    {
      if (player_bits[player] & PB_SEND_EVENT_UNTAP_CARD_TO_ALL)
        dispatch_event(player, c, EVENT_UNTAP_CARD);
      else
        dispatch_event_with_attacker_to_one_card(player, c, EVENT_UNTAP_CARD, 1 - player, -1);
    }

  recalculate_all_cards_in_play_2022();
  EXE_FN(void, 0x472260, void)();

  if (!EXE_DWORD(0x7a34d0))
    EXE_FN(int, 0x439c90, int)(1);

  dispatch_event(player, 0, EVENT_END_OF_UNTAP_STEP);

  EXE_FN(void, 0x43a060, void)();

  return is_anyone_dead_2022();
}
''',

'26b0ca0': r'''
int upkeep_phase_2022(int player)
{

  if (trace_mode & 2)
  {
    char str[100];
    sprintf(str, "%d: Entering UpKeep Phase.\n", EXE_DWORD(0x60EC40)++);
    EXE_FN(void, 0x4a7d80, const char *)(str);
  }

  EXE_DWORD(0x7A2FD8) = 0;
  EXE_DWORD(0x7A34D0) = 0;
  EXE_DWORD(0x60A43C) = 0;

  if (trace_mode & 2)
  {
    if (EXE_DWORD(0x4EF1AC) == -1)
      EXE_DWORD(0x60A54C) = 0;
    else
      EXE_DWORD(0x60A54C) = ((EXE_DWORD(0x4EF1AC) != 4 || EXE_DWORD(0x4EF1B0) != player) &&
                             (EXE_DWORD(0x60A4AC) != 4 || EXE_DWORD(0x60A55C) != player));
  }
  else if (ai_is_speculating != 1)
    EXE_DWORD(0x60A54C) =
        (EXE_DWORD(0x4EF1AC) != -1 && (EXE_DWORD(0x4EF1AC) != 4 || EXE_DWORD(0x4EF1B0) != player));

  if (ai_is_speculating != 1 && !(trace_mode & 2))
  {
    EXE_FN(void, 0x472260, void)();
    EXE_FN(void, 0x435140, int, int)(17, 10);
  }
speculate_more:
  if (EXE_DWORD(0x60EC3C) == 17)
  {

    EXE_FN(void, 0x498ED0, void)();
    EXE_DWORD(0x72835C) = 0;
    ai_modifier = 0;
  }

  current_phase = PHASE_BEGIN_UPKEEP;
  phase_changed_2022(player, PHASE_BEGIN_UPKEEP);
  dispatch_trigger2(player, TRIGGER_UPKEEP, EXE_STR(0x739C20), 0, 0, 0);

  EXE_DWORD(0x60A43C) = 1;
  current_phase = PHASE_UPKEEP;
  EXE_FN(int, 0x436A20, int, int, const char *, int)(-1, current_phase, EXE_STR(0x60EEAC),
                                                     EVENT_UPKEEP_PHASE);
  EXE_DWORD(0x60A43C) = 0;

  dispatch_trigger2(player, TRIGGER_END_UPKEEP, EXE_STR(0x78FA70), 0, 0, 0);

  EXE_DWORD(0x60A54C) = 0;
  EXE_DWORD(0x620860) = 1;
  EXE_FN(void, 0x477070, void)();

  EXE_DWORD(0x620860) = 0;
  EXE_DWORD(0x7A2FD8) = 0;
  mana_burn();

  int phase_code_to_go_to, becomes_second_arg_of_main_phase_and_discard_phase,
      becomes_third_arg_of_main_phase;
  hack_ai_decision_phase_upkeep = 1;
  ai_decision_phase(player, &phase_code_to_go_to,
                    &becomes_second_arg_of_main_phase_and_discard_phase,
                    &becomes_third_arg_of_main_phase);
  if (phase_code_to_go_to == 4)
    goto speculate_more;

  return is_anyone_dead_2022();
}
''',

'26b0f60': r'''
int sub_495BE0_2022(int plr)
{
  int num_per_col[COLOR_WHITE + 1] = {0};
  int i;

  if (plr == -1)
  {
    for (i = 0; i < 500; ++i)
      if (Deck[i] != -1 && !(Deck[i] & 0x88000))
      {
        color_test_t cols = cards_data[Deck[i] & 0x7FFF].color;
        if (cols & COLOR_TEST_BLACK)
          ++num_per_col[COLOR_BLACK];
        if (cols & COLOR_TEST_WHITE)
          ++num_per_col[COLOR_WHITE];
        if (cols & COLOR_TEST_GREEN)
          ++num_per_col[COLOR_GREEN];
        if (cols & COLOR_TEST_RED)
          ++num_per_col[COLOR_RED];
        if (cols & COLOR_TEST_BLUE)
          ++num_per_col[COLOR_BLUE];
      }
  }
  else
  {
    for (i = 0; i < 200; ++i)
    {
      csvid_t csvid = initial_library[plr][i].csvid;
      int num = initial_library[plr][i].numcards;
      if (csvid != -1 && num > 0)
      {
        color_test_t cols = cards_data[get_internal_card_id_from_csv_id(csvid)].color;
        if (cols & COLOR_TEST_BLACK)
          num_per_col[COLOR_BLACK] += num;
        if (cols & COLOR_TEST_WHITE)
          num_per_col[COLOR_WHITE] += num;
        if (cols & COLOR_TEST_GREEN)
          num_per_col[COLOR_GREEN] += num;
        if (cols & COLOR_TEST_RED)
          num_per_col[COLOR_RED] += num;
        if (cols & COLOR_TEST_BLUE)
          num_per_col[COLOR_BLUE] += num;
      }
    }
  }

  int highest_idx = 0, highest_num = -1;
  for (i = COLOR_BLACK; i <= COLOR_WHITE; ++i)
    if (num_per_col[i] > highest_num)
    {
      highest_idx = i;
      highest_num = num_per_col[i];
    }

  return highest_idx;
}
''',

'2377ac0': r'''
void untap_phasing_2022(int player, int shimmering_lands, int curse_flag)
{

  int c, any_phasing = 0;
  for (c = 0; c < active_cards_count[player]; ++c)
  {
    card_instance_t *instance = get_card_instance(player, c);
    if (instance->internal_card_id >= 0)
    {
      int state = instance->state & (STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY);
      if (state == (STATE_OUBLIETTED | STATE_IN_PLAY))
      {
        phase_in_impl_2022(player, c, SF_PHASED_OUT, 1);
        any_phasing = 1;
        continue;
      }

      if (shimmering_lands && is_what(player, c, TYPE_LAND))
      {
        static int shimmer_subtypes[] = {
            SUBTYPE_FOREST,      SUBTYPE_ISLAND,
            SUBTYPE_MOUNTAIN,    SUBTYPE_PLAINS,
            SUBTYPE_SWAMP,       SUBTYPE_DESERT,
            SUBTYPE_GATE,        SUBTYPE_LAIR,
            SUBTYPE_LOCUS,       SUBTYPE_MINE,
            SUBTYPE_POWER_PLANT, SUBTYPE_TOWER,
            SUBTYPE_URZAS,       -1,
        };
        int i;
        for (i = 0; shimmer_subtypes[i] >= 0; ++i)
        {
          if ((shimmering_lands & (1 << i)) && has_subtype(player, c, shimmer_subtypes[i]))
          {
            phase_out_impl_2022(player, c, SF_PHASED_OUT, 1);
            any_phasing = 1;
            continue;
          }
        }
      }

      if (curse_flag && state == STATE_IN_PLAY && is_what(player, c, TYPE_PERMANENT))
      {
        for (int k = 0; k < active_cards_count[1 - player]; ++k)
        {
          if (in_play(1 - player, k))
          {
            card_instance_t *attachment = get_card_instance(1 - player, k);
            if (attachment->damage_target_player == player && attachment->damage_target_card == c &&
                (cards_data[attachment->internal_card_id].id == CARD_ID_TEFERIS_CURSE ||
                 cards_data[attachment->internal_card_id].id == CARD_ID_CLOAK_OF_INVISIBILITY) &&
                call_card_function(1 - player, k, EVENT_PHASING) == RESULT_PHASED_OUT)
              any_phasing = 1;
          }
        }
      }

      if (state == STATE_IN_PLAY &&
          call_card_function(player, c, EVENT_PHASING) == RESULT_PHASED_OUT)
      {
        any_phasing = 1;
      }
    }
  }

  if (any_phasing)
  {

    for (c = 0; c < active_cards_count[player]; ++c)
    {
      card_instance_t *instance = get_card_instance(player, c);
      if (instance->internal_card_id >= 0)
      {
        int state = instance->state & (STATE_OUBLIETTED | STATE_INVISIBLE | STATE_IN_PLAY);

        if (state == (STATE_OUBLIETTED | STATE_IN_PLAY))
        {
          phase_in_impl_2022(player, c, SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY, 0);
          continue;
        }

        if (state == STATE_IN_PLAY)
        {
          int flags = check_special_flags(player, c, SF_PHASED_OUT | SF_PHASED_OUT_INDIRECTLY);
          if (flags)
          {
            phase_out_impl_2022(player, c, flags, 0);
          }
        }
      }
    }
  }
}
''',

}
