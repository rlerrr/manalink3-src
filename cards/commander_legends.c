#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a2a30; original cards/commander_legends.c:94. */
int card_aesi_tyrant_of_gyre_strait(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  result = duh_mode(player);
  landfall_mode(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    draw_a_card(player);
  }
  check_playable_lands(player);
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x25a1f80; original cards/commander_legends.c:653. */
static char *aura_or_equipment(int who_chooses, int player, int card)
{
  int result;
  result = has_subtype(player, card, SUBTYPE_EQUIPMENT);
  if ((result != 0) && (result = is_what(player, card, 2), result == 0))
  {
    return NULL;
  }
  result = has_subtype(player, card, SUBTYPE_AURA);
  if (result == 0)
  {
    return "must be an Equipment or Aura.";
  }
  if ((player == 1) && (result = has_subtype(1, card, SUBTYPE_AURA_PERMANENT), result == 0))
  {
    return "must be an Equipment or Aura.";
  }
  return NULL;
}

/* ManalinkEh 2022: 0x25a4850; original cards/commander_legends.c:947. */
int card_bladegriff_prototype(int player, int card, event_t event)
{
  unsigned int uVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  int *piStack_74;
  int iStack_70;
  int times_damaged[2];
  target_definition_t td;
  iVar2 = damage_dealt_by_me(player, card, event,
                             DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                 DDBM_MUST_BE_COMBAT_DAMAGE);
  if (iVar2 != 0)
  {
    instance = get_card_instance(player, card);
    iStack_70 = 0;
    uVar1 = instance->targets[1].player;
    instance->targets[1].player = 0;
    times_damaged[1] = uVar1 >> 8 & 0xff;
    iVar2 = 1 - player;
    times_damaged[0] = uVar1 & 0xff;
    piStack_74 = times_damaged;
    do
    {
      default_target_definition(player, card, &td, 0x47);
      td.illegal_type = 1;
      td.allow_cancel = 0;
      iVar5 = *piStack_74;
      td.allowed_controller = iVar2;
      td.preferred_controller = iVar2;
      td.who_chooses = iStack_70;
      while ((iVar5 != 0 && (iVar4 = can_target(&td), iVar4 != 0)))
      {
        if (iVar2 == iStack_70)
        {
          new_pick_target(&td, "Select target nonland permanent you control.", 0, 0x80);
        }
        else
        {
          new_pick_target(&td, "Select target nonland permanent your opponent controls.", 0, 0x80);
        }
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        iVar5 = iVar5 + -1;
        instance->number_of_targets = '\0';
        *piStack_74 = iVar5;
      }
      iStack_70 = iStack_70 + 1;
      piStack_74 = piStack_74 + 1;
    } while (iStack_70 != 2);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a4ee0; original cards/commander_legends.c:1118. */
int card_breeches_brazen_plunderer(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  check_legend_rule(player, card, event);
  result = subtype_deals_damage(player, card, event, player, 0x95, 0x12);
  if (result == 0)
  {
    return 0;
  }
  default_test_definition(&this_test, 0xff);
  exile_top_x_card_you_may_play_them(player, card, 1, &this_test, 1 - player,
                                     MPCFE_UNTIL_EOT | MPCFE_FOR_CMC);
  return 0;
}

/* ManalinkEh 2022: 0x25a0730; original cards/commander_legends.c:1163. */
static void brinelin_ability(int player, int card)
{
  int result;
  card_instance_t *instance;
  target_definition_t td1;

  default_target_definition(player, card, &td1, 0x47);
  td1.illegal_type = 1;
  result = can_target(&td1);
  if (result != 0)
  {
    result = new_pick_target(&td1, "Select target nonland permanent.", 0, 0x80);
    if (result != 0)
    {
      instance = get_card_instance(player, card);
      bounce_permanent(instance->targets[0].player, instance->targets[0].card);
    }
  }
  instance = get_card_instance(player, card);
  instance->number_of_targets = '\0';
  return;
}

/* ManalinkEh 2022: 0x25a0360; original cards/commander_legends.c:1588. */
static char *is_commander(int who_chooses, int player, int card, int targeting_player,
                          int targeting_card)
{
  int result;
  char *pcVar2;
  result = check_special_flags2_2022(player, card, 0x1000000);
  pcVar2 = NULL;
  if (result == 0)
  {
    pcVar2 = "must be a Commander.";
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x25a6e60; original cards/commander_legends.c:1944. */
int card_demonic_lore(int player, int card, event_t event)
{
  int result;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_cards(player, 3);
  }
  if ((current_turn == player) && (result = eot_trigger(player, card, event), result != 0))
  {
    lose_life(player, hand_count[player] * 2);
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x25a71e0; original cards/commander_legends.c:2048. */
int card_eyeblight_cullers(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  default_token_definition(player, card, 0x492c, &token);
  token.qty = '\x03';
  generate_token(&token);
  mill(player, 3);
  return 0;
}

/* ManalinkEh 2022: 0x25a8430; original cards/commander_legends.c:2437. */
int card_guildless_commons(int player, int card, event_t event)
{
  int result;
  result = karoo(player, card, event, 0, 0, 0x3d67);
  return result;
}

/* ManalinkEh 2022: 0x259ff40; original cards/commander_legends.c:2847. */
int jared_carthalion_effect(int player, int card, event_t event)
{
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x25a12d0; original cards/commander_legends.c:3543. */
static char *attacking_the_same_player_or_planeswalker(int who_chooses, int player, int card,
                                                       int targeting_player, int targeting_card)
{
  int iVar1;
  int iVar2;
  char *pcVar3;

  iVar1 = planeswalker_its_attacking(targeting_player, targeting_card);
  iVar2 = planeswalker_its_attacking(player, card);
  pcVar3 = NULL;
  if (iVar1 != iVar2)
  {
    pcVar3 = "target creature must attack the same player or planeswalker";
  }
  return pcVar3;
}

/* ManalinkEh 2022: 0x25ac660; original cards/commander_legends.c:3929. */
int card_malcolm_keen_eyed_navigator(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  check_legend_rule(player, card, event);
  result = subtype_deals_damage(player, card, event, player, 0x95, 6);
  if (result == 0)
  {
    return 0;
  }
  generate_token_by_id_2022(player, card, 0x4998);
  instance = get_card_instance(player, card);
  instance->targets[1].card = 0;
  return 0;
}

/* ManalinkEh 2022: 0x25ac6f0; original cards/commander_legends.c:3947. */
int card_merchant_raiders(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td1;

  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &recovered_stack_td1.target, 2);
    pcVar2 = get_card_instance(player, card);
    result = can_target(&recovered_stack_td1.target);
    if (result != 0)
    {
      result = new_pick_target(&recovered_stack_td1.target, "TARGET_CREATURE", 0, 0);
      if (result != 0)
      {
        tap_card(pcVar2->targets[0].player, pcVar2->targets[0].card);
        result = create_targetted_legacy_effect(player, card, dungeon_geist_effect,
                                                pcVar2->targets[0].player, pcVar2->targets[0].card);
        pcVar3 = get_card_instance(player, result);
        pcVar3->targets[0].player = player;
        pcVar3->targets[0].card = card;
        pcVar3->number_of_targets = '\x01';
      }
    }
    pcVar2->number_of_targets = '\0';
  }
  if ((trigger_condition == 0xdb) && (trigger_cause_controller == player))
  {
    default_test_definition(&recovered_stack_td1.test, 0x47);
    etb_ability_global(player, event, 2, TYPE_NONE, &recovered_stack_td1.test, player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25acac0; original cards/commander_legends.c:4117. */
int card_mnemonic_deluge(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        new_default_test_definition(&this_test, 0x38, "Select an instant or sorcery card.");
        this_test.ai_selection_mode = 2;
        if ((card != affected_card) || (player != affected_card_controller))
          goto LAB_025acb21;
        generic_spell(player, card, EVENT_CAST_SPELL, 0x1000, NULL, NULL, 1, &this_test);
        if (spell_fizzled != 1)
        {
          instance = get_card_instance(player, card);
          ai_modifier = ai_modifier + cards_ptr[graveyard_ptr_mutable[instance->targets[0].player]
                                                                     [instance->targets[0].card]]
                                          ->ai_base_value;
        }
      }
      return 0;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      new_default_test_definition(&this_test, 0x38, "Select an instant or sorcery card.");
      this_test.ai_selection_mode = 2;
      if (event == EVENT_RESOLVE_SPELL)
      {
        instance = get_card_instance(player, card);
        result = validate_target_from_grave_source(player, card, instance->targets[0].player, 1);
        if (result != -1)
        {
          result = graveyard_ptr_mutable[instance->targets[0].player][result];
          copy_spell_2022(player, (unsigned int)cards_data[result].id);
          copy_spell_2022(player, (unsigned int)cards_data[result].id);
          copy_spell_2022(player, (unsigned int)cards_data[result].id);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_025acb21;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  new_default_test_definition(&this_test, 0x38, "Select an instant or sorcery card.");
  this_test.ai_selection_mode = 2;
LAB_025acb21:
  result = generic_spell(player, card, event, 0x1000, NULL, NULL, 1, &this_test);
  return result;
}

/* ManalinkEh 2022: 0x25ad060; original cards/commander_legends.c:4225. */
int card_necrotic_hex(int player, int card, event_t event)
{
  int iVar1;
  unsigned int t_player;
  int iVar2;
  unsigned int uStack_64;
  token_generation_t token;
  if (event == EVENT_RESOLVE_SPELL)
  {
    uStack_64 = 0;
    do
    {
      t_player = uStack_64 ^ current_turn;
      iVar1 = 6;
      do
      {
        iVar2 = player_sacrifices_a_permanent(player, card, t_player, 2,
                                              SAC_RETURN_CHOICE | SAC_CAUSED | SAC_NO_CANCEL);
        if (iVar2 == 0)
          break;
        iVar1 = iVar1 + -1;
      } while (iVar1 != 0);
      uStack_64 = uStack_64 + 1;
    } while (uStack_64 != 2);
    default_token_definition(player, card, 0x49ac, &token);
    token.qty = '\x06';
    token.action = TOKEN_ACTION_TAPPED;
    generate_token(&token);
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x25a2630; original cards/commander_legends.c:4452. */
static int plague_reaver_effect(int player, int card, event_t event)
{
  int player_00;
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  iVar2 = upkeep_trigger(player, card, event);
  if (iVar2 != 0)
  {
    instance = get_card_instance(player, card);
    if (instance->targets[0].player == current_turn)
    {
      instance = get_card_instance(player, card);
      iVar2 = instance->targets[1].player;
      player_00 = instance->targets[0].player;
      iVar1 = instance->targets[1].card;
      iVar4 = count_graveyard(iVar2);
      iVar4 = iVar4 + -1;
      if (-1 < iVar4)
      {
        do
        {
          if (iVar1 == graveyard_ptr_mutable[iVar2][iVar4])
          {
            reanimate_permanent(player_00, -1, iVar2, iVar4, REANIMATE_DEFAULT);
            break;
          }
          iVar4 = iVar4 + -1;
        } while (iVar4 != -1);
      }
      kill_card(player, card, KILL_EXILE);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25adff0; original cards/commander_legends.c:4564. */
int card_pride_of_the_perfect(int player, int card, event_t event)
{
  boost_creature_type(player, card, event, SUBTYPE_ELF, 2, 0, 0,
                      BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x25ae050; original cards/commander_legends.c:4575. */
int card_profane_transfusion(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  token_generation_t token;
  target_definition_t td;
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    iVar3 = would_validate_arbitrary_target(&td, player, -1);
    if ((iVar3 != 0) && (iVar3 = would_validate_arbitrary_target(&td, 1 - player, -1), iVar3 != 0))
    {
      iVar3 = basic_spell(player, card, EVENT_CAN_CAST);
      return iVar3;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance = get_card_instance(player, card);
        instance->targets[0].player = player;
        instance->targets[1].card = -1;
        instance->targets[0].card = -1;
        instance->targets[1].player = 1 - player;
        instance->number_of_targets = '\x02';
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar3 = validate_target(player, card, &td, 0);
      if ((iVar3 != 0) && (iVar3 = validate_target(player, card, &td, 1), iVar3 != 0))
      {
        iVar3 = life_total_can_change(0);
        if (iVar3 == 0)
        {
          return 0;
        }
        iVar3 = life_total_can_change(1);
        if (iVar3 == 0)
        {
          return 0;
        }
        iVar3 = life[player];
        iVar1 = life[1 - player];
        if (iVar3 < iVar1)
        {
          iVar4 = iVar1 - iVar3;
        }
        else
        {
          iVar4 = 0;
          if (iVar1 < iVar3)
          {
            iVar4 = iVar3 - iVar1;
          }
        }
        exchange_life_totals(player, 1 - player);
        default_token_definition(player, card, 0x4949, &token);
        token.color_forced = '\x01';
        token.action = TOKEN_ACTION_CONVERT_INTO_ARTIFACT;
        token.pow = iVar4;
        token.tou = iVar4;
        generate_token(&token);
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25ae510; original cards/commander_legends.c:4680. */
int card_radiant_serra_archangel(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.illegal_state = 1;
    td.required_abilities = 0x20;
    td.illegal_abilities = 0;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x40004, 0, 0, 0, 0, 0,
                                         0, 0, &td, NULL);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0x40006, 0, 0, 0, 0, 0, 0, 0, &td,
                                "Select another untapped creature with flying to tap.");
      if (spell_fizzled == 1)
      {
        return 0;
      }
      tap_card(instance->targets[0].player, instance->targets[0].card);
      instance->number_of_targets = '\0';
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.illegal_state = 1;
    td.required_abilities = 0x20;
    td.illegal_abilities = 0;
    instance = get_card_instance(player, card);
  }
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  result = select_a_protection(player);
  pump_ability_until_eot_no_repeat_2022(player, card, instance->parent_controller, instance->parent_card,
                                   result, 0);
  return 0;
}

/* ManalinkEh 2022: 0x25ae9b0; original cards/commander_legends.c:4774. */
int card_reshape_the_earth(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&test, 1, "Select a land card.");
    test.qty = 10;
    new_global_tutor(player, player, 1, 5, 0, 1, &test);
    kill_card(player, card, KILL_BURY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25af040; original cards/commander_legends.c:4899. */
int card_rograkh_son_of_rohgahh(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  menace(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x25afc40; original cards/commander_legends.c:5150. */
int card_salamander_warrior(int player, int card, event_t event)
{
  int result;
  result = token_characteristic_setting_effects(player, card, event, player, card);
  return result;
}

/* ManalinkEh 2022: 0x25b0200; original cards/commander_legends.c:5373. */
int card_soulfire_eruption(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t *ptStack_68;
  target_definition_t td;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if (card != affected_card)
      {
        return 0;
      }
      if (player != affected_card_controller)
      {
        return 0;
      }
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 0x102);
        td.zone = 0x1200;
        td.allow_cancel = 3;
        if (event == EVENT_RESOLVE_SPELL)
        {
          instance = get_card_instance(player, card);
          if (instance->number_of_targets != '\0')
          {
            iVar1 = 0;
            do
            {
              ptStack_68 = &td;
              iVar3 = validate_target(player, card, ptStack_68, iVar1);
              if ((iVar3 != 0) && (*deck_ptr[player] != -1))
              {
                iVar3 = get_cmc_by_internal_id(*deck_ptr[player]);
                damage_creature(instance->targets[iVar1].player, instance->targets[iVar1].card,
                                iVar3, player, card);
                exile_top_x_card_you_may_play_them(player, card, 1, NULL, player,
                                                   MPCFE_UNTIL_END_OF_YOUR_NEXT_TURN);
              }
              iVar1 = iVar1 + 1;
            } while (iVar1 < (int)(unsigned int)instance->number_of_targets);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_025b0265;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
  td.allow_cancel = 3;
LAB_025b0265:
  iVar1 = generic_spell(player, card, event, 0xa1, &td,
                        "Select target creatures, planeswalkers, and/or players.", 10, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x25a03f0; original cards/commander_legends.c:5437. */
static char *me_or_commander(int who_chooses, int player, int card, int targeting_player,
                             int targeting_card)
{
  int result;
  result = check_special_flags2_2022(player, card, 0x1000000);
  if ((result == 0) && ((player != targeting_player || (card != targeting_card))))
  {
    return "must be Stumpsquall Hydra or a Commander.";
  }
  return NULL;
}

/* ManalinkEh 2022: 0x25b0ff0; original cards/commander_legends.c:5732. */
int card_toggo_goblin_weaponsmith(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  landfall_mode(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  generate_token_by_id_2022(player, card, 0x56b0);
  return 0;
}

/* ManalinkEh 2022: 0x25b13f0; original cards/commander_legends.c:5829. */
int card_triumphant_reckoning(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&test, 0x144);
    new_reanimate_all(player, card, player, &test, REANIMATE_DEFAULT);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25b1ba0; original cards/commander_legends.c:6021. */
int card_wrong_turn(int player, int card, event_t event)
{
  uint8_t uVar1;
  card_instance_t *instance;
  int result;
  target_definition_t td;
  target_definition_t td2;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    default_target_definition(player, card, &td2, 0);
    td2.zone = 0x1000;
    td2.allowed_controller = 1 - player;
    get_card_instance(player, card);
    result = would_validate_arbitrary_target(&td2, 1 - player, -1);
    if (result != 0)
    {
      result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
      return result;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      default_target_definition(player, card, &td2, 0);
      td2.zone = 0x1000;
      td2.allowed_controller = 1 - player;
      instance = get_card_instance(player, card);
      if ((card == affected_card) &&
          ((player == affected_card_controller &&
            (generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL),
             spell_fizzled != 1))))
      {
        uVar1 = instance->number_of_targets;
        instance->targets[1].player = 1 - player;
        instance->targets[1].card = -1;
        instance->number_of_targets = uVar1 + '\x01';
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    default_target_definition(player, card, &td2, 0);
    td2.allowed_controller = 1 - player;
    td2.zone = 0x1000;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = validate_target(player, card, &td, 0);
      if ((result != 0) && (result = validate_target(player, card, &td2, 1), result != 0))
      {
        gain_control_permanently(instance->targets[1].player, instance->targets[0].player,
                                 instance->targets[0].card);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a4770; original cards/commander_legends.c:924. */
int card_biowaste_blob(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  if ((trigger_condition == 0xc9) && (current_turn == player))
  {
    result = liutenant(player);
    if (result != 0)
    {
      upkeep_trigger_ability(player, card, event, player);
    }
  }
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    result = liutenant(player);
    if (result != 0)
    {
      result = get_id(player, card);
      default_token_definition(player, card, result, &token);
      generate_token(&token);
    }
  }
  boost_creature_type(player, card, event, SUBTYPE_OOZE, 1, 1, 0,
                      BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  return 0;
}

/* ManalinkEh 2022: 0x25a78d0; original cards/commander_legends.c:2211. */
int card_ghen_arcanum_weaver(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  test_definition_t test_sac;
  test_definition_t test_return;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    new_default_test_definition(&test_sac, 4, "Select an enchantment to sacrifice.");
    new_default_test_definition(&test_return, 4, "Select an enchantment card.");
    test_return.ai_selection_mode = 1;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = new_can_sacrifice_as_cost(player, card, &test_sac);
      if (result == 0)
      {
        return 0;
      }
      result = generic_activated_ability_with_target_in_grave(
          player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, 0, 1, 0, 0, 1, 1, 0, NULL, NULL,
          TYPE_NONE, &test_return, 8);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 1, 0, 0, 1, 1, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      uVar1 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                            &test_sac);
      if (uVar1 != 0)
      {
        if (spell_fizzled == 1)
        {
          return 0;
        }
        result = select_target_from_grave_source(player, card, player, SFG_0, 1, &test_return, 0);
        if (result != 0)
        {
          if (spell_fizzled == 1)
          {
            return 0;
          }
          kill_card((int)uVar1 >> 0x10 & 0xff, uVar1 >> 0x18, KILL_SACRIFICE);
          return 0;
        }
      }
      spell_fizzled = 1;
      remove_untargettable_from_all();
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test_sac, 4, "Select an enchantment to sacrifice.");
    new_default_test_definition(&test_return, 4, "Select an enchantment card.");
  }
  test_return.ai_selection_mode = 1;
  if ((event == EVENT_RESOLVE_ACTIVATION) &&
      (result = validate_target_from_grave_source(player, card, player, 0), result != -1))
  {
    reanimate_permanent(player, card, player, result, REANIMATE_DEFAULT);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a7bf0; original cards/commander_legends.c:2283. */
int card_glacian_powerstone_engineer(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int *piVar4;
  target_definition_t td1;
  int selected[50];

  check_legend_rule(player, card, event);
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    return iVar3;
  }
  if (event != EVENT_ACTIVATE)
  {
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    reveal_top_cards_of_library_and_choose_type(player, card, player, instance->info_slot, 1,
                                                TUTOR_HAND, 0, TUTOR_GRAVE, 0, TYPE_ANY);
    return 0;
  }
  default_target_definition(player, card, &td1, 0x40);
  td1.preferred_controller = player;
  td1.allowed_controller = player;
  td1.illegal_state = 0x10;
  td1.illegal_abilities = 0;
  instance = get_card_instance(player, card);
  iVar3 = 0;
  while (true)
  {
    iVar1 = can_target(&td1);
    if (iVar1 == 0)
      break;
    iVar1 = new_pick_target(&td1, "Select an artifact to tap.", 0, 0x80);
    if (iVar1 == 0)
      break;
    iVar1 = instance->targets[0].card;
    selected[iVar3] = iVar1;
    state_untargettable(player, iVar1, 1);
    iVar3 = iVar3 + 1;
    instance->number_of_targets = '\0';
    if (current_turn != player)
    {
      ai_modifier = ai_modifier + 0xf;
    }
  }
  instance->info_slot = iVar3;
  generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  if (0 < iVar3)
  {
    piVar4 = selected;
    do
    {
      iVar1 = *piVar4;
      state_untargettable(player, iVar1, 1);
      if (spell_fizzled != 1)
      {
        tap_card(player, iVar1);
      }
      piVar4 = piVar4 + 1;
    } while (piVar4 != selected + iVar3);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a1d20; original cards/commander_legends.c:2934. */
static int attached_creature_will_deal_triple_damage_until_eot(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;

  pcVar1 = combat_damage_being_prevented(event);
  if (((pcVar1 != NULL) && (pcVar1->damage_target_player != player)) &&
      (pcVar1->damage_target_card == -1))
  {
    pcVar2 = get_card_instance(player, card);
    if (((pcVar2 != NULL) && (pcVar2->damage_target_player != -1)) &&
        ((pcVar2->damage_target_player == pcVar1->damage_source_player &&
          (pcVar1->damage_source_card == pcVar2->damage_target_card))))
    {
      pcVar1->info_slot = pcVar1->info_slot * 3;
    }
  }
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x25aaed0; original cards/commander_legends.c:3479. */
int card_keskit_the_flesh_sculptor(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  test_definition_t this_test;
  test_definition_t this_test2;

  check_legend_rule(player, card, event);
  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&this_test, 0x42, "Select an artifact or creature to sacrifice.");
    this_test.not_me = 1;
    this_test.qty = 3;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&this_test, 0x42, "Select an artifact or creature to sacrifice.");
    this_test.not_me = 1;
    this_test.qty = 3;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      iVar1 = count_deck(player);
      iVar2 = count_deck(player);
      if (iVar2 != 0)
      {
        if (2 < iVar1)
        {
          iVar1 = 2;
        }
        if (3 < iVar2)
        {
          iVar2 = 3;
        }
        new_default_test_definition(&this_test2, 0xff, "Select a card.");
        this_test2.qty = (unsigned short)iVar1;
        this_test2.create_minideck = (unsigned short)iVar2;
        this_test2.no_shuffle = 1;
        new_global_tutor(player, player, 1, 1, 1, 1, &this_test2);
        if (0 < iVar2 - iVar1)
        {
          mill(player, iVar2 - iVar1);
        }
      }
    }
  }
  iVar1 = generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_UNTAPPED, 0, 0,
                                                           0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
                                                           &this_test, SARV_0);
  return iVar1;
}

/* ManalinkEh 2022: 0x25ac340; original cards/commander_legends.c:3880. */
int card_magus_of_the_order(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  const char *pcVar3;
  char msg[100];
  test_definition_t this_test;
  test_definition_t this_test2;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_test_definition(&this_test, 2);
    this_test.color = get_sleighted_color_test(player, card, 8);
    this_test.not_me = 1;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 0, 0, 0, 1, 0, 0, 0,
                                         NULL, NULL);
      if (result == 0)
      {
        return 0;
      }
      result = new_can_sacrifice_as_cost(player, card, &this_test);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      result = charge_mana_for_activated_ability_2022(player, card, 0, 0, 0, 1, 0, 0);
      if (result == 0)
      {
        return 0;
      }
      pcVar3 = get_sleighted_color_text(player, card, "Select a %s creature to sacrifice", 3);
      scnprintf(this_test.message, 100, pcVar3);
      uVar2 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                            &this_test);
      if (uVar2 != 0)
      {
        add_state(player, card, 0x10);
        tapped_for_mana_color = -1;
        kill_card(player, card, KILL_SACRIFICE);
        kill_card((int)uVar2 >> 0x10 & 0xff, uVar2 >> 0x18, KILL_SACRIFICE);
        return 0;
      }
      spell_fizzled = 1;
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_test_definition(&this_test, 2);
    this_test.color = get_sleighted_color_test(player, card, 8);
  }
  this_test.not_me = 1;
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  pcVar3 = get_sleighted_color_text(player, card, "Select a %s creature card.", 3);
  scnprintf(msg, 100, pcVar3);
  new_default_test_definition(&this_test2, 2, msg);
  this_test2.color = get_sleighted_color_test(player, card, 8);
  new_global_tutor(player, player, 1, 4, 0, 2, &this_test2);
  return 0;
}

/* ManalinkEh 2022: 0x25b0e30; original cards/commander_legends.c:5677. */
int card_thalisse_reverent_medium(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  check_legend_rule(player, card, event);
  result = eot_trigger(player, card, event);
  if (result != 0)
  {
    result = get_trap_condition(player, 0x206);
    if (result != 0)
    {
      default_token_definition(player, card, 0x498d, &token);
      token.tou = 1;
      token.pow = 1;
      token.color_forced = ' ';
      token.key_plus = 0x20;
      result = get_trap_condition(player, 0x206);
      token.qty = (unsigned char)result;
      generate_token(&token);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x259ff70; original cards/commander_legends.c:5022. */
static char *is_entered_the_battlefield_this_turn(int who_chooses, int player, int card)
{
  int result;
  char *pcVar2;

  result = check_special_flags_2022(player, card, 0x1000);
  pcVar2 = NULL;
  if (result == 0)
  {
    pcVar2 = "must have entered the battlefield this turn.";
  }
  return pcVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int is_not_commander(int iid, int val, int player, int card);
int count_creatures_with_p1_p1_counter(int player);

/* ManalinkEh 2022: 0x25a2970; original cards/commander_legends.c:76. */
int card_abomination_of_llanowar(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  menace(player, card, event);
  if (((event - EVENT_POWER < 2) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    iVar1 = is_humiliated_for_ability(player, card, AL_STATIC);
    if (iVar1 == 0)
    {
      iVar1 = count_graveyard_by_subtype(player, SUBTYPE_ELF);
      iVar2 = count_subtype(player, 0xff, 0x3c);
      set_base_pt(iVar1 + iVar2);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a2ab0; original cards/commander_legends.c:112. */
int card_akromas_will(int player, int card, event_t event)
{
  int result;
  int sp_ability;
  int ability;

  if (event == EVENT_RESOLVE_SPELL)
  {

    result = liutenant(player);
    result = dialog_impl(
        player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\003", "\001\014",
        "Flying, vigilance, double strike", 1, (unsigned int)(current_phase < 0x19) * 9 + 1,
        "Lifelink, indestructible, protection from all colors", 1,
        ((current_turn != player) - 1 & 0xfffffff2) + 0xf, "Do both", result, 0x14, 0, 0, 0, 0);
    if (result == 2)
    {
      sp_ability = 0x802;
      ability = 0xf800;
    }
    else
    {
      sp_ability = ((result == 3) - 1 & 0xfffff7fe) + 0x80a;
      ability = ((result == 3) - 1 & 0xffff0800) + 0x4000f820;
    }
    pump_creatures_until_eot_no_repeat(player, card, player, result, ability, sp_ability, NULL);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25a2de0; original cards/commander_legends.c:182. */
int card_alena_kessig_trapper(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int card_00;
  int iVar3;

  check_legend_rule(player, card, event);
  if (((event == EVENT_COUNT_MANA) || (event == EVENT_ACTIVATE)) ||
      ((event == EVENT_CAN_ACTIVATE && (player == 1))))
  {

    iVar3 = 0;
    card_00 = active_cards_count[player] + -1;
    if (-1 < card_00)
    {
      do
      {

        while ((((instance = in_play(player, card_00),
                  instance == NULL || (iVar2 = is_what(player, card_00, 2), iVar2 == 0)) ||
                 (iVar2 = check_special_flags_2022(player, card_00, 0x1000), iVar2 == 0)) ||
                (iVar2 = get_power(player, card_00), iVar2 <= iVar3)))
        {
          card_00 = card_00 + -1;
          if (card_00 == -1)
            goto LAB_025a2e8e;
        }
        card_00 = card_00 + -1;
        iVar3 = iVar2;
      } while (card_00 != -1);
    }
  }
  else
  {
    iVar3 = 0;
  }
LAB_025a2e8e:
  iVar3 = mana_producing_creature(player, card, event, 0x30, COLOR_RED, iVar3);
  return iVar3;
}

/* ManalinkEh 2022: 0x25a35c0; original cards/commander_legends.c:423. */
int card_anara_wolvid_familiar(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  if ((event == EVENT_ABILITIES) && (current_turn == player))
  {
    result = check_special_flags2_2022(affected_card_controller, affected_card, 0x1000000);
    if (result != 0)
    {
      instance = in_play(player, card);
      if (instance != NULL)
      {
        result = is_humiliated_for_ability(player, card, AL_STATIC);
        if ((result == 0) && (affected_card_controller == player))
        {
          indestructible(player, affected_card, EVENT_ABILITIES);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a1dc0; original cards/commander_legends.c:664. */
static char *aura_enchanting_player(int who_chooses, int player, int card)
{
  int result;
  card_instance_t *instance;

  result = is_what(player, card, 4);

  if (((result != 0) && (instance = in_play(player, card), instance->damage_target_player != -1)) &&
      (instance->damage_target_card == -1))
  {
    return NULL;
  }
  return "must be an Equipment or Aura.";
}

/* ManalinkEh 2022: 0x25a4100; original cards/commander_legends.c:755. */
int card_armix_filigree_thrasher(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  iVar1 = duh_mode(player);
  dat_ability(player, card, event, (iVar1 != 0) + DAT_RESOLVE_TRIGGER_OPTIONAL, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    instance = get_card_instance(player, card);
    new_default_test_definition(&test, 0xff, "Select a card to discard.");
    iVar1 = new_select_a_card(player, player, 4, 0, 3, -1, &test);
    if (iVar1 != -1)
    {
      new_discard_card(player, iVar1, 0, player);
      default_target_definition(player, card, &td, 2);
      td.allow_cancel = 0;
      iVar1 = can_target(&td);
      if (iVar1 != 0)
      {
        iVar1 = new_pick_target(&td, "Select target creature opponent controls.", 0, 0x80);
        if (iVar1 != 0)
        {

          iVar1 = count_subtype(player, 0x40, -1);
          iVar3 = count_graveyard_by_type(player, 0x40);
          pump_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card,
                         -(iVar3 + iVar1), -(iVar3 + iVar1));
        }
      }
      instance->number_of_targets = '\0';
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a4c60; original cards/commander_legends.c:1053. */
int card_blim_comedic_genius(int player, int card, event_t event)
{
  unsigned int uVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  int *piStack_70;
  int times_damaged[2];
  target_definition_t td;

  check_legend_rule(player, card, event);
  iVar2 = damage_dealt_by_me(player, card, event,
                             DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                 DDBM_MUST_BE_COMBAT_DAMAGE);
  if (iVar2 != 0)
  {

    instance = get_card_instance(player, card);
    uVar1 = instance->targets[1].player;
    instance->targets[1].player = 0;
    times_damaged[1] = uVar1 >> 8 & 0xff;
    times_damaged[0] = uVar1 & 0xff;
    piStack_70 = times_damaged;
    do
    {
      default_target_definition(player, card, &td, 0x47);
      td.allow_cancel = 0;
      iVar2 = *piStack_70;
      td.preferred_controller = player;
      td.allowed_controller = player;
      while ((iVar2 != 0 && (iVar4 = can_target(&td), iVar4 != 0)))
      {
        new_pick_target(&td, "Select target permanent you control.", 0, 0x80);
        give_control_of_self(instance->targets[0].player, instance->targets[0].card);
        iVar2 = iVar2 + -1;
        instance->number_of_targets = '\0';
        *piStack_70 = iVar2;
      }
      piStack_70 = piStack_70 + 1;
    } while ((target_definition_t *)piStack_70 != &td);

    iVar2 = count_permanents_you_control_but_you_dont_own(0, 0x47);
    lose_life_2022(0, iVar2);
    new_multidiscard(0, iVar2, 2, player);
    iVar2 = count_permanents_you_control_but_you_dont_own(1, 0x47);
    lose_life_2022(1, iVar2);
    new_multidiscard(1, iVar2, 2, player);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a4e20; original cards/commander_legends.c:1098. */
int card_body_of_knowledge(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (((event - EVENT_POWER < 2) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    set_base_pt(hand_count[player]);
  }
  result = damage_dealt_to_me_arbitrary(player, card, event, DDBM_0, player, card);
  if (result != 0)
  {
    instance = get_card_instance(player, card);
    draw_cards(player, instance->targets[7].card);
    instance = get_card_instance(player, card);
    instance->targets[7].card = 0;
  }
  result = card_spellbook(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25a5350; original cards/commander_legends.c:1241. */
int card_coercive_recruiter(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td1;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &recovered_stack_td1.target, 2);
    instance = get_card_instance(player, card);
    result = can_target(&recovered_stack_td1.target);
    if (result != 0)
    {
      result = new_pick_target(&recovered_stack_td1.target, "TARGET_CREATURE", 0, 0);
      if (result != 0)
      {

        result = add_a_subtype_until_eot(player, card, instance->targets[0].player,
                                         instance->targets[0].card, SUBTYPE_PIRATE);
        add_status(player, result, 0x10000);
        effect_act_of_treason(player, card, instance->targets[0].player, instance->targets[0].card);
      }
    }
    instance->number_of_targets = '\0';
  }
  if ((trigger_condition == 0xdb) && (trigger_cause_controller == player))
  {
    default_test_definition(&recovered_stack_td1.test, 0x47);
    etb_ability_global(player, event, 2, TYPE_NONE, &recovered_stack_td1.test, player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a73a0; original cards/commander_legends.c:2098. */
int card_falthis_shadowcat_familiar(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  if ((event == EVENT_ABILITIES) && (affected_card_controller == player))
  {
    result = is_what(player, affected_card, 0x47);
    if (result != 0)
    {
      result = check_special_flags2_2022(affected_card_controller, affected_card, 0x1000000);
      if (result != 0)
      {
        instance = in_play(player, card);
        if (instance != NULL)
        {
          result = is_humiliated_for_ability(player, card, AL_STATIC);
          if (result == 0)
          {
            deathtouch(affected_card_controller, affected_card, EVENT_ABILITIES);
            menace(affected_card_controller, affected_card, EVENT_ABILITIES);
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a7720; original cards/commander_legends.c:2169. */
int card_flamekin_herald(int player, int card, event_t event)
{
  int result;

  if (trigger_condition != 0xd3)
  {
    return 0;
  }
  if ((card == affected_card) && (player == affected_card_controller))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      result = check_special_flags2_2022(trigger_cause_controller, trigger_cause, 0x1000000);
      if (result != 0)
      {
        result = new_specific_spell_played(player, card, event, player, 2, NULL);
        if (result != 0)
        {
          result = get_cmc(trigger_cause_controller, trigger_cause);
          do_cascade(player, card, result);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a7ed0; original cards/commander_legends.c:2338. */
int card_gnostro_voice_of_the_crags(int player, int card, event_t event)
{
  uint8_t uVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int iVar5;
  target_definition_t td1;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td1, 2);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      return iVar4;
    }
    if (event == EVENT_ACTIVATE)
    {

      pcVar3 = get_card_instance(player, card);
      pcVar3->number_of_targets = '\0';
      pcVar2 = get_card_instance(player, card);
      pcVar2->mode = pcVar3->number_of_targets;
      iVar4 = life[player];
      iVar5 = can_target(&td1);
      iVar4 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014", "Scry", 1, 10,
                          "Damage creature", iVar5, 0x1e, "Gain life", 1,
                          ((iVar4 < 6) - 1 & 0xffffffe2) + 0x32, 0, 0, 0, 0);
      if (iVar4 == 2)
      {
        pick_target(&td1, "TARGET_CREATURE");
      }
      if (spell_fizzled == 1)
      {
        return 0;
      }
      generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      pcVar3 = get_card_instance(player, card);
      pcVar3->mode = (uint8_t)iVar4;
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td1, 2);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    pcVar3 = get_card_instance(player, card);
    uVar1 = pcVar3->mode;
    iVar4 = get_storm_count();
    if (uVar1 == '\x01')
    {
      scry(player, iVar4);
    }
    else if (uVar1 == '\x02')
    {
      iVar5 = valid_target(&td1);
      if (iVar5 != 0)
      {
        damage_target0(player, card, iVar4);
      }
    }
    else if (uVar1 == '\x03')
    {
      gain_life_2022(player, iVar4);
      return 0;
    }
  }

  return 0;
}

/* ManalinkEh 2022: 0x25a8610; original cards/commander_legends.c:2487. */
int count_creatures_with_p1_p1_counter(int player)
{
  card_instance_t *instance;
  int result;
  int player_00;
  int card;
  int iStack_14;

  player_00 = 0;
  iStack_14 = 0;
  do
  {

    if (((player == player_00) || (player_00 == 2)) && (0 < active_cards_count[player]))
    {
      card = 0;
      do
      {
        instance = in_play(player_00, card);
        if (instance != NULL)
        {
          result = is_what(player_00, card, 2);
          if (result != 0)
          {
            result = count_counters(player_00, card, COUNTER_P1_P1);
            iStack_14 = (iStack_14 + 1) - (unsigned int)(result == 0);
          }
        }
        card = card + 1;
      } while (card < active_cards_count[player]);
    }
    player_00 = player_00 + 1;
  } while (player_00 != 2);
  return iStack_14;
}

/* ManalinkEh 2022: 0x25a86a0; original cards/commander_legends.c:2501. */
int card_hamza_guardian_of_arashin(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
  }
  else
  {
    if (event != EVENT_MODIFY_COST_GLOBAL)
    {
      return 0;
    }
    instance = in_play(player, card);
    if (instance == NULL)
    {
      return 0;
    }
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result != 0)
    {
      return 0;
    }
    if (affected_card_controller != player)
    {
      return 0;
    }
    result = is_what(player, affected_card, 2);
    if (result == 0)
    {
      return 0;
    }
  }
  result = count_creatures_with_p1_p1_counter(player);
  COST_COLORLESS = COST_COLORLESS - (char)result;
  return 0;
}

/* ManalinkEh 2022: 0x25a8960; original cards/commander_legends.c:2611. */
int card_horizon_stone(int player, int card, event_t event)
{
  uint8_t *puVar1;
  unsigned char bVar2;
  int iVar3;
  uint8_t *puVar4;
  card_instance_t *instance;
  int iVar5;

  if ((event == EVENT_MANA_POOL_DRAINING) &&
      (iVar3 = is_humiliated_for_ability(player, card, AL_STATIC), iVar3 == 0))
  {
    puVar4 = mana_doesnt_drain_from_pool[player];
    do
    {

      puVar1 = puVar4 + 1;
      puVar4 = puVar4 + 1;
      *puVar4 = *puVar1 | 0x40;
    } while (puVar4 != mana_doesnt_drain_from_pool[player] + 5);
    bVar2 = mana_doesnt_drain_from_pool[player][6];
    mana_doesnt_drain_from_pool[player][0] = mana_doesnt_drain_from_pool[player][0] | 0x80;
    mana_doesnt_drain_from_pool[player][6] = bVar2 | 0x80;
  }
  iVar3 = eot_trigger(player, card, event);
  if ((((iVar3 != 0) &&
        ((ai_is_speculating == 1 || ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))))) &&
       (iVar3 = is_humiliated_for_ability(player, card, AL_TRIG), iVar3 == 0)) &&
      (((current_turn != player ||
         (iVar3 = check_battlefield_for_id(player, 0x1921), iVar3 != 0)) ||
        (iVar3 = check_battlefield_for_id(player, 0x34d1), iVar3 != 0))))
  {

    needed_mana_colors = 0x7f;
    iVar3 = 0;
    if (0 < active_cards_count[player])
    {
      do
      {
        instance = in_play(player, iVar3);
        if (((instance != NULL) &&
             ((cards_data[instance->internal_card_id].extra_ability & 0x1000) != 0)) &&
            (iVar5 = call_card_function_i(instance, player, iVar3, EVENT_CAN_ACTIVATE), iVar5 != 0))
        {
          force_activation_for_mana(player, iVar3, COLOR_TEST_ARTIFACT | COLOR_TEST_ANY);
          needed_mana_colors = 0x7f;
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 < active_cards_count[player]);
    }
    needed_mana_colors = 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a8b20; original cards/commander_legends.c:2655. */
int card_hullbreacher(int player, int card, event_t event)
{
  int result;

  result = notion_thief_effect(player, card, event);
  if (result != 0)
  {
    generate_token_by_id_2022(player, card, 0x4998);
  }
  result = flash(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25aa8c0; original cards/commander_legends.c:3367. */
int card_kangee_sky_warden(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);

  if ((event == EVENT_DAT_ABILITY) && (iVar1 = active_cards_count[player] + -1, -1 < iVar1))
  {
    do
    {
      instance = in_play(player, iVar1);
      if ((instance != NULL) &&
          (((iVar3 = is_what(player, iVar1, 2),
             iVar3 != 0 && (iVar3 = is_attacking(player, iVar1), iVar3 != 0)) &&
            (iVar3 = check_for_ability(player, iVar1, 0x20), iVar3 != 0))))
      {
        pump_until_eot_merge_previous(player, card, player, iVar1, 2, 0);
      }
      iVar1 = iVar1 + -1;
    } while (iVar1 != -1);
  }
  iVar1 = blocking(player, card, event);

  if (((iVar1 != 0) && (iVar1 = is_humiliated_for_ability(player, card, AL_TRIG), iVar1 == 0)) &&
      ((current_turn != player && (iVar1 = active_cards_count[player] + -1, -1 < iVar1))))
  {
    do
    {
      instance = in_play(player, iVar1);
      if (((instance != NULL) && (iVar3 = is_what(player, iVar1, 2), iVar3 != 0)) &&
          ((iVar3 = blocking(player, iVar1, event),
            iVar3 != 0 && (iVar3 = check_for_ability(player, iVar1, 0x20), iVar3 != 0))))
      {
        pump_until_eot_merge_previous(player, card, player, iVar1, 0, 2);
      }
      iVar1 = iVar1 + -1;
    } while (iVar1 != -1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a0390; original cards/commander_legends.c:3447. */
static int commander_test(int iid, int unused, int player, int card)
{
  int result;

  result = check_special_flags2_2022(player, card, 0x1000000);
  return (unsigned int)(result != 0);
}

/* ManalinkEh 2022: 0x25ab600; original cards/commander_legends.c:3638. */
int card_kwain_itinerant_meddler(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  unsigned int player_00;
  int dc[2];

  check_legend_rule(player, card, event);
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    dc[0] = 0;
    dc[1] = 0;

    uVar2 = 0;
    do
    {
      player_00 = uVar2 ^ current_turn;
      result = count_deck(player_00);
      result = dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\002", "\001\014",
                           "\001\010", player_00, "\001\003", "Draw a card", 1,
                           ((9 < result) - 1 & 0xffffff38) + 100, "Decline", 1, 0, 0, 0, 0, 0);
      dc[player_00] = result;
      if (result == 1)
      {
        draw_a_card(player_00);
      }
      uVar2 = uVar2 + 1;
    } while (uVar2 != 2);

    if (dc[current_turn] == 1)
    {
      gain_life_2022(current_turn, 1);
    }
    if (dc[current_turn ^ 1U] == 1)
    {
      gain_life_2022(current_turn ^ 1U, 1);
    }
  }
  result = generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25ade60; original cards/commander_legends.c:4531. */
int card_prava_of_the_steel_legion(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  if (event - EVENT_POWER < 2)
  {
    if ((((affected_card_controller == player) && (current_turn == player)) &&
         (instance = in_play(player, card), instance != NULL)) &&
        ((result = is_humiliated_for_ability(player, card, AL_STATIC),
          result == 0 &&
              (result = is_token(affected_card_controller, affected_card), result != 0))))
    {
      if (event == EVENT_POWER)
      {
        event_result = event_result + 1;
      }
      else
      {
        event_result = event_result + 4;
      }
    }
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_025adec5:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_025adece;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      if ((current_turn != player) && (current_phase < 0x17))
      {
        ai_modifier = ai_modifier + 0x30;
      }
      goto LAB_025adece;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_025adec5;
    }
  }
  generate_token_by_id_2022(player, card, 0x4986);
LAB_025adece:
  result = generic_activated_ability(player, card, event, 0, 3, 0, 0, 0, 0, 1, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x25ae2d0; original cards/commander_legends.c:4628. */
int card_promise_of_tomorrow(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  unsigned int *puVar3;
  card_instance_t *instance;
  int iid;
  int leg;
  int idx;

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    instance = in_play(affected_card_controller, affected_card);
    if (instance != NULL)
    {
      result = is_what(affected_card_controller, affected_card, 2);
      if ((result != 0) && (affected_card_controller == player))
      {
        result = is_token(player, affected_card);
        if (result == 0)
        {
          result = check_for_death_trigger_removal(affected_card_controller, affected_card);
          if (result == 0)
          {

            instance = get_card_instance(affected_card_controller, affected_card);
            if ((instance->kill_code & 0xfb) != 0)
            {

              result = get_owner(affected_card_controller, affected_card);
              iid = get_original_internal_card_id(affected_card_controller, affected_card);
              exiledby_remember(player, card, result, iid, NULL, NULL);
              instance->kill_code = '\x04';
            }
          }
        }
      }
    }
  }
  result = eot_trigger(player, card, event);
  if (result != 0)
  {
    result = count_subtype(player, 2, -1);
    if (result < 1)
    {

      state_untargettable(player, card, 1);
      leg = 0;
      idx = 0;
      while (true)
      {
        puVar3 = (unsigned int *)exiledby_find_any(player, card, &leg, &idx);
        if (puVar3 == NULL)
          break;
        uVar1 = *puVar3;
        *puVar3 = 0xffffffff;
        result = add_card_to_hand(player, uVar1 & 0x7fffffff);
        if (player != uVar1 >> 0x1f)
        {
          if (player == 1)
          {
            remove_state(1, result, 0x1000);
          }
          else
          {

            add_state(player, result, 0x1000);
          }
        }
        put_into_play(player, result);
      }
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25aea60; original cards/commander_legends.c:4791. */
int card_reyav_master_smith(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned char *pbVar3;

  check_legend_rule(player, card, event);
  store_attackers(player, card, event, DAT_TRACK, player, -1, NULL);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  if (event == EVENT_DAT_ABILITY)
  {

    instance = get_card_instance(player, card);
    if (0 < instance->info_slot)
    {
      pbVar3 = (unsigned char *)((int)&instance->targets[1].card + instance->info_slot + 3);
      do
      {
        result = is_enchanted(current_turn, (unsigned int)*pbVar3);
        if (result == 0)
        {
          result = is_equipped(current_turn, (unsigned int)*pbVar3);
          if (result != 0)
            goto LAB_025aeb3a;
        }
        else
        {
        LAB_025aeb3a:
          pump_ability_until_eot_no_repeat_2022(player, card, current_turn, (unsigned int)*pbVar3,
                                           0x40000000, 0);
        }
        pbVar3 = pbVar3 + -1;
        if (pbVar3 == (unsigned char *)((int)&instance->targets[1].card + 3))
        {
          return 0;
        }
      } while (true);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25aeb80; original cards/commander_legends.c:4811. */
int card_rock_token(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int iVar4;
  unsigned int uVar5;
  int iVar6;
  int player_00;
  target_definition_t td;

  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    pcVar2 = get_card_instance(player, card);
  LAB_025aebdc:
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      if (pcVar2->info_slot == 1)
      {
        iVar6 = basic_equipment(player, card, EVENT_RESOLVE_ACTIVATION, 1);
        return iVar6;
      }
      if (pcVar2->info_slot == 2)
      {
        td.illegal_abilities = pcVar2->targets[1].player;
        iVar6 = valid_target(&td);
        if (iVar6 != 0)
        {
          damage_target0(player, card, 2);
          return 0;
        }
      }
    }
    return 0;
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
  pcVar2 = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    pcVar3 = is_equipping(player, card);
    if (pcVar3 != NULL)
    {

      cVar1 = pcVar2->damage_target_player;
      iVar6 = pcVar2->damage_target_card;
      td.illegal_abilities = get_protections_from((int)cVar1, iVar6);
      iVar6 = granted_generic_activated_ability(player, card, (int)cVar1, iVar6, EVENT_CAN_ACTIVATE,
                                                5, 1, 0, 0, 0, 0, 0, 0, &td, NULL);
      if ((iVar6 != 0) && (iVar6 = can_sacrifice_this_as_cost(player, card), iVar6 != 0))
      {
        return 1;
      }
    }
    iVar6 = basic_equipment(player, card, EVENT_CAN_ACTIVATE, 1);
    return iVar6;
  }
  if (event != EVENT_ACTIVATE)
    goto LAB_025aebdc;

  iVar6 = can_activate_basic_equipment(player, card, EVENT_ACTIVATE, 1);
  pcVar3 = is_equipping(player, card);
  if (pcVar3 != NULL)
  {

    cVar1 = pcVar2->damage_target_player;
    iVar4 = pcVar2->damage_target_card;
    td.illegal_abilities = get_protections_from((int)cVar1, iVar4);
    iVar4 = granted_generic_activated_ability(player, card, (int)cVar1, iVar4, EVENT_CAN_ACTIVATE,
                                              5, 1, 0, 0, 0, 0, 0, 0, &td, NULL);
    if (iVar4 != 0)
    {
      iVar4 = can_sacrifice_this_as_cost(player, card);
      uVar5 = (unsigned int)(iVar4 != 0);
      goto LAB_025aed17;
    }
  }
  uVar5 = 0;
LAB_025aed17:
  iVar6 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Equip", iVar6, 5, "Sac & damage",
                      uVar5, 10, 0, 0, 0, 0);
  if (iVar6 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (iVar6 == 1)
  {
    iVar6 = basic_equipment(player, card, EVENT_ACTIVATE, 1);
    return iVar6;
  }
  if (iVar6 != 2)
  {
    return 0;
  }
  pcVar3 = is_equipping(player, card);
  if (pcVar3 == NULL)
  {
    return 0;
  }

  player_00 = (int)pcVar2->damage_target_player;
  iVar6 = pcVar2->damage_target_card;
  td.illegal_abilities = get_protections_from(player_00, iVar6);
  iVar4 = get_protections_from(player_00, iVar6);
  pcVar2->targets[1].player = iVar4;
  iVar6 = granted_generic_activated_ability(player, card, player_00, iVar6, EVENT_ACTIVATE, 5, 1, 0,
                                            0, 0, 0, 0, 0, &td, "TARGET_ANY");
  if (iVar6 == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_SACRIFICE);
  return 0;
}

/* ManalinkEh 2022: 0x25af080; original cards/commander_legends.c:4911. */
int card_rootweaver_druid(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int iStack_f8;
  int array[3];
  test_definition_t test;

  iVar1 = duh_mode(player);
  etb_ability_2022(player, card, event, (iVar1 != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {

    new_default_test_definition(&test, 1, "Select a land card.");
    iVar1 = 1 - player;
    test.qty = 3;
    new_global_tutor(iVar1, iVar1, 1, 0xd, 0, 1, &test);
    if (active_cards_count[iVar1] < 1)
    {
      iStack_f8 = 0;
    }
    else
    {
      iVar4 = 0;
      iStack_f8 = 0;
      do
      {
        instance = in_hand(iVar1, iVar4);
        if ((instance != NULL) && (iVar3 = check_state(iVar1, 1, 0x200000), iVar3 != 0))
        {
          array[iStack_f8] = iVar4;
          iStack_f8 = iStack_f8 + 1;
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 < active_cards_count[iVar1]);
    }
    iVar4 = select_card_from_zone(iVar1, iVar1, array, iStack_f8, 1, 3, -1, &test);
    if (-1 < iVar4)
    {

      iVar3 = get_original_internal_card_id(iVar1, iVar4);
      obliterate_card(iVar1, iVar4);
      iVar4 = add_card_to_hand(player, iVar3);
      if (player == 0)
      {
        add_state(0, iVar4, 0x1000);
      }
      else
      {
        remove_state(player, iVar4, 0x1000);
      }
      add_state(player, iVar4, 0x10);
      put_into_play(player, iVar4);
    }
    iVar4 = active_cards_count[iVar1] + -1;
    if (-1 < iVar4)
    {
      do
      {
        instance = in_hand(iVar1, iVar4);
        if ((instance != NULL) && (iVar3 = check_state(iVar1, 1, 0x200000), iVar3 != 0))
        {
          add_state(iVar1, iVar4, 0x10);
          remove_state(iVar1, iVar4, 0x200000);
          put_into_play(iVar1, iVar4);
        }
        iVar4 = iVar4 + -1;
      } while (iVar4 != -1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25afde0; original cards/commander_legends.c:5199. */
int card_siani_eye_of_the_storm(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int card_00;
  int number_of_cards;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }

  if (active_cards_count[player] < 1)
  {
    number_of_cards = 0;
  }
  else
  {
    card_00 = 0;
    number_of_cards = 0;
  LAB_025afe5a:
    do
    {
      instance = in_play(player, card_00);
      if (instance != NULL)
      {
        result = is_attacking(player, card_00);
        if (result != 0)
        {
          result = check_for_ability(player, card_00, 0x20);
          number_of_cards = (number_of_cards + 1) - (unsigned int)(result == 0);
          card_00 = card_00 + 1;
          if (active_cards_count[player] <= card_00)
            break;
          goto LAB_025afe5a;
        }
      }
      card_00 = card_00 + 1;
    } while (card_00 < active_cards_count[player]);
  }
  scry(player, number_of_cards);
  return 0;
}

/* ManalinkEh 2022: 0x25a03c0; original cards/commander_legends.c:5222. */
static int is_not_commander(int iid, int val, int player, int card)
{
  int result;

  result = check_special_flags2_2022(player, card, 0x1000000);
  return (unsigned int)(result == 0);
}

/* ManalinkEh 2022: 0x25afec0; original cards/commander_legends.c:5225. */
int card_slash_the_ranks(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 0x102);
    this_test.special_selection_function = is_not_commander;
    new_manipulate_all(player, card, 2, &this_test, ACT_KILL_DESTROY);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25b0840; original cards/commander_legends.c:5533. */
int card_szats_will(int player, int card, event_t event)
{
  int *piVar1;
  int result;
  unsigned int uVar3;
  int *piVar4;
  short sVar5;
  token_generation_t token;

  if (event == EVENT_RESOLVE_SPELL)
  {

    result = liutenant(player);
    uVar3 = dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\002", "\001\003", "\001\014",
                        "Sacrifice", 1, 0x14, "Exile graveyards", 1, 10, "Do both", result, 0x14, 0,
                        0, 0, 0);
    if ((uVar3 & 1) != 0)
    {
      player_sacrifices_the_creature_with_highest_power(player, card, 1 - player);
    }
    if ((uVar3 & 2) != 0)
    {

      sVar5 = 0;
      piVar4 = graveyard_ptr_mutable[1 - player];
      piVar1 = piVar4 + 500;
      do
      {
        if (*piVar4 == -1)
          break;

        if (sVar5 < cards_data[*piVar4].power)
        {
          sVar5 = cards_data[*piVar4].power;
        }
        piVar4 = piVar4 + 1;
      } while (piVar4 != piVar1);
      rfg_whole_graveyard(1 - player);
      default_token_definition(player, card, 0x4996, &token);
      token.qty = (unsigned char)sVar5;
      token.pow = 0;
      token.tou = 1;
      token.color_forced = '\x02';
      generate_token(&token);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25b1060; original cards/commander_legends.c:5750. */
int card_tormod_the_desecrator(int player, int card, event_t event)
{
  token_generation_t token;

  check_legend_rule(player, card, event);
  if ((event == EVENT_REMOVED_FROM_GRAVEYARD) && (affected_card_controller == player))
  {
    default_token_definition(player, card, 0x49ac, &token);
    token.action = TOKEN_ACTION_TAPPED;
    generate_token(&token);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25b1510; original cards/commander_legends.c:5860. */
int card_tuya_bearclaw(int player, int card, event_t event)
{
  int power;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }

  default_test_definition(&this_test, 2);
  this_test.not_me = 1;
  power = check_battlefield_for_special_card(player, card, player, 0x80, &this_test);
  pump_until_eot_2022(player, card, player, card, power, power);
  return 0;
}

/* ManalinkEh 2022: 0x25b1eb0; original cards/commander_legends.c:6063. */
int card_wyleth_soul_of_steel(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }

  iVar1 = count_auras_enchanting_me(player, card);
  iVar2 = equipments_attached_to_me(player, card, 1);
  draw_cards(player, iVar1 + iVar2);
  return 0;
}

/* ManalinkEh 2022: 0x25b1f50; original cards/commander_legends.c:6082. */
int card_yurlok_of_scorch_thrash(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  unsigned int player_00;
  int iVar3;

  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  if (event == EVENT_MANA_POOL_DRAINING)
  {
    iVar2 = is_humiliated_for_ability(player, card, AL_STATIC);
    if (iVar2 == 0)
    {

      iVar3 = 0;
      iVar2 = 0;
      do
      {
        piVar1 = mana_pool[current_turn] + iVar2;
        iVar2 = iVar2 + 1;
        iVar3 = iVar3 + *piVar1;
      } while (iVar2 != 6);
      lose_life(current_turn, iVar3 + mana_pool[current_turn][6]);
      player_00 = current_turn ^ 1;
      iVar3 = 0;
      iVar2 = 0;
      do
      {
        piVar1 = mana_pool[player_00] + iVar3;
        iVar3 = iVar3 + 1;
        iVar2 = iVar2 + *piVar1;
      } while (iVar3 != 6);
      lose_life(player_00, iVar2 + mana_pool[player_00][6]);
    }
  }
  else
  {
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x200001, 1, 0, 0, 0, 0,
                                        0, 0, NULL, NULL);
      return iVar2;
    }
    if ((event == EVENT_ACTIVATE) && (generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 1,
                                                                0, 0, 0, 0, 0, 0, NULL, NULL),
                                      spell_fizzled != 1))
    {
      produce_mana_multi(player, 0, 1, 0, 1, 1, 0);
      produce_mana_multi(1 - player, 0, 1, 0, 1, 1, 0);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x259ffa0; original cards/commander_legends.c:206. */
static int alhard_solemn_ritualist_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  token_generation_t token;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (iVar2 = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           iVar2 != 0x5631))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (iVar2 = effect_follows_control_of_attachment(player, card, 0x4700), iVar2 != 0))
    {
      return 0;
    }
  LAB_0259ffd5:
    iVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
      goto LAB_0259fff7;
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (iVar2 = effect_follows_control_of_attachment(player, card, event), iVar2 != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_0259ffd5;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);
    if (((pcVar3 != NULL) &&
         (iVar2 = is_this_dying(affected_card_controller, affected_card), iVar2 != 0)) &&
        (iVar2 = count_counters(affected_card_controller, affected_card, COUNTER_P1_P1),
         iVar2 != 0))
    {

      if ((pcVar1->damage_target_card == affected_card) &&
          (pcVar1->damage_target_player == affected_card_controller))
      {
        pcVar1->damage_target_card = -1;
        pcVar1->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        if (pcVar1->targets[0xb].player < 0)
        {
          pcVar1->targets[0xb].player = 0;
        }
        *(uint8_t *)((int)&pcVar1->targets[0xb].player + 2) = 1;
      }
      count_for_gfp_ability_and_store_values(player, card, 0x77, player, 2, NULL, 0x40, 0);
    }
    iVar2 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
    {
      return 0;
    }
  }
  iVar4 = check_status(player, card, 0x10000);
  if (iVar4 == 0)
  {

    default_token_definition(player, card, 0x498d, &token);
    token.pow = 1;
    token.tou = 1;
    token.color_forced = ' ';
    token.key_plus = 0x20;
    token.qty = (unsigned char)iVar2;
    generate_token(&token);
    kill_card(player, card, KILL_EXILE);
  }
LAB_0259fff7:
  if ((event == EVENT_CLEANUP) && (iVar2 = check_status(player, card, 0x10000), iVar2 == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static void slurrk_trigger(int player, int card);
static int juri_master_of_the_revue_effect(int player, int card, event_t event);
static int slurrk_effect(int player, int card, event_t event);
static void ich_tekik_ability(int player, int card);
static int ich_tekik_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25a19f0; original cards/commander_legends.c:2669. */
static void ich_tekik_ability(int player, int card)
{
  test_definition_t test;

  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  default_test_definition(&test, 0x47);
  test.subtype = 0x4c;
  new_manipulate_all(player, card, player, &test, ACT_ADD_COUNTERS_BASE | ACT_KILL_BURY);
  return;
}

/* ManalinkEh 2022: 0x25a1a70; original cards/commander_legends.c:2677. */
static int ich_tekik_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  card_instance_t *pcVar3;
  int result;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x5678))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, 0x4700), result != 0))
    {
      return 0;
    }
  LAB_025a1aa5:
    uVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto joined_r0x025a1aca;
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, event), result != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_025a1aa5;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);

    if ((pcVar3 != NULL) && (pcVar3 = get_card_instance(affected_card_controller, affected_card),
                             pcVar3->kill_code != '\0'))
    {

      if ((-1 < pcVar1->damage_target_player) &&
          ((pcVar1->damage_target_card == affected_card &&
            (pcVar1->damage_target_player == affected_card_controller))))
      {
        pcVar1->damage_target_card = -1;
        pcVar1->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        if (pcVar1->targets[0xb].player < 0)
        {
          pcVar1->targets[0xb].player = 0;
        }
        *(uint8_t *)((int)&pcVar1->targets[0xb].player + 1) = 1;
      }
      count_for_gfp_ability(player, card, 0x77, 2, 0x40, NULL);
    }
    uVar2 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {

    for (uVar2 = uVar2 & 0xff; uVar2 != 0; uVar2 = uVar2 - 1)
    {
      ich_tekik_ability(player, card);
    }
    kill_card(player, card, KILL_EXILE);
  }
joined_r0x025a1aca:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a8b80; original cards/commander_legends.c:2729. */
int card_ich_tekik_salvage_splicer(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int iVar3;
  int iVar4;

  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar4 = 0;
        do
        {
          instance = in_play(player, iVar4);

          if ((((instance != NULL) && (iVar3 = is_what(player, iVar4, 0x80), iVar3 != 0)) &&
               (instance = get_card_instance(player, iVar4),
                instance->info_slot_fn == ich_tekik_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_025a8c50;
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[player]);
      }

      iVar4 = create_targetted_legacy_effect(player, card, ich_tekik_effect, player, card);
      instance = get_card_instance(player, iVar4);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar4, 0x10000);
    }
  LAB_025a8c50:
    etb_ability_2022(player, card, EVENT_STATIC_EFFECTS, RESOLVE_TRIGGER_MANDATORY);
  }
  else
  {
    etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (event == EVENT_ETB)
    {
      generate_tokens_by_id(player, card, 0x4940, 1);
    }
    else if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {
      count_for_gfp_ability(player, card, 0x77, 2, 0x40, NULL);
    }
  }
  instance = get_card_instance(player, card);

  if (((instance->kill_code == '\0') &&
       (uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar2 != 0)) &&
      (uVar2 = uVar2 & 0xff, uVar2 != 0))
  {
    do
    {
      ich_tekik_ability(player, card);
      uVar2 = uVar2 - 1;
    } while (uVar2 != 0);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a1310; original cards/commander_legends.c:3060. */
static int juri_master_of_the_revue_effect(int player, int card, event_t event)
{
  uint8_t uVar1;
  int card_00;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int iVar4;
  unsigned int uVar5;
  int iVar6;
  target_definition_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar2 = get_card_instance(player, card);
      if ((-1 < pcVar2->damage_target_player) &&
          (iVar4 = get_id((int)pcVar2->damage_target_player, pcVar2->damage_target_card),
           iVar4 != 0x5680))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar2 = get_card_instance(player, card);
    if ((-1 < pcVar2->damage_target_player) &&
        (iVar4 = effect_follows_control_of_attachment(player, card, 0x4700), iVar4 != 0))
    {
      return 0;
    }
  LAB_025a1450:
    uVar5 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar5 == 0)
      goto LAB_025a1472;
  }
  else
  {
    pcVar2 = get_card_instance(player, card);
    if ((-1 < pcVar2->damage_target_player) &&
        (iVar4 = effect_follows_control_of_attachment(player, card, event), iVar4 != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_025a1450;

    pcVar2 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);
    if (pcVar3 != NULL)
    {

      pcVar3 = get_card_instance(affected_card_controller, affected_card);
      uVar1 = pcVar3->kill_code;
      if (uVar1 != '\0')
      {

        if (((-1 < pcVar2->damage_target_player) &&
             (pcVar2->damage_target_card == affected_card)) &&
            (pcVar2->damage_target_player == affected_card_controller))
        {
          remove_status(player, card, 0x10000);
          if (pcVar2->targets[0xb].player < 0)
          {
            pcVar2->targets[0xb].player = 0;
          }
          *(uint8_t *)((int)&pcVar2->targets[0xb].player + 1) = 1;
          return 0;
        }
        if (((player == affected_card_controller) &&
             (iVar4 = is_what(player, affected_card, 0x47), uVar1 == '\x03')) &&
            ((iVar4 != 0 &&
              (iVar4 = check_special_flags_2022(affected_card_controller, affected_card, 0x800),
               iVar4 == 0))))
        {
          if (pcVar2->targets[0xb].player < 0)
          {
            pcVar2->targets[0xb].player = 0;
          }
          *(char *)&pcVar2->targets[0xb].player = (char)pcVar2->targets[0xb].player + '\x01';
        }
      }
    }
    uVar5 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar5 == 0)
    {
      return 0;
    }
  }
  iVar4 = check_status(player, card, 0x10000);
  if (iVar4 == 0)
  {

    pcVar2 = get_card_instance(player, card);
    iVar4 = pcVar2->targets[0].player;
    card_00 = pcVar2->targets[0].card;
    default_target_definition(iVar4, card_00, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 0;
    add_counters_from_source(player, card, iVar4, card_00, COUNTER_P1_P1, uVar5 & 0xff);
    iVar6 = can_target(&td);
    if ((iVar6 != 0) && (iVar6 = new_pick_target(&td, "Select a target.", 0, 0x80), iVar6 != 0))
    {
      iVar6 = get_power(iVar4, card_00);
      damage_target0(iVar4, card_00, iVar6);
    }
    kill_card(player, card, KILL_EXILE);
  }
LAB_025a1472:
  if ((event == EVENT_CLEANUP) && (iVar4 = check_status(player, card, 0x10000), iVar4 == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a9b50; original cards/commander_legends.c:3134. */
int card_juri_master_of_the_revue(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;

  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar2 = 0;
        do
        {
          instance = in_play(player, iVar2);

          if ((((instance != NULL) && (iVar3 = is_what(player, iVar2, 0x80), iVar3 != 0)) &&
               (instance = get_card_instance(player, iVar2),
                instance->info_slot_fn == juri_master_of_the_revue_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_025a9b85;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, juri_master_of_the_revue_effect, player,
                                             card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  }
  else
  {

    if (((event == EVENT_GRAVEYARD_FROM_PLAY) &&
         (instance = in_play(affected_card_controller, affected_card), instance != NULL)) &&
        ((affected_card_controller == player &&
          ((((affected_card != card &&
              (iVar2 = is_what(player, affected_card, 0x47), iVar2 != 0)) &&
             (instance->kill_code == '\x03')) &&
            (iVar2 = check_special_flags_2022(affected_card_controller, affected_card, 0x800),
             iVar2 == 0))))))
    {

      instance = get_card_instance(player, card);
      if (instance->targets[0xb].player < 0)
      {
        instance->targets[0xb].player = 0;
      }
      *(char *)&instance->targets[0xb].player = (char)instance->targets[0xb].player + '\x01';
    }
  }
LAB_025a9b85:
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (uVar4 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar4 != 0))
  {

    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, uVar4 & 0xff);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25ab8e0; original cards/commander_legends.c:3683. */
int card_lathiel_the_bounteous_dawn(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  target_definition_t td;
  char buffer[100];

  check_legend_rule(player, card, event);
  lifelink(player, card, event);
  iVar1 = eot_trigger(player, card, event);
  if (iVar1 != 0)
  {

    iVar1 = get_trap_condition(player, 1);
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.special = 0x4000;
    if (iVar1 != 0)
    {
      iVar2 = can_target(&td);
      if (iVar2 != 0)
      {
        iVar2 = eot_trigger(player, card, event);
        if (iVar2 != 0)
        {

          instance = get_card_instance(player, card);
          if (0 < iVar1)
          {
            iVar2 = 0;
            do
            {
              iVar2 = iVar2 + 1;
              scnprintf(buffer, 100,
                        "Select target creature which will get a +1/+1 counter (%d of %d)", iVar2,
                        iVar1);
              iVar4 = new_pick_target(&td, buffer, 0, 0x80);
              if (iVar4 == 0)
                break;
              add_counters_from_source(player, card, instance->targets[0].player,
                                       instance->targets[0].card, COUNTER_P1_P1, 1);
            } while (iVar2 != iVar1);
          }
          instance->number_of_targets = '\0';
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25ad4c0; original cards/commander_legends.c:4311. */
int card_nightshade_harvester(int player, int card, event_t event)
{
  test_definition_t test;

  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 1);
    etb_ability_global(1 - player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event != EVENT_ETB)
  {
    return 0;
  }
  lose_life_2022(1 - player, 1);
  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  return 0;
}

/* ManalinkEh 2022: 0x25afc50; original cards/commander_legends.c:5154. */
int card_sengir_the_dark_baron(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  when_another_permanent_dies_add_counters_on_this(player, card, event, 2, 0, 2);
  return 0;
}

/* ManalinkEh 2022: 0x25a02b0; original cards/commander_legends.c:5242. */
static void slurrk_trigger(int player, int card)
{
  card_instance_t *instance;
  int result;
  int card_00;

  if (active_cards_count[player] < 1)
  {
    return;
  }
  card_00 = 0;
  do
  {
    while (((instance = in_play(player, card_00),
             instance != NULL && (result = is_what(player, card_00, 2), result != 0)) &&
            (result = count_counters(player, card_00, COUNTER_P1_P1), 0 < result)))
    {
      result = card_00 + 1;
      add_counters_from_source(player, card, player, card_00, COUNTER_P1_P1, 1);
      card_00 = result;
      if (active_cards_count[player] <= result)
      {
        return;
      }
    }
    card_00 = card_00 + 1;
  } while (card_00 < active_cards_count[player]);
  return;
}

/* ManalinkEh 2022: 0x25a16e0; original cards/commander_legends.c:5250. */
static int slurrk_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  int result;
  card_instance_t *pcVar4;
  event_t event_00;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x56bb))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if (-1 < pcVar1->damage_target_player)
    {
      result = effect_follows_control_of_attachment(player, card, 0x4700);
      if (result != 0)
      {
        return 0;
      }
      goto LAB_025a171c;
    }
    event_00 = EVENT_STATIC_EFFECTS;
  LAB_025a17f1:
    uVar2 = resolve_gfp_ability_2022(player, card, event_00, RESOLVE_TRIGGER_MANDATORY);
    result = check_status(player, card, 0x10000);
    if (result != 0)
    {
      return 0;
    }
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, event), result != 0))
    {
      return 0;
    }
    if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {

      pcVar1 = get_card_instance(player, card);
      pcVar4 = in_play(affected_card_controller, affected_card);

      if ((pcVar4 != NULL) && (pcVar4 = get_card_instance(affected_card_controller, affected_card),
                               pcVar4->kill_code != '\0'))
      {

        if ((-1 < pcVar1->damage_target_player) &&
            ((affected_card == pcVar1->damage_target_card &&
              (pcVar1->damage_target_player == affected_card_controller))))
        {
          pcVar1->damage_target_card = -1;
          pcVar1->damage_target_player = -1;
          remove_status(player, card, 0x10000);
          if (pcVar1->targets[0xb].player < 0)
          {
            pcVar1->targets[0xb].player = 0;
          }
          *(uint8_t *)((int)&pcVar1->targets[0xb].player + 2) = 1;
        }
        result = count_counters(affected_card_controller, affected_card, COUNTER_P1_P1);
        if (result != 0)
        {
          count_for_gfp_ability(player, card, 0x77, 2, 2, NULL);
        }
      }
      event_00 = EVENT_GRAVEYARD_FROM_PLAY;
      goto LAB_025a17f1;
    }
  LAB_025a171c:
    uVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    result = check_status(player, card, 0x10000);
    if (result != 0)
      goto LAB_025a1782;
  }

  uVar2 = uVar2 & 0xff;
  if (uVar2 != 0)
  {
    do
    {
      uVar2 = uVar2 - 1;
      slurrk_trigger(player, card);
    } while (0 < (int)uVar2);
  }
  kill_card(player, card, KILL_EXILE);
LAB_025a1782:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25aff60; original cards/commander_legends.c:5305. */
int card_slurrk_all_ingesting(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;

  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar2 = 0;
        do
        {
          instance = in_play(player, iVar2);

          if ((((instance != NULL) && (iVar3 = is_what(player, iVar2, 0x80), iVar3 != 0)) &&
               (instance = get_card_instance(player, iVar2),
                instance->info_slot_fn == slurrk_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_025aff95;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, slurrk_effect, player, card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  }
  else if ((event == EVENT_GRAVEYARD_FROM_PLAY) &&
           (iVar2 = count_counters(affected_card_controller, affected_card, COUNTER_P1_P1),
            iVar2 != 0))
  {
    count_for_gfp_ability(player, card, 0x77, player, 2, NULL);
  }
LAB_025aff95:
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (uVar4 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar4 != 0))
  {

    for (uVar4 = uVar4 & 0xff; uVar4 != 0; uVar4 = uVar4 - 1)
    {
      slurrk_trigger(player, card);
    }
  }
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, 5);
  return 0;
}

/* ManalinkEh 2022: 0x25b0450; original cards/commander_legends.c:5446. */
int card_stumpsquall_hydra(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  target_definition_t td;
  char buffer[100];

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar3 = is_token(player, card);
      if (iVar3 == 0)
      {
        generic_spell(player, card, EVENT_CAST_SPELL, 4, NULL, NULL, 0, NULL);
      }
    }
    etb_ability_2022(player, card, EVENT_CAST_SPELL, RESOLVE_TRIGGER_MANDATORY);
    return 0;
  }
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    instance = get_card_instance(player, card);
    iVar3 = instance->info_slot;
    if (iVar3 != 0)
    {

      default_target_definition(player, card, &td, 0x47);
      td.preferred_controller = player;
      td.illegal_abilities = 0;
      td.special = 0x8000;
      td.extra = (int32_t)me_or_commander;
      td.allow_cancel = 2;
      if (0 < iVar3)
      {
        iVar4 = 0;
        do
        {
          iVar5 = iVar4 + 1;
          scnprintf(buffer, 100, "Select Stumpsquall Hydra or a Commaner (counter %d of %d)", iVar5,
                    iVar3);
          iVar2 = can_target(&td);
          if (iVar2 == 0)
          {
          LAB_025b05bd:
            if (iVar4 < iVar3)
            {
              add_counters_from_source(player, card, player, card, COUNTER_P1_P1, iVar3 - iVar4);
            }
            break;
          }
          iVar2 = new_pick_target(&td, buffer, 0, 0x80);
          if (iVar2 == 0)
            goto LAB_025b05bd;
          add_counters_from_source(player, card, instance->targets[0].player,
                                   instance->targets[0].card, COUNTER_P1_P1, 1);
          iVar4 = iVar5;
        } while (iVar5 != iVar3);
      }
      instance->number_of_targets = '\0';
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a3810; original cards/commander_legends.c:470. */
int card_apex_devastator(int player, int card, event_t event)
{
  multicascade(player, card, event, 4);
  return 0;
}

/* ManalinkEh 2022: 0x25a44f0; original cards/commander_legends.c:861. */
int card_belbe_corrupted_observer(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  result = beginning_of_phase(player, card, event, 2, -1, 0x1e);
  if (result != 0)
  {
    result = get_trap_condition(1 - player, 2);
    if (result != 0)
    {
      instance = in_play(player, card);
      if (instance != NULL)
      {
        result = is_humiliated_for_ability(player, card, AL_STATIC);
        if (result == 0)
        {
          produce_mana(current_turn, 0, 2);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a8470; original cards/commander_legends.c:2448. */
int card_halana_kessig_ranger(int player, int card, event_t event)
{
  int src_card;
  int result;
  card_instance_t *instance;
  int amt;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_test;

  check_legend_rule(player, card, event);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&recovered_stack_test.test, 2);
    recovered_stack_test.test.not_me = 1;
    result = duh_mode(player);
    etb_ability_global(player, event, (result != 0) + 1, TYPE_NONE, &recovered_stack_test.test,
                       player, card);
  }
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &recovered_stack_test.target, 2);
    result = can_target(&recovered_stack_test.target);
    if (result != 0)
    {
      result = has_mana(player, 0, 2);
      if (result != 0)
      {
        charge_mana(player, COLOR_COLORLESS, 2);
        if (spell_fizzled != 1)
        {
          result = new_pick_target(&recovered_stack_test.target, "TARGET_CREATURE", 0, 1);
          if (result != 0)
          {

            instance = get_card_instance(player, card);
            src_card = trigger_cause;
            result = trigger_cause_controller;
            amt = get_power(trigger_cause_controller, trigger_cause);
            damage_creature(instance->targets[0].player, instance->targets[0].card, amt, result,
                            src_card);
            instance->number_of_targets = '\0';
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a8760; original cards/commander_legends.c:2540. */
int card_hans_eriksson(int player, int card, event_t event)
{
  int *piVar1;
  int result;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  reveal_top_cards_of_library(player, 1);
  result = is_what(-1, *deck_ptr[player], 2);
  if (result != 0)
  {

    piVar1 = deck_ptr[player];
    result = is_what(-1, *piVar1, 2);
    if ((result == 0) ||
        (result = grafdiggers_cages(player, TYPE_CREATURE, TUTOR_FROM_DECK), result == 0))
    {
      result = add_card_to_hand(player, *piVar1);
      remove_card_from_deck(player, 0);
      add_state(player, result, 0x14);
      set_special_flags_2022(player, result, 0x50000);
      put_into_play(player, result);
      if (result == 0)
      {
        return 0;
      }
    }
    else
    {
      result = -1;
    }
    fight(player, result, player, card);
    return 0;
  }
  add_card_to_hand(player, *deck_ptr[player]);
  obliterate_top_card_of_deck(player);
  return 0;
}

/* ManalinkEh 2022: 0x25a9200; original cards/commander_legends.c:2885. */
int card_jeskas_will(int player, int card, event_t event)
{
  unsigned char bVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  unsigned int uVar5;

  if (event == EVENT_CAN_CAST)
  {
    get_card_instance(player, card);
    iVar3 = basic_spell(player, card, EVENT_CAN_CAST);
    return iVar3;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        iVar3 = opponent_is_valid_target(player, card);
        uVar5 = 0;
        if (iVar3 != 0)
        {
          iVar4 = liutenant(player);
          uVar5 = (unsigned int)(iVar4 != 0);
        }
        uVar5 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033", "Mana", iVar3,
                            ((3 < hand_count[1 - player]) - 1 & 0xffffffed) + 0x14, "Exile", 1,
                            (unsigned int)(current_turn != player) * 9 + 1, "Do both", uVar5, 0x1e,
                            0, 0, 0, 0);
        if (uVar5 == 0)
        {
          spell_fizzled = 1;
        }
        else
        {
          if ((uVar5 & 1) != 0)
          {
            target_opponent(player, card);
            ai_modifier = ai_modifier + hand_count[1 - player] * 10;
          }
          if ((uVar5 & 2) != 0)
          {
            ai_modifier = ai_modifier + 0x32;
          }
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      bVar1 = instance->mode;
      if ((bVar1 & 1) != 0)
      {
        iVar3 = opponent_is_valid_target(player, card);
        if (iVar3 != 0)
        {
          produce_mana(player, 4, hand_count[1 - player]);
        }
      }
      if ((bVar1 & 2) != 0)
      {
        exile_top_x_card_you_may_play_them(player, card, 3, NULL, player, MPCFE_UNTIL_EOT);
      }
      kill_card(player, card, KILL_BURY);
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    get_card_instance(player, card);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25aaab0; original cards/commander_legends.c:3409. */
int card_keeper_of_the_accord(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  const char *pcVar5;
  int l_count[2];
  int c_count[2];
  char buffer[50];
  test_definition_t test;

  iVar1 = eot_trigger(player, card, event);
  iVar4 = current_turn;
  if ((iVar1 != 0) && (current_turn != player))
  {

    l_count[0] = 0;
    iVar1 = active_cards_count[current_turn] + -1;
    l_count[1] = 0;
    c_count[0] = 0;
    c_count[1] = 0;
    if (-1 < iVar1)
    {
      do
      {
        instance = in_play(iVar4, iVar1);
        if (instance != NULL)
        {
          iVar3 = is_what(iVar4, iVar1, 2);
          if (iVar3 != 0)
          {
            c_count[iVar4] = c_count[iVar4] + 1;
          }
          iVar3 = is_what(iVar4, iVar1, 1);
          if (iVar3 != 0)
          {
            l_count[iVar4] = l_count[iVar4] + 1;
          }
        }
        iVar1 = iVar1 + -1;
      } while (iVar1 != -1);
    }
    iVar4 = 1 - current_turn;
    iVar1 = active_cards_count[iVar4] + -1;
    if (-1 < iVar1)
    {
      do
      {
        instance = in_play(iVar4, iVar1);
        if (instance != NULL)
        {
          iVar3 = is_what(iVar4, iVar1, 2);
          if (iVar3 != 0)
          {
            c_count[iVar4] = c_count[iVar4] + 1;
          }
          iVar3 = is_what(iVar4, iVar1, 1);
          if (iVar3 != 0)
          {
            l_count[iVar4] = l_count[iVar4] + 1;
          }
        }
        iVar1 = iVar1 + -1;
      } while (iVar1 != -1);
    }
    if (c_count[player] < c_count[1 - player])
    {
      generate_token_by_id_2022(player, card, 0x4986);
    }
    pcVar5 = get_hacked_land_text(player, card, " Search for a basic %s\n Decline", 0x5007);
    scnprintf(buffer, 0x32, pcVar5);
    if ((l_count[player] < l_count[1 - player]) &&
        (iVar4 = do_dialog(player, player, card, -1, -1, buffer, 0), iVar4 == 0))
    {
      pcVar5 = get_hacked_land_text(player, card, "Select a basic %s card.", 0x5007);
      new_default_test_definition(&test, 1, pcVar5);
      test.subtype = get_hacked_subtype(player, card, 0x5007);
      test.sub2 = 0x2000;
      test.subtype_flag = 4;
      new_global_tutor(player, player, 1, 5, 0, 5, &test);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25b19d0; original cards/commander_legends.c:5982. */
int card_wheel_of_misfortune(int player, int card, event_t event)
{
  int player_00;
  int result;
  unsigned int argument1;
  int argument2;
  int number[2];
  char buffer[100];

  if (event == EVENT_RESOLVE_SPELL)
  {
    number[1] = 1;
    result = number[1];
    if (6 < life[1])
    {
      number[1] = internal_rand(life[1] + -6);
      result = number[1];
    }
    do
    {
      number[0] = choose_a_number(0, "Choose a number 0 or greater", 1);
    } while (number[0] < 0);
    scnprintf(buffer, 100, "Opponent chosen: %d", result);
    argument1 = 1 - player;
    do_dialog(0, player, card, -1, -1, buffer, 0);
    player_00 = current_turn;
    result = number[player];
    argument2 = number[argument1];
    if (argument2 < result)
    {
      damage_player(player, result, player, card);
      new_discard_all(player, player);
      draw_cards(player, 7);
    }
    else
    {
      if (argument2 <= result)
      {

        damage_player(current_turn, number[current_turn], player, card);
        new_discard_all(player_00, player);
        draw_cards(player_00, 7);
        argument1 = current_turn ^ 1;
        argument2 = number[argument1];
      }
      damage_player(argument1, argument2, player, card);
      new_discard_all(argument1, player);
      draw_cards(argument1, 7);
    }
    kill_card(player, card, KILL_BURY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a50e0; original cards/commander_legends.c:1177. */
int card_brinelin_the_moon_kraken(int player, int card, event_t event)
{
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    brinelin_ability(player, card);
    if (trigger_condition != 0xd3)
    {
      return 0;
    }
  }
  else if (trigger_condition != 0xd3)
    goto LAB_025a5131;
  if (trigger_cause_controller == player)
  {
    default_test_definition(&this_test, 0xff);
    this_test.cmc = 5;
    this_test.cmc_flag = 2;
    sp_ability_trigger(player, event, 2, TYPE_NONE, &this_test, player, card);
  }
LAB_025a5131:
  if (event != EVENT_SPA)
  {
    return 0;
  }
  brinelin_ability(player, card);
  return 0;
}

/* ManalinkEh 2022: 0x25ab540; original cards/commander_legends.c:3615. */
int card_krark_the_thumbless(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  sp_ability_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }

  result = flip_a_coin(player, card);
  if (result != 0)
  {
    copy_spell_from_stack_2022(player, trigger_cause_controller, trigger_cause);
    return 0;
  }
  bounce_permanent(trigger_cause_controller, trigger_cause);
  return 0;
}

/* ManalinkEh 2022: 0x25aba70; original cards/commander_legends.c:3717. */
int card_liesa_shroud_of_dusk(int player, int card, event_t event)
{
  check_legend_rule(player, card, event);
  lifelink(player, card, event);
  sp_ability_trigger(2, event, 2,
                     TARGET_TYPE_PLANESWALKER | TYPE_ARTIFACT | TYPE_SPELL | TYPE_ENCHANTMENT |
                         TYPE_CREATURE,
                     NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  lose_life_2022(trigger_cause_controller, 2);
  return 0;
}

/* ManalinkEh 2022: 0x25ad860; original cards/commander_legends.c:4383. */
int card_nymris_oonas_trickster(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (((trigger_condition == 0xd3) && (1 - player == current_turn)) &&
      (trigger_cause_controller == player))
  {
    result = get_specific_storm_count(player);
    if (result == 0)
    {
      sp_ability_trigger(player, event, 2, TYPE_ANY, NULL, player, card);
    }
  }
  if (event == EVENT_SPA)
  {
    new_default_test_definition(&test, 0xff, "Select a card to put into your hand.");
    reveal_top_cards_of_library_and_choose(player, card, player, 2, 1, TUTOR_HAND, 0, TUTOR_GRAVE,
                                           0, &test);
  }
  result = flash(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int encore_granted_legacy(int player, int card, event_t event);
static int encore_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25a1e20; original cards/commander_legends.c:9. */
static int encore_legacy(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int result;
  int player_00;

  if (event == EVENT_SET_LEGACY_EFFECT_NAME)
  {
    instance = get_card_instance(player, card);
    if (-1 < instance->targets[0].card)
    {
      scnprintf(set_legacy_effect_name_addr, 0x33, "Encore");
    }
  }
  else if (event == EVENT_SET_LEGACY_EFFECT_TEXT)
  {
    scnprintf(set_legacy_effect_name_addr, 400,
              "Encore\n Exile this card from your graveyard: For each opponent, create a token "
              "copy that attacks that opponent this turn if able. They gain haste. Sacrifice them "
              "at the beginning of the next end step.");
  }
  if (((unsigned int)player < 3) && ((unsigned int)card < 0x97))
  {

    instance = get_card_instance(player, card);
    if (instance != NULL)
    {

      player_00 = (int)instance->damage_target_player;
      card_00 = instance->damage_target_card;
      if ((-1 < player_00) && (-1 < card_00))
      {
        haste(player_00, card_00, event);
        attack_if_able_2022(player_00, card_00, event);
        result = eot_trigger(player, card, event);
        if (result != 0)
        {
          result = check_state(player_00, card_00, STATE_OUBLIETTED);
          if (result == 0)
          {
            kill_card(player_00, card_00, KILL_SACRIFICE);
          }
          else
          {
            kill_card(player, card, KILL_EXILE);
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a2850; original cards/commander_legends.c:43. */
static int encore(int player, int card, event_t event, card_id_t cid, int x, int b, int u, int g,
                  int r, int w)
{
  int result;
  token_generation_t token;

  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = has_mana_multi(player, x, b, u, g, r, w);
    if ((result != 0) &&
        (result = can_sorcery_be_played(player, EVENT_GRAVEYARD_ABILITY), result != 0))
    {
      return 0x20;
    }
  }
  else if (event == EVENT_PAY_FLASHBACK_COSTS)
  {
    charge_mana_multi(player, x, b, u, g, r, w);
    if (spell_fizzled != 1)
    {
      return 2;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATED_GRAVEYARD_ABILITY)
  {
    default_token_definition(player, card, cid, &token);
    token.legacy = '\x01';
    token.special_code_for_legacy = encore_legacy;
    generate_token(&token);
  }
  return (unsigned int)(event == EVENT_CAN_CAST);
}

/* ManalinkEh 2022: 0x25a0440; original cards/commander_legends.c:481. */
static int encore_granted_legacy(int player, int card, event_t event)
{
  unsigned short uVar1;
  int player_00;
  int source;
  card_instance_t *instance;
  int result;
  token_generation_t token;

  if (event != EVENT_CLEANUP)
  {
    if (event != EVENT_STATIC_EFFECTS)
      goto LAB_025a0464;
    instance = get_card_instance(player, card);
    if (instance->targets[1].card != -1)
    {
      return 0;
    }
  }
  kill_card(player, card, KILL_EXILE);
LAB_025a0464:
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (instance->targets[5].player == 0x42)
      {
        return 0;
      }
      result = find_in_graveyard_by_source(instance->targets[2].card, instance->targets[1].player,
                                           instance->targets[1].card);
      instance->targets[1].card = result;
      if (result == -1)
      {
        return 0;
      }
      result = has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, instance->targets[0].card);
      if (result == 0)
      {
        return 0;
      }
      result = can_sorcery_be_played(player, EVENT_CAN_ACTIVATE);
      if (result == 0)
      {
        return 0;
      }
      return 1;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_ACTIVATE)
  {

    result = instance->targets[1].card;
    player_00 = instance->targets[2].card;
    uVar1 = cards_data[instance->targets[0].card].id;
    source = instance->targets[1].player;
    charge_mana_from_id(player, -1, EVENT_ACTIVATE, (unsigned int)uVar1);
    if (spell_fizzled != 1)
    {
      result = find_in_graveyard_by_source(player_00, source, result);
      rfg_card_from_grave(player_00, result);
      instance->targets[5].player = 0x42;
      instance->targets[5].card = (unsigned int)uVar1;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    default_token_definition(instance->parent_controller, instance->parent_card,
                             instance->targets[5].card, &token);
    token.legacy = '\x01';
    token.special_code_for_legacy = encore_legacy;
    generate_token(&token);
    instance->targets[1].card = -1;
    instance->targets[1].player = -1;
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a3840; original cards/commander_legends.c:552. */
int card_araumi_of_the_dead_tide(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int card_00;
  card_instance_t *pcVar6;
  card_instance_t *pcStack_1c4;
  test_definition_t test;
  test_definition_t test2;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    pcStack_1c4 = get_card_instance(player, card);
    new_default_test_definition(&test, 2, "Select target creature card.");
    test.ai_selection_mode = 0x101;
    if (event == EVENT_CAN_ACTIVATE)
    {

      iVar5 = generic_activated_ability_with_target_in_grave(player, card, EVENT_CAN_ACTIVATE,
                                                             GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0,
                                                             NULL, NULL, TYPE_NONE, &test, 8);
      if (iVar5 == 0)
      {
        return 0;
      }
      if (player != 0)
      {
        iVar3 = count_graveyard(player);
        if (iVar3 < 2)
        {
          return 0;
        }
        return iVar5;
      }
      return iVar5;
    }
    if (event == EVENT_ACTIVATE)
    {

      new_default_test_definition(&test2, 0xff, "Select a card to exile.");
      test2.ai_selection_mode = 0x303;
      test.id = 0x388;
      test.id_flag = 1;
      iVar5 = new_select_a_card(player, player, 2, 0, 3, -1, &test2);
      if (iVar5 == -1)
      {
        return 0;
      }

      iVar3 = turn_card_in_grave_face_down(player, iVar5);
      iVar4 = new_special_count_grave(player, &test);
      if (iVar4 == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        generic_activated_ability_with_target_in_grave(player, card, EVENT_ACTIVATE, GAA_UNTAPPED,
                                                       0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
                                                       &test, 8);
      }
      turn_card_in_grave_face_up(player, iVar5, iVar3);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      rfg_card_from_grave(player, iVar5);
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    pcStack_1c4 = get_card_instance(player, card);
    new_default_test_definition(&test, 2, "Select target creature card.");
  }
  test.ai_selection_mode = 0x101;

  if ((event == EVENT_RESOLVE_ACTIVATION) &&
      (iVar5 = validate_target_from_grave_source(player, card, player, 0), iVar5 != -1))
  {

    iVar3 = pcStack_1c4->parent_card;
    iVar4 = pcStack_1c4->parent_controller;
    if (iVar5 < 0)
    {
      backtrace_and_abort("cards/commander_legends.c:539\ngraveyard_pos >= 0");
    }
    card_00 = create_legacy_activate(iVar4, iVar3, encore_granted_legacy);
    pcVar6 = get_card_instance(iVar4, card_00);
    piVar1 = graveyard_ptr_mutable[iVar4];
    pcVar6->targets[0].card = piVar1[iVar5];
    iVar2 = graveyard_source[iVar4][iVar5];
    pcVar6->targets[1].card = iVar5;
    pcVar6->targets[1].player = iVar2;
    pcVar6->targets[2].card = iVar4;
    pcVar6->targets[3].player = iVar4;
    pcVar6->targets[3].card = iVar3;
    create_card_name_legacy(iVar4, card_00, (unsigned int)cards_data[piVar1[iVar5]].id);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a4450; original cards/commander_legends.c:841. */
int card_azure_fleet_admiral(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    if (event == EVENT_BLOCK_LEGALITY)
    {
      instance = in_play(player, card);
      if (instance != NULL)
      {
        result = is_humiliated_for_ability(player, card, AL_STATIC);
        if (result == 0)
        {
          result = who_is_the_monarch();
          if (result != player)
          {
            event_result = 1;
          }
        }
      }
    }
    return 0;
  }
  you_become_the_monarch(player);
  return 0;
}

/* ManalinkEh 2022: 0x25a6530; original cards/commander_legends.c:1706. */
int card_court_of_ambition(int player, int card, event_t event)
{
  int iVar1;
  int argument1;
  int iVar2;
  int quantity;
  char buffer[100];

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    you_become_the_monarch(player);
    upkeep_trigger_ability(player, card, EVENT_ETB, player);
  }
  else
  {
    upkeep_trigger_ability(player, card, event, player);
    if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    {

      iVar1 = who_is_the_monarch();
      argument1 = 1 - player;
      quantity = (iVar1 == player) + 1;
      iVar1 = (unsigned int)(iVar1 == player) * 3 + 3;
      if (quantity <= hand_count[argument1])
      {

        scnprintf(buffer, 100, " Lose %d life\n Discard %d cards", iVar1, quantity);
        iVar2 =
            do_dialog(argument1, player, card, -1, -1, buffer, (unsigned int)(life[argument1] < 7));
        if (iVar2 != 0)
        {
          new_multidiscard(argument1, quantity, 2, player);
          goto LAB_025a6581;
        }
      }
      lose_life(argument1, iVar1);
    }
  }
LAB_025a6581:
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x25a66a0; original cards/commander_legends.c:1749. */
int card_court_of_bounty(int player, int card, event_t event)
{
  int result;
  int iStack_ec;
  test_definition_t test;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    you_become_the_monarch(player);
    upkeep_trigger_ability(player, card, EVENT_ETB, player);
  }
  else
  {
    upkeep_trigger_ability(player, card, event, player);
    if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    {

      new_default_test_definition(&test, 1, "Select a Land card.");
      result = who_is_the_monarch();
      iStack_ec = 1;
      if (result == player)
      {
        new_default_test_definition(&test, 3, "Select a Creature or Land card.");
        iStack_ec = 2;
      }
      test.zone = 0x100;
      result = check_battlefield_for_special_card(player, card, player, 4, &test);
      if (result != 0)
      {
        test.zone = 0x200;
        new_global_tutor(player, player, 4, 4, 0, iStack_ec, &test);
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25a6810; original cards/commander_legends.c:1781. */
int card_court_of_cunning(int player, int card, event_t event)
{
  size_t __n_1;
  int result;
  uint32_t uVar2;
  unsigned int uVar3;
  uint32_t uVar4;
  uint32_t uVar5;
  target_definition_t td;
  char str1[100];
  char str2[100];
  char str3[100];

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    you_become_the_monarch(player);
    upkeep_trigger_ability(player, card, EVENT_ETB, player);
  }
  else
  {
    upkeep_trigger_ability(player, card, event, player);
    if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    {

      result = who_is_the_monarch();
      __n_1 = (unsigned int)(result == player) * 8 + 2;
      default_target_definition(player, card, &td, 0xff);
      td.zone = 0x1000;
      snprintf(str1, 100, "Opponent mills %d cards", __n_1);
      snprintf(str2, 100, "You mill %d cards", __n_1);
      snprintf(str3, 100, "Both players mill %d cards", __n_1);
      result = would_validate_arbitrary_target(&td, 1 - player, -1);
      if (result == 0)
      {
        result = would_validate_arbitrary_target(&td, player, -1);
        if (result == 0)
        {
          uVar4 = 0;
          uVar2 = 0;
          uVar5 = 0;
        }
        else
        {
          uVar2 = 0;
          uVar4 = 2;
          uVar5 = 0;
        }
      }
      else
      {
        result = would_validate_arbitrary_target(&td, player, -1);
        if (result == 0)
        {
          uVar4 = 0;
          uVar5 = 1;
          uVar2 = 0;
        }
        else
        {
          uVar5 = 1;
          uVar4 = 2;
          uVar2 = 4;
        }
      }
      uVar3 = dialog_impl(player, card, EVENT_RESOLVE_ACTIVATION, "\001\002", "\001\014", str1,
                          uVar5, 0x32, str2, uVar4, 0xffffffce, str3, uVar2, 0xffffffce,
                          "Do Nothing", 1, 0xffffffce, 0, 0, 0, 0);
      if ((uVar3 & 1) != 0)
      {
        mill(1 - player, __n_1);
      }
      if ((uVar3 & 2) != 0)
      {
        mill(player, __n_1);
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25a6af0; original cards/commander_legends.c:1838. */
int card_court_of_grace(int player, int card, event_t event)
{
  int result;
  token_generation_t token;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    you_become_the_monarch(player);
    upkeep_trigger_ability(player, card, EVENT_ETB, player);
  }
  else
  {
    upkeep_trigger_ability(player, card, event, player);
    if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    {
      default_token_definition(player, card, 0x498d, &token);
      token.tou = 1;
      token.pow = 1;
      token.color_forced = ' ';
      token.key_plus = 0x20;
      result = who_is_the_monarch();
      if (result == player)
      {
        default_token_definition(player, card, 0x48ff, &token);
        token.tou = 4;
        token.pow = 4;
      }
      generate_token(&token);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25a6c00; original cards/commander_legends.c:1869. */
int card_court_of_ire(int player, int card, event_t event)
{
  size_t __n_1;
  int result;
  card_instance_t *instance;
  target_definition_t td1;
  char msg[100];

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    you_become_the_monarch(player);
    upkeep_trigger_ability(player, card, EVENT_ETB, player);
  }
  else
  {
    upkeep_trigger_ability(player, card, event, player);
    if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    {

      result = who_is_the_monarch();
      __n_1 = (unsigned int)(result == player) * 5 + 2;
      default_target_definition(player, card, &td1, 2);
      td1.zone = 0x1200;
      instance = get_card_instance(player, card);
      result = can_target(&td1);
      if (result != 0)
      {
        snprintf(msg, 100, "Select a target (%d damage).", __n_1);
        result = new_pick_target(&td1, msg, 0, 0x80);
        if (result != 0)
        {
          damage_creature(instance->targets[0].player, instance->targets[0].card, __n_1, player,
                          card);
        }
      }
      instance->number_of_targets = '\0';
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25a6d90; original cards/commander_legends.c:1924. */
int card_dawnglade_regent(int player, int card, event_t event)
{
  int result;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    if ((event == EVENT_ABILITIES) && (affected_card_controller == player))
    {
      result = is_what(player, affected_card, 0x47);
      if (result != 0)
      {
        result = is_humiliated_for_ability(player, card, AL_STATIC);
        if (result == 0)
        {
          result = who_is_the_monarch();
          if (result == player)
          {
            hexproof_2022(affected_card_controller, affected_card, EVENT_ABILITIES);
          }
        }
      }
    }
    return 0;
  }
  you_become_the_monarch(player);
  return 0;
}

/* ManalinkEh 2022: 0x25a6ff0; original cards/commander_legends.c:1996. */
int card_emberwilde_captain(int player, int card, event_t event)
{
  int attacker_player;
  int argument3;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  attacker_player = 1 - player;
  if (event != EVENT_ETB)
  {
    store_attackers(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, attacker_player, -1, NULL);
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    if (event == EVENT_DAT_ABILITY)
    {
      argument3 = who_is_the_monarch();
      if (argument3 == player)
      {
        damage_player(attacker_player, hand_count[attacker_player], argument3, card);
      }
    }
    return 0;
  }
  you_become_the_monarch(player);
  store_attackers(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, attacker_player, -1,
                  NULL);
  dat_ability(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  return 0;
}

/* ManalinkEh 2022: 0x25a7260; original cards/commander_legends.c:2067. */
int card_fall_from_favor(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int player_00;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    you_become_the_monarch(player);
    instance = get_card_instance(player, card);
    if ((-1 < instance->damage_target_player) && (-1 < instance->damage_target_card))
    {
      tap_card((int)instance->damage_target_player, instance->damage_target_card);
    }
  }
  else if (event == EVENT_UNTAP)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      iVar1 = is_humiliated_for_ability(player, card, AL_STATIC);
      if (iVar1 == 0)
      {

        instance = get_card_instance(player, card);
        player_00 = (int)instance->damage_target_player;
        iVar1 = instance->damage_target_card;
        if ((((-1 < player_00) && (-1 < iVar1)) && (current_turn == player_00)) &&
            (((current_phase == 1 && (iVar1 == affected_card)) &&
              (player_00 == affected_card_controller))))
        {
          iVar3 = who_is_the_monarch();
          if (iVar3 != player_00)
          {
            does_not_untap(player_00, iVar1, EVENT_UNTAP);
          }
        }
      }
    }
  }
  iVar1 = disabling_aura_2022(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x25a75b0; original cards/commander_legends.c:2135. */
int card_feast_of_succession(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    pump_creatures_until_eot(player, card, 2, 0, -4, -4, 0, 0, NULL);
    you_become_the_monarch(player);
    kill_card(player, card, KILL_BURY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25a90a0; original cards/commander_legends.c:2852. */
int card_jared_carthalion_true_heir(int player, int card, event_t event)
{
  char cVar1;
  int num;
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    if (event == EVENT_PREVENT_DAMAGE)
    {
      instance = in_play(player, card);
      if (instance != NULL)
      {
        result = is_humiliated_for_ability(player, card, AL_STATIC);
        if (result == 0)
        {

          instance = get_card_instance(affected_card_controller, affected_card);
          if ((((instance->internal_card_id == damage_card) && (0 < instance->info_slot)) &&
               (cVar1 = instance->damage_target_player, cVar1 == player)) &&
              (instance->damage_target_card == card))
          {
            result = who_is_the_monarch();
            if (result == cVar1)
            {

              num = instance->info_slot;
              prevent_damage(affected_card_controller, affected_card, -1);
              add_counters_from_source(result, card, result, card, COUNTER_P1_P1, num);
            }
          }
        }
      }
    }
    return 0;
  }
  you_become_the_monarch(1 - player);
  create_legacy_effect(player, card, jared_carthalion_effect);
  return 0;
}

/* ManalinkEh 2022: 0x25b03e0; original cards/commander_legends.c:5419. */
int card_staunch_throneguard(int player, int card, event_t event)
{
  vigilance_2022(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  you_become_the_monarch(player);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a3450; original cards/commander_legends.c:388. */
int card_amphin_mutineer(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_AMPHIN_MUTINEER = 22067
  };

  int result;
  card_instance_t *instance;
  token_generation_t token;
  target_definition_t td1;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td1, 2);
    td1.required_subtype = 0xa0;
    td1.special = 0x1000;
    instance = get_card_instance(player, card);
    result = can_target(&td1);
    if (result != 0)
    {
      result = new_pick_target(&td1, "Select up to one target non-Salamander creature", 0, 0x80);
      if (result != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
        default_token_definition(player, card, 0x56b6, &token);
        token.t_player = instance->targets[0].player;
        token.color_forced = '\x04';
        generate_token(&token);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = encore(player, card, event, CARD_ID_AMPHIN_MUTINEER, 4, 0, 2, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25a4f90; original cards/commander_legends.c:1138. */
int card_briarblade_adept(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_BRIARBLADE_ADEPT = 22091
  };

  int result;
  card_instance_t *instance;
  target_definition_t td;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = 1 - player;
    td.allow_cancel = 0;
    td.preferred_controller = td.allowed_controller;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Select target creature controlled by an opponent.", 0, 0x80);
      if (result != 0)
      {
        pump_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card, -1,
                       -1);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = encore(player, card, event, CARD_ID_BRIARBLADE_ADEPT, 3, 1, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25a5280; original cards/commander_legends.c:1224. */
int card_coastline_marauders(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_COASTLINE_MARAUDERS = 22094
  };

  int result;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    result = count_permanents_by_type(1 - player, TYPE_LAND);
    pump_until_eot_2022(player, card, player, card, result, 0);
  }
  result = encore(player, card, event, CARD_ID_COASTLINE_MARAUDERS, 4, 0, 0, 0, 2, 0);
  return result;
}

/* ManalinkEh 2022: 0x25a6f00; original cards/commander_legends.c:1977. */
int card_elvish_dreadlord(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_ELVISH_DREADLORD = 22109
  };

  int result;
  test_definition_t this_test;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    default_test_definition(&this_test, 2);
    this_test.subtype = 0x3c;
    this_test.subtype_flag = 1;
    pump_creatures_until_eot(player, card, 2, 0, -3, -3, 0, 0, &this_test);
  }
  result = encore(player, card, event, CARD_ID_ELVISH_DREADLORD, 5, 2, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25a7130; original cards/commander_legends.c:2032. */
int card_exquisite_huntmaster(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_EXQUISITE_HUNTMASTER = 22113
  };

  int result;
  token_generation_t token;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    default_token_definition(player, card, 0x492c, &token);
    generate_token(&token);
  }
  result = encore(player, card, event, CARD_ID_EXQUISITE_HUNTMASTER, 4, 1, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25a7490; original cards/commander_legends.c:2115. */
int card_fathom_fleet_swordjack(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_FATHOM_FLEET_SWORDJACK = 22117
  };

  int result;
  int amt;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    result = planeswalker_its_attacking(player, card);
    if (result < 0)
    {
      result = count_subtype(player, 0x40, -1);
      damage_player(1 - player, result, player, card);
    }
    else
    {
      amt = count_subtype(player, 0x40, -1);
      damage_creature(1 - player, result, amt, player, card);
    }
  }
  result = encore(player, card, event, CARD_ID_FATHOM_FLEET_SWORDJACK, 5, 0, 0, 0, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x25a7650; original cards/commander_legends.c:2151. */
int card_fin_clade_fugitives(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_FIN_CLADE_FUGITIVES = 22119
  };

  int result;

  if (event == EVENT_BLOCK_LEGALITY)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (((result == 0) && (attacking_card_controller == player)) && (attacking_card == card))
    {
      result = get_power(affected_card_controller, affected_card);
      if (result < 3)
      {
        event_result = event_result | 1;
      }
    }
  }
  result = encore(player, card, event, CARD_ID_FIN_CLADE_FUGITIVES, 4, 0, 0, 1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25a8eb0; original cards/commander_legends.c:2809. */
int card_impulsive_pilferer(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_IMPULSIVE_PILFERER = 22138
  };

  int result;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    generate_token_by_id_2022(player, card, 0x4998);
  }
  result = encore(player, card, event, CARD_ID_IMPULSIVE_PILFERER, 3, 0, 0, 0, 1, 0);
  return result;
}

/* ManalinkEh 2022: 0x25aa7a0; original cards/commander_legends.c:3345. */
int card_kangees_lieutenant(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_KANGEES_LIEUTENANT = 22147
  };

  int iVar1;
  card_instance_t *instance;
  int iVar3;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);

  if ((event == EVENT_DAT_ABILITY) && (iVar1 = active_cards_count[player] + -1, -1 < iVar1))
  {
    do
    {
      instance = in_play(player, iVar1);
      if ((instance != NULL) &&
          (((iVar3 = is_what(player, iVar1, 2),
             iVar3 != 0 && (iVar3 = is_attacking(player, iVar1), iVar3 != 0)) &&
            (iVar3 = check_for_ability(player, iVar1, 0x20), iVar3 != 0))))
      {
        pump_until_eot_merge_previous_2022(player, card, player, iVar1, 1, 1);
      }
      iVar1 = iVar1 + -1;
    } while (iVar1 != -1);
  }
  iVar1 = encore(player, card, event, CARD_ID_KANGEES_LIEUTENANT, 5, 0, 0, 0, 0, 1);
  return iVar1;
}

/* ManalinkEh 2022: 0x25ab110; original cards/commander_legends.c:3517. */
int card_kinsbaile_courier(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_KINSBAILE_COURIER = 22153
  };

  int result;
  card_instance_t *instance;
  target_definition_t td;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 0;
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_CREATURE");
      if (result != 0)
      {
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 1);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = encore(player, card, event, CARD_ID_KINSBAILE_COURIER, 2, 0, 0, 0, 0, 1);
  return result;
}

/* ManalinkEh 2022: 0x25ab240; original cards/commander_legends.c:3552. */
int card_kitesail_skirmisher(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_KITESAIL_SKIRMISHER = 22154
  };

  int result;
  card_instance_t *instance;
  target_definition_t td;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    base_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.special = 0x8000;
    td.required_state = 2;
    td.extra = (int32_t)attacking_the_same_player_or_planeswalker;
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(
          &td, "Select target creature attacking the same player or planeswalker.", 0, 0x80);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        pump_ability_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card,
                               0, 0, 0x20, 0);
      }
    }
  }
  result = encore(player, card, event, CARD_ID_KITESAIL_SKIRMISHER, 4, 0, 1, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25ada80; original cards/commander_legends.c:4434. */
int card_phyrexian_triniform(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_PHYREXIAN_TRINIFORM = 22179
  };

  int result;
  token_generation_t token;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    default_token_definition(player, card, 0x56dc, &token);
    token.tou = 3;
    token.pow = 3;
    token.qty = '\x03';
    generate_token(&token);
  }
  result = encore(player, card, event, CARD_ID_PHYREXIAN_TRINIFORM, 0xc, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25ae740; original cards/commander_legends.c:4719. */
int card_rakshasa_debaser(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_RAKSHASA_DEBASER = 22187
  };

  int result;
  int player_00;
  test_definition_t test;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    player_00 = 1 - player;
    result = count_graveyard_by_type(player_00, 2);
    if (result != 0)
    {
      result = graveyard_has_shroud(player_00);
      if (result == 0)
      {
        new_default_test_definition(&test, 2, "Select a creature card.");
        new_global_tutor(player, player_00, 2, 4, 1, 2, &test);
      }
    }
  }
  result = encore(player, card, event, CARD_ID_RAKSHASA_DEBASER, 6, 2, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x25b0170; original cards/commander_legends.c:5360. */
int card_soul_of_eternity(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_SOUL_OF_ETERNITY = 22204
  };

  int result;

  if (((event - EVENT_POWER < 2) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    set_base_pt(life[player]);
  }
  result = encore(player, card, event, CARD_ID_SOUL_OF_ETERNITY, 7, 0, 0, 0, 0, 2);
  return result;
}

/* ManalinkEh 2022: 0x25b1480; original cards/commander_legends.c:5846. */
int card_trove_tracker(int player, int card, event_t event)
{
  enum
  {
    CARD_ID_TROVE_TRACKER = 22221
  };

  int result;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    draw_a_card(player);
  }
  result = encore(player, card, event, CARD_ID_TROVE_TRACKER, 5, 0, 2, 0, 0, 0);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int count_how_many_times_a_commander_was_played(int player);

/* ManalinkEh 2022: 0x25a07d0; original cards/commander_legends.c:65. */
static int count_how_many_times_a_commander_was_played(int player)
{
  int iVar1;
  int iVar2;

  iVar1 = locate_edh_card_2022(player);
  if (iVar1 == -1)
  {
    return 0;
  }
  iVar2 = count_counters(player, iVar1, COUNTER_VORTEX);
  iVar1 = count_counters(player, iVar1, COUNTER_VELOCITY);
  return iVar1 / 2 + iVar2 / 2;
}

/* ManalinkEh 2022: 0x25a51e0; original cards/commander_legends.c:1207. */
int card_captain_vargus_wrath(int player, int card, event_t event)
{
  int power;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }

  power = count_how_many_times_a_commander_was_played(player);
  pump_subtype_until_eot_2022(player, card, player, 0x95, power, power, 0, 0);
  return 0;
}

/* ManalinkEh 2022: 0x25a2020; original cards/commander_legends.c:2568. */
static int hellkite_courser_effect(int player, int card, event_t event)
{
  char cVar1;
  int result;
  card_instance_t *instance;

  if (event == EVENT_ABILITIES)
  {

    instance = get_card_instance(player, card);
    cVar1 = instance->damage_target_player;

    if (((-1 < cVar1) && (instance->damage_target_card == affected_card)) &&
        (cVar1 == affected_card_controller))
    {
      haste((int)cVar1, instance->damage_target_card, EVENT_ABILITIES);
    }
  }
  result = eot_trigger(player, card, event);
  if (result == 0)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_player)
  {

    put_commander_in_the_command_zone_from_play((int)instance->damage_target_player,
                                                instance->damage_target_card);
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x25b09e0; original cards/commander_legends.c:5572. */
int card_tevesh_szat_doom_of_fools(int player, int card, event_t event)
{
  uint8_t uVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  unsigned int uVar5;
  unsigned int player_00;
  int iVar6;
  target_definition_t td;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_token;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x42);
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.illegal_abilities = 0;
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->mode = '\0';
      iVar2 = target_available(player, card, &td);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "\001\033",
                          "Create two 0/1 black Thrulls", 1, 10, 2, "Sac & draw", 1,
                          (iVar2 - hand_count[player]) * 5 + 0x23, 1, "Steal all Commanders", 100,
                          10, 0xfffffff6, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_025b0a21;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_025b0a21;

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x42);
  }
  td.allowed_controller = player;
  td.illegal_abilities = 0;
  td.preferred_controller = td.allowed_controller;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    uVar1 = instance->mode;
    if (uVar1 == '\x02')
    {

      new_default_test_definition(&recovered_stack_token.test, 0x102,
                                  "Select a creature or a planeswalker to sacrifice.");
      uVar5 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_JUST_MARK,
                            &recovered_stack_token.test);
      if (uVar5 != 0)
      {

        player_00 = uVar5 >> 0x10 & 0xff;
        iVar2 = check_special_flags2_2022(player_00, uVar5 >> 0x18, 0x1000000);
        kill_card(player_00, uVar5 >> 0x18, KILL_SACRIFICE);
        draw_cards(player, (iVar2 != 0) + 2);
      }
    }
    else if (uVar1 == '\x03')
    {

      recovered_stack_token.token.id = -1;
      iVar2 = 1 - player;
      recovered_stack_token.token.s_player = -1;
      recovered_stack_token.token.s_card = -1;
      if (0 < active_cards_count[iVar2])
      {
        iVar6 = 0;
        do
        {
          instance = in_play(iVar2, iVar6);
          if (((instance != NULL) && (iVar4 = is_what(iVar2, iVar6, 0x47), iVar4 != 0)) &&
              (iVar4 = check_special_flags2_2022(iVar2, iVar6, 0x1000000), iVar4 != 0))
          {
            gain_control(player, card, iVar2, iVar6);
          }
          iVar6 = iVar6 + 1;
        } while (iVar6 < active_cards_count[iVar2]);
      }
      iVar6 = 0;
      do
      {

        iVar4 = put_all_commanders_from_command_zone_to(iVar6, 4, &recovered_stack_token.token.id);
        if ((iVar2 == iVar6) && (-1 < iVar4))
        {
          if (-1 < recovered_stack_token.token.s_player)
          {
            gain_control(player, card, iVar2, recovered_stack_token.token.s_player);
          }
          if (-1 < recovered_stack_token.token.s_card)
          {
            gain_control(player, card, iVar2, recovered_stack_token.token.s_card);
          }
        }
        iVar6 = iVar6 + 1;
      } while (iVar6 != 2);
    }
    else if (uVar1 == '\x01')
    {
      default_token_definition(player, card, 0x4996, &recovered_stack_token.token);
      recovered_stack_token.token.pow = 0;
      recovered_stack_token.token.tou = 1;
      recovered_stack_token.token.color_forced = '\x02';
      recovered_stack_token.token.qty = '\x02';
      generate_token(&recovered_stack_token.token);
    }
  }
LAB_025b0a21:
  iVar2 = planeswalker(player, card, event, 4);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int colfenor_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25a0850; original cards/commander_legends.c:1305. */
static int colfenor_effect(int player, int card, event_t event)
{
  short sVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  target_t *ptVar5;
  int iVar6;
  unsigned int uVar7;
  int iVar8;
  int iVar9;
  short sVar10;
  target_t *ptVar11;
  unsigned int id;
  int iVar12;
  short *psVar13;
  int *csvidb;
  int *piVar14;
  int *piVar15;
  int iStack_21c;
  int iStack_20c;
  int (*paiStack_208)[3];
  int16_t iids[10];
  int16_t tou[10];
  int allowed[10][3];
  int csvids[10][3];
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar2 = get_card_instance(player, card);
      if ((-1 < pcVar2->damage_target_player) &&
          (iVar3 = get_id((int)pcVar2->damage_target_player, pcVar2->damage_target_card),
           iVar3 != 0x5650))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar2 = get_card_instance(player, card);
    if ((-1 < pcVar2->damage_target_player) &&
        (iVar3 = effect_follows_control_of_attachment(player, card, 0x4700), iVar3 != 0))
    {
      return 0;
    }
  LAB_025a0887:
    iVar3 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (iVar3 == 0)
      goto joined_r0x025a0c30;
  }
  else
  {
    pcVar2 = get_card_instance(player, card);
    if ((-1 < pcVar2->damage_target_player) &&
        (iVar3 = effect_follows_control_of_attachment(player, card, event), iVar3 != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_025a0887;

    pcVar2 = get_card_instance(player, card);
    pcVar4 = in_play(affected_card_controller, affected_card);

    if ((pcVar4 != NULL) && (pcVar4 = get_card_instance(affected_card_controller, affected_card),
                             pcVar4->kill_code != '\0'))
    {

      if ((-1 < pcVar2->damage_target_player) &&
          ((pcVar2->damage_target_card == affected_card &&
            (pcVar2->damage_target_player == affected_card_controller))))
      {
        pcVar2->damage_target_card = -1;
        pcVar2->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        if (pcVar2->targets[0xb].player < 0)
        {
          pcVar2->targets[0xb].player = 0;
        }
        *(uint8_t *)((int)&pcVar2->targets[0xb].player + 1) = 1;
      }
      if ((((player == affected_card_controller) &&
            (iVar3 = is_what(player, affected_card, 2), iVar3 != 0)) &&
           (iVar3 = is_this_dying(affected_card_controller, affected_card), iVar3 != 0)) &&
          (iVar3 = check_for_death_trigger_removal(affected_card_controller, affected_card),
           iVar3 == 0))
      {

        pcVar2 = get_card_instance(player, card);
        if (pcVar2->targets[0xb].player < 0)
        {
          pcVar2->targets[0xb].player = 0;
        }
        *(char *)&pcVar2->targets[0xb].player = (char)pcVar2->targets[0xb].player + '\x01';
        iVar3 = get_toughness(affected_card_controller, affected_card);
        if ((short)iVar3 != 0)
        {

          iVar12 = get_original_internal_card_id(affected_card_controller, affected_card);

          iVar6 = 1;
          do
          {
            if (pcVar2->targets[iVar6].player == -1)
            {
              pcVar2->targets[iVar6].player = (int)(short)iVar12;
              pcVar2->targets[iVar6].card = (int)(short)iVar3;
              break;
            }
            iVar6 = iVar6 + 1;
          } while (iVar6 != 10);
        }
      }
    }
    iVar3 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (iVar3 == 0)
    {
      return 0;
    }
  }
  iVar3 = check_status(player, card, 0x10000);

  if (iVar3 == 0)
  {
    pcVar2 = get_card_instance(player, card);

    ptVar11 = pcVar2->targets + 1;
    ptVar5 = ptVar11;
    iVar3 = 0;
    do
    {
      iVar12 = iVar3;
      if (ptVar5->player != -1)
      {
        iVar12 = iVar3 + 1;
        iids[iVar3] = (int16_t)ptVar5->player;
        tou[iVar3] = (int16_t)ptVar5->card;
      }
      ptVar5 = ptVar5 + 1;
      iVar3 = iVar12;
    } while (ptVar5 != pcVar2->targets + 10);
    do
    {

      ptVar11->card = -1;
      ptVar11->player = -1;
      ptVar11 = ptVar11 + 1;
    } while (ptVar11 != pcVar2->targets + 10);
    if (0 < iVar12)
    {
      psVar13 = iids;
      piVar15 = csvids[0] + 2;
      piVar14 = allowed[0] + 2;
      csvidb = piVar15;
      do
      {
        unmeld((unsigned int)cards_data[*psVar13].id, csvidb + -1, csvidb);
        iVar3 = count_graveyard_by_id(player, csvidb[-1]);
        piVar14[-1] = iVar3 + -1;
        if (*csvidb == -1)
        {
          *piVar14 = 0;
        }
        else
        {
          iVar3 = count_graveyard_by_id(player, *csvidb);
          *piVar14 = iVar3 + -1;
        }
        psVar13 = psVar13 + 1;
        csvidb = csvidb + 3;
        piVar14 = piVar14 + 3;
      } while (psVar13 != iids + iVar12);
      paiStack_208 = allowed;

      iStack_21c = 0;
      do
      {
        sVar1 = iids[iStack_21c];
        id = (unsigned int)cards_data[sVar1].id;
        default_test_definition(&test, 2);
        test.toughness_flag = 3;
        test.id_flag = 1;
        iVar3 = (int)tou[iStack_21c];
        iStack_20c = card_counts[player][5] + 1;
        test.toughness = iVar3;
        do
        {
          if (*piVar15 == -1)
          {
            if ((((*paiStack_208)[1] == 0) && (iVar6 = is_id_in_grave(player, id), iVar6 != 0)) &&
                (iVar6 = get_base_toughness_iid(player, (int)sVar1), iVar6 < iVar3))
            {
              scnprintf(test.message, 100,
                        " Choose a creature card with toughness less than %d. (not %s)", iVar3,
                        cards_ptr[id]->name);
              test.id = id;
              goto LAB_025a0e07;
            }
            scnprintf(test.message, 100, " Choose a creature card with toughness less than %d.",
                      iVar3);
            test.id = id;
            iVar6 = new_special_count_grave(player, &test);
          }
          else
          {

            iVar6 = get_internal_card_id_from_csv_id(piVar15[-1]);
            iVar9 = get_internal_card_id_from_csv_id(*piVar15);
            if ((*paiStack_208)[1] == 0)
            {
              iVar8 = get_base_toughness_iid(player, iVar6);
              if (((iVar8 < iVar3) && (iVar8 = is_id_in_grave(player, piVar15[-1]), iVar8 != 0)) &&
                  (((*paiStack_208)[2] == 0 &&
                    ((iVar8 = get_base_toughness_iid(player, iVar9),
                      iVar8 < iVar3 && (iVar8 = is_id_in_grave(player, *piVar15), iVar8 != 0))))))
              {
                scnprintf(test.message, 100,
                          " Choose a creature card with toughness less than %d. (not %s or %s)",
                          iVar3, cards_ptr[piVar15[-1]]->name, cards_ptr[*piVar15]->name);
                test.id = piVar15[-1];
                test.id2 = *piVar15;
              }
              else
              {
                iVar6 = get_base_toughness_iid(player, iVar6);
                if ((iVar3 <= iVar6) || (iVar6 = is_id_in_grave(player, piVar15[-1]), iVar6 == 0))
                  goto LAB_025a0dba;
                scnprintf(test.message, 100,
                          " Choose a creature card with toughness less than %d. (not %s)", iVar3,
                          cards_ptr[piVar15[-1]]->name);
                test.id = piVar15[-1];
              }
            }
            else
            {
            LAB_025a0dba:
              if ((((*paiStack_208)[2] == 0) &&
                   (iVar6 = get_base_toughness_iid(player, iVar9), iVar6 < iVar3)) &&
                  (iVar6 = is_id_in_grave(player, *piVar15), iVar6 != 0))
              {
                scnprintf(test.message, 100,
                          " Choose a creature card with toughness less than %d. (not %s)", iVar3,
                          cards_ptr[*piVar15]->name);
                test.id = *piVar15;
              }
              else
              {
                scnprintf(test.message, 100, " Choose a creature card with toughness less than %d.",
                          iVar3);
              }
            }
          LAB_025a0e07:
            iVar6 = new_special_count_grave(player, &test);
          }

          if ((iVar6 != 0) &&
              (iVar6 = new_global_tutor(player, player, 2, 1, 0, 1, &test), -1 < iVar6))
          {
            uVar7 = get_id(player, iVar6);
            if (uVar7 == id)
            {
              iVar6 = iStack_21c;
              sVar10 = sVar1;
              if (iStack_21c < iVar12)
              {
                while (true)
                {
                  if (sVar1 == sVar10)
                  {
                    allowed[iVar6][1] = allowed[iVar6][1] + -1;
                  }
                  iVar6 = iVar6 + 1;
                  if (iVar6 == iVar12)
                    break;
                  sVar10 = iids[iVar6];
                }
              }
            }
            else
            {
              iVar8 = get_id(player, iVar6);
              iVar9 = piVar15[-1];
              if (iVar8 == iVar9)
              {
                if (iStack_21c < iVar12)
                {
                  iVar8 = (int)paiStack_208 - (int)allowed;
                  iVar6 = iVar9;
                  while (true)
                  {
                    if (iVar9 == iVar6)
                    {
                      *(int *)((int)allowed[0] + iVar8 + 4) =
                          *(int *)((int)allowed[0] + iVar8 + 4) + -1;
                    }
                    if (iVar12 * 0xc == iVar8 + 0xc)
                      break;
                    iVar6 = *(int *)((int)csvids[1] + iVar8 + 4);
                    iVar8 = iVar8 + 0xc;
                  }
                }
              }
              else
              {
                iVar9 = get_id(player, iVar6);
                iVar6 = *piVar15;
                if ((iVar9 == iVar6) && (iStack_21c < iVar12))
                {
                  iVar8 = (int)paiStack_208 - (int)allowed;
                  iVar9 = iVar6;
                  while (true)
                  {
                    if (iVar6 == iVar9)
                    {
                      *(int *)((int)allowed[0] + iVar8 + 8) =
                          *(int *)((int)allowed[0] + iVar8 + 8) + -1;
                    }
                    if (iVar12 * 0xc == iVar8 + 0xc)
                      break;
                    iVar9 = *(int *)((int)csvids[1] + iVar8 + 8);
                    iVar8 = iVar8 + 0xc;
                  }
                }
              }
            }
          }
          iStack_20c = iStack_20c + -1;
        } while (iStack_20c != 0);
        iStack_21c = iStack_21c + 1;
        paiStack_208 = paiStack_208 + 1;
        piVar15 = piVar15 + 3;
      } while (iStack_21c != iVar12);
    }
    kill_card(player, card, KILL_EXILE);
  }
  else
  {

    pcVar2 = get_card_instance(player, card);

    ptVar11 = pcVar2->targets + 1;
    do
    {
      ptVar11->card = -1;
      ptVar11->player = -1;
      ptVar11 = ptVar11 + 1;
    } while (ptVar11 != pcVar2->targets + 10);
  }
joined_r0x025a0c30:
  if ((event == EVENT_CLEANUP) && (iVar3 = check_status(player, card, 0x10000), iVar3 == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a5500; original cards/commander_legends.c:1450. */
int card_colfenor_the_last_yew(int player, int card, event_t event)
{
  short sVar1;
  int iVar2;
  card_instance_t *instance;
  target_t *ptVar4;
  int iVar5;
  unsigned int uVar6;
  int iVar7;
  int iVar8;
  unsigned int id;
  short sVar9;
  target_t *ptVar10;
  int iVar11;
  int *csvidb;
  int *piVar12;
  int *piVar13;
  short *psVar14;
  int iStack_21c;
  int iStack_20c;
  int (*paiStack_208)[3];
  int16_t iids[10];
  int16_t tou[10];
  int allowed[10][3];
  int csvids[10][3];
  test_definition_t test;

  check_legend_rule(player, card, event);
  vigilance_2022(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      iVar2 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          instance = in_play(player, iVar2);

          if ((((instance != NULL) && (iVar11 = is_what(player, iVar2, 0x80), iVar11 != 0)) &&
               (instance = get_card_instance(player, iVar2),
                instance->info_slot_fn == colfenor_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_025a5553;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, colfenor_effect, player, card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  }
  else if (((event == EVENT_GRAVEYARD_FROM_PLAY) && (affected_card_controller == player)) &&
           ((iVar2 = is_what(player, affected_card, 2),
             iVar2 != 0 && ((iVar2 = is_this_dying(affected_card_controller, affected_card),
                             iVar2 != 0 && (iVar2 = check_for_death_trigger_removal(
                                                affected_card_controller, affected_card),
                                            iVar2 == 0))))))
  {

    instance = get_card_instance(player, card);
    if (instance->targets[0xb].player < 0)
    {
      instance->targets[0xb].player = 0;
    }
    *(char *)&instance->targets[0xb].player = (char)instance->targets[0xb].player + '\x01';
    iVar2 = get_toughness(affected_card_controller, affected_card);
    if ((short)iVar2 != 0)
    {

      iVar5 = get_original_internal_card_id(affected_card_controller, affected_card);
      iVar11 = 0;

      do
      {
        if (instance->targets[iVar11].player == -1)
        {
          instance->targets[iVar11].player = (int)(short)iVar5;
          instance->targets[iVar11].card = (int)(short)iVar2;
          break;
        }
        iVar11 = iVar11 + 1;
      } while (iVar11 != 10);
    }
  }
LAB_025a5553:
  iVar2 = duh_mode(player);
  iVar2 = resolve_gfp_ability(player, card, event, (iVar2 != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if ((iVar2 != 0) && (instance = get_card_instance(player, card), instance->kill_code == '\0'))
  {

    instance = get_card_instance(player, card);

    ptVar10 = instance->targets;
    ptVar4 = ptVar10;
    iVar2 = 0;
    do
    {
      iVar11 = iVar2;
      if (ptVar4->player != -1)
      {
        iVar11 = iVar2 + 1;
        iids[iVar2] = (int16_t)ptVar4->player;
        tou[iVar2] = (int16_t)ptVar4->card;
      }
      ptVar4 = ptVar4 + 1;
      iVar2 = iVar11;
    } while (ptVar4 != instance->targets + 10);
    do
    {

      ptVar10->card = -1;
      ptVar10->player = -1;
      ptVar10 = ptVar10 + 1;
    } while (ptVar10 != instance->targets + 10);
    if (0 < iVar11)
    {
      psVar14 = iids;
      piVar13 = csvids[0] + 2;
      piVar12 = allowed[0] + 2;
      csvidb = piVar13;
      do
      {
        unmeld((unsigned int)cards_data[*psVar14].id, csvidb + -1, csvidb);
        iVar2 = count_graveyard_by_id(player, csvidb[-1]);
        piVar12[-1] = iVar2 + -1;
        if (*csvidb == -1)
        {
          *piVar12 = 0;
        }
        else
        {
          iVar2 = count_graveyard_by_id(player, *csvidb);
          *piVar12 = iVar2 + -1;
        }
        psVar14 = psVar14 + 1;
        csvidb = csvidb + 3;
        piVar12 = piVar12 + 3;
      } while (psVar14 != iids + iVar11);
      iStack_21c = 0;
      paiStack_208 = allowed;

      do
      {
        sVar1 = iids[iStack_21c];
        id = (unsigned int)cards_data[sVar1].id;
        default_test_definition(&test, 2);
        test.toughness_flag = 3;
        test.id_flag = 1;
        iVar2 = (int)tou[iStack_21c];
        iStack_20c = card_counts[player][5] + 1;
        test.toughness = iVar2;
      LAB_025a59cb:
        do
        {
          if (*piVar13 == -1)
          {
            if ((((*paiStack_208)[1] == 0) && (iVar5 = is_id_in_grave(player, id), iVar5 != 0)) &&
                (iVar5 = get_base_toughness_iid(player, (int)sVar1), iVar5 < iVar2))
            {
              scnprintf(test.message, 100,
                        " Choose a creature card with toughness less than %d. (not %s)", iVar2,
                        cards_ptr[id]->name);
              test.id = id;
              goto LAB_025a59af;
            }
            scnprintf(test.message, 100, " Choose a creature card with toughness less than %d.",
                      iVar2);
            test.id = id;
            iVar5 = new_special_count_grave(player, &test);
          }
          else
          {

            iVar5 = get_internal_card_id_from_csv_id(piVar13[-1]);
            iVar8 = get_internal_card_id_from_csv_id(*piVar13);
            if ((*paiStack_208)[1] == 0)
            {
              iVar7 = get_base_toughness_iid(player, iVar5);
              if ((((iVar7 < iVar2) && (iVar7 = is_id_in_grave(player, piVar13[-1]), iVar7 != 0)) &&
                   ((*paiStack_208)[2] == 0)) &&
                  ((iVar7 = get_base_toughness_iid(player, iVar8),
                    iVar7 < iVar2 && (iVar7 = is_id_in_grave(player, *piVar13), iVar7 != 0))))
              {
                scnprintf(test.message, 100,
                          " Choose a creature card with toughness less than %d. (not %s or %s)",
                          iVar2, cards_ptr[piVar13[-1]]->name, cards_ptr[*piVar13]->name);
                test.id = piVar13[-1];
                test.id2 = *piVar13;
              }
              else
              {
                iVar5 = get_base_toughness_iid(player, iVar5);
                if ((iVar2 <= iVar5) || (iVar5 = is_id_in_grave(player, piVar13[-1]), iVar5 == 0))
                  goto LAB_025a5962;
                scnprintf(test.message, 100,
                          " Choose a creature card with toughness less than %d. (not %s)", iVar2,
                          cards_ptr[piVar13[-1]]->name);
                test.id = piVar13[-1];
              }
            }
            else
            {
            LAB_025a5962:
              if ((((*paiStack_208)[2] == 0) &&
                   (iVar5 = get_base_toughness_iid(player, iVar8), iVar5 < iVar2)) &&
                  (iVar5 = is_id_in_grave(player, *piVar13), iVar5 != 0))
              {
                scnprintf(test.message, 100,
                          " Choose a creature card with toughness less than %d. (not %s)", iVar2,
                          cards_ptr[*piVar13]->name);
                test.id = *piVar13;
              }
              else
              {
                scnprintf(test.message, 100, " Choose a creature card with toughness less than %d.",
                          iVar2);
              }
            }
          LAB_025a59af:
            iVar5 = new_special_count_grave(player, &test);
          }

          if ((iVar5 != 0) &&
              (iVar5 = new_global_tutor(player, player, 2, 1, 0, 1, &test), -1 < iVar5))
          {
            uVar6 = get_id(player, iVar5);
            if (uVar6 == id)
            {
              iVar5 = iStack_21c;
              sVar9 = sVar1;
              if (iStack_21c < iVar11)
              {
                while (true)
                {
                  if (sVar1 == sVar9)
                  {
                    allowed[iVar5][1] = allowed[iVar5][1] + -1;
                  }
                  iVar5 = iVar5 + 1;
                  if (iVar5 == iVar11)
                    break;
                  sVar9 = iids[iVar5];
                }
              }
            }
            else
            {
              iVar7 = get_id(player, iVar5);
              iVar8 = piVar13[-1];
              if (iVar7 == iVar8)
              {
                if (iStack_21c < iVar11)
                {
                  iVar7 = (int)paiStack_208 - (int)allowed;
                  iVar5 = iVar8;
                  while (true)
                  {
                    if (iVar8 == iVar5)
                    {
                      *(int *)((int)allowed[0] + iVar7 + 4) =
                          *(int *)((int)allowed[0] + iVar7 + 4) + -1;
                    }
                    if (iVar11 * 0xc == iVar7 + 0xc)
                      break;
                    iVar5 = *(int *)((int)csvids[1] + iVar7 + 4);
                    iVar7 = iVar7 + 0xc;
                  }
                }
              }
              else
              {
                iVar8 = get_id(player, iVar5);
                iVar5 = *piVar13;
                if ((iVar8 == iVar5) && (iStack_21c < iVar11))
                {
                  iVar7 = (int)paiStack_208 - (int)allowed;
                  iVar8 = iVar5;
                  while (true)
                  {
                    if (iVar5 == iVar8)
                    {
                      *(int *)((int)allowed[0] + iVar7 + 8) =
                          *(int *)((int)allowed[0] + iVar7 + 8) + -1;
                    }
                    if (iVar11 * 0xc == iVar7 + 0xc)
                      break;
                    iVar8 = *(int *)((int)csvids[1] + iVar7 + 8);
                    iVar7 = iVar7 + 0xc;
                  }
                  iStack_20c = iStack_20c + -1;
                  if (iStack_20c == 0)
                    break;
                  goto LAB_025a59cb;
                }
              }
            }
          }
          iStack_20c = iStack_20c + -1;
        } while (iStack_20c != 0);
        iStack_21c = iStack_21c + 1;
        paiStack_208 = paiStack_208 + 1;
        piVar13 = piVar13 + 3;
      } while (iStack_21c != iVar11);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25acd50; original cards/commander_legends.c:4156. */
int card_nadiers_nightblade(int player, int card, event_t event)
{
  test_definition_t test;

  if ((((trigger_condition == 0xd4) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    default_test_definition(&test, 0xff);
    test.type_flag = 6;
    test_leaves_play_mode(player, card, event, player, RESOLVE_TRIGGER_MANDATORY, &test, 1, 1);
  }
  if (event != EVENT_LEAVES_PLAY_ABILITY)
  {
    return 0;
  }
  lose_life(1 - player, 1);
  gain_life_2022(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x25ace40; original cards/commander_legends.c:4177. */
int card_nadier_agent_of_the_duskenel(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  check_legend_rule(player, card, event);
  leaves_play_2022(player, card, event, 1, 1);
  if ((((trigger_condition == 0xd4) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    default_test_definition(&test, 0xff);
    test.type_flag = 6;
    test_leaves_play(player, card, event, player, &test, 1, 1);
  }
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    result = is_token(trigger_cause_controller, trigger_cause);
    if (result != 0)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    }
    if ((trigger_cause == card) && (trigger_cause_controller == player))
    {
      result = get_power(player, card);
      generate_tokens_by_id_2022(player, card, 0x492c, result);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25ad980; original cards/commander_legends.c:4406. */
int card_obeka_brute_chronologist(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    result = do_dialog(current_turn, player, card, -1, -1, " End current turn\n No thanks",
                       (unsigned int)(player != 1 || current_turn != player));
    if (result == 0)
    {
      end_the_turn(player, card, EVENT_RESOLVE_ACTIVATION);
    }
  }
  result = generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a3dd0; original cards/commander_legends.c:675. */
int card_ardenn_intrepid_archaeologist(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  target_definition_t td;

  check_legend_rule(player, card, event);
  iVar1 = beginning_of_combat_2022(player, card, event, player);
  if (iVar1 != 0)
  {

    pcVar2 = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x147);
    td.zone = 0x1200;
    td.preferred_controller = player;
    iVar1 = new_pick_target(&td, "Select target permanent or player.", 0, 0x80);
    if (iVar1 != 0)
    {

      iVar1 = pcVar2->targets[0].player;
      pcVar2->number_of_targets = '\0';
      iVar5 = pcVar2->targets[0].card;
      if (iVar5 == -1)
      {
        default_target_definition(player, card, &td, 0x47);
        td.preferred_controller = player;
        td.allowed_controller = player;
        td.illegal_abilities = 0;
        td.special = 0x8000;
        td.extra = (int32_t)aura_enchanting_player;
        while (true)
        {
          iVar5 = can_target(&td);
          if (iVar5 == 0)
            break;
          iVar5 = new_pick_target(&td, "Select an Aura you control enchanting a player.", 0, 0x80);
          if (iVar5 == 0)
            break;

          state_untargettable(pcVar2->targets[0].player, pcVar2->targets[0].card, 1);
          pcVar4 = get_card_instance(pcVar2->targets[0].player, pcVar2->targets[0].card);
          pcVar4->targets[0].player = iVar1;
          pcVar4->targets[4].player = iVar1;
          pcVar4->damage_target_player = (int8_t)iVar1;
          pcVar4->targets[0].card = -1;
          pcVar4->targets[4].card = -1;
          pcVar4->damage_target_card = -1;
          pcVar2->number_of_targets = '\0';
        }
      }
      else
      {
        default_target_definition(player, card, &td, 0x47);
        td.preferred_controller = player;
        td.allowed_controller = player;
        td.illegal_abilities = 0;
        td.special = 0x8000;
        td.extra = (int32_t)aura_or_equipment;
        td.allow_cancel = 2;
        while (true)
        {
          iVar3 = can_target(&td);
          if (iVar3 == 0)
            break;
          iVar3 = new_pick_target(&td, "Select an Aura or Equipment", 0, 0x80);
          if (iVar3 == 0)
            break;
          state_untargettable(pcVar2->targets[0].player, pcVar2->targets[0].card, 1);
          iVar3 =
              has_subtype(pcVar2->targets[0].player, pcVar2->targets[0].card, SUBTYPE_EQUIPMENT);
          if (iVar3 == 0)
          {

            set_special_flags_2022(pcVar2->targets[0].player, pcVar2->targets[0].card, 0x20800000);
            pcVar4 = get_card_instance(pcVar2->targets[0].player, pcVar2->targets[0].card);
            pcVar4->targets[0].player = iVar1;
            pcVar4->targets[0].card = iVar5;
            iVar3 = call_card_function(pcVar2->targets[0].player, pcVar2->targets[0].card,
                                       EVENT_MOVE_AURA);
            if (iVar3 != 0)
            {
              call_card_function(pcVar2->targets[0].player, pcVar2->targets[0].card,
                                 EVENT_RESOLVE_MOVING_AURA);
            }
            remove_special_flags_2022(pcVar2->targets[0].player, pcVar2->targets[0].card, 0x20800000);
          }
          else
          {
            iVar3 = is_what(iVar1, iVar5, 2);
            if (iVar3 != 0)
            {
              equip_target_creature(pcVar2->targets[0].player, pcVar2->targets[0].card, iVar1,
                                    iVar5);
            }
          }
          pcVar2->number_of_targets = '\0';
        }
      }
      remove_untargettable_from_all();
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a7800; original cards/commander_legends.c:2186. */
int card_forceful_denial(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = 0;
    counterspell(player, card, EVENT_CAST_SPELL, NULL, 0);
    if (spell_fizzled != 1)
    {
      cascade_2022(player, card, EVENT_CAST_SPELL);
      counterspell(player, card, EVENT_CAST_SPELL, NULL, 0);
    }
  }
  else
  {
    result = counterspell(player, card, event, NULL, 0);
  }
  return result;
}

/* ManalinkEh 2022: 0x25a8da0; original cards/commander_legends.c:2787. */
int card_imoti_celebrant_of_bounty(int player, int card, event_t event)
{
  int result;

  cascade_2022(player, card, event);
  check_legend_rule(player, card, event);
  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      result = get_cmc(trigger_cause_controller, trigger_cause);
      if (5 < result)
      {
        result = check_special_flags_2022(trigger_cause_controller, trigger_cause, 0x40000);
        if (result == 0)
        {
          result = new_specific_spell_played(player, card, event, player, 2, NULL);
          if (result != 0)
          {
            result = get_cmc(trigger_cause_controller, trigger_cause);
            do_cascade(player, card, result);
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a8f40; original cards/commander_legends.c:2822. */
int card_ingenuity_engine(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  cascade_2022(player, card, event);
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    td.preferred_controller = player;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x40);
    td.preferred_controller = player;
    td.allowed_controller = player;
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      action_on_target(player, card, 0, ACT_BOUNCE);
    }
  }
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_CAN_TARGET | GAA_UNTAPPED, 1, 0, 0, 0, 0, 0, 0, &td,
      "TARGET_ARTIFACT", TYPE_ARTIFACT, NULL, SARV_REPORT_CMC);
  return result;
}

/* ManalinkEh 2022: 0x25ac870; original cards/commander_legends.c:3984. */
int card_meteoric_mace(int player, int card, event_t event)
{
  int result;

  cascade_2022(player, card, event);
  result = vanilla_equipment(player, card, event, 4, 4, 0, 0x80, 0);
  return result;
}

/* ManalinkEh 2022: 0x25acfa0; original cards/commander_legends.c:4205. */
int card_natural_reclamation(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_target_definition(player, card, &td, 0x44);
    generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "DISENCHANT", 1, NULL);
    if (spell_fizzled == 1)
    {
      result = 0;
    }
    else
    {
      cascade_2022(player, card, EVENT_CAST_SPELL);
      result = 0;
    }
  }
  else
  {
    result = card_disenchant(player, card, event);
  }
  return result;
}

/* ManalinkEh 2022: 0x25ad5c0; original cards/commander_legends.c:4331. */
int card_numa_joraga_chieftain(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  card_instance_t *pcVar4;
  card_instance_t *pcVar5;
  int *piVar6;
  int iStack_590;
  int tc[2];
  target_definition_t td;
  char buffer[100];
  int targs[2][151];

  check_legend_rule(player, card, event);
  iVar1 = beginning_of_combat_2022(player, card, event, player);

  if ((iVar1 != 0) && (iVar1 = charge_mana_for_double_x(player, 0), iVar1 != 0))
  {

    iVar2 = iVar1 / 2;
    default_target_definition(player, card, &td, 0x47);
    td.preferred_controller = player;
    td.required_subtype = 0x3c;
    if (player == 1)
    {
      td.required_type = 2;
    }
    td.allow_cancel = 0;
    tc[0] = 0;
    tc[1] = 0;
    if ((iVar2 != 0) && (iVar3 = can_target(&td), iVar3 != 0))
    {

      pcVar4 = get_card_instance(player, card);
      if (1 < iVar1)
      {
        iVar1 = 0;
        do
        {
          iVar1 = iVar1 + 1;
          scnprintf(buffer, 100, "Select target Elf which will get a +1/+1 counter (%d of %d)",
                    iVar1, iVar2);
          iVar3 = new_pick_target(&td, buffer, 0, 0x80);
          if (iVar3 == 0)
            break;
          pcVar5 = get_card_instance(pcVar4->targets[0].player, pcVar4->targets[0].card);
          pcVar5->state = pcVar5->state | 0x200000;
          iVar3 = pcVar4->targets[0].player;
          pcVar4->number_of_targets = '\0';
          targs[iVar3][tc[iVar3]] = pcVar4->targets[0].card;
          tc[iVar3] = tc[iVar3] + 1;
        } while (iVar1 < iVar2);
      }
      iStack_590 = 0;
      iVar1 = 0;
      do
      {
        iVar2 = tc[iVar1];
        if (0 < iVar2)
        {
          piVar6 = targs[0] + iStack_590;
          do
          {
            iVar3 = *piVar6;
            piVar6 = piVar6 + 1;
            pcVar4 = get_card_instance(iVar1, iVar3);
            pcVar4->state = pcVar4->state & 0xffdfffff;
            add_counters_from_source(player, card, iVar1, iVar3, COUNTER_P1_P1, 1);
          } while (piVar6 != targs[0] + iVar2 + iStack_590);
        }
        iVar1 = iVar1 + 1;
        iStack_590 = iStack_590 + 0x97;
      } while (iVar1 != 2);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25b06b0; original cards/commander_legends.c:5495. */
int card_sweet_gum_recluse(int player, int card, event_t event)
{
  unsigned char bVar1;
  int result;
  card_instance_t *instance;
  target_definition_t td;

  cascade_2022(player, card, event);
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.special = 0x8000;
    td.extra = (int32_t)is_entered_the_battlefield_this_turn;
    instance = get_card_instance(player, card);
    bVar1 = instance->number_of_targets;
    if (bVar1 < 10)
    {
      result = can_target(&td);
      while ((result != 0 &&
              (result = new_pick_target(
                   &td, "Select a creature that entered the battlefield this turn.", -1, 0x80),
               result != 0)))
      {

        state_untargettable(instance->targets[instance->number_of_targets - 1].player,
                            instance->targets[instance->number_of_targets - 1].card, 1);
        if (9 < instance->number_of_targets)
          goto LAB_025b07cf;
        result = can_target(&td);
      }
      bVar1 = instance->number_of_targets;
    }
    if (bVar1 != 0)
    {
    LAB_025b07cf:
      result = 0;
      do
      {
        state_untargettable(instance->targets[result].player, instance->targets[result].card, 0);
        add_counters_from_source(player, card, instance->targets[result].player,
                                 instance->targets[result].card, COUNTER_P1_P1, 3);
        result = result + 1;
      } while (result < (int)(unsigned int)instance->number_of_targets);
    }
    instance->number_of_targets = '\0';
  }
  result = flash(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25b15e0; original cards/commander_legends.c:5894. */
int card_volcanic_torrent(int player, int card, event_t event)
{
  int result;

  cascade_2022(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {

    result = get_specific_storm_count(player);
    new_damage_all(player, card, 1 - player, result, 0x21, NULL);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int is_aura_or_equipment_by_iid(int iid, int unused, int player, int card);

/* ManalinkEh 2022: 0x25a0670; original cards/commander_legends.c:787. */
static int is_aura_or_equipment_by_iid(int iid, int unused, int player, int card)
{
  int result;

  result = is_what(-1, iid, 0x40);
  if (result != 0)
  {
    result = has_subtype_by_id_2022(player, (unsigned int)cards_data[iid].id, SUBTYPE_EQUIPMENT);
    if (result != 0)
    {
      return 1;
    }
  }
  result = is_what(-1, iid, 4);
  if (result == 0)
  {
    return 0;
  }
  result = has_subtype_by_id_2022(player, (unsigned int)cards_data[iid].id, SUBTYPE_AURA);
  return (unsigned int)(result != 0);
}

/* ManalinkEh 2022: 0x25a42c0; original cards/commander_legends.c:797. */
int card_armored_skyhunter(int player, int card, event_t event)
{
  card_instance_t *instance;
  int card_00;
  int result;
  target_definition_t td;
  test_definition_t test;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  new_default_test_definition(&test, 0x47, "Select an Aura or Equipment card.");
  test.special_selection_function = is_aura_or_equipment_by_iid;
  card_00 = reveal_top_cards_of_library_and_choose(player, player, player, 6, 0, TUTOR_PLAY, 1,
                                                   TUTOR_BOTTOM_OF_DECK_RANDOM, 0, &test);
  if (-1 < card_00)
  {
    result = has_subtype(player, card_00, SUBTYPE_EQUIPMENT);
    if (result != 0)
    {
      default_target_definition(player, card_00, &td, 2);
      td.preferred_controller = player;
      td.allowed_controller = player;
      result = can_target(&td);
      if (result != 0)
      {
        result = new_pick_target(&td, "Select target creature to attach the Equipment.", 0, 0x80);
        if (result != 0)
        {
          equip_target_creature(player, card_00, instance->targets[0].player,
                                instance->targets[0].card);
        }
      }
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a32b0; original cards/commander_legends.c:352. */
int card_amareth_the_lustrous(int player, int card, event_t event)
{
  enum
  {
    TUTOR_0 = 0
  };

  int *piVar1;
  int iid;
  type_t tVar2;
  int card_00;
  type_t tStack_f0;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((trigger_condition == 0xdb) &&
      ((trigger_cause_controller != player || (trigger_cause != card))))
  {
    etb_ability_global(player, event, 2, TYPE_PERMANENT, NULL, player, card);
  }
  if (event == EVENT_ETB)
  {

    piVar1 = deck_ptr[player];
    tVar2 = get_type(trigger_cause_controller, trigger_cause);
    iid = *piVar1;
    if (iid != -1)
    {

      card_00 = add_card_to_hand(player, iid);
      if (card_00 == -1)
      {
        tStack_f0 = get_type_with_iid(-1, -1, iid);
      }
      else
      {
        tStack_f0 = get_type_with_iid(player, card_00, iid);
        obliterate_card(player, card_00);
      }
      new_default_test_definition(&test, 0xff, "Select a card to add to your hand");
      test.qty = (unsigned short)((tStack_f0 & tVar2) != TYPE_NONE);
      reveal_top_cards_of_library_and_choose(player, card, player, 1, 0, TUTOR_HAND, 1, TUTOR_0, 0,
                                             &test);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a20f0; original cards/commander_legends.c:3737. */
static int card_exiled_with_aegis_counter(int player, int card, event_t event)
{
  enum
  {
    COUNTER_AEGIS = 173
  };

  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int card_00;
  int iStack_14;

  if (event == EVENT_SET_LEGACY_EFFECT_NAME)
  {
    instance = get_card_instance(player, card);
    if (-1 < instance->targets[0].card)
    {

      instance = get_card_instance(player, card);
      scnprintf(set_legacy_effect_name_addr, 0x33, "%s",
                cards_ptr[cards_data[instance->targets[0].card].id]->name);
    }
  }
  else
  {
    if (event == EVENT_SET_LEGACY_EFFECT_TEXT)
    {
      scnprintf(set_legacy_effect_name_addr, 0x33, "Exiled with an aegis counter.");
      return 0;
    }
    if (event == EVENT_STATIC_EFFECTS)
    {

      instance = get_card_instance(player, card);
      iStack_14 = 0;
      iVar3 = instance->targets[0].card;
      if (0 < active_cards_count[player])
      {
        card_00 = 0;
        do
        {
          instance = in_play(player, card_00);
          if (instance != NULL)
          {
            iVar2 = is_what(player, card_00, 0x80);
            if (iVar2 != 0)
            {

              instance = get_card_instance(player, card_00);
              if (instance->info_slot_fn == card_exiled_with_aegis_counter)
              {
                iVar2 = count_counters(player, card_00, COUNTER_AEGIS);
                if ((iVar2 != 0) && (iVar3 == instance->targets[0].card))
                {
                  iStack_14 = iStack_14 + 1;
                }
              }
            }
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[player]);
      }
      iVar3 = count_iid_in_rfg(player, iVar3);
      if (iVar3 < iStack_14)
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25afcb0; original cards/commander_legends.c:5172. */
int card_seraphic_greatsword(int player, int card, event_t event)
{
  enum
  {
    TOKEN_ACTION_SKIP_ATTACK_CHECK = 2097152
  };

  card_instance_t *instance;
  int result;
  token_generation_t token;

  if (((unsigned int)player < 3) && ((unsigned int)card < 0x97))
  {
    instance = is_equipping(player, card);
    if (instance != NULL)
    {
      store_attackers(player, card, event, DAT_ATTACKS_PLAYER | DAT_RESOLVE_TRIGGER_MANDATORY,
                      (int)instance->damage_target_player, instance->damage_target_card, NULL);
      dat_ability(player, card, event, DAT_DONT_REPORT_VALUE, 1);
      if ((event == EVENT_DAT_ABILITY) && (life[current_turn] <= life[1 - current_turn]))
      {
        default_token_definition(player, card, 0x48ff, &token);
        token.action = TOKEN_ACTION_SKIP_ATTACK_CHECK | TOKEN_ACTION_ATTACKING;
        generate_token(&token);
      }
    }
    result = vanilla_equipment(player, card, event, 4, 2, 2, 0, 0);
    return result;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a3670; original cards/commander_legends.c:445. */
int card_anointer_of_valor(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int iVar4;

  store_attackers(player, card, event, DAT_TRACK, player, -1, NULL);
  iVar1 = duh_mode(player);
  dat_ability(player, card, event, (iVar1 != 0) + DAT_RESOLVE_TRIGGER_OPTIONAL, 1);
  if (event == EVENT_DAT_ABILITY)
  {

    pcVar2 = get_card_instance(player, card);
    iVar1 = pcVar2->info_slot + -1;
    if (-1 < iVar1)
    {
      do
      {
        iVar4 = has_mana(player, 0, 3);
        if (iVar4 == 0)
        {
          return 0;
        }
        pcVar3 = in_play(current_turn,
                         (unsigned int)*(unsigned char *)((int)&pcVar2->targets[2].player + iVar1));
        if (pcVar3 != NULL)
        {
          add_status(current_turn,
                     (unsigned int)*(unsigned char *)((int)&pcVar2->targets[2].player + iVar1),
                     0x20000);
          charge_mana(player, COLOR_COLORLESS, 3);
          remove_status(current_turn,
                        (unsigned int)*(unsigned char *)((int)&pcVar2->targets[2].player + iVar1),
                        0x20000);
          if (spell_fizzled != 1)
          {
            add_counters_from_source(
                player, card, current_turn,
                (unsigned int)*(unsigned char *)((int)&pcVar2->targets[2].player + iVar1),
                COUNTER_P1_P1, 1);
          }
        }
        iVar1 = iVar1 + -1;
      } while (iVar1 != -1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25aad60; original cards/commander_legends.c:3452. */
int card_keleth_sunmane_familiar(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (event == EVENT_DECLARE_ATTACKERS)
  {
    default_test_definition(&test, 2);
    test.special_selection_function = commander_test;
    store_attackers(player, card, EVENT_DECLARE_ATTACKERS, DAT_TRACK, player, -1, &test);
    dat_ability(player, card, EVENT_DECLARE_ATTACKERS, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
    return 0;
  }
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  if (event == EVENT_DAT_ABILITY)
  {

    pcVar1 = get_card_instance(player, card);
    result = pcVar1->info_slot + -1;
    if (-1 < result)
    {
      do
      {
        pcVar2 =
            in_play(current_turn,
                    (unsigned int)*(unsigned char *)((int)&pcVar1->targets[2].player + result));
        if (pcVar2 != NULL)
        {
          add_counters_from_source(
              player, card, current_turn,
              (unsigned int)*(unsigned char *)((int)&pcVar1->targets[2].player + result),
              COUNTER_P1_P1, 1);
        }
        result = result + -1;
      } while (result != -1);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a2ed0; original cards/commander_legends.c:262. */
int card_alharu_solemn_ritualist(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  bool bVar4;
  int tgs[3];

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    token_generation_t token;
    uint8_t bytes[84];
  } recovered_stack_td1;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    state_untargettable(player, card, 1);
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &recovered_stack_td1.target, 2);
    tgs[0] = card;
    recovered_stack_td1.target.preferred_controller = player;
    recovered_stack_td1.target.allowed_controller = player;
    tgs[1] = -1;
    tgs[2] = -1;
    iVar3 = 1;
    do
    {
      iVar2 = can_target(&recovered_stack_td1.target);
      if (iVar2 != 0)
      {
        iVar2 = new_pick_target(&recovered_stack_td1.target, "TARGET_CREATURE_YOU_CONTROL", 0, 0);
        if (iVar2 == 0)
          break;
        iVar2 = instance->targets[0].card;
        tgs[iVar3] = iVar2;
        state_untargettable(player, iVar2, 1);
        instance->number_of_targets = '\0';
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 != 3);
    iVar3 = 0;
  LAB_025a30d7:
    do
    {
      card_00 = tgs[iVar3];
      iVar2 = iVar3 + 1;
      if (card_00 != -1)
      {
        state_untargettable(player, card_00, 0);
        bVar4 = iVar3 == 0;
        iVar3 = iVar2;
        if (bVar4)
          goto LAB_025a30d7;
        add_counters_from_source(player, card, player, card_00, COUNTER_P1_P1, 1);
      }
      iVar3 = iVar2;
    } while (iVar2 < 3);
  }
  else if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar3 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          instance = in_play(player, iVar3);
          if ((((instance != NULL) && (iVar2 = is_what(player, iVar3, 0x80), iVar2 != 0)) &&
               (instance = get_card_instance(player, iVar3),
                instance->info_slot_fn == alhard_solemn_ritualist_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_025a2f30;
          iVar3 = iVar3 + 1;
        } while (iVar3 < active_cards_count[player]);
      }
      iVar3 = create_targetted_legacy_effect(player, card, alhard_solemn_ritualist_effect, player,
                                             card);
      instance = get_card_instance(player, iVar3);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar3, 0x10000);
    }
  }
  else if (((event == EVENT_GRAVEYARD_FROM_PLAY) &&
            (instance = in_play(affected_card_controller, affected_card), instance != NULL)) &&
           ((iVar3 = is_this_dying(affected_card_controller, affected_card),
             iVar3 != 0 &&
                 (iVar3 = count_counters(affected_card_controller, affected_card, COUNTER_P1_P1),
                  iVar3 != 0))))
  {
    count_for_gfp_ability_and_store_values(player, card, 0x77, player, 2, NULL, 0x40, 0);
  }
LAB_025a2f30:
  instance = get_card_instance(player, card);
  if (instance->kill_code == '\0')
  {
    iVar3 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    default_token_definition(player, card, 0x498d, &recovered_stack_td1.token);
    recovered_stack_td1.token.qty = (uint8_t)iVar3;
    recovered_stack_td1.target.required_abilities = 1;
    recovered_stack_td1.target.illegal_abilities = 1;
    recovered_stack_td1.token.color_forced = 0x20;
    recovered_stack_td1.target.required_color = 0x20;
    generate_token(&recovered_stack_td1.token);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25af4b0; original cards/commander_legends.c:5029. */
int card_sakashimas_protege(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  int t_player;
  target_definition_t td;

  cascade_2022(player, card, event);
  if (event == EVENT_RESOLVE_SPELL)
  {
    base_target_definition(player, card, &td, 0x47);
    td.preferred_controller = player;
    td.special = 0x8000;
    td.extra = (int32_t)is_entered_the_battlefield_this_turn;
    instance = get_card_instance(player, card);
    instance->state = instance->state | STATE_OUBLIETTED;
    result = new_pick_target(&td, "Select any permanent that entered the battlefield this turn.", 0,
                             0x80);
    instance->state = instance->state & 0xff7fffff;
    if (result == 0)
    {
      spell_fizzled = 0;
      result = -1;
      t_player = -1;
      goto LAB_025af4dd;
    }
    result = instance->targets[0].card;
    instance->number_of_targets = '\0';
    t_player = instance->targets[0].player;
    if ((result != card) || (player != t_player))
      goto LAB_025af4dd;
  }
  result = -1;
  t_player = -1;
LAB_025af4dd:
  enters_the_battlefield_as_copy_of(player, card, event, t_player, result);
  result = flash(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a49c0; original cards/commander_legends.c:984. */
int card_blazing_sunsteel(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int iVar5;
  int dam[10];
  target_definition_t td;
  char prompt[100];

  pcVar1 = is_equipping(player, card);
  if ((pcVar1 != NULL) && (iVar2 = is_humiliated_for_ability(player, card, AL_TRIG), iVar2 == 0))
  {

    pcVar1 = get_card_instance(player, card);
    pcVar3 = damage_being_dealt(event);
    if ((pcVar3 != NULL) && (((pcVar3->damage_target_card == pcVar1->damage_target_card &&
                               (pcVar3->damage_target_player == pcVar1->damage_target_player)) &&
                              (iVar2 = pcVar1->info_slot, iVar2 < 8))))
    {
      iVar4 = iVar2 + 1;
      if (iVar2 < 1)
      {
        iVar4 = 2;
        iVar2 = 1;
      }
      pcVar1->info_slot = iVar4;
      pcVar1->targets[iVar2].player = pcVar3->info_slot;
    }
    if (((trigger_condition == 0xd7) && (card == affected_card)) &&
        ((player == affected_card_controller &&
          ((reason_for_trigger_controller == player && (1 < pcVar1->info_slot))))))
    {
      if (event == EVENT_TRIGGER)
      {
        event_result = event_result | 2;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {

        default_target_definition(player, card, &td, 0x102);
        td.illegal_abilities =
            get_protections_from((int)pcVar1->damage_target_player, pcVar1->damage_target_card);
        prompt[0] = '\0';
        iVar2 = pcVar1->info_slot;
        td.zone = 0x1200;
        td.allow_cancel = 0;
        if (iVar2 < 2)
        {
          pcVar1->info_slot = 1;
        }
        else
        {
          iVar4 = 1;
          do
          {
            dam[iVar4] = pcVar1->targets[iVar4].player;
            iVar4 = iVar4 + 1;
          } while (iVar4 != iVar2);
          pcVar1->info_slot = 1;
          iVar4 = 1;
          do
          {
            if (ai_is_speculating != 1)
            {
              load_text_2022(NULL, "TARGET_ANY");
              scnprintf(prompt, 100, "%s (%d damage)", text_lines[0], dam[iVar4]);
            }
            pcVar1->number_of_targets = '\0';
            iVar5 = can_target(&td);
            if ((iVar5 != 0) && (iVar5 = pick_next_target_noload(&td, prompt), iVar5 != 0))
            {
              damage_creature(pcVar1->targets[0].player, pcVar1->targets[0].card, dam[iVar4],
                              (int)pcVar1->damage_target_player, pcVar1->damage_target_card);
            }
            iVar4 = iVar4 + 1;
          } while (iVar4 != iVar2);
        }
      }
    }
  }
  iVar2 = vanilla_equipment(player, card, event, 4, 1, 0, 0, 0);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25ab7c0; original cards/commander_legends.c:3664. */
int card_laboratory_drudge(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  inst_info_t iVar3;

  if (((event == EVENT_CAST_SPELL) &&
       (((card != affected_card || (player != affected_card_controller)) &&
         (instance = in_play(player, card), instance != NULL)))) &&
      (iVar1 = check_special_flags_2022(affected_card_controller, affected_card, 0x10000000),
       iVar1 != 0))
  {
    instance = get_card_instance(player, card);
    set_inst_info(instance, 0x568e, 0, 0x42);
  }
  iVar1 = eot_trigger(player, card, event);
  if (iVar1 != 0)
  {
    instance = get_card_instance(player, card);
    iVar3 = inst_info(instance, 0x568e);
    if (iVar3.data2 == 0x42)
    {
      draw_a_card(player);
      instance = get_card_instance(player, card);
      clear_info(instance, 0x568e);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a45b0; original cards/commander_legends.c:877. */
int card_bell_borca_spectral_sergeant(int player, int card, event_t event)
{
  unsigned short uVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int newval;
  inst_info_t iVar4;

  check_legend_rule(player, card, event);
  if (event == EVENT_CARD_TO_EXILE)
  {

    iVar3 = get_cmc_by_internal_id(affected_card);
    instance = get_card_instance(player, card);
    iVar4 = inst_info(instance, 0x5642);
    if ((int)(((uint32_t)iVar4.id | ((uint32_t)iVar4.data2 << 16)) >> 0x10) < iVar3)
    {
      set_inst_info(instance, 0x5642, 0, (uint16_t)iVar3);
    }
    upkeep_trigger_ability(player, card, EVENT_CARD_TO_EXILE, player);
    return 0;
  }
  if (event != EVENT_POWER)
  {
    upkeep_trigger_ability(player, card, event, player);
    if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
    {
      if (event == EVENT_CLEANUP)
      {

        instance = get_card_instance(player, card);
        clear_info(instance, 0x5642);
      }
      return 0;
    }

    uVar1 = cards_data[*deck_ptr[player]].id;
    rfg_top_card_of_deck(player);
    create_may_play_card_from_exile_effect(player, card, player, (unsigned int)uVar1,
                                           MPCFE_UNTIL_EOT);
    return 0;
  }
  if ((card == affected_card) && (player == affected_card_controller))
  {

    if ((player < 0) || (card < 0))
    {
      newval = 0;
    }
    else
    {

      instance = get_card_instance(player, card);
      iVar4 = inst_info(instance, 0x5642);
      newval = ((uint32_t)iVar4.id | ((uint32_t)iVar4.data2 << 16)) >> 0x10;
    }
    set_base_pt(newval);
  }
  upkeep_trigger_ability(player, card, EVENT_POWER, player);
  return 0;
}

/* ManalinkEh 2022: 0x25a94b0; original cards/commander_legends.c:2947. */
int card_jeska_thrice_reborn(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  unsigned int amt;
  int iVar5;
  inst_info_t iVar6;
  int player_is_tagetted[2];
  target_definition_t td;
  target_definition_t td3;
  char buffer[100];

  check_legend_rule(player, card, event);
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar3 = count_how_many_times_a_commander_was_played(player);
      add_counters_from_source(player, card, player, card, COUNTER_LOYALTY, iVar3);
    }
  }
  else if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    default_target_definition(player, card, &td3, 0x102);
    td3.zone = 0x1200;
    iVar3 = can_target(&td3);
    iVar4 = can_target(&td);
    iVar3 = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                        "Target creature deals triple damage until EOT", iVar4, 10, 0,
                        "X damage to up to 3 targets", iVar3, 5, 0, 0, 0, 0, 0);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (iVar3 == 0)
      {
        return 0;
      }
    }
    else
    {

      instance->targets[0].card = -1;
      instance->targets[0].player = -1;
      instance->targets[1].card = -1;
      instance->targets[1].player = -1;
      instance->targets[2].card = -1;
      instance->targets[2].player = -1;
      instance->targets[3].card = -1;
      instance->targets[3].player = -1;
      instance->number_of_targets = '\0';
      if (iVar3 == 1)
      {
        new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      }
      else if (iVar3 == 2)
      {

        player_is_tagetted[0] = 0;
        player_is_tagetted[1] = 0;
        iVar3 = 0;
        do
        {
          iVar5 = iVar3 + 1;
          scnprintf(buffer, 100, "Select target %d", iVar5);
          iVar4 = new_pick_target(&td3, buffer, iVar3, 0x80);
          if (iVar4 != 0)
          {
            iVar4 = instance->targets[iVar3].card;
            iVar3 = instance->targets[iVar3].player;
            if (iVar4 == -1)
            {
              if (player_is_tagetted[iVar3] != 0)
              {
                spell_fizzled = 1;
                break;
              }
              player_is_tagetted[iVar3] = 1;
            }
            else
            {
              state_untargettable(iVar3, iVar4, 1);
            }
          }
          iVar3 = iVar5;
        } while (iVar5 != 3);
        iVar3 = 0;
        if (instance->number_of_targets != '\0')
        {
          do
          {
            iVar4 = instance->targets[iVar3].card;
            if (-1 < iVar4)
            {
              state_untargettable(instance->targets[iVar3].player, iVar4, 0);
            }
            iVar3 = iVar3 + 1;
          } while (iVar3 < (int)(unsigned int)instance->number_of_targets);
        }
        iVar3 = count_counters(player, card, COUNTER_LOYALTY);
        if (player == 0)
        {
          iVar4 = choose_a_number(0, "How many Loyalty counters you will remove?", iVar3);
          bVar1 = iVar3 < iVar4;
        }
        else
        {
          iVar4 = iVar3 + -1;
          bVar1 = false;
        }
        if ((iVar4 < 0) || (bVar1))
        {
          spell_fizzled = 1;
          return 0;
        }
        remove_counters_from_source(player, card, player, card, COUNTER_LOYALTY, iVar4);
        set_inst_info(instance, 0x567e, 0, (uint16_t)iVar4);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    default_target_definition(player, card, &td3, 0x102);
    td3.zone = 0x1200;
    iVar3 = can_target(&td3);
    iVar4 = can_target(&td);
    iVar3 = dialog_impl(player, card, event, "\001\002", "\001\017", "\001\033",
                        "Target creature deals triple damage until EOT", iVar4, 10, 0,
                        "X damage to up to 3 targets", iVar3, 5, 0, 0, 0, 0, 0);
    if (iVar3 == 1)
    {
      iVar3 = validate_target(player, card, &td, 0);
      if (iVar3 != 0)
      {
        create_targetted_legacy_effect(player, card,
                                       attached_creature_will_deal_triple_damage_until_eot,
                                       instance->targets[0].player, instance->targets[0].card);
      }
    }
    else if (iVar3 == 2)
    {

      iVar6 = inst_info(instance, 0x567e);
      amt = ((uint32_t)iVar6.id | ((uint32_t)iVar6.data2 << 16)) >> 0x10;
      if ((amt != 0) && (instance->number_of_targets != '\0'))
      {
        iVar3 = 0;
        do
        {
          iVar4 = validate_target(player, card, &td3, iVar3);
          if (iVar4 != 0)
          {
            damage_creature(instance->targets[iVar3].player, instance->targets[iVar3].card, amt,
                            player, card);
          }
          iVar3 = iVar3 + 1;
        } while (iVar3 < (int)(unsigned int)instance->number_of_targets);
      }
    }
  }
  iVar3 = planeswalker(player, card, event, 0);
  return iVar3;
}

/* ManalinkEh 2022: 0x25b0ee0; original cards/commander_legends.c:5697. */
int card_the_prismatic_piper(int player, int card, event_t event)
{
  card_instance_t *inst;
  unsigned int uVar1;
  int player_00;
  int card_00;
  card_instance_t *pcVar2;
  inst_info_t result;

  check_legend_rule(player, card, event);
  if (event != EVENT_SET_COLOR)
  {
    return 0;
  }
  if (card != affected_card)
  {
    return 0;
  }
  if (player != affected_card_controller)
  {
    return 0;
  }

  inst = get_card_instance(player, card);
  result = inst_info(inst, 0x56c6);
  uVar1 = ((uint32_t)result.id | ((uint32_t)result.data2 << 16)) >> 0x10;
  if (uVar1 == 0)
  {

    player_00 = get_owner(player, card);
    card_00 = locate_edh_card_2022(player_00);
    if (card_00 == -1)
    {
      set_inst_info(inst, 0x56c6, 0, 0x800);
      uVar1 = 0;
      goto LAB_025b0f50;
    }
    pcVar2 = get_card_instance(player_00, card_00);
    uVar1 = pcVar2->targets[6].card;
    set_inst_info(inst, 0x56c6, 0, (uint16_t)uVar1);
  }
  uVar1 = uVar1 & 0x3e;
LAB_025b0f50:
  event_result = event_result | uVar1;
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a8200; original cards/commander_legends.c:2390. */
int card_gor_muldrak_amphinologist(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;
  int counts[2];
  token_generation_t token;

  check_legend_rule(player, card, event);
  if ((((event == EVENT_STATIC_EFFECTS || event == EVENT_ABILITIES) ||
        (event == EVENT_PROT_MISC)) &&
       (instance = in_play(player, card), instance != NULL)) &&
      (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
  {
    if (event == EVENT_STATIC_EFFECTS)
    {
      *(unsigned int *)(&player_bits + player * 4) =
          *(unsigned int *)(&player_bits + player * 4) | 0x4000000;
    }
    else if (event == EVENT_PROT_MISC)
    {
      if (affected_card == -1)
      {
        if (((unsigned int)event_result >> 24 & 0xff) == '\x06')
        {
          event_result = event_result | 1;
        }
        result = has_subtype(attacking_card_controller, attacking_card, SUBTYPE_SALAMANDER);
        if (result != 0)
        {
          event_result = event_result | 1;
        }
      }
      if (affected_card_controller == player)
      {
        protection_from_subtype(player, affected_card, EVENT_PROT_MISC, SUBTYPE_SALAMANDER);
      }
    }
    else if ((event == EVENT_ABILITIES) && (affected_card_controller == player))
    {
      special_abilities2(player, affected_card, EVENT_ABILITIES, SP_KEYWORD2_GRANTED_PROT_MISC);
    }
  }
  result = eot_trigger(player, card, event);
  if ((result != 0) && (current_turn == player))
  {
    counts[0] = count_permanents_by_type(0, TYPE_CREATURE);
    counts[1] = count_permanents_by_type(1, TYPE_CREATURE);

    uVar3 = 0;
    do
    {
      uVar4 = current_turn ^ uVar3;
      if (counts[uVar4] <= counts[1 - uVar4])
      {
        default_token_definition(player, card, 0x56b6, &token);
        token.t_player = uVar4;
        generate_token(&token);
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 != 2);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25ad130; original cards/commander_legends.c:4254. */
int card_nevinyrral_urborg_tyrant(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  bool bVar5;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_token;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x49ac, &recovered_stack_token.token);
    recovered_stack_token.token.tou = 2;
    recovered_stack_token.token.pow = 2;
    recovered_stack_token.token.color_forced = '\x02';
    recovered_stack_token.token.qty = (unsigned char)creatures_dead_this_turn;
    recovered_stack_token.token.action = TOKEN_ACTION_TAPPED;
    generate_token(&recovered_stack_token.token);
    misc_hexproof(player, card, EVENT_ETB);
  }
  else
  {
    misc_hexproof(player, card, event);
    if ((((event == EVENT_PROT_MISC) && (card == affected_card)) &&
         (player == affected_card_controller)) &&
        (iVar1 = is_what(attacking_card_controller, attacking_card, 0x46), iVar1 != 0))
    {
      event_result = event_result | 1;
    }
  }
  iVar1 = duh_mode(player);
  iVar1 = this_dies_trigger(player, card, event, (iVar1 != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if ((iVar1 != 0) && (iVar2 = has_mana(player, 0, 1), iVar1 = current_turn, iVar2 != 0))
  {
    if (player == 1)
    {
      recovered_stack_token.token.id = 0;
      recovered_stack_token.token.s_player = 0;
      recovered_stack_token.token.s_card = 0;
      recovered_stack_token.token.eff_player = 0;
      iVar2 = active_cards_count[current_turn] + -1;
      if (-1 < iVar2)
      {
        bVar5 = current_turn != 1;
        do
        {
          if (((card != iVar2) || (bVar5)) &&
              ((instance = in_play(iVar1, iVar2),
                instance != NULL && (iVar4 = is_what(iVar1, iVar2, 0x46), iVar4 != 0))))
          {
            (&recovered_stack_token.token.id)[iVar1 * 2] =
                (&recovered_stack_token.token.id)[iVar1 * 2] + 1;
            iVar4 = get_cmc(iVar1, iVar2);
            (&recovered_stack_token.token.s_player)[iVar1 * 2] =
                (&recovered_stack_token.token.s_player)[iVar1 * 2] + iVar4;
          }
          iVar2 = iVar2 + -1;
        } while (iVar2 != -1);
      }
      iVar1 = 1 - current_turn;
      iVar2 = active_cards_count[iVar1] + -1;
      if (-1 < iVar2)
      {
        do
        {
          if ((((card != iVar2) || (iVar1 != 1)) &&
               (instance = in_play(iVar1, iVar2), instance != NULL)) &&
              (iVar4 = is_what(iVar1, iVar2, 0x46), iVar4 != 0))
          {
            *(int *)((int)&recovered_stack_token.token + iVar1 * 8) =
                *(int *)((int)&recovered_stack_token.token + iVar1 * 8) + 1;
            iVar4 = get_cmc(iVar1, iVar2);
            *(int *)((int)&recovered_stack_token.token + iVar1 * 8 + 4) =
                *(int *)((int)&recovered_stack_token.token + iVar1 * 8 + 4) + iVar4;
          }
          iVar2 = iVar2 + -1;
        } while (iVar2 != -1);
      }
      if (recovered_stack_token.token.id - recovered_stack_token.token.s_card < 2)
      {
        return 0;
      }
      if (recovered_stack_token.token.s_player - recovered_stack_token.token.eff_player < 5)
      {
        return 0;
      }
    }
    iVar1 = charge_mana(player, COLOR_COLORLESS, 1);
    if (iVar1 != 0)
    {
      default_test_definition(&recovered_stack_token.test, 0x46);
      new_manipulate_all(player, card, 2, &recovered_stack_token.test, ACT_KILL_DESTROY);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25ae850; original cards/commander_legends.c:4738. */
int card_rebbec_architect_of_ascension(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int card_00;

  check_legend_rule(player, card, event);
  if (event == EVENT_ABILITIES)
  {
    if ((affected_card_controller == player) &&
        (iVar1 = is_what(player, affected_card, 0x40), iVar1 != 0))
    {
      special_abilities2(affected_card_controller, affected_card, EVENT_ABILITIES,
                         SP_KEYWORD2_GRANTED_PROT_MISC);
    }
  }
  else if (((event == EVENT_PROT_MISC) && (affected_card_controller == player)) &&
           (iVar1 = is_what(player, affected_card, 0x40), iVar1 != 0))
  {
    if (((unsigned int)event_result >> 24 & 0xff) == '\x06')
    {
      event_result = event_result | 1;
    }
    else
    {

      iVar1 = get_cmc(attacking_card_controller, attacking_card);
      if (0 < active_cards_count[player])
      {
        card_00 = 0;
        do
        {
          instance = in_play(player, card_00);
          if (((instance != NULL) && (iVar3 = is_what(player, card_00, 0x40), iVar3 != 0)) &&
              (iVar3 = get_cmc(player, card_00), iVar3 == iVar1))
          {
            event_result = event_result | 1;
            return 0;
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[player]);
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25b10d0; original cards/commander_legends.c:5776. */
int card_trench_behemoth(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td2;

  landfall_mode(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td2, 2);
    td2.allowed_controller = 1 - player;
    td2.allow_cancel = 0;
    td2.preferred_controller = td2.allowed_controller;
    instance = get_card_instance(player, card);
    result = can_target(&td2);
    if ((result != 0) &&
        (result = new_pick_target(&td2, "Select target creature controlled by opponent.", 0, 0x80),
         result != 0))
    {
      create_targetted_legacy_effect(player, card, effect_must_attack_on_next_combat,
                                     instance->targets[0].player, instance->targets[0].card);
    }
    instance->number_of_targets = '\0';
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition(player, card, &td2, 1);
      td2.preferred_controller = player;
      td2.allowed_controller = player;
      td2.illegal_abilities = 0;
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 0, 0, 0, 0, 0, 0, 0,
                                           &td2, NULL);
        return result;
      }
      if (event == EVENT_ACTIVATE)
      {
        generic_activated_ability(player, card, EVENT_ACTIVATE, 6, 0, 0, 0, 0, 0, 0, 0, &td2,
                                  "Select a land you control.");
        if (spell_fizzled == 1)
        {
          return 0;
        }
        bounce_permanent(instance->targets[0].player, instance->targets[0].card);
        instance->number_of_targets = '\0';
        result = is_target(player, card, 1 - player);
        if (result == 0)
        {
          return 0;
        }
        ai_modifier = ai_modifier + 0x30;
        return 0;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        return 0;
      }
      default_target_definition(player, card, &td2, 1);
      td2.preferred_controller = player;
      td2.allowed_controller = player;
      td2.illegal_abilities = 0;
      instance = get_card_instance(player, card);
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      untap_card(instance->parent_controller, instance->parent_card);
      pump_ability_until_eot_no_repeat(player, card, instance->parent_controller,
                                       instance->parent_card, 0, (SP_KEYWORD_HEXPROOF));
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a3c10; original cards/commander_legends.c:606. */
int card_archelos_lagoon_mystic(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    card_flags[player] = card_flags[player] | 8;
    return 0;
  }
  if ((((event == EVENT_CAST_SPELL) || (event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE)) &&
       ((card != affected_card || (player != affected_card_controller)))) &&
      (((instance = in_play(player, card),
         instance != NULL &&
             (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
        (result = is_what(affected_card_controller, affected_card, 0x47), result != 0))))
  {
    result = is_tapped(player, card);
    if (result == 0)
    {
      result = check_state(affected_card_controller, affected_card, 0x10);
      if (result != 0)
      {
        remove_state(affected_card_controller, affected_card, 0x10);
      }
    }
    else
    {
      add_state(affected_card_controller, affected_card, 0x10);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25a3d50; original cards/commander_legends.c:633. */
int card_archon_of_coronation(int player, int card, event_t event)
{
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    you_become_the_monarch(player);
    return 0;
  }
  if (event != EVENT_STATIC_EFFECTS)
  {
    return 0;
  }
  card_flags[player] = card_flags[player] | 0x10;
  return 0;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x259ff10; original cards/commander_legends.c:5067. */
static int test_exclude_this_creature(int iid, int val, int player, int card)
{
  if (card != (val & 0xffffU))
  {
    return 1;
  }
  return (unsigned int)((unsigned int)val >> 0x10 != player);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25a22a0; original cards/commander_legends.c:3997. */
static void miara_trigger(int player, int amount)
{
  int result;
  if (amount < 1)
  {
    return;
  }
  do
  {
    result = has_mana(player, 0, 1);
    if (result == 0)
    {
      return;
    }
    result = can_pay_life(player, 1);
    if (result == 0)
    {
      return;
    }
    charge_mana(player, COLOR_COLORLESS, 1);
    if (spell_fizzled == 1)
    {
      return;
    }
    lose_life(player, 1);
    draw_cards(player, 1);
    amount = amount + -1;
  } while (amount != 0);
  return;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25a2700; original cards/commander_legends.c:4960. */
static int sakashima_effect(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  result = effect_follows_control_of_attachment(player, card, event);
  if ((result == 0) && (event == EVENT_STATIC_EFFECTS))
  {

    instance = in_play(player, card);

    if ((instance != NULL) &&
        ((-1 < instance->damage_target_player && (-1 < instance->damage_target_card))))
    {
      disable_legend_rule(player, card, EVENT_STATIC_EFFECTS, (int)instance->damage_target_player);
    }
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int miara_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x25a2340; original cards/commander_legends.c:4010. */
static int miara_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  card_instance_t *pcVar3;
  int result;
  test_definition_2022_t this_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x5697))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, 0x4700), result != 0))
    {
      return 0;
    }
  LAB_025a2378:
    uVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto LAB_025a239a;
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if ((-1 < pcVar1->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, event), result != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_025a2378;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);

    if ((pcVar3 != NULL) && (pcVar3 = get_card_instance(affected_card_controller, affected_card),
                             pcVar3->kill_code != '\0'))
    {
      default_test_definition_2022(&this_test, 0x47);
      this_test.subtype = 0x3c;

      if ((-1 < pcVar1->damage_target_player) &&
          ((pcVar1->damage_target_card == affected_card &&
            (pcVar1->damage_target_player == affected_card_controller))))
      {
        pcVar1->damage_target_card = -1;
        pcVar1->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        if (pcVar1->targets[0xb].player < 0)
        {
          pcVar1->targets[0xb].player = 0;
        }
        *(uint8_t *)((int)&pcVar1->targets[0xb].player + 1) = 1;
      }
      count_for_gfp_ability_2022(player, card, 0x77, player, 0, &this_test);
    }
    uVar2 = resolve_gfp_ability_2022(player, card, EVENT_GRAVEYARD_FROM_PLAY,
                                     RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {

    miara_trigger(player, uVar2 & 0xff);
    kill_card(player, card, KILL_EXILE);
  }
LAB_025a239a:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25ac8e0; original cards/commander_legends.c:4065. */
int card_miara_thorn_of_the_glade(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  unsigned int uVar3;
  int iVar4;
  test_definition_2022_t this_test;

  check_legend_rule(player, card, event);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar4 = 0;
        do
        {
          instance = in_play(player, iVar4);

          if ((((instance != NULL) && (iVar2 = is_what(player, iVar4, 0x80), iVar2 != 0)) &&
               (instance = get_card_instance(player, iVar4),
                instance->info_slot_fn == miara_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_025ac914;
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[player]);
      }

      iVar4 = create_targetted_legacy_effect(player, card, miara_effect, player, card);
      instance = get_card_instance(player, iVar4);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar4, 0x10000);
    }
  }
  else if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    default_test_definition_2022(&this_test, 0x47);
    this_test.subtype = 0x3c;
    count_for_gfp_ability_2022(player, card, 0x77, 2, 0, &this_test);
  }
LAB_025ac914:
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (uVar3 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY),
       uVar3 != 0))
  {

    miara_trigger(player, uVar3 & 0xff);
    return 0;
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25af300; original cards/commander_legends.c:4976. */
int card_sakashima_of_a_thousand_faces(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_2022_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    disable_legend_rule(player, card, EVENT_STATIC_EFFECTS, player);
    check_legend_rule(player, card, EVENT_STATIC_EFFECTS);
  }
  else
  {
    check_legend_rule(player, card, event);
    if (((event == EVENT_CHANGE_TYPE) && (card == affected_card)) &&
        (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar3 = 0;
        do
        {
          instance = in_play(player, iVar3);

          if ((((instance != NULL) && (iVar2 = is_what(player, iVar3, 0x80), iVar2 != 0)) &&
               (instance = get_card_instance(player, iVar3),
                instance->info_slot_fn == sakashima_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_025af335;
          iVar3 = iVar3 + 1;
        } while (iVar3 < active_cards_count[player]);
      }

      iVar3 = create_targetted_legacy_effect(player, card, sakashima_effect, player, card);
      instance = get_card_instance(player, iVar3);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar3, 0x10000);
    }
  }
LAB_025af335:
  base_target_definition_2022(player, card, &td, 2);
  td.preferred_controller = player;
  td.allowed_controller = player;
  enters_the_battlefield_as_copy_of_any_2022(player, card, event, &td,
                                             "TARGET_CREATURE_YOU_CONTROL");
  return 0;
}

/* ManalinkEh 2022: 0x25af5d0; original cards/commander_legends.c:5070. */
int card_sakashimas_will(int player, int card, event_t event)
{
  unsigned char bVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int uVar4;
  int iVar5;
  int iVar6;
  target_definition_2022_t td;
  target_definition_2022_t td2;
  target_definition_2022_t td3;
  test_definition_2022_t test;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition_2022(player, card, &td, 2);
    td.zone = 0x1000;
    default_target_definition_2022(player, card, &td2, 2);
    td2.who_chooses = 1 - player;
    td2.illegal_abilities = 0;
    td2.allow_cancel = 0;
    td2.allowed_controller = td2.who_chooses;
    td2.preferred_controller = td2.who_chooses;
    default_target_definition_2022(player, card, &td3, 2);
    td3.preferred_controller = player;
    td3.allowed_controller = player;
    td3.illegal_abilities = 0;
    td3.allow_cancel = 0;
    get_card_instance(player, card);
    iVar5 = basic_spell(player, card, EVENT_CAN_CAST);
    return iVar5;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition_2022(player, card, &td, 2);
      td.zone = 0x1000;
      default_target_definition_2022(player, card, &td2, 2);
      iVar5 = 1 - player;
      td2.illegal_abilities = 0;
      td2.allow_cancel = 0;
      td2.who_chooses = iVar5;
      td2.allowed_controller = iVar5;
      td2.preferred_controller = iVar5;
      default_target_definition_2022(player, card, &td3, 2);
      td3.preferred_controller = player;
      td3.allowed_controller = player;
      td3.illegal_abilities = 0;
      td3.allow_cancel = 0;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {

        instance->number_of_targets = '\0';
        instance->info_slot = 0;
        iVar3 = would_validate_arbitrary_target_2022(&td, iVar5, -1);
        uVar4 = 0;
        if (iVar3 != 0)
        {
          iVar6 = liutenant(player);
          uVar4 = (unsigned int)(iVar6 != 0);
        }
        uVar4 =
            dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033", "Steal a creature",
                        iVar3, 0x14, "Mass copy", 1, 10, "Do both", uVar4, 0x1e, 0, 0, 0, 0);
        if (uVar4 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if ((uVar4 & 1) != 0)
        {
          instance->targets[0].player = iVar5;
          instance->targets[0].card = -1;
          instance->number_of_targets = '\x01';
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition_2022(player, card, &td, 2);
    td.zone = 0x1000;
    default_target_definition_2022(player, card, &td2, 2);
    td2.who_chooses = 1 - player;
    td2.illegal_abilities = 0;
    td2.allow_cancel = 0;
    td2.allowed_controller = td2.who_chooses;
    td2.preferred_controller = td2.who_chooses;
    default_target_definition_2022(player, card, &td3, 2);
    td3.preferred_controller = player;
    td3.allowed_controller = player;
    td3.illegal_abilities = 0;
    td3.allow_cancel = 0;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      bVar1 = instance->mode;
      if ((((bVar1 & 1) != 0) && (iVar5 = valid_target_2022(&td), iVar5 != 0)) &&
          (iVar5 = can_target_2022(&td2), iVar5 != 0))
      {
        new_pick_target_2022(&td2, "Select a creature you control.", 1, 0x80);
        gain_control(player, card, instance->targets[1].player, instance->targets[1].card);
      }
      if (((bVar1 & 2) != 0) && (iVar5 = can_target_2022(&td3), iVar5 != 0))
      {
        new_pick_target_2022(&td3, "Select a creature you control.", 2, 0x80);
        new_default_test_definition_2022(&test, 2, "");
        test.special_selection_function = test_exclude_this_creature;
        test.value_for_special_selection_function =
            (unsigned int)(unsigned short)instance->targets[0].card | instance->targets[0].player
                                                                          << 0x10;
        shapeshift_all_2022(player, card, player, &test, instance->targets[2].player,
                            instance->targets[2].card, 1);
      }
      kill_card(player, card, KILL_BURY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition_2022(player, card, &td, 2);
    td.zone = 0x1000;
    default_target_definition_2022(player, card, &td2, 2);
    td2.who_chooses = 1 - player;
    td2.illegal_abilities = 0;
    td2.allow_cancel = 0;
    td2.allowed_controller = td2.who_chooses;
    td2.preferred_controller = td2.who_chooses;
    default_target_definition_2022(player, card, &td3, 2);
    td3.preferred_controller = player;
    td3.allowed_controller = player;
    td3.illegal_abilities = 0;
    td3.allow_cancel = 0;
    get_card_instance(player, card);
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25b21a0; original cards/commander_legends.c:6120. */
int card_zara_renegade_recruiter(int player, int card, event_t event)
{
  int target_card;
  test_definition_2022_t test;

  check_legend_rule(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if ((event == EVENT_DAT_ABILITY) && (hand_count[1 - player] != 0))
  {

    new_default_test_definition_2022(&test, 2, "Select a creature card.");
    target_card = new_global_tutor_2022(player, 1 - player, 4, 8, 0, 2, &test);
    if (-1 < target_card)
    {
      create_targetted_legacy_effect(player, card, bounce_at_eot, player, target_card);
    }
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x25b1670; original cards/commander_legends.c:5913. */
int card_vow_of_torment(int player, int card, event_t event)
{
  int result;

  result = vow_2022(player, card, event, 2, 2, 0, 0x10000);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25a9da0; original cards/commander_legends.c:3195. */
int card_kamahls_will(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  unsigned char bVar3;
  card_instance_t *pcVar4;
  int iVar5;
  card_instance_t *pcVar6;
  unsigned int uVar7;
  int iVar8;
  int *piVar9;
  int iVar10;
  bool bVar11;

  unsigned int uStack_31c;
  target_definition_2022_t td;
  target_definition_2022_t td2;
  int marked[151];

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition_2022(player, card, &td, 1);
    td.preferred_controller = player;
    td.allowed_controller = player;
    default_target_definition_2022(player, card, &td2, 2);
    td2.allowed_controller = 1 - player;
    td2.preferred_controller = td2.allowed_controller;
    get_card_instance(player, card);
    iVar10 = basic_spell(player, card, EVENT_CAN_CAST);
    if (iVar10 == 0)
    {
      return 0;
    }
    iVar10 = can_target_2022(&td);
    if (iVar10 != 0)
    {
      return 1;
    }
    iVar10 = can_target_2022(&td2);
    return (unsigned int)(iVar10 != 0);
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition_2022(player, card, &td, 1);
      td.preferred_controller = player;
      td.allowed_controller = player;
      default_target_definition_2022(player, card, &td2, 2);
      td2.allowed_controller = 1 - player;
      td2.preferred_controller = td2.allowed_controller;
      pcVar4 = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        pcVar4->number_of_targets = '\0';
        pcVar4->info_slot = 0;
        iVar10 = can_target_2022(&td);
        iVar5 = can_target_2022(&td2);
        if ((iVar5 == 0) || (iVar10 == 0))
        {
          uVar7 = 0;
        }
        else
        {
          iVar8 = liutenant(player);
          uVar7 = (unsigned int)(iVar8 != 0);
        }
        uVar7 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\033", "Animate lands",
                            iVar10, 10,
                            "All your creatures deals damage to target creature you don't control",
                            iVar5, 0x14, "Do both", uVar7, 0x1e, 0, 0, 0, 0);
        if (uVar7 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if ((uVar7 & 1) != 0)
        {
          iVar5 = 0;
          iVar10 = can_target_2022(&td);
          do
          {
            if ((iVar10 == 0) ||
                (iVar10 = new_pick_target_2022(&td, "Select target land you control.", iVar5, 0x80),
                 iVar10 == 0))
              break;
            state_untargettable(pcVar4->targets[iVar5].player, pcVar4->targets[iVar5].card, 1);
            iVar5 = iVar5 + 1;
            ai_modifier =
                ai_modifier +
                (current_phase <= (current_turn == player ? PHASE_MAIN1 : PHASE_BEFORE_BLOCKING)
                     ? 20
                     : -20);
            iVar10 = can_target_2022(&td);
          } while (iVar5 < 10);
          if (pcVar4->number_of_targets != '\0')
          {
            iVar10 = 0;
            do
            {
              ptVar2 = pcVar4->targets + iVar10;
              ptVar1 = pcVar4->targets + iVar10;
              iVar10 = iVar10 + 1;
              state_untargettable(ptVar1->player, ptVar2->card, 0);
            } while (iVar10 < (int)(unsigned int)pcVar4->number_of_targets);
          }
        }
        if (((uVar7 & 2) != 0) &&
            (iVar10 = new_pick_target_2022(&td2, "Select target creature you don\'t control.",
                                           (unsigned int)pcVar4->number_of_targets, 0x81),
             iVar10 != 0))
        {
          iVar10 = count_subtype(player, 2, -1);
          iVar5 = iVar10 * 0x14;
          if ((current_turn != player) || (0x14 < current_phase))
          {
            iVar5 = iVar10 * -0x14;
          }
          ai_modifier = ai_modifier + iVar5;
        }
      }
    }
    return 0;
  }
  if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
      (event != EVENT_CHANGE_TARGET))
  {
    return 0;
  }
  default_target_definition_2022(player, card, &td, 1);
  td.preferred_controller = player;
  td.allowed_controller = player;
  default_target_definition_2022(player, card, &td2, 2);
  td2.allowed_controller = 1 - player;
  td2.preferred_controller = td2.allowed_controller;
  pcVar4 = get_card_instance(player, card);
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }
  bVar3 = pcVar4->mode & 2;
  if ((pcVar4->mode & 1) == 0)
  {
  LAB_025a9f71:
    if (bVar3 == 0)
      goto LAB_025a9f7e;
    uStack_31c = pcVar4->number_of_targets - 1;
  }
  else
  {
    uStack_31c = (unsigned int)pcVar4->number_of_targets;
    if (bVar3 == 0)
    {
      if (uStack_31c == 0)
        goto LAB_025a9f7e;
    LAB_025a9e95:
      iVar10 = 0;
      do
      {
        while (iVar5 = validate_target_2022(player, card, &td, iVar10), iVar5 != 0)
        {
          iVar5 = turn_into_creature(player, card, pcVar4->targets[iVar10].player,
                                     pcVar4->targets[iVar10].card, 1, 1, 1);
          pcVar6 = get_card_instance(player, iVar5);
          pcVar6->targets[8].card = 0x80c;
          pcVar6 = get_card_instance(player, iVar5);
          pcVar6->targets[9].card = 0x3a;
          ptVar2 = pcVar4->targets + iVar10;
          ptVar1 = pcVar4->targets + iVar10;
          iVar10 = iVar10 + 1;
          get_abilities(ptVar1->player, ptVar2->card, EVENT_CHANGE_TYPE, -1);
          if ((int)uStack_31c <= iVar10)
            goto LAB_025a9f71;
        }
        iVar10 = iVar10 + 1;
      } while (iVar10 < (int)uStack_31c);
      goto LAB_025a9f71;
    }
    uStack_31c = uStack_31c - 1;
    if (0 < (int)uStack_31c)
      goto LAB_025a9e95;
  }
  uStack_31c = (int)(uStack_31c ^ 0xffffffff) >> 0x1f & uStack_31c;
  iVar10 = validate_target_2022(player, card, &td2, uStack_31c);
  if ((iVar10 != 0) && (0 < active_cards_count[player]))
  {
    iVar5 = 0;
    iVar10 = 0;
    do
    {
      pcVar6 = in_play(player, iVar5);
      if ((pcVar6 != NULL) && (iVar8 = is_what(player, iVar5, 2), iVar8 != 0))
      {
        marked[iVar10] = iVar5;
        iVar10 = iVar10 + 1;
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < active_cards_count[player]);
    if (0 < iVar10)
    {
      piVar9 = marked + iVar10 + -1;
      do
      {
        iVar10 = *piVar9;
        iVar5 = get_power(player, iVar10);
        damage_creature(pcVar4->targets[uStack_31c].player, pcVar4->targets[uStack_31c].card, iVar5,
                        player, iVar10);
        bVar11 = marked != piVar9;
        piVar9 = piVar9 + -1;
      } while (bVar11);
    }
  }
LAB_025a9f7e:
  kill_card(player, card, KILL_BURY);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x25aa4f0; original cards/commander_legends.c:3299. */
int card_kamahl_heart_of_krosa(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  target_definition_2022_t td;
  check_legend_rule(player, card, event);
  result = beginning_of_combat_2022(player, card, event, player);
  if (result != 0)
  {
    pump_creatures_until_eot_2022(player, card, player, 0, 3, 3, 0x80, 0, NULL);
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition_2022(player, card, &td, 1);
    td.preferred_controller = player;
    td.allowed_controller = player;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 4, 1, 0, 0, 1, 0, 0,
                                              0, &td, NULL);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 6, 1, 0, 0, 1, 0, 0, 0, &td,
                                     "Select target land you control.");
      if (spell_fizzled == 1)
      {
        return 0;
      }
      ai_modifier =
          ai_modifier +
          (current_phase <= (current_turn == player ? PHASE_MAIN1 : PHASE_BEFORE_BLOCKING) ? 20
                                                                                           : -20);
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition_2022(player, card, &td, 1);
  }
  td.allowed_controller = player;
  if ((event == EVENT_RESOLVE_ACTIVATION) && (td.preferred_controller = td.allowed_controller,
                                              result = valid_target_2022(&td), result != 0))
  {
    instance = get_card_instance(player, card);
    result = turn_into_creature(player, card, instance->targets[0].player,
                                instance->targets[0].card, 1, 1, 1);
    instance = get_card_instance(player, result);
    instance->targets[8].card = 0x80c;
    instance = get_card_instance(player, result);
    instance->targets[9].card = 0x3a;
    return 0;
  }
  return 0;
}

#endif
