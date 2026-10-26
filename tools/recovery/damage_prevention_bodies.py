"""Typed October 2022 damage prevention and Fog effects."""

BODIES = {
'2679080': r'''
void prevent_all_my_damage_impl_2022(int player, int card, int flags)
{
  card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
  if (damage->internal_card_id == damage_card && damage->info_slot > 0 &&
      damage->damage_source_card == card && damage->damage_source_player == player)
  {
    int combat = 1;
    if (flags & DDBM_MUST_BE_COMBAT_DAMAGE)
      combat = (damage->token_status & (STATUS_COMBAT_DAMAGE | STATUS_FIRST_STRIKE_DAMAGE)) != 0;
    if ((!(flags & DDBM_MUST_DAMAGE_PLAYER) ||
         (damage->damage_target_card == -1 &&
          !check_special_flags_2022(damage->damage_source_player, damage->damage_source_card,
                                    SF_ATTACKING_PWALKER))) &&
        (!(flags & DDBM_MUST_DAMAGE_OPPONENT) ||
         (damage->damage_target_card == -1 && damage->damage_target_player == 1 - player)) && combat)
      get_card_instance(affected_card_controller, affected_card)->info_slot = 0;
  }
}
''',
'2679180': r'''
int prevent_all_damage_dealt_by_target_legacy_2022(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->damage_target_player >= 0 && event == EVENT_PREVENT_DAMAGE)
  {
    prevent_all_my_damage_impl_2022(instance->damage_target_player, instance->damage_target_card,
                                   instance->targets[1].player);
    return 0;
  }
  if (event == EVENT_CLEANUP)
    kill_card(player, card, KILL_EXILE);
  return 0;
}
''',
'2679240': r'''
int no_combat_damage_this_turn_2022(int player, int card, event_t event)
{
  card_instance_t *damage = combat_damage_being_prevented(event);
  if (damage)
  {
    card_instance_t *instance = get_card_instance(player, card);
    if (damage->damage_source_card == instance->targets[0].card &&
        damage->damage_source_player == instance->targets[0].player)
      damage->info_slot = 0;
  }
  else if (event == EVENT_CLEANUP)
    kill_card(player, card, KILL_EXILE);
  return 0;
}
''',
'26813f0': r'''
void negate_combat_damage_this_turn_2022(int player, int card, int t_player, int t_card,
                                       int alternate_legacy_text_number)
{
  int legacy = create_targetted_legacy_effect(player, card, no_combat_damage_this_turn_2022,
                                             t_player, t_card);
  if (legacy >= 0 && alternate_legacy_text_number > 0)
    alternate_legacy_text_2022_impl(alternate_legacy_text_number, player, legacy);
}
''',
'2683470': r'''
int damage_cannot_be_prevented_until_eot_legacy_2022(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card)
      get_card_instance(affected_card_controller, affected_card)->state |= STATE_CANNOT_TARGET;
  }
  if (eot_trigger(player, card, event))
    kill_card(player, card, KILL_EXILE);
  return 0;
}
''',
'2683660': r'''
int no_damage_this_turn_2022(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->info_slot > 0 &&
        damage->damage_source_player == instance->targets[0].player &&
        damage->damage_source_card == instance->targets[0].card)
      get_card_instance(affected_card_controller, affected_card)->info_slot = 0;
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    if (card == affected_card && player == affected_card_controller &&
        instance->damage_target_player >= 0 && (land_can_be_played & LCBP_PENDING_DAMAGE_CARDS))
    {
      instance->damage_target_card = -1;
      instance->damage_target_player = -1;
      event_result = 1;
    }
  }
  else if (event == EVENT_AFTER_DAMAGE && instance->damage_target_player == -1 &&
           !(land_can_be_played & LCBP_PENDING_DAMAGE_CARDS))
    kill_card(player, card, KILL_EXILE);
  if (eot_trigger(player, card, event))
    kill_card(player, card, KILL_EXILE);
  return 0;
}
''',
'26837c0': r'''
int prevent_all_damage_to_target_legacy_2022(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->info_slot > 0 &&
        damage->damage_target_card == instance->targets[0].card &&
        damage->damage_target_player == instance->targets[0].player)
    {
      int good = instance->targets[1].player < 0 ||
                 (damage->damage_source_card == instance->targets[1].card &&
                  damage->damage_source_player == instance->targets[1].player);
      if (instance->targets[2].player > 1)
      {
        int color = in_play(damage->damage_source_player, damage->damage_source_card)
                        ? get_color_2022(damage->damage_source_player, damage->damage_source_card)
                        : damage->initial_color;
        if (!(color & instance->targets[2].player))
          good = 0;
      }
      if (good)
        get_card_instance(affected_card_controller, affected_card)->info_slot = 0;
    }
  }
  if (instance->targets[2].player > 0 && (instance->targets[2].player & 1) &&
      eot_trigger(player, card, event))
    kill_card(player, card, KILL_EXILE);
  return 0;
}
''',
'2685200': r'''
int my_damage_cannot_be_prevented_2022(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *instance = get_card_instance(player, card);
    int source_player = is_what_2022(player, card, TYPE_EFFECT) ? instance->targets[0].player : player;
    int source_card = is_what_2022(player, card, TYPE_EFFECT) ? instance->targets[0].card : card;
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->damage_source_player == source_player &&
        damage->damage_source_card == source_card)
      get_card_instance(affected_card_controller, affected_card)->state |= STATE_CANNOT_TARGET;
  }
  else if (event == EVENT_CLEANUP && is_what_2022(player, card, TYPE_EFFECT))
    kill_card(player, card, KILL_EXILE);
  return 0;
}
''',
'2686f00': r'''
int fog_effect_legacy_2022(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *instance = get_card_instance(player, card);
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->info_slot > 0 &&
        !(get_card_instance(affected_card_controller, affected_card)->state & STATE_CANNOT_TARGET))
    {
      if (instance->targets[2].player >= 0 && instance->targets[2].card >= 0 &&
          instance->targets[2].player == damage->damage_source_player &&
          instance->targets[2].card == damage->damage_source_card)
        return 0;
      fog_special_flags_t flags = instance->targets[0].card;
      if ((flags & FOG_BY_ANY_TYPE) ||
          is_what_2022(damage->damage_source_player, damage->damage_source_card, TYPE_CREATURE))
      {
        if (instance->targets[0].player != ANYBODY &&
            instance->targets[0].player != ((flags & FOG_BY_CONTROLLED_BY_PLAYER)
                                               ? damage->damage_source_player
                                               : damage->damage_target_player))
          return 0;
        if ((flags & FOG_COMBAT_DAMAGE_ONLY) &&
            !(damage->token_status & (STATUS_COMBAT_DAMAGE | STATUS_FIRST_STRIKE_DAMAGE)))
          return 0;
        if ((flags & FOG_ONLY_DAMAGE_TO_T_PLAYER) &&
            (damage->damage_target_card != -1 || damage_is_to_planeswalker(damage)))
          return 0;
        if ((flags & FOG_UNBLOCKED_CREATURES_ONLY) &&
            !is_unblocked(damage->damage_source_player, damage->damage_source_card))
          return 0;
        if ((flags & FOG_CREATURES_WITHOUT_TRAMPLE) &&
            check_for_ability(damage->damage_source_player, damage->damage_source_card, KEYWORD_TRAMPLE))
          return 0;
        if (flags & FOG_COLOR_MASK)
        {
          int color = damage->initial_color;
          if (in_play(damage->damage_source_player, damage->damage_source_card))
            color = get_color_2022(damage->damage_source_player, damage->damage_source_card);
          if (flags & FOG_INVERSE_COLOR_CHECK)
          {
            if (color & flags)
              return 0;
          }
          else if (!(color & flags))
            return 0;
        }
        if ((flags & FOG_BY_NON_SUBTYPE) && instance->targets[1].player > 0 &&
            has_subtype(damage->damage_source_player, damage->damage_source_card,
                        instance->targets[1].player))
          return 0;
        if (instance->display_pic_csv_id == 0x2efd &&
            (has_subtype(damage->damage_source_player, damage->damage_source_card, SUBTYPE_WOLF) ||
             has_subtype(damage->damage_source_player, damage->damage_source_card, SUBTYPE_WEREWOLF)))
          return 0;
        get_card_instance(affected_card_controller, affected_card)->info_slot = 0;
      }
    }
  }
  else if (event == EVENT_CLEANUP)
    kill_card(player, card, KILL_EXILE);
  return 0;
}
''',
'26a3f20': r'''
int prevent_damage_2022(int damage_player, int damage_card_index, int amount)
{
  card_instance_t *damage = get_card_instance(damage_player, damage_card_index);
  int original = damage->info_slot;
  if (amount != -1 && amount < original)
  {
    damage->info_slot = original - amount;
    return amount;
  }
  damage->info_slot = 0;
  return original;
}
''',
'26a3f70': r'''
int effect_prevent_the_next_n_damage_2022(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *instance = get_card_instance(player, card);
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card &&
        damage->damage_target_player == instance->targets[0].player &&
        damage->damage_target_card == instance->targets[0].card && damage->info_slot > 0 &&
        !(get_card_instance(affected_card_controller, affected_card)->state & STATE_CANNOT_TARGET))
    {
      int amount = damage->info_slot;
      if (!(instance->targets[2].player & PREVENT_INFINITE) && instance->counter_toughness < amount)
        amount = instance->counter_toughness;
      if (amount > 0)
      {
        prevent_damage_2022(affected_card_controller, affected_card, amount);
        if (!(instance->targets[2].player & PREVENT_INFINITE))
          instance->counter_toughness -= amount;
        if ((instance->targets[2].player & PREVENT_ADD_1_1_COUNTER) && damage->damage_target_card >= 0)
          add_counters_from_source(player, card, damage->damage_target_player,
                                   damage->damage_target_card, COUNTER_P1_P1, amount);
        if (instance->targets[2].player & PREVENT_GAIN_LIFE)
          gain_life_impl_2022(player, amount, 0);
        if ((instance->targets[2].player & PREVENT_REDIRECT) && instance->targets[1].player >= 0 &&
            (instance->targets[1].card == -1 ||
             in_play(instance->targets[1].player, instance->targets[1].card)))
          damage_creature(instance->targets[1].player, instance->targets[1].card, amount,
                           damage->damage_source_player, damage->damage_source_card);
      }
      if (instance->counter_toughness <= 0 && !(instance->targets[2].player & PREVENT_INFINITE))
        kill_card(player, card, KILL_EXILE);
      else
      {
        unsigned char *low = (unsigned char *)&instance->untap_status;
        unsigned char value = *low & 0xfc;
        value = value < 0xfc ? value + 4 : 0;
        *low = (*low & 3) | value;
      }
    }
  }
  else if (event == EVENT_SET_LEGACY_EFFECT_NAME)
  {
    card_instance_t *instance = get_card_instance(player, card);
    snprintf(set_legacy_effect_name_addr, 51, "%s (%d)",
             cards_ptr[instance->display_pic_csv_id]->name, (int)instance->counter_toughness);
  }
  if (eot_trigger(player, card, event))
    kill_card(player, card, KILL_EXILE);
  return 0;
}
''',
'2687320': r'''
void prevent_the_next_n_damage_2022(int player, int card, int t_player, int t_card, int amount,
                                  prevent_t flags, int redir_to_player, int redir_to_card)
{
  if (amount <= 0 && !(flags & PREVENT_INFINITE))
    return;
  int legacy;
  int opponent = 0;
  if (t_card != -1)
    legacy = create_targetted_legacy_effect(player, card, effect_prevent_the_next_n_damage_2022,
                                           t_player, t_card);
  else if (player == t_player)
    legacy = create_legacy_effect(player, card, effect_prevent_the_next_n_damage_2022);
  else
  {
    legacy = create_legacy_effect_for_opponent(player, card, effect_prevent_the_next_n_damage_2022,
                                              -1, -1);
    player = 1 - player;
    opponent = 1;
  }
  if (legacy == -1)
    return;
  card_instance_t *instance = get_card_instance(player, legacy);
  instance->targets[0].player = t_player;
  instance->targets[0].card = t_card;
  instance->number_of_targets = 1;
  if (opponent)
    instance->state ^= STATE_OWNED_BY_OPPONENT;
  if (flags & PREVENT_REDIRECT)
  {
    if ((unsigned int)redir_to_player < 2U && (unsigned int)redir_to_card + 1U < 151U)
    {
      instance->number_of_targets = 2;
      instance->targets[1].player = redir_to_player;
      instance->targets[1].card = redir_to_card;
    }
    else
      flags &= ~PREVENT_REDIRECT;
  }
  instance->targets[2].player = flags;
  if (!(flags & PREVENT_INFINITE))
  {
    instance->eot_toughness = EVENT_SET_LEGACY_EFFECT_NAME;
    instance->counter_toughness = (int16_t)amount;
  }
  if (land_can_be_played & LCBP_DAMAGE_PREVENTION)
    for (int p = 0; p < 2; ++p)
      for (int c = 0; c < active_cards_count[p]; ++c)
      {
        card_instance_t *damage = in_play(p, c);
        if (damage && damage->internal_card_id == damage_card && !(damage->state & STATE_TAPPED))
        {
          dispatch_event_arbitrary_to_one_card(player, legacy, EVENT_PREVENT_DAMAGE, p, c, 1 - p, -1);
          if (instance->internal_card_id == -1)
            return;
        }
      }
}
''',
'26885d0': r'''
int fog_special2_2022(int player, int card, int t_player, fog_special_flags_t flags, int param)
{
  if (!(flags & FOG_ONLY_DAMAGE_TO_T_PLAYER))
  {
    int protect = ANYBODY;
    if (t_player != ANYBODY)
      protect = (flags & FOG_BY_CONTROLLED_BY_PLAYER) ? 1 - t_player : t_player;
    for (int p = 0; p < 2; ++p)
      if (protect == ANYBODY || protect == p)
        for (int c = 0; c < active_cards_count[p]; ++c)
          if (in_play(p, c) && is_planeswalker_2022(p, c))
            set_special_flags_2022(p, c, SF_WONT_RECEIVE_DAMAGE);
  }
  int legacy = create_legacy_effect(player, card, fog_effect_legacy_2022);
  card_instance_t *instance = get_card_instance(player, legacy);
  instance->targets[0].player = t_player;
  instance->targets[0].card = flags;
  instance->targets[1].player = param;
  instance->targets[1].card = get_id(player, card);
  return legacy;
}
''',
'2688700': r'''
int fog_special_2022(int player, int card, int t_player, int flags)
{
  return fog_special2_2022(player, card, t_player, flags, 0);
}
''',
'2688730': r'''
int fog_effect_2022(int player, int card)
{
  return fog_special2_2022(player, card, ANYBODY, FOG_COMBAT_DAMAGE_ONLY, 0);
}
''',
'2688d80': r'''
void negate_damage_this_turn_2022(int player, int card, int t_player, int t_card, int unused)
{
  create_targetted_legacy_effect(player, card, no_damage_this_turn_2022, t_player, t_card);
}
''',
'268eaf0': r'''
int prevent_all_damage_to_target_2022(int player, int card, int t_player, int t_card, int mode)
{
  int legacy;
  card_instance_t *instance;
  if (t_card == -1)
  {
    legacy = create_legacy_effect(player, card, prevent_all_damage_to_target_legacy_2022);
    instance = get_card_instance(player, legacy);
    instance->targets[0].player = t_player;
    instance->targets[0].card = -1;
  }
  else
  {
    legacy = create_targetted_legacy_effect(player, card, prevent_all_damage_to_target_legacy_2022,
                                           t_player, t_card);
    instance = get_card_instance(player, legacy);
  }
  instance->targets[2].player = mode;
  return legacy;
}
''',
'268ea80': r'''
int prevent_all_damage_2022(int player, int card, event_t event)
{
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->damage_target_card == card &&
        damage->damage_target_player == player && damage->info_slot > 0)
      get_card_instance(affected_card_controller, affected_card)->info_slot = 0;
  }
  return 0;
}
''',
'2695540': r'''
void damage_target_creature_or_player_2022(int t_player, int t_card, int amount,
                                         int source_player, int source_card)
{
  if (t_card != -1)
    damage_creature(t_player, t_card, amount, source_player, source_card);
  else
    damage_player(t_player, amount, source_player, source_card);
}
''',
'2696100': r'''
int damage_cannot_be_prevented_until_eot_2022(int player, int card)
{
  return create_legacy_effect(player, card, damage_cannot_be_prevented_until_eot_legacy_2022);
}
''',
'2697740': r'''
void prevent_all_my_damage_2022(int player, int card, event_t event, int flags)
{
  if (event == EVENT_PREVENT_DAMAGE)
    prevent_all_my_damage_impl_2022(player, card, flags);
}
''',
'2697760': r'''
int prevent_all_damage_dealt_by_target_2022(int player, int card, int t_player, int t_card,
                                         int flags)
{
  int legacy = create_targetted_legacy_effect(player, card,
                                             prevent_all_damage_dealt_by_target_legacy_2022,
                                             t_player, t_card);
  get_card_instance(player, legacy)->targets[1].player = flags;
  return legacy;
}
''',
'26a4230': r'''
int prevent_damage_until_eot_legacy_2022(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_PREVENT_DAMAGE)
  {
    card_instance_t *damage = get_card_instance(affected_card_controller, affected_card);
    if (damage->internal_card_id == damage_card && damage->info_slot > 0 &&
        damage->damage_target_player == instance->targets[0].player &&
        damage->damage_target_card == instance->targets[0].card)
    {
      int amount = damage->info_slot;
      int remaining = instance->targets[1].player;
      if (remaining != -1 && remaining < amount)
      {
        prevent_damage_2022(affected_card_controller, affected_card, remaining);
        instance->targets[1].player = 0;
      }
      else
      {
        if (remaining != -1)
        {
          instance->targets[1].player = remaining - amount;
          damage = get_card_instance(affected_card_controller, affected_card);
        }
        damage->info_slot = 0;
      }
    }
  }
  if (eot_trigger(player, card, event))
    kill_card(player, card, KILL_EXILE);
  return 0;
}
''',
'26a4350': r'''
int prevent_damage_until_eot_2022(int player, int card, int t_player, int t_card, int maximum)
{
  prevent_damage_2022(t_player, t_card, maximum);
  if (maximum != -1 && maximum <= 0)
    return -1;
  card_instance_t *damage = get_card_instance(t_player, t_card);
  int legacy = create_legacy_effect(player, card, prevent_damage_until_eot_legacy_2022);
  card_instance_t *instance = get_card_instance(player, legacy);
  instance->targets[0].player = damage->damage_source_player;
  instance->targets[0].card = damage->damage_source_card;
  instance->targets[1].player = maximum;
  instance->targets[2].card = get_id(player, card);
  return legacy;
}
''',
}
