
#include "manalink.h"

int attach_aura_to_target(int aura_player, int aura_card, event_t event, int t_player, int t_card){
	if( ! check_for_guardian_beast_protection(t_player, t_card) ){
		if( event == EVENT_RESOLVE_SPELL ){
			set_special_flags(aura_player, aura_card, SF_CORRECTLY_RESOLVED);
		}
		card_instance_t* instance = get_card_instance(aura_player, aura_card);
		instance->damage_target_player = t_player;
		instance->damage_target_card = t_card;
		return 1;
	}
	return 0;
}

int aura_attached_to_me(int player, int card, event_t event, resolve_trigger_t trigger_mode, test_definition_t *test2){
	if( trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me( player, card ) && reason_for_trigger_controller == player &&
		!is_humiliated(player, card)
	  ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_ENCHANTMENT);
		this_test.subtype = SUBTYPE_AURA;
		if( new_make_test_in_play(trigger_cause_controller, trigger_cause, -1, &this_test) ){
			card_instance_t *aura = get_card_instance(trigger_cause_controller, trigger_cause);
			if( aura->damage_target_player == player && aura->damage_target_card == card ){
				if( test2 == NULL || (test2 != NULL && new_make_test_in_play(trigger_cause_controller, trigger_cause, -1, test2)) ){
					if(event == EVENT_TRIGGER){
						event_result |= trigger_mode;
					}
					else if(event == EVENT_RESOLVE_TRIGGER){
							return 1;
					}
				}
			}
		}
	}

	return 0;
}

int generic_aura_impl(int player, int card, event_t event, target_definition_t *td, const char* prompt, int p_plus, int t_plus, int k_plus, 
						int k_special, int subtype, int clr, int flags )
{

	card_instance_t* instance = get_card_instance(player, card);
	int csvid = cards_data[instance->internal_card_id].id;

	if( prompt == NULL ){
		prompt = "TARGET_CREATURE";
	}

	if( csvid == CARD_ID_EMBLEM_OF_THE_WARMIND || csvid == CARD_ID_DYING_WISH ||
		csvid == CARD_ID_MURDER_INVESTIGATION || csvid == CARD_ID_FAITH_UNBROKEN ||
		csvid == CARD_ID_INFERNO_FIST
	  ){
		prompt = "TARGET_CREATURE_YOU_CONTROL";
		td->allowed_controller = player;
	}
	if( csvid == CARD_ID_ROOTS){
		td->illegal_abilities |= KEYWORD_FLYING;
	}
	if( csvid == CARD_ID_WURMWEAVER_COIL){
		td->required_color = COLOR_TEST_GREEN;
	}
	if( csvid == CARD_ID_GROUNDED && player == AI ){
		td->required_abilities = KEYWORD_FLYING;
	}
	if( check_special_flags3(player, card, SF3_IN_PLAY_DIRECTLY) ){ //In case an Aura enters the battlefield directly from exile, graveyard or library.
		td->illegal_abilities = 0;
	}

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET | GS_AURA, td, NULL, 1, NULL);
	}

	if (event == EVENT_CAST_SPELL && affect_me(player, card)){
		// Don't prompt for a target if one's been forced onto the card before putting it into play
		if( check_special_flags(player, card, SF_TARGETS_ALREADY_SET) ||
			(instance->damage_target_player >= 0 && instance->damage_target_card >= 0 )
		  ){
			instance->targets[0].player = instance->damage_target_player;
			instance->targets[0].card = instance->damage_target_card;
			instance->number_of_targets = 1;
		}
		else{
			generic_spell(player, card, event, GS_CAN_TARGET | GS_AURA, td, prompt, 1, NULL);
		}
		if (player == AI && !(trace_mode & 2)){
			// Discourage the AI to enchant a creature which cannot attack or block
			if( p_plus || t_plus || k_plus || (k_special && k_special != SP_KEYWORD_CANNOT_BLOCK) ){
				card_instance_t* tgt_inst = get_card_instance(instance->targets[0].player, instance->targets[0].card);
				if( (get_special_abilities_by_instance(tgt_inst) & SP_KEYWORD_CANNOT_BLOCK) &&
					((tgt_inst->regen_status & KEYWORD_DEFENDER) && !(tgt_inst->token_status & STATUS_WALL_CAN_ATTACK)) )
				{
					ai_modifier -= 24;
				}
			}
			ai_modifier += (instance->targets[0].player == td->preferred_controller) ? 24 : -24;
		}

	}

	if( event == EVENT_RESOLVE_SPELL ){
		int good = 0;
		// Specifically avoid validating target if one's been forced onto the card before putting it into play
		if( instance->damage_target_player >= 0 && instance->damage_target_card >= 0 ){
			good = 1;
		}
		if( ! good ){
			if( valid_target(td) ){
				good = 1;
				//Dealing with the only two cases of "can't be the target of Aura spells"
				if( ! is_humiliated(instance->targets[0].player, instance->targets[0].card) ){
					int this_csvid = get_id(instance->targets[0].player, instance->targets[0].card);
					if( this_csvid == CARD_ID_TETSUO_UMEZAWA || this_csvid == CARD_ID_BARTEL_RUNEAXE ){
						good = 0;
					}
				}
			}
		}
		if( good ){
			good = attach_aura_to_target(player, card, event, instance->targets[0].player, instance->targets[0].card);
		}
		if( good ){
			remove_special_flags3(player, card, SF3_IN_PLAY_DIRECTLY);
			if( flags & GA_FORBID_NONMANA_ACTIVATED_ABILITIES ){
				disable_nonmana_activated_abilities(instance->targets[0].player, instance->targets[0].card, 1);
			}
			if( flags & GA_FORBID_HUMILIATE ){
				humiliate(player, card, instance->targets[0].player, instance->targets[0].card, 1);
			}
			if( flags & GA_FORBID_ALL_ACTIVATED_ABILITIES ){
				disable_all_activated_abilities(instance->targets[0].player, instance->targets[0].card, 1);
			}
			if( subtype > 0 ){
				if( flags & GA_FORCE_SUBTYPE ){
					force_a_subtype(instance->targets[0].player, instance->targets[0].card, subtype);
				}
				if( flags & GA_ADD_SUBTYPE ){
					add_a_subtype(instance->targets[0].player, instance->targets[0].card, subtype);
				}
			}
		}
		else{
			kill_card(player, card, KILL_BURY);
			cancel = 1;
		}
		instance->number_of_targets = 0;
	}

	if( in_play(player, card) && instance->damage_target_player > -1 ){

		if( event == EVENT_POWER && affect_me(instance->damage_target_player, instance->damage_target_card) ){
			event_result += p_plus;
		}

		if( event == EVENT_TOUGHNESS && affect_me(instance->damage_target_player, instance->damage_target_card) ){
			event_result += t_plus;
		}

		if( event == EVENT_ABILITIES ){
			if( affect_me(instance->damage_target_player, instance->damage_target_card) ){
				event_result |= k_plus;
			}
			if( affect_me(player, card) ){
				int abils = get_abilities(instance->damage_target_player, instance->damage_target_card, event, -1);
				int my_clr = get_color(player, card);
				int i;
				for(i=1; i<6; i++){
					if( (abils & (1<<(10+i))) && (my_clr & (1<<i)) ){
						kill_card(player, card, KILL_SACRIFICE);
					}
				}
			}
		}

		if( event == EVENT_SET_COLOR && affect_me(instance->damage_target_player, instance->damage_target_card) ){
			if( flags & GA_FORCE_COLOR ){
				event_result = clr;
			}
			else{
				event_result |= clr;
			}
		}

		if( k_special > 0 ){
			special_abilities(instance->damage_target_player, instance->damage_target_card, event, k_special, player, card);
		}

		if( leaves_play(player, card, event) && instance->damage_target_player != -1 ){
			if( flags & GA_FORBID_NONMANA_ACTIVATED_ABILITIES ){
				disable_nonmana_activated_abilities(instance->damage_target_player, instance->damage_target_card, 0);
			}
			if( flags & GA_FORBID_HUMILIATE ){
				humiliate(player, card, instance->damage_target_player, instance->damage_target_card, 0);
			}
			if( flags & GA_FORBID_ALL_ACTIVATED_ABILITIES ){
				disable_all_activated_abilities(instance->damage_target_player, instance->damage_target_card, 0);
			}
			if( subtype > 0 ){
				if( flags & GA_FORCE_SUBTYPE ){
					reset_subtypes(instance->damage_target_player, instance->damage_target_card, 1);
				}
				if( flags & GA_ADD_SUBTYPE ){
					reset_subtypes(instance->damage_target_player, instance->damage_target_card, 2);
				}
			}
		}

		if( event == EVENT_CAN_MOVE_AURA ){
			target_definition_t *td1;
			td1 = td;
			if( ! check_special_flags(player, card, SF_MOVE_AURA_LEGAL_TARGET) ){
				td1->illegal_abilities = 0;
			}
			if( check_special_flags(player, card, SF_MOVE_AURA_SAME_CONTROLLER) ){
				td1->allowed_controller = instance->damage_target_player;
				td1->preferred_controller = instance->damage_target_player;
			}
			if( check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) || 
				check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P1) )
			{
				td1->required_type = get_type(instance->damage_target_player, instance->damage_target_card);
			}
			if( target_available(player, card, td1) > 1 ){
				return 1;
			}
			else{
				return 0;
			}
		}

		if( event == EVENT_MOVE_AURA ){
			state_untargettable(instance->damage_target_player, instance->damage_target_card, 1);
			target_definition_t *td1;
			td1 = td;
			if( ! check_special_flags(player, card, SF_MOVE_AURA_LEGAL_TARGET) ){
				td1->illegal_abilities = 0;
			}
			if( check_special_flags(player, card, SF_MOVE_AURA_SAME_CONTROLLER) ){
				td1->allowed_controller = instance->damage_target_player;
				td1->preferred_controller = instance->damage_target_player;
			}
			if( check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) || 
				check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P1) )
			{
				td1->required_type = get_type(instance->damage_target_player, instance->damage_target_card);
				td1->who_chooses = check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) ? 0 : 1;
			}
			int result = new_pick_target(td1, prompt, 0, GS_LITERAL_PROMPT);
			state_untargettable(instance->damage_target_player, instance->damage_target_card, 0);
			return result;
		}

		if( event == EVENT_RESOLVE_MOVING_AURA ){
			target_definition_t *td1;
			td1 = td;
			if( ! check_special_flags(player, card, SF_MOVE_AURA_LEGAL_TARGET) ){
				td1->illegal_abilities = 0;
			}
			if( check_special_flags(player, card, SF_MOVE_AURA_SAME_CONTROLLER) ){
				td1->allowed_controller = instance->damage_target_player;
				td1->preferred_controller = instance->damage_target_player;
			}
			if( check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) || 
				check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P1) )
			{
				td1->required_type = get_type(instance->damage_target_player, instance->damage_target_card);
				td1->who_chooses = check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) ? 0 : 1;
			}
			if( valid_target(td1) && attach_aura_to_target(player, card, event, instance->targets[0].player, instance->targets[0].card) ){
				if( flags & GA_FORBID_NONMANA_ACTIVATED_ABILITIES ){
					disable_nonmana_activated_abilities(instance->damage_target_player, instance->damage_target_card, 0);
					disable_nonmana_activated_abilities(instance->targets[0].player, instance->targets[0].card, 1);
				}
				if( flags & GA_FORBID_HUMILIATE ){
					humiliate(player, card, instance->damage_target_player, instance->damage_target_card, 0);
					humiliate(player, card, instance->targets[0].player, instance->targets[0].card, 1);
				}
				if( flags & GA_FORBID_ALL_ACTIVATED_ABILITIES ){
					disable_all_activated_abilities(instance->damage_target_player, instance->damage_target_card, 0);
					disable_all_activated_abilities(instance->targets[0].player, instance->targets[0].card, 1);
				}
				if( subtype > 0 ){
					if( flags & GA_FORCE_SUBTYPE ){
						reset_subtypes(instance->damage_target_player, instance->damage_target_card, 1);
						force_a_subtype(instance->targets[0].player, instance->targets[0].card, subtype);
					}
					if( flags & GA_ADD_SUBTYPE ){
						reset_subtypes(instance->damage_target_player, instance->damage_target_card, 2);
						add_a_subtype(instance->targets[0].player, instance->targets[0].card, subtype);
					}
				}
			}
		}
	}
	return 0;
