#include "manalink.h"

// Functions
static int control_an_untapped_land(int player){
	int i;
	for(i=0; i<2; i++){
		if( player == i || player == 2 ){
			int count = 0;
			while( count < active_cards_count[i] ){
					if( in_play(i, count) && is_what(i, count, TYPE_LAND) && ! is_tapped(i, count) ){
						return 1;
					}
					count++;
			}
		}
	}
	return 0;
}


// Cards
int card_abolish(int player, int card, event_t event){
	/*
	  Abolish |1|W|W
	  Instant
	  You may discard a Plains card rather than pay Abolish's mana cost.
	  Destroy target artifact or enchantment.
	*/

	if( event == EVENT_MODIFY_COST ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_LAND, "Select a Plains card.");
		this_test.zone = TARGET_ZONE_HAND;
		this_test.subtype = SUBTYPE_PLAINS;	// can't hack in time
		if( check_battlefield_for_special_card(player, card, player, 0, &this_test) ){
			null_casting_cost(player, card);
			get_card_instance(player, card)->info_slot = 1;
		}
		else{
			get_card_instance(player, card)->info_slot = 0;
		}
	}

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT | TYPE_ENCHANTMENT);

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_LAND, "Select a Plains card.");
	this_test.zone = TARGET_ZONE_HAND;
	this_test.subtype = SUBTYPE_PLAINS;	// can't hack in time

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if(event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		if( ! played_for_free(player, card) && ! is_token(player, card) && instance->info_slot == 1 ){
			int choice = 0;
			if( has_mana_to_cast_id(player, event, get_id(player, card)) ){
				choice = do_dialog(player, player, card, -1, -1, " Discard a Plains card\n Play normally\n Cancel", 1);
			}

			if( choice == 0 ){
				int selected = new_select_a_card(player, player, TUTOR_FROM_HAND, 0, AI_MIN_VALUE, -1, &this_test);
				if( selected > -1 ){
					discard_card(player, selected);
					td.allow_cancel = 0;
				}
				else{
					spell_fizzled = 1;
				}
			}
			else if( choice == 1 ){
					charge_mana_from_id(player, card, event, get_id(player, card));
			}
			else{
				spell_fizzled = 1;
			}
			if( spell_fizzled == 1 ){
				return 0;
			}
		}
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "DISENCHANT", 1, NULL);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_alexi_zephyr_mage(int player, int card, event_t event){
/*
Alexi, Zephyr Mage |3|U|U
Legendary Creature — Human Spellshaper 3/3, 3UU (5)
{X}{U}, {T}, Discard two cards: Return X target creatures to their owners' hands.
*/
	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE && hand_count[player] > 1 ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_U(1), 0, &td, NULL);
	}

	if(event == EVENT_ACTIVATE ){
		int trgs = 0;
		while( has_mana_for_activated_ability(player, card, MANACOST_XU(trgs, 1)) && can_target(&td) ){
				if( new_pick_target(&td, "TARGET_CREATURE", trgs, 0) ){
					state_untargettable(instance->targets[trgs].player, instance->targets[trgs].card, 1);
					trgs++;
				}
				else{
					break;
				}
		}
		if( trgs == 0 ){
			spell_fizzled = 1;
		}
		else{
			int i;
			for(i=0; i<trgs; i++){
				state_untargettable(instance->targets[i].player, instance->targets[i].card, 0);
			}
			instance->number_of_targets = trgs;
			charge_mana_for_activated_ability(player, card, MANACOST_XU(trgs, 1));
			if( spell_fizzled != 1 ){
				tap_card(player, card);
				multidiscard(player, 2, 0);
				instance->info_slot = trgs;
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int i;
		for(i=0; i<instance->info_slot; i++){
			if( validate_target(player, card, &td, i) ){
				bounce_permanent(instance->targets[i].player, instance->targets[i].card);
			}
		}
	}

	return 0;
}

int card_aura_fracture(int player, int card, event_t event){
/*
Aura Fracture |2|W
Enchantment
Sacrifice a land: Destroy target enchantment.
*/
	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ENCHANTMENT);

	test_definition_t test;
	new_default_test_definition(&test, TYPE_LAND, "Select a land to sacrifice.");

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
	}

	return generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_CAN_TARGET, MANACOST0, 0, &td, "TARGET_ENCHANTMENT",
															0, &test, 0);
}

int card_avatar_of_hope(int player, int card, event_t event)
{
/* CARD_ID_AVATAR_OF_HOPE	975
Avatar of Hope	|6|W|W
Creature - Avatar 4/9
If you have 3 or less life, ~ costs |6 less to cast.
Flying
~ can block any number of creatures. */

	// 415910
	if (event == EVENT_MODIFY_COST && life[player] <= 3){
		COST_COLORLESS -= 6;
	}

	creature_can_block_additional(player, card, event, 255);

	return 0;
}

int card_avatar_of_fury(int player, int card, event_t event){
/* CARD_ID_AVATAR_OF_FURY	4796
Avatar of Fury	|6|R|R
Creature - Avatar 6/6
If an opponent controls seven or more lands, ~ costs |6 less to cast.
Flying
|R: ~ gets +1/+0 until end of turn. */

	if( event == EVENT_MODIFY_COST ){
		if( count_subtype(1-player, TYPE_LAND, -1) > 6 ){
			COST_COLORLESS-=6;
		}
	}

	return generic_shade(player, card, event, 0, MANACOST_R(1), 1, 0, 0, 0);
}

int card_avatar_of_might(int player, int card, event_t event){
/* CARD_ID_AVATAR_OF_MIGHT	4797
Avatar of Might	|6|G|G
Creature - Avatar 8/8
If an opponent controls at least four more creatures than you, ~ costs |6 less to cast.
Trample (This creature can deal excess combat damage to defending player or planeswalker while attacking.) */

	if( event == EVENT_MODIFY_COST ){
		if( count_subtype(1-player, TYPE_CREATURE, -1)-count_subtype(player, TYPE_CREATURE, -1) > 3){
			COST_COLORLESS-=6;
		}
	}

	return 0;
}

int card_avatar_of_will(int player, int card, event_t event){
/* CARD_ID_AVATAR_OF_WILL	4798
Avatar of Will	|6|U|U
Creature - Avatar 5/6
If an opponent has no cards in hand, ~ costs |6 less to cast.
Flying */

	if( event == EVENT_MODIFY_COST ){
		if( hand_count[1-player] < 1 ){
			COST_COLORLESS-=6;
		}
	}

	return 0;
}

int card_avatar_of_wOE(int player, int card, event_t event){
/* CARD_ID_AVATAR_OF_WOE	4799
Avatar of Woe	|6|B|B
Creature - Avatar 6/5
If there are ten or more creature cards total in all graveyards, ~ costs |6 less to cast.
Fear (This creature can't be blocked except by artifact creatures and/or black creatures.)
|T: Destroy target creature. It can't be regenerated. */

	fear(player, card, event);

	if( event == EVENT_MODIFY_COST ){
		if( count_graveyard_by_type(1-player, TYPE_CREATURE)+count_graveyard_by_type(player, TYPE_CREATURE) > 9){
			COST_COLORLESS-=6;
		}
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST0, 0, &td, "TARGET_CREATURE");
}

int card_blessed_wind(int player, int card, event_t event){
/* CARD_ID_BLESSED_WIND	4801
Blessed Wind	|7|W|W
Sorcery
Target player's life total becomes 20. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;
	td.preferred_controller = ANYBODY;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
		if( spell_fizzled != 1 ){
			if( instance->targets[0].player == player ){
				ai_modifier += ((20-life[player]) * 8);
			}
			if( instance->targets[0].player == 1-player ){
				ai_modifier += ((life[1-player]-20) * 8);
			}
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			set_life_total(instance->targets[0].player, 20);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
}

int card_branded_brawlers(int player, int card, event_t event){
/* CARD_ID_BRANDED_BRAWLERS	4804
Branded Brawlers	|R
Creature - Human Soldier 2/2
~ can't attack if defending player controls an untapped land.
~ can't block if you control an untapped land. */

	if (event == EVENT_ATTACK_LEGALITY && affect_me(player, card) && ! is_humiliated(player, card) && control_an_untapped_land(1-player))
		event_result = 1;

	if (event == EVENT_BLOCK_LEGALITY && affect_me(player, card) && ! is_humiliated(player, card) && control_an_untapped_land(player))
		event_result = 1;

	return 0;
}

int card_calming_verses(int player, int card, event_t event){
/* CARD_ID_CALMING_VERSE	4806
Calming Verse	|3|G
Sorcery
Destroy all enchantments you don't control. Then, if you control an untapped land, destroy all enchantments you control. */

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_ENCHANTMENT);
		new_manipulate_all(player, card, 1-player, &this_test, KILL_DESTROY);
		if( control_an_untapped_land(player) ){
			new_manipulate_all(player, card, player, &this_test, KILL_DESTROY);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_chimeric_idol(int player, int card, event_t event){
/* CARD_ID_CHIMERIC_IDOL	4809
Chimeric Idol	|3
Artifact
|0: Tap all lands you control. ~ becomes a 3/3 Turtle artifact creature until end of turn. */

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_LAND);
		new_manipulate_all(player, card, player, &this_test, ACT_TAP);
		artifact_animation(instance->parent_controller, instance->parent_card, instance->parent_controller, instance->parent_card,
							1, 3, 3, 0, 0);
	}

	return generic_activated_ability(player, card, event, GAA_TYPE_CHANGE, MANACOST0, 0, NULL, NULL);
}

int card_citadel_of_pain(int player, int card, event_t event){
/* CARD_ID_CITADEL_OF_PAIN	4810
Citadel of Pain	|2|R
Enchantment
At the beginning of each player's end step, ~ deals X damage to that player, where X is the number of untapped lands he or she controls. */

	if( eot_trigger(player, card, event) ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_LAND);
		this_test.state = STATE_TAPPED;
		this_test.state_flag = 1;
		int amount = check_battlefield_for_special_card(player, card, current_turn, 4, &this_test);
		if( amount > 0 ){
			damage_player(current_turn, amount, player, card);
		}
	}

	return global_enchantment(player, card, event);
}

int card_coastal_hornclaw(int player, int card, event_t event){
	/*
	  Coastal Hornclaw |4|U
	  Creature - Bird 3/3
	  Sacrifice a land: Coastal Hornclaw gains flying until end of turn.
	*/

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_LAND, "Select a land to sacrifice.");

	card_instance_t *instance = get_card_instance(player, card);

	if (event == EVENT_RESOLVE_ACTIVATION ){
		pump_ability_until_eot(instance->parent_controller, instance->parent_card, instance->parent_controller, instance->parent_card,
								0, 0, KEYWORD_FLYING, 0);
	}

	return generic_activated_ability_with_sacrifice_as_cost(player, card, event, 0, MANACOST0, 0, NULL, NULL, 0, &test, 0);
}

int card_denying_wind(int player, int card, event_t event){
/* CARD_ID_DENYING_WIND	4815
Denying Wind	|7|U|U
Sorcery
Search target player's library for up to seven cards and exile them. Then that player shuffles his or her library. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			test_definition_t this_test;
			default_test_definition(&this_test, TYPE_ANY);
			this_test.no_shuffle = 1;

			int amount = MIN(7, count_deck(instance->targets[0].player));

			while( amount > 0 ){
					scnprintf(this_test.message, 100, "Select a card to exile (%d left)", amount);
					if( new_global_tutor(player, instance->targets[0].player, TUTOR_FROM_DECK, TUTOR_RFG, 0, AI_MAX_VALUE, &this_test) != -1 ){
						amount--;
					}
					else{
						break;
					}
			}
			shuffle(instance->targets[0].player);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
}

static int token_generated_by_me(int player, int card, int tok_player, int tok_card){
	if( ! is_token(tok_player, tok_card) ){
		return 0;
	}
	card_instance_t *instance = get_card_instance(tok_player, tok_card);
	if( instance->damage_source_player > -1 ){
		if( instance->damage_source_player == player && instance->damage_source_card == card ){
			return 1;
		}
	}
	return 0;
}

int card_dual_nature(int player, int card, event_t event){
/*
Dual Nature English |4|G|G
Whenever a nontoken creature enters the battlefield, its controller puts a token that's a copy of that creature onto the battlefield.
Whenever a nontoken creature leaves the battlefield, exile all tokens with the same name as that creature.
When Dual Nature leaves the battlefield, exile all tokens put onto the battlefield with Dual Nature.
*/
	if( ! is_humiliated(player, card) ){
		test_definition_t this_test1;
		default_test_definition(&this_test1, TYPE_CREATURE);
		this_test1.type_flag = F1_NO_TOKEN;

		etb_ability_global(ANYBODY, event, RESOLVE_TRIGGER_MANDATORY, 0, &this_test1, player, card);

		if( event == EVENT_ETB_ABILITY ){
			int id = get_id(trigger_cause_controller, trigger_cause);

			token_generation_t token;
			default_token_definition(player, card, id, &token);
			token.t_player = trigger_cause_controller;
			token.legacy = 1;
			token.special_code_for_legacy = &empty;
			generate_token(&token);
		}

		if( trigger_condition == TRIGGER_LEAVE_PLAY && affect_me( player, card ) &&
		   reason_for_trigger_controller == affected_card_controller )
		{
			int trig = 0;
			int id = get_card_name(trigger_cause_controller, trigger_cause);
			if( id != -1 ){ //If a face-down card leaves play, Double Nature will trigger but won't do nothing as it has no name
				if( is_what(trigger_cause_controller, trigger_cause, TYPE_CREATURE) && ! is_token(trigger_cause_controller, trigger_cause) ){
					trig = 1;
				}

				if( trig > 0 ){
					if(event == EVENT_TRIGGER){
						event_result |= RESOLVE_TRIGGER_MANDATORY;
					}
					else if(event == EVENT_RESOLVE_TRIGGER){
							test_definition_t this_test;
							default_test_definition(&this_test, TYPE_CREATURE);
							this_test.type_flag = F1_IS_TOKEN;
							this_test.id = id;
							APNAP(p, {new_manipulate_all(player, card, p, &this_test, KILL_EXILE);};);
					}
				}
			}
		}

		if( leaves_play(player, card, event) ){
			int i;
			for(i=0; i<2; i++){
				int p = i == 0 ? player : 1-player;
				int c;
				for(c=active_cards_count[p]-1; c > -1; c--){
					if( in_play(p, c) && is_what(p, c, TYPE_CREATURE) ){
						if( token_generated_by_me(player, card, p, c) ){
							kill_card(p, c, KILL_EXILE);
						}
					}
				}
			}
		}
	}

	return global_enchantment(player, card, event);
}

int card_entangler(int player, int card, event_t event)
{
  attached_creature_can_block_additional(player, card, event, 255);
  return vanilla_aura(player, card, event, player);
}

int card_foil(int player, int card, event_t event){

	if( event == EVENT_MODIFY_COST ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_LAND, "Select an Island card.");
		this_test.zone = TARGET_ZONE_HAND;
		this_test.subtype = SUBTYPE_ISLAND;	// can't hack in time
		if( hand_count[player] > 1 && check_battlefield_for_special_card(player, card, player, 0, &this_test) ){
			null_casting_cost(player, card);
			get_card_instance( player, card )->info_slot = 1;
		}
		else{
			get_card_instance( player, card )->info_slot = 0;
		}
	}

	if( event == EVENT_CAN_CAST ){
		return counterspell(player, card, event, NULL, 0);
	}

	if(event == EVENT_CAST_SPELL && affect_me(player, card) ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_LAND, "Select an Island card.");
		this_test.zone = TARGET_ZONE_HAND;
		this_test.subtype = SUBTYPE_ISLAND;	// can't hack in time
		if( ! played_for_free(player, card) && ! is_token(player, card) && get_card_instance( player, card )->info_slot == 1 ){
			int choice = 0;
			if( has_mana_to_cast_id(player, event, get_id(player, card)) ){
				choice = do_dialog(player, player, card, -1, -1, " Discard an Island + another card\n Play normally\n Cancel", 1);
			}

			if( choice == 0 ){
				int selected = new_select_a_card(player, player, TUTOR_FROM_HAND, 0, AI_MIN_VALUE, -1, &this_test);
				if( selected > -1 ){
					discard_card(player, selected);
					discard(player, 0, player);
				}
				else{
					spell_fizzled = 1;
				}
			}
			else if( choice == 1 ){
					charge_mana_from_id(player, card, event, get_id(player, card));
			}
			else{
				spell_fizzled = 1;
				return 0;
			}
		}
		if( spell_fizzled != 1 ){
			counterspell(player, card, event, NULL, 0);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		return counterspell(player, card, event, NULL, 0);
	}

	return 0;
}

int card_greel_mind_raker(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE && hand_count[player] > 1 ){
		return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_UNTAPPED, MANACOST_B(1), 0, &td, NULL);;
	}

	if(event == EVENT_ACTIVATE ){
		ai_modifier += (((hand_count[1-player]-hand_count[player])-2)*8);
		generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_UNTAPPED, MANACOST_XB(-1, 1), 0, &td, "TARGET_PLAYER");
		if( spell_fizzled != 1 ){
			multidiscard(player, 2, 0);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			new_multidiscard(instance->targets[0].player, instance->info_slot, DISC_RANDOM, player);
		}
	}

	return 0;
}

int card_gulf_squid(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, 0);
		td.zone = TARGET_ZONE_PLAYERS;
		td.allow_cancel = 0;

		card_instance_t *instance = get_card_instance(player, card);

		if( can_target(&td) && new_pick_target(&td, "TARGET_PLAYER", 0, 0) ){
			test_definition_t this_test;
			default_test_definition(&this_test, TYPE_LAND);
			new_manipulate_all(player, card, instance->targets[0].player, &this_test, ACT_TAP);
		}
		instance->number_of_targets = 0;
	}

	return 0;
}

