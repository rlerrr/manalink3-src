"""Typed October 2022 type, mana-value, PT and battlefield queries."""

BODIES = {
'2681af0': r'''
int is_planeswalker_2022(int player, int card)
{

  if (player == -1)
  {
    card_data_t *card_d = &cards_data[card];
    if ((card_d->type & TYPE_ENCHANTMENT) && card_d->cc[2] == 9)
    {
      return 1;
    }
  }
  else
  {
    card_data_t *card_d = get_card_data(player, card);
    if ((card_d->type & TYPE_ENCHANTMENT) && card_d->cc[2] == 9)
    {
      return 1;
    }
  }

  return 0;
}
''',

'2681b50': r'''
type_t get_type_with_iid_2022(int player, int card, int iid)
{
  type_t typ = cards_data[iid].type;
  if ((typ & (TYPE_INSTANT | TYPE_INTERRUPT)) && (typ & (TYPE_PERMANENT | TYPE_SORCERY)))
    typ &= ~(TYPE_INSTANT | TYPE_INTERRUPT);
  if (typ & TYPE_INTERRUPT)
    typ |= TYPE_INSTANT;
  if (player == -1 || card == -1)
  {
    if ((cards_data[iid].type & TYPE_ENCHANTMENT) && cards_data[iid].cc[2] == 9)
      typ = (typ & ~TYPE_ENCHANTMENT) | TARGET_TYPE_PLANESWALKER;
  }
  else
  {
    if (is_planeswalker_2022(player, card) ||
        ((cards_data[iid].type & TYPE_ENCHANTMENT) && cards_data[iid].cc[2] == 9))
      typ = (typ & ~TYPE_ENCHANTMENT) | TARGET_TYPE_PLANESWALKER;
    if (typ & (TYPE_PERMANENT | TARGET_TYPE_PLANESWALKER))
    {
      int flags =
          check_special_flags2_2022(player, card, SF2_ENCHANTED_EVENING | SF2_MYCOSYNTH_LATTICE);
      if (flags & SF2_ENCHANTED_EVENING)
        typ |= TYPE_ENCHANTMENT;
      if (flags & SF2_MYCOSYNTH_LATTICE)
        typ |= TYPE_ARTIFACT;
    }
  }
  return typ;
}
''',

'2681c50': r'''
type_t get_type_2022(int player, int card)
{
  int iid;
  if (player < 0)
    iid = card;
  else
  {
    card_instance_t *instance = get_card_instance(player, card);
    iid = instance->internal_card_id;
    if (iid < 0)
      iid = instance->backup_internal_card_id;
  }
  return get_type_with_iid_2022(player, card, iid);
}
''',

'2681ca0': r'''
int is_what_2022(int player, int card, int test_type)
{

  int typ;
  if (player == -1)
  {
    typ = cards_data[card].type;
    if ((test_type & TYPE_CREATURE) && cards_data[card].id == 0x5a13)
      return 1;
  }
  else
  {
    typ = get_card_data(player, card)->type;
  }

  if ((test_type & TARGET_TYPE_PLANESWALKER) && is_planeswalker_2022(player, card))
  {
    return 1;
  }

  if (test_type & TYPE_ENCHANTMENT)
  {
    if (player != -1 && (typ & TYPE_PERMANENT) &&
        check_special_flags2(player, card, SF2_ENCHANTED_EVENING))
    {
      typ |= TYPE_ENCHANTMENT;
    }
    else if ((test_type & TYPE_PERMANENT) != TYPE_PERMANENT && is_planeswalker_2022(player, card))
    {
      return 0;
    }
  }

  if (test_type & TYPE_ARTIFACT)
  {
    if (player != -1 && (typ & TYPE_PERMANENT) &&
        check_special_flags2(player, card, SF2_MYCOSYNTH_LATTICE))
    {
      typ |= TYPE_ARTIFACT;
    }
  }

  if (typ & TYPE_INTERRUPT)
  {
    typ |= TYPE_INSTANT;
  }

  if ((typ & (TYPE_INSTANT | TYPE_INTERRUPT)) && (typ & (TYPE_PERMANENT | TYPE_SORCERY)))
  {
    typ &= ~(TYPE_INSTANT | TYPE_INTERRUPT);
  }

  return typ & test_type ? 1 : 0;
}
''',

'2677b70': r'''
int get_pt_for_creature_with_variable_pt_2022(int player, int iid, event_t event)
{
  if (cards_data[iid].id == 0x5a13) /* Grist, the Hunger Tide. */
    return 1;
  int curr = event == EVENT_POWER ? cards_data[iid].power : cards_data[iid].toughness;
  if ((curr > 0 && (curr & 0x4000)))
  {
    curr &= ~0x4000;

    int old_affected_card_controller = affected_card_controller;
    int old_affected_card = affected_card;
    int old_event_result = event_result;

    affected_card_controller = player;
    affected_card = -1;
    event_result = curr;
    call_card_fn(cards_data[iid].code_pointer, NULL, player, -1, event);
    curr = event_result;

    affected_card_controller = old_affected_card_controller;
    affected_card = old_affected_card;
    event_result = old_event_result;
  }
  return curr;
}
''',

'2696aa0': r'''
int get_base_power_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  int iid = instance->internal_card_id;
  if (iid == -1)
    iid = instance->backup_internal_card_id;
  return get_pt_for_creature_with_variable_pt_2022(player, iid, EVENT_POWER);
}
''',

'2696ae0': r'''
int get_base_power_iid_2022(int owner, int iid)
{
  return get_pt_for_creature_with_variable_pt_2022(owner, iid, EVENT_POWER);
}
''',

'2696b00': r'''
int get_base_toughness_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  int iid = instance->internal_card_id;
  if (iid == -1)
    iid = instance->backup_internal_card_id;
  return get_pt_for_creature_with_variable_pt_2022(player, iid, EVENT_TOUGHNESS);
}
''',

'2696b40': r'''
int get_base_toughness_iid_2022(int owner, int iid)
{
  return get_pt_for_creature_with_variable_pt_2022(owner, iid, EVENT_TOUGHNESS);
}
''',

'267ed20': r'''
int split_cards_cmc_table_by_id_2022(int csvid, int half_played)
{
  int first = split_cards_id_table(csvid, 1);
  if (first < 0)
    return -1;
  if (num_bits_set((unsigned int)half_played) - 1U <= 1U)
  {
    int selected = split_cards_id_table(csvid, half_played);
    return get_cmc_by_internal_id_2022(get_internal_card_id_from_csv_id(selected));
  }
  int result = get_cmc_by_internal_id_2022(get_internal_card_id_from_csv_id(first));
  int second = split_cards_id_table(csvid, 2);
  return result + get_cmc_by_internal_id_2022(get_internal_card_id_from_csv_id(second));
}
''',

'267ecb0': r'''
int get_cmc_by_internal_id_2022(int iid)
{
  int result = cards_data[iid].cc[0];
  if (cards_data[iid].cc[1] <= 16)
    result += cards_data[iid].cc[1];
  if (has_no_mana_cost(-1, iid))
    return 0;
  int split = split_cards_cmc_table_by_id_2022(cards_data[iid].id, 0);
  return split < 0 ? result : split;
}
''',

'2694100': r'''
int get_cmc_2022(int player, int card)
{
  if (player == -1)
    return get_cmc_by_internal_id_2022(card);
  card_instance_t *instance = get_card_instance(player, card);
  if (is_token(player, card))
    return 0;
  int iid = instance->internal_card_id;
  if (iid == -1)
    iid = instance->backup_internal_card_id;
  if (in_play(player, card) && check_special_flags3(player, card, SF3_CARD_IS_FLIPPED))
    iid = get_original_internal_card_id(player, card);
  int result = get_cmc_by_internal_id_2022(iid);
  if (is_x_spell(player, card) && (instance->state & STATE_INVISIBLE))
    result += instance->info_slot;
  int csvid = get_id(player, card);
  int half = get_card_instance(player, card)->info_slot;
  int split = split_cards_cmc_table_by_id_2022(csvid, half);
  if (split >= 0)
  {
    /* The native second X-spell query is made even though its return is unused. */
    is_x_spell(player, card);
    csvid = get_id(player, card);
    if ((csvid == CARD_ID_CRIME_PUNISHMENT && (instance->info_slot & 2)) ||
        (csvid == CARD_ID_SUPPLY_DEMAND && (instance->info_slot & 1)))
      split += instance->targets[1].card > 0 ? instance->targets[1].card : 0;
    return split;
  }
  return result;
}
''',

'2692070': r'''
int get_highest_power_2022(int player)
{
  int result = 0;
  for (int p = 0; p < 2; ++p)
    if (p == player || player == ANYBODY)
      for (int c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what_2022(p, c, TYPE_CREATURE))
        {
          int power = get_power(p, c);
          if (power > result)
            result = power;
        }
  return result;
}
''',

'2691ae0': r'''
int get_power_sum_2022(int player)
{

  int result = 0;
  int i;
  for (i = 0; i < 2; i++)
  {
    if (i == player || player == 2)
    {
      int count = 0;
      while (count < active_cards_count[i])
      {
        if (in_play(i, count) && is_what_2022(i, count, TYPE_CREATURE))
        {
          result += get_power(i, count);
        }
        count++;
      }
    }
  }

  return result;
}
''',

'2691a40': r'''
int count_defenders_2022(int player)
{
  int p, c, rval = 0;
  for (p = 0; p < 2; ++p)
    if (p == player || player == ANYBODY)
      for (c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what_2022(p, c, TYPE_CREATURE) &&
            check_for_ability(p, c, KEYWORD_DEFENDER))
          ++rval;

  return rval;
}
''',

'268c7a0': r'''
int count_creatures_by_toughness_2022(int player, int pow, int mode)
{

  int count = 0;
  int result = 0;
  while (count < active_cards_count[player])
  {
    if (in_play(player, count) && is_what_2022(player, count, TYPE_CREATURE))
    {
      int his_pow = get_toughness(player, count);
      if (mode == 0 && his_pow == pow)
      {
        result++;
      }
      if (mode == 1 && his_pow > pow)
      {
        result++;
      }
      if (mode == 2 && his_pow < pow)
      {
        result++;
      }
    }
    count++;
  }
  return result;
}
''',

'268c6e0': r'''
int count_creatures_by_power_2022(int player, int pow, int mode)
{

  int count = 0;
  int result = 0;
  while (count < active_cards_count[player])
  {
    if (in_play(player, count) && is_what_2022(player, count, TYPE_CREATURE))
    {
      int his_pow = get_power(player, count);
      if (mode == 0 && his_pow == pow)
      {
        result++;
      }
      if (mode == 1 && his_pow > pow)
      {
        result++;
      }
      if (mode == 2 && his_pow < pow)
      {
        result++;
      }
    }
    count++;
  }
  return result;
}
''',

'26944f0': r'''
int get_highest_cmc_nonland_2022(int player)
{
  int c, result = 0;
  for (c = 0; c < active_cards_count[player]; ++c)
    if (in_play(player, c) && is_what_2022(player, c, TYPE_PERMANENT) &&
        !is_what_2022(player, c, TYPE_LAND))
    {
      int cmc = get_cmc_2022(player, c);
      if (cmc > result)
        result = cmc;
    }

  return result;
}
''',

'2694440': r'''
int get_highest_cmc_2022(int player, type_t type)
{
  int p, c, result = 0;
  for (p = 0; p < 2; p++)
  {
    if (player == p || player == ANYBODY)
    {
      for (c = 0; c < active_cards_count[p]; ++c)
      {
        if (in_play(p, c) && is_what_2022(p, c, type))
        {
          int cmc = get_cmc_2022(p, c);
          if (cmc > result)
          {
            result = cmc;
          }
        }
      }
    }
  }
  return result;
}
''',

'2694390': r'''
int get_most_common_cmc_in_hand_2022(int player, type_t type)
{
  int c, cmc[17] = {0};

  for (c = 0; c < active_cards_count[player]; ++c)
    if (in_hand(player, c) && is_what_2022(player, c, type))
      cmc[get_cmc_2022(player, c)]++;

  int i, result = -1, par = 0;
  for (i = 0; i < 17; ++i)
    if (cmc[i] > par)
    {
      par = cmc[i];
      result = i;
    }

  return result;
}
''',

'26942c0': r'''
int get_most_common_cmc_nonland_2022(int player)
{
  int c, cmc[17] = {0};

  for (c = 0; c < active_cards_count[player]; ++c)
    if (in_play(player, c) && is_what_2022(player, c, TYPE_PERMANENT) &&
        !is_what_2022(player, c, TYPE_LAND))
      cmc[get_cmc_2022(player, c)]++;

  int i, result = -1, par = 0;
  for (i = 0; i < 17; ++i)
    if (cmc[i] > par)
    {
      par = cmc[i];
      result = i;
    }

  return result;
}
''',

'2683b50': r'''
int count_permanents_by_type_2022(int player, type_t type)
{
  int p, c, count = 0;
  for (p = 0; p <= 1; ++p)
    if (player == ANYBODY || player == p)
      for (c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what_2022(p, c, type) && !is_what_2022(p, c, TYPE_EFFECT))
          ++count;
  return count;
}
''',

'2683e60': r'''
int count_permanents_by_color_2022(int player, int selected_type, int selected_color)
{
  int result = 0;
  if (player == ANYBODY)
  {
    result = count_permanents_by_color_2022(0, selected_type, selected_color);
    player = 1;
  }
  for (int c = 0; c < active_cards_count[player]; ++c)
    if (is_what_2022(player, c, selected_type) && in_play(player, c) &&
        (get_color_2022(player, c) & selected_color))
      ++result;
  return result;
}
''',

'267bbb0': r'''
int count_cards_by_id_2022(int player, int id)
{
  int total = 0;
  int i;
  for (i = 0; i < 2; i++)
  {
    if (player == 2 || i == player)
    {
      int count = 0;
      while (count < active_cards_count[i])
      {
        if (in_play(i, count) && !is_humiliated_for_ability(i, count, AL_STATIC) &&
            get_id(i, count) == id)
        {
          total++;
        }
        count++;
      }
    }
  }
  return total;
}
''',

'2682d30': r'''
int check_battlefield_for_subtype_2022(int what_player, int type, int subtype)
{
  int p, c;
  for (p = 0; p <= 1; ++p)
    if (what_player == ANYBODY || what_player == p)
      for (c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what_2022(p, c, type) &&
            (subtype == -1 || has_subtype(p, c, subtype)))
          return 1;
  return 0;
}
''',

'26828d0': r'''
int count_subtype_2022(int what_player, int type, int subtype)
{
  int p, c, count = 0;
  for (p = 0; p <= 1; ++p)
    if (what_player == ANYBODY || what_player == p)
      for (c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what_2022(p, c, type) && !is_what_2022(p, c, TYPE_EFFECT) &&
            (subtype == -1 || has_subtype(p, c, subtype)))
          ++count;
  return count;
}
''',

'267a900': r'''
int count_shrines_2022(int player, int card)
{
  target_definition_2022_t td;
  default_target_definition(player, card, &td, TYPE_ENCHANTMENT);
  td.required_subtype = SUBTYPE_SHRINE;
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  return target_available(player, card, &td);
}
''',

}
