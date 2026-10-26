"""Typed stack/activation, trigger-response and opponent/deck utility reconstructions."""

BODIES = {
'26a9420': r'''
int create_legacy_activate_from_iid(int player, int iid, CardFunction func_ptr, int t_player, int t_card)
{
  int card = add_card_to_hand(player, LEGACY_EFFECT_ACTIVATED);
  if (card != -1) {
    card_instance_t *legacy = get_card_instance(player, card);
    --hand_count[player];
    legacy->state = player == 1 ? 0x1002 : 2;
    uint8_t color = cards_data[iid].color;
    legacy->damage_source_player = player;
    legacy->color = color;
    legacy->initial_color = color;
    legacy->card_color = color;
    legacy->mana_color = color;
    int csvid = (uint16_t)cards_data[iid].id;
    legacy->damage_source_card = card;
    legacy->display_pic_csv_id = csvid;
    int num_pics = cards_ptr[csvid]->num_pics;
    legacy->display_pic_num = num_pics > 1 ? internal_rand(num_pics) : 0;
    legacy->damage_target_player = t_player;
    legacy->damage_target_card = t_card;
    if (t_player != -1 && t_card != -1) get_card_instance(t_player, t_card)->regen_status |= 0x1f000000;
    legacy->info_slot_fn = func_ptr;
  }
  return card;
}
''',
'26adc00': r'''
int fx_activation_906(int player, int card, event_t event)
{
  if (event != EVENT_RESOLVE_ACTIVATION && event != EVENT_RESOLVE_TRIGGER) return 0;
  card_instance_t *instance = get_card_instance(player, card);
  EXE_DWORD(0x7a3610) = instance->parent_controller; /* activating_player */
  EXE_DWORD(0x7876e8) = instance->parent_card; /* activating_card */
  int result = call_card_fn(cards_data[instance->original_internal_card_id].code_pointer, instance, player, card, event);
  EXE_DWORD(0x7a3610) = -1;
  EXE_DWORD(0x7876e8) = -1;
  return result;
}
''',
'26afd30': r'''
int put_card_on_stack_2022(int player, int card, int second_call)
{
  card_instance_t *instance = get_card_instance(player, card);
  int iid = instance->internal_card_id;
  if (iid < 0) return 0;
  char str[500];
  int old_iid, old_controller, old_card;
  if (!second_call) {
    if (trace_mode & 2) {
      char trace[300];
      int count = EXE_DWORD(0x60ec40)++;
      sprintf(trace, "%d: Player %d is playing %s(%d).\n", count, player, cards_data[iid].name, card);
      EXE_FN(void, 0x4a7d80, const char *)(trace);
    }
    if (!compute_and_check_casting_cost(player, player, card)) return 0;
    engine_selected_card = -1;
    EXE_DWORD(0x78676c) = 0;
    max_x_value = -1;
    if ((cards_data[iid].type & 0x3c) &&
        !dispatch_event_to_single_card(player, card, EVENT_CAN_CAST, 1 - player, -1)) return 0;
    spell_fizzled = -1;
    old_iid = EXE_DWORD(0x4ef19c);
    old_controller = card_on_stack_controller;
    old_card = card_on_stack;
    EXE_DWORD(0x4ef19c) = iid;
    card_on_stack_controller = player;
    card_on_stack = card;
    if (stack_size < 32) put_card_or_activation_onto_stack(player, card, EVENT_RESOLVE_SPELL, player, 0);
    EXE_FN(void, 0x430150, void)();
    if (!EXE_DWORD(0x78676c)) charge_spell_cost_2022(&cards_data[iid], player, card);
    max_x_value = -1;
    if (!(instance->token_status & 8)) {
      --hand_count[player];
      if (player != -1) {
        uint8_t type = cards_data[iid].type;
        if (type & 2) ++creature_count[player];
        if (type & 0x40) ++artifact_count[player];
        if (type & 4) ++enchantment_count[player];
        types_of_cards_on_bf[player] |= type;
      }
    }
    redraw_libraries();
    instance->state |= 0x30020;
    land_can_be_played |= 0x20;
    EXE_DWORD(0x4ef19c) = old_iid;
    card_on_stack_controller = old_controller;
    card_on_stack = old_card;
    if (spell_fizzled != 1) {
      if (dispatch_event(player, card, (event_t)0x6c)) spell_fizzled = 1;
      if (spell_fizzled == 1) EXE_FN(void, 0x430200, int)(player);
      EXE_FN(void, 0x4301e0, void)();
      if (spell_fizzled == 1) goto failed;
      if (player == 1 && !(trace_mode & 2) && ai_is_speculating == 1 && cards_data[iid].cc[1] == 0xff)
        ai_modifier -= 12 * EXE_FN(int, 0x435940, int)(1);
      if (instance->internal_card_id == -1) {
        obliterate_top_card_of_stack();
        land_can_be_played &= ~0x20;
        call_sub_437E20_unless_ai_is_speculating();
      }
      return 1;
    }
    EXE_FN(void, 0x4301e0, void)();
  } else {
    old_iid = EXE_DWORD(0x4ef19c);
    old_controller = card_on_stack_controller;
    old_card = card_on_stack;
    EXE_DWORD(0x4ef19c) = iid;
    card_on_stack_controller = player;
    card_on_stack = card;
    if (!(cards_data[iid].type & 1)) {
      recopy_card_onto_stack(1);
      if (!(cards_data[iid].type & 1)) {
        if (EXE_DWORD(0x628c7c)) EXE_DWORD(0x628c7c) |= 2;
        const char *name = EXE_FN(const char *, 0x439690, int, int)(card_on_stack_controller, card_on_stack);
        sprintf((char *)0x60a690, EXE_STR(0x6288d8), name);
        EXE_FN(int, 0x436a20, int, int, const char *, int)(-2, current_phase, EXE_STR(0x60a690), 0xd3);
        if (EXE_DWORD(0x628c7c) & 2) {
          call_sub_437E20_unless_ai_is_speculating();
          EXE_STDCALL_FN(void, 0x4d5d32, int)(3000);
        }
      }
    }
    EXE_DWORD(0x4ef19c) = old_iid;
    card_on_stack_controller = old_controller;
    card_on_stack = old_card;
    if (instance->internal_card_id == -1) spell_fizzled = 1;
    land_can_be_played &= ~0x20;
    instance->state |= player == 0 ? 0x80 : 0x400080;
    if (ai_is_speculating != 1) EXE_FN(void, 0x436700, void)();
    if (spell_fizzled == 1) goto failed;
    if (player == 1 && ai_is_speculating != 1 && !EXE_DWORD(0x628c7c) && !(cards_data[iid].type & 1)) {
      load_text_2022(NULL, "PROMPT_CAST1");
      sprintf(str, text_lines[0], opponent_name);
      if (cards_data[iid].cc[1] == 0xff) sprintf(str, EXE_STR(0x715dd8), opponent_name, x_value);
      target_t target = {-1, -1};
      if (instance->number_of_targets == 1) target = instance->targets[0];
      EXE_FN(int, 0x471b60, int, int, int, int, const char *, int)(1, card, target.player, target.card, str, 0);
    }
  }
  call_sub_437E20_unless_ai_is_speculating();
  if (spell_fizzled != 1) {
    EXE_FN(void, 0x4373b0, int, int)(player, card);
    return 1;
  }
failed:;
  int p = player;
  if (instance->token_status & 8) p = EXE_FN(int, 0x4779f0, int, int, int)(player, card, 1);
  else if (instance->internal_card_id != -1) ++hand_count[player];
  uint8_t type = cards_data[iid].type;
  if (type & 2) --creature_count[p];
  if (type & 0x40) --artifact_count[p];
  if (type & 4) --enchantment_count[p];
  redraw_libraries();
  instance->state &= 0xfffcff5d;
  if (player == 1 && !(trace_mode & 2)) EXE_DWORD(0x7a372c) = 1;
  spell_fizzled = 0;
  obliterate_top_card_of_stack();
  land_can_be_played &= ~0x20;
  return 0;
}
''',
'26ae510': r'''
int put_card_on_stack2_2022(int player, int card)
{
  int changed = EXE_DWORD(0x62bce8) == 0xd3;
  if (changed) EXE_DWORD(0x628534) = 1;
  if (!put_card_on_stack_2022(player, card, 0) || !put_card_on_stack_2022(player, card, 1)) {
    if (changed) EXE_DWORD(0x628534) = 0;
    return 0;
  }
  int result = put_card_on_stack3(player, card) != 0;
  if (changed) EXE_DWORD(0x628534) = 0;
  return result;
}
''',
'26cbf50': r'''
void allow_response_to_trigger(int player, int card)
{
  const char *name = EXE_FN(const char *, 0x439690, int, int)(player, card);
  sprintf((char *)0x60a690, "Trigger %s", name);
  int a1 = EXE_DWORD(0x60a54c) && (player == 0 || (trace_mode & 2)) ? -1 : -2;
  EXE_FN(int, 0x436a20, int, int, const char *, int)(a1, current_phase, EXE_STR(0x60a690), EVENT_RESOLVE_TRIGGER);
}
''',
'26c7890': r'''
int limit_allowed_in_deck(int csvid)
{
  static const int unlimited_list[17] = {
    91, 126, 164, 188, 239, 2100, 2101, 2102, 2103, 2104, 20819, 15719, 22636, 19685, 18489, 7107, 13335
  };
  for (unsigned int i = 0; i < 17; ++i) if (csvid == unlimited_list[i]) return 5000;
  return csvid == 0x5100 ? 7 : 4;
}
''',
'26c78f0': r'''
int random_rogue(void)
{
  typedef struct {
    char name[200];
    char quote[300];
    char imgname[268];
    int difficulty;
    char padding[4004];
  } rogue_t;
  rogue_t *rogues = (rogue_t *)0x62c3c0;
  int num_rogues = EXE_DWORD(0x7370b8);
  int ok_rogues[num_rogues];
  read_and_adjust_settings();
  int num_ok_rogues = 0;
  for (int i = 0; i < num_rogues; ++i)
    if (rogues[i].name[0] && rogues[i].imgname[0] && rogues[i].difficulty == opponent_skill) ok_rogues[num_ok_rogues++] = i;
  if (!num_ok_rogues) {
    for (int i = 0; i < num_rogues; ++i)
      if (rogues[i].name[0] && rogues[i].imgname[0]) ok_rogues[num_ok_rogues++] = i;
  }
  if (num_ok_rogues) return ok_rogues[internal_rand(num_ok_rogues)];
  popup("Fatal error", "No valid opponents defined in Rogues.csv.");
  abort();
}
'''
}
