// -*- c-basic-offset:2 -*-
// Keyword abilities, and abilities handled analagously to them.

#include "manalink.h"

/* Canonical recovered helper declarations. */
void remove_abilities_by_test(int player, event_t event, test_definition_t *test,
                                   ability_loss_t mode);
int can_use_activated_abilities(int player, int card);
int can_use_activated_abilities_by_instance(card_instance_t *instance);
int can_produce_mana(int player, int card);
void disable_all_activated_abilities(int player, int card, int mode);
void disable_nonmana_activated_abilities(int player, int card, int mode);
int humiliate_and_set_pt_abilities(int player, int card, int t_player, int t_card, int mode,
                                        test_definition_t *hc);
void remove_special_ability(int player, int card, int sa);
int check_for_special_ability(int player, int card, int keyword);
int get_special_abilities_by_instance(card_instance_t *instance);
int get_special_abilities(int player, int card);
void special_abilities(int player, int card, event_t event, int sp_abilities,
                            int source_player, int source_card);
int attack_if_able(int player, int card, event_t event);
int block_if_able(int player, int card, event_t event);
void vigilance(int player, int card, event_t event);
void granted_flanking(int s_player, int s_card, int t_player, int t_card, event_t event);
int humiliate(int src_player, int src_card, int t_player, int t_card, int mode);
int humility_legacy(int player, int card, event_t event);
static void lure_effect(int player, int card, event_t event, int luring_player,
                             int luring_card, int all_must_block, int subtype, int ability,
                             const char *prompt);
int is_humiliated(int player, int card, ability_loss_t mode);

/***********************
* Removal of abilities *
***********************/

/* Uses bits 0-11 of all cards' targets[17].card to indicate non-mana activated abilities removed (0-3), all activated abilities removed (4-7), and all
 * abilities removed (8-11).  Just in case one of these types of removal is imposed 16 times. */







int is_humiliated_by_instance(card_instance_t *instance, ability_loss_t mode)
{
  ability_loss_t aVar1;
  aVar1 = instance->targets[0x11].card;
  if (0 < (int)aVar1)
  {
    return aVar1 & mode;
  }
  return 0;
}



/* ManalinkEh 2022: 0x26dd430; original functions/abilities.c:47. */
/* Native public is_humiliated(player, card, mode), with its private worker
 * inlined here. targets[17].card holds the ability-class removal mask. */
int is_humiliated(int player, int card, ability_loss_t mode)
{
  if (player == -1 || card == -1)
    return 0;
  int flags = get_card_instance(player, card)->targets[17].card;
  return flags > 0 ? flags & mode : 0;
}





static void real_humiliation(int t_player, int t_card, int mode){
	card_instance_t* instance = get_card_instance(t_player, t_card);

	if (instance->targets[17].card < 0){
		instance->targets[17].card = 0;
	}

	uint32_t u = instance->targets[17].card & 0xF00;

	if (mode == HUMILIATE_ON){
		if (u != 0xF00){	// counter not maxed
			u += 0x100;
		}
	}
	if (mode == HUMILIATE_OFF){
		if (u != 0){	// any counters to remove
			u -= 0x100;
		}
	}

	instance->targets[17].card &= ~0xF00;
	instance->targets[17].card |= u;
}

int set_pt_legacy(int player, int card, event_t event)
{
  char cVar1;
  unsigned int uVar2;
  card_instance_t *instance;
  unsigned int uVar4;
  int result;
  unsigned int uVar6;

  instance = get_card_instance(player, card);
  cVar1 = instance->damage_target_player;
  if (cVar1 < '\0')
  {
  LAB_026dc7d8:
    uVar2 = instance->targets[1].player;
    if ((int)uVar2 < 0)
    {
      return 0;
    }
    uVar6 = event == EVENT_CLEANUP & uVar2;
    leaves_play(instance->targets[2].player, instance->targets[2].card, event, 0, 0);
    uVar4 = uVar2 & 4;
    if (((event == EVENT_LEAVES_PLAY_ABILITY) && ((uVar2 & 2) != 0)) &&
        (-1 < instance->targets[2].player))
    {
      if ((uVar4 == 0) ||
          ((current_phase != 4 || (instance->damage_target_player != current_turn))))
        goto LAB_026dc890;
      uVar6 = 1;
      goto LAB_026dc868;
    }
  LAB_026dc90a:
    if ((uVar4 != 0) && (current_phase == 4))
      goto LAB_026dc91f;
  }
  else
  {
    if (event == EVENT_POWER)
    {
      if (((instance->damage_target_card == affected_card) &&
           (cVar1 == affected_card_controller)) &&
          (-1 < instance->counter_power))
      {
        set_base_pt((int)instance->counter_power);
      }
      uVar4 = instance->targets[1].player;
      if ((int)uVar4 < 0)
      {
        return 0;
      }
      uVar6 = 0;
      leaves_play(instance->targets[2].player, instance->targets[2].card, EVENT_POWER, 0, 0);
      uVar4 = uVar4 & 4;
      goto LAB_026dc90a;
    }
    if (event != EVENT_TOUGHNESS)
      goto LAB_026dc7d8;
    if (((instance->damage_target_card == affected_card) && (cVar1 == affected_card_controller)) &&
        (-1 < instance->counter_toughness))
    {
      set_base_pt((int)instance->counter_toughness);
    }
    uVar4 = instance->targets[1].player;
    if ((int)uVar4 < 0)
    {
      return 0;
    }
    leaves_play(instance->targets[2].player, instance->targets[2].card, EVENT_TOUGHNESS, 0, 0);
    if ((uVar4 & 4) == 0)
    {
      return 0;
    }
    if (current_phase != 4)
    {
      return 0;
    }
    uVar6 = 0;
  LAB_026dc91f:
    if (instance->damage_target_player == current_turn)
    {
    LAB_026dc868:
      result = upkeep_trigger(player, card, event);
      if (result != 0)
        goto LAB_026dc890;
    }
  }
  if (uVar6 == 0)
  {
    return 0;
  }
LAB_026dc890:
  kill_card(player, card, KILL_EXILE);
  return 0;
}

static int is_pt_effect(int player, int card){
	card_instance_t* inst = get_card_instance(player, card);
	if( is_what(player, card, TYPE_EFFECT) && (inst->info_slot == (int)set_pt_legacy || inst->display_pic_csv_id == CARD_ID_GIGANTOPLASM) ){
		return 1;
	}
	return 0;
}




int real_set_pt(int player, int card, int t_player, int t_card, int pow, int tou, int mode)
{
  if (mode & 8)
    for (int p = 0; p < 2; ++p)
      for (int c = active_cards_count[p]; c >= 0; --c)
      {
        card_instance_t *instance = in_play(p, c);
        if (instance && instance->damage_target_player == t_player &&
            instance->damage_target_card == t_card && instance->targets[2].player == player &&
            instance->targets[2].card == card && is_pt_effect(p, c))
          kill_card(p, c, KILL_EXILE);
      }
  int leg = create_targetted_legacy_effect(player, card, set_pt_legacy, t_player, t_card);
  if (leg != -1)
  {
    card_instance_t *legacy = get_card_instance(player, leg);
    legacy->targets[1].player = mode;
    legacy->counter_power = (int16_t)pow;
    legacy->counter_toughness = (int16_t)tou;
    legacy->targets[2].player = player;
    legacy->targets[2].card = card;
  }
  return leg;
}

int set_pt_and_abilities_until_eot(int player, int card, int t_player, int t_card, int pow, int tou,
                                   int key, int s_key, int mode)
{
  // pow = -1 : leave power unchanged
  // tou = -1 : leave toughness unchanged
  if (t_player != ANYBODY && t_card != -1)
  {
    real_set_pt(player, card, t_player, t_card, pow, tou, mode | 1);
    int legacy = -1;
    if (key || s_key)
    {
      legacy = pump_ability_until_eot(player, card, t_player, t_card, 0, 0, key, s_key);
      add_status(player, legacy, STATUS_INVISIBLE_FX);
    }
    return legacy;
  }
  int i;
  for (i = 0; i < 2; i++)
  {
    if (i == t_player || t_player == ANYBODY)
    {
      int count = active_cards_count[i] - 1;
      while (count > -1)
      {
        if (in_play(i, count) && is_what(i, count, TYPE_CREATURE))
        {
          int good = 1;
          if ((mode & 1) && !(is_attacking(i, count) && is_unblocked(i, count)))
          {
            good = 0;
          }
          if (good == 1)
          {
            real_set_pt(player, card, i, count, pow, tou, mode | 1);
            if (key || s_key)
            {
              int legacy = pump_ability_until_eot(player, card, i, count, 0, 0, key, s_key);
              add_status(player, legacy, STATUS_INVISIBLE_FX);
            }
          }
        }
        count--;
      }
    }
  }
  return -1;
}


static void set_humiliation_legacy(int player, int card, int mode, test_definition_t *hc){
	card_instance_t *instance = get_card_instance(player, card);
	instance->targets[1].player = hc != NULL ? hc->power : 1;
	instance->targets[1].card = hc != NULL ? hc->toughness : 1;
	instance->targets[2].player = hc != NULL ? hc->keyword : 0;
	instance->targets[2].card = hc != NULL ? hc->color : 0;
	instance->targets[3].player = mode;
	instance->targets[3].card = hc != NULL ? hc->subtype : 0;
}





/****************************
* Abilities in sp_keyword_t *
****************************/

// static int block_if_able(int player, int card, event_t event);




// Single-bit abilities



void cannot_block(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x20;
    return;
  }
  return;
}

void fear(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 1;
    return;
  }
  return;
}

void horsemanship2(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x40000000;
    return;
  }
  return;
}

void intimidate(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x2000000;
    return;
  }
  return;
}
void lifelink(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 2;
    return;
  }
  return;
}
void rfg_when_damage(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x100000;
    return;
  }
  return;
}
void shadow(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x100;
    return;
  }
  return;
}
void unblockable(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x10;
    return;
  }
  return;
}
void wither(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x4000;
    return;
  }
  return;
}

#undef SP_ABILITY
#undef SP_ABILITY_SUPPRESSABLE

// Usually pump_ability_until_eot() is better, but this can go on non-creatures too (e.g. Boros Charm).
int indestructible_until_eot(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES)
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  if (affect_me(instance->damage_target_player, instance->damage_target_card))
		indestructible(instance->damage_target_player, instance->damage_target_card, event);
	}

  if (event == EVENT_CLEANUP)
	kill_card(player, card, KILL_EXILE);

  return 0;
}

// Protection
void protection_from_creatures(int player, int card, event_t event)
{
  unblockable(player, card, event);

  card_instance_t* damage = damage_being_prevented(event);
  if (damage
	  && damage->damage_target_card == card && damage->damage_target_player == player
	  && (damage->targets[3].player & TYPE_CREATURE))
	damage->info_slot = 0;
}

// Regeneration

int can_be_regenerated(int player, int card)	// by something else.
{
  card_instance_t* instance = in_play(player, card);

  return ((!instance	// not in play
		   || instance->kill_code != KILL_DESTROY	// not being destroyed (or is being buried)
		   || (instance->token_status & STATUS_CANNOT_REGENERATE))
		  ? 0 : 99);
}

int can_regenerate(int player, int card){
	if ((land_can_be_played & LCBP_REGENERATION)	// This seems actively unhelpful - it prevents regeneration_shield_legacy() from working, for instance.
		&& can_be_regenerated(player, card)
		&& can_use_activated_abilities(player, card)){
		return 99;
	} else {
		return 0;
	}
}

int regenerate_target(int player, int card)
{
  card_instance_t *instance;
  int result;
  if ((player != -1) && (card != -1))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      instance = get_card_instance(player, card);
      if ((instance->kill_code == '\x02') && ((instance->token_status & 0x800000) == 0))
      {
        regenerate_target_exe(player, card);
        result = get_id(player, card);
        if (result == 0x934)
        {
          result = is_humiliated(player, card, AL_STATIC);
          if (result == 0)
          {
            spiny_starfish_splitting_ability(player, card);
          }
        }
      }
    }
  }
  return 0;
}

int regeneration(int player, int card, event_t event, int cless, int black, int blue, int green, int red, int white){

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t* instance = get_card_instance(player, card);
		if( can_regenerate(instance->parent_controller, instance->parent_card) ){
			regenerate_target(instance->parent_controller, instance->parent_card);
		}
	}
	return generic_activated_ability(player, card, event, GAA_REGENERATION, cless, black, blue, green, red, white, 0, NULL, NULL);
}

int regeneration_shield_legacy(int player, int card, event_t event){
	/* targets[0]: Permanent this is attached to (also in card_instance_t::damage_target_player/card) */

	card_instance_t *instance = get_card_instance(player, card);

	if (instance->targets[0].player >= 0){
		if (instance->targets[1].player < 0){
			instance->targets[1].player = 0;
		}
		if (event == EVENT_ABILITIES && affect_me(instance->targets[0].player, instance->targets[0].card)){
			event_result |= KEYWORD_REGENERATION;	// mostly cosmetic, though its presence affects the AI
		}

		static int regeneration_from_shield_in_progress = 0;	// May not be necessary
		if (can_be_regenerated(instance->targets[0].player, instance->targets[0].card)
			&& !regeneration_from_shield_in_progress){
			++regeneration_from_shield_in_progress;
			regenerate_target(instance->targets[0].player, instance->targets[0].card);
			kill_card(player, card, KILL_EXILE);
			--regeneration_from_shield_in_progress;
		}
	}

	if (event == EVENT_CLEANUP || event == EVENT_SHOULD_AI_PLAY){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int regenerate_or_shield(int player, int card, int t_player, int t_card){
	if ((player == card && t_player == t_card) ? can_regenerate(player, card) : can_be_regenerated(player, card)){
		regenerate_target(t_player, t_card);
		return -1;
	} else {
		return create_targetted_legacy_effect(player, card, &regeneration_shield_legacy, t_player, t_card);
	}
}

static int legacy_cannot_regenerate_until_eot(int player, int card, event_t event){

	card_instance_t *instance = get_card_instance(player, card);

	if( eot_trigger(player, card, event) ){
		remove_status(instance->targets[0].player, instance->targets[0].card, STATUS_CANNOT_REGENERATE);
		kill_card(player, card, KILL_EXILE);
	}
	return 0;
}

int cannot_regenerate_until_eot(int player, int card, int t_player, int t_card){
	add_status(t_player, t_card, STATUS_CANNOT_REGENERATE);
	int leg = create_targetted_legacy_effect(player, card, &legacy_cannot_regenerate_until_eot, t_player, t_card);
	int leg2 = create_legacy_effect_exe(player, card, LEGACY_EFFECT_GENERIC, t_player, t_card);
	if (leg2 != -1){
		// exe looks, not for STATUS_CANNOT_REGENERATE on a card trying to regenerate, but a LEGACY_EFFECT_GENERIC with STATUS_CANNOT_REGENERATE attached to the card
		card_instance_t* legacy = get_card_instance(player, leg2);
		legacy->token_status |= STATUS_CANNOT_REGENERATE | STATUS_INVISIBLE_FX;
		// will be removed at eot since STATUS_PERMANENT isn't set
	}
	return leg;
}

// Other abilities



static int can_block_target_player, can_block_target_card;
static const char* can_block_target(int who_chooses, int player, int card)
{
  if (can_block_me(player, card, can_block_target_player, can_block_target_card))
	return NULL;
  else
	return EXE_STR(0x5b8580);	// localized "Illegal block." from PROMPT_DEFENDWHOM
}

int can_block(int player, int card){
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = 1-player;
	td.allowed_controller = 1-player;
	td.allow_cancel = 0;
	td.required_state = TARGET_STATE_ATTACKING;
	td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
	td.extra = (int32_t)can_block_target;
	can_block_target_player = player;
	can_block_target_card = card;
	return can_target(&td);
}

void select_blocker(int player, int card, int player_who_chooses_blockers){
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = 1-player;
	td.allowed_controller = 1-player;
	td.allow_cancel = 0;
	td.who_chooses = player_who_chooses_blockers;
	td.required_state = TARGET_STATE_ATTACKING;
	td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
	td.extra = (int32_t)can_block_target;
	can_block_target_player = player;
	can_block_target_card = card;

	card_instance_t *instance = get_card_instance(player, card);
	while( instance->blocking >= 255 && can_target(&td) ){
			instance->number_of_targets = 0;
			if( pick_target(&td, "PROMPT_DEFENDWHOM") ){
				block(player, card, 1-player, instance->targets[0].card);
			}
	}
}



const char* target_must_be_able_to_block_me(int who_chooses, int player, int card, int targeting_player, int targeting_card)
{
  return get_card_instance(player, card)->blocking == 255 && is_legal_block(player, card, targeting_player, targeting_card) ? NULL : EXE_STR(0x5b8580);	// localized "Illegal block." from PROMPT_DEFENDWHOM
}



int everybody_must_block_me(int player, int card, event_t event)
{
  lure_effect(player, card, event, player, card, 1, -1, 0, NULL);
  return 0;
}

void subtype_must_block_me(int player, int card, event_t event, subtype_t subtype, const char* prompt)
{
  lure_effect(player, card, event, player, card, 1, subtype, 0, prompt);
}

int must_be_blocked(int player, int card, event_t event)
{
  lure_effect(player, card, event, player, card, 0, -1, 0, NULL);
  return 0;
}

static int target_must_block_me_legacy(int player, int card, event_t event)
{
  if (trigger_condition == TRIGGER_MUST_BLOCK &&
      (event == EVENT_TRIGGER || event == EVENT_RESOLVE_TRIGGER) && affect_me(player, card))
  {
    card_instance_t *instance = get_card_instance(player, card), *luring, *lured;

    int luring_player = instance->damage_source_player;
    int luring_card = instance->damage_source_card;

    int lured_player = instance->damage_target_player;
    int lured_card = instance->damage_target_card;

    if (luring_player == current_turn &&
        reason_for_trigger_controller ==
            ((event_flags & EF_ATTACKER_CHOOSES_BLOCKERS) ? current_turn : 1 - current_turn) &&
        (luring = in_play(luring_player, luring_card)) && (luring->state & STATE_ATTACKING)

        && luring_player != lured_player && (lured = in_play(lured_player, lured_card)) &&
        lured->blocking == 255 &&
        is_legal_block(lured_player, lured_card, luring_player, luring_card))
    {
      if (event == EVENT_TRIGGER)
        event_result |= RESOLVE_TRIGGER_MANDATORY;

      if (event == EVENT_RESOLVE_TRIGGER)
        block(lured_player, lured_card, luring_player, luring_card);
    }
  }

  if ((event == EVENT_CLEANUP && (get_card_instance(player, card)->targets[2].player & 1)) ||
      (event == EVENT_PHASE_CHANGED && current_phase == PHASE_AFTER_BLOCKING &&
       (get_card_instance(player, card)->targets[2].player & 2)))
    kill_card(player, card, KILL_EXILE);

  return 0;
}

int target_must_block_me(int player, int card, int t_player, int t_card, int mode){
	// bitfields_for_mode
	// 1 --> Effect ends at eot
	int legacy_card = create_targetted_legacy_effect(player, card, &target_must_block_me_legacy, t_player, t_card);
	card_instance_t *legacy_instance = get_card_instance(player, legacy_card);
	legacy_instance->targets[2].player = mode;
	return legacy_card;
}

int exalted(int player, int card, event_t event, int ability, int sp_ability)
{
  if (declare_attackers_trigger(player, card, event, DAT_ATTACKS_ALONE|DAT_TRACK, player, -1))
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  int attacker = BYTE0(instance->targets[2].player);
	  if (in_play(player, attacker))
		pump_ability_until_eot(player, card, player, attacker, 1, 1, ability, sp_ability);

	  return attacker;
	}

  return -1;
}

int check_for_flanking_removal(int player, int card)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int card_00;

  iVar1 = check_state(player, card, 0x44);
  if ((iVar1 != 0) && (iVar1 = 1 - player, 0 < active_cards_count[iVar1]))
  {
    card_00 = 0;
    do
    {
      instance = in_play(iVar1, card_00);
      if ((instance != NULL) && (iVar3 = get_id(iVar1, card_00), iVar3 == 0x962))
      {
        if ((iVar1 == -1) || (card_00 == -1))
        {
          return 1;
        }
        iVar3 = is_humiliated(iVar1, card_00, AL_KEY);
        if (iVar3 == 0)
        {
          return 1;
        }
      }
      card_00 = card_00 + 1;
    } while (card_00 < active_cards_count[iVar1]);
  }
  return 0;
}



void flanking(int player, int card, event_t event)
{
  granted_flanking(player, card, player, card, event);
}

int flash(int player, int card, event_t event){
	// Must also be set as type instant
	if (event == EVENT_CAN_CAST){
		return 1;
	} else if (event == EVENT_CAST_SPELL && affect_me(player, card)){
		ai_modifier -= 16;
	}
	return 0;
}

int freeze_when_damage(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  instance->ext->sp_keywords = instance->ext->sp_keywords | 0x1000000;
  if (((player != -1) && (card != -1)) &&
      (result = is_humiliated(player, card, AL_TRIG), result != 0))
  {
    return 0;
  }
  damage_effects(player, card, event);
  if (((2 < instance->targets[1].player) && (trigger_condition == 0xd7)) &&
      ((card == affected_card &&
        ((player == affected_card_controller && (reason_for_trigger_controller == player))))))
  {
    result = 2;
    do
    {
      ptVar2 = instance->targets + result;
      ptVar1 = instance->targets + result;
      result = result + 1;
      effect_frost_titan(player, card, ptVar1->player, ptVar2->card);
    } while (result < instance->targets[1].player);
    instance->targets[1].player = 2;
  }
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
  instance->targets[1].player = 2;
  return 0;
}

void give_haste(int player, int card) // Avoid!
{
  card_instance_t *instance = in_play(player, card);
  if (instance)
    instance->state &= ~0x30000U;
}

void haste(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 4;
    return;
  }
  return;
}

void persist(int player, int card, event_t event)
{
  if (count_minus1_minus1_counters(player, card) <= 0)
	{
	  nice_creature_to_sacrifice(player, card);

	  int owner, position;
	  if (this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY)
		  && find_in_owners_graveyard(player, card, &owner, &position))
		reanimate_permanent(owner, -1, owner, position, REANIMATE_MINUS1_MINUS1_COUNTER);	// -1 for card doesn't abort since no legacy is added
	}
}



/*************************
* Detection of abilities *
*************************/

int check_for_ability(int player, int card, int keyword){
	if( card != -1 ){
		if( get_abilities(player, card, EVENT_ABILITIES, -1) & keyword ){
			return 1;
		}
	}
	else{
		int count = 0;
		while( count < active_cards_count[player] ){
				if( in_play(player, count) && is_what(player, count, TYPE_CREATURE) ){
					if( get_abilities(player, count, EVENT_ABILITIES, -1) & keyword ){
						return 1;
					}
				}
				count++;
		}
	}
	return 0;
}









void set_special_abilities(int player, int card, int type, int value)
{
  if (card != -1)
  {
    get_card_instance(player, card)->ext->sp_keywords |= value;
    return;
  }
  for (int p = 0; p < 2; ++p)
    if (p == player || player == ANYBODY)
      for (int c = active_cards_count[p] - 1; c >= 0; --c)
        if (in_play(p, c) && is_what(p, c, type))
          get_card_instance(p, c)->ext->sp_keywords |= value;
}

int legacy_negate_ability_and_special_ability(int player, int card, event_t event)
{

  if (event == EVENT_ABILITIES)
  {
    card_instance_t *instance = get_card_instance(player, card);
    if (instance->damage_target_player > -1)
    {
      if (affect_me(instance->damage_target_player, instance->damage_target_card))
      {
        if (instance->targets[1].player > -1)
        {
          event_result &= ~instance->targets[1].player;
        }
        if (instance->targets[1].card > -1)
        {
          remove_special_ability(instance->damage_target_player, instance->damage_target_card,
                                 instance->targets[1].card);
        }
      }
    }
  }

  if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
  }

  return 0;
}

int effect_on_all_negate_ability_and_special_ability_legacy_attached_to_me(int player, int card,
                                                                           int effect)
{
  // effect = 0 --> check only
  // effect = 1 -> enable the "control legacy" with the highest timestamp (AKA the most recent)
  // effect = -1 -> disable all "control legacies"
  int result = 0;
  int legacies[100][3];
  int lc = 0;
  card_instance_t *inst;
  int p, c;
  for (p = 0; p <= 1; ++p)
  {
    for (c = active_cards_count[p]; c > -1; c--)
    {
      if ((inst = in_play(p, c)) && inst->damage_target_player == player &&
          inst->damage_target_card == card)
      {
        if (is_what(p, c, TYPE_EFFECT) &&
            inst->info_slot == (int)legacy_negate_ability_and_special_ability)
        {
          if (effect == 1 && !check_status(p, c, STATUS_CONTROLLED))
          {
            legacies[lc][0] = inst->timestamp;
            legacies[lc][1] = p;
            legacies[lc][2] = c;
            lc++;
            result = 1;
          }
          if (effect == -1)
          {
            remove_status(p, c, STATUS_CONTROLLED);
            result = 1;
          }
        }
      }
    }
  }
  if (lc)
  {
    int max_timestamp = 0;
    int id_of_legacy_to_activate = 0;
    int i;
    for (i = 0; i < lc; i++)
    {
      if (legacies[i][0] > max_timestamp)
      {
        max_timestamp = legacies[i][0];
      }
      id_of_legacy_to_activate = i;
    }
    add_status(legacies[id_of_legacy_to_activate][1], legacies[id_of_legacy_to_activate][2],
               STATUS_CONTROLLED | STATUS_INVISIBLE_FX);
  }
  return result;
}