//	return aura_pump(player, card, event, p_plus, t_plus);
}

int generic_aura(int player, int card, event_t event, int pref_controller, int p_plus, int t_plus, int k_plus, int k_special, int subtype, int clr, int flags ){
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = pref_controller;
	return generic_aura_impl(player, card, event, &td, NULL, p_plus, t_plus, k_plus, k_special, subtype, clr, flags);
}

// An aura that does absolutely nothing itself.
static int targeted_aura_impl(int player, int card, event_t event, target_definition_t* td, const char* prompt, int custom_prompt, int disabling_mode){

	card_instance_t *instance = get_card_instance(player, card);

	if (event == EVENT_CAN_CAST){
		//In case an Aura enters the battlefield directly from exile, graveyard or library.
		if( check_special_flags3(player, card, SF3_IN_PLAY_DIRECTLY) ){
			td->illegal_abilities = 0;
		}
		return generic_spell(player, card, event, GS_CAN_TARGET, td, NULL, 1, NULL);
	}

	if (event == EVENT_CAST_SPELL && affect_me(player, card)){
		//In case an Aura enters the battlefield directly from exile, graveyard or library.
		if( check_special_flags3(player, card, SF3_IN_PLAY_DIRECTLY) ){
			td->illegal_abilities = 0;
		}
		// Don't prompt for a target if one's been forced onto the card before putting it into play
		if (instance->damage_target_player >= 0 && instance->damage_target_card >= 0){
			instance->targets[0].player = instance->damage_target_player;
			instance->targets[0].card = instance->damage_target_card;
			instance->number_of_targets = 1;
		}
		else{
			if( custom_prompt ){
				generic_spell(player, card, event, GS_CAN_TARGET | GS_AURA | GS_LITERAL_PROMPT, td, prompt, 1, NULL);
			}
			else{
				generic_spell(player, card, event, GS_CAN_TARGET | GS_AURA, td, prompt, 1, NULL);
			}
			if (player == AI && !(trace_mode & 2)){
				ai_modifier += (instance->targets[0].player == td->preferred_controller) ? 24 : -24;
			}
		}
	}

	if (event == EVENT_RESOLVE_SPELL){
		int good = 0;
		// Specifically avoid validating target if one's been forced onto the card before putting it into play
		if( instance->damage_target_player >= 0 && instance->damage_target_card >= 0 ){
			good = 1;
		}
		if( ! good ){
			if( valid_target(td) ){
				good = 1;
				//Dealing with the only two cases of "can't be the target of Aura spells"
				if( ! is_humiliated(instance->targets[0].player, instance->targets[0].card) ){
					int this_csvid = get_id(instance->targets[0].player, instance->targets[0].card);
					if( this_csvid == CARD_ID_TETSUO_UMEZAWA || this_csvid == CARD_ID_BARTEL_RUNEAXE ){
						good = 0;
					}
				}
			}
		}
		if( good ){
			good = attach_aura_to_target(player, card, event, instance->targets[0].player, instance->targets[0].card);
		}
		if( good ){
			remove_special_flags3(player, card, SF3_IN_PLAY_DIRECTLY);
			if( disabling_mode == 1 ){
				disable_nonmana_activated_abilities(instance->targets[0].player, instance->targets[0].card, 1);
			}
			if( disabling_mode == 2 ){
				disable_all_activated_abilities(instance->targets[0].player, instance->targets[0].card, 1);
			}
			if( disabling_mode == 3 ){
				humiliate(player, card, instance->targets[0].player, instance->targets[0].card, 1);
			}
		}
		else {
			kill_card(player, card, KILL_BURY);
			cancel = 1;
		}
		instance->number_of_targets = 0;
	}

	if (event == EVENT_GET_SELECTED_CARD){
		*(int*)(0x78FA30) = instance->damage_target_card | (instance->damage_target_player << 8);
	}

	if (in_play(player, card) && instance->targets[0].player != -1 && instance->targets[0].card != -1){
			if( (leaves_play(player, card, event) && instance->targets[0].player != -1) ||
				(instance->damage_target_player != instance->targets[2].player && instance->damage_target_card != instance->targets[2].card)
			  ){
				instance->targets[2].player = instance->damage_target_player;
				instance->targets[2].card = instance->damage_target_card;
			}
			if( event == EVENT_CAN_MOVE_AURA ){
				target_definition_t *td1;
				td1 = td;
				if( ! check_special_flags(player, card, SF_MOVE_AURA_LEGAL_TARGET) ){
					td1->illegal_abilities = 0;
				}
				if( check_special_flags(player, card, SF_MOVE_AURA_SAME_CONTROLLER) ){
					td1->allowed_controller = instance->damage_target_player;
					td1->preferred_controller = instance->damage_target_player;
				}
				if( check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) || 
					check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P1) )
				{
					td1->required_type = get_type(instance->damage_target_player, instance->damage_target_card);
				}
				if( target_available(player, card, td1) > 1 ){
					return 1;
				}
				else{
					return 0;
				}
			}
			if( event == EVENT_MOVE_AURA ){
				target_definition_t *td1;
				td1 = td;
				if( ! check_special_flags(player, card, SF_MOVE_AURA_LEGAL_TARGET) ){
					td1->illegal_abilities = 0;
				}
				if( check_special_flags(player, card, SF_MOVE_AURA_SAME_CONTROLLER) ){
					td1->allowed_controller = instance->damage_target_player;
					td1->preferred_controller = instance->damage_target_player;
				}
				if( check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) || 
					check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P1) )
				{
					td1->required_type = get_type(instance->damage_target_player, instance->damage_target_card);
					td1->who_chooses = check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) ? 0 : 1;
				}
				if( pick_target(td1, "TARGET_CREATURE") ){
					return 1;
				}
				else{
					return 0;
				}
			}
			if( event == EVENT_RESOLVE_MOVING_AURA ){
				target_definition_t *td1;
				td1 = td;
				if( ! check_special_flags(player, card, SF_MOVE_AURA_LEGAL_TARGET) ){
					td1->illegal_abilities = 0;
				}
				if( check_special_flags(player, card, SF_MOVE_AURA_SAME_CONTROLLER) ){
					td1->allowed_controller = instance->damage_target_player;
					td1->preferred_controller = instance->damage_target_player;
				}
				if( check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) || 
					check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P1) )
				{
					td1->required_type = get_type(instance->damage_target_player, instance->damage_target_card);
					td1->who_chooses = check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) ? 0 : 1;
				}
				if( valid_target(td1) ){
					if( disabling_mode == 1 ){
						disable_nonmana_activated_abilities(instance->damage_target_player, instance->damage_target_card, 0);
					}
					if( disabling_mode == 2 ){
						disable_all_activated_abilities(instance->damage_target_player, instance->damage_target_card, 0);
					}
					if( disabling_mode == 3 ){
						humiliate(player, card, instance->damage_target_player, instance->damage_target_card, 0);
					}
					if( attach_aura_to_target(player, card, event, instance->targets[0].player, instance->targets[0].card) ){
						if( disabling_mode == 1 ){
							disable_nonmana_activated_abilities(instance->targets[0].player, instance->targets[0].card, 1);
						}
						if( disabling_mode == 2 ){
							disable_all_activated_abilities(instance->targets[0].player, instance->targets[0].card, 1);
						}
						if( disabling_mode == 3 ){
							humiliate(player, card, instance->targets[0].player, instance->targets[0].card, 1);
						}
					}
				}
			}
			if( event == EVENT_ABILITIES ){
				if( affect_me(player, card) ){
					int abils = get_abilities(instance->damage_target_player, instance->damage_target_card, event, -1);
					int my_clr = get_color(player, card);
					int i;
					for(i=1; i<6; i++){
						if( (abils & (1<<(10+i))) && (my_clr & (1<<i)) ){
							kill_card(player, card, KILL_SACRIFICE);
						}
					}
				}
			}
	}

	return 0;
}

int targeted_aura(int player, int card, event_t event, target_definition_t* td, const char* prompt){
	return targeted_aura_impl(player, card, event, td, prompt, 0, 0);
}

int targeted_aura_custom_prompt(int player, int card, event_t event, target_definition_t* td, const char* prompt){
	return targeted_aura_impl(player, card, event, td, prompt, 1, 0);
}

int disabling_targeted_aura(int player, int card, event_t event, target_definition_t* td, const char* prompt, int mode){

	int rval = targeted_aura_impl(player, card, event, td, prompt, 1, mode);

	if (event == EVENT_CAST_SPELL && affect_me(player, card) && cancel != 1)	// after vanilla_aura()
	{
		card_instance_t* instance = get_card_instance(player, card);
		card_instance_t* tgt_inst = get_card_instance(instance->targets[0].player, instance->targets[0].card);

		if ((tgt_inst->regen_status & KEYWORD_DEFENDER) && !(tgt_inst->token_status & STATUS_WALL_CAN_ATTACK))
			ai_modifier -= 48;

		if (instance->damage_target_player < 0 && instance->damage_target_card < 0
			&& another_copy_attached(player, card, get_card_instance(player, card)->internal_card_id))
			ai_modifier -= 96;
	}

	if (event == EVENT_SHOULD_AI_PLAY)
	{
		card_instance_t* instance = get_card_instance(player, card);
		if (instance->damage_target_player == HUMAN)
			ai_modifier += get_card_instance(instance->damage_target_player, instance->damage_target_card)->attack_rating / 2;
	}

	return rval;
}

// An aura with enchant creature that does absolutely nothing itself.
int vanilla_aura(int player, int card, event_t event, int pref_controller){
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = pref_controller;

	return targeted_aura(player, card, event, &td, "TARGET_CREATURE");
}

// A frontend to vanilla_aura() with AI settings for Pacifism-like auras.
int disabling_aura(int player, int card, int event)
{
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	return disabling_targeted_aura(player, card, event, &td, "Select target creature.", 0);
}

int aura_with_variable_pt_boost_depending_on_condition(int player, int card, event_t event, target_definition_t *td, test_definition_t *this_test, int pow, int tou, int pow2, int tou2){

	card_instance_t *instance = get_card_instance(player, card);

	if( in_play(player, card) && instance->damage_target_player > -1 ){
		int p = instance->damage_target_player;
		int c = instance->damage_target_card;

		if( event == EVENT_POWER && affect_me(p, c) ){
			int plus = new_make_test_in_play(p, c, -1, this_test) ? pow : pow2;
			event_result+=plus;
		}

		if( event == EVENT_TOUGHNESS && affect_me(p, c) ){
			int plus = new_make_test_in_play(p, c, -1, this_test) ? tou : tou2;
			event_result+=plus;
		}
	}
	return targeted_aura(player, card, event, td, "TARGET_CREATURE");
}

int count_auras_enchanting_me(int player, int card){
	card_instance_t* aura;
	int result = 0;
	int i, count;
	for (i = 0; i < 2; i++){
		for (count = 0; count < active_cards_count[i]; ++count){
			if( (aura = in_play(i, count)) && is_what(i, count, TYPE_ENCHANTMENT) && has_subtype(i, count, SUBTYPE_AURA) &&
				aura->damage_target_player == player && aura->damage_target_card == card
			  ){
				result++;
			}
		}
	}
	return result;
}

