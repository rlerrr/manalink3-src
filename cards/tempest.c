#include "manalink.h"

// ------------ General functions

int tempest_painland(int player, int card, event_t event){

	comes_into_play_tapped(player, card, event);

	return generic_painland(player, card, event);
}

static int tempest_medallion(int player, int card, event_t event, int clr){
	if( event == EVENT_MODIFY_COST_GLOBAL && ! is_humiliated(player, card) ){
		if( affected_card_controller == player && (get_color(affected_card_controller, affected_card) & clr) ){
			COST_COLORLESS--;
		}
	}

   return 0;
}

int buyback(int player, int card, int colorless, int black, int blue, int green, int red, int white){
	if( ! is_token(player, card) ){
		int diff = 0;
		if( ! played_for_free(player, card) ){
			diff = MIN(0, cards_ptr[get_id(player, card)]->req_colorless+true_get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, -1));
		}
		diff-=(2*count_cards_by_id(player, CARD_ID_MEMORY_CRYSTAL));
		int cless = MAX(0, colorless+diff);
		if( has_mana_multi(player, cless, black, blue, green, red, white) ){
			int choice = do_dialog(player, player, card, -1, -1, " Buyback\n Play normally", 0);
			if( choice == 0 ){
				charge_mana_multi(player, cless, black, blue, green, red, white);
				if( spell_fizzled != 1 ){
					return 1;
				}
			}
		}
	}
	return 0;
}

int generic_licid(int player, int card, event_t event, int colorless, int black, int blue, int green, int red, int white, int pow, int tou, int key, int s_key,
					int pref_controller
  ){
	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CHANGE_TYPE && affect_me(player, card) && instance->dummy3 > 0 ){
		event_result = instance->dummy3;
	}

	if( ! is_what(player, card, TYPE_CREATURE) && instance->damage_target_player > -1 ){
		if( event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE ){
			return generic_activated_ability(player, card, event, 0, 0, black, blue, green, red, white, 0, 0, 0);
		}
		if( event == EVENT_RESOLVE_ACTIVATION ){
			card_instance_t *parent = get_card_instance(instance->parent_controller, instance->parent_card);
			parent->dummy3 = parent->damage_target_player = parent->damage_target_card = -1;
			parent->regen_status |= KEYWORD_RECALC_ALL ;
			get_abilities(instance->parent_controller, instance->parent_card, EVENT_CHANGE_TYPE, -1);
		}
		return generic_aura(player, card, event, player, pow, tou, key, s_key, 0, 0, 0);
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = pref_controller;

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED+GAA_NONSICK+GAA_CAN_TARGET, colorless, black, blue, green, red, white, 0,
										&td, "TARGET_CREATURE");
	}

	if( event == EVENT_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED+GAA_NONSICK+GAA_CAN_TARGET, colorless, black, blue, green, red, white, 0,
										&td, "TARGET_CREATURE");
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			card_instance_t *parent = get_card_instance(instance->parent_controller, instance->parent_card);
			int newtype = create_a_card_type(parent->internal_card_id);
			cards_at_7c7000[newtype]->type = TYPE_ENCHANTMENT;
			cards_at_7c7000[newtype]->power = 0;
			cards_at_7c7000[newtype]->toughness = 0;
			parent->dummy3 = newtype;
			attach_aura_to_target(instance->parent_controller, instance->parent_card, event, instance->targets[0].player, instance->targets[0].card);
			parent->regen_status |= KEYWORD_RECALC_ALL ;
			get_abilities(instance->parent_controller, instance->parent_card, EVENT_CHANGE_TYPE, -1);
		}
	}

	return 0;
}

const char* activating_sliver(int who_chooses, int player, int card){
	if(	can_use_activated_abilities(player, card) ){
		return NULL;
	}
	return "this Sliver cannot use activated abilities.";
}

int sliver_shared_shade_ability(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);

	if( instance->targets[1].player > -1 ){
		int p = instance->targets[1].player;
		int c = instance->targets[1].card;

		if( leaves_play(p, c, event) ){
			add_status(player, card, STATUS_INVISIBLE_FX);
		}

		if( ! in_play(p, c) ){
			if( event == EVENT_CAN_ACTIVATE ){
				return 0;
			}

			if( event == EVENT_CLEANUP ){
				kill_card(player, card, KILL_EXILE);
			}
		}

		if( is_humiliated(p, c) ){
			return 0;
		}

		if( ! IS_GAA_EVENT(event) ){
			return 0;
		}

		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.allowed_controller = player;
		td.preferred_controller = player;
		td.required_subtype = SUBTYPE_SLIVER;
		td.extra = (int32_t)activating_sliver;
		td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
		td.illegal_abilities = 0;

		if( event == EVENT_CAN_ACTIVATE ){
			int cc;
			for(cc=0; cc < active_cards_count[player]; cc++){
				if( in_play(player, cc) && is_what(player, cc, TYPE_PERMANENT) && has_subtype(player, cc, SUBTYPE_SLIVER) ){
					if( generic_activated_ability(player, cc, EVENT_CAN_ACTIVATE, 0, MANACOST_X(2), 0, NULL, NULL) ){
						return 1;
					}
				}
			}
		}

		if( event == EVENT_ACTIVATE ){
			char buffer[100];
			scnprintf(buffer, 100, "%s: select a Sliver to activate.", cards_ptr[get_id(p, c)]->name);
			if( new_pick_target(&td, buffer, 0, 1 | GS_LITERAL_PROMPT) ){
				instance->number_of_targets = 1;
				if( can_use_activated_abilities(instance->targets[0].player, instance->targets[0].card) ){
					charge_mana_for_activated_ability(instance->targets[0].player, instance->targets[0].card, MANACOST_X(2));
				}
				else{
					spell_fizzled = 1;
				}
			}
		}

		if( event == EVENT_RESOLVE_ACTIVATION ){
			int pow = instance->targets[2].player > 0 ? instance->targets[2].player : 0;
			int tou = instance->targets[2].card > 0 ? instance->targets[2].card : 0;
			int key = instance->targets[3].player > 0 ? instance->targets[3].player : 0;
			int s_key = instance->targets[3].card > 0 ? instance->targets[3].card : 0;
			if( ! key && ! s_key ){
				pump_until_eot(instance->targets[0].player, instance->targets[0].card, instance->targets[0].player, instance->targets[0].card, pow, tou);
			}
			else{
				pump_ability_until_eot(instance->targets[0].player, instance->targets[0].card, instance->targets[0].player, instance->targets[0].card,
										pow, tou, key, s_key);
			}
		}
	}

	return 0;
}

int sliver_with_shared_shade_ability(int player, int card, event_t event, int pow, int tou, int key, int s_key){

	if( event == EVENT_RESOLVE_SPELL ){
		int legacy = create_legacy_activate(player, card, &sliver_shared_shade_ability);
		card_instance_t *instance = get_card_instance(player, legacy);
		instance->targets[1].player = player;
		instance->targets[1].card = card;
		instance->targets[2].player = pow;
		instance->targets[2].card = tou;
		instance->targets[3].player = key;
		instance->targets[3].card = s_key;
		instance->number_of_targets = 1;
		int fake = add_card_to_hand(1-player, get_card_instance(player, card)->internal_card_id);
		legacy = create_legacy_activate(1-player, fake, &sliver_shared_shade_ability);
		instance = get_card_instance(1-player, legacy);
		instance->targets[1].player = player;
		instance->targets[1].card = card;
		instance->targets[2].player = pow;
		instance->targets[2].card = tou;
		instance->targets[3].player = key;
		instance->targets[3].card = s_key;
		instance->number_of_targets = 1;
		obliterate_card(1-player, fake);
	}

	return slivercycling(player, card, event);
}

int shared_sliver_activated_ability(int player, int card, event_t event, int mode, int cless, int black, int blue, int green, int red, int white,
									uint32_t variable_costs, target_definition_t *td2, const char *prompt){

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_PERMANENT);
	td.allowed_controller = player;
	td.preferred_controller = player;
	td.required_subtype = SUBTYPE_SLIVER;
	td.extra = (int32_t)activating_sliver;
	td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
	td.illegal_state = (mode & GAA_UNTAPPED) ? TARGET_STATE_TAPPED | TARGET_STATE_SUMMONING_SICK : 0;
	td.illegal_abilities = 0;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE && in_play(player, card) ){
		int cc;
		for(cc=0; cc < active_cards_count[player]; cc++){
			if( in_play(player, cc) && is_what(player, cc, TYPE_PERMANENT) && has_subtype(player, cc, SUBTYPE_SLIVER) ){
				int result = generic_activated_ability(player, cc, EVENT_CAN_ACTIVATE, mode, cless, black, blue, green, red, white, variable_costs, td2, prompt);
				if( result ){
					return result;
				}
			}
		}
	}

	if( event == EVENT_ACTIVATE ){
		if( new_pick_target(&td, "Select a Sliver to activate", 0, 1 | GS_LITERAL_PROMPT) ){
			instance->number_of_targets = 1;
			if( can_use_activated_abilities(instance->targets[0].player, instance->targets[0].card) ){
				generic_activated_ability(instance->targets[0].player, instance->targets[0].card, event,
											mode, cless, black, blue, green, red, white, variable_costs, td2, prompt);
				if( spell_fizzled != 1 ){
					instance->targets[2] = get_card_instance(instance->targets[0].player, instance->targets[0].card)->targets[0];
				}
			}
			else{
				spell_fizzled = 1;
			}
		}
	}

	return 0;
}

//--------------- Cards

int card_abandon_hope(int player, int card, event_t event){

	/* Abandon Hope	|X|1|B
	 * Sorcery
	 * As an additional cost to cast ~, discard X cards.
	 * Look at target opponent's hand and choose X cards from it. That player discards those cards. */

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_X_SPELL | GS_CAN_ONLY_TARGET_OPPONENT, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		set_special_flags2(player, card, SF2_X_SPELL);
		if( (played_for_free(player, card) || is_token(player, card)) && ! check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
			return 0;
		}
		if( ! check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
			int original_hand[2][hand_count[player]];
			int ohc = 0;
			int i;
			for(i=0; i<active_cards_count[player]; i++){
				if( in_hand(player, i) ){
					original_hand[0][ohc] = get_original_internal_card_id(player, i);
					original_hand[1][ohc] = i;
					ohc++;
				}
			}
			int to_discard[hand_count[player]];
			int tdc = 0;
			test_definition_t this_test;
			new_default_test_definition(&this_test, TYPE_ANY, "Select a card to discard.");
			while( ohc && has_mana(player, COLOR_COLORLESS, tdc+1) ){
					int selected = select_card_from_zone(player, player, original_hand[0], ohc, 0, AI_MIN_VALUE, -1, &this_test);
					if( selected != -1 ){
						to_discard[tdc] = original_hand[1][selected];
						tdc++;
						for(i=selected; i<ohc; i++){
							original_hand[0][i] = original_hand[0][i+1];
							original_hand[1][i] = original_hand[1][i+1];
						}
						ohc--;
					}
					else{
						break;
					}
			}
			if( tdc ){
				charge_mana(player, COLOR_COLORLESS, tdc);
				if( spell_fizzled != 1 ){
					for(i=0; i<tdc; i++){
						discard_card(player, to_discard[i]);
					}
					instance->info_slot = tdc;
				}
			}
		}
		instance->targets[0].player = 1-player;
		instance->targets[0].card = -1;
		instance->number_of_targets = 1;
	}

	if(event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			test_definition_t this_test;
			default_test_definition(&this_test, TYPE_ANY);

			ec_definition_t this_definition;
			default_ec_definition(instance->targets[0].player, player, &this_definition);
			this_definition.qty = instance->info_slot;

			new_effect_coercion(&this_definition, &this_test);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}


int card_advance_scout(int player, int card, event_t event){
/* CARD_ID_ADVANCE_SCOUT	3073
Advance Scout	|1|W
Creature - Human Soldier Scout 1/1
First strike
|W: Target creature gains first strike until end of turn. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			pump_ability_until_eot(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card,
									0, 0, KEYWORD_FIRST_STRIKE, 0);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_W(1), 0, &td, "TARGET_CREATURE");
}

int card_aftershock(int player, int card, event_t event){
/* CARD_ID_AFTERSHOCK	3074
Aftershock	|2|R|R
Sorcery
Destroy target artifact, creature, or land. ~ deals 3 damage to you. */

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT|TYPE_CREATURE|TYPE_LAND);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
			damage_player(player, 3, player, card);
		}
		kill_card(player, card, KILL_DESTROY);
	}
	return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td, "Select target artifact, creature or land.", 1, NULL);
}

int card_altar_of_dementia(int player, int card, event_t event){

	/* Altar of Dementia	|2
	 * Artifact
	 * Sacrifice a creature: Target player puts a number of cards equal to the sacrificed creature's power from the top of his or her library into his or her
	 * graveyard. */

	if (!IS_GAA_EVENT(event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_SACRIFICE_CREATURE, MANACOST0, 0, &td, NULL);
	}

	if( event == EVENT_ACTIVATE ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");
		int sac = new_sacrifice(player, card, player, SAC_JUST_MARK|SAC_AS_COST|SAC_RETURN_CHOICE, &test);
		if (!sac){
			cancel = 1;
			return 0;
		}
		instance->number_of_targets = 0;
		if( pick_target(&td, "TARGET_PLAYER") ){
			instance->info_slot = get_power(BYTE2(sac), BYTE3(sac));
			kill_card(BYTE2(sac), BYTE3(sac), KILL_SACRIFICE);
		}
		else{
			state_untargettable(BYTE2(sac), BYTE3(sac), 0);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			mill(instance->targets[0].player, instance->info_slot);
		}
	}

	return 0;
}

static int aluren_effect(int player, int card, event_t event){

	if( is_what(player, card, TYPE_EFFECT) && get_card_instance(player, card)->targets[0].player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);
		if( is_humiliated(instance->targets[0].player, instance->targets[0].card) ){
			return 0;
		}
		if( other_leaves_play(player, card, instance->targets[0].player, instance->targets[0].card, event) ){
			kill_card(player, card, KILL_EXILE);
		}
	}

	if( event == EVENT_CAN_ACTIVATE ){
		int i;
		for(i=0; i<active_cards_count[player]; i++){
			if( in_hand(player, i) && is_what(player, i, TYPE_CREATURE) && get_cmc(player, i) <= 3 ){
				return 1;
			}
		}
	}

	if( event == EVENT_ACTIVATE ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_CREATURE, "Select a creature card with CMC <= 3 to play.");
		this_test.cmc = 4;
		this_test.cmc_flag = F5_CMC_LESSER_THAN_VALUE;
		int selected = new_select_a_card(player, player, TUTOR_FROM_HAND, 0, AI_MAX_VALUE, -1, &this_test);
		if( selected != -1 ){
			play_card_in_hand_for_free(player, selected);
			cant_be_responded_to = 1;	// The spell will be respondable to, but this (fake) activation won't
		}
		else{
			spell_fizzled = 1;
		}
	}
	return 0;
}

int card_aluren(int player, int card, event_t event){
	/*
	  Aluren |2|G|G
	  Enchantment
	  Any player may play creature cards with converted mana cost 3 or less without paying their mana cost and as though they had flash.
	*/

	if( event == EVENT_RESOLVE_SPELL ){
		int fake = add_card_to_hand(1-player, get_card_instance(player, card)->internal_card_id);
		int legacy = create_legacy_activate(1-player, fake, &aluren_effect);
		card_instance_t *instance = get_card_instance(1-player, legacy);
		instance->targets[0].player = player;
		instance->targets[0].card = card;
		instance->number_of_targets = 1;
		obliterate_card(1-player, fake);
	}

	if( (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE) && ! is_humiliated(player, card) ){
		return aluren_effect(player, card, event);
	}

	return global_enchantment(player, card, event);
}

int card_ancient_runes(int player, int card, event_t event){
/* CARD_ID_ANCIENT_RUNES	3077
Ancient Runes	|2|R
Enchantment
At the beginning of each player's upkeep, ~ deals damage to that player equal to the number of artifacts he or she controls. */

	upkeep_trigger_ability(player, card, event, ANYBODY);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		damage_player(current_turn, count_subtype(current_turn, TYPE_ARTIFACT, -1), player, card);
	}

	return global_enchantment(player, card, event);
}

int card_ancient_tomb(int player, int card, event_t event){
/* CARD_ID_ANCIENT_TOMB	3078
Ancient Tomb	""
Land
|T: Add |C|C to your mana pool. ~ deals 2 damage to you. */

	if(event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		damage_player(player, 2, instance->parent_controller, instance->parent_card);
	}
	return two_mana_land(player, card, event, COLOR_COLORLESS, COLOR_COLORLESS);
}

int card_anoint(int player, int card, event_t event){
/* CARD_ID_ANOINT	3080
Anoint	|W
Instant
Buyback |3 (You may pay an additional |3 as you cast this spell. If you do, put this card into your hand as it resolves.)
Prevent the next 3 damage that would be dealt to target creature this turn. */

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.extra = damage_card;
	td.special = TARGET_SPECIAL_DAMAGE_CREATURE;
	td.required_type = 0;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET | GS_DAMAGE_PREVENTION, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_CAN_TARGET | GS_DAMAGE_PREVENTION, &td, "TARGET_DAMAGE", 1, NULL);
		if( spell_fizzled != 1 ){
			instance->info_slot = buyback(player, card, MANACOST_X(3));
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			prevent_damage(instance->targets[0].player, instance->targets[0].card, 3);
			if( instance->info_slot == 1 ){
				bounce_permanent(player, card);
				return 0;
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}
	return 0;
}

int card_apocalypse(int player, int card, event_t event){
/* CARD_ID_APOCALYPSE	3082
Apocalypse	|2|R|R|R
Sorcery
Exile all permanents. You discard your hand. */

	if( event == EVENT_RESOLVE_SPELL){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_PERMANENT);
		APNAP(p, {new_manipulate_all(player, card, p, &this_test, KILL_EXILE);};);
		discard_all(player);
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
}

int card_armord_sliver(int player, int card, event_t event){
	/*
	  Armor Sliver |2|W
	  Creature - Sliver 2/2
	  All Sliver creatures have "{2}: This creature gets +0/+1 until end of turn."
	*/
	return sliver_with_shared_shade_ability(player, card, event, 0, 1, 0, 0);
}

int card_auratog(int player, int card, event_t event){
	/*
	  Auratog English |1|W
	  Creature - Atog 1/2
	  Sacrifice an enchantment: Auratog gets +2/+2 until end of turn.
	*/
	return generic_husk(player, card, event, TYPE_ENCHANTMENT, 2, 2, 0, 0);
}

int card_barbed_sliver(int player, int card, event_t event){
	/*
	  Barbed Sliver |2|R
	  Creature - Sliver 2/2
	  All Sliver creatures have "{2}: This creature gets +1/+0 until end of turn."
	*/
	return sliver_with_shared_shade_ability(player, card, event, 1, 0, 0, 0);
}

int card_blood_frenzy(int player, int card, event_t event){
/* CARD_ID_BLOOD_FRENZY	3090
Blood Frenzy	|1|R
Instant
Cast ~ only before the combat damage step.
Target attacking or blocking creature gets +4/+0 until end of turn. Destroy that creature at the beginning of the next end step. */

	if (event == EVENT_CHECK_PUMP ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.preferred_controller = player;
		td.required_state = TARGET_STATE_IN_COMBAT;
		if( current_phase == PHASE_AFTER_BLOCKING && generic_spell(player, card, EVENT_CAN_CAST, GS_CAN_TARGET, &td, NULL, 1, NULL) ){
			return vanilla_instant_pump(player, card, event, ANYBODY, player, 4, 0, 0, 0);
		}
	}

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;
	td.required_state = TARGET_STATE_IN_COMBAT;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST && current_phase == PHASE_AFTER_BLOCKING ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td, "Select target attacking or blocking creature", 1, NULL);
	}

	if(event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			pump_ability_t pump;
			default_pump_ability_definition(player, card, &pump, 4, 0, 0, 0);
			pump.paue_flags = PAUE_END_AT_EOT | PAUE_REMOVE_TARGET_AT_EOT;
			pump.eot_removal_method = KILL_DESTROY;
			pump_ability(player, card, instance->targets[0].player, instance->targets[0].card, &pump);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_bottle_gnomes(int player, int card, event_t event){
/* CARD_ID_BOTTLE_GNOMES	1705
Bottle Gnomes	|3
Artifact Creature - Gnome 1/3
Sacrifice ~: You gain 3 life. */

	if( event == EVENT_RESOLVE_ACTIVATION ){
		gain_life(player, 3);
	}

	return generic_activated_ability(player, card, event, GAA_SACRIFICE_ME, MANACOST0, 0, NULL, NULL);
}

int card_broken_fall(int player, int card, event_t event){
/* CARD_ID_BROKEN_FALL	3094
Broken Fall	|2|G
Enchantment
Return ~ to its owner's hand: Regenerate target creature. */

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.preferred_controller = player;
	td.required_state = TARGET_STATE_DESTROYED;
	if( player == AI ){
		td.special = TARGET_SPECIAL_REGENERATION;
	}

	if( event == EVENT_CAN_ACTIVATE && ( land_can_be_played & LCBP_REGENERATION ) ){
		if( generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_BOUNCE_ME, MANACOST0, 0, &td, NULL) ){
			return 99;
		}
	}

	if( event == EVENT_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_BOUNCE_ME | GAA_LITERAL_PROMPT, MANACOST0, 0,
										&td, "Select target creature to regenerate.");
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		if( valid_target(&td) && can_be_regenerated(instance->targets[0].player, instance->targets[0].card) ){
			regenerate_target( instance->targets[0].player, instance->targets[0].card );
		}
	}

	return 0;
}

int card_capsize(int player, int card, event_t event){

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_PERMANENT);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PERMANENT", 1, NULL);
		if( spell_fizzled != 1 ){
			instance->info_slot = buyback(player, card, MANACOST_X(3));
		}
	}

	if(event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			bounce_permanent(instance->targets[0].player, instance->targets[0].card);
			if( instance->info_slot == 1 ){
				bounce_permanent(player, card);
				return 0;
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_carrionette(int player, int card, event_t event){
	/* Carrionette	|1|B
	 * Creature - Skeleton 1/1
	 * |2|B|B: Exile ~ and target creature unless that creature's controller pays |2. Activate this ability only if ~ is in your graveyard. */

	if( event == EVENT_GRAVEYARD_ABILITY && has_mana_multi(player, MANACOST_XB(2, 2)) ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE );

		if( can_target(&td) ){
			return GA_BASIC;
		}
	}

	if( event == EVENT_PAY_FLASHBACK_COSTS ){
		charge_mana_multi(player, MANACOST_XB(2, 2));
		if( spell_fizzled != 1){
			return GAPAID_EXILE;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATED_GRAVEYARD_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE );
		td.allow_cancel = 0;

		card_instance_t *instance= get_card_instance(player, card);
		if( can_target(&td) && pick_target(&td, "TARGET_CREATURE") ){
			charge_mana(instance->targets[0].player, COLOR_COLORLESS, 2);
			if( spell_fizzled == 1 ){
				kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
			}
		}
	}

	return 0;
}

int card_choke(int player, int card, event_t event){
	/*
	  Choke |2|G
	  Enchantment
	  Islands don't untap during their controllers' untap steps.
	*/

	if( event == EVENT_CAN_CAST ){
		return basic_spell(player, card, event);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier+=5*(count_subtype(1-player, TYPE_LAND, SUBTYPE_ISLAND)-count_subtype(player, TYPE_LAND, SUBTYPE_ISLAND));
	}

	if( current_phase == PHASE_UNTAP && event == EVENT_UNTAP ){
		if( has_subtype(affected_card_controller, affected_card, get_hacked_subtype(player, card, SUBTYPE_ISLAND)) ){
			does_not_untap(affected_card_controller, affected_card, event);
		}
	}

	return 0;
}

const char* sliver_to_regenerate(int who_chooses, int player, int card){
	if(	can_use_activated_abilities(player, card) ){
		if( who_chooses == HUMAN || (who_chooses == AI && can_regenerate(player, card)) ){
			return NULL;
		}
	}
	return "this Sliver cannot regenerate.";
}

int clot_sliver_shared_ability(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);

	if( instance->targets[1].player > -1 ){
		int p = instance->targets[1].player;
		int c = instance->targets[1].card;

		if( leaves_play(p, c, event) ){
			kill_card(player, card, KILL_EXILE);
		}

		if( is_humiliated(p, c) ){
			return 0;
		}

		if( ! IS_GAA_EVENT(event) ){
			return 0;
		}

		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.required_state = TARGET_STATE_DESTROYED;
		td.preferred_controller = player;
		td.required_subtype = SUBTYPE_SLIVER;
		td.extra = (int32_t)sliver_to_regenerate;
		td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
		td.illegal_abilities = 0;

		if( event == EVENT_CAN_ACTIVATE && in_play(p, c) ){
			if( land_can_be_played & LCBP_REGENERATION ){
				int pp, cc;
				for(pp = 0; pp<2; pp++){
					for(cc=0; cc < active_cards_count[pp]; cc++){
						if( in_play(pp, cc) && is_what(pp, cc, TYPE_PERMANENT) && has_subtype(pp, cc, SUBTYPE_SLIVER) ){
							int result = regeneration(pp, cc, EVENT_CAN_ACTIVATE, MANACOST_X(2));
							if( result ){
								return result;
							}
						}
					}
				}
			}
		}

		if( event == EVENT_ACTIVATE ){
			if( new_pick_target(&td, "Select a Sliver to regenerate.", 0, 1 | GS_LITERAL_PROMPT) ){
				instance->number_of_targets = 1;
				if( can_use_activated_abilities(instance->targets[0].player, instance->targets[0].card) ){
					charge_mana_for_activated_ability(instance->targets[0].player, instance->targets[0].card, MANACOST_X(2));
				}
				else{
					spell_fizzled = 1;
				}
			}
		}

		if( event == EVENT_RESOLVE_ACTIVATION && valid_target(&td) && can_regenerate(instance->targets[0].player, instance->targets[0].card) ){
			regenerate_target(instance->targets[0].player, instance->targets[0].card);
		}
	}

	return 0;
}