static int create_negate_ability_and_special_ability_until_eot(int player, int card, int t_player, int t_card, keyword_t keyword,
	int sp_ability)
{
	effect_on_all_negate_ability_and_special_ability_legacy_attached_to_me(t_player, t_card, -1);
	int legacy = create_targetted_legacy_effect(player, card, &legacy_negate_ability_and_special_ability, t_player, t_card);
	card_instance_t *leg = get_card_instance(player, legacy);
	leg->targets[1].player = keyword;
	leg->targets[1].card = sp_ability;
	leg->targets[2].player = 1;
	return legacy;
}

/* Affects all of t_player's creatures if t_card == -1, and all creatures if t_player == 2 as well.  t_player == 2, t_card != -1 is undefined behavior.  Returns
 * created legacy if t_player != 2 and t_card != -1 (i.e., a single creature), otherwise -1. */
static int negate_ability_and_special_ability_until_eot_impl(int player, int card, int t_player, int t_card, keyword_t keyword, int s_keyword){
	int i;
	for(i=0; i<2; i++){
		if( i == t_player || t_player == ANYBODY ){
			if( t_card != -1 ){
				return create_negate_ability_and_special_ability_until_eot(player, card, t_player, t_card, keyword, s_keyword);
			}
			else{
				int count = active_cards_count[i]-1;
				while( count > -1 ){
						if( in_play(i, count) && is_what(i, count, TYPE_CREATURE) ){
							create_negate_ability_and_special_ability_until_eot(player, card, i, count, keyword, s_keyword);
						}
						count--;
				}
			}
		}
	}
	return -1;
}

int negate_ability_until_eot(int player, int card, int t_player, int t_card, keyword_t keyword){
	return negate_ability_and_special_ability_until_eot_impl(player, card, t_player, t_card, keyword, 0);
}

int negate_ability_and_special_ability_until_eot(int player, int card, int t_player, int t_card, keyword_t keyword, int s_keyword){
	return negate_ability_and_special_ability_until_eot_impl(player, card, t_player, t_card, keyword, s_keyword);
}

/* Native colored protection contributes protection bits during EVENT_ABILITIES. */

int generic_protection_from(int player, int card, event_t event, color_test_t cols)
{
  if (event == EVENT_ABILITIES && affect_me(player, card))
  {
    int old_event_result = event_result;
    cols = get_sleighted_color_test(player, card, cols & COLOR_TEST_ANY_COLORED) |
           (cols & COLOR_TEST_ARTIFACT);
    event_result = old_event_result | (cols << 10); // COLOR_TEST_BLACK => KEYWORD_PROT_BLACK, etc.
  }

  return 0;
}

int protection_from_black(int player, int card, event_t event){
	return generic_protection_from(player, card, event, COLOR_TEST_BLACK);
}

int protection_from_red(int player, int card, event_t event){
	return generic_protection_from(player, card, event, COLOR_TEST_RED);
}

int protection_from_blue(int player, int card, event_t event){
	return generic_protection_from(player, card, event, COLOR_TEST_BLUE);
}

int protection_from_green(int player, int card, event_t event){
	return generic_protection_from(player, card, event, COLOR_TEST_GREEN);
}

int protection_from_white(int player, int card, event_t event){
	return generic_protection_from(player, card, event, COLOR_TEST_WHITE);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

static void set_misc_protection_impl(int player, int card);

/* ManalinkEh 2022: 0x26de880; original functions/abilities.c:665. */
void misc_protection(int player, int card, event_t event)
{
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    set_misc_protection_impl(player, card);
    return;
  }
  return;
}

static void get_protection_source_impl(int8_t can_return_dmg, int *eff_acc, int *eff_ac);

