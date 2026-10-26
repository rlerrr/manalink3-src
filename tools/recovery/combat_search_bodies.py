"""Native combat search with explicit candidate storage and signed score arithmetic."""

BODIES = {
'26a8440': r'''
int mod_pow_unblocked(int pow_unblocked, int player, int card)
{
  int id = get_id(player, card), result = pow_unblocked;
  if (id == 0x1744) result += count_graveyard(EXE_DWORD(0x607c2c));
  else if (id < 0x1745) {
    if (id == 0x8fe) result = 2;
    else if (id == 0x908) result = 4;
    else if (id == 0x173) result += 2;
  } else if (id == 0x1916) result = 9999;
  else if (id == 0x3372) result = life[1 - player] - 1;
  if (has_subtype(player, card, SUBTYPE_ROGUE) && check_battlefield_for_id(player, 0x24de)) result += 2;
  return result;
}
''',
'26aa890': r'''
void ai_assign_one_blocker(int player, int blockernum)
{
  int *assignment = EXE_DWORD_PTR(0x607ae4);
  int *attack_cards = EXE_DWORD_PTR(0x607698);
  int *num_blocks = EXE_DWORD_PTR(0x607d78);
  int *blocker_cards = EXE_DWORD_PTR(0x608304);
  if (EXE_DWORD(0x607d54) != blockernum) {
    card_instance_t *inst = get_card_instance(ai_combat_defending_player_2022, blocker_cards[blockernum]);
    if (!(inst->state & 0x8000) || inst->blocking == 255) {
      assignment[blockernum] = -1;
      ai_assign_one_blocker(player, blockernum + 1);
    }
    uint32_t bit = 1u << (blockernum & 31);
    for (int a = 0; a < EXE_DWORD(0x607ccc); ++a) {
      int count = num_blocks[a], card = attack_cards[a];
      if (count < EXE_DWORD(0x607860) || count < minimum_blockers_2022(1 - ai_combat_defending_player_2022, card)) {
        if ((!check_for_special_ability(1 - ai_combat_defending_player_2022, card, 0x200000) || num_blocks[a] < 1) &&
            ((uint32_t)EXE_DWORD_PTR(0x607864)[a] & bit) &&
            (!(inst->state & 0x8000) || inst->blocking == (uint32_t)attack_cards[a])) {
          ++num_blocks[a];
          assignment[blockernum] = attack_cards[a];
          ai_assign_one_blocker(player, blockernum + 1);
          --num_blocks[a];
        }
      }
    }
    return;
  }
  ai_defensive_modifier = 0;
  int life_if_unblocked = life[ai_combat_defending_player_2022];
  int cop_times = EXE_DWORD(0x60a42c);
  int outflanked[blockernum + 1];
  memset(outflanked, 0, (blockernum + 1) * sizeof(int));
  int cop_benefit[16], blockers_that_will_be_destroyed[16];
  int num_cop_benefit = 0, rating = 0, rampage = 0;
  for (int a = 0; a < EXE_DWORD(0x607ccc); ++a) {
    int attack_power = EXE_DWORD_PTR(0x6081fc)[a];
    int attack_toughness = EXE_DWORD_PTR(0x6077a0)[a];
    int attacker = attack_cards[a];
    ai_defensive_modifier += EXE_DWORD_PTR(0x607cd0)[a];
    int attacker_flanking = check_for_special_ability(1 - ai_combat_defending_player_2022, attacker, 0x2000);
    int num_destroyed = 0, any_blockers_have_banding = 0, any_destroyed = 0;
    int total_pow = 0, total_tgh = 0, num_blocking = 0;
    uint32_t attacker_bit = 1u << (a & 31);
    for (int b = 0; b < EXE_DWORD(0x607d54); ++b) {
      if (assignment[b] != attacker) continue;
      ++num_blocking;
      if (num_blocking == 2 && check_for_special_ability(1 - ai_combat_defending_player_2022, attacker, 0x40))
        rampage = dispatch_event_with_attacker(1 - ai_combat_defending_player_2022, attacker, (event_t)0x5e00, -1, 0x40);
      ai_defensive_modifier += EXE_DWORD_PTR(0x607c70)[b];
      int original_tgh = EXE_DWORD_PTR(0x6077e0)[b];
      int tgh = original_tgh, flanked = 0;
      if (attacker_flanking && !check_for_special_ability(ai_combat_defending_player_2022, blocker_cards[b], 0x2000)) {
        if (original_tgh < 2) {
          outflanked[b] = 1;
          any_destroyed = 1;
          blockers_that_will_be_destroyed[num_destroyed++] = b;
          continue;
        }
        --tgh;
        flanked = 1;
      }
      outflanked[b] = 0;
      int pow = EXE_DWORD_PTR(0x6082c0)[b] - flanked;
      total_pow += pow > 0 ? pow : 0;
      total_tgh += tgh > 0 ? tgh : 0;
      if (EXE_DWORD_PTR(0x607618)[b] & 0x40) any_blockers_have_banding = 1;
      if ((uint32_t)EXE_DWORD_PTR(0x607d58)[b] & attacker_bit) total_pow += 99;
      int destroys_this = ((uint32_t)EXE_DWORD_PTR(0x607cb0)[a] >> (b & 31)) & 1;
      if (destroys_this) any_destroyed = destroys_this;
      else if (!any_destroyed && attack_power < original_tgh) {
        any_destroyed = 0;
        continue;
      }
      if (!(EXE_DWORD_PTR(0x607618)[b] & 0x200)) blockers_that_will_be_destroyed[num_destroyed++] = b;
    }
    int32_t combinations = 1u << (num_destroyed & 31);
    if (rampage > 0) {
      rampage *= num_blocking - 1;
      attack_power += rampage;
      attack_toughness += rampage;
    }
    uint32_t attack_abils = EXE_DWORD_PTR(0x6075d8)[a];
    int new_life = life_if_unblocked, evaluate_damage = 0;
    if (num_blocking < minimum_blockers_2022(1 - ai_combat_defending_player_2022, attacker)) {
      if (!EXE_DWORD(0x60a424) || EXE_DWORD(0x60a424) <= EXE_DWORD_PTR(0x607820)[a]) {
        if (player_bits[1 - ai_combat_defending_player_2022] & 0x80000) {
          int commander_damage = check_special_flags2(1 - ai_combat_defending_player_2022, attacker, 0x1000000);
          if (!commander_damage) {
            int commander = get_commander_id(1 - ai_combat_defending_player_2022, 1);
            if (commander == get_id(1 - ai_combat_defending_player_2022, attacker)) commander_damage = 1;
            else {
              commander = get_commander_id(1 - ai_combat_defending_player_2022, 2);
              if (commander == get_id(1 - ai_combat_defending_player_2022, attacker)) commander_damage = 1;
            }
          }
          if (commander_damage) {
            int edh = locate_edh_card_2022(1 - ai_combat_defending_player_2022);
            if (count_counters(1 - ai_combat_defending_player_2022, edh, (counter_t)0x1c) + attack_power >= 21) new_life -= 9999;
          }
        }
        new_life -= mod_pow_unblocked(attack_power, 1 - ai_combat_defending_player_2022, attacker);
        evaluate_damage = 1;
      }
    } else if ((attack_abils & 0x80) && total_tgh < attack_power) {
      new_life -= attack_power - total_tgh;
      evaluate_damage = 1;
    }
    if (evaluate_damage) {
      if (EXE_DWORD(0x60a428) && new_life < life_if_unblocked &&
          (EXE_DWORD(0x60a428) & get_color(1 - ai_combat_defending_player_2022, attacker)))
        cop_benefit[num_cop_benefit++] = life_if_unblocked - new_life;
      life_if_unblocked = new_life;
    }
    if (num_blocking > 0 &&
        ((attack_toughness <= total_pow && (!(attack_abils & 0x300) || num_destroyed < 1)) ||
         (EXE_DWORD(0x60a424) && EXE_DWORD_PTR(0x607820)[a] < EXE_DWORD(0x60a424)))) {
      int64_t convert_64 = (int64_t)EXE_DWORD_PTR(0x60823c)[a] * EXE_DWORD_PTR(0x628c14)[3 - ai_combat_defending_player_2022];
      rating += (int)(convert_64 / 8);
    }
    int best_opinion = -1;
    int effective_attack_power = any_destroyed ? 99 : attack_power;
    total_pow -= num_blocking;
    for (int32_t combo = 0; combo < combinations; ++combo) {
      int remaining = effective_attack_power, attack_rating = 0;
      if (!any_destroyed && any_blockers_have_banding) {
        if (total_pow < effective_attack_power) {
          attack_rating = combo ? 9999 : 0;
          for (int k = 0; k < num_destroyed; ++k) {
            if (!(combo & (1u << (k & 31)))) continue;
            int n = blockers_that_will_be_destroyed[k];
            if (EXE_DWORD_PTR(0x6077e0)[n] <= effective_attack_power) {
              int value = EXE_DWORD_PTR(0x60834c)[n];
              remaining = is_protected_from_2022(ai_combat_defending_player_2022, blocker_cards[n], 1 - ai_combat_defending_player_2022, attacker, IPF2022_DAMAGE);
              if (!remaining) {
                if (value < attack_rating) attack_rating = value;
                break;
              }
            }
            attack_rating = 0;
            remaining = effective_attack_power;
          }
        }
      } else {
        for (int l = 0; l < num_destroyed; ++l) {
          if (!(combo & (1u << (l & 31)))) continue;
          int n = blockers_that_will_be_destroyed[l];
          if (!(EXE_DWORD_PTR(0x607618)[n] & 0x100) || EXE_DWORD_PTR(0x6082c0)[n] < attack_toughness) {
            if (outflanked[n] || !is_protected_from_2022(ai_combat_defending_player_2022, blocker_cards[n], 1 - ai_combat_defending_player_2022, attacker, IPF2022_DAMAGE))
              attack_rating += EXE_DWORD_PTR(0x60834c)[n];
          }
          remaining -= EXE_DWORD_PTR(0x6077e0)[n];
          if (remaining < 0) break;
        }
      }
      if (remaining >= 0 && best_opinion < attack_rating) best_opinion = attack_rating;
    }
    rating -= best_opinion;
  }
  rating += ai_defensive_modifier;
  while (cop_times != 0 && num_cop_benefit > 0) {
    int best = 0, best_idx = 0;
    for (int i = 0; i < num_cop_benefit; ++i) {
      if (best < cop_benefit[i]) { best = cop_benefit[i]; best_idx = i; }
    }
    if (!best) break;
    --cop_times;
    life_if_unblocked += cop_benefit[best_idx];
    cop_benefit[best_idx] = 0;
  }
  int penalty;
  if (life_if_unblocked < 1) penalty = life_if_unblocked * -96 + 999;
  else {
    int64_t rating_64 = (int64_t)(life[ai_combat_defending_player_2022] - life_if_unblocked) * EXE_DWORD_PTR(0x628c14)[ai_combat_defending_player_2022];
    penalty = (int)((rating_64 * 6) / life_if_unblocked);
  }
  if (EXE_DWORD(0x60827c) < rating - penalty) {
    EXE_DWORD(0x60827c) = rating - penalty;
    memcpy(EXE_DWORD_PTR(0x60775c), assignment, 16 * sizeof(int));
  }
}
''',
'26abd60': r'''
void ai_assign_blockers_2022(int player)
{
  EXE_FN(void, 0x4af6f0, int)(player);
  EXE_DWORD(0x60827c) = -9999;
  memset(EXE_DWORD_PTR(0x6076d8), 0, 16 * sizeof(int));
  ai_assign_one_blocker(player, 0);
  if (ai_is_speculating != 1) {
    if (current_turn != 0) { enforce_minimum_blockers(); return; }
    if (event_flags & 8) { human_assign_blockers_2022(player); return; }
  }
  for (int i = 0; i < EXE_DWORD(0x607d54); ++i)
    get_card_instance(EXE_DWORD(0x607c2c), EXE_DWORD_PTR(0x608304)[i])->blocking = EXE_DWORD_PTR(0x60775c)[i];
  enforce_minimum_blockers();
  for (int i = 0; i < EXE_DWORD(0x607d54); ++i) {
    uint32_t assignment = EXE_DWORD_PTR(0x60775c)[i];
    card_instance_t *inst = get_card_instance(EXE_DWORD(0x607c2c), EXE_DWORD_PTR(0x608304)[i]);
    if (inst->blocking == 255) continue;
    uint8_t blk = assignment;
    if (EXE_DWORD_PTR(0x60775c)[i] != -1) {
      forbid_attack = 0;
      if (event_flags & 0x800000) {
        EXE_FN(void, 0x435c80, void)();
        trigger_cause_controller = EXE_DWORD(0x607c2c);
        trigger_cause = EXE_DWORD_PTR(0x608304)[i];
        dispatch_trigger(1 - current_turn, TRIGGER_PAY_TO_BLOCK, EXE_STR(0x790248), 1);
        EXE_FN(void, 0x435cd0, void)();
        if (forbid_attack) {
          inst->blocking = 255;
          forbid_attack = 0;
          inst->state &= 0xffff7fff;
          continue;
        }
      }
      if (event_flags & 0x100000)
        dispatch_trigger2(current_turn, TRIGGER_BLOCKER_CHOSEN, EXE_STR(0x790074), 0,
            EXE_DWORD(0x607c2c), EXE_DWORD_PTR(0x608304)[i]);
      if (blk != 255) {
        get_card_instance(1 - EXE_DWORD(0x607c2c), assignment & 255)->state |= 0x4000;
        inst->state |= 8;
      }
    }
    int redisplay = ai_is_speculating != 1;
    inst->blocking = blk;
    if (redisplay) {
      EXE_FN(void, 0x437e20, void)();
      if (EXE_DWORD_PTR(0x60775c)[i] != -1) play_sound_effect(WAV_BLOCK2);
    }
  }
  enforce_minimum_blockers();
}
'''
}


