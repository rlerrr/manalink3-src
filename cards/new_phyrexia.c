#include "manalink.h"

/* Canonical recovered helper declarations. */
int has_phyrexian_mana(int player, int cless, int black, int blue, int green, int red,
                            int white, int clred_artifact);
int has_phyrexian_mana_for_activated_ability(int player, int card, int cless, int black,
                                                  int blue, int green, int red, int white);
int charge_phyrexian_mana(int player, int card, event_t event, int cless, int black, int blue,
                               int green, int red, int white, int clred_artifact);
int charge_phyrexian_mana_for_activated_ability(int player, int card, int cless, int black,
                                                     int blue, int green, int red, int white);
int phyrexian_casting_costs(int player, int card, event_t event, int cless, int black,
                                 int blue, int green, int red, int white, int clred_artifact);
static int charge_phyrexian_mana_impl(int player, int cless, int black, int blue, int green,
                                           int red, int white, int clred_artifact);

// ---- GLOBAL FUNCTIONS ----














static int generic_splicer(int player, int card, event_t event, int pow, int tou, int keyword, int golems ){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		generate_tokens_by_id(player, card, CARD_ID_GOLEM, golems);
	}

	return boost_creature_type(player, card, event, SUBTYPE_GOLEM, pow, tou, keyword, BCT_CONTROLLER_ONLY | BCT_INCLUDE_SELF);
}

static void np_shrines(int player, int card, event_t event, int clr)
{

  /* Shrine of [Something]	|[cost]
   * Artifact
   * At the beginning of your upkeep or whenever you cast a [clr] spell, put a charge counter on ~.
   * |[act_cost] |T, Sacrifice ~: [do something] */

  // This shouldn't include resolution or construction of the target object - you can't factor out a
  // function by special-casing all of its callers.

  upkeep_trigger_ability(player, card, event, player);

  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    add_counters(player, card, player, card, COUNTER_CHARGE, 1);
  }

  if (trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card))
  {
    test_definition_t test;
    default_test_definition(&test, TYPE_ANY);
    test.color = get_sleighted_color_test(player, card, clr);
    sp_ability_trigger(player, event, RESOLVE_TRIGGER_MANDATORY, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_SPA)
    add_counters(player, card, player, card, COUNTER_CHARGE, 1);
}

static int is_poisoned(int player){
	if( POISON_COUNTERS(player) > 0 ){
		return 1;
	}
	return 0;
}

//---- CARDS ----

int card_batterskull(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  card_instance_t *pcVar5;
  unsigned int uVar6;
  unsigned int uVar7;
  int iVar8;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    token_generation_t token;
    uint8_t bytes[84];
  } recovered_stack_token;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_token_definition(player, card, 0x4935, &recovered_stack_token.token);
    recovered_stack_token.token.action = TOKEN_ACTION_EQUIP;
    generate_token(&recovered_stack_token.token);
    pcVar2 = in_play(player, card);
    if (pcVar2 == NULL)
    {
      return 0;
    }
    iVar3 = is_humiliated(player, card, AL_STATIC);
    if (iVar3 == 0)
      goto LAB_0226e15d;
  }
  else
  {
    pcVar2 = in_play(player, card);
    if ((pcVar2 != NULL) &&
        (iVar3 = is_humiliated(player, card, AL_STATIC), iVar3 == 0))
    {
    LAB_0226e15d:
      pcVar2 = is_equipping(player, card);
      if ((pcVar2 != NULL) && (pcVar2 = get_card_instance(player, card), event - EVENT_POWER < 3))
      {
        if ((pcVar2->damage_target_card == affected_card) &&
            (pcVar2->damage_target_player == affected_card_controller))
        {
          iVar3 = vanilla_equipment(player, card, event, 5, 4, 4, 0, 10);
          return iVar3;
        }
        goto LAB_0226deaf;
      }
    }
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      pcVar2 = get_card_instance(player, card);
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        if (iVar3 == 0)
        {
          iVar3 = basic_equipment(player, card, EVENT_CAN_ACTIVATE, 5);
          return iVar3;
        }
        return 1;
      }
      if (event == EVENT_ACTIVATE)
      {
        iVar3 = can_activate_basic_equipment(player, card, EVENT_ACTIVATE, 5);
        iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        pcVar5 = is_equipping(player, card);
        if (pcVar5 == NULL)
        {
          default_target_definition(player, card, &recovered_stack_token.target, 2);
          recovered_stack_token.token.s_card = player;
          recovered_stack_token.token.s_player = player;
          iVar8 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 5, 0, 0, 0, 0, 0,
                                            0, &recovered_stack_token.target, NULL);
          iVar8 = (-(unsigned int)(iVar8 == 0) & 9) + 1;
        }
        else
        {
          cVar1 = pcVar2->damage_target_player;
          iVar8 = pcVar2->damage_target_card;
          uVar6 = get_special_abilities((int)cVar1, iVar8);
          uVar7 = get_abilities((int)cVar1, iVar8, EVENT_ABILITIES, -1);
          iVar8 = 1;
          if ((uVar7 & 0x800000) != 0)
          {
            iVar8 = (-(unsigned int)((uVar6 & 0x20) == 0) & 0xfffffff7) + 10;
          }
        }
        iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Equip", iVar3, 5,
                            "Return Batterskul to hand", iVar4, iVar8, 0, 0, 0, 0);
        if (iVar3 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        if (iVar3 == 1)
        {
          iVar3 = basic_equipment(player, card, EVENT_ACTIVATE, 5);
          return iVar3;
        }
        if (iVar3 != 2)
        {
          return 0;
        }
        ai_modifier = ai_modifier + ((1 - player == current_turn) - 1 & 0xffffffe8) + 0x30;
        iVar3 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        return iVar3;
      }
      goto LAB_0226dec7;
    }
  }
LAB_0226deaf:
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
  pcVar2 = get_card_instance(player, card);
LAB_0226dec7:
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (pcVar2->info_slot == 1)
    {
      iVar3 = basic_equipment(player, card, EVENT_RESOLVE_ACTIVATION, 5);
      return iVar3;
    }
    if (pcVar2->info_slot == 2)
    {
      bounce_permanent(pcVar2->parent_controller, pcVar2->parent_card);
    }
  }
  return 0;
}


int card_blade_splicer(int player, int card, event_t event ){
	/* Blade Splicer	|2|W
	 * Creature - Human Artificer 1/1
	 * When ~ enters the battlefield, put a 3/3 colorless Golem artifact creature token onto the battlefield.
	 * Golem creatures you control have first strike. */

	return generic_splicer(player, card, event, 0, 0, KEYWORD_FIRST_STRIKE, 1);
}

int card_beast_within(int player, int card, event_t event ){
	/* Beast Within	|2|G
	 * Instant
	 * Destroy target permanent. Its controller puts a 3/3 |Sgreen Beast creature token onto the battlefield. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_PERMANENT);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
			token_generation_t token;
			default_token_definition(player, card, CARD_ID_BEAST, &token);
			token.t_player = instance->targets[0].player;
			generate_token(&token);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PERMANENT", 1, NULL);
}


int card_cathedral_membrane(int player, int card, event_t event ){
/*
Cathedral Membrane |1|{WP}
Artifact Creature — Wall 0/3
({WP} can be paid with either {W} or 2 life.)
Defender
When Cathedral Membrane dies during combat, it deals 6 damage to each creature it blocked this combat.
*/
	phyrexian_casting_costs(player, card, event, MANACOST_XW(1, 1), 1);

	if( current_phase > PHASE_MAIN1 && current_phase < PHASE_MAIN2 && this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY) ){
		card_instance_t* instance = get_card_instance(player, card);
		if( instance->blocking < 255 && in_play(1-player, instance->blocking) ){
			damage_creature(1-player, instance->blocking, 6, player, card);
		}
	}

	return 0;
}

// Chancellor of the Tangle code is in "rules_engine.c" in "functions" folder

int card_deceiver_exarch(int player, int card, event_t event ){
/*
Deceiver Exarch |2|U
Creature — Cleric 1/4
Flash (You may cast this spell any time you could cast an instant.)
When Deceiver Exarch enters the battlefield, choose one —
• Untap target permanent you control.
• Tap target permanent an opponent controls.
*/
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_PERMANENT);
		td.preferred_controller = ANYBODY;
		td.allow_cancel = 0;

		card_instance_t *instance = get_card_instance(player, card);

		if( can_target(&td) && new_pick_target(&td, "TARGET_PERMANENT", 0, 0) ){
			int p = instance->targets[0].player;
			int c = instance->targets[0].card;
			if( is_what(p, c, TYPE_LAND) ){
				ai_modifier -= 24;
			}
			if( is_what(p, c, TYPE_ENCHANTMENT | TARGET_TYPE_PLANESWALKER) ){
				ai_modifier -= 96;
			}
			if( p == player ){
				if( ! is_tapped(p, c) ){
					ai_modifier -= 96;
				}
				if( get_id(p, c) == CARD_ID_KIKI_JIKI_MIRROR_BREAKER ){
					ai_modifier += 128;
				}
				ai_modifier += (get_cmc(p, c) * 8);
			}
			else{
				if( is_tapped(p, c) ){
					ai_modifier -= 96;
				}
				ai_modifier += (get_cmc(p, c) * 8);
			}
			action_on_target(player, card, 0, p == player ? ACT_UNTAP : ACT_TAP);
		}
	}

	return flash(player, card, event);
}

int card_dismember(int player, int card, event_t event ){
/* CARD_ID_DISMEMBER	11592
Dismember	|1|PB|PB
Instant
(|PB can be paid with either |B or 2 life.)
Target creature gets -5/-5 until end of turn. */

	if (event == EVENT_CHECK_PUMP )
	{
		return vanilla_instant_pump(player, card, event, ANYBODY, 1-player, -5, -5, 0, 0);
	}

	if( event == EVENT_MODIFY_COST && affect_me(player, card) ){
		return phyrexian_casting_costs(player, card, event, MANACOST_XB(1, 2), 0);
	}

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		int life_before = life[player];
		int result = 1;
		if( ! played_for_free(player, card) && ! is_token(player, card) && instance->info_slot == 1 ){
			result = charge_phyrexian_mana(player, card, event, MANACOST_XB(1, 2), 0);
		}
		if( result ){
			if( life[player] < life_before ){
				td.allow_cancel = 0;
			}
			return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
		}
		else{
			spell_fizzled = 1;
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, -5, -5);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	 return 0;
}

int card_dispatch(int player, int card, event_t event ){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += ((current_phase < PHASE_DECLARE_BLOCKERS || metalcraft(player, card)) ? 24 : -24);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			tap_card(instance->targets[0].player, instance->targets[0].card);
			if( metalcraft(player, card) ){
				kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
}


int card_elesh_norn(int player, int card, event_t event ){

	check_legend_rule(player, card, event);

	vigilance(player, card, event);

	if( in_play(player, card) && ! is_humiliated(player, card, AL_STATIC) ){
		if( (event == EVENT_POWER || event == EVENT_TOUGHNESS) ){
			if( affect_me(player, card) ){
				return 0;
			}
			if( affected_card_controller == player ){
				event_result+=2;
			}
			else{
				 event_result-=2;
			}
		}
	}

	return 0;
}

int card_fresh_meat(int player, int card, event_t event){
	/* Fresh Meat	|3|G
	 * Instant
	 * Put a 3/3 |Sgreen Beast creature token onto the battlefield for each creature put into your graveyard from the battlefield this turn. */

	if( event == EVENT_RESOLVE_SPELL ){
		generate_tokens_by_id(player, card, CARD_ID_BEAST, get_dead_count(player, TYPE_CREATURE));
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_geths_verdict(int player, int card, event_t event ){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			test_definition_t test;
			new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");
			new_sacrifice(player, card, instance->targets[0].player, SAC_CAUSED | SAC_NO_CANCEL, &test);
			lose_life(instance->targets[0].player, 1);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
}

int card_gitaxian_probe(int player, int card, event_t event ){

	if( event == EVENT_MODIFY_COST ){
		return phyrexian_casting_costs(player, card, event, MANACOST_U(1), 0);
	}

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance( player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		int result = 1;
		if( ! played_for_free(player, card) && ! is_token(player, card) && instance->info_slot == 1 ){
			result = charge_phyrexian_mana(player, card, event, MANACOST_U(1), 0);
		}
		if( ! result ){
			spell_fizzled = 1;
			return 0;
		}
		ai_modifier += 24;
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
	}


	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			if( player != instance->targets[0].player ){
				reveal_target_player_hand(instance->targets[0].player);
			}
			draw_cards(player, 1);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

static const char* target_must_have_counters(int who_chooses, int player, int card, int targeting_player, int targeting_card)
{
  return count_counters(player, card, -1) ? NULL : "must have counters";
}
int card_hex_parasite(int player, int card, event_t event)
{
  CardFunction p_Var1;
  card_instance_t *pcVar2;
  int iVar3;
  counter_t type;
  int iVar4;
  card_instance_t *pcVar5;
  target_definition_t td;

  if (event == EVENT_POW_BOOST)
  {
    iVar3 = has_mana(player, 7, 1);
    iVar4 = count_counters_by_counter_and_card_type(player, ~COUNTER_P1_P1, TYPE_PERMANENT);
    if (iVar4 < iVar3)
    {
      return iVar4;
    }
    return iVar3;
  }
  if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_ACTIVATE))
  {
    return 0;
  }
  pcVar2 = get_card_instance(player, card);
  default_target_definition(player, card, &td, 0x47);
  td.preferred_controller = 2;
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    if (event == EVENT_RESOLVE_ACTIVATION)
      goto LAB_0226f6b0;
    td.special = 0x8000;
    td.extra = (int32_t)target_must_have_counters;
    if (event != EVENT_CAN_ACTIVATE)
    {
      if (event != EVENT_ACTIVATE)
      {
        return 0;
      }
      goto LAB_0226f7b6;
    }
    iVar3 = 1;
    if (ai_is_speculating == 1)
      goto LAB_0226f89a;
    if ((player == 1) && (((unsigned char)trace_mode & 2) == 0))
    {
      iVar3 = 1;
      goto LAB_0226f89a;
    }
  }
  else if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
    LAB_0226f7b6:
      pcVar2->number_of_targets = '\0';
      iVar4 = pick_target(&td, "TARGET_PERMANENT");
      iVar3 = max_x_value;
      if (iVar4 == 0)
      {
        return 0;
      }
      if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
      {
        max_x_value =
            count_counters(pcVar2->targets[0].player, pcVar2->targets[0].card, ~COUNTER_P1_P1);
      }
      charge_phyrexian_mana_for_activated_ability(player, card, -1, 1, 0, 0, 0, 0);
      max_x_value = iVar3;
      if (spell_fizzled == 1)
      {
        pcVar2->number_of_targets = '\0';
        return 0;
      }
      pcVar2->info_slot = x_value;
      return 0;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      return 0;
    }
  LAB_0226f6b0:
    iVar3 = valid_target(&td);
    if (iVar3 != 0)
    {
      iVar3 = 0;
      if (0 < pcVar2->info_slot)
      {
        do
        {
          iVar4 =
              count_counters(pcVar2->targets[0].player, pcVar2->targets[0].card, ~COUNTER_P1_P1);
          if ((iVar4 == 0) ||
              (type = choose_existing_counter_type(
                   player, player, card, pcVar2->targets[0].player, pcVar2->targets[0].card,
                   CECT_REMOVE | CECT_CONSIDER_ALL | CECT_AI_CAN_CANCEL | CECT_HUMAN_CAN_CANCEL, -1,
                   -1),
               type == COUNTER_invalid))
            break;
          iVar3 = iVar3 + 1;
          remove_counters(player, card, pcVar2->targets[0].player,
                                      pcVar2->targets[0].card, type, 1);
          p_Var1 = pcVar2->info_slot_fn;
          pcVar2->info_slot = (int32_t)(p_Var1 + -1);
        } while (0 < (int)(p_Var1 + -1));
        if ((0 < iVar3) &&
            (pcVar5 = in_play(pcVar2->parent_controller, pcVar2->parent_card), pcVar5 != NULL))
        {
          spell_fizzled = 0;
          pump_until_eot_merge_previous(player, card, pcVar2->parent_controller,
                                        pcVar2->parent_card, iVar3, 0);
          return 0;
        }
      }
      spell_fizzled = 1;
    }
    return 0;
  }
  iVar3 = 0;
LAB_0226f89a:
  iVar3 = has_phyrexian_mana_for_activated_ability(player, card, iVar3, 1, 0, 0, 0, 0);
  if (iVar3 == 0)
  {
    return 0;
  }
  iVar3 = can_target(&td);
  return (unsigned int)(iVar3 != 0);
}

int card_jin_gitaxias_core_augur(int player, int card, event_t event){

	/* Jin-Gitaxias, Core Augur	|8|U|U
	 * Legendary Creature - Praetor 5/4
	 * Flash
	 * At the beginning of your end step, draw seven cards.
	 * Each opponent's maximum hand size is reduced by seven. */

	check_legend_rule(player, card, event);

	if( current_turn == player && eot_trigger(player, card, event) ){
		draw_cards(player, 7);
		if( hand_count[player] > 7 ){
			multidiscard(player, hand_count[player] - 7, 0);
		}
	}

	if( event == EVENT_MAX_HAND_SIZE && in_play(player, card) && current_turn == 1-player && ! is_humiliated(player, card, AL_STATIC) ){
		event_result-=7;
	}

	return flash(player, card, event);
}