int card_clot_sliver(int player, int card, event_t event){
	/*
	  Clot Sliver |1|B
	  Creature - Sliver 1/1
	  All Slivers have "{2}: Regenerate this permanent."
	*/

	if( event == EVENT_RESOLVE_SPELL ){
		int legacy = create_legacy_activate(player, card, &clot_sliver_shared_ability);
		card_instance_t *instance = get_card_instance(player, legacy);
		instance->targets[1].player = player;
		instance->targets[1].card = card;
		instance->number_of_targets = 1;
		int fake = add_card_to_hand(1-player, get_card_instance(player, card)->internal_card_id);
		legacy = create_legacy_activate(1-player, fake, &clot_sliver_shared_ability);
		instance = get_card_instance(1-player, legacy);
		instance->targets[1].player = player;
		instance->targets[1].card = card;
		instance->number_of_targets = 1;
		obliterate_card(1-player, fake);
	}
	return slivercycling(player, card, event);
}

int card_cloudchaser_eagle(int player, int card, event_t event){
	/*
	  Cloudchaser Eagle |3|W
	  Creature - Bird 2/2
	  Flying
	  When Cloudchaser Eagle enters the battlefield, destroy target enchantment.
	*/
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_ENCHANTMENT);
		td.allow_cancel = 0;

		if( can_target(&td) && new_pick_target(&td, "TARGET_ENCHANTMENT", 0, 0) ){
			action_on_target(player, card, 0, KILL_DESTROY);
		}
		get_card_instance(player, card)->number_of_targets = 0;
	}

	return 0;
}

int cq_effect(int player, int card, event_t event){

	if( get_card_instance(player, card)->targets[0].player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);
		int cq_player = instance->targets[0].player;
		int cq_card = instance->targets[0].card;
		if( ! is_tapped(cq_player, cq_card) || other_leaves_play(player, card, cq_player, cq_card, event) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
		}
	}

	return 0;
}

int card_coffin_queen(int player, int card, event_t event){

	choose_to_untap(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_CREATURE, "Select target creature card.");

	if (event == EVENT_RESOLVE_ACTIVATION){
		card_instance_t* instance = get_card_instance(player, card);
		int selected = validate_target_from_grave_source(player, card, instance->targets[0].player, 1);
		if (selected != -1){
			int zombie = reanimate_permanent(player, card, instance->targets[2].player, selected, REANIMATE_DEFAULT | REANIMATE_NO_CONTROL_LEGACY);

			if( zombie > -1 ){
				int legacy = create_targetted_legacy_effect(instance->parent_controller, instance->parent_card, &cq_effect, player, zombie);
				card_instance_t *leg = get_card_instance(player, legacy);
				leg->targets[0].player = instance->parent_controller;
				leg->targets[0].card = instance->parent_card;
				leg->number_of_targets = 1;
				card_instance_t *parent = get_card_instance(instance->parent_controller, instance->parent_card);
				parent->targets[1] = instance->targets[0];
			}
		}
	}

	return generic_activated_ability_with_target_in_grave(player, card, event, 0, MANACOST_XB(2,1), 0, NULL, NULL,
															0, &test, GS_GRAVE_RECYCLER_BOTH_GRAVES);
}

int card_cold_storage(int player, int card, event_t event){
	/*
	  Cold Storage |4
	  Artifact
	  {3}: Exile target creature you control.
	  Sacrifice Cold Storage: Return each creature card exiled with Cold Storage to the battlefield under your control.
	*/
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.allowed_controller = player;
	td.preferred_controller = player;
	if( player == AI ){
		td.required_state = TARGET_STATE_DESTROYED;
	}

	enum{
		CHOICE_EXILE = 1,
		CHOICE_RETURN,
		CHOICE_SHOW_EXILED
	};

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_CAN_ACTIVATE ){
		if( player == AI ){
			if( generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_REGENERATION, MANACOST_X(3), 0, &td, "TARGET_CREATURE") ){
				return 0x63;
			}
		}
		if( player == HUMAN ){
			if( generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_X(3), 0, &td, "TARGET_CREATURE") ){
				return 1;
			}
			if( exiledby_count(player, card, ANYBODY) ){
				return 1;
			}
		}
		if( generic_activated_ability(player, card, event, GAA_SACRIFICE_ME, MANACOST0, 0, NULL, NULL) ){
			return 1;
		}
	}

	if( event == EVENT_ACTIVATE){
		instance->info_slot = instance->number_of_targets = 0;
		int abils[3] = {
						(player == AI ? generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_CAN_TARGET | GAA_REGENERATION, MANACOST_X(3), 0, &td, NULL) :
						generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_CAN_TARGET, MANACOST_X(3), 0, &td, NULL)),
						generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_SACRIFICE_ME, MANACOST0, 0, NULL, NULL),
						player == HUMAN && exiledby_count(player, card, ANYBODY)
						};
		int choice = DIALOG(player, card, event, DLG_RANDOM, DLG_NO_STORAGE,
							"Exile creature", abils[0], 10,
							"Sac & return creature", abils[1], (instance->targets[1].player > -1 ? instance->targets[1].player*5 : 0),
							"Show exiled creatures", abils[2], 1);

		if( ! choice ){
			spell_fizzled = 1;
			return 0;
		}
		if( choice == CHOICE_EXILE ){
			if( charge_mana_for_activated_ability(player, card, MANACOST_X(3)) ){
				pick_target(&td, "TARGET_CREATURE");
			}
		}
		if( choice == CHOICE_RETURN ){
			if( charge_mana_for_activated_ability(player, card, MANACOST0) ){
				instance->targets[1].card = exiledby_detach(player, card);
				kill_card(player, card, KILL_SACRIFICE);
			}
		}
		if( choice == CHOICE_SHOW_EXILED ){
			exiledby_choose(player, card, CARD_ID_COLD_STORAGE, EXBY_CHOOSE, 0, "Creature", 0);
			spell_fizzled = 1;
		}
		if( spell_fizzled != 1 ){
			instance->info_slot = choice;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION){
		if( instance->info_slot == CHOICE_EXILE ){
			if( valid_target(&td) ){
				if( player == AI ){
					regenerate_target(instance->targets[0].player, instance->targets[0].card);
				}
				exile_card_and_remember_it_on_exiledby(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card);
				int prev = get_card_instance(instance->parent_controller, instance->parent_card)->targets[1].player;
				get_card_instance(instance->parent_controller, instance->parent_card)->targets[1].player = (prev != -1 ? prev+1 : 1);
			}
		}

		if( instance->info_slot == CHOICE_RETURN ){
			int leg = 0;
			int idx = 0;
			int* loc;

			while ((loc = exiledby_find_any(player-2, instance->targets[1].card, &leg, &idx)) != NULL){
					int owner = (*loc & 0x80000000) ? 1 : 0;
					int iid = *loc & ~0x80000000;
					*loc = -1;
					if( check_rfg(owner, cards_data[iid].id) ){
						int card_added = add_card_to_hand(player, iid);
						remove_card_from_rfg(owner, cards_data[iid].id);
						if( owner != player ){
							if( player == AI ){
								remove_state(player, card_added, STATE_OWNED_BY_OPPONENT);
							}
							else{
								add_state(player, card_added, STATE_OWNED_BY_OPPONENT);
							}
						}
						put_into_play(player, card_added);
						if( owner != player ){
							create_targetted_legacy_effect(player, card, &empty, player, card_added);
						}
					}
			}
		}
	}

	return 0;
}

int card_commander_greven_il_vec(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	fear(player, card, event);

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		if( count_permanents_by_type(player, TYPE_CREATURE) < 1 ){
			ai_modifier-=128;
		}
	}

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");
		new_sacrifice(player, card, player, SAC_CAUSED | SAC_NO_CANCEL, &test);
	}
	return 0;
}

int card_corpse_dance(int player, int card, event_t event){

	if( event == EVENT_CAN_CAST ){
		return basic_spell(player, card, event);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		basic_spell(player, card, event);
		if( spell_fizzled != 1 ){
			get_card_instance(player, card)->info_slot = buyback(player, card, MANACOST_X(2));
		}
		if( ! count_graveyard_by_type(player, TYPE_CREATURE) ){
			ai_modifier -= 96;
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		const int *grave = get_grave(player);
		int count = count_graveyard(player)-1;
		while( count > -1 ){
				if( is_what(-1, grave[count], TYPE_CREATURE) ){
					reanimate_permanent(player, card, player, count, REANIMATE_HASTE_AND_EXILE_AT_EOT);
					break;
				}
				count--;
		}
		if( get_card_instance(player, card)->info_slot == 1 ){
			bounce_permanent(player, card);
			return 0;
		}
		else{
			kill_card(player, card, KILL_DESTROY);
		}
	}

	return 0;
}

int card_crazed_armodon(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		pump_ability_t pump;
		default_pump_ability_definition(instance->parent_controller, instance->parent_card, &pump, 3, 0, KEYWORD_TRAMPLE, 0);
		pump.paue_flags = PAUE_END_AT_EOT | PAUE_REMOVE_TARGET_AT_EOT;
		pump.eot_removal_method = KILL_DESTROY;
		pump_ability(instance->parent_controller, instance->parent_card, instance->parent_controller, instance->parent_card, &pump);
	}

	return generic_activated_ability(player, card, event, GAA_ONCE_PER_TURN, MANACOST_G(1), 0, NULL, NULL);
}

int card_magus_of_the_scroll(int player, int card, event_t event);

int card_cursed_scroll(int player, int card, event_t event){
	/* Cursed Scroll	|1	0x2001812
	 * Artifact
	 * |3, |T: Choose a card name, then reveal a card at random from your hand. If that card has the chosen name, ~ deals 2 damage to any target. */
	if( ! is_unlocked(player, card, event, 6) ){ return 0; }

	return card_magus_of_the_scroll(player, card, event);
}

int card_darkling_stalker(int player, int card, event_t event){

	if (event == EVENT_POW_BOOST || event == EVENT_TOU_BOOST){
		return generic_shade(player, card, event, 0, MANACOST_B(1), 1, 1, 0, 0);
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	enum{
		CHOICE_PUMP = 1,
		CHOICE_REGENERATE
	};


	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability(player, card, event, 0, MANACOST_B(1), 0, NULL, NULL) ){
			if( ( land_can_be_played & LCBP_REGENERATION) && (! IS_AI(player) || can_regenerate(player, card)) ){
				return 0x63;
			}
			return 1;
		}
	}

	if(event == EVENT_ACTIVATE ){
		instance->info_slot = CHOICE_PUMP;
		if( ( land_can_be_played & LCBP_REGENERATION) && (! IS_AI(player) || can_regenerate(player, card)) ){
			instance->info_slot = CHOICE_REGENERATE;
		}
		return generic_activated_ability(player, card, event, 0, MANACOST_B(1), 0, NULL, NULL);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == CHOICE_PUMP ){
			return generic_shade(player, card, event, 0, MANACOST_B(1), 1, 1, 0, 0);
		}
		if( instance->info_slot == CHOICE_REGENERATE && can_regenerate(instance->parent_controller, instance->parent_card) ){
			regenerate_target(instance->parent_controller, instance->parent_card);
		}
	}

	return 0;
}

int card_dauthi_embrace(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	if( event == EVENT_ACTIVATE ){
		ai_modifier += (current_phase < PHASE_DECLARE_BLOCKERS ? 48 : -48);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			card_instance_t *instance = get_card_instance(player, card);
			pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 0, 0, 0, SP_KEYWORD_SHADOW);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_B(2), 0, &td, "TARGET_CREATURE");
}

int card_dauthi_ghoul(int player, int card, event_t event){

	shadow(player, card, event);

	if( event == EVENT_GRAVEYARD_FROM_PLAY ){
		if( in_play(affected_card_controller, affected_card) &&
			is_what(affected_card_controller, affected_card, TYPE_CREATURE) &&
			check_for_special_ability(affected_card_controller, affected_card, SP_KEYWORD_SHADOW) )
		{
			count_for_gfp_ability(player, card, event, ANYBODY, TYPE_CREATURE, NULL);
		}
	}

	int result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
	if( result ){
		add_1_1_counters(player, card, BYTE0(result));
	}

	return 0;
}

int card_dauthi_horror(int player, int card, event_t event){
	shadow(player, card, event);
	if(event == EVENT_BLOCK_LEGALITY && ! is_humiliated(player, card) ){
		if(player == attacking_card_controller && card == attacking_card ){
			if (get_color(affected_card_controller, affected_card) & get_sleighted_color_test(player, card, COLOR_TEST_WHITE) ){
				event_result = 1;
			}
		}
	}
	return 0;
}

int card_dauthi_marauder(int player, int card, event_t event){
	shadow(player, card, event);
	return 0;
}

int card_dauthi_mercenary(int player, int card, event_t event){
	shadow(player, card, event);
	return generic_shade(player, card, event, 0, MANACOST_XB(1, 1), 1, 0, 0, 0);
}

int card_dauthi_mindripper(int player, int card, event_t event){

   shadow(player, card, event);

   if( ! is_humiliated(player, card) ){
		if( event == EVENT_CAN_ACTIVATE && is_attacking(player, card) && is_unblocked(player, card) &&
			current_phase == PHASE_AFTER_BLOCKING )
		{
			return 1;
		}

		if( event == EVENT_ACTIVATE){
			kill_card(player, card, KILL_SACRIFICE);
		}

		if( event == EVENT_RESOLVE_ACTIVATION ){
			new_multidiscard(1-player, 3, 0, player);
		}
	}

	return 0;
}


int card_dauthi_slayer(int player, int card, event_t event){
	shadow(player, card, event);
	return attack_if_able(player, card, event);
}

int card_death_pits_of_rath(int player, int card, event_t event){
	/*
	  Death Pits of Rath |3|B|B
	  Enchantment
	  Whenever a creature is dealt damage, destroy it. It can't be regenerated.
	*/

	if( ! is_humiliated(player, card) ){
		if( event == EVENT_DEAL_DAMAGE ){
			card_instance_t *instance = get_card_instance(player, card);
			card_instance_t *damage= get_card_instance( affected_card_controller, affected_card);
			if( damage->internal_card_id == damage_card ){
				if( damage->damage_target_card != -1 ){
					if( is_what(damage->damage_target_player, damage->damage_target_card, TYPE_CREATURE) ){
						int good = 0;
						if( damage->info_slot > 0 ){
							good = 1;
						}
						else{
							card_instance_t *trg = get_card_instance(damage->damage_source_player, damage->damage_source_card);
							if( trg->targets[16].player > 0 ){
								good = 1;
							}
						}

						if( good == 1 ){
							if( instance->info_slot < 0 ){
								instance->info_slot = 0;
							}
							instance->targets[instance->info_slot].player = damage->damage_target_player;
							instance->targets[instance->info_slot].card = damage->damage_target_card;
							instance->info_slot++;
						}
					}
				}
			}
		}

		if( get_card_instance(player, card)->info_slot > 0 && trigger_condition == TRIGGER_DEAL_DAMAGE && affect_me(player, card) &&
			reason_for_trigger_controller == player ){
			if(event == EVENT_TRIGGER){
				event_result |= RESOLVE_TRIGGER_MANDATORY;
			}
			else if(event == EVENT_RESOLVE_TRIGGER){
					card_instance_t *instance = get_card_instance(player, card);
					int i;
					for(i=0;i<instance->info_slot;i++){
						if( in_play(instance->targets[i].player, instance->targets[i].card) ){
							kill_card(instance->targets[i].player, instance->targets[i].card, KILL_BURY);
						}
					}
					instance->info_slot = 0;
			}
		}
	}

	return global_enchantment(player, card, event);
}

int card_diabolic_edict(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.zone = TARGET_ZONE_PLAYERS;

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			card_instance_t *instance = get_card_instance(player, card);

			test_definition_t test;
			new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");
			new_sacrifice(player, card, instance->targets[0].player, SAC_CAUSED | SAC_NO_CANCEL, &test);
		}
		kill_card(player, card, KILL_DESTROY );
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);

}

int card_dirtcowl_wurm(int player, int card, event_t event){

	if( in_play(player, card) && ! is_humiliated(player, card) ){
		if( whenever_a_player_plays_a_land(player, card, event, 1-player, RESOLVE_TRIGGER_MANDATORY) ){
			add_1_1_counter(player, card);
		}
	}

	return 0;
}

int card_dismiss2(int player, int card, event_t event){
	/*
	  Dismiss |2|U|U
	  Instant
	  Counter target spell.
	  Draw a card.
	*/

	if( event == EVENT_RESOLVE_SPELL ){
		if( counterspell_validate(player, card, NULL, 0) ){
			card_instance_t* instance = get_card_instance(player, card);
			real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
			draw_cards(player, 1);
		}
		kill_card(player, card, KILL_DESTROY);
		return 0;
	}

	return counterspell(player, card, event, NULL, 0);
}

int card_disturbed_burial(int player, int card, event_t event)
{
  // Buyback |3
  // Return target creature card from your graveyard to your hand.

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_CREATURE, "Select a creature card.");

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_GRAVE_RECYCLER, NULL, NULL, 1, &this_test);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_GRAVE_RECYCLER, NULL, NULL, 1, &this_test);
		if( spell_fizzled != 1 ){
			get_card_instance(player, card)->info_slot = buyback(player, card, MANACOST_X(3));
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int selected = validate_target_from_grave(player, card, player, 0);
		if( selected != -1 ){
			from_grave_to_hand(player, selected, TUTOR_HAND);
			if( get_card_instance(player, card)->info_slot == 1 ){
				bounce_permanent(player, card);
				return 0;
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_dracoplasm(int player, int card, event_t event){

   if( event == EVENT_RESOLVE_SPELL ){
	   card_instance_t *instance = get_card_instance(player, card);

	   target_definition_t td;
	   default_target_definition(player, card, &td, TYPE_CREATURE);
	   td.preferred_controller = player;
	   td.allowed_controller = player;
	   td.illegal_abilities = 0;

	   add_state(player, card, STATE_OUBLIETTED);
	   instance->targets[1].player = 0;
	   instance->targets[1].card = 0;

	   test_definition_t test;
	   new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");
	   while( can_target(&td) ){
		   int sac = new_sacrifice(player, card, player, SAC_JUST_MARK|SAC_AS_COST|SAC_RETURN_CHOICE, &test);
		   if (!sac){
			   break;
		   }
		   instance->targets[1].player += get_power(BYTE2(sac), BYTE3(sac));
		   instance->targets[1].card += get_toughness(BYTE2(sac), BYTE3(sac));
		   kill_card(BYTE2(sac), BYTE3(sac), KILL_SACRIFICE);
	   }
	   remove_state(player, card, STATE_OUBLIETTED);
   }

   if( event == EVENT_POWER && affect_me(player, card) ){
	   card_instance_t *instance = get_card_instance(player, card);
	   event_result += (instance->targets[1].player > -1 ? instance->targets[1].player : 0);
   }

   if( event == EVENT_TOUGHNESS && affect_me(player, card) ){
	   card_instance_t *instance = get_card_instance(player, card);
	   event_result += (instance->targets[1].card > -1 ? instance->targets[1].card : 0);
   }

	return generic_shade(player, card, event, 0, MANACOST_R(1), 1, 0, 0, 0);
}

int card_dregs_of_sorrow(int player, int card, event_t event){

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.illegal_color = get_sleighted_color_test(player, card, COLOR_TEST_BLACK);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return can_target(&td);
	}
	else if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
			int trgs = 0;
			instance->number_of_targets = 0;
			while( can_target(&td) && has_mana(player, COLOR_COLORLESS, trgs+1) ){
					if( new_pick_target(&td, get_sleighted_color_text(player, card, "Select target non%s creature", COLOR_BLACK), trgs, GS_LITERAL_PROMPT) ){
						state_untargettable(instance->targets[trgs].player, instance->targets[trgs].card, 1);
						trgs++;
					}
					else{
						break;
					}
			}

			if( trgs > 0 ){
				int k;
				for(k=0; k<trgs; k++){
					state_untargettable(instance->targets[k].player, instance->targets[k].card, 0);
				}
				charge_mana(player, COLOR_COLORLESS, trgs);
				if( spell_fizzled != 1 ){
					instance->info_slot = trgs;
				}
			}
			else{
				spell_fizzled = 1;
			}
	}
	else if( event == EVENT_RESOLVE_SPELL ){
			int k;
			int good = 0;
			for(k=0; k<instance->info_slot; k++){
				if( validate_target(player, card, &td, k) ){
					kill_card(instance->targets[k].player, instance->targets[k].card, KILL_DESTROY);
					good++;
				}
			}
			if( good > 0 ){
				draw_cards(player, instance->info_slot);
			}
			kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_earthcraft(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.allowed_controller = player;
	td.preferred_controller = player;
	td.illegal_state = TARGET_STATE_TAPPED;

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_LAND);
	td1.preferred_controller = player;
	td1.required_subtype = SUBTYPE_BASIC;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE && generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST0, 0, &td, NULL) ){
		return can_target(&td1);
	}

	if(event == EVENT_ACTIVATE ){
		if( charge_mana_for_activated_ability(player, card, MANACOST0) ){
			int creature_to_tap = -1;
			if( new_pick_target(&td, "Select a creature to tap.", 0, 1 | GS_LITERAL_PROMPT) ){
				add_state(instance->targets[0].player, instance->targets[0].card, STATE_TARGETTED);
				creature_to_tap = instance->targets[0].card;
				instance->number_of_targets = 0;
				if( new_pick_target(&td1, "Select a basic land to untap.", 0, 1 | GS_LITERAL_PROMPT) ){
					tap_card(player, creature_to_tap);
				}
				remove_state(player, creature_to_tap, STATE_TARGETTED);
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td1) ){
			untap_card( instance->targets[0].player, instance->targets[0].card);
		}
	}

	return 0;
}

int card_echo_chamber(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = 1-player;
	td.allowed_controller = 1-player;
	td.who_chooses = 1-player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			token_generation_t token;
			copy_token_definition(player, card, &token, instance->targets[0].player, instance->targets[0].card);
			token.legacy = 1;
			token.special_code_for_legacy = &haste_and_remove_eot;
			generate_token(&token);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET|GAA_CAN_SORCERY_BE_PLAYED, MANACOST_X(4), 0,
									&td, "TARGET_CREATURE");
}

int vineyard_effect(int player, int card, event_t event){

	if (event == EVENT_PHASE_CHANGED && current_phase == PHASE_MAIN1 ){
		if( get_card_instance(player, card)->targets[1].player != 1 ){
			produce_mana(current_turn, COLOR_GREEN, 2);
			get_card_instance(player, card)->targets[1].player = 1;
		}
	}

	if(event == EVENT_PHASE_CHANGED && current_phase != PHASE_MAIN1){
		get_card_instance(player, card)->targets[1].player = 0;
	}

	return 0;
}