BODIES['26ac340'] = r'''
int ai_choose_attackers(int player)
{
  typedef struct {
    int potential_power, potential_toughness;
    keyword_t abils;
    int opinion_of_game_state_without_this;
  } ai_combat_data_t;
  typedef struct { int card, effectiveness; int8_t bands, one_of_best_7; } PotentialAttacker;
  typedef struct { int card, pow; } CardPower;
  ai_combat_data_t (*data)[150] = (ai_combat_data_t (*)[150])0x609164;
  setup_ai_combat_data(player);
  int store_ai_is_speculating = ai_is_speculating;
  if (store_ai_is_speculating == 1) EXE_DWORD(0x60e9fc) ^= 2;
  for (int c = 0; c < active_cards_count[player]; ++c) get_card_instance(player, c)->state &= ~4;
  int banding_attack_indices[16], attack_indices[16] = {0}, attack_value_assessment[16] = {0};
  int num_banding_attack_indices = 0, num_attack_indices = 0, total_potential_power_banding = 0;
  uint32_t bitfield_of_unk8000 = 0, ai_combat_data_abils_not_present = UINT32_MAX;
  int combos = 1, has_banding_attackers = 0;
  {
    PotentialAttacker pot_atck[150];
    int num_pot_atck = 0, num_pot_atck_nonbanding = 0;
    for (int c = 0; c < active_cards_count[player]; ++c) {
      card_instance_t *inst = in_play(player, c);
      if (!inst) continue;
      if (!is_what(player, c, TYPE_CREATURE) && !(inst->state & 0x1000000)) continue;
      if ((inst->state & 0x20010) || !can_attack(player, c)) continue;
      pot_atck[num_pot_atck].card = c;
      pot_atck[num_pot_atck].effectiveness = EXE_FN(int, 0x4b4350, int, int)(player, c);
      int bands = (EXE_DWORD(0x60e9fc) & 2) && (inst->state & 0x8000) && (data[player][c].abils & 0x40);
      pot_atck[num_pot_atck].bands = bands;
      pot_atck[num_pot_atck].one_of_best_7 = 0;
      if (!bands) ++num_pot_atck_nonbanding;
      ++num_pot_atck;
    }
    if (num_pot_atck_nonbanding >= 8) {
      qsort(pot_atck, num_pot_atck, sizeof(PotentialAttacker),
          (int (*)(const void *, const void *))qsort_compare_attacker_effectiveness);
      int num_fast_attackers = 0;
      for (int i = 0; i < num_pot_atck && num_fast_attackers < 16; ++i) {
        if (!pot_atck[i].bands) attack_indices[++num_fast_attackers] = pot_atck[i].card;
      }
      int attacker_power[16] = {0};
      for (int i = 0; i < num_fast_attackers; ++i) attacker_power[i] = data[player][attack_indices[i]].potential_power;
      int num_potential_blockers = 0;
      for (int c = 0; c < active_cards_count[ai_combat_defending_player_2022]; ++c) {
        card_instance_t *inst = in_play(ai_combat_defending_player_2022, c);
        if (inst && !(inst->state & 0x10) && is_what(ai_combat_defending_player_2022, c, TYPE_CREATURE)) ++num_potential_blockers;
      }
      for (int i = 0; i < num_potential_blockers; ++i) {
        int highest_potential_power = 0, highest_potential_power_idx = 0;
        for (int j = 0; j < num_fast_attackers; ++j) {
          if (highest_potential_power < attacker_power[j]) {
            highest_potential_power = attacker_power[j];
            highest_potential_power_idx = j;
          }
        }
        attacker_power[highest_potential_power_idx] = 0;
      }
      int total_potential_unblocked_power = 0;
      for (int i = 0; i < num_fast_attackers; ++i) total_potential_unblocked_power += attacker_power[i];
      if (life[1 - player] <= total_potential_unblocked_power) {
        for (int i = 0; i < num_fast_attackers; ++i) get_card_instance(player, attack_indices[i])->state |= 4;
        int valid = 1;
        if (event_flags & 0x80) {
          event_result = 0;
          dispatch_event_raw(EVENT_ATTACK_LEGALITY_ALL);
          valid = event_result == 0;
        }
        if (valid) {
          must_attack = (1u << (num_fast_attackers & 31)) - 1;
          return must_attack;
        }
        for (int i = 0; i < num_fast_attackers; ++i) get_card_instance(player, attack_indices[i])->state &= ~4;
      }
    }
    int best_7_marked = 0;
    for (int i = 0; i < num_pot_atck && best_7_marked < 7; ++i) {
      if (!pot_atck[i].bands) { pot_atck[i].one_of_best_7 = 1; ++best_7_marked; }
    }
    if (num_pot_atck_nonbanding > 7)
      qsort(pot_atck, num_pot_atck, sizeof(PotentialAttacker),
          (int (*)(const void *, const void *))qsort_cardvalue_ascending);
    uint32_t banding_regen = 0;
    for (int i = 0; i < num_pot_atck; ++i) {
      int c = pot_atck[i].card;
      if (pot_atck[i].bands) {
        if (num_banding_attack_indices < 16) {
          keyword_t abils = data[player][c].abils;
          total_potential_power_banding += data[player][c].potential_power;
          ai_combat_data_abils_not_present &= abils;
          banding_regen |= abils & KEYWORD_REGENERATION;
          banding_attack_indices[num_banding_attack_indices++] = c;
        }
      } else if (pot_atck[i].one_of_best_7) {
        if (num_attack_indices > 6) backtrace_and_abort("functions/engine.c:2829\nnum_attack_indices < 7");
        card_instance_t *inst = get_card_instance(player, c);
        attack_indices[num_attack_indices] = c;
        attack_value_assessment[num_attack_indices] = pot_atck[i].effectiveness;
        uint32_t state = inst->state;
        inst->state = state | 4;
        if (state & 0x8000) bitfield_of_unk8000 |= 1u << (num_attack_indices & 31);
        ++num_attack_indices;
      }
    }
    has_banding_attackers = num_banding_attack_indices != 0;
    ai_combat_data_abils_not_present |= banding_regen;
    combos = 1u << (num_attack_indices & 31);
  }
  EXE_FN(void, 0x4af6f0, int)(player);
  int local_ai_combat_attack_power[16], local_ai_combat_attack_toughness[16];
  keyword_t local_ai_combat_attack_abils[16];
  int local_ai_combat_attack_rating[16], local_ai_combat_blocking_choices_bitmask[16];
  int local_ai_combat_attack_defensive_modifier[16], local_ai_combat_block_defensive_modifier[16];
  memcpy(local_ai_combat_attack_power, EXE_DWORD_PTR(0x6081fc), 64);
  memcpy(local_ai_combat_attack_toughness, EXE_DWORD_PTR(0x6077a0), 64);
  memcpy(local_ai_combat_attack_abils, EXE_DWORD_PTR(0x6075d8), 64);
  memcpy(local_ai_combat_attack_rating, EXE_DWORD_PTR(0x60823c), 64);
  memcpy(local_ai_combat_blocking_choices_bitmask, EXE_DWORD_PTR(0x607864), 64);
  memcpy(local_ai_combat_attack_defensive_modifier, EXE_DWORD_PTR(0x607cd0), 64);
  memcpy(local_ai_combat_block_defensive_modifier, EXE_DWORD_PTR(0x607c70), 64);
  EXE_FN(void, 0x498e30, void)();
  ai_is_speculating = 1;
  dispatch_event_raw(EVENT_SHOULD_AI_PLAY);
  ai_is_speculating = store_ai_is_speculating;
  int blocker_power[16], blocker_toughness[16];
  uint8_t card_to_blocker_pt_idx[150], card_to_attacker_pt_idx[150];
  int highest_blocker_toughness = 0, num_blocker_pt = 0;
  for (int c = 0; c < active_cards_count[ai_combat_defending_player_2022]; ++c) {
    card_instance_t *inst = in_play(ai_combat_defending_player_2022, c);
    if (!inst || !is_what(ai_combat_defending_player_2022, c, TYPE_CREATURE)) continue;
    int p = ai_combat_defending_player_2022;
    int pow = data[ai_combat_defending_player_2022][c].potential_power;
    int tgh = data[ai_combat_defending_player_2022][c].potential_toughness;
    blocker_power[num_blocker_pt] = pow;
    blocker_toughness[num_blocker_pt] = tgh;
    uint32_t ea = cards_data[inst->internal_card_id].extra_ability;
    CardFunction fn = cards_data[inst->internal_card_id].code_pointer;
    inst = get_card_instance(ai_combat_defending_player_2022, c);
    if (ea & 0x18) {
      int8_t old_activating_player = EXE_DWORD(0x7a3610);
      EXE_DWORD(0x7a3610) = p;
      int swapped = check_special_flags3(p, c, 0x100000);
      if (!swapped) {
        if (ea & 8) blocker_power[num_blocker_pt] = pow + call_card_fn(fn, inst, p, c, EVENT_POW_BOOST);
        if (ea & 0x10) {
          tgh += call_card_fn(fn, inst, p, c, EVENT_TOU_BOOST);
          blocker_toughness[num_blocker_pt] = tgh;
        }
      } else {
        if (ea & 8) {
          tgh += call_card_fn(fn, inst, p, c, EVENT_POW_BOOST);
          blocker_toughness[num_blocker_pt] = tgh;
        }
        if (ea & 0x10) blocker_power[num_blocker_pt] = pow + call_card_fn(fn, inst, p, c, EVENT_TOU_BOOST);
      }
      EXE_DWORD(0x7a3610) = old_activating_player;
    }
    if (highest_blocker_toughness < tgh) highest_blocker_toughness = tgh;
    card_to_blocker_pt_idx[c] = num_blocker_pt;
    if (++num_blocker_pt > 15) break;
  }
  int band = -1;
  for (int i = 0; i < num_attack_indices; ++i) {
    if (local_ai_combat_attack_power[i] < highest_blocker_toughness &&
        highest_blocker_toughness <= local_ai_combat_attack_power[i] + total_potential_power_banding &&
        local_ai_combat_attack_abils[i] == (local_ai_combat_attack_abils[i] & ai_combat_data_abils_not_present)) {
      local_ai_combat_attack_power[i] += total_potential_power_banding;
      band = attack_indices[i];
      break;
    }
  }
  int attacker_power[16] = {0}, attacker_toughness[16] = {0}, num_attacker_pt = 0;
  for (int c = 0; c < active_cards_count[player]; ++c) {
    card_instance_t *inst = in_play(player, c);
    if (!inst || !is_what(player, c, TYPE_CREATURE)) continue;
    int pow = data[player][c].potential_power, tgh = data[player][c].potential_toughness;
    attacker_power[num_attacker_pt] = pow;
    attacker_toughness[num_attacker_pt] = tgh;
    attack_value_assessment[num_attacker_pt] = (pow + 1) * (tgh + 1);
    for (int i = 0; i < EXE_DWORD(0x607ccc); ++i)
      if (EXE_DWORD_PTR(0x607698)[i] == c) attack_value_assessment[num_attacker_pt] = EXE_DWORD_PTR(0x60823c)[i];
    card_to_attacker_pt_idx[c] = num_attacker_pt;
    if (++num_attacker_pt > 15) break;
  }
  EXE_FN(void, 0x498e70, void)();
  int chosen_combo = 0, combo_rating = 9999;
  for (int combo = 0; combo < combos; ++combo) {
    EXE_DWORD(0x607ccc) = 0;
    EXE_DWORD(0x60779c) = 0;
    int8_t combo_valid = 1;
    for (int i = 0; i < num_attack_indices; ++i) {
      int c = attack_indices[i];
      card_instance_t *inst = get_card_instance(player, c);
      int csvid = get_id(player, c);
      if (((uint32_t)combo >> (i & 31)) & 1) {
        inst->state |= 4;
        int n = EXE_DWORD(0x607ccc);
        EXE_DWORD_PTR(0x607698)[n] = c;
        EXE_DWORD_PTR(0x6081fc)[n] = local_ai_combat_attack_power[i];
        EXE_DWORD_PTR(0x6077a0)[n] = local_ai_combat_attack_toughness[i];
        EXE_DWORD_PTR(0x6075d8)[n] = local_ai_combat_attack_abils[i];
        EXE_DWORD_PTR(0x60823c)[n] = local_ai_combat_attack_rating[i];
        EXE_DWORD_PTR(0x607864)[n] = local_ai_combat_blocking_choices_bitmask[i];
        EXE_DWORD_PTR(0x607cd0)[n] = local_ai_combat_attack_defensive_modifier[i];
        EXE_DWORD_PTR(0x607c70)[n] = local_ai_combat_block_defensive_modifier[i];
        if (csvid == 0x28 || csvid == 0x98) EXE_DWORD(0x60779c) |= 1u << (n & 31);
        ++EXE_DWORD(0x607ccc);
      } else {
        inst->state &= ~4;
        if (csvid == 0x19f || csvid == 0x84) combo_valid = 0;
        else combo_valid &= (bitfield_of_unk8000 & (1u << (i & 31))) == 0;
      }
    }
    if (EXE_DWORD(0x607ccc) > 16) backtrace_and_abort("functions/engine.c:2971\nnum_attackers <= 16");
    if (event_flags & 0x80) {
      event_result = 0;
      dispatch_event_raw(EVENT_ATTACK_LEGALITY_ALL);
      if (event_result) continue;
    }
    if (!combo_valid) continue;
    if (EXE_DWORD(0x60a424) && EXE_DWORD(0x607ccc) > 0) {
      int assassin_tgt[16];
      for (int i = 0; i < EXE_DWORD(0x607ccc); ++i)
        assassin_tgt[i] = (get_special_abilities(player, EXE_DWORD_PTR(0x607698)[i]) & 8) ? 9999 : i;
      if (EXE_DWORD(0x607ccc) > 0) {
        for (int j = 1; j < EXE_DWORD(0x607ccc); ++j) {
          int lhs = assassin_tgt[0], rhs = assassin_tgt[j];
          if (lhs != 9999 && rhs != 9999 && EXE_DWORD_PTR(0x6081fc)[lhs] < EXE_DWORD_PTR(0x6081fc)[rhs]) {
            assassin_tgt[j] = lhs;
            assassin_tgt[0] = rhs;
          }
        }
        for (int i = 0; i < EXE_DWORD(0x607ccc); ++i) {
          if (assassin_tgt[i] == 9999) EXE_DWORD_PTR(0x607820)[i] = 9999;
          else EXE_DWORD_PTR(0x607820)[assassin_tgt[i]] = i;
        }
      }
    }
    EXE_DWORD(0x607d50) = 1;
    ai_assign_blockers_2022(player);
    EXE_FN(void, 0x498e30, void)();
    ai_is_speculating = 1;
    dispatch_event_raw(EVENT_SHOULD_AI_PLAY);
    ai_is_speculating = store_ai_is_speculating;
    CardPower cardpow[8];
    for (int i = 0; i < 8; ++i) cardpow[i].pow = -1;
    for (int c = 0; c < active_cards_count[ai_combat_defending_player_2022]; ++c) {
      card_instance_t *inst = in_play(ai_combat_defending_player_2022, c);
      if (!inst || !is_what(ai_combat_defending_player_2022, c, TYPE_CREATURE) || !(inst->state & 0x400)) continue;
      int pow_unblocked = mod_pow_unblocked(blocker_power[card_to_blocker_pt_idx[c]], ai_combat_defending_player_2022, c);
      if (pow_unblocked < 0) pow_unblocked = 0;
      int pos = 0;
      while (pos < 8 && pow_unblocked <= cardpow[pos].pow) ++pos;
      if (pos == 8) continue;
      for (int j = 7; j > pos; --j) cardpow[j] = cardpow[j - 1];
      cardpow[pos].card = c;
      cardpow[pos].pow = pow_unblocked;
    }
    int total_unblocked_opp_power = 0;
    for (int i = 0; i < 8 && cardpow[i].pow != -1; ++i) {
      int oppcard = cardpow[i].card;
      keyword_t abils = get_abilities(ai_combat_defending_player_2022, oppcard, EVENT_ABILITIES, -1);
      int opp_tgh = blocker_toughness[card_to_blocker_pt_idx[oppcard]];
      int opp_pow = mod_pow_unblocked(blocker_power[card_to_blocker_pt_idx[oppcard]], ai_combat_defending_player_2022, oppcard);
      int my_chump = 0, have_chump = 0, free_block = 0;
      int64_t chump_assessment = 32767;
      for (int c = 0; c < active_cards_count[player]; ++c) {
        card_instance_t *inst = in_play(player, c);
        if (!inst || !(inst->state & 0x400) || !is_what(player, c, TYPE_CREATURE)) continue;
        uint32_t state = inst->state;
        int vigilant = check_for_special_ability(player, c, 8);
        if (state & (8 | (vigilant ? 0 : 4))) continue;
        if (!is_legal_block_impl_2022(player, c, ai_combat_defending_player_2022, oppcard, abils, 0)) continue;
        int idx = card_to_attacker_pt_idx[c];
        if (opp_pow < attacker_toughness[idx] || opp_tgh <= attacker_power[idx]) {
          inst->state |= 8;
          free_block = 1;
          break;
        }
        have_chump = 1;
        int64_t value = attack_value_assessment[idx];
        if (value < chump_assessment) { my_chump = c; chump_assessment = value; }
      }
      if (free_block) continue;
      int denominator = life[player] - opp_pow;
      if (denominator < 1) denominator = 1;
      else if (denominator > 99) denominator = 99;
      int cost = (int32_t)((uint32_t)EXE_DWORD_PTR(0x628c14)[player] * (uint32_t)opp_pow * 6u);
      cost /= denominator;
      if (have_chump) {
        int chump = (int)(((int64_t)EXE_DWORD_PTR(0x628c14)[player + 2] * chump_assessment) / 8);
        if (cost >= chump) {
          EXE_DWORD(0x60827c) += chump;
          get_card_instance(player, my_chump)->state |= 8;
          continue;
        }
      }
      EXE_DWORD(0x60827c) += cost;
      total_unblocked_opp_power += opp_pow;
    }
    EXE_FN(void, 0x498e70, void)();
    if (life[player] > 0 && life[player] <= total_unblocked_opp_power && life[1 - player] > 0)
      EXE_DWORD(0x60827c) += (total_unblocked_opp_power - life[player] + 2) * 128;
    if (EXE_DWORD(0x60827c) < combo_rating) {
      chosen_combo = combo;
      combo_rating = EXE_DWORD(0x60827c);
    }
  }
  if (chosen_combo && (event_flags & 0x2000000)) dispatch_event_raw(EVENT_BEFORE_COMBAT);
  for (int i = 0; i < num_attack_indices; ++i) {
    int c = attack_indices[i];
    card_instance_t *inst = get_card_instance(player, c);
    if (!(((uint32_t)chosen_combo >> (i & 31)) & 1)) inst->state &= ~4;
    else {
      inst->state |= 4;
      if (band == -1 && has_banding_attackers &&
          local_ai_combat_attack_abils[i] == (ai_combat_data_abils_not_present & local_ai_combat_attack_abils[i])) band = c;
    }
  }
  if (band == -1 || !has_banding_attackers || !(get_card_instance(player, band)->state & 4)) {
    if (!highest_blocker_toughness && has_banding_attackers) {
      for (int i = 0; i < num_banding_attack_indices; ++i) get_card_instance(player, banding_attack_indices[i])->state |= 4;
      chosen_combo = 1;
    }
  } else {
    for (int i = 0; i < num_banding_attack_indices; ++i) {
      card_instance_t *inst = get_card_instance(player, banding_attack_indices[i]);
      inst->blocking = band;
      inst->state |= 4;
    }
    get_card_instance(player, band)->blocking = band;
  }
  if (event_flags & 0x80) {
    do {
      event_result = 0;
      dispatch_event_raw(EVENT_ATTACK_LEGALITY_ALL);
      if (!event_result) { must_attack = chosen_combo; return chosen_combo; }
      undeclare_attacker(player, event_result & 0xffff);
    } while (event_result != 0);
  }
  must_attack = chosen_combo;
  return chosen_combo;
}
'''
