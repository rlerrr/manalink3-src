"""Typed October 2022 type changes, animation, copying and cleanup."""

BODIES = {
'2677a90': r'''
int remove_untargettable_legacy_2022(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card) &&
      reason_for_trigger_controller == player)
  {
    if (trigger_cause_controller == instance->targets[0].player &&
        check_state(trigger_cause_controller, trigger_cause, STATE_CANNOT_TARGET))
    {
      if (event == EVENT_TRIGGER)
      {
        event_result |= 2;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {
        state_untargettable(trigger_cause_controller, trigger_cause, 0);
        kill_card(player, card, KILL_EXILE);
      }
    }
  }
  if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}
''',

'2695e20': r'''
void type_uncounterable_2022(int player, int card, event_t event, int t_player, int type,
                             test_definition_2022_t *this_test)
{
  if (is_what(player, card, TYPE_PERMANENT) && is_humiliated_for_ability(player, card, AL_STATIC))
  {
    return;
  }
  if (event == EVENT_CAST_SPELL && !affect_me(player, card) &&
      !is_what(affected_card_controller, affected_card, TYPE_LAND) && in_play(player, card))
  {
    if (t_player == affected_card_controller || t_player == ANYBODY)
    {
      if (this_test != NULL)
      {
        if (new_make_test(
                player,
                get_card_instance(affected_card_controller, affected_card)->internal_card_id, -1,
                this_test))
        {
          state_untargettable(affected_card_controller, affected_card, 1);
        }
      }
      if (type)
      {
        if (is_what(affected_card_controller, affected_card, type))
        {
          state_untargettable(affected_card_controller, affected_card, 1);
        }
      }
      if (this_test == NULL && !type)
      {
        state_untargettable(affected_card_controller, affected_card, 1);
      }
    }
  }

  if (this_dies_trigger_impl_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY, player, 0, 0))
  {
    int legacy = create_legacy_effect(player, card, &remove_untargettable_legacy_2022);
    get_card_instance(player, legacy)->targets[0].player = player;
    get_card_instance(player, legacy)->token_status |= STATUS_INVISIBLE_FX;
  }

  if (trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card) &&
      reason_for_trigger_controller == player)
  {
    if ((trigger_cause_controller == t_player || t_player == ANYBODY) &&
        check_state(trigger_cause_controller, trigger_cause, STATE_CANNOT_TARGET))
    {
      if (event == EVENT_TRIGGER)
      {
        event_result |= 2;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {
        state_untargettable(trigger_cause_controller, trigger_cause, 0);
      }
    }
  }
}
''',

'2696130': r'''
int is_artifact_creature_by_internal_id_2022(int iid, int unused1, int unused2, int unused3)
{
  return is_what(-1, iid, TYPE_CREATURE) && is_what(-1, iid, TYPE_ARTIFACT) ? 1 : 0;
}
''',

'2696190': r'''
void global_type_change_2022(int player, int card, event_t event, int t_player, int added_type,
                             test_definition_2022_t *this_test, int pow, int tou, int key,
                             int subtype, int clr)
{
  if (event == EVENT_CHANGE_TYPE && in_play(player, card))
  {
    if (affected_card_controller == t_player || t_player == 2)
    {
      if (!is_what(affected_card_controller, affected_card, TYPE_EFFECT) &&
          new_make_test_in_play(affected_card_controller, affected_card, -1, this_test))
      {
        if (!(land_can_be_played & LCBP_DURING_EVENT_CHANGE_TYPE_SECOND_PASS))
        {

          land_can_be_played |= LCBP_NEED_EVENT_CHANGE_TYPE_SECOND_PASS;
        }
        else
        {
          int newtype = *(int *)(0x7a5380);
          int end = newtype + 16;

          for (; newtype < end; ++newtype)
          {
            if (cards_data[newtype].id == cards_data[event_result].id)
            {
              if (new_make_test(player, newtype, -1, this_test))
              {
                event_result = newtype;
                goto found;
              }
            }
          }

          newtype = create_a_card_type(event_result);
          if (newtype != -1)
          {
            cards_data[newtype].type |= added_type;
            if (pow == 16384)
              pow = get_cmc(affected_card_controller, affected_card);
            cards_data[newtype].power = pow;
            if (tou == 16384)
              tou = get_cmc(affected_card_controller, affected_card);
            cards_data[newtype].toughness = tou;
            cards_data[newtype].static_ability = key;
            event_result = newtype;
          }

        found:
          get_card_instance(affected_card_controller, affected_card)->token_status |=
              STATUS_ANIMATED;
          if (subtype > 0 && !has_subtype(affected_card_controller, affected_card, subtype))
          {
            add_a_subtype(affected_card_controller, affected_card, subtype);
          }
        }
      }
    }
  }

  if (event == EVENT_SET_COLOR && in_play(player, card) && clr > 0)
  {
    if (affected_card_controller == t_player || t_player == 2)
    {
      if (new_make_test_in_play(affected_card_controller, affected_card, -1, this_test))
      {
        event_result = get_sleighted_color_test_2022(player, card, clr);
      }
    }
  }
}
''',

'268cab0': r'''
int type_change_legacy_2022(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->targets[0].player < 0)
    return 0;
  int p = instance->targets[0].player;
  int c = instance->targets[0].card;
  int destroyed = 0;
  if (event == EVENT_CHANGE_TYPE && affect_me(p, c))
    event_result = instance->dummy3;
  if (instance->targets[9].player > 0)
  {
    if (instance->targets[9].player & 1)
      attached_is_forced_subtype(player, card, event, instance->targets[9].card);
    if (instance->targets[9].player & 2)
      attached_is_additional_subtype(player, card, event, instance->targets[9].card);
  }
  if (event == EVENT_STATIC_EFFECTS && instance->targets[1].player > 0)
  {
    if ((instance->targets[1].player & 2) && instance->targets[4].player >= 0 &&
        !in_play(instance->targets[4].player, instance->targets[4].card))
      destroyed = 1;
    if ((instance->targets[1].player & 4) &&
        !count_counters(p, c, (unsigned char)instance->targets[7].player))
      destroyed = 1;
  }
  if (event == EVENT_ABILITIES && affect_me(p, c) && instance->targets[8].player > 0)
    event_result |= instance->targets[8].player;
  if (event == EVENT_SET_COLOR && affect_me(p, c))
  {
    if (instance->targets[6].player > 0)
      event_result = instance->targets[6].player;
    if (instance->targets[6].card > 0)
      event_result |= instance->targets[6].card;
  }
  if (instance->targets[2].card == CARD_ID_GENJU_OF_THE_FIELDS)
    spirit_link_effect(p, c, event, p);
  if (instance->targets[8].card > 0)
    special_abilities(p, c, event, instance->targets[8].card, player, card);
  if (instance->targets[1].player > 0 && (instance->targets[1].player & 1) &&
      eot_trigger(player, card, event))
  {
    if (instance->targets[2].card == CARD_ID_CHIMERIC_COILS)
    {
      destroyed = 0;
      kill_card(p, c, KILL_SACRIFICE);
    }
    else
      destroyed = 1;
  }
  if (instance->targets[1].player > 0 && (instance->targets[1].player & 8) &&
      current_turn == instance->targets[4].player &&
      upkeep_trigger_mode_impl_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY, 0))
    destroyed = 1;
  if (instance->targets[1].player > 0 && (instance->targets[1].player & 0x20) &&
      event == EVENT_BEGIN_TURN && current_turn == player)
    destroyed = 1;
  if (instance->targets[1].player > 0 && (instance->targets[1].player & 0x10) &&
      end_of_combat_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY))
    destroyed = 1;
  if (destroyed)
  {
    remove_special_flags(p, c, SF_TYPE_ALREADY_CHANGED);
    kill_card(player, card, KILL_EXILE);
    recalculate_all_cards_in_play_2022();
  }
  return 0;
}
''',

'2693880': r'''
int turn_into_artifact_2022(int player, int card, int t_player, int t_card, int mode)
{
  int legacy = -1;
  if (t_player != -1 && t_card != -1)
  {
    card_instance_t *instance = get_card_instance(t_player, t_card);
    int newtype = create_a_card_type(instance->internal_card_id);
    cards_at_7c7000[newtype]->type |= (cards_data[instance->internal_card_id].type | TYPE_ARTIFACT);
    legacy =
        create_targetted_legacy_effect(player, card, &type_change_legacy_2022, t_player, t_card);
    card_instance_t *leg = get_card_instance(player, legacy);
    leg->dummy3 = newtype;
    leg->token_status |= STATUS_PERMANENT;
    leg->targets[1].player = mode;
  }
  return legacy;
}
''',

'2694d70': r'''
int turn_into_creature_2022(int player, int card, int t_player, int t_card, int mode, int pow,
                            int tou)
{

  int legacy = -1;
  if (t_player != -1 && t_card != -1)
  {
    card_instance_t *instance = get_card_instance(t_player, t_card);
    int newtype = create_a_card_type(instance->internal_card_id);
    cards_at_7c7000[newtype]->type |= (cards_data[instance->internal_card_id].type | TYPE_CREATURE);
    if (pow != 0)
    {
      cards_at_7c7000[newtype]->power = pow != -1 ? pow : get_cmc(t_player, t_card);
    }
    if (tou != 0)
    {
      cards_at_7c7000[newtype]->toughness = tou != -1 ? tou : get_cmc(t_player, t_card);
    }
    legacy = create_targetted_legacy_effect_no_repeat(player, card, &type_change_legacy_2022,
                                                      t_player, t_card);
    card_instance_t *leg = get_card_instance(player, legacy);
    leg->dummy3 = newtype;
    leg->token_status |= STATUS_PERMANENT;
    leg->targets[1].player = mode;
  }
  return legacy;
}
''',

'2695040': r'''
int artifact_animation_2022(int player, int card, int t_player, int t_card, int mode, int pow,
                            int tou, int abilities, int sp_abilities)
{

  int legacy_card = turn_into_creature_2022(player, card, t_player, t_card, mode, pow, tou);
  card_instance_t *legacy_instance = get_card_instance(player, legacy_card);
  legacy_instance->targets[2].card = get_id(player, card);
  legacy_instance->targets[8].player = abilities;
  legacy_instance->targets[8].card = sp_abilities;
  return legacy_card;
}
''',

'2696490': r'''
int global_type_animation_legacy_2022(int player, int card, event_t event)
{

  card_instance_t *instance = get_card_instance(player, card);

  if (instance->targets[0].player > -1)
  {

    test_definition_2022_t this_test;
    default_test_definition(&this_test, instance->targets[4].player);
    this_test.type_flag = instance->targets[4].card;
    this_test.subtype = instance->targets[5].player;
    this_test.subtype_flag = instance->targets[5].card;
    this_test.color = instance->targets[6].player;
    this_test.color_flag = instance->targets[6].card;
    this_test.cmc = instance->targets[7].player;
    this_test.cmc_flag = instance->targets[7].card;

    global_type_change_2022(player, card, event, instance->targets[0].player, TYPE_CREATURE,
                            &this_test, instance->targets[1].player, instance->targets[1].card,
                            instance->targets[2].player, instance->targets[3].card,
                            instance->targets[3].player);

    if (instance->targets[0].card > -1 && (instance->targets[0].card & 1))
    {
      if (eot_trigger(player, card, event))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
  }

  return 0;
}
''',

'2694ea0': r'''
int real_land_animation2_2022(int player, int card, int t_player, int t_card, int mode, int pow,
                              int tou, int key, int s_key, int clr)
{

  int legacy = turn_into_creature_2022(player, card, t_player, t_card, mode, pow, tou);
  card_instance_t *leg = get_card_instance(player, legacy);
  leg->targets[1].player = mode;

  leg->targets[2].card = get_id(player, card);
  leg->targets[4].player = player;
  leg->targets[4].card = card;
  leg->targets[6].player = clr;
  leg->targets[8].player = key;
  leg->targets[8].card = s_key;
  return legacy;
}
''',

'2694f40': r'''
int land_animation2_2022(int player, int card, int t_player, int t_card, int mode, int pow, int tou,
                         int key, int s_key, int clr, test_definition_2022_t *this_test)
{
  if (t_card != -1)
    return real_land_animation2_2022(player, card, t_player, t_card, mode, pow, tou, key, s_key,
                                     clr);
  int legacy = create_legacy_effect(player, card, &global_type_animation_legacy_2022);
  card_instance_t *leg = get_card_instance(player, legacy);
  leg->targets[0].player = t_player;
  leg->targets[0].card = mode;
  leg->targets[1].player = pow;
  leg->targets[1].card = tou;
  leg->targets[2].player = key;
  leg->targets[2].card = s_key;
  leg->targets[3].player = clr;
  leg->targets[3].card = -1;
  leg->targets[4].player = this_test->type;
  leg->targets[4].card = this_test->type_flag;
  leg->targets[5].player = this_test->subtype;
  leg->targets[5].card = this_test->subtype_flag;
  /* Native stores color_flag in color's slot; other filter slots remain untouched. */
  leg->targets[6].player = this_test->color_flag;
  leg->targets[7].card = this_test->cmc_flag;
  return legacy;
}
''',

'26928c0': r'''
int card_generic_animated_land_2022(int player, int card, event_t event)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (instance->targets[5].player > 0)
  {
    instance->display_pic_csv_id = instance->targets[5].player;
  }
  if (instance->targets[5].card > 0)
  {
    instance->display_pic_num = instance->targets[5].card;
  }
  if (event == EVENT_POWER && affect_me(player, card) && instance->targets[7].player > 0)
  {
    set_base_pt(instance->targets[7].player - 1);
  }
  if (event == EVENT_TOUGHNESS && affect_me(player, card) && instance->targets[7].card > 0)
  {
    set_base_pt(instance->targets[7].card - 1);
  }
  if (event == EVENT_ABILITIES && affect_me(player, card) && instance->targets[8].player > 0)
  {
    event_result |= instance->targets[8].player;
  }
  if (event == EVENT_SET_COLOR && affect_me(player, card) && instance->targets[6].player > 0)
  {
    event_result = instance->targets[6].player;
  }
  if (instance->targets[8].card > 0)
  {
    special_abilities(player, card, event, instance->targets[8].card, player, card);
  }
  int result = 0;
  if (instance->targets[6].card > -1)
  {
    int (*ptFunction)(int, int, event_t) = (void *)instance->targets[6].card;
    result = ptFunction(player, card, event);
  }
  return result;
}
''',

'267f5e0': r'''
void land_animation_2022(int player, int card)
{

  card_instance_t *instance = get_card_instance(player, card);

  if (instance->targets[13].player == instance->targets[13].card)
  {
    int id = instance->targets[13].player;
    id++;
    instance->targets[12].card = get_internal_card_id_from_csv_id(id);
    instance->targets[13].card = id;
  }
  else
  {
    int id = instance->targets[13].player;
    instance->targets[12].card = get_internal_card_id_from_csv_id(id);
    instance->targets[13].card = id;
  }
}
''',

'2695570': r'''
void true_transform_2022(int player, int card)
{

  card_instance_t *instance = get_card_instance(player, card);

  int id = instance->targets[13].player;
  if (instance->targets[13].player == instance->targets[13].card)
  {
    id++;
  }
  instance->targets[12].card = get_internal_card_id_from_csv_id(id);
  instance->targets[13].card = id;
}
''',

'269db70': r'''
int manland_shared_2022(int player, int card, event_t event, int colorless, int black, int blue,
                        int green, int red, int white)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    set_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
    int result = granted_generic_activated_ability_2022(player, card, player, card, event,
                                                        GAA_TYPE_CHANGE, colorless, black, blue,
                                                        green, red, white, 0, NULL, NULL);
    if (result && (get_id(player, card) != CARD_ID_NANTUKO_MONASTERY || has_threshold(player)))
    {
      remove_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
      return 1;
    }
    remove_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
  }
  else if (event == EVENT_ACTIVATE)
  {
    instance->info_slot = 0;
    int choice = 1;
    if (mana_producer_2022(player, card, EVENT_CAN_ACTIVATE))
    {
      set_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
      choice = 0;
      if (unknown62BCEC != 0 && needed_mana_colors == 0 &&
          granted_generic_activated_ability_2022(player, card, player, card, EVENT_CAN_ACTIVATE,
                                                 GAA_TYPE_CHANGE, colorless, black, blue, green,
                                                 red, white, 0, NULL, NULL) &&
          (get_id(player, card) != CARD_ID_NANTUKO_MONASTERY || has_threshold(player)))
        choice = do_dialog(player, player, card, -1, -1, " Generate Mana\n Animate\n Cancel", 1);
      remove_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
    }
    if (choice == 1)
    {
      set_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
      if (granted_generic_activated_ability_2022(player, card, player, card, event, GAA_TYPE_CHANGE,
                                                 colorless, black, blue, green, red, white, 0, NULL,
                                                 NULL))
        instance->info_slot = 66;
      remove_special_flags(player, card, SF_MANA_PRODUCER_DISABLED_FOR_AI);
      return 0;
    }
    if (choice != 0)
    {
      spell_fizzled = 1;
      return 0;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION && instance->info_slot == 66 &&
           instance->targets[13].player == instance->targets[13].card)
  {
    get_card_instance(player, instance->parent_card)->token_status |= STATUS_ANIMATED;
    true_transform_2022(player, instance->parent_card);
    return 0;
  }
  return mana_producer_2022(player, card, event);
}
''',

'269df80': r'''
int manland_animated_2022(int player, int card, event_t event, int colorless, int black, int blue,
                          int green, int red, int white)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (get_id(player, card) != CARD_ID_STALKING_STONES_ANIMATED && eot_trigger(player, card, event))
  {
    remove_status(player, card, STATUS_ANIMATED);
    remove_special_flags(player, card, SF_TYPE_ALREADY_CHANGED);
    true_transform_2022(player, card);
  }
  else if (event == EVENT_SET_COLOR && affect_me(player, card))
  {
    event_result = cards_data[instance->internal_card_id].color;
  }
  return manland_shared_2022(player, card, event, colorless, black, blue, green, red, white);
}
''',

'269e0b0': r'''
int manland_normal_2022(int player, int card, event_t event, int colorless, int black, int blue,
                        int green, int red, int white)
{
  double_faced_card(player, card, event);
  return manland_shared_2022(player, card, event, colorless, black, blue, green, red, white);
}
''',

'267f030': r'''
void cloning_2022(int player, int card, int t_player, int t_card)
{
  card_instance_t *instance = get_card_instance(player, card);
  if (t_player == -1)
  {
    instance->targets[12].card = t_card;
  }
  else
  {
    card_instance_t *target = get_card_instance(t_player, t_card);
    int iid = target->internal_card_id;
    if (iid == -1)
      iid = target->backup_internal_card_id;

    switch (get_id(player, card))
    {
    case CARD_ID_PHYREXIAN_METAMORPH:
      if (!(cards_data[iid].type & TYPE_ARTIFACT))
      {
        iid = create_a_card_type(iid);
        cards_data[iid].type |= TYPE_ARTIFACT;
      }
      break;

    case CARD_ID_COPY_ARTIFACT:
      if (!(cards_data[iid].type & TYPE_ENCHANTMENT) || cards_data[iid].cc[2] == 9)
      {
        iid = create_a_card_type(iid);
        cards_data[iid].type |= TYPE_ENCHANTMENT;
      }
      break;
    }
    if (target->targets[13].player > -1 && target->targets[13].card > -1)
    {
      if (t_player != -1 && check_status(t_player, t_card, STATUS_ANIMATED))
      {
        instance->targets[13].player =
            cards_data[get_original_internal_card_id(t_player, t_card)].id;
        instance->targets[13].card =
            cards_data[get_original_internal_card_id(t_player, t_card)].id + 1;
      }
      else
      {
        instance->targets[13].player = target->targets[13].player;
        instance->targets[13].card = target->targets[13].card;
      }
    }
    if (target->targets[12].player > -1)
    {
      if (t_player != -1 && check_status(t_player, t_card, STATUS_ANIMATED))
      {
        instance->targets[12].player =
            cards_data[get_original_internal_card_id(t_player, t_card)].id;
      }
      else
      {
        instance->targets[12].player = target->targets[12].player;
      }
    }
    if (t_player != -1 && check_status(t_player, t_card, STATUS_ANIMATED))
    {
      instance->targets[12].card = get_original_internal_card_id(t_player, t_card);
    }
    else
    {
      instance->targets[12].card = iid;
    }
    instance->initial_color = 0;
    copy_token_characteristics_for_clone_2022(player, card, t_player, t_card);
  }
}
''',

'267f2d0': r'''
void cloning_and_verify_legend_2022(int player, int card, int t_player, int t_card)
{
  cloning_2022(player, card, t_player, t_card);

  card_instance_t *instance = get_card_instance(player, card);
  instance->regen_status |= KEYWORD_RECALC_CHANGE_TYPE;
  get_abilities(player, card, EVENT_CHANGE_TYPE, -1);
  if (in_play(player, card))
    verify_legend_rule(player, card, get_id(player, card));
}
''',

'267f570': r'''
int enters_the_battlefield_as_copy_of_any_creature_2022(int player, int card, event_t event)
{
  target_definition_2022_t td;
  if (event == EVENT_RESOLVE_SPELL)
  {
    base_target_definition(player, card, &td, TYPE_CREATURE);
    td.preferred_controller = ANYBODY;
  }
  return enters_the_battlefield_as_copy_of_any(player, card, event, &td, "SELECT_A_CREATURE");
}
''',

'2686b30': r'''
int shapeshift_target_2022(int src_player, int src_card, int t_player, int t_card,
                           int turn_into_player, int turn_into_card, int mode)
{
  int legacy = real_shapeshift_target_2022(src_player, src_card, t_player, t_card, turn_into_player,
                                           turn_into_card, mode);

  if (in_play(t_player, t_card))
    verify_legend_rule(t_player, t_card, get_id(t_player, t_card));

  return legacy;
}
''',

}
