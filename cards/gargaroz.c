#include "manalink.h"

// Cards that couls change targets

static const char* has_only_1_target_and_could_legally_change_target(int who_chooses, int player, int card)
{
	card_instance_t* eff = get_card_instance(player, card);
	if( eff->number_of_targets == 0 || eff->number_of_targets > 1)
		return "must have only 1 target";
	if( ! call_card_function(player, card, EVENT_CAN_CHANGE_TARGET) )
		return "has only 1 legal target";

	return NULL;
}

int card_deflection(int player, int card, event_t event){
	// Used externally
	target_definition_t td_spell;
	counterspell_target_definition(player, card, &td_spell, TYPE_SPELL);	// Aura spells also target
	td_spell.extra = (int32_t)has_only_1_target_and_could_legally_change_target;
	td_spell.special |= TARGET_SPECIAL_EXTRA_FUNCTION;

	card_instance_t *instance = get_card_instance( player, card );

	if (event == EVENT_CAN_CAST ){
		return counterspell(player, card, event, &td_spell, 0);
	}

	if(event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		instance->targets[0].player = card_on_stack_controller;
		instance->targets[0].card = card_on_stack;
		call_card_function(instance->targets[0].player, instance->targets[0].card, EVENT_CHANGE_TARGET);
		if( spell_fizzled != 1 ){
			instance->targets[1] = get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[9];
			instance->number_of_targets = 2;
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[9].player = -1;
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[9].card = -1;
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->number_of_targets = 1;
		}
	}

	if(event == EVENT_RESOLVE_SPELL){
		if( counterspell_validate(player, card, &td_spell, 0) ){
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[0] = instance->targets[1];
		}
		kill_card(player, card, KILL_DESTROY);
	}

  return 0;
}

