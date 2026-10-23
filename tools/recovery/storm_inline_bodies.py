"""Source reconstructions of two DWARF inline routines in the native Rules Engine."""

RESET=r'''static void reset_storm_count_2022(int player, int card)
{
  enum { RULES_ENGINE_CSV_ID_2022 = 0x615a };
  target_definition_2022_t td;

  if (player != HUMAN)
    return;

  base_target_definition_2022(HUMAN, card, &td, TYPE_CREATURE);
  td.required_subtype = SUBTYPE_WEREWOLF;
  if (!can_target_2022(&td))
  {
    for (int p = 0; p < 2; ++p)
    {
      int rec = rules_engine_card[p];
      if (rec >= 0)
      {
        increase_trap_condition_impl(p, 0x403, 0, 0);
        increase_trap_condition_impl(p, 0x404, 0, 0);
        increase_trap_condition_impl(p, 0x405, 0, 0);
        increase_trap_condition_impl(p, 0x406, 0, 0);
        increase_trap_condition_impl(p, 0x407, 0, 0);
        remove_all_counters_from_source(HUMAN, card, p, rec, COUNTER_VORTEX);
        remove_all_counters_from_source(HUMAN, card, p, rec, COUNTER_BLOOD);
        remove_all_counters_from_source(HUMAN, card, p, rec, COUNTER_CHARGE);
        remove_all_counters_from_source(HUMAN, card, p, rec, COUNTER_DEVOTION);
      }
    }
  }
  else
  {
    int human_count = rules_engine_card[HUMAN] == -1 ? 0 :
        count_counters(HUMAN, rules_engine_card[HUMAN], COUNTER_VORTEX);
    int ai_count = rules_engine_card[AI] == -1 ? 0 :
        count_counters(AI, rules_engine_card[AI], COUNTER_VORTEX);
    for (int p = 0; p < 2; ++p)
    {
      for (int c = 0; c < active_cards_count[p]; ++c)
      {
        if (in_play(p, c))
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (has_subtype(p, c, SUBTYPE_WEREWOLF) && instance->targets[9].card != 0x42)
          {
            instance->targets[9].card = 0x42;
            instance->targets[8].player = human_count;
            instance->targets[8].card = ai_count;
          }
          if (get_id(p, c) == RULES_ENGINE_CSV_ID_2022)
          {
            increase_trap_condition_impl(p, 0x403, 0, 0);
            increase_trap_condition_impl(p, 0x404, 0, 0);
            increase_trap_condition_impl(p, 0x405, 0, 0);
            increase_trap_condition_impl(p, 0x406, 0, 0);
            increase_trap_condition_impl(p, 0x407, 0, 0);
            remove_all_counters_from_source(HUMAN, card, p, c, COUNTER_VORTEX);
            remove_all_counters_from_source(HUMAN, card, p, c, COUNTER_BLOOD);
            remove_all_counters_from_source(HUMAN, card, p, c, COUNTER_CHARGE);
            remove_all_counters_from_source(HUMAN, card, p, c, COUNTER_DEVOTION);
          }
        }
      }
    }
  }
}
'''

KEEP=r'''static void keep_storm_count_2022(int player, int card, event_t event)
{
  enum { TRAP_NONCREATURES_PLAYED_2022 = 0x408 };
  int not_cast;
  if (player != HUMAN)
    return;

  if (trigger_condition == TRIGGER_SPELL_CAST && card == affected_card &&
      affected_card_controller == HUMAN && reason_for_trigger_controller == HUMAN &&
      (not_cast = check_special_flags_2022(trigger_cause_controller, trigger_cause, SF_NOT_CAST),
       event == EVENT_END_TRIGGER && !not_cast))
  {
    card_instance_t *played = get_card_instance(trigger_cause_controller, trigger_cause);
    uint8_t types = cards_data[played->internal_card_id].type;
    if (!(types & TYPE_LAND))
    {
      int mode;
      if (types & TYPE_CREATURE)
        mode = STORM2022_CREATURE;
      else
      {
        increase_trap_condition_impl(trigger_cause_controller, TRAP_NONCREATURES_PLAYED_2022, 1, 1);
        mode = 0;
      }
      if (types & TYPE_ENCHANTMENT)
        mode |= STORM2022_ENCHANTMENT;
      if ((types & TYPE_SPELL) && !(types & TYPE_CREATURE))
        mode |= STORM2022_INSTANT_SORCERY;
      if (get_id(trigger_cause_controller, trigger_cause) == CARD_ID_PEER_THROUGH_DEPTHS)
        increase_trap_condition_impl(trigger_cause_controller, TRAP_PEER_THROUGH_DEPTHS, 1, 1);
      if (get_id(trigger_cause_controller, trigger_cause) == CARD_ID_REACH_THROUGH_MISTS)
        increase_trap_condition_impl(trigger_cause_controller, TRAP_REACH_THROUGH_MISTS, 1, 1);
      unsigned int colors = get_color(trigger_cause_controller, trigger_cause);
      increment_storm_count_2022(trigger_cause_controller, mode | (colors & COLOR_TEST_ANY_COLORED));
    }
  }
  else if (event == EVENT_CAN_SKIP_TURN)
    reset_storm_count_2022(player, card);
}
'''