int card_karn_liberated(int player, int card, event_t event)
{
  typedef struct
  {
    int edh_card;
    int general1_id;
    int general2_id;
    int8_t general1_ebk;
    int8_t general2_ebk;
  } karn_commander_snapshot_t;
  STATIC_ASSERT(sizeof(karn_commander_snapshot_t) == 16, karn_native_commander_snapshot_bytes);
  STATIC_ASSERT(offsetof(karn_commander_snapshot_t, general1_ebk) == 12,
                karn_native_commander_flag_offset);
  int exiled_by_karn[2][151];

  int32_t *piVar1;
  int *piVar2;
  unsigned short uVar3;
  int32_t iVar4;
  bool bVar5;
  uint8_t uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int32_t iVar10;
  card_instance_t *pcVar11;
  card_instance_t *pcVar12;
  card_instance_t *pcVar13;
  card_instance_t *pcVar14;
  unsigned int *puVar15;
  int iVar16;
  int iVar17;
  unsigned char bVar18;
  unsigned char bVar19;
  uint32_t uVar20;
  unsigned int uVar21;
  unsigned int uVar22;
  int *piVar25;
  bool bVar27;
  unsigned int uStack_5ac;
  int iStack_5a8;
  unsigned int uStack_5a4;
  int leg;
  int idx;
  int exc[2];
  karn_commander_snapshot_t edh[2];
  target_definition_t td;
  target_definition_t td1;
  test_definition_t this_test;
  bVar18 = 0;
  check_legend_rule(player, card, event);
  if ((event != EVENT_RESOLVE_ACTIVATION) &&
      (event != EVENT_CAN_ACTIVATE && event != EVENT_ACTIVATE))
    goto LAB_0226faef;
  pcVar14 = get_card_instance(player, card);
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  default_target_definition(player, card, &td1, 0x47);
  iVar7 = can_target(&td1);
  iVar8 = 1 - player;
  iVar9 = would_validate_arbitrary_target(&td, iVar8, -1);
  uVar20 = 0;
  if ((iVar9 != 0) && (uVar20 = 0, hand_count[iVar8] != 0))
  {
    uVar20 = 0xf;
  }
  iVar9 = can_target(&td);
  iVar7 = dialog_impl(player, card, event, "\001\002", "\001\017", "Exile a card in target's hand",
                      iVar9, uVar20, 4, "Exile a permanent", iVar7, 10, 0xfffffffd,
                      "Restart the game", 1, 0x14, 0xfffffff2, 0, 0, 0, 0);
  if (event == EVENT_CAN_ACTIVATE)
  {
    if (iVar7 == 0)
    {
      return 0;
    }
    goto LAB_0226faf9;
  }
  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE)
  {
    pcVar14->number_of_targets = '\0';
    if (iVar7 == 2)
    {
      pick_target(&td1, "TARGET_PERMANENT");
    }
    else if (iVar7 == 3)
    {
      iVar7 = count_counters(player, card, COUNTER_LOYALTY);
      if ((iVar7 < 0xf) || (pcVar14->kill_code != '\0'))
      {
        iVar7 = exiledby_detach(player, card);
        pcVar14->targets[1].card = iVar7;
      }
    }
    else if (iVar7 == 1)
    {
      pick_target(&td, "TARGET_PLAYER");
    }
    goto LAB_0226faef;
  }
  if (iVar7 == 2)
  {
    iVar7 = valid_target(&td1);
    if (iVar7 != 0)
    {
      iVar7 = is_token(pcVar14->targets[0].player, pcVar14->targets[0].card);
      if ((iVar7 == 0) &&
          (pcVar11 = in_play(pcVar14->parent_controller, pcVar14->parent_card), pcVar11 != NULL))
      {
        iVar7 =
            check_special_flags2(pcVar14->targets[0].player, pcVar14->targets[0].card, 0x1000000);
        if (iVar7 == 0)
        {
          iVar10 = pcVar14->targets[0].card;
          goto LAB_0226fe3b;
        }
        pcVar11 = get_card_instance(pcVar14->parent_controller, pcVar14->parent_card);
        pcVar12 = get_card_instance(pcVar14->targets[0].player, pcVar14->targets[0].card);
        iVar7 = pcVar12->internal_card_id;
        iVar8 = get_owner(pcVar14->targets[0].player, pcVar14->targets[0].card);
        iVar9 = locate_edh_card(iVar8);
        pcVar12 = get_card_instance(iVar8, iVar9);
        iVar10 = pcVar12->targets[2].card;
        pcVar11->targets[iVar8 + 5].player = pcVar12->targets[2].player;
        pcVar11->targets[iVar8 + 5].card = iVar10;
        if (pcVar11->targets[7].card == -1)
        {
          pcVar11->targets[7].card = 0;
        }
        iVar10 = pcVar14->targets[0].card;
        if ((iVar10 == pcVar12->targets[4].player) || (pcVar12->targets[2].player == iVar7))
        {
          kill_card(pcVar14->targets[0].player, iVar10, KILL_EXILE);
          iVar9 = check_rfg(iVar8, (unsigned int)cards_data[iVar7].id);
          if ((iVar9 != 0) && ((char)pcVar12->targets[3].player == '\x04'))
          {
            exiledby_remember(pcVar14->parent_controller, pcVar14->parent_card, iVar8, iVar7, NULL,
                              NULL);
            piVar1 = &pcVar11->targets[7].card;
            *piVar1 = *piVar1 | 1 << ((char)iVar8 * '\x02' & 0x1fU);
          }
        }
        else
        {
          if ((iVar10 != pcVar12->targets[4].card) && (pcVar12->targets[2].card != iVar7))
            goto LAB_0226fe3b;
          kill_card(pcVar14->targets[0].player, iVar10, KILL_EXILE);
          iVar9 = check_rfg(iVar8, (unsigned int)cards_data[iVar7].id);
          if ((iVar9 != 0) && (*(char *)((int)&pcVar12->targets[3].player + 1) == '\x04'))
          {
            exiledby_remember(pcVar14->parent_controller, pcVar14->parent_card, iVar8, iVar7, NULL,
                              NULL);
            piVar1 = &pcVar11->targets[7].card;
            *piVar1 = *piVar1 | 1 << ((char)iVar8 * '\x02' + 1U & 0x1f);
          }
        }
      }
      else
      {
        kill_card(pcVar14->targets[0].player, pcVar14->targets[0].card, KILL_EXILE);
      }
    }
  }
  else if (iVar7 == 3)
  {
    exc[0] = 0;
    memset(exiled_by_karn, 0xff, sizeof(exiled_by_karn));
    exc[1] = 0;
    iVar7 = pcVar14->targets[1].card;
    if (iVar7 < 0)
    {
      iStack_5a8 = pcVar14->parent_controller;
      iVar7 = pcVar14->parent_card;
    }
    else
    {
      iStack_5a8 = player + -2;
    }
    leg = 0;
    idx = 0;
    while (puVar15 = (unsigned int *)exiledby_find_any(iStack_5a8, iVar7, &leg, &idx),
           puVar15 != NULL)
    {
      uVar21 = *puVar15;
      *puVar15 = 0xffffffff;
      iVar16 = (int)uVar21 >> 0x1f;
      iVar9 = exc[-iVar16];
      exc[-iVar16] = iVar9 + 1;
      exiled_by_karn[-iVar16][iVar9] = uVar21 & 0x7fffffff;
    }
    iVar7 = pcVar14->targets[1].card;
    if (-1 < iVar7)
    {
      exiledby_destroy_detached(player, iVar7);
    }
    stack_size = 0;
    iVar7 = 0;
    do
    {
      iVar9 = stack_cards[iVar7].player;
      if (((iVar9 != -1) && (iVar16 = stack_cards[iVar7].card, iVar16 != -1)) &&
          ((iVar16 != card || (iVar9 != player))))
      {
        iVar9 = check_state(iVar9, iVar16, 0x10000000);
        if (iVar9 == 0)
        {
          kill_card(stack_cards[iVar7].player, stack_cards[iVar7].card, KILL_EXILE);
        }
        else
        {
          add_state(stack_cards[iVar7].player, stack_cards[iVar7].card, 0x40000000);
        }
      }
      stack_phase[iVar7] = -1;
      iVar7 = iVar7 + 1;
    } while (iVar7 != 0x20);
    memset(edh, 0xff, sizeof(edh));
    uStack_5ac = 0;
    bVar5 = false;
    uVar21 = current_turn;
    do
    {
      uVar22 = uStack_5ac ^ uVar21;
      iVar7 = active_cards_count[uVar22] + -1;
      if (-1 < iVar7)
      {
        bVar19 = (char)uVar22 * '\x02';
        do
        {
          while (
              (pcVar11 = in_play(uVar22, iVar7),
               pcVar11 != NULL && (iVar9 = has_subtype(uVar22, iVar7, SUBTYPE_AURA), iVar9 != 0)))
          {
            put_on_top_of_deck(uVar22, iVar7);
          LAB_02270250:
            iVar7 = iVar7 + -1;
            if (iVar7 == -1)
              goto LAB_022703a8;
          }
          iVar9 = get_id(uVar22, iVar7);
          if (iVar9 != 0x3953)
            goto LAB_02270250;
          edh[uVar22].edh_card = iVar7;
          remove_all_counters(uVar22, iVar7, uVar22, iVar7, ~COUNTER_P1_P1);
          pcVar11 = get_card_instance(uVar22, iVar7);
          uVar3 = cards_data[pcVar11->targets[2].player].id;
          edh[uVar22].general1_id = (unsigned int)uVar3;
          edh[uVar22].general2_id = (unsigned int)cards_data[pcVar11->targets[2].card].id;
          uVar21 = pcVar14->targets[7].card;
          if (0 < (int)uVar21)
          {
            bVar27 = false;
            if (((1 << (bVar19 & 0x1f) & uVar21) != 0) &&
                ((char)pcVar11->targets[3].player == '\x04'))
            {
              iVar9 = check_rfg(uVar22, (unsigned int)uVar3);
              bVar27 = iVar9 != 0;
              uVar21 = pcVar14->targets[7].card;
            }
            edh[uVar22].general1_ebk = bVar27;
            bVar27 = false;
            if (((1 << (bVar19 + 1 & 0x1f) & uVar21) != 0) &&
                (*(char *)((int)&pcVar11->targets[3].player + 1) == '\x04'))
            {
              iVar9 = check_rfg(uVar22, edh[uVar22].general2_id);
              bVar27 = iVar9 != 0;
            }
            edh[uVar22].general2_ebk = bVar27;
            pcVar14->targets[7].card = 0;
          }
          *(uint8_t *)&pcVar11->targets[3].player = 1;
          if (edh[uVar22].general2_id < 0)
            goto LAB_02270250;
          iVar7 = iVar7 + -1;
          *(uint8_t *)((int)&pcVar11->targets[3].player + 1) = 1;
        } while (iVar7 != -1);
      LAB_022703a8:
        iVar7 = active_cards_count[uVar22] + -1;
        uVar21 = current_turn;
        if (-1 < iVar7)
        {
          do
          {
            while (pcVar11 = in_hand(uVar22, iVar7), pcVar11 == NULL)
            {
              pcVar11 = in_play(uVar22, iVar7);
              if ((pcVar11 != NULL) && (card != iVar7))
              {
                iVar9 = get_original_internal_card_id(uVar22, iVar7);
                uVar3 = cards_data[iVar9].id;
                iVar16 = is_what(uVar22, iVar7, 0x80);
                if (iVar16 == 0)
                {
                  uVar6 = cards_data[iVar9].cc[2];
                }
                else
                {
                  uVar6 = cards_data[iVar9].cc[2];
                  if ((uVar6 != 'c' && uVar6 != '\x03') && (uVar3 != 0x394))
                  {
                    pcVar11 = get_card_instance(uVar22, iVar7);
                    if (pcVar11->info_slot_fn != companion_effect)
                    {
                      kill_card(uVar22, iVar7, KILL_EXILE);
                    }
                    goto LAB_022703d4;
                  }
                }
                if (uVar6 == '\x03')
                {
                  if (cards_data[iVar9].id != 0x3953)
                  {
                    iVar9 = is_plane_card((unsigned int)uVar3);
                    if (iVar9 == 0)
                    {
                      remove_all_counters(uVar22, iVar7, uVar22, iVar7, ~COUNTER_P1_P1);
                    }
                    else
                    {
                      kill_card(uVar22, iVar7, KILL_EXILE);
                      bVar5 = true;
                    }
                  }
                }
                else
                {
                  iVar9 = is_what(uVar22, iVar7, 0x47);
                  if (iVar9 != 0)
                  {
                    iVar9 = check_special_flags2(uVar22, iVar7, 0x1000000);
                    if (iVar9 == 0)
                    {
                      put_on_top_of_deck(uVar22, iVar7);
                    }
                    else
                    {
                      obliterate_card(uVar22, iVar7);
                    }
                  }
                }
              }
            LAB_022703d4:
              iVar7 = iVar7 + -1;
              uVar21 = current_turn;
              if (iVar7 == -1)
                goto LAB_02270442;
            }
            if ((edh[uVar22].edh_card == -1) ||
                ((iVar9 = get_id(uVar22, iVar7),
                  edh[uVar22].general1_id != iVar9 && (edh[uVar22].general2_id != iVar9))))
            {
              put_on_top_of_deck(uVar22, iVar7);
              goto LAB_022703d4;
            }
            obliterate_card(uVar22, iVar7);
            iVar7 = iVar7 + -1;
            add_csvid_to_rfg(uVar22, iVar9);
            uVar21 = current_turn;
          } while (iVar7 != -1);
        }
      }
    LAB_02270442:
      uStack_5ac = uStack_5ac + 1;
    } while (uStack_5ac != 2);
    reshuffle_grave_into_deck(uVar21, 1);
    reshuffle_grave_into_deck(current_turn ^ 1, 1);
    iVar7 = current_turn;
    iVar9 = get_starting_life_total(current_turn);
    life[iVar7] = iVar9;
    raw_set_poison(iVar7, '\0');
    raw_set_experience(iVar7, '\0');
    uVar21 = current_turn ^ 1;
    player_counters[player].energy_ = 0;
    iVar7 = get_starting_life_total(uVar21);
    life[uVar21] = iVar7;
    raw_set_poison(uVar21, '\0');
    raw_set_experience(uVar21, '\0');
    uStack_5a4 = 0;
    player_counters[player].energy_ = 0;
    do
    {
      uVar21 = uStack_5a4 ^ current_turn;
      iVar7 = exc[uVar21];
      if (0 < iVar7)
      {
        piVar25 = exiled_by_karn[uVar21];
        do
        {
          iVar9 = is_what(-1, *piVar25, 0x47);
          if ((iVar9 == 0) || (iVar9 = has_subtype_by_id(
                                   player, (unsigned int)cards_data[*piVar25].id, SUBTYPE_AURA),
                               iVar9 != 0))
          {
            remove_card_from_rfg(uVar21, (unsigned int)cards_data[*piVar25].id);
            raw_put_iid_on_top_of_deck(uVar21, *piVar25);
            *piVar25 = -1;
          }
          piVar25 = piVar25 + 1;
        } while ((exiled_by_karn[uVar21] + iVar7) != piVar25);
      }
      uStack_5a4 = uStack_5a4 + 1;
    } while (uStack_5a4 != 2);
    stack_size = 0;
    iVar7 = 0;
    do
    {
      iVar9 = stack_cards[iVar7].player;
      if (((iVar9 != -1) && (iVar16 = stack_cards[iVar7].card, iVar16 != -1)) &&
          ((iVar9 != player || (iVar16 != card))))
      {
        iVar9 = check_state(iVar9, iVar16, 0x10000000);
        if (iVar9 == 0)
        {
          kill_card(stack_cards[iVar7].player, stack_cards[iVar7].card, KILL_EXILE);
        }
        else
        {
          add_state(stack_cards[iVar7].player, stack_cards[iVar7].card, 0x40000000);
        }
      }
      stack_phase[iVar7] = -1;
      iVar7 = iVar7 + 1;
    } while (iVar7 != 0x20);
    uStack_5ac = 0;
    uVar21 = current_turn;
    do
    {
      uVar22 = uStack_5ac ^ uVar21;
      if (0 < exc[uVar22])
      {
        piVar25 = exiled_by_karn[uVar22];
        piVar2 = (exiled_by_karn[uVar22] + exc[uVar22]);
        do
        {
          while (true)
          {
            iVar7 = *piVar25;
            uVar21 = (uint16_t)cards_data[iVar7].id |
                     ((uint32_t)(uint16_t)cards_data[iVar7].reserved2 << 16);
            *piVar25 = -1;
            if (-1 < iVar7)
              break;
          LAB_02270820:
            piVar25 = piVar25 + 1;
            uVar21 = current_turn;
            if (piVar2 == piVar25)
              goto LAB_0227093e;
          }
          uVar21 = uVar21 & 0xffff;
          iVar9 = check_rfg(uVar22, uVar21);
          if (iVar9 == 0)
            goto LAB_02270820;
          iVar7 = add_card_to_hand(player, iVar7);
          if (player != uVar22)
          {
            if (player == 1)
            {
              remove_state(1, iVar7, 0x1000);
            }
            else
            {
              add_state(player, iVar7, 0x1000);
            }
          }
          remove_card_from_rfg(uVar22, uVar21);
          if ((edh[0].edh_card & edh[1].edh_card) != 0xffffffff)
          {
            if ((edh[player].general1_id == uVar21) && (edh[player].general1_ebk != '\0'))
            {
              pcVar14 = get_card_instance(player, edh[player].edh_card);
              *(uint8_t *)&pcVar14->targets[3].player = 4;
              commander_enters_the_battlefield(player, edh[player].edh_card, player, iVar7, 1);
              put_into_play(player, iVar7);
              edh[player].general1_ebk = -1;
            }
            else if ((edh[iVar8].general1_id == uVar21) && (edh[iVar8].general1_ebk != '\0'))
            {
              pcVar14 = get_card_instance(iVar8, edh[iVar8].edh_card);
              *(uint8_t *)&pcVar14->targets[3].player = 4;
              commander_enters_the_battlefield(iVar8, edh[iVar8].edh_card, player, iVar7, 1);
              put_into_play(player, iVar7);
              edh[iVar8].general1_ebk = -1;
            }
            else if ((edh[player].general2_id == uVar21) && (edh[player].general2_ebk != '\0'))
            {
              pcVar14 = get_card_instance(player, edh[player].edh_card);
              *(uint8_t *)((int)&pcVar14->targets[3].player + 1) = 4;
              commander_enters_the_battlefield(player, edh[player].edh_card, player, iVar7, 2);
              put_into_play(player, iVar7);
              edh[player].general2_ebk = -1;
            }
            else
            {
              if ((edh[iVar8].general2_id != uVar21) || (edh[iVar8].general2_ebk == '\0'))
                goto LAB_02270919;
              pcVar14 = get_card_instance(iVar8, edh[iVar8].edh_card);
              *(uint8_t *)((int)&pcVar14->targets[3].player + 1) = 4;
              commander_enters_the_battlefield(iVar8, edh[iVar8].edh_card, player, iVar7, 2);
              put_into_play(player, iVar7);
              edh[iVar8].general2_ebk = -1;
            }
            goto LAB_02270820;
          }
        LAB_02270919:
          piVar25 = piVar25 + 1;
          put_into_play(player, iVar7);
          uVar21 = current_turn;
        } while (piVar2 != piVar25);
      }
    LAB_0227093e:
      uStack_5ac = uStack_5ac + 1;
    } while (uStack_5ac != 2);
    iVar7 = count_rfg(uVar21);
    if (-1 < iVar7)
    {
      do
      {
        iVar8 = iVar7 + -1;
        remove_card_from_rfg_by_position(uVar21, iVar7);
        iVar7 = iVar8;
      } while (iVar8 != -1);
    }
    uVar21 = current_turn ^ 1;
    iVar7 = count_rfg(uVar21);
    if (-1 < iVar7)
    {
      do
      {
        iVar8 = iVar7 + -1;
        remove_card_from_rfg_by_position(uVar21, iVar7);
        iVar7 = iVar8;
      } while (iVar8 != -1);
    }
    uStack_5a4 = 0;
    do
    {
      uVar21 = uStack_5a4 ^ current_turn;
      iVar7 = 7;
      shuffle(uVar21);
      iVar8 = active_cards_count[uVar21] + -1;
      if (-1 < iVar8)
      {
        do
        {
          while (iVar17 = get_original_internal_card_id(uVar21, iVar8), iVar16 = current_turn,
                 iVar9 = event_result, current_turn = iVar16, cards_data[iVar17].cc[2] == '\x03')
          {
            current_turn = uVar21;
            call_card_function(uVar21, iVar8, EVENT_MAX_HAND_SIZE);
            if (event_result - 1U < 0x62)
            {
              iVar7 = event_result + -7 + iVar7;
            }
            iVar8 = iVar8 + -1;
            event_result = iVar9;
            current_turn = iVar16;
            if (iVar8 == -1)
              goto LAB_02270a60;
          }
          iVar8 = iVar8 + -1;
          event_result = iVar9;
        } while (iVar8 != -1);
      }
    LAB_02270a60:
      draw_cards(uVar21, iVar7);
      uStack_5a4 = uStack_5a4 + 1;
    } while (uStack_5a4 != 2);
    memset(mana_pool, 0, 0x40);
    cards_drawn_this_turn[0] = 0;
    cards_drawn_this_turn[1] = 0;
    land_can_be_played = land_can_be_played & 0xfffffffe;
    turn_count = 0;
    lands_played = 0;
    game_startup();
    uVar21 = 0;
    do
    {
      uVar22 = current_turn ^ uVar21;
      iVar7 = active_cards_count[uVar22] + -1;
      if (-1 < iVar7)
      {
        do
        {
          pcVar14 = in_play(uVar22, iVar7);
          if (pcVar14 != NULL)
          {
            pcVar14 = get_card_instance(uVar22, iVar7);
            pcVar14->state = pcVar14->state & 0xfffcffff;
          }
          iVar7 = iVar7 + -1;
        } while (iVar7 != -1);
      }
      uVar21 = uVar21 + 1;
    } while (uVar21 != 2);
    if (bVar5)
    {
      iVar7 = get_new_plane(-1);
      planeswalk_to(player, iVar7, 1);
    }
    if (current_turn == player)
    {
      untap_phase(player);
      upkeep_phase(player);
      current_phase = 0x14;
      EXE_FN(void, 0x4377e0, int, int)(player, 0x14);
    }
    else
    {
      end_the_turn(player, card, event);
    }
  }
  else if (((iVar7 == 1) && (iVar7 = valid_target(&td), iVar7 != 0)) &&
           (iVar7 = pcVar14->targets[0].player, hand_count[iVar7] != 0))
  {
    if (iVar7 == 0)
    {
      default_test_definition(&this_test, 0xff);
      iVar10 = pcVar14->targets[0].player;
    LAB_0226fd0b:
      iVar10 = new_select_a_card(iVar10, iVar10, 4, 1, 3, -1, &this_test);
    }
    else
    {
      default_test_definition(&this_test, 0x47);
      iVar7 = pcVar14->targets[0].player;
      this_test.type_flag = 1;
      iVar10 = new_select_a_card(iVar7, iVar7, 4, 1, 3, -1, &this_test);
      if (iVar10 == -1)
      {
        default_test_definition(&this_test, 0xff);
        iVar10 = pcVar14->targets[0].player;
        goto LAB_0226fd0b;
      }
    }
    pcVar11 = in_play(pcVar14->parent_controller, pcVar14->parent_card);
    if (pcVar11 == NULL)
    {
      rfg_card_in_hand(pcVar14->targets[0].player, iVar10);
    }
    else
    {
      iVar7 = locate_edh_card(pcVar14->targets[0].player);
      if (iVar7 != -1)
      {
        pcVar11 = get_card_instance(pcVar14->parent_controller, pcVar14->parent_card);
        pcVar12 = get_card_instance(pcVar14->targets[0].player, iVar7);
        iVar7 = pcVar14->targets[0].player;
        iVar4 = pcVar12->targets[2].player;
        pcVar11->targets[iVar7 + 5].card = pcVar12->targets[2].card;
        pcVar11->targets[iVar7 + 5].player = iVar4;
        if (pcVar11->targets[7].card == -1)
        {
          pcVar11->targets[7].card = 0;
        }
        pcVar13 = get_card_instance(pcVar14->targets[0].player, iVar10);
        iVar7 = pcVar14->targets[0].player;
        if ((pcVar13->internal_card_id == pcVar12->targets[2].player) &&
            ((char)pcVar12->targets[3].player == '\x04'))
        {
          piVar1 = &pcVar11->targets[7].card;
          *piVar1 = *piVar1 | 1 << ((char)iVar7 * '\x02' & 0x1fU);
        }
        else
        {
          pcVar13 = get_card_instance(iVar7, iVar10);
          if ((pcVar13->internal_card_id == pcVar12->targets[2].card) &&
              (*(char *)((int)&pcVar12->targets[3].player + 1) == '\x04'))
          {
            piVar1 = &pcVar11->targets[7].card;
            *piVar1 = *piVar1 | 1 << ((char)pcVar14->targets[0].player * '\x02' + 1U & 0x1f);
          }
        }
      }
    LAB_0226fe3b:
      exile_card_and_remember_it_on_exiledby(pcVar14->parent_controller, pcVar14->parent_card,
                                             pcVar14->targets[0].player, iVar10);
    }
  }