static int is_enchanted_impl(int player, int card, int subtype){
	card_instance_t* aura;
	int i, count;
	for (i = 0; i < 2; i++){
		for (count = 0; count < active_cards_count[i]; ++count){
			if( (aura = in_play(i, count)) && is_what(i, count, TYPE_ENCHANTMENT) && has_subtype(i, count, SUBTYPE_AURA) &&
				aura->damage_target_player == player && aura->damage_target_card == card
			  ){
				int result = 1;
				if( subtype > 0 && ! has_subtype(i, count, subtype) ){
					result = 0;
				}
				if( result ){
					return 1;
				}
			}
		}
	}
	return 0;
}

int is_enchanted(int player, int card){
	return is_enchanted_impl(player, card, 0);
}

void put_into_play_aura_attached_to_target(int aura_player, int aura_card, int t_player, int t_card){
	// aura_card have to be in hand
	set_special_flags(aura_player, aura_card, SF_TARGETS_ALREADY_SET);
	card_instance_t *aura = get_card_instance(aura_player, aura_card);
	aura->targets[0].player = t_player;
	aura->targets[0].card = t_card;
	aura->number_of_targets = 1;
	aura->damage_target_player = t_player;
	aura->damage_target_card = t_card;
	put_into_play(aura_player, aura_card);
}

/* Uses targets[9] to store the player/card it was attached to at the time it was activated.  Should be called *before* the EVENT_RESOLVE_ACTIVATION handler if
 * td is provided. */
int aura_granting_activated_ability(int player, int card, event_t event, int preferred_controller, int mode,
									int cless, int black, int blue, int green, int red, int white,
									uint32_t variable_costs, target_definition_t *td, const char *prompt)
{
  int rval = attachment_granting_activated_ability(player, card, event, mode, cless, black, blue, green, red, white, variable_costs, td, prompt);
  return rval ? rval : vanilla_aura(player, card, event, preferred_controller);
}

int gain_control_and_attach_as_aura(int player, int card, event_t event, int t_player, int t_card){

	int flag = player != t_player ? 1 : 0;
	if( check_special_flags3(t_player, t_card, SF3_IS_STEALING_AURA) ){
		card_instance_t *instance = get_card_instance(t_player, t_card);
		flag = player != instance->damage_target_player ? 1 : 0;
		int result = gain_control_permanently(player, instance->damage_target_player, instance->damage_target_card);
		if( result > -1 ){
			if( event == EVENT_RESOLVE_SPELL ){
				set_special_flags3(player, card, SF3_IS_STEALING_AURA);
			}
			if( flag ){
				add_state(player, card, STATE_POWER_STRUGGLE);
			}
			else{
				if( check_state(player, card, STATE_POWER_STRUGGLE) ){
					remove_state(player, card, STATE_POWER_STRUGGLE);
				}
			}
			result = gain_control_permanently_no_sound(player, t_player, t_card);
			return attach_aura_to_target(player, card, event, player, result);
		}
	}
	else{
		int result = gain_control_permanently(player, t_player, t_card);
		if( result > -1 ){
			if( event == EVENT_RESOLVE_SPELL ){
				set_special_flags3(player, card, SF3_IS_STEALING_AURA);
			}
			if( flag ){
				add_state(player, card, STATE_POWER_STRUGGLE);
			}
			else{
				if( check_state(player, card, STATE_POWER_STRUGGLE) ){
					remove_state(player, card, STATE_POWER_STRUGGLE);
				}
			}
			return attach_aura_to_target(player, card, event, player, result);
		}
	}

	return 0;
}