int card_heightened_awareness(int player, int card, event_t event){

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (hand_count[player] < 2 ? 48 : -12);
	}

	if( current_turn == player && event == EVENT_DRAW_PHASE && ! is_humiliated(player, card) ){
		event_result++;
	}

	if( event == EVENT_RESOLVE_SPELL && ! is_humiliated(player, card) ){
		discard_all(player);
	}

	return basic_spell(player, card, event);
}

int card_lesser_gargadon(int player, int card, event_t event){
	/*
	  Lesser Gargadon |2|R|R
	  Creature - Beast 6/4
	  Whenever Lesser Gargadon attacks or blocks, sacrifice a land.
	*/
	if( ! is_humiliated(player, card) ){
		if( declare_attackers_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY, player, card) || blocking(player, card, event) ){
			test_definition_t test;
			new_default_test_definition(&test, TYPE_LAND, "Select a land to sacrifice.");
			new_sacrifice(player, card, player, SAC_CAUSED | SAC_NO_CANCEL, &test);
		}
	}

	return 0;
}

int card_infernal_genesis(int player, int card, event_t event){

	upkeep_trigger_ability(player, card, event, ANYBODY);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		int *deck = deck_ptr[current_turn];
		if( deck[0] != -1 ){
			int cmc = get_cmc_by_id(cards_data[deck[0]].id);
			mill(current_turn, 1);

			token_generation_t token;
			default_token_definition(player, card, CARD_ID_MINION, &token);
			token.t_player = current_turn;
			token.pow = token.tou = 1;
			token.qty = cmc;
			generate_token(&token);
		}
	}

	return global_enchantment(player, card, event);
}

int card_jolrael_empress_of_beasts(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE && hand_count[player] >= 2 ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_XG(2, 1), 0, &td, NULL);
	}

	if(event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_XG(2, 1), 0, &td, "TARGET_PLAYER");
		if( spell_fizzled != 1 ){
			multidiscard(player, 2, 0);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			test_definition_t this_test;
			default_test_definition(&this_test, TYPE_LAND);
			land_animation2(instance->parent_card, instance->parent_card, instance->targets[0].player, -1, 1, 3, 3, 0, 0, 0, &this_test);
		}
	}

	return 0;
}

int card_keldon_firebombers(int player, int card, event_t event){
/*
Keldon Firebombers |3|R|R
Creature — Human Soldier 3/3
When Keldon Firebombers enters the battlefield, each player sacrifices all lands he or she controls except for three.
*/
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		int lands_to_sacrifice[2] = {count_subtype(0, TYPE_LAND, -1)-3, count_subtype(1, TYPE_LAND, -1)-3};
		APNAP(p,{
					if( lands_to_sacrifice[p] ){
						test_definition_t test;
						default_test_definition(&test, TYPE_LAND);
						scnprintf(test.message, 100, "Select %d land%s to sacrifice.", lands_to_sacrifice[p],
									lands_to_sacrifice[p] > 1 ? "s" : "");
						new_sacrifice(player, card, p, SAC_CAUSED | SAC_NO_CANCEL, &test);
					}
				}
		);
	}

	return 0;
}

int card_latulla_keldon_overseer(int player, int card, event_t event){
	/* Latulla, Keldon Overseer	|3|R|R	0x20087f7
	 * Legendary Creature - Human Spellshaper 3/3
	 * |X|R, |T, Discard two cards: ~ deals X damage to any target. */
	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE && hand_count[player] >= 2 ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_XR(2, 1), 0, &td, NULL);
	}

	if(event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_XR(2, 1), 0, &td, "TARGET_ANY");
		if( spell_fizzled != 1 ){
			multidiscard(player, 2, 0);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			damage_target0(player, card, instance->info_slot);
		}
	}

	return 0;
}

int card_mageta_the_lion(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	if( event == EVENT_CAN_ACTIVATE && hand_count[player] >= 2 ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_XW(2, 2), 0, NULL, NULL);
	}

	if(event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_XW(2, 2), 0, NULL, NULL);
		if( spell_fizzled != 1 ){
			multidiscard(player, 2, 0);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);

		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.not_me = 1;
		APNAP(p, { new_manipulate_all(instance->parent_controller, instance->parent_card, p, &this_test, KILL_BURY); };);
	}

	return 0;
}

int card_mungha_wurm(int player, int card, event_t event){

	if( current_turn == player ){
		return card_winter_orb(player, card, event);
	}

	return 0;
}

int card_overburden(int player, int card, event_t event){

	if( trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card) ){
		test_definition_t test;
		default_test_definition(&test, TYPE_CREATURE);
		test.type_flag = F1_NO_TOKEN;

		etb_ability_global(player, event, RESOLVE_TRIGGER_MANDATORY, 0, &test, player, card);
	}

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_LAND);
		td.illegal_abilities = 0;
		td.allow_cancel = 0;
		td.allowed_controller = td.preferred_controller = trigger_cause_controller;
		td.who_chooses = trigger_cause_controller;

		card_instance_t *instance = get_card_instance(player, card);

		if( new_pick_target(&td, "Select a land you control to bounce.", 0, 0)  ){
			bounce_permanent(instance->targets[0].player, instance->targets[0].card);
		}
		instance->number_of_targets = 0;
	}

	return global_enchantment(player, card, event);
}

// pigmy razorback -> vanilla

int card_panic_attack(int player, int card, event_t event){
	/*
	  Panic Attack |2|R
	  Sorcery
	  Up to three target creatures can't block this turn.
	*/

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	if( event == EVENT_RESOLVE_SPELL ){
		card_instance_t *instance = get_card_instance(player, card);
		int i;
		for(i = 0; i < instance->number_of_targets; i++){
			if( validate_target(player, card, &td, i) ){
				pump_ability_until_eot(player, card, instance->targets[i].player, instance->targets[i].card, 0,0, 0,SP_KEYWORD_CANNOT_BLOCK);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_OPTIONAL_TARGET, &td, "TARGET_CREATURE", 3, NULL);
}

int card_plague_wind(int player, int card, event_t event){
	/*
	  Plague Wind |7|B|B
	  Sorcery
	  Destroy all creatures you don't control. They can't be regenerated.
	*/
	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		new_manipulate_all(player, card, 1-player, &this_test, KILL_BURY);
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
}

int card_rethink(int player, int card, event_t event){
	/* Rethink	|2|U
	 * Instant
	 * Counter target spell unless its controller pays |X, where X is its converted mana cost. */

	if( event == EVENT_RESOLVE_SPELL ){
		card_instance_t *instance = get_card_instance(player, card);
		counterspell_resolve_unless_pay_x(player, card, NULL, 0, get_cmc(instance->targets[0].player, instance->targets[0].card));
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_COUNTERSPELL, NULL, NULL, 1, NULL);
}

int card_reveille_squad(int player, int card, event_t event){
/*
Reveille Squad |2|W|W
Creature — Human Rebel 3/3
Whenever one or more creatures attack you, if Reveille Squad is untapped, you may untap all creatures you control.
*/
	if( ! is_tapped(player, card) ){
		int amt;
		if ((amt = declare_attackers_trigger(player, card, event, RESOLVE_TRIGGER_AI(player) | DAT_ATTACKS_PLAYER, 1-player, -1)))
		{
			test_definition_t test;
			default_test_definition(&test, TYPE_CREATURE);
			new_manipulate_all(player, card, player, &test, ACT_UNTAP);
		}
	}

	return 0;
}

int card_rhystic_circle(int player, int card, event_t event){
/* CARD_ID_RHYSTIC_CIRCLE	4872
Rhystic Circle	|2|W|W
Enchantment
|1: Any player may pay |1. If no one does, the next time a source of your choice would deal damage to you this turn, prevent that damage.*/

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);
	td1.extra = damage_card;
	td1.required_type = 0;
	td1.special = TARGET_SPECIAL_DAMAGE_PLAYER;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td1) ){
			int prevent = 1;
			if( has_mana(1-player, COLOR_ANY, 1) ){
				charge_mana(1-player, COLOR_ANY, 1);
				if( spell_fizzled != 1 ){
					prevent = 0;
				}
			}
			if( prevent ){
				prevent_damage(instance->targets[0].player, instance->targets[0].card, -1);
			}
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_DAMAGE_PREVENTION | GAA_LITERAL_PROMPT, MANACOST_X(1), 0,
									&td1, "Select target damage card attached to a player.");
}

int card_rhystic_deluge(int player, int card, event_t event){
/* CARD_ID_RHYSTIC_DELUGE	4873
Rhystic Deluge	|2|U
Enchantment
|U: Tap target creature unless its controller pays |1. */

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td1) ){
			int tap = 1;
			if( has_mana(instance->targets[0].player, COLOR_COLORLESS, 1) ){
				int ai_choice = instance->targets[0].player != player ? 0 : 1;
				int choice = do_dialog(instance->targets[0].player, player, card, -1, -1, " Pay 1\n Pass", ai_choice);
				if( ! choice ){
					charge_mana(instance->targets[0].player, COLOR_COLORLESS, 1);
					if( spell_fizzled != 1 ){
						tap = 0;
					}
				}
			}
			if( tap == 1 ){
				tap_card(instance->targets[0].player, instance->targets[0].card);
			}
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_U(1), 0, &td1, "TARGET_CREATURE");
}

int card_rhystic_study(int player, int card, event_t event){
/*CARD_ID_RHYSTIC_STUDY	4877
Rhystic Study	|2|U
Enchantment
Whenever an opponent casts a spell, you may draw a card unless that player pays |1. */

	if( new_specific_spell_played(player, card, event, 1-player, RESOLVE_TRIGGER_AI(player), NULL) ){
		charge_mana(1-player, COLOR_ANY, 1);
		if( spell_fizzled == 1 ){
			draw_cards(player, 1);
		}
	}

	return global_enchantment(player, card, event);
}

int card_rhystic_tutor(int player, int card, event_t event){
/* CARD_ID_RHYSTIC_TUTOR	4879
Rhystic Tutor	|2|B
Sorcery
Unless any player pays |2, search your library for a card, put that card into your hand, then shuffle your library. */

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier +=(has_mana(1-player, COLOR_COLORLESS, 2) ? -96 : 96);
	}

	if(event == EVENT_RESOLVE_SPELL ){
		charge_mana(1-player, COLOR_COLORLESS, 2);
		if( spell_fizzled == 1 ){
			test_definition_t test;
			new_default_test_definition(&test, TYPE_ANY, "Select a card.");
			new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 0, AI_MAX_VALUE, &test);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_searing_wind(int player, int card, event_t event){
	/* Searing Wind	|8|R	0x2008824
	 * Instant
	 * ~ deals 10 damage to any target. */
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_target0(player, card, 10);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ANY", 1, NULL);
}

static int shield_dancer_effect(int player, int card, event_t event){

	if( get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);

		int p = instance->damage_target_player;
		int c = instance->damage_target_card;

		if( event == EVENT_PREVENT_DAMAGE ){
			card_instance_t *source = get_card_instance(affected_card_controller, affected_card);
			if( damage_card == source->internal_card_id && source->info_slot > 0 ){
				if( source->damage_target_player == instance->targets[1].player &&
					source->damage_target_card == instance->targets[1].card &&
					source->damage_source_player == p &&
					source->damage_source_card == c )
				{
					source->damage_target_player = p;
					source->damage_target_card = c;
					kill_card(player, card, KILL_EXILE);
				}
			}
		}
	}

	if( event == EVENT_CLEANUP ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int card_shield_dancer(int player, int card, event_t event){
/*
Shield Dancer |2|W
Creature — Human Rebel 1/3
{2}{W}: The next time target attacking creature would deal combat damage to Shield Dancer this turn, that creature deals that damage
	to itself instead.
*/
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.required_state = TARGET_STATE_ATTACKING;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_XW(2, 1), 0, &td, NULL);
	}

	if(event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_XW(2, 1), 0, &td, NULL);
		if( spell_fizzled != 1 ){
			if( instance->targets[0].player == 1-player && instance->targets[0].card == instance->blocking ){
				ai_modifier += 48;
			}
			else{
				ai_modifier -= 48;
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			int legacy = create_targetted_legacy_effect(instance->parent_controller, instance->parent_card, &shield_dancer_effect,
														instance->targets[0].player, instance->targets[0].card);
			card_instance_t *trg = get_card_instance(player, legacy);
			trg->targets[1].player = instance->parent_controller;
			trg->targets[1].card = instance->parent_card;
			trg->number_of_targets = 2;
		}
	}

	return 0;
}

int card_snag(int player, int card, event_t event){

	if( event == EVENT_MODIFY_COST ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_LAND, "Select a Forest card.");
		this_test.zone = TARGET_ZONE_HAND;
		this_test.subtype = SUBTYPE_FOREST;	// can't hack in time

		if( check_battlefield_for_special_card(player, card, player, 0, &this_test) ){
			null_casting_cost(player, card);
			get_card_instance( player, card )->info_slot = 1;
		}
		else{
			get_card_instance( player, card )->info_slot = 0;
		}
	}

	if( event == EVENT_CAN_CAST ){
		return basic_spell(player, card, event);
	}

	if(event == EVENT_CAST_SPELL && affect_me(player, card) ){
		if( ! played_for_free(player, card) && ! is_token(player, card) && get_card_instance( player, card )->info_slot == 1 ){
			int choice = 0;
			if( has_mana_to_cast_id(player, event, get_id(player, card)) ){
				choice = do_dialog(player, player, card, -1, -1, " Discard a Forest\n Play normally\n Cancel", 1);
			}

			if( choice == 0 ){
				test_definition_t this_test;
				new_default_test_definition(&this_test, TYPE_LAND, "Select a Forest card.");
				this_test.zone = TARGET_ZONE_HAND;
				this_test.subtype = SUBTYPE_FOREST;	// can't hack in time

				int selected = new_select_a_card(player, player, TUTOR_FROM_HAND, 0, AI_MIN_VALUE, -1, &this_test);
				if( selected > -1 ){
					discard_card(player, selected);
				}
				else{
					spell_fizzled = 1;
				}
			}
			else if( choice == 1 ){
					charge_mana_from_id(player, card, event, get_id(player, card));
			}
			else{
				spell_fizzled = 1;
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		fog_special(player, card, ANYBODY, FOG_COMBAT_DAMAGE_ONLY | FOG_UNBLOCKED_CREATURES_ONLY);
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_spiketail_drake2(int player, int card, event_t event){
	/* Spiketail Drake	|3|U|U
	 * Creature - Drake 3/3
	 * Flying
	 * Sacrifice ~: Counter target spell unless its controller pays |3. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	counterspell_target_definition(player, card, &td, 0);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		counterspell_resolve_unless_pay_x(player, card, &td, 0, 3);
	}

	return generic_activated_ability(player, card, event, GAA_SACRIFICE_ME | GAA_SPELL_ON_STACK, MANACOST0, 0, &td, NULL);
}

int card_spiketail_hatchling(int player, int card, event_t event){
	/* Spiketail Hatchling	|1|U
	 * Creature - Drake 1/1
	 * Flying
	 * Sacrifice ~: Counter target spell unless its controller pays |1. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	counterspell_target_definition(player, card, &td, 0);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		counterspell_resolve_unless_pay_x(player, card, &td, 0, 1);
	}

	return generic_activated_ability(player, card, event, GAA_SACRIFICE_ME | GAA_SPELL_ON_STACK, MANACOST0, 0, &td, NULL);
}

int card_spitting_spider(int player, int card, event_t event){
	/*
	  Spitting Spider English |3|G|G
	  Creature - Spider 3/5
	  Reach (This creature can block creatures with flying.)
	  Sacrifice a land: Spitting Spider deals 1 damage to each creature with flying.
	*/

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_LAND, "Select a land to sacrifice.");

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		test_definition_t test2;
		default_test_definition(&test2, TYPE_CREATURE);
		test2.keyword = KEYWORD_FLYING;
		new_damage_all(instance->parent_controller, instance->parent_card, ANYBODY, 1, 0, &test2);
	}

	return generic_activated_ability_with_sacrifice_as_cost(player, card, event, 0, MANACOST0, 0, NULL, NULL, 0, &test, 0);
}

int card_spore_frog(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_ACTIVATION ){
		fog_effect(player, card);
	}

	return generic_activated_ability(player, card, event, GAA_SACRIFICE_ME, MANACOST0, 0, NULL, NULL);
}