int card_eladamris_vineyard(int player, int card, event_t event){

	vineyard_effect(player, card, event);

	return global_enchantment(player, card, event);
}

int card_eladamri_lord_of_leaves(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	boost_creature_type(player, card, event, SUBTYPE_ELF, 0, 0, KEYWORD_SHROUD | get_hacked_walk(player, card, KEYWORD_FORESTWALK), 0);

	return 0;
}

int card_elite_javelineers(int player, int card, event_t event){
	/*
	  Elite Javelineer |2|W
	  Creature - Human Soldier 2/2
	  Whenever Elite Javelineer blocks, it deals 1 damage to target attacking creature.
	*/

	if( blocking(player, card, event) && ! is_humiliated(player, card) ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.required_state = TARGET_STATE_ATTACKING;
		td.allow_cancel = 0;
		if( can_target(&td) && new_pick_target(&td, "Select target attacking creature.", 0, GS_LITERAL_PROMPT) ){
			damage_target0(player, card, 1);
		}
		get_card_instance(player, card)->number_of_targets = 0;
	}

	return 0;
}


int card_elven_warhounds(int player, int card, event_t event)
{
  // Whenever ~ becomes blocked by a creature, put that creature on top of its owner's library.
  if (event == EVENT_CHANGE_TYPE && affect_me(player, card) && current_turn == player && !is_humiliated(player, card))
	get_card_instance(player, card)->destroys_if_blocked |= DIFB_DESTROYS_ALL;

  if (event == EVENT_DECLARE_BLOCKERS && is_attacking(player, card) && !is_humiliated(player, card))
	{
	  char marked[2][151] = {{0}};
	  mark_each_creature_blocking_me(player, card, marked);
	  int c;
	  for (c = 0; c < active_cards_count[1-current_turn]; ++c)
		if (marked[1-current_turn][c] && in_play(1-current_turn, c))
		  put_on_top_of_deck(1-current_turn, c);
	}

  return 0;
}

int card_elvish_fury(int player, int card, event_t event){

	if( !IS_GS_EVENT(player, card, event)){
		return vanilla_instant_pump(player, card, event, ANYBODY, player, 2, 2, 0, 0);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
		if( spell_fizzled != 1 ){
			instance->info_slot = buyback(player, card, MANACOST_X(4));
		}
	}

	if(event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 2, 2);
			if( instance->info_slot == 1 ){
				bounce_permanent(player, card);
				return 0;
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_emerald_medallion(int player, int card, event_t event){
/* CARD_ID_EMERALD_MEDALLION	3132
Emerald Medallion	|2
Artifact
|SGreen spells you cast cost |1 less to cast. */

	return tempest_medallion(player, card, event, get_sleighted_color_test(player, card, COLOR_TEST_GREEN));
}

int card_emessi_tome(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_ACTIVATION ){
		draw_cards(player, 2);
		discard(player, 0, player);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_X(5), 0, NULL, NULL);
}

int card_essence_bottle(int player, int card, event_t event)
{
  /* Essence Bottle	|2
   * Artifact
   * |3, |T: Put an elixir counter on ~.
   * |T, Remove all elixir counters from ~: You gain 2 life for each elixir counter removed this way. */

  if (IS_ACTIVATING(event))
	{
	  if (event == EVENT_CAN_ACTIVATE && !(CAN_TAP(player, card) && can_use_activated_abilities(player, card)))
		return 0;

	  enum
	  {
		CHOICE_COUNTER = 1,
		CHOICE_LIFE
	  } choice = DIALOG(player, card, event,
						DLG_RANDOM,
						"Add a counter", 1, 3, DLG_MANA(MANACOST_X(3)),
						"Gain life", 1, count_counters(player, card, COUNTER_ELIXIR), DLG_MANA(MANACOST_X(0)));

	  if (event == EVENT_CAN_ACTIVATE)
		return choice;
	  else if (event == EVENT_ACTIVATE)
		{
		  card_instance_t* instance = get_card_instance(player, card);
		  switch (choice)
			{
			  case CHOICE_LIFE:
				instance->state |= STATE_TAPPED;
				instance->eot_toughness = count_counters(player, card, COUNTER_ELIXIR);
				remove_all_counters(player, card, COUNTER_ELIXIR);
				ai_modifier -= 32 + 2 * life[player];
				break;

			  case CHOICE_COUNTER:
				instance->state |= STATE_TAPPED;
				if (current_turn != player && current_phase == PHASE_DISCARD)
				  ai_modifier += 64;
				break;
			}
		}
	  else	// EVENT_RESOLVE_ACTIVATION
		switch (choice)
		  {
			case CHOICE_COUNTER:
			  add_counter(player, card, COUNTER_ELIXIR);
			  break;

			case CHOICE_LIFE:
			  gain_life(player, 2 * get_card_instance(player, card)->eot_toughness);
			  break;
		  }
	}

  return 0;
}

int card_energizer(int player, int card, event_t event){

	if( event == EVENT_POW_BOOST || event == EVENT_TOU_BOOST ){
		if( generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, MANACOST_X(2), 0, NULL, NULL) ){
			return 1;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		add_1_1_counter(instance->parent_controller, instance->parent_card);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_X(2), 0, NULL, NULL);
}

int card_escaped_shapeshifter(int player, int card, event_t event){

	if( event == EVENT_ABILITIES && affect_me(player, card) && ! is_humiliated(player, card) ){
		int keyword_mask = 	KEYWORD_FLYING | KEYWORD_FIRST_STRIKE | KEYWORD_TRAMPLE | KEYWORD_PROT_BLACK | KEYWORD_PROT_BLUE |
							KEYWORD_PROT_GREEN | KEYWORD_PROT_RED | KEYWORD_PROT_RED;
		int i;
		int keyword = 0;
		for(i=0; i<active_cards_count[1-player]; i++){
			if( in_play(1-player, i) && is_what(1-player, i, TYPE_CREATURE) && get_id(1-player, i) != get_id(player, card) ){
				int opp_key = get_abilities(1-player, i, EVENT_ABILITIES, -1);
				keyword |= (opp_key & keyword_mask);
			}
			if( keyword == keyword_mask ){
				break;
			}
		}
		event_result |= keyword;
	}

	return 0;
}

int card_evincars_justice(int player, int card, event_t event){

	if( event == EVENT_CAN_CAST ){
		return basic_spell(player, card, event);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		basic_spell(player, card, event);
		if( spell_fizzled != 1 ){
			get_card_instance(player, card)->info_slot = buyback(player, card, MANACOST_X(3));
		}
	}
	if( event == EVENT_RESOLVE_SPELL ){
		new_damage_all(player, card, ANYBODY, 2, NDA_ALL_CREATURES | NDA_PLAYER_TOO, NULL);
		if( get_card_instance(player, card)->info_slot == 1 ){
			bounce_permanent(player, card);
		}
		else{
			kill_card(player, card, KILL_DESTROY);
		}
	}

	return 0;
}

int card_extinction(int player, int card, event_t event){

	if(event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.subtype = select_a_subtype(player, card);

		new_manipulate_all(player, card, 2, &this_test, KILL_DESTROY);

		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_feverd_convulsion(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			add_minus1_minus1_counters(instance->targets[0].player, instance->targets[0].card, 1);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_XB(2, 2), 0, &td, "TARGET_CREATURE");
}

static int field_of_souls_effect(int player, int card, event_t event){

	if( event == EVENT_STATIC_EFFECTS && affect_me(player, card) ){
		card_instance_t *instance = get_card_instance(player, card);
		if( instance->damage_target_player > -1 ){
			if( get_id(instance->damage_target_player, instance->damage_target_card) != CARD_ID_FIELD_OF_SOULS ){
				kill_card(player, card, KILL_EXILE);
			}
		}
	}

	if( get_card_instance(player, card)->damage_target_player > -1 && effect_follows_control_of_attachment(player, card, event) ){
		return 0;
	}

	if( event == EVENT_GRAVEYARD_FROM_PLAY ){
		card_instance_t *instance = get_card_instance(player, card);
		if( in_play(affected_card_controller, affected_card) ){
			int kill_code = get_card_instance(affected_card_controller, affected_card)->kill_code;
			if( kill_code ){
				if( instance->damage_target_player > -1 ){
					int p = instance->damage_target_player;
					int c = instance->damage_target_card;
					if( affect_me(p, c) ){
						instance->damage_target_player = instance->damage_target_card = -1;
						remove_status(player, card, STATUS_INVISIBLE_FX);
						if( instance->targets[11].player < 0 ){
							instance->targets[11].player = 0;
						}
						SET_BYTE1(instance->targets[11].player) = 1;
					}
				}
				if( ! is_token(affected_card_controller, affected_card) && get_owner(affected_card_controller, affected_card) == player ){
					count_for_gfp_ability(player, card, event, ANYBODY, TYPE_CREATURE, NULL);
				}
			}
		}
	}

	if( ! check_status(player, card, STATUS_INVISIBLE_FX) ){
		int result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
		if( result ){
			token_generation_t token;
			default_token_definition(player, card, CARD_ID_SPIRIT, &token);
			token.color_forced = get_sleighted_color_test(player, card, COLOR_TEST_WHITE);
			token.key_plus = KEYWORD_FLYING;
			token.qty = BYTE0(result);
			generate_token(&token);

			kill_card(player, card, KILL_EXILE);
		}
	}

	//Reset counts if the legacy is still invisible
	if( trigger_condition == TRIGGER_GRAVEYARD_FROM_PLAY && event == EVENT_END_TRIGGER ){
		get_card_instance(player, card)->targets[11].player = 0;
	}

	if( event == EVENT_CLEANUP && ! check_status(player, card, STATUS_INVISIBLE_FX) ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int card_field_of_souls(int player, int card, event_t event){
	/* Field of Souls	|2|W|W
	 * Enchantment
	 * Whenever a nontoken creature is put into your graveyard from the battlefield, put a 1/1 |Swhite Spirit creature token with flying onto the battlefield. */

	if( event == EVENT_STATIC_EFFECTS && affect_me(player, card) ){
		int found = 0;
		int c;
		for(c=0; c<active_cards_count[player]; c++){
			if( in_play(player, c) && is_what(player, c, TYPE_EFFECT) ){
				card_instance_t *inst = get_card_instance(player, c);
				if( inst->info_slot == (int)field_of_souls_effect ){
					if( (inst->damage_target_player == player && inst->damage_target_card == card) ||
						(inst->targets[0].player == player && inst->targets[0].card == card) )
					{
						found = 1;
						break;
					}
				}
			}
		}
		if( ! found ){
			int legacy = create_targetted_legacy_effect(player, card, &field_of_souls_effect, player, card);
			card_instance_t *inst = get_card_instance(player, legacy);
			inst->targets[0].player = player;
			inst->targets[0].card = card;
			inst->number_of_targets = 1;
			add_status(player, legacy, STATUS_INVISIBLE_FX);
		}
	}

	if( event == EVENT_GRAVEYARD_FROM_PLAY && ! is_token(affected_card_controller, affected_card) &&
		get_owner(affected_card_controller, affected_card) == player )
	{
		count_for_gfp_ability(player, card, event, ANYBODY, TYPE_CREATURE, NULL);
	}

	if( get_card_instance(player, card)->kill_code <= 0 ){
		int result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
		if( result ){
			token_generation_t token;
			default_token_definition(player, card, CARD_ID_SPIRIT, &token);
			token.color_forced = get_sleighted_color_test(player, card, COLOR_TEST_WHITE);
			token.key_plus = KEYWORD_FLYING;
			token.qty = BYTE0(result);
			generate_token(&token);
		}
	}

	return global_enchantment(player, card, event);
}

int card_flickering_ward(int player, int card, event_t event){

	if( ! is_humiliated(player, card) && get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);
		if( event == EVENT_ABILITIES && instance->targets[1].card > -1 &&
			affect_me(instance->damage_target_player, instance->damage_target_card) )
		{
			event_result |= instance->targets[1].card;
		}
		if( event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE ){
			return generic_activated_ability(player, card, event, 0, MANACOST_W(1), 0, NULL, NULL);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		bounce_permanent(instance->parent_controller, instance->parent_card);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		get_card_instance(player, card)->targets[1].card = select_a_protection(player);
	}

	return vanilla_aura(player, card, event, player);
}

int card_flowstone_sculpture(int player, int card, event_t event){

	if( event == EVENT_ABILITIES && affect_me(player, card) && get_card_instance(player, card)->targets[1].card > 0 &&
		! is_humiliated(player, card) )
	{
		event_result |= get_card_instance(player, card)->targets[1].card;
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_DISCARD, MANACOST_X(2), 0, NULL, NULL);
	}

	if( event == EVENT_ACTIVATE ){
		int ai_choice = 0;
		if( current_phase < PHASE_DECLARE_BLOCKERS ){
			ai_choice = 1;
		}
		if( current_phase == PHASE_AFTER_BLOCKING ){
			int my_pow = get_power(player, card);
			int my_tou = get_toughness(player, card);
			if( is_attacking(player, card) && ! is_unblocked(player, card) ){
				int count = 0;
				int opp_pow = 0;
				int opp_tou = 0;
				while( count < active_cards_count[1-player] ){
						if( in_play(1-player, count) && is_what(1-player, count, TYPE_CREATURE) ){
							card_instance_t *this = get_card_instance(1-player, count);
							if( this->blocking == card ){
								opp_pow = get_power(1-player, count);
								opp_tou = get_toughness(1-player, count);
							}
						}
						count++;
				}
				if( my_tou > opp_pow && my_pow > opp_tou && ! check_for_ability(player, card, KEYWORD_TRAMPLE) ){
					ai_choice = 3;
				}
				if( my_tou <= opp_pow && my_pow >= opp_tou && ! check_for_ability(player, card, KEYWORD_FIRST_STRIKE) ){
					ai_choice = 2;
				}
			}
			if( current_turn != player && instance->blocking < 255 ){
				if( get_toughness(1-player, instance->blocking) <= my_pow && ! check_for_ability(player, card, KEYWORD_FIRST_STRIKE) ){
					ai_choice = 2;
				}
			}
		}
		int choice = do_dialog(player, player, card, -1, -1, " Add +1/+1 counter\n Gains flying\n Gains first strike\n Gains trample\n Cancel", ai_choice);
		if( choice != 4 ){
			if( charge_mana_for_activated_ability(player, card, MANACOST_X(2)) ){
				discard(player, 0, player);
				instance->info_slot = 66+choice;
			}
		}
		else{
			spell_fizzled = 1;
		}
	}
	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *par = get_card_instance(instance->parent_controller, instance->parent_card);
		if( par->targets[1].card < 0 ){
			par->targets[1].card = 0;
		}
		if( instance->info_slot == 66 ){
			add_1_1_counter(instance->parent_controller, instance->parent_card);
		}
		if( instance->info_slot == 67 ){
			par->targets[1].card |= KEYWORD_FLYING;
		}
		if( instance->info_slot == 68 ){
			par->targets[1].card |= KEYWORD_FIRST_STRIKE;
		}
		if( instance->info_slot == 69 ){
			par->targets[1].card |= KEYWORD_TRAMPLE;
		}
	}

	return 0;
}

int card_fools_tome(int player, int card, event_t event){

	if( event == EVENT_CAN_ACTIVATE && hand_count[player] ){
		return 0;
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		draw_cards(player, 1);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_X(2), 0, NULL, NULL);
}

int card_furnace_of_rath(int player, int card, event_t event)
{
	/* Bitter Feud	|4|R	0x2001da8
	 * Enchantment
	 * As ~ enters the battlefield, choose two players.
	 * If a source controlled by one of the chosen players would deal damage to the other chosen player or a permanent that player controls, that source deals double that damage to that player or permanent instead. */
	/* Furnace of Rath	|1|R|R|R	0x2001DA8
	 * Enchantment
	 * If a source would deal damage to a permanent or player, it deals double that damage to that permanent or player instead. */
	card_instance_t* damage = damage_being_dealt(event);
	if( damage && in_play(player, card) && ! is_humiliated(player, card) ){
		damage->info_slot *= 2;
	}

	return global_enchantment(player, card, event);
}

int card_gerrards_battle_cry(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		pump_subtype_until_eot(instance->parent_controller, instance->parent_card, player, -1, 1, 1, 0, 0);
	}

	return generic_activated_ability(player, card, event, 0, MANACOST_XW(2, 1), 0, NULL, NULL);
}

int card_goblin_bombardment(int player, int card, event_t event){
	/* Goblin Bombardment	|1|R	0x2003478
	 * Enchantment
	 * Sacrifice a creature: ~ deals 1 damage to any target. */
	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td1.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td1) ){
			damage_target0(player, card, 1);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_SACRIFICE_CREATURE, MANACOST0, 0,
									&td1, "TARGET_ANY");
}

int card_grindstone(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_PERMANENT);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			int *deck = deck_ptr[instance->targets[0].player];
			int to_mill = 2;
			if( get_global_color_hack(2) ){
				to_mill = count_deck(instance->targets[0].player);
			}
			else{
				int count = 0;
				while( deck[count] != -1 ){
						int c1 = get_color_by_internal_id_no_hack( deck[count] );
						int c2 = get_color_by_internal_id_no_hack( deck[count+1] );
						if(  (c1 & c2) && (c1 != COLOR_TEST_COLORLESS && c2 != COLOR_TEST_COLORLESS) ){
							to_mill +=2;
						}
						else{
							break;;
						}
						count +=2;
				}
				if( to_mill > count_deck(instance->targets[0].player) ){
					to_mill = count_deck(instance->targets[0].player);
				}
			}
			mill(instance->targets[0].player, to_mill);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_X(3), 0, &td, "TARGET_PLAYER");
}

int card_hannas_custody(int player, int card, event_t event){

	if( event == EVENT_ABILITIES && ! is_humiliated(player, card) && in_play(player, card) ){
		if( is_what(affected_card_controller, affected_card, TYPE_ARTIFACT) ){
			event_result |= KEYWORD_SHROUD;
		}
	}

	return global_enchantment(player, card, event);
}

int card_heart_sliver(int player, int card, event_t event)
{
  // All Sliver creatures have haste.
  boost_subtype(player, card, event, SUBTYPE_SLIVER, 0,0, 0,SP_KEYWORD_HASTE, BCT_INCLUDE_SELF);

  return slivercycling(player, card, event);
}

int card_heartwood_giant(int player, int card, event_t event){
	/* Heartwood Giant	|3|G|G	0x200a048
	 * Creature - Giant 4/4
	 * |T, Sacrifice |Ha Forest: ~ deals 2 damage to target player or planeswalker. */
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	default_test_definition(&test, TYPE_LAND);
	test.subtype = get_hacked_subtype(player, card, SUBTYPE_FOREST);

	target_definition_t td;
	default_target_definition(player, card, &td, TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_ACTIVATE ){
		scnprintf(test.message, 100, "%s", get_hacked_land_text(player, card, "Select a %s to sacrifice", SUBTYPE_FOREST));
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			damage_target0(player, card, 2);
		}
	}

	return generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST0, 0,
															&td, "TARGET_PLAYER_OR_PLANESWALKER", 0, &test, 0);
}

int card_helm_of_possession(int player, int card, event_t event){

	/* Helm of Possession	|4
	 * Artifact
	 * You may choose not to untap ~ during your untap step.
	 * |2, |T, Sacrifice a creature: Gain control of target creature for as long as you control ~ and ~ remains tapped. */

	choose_to_untap(player, card, event);

	if (!IS_GAA_EVENT(event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	if (event == EVENT_RESOLVE_ACTIVATION && valid_target(&td)){
		card_instance_t* instance = get_card_instance(player, card);
		gain_control_until_source_is_in_play_and_tapped(player, card, instance->targets[0].player, instance->targets[0].card,
														GCUS_CONTROLLED | GCUS_TAPPED);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET|GAA_SACRIFICE_CREATURE, MANACOST_X(2), 0,
									&td, "TARGET_CREATURE");
}

int card_horned_sliver(int player, int card, event_t event){
	boost_creature_type(player, card, event, SUBTYPE_SLIVER, 0, 0, KEYWORD_TRAMPLE, BCT_INCLUDE_SELF);
	return slivercycling(player, card, event);
}

int card_humility(int player, int card, event_t event){
	// See also abilities.c:humility_legacy()

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		new_manipulate_all(player, card, ANYBODY, &this_test, ACT_HUMILIATE);
	}

	if( event == EVENT_CAST_SPELL && is_what(affected_card_controller, affected_card, TYPE_CREATURE) ){
		humiliate(player, card, affected_card_controller, affected_card, 1);
	}

	if( leaves_play(player, card, event) ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		new_manipulate_all(player, card, 2, &this_test, ACT_DE_HUMILIATE);
	}

	if( event == EVENT_POWER || event == EVENT_TOUGHNESS || event == EVENT_ABILITIES ){
		if( !in_play(player, card) || ! in_play(affected_card_controller, affected_card) )
			return 0;

		if (!is_what(affected_card_controller, affected_card, TYPE_CREATURE))
			return 0;

		if (event == EVENT_POWER)
			event_result += (1 - get_base_power(affected_card_controller, affected_card));
		else if (event == EVENT_TOUGHNESS)
			event_result += (1 - get_base_toughness(affected_card_controller, affected_card));
		else if (event == EVENT_ABILITIES){
			event_result &= (KEYWORD_RECALC_CHANGE_TYPE|KEYWORD_RECALC_TOUGHNESS|KEYWORD_RECALC_POWER|KEYWORD_RECALC_ABILITIES|KEYWORD_RECALC_SET_COLOR);
			card_instance_t* aff = get_card_instance(player, card);
			aff->state &= ~STATE_VIGILANCE;
			// Probably appropriate to remove more, but these are the only ones that give icons, so meh.
			if (aff->targets[16].card != -1)
				aff->targets[16].card &= ~(SP_KEYWORD_FEAR|SP_KEYWORD_INTIMIDATE|SP_KEYWORD_UNBLOCKABLE|SP_KEYWORD_DEATHTOUCH|SP_KEYWORD_LIFELINK
										   |SP_KEYWORD_INDESTRUCTIBLE|SP_KEYWORD_SHADOW|SP_KEYWORD_HEXPROOF|SP_KEYWORD_HORSEMANSHIP|SP_KEYWORD_VIGILANCE);
		}
	}

	return global_enchantment(player, card, event);
}

int card_intuition(int player, int card, event_t event){

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += 96;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int cards = MIN(count_deck(player), 3);
		if( cards > 0 ){
			int i;
			int chosen[3] = {-1, -1, -1};
			int *deck = deck_ptr[player];
			for(i=0; i<cards; i++){
				int selected = pick_card_from_deck(player);
				chosen[i] = deck[selected];
				remove_card_from_deck(player, selected);
			}

			int card_to_add = -1;
			if( !IS_AI(1-player) ){
				while( card_to_add == -1 ){
						card_to_add = show_deck(1-player, chosen, 3, "Choose a card", 0, 0x7375B0 );
				}
			}
			else{
				i=0;
				int max_value = 1000;
				for(i=0; i<cards; i++){
					card_ptr_t* c = cards_ptr[ cards_data[chosen[i]].id ];
					if( c->ai_base_value < max_value ){
						max_value = c->ai_base_value;
						card_to_add = i;
					}
				}
			}

			add_card_to_hand(player, chosen[card_to_add]);
			chosen[card_to_add] = -1;
			int num_unchosen = 0;
			for (i = 0; i < cards; ++i){
				if( chosen[i] != -1 ){
					raw_put_iid_on_top_of_deck(player, chosen[i]);
					++num_unchosen;
				}
			}
			mill(player, num_unchosen);
		}
		shuffle(player);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

static const char* not_effect_from_monotype_planeswalker(int who_chooses, int player, int card)
{
  /* This is the first card I've seen that can target all of artifact, creature, enchantment, and land, but not planeswalker.  Putting TARGET_TYPE_PLANESWALKER
   * into td.illegal_type is tempting, but won't work - it'll fail for an animated Gideon Jura, a planeswalker-artifact (by Memnarch), or planeswalker-
   * enchantment (by Enchanted Evening). */
  card_instance_t* eff = get_card_instance(player, card);
  if (!is_what(-1, eff->original_internal_card_id, TYPE_ARTIFACT|TYPE_CREATURE|TYPE_LAND)
	  && is_planeswalker(-1, eff->original_internal_card_id)
	  && !check_special_flags2(eff->parent_controller, eff->parent_card, SF2_ENCHANTED_EVENING))	// works properly even if parent card has left play by now
	return "type";

  return NULL;
}

static int interdict_legacy(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance( player, card );
	if( instance->targets[0].player > -1 ){
		if( eot_trigger(player, card, event) ){
			disable_all_activated_abilities(instance->targets[0].player, instance->targets[0].card, 0);
			kill_card(player, card, KILL_EXILE);
		}
	}
	return 0;
}

int card_interdict(int player, int card, event_t event)
{
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

  target_definition_t td;
  counter_activated_target_definition(player, card, &td, TYPE_ARTIFACT|TYPE_CREATURE|TYPE_ENCHANTMENT|TYPE_LAND);
  td.extra = (int32_t)not_effect_from_monotype_planeswalker;
  td.special |= TARGET_SPECIAL_EXTRA_FUNCTION;

  card_instance_t *instance = get_card_instance( player, card );

  if (event == EVENT_CAN_CAST)
	return can_counter_activated_ability(player, card, event, &td);

  if (event == EVENT_CAST_SPELL && affect_me(player, card))
	{
	  instance->number_of_targets = 0;
	  return cast_counter_activated_ability(player, card, 0);
	}

  if (event == EVENT_RESOLVE_SPELL)
	{
	  if (resolve_counter_activated_ability(player, card, &td, 0))
		{
		  card_instance_t* tgt = get_card_instance(instance->targets[0].player, instance->targets[0].card);
		  disable_all_activated_abilities(tgt->parent_controller, tgt->parent_card, 1);
		  create_targetted_legacy_effect(player, card, &interdict_legacy, tgt->parent_controller, tgt->parent_card);
		  draw_cards(player, 1);
		}
	  kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_invulnerability(int player, int card, event_t event){

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.special = TARGET_SPECIAL_DAMAGE_PLAYER;
	td.extra = damage_card;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_DAMAGE_PREVENTION | GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_DAMAGE_PREVENTION | GS_CAN_TARGET, &td, "TARGET_DAMAGE", 1, NULL);
		if( spell_fizzled != 1 ){
			instance->info_slot = buyback(player, card, MANACOST_X(3));
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			prevent_damage(instance->targets[0].player, instance->targets[0].card, -1);
			if( instance->info_slot == 1 ){
				bounce_permanent(player, card);
				return 0;
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_jackal_pup(int player, int card, event_t event){
	if ( ! is_humiliated(player, card) && damage_dealt_to_me_arbitrary(player, card, event, 0, player, card)){
		card_instance_t* instance = get_card_instance(player, card);
		damage_player(player, instance->targets[7].card, player, card);
	}
	return 0;
}

int card_jet_medallion(int player, int card, event_t event){
/* CARD_ID_JET_MEDALLION	3180
Jet Medallion	|2
Artifact
|SBlack spells you cast cost |1 less to cast. */

	return tempest_medallion(player, card, event, get_sleighted_color_test(player, card, COLOR_TEST_BLACK));
}

int card_kezzerdrix(int player, int card, event_t event){

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		if( count_subtype(1-player, TYPE_CREATURE, -1) < 1 ){
			damage_player(player, 4, player, card);
		}
	}

	return 0;
}

int card_kindle(int player, int card, event_t event){
	/* Flame Burst	|1|R	0x200349b
	 * Instant
	 * ~ deals X damage to any target, where X is 2 plus the number of cards named ~ in all graveyards. */
	/* Kindle	|1|R	0x200349b
	 * Instant
	 * ~ deals X damage to any target, where X is 2 plus the number of cards named ~ in all graveyards. */
	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			int amount = 2;
			amount+= count_graveyard_by_id( player, get_id(player, card) );
			amount+= count_graveyard_by_id( 1-player, get_id(player, card) );
			damage_target0(player, card, amount);
		}
		kill_card(player, card, KILL_DESTROY);
	}
	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ANY", 1, NULL);
}

int card_knight_of_dawn(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		pump_ability_until_eot(instance->parent_controller, instance->parent_card, instance->parent_controller, instance->parent_card,
								0, 0, select_a_protection(player) | KEYWORD_RECALC_SET_COLOR, 0);
	}

	return generic_activated_ability(player, card, event, 0, MANACOST_W(2), 0, NULL, NULL);
}

static const char* is_blocking_kod(int who_chooses, int player, int card, int targeting_player, int targeting_card)
{
  if (is_blocking(card, targeting_card))
	return NULL;
  else
	return "must be blocking Knight of Dusk";
}

int card_knight_of_dusk(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td2;
	default_target_definition(player, card, &td2, TYPE_CREATURE);
	td2.allowed_controller =  1-player;
	td2.preferred_controller = 1-player;
	td2.special = TARGET_SPECIAL_EXTRA_FUNCTION;
	td2.extra = (int32_t)is_blocking_kod;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td2) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST_B(2), 0,
									&td2, "Select target creature blocking Knight of Dusk.");
}

