#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23b1fa0; original cards/commander_2019.c:2. */
static int goad_effect(int player, int card, event_t event)
{
  card_instance_t *instance;
  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_card)
  {
    instance = get_card_instance(player, card);
    attack_if_able((int)instance->damage_target_player, instance->damage_target_card, event);
  }
  if (event == EVENT_CAN_SKIP_TURN)
  {
    instance = get_card_instance(player, card);
    if (instance->damage_source_player == current_turn)
    {
      kill_card(player, card, KILL_EXILE);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23b2090; original cards/commander_2019.c:31. */
int card_anje_falkenrath(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  haste(player, card, event);
  result = discard_trigger(player, card, event, player, 2, 0);
  if ((result != 0) &&
      (result = get_original_internal_card_id(trigger_cause_controller, trigger_cause),
       cards_data[result].cc[2] == '\v'))
  {
    untap_card(player, card);
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      draw_a_card(player);
    }
  }
  result = generic_activated_ability(player, card, event, 0x21, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x23b21b0; original cards/commander_2019.c:62. */
int card_apex_altisaur(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if ((event != EVENT_ETB) && (result = damage_dealt_to_me_arbitrary(
                                   player, card, event, DDBM_TRIGGER_OPTIONAL, player, card),
                               result == 0))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = 1 - player;
  result = can_target(&td);
  if ((result != 0) &&
      (result = new_pick_target(&td, "Select target creature you don\'t control.", 0, 0x80),
       result != 0))
  {
    instance = get_card_instance(player, card);
    result = instance->targets[0].card;
    instance->number_of_targets = '\0';
    fight(player, card, instance->targets[0].player, result);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23b28a0; original cards/commander_2019.c:384. */
int card_dockside_extortionist(int player, int card, event_t event)
{
  int howmany;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  howmany = count_permanents_by_type(1 - player, TYPE_ARTIFACT | TYPE_ENCHANTMENT);
  generate_tokens_by_id(player, card, 0x4998, howmany);
  return 0;
}

/* ManalinkEh 2022: 0x23b2cd0; original cards/commander_2019.c:486. */
int card_gerrard_weatherlight_hero(int player, int card, event_t event)
{
  int result;
  check_legend_rule(player, card, event);
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  exile_from_owners_graveyard(player, card);
  reanimate_all_dead_this_turn(player, -0x7fffffbe);
  return 0;
}

/* ManalinkEh 2022: 0x23b2f70; original cards/commander_2019.c:572. */
int card_gift_of_doom(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      morph(player, card, EVENT_CAST_SPELL, 0, 0, 0, 0, 0, 0);
      result = get_id(player, card);
      if (result == 0x33c4)
      {
        return 0;
      }
      result = generic_aura(player, card, EVENT_CAST_SPELL, player, 0, 0, 0, 0xa00, 0, 0, 0);
      return result;
    }
  }
  else if ((((event == EVENT_CAN_CAST) || (event == EVENT_RESOLVE_SPELL)) ||
            (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
    goto LAB_023b2fac;
  if ((((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (2 < event - EVENT_POWER)) &&
        ((event != EVENT_RESOLVE_MOVING_AURA &&
          ((event != EVENT_CHANGE_TYPE && (event != EVENT_SET_COLOR)))))) &&
       (event != EVENT_GET_SELECTED_CARD)) &&
      (event != EVENT_STATIC_EFFECTS))
  {
    if (event == EVENT_CAN_UNMORPH)
    {
      default_test_definition(&this_test, 2);
      this_test.qty = 1;
      this_test.not_me = 1;
      result = new_can_sacrifice_as_cost(player, card, &this_test);
      return result;
    }
    if (event == EVENT_UNMORPH)
    {
      new_default_test_definition(&this_test, 2, "Select a creature to sacrifice.");
      this_test.not_me = 1;
      result = new_sacrifice(player, card, player, SAC_AS_COST, &this_test);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
    }
    else if (event == EVENT_TURNED_FACE_UP)
    {
      result = vanilla_aura(player, card, EVENT_CAN_CAST, player);
      if (result == 0)
      {
        kill_card(player, card, KILL_STATE_BASED_ACTION);
      }
      else
      {
        vanilla_aura(player, card, EVENT_CAST_SPELL, player);
        vanilla_aura(player, card, EVENT_RESOLVE_SPELL, player);
      }
    }
    result = morph(player, card, event, 0, 0, 0, 0, 0, 0);
    return result;
  }
LAB_023b2fac:
  morph(player, card, event, 0, 0, 0, 0, 0, 0);
  result = generic_aura(player, card, event, player, 0, 0, 0, 0xa00, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x23b2030; original cards/commander_2019.c:904. */
static char *commander_you_own(int who_chooses, int player, int card, int targeting_player,
                               int targeting_card)
{
  int result;
  char *pcVar2;
  result = get_owner(player, card);
  pcVar2 = "owner";
  if (result == targeting_player)
  {
    result = check_special_flags2(player, card, 0x1000000);
    pcVar2 = NULL;
    if (result == 0)
    {
      return "must be a commander";
    }
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x23b3dd0; original cards/commander_2019.c:983. */
int card_sevinnes_reclamation(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if ((event - EVENT_CAST_SPELL & 0xfffffff7) != 0)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = check_special_flags(player, card, 0x10000000);
      if (result != 0)
      {
        result = check_special_flags2(player, card, -0x80000000);
        if (result == 0)
        {
          copy_spell_from_stack(player, player, card);
        }
      }
      result = get_flashback(player, card);
      if (result != 0)
      {
        kill_card(player, card, KILL_EXILE);
      }
      result = validate_target_from_grave_source(player, card, player, 0);
      if (result != -1)
      {
        reanimate_permanent(player, card, player, result, REANIMATE_DEFAULT);
      }
      kill_card(player, card, KILL_DESTROY);
    }
    result = do_flashback(player, card, event, 4, 0, 0, 0, 0, 1);
    return result;
  }
  new_default_test_definition(&test, 0x47, "Select a permanent card with mana value 3 or less.");
  test.cmc = 4;
  test.cmc_flag = 3;
  result = generic_spell(player, card, event, 8, NULL, NULL, 0, &test);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23b32d0; original cards/commander_2019.c:622. */
int card_greven_predator_captain(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int card_00;
  test_definition_t test_sac;

  check_legend_rule(player, card, event);
  menace(player, card, event);
  if (event != EVENT_POWER)
  {
    dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_OPTIONAL, 0);
    if (event == EVENT_DAT_ABILITY)
    {
      result = can_sacrifice_type_as_cost(player, 1, TYPE_CREATURE);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        instance->token_status = instance->token_status | 0x20000;
        new_default_test_definition(&test_sac, 2, "Select another creature to sacrifice.");
        test_sac.not_me = 1;
        uVar3 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_JUST_MARK, &test_sac);
        if (uVar3 == 0)
        {
          spell_fizzled = 1;
          instance->token_status = instance->token_status & 0xfffdffff;
        }
        else
        {
          if (uVar3 == 0xffffffff)
          {
            backtrace_and_abort("cards/commander_2019.c:653\nsac != -1");
          }
          card_00 = uVar3 >> 0x18;
          uVar3 = (int)uVar3 >> 0x10 & 0xff;
          result = get_power(uVar3, card_00);
          draw_cards(player, result);
          result = get_toughness(uVar3, card_00);
          lose_life(player, result);
          kill_card(uVar3, card_00, KILL_SACRIFICE);
          instance->token_status = instance->token_status & 0xfffdffff;
        }
      }
    }
    return 0;
  }
  if ((card == affected_card) && (player == affected_card_controller))
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      result = get_trap_condition(player, 2);
      event_result = event_result + result;
      instance = get_card_instance(player, card);
      instance->regen_status = instance->regen_status | 0x4000000;
    }
  }
  dat_ability(player, card, EVENT_POWER, DAT_RESOLVE_TRIGGER_OPTIONAL, 0);
  return 0;
}

/* ManalinkEh 2022: 0x23b3840; original cards/commander_2019.c:784. */
int card_marisi_breaker_of_the_coil(int player, int card, event_t event)
{
  unsigned int uVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  int card_00;
  int num[2];
  test_definition_t test;

  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    iVar2 = affected_card_controller;
    if (affected_card_controller == -1)
    {
      iVar2 = event_result;
    }
    if ((((1 - player == iVar2) && (current_phase - 0x15U < 7)) &&
         (iVar2 = is_what(affected_card_controller, affected_card, 1), iVar2 == 0)) &&
        (iVar2 = is_humiliated_for_ability(player, card, AL_STATIC), iVar2 == 0))
    {
      infinite_casting_cost();
    }
  }
  default_test_definition(&test, 2);
  check_damage_test(player, -1, event,
                    DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                        DDBM_MUST_BE_COMBAT_DAMAGE,
                    player, card, 0, &test);
  iVar2 = resolve_damage_trigger(player, card, event,
                                 DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER |
                                     DDBM_MUST_BE_COMBAT_DAMAGE,
                                 player, card);
  if (iVar2 != 0)
  {

    iVar2 = 0;
    instance = get_card_instance(player, card);
    uVar1 = instance->targets[1].player;
    instance->targets[1].player = 0;
    num[0] = uVar1 & 0xff;
    num[1] = uVar1 >> 8 & 0xff;
    uVar1 = num[0];
    while (true)
    {
      if ((uVar1 != 0) && (0 < active_cards_count[iVar2]))
      {
        card_00 = 0;
        do
        {
          instance = in_play(iVar2, card_00);
          if ((instance != NULL) && (iVar4 = is_what(iVar2, card_00, 2), iVar4 != 0))
          {

            create_targetted_legacy_effect(player, card, goad_effect, iVar2, card_00);
          }
          card_00 = card_00 + 1;
        } while (card_00 < active_cards_count[iVar2]);
      }
      iVar2 = iVar2 + 1;
      if (iVar2 == 2)
        break;
      uVar1 = num[iVar2];
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x23b3a10; original cards/commander_2019.c:849. */
int card_ohran_frostfang(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_ABILITIES)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if ((result == 0) && (affected_card_controller == player))
    {
      result = is_what(player, affected_card, 2);
      if (result != 0)
      {
        result = is_attacking(affected_card_controller, affected_card);
        if (result != 0)
        {
          instance = in_play(player, card);
          if (instance != NULL)
          {
            deathtouch_2022(affected_card_controller, affected_card, EVENT_ABILITIES);
          }
        }
      }
    }
  }
  result = subtype_deals_damage(player, card, event, player, -1, 0x14);
  if (result == 0)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  draw_cards(player, instance->targets[1].card);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int atla_palani_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x23b14c0; original cards/commander_2019.c:95. */
static int atla_palani_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  int result;
  card_instance_t *pcVar4;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x4fe3))
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
      goto LAB_023b14ff;
    }
    uVar2 = resolve_gfp_ability(player, card, EVENT_STATIC_EFFECTS, RESOLVE_TRIGGER_MANDATORY);
    result = check_status(player, card, 0x10000);
    if (result != 0)
    {
      return 0;
    }
  LAB_023b153a:

    uVar2 = uVar2 & 0xff;
    if (uVar2 == 0)
      goto LAB_023b158c;
  }
  else
  {
    pcVar1 = get_card_instance(player, card);
    if (pcVar1->damage_target_player < '\0')
    {
      if (event == EVENT_GRAVEYARD_FROM_PLAY)
        goto LAB_023b1640;
    LAB_023b14ff:
      uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
      result = check_status(player, card, 0x10000);
      if (result != 0)
        goto LAB_023b158c;
      goto LAB_023b153a;
    }
    result = effect_follows_control_of_attachment(player, card, event);
    if (result != 0)
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_023b14ff;
  LAB_023b1640:

    pcVar1 = get_card_instance(player, card);
    pcVar4 = in_play(affected_card_controller, affected_card);

    if ((pcVar4 != NULL) && (pcVar4 = get_card_instance(affected_card_controller, affected_card),
                             pcVar4->kill_code != '\0'))
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
        *(uint8_t *)((int)&pcVar1->targets[0xb].player + 2) = 1;
      }
      default_test_definition(&test, 0x47);
      test.subtype = 0xfb;
      count_for_gfp_ability(player, card, 0x77, player, 0, &test);
    }
    uVar2 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    result = check_status(player, card, 0x10000);
    if (result != 0)
    {
      return 0;
    }
    uVar2 = uVar2 & 0xff;
    if (uVar2 == 0)
    {
      return 0;
    }
  }
  do
  {
    default_test_definition(&test, 2);
    uVar2 = uVar2 - 1;
    reveal_card_until_you_reveal_test(player, player, &test, 0x44);
    kill_card(player, card, KILL_EXILE);
  } while (uVar2 != 0);
