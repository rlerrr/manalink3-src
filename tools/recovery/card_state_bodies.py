"""Typed October 2022 card-state, land and mana-cost helpers."""

BODIES = {
'2680e20': r'''
int is_token_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (!(instance->token_status & STATUS_TOKEN))
    return 0;
  return !check_special_flags_2022(player, card, SF_UNEARTH);
}
''',
'26940a0': r'''
int is_x_spell_2022(int player, int card)
{
  card_data_t *data = player == -1 ? &cards_data[card] : get_card_data_2022(player, card);
  if (data->cc[1] == 255)
    return 1;
  return check_special_flags2_2022(player, card, SF2_X_SPELL);
}
''',
'2693940': r'''
int is_hardcoded_card_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  return (int)cards_data[instance->internal_card_id].code_pointer < 0x02001006;
}
''',
'2679820': r'''
int is_valid_card_2022(int id)
{
  if (id < 0 || id >= available_slots || !(card_coded[id / 8] & (1 << (id % 8))))
    return 0;
  int iid = get_internal_card_id_from_csv_id(id);
  if (cards_data[iid].cc[2] == 3 || cards_ptr[id]->expansion == 0x1000 ||
      cards_data[iid].type == TYPE_EFFECT)
    return 0;
  /* Native CSVIDs: seven ante cards, the dummy, Rules Engine and Deadbox. */
  return (id & ~4) != 0x38 && id != 0x2b && id != 0x1df && id != 0x1af &&
         id != 0x2fb && id != 0x32a && id != 0x394 && id != 0x615a && id != 0x6161;
}
''',
'26925a0': r'''
int is_stolen_2022(int player, int card)
{
  if (player == 0)
    return (get_card_instance(0, card)->state & STATE_OWNED_BY_OPPONENT) != 0;
  if (player == 1)
    return (get_card_instance(1, card)->state & STATE_OWNED_BY_OPPONENT) == 0;
  return 0;
}
''',
'267f6d0': r'''
int is_target_2022(int player, int card, int controller_of_effect)
{
  for (int i = 0; i < stack_size; ++i)
  {
    if (controller_of_effect == ANYBODY || stack_cards[i].player == controller_of_effect)
    {
      card_instance_t *instance = get_card_instance(stack_cards[i].player, stack_cards[i].card);
      for (int j = 0; j < instance->number_of_targets; ++j)
        if (instance->targets[j].player == player && instance->targets[j].card == card)
          return 1;
    }
  }
  return 0;
}
''',
'2682100': r'''
int is_sick_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->state & STATE_SUMMON_SICK)
    if (is_what_2022(player, card, TYPE_CREATURE))
      return !check_special_flags2_2022(player, card, SF2_THOUSAND_YEAR_ELIXIR);
  return 0;
}
''',
'2682170': r'''
int is_animated_and_sick_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->state & STATE_SUMMON_SICK)
    if (is_what_2022(player, card, TYPE_CREATURE))
      return !check_special_flags2_2022(player, card, SF2_THOUSAND_YEAR_ELIXIR);
  return 0;
}
''',
'26812d0': r'''
int is_attacking_2022(int player, int card)
{
  return (get_card_instance(player, card)->state & STATE_ATTACKING) != 0;
}
''',
'2681300': r'''
card_instance_t *in_play_and_attacking_2022(int player, int card)
{
  card_instance_t *instance = in_play(player, card);
  if (instance && !(instance->state & STATE_ATTACKING))
    return NULL;
  return instance;
}
''',
'268bf20': r'''
int check_state_2022(int player, int card, int state)
{
  return (get_card_instance(player, card)->state & state) != 0;
}
''',
'268bf50': r'''
int check_status_2022(int player, int card, int status)
{
  return (get_card_instance(player, card)->token_status & status) != 0;
}
''',
'2687990': r'''
void add_state_2022(int player, int card, int state)
{
  if (card != -1)
  {
    get_card_instance(player, card)->state |= state;
    return;
  }
  for (int p = 0; p < 2; ++p)
    if (player == p || player == ANYBODY)
      for (int c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what_2022(p, c, TYPE_CREATURE))
          get_card_instance(p, c)->state |= state;
}
''',
'2687a50': r'''
void remove_state_2022(int player, int card, int state)
{
  if (card != -1)
  {
    card_instance_t *instance = get_card_instance(player, card);
    if (instance->state & state)
      instance->state &= ~state;
    return;
  }
  for (int p = 0; p < 2; ++p)
    if (player == p || player == ANYBODY)
      for (int c = 0; c < active_cards_count[p]; ++c)
        if (in_play(p, c) && is_what_2022(p, c, TYPE_PERMANENT))
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (instance->state & state)
            instance->state &= ~state;
        }
}
''',
'2687d40': r'''
void add_status_2022(int player, int card, int status)
{
  get_card_instance(player, card)->token_status |= status;
}
''',
'2687d70': r'''
void remove_status_2022(int player, int card, int status)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->token_status & status)
    instance->token_status &= ~status;
}
''',
'2687da0': r'''
void state_untargettable_2022(int player, int card, int mode)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (mode == 0)
  {
    if (instance->state & STATE_CANNOT_TARGET)
      instance->state &= ~STATE_CANNOT_TARGET;
  }
  else if (mode == 1)
    instance->state |= STATE_CANNOT_TARGET;
}
''',
'2679c30': r'''
int tap_card_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (!(instance->state & STATE_TAPPED))
  {
    tapped_for_mana_color = -1;
    instance->state |= STATE_TAPPED;
    dispatch_event(player, card, EVENT_TAP_CARD);
  }
  return 0;
}
''',
'2679c90': r'''
void untap_card_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->state & STATE_TAPPED)
  {
    unsigned char bits = player_bits[player];
    instance->state &= ~STATE_TAPPED;
    if (bits & PB_SEND_EVENT_UNTAP_CARD_TO_ALL)
      dispatch_event(player, card, EVENT_UNTAP_CARD);
    else
      dispatch_event_with_attacker_to_one_card(player, card, EVENT_UNTAP_CARD, 1 - player, -1);
  }
}
''',
'2679d10': r'''
void untap_card_no_event_2022(int player, int card)
{
  get_card_instance(player, card)->state &= ~STATE_TAPPED;
}
''',
'267eb80': r'''
int is_creature_dead_2022(void)
{
  for (int p = 0; p < 2; ++p)
    for (int c = 0; c < active_cards_count[p]; ++c)
    {
      card_data_t *data = get_card_data_2022(p, c);
      if ((data->type & TYPE_CREATURE) && in_play(p, c) && can_regenerate(p, c))
        return 99;
    }
  return 0;
}
''',
'268e7a0': r'''
int has_dead_creature_2022(int player)
{
  for (int p = 0; p < 2; ++p)
    if (player == p || player == ANYBODY)
      for (int *grave = graveyard_ptr_mutable[p]; *grave != -1; ++grave)
        if (cards_data[*grave].type & TYPE_CREATURE)
          return 1;
  return 0;
}
''',
'269f2e0': r'''
card_instance_t *has_activation_on_stack_2022(int player, int card)
{
  for (int c = 0; c < active_cards_count[player]; ++c)
  {
    card_instance_t *instance = get_card_instance(player, c);
    if (instance && instance->internal_card_id == activation_card &&
        instance->parent_card == card && instance->parent_controller == player)
      return instance;
  }
  return NULL;
}
''',
'267c0c0': r'''
int is_basic_land_by_id_2022(int id)
{
  /* Printed basics, the five snow basics, and Wastes: native CSVIDs. */
  return id == 0x5b || id == 0x7e || id == 0xef || id == 0xbc || id == 0xa4 ||
         (unsigned int)id - 0x834U <= 4U || id == 0x3d67;
}
''',
'267c110': r'''
int is_basic_land_2022(int player, int card)
{
  return is_basic_land_by_id_2022(player == -1 ? cards_data[card].id : get_id(player, card));
}
''',
'2684ee0': r'''
int is_mana_producer_land_2022(int player, int card)
{
  int iid = player == -1 ? card : get_card_instance(player, card)->internal_card_id;
  return is_what_2022(-1, iid, TYPE_LAND) && (cards_data[iid].extra_ability & EA_MANA_SOURCE);
}
''',
'2667b10': r'''
int has_no_mana_cost_2022(int player, int card)
{
  int csvid = player == -1 ? cards_data[card].id : get_id(player, card);
  /* Native CSVIDs; independently decoded from the 2022 decision tree. */
  switch (csvid)
  {
    case 0x1dd5: /* Evermind */
    case 0x211d: /* Ancestral Vision */
    case 0x2181: /* Hypergenesis */
    case 0x2194: /* Living End */
    case 0x2197: /* Lotus Bloom */
    case 0x21c6: /* Restore Balance */
    case 0x221a: /* Wheel of Fate */
    case 0x4e75: /* Crashing Footfalls */
    case 0x4ebe: /* Mox Tantalite */
    case 0x59b6: /* Asmoranomardicadaistinaculdacar */
    case 0x5a01: /* Gaea's Will */
    case 0x5a0a: /* Glimpse of Tomorrow */
    case 0x5a1b: /* Inevitable Betrayal */
    case 0x5a4b: /* Profane Tutor */
    case 0x5a54: /* Resurgent Belief */
    case 0x5a72: /* Sol Talisman */
      return 1;
    default:
      return 0;
  }
}
''',
'26b3560': r'''
card_data_t *get_card_data_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  int iid = instance->internal_card_id;
  if (iid == -1)
    iid = instance->backup_internal_card_id;
  return &cards_data[iid];
}
''',
}