LAB_0226faef:
  if ((((event == EVENT_CAST_SPELL) &&
        ((card != affected_card || (player != affected_card_controller)))) &&
       (pcVar14 = in_play(player, card),
        -1 < pcVar14->targets[affected_card_controller + 5].player)) &&
      (-1 < pcVar14->targets[7].card))
  {
    iVar7 = get_original_internal_card_id(affected_card_controller, affected_card);
    if (iVar7 == pcVar14->targets[affected_card_controller + 5].player)
    {
      uVar21 = pcVar14->targets[7].card;
      bVar18 = (char)affected_card_controller * '\x02';
      if (((int)uVar21 >> (bVar18 & 0x1f) & 1U) != 0)
      {
        bVar18 = bVar18 & 0x1f;
        pcVar14->targets[7].card = (-2 << bVar18 | 0xfffffffeU >> 0x20 - bVar18) & uVar21;
        goto LAB_0226faf9;
      }
    }
    if (iVar7 == pcVar14->targets[affected_card_controller + 5].card)
    {
      uVar21 = pcVar14->targets[7].card;
      bVar18 = (char)affected_card_controller * '\x02' + 1;
      if (((int)uVar21 >> (bVar18 & 0x1f) & 1U) != 0)
      {
        bVar18 = bVar18 & 0x1f;
        pcVar14->targets[7].card = uVar21 & (-2 << bVar18 | 0xfffffffeU >> 0x20 - bVar18);
      }
    }
  }
LAB_0226faf9:
  iVar7 = planeswalker(player, card, event, 6);
  return iVar7;
}


int card_lashwrithe(int player, int card, event_t event){

	/* Lashwrithe	|4
	 * Artifact - Equipment
	 * Living weapon
	 * Equipped creature gets +1/+1 for each |H2Swamp you control.
	 * Equip |PB|PB */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		token_generation_t token;
		default_token_definition(player, card, CARD_ID_GERM, &token);
		token.action = TOKEN_ACTION_EQUIP;
		generate_token(&token);
	}

	if( is_equipping(player, card) ){
		card_instance_t *instance = get_card_instance(player, card);
		int p = instance->damage_target_player;
		int c = instance->damage_target_card;
		if( (event== EVENT_POWER || event == EVENT_TOUGHNESS) && affect_me(p, c) ){
			event_result += count_subtype(player, TYPE_PERMANENT, SUBTYPE_SWAMP);
		}
	}

	if( event == EVENT_CAN_ACTIVATE ){
		int c1 = get_updated_equip_cost(player, card, get_cost_mod_for_activated_abilities(player, card, 0, 2, 0, 0, 0, 0));
		if( has_phyrexian_mana(player, MANACOST_XB(c1, 2), 0) && basic_equipment(player, card, event, 0) ){
			return 1;
		}
	}

	if( event == EVENT_ACTIVATE ){
		int c1 = get_updated_equip_cost(player, card, get_cost_mod_for_activated_abilities(player, card, 0, 2, 0, 0, 0, 0));
		int good = 0;
		if( metalcraft(player, card) && check_battlefield_for_id(player, CARD_ID_PURESTEEL_PALADIN) ){
			good = 1;
		}
		if( good != 1 ){
			charge_phyrexian_mana( player, card, event, MANACOST_XB(c1, 2), 0);
		}
		if( spell_fizzled != 1 ){
			return basic_equipment(player, card, event, 0);
		}
	}

	if(event == EVENT_RESOLVE_ACTIVATION ){
		return basic_equipment(player, card, event, 0);
	}

	return 0;
}

int card_master_splicer(int player, int card, event_t event ){
	/* Master Splicer	|3|W
	 * Creature - Human Artificer 1/1
	 * When ~ enters the battlefield, put a 3/3 colorless Golem artifact creature token onto the battlefield.
	 * Golem creatures you control get +1/+1. */

	return generic_splicer(player, card, event, 1, 1, 0, 1);
}

int card_melira_sylvok_outcast(int player, int card, event_t event ){

	check_legend_rule(player, card, event);

	card_instance_t *instance = get_card_instance(player, card);

	// Deal with Infect
	if(event == EVENT_ABILITIES && affected_card_controller == 1-player &&
		is_what(affected_card_controller, affected_card, TYPE_CREATURE) && !is_humiliated(player, card, AL_STATIC) )
	{
		event_result &= ~KEYWORD_INFECT;
	}

	// Deal with Poison Counters
	if( event == EVENT_RESOLVE_SPELL ){
		instance->targets[9].player = POISON_COUNTERS(player);
	}

	if( instance->targets[9].player > -1 && POISON_COUNTERS(player) != instance->targets[9].player  ){
		if( POISON_COUNTERS(player) > instance->targets[9].player && !is_humiliated(player, card, AL_STATIC) ){
			raw_set_poison(player, instance->targets[9].player);
		}
		else{
			 instance->targets[9].player = POISON_COUNTERS(player);
		}
	}

	// The interaction with -1/-1 counters is coded in counters.c

	return 0;
}

static const char* cmc_is_1(int who_chooses, int player, int card){
	if( get_cmc(player, card) == 1 ){
		return NULL;
	}
	return "must be a spell with CMC 1";
}

int card_mental_misstep(int player, int card, event_t event ){

	if( event == EVENT_MODIFY_COST ){
		return phyrexian_casting_costs(player, card, event, MANACOST_U(1), 0);
	}

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	counterspell_target_definition(player, card, &td, TYPE_ANY);
	td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
	td.extra = (int32_t)cmc_is_1;

	if( event == EVENT_CAN_CAST ){
		return counterspell(player, card, event, &td, 0);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		phyrexian_casting_costs(player, card, event, MANACOST_U(1), 0);
		if( spell_fizzled != 1 ){
			return counterspell(player, card, event, &td, 0);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		return counterspell(player, card, event, &td, 0);
	}

	return 0;
}

int card_immolating_souleater(int player, int card, event_t event);
int card_moltensteel_dragon(int player, int card, event_t event)
{
  /* Moltensteel Dragon	|4|PR|PR
   * Artifact Creature - Dragon 4/4
   * Flying
   * |PR: ~ gets +1/+0 until end of turn. */

  phyrexian_casting_costs(player, card, event, MANACOST_XR(4, 2), 1);

  return card_immolating_souleater(player, card, event);
}

int card_norns_annex(int player, int card, event_t event ){

	phyrexian_casting_costs(player, card, event, MANACOST_XW(3, 2), 1);

	tax_attack(player, card, event);

	return 0;
}

int card_noxious_revival(int player, int card, event_t event){
/* CARD_ID_NOXIOUS_REVIVAL	11647
Noxious Revival	|PG
Instant
(|PG can be paid with either |G or 2 life.)
Put target card from a graveyard on top of its owner's library. */

	if( event == EVENT_MODIFY_COST ){
		return phyrexian_casting_costs(player, card, event, MANACOST_G(1), 0);
	}

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_ANY, "Select target card to put on top of deck.");

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_GRAVE_RECYCLER_BOTH_GRAVES, NULL, NULL, 1, &this_test);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		     phyrexian_casting_costs(player, card, event, MANACOST_G(1), 0);
			 select_target_from_either_grave(player, card, 0, AI_MIN_VALUE, AI_MAX_VALUE, &this_test, 0, 1);
	}

	if( event == EVENT_RESOLVE_SPELL){
		int selected = validate_target_from_grave_source(player, card, instance->targets[0].player, 1);
		if( selected != -1 ){
			from_graveyard_to_deck(instance->targets[0].player, selected, 1);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_phyrexian_metamorph(int player, int card, event_t event)
{
  /* Phyrexian Metamorph	|3|PU
   * Artifact Creature - Shapeshifter 0/0
   * You may have ~ enter the battlefield as a copy of any artifact or creature on the battlefield, except it's an artifact in addition to its other types. */

  phyrexian_casting_costs(player, card, event, MANACOST_XU(3,1), 1);

  target_definition_t td;
  if (event == EVENT_RESOLVE_SPELL)
	base_target_definition(player, card, &td, TYPE_CREATURE | TYPE_ARTIFACT);

  enters_the_battlefield_as_copy_of_any(player, card, event, &td, "SELECT_AN_ARTIFACT_OR_CREATURE");
  // type artifact added in cloning()

  return 0;
}

int card_phyrexian_obliterator(int player, int card, event_t event ){
/*
Phyrexian Obliterator |B|B|B|B
Creature — Horror 5/5
Trample
Whenever a source deals damage to Phyrexian Obliterator, that source's controller sacrifices that many permanents.
*/
	// An approximation - tells the AI that anything with at least 1 power that blocks this or is blocked by this will be destroyed.
	if (event == EVENT_CHANGE_TYPE && affect_me(player, card) && !is_humiliated(player, card, AL_TRIG)){
		get_card_instance(player, card)->destroys_if_blocked |= DIFB_ASK_CARD;
	}
	if (event == EVENT_CHECK_DESTROY_IF_BLOCKED && affect_me(player, card) && !is_humiliated(player, card, AL_TRIG)
		&& get_power(attacking_card_controller, attacking_card) >= 1){
		event_result |= 1;
	}

	if( event == EVENT_DEAL_DAMAGE){
		card_instance_t *instance = get_card_instance(player, card);
		card_instance_t *damage= get_card_instance( affected_card_controller, affected_card);
		if( damage->internal_card_id == damage_card && instance->info_slot != current_phase ){
			if( damage->damage_target_card == card && damage->damage_target_player == player && damage->info_slot > 0 ){
				if( instance->targets[8].player < 0 ){
					instance->targets[8].player = 0;
				}
				instance->targets[8].player++;

				int p = damage->damage_source_player;
				if( instance->targets[p+1].player < 0 ){
					instance->targets[p+1].player = 0;
				}
				instance->targets[p+1].player += damage->info_slot;
			}
		}
	}

	if( resolve_damage_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY, player, card) ){
		card_instance_t *instance = get_card_instance(player, card);
		APNAP(p,{
					if( instance->targets[p+1].player > 0 ){
						int amount = instance->targets[p+1].player;
						instance->targets[p+1].player = 0;

						test_definition_t test;
						default_test_definition(&test, TYPE_PERMANENT);
						test.qty = amount;
						if( amount == 1 ){
							scnprintf(test.message, 100, "Select a permanent to sacrifice.");
						}
						else{
							scnprintf(test.message, 100, "Select %d permanents to sacrifice.", amount);
						}
						new_sacrifice(player, card, p, SAC_CAUSED | SAC_NO_CANCEL, &test);
					}
				};
		);
	}


	return 0;
}

int card_phyrexian_swarmlord(int player, int card, event_t event){

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		token_generation_t token;
		default_token_definition(player, card, CARD_ID_INSECT, &token);
		token.legacy = 1;
		token.special_code_for_legacy = &empty;
		token.special_infos = 67;
		token.qty = POISON_COUNTERS(1-player);
		generate_token(&token);
	}

	return 0;
}

int card_phyrexian_unlife(int player, int card, event_t event)
{
  cannot_lose_the_game_for_having_less_than_0_life(player, card, event, player);

  card_instance_t* damage = damage_being_dealt(event), *instance;
  if (damage
	  && damage->damage_target_card == -1 && damage->damage_target_player == player
	  && (instance = in_play(player, card))
	  && instance->targets[5].player > 0)
	damage->regen_status |= KEYWORD_INFECT;

  return global_enchantment(player, card, event);
}

int card_postmortem_lunge(int player, int card, event_t event){

	if( event == EVENT_MODIFY_COST ){
		return phyrexian_casting_costs(player, card, event, MANACOST_B(1), 0);
	}

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_CREATURE, "Select a creature card");

	card_instance_t *instance= get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		if( generic_spell(player, card, event, GS_X_SPELL, NULL, NULL, 0, NULL) ){
			int mod = true_get_updated_casting_cost(player, card, -1, event, -1);
			int count = 0;
			int iid;
			while( (iid = get_grave(player)[count]) > -1 ){
					if( is_what(-1, iid, TYPE_CREATURE) ){
						if( has_phyrexian_mana(player, card, MANACOST_XB(get_cmc_by_internal_id(iid)-mod, 1)) ){
							return 1;
						}
					}
					count++;
			}
		}
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		int mod = true_get_updated_casting_cost(player, card, -1, event, -1);
		int available[2][100];
		int ac = 0;
		int count = 0;
		int iid;
		while( (iid = get_grave(player)[count]) > -1 ){
				if( is_what(-1, iid, TYPE_CREATURE) ){
					if( has_phyrexian_mana(player, card, MANACOST_XB(get_cmc_by_internal_id(iid)-mod, 1)) ){
						available[0][ac] = iid;
						available[1][ac] = count;
						ac++;
					}
				}
				count++;
		}
		int selected = select_card_from_zone(player, player, available[0], ac, 0, AI_MAX_CMC, -1, &this_test);
		if( selected == -1 ){
			spell_fizzled = 1;
			return 0;
		}
		instance->targets[0].player = available[1][selected];
		instance->targets[0].card = graveyard_source[player][available[1][selected]];
		charge_phyrexian_mana(player, card, event, MANACOST_XB(get_cmc_by_internal_id(available[0][selected]), 1), 0);
	}

	if( event == EVENT_RESOLVE_SPELL){
		int selected = validate_target_from_grave_source(player, card, player, 0);
		if( selected != -1 ){
			reanimate_permanent(player, card, player, selected, REANIMATE_HASTE_AND_EXILE_AT_EOT);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_puresteel_paladin(int player, int card, event_t event ){

	if( trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card) ){
		test_definition_t test;
		default_test_definition(&test, TYPE_PERMANENT);
		test.subtype = SUBTYPE_EQUIPMENT;
		etb_ability_global(player, event, RESOLVE_TRIGGER_AI(player), 0, &test, player, card);
	}

	if( event == EVENT_ETB_ABILITY ){
		draw_cards(player, 1);
	}

	return 0;
}

int card_sheoldred_whispering_one(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	upkeep_trigger_ability(player, card, event, 2);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		if( current_turn == player && count_graveyard_by_type(player, TYPE_CREATURE) > 0 && ! graveyard_has_shroud(2) ){
			test_definition_t this_test;
			new_default_test_definition(&this_test, TYPE_CREATURE, "Select target creature card.");
			this_test.ai_selection_mode = AI_MAX_CMC;

			new_global_tutor(player, player, TUTOR_FROM_GRAVE, TUTOR_PLAY, 1, AI_MAX_CMC, &this_test);
		}
		if( current_turn != player ){
			test_definition_t this_test;
			new_default_test_definition(&this_test, TYPE_CREATURE, "Select a creature to sacrifice.");
			new_sacrifice(player, card, current_turn, SAC_CAUSED | SAC_NO_CANCEL, &this_test);
		}
	}

	return 0;
}

int card_birthing_pod(int player, int card, event_t event){
/* Birthing Pod	|3|PG
 * Artifact
 * |1|PG, |T, Sacrifice a creature: Search your library for a creature card with converted mana cost equal to 1 plus the sacrificed
 *		creature's converted mana cost, put that card onto the battlefield, then shuffle your library.
 *		Activate this ability only any time you could cast a sorcery. */

	phyrexian_casting_costs(player, card, event, MANACOST_XG(3, 1), 1);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( has_phyrexian_mana_for_activated_ability(player, card, MANACOST_XG(1,1)) ){
			return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_SACRIFICE_CREATURE |
											GAA_DO_NOT_RECALCULATE_ACTIVATION_COST, MANACOST0, 0, NULL, NULL);
		}
	}

	if( event == EVENT_ACTIVATE){
		if( charge_phyrexian_mana_for_activated_ability(player, card, MANACOST_XG(1,1)) ){
			int sac = new_sacrifice(player, card, player, SAC_AS_COST | SAC_JUST_MARK | SAC_RETURN_CHOICE, &test);
			if( sac ){
				add_state(player, card, STATE_TAPPED);
				instance->info_slot = get_cmc(BYTE2(sac), BYTE3(sac));
				if( instance->info_slot < 6 ){
					ai_modifier += (instance->info_slot * 8);
				}
				kill_card(BYTE2(sac), BYTE3(sac), KILL_SACRIFICE);
			}
			else{
				spell_fizzled = 1;
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int amount = instance->info_slot;

		test_definition_t test2;
		default_test_definition(&test2, TYPE_CREATURE);
		test2.cmc = amount+1;
		scnprintf(test2.message, 100, "Select a creature card with CMC %d.", amount + 1);

		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_PLAY, 0, AI_MAX_VALUE, &test2);
	}

	return 0;
}

int card_shrine_of_boundless_growth(int player, int card, event_t event){

	/* Shrine of Boundless Growth	|3
	 * Artifact
	 * At the beginning of your upkeep or whenever you cast a |Sgreen spell, put a charge counter on ~.
	 * |T, Sacrifice ~: Add |1 to your mana pool for each charge counter on ~. */

	np_shrines(player, card, event, COLOR_TEST_GREEN);

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_SACRIFICE_ME, MANACOST0, 0, NULL, NULL);
	}

	if( event == EVENT_ACTIVATE ){
		int amount = count_counters(player, card, COUNTER_CHARGE);
		if( amount < 3 ){
			ai_modifier -= 48;
		}
		generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_SACRIFICE_ME, MANACOST0, 0, NULL, NULL);
		if( spell_fizzled != 1 ){
			produce_mana_tapped(player, card, COLOR_COLORLESS, amount);
		}
	}

	if (event == EVENT_COUNT_MANA && affect_me(player, card) && mana_producer(player, card, EVENT_CAN_ACTIVATE) ){
		int counters = count_counters(player, card, COUNTER_CHARGE);
		if (counters > 0){
			declare_mana_available(player, COLOR_COLORLESS, counters);
		}
	}

	return 0;
}

int card_shrine_of_burning_rage(int player, int card, event_t event ){
	/* Shrine of Burning Rage	|2	0x2004125
	 * Artifact
	 * At the beginning of your upkeep or whenever you cast a |Sred spell, put a charge counter on ~.
	 * |3, |T, Sacrifice ~: It deals damage equal to the number of charge counters on it to any target. */
	np_shrines(player, card, event, COLOR_TEST_RED);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_ACTIVATE ){
		instance->info_slot = count_counters(player, card, COUNTER_CHARGE);
		if( instance->info_slot < 3 ){
			ai_modifier -= 48;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			damage_target0(player, card, instance->info_slot);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_SACRIFICE_ME | GAA_CAN_TARGET, MANACOST_X(3), 0,
									&td, "TARGET_ANY");
}

int card_shrine_of_limitless_power(int player, int card, event_t event ){
	/* Shrine of Limitless Power	|3
	 * Artifact
	 * At the beginning of your upkeep or whenever you cast a |Sblack spell, put a charge counter on ~.
	 * |4, |T, Sacrifice ~: Target player discards a card for each charge counter on ~. */

	np_shrines(player, card, event, COLOR_TEST_BLACK);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_ACTIVATE ){
		instance->info_slot = count_counters(player, card, COUNTER_CHARGE);
		if( instance->info_slot < 3 ){
			ai_modifier -= 48;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			new_multidiscard(instance->targets[0].player, instance->info_slot, 0, player);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_SACRIFICE_ME | GAA_CAN_TARGET, MANACOST_X(4), 0,
									&td, "TARGET_PLAYER");
}


int card_shrine_of_loyal_legions(int player, int card, event_t event ){
	/* Shrine of Loyal Legions	|2
	 * Artifact
	 * At the beginning of your upkeep or whenever you cast a |Swhite spell, put a charge counter on ~.
	 * |3, |T, Sacrifice ~: Put a 1/1 colorless Myr artifact creature token onto the battlefield for each charge counter on ~. */

	np_shrines(player, card, event, COLOR_TEST_WHITE);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_ACTIVATE ){
		instance->info_slot = count_counters(player, card, COUNTER_CHARGE);
		if( instance->info_slot < 3 ){
			ai_modifier -= 48;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		token_generation_t token;
		default_token_definition(player, card, CARD_ID_MYR, &token);
		token.qty = instance->info_slot;
		generate_token(&token);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_SACRIFICE_ME, MANACOST_X(3), 0, NULL, NULL);
}

int card_shrine_of_piercing_vision(int player, int card, event_t event ){
	/* Shrine of Piercing Vision	|2
	 * Artifact
	 * At the beginning of your upkeep or whenever you cast a |Sblue spell, put a charge counter on ~.
	 * |T, Sacrifice ~: Look at the top X cards of your library, where X is the number of charge counters on ~. Put one of those cards into your hand and the
	 * rest on the bottom of your library in any order. */

	np_shrines(player, card, event, COLOR_TEST_BLUE);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_ACTIVATE ){
		instance->info_slot = count_counters(player, card, COUNTER_CHARGE);
		if( instance->info_slot < 3 ){
			ai_modifier -= 48;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		impulse_effect(player, instance->info_slot, 0);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_SACRIFICE_ME, MANACOST0, 0, NULL, NULL);
}