int card_krakilin(int player, int card, event_t event){

	if( IS_GAA_EVENT(event) ){
		regeneration(player, card, event, MANACOST_XG(1, 1));
	}

	return card_ivy_elemental(player, card, event);
}

int card_legacys_allure(int player, int card, event_t event){

	/* Legacy's Allure	|U|U
	 * Enchantment
	 * At the beginning of your upkeep, you may put a treasure counter on ~.
	 * Sacrifice ~: Gain control of target creature with power less than or equal to the number of treasure counters on ~. */

	upkeep_trigger_ability_mode(player, card, event, player, RESOLVE_TRIGGER_AI(player));

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		add_counter(player, card, COUNTER_TREASURE);
	}

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	card_instance_t *instance = get_card_instance( player, card);

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = 1-player;
	td.allowed_controller = 1-player;
	td.power_requirement = count_counters(player, card, COUNTER_TREASURE) | TARGET_PT_LESSER_OR_EQUAL;

	if(event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			gain_control(player, card, instance->targets[0].player, instance->targets[0].card);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_SACRIFICE_ME, MANACOST0, 0, &td, "TARGET_CREATURE");
}

int card_lightning_blast(int player, int card, event_t event){
	/* Flame Lash	|3|R	0x200cce4
	 * Instant
	 * ~ deals 4 damage to any target. */
	/* Lightning Blast	|3|R	0x200cce4
	 * Instant
	 * ~ deals 4 damage to any target. */
	/* Unfriendly Fire	|4|R	0x200cce4
	 * Instant
	 * ~ deals 4 damage to any target. */
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if(event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_target0(player, card, 4);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ANY", 1, NULL);
}

int card_living_death(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		new_reanimate_all(player, card, ANYBODY, &this_test, REANIMATE_SPECIAL_LIVING_DEAD);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_lobotomy(int player, int card, event_t event){

	/* Lobotomy	|2|U|B
	 * Sorcery
	 * Target player reveals his or her hand, then you choose a card other than a basic land card from it. Search that player's graveyard, hand, and library for
	 * all cards with the same name as the chosen card and exile them. Then that player shuffles his or her library. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	if(event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			int opponent = get_card_instance(player, card)->targets[0].player;
			if(hand_count[opponent]==0){
				kill_card(player, card, KILL_DESTROY);
				return 0;
			}
			int cards_array[ 500 ];
			int i=0;
			int hand_index = 0;
			int AI_pick = -1;
			int min_value = 0;
			for(i=0;i<active_cards_count[opponent]; i++){
				card_instance_t *instance = get_card_instance(opponent, i);
				if( ! ( instance->state & STATE_INVISIBLE ) && ! ( instance->state & STATE_IN_PLAY )  ){
					if( ! is_basic_land(opponent, i) ){
						card_ptr_t* c = cards_ptr[ get_id(opponent, i) ];
						if( c->ai_base_value > min_value ){
							min_value = c->ai_base_value;
							AI_pick = hand_index;
						}
					}
					cards_array[hand_index++] = instance->internal_card_id;
				}

			}

			// pick a card
			int selected = AI_pick;
			int valid_choice = 1;
			int id = -1;
			if( player == HUMAN ){
				selected = show_deck( player, cards_array, hand_count[opponent], "Pick a card", 0, 0x7375B0 );
				if( selected == -1 || is_basic_land_by_id(cards_data[cards_array[selected]].id) ){
					valid_choice = 0;
				}
				else{
					 id = cards_data[cards_array[selected]].id;
				}
			}

			if( valid_choice == 1 && id > -1 ){
				lobotomy_effect(player, 1-player, id, 1);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}
	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
}

int card_lotus_petal(int player, int card, event_t event)
{
  // 0x4dd5d1
  return artifact_mana_all_one_color(player, card, event, 1, 1);
}

int card_mana_severance(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		int choice = do_dialog(player, player, card, -1, -1, " Auto mode\n Manual mode", 0);
		if( choice == 0 ){
			int count = count_deck(player)-1;
			int *deck = deck_ptr[player];
			while( count > -1 ){
					if( cards_data[deck[count]].type & TYPE_LAND ){
						rfg_card_in_deck(player, count);
					}
					count--;
			}
		}
		else if( choice == 1 ){
				test_definition_t this_test;
				new_default_test_definition(&this_test, TYPE_LAND, "Select a land card to exile.");
				while( new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_RFG, 0, AI_MIN_VALUE, &this_test) != -1 ){
				}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_marble_titan(int player, int card, event_t event){

	if( event == EVENT_UNTAP && current_phase == PHASE_UNTAP && ! is_humiliated(player, card) ){
		if( is_what(affected_card_controller, affected_card, TYPE_CREATURE) &&
			get_power(affected_card_controller, affected_card) > 2 ){
			card_instance_t *instance= get_card_instance(affected_card_controller, affected_card);
			instance->untap_status &= ~3;
		}
	}

	return 0;
}

int card_meditate(int player, int card, event_t event)
{
  /* Meditate	|2|U
   * Instant
   * Draw four cards. You skip your next turn. */

	if (event == EVENT_RESOLVE_SPELL){
		draw_cards(player, 4);
		skip_next_turn(player, card, player);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int mindwhip_sliver_shared_ability(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);

	if( instance->targets[1].player > -1 ){
		int p = instance->targets[1].player;
		int c = instance->targets[1].card;

		if( leaves_play(p, c, event) ){
			add_status(player, card, STATUS_INVISIBLE_FX);
		}

		if( ! in_play(p, c) ){
			if( event == EVENT_CAN_ACTIVATE ){
				return 0;
			}

			if( event == EVENT_CLEANUP ){
				kill_card(player, card, KILL_EXILE);
			}
		}

		if( is_humiliated(p, c) ){
			return 0;
		}

		if( ! IS_GAA_EVENT(event) ){
			return 0;
		}

		target_definition_t td2;
		default_target_definition(player, card, &td2, 0);
		td2.zone = TARGET_ZONE_PLAYERS;

		if( event == EVENT_RESOLVE_ACTIVATION ){
			if( would_validate_arbitrary_target(&td2, instance->targets[2].player, instance->targets[2].card) ){
				discard(instance->targets[2].player, DISC_RANDOM, player);
			}
		}

		return shared_sliver_activated_ability(player, card, event, GAA_CAN_SORCERY_BE_PLAYED | GAA_SACRIFICE_ME | GAA_CAN_TARGET, MANACOST_X(2), 0,
												&td2, "TARGET_PLAYER");
	}

	return 0;
}

int card_mindwhip_sliver(int player, int card, event_t event){
	/*
	  Mindwhip Sliver |2|B
	  Creature - Sliver 2/2, 2B (3)
	  All Slivers have "{2}, Sacrifice this permanent: Target player discards a card at random. Activate this ability only any time you could cast a sorcery."
	*/

	if( event == EVENT_RESOLVE_SPELL ){
		int legacy = create_legacy_activate(player, card, &mindwhip_sliver_shared_ability);
		card_instance_t *instance = get_card_instance(player, legacy);
		instance->targets[1].player = player;
		instance->targets[1].card = card;
		instance->number_of_targets = 1;
		int fake = add_card_to_hand(1-player, get_card_instance(player, card)->internal_card_id);
		legacy = create_legacy_activate(1-player, fake, &mindwhip_sliver_shared_ability);
		instance = get_card_instance(1-player, legacy);
		instance->targets[1].player = player;
		instance->targets[1].card = card;
		instance->number_of_targets = 1;
		obliterate_card(1-player, fake);
	}
	return slivercycling(player, card, event);
}

int card_minion_of_the_wastes(int player, int card, event_t event){
	/* Minion of the Wastes	|3|B|B|B
	 * Creature - Minion 100/100
	 * Trample
	 * As ~ enters the battlefield, pay any amount of life.
	 * ~'s power and toughness are each equal to the life paid as it entered the battlefield. */

	if (card == -1){
		return 0;	// no life was paid if this isn't on the bf
	}

	card_instance_t *instance = get_card_instance( player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		int to_pay = life[player];
		if( player == AI ){
			to_pay = life[player]-7;
		}

		int amount = choose_a_number(player, "Pay how much life?", to_pay);
		if( amount > life[player] ){
			amount = life[player]-1;
		}
		if( amount < 0 ){
			amount = 0;
		}
		lose_life(player, amount);
		instance->targets[1].player = amount;
	}

	if( (event == EVENT_POWER || event == EVENT_TOUGHNESS) && affect_me(player, card) && ! is_humiliated(player, card) ){
		event_result+=instance->targets[1].player;
	}

	return 0;
}

int card_mirris_guile(int player, int card, event_t event){

	upkeep_trigger_ability_mode(player, card, event, player, RESOLVE_TRIGGER_AI(player));

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		rearrange_top_x(player, player, 3);
	}

	return global_enchantment(player, card, event);
}

int mnemonic_sliver_shared_ability(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);

	if( instance->targets[1].player > -1 ){
		int p = instance->targets[1].player;
		int c = instance->targets[1].card;

		if( leaves_play(p, c, event) ){
			add_status(player, card, STATUS_INVISIBLE_FX);
		}

		if( ! in_play(p, c) ){
			if( event == EVENT_CAN_ACTIVATE ){
				return 0;
			}

			if( event == EVENT_CLEANUP ){
				kill_card(player, card, KILL_EXILE);
			}
		}

		if( is_humiliated(p, c) ){
			return 0;
		}

		if( ! IS_GAA_EVENT(event) ){
			return 0;
		}

		if( event == EVENT_CAN_ACTIVATE && in_play(p, c) ){
			int pp = player;
			int cc;
			for(cc=0; cc < active_cards_count[pp]; cc++){
				if( in_play(pp, cc) && is_what(pp, cc, TYPE_PERMANENT) && has_subtype(pp, cc, SUBTYPE_SLIVER) ){
					if( generic_activated_ability(pp, cc, event, GAA_SACRIFICE_ME, MANACOST_X(2), 0, NULL, NULL) ){
						return 1;
					}
				}
			}
		}

		if( event == EVENT_ACTIVATE ){
			test_definition_t test;
			new_default_test_definition(&test, TYPE_PERMANENT, "Select a Sliver to sacrifice.");
			test.subtype = SUBTYPE_SLIVER;
			int sac = new_sacrifice(player, card, player, SAC_JUST_MARK|SAC_AS_COST|SAC_RETURN_CHOICE, &test);
			if (!sac){
				cancel = 1;
				return 0;
			}
			if( charge_mana_for_activated_ability(BYTE2(sac), BYTE3(sac), MANACOST_X(2)) ){
				kill_card(BYTE2(sac), BYTE3(sac), KILL_SACRIFICE);
			}
			else{
				state_untargettable(BYTE2(sac), BYTE3(sac), 0);
			}
		}

		if( event == EVENT_RESOLVE_ACTIVATION ){
			draw_cards(player, 1);
		}
	}

	return 0;
}

int card_mnemonic_sliver(int player, int card, event_t event){
	/*
	  Mnemonic Sliver |2|U
	  Creature - Sliver 2/2
	  All Slivers have "{2}, Sacrifice this permanent: Draw a card."
	*/

	if( event == EVENT_RESOLVE_SPELL ){
		int legacy = create_legacy_activate(player, card, &mnemonic_sliver_shared_ability);
		card_instance_t *instance = get_card_instance(player, legacy);
		instance->targets[1].player = player;
		instance->targets[1].card = card;
		instance->number_of_targets = 1;
		int fake = add_card_to_hand(1-player, get_card_instance(player, card)->internal_card_id);
		legacy = create_legacy_activate(1-player, fake, &mnemonic_sliver_shared_ability);
		instance = get_card_instance(1-player, legacy);
		instance->targets[1].player = player;
		instance->targets[1].card = card;
		instance->number_of_targets = 1;
		obliterate_card(1-player, fake);
	}
	return slivercycling(player, card, event);
}

int card_mogg_conscripts(int player, int card, event_t event){

	if( event == EVENT_ATTACK_LEGALITY && affect_me(player, card) && get_stormcreature_count(player) < 1 &&
		! is_humiliated(player, card) )
	{
		event_result = 1;
	}

	return 0;
}

int card_mogg_fanatic(int player, int card, event_t event){
	/* Mogg Fanatic	|R	0x200cd11
	 * Creature - Goblin 1/1
	 * Sacrifice ~: It deals 1 damage to any target. */
	/* Triskelavite	""(token)	0x200cd11
	 * Artifact Creature - Triskelavite 1/1
	 * Flying
	 * Sacrifice this creature: This creature deals 1 damage to any target. */
	// Used externally
	if (!IS_GAA_EVENT(event))
		return 0;

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if (event == EVENT_RESOLVE_ACTIVATION && valid_target(&td))
	{
		damage_target0(player, card, 1);
	}

	return generic_activated_ability(player, card, event, GAA_SACRIFICE_ME | GAA_CAN_TARGET, MANACOST0, 0, &td, "TARGET_ANY");
}

int card_mogg_raider(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	test_definition_t test;
	new_default_test_definition(&test, TYPE_PERMANENT, "Select a Goblin to sacrifice.");
	test.subtype = SUBTYPE_GOBLIN;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			pump_until_eot(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card, 1, 1);
		}
	}

	return generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_CAN_TARGET, MANACOST0, 0, &td, "TARGET_CREATURE",
															0, &test, 0);
}

int card_mongrel_pack(int player, int card, event_t event){
	/* Mongrel Pack	|3|G
	 * Creature - Hound 4/1
	 * When ~ dies during combat, put four 1/1 |Sgreen Hound creature tokens onto the battlefield. */

	if (current_phase > PHASE_MAIN1 && current_phase < PHASE_MAIN2 && this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY)){
		token_generation_t token;
		default_token_definition(player, card, CARD_ID_HOUND, &token);
		token.qty = 4;
		token.pow = 1;
		token.tou = 1;
		token.color_forced = get_sleighted_color_test(player, card, COLOR_TEST_GREEN);
		generate_token(&token);
	}

	return 0;
}

int card_mounted_archers(int player, int card, event_t event)
{
	if (event == EVENT_RESOLVE_ACTIVATION)
	{
		card_instance_t* instance = get_card_instance(player, card);
		can_block_additional_until_eot(instance->parent_controller, instance->parent_card,
										instance->parent_controller, instance->parent_card, 1);
	}

  return generic_activated_ability(player, card, event, GAA_NONE, MANACOST_W(1), 0, NULL, NULL);
}

int card_muscle_sliver(int player, int card, event_t event){
	boost_creature_type(player, card, event, SUBTYPE_SLIVER, 1, 1, 0, BCT_INCLUDE_SELF);
	return slivercycling(player, card, event);
}

int card_natures_revolt(int player, int card, event_t event){

	if( event == EVENT_CHANGE_TYPE ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_LAND);

		global_type_change(player, card, event, ANYBODY, TYPE_CREATURE, &this_test, 2, 2, 0, 0, 0);
	}

	return global_enchantment(player, card, event);
}

int card_orims_prayer(int player, int card, event_t event)
{
  /* Orim's Prayer	|1|W|W
   * Enchantment
   * Whenever one or more creatures attack you, you gain 1 life for each attacking creature. */

  int num;
  if ((num = declare_attackers_trigger(player, card, event, DAT_ATTACKS_PLAYER, 1-player, -1)))
	gain_life(player, num);

  return global_enchantment(player, card, event);
}

int card_orim_samite_healer(int player, int card, event_t event){
	/* Orim, Samite Healer	|1|W|W	0x20034dc
	 * Legendary Creature - Human Cleric 1/3
	 * |T: Prevent the next 3 damage that would be dealt to any target this turn. */
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.extra = damage_card;

	card_instance_t *instance = get_card_instance(player, card);

	check_legend_rule(player, card, event);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			prevent_damage(instance->targets[0].player, instance->targets[0].card, 3);
		}
	}
	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET | GAA_DAMAGE_PREVENTION, MANACOST0, 0,
									&td, "TARGET_DAMAGE");
}

int card_overrun(int player, int card, event_t event){

	if(event == EVENT_RESOLVE_SPELL ){
		pump_subtype_until_eot(player, card, player, -1, 3, 3, KEYWORD_TRAMPLE, 0);
		kill_card(player, card, KILL_DESTROY );
	}

	return basic_spell(player, card, event);
}

int card_patchwork_gnomes(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		if( can_regenerate(player, instance->parent_card) ){
			regenerate_target(player, instance->parent_card);
		}
	}

	return generic_activated_ability(player, card, event, GAA_DISCARD | GAA_REGENERATION, MANACOST0, 0, NULL, NULL);
}

int card_pearl_medallion(int player, int card, event_t event){
/* CARD_ID_PEARL_MEDALLION	3225
Pearl Medallion	|2
Artifact
|SWhite spells you cast cost |1 less to cast. */

	return tempest_medallion(player, card, event, get_sleighted_color_test(player, card, COLOR_TEST_WHITE));
}

int card_pegasus_refuge(int player, int card, event_t event){
	/* Pegasus Refuge	|3|W
	 * Enchantment
	 * |2, Discard a card: Put a 1/1 |Swhite Pegasus creature token with flying onto the battlefield. */

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	if(event == EVENT_RESOLVE_ACTIVATION ){
		token_generation_t token;
		default_token_definition(player, card, CARD_ID_PEGASUS, &token);
		token.pow = token.tou = 1;
		token.color_forced = get_sleighted_color_test(player, card, COLOR_TEST_WHITE);
		token.key_plus = KEYWORD_FLYING;
		generate_token(&token);
	}

	return generic_activated_ability(player, card, event, GAA_DISCARD, MANACOST_X(2), 0, NULL, NULL);
}

int card_perish(int player, int card, event_t event){

	if(event == EVENT_RESOLVE_SPELL){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.color = get_sleighted_color_test(player, card, COLOR_TEST_GREEN);
		new_manipulate_all(player, card, ANYBODY, &this_test, KILL_BURY);

		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_precognition(int player, int card, event_t event){

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, 0);
		td.zone = TARGET_ZONE_PLAYERS;

		if( would_validate_arbitrary_target(&td, 1-player, -1) ){
			int *deck = deck_ptr[1-player];
			if( deck[0] != -1 ){
				int selected = -1;
				if( player == HUMAN ){
					selected = show_deck( player, deck, 1, "Choose a card to put on the bottom", 0, 0x7375B0 );
				}
				else{
					 selected = 0;
				}
				if( selected != -1 ){
					deck[ count_deck(player) ] = deck[0];
					remove_card_from_deck( player, 0 );
				}
			}
		}
	}

	return global_enchantment(player, card, event);
}

int card_propaganda(int player, int card, event_t event){
	tax_attack(player, card, event);
	return global_enchantment(player, card, event);
}

int card_puppet_strings(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	if( event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_X(2), 0, &td, "TARGET_CREATURE");
		if( spell_fizzled != 1 ){
			ai_modifier_twiddle(player, card, 0);
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			twiddle(player, card, 0);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_X(2), 0, &td, "TARGET_CREATURE");
}

int card_rathi_dragon(int player, int card, event_t event){

	if( player == AI && event == EVENT_CAST_SPELL && affect_me(player, card) ){
		if( count_subtype(player, TYPE_LAND, SUBTYPE_MOUNTAIN) < 2 ){
			ai_modifier-=500;
		}
	}

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_LAND, "Select a Mountain to sacrifice.");
		test.subtype = SUBTYPE_MOUNTAIN;
		int sac[2] = {-1, -1};
		int i;
		for(i=0; i<2; i++){
			sac[i] = new_sacrifice(player, card, player, SAC_JUST_MARK|SAC_AS_COST|SAC_RETURN_CHOICE, &test);
			if (!sac[i]){
				break;
			}
		}
		if( !sac[0] || ! sac[1] ){
			for(i=0; i<2; i++){
				if( sac[i] ){
					state_untargettable(BYTE2(sac[i]), BYTE3(sac[i]), 0);
				}
			}
			kill_card(player, card, KILL_SACRIFICE);
		}
		else{
			for(i=0; i<2; i++){
				kill_card(BYTE2(sac[i]), BYTE3(sac[i]), KILL_SACRIFICE);
			}
		}
	}

	return 0;
}

