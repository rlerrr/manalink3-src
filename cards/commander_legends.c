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
  result = check_special_flags2(player, card, 0x1000000);
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
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  generate_token_by_id(player, card, 0x4998);
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
  target_definition_t td1;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td1, 2);
    pcVar2 = get_card_instance(player, card);
    result = can_target(&td1);
    if (result != 0)
    {
      result = new_pick_target(&td1, "TARGET_CREATURE", 0, 0);
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
    default_test_definition((test_definition_t *)&td1, 0x47);
    etb_ability_global(player, event, 2, TYPE_NONE, (test_definition_t *)&td1, player, card);
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
          copy_spell(player, (unsigned int)cards_data[result].id);
          copy_spell(player, (unsigned int)cards_data[result].id);
          copy_spell(player, (unsigned int)cards_data[result].id);
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
  pump_ability_until_eot_no_repeat(player, card, instance->parent_controller, instance->parent_card,
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
  result = check_special_flags2(player, card, 0x1000000);
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
  generate_token_by_id(player, card, 0x56b0);
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