int card_spined_thopter(int player, int card, event_t event ){

	phyrexian_casting_costs(player, card, event, 2, 0, 1, 0, 0, 0, 1);

	return 0;
}

int card_sword_of_war_and_peace(int player, int card, event_t event)
{
  /* Sword of War and Peace	|3
   * Artifact - Equipment
   * Equipped creature gets +2/+2 and has protection from |Sred and from |Swhite.
   * Whenever equipped creature deals combat damage to a player, ~ deals damage to that player equal to the number of cards in his or her hand and you gain 1
   * life for each card in your hand.
   * Equip |2 */

   if (equipped_creature_deals_damage(player, card, event, DDBM_MUST_BE_COMBAT_DAMAGE|DDBM_MUST_DAMAGE_PLAYER|DDBM_TRACE_DAMAGED_PLAYERS))
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  int times_damaged[2] = { BYTE0(instance->targets[1].player), BYTE1(instance->targets[1].player) };
	  instance->targets[1].player = 0;
	  int p;
	  for (p = 0; p <= 1; ++p)
		for (; times_damaged[p] > 0; --times_damaged[p])
		  {
			damage_player(p, hand_count[p], player, card);
			gain_life(player, hand_count[player]);
		  }
	}

	return vanilla_equipment(player, card, event, 2, 2, 2, KEYWORD_PROT_RED | KEYWORD_PROT_WHITE, 0);
}