LAB_023b158c:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x23b22a0; original cards/commander_2019.c:154. */
int card_atla_palani_nest_tender(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uStack_ec;
  test_definition_t test;

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
                instance->info_slot_fn == atla_palani_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_023b23dc;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, atla_palani_effect, player, card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  LAB_023b23dc:
    instance = get_card_instance(player, card);
    if (instance->kill_code == '\0')
    {
    LAB_023b2408:

      uStack_ec = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
      if (uStack_ec != 0)
      {
      LAB_023b242b:

        uStack_ec = uStack_ec & 0xff;
        if (uStack_ec != 0)
        {
          do
          {
            default_test_definition(&test, 2);
            reveal_card_until_you_reveal_test(player, player, &test, 0x44);
            uStack_ec = uStack_ec - 1;
          } while (uStack_ec != 0);
        }
      }
      goto LAB_023b22df;
    }
  LAB_023b23ee:
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_023b22ef;
    }
  }
  else
  {
    if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {
      default_test_definition(&test, 0x47);
      test.subtype = 0xfb;
      count_for_gfp_ability(player, card, 0x77, player, 0, &test);
      instance = get_card_instance(player, card);
      if (instance->kill_code != '\0')
      {
        return 0;
      }
      uStack_ec =
          resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
      if (uStack_ec != 0)
        goto LAB_023b242b;
      goto LAB_023b23ee;
    }
    instance = get_card_instance(player, card);
    if (instance->kill_code == '\0')
      goto LAB_023b2408;
  LAB_023b22df:
    if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
      goto LAB_023b23ee;
  LAB_023b22ef:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_023b22f9;
  }
  generate_token_by_id(player, card, 0x4fed);