static int is_enchanted_by_another_stealing_aura(int player, int card, int orig_aura_player, int orig_aura_card){
	card_instance_t* aura;
	int i, count;
	for( i = 0; i < 2; i++ ){
		for( count = 0; count < active_cards_count[i]; ++count ){
			if( (aura = in_play(i, count)) && is_what(i, count, TYPE_ENCHANTMENT) && has_subtype(i, count, SUBTYPE_AURA) ){
				if( !( i == orig_aura_player && count == orig_aura_card ) ){
					if( aura->damage_target_player == player && aura->damage_target_card == card &&
						check_special_flags3(i, count, SF3_IS_STEALING_AURA)
					  ){
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

void end_of_stealing_aura_effect(int player, int card){
// Usually called when aura leaves play, but not only (See Frenzied Fugue in Commander 2016)
	card_instance_t *instance = get_card_instance(player, card);
	if( check_state(player, card, STATE_POWER_STRUGGLE) && in_play(instance->damage_target_player, instance->damage_target_card)){
		if( ! check_special_flags3(instance->damage_target_player, instance->damage_target_card, SF3_IS_STEALING_AURA) ){
			if( ! is_enchanted_by_another_stealing_aura(instance->damage_target_player, instance->damage_target_card, player, card) ){
				re_enable_the_most_recent_control_effect_attached_to_me(instance->damage_target_player, instance->damage_target_card);
			}
			gain_control_permanently_no_sound(1-instance->damage_target_player, instance->damage_target_player, instance->damage_target_card);
		}
		else{
			card_instance_t *aura = get_card_instance(instance->damage_target_player, instance->damage_target_card);
			gain_control_permanently_no_sound(1-aura->damage_target_player, aura->damage_target_player, aura->damage_target_card);
			gain_control_permanently_no_sound(1-instance->damage_target_player, instance->damage_target_player, instance->damage_target_card);
		}
	}
}

int generic_stealing_aura(int player, int card, event_t event, target_definition_t *td, const char *prompt){

	if( player < 0 || card < 0 ){
		return 0;
	}

	if(event == EVENT_RESOLVE_SPELL){
		card_instance_t *instance = get_card_instance(player, card);
		if( valid_target(td) ){
			gain_control_and_attach_as_aura(player, card, event, instance->targets[0].player, instance->targets[0].card);
		}
		else{
			kill_card(player, card, KILL_SACRIFICE);
		}
	}

	if( event == EVENT_CAN_MOVE_AURA || event == EVENT_MOVE_AURA ){
		return targeted_aura(player, card, event, td, prompt);
	}

	if( event == EVENT_RESOLVE_MOVING_AURA ){
		if( valid_target(td) ){
			card_instance_t *instance = get_card_instance(player, card);
			gain_control_and_attach_as_aura(player, card, event, instance->targets[0].player, instance->targets[0].card);
		}
	}

	if( leaves_play(player, card, event) ){
		end_of_stealing_aura_effect(player, card);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET | GS_AURA, td, prompt, 1, NULL);
}

static int generic_animating_aura_impl(int player, int card, event_t event, target_definition_t *td, const char *prompt, int pow, int tou, int key, int s_key, int custom_prompt, subtype_t added_subtype){

	if( get_card_instance(player, card)->damage_target_player > -1 && in_play(player, card) && ! is_humiliated(player, card) ){
		card_instance_t *instance = get_card_instance(player, card);
		int p = instance->damage_target_player;
		int c = instance->damage_target_card;

		if( affect_me(p, c) ){
			switch( event ){
					case EVENT_ABILITIES:
					{
						event_result |= key;
						special_abilities(p, c, event, s_key, player, card);
						break;
					}
					case EVENT_POWER:
					{
						if( pow > -1 ){
							event_result += (pow - get_base_power(p, c));
						}
						break;
					}
					case EVENT_TOUGHNESS:
					{
						if( tou > -1 ){
							event_result += (tou - get_base_toughness(p, c));
						}
						break;
					}
					case EVENT_CHANGE_TYPE:
					{
						if( ! is_what(-1, event_result, TYPE_CREATURE) ){
							if( instance->dummy3 < 1 ){
								int newtype = create_a_card_type(event_result);
								cards_at_7c7000[newtype]->type |= (cards_data[event_result].type | TYPE_CREATURE);
								cards_at_7c7000[newtype]->power = (pow == -1 ? get_cmc(p, c) : pow);
								cards_at_7c7000[newtype]->toughness = (tou == -1 ? get_cmc(p, c) : tou);
								instance->dummy3 = newtype;
								add_status(p, c, STATUS_ANIMATED);
							}
							event_result = instance->dummy3;
						}
						else{
							remove_status(p, c, STATUS_ANIMATED);
						}
						break;
					}
					case EVENT_RESOLVE_MOVING_AURA:
					{
						target_definition_t *td1;
						td1 = td;
						if( ! check_special_flags(player, card, SF_MOVE_AURA_LEGAL_TARGET) ){
							td1->illegal_abilities = 0;
						}
						if( check_special_flags(player, card, SF_MOVE_AURA_SAME_CONTROLLER) ){
							td1->allowed_controller = instance->damage_target_player;
							td1->preferred_controller = instance->damage_target_player;
						}
						if( check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) || 
							check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P1) )
						{
							td1->required_type = get_type(instance->damage_target_player, instance->damage_target_card);
							td1->who_chooses = check_special_flags3(player, card, SF3_ENCHANTMENT_ALTERATION_P0) ? 0 : 1;
						}
						if( valid_target(td1) ){
							int prev_p = p;
							int prev_c = c;
							if( attach_aura_to_target(player, card, event, instance->targets[0].player, instance->targets[0].card) ){
								instance->dummy3 = 0;
								if( added_subtype ){
									reset_subtypes(prev_p, prev_c, RS_RESET_ADDED);
									add_a_subtype(instance->targets[0].player, instance->targets[0].card, added_subtype);
								}
							}
						}
						return 0; //Or it will fall through the default "targeted_aura"
					}
					default:
						break;
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(td) ){
			card_instance_t *instance = get_card_instance(player, card);
			if( attach_aura_to_target(player, card, event, instance->targets[0].player, instance->targets[0].card) ){
				if( added_subtype > 0 ){
					add_a_subtype(instance->targets[0].player, instance->targets[0].card, added_subtype);
				}
			}
			else{
				kill_card(player, card, KILL_SACRIFICE);
			}
		}
		else{
			kill_card(player, card, KILL_SACRIFICE);
		}
		return 0;
	}

	if( custom_prompt ){
		return targeted_aura_custom_prompt(player, card, event, td, prompt);
	}
	return targeted_aura(player, card, event, td, prompt);
}

int generic_animating_aura(int player, int card, event_t event, target_definition_t *td, const char *prompt, int pow, int tou, int key, int s_key){
	return generic_animating_aura_impl(player, card, event, td, prompt, pow, tou, key, s_key, 0, 0);
}

int generic_animating_aura_custom_prompt(int player, int card, event_t event, target_definition_t *td, const char *prompt, int pow, int tou, int key, int s_key){
	return generic_animating_aura_impl(player, card, event, td, prompt, pow, tou, key, s_key, 1, 0);
}

int generic_animating_aura_with_added_subtype(int player, int card, event_t event, target_definition_t *td, const char *prompt, int pow, int tou, int key, int s_key, subtype_t subtype){
	return generic_animating_aura_impl(player, card, event, td, prompt, pow, tou, key, s_key, 1, subtype);
}

static int when_enchanted_permanent_dies_return_aura_to_hand_legacy(int player, int card, event_t event){

	card_instance_t *instance = get_card_instance(player, card);

	if( resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY) ){
		seek_grave_for_id_to_reanimate(player, card, instance->targets[0].player, cards_data[instance->targets[0].card].id, REANIMATE_RETURN_TO_HAND);
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

void when_enchanted_permanent_dies_return_aura_to_hand(int player, int card, event_t event){

	card_instance_t *instance = get_card_instance(player, card);

	if( in_play(player, card) && instance->damage_target_player != - 1 ){
		int t_player = instance->damage_target_player;
		int t_card = instance->damage_target_card;

		if( event == EVENT_GRAVEYARD_FROM_PLAY ){
			if( affect_me(t_player, t_card) && in_play(t_player, t_card) ){
				card_instance_t *land = get_card_instance(t_player, t_card);
				if( land->kill_code > 0 && land->kill_code < KILL_EXILE ){
					set_special_flags3(player, card, SF3_ENCHANTED_PERMANENT_DYING);
				}
			}
			if( affect_me(player, card) && in_play(player, card) ){
				if( instance->kill_code > 0 && instance->kill_code < KILL_EXILE ){
					if( check_special_flags3(player, card, SF3_ENCHANTED_PERMANENT_DYING) ){
						int legacy = create_legacy_effect(player, card, &when_enchanted_permanent_dies_return_aura_to_hand_legacy);
						get_card_instance(player, legacy)->targets[0].player = get_owner(player, card);
						get_card_instance(player, legacy)->targets[0].card = get_original_internal_card_id(player, card);
						get_card_instance(player, legacy)->targets[11].player = 66;
					}
				}
			}
		}
	}
}

void cannot_be_enchanted_granted(int granting_player, int granting_card, event_t event, int t_player, int t_card){

	if( event == EVENT_STATIC_EFFECTS ){
		card_instance_t* aura;
		int i, count;
		for (i = 0; i < 2; i++){
			for(count = active_cards_count[i]-1; count >-1 ; count--){
				if( (aura = in_play(i, count)) && is_what(i, count, TYPE_ENCHANTMENT) && has_subtype(i, count, SUBTYPE_AURA) &&
					aura->damage_target_player == t_player && aura->damage_target_card == t_card
				  ){
					if( granting_player == -1 || !(i== granting_player && count == granting_card) ){
						kill_card(i, count, KILL_SACRIFICE);
					}
				}
			}
		}
	}
}

void cannot_be_enchanted(int player, int card, event_t event){
	cannot_be_enchanted_granted(-1, -1, event, player, card);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void attached_gets_pt(int player, int card, event_t event, int power, int toughness);

static void attached_gets_pt_part_2022(int player, int card, event_t event, int power,
                                       int toughness);

/* ManalinkEh 2022: 0x26e7870; original functions/auras.c:41. */
void attached_gets_pt(int player, int card, event_t event, int power, int toughness)
{
  if (1 < event - EVENT_POWER)
  {
    return;
  }
  attached_gets_pt_part_2022(player, card, event, power, toughness);
  return;
}

static void attached_set_pt_part_2022(int player, int card, event_t event, int power, int toughness);

/* ManalinkEh 2022: 0x26e78b0; original functions/auras.c:62. */
void attached_set_pt(int player, int card, event_t event, int power, int toughness)
{
  if (1 < event - EVENT_POWER)
  {
    return;
  }
  attached_set_pt_part_2022(player, card, event, power, toughness);
  return;
}

static void disabling_aura_ai_part_2022(int player, int card, event_t event);

/* ManalinkEh 2022: 0x26e8710; original functions/auras.c:528. */
void disabling_aura_ai(int player, int card, event_t event)
{
  if ((player == 1) && (((unsigned char)trace_mode & 2) == 0))
  {
    disabling_aura_ai_part_2022(1, card, event);
    return;
  }
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void attached_is_additional_subtype(int player, int card, event_t event, subtype_t subtype);
void attached_is_forced_subtype(int player, int card, event_t event, subtype_t subtype);
void attached_is_additional_color(int player, int card, event_t event, color_test_t color);

/* ManalinkEh 2022: 0x26e7ad0; original functions/auras.c:129. */
void disable_attached(int player, int card, event_t event, int disabling_mode)
{
  char cVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;

  if (event != EVENT_ABILITIES)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar3 = has_subtype(player, card, SUBTYPE_AURA);
      if (iVar3 == 0)
      {
        return;
      }
    }
    else if (event != EVENT_EQUIPMENT_ATTACHED)
    {
      return;
    }
  }

  instance = in_play(player, card);
  if (((instance != NULL) && (-1 < instance->damage_target_card)) &&
      (iVar3 = is_humiliated_for_ability(player, card, AL_STATIC), iVar3 == 0))
  {
    cVar1 = instance->damage_target_player;
    iVar3 = instance->damage_target_card;
    if (event == EVENT_ABILITIES)
    {
      if ((iVar3 == affected_card) && (cVar1 == affected_card_controller))
        goto LAB_026e7b82;
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      iVar4 = has_subtype(player, card, SUBTYPE_AURA);
      if (iVar4 != 0)
        goto LAB_026e7b82;
    }
    else if (event == EVENT_EQUIPMENT_ATTACHED)
    {
    LAB_026e7b82:
      remove_abilities((int)cVar1, iVar3, event, disabling_mode);
      return;
    }
  }
  return;
}

/* ManalinkEh 2022: 0x26e7bf0; original functions/auras.c:144. */
void attached_is_additional_subtype(int player, int card, event_t event, subtype_t subtype)
{
  card_instance_t *instance;
  int result;
  int id;
  int player_00;

  if (event != EVENT_CHANGE_TYPE)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = has_subtype(player, card, SUBTYPE_AURA);
      if (result == 0)
      {
        return;
      }
    }
    else if (event != EVENT_EQUIPMENT_ATTACHED)
    {
      return;
    }
  }

  instance = in_play(player, card);
  if ((((instance != NULL) && (-1 < instance->damage_target_card)) &&
       (result = is_humiliated_for_ability(player, card, AL_STATIC), subtype != SUBTYPE_INVALID)) &&
      (result == 0))
  {
    player_00 = (int)instance->damage_target_player;
    result = instance->damage_target_card;
    add_a_subtype_2022(player_00, result, subtype);
    if (subtype == SUBTYPE_LEGEND)
    {
      id = get_card_name(player_00, result);
      verify_legend_rule(player_00, result, id);
      return;
    }
  }
  return;
}

/* ManalinkEh 2022: 0x26e7cc0; original functions/auras.c:161. */
void attached_is_forced_subtype(int player, int card, event_t event, subtype_t subtype)
{
  card_instance_t *instance;
  int result;
  int id;
  int player_00;

  if (event != EVENT_CHANGE_TYPE)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = has_subtype(player, card, SUBTYPE_AURA);
      if (result == 0)
      {
        return;
      }
    }
    else if (event != EVENT_EQUIPMENT_ATTACHED)
    {
      return;
    }
  }

  instance = in_play(player, card);
  if ((((instance != NULL) && (-1 < instance->damage_target_card)) &&
       (result = is_humiliated_for_ability(player, card, AL_STATIC), subtype != SUBTYPE_INVALID)) &&
      (result == 0))
  {
    player_00 = (int)instance->damage_target_player;
    result = instance->damage_target_card;
    force_a_subtype_2022(player_00, result, subtype);
    if (subtype == SUBTYPE_LEGEND)
    {
      id = get_card_name(player_00, result);
      verify_legend_rule(player_00, result, id);
      return;
    }
  }
  return;
}

/* ManalinkEh 2022: 0x26e7d90; original functions/auras.c:178. */
void attached_is_additional_color(int player, int card, event_t event, color_test_t color)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;
  unsigned int uVar4;

  if (event != EVENT_SET_COLOR)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = has_subtype(player, card, SUBTYPE_AURA);
      if (result == 0)
      {
        return;
      }
    }
    else if (event != EVENT_EQUIPMENT_ATTACHED)
    {
      return;
    }
  }

  pcVar1 = in_play(player, card);
  if ((((pcVar1 != NULL) && (-1 < pcVar1->damage_target_card)) &&
       (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
      ((pcVar3 = in_play(player, card), pcVar3 != NULL && (-1 < pcVar1->damage_target_card))))
  {

    if (event == EVENT_SET_COLOR)
    {
      if (pcVar1->damage_target_card != affected_card)
      {
        return;
      }
      if (pcVar1->damage_target_player != affected_card_controller)
      {
        return;
      }
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      result = has_subtype(player, card, SUBTYPE_AURA);
      if (result == 0)
      {
        return;
      }
    }
    else if (event != EVENT_EQUIPMENT_ATTACHED)
    {
      return;
    }
    uVar4 = get_sleighted_color_test(player, card, color);
    event_result = event_result | uVar4;
  }
  return;
}

/* ManalinkEh 2022: 0x26e7eb0; original functions/auras.c:194. */
void attached_is_forced_color(int player, int card, event_t event, color_test_t color)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;

  if (event != EVENT_SET_COLOR)
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = has_subtype(player, card, SUBTYPE_AURA);
      if (result == 0)
      {
        return;
      }
    }
    else if (event != EVENT_EQUIPMENT_ATTACHED)
    {
      return;
    }
  }

  pcVar1 = in_play(player, card);
  if ((((pcVar1 != NULL) && (-1 < pcVar1->damage_target_card)) &&
       (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
      ((pcVar3 = in_play(player, card), pcVar3 != NULL && (-1 < pcVar1->damage_target_card))))
  {

    if (event == EVENT_SET_COLOR)
    {
      if (pcVar1->damage_target_card != affected_card)
      {
        return;
      }
      if (pcVar1->damage_target_player != affected_card_controller)
      {
        return;
      }
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      result = has_subtype(player, card, SUBTYPE_AURA);
      if (result == 0)
      {
        return;
      }
    }
    else if (event != EVENT_EQUIPMENT_ATTACHED)
    {
      return;
    }
    event_result = get_sleighted_color_test(player, card, color);
  }
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void attached_gets_kw(int player, int card, event_t event, keyword_t keyword,
                      sp_keyword_t sp_keyword);

/* ManalinkEh 2022: 0x26e78f0; original functions/auras.c:82. */
void attached_gets_kw(int player, int card, event_t event, keyword_t keyword,
                      sp_keyword_t sp_keyword)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_ABILITIES)
  {

    instance = in_play(player, card);
    if (instance == NULL)
    {
      return;
    }
    if (instance->damage_target_card < 0)
    {
      return;
    }
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result != 0)
    {
      return;
    }
    if ((instance->damage_target_card == affected_card) &&
        (instance->damage_target_player == affected_card_controller))
    {
      get_sleighted_hacked_suppressed_ability(player, card, (int)instance->damage_target_player,
                                              &keyword, &sp_keyword);
      event_result = event_result | keyword;
    }
  }

  if ((((sp_keyword != SP_KEYWORD_0) && (instance = in_play(player, card), instance != NULL)) &&
       (-1 < instance->damage_target_card)) &&
      (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
  {
    special_abilities_2022((int)instance->damage_target_player, instance->damage_target_card, event,
                      sp_keyword, player, card);
  }
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void attached_gets_sp_kw2(int player, int card, event_t event, sp_keyword2_t sp_keyword2);

/* ManalinkEh 2022: 0x26e7a30; original functions/auras.c:109. */
void attached_gets_sp_kw2(int player, int card, event_t event, sp_keyword2_t sp_keyword2)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_ABILITIES)
  {

    instance = in_play(player, card);
    if ((instance != NULL) && (-1 < instance->damage_target_card))
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);

      if ((sp_keyword2 != SP_KEYWORD2_0) && ((result == 0 && (-1 < instance->damage_target_card))))
      {
        result = is_humiliated_for_ability(player, card, AL_STATIC);
        if (result == 0)
        {
          special_abilities2((int)instance->damage_target_player, instance->damage_target_card,
                             EVENT_ABILITIES, sp_keyword2);
          return;
        }
      }
    }
  }
  return;
}


/* Recovered October 2022 Aura compiler parts. */