/* ManalinkEh 2022: 0x26e09a0; original functions/abilities.c:2930. */
void get_protection_source(int8_t can_return_dmg, int *eff_acc, int *eff_ac)
{
  if (-1 < attacking_card)
  {
    get_protection_source_impl(can_return_dmg, eff_acc, eff_ac);
    return;
  }
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void remove_abilities(int player, int card, event_t event, ability_loss_t mode);
void remove_abilities_by_test(int player, event_t event, test_definition_t *test,
                              ability_loss_t mode);

/* Retained 208-byte test-layout compatibility implementation.
 * The native 2022 entry is remove_abilities_by_test_2022 below. */


/* ManalinkEh 2022: 0x26ddb20; original functions/abilities.c:355. */
void remove_abilities(int player, int card, event_t event, ability_loss_t mode)
{
  ability_loss_t aVar1;
  card_instance_t *instance;

  instance = in_play(affected_card_controller, affected_card);
  if (instance != NULL)
  {
    aVar1 = instance->targets[0x11].card;
    if (-1 < (int)aVar1)
    {
      mode = mode | aVar1;
    }
    instance->targets[0x11].card = mode;
  }
  return;
}

/* ManalinkEh 2022: 0x26ddb60; original functions/abilities.c:367. */
int has_an_activated_ability(iid_t iid)
{
  int iVar1;
  char cVar2;
  int iVar3;
  unsigned int uVar4;
  unsigned short uVar5;

  iVar3 = 0;
  if ((cards_data[iid].type & 0x47) != 0)
  {
    iVar1 = *(int *)&cards_data[iid].id;
    uVar5 = (unsigned short)iVar1;
    if (uVar5 != 0x2baa)
    {
      cVar2 = (char)iVar1;
      if (uVar5 < 0x2bab)
      {
        if (uVar5 == 0x1710)
        {
          return 0;
        }
        if (uVar5 < 0x1711)
        {
          if (uVar5 == 0xed1)
          {
            return 0;
          }
          if (uVar5 < 0xed2)
          {
            if (uVar5 == 0xba8)
            {
              return 0;
            }
            if (uVar5 < 0xba9)
            {
              if (uVar5 == 0x7fd)
              {
                return 0;
              }
              if (uVar5 < 0x7fe)
              {
                if (uVar5 == 0x2ad)
                {
                  return 0;
                }
                if (uVar5 == 0x789)
                {
                  return 0;
                }
                if (uVar5 == 0x50)
                {
                  return 0;
                }
              }
              else
              {
                if (uVar5 == 0xac6)
                {
                  return 0;
                }
                if (uVar5 == 0xb84)
                {
                  return 0;
                }
              }
            }
            else
            {
              if (uVar5 == 0xcb7)
              {
                return 0;
              }
              if (uVar5 < 0xcb8)
              {
                if (uVar5 == 0xbcc)
                {
                  return 0;
                }
                if (uVar5 == 0xc30)
                {
                  return 0;
                }
                if (uVar5 == 0xbaa)
                {
                  return 0;
                }
              }
              else
              {
                if (uVar5 == 0xd57)
                {
                  return 0;
                }
                if (uVar5 == 0xd72)
                {
                  return 0;
                }
              }
            }
          }
          else
          {
            if (uVar5 == 0x1530)
            {
              return 0;
            }
            if (uVar5 < 0x1531)
            {
              if (uVar5 == 0x1261)
              {
                return 0;
              }
              if (uVar5 < 0x1262)
              {
                if (uVar5 == 0x11f2)
                {
                  return 0;
                }
                if (uVar5 == 0x1227)
                {
                  return 0;
                }
                if (uVar5 == 0xf7c)
                {
                  return 0;
                }
              }
              else if (uVar5 < 0x1266)
              {
                if (0x1262 < uVar5)
                {
                  return 0;
                }
              }
              else if (uVar5 == 0x144c)
              {
                return 0;
              }
            }
            else
            {
              if (uVar5 == 0x158a)
              {
                return 0;
              }
              if (uVar5 < 0x158b)
              {
                if (uVar5 == 0x155c)
                {
                  return 0;
                }
                if (uVar5 == 0x1580)
                {
                  return 0;
                }
              }
              else
              {
                if (uVar5 == 0x161f)
                {
                  return 0;
                }
                if (uVar5 == 0x1652)
                {
                  return 0;
                }
              }
            }
          }
        }
        else
        {
          if (uVar5 == 0x2532)
          {
            return 0;
          }
          if (uVar5 < 0x2533)
          {
            if (uVar5 == 0x1f18)
            {
              return 0;
            }
            if (uVar5 < 0x1f19)
            {
              if (uVar5 == 0x1b7a)
              {
                return 0;
              }
              if (uVar5 < 0x1b7b)
              {
                if (uVar5 == 0x1998)
                {
                  return 0;
                }
                if (uVar5 == 0x1a6a)
                {
                  return 0;
                }
                if (uVar5 == 0x1805)
                {
                  return 0;
                }
              }
              else
              {
                if (uVar5 == 0x1beb)
                {
                  return 0;
                }
                if (uVar5 == 0x1ea3)
                {
                  return 0;
                }
              }
            }
            else
            {
              if (uVar5 == 0x21f5)
              {
                return 0;
              }
              if (uVar5 < 0x21f6)
              {
                if (uVar5 == 0x20bd)
                {
                  return 0;
                }
                if (uVar5 == 0x215c)
                {
                  return 0;
                }
                if (uVar5 == 0x20a0)
                {
                  return 0;
                }
              }
              else
              {
                if (uVar5 == 0x230a)
                {
                  return 0;
                }
                if (uVar5 == 0x238d)
                {
                  return 0;
                }
              }
            }
          }
          else if (uVar5 < 0x298f)
          {
            if (uVar5 < 0x2982)
            {
              if (uVar5 == 0x2631)
              {
                return 0;
              }
              if (uVar5 < 0x2632)
              {
                if (uVar5 == 0x259e)
                {
                  return 0;
                }
                if (uVar5 == 0x260a)
                {
                  return 0;
                }
                if (uVar5 == 0x2544)
                {
                  return 0;
                }
              }
              else
              {
                if (uVar5 == 0x2689)
                {
                  return 0;
                }
                if (uVar5 == 0x28bd)
                {
                  return 0;
                }
              }
            }
            else if ((1 << (cVar2 + 0x7eU & 0x1f) & 0x1201U) != 0)
            {
              return 0;
            }
          }
          else
          {
            if (uVar5 == 0x2b2b)
            {
              return 0;
            }
            if (uVar5 == 0x2b83)
            {
              return 0;
            }
            if (uVar5 == 0x29af)
            {
              return 0;
            }
          }
        }
      }
      else
      {
        if (uVar5 == 0x3471)
        {
          return 0;
        }
        if (uVar5 < 0x3472)
        {
          if (uVar5 == 0x3163)
          {
            return 0;
          }
          if (uVar5 < 0x3164)
          {
            if (uVar5 == 0x2f0b)
            {
              return 0;
            }
            if (uVar5 < 0x2f0c)
            {
              if (uVar5 == 0x2d8e)
              {
                return 0;
              }
              if (uVar5 < 0x2d8f)
              {
                if (uVar5 == 0x2c5e)
                {
                  return 0;
                }
                if (uVar5 == 0x2d02)
                {
                  return 0;
                }
                if (uVar5 == 0x2bd8)
                {
                  return 0;
                }
              }
              else if ((iVar1 - 0x2ddeU & 0xffbf) == 0)
              {
                return 0;
              }
            }
            else
            {
              if (uVar5 == 0x3046)
              {
                return 0;
              }
              if (uVar5 < 0x3047)
              {
                if (uVar5 == 0x2ff2)
                {
                  return 0;
                }
                if (uVar5 == 0x300a)
                {
                  return 0;
                }
                if (uVar5 == 0x2f2f)
                {
                  return 0;
                }
              }
              else
              {
                if (uVar5 == 0x307d)
                {
                  return 0;
                }
                if (uVar5 == 0x3136)
                {
                  return 0;
                }
              }
            }
          }
          else
          {
            if (uVar5 == 0x323d)
            {
              return 0;
            }
            if (uVar5 < 0x323e)
            {
              if (uVar5 < 0x320e)
              {
                if (uVar5 < 0x31f5)
                {
                  if (uVar5 == 0x31a1)
                  {
                    return 0;
                  }
                  if (uVar5 == 0x31cd)
                  {
                    return 0;
                  }
                }
                else if ((1 << (cVar2 + 0xbU & 0x1f) & 0x1400005U) != 0)
                {
                  return 0;
                }
              }
            }
            else
            {
              if (uVar5 == 0x330a)
              {
                return 0;
              }
              if (uVar5 < 0x330b)
              {
                if (uVar5 == 0x327a)
                {
                  return 0;
                }
                if (uVar5 == 0x32d9)
                {
                  return 0;
                }
              }
              else
              {
                if (uVar5 == 0x331a)
                {
                  return 0;
                }
                if (uVar5 == 0x337a)
                {
                  return 0;
                }
              }
            }
          }
        }
        else
        {
          if (uVar5 == 0x382e)
          {
            return 0;
          }
          if (uVar5 < 0x382f)
          {
            if (uVar5 == 0x35cd)
            {
              return 0;
            }
            if (uVar5 < 0x35ce)
            {
              if (uVar5 < 0x358f)
              {
                if (uVar5 < 0x3570)
                {
                  if (uVar5 == 0x34d1)
                  {
                    return 0;
                  }
                  if (uVar5 == 0x3547)
                  {
                    return 0;
                  }
                }
                else if ((1 << (cVar2 + 0x90U & 0x1f) & 0x40038003U) != 0)
                {
                  return 0;
                }
              }
              else if (uVar5 == 0x35b2)
              {
                return 0;
              }
            }
            else
            {
              if (uVar5 == 0x3700)
              {
                return 0;
              }
              if (uVar5 < 0x3701)
              {
                if (uVar5 == 0x35df)
                {
                  return 0;
                }
                if (uVar5 == 0x3681)
                {
                  return 0;
                }
              }
              else
              {
                if (uVar5 == 0x3704)
                {
                  return 0;
                }
                if (uVar5 == 0x37e7)
                {
                  return 0;
                }
              }
            }
          }
          else
          {
            if (uVar5 == 0x3f81)
            {
              return 0;
            }
            if (uVar5 < 0x3f82)
            {
              if (uVar5 == 0x3d25)
              {
                return 0;
              }
              if (uVar5 < 0x3d26)
              {
                if (uVar5 == 0x3bed)
                {
                  return 0;
                }
                if (uVar5 == 0x3c9c)
                {
                  return 0;
                }
                if (uVar5 == 0x3a4b)
                {
                  return 0;
                }
              }
              else
              {
                if (uVar5 == 0x3e38)
                {
                  return 0;
                }
                if (uVar5 == 0x3f37)
                {
                  return 0;
                }
              }
            }
            else
            {
              if (uVar5 == 0x4330)
              {
                return 0;
              }
              if (uVar5 < 0x4331)
              {
                if (uVar5 < 0x4186)
                {
                  if (0x4183 < uVar5)
                  {
                    return 0;
                  }
                }
                else if (uVar5 == 0x42a7)
                {
                  return 0;
                }
              }
              else
              {
                if (uVar5 == 0x4837)
                {
                  return 0;
                }
                if (uVar5 == 0x4b9c)
                {
                  return 0;
                }
              }
            }
          }
        }
      }
      if (((cards_data[iid].extra_ability & 0x1000) != 0) || (uVar5 == 0x36eb))
      {
        return 2;
      }
      iVar3 = 1;
      if ((cards_data[iid].extra_ability & 3) == 0)
      {
        uVar4 = check_card_for_rules_engine(iid);
        return uVar4 >> 0xc & 1;
      }
    }
  }
  return iVar3;
}

/* ManalinkEh 2022: 0x26e0db0; original functions/abilities.c:3063. */
void protection_from_everything(int player, int card, event_t event)
{
  if (event == EVENT_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      set_misc_protection_impl(player, card);
      return;
    }
  }
  else if (((event == EVENT_PROT_MISC) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    event_result = event_result | 1;
    return;
  }
  return;
}


/* ManalinkEh 2022: 0x26dd200; original functions/abilities.c:2930. */
static void get_protection_source_impl(int8_t can_return_dmg, int *eff_acc, int *eff_ac)
{
  char cVar1;
  int result;
  unsigned int uVar3;
  card_instance_t *instance;
  int32_t card;
  int32_t player;

  *eff_acc = attacking_card_controller;
  result = attacking_card;
  *eff_ac = attacking_card;
  instance = get_card_instance(*eff_acc, result);
  if (instance == NULL)
  {
    return;
  }
  do
  {
    while (result = instance->internal_card_id, result == damage_card)
    {
      if (can_return_dmg != '\0')
      {
        return;
      }
      cVar1 = instance->damage_source_player;
      result = instance->damage_source_card;
      instance = get_card_instance((int)cVar1, result);
      if (instance == NULL)
      {
        return;
      }
      *eff_acc = (int)cVar1;
      *eff_ac = result;
    }
    if (result == activation_card)
    {
      player = instance->parent_controller;
      card = instance->parent_card;
    }
    else
    {
      uVar3 = (unsigned int)cards_data[result].id;
      if ((uint16_t)uVar3 == 0x386)
      {
        player = (int32_t)instance->damage_target_player;
        if (-1 < instance->damage_target_player)
        {
          card = instance->damage_target_card;
          goto LAB_026dd2b7;
        }
      }
      else if (1 < (uVar3 & 0xffff) - 0x386)
      {
        return;
      }
      if (result == LEGACY_EFFECT_ALADDIN)
      {
        player = instance->targets[0].player;
        card = instance->targets[0].card;
      }
      else
      {
        player = (int32_t)instance->damage_source_player;
        card = instance->damage_source_card;
      }
    }
  LAB_026dd2b7:
    instance = get_card_instance(player, card);
    if (instance == NULL)
    {
      return;
    }
    *eff_acc = player;
    *eff_ac = card;
  } while (true);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x26e0e10; original functions/abilities.c:3070. */
void protection_from_colored_spells(int player, int card, event_t event)
{
  int player_00;
  char cVar1;
  int iVar2;
  unsigned int uVar3;
  card_instance_t *instance;
  int iVar5;

  player_00 = attacking_card_controller;
  iVar2 = attacking_card;
  if (event == EVENT_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      set_misc_protection_impl(player, card);
      return;
    }
  }
  else if (((event == EVENT_PROT_MISC) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    cVar1 = (char)((unsigned int)event_result >> 0x18);
    if (cVar1 == '\x06')
    {
      event_result = event_result | 1;
    }
    else
    {
      if (cVar1 == '\x01')
      {

        instance = get_card_instance(attacking_card_controller, attacking_card);
        if (instance->internal_card_id != damage_card)
        {
          backtrace_and_abort("functions/abilities.c:3078\nget_card_instance(p, "
                              "c)->internal_card_id == damage_card");
        }
        uVar3 = get_color(player_00, iVar2);
        if ((uVar3 & 0x3e) == 0)
        {
          return;
        }
        iVar5 = is_being_cast(player_00, iVar2);
        if (iVar5 == 0)
        {
          return;
        }
        instance = get_card_instance(player_00, iVar2);
        if ((instance->targets[3].player & 0x47) != 0)
        {
          return;
        }
      }
      else
      {
        if (cVar1 != '\x05')
        {
          return;
        }
        iVar2 = is_being_cast(attacking_card_controller, attacking_card);
        if (iVar2 == 0)
        {
          return;
        }
        uVar3 = get_color(attacking_card_controller, attacking_card);
        if ((uVar3 & 0x3e) == 0)
        {
          return;
        }
      }
      event_result = event_result | 1;
    }
  }
  return;
}


/* ManalinkEh 2022: 0x26dd130; original functions/abilities.c:665. */
static void set_misc_protection_impl(int player, int card)
{
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  instance->ext->sp_keywords2 = instance->ext->sp_keywords2 | 1;
  return;
}


/* ManalinkEh 2022: 0x26df800; original functions/abilities.c:1250. */
int get_special_abilities_by_instance(card_instance_t *instance)
{
  unsigned int uVar1;
  unsigned int uVar2;

  uVar2 = instance->ext->sp_keywords;
  if ((instance->state & 0x2000) != 0)
  {
    uVar2 = uVar2 | 8;
    instance->ext->sp_keywords = uVar2;
  }
  uVar1 = instance->targets[0x11].card;
  if (0 < (int)uVar1)
  {
    if ((uVar1 & 0x40) != 0)
    {
      uVar2 = 0;
    }
    if ((uVar1 & 0x80) != 0)
    {
      uVar2 = uVar2 | 0x800;
    }
  }
  return uVar2;
}


/* ManalinkEh 2022: 0x26e05a0; original functions/abilities.c:2793. */
void remove_special_ability(int player, int card, int sa)
{
  card_instance_t *instance;
  instance = get_card_instance(player, card);
  instance->ext->sp_keywords = instance->ext->sp_keywords & (sa ^ 0xffffffffU);
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void squirrellink(int player, int card, event_t event);
void trample_over_planeswalkers(int player, int card, event_t event);
void misc_hexproof(int player, int card, event_t event);
void minblockers_3(int player, int card, event_t event);
void maxblockers_1(int player, int card, event_t event);
void special_abilities2(int player, int card, event_t event, sp_keyword2_t sp_keyword2);

/* ManalinkEh 2022: 0x26de3c0; original functions/abilities.c:602. */
void special_abilities2(int player, int card, event_t event, sp_keyword2_t sp_keyword2)
{
  int result;
  card_instance_t *instance;

  if (((event != EVENT_ABILITIES) || (card != affected_card)) ||
      (player != affected_card_controller))
  {
    return;
  }
  if (((player != -1) && (card != -1)) &&
      (result = is_humiliated(player, card, AL_KEY), result != 0))
  {
    return;
  }
  instance = get_card_instance(player, card);
  instance->ext->sp_keywords2 = instance->ext->sp_keywords2 | sp_keyword2;
  return;
}

/* ManalinkEh 2022: 0x26de6f0; original functions/abilities.c:660. */
void squirrellink(int player, int card, event_t event)
{
  card_instance_t *instance;

  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {

    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x20000000;
    return;
  }
  return;
}

/* ManalinkEh 2022: 0x26de830; original functions/abilities.c:664. */
void trample_over_planeswalkers(int player, int card, event_t event)
{
  card_instance_t *instance;

  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {

    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x40000;
    return;
  }
  return;
}

/* ManalinkEh 2022: 0x26de8b0; original functions/abilities.c:666. */
void misc_hexproof(int player, int card, event_t event)
{
  card_instance_t *instance;

  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {

    instance = get_card_instance(player, card);
    instance->ext->sp_keywords2 = instance->ext->sp_keywords2 | 4;
    return;
  }
  return;
}

/* ManalinkEh 2022: 0x26df640; original functions/abilities.c:1183. */
void minblockers_3(int player, int card, event_t event)
{
  card_instance_t *instance;

  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {

    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x80000;
    return;
  }
  return;
}

/* ManalinkEh 2022: 0x26df690; original functions/abilities.c:1191. */
void maxblockers_1(int player, int card, event_t event)
{
  card_instance_t *instance;

  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {

    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x200000;
    return;
  }
  return;
}


/* ManalinkEh 2022: 0x26d7530; original functions/abilities.c:1269. */
static int has_sp_key_by_csvid_impl(int csvid, int sp_key)
{
  char cVar1;
  unsigned int uVar2;
  unsigned int uVar3;

  uVar2 = sp_key & 1;
  cVar1 = (char)csvid;
  if ((uVar2 == 0) || (csvid == 0x1cae))
    goto LAB_026d7540;
  if (csvid < 0x1caf)
  {
    if (csvid == 0x1287)
      goto LAB_026d7540;
    if (0x1287 < csvid)
    {
      if (csvid != 0x1ab6)
      {
        if (csvid < 0x1ab7)
        {
          if (csvid != 0x1744)
          {
            if (csvid < 0x1745)
            {
              if (csvid != 0x12bf)
              {
                uVar2 = (unsigned int)(csvid == 0x15c2);
              }
            }
            else
            {
              uVar2 = (unsigned int)(csvid == 0x19fd);
            }
          }
        }
        else if (csvid != 0x1b7c)
        {
          if (csvid < 0x1b7d)
          {
            if (csvid != 0x1abe)
            {
              uVar2 = (unsigned int)(csvid == 0x1ae1);
            }
          }
          else
          {
            uVar2 = (unsigned int)(csvid == 0x1c96);
          }
        }
      }
      goto LAB_026d7540;
    }
    if (csvid == 0xae4)
      goto LAB_026d7540;
    if (0xae4 < csvid)
    {
      if (csvid != 0x1063)
      {
        if (csvid < 0x1064)
        {
          if (csvid != 0xbd4)
          {
            uVar2 = (unsigned int)(csvid == 0xc25);
          }
        }
        else
        {
          uVar2 = (unsigned int)(csvid == 0x1139);
        }
      }
      goto LAB_026d7540;
    }
    if (csvid == 0x4c4)
      goto LAB_026d7540;
    if (csvid < 0x4c5)
    {
      if (csvid != 0x416)
      {
        uVar2 = (unsigned int)(csvid == 0x479);
      }
      goto LAB_026d7540;
    }
    if (csvid == 0x69a)
      goto LAB_026d7540;
    uVar2 = 0;
    if ((sp_key & 2U) == 0)
      goto LAB_026d789a;
    uVar2 = 0;
  LAB_026d9c11:
    if ((csvid == 0x54d) || (csvid == 0x585))
      goto LAB_026d7897;
    if ((sp_key & 4U) == 0)
      goto LAB_026d75bd;
  LAB_026d78b4:
    if (csvid == 0x1de1)
      goto LAB_026d8d12;
    if (0x1de1 < csvid)
    {
    LAB_026dc27a:
      if (csvid < 0x2841)
      {
        if (0x2826 < csvid)
        {
          uVar3 = 1U << (cVar1 - 0x27U & 0x1f) & 0x2020401;
        joined_r0x026dc2fb:
          if (uVar3 != 0)
            goto LAB_026d8d12;
          goto LAB_026d75bd;
        }
      LAB_026dc28c:
        if (0x234f < csvid)
          goto LAB_026d9af8;
        if (0x2339 < csvid)
        {
          uVar3 = 1U << (cVar1 - 0x3aU & 0x1f) & 0x204001;
          goto joined_r0x026dc2fb;
        }
        if (csvid == 0x20d4)
          goto LAB_026d8d12;
        if (csvid < 0x20d5)
        {
          if (csvid == 0x1efe)
            goto LAB_026d8d12;
          if (csvid < 0x1eff)
          {
            if (csvid == 0x1eb3)
              goto LAB_026d8d12;
            if (0x1eb3 < csvid)
            {
              if (csvid == 0x1ec7)
                goto LAB_026d8d12;
              if ((sp_key & 8U) == 0)
                goto LAB_026d7927;
              goto LAB_026d93dd;
            }
            if ((csvid == 0x1e1e) || (csvid == 0x1e40))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026da6c2:
            if ((csvid != 0x1e50) && (csvid != 0x1e80))
              goto LAB_026da6d8;
            goto LAB_026d814b;
          }
          if (csvid == 0x2067)
            goto LAB_026d8d12;
          if (csvid < 0x2068)
          {
            if ((csvid == 0x1f7b) || (csvid == 0x201d))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
            goto LAB_026daddf;
          }
          if (csvid == 0x20b3)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        }
        else
        {
          if (csvid == 0x22a5)
            goto LAB_026d8d12;
          if (0x22a5 < csvid)
          {
            if ((csvid != 0x22ef) && ((csvid & 0xffffffefU) != 0x22c2))
              goto LAB_026d75bd;
            goto LAB_026d8d12;
          }
          if (csvid == 0x2269)
            goto LAB_026d8d12;
          if (0x2269 < csvid)
          {
            if (csvid != 0x22a0)
              goto LAB_026d75bd;
            goto LAB_026d8d12;
          }
          if ((csvid == 0x2123) || (csvid == 0x21a8))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d9677:
          if (csvid == 0x21d7)
            goto LAB_026d814b;
          if (0x21d7 < csvid)
            goto LAB_026d9688;
        LAB_026daddf:
          if (csvid == 0x1ffc)
            goto LAB_026d814b;
          if (csvid < 0x1ffd)
          {
            if (csvid < 0x1eff)
            {
              if (0x1ee7 < csvid)
              {
                uVar3 = 1U << (cVar1 + 0x18U & 0x1f) & 0x400011;
                goto joined_r0x026dae35;
              }
            }
            else if (csvid == 0x1f96)
              goto LAB_026d814b;
            goto LAB_026d7927;
          }
        }
        if (((csvid - 0x2186U & 0xfffffffb) != 0) && (csvid != 0x2088))
          goto LAB_026d7927;
        goto LAB_026d814b;
      }
    LAB_026dbe0d:
      if (csvid == 0x2cc1)
        goto LAB_026d8d12;
      if (csvid < 0x2cc2)
      {
      LAB_026dbe1e:
        if (csvid == 0x2a50)
          goto LAB_026d8d12;
        if (0x2a50 < csvid)
          goto LAB_026dbe2f;
        if (csvid < 0x2935)
        {
          if ((0x2932 < csvid) || (csvid == 0x28b6))
            goto LAB_026d8d12;
          if (csvid < 0x28b7)
          {
            if ((csvid == 0x2864) || (csvid == 0x2878))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
            goto LAB_026dc16f;
          }
          if (csvid == 0x291d)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026dc148:
          if (csvid != 0x292c)
          {
            if (0x292c < csvid)
              goto LAB_026dbff8;
            if (csvid != 0x280a)
              goto LAB_026dc164;
          }
          goto LAB_026d814b;
        }
        if (csvid == 0x29df)
          goto LAB_026d8d12;
        if (0x29df < csvid)
        {
          if (csvid != 0x29fe)
            goto LAB_026d75bd;
          goto LAB_026d8d12;
        }
        if ((csvid & 0xffffff7fU) == 0x2946)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026dc030:
        if ((csvid != 0x2953) && (csvid != 0x29a6))
          goto LAB_026d7652;
        goto LAB_026d814b;
      }
    LAB_026dc0c2:
      if (csvid == 0x2fc6)
        goto LAB_026d8d12;
      if (csvid < 0x2fc7)
      {
      LAB_026dbd63:
        if (csvid == 0x2ea7)
          goto LAB_026d8d12;
        if (csvid < 0x2ea8)
        {
          if (csvid == 0x2df2)
            goto LAB_026d8d12;
          if (csvid < 0x2df3)
          {
            if ((csvid == 0x2cc3) || (csvid == 0x2da6))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
            goto LAB_026dc137;
          }
          if (csvid == 0x2e0d)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026dbf10:
          if (csvid != 0x2e5e)
          {
            if (0x2e5e < csvid)
              goto LAB_026dbf1d;
            if (csvid != 0x2d5f)
            {
              if (csvid < 0x2d60)
              {
                if ((csvid == 0x2d3e) || (csvid == 0x2d4b))
                  goto LAB_026d814b;
              }
              else if (csvid == 0x2df3)
                goto LAB_026d814b;
              goto LAB_026d7652;
            }
          }
        }
        else
        {
          if (csvid == 0x2f90)
            goto LAB_026d8d12;
          if (0x2f90 < csvid)
          {
            if (csvid == 0x2fad)
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026dbda0:
            if (csvid == 0x2fc3)
              goto LAB_026d814b;
            if (0x2fc3 < csvid)
              goto LAB_026dbdb1;
            goto LAB_026dbf10;
          }
          if ((csvid == 0x2eec) || (csvid == 0x2f8c))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026dbf1d:
          if (((csvid != 0x2eba) && (csvid != 0x2f40)) && (csvid != 0x2ea2))
            goto LAB_026d7652;
        }
      }
      else
      {
        if (csvid == 0x3081)
          goto LAB_026d8d12;
        if (0x3081 < csvid)
        {
          if (((csvid == 0x3113) || (csvid == 0x313c)) || (csvid == 0x30e2))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          goto LAB_026d885f;
        }
        if (csvid == 0x302e)
          goto LAB_026d8d12;
        if (0x302e < csvid)
        {
          if (csvid != 0x3051)
            goto LAB_026d75bd;
          goto LAB_026d8d12;
        }
        if ((csvid == 0x2fef) || (csvid == 0x3015))
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026dbdb1:
        if (csvid != 0x3020)
        {
          if (csvid < 0x3021)
          {
            if (csvid < 0x3001)
            {
              if ((0x2ffe < csvid) || (csvid == 0x2fdd))
                goto LAB_026d814b;
            }
            else if (csvid == 0x3016)
              goto LAB_026d814b;
            goto LAB_026d7652;
          }
          if ((csvid != 0x30c6) && ((csvid != 0x30ef && (csvid != 0x30a2))))
            goto LAB_026d7652;
        }
      }
      goto LAB_026d814b;
    }
    if (csvid == 0xff2)
      goto LAB_026d8d12;
    if (0xff2 < csvid)
    {
      if (csvid == 0x16c0)
        goto LAB_026d8d12;
      if (0x16c0 < csvid)
      {
        if (csvid == 0x1a14)
          goto LAB_026d8d12;
        if (csvid < 0x1a15)
        {
          if (csvid == 0x18cf)
            goto LAB_026d8d12;
          if (csvid < 0x18d0)
          {
            if (csvid == 0x17b5)
              goto LAB_026d8d12;
            if (0x17b5 < csvid)
            {
              if ((csvid != 0x183b) && (csvid != 0x1858))
                goto LAB_026d75bd;
              goto LAB_026d8d12;
            }
            if ((csvid == 0x170c) || (csvid == 0x17a9))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          }
          else
          {
            if (csvid == 0x1925)
              goto LAB_026d8d12;
            if (0x1925 < csvid)
            {
              if (csvid == 0x198f)
                goto LAB_026d8d12;
              if ((sp_key & 8U) == 0)
                goto LAB_026d7927;
            LAB_026da736:
              if ((csvid != 0x199d) && (csvid != 0x1b1d))
                goto LAB_026d7927;
              goto LAB_026d814b;
            }
            if ((csvid == 0x18d3) || (csvid == 0x190b))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026d93ee:
            if (csvid == 0x1909)
              goto LAB_026d814b;
            if (0x1909 < csvid)
              goto LAB_026da6b1;
          }
          if (csvid != 0x16ce)
          {
            if (csvid < 0x16cf)
            {
            LAB_026da840:
              if (csvid != 0x15fc)
              {
                if (csvid < 0x15fd)
                {
                  if (csvid != 0x14c6)
                    goto joined_r0x026da892;
                }
                else if (csvid != 0x1677)
                  goto LAB_026d7927;
              }
            }
            else if (((csvid != 0x16ee) && (csvid != 0x171a)) && (csvid != 0x16e8))
              goto LAB_026d7927;
          }
        }
        else
        {
          if (csvid == 0x1bf2)
            goto LAB_026d8d12;
          if (csvid < 0x1bf3)
          {
            if (csvid == 0x1b11)
              goto LAB_026d8d12;
            if (csvid < 0x1b12)
            {
              if (csvid < 0x1aaf)
              {
                if (0x1aac < csvid)
                  goto LAB_026d8d12;
              }
              else if (csvid == 0x1abf)
                goto LAB_026d8d12;
              goto LAB_026d75bd;
            }
            if (csvid == 0x1ba3)
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026da6b1:
            if (csvid != 0x1bd0)
            {
              if (0x1bd0 < csvid)
                goto LAB_026da6c2;
              if (csvid != 0x1bac)
              {
                if (csvid < 0x1bad)
                  goto LAB_026da736;
                if (csvid != 0x1bc0)
                  goto LAB_026d7927;
              }
            }
          }
          else
          {
            if (csvid == 0x1d0c)
              goto LAB_026d8d12;
            if (0x1d0c < csvid)
            {
              if (csvid != 0x1db0)
                goto LAB_026d75bd;
              goto LAB_026d8d12;
            }
            if ((csvid == 0x1c6c) || (csvid == 0x1cc1))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026da6d8:
            if (csvid != 0x1c77)
              goto LAB_026d7927;
          }
        }
        goto LAB_026d814b;
      }
      if (csvid == 0x1390)
        goto LAB_026d8d12;
      if (csvid < 0x1391)
      {
        if (csvid < 0x10de)
        {
          if (0x10d8 < csvid)
          {
            uVar3 = 1U << (cVar1 + 0x27U & 0x1f) & 0x15;
          joined_r0x026da8e8:
            if (uVar3 == 0)
              goto LAB_026d75bd;
            goto LAB_026d8d12;
          }
          if ((csvid == 0x1071) || ((csvid & 0xffffffefU) == 0x1024))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d8d80:
          if (csvid < 0x108f)
          {
            if (0x1074 < csvid)
            {
              uVar3 = 1U << (cVar1 + 0x8bU & 0x1f) & 0x2200001;
            joined_r0x026dae35:
              if (uVar3 != 0)
                goto LAB_026d814b;
            }
          }
          else if (csvid == 0x1108)
            goto LAB_026d814b;
        }
        else
        {
          if (csvid == 0x11a7)
            goto LAB_026d8d12;
          if (csvid < 0x11a8)
          {
            if ((csvid == 0x117e) || (csvid == 0x11a1))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026d8d6a:
            if (csvid == 0x110f)
              goto LAB_026d814b;
            if (csvid < 0x1110)
              goto LAB_026d8d80;
          }
          else
          {
            if (csvid == 0x1298)
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          }
          if (((csvid == 0x120f) || (csvid == 0x1345)) || (csvid == 0x119b))
            goto LAB_026d814b;
        }
      }
      else
      {
        if (csvid == 0x15d4)
          goto LAB_026d8d12;
        if (0x15d4 < csvid)
        {
          if (csvid == 0x1696)
            goto LAB_026d8d12;
          if (0x1696 < csvid)
          {
            if (csvid != 0x16bb)
              goto LAB_026d75bd;
            goto LAB_026d8d12;
          }
          if ((csvid == 0x15e7) || (csvid == 0x160d))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          goto LAB_026da840;
        }
        if (csvid == 0x1502)
          goto LAB_026d8d12;
        if (csvid < 0x1503)
        {
          if ((csvid == 0x13d9) || (csvid == 0x13fe))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          goto LAB_026d8d37;
        }
        if (csvid == 0x151b)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      joined_r0x026da892:
        if (csvid == 0x157e)
          goto LAB_026d814b;
      }
      goto LAB_026d7927;
    }
    if (csvid == 0x602)
      goto LAB_026d8d12;
    if (csvid < 0x603)
    {
      if (csvid < 0x4d4)
      {
        if (csvid < 0x4c5)
        {
          if (csvid < 0x430)
          {
            if (csvid < 0x417)
            {
              if ((csvid == 0xab) || (csvid == 0x127))
                goto LAB_026d8d12;
              if ((sp_key & 8U) == 0)
                goto LAB_026d7927;
              goto LAB_026d91df;
            }
            if ((1U << (cVar1 - 0x17U & 0x1f) & 0x1100001U) != 0)
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026d91f0:
            if (((csvid & 0xffffffbfU) == 0x3bb) || (csvid == 0x381))
              goto LAB_026d814b;
          }
          else
          {
            if (((csvid == 0x4a0) || (csvid == 0x4ae)) || (csvid == 0x462))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026d910e:
            if (0x4ae < csvid)
              goto LAB_026d9119;
          LAB_026d91df:
            if (csvid == 0x35b)
              goto LAB_026d814b;
            if (0x35b < csvid)
              goto LAB_026d91f0;
            if (csvid == 0x242)
              goto LAB_026d814b;
            if (csvid < 0x243)
            {
              if ((csvid == 0xdd) || (csvid == 0x226))
                goto LAB_026d814b;
            }
            else if (csvid == 0x2f3)
              goto LAB_026d814b;
          }
        }
        else
        {
          if ((1U << (cVar1 + 0x3bU & 0x1f) & 0x4011U) != 0)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d923a:
          if (0x4c9 < csvid)
            goto LAB_026da2ed;
          if (0x4c7 < csvid)
            goto LAB_026d814b;
        }
      }
      else
      {
        if (0x531 < csvid)
        {
          if (((csvid == 0x57b) || (csvid == 0x5c4)) || (csvid == 0x557))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d9119:
          if (csvid == 0x560)
            goto LAB_026d814b;
          if (0x560 < csvid)
            goto LAB_026d9126;
        LAB_026d9172:
          if (csvid != 0x531)
          {
            if (csvid < 0x532)
              goto LAB_026d923a;
            if (csvid != 0x554)
              goto LAB_026d7927;
          }
          goto LAB_026d814b;
        }
        if (0x512 < csvid)
        {
          if ((1U << (cVar1 - 0x13U & 0x1f) & 0x40200001U) != 0)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          goto LAB_026d9172;
        }
        if ((csvid - 0x4d9U < 0x1d) &&
            ((1U << ((unsigned char)(csvid - 0x4d9U) & 0x1f) & 0x10400001U) != 0))
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026da2ed:
        if (csvid == 0x50c)
          goto LAB_026d814b;
      }
      goto LAB_026d7927;
    }
    if (csvid < 0xb4f)
    {
      if (0xb4c < csvid)
        goto LAB_026d8d12;
      if (csvid < 0x6a2)
      {
        if (0x686 < csvid)
        {
          switch (csvid)
          {
          case 0x687:
          case 0x689:
          case 0x690:
          case 0x695:
          case 0x6a1:
            goto LAB_026d8d12;
          default:
            goto LAB_026d75bd;
          }
        }
        if (csvid == 0x666)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026d9126:
        if (((csvid != 0x63c) && (csvid != 0x6d0)) && (csvid != 0x58c))
          goto LAB_026d7927;
        goto LAB_026d814b;
      }
      if (csvid == 0x951)
        goto LAB_026d8d12;
      if (csvid < 0x952)
      {
        if ((csvid == 0x744) || (csvid == 0x87b))
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
        goto LAB_026d8d48;
      }
      if (csvid == 0xae5)
        goto LAB_026d8d12;
      if ((sp_key & 8U) == 0)
        goto LAB_026d7927;
    LAB_026da598:
      if (csvid == 0xaeb)
        goto LAB_026d814b;
      if (csvid < 0xaec)
      {
        if ((csvid != 0x966) && (csvid != 0xaa5))
          goto LAB_026d7927;
        goto LAB_026d814b;
      }
      if (csvid == 0xd60)
        goto LAB_026d814b;
      if ((sp_key & 0x100U) == 0)
        goto LAB_026d7652;
      goto LAB_026da5b9;
    }
    if (csvid == 0xe4b)
      goto LAB_026d8d12;
    if (csvid < 0xe4c)
    {
      if (csvid == 0xd2e)
        goto LAB_026d8d12;
      if (csvid < 0xd2f)
      {
        if ((csvid != 0xbed) && (csvid != 0xd2a))
          goto LAB_026d75bd;
        goto LAB_026d8d12;
      }
      if (csvid == 0xe42)
        goto LAB_026d8d12;
      if ((sp_key & 8U) == 0)
        goto LAB_026d7927;
    LAB_026da51d:
      if (csvid == 0xdf3)
        goto LAB_026d814b;
      if (csvid < 0xdf4)
        goto LAB_026da598;
    }
    else
    {
      if (csvid == 0xf3b)
        goto LAB_026d8d12;
      if (0xf3b < csvid)
      {
        if (csvid == 0xf82)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026d8d59:
        if (csvid == 0xfcd)
          goto LAB_026d814b;
        if (0xfcd < csvid)
          goto LAB_026d8d6a;
        goto LAB_026da51d;
      }
      if ((csvid == 0xe97) || (csvid == 0xec8))
        goto LAB_026d8d12;
      if ((sp_key & 8U) == 0)
        goto LAB_026d7927;
    }
    if (((csvid == 0xe55) || (csvid == 0xed0)) || (csvid == 0xe4e))
      goto LAB_026d814b;
    if ((sp_key & 0x100U) == 0)
      goto LAB_026d7652;
  LAB_026da554:
    if (csvid != 0xdf6)
      goto joined_r0x026da562;
    goto LAB_026d7962;
  }
  if (csvid == 0x2398)
  {
  LAB_026d7540:
    if ((sp_key & 2U) == 0)
      goto LAB_026d789a;
    if (csvid < 0x3f99)
    {
      if (0x3f8c < csvid)
      {
        if ((1U << (cVar1 + 0x73U & 0x1f) & 0x811U) == 0)
          goto LAB_026d789a;
      LAB_026d7897:
        uVar2 = uVar2 | 2;
      LAB_026d789a:
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
        if (csvid == 0x3155)
          goto LAB_026d8d12;
        if (csvid < 0x3156)
          goto LAB_026d78b4;
      LAB_026d8cfa:
        if (csvid == 0x43b4)
          goto LAB_026d8d12;
        if (csvid < 0x43b5)
        {
        LAB_026d8d07:
          if (csvid == 0x3c13)
            goto LAB_026d8d12;
          if (0x3c13 < csvid)
          {
          LAB_026d907f:
            if (csvid != 0x4023)
            {
              if (0x4023 < csvid)
                goto LAB_026d9090;
              if (csvid < 0x3e97)
                goto LAB_026d9442;
              goto LAB_026d9544;
            }
            goto LAB_026d8d12;
          }
        LAB_026d8de9:
          if (csvid == 0x35cc)
            goto LAB_026d8d12;
          if (0x35cc < csvid)
            goto LAB_026d8dfa;
        LAB_026d8fdf:
          if (csvid == 0x32c1)
            goto LAB_026d8d12;
          if (0x32c1 < csvid)
            goto LAB_026d8ff0;
          if (0x31fe < csvid)
          {
            if ((csvid == 0x3231) || (csvid == 0x3256))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
            goto LAB_026da220;
          }
          if (0x31e4 < csvid)
          {
            uVar3 = 1U << (cVar1 + 0x1bU & 0x1f) & 0x2800001;
            goto joined_r0x026da8e8;
          }
          if (csvid == 0x31a7)
            goto LAB_026d8d12;
          if (0x31a7 < csvid)
          {
            if ((csvid == 0x31cd) || (csvid == 0x31d4))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
            goto LAB_026da1d8;
          }
          if ((csvid == 0x316e) || (csvid == 0x3182))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026da9a3:
          if (csvid == 0x3180)
            goto LAB_026d814b;
        }
        else
        {
        LAB_026d9323:
          if (0x4f8e < csvid)
            goto LAB_026d9737;
          if (csvid < 0x4f73)
          {
          LAB_026d9339:
            if (csvid != 0x4a9e)
            {
              if (csvid < 0x4a9f)
                goto LAB_026dae9c;
              if (csvid != 0x4d66)
              {
                if (0x4d66 < csvid)
                  goto LAB_026d935b;
                goto LAB_026d9e4f;
              }
            }
            goto LAB_026d8d12;
          }
          if ((1U << (cVar1 + 0x8dU & 0x1f) & 0x8000009U) != 0)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d96e1:
          if (csvid < 0x4f86)
          {
          LAB_026d9391:
            if (0x4f70 < csvid)
            {
              uVar3 = 1U << (cVar1 + 0x8fU & 0x1f) & 0x100041;
              goto joined_r0x026da0bb;
            }
            if ((csvid == 0x4f47) || (csvid == 0x4f60))
              goto LAB_026d814b;
          }
          else if ((csvid - 0x4fbfU & 0xfffffffd) == 0)
            goto LAB_026d814b;
        }
        goto LAB_026d7652;
      }
      if (csvid == 0x31fc)
        goto LAB_026d7897;
      if (csvid < 0x31fd)
      {
        if (csvid == 0x2b3b)
          goto LAB_026d7897;
        if (csvid < 0x2b3c)
        {
          if (csvid == 0x2777)
            goto LAB_026d7897;
          if (csvid < 0x2778)
            goto LAB_026d9aba;
          if (csvid == 0x2915)
            goto LAB_026d7897;
          if (csvid < 0x2916)
          {
            if (((csvid == 0x2892) || (csvid == 0x289e)) || (csvid == 0x2871))
              goto LAB_026d7897;
            if ((sp_key & 4U) == 0)
              goto LAB_026d75bd;
            goto LAB_026dc27a;
          }
          if (((csvid == 0x29cf) || (csvid == 0x2aab)) || (csvid == 0x292c))
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
          goto LAB_026dbe1e;
        }
        if (csvid == 0x2f87)
          goto LAB_026d7897;
        if (0x2f87 < csvid)
        {
          if (csvid == 0x30e9)
            goto LAB_026d7897;
          if (0x30e9 < csvid)
          {
            if ((csvid != 0x313d) && (csvid != 0x3198))
            {
              if (csvid == 0x30fa)
              {
                uVar2 = uVar2 | 2;
              }
              goto LAB_026d789a;
            }
            goto LAB_026d7897;
          }
          if (((csvid == 0x3062) || (csvid == 0x30ae)) || (csvid == 0x2f8b))
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
          goto LAB_026dc0c2;
        }
        if (csvid == 0x2dba)
          goto LAB_026d7897;
        if (0x2dba < csvid)
        {
          if (((csvid == 0x2e57) || (csvid == 0x2eed)) || (csvid == 0x2df5))
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
          goto LAB_026dbd63;
        }
        if (csvid == 0x2cb3)
          goto LAB_026d7897;
        if (0x2cb3 < csvid)
        {
          if (csvid == 0x2d3b)
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
          goto LAB_026dbe0d;
        }
        if ((csvid == 0x2ba4) || (csvid == 0x2c95))
          goto LAB_026d7897;
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026dbe2f:
        if (csvid == 0x2bca)
          goto LAB_026d8d12;
        if (csvid < 0x2bcb)
        {
          if (csvid < 0x2b53)
          {
            if (0x2b3b < csvid)
            {
              uVar3 = 1U << (cVar1 - 0x3cU & 0x1f) & 0x400081;
              goto joined_r0x026da8e8;
            }
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          }
          else
          {
            if (csvid == 0x2b73)
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026dbff8:
            if (csvid == 0x2bab)
              goto LAB_026d814b;
            if (0x2bab < csvid)
              goto LAB_026dbe70;
          }
          if (csvid != 0x2acc)
          {
            if (csvid < 0x2acd)
              goto LAB_026dc030;
            if (csvid != 0x2ada)
              goto LAB_026d7652;
          }
        }
        else
        {
          if (((csvid == 0x2c1c) || (csvid == 0x2c4c)) || (csvid == 0x2bf5))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026dbe70:
          if (((csvid != 0x2bff) && (csvid != 0x2cca)) && (csvid != 0x2be7))
            goto LAB_026d7652;
        }
        goto LAB_026d814b;
      }
      if (csvid == 0x3a2f)
        goto LAB_026d7897;
      if (csvid < 0x3a30)
      {
        if (csvid == 0x349b)
          goto LAB_026d7897;
        if (0x349b < csvid)
        {
          if (csvid == 0x3782)
            goto LAB_026d7897;
          if (csvid < 0x3783)
          {
            if (((csvid == 0x362a) || (csvid == 0x376c)) || (csvid == 0x356e))
              goto LAB_026d7897;
            if ((sp_key & 4U) == 0)
              goto LAB_026d75bd;
            goto LAB_026d8de9;
          }
          if (((csvid == 0x3927) || (csvid == 0x3a10)) || (csvid == 0x382b))
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
        LAB_026d8dfa:
          if (csvid == 0x38f3)
            goto LAB_026d8d12;
          if (0x38f3 < csvid)
          {
            if (csvid == 0x3b66)
              goto LAB_026d8d12;
            if (csvid < 0x3b67)
              goto LAB_026d8f78;
            if (csvid == 0x3ba5)
              goto LAB_026d8d12;
            if (0x3ba5 < csvid)
            {
              if (csvid == 0x3bd2)
                goto LAB_026d8d12;
              if ((sp_key & 8U) == 0)
                goto LAB_026d7927;
              goto LAB_026d8663;
            }
            if ((csvid == 0x3b6a) || (csvid == 0x3b9b))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026d8e67:
            if (csvid < 0x3b87)
            {
              if (0x3b6c < csvid)
              {
                uVar3 = 1U << (cVar1 + 0x93U & 0x1f) & 0x2000081;
                goto joined_r0x026da0bb;
              }
              goto LAB_026d8e7d;
            }
            if (csvid != 0x3bca)
              goto LAB_026d7652;
            goto LAB_026d814b;
          }
          if (csvid == 0x37fc)
            goto LAB_026d8d12;
          if (csvid < 0x37fd)
          {
            if (csvid == 0x3789)
              goto LAB_026d8d12;
            if (csvid < 0x378a)
            {
              if ((csvid == 0x35e3) || (csvid == 0x35f6))
                goto LAB_026d8d12;
              if ((sp_key & 8U) == 0)
                goto LAB_026d7927;
              goto LAB_026d9289;
            }
            if ((csvid == 0x37c6) || (csvid == 0x37e9))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026d92a5:
            if (csvid < 0x37ea)
            {
              if (((csvid == 0x3777) || (csvid == 0x3785)) || (csvid == 0x3764))
                goto LAB_026d814b;
              goto LAB_026d7652;
            }
          LAB_026d92b0:
            if ((csvid == 0x38c6) || (csvid == 0x38cb))
              goto LAB_026d814b;
          }
          else
          {
            if (csvid == 0x389e)
              goto LAB_026d8d12;
            if (0x389e < csvid)
            {
              if (csvid == 0x38d7)
                goto LAB_026d8d12;
              if ((sp_key & 8U) == 0)
                goto LAB_026d7927;
              goto LAB_026d92b0;
            }
            if ((csvid - 0x3855U & 0xffffffef) == 0)
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          }
          if (csvid != 0x3881)
            goto LAB_026d7652;
          goto LAB_026d814b;
        }
        if (0x33e8 < csvid)
        {
          if (csvid == 0x3493)
            goto LAB_026d7897;
        LAB_026dbcb1:
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
        LAB_026d9260:
          if (csvid - 0x34adU < 2)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          goto LAB_026d9278;
        }
        if (0x33c9 < csvid)
        {
          if ((1U << (cVar1 + 0x36U & 0x1f) & 0x40100001U) == 0)
            goto LAB_026dbcb1;
          goto LAB_026d7897;
        }
        if (csvid == 0x3345)
          goto LAB_026d7897;
        if (csvid < 0x3346)
        {
          if ((csvid == 0x3236) || (csvid == 0x330f))
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
          goto LAB_026d8fdf;
        }
        if (csvid == 0x33b2)
          goto LAB_026d7897;
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026d8ff0:
        if (csvid == 0x33ab)
          goto LAB_026d8d12;
        if (csvid < 0x33ac)
        {
          if (csvid == 0x335b)
            goto LAB_026d8d12;
          if (csvid < 0x335c)
          {
            if ((csvid == 0x32d3) || (csvid == 0x330f))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
            goto LAB_026da150;
          }
          if (csvid == 0x3377)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026da108:
          if (csvid != 0x339d)
          {
            if (csvid < 0x339e)
            {
              if ((csvid == 0x336a) || (csvid == 0x3378))
                goto LAB_026d814b;
            }
            else if (csvid == 0x33b0)
              goto LAB_026d814b;
            goto LAB_026d7652;
          }
        }
        else
        {
          if (csvid == 0x34fb)
            goto LAB_026d8d12;
          if (csvid < 0x34fc)
          {
            if (csvid != 0x33ba)
              goto LAB_026d9260;
            goto LAB_026d8d12;
          }
          if (csvid == 0x35a9)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d9028:
          if (csvid != 0x3544)
          {
            if (csvid - 0x355bU < 0x14)
            {
              uVar3 = 1U << ((unsigned char)(csvid - 0x355bU) & 0x1f) & 0x88001;
              goto joined_r0x026da0bb;
            }
            goto LAB_026d7652;
          }
        }
        goto LAB_026d814b;
      }
      if (csvid == 0x3e96)
        goto LAB_026d7897;
      if (csvid < 0x3e97)
      {
        if (csvid == 0x3d07)
          goto LAB_026d7897;
        if (csvid < 0x3d08)
        {
          if (csvid == 0x3b44)
            goto LAB_026d7897;
          if (0x3b44 < csvid)
          {
            if (csvid == 0x3b93)
              goto LAB_026d7897;
            if ((sp_key & 4U) == 0)
              goto LAB_026d75bd;
            goto LAB_026d8d07;
          }
          if ((csvid == 0x3a59) || (csvid == 0x3aab))
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
        LAB_026d8f78:
          if (csvid == 0x3a64)
            goto LAB_026d8d12;
          if (csvid < 0x3a65)
          {
            if ((csvid != 0x391d) && (csvid != 0x3a30))
              goto LAB_026d75bd;
            goto LAB_026d8d12;
          }
          if (csvid == 0x3b1a)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d8e7d:
          if (csvid != 0x3a71)
          {
            if (csvid < 0x3a72)
            {
              if ((csvid == 0x3a11) || (csvid == 0x3a57))
                goto LAB_026d814b;
            }
            else if (csvid == 0x3afc)
              goto LAB_026d814b;
            goto LAB_026d7652;
          }
          goto LAB_026d814b;
        }
        if (((csvid == 0x3df5) || (csvid == 0x3e1d)) || (csvid == 0x3dc5))
          goto LAB_026d7897;
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026d9442:
        if (0x3e8e < csvid)
        {
          uVar3 = 1U << (cVar1 + 0x71U & 0x1f) & 0x85;
          goto joined_r0x026da8e8;
        }
        if (csvid < 0x3d4d)
        {
          if (csvid < 0x3d2e)
          {
            if ((csvid == 0x3cbb) || (csvid == 0x3ce7))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
            goto LAB_026d8674;
          }
          if ((1U << (cVar1 - 0x2eU & 0x1f) & 0x40000005U) != 0)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d8685:
          if (csvid != 0x3d48)
          {
            if (csvid < 0x3d49)
            {
              if (csvid != 0x3cea)
                goto LAB_026d7652;
            }
            else
            {
              uVar3 = csvid - 0x3d4f;
            joined_r0x026d9ec8:
              if (1 < uVar3)
                goto LAB_026d7652;
            }
          }
          goto LAB_026d814b;
        }
        if (csvid == 0x3ddc)
          goto LAB_026d8d12;
        if (csvid < 0x3ddd)
        {
          if ((csvid == 0x3db8) || (csvid == 0x3dd4))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          goto LAB_026d8652;
        }
        if (csvid == 0x3df4)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      joined_r0x026d94ae:
        if (csvid != 0x3e42)
          goto LAB_026d7652;
      }
      else if (csvid < 0x3f19)
      {
        if (0x3efe < csvid)
        {
          if ((1U << (cVar1 + 1U & 0x1f) & 0x2040001U) != 0)
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
        LAB_026d9647:
          if (csvid != 0x3f01)
            goto LAB_026d75bd;
          goto LAB_026d8d12;
        }
        if (((csvid == 0x3ee0) || (csvid == 0x3eef)) || (csvid == 0x3eae))
          goto LAB_026d7897;
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026d9636:
        if (csvid == 0x3efb)
          goto LAB_026d8d12;
        if (0x3efb < csvid)
          goto LAB_026d9647;
        if ((csvid == 0x3ec8) || (csvid == 0x3ef4))
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026d9df5:
        if (csvid < 0x3eaf)
        {
          if ((csvid < 0x3ead) && (csvid != 0x3d7a))
            goto joined_r0x026d94ae;
        }
        else if (csvid != 0x3f54)
          goto LAB_026d7652;
      }
      else
      {
        if (csvid == 0x3f64)
          goto LAB_026d7897;
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026d9544:
        if (csvid == 0x3f1f)
          goto LAB_026d8d12;
        if (csvid < 0x3f20)
          goto LAB_026d9636;
        if (((csvid == 0x3faf) || (csvid == 0x3ffb)) || (csvid == 0x3fac))
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026d9580:
        if (csvid != 0x3f72)
        {
          if (csvid < 0x3f73)
            goto LAB_026d9df5;
          if (((csvid != 0x3f98) && (csvid != 0x3fb7)) && (csvid != 0x3f8d))
            goto LAB_026d7652;
        }
      }
      goto LAB_026d814b;
    }
    if (csvid == 0x4f4a)
      goto LAB_026d7897;
    if (csvid < 0x4f4b)
    {
      if (csvid == 0x47dc)
        goto LAB_026d7897;
      if (0x47dc < csvid)
      {
        if (csvid == 0x4bbf)
          goto LAB_026d7897;
        if (csvid < 0x4bc0)
        {
          if (csvid == 0x49d3)
            goto LAB_026d7897;
          if (0x49d3 < csvid)
          {
            if (((csvid == 0x4ac8) || (csvid == 0x4b60)) || (csvid == 0x4ac5))
              goto LAB_026d7897;
            if ((sp_key & 4U) == 0)
              goto LAB_026d75bd;
            goto LAB_026d9339;
          }
          if (csvid == 0x483e)
            goto LAB_026d7897;
          if (0x483e < csvid)
          {
            if (csvid == 0x49d1)
              goto LAB_026d7897;
            if ((sp_key & 4U) == 0)
              goto LAB_026d75bd;
            goto LAB_026daea9;
          }
          if ((csvid == 0x4818) || (csvid == 0x4821))
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
        LAB_026d856f:
          if (csvid == 0x480c)
            goto LAB_026d8d12;
          if (csvid < 0x480d)
          {
            if ((csvid == 0x47cd) || (csvid == 0x47f2))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
            goto LAB_026d837a;
          }
          if (csvid == 0x4835)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d8598:
          if ((csvid == 0x4872) || (csvid == 0x488c))
            goto LAB_026d814b;
        }
        else
        {
          if (csvid == 0x4d66)
            goto LAB_026d7897;
          if (csvid < 0x4d67)
          {
            if ((csvid - 0x4d3aU < 0x13) &&
                ((1U << ((unsigned char)(csvid - 0x4d3aU) & 0x1f) & 0x40005U) != 0))
              goto LAB_026d7897;
            if ((sp_key & 4U) == 0)
              goto LAB_026d75bd;
          LAB_026d9e4f:
            if (csvid == 0x4c33)
              goto LAB_026d8d12;
            if (0x4c33 < csvid)
            {
              if (csvid == 0x4ca7)
                goto LAB_026d8d12;
              if (0x4ca7 < csvid)
              {
                if (csvid == 0x4d41)
                  goto LAB_026d8d12;
                if ((sp_key & 8U) == 0)
                  goto LAB_026d7927;
                goto LAB_026d9e81;
              }
              if ((csvid == 0x4c75) || (csvid == 0x4c9e))
                goto LAB_026d8d12;
              if ((sp_key & 8U) == 0)
                goto LAB_026d7927;
            LAB_026d9e98:
              if (csvid < 0x4c5b)
              {
                if (0x4c3e < csvid)
                {
                  uVar3 = 1U << (cVar1 - 0x3fU & 0x1f) & 0x8000201;
                  goto joined_r0x026da0bb;
                }
                if (csvid != 0x4c15)
                {
                  if (0x4c15 < csvid)
                  {
                    uVar3 = csvid - 0x4c29;
                    goto joined_r0x026d9ec8;
                  }
                  if (csvid != 0x4bfb)
                    goto LAB_026d7652;
                }
              }
              else if ((csvid != 0x4c9e) && (csvid != 0x4ccb))
                goto LAB_026d7652;
              goto LAB_026d814b;
            }
            if (csvid == 0x4b58)
              goto LAB_026d8d12;
            if (0x4b58 < csvid)
            {
              if ((csvid == 0x4bb6) || (csvid == 0x4c2c))
                goto LAB_026d8d12;
              if ((sp_key & 8U) == 0)
                goto LAB_026d7927;
              goto LAB_026d75d7;
            }
            if ((csvid == 0x4ab2) || (csvid == 0x4b50))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
          LAB_026d8398:
            if (csvid < 0x4abe)
            {
              if (0x4aa1 < csvid)
              {
                uVar3 = 1U << (cVar1 + 0x5eU & 0x1f) & 0x8020001;
                goto joined_r0x026da0bb;
              }
              if ((csvid == 0x4a3b) || (csvid == 0x4a62))
                goto LAB_026d814b;
            }
            else if ((csvid & 0xffffffefU) == 0x4b64)
              goto LAB_026d814b;
          }
          else
          {
            if (csvid == 0x4de3)
              goto LAB_026d7897;
            if (csvid < 0x4de4)
            {
              if (csvid - 0x4d7dU < 2)
                goto LAB_026d7897;
              if ((sp_key & 4U) == 0)
                goto LAB_026d75bd;
            LAB_026dafb8:
              if ((csvid == 0x4d94) || (csvid == 0x4db3))
                goto LAB_026d8d12;
              if ((sp_key & 8U) == 0)
                goto LAB_026d7927;
            LAB_026dafd7:
              if (csvid < 0x4e04)
              {
                if (0x4de6 < csvid)
                {
                  uVar3 = 1U << (cVar1 + 0x19U & 0x1f) & 0x10000401;
                  goto joined_r0x026da0bb;
                }
                if (((csvid == 0x4d37) || (csvid == 0x4dc1)) || (csvid == 0x4d28))
                  goto LAB_026d814b;
                goto LAB_026d7652;
              }
            }
            else
            {
              if (csvid == 0x4e2e)
                goto LAB_026d7897;
              if ((sp_key & 4U) == 0)
                goto LAB_026d75bd;
            LAB_026d935b:
              if (csvid == 0x4e9a)
                goto LAB_026d8d12;
              if (0x4e9a < csvid)
              {
                if (csvid == 0x4f1a)
                  goto LAB_026d8d12;
                if (csvid < 0x4f1b)
                {
                  if ((csvid == 0x4eb2) || (csvid == 0x4ec7))
                    goto LAB_026d8d12;
                  if ((sp_key & 8U) == 0)
                    goto LAB_026d7927;
                  goto LAB_026d75fe;
                }
                if (csvid == 0x4f1d)
                  goto LAB_026d8d12;
                if ((sp_key & 8U) == 0)
                  goto LAB_026d7927;
                goto LAB_026d9391;
              }
              if (csvid == 0x4e16)
                goto LAB_026d8d12;
              if (csvid < 0x4e17)
                goto LAB_026dafb8;
              if (csvid == 0x4e33)
                goto LAB_026d8d12;
              if ((sp_key & 8U) == 0)
                goto LAB_026d7927;
            }
            if (csvid == 0x4e2e)
              goto LAB_026d814b;
          }
        }
        goto LAB_026d7652;
      }
      if (csvid == 0x43a0)
        goto LAB_026d7897;
      if (0x43a0 < csvid)
      {
        if (csvid == 0x4712)
          goto LAB_026d7897;
        if (csvid < 0x4713)
        {
          if (((csvid == 0x444f) || (csvid == 0x44ae)) || (csvid == 0x43c9))
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
          goto LAB_026d8cfa;
        }
        if (((csvid == 0x47ab) || (csvid == 0x47cc)) || (csvid == 0x4767))
          goto LAB_026d7897;
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026dae9c:
        if (csvid == 0x47a4)
          goto LAB_026d8d12;
        if (0x47a4 < csvid)
        {
        LAB_026daea9:
          if (csvid == 0x487c)
            goto LAB_026d8d12;
          if (csvid < 0x487d)
            goto LAB_026d856f;
          if (csvid == 0x49fd)
            goto LAB_026d8d12;
          if (0x49fd < csvid)
          {
            if (csvid == 0x4a8d)
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
            goto LAB_026d8387;
          }
          if ((csvid - 0x48adU & 0xfffffff7) == 0)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d84f8:
          if (csvid != 0x49ec)
          {
            if (0x49ec < csvid)
            {
              if (csvid - 0x49f7U < 0x1e)
              {
                uVar3 = 1U << ((unsigned char)(csvid - 0x49f7U) & 0x1f) & 0x20002001;
                goto joined_r0x026da0bb;
              }
              goto LAB_026d7652;
            }
            if (csvid != 0x48a6)
            {
              if (csvid < 0x48a7)
                goto LAB_026d8598;
              if (csvid != 0x49c1)
                goto LAB_026d7652;
            }
          }
          goto LAB_026d814b;
        }
        if (csvid == 0x449c)
          goto LAB_026d8d12;
        if (csvid < 0x449d)
        {
          if (0x43d8 < csvid)
          {
            if ((csvid == 0x440c) || (csvid == 0x443c))
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
            goto LAB_026d83f0;
          }
          if (0x43cf < csvid)
          {
            uVar3 = 1U << (cVar1 + 0x30U & 0x1f) & 0x111;
            goto joined_r0x026da8e8;
          }
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d85d8:
          if (csvid != 0x43c7)
          {
            if (csvid < 0x43c8)
            {
              if (csvid != 0x4331)
              {
                if (csvid < 0x4332)
                {
                  if ((csvid == 0x42c3) || (csvid == 0x42e1))
                    goto LAB_026d814b;
                }
                else if (csvid == 0x434b)
                  goto LAB_026d814b;
                goto LAB_026d7652;
              }
            }
            else if (((csvid != 0x440c) && (csvid != 0x441a)) && (csvid != 0x43f9))
              goto LAB_026d7652;
          }
        }
        else
        {
          if (csvid == 0x4719)
            goto LAB_026d8d12;
          if (0x4719 < csvid)
          {
            if (csvid == 0x477b)
              goto LAB_026d8d12;
            if ((sp_key & 8U) == 0)
              goto LAB_026d7927;
            goto LAB_026d8401;
          }
          if ((csvid == 0x4675) || (csvid == 0x468b))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
        LAB_026d84a0:
          if (csvid != 0x466c)
          {
            if (csvid < 0x466d)
            {
              if ((csvid == 0x4492) || (csvid == 0x44ae))
                goto LAB_026d814b;
            }
            else if (csvid == 0x4763)
              goto LAB_026d814b;
            goto LAB_026d7652;
          }
        }
        goto LAB_026d814b;
      }
      if (csvid == 0x41cb)
        goto LAB_026d7897;
      if (csvid < 0x41cc)
      {
        if (csvid == 0x40fd)
          goto LAB_026d7897;
        if (csvid < 0x40fe)
        {
          if ((csvid == 0x3fc7) || (csvid == 0x40cc))
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
          goto LAB_026d907f;
        }
        if (csvid == 0x4145)
          goto LAB_026d7897;
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026d9090:
        if (csvid == 0x416a)
          goto LAB_026d8d12;
        if (0x416a < csvid)
          goto LAB_026dbaa7;
        if (csvid == 0x4091)
          goto LAB_026d8d12;
        if (csvid < 0x4092)
        {
          if (csvid != 0x4065)
          {
            if (csvid < 0x4066)
            {
              if ((csvid == 0x402d) || (csvid == 0x4055))
                goto LAB_026d8d12;
            }
            else if (csvid == 0x408e)
              goto LAB_026d8d12;
            goto LAB_026d75bd;
          }
          goto LAB_026d8d12;
        }
        if (csvid == 0x40f2)
          goto LAB_026d8d12;
        if (csvid < 0x40f3)
        {
          if ((csvid == 0x40d1) || (csvid == 0x40e4))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          goto LAB_026dc5bf;
        }
        if (csvid == 0x4139)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026d90d7:
        if (csvid != 0x414c)
        {
          if (0x414c < csvid)
            goto LAB_026d90e8;
          if ((csvid != 0x40f2) && (csvid != 0x4109))
            goto LAB_026d7652;
        }
      }
      else
      {
        if (((csvid == 0x4342) || (csvid == 0x4385)) || (csvid == 0x4272))
          goto LAB_026d7897;
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026dbaa7:
        if (csvid == 0x4227)
          goto LAB_026d8d12;
        if (0x4227 < csvid)
        {
          if (csvid == 0x438f)
            goto LAB_026d8d12;
          if (0x438f < csvid)
          {
            if (csvid != 0x4398)
              goto LAB_026d75bd;
            goto LAB_026d8d12;
          }
          if ((csvid == 0x4274) || (csvid == 0x42e8))
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          goto LAB_026d836f;
        }
        if (csvid == 0x41ec)
          goto LAB_026d8d12;
        if (0x41ec < csvid)
        {
          if (csvid != 0x421b)
            goto LAB_026d75bd;
          goto LAB_026d8d12;
        }
        if ((csvid - 0x41bfU & 0xfffffffd) == 0)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026d90e8:
        if ((csvid != 0x41ca) && (csvid != 0x4274))
          goto LAB_026d7652;
      }
      goto LAB_026d814b;
    }
    if (csvid < 0x523e)
    {
      if (0x5220 < csvid)
      {
        if ((1U << (cVar1 - 0x21U & 0x1f) & 0x10000009U) != 0)
        {
          uVar2 = uVar2 | 2;
        }
        goto LAB_026d789a;
      }
      if (csvid < 0x5030)
      {
        if (0x5013 < csvid)
        {
          if ((1U << (cVar1 - 0x14U & 0x1f) & 0x8000101U) != 0)
          {
            uVar2 = uVar2 | 2;
          }
          goto LAB_026d789a;
        }
        if (0x4fcb < csvid)
        {
          if ((csvid == 0x4fdd) || (csvid == 0x4ffa))
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
        LAB_026d997f:
          if (csvid != 0x500f)
          {
            if (csvid < 0x5010)
              goto LAB_026d99c2;
            if (csvid != 0x5013)
              goto LAB_026d75bd;
          }
          goto LAB_026d8d12;
        }
        if (csvid < 0x4fb5)
        {
          if ((sp_key & 4U) != 0)
            goto LAB_026d9323;
          goto LAB_026d75bd;
        }
        if ((1U << (cVar1 + 0x4bU & 0x1f) & 0x440001U) != 0)
          goto LAB_026d7897;
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026d99c2:
        if ((csvid == 0x4fc6) || (csvid == 0x4fdf))
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026d760f:
        if (csvid != 0x4fe8)
        {
          if (csvid < 0x4fe9)
            goto LAB_026d96e1;
        LAB_026d7620:
          if (csvid != 0x50ab)
          {
            if (csvid < 0x50ac)
              goto LAB_026d9934;
          LAB_026d7631:
            if (((csvid != 0x50e2) && (csvid != 0x50e7)) && (csvid != 0x50b0))
              goto LAB_026d7652;
          }
        }
      }
      else
      {
        if (csvid == 0x51d6)
          goto LAB_026d7897;
        if (0x51d6 < csvid)
        {
          if ((csvid == 0x51dc) || (csvid == 0x51ff))
            goto LAB_026d7897;
          if ((sp_key & 4U) == 0)
            goto LAB_026d75bd;
        LAB_026db0a8:
          if (csvid == 0x521d)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          goto LAB_026d75e8;
        }
        if (((csvid == 0x50c6) || (csvid == 0x510e)) || (csvid == 0x5097))
          goto LAB_026d7897;
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026d8212:
        if (csvid == 0x508c)
          goto LAB_026d8d12;
        if (0x508c < csvid)
          goto LAB_026d8223;
        if (csvid == 0x503a)
          goto LAB_026d8d12;
        if (csvid < 0x503b)
          goto LAB_026d997f;
        if (csvid < 0x505b)
        {
          if (0x5045 < csvid)
          {
            uVar3 = 1U << (cVar1 + 0xbaU & 0x1f) & 0x100801;
            goto joined_r0x026da8e8;
          }
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          if (csvid == 0x5045)
            goto LAB_026d814b;
          goto LAB_026d7652;
        }
        if (csvid == 0x506e)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026d9934:
        if (((csvid != 0x5066) && (csvid != 0x506b)) && (csvid != 0x5045))
          goto LAB_026d7652;
      }
      goto LAB_026d814b;
    }
    if (csvid == 0x5464)
      goto LAB_026d7897;
    if (csvid < 0x5465)
    {
      if (0x5426 < csvid)
      {
      LAB_026d972e:
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026d9737:
        if (csvid == 0x53a4)
          goto LAB_026d8d12;
        if (csvid < 0x53a5)
          goto LAB_026d8212;
        goto LAB_026d9748;
      }
      if (0x540f < csvid)
      {
        if ((1U << (cVar1 - 0x10U & 0x1f) & 0x420c01U) != 0)
          goto LAB_026d7897;
        if ((sp_key & 4U) == 0)
          goto LAB_026d75bd;
      LAB_026d817a:
        if ((csvid & 0xfffffff7U) == 0x5427)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
        goto LAB_026d8194;
      }
      if (csvid == 0x536e)
        goto LAB_026d7897;
      if (0x536e < csvid)
      {
        if (csvid != 0x53a1)
          goto LAB_026d972e;
        goto LAB_026d7897;
      }
      if ((csvid == 0x5306) || (csvid == 0x5359))
        goto LAB_026d7897;
      if ((sp_key & 4U) == 0)
        goto LAB_026d75bd;
    LAB_026d8223:
      if (csvid == 0x526e)
        goto LAB_026d8d12;
      if (csvid < 0x526f)
      {
        if (0x50f2 < csvid)
          goto LAB_026db0a8;
        if (csvid < 0x50e2)
        {
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          goto LAB_026d7620;
        }
        if ((1U << (cVar1 + 0x1eU & 0x1f) & 0x10011U) != 0)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
        goto LAB_026d7631;
      }
      if (csvid == 0x52da)
        goto LAB_026d8d12;
      if (0x52da < csvid)
      {
        if (csvid == 0x5352)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
        goto LAB_026d8255;
      }
      if ((csvid == 0x5297) || (csvid == 0x52ae))
        goto LAB_026d8d12;
      if ((sp_key & 8U) == 0)
        goto LAB_026d7927;
    LAB_026d82c8:
      if (csvid != 0x529d)
      {
        if (csvid < 0x529e)
        {
          if ((csvid == 0x5220) || (csvid == 0x523d))
            goto LAB_026d814b;
        }
        else if (csvid == 0x52cc)
        {
          uVar2 = uVar2 | 8;
          goto LAB_026d7927;
        }
        goto LAB_026d7652;
      }
    }
    else if (csvid < 0x55b2)
    {
      if (0x55a4 < csvid)
      {
        if ((1U << (cVar1 + 0x5bU & 0x1f) & 0x1041U) != 0)
        {
          uVar2 = uVar2 | 2;
        }
        goto LAB_026d789a;
      }
      if (((csvid == 0x54a8) || (csvid == 0x5554)) || (csvid == 0x5479))
        goto LAB_026d7897;
      if ((sp_key & 4U) == 0)
        goto LAB_026d75bd;
    LAB_026d9748:
      if (csvid == 0x5522)
        goto LAB_026d8d12;
      if (0x5522 < csvid)
        goto LAB_026d9759;
      if (csvid < 0x53fa)
      {
        if (csvid < 0x53e3)
        {
          if ((csvid != 0x53c4) && (csvid != 0x53d9))
            goto LAB_026d75bd;
          goto LAB_026d8d12;
        }
        if ((1U << (cVar1 + 0x1dU & 0x1f) & 0x400401U) != 0)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026d833c:
        if (csvid != 0x53ed)
          goto LAB_026d7652;
        goto LAB_026d814b;
      }
      if (csvid == 0x54af)
        goto LAB_026d8d12;
      if (csvid < 0x54b0)
        goto LAB_026d817a;
      if (csvid == 0x54db)
        goto LAB_026d8d12;
      if ((sp_key & 8U) == 0)
        goto LAB_026d7927;
    LAB_026d98a0:
      if (csvid != 0x54c4)
      {
        if (csvid < 0x54c5)
        {
          if ((csvid == 0x5487) || (csvid == 0x54a8))
            goto LAB_026d814b;
        }
        else if (csvid == 0x54ca)
          goto LAB_026d814b;
        goto LAB_026d7652;
      }
    }
    else
    {
      if (csvid - 0x568fU < 2)
        goto LAB_026d7897;
      if ((sp_key & 4U) == 0)
        goto LAB_026d75bd;
    LAB_026d9759:
      if (csvid == 0x5623)
        goto LAB_026d8d12;
      if (csvid < 0x5624)
      {
        if (csvid == 0x55bf)
          goto LAB_026d8d12;
        if (0x55bf < csvid)
        {
          if (csvid - 0x55dbU < 2)
            goto LAB_026d8d12;
          if ((sp_key & 8U) == 0)
            goto LAB_026d7927;
          goto LAB_026d97bd;
        }
        if ((csvid == 0x5538) || (csvid == 0x5572))
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      }
      else
      {
        if (((csvid == 0x5648) || (csvid == 0x566a)) || (csvid == 0x5627))
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026d81a1:
        if (0x562f < csvid)
        {
          if (csvid != 0x56c0)
          {
            if (csvid < 0x56c1)
            {
              if (csvid == 0x5650)
                goto LAB_026d814b;
              if (csvid == 0x5684)
              {
                uVar2 = uVar2 | 8;
                goto LAB_026d7927;
              }
            }
            else if (csvid == 0x56d7)
              goto LAB_026d814b;
            goto LAB_026d7652;
          }
          goto LAB_026d814b;
        }
      LAB_026d97bd:
        if (0x5610 < csvid)
        {
          uVar3 = 1U << (cVar1 - 0x11U & 0x1f) & 0x48010001;
          goto joined_r0x026da0bb;
        }
      }
      if (csvid != 0x5543)
      {
        if (csvid < 0x5544)
          goto LAB_026d98a0;
        if (((csvid != 0x559d) && (csvid != 0x55b8)) && (csvid != 0x5549))
          goto LAB_026d7652;
      }
    }
    goto LAB_026d814b;
  }
  if (0x2398 < csvid)
  {
    if (csvid != 0x2564)
    {
      if (csvid < 0x2565)
      {
        if (csvid != 0x24bf)
        {
          if (csvid < 0x24c0)
          {
            if (csvid != 0x242a)
            {
              uVar2 = (unsigned int)(csvid == 0x2437);
            }
          }
          else
          {
            uVar2 = (unsigned int)(csvid == 0x2520);
          }
        }
      }
      else if ((csvid != 0x2835) && (csvid != 0x2840))
      {
        uVar2 = (unsigned int)(csvid == 0x265b);
      }
    }
    goto LAB_026d7540;
  }
  if (csvid == 0x2066)
    goto LAB_026d7540;
  if (csvid < 0x2067)
  {
    if (csvid != 0x1f3b)
    {
      if (csvid < 0x1f3c)
      {
        if (csvid != 0x1d1e)
        {
          uVar2 = (unsigned int)(csvid == 0x1e89);
        }
      }
      else
      {
        uVar2 = (unsigned int)(csvid == 0x2051);
      }
    }
    goto LAB_026d7540;
  }
  if (csvid == 0x2245)
    goto LAB_026d7540;
  if (csvid < 0x2246)
  {
    if (csvid != 0x20e5)
    {
      uVar2 = (unsigned int)(csvid == 0x213e);
    }
    goto LAB_026d7540;
  }
  if (csvid == 0x2317)
    goto LAB_026d7540;
  uVar2 = 0;
  if ((sp_key & 2U) == 0)
    goto LAB_026d789a;
  uVar2 = 0;
LAB_026d9aba:
  if (csvid == 0x2389)
    goto LAB_026d7897;
  if (csvid < 0x238a)
  {
    if (csvid == 0x2310)
      goto LAB_026d7897;
    if (csvid < 0x2311)
      goto LAB_026d9c11;
    if (csvid == 0x237f)
      goto LAB_026d7897;
    if ((sp_key & 4U) == 0)
      goto LAB_026d75bd;
    goto LAB_026dc28c;
  }
  if (((csvid == 0x266a) || (csvid == 0x26ac)) || (csvid == 0x2624))
    goto LAB_026d7897;
  if ((sp_key & 4U) == 0)
    goto LAB_026d75bd;
LAB_026d9af8:
  if (csvid == 0x264b)
    goto LAB_026d8d12;
  if (0x264b < csvid)
  {
    if (csvid == 0x272f)
    {
    LAB_026d8d12:
      uVar2 = uVar2 | 4;
    LAB_026d75bd:
      if ((sp_key & 8U) == 0)
        goto LAB_026d7927;
      if (csvid != 0x3906)
      {
        if (csvid < 0x3907)
        {
        LAB_026d884e:
          if (csvid != 0x25eb)
          {
            if (csvid < 0x25ec)
            {
            LAB_026d8d37:
              if (csvid != 0x13e4)
              {
                if (csvid < 0x13e5)
                {
                LAB_026d8d48:
                  if (csvid != 0x89f)
                  {
                    if (0x89f < csvid)
                      goto LAB_026d8d59;
                    if (csvid != 0x4ae)
                      goto LAB_026d910e;
                  }
                }
                else
                {
                LAB_026d93dd:
                  if (csvid != 0x1ec1)
                  {
                    if (csvid < 0x1ec2)
                      goto LAB_026d93ee;
                    goto LAB_026d9677;
                  }
                }
              }
            }
            else
            {
            LAB_026d885f:
              if (csvid != 0x3120)
              {
                if (csvid < 0x3121)
                {
                LAB_026dc137:
                  if (csvid != 0x2cf5)
                  {
                    if (0x2cf5 < csvid)
                      goto LAB_026dbda0;
                    goto LAB_026dc148;
                  }
                }
                else
                {
                LAB_026d9278:
                  if (csvid != 0x34ea)
                  {
                    if (csvid < 0x34eb)
                    {
                    LAB_026da150:
                      if (csvid != 0x331f)
                      {
                        if (csvid < 0x3320)
                        {
                        LAB_026da220:
                          if (csvid != 0x3252)
                          {
                            if (csvid < 0x3253)
                            {
                            LAB_026da1d8:
                              if (csvid != 0x31cc)
                              {
                                if (csvid < 0x31cd)
                                {
                                  if (csvid != 0x3147)
                                    goto LAB_026da9a3;
                                }
                                else if (csvid != 0x3203)
                                  goto LAB_026d7652;
                              }
                            }
                            else if (((csvid != 0x32bd) && (csvid != 0x32c4)) && (csvid != 0x3256))
                              goto LAB_026d7652;
                          }
                        }
                        else if (csvid != 0x33de)
                        {
                          if (csvid < 0x33df)
                            goto LAB_026da108;
                          if (((csvid != 0x3497) && (csvid != 0x34bd)) && (csvid != 0x343c))
                            goto LAB_026d7652;
                        }
                      }
                    }
                    else
                    {
                    LAB_026d9289:
                      if (csvid != 0x36f7)
                      {
                        if (csvid < 0x36f8)
                        {
                          if (csvid != 0x3603)
                          {
                            if (csvid < 0x3604)
                              goto LAB_026d9028;
                            if (((csvid != 0x367e) && (csvid != 0x3684)) && (csvid != 0x3637))
                              goto LAB_026d7652;
                          }
                        }
                        else if (csvid != 0x37e9)
                          goto LAB_026d92a5;
                      }
                    }
                  }
                }
              }
            }
          }
        }
        else
        {
        LAB_026d75d7:
          if (csvid != 0x4bcb)
          {
            if (csvid < 0x4bcc)
            {
            LAB_026d836f:
              if (csvid < 0x42bb)
              {
                if (csvid < 0x42a1)
                {
                LAB_026d8652:
                  if (csvid != 0x3d65)
                  {
                    if (0x3d65 < csvid)
                    {
                    LAB_026dc5bf:
                      if (0x40cc < csvid)
                        goto LAB_026d90d7;
                      if (csvid < 0x40af)
                        goto LAB_026d9580;
                      uVar3 = 1U << (cVar1 + 0x51U & 0x1f) & 0x22000001;
                      goto joined_r0x026da0bb;
                    }
                  LAB_026d8663:
                    if (csvid != 0x3be4)
                    {
                      if (csvid < 0x3be5)
                        goto LAB_026d8e67;
                    LAB_026d8674:
                      if (csvid != 0x3cbf)
                      {
                        if (0x3cbf < csvid)
                          goto LAB_026d8685;
                        if (csvid != 0x3c9f)
                        {
                          if (0x3c9f < csvid)
                          {
                            uVar3 = csvid - 0x3ca1;
                            goto joined_r0x026d9ec8;
                          }
                          if (csvid != 0x3c82)
                            goto LAB_026d7652;
                        }
                      }
                    }
                  }
                }
                else
                {
                  uVar3 = 1U << (cVar1 + 0x5fU & 0x1f) & 0x2008001;
                joined_r0x026da0bb:
                  if (uVar3 == 0)
                    goto LAB_026d7652;
                }
              }
              else
              {
              LAB_026d837a:
                if (csvid != 0x47cc)
                {
                  if (csvid < 0x47cd)
                  {
                  LAB_026d83f0:
                    if (csvid != 0x4489)
                    {
                      if (csvid < 0x448a)
                        goto LAB_026d85d8;
                    LAB_026d8401:
                      if (csvid != 0x4780)
                      {
                        if (csvid < 0x4781)
                          goto LAB_026d84a0;
                        if (csvid != 0x47ac)
                        {
                          if (csvid < 0x47ad)
                          {
                            uVar3 = csvid - 0x479e;
                            goto joined_r0x026d9ec8;
                          }
                          if (csvid != 0x47ae)
                            goto LAB_026d7652;
                        }
                      }
                    }
                  }
                  else
                  {
                  LAB_026d8387:
                    if (csvid != 0x4a21)
                    {
                      if (0x4a21 < csvid)
                        goto LAB_026d8398;
                      goto LAB_026d84f8;
                    }
                  }
                }
              }
            }
            else
            {
            LAB_026d75e8:
              if (csvid < 0x5120)
              {
                if (0x5104 < csvid)
                {
                  uVar3 = 1U << (cVar1 - 5U & 0x1f) & 0x4000045;
                  goto joined_r0x026da0bb;
                }
              LAB_026d75fe:
                if (csvid != 0x4ee7)
                {
                  if (0x4ee7 < csvid)
                    goto LAB_026d760f;
                LAB_026d9e81:
                  if (csvid != 0x4cd9)
                  {
                    if (csvid < 0x4cda)
                      goto LAB_026d9e98;
                    goto LAB_026dafd7;
                  }
                }
              }
              else
              {
              LAB_026d8194:
                if (csvid != 0x5450)
                {
                  if (0x5450 < csvid)
                    goto LAB_026d81a1;
                LAB_026d8255:
                  if (csvid < 0x5363)
                  {
                    if (0x5348 < csvid)
                    {
                      uVar3 = 1U << (cVar1 + 0xb7U & 0x1f) & 0x20c0001;
                      goto joined_r0x026da0bb;
                    }
                    if (csvid != 0x52e5)
                    {
                      if (csvid < 0x52e6)
                        goto LAB_026d82c8;
                      if ((csvid != 0x5329) && (csvid != 0x5342))
                      {
                        if (csvid == 0x531d)
                        {
                          uVar2 = uVar2 | 8;
                          goto LAB_026d7927;
                        }
                        goto LAB_026d7652;
                      }
                    }
                  }
                  else
                  {
                    if (csvid < 0x5424)
                    {
                      if (csvid < 0x5412)
                        goto LAB_026d833c;
                      uVar3 = 1U << (cVar1 - 0x12U & 0x1f) & 0x20021;
                      goto joined_r0x026da0bb;
                    }
                    if (csvid != 0x5440)
                      goto LAB_026d7652;
                  }
                }
              }
            }
          }
        }
      }
    LAB_026d814b:
      uVar2 = uVar2 | 8;
    LAB_026d7927:
      if ((sp_key & 0x100U) != 0)
      {
        if (csvid != 0x214f)
        {
          if (0x214f < csvid)
            goto LAB_026d793d;
        LAB_026da5b9:
          if (csvid != 0xd63)
          {
            if (csvid < 0xd64)
            {
              if (csvid < 0xc32)
              {
                if (csvid < 0xc2c)
                  goto joined_r0x026da636;
              }
              else if ((0x1c < csvid - 0xcd5U) ||
                       ((1U << ((unsigned char)(csvid - 0xcd5U) & 0x1f) & 0x1d0000fdU) == 0))
                goto joined_r0x026da562;
            }
            else if (csvid != 0xdea)
            {
              if (0xdea < csvid)
                goto LAB_026da554;
              if (csvid != 0xd72)
              {
                if (csvid - 0xd95U < 3)
                {
                  uVar2 = (uVar2 | 0x100U);
                  goto LAB_026d7652;
                }
                goto joined_r0x026da562;
              }
            }
          }
        }
        goto LAB_026d7962;
      }
    }
    else
    {
      if (csvid < 0x2730)
      {
        if (csvid != 0x26c5)
        {
          if (csvid < 0x26c6)
          {
            if ((csvid == 0x264f) || (csvid == 0x26a3))
              goto LAB_026d8d12;
          }
          else if (csvid == 0x2701)
            goto LAB_026d8d12;
          goto LAB_026d75bd;
        }
        goto LAB_026d8d12;
      }
      if (csvid < 0x2807)
      {
        if (((0x2804 < csvid) || (csvid == 0x27ac)) || (csvid == 0x27cd))
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      }
      else
      {
        if (csvid == 0x280a)
          goto LAB_026d8d12;
        if ((sp_key & 8U) == 0)
          goto LAB_026d7927;
      LAB_026dc164:
        if (0x280a < csvid)
        {
        LAB_026dc16f:
          if (((csvid != 0x289b) && (csvid != 0x28ba)) && (csvid != 0x2871))
            goto LAB_026d7652;
          goto LAB_026d814b;
        }
      }
      if (csvid == 0x27c1)
        goto LAB_026d814b;
      if (csvid < 0x27c2)
      {
        if ((csvid == 0x2760) || (csvid == 0x27bd))
          goto LAB_026d814b;
      }
      else if (csvid == 0x27df)
        goto LAB_026d814b;
    }
  LAB_026d7652:
    if ((sp_key & 0x200U) == 0)
      goto LAB_026d7c41;
    if (csvid == 0x3ec1)
      goto LAB_026d7e06;
    if (csvid < 0x3ec2)
    {
      if (csvid < 0x3375)
      {
        if (0x336a < csvid)
        {
          uVar3 = 1U << (cVar1 + 0x95U & 0x1f) & 0x281;
          goto joined_r0x026d8beb;
        }
        if (csvid == 0x2c95)
          goto LAB_026d7e06;
        if (0x2c95 < csvid)
        {
          if (csvid == 0x2f85)
            goto LAB_026d7e06;
          if (csvid < 0x2f86)
          {
            if (csvid < 0x2e6c)
            {
              if (0x2e57 < csvid)
              {
                uVar3 = 1U << (cVar1 + 0xa8U & 0x1f) & 0x80101;
                goto joined_r0x026d8beb;
              }
              if (((csvid == 0x2de2) || (csvid == 0x2de9)) || (csvid == 0x2cb9))
                goto LAB_026d7e06;
              if ((sp_key & 0x800U) == 0)
                goto LAB_026d76e0;
              goto LAB_026d8b60;
            }
            if (csvid == 0x2f4a)
              goto LAB_026d7e06;
            if ((sp_key & 0x800U) == 0)
              goto LAB_026d76e0;
          }
          else
          {
            if (csvid == 0x3162)
              goto LAB_026d7e06;
            if (0x3162 < csvid)
            {
              if (((csvid != 0x3234) && (csvid != 0x332b)) && (csvid != 0x321b))
                goto LAB_026d7c41;
              goto LAB_026d7e06;
            }
            if (((csvid == 0x3067) || (csvid == 0x30e4)) || (csvid == 0x2fbe))
              goto LAB_026d7e06;
            if ((sp_key & 0x800U) == 0)
              goto LAB_026d76e0;
          LAB_026d8c22:
            if ((csvid == 0x2fd2) || (csvid == 0x3016))
              goto LAB_026d7bb6;
          }
          if (csvid != 0x2e88)
            goto LAB_026d7ab0;
          goto LAB_026d7bb6;
        }
        if (csvid < 0x28a3)
        {
          if (0x2888 < csvid)
          {
            uVar3 = 1U << (cVar1 + 0x77U & 0x1f) & 0x2000005;
            goto joined_r0x026d8beb;
          }
          if (csvid == 0x23fe)
            goto LAB_026d7e06;
          if (csvid < 0x23ff)
          {
            if (csvid != 0x2350)
              goto LAB_026d86ff;
            goto LAB_026d7e06;
          }
          if ((csvid == 0x270d) || (csvid == 0x27ca))
            goto LAB_026d7e06;
          if ((sp_key & 0x800U) == 0)
            goto LAB_026d76e0;
        LAB_026d8b71:
          if (csvid == 0x275a)
            goto LAB_026d7bb6;
          if (csvid < 0x275b)
            goto LAB_026d8720;
        }
        else
        {
          if (csvid == 0x2a92)
            goto LAB_026d7e06;
          if (csvid < 0x2a93)
          {
            if (((csvid & 0xfffffff7U) != 0x29c7) && (csvid != 0x2930))
              goto LAB_026d7c41;
            goto LAB_026d7e06;
          }
          if ((csvid == 0x2afb) || (csvid - 0x2b77U < 2))
            goto LAB_026d7e06;
          if ((sp_key & 0x800U) == 0)
            goto LAB_026d76e0;
        }
        if (csvid < 0x2be8)
        {
          if (((0x2be3 < csvid) || ((csvid == 0x2b3e || (csvid == 0x2b96)))) || (csvid == 0x2b3b))
            goto LAB_026d7bb6;
        }
        else if ((csvid == 0x2c9b) || (csvid == 0x2cad))
          goto LAB_026d7bb6;
        goto LAB_026d7ab0;
      }
      if (csvid == 0x3891)
        goto LAB_026d7e06;
      if (csvid < 0x3892)
      {
        if (csvid == 0x363d)
          goto LAB_026d7e06;
        if (csvid < 0x363e)
        {
          if (csvid == 0x3448)
            goto LAB_026d7e06;
          if (0x3448 < csvid)
          {
            if ((csvid != 0x34e9) && ((csvid & 0xfffffffbU) != 0x3499))
              goto LAB_026d7c41;
            goto LAB_026d7e06;
          }
          if ((csvid == 0x33fd) || ((csvid == 0x3438 || (csvid == 0x33df))))
            goto LAB_026d7e06;
          if ((sp_key & 0x800U) == 0)
            goto LAB_026d76e0;
        LAB_026d8a82:
          if (csvid != 0x33ae)
          {
            if (csvid < 0x33af)
              goto LAB_026d8c22;
            if (((csvid != 0x3476) && (csvid != 0x348e)) && (csvid != 0x3462))
              goto LAB_026d7ab0;
          }
          goto LAB_026d7bb6;
        }
        if (csvid == 0x37c9)
          goto LAB_026d7e06;
        if (0x37c9 < csvid)
        {
          if ((csvid != 0x3815) && (csvid != 0x3829))
            goto LAB_026d7c41;
          goto LAB_026d7e06;
        }
        if ((csvid == 0x367c) || ((csvid == 0x3760 || (csvid == 0x363f))))
          goto LAB_026d7e06;
        if ((sp_key & 0x800U) == 0)
          goto LAB_026d76e0;
      LAB_026d8a00:
        if (0x367a < csvid)
          goto LAB_026d89b0;
        if (csvid < 0x365d)
        {
          if ((csvid == 0x3615) || (csvid == 0x365b))
            goto LAB_026d7bb6;
        }
        else if ((1U << (cVar1 + 0xa3U & 0x1f) & 0x20000021U) != 0)
          goto LAB_026d7bb6;
      }
      else
      {
        if (csvid == 0x3b5a)
          goto LAB_026d7e06;
        if (csvid < 0x3b5b)
        {
          if (csvid != 0x3a52)
          {
            if (csvid < 0x3a53)
            {
              if (((csvid == 0x3a21) || (csvid == 0x3a32)) || (csvid == 0x3947))
                goto LAB_026d7e06;
            }
            else if ((csvid == 0x3b25) || ((csvid - 0x3aafU & 0xffffffdf) == 0))
              goto LAB_026d7e06;
            goto LAB_026d7c41;
          }
          goto LAB_026d7e06;
        }
        if (csvid == 0x3cc3)
          goto LAB_026d7e06;
        if (0x3cc3 < csvid)
        {
          if ((csvid == 0x3e37) || (csvid == 0x3e70))
            goto LAB_026d7e06;
          if ((sp_key & 0x800U) == 0)
            goto LAB_026d76e0;
          goto LAB_026d7c6c;
        }
        if (((csvid == 0x3c0c) || (csvid == 0x3cb6)) || (csvid == 0x3bdc))
          goto LAB_026d7e06;
        if ((sp_key & 0x800U) == 0)
          goto LAB_026d76e0;
      LAB_026d89b0:
        if (csvid == 0x3c71)
        {
        LAB_026d7bb6:
          uVar2 = uVar2 | 0x800;
          goto LAB_026d76e0;
        }
        if (csvid == 0x3db6)
        {
          uVar2 = uVar2 | 0x800;
          goto LAB_026d76e0;
        }
      }
      goto LAB_026d7ab0;
    }
    if (0x4ce6 < csvid)
    {
      if (csvid == 0x5248)
        goto LAB_026d7e06;
      if (csvid < 0x5249)
      {
        if (csvid < 0x4fde)
        {
          if (0x4fc6 < csvid)
          {
            uVar3 = 1U << (cVar1 + 0x39U & 0x1f) & 0x400011;
            goto joined_r0x026d8beb;
          }
          if (csvid == 0x4ecb)
            goto LAB_026d7e06;
          if (csvid < 0x4ecc)
          {
            if (((csvid == 0x4ddc) || (csvid == 0x4de3)) || (csvid == 0x4db7))
              goto LAB_026d7e06;
          }
          else if ((csvid == 0x4f1b) || (csvid == 0x4f67))
            goto LAB_026d7e06;
          goto LAB_026d7c41;
        }
        if (csvid == 0x50cb)
          goto LAB_026d7e06;
        if (csvid < 0x50cc)
        {
          if ((csvid != 0x501c) && (csvid != 0x507b))
            goto LAB_026d7c41;
          goto LAB_026d7e06;
        }
        if ((csvid == 0x50e2) || (csvid == 0x5141))
          goto LAB_026d7e06;
        if ((sp_key & 0x800U) == 0)
          goto LAB_026d76e0;
      LAB_026d7a78:
        if (csvid != 0x5227)
        {
          if (csvid < 0x5228)
          {
          LAB_026d7b90:
            if (csvid != 0x4857)
            {
              if (csvid < 0x4858)
              {
                if (csvid != 0x427b)
                  goto LAB_026dac43;
              }
              else if ((csvid != 0x51e4) && (csvid != 0x5211))
                goto LAB_026d7bab;
            }
          }
          else
          {
          LAB_026d7a89:
            if (csvid != 0x52a4)
            {
              if (0x52a4 < csvid)
                goto LAB_026d7a9a;
              if (((csvid != 0x5259) && (csvid != 0x5273)) && (csvid != 0x523b))
                goto LAB_026d7ab0;
            }
          }
        }
      }
      else
      {
        if (csvid == 0x5460)
          goto LAB_026d7e06;
        if (csvid < 0x5461)
        {
          if (csvid == 0x5345)
            goto LAB_026d7e06;
          if (csvid < 0x5346)
          {
            if (((csvid == 0x52bd) || (csvid == 0x52fe)) || (csvid == 0x524d))
              goto LAB_026d7e06;
            if ((sp_key & 0x800U) == 0)
              goto LAB_026d76e0;
            goto LAB_026d7a89;
          }
          if (((csvid == 0x53b9) || (csvid == 0x5413)) || (csvid == 0x536e))
            goto LAB_026d7e06;
          if ((sp_key & 0x800U) == 0)
            goto LAB_026d76e0;
        LAB_026d7a9a:
          if (csvid == 0x5429)
            goto LAB_026d7bb6;
        }
        else
        {
          if (csvid == 0x5601)
            goto LAB_026d7e06;
          if (0x5601 < csvid)
          {
            if ((csvid != 0x5627) && (csvid != 0x565d))
              goto LAB_026d7c41;
            goto LAB_026d7e06;
          }
          if ((csvid == 0x54f3) || ((csvid == 0x55a5 || (csvid == 0x5483))))
            goto LAB_026d7e06;
          if ((sp_key & 0x800U) == 0)
            goto LAB_026d76e0;
        }
        if (csvid != 0x55ea)
          goto LAB_026d7ab0;
      }
      goto LAB_026d7bb6;
    }
    if (0x4cdf < csvid)
    {
      uVar3 = 1U << (cVar1 + 0x20U & 0x1f) & 0x45;
    joined_r0x026d8beb:
      if (uVar3 == 0)
        goto LAB_026d7c41;
    LAB_026d7e06:
      uVar2 = uVar2 | 0x200;
    LAB_026d7c41:
      if ((sp_key & 0x800U) == 0)
        goto LAB_026d76e0;
      if (csvid != 0x3609)
      {
        if (csvid < 0x360a)
        {
        LAB_026d8b60:
          if (csvid != 0x2d42)
          {
            if (csvid < 0x2d43)
              goto LAB_026d8b71;
            if (csvid != 0x34b7)
            {
              if (csvid < 0x34b8)
                goto LAB_026d8a82;
              if (csvid != 0x35aa)
              {
                if (csvid < 0x35ab)
                {
                  if (((csvid == 0x3504) || (csvid == 0x357c)) || (csvid == 0x34d4))
                    goto LAB_026d7bb6;
                }
                else if ((csvid == 0x35b8) || (csvid == 0x35d4))
                  goto LAB_026d7bb6;
                goto LAB_026d7ab0;
              }
            }
          }
        }
        else
        {
        LAB_026d7c5b:
          if (csvid != 0x4256)
          {
            if (0x4256 < csvid)
              goto LAB_026d7a78;
          LAB_026d7c6c:
            if (csvid != 0x3e96)
            {
              if (csvid < 0x3e97)
                goto LAB_026d8a00;
              if (csvid != 0x41df)
              {
                if (csvid < 0x41e0)
                  goto LAB_026d7c00;
                if (csvid != 0x4227)
                {
                  if (csvid == 0x4239)
                  {
                    uVar2 = uVar2 | 0x800;
                    goto LAB_026d76e0;
                  }
                  goto LAB_026d7ab0;
                }
              }
            }
          }
        }
      }
      goto LAB_026d7bb6;
    }
    if (csvid == 0x4709)
      goto LAB_026d7e06;
    if (csvid < 0x470a)
    {
      if (csvid == 0x4145)
        goto LAB_026d7e06;
      if (0x4145 < csvid)
      {
        if (csvid == 0x42aa)
          goto LAB_026d7e06;
        if (0x42aa < csvid)
        {
          if (((csvid != 0x43f3) && (csvid != 0x46b7)) && (csvid != 0x433d))
            goto LAB_026d7c41;
          goto LAB_026d7e06;
        }
        if (((csvid == 0x41d4) || (csvid == 0x426e)) || (csvid == 0x4174))
          goto LAB_026d7e06;
        if ((sp_key & 0x800U) == 0)
          goto LAB_026d76e0;
        goto LAB_026d7c5b;
      }
      if (csvid == 0x40a3)
        goto LAB_026d7e06;
      if (csvid < 0x40a4)
      {
        if (((csvid == 0x400f) || (csvid == 0x4065)) || (csvid == 0x3ef1))
          goto LAB_026d7e06;
        if ((sp_key & 0x800U) == 0)
          goto LAB_026d76e0;
      }
      else
      {
        if (((csvid == 0x40f6) || (csvid == 0x410a)) || (csvid == 0x40cc))
          goto LAB_026d7e06;
        if ((sp_key & 0x800U) == 0)
          goto LAB_026d76e0;
      LAB_026d7c00:
        if ((csvid == 0x40f7) || (csvid == 0x41d5))
          goto LAB_026d7bb6;
      }
      if (csvid == 0x3f75)
        goto LAB_026d7bb6;
      if ((sp_key & 0x2000U) == 0)
        goto LAB_026d7ab0;
      goto LAB_026d8dc5;
    }
    if (csvid == 0x4a01)
      goto LAB_026d7e06;
    if (0x4a01 < csvid)
    {
      if (csvid < 0x4c02)
      {
        if (0x4bf1 < csvid)
        {
          uVar3 = 1U << (cVar1 + 0xeU & 0x1f) & 0x8021;
          goto joined_r0x026d8beb;
        }
        if (((csvid == 0x4aa7) || (csvid == 0x4bc1)) || (csvid == 0x4a24))
          goto LAB_026d7e06;
      }
      if ((sp_key & 0x800U) == 0)
        goto LAB_026d76e0;
    LAB_026d7bab:
      if (csvid != 0x4bc5)
        goto LAB_026d7ab0;
      goto LAB_026d7bb6;
    }
    if (csvid == 0x47ab)
      goto LAB_026d7e06;
    if (0x47ab < csvid)
    {
      if (((csvid & 0xffffffefU) == 0x48cb) || (csvid == 0x47df))
        goto LAB_026d7e06;
      if ((sp_key & 0x800U) == 0)
        goto LAB_026d76e0;
      goto LAB_026d7b90;
    }
    if (((csvid == 0x4789) || (csvid == 0x4793)) || (csvid == 0x470f))
      goto LAB_026d7e06;
    if ((sp_key & 0x800U) == 0)
      goto LAB_026d76e0;
  LAB_026dac43:
    if ((csvid == 0x479f) || (csvid == 0x426e))
      goto LAB_026d7bb6;
    if ((sp_key & 0x2000U) == 0)
      goto LAB_026d7ab0;
  LAB_026d8dd0:
    if (csvid == 0x45a1)
    {
      uVar2 = uVar2 | 0x2000;
    }
    goto LAB_026d7ab0;
  }
  if (csvid == 0x2510)
    goto LAB_026d8d12;
  if (0x2510 < csvid)
  {
    if (csvid == 0x25a8)
      goto LAB_026d8d12;
    if (0x25a8 < csvid)
    {
      if (csvid == 0x25d0)
        goto LAB_026d8d12;
      if ((sp_key & 8U) == 0)
        goto LAB_026d7927;
      goto LAB_026d884e;
    }
    if ((csvid == 0x2527) || (csvid == 0x258b))
      goto LAB_026d8d12;
    if ((sp_key & 8U) == 0)
      goto LAB_026d7927;
    if (csvid == 0x256c)
      goto LAB_026d814b;
  LAB_026d96af:
    if (csvid != 0x2477)
      goto LAB_026d7652;
    goto LAB_026d814b;
  }
  if (0x23d5 < csvid)
  {
    if (csvid == 0x2490)
      goto LAB_026d8d12;
    if ((sp_key & 8U) == 0)
      goto LAB_026d7927;
  LAB_026d9688:
    if (csvid != 0x2447)
    {
      if (csvid < 0x2448)
        goto LAB_026dc53a;
      if ((csvid != 0x24e0) && (csvid != 0x256c))
        goto LAB_026d96af;
    }
    goto LAB_026d814b;
  }
  if (csvid < 0x23c3)
  {
    if (csvid == 0x2388)
      goto LAB_026d8d12;
    if ((sp_key & 8U) == 0)
      goto LAB_026d7927;
  }
  else
  {
    if ((1U << (cVar1 + 0x3dU & 0x1f) & 0x50001U) != 0)
      goto LAB_026d8d12;
    if ((sp_key & 8U) == 0)
      goto LAB_026d7927;
  LAB_026dc53a:
    if ((csvid == 0x23d3) || (csvid == 0x240d))
      goto LAB_026d814b;
  }
  if (csvid == 0x2366)
    goto LAB_026d814b;
  if ((sp_key & 0x100U) == 0)
    goto LAB_026d7652;
LAB_026d793d:
  if (csvid == 0x2220)
  {
  LAB_026d7962:
    uVar2 = (uVar2 | 0x100U);
    goto LAB_026d7652;
  }
  if (0x2220 < csvid)
  {
    if (csvid != 0x2344)
    {
      if (csvid < 0x2345)
      {
        if ((csvid != 0x22b6) && (csvid != 0x22f5))
          goto joined_r0x026da562;
      }
      else if (csvid != 0x234a)
        goto LAB_026d7652;
    }
    goto LAB_026d7962;
  }
  if (csvid == 0x21a8)
    goto LAB_026d7962;
  if (csvid < 0x21a9)
  {
    if ((csvid == 0x215b) || (csvid == 0x2196))
      goto LAB_026d7962;
  joined_r0x026da562:
    if ((sp_key & 0x200U) != 0)
      goto LAB_026d86ff;
    goto LAB_026d7c41;
  }
  if (csvid == 0x21ec)
    goto LAB_026d7962;
joined_r0x026da636:
  if ((sp_key & 0x200U) == 0)
    goto LAB_026d7c41;
LAB_026d86ff:
  if ((csvid & 0xfffffffdU) == 0x550)
    goto LAB_026d7e06;
  if ((sp_key & 0x800U) == 0)
    goto LAB_026d76e0;
LAB_026d8720:
  if (csvid == 0x1b0a)
    goto LAB_026d7bb6;
  if (0x1b0a < csvid)
  {
    if (((csvid == 0x21ed) || (csvid == 0x2686)) || (csvid == 0x1c77))
      goto LAB_026d7bb6;
    if ((sp_key & 0x2000U) == 0)
      goto LAB_026d7ab0;
  LAB_026d875b:
    if (csvid != 0x21b4)
    {
      if (csvid < 0x21b5)
      {
        if (csvid != 0x217c)
        {
          if (csvid < 0x217d)
            goto LAB_026dad70;
          if ((csvid & 0xffffffdfU) != 0x218f)
            goto LAB_026d773d;
        }
      }
      else if (csvid != 0x22fb)
      {
        if (csvid < 0x22fc)
        {
          if ((csvid != 0x2285) && (csvid != 0x22c0))
          {
            if (csvid == 0x21db)
            {
              uVar2 = uVar2 | 0x2000;
              goto LAB_026d7ab0;
            }
            goto LAB_026d773d;
          }
        }
        else
        {
        LAB_026d8dc5:
          if (csvid != 0x40f5)
            goto LAB_026d8dd0;
        }
      }
    }
    goto LAB_026d7e2b;
  }
  if (0x1ad7 < csvid)
  {
    if (csvid - 0x1ad9U < 3)
      goto LAB_026d7bb6;
    if ((sp_key & 0x2000U) == 0)
      goto LAB_026d7ab0;
  LAB_026dad70:
    if ((csvid - 0x2126U < 0xc) &&
        ((1U << ((unsigned char)(csvid - 0x2126U) & 0x1f) & 0x805U) != 0))
    {
      uVar2 = uVar2 | 0x2000;
      goto LAB_026d7ab0;
    }
    goto LAB_026d773d;
  }
  if ((0x1ad4 < csvid) || (csvid == 0x619))
    goto LAB_026d7bb6;
LAB_026d76e0:
  if ((sp_key & 0x2000U) == 0)
    goto LAB_026d7ab0;
  if (csvid == 0xbde)
    goto LAB_026d7e2b;
  if (0xbde < csvid)
    goto LAB_026d875b;
  if (csvid == 0xa48)
    goto LAB_026d7e2b;
  if (0xa48 < csvid)
  {
    if (csvid < 0xaff)
    {
      if (csvid < 0xae5)
      {
        if (csvid == 0xa8e)
          goto LAB_026d7e2b;
        if (csvid < 0xa8f)
        {
          if (csvid - 0xa69U < 2)
          {
            uVar2 = uVar2 | 0x2000;
            goto LAB_026d7ab0;
          }
        }
        else if (csvid - 0xaaeU < 2)
          goto LAB_026d7e2b;
      }
      else if ((1U << (cVar1 + 0x1bU & 0x1f) & 0x2000011U) != 0)
      {
        uVar2 = uVar2 | 0x2000;
        goto LAB_026d7ab0;
      }
    }
    goto LAB_026d773d;
  }
  if (csvid == 0x9f6)
  {
  LAB_026d7e2b:
    uVar2 = uVar2 | 0x2000;
  LAB_026d7ab0:
    if ((sp_key & 0x4000U) == 0)
      goto LAB_026d7753;
    if (0x25cd < csvid)
    {
      if (csvid < 0x269a)
      {
        if (0x2664 < csvid)
        {
          switch (csvid)
          {
          case 0x2665:
          case 0x2666:
          case 0x2670:
          case 0x2678:
          case 0x2680:
          case 0x268f:
          case 0x2699:
            goto LAB_026d87bb;
          default:
            goto LAB_026d7753;
          }
        }
        if ((csvid != 0x25f7) &&
            ((0x1d < csvid - 0x262cU ||
              ((1U << ((unsigned char)(csvid - 0x262cU) & 0x1f) & 0x28000001U) == 0))))
          goto LAB_026d7b03;
      }
      else if (csvid != 0x26a7)
        goto LAB_026d7b03;
    LAB_026d87bb:
      uVar2 = uVar2 | 0x4000;
      goto LAB_026d7753;
    }
    if (csvid < 0x25bb)
    {
      if (csvid < 0x2584)
      {
        if (csvid < 0x2562)
          goto LAB_026d7742;
        switch (csvid)
        {
        case 0x2562:
        case 0x2569:
        case 0x2571:
        case 0x2576:
        case 0x2583:
          goto LAB_026d87bb;
        default:
          goto LAB_026d7753;
        }
      }
      if (csvid != 0x2592)
        goto LAB_026d7b03;
      goto LAB_026d87bb;
    }
    if ((1U << (cVar1 + 0x45U & 0x1f) & 0x49001U) != 0)
    {
      uVar2 = uVar2 | 0x4000;
      goto LAB_026d7753;
    }
  LAB_026d7b03:
    if ((sp_key & 0x8000U) != 0)
    {
    LAB_026d80d9:
      if (csvid - 0x1e46U < 2)
        goto LAB_026d7fcb;
    }
  LAB_026d775c:
    if ((sp_key & 0x10000U) == 0)
      goto LAB_026d77c0;
    if (csvid != 0x4455)
    {
      if (0x4455 < csvid)
      {
        if (csvid != 0x4e98)
        {
          if (csvid < 0x4e99)
          {
            if (csvid != 0x4abd)
            {
              if (csvid < 0x4abe)
              {
                if (csvid != 0x4796)
                {
                  if (csvid < 0x4797)
                  {
                    if (csvid != 0x4689)
                    {
                      if (csvid < 0x468a)
                      {
                        if ((csvid == 0x447e) || (csvid == 0x463d))
                          goto LAB_026d77b7;
                      }
                      else if ((csvid == 0x4725) || (csvid == 0x4790))
                        goto LAB_026d77b7;
                      goto LAB_026d7f47;
                    }
                  }
                  else if (csvid != 0x49c9)
                  {
                    if (csvid < 0x49ca)
                    {
                      if ((csvid == 0x4805) || (csvid == 0x4856))
                        goto LAB_026d77b7;
                    }
                    else
                    {
                      if (csvid == 0x4a5a)
                        goto LAB_026d77b7;
                      if (csvid == 0x4aa1)
                      {
                        uVar2 = uVar2 | 0x10000;
                        goto LAB_026d77c0;
                      }
                    }
                    goto LAB_026d7f47;
                  }
                }
              }
              else if (csvid != 0x4cf6)
              {
                if (csvid < 0x4cf7)
                {
                  if (csvid != 0x4bd7)
                  {
                    if (csvid < 0x4bd8)
                    {
                      if ((csvid == 0x4ac7) || (csvid == 0x4b88))
                        goto LAB_026d77b7;
                    }
                    else if ((csvid == 0x4cb4) || (csvid == 0x4ce6))
                      goto LAB_026d77b7;
                    goto LAB_026d7f47;
                  }
                }
                else if (csvid != 0x4dca)
                {
                  if (csvid < 0x4dcb)
                  {
                    if ((csvid == 0x4d97) || (csvid == 0x4db4))
                      goto LAB_026d77b7;
                  }
                  else if ((csvid == 0x4dec) || (csvid == 0x4e6b))
                    goto LAB_026d77b7;
                  goto LAB_026d7f47;
                }
              }
            }
          }
          else if (csvid != 0x5351)
          {
            if (csvid < 0x5352)
            {
              if (csvid != 0x511f)
              {
                if (csvid < 0x5120)
                {
                  if (csvid != 0x5058)
                  {
                    if (csvid < 0x5059)
                    {
                      if ((csvid == 0x4ff5) || (csvid == 0x502e))
                        goto LAB_026d77b7;
                    }
                    else if ((csvid == 0x50af) || (csvid == 0x5119))
                      goto LAB_026d77b7;
                    goto LAB_026d7f47;
                  }
                }
                else if (csvid != 0x52c9)
                {
                  if (csvid < 0x52ca)
                  {
                    if ((csvid == 0x51e5) || (csvid == 0x523d))
                      goto LAB_026d77b7;
                  }
                  else if ((csvid == 0x52fa) || (csvid == 0x533f))
                    goto LAB_026d77b7;
                  goto LAB_026d7f47;
                }
              }
            }
            else if (csvid != 0x5518)
            {
              if (csvid < 0x5519)
              {
                if (csvid != 0x5403)
                {
                  if (csvid < 0x5404)
                  {
                    if ((csvid == 0x539f) || (csvid == 0x53f6))
                      goto LAB_026d77b7;
                  }
                  else if ((csvid == 0x5428) || (csvid == 0x54da))
                    goto LAB_026d77b7;
                  goto LAB_026d7f47;
                }
              }
              else if (csvid != 0x564a)
              {
                if (csvid < 0x564b)
                {
                  if ((csvid == 0x55ab) || (csvid == 0x562c))
                    goto LAB_026d77b7;
                }
                else if (csvid == 0x56b1)
                {
                  uVar2 = uVar2 | 0x10000;
                  goto LAB_026d77c0;
                }
                goto LAB_026d7f47;
              }
            }
          }
        }
        goto LAB_026d77b7;
      }
      if (csvid == 0x3ef4)
        goto LAB_026d77b7;
      if (csvid < 0x3ef5)
      {
        if (csvid == 0x3b21)
          goto LAB_026d77b7;
        if (csvid < 0x3b22)
        {
          if (csvid == 0x270d)
            goto LAB_026d77b7;
          if (csvid < 0x270e)
            goto LAB_026db89c;
          if (csvid == 0x34d6)
            goto LAB_026d77b7;
          if (csvid < 0x34d7)
          {
            if ((csvid == 0x2e4c) || (csvid == 0x32ee))
              goto LAB_026d77b7;
          }
          else if ((csvid == 0x3a5e) || (csvid == 0x3af1))
            goto LAB_026d77b7;
          goto LAB_026d77c0;
        }
        if (csvid == 0x3df9)
          goto LAB_026d77b7;
        if (csvid < 0x3dfa)
        {
          if (csvid < 0x3cf7)
          {
            if (((csvid < 0x3cf5) && (csvid != 0x3be3)) && (csvid != 0x3c4c))
              goto LAB_026d77c0;
            goto LAB_026d77b7;
          }
          if ((csvid == 0x3d0f) || (csvid == 0x3dd2))
            goto LAB_026d77b7;
          if ((sp_key & 0x20000U) == 0)
            goto LAB_026d7f47;
          goto LAB_026db802;
        }
        if (csvid == 0x3e52)
          goto LAB_026d77b7;
        if (0x3e52 < csvid)
        {
          if ((csvid != 0x3e8f) && (csvid != 0x3eca))
            goto LAB_026d8006;
          goto LAB_026d77b7;
        }
        if ((csvid == 0x3e17) || (csvid == 0x3e3c))
          goto LAB_026d77b7;
        if ((sp_key & 0x20000U) == 0)
          goto LAB_026d7f47;
        goto LAB_026da332;
      }
      if (csvid == 0x41d5)
        goto LAB_026d77b7;
      if (0x41d5 < csvid)
      {
        if (csvid == 0x430e)
          goto LAB_026d77b7;
        if (csvid < 0x430f)
        {
          if (csvid == 0x4261)
            goto LAB_026d77b7;
          if (csvid < 0x4262)
          {
            if ((csvid == 0x41f0) || (csvid == 0x420e))
              goto LAB_026d77b7;
          }
          else if ((csvid == 0x42e3) || (csvid == 0x42f7))
            goto LAB_026d77b7;
        }
        else if (csvid < 0x4408)
        {
          if (csvid < 0x43f2)
          {
            if ((csvid == 0x439a) || (csvid == 0x43a3))
              goto LAB_026d77b7;
          }
          else if ((1U << (cVar1 + 0xeU & 0x1f) & 0x208003U) != 0)
            goto LAB_026d77b7;
        }
        goto LAB_026d7f47;
      }
      if (csvid == 0x40b9)
        goto LAB_026d77b7;
      if (0x40b9 < csvid)
      {
        if (csvid != 0x4146)
        {
          if (csvid < 0x4147)
          {
            if ((csvid == 0x40e2) || (csvid == 0x4118))
              goto LAB_026d77b7;
          }
          else if ((csvid == 0x4163) || (csvid == 0x41a6))
            goto LAB_026d77b7;
          goto LAB_026d7f47;
        }
        goto LAB_026d77b7;
      }
      if (csvid == 0x403e)
        goto LAB_026d77b7;
      if (0x403e < csvid)
      {
        if ((csvid != 0x4051) && (csvid != 0x4064))
          goto LAB_026d7f47;
        goto LAB_026d77b7;
      }
      if ((csvid == 0x3f9c) || (csvid == 0x3fd8))
        goto LAB_026d77b7;
      if ((sp_key & 0x20000U) == 0)
        goto LAB_026d7f47;
    LAB_026d77dd:
      if (csvid != 0x3f64)
      {
        if (csvid < 0x3f65)
        {
          if ((csvid != 0x3e86) && (csvid != 0x3ee4))
            goto LAB_026d7804;
        }
        else if (csvid != 0x3f6e)
          goto LAB_026d7f47;
      }
      goto LAB_026da344;
    }
  LAB_026d77b7:
    uVar2 = uVar2 | 0x10000;
  LAB_026d77c0:
    if ((sp_key & 0x20000U) != 0)
      goto LAB_026d77cc;
  LAB_026d7f47:
    if (((sp_key & 0x20000000U) != 0) && (csvid == 0x4622))
    {
      uVar2 = uVar2 | 0x20000000;
      goto joined_r0x026d7f21;
    }
  LAB_026d7804:
    if ((sp_key & 0x800000U) == 0)
      goto LAB_026d7ece;
  LAB_026d7810:
    if (csvid == 0x3924)
    {
    LAB_026d7ec8:
      uVar2 = uVar2 | 0x800000;
      goto LAB_026d7ece;
    }
    if (0x3924 < csvid)
    {
      if (csvid != 0x4814)
      {
        if (csvid < 0x4815)
        {
          if (csvid != 0x4276)
          {
            if (csvid < 0x4277)
            {
              if (csvid != 0x3d46)
              {
                if (csvid < 0x3d47)
                {
                  if (csvid != 0x3c2a)
                  {
                    if (csvid < 0x3c2b)
                    {
                      if (csvid == 0x3a08)
                        goto LAB_026d7ec8;
                      if (csvid != 0x3a17)
                        goto joined_r0x026d7f21;
                      uVar2 = uVar2 | 0x800000;
                    }
                    else
                    {
                      if (csvid != 0x3cfb)
                        goto joined_r0x026d7f21;
                      uVar2 = uVar2 | 0x800000;
                    }
                    goto LAB_026d7ece;
                  }
                }
                else if ((csvid != 0x3db6) && (csvid != 0x411f))
                {
                  if (csvid != 0x3d80)
                    goto joined_r0x026d7f21;
                  uVar2 = uVar2 | 0x800000;
                  goto LAB_026d7ece;
                }
              }
            }
            else if (csvid != 0x45f7)
            {
              if (csvid < 0x45f8)
              {
                if ((csvid == 0x43d6) || (csvid == 0x4427))
                  goto LAB_026d7ec8;
                if (csvid != 0x434d)
                  goto joined_r0x026d7f21;
                uVar2 = uVar2 | 0x800000;
              }
              else
              {
                if (csvid == 0x47c7)
                  goto LAB_026d7ec8;
                if (csvid < 0x47c8)
                {
                  if (csvid != 0x4776)
                    goto joined_r0x026d7f21;
                  uVar2 = uVar2 | 0x800000;
                }
                else
                {
                  if (1 < csvid - 0x480fU)
                    goto joined_r0x026d7f21;
                  uVar2 = uVar2 | 0x800000;
                }
              }
              goto LAB_026d7ece;
            }
          }
        }
        else if (csvid != 0x4f8f)
        {
          if (csvid < 0x4f90)
          {
            if (csvid != 0x4bf6)
            {
              if (csvid < 0x4bf7)
              {
                if (csvid != 0x4b4f)
                {
                  if (csvid < 0x4b50)
                  {
                    if (csvid == 0x4a63)
                      goto LAB_026d7ec8;
                    if (csvid != 0x4ac0)
                      goto joined_r0x026d7f21;
                    uVar2 = uVar2 | 0x800000;
                  }
                  else
                  {
                    if (csvid != 0x4bf2)
                      goto joined_r0x026d7f21;
                    uVar2 = uVar2 | 0x800000;
                  }
                  goto LAB_026d7ece;
                }
              }
              else if ((csvid != 0x4d29) && (csvid != 0x4e55))
              {
                if (csvid != 0x4cc6)
                  goto joined_r0x026d7f21;
                uVar2 = uVar2 | 0x800000;
                goto LAB_026d7ece;
              }
            }
          }
          else if (csvid != 0x54a9)
          {
            if (csvid < 0x54aa)
            {
              if ((csvid == 0x544e) || (csvid == 0x5481))
                goto LAB_026d7ec8;
              if (csvid != 0x5404)
                goto joined_r0x026d7f21;
              uVar2 = uVar2 | 0x800000;
            }
            else
            {
              if ((csvid == 0x5538) || (csvid == 0x569d))
                goto LAB_026d7ec8;
              if (csvid != 0x54ab)
                goto joined_r0x026d7f21;
              uVar2 = uVar2 | 0x800000;
            }
            goto LAB_026d7ece;
          }
        }
      }
      goto LAB_026d7ec8;
    }
    if (csvid == 0x307e)
      goto LAB_026d7ec8;
    if (0x307e < csvid)
    {
      if (csvid == 0x3618)
        goto LAB_026d7ec8;
      if (csvid < 0x3619)
      {
        if (csvid == 0x333a)
          goto LAB_026d7ec8;
        if (0x333a < csvid)
        {
          if (((csvid == 0x344c) || (csvid == 0x34fe)) || (csvid == 0x3431))
            goto LAB_026d7ec8;
          if ((sp_key & 0x2000000U) == 0)
            goto joined_r0x026d7f21;
          goto LAB_026db312;
        }
        if (csvid == 0x3208)
          goto LAB_026d7ec8;
        if (0x3208 < csvid)
        {
          if (csvid == 0x32c0)
            goto LAB_026d7ec8;
          if ((sp_key & 0x2000000U) == 0)
            goto joined_r0x026d7f21;
          goto LAB_026db31f;
        }
        if ((csvid == 0x30b1) || (csvid == 0x3138))
          goto LAB_026d7ec8;
        if ((sp_key & 0x2000000U) == 0)
          goto joined_r0x026d7f21;
        goto LAB_026db5d3;
      }
      if (csvid == 0x3819)
        goto LAB_026d7ec8;
      if (csvid < 0x381a)
      {
        if (((csvid == 0x377a) || (csvid == 0x3800)) || (csvid == 0x36dc))
          goto LAB_026d7ec8;
        if ((sp_key & 0x2000000U) == 0)
          goto joined_r0x026d7f21;
        goto LAB_026db2a6;
      }
      if (((csvid - 0x3894U & 0xfffffffb) == 0) || (csvid == 0x385f))
        goto LAB_026d7ec8;
      if ((sp_key & 0x2000000U) == 0)
        goto joined_r0x026d7f21;
    LAB_026db23c:
      if (csvid == 0x387d)
      {
        uVar2 = uVar2 | 0x2000000;
      }
      goto joined_r0x026d7f21;
    }
    if (csvid == 0x2ba2)
      goto LAB_026d7ec8;
    if (0x2ba2 < csvid)
    {
      if (0x2e22 < csvid)
      {
        if ((csvid == 0x2eda) || (csvid == 0x2ee8))
          goto LAB_026d7ec8;
        if (csvid == 0x2eb9)
        {
          uVar2 = uVar2 | 0x800000;
        }
        goto LAB_026d7ece;
      }
      if (0x2e03 < csvid)
      {
        if ((1U << (cVar1 - 4U & 0x1f) & 0x40400001U) == 0)
          goto joined_r0x026d7f21;
        uVar2 = uVar2 | 0x800000;
        goto LAB_026d7ece;
      }
      if (csvid == 0x2d15)
        goto LAB_026d7ec8;
      if ((sp_key & 0x2000000U) == 0)
        goto joined_r0x026d7f21;
      goto LAB_026d7ef0;
    }
    if (csvid == 0x1fbf)
      goto LAB_026d7ec8;
    if (csvid < 0x1fc0)
    {
      if (csvid != 0x1a65)
      {
        if (csvid < 0x1a66)
        {
          if (csvid == 0x10f8)
            goto LAB_026d7ec8;
          if (csvid != 0x1107)
            goto joined_r0x026d7f21;
          uVar2 = uVar2 | 0x800000;
        }
        else
        {
          if (csvid != 0x1aa4)
            goto joined_r0x026d7f21;
          uVar2 = uVar2 | 0x800000;
        }
        goto LAB_026d7ece;
      }
      goto LAB_026d7ec8;
    }
    if (((csvid == 0x268e) || (csvid == 0x287d)) || (csvid == 0x252f))
      goto LAB_026d7ec8;
    if ((sp_key & 0x2000000U) == 0)
      goto joined_r0x026d7f21;
  LAB_026db176:
    if (csvid != 0x290a)
    {
      if (csvid == 0x2941)
      {
        uVar2 = uVar2 | 0x2000000;
      }
      goto joined_r0x026d7f21;
    }
  }
  else
  {
    if (0x9f6 < csvid)
    {
      if ((csvid != 0xa26) && (csvid != 0xa32))
        goto LAB_026d773d;
      goto LAB_026d7e2b;
    }
    if (csvid == 0x9ab)
      goto LAB_026d7e2b;
    if (0x9ab < csvid)
    {
      if (csvid != 0x9d3)
        goto LAB_026d773d;
      uVar2 = uVar2 | 0x2000;
      goto LAB_026d7ab0;
    }
    if (csvid - 0x975U < 2)
    {
      uVar2 = uVar2 | 0x2000;
      goto LAB_026d7ab0;
    }
  LAB_026d773d:
    if ((sp_key & 0x4000U) != 0)
    {
    LAB_026d7742:
      if ((csvid & 0xfffffff7U) == 0x2510)
        goto LAB_026d87bb;
    }
  LAB_026d7753:
    if ((sp_key & 0x8000U) == 0)
      goto LAB_026d775c;
    if (csvid == 0x1d44)
    {
    LAB_026d7fcb:
      uVar2 = uVar2 | 0x8000;
      goto LAB_026d775c;
    }
    if (0x1d44 < csvid)
    {
      if (csvid != 0x1db1)
      {
        if (csvid < 0x1db2)
        {
          if (csvid < 0x1d93)
          {
            if (csvid < 0x1d7a)
            {
              if ((csvid == 0x1d69) || ((csvid & 0xfffffff7U) == 0x1d47))
                goto LAB_026d7fcb;
            }
            else if ((1U << (cVar1 + 0x86U & 0x1f) & 0x1004001U) != 0)
              goto LAB_026d7fcb;
          }
          else if (csvid == 0x1da0)
            goto LAB_026d7fcb;
        }
        else
        {
          if (0x1e05 < csvid)
          {
            if (csvid != 0x1e2f)
              goto LAB_026d80d9;
            goto LAB_026d7fcb;
          }
          if (0x1de4 < csvid)
          {
            switch (csvid)
            {
            case 0x1de5:
            case 0x1de6:
            case 0x1dec:
            case 0x1df1:
            case 0x1e00:
            case 0x1e05:
              goto LAB_026d7fcb;
            }
          }
        }
        goto LAB_026d775c;
      }
      goto LAB_026d7fcb;
    }
    if (csvid < 0x1ca0)
    {
      if (0x1c6d < csvid)
      {
        switch (csvid)
        {
        case 0x1c6e:
        case 0x1c77:
        case 0x1c79:
        case 0x1c8c:
        case 0x1c93:
        case 0x1c99:
        case 0x1c9f:
          goto LAB_026d7fcb;
        }
        goto LAB_026d775c;
      }
      switch (csvid)
      {
      case 0x1bfa:
      case 0x1c02:
      case 0x1c07:
      case 0x1c14:
      case 0x1c1a:
        goto LAB_026d7fcb;
      case 0x1bfb:
      case 0x1bfc:
      case 0x1bfd:
      case 0x1bfe:
      case 0x1bff:
      case 0x1c00:
      case 0x1c01:
      case 0x1c03:
      case 0x1c04:
      case 0x1c05:
      case 0x1c06:
      case 0x1c08:
      case 0x1c09:
      case 0x1c0a:
      case 0x1c0b:
      case 0x1c0c:
      case 0x1c0d:
      case 0x1c0e:
      case 0x1c0f:
      case 0x1c10:
      case 0x1c11:
      case 0x1c12:
      case 0x1c13:
      case 0x1c15:
      case 0x1c16:
      case 0x1c17:
      case 0x1c18:
      case 0x1c19:
        goto LAB_026d775c;
      default:
        if ((sp_key & 0x10000U) == 0)
          goto LAB_026d77c0;
      }
    LAB_026db89c:
      if (csvid == 0xf6a)
        goto LAB_026d77b7;
      if (0xf6a < csvid)
        goto LAB_026d7ff0;
      if ((csvid == 0x49a) || (csvid == 0x86d))
        goto LAB_026d77b7;
    }
    else
    {
      if (csvid < 0x1cca)
      {
        if ((csvid < 0x1cc1) || ((1U << (cVar1 + 0x3fU & 0x1f) & 0x131U) == 0))
          goto LAB_026d775c;
        goto LAB_026d7fcb;
      }
      if (csvid == 0x1ce8)
        goto LAB_026d7fcb;
      if ((sp_key & 0x10000U) == 0)
        goto LAB_026d77c0;
    LAB_026d7ff0:
      if ((csvid == 0x1d00) || (csvid == 0x255c))
        goto LAB_026d77b7;
    }
  LAB_026d8006:
    if ((sp_key & 0x20000U) != 0)
    {
    LAB_026d77cc:
      if (csvid == 0x3e60)
        goto LAB_026da344;
      if (0x3e60 < csvid)
        goto LAB_026d77dd;
    LAB_026db802:
      if (csvid < 0x3dd1)
      {
        if ((0x3dc4 < csvid) && ((1U << (cVar1 + 0x3bU & 0x1f) & 0x821U) != 0))
        {
        LAB_026da344:
          uVar2 = uVar2 | 0x20000;
          goto LAB_026d7f47;
        }
      }
      else
      {
      LAB_026da332:
        if ((csvid == 0x3e2a) || (csvid == 0x3e37))
          goto LAB_026da344;
      }
      goto LAB_026d7804;
    }
    if ((sp_key & 0x800000U) != 0)
      goto LAB_026d7810;
  LAB_026d7ece:
    if ((sp_key & 0x2000000U) == 0)
      goto joined_r0x026d7f21;
    if (csvid < 0x2fc9)
    {
      if (0x2fae < csvid)
      {
        if ((1U << (cVar1 + 0x51U & 0x1f) & 0x2000101U) != 0)
        {
          uVar2 = uVar2 | 0x2000000;
        }
        goto joined_r0x026d7f21;
      }
    LAB_026d7ef0:
      if (csvid != 0x2d31)
      {
        if (csvid < 0x2d32)
        {
          if (csvid != 0x2bc8)
          {
            if (0x2bc8 < csvid)
            {
              if (csvid == 0x2bfd)
              {
                uVar2 = uVar2 | 0x2000000;
              }
              goto joined_r0x026d7f21;
            }
            goto LAB_026db176;
          }
        }
        else if (csvid != 0x2f33)
        {
          if (csvid < 0x2f34)
          {
            if (csvid != 0x2e73)
            {
              if (csvid == 0x2eb6)
              {
                uVar2 = uVar2 | 0x2000000;
              }
              goto joined_r0x026d7f21;
            }
          }
          else if (csvid != 0x2f89)
            goto joined_r0x026d7f21;
        }
      }
    }
    else
    {
    LAB_026db312:
      if (csvid != 0x33fc)
      {
        if (csvid < 0x33fd)
        {
        LAB_026db31f:
          if (csvid != 0x32f8)
          {
            if (0x32f8 < csvid)
            {
              if (csvid < 0x33c7)
              {
                if (0x33c4 < csvid)
                {
                  uVar2 = uVar2 | 0x2000000;
                }
              }
              else if (csvid == 0x33dc)
              {
                uVar2 = uVar2 | 0x2000000;
              }
              goto joined_r0x026d7f21;
            }
            if (csvid != 0x306b)
            {
            LAB_026db5d3:
              if (csvid == 0x30f8)
              {
                uVar2 = uVar2 | 0x2000000;
              }
              goto joined_r0x026d7f21;
            }
          }
        }
        else
        {
        LAB_026db2a6:
          if (csvid != 0x372e)
          {
            if (0x372e < csvid)
              goto LAB_026db23c;
            if (csvid != 0x3459)
            {
              if (csvid == 0x3596)
              {
                uVar2 = uVar2 | 0x2000000;
              }
              goto joined_r0x026d7f21;
            }
          }
        }
      }
    }
  }
  uVar2 = uVar2 | 0x2000000;
joined_r0x026d7f21:
  if ((sp_key & 0x40000000U) != 0)
  {
    switch (csvid)
    {
    case 0x107a:
    case 0x107e:
    case 0x1084:
    case 0x1086:
    case 0x108a:
    case 0x108e:
    case 0x108f:
    case 0x1099:
    case 0x109b:
    case 0x10a1:
    case 0x10a4:
    case 0x10a6:
    case 0x10a8:
    case 0x10ae:
    case 0x10be:
    case 0x10c0:
    case 0x10c1:
    case 0x10c2:
    case 0x10c3:
    case 0x10c6:
    case 0x10cb:
    case 0x10d9:
    case 0x10da:
    case 0x10e1:
    case 0x10e2:
    case 0x10e4:
    case 0x10ef:
      return uVar2 | 0x40000000;
    }
  }
  return uVar2;
}


/* ManalinkEh 2022: 0x26df840; original functions/abilities.c:2780. */
int get_special_abilities(int player, int card)
{
  int result;
  card_instance_t *instance;
  if (player == -1)
  {
    result = has_sp_key_by_csvid_impl((unsigned int)cards_data[card].id, 0x6283eb0f);
    return result;
  }
  instance = get_card_instance(player, card);
  result = get_special_abilities_by_instance(instance);
  return result;
}


/* ManalinkEh 2022: 0x26df890; original functions/abilities.c:2786. */
int check_for_special_ability(int player, int card, int keyword)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar2;
  if (player == -1)
  {
    result = has_sp_key_by_csvid_impl((unsigned int)cards_data[card].id, keyword);
    return result;
  }
  instance = get_card_instance(player, card);
  uVar2 = get_special_abilities_by_instance(instance);
  return uVar2 & keyword;
}


#ifndef SHANDALAR
/* Recovered October 2022 ability writers and Aura pump consumers. */
static void lure_effect_part(int player, int card, event_t event, int luring_player,
                                  int luring_card, int all_must_block, int subtype, int ability,
                                  const char *prompt);
static void lure_effect(int player, int card, event_t event, int luring_player,
                             int luring_card, int all_must_block, int subtype, int ability,
                             const char *prompt);
static void deathtouch_part(int player, int card);
static void hexproof_part(int player, int card);
static void indestructible_part(int player, int card);
static int attack_if_able_part(int player, int card);
static void vigilance_part(int player, int card);
static void granted_flanking_part(int s_player, int s_card, int t_player, int t_card);

/* ManalinkEh 2022: 0x26dcca0; original functions/abilities.c:881. */
static void lure_effect_part(int player, int card, event_t event, int luring_player,
                                  int luring_card, int all_must_block, int subtype, int ability,
                                  const char *prompt)
{
  uint8_t uVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  unsigned int uVar5;
  int iVar6;
  target_t tgt;
  target_definition_t td;
  char fullprompt[200];

  if (current_turn == luring_player)
  {
    iVar6 = luring_player;
    if (((unsigned char)event_flags & 8) == 0)
    {
      iVar6 = 1 - luring_player;
    }
    if (((reason_for_trigger_controller == iVar6) &&
         (pcVar2 = in_play(luring_player, luring_card), pcVar2 != NULL)) &&
        ((pcVar2->state & 4) != 0))
    {
      if ((all_must_block == 0) && (iVar3 = is_unblocked(luring_player, luring_card), iVar3 == 0))
      {
        return;
      }
      default_target_definition(luring_player, luring_card, &td, 0);
      td.allowed_controller = 1 - luring_player;
      if (subtype < 0)
      {
        subtype = -1;
      }
      td.required_abilities = (int)(ability ^ 0xffffffffU) >> 0x1f & ability;
      td.illegal_abilities = 0;
      td.special = 0x8400;
      td.extra = (int32_t)target_must_be_able_to_block_me;
      td.who_chooses = iVar6;
      td.preferred_controller = td.allowed_controller;
      td.required_subtype = subtype;
      iVar6 = can_target(&td);
      if (iVar6 != 0)
      {
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
        }
        else if (event == EVENT_RESOLVE_TRIGGER)
        {
          if (ai_is_speculating == 1)
          {
            fullprompt[0] = '\0';
          }
          else
          {
            if ((prompt == NULL) || (*prompt == '\0'))
            {
              load_text(NULL, "PROMPT_CHOOSEBLOCKERS");
              prompt = text_lines[0];
            }
            iVar6 = is_what(player, card, 0x80);
            if (iVar6 == 0)
            {
              uVar5 = get_id(player, card);
            }
            else
            {
              pcVar4 = get_card_instance(player, card);
              uVar5 = (unsigned int)pcVar4->display_pic_csv_id;
            }
            scnprintf(fullprompt, 200, "%s: %s", cards_ptr[uVar5]->full_name, prompt);
          }
          trigger_condition = 0xffffffff;
          uVar1 = pcVar2->number_of_targets;
          iVar6 = select_target(luring_player, luring_card, &td, fullprompt, &tgt);
          trigger_condition = 0xda;
          pcVar2->number_of_targets = uVar1;
          if (iVar6 != 0)
          {
            block(tgt.player, tgt.card, luring_player, luring_card);
            if (all_must_block == 0)
            {
              return;
            }
            iVar6 = can_target(&td);
            if (iVar6 == 0)
            {
              return;
            }
          }
          pcVar2 = get_card_instance(player, card);
          pcVar2->state = pcVar2->state & 0xfffffeff;
        }
      }
    }
  }
  return;
}


/* ManalinkEh 2022: 0x26dcfb0; original functions/abilities.c:881. */
static void lure_effect(int player, int card, event_t event, int luring_player,
                             int luring_card, int all_must_block, int subtype, int ability,
                             const char *prompt)
{
  if (trigger_condition != 0xda)
  {
    return;
  }
  if ((((uint32_t)event - (uint32_t)EVENT_TRIGGER < 2U) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    lure_effect_part(player, card, event, luring_player, luring_card, all_must_block, subtype,
                          ability, prompt);
    return;
  }
  return;
}


/* ManalinkEh 2022: 0x26dd080; original functions/abilities.c:651. */
static void deathtouch_part(int player, int card)
{
  card_ext_t *pcVar1;
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  pcVar1 = instance->ext;
  if (((uint32_t)player_bits[player] & 0x40) != 0)
  {
    pcVar1->sp_keywords = pcVar1->sp_keywords & 0xfffffdff;
    return;
  }
  pcVar1->sp_keywords = pcVar1->sp_keywords | 0x200;
  return;
}


/* ManalinkEh 2022: 0x26dd0c0; original functions/abilities.c:653. */
static void hexproof_part(int player, int card)
{
  card_ext_t *pcVar1;
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  pcVar1 = instance->ext;
  if ((((uint32_t)player_bits[player] >> 8 & 0xffU) & 1) != 0)
  {
    pcVar1->sp_keywords = pcVar1->sp_keywords & 0xff7fffff;
    return;
  }
  pcVar1->sp_keywords = pcVar1->sp_keywords | 0x800000;
  return;
}


/* ManalinkEh 2022: 0x26dd110; original functions/abilities.c:655. */
static void indestructible_part(int player, int card)
{
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  instance->ext->sp_keywords = instance->ext->sp_keywords | 0x800;
  return;
}


/* ManalinkEh 2022: 0x26dd150; original functions/abilities.c:803. */
static int attack_if_able_part(int player, int card)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  iVar3 = player;
  instance = get_card_instance(player, card);
  if ((instance->state & 0x8000) == 0)
  {
    instance->state = instance->state | 0x8000;
    iVar2 = can_attack(player, card);
    iVar3 = player;
    if (iVar2 != 0)
    {
      must_attack = 1;
      return player;
    }
  }
  return iVar3;
}


/* ManalinkEh 2022: 0x26dd1a0; original functions/abilities.c:1140. */
static void vigilance_part(int player, int card)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  instance->ext->sp_keywords = instance->ext->sp_keywords | 8;
  if (((player != -1) && (card != -1)) &&
      (result = is_humiliated(player, card, AL_KEY), result != 0))
  {
    return;
  }
  instance->state = instance->state | 0x2000;
  return;
}