LAB_023b22f9:
  iVar2 = generic_activated_ability(player, card, event, 1, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23b2a50; original cards/commander_2019.c:424. */
int card_elsha_of_the_infinite(int player, int card, event_t event)
{
  int result;
  uint32_t uVar2;
  unsigned int uVar3;

  check_legend_rule(player, card, event);
  prowess(player, card, event);
  if (event == EVENT_CAN_ACTIVATE)
  {
    return (unsigned int)(*deck_ptr[player] != -1);
  }
  if (event != EVENT_ACTIVATE)
  {
    return 0;
  }

  result = *deck_ptr[player];
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    uVar2 = 0;
    if (result != -1)
      goto LAB_023b2acd;
  }
  else
  {
    uVar2 = 1;
    if (result == -1)
    {
      uVar2 = 0;
      uVar3 = 0;
      goto LAB_023b2af7;
    }
  LAB_023b2acd:
    result = is_what(-1, result, 2);
    if (result == 0)
    {
      result = is_what(-1, *deck_ptr[player], 1);
      uVar3 = 0;
      if (result == 0)
      {
        result = has_mana_to_cast_iid(player, EVENT_ACTIVATE, *deck_ptr[player]);
        uVar3 = (unsigned int)(result != 0);
      }
      goto LAB_023b2af7;
    }
  }
  uVar3 = 0;
LAB_023b2af7:
  result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016",
                       "Check top card of deck", uVar2, 0, "Play top card", uVar3, 10, 0, 0, 0, 0);
  if (result != 0)
  {
    if (result != 1)
    {
      if (result == 2)
      {
        result = charge_mana_from_id(player, -1, EVENT_ACTIVATE,
                                     (unsigned int)cards_data[*deck_ptr[player]].id);
        if (result == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        play_card_in_deck_for_free(player, player, 0);
        cant_be_responded_to = 1;
      }
      return 0;
    }
    show_deck(player, deck_ptr[player], 1, "Here\'s the top card of your deck", 0,
              ENGINE_DONE_LABEL);
  }
  spell_fizzled = 1;
  return 0;
}

