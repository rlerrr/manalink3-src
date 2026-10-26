#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2336740; original cards/modern_horizons.c:229. */
int card_ayulas_influence(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  test_definition_t this_test;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    new_default_test_definition(&this_test, 1, "Select a land card to discard.");
    this_test.zone = 0x100;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 0, 0, 0,
                                         NULL, NULL);
      if (result == 0)
      {
        return 0;
      }
      result = check_battlefield_for_special_card(player, card, player, 0, &this_test);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      result = charge_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0);
      if (result == 0)
      {
        return 0;
      }
      instance->number_of_targets = '\0';
      result = new_select_a_card(player, player, 4, 0, 3, -1, &this_test);
      if (result != -1)
      {
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
      result = global_enchantment(player, card, event);
      return result;
    }
    new_default_test_definition(&this_test, 1, "Select a land card to discard.");
    this_test.zone = 0x100;
    get_card_instance(player, card);
  }
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  generate_token_by_id(player, card, 0x4906);
  return 0;
}

/* ManalinkEh 2022: 0x2336cb0; original cards/modern_horizons.c:327. */
int card_azra_smokeshaper(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "ASHNODS_BATTLEGEAR", 0, 0);
      if (result != 0)
      {
        pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card,
                               0, 0, 0, 0x800);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = ninjutsu(player, card, event, 1, 1, 0, 0, 0, 0, 0x4e64);
  return result;
}