/* ManalinkEh 2022: 0x26de480; original functions/abilities.c:651. */
void deathtouch(int player, int card, event_t event)
{
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    deathtouch_part(player, card);
    return;
  }
  return;
}


/* ManalinkEh 2022: 0x26de500; original functions/abilities.c:653. */
void hexproof(int player, int card, event_t event)
{
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    hexproof_part(player, card);
    return;
  }
  return;
}


/* ManalinkEh 2022: 0x26de580; original functions/abilities.c:655. */
void indestructible(int player, int card, event_t event)
{
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    indestructible_part(player, card);
    return;
  }
  return;
}


/* ManalinkEh 2022: 0x26dedc0; original functions/abilities.c:803. */
int attack_if_able(int player, int card, event_t event)
{
  if (((event == EVENT_BEGIN_COMBAT) && (current_turn == player)) && (forbid_attack == 0))
  {
    attack_if_able_part(player, card);
  }
  return 0;
}


/* ManalinkEh 2022: 0x26def40; original functions/abilities.c:861. */
int block_if_able(int player, int card, event_t event)
{
  unsigned int uVar1;
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  uVar1 = instance->ext->sp_keywords;
  if ((uVar1 & 0x4000000) == 0)
  {
    instance->ext->sp_keywords = uVar1 | 0x4000000;
  }
  if ((current_turn != player) && (event == EVENT_DECLARE_BLOCKERS))
  {
    select_blocker(player, card, player);
    return 0;
  }
  return 0;
}