int card_surgical_extraction(int player, int card, event_t event){

	/* Surgical Extraction	|PB
	 * Instant
	 * Choose target card in a graveyard other than a basic land card. Search its owner's graveyard, hand, and library for any number of cards with the same
	 * name as that card and exile them. Then that player shuffles his or her library. */

	if( event == EVENT_MODIFY_COST ){
		phyrexian_casting_costs(player, card, event, MANACOST_B(1), 0);
	}

	if (!IS_CASTING(player, card, event)){
		return 0;
	}

	card_instance_t *instance= get_card_instance(player, card);

	test_definition_t this_test;
	new_default_test_definition(&this_test, 0, "Select target card other than a basic land.");
	this_test.subtype = SUBTYPE_BASIC;
	this_test.subtype_flag = 1;

	if( event == EVENT_CAN_CAST ){
		if( IS_AI(player) ){
			return generic_spell(player, card, event, GS_GRAVE_RECYCLER_OPP_GRAVE, NULL, NULL, 1, &this_test);
		}
		else{
			return generic_spell(player, card, event, GS_GRAVE_RECYCLER_BOTH_GRAVES, NULL, NULL, 1, &this_test);
		}
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		phyrexian_casting_costs(player, card, event, MANACOST_B(1), 0);
		if( spell_fizzled != 1 ){
			if( IS_AI(player) ){
				return generic_spell(player, card, event, GS_GRAVE_RECYCLER_OPP_GRAVE, NULL, NULL, 1, &this_test);
			}
			else{
				return generic_spell(player, card, event, GS_GRAVE_RECYCLER_BOTH_GRAVES, NULL, NULL, 1, &this_test);
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL){
		if( IS_AI(player) ){
			int selected = validate_target_from_grave_source(player, card, 1-player, 0);
			if( selected != -1 ){
				int id = cards_data[get_grave(1-player)[selected]].id;
				lobotomy_effect(player, 1-player, id, -1, 0);
			}
		}
		else{
			int selected = validate_target_from_grave_source(player, card, instance->targets[0].player, 1);
			if( selected != -1 ){
				const int *grave = get_grave(instance->targets[0].player);
				int id = cards_data[grave[selected]].id;
				lobotomy_effect(player, instance->targets[0].player, id, -1, 0);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_tezzerets_gambit(int player, int card, event_t event ){

	if( event == EVENT_RESOLVE_SPELL ){
		draw_cards(player, 2);
		proliferate(player, card);
		kill_card(player, card, KILL_DESTROY);
	}

	return phyrexian_casting_costs(player, card, event, MANACOST_XU(3, 1), 0);
}


int card_urabrask_the_hidden(int player, int card, event_t event){

	/* Urabrask the Hidden	|3|R|R
	 * Legendary Creature - Praetor 4/4
	 * Creatures you control have haste.
	 * Creatures your opponents control enter the battlefield tapped. */

	check_legend_rule(player, card, event);

	boost_subtype(player, card, event, -1, 0,0, 0,SP_KEYWORD_HASTE, BCT_CONTROLLER_ONLY|BCT_INCLUDE_SELF);

	permanents_enters_battlefield_tapped(player, card, event, 1-player, TYPE_CREATURE, NULL);

	return 0;
}

int card_vault_skirge(int player, int card, event_t event ){

	phyrexian_casting_costs(player, card, event, MANACOST_XB(1, 1), 1);

	lifelink(player, card, event);

	return 0;
}

int card_vital_splicer(int player, int card, event_t event){
	/* Vital Splicer	|3|G
	 * Creature - Human Artificer 1/1
	 * When ~ enters the battlefield, put a 3/3 colorless Golem artifact creature token onto the battlefield.
	 * |1: Regenerate target Golem you control. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_PERMANENT);
	td.preferred_controller = player;
	td.required_subtype = SUBTYPE_GOLEM;
	td.required_state = TARGET_STATE_DESTROYED;
	if( player == AI ){
		td.special = TARGET_SPECIAL_REGENERATION;
	}

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( land_can_be_played & LCBP_REGENERATION ){
			if( generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_X(1), 0, &td, NULL) ){
				return 99;
			}
		}
	}

	if( event == EVENT_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST_X(1), 0,
										&td, "Select target Golem.");
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) && can_be_regenerated(instance->targets[0].player, instance->targets[0].card) ){
			regenerate_target( instance->targets[0].player, instance->targets[0].card );
		}
	}

	return generic_splicer(player, card, event, 0, 0, 0, 1);
}

int card_volt_charge(int player, int card, event_t event ){
	/* Volt Charge	|2|R	0x20037b6
	 * Instant
	 * ~ deals 3 damage to any target. Proliferate. */
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_target0(player, card, 3);
			proliferate(player, card);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ANY", 1, NULL);
}

int card_vorinclex_voice_of_hunger(int player, int card, event_t event ){
/* CARD_ID_VORINCLEX_VOICE_OF_HUNGER	11713
Vorinclex, Voice of Hunger	|6|G|G
Legendary Creature - Praetor 7/6
Trample
Whenever you tap a land for mana, add one mana to your mana pool of any type that land produced.
Whenever an opponent taps a land for mana, that land doesn't untap during its controller's next untap step. */

	check_legend_rule(player, card, event);

	if( in_play(player, card) && ! is_humiliated(player, card, AL_TRIG) ){
		if( event == EVENT_TAP_CARD && tapped_for_mana_color >= 0 && is_what(affected_card_controller, affected_card, TYPE_LAND) )
		{
			if( affected_card_controller == player ){
				card_mana_flare(affected_card_controller, affected_card, EVENT_TAP_CARD);
			}
			else{
				does_not_untap_effect(player, card, affected_card_controller, affected_card, 0, 1);
			}
		}

		if( event == EVENT_COUNT_MANA && affected_card_controller == player && is_what(affected_card_controller, affected_card, TYPE_LAND)&&
			! is_tapped(affected_card_controller, affected_card) )
		{
			mana_producer(affected_card_controller, affected_card, EVENT_COUNT_MANA);
		}
	}

	return 0;
}

int card_wing_splicer(int player, int card, event_t event ){
	/* Wing Splicer	|3|U
	 * Creature - Human Artificer 1/1
	 * When ~ enters the battlefield, put a 3/3 colorless Golem artifact creature token onto the battlefield.
	 * Golem creatures you control have flying. */

	return generic_splicer(player, card, event, 0, 0, KEYWORD_FLYING, 1);
}

int card_act_of_aggression(int player, int card, event_t event){

	if( event == EVENT_MODIFY_COST ){
		phyrexian_casting_costs(player, card, event, MANACOST_XR(3, 2), 0);
	}

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.allowed_controller = td.preferred_controller = 1-player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		int life_before = life[player];
		phyrexian_casting_costs(player, card, event, MANACOST_XR(3, 2), 0);
		if( spell_fizzled != 1 ){
			td.allow_cancel = (life[player] < life_before ? 0 : 1);
			generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td, "Select target creature your opponent controls",
						1, NULL);
			if( spell_fizzled != 1 ){
				ai_modifier_for_act_of_treason(player, card);
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			effect_act_of_treason(player, card, instance->targets[0].player, instance->targets[0].card);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_apostles_blessing(int player, int card, event_t event){

	/* Apostle's Blessing	|1|PW
	 * Instant
	 * Target artifact or creature you control gains protection from artifacts or from the color of your choice until end of turn. */

	if( event == EVENT_MODIFY_COST ){
		phyrexian_casting_costs(player, card, event, MANACOST_XW(1, 1), 0);
	}

	if (!IS_CASTING(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE | TYPE_ARTIFACT);
	td.allowed_controller = player;
	td.preferred_controller = player;
	td.allow_cancel = 0;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, NULL, 1, NULL);

	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		int life_before = life[player];
		phyrexian_casting_costs(player, card, event, MANACOST_XW(1, 1), 0);
		if( spell_fizzled != 1 ){
			if( life_before > life[player] ){
				td.allow_cancel = 0;
				return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td,
									"Select target artifact or creature you control", 1, NULL);
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			int choice = do_dialog(player, player, card, -1, -1,
									" Prot. from Black\n Prot. from Blue\n Prot. from Green\n Prot from Red\n Prot. from White\n Prot from Artifacts",
									get_deck_color(player, 1-player) - 1);
			int key = (1 << (11+choice)) | KEYWORD_RECALC_SET_COLOR;
			pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 0, 0, key, 0);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

static int arm_with_aether_legacy(int player, int card, event_t event)
{

  if (get_card_instance(player, card)->damage_target_player > -1)
  {
    card_instance_t *instance = get_card_instance(player, card);

    int p = instance->damage_target_player;
    int c = instance->damage_target_card;

    card_instance_t *damage = damage_being_dealt(event);
    if (damage)
    {
      if (damage->damage_source_player == p && damage->damage_source_card == c)
      {
        if (damage->damage_target_player == 1 - p && damage->damage_target_card == -1 &&
            !damage_is_to_planeswalker(damage))
        {
          if (instance->targets[8].player < 0)
          {
            instance->targets[8].player = 0;
          }
          instance->targets[8].player++;
        }
      }
    }

    if (resolve_damage_trigger(player, card, event, !duh_mode(player), player, card))
    {
      target_definition_t td;
      default_target_definition(player, card, &td, TYPE_CREATURE);
      td.allowed_controller = td.preferred_controller = 1 - player;
      td.illegal_abilities = get_protections_from(p, c);
      if (can_target(&td) && new_pick_target(&td, "TARGET_CREATURE", 0, 0))
      {
        bounce_permanent(instance->targets[0].player, instance->targets[0].card);
      }
      instance->number_of_targets = 0;
    }
  }

  if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
  }

  return 0;
}

int card_arm_with_aether(int player, int card, event_t event){
/*
Arm with Æther |2|U
Sorcery
Until end of turn, creatures you control gain "Whenever this creature deals damage to an opponent,
		you may return target creature that player controls to its owner's hand."
*/
	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		if( current_phase < PHASE_AFTER_BLOCKING ){
			ai_modifier += (count_subtype(player, TYPE_CREATURE, -1) * 8);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		create_legacy_effect(player, card, &arm_with_aether_legacy);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}


int card_artillerize(int player, int card, event_t event){
	/* Artillerize	|3|R	0x2004625
	 * Instant
	 * As an additional cost to cast this spell, sacrifice an artifact or creature.
	 * ~ deals 5 damage to any target. */
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_CREATURE | TYPE_ARTIFACT, "Select an artifact or creature to sacrifice.");

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_CAN_CAST && new_can_sacrifice_as_cost(player, card, &test) ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ANY", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		get_card_instance(player, card)->number_of_targets = 0;
		int sac = new_sacrifice(player, card, player, SAC_AS_COST | SAC_JUST_MARK | SAC_RETURN_CHOICE, &test);
		if( sac ){
			state_untargettable(BYTE2(sac), BYTE3(sac), 0);
			generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ANY", 1, NULL);
			if( spell_fizzled != 1 ){
				kill_card(BYTE2(sac), BYTE3(sac), KILL_SACRIFICE);
			}
		}
		else{
			spell_fizzled = 1;
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_target0(player, card, 5);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_auriok_survivors(int player, int card, event_t event){
/* Auriok Survivors	|5|W
 * Creature - Human Soldier 4/6
 * When ~ enters the battlefield, you may return target Equipment card from your graveyard to the battlefield.
 * If you do, you may attach it to ~. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_AI(player));

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_PERMANENT, "Select target Equipment card.");
		test.subtype = SUBTYPE_EQUIPMENT;
		if( ! graveyard_has_shroud(player) && new_special_count_grave(player, &test) ){
			int equip = new_global_tutor(player, player, TUTOR_FROM_GRAVE, TUTOR_PLAY, 0, AI_MAX_CMC, &test);
			if( equip == -1 ){
				return 0;
			}
			int priority_equip = 10;
			if( check_for_ability(player, card, KEYWORD_DEFENDER) && check_for_special_ability(player, card, SP_KEYWORD_CANNOT_BLOCK) ){
				priority_equip = 1;
			}
			int choice = DIALOG(player, card, EVENT_ACTIVATE, DLG_RANDOM, DLG_FULLCARD(player, equip), DLG_NO_STORAGE, DLG_NO_CANCEL,
								"Attach to Auriok Survivors", 1, priority_equip,
								"Decline", 1, 5);
			if( choice == 1 ){
				equip_target_creature(player, equip, player, card);
			}
		}
	}

	return 0;
}

int card_blind_zealot(int player, int card, event_t event){
/*
Blind Zealot |1|B|B
Creature — Human Cleric 2/2
Intimidate (This creature can't be blocked except by artifact creatures and/or creatures that share a color with it.)
Whenever Blind Zealot deals combat damage to a player, you may sacrifice it. If you do, destroy target creature that player controls.
*/
	intimidate(player, card, event);

	int result = whenever_i_deal_damage_to_a_player(player, card, event, DDBM_MUST_BE_COMBAT_DAMAGE | RESOLVE_TRIGGER_AI(player));
	if( result && ! check_special_flags2(player, card, SF2_CANNOT_BE_SACRIFICED) ){
		int times_damaged[2] = {BYTE0(result), BYTE1(result)};
		APNAP(p,{
					if( times_damaged[p] && (! IS_AI(player) || p != player) ){
						target_definition_t td;
						default_target_definition(player, card, &td, TYPE_CREATURE);
						td.allowed_controller = td.preferred_controller = p;

						char msg[100];
						if( p == player ){
							scnprintf(msg, 100, "Select target creature you control.");
						}
						else{
							scnprintf(msg, 100, "Select target creature your opponent controls.");
						}
						card_instance_t *instance = get_card_instance(player, card);

						if( new_pick_target(&td, msg, 0, GS_LITERAL_PROMPT) ){
							kill_card(player, card, KILL_SACRIFICE);
							kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
							return 0;
						}
					}
				}
		);
	}

	return 0;
}

int card_blinding_souleater(int player, int card, event_t event){

	/* Blinding Souleater	|3
	 * Artifact Creature - Cleric 1/3
	 * |PW, |T: Tap target creature. */

	if (!IS_ACTIVATING(event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( has_phyrexian_mana_for_activated_ability(player, card, MANACOST_W(1)) ){
			return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET | GAA_DO_NOT_RECALCULATE_ACTIVATION_COST,
											MANACOST0, 0, &td, NULL);
		}
	}

	if (event == EVENT_ACTIVATE){
		instance->number_of_targets = 0;
		int life_before = life[player];
		if( charge_phyrexian_mana_for_activated_ability(player, card, MANACOST_W(1)) ){
			td.allow_cancel = (life_before < life[player] ? 0 : 1);
			return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET | GAA_DO_NOT_RECALCULATE_ACTIVATION_COST,
											MANACOST0, 0, &td, "TARGET_CREATURE");
		}
	}

	if (event == EVENT_RESOLVE_ACTIVATION){
		if (valid_target(&td)){
			tap_card(instance->targets[0].player, instance->targets[0].card);
		}
	}

	return 0;
}

int card_brutalizer_exarch(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_PERMANENT);
		td.illegal_type = TYPE_CREATURE;
		td.allow_cancel = 0;

		card_instance_t *instance = get_card_instance(player, card);

		int choice = DIALOG(player, card, EVENT_ACTIVATE, DLG_RANDOM, DLG_AUTOCHOOSE_IF_1, DLG_NO_STORAGE, DLG_NO_CANCEL,
							"Tutor Creature", 1, 1,
							"Remove permanent", can_target(&td), 1);
		if( choice == 1 ){
			test_definition_t test;
			new_default_test_definition(&test, TYPE_CREATURE, "Select a creature card.");
			new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_DECK, 0, AI_MAX_VALUE, &test);
		}
		if( choice == 2 ){
			if( new_pick_target(&td, "Select target non-creature permanent.", 0, GS_LITERAL_PROMPT) ){
				put_on_bottom_of_deck(instance->targets[0].player, instance->targets[0].card);
			}
		}
	}

	return 0;
}

int card_caged_sun(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  card_data_t *pcVar4;
  unsigned int uVar5;

  pcVar1 = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = choose_a_color_and_show_legacy(player, card, player, -1);
    pcVar1->targets[9].player = result;
    pcVar1->targets[9].card = 1 << ((unsigned char)result & 0x1f);
  }
  pcVar2 = in_play(player, card);
  if (pcVar2 != NULL)
  {
    result = is_humiliated(player, card, AL_TRIG);
    if (((event - EVENT_COUNT_MANA & 0xfffffffd) == 0 && result == 0) &&
        (affected_card_controller == player))
    {
      result = is_what(player, affected_card, 1);
      if (result != 0)
      {
        pcVar4 = get_card_data(affected_card_controller, affected_card);
        if (event == EVENT_TAP_CARD)
        {
          result = pcVar1->targets[9].player;
          if ((-1 < result) &&
              ((result == tapped_for_mana_color ||
                ((tapped_for_mana_color == 0x100 && (0 < tapped_for_mana[result]))))))
          {
            produce_mana(affected_card_controller, result, 1);
          }
        }
        else if ((pcVar1->targets[9].card & (unsigned int)pcVar4->color) != 0)
        {
          result = is_tapped(affected_card_controller, affected_card);
          if (result == 0)
          {
            result = is_animated_and_sick(affected_card_controller, affected_card);
            if (result == 0)
            {
              result = can_produce_mana(affected_card_controller, affected_card);
              if (result != 0)
              {
                declare_mana_available(affected_card_controller, pcVar1->targets[9].player, 1);
              }
            }
          }
        }
      }
    }
  }
  pcVar2 = in_play(player, card);
  if (pcVar2 != NULL)
  {
    result = is_humiliated(player, card, AL_STATIC);
    if (((result == 0) && (event - EVENT_POWER < 2)) && (affected_card_controller == player))
    {
      uVar5 = get_color(player, affected_card);
      if ((pcVar1->targets[9].card & uVar5) != 0)
      {
        event_result = event_result + 1;
      }
    }
  }
  return 0;
}

int card_caress_of_phyrexia(int player, int card, event_t event){
/*
Caress of Phyrexia |3|B|B
Sorcery
Target player draws three cards, loses 3 life, and gets three poison counters.
*/

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;
	td.preferred_controller = ANYBODY;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
		if( spell_fizzled != 1 ){
			if( instance->targets[0].player == player ){
				if( life[player]-3 < 6 || POISON_COUNTERS(player)+3 >= 6 ){
					ai_modifier -= 96;
				}
				else{
					ai_modifier += 24;
				}
			}
			else{
				if( life[1-player] <= 3|| POISON_COUNTERS(1-player) >= 7 ){
					ai_modifier += 96;
				}
				else{
					ai_modifier -= 96;
				}
			}
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			draw_cards(instance->targets[0].player, 3);
			lose_life(instance->targets[0].player, 3);
			poison(instance->targets[0].player, 3);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
}

int card_chained_throatseeker(int player, int card, event_t event)
{
	if (event == EVENT_ATTACK_LEGALITY && affect_me(player, card) && !is_poisoned(1-player) && ! is_humiliated(player, card, AL_STATIC) )
		event_result = 1;

	return 0;
}

int card_coa_special_effect(int player, int card, event_t event){

	if( new_specific_spell_played(player, card, event, 1-player, RESOLVE_TRIGGER_MANDATORY, NULL) ){
		int kill = 1;
		if( has_mana(1-player, COLOR_ANY, 1) && charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, 1-player, COLOR_ANY, 1) ){
			kill = 0;
		}
		if( kill ){
			real_counter_a_spell(player, card, trigger_cause_controller, trigger_cause);
		}
		if( is_what(player, card, TYPE_EFFECT) ){
			kill_card(player, card, KILL_EXILE);
		}
	}

	return 0;
}

int card_chancellor_of_the_annex(int player, int card, event_t event){
	/* Chancellor of the Annex	|4|W|W|W
	 * Creature - Angel 5/6
	 * You may reveal this card from your opening hand. If you do, when each opponent casts his or her first spell of the game, counter that spell unless that player pays |1.
	 * Flying
	 * Whenever an opponent casts a spell, counter it unless that player pays |1. */

	return card_coa_special_effect(player, card, event);
}

int card_chancellor_of_the_forge(int player, int card, event_t event){
	/* Chancellor of the Forge	|4|R|R|R
	 * Creature - Giant 5/5
	 * You may reveal this card from your opening hand. If you do, at the beginning of the first upkeep, put a 1/1 |Sred Goblin creature token with haste onto the battlefield.
	 * When ~ enters the battlefield, put X 1/1 |Sred Goblin creature tokens with haste onto the battlefield, where X is the number of creatures you control. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		int amount = count_permanents_by_type(player, TYPE_CREATURE);
		token_generation_t token;
		default_token_definition(player, card, CARD_ID_GOBLIN, &token);
		token.qty = amount;
		token.s_key_plus = SP_KEYWORD_HASTE;
		generate_token(&token);
	}

	return 0;
}

int card_chancellor_of_the_spires(int player, int card, event_t event){
/* Chancellor of the Spires	|4|U|U|U
 * Creature - Sphinx 5/7
 * You may reveal this card from your opening hand. If you do, at the beginning of the first upkeep, each opponent puts the top seven cards
 * 		of his or her library into his or her graveyard.
 * Flying
 * When ~ enters the battlefield, you may cast target instant or sorcery card from an opponent's graveyard without paying its mana cost. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		if( !graveyard_has_shroud(1-player) ){
			const int *grave = get_grave(1-player);
			int pa[2][count_graveyard(1-player)];
			int pac = 0;
			int i = 0;
			while( grave[i] != -1 ){
					if( can_legally_play_iid(player, grave[i]) ){
						pa[0][pac] = grave[i];
						pa[1][pac] = i;
						pac++;
					}
					i++;
			}
			test_definition_t this_test;
			new_default_test_definition(&this_test, TYPE_SPELL, "Select target instant or sorcery card.");
			int selected = select_card_from_zone(player, player, pa[0], pac, 0, AI_MAX_CMC, -1, &this_test);
			if( selected != -1 ){
				play_card_in_grave_for_free(player, 1-player, pa[1][selected]);
			}
		}
	}

	return 0;
}

int card_chancellor_of_the_tangle(int player, int card, event_t event){
	vigilance(player, card, event);
	return 0;
}

int card_conversion_chamber(int player, int card, event_t event)
{
  /* Conversion Chamber	|3
   * Artifact
   * |2, |T: Exile target artifact card from a graveyard. Put a charge counter on ~.
   * |2, |T, Remove a charge counter from ~: Put a 3/3 colorless Golem artifact creature token onto the battlefield. */

  if (IS_ACTIVATING(event))
	{
	  if (event == EVENT_CAN_ACTIVATE && !(CAN_TAP(player, card) && CAN_ACTIVATE(player, card, MANACOST_X(2))))
		return 0;

	  enum
	  {
		CHOICE_EXILE = 1,
		CHOICE_GOLEM
	  } choice = DIALOG(player, card, event,
						"Exile an artifact", any_in_graveyard_by_type(ANYBODY, TYPE_ARTIFACT) && !graveyard_has_shroud(ANYBODY), 1,
						"Golem token", count_counters(player, card, COUNTER_CHARGE), 2);

	  if (event == EVENT_CAN_ACTIVATE)
		return choice;
	  else if (event == EVENT_ACTIVATE)
		{
		  if (charge_mana_for_activated_ability(player, card, MANACOST_X(2)))
			switch (choice)
			  {
				case CHOICE_EXILE:
				  ;test_definition_t test;
				  new_default_test_definition(&test, TYPE_ARTIFACT, "Select an artifact card to exile.");

				  if (select_target_from_either_grave(player, card, 0, AI_MIN_VALUE, AI_MAX_VALUE, &test, 0, 1) != -1)
					{
					  tap_card(player, card);
					  if (player == AI && !(current_turn == 1-player && current_phase == PHASE_DISCARD))
						ai_modifier -= 64;
					}
				  break;

				case CHOICE_GOLEM:
				  remove_counters(player, card, player, card, COUNTER_CHARGE, 1);
				  tap_card(player, card);
				  break;
			  }
		}
	  else	// EVENT_RESOLVE_ACTIVATION
		switch (choice)
		  {
			case CHOICE_EXILE:
			  ;int selected, tgt_gy = get_card_instance(player, card)->targets[0].player;
			  if ((selected = validate_target_from_grave_source(player, card, tgt_gy, 1)) != -1)
				{
				  rfg_card_from_grave(tgt_gy, selected);
				  add_counters(player, card, player, card, COUNTER_CHARGE, 1);
				}
			  break;

			case CHOICE_GOLEM:
			  generate_token_by_id(player, card, CARD_ID_GOLEM);
			  break;
		  }
	}

  if (event == EVENT_SHOULD_AI_PLAY)
	ai_modifier += (player == AI ? 24 : -24) * count_counters(player, card, COUNTER_CHARGE);

  return 0;
}

int card_corrosive_gale(int player, int card, event_t event){

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_MODIFY_COST ){
		return phyrexian_casting_costs(player, card, event, MANACOST_G(1), 0);
	}

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_X_SPELL, NULL, NULL, 0, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		phyrexian_casting_costs(player, card, event, MANACOST_G(1), 0);
		if( ! played_for_free(player, card) && ! is_token(player, card) && spell_fizzled != 1 ){
			charge_mana(player, COLOR_COLORLESS, -1);
			if( spell_fizzled != 1 ){
				set_x_for_x_spells(player, card, event, x_value);
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( instance->info_slot > 0 ){
			test_definition_t this_test;
			default_test_definition(&this_test, TYPE_CREATURE);
			this_test.keyword = KEYWORD_FLYING;
			new_damage_all(player, card, ANYBODY, instance->info_slot, 0, &this_test);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_corrupted_resolve(int player, int card, event_t event){

	if( event == EVENT_CAN_CAST ){
		if( counterspell(player, card, event, NULL, 0) ){
			if( is_poisoned(card_on_stack_controller) ){
				return 99;
			}
		}
		return 0;
	}

	return counterspell(player, card, event, NULL, 0);
}

int card_darksteel_relic(int player, int card, event_t event){

	indestructible(player, card, event);

	return 0;
}

int card_death_hood_cobra(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	card_instance_t *instance = get_card_instance(player, card);

	enum{
		CHOICE_REACH = 1,
		CHOICE_DEATHTOUCH,
	};

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, 0, MANACOST_XG(1, 1), 0, NULL, NULL);
	}

	if( event == EVENT_ACTIVATE){
		if( charge_mana_for_activated_ability(player, card, MANACOST_XG(1, 1)) ){
			int priorities[3] = {
								0,
								current_phase < PHASE_DECLARE_BLOCKERS ? 10 : 1,
								current_phase == PHASE_AFTER_BLOCKING && check_state(player, card, STATE_BLOCKING) ? 10 : 1
			};
			int choice = DIALOG(player, card, event, DLG_RANDOM,
								"Gains Reach", 1, priorities[1],
								"Gains Deathtouch", 1, priorities[2]);
			if( ! choice ){
				spell_fizzled = 1;
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int key = instance->info_slot == CHOICE_REACH ? KEYWORD_REACH : 0;
		int s_key = instance->info_slot == CHOICE_DEATHTOUCH ? SP_KEYWORD_DEATHTOUCH : 0;
		pump_ability_until_eot(instance->parent_controller, instance->parent_card, instance->parent_controller, instance->parent_card,
								0, 0, key, s_key);
	}

	return 0;
}

int card_defensive_stance(int player, int card, event_t event){
	return generic_aura(player, card, event, player, -1, 1, 0, 0, 0, 0, 0);
}

int card_dementia_bat(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			new_multidiscard(instance->targets[0].player, 2, 0, player);
		}
	}

	return generic_activated_ability(player, card, event, GAA_SACRIFICE_ME | GAA_CAN_TARGET, MANACOST_XB(4, 1), 0, &td, "TARGET_PLAYER");
}

int card_despise(int player, int card, event_t event ){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			test_definition_t this_test;
			new_default_test_definition(&this_test, TYPE_CREATURE | TARGET_TYPE_PLANESWALKER, "Select a creature or planeswalker card.");

			ec_definition_t this_definition;
			default_ec_definition(instance->targets[0].player, player, &this_definition);
			new_effect_coercion(&this_definition, &this_test);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
}

static int due_respect_legacy(int player, int card, event_t event ){

	permanents_enters_battlefield_tapped(player, card, event, ANYBODY, TYPE_PERMANENT, NULL);

	if( event == EVENT_CLEANUP ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int card_due_respect(int player, int card, event_t event ){

	/* Due Respect	|1|W
	 * Instant
	 * Permanents enter the battlefield tapped this turn.
	 * Draw a card. */

	if( event == EVENT_CAN_CAST ){
		int result = basic_spell(player, card, event);
		if( result ){
			if( card_on_stack_controller != -1 && card_on_stack_controller != -1 &&
				is_what(card_on_stack_controller, card_on_stack_controller, TYPE_PERMANENT) )
			{
				result = 99;
			}
			if( result ){
				if( player == AI && current_turn != player ){
					return result;
				}
				if( player == HUMAN ){
					return result;
				}
			}
		}
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (current_turn == 1-player ? 48 : -48);
		card_instance_t *instance = get_card_instance(player, card);
		instance->targets[0].player = card_on_stack_controller;
		instance->targets[0].card = card_on_stack;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		card_instance_t *instance = get_card_instance(player, card);
		if( instance->targets[0].player > -1 && instance->targets[0].card > -1 &&
			is_what(instance->targets[0].player, instance->targets[0].card, TYPE_PERMANENT) )
		{
			add_state(instance->targets[0].player, instance->targets[0].card, STATE_TAPPED);
		}
		create_legacy_effect(player, card, &due_respect_legacy);
		draw_cards(player, 1);
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_card_entomber_exarch(int player, int card, event_t event){
/*
Entomber Exarch |2|B|B
Creature — Cleric 2/2
When Entomber Exarch enters the battlefield, choose one —
• Return target creature card from your graveyard to your hand.
• Target opponent reveals his or her hand, you choose a noncreature card from it, then that player discards that card.
*/
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_CREATURE, "Select target creature card.");

		target_definition_t td;
		default_target_definition(player, card, &td, 0);
		td.zone = TARGET_ZONE_PLAYERS;

		int can_raise_dead = ! graveyard_has_shroud(player) && new_special_count_grave(player, &test) ? 1 : 0;
		int can_target_opponent = would_validate_arbitrary_target(&td, 1-player, -1);

		card_instance_t *instance = get_card_instance(player, card);

		if( can_raise_dead || can_raise_dead ){
			int choice = DIALOG(player, card, event, DLG_RANDOM, DLG_NO_STORAGE, DLG_NO_CANCEL, DLG_AUTOCHOOSE_IF_1,
								"Raise dead", can_raise_dead, 5,
								"Opponent discards", can_target_opponent, hand_count[1-player] > 0 ? 5 : 1);
			if( choice == 1 ){
				new_global_tutor(player, player, TUTOR_FROM_GRAVE, TUTOR_HAND, 0, AI_MAX_VALUE, &test);
			}
			if( choice == 2 ){
				test_definition_t this_test;
				new_default_test_definition(&this_test, TYPE_CREATURE, "Select a noncreature card.");
				this_test.type_flag = DOESNT_MATCH;

				ec_definition_t this_definition;
				default_ec_definition(instance->targets[0].player, player, &this_definition);
				new_effect_coercion(&this_definition, &this_test);
			}
		}
	}

	return 0;
}

int card_etched_monstrosity(int player, int card, event_t event){

	/* Etched Monstrosity	|5
	 * Artifact Creature - Golem 10/10
	 * ~ enters the battlefield with five -1/-1 counters on it.
	 * |W|U|B|R|G, Remove five -1/-1 counters from ~: Target player draws three cards. */

	enters_the_battlefield_with_counters(player, card, event, COUNTER_M1_M1, 5);

	if( (event == EVENT_POW_BOOST || event == EVENT_TOU_BOOST) && affect_me(player, card) ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.zone = TARGET_ZONE_PLAYERS;
		td.preferred_controller = ANYBODY;
		if( generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_CAN_TARGET, 0, 1, 1, 1, 1, 1, GVC_COUNTERS(COUNTER_M1_M1, 5),
										&td, NULL) )
		{
			return 5;
		}
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;
	td.preferred_controller = ANYBODY;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_ACTIVATE){
		generic_activated_ability(player, card, event, GAA_CAN_TARGET, 0, 1, 1, 1, 1, 1, GVC_COUNTERS(COUNTER_M1_M1, 5),
									&td, "TARGET_PLAYER");
		if( spell_fizzled != 1 ){
			if( instance->targets[0].player == 1-player ){
				ai_modifier += (count_deck(1-player) <= 3 ? 96 : -96);
			}
			else{
				ai_modifier += (count_subtype(player, TYPE_LAND, -1)*8);
			}
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			draw_cards(instance->targets[0].player, 3);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET, 0, 1, 1, 1, 1, 1, GVC_COUNTERS(COUNTER_M1_M1, 5), &td, NULL);
}

int card_exclusion_ritual(int player, int card, event_t event){

	if( event == EVENT_MODIFY_COST_GLOBAL && ! is_humiliated(player, card, AL_STATIC) ){
		card_instance_t *instance = get_card_instance(player, card);
		int i;
		for(i=1; i<instance->info_slot; i++){
			if( has_same_name(affected_card_controller, affected_card, -1, cards_data[instance->targets[i].card].id) ){
				infinite_casting_cost();
			}
		}
	}

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_PERMANENT);
		td.illegal_type = TYPE_LAND;
		td.allow_cancel = 0;

		card_instance_t *instance = get_card_instance(player, card);

		if( can_target(&td)  ){
			if( new_pick_target(&td, "Select target nonland permanent.", 0, GS_LITERAL_PROMPT) ){
				int pos = MAX(1, instance->info_slot);
				instance->targets[pos].card = get_card_name(instance->targets[0].player, instance->targets[0].card);
				rfg_target_permanent(instance->targets[0].player, instance->targets[0].card);
				create_card_name_legacy(player, card, instance->targets[pos].card);
				instance->info_slot = pos+1;
			}
		}
	}

	return global_enchantment(player, card, event);
}


int card_fallen_ferromancer(int player, int card, event_t event){
	/* Fallen Ferromancer	|3|R	0x20046a7
	 * Creature - Human Shaman 1/1
	 * Infect
	 * |1|R, |T: ~ deals 1 damage to any target. */
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if (valid_target(&td)){
			damage_target0(player, card, 1);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_XR(1, 1), 0,
									&td, "TARGET_ANY");
}

int card_forced_worship(int player, int card, event_t event){

	if( in_play(player, card) && get_card_instance(player, card)->damage_target_player > -1 && ! is_humiliated(player, card, AL_STATIC) ){
		card_instance_t *instance = get_card_instance(player, card);
		if( IS_GAA_EVENT(event) ){
			return generic_activated_ability(player, card, event, 0, MANACOST_XW(2, 1), 0, NULL, NULL);
		}

		cannot_attack(instance->damage_target_player, instance->damage_target_card, event);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		if( in_play(instance->parent_controller, instance->parent_card) ){
			bounce_permanent(instance->parent_controller, instance->parent_card);
		}
	}

	return disabling_aura(player, card, event);
}

int card_furnace_scamp(int player, int card, event_t event){

	int result = whenever_i_deal_damage_to_a_player(player, card, event, DDBM_MUST_BE_COMBAT_DAMAGE | RESOLVE_TRIGGER_AI(player));
	if( result && ! check_special_flags2(player, card, SF2_CANNOT_BE_SACRIFICED) ){
		int times_damaged[2] = {BYTE0(result), BYTE1(result)};
		APNAP(p,{
					if( times_damaged[p] && (! IS_AI(player) || p != player) ){
						char msg[100];
						if( p == player ){
							scnprintf(msg, 100, "Sac & deal 3 damage to yourself.");
						}
						else{
							scnprintf(msg, 100, "Sac & deal 3 damage to opponent.");
						}
						int choice = DIALOG(player, card, EVENT_ACTIVATE, DLG_RANDOM, DLG_NO_STORAGE, DLG_NO_CANCEL,
											msg, 1, 5,
											"Decline", 1, 1);
						if( choice == 1 ){
							damage_player(p, 3, player, card);
							kill_card(player, card, KILL_SACRIFICE);
							return 0;
						}
					}
				}
		);
	}

	return 0;
}

int card_glissas_scorn(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT);

	card_instance_t *instance = get_card_instance(player, card);

	if(event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
			lose_life(instance->targets[0].player, 1);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ARTIFACT", 1, NULL);
}

int card_glistening_oil(int player, int card, event_t event){

	immortal_enchantment(player, card, event);

	if( in_play(player, card) && get_card_instance(player, card)->damage_target_player != -1 ){
		card_instance_t *instance = get_card_instance(player, card);

		int t_player = instance->damage_target_player;
		int t_card = instance->damage_target_card;

		upkeep_trigger_ability(player, card, event, player);

		if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
			add_counters(player, card, t_player, t_card, COUNTER_M1_M1, 1);
		}
	}

	return generic_aura(player, card, event, player, 0, 0, KEYWORD_INFECT, 0, 0, 0, 0);
}

int card_greenhilt_trainee(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_ACTIVATION){
		if( valid_target(&td) ){
			pump_until_eot(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card, 4, 4);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST0, 0, &td, "TARGET_CREATURE");
}

int card_gremlin_mine(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t td;
  target_definition_t td2;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x40);
    td.special = 8;
    default_target_definition(player, card, &td2, 0x40);
    if (player == 1)
      goto LAB_02277db0;
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x40);
    td.special = 8;
    default_target_definition(player, card, &td2, 0x40);
    td2.illegal_type = 2;
    if (player != 1)
    {
      instance = get_card_instance(player, card);
      goto LAB_02277b8a;
    }
  LAB_02277db0:
    td2.special = 0x40000;
    td2.extra = 0x7b;
  }
  td2.illegal_type = 2;
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 9, 1, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    if (iVar3 == 0)
    {
      return 0;
    }
    iVar3 = can_target(&td);
    if ((iVar3 == 0) && (iVar3 = can_target(&td2), iVar3 == 0))
    {
      return 0;
    }
    return 1;
  }
  if (event == EVENT_ACTIVATE)
  {
    instance->info_slot = 0;
    iVar3 = can_target(&td2);
    iVar2 = can_target(&td);
    iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\014",
                        "Damage artifact creature", iVar2, 10, "Remove Charge counters", iVar3, 5,
                        0, 0, 0, 0);
    if (iVar3 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    if (iVar3 == 1)
    {
      iVar3 = charge_mana_for_activated_ability(player, card, 1, 0, 0, 0, 0, 0);
      if (iVar3 == 0)
      {
        return 0;
      }
      iVar3 = new_pick_target(&td, "Select target artifact creature.", 0, 0x81);
      if (iVar3 == 0)
      {
        return 0;
      }
      add_state(player, card, 0x10);
      tapped_for_mana_color = -1;
      instance->info_slot = 1;
    }
    else
    {
      if (iVar3 != 2)
      {
        return 0;
      }
      iVar3 = charge_mana_for_activated_ability(player, card, 1, 0, 0, 0, 0, 0);
      if (iVar3 == 0)
      {
        return 0;
      }
      iVar3 = new_pick_target(&td2, "Select target target noncreature artifact.", 0, 0x81);
      if (iVar3 == 0)
      {
        return 0;
      }
      add_state(player, card, 0x10);
      tapped_for_mana_color = -1;
      instance->info_slot = 2;
    }
    kill_card(player, card, KILL_SACRIFICE);
    return 0;
  }
LAB_02277b8a:
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    iVar3 = instance->info_slot;
    if (iVar3 == 1)
    {
      iVar3 = valid_target(&td);
      if (iVar3 != 0)
      {
        damage_creature(instance->targets[0].player, instance->targets[0].card, 4, player, card);
      }
      iVar3 = instance->info_slot;
    }
    if ((iVar3 == 2) && (iVar3 = valid_target(&td2), iVar3 != 0))
    {
      remove_counters(player, card, instance->targets[0].player,
                                  instance->targets[0].card, COUNTER_CHARGE, 4);
      return 0;
    }
  }
  return 0;
}

int card_grim_affliction(int player, int card, event_t event ){
/* CARD_ID_GRIM_AFFLICTION	11612
Grim Affliction	|2|B
Instant
Put a -1/-1 counter on target creature, then proliferate.
(You choose any number of permanents and/or players with counters on them, then give each another counter of a kind already there.) */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			add_counters(player, card, instance->targets[0].player, instance->targets[0].card, COUNTER_M1_M1, 1);
			proliferate(player, card);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
}