int card_reanimate(int player, int card, event_t event){

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_CREATURE, "Select a creature card.");

	if( event == EVENT_RESOLVE_SPELL ){
		card_instance_t *instance = get_card_instance(player, card);
		int selected = validate_target_from_grave_source(player, card, instance->targets[0].player, 1);
		if( selected != -1 ){
			int zombo = reanimate_permanent(player, card, instance->targets[0].player, selected, REANIMATE_DEFAULT);
			lose_life(player, get_cmc(player, zombo));
		}
		kill_card(player, card, KILL_SACRIFICE);
	}

	return generic_spell(player, card, event, GS_GRAVE_RECYCLER_BOTH_GRAVES, NULL, NULL, 1, &this_test);
}

int card_reckless_spite(int player, int card, event_t event){

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.illegal_color = get_sleighted_color_test(player, card, COLOR_TEST_BLACK);

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_SPELL ){
		int resolved = 0;
		int i;
		for(i=0; i<2; i++){
			if( validate_target(player, card, &td, i) ){
				kill_card(instance->targets[i].player, instance->targets[i].card, KILL_DESTROY);
				resolved++;
			}
		}
		if( resolved > 0 ){
			lose_life(player, 5);
		}
		kill_card(player, card, KILL_SACRIFICE);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td,
						get_sleighted_color_text(player, card, "Select target non%s creature.", COLOR_BLACK), 2, NULL);
}

int card_recycle(int player, int card, event_t event){

	if( ! is_humiliated(player, card) ){
		skip_your_draw_step(player, event);

		if( current_turn == player && event == EVENT_MAX_HAND_SIZE){
		   event_result = 2;
		}

		if(trigger_condition == TRIGGER_SPELL_CAST && affect_me(player, card) && player == trigger_cause_controller
		   && player == reason_for_trigger_controller ){
		   if(event == EVENT_TRIGGER){
			  event_result |= RESOLVE_TRIGGER_MANDATORY;
		   }
		   else if(event == EVENT_RESOLVE_TRIGGER){
				   draw_a_card(player);
		   }
		}
	}

	return global_enchantment(player, card, event);
}

int card_reflecting_pool(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		int colors = get_color_of_mana_produced_by_id(get_id(player, card), COLOR_TEST_ANY_COLORED, player);
		if (colors < 0){
			colors = 0;
		}
		play_land_sound_effect_force_color(player, card, colors);
		return 0;	// so mana_producer() doesn't play the colorless sound
	}

	return mana_producer(player, card, event);
}

int card_repentance(int player, int card, event_t event){

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_creature(instance->targets[0].player, instance->targets[0].card,
							get_power(instance->targets[0].player, instance->targets[0].card),
							instance->targets[0].player, instance->targets[0].card);
		}
		kill_card(player, card, KILL_SACRIFICE);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
}

int card_respite(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		gain_life(player, count_attackers(current_turn));
		fog_effect(player, card);
		kill_card(player, card, KILL_DESTROY);
		return 0;
	}

	return card_fog(player, card, event);
}

int card_rolling_thunder(int player, int card, event_t event){
	/* Rolling Thunder	|X|R|R	0x2003504
	 * Sorcery
	 * ~ deals X damage divided as you choose among any number of targets. */
	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_X_SPELL | GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		set_special_flags2(player, card, SF2_X_SPELL);
		if( (played_for_free(player, card) || is_token(player, card)) && ! check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
			return 0;
		}
		instance->number_of_targets = 0;
		int trgs = 0;
		while( 1 ){
				if( check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
					if( trgs >= instance->info_slot ){
						break;
					}
				}
				else{
					if( ! has_mana(player, COLOR_COLORLESS, trgs+1) ){
						break;
					}
				}
				if( new_pick_target(&td, "TARGET_ANY", trgs, 0) ){
					if( instance->targets[trgs].card != -1 ){
						add_state(instance->targets[trgs].player, instance->targets[trgs].card, STATE_TARGETTED);
					}
					trgs++;
				}
				else{
					break;
				}
		}
		if( ! trgs ){
			spell_fizzled = 1;
			return 0;
		}
		int i;
		for(i=0; i<trgs; i++){
			if( instance->targets[i].card != -1 ){
				remove_state(instance->targets[i].player, instance->targets[i].card, STATE_TARGETTED);
			}
		}
		if( ! played_for_free(player, card) && ! is_token(player, card) ){
			charge_mana(player, COLOR_COLORLESS, trgs);
			if( spell_fizzled != 1 ){
				instance->info_slot = trgs;
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		divide_damage(player, card, &td);
		kill_card(player, card, KILL_DESTROY);
	}

   return 0;
}

int card_root_maze(int player, int card, event_t event){

	/* Root Maze	|G
	 * Enchantment
	 * Artifacts and lands enter the battlefield tapped. */

	permanents_enters_battlefield_tapped(player, card, event, ANYBODY, TYPE_ARTIFACT | TYPE_LAND, NULL);

	return global_enchantment(player, card, event);
}

int card_ruby_medallion(int player, int card, event_t event){
/* CARD_ID_RUBY_MEDALLION	3256
Ruby Medallion	|2
Artifact
|SRed spells you cast cost |1 less to cast. */

	return tempest_medallion(player, card, event, get_sleighted_color_test(player, card, COLOR_TEST_RED));
}

int card_sadistic_glee(int player, int card, event_t event){

	if( ! is_humiliated(player, card) && get_card_instance(player, card)->damage_target_player > -1 ){
		if( event == EVENT_GRAVEYARD_FROM_PLAY ){
			count_for_gfp_ability(player, card, event, ANYBODY, TYPE_CREATURE, 0);
		}
	}

	int result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
	if( result && get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);
		add_1_1_counters(instance->damage_target_player, instance->damage_target_card, BYTE0(result));
	}

	return vanilla_aura(player, card, event, player);
}

int card_safeguard(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			negate_combat_damage_this_turn(instance->parent_controller, instance->parent_card,
											instance->targets[0].player, instance->targets[0].card, 0);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_XW(2, 1), 0, &td, "TARGET_CREATURE");
}


int card_sapphire_medallion(int player, int card, event_t event){
/* CARD_ID_SAPPHIRE_MEDALLION	3261
Sapphire Medallion	|2
Artifact
|SBlue spells you cast cost |1 less to cast. */

	return tempest_medallion(player, card, event, get_sleighted_color_test(player, card, COLOR_TEST_BLUE));
}

int card_scalding_tongs(int player, int card, event_t event){
	/* Scalding Tongs	|2	0x2003531
	 * Artifact
	 * At the beginning of your upkeep, if you have three or fewer cards in hand, ~ deals 1 damage to target opponent or planeswalker. */
	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY && hand_count[player] <= 3 ){
		target_definition_t td_opw;
		opponent_or_pw_target_definition(player, card, &td_opw);
		if (can_target(&td_opw) && pick_target(&td_opw, "TARGET_OPPONENT_OR_PLANESWALKER")){
			damage_target0(player, card, 1);
			get_card_instance(player, card)->number_of_targets = 0;
		}
	}

	return 0;
}

int card_sarcomancy (int player, int card, event_t event){
	/* Sarcomancy	|B
	 * Enchantment
	 * When ~ enters the battlefield, put a 2/2 |Sblack Zombie creature token onto the battlefield.
	 * At the beginning of your upkeep, if there are no Zombies on the battlefield, ~ deals 1 damage to you. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		generate_token_by_id(player, card, CARD_ID_ZOMBIE);
	}

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		if( ! count_subtype(player, TYPE_PERMANENT, SUBTYPE_ZOMBIE) ){
			damage_player(player, 1, player, card);
		}
	}

	return global_enchantment(player, card, event);
}

int card_scorched_earth(int player, int card, event_t event){

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_LAND );

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_LAND, "Select a land card to discard.");
	this_test.zone = TARGET_ZONE_HAND;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_CAN_CAST ){
		if( generic_spell(player, card, event, GS_X_SPELL | GS_CAN_TARGET, &td, NULL, 1, NULL) ){
			return check_battlefield_for_special_card(player, card, player, 0, &this_test);
		}
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		set_special_flags2(player, card, SF2_X_SPELL);
		instance->number_of_targets = 0;
		if( (played_for_free(player, card) || is_token(player, card)) && ! check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
			return 0;
		}
		if( ! check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
			instance->info_slot = check_battlefield_for_special_card(player, card, player, CBFSC_GET_COUNT, &this_test);
		}
		int trgs = 0;
		while( trgs < instance->info_slot && can_target(&td) ){
				if( ! check_special_flags2(player, card, SF2_COPIED_FROM_STACK) && ! has_mana(player, COLOR_COLORLESS, trgs+1) ){
					break;
				}
				if( new_pick_target(&td, "TARGET_LAND", trgs, 0) ){
					state_untargettable(instance->targets[trgs].player, instance->targets[trgs].card, 1);
					trgs++;
				}
				else{
					break;
				}
		}
		int i;
		for(i=0; i<instance->number_of_targets; i++){
			state_untargettable(instance->targets[i].player, instance->targets[i].card, 0);
		}
		if( ! check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
			int original_hand[2][hand_count[player]];
			int ohc = 0;
			for(i=0; i<active_cards_count[player]; i++){
				if( in_hand(player, i) ){
					original_hand[0][ohc] = get_original_internal_card_id(player, i);
					original_hand[1][ohc] = i;
					ohc++;
				}
			}
			int to_discard[hand_count[player]];
			int tdc = 0;
			while( tdc < instance->number_of_targets ){
					int selected = select_card_from_zone(player, player, original_hand[0], ohc, 0, AI_MIN_VALUE, -1, &this_test);
					if( selected != -1 ){
						to_discard[tdc] = original_hand[1][selected];
						tdc++;
						for(i=selected; i<ohc; i++){
							original_hand[0][i] = original_hand[0][i+1];
							original_hand[1][i] = original_hand[1][i+1];
						}
						ohc--;
					}
					else{
						break;
					}
			}
			if( tdc < instance->number_of_targets ){
				spell_fizzled = 1;
				return 0;
			}
			charge_mana(player, COLOR_COLORLESS, instance->number_of_targets);
			if( spell_fizzled != 1 ){
				for(i=0; i<instance->number_of_targets; i++){
					discard_card(player, to_discard[i]);
				}
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int i;
		for(i=0; i<instance->number_of_targets; i++){
			if( validate_target(player, card, &td, i) ){
				kill_card(instance->targets[i].player, instance->targets[i].card, KILL_DESTROY);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_scroll_rack(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_ACTIVATION ){
		char msg[100] = "Select a card to put on top.";
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_ANY, msg);

		int my_hand[100];
		int mhc = 0;
		while( hand_count[player] > 0 ){
				int selected = new_select_a_card(player, player, TUTOR_FROM_HAND, 0, AI_MIN_VALUE, -1, &this_test);
				if( selected != -1 ){
					card_instance_t *this = get_card_instance( player, selected );
					my_hand[mhc] = this->internal_card_id;
					mhc++;
					obliterate_card(player, selected);
				}
				else{
					break;
				}
		}
		if( mhc > 0 ){
			draw_cards(player, mhc);
			int i;
			for(i = 0; i < mhc; i++){
				int card_added = add_card_to_hand(player, my_hand[i]);
				put_on_top_of_deck(player, card_added);
			}
			rearrange_top_x(player, player, mhc);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_X(1), 0, NULL, NULL);
}

int card_searing_touch(int player, int card, event_t event){
	/* Searing Touch	|R	0x200a0c0
	 * Instant
	 * Buyback |4
	 * ~ deals 1 damage to any target. */
	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ANY", 1, NULL);
		if( spell_fizzled != 1 ){
			instance->info_slot = buyback(player, card, MANACOST_X(4));
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_target0(player, card, 1);
			if( instance->info_slot == 1 ){
				bounce_permanent(player, card);
				return 0;
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_seeker_of_skybreak(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST0, 0, &td, "TARGET_CREATURE");
		if( spell_fizzled != 1 ){
			ai_mod_for_untap_effects(player, card, 0, 0);
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			untap_card(instance->targets[0].player, instance->targets[0].card);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST0, 0, &td, "TARGET_CREATURE");
}

int card_selenia_dark_angel(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		if( in_play(instance->parent_controller, instance->parent_card) ){
			bounce_permanent(instance->parent_controller, instance->parent_card);
		}
	}

	return generic_activated_ability(player, card, event, 0, MANACOST0, 2, NULL, NULL);
}

int card_serene_offering(int player, int card, event_t event){

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ENCHANTMENT);

	card_instance_t *instance = get_card_instance(player, card);

	if(event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			int amount = get_cmc(instance->targets[0].player, instance->targets[0].card);
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
			gain_life(player, amount);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ENCHANTMENT", 1, NULL);
}

int card_shadow_rift(int player, int card, event_t event){
	/* Shadow Rift	|U
	 * Instant
	 * Target creature gains shadow until end of turn.
	 * Draw a card. */
	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = ANYBODY;
	return vanilla_pump(player, card, event, &td, 0, 0, VANILLA_PUMP_DRAW_A_CARD, SP_KEYWORD_SHADOW);
}

int card_shocker(int player, int card, event_t event){
/*CARD_ID_SHOCKER	3277
Shocker	|1|R
Creature - Insect 1/1
Whenever ~ deals damage to a player, that player discards all the cards in his or her hand, then draws that many cards.*/


	int result = whenever_i_deal_damage_to_a_player(player, card, event, 0);
	if( result ){
		int times_damaged[2] = {BYTE0(result), BYTE1(result)};
		APNAP(p,{
					while( times_damaged[p] ){
							int amount = hand_count[p];
							new_discard_all(p, player);
							draw_cards(p, amount);
							times_damaged[p]--;
					}
				}
		);
	}

	return 0;
}

int card_skyshroud_ranger(int player, int card, event_t event){

	// |T: You may put a land card from your hand onto the battlefield. Activate this ability only any time you could cast a sorcery.
	if (event == EVENT_RESOLVE_ACTIVATION){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_LAND, "Select a land to put into play.");
		new_global_tutor(player, player, TUTOR_FROM_HAND, TUTOR_PLAY, 0, AI_MAX_VALUE, &this_test);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_SORCERY_BE_PLAYED, MANACOST_X(0), 0, NULL, NULL);
}

int card_skyshroud_elves(int player, int card, event_t event){
	if (event == EVENT_COUNT_MANA){
		if (affect_me(player, card) && !is_tapped(player, card) && !is_sick(player, card) && can_produce_mana(player, card))
			declare_mana_available(player, COLOR_GREEN, 1);
	} else if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE){
		if (!can_produce_mana(player, card)){
			return 0;
		}

		int can_produce_g = !is_tapped(player, card) && !is_sick(player, card);
		if (player == AI){
			if (event == EVENT_CAN_ACTIVATE){
				return can_produce_g;
			} else {
				produce_mana_tapped(player, card, COLOR_GREEN, 1);
				return 0;
			}
		}

		card_instance_t *instance = get_card_instance(player, card);
		int can_produce_rw = instance->info_slot <= 0 && has_mana(player, COLOR_ANY, 1);

		if (event == EVENT_CAN_ACTIVATE){
			return can_produce_g || can_produce_rw;
		}

		int choice;
		if (can_produce_rw){
			choice = COLOR_TEST_RED|COLOR_TEST_WHITE;
			if (can_produce_g){
				choice |= COLOR_TEST_GREEN;
			}
			choice = choose_a_color_exe(player, "What kind of mana?", 1, 0, choice);
			if (choice == -1){
				cancel = 1;
				return 0;
			}
		} else {
			choice = COLOR_GREEN;
		}

		if (choice == COLOR_GREEN){
			produce_mana_tapped(player, card, COLOR_GREEN, 1);
		} else {
			instance->info_slot = 1;
			charge_mana(player, COLOR_COLORLESS, 1);
			instance->info_slot = 0;
			if (cancel == 1){
				return 0;
			}
			produce_mana(player, choice, 1);
		}
	}
	return 0;
}

int card_skyshroud_vampire(int player, int card, event_t event){

	if (event == EVENT_POW_BOOST || event == EVENT_TOU_BOOST){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.zone = TARGET_ZONE_HAND;
		return generic_shade_amt_can_pump(player, card, 2, 0, MANACOST0,
										check_battlefield_for_special_card(player, card, player, CBFSC_GET_COUNT, &this_test));
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_CREATURE, "Select a creature card.");
	this_test.zone = TARGET_ZONE_HAND;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability(player, card, event, 0, MANACOST0, 0, NULL, NULL) ){
			return check_battlefield_for_special_card(player, card, player, 0, &this_test);
		}
	}

	if( event == EVENT_ACTIVATE ){
		if( charge_mana_for_activated_ability(player, card, MANACOST0) ){
			int selected = new_select_a_card(player, player, TUTOR_FROM_HAND, 0, AI_MIN_VALUE, -1, &this_test);
			if( selected != -1 ){
				discard_card(player, selected);
			}
			else{
				spell_fizzled = 1;
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		pump_until_eot(instance->parent_controller, instance->parent_card, instance->parent_controller, instance->parent_card, 2, 2);
	}


	return 0;
}

int card_soltari_crusader(int player, int card, event_t event){

	shadow(player, card, event);

	return generic_shade(player, card, event, 0, MANACOST_XW(1, 1), 1, 0, 0, 0);
}

int card_soltari_emissary(int player, int card, event_t event){
	return generic_shade(player, card, event, 0, MANACOST_W(1), 0, 0, 0, SP_KEYWORD_SHADOW);
}

int soltari_guerrillas_effect(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);
	card_instance_t* damage = combat_damage_being_prevented(event);
	if( damage && instance->targets[1].player > -1 &&
		damage->damage_source_player == instance->damage_target_player && damage->damage_source_card == instance->damage_target_card &&
		damage->damage_target_card == -1 && !check_special_flags(damage->damage_source_player, damage->damage_source_card, SF_ATTACKING_PWALKER) &&
		damage->damage_target_player != player &&
		in_play(instance->targets[0].player, instance->targets[0].card)
	  ){
		int dam = damage->info_slot;
		damage->info_slot = 0;
		damage_creature(instance->targets[1].player, instance->targets[1].card, dam, instance->damage_target_player, instance->damage_target_card);
		kill_card(player, card, KILL_EXILE);
	}

	if( event == EVENT_CLEANUP ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int card_soltari_guerrillas(int player, int card, event_t event){

	shadow(player, card, event);

	if( ! IS_GAA_EVENT(event) || is_humiliated(player, card) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( is_attacking(player, card) && is_unblocked(player, card) && current_phase == PHASE_AFTER_BLOCKING && instance->info_slot != 66 ){
			return generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST0, 0, &td, NULL);
		}
	}

	if( event == EVENT_ACTIVATE){
		generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST0, 0, &td, NULL);
		if( spell_fizzled != 1 ){
			instance->info_slot = 66;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int legacy = create_targetted_legacy_effect(instance->parent_controller, instance->parent_card, &soltari_guerrillas_effect,
													instance->parent_controller, instance->parent_card);
		card_instance_t *leg = get_card_instance(player, legacy);
		leg->targets[1] = instance->targets[0];
		leg->number_of_targets = 2;
	}

	if( event == EVENT_CLEANUP ){
		instance->info_slot = 0;
	}

	return 0;
}

int card_soltari_lancer(int player, int card, event_t event){
	shadow(player, card, event);
	if( event == EVENT_ABILITIES && affect_me(player, card) && is_attacking(player, card) && ! is_humiliated(player, card) ){
		event_result |= KEYWORD_FIRST_STRIKE;
	}
	return 0;
}

int card_soltari_trooper(int player, int card, event_t event)
{
	/* Soltari Trooper	|1|W
	 * Creature - Soltari Soldier 1/1
	 * Shadow
	 * Whenever ~ attacks, it gets +1/+1 until end of turn. */

	shadow(player, card, event);

	return when_attacks_pump_self(player, card, event, 1,1);
}

int card_spike_drone(int player, int card, event_t event){

	return generic_spike(player, card, event, 1);
}

int card_spirit_mirror(int player, int card, event_t event){
	/* Spirit Mirror	|2|W|W
	 * Enchantment
	 * At the beginning of your upkeep, if there are no Reflection tokens on the battlefield, put a 2/2 |Swhite Reflection creature token onto the battlefield.
	 * |0: Destroy target Reflection. */

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_PERMANENT);
		this_test.type_flag = F1_IS_TOKEN;
		this_test.subtype = SUBTYPE_REFLECTION;

		if( ! check_battlefield_for_special_card(player, card, ANYBODY, 0, &this_test) ){
			token_generation_t token;
			default_token_definition(player, card, CARD_ID_REFLECTION, &token);
			token.pow = token.tou = 2;
			token.color_forced = get_sleighted_color_test(player, card, COLOR_TEST_WHITE);
			generate_token(&token);
		}
	}

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_PERMANENT);
	td.required_subtype = SUBTYPE_REFLECTION;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST0, 0,
									&td, "Select target Reflection.");
}

int card_starke_of_rath(int player, int card, event_t event)
{
  /* Starke of Rath	|1|R|R
   * Legendary Creature - Human Rogue 2/2
   * |T: Destroy target artifact or creature. That permanent's controller gains control of ~. */

  check_legend_rule(player, card, event);

  if (!IS_GAA_EVENT(event))
	return 0;

  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_ARTIFACT | TYPE_CREATURE);

  if (event == EVENT_RESOLVE_ACTIVATION && valid_target(&td))
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  int pp = instance->parent_controller, pc = instance->parent_card;

	  kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);

	  if (pp != instance->targets[0].player)
		give_control_of_self(pp, pc);
	}

  return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET, MANACOST0, 0, &td, "PUTREFY");	// Select target artifact or creature
}

static const char* can_be_untapped(int who_chooses, int player, int card)
{
	if( ! check_special_flags2(player, card, SF2_COULD_NOT_UNTAP) ){
		return NULL;
	}
	return "this permanent couldn't be untapped";
}

int card_static_orb(int player, int card, event_t event){
	// Used externally
	card_instance_t *instance = get_card_instance(player, card);

	if( ! is_tapped(player, card) && current_phase == PHASE_UNTAP && event == EVENT_UNTAP && ! is_humiliated(player, card) ){
		// This block avoid that multiple Static Orbs allow to untap more than 2 permanentes per untap
		int i;
		for(i=0; i<2; i++){
			int count = active_cards_count[i]-1;
			while( count > -1 ){
					if( in_play(i, count) && get_id(i, count) == get_id(player, card) && !(i == player && count == card) ){
						get_card_instance(i, count)->info_slot = 2;
					}
					count--;
			}
		}

		if( instance->info_slot == 0 ){
			target_definition_t td;
			default_target_definition(player, card, &td, TYPE_PERMANENT);
			td.illegal_type = TYPE_ENCHANTMENT | TARGET_TYPE_PLANESWALKER;
			td.allowed_controller = current_turn;
			td.preferred_controller = current_turn;
			td.required_state = TARGET_STATE_TAPPED;
			td.who_chooses = current_turn;
			td.illegal_abilities = 0;
			td.allow_cancel = 0;
			td.extra = (int32_t)can_be_untapped;
			td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
			while( instance->info_slot < 2 && can_target(&td) ){
					if( select_target(player, card, &td, "Select a permanent to untap", &(instance->targets[0])) ){
						instance->number_of_targets = 1;
						untap_card(instance->targets[0].player, instance->targets[0].card);
						instance->info_slot++;
					}
			}
		}

		get_card_instance(affected_card_controller, affected_card)->untap_status &= ~3;
	}

	if( event == EVENT_CLEANUP ){
		instance->info_slot = 0;
	}

	return 0;
}

int card_staunch_defenders(int player, int card, event_t event){
	// original code : 0x413EE0

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		gain_life(player, 4);
	}

	return 0;
}

int card_storm_front(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.required_abilities = KEYWORD_FLYING;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			tap_card(instance->targets[0].player, instance->targets[0].card);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST_G(2), 0,
									&td, "Select target creature with flying.");
}

int card_talon_sliver(int player, int card, event_t event){
	boost_creature_type(player, card, event, SUBTYPE_SLIVER, 0, 0, KEYWORD_FIRST_STRIKE, BCT_INCLUDE_SELF);
	return slivercycling(player, card, event);
}

