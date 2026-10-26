"""Typed reconstructions of native engine state, legacy and mana helpers."""

BODIES = {
'26a8700': r'''
void count_colors_of_lands_in_play_2022(void)
{

  int i;
  for (i = 0; i < 8; ++i)
  {
    basiclandtypes_controlled[0][i] = 0;
    basiclandtypes_controlled[1][i] = 0;
    landsofcolor_controlled[0][i] = 0;
    landsofcolor_controlled[1][i] = 0;
  }

  colors_of_lands_in_play[0] = 0;
  colors_of_lands_in_play[1] = 0;

  int player, card;
  for (player = 0; player < 2; ++player)
    for (card = 0; card < active_cards_count[player]; ++card)
      if (in_play(player, card) && is_what(player, card, TYPE_LAND))
      {
        card_instance_t *instance = get_card_instance(player, card);

        int colors = instance->card_color | mana_colors_added[player];
        int basics;

        ++landsofcolor_controlled[player][COLOR_ANY];
        ++basiclandtypes_controlled[player][COLOR_ANY];

        if (check_special_flags2(player, card, SF2_PRISMATIC_OMEN))
        {
          colors |= COLOR_TEST_ANY_COLORED;
          basics = COLOR_TEST_ANY_COLORED;
        }
        else
        {
          basics = 0;

          if (has_subtype(player, card, SUBTYPE_SWAMP))
            basics |= COLOR_TEST_BLACK;
          if (has_subtype(player, card, SUBTYPE_ISLAND))
            basics |= COLOR_TEST_BLUE;
          if (has_subtype(player, card, SUBTYPE_FOREST))
            basics |= COLOR_TEST_GREEN;
          if (has_subtype(player, card, SUBTYPE_MOUNTAIN))
            basics |= COLOR_TEST_RED;
          if (has_subtype(player, card, SUBTYPE_PLAINS))
            basics |= COLOR_TEST_WHITE;

          colors |= basics;
        }

        if (check_special_flags2(player, card, SF2_CONTAMINATION))
          colors = COLOR_TEST_BLACK;

        if (colors & COLOR_TEST_BLACK)
          ++landsofcolor_controlled[player][COLOR_BLACK];
        if (basics & COLOR_TEST_BLACK)
          ++basiclandtypes_controlled[player][COLOR_BLACK];

        if (colors & COLOR_TEST_BLUE)
          ++landsofcolor_controlled[player][COLOR_BLUE];
        if (basics & COLOR_TEST_BLUE)
          ++basiclandtypes_controlled[player][COLOR_BLUE];

        if (colors & COLOR_TEST_GREEN)
          ++landsofcolor_controlled[player][COLOR_GREEN];
        if (basics & COLOR_TEST_GREEN)
          ++basiclandtypes_controlled[player][COLOR_GREEN];

        if (colors & COLOR_TEST_RED)
          ++landsofcolor_controlled[player][COLOR_RED];
        if (basics & COLOR_TEST_RED)
          ++basiclandtypes_controlled[player][COLOR_RED];

        if (colors & COLOR_TEST_WHITE)
          ++landsofcolor_controlled[player][COLOR_WHITE];
        if (basics & COLOR_TEST_WHITE)
          ++basiclandtypes_controlled[player][COLOR_WHITE];

        if (colors & COLOR_TEST_ARTIFACT)
          ++landsofcolor_controlled[player][COLOR_ARTIFACT];

        if (!(colors & (COLOR_TEST_ANY_COLORED | COLOR_TEST_ARTIFACT)))
          ++landsofcolor_controlled[player][COLOR_COLORLESS];

        if (!(basics & COLOR_TEST_ANY_COLORED))
          ++basiclandtypes_controlled[player][COLOR_COLORLESS];

        colors_of_lands_in_play[player] |= colors;
      }

  colors_of_lands_in_play[0] &= COLOR_TEST_ANY_COLORED;
  colors_of_lands_in_play[1] &= COLOR_TEST_ANY_COLORED;

  if ((player_bits[current_turn] & PB_COUNT_TOTAL_PLAYABLE_LANDS) &&
      (land_can_be_played & LCBP_LAND_HAS_BEEN_PLAYED) &&
      lands_played < total_playable_lands(current_turn))
    land_can_be_played &= ~LCBP_LAND_HAS_BEEN_PLAYED;
}
''',

'26a8bb0': r'''
void phase_changed_2022(int player, int new_phase)
{

  if (ai_is_speculating != 1)
    EXE_FN(void, 0x4377e0, int, int)(player, new_phase);

  cancel = 0;

  dispatch_event(player, 0, EVENT_PHASE_CHANGED);
}
''',

'26a8c00': r'''
int effect_asterisk_2022(int player, int card, event_t event)
{

  card_instance_t *instance = get_card_instance(player, card);
  if (!affect_me(instance->damage_target_player, instance->damage_target_card) ||
      affected_card == -1 ||
      !((event == EVENT_POWER && (instance->eot_toughness & ASTERISK_AFFECTS_POWER)) ||
        (event == EVENT_TOUGHNESS && (instance->eot_toughness & ASTERISK_AFFECTS_TOUGHNESS))))
    return 0;

  int p, c;
  int tp = instance->damage_target_player, tc = instance->damage_target_card;
  int modifier = 0;
  int count_who = instance->eot_toughness & (ASTERISK_COUNT_CONTROLLER | ASTERISK_COUNT_OPPONENT);
  int category =
      instance->eot_toughness &
      (ASTERISK_SUBTYPE_OF_INFO_SLOT | ASTERISK_HAND_COUNT |
       ASTERISK_CREATURES_IN_GRAVEYARD_PLUS_POWER_IN_BYTE0_TOUGHNESS_IN_BYTE1_OF_INFO_SLOT |
       ASTERISK_POWER_IN_BYTE0_TOUGHNESS_IN_BYTE1_MIN1_OF_INFO_SLOT | ASTERISK_NONWALL_CREATURES |
       ASTERISK_POWER_IN_BYTE0_TOUGHNESS_IN_BYTE1_OF_INFO_SLOT | ASTERISK_IID_OF_INFO_SLOT |
       ASTERISK_BASICLAND_TYPES_OF_INFO_SLOT);

  switch (category)
  {
  case ASTERISK_BASICLAND_TYPES_OF_INFO_SLOT:

    if (instance->eot_toughness & ASTERISK_COUNT_CONTROLLER)
      modifier += basiclandtypes_controlled[tp][get_hacked_color(tp, tc, instance->info_slot)];

    if (instance->eot_toughness & ASTERISK_COUNT_OPPONENT)
      modifier += basiclandtypes_controlled[1 - tp][get_hacked_color(tp, tc, instance->info_slot)];

    break;

  case ASTERISK_IID_OF_INFO_SLOT:

    for (p = 0; p < 2; ++p)
      if (count_who & (p == tp ? ASTERISK_COUNT_CONTROLLER : ASTERISK_COUNT_OPPONENT))
        for (c = 0; c < active_cards_count[p]; ++c)
          if (in_play(p, c))
          {
            if (instance->eot_toughness &
                ASTERISK_TYPE_OF_INFO_SLOT__MUST_ALSO_SET_IID_OF_INFO_SLOT)
            {
              if (is_what(p, c, instance->info_slot))
                ++modifier;
            }
            else if (get_id(p, c) == cards_data[instance->info_slot].id)
              ++modifier;
          }
    break;

  case ASTERISK_NONWALL_CREATURES:

    for (p = 0; p < 2; ++p)
      if (count_who & (p == tp ? ASTERISK_COUNT_CONTROLLER : ASTERISK_COUNT_OPPONENT))
        for (c = 0; c < active_cards_count[p]; ++c)
          if (in_play(p, c) && is_what(p, c, TYPE_CREATURE) && !has_subtype(p, c, SUBTYPE_WALL))
            ++modifier;
    break;

  case ASTERISK_CREATURES_IN_GRAVEYARD_PLUS_POWER_IN_BYTE0_TOUGHNESS_IN_BYTE1_OF_INFO_SLOT:

    for (p = 0; p < 2; ++p)
      if (count_who & (p == tp ? ASTERISK_COUNT_CONTROLLER : ASTERISK_COUNT_OPPONENT))
        for (c = 0; c < 500; ++c)
        {
          int iid = get_grave(p)[c];
          if (iid >= 0 && is_what(-1, iid, TYPE_CREATURE))
            ++modifier;
        }

    if (event == EVENT_POWER)
      modifier += BYTE0(instance->info_slot);
    else
      modifier += BYTE1(instance->info_slot);
    break;

  case ASTERISK_HAND_COUNT:

    if (count_who & ASTERISK_COUNT_CONTROLLER)
      modifier += hand_count[player];
    if (count_who & ASTERISK_COUNT_OPPONENT)
      modifier += hand_count[1 - player];
    break;

  case ASTERISK_SUBTYPE_OF_INFO_SLOT:

    for (p = 0; p < 2; ++p)
      if (count_who & (p == tp ? ASTERISK_COUNT_CONTROLLER : ASTERISK_COUNT_OPPONENT))
        for (c = 0; c < active_cards_count[p]; ++c)
          if (in_play(p, c) &&
              has_subtype(p, c, from_hardcodedsubtype_to_subtype(instance->info_slot)))
            ++modifier;
    break;

  case ASTERISK_POWER_IN_BYTE0_TOUGHNESS_IN_BYTE1_OF_INFO_SLOT:

    if (event == EVENT_POWER)
      modifier = BYTE0(instance->info_slot);
    else
      modifier = BYTE1(instance->info_slot);
    break;

  case ASTERISK_POWER_IN_BYTE0_TOUGHNESS_IN_BYTE1_MIN1_OF_INFO_SLOT:

    if (event == EVENT_POWER)
      modifier = BYTE0(instance->info_slot);
    else
    {
      modifier = BYTE1(instance->info_slot);
      if (event_result + modifier < 1)
      {
        if (event_result > 1)
          modifier = 1 - event_result;
        else
          modifier = 0;
      }
    }
    break;
  }

  event_result += modifier;

  if (event == EVENT_POWER)
    instance->counter_power = modifier;
  else
    instance->counter_toughness = modifier;

  return 0;
}
''',

'26a9140': r'''
int create_legacy_effect_exe_2022(int player, int card, int iid, int t_player, int t_card)
{

  if (hack_force_effect_change_source.from.player != -1 &&
      player == hack_force_effect_change_source.from.player &&
      card == hack_force_effect_change_source.from.card)
  {
    player = hack_force_effect_change_source.to.player;
    card = hack_force_effect_change_source.to.card;
  }

  card_instance_t *instance = get_card_instance(player, card);
  int csvid;

  if (instance->internal_card_id == activation_card)
    return EXE_FN(int, 0x4a09a0, int, int, int, int,
                  int)(instance->parent_controller, instance->parent_card, iid, t_player, t_card);

  if (instance->internal_card_id < 0)
    return EXE_FN(int, 0x4a09a0, int, int, int, int, int)(player, card, iid, t_player, t_card);

  csvid = cards_data[instance->internal_card_id].id;
  if (csvid >= 901 && csvid <= 908)
  {
    int leg = EXE_FN(int, 0x4a09a0, int, int, int, int, int)(player, card, iid, t_player, t_card);
    if (leg != -1)
    {
      card_instance_t *legacy = get_card_instance(player, leg);
      legacy->damage_source_player = instance->damage_source_player;
      legacy->damage_source_card = instance->damage_source_card;
      legacy->original_internal_card_id = instance->original_internal_card_id;
      legacy->display_pic_csv_id = instance->display_pic_csv_id;
      legacy->display_pic_num = instance->display_pic_num;
    }
    return leg;
  }

  if (csvid == CARD_ID_FACE_DOWN_CREATURE && instance->targets[12].player >= 0 &&
      instance->targets[12].player <= available_slots)
  {
    int leg = EXE_FN(int, 0x4a09a0, int, int, int, int, int)(player, card, iid, t_player, t_card);
    if (leg != -1)
    {
      card_instance_t *legacy = get_card_instance(player, leg);
      legacy->original_internal_card_id =
          get_internal_card_id_from_csv_id(instance->targets[12].player);
      legacy->display_pic_csv_id = instance->targets[12].player;
      legacy->display_pic_num = get_card_image_number(instance->targets[12].player, player, card);
    }
    return leg;
  }

  return EXE_FN(int, 0x4a09a0, int, int, int, int, int)(player, card, iid, t_player, t_card);
}
''',

'26a9320': r'''
int create_legacy_effect_from_iid_2022(int player, int iid, int (*func_ptr)(int, int, event_t),
                                       int t_player, int t_card)
{
  int legacy_card = add_card_to_hand(player, LEGACY_EFFECT_CUSTOM);
  if (legacy_card == -1)
    return -1;

  card_instance_t *legacy = get_card_instance(player, legacy_card);
  legacy->state = (player == 1) ? STATE_IN_PLAY | STATE_OWNED_BY_OPPONENT : STATE_IN_PLAY;
  --hand_count[player];

  card_data_t *cd = &cards_data[iid];
  legacy->mana_color = legacy->card_color = legacy->initial_color = legacy->color = cd->color;
  legacy->damage_source_player = player;
  legacy->damage_source_card = legacy_card;

  int csvid = cards_data[iid].id;

  legacy->display_pic_csv_id = csvid;
  int num_pics = cards_ptr[csvid]->num_pics;
  legacy->display_pic_num = num_pics <= 1 ? 0 : internal_rand(num_pics);

  legacy->damage_target_player = t_player;
  legacy->damage_target_card = t_card;

  if (t_player != -1 && t_card != -1)
  {
    card_instance_t *attached_to = get_card_instance(t_player, t_card);
    attached_to->regen_status |= KEYWORD_RECALC_ALL;
  }

  legacy->info_slot = (int)func_ptr;

  return legacy_card;
}
''',

'26a9520': r'''
void format_manacost_into_global_allpurpose_buffer_2022(int ignored, int *mana_cost_array,
                                                        int x_so_far, int max_x)
{

  char intro[300];
  intro[0] = 0;

  if (stack_size > 0)

    EXE_FN(void, 0x436980, char *, int, int,
           int)(intro, stack_data[stack_size - 1].generating_event,
                stack_cards[stack_size - 1].player, stack_cards[stack_size - 1].card);

  char str[100];
  str[0] = 0;
  char *p = str;

  if (mana_cost_array[COLOR_COLORLESS] == -1 || mana_cost_array[COLOR_ARTIFACT] == -1)
  {

    int our_xx = hack_xx;
    const char *cost;
    if (hack_xx == 1 && stack_size > 0 &&
        stack_data[stack_size - 1].generating_event == EVENT_RESOLVE_SPELL &&
        (cost =
             cards_ptr[get_id(stack_cards[stack_size - 1].player, stack_cards[stack_size - 1].card)]
                 ->mana_cost_text) &&
        strstr(cost, "|X|X"))
    {
      our_xx = 2;
      if (strstr(cost, "|X|X|X"))
        our_xx = 3;
    }

    if (override_xx != 0)
      our_xx = override_xx;

    int divisor = our_xx == 2 ? 2 : our_xx == 3 ? 3 : 1;
    const char *fmt = divisor == 1 ? "|X" : divisor == 2 ? "|X|X" : "|X|X|X";

    if (max_x == -1)
      p += sprintf(p, EXE_STR(0x728578), fmt, x_so_far / divisor);
    else if (max_x > x_so_far)
      p += sprintf(p, EXE_STR(0x62853C), fmt, x_so_far / divisor, max_x / divisor);
  }
  else if (mana_cost_array[COLOR_COLORLESS] || mana_cost_array[COLOR_ARTIFACT])
  {
    int generic = mana_cost_array[COLOR_COLORLESS] + mana_cost_array[COLOR_ARTIFACT];
    if (generic > 20)
      while (generic >= 10)
      {
        p += sprintf(p, "|10");
        generic -= 10;
      }

    if (generic)
      p += sprintf(p, "|%d", generic);
  }

  char mana_symbol[3] = {'|', 'B', 0};
  const char *mana_colors = "XBUGRW";
  int i, j;
  for (i = COLOR_BLACK; i <= COLOR_WHITE; ++i)
    if (mana_cost_array[i])
    {
      mana_symbol[1] = mana_colors[i];

      if (mana_cost_array[i] == -1)
      {
        if (max_x == -1)
          p += sprintf(p, EXE_STR(0x728578), mana_symbol, x_so_far);
        else if (max_x > x_so_far)
          p += sprintf(p, EXE_STR(0x62853C), mana_symbol, x_so_far, max_x);
      }
      else
        for (j = 0; mana_cost_array[i] > j; ++j)
          p += sprintf(p, "%s", mana_symbol);
    }

  p = (char *)(0x60a690);
  p += sprintf(p, "%s, ", intro);
  sprintf(p, EXE_STR(0x786B00), str);
}
''',

'26a98c0': r'''
int autotap_mana_source_2022(int player, int card)
{
  card_instance_t *inst = get_card_instance(player, card);
  if (inst->state & STATE_TAPPED)
    return 0;

  if ((inst->state & STATE_SUMMON_SICK) && is_what(player, card, TYPE_CREATURE))
    return 0;

  int old_land_can_be_played = land_can_be_played;
  land_can_be_played |= LCBP_CHARGING_MANA;

  int old_pay_mana[8], old_x_value, old_max_x_value;

  int *old_charge_mana_addr_of_x_value = charge_mana_addr_of_x_value;
  charge_mana_addr_of_x_value = &old_x_value;
  int (*old_charge_mana_addr_of_pay_mana_xbugrw)[8] = charge_mana_addr_of_pay_mana_xbugrw;
  charge_mana_addr_of_pay_mana_xbugrw = &old_pay_mana;
  int *old_charge_mana_addr_of_max_x_value = charge_mana_addr_of_max_x_value;
  charge_mana_addr_of_max_x_value = &old_max_x_value;

  old_x_value = x_value;
  x_value = 0;
  old_max_x_value = max_x_value;
  max_x_value = -1;

  int c;
  for (c = 0; c < 8; ++c)
  {
    old_pay_mana[c] = pay_mana_xbugrwaU[c];
    pay_mana_xbugrwaU[c] = 0;
  }

  if (stack_size < 32)
    put_card_or_activation_onto_stack_impl_2022(player, card, EVENT_RESOLVE_ACTIVATION, player, 0);
  needed_mana_colors = payment_calculate_needed_mana_colors(old_pay_mana);
  tapped_for_mana_color = -1;
  int was_tapped = inst->state & STATE_TAPPED;

  event_activate_then_duplicate_into_stack(player, card, EVENT_ACTIVATE, 1 - player, -1);

  needed_mana_colors = 0;

  int rval;
  if (cancel == 1)
  {
    cancel = 0;
    obliterate_top_card_of_stack();
    rval = 0;
  }
  else
  {
    land_can_be_played &= ~LCBP_CHARGING_MANA;

    if (!was_tapped && (inst->state & STATE_TAPPED))
    {
      dispatch_event(player, card, EVENT_TAP_CARD);
      dispatch_trigger2(player, TRIGGER_TAP_CARD, EXE_STR(0x78fba8), 1, player, card);
    }
    play_sound_effect(WAV_TAP);
    payment_resolve_top_card_on_stack();

    rval = 1;
  }

  charge_mana_addr_of_x_value = old_charge_mana_addr_of_x_value;
  charge_mana_addr_of_pay_mana_xbugrw = old_charge_mana_addr_of_pay_mana_xbugrw;
  charge_mana_addr_of_max_x_value = old_charge_mana_addr_of_max_x_value;

  x_value = old_x_value;
  max_x_value = old_max_x_value;

  for (c = 0; c < 8; ++c)
    pay_mana_xbugrwaU[c] = old_pay_mana[c];

  land_can_be_played = old_land_can_be_played;

  return rval;
}
''',

'26a9e40': r'''
void recalculate_all_cards_in_play_2022(void)
{
  memset(card_counts, 0, sizeof(card_counts));
  player_bits[0] &= 0xf0c0000fu;
  player_bits[1] &= 0xf0c0000fu;
  memset(card_flags, 0, sizeof(card_flags));
  memset(add_to_die_rolls, 0, sizeof(add_to_die_rolls));
  memset(roll_extra_dice, 0, sizeof(roll_extra_dice));

  for (int i = 0; i < 15; ++i)
  {
    added_subtypes_not_in_play[1][i].subtype = SUBTYPE_INVALID;
    added_subtypes_not_in_play[0][i].subtype = SUBTYPE_INVALID;
    added_subtypes_not_in_play[1][i].added_forced = 0;
    added_subtypes_not_in_play[0][i].added_forced = 0;
  }

  minimize_nondraining_mana();

  int p, c;

  for (p = 0; p < 2; ++p)
  {
    card_instance_t *instance = get_card_instance(p, 0);
    for (c = 0; c < active_cards_count[p]; ++c, ++instance)
    {
      if (instance->internal_card_id == -1)
        continue;
      if ((instance->state & (STATE_OUBLIETTED | STATE_IN_PLAY)) != STATE_IN_PLAY)
      {
        reset_subtypes_from_instance_2022(instance, instance->internal_card_id);
        continue;
      }
      instance->regen_status |= KEYWORD_RECALC_CHANGE_TYPE;
      get_abilities(p, c, EVENT_CHANGE_TYPE, -1);
      instance->regen_status |= KEYWORD_RECALC_SET_COLOR | KEYWORD_RECALC_ABILITIES |
                                KEYWORD_RECALC_POWER | KEYWORD_RECALC_TOUGHNESS;
      get_abilities(p, c, EVENT_SET_COLOR, -1);
      get_abilities(p, c, EVENT_ABILITIES, -1);
    }
  }

  count_colors_of_lands_in_play_2022();

  count_mana_2022();

  player_bits[0] &= ~PB_HAND_REVEALED;
  player_bits[1] &= ~PB_HAND_REVEALED;

  card_instance_t *instance;
  for (p = 0; p < 2; ++p)
    for (c = 0; c < active_cards_count[p]; ++c)
      if ((instance = in_play(p, c)))
      {
        if (is_what(p, c, TYPE_CREATURE))
        {
          get_abilities(p, c, EVENT_TOUGHNESS, -1);
          if (instance->internal_card_id == -1)
            continue;
          get_abilities(p, c, EVENT_POWER, -1);
        }

        if (instance->counters > 0 && instance->counters_m1m1 > 0 &&
            is_what(p, c, TYPE_PERMANENT) &&
            !(instance->toughness <= 0 && is_what(p, c, TYPE_CREATURE)))
        {
          int n = MIN(instance->counters, instance->counters_m1m1);
          instance->counters -= n;
          instance->counters_m1m1 -= n;
        }

        dispatch_event_with_attacker_to_one_card(p, c, EVENT_STATIC_EFFECTS, 1 - p, -1);
      }

  if (ai_is_speculating == 0)
  {
    if (!(player_bits[0] & PB_HAND_REVEALED) != !(EXE_DWORD(0x628C24) & (1 << 0)))
    {
      if (player_bits[0] & PB_HAND_REVEALED)
        EXE_DWORD(0x628C24) |= 1 << 0;
      else
        EXE_DWORD(0x628C24) &= ~(1 << 0);
    }

    if (!(player_bits[1] & PB_HAND_REVEALED) != !(EXE_DWORD(0x628C24) & (1 << 1)))
    {
      if (player_bits[1] & PB_HAND_REVEALED)
        EXE_DWORD(0x628C24) |= 1 << 1;
      else
        EXE_DWORD(0x628C24) &= ~(1 << 1);

      EXE_FN(void, 0x47D2F0, void *)(EXE_PTR_VOID(0x786DC4));
    }
  }
}
''',

'2672860': r'''
void reset_subtypes_from_instance_2022(card_instance_t *inst, iid_t iid)
{
  card_ptr_t *cp = cards_ptr[cards_data[iid].id];
  for (int i = 0; i < 7; ++i)
  {
    int16_t st = cp->types[i];
    if (st < 0)
      st = 0;
    if ((st >= 0x1000 && st < 0x100a && ((1u << (st & 31)) & 0x35fu)) || st == 0x2005)
      st = 0;
    inst->ext->subtype[i] = st;
  }
  for (int i = 7; i < 12; ++i)
    inst->ext->subtype[i] = 0;
}
''',

}