/* ManalinkEh 2022: 0x26e65a0; original functions/auras.c:41. */
static void attached_gets_pt_part_2022(int player, int card, event_t event, int power,
                                       int toughness)
{
  card_instance_t *instance;
  int result;

  instance = in_play(player, card);
  if ((instance != NULL) && (-1 < instance->damage_target_card))
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      if (event == EVENT_POWER)
      {
        if ((instance->damage_target_card == affected_card) &&
            (instance->damage_target_player == affected_card_controller))
        {
          event_result = event_result + power;
        }
      }
      else if (((event == EVENT_TOUGHNESS) && (instance->damage_target_card == affected_card)) &&
               (instance->damage_target_player == affected_card_controller))
      {
        event_result = event_result + toughness;
      }
    }
  }
  return;
}


/* ManalinkEh 2022: 0x26e6650; original functions/auras.c:62. */
static void attached_set_pt_part_2022(int player, int card, event_t event, int power, int toughness)
{
  card_instance_t *instance;
  int result;

  instance = in_play(player, card);
  if (((instance != NULL) && (-1 < instance->damage_target_card)) &&
      (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
  {
    if (event == EVENT_POWER)
    {
      if (((instance->damage_target_card == affected_card) &&
           (affected_card_controller == instance->damage_target_player)) &&
          (-1 < power))
        goto LAB_026e66df;
    }
    else if (((event == EVENT_TOUGHNESS) && (instance->damage_target_card == affected_card)) &&
             ((affected_card_controller == instance->damage_target_player && (-1 < toughness))))
    {
      power = toughness;
    LAB_026e66df:
      set_base_pt(power);
      return;
    }
  }
  return;
}


/* ManalinkEh 2022: 0x26e6710; original functions/auras.c:528. */
static void disabling_aura_ai_part_2022(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  if (event == EVENT_CAST_SPELL)
  {
    if (((affected_card == card) && (affected_card_controller == player)) && (spell_fizzled != 1))
    {
      pcVar1 = get_card_instance(player, card);
      pcVar2 = get_card_instance(pcVar1->targets[0].player, pcVar1->targets[0].card);
      ai_modifier =
          ((pcVar1->targets[0].player == 1 - player) - 1 & 0xffffffd0) + 0x18 + ai_modifier;
      if (((pcVar2->regen_status & 0x800000) != 0) && ((pcVar2->token_status & 0x800) == 0))
      {
        ai_modifier = ai_modifier + -0x30;
      }
      if ((pcVar1->damage_target_player < '\0') && (pcVar1->damage_target_card < 0))
      {
        pcVar1 = get_card_instance(player, card);
        result = another_copy_attached(player, card, pcVar1->internal_card_id);
        if (result != 0)
        {
          ai_modifier = ai_modifier + -0x60;
        }
      }
    }
  }
  else if (event == EVENT_SHOULD_AI_PLAY)
  {
    pcVar1 = get_card_instance(player, card);
    if (pcVar1->damage_target_player == '\0')
    {
      pcVar1 = get_card_instance(0, pcVar1->damage_target_card);
      ai_modifier = ai_modifier + pcVar1->attack_rating / 2;
    }
  }
  return;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int aura_impl(int player, int card, event_t event, target_definition_2022_t *td,
                     char *prompt, int literal_prompt);
int aura(int player, int card, event_t event, target_definition_2022_t *td, char *prompt);
int aura_pref_controller(int player, int card, event_t event, int pref_controller, char *prompt);

/* ManalinkEh 2022: 0x26e68d0; original functions/auras.c:331. */
static int aura_impl(int player, int card, event_t event, target_definition_2022_t *td,
                     char *prompt, int literal_prompt)
{
  char cVar1;
  card_instance_t *pcVar2;
  int iVar3;
  type_t tVar4;
  int iVar5;
  card_instance_t *pcVar6;
  target_definition_2022_t td_default;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_026e6cbc;
    }
    else if ((event != EVENT_RESOLVE_SPELL) &&
             ((event != EVENT_CAN_CHANGE_TARGET && (event != EVENT_CHANGE_TARGET))))
    {
    LAB_026e6cbc:
      if (((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
          (((((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
              (event != EVENT_CHANGE_TYPE)) &&
             ((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)))) &&
            (event != EVENT_STATIC_EFFECTS))))
      {
        return 0;
      }
    }
  }
  pcVar2 = get_card_instance(player, card);
  if (td == NULL)
  {
    default_target_definition_2022(player, card, &td_default, 2);
    td = &td_default;
  }
  if (prompt == NULL && literal_prompt == 0)
  {
    prompt = "TARGET_CREATURE";
  }
  iVar3 = check_special_flags3_2022(player, card, 0x40);
  if (iVar3 != 0)
  {
    td->illegal_abilities = td->illegal_abilities & 0xfffdffff;
  }
  if ((event == EVENT_CAN_CAST || event == EVENT_CHANGE_TARGET) ||
      (event == EVENT_CAN_CHANGE_TARGET))
  {
    iVar3 = generic_spell_2022(player, card, event, 0x201, td, prompt, 1, NULL);
    return iVar3;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar3 = check_special_flags_2022(player, card, 0x20000000);
      if (iVar3 == 0)
      {
        if ((-1 < pcVar2->damage_target_player) && (iVar3 = pcVar2->damage_target_card, -1 < iVar3))
          goto LAB_026e6de2;
        generic_spell_2022(player, card, EVENT_CAST_SPELL,
                           (-(unsigned int)(literal_prompt == 0) & 0xffffff80) + 0x281, td, prompt,
                           1, NULL);
      }
      else
      {
        iVar3 = pcVar2->damage_target_card;
      LAB_026e6de2:
        pcVar2->targets[0].player = (int)pcVar2->damage_target_player;
        pcVar2->targets[0].card = iVar3;
        pcVar2->number_of_targets = '\x01';
        remove_special_flags_2022(player, card, 0x20000000);
      }
      if ((player == 1) && (((unsigned char)trace_mode & 2) == 0))
      {
        ai_modifier = ai_modifier +
                      ((pcVar2->targets[0].player == td->preferred_controller) - 1 & 0xffffffd0) +
                      0x18;
      }
    }
    pcVar6 = in_play(player, card);
    if (pcVar6 == NULL)
    {
      return 0;
    }
  LAB_026e6a20:
    cVar1 = pcVar2->damage_target_player;
    if (cVar1 < '\0')
      goto LAB_026e6b88;
  }
  else
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      pcVar6 = in_play(player, card);
      if (pcVar6 == NULL)
        goto LAB_026e6b88;
      goto LAB_026e6a20;
    }

    if ((((-1 < pcVar2->damage_target_player) && (-1 < pcVar2->damage_target_card)) ||
         ((iVar3 = valid_target_2022(td),
           iVar3 != 0 &&
               ((iVar3 = is_humiliated_for_ability(pcVar2->targets[0].player,
                                                   pcVar2->targets[0].card, AL_STATIC),
                 iVar3 != 0 || ((iVar3 = get_id(pcVar2->targets[0].player, pcVar2->targets[0].card),
                                 iVar3 != 0x32b && (iVar3 != 0x242)))))))) &&
        (iVar3 = attach_aura_to_target_2022(player, card, EVENT_RESOLVE_SPELL, pcVar2->targets[0].player,
                                       pcVar2->targets[0].card),
         iVar3 != 0))
    {
      remove_special_flags3_2022(player, card, 0x40);
      pcVar2->number_of_targets = '\0';
      return 1;
    }
    kill_card(player, card, KILL_STATE_BASED_ACTION);
    pcVar2->number_of_targets = '\0';
    spell_fizzled = 1;
    pcVar6 = in_play(player, card);
    if (pcVar6 == NULL)
    {
      return 0;
    }
    cVar1 = pcVar2->damage_target_player;
    if (cVar1 < '\0')
    {
      return 0;
    }
  }
  iVar3 = pcVar2->damage_target_card;
  if (-1 < iVar3)
  {
    if (event == EVENT_STATIC_EFFECTS)
    {
      if (card != affected_card)
      {
        return 0;
      }
      if (player != affected_card_controller)
      {
        return 0;
      }

      td->preferred_controller = td->allowed_controller;
      td->flags = td->flags | 3;
      iVar3 = get_id((int)cVar1, iVar3);
      iVar5 = validate_arbitrary_target_2022(td, (int)pcVar2->damage_target_player,
                                             pcVar2->damage_target_card);
      if (iVar5 != 0)
      {
        iVar5 = is_humiliated_for_ability((int)pcVar2->damage_target_player,
                                          pcVar2->damage_target_card, AL_STATIC);
        if (iVar5 != 0)
        {
          return 0;
        }
        if ((iVar3 != 0x32b) && (iVar3 != 0x242))
        {
          return 0;
        }
      }
      kill_card(player, card, KILL_STATE_BASED_ACTION);
      return 0;
    }
    if (event == EVENT_CAN_MOVE_AURA)
    {

      td->preferred_controller = 2;
      iVar3 = check_special_flags_2022(player, card, SF_MOVE_AURA_LEGAL_TARGET);
      if (iVar3 == 0)
      {
        td->illegal_abilities = 0;
      }
      iVar3 = check_special_flags_2022(player, card, 0x1000000);
      if (iVar3 != 0)
      {
        cVar1 = pcVar2->damage_target_player;
        td->allowed_controller = (int)cVar1;
        td->preferred_controller = (int)cVar1;
      }
      iVar3 = check_special_flags3_2022(player, card, 0x8000);
      if ((iVar3 != 0) || (iVar3 = check_special_flags3_2022(player, card, 0x10000), iVar3 != 0))
      {
        tVar4 = get_type((int)pcVar2->damage_target_player, pcVar2->damage_target_card);
        td->required_type = tVar4;
      }
      iVar3 = target_available_2022(player, card, td);
      return (unsigned int)(1 < iVar3);
    }
    if (event == EVENT_MOVE_AURA)
    {

      state_untargettable((int)cVar1, iVar3, 1);
      td->preferred_controller = 2;
      iVar3 = check_special_flags_2022(player, card, SF_MOVE_AURA_LEGAL_TARGET);
      if (iVar3 == 0)
      {
        td->illegal_abilities = 0;
      }
      iVar3 = check_special_flags_2022(player, card, 0x1000000);
      if (iVar3 != 0)
      {
        cVar1 = pcVar2->damage_target_player;
        td->allowed_controller = (int)cVar1;
        td->preferred_controller = (int)cVar1;
      }
      iVar3 = check_special_flags3_2022(player, card, 0x8000);
      if ((iVar3 != 0) || (iVar3 = check_special_flags3_2022(player, card, 0x10000), iVar3 != 0))
      {
        tVar4 = get_type((int)pcVar2->damage_target_player, pcVar2->damage_target_card);
        td->required_type = tVar4;
        iVar3 = check_special_flags3_2022(player, card, 0x8000);
        td->who_chooses = (unsigned int)(iVar3 == 0);
      }
      iVar3 = check_special_flags_2022(player, card, 0x20000000);
      if (iVar3 == 0)
      {
        if (literal_prompt != 0)
        {
          literal_prompt = 0x80;
        }
        iVar3 = new_pick_target_2022(td, prompt, 0, literal_prompt);
      }
      else
      {
        iVar3 = validate_target_2022(player, card, td, 0);
      }
      state_untargettable((int)pcVar2->damage_target_player, pcVar2->damage_target_card, 0);
      return iVar3;
    }
    if (event == EVENT_RESOLVE_MOVING_AURA)
    {

      iVar3 = check_special_flags_2022(player, card, SF_MOVE_AURA_LEGAL_TARGET);
      if (iVar3 == 0)
      {
        td->illegal_abilities = 0;
      }
      iVar3 = check_special_flags_2022(player, card, 0x1000000);
      if (iVar3 != 0)
      {
        cVar1 = pcVar2->damage_target_player;
        td->allowed_controller = (int)cVar1;
        td->preferred_controller = (int)cVar1;
      }
      iVar3 = check_special_flags3_2022(player, card, 0x8000);
      if ((iVar3 != 0) || (iVar3 = check_special_flags3_2022(player, card, 0x10000), iVar3 != 0))
      {
        tVar4 = get_type((int)pcVar2->damage_target_player, pcVar2->damage_target_card);
        td->required_type = tVar4;
        iVar3 = check_special_flags3_2022(player, card, 0x8000);
        td->who_chooses = (unsigned int)(iVar3 == 0);
      }
      iVar3 = valid_target_2022(td);
      if (iVar3 == 0)
      {
        return 0;
      }
      iVar3 = attach_aura_to_target_2022(player, card, EVENT_RESOLVE_MOVING_AURA,
                                    pcVar2->targets[0].player, pcVar2->targets[0].card);
      if (iVar3 == 0)
      {
        return 0;
      }
      return 1;
    }
  }
LAB_026e6b88:
  if (event != EVENT_GET_SELECTED_CARD)
  {
    return 0;
  }
  engine_selected_card = (int)pcVar2->damage_target_player << 8 | pcVar2->damage_target_card;
  return 0;
}