/* ManalinkEh 2022: 0x26df490; original functions/abilities.c:1140. */
void vigilance(int player, int card, event_t event)
{
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    vigilance_part(player, card);
    return;
  }
  return;
}


/* ManalinkEh 2022: 0x26df8e0; original functions/abilities.c:1054. */
static void granted_flanking_part(int s_player, int s_card, int t_player, int t_card)
{
  int result;
  char *pcVar2;
  int card;
  bool bVar3;
  unsigned char bVar4;
  unsigned int uStack_148;
  char marked[2][151];
  bVar4 = 0;
  result = check_for_special_ability(t_player, t_card, 0x2000);
  if (result != 0)
  {
    uStack_148 = 0x12e;
    bVar3 = ((unsigned int)marked & 2) != 0;
    pcVar2 = marked[0];
    if (bVar3)
    {
      marked[0][0] = '\0';
      marked[0][1] = '\0';
      pcVar2 = marked[0] + 2;
      uStack_148 = 300;
    }
    for (uStack_148 = uStack_148 >> 2; uStack_148 != 0; uStack_148 = uStack_148 - 1)
    {
      pcVar2[0] = '\0';
      pcVar2[1] = '\0';
      pcVar2[2] = '\0';
      pcVar2[3] = '\0';
      pcVar2 = pcVar2 + (unsigned int)bVar4 * -8 + 4;
    }
    if (!bVar3)
    {
      pcVar2[0] = '\0';
      pcVar2[1] = '\0';
    }
    mark_each_creature_blocking_me(t_player, t_card, marked);
    result = 1 - current_turn;
    if (0 < active_cards_count[result])
    {
      card = 0;
      do
      {
        if (marked[result][card] != '\0')
        {
          result = check_for_special_ability(result, card, 0x2000);
          if (result == 0)
          {
            result = pump_until_eot(s_player, s_card, 1 - current_turn, card, -1, -1);
            alternate_legacy_text(1, s_player, result);
          }
        }
        result = 1 - current_turn;
        card = card + 1;
      } while (card < active_cards_count[result]);
    }
  }
  return;
}