int card_squirrel_wrangler(int player, int card, event_t event){
	/* Squirrel Wrangler	|2|G|G
	 * Creature - Human Druid 2/2
	 * |1|G, Sacrifice a land: Put two 1/1 |Sgreen Squirrel creature tokens onto the battlefield.
	 * |1|G, Sacrifice a land: Squirrel creatures get +1/+1 until end of turn. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_LAND, "Select a land to sacrifice.");

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability_with_sacrifice_as_cost(player, card, event, 0, MANACOST_XG(1, 1), 0, NULL, NULL, 0, &test, 0);
	}

	if(event == EVENT_ACTIVATE ){
		int choice = DIALOG(player, card, event, DLG_RANDOM,
							"Generate 2 Squirrels", 1, 5,
							"Pump your Squirrels", count_subtype(player, TYPE_CREATURE, SUBTYPE_SQUIRREL)*2, 10);
		if( ! choice ){
			spell_fizzled = 1;
			return 0;
		}
		return generic_activated_ability_with_sacrifice_as_cost(player, card, event, 0, MANACOST_XG(1, 1), 0, NULL, NULL, 0, &test, 0);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == 1 ){
			generate_tokens_by_id(instance->parent_controller, instance->parent_card, CARD_ID_SQUIRREL, 2);
		}
		if( instance->info_slot == 2 ){
			pump_subtype_until_eot(instance->parent_controller, instance->parent_card, 2, SUBTYPE_SQUIRREL, 1, 1, 0, 0);
		}
	}

	return 0;
}

int card_sword_dancer(int player, int card, event_t event){
	/*
	  Sword Dancer |1|W
	  Creature - Human Rebel 1/2
	  {W}{W}: Target attacking creature gets -1/-0 until end of turn.
	*/
	if (!IS_GAA_EVENT(event)  ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.required_state = TARGET_STATE_ATTACKING;

	if (event == EVENT_RESOLVE_ACTIVATION && valid_target(&td) ){
		card_instance_t *instance = get_card_instance(player, card);
		pump_until_eot(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card, -1, 0);
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST_W(2), 0,
									&td, "Select target attacking creature.");
}

int card_thrive(int player, int card, event_t event){
/*
Thrive |X|G
Sorcery
Put a +1/+1 counter on each of X target creatures.
*/
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;
	td.allow_cancel = 2; //"Done" button

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST && can_target(&td) ){
		return generic_spell(player, card, event, GS_X_SPELL, NULL, NULL, 0, NULL);
	}

	if(event == EVENT_CAST_SPELL && affect_me(player, card) ){
		if( (played_for_free(player, card) || is_token(player, card)) && ! check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
			return 0;
		}
		int diff = true_get_updated_casting_cost(player, card, -1, event, -1);
		int max = has_mana(player, COLOR_ANY, 1) - diff;
		if( check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
			max = instance->info_slot;
		}
		int trgs = 0;
		while( can_target(&td) && trgs < max ){
				if( new_pick_target(&td, "TARGET_CREATURE", trgs, 0) ){
					state_untargettable(instance->targets[trgs].player, instance->targets[trgs].card, 1);
					trgs++;
				}
				else{
					break;
				}
		}
		int i;
		for(i=0; i<trgs; i++){
			state_untargettable(instance->targets[i].player, instance->targets[i].card, 0);
		}
		if( ! played_for_free(player, card) && ! is_token(player, card) ){
			charge_mana(player, COLOR_COLORLESS, trgs + diff);
			if( spell_fizzled != 1 ){
				instance->info_slot = trgs;
			}
		}
	}

	if(event == EVENT_RESOLVE_SPELL ){
		int i;
		for(i=0; i<instance->info_slot; i++){
			if( validate_target(player, card, &td, i) ){
				add_1_1_counter(instance->targets[i].player, instance->targets[i].card);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_troublesome_spirit(int player, int card, event_t event){

	if( current_turn == player && eot_trigger(player, card, event) ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_LAND);
		new_manipulate_all(player, card, player, &this_test, ACT_TAP);
	}

	return 0;
}

// veteran brawlers --> branded brawlers

int card_vitalizing_wind(int player, int card, event_t event){

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (count_subtype(player, TYPE_CREATURE, -1) * 16);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		pump_subtype_until_eot(player, card, player, -1, 7, 7, 0, 0);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_well_of_discovery(int player, int card, event_t event){

	if(current_turn == player && trigger_condition == TRIGGER_EOT &&  affect_me(player, card ) && reason_for_trigger_controller == player){
		if( ! control_an_untapped_land(player) && ! is_humiliated(player, card) ){
			if(event == EVENT_TRIGGER){
				event_result |= 2;
			}
			else if(event == EVENT_RESOLVE_TRIGGER){
					draw_cards(player, 1);
			}
		}
	}
	return 0;
}

int card_well_of_life(int player, int card, event_t event){

	if( current_turn == player && trigger_condition == TRIGGER_EOT &&  affect_me(player, card ) && reason_for_trigger_controller == player){
		if( ! control_an_untapped_land(player) && ! is_humiliated(player, card) ){
			if(event == EVENT_TRIGGER){
				event_result |= 2;
			}
			else if(event == EVENT_RESOLVE_TRIGGER){
					gain_life(player, 2);
			}
		}
	}

	return 0;
}

int card_whip_sergeant(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_R(1), 0, &td, "TARGET_CREATURE");
		if( spell_fizzled != 1 ){
			ai_modifier_for_haste_granting_effects(player, card);
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			pump_ability_until_eot(instance->parent_controller, instance->parent_card,
									instance->targets[0].player,  instance->targets[0].card, 0, 0, 0, SP_KEYWORD_HASTE);
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_R(1), 0, &td, "TARGET_CREATURE");
}

int card_wild_might(int player, int card, event_t event){

	if(event == EVENT_RESOLVE_SPELL ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.preferred_controller = player;

		card_instance_t *instance = get_card_instance(player, card);

		if( valid_target(&td) ){
			pump_until_eot(player, card, instance->targets[0].player,  instance->targets[0].card, 1, 1);
			charge_mana(1-instance->targets[0].player, COLOR_COLORLESS, 2);
			if( spell_fizzled == 1 ){
				pump_until_eot_merge_previous(player, card, instance->targets[0].player,  instance->targets[0].card, 4, 4);
			}
		}
		kill_card(player, card, KILL_DESTROY);
		return 0;
	}
	int pump = ! has_mana(1-player, COLOR_ANY, 1) ? 5 : 1;
	return vanilla_instant_pump(player, card, event, ANYBODY, player, pump, pump, 0, 0);
}

int card_withdraw(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if(event == EVENT_RESOLVE_SPELL ){
		if( validate_target(player, card, &td, 0) ){
			bounce_permanent(instance->targets[0].player,  instance->targets[0].card);
		}
		if( validate_target(player, card, &td, 1) ){
			charge_mana(instance->targets[1].player, COLOR_ANY, 1);
			if( spell_fizzled == 1 ){
				bounce_permanent(instance->targets[1].player,  instance->targets[1].card);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 2, NULL);
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int was_dealt_damage_this_turn(int unused, int unused2, int player, int card);
static int mirror_strike_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x2600670; original cards/prophecy.c:1709. */
int card_agent_of_shauku(int player, int card, event_t event)
{
  card_instance_t *instance;
  unsigned int uVar2;
  int result;
  unsigned int player_00;
  target_definition_t td;
  test_definition_t test;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_sacrifice_type_as_cost(player, 1, TYPE_LAND);
      if (result == 0)
      {
        return 0;
      }
      result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 1, 1, 0, 0, 0, 0, 0,
                                         &td, NULL);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      new_default_test_definition(&test, 1, "Select a land to sacrifice.");
      uVar2 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                            &test);
      if (uVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 1, 1, 0, 0, 0, 0, 0, &td,
                                "TARGET_CREATURE");
      player_00 = (int)uVar2 >> 0x10 & 0xff;
      if (spell_fizzled != 1)
      {
        kill_card(player_00, uVar2 >> 0x18, KILL_SACRIFICE);
        if (current_phase != 0x17)
        {
          return 0;
        }
        ai_modifier = ai_modifier + 0x18;
        return 0;
      }
      state_untargettable(player_00, uVar2 >> 0x18, 0);
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
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
  {
    pump_ability_until_eot(instance->parent_controller, instance->parent_card,
                           instance->targets[0].player, instance->targets[0].card, 2, 0, 0, 0);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2600ba0; original cards/prophecy.c:1803. */
int card_bog_elemental(int player, int card, event_t event)
{
  int result;
  test_definition_t test2;
  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    new_default_test_definition(&test2, 1, "Select a land to sacrifice.");
    result = new_sacrifice(player, card, player, 0, &test2);
    if (result == 0)
    {
      kill_card(player, card, KILL_SACRIFICE);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2600f60; original cards/prophecy.c:1897. */
int card_chilling_apparition(int player, int card, event_t event)
{
  int result;
  whenever_i_deal_damage_to_a_player_he_discards_a_card(player, card, event,
                                                        DDBM_MUST_BE_COMBAT_DAMAGE, 0);
  result = regeneration(player, card, event, 0, 1, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x2601500; original cards/prophecy.c:2033. */
int card_despoil(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 1);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = validate_target(player, card, &td, 0);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            lose_life(instance->targets[0].player, 2);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02601566;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 1);
  get_card_instance(player, card);
LAB_02601566:
  result = generic_spell(player, card, event, 1, &td, "TARGET_LAND", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2601670; original cards/prophecy.c:2058. */
int card_devastate(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 1);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = validate_target(player, card, &td, 0);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            new_damage_all(player, card, 2, 1, 3, NULL);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_026016d6;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 1);
  get_card_instance(player, card);
LAB_026016d6:
  result = generic_spell(player, card, event, 1, &td, "TARGET_LAND", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2601880; original cards/prophecy.c:2102. */
int card_elephant_resurgence(int player, int card, event_t event)
{
  unsigned int uVar1;
  int result;
  token_generation_t token;
  result = current_turn;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_token_definition(player, card, 0x4929, &token);
    token.t_player = result;
    token.special_infos = 0x42;
    token.tou = 0x4000;
    token.pow = 0x4000;
    generate_token(&token);
    uVar1 = current_turn ^ 1;
    default_token_definition(player, card, 0x4929, &token);
    token.special_infos = 0x42;
    token.tou = 0x4000;
    token.pow = 0x4000;
    token.t_player = uVar1;
    generate_token(&token);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25fb150; original cards/prophecy.c:2975. */
static int was_dealt_damage_this_turn(int unused, int unused2, int player, int card)
{
  int result;
  card_instance_t *instance;
  result = is_what(player, card, 2);
  if (result == 0)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  return (unsigned int)(instance->damage_on_card != 0);
}

/* ManalinkEh 2022: 0x2603a90; original cards/prophecy.c:2982. */
int card_inflame(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&test, 2, "");
    test.special_selection_function = was_dealt_damage_this_turn;
    new_damage_all(player, card, 2, 2, 0, &test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2604a00; original cards/prophecy.c:3274. */
int card_mine_bearer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.required_state = 2;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.required_state = 2;
    instance = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
  }
  result = generic_activated_ability(player, card, event, 0xf, 0, 0, 0, 0, 0, 0, 0, &td,
                                     "Select target attacking creature to destroy.");
  return result;
}

/* ManalinkEh 2022: 0x25facb0; original cards/prophecy.c:3300. */
static int mirror_strike_effect(int player, int card, event_t event)
{
  char cVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  instance = get_card_instance(player, card);
  if (-1 < instance->damage_target_player)
  {
    instance = get_card_instance(player, card);
    cVar1 = instance->damage_target_player;
    iVar4 = instance->damage_target_card;
    iVar2 = instance->targets[1].player;
    instance = damage_being_dealt(event);
    if ((((instance != NULL) && (cVar1 == instance->damage_source_player)) &&
         (instance->damage_source_card == iVar4)) &&
        ((instance->damage_target_player == iVar2 && (instance->damage_target_card == -1))))
    {
      iVar4 = damage_is_to_planeswalker(instance);
      if ((iVar4 == 0) && ((instance->token_status & 0x140000) != 0))
      {
        instance->damage_target_player = cVar1;
        instance->damage_target_card = -1;
      }
    }
  }
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x2604b50; original cards/prophecy.c:3328. */
int card_mirror_strike(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.required_state = 2;
    td.illegal_state = 8;
    get_card_instance(player, card);
    if ((current_phase != 0x18) && (result = can_target(&td), result != 0))
    {
      return 0;
    }
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 2);
        td.required_state = 2;
        td.illegal_state = 8;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            result = create_targetted_legacy_effect(player, card, mirror_strike_effect,
                                                    instance->targets[0].player,
                                                    instance->targets[0].card);
            instance = get_card_instance(player, result);
            instance->targets[1].player = player;
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02604c32;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td, 2);
    td.required_state = 2;
    td.illegal_state = 8;
    get_card_instance(player, card);
  }
LAB_02604c32:
  result =
      generic_spell(player, card, event, 0x81, &td, "Select target unblocked creature.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2605450; original cards/prophecy.c:3501. */
int card_pit_raptor(int player, int card, event_t event)
{
  basic_upkeep(player, card, event, 2, 2, 0, 0, 0, 0);
  return 0;
}

/* ManalinkEh 2022: 0x2606c50; original cards/prophecy.c:4173. */
int card_silt_crawler(int player, int card, event_t event)
{
  test_definition_t this_test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_test_definition(&this_test, 1);
  new_manipulate_all(player, card, player, &this_test, ACT_TAP);
  return 0;
}

/* ManalinkEh 2022: 0x2607920; original cards/prophecy.c:4488. */
int card_trenching_steed(int player, int card, event_t event)
{
  int result;
  result = generic_husk(player, card, event, 1, 0, 3, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x26084e0; original cards/prophecy.c:4842. */
int card_whipstitched_zombie(int player, int card, event_t event)
{
  basic_upkeep(player, card, event, 0, 1, 0, 0, 0, 0);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2600c40; original cards/prophecy.c:1827. */
int card_bog_glider(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&test, 0x47, "Select a Mercenary card");
    test.subtype = 0x73;
    test.cmc = 3;
    test.cmc_flag = 3;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 0x47, "Select a Mercenary card");
    test.subtype = 0x73;
    test.cmc = 3;
    test.cmc_flag = 3;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      new_global_tutor(player, player, 1, 4, 0, 5, &test);
    }
  }
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_LAND, NULL, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x2600fe0; original cards/prophecy.c:1908. */
int card_coffin_puppets(int player, int card, event_t event)
{
  int result;
  test_definition_t test_food;

  new_default_test_definition(&test_food, 1, "Select a land to sacrifice");
  test_food.qty = 2;
  if (event == EVENT_GRAVEYARD_ABILITY)
  {
    result = check_battlefield_for_subtype(player, 1, 0x5009);
    if ((result != 0) &&
        (result = generic_activated_ability_with_sacrifice_as_cost(
             player, card, EVENT_CAN_ACTIVATE, GAA_IN_YOUR_TURN | GAA_ONLY_ON_UPKEEP, 0, 0, 0, 0, 0,
             0, 0, NULL, NULL, TYPE_NONE, &test_food, SARV_0),
         result != 0))
    {
      return 8;
    }
  }
  else if (event == EVENT_PAY_FLASHBACK_COSTS)
  {
    generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_ACTIVATE, GAA_IN_YOUR_TURN,
                                                     0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
                                                     &test_food, SARV_0);
    return (unsigned int)(spell_fizzled != 1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2603f30; original cards/prophecy.c:3065. */
int card_keldon_arsonist(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  test_definition_t test;

  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&test, 1, "Select a land card to sacrifice.");
    test.qty = 2;
    default_target_definition(player, card, &td, 1);
    td.preferred_controller = player;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 1, "Select a land card to sacrifice.");
    test.qty = 2;
    default_target_definition(player, card, &td, 1);
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
  }
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, 1, 0, 0, 0, 0, 0, 0, &td,
      "Select target land to destroy.", TYPE_NONE, &test, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x2607110; original cards/prophecy.c:4292. */
int card_stormwatch_eagle(int player, int card, event_t event)
{
  card_instance_t *instance;
  generic_activated_ability_flags_t flags;
  int result;
  test_definition_t test;

  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&test, 1, "Select a land to sacrifice.");
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 1, "Select a land to sacrifice.");
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      bounce_permanent(instance->parent_controller, instance->parent_card);
    }
  }
  if (ai_is_speculating == 1)
  {
    flags = GAA_ONCE_PER_TURN;
  }
  else
  {
    flags = GAA_NONE;
    if (player == 1)
    {
      flags = (unsigned int)(((unsigned char)trace_mode & 2) == 0) << 0xd;
    }
  }
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, flags, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x2607970; original cards/prophecy.c:4515. */
int card_troubled_healer(int player, int card, event_t event)
{
  card_instance_t *instance;
  generic_activated_ability_flags_t flags;
  int result;
  target_definition_t td;
  test_definition_t test;

  if (event == EVENT_ACTIVATE)
  {
    new_default_test_definition(&test, 1, "Select a land to sacrifice.");
    default_target_definition(player, card, &td, 0);
    td.extra = damage_card;
    get_card_instance(player, card);
    if (ai_is_speculating != 1)
      goto LAB_02607a26;
  LAB_02607b1e:
    result = count_subtype(player, 1, -1);
    if (result < 5)
    {
      flags = GAA_DAMAGE_PREVENTION | GAA_ONCE_PER_TURN | GAA_CAN_TARGET;
      goto LAB_02607a34;
    }
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 1, "Select a land to sacrifice.");
    default_target_definition(player, card, &td, 0);
    td.extra = damage_card;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      prevent_damage(instance->targets[0].player, instance->targets[0].card, 2);
    }
    if (ai_is_speculating == 1)
      goto LAB_02607b1e;
  LAB_02607a26:
    if ((player == 1) && (((unsigned char)trace_mode & 2) == 0))
      goto LAB_02607b1e;
  }
  flags = GAA_DAMAGE_PREVENTION | GAA_CAN_TARGET;
LAB_02607a34:
  result = generic_activated_ability_with_sacrifice_as_cost(player, card, event, flags, 0, 0, 0, 0,
                                                            0, 0, 0, &td, "TARGET_DAMAGE",
                                                            TYPE_NONE, &test, SARV_0);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x26005c0; original cards/prophecy.c:1692. */
int card_living_terrain(int player, int card, event_t event)
{
  int result;
  target_definition_t td;

  default_target_definition(player, card, &td, 1);
  td.preferred_controller = player;
  attached_is_additional_subtype(player, card, event, SUBTYPE_TREEFOLK);
  attached_is_additional_color(player, card, event, COLOR_TEST_GREEN);
  result = generic_animating_aura(player, card, event, &td, "TARGET_LAND", 5, 6, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x26017f0; original cards/prophecy.c:2083. */
int card_elephant_token(int player, int card, event_t event)
{
  int result;

  if (card != -1)
  {
    if (((event - EVENT_POWER < 2) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      result = get_special_infos(player, card);
      if (result == 0x42)
      {
        result = count_graveyard_by_type(player, 2);
        set_characteristic_pt(result);
      }
    }
    result = generic_token(player, card, event);
    return result;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25f9e40; original cards/prophecy.c:2124. */
static int endbringers_revel_legacy(int player, int card, event_t event)
{
  int granting_card;
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  test_definition_t *ptStack_ec;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar2 = instance->targets[0].player;
      if (-1 < iVar2)
      {
        iVar2 = get_id(iVar2, instance->targets[0].card);
        if (iVar2 != 0x12d4)
        {
          kill_card(player, card, KILL_EXILE);
        }
        instance = in_play(instance->targets[0].player, instance->targets[0].card);
        if (instance == NULL)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      get_card_instance(player, card);
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      instance = get_card_instance(player, card);
      iVar2 = instance->targets[0].player;
      granting_card = instance->targets[0].card;
      if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      {
        new_default_test_definition(&test, 2, "Select target creature card.");
        if (event == EVENT_CAN_ACTIVATE)
        {
          iVar3 = any_in_graveyard_by_type(2, 2);
          if (iVar3 == 0)
          {
            return 0;
          }
          iVar3 = graveyard_has_shroud(2);
          if (iVar3 != 0)
          {
            return 0;
          }
          iVar2 = granted_generic_activated_ability(iVar2, granting_card, player, card,
                                                    EVENT_CAN_ACTIVATE, 0x10, 4, 0, 0, 0, 0, 0, 0,
                                                    NULL, NULL);
          if (iVar2 == 0)
          {
            return 0;
          }
          return 1;
        }
      }
      else
      {
        if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        {
          return 0;
        }
        new_default_test_definition(&test, 2, "Select target creature card.");
      }
      ptStack_ec = &test;
      if (event == EVENT_ACTIVATE)
      {
        granted_generic_activated_ability(iVar2, granting_card, player, card, EVENT_ACTIVATE, 0, 4,
                                          0, 0, 0, 0, 0, 0, NULL, NULL);
        if (spell_fizzled != 1)
        {
          select_target_from_either_grave(player, card, SFG_0, 2, 2, ptStack_ec, 1, 2);
        }
      }
      else
      {

        if ((event == EVENT_RESOLVE_ACTIVATION) &&
            (iVar2 =
                 validate_target_from_grave_source(player, card, instance->targets[1].player, 2),
             iVar2 != -1))
        {
          from_grave_to_hand(instance->targets[1].player, iVar2, 1);
        }
      }
    }
  }

  return 0;
}

/* ManalinkEh 2022: 0x25fa1c0; original cards/prophecy.c:2232. */
static int excavation_legacy(int player, int card, event_t event)
{
  int granting_card;
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  unsigned int player_00;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar3 = instance->targets[0].player;
      if (-1 < iVar3)
      {
        iVar3 = get_id(iVar3, instance->targets[0].card);
        if (iVar3 != 0x12d6)
        {
          kill_card(player, card, KILL_EXILE);
        }
        instance = in_play(instance->targets[0].player, instance->targets[0].card);
        if (instance == NULL)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      get_card_instance(player, card);
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {

      instance = get_card_instance(player, card);
      iVar3 = instance->targets[0].player;
      granting_card = instance->targets[0].card;
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar2 = can_sacrifice_type_as_cost(player, 1, TYPE_LAND);
        if ((iVar2 != 0) && (iVar3 = granted_generic_activated_ability(
                                 iVar3, granting_card, player, card, EVENT_CAN_ACTIVATE, 0, 1, 0, 0,
                                 0, 0, 0, 0, NULL, NULL),
                             iVar3 != 0))
        {
          return 1;
        }
      }
      else if (event == EVENT_ACTIVATE)
      {

        new_default_test_definition(&test, 1, "Select a land to sacrifice.");
        uVar4 = new_sacrifice(iVar3, granting_card, player,
                              SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
        if (uVar4 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        granted_generic_activated_ability(iVar3, granting_card, player, card, EVENT_ACTIVATE, 0, 1,
                                          0, 0, 0, 0, 0, 0, NULL, NULL);
        player_00 = (int)uVar4 >> 0x10 & 0xff;
        if (spell_fizzled == 1)
        {
          state_untargettable(player_00, uVar4 >> 0x18, 0);
        }
        else
        {
          kill_card(player_00, uVar4 >> 0x18, KILL_SACRIFICE);
        }
      }
      else if (event == EVENT_RESOLVE_ACTIVATION)
      {
        draw_a_card(player);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2602350; original cards/prophecy.c:2422. */
int card_fen_stalker(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = control_an_untapped_land(player);
    if (result == 0)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        fear(player, card, EVENT_ABILITIES);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x26023c0; original cards/prophecy.c:2434. */
int card_fickle_efreet(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (((event == EVENT_DECLARE_ATTACKERS) && (result = is_attacking(player, card), result != 0)) ||
      ((result = blocking(player, card, event),
        result != 0 && (result = is_humiliated_for_ability(player, card, AL_TRIG), result == 0))))
  {
    instance = get_card_instance(player, card);
    instance->targets[1].card = 0x42;
  }
  instance = get_card_instance(player, card);
  if ((instance->targets[1].card == 0x42) &&
      (result = end_of_combat_trigger(player, card, event, 2), result != 0))
  {
    result = flip_a_coin(player, card);
    if (result == 0)
    {
      instance = get_card_instance(player, card);
      instance->targets[1].card = 0;
      give_control_of_self(player, card);
    }
    instance = get_card_instance(player, card);
    instance->targets[1].card = 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x26036d0; original cards/prophecy.c:2890. */
int card_hazy_homunculus(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  if (event == EVENT_BLOCK_LEGALITY)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (((result == 0) && (attacking_card_controller == player)) && (attacking_card == card))
    {
      default_test_definition(&test, 1);
      test.state = 0x10;
      test.state_flag = 1;
      result = check_battlefield_for_special_card(player, card, 1 - current_turn, 0, &test);
      if (result != 0)
      {
        event_result = 1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2603790; original cards/prophecy.c:2911. */
int card_hollow_warrior(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  if (trigger_condition == 0xdc)
  {
    if (current_phase != 0x15)
    {
      return 0;
    }
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (((((card == affected_card) && (player == affected_card_controller)) &&
          (reason_for_trigger_controller == player)) &&
         ((result == 0 && forbid_attack == 0 && (trigger_cause_controller == player)))) &&
        (trigger_cause == card))
    {
      if (event == EVENT_TRIGGER)
      {
        event_result = event_result | 2;
      }
      else if (event == EVENT_RESOLVE_TRIGGER)
      {
        base_target_definition(player, card, &td, 2);
        td.allowed_controller = player;
        td.preferred_controller = player;
        td.illegal_state = 0x3f;
        td.special = 0x4000;
        result = can_target(&td);
        if ((result == 0) ||
            (result = new_pick_target(
                 &td, "Select an untapped creature you control not declared as an attacker.", 0,
                 0x80),
             result == 0))
        {
          forbid_attack = 1;
        }
        else
        {

          instance = get_card_instance(player, card);
          tap_card(instance->targets[0].player, instance->targets[0].card);
          instance->number_of_targets = '\0';
          if (forbid_attack == 0)
            goto LAB_026037a7;
        }
        remove_state(trigger_cause_controller, trigger_cause, 0x8000);
      }
    }
  }
LAB_026037a7:
  if (((((trigger_condition == 0xdd) && (current_phase == 0x17)) && (card == affected_card)) &&
       (((player == affected_card_controller &&
          (1 - current_turn == reason_for_trigger_controller)) &&
         ((trigger_cause_controller == player &&
           ((trigger_cause == card && (forbid_attack == 0)))))))) &&
      (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result = event_result | 2;
    }
    else if (event == EVENT_RESOLVE_TRIGGER)
    {
      base_target_definition(player, card, &td, 2);
      td.allowed_controller = player;
      td.preferred_controller = player;
      td.illegal_state = 0x3f;
      td.special = 0x4000;
      result = can_target(&td);
      if ((result == 0) ||
          (result = new_pick_target(
               &td, "Select an untapped creature you control not declared as a blocker.", 0, 0x80),
           result == 0))
      {
        forbid_attack = 1;
      }
      else
      {

        instance = get_card_instance(player, card);
        tap_card(instance->targets[0].player, instance->targets[0].card);
        instance->number_of_targets = '\0';
      }
    }
    if (forbid_attack != 0)
    {
      remove_state(player, card, 0x8008);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2604420; original cards/prophecy.c:3161. */
int card_keldon_berserker(int player, int card, event_t event)
{
  int result;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    result = control_an_untapped_land(player);
    if (result == 0)
    {
      pump_until_eot(player, card, player, card, 3, 0);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25f8e30; original cards/prophecy.c:3561. */
static int quicksilver_wall_legacy(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar3 = instance->targets[0].player;
      if (-1 < iVar3)
      {
        iVar3 = get_id(iVar3, instance->targets[0].card);
        if (iVar3 != 0x1303)
        {
          kill_card(player, card, KILL_EXILE);
        }
        instance = in_play(instance->targets[0].player, instance->targets[0].card);
        if (instance == NULL)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      get_card_instance(player, card);
    }
    return 0;
  }
  instance = get_card_instance(player, card);
  if (instance->targets[0].player == -1)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  iVar3 = instance->targets[0].player;
  card_00 = instance->targets[0].card;
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_025f8ebe:
    if (event == EVENT_RESOLVE_ACTIVATION)
      goto LAB_025f8e8c;
  }
  else
  {
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_025f8ebe;
    }
  LAB_025f8e8c:
    instance = in_play(iVar3, card_00);
    if (instance != NULL)
    {
      bounce_permanent(iVar3, card_00);
    }
  }
  iVar2 = 1;
  if (ai_is_speculating != 1)
  {
    iVar2 = 0x2000;
    if ((trace_mode & 2U) == 0 && player == 0)
      goto LAB_025f8ef3;
    iVar2 = 0;
  }
  iVar2 = (unsigned int)(1 - player == iVar2) << 0xd;
LAB_025f8ef3:
  iVar3 = granted_generic_activated_ability(iVar3, card_00, player, card, event, iVar2, 4, 0, 0, 0,
                                            0, 0, 0, NULL, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x25f9030; original cards/prophecy.c:3888. */
static int ribbon_snake_legacy(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar3 = instance->targets[0].player;
      if (-1 < iVar3)
      {
        iVar3 = get_id(iVar3, instance->targets[0].card);
        if (iVar3 != 0x1311)
        {
          kill_card(player, card, KILL_EXILE);
        }
        instance = in_play(instance->targets[0].player, instance->targets[0].card);
        if (instance == NULL)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      get_card_instance(player, card);
    }
    return 0;
  }
  instance = get_card_instance(player, card);
  if (instance->targets[0].player == -1)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  iVar3 = instance->targets[0].player;
  card_00 = instance->targets[0].card;
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_025f929a:
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
    LAB_025f9098:

      iVar2 = pump_ability_until_eot(iVar3, card_00, iVar3, card_00, 0, 0, 0x20, 0);
      if (iVar2 != -1)
      {
        instance = get_card_instance(iVar3, iVar2);
        instance->targets[4].player = 0;
      }
    }
  }
  else
  {
    if (event != EVENT_ACTIVATE)
    {
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        if (event != EVENT_CLEANUP)
        {
          return 0;
        }
        goto LAB_025f929a;
      }
      goto LAB_025f9098;
    }
    instance = get_card_instance(iVar3, card_00);
    ai_modifier =
        ai_modifier + (-(unsigned int)((instance->regen_status & 0x20) == 0) & 0xffffffa0) + 0x30;
  }
  iVar2 = 1;
  if (ai_is_speculating != 1)
  {
    iVar2 = 0x2000;
    if ((trace_mode & 2U) == 0 && player == 0)
      goto LAB_025f9198;
    iVar2 = 0;
  }
  iVar2 = (unsigned int)(1 - player == iVar2) << 0xd;
LAB_025f9198:
  iVar3 = granted_generic_activated_ability(iVar3, card_00, player, card, event, iVar2, 2, 0, 0, 0,
                                            0, 0, 0, NULL, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x2606670; original cards/prophecy.c:3984. */
int card_root_cage(int player, int card, event_t event)
{
  int result;

  if ((current_phase == 1) && (event == EVENT_UNTAP))
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      result = is_what(affected_card_controller, affected_card, 2);
      if (result != 0)
      {
        result = has_subtype(affected_card_controller, affected_card, SUBTYPE_MERCENARY);
        if (result != 0)
        {
          does_not_untap(affected_card_controller, affected_card, EVENT_UNTAP);
        }
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x25f92c0; original cards/prophecy.c:3999. */
static int samite_sanctuary_legacy(int player, int card, event_t event)
{
  int player_00;
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar2 = instance->targets[0].player;
      if (-1 < iVar2)
      {
        iVar2 = get_id(iVar2, instance->targets[0].card);
        if (iVar2 != 0x1313)
        {
          kill_card(player, card, KILL_EXILE);
        }
        instance = in_play(instance->targets[0].player, instance->targets[0].card);
        if (instance == NULL)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      get_card_instance(player, card);
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      instance = get_card_instance(player, card);
      iVar2 = instance->targets[0].card;
      player_00 = instance->targets[0].player;
      if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      {
        default_target_definition(player_00, iVar2, &td, 0xff);
        td.extra = damage_card;
        td.special = 0x100000;
        if (event == EVENT_CAN_ACTIVATE)
        {
          iVar2 = generic_activated_ability(player_00, iVar2, EVENT_CAN_ACTIVATE, 0x4000004, 2, 0,
                                            0, 0, 0, 0, 0, &td, "TARGET_DAMAGE");
          if (iVar2 == 0)
          {
            return 0;
          }
          return 99;
        }
      }
      else
      {
        if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        {
          return 0;
        }
        default_target_definition(player_00, iVar2, &td, 0xff);
      }
      td.extra = damage_card;
      td.special = 0x100000;
      if (event == EVENT_ACTIVATE)
      {
        instance->number_of_targets = '\x01';
        iVar3 = new_pick_target(&td, "TARGET_DAMAGE", 0, 1);
        if (iVar3 != 0)
        {
          iVar2 = granted_generic_activated_ability(player_00, iVar2, player, card, EVENT_ACTIVATE,
                                                    0, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
          return iVar2;
        }
      }
      else if (event == EVENT_RESOLVE_ACTIVATION)
      {

        instance = get_card_instance(player_00, iVar2);
        iVar2 = validate_target(player, card, &td, 1);
        if (iVar2 != 0)
        {
          prevent_damage(instance->targets[0].player, instance->targets[0].card, 1);
        }
      }
    }
  }

  return 0;
}

/* ManalinkEh 2022: 0x26069b0; original cards/prophecy.c:4108. */
int card_scoria_cat(int player, int card, event_t event)
{
  int result;

  if (((event - EVENT_POWER < 2) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = control_an_untapped_land(player);
    if (result == 0)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        event_result = event_result + 3;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2606a20; original cards/prophecy.c:4127. */
int card_sheltering_prayers(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (((event == EVENT_ABILITIES) && (instance = in_play(player, card), instance != NULL)) &&
      (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
  {
    if (((affected_card_controller == player) &&
         (result = is_basic_land(affected_card_controller, affected_card), result != 0)) &&
        (result = count_permanents_by_type(affected_card_controller, TYPE_LAND), result < 4))
    {
      event_result = event_result | 0x20000;
    }
    if (((1 - player == affected_card_controller) &&
         (result = is_basic_land(affected_card_controller, affected_card), result != 0)) &&
        (result = count_permanents_by_type(affected_card_controller, TYPE_LAND), result < 4))
    {
      event_result = event_result | 0x20000;
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2607050; original cards/prophecy.c:4277. */
int card_spur_grappler(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_POWER)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = control_an_untapped_land(player);
      if (result == 0)
      {
        result = is_humiliated_for_ability(player, card, AL_STATIC);
        if (result == 0)
        {
          event_result = event_result + 2;
        }
      }
    }
  }
  else if (((event == EVENT_TOUGHNESS) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    result = control_an_untapped_land(player);
    if (result == 0)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        event_result = event_result + 1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2607280; original cards/prophecy.c:4315. */
int card_sunken_field(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  int player_00;
  card_instance_t *pcVar3;
  event_t event_00;
  target_definition_t *td_00;
  target_definition_t td;

  pcVar1 = in_play(player, card);
  if ((pcVar1 == NULL) ||
      (pcVar1 = get_card_instance(player, card), pcVar1->damage_target_player < '\0'))
  {
  LAB_026072bb:
    if (event == EVENT_CAN_CAST)
      goto LAB_026072d8;
    if (event != EVENT_CAST_SPELL)
      goto LAB_026072c5;
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_026072d8;
  }
  else
  {

    pcVar1 = get_card_instance(player, card);
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      player_00 = (int)pcVar1->damage_target_player;
      td_00 = &td;
      result = pcVar1->damage_target_card;
      counterspell_target_definition(player_00, result, td_00, 0xff);
      if (event == EVENT_CAN_ACTIVATE)
      {
        event_00 = EVENT_CAN_ACTIVATE;
      LAB_026074fd:
        result = granted_generic_activated_ability(player, card, player_00, result, event_00, 0x401,
                                                   0, 0, 0, 0, 0, 0, 0, td_00, NULL);
        return result;
      }
      if (event == EVENT_ACTIVATE)
      {

        pcVar3 = get_card_instance(player_00, result);
        pcVar3->targets[0].player = card_on_stack_controller;
        pcVar3->targets[0].card = card_on_stack;
        pcVar1->number_of_targets = '\x01';
        td_00 = NULL;
        event_00 = EVENT_ACTIVATE;
        goto LAB_026074fd;
      }
    }
    else
    {
      if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      {
        return 0;
      }
      counterspell_target_definition((int)pcVar1->damage_target_player, pcVar1->damage_target_card,
                                     &td, 0xff);
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_026072bb;
    counterspell_resolve_unless_pay_x(player, card, &td, 0, 1);
  LAB_026072c5:
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
      goto LAB_026072d8;
  }
  if ((((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (2 < event - EVENT_POWER)) &&
        ((event != EVENT_RESOLVE_MOVING_AURA &&
          ((event != EVENT_CHANGE_TYPE && (event != EVENT_SET_COLOR)))))) &&
       (event != EVENT_GET_SELECTED_CARD)) &&
      (event != EVENT_STATIC_EFFECTS))
  {
    return 0;
  }
LAB_026072d8:
  default_target_definition(player, card, &td, 1);
  td.preferred_controller = player;
  result = targeted_aura(player, card, event, &td, "TARGET_LAND");
  return result;
}

/* ManalinkEh 2022: 0x25f9620; original cards/prophecy.c:4367. */
static int task_mage_assembly_legacy(int player, int card, event_t event)
{
  int granting_card;
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  event_t event_00;
  target_definition_t *td_00;
  const char *prompt;
  target_definition_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar2 = instance->targets[0].player;
      if (-1 < iVar2)
      {
        iVar2 = get_id(iVar2, instance->targets[0].card);
        if (iVar2 != 0x1324)
        {
          kill_card(player, card, KILL_EXILE);
        }
        instance = in_play(instance->targets[0].player, instance->targets[0].card);
        if (instance == NULL)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      get_card_instance(player, card);
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      instance = get_card_instance(player, card);
      if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      {
        td_00 = &td;
        iVar2 = instance->targets[0].player;
        granting_card = instance->targets[0].card;
        default_target_definition(player, card, td_00, 2);
        if (event == EVENT_CAN_ACTIVATE)
        {
          prompt = "TARGET_CREATURE";
          iVar3 = 0x14;
          event_00 = EVENT_CAN_ACTIVATE;
        LAB_025f9888:
          iVar2 = granted_generic_activated_ability(iVar2, granting_card, player, card, event_00,
                                                    iVar3, 2, 0, 0, 0, 0, 0, 0, td_00, prompt);
          return iVar2;
        }
        if (event == EVENT_ACTIVATE)
        {
          instance->number_of_targets = '\x01';
          iVar3 = new_pick_target(td_00, "TARGET_CREATURE", 0, 0);
          if (iVar3 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          prompt = NULL;
          td_00 = NULL;
          iVar3 = 0x10;
          event_00 = EVENT_ACTIVATE;
          goto LAB_025f9888;
        }
      }
      else
      {
        if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        {
          return 0;
        }
        default_target_definition(player, card, &td, 2);
      }
      if ((event == EVENT_RESOLVE_ACTIVATION) &&
          (iVar2 = validate_target(player, card, &td, 0), iVar2 != 0))
      {
        damage_target0(player, card, 1);
      }
    }
  }

  return 0;
}

/* ManalinkEh 2022: 0x2607880; original cards/prophecy.c:4476. */
int card_thresher_beast(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_DECLARE_BLOCKERS)
  {
    result = is_attacking(player, card);
    if (result != 0)
    {
      result = is_unblocked(player, card);
      if (result == 0)
      {
        result = is_humiliated_for_ability(player, card, AL_TRIG);
        if (result == 0)
        {
          player_sacrifices_a_permanent(player, card, 1 - current_turn, 1,
                                        SAC_CAUSED | SAC_NO_CANCEL);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25f9900; original cards/prophecy.c:4585. */
static int vintara_elephant_legacy(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar3 = instance->targets[0].player;
      if (-1 < iVar3)
      {
        iVar3 = get_id(iVar3, instance->targets[0].card);
        if (iVar3 != 0x132c)
        {
          kill_card(player, card, KILL_EXILE);
        }
        instance = in_play(instance->targets[0].player, instance->targets[0].card);
        if (instance == NULL)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      get_card_instance(player, card);
    }
    return 0;
  }
  instance = get_card_instance(player, card);
  if (instance->targets[0].player == -1)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  iVar3 = instance->targets[0].player;
  card_00 = instance->targets[0].card;
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_025f9b72:
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
    LAB_025f9968:

      iVar2 = pump_ability_until_eot(iVar3, card_00, iVar3, card_00, 0, 0, 0x80, 0);
      if (iVar2 != -1)
      {
        instance = get_card_instance(iVar3, iVar2);
        instance->targets[4].player = 0;
      }
    }
  }
  else
  {
    if (event != EVENT_ACTIVATE)
    {
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        if (event != EVENT_CLEANUP)
        {
          return 0;
        }
        goto LAB_025f9b72;
      }
      goto LAB_025f9968;
    }
    instance = get_card_instance(iVar3, card_00);
    ai_modifier =
        ai_modifier + (-(unsigned int)((instance->regen_status & 0x80) == 0) & 0xffffffa0) + 0x30;
  }
  iVar2 = 1;
  if (ai_is_speculating != 1)
  {
    iVar2 = 0x2000;
    if ((trace_mode & 2U) == 0 && player == 0)
      goto LAB_025f9a6a;
    iVar2 = 0;
  }
  iVar2 = (unsigned int)(1 - player == iVar2) << 0xd;
LAB_025f9a6a:
  iVar3 = granted_generic_activated_ability(iVar3, card_00, player, card, event, iVar2, 3, 0, 0, 0,
                                            0, 0, 0, NULL, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x2608080; original cards/prophecy.c:4681. */
int card_vintara_snapper(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_ABILITIES) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = control_an_untapped_land(player);
    if (result == 0)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        event_result = event_result | 0x20000;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x25fad80; original cards/prophecy.c:4693. */
static int wall_of_vipers_legacy(int player, int card, event_t event)
{
  int granting_card;
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t *ptStack_68;
  target_definition_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar3 = instance->targets[0].player;
      if (-1 < iVar3)
      {
        iVar3 = get_id(iVar3, instance->targets[0].card);
        if (iVar3 != 0x132f)
        {
          kill_card(player, card, KILL_EXILE);
        }
        instance = in_play(instance->targets[0].player, instance->targets[0].card);
        if (instance == NULL)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      get_card_instance(player, card);
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      instance = get_card_instance(player, card);
      iVar3 = instance->targets[0].player;
      granting_card = instance->targets[0].card;
      if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      {
        default_target_definition(player, card, &td, 2);
        if (event == EVENT_CAN_ACTIVATE)
        {
          if (current_phase != 0x18)
          {
            return 0;
          }
          if (current_turn == iVar3)
          {
            return 0;
          }
          if (instance->blocking == 0xff)
          {
            return 0;
          }
          iVar2 = would_validate_arbitrary_target(&td, 1 - iVar3, (unsigned int)instance->blocking);
          if (iVar2 == 0)
          {
            return 0;
          }
          iVar3 = granted_generic_activated_ability(iVar3, granting_card, player, card,
                                                    EVENT_CAN_ACTIVATE, 4, 3, 0, 0, 0, 0, 0, 0, &td,
                                                    "TARGET_CREATURE");
          return iVar3;
        }
      }
      else
      {
        if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
        {
          return 0;
        }
        default_target_definition(player, card, &td, 2);
      }
      ptStack_68 = &td;
      if (event == EVENT_ACTIVATE)
      {
        if ((((current_phase == 0x18) && (current_turn != iVar3)) &&
             ((instance->blocking != 0xff &&
               ((iVar2 = would_validate_arbitrary_target(ptStack_68, 1 - iVar3,
                                                         (unsigned int)instance->blocking),
                 iVar2 != 0 && (iVar3 = granted_generic_activated_ability(
                                    iVar3, granting_card, player, card, EVENT_ACTIVATE, 4, 3, 0, 0,
                                    0, 0, 0, 0, ptStack_68, "TARGET_CREATURE"),
                                iVar3 != 0)))))) &&
            (iVar3 = new_pick_target(ptStack_68,
                                     "Select target creature Wall of Vipers is blocking.", 0, 0x80),
             iVar3 != 0))
        {
          instance->info_slot = 1;
          instance->number_of_targets = '\x01';
          instance->targets[0].player = 1 - player;
          instance->targets[0].card = (unsigned int)instance->blocking;
        }
      }
      else if (((event == EVENT_RESOLVE_ACTIVATION) &&
                (kill_card(player, card, KILL_DESTROY), 0 < instance->info_slot)) &&
               (iVar3 = valid_target(ptStack_68), iVar3 != 0))
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
    }
  }

  return 0;
}

/* ManalinkEh 2022: 0x26086b0; original cards/prophecy.c:4905. */
int card_wintermoon_mesa(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  bool bVar4;
  target_definition_t *ptStack_68;
  target_definition_t td;

  comes_into_play_tapped(player, card, event);
  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      default_target_definition(player, card, &td, 1);
      instance = get_card_instance(player, card);

      iVar1 = paying_mana();
      if ((iVar1 == 0) && (iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0xd,
                                                             3, 0, 0, 0, 0, 0, 0, &td, NULL),
                           iVar1 != 0))
      {
        bVar4 = ai_is_speculating != 1;
        instance->info_slot = 2;
        if ((bVar4) && ((player != 1 || (((unsigned char)trace_mode & 2) != 0))))
        {
          iVar1 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "Get mana", 1,
                              1, "Tap 2 target lands", 1, 1, 0, 0, 0, 0);
          if (iVar1 == 0)
          {
            spell_fizzled = 1;
            return 0;
          }
          if (iVar1 == 1)
            goto LAB_02608760;
          if (iVar1 != 2)
          {
            return 0;
          }
        }
        add_state(player, card, 0x10);
        generic_activated_ability(player, card, EVENT_ACTIVATE, 4, 2, 0, 0, 0, 0, 0, 0, &td,
                                  "TARGET_LAND");
        if (spell_fizzled == 1)
        {
          remove_state(player, card, 0x10);
        }
        else
        {
          state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
          iVar1 = new_pick_target(&td, "TARGET_LAND", 1, 1);
          if (iVar1 == 0)
          {
            state_untargettable(instance->targets[0].player, instance->targets[0].card, 0);
            remove_state(player, card, 0x10);
            return 0;
          }
          kill_card(player, card, KILL_SACRIFICE);
          state_untargettable(instance->targets[0].player, instance->targets[0].card, 0);
          tapped_for_mana_color = -1;
        }
        return 0;
      }
      instance->info_slot = 1;
    LAB_02608760:
      iVar1 = mana_producer(player, card, EVENT_ACTIVATE);
      return iVar1;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
    {
      default_target_definition(player, card, &td, 1);
      instance = get_card_instance(player, card);
      ptStack_68 = &td;
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }
      if (instance->info_slot != 2)
      {
        return 0;
      }

      if (instance->number_of_targets == '\0')
      {
        return 0;
      }
      iVar1 = 0;
      do
      {
        iVar3 = validate_target(player, card, ptStack_68, iVar1);
        if (iVar3 != 0)
        {
          tap_card(instance->targets[iVar1].player, instance->targets[iVar1].card);
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < (int)(unsigned int)instance->number_of_targets);
      return 0;
    }
  }
  iVar1 = mana_producer(player, card, event);
  return iVar1;
}

/* ManalinkEh 2022: 0x25f9ba0; original cards/prophecy.c:4986. */
static int zerapa_minotaur_legacy(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;
  int iVar2;
  int iVar3;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar3 = instance->targets[0].player;
      if (-1 < iVar3)
      {
        iVar3 = get_id(iVar3, instance->targets[0].card);
        if (iVar3 != 0x1339)
        {
          kill_card(player, card, KILL_EXILE);
        }
        instance = in_play(instance->targets[0].player, instance->targets[0].card);
        if (instance == NULL)
        {
          kill_card(player, card, KILL_EXILE);
        }
      }
    }
    instance = get_card_instance(player, card);
    if (instance->targets[0].player != -1)
    {
      get_card_instance(player, card);
    }
    return 0;
  }
  instance = get_card_instance(player, card);
  if (instance->targets[0].player == -1)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  iVar3 = instance->targets[0].player;
  card_00 = instance->targets[0].card;
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_025f9e12:
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
    LAB_025f9c08:

      iVar2 = pump_ability_until_eot(iVar3, card_00, iVar3, card_00, 0, 0, 0x100, 0);
      if (iVar2 != -1)
      {
        instance = get_card_instance(iVar3, iVar2);
        instance->targets[4].player = 0;
      }
    }
  }
  else
  {
    if (event != EVENT_ACTIVATE)
    {
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        if (event != EVENT_CLEANUP)
        {
          return 0;
        }
        goto LAB_025f9e12;
      }
      goto LAB_025f9c08;
    }
    instance = get_card_instance(iVar3, card_00);
    ai_modifier =
        ai_modifier + (-(unsigned int)((instance->regen_status & 0x100) == 0) & 0xffffffd0) + 0x18;
  }
  iVar2 = 1;
  if (ai_is_speculating != 1)
  {
    iVar2 = 0x2000;
    if ((trace_mode & 2U) == 0 && player == 0)
      goto LAB_025f9d0a;
    iVar2 = 0;
  }
  iVar2 = (unsigned int)(1 - player == iVar2) << 0xd;
LAB_025f9d0a:
  iVar3 = granted_generic_activated_ability(iVar3, card_00, player, card, event, iVar2, 2, 0, 0, 0,
                                            0, 0, 0, NULL, NULL);
  return iVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2601960; original cards/prophecy.c:2177. */
int card_endbringers_revel(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int card_00;
  test_definition_t test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    pcVar2 = in_play(player, card);
    if (((pcVar2 != NULL) && (card == affected_card)) && (player == affected_card_controller))
    {

      iVar3 = 0;
      pcVar2 = get_card_instance(player, card);
      bVar1 = false;
      do
      {
        card_00 = 0;
        if (-1 < active_cards_count[iVar3])
        {
          do
          {
            pcVar4 = in_play(iVar3, card_00);

            if (((pcVar4 != NULL) && (iVar5 = is_what(iVar3, card_00, 0x80), iVar5 != 0)) &&
                ((pcVar4 = get_card_instance(iVar3, card_00),
                  pcVar4->info_slot_fn == endbringers_revel_legacy &&
                      ((pcVar4->targets[0].player == player &&
                        (pcVar4->targets[0].card == card))))))
            {
              if (player == iVar3)
              {
                kill_card(player, card_00, KILL_EXILE);
              }
              else
              {
                if (iVar3 == 1)
                  goto LAB_02601a84;
                bVar1 = true;
              }
            }
            card_00 = card_00 + 1;
          } while (card_00 <= active_cards_count[iVar3]);
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 != 2);
      if ((!bVar1) && (pcVar2->targets[9].card != 0x42))
      {
        create_legacy_activate_for_opponent(player, card, endbringers_revel_legacy, player, card);
      }
    }
  }
  else if ((((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
            (event == EVENT_RESOLVE_ACTIVATION)) ||
           (event == EVENT_CLEANUP))
  {
    new_default_test_definition(&test, 2, "Select target creature card.");
    pcVar2 = get_card_instance(player, card);

    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (iVar3 = validate_target_from_grave_source(player, card, pcVar2->targets[0].player, 1),
         iVar3 != -1))
    {
      from_grave_to_hand(pcVar2->targets[0].player, iVar3, 1);
    }
    iVar3 = generic_activated_ability_with_target_in_grave(
        player, card, event, GAA_CAN_SORCERY_BE_PLAYED, 4, 0, 0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE,
        &test, 0x1000);
    return iVar3;
  }
LAB_02601a84:
  iVar3 = global_enchantment(player, card, event);
  return iVar3;
}

/* ManalinkEh 2022: 0x2601c40; original cards/prophecy.c:2287. */
int card_excavation(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *pcVar2;
  int iVar3;
  unsigned int uVar4;
  card_instance_t *pcVar5;
  int iVar6;
  int iVar7;
  unsigned int player_00;
  test_definition_t test;

  if (event != EVENT_STATIC_EFFECTS)
  {
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar3 = can_sacrifice_type_as_cost(player, 1, TYPE_LAND);
      iVar7 = 0;
      if (iVar3 != 0)
      {
        iVar7 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 1, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
      }
    }
    else if (event == EVENT_ACTIVATE)
    {

      new_default_test_definition(&test, 1, "Select a land to sacrifice.");
      uVar4 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                            &test);
      if (uVar4 == 0)
      {
        spell_fizzled = 1;
        iVar7 = 0;
      }
      else
      {
        generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 1, 0, 0, 0, 0, 0, 0, NULL, NULL);
        player_00 = (int)uVar4 >> 0x10 & 0xff;
        if (spell_fizzled == 1)
        {
          iVar7 = 0;
          state_untargettable(player_00, uVar4 >> 0x18, 0);
        }
        else
        {
          iVar7 = 0;
          kill_card(player_00, uVar4 >> 0x18, KILL_SACRIFICE);
        }
      }
    }
    else
    {
      if (event == EVENT_RESOLVE_ACTIVATION)
      {
        draw_a_card(player);
        return 0;
      }
      if (event != EVENT_CLEANUP)
        goto LAB_02601cdc;
      iVar7 = 0;
    }
    return iVar7;
  }
  pcVar2 = in_play(player, card);
  if (((pcVar2 != NULL) && (card == affected_card)) && (player == affected_card_controller))
  {

    iVar7 = 0;
    pcVar2 = get_card_instance(player, card);
    bVar1 = false;
    do
    {
      iVar3 = 0;
      if (-1 < active_cards_count[iVar7])
      {
        do
        {
          pcVar5 = in_play(iVar7, iVar3);

          if (((pcVar5 != NULL) && (iVar6 = is_what(iVar7, iVar3, 0x80), iVar6 != 0)) &&
              ((pcVar5 = get_card_instance(iVar7, iVar3),
                pcVar5->info_slot_fn == excavation_legacy &&
                    ((pcVar5->targets[0].player == player && (pcVar5->targets[0].card == card))))))
          {
            if (player == iVar7)
            {
              kill_card(player, iVar3, KILL_EXILE);
            }
            else
            {
              if (iVar7 == 1)
                goto LAB_02601cdc;
              bVar1 = true;
            }
          }
          iVar3 = iVar3 + 1;
        } while (iVar3 <= active_cards_count[iVar7]);
      }
      iVar7 = iVar7 + 1;
    } while (iVar7 != 2);
    if ((!bVar1) && (pcVar2->targets[9].card != 0x42))
    {
      create_legacy_activate_for_opponent(player, card, excavation_legacy, player, card);
    }
  }
LAB_02601cdc:
  iVar7 = global_enchantment(player, card, event);
  return iVar7;
}

/* ManalinkEh 2022: 0x2605540; original cards/prophecy.c:3597. */
int card_quicksilver_wall(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int iVar5;
  int card_00;

  if (event == EVENT_STATIC_EFFECTS)
  {
    pcVar2 = in_play(player, card);
    if (((pcVar2 != NULL) && (card == affected_card)) && (player == affected_card_controller))
    {

      iVar4 = 0;
      pcVar2 = get_card_instance(player, card);
      bVar1 = false;
      do
      {
        card_00 = 0;
        if (-1 < active_cards_count[iVar4])
        {
          do
          {
            pcVar3 = in_play(iVar4, card_00);

            if (((pcVar3 != NULL) && (iVar5 = is_what(iVar4, card_00, 0x80), iVar5 != 0)) &&
                ((pcVar3 = get_card_instance(iVar4, card_00),
                  pcVar3->info_slot_fn == quicksilver_wall_legacy &&
                      ((pcVar3->targets[0].player == player &&
                        (pcVar3->targets[0].card == card))))))
            {
              if (player == iVar4)
              {
                kill_card(player, card_00, KILL_EXILE);
              }
              else
              {
                if (iVar4 == 1)
                {
                  return 0;
                }
                bVar1 = true;
              }
            }
            card_00 = card_00 + 1;
          } while (card_00 <= active_cards_count[iVar4]);
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 != 2);
      if ((!bVar1) && (pcVar2->targets[9].card != 0x42))
      {
        create_legacy_activate_for_opponent(player, card, quicksilver_wall_legacy, player, card);
      }
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_026055c1:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_026055c6;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_026055c1;
  }

  pcVar2 = get_card_instance(player, card);
  pcVar3 = in_play(pcVar2->parent_controller, pcVar2->parent_card);
  if (pcVar3 != NULL)
  {
    bounce_permanent(pcVar2->parent_controller, pcVar2->parent_card);
  }
LAB_026055c6:
  iVar4 = generic_activated_ability(player, card, event, 0, 4, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar4;
}

/* ManalinkEh 2022: 0x26063c0; original cards/prophecy.c:3928. */
int card_ribbon_snake(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int card_00;

  if (event == EVENT_STATIC_EFFECTS)
  {
    pcVar2 = in_play(player, card);
    if (((pcVar2 != NULL) && (card == affected_card)) && (player == affected_card_controller))
    {

      iVar3 = 0;
      pcVar2 = get_card_instance(player, card);
      bVar1 = false;
      do
      {
        card_00 = 0;
        if (-1 < active_cards_count[iVar3])
        {
          do
          {
            pcVar4 = in_play(iVar3, card_00);

            if (((pcVar4 != NULL) && (iVar5 = is_what(iVar3, card_00, 0x80), iVar5 != 0)) &&
                ((pcVar4 = get_card_instance(iVar3, card_00),
                  pcVar4->info_slot_fn == ribbon_snake_legacy &&
                      ((pcVar4->targets[0].player == player &&
                        (pcVar4->targets[0].card == card))))))
            {
              if (player == iVar3)
              {
                kill_card(player, card_00, KILL_EXILE);
              }
              else
              {
                if (iVar3 == 1)
                {
                  return 0;
                }
                bVar1 = true;
              }
            }
            card_00 = card_00 + 1;
          } while (card_00 <= active_cards_count[iVar3]);
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 != 2);
      if ((!bVar1) && (pcVar2->targets[9].card != 0x42))
      {
        create_legacy_activate_for_opponent(player, card, ribbon_snake_legacy, player, card);
      }
    }
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_02606479:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02606482;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      pcVar2 = get_card_instance(player, card);
      ai_modifier =
          ai_modifier + (-(unsigned int)((pcVar2->regen_status & 0x20) == 0) & 0xffffffa0) + 0x30;
      goto LAB_02606482;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_02606479;
    }
  }

  pcVar2 = get_card_instance(player, card);
  iVar3 = pump_ability_until_eot(pcVar2->parent_controller, pcVar2->parent_card,
                                 pcVar2->parent_controller, pcVar2->parent_card, 0, 0, 0x20, 0);
  if (iVar3 != -1)
  {
    pcVar2 = get_card_instance(pcVar2->parent_controller, iVar3);
    pcVar2->targets[4].player = 0;
  }
LAB_02606482:
  iVar3 = generic_activated_ability(player, card, event, 0, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x2606730; original cards/prophecy.c:4053. */
int card_samite_sanctuary(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int card_00;
  target_definition_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    pcVar2 = in_play(player, card);
    if (((pcVar2 != NULL) && (card == affected_card)) && (player == affected_card_controller))
    {

      pcVar2 = get_card_instance(player, card);

      iVar3 = 0;
      bVar1 = false;
      do
      {
        card_00 = 0;
        if (-1 < active_cards_count[iVar3])
        {
          do
          {
            pcVar4 = in_play(iVar3, card_00);

            if (((pcVar4 != NULL) && (iVar5 = is_what(iVar3, card_00, 0x80), iVar5 != 0)) &&
                ((pcVar4 = get_card_instance(iVar3, card_00),
                  pcVar4->info_slot_fn == samite_sanctuary_legacy &&
                      ((pcVar4->targets[0].player == player &&
                        (pcVar4->targets[0].card == card))))))
            {
              if (player == iVar3)
              {
                kill_card(player, card_00, KILL_EXILE);
              }
              else
              {
                if (iVar3 == 1)
                  goto LAB_02606838;
                bVar1 = true;
              }
            }
            card_00 = card_00 + 1;
          } while (card_00 <= active_cards_count[iVar3]);
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 != 2);
      if ((!bVar1) && (pcVar2->targets[9].card != 0x42))
      {
        create_legacy_activate_for_opponent(player, card, samite_sanctuary_legacy, player, card);
      }
    }
  }
  else if ((((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
            (event == EVENT_RESOLVE_ACTIVATION)) ||
           (event == EVENT_CLEANUP))
  {
    default_target_definition(player, card, &td, 0xff);
    td.extra = damage_card;
    td.special = 0x100000;
    pcVar2 = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (iVar3 = validate_target(player, card, &td, 0), iVar3 != 0))
    {
      prevent_damage(pcVar2->targets[0].player, pcVar2->targets[0].card, 1);
    }
    iVar3 = generic_activated_ability(player, card, event, 0x1000004, 2, 0, 0, 0, 0, 0, 0, &td,
                                      "TARGET_DAMAGE");
    return iVar3;
  }
LAB_02606838:
  iVar3 = global_enchantment(player, card, event);
  return iVar3;
}

/* ManalinkEh 2022: 0x2607580; original cards/prophecy.c:4420. */
int card_task_mage_assembly(int player, int card, event_t event)
{
  bool bVar1;
  int iVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int card_00;
  target_definition_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    pcVar3 = in_play(player, card);
    if (((pcVar3 != NULL) && (card == affected_card)) && (player == affected_card_controller))
    {

      iVar2 = count_subtype(2, 2, -1);
      if (iVar2 == 0)
      {
        kill_card(player, card, KILL_SACRIFICE);
      }

      iVar2 = 0;
      pcVar3 = get_card_instance(player, card);
      bVar1 = false;
      do
      {
        card_00 = 0;
        if (-1 < active_cards_count[iVar2])
        {
          do
          {
            pcVar4 = in_play(iVar2, card_00);

            if (((pcVar4 != NULL) && (iVar5 = is_what(iVar2, card_00, 0x80), iVar5 != 0)) &&
                ((pcVar4 = get_card_instance(iVar2, card_00),
                  pcVar4->info_slot_fn == task_mage_assembly_legacy &&
                      ((pcVar4->targets[0].player == player &&
                        (pcVar4->targets[0].card == card))))))
            {
              if (player == iVar2)
              {
                kill_card(player, card_00, KILL_EXILE);
              }
              else
              {
                if (iVar2 == 1)
                {
                  count_subtype(2, 2, -1);
                  goto LAB_02607678;
                }
                bVar1 = true;
              }
            }
            card_00 = card_00 + 1;
          } while (card_00 <= active_cards_count[iVar2]);
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 != 2);
      iVar2 = count_subtype(2, 2, -1);
      if (((!bVar1) && (iVar2 != 0)) && (pcVar3->targets[9].card != 0x42))
      {
        create_legacy_activate_for_opponent(player, card, task_mage_assembly_legacy, player, card);
      }
    }
  }
  else if (((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE)) ||
           ((event == EVENT_RESOLVE_ACTIVATION || (event == EVENT_CLEANUP))))
  {
    default_target_definition(player, card, &td, 2);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (iVar2 = valid_target(&td), iVar2 != 0))
    {
      damage_target0(player, card, 1);
    }
    iVar2 = generic_activated_ability(player, card, event, 0x14, 2, 0, 0, 0, 0, 0, 0, &td,
                                      "TARGET_CREATURE");
    return iVar2;
  }
LAB_02607678:
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x2607dd0; original cards/prophecy.c:4625. */
int card_vintara_elephant(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int card_00;

  if (event == EVENT_STATIC_EFFECTS)
  {
    pcVar2 = in_play(player, card);
    if (((pcVar2 != NULL) && (card == affected_card)) && (player == affected_card_controller))
    {

      iVar3 = 0;
      pcVar2 = get_card_instance(player, card);
      bVar1 = false;
      do
      {
        card_00 = 0;
        if (-1 < active_cards_count[iVar3])
        {
          do
          {
            pcVar4 = in_play(iVar3, card_00);

            if (((pcVar4 != NULL) && (iVar5 = is_what(iVar3, card_00, 0x80), iVar5 != 0)) &&
                ((pcVar4 = get_card_instance(iVar3, card_00),
                  pcVar4->info_slot_fn == vintara_elephant_legacy &&
                      ((pcVar4->targets[0].player == player &&
                        (pcVar4->targets[0].card == card))))))
            {
              if (player == iVar3)
              {
                kill_card(player, card_00, KILL_EXILE);
              }
              else
              {
                if (iVar3 == 1)
                {
                  return 0;
                }
                bVar1 = true;
              }
            }
            card_00 = card_00 + 1;
          } while (card_00 <= active_cards_count[iVar3]);
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 != 2);
      if ((!bVar1) && (pcVar2->targets[9].card != 0x42))
      {
        create_legacy_activate_for_opponent(player, card, vintara_elephant_legacy, player, card);
      }
    }
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_02607e89:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02607e92;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      pcVar2 = get_card_instance(player, card);
      ai_modifier =
          ai_modifier + (-(unsigned int)((pcVar2->regen_status & 0x80) == 0) & 0xffffffa0) + 0x30;
      goto LAB_02607e92;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_02607e89;
    }
  }

  pcVar2 = get_card_instance(player, card);
  iVar3 = pump_ability_until_eot(pcVar2->parent_controller, pcVar2->parent_card,
                                 pcVar2->parent_controller, pcVar2->parent_card, 0, 0, 0x80, 0);
  if (iVar3 != -1)
  {
    pcVar2 = get_card_instance(pcVar2->parent_controller, iVar3);
    pcVar2->targets[4].player = 0;
  }
LAB_02607e92:
  iVar3 = generic_activated_ability(player, card, event, 0, 3, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x26080e0; original cards/prophecy.c:4761. */
int card_wall_of_vipers(int player, int card, event_t event)
{
  unsigned char bVar1;
  bool bVar2;
  card_instance_t *pcVar3;
  int iVar4;
  card_instance_t *pcVar5;
  int iVar6;
  int card_00;
  target_definition_t td;

  if (event == EVENT_STATIC_EFFECTS)
  {
    pcVar3 = in_play(player, card);
    if (((pcVar3 != NULL) && (card == affected_card)) && (player == affected_card_controller))
    {

      iVar4 = count_subtype(2, 2, -1);
      if (iVar4 == 0)
      {
        kill_card(player, card, KILL_SACRIFICE);
      }

      iVar4 = 0;
      pcVar3 = get_card_instance(player, card);
      bVar2 = false;
      do
      {
        card_00 = 0;
        if (-1 < active_cards_count[iVar4])
        {
          do
          {
            pcVar5 = in_play(iVar4, card_00);

            if (((pcVar5 != NULL) && (iVar6 = is_what(iVar4, card_00, 0x80), iVar6 != 0)) &&
                ((pcVar5 = get_card_instance(iVar4, card_00),
                  pcVar5->info_slot_fn == wall_of_vipers_legacy &&
                      ((pcVar5->targets[0].player == player &&
                        (pcVar5->targets[0].card == card))))))
            {
              if (player == iVar4)
              {
                kill_card(player, card_00, KILL_EXILE);
              }
              else
              {
                if (iVar4 == 1)
                  goto LAB_0260836d;
                bVar2 = true;
              }
            }
            card_00 = card_00 + 1;
          } while (card_00 <= active_cards_count[iVar4]);
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 != 2);
      if ((!bVar2) && (pcVar3->targets[9].card != 0x42))
      {
        create_legacy_activate_for_opponent(player, card, wall_of_vipers_legacy, player, card);
      }
    }
  LAB_0260836d:
    default_target_definition(player, card, &td, 2);
  }
  else
  {
    default_target_definition(player, card, &td, 2);
    if (event == EVENT_CAN_ACTIVATE)
    {

      pcVar3 = get_card_instance(player, card);
      if ((((current_phase == 0x18) && (current_turn != player)) && (pcVar3->blocking != 0xff)) &&
          (iVar4 = would_validate_arbitrary_target(&td, 1 - player, (unsigned int)pcVar3->blocking),
           iVar4 != 0))
      {
        iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        return iVar4;
      }
    }
    else if (event == EVENT_ACTIVATE)
    {

      pcVar3 = get_card_instance(player, card);
      if (((current_phase == 0x18) && (current_turn != player)) && (pcVar3->blocking != 0xff))
      {
        iVar4 = would_validate_arbitrary_target(&td, 1 - player, (unsigned int)pcVar3->blocking);
        if (((iVar4 != 0) &&
             (iVar4 = charge_mana_for_activated_ability(player, card, 3, 0, 0, 0, 0, 0),
              iVar4 != 0)) &&
            (iVar4 = new_pick_target(&td, "Select target creature Wall of Vipers is blocking.", 0,
                                     0x80),
             iVar4 != 0))
        {
          bVar1 = pcVar3->blocking;
          pcVar3->targets[0].player = 1 - player;
          pcVar3->targets[0].card = (unsigned int)bVar1;
          pcVar3->info_slot = 1;
          pcVar3->number_of_targets = '\x01';
        }
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {

      pcVar3 = get_card_instance(player, card);
      kill_card(player, card, KILL_DESTROY);
      if ((0 < pcVar3->info_slot) && (iVar4 = valid_target(&td), iVar4 != 0))
      {
        kill_card(pcVar3->targets[0].player, pcVar3->targets[0].card, KILL_DESTROY);
        return 0;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x2608ac0; original cards/prophecy.c:5026. */
int card_zerapa_minotaur(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *pcVar2;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int card_00;

  if (event == EVENT_STATIC_EFFECTS)
  {
    pcVar2 = in_play(player, card);
    if (((pcVar2 != NULL) && (card == affected_card)) && (player == affected_card_controller))
    {

      iVar3 = 0;
      pcVar2 = get_card_instance(player, card);
      bVar1 = false;
      do
      {
        card_00 = 0;
        if (-1 < active_cards_count[iVar3])
        {
          do
          {
            pcVar4 = in_play(iVar3, card_00);

            if (((pcVar4 != NULL) && (iVar5 = is_what(iVar3, card_00, 0x80), iVar5 != 0)) &&
                ((pcVar4 = get_card_instance(iVar3, card_00),
                  pcVar4->info_slot_fn == zerapa_minotaur_legacy &&
                      ((pcVar4->targets[0].player == player &&
                        (pcVar4->targets[0].card == card))))))
            {
              if (player == iVar3)
              {
                kill_card(player, card_00, KILL_EXILE);
              }
              else
              {
                if (iVar3 == 1)
                {
                  return 0;
                }
                bVar1 = true;
              }
            }
            card_00 = card_00 + 1;
          } while (card_00 <= active_cards_count[iVar3]);
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 != 2);
      if ((!bVar1) && (pcVar2->targets[9].card != 0x42))
      {
        create_legacy_activate_for_opponent(player, card, zerapa_minotaur_legacy, player, card);
      }
    }
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_02608b79:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02608b82;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      pcVar2 = get_card_instance(player, card);
      ai_modifier =
          ai_modifier + (-(unsigned int)((pcVar2->regen_status & 0x100) == 0) & 0xffffffa0) + 0x30;
      goto LAB_02608b82;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_02608b79;
    }
  }

  pcVar2 = get_card_instance(player, card);
  iVar3 = pump_ability_until_eot(pcVar2->parent_controller, pcVar2->parent_card,
                                 pcVar2->parent_controller, pcVar2->parent_card, 0, 0, 0x100, 0);
  if (iVar3 != -1)
  {
    pcVar2 = get_card_instance(pcVar2->parent_controller, iVar3);
    pcVar2->targets[4].player = 0;
  }
LAB_02608b82:
  iVar3 = generic_activated_ability(player, card, event, 0, 2, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return iVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
void death_charmer_effect(int player, int card, int t_player, int t_card);
void plague_fiend_effect(int player, int card, int t_player, int t_card);
void soul_charmer_effect(int player, int card, int t_player, int t_card);
int destroy_unless_pay_2(int player, int card, event_t event);
int gain_2_life_unless_they_pay_2(int player, int card, event_t event);
int lose_2_life_unless_pay_2(int player, int card, event_t event);
int get_power_sum_of_untapped_creatures(int player);

/* ManalinkEh 2022: 0x2601180; original cards/prophecy.c:1935. */
int card_copper_leaf_angel(int player, int card, event_t event)
{
  unsigned int uVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  bool bVar5;
  card_instance_t *instance;
  int iStack_348;
  test_definition_t test2;
  int devoured[151];

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    new_default_test_definition(&test2, 1, "");
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar2 = new_can_sacrifice_as_cost(player, card, &test2);
      if (iVar2 == 0)
      {
        return 0;
      }
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      return iVar2;
    }
    if (event == EVENT_ACTIVATE)
    {

      bVar5 = ai_is_speculating == 1;
      instance->info_slot = 0;
      if ((bVar5) || ((iStack_348 = 0x97, player == 1 && (((unsigned char)trace_mode & 2) == 0))))
      {
        iStack_348 = internal_rand(10);
        iStack_348 = iStack_348 + 3;
      }
      iVar2 = 0;

      while ((iVar3 = iVar2, iVar2 = new_can_sacrifice_as_cost(player, card, &test2),
              iVar2 != 0 &&
                  (uVar1 = new_sacrifice(player, card, player,
                                         SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test2),
                   uVar1 != 0)))
      {
        devoured[iVar3] = uVar1 >> 0x18;
        iVar2 = iVar3 + 1;
        if (iStack_348 == iVar3 + 1)
        {
        LAB_026012ba:
          generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL,
                                    NULL);
          if (iVar3 < 0)
          {
            return 0;
          }
          piVar4 = devoured + iVar3;
          do
          {
            if (spell_fizzled == 1)
            {
              state_untargettable(player, *piVar4, 0);
            }
            else
            {
              kill_card(player, *piVar4, KILL_SACRIFICE);
              instance->info_slot_fn = instance->info_slot_fn + 1;
            }
            bVar5 = piVar4 != devoured;
            piVar4 = piVar4 + -1;
          } while (bVar5);
          return 0;
        }
      }
      iVar3 = iVar3 + -1;
      goto LAB_026012ba;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test2, 1, "");
    instance = get_card_instance(player, card);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    add_counters_from_source(player, card, instance->parent_controller, instance->parent_card,
                             COUNTER_P1_P1, instance->info_slot);
  }
  return 0;
}

/* ManalinkEh 2022: 0x25fb320; original cards/prophecy.c:1995. */
int lose_2_life_unless_pay_2(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_RESOLVE_TRIGGER)
  {

    instance = get_card_instance(player, card);
    result = has_mana((int)instance->damage_target_player, 7, 2);
    if ((result == 0) ||
        (result = charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER,
                                              (int)instance->damage_target_player, COLOR_ANY, 2),
         result == 0))
    {
      lose_life((int)instance->damage_target_player, 2);
    }
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25f8da0; original cards/prophecy.c:2017. */
void death_charmer_effect(int player, int card, int t_player, int t_card)
{
  create_targetted_legacy_effect(player, card, lose_2_life_unless_pay_2, t_player, t_card);
  return;
}

/* ManalinkEh 2022: 0x2601490; original cards/prophecy.c:2021. */
int card_death_charmer(int player, int card, event_t event)
{
  int result;

  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result != 0)
  {
    return 0;
  }
  for_each_creature_damaged_by_me(player, card, event, 4, death_charmer_effect, player, card);
  return 0;
}

/* ManalinkEh 2022: 0x2601ff0; original cards/prophecy.c:2357. */
int card_excise(int player, int card, event_t event)
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
        td.required_state = 2;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if ((result != 0) &&
              ((result = has_mana(instance->targets[0].player, 7, instance->info_slot),
                result == 0 || (result = charge_mana_while_resolving(
                                    player, card, EVENT_RESOLVE_SPELL, instance->targets[0].player,
                                    COLOR_ANY, instance->info_slot),
                                result == 0))))
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_0260205d;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.required_state = 2;
  get_card_instance(player, card);
LAB_0260205d:
  result =
      generic_spell(player, card, event, 0x85, &td, "Select target attacking creature.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x26024d0; original cards/prophecy.c:2455. */
int card_flameshot(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_t *ret_location;
  int iStack_140;
  target_definition_t td;
  test_definition_t this_test;

  if (event == EVENT_MODIFY_COST)
  {
    new_default_test_definition(&this_test, 1, "Select a Mountain card.");
    this_test.zone = 0x100;
    this_test.subtype = 0x5006;
    result = check_battlefield_for_special_card(player, card, player, 0, &this_test);
    if (result == 0)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
      return 0;
    }
    null_casting_cost(player, card);
    instance = get_card_instance(player, card);
    instance->info_slot = 1;
    return 0;
  }
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    new_default_test_definition(&this_test, 1, "Select a Mountain card.");
    this_test.zone = 0x100;
    this_test.subtype = 0x5006;
    get_card_instance(player, card);
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    return result;
  }
  if (event != EVENT_CAST_SPELL)
  {
    if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_RESOLVE_SPELL))
    {
      if (event == EVENT_CHANGE_TARGET)
      {
        default_target_definition(player, card, &td, 2);
        new_default_test_definition(&this_test, 1, "Select a Mountain card.");
        this_test.zone = 0x100;
        this_test.subtype = 0x5006;
        get_card_instance(player, card);
        return 0;
      }
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    new_default_test_definition(&this_test, 1, "Select a Mountain card.");
    this_test.zone = 0x100;
    this_test.subtype = 0x5006;
    get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      divide_damage(player, card, &td);
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
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
  default_target_definition(player, card, &td, 2);
  new_default_test_definition(&this_test, 1, "Select a Mountain card.");
  this_test.zone = 0x100;
  this_test.subtype = 0x5006;
  instance = get_card_instance(player, card);
  if (card != affected_card)
  {
    return 0;
  }
  if (player != affected_card_controller)
  {
    return 0;
  }

  instance->number_of_targets = '\0';
  result = played_for_free(player, card);
  if (((result == 0) && (result = is_token(player, card), result == 0)) &&
      (instance->info_slot == 1))
  {

    result = get_id(player, card);
    result = has_mana_to_cast_id(player, EVENT_CAST_SPELL, result);
    if ((result == 0) ||
        (result = do_dialog(player, player, card, -1, -1,
                            " Discard a Mountain card\n Play normally\n Cancel", 1),
         result == 0))
    {
      result = new_select_a_card(player, player, 4, 0, 3, -1, &this_test);
      if (result < 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      discard_card(player, result);
      td.allow_cancel = 0;
    }
    else
    {
      if (result != 1)
      {
        spell_fizzled = 1;
        return 0;
      }
      result = get_id(player, card);
      charge_mana_from_id(player, card, EVENT_CAST_SPELL, result);
    }
    if (spell_fizzled == 1)
    {
      return 0;
    }
  }
  ret_location = instance->targets;
  iStack_140 = 0;
  do
  {
    result = select_target(player, card, &td, "Select target creature.", ret_location);
    if (result == 0)
    {
      if (iStack_140 != 3)
      {
        spell_fizzled = 1;
      }
      break;
    }
    iStack_140 = iStack_140 + 1;
    ret_location = ret_location + 1;
  } while (iStack_140 != 3);
  result = basic_spell(player, card, EVENT_CAST_SPELL);
  return result;
}

/* ManalinkEh 2022: 0x26029e0; original cards/prophecy.c:2545. */
int card_flay(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
  {
  LAB_02602a52:
    result = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_02602a52;
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      discard(instance->targets[0].player, 1, player);
      result = has_mana(instance->targets[0].player, 7, 1);
      if ((result == 0) ||
          (result = charge_mana(instance->targets[0].player, COLOR_ANY, 1), result == 0))
      {
        discard(instance->targets[0].player, 1, player);
      }
    }
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x2603b40; original cards/prophecy.c:2999. */
int card_jeweled_spirit(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  test_definition_t test;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    new_default_test_definition(&test, 1, "Select a land card to sacrifice.");
    test.qty = 2;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_CAN_ACTIVATE,
                                                                GAA_NONE, 0, 0, 0, 0, 0, 0, 0, NULL,
                                                                NULL, TYPE_NONE, &test, SARV_0);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_ACTIVATE, GAA_NONE, 0, 0,
                                                       0, 0, 0, 0, 0, NULL, NULL, TYPE_NONE, &test,
                                                       SARV_0);
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    new_default_test_definition(&test, 1, "Select a land card to sacrifice.");
  }
  test.qty = 2;
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  result = get_deck_color(player, 1 - player);
  result = do_dialog(player, player, card, -1, -1,
                     " Prot. from Black\n Prot. from Blue\n Prot. from Green\n Prot from Red\n "
                     "Prot. from White\n Prot from Artifacts",
                     result + -1);
  pump_ability_until_eot(player, card, instance->parent_controller, instance->parent_card, 0, 0,
                         1 << ((char)result + 0xbU & 0x1f) | 0x10000000, 0);
  return 0;
}

/* ManalinkEh 2022: 0x26040f0; original cards/prophecy.c:3093. */
int get_power_sum_of_untapped_creatures(int player)
{
  card_instance_t *instance;
  int result;
  int player_00;
  int card;
  int iStack_14;

  iStack_14 = 0;
  player_00 = 0;
  do
  {

    if (((player == player_00) || (player == 2)) && (0 < active_cards_count[player_00]))
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
            result = is_tapped(player_00, card);
            if (result == 0)
            {
              result = get_power(player_00, card);
              iStack_14 = iStack_14 + result;
            }
          }
        }
        card = card + 1;
      } while (card < active_cards_count[player_00]);
    }
    player_00 = player_00 + 1;
  } while (player_00 != 2);
  return iStack_14;
}

/* ManalinkEh 2022: 0x2604190; original cards/prophecy.c:3112. */
int card_keldon_battlewagon(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t td;

  cannot_block(player, card, event);
  iVar1 = declare_attackers_trigger(player, card, event, 0, player, card);
  if (iVar1 != 0)
  {
    create_targetted_legacy_effect(player, card, sacrifice_at_end_of_combat, player, card);
  }
  if (event - EVENT_POW_BOOST < 2)
  {

    base_target_definition(player, card, &td, 2);
    td.allowed_controller = player;
    td.preferred_controller = player;
    td.illegal_state = 1;
    iVar3 = target_available(player, card, &td);
    iVar1 = 0;
    if (0 < iVar3)
    {
      iVar1 = can_use_activated_abilities(player, card);
      if (iVar1 == 0)
      {
        return 0;
      }
      iVar1 = has_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0);
      if (iVar1 == 0)
      {
        return 0;
      }
      iVar1 = get_power_sum_of_untapped_creatures(player);
    }
  }
  else
  {
    if ((((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE)) &&
         (event != EVENT_RESOLVE_ACTIVATION)) &&
        (event != EVENT_CLEANUP))
    {
      return 0;
    }

    instance = get_card_instance(player, card);
    base_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.illegal_state = 1;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      iVar1 = get_power(instance->targets[9].player, instance->targets[9].card);
      iVar1 = generic_shade_merge_pt(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0, 0, 0, 0, 0, 0,
                                     iVar1, 0);
      return iVar1;
    }
    iVar1 = generic_activated_ability_with_tapsubtype_as_cost(player, card, event, GAA_NONE, 0, 0,
                                                              0, 0, 0, 0, 0, NULL, NULL, &td, 1);
  }
  return iVar1;
}

/* ManalinkEh 2022: 0x26045d0; original cards/prophecy.c:3201. */
int card_mana_vapors(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
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
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      get_card_instance(player, card);

      if (((card == affected_card) && (player == affected_card_controller)) &&
          (iVar1 = 1 - player, 0 < active_cards_count[iVar1]))
      {
        iVar4 = 0;
        do
        {
          instance = in_play(iVar1, iVar4);
          if (((instance != NULL) && (iVar3 = is_what(iVar1, iVar4, 1), iVar3 != 0)) &&
              (iVar3 = is_tapped(iVar1, iVar4), iVar3 != 0))
          {
            ai_modifier = ai_modifier + iVar4 * 4;
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[iVar1]);
      }
      goto LAB_02604647;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        iVar1 = valid_target(&td);
        if (iVar1 != 0)
        {

          iVar1 = instance->targets[0].player;
          iVar4 = active_cards_count[iVar1] + -1;
          if (-1 < iVar4)
          {
            do
            {
              instance = in_play(iVar1, iVar4);
              if ((instance != NULL) && (iVar3 = is_what(iVar1, iVar4, 1), iVar3 != 0))
              {
                does_not_untap_effect(player, card, iVar1, iVar4, 0, 1);
              }
              iVar4 = iVar4 + -1;
            } while (iVar4 != -1);
          }
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_02604647;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  get_card_instance(player, card);
LAB_02604647:
  iVar1 = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return iVar1;
}

/* ManalinkEh 2022: 0x2604d30; original cards/prophecy.c:3361. */
int card_nakaya_shade(int player, int card, event_t event)
{
  bool bVar1;
  int result;
  unsigned int uVar3;
  unsigned int argument1;

  if (event != EVENT_CAN_ACTIVATE)
  {
    if (event == EVENT_ACTIVATE)
    {
      result = charge_mana_for_activated_ability(player, card, 0, 1, 0, 0, 0, 0);
    }
    else
    {
      if (event == EVENT_RESOLVE_ACTIVATION)
      {

        uVar3 = 0;
        bVar1 = true;
        do
        {
          argument1 = current_turn ^ uVar3;
          result = has_mana(argument1, 7, 2);

          if ((((bVar1) && (result != 0)) &&
               (result = do_dialog(argument1, player, card, -1, -1, " Pay 2 stop boost\n Pass",
                                   (unsigned int)(player == argument1)),
                result == 0)) &&
              (charge_mana_while_resolving(player, card, EVENT_RESOLVE_ACTIVATION, argument1,
                                           COLOR_COLORLESS, 2),
               spell_fizzled != 1))
          {
            bVar1 = false;
          }
          uVar3 = uVar3 + 1;
        } while (uVar3 != 2);
        if (bVar1)
        {
          generic_shade_merge_pt(player, card, EVENT_RESOLVE_ACTIVATION, 0, 0, 1, 0, 0, 0, 0, 1, 1);
        }
      }
      else if (event != EVENT_CLEANUP)
        goto LAB_02604da4;
      result = 0;
    }
    return result;
  }
  event = EVENT_CAN_ACTIVATE;
LAB_02604da4:
  result = generic_shade_merge_pt(player, card, event, 0, 0, 1, 0, 0, 0, 0, 1, 1);
  return result;
}

/* ManalinkEh 2022: 0x2605170; original cards/prophecy.c:3430. */
int card_outbreak(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;

  if (event == EVENT_MODIFY_COST)
  {
    new_default_test_definition(&this_test, 1, "Select a Swamp card.");
    this_test.zone = 0x100;
    this_test.subtype = 0x5009;
    result = check_battlefield_for_special_card(player, card, player, 0, &this_test);
    if (result != 0)
    {
      null_casting_cost(player, card);
      instance = get_card_instance(player, card);
      instance->info_slot = 1;
      return 0;
    }
    instance = get_card_instance(player, card);
    instance->info_slot = 0;
    return 0;
  }
  if (event == EVENT_CAN_CAST)
  {
    result = basic_spell(player, card, EVENT_CAN_CAST);
    return result;
  }
  if (event != EVENT_CAST_SPELL)
  {
    if (event != EVENT_RESOLVE_SPELL)
    {
      return 0;
    }
    default_test_definition(&this_test, 2);
    this_test.subtype = select_a_subtype(player, card);
    pump_creatures_until_eot(player, card, 2, 0, -1, -1, 0, 0, &this_test);
    kill_card(player, card, KILL_DESTROY);
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
  result = played_for_free(player, card);
  if (result != 0)
  {
    return 0;
  }
  result = is_token(player, card);
  if (result != 0)
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  if (instance->info_slot == 1)
  {

    result = get_id(player, card);
    result = has_mana_to_cast_id(player, EVENT_CAST_SPELL, result);
    if ((result == 0) || (result = do_dialog(player, player, card, -1, -1,
                                             " Discard a Swamp\n Play normally\n Cancel", 1),
                          result == 0))
    {

      new_default_test_definition(&this_test, 1, "Select a Swamp card.");
      this_test.zone = 0x100;
      this_test.subtype = 0x5009;
      result = new_select_a_card(player, player, 4, 0, 3, -1, &this_test);
      if (-1 < result)
      {
        discard_card(player, result);
        return 0;
      }
    }
    else if (result == 1)
    {
      result = get_id(player, card);
      charge_mana_from_id(player, card, EVENT_CAST_SPELL, result);
      return 0;
    }
    spell_fizzled = 1;
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25fb1a0; original cards/prophecy.c:3512. */
int destroy_unless_pay_2(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_RESOLVE_TRIGGER)
  {

    instance = get_card_instance(player, card);
    result = has_mana((int)instance->damage_target_player, 7, 2);
    if ((result == 0) ||
        (result = charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER,
                                              (int)instance->damage_target_player, COLOR_ANY, 2),
         result == 0))
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25f8dd0; original cards/prophecy.c:3535. */
void plague_fiend_effect(int player, int card, int t_player, int t_card)
{
  create_targetted_legacy_effect(player, card, destroy_unless_pay_2, t_player, t_card);
  return;
}

/* ManalinkEh 2022: 0x26054b0; original cards/prophecy.c:3538. */
int card_plague_fiend(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result == 0)
  {
    if (((event == EVENT_CHANGE_TYPE) && (card == affected_card)) &&
        (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      instance->destroys_if_blocked = instance->destroys_if_blocked | 0x10;
    }
    for_each_creature_damaged_by_me(player, card, event, 4, plague_fiend_effect, player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x2605b00; original cards/prophecy.c:3724. */
int card_rhystic_lightning(int player, int card, event_t event)
{
  bool bVar1;
  int result;
  unsigned int argument1;
  unsigned int uVar3;
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
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {

            bVar1 = true;
            uVar3 = 0;
            do
            {
              argument1 = current_turn ^ uVar3;
              result = has_mana(argument1, 7, 2);

              if ((((bVar1) && (result != 0)) &&
                   (result = do_dialog(argument1, player, card, -1, -1, " Pay 2 less damage\n Pass",
                                       (unsigned int)(player == argument1)),
                    result == 0)) &&
                  (charge_mana_while_resolving(player, card, EVENT_RESOLVE_SPELL, argument1,
                                               COLOR_COLORLESS, 2),
                   spell_fizzled != 1))
              {
                bVar1 = false;
              }
              uVar3 = uVar3 + 1;
            } while (uVar3 != 2);
            if (bVar1)
            {
              damage_target0(player, card, 4);
            }
            else
            {
              damage_target0(player, card, 2);
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_02605b5c;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
LAB_02605b5c:
  result = generic_spell(player, card, event, 1, &td, "TARGET_ANY", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x2605d30; original cards/prophecy.c:3765. */
int card_rhystic_scrying(int player, int card, event_t event)
{
  bool bVar1;
  int result;
  unsigned int argument1;
  unsigned int uVar3;

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
      if ((event != EVENT_RESOLVE_SPELL && event != EVENT_CAN_CHANGE_TARGET) &&
          (event != EVENT_CHANGE_TARGET))
      {
        return 0;
      }
      if (event == EVENT_RESOLVE_SPELL)
      {

        draw_cards(player, 3);

        bVar1 = false;
        uVar3 = 0;
        do
        {
          argument1 = current_turn ^ uVar3;
          result = has_mana(argument1, 7, 2);

          if (((result != 0) &&
               (result = do_dialog(argument1, player, card, -1, -1, " Pay 2 discard 3\n Pass",
                                   (unsigned int)(player == argument1)),
                result == 0)) &&
              (charge_mana_while_resolving(player, card, EVENT_RESOLVE_SPELL, argument1,
                                           COLOR_COLORLESS, 2),
               spell_fizzled != 1))
          {
            bVar1 = true;
          }
          uVar3 = uVar3 + 1;
        } while (uVar3 != 2);
        if (bVar1)
        {
          multidiscard(player, 3, 0);
        }
        kill_card(player, card, KILL_DESTROY);
      }
    }
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x2605ee0; original cards/prophecy.c:3798. */
int card_rhystic_shield(int player, int card, event_t event)
{
  bool bVar1;
  int iVar2;
  int iVar3;
  unsigned int argument1;
  unsigned int uVar4;

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

      uVar4 = 1;
      if ((ai_is_speculating != 1) && (uVar4 = 0, player == 1))
      {
        uVar4 = (unsigned int)(((unsigned char)trace_mode & 2) == 0);
      }
      if (current_phase == 0x18)
      {
        if (0 < active_cards_count[uVar4])
        {
          iVar2 = 0;
          do
          {
            iVar3 = is_unblocked(uVar4, iVar2);
            if ((iVar3 == 0) || (current_turn != uVar4))
            {
              ai_modifier = ai_modifier + 0x18;
            }
            else
            {
              ai_modifier = ai_modifier + -0x40;
            }
            iVar2 = iVar2 + 1;
          } while (iVar2 < active_cards_count[uVar4]);
        }
      }
      else
      {
        ai_modifier = ai_modifier + -0x10;
      }
    }
    else
    {
      if ((event != EVENT_CAN_CHANGE_TARGET && event != EVENT_RESOLVE_SPELL) &&
          (event != EVENT_CHANGE_TARGET))
      {
        return 0;
      }
      if (event == EVENT_RESOLVE_SPELL)
      {

        pump_subtype_until_eot(player, card, player, -1, 0, 1, 0, 0);

        bVar1 = true;
        uVar4 = 0;
        do
        {
          argument1 = current_turn ^ uVar4;
          iVar2 = has_mana(argument1, 7, 2);

          if ((((bVar1) && (iVar2 != 0)) &&
               (iVar2 = do_dialog(argument1, player, card, -1, -1, " Pay 2 or extra tough\n Pass",
                                  (unsigned int)(player == argument1)),
                iVar2 == 0)) &&
              (charge_mana_while_resolving(player, card, EVENT_RESOLVE_SPELL, argument1,
                                           COLOR_COLORLESS, 2),
               spell_fizzled != 1))
          {
            bVar1 = false;
          }
          uVar4 = uVar4 + 1;
        } while (uVar4 != 2);
        if (bVar1)
        {
          pump_subtype_until_eot(player, card, player, -1, 0, 2, 0, 0);
        }
        kill_card(player, card, KILL_DESTROY);
      }
    }
  }
  iVar2 = basic_spell(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x2606180; original cards/prophecy.c:3848. */
int card_rhystic_syphon(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int argument7;
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
        default_target_definition(player, card, &td, 0);
        td.preferred_controller = 1 - player;
        td.zone = 0x1000;
        instance = get_card_instance(player, card);
        if (event != EVENT_RESOLVE_SPELL)
          goto LAB_026061fb;
        iVar2 = valid_target(&td);
        if (iVar2 != 0)
        {

          iVar2 = instance->targets[0].player;
          iVar3 = has_mana(iVar2, 7, 3);
          if (iVar3 != 0)
          {

            if (ai_is_speculating == 1)
            {
              argument7 = 0;
            }
            else
            {
              argument7 = 1;
              if (iVar2 == 1)
              {
                argument7 = (unsigned int)trace_mode >> 1 & 1;
              }
            }
            iVar3 = do_dialog(iVar2, player, card, -1, -1, " Pay 3 or lose life\n Pass", argument7);
            if ((iVar3 == 0) && (charge_mana_while_resolving(player, card, EVENT_RESOLVE_SPELL,
                                                             iVar2, COLOR_COLORLESS, 3),
                                 spell_fizzled != 1))
              goto LAB_026062b7;
          }
          lose_life(iVar2, 5);
          gain_life(player, 5);
        }
      LAB_026062b7:
        kill_card(player, card, KILL_DESTROY);
        goto LAB_026061fb;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0);
  td.preferred_controller = 1 - player;
  td.zone = 0x1000;
  get_card_instance(player, card);
LAB_026061fb:
  iVar2 = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x25fb260; original cards/prophecy.c:4190. */
int gain_2_life_unless_they_pay_2(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_RESOLVE_TRIGGER)
  {

    instance = get_card_instance(player, card);
    result = has_mana((int)instance->damage_target_player, 7, 2);
    if ((result == 0) ||
        (result = charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER,
                                              (int)instance->damage_target_player, COLOR_ANY, 2),
         result == 0))
    {
      gain_life(player, 2);
    }
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x25f8e00; original cards/prophecy.c:4209. */
void soul_charmer_effect(int player, int card, int t_player, int t_card)
{
  create_targetted_legacy_effect(player, card, gain_2_life_unless_they_pay_2, t_player, t_card);
  return;
}

/* ManalinkEh 2022: 0x2606ce0; original cards/prophecy.c:4212. */
int card_soul_charmer(int player, int card, event_t event)
{
  int result;

  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result != 0)
  {
    return 0;
  }
  for_each_creature_damaged_by_me(player, card, event, 4, soul_charmer_effect, player, card);
  return 0;
}

/* ManalinkEh 2022: 0x2606d50; original cards/prophecy.c:4224. */
int card_soul_strings(int player, int card, event_t event)
{
  bool bVar1;
  int result;
  card_instance_t *instance;
  unsigned int uVar4;
  unsigned int argument1;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    result = count_graveyard_by_type(player, 2);
    if (result < 2)
    {
      return 0;
    }
    result = graveyard_has_shroud(player);
    return (unsigned int)(result == 0);
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      new_default_test_definition(&test, 2, "Select two target creature cards.");
      instance = get_card_instance(player, card);
      select_multiple_cards_from_graveyard(player, player, -1, 1, &test, 2, instance->targets);
    }
    goto LAB_02606d79;
  }
  if (event != EVENT_RESOLVE_SPELL)
    goto LAB_02606d79;

  instance = get_card_instance(player, card);
  bVar1 = true;
  uVar4 = 0;
  do
  {
    argument1 = current_turn ^ uVar4;
    result = has_mana(argument1, 7, instance->info_slot);

    if ((((result != 0) && (bVar1)) &&
         (result = do_dialog(argument1, player, card, -1, -1, " Pay x or cards back to hand\n Pass",
                             (unsigned int)(player == argument1)),
          result == 0)) &&
        (charge_mana_while_resolving(player, card, EVENT_RESOLVE_SPELL, argument1, COLOR_COLORLESS,
                                     instance->info_slot),
         spell_fizzled != 1))
    {
      kill_card(player, card, KILL_DESTROY);
      bVar1 = false;
    }
    uVar4 = uVar4 + 1;
  } while (uVar4 != 2);
  if (!bVar1)
    goto LAB_02606d79;

  result = validate_target_from_grave(player, card, player, 0);
  if (result == -1)
  {
    result = validate_target_from_grave(player, card, player, 1);
    if (result == -1)
    {
      spell_fizzled = 1;
    }
    else
    {
    LAB_02607030:
      from_grave_to_hand(player, result, 1);
    }
  }
  else
  {
    from_grave_to_hand(player, result, 1);
    result = validate_target_from_grave(player, card, player, 1);
    if (result != -1)
      goto LAB_02607030;
  }
  kill_card(player, card, KILL_DESTROY);
LAB_02606d79:
  result = generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x26085e0; original cards/prophecy.c:4881. */
int card_wing_storm(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  unsigned int t_player;
  test_definition_t this_test;

  if (event == EVENT_RESOLVE_SPELL)
  {

    uVar2 = 0;
    default_test_definition(&this_test, 2);
    this_test.keyword = 0x20;
    do
    {

      t_player = current_turn ^ uVar2;
      result = check_battlefield_for_special_card(player, card, t_player, 4, &this_test);
      if (result * 2 != 0)
      {
        damage_player(t_player, result * 2, player, card);
      }
      uVar2 = uVar2 + 1;
    } while (uVar2 != 2);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2600da0; original cards/prophecy.c:1854. */
int card_celestial_convergence(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;

  enters_the_battlefield_with_counters(player, card, event, COUNTER_OMEN, 7);
  upkeep_trigger_ability(player, card, event, player);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    remove_counters_from_source(player, card, player, card, COUNTER_OMEN, 1);
  }
  else if (event == EVENT_STATIC_EFFECTS)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {

      instance = get_card_instance(player, card);
      iVar2 = count_counters(player, card, COUNTER_OMEN);
      if (iVar2 < 1)
      {
        if (instance->info_slot == 0x12c7)
        {
          iVar2 = is_humiliated_for_ability(player, card, AL_STATIC);
          if (iVar2 == 0)
          {
            iVar2 = life[1 - player];
            iVar1 = life[player];
            if (iVar2 < iVar1)
            {
              lose_the_game(1 - player);
            }
            else if (iVar1 < iVar2)
            {
              lose_the_game(player);
            }
            else if (iVar1 == iVar2)
            {
              lose_the_game(2);
            }
          }
          else
          {
            instance->info_slot = 0;
          }
        }
      }
      else
      {
        instance->info_slot_fn = (CardFunction)0x12c7;
      }
    }
  }
  else if ((event == EVENT_SHOULD_AI_PLAY) &&
           (((ai_is_speculating == 1 ||
              ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))) &&
             (life[0] < life[1]))))
  {
    ai_modifier = ai_modifier + 0x18;
  }
  iVar2 = global_enchantment(player, card, event);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2605900; original cards/prophecy.c:3679. */
int card_rhystic_cave(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *instance;
  int result;
  unsigned int argument1;
  unsigned int uVar4;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      if ((((unsigned int)land_can_be_played >> 16 & 0xff) & 4) == 0)
      {
        result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
        return result;
      }
    }
    else
    {

      if ((ai_is_speculating == 1) ||
          ((result = 0, player == 1 && (((unsigned char)trace_mode & 2) == 0))))
      {
        result = get_deck_color(player, player);
      }
      add_state(player, card, 0x10);

      uVar4 = 0;
      result = choose_a_color(player, result);
      instance->info_slot = 1 << ((unsigned char)result & 0x1f);
      bVar1 = true;
      do
      {
        argument1 = current_turn ^ uVar4;
        result = has_mana(argument1, 7, 1);
        if ((bVar1) && (result != 0))
        {

          result = do_dialog(argument1, player, card, -1, -1, " Pay 1 stop mana\n Pass",
                             (unsigned int)(player == argument1));
          if (result == 0)
          {
            charge_mana_while_resolving(player, card, EVENT_ACTIVATE, argument1, COLOR_COLORLESS,
                                        1);
            if (spell_fizzled != 1)
            {
              tapped_for_mana_color = -1;
              bVar1 = false;
            }
          }
        }
        uVar4 = uVar4 + 1;
      } while (uVar4 != 2);
      if (bVar1)
      {
        produce_mana_all_one_color(player, instance->info_slot, 1);
      }
    }
  }
  else if (((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP)) ||
           ((event == EVENT_COUNT_MANA &&
             ((card == affected_card && (player == affected_card_controller))))))
  {

    get_card_instance(player, card);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2606b30; original cards/prophecy.c:4150. */
int card_shrouded_serpent(int player, int card, event_t event)
{
  int result;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if ((event == EVENT_DAT_ABILITY) &&
      (((result = has_mana(1 - current_turn, 7, 4),
         result == 0 || (result = do_dialog(1 - current_turn, player, card, -1, -1,
                                            " Pay 4 or this ublocked\n Decline", 0),
                         result != 0)) ||
        (result = charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, 1 - current_turn,
                                              COLOR_ANY, 4),
         result == 0))))
  {
    pump_ability_until_eot_no_repeat_alt(player, card, player, card, 0, 0x10, 1);
    return 0;
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2602e70; original cards/prophecy.c:2622. */
int card_forgotten_harvest(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  char *pcVar3;
  char *pcVar4;
  unsigned char bVar5;
  target_definition_t td;
  char msg[100];
  test_definition_t this_test;

  bVar5 = 0;
  result = duh_mode(player);
  upkeep_trigger_ability_mode(player, card, event, player,
                              (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {

    instance = get_card_instance(player, card);
    msg[0x1d] = '\0';
    pcVar3 = "Select a land card to exile.";
    pcVar4 = msg;
    for (result = 7; result != 0; result = result + -1)
    {
      *(uint32_t *)pcVar4 = *(uint32_t *)pcVar3;
      pcVar3 = pcVar3 + (unsigned int)bVar5 * -8 + 4;
      pcVar4 = pcVar4 + (unsigned int)bVar5 * -8 + 4;
    }
    *pcVar4 = *pcVar3;
    pcVar3 = msg + 0x20;
    for (result = 0x11; result != 0; result = result + -1)
    {
      pcVar3[0] = '\0';
      pcVar3[1] = '\0';
      pcVar3[2] = '\0';
      pcVar3[3] = '\0';
      pcVar3 = pcVar3 + (unsigned int)bVar5 * -8 + 4;
    }
    msg[0x1e] = '\0';
    msg[0x1f] = '\0';
    new_default_test_definition(&this_test, 1, msg);
    result = new_global_tutor(player, player, 2, 6, 0, 3, &this_test);
    if (-1 < result)
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      result = can_target(&td);
      if (result != 0)
      {
        result = pick_target(&td, "TARGET_CREATURE");
        if (result != 0)
        {
          add_counters_from_source(player, card, instance->targets[0].player,
                                   instance->targets[0].card, COUNTER_P1_P1, 1);
        }
      }
    }
    instance->number_of_targets = '\0';
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2604870; original cards/prophecy.c:3243. */
int card_mercenary_informer(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;
  target_definition_t td;

  special_abilities2(player, card, event, SP_KEYWORD2_PROT_MISC);
  if (event == EVENT_PROT_MISC)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (((unsigned int)event_result >> 24 & 0xff) == '\x05'))
    {
      uVar3 = get_color(attacking_card_controller, attacking_card);
      uVar4 = get_sleighted_color_test(player, card, 2);
      if ((uVar3 & uVar4) != 0)
      {
        event_result = event_result | 1;
      }
    }
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 0x1000;
    td.required_subtype = 0x73;
  }
  else
  {
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 0x1000;
    td.required_subtype = 0x73;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        put_on_bottom_of_deck(instance->targets[0].player, instance->targets[0].card);
      }
    }
  }
  result = generic_activated_ability(player, card, event, 6, 2, 0, 0, 0, 0, 1, 0, &td,
                                     "Select target nontoken Mercenary card.");
  return result;
}

/* ManalinkEh 2022: 0x2605770; original cards/prophecy.c:3648. */
int card_rebel_informer(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;
  target_definition_t td;

  special_abilities2(player, card, event, SP_KEYWORD2_PROT_MISC);
  if (event == EVENT_PROT_MISC)
  {
    if (((card == affected_card) && (player == affected_card_controller)) &&
        (((unsigned int)event_result >> 24 & 0xff) == '\x05'))
    {
      uVar3 = get_color(attacking_card_controller, attacking_card);
      uVar4 = get_sleighted_color_test(player, card, 0x20);
      if ((uVar3 & uVar4) != 0)
      {
        event_result = event_result | 1;
      }
    }
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 0x1000;
    td.required_subtype = 0x9b;
  }
  else
  {
    default_target_definition(player, card, &td, 0x47);
    td.illegal_type = 0x1000;
    td.required_subtype = 0x9b;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        put_on_bottom_of_deck(instance->targets[0].player, instance->targets[0].card);
      }
    }
  }
  result = generic_activated_ability(player, card, event, 6, 3, 0, 0, 0, 0, 0, 0, &td,
                                     "Select target nontoken Rebel card.");
  return result;
}
