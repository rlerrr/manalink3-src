"""Four 2022 card/legacy families, preserving older card entry points."""

BODIES = {
'2340190': r'''
void bruna_ability(int player, int card)
{
  while (check_targets_for_bruna(player, card, 0) != 0) {
    if (player == 0) {
      target_definition_2022_t td;
      base_target_definition_2022(0, card, &td, 4);
      td.required_subtype = 0x4002;
      card_instance_t *instance = get_card_instance(0, card);
      instance->number_of_targets = 0;
      if (!pick_next_target_noload_2022(&td, "Select an Aura card with enchant creature.")) {
        spell_fizzled = 0;
        break;
      }
      card_instance_t *aura = get_card_instance(instance->targets[0].player, instance->targets[0].card);
      aura->damage_target_player = 0;
      aura->damage_target_card = card;
    } else {
      int result = check_targets_for_bruna(player, card, 1);
      if (result >= 0) {
        card_instance_t *aura = get_card_instance(1 - player, result);
        aura->damage_target_player = player;
        aura->damage_target_card = card;
      }
    }
  }
  test_definition_2022_t this_test;
  new_default_test_definition_2022(&this_test, 4, "Select an Aura card with enchant creature.");
  this_test.subtype = 0x4002;
  while (new_special_count_grave_2022(player, &this_test)) {
    int result = new_global_tutor_2022(player, player, 2, 1, 0, 2, &this_test);
    if (result < 0) break;
    put_into_play_aura_attached_to_target_2022(player, result, player, card);
  }
  this_test.zone = 0x100;
  while (check_battlefield_for_special_card_2022(player, card, player, 0, &this_test)) {
    int result = new_select_a_card_2022(player, player, 4, 0, 2, -1, &this_test);
    if (result < 0) break;
    put_into_play_aura_attached_to_target_2022(player, result, player, card);
  }
}
''',
'234ffc0': r'''
int card_bruna_light_of_alabaster_2022(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY) bruna_ability(player, card);
  if (blocking(player, card, event) && !is_humiliated_for_ability(player, card, AL_TRIG)) bruna_ability(player, card);
  return 0;
}
''',
'24bad70': r'''
const char *is_damaging_stored_creature(int who_chooses, int player, int card)
{
  card_instance_t *dmg = get_card_instance(player, card);
  if (dmg->internal_card_id == damage_card && dmg->damage_target_player == aff_player &&
      dmg->damage_target_card == aff_card && dmg->info_slot > 0) return NULL;
  return "must damage Personal Incarnation";
}
''',
'24bc2e0': r'''
int effect_personal_incarnation(int player, int card, event_t event)
{
  if (event == EVENT_STATIC_EFFECTS) {
    if (affect_me(player, card)) {
      card_instance_t *instance = get_card_instance(player, card);
      if (instance->targets[9].player >= 0) {
        if (get_id(instance->targets[9].player, instance->targets[9].card) != 0xb5) kill_card(player, card, KILL_EXILE);
        if (!in_play(instance->targets[9].player, instance->targets[9].card)) kill_card(player, card, KILL_EXILE);
      }
    }
  } else if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION || event == EVENT_CLEANUP) {
    if (get_card_instance(player, card)->targets[9].player != -1) {
      card_instance_t *instance = get_card_instance(player, card);
      int p = instance->targets[9].player, c = instance->targets[9].card;
      target_definition_2022_t td;
      default_target_definition_2022(player, card, &td, 0x80);
      td.special = 0x8000;
      td.extra = (int32_t)(intptr_t)is_damaging_stored_creature;
      aff_card = c; aff_player = p;
      if (event == EVENT_RESOLVE_ACTIVATION && valid_target_2022(&td)) {
        card_instance_t *dmg = get_card_instance(instance->targets[0].player, instance->targets[0].card);
        int owner = get_owner(p, c);
        prevent_the_next_n_damage(p, c, dmg->damage_target_player, dmg->damage_target_card, 1, PREVENT_REDIRECT, owner, -1);
      }
      return generic_activated_ability_2022(player, card, event, 0x1000004, 0, 0, 0, 0, 0, 0, 0, &td, "TARGET_DAMAGE");
    }
  }
  return 0;
}
''',
'24ca360': r'''
int card_personal_incarnation_2022(int player, int card, event_t event)
{
  if (this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY) && life[get_owner(player, card)] > 0) {
    uint32_t amount = round_up_value(life[get_owner(player, card)]);
    lose_life_2022(get_owner(player, card), amount);
  }
  if (event == EVENT_STATIC_EFFECTS) {
    if (get_owner(player, card) != player && affect_me(player, card)) {
      int found = 0;
      for (int p = 0; p != 2; ++p) {
        for (int c = 0; c <= active_cards_count[p]; ++c) {
          if (in_play(p, c) && is_what(p, c, 0x80)) {
            card_instance_t *instance = get_card_instance(p, c);
            if (instance->info_slot_fn == effect_personal_incarnation &&
                instance->targets[9].player == player && instance->targets[9].card == card) {
              if (p == player) kill_card(player, c, KILL_EXILE);
              else { if (p == 1) return 0; found = 1; }
            }
          }
        }
      }
      if (!found) {
        card_instance_t *instance = get_card_instance(player, card);
        int p = 1 - player;
        int fake = add_card_to_hand(p, instance->internal_card_id);
        int legacy = create_legacy_effect_exe(p, fake, EXE_DWORD(0x739d5c), -1, -1);
        card_instance_t *leg = get_card_instance(p, legacy);
        leg->info_slot_fn = effect_personal_incarnation;
        leg->targets[9].player = player; leg->targets[9].card = card;
        leg->number_of_targets = 1;
        obliterate_card(p, fake);
      }
    }
  } else if ((event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION || event == EVENT_CLEANUP) && get_owner(player, card) == player) {
    target_definition_2022_t td;
    default_target_definition_2022(player, card, &td, 0x80);
    td.special = 0x8000;
    td.extra = (int32_t)(intptr_t)is_damaging_attached_creature;
    td.illegal_abilities = 0;
    card_instance_t *instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION && valid_target_2022(&td)) {
      card_instance_t *dmg = get_card_instance(instance->targets[0].player, instance->targets[0].card);
      prevent_the_next_n_damage(instance->parent_controller, instance->parent_card,
          dmg->damage_target_player, dmg->damage_target_card, 1, PREVENT_REDIRECT, instance->parent_controller, -1);
    }
    return generic_activated_ability_2022(player, card, event, 0x1000004, 0, 0, 0, 0, 0, 0, 0, &td, "TARGET_DAMAGE");
  }
  return 0;
}
''',
'25686b0': r'''
int toshiro_effect(int player, int card, event_t event)
{
  if (event == EVENT_STATIC_EFFECTS) {
    if (affect_me(player, card)) {
      card_instance_t *instance = get_card_instance(player, card);
      if (instance->damage_target_player >= 0 && get_id(instance->damage_target_player, instance->damage_target_card) != 0x1da0) kill_card(player, card, KILL_EXILE);
    }
    card_instance_t *instance = get_card_instance(player, card);
    if (instance->damage_target_player >= 0 && effect_follows_control_of_attachment(player, card, 0x4700)) return 0;
  } else {
    card_instance_t *instance = get_card_instance(player, card);
    if (instance->damage_target_player >= 0 && effect_follows_control_of_attachment(player, card, event)) return 0;
    if (event == EVENT_GRAVEYARD_FROM_PLAY) {
      instance = get_card_instance(player, card);
      if (in_play(affected_card_controller, affected_card) && is_this_dying(affected_card_controller, affected_card)) {
        if (instance->damage_target_card == affected_card && instance->damage_target_player == affected_card_controller) {
          instance->damage_target_card = -1; instance->damage_target_player = -1;
          remove_status(player, card, 0x10000);
          if (instance->targets[11].player < 0) instance->targets[11].player = 0;
          ((uint8_t *)&instance->targets[11].player)[2] = 1;
        }
        count_for_gfp_ability_2022(player, card, 0x77, 1 - player, 2, NULL);
      }
    }
  }
  int result = resolve_gfp_ability_2022(player, card, event, (duh_mode(player) != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (result && !check_status(player, card, 0x10000)) {
    unsigned amount = (uint32_t)result & 0xff;
    for (;;) {
      int playable[2][amount];
      int pc = 0;
      const int *grave = get_grave(player);
      for (int i = 0; grave[i] != -1; ++i) {
        if (is_what(-1, grave[i], 0x30) && can_play_iid(player, event, grave[i])) {
          playable[0][pc] = grave[i]; playable[1][pc] = i;
          if (++pc == amount) break;
        }
      }
      if (pc == 0) continue;
      test_definition_2022_t this_test;
      new_default_test_definition_2022(&this_test, 0xff, "Select an Instant to play");
      int selected = select_card_from_zone_2022(player, player, playable[0], pc, 0, 1, -1, &this_test);
      if (selected == -1) break;
      if (!charge_mana_from_id(player, card, event, (uint16_t)cards_data[playable[0][selected]].id)) continue;
      play_card_in_grave_for_free_and_exile_it(player, player, playable[1][selected]);
      if (--pc <= 0) break;
    }
    kill_card(player, card, KILL_EXILE);
  }
  if (event == EVENT_CLEANUP && !check_status(player, card, 0x10000)) kill_card(player, card, KILL_EXILE);
  return 0;
}
''',
'25705a0': r'''
int card_toshiro_umezawa_2022(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS) {
    if (affect_me(player, card)) {
      int found = 0;
      for (int c = 0; c < active_cards_count[player]; ++c) {
        if (in_play(player, c) && is_what(player, c, 0x80)) {
          card_instance_t *instance = get_card_instance(player, c);
          if (instance->info_slot_fn == toshiro_effect &&
              ((instance->damage_target_player == player && instance->damage_target_card == card) ||
               (instance->targets[0].player == player && instance->targets[0].card == card))) { found = 1; break; }
        }
      }
      if (!found) {
        int legacy = create_targetted_legacy_effect(player, card, toshiro_effect, player, card);
        card_instance_t *instance = get_card_instance(player, legacy);
        instance->targets[0].player = player; instance->targets[0].card = card;
        instance->number_of_targets = 1;
        add_status(player, legacy, 0x10000);
      }
    }
  } else if (event == EVENT_GRAVEYARD_FROM_PLAY) count_for_gfp_ability_2022(player, card, 0x77, 1 - player, 2, NULL);
  int amount = resolve_gfp_ability_2022(player, card, event, (duh_mode(player) != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (amount && !graveyard_has_shroud(player)) {
    for (;;) {
      int playable[2][amount];
      int pc = 0;
      const int *grave = get_grave(player);
      for (int i = 0; grave[i] != -1; ++i) {
        if (is_what(-1, grave[i], 0x30) && can_play_iid(player, event, grave[i])) {
          playable[0][pc] = grave[i]; playable[1][pc] = i;
          if (++pc == amount) break;
        }
      }
      if (pc == 0) continue;
      test_definition_2022_t this_test;
      new_default_test_definition_2022(&this_test, 0xff, "Select an Instant to play");
      int selected = select_card_from_zone_2022(player, player, playable[0], pc, 0, 1, -1, &this_test);
      if (selected == -1) break;
      if (!charge_mana_from_id(player, card, event, (uint16_t)cards_data[playable[0][selected]].id)) continue;
      play_card_in_grave_for_free_and_exile_it(player, player, playable[1][selected]);
      if (--pc <= 0) break;
    }
  }
  return bushido(player, card, event, 1);
}
''',
'2571c90': r'''
int sehts_tiger_legacy(int player, int card, event_t event)
{
  if (in_play(player, card) && !is_humiliated_for_ability(player, card, AL_STATIC)) player_bits[player] |= 0x4000000;
  if (event == EVENT_PROT_MISC) {
    if (affected_card_controller == player && affected_card == -1) {
      if (((uint32_t)event_result >> 24) != 6) {
        uint32_t color = get_color(attacking_card_controller, attacking_card);
        if ((get_card_instance(player, card)->targets[1].card & color) == 0) return 0;
      }
      event_result |= 1;
    }
  } else if (event == EVENT_BEGIN_TURN) kill_card(player, card, KILL_EXILE);
  return 0;
}
''',
'257a9d0': r'''
int card_sehts_tiger_2022(int player, int card, event_t event)
{
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB) {
    int legacy = create_legacy_effect(player, card, sehts_tiger_legacy);
    int color = get_deck_color(player, 1 - player);
    color = choose_a_color(player, color);
    get_card_instance(player, legacy)->targets[1].card = 1u << ((uint32_t)color & 31);
  } else if (event == EVENT_CLEANUP) get_card_instance(player, card)->info_slot = 0;
  return flash(player, card, event);
}
'''
}