/* ManalinkEh 2022: 0x26dfa50; original functions/abilities.c:1054. */
void granted_flanking(int s_player, int s_card, int t_player, int t_card, event_t event)
{
  int result;
  card_instance_t *instance;
  if (event == EVENT_ABILITIES)
  {
    if ((t_card == affected_card) && (t_player == affected_card_controller))
    {
      result = check_for_flanking_removal(t_player, t_card);
      if (result == 0)
      {
        instance = get_card_instance(t_player, t_card);
        instance->ext->sp_keywords = instance->ext->sp_keywords | 0x2000;
      }
    }
  }
  else if (event == EVENT_DECLARE_BLOCKERS)
  {
    result = is_attacking(t_player, t_card);
    if (result != 0)
    {
      granted_flanking_part(s_player, s_card, t_player, t_card);
      return;
    }
  }
  return;
}


/* ManalinkEh 2022: 0x26dfaf0; original functions/abilities.c:519. */
void special_abilities(int player, int card, event_t event, int sp_abilities,
                            int source_player, int source_card)
{
  unsigned char bVar1;
  unsigned char bVar2;
  unsigned char bVar3;
  unsigned char bVar4;
  card_instance_t *pcVar5;
  card_instance_t *pcVar6;
  unsigned char bVar7;
  unsigned char bVar8;
  bool bVar9;

  pcVar5 = get_card_instance(player, card);
  bVar9 = event == EVENT_ABILITIES;
  bVar1 = (unsigned char)((unsigned int)sp_abilities >> 1) & bVar9;
  bVar2 = (unsigned char)((unsigned int)sp_abilities >> 4) & bVar9;
  bVar8 = (unsigned char)((unsigned int)sp_abilities >> 2) & bVar9;
  bVar3 = (unsigned char)((unsigned int)sp_abilities >> 5) & bVar9;
  bVar7 = (unsigned char)((unsigned int)sp_abilities >> 3) & bVar9;
  bVar4 = (unsigned char)((unsigned int)sp_abilities >> 8) & bVar9;
  if (((sp_abilities & 1U) == 0) || (!bVar9))
  {
  LAB_026dfb5b:
    if ((bVar1 != 0) && (card == affected_card))
    {
      if (player != affected_card_controller)
        goto LAB_026dfed0;
      pcVar6 = get_card_instance(player, card);
      pcVar6->ext->sp_keywords = pcVar6->ext->sp_keywords | 2;
    }
  LAB_026dfb65:
    if ((bVar8 != 0) && (card == affected_card))
    {
      if (player != affected_card_controller)
        goto LAB_026dfed8;
      pcVar6 = get_card_instance(player, card);
      pcVar6->ext->sp_keywords = pcVar6->ext->sp_keywords | 4;
    }
  LAB_026dfb6d:
    if ((bVar7 == 0) || (card != affected_card))
      goto LAB_026dfb75;
    if (player == affected_card_controller)
    {
      vigilance_part(player, card);
      goto LAB_026dfb75;
    }
  LAB_026dfee0:
    if (bVar2 != 0)
      goto LAB_026dfef0;
  LAB_026dfb7f:
    if ((bVar3 == 0) || (card != affected_card))
      goto LAB_026dfb89;
    if (player == affected_card_controller)
    {
      pcVar6 = get_card_instance(player, card);
      pcVar6->ext->sp_keywords = pcVar6->ext->sp_keywords | 0x20;
      goto LAB_026dfb89;
    }
  LAB_026dff00:
    if (bVar4 != 0)
    {
      bVar1 = (unsigned char)((unsigned int)sp_abilities >> 9) & bVar9;
      goto LAB_026dff18;
    }
  LAB_026dfb93:
    if (((sp_abilities & 0x200U) != 0) && (bVar9))
    {
    LAB_026dff68:
      if ((card == affected_card) && (player == affected_card_controller))
      {
        deathtouch_part(player, card);
      }
      goto LAB_026dfbb4;
    }
  LAB_026dfba2:
    if (((sp_abilities & 0x400U) == 0) || (event != EVENT_BEGIN_COMBAT))
      goto LAB_026dfbb4;
    if ((player == current_turn) && (forbid_attack == 0))
    {
      attack_if_able_part(player, card);
    }
  }
  else
  {
    if (card != affected_card)
      goto LAB_026dfb65;
    if (player == affected_card_controller)
    {
      pcVar6 = get_card_instance(player, card);
      pcVar6->ext->sp_keywords = pcVar6->ext->sp_keywords | 1;
      goto LAB_026dfb5b;
    }
    if (bVar1 == 0)
      goto LAB_026dfb65;
  LAB_026dfed0:
    if (bVar8 == 0)
      goto LAB_026dfb6d;
  LAB_026dfed8:
    if (bVar7 != 0)
      goto LAB_026dfee0;
  LAB_026dfb75:
    if ((bVar2 == 0) || (card != affected_card))
      goto LAB_026dfb7f;
    if (player == affected_card_controller)
    {
      pcVar6 = get_card_instance(player, card);
      pcVar6->ext->sp_keywords = pcVar6->ext->sp_keywords | 0x10;
      goto LAB_026dfb7f;
    }
  LAB_026dfef0:
    if (bVar3 != 0)
      goto LAB_026dff00;
  LAB_026dfb89:
    if (bVar4 == 0)
      goto LAB_026dfb93;
    if (card != affected_card)
    {
      if (((sp_abilities & 0x200U) != 0) && (bVar9))
        goto LAB_026dfbb4;
      goto LAB_026dfba2;
    }
    bVar1 = (unsigned char)((unsigned int)sp_abilities >> 9) & bVar9;
    if (player == affected_card_controller)
    {
      pcVar6 = get_card_instance(player, card);
      pcVar6->ext->sp_keywords = pcVar6->ext->sp_keywords | 0x100;
      if (bVar1 == 0)
        goto LAB_026dfbb4;
      goto LAB_026dff68;
    }
  LAB_026dff18:
    if (bVar1 == 0)
      goto LAB_026dfba2;
  LAB_026dfbb4:
    if (((((sp_abilities & 0x800U) != 0) && (bVar9)) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      indestructible_part(player, card);
    }
  }
  if ((sp_abilities & 0x1000U) != 0)
  {
    lure_effect(source_player, source_card, event, player, card, 0, -1, 0, NULL);
  }
  if ((sp_abilities & 0x2000U) != 0)
  {
    granted_flanking(source_player, source_card, player, card, event);
  }
  if ((((sp_abilities & 0x4000U) != 0) && (bVar9)) &&
      ((card == affected_card && (player == affected_card_controller))))
  {
    pcVar6 = get_card_instance(player, card);
    pcVar6->ext->sp_keywords = pcVar6->ext->sp_keywords | 0x4000;
  }
  if ((sp_abilities & 0x400000U) != 0)
  {
    lure_effect(source_player, source_card, event, player, card, 1, -1, 0, NULL);
  }
  bVar1 = (unsigned char)((unsigned int)sp_abilities >> 0x18);
  if (((sp_abilities & 0x800000U) == 0) || (!bVar9))
  {
  LAB_026dfc0b:
    if ((((bVar1 >> 1 & bVar9) != 0) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      pcVar6 = get_card_instance(player, card);
      pcVar6->ext->sp_keywords = pcVar6->ext->sp_keywords | 0x2000000;
    }
  }
  else if ((card == affected_card) && (player == affected_card_controller))
  {
    hexproof_part(player, card);
    goto LAB_026dfc0b;
  }
  if ((sp_abilities & 0x4000000U) != 0)
  {
    block_if_able(player, card, event);
  }
  if ((sp_abilities & 0x8000000U) != 0)
  {
    pcVar5->ext->sp_keywords = pcVar5->ext->sp_keywords | 0x8000000;
  }
  bVar2 = (unsigned char)((unsigned int)sp_abilities >> 0x15) & bVar9;
  bVar4 = (unsigned char)((unsigned int)sp_abilities >> 0x13) & bVar9;
  bVar3 = (unsigned char)((unsigned int)sp_abilities >> 0x11) & bVar9;
  bVar7 = bVar1 >> 5 & bVar9;
  if (((sp_abilities & 0x10000U) == 0) || (!bVar9))
  {
  LAB_026dfc72:
    if ((bVar4 != 0) && (card == affected_card))
    {
      if (player != affected_card_controller)
        goto LAB_026dfe78;
      pcVar5 = get_card_instance(player, card);
      pcVar5->ext->sp_keywords = pcVar5->ext->sp_keywords | 0x80000;
    }
  LAB_026dfc7a:
    if (bVar2 == 0)
    {
    LAB_026dfc84:
      if ((bVar3 != 0) && (card == affected_card))
      {
        if (player != affected_card_controller)
          goto LAB_026dfe98;
        pcVar5 = get_card_instance(player, card);
        pcVar5->ext->sp_keywords = pcVar5->ext->sp_keywords | 0x20000;
      }
    }
    else if (card == affected_card)
    {
      if (player != affected_card_controller)
        goto LAB_026dfe88;
      pcVar5 = get_card_instance(player, card);
      pcVar5->ext->sp_keywords = pcVar5->ext->sp_keywords | 0x200000;
      goto LAB_026dfc84;
    }
  LAB_026dfc8e:
    if (bVar7 != 0)
    {
      if (card != affected_card)
      {
        return;
      }
      if (player != affected_card_controller)
      {
        return;
      }
      pcVar5 = get_card_instance(player, card);
      pcVar5->ext->sp_keywords = pcVar5->ext->sp_keywords | 0x20000000;
    }
  }
  else
  {
    if (card != affected_card)
      goto LAB_026dfc7a;
    if (player == affected_card_controller)
    {
      pcVar5 = get_card_instance(player, card);
      pcVar5->ext->sp_keywords = pcVar5->ext->sp_keywords | 0x10000;
      goto LAB_026dfc72;
    }
    if (bVar4 == 0)
      goto LAB_026dfc7a;
  LAB_026dfe78:
    if (bVar2 == 0)
      goto LAB_026dfc84;
  LAB_026dfe88:
    if (bVar3 == 0)
      goto LAB_026dfc8e;
  LAB_026dfe98:
    if (bVar7 != 0)
    {
      return;
    }
  }
  if ((((bVar1 >> 6 & bVar9) != 0) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    pcVar5 = get_card_instance(player, card);
    pcVar5->ext->sp_keywords = pcVar5->ext->sp_keywords | 0x40000000;
  }
  return;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void ability_must_block_me(int player, int card, event_t event, int ability, char *prompt);

/* ManalinkEh 2022: 0x26df030; original functions/abilities.c:964. */
void ability_must_block_me(int player, int card, event_t event, int ability, char *prompt)
{
  lure_effect(player, card, event, player, card, 1, -1, ability, prompt);
  return;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 pump effects and cleanup helpers. */


/* ManalinkEh 2022: 0x26dda00; original functions/abilities.c:325. */
int humiliate(int src_player, int src_card, int t_player, int t_card, int mode)
{
  int result;
  test_definition_t test;
  default_test_definition(&test, 0xff);
  test.toughness = 1;
  test.power = 1;
  result = humiliate_and_set_pt_abilities(src_player, src_card, t_player, t_card, mode, &test);
  return result;
}


/* ManalinkEh 2022: 0x26dd4d0; original functions/abilities.c:74. */
void disable_all_activated_abilities(int player, int card, int mode)
{
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  if (instance->targets[0x11].card < 0)
  {
    instance->targets[0x11].card = 0;
  }
  if (mode != 1)
  {
    if (mode == 0)
    {
      instance->targets[0x11].card = instance->targets[0x11].card & 0xffffffc7;
    }
    return;
  }
  instance->targets[0x11].card = instance->targets[0x11].card | 0x38;
  return;
}


/* ManalinkEh 2022: 0x26dd460; original functions/abilities.c:55. */
void disable_nonmana_activated_abilities(int player, int card, int mode)
{
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  if (instance->targets[0x11].card < 0)
  {
    instance->targets[0x11].card = 0;
  }
  if (mode != 1)
  {
    if (mode == 0)
    {
      instance->targets[0x11].card = instance->targets[0x11].card & 0xffffffd7;
    }
    return;
  }
  instance->targets[0x11].card = instance->targets[0x11].card | 0x28;
  return;
}


/* ManalinkEh 2022: 0x26dd890; original functions/abilities.c:302. */
int humiliate_and_set_pt_abilities(int player, int card, int t_player, int t_card, int mode,
                                        test_definition_t *hc)
{
  int card_00;
  card_instance_t *instance;
  unsigned int uVar2;

  if ((mode & 6U) == 0)
  {
    instance = get_card_instance(t_player, t_card);
    if (instance->targets[0x11].card < 0)
    {
      instance->targets[0x11].card = 0;
    }
    if (mode == 1)
    {
      instance->targets[0x11].card = instance->targets[0x11].card | 0x7e;
      return 1;
    }
    if (mode == 0)
    {
      instance->targets[0x11].card = instance->targets[0x11].card & 0xffffff81;
      return 0;
    }
  }
  else
  {
    card_00 = create_targetted_legacy_effect(player, card, humility_legacy, t_player, t_card);
    instance = get_card_instance(player, card_00);
    instance->targets[0].player = t_player;
    instance->targets[0].card = t_card;
    instance = get_card_instance(player, card_00);
    if (hc == NULL)
    {
      instance->targets[1].player = -1;
      instance->targets[1].card = -1;
      instance->targets[2].player = 0;
      instance->targets[2].card = 0;
      instance->targets[3].player = mode;
      uVar2 = 0;
    }
    else
    {
      uVar2 = hc->subtype;
      instance->targets[1].player = hc->power;
      instance->targets[1].card = hc->toughness;
      instance->targets[2].player = hc->keyword;
      instance->targets[2].card = hc->color;
      instance->targets[3].player = mode;
    }
    instance->targets[3].card = uVar2;
    mode = card_00;
  }
  return mode;
}


/* ManalinkEh 2022: 0x26e0300; original functions/abilities.c:229. */
int humility_legacy(int player, int card, event_t event)
{
  int card_00;
  unsigned int uVar1;
  subtype_t subtype;
  int sp_abilities;
  unsigned int uVar2;
  card_instance_t *instance;
  unsigned int uVar4;
  int result;

  if (2 < (unsigned int)player)
  {
    return 0;
  }
  if (0x96 < (unsigned int)card)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  result = (int)instance->damage_target_player;
  if (result < 0)
  {
    return 0;
  }
  card_00 = instance->damage_target_card;
  if ((card_00 != affected_card) || (result != affected_card_controller))
  {
  LAB_026e034d:
    if ((event != EVENT_CLEANUP) && (event != EVENT_SHOULD_AI_PLAY))
    {
      return 0;
    }
    uVar1 = instance->targets[3].player;
    if ((int)uVar1 < 0)
    {
      return 0;
    }
    if ((uVar1 & 4) == 0)
    {
      return 0;
    }
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  switch (event)
  {
  case EVENT_POWER:
    result = instance->targets[1].player;
    goto joined_r0x026e047d;
  case EVENT_TOUGHNESS:
    result = instance->targets[1].card;
  joined_r0x026e047d:
    if (-1 < result)
    {
      set_base_pt(result);
    }
    break;
  case EVENT_ABILITIES:
    uVar1 = instance->targets[2].player;
    sp_abilities = instance->targets[4].player;
    instance = get_card_instance(result, card_00);
    uVar4 = 0x3e;
    uVar2 = instance->targets[0x11].card;
    if (-1 < (int)uVar2)
    {
      uVar4 = uVar2 | 0x3e;
    }
    instance->targets[0x11].card = uVar4;
    instance = get_card_instance(result, card_00);
    instance->state = instance->state & 0xffffdfff;
    instance->ext->sp_keywords = 0;
    if (0 < sp_abilities)
    {
      special_abilities(result, card_00, EVENT_ABILITIES, sp_abilities, player, card);
    }
    event_result = event_result & 0x9f080000U | uVar1 | 0x1f000000;
    break;
  default:
    goto LAB_026e034d;
  case EVENT_SET_COLOR:
    result = instance->targets[2].card;
    if (result != 0)
    {
      event_result = result;
    }
    break;
  case EVENT_CHANGE_TYPE:
    subtype = instance->targets[3].card;
    if (subtype != SUBTYPE_INVALID)
    {
      force_a_subtype(result, card_00, subtype);
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 activated-ability core. */

/* ManalinkEh 2022: 0x26dd340; original functions/abilities.c:13. */
int can_use_activated_abilities(int player, int card)
{
  unsigned int uVar1;
  card_instance_t *instance;
  unsigned int uVar3;

  instance = get_card_instance(player, card);
  uVar1 = instance->targets[0x11].card;
  uVar3 = 1;
  if (0 < (int)uVar1)
  {
    uVar3 = (unsigned int)((uVar1 & 0x28) == 0);
  }
  return uVar3;
}


/* ManalinkEh 2022: 0x26dd380; original functions/abilities.c:19. */
int can_use_activated_abilities_by_instance(card_instance_t *instance)
{
  unsigned int uVar1;
  unsigned int uVar2;
  uVar1 = instance->targets[0x11].card;
  uVar2 = 1;
  if (0 < (int)uVar1)
  {
    uVar2 = (unsigned int)((uVar1 & 0x28) == 0);
  }
  return uVar2;
}


/* ManalinkEh 2022: 0x26dd3a0; original functions/abilities.c:24. */
int can_produce_mana(int player, int card)
{
  card_instance_t *instance;
  unsigned int uVar2;
  unsigned int uVar3;
  unsigned char bVar4;

  instance = get_card_instance(player, card);
  uVar3 = instance->targets[0x11].card;
  if ((int)uVar3 < 1)
  {
    bVar4 = 1;
    uVar3 = 1;
  }
  else
  {
    uVar2 = uVar3 >> 4 ^ 1;
    uVar3 = uVar2 & 1;
    bVar4 = (unsigned char)uVar2 & 1;
  }
  if ((player == 1) && (bVar4 != 0))
  {
    uVar3 = (unsigned int)((instance->ext->special_flags & SF_MANA_PRODUCER_DISABLED_FOR_AI) == 0);
  }
  return uVar3;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26dda70; original functions/abilities.c:332. */
void remove_abilities_by_test(int player, event_t event, test_definition_t *test,
                                   ability_loss_t mode)
{
  ability_loss_t aVar1;
  card_instance_t *instance;
  int result;

  if ((((event == EVENT_ABILITIES) || (event == EVENT_CAST_SPELL)) &&
       (instance = in_play(affected_card_controller, affected_card), instance != NULL)) &&
      (((player == 2 || (affected_card_controller == player)) &&
        ((test == NULL ||
          (result = new_make_test_in_play(affected_card_controller, affected_card, -1, test),
           result != 0))))))
  {
    instance = get_card_instance(affected_card_controller, affected_card);
    aVar1 = instance->targets[0x11].card;
    if (-1 < (int)aVar1)
    {
      mode = mode | aVar1;
    }
    instance->targets[0x11].card = mode;
    return;
  }
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26e09d0; original functions/abilities.c:2981. */
void protection_from_type(int player, int card, event_t event, type_t type)
{
  card_instance_t *instance;
  int result;
  int p;
  int c;
  if ((type & (TYPE_ARTIFACT | TYPE_INSTANT | TYPE_SORCERY | TYPE_CREATURE | TYPE_LAND)) !=
      TYPE_NONE)
  {
    backtrace_and_abort("functions/abilities.c:2983\n!(type & (TYPE_ARTIFACT | TYPE_INSTANT | "
                        "TYPE_SORCERY | TYPE_CREATURE | TYPE_LAND))");
  }
  if (event == EVENT_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      set_misc_protection_impl(player, card);
      return;
    }
  }
  else if (((event == EVENT_PROT_MISC) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    if (((unsigned int)event_result >> 24 & 0xff) == '\x06')
    {
      event_result = event_result | 1;
      return;
    }
    get_protection_source(1, &p, &c);
    instance = get_card_instance(p, c);
    if (instance->internal_card_id == damage_card)
    {
      if ((type & instance->targets[3].player) == TYPE_NONE)
      {
        return;
      }
    }
    else
    {
      result = is_what(p, c, type);
      if (result == 0)
      {
        return;
      }
    }
    event_result = event_result | 1;
    return;
  }
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26e0b90; original functions/abilities.c:3018. */
void protection_from_colorless(int player, int card, event_t event)
{
  unsigned int uVar1;
  int p;
  int c;
  if (event == EVENT_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      set_misc_protection_impl(player, card);
      return;
    }
  }
  else if (((event == EVENT_PROT_MISC) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    if (((unsigned int)event_result >> 24 & 0xff) == '\x06')
    {
      event_result = event_result | 1;
      return;
    }
    get_protection_source(1, &p, &c);
    uVar1 = get_color(p, c);
    if ((uVar1 & 0x3e) == 0)
    {
      event_result = event_result | 1;
      return;
    }
  }
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26e0d00; original functions/abilities.c:3048. */
void protection_from_monocolored(int player, int card, event_t event)
{
  int result;
  int p;
  int c;
  if (event == EVENT_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      set_misc_protection_impl(player, card);
      return;
    }
  }
  else if (((event == EVENT_PROT_MISC) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    if (((unsigned int)event_result >> 24 & 0xff) == '\x06')
    {
      event_result = event_result | 1;
      return;
    }
    get_protection_source(1, &p, &c);
    result = count_colors(p, c);
    if (result == 1)
    {
      event_result = event_result | 1;
      return;
    }
  }
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26e0f50; original functions/abilities.c:3089. */
void cannot_be_target_of_abilities(int player, int card, event_t event)
{
  int result;
  int p;
  int c;
  if (event == EVENT_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      set_misc_protection_impl(player, card);
      return;
    }
  }
  else if ((((event == EVENT_PROT_MISC) && (card == affected_card)) &&
            (player == affected_card_controller)) &&
           (((unsigned int)event_result >> 24 & 0xff) == '\x05'))
  {
    get_protection_source(1, &p, &c);
    result = is_being_cast(p, c);
    if (result == 0)
    {
      event_result = event_result | 1;
      return;
    }
  }
  return;
}

#endif


int fx_menace(int player, int card, event_t event)
{
  if (get_card_instance(player, card)->damage_target_player > -1)
  {
    card_instance_t *instance = get_card_instance(player, card);

    special_abilities(instance->damage_target_player, instance->damage_target_card, event,
                      SP_KEYWORD_MENACE, player, card);

    if (event == EVENT_CLEANUP && is_what(player, card, TYPE_EFFECT))
    {
      card_instance_t *inst = get_card_instance(player, card);
      remove_special_ability(inst->damage_target_player, inst->damage_target_card,
                             SP_KEYWORD_MENACE);
      kill_card(player, card, KILL_EXILE);
    }
  }

  return 0;
}

void menace(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x10000;
    return;
  }
  return;
}


void rampage(int player, int card, event_t event, int number)
{
  int result;
  card_instance_t *instance;
  if (event == EVENT_DECLARE_BLOCKERS)
  {
    if (current_turn == player)
    {
      if (((card != -1) && (player != -1)) &&
          (result = is_humiliated(player, card, AL_KEY), result != 0))
      {
        return;
      }
      result = count_my_blockers(player, card);
      if (1 < result)
      {
        result = (result + -1) * number;
        pump_until_eot(player, card, player, card, result, result);
        return;
      }
    }
  }
  else if (event == EVENT_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      instance = get_card_instance(player, card);
      instance->ext->sp_keywords = instance->ext->sp_keywords | 0x40;
      return;
    }
  }
  else if (((event == EVENT_QUERY_AFF) && (card == affected_card)) &&
           ((player == affected_card_controller && (attacking_card == 0x40))))
  {
    if (((player != -1) && (card != -1)) &&
        (result = is_humiliated(player, card, AL_KEY), result != 0))
    {
      return;
    }
    event_result = event_result + number;
    return;
  }
  return;
}


void skulk(int player, int card, event_t event)
{
  card_instance_t *instance;
  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = get_card_instance(player, card);
    instance->ext->sp_keywords = instance->ext->sp_keywords | 0x20000;
    return;
  }
  return;
}


void protection_from_subtype(int player, int card, event_t event, subtype_t subt)
{
  int p, c;
  if (event == EVENT_ABILITIES)
  {
    if (affect_me(player, card))
      set_misc_protection_impl(player, card);
  }
  else if (event == EVENT_PROT_MISC && affect_me(player, card))
  {
    if (((unsigned int)event_result >> 24 & 0xff) == 6)
    {
      event_result |= 1;
      return;
    }
    // Native non-query events require a valid attacking-card source.
    if (attacking_card >= 0)
      get_protection_source_impl(1, &p, &c);
    if (has_subtype(p, c, subt))
      event_result |= 1;
  }
}

void protection_from_multicolored(int player, int card, event_t event)
{
  int result;
  int p;
  int c;
  if (event == EVENT_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      set_misc_protection_impl(player, card);
      return;
    }
  }
  else if (((event == EVENT_PROT_MISC) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    if (((unsigned int)event_result >> 24 & 0xff) == '\x06')
    {
      event_result = event_result | 1;
      return;
    }
    /* Native non-query protection events require a nonnegative source card.
     * The original also leaves p/c unwritten when this precondition fails. */
    if (-1 < attacking_card)
    {
      get_protection_source_impl('\x01', &p, &c);
    }
    result = count_colors(p, c);
    if (1 < result)
    {
      event_result = event_result | 1;
      return;
    }
  }
  return;
}