int card_divert(int player, int card, event_t event){

	target_definition_t td_spell;
	counterspell_target_definition(player, card, &td_spell, TYPE_SPELL);
	td_spell.extra = (int32_t)has_only_1_target_and_could_legally_change_target;
	td_spell.special |= TARGET_SPECIAL_EXTRA_FUNCTION;

	card_instance_t *instance = get_card_instance( player, card );

	if (event == EVENT_CAN_CAST ){
		return card_deflection(player, card, event);
	}

	if(event == EVENT_CAST_SPELL && affect_me(player, card) ){
		if( has_mana(card_on_stack_controller, COLOR_COLORLESS, 2) )
			ai_modifier-=25;
		return card_deflection(player, card, event);
	}

	if(event == EVENT_RESOLVE_SPELL){
		if( counterspell_validate(player, card, &td_spell, 0) ){
			charge_mana(instance->targets[0].player, COLOR_COLORLESS, 2);
			if( spell_fizzled == 1 ){
				get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[0] = instance->targets[1];
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

  return 0;
}

int card_imps_michief(int player, int card, event_t event){

	target_definition_t td_spell;
	counterspell_target_definition(player, card, &td_spell, TYPE_SPELL);
	td_spell.extra = (int32_t)has_only_1_target_and_could_legally_change_target;
	td_spell.special |= TARGET_SPECIAL_EXTRA_FUNCTION;

	card_instance_t *instance = get_card_instance( player, card );

	if (event == EVENT_CAN_CAST ){
		return card_deflection(player, card, event);
	}

	if(event == EVENT_CAST_SPELL && affect_me(player, card) ){
		if( life[player]-get_cmc(card_on_stack_controller, card_on_stack) < 6)
			ai_modifier-=25;
		return card_deflection(player, card, event);
	}

	if(event == EVENT_RESOLVE_SPELL){
		if( counterspell_validate(player, card, &td_spell, 0) ){
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[0] = instance->targets[1];
			lose_life(player, get_cmc(instance->targets[0].player, instance->targets[0].card));
		}
		kill_card(player, card, KILL_DESTROY);
	}

  return 0;
}

int card_misdirection(int player, int card, event_t event){

	al_pitchspell(player, card, event, COLOR_BLUE, 1);

	if (event == EVENT_CAN_CAST ){
		return card_deflection(player, card, event);
	}

	if(event == EVENT_CAST_SPELL && affect_me(player, card) ){
		if( casting_al_pitchspell(player, card, event, COLOR_BLUE, 1) ){
			set_special_flags2(card_on_stack_controller, card_on_stack, SF2_MISDIRECTION);
			card_deflection(player, card, event);
			remove_special_flags2(card_on_stack_controller, card_on_stack, SF2_MISDIRECTION);
		}
		else{
			spell_fizzled = 1;
		}
	}

	if(event == EVENT_RESOLVE_SPELL){
		return card_deflection(player, card, event);
	}

	return 0;
}

int quicksilver_dragon_card = -1;
static const char* has_only_1_target_and_could_legally_change_target_and_targets_me(int who_chooses, int player, int card)
{
	card_instance_t* eff = get_card_instance(player, card);
	if( eff->number_of_targets == 0 || eff->number_of_targets > 1)
		return "must have only 1 target";
	if( !(eff->targets[0].player == who_chooses && eff->targets[0].card == quicksilver_dragon_card) )
		return "must target Quicksilver Dragon";
	if( ! call_card_function(player, card, EVENT_CAN_CHANGE_TARGET) )
		return "has only 1 legal target";

	return NULL;
}

int card_quicksilver_dragon(int player, int card, event_t event){
	target_definition_t td_spell;
	counterspell_target_definition(player, card, &td_spell, TYPE_SPELL);
	td_spell.extra = (int32_t)has_only_1_target_and_could_legally_change_target_and_targets_me;
	td_spell.special |= TARGET_SPECIAL_EXTRA_FUNCTION;

	quicksilver_dragon_card = card;

	card_instance_t *instance = get_card_instance( player, card );

	if (event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability(player, card, event, GAA_SPELL_ON_STACK, 0, 0, 1, 0, 0, 0, 0, &td_spell, NULL) ){
			if( call_card_function(card_on_stack_controller, card_on_stack_controller, EVENT_CHANGE_TARGET) ){
				return 99;
			}
		}
	}

	if(event == EVENT_ACTIVATE ){
		set_special_flags2(card_on_stack_controller, card_on_stack_controller, SF2_QUICKSILVER_DRAGON);
		call_card_function(card_on_stack_controller, card_on_stack_controller, EVENT_CHANGE_TARGET);
		if( spell_fizzled != 1 ){
			instance->targets[0] = get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[9];
			instance->number_of_targets = 1;
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[9].player = -1;
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[9].card = -1;
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->number_of_targets = 1;
		}
		remove_special_flags2(card_on_stack_controller, card_on_stack_controller, SF2_QUICKSILVER_DRAGON);
	}

	if(event == EVENT_RESOLVE_ACTIVATION){
		if( counterspell_validate(player, card, &td_spell, 0) ){
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[0] = instance->targets[0];
		}
	}
	return 0;
}

static const char* has_only_1_target_and_could_legally_change_target_and_targets_a_player(int who_chooses, int player, int card)
{
	card_instance_t* eff = get_card_instance(player, card);
	if( eff->number_of_targets == 0 || eff->number_of_targets > 1)
		return "must have only 1 target";
	if( eff->targets[0].card != -1 )
		return "must target a player";
	if( ! call_card_function(player, card, EVENT_CAN_CHANGE_TARGET) )
		return "has only 1 legal target";

	return NULL;
}

int card_rebound(int player, int card, event_t event){

	target_definition_t td_spell;
	counterspell_target_definition(player, card, &td_spell, TYPE_SPELL);
	td_spell.extra = (int32_t)has_only_1_target_and_could_legally_change_target_and_targets_a_player;
	td_spell.special |= TARGET_SPECIAL_EXTRA_FUNCTION;

	card_instance_t *instance = get_card_instance( player, card );

	if (event == EVENT_CAN_CAST ){
		return counterspell(player, card, event, &td_spell, 0);
	}

	if(event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		instance->targets[0].player = card_on_stack_controller;
		instance->targets[0].card = card_on_stack;
		set_special_flags2(instance->targets[0].player, instance->targets[0].card, SF2_REBOUND);
		call_card_function(instance->targets[0].player, instance->targets[0].card, EVENT_CHANGE_TARGET);
		if( spell_fizzled != 1 ){
			instance->targets[1] = get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[9];
			instance->number_of_targets = 2;
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[9].player = -1;
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[9].card = -1;
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->number_of_targets = 1;
		}
		remove_special_flags2(instance->targets[0].player, instance->targets[0].card, SF2_REBOUND);
	}

	if(event == EVENT_RESOLVE_SPELL){
		if( counterspell_validate(player, card, &td_spell, 0) ){
			get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[0] = instance->targets[1];
		}
		kill_card(player, card, KILL_DESTROY);
	}

  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x24279f0; original cards/gargaroz.c:234. */
static char *
has_only_1_target_and_could_legally_change_target_and_targets_a_creature(int who_chooses,
                                                                         int player, int card)
{
  card_instance_t *instance;
  int result;
  char *pcVar3;

  instance = get_card_instance(player, card);
  pcVar3 = "must have only 1 target";
  if (instance->number_of_targets == '\x01')
  {
    result = is_what(instance->targets[0].player, instance->targets[0].card, 2);
    pcVar3 = "must target a creature";
    if (result != 0)
    {
      result = call_card_function(player, card, EVENT_CAN_CHANGE_TARGET);
      pcVar3 = NULL;
      if (result == 0)
      {
        pcVar3 = "has only 1 legal target";
      }
    }
  }
  return pcVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2427d10; original cards/gargaroz.c:57. */
int card_imps_mischief(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;
  target_definition_t td_spell;

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
      counterspell_target_definition(player, card, &td_spell, 0xff);
      td_spell.special = td_spell.special | 0x8000;
      td_spell.extra = (int32_t)has_only_1_target_and_could_legally_change_target;
      get_card_instance(player, card);
      if (((card == affected_card) && (player == affected_card_controller)) &&
          (iVar1 = life[player], iVar2 = get_cmc(card_on_stack_controller, card_on_stack),
           iVar1 - iVar2 < 6))
      {
        ai_modifier = ai_modifier + -0x19;
      }
      goto LAB_02427d86;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      counterspell_target_definition(player, card, &td_spell, 0xff);
      td_spell.special = td_spell.special | 0x8000;
      td_spell.extra = (int32_t)has_only_1_target_and_could_legally_change_target;
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        iVar1 = counterspell_validate(player, card, &td_spell, 0);
        if (iVar1 != 0)
        {
          deflect_one_target(player, card, EVENT_RESOLVE_SPELL, &td_spell);
          iVar1 = get_cmc(instance->targets[0].player, instance->targets[0].card);
          lose_life_2022(player, iVar1);
        }
        kill_card(player, card, KILL_DESTROY);
        return 0;
      }
      goto LAB_02427d86;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  counterspell_target_definition(player, card, &td_spell, 0xff);
  td_spell.special = td_spell.special | 0x8000;
  td_spell.extra = (int32_t)has_only_1_target_and_could_legally_change_target;
  get_card_instance(player, card);
LAB_02427d86:
  iVar1 = deflect_one_target(player, card, event, &td_spell);
  return iVar1;
}

/* ManalinkEh 2022: 0x2428450; original cards/gargaroz.c:199. */
int card_ricochet_trap(int player, int card, event_t event)
{
  int result;
  target_definition_t td_spell;

  if (event == EVENT_MODIFY_COST)
  {
    result = get_trap_condition(1 - player, 0x404);
    if (result < 1)
    {
      return 0;
    }
    COST_COLORLESS = '\0';
    COST_BLACK = '\0';
    COST_BLUE = '\0';
    COST_GREEN = '\0';
    COST_RED = '\x01';
    COST_WHITE = '\0';
    return 0;
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
        counterspell_target_definition(player, card, &td_spell, 0xff);
        td_spell.special = td_spell.special | 0x8000;
        td_spell.extra = (int32_t)has_only_1_target_and_could_legally_change_target;
        if (event == EVENT_RESOLVE_SPELL)
        {
          deflect_one_target(player, card, EVENT_RESOLVE_SPELL, &td_spell);
          kill_card(player, card, KILL_DESTROY);
          return 0;
        }
        goto LAB_024284b5;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  counterspell_target_definition(player, card, &td_spell, 0xff);
  td_spell.special = td_spell.special | 0x8000;
LAB_024284b5:
  td_spell.extra = (int32_t)has_only_1_target_and_could_legally_change_target;
  result = deflect_one_target(player, card, event, &td_spell);
  return result;
}

/* ManalinkEh 2022: 0x24287b0; original cards/gargaroz.c:279. */
int card_redirect(int player, int card, event_t event)
{
  int result;

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
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = counterspell_validate(player, card, NULL, 0);
        if (result != 0)
        {
          deflect_multi_targets(player, card, EVENT_RESOLVE_SPELL, NULL);
        }
        kill_card(player, card, KILL_DESTROY);
        return 0;
      }
      if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
      {
        return 0;
      }
    }
  }
  result = deflect_multi_targets(player, card, event, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2428890; original cards/gargaroz.c:298. */
int card_wild_ricochet(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        counterspell_target_definition(player, card, &td, 0x38);
        if ((card == affected_card) && (player == affected_card_controller))
          goto LAB_02428993;
      }
    }
    else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      counterspell_target_definition(player, card, &td, 0x38);
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = counterspell_validate(player, card, &td, 0);
        if (result != 0)
        {
          instance = get_card_instance(player, card);
          deflect_multi_targets(player, card, EVENT_RESOLVE_SPELL, &td);
          copy_spell_from_stack(player, instance->targets[0].player, instance->targets[0].card);
        }
        kill_card(player, card, KILL_DESTROY);
        return 0;
      }
    }
    else if (event == EVENT_CHANGE_TARGET)
    {
      counterspell_target_definition(player, card, &td, 0x38);
    }
    return 0;
  }
  counterspell_target_definition(player, card, &td, 0x38);