int card_gut_shot(int player, int card, event_t event){
	/* Gut Shot	|PR	0x20046cf
	 * Instant
	 * ~ deals 1 damage to any target. */
	if( event == EVENT_MODIFY_COST ){
		phyrexian_casting_costs(player, card, event, MANACOST_R(1), 0);
	}

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER );
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ANY", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		phyrexian_casting_costs(player, card, event, MANACOST_R(1), 0);
		if( spell_fizzled != 1 ){
			return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ANY", 1, NULL);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_target0(player, card, 1);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_ichor_explosion(int player, int card, event_t event){
/* CARD_ID_ICHOR_EXPLOSION	11616
Ichor Explosion	|5|B|B
Sorcery
As an additional cost to cast ~, sacrifice a creature.
All creatures get -X/-X until end of turn, where X is the sacrificed creature's power. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_SAC_CREATURE_AS_COST, NULL, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		int sac = new_sacrifice(player, card, player, SAC_JUST_MARK|SAC_AS_COST|SAC_RETURN_CHOICE, &test);
		if (!sac){
			cancel = 1;
			return 0;
		}
		instance->info_slot = get_power(BYTE2(sac), BYTE3(sac));
		kill_card(BYTE2(sac), BYTE3(sac), KILL_SACRIFICE);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int amount = instance->info_slot;
		if( amount > 0 ){
			APNAP(p,{ pump_subtype_until_eot(player, card, p, -1, -amount, -amount, 0, 0); };);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_immolating_souleater(int player, int card, event_t event)
{
  /* Immolating Souleater	|2
   * Artifact Creature - Hound 1/1
   * |PR: ~ gets +1/+0 until end of turn. */
  // Used externally
  if (event == EVENT_POW_BOOST)
	{
	  if (!can_pay_life(player, 2))
		return generic_shade_amt_can_pump(player, card, 1, 0, MANACOST_R(1), -1);
	  else
		{
		  int mod = get_cost_mod_for_activated_abilities(player, card, MANACOST_X(0));
		  if (mod == 0)
			return generic_shade_amt_can_pump(player, card, 1, 0, MANACOST_R(1), -1) + generic_shade_amt_can_pump(player, card, 1, 2, MANACOST_X(0), -1);
		  else
			{
			  // This isn't going to be perfectly accurate due to mana sources that can tap for variable amounts of mana, but should be close.
			  int red_available = has_mana(player, COLOR_RED, 1);
			  int cless_available = has_mana(player, COLOR_ANY, 1);
			  int life_available = (life[player] - 1) / 2;
			  if (life_available >= cless_available / mod)
				return cless_available / mod;
			  else
				{
				  int amt = life_available;
				  cless_available -= life_available * mod;

				  if (red_available < cless_available / (mod + 1))
					amt += red_available;
				  else
					amt += cless_available / (mod + 1);

				  return amt;
				}
			}
		}
	}

  if (event == EVENT_CAN_ACTIVATE)
	return can_use_activated_abilities(player, card) && has_phyrexian_mana_for_activated_ability(player, card, MANACOST_R(1));

  if (event == EVENT_ACTIVATE)
	charge_phyrexian_mana_for_activated_ability(player, card, MANACOST_R(1));

  if (event == EVENT_RESOLVE_ACTIVATION)
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  if (in_play(instance->parent_controller, instance->parent_card))
		pump_until_eot_merge_previous(player, card, instance->parent_controller, instance->parent_card, 1,0);
	}

  return 0;
}

int card_impaler_shrike(int player, int card, event_t event)
{
  /* Impaler Shrike	|2|U|U
   * Creature - Bird 3/1
   * Flying
   * Whenever ~ deals combat damage to a player, you may sacrifice it. If you do, draw three cards. */

	int mode = RESOLVE_TRIGGER_OPTIONAL;
	if( IS_AI(player) ){
		mode = hand_count[player] <= 2 ? RESOLVE_TRIGGER_MANDATORY : 0;
	}
	int result = whenever_i_deal_damage_to_a_player(player, card, event, DDBM_MUST_BE_COMBAT_DAMAGE | mode);
	if( result && ! check_special_flags2(player, card, SF2_CANNOT_BE_SACRIFICED) ){
		draw_cards(player, 3);
		kill_card(player, card, KILL_SACRIFICE);
	}

	return 0;
}

int card_inquisitor_exarch(int player, int card, event_t event){
/* CARD_ID_INQUISITOR_EXARCH	11619
Inquisitor Exarch	|W|W
Creature - Cleric 2/2
When ~ enters the battlefield, choose one -
* You gain 2 life.
* Target opponent loses 2 life. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.zone = TARGET_ZONE_PLAYERS;

		int choice = DIALOG(player, card, event, DLG_RANDOM, DLG_NO_STORAGE, DLG_NO_CANCEL, DLG_AUTOCHOOSE_IF_1,
							"Gain 2 life", 1, life[player] < 6 ? 10 : 1,
							"Opponent lose 2 life", would_validate_arbitrary_target(&td, 1-player, -1), 5);
		if( choice == 1 ){
			gain_life(player, 2);
		}
		if( choice == 2 ){
			 lose_life(1-player, 2);
		}
	}

	return 0;
}

int card_insatiable_souleater(int player, int card, event_t event){

	/* Insatiable Souleater	|4
	 * Artifact Creature - Beast 5/1
	 * |PG: ~ gains trample until end of turn. */

	if( event == EVENT_CAN_ACTIVATE ){
		if( has_phyrexian_mana_for_activated_ability(player, card, MANACOST_G(1)) ){
			return generic_activated_ability(player, card, event, GAA_DO_NOT_RECALCULATE_ACTIVATION_COST, MANACOST0, 0, NULL, NULL);
		}
	}

	if (event == EVENT_ACTIVATE){
		charge_phyrexian_mana_for_activated_ability(player, card, MANACOST_G(1));
	}

	if (event == EVENT_RESOLVE_ACTIVATION){
		card_instance_t* instance = get_card_instance(player, card);
		if (in_play(instance->parent_controller, instance->parent_card)){
			pump_ability_until_eot(instance->parent_controller, instance->parent_card, instance->parent_controller, instance->parent_card,
									0, 0, KEYWORD_TRAMPLE, 0);
		}
	}

	return 0;
}

int card_invader_parasite(int player, int card, event_t event){
/*
Invader Parasite |3|R|R
Creature — Insect 3/2
Imprint — When Invader Parasite enters the battlefield, exile target land.
Whenever a land with the same name as the exiled card enters the battlefield under an opponent's control, Invader Parasite deals 2 damage
		to that player.
*/
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card) ){
		card_instance_t *instance = get_card_instance(player, card);
		int trig = 0;
		int i;
		for(i=1; i<BYTE0(instance->info_slot); i++){
			if( get_card_name(trigger_cause_controller, trigger_cause) == instance->targets[i].card ){
				trig = 1;
				break;
			}
		}
		if( trig ){
			SET_BYTE1(instance->info_slot) = 1;
			etb_ability_global(player, event, RESOLVE_TRIGGER_MANDATORY, TYPE_LAND, NULL, player, card);
			SET_BYTE1(instance->info_slot) = 0;
		}
	}

	if( event == EVENT_ETB_ABILITY ){
		card_instance_t *instance = get_card_instance(player, card);
		if( BYTE1(instance->info_slot) == 1 ){
			damage_player(trigger_cause_controller, 2, player, card);
		}
		else{
			target_definition_t td;
			default_target_definition(player, card, &td, TYPE_LAND);
			td.allow_cancel = 0;

			if( can_target(&td) && new_pick_target(&td, "TARGET_LAND", 0, 0) ){
				int pos = MAX(BYTE0(instance->info_slot), 1);
				instance->targets[pos].card = get_card_name(instance->targets[0].player, instance->targets[0].card);
				kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
				create_card_name_legacy(player, card, instance->targets[pos].card);
				SET_BYTE0(instance->info_slot) = pos+1;
			}
			instance->number_of_targets = 0;
		}
	}

	return 0;
}

int card_jor_kadeen_the_prevailer(int player, int card, event_t event){
/* CARD_ID_JOR_KADEEN_THE_PREVAILER	11624
Jor Kadeen, the Prevailer	|3|R|W
Legendary Creature - Human Warrior 5/4
First strike
Metalcraft - Creatures you control get +3/+0 as long as you control three or more artifacts. */

	check_legend_rule(player, card, event);

	if( metalcraft(player, card) && ! is_humiliated(player, card, AL_STATIC) ){
		boost_creature_type(player, card, event, -1, 3, 0, 0, BCT_CONTROLLER_ONLY | BCT_INCLUDE_SELF);
	}

	return 0;
}

int card_kiln_walker(int player, int card, event_t event)
{
	/* Kiln Walker	|3
	 * Artifact Creature - Construct 0/3
	 * Whenever ~ attacks, it gets +3/+0 until end of turn. */

	return when_attacks_pump_self(player, card, event, 3,0);
}

int card_leeching_bite(int player, int card, event_t event){
/* CARD_ID_LEECHING_BITE	11628
Leeching Bite	|1|G
Instant
Target creature gets +1/+1 until end of turn. Another target creature gets -1/-1 until end of turn. */

	if( event == EVENT_CHECK_PUMP ){
		return vanilla_instant_pump(player, card, event, ANYBODY, player, 1, 1, 0, 0);
	}

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST && can_target(&td) ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		td.preferred_controller = player;
		generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td, "Select target creature for +1/+1.", 1, NULL);
		if( spell_fizzled != 1 ){
			td.preferred_controller = 1-player;
			state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
			new_pick_target(&td, "Select target creature for -1/-1.", 1, 1 | GS_LITERAL_PROMPT);
			state_untargettable(instance->targets[0].player, instance->targets[0].card, 0);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( validate_target(player, card, &td, 0) ){
			pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 1, 1);
		}
		if( validate_target(player, card, &td, 1) ){
			pump_until_eot(player, card, instance->targets[1].player, instance->targets[1].card, -1, -1);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_lifes_finale(int player, int card, event_t event){
/* CARD_ID_LIFES_FINALE	11629
Life's Finale	|4|B|B
Sorcery
Destroy all creatures, then search target opponent's library for up to three creature cards and put them into his or her graveyard.
Then that player shuffles his or her library. */

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_CREATURE, "Select a creature card.");
		APNAP(p, { new_manipulate_all(player, card, p, &test, KILL_DESTROY); };);

		int i;
		for(i=0; i<3; i++){
			if( new_global_tutor(player, 1-player, TUTOR_FROM_DECK, TUTOR_GRAVE, 0, AI_MAX_VALUE, &test) == -1 ){
				break;
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}
	return basic_spell(player, card, event);
}

int card_marrow_shards(int player, int card, event_t event){
/* CARD_ID_MARROW_SHARDS	11632
Marrow Shards	|PW
Instant
(|PW can be paid with either |W or 2 life.)
~ deals 1 damage to each attacking creature. */

	if( event == EVENT_CAN_CAST ){
		return basic_spell(player, card, event);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		new_damage_all(player, card, current_turn, 1, NDA_ALL_CREATURES, NULL);
		kill_card(player, card, KILL_DESTROY);
	}

	return phyrexian_casting_costs(player, card, event, MANACOST_W(1), 0);
}

int card_maul_splicer(int player, int card, event_t event ){
	/* Maul Splicer	|6|G
	 * Creature - Human Artificer 1/1
	 * When ~ enters the battlefield, put two 3/3 colorless Golem artifact creature tokens onto the battlefield.
	 * Golem creatures you control have trample. */

	return generic_splicer(player, card, event, 0, 0, KEYWORD_TRAMPLE, 2);
}

int card_mindcrank(int player, int card, event_t event ){
/* CARD_ID_MINDCRANK	11637
Mindcrank	|2
Artifact
Whenever an opponent loses life, that player puts that many cards from the top of his or her library into his or her graveyard.
(Damage dealt by sources without infect causes loss of life.) */

	return 0;
}

int card_mindculling(int player, int card, event_t event){
/* CARD_ID_MINDCULLING	11638
Mindculling	|5|U
Sorcery
You draw two cards and target opponent discards two cards. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.zone = TARGET_ZONE_PLAYERS;

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			draw_cards(player, 2);
			new_multidiscard(1-player, 2, 0, player);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td, NULL, 1, NULL);
}

int card_mortis_dogs(int player, int card, event_t event)
{
/* CARD_ID_MORTIS_DOGS	11640
Mortis Dogs	|3|B
Creature - Hound 2/2
Whenever ~ attacks, it gets +2/+0 until end of turn.
When ~ dies, target player loses life equal to its power. */

  if (declare_attackers_trigger(player, card, event, 0, player, card))
	pump_until_eot(player, card, player, card, 2, 0);

  if (event == EVENT_POW_BOOST && current_turn == player && current_phase <= PHASE_DECLARE_ATTACKERS)
	return 2;

  // When ~ dies, target player loses life equal to its power.
  if (this_dies_trigger(player, card, event, 2)
	  && pick_player_duh(player, card, 1-player, 0))
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  lose_life(instance->targets[0].player, instance->power);
	}

  return 0;
}

int card_mutagenic_growth(int player, int card, event_t event){

	if( event == EVENT_CHECK_PUMP ){
		return vanilla_instant_pump(player, card, event, ANYBODY, player, 2, 2, 0, 0);
	}

	if( event == EVENT_MODIFY_COST ){
		phyrexian_casting_costs(player, card, event, MANACOST_G(1), 0);
	}

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		phyrexian_casting_costs(player, card, event, MANACOST_G(1), 0);
		if( spell_fizzled != 1 ){
			return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 2, 2);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_mycosynth_fiend(int player, int card, event_t event){

	if( in_play(player, card) && ! is_humiliated(player, card, AL_STATIC) ){
		if( (event == EVENT_POWER || event == EVENT_TOUGHNESS) && affect_me(player, card) ){
			event_result += POISON_COUNTERS(1-player);
		}
	}

	return 0;
}

int card_mycosynth_wellsprin(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_AI(player));

	if( event == EVENT_ETB_ABILITY ){
		tutor_basic_land(player, 0, 0);
	}

	if( this_dies_trigger(player, card, event, RESOLVE_TRIGGER_AI(player)) ){
		tutor_basic_land(player, 0, 0);
	}

	return 0;
}

int card_necropouncer(int player, int card, event_t event){

	if( is_equipping(player, card) ){
		card_instance_t *instance = get_card_instance(player, card);
		int p = instance->damage_target_player;
		int c = instance->damage_target_card;
		if( (event == EVENT_ABILITIES || event == EVENT_POWER || event == EVENT_TOUGHNESS) && affect_me(p, c) ){
			return vanilla_equipment(player, card, event, 2, 3, 1, 0, SP_KEYWORD_HASTE);
		}
	}

	return living_weapon(player, card, event, 2);
}

int card_numbing_dose(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int iVar5;
  target_definition_t td;

  instance = in_play(player, card);
  if (((instance != NULL) &&
       (instance = get_card_instance(player, card), instance->damage_target_player != -1)) &&
      (iVar3 = is_humiliated(player, card, AL_STATIC), iVar3 == 0))
  {
    instance = get_card_instance(player, card);
    does_not_untap((int)instance->damage_target_player, instance->damage_target_card, event);
  }
  instance = in_play(player, card);
  if (((instance == NULL) ||
       (instance = get_card_instance(player, card), instance->damage_target_player == -1)) ||
      (iVar3 = is_humiliated(player, card, AL_TRIG), iVar3 != 0))
  {
  LAB_02279e7e:
    if (event == EVENT_SHOULD_AI_PLAY)
    {
      instance = get_card_instance(player, card);
      if (instance->damage_target_player == '\0')
      {
        iVar3 = get_cmc(0, instance->damage_target_card);
        ai_modifier = ai_modifier + iVar3 * 4;
      }
      goto LAB_02279f0c;
    }
    if (event == EVENT_CAN_CAST)
      goto LAB_02279ea9;
    if (event != EVENT_CAST_SPELL)
      goto LAB_02279e94;
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_02279f0c;
    default_target_definition(player, card, &td, 0x42);
    instance = get_card_instance(player, card);
  }
  else
  {
    instance = get_card_instance(player, card);
    cVar1 = instance->damage_target_player;
    upkeep_trigger_ability(player, card, event, (int)cVar1);
    if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
      goto LAB_02279e7e;
    lose_life((int)cVar1, 1);
  LAB_02279e94:
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
    {
    LAB_02279ea9:
      default_target_definition(player, card, &td, 0x42);
      get_card_instance(player, card);
      goto LAB_02279ed6;
    }
  LAB_02279f0c:
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (2 < event - EVENT_POWER)) &&
        (event != EVENT_RESOLVE_MOVING_AURA))
    {
      if ((event == EVENT_CHANGE_TYPE) || (event == EVENT_SET_COLOR))
        goto LAB_02279ea9;
      if ((event != EVENT_GET_SELECTED_CARD) && (event != EVENT_STATIC_EFFECTS))
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td, 0x42);
    instance = get_card_instance(player, card);
    if (event != EVENT_CAST_SPELL)
      goto LAB_02279ed6;
  }
  if ((card == affected_card) && (player == affected_card_controller))
  {
    instance->number_of_targets = '\0';
    iVar3 = new_pick_target(&td, "Select target artifact or creature.", 0, 0x81);
    if (iVar3 == 0)
    {
      return 0;
    }
    iVar4 = get_cmc(instance->targets[0].player, instance->targets[0].card);
    iVar3 = iVar4 * 8;
    iVar5 = is_tapped(instance->targets[0].player, instance->targets[0].card);
    if (iVar5 == 0)
    {
      iVar3 = iVar4 * -8;
    }
    ai_modifier = ai_modifier + iVar3;
    return 0;
  }
LAB_02279ed6:
  iVar3 =
      targeted_aura_custom_prompt(player, card, event, &td, "Select target artifact or creature.");
  return iVar3;
}

int card_omen_machine(int player, int card, event_t event){
/* Omen Machine	|6
 * Artifact
 * Players can't draw cards.
 * At the beginning of each player's draw step, that player exiles the top card of his or her library.
 * If it's a land card, the player puts it onto the battlefield.
 * Otherwise, the player casts it without paying its mana cost if able. */

	if( ! is_humiliated(player, card, AL_STATIC) && trigger_condition == TRIGGER_REPLACE_CARD_DRAW && affect_me(player, card) &&
		event == EVENT_TRIGGER )
	{
		suppress_draw = 1;	// Deliberately do this during EVENT_TRIGGER, not EVENT_RESOLVE_TRIGGER, so nothing else can replace the (nonexistent) draw
	}

	upkeep_trigger_ability(player, card, event, ANYBODY);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		int iid = deck_ptr[current_turn][0];
		if( iid > -1 ){
			rfg_top_card_of_deck(current_turn);
			if( is_what(-1, iid, TYPE_PERMANENT) ){
				remove_card_from_rfg(current_turn, cards_data[iid].id);
				int card_added = add_card_to_hand(current_turn, iid);
				put_into_play(current_turn, card_added);
			}
			else{
				if( can_legally_play_iid(current_turn, iid) ){
					play_card_in_exile_for_free(current_turn, current_turn, cards_data[iid].id);
				}
			}
		}
	}

	return 0;
}

int card_parasitic_implant(int player, int card, event_t event){
	/* Parasitic Implant	|3|B
	 * Enchantment - Aura
	 * Enchant creature
	 * At the beginning of your upkeep, enchanted creature's controller sacrifices it and you put a 1/1 colorless Myr artifact creature token onto the battlefield. */

	if( in_play(player, card) && get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);
		if( current_turn == player && upkeep_trigger(player, card, event) ){
			generate_token_by_id(player, card, CARD_ID_MYR);
			kill_card(instance->damage_target_player, instance->damage_target_card, KILL_SACRIFICE);
		}
	}

	return disabling_aura(player, card, event);
}

int card_pestilent_souleater(int player, int card, event_t event){

	/* Pestilent Souleater	|5
	 * Artifact Creature - Insect 3/3
	 * |PB: ~ gains infect until end of turn. */

	if (event == EVENT_CAN_ACTIVATE){
		if( has_phyrexian_mana_for_activated_ability(player, card, MANACOST_B(1)) ){
			return generic_activated_ability(player, card, event, GAA_DO_NOT_RECALCULATE_ACTIVATION_COST, MANACOST0, 0, NULL, NULL);
		}
	}

	if (event == EVENT_ACTIVATE){
		if( charge_phyrexian_mana_for_activated_ability(player, card, MANACOST_B(1)) ){
			if( is_attacking(player, card) && is_unblocked(player, card) && current_phase == PHASE_AFTER_BLOCKING ){
				ai_modifier += 48;
			}
			return generic_activated_ability(player, card, event, GAA_DO_NOT_RECALCULATE_ACTIVATION_COST, MANACOST0, 0, NULL, NULL);
		}
	}

	if (event == EVENT_RESOLVE_ACTIVATION){
		card_instance_t* instance = get_card_instance(player, card);
		if (in_play(instance->parent_controller, instance->parent_card)){
			pump_ability_until_eot(player, card, instance->parent_controller, instance->parent_card, 0,0, KEYWORD_INFECT,0);
		}
	}

	return 0;
}