int card_thalakos_seer(int player, int card, event_t event){
	shadow(player, card, event);
	if( leaves_play(player, card, event) ){
		draw_cards(player, 1);
	}
	return 0;
}

int card_thumbscrew(int player, int card, event_t event){
	/* Thumbscrews	|2	0x2003572
	 * Artifact
	 * At the beginning of your upkeep, if you have five or more cards in hand, ~ deals 1 damage to target opponent or planeswalker. */
	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY && hand_count[player] >= 5 ){
		target_definition_t td_opw;
		opponent_or_pw_target_definition(player, card, &td_opw);
		if (can_target(&td_opw) && pick_target(&td_opw, "TARGET_OPPONENT_OR_PLANESWALKER")){
			damage_target0(player, card, 1);
			get_card_instance(player, card)->number_of_targets = 0;
		}
	}

	return 0;
}

int card_time_warp(int player, int card, event_t event)
{
	/* Time Warp	|3|U|U
	 * Sorcery
	 * Target player takes an extra turn after this one. */

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;
	td.preferred_controller = player;

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += 96;
	}

	if (event == EVENT_RESOLVE_SPELL){
		card_instance_t* instance = get_card_instance(player, card);
		if (valid_target(&td)){
			if( instance->targets[0].player == player ){
				time_walk_effect(player, card);
			}
			else{
				int fake = add_card_to_hand(instance->targets[0].player, get_original_internal_card_id(player, card));
				time_walk_effect(instance->targets[0].player, fake);
				obliterate_card(instance->targets[0].player, fake);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
}

int card_tooth_and_claw(int player, int card, event_t event){
	/* Tooth and Claw	|3|R
	 * Enchantment
	 * Sacrifice two creatures: Put a 3/1 |Sred Beast creature token named Carnivore onto the battlefield. */

	if( event == EVENT_CAN_ACTIVATE ){
		test_definition_t test;
		default_test_definition(&test, TYPE_CREATURE);
		test.qty = 2;
		return generic_activated_ability_with_sacrifice_as_cost(player, card, event, 0, MANACOST0, 0, NULL, NULL, 0, &test, 0);
	}

	if( event == EVENT_ACTIVATE ){
		int sacs[2] = {-1};
		test_definition_t test;
		new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");
		int i;
		for(i=0; i<2; i++){
			sacs[i] = new_sacrifice(player, card, player, SAC_JUST_MARK|SAC_AS_COST|SAC_RETURN_CHOICE, &test);
			if (!sacs[i]){
				cancel = 1;
				break;
			}
		}
		for(i=0; i<2; i++){
			if( sacs[i] > -1 ){
				state_untargettable(BYTE2(sacs[i]), BYTE3(sacs[i]), 0);
			}
		}
		generic_activated_ability(player, card, event, 0, MANACOST0, 0, NULL, NULL);
		if( spell_fizzled != 1 ){
			kill_card(BYTE2(sacs[0]), BYTE3(sacs[0]), KILL_SACRIFICE);
			kill_card(BYTE2(sacs[1]), BYTE3(sacs[1]), KILL_SACRIFICE);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		generate_token_by_id(instance->parent_controller, instance->parent_card, CARD_ID_CARNIVORE);
	}

	return global_enchantment(player, card, event);
}

int card_torture_chamber(int player, int card, event_t event){

	/* Torture Chamber	|3
	 * Artifact
	 * At the beginning of your upkeep, put a pain counter on ~.
	 * At the beginning of your end step, ~ deals damage to you equal to the number of pain counters on it.
	 * |1, |T, Remove all pain counters from ~: ~ deals damage to target creature equal to the number of pain counters removed this way. */

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		add_counter(player, card, COUNTER_PAIN);
	}

	if( count_counters(player, card, COUNTER_PAIN) > 0 && current_turn == player && eot_trigger(player, card, event) ){
		int amount = count_counters(player, card, COUNTER_PAIN);
		damage_player(player, amount, player, card);
	}

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET, MANACOST_X(1), 0, &td, NULL);
	}

	if( event == EVENT_ACTIVATE ){
		instance->number_of_targets = 0;
		if( charge_mana_for_activated_ability(player, card, MANACOST_X(1)) && pick_target(&td, "TARGET_CREATURE") ){
			instance->info_slot = count_counters(player, card, COUNTER_PAIN);
			remove_counters(player, card, COUNTER_PAIN, instance->info_slot);
			tap_card(player, card);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			damage_target0(player, card, instance->info_slot);
		}
	}

	return 0;
}

int card_tradewind_rider(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	card_instance_t *instance = get_card_instance( player, card );

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.illegal_abilities = 0;
	td.allowed_controller = player;
	td.preferred_controller = player;
	td.illegal_state = TARGET_STATE_TAPPED;
	td.special = TARGET_SPECIAL_NOT_ME;

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_PERMANENT);

	if( event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST0, 0, &td1, NULL)  ){
			return target_available(player, card, &td) > 1;
		}
	}

	if(event == EVENT_ACTIVATE ){
		instance->number_of_targets = 0;
		if( charge_mana_for_activated_ability(player, card, MANACOST0) ){
			int tapped[2] = {-1, -1};
			int tc = 0;
			while( tc < 2 ){
				if( select_target(player, card, &td, "Select a creature to tap.", &(instance->targets[tc])) ){
					state_untargettable(instance->targets[tc].player, instance->targets[tc].card, 1);
					tapped[tc] = instance->targets[tc].card;
					tc++;
				}
				else{
					break;
				}
			}
			int i;
			for(i=0; i<tc; i++){
				state_untargettable(instance->targets[i].player, instance->targets[i].card, 0);
			}
			if( tc < 2 ){
				spell_fizzled = 1;
				return 0;
			}
			instance->number_of_targets = 0;
			if( pick_target(&td1, "TARGET_PERMANENT") ){
				for(i=0; i<2; i++){
					tap_card(player, tapped[i]);
				}
				tap_card(player, card);
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION){
		if(valid_target(&td1) ){
			bounce_permanent(instance->targets[0].player, instance->targets[0].card);
		}
	}

	return 0;
}

static int effect_unstable_shapeshifter(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);
	if( instance->damage_target_player > -1 ){
		if( ! is_humiliated(instance->damage_target_player, instance->damage_target_card) ){
			if (specific_cip(player, card, event, ANYBODY, RESOLVE_TRIGGER_MANDATORY, TYPE_CREATURE,MATCH, 0,0, 0,0, 0,0, -1,0)){
				cloning_and_verify_legend(instance->damage_target_player, instance->damage_target_card, instance->targets[1].player, instance->targets[1].card);
			}
		}
	}

  return 0;
}
int card_unstable_shapeshifter(int player, int card, event_t event)
{
	/* Unstable Shapeshifter	|3|U	0x200357c
	 * Creature - Shapeshifter 0/1
	 * Whenever another creature enters the battlefield, ~ becomes a copy of that creature, except it has this ability. */
  cloning_card(player, card, event);

  if (event == EVENT_RESOLVE_SPELL)
	create_targetted_legacy_effect(player, card, &effect_unstable_shapeshifter, player, card);

  return 0;
}

int card_verdant_force(int player, int card, event_t event){
	/* Verdant Force	|5|G|G|G
	 * Creature - Elemental 7/7
	 * At the beginning of each upkeep, put a 1/1 |Sgreen Saproling creature token onto the battlefield. */

	upkeep_trigger_ability(player, card, event, ANYBODY);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		generate_token_by_id(player, card, CARD_ID_SAPROLING);
	}

	return 0;
}

int card_vathi_il_dal(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			int ai_choice = 1;
			if( is_attacking(instance->targets[0].player, instance->targets[0].card) &&
				is_unblocked(instance->targets[0].player, instance->targets[0].card) )
			{
				ai_choice = 0;
			}
			int choice = do_dialog(player, player, card, -1, -1, " Turn power to 1\n Turn toughness to 1", ai_choice);
			if( choice == 0 ){
				set_pt_and_abilities_until_eot(instance->parent_controller, instance->parent_card,
												instance->targets[0].player, instance->targets[0].card, 1, -1, 0, 0, 0);
			}
			else{
				set_pt_and_abilities_until_eot(instance->parent_controller, instance->parent_card,
												instance->targets[0].player, instance->targets[0].card, -1, 1, 0, 0, 0);
			}
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST0, 0, &td, "TARGET_CREATURE");
}

int card_wasteland(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) || event == EVENT_CAN_ACTIVATE ){
		return mana_producer(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_LAND);
	td.required_subtype = SUBTYPE_NONBASIC;

	card_instance_t *instance = get_card_instance(player, card);

	if (event == EVENT_ACTIVATE){
		instance->info_slot = 0;
		int choice = 0;
		if( !paying_mana() && generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED | GAA_CAN_TARGET | GAA_SACRIFICE_ME, MANACOST0, 0, &td, NULL) ){
			choice = do_dialog(player, player, card, -1, -1, " Get mana\n Destroy a nonbasic land\n Cancel", 1);
		}
		if( choice == 0 ){
			return mana_producer(player, card, event);
		}
		else if( choice == 1 ){
				add_state(player, card, STATE_TAPPED);
				if( charge_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0) &&
					new_pick_target(&td, "TARGET_NONBASIC_LAND", 0, 1)
				  ){
					instance->info_slot = 1;
					kill_card(player, card, KILL_SACRIFICE);
				}
				else{
					remove_state(player, card, STATE_TAPPED);
				}
		}
		else{
			spell_fizzled = 1;
		}
	}

	if(event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == 1 && validate_target(player, card, &td, 0) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
	}

	return 0;
}

int card_wild_wurm(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		if( ! flip_a_coin(player, card) ){
			bounce_permanent(player, card);
		}
	}

	return 0;
}

int card_winged_sliver(int player, int card, event_t event){
	boost_creature_type(player, card, event, SUBTYPE_SLIVER, 0, 0, KEYWORD_FLYING, BCT_INCLUDE_SELF);
	return slivercycling(player, card, event);
}

int card_whispers_of_the_muse(int player, int card, event_t event){

	if( event == EVENT_CAN_CAST ){
		return basic_spell(player, card, event);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		int total = get_total_available_lands(player);
		if( total <= 3 || total >= 6 ){
			ai_modifier += 24;
		}
		if( has_mana_multi(player, MANACOST_X(5)) ){
			ai_modifier += 24;
		}
		basic_spell(player, card, event);
		if( spell_fizzled != 1 ){
			get_card_instance(player, card)->info_slot = buyback(player, card, MANACOST_X(5));
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		draw_cards(player, 1);
		if( get_card_instance(player, card)->info_slot == 1 ){
			bounce_permanent(player, card);
		}
		else{
			kill_card(player, card, KILL_DESTROY);
		}
	}

	return 0;
}

int card_worthy_cause(int player, int card, event_t event){

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_SAC_CREATURE_AS_COST, NULL, NULL, 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");
		int sac = new_sacrifice(player, card, player, SAC_JUST_MARK|SAC_AS_COST|SAC_RETURN_CHOICE, &test);
		if (!sac){
			cancel = 1;
			return 0;
		}
		instance->targets[1].player = get_toughness(BYTE2(sac), BYTE3(sac));
		instance->info_slot = buyback(player, card, MANACOST_X(2));
		if( spell_fizzled != 1 ){
			kill_card(BYTE2(sac), BYTE3(sac), KILL_SACRIFICE);
		}
		else{
			state_untargettable(BYTE2(sac), BYTE3(sac), 0);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		gain_life(player, instance->targets[1].player);

		if( instance->info_slot == 1 ){
			bounce_permanent(player, card);
		}
		else{
			kill_card(player, card, KILL_DESTROY);
		}
	}

	return 0;
}


int card_winds_of_rath(int player, int card, event_t event)
{
  // Destroy all creatures that aren't enchanted. They can't be regenerated.

	if (event == EVENT_RESOLVE_SPELL){
		char enchanted[2][151] = {{0}};
		int p, c;
		card_instance_t* aura;
		for (p = 0; p <= 1; ++p){
			for (c = 0; c < active_cards_count[p]; ++c){
				if ((aura = in_play(p, c)) && is_what(p, c, TYPE_ENCHANTMENT) && has_subtype(p, c, SUBTYPE_AURA)
					&& aura->damage_target_player >= 0 && aura->damage_target_player <= 1
					&& aura->damage_target_card >= 0 && aura->damage_target_card <= 150
				  ){
					enchanted[aura->damage_target_player][aura->damage_target_card] = 1;
				}
			}

			for (p = 0; p <= 1; ++p){
				for (c = 0; c < active_cards_count[p]; ++c){
					if (in_play(p, c) && is_what(p, c, TYPE_CREATURE) && !enchanted[p][c]){
						kill_card(p, c, KILL_BURY);
					}
				}
			}
		}

		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
}

int card_rootwater_matriarch(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.allowed_controller = 1-player;
	if( duh_mode(player) ){
		td.required_state = TARGET_STATE_ENCHANTED;
	}

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION && valid_target(&td) ){
		if( is_enchanted(instance->targets[0].player, instance->targets[0].card) ){
			gain_control_until_source_is_in_play_and_tapped(instance->parent_controller, instance->parent_card,
															instance->targets[0].player, instance->targets[0].card,
															GCUS_ENCHANTED);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST0, 0, &td, "TARGET_CREATURE");
}

int card_enraging_licid(int player, int card, event_t event){
	return generic_licid(player, card, event, MANACOST_R(1), 0, 0, 0, SP_KEYWORD_HASTE, player);
}

int card_leeching_licid(int player, int card, event_t event){

	card_instance_t *instance = get_card_instance(player, card);

	if( ! is_what(player, card, TYPE_CREATURE) && instance->damage_target_player > -1 ){
		upkeep_trigger_ability(player, card, event, instance->damage_target_player);

		if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
			damage_player(instance->damage_target_player, 1, player, card);
		}
	}

	return generic_licid(player, card, event, MANACOST_B(1), 0, 0, 0, 0, 1-player);
}

int card_nurturing_licid(int player, int card, event_t event){

	if( event == EVENT_CHANGE_TYPE && affect_me(player, card) && get_card_instance(player, card)->targets[3].card != -1 ){
		event_result = get_card_instance(player, card)->targets[3].card;
	}

	if( ! is_what(player, card, TYPE_CREATURE) ){
		card_instance_t *instance = get_card_instance(player, card);
		if( instance->damage_target_player > -1 ){
			if( event == EVENT_CAN_ACTIVATE ){
				if( generic_activated_ability(player, card, event, 0, MANACOST_G(1), 0, NULL, NULL) ){
					if( (land_can_be_played & LCBP_REGENERATION) &&
						get_card_instance(instance->damage_target_player, instance->damage_target_card)->kill_code == KILL_DESTROY
					  ){
						return 99;
					}
					return 1;
				}
			}

			if( event == EVENT_ACTIVATE ){
				if( charge_mana_for_activated_ability(player, card, MANACOST_G(1)) ){
					instance->info_slot = 2;
					if( (land_can_be_played & LCBP_REGENERATION) &&
						get_card_instance(instance->damage_target_player, instance->damage_target_card)->kill_code == KILL_DESTROY
					  ){
						instance->info_slot = can_be_regenerated(instance->damage_target_player, instance->damage_target_card) ? 1 : 2;
					}
				}
			}

			if( event == EVENT_RESOLVE_ACTIVATION ){
				if( instance->info_slot == 1 && can_be_regenerated(instance->damage_target_player, instance->damage_target_card) ){
					regenerate_target(instance->damage_target_player, instance->damage_target_card);
				}
				if( instance->info_slot == 2 ){
					card_instance_t *parent = get_card_instance(instance->parent_controller, instance->parent_card);
					reset_subtypes(instance->parent_controller, instance->parent_card, 2);
					parent->damage_target_player = parent->damage_target_card = parent->targets[3].card = -1;
				}
			}
			return generic_aura(player, card, event, player, 0, 0, KEYWORD_REGENERATION, 0, 0, 0, 0);
		}
	}

	return generic_licid(player, card, event, MANACOST_G(1), 0, 0, 0, 0, player);
}

int card_quickening_licid(int player, int card, event_t event){
	return generic_licid(player, card, event, MANACOST_XW(1, 1), 0, 0, KEYWORD_FIRST_STRIKE, 0, player);
}

int card_stinging_licid(int player, int card, event_t event){

	if( ! is_what(player, card, TYPE_CREATURE) && get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);
		if( event == EVENT_TAP_CARD && affect_me(instance->damage_target_player, instance->damage_target_card) ){
			damage_player(instance->damage_target_player, 2, player, card);
		}
	}

	return generic_licid(player, card, event, MANACOST_XU(1, 1), 0, 0, 0, 0, 1-player);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21546c0; original cards/tempest.c:4205. */
int card_sacred_guide(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    default_test_definition(&this_test, 0xff);
    this_test.color = get_sleighted_color_test(player, card, 0x20);
    reveal_card_until_you_reveal_test(player, player, &this_test, 0x28);
  }
  result = generic_activated_ability(player, card, event, 8, 1, 0, 0, 0, 0, 1, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2159660; original cards/tempest.c:5848. */
int card_avenging_angel(int player, int card, event_t event)
{
  int result;
  int owner;
  int position;
  result = duh_mode(player);
  result = this_dies_trigger(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (result != 0)
  {
    result = find_in_owners_graveyard(player, card, &owner, &position);
    if (result != 0)
    {
      result = graveyard_ptr_mutable[owner][position];
      remove_card_from_grave(owner, position);
      put_iid_under_the_first_x_cards_of_library(owner, result, 0);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2159f70; original cards/tempest.c:6017. */
int card_canyon_drake(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_POW_BOOST)
  {
    if (0 < hand_count[player])
    {
      result = generic_shade_amt_can_pump_2022(player, card, 2, 0, 1, 0, 0, 0, 0, 0, hand_count[player]);
      return result;
    }
  }
  else
  {
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x20, 1, 0, 0, 0, 0, 0,
                                         0, NULL, NULL);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_shade_merge_pt(player, card, EVENT_ACTIVATE, 0, 1, 0, 0, 0, 0, 0, 0, 0);
      if (spell_fizzled != 1)
      {
        discard(player, 1, player);
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result =
          generic_shade_merge_pt(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0, 0, 0, 0, 0, 0, 2, 0);
      return result;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x215a260; original cards/tempest.c:6074. */
int card_chill(int player, int card, event_t event)
{
  unsigned int uVar1;
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    uVar1 = get_color(affected_card_controller, affected_card);
    if ((uVar1 & 0x10) != 0)
    {
      COST_COLORLESS = COST_COLORLESS + '\x02';
    }
  }
  if (event != EVENT_SHOULD_AI_PLAY)
  {
    return (unsigned int)(event == EVENT_CAN_CAST);
  }
  ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
  return 0;
}

/* ManalinkEh 2022: 0x2147c10; original cards/tempest.c:6094. */
static char *prevent_damage_from_shadow_creature(int who_chooses, int player, int card)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if ((((instance->internal_card_id == damage_card) &&
        (instance->damage_target_player == player)) &&
       (result = is_what((int)instance->damage_source_player, instance->damage_source_card, 2),
        result != 0)) &&
      (result = check_for_special_ability((int)instance->damage_source_player,
                                          instance->damage_source_card, 0x100),
       result != 0))
  {
    return NULL;
  }
  return "must be combat damage from a creature with shadow";
}

/* ManalinkEh 2022: 0x215a850; original cards/tempest.c:6194. */
int card_deadshot(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    get_card_instance(player, card);
    iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 2, NULL);
    return iVar2;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        iVar2 = pick_target(&td, "TARGET_CREATURE");
        if (iVar2 != 0)
        {
          iVar2 = is_token(player, card);
          if (iVar2 == 0)
          {
            state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
            iVar2 = can_target(&td);
            if (iVar2 != 0)
            {
              new_pick_target(&td, "TARGET_ANOTHER_CREATURE", 1, 1);
            }
            state_untargettable(instance->targets[0].player, instance->targets[0].card, 0);
          }
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar2 = valid_target(&td);
      if (iVar2 != 0)
      {
        tap_card(instance->targets[0].player, instance->targets[0].card);
        iVar2 = get_power(instance->targets[0].player, instance->targets[0].card);
        iVar3 = validate_target(player, card, &td, 1);
        if (iVar3 != 0)
        {
          damage_creature(instance->targets[1].player, instance->targets[1].card, iVar2, player,
                          card);
        }
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 2);
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2147610; original cards/tempest.c:6300. */
static void blocked_by_me_or_my_blocker_gets_1_1(int player, int card, int t_player, int t_card)
{
  pump_ability_until_eot_2022(player, card, t_player, t_card, 1, 1, 0, 0);
  return;
}

/* ManalinkEh 2022: 0x215b450; original cards/tempest.c:6458. */
int card_gallantry(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (event != EVENT_CHECK_PUMP)
  {
    if (event != EVENT_CAN_CAST)
    {
      if (event == EVENT_CAST_SPELL)
      {
        if ((card != affected_card) || (player != affected_card_controller))
        {
          return 0;
        }
      }
      else if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
               (event != EVENT_CHANGE_TARGET))
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.required_state = 0x10;
    result = vanilla_pump(player, card, event, &td, 4, 4, 0x1000000, 0);
    return result;
  }
  instance = get_card_instance(player, card);
  result = has_mana_to_cast_iid(player, EVENT_CAN_CAST, instance->internal_card_id);
  if (result == 0)
  {
    return 0;
  }
  if (current_turn != player)
  {
    pumpable_power[player] = pumpable_power[player] + 4;
    pumpable_toughness[player] = pumpable_toughness[player] + 4;
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x215ba50; original cards/tempest.c:6577. */
int card_heartwood_dryad(int player, int card, event_t event)
{
  if (event != EVENT_ABILITIES)
  {
    return 0;
  }
  special_abilities_2022(player, card, EVENT_ABILITIES, 0x8000000, player, card);
  return 0;
}

/* ManalinkEh 2022: 0x215be60; original cards/tempest.c:6664. */
int card_legerdemain(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  target_definition_t td2;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x42);
    td.preferred_controller = player;
    td.allowed_controller = player;
    default_target_definition(player, card, &td2, 0x42);
    td.preferred_controller = 1 - player;
    td2.allowed_controller = td.preferred_controller;
    get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
      return (unsigned int)(result != 0);
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x42);
      td.preferred_controller = player;
      td.allowed_controller = player;
      default_target_definition(player, card, &td2, 0x42);
      td.preferred_controller = 1 - player;
      td2.allowed_controller = td.preferred_controller;
      instance = get_card_instance(player, card);
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        result = new_pick_target(&td, "Select target artifact or creature you control.", 0, 0x81);
        if (result != 0)
        {
          new_pick_target(&td2, "Select target artifact or creature controlled by opponent.", 1,
                          0x81);
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x42);
    td.preferred_controller = player;
    td.allowed_controller = player;
    default_target_definition(player, card, &td2, 0x42);
    td.preferred_controller = 1 - player;
    td2.allowed_controller = td.preferred_controller;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = validate_target(player, card, &td, 0);
      if ((result != 0) && (result = validate_target(player, card, &td2, 1), result != 0))
      {
        exchange_control_of_target_permanents(
            player, card, instance->targets[0].player, instance->targets[0].card,
            instance->targets[1].player, instance->targets[1].card);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 0x42);
    td.preferred_controller = player;
    td.allowed_controller = player;
    default_target_definition(player, card, &td2, 0x42);
    td.preferred_controller = 1 - player;
    td2.allowed_controller = td.preferred_controller;
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x215ca50; original cards/tempest.c:6893. */
int card_mogg_cannon(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allowed_controller = player;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      result = pump_ability_until_eot_2022(instance->parent_controller, instance->parent_card,
                                      instance->targets[0].player, instance->targets[0].card, 1, 0,
                                      0x20, 0x10000000);
      instance = get_card_instance(player, result);
      instance->targets[3].card = 2;
    }
  }
  result = generic_activated_ability(player, card, event, 5, 0, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x215ce60; original cards/tempest.c:7014. */
int card_rats_of_rath(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x43);
    td.allowed_controller = player;
    td.preferred_controller = player;
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 6, 0, 1, 0, 0, 0, 0, 0, &td,
                                "Select target artifact, creature, or land you control.");
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x43);
  }
  td.allowed_controller = player;
  td.preferred_controller = td.allowed_controller;
  if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
  {
    instance = get_card_instance(player, card);
    kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
  }
  result = generic_activated_ability(player, card, event, 4, 0, 1, 0, 0, 0, 0, 0, &td, NULL);
  return result;
}

/* ManalinkEh 2022: 0x215d260; original cards/tempest.c:7082. */
int card_reap(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  target_definition_t td;
  target_definition_t td_opp;
  test_definition_t this_test;

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
    else if ((event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET) &&
             (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 0xff);
  default_target_definition(player, card, &td_opp, 0);
  td_opp.zone = 0x1000;
  td_opp.allowed_controller = 1 - player;
  iVar1 = get_sleighted_color_test(player, card, 2);
  iVar1 = count_permanents_by_color(1 - player, 0x47, iVar1);
  new_default_test_definition(&this_test, 0xff,
                              "Select a card to return from your graveyard to your hand.");
  iVar1 = spell_return_up_to_n_target_cards_from_your_graveyard_to_your_hand(player, card, event,
                                                                             iVar1, &this_test, 0);
  iVar2 = can_target(&td_opp);
  if (iVar2 == 0)
  {
    return 0;
  }
  return iVar1;
}

/* ManalinkEh 2022: 0x215d3d0; original cards/tempest.c:7115. */
int card_rootwater_diver(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&this_test, 0x40, "Select an artifact card.");
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&this_test, 0x40, "Select an artifact card.");
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (result = validate_target_from_grave_source(player, card, player, 0), result != -1))
    {
      from_grave_to_hand(player, result, 1);
    }
  }
  result = generic_activated_ability_with_target_in_grave(
      player, card, event, GAA_SACRIFICE_ME | GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL,
      TYPE_NONE, &this_test, 8);
  return result;
}