LAB_02428993:
  result = deflect_multi_targets(player, card, event, &td);
  return result;
}

/* ManalinkEh 2022: 0x2428a20; original cards/gargaroz.c:325. */
int card_goblin_flectomancer(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    counterspell_target_definition(player, card, &td, 0x38);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x408, 0, 0, 0, 0, 0, 0,
                                         0, &td, NULL);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 0x408, 0, 0, 0, 0, 0, 0, 0, &td,
                                NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      deflect_multi_targets(player, card, EVENT_CAST_SPELL, &td);
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    counterspell_target_definition(player, card, &td, 0x38);
  }
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  result = counterspell_validate(player, card, &td, 0);
  if (result != 0)
  {
    deflect_multi_targets(player, card, EVENT_RESOLVE_SPELL, &td);
  }
  kill_card(player, card, KILL_SACRIFICE);
  return 0;
}

/* ManalinkEh 2022: 0x2428c10; original cards/gargaroz.c:369. */
int card_willbender(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_UNMORPH)
  {
    iVar3 = has_mana_multi(player, 1, 0, 1, 0, 0, 0);
    iVar2 = 0;
    if (iVar3 != 0)
    {
      iVar2 = deflect_one_target(player, card, EVENT_CAN_CAST, NULL);
      instance->targets[9].card = iVar2;
      if (iVar2 == 0)
      {
        iVar2 = 1;
      }
    }
  }
  else
  {
    iVar2 = morph(player, card, event, 1, 0, 1, 0, 0, 0);
    if (event == EVENT_UNMORPH)
    {
      if (((spell_fizzled == 1) || (card_on_stack_controller == -1)) ||
          (instance->targets[9].card != 99))
      {
        ai_modifier = ai_modifier + -0x60;
        return iVar2;
      }
      deflect_one_target(player, card, EVENT_CAST_SPELL, NULL);
      if (spell_fizzled == 1)
      {
        spell_fizzled = 0;
        ai_modifier = ai_modifier + -0x60;
        instance->targets[9].card = 0;
      }
    }
    else if (event == EVENT_TURNED_FACE_UP)
    {
      if (instance->targets[9].card == 99)
      {
        deflect_one_target(player, card, EVENT_RESOLVE_SPELL, NULL);
      }
      spell_fizzled = 0;
    }
  }
  return iVar2;
}

