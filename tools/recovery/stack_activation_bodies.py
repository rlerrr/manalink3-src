"""Typed reconstructions of native stack proxies, finalization and mana activation."""

BODIES = {
'26a8540': r'''
void put_card_or_activation_onto_stack_impl_2022(int player, int card, event_t event, int a4,
                                                 int a5)
{

  card_instance_t *instance = get_card_instance(player, card);

  stack_data[stack_size].internal_card_id = instance->internal_card_id;
  stack_data[stack_size].generating_event = event;
  stack_data[stack_size].unknown_sdt = a4;

  int stack_card;
  if (event == EVENT_RESOLVE_SPELL || event == EVENT_RESOLVE_TRIGGER ||
      instance->internal_card_id <= 4)
    stack_card = card;
  else
  {
    stack_card = add_card_to_hand(player, activation_card);
    if (stack_card == -1)
      return;
    card_instance_t *stack_inst = get_card_instance(player, stack_card);
    int orig_timestamp = stack_inst->timestamp;
    memcpy_inst_and_ext(stack_inst, instance);
    stack_inst->internal_card_id = activation_card;
    stack_inst->unknown0x14 = 0;
    stack_inst->kill_code = 0;
    if (instance->internal_card_id != -1)
      stack_inst->original_internal_card_id = instance->internal_card_id;

    else
      stack_inst->original_internal_card_id = instance->backup_internal_card_id;

    stack_inst->state |= STATE_IN_PLAY;
    --hand_count[player];
    stack_inst->parent_controller = player;
    stack_inst->parent_card = card;
    stack_inst->timestamp = orig_timestamp;

    stack_inst->token_status &= ~STATUS_DYING;
  }

  stack_cards[stack_size].player = player;
  stack_cards[stack_size].card = stack_card;

  stack_damage_targets[stack_size].player = instance->damage_target_player;
  stack_damage_targets[stack_size].card = instance->damage_target_card;

  if (trigger_condition == (trigger_t)(-1))
    stack_phase[stack_size] = current_phase;
  else
    stack_phase[stack_size] = trigger_condition;

  if (ai_is_speculating != 1)
    EXE_DWORD_PTR(0x628674)[stack_size] = a5;

  ++stack_size;
  stack_cards[stack_size].player = -1;

  if (totally_bletcherous_hack_dont_reset_x_value.player >= 0 && event == EVENT_RESOLVE_SPELL &&
      totally_bletcherous_hack_dont_reset_x_value.player == player &&
      totally_bletcherous_hack_dont_reset_x_value.card == card)
  {
    totally_bletcherous_hack_dont_reset_x_value.player = -1;
    EXE_DWORD(0x78676C) = 1;
  }
}
''',

'26ade00': r'''
void put_card_or_activation_onto_stack_2022(int player, int card, event_t event, int a4, int a5)
{
  if (stack_size >= 32)
    return;
  put_card_or_activation_onto_stack_impl_2022(player, card, event, a4, a5);
}
''',

'26ade40': r'''
void recopy_card_onto_stack_2022(int a1)
{

  int pos = stack_size - 1;

  card_instance_t *stack_inst = get_card_instance(stack_cards[pos].player, stack_cards[pos].card);
  if (stack_inst->internal_card_id == activation_card)
  {
    int pc = stack_inst->parent_card;
    int pp = stack_inst->parent_controller;
    int ts = stack_inst->timestamp;

    card_instance_t *parent_inst =
        get_card_instance(stack_inst->parent_controller, stack_inst->parent_card);

    if (parent_inst->state & STATE_DONT_RECOPY_ONTO_STACK)
      parent_inst->state &= ~STATE_DONT_RECOPY_ONTO_STACK;
    else
    {
      memcpy_inst_and_ext(stack_inst, parent_inst);

      stack_inst->internal_card_id = activation_card;
      stack_inst->unknown0x14 = 0;
      stack_inst->kill_code = 0;
      stack_inst->state |= STATE_IN_PLAY;
      stack_inst->parent_controller = pp;
      stack_inst->parent_card = pc;
      stack_inst->timestamp = ts;

      if (parent_inst->internal_card_id != -1)
        stack_inst->original_internal_card_id = parent_inst->internal_card_id;

      else
        stack_inst->original_internal_card_id = parent_inst->backup_internal_card_id;

      if ((int)stack_inst->original_internal_card_id < damage_card ||
          (int)stack_inst->original_internal_card_id >= damage_card + 47)
      {
        stack_inst->display_pic_csv_id = cards_data[stack_inst->original_internal_card_id].id;

        stack_inst->display_pic_num = get_card_image_number(stack_inst->display_pic_csv_id, pp, pc);
      }
    }

    if (stack_inst->internal_card_id == activation_card)
      stack_inst->token_status &= ~STATUS_DYING;
  }

  if (ai_is_speculating != 1)
    EXE_DWORD_PTR(0x628674)[pos] = a1;

  stack_damage_targets[pos].player = stack_inst->damage_target_player;
  stack_damage_targets[pos].card = stack_inst->damage_target_card;
}
''',

'26adc90': r'''
void allow_response_to_activation_2022(int player, int card)
{

  const char *name = EXE_FN(const char *, 0x439690, int, int)(player, card);
  sprintf((char *)(0x60A690), EXE_STR(0x786264), name);

  int a1 = (EXE_DWORD(0x60A54C) && (player == 0 || (trace_mode & 2))) ? -1 : -2;
  EXE_FN(int, 0x436A20, int, int, const char *, int)(a1, current_phase, EXE_STR(0x60A690),
                                                     EVENT_ACTIVATE);
}
''',

'26add20': r'''
int finalize_activation_2022(int player, int card)
{

  if (cancel == 1)
    return 1;

  card_instance_t *instance = get_card_instance(player, card);
  int iid = get_card_instance(player, card)->internal_card_id;

  if (iid == -1)
    iid = instance->backup_internal_card_id;

  int tapping_for_mana =
      (cards_data[iid].extra_ability & EA_MANA_SOURCE) && tapped_for_mana_color != -1;

  if (!tapping_for_mana && !cant_be_responded_to)
    allow_response_to_activation_2022(player, card);

  EXE_FN(void, 0x436740, void)();

  dispatch_trigger2(current_turn, TRIGGER_TAP_CARD, EXE_STR(0x78fba8), 0, player, card);

  return 1;
}
''',

'26adfa0': r'''
int put_card_on_stack3_2022(int player, int card)
{
  enum
  {
    STATE_BEING_CAST_2022 = 0x80000000u,
    ENABLE_XTRIGGER_REPLACE_ETB_2022 = 0x80,
    XTRIGGER_REPLACE_ETB_2022 = 0x01000e00,
    WAV_SNOW_2022 = 84
  };

  card_instance_t *instance = get_card_instance(player, card);

  int iid = instance->internal_card_id;
  instance->state |= STATE_BEING_CAST_2022;
  if (iid == -1)
  {
    obliterate_top_card_of_stack();
    land_can_be_played &= ~LCBP_SPELL_BEING_PLAYED;
    return 0;
  }

  if (is_what(player, card, TYPE_LAND))
    dispatch_event(player, card, EVENT_LAND_PLAYED);
  else if (!check_special_flags(player, card, SF_NOT_CAST))
  {
    unsigned int amount = instance->ext->times_replicate_paid;
    if (query(player, card, iid, QUERY_STORM))
      amount = get_storm_count_2022();
    if (query(player, card, iid, QUERY_GRAVESTORM))
      amount = get_dead_count(2, 0xff);
    while (amount != 0)
    {
      copy_spell_from_stack_2022(player, player, card);
      --amount;
    }
    dispatch_event(player, card, EVENT_SPELL_CAST);
  }
  dispatch_event_to_single_card(player, card, EVENT_THIS_SPELL_CAST, -1, -1);

  dispatch_trigger2(current_turn, TRIGGER_SPELL_CAST, EXE_STR(0x715a04), 0, player, card);

  land_can_be_played &= ~LCBP_SPELL_BEING_PLAYED;
  iid = instance->internal_card_id;
  if (iid == -1)
  {
    obliterate_top_card_of_stack();
    return 0;
  }

  card_data_t *cd = get_card_data(player, card);

  if (!is_what(player, card, TYPE_LAND))
  {
    const char *name = EXE_FN(const char *, 0x439690, int, int)(player, card);
    sprintf((char *)(0x60A690), EXE_STR(0x737bc0), name);
    EXE_FN(int, 0x436A20, int, int, const char *, int)(-2, current_phase, EXE_STR(0x60A690),
                                                       EVENT_CAST_SPELL);
  }
  instance->state &= ~STATE_INVISIBLE;
  instance->state |= STATE_IN_PLAY;

  uint8_t type = cd->type;
  types_of_cards_on_bf[player] |= type;
  if (ai_is_speculating != 1)
  {
    if (type & TYPE_CREATURE)
      play_sound_effect(WAV_SUMMON);
    if (type & TYPE_ARTIFACT)
      play_sound_effect(WAV_ARTIFACT);
    if (type & TYPE_ENCHANTMENT)
    {
      if (cd->cc[2] == 9)
        play_sound_effect(WAV_PLANESWALKER);
      else
        play_sound_effect(WAV_ENCHANT);
    }
    if (type & TYPE_INSTANT)
      play_sound_effect(WAV_INSTANT);
    if (type & TYPE_INTERRUPT)
      play_sound_effect(WAV_INTERUPT);
    if (type & TYPE_SORCERY)
      play_sound_effect(WAV_SORCERY);
    if (has_subtype(player, card, SUBTYPE_SNOW))
      play_sound_effect((wav_t)WAV_SNOW_2022);
  }

  instance->state &= ~STATE_BEING_CAST_2022;
  if (enable_xtrigger_flags & ENABLE_XTRIGGER_REPLACE_ETB_2022)
    dispatch_xtrigger2(player, (xtrigger_t)XTRIGGER_REPLACE_ETB_2022, "enters the battlefield", 0,
                       player, card);

  EXE_FN(void, 0x436740, void)();

  if (!(instance->internal_card_id == -1 && is_what(-1, iid, TYPE_PERMANENT)))
  {
    push_affected_card_stack();
    get_card_instance(player, card)->regen_status |= KEYWORD_RECALC_ALL;
    dispatch_trigger2(current_turn, TRIGGER_COMES_INTO_PLAY, EXE_STR(0x787108), 0, player, card);
    pop_affected_card_stack();
  }

  EXE_FN(void, 0x477070, void)();
  event_flags &= ~EF_RERUN_DAMAGE_PREVENTION;
  EXE_DWORD(0x4EF1B4) = 0x477B30;
  EXE_FN(int, 0x477a90, void)();
  EXE_FN(void, 0x477070, void)();
  EXE_DWORD(0x4EF19C) = -1;

  if (cd->type & TYPE_LAND)
    ++lands_played;

  if (cancel == 1)
  {
    if (ai_is_speculating != 1)
    {
      load_text_2022(0, "PROMPT_FIZZLE");
      EXE_FN(int, 0x471D70, const char *)(EXE_STR(0x60A690));
      EXE_STDCALL_FN(void, 0x4D5D32, int)(2000);
      EXE_FN(int, 0x471D70, const char *)("");
    }
    EXE_DWORD(0x7A372C) = 1;
    cancel = 0;
    instance->state &= ~STATE_BEING_CAST_2022;
    return 0;
  }
  else
  {
    cancel = 0;
    EXE_FN(void, 0x472260, void)();
    return 1;
  }
}
''',

'26b0480': r'''
int is_a_tappable_mana_source_2022(int player, int card)
{
  card_instance_t *instance = in_play(player, card);
  if (instance && (cards_data[instance->internal_card_id].extra_ability & EA_MANA_SOURCE))
  {
    dispatch_event_with_attacker_to_one_card(player, card, EVENT_CAN_ACTIVATE, 1 - player, -1);
    return event_rval;
  }
  return 0;
}
''',

'26b0500': r'''
void force_activation_for_mana_2022(int player, int card, color_test_t colors)
{
  if (stack_size < 32)
    put_card_or_activation_onto_stack_impl_2022(player, card, EVENT_RESOLVE_ACTIVATION, player, 0);
  needed_mana_colors = colors;
  tapped_for_mana_color = -1;
  int was_tapped = is_tapped(player, card);
  event_activate_then_duplicate_into_stack(player, card, EVENT_ACTIVATE, 1 - player, -1);
  needed_mana_colors = 0;
  if (cancel == 1)
  {
    cancel = 0;
    obliterate_top_card_of_stack();
  }
  else
  {
    if (was_tapped || !is_tapped(player, card))
      dispatch_event(player, card, EVENT_PLAY_ABILITY);
    else
    {
      dispatch_event(player, card, EVENT_TAP_CARD);
      dispatch_event(player, card, EVENT_TAPPED_TO_PLAY_ABILITY);
      dispatch_trigger2(player, TRIGGER_TAP_CARD, EXE_STR(0x78fba8), 1, player, card);
    }
    play_sound_effect(WAV_TAP);
    EXE_FN(void, 0x436740, void)();
    EXE_FN(void, 0x472260, void)();
  }
}
''',

'26b0650': r'''
void tap_card_for_mana_2022(int player, int card)
{
  force_activation_for_mana_2022(player, card, COLOR_TEST_COLORLESS);
}
''',

}