int card_phyrexias_core(int player, int card, event_t event){

	/* Phyrexia's Core	""
	 * Land
	 * |T: Add |1 to your mana pool.
	 * |1, |T, Sacrifice an artifact: You gain 1 life. */

	if( ! IS_GAA_EVENT(event) || event == EVENT_CAN_ACTIVATE ){
		return mana_producer(player, card, event);
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_ARTIFACT, "Select an artifact to sacrifice.");

	card_instance_t *instance = get_card_instance(player, card);

	enum{
		CHOICE_MANA = 1,
		CHOICE_GAIN_LIFE
	};

	if( event == EVENT_ACTIVATE ){
		int choice = CHOICE_MANA;
		if( ! paying_mana() && generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, MANACOST_X(2), 0, NULL, NULL) &&
			new_can_sacrifice_as_cost(player, card, &test) )
		{
			choice = CHOICE_GAIN_LIFE;
			choice = do_dialog(player, player, card, -1, -1, " Add 1\n Sac artifact and gain 1 life\n Cancel", 1);
		}

		if( choice == CHOICE_GAIN_LIFE && ! IS_AI(player) ){
			choice = DIALOG(player, card, event, DLG_RANDOM,
							"Get mana", 1, 1,
							"Sac artifact & gain life", 1, 5);
			if( ! choice ){
				spell_fizzled = 1;
				return 0;
			}
		}
		if( choice == CHOICE_MANA ){
			return mana_producer(player, card, event);
		}
		if( choice == CHOICE_GAIN_LIFE ){
			add_state(player, card, STATE_TAPPED);
			generic_activated_ability(player, card, event, 0, MANACOST_X(1), 0, NULL, NULL);
			if( spell_fizzled == 1 ){
				remove_state(player, card, STATE_TAPPED);
				return 0;
			}
			if( ! new_sacrifice(player, card, player, SAC_AS_COST, &test)){
				remove_state(player, card, STATE_TAPPED);
				spell_fizzled = 1;
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == CHOICE_GAIN_LIFE ){
			gain_life(player, 1);
		}
	}

	return 0;
}

int card_phyrexian_ingester(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_AI(player));

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);

		card_instance_t *instance = get_card_instance(player, card);

		if( can_target(&td) && new_pick_target(&td, "TARGET_CREATURE", 0, 0) ){
			int pos = MAX(1, instance->info_slot);
			if( pos < 10 ){
				instance->targets[pos].player = get_owner(instance->targets[0].player, instance->targets[0].card);
				instance->targets[pos].card = get_original_internal_card_id(instance->targets[0].player, instance->targets[0].card);
				create_card_name_legacy(player, card, cards_data[instance->targets[pos].card].id);
				rfg_target_permanent(instance->targets[0].player, instance->targets[0].card);
				instance->info_slot = pos+1;
			}
		}
		instance->number_of_targets = 0;
	}

	if( in_play(player, card) && ! is_humiliated(player, card, AL_STATIC) && get_card_instance(player, card)->info_slot > 1 ){
		card_instance_t *instance = get_card_instance(player, card);
		if( event == EVENT_POWER && affect_me(player, card) ){
			int max_pow = 0;
			int i;
			for(i=1; i<instance->info_slot; i++){
				int owner = instance->targets[i].player;
				int iid = instance->targets[i].card;
				int this_pow = get_base_power_iid(owner, iid);
				if( this_pow > max_pow ){
					max_pow = this_pow;
				}
			}
			event_result += max_pow;
		}
		if( event == EVENT_TOUGHNESS && affect_me(player, card) ){
			int max_tou = 0;
			int i;
			for(i=1; i<instance->info_slot; i++){
				int owner = instance->targets[i].player;
				int iid = instance->targets[i].card;
				int this_tou = get_base_toughness_iid(owner, iid);
				if( this_tou > max_tou ){
					max_tou = this_tou;
				}
			}
			event_result += max_tou;
		}
	}

	return 0;
}

int card_pith_driller(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		card_instance_t* instance = get_card_instance(player, card);

		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE );
		td.allow_cancel = 0;

		if( can_target(&td) && new_pick_target(&td, "TARGET_CREATURE", 0, 0) ){
			add_counters(player, card, instance->targets[0].player, instance->targets[0].card, COUNTER_M1_M1, 1);
		}
		instance->number_of_targets = 0;
	}

	return phyrexian_casting_costs(player, card, event, MANACOST_XB(4, 1), 1);
}

int card_porcelain_legionnaire(int player, int card, event_t event){

	return phyrexian_casting_costs(player, card, event, MANACOST_XW(2, 1), 1);
}

int card_praetors_grasp(int player, int card, event_t event)
{
  /* Praetor's Grasp	|1|B|B
   * Sorcery
   * Search target opponent's library for a card and exile it face down. Then that player shuffles his or her library.
   * You may look at and play that card for as
   * long as it remains exiled. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	if (event == EVENT_CAST_SPELL && affect_me(player, card))
		ai_modifier += 96;

	if (event == EVENT_RESOLVE_SPELL)
	{
		if( valid_target(&td) ){
			test_definition_t test;
			new_default_test_definition(&test, 0, "Select a card to exile.");

			int csvid = new_global_tutor(player, 1-player, TUTOR_FROM_DECK, TUTOR_RFG, 1, AI_MAX_VALUE, &test);
			if (csvid != -1)
			{
				create_may_play_card_from_exile_effect(player, card, 1-player, csvid, MPCFE_FACE_DOWN);
			}
		}
		else
			spell_fizzled = 1;

		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td, NULL, 1, NULL);
}

int card_priest_of_urabrask(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		produce_mana(player, COLOR_RED, 3);
	}

	return 0;
}

int card_pristine_talisman(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_ACTIVATION ){
		gain_life(player, 1);
	}

	return mana_producer(player, card, event);
}

int card_psychic_barrier(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	counterspell_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( counterspell_validate(player, card, &td, 0) ){
			lose_life(instance->targets[0].player, 1);
			real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_COUNTERSPELL, &td, NULL, 1, NULL);
}

int card_rage_extractor(int player, int card, event_t event){
	/* Rage Extractor	|4|PR	0x200476f
	 * Artifact
	 * Whenever you cast a spell with |P in its mana cost, ~ deals damage equal to that spell's converted mana cost to any target. */
	if( trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card) ){
		if( is_phyrexian(get_id(trigger_cause_controller, trigger_cause)) ){
			test_definition_t test;
			default_test_definition(&test, TYPE_ANY);
			if( new_specific_spell_played(player, card, event, player, RESOLVE_TRIGGER_MANDATORY, &test) ){
				target_definition_t td;
				default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER );
				td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;
				td.allow_cancel = 0;

				card_instance_t *instance = get_card_instance(player, card);

				if( can_target(&td) && new_pick_target(&td, "TARGET_ANY", 0, 0) ){
					damage_target0(player, card, get_cmc(trigger_cause_controller, trigger_cause));

				}
				instance->number_of_targets = 0;
			}
		}
	}

	return phyrexian_casting_costs(player, card, event, MANACOST_XR(4, 1), 1);
}

int card_reaper_of_sheoldred(int player, int card, event_t event){

	if( in_play(player, card) && ! is_humiliated(player, card, AL_TRIG) ){
		card_instance_t *damage = damage_being_dealt(event);
		if( damage ){
			if( damage->damage_target_card == card && damage->damage_target_player == player && damage->info_slot > 0 ){
				card_instance_t *instance = get_card_instance(player, card);
				if( instance->targets[8].player < 0 ){
					instance->targets[8].player = 0;
				}
				instance->targets[8].player++;
				if( instance->targets[1].player < 0 ){
					instance->targets[1].player = 0;
				}
				if( damage->damage_source_player == 0 ){
					SET_BYTE0(instance->targets[1].player)++;
				}
				if( damage->damage_source_player == 1 ){
					SET_BYTE1(instance->targets[1].player)++;
				}
			}
		}
	}

	if( resolve_damage_trigger(player, card, event, DDBM_TRACE_DAMAGED_PLAYERS, player, card) ){
		card_instance_t *instance = get_card_instance(player, card);
		int player_who_damaged_me[2] = {BYTE0(instance->targets[1].player), BYTE1(instance->targets[1].player)};
		instance->targets[1].player = 0;
		APNAP(p,{
					if( player_who_damaged_me[p] ){
						poison(p, 1);
					}
				}
		);
	}

	return 0;
}

int card_remember_the_fallen(int player, int card, event_t event)
{
  /* Remember the Fallen	|2|W
   * Sorcery
   * Choose one or both - Return target creature card from your graveyard to your hand; and/or return target artifact card from your graveyard to your hand. */

  return spell_return_one_or_two_cards_from_gy_to_hand(player, card, event, TYPE_CREATURE, TYPE_ARTIFACT);
}

int card_ruthless_invasion(int player, int card, event_t event){

	if( event == EVENT_CAN_CAST ){
		return basic_spell(player, card, event);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int i;
		for(i=0; i<2; i++){
			int count = active_cards_count[i]-1;
			while( count > -1 ){
					if( in_play(i, count) && is_what(i, count, TYPE_CREATURE) && !  is_what(i, count, TYPE_ARTIFACT) ){
						pump_ability_until_eot(player, card, i, count, 0, 0, 0, SP_KEYWORD_CANNOT_BLOCK);
					}
					count--;
			}
		}
	}

	return phyrexian_casting_costs(player, card, event, MANACOST_XW(3, 1), 0);
}

int card_scrapyard_salvo(int player, int card, event_t event){
	/* Scrapyard Salvo	|1|R|R	0x200477e
	 * Sorcery
	 * ~ deals damage to target player or planeswalker equal to the number of artifact cards in your graveyard. */
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TARGET_TYPE_PLANESWALKER );
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_target0(player, card, count_graveyard_by_type(player, TYPE_ARTIFACT));
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER_OR_PLANESWALKER", 1, NULL);
}

int card_sensor_splicer(int player, int card, event_t event ){
	/* Sensor Splicer	|4|W
	 * Creature - Artificer 1/1
	 * When ~ enters the battlefield, put a 3/3 colorless Golem artifact creature token onto the battlefield.
	 * Golem creatures you control have vigilance. */

	if( in_play(player, card) && ! is_humiliated(player, card, AL_STATIC) ){
		if( event == EVENT_ABILITIES && affected_card_controller == player &&
			has_subtype(affected_card_controller, affected_card, SUBTYPE_GOLEM) )
		{
			vigilance(affected_card_controller, affected_card, event);
		}
	}

	return generic_splicer(player, card, event, 0, 0, 0, 1);
}

int card_shattered_angel(int player, int card, event_t event ){

	etb_ability_global(1-player, event, RESOLVE_TRIGGER_AI(player), TYPE_LAND, 0, player, card);

	if( event == EVENT_ETB_ABILITY ){
		gain_life(player, 3);
	}

	return 0;
}

int card_sickleslicer(int player, int card, event_t event){

	if( is_equipping(player, card) ){
		card_instance_t *instance = get_card_instance(player, card);
		int p = instance->damage_target_player;
		int c = instance->damage_target_card;
		if( (event == EVENT_POWER || event == EVENT_TOUGHNESS) && affect_me(p, c) ){
			return vanilla_equipment(player, card, event, 4, 2, 2, 0, 0);
		}
	}

	return living_weapon(player, card, event, 4);
}

int card_slag_fiend(int player, int card, event_t event)
{
  int newval;
  if ((((event - EVENT_POWER < 2) && (player != -1)) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    newval = count_graveyard_by_type(2, 0x40);
    set_characteristic_pt(newval);
  }
  return 0;
}

int card_slash_panter(int player, int card, event_t event){

	haste(player, card, event);

	return phyrexian_casting_costs(player, card, event, MANACOST_XR(4, 1), 1);
}

int card_soul_conduit(int player, int card, event_t event){
/*
Soul Conduit |6
Artifact
{6}, {T}: Two target players exchange life totals.
*/
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( would_validate_arbitrary_target(&td, player, -1) &&  would_validate_arbitrary_target(&td, 1-player, -1) ){
			return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_X(6), 0, NULL, NULL);
		}
	}

	if( event == EVENT_ACTIVATE ){
		ai_modifier += ((life[player] - life[1-player]) * 8);
		generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_X(6), 0, NULL, NULL);
		if( spell_fizzled != 1 ){
			instance->targets[0].player = 1-player;
			instance->targets[0].card = -1;
			instance->targets[1].player = player;
			instance->targets[1].card = -1;
			instance->number_of_targets = 2;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( validate_target(player, card, &td, 0) && validate_target(player, card, &td, 1) ){
			exchange_life_totals(instance->targets[0].player, instance->targets[1].player);
		}
	}

	return 0;
}

static const char *can_legally_change_target_to_me(int who_chooses, int player, int card,
                                                   int targeting_player, int targeting_card)
{
  card_instance_t *eff = get_card_instance(player, card);
  if (eff->number_of_targets == 0)
    return "must have at least 1 target";
  eff->targets[9].player = targeting_player;
  eff->targets[9].card = targeting_card;
  set_special_flags2(player, card, SF2_SPELLSKITE);
  int rvalue = call_card_function(player, card, EVENT_CAN_CAST);
  eff->targets[9].player = -1;
  eff->targets[9].card = -1;
  remove_special_flags2(player, card, SF2_SPELLSKITE);
  return rvalue ? NULL : "cannot target Spellskite";
}

int card_spellskite(int player, int card, event_t event)
{
  target_definition_t td_spell;
  counterspell_target_definition(player, card, &td_spell, TYPE_ANY);
  td_spell.extra = (int32_t)can_legally_change_target_to_me;
  td_spell.special |= TARGET_SPECIAL_EXTRA_FUNCTION;

  card_instance_t *instance = get_card_instance(player, card);

  if (event == EVENT_CAN_ACTIVATE)
  {
    if (has_phyrexian_mana_for_activated_ability(player, card, 0, 0, 1, 0, 0, 0))
    {
      if (generic_activated_ability(player, card, event, GAA_SPELL_ON_STACK, 0, 0, 0, 0, 0, 0, 0,
                                    &td_spell, "TARGET_SPELL"))
      {
        return 99;
      }
    }
  }

  if (event == EVENT_ACTIVATE)
  {
    if (charge_phyrexian_mana_for_activated_ability(player, card, 0, 0, 1, 0, 0, 0))
    {
      instance->targets[0].player = card_on_stack_controller;
      instance->targets[0].card = card_on_stack;
      instance->number_of_targets = 1;
      set_special_flags2(instance->targets[0].player, instance->targets[0].card, SF2_SPELLSKITE);
      int rvalue = call_card_function(instance->targets[0].player, instance->targets[0].card,
                                      EVENT_CHANGE_TARGET);
      remove_special_flags2(instance->targets[0].player, instance->targets[0].card, SF2_SPELLSKITE);
      if (rvalue != -1)
      {
        instance->info_slot = rvalue;
      }
      else
      {
        spell_fizzled = 1;
      }
    }
  }

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    if (counterspell_validate(player, card, &td_spell, 0))
    {
      card_instance_t *spell =
          get_card_instance(instance->targets[0].player, instance->targets[0].card);
      spell->targets[instance->info_slot].player = player;
      spell->targets[instance->info_slot].card = instance->parent_card;
    }
  }

  return 0;
}

static int artifact_natively_use_charge_counters(int player, int card)
{
	return get_counter_type_by_id(get_id(player, card)) == COUNTER_CHARGE ? 1 : 0;
}
int card_surge_node(int player, int card, event_t event)
{
  /* Surge Node	|1
   * Artifact
   * ~ enters the battlefield with six charge counters on it.
   * |1, |T, Remove a charge counter from ~: Put a charge counter on target artifact. */

	enters_the_battlefield_with_counters(player, card, event, COUNTER_CHARGE, 6);

	if (!IS_GAA_EVENT(event))
		return 0;

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET, MANACOST_X(1), GVC_COUNTER(COUNTER_CHARGE),
									&td, "TARGET_ARTIFACT");
		if( spell_fizzled != 1 ){
			if( artifact_natively_use_charge_counters(instance->targets[0].player, instance->targets[0].card) &&
				get_id(instance->targets[0].player, instance->targets[0].card) != CARD_ID_SURGE_NODE )
			{
				ai_modifier += 48;
			}
			else{
				ai_modifier -= 96;
			}
		}
		return 0;
	}

	if (event == EVENT_RESOLVE_ACTIVATION && valid_target(&td))
	{
		add_counters(player, card, instance->targets[0].player, instance->targets[0].card, COUNTER_CHARGE, 1);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET, MANACOST_X(1), GVC_COUNTER(COUNTER_CHARGE),
									&td, "TARGET_ARTIFACT");
}

int card_suture_priest(int player, int card, event_t event ){

	etb_ability_global(ANYBODY, event, RESOLVE_TRIGGER_AI(player), TYPE_CREATURE, NULL, player, card);

	if( event == EVENT_ETB_ABILITY ){
		if( trigger_cause_controller == player ){
			gain_life(player, 1);
		}
		else{
			lose_life(1-player, 1);
		}
	}

	return 0;
}

int card_thundering_thanadon(int player, int card, event_t event ){
	return phyrexian_casting_costs(player, card, event, MANACOST_XG(4, 2), 1);
}

int card_tormentor_exarch(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.preferred_controller = player;
		td.allow_cancel = 0;

		target_definition_t td1;
		default_target_definition(player, card, &td1, TYPE_CREATURE);
		td1.preferred_controller = 1-player;
		td1.allow_cancel = 0;

		card_instance_t *instance = get_card_instance(player, card);

		int choice = DIALOG(player, card, event, DLG_RANDOM, DLG_NO_STORAGE, DLG_NO_CANCEL, DLG_AUTOCHOOSE_IF_1,
							"Give +2/+0", can_target(&td), 5,
							"Give +0/-2", can_target(&td1), 5);
		if( choice == 1 ){
			if( new_pick_target(&td, "Select target creature you control.", 0, GS_LITERAL_PROMPT) ){
				pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 2, 0);
			}
		}
		if( choice == 2 ){
			if( new_pick_target(&td1, "Select target creature you opponent controls.", 0, GS_LITERAL_PROMPT) ){
				pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 0, -2);
			}
		}
		instance->number_of_targets = 0;
	}

	return 0;
}

int card_trespassing_souleater(int player, int card, event_t event){

	/* Trespassing Souleater	|3
	 * Artifact Creature - Construct 2/2
	 * |PU: ~ is unblockable this turn. */

	if( event == EVENT_CAN_ACTIVATE ){
		if( has_phyrexian_mana_for_activated_ability(player, card, MANACOST_U(1)) ){
			return generic_activated_ability(player, card, event, GAA_DO_NOT_RECALCULATE_ACTIVATION_COST, MANACOST0, 0, NULL, NULL);
		}
	}
	if (event == EVENT_ACTIVATE){
		if( charge_phyrexian_mana_for_activated_ability(player, card, MANACOST_U(1)) ){
			return generic_activated_ability(player, card, event, GAA_DO_NOT_RECALCULATE_ACTIVATION_COST, MANACOST0, 0, NULL, NULL);
		}
	}

	if (event == EVENT_RESOLVE_ACTIVATION){
		card_instance_t* instance = get_card_instance(player, card);
		if (in_play(instance->parent_controller, instance->parent_card)){
			pump_ability_until_eot(player, card, instance->parent_controller, instance->parent_card, 0,0, 0,SP_KEYWORD_UNBLOCKABLE);
		}
	}

	return 0;
}

int card_triumph_of_the_horde(int player, int card, event_t event){
/* CARD_ID_TRIUMPH_OF_THE_HORDES	11702
Triumph of the Hordes	|2|G|G
Sorcery
Until end of turn, creatures you control get +1/+1 and gain trample and infect.
(Creatures with infect deal damage to creatures in the form of -1/-1 counters and to players in the form of poison counters.) */

	if( event == EVENT_RESOLVE_SPELL ){
		pump_subtype_until_eot(player, card, player, -1, 1, 1, KEYWORD_TRAMPLE | KEYWORD_INFECT, 0);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_unwinding_clock(int player, int card, event_t event){
/*
Unwinding Clock |4
Artifact
Untap all artifacts you control during each other player's untap step.
*/

	if( beginning_of_phase(player, card, event, 1-player, -1, PHASE_UNTAP) ){
		test_definition_t test;
		default_test_definition(&test, TYPE_ARTIFACT);
		new_manipulate_all(player, card, player, &test, ACT_UNTAP);
	}

	return 0;
}

int card_vapor_snag(int player, int card, event_t event){
/* CARD_ID_VAPOR_SNAG	11705
Vapor Snag	|U
Instant
Return target creature to its owner's hand. Its controller loses 1 life. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			lose_life(instance->targets[0].player, 1);
			bounce_permanent(instance->targets[0].player, instance->targets[0].card);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
}

int card_victorious_destruction(int player, int card, event_t event ){
/* CARD_ID_VICTORIOUS_DESTRUCTION	11707
Victorious Destruction	|4|R
Sorcery
Destroy target artifact or land. Its controller loses 1 life. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT | TYPE_LAND);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			lose_life(instance->targets[0].player, 1);
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td, "Select target artifact or creature", 1, NULL);
}

int card_viral_drake(int player, int card, event_t event){
/* CARD_ID_VIRAL_DRAKE	11708
Viral Drake	|3|U
Creature - Drake 1/4
Flying
Infect (This creature deals damage to creatures in the form of -1/-1 counters and to players in the form of poison counters.)
|3|U: Proliferate. (You choose any number of permanents and/or players with counters on them, then give each another counter of a kind already there.) */

	if( event == EVENT_ACTIVATE ){
		if( POISON_COUNTERS(1-player) || count_minus1_minus1_counters(1-player, -1) ){
			ai_modifier += 48;
		}
		if( count_1_1_counters(player, -1) ){
			ai_modifier += 24;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		proliferate(player, card);
	}

	return generic_activated_ability(player, card, event, 0, MANACOST_XU(3, 1), 0, NULL, NULL);
}

int card_viridian_betrayers(int player, int card, event_t event)
{
/* CARD_ID_VIRIDIAN_BETRAYERS	11709
Viridian Betrayers	|1|G|G
Creature - Elf Warrior 3/1
~ has infect as long as an opponent is poisoned. (It deals damage to creatures in the form of -1/-1 counters and to players in the form of poison counters.) */

	if( event == EVENT_ABILITIES && affect_me(player, card) && is_poisoned(1-player) && ! is_humiliated(player, card, AL_STATIC) )
		event_result |= KEYWORD_INFECT;

	return 0;
}

static int viridian_harvest_legacy(int player, int card, event_t event){

	if( resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY) ){
		gain_life(player, 6);
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int card_viridian_harvest(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  result = attached_creature_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    gain_life(player, 6);
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_0227ce30;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_0227ce30;
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
           (event != EVENT_CHANGE_TYPE)))) &&
        (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
          (event != EVENT_STATIC_EFFECTS))))
    {
      return 0;
    }
  }
