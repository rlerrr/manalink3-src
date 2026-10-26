"""Typed October 2022 tap/untap, disabling and AI-scoring reconstructions."""

BODIES = {
'2678530': r'''
const char *has_activated_ability_2022(int who_chooses, int player, int card)
{
  int iid = get_card_instance(player, card)->internal_card_id;
  return has_an_activated_ability(iid) ? NULL : "AI helper function";
}
''',

'26a70d0': r'''
int select_permanent_to_disable_2022(int player, int card, target_definition_2022_t *td,
                                     const char *prompt, test_definition_2022_t *test)
{
  td->allow_cancel = 0;
  td->illegal_abilities = 0;
  if (IS_AI(player))
  {
    td->special = TARGET_SPECIAL_EXTRA_FUNCTION;
    td->extra = (int32_t)has_activated_ability_2022;
  }

  card_instance_t *instance = get_card_instance(player, card);

  int choice = 1;
  if (IS_AI(player))
  {
    state_untargettable(player, card, 1);
  }
  if (can_target(td))
  {
    choice = do_dialog(player, player, card, -1, -1,
                       " Select a card in play\n Choose a card from a list", 0);
  }
  int csvid = 0;
  if (choice == 0)
  {
    if (new_pick_target(td, prompt, 0, GS_LITERAL_PROMPT))
    {
      csvid = get_card_name_2022(instance->targets[0].player, instance->targets[0].card);
    }
  }
  else
  {
    if (IS_AI(player))
    {
      csvid = ai_name_a_random_csvid_2022(player, card, 1 - player, 0, test);
    }
    else
    {
      csvid = new_card_from_list(player, 3, test);
    }
  }
  if (IS_AI(player))
  {
    state_untargettable(player, card, 0);
  }
  return csvid;
}
''',

'26a7330': r'''
void activated_abilities_disabler_enters_bf_2022(int player, int card, int action)
{
  int csvid = get_card_instance(player, card)->info_slot;
  if (csvid > 0)
    create_card_name_legacy_2022(player, card, csvid);
}
''',

'26a7370': r'''
int activated_abilities_disabler_2022(int player, int card, event_t event, int action)
{
  if (event == EVENT_ABILITIES && !affect_me(player, card) &&
      !is_humiliated_for_ability(player, card, AL_STATIC))
  {
    int csvid = get_card_instance(player, card)->info_slot;
    if (csvid > 0 && get_id(affected_card_controller, affected_card) ==
                         get_card_instance(player, card)->info_slot)
      action_on_card_2022(player, card, affected_card_controller, affected_card, action);
  }
  return 0;
}
''',

'26a7050': r'''
void ai_modifier_for_land_untappers_2022(int player, int card, int target_pos)
{
  card_instance_t *instance = get_card_instance(player, card);
  int iid =
      get_card_instance(instance->targets[target_pos].player, instance->targets[target_pos].card)
          ->internal_card_id;
  ai_modifier +=
      (is_tapped(instance->targets[target_pos].player, instance->targets[target_pos].card)
           ? cards_ptr[cards_data[iid].id]->ai_base_value
           : -48);
}
''',

'26a6f40': r'''
void ai_modifier_for_haste_granting_effects_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  int mod = -48;
  if (is_sick(instance->targets[0].player, instance->targets[0].card))
  {
    mod = 0;
    if (current_turn == player && current_phase < PHASE_DECLARE_ATTACKERS)
    {
      mod += 48;
    }
    int iid =
        get_card_instance(instance->targets[0].player, instance->targets[0].card)->internal_card_id;
    if (cards_data[iid].extra_ability & EA_ACT_ABILITY)
    {
      mod += 24;
    }
  }
  ai_modifier += mod;
}
''',

'26a6a90': r'''
void ai_modifier_for_targeted_card_drawing_2022(int player, int card, int target_pos,
                                                int cards_to_draw)
{
  card_instance_t *instance = get_card_instance(player, card);
  int targeted_player = instance->targets[target_pos].player;
  int damage_from_triggers = 0;
  CYCLE_ALL_CARDS(p, c, {
    if (in_play(p, c) && !is_humiliated_for_ability(p, c, AL_TRIG))
    {
      int csvid = get_id(p, c);
      switch (csvid)
      {
      case CARD_ID_OB_NIXILIS_REIGNITED_EMBLEM:
      {
        if (p == targeted_player)
        {
          damage_from_triggers += 2;
        }
        break;
      }

      case CARD_ID_PHYREXIAN_TYRANNY:
        damage_from_triggers += 2;
        break;

      case CARD_ID_SPITEFUL_VISIONS:
        damage_from_triggers++;
        break;

      case CARD_ID_UNDERWORLD_DREAMS:
      case CARD_ID_FATE_UNRAVELER:
      case CARD_ID_NEKUSAR_THE_MINDRAZER:
      {
        damage_from_triggers += (p != targeted_player ? 1 : 0);
      }
      }
    }
  });
  if (targeted_player == player)
  {
    if (life[player] - (damage_from_triggers * cards_to_draw) < 6)
    {
      ai_modifier -= 96;
    }
    else
    {
      int amount = get_total_available_lands(player);
      ai_modifier += (amount < 3 || amount >= 6 ? 48 : -48);
    }
  }
  if (targeted_player == 1 - player)
  {
    if (life[player] - (damage_from_triggers * cards_to_draw) <= 0)
    {
      ai_modifier += 96;
    }
    else
    {
      ai_modifier -= 96;
    }
  }
}
''',

'26a6340': r'''
void ai_modifier_for_act_of_treason_2022(int player, int card)
{

  card_instance_t *instance = get_card_instance(player, card);
  if (instance->targets[0].player > -1)
  {
    if (is_what(instance->targets[0].player, instance->targets[0].card, TYPE_CREATURE))
    {
      int pow = get_power(instance->targets[0].player, instance->targets[0].card);
      if (instance->targets[0].player == player)
      {
        int mod = -(pow * 8);
        if (is_sick(instance->targets[0].player, instance->targets[0].card) &&
            current_phase < PHASE_DECLARE_ATTACKERS && current_turn == player)
        {
          mod = pow * 8;
        }
        if (is_tapped(instance->targets[0].player, instance->targets[0].card) &&
            current_phase < PHASE_DECLARE_BLOCKERS && current_turn == 1 - player)
        {
          mod = pow * 8;
        }
        ai_modifier += mod;
      }
      else
      {
        int mod = pow * 8;
        if ((get_card_instance(instance->targets[0].player, instance->targets[0].card)
                 ->regen_status &
             KEYWORD_DEFENDER) &&
            check_for_special_ability(instance->targets[0].player, instance->targets[0].card,
                                      SP_KEYWORD_CANNOT_BLOCK))
        {
          mod = -(pow * 8);
        }
        ai_modifier += mod;
      }
    }
    else
    {
      int cmc = get_cmc(instance->targets[0].player, instance->targets[0].card);
      if (instance->targets[0].player == player)
      {
        ai_modifier +=
            (is_tapped(instance->targets[0].player, instance->targets[0].card) ? (cmc * 8)
                                                                               : -(cmc * 8));
      }
      else
      {
        ai_modifier += (cmc * 8);
      }
    }
  }
}
''',

'26a6500': r'''
int aot_like_spell_2022(int player, int card, event_t event, target_definition_2022_t *td,
                        const char *msg, int additional_flags)
{
  card_instance_t *instance = get_card_instance(player, card);
  int flags = GS_CAN_TARGET | additional_flags;
  if ((event == EVENT_CAST_SPELL && affect_me(player, card)) || event == EVENT_CHANGE_TARGET)
  {
    generic_spell_2022(player, card, event, flags, td, msg, 1, NULL);
    if (event == EVENT_CHANGE_TARGET)
      td->who_chooses = attacking_card_controller;
    if (spell_fizzled != 1)
      ai_modifier_for_act_of_treason_2022(player, card);
    return 0;
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (valid_target(td))
      effect_act_of_treason(player, card, instance->targets[0].player, instance->targets[0].card);
    kill_card(player, card, KILL_DESTROY);
  }
  return generic_spell_2022(player, card, event, flags, td, msg, 1, NULL);
}
''',

'26a7450': r'''
void ai_mod_for_tap_effects_2022(int player, int card, int target_pos)
{

  card_instance_t *instance = get_card_instance(player, card);
  int p = instance->targets[target_pos].player;
  int c = instance->targets[target_pos].card;
  int mod = 0;
  if (is_tapped(p, c))
  {
    ai_modifier -= 96;
    return;
  }
  if (current_turn == player)
  {
    if (is_what(p, c, TYPE_CREATURE))
    {
      if (current_phase < PHASE_DECLARE_BLOCKERS)
      {
        int pow = get_power(p, c);
        int mul = 1;
        if (check_for_ability(p, c, KEYWORD_INFECT | KEYWORD_FIRST_STRIKE | KEYWORD_DOUBLE_STRIKE))
        {
          mul++;
        }
        if (check_for_special_ability(p, c, SP_KEYWORD_DEATHTOUCH))
        {
          mul++;
        }
        mod += (!check_for_special_ability(p, c, SP_KEYWORD_CANNOT_BLOCK) ? (pow * mul) : -24);
      }
      else
      {
        mod -= 96;
      }
    }
    int iid = get_card_instance(p, c)->internal_card_id;
    if (cards_data[iid].extra_ability & EA_ACT_ABILITY)
    {
      mod += (get_cmc(p, c) * 4);
    }
  }
  if (current_turn == 1 - player)
  {
    int mul = 2;
    if (is_what(p, c, TYPE_CREATURE))
    {
      int pow = get_power(p, c);
      if (current_phase < PHASE_DECLARE_ATTACKERS)
      {
        mul = 3;
        if (check_for_ability(p, c, KEYWORD_FLYING))
        {
          mul++;
        }
        if (check_for_special_ability(p, c, SP_KEYWORD_SHADOW | SP_KEYWORD_UNBLOCKABLE))
        {
          mul++;
        }
        mod += (!check_for_ability(p, c, KEYWORD_DEFENDER) ? (pow * mul) : -24);
      }
      else
      {
        if (check_for_ability(p, c, KEYWORD_FLYING))
        {
          mul++;
        }
        mod += (!check_for_special_ability(p, c, SP_KEYWORD_CANNOT_BLOCK) ? (pow * mul) : -24);
      }
    }
    int iid = get_card_instance(p, c)->internal_card_id;
    if (cards_data[iid].extra_ability & EA_ACT_ABILITY)
    {
      mod += (get_cmc(p, c) * 4);
    }
  }
  ai_modifier += mod;
}
''',

'26a7700': r'''
void ai_mod_for_untap_effects_2022(int player, int card, int target_pos, int total_mana_spent)
{

  card_instance_t *instance = get_card_instance(player, card);
  int p = instance->targets[target_pos].player;
  int c = instance->targets[target_pos].card;
  int pow_mod = current_turn == player ? 8 : 12;
  int ab_mod = current_turn == player ? 4 : 8;
  int csvid = get_id(p, c);
  int mod = 0;
  if (!is_tapped(p, c))
  {
    mod -= 48;
  }
  if (is_what(p, c, TYPE_CREATURE))
  {
    mod += (get_power(p, c) * pow_mod);
    int iid = get_card_instance(p, c)->internal_card_id;
    if ((cards_data[iid].extra_ability & EA_ACT_ABILITY) && can_use_activated_abilities(p, c) &&
        !is_sick(p, c))
    {
      mod += ((get_cmc(p, c) - total_mana_spent) * ab_mod);
    }
  }
  else
  {
    if (can_use_activated_abilities(p, c) && !is_animated_and_sick(p, c))
    {
      switch (csvid)
      {
      case CARD_ID_TOLARIAN_ACADEMY:
      {
        mod += ((count_subtype(p, TYPE_ARTIFACT, -1) - total_mana_spent) * (ab_mod * 2));
        break;
      }
      case CARD_ID_SERRAS_SANCTUM:
      {
        mod += ((count_subtype(p, TYPE_ENCHANTMENT, -1) - total_mana_spent) * (ab_mod * 2));
        break;
      }
      case CARD_ID_GAEAS_CRADLE:
      {
        mod += ((count_subtype(p, TYPE_CREATURE, -1) - total_mana_spent) * (ab_mod * 2));
        break;
      }
      case CARD_ID_GRIM_MONOLITH:
      case CARD_ID_MANA_VAULT:
      case CARD_ID_MISHRAS_WORKSHOP:
      {
        mod += ((3 - total_mana_spent) * (ab_mod * 2));
        break;
      }
      case CARD_ID_SOL_RING:
      {
        mod += ((2 - total_mana_spent) * (ab_mod * 2));
        break;
      }
      default:
      {
        mod += ((get_cmc(p, c) - total_mana_spent) * ab_mod);
        break;
      }
      }
    }
  }
  ai_modifier += (p == player ? mod : -mod);
}
''',

'2696810': r'''
void twiddle_2022(int player, int card, int tgtnum)
{
  card_instance_t *instance = get_card_instance(player, card);
  int p = instance->targets[tgtnum].player;
  int c = instance->targets[tgtnum].card;

  load_text_2022(0, "TWIDDLE");
  switch (DIALOG(player, card, EVENT_ACTIVATE, DLG_RANDOM, DLG_NO_STORAGE, DLG_SMALLCARD(p, c),
                 text_lines[1], 1, p == AI ? 1 : 3, text_lines[2], 1, p == AI ? 3 : 1))
  {
  case 1:
    tap_card(p, c);
    break;

  case 2:
    untap_card(p, c);
    break;
  }
}
''',

'2696930': r'''
void ai_modifier_twiddle_2022(int player, int card, int tgtnum)
{
  card_instance_t *instance = get_card_instance(player, card);
  int p = instance->targets[tgtnum].player;
  int c = instance->targets[tgtnum].card;

  if (!is_what(p, c, TYPE_CREATURE) &&
      (!(get_card_data(p, c)->extra_ability & (EA_ACT_ABILITY | EA_ACT_INTERRUPT)) ||
       !is_what(p, c, TYPE_CREATURE | TYPE_ARTIFACT | TYPE_LAND)))
    ai_modifier -= 24;

  if (!(p == AI) != !(is_tapped(p, c)) || (player == p && card == c))
    ai_modifier -= 96;
}
''',

'2696610': r'''
void untap_only_1_permanent_per_upkeep_2022(int player, int card, event_t event, int t_player,
                                            int type)
{
  target_definition_2022_t td;
  default_target_definition(player, card, &td, type);
  td.required_state = TARGET_STATE_TAPPED;
  td.illegal_abilities = 0;
  td.allowed_controller = t_player != 2 ? t_player : current_turn;
  td.preferred_controller = t_player != 2 ? t_player : current_turn;
  td.who_chooses = t_player != 2 ? t_player : current_turn;
  td.allow_cancel = 0;

  card_instance_t *instance = get_card_instance(player, card);

  upkeep_trigger_ability(player, card, event, t_player);

  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    char prompt[200];
    if (ai_is_speculating == 1)
    {
      prompt[0] = 0;
    }
    else
    {
      char typetext[100];
      type_text_2022_impl(typetext, 100, type, TYPETEXT_0);
      scnprintf(prompt, 200, "PROCESSING %s: Select %s to untap.",
                cards_ptr[get_id(player, card)]->full_name, typetext);
    }
    instance->number_of_targets = 0;
    if (can_target(&td) && pick_next_target_noload(&td, prompt))
    {
      instance->number_of_targets = 0;
      untap_card(instance->targets[0].player, instance->targets[0].card);
    }
  }

  if (current_phase == PHASE_UNTAP && event == EVENT_UNTAP &&
      (current_turn == t_player || t_player == 2) &&
      is_what(affected_card_controller, affected_card, type))
  {
    card_instance_t *instance2 = get_card_instance(affected_card_controller, affected_card);
    instance2->untap_status &= ~3;
  }
}
''',

'2695480': r'''
void untap_permanents_during_opponents_untap_2022(int player, int card, type_t types,
                                                  int *where_to_store)
{
  if (current_phase == PHASE_UNTAP && current_turn == 1 - player)
  {
    if (*where_to_store != 77)
    {
      manipulate_all(player, card, player, types, MATCH, 0, 0, 0, 0, 0, 0, -1, 0, ACT_UNTAP);
      *where_to_store = 77;
    }
  }
  else if (*where_to_store == 77)
  {
    *where_to_store = 0;
  }
}
''',

'2687840': r'''
int choosing_to_untap_2022(int player, int card, event_t event)
{
  if (event == EVENT_UNTAP && affect_me(player, card))
    get_card_instance(player, card)->untap_status &= ~2;

  if (current_phase == PHASE_UNTAP && affect_me(player, card))
  {
    if (event == EVENT_TRIGGER)
    {
      card_instance_t *instance = get_card_instance(player, card);
      if ((instance->untap_status & 3) == 1 &&
          !(cards_data[instance->internal_card_id].type & types_that_dont_untap))
      {
        if (!IS_AI(player))
          event_result |= 1;
        else if (call_card_function_i(instance, player, card, EVENT_CHOOSING_TO_UNTAP))
          event_result |= 2;
      }
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
      get_card_instance(player, card)->untap_status |= 2;
  }

  return event == EVENT_CHOOSING_TO_UNTAP;
}
''',

'2687680': r'''
void choose_to_untap_2022(int player, int card, event_t event)
{

  card_instance_t *instance = get_card_instance(player, card);

  if (event == EVENT_CAST_SPELL && affect_me(player, card))
  {
    set_special_flags2(player, card, SF2_COULD_NOT_UNTAP);
  }

  if (instance->targets[1].player > -1 && instance->targets[1].card > -1 &&
      !in_play(instance->targets[1].player, instance->targets[1].card))
  {
    instance->targets[1].player = -1;
  }

  if (event == EVENT_UNTAP && affect_me(player, card) && is_tapped(player, card))
  {
    instance->untap_status &= ~2;
  }

  if (current_phase == PHASE_UNTAP && affect_me(player, card))
  {
    if (event == EVENT_TRIGGER)
    {
      if (instance->untap_status & 1)
      {
        if (!(instance->untap_status & 2) &&
            !(cards_data[instance->internal_card_id].type & types_that_dont_untap))
        {
          if ((player != 1 || (trace_mode & 2)) && ai_is_speculating != 1)
          {
            event_result |= 1;
          }
          else if (instance->targets[1].player > -1 && instance->targets[1].card > -1 &&
                   in_play(instance->targets[1].player, instance->targets[1].card))
          {
            ;
          }
          else
          {
            event_result |= 2;
          }
        }
      }
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      instance->untap_status |= 2;
    }
  }
}
''',

'268c010': r'''
void untap_lands_2022(int player, int card, int num_lands)
{
  int i = 0;
  card_instance_t *instance = get_card_instance(player, card);
  target_definition_2022_t td;
  default_target_definition(player, card, &td, TYPE_LAND);
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  while (i++ < num_lands)
  {
    if (can_target(&td))
    {
      if (select_target(player, card, &td, "Choose a land to untap", NULL))
      {
        instance->number_of_targets = 1;
        untap_card(instance->targets[0].player, instance->targets[0].card);
      }
      else
      {
        i = num_lands;
      }
    }
  }
}
''',

'2689800': r'''
void remove_summoning_sickness_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->state & STATE_SUMMON_SICK)
    instance->state &= ~(STATE_SUMMON_SICK | STATE_SICKNESS_UNUSED);
}
''',

'2696a00': r'''
void remove_from_combat_2022(int player, int card)
{
  card_instance_t *instance = get_card_instance(player, card);

  instance->blocking = -1;

  if (instance->state & STATE_ATTACKING)
    instance->state |= STATE_ATTACKED;

  if (instance->state & STATE_BLOCKING)
    instance->state |= STATE_BLOCKED;

  instance->state &= ~(STATE_ATTACKING | STATE_BLOCKING);
}
''',

}