/* ManalinkEh 2022: 0x215d700; original cards/tempest.c:7185. */
int card_shadowstorm(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&test, 2, "");
    test.keyword = 0x100;
    test.keyword_flag = 2;
    new_damage_all(player, card, 2, 2, 0, &test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x215dab0; original cards/tempest.c:7266. */
int card_spontaneous_combustion(int player, int card, event_t event)
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
    else if (event == EVENT_RESOLVE_SPELL)
    {
      new_damage_all(player, card, 2, 3, 1, NULL);
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  result = generic_spell(player, card, event, 0x400, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x215db90; original cards/tempest.c:7285. */
int card_squees_toy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  default_target_definition(player, card, &td, 0xff);
  td.extra = damage_card;
  td.special = 0x100000;
  instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    result = validate_target(player, card, &td, 0);
    if (result != 0)
    {
      prevent_damage(instance->targets[0].player, instance->targets[0].card, 1);
    }
  }
  result = generic_activated_ability(player, card, event, 0x1000005, 0, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_DAMAGE");
  return result;
}

/* ManalinkEh 2022: 0x215df40; original cards/tempest.c:7356. */
int card_thalakos_dreamsower(int player, int card, event_t event)
{
  int iVar1;
  int32_t iVar2;
  unsigned int uVar3;
  card_instance_t *pcVar4;
  card_instance_t *pcVar5;
  int iVar6;
  int times_damaged[2];
  target_definition_t td;

  shadow(player, card, event);
  choose_to_untap(player, card, event);
  uVar3 = whenever_i_deal_damage_to_a_player(player, card, event, DDBM_MUST_DAMAGE_OPPONENT);
  if (uVar3 != 0)
  {
    times_damaged[1] = uVar3 >> 8 & 0xff;
    times_damaged[0] = uVar3 & 0xff;
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 0;
    pcVar4 = get_card_instance(player, card);
    iVar1 = times_damaged[1 - player];
    while (((iVar1 != 0 && (iVar6 = can_target(&td), iVar6 != 0)) &&
            (iVar6 = new_pick_target(&td, "Select target creature to tap.", 0, 0x80), iVar6 != 0)))
    {
      tap_card(pcVar4->targets[0].player, pcVar4->targets[0].card);
      does_not_untap_until_im_tapped(player, card, pcVar4->targets[0].player,
                                     pcVar4->targets[0].card);
      pcVar5 = get_card_instance(player, card);
      iVar1 = iVar1 + -1;
      iVar2 = pcVar4->targets[0].player;
      pcVar5->targets[1].card = pcVar4->targets[0].card;
      pcVar5->targets[1].player = iVar2;
    }
    pcVar4->number_of_targets = '\0';
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x215e2f0; original cards/tempest.c:7441. */
int card_twitch(int player, int card, event_t event)
{
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
        default_target_definition(player, card, &td, 0x43);
        td.preferred_controller = 2;
        if (event != EVENT_RESOLVE_SPELL)
        {
          result = generic_spell(player, card, event, 1, &td, "TWIDDLE", 1, NULL);
          return result;
        }
        result = valid_target(&td);
        if (result != 0)
        {
          twiddle(player, card, 0);
          draw_a_card(player);
        }
        kill_card(player, card, KILL_DESTROY);
        result = generic_spell(player, card, EVENT_RESOLVE_SPELL, 1, &td, "TWIDDLE", 1, NULL);
        return result;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x43);
  td.preferred_controller = 2;
  result = generic_spell(player, card, event, 1, &td, "TWIDDLE", 1, NULL);
  if ((((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
       (affected_card_controller == player)) &&
      ((player == 1 && (spell_fizzled != 1))))
  {
    ai_modifier_twiddle(1, card, 0);
  }
  return result;
}

/* ManalinkEh 2022: 0x215e690; original cards/tempest.c:7528. */
int card_bandage(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x102);
    td.preferred_controller = player;
    td.zone = 0x1200;
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_ANY", 1, NULL);
    if (result != 0)
    {
      return (-(unsigned int)((land_can_be_played & 4U) == 0) & 0xffffff9e) + 99;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 0x102);
      td.preferred_controller = player;
      td.zone = 0x1200;
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance = get_card_instance(player, card);
        instance->number_of_targets = '\0';
        generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                      "Select any target to prevent the next 1 damage to it", 1, NULL);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x102);
    td.preferred_controller = player;
    td.zone = 0x1200;
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        prevent_the_next_n_damage(player, card, instance->targets[0].player,
                                  instance->targets[0].card, 1, 0, 0, 0);
        draw_a_card(player);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 0x102);
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x215a2c0; original cards/tempest.c:6106. */
int card_circle_of_protection_shadow(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x80);
    td.special = 0x8000;
    td.extra = (int32_t)prevent_damage_from_shadow_creature;
    td.illegal_abilities = 0;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = global_enchantment(player, card, event);
      return result;
    }
    default_target_definition(player, card, &td, 0x80);
    td.special = 0x8000;
    td.extra = (int32_t)prevent_damage_from_shadow_creature;
    td.illegal_abilities = 0;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        instance = get_card_instance(instance->targets[0].player, instance->targets[0].card);
        if (0 < instance->info_slot)
        {
          instance->info_slot = 0;
        }
      }
    }
  }
  result = generic_activated_ability(player, card, event, 0x1000006, 1, 0, 0, 0, 0, 0, 0, &td,
                                     "Select target damage from a creature with shadow.");
  return result;
}

/* ManalinkEh 2022: 0x215c590; original cards/tempest.c:6804. */
int card_marsh_lurker(int player, int card, event_t event)
{
  const char *pcVar1;
  int result;
  test_definition_t this_test;

  if (event == EVENT_ACTIVATE)
  {
    pcVar1 = get_hacked_land_text(player, card, "Select a %s to sacrifice.", 0x5009);
    new_default_test_definition(&this_test, 1, pcVar1);
    this_test.subtype = get_hacked_subtype(player, card, 0x5009);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    pcVar1 = get_hacked_land_text(player, card, "Select a %s to sacrifice.", 0x5009);
    new_default_test_definition(&this_test, 1, pcVar1);
    this_test.subtype = get_hacked_subtype(player, card, 0x5009);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      pump_ability_until_eot_2022(player, card, player, card, 0, 0, 0, 1);
    }
  }
  result = generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_NONE, 0, 1, 0,
                                                            0, 0, 0, 0, NULL, NULL, TYPE_NONE,
                                                            &this_test, SARV_0);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2148470; original cards/tempest.c:6357. */
static char *is_blocking_flowstone_salamander(int who_chooses, int player, int card,
                                              int targeting_player, int targeting_card)
{
  int result;

  result = is_blocking(card, targeting_card);
  if (result == 0)
  {
    return "must be blocking this attacking creature";
  }
  return NULL;
}

/* ManalinkEh 2022: 0x215bb00; original cards/tempest.c:6605. */
int card_imps_taunt(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    get_card_instance(player, card);
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      instance = get_card_instance(player, card);
      if ((card == affected_card) &&
          ((player == affected_card_controller &&
            (generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                           "Select target creature it attacks this turn if able.", 1, NULL),
             spell_fizzled != 1))))
      {
        result = buyback(player, card, 3, 0, 0, 0, 0, 0);
        instance->info_slot = result;
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if ((result == 0) || (pump_ability_until_eot_2022(player, card, instance->targets[0].player,
                                                   instance->targets[0].card, 0, 0, 0, 0x400),
                            instance->info_slot != 1))
      {
        kill_card(player, card, KILL_DESTROY);
        return 0;
      }
      bounce_permanent(player, card);
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 2);
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x215d510; original cards/tempest.c:7139. */
int card_rootwater_shaman(int player, int card, event_t event)
{
  int result;
  const char *prompt;
  target_definition_t td;

  if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_ACTIVATE))
  {
    return 0;
  }
  base_target_definition(player, card, &td, 4);
  td.required_subtype = 0x4002;
  td.preferred_controller = player;
  td.allowed_controller = player;
  td.zone = 0x100;
  if (event == EVENT_ACTIVATE)
  {
    prompt = "Select an aura card with enchant creature.";
  }
  else
  {
    prompt = NULL;
  }
  result = can_play_cards_as_though_they_had_flash(player, card, event, &td, prompt, 1);
  return result;
}

/* ManalinkEh 2022: 0x215e190; original cards/tempest.c:7417. */
int card_trumpeting_armodon(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      target_must_block_me(instance->parent_controller, instance->parent_card,
                           instance->targets[0].player, instance->targets[0].card, 1);
    }
  }
  result = generic_activated_ability(player, card, event, 6, 1, 0, 0, 1, 0, 0, 0, &td,
                                     "Select target creature that will be forced to block this.");
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
char *with_shadow(int who_chooses, int player, int card);
static int effect_maddening_imp(int player, int card, event_t event);

/* ManalinkEh 2022: 0x215ab10; original cards/tempest.c:6238. */
int card_dread_of_night(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;

  if (((event - EVENT_POWER < 2) &&
       (((card != affected_card || (player != affected_card_controller)) &&
         (instance = in_play(player, card), instance != NULL)))) &&
      ((instance = in_play(affected_card_controller, affected_card),
        instance != NULL &&
            (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))))
  {
    uVar3 = get_color(affected_card_controller, affected_card);
    uVar4 = get_sleighted_color_test(player, card, 0x20);
    if ((uVar3 & uVar4) != 0)
    {
      event_result = event_result + -1;
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x215b280; original cards/tempest.c:6428. */
int card_fylamarid(int player, int card, event_t event)
{
  card_instance_t *instance;
  const char *prompt;
  int result;
  color_test_t new_color;
  unsigned int uVar3;
  unsigned int uVar4;
  target_definition_t td;

  if (event == EVENT_BLOCK_LEGALITY)
  {
    if ((attacking_card_controller == player) && (attacking_card == card))
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        uVar3 = get_color(affected_card_controller, affected_card);
        uVar4 = get_sleighted_color_test(player, card, 4);
        if ((uVar3 & uVar4) != 0)
        {
          event_result = 1;
        }
      }
    }
    default_target_definition(player, card, &td, 2);
    get_card_instance(player, card);
  }
  else
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        new_color = get_sleighted_color_test(player, card, 4);
        change_color(instance->parent_controller, instance->parent_card,
                     instance->targets[0].player, instance->targets[0].card, new_color,
                     CHANGE_COLOR_END_AT_EOT | CHANGE_COLOR_SET);
      }
    }
  }
  prompt = get_sleighted_color_text(player, card,
                                    "Select target creature it becomes %s until end of turn.", 2);
  result = generic_activated_ability(player, card, event, 6, 0, 0, 1, 0, 0, 0, 0, &td, prompt);
  return result;
}

/* ManalinkEh 2022: 0x215b590; original cards/tempest.c:6485. */
int card_ghost_town(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uStack_18;

  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x40000000, 0, 0, 0, 0, 0,
                                      0, 0, NULL, NULL);
    if (iVar3 != 0)
    {
      return 1;
    }
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    if (iVar3 == 0)
    {
      get_card_instance(player, card);
      return 0;
    }
    event = EVENT_CAN_ACTIVATE;
    goto LAB_0215b7c1;
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      instance = get_card_instance(player, card);
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }
      if (instance->info_slot != 3)
      {
        return 0;
      }
      bounce_permanent(instance->parent_controller, instance->parent_card);
      return 0;
    }
    goto LAB_0215b7c1;
  }
  instance = get_card_instance(player, card);

  instance->info_slot = 0;
  iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL,
                                    NULL);
  iVar2 = paying_mana();
  uStack_18 = 0;
  if (iVar2 == 0)
  {
    iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x40000000, 0, 0, 0, 0, 0,
                                      0, 0, NULL, NULL);
    uStack_18 = (unsigned int)(iVar2 != 0);
  }
  if (iVar3 == 0)
  {
    iVar2 = paying_mana();
    if (iVar2 != 0)
    {
      spell_fizzled = 1;
      return 0;
    }
  LAB_0215b7f0:
    iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "Get mana", iVar3, 10,
                        "Return this to hand", uStack_18, 5, 0, 0, 0, 0);
    if (iVar3 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    if (iVar3 != 1)
    {
      instance->info_slot = iVar3;
      if (iVar3 == 2)
      {
        iVar3 = charge_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 0, 0);
        if (iVar3 != 0)
        {
          instance->info_slot = 3;
        }
        tapped_for_mana_color = -1;
      }
      return 0;
    }
  }
  else
  {
    iVar2 = paying_mana();
    if (iVar2 == 0)
      goto LAB_0215b7f0;
  }
  event = EVENT_ACTIVATE;
LAB_0215b7c1:
  iVar3 = mana_producer(player, card, event);
  return iVar3;
}