/* ManalinkEh 2022: 0x26e84a0; original functions/auras.c:505. */
int aura(int player, int card, event_t event, target_definition_2022_t *td, char *prompt)
{
  int result;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_026e84cc;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_026e84cc;
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
           (event != EVENT_CHANGE_TYPE)))) &&
        (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
          (event != EVENT_STATIC_EFFECTS))))
    {
      return 0;
    }
  }
LAB_026e84cc:
  result = aura_impl(player, card, event, td, prompt, 0);
  return result;
}

/* ManalinkEh 2022: 0x26e8560; original functions/auras.c:511. */
int aura_literal_prompt(int player, int card, event_t event, target_definition_2022_t *td,
                        char *prompt)
{
  int result;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_026e858c;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_026e858c;
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
           (event != EVENT_CHANGE_TYPE)))) &&
        (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
          (event != EVENT_STATIC_EFFECTS))))
    {
      return 0;
    }
  }
LAB_026e858c:
  result = aura_impl(player, card, event, td, prompt, 1);
  return result;
}

/* ManalinkEh 2022: 0x26e8620; original functions/auras.c:517. */
int aura_pref_controller(int player, int card, event_t event, int pref_controller, char *prompt)
{
  int result;
  target_definition_2022_t td;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_026e8650;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_026e8650;
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
           (event != EVENT_CHANGE_TYPE)))) &&
        (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
          (event != EVENT_STATIC_EFFECTS))))
    {
      return 0;
    }
  }
LAB_026e8650:
  default_target_definition_2022(player, card, &td, 2);
  td.preferred_controller = pref_controller;
  result = aura_impl(player, card, event, &td, prompt, 0);
  return result;
}
#endif


#ifndef SHANDALAR
/* Recovered October 2022 Aura targeting and attachment helpers. */
static int targeted_aura_impl_2022(int player, int card, event_t event,
                                   target_definition_2022_t *td, char *prompt, int custom_prompt,
                                   int flags);
static int when_enchanted_permanent_dies_return_aura_to_hand_legacy_2022(int player, int card,
                                                                         event_t event);

/* ManalinkEh 2022: 0x26e6850; original functions/auras.c:3. */
int attach_aura_to_target_2022(int aura_player, int aura_card, event_t event, int t_player,
                               int t_card)
{
  int result;
  card_instance_t *instance;
  result = check_for_guardian_beast_protection(t_player, t_card);
  if (result != 0)
  {
    return 0;
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    set_special_flags_2022(aura_player, aura_card, 0x2000000);
  }
  instance = get_card_instance(aura_player, aura_card);
  instance->damage_target_player = (int8_t)t_player;
  instance->damage_target_card = t_card;
  return 1;
}


/* ManalinkEh 2022: 0x26e7160; original functions/auras.c:587. */
static int targeted_aura_impl_2022(int player, int card, event_t event,
                                   target_definition_2022_t *td, char *prompt, int custom_prompt,
                                   int flags)
{
  char cVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int result;
  ability_loss_t aStack_20;

  pcVar2 = get_card_instance(player, card);
  pcVar3 = in_play(player, card);
  if (pcVar3 != NULL)
  {
    cVar1 = pcVar2->damage_target_player;
    if ((((event == EVENT_ABILITIES) && (-1 < cVar1)) &&
         (pcVar2->damage_target_card == affected_card)) &&
        (cVar1 == affected_card_controller))
    {
      aStack_20 = -(flags & 1U) & AL_NM_ACT;
      if ((flags & 2U) != 0)
      {
        aStack_20 = AL_ALL;
      }
      if ((flags & 4U) != 0)
      {
        aStack_20 = aStack_20 | AL_ALL_ACT;
      }
      remove_abilities((int)cVar1, pcVar2->damage_target_card, EVENT_ABILITIES, aStack_20);
    }
  }
  result = aura_impl(player, card, event, td, prompt, custom_prompt);
  return result;
}


/* ManalinkEh 2022: 0x26e7730; original functions/auras.c:16. */
int aura_attached_to_me_2022(int player, int card, event_t event, resolve_trigger_t trigger_mode,
                             test_definition_2022_t *test2)
{
  int result;
  card_instance_t *instance;
  test_definition_2022_t this_test;
  if ((((trigger_condition != 0xdb) || (card != affected_card)) ||
       (player != affected_card_controller)) ||
      ((reason_for_trigger_controller != player ||
        (result = is_humiliated_for_ability(player, card, AL_TRIG), result != 0))))
  {
    return 0;
  }
  default_test_definition_2022(&this_test, 4);
  this_test.subtype = 0x4000;
  result = new_make_test_in_play_2022(trigger_cause_controller, trigger_cause, -1, &this_test);
  if (result == 0)
  {
    return 0;
  }
  instance = get_card_instance(trigger_cause_controller, trigger_cause);
  if (instance->damage_target_player != player)
  {
    return 0;
  }
  if (instance->damage_target_card != card)
  {
    return 0;
  }
  if ((test2 != NULL) &&
      (result = new_make_test_in_play_2022(trigger_cause_controller, trigger_cause, -1, test2),
       result == 0))
  {
    return 0;
  }
  if (event == EVENT_TRIGGER)
  {
    event_result = event_result | trigger_mode;
    return 0;
  }
  if (event != EVENT_RESOLVE_TRIGGER)
  {
    return 0;
  }
  return 1;
}


/* ManalinkEh 2022: 0x26e8860; original functions/auras.c:610. */
int targeted_aura_2022(int player, int card, event_t event, target_definition_2022_t *td,
                       char *prompt)
{
  int result;
  result = targeted_aura_impl_2022(player, card, event, td, prompt, 0, 0);
  return result;
}


/* ManalinkEh 2022: 0x26e8890; original functions/auras.c:614. */
int targeted_aura_custom_prompt_2022(int player, int card, event_t event,
                                     target_definition_2022_t *td, char *prompt)
{
  int result;
  result = targeted_aura_impl_2022(player, card, event, td, prompt, 1, 0);
  return result;
}


/* ManalinkEh 2022: 0x26e88c0; original functions/auras.c:618. */
int disabling_targeted_aura_2022(int player, int card, event_t event, target_definition_2022_t *td,
                                 char *prompt, int mode)
{
  int result;

  result = targeted_aura_impl_2022(player, card, event, td, prompt, 1, mode);
  if ((player == 1) && (((unsigned char)trace_mode & 2) == 0))
  {
    disabling_aura_ai_part_2022(1, card, event);
  }
  return result;
}


/* ManalinkEh 2022: 0x26e8920; original functions/auras.c:628. */
int vanilla_aura_2022(int player, int card, event_t event, int pref_controller)
{
  int result;
  target_definition_2022_t td;
  default_target_definition_2022(player, card, &td, 2);
  td.preferred_controller = pref_controller;
  result = aura(player, card, event, &td, "TARGET_CREATURE");
  return result;
}


/* ManalinkEh 2022: 0x26e8980; original functions/auras.c:637. */
int disabling_aura_2022(int player, int card, int event)
{
  int result;
  target_definition_2022_t td;
  default_target_definition_2022(player, card, &td, 2);
  result = disabling_targeted_aura_2022(player, card, event, &td, "Select target creature.", 0);
  return result;
}


/* ManalinkEh 2022: 0x26e89e0; original functions/auras.c:644. */
int aura_with_variable_pt_boost_depending_on_condition_2022(int player, int card, event_t event,
                                                            target_definition_2022_t *td,
                                                            test_definition_2022_t *this_test,
                                                            int pow, int tou, int pow2, int tou2)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  int player_00;
  int iStack_24;
  int iStack_20;

  iStack_20 = pow;
  iStack_24 = tou;
  pcVar1 = get_card_instance(player, card);
  pcVar2 = in_play(player, card);
  if ((pcVar2 != NULL) && (-1 < pcVar1->damage_target_player))
  {
    player_00 = (int)pcVar1->damage_target_player;
    result = pcVar1->damage_target_card;
    if (event == EVENT_POWER)
    {
      if ((result == affected_card) && (player_00 == affected_card_controller))
      {
        result = new_make_test_in_play_2022(player_00, result, -1, this_test);
        if (result == 0)
        {
          iStack_20 = pow2;
        }
        event_result = event_result + iStack_20;
      }
    }
    else if (((event == EVENT_TOUGHNESS) && (result == affected_card)) &&
             (player_00 == affected_card_controller))
    {
      result = new_make_test_in_play_2022(player_00, result, -1, this_test);
      if (result == 0)
      {
        iStack_24 = tou2;
      }
      event_result = event_result + iStack_24;
    }
  }
  result = targeted_aura_impl_2022(player, card, event, td, "TARGET_CREATURE", 0, 0);
  return result;
}


/* ManalinkEh 2022: 0x26e8c90; original functions/auras.c:707. */
void put_into_play_aura_attached_to_target_2022(int aura_player, int aura_card, int t_player,
                                                int t_card)
{
  card_instance_t *instance;

  set_special_flags_2022(aura_player, aura_card, 0x20000000);
  instance = get_card_instance(aura_player, aura_card);
  instance->targets[0].player = t_player;
  instance->targets[0].card = t_card;
  instance->number_of_targets = '\x01';
  instance->damage_target_player = (int8_t)t_player;
  instance->damage_target_card = t_card;
  put_into_play(aura_player, aura_card);
  return;
}