LAB_0227ce30:
  default_target_definition(player, card, &td, 0x40);
  td.preferred_controller = player;
  result = targeted_aura(player, card, event, &td, "TARGET_ARTIFACT");
  return result;
}

int card_war_report(int player, int card, event_t event ){
/* CARD_ID_WAR_REPORT	11715
War Report	|3|W
Instant
You gain life equal to the number of creatures on the battlefield plus the number of artifacts on the battlefield. */

	if( event == EVENT_RESOLVE_SPELL ){
		int amount = count_subtype(ANYBODY, TYPE_ARTIFACT, -1) + count_subtype(ANYBODY, TYPE_CREATURE, -1);
		gain_life(player, amount);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_whipflare(int player, int card, event_t event ){
/* CARD_ID_WHIPFLARE	11716
Whipflare	|1|R
Sorcery
~ deals 2 damage to each nonartifact creature. */

	if( event == EVENT_RESOLVE_SPELL ){
		APNAP(p,{
					int c;
					for(c = active_cards_count[p]-1; c > -1; c--){
						if( in_play(p, c) && is_what(p, c, TYPE_CREATURE) && ! is_what(p, c, TYPE_ARTIFACT) ){
							damage_creature(p, c, 2, player, card);
						}
					}
				}
		);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_whispering_specter(int player, int card, event_t event){
/*
Whispering Specter |1|B|B
Creature — Specter 1/1
Flying
Infect (This creature deals damage to creatures in the form of -1/-1 counters and to players in the form of poison counters.)
Whenever Whispering Specter deals combat damage to a player, you may sacrifice it. If you do, that player discards a card for each
	poison counter he or she has.
*/
	int result = whenever_i_deal_damage_to_a_player(player, card, event, DDBM_MUST_BE_COMBAT_DAMAGE);
	if( result && ! check_special_flags2(player, card, SF2_CANNOT_BE_SACRIFICED) ){
		int times_damaged[2] = {BYTE0(result), BYTE1(result)};
		APNAP(p,{
					if( times_damaged[p] ){
						int choice = 2;
						if( POISON_COUNTERS(p) > 1 && p != player ){
							choice = 1;
						}
						char msg[100];
						if( p == player ){
							scnprintf(msg, 100, "Sac and you discard");
						}
						else{
							scnprintf(msg, 100, "Sac and opponent discards");
						}
						choice = DIALOG(player, card, event, DLG_RANDOM, DLG_NO_CANCEL, DLG_NO_STORAGE,
										msg, 1, 1,
										"Decline", 1, 1);
						if( choice == 1 ){
							kill_card(player, card, KILL_SACRIFICE);
							new_multidiscard(p, POISON_COUNTERS(p), 0, player);
							return 0;
						}
					}
				}
		);
	}

	return 0;
}

int card_xenograft(int player, int card, event_t event){
/*
Xenograft |4|U
Enchantment
As Xenograft enters the battlefield, choose a creature type.
Each creature you control is the chosen type in addition to its other types.
*/
// All the true effects are handled in "has_creature_type.c"

	if( event == EVENT_RESOLVE_SPELL ){
		get_card_instance(player, card)->info_slot = select_a_subtype(player, card);
	}

	return global_enchantment(player, card, event);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int deal_6_damage_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x226d1a0; original cards/new_phyrexia.c:492. */
static int deal_6_damage_legacy(int player, int card, event_t event)
{
  int player_00;
  int card_00;
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;

  instance = get_card_instance(player, card);
  if ((((event == EVENT_GRAVEYARD_FROM_PLAY) &&
        (iVar2 = instance->targets[0].card, iVar2 == affected_card)) &&
       (player_00 = instance->targets[0].player, player_00 == affected_card_controller)) &&
      ((iVar3 = graveyard_from_play(player_00, iVar2, EVENT_GRAVEYARD_FROM_PLAY),
        iVar3 != 0 && (iVar3 = check_for_death_trigger_removal(player_00, iVar2), iVar3 == 0))))
  {

    iVar3 = instance->targets[1].player;
    card_00 = instance->targets[1].card;
    instance = in_play(iVar3, card_00);
    if (instance != NULL)
    {

      iVar4 = is_what(player_00, iVar2, 2);
      if (iVar4 == 0)
      {
        damage_creature(iVar3, card_00, 6, player_00, iVar2);
      }
      else
      {
        iVar4 = card_counts[player_00][5] + 1;
        do
        {
          damage_creature(iVar3, card_00, 6, player_00, iVar2);
          iVar4 = iVar4 + -1;
        } while (iVar4 != 0);
      }
      kill_card(player, card, KILL_EXILE);
    }
  }
  iVar2 = end_of_combat_trigger(player, card, event, 2);
  if (iVar2 != 0)
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x226d140; original cards/new_phyrexia.c:519. */
static void cathedral_membrane_legacy(int player, int card, int t_player, int t_card)
{
  int card_00;
  card_instance_t *instance;

  card_00 = create_targetted_legacy_effect(player, card, deal_6_damage_legacy, t_player, t_card);
  instance = get_card_instance(player, card_00);
  instance->targets[0].player = player;
  instance->targets[0].card = card;
  instance->targets[1].player = t_player;
  instance->targets[1].card = t_card;
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int bludgeon_brawl_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x226ce90; original cards/new_phyrexia.c:418. */
static int bludgeon_brawl_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int player_00;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int iVar5;
  target_definition_t td;

  pcVar1 = get_card_instance(player, card);
  player_00 = (int)pcVar1->damage_target_player;
  iVar5 = pcVar1->damage_target_card;
  pcVar2 = in_play(player_00, iVar5);
  if ((pcVar2 != NULL) && (pcVar3 = is_equipping(player_00, iVar5), pcVar3 != NULL))
  {
    iVar4 = get_cmc(player_00, iVar5);
    modify_pt_and_abilities((int)pcVar2->damage_target_player, pcVar2->damage_target_card, event,
                            iVar4, 0, 0);
  }
  pcVar2 = get_card_instance(player, card);
  if (pcVar2->targets[1].player < 0)
  {
  LAB_0226cf3f:
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      goto LAB_0226cf5d;
  }
  else
  {
    other_leaves_play(player, card, pcVar1->targets[1].player, pcVar1->targets[1].card, event,
                           0, 0);
    if (event == EVENT_LEAVES_PLAY_ABILITY)
    {
    LAB_0226d097:
      unattach(player_00, iVar5);
      kill_card(player, card, KILL_EXILE);
      goto LAB_0226cf3f;
    }
    if (event != EVENT_CARDCONTROLLED)
    {
      if (event != EVENT_STATIC_EFFECTS)
        goto LAB_0226cf3f;
      iVar4 =
          is_humiliated(pcVar1->targets[1].player, pcVar1->targets[1].card, AL_STATIC);
      if (iVar4 == 0)
      {
        return 0;
      }
      goto LAB_0226d097;
    }
    if ((pcVar1->targets[1].card == affected_card) &&
        (pcVar1->targets[1].player == affected_card_controller))
      goto LAB_0226d097;
  }
  if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
LAB_0226cf5d:
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = player;
  td.preferred_controller = player;
  if ((event == EVENT_RESOLVE_ACTIVATION) &&
      (iVar4 = validate_target(player, card, &td, 0), iVar4 != 0))
  {
    equip_target_creature(player_00, iVar5, pcVar1->targets[0].player, pcVar1->targets[0].card);
  }
  iVar4 = get_cmc(player_00, iVar5);
  iVar5 =
      granted_generic_activated_ability(player, card, player_00, iVar5, event, 0x102, iVar4, 0, 0,
                                        0, 0, 0, 0, &td, "Select target creature you control.");
  return iVar5;
}

/* ManalinkEh 2022: 0x226e590; original cards/new_phyrexia.c:458. */
int card_bludgeon_brawl(int player, int card, event_t event)
{
  bool bVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;

  if (((((event == EVENT_CHANGE_TYPE) && (instance = in_play(player, card), instance != NULL)) &&
        (iVar2 = is_humiliated(player, card, AL_STATIC), iVar2 == 0)) &&
       ((iVar2 = is_what(affected_card_controller, affected_card, 0x40),
         iVar2 != 0 &&
             (iVar2 = is_what(affected_card_controller, affected_card, 2), iVar2 == 0)))) &&
      (iVar2 = is_equipment(affected_card_controller, affected_card), iVar2 == 0))
  {

    add_a_subtype(affected_card_controller, affected_card, SUBTYPE_EQUIPMENT);
    if (0 < active_cards_count[player])
    {
      iVar2 = 0;
      bVar1 = false;
      do
      {
        instance = in_play(player, iVar2);

        if ((((instance != NULL) && (iVar4 = is_what(player, iVar2, 0x80), iVar4 != 0)) &&
             (instance = get_card_instance(player, iVar2),
              instance->info_slot_fn == bludgeon_brawl_effect)) &&
            (((instance->damage_target_player == affected_card_controller &&
               (instance->damage_target_card == affected_card)) ||
              ((affected_card_controller == instance->targets[6].player &&
                (instance->targets[6].card == affected_card))))))
        {
          bVar1 = true;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < active_cards_count[player]);
      if (bVar1)
        goto LAB_0226e5a7;
    }

    iVar2 = create_targetted_legacy_activate(player, card, bludgeon_brawl_effect,
                                             affected_card_controller, affected_card);
    instance = get_card_instance(player, iVar2);
    instance->targets[6].player = affected_card_controller;
    instance->targets[6].card = affected_card;
    instance->targets[1].player = player;
    instance->targets[1].card = card;
    instance->number_of_targets = '\x01';
  }
LAB_0226e5a7:
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}


#ifndef SHANDALAR
/* Recovered October 2022 Phyrexian cost helpers. */
int charge_mana_called_from_pay_phyrexian_mana;
static const char *const phyrexian_color_words[6] = {"colorless", "black", "blue", "green", "red", "white"};

/* ManalinkEh 2022: 0x226caa0; original cards/new_phyrexia.c:60. */
static int charge_phyrexian_mana_impl(int player, int cless, int black, int blue, int green,
                                           int red, int white, int clred_artifact)
{
  int iVar1;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  int argument3;
  int iStack_110;
  int iStack_10c;
  int manacost[6];
  int manacost_modified[6];
  char buffer[200];

  manacost[3] = green;
  manacost[5] = white;
  manacost_modified[3] = green;
  manacost[0] = 0;
  manacost[2] = blue;
  manacost[4] = red;
  manacost_modified[0] = 0;
  manacost_modified[2] = blue;
  manacost_modified[4] = red;
  manacost_modified[5] = white;
  manacost[1] = black;
  manacost_modified[1] = black;
  iStack_110 = can_pay_life(player, 2);
  iVar1 = how_much_life_can_i_pay(player);
  iVar2 = has_mana(player, (-(unsigned int)(clred_artifact == 0) ^ 0xffffffff) & 6, cless);
  if (iVar2 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (iStack_110 != 0)
  {
    iVar2 = 1;
    iStack_110 = 0;
    while (true)
    {
      if (black != 0)
      {
        argument3 = (iVar1 - iStack_110) / 2;
        if (black < argument3)
        {
          argument3 = black;
        }
        iVar3 = has_mana(player, iVar2, 1);
        uVar4 = black - iVar3 & (int)(black - iVar3 ^ 0xffffffffU) >> 0x1f;
        sprintf(buffer, " Pay 2 life for how much %s mana? (max %d, min %d)",
                phyrexian_color_words[iVar2], argument3, uVar4);
        while (true)
        {
          iVar3 = choose_a_number(player, buffer, argument3);
          if (iVar3 == -1)
          {
            spell_fizzled = 1;
            return 0;
          }
          if ((iVar3 <= argument3) && ((int)uVar4 <= iVar3))
            break;
          if (player == 1)
          {
            spell_fizzled = 1;
            ai_modifier = ai_modifier + -100;
            return 0;
          }
        }
        if ((((iVar1 - iStack_110) + -6) / 2 <= iVar3) && (player == 1))
        {
          ai_modifier = ai_modifier + -999;
          spell_fizzled = 1;
          return 0;
        }
        iStack_110 = iStack_110 + iVar3 * 2;
        manacost_modified[iVar2] = manacost_modified[iVar2] - iVar3;
      }
      iVar2 = iVar2 + 1;
      if (iVar2 == 6)
        break;
      black = manacost[iVar2];
    }
    white = manacost_modified[5];
    green = manacost_modified[3];
    black = manacost_modified[1];
    red = manacost_modified[4];
    blue = manacost_modified[2];
  }
  charge_mana_called_from_pay_phyrexian_mana = 1;
  iStack_10c = cless;
  if (clred_artifact != 0)
  {
    iStack_10c = 0;
    clred_artifact = cless;
  }
  charge_mana_multi_a(player, iStack_10c, black, blue, green, red, white, clred_artifact);
  charge_mana_called_from_pay_phyrexian_mana = 0;
  if (spell_fizzled != 1)
  {
    lose_life(player, iStack_110);
    return 1;
  }
  charge_mana_called_from_pay_phyrexian_mana = 0;
  return 0;
}


/* ManalinkEh 2022: 0x226d7a0; original cards/new_phyrexia.c:5. */
int has_phyrexian_mana(int player, int cless, int black, int blue, int green, int red,
                            int white, int clred_artifact)
{
  int iVar1;
  int iVar2;
  unsigned int uVar3;
  int iVar4;
  int argument2;
  int manacost[5];
  manacost[1] = blue;
  manacost[0] = black;
  manacost[2] = green;
  manacost[3] = red;
  manacost[4] = white;
  iVar1 = has_mana_multi(player, cless, black, blue, green, red, white);
  if (iVar1 == 0)
  {
    iVar1 = has_mana(player, (-(unsigned int)(clred_artifact == 0) ^ 0xffffffff) & 6, cless);
    uVar3 = 0;
    if ((iVar1 != 0) && (iVar1 = can_pay_life(player, 1), iVar1 != 0))
    {
      iVar1 = 0x32;
      argument2 = 0;
      while (true)
      {
        argument2 = argument2 + 1;
        iVar4 = 4;
        if (black < 1)
        {
          iVar1 = iVar1 + -10;
        }
        else
        {
          do
          {
            for (; black <= iVar4; iVar4 = iVar4 + -1)
            {
              iVar1 = iVar1 + -2;
            }
            iVar2 = has_mana(player, argument2, iVar4 + 1);
            if (iVar2 != 0)
            {
              iVar1 = iVar1 + -2;
            }
            iVar4 = iVar4 + -1;
          } while (-1 < iVar4);
        }
        if (argument2 == 5)
          break;
        black = manacost[argument2];
      }
      if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
      {
        uVar3 = (unsigned int)(6 < life[player] - iVar1);
      }
      else
      {
        uVar3 = (unsigned int)(iVar1 <= life[player]);
      }
    }
    return uVar3;
  }
  return 1;
}


/* ManalinkEh 2022: 0x226d900; original cards/new_phyrexia.c:50. */
int has_phyrexian_mana_for_activated_ability(int player, int card, int cless, int black,
                                                  int blue, int green, int red, int white)
{
  int result;

  result = get_cost_mod_for_activated_abilities(
      player, card, (int)(cless ^ 0xffffffffU) >> 0x1f & cless, black, blue, green, red, white);
  result = has_phyrexian_mana(player, result, black, blue, green, red, white, 0);
  return result;
}


/* ManalinkEh 2022: 0x226d990; original cards/new_phyrexia.c:135. */
int charge_phyrexian_mana(int player, int card, event_t event, int cless, int black, int blue,
                               int green, int red, int white, int clred_artifact)
{
  int result;
  result = get_updated_casting_cost(player, card, -1, event, cless);
  result = charge_phyrexian_mana_impl(player, result, black, blue, green, red, white,
                                           clred_artifact);
  return result;
}


/* ManalinkEh 2022: 0x226da10; original cards/new_phyrexia.c:141. */
int charge_phyrexian_mana_for_activated_ability(int player, int card, int cless, int black,
                                                     int blue, int green, int red, int white)
{
  int iVar1;
  int iVar2;
  int colorless;

  iVar2 = 0;
  if (cless < 1)
  {
    iVar2 = cless;
  }
  iVar1 = get_cost_mod_for_activated_abilities(
      player, card, (int)(cless ^ 0xffffffffU) >> 0x1f & cless, black, blue, green, red, white);
  colorless = iVar2;
  if ((cless < 0) && (iVar1 == 0))
  {
    colorless = 0;
    iVar1 = iVar2;
  }
  iVar1 = charge_phyrexian_mana_impl(player, iVar1, black, blue, green, red, white, 0);
  iVar2 = 0;
  if ((iVar1 != 0) && (iVar2 = 1, colorless != 0))
  {
    iVar2 = charge_mana_multi(player, colorless, 0, 0, 0, 0, 0);
    return (unsigned int)(iVar2 != 0);
  }
  return iVar2;
}


/* ManalinkEh 2022: 0x226db00; original cards/new_phyrexia.c:156. */
int phyrexian_casting_costs(int player, int card, event_t event, int cless, int black,
                                 int blue, int green, int red, int white, int clred_artifact)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int manacost[5];

  instance = get_card_instance(player, card);
  manacost[0] = black;
  manacost[1] = blue;
  manacost[2] = green;
  manacost[3] = red;
  manacost[4] = white;
  if (event != EVENT_MODIFY_COST)
  {
    if (event == EVENT_CAN_CAST)
    {
      return 1;
    }
    if (event != EVENT_CAST_SPELL)
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
    if (instance->info_slot != 1)
    {
      return 0;
    }
    iVar2 = played_for_free(player, card);
    iVar3 = is_token(player, card);
    if (iVar2 != 0 || iVar3 != 0)
    {
      return 0;
    }
    iVar2 = charge_phyrexian_mana(player, card, EVENT_CAST_SPELL, cless, black, blue, green,
                                       red, white, clred_artifact);
    if (iVar2 != 0)
    {
      return 0;
    }
    spell_fizzled = 1;
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
  iVar2 = get_updated_casting_cost(player, card, -1, EVENT_MODIFY_COST, -1);
  if (clred_artifact == 0)
  {
    iVar3 = has_mana_multi(player, iVar2, black, blue, green, red, white);
  }
  else
  {
    iVar3 = has_mana_multi_a(player, iVar2, black, blue, green, red, white);
  }
  if (iVar3 != 0)
    goto LAB_0226dd40;
  if (clred_artifact == 0)
  {
    iVar2 = has_mana(player, 0, iVar2);
    if (iVar2 == 0)
    {
      instance->info_slot = 0;
      return 0;
    }
  }
  else
  {
    iVar2 = has_mana(player, 6, iVar2);
    if (iVar2 == 0)
    {
      instance->info_slot = 0;
      goto LAB_0226dc32;
    }
  }
  iVar2 = can_pay_life(player, 1);
  if (iVar2 != 0)
  {
    iVar3 = 0;
    iVar2 = 0x32;
    while (true)
    {
      iVar3 = iVar3 + 1;
      if (black < 1)
      {
        iVar2 = iVar2 + -10;
      }
      else
      {
        iVar5 = 4;
        do
        {
          for (; black <= iVar5; iVar5 = iVar5 + -1)
          {
            iVar2 = iVar2 + -2;
          }
          iVar4 = has_mana(player, iVar3, iVar5 + 1);
          if (iVar4 != 0)
          {
            iVar2 = iVar2 + -2;
          }
          iVar5 = iVar5 + -1;
        } while (-1 < iVar5);
      }
      if (iVar3 == 5)
        break;
      black = manacost[iVar3];
    }
    if (player == 1)
    {
      if (6 < life[1] - iVar2)
        goto LAB_0226dd40;
    }
    else if (iVar2 <= life[player])
    {
    LAB_0226dd40:
      null_casting_cost(player, card);
      instance->info_slot = 1;
      return 0;
    }
  }
  instance->info_slot = 0;
  if (clred_artifact == 0)
  {
    return 0;
  }
LAB_0226dc32:
  infinite_casting_cost();
  return 0;
}

#endif