/* ManalinkEh 2022: 0x23b3540; original cards/commander_2019.c:662. */
int card_grismold_the_dreadsower(int player, int card, event_t event)
{
  int iVar1;
  unsigned int uVar2;
  int iVar3;
  token_generation_t token;

  check_legend_rule(player, card, event);
  if ((current_turn == player) &&
      (iVar3 = eot_trigger(player, card, event), iVar1 = current_turn, iVar3 != 0))
  {

    default_token_definition(player, card, 0x4973, &token);
    token.t_player = iVar1;
    token.pow = 1;
    token.tou = 1;
    generate_token(&token);
    uVar2 = current_turn ^ 1;
    default_token_definition(player, card, 0x4973, &token);
    token.pow = 1;
    token.tou = 1;
    token.t_player = uVar2;
    generate_token(&token);
  }
  count_for_gfp_ability_and_store_values(player, card, event, 2, 2, NULL, 0x400, 0);
  uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (uVar2 != 0)
  {

    for (uVar2 = uVar2 & 0xff; uVar2 != 0; uVar2 = uVar2 - 1)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23b25b0; original cards/commander_2019.c:235. */
int card_bone_miser(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;

  iVar1 = discard_trigger(player, card, event, player, 2, 0);
  if (iVar1 == 0)
  {
    return 0;
  }

  iVar1 = is_what(trigger_cause_controller, trigger_cause, 2);
  iVar2 = is_what(trigger_cause_controller, trigger_cause, 1);
  if (iVar1 == 0)
  {
    if (iVar2 == 0)
    {
      draw_a_card(player);
      return 0;
    }
  }
  else
  {
    generate_token_by_id(player, card, 0x49ac);
    if (iVar2 == 0)
    {
      return 0;
    }
  }
  produce_mana(player, 1, 2);
  return 0;
}

/* ManalinkEh 2022: 0x23b3b30; original cards/commander_2019.c:911. */
int card_sanctum_of_eternity(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      default_target_definition(player, card, &td, 0x47);
      td.special = 0x8000;
      td.extra = (int32_t)commander_you_own;
      instance = get_card_instance(player, card);

      instance->info_slot = 0;
      result = paying_mana();
      if (((result == 0) &&
           (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 7, 2, 0, 0, 0, 0,
                                               0, 0, &td, "Select a commander you own."),
            result != 0)) &&
          (result = do_dialog(player, player, card, -1, -1,
                              " Generate mana\n Bounce your commander\n Cancel", 1),
           result != 0))
      {
        if (result != 1)
        {
          return 0;
        }
        add_state(player, card, 0x10);
        result = generic_activated_ability(player, card, EVENT_ACTIVATE, 6, 2, 0, 0, 0, 0, 0, 0,
                                           &td, "Select a commander you own.");
        if (result == 0)
        {
          remove_state(player, card, 0x10);
        }
        else
        {
          instance->info_slot = 1;
        }
        tapped_for_mana_color = -1;
        return 0;
      }
      result = mana_producer(player, card, EVENT_ACTIVATE);
      return result;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      default_target_definition(player, card, &td, 0x47);
      td.special = 0x8000;
      td.extra = (int32_t)commander_you_own;
      instance = get_card_instance(player, card);
      if ((event == EVENT_RESOLVE_ACTIVATION) && (0 < instance->info_slot))
      {
        bounce_permanent(instance->targets[0].player, instance->targets[0].card);
        return 0;
      }
      return 0;
    }
  }
  result = mana_producer(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23b3f90; original cards/commander_2019.c:1032. */
int card_song_of_the_worldsoul(int player, int card, event_t event)
{
  int result;

  sp_ability_trigger(player, event, 2, TYPE_ANY, NULL, player, card);
  if (event == EVENT_SPA)
  {
    populate(player, card);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int effect_volrath_the_shapestealer(int player, int card, event_t event);
static int volrath_the_shapestealer_impl(int player, int card, event_t event, int is_effect);

/* ManalinkEh 2022: 0x23b1bd0; original cards/commander_2019.c:1106. */
static int volrath_the_shapestealer_impl(int player, int card, event_t event, int is_effect)
{
  char cVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int card_00;
  int iStack_78;
  int iStack_74;
  target_definition_t td_copy;

  pcVar2 = get_card_instance(player, card);
  pcVar4 = pcVar2;
  iStack_78 = player;
  iStack_74 = card;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    iStack_78 = pcVar2->parent_controller;
    iStack_74 = pcVar2->parent_card;
    pcVar4 = get_card_instance(iStack_78, iStack_74);
  }
  if (is_effect == 0)
  {
    check_legend_rule(player, card, event);
    cloning_card(player, card, event);
    iVar3 = beginning_of_combat(player, card, event, player);
    if (iVar3 != 0)
    {
      default_target_definition(player, card, &td_copy, 2);
      td_copy.preferred_controller = 1 - player;
      iVar3 = can_target(&td_copy);
      if ((iVar3 != 0) && (iVar3 = new_pick_target(&td_copy, "TARGET_CREATURE", 0, 1), iVar3 != 0))
      {
        add_counters_from_source(player, card, pcVar2->targets[0].player, pcVar2->targets[0].card,
                                 COUNTER_M1_M1, 1);
      }
    }
  LAB_023b1c57:
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      goto LAB_023b1c75;
  }
  else
  {
    iStack_78 = (int)pcVar4->damage_target_player;
    iStack_74 = pcVar4->damage_target_card;
    cVar1 = pcVar2->damage_target_player;
    if (cVar1 < '\0')
      goto LAB_023b1c57;
    if (event != EVENT_POWER)
    {
      if (event == EVENT_TOUGHNESS)
      {
        if (pcVar2->damage_target_card != affected_card)
        {
          return 0;
        }
        if (cVar1 != affected_card_controller)
        {
          return 0;
        }
        if (pcVar2->counter_toughness < 0)
        {
          return 0;
        }
        set_base_pt(5);
        return 0;
      }
      goto LAB_023b1c57;
    }
    if (((pcVar2->damage_target_card == affected_card) && (cVar1 == affected_card_controller)) &&
        (-1 < pcVar2->counter_power))
    {
      set_base_pt(7);
    }
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
LAB_023b1c75:
  default_target_definition(player, card, &td_copy, 2);
  td_copy.special = 0x40000;
  td_copy.extra = 0;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar3 = 0;
    do
    {
      card_00 = active_cards_count[iVar3];
      if (-1 < card_00)
      {
        do
        {
          pcVar4 = in_play(iVar3, card_00);
          if ((((pcVar4 != NULL) && (pcVar4->damage_target_player == iStack_78)) &&
               (pcVar4->damage_target_card == iStack_74)) &&
              ((iVar5 = is_what(iVar3, card_00, 0x80),
                iVar5 != 0 && (pcVar4->info_slot_fn == effect_volrath_the_shapestealer))))
          {
            kill_card(iVar3, card_00, KILL_EXILE);
          }
          card_00 = card_00 + -1;
        } while (card_00 != -1);
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 != 2);
    iVar3 = create_targetted_legacy_activate(iStack_78, iStack_74, effect_volrath_the_shapestealer,
                                             iStack_78, iStack_74);
    set_legacy_image(iStack_78, 0x5018, iVar3);
    cloning_and_verify_legend(iStack_78, iStack_74, pcVar2->targets[0].player,
                              pcVar2->targets[0].card);
  }
  iVar3 = generic_activated_ability(player, card, event, 6, 1, 0, 0, 0, 0, 0, 0, &td_copy,
                                    "Select target creature with a counter on it");
  return iVar3;
}

/* ManalinkEh 2022: 0x23b1f80; original cards/commander_2019.c:1176. */
static int effect_volrath_the_shapestealer(int player, int card, event_t event)
{
  int result;

  result = volrath_the_shapestealer_impl(player, card, event, 1);
  return result;
}

/* ManalinkEh 2022: 0x23b4000; original cards/commander_2019.c:1179. */
int card_volrath_the_shapestealer(int player, int card, event_t event)
{
  int result;

  result = volrath_the_shapestealer_impl(player, card, event, 0);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23b2920; original cards/commander_2019.c:397. */
int card_doomed_artisan(int player, int card, event_t event)
{
  enum
  {
    SUBTYPE_SCULPTURE = 257
  };

  int result;
  token_generation_t token;

  if (((((event - EVENT_BLOCK_LEGALITY < 2) || (event == EVENT_ABILITIES)) &&
        (result = has_subtype(affected_card_controller, affected_card, SUBTYPE_SCULPTURE),
         result != 0)) &&
       ((affected_card_controller == player &&
         (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)))) &&
      ((1 < event - EVENT_BLOCK_LEGALITY || (event_result = 1, event == EVENT_ABILITIES))))
  {
    cannot_attack(affected_card_controller, affected_card, EVENT_ABILITIES);
    cannot_block(affected_card_controller, affected_card, EVENT_ABILITIES);
  }
  if ((current_turn == player) && (result = eot_trigger(player, card, event), result != 0))
  {
    default_token_definition(player, card, 0x500b, &token);
    token.special_infos = 0x42;
    generate_token(&token);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23b2d40; original cards/commander_2019.c:546. */
int card_ghired_conclave_exile(int player, int card, event_t event)
{
  int result;
  target_t tgt;
  token_generation_t token_1;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_token;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x4978, &recovered_stack_token.token);
    recovered_stack_token.token.tou = 4;
    recovered_stack_token.token.pow = 4;
    recovered_stack_token.token.key_plus = 0x80;
    generate_token(&recovered_stack_token.token);
    dat_ability(player, card, EVENT_ETB, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
    return 0;
  }
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    if ((player == 1) || (ai_is_speculating == 1))
    {
      default_test_definition(&recovered_stack_token.test, 2);
      tgt.card =
          check_battlefield_for_special_card(player, card, player, 1, &recovered_stack_token.test);
      tgt.player = player;
      if (tgt.card < 0)
      {
        return 0;
      }
    }
    else
    {
      base_target_definition(player, card, &recovered_stack_token.target, 2);
      recovered_stack_token.token.s_player = player;
      recovered_stack_token.token.s_card = player;
      recovered_stack_token.token.special_flags2 = 0;
      recovered_stack_token.token.keep_track_of_tokens_generated = 0x8000;
      recovered_stack_token.target.extra = (int32_t)(target_is_token_too_2022);
      result = can_target(&recovered_stack_token.target);
      if (result == 0)
      {
        return 0;
      }
      result = select_target(player, card + -1000, &recovered_stack_token.target,
                             "Select a creature token you control.", &tgt);
      if (result == 0)
      {
        return 0;
      }
    }
    copy_token_definition(player, card, &token_1, tgt.player, tgt.card);
    token_1.action = TOKEN_ACTION_ATTACKING;
    generate_token(&token_1);
  }
  return 0;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x23b36b0; original cards/commander_2019.c:720. */
int card_krrik_son_of_yawgmoth(int player, int card, event_t event)
{
  int result;
  test_definition_2022_t test;

  check_legend_rule(player, card, event);
  lifelink(player, card, event);
  phyrexian_casting_costs(player, card, event, 4, 3, 0, 0, 0, 0, 0);
  if (event == EVENT_STATIC_EFFECTS)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      card_flags[player] = card_flags[player] | 2;
    }
    if (trigger_condition != 0xd3)
    {
      return 0;
    }
  }
  else if (trigger_condition != 0xd3)
    goto LAB_023b3741;
  if ((card == affected_card) && (player == affected_card_controller))
  {
    default_test_definition(&test, 0xff);
    test.color = get_sleighted_color_test(player, card, 2);
    sp_ability_trigger(player, event, 2, TYPE_NONE, &test, player, card);
  }
LAB_023b3741:
  if (event != EVENT_SPA)
  {
    return 0;
  }
  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x23b1850; original cards/commander_2019.c:260. */
static int chainers_legacy(int player, int card, event_t event)
{
  int iVar6;
  int iVar7;
  int iVar8;
  int *piVar4;
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar6 = *graveyard_ptr_mutable[player];
    if (iVar6 != -1)
    {
      iVar8 = 0;
      do
      {

        iVar6 = is_what(-1, iVar6, 2);
        piVar4 = graveyard_ptr_mutable[player];
        if (iVar6 != 0)
        {

          iVar6 = can_legally_play_iid(player, *(int *)((int)piVar4 + iVar8));
          piVar4 = graveyard_ptr_mutable[player];
          if (iVar6 != 0)
          {

            iVar7 = has_mana_to_cast_iid(player, EVENT_CAN_ACTIVATE, *(int *)((int)piVar4 + iVar8));
            if (iVar7 == 0)
            {
              piVar4 = graveyard_ptr_mutable[player];
            }
            else
            {
              if (iVar6 == 99)
              {
                return 99;
              }

              iVar6 = can_sorcery_be_played(player, EVENT_CAN_ACTIVATE);
              if ((iVar6 != 0) || (piVar4 = graveyard_ptr_mutable[player],
                                   (cards_data[*(int *)((int)piVar4 + iVar8)].type & 0x30) != 0))
              {
                return 1;
              }
            }
          }
        }
        iVar8 = iVar8 + 4;
        iVar6 = *(int *)((int)piVar4 + iVar8);
      } while (iVar6 != -1);
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    int grave_count = count_graveyard(player);
    int playable[2][grave_count];
    int count = 0;
    int i = 0;
    int *grave = graveyard_ptr_mutable[player];
    int iid = *grave;
    test_definition_2022_t this_test;
    while (iid != -1)
    {
      if (!is_what(-1, iid, TYPE_CREATURE))
      {
        grave = graveyard_ptr_mutable[player];
        goto next_card;
      }
      grave = graveyard_ptr_mutable[player];
      int legal = can_legally_play_iid(player, grave[i]);
      grave = graveyard_ptr_mutable[player];
      if (!legal)
        goto next_card;
      if (!has_mana_to_cast_iid(player, EVENT_ACTIVATE, grave[i]))
      {
        grave = graveyard_ptr_mutable[player];
        goto next_card;
      }
      int sorcery_ok = can_sorcery_be_played(player, EVENT_ACTIVATE);
      grave = graveyard_ptr_mutable[player];
      iid = grave[i];
      if (sorcery_ok || (cards_data[iid].type & 0x30))
      {
        playable[0][count] = iid;
        playable[1][count] = i;
        ++count;
      }
    next_card:
      iid = grave[i + 1];
      ++i;
    }
    new_default_test_definition(&this_test, 0xff, "Select a creature card to play.");
    int selected =
        select_card_from_zone(player, player, playable[0], count, 0, 1, -1, &this_test);
    if (selected != -1 &&
        charge_mana_from_id(player, -1, EVENT_ACTIVATE, cards_data[playable[0][selected]].id))
    {
      play_card_in_grave_for_free(player, player, playable[1][selected]);
      cant_be_responded_to = 1;
      kill_card(player, card, KILL_EXILE);
      return 0;
    }
    spell_fizzled = 1;
    return 0;
  }
  else if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x23b26b0; original cards/commander_2019.c:329. */
int card_chainer_nightmare_adept(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_2022_t test;
  check_legend_rule(player, card, event);
  if ((((trigger_condition == TRIGGER_COMES_INTO_PLAY) &&
        (result = not_played_from_hand(trigger_cause_controller, trigger_cause), result != 0)) &&
       (card == affected_card)) &&
      ((player == affected_card_controller && (reason_for_trigger_controller == player))))
  {
    new_default_test_definition(&test, 2, "");
    test.type_flag = 3;
    etb_ability_global(player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_ETB)
  {
    result = pump_ability_until_eot_no_repeat(player, card, trigger_cause_controller,
                                                   trigger_cause, 0, 4);
    if (current_turn == player)
    {
      instance = get_card_instance(player, result);
      instance->targets[7].player = 1;
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_023b2721:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_023b2726;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_023b2721;
  }
  create_legacy_activate(player, card, chainers_legacy);
LAB_023b2726:
  result =
      generic_activated_ability(player, card, event, 0x2020, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

#endif
