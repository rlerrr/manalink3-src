"""Combat legality, blocker enforcement/selection and speculative stat setup."""

BODIES = {
'26aa220': r'''
int minimum_blockers_2022(int player, int card)
{
  if (check_for_special_ability(player, card, 0x80000)) return 3;
  return check_for_special_ability(player, card, 0x10000) != 0 ? 2 : 1;
}
''',
'26aa280': r'''
int is_legal_block_impl2(int blocking_player, int blocking_card, int blocked_player,
    int blocked_card, keyword_t abils, int landwalks)
{
  card_instance_t *blocking = get_card_instance(blocking_player, blocking_card);
  if (blocking->internal_card_id < 0) return 0;
  uint32_t state = blocking->state;
  if (!(cards_data[blocking->internal_card_id].type & TYPE_CREATURE) && !(state & 0x2000000)) return 0;
  uint32_t illegal_state = 0x800008 | ((card_flags[blocking_player] & 1) ? 0 : 0x10);
  if (state & illegal_state) return 0;
  if (state & 4) {
    if (!(get_special_abilities_by_instance_2022(blocking) & 8)) return 0;
    state = blocking->state;
  }
  if (!(state & 2)) return 0;
  if ((abils & KEYWORD_FLYING) && !(get_abilities(blocking_player, blocking_card, EVENT_ABILITIES, -1) & 0x420)) return 0;
  if ((abils & KEYWORD_PROT_COLORED) && (((uint32_t)get_color(blocking_player, blocking_card) << 10) & abils & KEYWORD_PROT_COLORED)) return 0;
  if (abils & KEYWORD_PROT_CREATURES) return 0;
  if (is_what(blocking_player, blocking_card, TYPE_ARTIFACT) && (abils & KEYWORD_PROT_ARTIFACTS)) return 0;
  if (has_protection_from_impl(blocked_player, blocked_card, blocking_player, blocking_card, IPF2022_BLOCK, abils)) return 0;
  uint32_t walks = abils & landwalks;
  if (walks & KEYWORD_BASIC_LANDWALK) {
    if ((walks & KEYWORD_SWAMPWALK) && !(player_bits[blocked_player] & 0x400)) return 0;
    if ((walks & KEYWORD_ISLANDWALK) && !(player_bits[blocked_player] & 0x800)) return 0;
    if ((walks & KEYWORD_FORESTWALK) && !(player_bits[blocked_player] & 0x1000)) return 0;
    if ((walks & KEYWORD_MOUNTAINWALK) && !(player_bits[blocked_player] & 0x2000)) return 0;
    if ((walks & KEYWORD_PLAINSWALK) && !(player_bits[blocked_player] & 0x4000)) return 0;
  }
  card_instance_t *blocked = get_card_instance(blocked_player, blocked_card);
  uint32_t blocked_sp = is_humiliated_for_ability(blocked_player, blocked_card, AL_KEY) ? 0 : blocked->ext->sp_keywords;
  uint32_t blocking_sp = is_humiliated_for_ability(blocking_player, blocking_card, AL_KEY) ? 0 : blocking->ext->sp_keywords;
  if (blocking_sp & 0x20) return 0;
  if (blocked_sp & 0x10) return 0;
  if (blocked_sp & 0x100) {
    if (!(blocking_sp & 0x8000100)) return 0;
  } else if (blocking_sp & 0x100) return 0;
  if ((blocked_sp & 0x40000000) && !(blocking_sp & 0x40000000)) return 0;
  if ((blocked_sp & 1) && !(get_color(blocking_player, blocking_card) & 2) && !is_what(blocking_player, blocking_card, TYPE_ARTIFACT)) return 0;
  if ((blocked_sp & 0x2000000) && !has_my_colors(blocking_player, blocking_card, blocked_player, blocked_card) && !is_what(blocking_player, blocking_card, TYPE_ARTIFACT)) return 0;
  if (blocked_sp & 0x20000) {
    int blocking_power = get_power(blocking_player, blocking_card);
    int blocked_power = get_power(blocked_player, blocked_card);
    if (blocked_power < blocking_power) return 0;
  }
  if (blocked_sp & 0x200000) {
    if (blocked_sp & 0x90000) return 0;
    if (dispatch_event_with_attacker(blocking_player, blocking_card, EVENT_BLOCK_LEGALITY, blocked_player, blocked_card) < 1) return -1;
    return 0;
  }
  if (dispatch_event_with_attacker(blocking_player, blocking_card, EVENT_BLOCK_LEGALITY, blocked_player, blocked_card) < 1)
    return minimum_blockers_2022(blocked_player, blocked_card);
  return 0;
}
''',
'26aa700': r'''
void enforce_minimum_blockers(void)
{
  int atk = current_turn, def = 1 - atk;
  int num_blocking[151] = {0};
  for (int c = 0; c < active_cards_count[def]; ++c) {
    card_instance_t *inst = in_play(def, c);
    if (inst && inst->blocking < 151) ++num_blocking[inst->blocking];
  }
  for (unsigned int c = 0; (int)c < active_cards_count[atk]; ++c) {
    card_instance_t *inst = in_play(atk, c);
    if (inst && (inst->state & 4) && num_blocking[c] > 0) {
      int count = num_blocking[c];
      if (count < minimum_blockers_2022(atk, c) ||
          (check_for_special_ability(atk, c, 0x200000) && count > 1)) {
        for (int b = 0; b < active_cards_count[def]; ++b) {
          inst = in_play(def, b);
          if (inst && inst->blocking == c) {
            --count;
            inst->blocking = 255;
            inst->state &= 0xffff7ff7;
            num_blocking[c] = count;
          }
        }
        if (count != 0) backtrace_and_abort("functions/engine.c:2266\nnum_blocking[c] == 0");
      }
    }
  }
}
''',
'26ab5a0': r'''
int is_legal_block_impl_2022(int blocking_player, int blocking_card, int blocked_player,
    int blocked_card, keyword_t abils, int ignored_previously_landwalks)
{
  int min_blk = is_legal_block_impl2(blocking_player, blocking_card, blocked_player, blocked_card, abils, ignored_previously_landwalks);
  if ((uint32_t)min_blk > 1) {
    if (min_blk < 2) {
      for (int c = 0; c < active_cards_count[blocking_player]; ++c) {
        if (c == blocking_card) continue;
        card_instance_t *inst = in_play(blocking_player, c);
        if (inst && inst->blocking == (uint32_t)blocked_card) return 0;
      }
      return 1;
    }
    int total_blockers = 1;
    for (int c = 0; c < active_cards_count[blocking_player]; ++c) {
      if (c == blocking_card) continue;
      card_instance_t *inst = in_play(blocking_player, c);
      if (inst && (inst->blocking == (uint32_t)blocked_card ||
          is_legal_block_impl2(blocking_player, c, blocked_player, blocked_card, abils, ignored_previously_landwalks))) {
        if (++total_blockers >= min_blk) return 1;
      }
    }
    return 0;
  }
  return min_blk;
}
''',
'26ab6b0': r'''
int is_legal_block_2022(int blocking_player, int blocking_card, int blocked_player, int blocked_card)
{
  keyword_t abils = get_abilities(blocked_player, blocked_card, EVENT_ABILITIES, -1);
  int player1_landwalks, player0_landwalks;
  EXE_FN(void, 0x499df0, int *, int *)(&player1_landwalks, &player0_landwalks);
  return is_legal_block_impl_2022(blocking_player, blocking_card, blocked_player, blocked_card,
      abils, blocking_player == 1 ? player1_landwalks : player0_landwalks);
}
''',
'26ab990': r'''
void human_assign_blockers_2022(int player)
{
  if (ai_is_speculating == 1) return;
  int def = 1 - player, who_chooses = def;
  if (event_flags & 8) {
    if (current_turn == 1) return;
    who_chooses = player;
  }
  EXE_FN(void, 0x472260, void)();
  target_definition_2022_t td_choose_blocker;
  base_target_definition_2022(def, 0, &td_choose_blocker, 0x2000002);
  td_choose_blocker.zone = 0x2200;
  td_choose_blocker.special = 0x400;
  td_choose_blocker.illegal_state = 0;
  td_choose_blocker.allow_cancel = 2;
  td_choose_blocker.who_chooses = who_chooses;
  td_choose_blocker.allowed_controller = def;
  td_choose_blocker.preferred_controller = def;
  target_definition_2022_t td_block_which_attacker;
  base_target_definition_2022(player, 0, &td_block_which_attacker, TYPE_CREATURE);
  td_block_which_attacker.allowed_controller = player;
  td_block_which_attacker.preferred_controller = player;
  td_block_which_attacker.required_state = 2;
  td_block_which_attacker.who_chooses = who_chooses;
  target_t blocker, blocked;
  for (;;) {
    int any = 0;
    for (int i = 0; i < active_cards_count[player] && !any; ++i) {
      if (!(get_card_instance(player, i)->state & 4)) continue;
      for (int j = 0; j < active_cards_count[def]; ++j) {
        card_instance_t *inst = get_card_instance(def, j);
        uint32_t mask = 10 | ((card_flags[def] & 1) ? 0 : 0x10);
        if ((inst->state & mask) == 2 && is_legal_block_2022(def, j, player, i)) {
          any = 1;
          break;
        }
      }
    }
    if (!any) break;
    load_text_2022(NULL, "PROMPT_CHOOSEBLOCKERS");
    if (!select_target_2022(def, -1000, &td_choose_blocker, text_lines[0], &blocker)) break;
    forbid_attack = 0;
    if (event_flags & 0x800000) {
      EXE_FN(void, 0x435c80, void)();
      trigger_cause_controller = blocker.player;
      trigger_cause = blocker.card;
      dispatch_trigger(def, TRIGGER_PAY_TO_BLOCK, EXE_STR(0x790248), 1);
      EXE_FN(void, 0x435cd0, void)();
      if (forbid_attack) continue;
    }
    if (select_target_2022(player, -1000, &td_block_which_attacker, text_lines[1], &blocked)) {
      if (!EXE_FN(int, 0x4350e0, int, int, int, int)(blocker.player, blocker.card, blocked.player, blocked.card)) {
        load_text_2022(NULL, "PROMPT_CHOOSEBLOCKERS");
        set_centerwindow_txt(text_lines[2]);
        EXE_STDCALL_FN(void, 0x4d5d32, int)(2000);
        set_centerwindow_txt("");
      } else {
        card_instance_t *inst = get_card_instance(blocked.player, blocked.card);
        int band = inst->blocking == 255 ? blocked.card : inst->blocking;
        inst = get_card_instance(blocker.player, blocker.card);
        inst->blocking = band;
        inst->state |= 8;
        get_card_instance(blocked.player, blocked.card)->state |= 0x4000;
        play_sound_effect(WAV_BLOCK2);
        EXE_FN(void, 0x472260, void)();
        if (event_flags & 0x100000)
          dispatch_trigger2(current_turn, TRIGGER_BLOCKER_CHOSEN, EXE_STR(0x790074), 0, blocker.player, blocker.card);
      }
    }
  }
  enforce_minimum_blockers();
}
''',
'26ac0a0': r'''
void setup_ai_combat_data(int player)
{
  typedef struct {
    int potential_power, potential_toughness;
    keyword_t abils;
    int opinion_of_game_state_without_this;
  } ai_combat_data_t;
  ai_combat_data_t (*data)[150] = (ai_combat_data_t (*)[150])0x609164;
  int old_ai_is_speculating = ai_is_speculating;
  int old_speculating_during_combat = EXE_DWORD(0x7a5378);
  ai_is_speculating = 1;
  EXE_DWORD(0x7a5378) = 1;
  EXE_DWORD(0x60a424) = 0;
  memset(data, 0, 4800);
  int final_check_abilities_cop_colors = 0;
  int p = player;
  for (int pass = 0; pass < 2; ++pass) {
    EXE_DWORD(0x60a428) = 0;
    for (int c = 0; c < active_cards_count[p]; ++c) {
      card_instance_t *inst = in_play(p, c);
      if (!inst) continue;
      uint32_t old_state = inst->state;
      if (p == 1 - player) inst->state = old_state | 8;
      else if (p == 1) {
        if (check_for_special_ability(1, c, 8)) inst->state |= 4;
        else inst->state |= 0x14;
      }
      inst->regen_status |= 0xe000000;
      check_abilities_power_2022 = get_abilities(p, c, EVENT_POWER, -1);
      check_abilities_toughness_2022 = get_abilities(p, c, EVENT_TOUGHNESS, -1);
      check_abilities_keywords_2022 = get_abilities(p, c, EVENT_ABILITIES, -1);
      int old_activating_player = EXE_DWORD(0x7a3610);
      EXE_DWORD(0x7a3610) = p;
      dispatch_event(p, c, EVENT_CHECK_ABILITIES);
      EXE_DWORD(0x7a3610) = old_activating_player;
      data[p][c].potential_power += check_abilities_power_2022;
      data[p][c].potential_toughness += check_abilities_toughness_2022;
      data[p][c].abils |= check_abilities_keywords_2022;
      int indestructible = check_for_special_ability(p, c, 0x800);
      uint32_t abils = data[p][c].abils;
      if (indestructible) {
        abils |= 0x200;
        data[p][c].abils = abils;
      }
      if (abils & 0x40000000) {
        data[p][c].abils = abils | 0x100;
        if (data[p][c].potential_power > 0) data[p][c].potential_power *= 2;
      }
      inst->state = old_state;
    }
    if (p == 1 - player) final_check_abilities_cop_colors = EXE_DWORD(0x60a428);
    p = 1 - p;
  }
  EXE_DWORD(0x60a428) = final_check_abilities_cop_colors;
  EXE_DWORD(0x60a42c) = final_check_abilities_cop_colors ? has_mana(player, 7, 1) : 0;
  recalculate_all_cards_in_play();
  ai_is_speculating = old_ai_is_speculating;
  EXE_DWORD(0x7a5378) = old_speculating_during_combat;
}
'''
}
