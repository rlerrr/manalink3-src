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
  test_definition_t this_test;
  if ((((trigger_condition == 0xdb) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    default_test_definition(&this_test, 0x47);
    this_test.subtype = 0xb1;
    etb_ability_global(player, event, 2, TYPE_NONE, &this_test, player, card);
  }
  card_00 = trigger_cause;
  iVar1 = trigger_cause_controller;
  if (event == EVENT_ETB)
  {
    default_target_definition(trigger_cause_controller, trigger_cause,
                              (target_definition_t *)&this_test, 0x100);
    this_test.message[0xc] = '\0';
    this_test.message[0xd] = '\x12';
    this_test.message[0xe] = '\0';
    this_test.message[0xf] = '\0';
    this_test.message[0x44] = '\0';
    this_test.message[0x45] = '\0';
    this_test.message[0x46] = '\0';
    this_test.message[0x47] = '\0';
    instance = get_card_instance(iVar1, card_00);
    iVar3 = can_target((target_definition_t *)&this_test);
    if (iVar3 != 0)
    {
      iVar3 =
          new_pick_target((target_definition_t *)&this_test, "TARGET_PLAYER_OR_PLANESWALKER", 0, 0);
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