/* ManalinkEh 2022: 0x2337190; original cards/modern_horizons.c:594. */
int card_defile(int player, int card, event_t event)
{
  int result;

  result = get_hacked_subtype(player, card, 0x5009);
  result = count_subtype(player, 1, result);
  result = vanilla_instant_pump(player, card, event, 2, player, -result, -result, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2337210; original cards/modern_horizons.c:605. */
int card_dregscape_sliver(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if ((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS))
  {
    result = unearth(player, event, 2, 0, 0, 0, 0, 0);
    return result;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = has_mana_multi(player, 2, 0, 0, 0, 0, 0);
    if (((result != 0) &&
         (result = any_in_graveyard_by_subtype(player, SUBTYPE_SLIVER), result != 0)) &&
        (result = can_sorcery_be_played(player, EVENT_CAN_ACTIVATE), result != 0))
    {
      return 1;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    charge_mana_multi(player, 2, 0, 0, 0, 0, 0);
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    new_default_test_definition(&this_test, 2, "Select a Sliver to unearth.");
    this_test.subtype = 0xb1;
    result = new_select_a_card(player, player, 2, 1, 2, -1, &this_test);
    if (result != -1)
    {
      reanimate_permanent(player, card, player, result, REANIMATE_UNEARTH);
    }
  }
  result = slivercycling(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2337400; original cards/modern_horizons.c:641. */
int card_echo_of_eons(int player, int card, event_t event)
{
  int result;
  if (((event == EVENT_GRAVEYARD_ABILITY) || (event == EVENT_PAY_FLASHBACK_COSTS)) ||
      ((event - EVENT_RESOLVE_ACTIVATED_GRAVEYARD_ABILITY & 0xfffffeff) == 0))
  {
    result = do_flashback(player, card, event, 2, 0, 1, 0, 0, 0);
    return result;
  }
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
    else if (event == EVENT_RESOLVE_SPELL)
    {
      result = get_flashback(player, card);
      if (result == 0)
      {
        resolve_timetwister(player, card, EVENT_RESOLVE_SPELL);
        kill_card(player, card, KILL_DESTROY);
      }
      else
      {
        resolve_timetwister(player, card, EVENT_RESOLVE_SPELL);
        kill_card(player, card, KILL_EXILE);
      }
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2337fe0; original cards/modern_horizons.c:970. */
int card_fallen_shinobi(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  card_instance_t *instance;
  test_definition_t this_test;
  result = damage_dealt_by_me(player, card, event,
                              DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                  DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result != 0)
  {
    instance = get_card_instance(player, card);
    uVar1 = instance->targets[1].player;
    instance->targets[1].player = 0;
    if ((char)uVar1 != '\0')
    {
      default_test_definition(&this_test, 0xff);
      exile_top_x_card_you_may_play_them(player, card, 2, &this_test, 0,
                                         MPCFE_UNTIL_EOT | MPCFE_FOR_FREE);
    }
    if ((uVar1 >> 8 & 0xff) != 0)
    {
      default_test_definition(&this_test, 0xff);
      exile_top_x_card_you_may_play_them(player, card, 2, &this_test, 1,
                                         MPCFE_UNTIL_EOT | MPCFE_FOR_FREE);
    }
  }
  result = ninjutsu(player, card, event, 2, 1, 1, 0, 0, 0, 0x4e85);
  return result;
}

/* ManalinkEh 2022: 0x2338a10; original cards/modern_horizons.c:1254. */
int card_generous_gift(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  token_generation_t token;
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
        default_target_definition(player, card, &td, 0x47);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            default_token_definition(player, card, 0x4929, &token);
            token.t_player = instance->targets[0].player;
            generate_token(&token);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02338a7c;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  get_card_instance(player, card);
LAB_02338a7c:
  result = generic_spell(player, card, event, 1, &td, "TARGET_PERMANENT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2339350; original cards/modern_horizons.c:1486. */
int card_hall_of_heliods_generosity(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  result = recycling_land(player, card, event, 4, 1, 0, 0, 0, 0, 1);
  return result;
}

/* ManalinkEh 2022: 0x233a4b0; original cards/modern_horizons.c:2005. */
int card_ingenious_infiltrator(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (event == EVENT_DEAL_DAMAGE)
  {
    default_test_definition(&test, 2);
    test.subtype = 0x83;
    check_damage_test(player, -1, EVENT_DEAL_DAMAGE,
                      DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                          DDBM_MUST_BE_COMBAT_DAMAGE,
                      player, card, 0, &test);
  }
  result = resolve_damage_trigger(
      player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE, player, card);
  if (result != 0)
  {
    draw_cards(player, result);
  }
  result = ninjutsu(player, card, event, 0, 1, 1, 0, 0, 0, 0x4eaa);
  return result;
}

/* ManalinkEh 2022: 0x233ab10; original cards/modern_horizons.c:2177. */
int card_lavabelly_sliver(int player, int card, event_t event)
{
  int card_00;
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_this_test;

  if ((((trigger_condition == 0xdb) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    default_test_definition(&recovered_stack_this_test.test, 0x47);
    recovered_stack_this_test.test.subtype = 0xb1;
    etb_ability_global(player, event, 2, TYPE_NONE, &recovered_stack_this_test.test, player, card);
  }
  card_00 = trigger_cause;
  iVar1 = trigger_cause_controller;
  if (event == EVENT_ETB)
  {
    default_target_definition(trigger_cause_controller, trigger_cause,
                              &recovered_stack_this_test.target, 0x100);
    recovered_stack_this_test.test.message[0xc] = '\0';
    recovered_stack_this_test.test.message[0xd] = '\x12';
    recovered_stack_this_test.test.message[0xe] = '\0';
    recovered_stack_this_test.test.message[0xf] = '\0';
    recovered_stack_this_test.test.message[0x44] = '\0';
    recovered_stack_this_test.test.message[0x45] = '\0';
    recovered_stack_this_test.test.message[0x46] = '\0';
    recovered_stack_this_test.test.message[0x47] = '\0';
    instance = get_card_instance(iVar1, card_00);
    iVar3 = can_target(&recovered_stack_this_test.target);
    if (iVar3 != 0)
    {
      iVar3 =
          new_pick_target(&recovered_stack_this_test.target, "TARGET_PLAYER_OR_PLANESWALKER", 0, 0);
      if (iVar3 != 0)
      {
        damage_creature(instance->targets[0].player, instance->targets[0].card, 1, iVar1, card_00);
        gain_life(player, 1);
      }
    }
    instance->number_of_targets = '\0';
  }
  iVar1 = slivercycling(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x233ad60; original cards/modern_horizons.c:2239. */
int card_llanowar_tribe(int player, int card, event_t event)
{
  int result;
  result = mana_producing_creature(player, card, event, 5, COLOR_GREEN, 3);
  return result;
}

/* ManalinkEh 2022: 0x233af90; original cards/modern_horizons.c:2312. */
int card_mist_syndicate_naga(int player, int card, event_t event)
{
  int result;
  result =
      damage_dealt_by_me(player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result != 0)
  {
    copy_token(player, card, player, card);
  }
  result = ninjutsu(player, card, event, 2, 0, 1, 0, 0, 0, 0x4eb9);
  return result;
}

/* ManalinkEh 2022: 0x233b030; original cards/modern_horizons.c:2334. */
int card_moonblade_shinobi(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  result =
      damage_dealt_by_me(player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result != 0)
  {
    default_token_definition(player, card, 0x4951, &token);
    token.tou = 1;
    token.pow = 1;
    token.key_plus = 0x20;
    generate_token(&token);
  }
  result = ninjutsu(player, card, event, 2, 0, 1, 0, 0, 0, 0x4ebb);
  return result;
}

/* ManalinkEh 2022: 0x233b370; original cards/modern_horizons.c:2426. */
int card_natures_chant(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  hybrid(player, card, event);
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
        default_target_definition(player, card, &td, 0x44);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0233b3e9;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x44);
  get_card_instance(player, card);
LAB_0233b3e9:
  result = generic_spell(player, card, event, 1, &td, "DISENCHANT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x233b4d0; original cards/modern_horizons.c:2454. */
int card_ninja_of_the_new_moon(int player, int card, event_t event)
{
  int result;
  result = ninjutsu(player, card, event, 3, 1, 0, 0, 0, 0, 0x4ec2);
  return result;
}

/* ManalinkEh 2022: 0x233bd50; original cards/modern_horizons.c:2706. */
int card_putrid_goblin(int player, int card, event_t event)
{
  persist(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x233bd80; original cards/modern_horizons.c:2736. */
int card_ranger_captain_of_eos(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&this_test, 2, "Select a creature with CMC 1 or less");
    this_test.cmc = 2;
    this_test.cmc_flag = 3;
    new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_0233bdf9:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_0233bdfe;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_0233bdf9;
  }
  target_player_cant_cast_type(player, card, 1 - player, 0x17c);
LAB_0233bdfe:
  result = generic_activated_ability(player, card, event, 8, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x233bee0; original cards/modern_horizons.c:2768. */
int card_ransack_the_lab(int player, int card, event_t event)
{
  int result;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
        return 0;
      }
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      reveal_top_cards_of_library_and_choose_type(player, card, player, 3, 1, TUTOR_HAND, 0,
                                                  TUTOR_GRAVE, 0, TYPE_ANY);
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x233c360; original cards/modern_horizons.c:3059. */
int card_shenanigans(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_DREDGE)
  {
    return 1;
  }
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
        default_target_definition(player, card, &td, 0x40);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0233c3d7;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x40);
  get_card_instance(player, card);
LAB_0233c3d7:
  result = generic_spell(player, card, event, 1, &td, "TARGET_ARTIFACT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x233c830; original cards/modern_horizons.c:3148. */
int card_smiting_helix(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  if (event == EVENT_GRAVEYARD_ABILITY)
  {
  LAB_0233c92a:
    result = do_flashback(player, card, event, 0, 0, 0, 0, 1, 1);
    return result;
  }
  if (event == EVENT_PAY_FLASHBACK_COSTS)
  {
    ai_modifier = ai_modifier + ((life[player] < 6) - 1 & 0xffffffe8) + 0x30;
    goto LAB_0233c92a;
  }
  if ((event - EVENT_RESOLVE_ACTIVATED_GRAVEYARD_ABILITY & 0xfffffeff) == 0)
    goto LAB_0233c92a;
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
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      if ((card == affected_card) && (player == affected_card_controller))
      {
        ai_modifier = ai_modifier + ((life[player] < 6) - 1 & 0xffffffe8) + 0x30;
      }
      goto LAB_0233c8bc;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0x102);
      td.zone = 0x1200;
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = valid_target(&td);
        if (result != 0)
        {
          damage_target0(player, card, 3);
          gain_life(player, 3);
        }
        result = get_flashback(player, card);
        if (result == 0)
        {
          kill_card(player, card, KILL_DESTROY);
        }
        else
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
      goto LAB_0233c8bc;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
LAB_0233c8bc:
  result = generic_spell(player, card, event, 1, &td, "TARGET_ANY", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23354b0; original cards/modern_horizons.c:3255. */
int spiteful_sliver_legacy(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int player_00;
  target_definition_t td;

  if (((trigger_condition == 0xd7) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    pcVar2 = get_card_instance(player, card);
    if (pcVar2 != NULL)
    {
      player_00 = (int)pcVar2->damage_target_player;
      iVar1 = pcVar2->damage_target_card;
      pcVar3 = get_card_instance(player_00, iVar1);
      if ((pcVar3 != NULL) && (reason_for_trigger_controller == player_00))
      {
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
        }
        else if (event == EVENT_RESOLVE_TRIGGER)
        {
          default_target_definition(player_00, iVar1, &td, 0x100);
          td.zone = 0x1200;
          td.allow_cancel = 0;
          pcVar3 = get_card_instance(player_00, iVar1);
          iVar4 = can_target(&td);
          if (iVar4 != 0)
          {
            iVar4 = new_pick_target(&td, "TARGET_PLAYER_OR_PLANESWALKER", 0, 0);
            if (iVar4 != 0)
            {
              damage_creature(pcVar3->targets[0].player, pcVar3->targets[0].card,
                              pcVar2->targets[9].card, player_00, iVar1);
            }
          }
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
  }
  iVar1 = eot_trigger(player, card, event);
  if (iVar1 == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x233cde0; original cards/modern_horizons.c:3311. */
int card_springbloom_druid(int player, int card, event_t event)
{
  int result;
  test_definition_t test_sac;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test_sac, 1, "Select a land to sacrifice.");
    result = new_sacrifice(player, card, player, 0, &test_sac);
    if (result != 0)
    {
      tutor_basic_lands(player, TUTOR_PLAY_TAPPED, 2);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x233ce90; original cards/modern_horizons.c:3356. */
int card_sword_of_sinew_and_steel(int player, int card, event_t event)
{
  int iVar1;
  unsigned int uVar2;
  unsigned int uVar3;
  card_instance_t *instance;
  int iVar5;
  target_definition_t td1;
  target_definition_t td2;

  iVar1 = equipped_creature_deals_damage(player, card, event,
                                         DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (iVar1 != 0)
  {
    default_target_definition(player, card, &td1, 0x100);
    default_target_definition(player, card, &td2, 0x40);
    td2.allow_cancel = 1;
    td1.allow_cancel = 1;
    instance = get_card_instance(player, card);
    if (0 < iVar1)
    {
      do
      {
        instance->number_of_targets = '\0';
        iVar5 = can_target(&td1);
        if ((iVar5 != 0) &&
            (iVar5 = new_pick_target(&td1, "TARGET_PLANESWALKER", 0, 0), iVar5 != 0))
        {
          kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        }
        instance->number_of_targets = '\0';
        iVar5 = can_target(&td2);
        if ((iVar5 != 0) && (iVar5 = new_pick_target(&td2, "TARGET_ARTIFACT", 0, 0), iVar5 != 0))
        {
          kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        }
        iVar1 = iVar1 + -1;
      } while (iVar1 != 0);
    }
  }
  uVar2 = get_sleighted_protection(player, card, 0x800);
  uVar3 = get_sleighted_protection(player, card, 0x4000);
  iVar1 = vanilla_equipment(player, card, event, 2, 2, 2, uVar2 | uVar3, 0);
  return iVar1;
}

/* ManalinkEh 2022: 0x233d5d0; original cards/modern_horizons.c:3552. */
int card_throatseeker(int player, int card, event_t event)
{
  int result;
  if ((event == EVENT_ABILITIES) && (affected_card_controller == player))
  {
    result = has_subtype(affected_card_controller, affected_card, SUBTYPE_NINJA);
    if (result != 0)
    {
      result = is_unblocked(affected_card_controller, affected_card);
      if (result != 0)
      {
        lifelink(affected_card_controller, affected_card, EVENT_ABILITIES);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x233d6d0; original cards/modern_horizons.c:3609. */
int card_tribute_mage(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  new_default_test_definition(&this_test, 0x40, "Select an artifact card with CMC 2");
  this_test.cmc = 2;
  this_test.cmc_flag = 0;
  new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
  return 0;
}

/* ManalinkEh 2022: 0x233dbb0; original cards/modern_horizons.c:3827. */
int card_weather_the_storm(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  if (event == EVENT_CAN_CAST)
  {
    iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 0, NULL, NULL, 0, NULL);
    return iVar2;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (basic_spell(player, card, EVENT_CAST_SPELL), spell_fizzled != 1))
    {
      iVar1 = check_special_flags(player, card, 0x40000);
      iVar2 = 0;
      if (iVar1 == 0)
      {
        for (; iVar1 = get_storm_count(), iVar2 < iVar1; iVar2 = iVar2 + 1)
        {
          gain_life(player, 3);
        }
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    gain_life(player, 3);
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2336e60; original cards/modern_horizons.c:452. */
int card_cabal_therapist(int player, int card, event_t event)
{
  int t_player;
  card_instance_t *instance;
  int result;
  ec_definition_t this_definition;
  target_definition_t td;
  test_definition_t test_sac;
  test_definition_t this_test;
  test_definition_t this_test2;

  menace(player, card, event);
  if (event != EVENT_PHASE_CHANGED)
  {
    return 0;
  }
  if ((current_turn == player) && (current_phase == 0x14))
  {
    instance = get_card_instance(player, card);
    if ((instance->info_slot != 0) &&
        (result = can_sacrifice_type_as_cost(player, 1, TYPE_CREATURE), result != 0))
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
      instance->token_status = instance->token_status | 0x20000;
      new_default_test_definition(&test_sac, 2, "Select a creature to sacrifice.");
      result = new_sacrifice(player, card, player, 0, &test_sac);
      if (result == 0)
      {
        instance->token_status = instance->token_status & 0xfffdffff;
        return 0;
      }
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      new_pick_target(&td, "TARGET_PLAYER", 0, 1);
      result = valid_target(&td);
      if ((result != 0) && (0 < hand_count[instance->targets[0].player]))
      {
        new_default_test_definition(&this_test, 1, "Select a nonland card.");
        this_test.type_flag = 1;
        result = name_a_card(player, instance->targets[0].player, &this_test, 3);
        if (-1 < result)
        {
          default_test_definition(&this_test2, 0xff);
          t_player = instance->targets[0].player;
          this_test2.id = result;
          default_ec_definition(t_player, t_player, &this_definition);
          this_definition.effect = 0x81;
          new_effect_coercion(&this_definition, &this_test2);
        }
      }
      instance->token_status = instance->token_status & 0xfffdffff;
    }
    if ((current_phase == 0x14) && (current_turn == player))
    {
      return 0;
    }
  }
  instance = get_card_instance(player, card);
  instance->info_slot = 0x42;
  return 0;
}

/* ManalinkEh 2022: 0x2337c30; original cards/modern_horizons.c:866. */
int card_ephemerate(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
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
        default_target_definition(player, card, &td, 2);
        td.allowed_controller = player;
        td.preferred_controller = player;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result == 0)
          {
            kill_card(player, card, KILL_DESTROY);
          }
          else
          {
            blink_effect(instance->targets[0].player, instance->targets[0].card, NULL);
            rebound(player, card);
          }
        }
        goto LAB_02337c9c;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = player;
  td.preferred_controller = player;
  get_card_instance(player, card);
LAB_02337c9c:
  result = generic_spell(player, card, event, 1, &td, "ASHNODS_BATTLEGEAR", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2337da0; original cards/modern_horizons.c:898. */
int card_etchings_of_the_chosen(int player, int card, event_t event)
{
  card_instance_t *instance;
  const char *msg;
  int result;
  subtype_t subtype;
  target_definition_t td;
  test_definition_t test;

  instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    subtype = select_a_subtype(player, card);
    instance->info_slot = subtype;
    boost_creature_type(player, card, EVENT_RESOLVE_SPELL, subtype, 1, 1, 0,
                        BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  }
  else
  {
    boost_creature_type(player, card, event, instance->info_slot, 1, 1, 0,
                        BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
    if ((((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
         (event == EVENT_RESOLVE_ACTIVATION)) ||
        (event == EVENT_CLEANUP))
    {
      msg = get_subtype_text("Select %a to sacrifice.", instance->info_slot);
      new_default_test_definition(&test, 2, msg);
      test.subtype = instance->info_slot;
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      td.preferred_controller = player;
      if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
      {
        pump_ability_until_eot_no_repeat(instance->parent_controller, instance->parent_card,
                                         instance->targets[0].player, instance->targets[0].card, 0,
                                         0x800);
      }
      result = generic_activated_ability_with_sacrifice_as_cost(
          player, card, event, GAA_CAN_TARGET, 1, 0, 0, 0, 0, 0, 0, &td, "ASHNODS_BATTLEGEAR",
          TYPE_NONE, &test, SARV_0);
      return result;
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2338130; original cards/modern_horizons.c:1003. */
int card_feaster_of_fools(int player, int card, event_t event)
{
  card_ptr_t *pcVar1;
  card_instance_t *instance;
  int result;

  devour(player, card, event, 2);
  instance = get_card_instance(player, card);
  if (event == EVENT_MODIFY_COST)
  {
    result = get_id(player, card);
    pcVar1 = cards_ptr[result];
    result = has_convoked_mana_extended(
        player, card, EVENT_MODIFY_COST, (unsigned int)pcVar1->req_colorless,
        (unsigned int)pcVar1->req_black, (unsigned int)pcVar1->req_blue,
        (unsigned int)pcVar1->req_green, (unsigned int)pcVar1->req_red,
        (unsigned int)pcVar1->req_white);
    if (result != 2)
    {
    code_r0x021aa704:
      instance->info_slot = 0;
      return 0;
    }
    null_casting_cost(player, card);
    instance->info_slot = 1;
  }
  else if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    result = cast_spell_with_convoke(player, card, EVENT_CAST_SPELL);
    if (result != 0)
      goto code_r0x021aa704;
    spell_fizzled = 1;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2338fa0; original cards/modern_horizons.c:1394. */
int card_goblin_engineer(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  test_definition_t test_sac;
  test_definition_t test_return;

  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    new_default_test_definition(&test_return, 0x40,
                                "Select an artifact card to put into your graveyard.");
    new_global_tutor(player, player, 1, 3, 0, 6, &test_return);
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    new_default_test_definition(&test_sac, 0x40, "Select an artifact to sacrifice.");
    new_default_test_definition(&test_return, 0x40, "Select an artifact card with CMC 3 or less.");
    test_return.cmc = 4;
    test_return.cmc_flag = 3;
    test_return.ai_selection_mode = 1;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = new_can_sacrifice_as_cost(player, card, &test_sac);
      if (result == 0)
      {
        return 0;
      }
      result = generic_activated_ability_with_target_in_grave(
          player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, 0, 0, 0, 0, 1, 0, 0, NULL, NULL,
          TYPE_NONE, &test_return, 8);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 1, 0, 0, NULL, NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      uVar2 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                            &test_sac);
      if (uVar2 != 0)
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
          kill_card((int)uVar2 >> 0x10 & 0xff, uVar2 >> 0x18, KILL_SACRIFICE);
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
    new_default_test_definition(&test_sac, 0x40, "Select an artifact to sacrifice.");
    new_default_test_definition(&test_return, 0x40, "Select an artifact card with CMC 3 or less.");
  }
  test_return.ai_selection_mode = 1;
  test_return.cmc_flag = 3;
  test_return.cmc = 4;
  if ((event == EVENT_RESOLVE_ACTIVATION) &&
      (result = validate_target_from_grave_source(player, card, player, 0), result != -1))
  {
    reanimate_permanent(player, card, player, result, REANIMATE_DEFAULT);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x233b530; original cards/modern_horizons.c:2519. */
int card_plague_engineer(int player, int card, event_t event)
{
  card_instance_t *instance;
  subtype_t subtype;

  deathtouch(player, card, event);
  instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    subtype = select_a_subtype(player, card);
    instance->info_slot = subtype;
  }
  else
  {
    subtype = instance->info_slot;
  }
  boost_creature_type(player, card, event, subtype, -1, -1, 0, BCT_OPPONENT_ONLY);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2336250; original cards/modern_horizons.c:120. */
int card_arcums_astrolabe(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_MODIFY_COST)
  {

    iVar1 = get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, 0);
    iVar2 = can_sorcery_be_played(player, EVENT_MODIFY_COST);
    if ((iVar2 == 0) || (iVar1 = has_snow_mana(player, 1, iVar1), iVar1 == 0))
    {
      infinite_casting_cost();
    }
    else
    {
      null_casting_cost(player, card);
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      iVar1 = played_for_free(player, card);
      if ((iVar1 != 0) || (iVar1 = is_token(player, card), iVar1 != 0))
      {
        return 1;
      }
      iVar1 = get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, 0);
      iVar1 = charge_snow_mana(player, card, 1, iVar1);
      if (iVar1 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
    }
  }
  else if (event == EVENT_ETB)
  {
    draw_a_card(player);
  }
  iVar1 = card_viridian_acolyte(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x23370c0; original cards/modern_horizons.c:520. */
int card_cleaving_sliver(int player, int card, event_t event)
{
  int result;

  boost_creature_type(player, card, event, SUBTYPE_SLIVER, 2, 0, 0,
                      BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  result = slivercycling(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2337120; original cards/modern_horizons.c:531. */
int card_cloudshredder_sliver(int player, int card, event_t event)
{
  int result;

  boost_subtype(player, card, event, SUBTYPE_SLIVER, 0, 0, 0x20, 4,
                BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  result = slivercycling(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x233aaa0; original cards/modern_horizons.c:2166. */
int card_lancer_sliver(int player, int card, event_t event)
{
  int result;

  boost_subtype(player, card, event, SUBTYPE_SLIVER, 0, 0, 0x100, 0,
                BCT_INCLUDE_SELF | BCT_CONTROLLER_ONLY);
  result = slivercycling(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x233acb0; original cards/modern_horizons.c:2219. */
int card_lightning_skelemental(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  card_instance_t *instance;

  result = damage_dealt_by_me(player, card, event,
                              DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                  DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result != 0)
  {

    instance = get_card_instance(player, card);
    uVar1 = instance->targets[1].player;
    instance->targets[1].player = 0;
    new_multidiscard(0, (uVar1 & 0xff) * 2, 0, player);
    new_multidiscard(1, (uVar1 >> 8 & 0xff) * 2, 0, player);
  }
  result = card_ball_lightning(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23351f0; original cards/modern_horizons.c:669. */
int undying_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int player_00;
  int iVar3;

  instance = get_card_instance(player, card);

  if ((instance->damage_target_player < '\0') ||
      (instance = get_card_instance(player, card), event != EVENT_GRAVEYARD_FROM_PLAY))
  {
    iVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
      goto joined_r0x023352e8;
  }
  else
  {
    iVar2 = instance->damage_target_card;
    if ((((iVar2 == affected_card) && (player_00 = (int)instance->damage_target_player,
                                       player_00 == affected_card_controller)) &&
         (iVar3 = is_token(player_00, iVar2), iVar3 == 0)) &&
        (((iVar3 = count_counters(player_00, iVar2, COUNTER_P1_P1),
           iVar3 < 1 && (iVar3 = check_for_death_trigger_removal(player_00, iVar2), iVar3 == 0)) &&
          (iVar3 = is_this_dying(player_00, iVar2), iVar3 != 0))))
    {
      iVar3 = get_owner(player_00, iVar2);
      instance->targets[0].player = iVar3;
      iVar2 = get_original_internal_card_id(player_00, iVar2);
      instance->targets[0xb].player = 1;
      instance->damage_target_card = -1;
      instance->targets[0].card = iVar2;
      instance->damage_target_player = -1;
    }
    iVar2 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (iVar2 == 0)
    {
      return 0;
    }
  }

  instance = get_card_instance(player, card);
  iVar2 = instance->targets[0].player;
  seek_grave_for_id_to_reanimate(iVar2, -1, iVar2,
                                 (unsigned int)cards_data[instance->targets[0].card].id,
                                 REANIMATE_PLUS1_PLUS1_COUNTER);
  kill_card(player, card, KILL_EXILE);
joined_r0x023352e8:
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x2338170; original cards/modern_horizons.c:1022. */
int card_first_slivers_chosen(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int card_00;
  int iStack_14;

  store_attackers(player, card, event, DAT_ATTACKS_ALONE | DAT_TRACK, player, -1, NULL);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 1);
  if (event == EVENT_DAT_ABILITY)
  {

    instance = get_card_instance(player, card);
    card_00 = (unsigned int)(unsigned char)instance->targets[2].player;
    instance = in_play(player, card_00);

    if ((instance != NULL) && (0 < active_cards_count[player]))
    {
      iStack_14 = 0;
      iVar1 = 0;
      do
      {
        instance = in_play(player, iVar1);
        if (instance != NULL)
        {
          iVar3 = is_what(player, iVar1, 2);
          if (iVar3 != 0)
          {
            iVar3 = has_subtype(player, iVar1, SUBTYPE_SLIVER);
            if (iVar3 != 0)
            {
              iVar3 = is_humiliated(player, iVar1, AL_TRIG);
              iStack_14 = iStack_14 + (unsigned int)(iVar3 == 0);
            }
          }
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player]);
      if (0 < iStack_14)
      {
        pump_until_eot(player, card, player, card_00, iStack_14, iStack_14);
      }
    }
  }
  iVar1 = slivercycling(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x233a3c0; original cards/modern_horizons.c:1953. */
int card_ice_fang_coatl(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iStack_18;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_a_card(player);
  }
  else
  {

    if ((((event == EVENT_ABILITIES) && (card == affected_card)) &&
         (player == affected_card_controller)) &&
        (iVar1 = active_cards_count[player] + -1, -1 < iVar1))
    {
      iStack_18 = 0;
      do
      {
        instance = in_play(player, iVar1);
        if ((card != iVar1) && (instance != NULL))
        {
          iVar3 = is_snow_permanent(player, iVar1);
          iStack_18 = (iStack_18 + 1) - (unsigned int)(iVar3 == 0);
        }
        iVar1 = iVar1 + -1;
        if (iVar1 < 0)
        {
          if (iStack_18 < 3)
            goto LAB_0233a3fb;
          break;
        }
      } while (iStack_18 < 3);
      deathtouch(player, card, EVENT_ABILITIES);
    }
  }
LAB_0233a3fb:
  iVar1 = flash(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x233c030; original cards/modern_horizons.c:2954. */
int card_serra_the_benevolent(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  token_generation_t token;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_test_definition(&test, 2);
    test.keyword = 0x20;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = check_battlefield_for_id(player, 0x4ee9);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Pump flyers", 1,
                           5, 2, "Angel", 1, 10, 0xfffffffd, "Emblem", 1,
                           (-(unsigned int)(result == 0) & 0x28) - 0x14, 0xfffffffa, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      goto LAB_0233c0ab;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_0233c0ab;
    default_test_definition(&test, 2);
    test.keyword = 0x20;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    result = instance->info_slot;
    if (result == 1)
    {
      pump_creatures_until_eot_merge_pt(player, card, player, 1, 1, &test);
    }
    else if (result == 2)
    {
      default_token_definition(player, card, 0x48ff, &token);
      token.s_key_plus = 8;
      generate_token(&token);
    }
    else if (result == 3)
    {
      generate_token_by_id(player, card, 0x4ee9);
    }
  }
LAB_0233c0ab:
  result = planeswalker(player, card, event, 4);
  return result;
}

/* ManalinkEh 2022: 0x233c290; original cards/modern_horizons.c:3017. */
int card_serra_the_benevolent_emblem(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_DAMAGE_REDUCTION)
  {

    if (((((event_result < 1) &&
           (instance = get_card_instance(affected_card_controller, affected_card),
            instance != NULL)) &&
          (result = (int)instance->damage_target_player, result == player)) &&
         ((instance->damage_target_card == -1 && (0 < creature_count[result])))) &&
        (event_result = life[result], 0 < event_result))
    {
      event_result = 1;
    }
  }
  else if (((event == EVENT_SHOULD_AI_PLAY) &&
            (instance = in_play(player, card), instance != NULL)) &&
           (0 < creature_count[player]))
  {
    if (player == 1)
    {
      ai_modifier = ai_modifier + 0x1e0;
    }
    else
    {
      ai_modifier = ai_modifier + -0x1e0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x233c4e0; original cards/modern_horizons.c:3104. */
int card_sisay_weatherlight_captain(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  unsigned int uVar5;
  bool bVar6;
  unsigned int uStack_13c;
  char buffer[75];
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  if (event - EVENT_POWER < 2)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (iVar2 = is_humiliated(player, card, AL_STATIC), iVar1 = current_turn,
         iVar2 == 0))
    {

      iVar2 = active_cards_count[current_turn] + -1;
      if (iVar2 < 0)
      {
        uStack_13c = 0;
      }
      else
      {
        bVar6 = player == current_turn;
        uStack_13c = 0;
        do
        {
          if (((card != iVar2) && (bVar6)) &&
              ((instance = in_play(iVar1, iVar2),
                instance != NULL &&
                    ((iVar4 = is_what(iVar1, iVar2, 0x47),
                      iVar4 != 0 && (iVar4 = is_legendary(iVar1, iVar2), iVar4 != 0))))))
          {
            uVar5 = get_color(iVar1, iVar2);
            uStack_13c = uStack_13c | uVar5 & 0x3e;
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
          if ((((card != iVar2) && (player == iVar1)) &&
               (instance = in_play(iVar1, iVar2), instance != NULL)) &&
              ((iVar4 = is_what(iVar1, iVar2, 0x47),
                iVar4 != 0 && (iVar4 = is_legendary(iVar1, iVar2), iVar4 != 0))))
          {
            uVar5 = get_color(iVar1, iVar2);
            uStack_13c = uStack_13c | uVar5 & 0x3e;
          }
          iVar2 = iVar2 + -1;
        } while (iVar2 != -1);
      }
      uVar5 = num_bits_set(uStack_13c);
      event_result = event_result + uVar5;
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_0233c51b:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_0233c520;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_0233c51b;
  }
  iVar1 = get_power(player, card);
  scnprintf(buffer, 0x4b, "Select a legendary permanent card with converted mana cost %d or less",
            iVar1);
  new_default_test_definition(&this_test, 0x47, buffer);
  this_test.subtype = 0x2001;
  iVar1 = get_power(player, card);
  this_test.cmc = iVar1 + 1;
  this_test.cmc_flag = 3;
  new_global_tutor(player, player, 1, 4, 0, 1, &this_test);
LAB_0233c520:
  iVar1 = generic_activated_ability(player, card, event, 0x100000, 0, 1, 1, 1, 1, 1, 0, NULL, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x233d790; original cards/modern_horizons.c:3692. */
int card_urza_lord_high_artificer(int player, int card, event_t event)
{
  unsigned short uVar1;
  int iVar2;
  int iVar3;
  card_instance_t *instance;
  token_generation_t token;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x4918, &token);
    token.tou = 0;
    token.pow = 0;
    token.special_infos = 0x42;
    generate_token(&token);
  }
  else
  {
    if (event == EVENT_COUNT_MANA)
    {
      iVar2 = tap_a_permanent_you_control_for_mana(player, card, EVENT_COUNT_MANA, TYPE_ARTIFACT, 0,
                                                   COLOR_BLUE, 1);
      return iVar2;
    }
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = tap_a_permanent_you_control_for_mana(player, card, EVENT_CAN_ACTIVATE, TYPE_ARTIFACT,
                                                   0, COLOR_BLUE, 1);
      if (iVar2 == 0)
      {
        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 5, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        return iVar2;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance = get_card_instance(player, card);
      instance->info_slot = 0;
      iVar2 = tap_a_permanent_you_control_for_mana(player, card, EVENT_CAN_ACTIVATE, TYPE_ARTIFACT,
                                                   0, COLOR_BLUE, 1);
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 5, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016",
                          "Tap an artifact for mana", iVar2, 1, "Cast from library", iVar3, 5, 0, 0,
                          0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 != 1)
      {
        if (iVar2 != 2)
        {
          return 0;
        }
        iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 5, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        return iVar2;
      }
      iVar2 = tap_a_permanent_you_control_for_mana(player, card, EVENT_ACTIVATE, TYPE_ARTIFACT, 0,
                                                   COLOR_BLUE, 1);
      return iVar2;
    }
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) &&
      (instance = get_card_instance(player, card), instance->info_slot == 2))
  {

    shuffle(player);
    if (*deck_ptr[player] != -1)
    {

      uVar1 = cards_data[*deck_ptr[player]].id;
      rfg_card_in_deck(player, 0);
      create_may_play_card_from_exile_effect(player, card, player, (unsigned int)uVar1,
                                             MPCFE_UNTIL_EOT | MPCFE_FOR_FREE);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2337540; original cards/modern_horizons.c:700. */
int card_endling(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int iVar3;
  unsigned int uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iStack_1c;
  uint32_t uStack_14;

  if (event - EVENT_POW_BOOST < 2)
  {
    if (current_turn == player)
    {
      iVar8 = -1;
      iVar7 = 1;
    }
    else
    {
      iVar8 = 1;
      iVar7 = -1;
    }
    iVar7 = generic_shade(player, card, event, 0, 1, 0, 0, 0, 0, 0, iVar7, iVar8, 0, 0);
    return iVar7;
  }
  if (event == EVENT_CLEANUP)
  {

    instance = get_card_instance(player, card);
    instance->targets[1].card = 0;
    instance->targets[1].player = 0;
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      instance = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar7 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 1, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        return iVar7;
      }
      if (event != EVENT_ACTIVATE)
        goto LAB_023375b9;

      iVar7 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 0, 1, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar8 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 1, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      uVar2 = get_special_abilities(player, card);
      uVar4 = instance->targets[1].card;
      uVar2 = uVar2 | uVar4 & (int)(uVar4 ^ 0xffffffff) >> 0x1f;
      if ((uVar2 & 0x10000) == 0)
      {
        if (0x14 < current_phase)
        {
          iStack_1c = 1;
          goto LAB_023377ea;
        }
        if (current_turn == player)
        {
          iVar3 = is_sick(player, card);
          iStack_1c = (-(unsigned int)(iVar3 == 0) & 9) + 1;
          goto LAB_023377ea;
        }
        iStack_1c = 1;
        if ((uVar2 & 0x200) == 0)
        {
          uStack_14 = 10;
          goto LAB_02337805;
        }
      }
      else
      {
        iStack_1c = 1;
      LAB_023377ea:
        if (((uVar2 & 0x200) == 0) && (uStack_14 = 10, current_phase < 0x19))
          goto LAB_02337805;
      }
      uStack_14 = 1;
    LAB_02337805:
      iVar3 = is_this_dying(player, card);
      if (current_turn == player)
      {
        iVar6 = 1;
        iVar5 = ((current_phase == 0x18) - 1 & 0xfffffff9) + 8;
      }
      else
      {
        iVar5 = 1;
        iVar6 = ((current_phase == 0x18) - 1 & 0xfffffff9) + 8;
      }
      iVar7 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Menace", iVar7, iStack_1c,
                          "Deathtouch", iVar7, uStack_14, "Undying", iVar7,
                          (-(unsigned int)(iVar3 == 0) & 0xfffffff7) + 10, "+1/-1", iVar8, iVar5,
                          "-1/+1", iVar8, iVar6, 0, 0, 0, 0);
      if (iVar7 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar7 - 1U < 3)
      {
        iVar8 = charge_mana_for_activated_ability(player, card, 0, 1, 0, 0, 0, 0);
        if (iVar8 == 0)
        {
          return 0;
        }
        uVar4 = instance->targets[1].card;
        if ((int)uVar4 < -1)
        {
          uVar4 = 0;
        }
        if (iVar7 == 1)
        {
          uVar4 = uVar4 | 0x10000;
        }
        else if (iVar7 == 2)
        {
          uVar4 = uVar4 | 0x200;
        }
        instance->targets[1].card = uVar4;
        return 0;
      }
      if (1 < iVar7 - 4U)
      {
        return 0;
      }
      charge_mana_for_activated_ability(player, card, 1, 0, 0, 0, 0, 0);
      return 0;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      return 0;
    }
  }
  instance = get_card_instance(player, card);
LAB_023375b9:
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }

  iVar7 = instance->info_slot;
  if (iVar7 == 1)
  {
    iVar7 = 0x10000;
  }
  else
  {
    if (iVar7 != 2)
    {
      if (iVar7 == 3)
      {
        iVar7 = create_targetted_legacy_effect(player, card, undying_legacy,
                                               instance->parent_controller, instance->parent_card);
        alternate_legacy_text(1, player, iVar7);
        return 0;
      }
      if (iVar7 == 4)
      {
        iVar8 = -1;
        iVar7 = 1;
      }
      else
      {
        if (iVar7 != 5)
        {
          return 0;
        }
        iVar8 = 1;
        iVar7 = -1;
      }
      pump_until_eot_merge_previous_alt(player, card, instance->parent_controller,
                                        instance->parent_card, iVar7, iVar8, 2);
      return 0;
    }
    iVar7 = 0x200;
  }
  iVar7 = pump_ability_until_eot(player, card, instance->parent_controller, instance->parent_card,
                                 0, 0, 0, iVar7);
  alternate_legacy_text(1, player, iVar7);
  return 0;
}

/* ManalinkEh 2022: 0x233e340; original cards/modern_horizons.c:4000. */
int card_wrenn_and_six(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iStack_148;
  uint32_t uStack_144;
  target_definition_t *ptStack_140;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    new_default_test_definition(&test, 1, "Select target land card.");
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      if ((player == 1) &&
          (iVar4 = count_counters(1, card, COUNTER_LOYALTY), iVar2 = current_turn, 1 < iVar4))
      {

        iStack_148 = current_turn;
        iVar4 = active_cards_count[current_turn] + -1;
        if (iVar4 < 0)
        {
          uStack_144 = 1;
        }
        else
        {
          do
          {
            if (((iVar2 != 1) && (instance = in_play(iVar2, iVar4), instance != NULL)) &&
                (iVar3 = is_what(iVar2, iVar4, 2), iVar3 != 0))
            {
              iVar3 = get_toughness(iVar2, iVar4);
              instance = get_card_instance(iVar2, iVar4);
              if (iVar3 - (unsigned int)instance->damage_on_card == 1)
              {
                iStack_148 = current_turn;
                uStack_144 = 0x19;
                goto LAB_0233e708;
              }
            }
            iVar4 = iVar4 + -1;
          } while (iVar4 != -1);
          iStack_148 = current_turn;
          uStack_144 = 1;
        }
      LAB_0233e708:
        iVar2 = 1 - iStack_148;
        iVar4 = active_cards_count[iVar2] + -1;
        current_turn = iStack_148;
        if (-1 < iVar4)
        {
          do
          {
            if (((iVar2 != 1) && (instance = in_play(iVar2, iVar4), instance != NULL)) &&
                (iVar3 = is_what(iVar2, iVar4, 2), iVar3 != 0))
            {
              iVar3 = get_toughness(iVar2, iVar4);
              instance = get_card_instance(iVar2, iVar4);
              if (iVar3 - (unsigned int)instance->damage_on_card == 1)
              {
                uStack_144 = 0x19;
                break;
              }
            }
            iVar4 = iVar4 + -1;
          } while (iVar4 != -1);
        }
      }
      else
      {
        uStack_144 = 1;
      }
      iVar2 = check_battlefield_for_id(player, 0x4f2a);
      iVar4 = -100;
      if (iVar2 == 0)
      {
        iVar2 = count_counters(player, card, COUNTER_LOYALTY);
        iVar4 = ((7 < iVar2) - 1 & 0xffffffec) + 0x1e;
      }
      iVar2 = can_target(&td);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017",
                          "Return land from grave", 1, 10, 1, "Deal 1 damage", iVar2, uStack_144,
                          0xffffffff, "Emblem", 1, iVar4, 0xfffffff9, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 == 1)
      {
        iVar2 = new_special_count_grave(player, &test);
        if ((iVar2 != 0) && (iVar2 = graveyard_has_shroud(player), iVar2 == 0))
        {
          select_target_from_grave_source(player, card, player, SFG_0, 1, &test, 0);
        }
      }
      else if (iVar2 == 2)
      {
        pick_target(&td, "TARGET_ANY");
      }
      goto LAB_0233e3e8;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_0233e3e8;
    new_default_test_definition(&test, 1, "Select target land card.");
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
  }
  ptStack_140 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar2 = instance->info_slot;
    if (iVar2 == 1)
    {

      iVar2 = validate_target_from_grave_source(player, card, player, 0);
      if (iVar2 != -1)
      {
        from_grave_to_hand(player, iVar2, 1);
      }
    }
    else if (iVar2 == 2)
    {
      iVar2 = valid_target(ptStack_140);
      if (iVar2 != 0)
      {
        damage_target0(player, card, 1);
      }
    }
    else if (iVar2 == 3)
    {
      generate_token_by_id(player, card, 0x4f2a);
    }
  }
LAB_0233e3e8:
  iVar2 = planeswalker(player, card, event, 3);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int tempered_sliver_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x233d080; original cards/modern_horizons.c:3390. */
int card_sword_of_truth_and_justice(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  int result;
  card_instance_t *instance;
  int iStack_68;
  target_definition_t td;

  iStack_68 = equipped_creature_deals_damage(player, card, event,
                                             DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (iStack_68 != 0)
  {

    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    if (0 < iStack_68)
    {
      do
      {
        instance->number_of_targets = '\0';
        result = can_target(&td);
        if ((result != 0) && (result = pick_target(&td, "ASHNODS_BATTLEGEAR"), result != 0))
        {
          add_counters(player, card, instance->targets[0].player,
                                   instance->targets[0].card, COUNTER_P1_P1, 1);
        }
        proliferate(player, card);
        iStack_68 = iStack_68 + -1;
      } while (iStack_68 != 0);
    }
  }
  uVar1 = get_sleighted_protection(player, card, 0x1000);
  uVar2 = get_sleighted_protection(player, card, 0x8000);
  result = vanilla_equipment(player, card, event, 2, 2, 2, uVar1 | uVar2, 0);
  return result;
}

/* ManalinkEh 2022: 0x2335650; original cards/modern_horizons.c:3488. */
int tempered_sliver_legacy(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  int player_00;

  if (((trigger_condition == 0xd7) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      player_00 = (int)instance->damage_target_player;
      result = instance->damage_target_card;
      instance = in_play(player_00, result);
      if ((instance != NULL) && (reason_for_trigger_controller == player_00))
      {
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
        }
        else if (event == EVENT_RESOLVE_TRIGGER)
        {
          add_counters(player_00, result, player_00, result, COUNTER_P1_P1, 1);
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
  }
  result = eot_trigger(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x233d3a0; original cards/modern_horizons.c:3506. */
int card_tempered_sliver(int player, int card, event_t event)
{
  int card_00;
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  if (event == EVENT_DEAL_DAMAGE)
  {

    instance = combat_damage_being_dealt(EVENT_DEAL_DAMAGE);
    if ((((instance != NULL) && (iVar1 = (int)instance->damage_source_player, iVar1 == player)) &&
         (card_00 = instance->damage_source_card, card_00 != -1)) &&
        ((0 < instance->info_slot && (instance->damage_target_card == -1))))
    {
      iVar3 = check_special_flags(iVar1, card_00, 0x40);
      if (iVar3 == 0)
      {
        iVar3 = is_what(iVar1, card_00, 2);
        if (iVar3 != 0)
        {
          iVar3 = has_subtype(iVar1, card_00, SUBTYPE_SLIVER);
          if (iVar3 != 0)
          {
            iVar3 = is_humiliated(iVar1, card, AL_STATIC);
            if (iVar3 == 0)
            {
              iVar3 = is_humiliated(iVar1, card_00, AL_TRIG);
              if (iVar3 == 0)
              {
                create_targetted_legacy_effect(iVar1, card_00, tempered_sliver_legacy, iVar1,
                                               card_00);
              }
            }
          }
        }
      }
    }
  }
  iVar1 = slivercycling(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x233ebe0; original cards/modern_horizons.c:4161. */
int card_yawgmoth_thran_physician(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t td;
  test_definition_t test;

  check_legend_rule(player, card, event);
  protection_from_subtype(player, card, event, SUBTYPE_HUMAN);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    new_default_test_definition(&test, 2, "Select creature to sacrifice");
    test.not_me = 1;
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_CAN_ACTIVATE,
                                                               GAA_NONE, 0, 0, 0, 0, 0, 0, 1, NULL,
                                                               NULL, TYPE_NONE, &test, SARV_0);
      if (iVar2 == 0)
      {
        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x20, 0, 2, 0, 0, 0, 0,
                                          0, NULL, NULL);
        return iVar2;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->info_slot = 0;
      iVar2 = generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_CAN_ACTIVATE,
                                                               GAA_NONE, 0, 0, 0, 0, 0, 0, 1, NULL,
                                                               NULL, TYPE_NONE, &test, SARV_0);
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x20, 0, 2, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      iVar2 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016",
                          "-1/-1 counter & draw", iVar2, 1, "Proliferate", iVar3, 1, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 != 1)
      {
        if (iVar2 != 2)
        {
          return 0;
        }
        iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x20, 0, 2, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        return iVar2;
      }
      generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_ACTIVATE, GAA_NONE, 0, 0,
                                                       0, 0, 0, 0, 1, NULL, NULL, TYPE_NONE, &test,
                                                       SARV_0);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      iVar2 = can_target(&td);
      if (iVar2 == 0)
      {
        return 0;
      }
      new_pick_target(&td, "TARGET_CREATURE", 0, 0);
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 2, "Select creature to sacrifice");
    test.not_me = 1;
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 1;
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (instance->info_slot == 1)
    {
      iVar2 = instance->targets[0].card;
      if (iVar2 != -1)
      {
        add_counters(player, card, instance->targets[0].player, iVar2, COUNTER_M1_M1,
                                 1);
      }
      draw_a_card(player);
    }
    else if (instance->info_slot == 2)
    {
      proliferate(player, card);
      return 0;
    }
  }

  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2335c60; original cards/modern_horizons.c:31. */
int card_archmages_charm(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  target_definition_t td2;
  target_definition_t td3;

  if (event == EVENT_CAN_COUNTER)
  {
    iVar2 = has_mana(player, 2, 3);
    if (iVar2 != 0)
    {
      ai_modifier = ai_modifier + 0x18;
    }
  }
  else
  {
    if (event == EVENT_CAN_CAST)
    {
      get_card_instance(player, card);
      default_target_definition(player, card, &td2, 0);
      td2.zone = 0x1000;
      default_target_definition(player, card, &td3, 0x47);
      td3.special = 0x800;
      td3.extra = 1;
      td3.illegal_type = 1;
      iVar2 = counterspell(player, card, EVENT_CAN_CAST, NULL, 0);
      if (iVar2 != 0)
      {
        return 99;
      }
      iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td2, NULL, 1, NULL);
      if (iVar2 != 0)
      {
        return 1;
      }
      iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td3, NULL, 1, NULL);
      return iVar2;
    }
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance = get_card_instance(player, card);
        default_target_definition(player, card, &td2, 0);
        td2.zone = 0x1000;
        default_target_definition(player, card, &td3, 0x47);
        td3.special = 0x800;
        td3.extra = 1;
        td3.illegal_type = 1;
        if ((card == affected_card) && (player == affected_card_controller))
        {
          instance->number_of_targets = '\0';
          iVar2 = check_special_flags2(player, card, -0x80000000);
          if (iVar2 == 0)
          {

            iVar2 = counterspell(player, card, EVENT_CAN_CAST, NULL, 0);
            iVar3 = can_target(&td2);
            iVar4 = can_target(&td3);
            iVar2 =
                dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "Counter a spell", iVar2,
                            1000000, "Draw", iVar3, 10, "Steal a permanent", iVar4, 10, 0, 0, 0, 0);
            if (iVar2 == 0)
            {
              spell_fizzled = 1;
              return 0;
            }
          }
          iVar2 = instance->info_slot;
          if (iVar2 == 1)
          {
            iVar2 = counterspell(player, card, EVENT_CAST_SPELL, NULL, 0);
            return iVar2;
          }
          if (iVar2 == 2)
          {
            iVar2 =
                generic_spell(player, card, EVENT_CAST_SPELL, 1, &td2, "TARGET_PLAYER", 1, NULL);
            return iVar2;
          }
          if (iVar2 == 3)
          {
            iVar2 = generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td3,
                                  "Select target nonland permanent with CMC 1 or less.", 1, NULL);
            return iVar2;
          }
        }
      }
    }
    else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      instance = get_card_instance(player, card);
      default_target_definition(player, card, &td2, 0);
      td2.zone = 0x1000;
      default_target_definition(player, card, &td3, 0x47);
      td3.special = 0x800;
      td3.extra = 1;
      td3.illegal_type = 1;
      if (event == EVENT_RESOLVE_SPELL)
      {
        iVar2 = instance->info_slot;
        if (iVar2 == 1)
        {
          iVar2 = counterspell(player, card, EVENT_RESOLVE_SPELL, NULL, 0);
          return iVar2;
        }
        if (iVar2 == 2)
        {
          iVar2 = valid_target(&td2);
          if (iVar2 != 0)
          {
            draw_cards(instance->targets[0].player, 2);
          }
          iVar2 = instance->info_slot;
        }
        if ((iVar2 == 3) && (iVar2 = valid_target(&td3), iVar2 != 0))
        {
          gain_control_permanently(player, instance->targets[0].player, instance->targets[0].card);
        }
        kill_card(player, card, KILL_DESTROY);
        return 0;
      }
    }
    else if (event == EVENT_CHANGE_TARGET)
    {
      get_card_instance(player, card);
      default_target_definition(player, card, &td2, 0);
      td2.zone = 0x1000;
      default_target_definition(player, card, &td3, 0x47);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2336570; original cards/modern_horizons.c:186. */
int card_astral_drift(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int argument7;
  target_definition_t td;

  if (1 < event - EVENT_CAN_ACTIVATE_FROM_HAND)
  {
    if (((event == EVENT_RESOLVE_ACTIVATION_FROM_HAND) && (spell_fizzled == 0)) &&
        (((((unsigned int)player_bits[player] >> 24 & 0xff) & 2) == 0 || (affected_card != card))))
    {

      draw_cards(player, 1);
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = 1 - player;
      td.allow_cancel = 0;
      argument7 = 0;
      if (player == 1)
      {
        td.allowed_controller = 0;
        result = can_target(&td);
        td.allowed_controller = 2;
        argument7 = (unsigned int)(result == 0);
      }
      result = can_target(&td);
      if (result != 0)
      {

        result =
            do_dialog(player, player, card, -1, -1, " Activate Astral Drift\n Pass", argument7);
        if (result == 0)
        {
          result = pick_target(&td, "TARGET_CREATURE");
          if (result != 0)
          {

            instance = get_card_instance(player, card);
            result = instance->targets[0].card;
            instance->number_of_targets = '\x01';
            remove_until_eot(player, card, instance->targets[0].player, result);
          }
        }
      }
    }
    result = global_enchantment(player, card, event);
    return result;
  }
  result = cycling(player, card, event, 2, 0, 0, 0, 0, 1);
  return result;
}

/* ManalinkEh 2022: 0x2338ba0; original cards/modern_horizons.c:1284. */
int card_geomancers_gambit(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
  LAB_02338c4c:
    default_target_definition(player, card, &td, 1);
    if (player != 1)
      goto LAB_02338c00;
  LAB_02338c6f:
    td.required_subtype = 0x2000;
    td.special = 0x1000;
  }
  else
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
      goto LAB_02338c4c;
    }
    if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      goto LAB_02338c4c;
    }
    default_target_definition(player, card, &td, 1);
    if (player == 1)
      goto LAB_02338c6f;
  }
  if (event != EVENT_RESOLVE_SPELL)
    goto LAB_02338c00;
  result = valid_target(&td);
  if (result != 0)
  {

    instance = get_card_instance(player, card);
    kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    result = instance->targets[0].player;
    if (result == 0)
    {
      result = do_dialog(0, player, card, -1, -1, " Tutor a basic land\n Pass", 0);
      if (result == 1)
        goto LAB_02338c92;
      result = instance->targets[0].player;
    }
    tutor_basic_land(result, 1, 0);
  }
LAB_02338c92:
  kill_card(player, card, KILL_DESTROY);
LAB_02338c00:
  result = generic_spell(player, card, event, 1, &td, "TARGET_LAND", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x233a5c0; original cards/modern_horizons.c:2035. */
int card_kayas_guile(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  unsigned int uVar3;
  int iVar4;
  int *piVar5;
  uint32_t uVar6;
  uint32_t uVar7;
  uint32_t uVar8;
  uint32_t uVar9;
  uint32_t uStack_f4;
  uint32_t uStack_f0;
  int iStack_ec;
  union
  {
    int priority[6];
    test_definition_t this_test;
    token_generation_t token;
  } storage;

  if (event == EVENT_CAN_CAST)
  {
    return 1;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
      {
        storage.priority[1] = 10;
        storage.priority[2] = 6;
        iVar4 = count_permanents_by_type(1 - player, TYPE_CREATURE);
        iVar1 = count_permanents_by_type(player, TYPE_CREATURE);
        uVar6 = 0x14;
        storage.priority[5] = 0x14;
        uStack_f4 = 10;
        uStack_f0 = 6;
        iStack_ec = (iVar4 - iVar1) * 3;
        iVar4 = 0xf - life[player];
        storage.priority[3] = iStack_ec;
        storage.priority[4] = iVar4;
      }
      else
      {
        piVar5 = storage.priority;
        for (iVar4 = 6; iVar4 != 0; iVar4 = iVar4 + -1)
        {
          *piVar5 = 0;
          piVar5 = piVar5 + 1;
        }
        uStack_f4 = 0;
        uStack_f0 = 0;
        iStack_ec = 0;
        iVar4 = 0;
        uVar6 = 0;
      }
      iVar1 = has_mana(player, 7, 3);
      uVar9 = 0;
      uVar8 = 0;
      uVar7 = 0;
      iVar1 =
          dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014", "Opponent sacrifices",
                      1, uStack_f4, "Exile graveyard", 1, uStack_f0, "Create a Spirit", 1,
                      iStack_ec, "Gain 4 life", 1, iVar4, "Entwine", iVar1, uVar6, 0, 0, 0, 0);
      if (iVar1 == 5)
      {
        charge_mana(player, COLOR_ANY, 3);
        if (spell_fizzled == 1)
        {
          return 0;
        }
        instance = get_card_instance(player, card);
        uVar3 = 0x1e;
      }
      else
      {
        if ((iVar1 == 0) ||
            (iVar4 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\014",
                                 "Opponent sacrifices", (unsigned int)(iVar1 != 1), uStack_f4,
                                 "Exile graveyard", (unsigned int)(iVar1 != 2), uStack_f0,
                                 "Create a Spirit", (unsigned int)(iVar1 != 3), iStack_ec,
                                 "Gain 4 life", (unsigned int)(iVar1 != 4), iVar4, 0, 0, 0, 0,
                                 uVar7, uVar8, uVar9),
             iVar4 == 0))
        {
          spell_fizzled = 1;
          return 0;
        }
        instance = get_card_instance(player, card);
        uVar3 = 1 << ((unsigned char)iVar1 & 0x1f) | 1 << ((unsigned char)iVar4 & 0x1f);
      }
      instance->info_slot = uVar3;
    }
    return 0;
  }
  if ((event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET) &&
      (event != EVENT_CHANGE_TARGET))
  {
    return 0;
  }
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }

  iVar4 = 2;
LAB_0233a8f2:
  while (true)
  {
    iVar1 = iVar4 + -1;
    instance = get_card_instance(player, card);
    if ((instance->info_slot >> ((unsigned char)iVar1 & 0x1f) & 1U) == 0)
      goto LAB_0233a940;
    if (iVar1 != 3)
      break;
    default_token_definition(player, card, 0x498d, &storage.token);
    iVar4 = iVar4 + 1;
    generate_token(&storage.token);
  }
  if (iVar1 < 4)
  {
    if (iVar1 == 1)
    {
      new_default_test_definition(&storage.this_test, 2,
                                  "Select a creature to sacrifice.");
      new_sacrifice(player, card, 1 - player, SAC_CAUSED | SAC_NO_CANCEL,
                    &storage.this_test);
      iVar4 = iVar4 + 1;
      goto LAB_0233a8f2;
    }
    if (iVar1 != 2)
      goto LAB_0233a940;
    rfg_whole_graveyard(1 - player);
  }
  else
  {
    if (iVar1 == 4)
    {
      gain_life(player, 4);
    LAB_0233a945:
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  LAB_0233a940:
    if (4 < iVar4)
      goto LAB_0233a945;
  }
  iVar4 = iVar4 + 1;
  goto LAB_0233a8f2;
}

/* ManalinkEh 2022: 0x233b6f0; original cards/modern_horizons.c:2590. */
int card_pondering_mage(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  unsigned int argument7;
  int iVar3;
  int iStack_ec;
  test_definition_t test;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }

  if ((ai_is_speculating == 1) ||
      ((argument7 = 0, player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {

    iVar1 = count_deck(player);
    iStack_ec = iVar1;
    if (3 < iVar1)
    {
      iStack_ec = 3;
    }
    if (iVar1 < 1)
    {
      count_subtype(player, 1, -1);
      argument7 = 0;
      default_test_definition(&test, 1);
      test.zone = 0x100;
      check_battlefield_for_special_card(player, card, player, 4, &test);
    }
    else
    {
      iVar3 = 0;
      iVar1 = 0;
      do
      {
        iVar2 = is_what(-1, deck_ptr[player][iVar3], 1);
        iVar1 = (iVar1 + 1) - (unsigned int)(iVar2 == 0);
        iVar3 = iVar3 + 1;
      } while (iVar3 < iStack_ec);
      iVar3 = count_subtype(player, 1, -1);
      default_test_definition(&test, 1);
      test.zone = 0x100;
      iVar2 = check_battlefield_for_special_card(player, card, player, 4, &test);
      if ((iVar1 < 2) || (argument7 = 1, iVar2 + iVar3 < 6))
      {
        argument7 = (unsigned int)(iVar2 + iVar3 < 3 && iVar1 != 0);
      }
    }
  }
  rearrange_top_x(player, player, 3);
  iVar1 = do_dialog(player, player, card, -1, -1, " Shuffle\n Pass", argument7);
  if (iVar1 == 0)
  {
    shuffle(player);
  }
  draw_cards(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x233d1e0; original cards/modern_horizons.c:3456. */
int card_tectonic_reformation(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  unsigned int amount;
  test_definition_t this_test;

  if (event - EVENT_CAN_ACTIVATE_FROM_HAND < 3)
  {
    iVar1 = us_cycling(player, card, event);
    return iVar1;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {

    iVar1 = check_battlefield_for_id(player, 0x4252);
    if ((iVar1 == 0) || (hand_count[player] < 7))
    {
      check_hand_for_type(player, card, 1);
    }
    else
    {
      iVar1 = check_hand_for_type(player, card, 1);
      if (iVar1 != 0)
      {
        return 1;
      }
    }
    iVar1 = has_mana(player, 4, 1);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {

      new_default_test_definition(&this_test, 1, "Select a land card to cycle");
      iVar1 = new_select_a_card(player, player, 4, 0, 3, -1, &this_test);
      iVar2 = check_battlefield_for_id(player, 0x4252);
      amount = 1;
      if (iVar2 != 0)
      {
        amount = (unsigned int)(hand_count[player] < 7);
      }
      charge_mana(player, COLOR_RED, amount);
      if (spell_fizzled != 1)
      {
        discard_card(player, iVar1);
      }
    }
    else if ((event == EVENT_RESOLVE_ACTIVATION) && (spell_fizzled != 1))
    {
      draw_a_card(player);
    }
    iVar1 = global_enchantment(player, card, event);
  }
  return iVar1;
}

/* ManalinkEh 2022: 0x233dca0; original cards/modern_horizons.c:3876. */
int card_winding_way(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iStack_14;

  if (event == EVENT_RESOLVE_SPELL)
  {

    iVar1 = has_mana(player, 7, 1);
    iVar1 = dialog_impl(player, card, EVENT_RESOLVE_SPELL, "\001\014", "\001\003", "Creature", 1, 4,
                        "Land", 1, 9 - iVar1, 0, 0, 0, 0);
    if (iVar1 == 0)
    {
      spell_fizzled = 1;
    }
    else
    {

      iVar2 = count_deck(player);
      iStack_14 = iVar2;
      if (4 < iVar2)
      {
        iStack_14 = 4;
      }
      if (iVar2 != 0)
      {

        show_deck(0, deck_ptr[player], iStack_14, "Here are the first 4 card of deck", 0,
                  ENGINE_DONE_LABEL);
        do
        {
          iVar2 = *deck_ptr[player];
          iVar3 = is_what(-1, iVar2, (iVar1 == 1) + 1);
          if (iVar3 == 0)
          {
            mill(player, 1);
          }
          else
          {

            add_card_to_hand(player, iVar2);
            remove_card_from_deck(player, 0);
          }
          iStack_14 = iStack_14 + -1;
        } while (iStack_14 != 0);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x233e8b0; original cards/modern_horizons.c:4088. */
/* DWARF gives positions[500] and test_hand the same stack location in
 * different lexical scopes. Keep them as separate source objects. */
int card_wrenn_and_six_emblem(int player, int card, event_t event)
{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iStack_1164;
  test_definition_t test;
  test_definition_t this_test;
  int iids[500];
  int positions[500];
  test_definition_t test_hand;
  if (event == EVENT_CAN_ACTIVATE)
  {
    default_test_definition(&test_hand, 1);
    test_hand.zone = 0x100;
    iVar2 = check_battlefield_for_special_card(player, card, player, 0, &test_hand);
    if (iVar2 != 0)
    {
      piVar1 = graveyard_ptr_mutable[player];
      iVar2 = count_graveyard(player);
      if (-1 < iVar2)
      {
        iStack_1164 = 0;
        do
        {
          iVar3 = piVar1[iVar2];
          if (((iVar3 != -1) && (iVar4 = is_what(-1, iVar3, 0x38), iVar4 != 0)) &&
              (iVar4 = has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, iVar3), iVar4 != 0))
          {
            iVar3 = can_legally_play_iid_now(player, iVar3, EVENT_CAN_ACTIVATE);
            if (iVar3 == 99)
            {
              return 99;
            }
            if (iVar3 != 0)
            {
              iStack_1164 = iVar3;
            }
          }
          iVar2 = iVar2 + -1;
        } while (iVar2 != -1);
        return iStack_1164;
      }
    }
  }
  else
  {
    if (event != EVENT_ACTIVATE)
    {
      return 0;
    }
    iVar3 = 0;
    piVar1 = graveyard_ptr_mutable[player];
    iVar2 = *piVar1;
    if (iVar2 != -1)
    {
      iVar4 = 0;
      do
      {
        if ((((cards_data[iVar2].type & 0x38) != 0) && ((cards_data[iVar2].type & 2) == 0)) &&
            ((iVar2 = has_mana_to_cast_iid(player, EVENT_ACTIVATE, iVar2),
              iVar2 != 0 && ((iVar2 = is_suspend_only_card(-1, piVar1[iVar4]),
                              iVar2 == 0 && (iVar2 = can_legally_play_iid_now(player, piVar1[iVar4],
                                                                              EVENT_ACTIVATE),
                                             iVar2 != 0))))))
        {
          iVar2 = piVar1[iVar4];
          positions[iVar3] = iVar4;
          iids[iVar3] = iVar2;
          iVar3 = iVar3 + 1;
        }
        iVar4 = iVar4 + 1;
        iVar2 = piVar1[iVar4];
      } while (iVar2 != -1);
    }
    default_test_definition(&test, 0);
    iVar2 = select_card_from_zone(player, player, iids, iVar3, 0, 1, -1, &test);
    if ((iVar2 == -1) || (iVar3 = charge_mana_from_id(player, -1, EVENT_ACTIVATE,
                                                      (unsigned int)cards_data[iids[iVar2]].id),
                          iVar3 == 0))
    {
      spell_fizzled = 1;
    }
    else
    {
      new_default_test_definition(&this_test, 1, "Select a land card to discard.");
      iVar3 = new_select_a_card(player, player, 4, 0, 3, -1, &this_test);
      if (iVar3 != -1)
      {
        discard_card(player, iVar3);
        play_card_in_grave_for_free(player, player, positions[iVar2]);
        cant_be_responded_to = 1;
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23363d0; original cards/modern_horizons.c:158. */
int card_aria_of_flame(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    gain_life(1 - player, 10);
    sp_ability_trigger(player, EVENT_ETB, 2, TYPE_SPELL, NULL, player, card);
  }
  else
  {
    sp_ability_trigger(player, event, 2, TYPE_SPELL, NULL, player, card);
    if (event == EVENT_SPA)
    {
      add_counters(player, card, player, card, COUNTER_VERSE, 1);
      default_target_definition(player, card, &td, 0x100);
      td.zone = 0x1200;
      td.allow_cancel = 0;
      result = can_target(&td);
      if (result != 0)
      {
        result = new_pick_target(&td, "TARGET_PLAYER_OR_PLANESWALKER", 0, 0);
        if (result != 0)
        {
          result = count_counters(player, card, COUNTER_VERSE);
          damage_target0(player, card, result);
        }
      }
      instance = get_card_instance(player, card);
      instance->number_of_targets = '\0';
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x233ada0; original cards/modern_horizons.c:2277. */
int card_mirrodin_besieged(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  if (event == EVENT_RESOLVE_SPELL)
  {

    iVar2 = get_hand_sum(player, TYPE_ARTIFACT);
    iVar3 = count_graveyard_by_type(player, 0x40);
    iVar2 = do_dialog(player, player, card, -1, -1, " Mirran\n Phyrexian",
                      (unsigned int)(iVar2 <= iVar3));
    instance = get_card_instance(player, card);
    instance->targets[1].player = iVar2;
    instance = get_card_instance(player, card);
    iVar2 = instance->targets[1].player;
    if (iVar2 != 0)
      goto LAB_0233add7;
    sp_ability_trigger(player, EVENT_RESOLVE_SPELL, 2, TYPE_ARTIFACT, NULL, player, card);
  }
  else
  {
    instance = get_card_instance(player, card);
    iVar2 = instance->targets[1].player;
    if (iVar2 != 0)
      goto LAB_0233add7;
    sp_ability_trigger(player, event, 2, TYPE_ARTIFACT, NULL, player, card);
    if (event == EVENT_SPA)
    {
      generate_token_by_id(player, card, 0x4968);
    }
  }
  iVar2 = instance->targets[1].player;
LAB_0233add7:
  if ((iVar2 == 1) && (current_turn == player))
  {
    iVar2 = eot_trigger(player, card, event);
    if (iVar2 != 0)
    {
      draw_a_card(player);
      discard(player, 0, player);
      iVar2 = count_graveyard_by_type(player, 0x40);
      if (0xe < iVar2)
      {
        target_opponent(player, card);
        lose_the_game(instance->targets[0].player);
      }
    }
  }
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x233b2e0; original cards/modern_horizons.c:2399. */
int card_mox_tantalite(int player, int card, event_t event)
{
  int result;

  result = suspend_only(player, card, event, 3, 0, 0, 0, 0, 0, 0);
  if (result != 0)
  {
    return result;
  }
  result = artifact_mana_all_one_color(player, card, event, 1, 0);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int bladeback_sliver_shared_ability(int player, int card, event_t event);
static int enduring_sliver_shared_ability(int player, int card, event_t event);
static int hollowhead_sliver_shared_ability(int player, int card, event_t event);
static int scuttling_sliver_shared_ability(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2334bb0; original cards/modern_horizons.c:372. */
static int bladeback_sliver_shared_ability(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  target_definition_t td2;

  pcVar1 = get_card_instance(player, card);
  iVar4 = pcVar1->targets[1].player;
  iVar2 = 0;
  if (-1 < iVar4)
  {

    iVar2 = pcVar1->targets[1].card;
    leaves_play(iVar4, iVar2, event, 0, 0);
    if (event == EVENT_LEAVES_PLAY_ABILITY)
    {
      add_status(player, card, 0x10000);
      in_play(iVar4, iVar2);
    }
    else
    {
      pcVar3 = in_play(iVar4, iVar2);
      if (pcVar3 == NULL)
      {
        if (event == EVENT_CAN_ACTIVATE)
        {
          return 0;
        }
        if (event == EVENT_CLEANUP)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    iVar4 = is_humiliated(iVar4, iVar2, AL_NM_NL_ACT);
    if ((iVar4 != 0) || (0 < hand_count[player]))
    {
      return 0;
    }
    if (event == EVENT_ACTIVATE)
    {
      default_target_definition(player, card, &td2, 0x100);
      td2.zone = 0x1200;
      td2.allow_cancel = 0;
    }
    else
    {
      if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        return 0;
      }
      default_target_definition(player, card, &td2, 0x100);
      td2.zone = 0x1200;
      td2.allow_cancel = 0;
      if ((event == EVENT_RESOLVE_ACTIVATION) &&
          (iVar4 = would_validate_arbitrary_target(&td2, pcVar1->targets[2].player,
                                                   pcVar1->targets[2].card),
           iVar4 != 0))
      {
        damage_creature(pcVar1->targets[2].player, pcVar1->targets[2].card, 1,
                        pcVar1->targets[0].player, pcVar1->targets[0].card);
      }
    }
    iVar2 = shared_sliver_activated_ability(player, card, event, 5, 0, 0, 0, 0, 0, 0, 0, &td2,
                                            "TARGET_ANY");
  }
  return iVar2;
}

/* ManalinkEh 2022: 0x2336e00; original cards/modern_horizons.c:420. */
int card_bladeback_sliver(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_RESOLVE_SPELL)
  {

    result = create_legacy_activate(player, card, bladeback_sliver_shared_ability);
    instance = get_card_instance(player, result);
    instance->targets[1].player = player;
    instance->targets[1].card = card;
    instance->number_of_targets = '\x01';
  }
  result = slivercycling(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2334e10; original cards/modern_horizons.c:806. */
static int enduring_sliver_shared_ability(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int iVar4;

  pcVar2 = get_card_instance(player, card);
  iVar4 = pcVar2->targets[1].player;
  if (iVar4 < 0)
  {
    return 0;
  }

  iVar1 = pcVar2->targets[1].card;
  leaves_play(iVar4, iVar1, event, 0, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    add_status(player, card, 0x10000);
    in_play(iVar4, iVar1);
    iVar4 = is_humiliated(iVar4, iVar1, AL_NM_NL_ACT);
    if (iVar4 != 0)
    {
      return 0;
    }
  LAB_02334eb0:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_02334ec4;
    }
  }
  else
  {
    pcVar3 = in_play(iVar4, iVar1);
    if (pcVar3 == NULL)
    {
      if (event == EVENT_CAN_ACTIVATE)
      {
        return 0;
      }
      if (event == EVENT_CLEANUP)
      {
        kill_card(player, card, KILL_EXILE);
        iVar4 = is_humiliated(iVar4, iVar1, AL_NM_NL_ACT);
        if (iVar4 != 0)
        {
          return 0;
        }
        goto LAB_02334ece;
      }
    }
    iVar4 = is_humiliated(iVar4, iVar1, AL_NM_NL_ACT);
    if (iVar4 != 0)
    {
      return 0;
    }
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_02334eb0;
  LAB_02334ec4:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02334ece;
  }
  iVar4 = pcVar2->targets[0].card;
  iVar1 = pcVar2->targets[0].player;
  add_counters(iVar1, iVar4, iVar1, iVar4, COUNTER_P1_P1, 1);
LAB_02334ece:
  iVar4 =
      shared_sliver_activated_ability(player, card, event, 0x11, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar4;
}

/* ManalinkEh 2022: 0x2337bd0; original cards/modern_horizons.c:847. */
int card_enduring_sliver(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_RESOLVE_SPELL)
  {

    result = create_legacy_activate(player, card, enduring_sliver_shared_ability);
    instance = get_card_instance(player, result);
    instance->targets[1].player = player;
    instance->targets[1].card = card;
    instance->number_of_targets = '\x01';
  }
  result = slivercycling(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2335010; original cards/modern_horizons.c:1895. */
static int hollowhead_sliver_shared_ability(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  result = instance->targets[1].player;
  if (result < 0)
  {
    return 0;
  }

  card_00 = instance->targets[1].card;
  leaves_play(result, card_00, event, 0, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    add_status(player, card, 0x10000);
    in_play(result, card_00);
    result = is_humiliated(result, card_00, AL_NM_NL_ACT);
    if (result != 0)
    {
      return 0;
    }
  LAB_023350ae:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_023350c2;
    }
  }
  else
  {
    instance = in_play(result, card_00);
    if (instance == NULL)
    {
      if (event == EVENT_CAN_ACTIVATE)
      {
        return 0;
      }
      if (event == EVENT_CLEANUP)
      {
        kill_card(player, card, KILL_EXILE);
        result = is_humiliated(result, card_00, AL_NM_NL_ACT);
        if (result != 0)
        {
          return 0;
        }
        goto LAB_023350cc;
      }
    }
    result = is_humiliated(result, card_00, AL_NM_NL_ACT);
    if (result != 0)
    {
      return 0;
    }
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_023350ae;
  LAB_023350c2:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_023350cc;
  }
  draw_a_card(player);
LAB_023350cc:
  result =
      shared_sliver_activated_ability(player, card, event, 0x21, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x233a360; original cards/modern_horizons.c:1936. */
int card_hollowhead_sliver(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_RESOLVE_SPELL)
  {

    result = create_legacy_activate(player, card, hollowhead_sliver_shared_ability);
    instance = get_card_instance(player, result);
    instance->targets[1].player = player;
    instance->targets[1].card = card;
    instance->number_of_targets = '\x01';
  }
  result = slivercycling(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2335750; original cards/modern_horizons.c:2883. */
static int scuttling_sliver_shared_ability(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;

  pcVar1 = get_card_instance(player, card);
  result = pcVar1->targets[1].player;
  if (result < 0)
  {
    return 0;
  }

  card_00 = pcVar1->targets[1].card;
  leaves_play(result, card_00, event, 0, 0);
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    add_status(player, card, 0x10000);
    in_play(result, card_00);
    result = is_humiliated(result, card_00, AL_NM_NL_ACT);
    if (result != 0)
    {
      return 0;
    }
  LAB_023357f0:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_02335804;
    }
  }
  else
  {
    pcVar2 = in_play(result, card_00);
    if (pcVar2 == NULL)
    {
      if (event == EVENT_CAN_ACTIVATE)
      {
        return 0;
      }
      if (event == EVENT_CLEANUP)
      {
        kill_card(player, card, KILL_EXILE);
        result = is_humiliated(result, card_00, AL_NM_NL_ACT);
        if (result != 0)
        {
          return 0;
        }
        goto LAB_0233580e;
      }
    }
    result = is_humiliated(result, card_00, AL_NM_NL_ACT);
    if (result != 0)
    {
      return 0;
    }
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_023357f0;
  LAB_02335804:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_0233580e;
  }
  untap_card(pcVar1->targets[0].player, pcVar1->targets[0].card);
LAB_0233580e:
  result = shared_sliver_activated_ability(player, card, event, 0, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x233bfd0; original cards/modern_horizons.c:2924. */
int card_scuttling_sliver(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_RESOLVE_SPELL)
  {

    result = create_legacy_activate(player, card, scuttling_sliver_shared_ability);
    instance = get_card_instance(player, result);
    instance->targets[1].player = player;
    instance->targets[1].card = card;
    instance->number_of_targets = '\x01';
  }
  result = slivercycling(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x233cad0; original cards/modern_horizons.c:3205. */
int card_soulherder(int player, int card, event_t event)
{
  int result;
  resolve_trigger_t trig_mode;
  card_instance_t *instance;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_td;

  if (trigger_condition == 0xcd)
  {
    if (((card != affected_card) || (player != affected_card_controller)) ||
        (current_turn != player))
      goto LAB_0233cafe;

    default_target_definition(player, card, &recovered_stack_td.target, 2);
    recovered_stack_td.target.allowed_controller = player;
    recovered_stack_td.target.preferred_controller = player;
    recovered_stack_td.target.special = 0x4000;
    instance = get_card_instance(player, card);
    instance->number_of_targets = '\0';
    result = can_target(&recovered_stack_td.target);
    trig_mode = RESOLVE_TRIGGER_0;
    if (result != 0)
    {
      result = duh_mode(player);
      trig_mode = (result != 0) + RESOLVE_TRIGGER_OPTIONAL;
    }
    result = eot_trigger_mode(player, card, event, player, trig_mode);
    if ((result != 0) &&
        (result = pick_target(&recovered_stack_td.target, "ASHNODS_BATTLEGEAR"), result != 0))
    {
      blink_effect(instance->targets[0].player, instance->targets[0].card, NULL);
    }
  }
  if (((trigger_condition == 0xd4) && (card == affected_card)) &&
      ((player == affected_card_controller &&
        ((reason_for_trigger_controller == player &&
          (instance = get_card_instance(trigger_cause_controller, trigger_cause),
           instance->kill_code == '\x04'))))))
  {
    default_test_definition(&recovered_stack_td.test, 2);
    test_leaves_play(player, card, event, 2, &recovered_stack_td.test, 1, 1);
  }
LAB_0233cafe:
  if (event == EVENT_LEAVES_PLAY_ABILITY)
  {
    add_counters(player, card, player, card, COUNTER_P1_P1, 1);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x233d4d0; original cards/modern_horizons.c:3529. */
int card_the_first_sliver(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  check_legend_rule(player, card, event);
  cascade(player, card, event);
  if (event == EVENT_CAST_SPELL)
  {
    instance = in_play(player, card);
    if ((instance != NULL) && (affected_card_controller == player))
    {
      result = has_subtype(player, affected_card, SUBTYPE_SLIVER);
      if (result != 0)
      {
        result = is_token(affected_card_controller, affected_card);
        if (result == 0)
        {
          result = check_special_flags(affected_card_controller, affected_card, 0x40000);
          if (result == 0)
          {
            result = is_humiliated(player, card, AL_TRIG);
            if (result == 0)
            {
              do_cascade(affected_card_controller, affected_card, -1);
            }
          }
        }
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x233b0f0; original cards/modern_horizons.c:2353. */
int card_morophon_the_boundless(int player, int card, event_t event)
{
  subtype_t subtype;
  card_instance_t *instance;
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    if (player != 1)
    {
      instance = get_card_instance(player, card);
      result = select_a_subtype(player, card);
      instance->info_slot = result;
      return 0;
    }
    state_untargettable(1, card, 1);
    instance = get_card_instance(1, card);
    result = select_a_subtype(1, card);
    instance->info_slot = result;
    state_untargettable(1, card, 0);
  }
  else if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    if (affected_card_controller == player)
    {
      instance = get_card_instance(player, card);
      result = has_subtype(affected_card_controller, affected_card, instance->info_slot);
      if (result != 0)
      {
        result = is_humiliated(player, card, AL_STATIC);
        if (result == 0)
        {
          COST_RED = COST_RED + -1;
          COST_WHITE = COST_WHITE + -1;
          COST_BLUE = COST_BLUE + -1;
          COST_BLACK = COST_BLACK + -1;
          COST_GREEN = COST_GREEN + -1;
        }
      }
    }
  }
  else if (((event - EVENT_POWER < 2) && (affected_card_controller == player)) &&
           (affected_card != card))
  {
    result = is_humiliated(player, card, AL_STATIC);
    if (result == 0)
    {

      instance = get_card_instance(player, card);
      subtype = instance->info_slot;
      if (subtype - SUBTYPE_ALLY < 0x113)
      {
        instance = in_play(affected_card_controller, affected_card);
        if (instance != NULL)
        {
          result = has_subtype(affected_card_controller, affected_card, subtype);
          if (result != 0)
          {
            event_result = event_result + 1;
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x233cce0; original cards/modern_horizons.c:3282. */
int card_spiteful_sliver(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;

  if (event == EVENT_DEAL_DAMAGE)
  {

    pcVar2 = damage_being_dealt(EVENT_DEAL_DAMAGE);
    if ((pcVar2 != NULL) && (pcVar2->damage_target_player == player))
    {
      iVar1 = pcVar2->damage_target_card;
      if (iVar1 != -1)
      {
        iVar3 = is_what(player, iVar1, 2);
        if (iVar3 != 0)
        {
          iVar3 = has_subtype(player, iVar1, SUBTYPE_SLIVER);
          if (iVar3 != 0)
          {
            iVar3 = is_humiliated(player, iVar1, AL_TRIG);
            if (iVar3 == 0)
            {

              iVar1 = create_targetted_legacy_effect(player, iVar1, spiteful_sliver_legacy, player,
                                                     iVar1);
              pcVar4 = get_card_instance(player, iVar1);
              pcVar4->targets[9].card = pcVar2->info_slot;
            }
          }
        }
      }
    }
  }
  iVar1 = slivercycling(player, card, event);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2336980; original cards/modern_horizons.c:267. */
int card_ayula_queen_among_bears(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  target_definition_t tdb;
  target_definition_t td;

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
    new_default_test_definition(&recovered_stack_test.test, 0x47, "");
    recovered_stack_test.test.subtype = 0x12;
    recovered_stack_test.test.not_me = 1;
    etb_ability_global(player, event, 2, TYPE_NONE, &recovered_stack_test.test, player, card);
  }
  if (event == EVENT_ETB)
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &tdb, 2);
    tdb.required_subtype = 0x12;
    tdb.preferred_controller = player;
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.allow_cancel = 0;
    default_target_definition(player, card, &recovered_stack_test.target, 2);
    recovered_stack_test.test.message[0x44] = '\0';
    recovered_stack_test.test.message[0x45] = '\0';
    recovered_stack_test.test.message[0x46] = '\0';
    recovered_stack_test.test.message[0x47] = '\0';
    recovered_stack_test.target.allowed_controller = (int32_t)(1 - player);
    recovered_stack_test.target.preferred_controller =
        (int32_t)((uint32_t)(recovered_stack_test.target.allowed_controller));
    result = can_target(&td);
    uVar3 = 0;
    if (result != 0)
    {
      result = can_target(&recovered_stack_test.target);
      uVar3 = (unsigned int)(result != 0);
    }
    result = can_target(&tdb);
    result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\003", "\001\016",
                         "Counters", result, 5, "Fight", uVar3, 5, 0, 0, 0, 0);
    if (result == 1)
    {
      instance->number_of_targets = '\0';
      new_pick_target(&tdb, "Select target bear", 0, 0x80);
      add_counters(player, card, instance->targets[0].player, instance->targets[0].card,
                               COUNTER_P1_P1, 2);
    }
    else if (result == 2)
    {
      instance->number_of_targets = '\0';
      result = new_pick_target(&td, "Select target creature you control.", 0, 0x81);
      if (result != 0)
      {
        new_pick_target(&recovered_stack_test.target, "Select target creature opponent controls.",
                        1, 0x81);
      }
      fight(instance->targets[0].player, instance->targets[0].card, instance->targets[1].player,
            instance->targets[1].card);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int pro_colorless_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23353f0; original cards/modern_horizons.c:1319. */
static int pro_colorless_legacy(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;

  attached_gets_sp_kw2(player, card, event, SP_KEYWORD2_GRANTED_PROT_MISC);
  if (event == EVENT_PROT_MISC)
  {
    instance = get_card_instance(player, card);
    if (((instance != NULL) && (instance->damage_target_card == affected_card)) &&
        (instance->damage_target_player == affected_card_controller))
    {
      uVar2 = get_color(attacking_card_controller, attacking_card);
      if ((uVar2 & 0x3e) == 0)
      {
        event_result = event_result | 1;
      }
    }
  }
  else if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2338d80; original cards/modern_horizons.c:1331. */
int card_giver_of_runes(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  color_t color;
  keyword_t kVar3;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {

      result = get_deck_color(player, 1 - player);
      color = choose_a_color_from(player, result, COLOR_TEST_ANY);
      if (color == COLOR_COLORLESS)
      {
        create_targetted_legacy_effect(player, card, pro_colorless_legacy,
                                       instance->targets[0].player, instance->targets[0].card);
      }
      else
      {
        kVar3 = color_to_protection(color);
        pump_ability_until_eot(instance->parent_controller, instance->parent_card,
                               instance->targets[0].player, instance->targets[0].card, 0, 0,
                               kVar3 | KEYWORD_RECALC_SET_COLOR, 0);
      }
    }
  }
  result = generic_activated_ability(player, card, event, 5, 0, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x233b920; original cards/modern_horizons.c:2630. */
int card_prismatic_vista(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  bool bVar3;
  test_definition_t this_test;

  if (event == EVENT_RESOLVE_SPELL)
  {
    play_land_sound_effect(player, card);
  }
  else
  {
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (mana_colors_added[player] == 0)
      {
        result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 0, 0, 0, 0, 0, 0, 1,
                                           NULL, NULL);
        return result;
      }
      result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      instance = get_card_instance(player, card);
      if (6 < life[player])
      {
        ai_modifier = ai_modifier + 1000;
      }
      if (mana_colors_added[player] == 0)
      {
        instance->info_slot = 2;
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 9, 0, 0, 0, 0, 0, 0, 1,
                                           NULL, NULL);
        return result;
      }

      result = paying_mana();
      if ((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9,
                                                               1, 0, 0, 0, 0, 0, 1, NULL, NULL),
                            result != 0))
      {
        bVar3 = ai_is_speculating != 1;
        instance->info_slot = 2;
        if ((bVar3) && ((player != 1 || (((unsigned char)trace_mode & 2) != 0))))
        {
          result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1, "Fetch",
                               1, 1, 0, 0, 0, 0);
          if (result == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if (result == 1)
            goto LAB_0233b9e2;
          if (result != 2)
          {
            return 0;
          }
        }
        generic_activated_ability(player, card, EVENT_ACTIVATE, 9, 0, 0, 0, 0, 0, 0, 1, NULL, NULL);
        return 0;
      }
      instance->info_slot = 1;
    LAB_0233b9e2:
      mana_producer(player, card, EVENT_ACTIVATE);
      return 0;
    }
    if ((event == EVENT_CLEANUP) || (event == EVENT_RESOLVE_ACTIVATION))
    {
      instance = get_card_instance(player, card);
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }
      if (instance->info_slot != 2)
      {
        return 0;
      }
      new_default_test_definition(&this_test, 1, "Select a basic land");
      this_test.subtype = 0x2000;
      new_global_tutor(player, player, 1, 4, 0, 1, &this_test);
      return 0;
    }
  }
  if (mana_colors_added[player] == 0)
  {
    return 0;
  }
  result = mana_producer(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23382e0; original cards/modern_horizons.c:1068. */
int card_force_of_negation(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_MODIFY_COST)
  {
    if (current_turn != player)
    {
      result = get_sleighted_color(player, card, 2);
      al_pitchspell(player, card, EVENT_MODIFY_COST, result, 0);
    }
    counterspell_target_definition(player, card, &td, 0xff);
    return 0;
  }
  counterspell_target_definition(player, card, &td, 0xff);
  td.illegal_type = 2;
  if (event == EVENT_CAN_CAST)
  {
    result = generic_spell(player, card, EVENT_CAN_CAST, 2, &td, NULL, 0, NULL);
    return result;
  }
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
  else if (event != EVENT_CHANGE_TARGET)
    goto LAB_02338392;
  if ((current_turn == player) ||
      (result = casting_al_pitchspell(player, card, event, 2, 0), result != 0))
  {
    result = counterspell(player, card, event, &td, 0);
    return result;
  }
  spell_fizzled = 1;
LAB_02338392:
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }
  result = counterspell_validate(player, card, &td, 0);
  if (result != 0)
  {

    instance = get_card_instance(player, card);
    result = pre_counterspell(player, card, instance->targets[0].player, instance->targets[0].card);
    if (result == 0)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
    }
    else
    {
      put_on_top_of_deck(instance->targets[0].player, instance->targets[0].card);
      rfg_top_card_of_deck(instance->targets[0].player);
    }
  }
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x233d650; original cards/modern_horizons.c:3566. */
int card_throes_of_chaos(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_CAN_CAST)
  {
    cascade(player, card, event);
    if (event == EVENT_RESOLVE_SPELL)
    {
      kill_card(player, card, KILL_DESTROY);
    }
    result = retrace(player, card, event, 0x4f0a);
    return result;
  }
  result = basic_spell(player, card, EVENT_CAN_CAST);
  return result;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2338f60; original cards/modern_horizons.c:1369. */
int card_gluttonous_slug(int player, int card, event_t event)
{
  menace(player, card, event);
  evolve(player, card, event);
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23393d0; original cards/modern_horizons.c:1511. */
int card_hexdrinker(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_PROT_MISC) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = count_counters(player, card, COUNTER_LEVEL);
    if (7 < result)
    {
      protection_from_everything(player, card, EVENT_PROT_MISC);
    }
  }
  result = level_up(player, card, event, 1, 0, 0, 0, 0, 0, 2, 1, 3, 4, 4,
                         KEYWORD_PROT_INSTANTS, SP_KEYWORD_0, 8, 6, 6, KEYWORD_0, SP_KEYWORD_0);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2335b80; original cards/modern_horizons.c:2540. */
static int sac_at_eot(int player, int card, event_t event)
{
  char cVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_player)
  {
    instance = get_card_instance(player, card);
    if ((((trigger_condition == TRIGGER_EOT) && (card == affected_card)) &&
         (player == affected_card_controller)) &&
        (player == reason_for_trigger_controller))
    {
      iVar2 = instance->targets[1].player;
      if (iVar2 < 1)
      {
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
        }
        else if (event == EVENT_RESOLVE_TRIGGER)
        {
          cVar1 = instance->damage_target_player;
          iVar2 = instance->damage_target_card;
          iVar4 = check_state((int)cVar1, iVar2, STATE_OUBLIETTED);
          if (iVar4 == 0)
          {
            kill_card((int)cVar1, iVar2, KILL_SACRIFICE);
          }
          else
          {
            kill_card(player, card, KILL_EXILE);
          }
        }
      }
      else
      {
        instance->targets[1].player = iVar2 + -1;
      }
    }
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x233b5c0; original cards/modern_horizons.c:2569. */
int card_planebound_accomplice(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    instance = get_card_instance(player, card);
    new_default_test_definition(&this_test, 0x100, "Select a planeswalker card.");
    result = new_global_tutor(player, player, 4, 4, 0, 2, &this_test);
    if (-1 < result)
    {
      create_targetted_legacy_effect(instance->parent_controller, instance->parent_card, sac_at_eot,
                                     player, result);
    }
  }
  result = generic_activated_ability(player, card, event, 0, 0, 0, 0, 0, 1, 0, 0, NULL, NULL);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x233de70; original cards/modern_horizons.c:3912. */
int card_winds_of_abandon(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  int iVar5;
  target_definition_t td;
  test_definition_t test;
  test_definition_t this_test;
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = 1 - player;
    instance = get_card_instance(player, card);
    iVar5 = generic_spell(player, card, EVENT_CAN_CAST, 0, NULL, NULL, 0, NULL);
    if (iVar5 != 0)
    {
      if (instance->info_slot == 1)
      {
        return 1;
      }
      iVar5 = can_target(&td);
      return iVar5;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.allowed_controller = 1 - player;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        iVar5 = check_special_flags2(player, card, -0x80000000);
        if ((iVar5 != 0) || (uVar4 = pay_overload(player, card, 4, 0, 0, 0, 0, 2), uVar4 == 1))
        {
          new_pick_target(&td, "Select target creature you don\'t control.", 0, 0x81);
          if (spell_fizzled == 1)
          {
            return 0;
          }
          uVar4 = 1;
        }
        instance->info_slot = instance->info_slot | uVar4;
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    iVar5 = 1 - player;
    td.allowed_controller = iVar5;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      if (((unsigned int)instance->info_slot & 2) == 0)
      {
        iVar5 = valid_target(&td);
        if (iVar5 != 0)
        {
          kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
          iVar5 = instance->targets[0].player;
          if ((iVar5 == 1) ||
              (iVar5 = do_dialog(iVar5, player, card, -1, -1, " Search a basic land\n Pass", 0),
               iVar5 == 0))
          {
            new_default_test_definition(&this_test, 1, "Select a basic land card.");
            this_test.subtype = 0x2000;
            iVar5 = instance->targets[0].player;
            new_global_tutor(iVar5, iVar5, 1, 5, 0, 5, &this_test);
          }
        }
      }
      else
      {
        default_test_definition(&test, 2);
        iVar2 = new_manipulate_all(player, card, iVar5, &test, ACT_KILL_REMOVE);
        if ((player == 0) ||
            (iVar3 = do_dialog(iVar5, player, card, -1, -1, " Search for basic lands\n Pass", 0),
             iVar3 == 0))
        {
          new_default_test_definition(&this_test, 1, "Select a basic land card.");
          this_test.subtype = 0x2000;
          this_test.qty = (unsigned short)iVar2;
          new_global_tutor(iVar5, iVar5, 1, 5, 0, 5, &this_test);
        }
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  iVar5 = overload(player, card, event, 4, 0, 0, 0, 0, 2);
  return iVar5;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2335930; original cards/modern_horizons.c:1533. */
static int has_convoked_mana_for_hogaak_impl(int player, int cless, int black, int blue, int green,
                                      int red, int white)
{
  int size = active_cards_count[player];
  int tapped[size];
  int tapped_for_color[size][5];
  memset(tapped_for_color, 0, sizeof tapped_for_color);
  int attempt = 0;
  int scan_size = size;
  for (;;)
  {
    int clrs[6] = {cless, black, blue, green, red, white};
    int result = 0;
    int tc = 0;
    if (scan_size > 0)
    {
      int i = 0;
      do
      {
        if (in_play(player, i) && !is_tapped(player, i) && is_what(player, i, TYPE_CREATURE))
        {
          unsigned int color = get_color(player, i);
          for (int k = 1; k != 6; ++k)
          {
            if ((color >> k & 1U) && clrs[k] > 0 && tapped_for_color[i][k - 1] != 1)
            {
              --clrs[k];
              tapped_for_color[i][k - 1] = 1;
              add_state(player, i, STATE_TAPPED);
              tapped[tc++] = i;
              goto after_creature;
            }
          }
          if (clrs[0] != 0)
          {
            --clrs[0];
            add_state(player, i, STATE_TAPPED);
            tapped[tc++] = i;
          }
        }
      after_creature:
        if (clrs[0] + clrs[1] + clrs[2] + clrs[3] + clrs[4] + clrs[5] == 0)
          result = 1;
        ++i;
      } while (i < active_cards_count[player]);
      for (int i = 0; i < tc; ++i)
        remove_state(player, tapped[i], STATE_TAPPED);
      if (tc > 0 && result != 0)
        return 2;
    }
    ++attempt;
    if (attempt > 49 || result != 0)
      return result;
    scan_size = active_cards_count[player];
  }
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x23394f0; original cards/modern_horizons.c:1581. */
int can_cast_hogaak(int player, int card, event_t event, int from_grave)
{
  int iVar1;
  int iVar2;
  unsigned int uVar3;

  iVar1 = get_updated_casting_cost(player, card, -1, event, -1);
  iVar2 = count_graveyard(player);
  uVar3 = iVar2 - (unsigned int)(from_grave != 0);
  uVar3 = iVar1 - (uVar3 & (int)(uVar3 ^ 0xffffffff) >> 0x1f);
  uVar3 = uVar3 & (int)(uVar3 ^ 0xffffffff) >> 0x1f;
  iVar1 = has_convoked_mana_for_hogaak_impl(player, uVar3, 2, 0, 0, 0, 0);
  if (iVar1 != 2)
  {
    iVar1 = has_convoked_mana_for_hogaak_impl(player, uVar3, 1, 0, 1, 0, 0);
    if (iVar1 != 2)
    {
      iVar1 = has_convoked_mana_for_hogaak_impl(player, uVar3, 0, 0, 2, 0, 0);
      return (unsigned int)(iVar1 == 2);
    }
  }
  return 1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2339600; original cards/modern_horizons.c:1725. */
int charge_mana_hybrid_to_hogaak_convoke(int player, int card, int number, int first_color,
                                         int second_color, int colorless)
{
  int mana[6] = {0, 0, 0, 0, 0, 0};
  int mana2[10];
  char buffer[500];
  test_definition_t this_test;
  int pos = scnprintf(buffer, 500, " Cancel\n");
  int options = 1;
  if (number >= 0)
  {
    int i = number;
    do
    {
      mana[first_color] = i;
      mana[second_color] = number - i;
      if (has_convoked_mana_for_hogaak_impl(player, colorless, mana[1], mana[2], mana[3], mana[4],
                                            mana[5]))
      {
        mana2[options] = 0;
        mana_into_string(colorless, mana[1], mana[2], mana[3], mana[4], mana[5], &this_test);
        pos += scnprintf(buffer + pos, 500 - pos, " Pay %s\n", this_test.message);
        for (int color = 1; color != 6; ++color)
        {
          for (int k = 0; k < mana[color]; ++k)
          {
            mana2[options] |= 1U << (((color - 1) * 6 + k) & 31);
          }
        }
        ++options;
      }
      --i;
    } while (i != -1);
  }
  int choice = 1;
  if (number >= 0 && options >= 3 && player == 0)
  {
    choice = do_dialog(0, 0, card, -1, -1, buffer, 1);
    if (choice == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
  }
  unsigned int encoded = mana2[choice];
  for (int i = 0; i != 6; ++i)
    mana[i] = 0;
  for (int color = 1; color != 6; ++color)
  {
    for (int k = 0; k != 6; ++k)
    {
      if (encoded >> ((color - 1) * 6 + k) & 1U)
        ++mana[color];
    }
  }
  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_CREATURE);
  td.allowed_controller = player;
  td.preferred_controller = player;
  int size = active_cards_count[player];
  td.illegal_state = 1;
  td.illegal_abilities = 0;
  td.allow_cancel = 1;
  int tapped[size];
  memset(tapped, 0xff, sizeof tapped);
  card_instance_t *instance = get_card_instance(player, card);
  if (player != 0)
  {
    int color_size = active_cards_count[player];
    int tapped_for_color[color_size][5];
    memset(tapped_for_color, 0, sizeof tapped_for_color);
    int attempts_left = 50;
    int scan_size = color_size;
    for (;;)
    {
      int clrs[6] = {colorless, mana[1], mana[2], mana[3], mana[4], mana[5]};
      int tc = 0;
      if (scan_size > 0)
      {
        int i = 0;
        do
        {
          if (in_play(player, i) && !is_tapped(player, i) && is_what(player, i, TYPE_CREATURE))
          {
            unsigned int color = get_color(player, i);
            for (int k = 1; k != 6; ++k)
            {
              if ((color >> k & 1U) && clrs[k] > 0 && tapped_for_color[i][k - 1] != 1)
              {
                --clrs[k];
                tapped_for_color[i][k - 1] = 1;
                add_state(player, i, STATE_TAPPED);
                tapped[tc++] = i;
                goto after_creature;
              }
            }
            if (clrs[0] != 0)
            {
              add_state(player, i, STATE_TAPPED);
              tapped[tc++] = i;
              --clrs[0];
            }
          }
        after_creature:
          if (clrs[0] + clrs[1] + clrs[2] + clrs[3] + clrs[4] + clrs[5] == 0)
          {
            if (spell_fizzled != 1)
            {
              for (int j = 0; j < tc; ++j)
                tap_card(player, tapped[j]);
              return (unsigned int)(spell_fizzled != 1);
            }
            break;
          }
          ++i;
        } while (i < active_cards_count[player]);
        for (int j = 0; j < tc; ++j)
          remove_state(player, tapped[j], STATE_TAPPED);
      }
      --attempts_left;
      if (attempts_left == 0)
      {
        spell_fizzled = 1;
        return (unsigned int)(spell_fizzled != 1);
      }
      scan_size = active_cards_count[player];
    }
  }
  int clrs[6] = {colorless, mana[1], mana[2], mana[3], mana[4], mana[5]};
  int tc = 0;
  do
  {
    if (!can_target(&td))
      break;
    if (!select_target(0, card, &td, "Select a creature to tap for Convoke.", NULL))
    {
      spell_fizzled = 1;
      goto rollback;
    }
    instance->number_of_targets = 0;
    if (will_tap_permanent_for_convoke(instance->targets[0].player, instance->targets[0].card,
                                            clrs))
    {
      add_state(instance->targets[0].player, instance->targets[0].card, STATE_TAPPED);
      tapped[tc++] = instance->targets[0].card;
    }
  } while (clrs[1] + clrs[0] + clrs[2] + clrs[3] + clrs[4] + clrs[5] != 0);
  if (spell_fizzled == 1)
  {
  rollback:
    for (int i = 0; i < tc; ++i)
      remove_state(0, tapped[i], STATE_TAPPED);
  }
  else
  {
    for (int i = 0; i < tc; ++i)
      tap_card(0, tapped[i]);
  }
  return (unsigned int)(spell_fizzled != 1);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2339e70; original cards/modern_horizons.c:1781. */
int cast_hogaak(int player, int card, event_t event, int from_grave)
{
  unsigned short uVar1;
  card_ptr_t *pcVar2;
  int *piVar3;
  int32_t iVar4;
  int32_t *piVar5;
  card_instance_t *instance;
  int iVar7;
  unsigned int cless;
  unsigned int uVar8;
  unsigned int uVar9;
  int iVar10;
  target_t *ptVar11;
  unsigned int uStack_f4;
  test_definition_t this_test;

  instance = get_card_instance(player, card);
  iVar7 = get_id(player, card);
  pcVar2 = cards_ptr[iVar7];
  cless = get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, -1);
  iVar7 = has_convoked_mana_for_hogaak_impl(
      player, cless, (unsigned int)pcVar2->req_black, (unsigned int)pcVar2->req_blue,
      (unsigned int)pcVar2->req_green, (unsigned int)pcVar2->req_red,
      (unsigned int)pcVar2->req_white);
  if (iVar7 == 2)
  {
    iVar7 = do_dialog(player, player, card, -1, -1, " Use Delve\n Don\'t use Delve\n Cancel", 1);
    if (iVar7 == 2)
    {
      spell_fizzled = 1;
      return 0;
    }
    if (iVar7 != 0)
    {
      if (iVar7 != 1)
      {
        return 0;
      }
      charge_mana_hybrid_to_hogaak_convoke(player, card, 2, 1, 3, cless);
      return (unsigned int)(spell_fizzled != 1);
    }
  }
  iVar7 = count_graveyard(player);
  uVar8 = iVar7 - (unsigned int)(from_grave != 0);
  uVar8 = uVar8 & (int)(uVar8 ^ 0xffffffff) >> 0x1f;
  uStack_f4 = cless;
  if ((int)uVar8 < (int)cless)
  {
    uStack_f4 = uVar8;
  }
  new_default_test_definition(&this_test, 0xff, "Select a card to exile for Delve.");
  if (from_grave != 0)
  {
    piVar3 = graveyard_ptr_mutable[player];
    do
    {
      uVar1 = cards_data[piVar3[uVar8]].id;
      uVar9 = get_id(player, card);
      if (uVar1 == uVar9)
      {
        iVar7 = turn_card_in_grave_face_down(player, uVar8);
        goto LAB_02339fcb;
      }
      uVar8 = uVar8 - 1;
    } while (uVar8 != 0xffffffff);
  }
  iVar7 = -1;
LAB_02339fcb:
  iVar10 = select_multiple_cards_from_graveyard(player, player, 0, 3, &this_test, uStack_f4,
                                                     instance->targets);
  for (int i = 9; i > 0; --i)
  {
    instance->targets[i] = instance->targets[i - 1];
  }
  if (from_grave != 0)
  {
    turn_card_in_grave_face_up(player, uVar8, iVar7);
  }
  charge_mana_hybrid_to_hogaak_convoke(player, card, 2, 1, 3, cless - iVar10);
  if (spell_fizzled != 1)
  {
    if (0 < iVar10)
    {
      ptVar11 = instance->targets + 1;
      do
      {
        piVar5 = &ptVar11->player;
        ptVar11 = ptVar11 + 1;
        rfg_card_from_grave(player, *piVar5);
      } while (ptVar11 != instance->targets + iVar10 + 1);
    }
    return 1;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x233a1b0; original cards/modern_horizons.c:1846. */
int card_hogaak_arisen_necropolis(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  if (event == EVENT_MODIFY_COST)
  {
    result = can_cast_hogaak(player, card, EVENT_MODIFY_COST, 0);
    if (result == 0)
    {
      infinite_casting_cost();
    }
    else
    {
      null_casting_cost(player, card);
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = played_for_free(player, card);
      if ((result != 0) || (result = is_token(player, card), result != 0))
      {
        return 1;
      }
      result = cast_hogaak(player, card, EVENT_CAST_SPELL, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
    }
  }
  else if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = can_sorcery_be_played(player, EVENT_GRAVEYARD_ABILITY);
    if ((result != 0) &&
        (result = can_cast_hogaak(player, card, EVENT_GRAVEYARD_ABILITY, 1), result != 0))
    {
      return 2;
    }
  }
  else if (event == EVENT_PAY_FLASHBACK_COSTS)
  {
    result = cast_hogaak(player, card, EVENT_PAY_FLASHBACK_COSTS, 1);
    return (unsigned int)(result != 0);
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2338530; original cards/modern_horizons.c:1125. */
int card_force_of_vigor(int player, int card, event_t event)
{
  target_t *ptVar1;
  int iVar2;
  target_t *ptVar3;
  unsigned int uVar5;
  card_instance_t *pcVar6;
  int iVar7;
  card_instance_t *pcVar8;
  int iVar9;
  int iVar10;
  target_definition_t td;

  if (event == EVENT_MODIFY_COST)
  {
    if (current_turn != player)
    {
      iVar9 = get_sleighted_color(player, card, 3);
      al_pitchspell(player, card, EVENT_MODIFY_COST, iVar9, 0);
    }
    default_target_definition(player, card, &td, 0x44);
    return 0;
  }
  default_target_definition(player, card, &td, 0x44);
  if (event == EVENT_CAN_CAST)
  {
    iVar9 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "DISENCHANT", 1, NULL);
    return iVar9;
  }
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
    pcVar8 = get_card_instance(player, card);
    iVar9 = check_special_flags2(player, card, 0x2000);
    if (iVar9 == 0)
    {
      check_special_flags2(player, card, 0x8000);
    }
  }
  else
  {
    if (event != EVENT_CHANGE_TARGET)
      goto LAB_023387ea;
    pcVar8 = get_card_instance(player, card);
    iVar9 = check_special_flags2(player, card, 0x10000);
    if (iVar9 != 0)
    {
      int count = (unsigned int)pcVar8->number_of_targets;
      int t_array[count];
      int tac = 0;
      if (pcVar8->number_of_targets != 0)
      {
        do
        {
          int c = pcVar8->targets[tac].card;
          int p = pcVar8->targets[tac].player;
          ++tac;
          if (c == -1)
            t_array[tac - 1] = get_internal_card_id_from_csv_id(p == player ? 0x615a : 0x6161);
          else
            t_array[tac - 1] = get_card_instance(p, c)->internal_card_id;
        } while (tac < (unsigned int)pcVar8->number_of_targets);
      }
      return show_deck(player, t_array, tac,
                            " Select the target to change (Rules Engine = you, Deadbox = opponent)",
                            0, ENGINE_DONE_LABEL);
    }
    iVar9 = check_special_flags2(player, card, 0x4000);
    if (iVar9 != 0)
    {
      td.zone = 0x200;
      td.required_type = 2;
    }
    iVar9 = check_special_flags2(player, card, 0x2000);
    if (iVar9 == 0)
    {
      iVar9 = check_special_flags2(player, card, 0x8000);
      if (iVar9 == 0)
        goto LAB_023386fc;
    }
    td.who_chooses = attacking_card_controller;
    td.allow_cancel = 0;
  }
LAB_023386fc:
  if (current_turn != player)
  {
    iVar9 = casting_al_pitchspell(player, card, event, 3, 0);
    if (iVar9 == 0)
    {
      spell_fizzled = 1;
    }
  }
  pcVar8->number_of_targets = '\0';
  iVar9 = 0;
  do
  {
    iVar10 = can_target(&td);
    if (iVar10 == 0)
      break;
    iVar10 = new_pick_target(&td, "DISENCHANT", iVar9, 0);
    if (iVar10 == 0)
      break;
    ptVar3 = pcVar8->targets + iVar9;
    ptVar1 = pcVar8->targets + iVar9;
    iVar9 = iVar9 + 1;
    state_untargettable(ptVar1->player, ptVar3->card, 1);
  } while (iVar9 != 2);
  iVar9 = pcVar8->targets[0].player;
  if ((iVar9 != -1) && (iVar10 = pcVar8->targets[0].card, iVar10 != -1))
  {
    state_untargettable(iVar9, iVar10, 0);
  }
  iVar9 = pcVar8->targets[1].player;
  if ((iVar9 != -1) && (iVar10 = pcVar8->targets[1].card, iVar10 != -1))
  {
    state_untargettable(iVar9, iVar10, 0);
  }
  if (event == EVENT_CHANGE_TARGET)
  {
    iVar9 = pcVar8->targets[0].player;
    if ((iVar9 != -1) && (iVar10 = pcVar8->targets[0].card, iVar10 != -1))
    {
      state_untargettable(iVar9, iVar10, 0);
    }
    iVar9 = pcVar8->targets[1].player;
    if (iVar9 == -1)
    {
      return 0;
    }
    iVar10 = pcVar8->targets[1].card;
    if (iVar10 == -1)
    {
      return 0;
    }
    state_untargettable(iVar9, iVar10, 0);
    return 0;
  }
LAB_023387ea:
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }
  iVar9 = 0;
  pcVar8 = get_card_instance(player, card);
  if (pcVar8->number_of_targets != '\0')
  {
    do
    {
      ptVar3 = pcVar8->targets + iVar9;
      ptVar1 = pcVar8->targets + iVar9;
      iVar9 = iVar9 + 1;
      kill_card(ptVar1->player, ptVar3->card, KILL_DESTROY);
    } while (iVar9 < (int)(unsigned int)pcVar8->number_of_targets);
  }
  pcVar8->number_of_targets = '\0';
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

#endif