/* ManalinkEh 2022: 0x26e8d90; original functions/auras.c:729. */
int gain_control_and_attach_as_aura_2022(int player, int card, event_t event, int t_player,
                                         int t_card)
{
  char cVar1;
  int result;
  card_instance_t *instance;

  result = check_special_flags3_2022(t_player, t_card, 0x100);
  if (result == 0)
  {
    t_card = gain_control_permanently(player, t_player, t_card);
    if (-1 < t_card)
    {
      if (event == EVENT_RESOLVE_SPELL)
      {
        set_special_flags3_2022(player, card, 0x100);
      }
      if (player == t_player)
      {
        result = check_state(player, card, 0x400000);
        if (result != 0)
        {
          remove_state(player, card, 0x400000);
        }
      }
      else
      {
        add_state(player, card, 0x400000);
      }
      goto LAB_026e8e42;
    }
  }
  else
  {
    instance = get_card_instance(t_player, t_card);
    cVar1 = instance->damage_target_player;
    result = gain_control_permanently(player, (int)cVar1, instance->damage_target_card);
    if (-1 < result)
    {
      if (event == EVENT_RESOLVE_SPELL)
      {
        set_special_flags3_2022(player, card, 0x100);
      }
      if (cVar1 == player)
      {
        result = check_state(player, card, 0x400000);
        if (result != 0)
        {
          remove_state(player, card, 0x400000);
        }
      }
      else
      {
        add_state(player, card, 0x400000);
      }
      t_card = gain_control_permanently_no_sound(player, t_player, t_card);
    LAB_026e8e42:
      result = attach_aura_to_target_2022(player, card, event, player, t_card);
      return result;
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x26e8f40; original functions/auras.c:792. */
void end_of_stealing_aura_effect_2022(int player, int card)
{
  char cVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int iVar6;
  int card_00;

  pcVar2 = get_card_instance(player, card);
  iVar3 = check_state(player, card, 0x400000);
  if ((iVar3 != 0) &&
      (pcVar4 = in_play((int)pcVar2->damage_target_player, pcVar2->damage_target_card),
       pcVar4 != NULL))
  {
    iVar5 = check_special_flags3_2022((int)pcVar2->damage_target_player, pcVar2->damage_target_card,
                                      0x100);
    iVar3 = pcVar2->damage_target_card;
    if (iVar5 == 0)
    {
      cVar1 = pcVar2->damage_target_player;
      iVar5 = 0;
      do
      {
        card_00 = 0;
        if (0 < active_cards_count[iVar5])
        {
          do
          {
            pcVar4 = in_play(iVar5, card_00);
            if (((((pcVar4 != NULL) && (iVar6 = is_what(iVar5, card_00, 4), iVar6 != 0)) &&
                  (iVar6 = has_subtype(iVar5, card_00, SUBTYPE_AURA), iVar6 != 0)) &&
                 ((player != iVar5 || (card != card_00)))) &&
                ((cVar1 == pcVar4->damage_target_player &&
                  ((iVar3 == pcVar4->damage_target_card &&
                    (iVar6 = check_special_flags3_2022(iVar5, card_00, 0x100), iVar6 != 0))))))
              goto LAB_026e9090;
            card_00 = card_00 + 1;
          } while (card_00 < active_cards_count[iVar5]);
        }
        iVar5 = iVar5 + 1;
      } while (iVar5 != 2);
      re_enable_the_most_recent_control_effect_attached_to_me((int)pcVar2->damage_target_player,
                                                              pcVar2->damage_target_card);
    LAB_026e9090:
      gain_control_permanently_no_sound(1 - pcVar2->damage_target_player,
                                        (int)pcVar2->damage_target_player,
                                        pcVar2->damage_target_card);
    }
    else
    {
      pcVar4 = get_card_instance((int)pcVar2->damage_target_player, iVar3);
      gain_control_permanently_no_sound(1 - pcVar4->damage_target_player,
                                        (int)pcVar4->damage_target_player,
                                        pcVar4->damage_target_card);
      gain_control_permanently_no_sound(1 - pcVar2->damage_target_player,
                                        (int)pcVar2->damage_target_player,
                                        pcVar2->damage_target_card);
    }
  }
  return;
}


/* ManalinkEh 2022: 0x26e9110; original functions/auras.c:810. */
int generic_stealing_aura_2022(int player, int card, event_t event, target_definition_2022_t *td,
                               char *prompt)
{
  int result;
  card_instance_t *instance;
  if ((-1 < player) && (-1 < card))
  {
    if (event == EVENT_RESOLVE_SPELL)
    {
      instance = get_card_instance(player, card);
      result = valid_target_2022(td);
      if (result == 0)
      {
        kill_card(player, card, KILL_SACRIFICE);
      }
      else
      {
        gain_control_and_attach_as_aura_2022(player, card, EVENT_RESOLVE_SPELL,
                                             instance->targets[0].player,
                                             instance->targets[0].card);
      }
      leaves_play_2022(player, card, EVENT_RESOLVE_SPELL, 0, 0);
    }
    else
    {
      if ((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CAN_MOVE_AURA)
      {
        result = targeted_aura_impl_2022(player, card, event, td, prompt, 0, 0);
        return result;
      }
      if (event == EVENT_RESOLVE_MOVING_AURA)
      {
        result = valid_target_2022(td);
        if (result != 0)
        {
          end_of_stealing_aura_effect_2022(player, card);
          instance = get_card_instance(player, card);
          gain_control_and_attach_as_aura_2022(player, card, EVENT_RESOLVE_MOVING_AURA,
                                               instance->targets[0].player,
                                               instance->targets[0].card);
        }
        leaves_play_2022(player, card, EVENT_RESOLVE_MOVING_AURA, 0, 0);
      }
      else
      {
        leaves_play_2022(player, card, event, 0, 0);
        if (event == EVENT_LEAVES_PLAY_ABILITY)
        {
          result = check_special_flags_2022(player, card, 0x2000000);
          if (result != 0)
          {
            end_of_stealing_aura_effect_2022(player, card);
          }
        }
      }
    }
    result = generic_spell_2022(player, card, event, 0x201, td, prompt, 1, NULL);
    return result;
  }
  return 0;
}


/* ManalinkEh 2022: 0x26e6500; original functions/auras.c:962. */
static int when_enchanted_permanent_dies_return_aura_to_hand_legacy_2022(int player, int card,
                                                                         event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  seek_grave_for_id_to_reanimate(player, card, instance->targets[0].player,
                                 (unsigned int)cards_data[instance->targets[0].card].id,
                                 REANIMATE_RETURN_TO_HAND);
  kill_card(player, card, KILL_EXILE);
  return 0;
}


/* ManalinkEh 2022: 0x26e94c0; original functions/auras.c:974. */
void when_enchanted_permanent_dies_return_aura_to_hand_2022(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;

  pcVar1 = get_card_instance(player, card);
  pcVar2 = in_play(player, card);
  iVar3 = affected_card;
  if (((pcVar2 != NULL) && (pcVar1->damage_target_player != -1)) &&
      (event == EVENT_GRAVEYARD_FROM_PLAY))
  {
    if (((pcVar1->damage_target_card == affected_card) &&
         (iVar4 = (int)pcVar1->damage_target_player, iVar4 == affected_card_controller)) &&
        ((pcVar2 = in_play(iVar4, affected_card),
          pcVar2 != NULL && (pcVar2 = get_card_instance(iVar4, iVar3),
                             (unsigned char)(pcVar2->kill_code - 1) < 3))))
    {
      set_special_flags3_2022(player, card, 4);
    }
    if (((card == affected_card) && (player == affected_card_controller)) &&
        ((pcVar2 = in_play(player, card),
          pcVar2 != NULL && (((unsigned char)(pcVar1->kill_code - 1) < 3 &&
                              (iVar3 = check_special_flags3_2022(player, card, 4), iVar3 != 0))))))
    {
      iVar3 = create_legacy_effect(player, card,
                                   when_enchanted_permanent_dies_return_aura_to_hand_legacy_2022);
      pcVar1 = get_card_instance(player, iVar3);
      iVar4 = get_owner(player, card);
      pcVar1->targets[0].player = iVar4;
      pcVar1 = get_card_instance(player, iVar3);
      iVar4 = get_original_internal_card_id(player, card);
      pcVar1->targets[0].card = iVar4;
      pcVar1 = get_card_instance(player, iVar3);
      pcVar1->targets[0xb].player = 0x42;
    }
  }
  return;
}


/* ManalinkEh 2022: 0x26e9630; original functions/auras.c:1003. */
void cannot_be_enchanted_granted_2022(int granting_player, int granting_card, event_t event,
                                      int t_player, int t_card)
{
  char cVar1;
  int result;
  if (granting_player == -1)
  {
    misc_protection(t_player, t_card, event);
  }
  else
  {
    special_abilities2(t_player, t_card, event, SP_KEYWORD2_GRANTED_PROT_MISC);
  }
  if (((((event == EVENT_PROT_MISC) && (t_card == affected_card)) &&
        (t_player == affected_card_controller)) &&
       ((cVar1 = (char)((unsigned int)event_result >> 0x18),
         cVar1 == '\x02' ||
             ((cVar1 == '\x05' &&
               (result = has_subtype(attacking_card_controller, attacking_card, SUBTYPE_AURA),
                result != 0)))))) &&
      ((granting_player == -1 ||
        ((granting_player != attacking_card_controller || (granting_card != attacking_card))))))
  {
    event_result = event_result | 1;
  }
  return;
}


/* ManalinkEh 2022: 0x26e9710; original functions/auras.c:1014. */
void cannot_be_enchanted_2022(int player, int card, event_t event)
{
  cannot_be_enchanted_granted_2022(-1, -1, event, player, card);
  return;
}


/* ManalinkEh 2022: 0x26e9750; original functions/auras.c:1018. */
int enchant_player_2022(int player, int card, event_t event, target_definition_2022_t *td)
{
  int32_t iVar1;
  card_instance_t *instance;
  int iVar3;

  instance = get_card_instance(player, card);
  td->flags = td->flags | 2;
  if (event == EVENT_SHOULD_AI_PLAY)
  {
    iVar3 = should_ai_play(player, card);
    return iVar3;
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        iVar3 = check_special_flags_2022(player, card, 0x20000000);
        if (iVar3 == 0)
        {
          load_text_2022(NULL, "TARGET_PLAYER");
          iVar3 = select_target_2022(td->player, td->card, td, text_lines[0], NULL);
          if (iVar3 == 0)
          {
            spell_fizzled = 1;
          }
        }
      }
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      iVar3 = valid_target_2022(td);
      if (iVar3 != 0)
      {
        iVar1 = instance->targets[0].card;
        instance->targets[4].player = instance->targets[0].player;
        instance->damage_target_player = (int8_t)instance->targets[0].player;
        instance->targets[4].card = iVar1;
        return 0;
      }
      kill_card(player, card, KILL_SACRIFICE);
    }
    return 0;
  }
  iVar3 = can_target_2022(td);
  return iVar3;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 ability writers and Aura pump consumers. */
static int generic_animating_aura_impl_2022(int player, int card, event_t event,
                                            target_definition_2022_t *td, char *prompt, int pow,
                                            int tou, int key, int s_key, int custom_prompt,
                                            subtype_t added_subtype);

/* ManalinkEh 2022: 0x26e7250; original functions/auras.c:847. */
static int generic_animating_aura_impl_2022(int player, int card, event_t event,
                                            target_definition_2022_t *td, char *prompt, int pow,
                                            int tou, int key, int s_key, int custom_prompt,
                                            subtype_t added_subtype)
{
  char cVar1;
  uint8_t uVar2;
  card_instance_t *instance;
  int iVar4;
  int player_00;
  type_t tVar5;
  int iVar6;
  instance = get_card_instance(player, card);
  if (((-1 < instance->damage_target_player) &&
       (instance = in_play(player, card), instance != NULL)) &&
      (iVar4 = is_humiliated_for_ability(player, card, AL_STATIC), iVar4 == 0))
  {
    instance = get_card_instance(player, card);
    iVar4 = instance->damage_target_card;
    if ((iVar4 == affected_card) &&
        (player_00 = (int)instance->damage_target_player, player_00 == affected_card_controller))
    {
      if (event == EVENT_ABILITIES)
      {
        event_result = event_result | key;
        special_abilities_2022(player_00, iVar4, EVENT_ABILITIES, s_key, player, card);
        goto LAB_026e72ae;
      }
      if (event < EVENT_RECALC_DAMAGE)
      {
        if (((event == EVENT_POWER) || (pow = tou, event == EVENT_TOUGHNESS)) && (0 < pow))
        {
          set_base_pt(pow);
        }
        goto LAB_026e72ae;
      }
      if (event == EVENT_CHANGE_TYPE)
      {
        iVar6 = is_what(-1, event_result, 2);
        if (iVar6 == 0)
        {
          if (instance->dummy3 < 1)
          {
            iVar6 = create_a_card_type(event_result);
            uVar2 = cards_at_7c7000[iVar6]->type;
            cards_at_7c7000[iVar6]->type = cards_data[event_result].type | uVar2 | 2;
            if (pow != 0)
            {
              if (pow == -1)
              {
                pow = get_cmc(player_00, iVar4);
              }
              cards_at_7c7000[iVar6]->power = (int16_t)pow;
            }
            if (tou != 0)
            {
              if (tou == -1)
              {
                tou = get_cmc(player_00, iVar4);
              }
              cards_at_7c7000[iVar6]->toughness = (int16_t)tou;
            }
            instance->dummy3 = iVar6;
            add_status(player_00, iVar4, 0x40);
          }
          event_result = instance->dummy3;
        }
        else
        {
          remove_status(player_00, iVar4, 0x40);
        }
        if (added_subtype != SUBTYPE_INVALID)
        {
          add_a_subtype_2022(instance->targets[0].player, instance->targets[0].card, added_subtype);
        }
        goto LAB_026e72ae;
      }
      if (event == EVENT_RESOLVE_MOVING_AURA)
      {
        iVar4 = check_special_flags_2022(player, card, SF_MOVE_AURA_LEGAL_TARGET);
        if (iVar4 == 0)
        {
          td->illegal_abilities = 0;
        }
        iVar4 = check_special_flags_2022(player, card, 0x1000000);
        if (iVar4 != 0)
        {
          cVar1 = instance->damage_target_player;
          td->allowed_controller = (int)cVar1;
          td->preferred_controller = (int)cVar1;
        }
        iVar4 = check_special_flags3_2022(player, card, 0x8000);
        if ((iVar4 != 0) || (iVar4 = check_special_flags3_2022(player, card, 0x10000), iVar4 != 0))
        {
          tVar5 = get_type((int)instance->damage_target_player, instance->damage_target_card);
          td->required_type = tVar5;
          iVar4 = check_special_flags3_2022(player, card, 0x8000);
          td->who_chooses = (unsigned int)(iVar4 == 0);
        }
        iVar4 = valid_target_2022(td);
        if (iVar4 == 0)
        {
          return 0;
        }
        iVar4 = attach_aura_to_target_2022(player, card, EVENT_RESOLVE_MOVING_AURA,
                                           instance->targets[0].player, instance->targets[0].card);
        if (iVar4 == 0)
        {
          return 0;
        }
        instance->dummy3 = 0;
        return 0;
      }
    }
  }
  if (event == EVENT_RESOLVE_SPELL)
  {
    iVar4 = valid_target_2022(td);
    if (iVar4 != 0)
    {
      instance = get_card_instance(player, card);
      iVar4 = attach_aura_to_target_2022(player, card, EVENT_RESOLVE_SPELL,
                                         instance->targets[0].player, instance->targets[0].card);
      if (iVar4 != 0)
      {
        return 0;
      }
    }
    kill_card(player, card, KILL_STATE_BASED_ACTION);
    return 0;
  }
LAB_026e72ae:
  pow = (int)(custom_prompt != 0);
  iVar4 = targeted_aura_impl_2022(player, card, event, td, prompt, pow, 0);
  return iVar4;
}


/* ManalinkEh 2022: 0x26e8180; original functions/auras.c:255. */
void pump_attached_2022(int player, int card, event_t event, pump_ability_2022_t *ability_mod)
{
  int power;
  color_test_t color;
  subtype_t subtype;
  card_instance_t *pcVar1;
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  pump_ability_2022_t default_ability_mod;

  pcVar1 = in_play(player, card);
  if ((((event == EVENT_RESOLVE_SPELL) &&
        (result = has_subtype(player, card, SUBTYPE_AURA), player == 1)) &&
       (result != 0)) &&
      ((((unsigned char)trace_mode & 2) == 0 &&
        (((0 < ability_mod->pow_mod || 0 < ability_mod->tou_mod ||
           (ability_mod->key_mod != KEYWORD_0)) ||
          ((ability_mod->skey_mod & ~SP_KEYWORD_CANNOT_BLOCK) != SP_KEYWORD_0))))))
  {
    instance = get_card_instance(pcVar1->targets[0].player, pcVar1->targets[0].card);
    uVar3 = get_special_abilities_by_instance_2022(instance);
    if ((((uVar3 & 0x20) != 0) && ((instance->regen_status & 0x800000) != 0)) &&
        ((instance->token_status & 0x800) == 0))
    {
      ai_modifier = ai_modifier + -0x18;
    }
  }
  else if (pcVar1 == NULL)
  {
    return;
  }
  if (((-1 < pcVar1->damage_target_card) &&
       (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0)) &&
      (result = is_what((int)pcVar1->damage_target_player, pcVar1->damage_target_card, 2),
       result != 0))
  {
    if (ability_mod == NULL)
    {
      ability_mod = &default_ability_mod;
      default_pump_ability_definition_2022(player, card, ability_mod, 0, 0, 0, 0);
    }
    result = ability_mod->tou_mod;
    power = ability_mod->pow_mod;
    if (result != 0 || power != 0)
    {
      if ((ability_mod->paue_flags & 0x100000) == 0)
      {
        if ((uint32_t)event - (uint32_t)EVENT_POWER < 2U)
        {
          attached_gets_pt_part_2022(player, card, event, power, result);
        }
      }
      else if ((uint32_t)event - (uint32_t)EVENT_POWER < 2U)
      {
        attached_set_pt_part_2022(player, card, event, power, result);
      }
    }
    if (ability_mod->skey_mod != SP_KEYWORD_0 || ability_mod->key_mod != KEYWORD_0)
    {
      attached_gets_kw(player, card, event, ability_mod->key_mod, ability_mod->skey_mod);
    }
    if (ability_mod->disabling_mode != 0)
    {
      disable_attached(player, card, event, ability_mod->disabling_mode);
    }
    color = ability_mod->color;
    if (color != COLOR_TEST_0)
    {
      if ((ability_mod->paue_flags & 0x20000) == 0)
      {
        attached_is_additional_color(player, card, event, color);
      }
      else
      {
        attached_is_forced_color(player, card, event, color);
      }
    }
    subtype = ability_mod->subtype;
    if (subtype != SUBTYPE_INVALID)
    {
      if ((ability_mod->paue_flags & 0x40000U) == 0)
      {
        if ((ability_mod->paue_flags & 0x80000U) != 0)
        {
          attached_is_additional_subtype(player, card, event, subtype);
        }
      }
      else
      {
        attached_is_forced_subtype(player, card, event, subtype);
      }
    }
    if (ability_mod->can_block_additional_creatures != 0)
    {
      event_flags = event_flags | 0x100000;
      attached_creature_can_block_additional(player, card, event,
                                             ability_mod->can_block_additional_creatures);
    }
  }
  return;
}


/* ManalinkEh 2022: 0x26e8740; original functions/auras.c:580. */
int generic_aura_2022(int player, int card, event_t event, int pref_controller, int p_plus,
                      int t_plus, int k_plus, int k_special, int subtype, int clr, int flags)
{
  int result;
  pump_ability_2022_t ability_mod;
  target_definition_2022_t td;
  default_target_definition_2022(player, card, &td, 2);
  td.preferred_controller = pref_controller;
  default_pump_ability_definition_2022(player, card, &ability_mod, p_plus, t_plus, k_plus,
                                       k_special);
  ability_mod.subtype = subtype;
  ability_mod.color = clr;
  if ((flags & 8U) != 0)
  {
    ability_mod.paue_flags = ability_mod.paue_flags | 0x80000;
  }
  if ((flags & 0x10U) != 0)
  {
    ability_mod.paue_flags = ability_mod.paue_flags | 0x40000;
  }
  if ((flags & 0x20U) != 0)
  {
    ability_mod.paue_flags = ability_mod.paue_flags | 0x20000;
  }
  if ((flags & 1U) != 0)
  {
    ability_mod.disabling_mode = ability_mod.disabling_mode | 0x28;
  }
  if ((flags & 2U) != 0)
  {
    ability_mod.disabling_mode = ability_mod.disabling_mode | 0x7e;
  }
  if ((flags & 4U) != 0)
  {
    ability_mod.disabling_mode = ability_mod.disabling_mode | 0x38;
  }
  pump_attached_2022(player, card, event, &ability_mod);
  result = aura(player, card, event, &td, "TARGET_CREATURE");
  return result;
}


/* ManalinkEh 2022: 0x26e9370; original functions/auras.c:950. */
int generic_animating_aura_2022(int player, int card, event_t event, target_definition_2022_t *td,
                                char *prompt, int pow, int tou, int key, int s_key)
{
  int result;
  result = generic_animating_aura_impl_2022(player, card, event, td, prompt, pow, tou, key, s_key,
                                            0, SUBTYPE_INVALID);
  return result;
}


/* ManalinkEh 2022: 0x26e93e0; original functions/auras.c:954. */
int generic_animating_aura_custom_prompt_2022(int player, int card, event_t event,
                                              target_definition_2022_t *td, char *prompt, int pow,
                                              int tou, int key, int s_key)
{
  int result;
  result = generic_animating_aura_impl_2022(player, card, event, td, prompt, pow, tou, key, s_key,
                                            1, SUBTYPE_INVALID);
  return result;
}


/* ManalinkEh 2022: 0x26e9450; original functions/auras.c:958. */
int generic_animating_aura_with_added_subtype_2022(int player, int card, event_t event,
                                                   target_definition_2022_t *td, char *prompt,
                                                   int pow, int tou, int key, int s_key,
                                                   subtype_t subtype)
{
  int result;
  result = generic_animating_aura_impl_2022(player, card, event, td, prompt, pow, tou, key, s_key,
                                            1, subtype);
  return result;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 protection-Aura recursion guard. */
static target_t ward_recalculation_2022 = {-1, -1};
/* ManalinkEh 2022: 0x26e7fd0; original functions/auras.c:213. */
int this_effect_doesnt_remove_this(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  int player_00;
  target_definition_2022_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    pcVar1 = in_play(player, card);
    if ((pcVar1 != NULL) && (pcVar1->damage_target_player != -1))
    {
      if (ward_recalculation_2022.card != card)
      {
        pcVar2 = in_play((int)pcVar1->damage_target_player, pcVar1->damage_target_card);
        if (pcVar2 == NULL)
        {
          kill_card(player, card, KILL_STATE_BASED_ACTION);
          return 1;
        }
        ward_recalculation_2022.player = player;
        player_00 = (int)pcVar1->damage_target_player;
        ward_recalculation_2022.card = card;
        card_00 = pcVar1->damage_target_card;
        pcVar2->regen_status = pcVar2->regen_status | 0x8000000;
        get_abilities(player_00, card_00, EVENT_ABILITIES, -1);
        default_target_definition_2022(player, card, &td, 2);
        td.flags = td.flags | 3;
        result = validate_arbitrary_target_2022(&td, player_00, card_00);
        ward_recalculation_2022.player = -1;
        ward_recalculation_2022.card = -1;
        pcVar2->regen_status = pcVar2->regen_status | 0x8000000;
        if (result == 0)
        {
          kill_card(player, card, KILL_STATE_BASED_ACTION);
        }
        else
        {
          get_abilities(player_00, card_00, EVENT_ABILITIES, -1);
        }
      }
      return 1;
    }
  }
  else if ((event == EVENT_ABILITIES) && (ward_recalculation_2022.player == player))
  {
    return (unsigned int)(ward_recalculation_2022.card == card);
  }
  return 0;
}

#endif
