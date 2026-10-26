"""Typed reconstructions of complete AI snapshots, save supplement and support helpers."""

ITEMS = [('card_ext', 57600, True), ('graveyard_source', 4000, True), ('next_graveyard_source_id', 4, False), ('turn_count', 4, False), ('always_prompt_for_color', 1, False), ('mulligans_complete', 8, True), ('challenge_round', 4, False), ('challenge1', 4, False), ('challenge2', 4, False), ('challenge3', 4, False), ('challenge4', 4, False), ('counters_added', 4, False), ('counters_added_type', 4, False), ('mana_colors_added', 8, True), ('mana_color_override', 64, True), ('mana_quantity_override', 64, True), ('multi_mana_transmutation', 16, True), ('card_counts', 14, True), ('card_flags', 8, True), ('your_spells_cant_be_countered_by_color', 2, True), ('roll_extra_dice', 8, True), ('add_to_die_rolls', 8, True), ('added_subtypes_not_in_play', 256, True)]

BODIES = {
'26b1700': r'''
void backup_data_for_ai_2022(void)
{
  memcpy_suppress_size_warning((void *)0x50be70, &must_attack, 0x1a740);
  STATIC_ASSERT(sizeof(card_data_t) == 72, sizeof_card_data_t_must_be_72);
  memcpy((void *)0x5265b0, &cards_data[EXE_DWORD(0x7a5380)], 16 * sizeof(card_data_t));
  EXE_DWORD(0x5b9b3c) = ai_modifier;
  memcpy(backup_card_ext, card_ext, sizeof(card_ext));
  memcpy(backup_graveyard_source, graveyard_source, sizeof(graveyard_source));
  backup_counters_added_type = counters_added_type;
  backup_next_graveyard_source_id = next_graveyard_source_id;
  backup_turn_count = turn_count;
  backup_always_prompt_for_color = always_prompt_for_color;
  memcpy(backup_mulligans_complete, mulligans_complete, sizeof(mulligans_complete));
  backup_challenge_round = challenge_round;
  backup_challenge1 = challenge1;
  backup_challenge2 = challenge2;
  backup_challenge3 = challenge3;
  backup_challenge4 = challenge4;
  backup_counters_added = counters_added;
  memcpy(backup_mana_colors_added, mana_colors_added, sizeof(mana_colors_added));
  memcpy(backup_mana_color_override, mana_color_override, sizeof(mana_color_override));
  memcpy(backup_mana_quantity_override, mana_quantity_override, sizeof(mana_quantity_override));
  memcpy(backup_multi_mana_transmutation, multi_mana_transmutation,
         sizeof(multi_mana_transmutation));
  memcpy(backup_card_counts, card_counts, sizeof(card_counts));
  memcpy(backup_card_flags, card_flags, sizeof(card_flags));
  memcpy(backup_your_spells_cant_be_countered_by_color, your_spells_cant_be_countered_by_color,
         sizeof(your_spells_cant_be_countered_by_color));
  memcpy(backup_roll_extra_dice, roll_extra_dice, sizeof(roll_extra_dice));
  memcpy(backup_add_to_die_rolls, add_to_die_rolls, sizeof(add_to_die_rolls));
  memcpy(backup_added_subtypes_not_in_play, added_subtypes_not_in_play,
         sizeof(added_subtypes_not_in_play));
}
''',

'26b1900': r'''
void restore_data_for_ai_2022(void)
{
  memcpy_suppress_size_warning(&must_attack, (void *)0x50be70, 0x1a740);
  STATIC_ASSERT(sizeof(card_data_t) == 72, sizeof_card_data_t_must_be_72);
  memcpy(&cards_data[EXE_DWORD(0x7a5380)], (void *)0x5265b0, 16 * sizeof(card_data_t));
  ai_modifier = EXE_DWORD(0x5b9b3c);
  memcpy(card_ext, backup_card_ext, sizeof(card_ext));
  memcpy(graveyard_source, backup_graveyard_source, sizeof(graveyard_source));
  counters_added_type = backup_counters_added_type;
  next_graveyard_source_id = backup_next_graveyard_source_id;
  turn_count = backup_turn_count;
  always_prompt_for_color = backup_always_prompt_for_color;
  memcpy(mulligans_complete, backup_mulligans_complete, sizeof(mulligans_complete));
  challenge_round = backup_challenge_round;
  challenge1 = backup_challenge1;
  challenge2 = backup_challenge2;
  challenge3 = backup_challenge3;
  challenge4 = backup_challenge4;
  counters_added = backup_counters_added;
  memcpy(mana_colors_added, backup_mana_colors_added, sizeof(mana_colors_added));
  memcpy(mana_color_override, backup_mana_color_override, sizeof(mana_color_override));
  memcpy(mana_quantity_override, backup_mana_quantity_override, sizeof(mana_quantity_override));
  memcpy(multi_mana_transmutation, backup_multi_mana_transmutation,
         sizeof(multi_mana_transmutation));
  memcpy(card_counts, backup_card_counts, sizeof(card_counts));
  memcpy(card_flags, backup_card_flags, sizeof(card_flags));
  memcpy(your_spells_cant_be_countered_by_color, backup_your_spells_cant_be_countered_by_color,
         sizeof(your_spells_cant_be_countered_by_color));
  memcpy(roll_extra_dice, backup_roll_extra_dice, sizeof(roll_extra_dice));
  memcpy(add_to_die_rolls, backup_add_to_die_rolls, sizeof(add_to_die_rolls));
  memcpy(added_subtypes_not_in_play, backup_added_subtypes_not_in_play,
         sizeof(added_subtypes_not_in_play));
}
''',

'26b1b00': r'''
void backup_data_for_ai_0_2022(void)
{
  memcpy_suppress_size_warning((void *)0x526a30, &must_attack, 0x1cd00);
  EXE_DWORD(0x5bbb44) = 0;
  STATIC_ASSERT(sizeof(card_data_t) == 72, sizeof_card_data_t_must_be_72);
  memcpy((void *)0x543730, &cards_data[EXE_DWORD(0x7a5380)], 16 * sizeof(card_data_t));
  if (stack_size < 0)
    backtrace_and_abort("functions/ai.c:247\nstack_size >= 0");
  memcpy(backup0_card_ext, card_ext, sizeof(card_ext));
  memcpy(backup0_graveyard_source, graveyard_source, sizeof(graveyard_source));
  backup0_counters_added_type = counters_added_type;
  backup0_next_graveyard_source_id = next_graveyard_source_id;
  backup0_turn_count = turn_count;
  backup0_always_prompt_for_color = always_prompt_for_color;
  memcpy(backup0_mulligans_complete, mulligans_complete, sizeof(mulligans_complete));
  backup0_challenge_round = challenge_round;
  backup0_challenge1 = challenge1;
  backup0_challenge2 = challenge2;
  backup0_challenge3 = challenge3;
  backup0_challenge4 = challenge4;
  backup0_counters_added = counters_added;
  memcpy(backup0_mana_colors_added, mana_colors_added, sizeof(mana_colors_added));
  memcpy(backup0_mana_color_override, mana_color_override, sizeof(mana_color_override));
  memcpy(backup0_mana_quantity_override, mana_quantity_override, sizeof(mana_quantity_override));
  memcpy(backup0_multi_mana_transmutation, multi_mana_transmutation,
         sizeof(multi_mana_transmutation));
  memcpy(backup0_card_counts, card_counts, sizeof(card_counts));
  memcpy(backup0_card_flags, card_flags, sizeof(card_flags));
  memcpy(backup0_your_spells_cant_be_countered_by_color, your_spells_cant_be_countered_by_color,
         sizeof(your_spells_cant_be_countered_by_color));
  memcpy(backup0_roll_extra_dice, roll_extra_dice, sizeof(roll_extra_dice));
  memcpy(backup0_add_to_die_rolls, add_to_die_rolls, sizeof(add_to_die_rolls));
  memcpy(backup0_added_subtypes_not_in_play, added_subtypes_not_in_play,
         sizeof(added_subtypes_not_in_play));
}
''',

'26b1d20': r'''
void restore_data_for_ai_0_2022(void)
{
  memcpy_suppress_size_warning(&must_attack, (void *)0x526a30, 0x1cd00);
  STATIC_ASSERT(sizeof(card_data_t) == 72, sizeof_card_data_t_must_be_72);
  memcpy(&cards_data[EXE_DWORD(0x7a5380)], (void *)0x543730, 16 * sizeof(card_data_t));
  memcpy(card_ext, backup0_card_ext, sizeof(card_ext));
  memcpy(graveyard_source, backup0_graveyard_source, sizeof(graveyard_source));
  counters_added_type = backup0_counters_added_type;
  next_graveyard_source_id = backup0_next_graveyard_source_id;
  turn_count = backup0_turn_count;
  always_prompt_for_color = backup0_always_prompt_for_color;
  memcpy(mulligans_complete, backup0_mulligans_complete, sizeof(mulligans_complete));
  challenge_round = backup0_challenge_round;
  challenge1 = backup0_challenge1;
  challenge2 = backup0_challenge2;
  challenge3 = backup0_challenge3;
  challenge4 = backup0_challenge4;
  counters_added = backup0_counters_added;
  memcpy(mana_colors_added, backup0_mana_colors_added, sizeof(mana_colors_added));
  memcpy(mana_color_override, backup0_mana_color_override, sizeof(mana_color_override));
  memcpy(mana_quantity_override, backup0_mana_quantity_override, sizeof(mana_quantity_override));
  memcpy(multi_mana_transmutation, backup0_multi_mana_transmutation,
         sizeof(multi_mana_transmutation));
  memcpy(card_counts, backup0_card_counts, sizeof(card_counts));
  memcpy(card_flags, backup0_card_flags, sizeof(card_flags));
  memcpy(your_spells_cant_be_countered_by_color, backup0_your_spells_cant_be_countered_by_color,
         sizeof(your_spells_cant_be_countered_by_color));
  memcpy(roll_extra_dice, backup0_roll_extra_dice, sizeof(roll_extra_dice));
  memcpy(add_to_die_rolls, backup0_add_to_die_rolls, sizeof(add_to_die_rolls));
  memcpy(added_subtypes_not_in_play, backup0_added_subtypes_not_in_play,
         sizeof(added_subtypes_not_in_play));
}
''',

'26b1f10': r'''
int save_or_load_supplement_2022(int status)
{
  status &= save_or_load_data(card_ext, sizeof(card_ext));
  status &= save_or_load_data(graveyard_source, sizeof(graveyard_source));
  status &= save_or_load_data(&next_graveyard_source_id, sizeof(next_graveyard_source_id));
  status &= save_or_load_data(&turn_count, sizeof(turn_count));
  status &= save_or_load_data(&always_prompt_for_color, sizeof(always_prompt_for_color));
  status &= save_or_load_data(mulligans_complete, sizeof(mulligans_complete));
  status &= save_or_load_data(&challenge_round, sizeof(challenge_round));
  status &= save_or_load_data(&challenge1, sizeof(challenge1));
  status &= save_or_load_data(&challenge2, sizeof(challenge2));
  status &= save_or_load_data(&challenge3, sizeof(challenge3));
  status &= save_or_load_data(&challenge4, sizeof(challenge4));
  status &= save_or_load_data(&counters_added, sizeof(counters_added));
  status &= save_or_load_data(&counters_added_type, sizeof(counters_added_type));
  status &= save_or_load_data(mana_colors_added, sizeof(mana_colors_added));
  status &= save_or_load_data(mana_color_override, sizeof(mana_color_override));
  status &= save_or_load_data(mana_quantity_override, sizeof(mana_quantity_override));
  status &= save_or_load_data(multi_mana_transmutation, sizeof(multi_mana_transmutation));
  status &= save_or_load_data(card_counts, sizeof(card_counts));
  status &= save_or_load_data(card_flags, sizeof(card_flags));
  status &= save_or_load_data(your_spells_cant_be_countered_by_color,
                              sizeof(your_spells_cant_be_countered_by_color));
  status &= save_or_load_data(roll_extra_dice, sizeof(roll_extra_dice));
  status &= save_or_load_data(add_to_die_rolls, sizeof(add_to_die_rolls));
  status &= save_or_load_data(added_subtypes_not_in_play, sizeof(added_subtypes_not_in_play));
  if (EXE_DWORD(0x5bc5f8))
    --turn_count;
  return status;
}
''',

'26b1500': r'''
void human_autotap_for_mana_2022(int player, int *amt, int *unknown_v46, int unknown_v47)
{

  if (!pay_mana_maximally_satisfied(&PAY_MANA_COLORLESS, x_value, max_x_value))
  {
    try_to_pay_for_mana_by_autotapping(player, amt, unknown_v46,
                                       AUTOTAP_NO_CREATURES | AUTOTAP_NO_ARTIFACTS |
                                           AUTOTAP_NO_DONT_AUTO_TAP | AUTOTAP_NO_NONBASIC_LANDS,
                                       unknown_v47);
  }

  if (!pay_mana_maximally_satisfied(&PAY_MANA_COLORLESS, x_value, max_x_value))
  {
    try_to_pay_for_mana_by_autotapping(
        player, amt, unknown_v46,
        AUTOTAP_NO_CREATURES | AUTOTAP_NO_ARTIFACTS | AUTOTAP_NO_DONT_AUTO_TAP, unknown_v47);
  }

  if ((player != 1 || (trace_mode & 2)) && ai_is_speculating != 1)
  {
    if (get_setting(SETTING_AUTOTAP_ARTIFACTS))
    {

      if (!pay_mana_maximally_satisfied(&PAY_MANA_COLORLESS, x_value, max_x_value))
      {
        try_to_pay_for_mana_by_autotapping(
            player, amt, unknown_v46, AUTOTAP_NO_CREATURES | AUTOTAP_NO_DONT_AUTO_TAP, unknown_v47);
      }

      if (get_setting(SETTING_AUTOTAP_CREATURES))
      {

        if (!pay_mana_maximally_satisfied(&PAY_MANA_COLORLESS, x_value, max_x_value))
        {
          try_to_pay_for_mana_by_autotapping(player, amt, unknown_v46, AUTOTAP_NO_DONT_AUTO_TAP,
                                             unknown_v47);
        }
      }
    }
    else if (get_setting(SETTING_AUTOTAP_CREATURES))
    {

      if (!pay_mana_maximally_satisfied(&PAY_MANA_COLORLESS, x_value, max_x_value))
      {
        try_to_pay_for_mana_by_autotapping(
            player, amt, unknown_v46, AUTOTAP_NO_ARTIFACTS | AUTOTAP_NO_DONT_AUTO_TAP, unknown_v47);
      }
    }
  }
}
''',

'26b1100': r'''
int check_destroys_if_blocked_oneside_2022(int lethal_player, int lethal_card, int other_player,
                                           int other_card)
{
  card_instance_t *instance = get_card_instance(lethal_player, lethal_card);
  int lethal = instance->destroys_if_blocked;
  int result = 0;
  if (lethal)
  {
    if ((lethal & DIFB_DESTROYS_ALL) == DIFB_DESTROYS_ALL)
      result = 1;
    else if ((lethal & DIFB_DESTROYS_WALLS) && has_subtype(other_player, other_card, SUBTYPE_WALL))
      result = 1;
    else if ((lethal & DIFB_DESTROYS_NONWALLS) &&
             !has_subtype(other_player, other_card, SUBTYPE_WALL))
      result = 1;

    if (result != 1 && (lethal & DIFB_DESTROYS_UNPROTECTED) &&
        !is_protected_from(other_player, other_card, lethal_player, lethal_card, IPF2022_DAMAGE))
      result = 1;

    if (lethal & (DIFB_ASK_CARD | DIFB_ASK_ALL_CARDS))
    {
      if (lethal & DIFB_ASK_ALL_CARDS)
        result |= dispatch_event_with_attacker(
            lethal_player, lethal_card, EVENT_CHECK_DESTROY_IF_BLOCKED, other_player, other_card);
      else
        result |= dispatch_event_with_attacker_to_one_card(
            lethal_player, lethal_card, EVENT_CHECK_DESTROY_IF_BLOCKED, other_player, other_card);
    }
  }
  return result;
}
''',

'26b2140': r'''
int check_destroys_if_blocked_2022(int attack_player, int attack_card, int defend_player,
                                   int defend_card)
{
  int result1 = check_destroys_if_blocked_oneside_2022(attack_player, attack_card, defend_player,
                                                       defend_card);

  int result2 = check_destroys_if_blocked_oneside_2022(defend_player, defend_card, attack_player,
                                                       attack_card);

  result2 = ((result2 & 1) << 1) | ((result2 & 2) >> 1);

  return result1 | result2;
}
''',

}