/* ManalinkEh 2022: 0x215c1d0; original cards/tempest.c:6706. */
int card_light_of_day(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;

  if (event == EVENT_ABILITIES)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      instance = in_play(player, card);
      if (instance != NULL)
      {
        uVar3 = get_color(affected_card_controller, affected_card);
        uVar4 = get_sleighted_color_test(player, card, 2);
        if ((uVar3 & uVar4) != 0)
        {
          cannot_attack_2022(affected_card_controller, affected_card, EVENT_ABILITIES);
          cannot_block(affected_card_controller, affected_card, EVENT_ABILITIES);
        }
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2148c40; original cards/tempest.c:6721. */
static int effect_maddening_imp(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int card_00;

  all_must_attack_if_able(player, event, -1);
  if (((event == EVENT_CLEANUP) && (card == affected_card)) && (player == affected_card_controller))
  {

    card_00 = active_cards_count[current_turn] + -1;
    if (-1 < card_00)
    {
      do
      {
        instance = in_play(current_turn, card_00);
        if (((instance != NULL) && (result = is_what(current_turn, card_00, 2), result != 0)) &&
            ((result = has_subtype(current_turn, card_00, SUBTYPE_WALL),
              result == 0 && (instance = get_card_instance(current_turn, card_00),
                              (instance->state & 0x20040) == 0))))
        {
          kill_card(current_turn, card_00, KILL_DESTROY);
        }
        card_00 = card_00 + -1;
      } while (card_00 != -1);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return 0;
}

/* ManalinkEh 2022: 0x215c2a0; original cards/tempest.c:6740. */
int card_maddening_imp(int player, int card, event_t event)
{
  int result;
  int player_00;

  if (event == EVENT_ACTIVATE)
  {
    result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x40000001, 0, 0, 0, 0, 0, 0,
                                       0, NULL, NULL);
    if (result == 0)
    {
      return 0;
    }
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    result =
        generic_activated_ability(player, card, event, 0x40000001, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
    if (result == 0)
      goto LAB_0215c33c;
  }
  if (current_phase < 0x15)
  {
    return 1;
  }
LAB_0215c33c:
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    result = get_original_internal_card_id(player, card);
    player_00 = 1 - player;
    result = add_card_to_hand(player_00, result);
    create_legacy_effect(player_00, result, effect_maddening_imp);
    obliterate_card(player_00, result);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2148b60; original cards/tempest.c:6825. */
char *with_shadow(int who_chooses, int player, int card)
{
  card_instance_t *instance;
  int result;

  instance = in_play(player, card);
  if (((instance != NULL) && (result = is_what(player, card, 2), result != 0)) &&
      (result = check_for_special_ability(player, card, 0x100), result != 0))
  {
    return NULL;
  }
  return "illegal target doesn\'t have shadow.";
}

/* ManalinkEh 2022: 0x215c730; original cards/tempest.c:6835. */
int card_maze_of_shadows(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      default_target_definition(player, card, &td, 2);
      td.required_state = 2;
      td.extra = (int32_t)with_shadow;
      td.special = 0x8000;
      instance = get_card_instance(player, card);

      result = paying_mana();
      if ((result == 0) && (result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5,
                                                               1, 0, 0, 0, 0, 0, 0, &td, NULL),
                            result != 0))
      {
        result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Get mana", 1, 1,
                             "Prevent damage", 1, 1, 0, 0, 0, 0);
        if (result == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        instance->info_slot = result;
        if (result != 1)
        {
          if (result != 2)
          {
            return 0;
          }
          generic_activated_ability(player, card, EVENT_ACTIVATE, 7, 0, 0, 0, 0, 0, 0, 0, &td,
                                    "Select target attacking creature with shadow");
          return 0;
        }
      }
      else
      {
        instance->info_slot = 1;
      }
      mana_producer(player, card, EVENT_ACTIVATE);
      return 0;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      default_target_definition(player, card, &td, 2);
      td.required_state = 2;
      td.extra = (int32_t)with_shadow;
      td.special = 0x8000;
      instance = get_card_instance(player, card);
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }
      if (instance->info_slot == 2)
      {
        result = validate_target(player, card, &td, 0);
        if (result != 0)
        {
          untap_card(instance->targets[0].player, instance->targets[0].card);
          maze_of_ith_effect(player, instance->parent_card, instance->targets[0].player,
                             instance->targets[0].card);
          return 0;
        }
        return 0;
      }
      return 0;
    }
  }
  result = mana_producer(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x215cbe0; original cards/tempest.c:6928. */
int card_mogg_squad(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  if (((event - EVENT_POWER < 2) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      default_test_definition(&this_test, 2);
      this_test.not_me = 1;
      result = check_battlefield_for_special_card(player, card, 2, 4, &this_test);
      event_result = event_result - result;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x215cdd0; original cards/tempest.c:6988. */
int card_pallimud(int player, int card, event_t event)
{
  int newval;
  test_definition_t this_test;

  if ((((event == EVENT_POWER) && (card == affected_card)) &&
       (affected_card_controller == player)) &&
      (player != -1))
  {
    default_test_definition(&this_test, 1);
    this_test.state = 0x10;
    newval = check_battlefield_for_special_card(player, card, 1 - player, 4, &this_test);
    set_characteristic_pt(newval);
  }
  return 0;
}

/* ManalinkEh 2022: 0x215e0a0; original cards/tempest.c:7399. */
int card_thalakos_mistfolk(int player, int card, event_t event)
{
  int result;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;

  shadow(player, card, event);
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    pcVar2 = get_card_instance(player, card);
    pcVar3 = in_play(pcVar2->parent_controller, pcVar2->parent_card);
    if (pcVar3 != NULL)
    {
      put_on_top_of_deck(pcVar2->parent_controller, pcVar2->parent_card);
    }
  }
  result = generic_activated_ability(player, card, event, 0, 0, 0, 1, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x215d010; original cards/tempest.c:7045. */
int card_reality_anchor(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 2);
        if ((card != affected_card) || (player != affected_card_controller))
          goto LAB_0215d064;
        instance = get_card_instance(player, card);
        generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                      "Select target creature it loses shadow until end of turn.", 1, NULL);
        if (spell_fizzled != 1)
        {
          result = check_for_special_ability(instance->targets[0].player, instance->targets[0].card,
                                             0x100);
          ai_modifier = (-(unsigned int)(result == 0) & 0xffffffdc) + ai_modifier + 0x48 +
                        ((instance->targets[0].player == 1 - player) - 1 & 0xffffffa0);
        }
      }
      return 0;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 2);
      if (event == EVENT_RESOLVE_SPELL)
      {

        instance = get_card_instance(player, card);
        result = valid_target(&td);
        if (result != 0)
        {
          negate_ability_and_special_ability_until_eot(player, card, instance->targets[0].player,
                                                       instance->targets[0].card, KEYWORD_0, 0x100);
          draw_a_card(player);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_0215d064;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 2);
LAB_0215d064:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target creature it loses shadow until end of turn.", 1, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21484a0; original cards/tempest.c:4222. */
static int sadistic_glee_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  card_instance_t *pcVar3;
  int result;
  int player_00;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0xcba))
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
  LAB_021484d4:
    uVar2 = resolve_gfp_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto LAB_021484f9;
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
      goto LAB_021484d4;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);
    if ((pcVar3 != NULL) &&
        (result = is_this_dying(affected_card_controller, affected_card), result != 0))
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
      count_for_gfp_ability(player, card, 0x77, 2, 2, NULL);
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

    pcVar1 = get_card_instance(player, card);
    pcVar1 = get_card_instance(pcVar1->targets[0].player, pcVar1->targets[0].card);
    player_00 = (int)pcVar1->damage_target_player;
    result = pcVar1->damage_target_card;
    if (((-1 < player_00) && (-1 < result)) &&
        (pcVar1 = in_play(player_00, result), pcVar1 != NULL))
    {

      for (uVar2 = uVar2 & 0xff; uVar2 != 0; uVar2 = uVar2 - 1)
      {
        add_counters_from_source(player, card, player_00, result, COUNTER_P1_P1, 1);
      }
    }
    kill_card(player, card, KILL_EXILE);
  }
LAB_021484f9:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2159870; original cards/tempest.c:5918. */
int card_bounty_hunter(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  const char *prompt;
  target_definition_t *ptStack_c0;
  card_instance_t *instance;
  target_definition_t td;
  target_definition_t td2;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    iVar4 = 1 - player;
    td.preferred_controller = iVar4;
    td.illegal_color = get_sleighted_color_test(player, card, 2);
    default_target_definition(player, card, &td2, 2);
    td2.special = 0x40000;
    td2.extra = 0x14;
    td2.preferred_controller = iVar4;
    if (event == EVENT_CAN_ACTIVATE)
    {

      iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if (iVar4 == 0)
      {
        return 0;
      }
      iVar4 = can_target(&td);
      if ((iVar4 == 0) && (iVar4 = can_target(&td2), iVar4 == 0))
      {
        return 0;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->info_slot = 0;
      instance->number_of_targets = '\0';
      iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                        &td, NULL);
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                        &td2, NULL);
      iVar4 = count_counters(iVar4, card, COUNTER_BOUNTY);
      iVar3 = can_target(&td2);
      iVar4 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Add Bounty counter", iVar1,
                          (unsigned int)(iVar4 == 0), "Destroy creature with bounty", iVar2,
                          (unsigned int)(iVar3 != 0), 0, 0, 0, 0);
      if (iVar4 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      instance->info_slot = iVar4;
      if (iVar4 == 1)
      {
        iVar4 = generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        if (iVar4 != 0)
        {
          prompt = get_sleighted_color_text(player, card, "Select target non%s creature", 1);
          new_pick_target(&td, prompt, 0, 0x81);
          if (spell_fizzled == 1)
          {
            remove_state(player, card, 0x10);
          }
        }
        iVar4 =
            count_counters(instance->targets[0].player, instance->targets[0].card, COUNTER_BOUNTY);
        if ((((iVar4 < 1) && (iVar4 = can_target(&td), iVar4 != 0)) && (current_turn != player)) &&
            (current_phase == 0x1f))
        {
          ai_modifier = ai_modifier + 0x30;
          return 0;
        }
        ai_modifier = ai_modifier + -0x30;
        return 0;
      }
      if (iVar4 != 2)
      {
        return 0;
      }
      iVar4 = generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL,
                                        NULL);
      if ((iVar4 != 0) &&
          (new_pick_target(&td2, "Select target creature with a bounty counter to destroy.", 1,
                           0x81),
           spell_fizzled == 1))
      {
        remove_state(player, card, 0x10);
      }
      iVar4 =
          count_counters(instance->targets[1].player, instance->targets[1].card, COUNTER_BOUNTY);
      if (iVar4 < 1)
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
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    td.illegal_color = get_sleighted_color_test(player, card, 2);
    default_target_definition(player, card, &td2, 2);
    td2.preferred_controller = 1 - player;
  }
  ptStack_c0 = &td;
  td2.special = 0x40000;
  td2.extra = 0x14;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    if (instance->info_slot == 1)
    {
      iVar4 = valid_target(ptStack_c0);
      if (iVar4 != 0)
      {
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_BOUNTY, 1);
      }
    }
    else if ((instance->info_slot == 2) &&
             (iVar4 = validate_target(player, card, &td2, 1), iVar4 != 0))
    {
      kill_card(instance->targets[1].player, instance->targets[1].card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x215d8e0; original cards/tempest.c:7227. */
int card_souldrinker(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    }
  }
  result = generic_activated_ability(player, card, event, 0, 0, 0, 0, 0, 0, 0, 3, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void bellowing_fiend_effect(int player, int card, int t_player, int t_card);
int damage_controller_and_you(int player, int card, event_t event);

/* ManalinkEh 2022: 0x214b120; original cards/tempest.c:762. */
int card_booby_trap(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  test_definition_t test;

  if (event == EVENT_RESOLVE_SPELL)
  {

    instance = get_card_instance(player, card);
    new_default_test_definition(&test, 0xff, "Select a card other than a basic land.");
    test.subtype = 0x2000;
    test.subtype_flag = 1;
    if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
    {
      iVar1 = ai_name_a_random_csvid(player, card, 1 - player, TYPE_NONE, &test);
    }
    else
    {
      iVar1 = new_card_from_list(player, 3, &test);
    }
    if (-1 < iVar1)
    {
      instance->info_slot = iVar1;
      create_card_name_legacy(player, card, iVar1);
    }
  }
  if ((((trigger_condition == 0xd0) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (((reason_for_trigger_controller != player && (trigger_cause_controller != player)) &&
        (suppress_draw == 0))))
  {
    iVar1 = is_humiliated_for_ability(player, card, AL_TRIG);
    if (iVar1 == 0)
    {

      instance = get_card_instance(player, card);
      if (event == EVENT_TRIGGER)
      {
        event_result = event_result | 2;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {

        iVar1 = get_original_internal_card_id(trigger_cause_controller, trigger_cause);
        iVar3 = get_internal_card_id_from_csv_id(instance->info_slot);
        reveal_card_iid(player, card, iVar1);
        if (iVar1 == iVar3)
        {
          damage_player(1 - player, 10, player, card);
          kill_card(player, card, KILL_SACRIFICE);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x214fa10; original cards/tempest.c:2359. */
int card_fireslinger(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    instance = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      damage_target0(player, card, 1);
      damage_player(instance->parent_controller, 1, instance->parent_controller,
                    instance->parent_card);
    }
  }
  result =
      generic_activated_ability(player, card, event, 5, 0, 0, 0, 0, 0, 0, 0, &td, "TARGET_ANY");
  return result;
}

/* ManalinkEh 2022: 0x21489c0; original cards/tempest.c:5867. */
int damage_controller_and_you(int player, int card, event_t event)
{
  card_instance_t *instance;

  if (event != EVENT_RESOLVE_TRIGGER)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  damage_player((int)instance->damage_target_player, 3, player, card);
  damage_player(player, 3, player, card);
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x2147be0; original cards/tempest.c:5878. */
void bellowing_fiend_effect(int player, int card, int t_player, int t_card)
{
  create_targetted_legacy_effect(player, card, damage_controller_and_you, t_player, t_card);
  return;
}

/* ManalinkEh 2022: 0x2159710; original cards/tempest.c:5882. */
int card_bellowing_fiend(int player, int card, event_t event)
{
  int result;

  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result != 0)
  {
    return 0;
  }
  for_each_creature_damaged_by_me(player, card, event, 0, bellowing_fiend_effect, player, card);
  return 0;
}

/* ManalinkEh 2022: 0x2159780; original cards/tempest.c:5895. */
int card_blood_pet(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = can_use_activated_abilities_2022(player, card);
    if (result != 0)
    {
      result = can_sacrifice_this_as_cost(player, card);
      return result;
    }
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      produce_mana(player, 1, 1);
      tapped_for_mana_color = -2;
      kill_card(player, card, KILL_SACRIFICE);
      return 0;
    }
    if (((event == EVENT_COUNT_MANA) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      result = can_sacrifice_this_as_cost(player, card);
      if (result != 0)
      {
        result = can_use_activated_abilities_2022(player, card);
        if (result != 0)
        {
          declare_mana_available(player, 1, 1);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x215e5f0; original cards/tempest.c:7504. */
int card_watchdog(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  block_if_able_2022(player, card, event);
  instance = in_play(player, card);
  if (instance != NULL)
  {
    result = is_tapped(player, card);
    if (result == 0)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (((event == EVENT_POWER) && (result == 0)) && (1 - player == affected_card_controller))
      {
        result = is_attacking(1 - player, affected_card);
        if (result != 0)
        {
          event_result = event_result + -1;
        }
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x215b970; original cards/tempest.c:6558. */
int card_havoc(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0xff);
    test.color = get_sleighted_color_test(player, card, 0x20);
    sp_ability_trigger(1 - player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_SPA)
  {
    lose_life_2022(1 - player, 2);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x215bd80; original cards/tempest.c:6645. */
int card_insight(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0xff);
    test.color = get_sleighted_color_test(player, card, 8);
    sp_ability_trigger(1 - player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_SPA)
  {
    draw_cards(player, 1);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x215e510; original cards/tempest.c:7485. */
int card_warmth(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (((trigger_condition == 0xd3) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 0xff);
    test.color = get_sleighted_color_test(player, card, 0x10);
    sp_ability_trigger(1 - player, event, 2, TYPE_NONE, &test, player, card);
  }
  if (event == EVENT_SPA)
  {
    gain_life_2022(player, 2);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x215a1a0; original cards/tempest.c:6053. */
int card_chaotic_goo(int player, int card, event_t event)
{
  int result;

  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, 3);
  result = duh_mode(player);
  upkeep_trigger_ability_mode(player, card, event, player,
                              (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    return 0;
  }
  result = flip_a_coin(player, card);
  if (result != 0)
  {
    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    return 0;
  }
  remove_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  return 0;
}

/* ManalinkEh 2022: 0x215c430; original cards/tempest.c:6765. */
int card_magmasaur(int player, int card, event_t event)
{
  int dmg;
  int result;
  test_definition_t this_test;

  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, 5);
  upkeep_trigger_ability_mode(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    return 0;
  }

  dmg = count_counters(player, card, COUNTER_P1_P1);
  default_test_definition(&this_test, 2);
  this_test.keyword = 0x20;
  this_test.keyword_flag = 1;
  result = do_dialog(player, player, card, -1, -1, " Remove counter\n sacrifice & damage", 0);
  if (result == 0)
  {
    remove_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
    return 0;
  }
  kill_card(player, card, KILL_SACRIFICE);
  new_damage_all(player, card, 2, dmg, 2, &this_test);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x215d5b0; original cards/tempest.c:7158. */
int card_segmented_wurm(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  const target_t *ptVar3;

  instance = in_play(player, card);
  if (instance != NULL)
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      ptVar3 = becomes_target_of_spell_or_effect(player, card, event, player, card, 2);
      if (ptVar3 != NULL)
      {
        add_counters_from_source(player, card, player, card, COUNTER_M1_M1, 1);
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x215d640; original cards/tempest.c:7172. */
int card_servant_of_volrath(int player, int card, event_t event)
{
  leaves_play_2022(player, card, event, 1, 1);
  if (event != EVENT_LEAVES_PLAY_ABILITY)
  {
    return 0;
  }
  impose_sacrifice(player, card, player, 1, 2, 0, 0, 0, 0, 0, 0, 0, -1, 0);
  return 0;
}

/* ManalinkEh 2022: 0x215dc90; original cards/tempest.c:7306. */
int card_steal_enchantment(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  leaves_play_2022(player, card, event, 0, 0);
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_0215dce8;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_0215dce8;
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
           (event != EVENT_CHANGE_TYPE)))) &&
        (((event != EVENT_SET_COLOR && ((event & ~EVENT_UPKEEP) != EVENT_GET_SELECTED_CARD)) &&
          ((event != EVENT_STATIC_EFFECTS && (event != EVENT_LEAVES_PLAY_ABILITY))))))
    {
      return 0;
    }
  }
LAB_0215dce8:
  default_target_definition(player, card, &td, 4);
  result = generic_stealing_aura(player, card, event, &td, "TARGET_ENCHANTMENT");
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2148db0; original cards/tempest.c:13. */
int tempest_does_not_untap_land(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;
  color_test_t cVar5;
  unsigned int uVar6;
  unsigned int uVar7;
  int iStack_18;
  color_test_t cStack_14;

  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))) &&
         (life[player] < 2)) &&
        (instance = get_card_instance(player, card), (instance->mana_color & 1) == 0))
    {
      return 0;
    }
  LAB_02148ddd:
    result = mana_producer(player, card, event);
    return result;
  }
  if (event == EVENT_ACTIVATE)
  {

    instance = get_card_instance(player, card);
    cVar5 = (unsigned int)instance->mana_color | mana_colors_added[player];
    if ((((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))) &&
         (life[player] < 2)) &&
        ((cVar5 & COLOR_TEST_COLORLESS) != COLOR_TEST_0))
    {
      produce_mana_tapped_all_one_color_2022(player, card,
                                             mana_colors_added[player] | COLOR_TEST_COLORLESS, 1);
    }
    else
    {
      produce_mana_tapped_all_one_color_2022(player, card, cVar5, 1);
      result = check_special_flags2_2022(player, card, 0x40);
      if (result == 0)
      {
        if ((chosen_colors &
             ((mana_colors_added[player] | COLOR_TEST_COLORLESS) ^ ~COLOR_TEST_0)) != COLOR_TEST_0)
        {
          does_not_untap_effect(player, card, player, card, 8, 1);
        }
      }
      else
      {

        cStack_14 = COLOR_TEST_0;
        uVar6 = 0;
        while (uVar7 = uVar6, uVar6 = uVar7 + 1, uVar6 != 7)
        {
          if (((mana_colors_added[player] | 1U) & uVar6) != 0)
          {
            cVar5 = color_to_color_test(mana_color_override[player][uVar7 + 1]);
            cStack_14 = cStack_14 | cVar5;
            if ((chosen_colors & (cStack_14 ^ ~COLOR_TEST_0)) != COLOR_TEST_0)
            {
              does_not_untap_effect(player, card, player, card, 8, 1);
            }
          }
        }
      }
    }
  }
  else
  {
    if (((event != EVENT_COUNT_MANA) || (card != affected_card)) ||
        (player != affected_card_controller))
      goto LAB_02148ddd;
    result = is_tapped(player, card);
    if (((result == 0) && (result = is_animated_and_sick(player, card), result == 0)) &&
        (result = can_produce_mana_2022(player, card), result != 0))
    {
      result = check_special_flags2_2022(player, card, 0x40);

      if (result == 0)
      {

        instance = get_card_instance(player, card);
        uVar6 = mana_colors_added[player] | (unsigned int)instance->mana_color;
        if (((ai_is_speculating == 1) ||
             ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))) &&
            (life[player] < 2))
        {
          if ((uVar6 & 1) != 0)
          {
            declare_mana_available(player, 0, 1);
          }
        }
        else
        {
          declare_mana_available_hex(player, uVar6, 1);
        }
      }
      else
      {
        instance = get_card_instance(player, card);
        uVar6 = (unsigned int)instance->mana_color | mana_colors_added[player];
        if (((ai_is_speculating == 1) ||
             ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))) &&
            (life[player] < 2))
        {
          if ((uVar6 & 1) != 0)
          {
            result = mana_quantity_override[player][0];
            if (result < 0)
            {
              result = 1;
            }
            declare_mana_available(player, mana_color_override[player][0], result);
          }
        }
        else
        {
          uVar7 = 0;
          iStack_18 = 1;
          do
          {
            uVar3 = color_to_color_test(uVar7);
            if (((uVar3 & uVar6) != 0) && (mana_color_override[player][uVar7] != 0))
            {
              uVar3 = color_to_color_test(uVar7);
              uVar4 = color_to_color_test(mana_color_override[player][uVar7]);
              uVar6 = uVar6 & (uVar3 ^ 0xffffffff) | uVar4;
              if (-1 < mana_quantity_override[player][uVar7])
              {
                iStack_18 = mana_quantity_override[player][uVar7];
              }
            }

            uVar7 = uVar7 + 1;
          } while (uVar7 < 7);
          declare_mana_available_hex(player, uVar6, iStack_18);
        }
      }
    }
  }
  return 0;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x215abf0; original cards/tempest.c:6264. */
int card_endless_scream(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int32_t number;

  instance = get_card_instance(player, card);
  number = x_value;
  if (((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance->info_slot = x_value;
    if (player == 1)
    {
      ai_modifier = ai_modifier + (number * 3 + -9) * 8;
    }
  }
  else
  {
    number = instance->info_slot;
  }
  enters_the_battlefield_with_counters(player, card, event, COUNTER_SCREAM, number);
  attached_gets_pt(player, card, event, instance->info_slot, 0);
  result = aura(player, card, event, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x215b160; original cards/tempest.c:6395. */
int card_frog_tongue(int player, int card, event_t event)
{
  int result;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    draw_a_card(player);
  }
  attached_gets_kw(player, card, event, KEYWORD_REACH, SP_KEYWORD_0);
  result = aura(player, card, event, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x215baa0; original cards/tempest.c:6594. */
int card_heros_resolve(int player, int card, event_t event)
{
  int result;

  attached_gets_pt(player, card, event, 1, 5);
  result = aura(player, card, event, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x215d7b0; original cards/tempest.c:7204. */
int card_shimmering_wings(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = in_play(player, card);
  if (instance != NULL)
  {
    instance = get_card_instance(player, card);
    if (-1 < instance->damage_target_player)
    {

      instance = get_card_instance(player, card);
      if (event != EVENT_ACTIVATE)
      {
        if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
          goto LAB_0215d7e4;
        if (event == EVENT_RESOLVE_ACTIVATION)
        {
          bounce_permanent(instance->parent_controller, instance->parent_card);
        }
      }
      result = generic_activated_ability(player, card, event, 0, 0, 0, 1, 0, 0, 0, 0, NULL, NULL);
      return result;
    }
  }
LAB_0215d7e4:
  attached_gets_kw(player, card, event, KEYWORD_FLYING, SP_KEYWORD_0);
  result = aura(player, card, event, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x215d9a0; original cards/tempest.c:7244. */
int card_spinal_graft(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  const target_t *ptVar4;

  pcVar1 = get_card_instance(player, card);
  pcVar2 = in_play(player, card);
  if ((((pcVar2 != NULL) && (-1 < pcVar1->damage_target_card)) &&
       (pcVar2 = in_play((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
        pcVar2 != NULL)) &&
      ((result = is_humiliated_for_ability(player, card, AL_TRIG),
        result == 0 && (ptVar4 = becomes_target_of_spell_or_effect(
                            player, card, event, (int)pcVar1->damage_target_player,
                            pcVar1->damage_target_card, 2),
                        ptVar4 != NULL))))
  {
    kill_card((int)pcVar1->damage_target_player, pcVar1->damage_target_card, KILL_BURY);
    return 1;
  }
  attached_gets_pt(player, card, event, 3, 3);
  result = aura(player, card, event, NULL, NULL);
  return result;
}
#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *is_already_damaged(int who_chooses, int player, int card);

/* ManalinkEh 2022: 0x2147060; original cards/tempest.c:6950. */
static char *is_already_damaged(int who_chooses, int player, int card)
{
  card_instance_t *instance;
  char *pcVar2;

  instance = get_card_instance(player, card);
  pcVar2 = NULL;
  if (instance->damage_on_card == 0)
  {
    pcVar2 = "must be a creature that was dealt damage this turn.";
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x215cc90; original cards/tempest.c:6956. */
int card_opportunist(int player, int card, event_t event)
{
  int result;
  target_definition_2022_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition_2022(player, card, &td, 2);
    td.extra = (int32_t)is_already_damaged;
    td.special = 0x8000;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition_2022(player, card, &td, 2);
    td.extra = (int32_t)is_already_damaged;
    td.special = 0x8000;
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target_2022(&td), result != 0))
    {
      damage_target0(player, card, 1);
    }
  }
  result =
      generic_activated_ability_2022(player, card, event, 7, 0, 0, 0, 0, 0, 0, 0, &td,
                                     "Select target creature that was dealt damage this turn.");
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x215b1f0; original cards/tempest.c:6414. */
int card_fugitive_druid(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_t *ptVar3;
  instance = in_play(player, card);
  if (instance != NULL)
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      ptVar3 = becomes_target_of_subtype_spell_2022(player, card, event, player, card, 2,
                                                    RESOLVE_TRIGGER_MANDATORY, 0x4000);
      if (ptVar3 != NULL)
      {
        draw_cards(player, 1);
      }
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x215a450; original cards/tempest.c:6136. */
int card_crown_of_flames(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  if (event == EVENT_SHOULD_AI_PLAY)
  {
    instance = get_card_instance(player, card);
    iVar1 = 0;
    if (instance->damage_target_player == player)
    {
      iVar1 = has_mana(player, 4, 1);
      iVar1 = iVar1 * (((player == 1) - 1 & 0xffffffe8) + 0xc);
    }
    ai_modifier = ai_modifier + iVar1;
  }
  else if (event == EVENT_CHECK_ABILITIES)
  {
    instance = get_card_instance(player, card);
    iVar1 = instance->damage_target_card;
    instance = get_card_instance(player, card);
    if ((((iVar1 == affected_card) &&
          (instance->damage_target_player == affected_card_controller)) &&
         (instance = get_card_instance(player, card), instance->damage_target_player == player)) &&
        ((instance = in_play(player, card),
          instance != NULL &&
              (iVar1 = can_use_activated_abilities_2022(player, card), iVar1 != 0))))
    {
      iVar1 = has_mana(player, 4, 1);
      check_abilities_power_2022 = check_abilities_power_2022 + iVar1;
    }
  }
  iVar1 = is_humiliated_for_ability(player, card, AL_NM_NL_ACT);
  if ((iVar1 == 0) &&
      (instance = get_card_instance(player, card), instance->damage_target_player != -1))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar1 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 0, 0, 0, 0, 0, 1, 0,
                                             0, NULL, NULL);
      return iVar1;
    }
    if (event == EVENT_ACTIVATE)
    {
      iVar1 = has_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 1, 0);
      iVar3 = has_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 1, 0);
      iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Boost enchanted +1/0.", iVar3,
                          0x14, "Bounce Crown of Flames.", iVar1, 5, 0, 0, 0, 0);
      if (iVar1 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      iVar1 = generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 0, 0, 0, 0, 0, 1, 0, 0,
                                             NULL, NULL);
      return iVar1;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      iVar1 = instance->info_slot;
      if (iVar1 == 1)
      {
        pump_until_eot_merge_previous_2022(instance->parent_controller, instance->parent_card,
                                           (int)instance->damage_target_player,
                                           instance->damage_target_card, 1, 0);
        iVar1 = instance->info_slot;
      }
      if (iVar1 == 2)
      {
        bounce_permanent(instance->parent_controller, instance->parent_card);
      }
    }
  }
  iVar1 = vanilla_aura_2022(player, card, event, player);
  return iVar1;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x215acc0; original cards/tempest.c:6305. */
int card_flailing_drake(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  if (event == EVENT_CHECK_ABILITIES)
  {
    if (((card == affected_card) && (player == affected_card_controller)) && (current_phase < 0x18))
    {
      check_abilities_power_2022 = check_abilities_power_2022 + -1;
      check_abilities_toughness_2022 = check_abilities_toughness_2022 + -1;
    }
  }
  else if (event == EVENT_DECLARE_BLOCKERS)
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      instance = get_card_instance(player, card);
      if ((current_turn == player) && ((instance->state & 4) != 0))
      {
        for_each_creature_blocking_me(current_turn, card, blocked_by_me_or_my_blocker_gets_1_1,
                                      current_turn, card);
      }
      if ((1 - current_turn == player) && ((instance->state & 8) != 0))
      {
        for_each_creature_blocked_by_me(player, card, blocked_by_me_or_my_blocker_gets_1_1, player,
                                        card);
      }
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x215adf0; original cards/tempest.c:6329. */
int card_flowstone_giant(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  if (event == EVENT_CHECK_ABILITIES)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      uVar2 = get_toughness(player, card);
      if ((uVar2 & 1) == 0)
      {
        result = (int)uVar2 / 2 + -1;
      }
      else
      {
        result = (int)(uVar2 - 1) / 2;
      }
      result = generic_shade_amt_can_pump_2022(player, card, 2, 0, 0, 0, 0, 0, 1, 0, result);
      check_abilities_power_2022 = check_abilities_power_2022 + result;
      check_abilities_toughness_2022 = check_abilities_toughness_2022 - result;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    result =
        generic_shade_merge_pt(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0, 0, 0, 0, 1, 0, 2, -2);
    return result;
  }
  result = generic_activated_ability_2022(player, card, event, 0, 0, 0, 0, 0, 1, 0, 0, NULL, NULL);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x215afa0; original cards/tempest.c:6364. */
int card_flowstone_salamander(int player, int card, event_t event)
{
  int result;
  target_definition_2022_t td2;
  if (event == EVENT_CHECK_ABILITIES)
  {
    if (((((current_turn == player) && (card == affected_card)) &&
          (player == affected_card_controller)) &&
         ((current_phase < 0x18 &&
           (result = can_use_activated_abilities_2022(player, card), result != 0)))) &&
        (result = has_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 0, 0), result != 0))
    {
      result = has_mana(player, 4, 1);
      check_abilities_power_2022 = check_abilities_power_2022 + result;
      check_abilities_toughness_2022 = check_abilities_toughness_2022 + result;
    }
  }
  else if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
           ((event == EVENT_RESOLVE_ACTIVATION || (event == EVENT_CLEANUP))))
  {
    default_target_definition_2022(player, card, &td2, 2);
    td2.special = 0x8000;
    td2.extra = (int32_t)is_blocking_flowstone_salamander;
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target_2022(&td2), result != 0))
    {
      damage_target0(player, card, 1);
    }
    result =
        generic_activated_ability_2022(player, card, event, 6, 0, 0, 0, 0, 1, 0, 0, &td2,
                                       "Select target creature blocking this deal 1 damage to it.");
    return result;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x215dda0; original cards/tempest.c:7324. */
int card_tahngarths_rage(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;

  pcVar1 = get_card_instance(player, card);
  if ((((event == EVENT_CHECK_ABILITIES) && (pcVar1->damage_target_card == affected_card)) &&
       (affected_card_controller == pcVar1->damage_target_player)) &&
      (pcVar1->damage_target_player == player))
  {
    pcVar3 = in_play(player, card);
    if ((pcVar3 != NULL) && (current_phase < 0x18))
    {
      check_abilities_power_2022 = check_abilities_power_2022 + 5;
      check_abilities_toughness_2022 = check_abilities_toughness_2022 + 1;
    }
  }
  result = is_humiliated_for_ability(player, card, AL_STATIC);
  if (result == 0)
  {
    pcVar3 = get_card_instance(player, card);
    if (-1 < pcVar3->damage_target_player)
    {
      if (event == EVENT_POWER)
      {
        if (pcVar1->damage_target_card == affected_card)
        {
          pcVar3 = in_play(player, card);
          if (pcVar3 != NULL)
          {
            result = is_attacking((int)pcVar1->damage_target_player, pcVar1->damage_target_card);
            if (result == 0)
            {
              event_result = event_result + -2;
            }
            else
            {
              event_result = event_result + 3;
            }
          }
        }
      }
      else if ((event == EVENT_TOUGHNESS) && (pcVar1->damage_target_card == affected_card))
      {
        pcVar3 = in_play(player, card);
        if (pcVar3 != NULL)
        {
          result = is_attacking((int)pcVar1->damage_target_player, pcVar1->damage_target_card);
          if (result == 0)
          {
            event_result = event_result + -1;
          }
        }
      }
    }
  }
  result = aura(player, card, event, NULL, NULL);
  return result;
}

#endif