/* ManalinkEh 2022: 0x24294b0; original cards/gargaroz.c:665. */
int card_aethersnatch(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;
  int card_00;

  pcVar1 = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
  {
    result = counterspell(player, card, EVENT_CAN_CAST, NULL, -1);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      pcVar3 = get_card_instance(card_on_stack_controller, card_on_stack);
      if (pcVar3->number_of_targets == '\x01')
      {
        pcVar1->info_slot = 1;
        result = deflect_one_target(player, card, EVENT_CAST_SPELL, NULL);
        return result;
      }
      pcVar3 = get_card_instance(card_on_stack_controller, card_on_stack);
      if (1 < pcVar3->number_of_targets)
      {
        pcVar1->info_slot = 2;
        result = deflect_multi_targets(player, card, EVENT_CAST_SPELL, NULL);
        return result;
      }
      result = counterspell(player, card, EVENT_CAST_SPELL, NULL, 0);
      return result;
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) &&
           (result = counterspell_validate(player, card, NULL, 0), result != 0))
  {
    result = pcVar1->info_slot;
    if (result == 1)
    {
      deflect_one_target(player, card, EVENT_RESOLVE_SPELL, NULL);
      result = pcVar1->info_slot;
    }
    if (result == 2)
    {
      deflect_multi_targets(player, card, EVENT_RESOLVE_SPELL, NULL);
    }
    result = is_what(pcVar1->targets[0].player, pcVar1->targets[0].card, 0x47);
    if (result != 0)
    {
      pcVar3 = get_card_instance(pcVar1->targets[0].player, pcVar1->targets[0].card);
      result = pcVar3->internal_card_id;
      if (-1 < result)
      {
        obliterate_card(pcVar1->targets[0].player, pcVar1->targets[0].card);
        card_00 = add_card_to_hand(player, result);
        put_into_play(player, card_00);
        result = pcVar1->targets[0].player;
        if (result != player)
        {
          if (result == 1)
          {
            add_state(player, card_00, 0x1000);
          }
          else
          {
            remove_state(player, card_00, 0x1000);
          }
        }
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *has_only_1_target_and_targets_a_creature_and_can_legally_change_target_to_me(
    int who_chooses, int player, int card, int targeting_player, int targeting_card);

/* ManalinkEh 2022: 0x24278a0; original cards/gargaroz.c:487. */
static char *has_only_1_target_and_targets_a_creature_and_can_legally_change_target_to_me(
    int who_chooses, int player, int card, int targeting_player, int targeting_card)
{
  int32_t iVar1;
  int32_t iVar2;
  card_instance_t *instance;
  char *pcVar4;
  int iVar5;

  instance = get_card_instance(player, card);
  pcVar4 = "must have only 1 target";
  if (instance->number_of_targets == '\x01')
  {
    iVar5 = is_what(instance->targets[0].player, instance->targets[0].card, 2);
    pcVar4 = "must target a creature";
    if (iVar5 != 0)
    {
      iVar1 = instance->targets[9].player;
      instance->targets[9].player = targeting_player;
      iVar2 = instance->targets[9].card;
      instance->targets[9].card = targeting_card;
      set_special_flags2_2022(player, card, 0x10000);
      iVar5 = call_card_function(player, card, EVENT_CAN_CAST);
      instance->targets[9].card = iVar2;
      instance->targets[9].player = iVar1;
      remove_special_flags2_2022(player, card, 0x10000);
      pcVar4 = "cannot target this creature";
      if (iVar5 != 0)
      {
        pcVar4 = NULL;
      }
    }
  }
  return pcVar4;
}

/* ManalinkEh 2022: 0x2428fe0; original cards/gargaroz.c:504. */
int card_muck_drubb(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td_spell;

  counterspell_target_definition(player, card, &td_spell, 0xff);
  td_spell.special = td_spell.special | 0x8000;
  td_spell.extra =
      (int32_t)has_only_1_target_and_targets_a_creature_and_can_legally_change_target_to_me;
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
  {
    result = counterspell(player, card, EVENT_CAN_CAST, &td_spell, 0);
    if (result == 0)
    {
      instance->info_slot = 0;
      return 1;
    }
    instance->info_slot = 1;
    result = 99;
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if (((card == affected_card) && (player == affected_card_controller)) &&
          (current_turn == player))
      {
        ai_modifier = ai_modifier + -10;
      }
      etb_ability_2022(player, card, EVENT_CAST_SPELL, RESOLVE_TRIGGER_MANDATORY);
    }
    else
    {
      etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
      result = card_on_stack_controller;
      if ((event == EVENT_ETB) && (instance->info_slot == 1))
      {
        instance->number_of_targets = '\x01';
        instance->targets[0].player = result;
        instance->targets[0].card = card_on_stack;
        result = counterspell_validate(player, card, &td_spell, 0);
        if (result != 0)
        {

          instance = get_card_instance(instance->targets[0].player, instance->targets[0].card);
          instance->targets[0].player = player;
          instance->targets[0].card = card;
        }
      }
    }
    result = madness(player, card, event, 2, 1, 0, 0, 0, 0);
  }
  return result;
}

/* ManalinkEh 2022: 0x24277b0; original cards/gargaroz.c:550. */
static char *has_only_1_target_and_can_legally_change_target_to_attached(
    int who_chooses, int spell_player, int spell_card, int targeting_player, int targeting_card)
{
  int32_t iVar1;
  int32_t iVar2;
  card_instance_t *pcVar3;
  char *pcVar4;
  card_instance_t *pcVar5;
  int iVar6;

  pcVar3 = get_card_instance(spell_player, spell_card);
  pcVar4 = "must have only 1 target";
  if (pcVar3->number_of_targets == '\x01')
  {
    pcVar5 = get_card_instance(targeting_player, targeting_card);
    iVar6 = pcVar5->damage_target_card;
    if ((pcVar5->damage_target_player < 0) || (iVar6 < 0))
    {
      return "targeting card must be attached";
    }
    iVar1 = pcVar3->targets[9].player;
    pcVar3->targets[9].player = (int)pcVar5->damage_target_player;
    iVar2 = pcVar3->targets[9].card;
    pcVar3->targets[9].card = iVar6;
    set_special_flags2_2022(spell_player, spell_card, 0x10000);
    iVar6 = call_card_function(spell_player, spell_card, EVENT_CAN_CAST);
    pcVar3->targets[9].player = iVar1;
    pcVar3->targets[9].card = iVar2;
    remove_special_flags2_2022(spell_player, spell_card, 0x10000);
    pcVar4 = "cannot target this creature";
    if (iVar6 != 0)
    {
      pcVar4 = NULL;
    }
  }
  return pcVar4;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x24291c0; original cards/gargaroz.c:571. */
int card_captured_by_the_consulate(int player, int card, event_t event)
{
  int32_t iVar1;
  card_instance_t *pcVar2;
  int iVar3;
  unsigned int uVar4;
  card_instance_t *pcVar5;
  target_definition_t td;

  pcVar2 = in_play(player, card);
  if (((((((pcVar2 == NULL) || (pcVar2->damage_target_player == -1)) ||
          (cannot_attack((int)pcVar2->damage_target_player, pcVar2->damage_target_card, event),
           trigger_condition != 0xd3)) ||
         ((card != affected_card || (player != affected_card_controller)))) ||
        ((reason_for_trigger_controller != player ||
          ((iVar3 = is_humiliated_for_ability(player, card, AL_TRIG),
            iVar3 != 0 || (trigger_cause_controller == player)))))) ||
       (iVar3 = is_what(trigger_cause_controller, trigger_cause, 1), iVar3 != 0)) ||
      ((iVar3 = is_token(trigger_cause_controller, trigger_cause),
        iVar3 != 0 ||
            (iVar3 = check_special_flags_2022(trigger_cause_controller, trigger_cause, 0x40000),
             iVar3 != 0))))
  {
  LAB_024291ec:
    if (event == EVENT_CAN_CAST)
      goto LAB_02429210;
    if (event != EVENT_CAST_SPELL)
      goto LAB_024291fa;
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_02429210;
  }
  else
  {
    if (event == EVENT_TRIGGER)
    {
      counterspell_target_definition(player, card, &td, 0xff);
      td.illegal_abilities = 0;
      td.extra = (int32_t)has_only_1_target_and_can_legally_change_target_to_attached;
      td.special = 0x8000;
      pcVar2->targets[0].player = trigger_cause_controller;
      iVar3 = trigger_cause;
      pcVar2->number_of_targets = '\x01';
      pcVar2->targets[0].card = iVar3;
      iVar3 = valid_target(&td);
      uVar4 = 0;
      if (iVar3 != 0)
      {
        uVar4 = event_result | 2;
      }
      event_result = uVar4;
      pcVar2->targets[0].card = -1;
      pcVar2->targets[0].player = -1;
      pcVar2->number_of_targets = '\0';
    }
    else
    {
      if (event != EVENT_RESOLVE_TRIGGER)
      {
        if (event != EVENT_END_TRIGGER)
          goto LAB_024291ec;
        pcVar2->targets[0].card = -1;
        pcVar2->targets[0].player = -1;
        pcVar2->number_of_targets = '\0';
        goto LAB_02429378;
      }

      pcVar5 = get_card_instance(trigger_cause_controller, trigger_cause);
      iVar1 = pcVar2->damage_target_card;
      pcVar5->targets[0].player = (int)pcVar2->damage_target_player;
      pcVar5->targets[0].card = iVar1;
    }
  LAB_024291fa:
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
      goto LAB_02429210;
  }
LAB_02429378:
  if (((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
        (2 < event - EVENT_POWER)) &&
       ((event != EVENT_RESOLVE_MOVING_AURA &&
         ((event != EVENT_CHANGE_TYPE && (event != EVENT_SET_COLOR)))))) &&
      ((event != EVENT_GET_SELECTED_CARD && (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_02429210:
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = 1 - player;
  td.preferred_controller = td.allowed_controller;
  iVar3 = disabling_targeted_aura(player, card, event, &td,
                                  "Select target creature you don\'t control.", 0);
  return iVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x24285e0; original cards/gargaroz.c:247. */
int card_meddle(int player, int card, event_t event)
{
  int result;
  target_definition_t td_spell;

  if (event == EVENT_CAN_CAST)
  {
    counterspell_target_definition(player, card, &td_spell, 0xff);
    td_spell.special = td_spell.special | 0x8000;
    td_spell.extra =
        (int32_t)has_only_1_target_and_could_legally_change_target_and_targets_a_creature;
    result = deflect_one_target(player, card, EVENT_CAN_CAST, &td_spell);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      counterspell_target_definition(player, card, &td_spell, 0xff);
      td_spell.special = td_spell.special | 0x8000;
      td_spell.extra =
          (int32_t)has_only_1_target_and_could_legally_change_target_and_targets_a_creature;
      if ((card == affected_card) && (player == affected_card_controller))
      {
        set_special_flags2_2022(card_on_stack_controller, card_on_stack, 0x4000);
        deflect_one_target(player, card, EVENT_CAST_SPELL, &td_spell);
        remove_special_flags2_2022(card_on_stack_controller, card_on_stack, 0x4000);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    counterspell_target_definition(player, card, &td_spell, 0xff);
    td_spell.special = td_spell.special | 0x8000;
    td_spell.extra =
        (int32_t)has_only_1_target_and_could_legally_change_target_and_targets_a_creature;
    if (event == EVENT_RESOLVE_SPELL)
    {
      deflect_one_target(player, card, EVENT_RESOLVE_SPELL, &td_spell);
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    counterspell_target_definition(player, card, &td_spell, 0xff);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2429740; original cards/gargaroz.c:722. */
int card_spelljack(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  if (event == EVENT_RESOLVE_SPELL)
  {

    iVar1 = counterspell_validate(player, card, NULL, 0);
    if (iVar1 != 0)
    {

      instance = get_card_instance(player, card);
      iVar1 = get_id(instance->targets[0].player, instance->targets[0].card);
      iVar3 =
          pre_counterspell(player, card, instance->targets[0].player, instance->targets[0].card);
      if (iVar3 == 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
      }
      else
      {
        put_on_top_of_deck(instance->targets[0].player, instance->targets[0].card);
        rfg_top_card_of_deck(instance->targets[0].player);
      }
      if (-1 < iVar1)
      {
        create_may_play_card_from_exile_effect(player, card, instance->targets[0].player, iVar1,
                                               MPCFE_FOR_FREE);
      }
    }
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = generic_spell(player, card, event, 2, NULL, NULL, 1, NULL);
  return iVar1;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *can_legally_change_target_to_me(int who_chooses, int player, int card,
                                             int targeting_player, int targeting_card);

/* ManalinkEh 2022: 0x24281d0; original cards/gargaroz.c:142. */
int card_silver_wyvern(int player, int card, event_t event)
{
  int result;

  if (((event != EVENT_ACTIVATE) && (1 < event - EVENT_RESOLVE_ACTIVATION)) &&
      (event != EVENT_CLEANUP))
  {
    return 0;
  }
  result = deflect_away_from_me(player, card, event, 0, 0, 0, 1, 0, 0, 0, 0, NULL, 1);
  return result;
}

/* ManalinkEh 2022: 0x24276f0; original cards/gargaroz.c:415. */
static char *can_legally_change_target_to_me(int who_chooses, int player, int card,
                                             int targeting_player, int targeting_card)
{
  int32_t iVar1;
  int32_t iVar2;
  card_instance_t *instance;
  char *pcVar4;
  int iVar5;

  instance = get_card_instance(player, card);
  pcVar4 = "must have at least 1 target";
  if (instance->number_of_targets != '\0')
  {
    iVar1 = instance->targets[9].player;
    instance->targets[9].player = targeting_player;
    iVar2 = instance->targets[9].card;
    instance->targets[9].card = targeting_card;
    set_special_flags2_2022(player, card, 0x10000);
    iVar5 = call_card_function(player, card, EVENT_CAN_CAST);
    instance->targets[9].card = iVar2;
    instance->targets[9].player = iVar1;
    remove_special_flags2_2022(player, card, 0x10000);
    pcVar4 = "cannot target this creature";
    if (iVar5 != 0)
    {
      return NULL;
    }
  }
  return pcVar4;
}

/* ManalinkEh 2022: 0x2428df0; original cards/gargaroz.c:430. */
int card_mizzium_meddler(int player, int card, event_t event)
{
  int player_00;
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;
  target_definition_2022_t td_spell;

  counterspell_target_definition_2022(player, card, &td_spell, 0xff);
  td_spell.special = td_spell.special | 0x8000;
  td_spell.extra = (int32_t)can_legally_change_target_to_me;
  pcVar1 = get_card_instance(player, card);
  result = duh_mode(player);
  etb_ability_2022(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  player_00 = card_on_stack_controller;
  result = card_on_stack;
  if (event == EVENT_CAN_CAST)
  {
    result = counterspell_2022(player, card, EVENT_CAN_CAST, &td_spell, 0);
    if (result == 0)
    {
      pcVar1->info_slot = 0;
      return 1;
    }
    pcVar1->info_slot = 1;
    result = 99;
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if (((card == affected_card) && (player == affected_card_controller)) &&
          (current_turn == player))
      {
        ai_modifier = ai_modifier + -10;
      }
    }
    else if ((event == EVENT_ETB) && (pcVar1->info_slot == 1))
    {

      pcVar1->targets[0].card = card_on_stack;
      pcVar1->targets[0].player = player_00;
      pcVar1->number_of_targets = '\x01';
      set_special_flags2_2022(player_00, result, 0x10000);
      result = call_card_function(pcVar1->targets[0].player, pcVar1->targets[0].card,
                                  EVENT_CHANGE_TARGET);
      remove_special_flags2_2022(pcVar1->targets[0].player, pcVar1->targets[0].card, 0x10000);
      if (result == -1)
      {
        spell_fizzled = 1;
      }
      else
      {
        pcVar1->info_slot = result;
      }
      result = counterspell_validate_2022(player, card, &td_spell, 0);
      if (result != 0)
      {

        pcVar3 = get_card_instance(pcVar1->targets[0].player, pcVar1->targets[0].card);
        result = pcVar1->info_slot;
        pcVar3->targets[result].player = player;
        pcVar3->targets[result].card = card;
        return 0;
      }
    }
    result = 0;
  }
  return result;
}

#endif
