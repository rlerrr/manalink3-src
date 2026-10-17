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
      result = charge_mana_for_activated_ability(player, card, 0, 0, 0, 1, 0, 0);
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

  result = check_special_flags(player, card, 0x1000);
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
  vigilance(player, card, event);
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
                 (iVar2 = check_special_flags(player, card_00, 0x1000), iVar2 == 0)) ||
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
    result = check_special_flags2(affected_card_controller, affected_card, 0x1000000);
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
          pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card,
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
    lose_life(0, iVar2);
    new_multidiscard(0, iVar2, 2, player);
    iVar2 = count_permanents_you_control_but_you_dont_own(1, 0x47);
    lose_life(1, iVar2);
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
  target_definition_t td1;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {

    default_target_definition(player, card, &td1, 2);
    instance = get_card_instance(player, card);
    result = can_target(&td1);
    if (result != 0)
    {
      result = new_pick_target(&td1, "TARGET_CREATURE", 0, 0);
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
    default_test_definition((test_definition_t *)&td1, 0x47);
    etb_ability_global(player, event, 2, TYPE_NONE, (test_definition_t *)&td1, player, card);
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
      result = check_special_flags2(affected_card_controller, affected_card, 0x1000000);
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
      result = check_special_flags2(trigger_cause_controller, trigger_cause, 0x1000000);
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
      gain_life(player, iVar4);
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
    generate_token_by_id(player, card, 0x4998);
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
  vigilance(player, card, event);
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

  result = check_special_flags2(player, card, 0x1000000);
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
      gain_life(current_turn, 1);
    }
    if (dc[current_turn ^ 1U] == 1)
    {
      gain_life(current_turn ^ 1U, 1);
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
  generate_token_by_id(player, card, 0x4986);
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
          pump_ability_until_eot_no_repeat(player, card, current_turn, (unsigned int)*pbVar3,
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
  etb_ability(player, card, event, (iVar1 != 0) + RESOLVE_TRIGGER_OPTIONAL);
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

  result = check_special_flags2(player, card, 0x1000000);
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
  pump_until_eot(player, card, player, card, power, power);
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
  vigilance(player, card, event);
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
    iVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
    iVar2 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
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
    uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
    uVar2 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
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
    etb_ability(player, card, EVENT_STATIC_EFFECTS, RESOLVE_TRIGGER_MANDATORY);
  }
  else
  {
    etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
              (iVar4 = check_special_flags(affected_card_controller, affected_card, 0x800),
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
            (iVar2 = check_special_flags(affected_card_controller, affected_card, 0x800),
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
  lose_life(1 - player, 1);
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
    uVar2 = resolve_gfp_ability(player, card, event_00, RESOLVE_TRIGGER_MANDATORY);
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
    uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
      (uVar4 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar4 != 0))
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
    etb_ability(player, card, EVENT_CAST_SPELL, RESOLVE_TRIGGER_MANDATORY);
    return 0;
  }
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
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
  test_definition_t test;

  check_legend_rule(player, card, event);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 2);
    test.not_me = 1;
    result = duh_mode(player);
    etb_ability_global(player, event, (result != 0) + 1, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, (target_definition_t *)&test, 2);
    result = can_target((target_definition_t *)&test);
    if (result != 0)
    {
      result = has_mana(player, 0, 2);
      if (result != 0)
      {
        charge_mana(player, COLOR_COLORLESS, 2);
        if (spell_fizzled != 1)
        {
          result = new_pick_target((target_definition_t *)&test, "TARGET_CREATURE", 0, 1);
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
      set_special_flags(player, result, 0x50000);
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
      generate_token_by_id(player, card, 0x4986);
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
