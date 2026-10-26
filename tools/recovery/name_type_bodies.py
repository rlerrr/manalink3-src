"""Typed October 2022 names, shared-name actions, legacy naming and type text."""

BODIES = {
'26798f0': r'''
const char *get_card_name_by_id_2022(int csvid) { return cards_ptr[csvid]->name; }
''',

'2679910': r'''
const char *get_card_name_by_instance_2022(card_instance_t *instance)
{
  return cards_ptr[cards_data[instance->internal_card_id].id]->name;
}
''',

'2679940': r'''
const char *get_card_or_subtype_name_2022(int subtype_or_csvid)
{
  if (subtype_or_csvid < 0)
    return cards_ptr[-(subtype_or_csvid + 1)]->name;
  else if (subtype_or_csvid <= 232)
    return strs_hunting_subtypenames[subtype_or_csvid];
  else
    return "";
}
''',

'26786e0': r'''
int subtype_name_legacy_2022(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_SET_LEGACY_EFFECT_NAME && instance->targets[0].player > -1)
  {
    scnprintf(set_legacy_effect_name_addr, 51, "%s",
              raw_get_subtype_text(instance->targets[0].player));
  }
  if (instance->targets[0].card == 2 && event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}
''',

'2678770': r'''
int legacy_name_with_rfg_check_2022(int player, int card, event_t event)
{

  if (event == EVENT_SET_LEGACY_EFFECT_NAME)
  {
    card_instance_t *instance = get_card_instance(player, card);
    if (instance->targets[1].card > -1)
    {
      scnprintf(set_legacy_effect_name_addr, 51, "%s",
                cards_ptr[cards_data[instance->targets[1].card].id]->name);
    }
  }

  if (event == EVENT_STATIC_EFFECTS && affect_me(player, card))
  {
    card_instance_t *instance = get_card_instance(player, card);
    if (instance->targets[1].card > -1 && instance->targets[1].player > -1)
    {
      if (!check_rfg(instance->targets[1].player, cards_data[instance->targets[1].card].id))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
  }

  return 0;
}
''',

'2693c00': r'''
int create_card_name_legacy_2022(int player, int card, int id)
{
  int legacy_card = create_legacy_effect_exe_2022(player, card, LEGACY_EFFECT_ASWAN, player, card);
  card_instance_t *legacy = get_card_instance(player, legacy_card);
  legacy->info_slot = -id - 1;
  return legacy_card;
}
''',

'2693f40': r'''
int create_hardcoded_subtype_name_legacy_2022(int player, int card, int subtype)
{
  int legacy_card = create_legacy_effect_exe_2022(player, card, LEGACY_EFFECT_ASWAN, player, card);
  card_instance_t *legacy = get_card_instance(player, legacy_card);
  legacy->info_slot = subtype;
  return legacy_card;
}
''',

'2693ee0': r'''
int create_subtype_name_legacy_2022(int player, int card, int subtype)
{
  int legacy_card =
      create_targetted_legacy_effect(player, card, &subtype_name_legacy_2022, player, card);
  get_card_instance(player, legacy_card)->targets[0].player = subtype;
  get_card_instance(player, legacy_card)->eot_toughness = EVENT_SET_LEGACY_EFFECT_NAME;
  return legacy_card;
}
''',

'26a6fe0': r'''
int create_legacy_name_with_rfg_check_2022(int player, int card, int owner, int iid)
{

  int legacy_card =
      create_targetted_legacy_effect(player, card, &legacy_name_with_rfg_check_2022, player, card);
  get_card_instance(player, legacy_card)->targets[1].player = owner;
  get_card_instance(player, legacy_card)->targets[1].card = iid;
  get_card_instance(player, legacy_card)->eot_toughness = EVENT_SET_LEGACY_EFFECT_NAME;
  return legacy_card;
}
''',

'269f930': r'''
int get_card_name_2022(int player, int card)
{
  int csvid = player == -1 ? cards_data[card].id : get_id(player, card);
  if (player != -1)
  {

    if (cards_data[get_card_instance(player, card)->original_internal_card_id].id ==
        CARD_ID_SAKASHIMA_THE_IMPOSTOR)
    {
      return CARD_ID_SAKASHIMA_THE_IMPOSTOR;
    }
    if (cards_data[get_card_instance(player, card)->original_internal_card_id].id ==
        CARD_ID_LAZAV_DIMIR_MASTERMIND)
    {
      return CARD_ID_LAZAV_DIMIR_MASTERMIND;
    }
    if (get_id(player, card) == CARD_ID_DARKSTEEL_MUTATION_INSECT)
    {
      return cards_data[get_card_instance(player, card)->targets[1].card].id;
    }

    if (get_id(player, card) == CARD_ID_WASTES)
    {
      int iid = get_card_instance(player, card)->targets[1].card;
      return iid == -1 ? CARD_ID_WASTES : cards_data[iid].id;
    }
  }
  if (csvid == CARD_ID_FACE_DOWN_CREATURE)
  {
    return -1;
  }
  return csvid;
}
''',

'269fa60': r'''
int has_same_name_2022(int player, int card, int player2, int card2)
{
  int id1 = get_card_name_2022(player, card);
  int id2 = get_card_name_2022(player2, card2);
  if (id1 == -1 || id2 == -1)
  {
    return 0;
  }

  if (id1 == CARD_ID_RIPTIDE_REPLICATOR_TOKEN && id2 == CARD_ID_RIPTIDE_REPLICATOR_TOKEN)
  {
    if (get_card_instance(player, card)->targets[1].card != -1 &&
        get_card_instance(player2, card2)->targets[1].card != -1 &&
        get_card_instance(player, card)->targets[1].card ==
            get_card_instance(player2, card2)->targets[1].card)
    {
      return 1;
    }
    return 0;
  }
  if (id1 == id2)
  {
    return 1;
  }
  return 0;
}
''',

'269fb40': r'''
void damage_creature_and_all_the_others_with_the_same_name_2022(int player, int card, int t_player,
                                                                int t_card, int dmg)
{
  int id = get_card_name_2022(t_player, t_card);
  if (id != -1)
  {
    test_definition_2022_t this_test;
    default_test_definition_2022(&this_test, TYPE_CREATURE);
    this_test.id = id;
    new_damage_all_2022(player, card, ANYBODY, dmg, 0, &this_test);
  }
  else
    damage_creature(t_player, t_card, dmg, player, card);
}
''',

'269fbf0': r'''
int tutor_card_with_the_same_name_2022(int player, int card, int who_tutors, int t_player,
                                       int tutor_from, int tutor_to, int must_tutor)
{
  int id = get_card_name_2022(player, card);

  if (id == CARD_ID_SPLINTER_TOKEN)
  {
    id = CARD_ID_SPLINTER;
  }
  char msg[100];
  if (id == -1)
  {
    scnprintf(msg, 100,
              "You won't be able to tutor any card as you're searching for a card with no name.");
  }
  else
  {
    scnprintf(msg, 100, "Select a card named %s", cards_ptr[id]->name);
  }
  test_definition_2022_t this_test;
  new_default_test_definition(&this_test, TYPE_ANY, msg);
  this_test.id = id;
  int result = new_global_tutor(who_tutors, t_player, tutor_from, tutor_to, must_tutor,
                                AI_FIRST_FOUND, &this_test);
  return result;
}
''',

'269fcf0': r'''
int count_permanents_by_name_2022(int player, type_t type, int csvid)
{
  if (csvid == -1)
    return 0;
  int result = 0;
  int p = current_turn;
  for (int c = active_cards_count[p] - 1; c >= 0; --c)
    if ((p == player || player == ANYBODY) && in_play(p, c) && is_what(p, c, type))
      result += get_card_name_2022(p, c) == csvid;
  p = 1 - current_turn;
  for (int c = active_cards_count[p] - 1; c >= 0; --c)
    if ((player == ANYBODY || p == player) && in_play(p, c) && is_what(p, c, type))
      result += get_card_name_2022(p, c) == csvid;
  return result;
}
''',

'26a5250': r'''
int ai_name_a_random_csvid_2022(int player, int card, int t_player, type_t type,
                                test_definition_2022_t *test)
{
  int csvid = CARD_ID_LIGHTNING_BOLT;
  for (int rounds = 0; rounds < 10; ++rounds)
  {
    int count = internal_rand(count_deck(t_player));
    int iid = deck_ptr[t_player][count];
    if (iid != -1 &&
        (type ? is_what(-1, iid, type) : test && new_make_test_2022(t_player, iid, -1, test)))
    {
      csvid = cards_data[deck_ptr[t_player][count]].id;
      break;
    }
  }
  char buffer[100];
  scnprintf(buffer, 100, "Opponent named %s", cards_ptr[csvid]->name);
  do_dialog(1 - player, player, card, -1, -1, buffer, 0);
  return csvid;
}
''',

'268edc0': r'''
int name_a_card_2022(int player, int ai_player, test_definition_2022_t *test, int mode)
{
  if (ai_is_speculating != 1 && (player != AI || (trace_mode & 2)))
    return new_card_from_list_2022(player, mode, test);
  int selected = new_select_a_card_2022(player, ai_player, 1, 7, 0, -1, test);
  int iid;
  if (selected != -1)
    iid = deck_ptr[ai_player][selected];
  else
  {
    selected = new_select_a_card_2022(player, ai_player, 2, 7, 0, -1, test);
    if (selected != -1)
      iid = graveyard_ptr_mutable[ai_player][selected];
    else
    {
      selected = new_select_a_card_2022(player, ai_player, 3, 7, 0, -1, test);
      if (selected == -1)
        return -1;
      iid = rfg_ptr[ai_player][selected];
    }
  }
  return mode & 2 ? cards_data[iid].id : iid;
}
''',

'2678c50': r'''
void alternate_legacy_text_2022_impl(int textnum, int player, int legacycard)
{
  card_instance_t *instance = get_card_instance(player, legacycard);
  if (cards_data[instance->internal_card_id].id != 902 &&
      cards_data[instance->internal_card_id].id != 903)
    backtrace_and_abort("functions/functions.c:15713\n(cards_data[instance->internal_card_id].id "
                        "== 902 || cards_data[instance->internal_card_id].id == 903) && "
                        "\"alternate_legacy_text only works on csvid 902 or 903 effects\"");
  if (instance->internal_card_id == LEGACY_EFFECT_PUMP)
    backtrace_and_abort(
        "functions/functions.c:15714\ninstance->internal_card_id != LEGACY_EFFECT_PUMP && "
        "\"alternate_legacy_text doesn't work on LEGACY_EFFECT_PUMP\"");
  instance->eot_toughness = (instance->eot_toughness & ~0x300) | textnum;
}
''',

'2693bc0': r'''
int alternate_legacy_text_2022(int textnum, int player, int legacycard)
{
  if (legacycard >= 0)
    alternate_legacy_text_2022_impl(textnum, player, legacycard);
  return legacycard;
}
''',

'2678d10': r'''
void type_text_2022_impl(char *buf, int maxlen, type_t type, type_text_t flags)
{

  const char *types[17] = {0};
  int num_types = 0;

  load_text_2022(0, "TYPE");

  if (!(flags & TYPETEXT_NO_ABBREV))
  {

    if ((type & (TYPE_INSTANT | TYPE_INTERRUPT)) == (TYPE_INSTANT | TYPE_INTERRUPT))
      type &= ~TYPE_INTERRUPT;

    if ((type & (TYPE_PERMANENT | TYPE_INSTANT | TYPE_SORCERY)) ==
        (TYPE_PERMANENT | TYPE_INSTANT | TYPE_SORCERY))
    {
      type &= ~(TYPE_NONEFFECT | TARGET_TYPE_PLANESWALKER);
      types[num_types++] = text_lines[2];
    }

    if ((type & TYPE_PERMANENT) == TYPE_PERMANENT)
    {
      type &= ~(TYPE_PERMANENT | TARGET_TYPE_PLANESWALKER);
      types[num_types++] = text_lines[9];
    }
  }

#define TYPE(typ, idx)                                                                             \
  if (type & typ)                                                                                  \
  types[num_types++] = text_lines[idx]
  TYPE(TYPE_ARTIFACT, 1);
  TYPE(TYPE_CREATURE, 3);
  TYPE(TYPE_ENCHANTMENT, 5);
  TYPE(TYPE_INSTANT, 6);
  TYPE(TYPE_INTERRUPT, 7);
  TYPE(TYPE_LAND, 8);
  TYPE(TARGET_TYPE_PLANESWALKER, 12);
  TYPE(TYPE_SORCERY, 14);

  sort_ArrayConstCharPtr(types, num_types);

  TYPE(TYPE_EFFECT, 4);

  if (num_types == 0)
    types[num_types++] = text_lines[0];

  const char *non_txt = (flags & TYPETEXT_INVERT) ? "non" : "";

  if (num_types == 1)
    scnprintf(buf, maxlen, "%s%s", non_txt, types[0]);
  else
  {
    int i;
    char *p = buf;
    int end = num_types - ((flags & TYPETEXT_INVERT) ? 1 : 2);
    for (i = 0; i < end; ++i)
      p += scnprintf(p, maxlen - (p - buf), "%s%s, ", non_txt, types[i]);

    if (flags & TYPETEXT_INVERT)
      scnprintf(p, maxlen - (p - buf), "%s%s", non_txt, types[num_types - 1]);
    else
      scnprintf(p, maxlen - (p - buf), "%s or %s", types[num_types - 2], types[num_types - 1]);
  }

  return;
#undef TYPE
}
''',

'26965d0': r'''
char *type_text_2022(char *buf, int maxlen, type_t type, type_text_t flags)
{
  if (ai_is_speculating == 1)
    *buf = 0;
  else
    type_text_2022_impl(buf, maxlen, type, flags);
  return buf;
}
''',

}
