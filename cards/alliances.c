#include "manalink.h"

static void al_lands(int player, int card, event_t event, int subtype, int untapped)
{
	if (event == EVENT_CAST_SPELL && affect_me(player, card) && IS_AI(player))
	{
		test_definition_t test;
		new_default_test_definition(&test, TYPE_LAND, "");
		if (untapped)
		{
			test.state = STATE_TAPPED;
			test.state_flag = DOESNT_MATCH;
			}
		test.subtype = subtype;
		test.not_me = 1;

		if( !check_battlefield_for_special_card(player, card, player, CBFSC_CHECK_ONLY, &test) ){
			ai_modifier -= 512;
		}
		else{
			mana_producer(player, card, event);
		}
	}

	if (event == EVENT_RESOLVE_SPELL)
	{
		play_land_sound_effect(player, card);

		test_definition_t test;
		new_default_test_definition(&test, TYPE_LAND,
									get_hacked_land_text(-1, -1,
													   untapped ? "Select an untapped %s to sacrifice."
													   : "Select %a to sacrifice.",
													   subtype));
		if (untapped)
		{
			test.state = STATE_TAPPED;
			test.state_flag = DOESNT_MATCH;
		}
		test.subtype = subtype;

		if (!new_sacrifice(player, card, player, SAC_NO_CANCEL, &test))
			kill_card(player, card, KILL_STATE_BASED_ACTION);

	}

}

int card_multi_purpose_al_token(int player, int card, event_t event){
	// 66 --> Graveborn (Kjeldoran Dead)
	// 67 --> Survivor (Kjeldoran Home Guard)
	// 68 --> Hippo (Pheldagriff)
	// 69 --> Starfish (Spiny Starfish)

	if( get_special_infos(player, card) == 66 || get_special_infos(player, card) == 70){
		haste(player, card, event);
	}

	if( event == EVENT_SET_COLOR && affect_me(player, card) ){
		if( get_special_infos(player, card) == 66 || get_special_infos(player, card) == 70 ){
			event_result = COLOR_TEST_RED;
			event_result |= COLOR_TEST_BLACK;
		}
		else if( get_special_infos(player, card) == 67 ){
			event_result = COLOR_TEST_WHITE;
		}
		else if( get_special_infos(player, card) == 68 ){
			event_result = COLOR_TEST_GREEN;
		}
		else if( get_special_infos(player, card) == 69 ){
			event_result = COLOR_TEST_BLUE;
		}
	}
	else if( event == EVENT_POWER && affect_me(player, card) ){
		if( get_special_infos(player, card) == 68 ){
			event_result++;
		}
		else if( get_special_infos(player, card) == 66 || get_special_infos(player, card) == 70){
			event_result+=3;
		}
	}

	else if( get_special_infos(player, card) == 66 && eot_trigger(player, card, event) ){
		kill_card(player, card, KILL_SACRIFICE);
	}


	return 0;
}

void al_pitchspell(int player, int card, event_t event, int clr, int life_to_pay){
	if( event == EVENT_MODIFY_COST ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_ANY);
		this_test.color = 1<<clr;
		this_test.zone = TARGET_ZONE_HAND;
		card_instance_t *instance= get_card_instance(player, card);
		if( check_battlefield_for_special_card(player, card, player, CBFSC_GET_COUNT, &this_test) > 1 ){
			int good = 1;
			int c1 = get_updated_casting_cost(player, card, -1, event, 0);
			if( ! has_mana(player, COLOR_COLORLESS, c1) ){
				good = 0;
			}
			if( good && life_to_pay > 0 && ! can_pay_life(player, life_to_pay) ){
				good = 0;
			}
			if( good == 1 ){
				null_casting_cost(player, card);
				instance->info_slot = 1;
			}
			else{
				instance->info_slot = 0;
			}
		}
		else{
			instance->info_slot = 0;
		}
	}
}

int casting_al_pitchspell(int player, int card, event_t event, int clr, int life_to_pay){
	int result = 1;
	if( ! played_for_free(player, card) && ! is_token(player, card) && get_card_instance(player, card)->info_slot == 1 ){
		result = 0;

		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_ANY);
		this_test.color = 1<<clr;
		this_test.zone = TARGET_ZONE_HAND;

		card_instance_t *instance= get_card_instance(player, card);

		enum{
			CHOICE_PITCH = 1,
			CHOICE_PLAY_NORMALLY
		};

		int c1 = get_updated_casting_cost(player, card, -1, event, 0);
		int can_pitch = ( check_battlefield_for_special_card(player, card, player, CBFSC_CHECK_ONLY, &this_test) &&
						has_mana(player, COLOR_ANY, c1) )|| can_pay_life(player, life_to_pay) ? 1 : 0;
		int can_play_normally = has_mana_to_cast_iid(player, event, instance->internal_card_id);

		int choice = can_pitch ? CHOICE_PITCH : CHOICE_PLAY_NORMALLY;
		if( can_pitch && can_play_normally ){
			char buffer2[100];
			int pos = scnprintf(buffer2, 100, get_sleighted_color_text(player, card, "Exile a %s card", clr));
			if( life_to_pay ){
				scnprintf(buffer2 + pos, 100-pos, " & pay %d life", life_to_pay);
			}
			choice = DIALOG(player, card, event, DLG_NO_STORAGE, DLG_RANDOM,
							buffer2, 			1, life[player]-life_to_pay > 6 ? 10 : 1,
							"Play normally", 	1, 5);
			if( ! choice ){
				spell_fizzled = 1;
				return 0;
			}
		}

		if( choice == CHOICE_PITCH ){
			charge_mana(player, COLOR_ANY, c1);
			if( spell_fizzled != 1 ){
				state_untargettable(player, card, 1);
				scnprintf(this_test.message, 100, "%s", get_sleighted_color_text(player, card, "Select a %s card to exile.", clr));
				int selected = new_select_a_card(player, player, TUTOR_FROM_HAND, 0, AI_MIN_CMC, -1, &this_test);
				if( selected != -1 ){
					rfg_card_in_hand(player, selected);
					state_untargettable(player, card, 0);
					if( life_to_pay > 0 ){
						lose_life(player, 1);
					}
					return 1;
				}
				else{
					state_untargettable(player, card, 0);
					return 0;
				}
			}
		}

		if( choice == CHOICE_PLAY_NORMALLY ){
			charge_mana_from_id(player, card, event, get_id(player, card));
			if( spell_fizzled != 1 ){
				return 1;
			}
		}
	}
	return result;
}

static int ad_cantrip(int player, int card, event_t event){

	card_instance_t *instance = get_card_instance(player, card);

	if( upkeep_trigger(player, card, event) ){
		int ai_choice = do_dialog(instance->targets[1].player, player, card, -1, -1, " Draw 2 cards\n Draw 1 card\n Pass", 0);
		if( 1-player == AI && count_deck(1-player) < 5 ){
			ai_choice = 2;
		}
		else if( 1-player == AI && count_deck(1-player) < 10 ){
			ai_choice = 1;
		}
		
		if( ai_choice < 2 ){
			draw_cards(instance->targets[1].player, 2-ai_choice);
		}

		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

// Cards

int card_arcane_denial(int player, int card, event_t event){
/* CARD_ID_ARCANE_DENIAL	2255
Arcane Denial	|1|U
Instant
Counter target spell. Its controller may draw up to two cards at the beginning of the next turn's upkeep.
You draw a card at the beginning of the next turn's upkeep. */

	if( event == EVENT_RESOLVE_SPELL){
		card_instance_t *instance = get_card_instance(player, card);
		cantrip(player, card, 1);
		int legacy2 = create_legacy_effect(player, card, &ad_cantrip );
		card_instance_t *leg2 = get_card_instance(player, legacy2);
		leg2->targets[1].player = instance->targets[0].player;
		return counterspell(player, card, event, NULL, 0);
	}

	return generic_spell(player, card, event, GS_COUNTERSPELL, NULL, NULL, 0, NULL);
}

int card_ashnods_cylix(int player, int card, event_t event)
{
/*
Ashnod's Cylix |2
Artifact
{3}, {T}: Target player looks at the top three cards of his or her library, puts one of them back on top of his or her library,
	then exiles the rest.
*/
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	if (event == EVENT_RESOLVE_ACTIVATION && valid_target(&td)){
		card_instance_t* instance = get_card_instance(player, card);

		int max = MIN(3, count_deck(instance->targets[0].player));
		if( max ){
			test_definition_t test;
			new_default_test_definition(&test, 0, "Select a card to put on top.");
			test.create_minideck = max;
			test.no_shuffle = 1;
			int selected = new_select_a_card(instance->targets[0].player, instance->targets[0].player, TUTOR_FROM_DECK, 1, AI_MAX_VALUE, -1, &test);
			int card_added = deck_ptr[instance->targets[0].player][selected];
			remove_card_from_deck(instance->targets[0].player, selected);
			max--;
			if( max ){
				int i;
				for(i=0; i<max; i++){
					rfg_top_card_of_deck(instance->targets[0].player);
				}
			}
			raw_put_iid_on_top_of_deck(instance->targets[0].player, card_added);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET, MANACOST_X(3), 0, &td, "TARGET_PLAYER");
}

int card_balduvian_dead(int player, int card, event_t event){
/* CARD_ID_BALDUVIAN_DEAD	2259
Balduvian Dead	|3|B
Creature - Zombie 2/3
|2|R, Exile a creature card from your graveyard: Create a 3/1 |Sblack and |Sred Graveborn creature token with haste.
Sacrifice it at the beginning of the next end step. */

	if( event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability(player, card, event, 0, MANACOST_XR(2, 1), 0, NULL, NULL) ){
			return count_graveyard_by_type(player, TYPE_CREATURE) > 0;
		}
	}

	if( event == EVENT_ACTIVATE){
		if( charge_mana_for_activated_ability(player, card, MANACOST_XR(2, 1)) ){
			test_definition_t this_test;
			new_default_test_definition(&this_test, TYPE_CREATURE, "Select a creature card to exile.");
			if( new_global_tutor(player, player, TUTOR_FROM_GRAVE, TUTOR_RFG, 0, AI_MIN_VALUE, &this_test) == -1 ){
				spell_fizzled = 1;
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		token_generation_t token;
		default_token_definition(player, card, CARD_ID_GRAVEBORN, &token);
		token.pow = 3;
		token.tou = 1;
		token.special_code_on_generation = &haste_and_sacrifice_token_at_end_of_next_turn;
		generate_token(&token);
	}

	return 0;
}

int card_balduvian_horde(int player, int card, event_t event){
/* CARD_ID_BALDUVIAN_HORDE	1686
Balduvian Horde	|2|R|R
Creature - Human Barbarian 5/5
When ~ enters the battlefield, sacrifice it unless you discard a card at random. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		int did_discard = 0;
		if(hand_count[player] > 0){
			char buffer[50];
			load_text(0, "BALDUVIAN_HORDE");
			snprintf(buffer, 50, " %s\n %s", text_lines[0], text_lines[1]);
			int choice = do_dialog(player, player, card, -1, -1, buffer, 0);
			if(choice == 0){
				discard(player, DISC_RANDOM, player);
				did_discard = 1;
			}
		}
		if(!did_discard){
			kill_card(player, card, KILL_SACRIFICE);
		}
	}

	return 0;
}

int card_balduvian_trading_post(int player, int card, event_t event){
/* CARD_ID_BALDUVIAN_TRADING_POST	2260
Balduvian Trading Post	""
Land
If ~ would enter the battlefield, sacrifice an untapped |H2Mountain instead. If you do, put ~ onto the battlefield. If you don't, put it into its owner's graveyard.
|T: Add |C|R to your mana pool.
|1, |T: ~ deals 1 damage to target attacking creature. */

	al_lands(player, card, event, SUBTYPE_MOUNTAIN, TARGET_STATE_TAPPED);

	if (event == EVENT_COUNT_MANA){
		if (affect_me(player, card) && mana_producer(player, card, EVENT_CAN_ACTIVATE) ){
			declare_mana_available(player, COLOR_COLORLESS, 1);
			declare_mana_available(player, COLOR_RED, 1);
		}
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.required_state = TARGET_STATE_ATTACKING;

	card_instance_t* instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE){
		return mana_producer(player, card, event);
	}

	if (event == EVENT_ACTIVATE){
		int choice = instance->number_of_targets = instance->info_slot = 0;
		int can_damage = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST_X(1), 0,
													&td, NULL);
		if( !paying_mana() && can_damage){
			choice = do_dialog(player, player, card, -1, -1, " Produce mana\n Damage an attacker\n Cancel", 1);
		}
		instance->info_slot = choice;

		if( choice == 0){
			produce_mana_tapped2(player, card, COLOR_COLORLESS, 1, COLOR_RED, 1);
		}

		if( choice == 1 ){
			add_state(player, card, STATE_TAPPED);
			generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST_X(1), 0,
										&td, "Select target attacking creature.");
			if( spell_fizzled == 1 ){
				untap_card_no_event(player, card);
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == 1 && valid_target(&td) ){
			damage_target0(player, card, 1);
		}
	}

	return 0;
}

int card_balduvian_war_makers(int player, int card, event_t event)
{
  // 0x1202759

  /* Balduvian War-Makers	|4|R
   * Creature - Human Barbarian 3/3
   * Haste
   * Rampage 1 */

  haste(player, card, event);
  rampage(player, card, event, 1);
  return 0;
}

int card_bestial_fury(int player, int card, event_t event){

	/* Bestial Fury	|2|R
	 * Enchantment - Aura
	 * Enchant creature
	 * When ~ enters the battlefield, draw a card at the beginning of the next turn's upkeep.
	 * Whenever enchanted creature becomes blocked, it gets +4/+0 and gains trample until end of turn. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		cantrip(player, card, 1);
	}

	if( in_play(player, card) && get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);
		if( event == EVENT_ATTACK_RATING && affect_me(instance->damage_target_player, instance->damage_target_card) ){
			ai_defensive_modifier -= 48;
		}
		if( event == EVENT_DECLARE_BLOCKERS && is_attacking(instance->damage_target_player, instance->damage_target_card) &&
			! is_unblocked(instance->damage_target_player, instance->damage_target_card)
		  ){
			pump_ability_until_eot(player, card, instance->damage_target_player, instance->damage_target_card, 4, 0, KEYWORD_TRAMPLE, 0);
		}
	}

	if( ! IS_AURA_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	return targeted_aura(player, card, event, &td, "TARGET_CREATURE");
}

static int effect_hunt(int player, int card, event_t event){

	if( eot_trigger(player, card, event) ){
		card_instance_t *instance = get_card_instance(player, card);
		remove_1_1_counter(instance->damage_target_player, instance->damage_target_card);
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int card_bounty_of_the_hunt(int player, int card, event_t event){
/* CARD_ID_BOUNTY_OF_THE_HUNT	2263
Bounty of the Hunt	|3|G|G
Instant
You may exile a |Sgreen card from your hand rather than pay ~'s mana cost.
Distribute three +1/+1 counters among one, two, or three target creatures.
For each +1/+1 counter you put on a creature this way, remove a +1/+1 counter from that creature at the beginning of the next cleanup step. */

	al_pitchspell(player, card, event, COLOR_GREEN, 0);

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);
	td1.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);


	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td1, "TARGET_CREATURE", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		int x;
		for(x = 0; x < 3; x++){
			// Maybe the only creature on the bf is a Maro.
			if (can_target(&td1) && new_pick_target(&td1, "TARGET_CREATURE", x, 1)){
				add_state(instance->targets[x].player, instance->targets[x].card, STATE_TARGETTED);
			}
		}
		for(x = 0; x < 3; x++){
			if (instance->targets[x].player != -1 && instance->targets[x].card != -1){
				remove_state(instance->targets[x].player, instance->targets[x].card, STATE_TARGETTED);
			}
		}
		if( ! casting_al_pitchspell(player, card, event, COLOR_GREEN, 0) ){
			spell_fizzled = 1;
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int x;
		for(x = 0; x < 3; x++){
			if( validate_target(player, card, &td1, x)){
				add_1_1_counter(instance->targets[x].player, instance->targets[x].card);
				create_targetted_legacy_effect(player, card, &effect_hunt, instance->targets[x].player, instance->targets[x].card);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_browse(int player, int card, event_t event){
/* CARD_ID_BROWSE	2264
Browse	|2|U|U
Enchantment
|2|U|U: Look at the top five cards of your library, put one of them into your hand, and exile the rest. */

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int number = MIN(5, count_deck(player));
		if( number ){
			    test_definition_t test;
			    new_default_test_definition(&test, TYPE_ANY, "Select a card to add to your hand.");
			    test.create_minideck = number;
			    test.no_shuffle = 1;

			    if( new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 1, AI_MAX_VALUE, &test) != -1 ){
				    number--;
			    }
			    if( number ){
				   int i;
				   for(i=0; i<number; i++ ){
					   rfg_top_card_of_deck(player);
				   }
			    }
		}
	}

	return generic_activated_ability(player, card, event, 0, MANACOST_XU(2, 2), 0, NULL, NULL);
}

int card_chaos_harlequin(int player, int card, event_t event)
{
/* CARD_ID_CHAOS_HARLEQUIN	2268
Chaos Harlequin	|2|R|R
Creature - Human 2/4
|R: Exile the top card of your library. If that card is a land card, ~ gets -4/-0 until end of turn. Otherwise, ~ gets +2/+0 until end of turn. */

  if (event == EVENT_RESOLVE_ACTIVATION)
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  int pp = instance->parent_controller;
	  int pc = instance->parent_card;
	  if (in_play(pp, pc))
		{
		  int is_land = deck_ptr[pp][0] != -1 && is_what(-1, deck_ptr[pp][0], TYPE_LAND);

		  if (deck_ptr[pp][0] != -1)
			rfg_top_card_of_deck(pp);

		  if (is_land)
			alternate_legacy_text(1, pp, pump_until_eot_merge_previous(pp, pc, pp, pc, -4,0));
		  else
			alternate_legacy_text(2, pp, pump_until_eot_merge_previous(pp, pc, pp, pc, 2,0));
		}
	  else if (deck_ptr[pp][0] != -1)
		rfg_top_card_of_deck(pp);
	}

  if (event == EVENT_POW_BOOST)
	return has_mana(player, COLOR_RED, 1);	// Assume it gets +1/+0 on average.

  return generic_activated_ability(player, card, event, 0, MANACOST_R(1), 0, NULL, NULL);
}

int card_contagion(int player, int card, event_t event){

	/* Contagion	|3|B|B
	 * Instant
	 * You may pay 1 life and exile a |Sblack card from your hand rather than pay ~'s mana cost.
	 * Distribute two -2/-1 counters among one or two target creatures. */

	al_pitchspell(player, card, event, COLOR_BLACK, 1);

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td1, "TARGET_CREATURE", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		int x;
		for(x = 0; x < 2; x++){
			// Maybe the only creature on the bf is a Maro.
			if (can_target(&td1) && new_pick_target(&td1, "TARGET_CREATURE", x, 1)){
				add_state(instance->targets[x].player, instance->targets[x].card, STATE_TARGETTED);
			}
		}
		for(x = 0; x < 2; x++){
			if (instance->targets[x].player != -1 && instance->targets[x].card != -1){
				remove_state(instance->targets[x].player, instance->targets[x].card, STATE_TARGETTED);
			}
		}
		if( spell_fizzled != 1 ){
			if( casting_al_pitchspell(player, card, event, COLOR_BLACK, 1) == 0 ){
				spell_fizzled = 1;
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int x;
		for(x = 0; x < 2; x++){
			if( validate_target(player, card, &td1, x)){
				add_counter(instance->targets[x].player, instance->targets[x].card, COUNTER_M2_M1);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_death_spark(int player, int card, event_t event){
	/* Death Spark	|R	0x2002ec9
	 * Instant
	 * ~ deals 1 damage to any target.
	 * At the beginning of your upkeep, if ~ is in your graveyard with a creature card directly above it, you may pay |1. If you do, return ~ to your hand. */
	if( event == EVENT_GRAVEYARD_ABILITY_UPKEEP && has_mana(player, COLOR_COLORLESS, 1) ){
		int choice = do_dialog(player, player, card, -1, -1," Return Death Spark to your hand\n Decline", 0);
		if( choice == 0 ){
			charge_mana(player, COLOR_COLORLESS, 1);
			if( spell_fizzled != 1){
				remove_state(player, card, STATE_INVISIBLE);
				hand_count[player]++;
				return -1;
			}
		}
		return -2;
	}

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_target0(player, card, 1);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ANY", 1, NULL);
}

int card_dimishing_returns(int player, int card, event_t event){
/* CARD_ID_DIMINISHING_RETURNS	2271
Diminishing Returns	|2|U|U
Sorcery
Each player shuffles his or her hand and graveyard into his or her library. You exile the top ten cards of your library. Then each player draws up to seven cards. */

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		if( hand_count[player] + count_graveyard(player) + count_deck(player) < 10 ){
			ai_modifier-=200;
		}
		else{
			ai_modifier+=(140-(hand_count[player]*20));
			ai_modifier+=((count_subtype(player, TYPE_LAND, -1)*10)-60);
			ai_modifier+=((hand_count[1-player]*20)-140);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		APNAP(p,{
					int count = active_cards_count[p]-1;
					while( count > -1 ){
							if( in_hand(p, count) ){
								put_on_top_of_deck(p, count);
							}
							count--;
					}
					reshuffle_grave_into_deck(p, 0);
				};
		);
		int x;
		for(x = 0; x < 10; x++){
			rfg_top_card_of_deck(player);
		}
		APNAP(p,{draw_some_cards_if_you_want(player, card, p, 7);};);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_diseased_vermin(int player, int card, event_t event){
/*
Diseased Vermin	|2|B
Creature — Rat 1/1
Whenever Diseased Vermin deals combat damage to a player, put an infection counter on it.
At the beginning of your upkeep, Diseased Vermin deals X damage to target opponent previously dealt damage by it,
where X is the number of infection counters on it.
*/

	if( trigger_condition == TRIGGER_UPKEEP && affect_me(player, card) ){
		target_definition_t td;
		default_target_definition(player, card, &td, 0);
		td.zone = TARGET_ZONE_PLAYERS;

		int will_damage = would_validate_arbitrary_target(&td, 1-player, -1) && count_counters(player, card, COUNTER_INFECTION) &&
							get_card_instance(player, card)->targets[0].player == 1-player ? 1 : 0;

		upkeep_trigger_ability_mode(player, card, event, player, will_damage ? RESOLVE_TRIGGER_MANDATORY : 0);
	}

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, 0);
		td.zone = TARGET_ZONE_PLAYERS;

		int will_damage = would_validate_arbitrary_target(&td, 1-player, -1) && count_counters(player, card, COUNTER_INFECTION) &&
							get_card_instance(player, card)->targets[0].player == 1-player ? 1 : 0;

		if( will_damage ){
			damage_player(1-player, count_counters(player, card, COUNTER_INFECTION), player, card);
		}
	}

	if( damage_dealt_by_me(player, card, event, DDBM_MUST_BE_COMBAT_DAMAGE | DDBM_MUST_DAMAGE_PLAYER | DDBM_TRACE_DAMAGED_PLAYERS) )
	{
		card_instance_t* instance = get_card_instance(player, card);
		int times_damaged[2] = { BYTE0(instance->targets[1].player), BYTE1(instance->targets[1].player) };
		instance->targets[1].player = 0;
		if( times_damaged[1-player] ){
			instance->targets[0].player = 1-player;
		}
		add_counter(player, card, COUNTER_INFECTION);
	}

	return 0;
}

int card_elvish_spirit_guide(int player, int card, event_t event){
	/* Elvish Spirit Guide	|2|G
	 * Creature - Elf Spirit 2/2
	 * Exile ~ from your hand: Add |G to your mana pool. */

	if( event == EVENT_CAN_ACTIVATE_FROM_HAND ){
		return 1;
	}

	if( event == EVENT_ACTIVATE_FROM_HAND ){
		kill_card(player, card, KILL_EXILE);
		return 1;
	}

	if( event == EVENT_RESOLVE_ACTIVATION_FROM_HAND ){
		produce_mana(player, COLOR_GREEN, 1);
	}

	return 0;
}

int card_energy_arc(int player, int card, event_t event){
/* CARD_ID_ENERGY_ARC	2276
Energy Arc	|W|U
Instant
Untap any number of target creatures. Prevent all combat damage that would be dealt to and dealt by those creatures this turn. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	if( player == AI ){
		td.preferred_controller = current_turn == player ? player : 1-player;
		td.required_state = current_turn == 1-player ? TARGET_STATE_ATTACKING : 0;
	}

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST  ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		int trgs = 0;
		while( can_target(&td) ){
				if( new_pick_target(&td, "TARGET_CREATURE", trgs, 0) ){
					state_untargettable(instance->targets[trgs].player, instance->targets[trgs].card, 1);
					trgs++;
				}
				else{
					break;
				}
		}
		if( trgs > 0 ){
			int i;
			for(i=0; i<trgs; i++){
				state_untargettable(instance->targets[i].player, instance->targets[i].card, 0);
			}
		}
	}

	if(event == EVENT_RESOLVE_SPELL ){
		int i;
		for(i=0; i<instance->number_of_targets; i++){
			if( validate_target(player, card, &td, i) ){
				untap_card(instance->targets[i].player, instance->targets[i].card);
				maze_of_ith_effect(player, card, instance->targets[i].player, instance->targets[i].card);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_exile(int player, int card, event_t event){
/* CARD_ID_EXILE	2279
Exile	|2|W
Instant
~ target non|Swhite attacking creature. You gain life equal to its toughness. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.illegal_color = get_sleighted_color_test(player, card, COLOR_TEST_WHITE);
	td.required_state = TARGET_STATE_ATTACKING;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			gain_life(player, get_toughness(instance->targets[0].player, instance->targets[0].card));
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT,
						&td, get_sleighted_color_text(player, card, "Select target non%s attacking creature.", COLOR_WHITE), 1, NULL);
}

int card_fatal_lore(int player, int card, event_t event){
/*
Fatal Lore |2|B|B
Sorcery
An opponent chooses one —
• You draw three cards.
• You destroy up to two target creatures that player controls. They can't be regenerated. That player draws up to three cards.
*/
	if( event == EVENT_RESOLVE_SPELL ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.allowed_controller = td.preferred_controller = 1-player;

		enum{
			CHOICE_DRAW_3 = 1,
			CHOICE_KILL_2
		};

		int priority_draw = hand_count[player]*5;
		int priority_kill = count_subtype(player, TYPE_CREATURE, -1)*5;
		if( ! can_target(&td) ){
			priority_draw = 0;
			priority_kill = 20;
		}

		int choice = DIALOG(player, card, EVENT_ACTIVATE, DLG_RANDOM, DLG_NO_STORAGE, DLG_NO_CANCEL, DLG_WHO_CHOOSES(1-player),
							"Opponent draws 3 cards", 1, priority_draw,
							"Opp. kills and you draw 3", 1, priority_kill);

		if( choice == CHOICE_DRAW_3 ){
			draw_cards(player, 3);
		}
		if( choice == CHOICE_KILL_2 ){
			int to_kill[2] = {-1, -1};
			int i;
			for(i=0; i<2; i++){
				if( new_pick_target(&td, "Select target creature your opponent controls.", 0, GS_LITERAL_PROMPT) ){
					to_kill[i] = get_card_instance(player, card)->targets[0].card;
					state_untargettable(1-player, to_kill[i], 1);
				}
			}
			for(i=0; i<2; i++){
				state_untargettable(1-player, to_kill[i], 0);
				kill_card(1-player, to_kill[i], KILL_SACRIFICE);
			}
			get_card_instance(player, card)->number_of_targets = 0;
			draw_some_cards_if_you_want(player, card, player, 3);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_feast_or_famine(int player, int card, event_t event){
	/* Feast or Famine	|3|B
	 * Instant
	 * Choose one -
	 * * Put a 2/2 |Sblack Zombie creature token onto the battlefield.
	 * * Destroy target nonartifact, non|Sblack creature. It can't be regenerated. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	enum{
		CHOICE_ZOMBIE = 1,
		CHOICE_TERROR
	};

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.illegal_type = TYPE_ARTIFACT;
	td.illegal_color = get_sleighted_color_test(player, card, COLOR_TEST_BLACK);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		if( ! check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
			instance->info_slot = 0;
			int priority_zombie = 15-(count_subtype(player, TYPE_CREATURE, -1)*5);
			int choice = DIALOG(player, card, EVENT_ACTIVATE, DLG_RANDOM,
								"Generate a Zombie", 1, priority_zombie,
								"Terror", can_target(&td), 10);
			if( ! choice ){
				spell_fizzled = 1;
				return 0;
			}
		}
		if( instance->info_slot == CHOICE_TERROR ){
			new_pick_target(&td, get_sleighted_color_text(player, card, "Select target non%s, nonartifact creature.", COLOR_BLACK),
							0, 1 | GS_LITERAL_PROMPT);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( instance->info_slot == CHOICE_ZOMBIE ){
			generate_token_by_id(player, card, CARD_ID_ZOMBIE);
		}
		if( instance->info_slot == CHOICE_TERROR && valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_force_of_will(int player, int card, event_t event){
/* CARD_ID_FORCE_OF_WILL	2285
Force of Will	|3|U|U
Instant
You may pay 1 life and exile a |Sblue card from your hand rather than pay ~'s mana cost.
Counter target spell. */

	al_pitchspell(player, card, event, COLOR_BLUE, 1);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_COUNTERSPELL, NULL, NULL, 0, NULL);
	}

	if(event == EVENT_CAST_SPELL && affect_me(player, card) ){
		if( casting_al_pitchspell(player, card, event, COLOR_BLUE, 1) ){
			return counterspell(player, card, event, NULL, 0);
		}
		else{
			spell_fizzled = 1;
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		return counterspell(player, card, event, NULL, 0);
	}

	return 0;
}

int card_gargantuan_gorilla(int player, int card, event_t event){
	/* Gargantuan Gorilla	|4|G|G|G
	 * Creature - Ape 7/7
	 * At the beginning of your upkeep, you may sacrifice |Ha Forest.
	 * If you sacrifice a snow |H2Forest this way, ~ gains trample until end of turn.
	 * If you don't sacrifice |Ha Forest, sacrifice ~ and it deals 7 damage to you.
	 * |T: ~ deals damage equal to its power to another target creature. That creature deals damage equal to its power to ~. */


	card_instance_t *instance = get_card_instance(player, card);

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_LAND, get_hacked_land_text(player, card, "Select a %s to sacrifice.", SUBTYPE_FOREST));
		test.subtype = get_hacked_subtype(player, card, SUBTYPE_FOREST);

		test_definition_t test2;
		default_test_definition(&test2, TYPE_LAND);
		test2.subtype = get_hacked_subtype(player, card, SUBTYPE_FOREST);
		test2.sub2 = SUBTYPE_SNOW;
		test2.subtype_flag = F2_MULTISUBTYPE_ALL;

		int kill = 1;
		if( new_can_sacrifice(player, card, player, &test) ){
			int sac = 0;
			if( player == AI ){
				if( new_can_sacrifice(player, card, player, &test2) ){
					sac = new_sacrifice(player, card, player, SAC_JUST_MARK | SAC_RETURN_CHOICE, &test2);
				}
			}
			if( ! sac ){
				sac = new_sacrifice(player, card, player, SAC_JUST_MARK | SAC_RETURN_CHOICE, &test);
			}
			if( sac ){
				if( is_snow_permanent(player, BYTE3(sac)) ){
					pump_ability_until_eot(player, card, player, card, 0, 0, KEYWORD_TRAMPLE, 0);
				}
				kill_card(player, BYTE3(sac), KILL_SACRIFICE);
				kill = 0;
			}
		}

		if( kill ){
			damage_player(player, 7, player, card);
			kill_card(player, card, KILL_SACRIFICE);
		}
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			// Not a fight.  Stop replacing this with fight().
			get_card_instance(instance->parent_controller, instance->parent_card)->regen_status |= KEYWORD_RECALC_POWER;
			int mypow = get_power(instance->parent_controller, instance->parent_card);
			damage_creature(instance->targets[0].player, instance->targets[0].card, mypow, instance->parent_controller, instance->parent_card);
			if (in_play(instance->parent_controller, instance->parent_card)){
				get_card_instance(instance->targets[0].player, instance->targets[0].card)->regen_status |= KEYWORD_RECALC_POWER;
				int hispow = get_power(instance->targets[0].player, instance->targets[0].card);
				damage_creature(instance->parent_controller, instance->parent_card, hispow, instance->targets[0].player, instance->targets[0].card);
			}
       }
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST0, 0, &td, "TARGET_CREATURE");
}

int card_gorilla_chieftain(int player, int card, event_t event){
/* CARD_ID_GORILLA_CHIEFTAIN	2291
Gorilla Chieftain	|2|G|G
Creature - Ape 3/3
|1|G: Regenerate ~. */

	return regeneration(player, card, event, MANACOST_XG(1, 1));
}

static int target_for_gorilla_shaman(int player, int card, int mode){
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT );
	td.illegal_type = TYPE_CREATURE;

	card_instance_t *instance= get_card_instance(player, card);

	int i;
	int trg = -1;
	int par = -1;
	for(i=0; i<2; i++){
		if( i == 1-player || player == HUMAN ){
			int count = 0;
			while( count < active_cards_count[i] ){
					if( in_play(i, count) && is_what(i, count, TYPE_ARTIFACT) && ! is_what(i, count, TYPE_CREATURE) ){
						int cost = (get_cmc(i, count)*2)+1;
						if( has_mana_for_activated_ability(player, card, MANACOST_X(cost)) ){
							instance->targets[0].player = i;
							instance->targets[0].card = count;
							instance->number_of_targets = 1;
							if( would_validate_target(player, card, &td, 0) ){
								if( mode == 0 || player == HUMAN ){
									return 1;
								}
								if( mode == 1 && get_base_value(i, count) > par ){
									par = get_base_value(i, count);
									trg = count;
								}
							}
						}
					}
					count++;
			}
		}
	}
	if( mode == 1 ){
		return trg;
	}
	return 0;
}

int card_gorilla_shaman(int player, int card, event_t event){
/* CARD_ID_GORILLA_SHAMAN	2292
Gorilla Shaman	|R
Creature - Ape Shaman 1/1
|X|X|1: Destroy target noncreature artifact with converted mana cost X. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT );
	td.illegal_type = TYPE_CREATURE;

	card_instance_t *instance= get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE && generic_activated_ability(player, card, event, 0, MANACOST0, 0, NULL, NULL) ){
		return target_for_gorilla_shaman(player, card, 0);
	}

	if( event == EVENT_ACTIVATE ){
		instance->number_of_targets = 0;
		int op = -1;
		int oc = -1;
		if( player == HUMAN ){
			if( select_target(player, card, &td, "Select a non-creature artifact", NULL) ){
				op = instance->targets[0].player;
				oc = instance->targets[0].card;
				int cost = (2*get_cmc( instance->targets[0].player, instance->targets[0].card ))+1;
				if( ! has_mana_for_activated_ability(player, card, MANACOST_X(cost)) ){
					spell_fizzled = 1;
					return 0;
				}
				if( charge_mana_for_activated_ability(player, card, MANACOST_X(cost)) ){
					instance->targets[0].player = op;
					instance->targets[0].card = oc;
					instance->number_of_targets = 1;
				}
			}
			else{
				spell_fizzled = 1;
				return 0;
			}
		}
		else if( player == AI ){
				int result = target_for_gorilla_shaman(player, card, 1);
				if( result != -1 ){
					int cost = (2*get_cmc( instance->targets[0].player, instance->targets[0].card ))+1;
					if( charge_mana_for_activated_ability(player, card, MANACOST_X(cost)) ){
					      instance->targets[0].player = 1-player;
					      instance->targets[0].card = result;
					      instance->number_of_targets = 1;
					}
				}
				else{
					spell_fizzled = 1;
					return 0;
				}
		}
	}
	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			kill_card( instance->targets[0].player, instance->targets[0].card, KILL_DESTROY );
		}
	}
	return 0;
}

int card_gorilla_war_cry(int player, int card, event_t event){
/* CARD_ID_GORILLA_WAR_CRY	2293
Gorilla War Cry	|1|R
Instant
Cast ~ only during combat before blockers are declared.
All creatures gain menace until end of turn. (They can't be blocked except by two or more creatures.)
Draw a card at the beginning of the next turn's upkeep. */

	if( event == EVENT_CAN_CAST && current_phase == PHASE_BEFORE_BLOCKING ){
		return basic_spell(player, card, event);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.state = STATE_ATTACKING;
		add_legacy_effect_to_all(player, card, &fx_menace, current_turn, &this_test);
		cantrip(player, card, 1);
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

// guerrilla tactics --> shock

int card_heart_of_yavimaya(int player, int card, event_t event){
/*
Heart of Yavimaya
Land
If Heart of Yavimaya would enter the battlefield, sacrifice a Forest instead.
	If you do, put Heart of Yavimaya onto the battlefield. If you don't, put it into its owner's graveyard.
{T}: Add {G} to your mana pool.
{T}: Target creature gets +1/+1 until end of turn.
*/

	al_lands(player, card, event, SUBTYPE_FOREST, 0);

	if( event == EVENT_COUNT_MANA && affect_me(player, card) ){
		return mana_producer(player, card, event);
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.preferred_controller = player;

	card_instance_t* instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		return mana_producer(player, card, event);
	}

	if( event == EVENT_ACTIVATE ){
		instance->info_slot = instance->number_of_targets = 0;
		int choice = 0;
		int can_pump = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST0, 0, &td, NULL);
		if( !paying_mana() && can_pump ){
			choice = do_dialog(player, player, card, -1, -1, " Produce mana\n Pump a creature\n Cancel", 1);
		}

		if( choice == 0){
			return mana_producer(player, card, event);
		}

		if( choice == 1 ){
			if (charge_mana_for_activated_ability(player, card, MANACOST0) && pick_target(&td, "TARGET_CREATURE")){
				instance->info_slot = 1;
				tap_card(player, card);
			}
		}

		if( choice == 2){
			spell_fizzled = 1;
		}
	}

	if (event == EVENT_RESOLVE_ACTIVATION){
		if( instance->info_slot == 1 && valid_target(&td) ){
			pump_until_eot(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card, 1, 1);
		}
	}

	return 0;
}

int card_helm_of_obedience(int player, int card, event_t event){
/*
Helm of Obedience |4
Artifact
{X}, {T}: Target opponent puts cards from the top of his or her library into his or her graveyard until a creature card or
X cards are put into that graveyard this way, whichever comes first. If a creature card is put into that graveyard this way,
sacrifice Helm of Obedience and put that card onto the battlefield under your control. X can't be 0.
*/
	if( ! is_unlocked(player, card, event, 35) ){ return 0; }

	if (!IS_GAA_EVENT(event)){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_ONLY_TARGET_OPPONENT, MANACOST_X(-1), 0, &td, NULL);
	}

	if( event == EVENT_ACTIVATE ){
		instance->info_slot = 0;
		if( charge_mana_for_activated_ability(player, card, MANACOST0) ){
			charge_mana(player, COLOR_ANY, -1);
			if( spell_fizzled != 1 ){
				if( x_value > 0 ){ //Can't activate this for 0
					instance->targets[0].player = 1-player;
					instance->targets[0].card = -1;
					instance->number_of_targets = 1;
					instance->info_slot = x_value;
					tap_card(player, card);
				}
				else{
					spell_fizzled = 1;
				}
			}
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			x_value = 0;
			special_mill(instance->parent_controller, instance->parent_card, CARD_ID_HELM_OF_OBEDIENCE, 1-player, instance->info_slot);
		}
	}

	return 0;
}

int card_hail_storm(int player, int card, event_t event){
/* CARD_ID_HAIL_STORM	2295
Hail Storm	|1|G|G
Instant
~ deals 2 damage to each attacking creature and 1 damage to you and each creature you control. */

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.state = STATE_ATTACKING;
		new_damage_all(player, card, current_turn, 2, 0, &this_test);
		new_damage_all(player, card, player, 1, NDA_ALL_CREATURES | NDA_PLAYER_TOO, NULL);
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
}

static int inheritance_effect(int player, int card, event_t event){

	if( event == EVENT_STATIC_EFFECTS && affect_me(player, card) ){
		card_instance_t *instance = get_card_instance(player, card);
		if( instance->damage_target_player > -1 ){
			if( get_id(instance->damage_target_player, instance->damage_target_card) != CARD_ID_INHERITANCE ){
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
					}
					if( instance->targets[11].player < 0 ){
						instance->targets[11].player = 0;
					}
					SET_BYTE1(instance->targets[11].player) = 1;
				}
				count_for_gfp_ability(player, card, event, ANYBODY, TYPE_CREATURE, NULL);
			}
		}
	}

	if( ! check_status(player, card, STATUS_INVISIBLE_FX) ){
		int result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
		if( result ){
			int amount = BYTE0(result);
			while( amount && has_mana(player, COLOR_ANY, 3) ){
					charge_mana_while_resolving(player, card, event, player, COLOR_COLORLESS, 3);
					if( spell_fizzled != 1 ){
						draw_cards(player, 1);
					}
					amount--;
			}
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

int card_inheritance(int player, int card, event_t event){
/*
Inheritance |W
Enchantment
Whenever a creature dies, you may pay {3}. If you do, draw a card.
*/
	if( event == EVENT_STATIC_EFFECTS && affect_me(player, card) ){
		int found = 0;
		int c;
		for(c=0; c<active_cards_count[player]; c++){
			if( in_play(player, c) && is_what(player, c, TYPE_EFFECT) ){
				card_instance_t *inst = get_card_instance(player, c);
				if( inst->info_slot == (int)inheritance_effect ){
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
			int legacy = create_targetted_legacy_effect(player, card, &inheritance_effect, player, card);
			card_instance_t *inst = get_card_instance(player, legacy);
			inst->targets[0].player = player;
			inst->targets[0].card = card;
			inst->number_of_targets = 1;
			add_status(player, legacy, STATUS_INVISIBLE_FX);
		}
	}

	if( event == EVENT_GRAVEYARD_FROM_PLAY ){
		count_for_gfp_ability(player, card, event, ANYBODY, TYPE_CREATURE, NULL);
	}

	if( get_card_instance(player, card)->kill_code <= 0 ){
		int result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
		if( result ){
			int amount = BYTE0(result);
			while( amount && has_mana(player, COLOR_ANY, 3) ){
					charge_mana_while_resolving(player, card, event, player, COLOR_COLORLESS, 3);
					if( spell_fizzled != 1 ){
						draw_cards(player, 1);
					}
					amount--;
			}
		}
	}

    return global_enchantment(player, card, event);
}

static int skip_draw_legacy(int player, int card, event_t event){

	card_instance_t *instance= get_card_instance(player, card);

	int p = instance->targets[0].player;

	if( p > -1 && current_turn == p && event == EVENT_DRAW_PHASE ){
		event_result-=99;
		kill_card(player, card, KILL_EXILE);
	}
	return 0;
}

static int effect_gargoyle(int player, int card, event_t event){

	if( resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY) ){
		card_instance_t *instance= get_card_instance(player, card);
		int owner = instance->targets[0].player;
		int iid = instance->targets[0].card;
		seek_grave_for_id_to_reanimate(owner, -1, owner, cards_data[iid].id, REANIMATE_DEFAULT);
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int ivory_gargoyle_impl(int player, int card, event_t event, int x, int b, int u, int g, int r, int w){

	if( event == EVENT_GRAVEYARD_FROM_PLAY && affect_me(player, card) && ! is_token(player, card) ){
		if( is_this_dying(player, card) ){
			int legacy = create_legacy_effect(player, card, &effect_gargoyle);
			get_card_instance(player, legacy)->targets[0].player = get_owner(player, card);
			get_card_instance(player, legacy)->targets[0].card = get_original_internal_card_id(player, card);
			get_card_instance(player, legacy)->targets[11].player = 1; // or resolve_gfp_ability won't trigger

			int legacy2 = create_legacy_effect(player, card, &skip_draw_legacy);
			get_card_instance(player, legacy2)->targets[0].player = player;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance= get_card_instance(player, card);
		kill_card(instance->parent_controller, instance->parent_card, KILL_EXILE);
	}

	return generic_activated_ability(player, card, event, 0, x, b, u, g, r, w, 0, 0, 0);
}

int card_ivory_gargoyle(int player, int card, event_t event)
{
/*
Ivory Gargoyle |4|W
Creature — Gargoyle 2/2, 4W (5)
Flying
When Ivory Gargoyle dies, return it to the battlefield under its owner's control at the beginning of the next end step and
	you skip your next draw step.
{4}{W}: Exile Ivory Gargoyle.
*/
	return ivory_gargoyle_impl(player, card, event, MANACOST_XW(4,1));
}

int card_juniper_order_advocate(int player, int card, event_t event){
/* CARD_ID_JUNIPER_ORDER_ADVOCATE	2301
Juniper Order Advocate	|2|W
Creature - Human Knight 1/2
As long as ~ is untapped, |Sgreen creatures you control get +1/+1. */

	if( in_play(player, card) && ! is_humiliated(player, card) ){
		if( ! is_tapped(player, card) && (event == EVENT_POWER || event == EVENT_TOUGHNESS) && affected_card_controller == player ){
			if( get_color(affected_card_controller, affected_card) & get_sleighted_color_test(player, card, COLOR_TEST_GREEN) ){
				event_result++;
			}
		}
	}

	return 0;
}

int card_kaysa(int player, int card, event_t event)
{
  /* Kaysa	|3|G|G
   * Legendary Creature - Elf Druid 2/3
   * |SGreen creatures you control get +1/+1. */

  check_legend_rule(player, card, event);

  if ((event == EVENT_TOUGHNESS || event == EVENT_POWER)
	  && affected_card_controller == player
	  && (get_color(affected_card_controller, affected_card) & get_sleighted_color_test(player, card, COLOR_TEST_GREEN))
	  && in_play(player, card) && in_play(affected_card_controller, affected_card)
	  && is_what(affected_card_controller, affected_card, TYPE_CREATURE)
	  && !is_humiliated(player, card))
	++event_result;

  return 0;
}

int card_keeper_of_tresserhorn(int player, int card, event_t event){
/* CARD_ID_KEEPER_OF_TRESSERHORN	2302
Keeper of Tresserhorn	|5|B
Creature - Avatar 6/6
Whenever ~ attacks and isn't blocked, it assigns no combat damage this turn and defending player loses 2 life. */

	if( event == EVENT_DECLARE_BLOCKERS && is_attacking(player, card) && is_unblocked(player, card) &&
		! is_humiliated(player, card) )
	{
		lose_life(1-player, 2);
		negate_combat_damage_this_turn(player, card, player, card, 0);
	}
	return 0;
}

int card_kjeldoran_home_guard(int player, int card, event_t event)
{
  /* Kjeldoran Home Guard	|3|W
   * Creature - Human Soldier 1/6
   * At end of combat, if ~ attacked or blocked this combat, put a -0/-1 counter on ~ and put a 0/1 |Swhite Deserter creature token onto the battlefield. */

  if (trigger_condition == TRIGGER_END_COMBAT && ! is_humiliated(player, card)
	  && (get_card_instance(player, card)->state & (STATE_ATTACKING|STATE_ATTACKED|STATE_BLOCKING|STATE_BLOCKED))
	  && end_of_combat_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY))
	{
		add_counter(player, card, COUNTER_M0_M1);

		token_generation_t token;
		default_token_definition(player, card, CARD_ID_DESERTER, &token);
		token.pow = 0;
		token.tou = 1;
		token.color_forced = COLOR_TEST_WHITE;
		generate_token(&token);

	}

  return 0;
}

int card_kjeldoran_outpost(int player, int card, event_t event){
	/* Kjeldoran Outpost	""
	 * Land
	 * If ~ would enter the battlefield, sacrifice |Ha Plains instead. If you do, put ~ onto the battlefield. If you don't, put it into its owner's graveyard.
	 * |T: Add |W to your mana pool.
	 * |1|W, |T: Put a 1/1 |Swhite Soldier creature token onto the battlefield. */

	al_lands(player, card, event, SUBTYPE_PLAINS, 0);

	if( event == EVENT_COUNT_MANA && affect_me(player, card) ){
		return mana_producer(player, card, event);
	}

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	card_instance_t* instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		return mana_producer(player, card, event);
	}

	if(event == EVENT_ACTIVATE ){
		int choice = 0;
		int can_make_token = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_UNTAPPED, MANACOST_XW(1, 2), 0, NULL, NULL);
		if( !paying_mana() && can_make_token ){
			choice = do_dialog(player, player, card, -1, -1, " Produce mana\n Generate a Soldier\n Cancel", 1);
		}

		instance->info_slot = choice;

		if (choice == 0){
			return mana_producer(player, card, event);
		}
		if (choice == 1){
			add_state(player, card, STATE_TAPPED);
			if (!charge_mana_for_activated_ability(player, card, MANACOST_XW(1,1))){
				untap_card_no_event(player, card);
			}
		}
		else{
			spell_fizzled = 1;
		}
	}

	if(event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == 1 ){
			generate_token_by_id(instance->parent_controller, instance->parent_card, CARD_ID_SOLDIER);
		}
	}

	return 0;
}

int krovikan_horror(int player, int card, event_t event){
/*
At the beginning of the end step, if Krovikan Horror is in your graveyard with a creature card directly above it,
you may return Krovikan Horror to your hand.
*/

	if( trigger_condition == TRIGGER_EOT && affect_me(player, card) ){
		int cnt = count_graveyard_by_id(player, CARD_ID_KROVIKAN_HORROR);
		if( cnt && eot_trigger(player, card, event) ){
			int horrors[count_graveyard(player)];
			int i;
			for(i=0; i<count_graveyard(player); i++){
				horrors[i] = 0;
			}
			for(i=0; i<count_graveyard(player); i++){
				if( cards_data[get_grave(player)[i]].id == CARD_ID_KROVIKAN_HORROR ){
					if( get_grave(player)[i+1] != -1 && is_what(-1, get_grave(player)[i+1], TYPE_CREATURE) ){
						horrors[i] = 1;
					}
				}
			}
			for(i=count_graveyard(player)-1; i > -1; i--){
				if( horrors[i] ){
					int choice	= do_dialog(player, player, card, -1, -1," Return Krovikan Horror\n Decline", 1);
					if( choice == 0 ){
						from_grave_to_hand(player, i, TUTOR_HAND);
					}
				}
			}
		}
	}

	return 0;
}

int card_krovikan_horror(int player, int card, event_t event){
	/* Krovikan Horror	|3|B	0x2002109
	 * Creature - Horror Spirit 2/2
	 * At the beginning of the end step, if ~ is in your graveyard with a creature card directly above it, you may return ~ to your hand.
	 * |1, Sacrifice a creature: ~ deals 1 damage to any target. */
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td1.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td1) ){
			damage_target0(player, card, 1);
		}
	}

	return generic_activated_ability(player, card, event, GAA_SACRIFICE_CREATURE+GAA_CAN_TARGET, MANACOST_X(1), 0,
									&td1, "TARGET_ANY");
}

int card_lake_of_the_dead(int player, int card, event_t event)
{
/* CARD_ID_LAKE_OF_THE_DEAD	2309
Lake of the Dead	""
Land
If ~ would enter the battlefield, sacrifice |Ha Swamp instead. If you do, put ~ onto the battlefield. If you don't, put it into its owner's graveyard.
|T: Add |B to your mana pool.
|T, Sacrifice |Ha Swamp: Add |B|B|B|B to your mana pool. */

	al_lands(player, card, event, SUBTYPE_SWAMP, 0);

#define CAN_SAC can_sacrifice_as_cost(player, 1, TYPE_LAND, MATCH, get_hacked_subtype(player, card, SUBTYPE_SWAMP), MATCH, 0,0,0,0,-1,0)

  if (event == EVENT_COUNT_MANA)
	{
	  if (affect_me(player, card) && !is_tapped(player, card) && !is_animated_and_sick(player, card) && can_produce_mana(player, card))
		declare_mana_available(player, COLOR_BLACK, CAN_SAC ? 4 : 1);
	}
  else if (event == EVENT_ACTIVATE)
	{
	  int choice = 1;

	  if (CAN_SAC)
		choice = DIALOG(player, card, event,
						DLG_RANDOM, DLG_NO_STORAGE,
						"Generate B", 1, 1,
						"Sacrifice a Swamp for BBBB", 1, 1);

	  if (choice == 1)
		produce_mana_tapped(player, card, COLOR_BLACK, 1);
	  else if (choice == 2 && sacrifice(player, card, player, 0, TYPE_LAND, MATCH, get_hacked_subtype(player, card, SUBTYPE_SWAMP), MATCH, 0,0,0,0,-1,0))
		produce_mana_tapped(player, card, COLOR_BLACK, 4);
	  else
		spell_fizzled = 1;
	}
  else
	return mana_producer(player, card, event);

  return 0;
#undef CAN_SAC
}

int card_lat_nam_legacy(int player, int card, event_t event){
/* CARD_ID_LAT_NAMS_LEGACY	2310
Lat-Nam's Legacy	|1|U
Instant
Shuffle a card from your hand into your library. If you do, draw two cards at the beginning of the next turn's upkeep. */

	if( event == EVENT_RESOLVE_SPELL){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_ANY, "Select a card to shuffle into deck.");
		int selected = new_select_a_card(player, player, TUTOR_FROM_HAND, 0, AI_MIN_VALUE, -1, &this_test);
		if( selected > -1 ){
			shuffle_into_library(player, selected);
			cantrip(player, card, 2);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_library_of_lat_nam(int player, int card, event_t event){
/*
Library of Lat-Nam |4|U
Sorcery
An opponent chooses one —
• You draw three cards at the beginning of the next turn's upkeep.
• You search your library for a card, put that card into your hand, then shuffle your library.
*/
	if( event == EVENT_RESOLVE_SPELL ){
		int priority_cantrip = hand_count[player]*5;
		int priority_tutor = has_mana(player, COLOR_ANY, -1)*5;
		int choice = DIALOG(player, card, EVENT_ACTIVATE, DLG_RANDOM, DLG_NO_STORAGE, DLG_NO_CANCEL, DLG_WHO_CHOOSES(1-player),
							"Cantrip for 3", 1, priority_cantrip,
							"Demonic Tutor", 1, priority_tutor);
		if( choice == 0 ){
			cantrip(player, card, 3);
		}
		else{
			test_definition_t this_test;
			new_default_test_definition(&this_test, TYPE_ANY, "Select a card to tutor.");
			new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 0, AI_MAX_VALUE, &this_test);
		}

		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_lim_duls_paladin(int player, int card, event_t event){
	/* Lim-Dul's Paladin	|2|B|R	0x2009d3c
	 * Creature - Human Knight 0/3
	 * Trample
	 * At the beginning of your upkeep, you may discard a card. If you don't, sacrifice ~ and draw a card.
	 * Whenever ~ becomes blocked, it gets +6/+3 until end of turn.
	 * Whenever ~ attacks and isn't blocked, it assigns no combat damage this turn and defending player loses 4 life. */
	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		test_definition_t this_test;
		char buffer[100] = "Select a card to discard.";
		new_default_test_definition(&this_test, TYPE_ANY, buffer);

		int kill = 1;
		if( hand_count[player] ){
			int selected = new_select_a_card(player, player, TUTOR_FROM_HAND, 0, AI_MIN_VALUE, -1, &this_test);
			if( selected != -1 ){
				discard_card(player, selected);
				kill = 0;
			}
		}
		if( kill ){
			draw_cards(player, 1);
			kill_card(player, card, KILL_SACRIFICE);
		}
	}

	if( event == EVENT_DECLARE_BLOCKERS && is_attacking(player, card) && ! is_humiliated(player, card) ){
		if( is_unblocked(player, card) ){
			lose_life(1-player, 4);
			negate_combat_damage_this_turn(player, card, player, card, 0);
		}
		else{
			pump_until_eot(player, card, player, card, 6, 3);
		}
	}
	return 0;
}

int card_lim_dul_vault(int player, int card, event_t event){
/* CARD_ID_LIM_DULS_VAULT	2313
Lim-Dul's Vault	|U|B
Instant
Look at the top five cards of your library. As many times as you choose, you may pay 1 life, put those cards on the bottom of your library in any order,
then look at the top five cards of your library. Then shuffle your library and put the last cards you looked at this way on top of it in any order. */

	if( event == EVENT_RESOLVE_SPELL ){
		int *deck = deck_ptr[player];
		int max = 5;
		while( max > 0 && deck[max-1] == -1 ){
				max--;
		}
		if( max > 0 ){
			if( player == AI ){
				rearrange_top_x(player, player, max);
			}
			else{
				while( 1 ){
						show_deck( player, deck_ptr[player], max, "Here are the top 5 cards of your library", 0, 0x7375B0 );
						int choice = 0;
						if( can_pay_life(player, 1) ){
							choice = do_dialog(player, player, card, -1, -1, " Rearrange\n Pay 1 life and continue", 0);
						}
						if(choice == 0){
							rearrange_top_x(player, player, max);
							break;
						}
						else{
							lose_life(player, 1);
							int i;
							for(i = 0; i < max; i++){
								deck[count_deck(player)] = deck[0];
								remove_card_from_deck(player, 0);
							}
						}
				}
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_lodestone_bauble(int player, int card, event_t event){
/*
Lodestone Bauble	|0	0x200f511
Artifact
|1, |T, Sacrifice ~: Put up to four target basic land cards from a player's graveyard on top of his or her library in any order.
	That player draws a card at the beginning of the next turn's upkeep.
*/

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_LAND, "Select target basic land card.");
	test.subtype = SUBTYPE_BASIC;

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;
	td.illegal_abilities = 0;
	td.preferred_controller = count_subtype(player, TYPE_LAND, -1) >= 6 ? 1-player : player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_SACRIFICE_ME, MANACOST_X(1), 0, NULL, NULL) ){
			if( (new_special_count_grave(player, &test) >= 0) && ! graveyard_has_shroud(player) ){
				return 1;
			}
			if( (new_special_count_grave(1-player, &test) >=0) && ! graveyard_has_shroud(1-player) ){
				return 1;
			}
		}
	}

	if( event == EVENT_ACTIVATE ){
		instance->number_of_targets = 0;
		int t_player = player;
		if( (new_special_count_grave(player, &test) >= 0) && ! graveyard_has_shroud(player) ){
			if( (new_special_count_grave(1-player, &test) >=0) && ! graveyard_has_shroud(1-player) ){
				new_pick_target(&td, "Select target graveyard.", 0, 1 | GS_LITERAL_PROMPT);
			}
		}
		else{
			t_player = 1-player;
		}
		if( spell_fizzled != 1 ){
			instance->targets[0].player = t_player;
			if( charge_mana_for_activated_ability(player, card, MANACOST_X(1)) ){
				select_multiple_cards_from_graveyard(player, t_player, 0, AI_FIRST_FOUND, &test, 4, &instance->targets[1]);
				if( spell_fizzled != 1 ){
					add_state(player, card, STATE_TAPPED);
					kill_card(player, card, KILL_SACRIFICE);
				}
			}
		}
	}

	if(event == EVENT_RESOLVE_ACTIVATION ){
		int i, num_validated = 0, num_targets = 0;
		for (i = 1; i <= 4; ++i){
			if (instance->targets[i].player != -1){
				++num_targets;
				int selected = validate_target_from_grave(player, card, instance->targets[0].player, i);
				if (selected != -1){
					from_graveyard_to_deck(instance->targets[0].player, selected, 1);
					++num_validated;
				}
			}
		}
		if (num_validated > 1){
			rearrange_top_x(player, instance->targets[0].player, num_validated);
		}
		if (num_validated == 0 && num_targets > 0){
			spell_fizzled = 1;
		}
		else{
			int result = cantrip(player, card, 1);
			card_instance_t *leg = get_card_instance(player, result);
			leg->targets[1].player = instance->targets[0].player;
		}
	}

	return 0;
}

int card_lord_of_tresserhorn(int player, int card, event_t event){
/* CARD_ID_LORD_OF_TRESSERHORN	2315
Lord of Tresserhorn	|1|U|B|R
Legendary Creature - Zombie 10/4
When ~ enters the battlefield, you lose 2 life, you sacrifice two creatures, and target opponent draws two cards.
|B: Regenerate ~. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice");
		test.qty = 2;

		int kill_me = 1;
		if( new_can_sacrifice(player, card, player, &test) ){
			new_sacrifice(player, card, player, SAC_CAUSED | SAC_NO_CANCEL, &test);
		    lose_life(player, 2);
		    draw_cards(1-player, 2);
			kill_me = 0;

	    }
		if( kill_me ){
			kill_card(player, card, KILL_SACRIFICE);
			lose_life(player, 2);
		    draw_cards(1-player, 2);
		}
	}
	return card_drudge_skeletons(player, card, event);
}

int card_misfortune(int player, int card, event_t event){
/* Misfortune	|1|B|R|G
 * Sorcery
 * An opponent chooses one -
 * You put a +1/+1 counter on each creature you control and gain 4 life;
 * Or you put a -1/-1 counter on each creature that player controls and ~ deals 4 damage to him or her. */

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t test;
		default_test_definition(&test, TYPE_CREATURE);
		test.toughness = 2;
		test.toughness_flag = F5_TOUGHNESS_LESSER_THAN_VALUE;

		test_definition_t test2;
		default_test_definition(&test2, TYPE_CREATURE);
		test2.toughness = 1;

		int priority_opp = life[player] + (check_battlefield_for_special_card(player, card, player, CBFSC_GET_COUNT, &test2)*2);
		int priority_you = life[1-player] - (check_battlefield_for_special_card(player, card, 1-player, CBFSC_GET_COUNT, &test)*2);

		int choice = DIALOG(player, card, EVENT_ACTIVATE, DLG_RANDOM, DLG_NO_STORAGE, DLG_NO_CANCEL, DLG_WHO_CHOOSES(1-player),
							"Opp: +1/+1 counters + 4 life", 	1, priority_opp,
							"You: -1/-1 counters + 4 damages", 	1, priority_you);

		if( choice == 1 ){
			manipulate_type(player, card, player, TYPE_CREATURE, ACT_ADD_COUNTERS(COUNTER_P1_P1, 1));
			gain_life(player, 4);
		}
		else{
			manipulate_type(player, card, 1-player, TYPE_CREATURE, ACT_ADD_COUNTERS(COUNTER_M1_M1, 1));
			damage_player(1-player, 4, player, card);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

static int natures_blessing_effect(int player, int card, event_t event){

	if( event == EVENT_ABILITIES && get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);
		if( affect_me(instance->damage_target_player, instance->damage_target_card) ){
			event_result |= instance->targets[1].card;
		}
	}

	return 0;
}

int card_natures_blessing(int player, int card, event_t event){
/* CARD_ID_NATURES_BLESSING	2321
Nature's Blessing	|2|G|W
Enchantment
|G|W, Discard a card: Put a +1/+1 counter on target creature or that creature gains banding, first strike, or trample.
(This effect lasts indefinitely. Any creatures with banding, and up to one without, can attack in a band. Bands are blocked as a group.
If any creatures with banding a player controls are blocking or being blocked by a creature, that player divides that creature's combat damage,
not its controller, among any of the creatures it's being blocked by or is blocking.) */

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			int keyword = 0;
			int ai_choice = 2;
			if( check_for_ability(instance->targets[0].player, instance->targets[0].card, KEYWORD_TRAMPLE) ){
				ai_choice = 1;
			}
			if( check_for_ability(instance->targets[0].player, instance->targets[0].card, KEYWORD_FIRST_STRIKE) ){
				ai_choice = 0;
			}
			int choice = do_dialog(player, player, card, -1, -1," Add +1/+1 counter\n Give First Strike\n Give Trample\n Give Banding",
									ai_choice);
			if( choice == 0 ){
				add_1_1_counter(instance->targets[0].player, instance->targets[0].card);
			}
			else  if( choice == 1 ){
					keyword = KEYWORD_FIRST_STRIKE;
			}
			else  if( choice == 2 ){
					keyword = KEYWORD_TRAMPLE;
			}
			else  if( choice == 3 ){
					keyword = KEYWORD_BANDING;
			}

			if( keyword > 0 ){
				// Search for another legacies of this card attached to the same creature
				int found_p = -1;
				int found_c = -1;
				CYCLE_ALL_CARDS(p, c, 	{
											if( in_play(p, c) && is_what(p, c, TYPE_EFFECT) ){
												card_instance_t *inst = get_card_instance(p, c);
												if( inst->info_slot == (int)natures_blessing_effect ){
													if( inst->damage_target_player == instance->targets[0].player &&
														inst->damage_target_card == instance->targets[0].card &&
														inst->targets[0].player == instance->parent_controller &&
														inst->targets[0].card == instance->parent_card )
													{
														found_p = p;
														found_c = c;
														goto end;
													}
												}
											}
										};
				);
				end:
				if( found_p == -1 ){
					found_p = player;
					found_c = create_targetted_legacy_effect(player, card, &natures_blessing_effect,
															instance->targets[0].player, instance->targets[0].card);
					card_instance_t *tok = get_card_instance(found_p, found_c);
					tok->targets[0].player = instance->parent_controller;
					tok->targets[0].card = instance->parent_card;
					tok->number_of_targets = 1;
				}
				card_instance_t *tok = get_card_instance(found_p, found_c);
				if( tok->targets[1].card < 0 ){
					tok->targets[1].card = 0;
				}
				tok->targets[1].card |= keyword;
			}
		}
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_DISCARD, MANACOST_GW(1, 1), 0, &td, "TARGET_CREATURE");
}

int card_natures_chosen(int player, int card, event_t event){
	/*
	  Nature's Chosen |G
	  Enchantment - Aura
	  Enchant creature you control
	  {0}: Untap enchanted creature. Activate this ability only during your turn and only once each turn.
	  Tap enchanted creature: Untap target artifact, creature, or land. Activate this ability only if enchanted creature is white and
	  is untapped and only once each turn.
	*/

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CLEANUP ){
		instance->targets[1].player = 0;
	}

	if( ! IS_AURA_EVENT(player, card, event) && ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.allowed_controller = player;
	td.preferred_controller = player;

	if( in_play(player, card) && instance->damage_target_player > -1 ){
		target_definition_t td1;
		default_target_definition(player, card, &td1, TYPE_ARTIFACT|TYPE_CREATURE|TYPE_LAND);
		td1.preferred_controller = player;

		int p = instance->damage_target_player;
		int c = instance->damage_target_card;

		enum{
			CHOICE_UNTAP_ENCHANTED_CRIT = 1,
			CHOICE_UNTAP_PERMANENT
		};

		if( event == EVENT_CAN_ACTIVATE ){
			int abil1 = (instance->targets[1].player < 0 ||  (instance->targets[1].player > -1 && !(instance->targets[1].player & CHOICE_UNTAP_ENCHANTED_CRIT))) ? 1 : 0;
			if( current_turn == player && abil1 && generic_activated_ability(player, card, event, 0, MANACOST0, 0, NULL, NULL) ){
				return 1;
			}
			int abil2 = (instance->targets[1].player < 0 || (instance->targets[1].player > -1 && !(instance->targets[1].player & CHOICE_UNTAP_PERMANENT))) ? 1 : 0;
			if( abil2 && ! is_tapped(p, c) && (get_color(p, c) & get_sleighted_color_test(player, card, COLOR_TEST_WHITE)) &&
				generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST0, 0, &td1, NULL)
			  ){
				return 1;
			}
		}
		if( event == EVENT_ACTIVATE ){
			instance->info_slot = instance->number_of_targets = 0;
			int abils[2] = {0, 0};
			int abil1 = (instance->targets[1].player < 0 ||  (instance->targets[1].player > -1 && !(instance->targets[1].player & CHOICE_UNTAP_ENCHANTED_CRIT))) ? 1 : 0;
			if( current_turn == player && abil1 && generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, MANACOST0, 0, NULL, NULL) ){
				abils[0] = 1;
			}
			int abil2 = (instance->targets[1].player < 0 || (instance->targets[1].player > -1 && !(instance->targets[1].player & CHOICE_UNTAP_PERMANENT))) ? 1 : 0;
			if( abil2 && ! is_tapped(p, c) && (get_color(p, c) & get_sleighted_color_test(player, card, COLOR_TEST_WHITE)) &&
				generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, GAA_CAN_TARGET, MANACOST0, 0, &td1, NULL)
			  ){
				abils[1] = 1;
			}
			int choice = DIALOG(player, card, event, DLG_RANDOM, DLG_NO_STORAGE,
							"Untap enchanted creature", abils[0], is_tapped(p, c) ? 10 : 0,
							"Untap permanent", abils[1], 10);
			if( ! choice ){
				spell_fizzled = 1;
				return 0;
			}
			if( charge_mana_for_activated_ability(player, card, MANACOST0) ){
				instance->info_slot = choice;
				if( choice == CHOICE_UNTAP_ENCHANTED_CRIT ){
					if( instance->targets[1].player < 0 ){
						instance->targets[1].player = 0;
					}
					instance->targets[1].player |= CHOICE_UNTAP_ENCHANTED_CRIT;
				}
				if( choice == CHOICE_UNTAP_PERMANENT ){
					if( new_pick_target(&td1, "Select target artifact, creature, or land.", 0, 1 | GS_LITERAL_PROMPT) ){
						tap_card(p, c);
						if( instance->targets[1].player < 0 ){
							instance->targets[1].player = 0;
						}
						instance->targets[1].player |= CHOICE_UNTAP_PERMANENT;
					}
				}
			}
		}
		if( event == EVENT_RESOLVE_ACTIVATION ){
			if( instance->info_slot == CHOICE_UNTAP_ENCHANTED_CRIT ){
				untap_card(p, c);
			}
			if( instance->info_slot == CHOICE_UNTAP_PERMANENT && valid_target(&td1) ){
				untap_card(instance->targets[0].player, instance->targets[0].card);
			}
		}
	}

	return targeted_aura(player, card, event, &td, "TARGET_CREATURE_YOU_CONTROL");
}

int card_noble_steeds(int player, int card, event_t event){
/* CARD_ID_NOBLE_STEEDS	2324
Noble Steeds	|2|W
Enchantment
|1|W: Target creature gains first strike until end of turn. */

	if( ! IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.preferred_controller = player;

	return vanilla_creature_pumper(player, card, event, MANACOST_XW(1, 1), 0, 0, 0, KEYWORD_FIRST_STRIKE, 0, &td);
}

int card_phantasmal_fiend(int player, int card, event_t event)
{
/* CARD_ID_PHANTASMAL_FIEND	2326
Phantasmal Fiend	|3|B
Creature - Illusion 1/5
|B: ~ gets +1/-1 until end of turn.
|1|U: Switch ~'s power and toughness until end of turn. */

  card_instance_t *instance = get_card_instance(player, card);

  if (event == EVENT_CAN_ACTIVATE || event == EVENT_ACTIVATE || event == EVENT_RESOLVE_ACTIVATION)
	{
	  // |B: ~ gets +1/-1 until end of turn.
	  // |1|U: Switch ~'s power and toughness until end of turn.
	  enum
	  {
		CHOICE_PUMP = 1,
		CHOICE_SWITCH
	  } choice = DIALOG(player, card, event,
						"+1/-1", 1, 1, DLG_MANA(MANACOST_B(1)),
						"Switch power and toughness", 1, 2, DLG_MANA(MANACOST_XU(1,1)));

	  if (event == EVENT_CAN_ACTIVATE)
		return choice && can_use_activated_abilities(player, card);
	  else if (event == EVENT_RESOLVE_ACTIVATION)
		switch (choice)
		  {
			case CHOICE_PUMP:
			  alternate_legacy_text(1, player, pump_until_eot_merge_previous(player, card, instance->parent_controller, instance->parent_card, 1, -1));
			  break;

			case CHOICE_SWITCH:
			  alternate_legacy_text(2, player, switch_power_and_toughness_until_eot(player, card, instance->parent_controller, instance->parent_card));
			  break;
		  }
	}

	return 0;
}

int card_phelddagrif(int player, int card, event_t event){
	/* Phelddagrif	|1|G|W|U
	 * Legendary Creature - Phelddagrif 4/4
	 * |G: ~ gains trample until end of turn. Target opponent puts a 1/1 |Sgreen Hippo creature token onto the battlefield.
	 * |W: ~ gains flying until end of turn. Target opponent gains 2 life.
	 * |U: Return ~ to its owner's hand. Target opponent may draw a card. */

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	enum{
		CHOICE_TRAMPLE = 1,
		CHOICE_FLYING,
		CHOICE_BOUNCE
	};

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability(player, card, event, GAA_CAN_ONLY_TARGET_OPPONENT, MANACOST0, 0, &td, NULL) ){
			if( has_mana_for_activated_ability(player, card, MANACOST_G(1)) ||
				has_mana_for_activated_ability(player, card, MANACOST_W(1)) ||
				has_mana_for_activated_ability(player, card, MANACOST_U(1))
			  ){
				return 1;
			}
		}
	}

	if( event == EVENT_ACTIVATE){
		int priorities[3] = {	is_attacking(player, card) && current_phase == PHASE_AFTER_BLOCKING && ! is_unblocked(player, card) ? 10 : 1,
								current_phase < PHASE_DECLARE_BLOCKERS ? 10 : 1,
								5
		};
		int choice = DIALOG(player, card, event, DLG_RANDOM, DLG_NO_STORAGE,
							"Gains Trample.", has_mana_for_activated_ability(player, card, MANACOST_G(1)), priorities[0],
							"Gains Flying.", has_mana_for_activated_ability(player, card, MANACOST_W(1)), priorities[1],
							"Gains Bounce.", has_mana_for_activated_ability(player, card, MANACOST_U(1)), priorities[2]);

		if( ! choice ){
			spell_fizzled = 1;
			return 0;
		}
		if( charge_mana_for_activated_ability(player, card, 0,
															0,
															(choice == CHOICE_BOUNCE ? 1 : 0),
															(choice == CHOICE_TRAMPLE ? 1 : 0),
															0,
															(choice == CHOICE_FLYING ? 1 : 0))
			){
				instance->info_slot = choice;
				instance->targets[0].player = 1-player;
				instance->targets[0].card = -1;
				instance->number_of_targets = 1;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			if( instance->info_slot == CHOICE_TRAMPLE ){
				alternate_legacy_text(1, player, pump_ability_until_eot(instance->parent_controller, instance->parent_card, instance->parent_controller, instance->parent_card,
										                                  0, 0, KEYWORD_TRAMPLE, 0));
				token_generation_t token;
				default_token_definition(player, card, CARD_ID_HIPPO, &token);
				token.t_player = instance->targets[0].player;
				token.pow = token.tou = 1;
				token.color_forced = COLOR_TEST_GREEN;
				generate_token(&token);
			}

			if( instance->info_slot == CHOICE_FLYING ){
				alternate_legacy_text(2, player, pump_ability_until_eot(instance->parent_controller, instance->parent_card, instance->parent_controller, instance->parent_card,
										                                  0, 0, KEYWORD_FLYING, 0));
				gain_life(instance->targets[0].player, 2);
			}

			if( instance->info_slot == CHOICE_BOUNCE ){
				draw_some_cards_if_you_want(instance->parent_controller, instance->parent_card, instance->targets[0].player, 1);
				if( in_play(instance->parent_controller, instance->parent_card) ){
					bounce_permanent(instance->parent_controller, instance->parent_card);
				}
			}
		}
	}

	return 0;
}

int card_phyrexian_boon(int player, int card, event_t event)
{
  /* Phyrexian Boon	|2|B
   * Enchantment - Aura
   * Enchant creature
   * Enchanted creature gets +2/+1 as long as it's |Sblack. Otherwise, it gets -1/-2. */

  if (event == EVENT_POWER || event == EVENT_TOUGHNESS || event == EVENT_SHOULD_AI_PLAY)
	{
	  card_instance_t* instance;
	  if (!(instance = in_play(player, card))
		  || instance->damage_target_player < 0
		  || (event != EVENT_SHOULD_AI_PLAY && !affect_me(instance->damage_target_player, instance->damage_target_card)))
		return 0;

	  int black = get_color(instance->damage_target_player, instance->damage_target_card) & get_sleighted_color_test(player, card, COLOR_TEST_BLACK);
	  switch (event)
		{
		  case EVENT_SHOULD_AI_PLAY:
			ai_modifier += (!black == !(instance->damage_target_player == AI)) ? 24 : -24;
			break;

		  case EVENT_POWER:
			event_result += black ? 2 : -1;
			break;

		  case EVENT_TOUGHNESS:
			event_result += black ? 1 : -2;
			break;

		  default:
			break;
		}
	}

  return vanilla_aura(player, card, event, ANYBODY);
}

int card_phyrexian_devourer(int player, int card, event_t event){

	/* Phyrexian Devourer	|6
	 * Artifact Creature - Construct 1/1
	 * When ~'s power is 7 or greater, sacrifice it.
	 * Exile the top card of your library: Put X +1/+1 counters on ~, where X is the exiled card's converted mana cost. */

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_STATIC_EFFECTS && affect_me(player, card) && ! is_humiliated(player, card) && get_power(player, card) >= 7 ){
		kill_card(player, card, KILL_SACRIFICE);
	}

	if( event == EVENT_CAN_ACTIVATE && CAN_ACTIVATE0(player, card) && deck_ptr[player][0] != -1 ){
		return 1;
	}

	if( event == EVENT_ACTIVATE){
		if( charge_mana_for_activated_ability(player, card, MANACOST0) ){
			int *deck = deck_ptr[player];
			int amount = get_cmc_by_id(cards_data[deck[0]].id);

			show_deck( player, deck, 1, "You will remove this card", 0, 0x7375B0 );

			rfg_top_card_of_deck(player);
			instance->info_slot = amount;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int amount = instance->info_slot;
		if( amount > 0 ){
			add_1_1_counters(instance->parent_controller, instance->parent_card, amount);
		}
	}

	return 0;
}

int card_phyrexian_war_beast(int player, int card, event_t event){
/* CARD_ID_PHYREXIAN_WAR_BEAST	2332
Phyrexian War Beast	|3
Artifact Creature - Beast 3/4
When ~ leaves the battlefield, sacrifice a land and ~ deals 1 damage to you. */

	if( leaves_play(player, card, event) ){
		impose_sacrifice(player, card, player, 1, TYPE_LAND, 0, 0, 0, 0, 0, 0, 0, -1, 0);
		damage_player(player, 1, player, card);
	}

	return 0;
}

int card_pillage(int player, int card, event_t event){
	/*
	  Pillage |1|R|R
	  Sorcery
	  Destroy target artifact or land. It can't be regenerated.
	*/
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT | TYPE_LAND);

	card_instance_t *instance = get_card_instance( player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
		}
		kill_card(player, card, KILL_DESTROY);
	}
	return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td, "Select target target artifact or land.", 1, NULL);
}

int card_primitive_justice(int player, int card, event_t event){
/* CARD_ID_PRIMITIVE_JUSTICE	2333
Primitive Justice	|1|R
Sorcery
As an additional cost to cast ~, you may pay |1|R and/or |1|G any number of times.
Destroy target artifact. For each additional |1|R you paid, destroy another target artifact.
For each additional |1|G you paid, destroy another target artifact, and you gain 1 life. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT);

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_ARTIFACT", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->info_slot = instance->number_of_targets = 0;
		if( pick_target(&td, "TARGET_ARTIFACT")  ){
			state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
			int trgs = 1;
			while( can_target(&td) && (has_mana_multi(player, MANACOST_XR(1, 1)) || has_mana_multi(player, MANACOST_XG(1, 1))) ){
					int choice = 0;
					if( has_mana_multi(player, MANACOST_XR(1, 1)) ){
						if( has_mana_multi(player, MANACOST_XG(1, 1)) ){
							choice = do_dialog(player, player, card, -1, -1, " Continue (pay 1R)\n  Continue (pay 1G)\n Stop", 1);
						}
					}
					else{
						choice = 1;
					}
					if( choice == 0 ){
						charge_mana_multi(player, MANACOST_XR(1, 1));
						if( spell_fizzled != 1 && new_pick_target(&td, "TARGET_ARTIFACT", trgs, 0) ){
							state_untargettable(instance->targets[trgs].player, instance->targets[trgs].card, 1);
							trgs++;
						}
						else{
							break;
						}
					}
					else if( choice == 1 ){
							charge_mana_multi(player, MANACOST_XG(1, 1));
							if( spell_fizzled != 1 && new_pick_target(&td, "TARGET_ARTIFACT", trgs, 0) ){
								state_untargettable(instance->targets[trgs].player, instance->targets[trgs].card, 1);
								instance->info_slot |= (1<<trgs);
								trgs++;
							}
							else{
								break;
							}
					}
					else{
						break;
					}
			}
			int i;
			for(i=0; i<instance->number_of_targets; i++){
				state_untargettable(instance->targets[i].player, instance->targets[i].card, 0);
			}
		}
	}

	if( event == EVENT_RESOLVE_SPELL){
		int i;
		for(i=0; i<instance->number_of_targets; i++){
			if( validate_target(player, card, &td, i) ){
				kill_card(instance->targets[i].player, instance->targets[i].card, KILL_DESTROY);
				if( instance->info_slot & (1<<i) ){
					gain_life(player, 1);
				}
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_pyrokinesis(int player, int card, event_t event){
/* CARD_ID_PYROKINESIS	2334
Pyrokinesis	|4|R|R
Instant
You may exile a |Sred card from your hand rather than pay ~'s mana cost.
~ deals 4 damage divided as you choose among any number of target creatures. */

	al_pitchspell(player, card, event, COLOR_RED, 0);

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td1, "TARGET_CREATURE", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		get_card_instance(player, card)->number_of_targets = 0;
		int x;
		for(x = 0; x < 4; x++){
			new_pick_target(&td1, "TARGET_CREATURE", x, 1);
		}
		if( spell_fizzled != 1 && ! casting_al_pitchspell(player, card, event, COLOR_RED, 0) ){
			spell_fizzled = 1;
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		divide_damage(player, card, &td1);
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_reinforcements(int player, int card, event_t event){
/* CARD_ID_REINFORCEMENTS	2335
Reinforcements	|W
Instant
Put up to three target creature cards from your graveyard on top of your library. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_CREATURE, "Select up to 3 creature cards.");

	card_instance_t* instance = get_card_instance(player, card);

	if (event == EVENT_CAN_CAST){
		return generic_spell(player, card, event, GS_GRAVE_RECYCLER, NULL, NULL, 0, &this_test);
	}
	if (event == EVENT_CAST_SPELL && affect_me(player, card)){
		if (count_graveyard_by_type(player, TYPE_CREATURE) <= 0 || graveyard_has_shroud(player)){
			ai_modifier -= 48;
		}
		select_multiple_cards_from_graveyard(player, player, 0, AI_MAX_VALUE, &this_test, 3, &instance->targets[0]);
	}

	if (event == EVENT_RESOLVE_SPELL){
		int i, num_validated = 0;
		for (i = 0; i < 3; ++i){
			int selected = validate_target_from_grave(player, card, player, i);
			if (selected != -1){
				from_graveyard_to_deck(player, selected, 1);
				++num_validated;
			}
		}
		if (num_validated > 1){
			rearrange_top_x(player, player, num_validated);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_reprisal(int player, int card, event_t event){

	/* Reprisal	|1|W
	 * Instant
	 * Destroy target creature with power 4 or greater. It can't be regenerated. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);
	td1.power_requirement = 4 | TARGET_PT_GREATER_OR_EQUAL;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td1) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td1, "Select target creature with power 4 or greater.",
						1, NULL);
}

int card_ritual_of_the_machine(int player, int card, event_t event){
/* CARD_ID_RITUAL_OF_THE_MACHINE	2337
Ritual of the Machine	|2|B|B
Sorcery
As an additional cost to cast ~, sacrifice a creature.
Gain control of target nonartifact, non|Sblack creature. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);
	td1.illegal_color = get_sleighted_color_test(player, card, COLOR_TEST_BLACK);
	td1.illegal_type = TYPE_ARTIFACT;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td1) ){
			gain_control_permanently(player, instance->targets[0].player, instance->targets[0].card);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET | GS_SAC_CREATURE_AS_COST | GS_LITERAL_PROMPT,
						&td1, get_sleighted_color_text(player, card, "Select target nonartifact, non%s creature.", COLOR_BLACK),
						1, NULL);
}

int card_rogue_skycaptain(int player, int card, event_t event){
/* CARD_ID_ROGUE_SKYCAPTAIN	2338
Rogue Skycaptain	|2|R
Creature - Human Rogue Mercenary 3/4
Flying
At the beginning of your upkeep, put a wage counter on ~. You may pay |2 for each wage counter on it.
If you don't, remove all wage counters from ~ and an opponent gains control of it. */

	if( current_turn == player && !is_humiliated(player, card) && trigger_condition == TRIGGER_UPKEEP && affect_me(player, card)
	    && reason_for_trigger_controller == player){
		int count = count_upkeeps(player);
		if(event == EVENT_TRIGGER && count > 0){
			event_result |= RESOLVE_TRIGGER_MANDATORY;
		}
		else if(event == EVENT_RESOLVE_TRIGGER){
				int swap = 0;
				while( count > 0 ){
						add_counter(player, card, COUNTER_WAGE);
						swap = 1;
						int amount = 2*count_counters(player, card, COUNTER_WAGE);
						if( has_mana(player, COLOR_COLORLESS, amount) ){
							int choice = do_dialog(player, player, card, -1, -1, " Pay Skycaptain's wager\n Pass", 0);
							if( choice == 0 ){
								charge_mana(player, COLOR_COLORLESS, amount);
								if( spell_fizzled != 1 ){
									swap = 0;
								}
							}
						}
						if( swap == 1 ){
							break;
						}
						count--;
				}
				if( swap == 1 ){
					remove_all_counters(player, card, COUNTER_WAGE);
					give_control(player, card, player, card);
				}
		}
	}

	return 0;
}

int effect_scars_of_the_veteran(int player, int card, event_t event){

	if( event == EVENT_PREVENT_DAMAGE ){
		card_instance_t* instance = get_card_instance(player, card);
		if( instance->counter_toughness <= 0 ){
			return 0;
		}
		card_instance_t* damage = get_card_instance(affected_card_controller, affected_card);
		if( damage_card == damage->internal_card_id && damage->damage_target_player == instance->damage_target_player &&
			damage->damage_target_card == instance->damage_target_card &&
			damage->info_slot > 0
		  ){
			int prevention = CLAMP(instance->counter_toughness, 0, damage->info_slot);
			prevent_damage(affected_card_controller, affected_card, prevention);
			instance->counter_toughness -= prevention;
		}
	}

	if( eot_trigger(player, card, event) ){
		card_instance_t* instance = get_card_instance(player, card);
		int counters = instance->targets[1].card - instance->counter_toughness;
		if( counters > 0 && instance->damage_target_card >= 0 && is_what(instance->damage_target_player, instance->damage_target_card, TYPE_CREATURE) ){
			add_counters(instance->damage_target_player, instance->damage_target_card, COUNTER_P0_P1, counters);
		}
		kill_card(player, card, KILL_EXILE);
	}

  return 0;
}


int card_scars_of_the_veteran(int player, int card, event_t event){
	/* Scars of the Veteran	|4|W	0x2002f23
	 * Instant
	 * You may exile a |Swhite card from your hand rather than pay this spell's mana cost.
	 * Prevent the next 7 damage that would be dealt to any target this turn. If it's a creature, put a +0/+1 counter on it for each 1 damage prevented this way at the beginning of the next end step. */
	al_pitchspell(player, card, event, COLOR_WHITE, 0);

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td1.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td1, "TARGET_ANY", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		if( casting_al_pitchspell(player, card, event, COLOR_WHITE, 0) ){
			return generic_spell(player, card, event, GS_CAN_TARGET, &td1, "TARGET_ANY", 1, NULL);
		}
		else{
			spell_fizzled = 1;
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td1) ){
			int leg = create_targetted_legacy_effect(player, card, &effect_scars_of_the_veteran,
													instance->targets[0].player, instance->targets[0].card);
			card_instance_t *tok = get_card_instance(player, leg);
			tok->targets[1].card = tok->counter_toughness = 7;
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_sheltered_valley(int player, int card, event_t event){

	/* Sheltered Valley	""
	 * Land
	 * If ~ would enter the battlefield, instead sacrifice each other permanent named ~ you control, then put ~ onto the battlefield.
	 * At the beginning of your upkeep, if you control three or fewer lands, you gain 1 life.
	 * |T: Add |1 to your mana pool. */

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_LAND);
		this_test.id = get_id(player, card);
		this_test.not_me = 1;
		new_manipulate_all(player, card, player, &this_test, KILL_SACRIFICE);
	}

	upkeep_trigger_ability(player, card, event, player);

	if( event == EVENT_UPKEEP_TRIGGER_ABILITY ){
		if( count_subtype(player, TYPE_LAND, -1) <= 3 ){
			gain_life(player, 1);
		}
	}

	return mana_producer(player, card, event);
}

int card_shield_sphere(int player, int card, event_t event){

	/* Shield Sphere	|0
	 * Artifact Creature - Wall 0/6
	 * Defender
	 * Whenever ~ blocks, put a -0/-1 counter on it. */

	if( current_turn == 1-player && ! is_humiliated(player, card) && blocking(player, card, event) ){
		add_counter(player, card, COUNTER_M0_M1);
	}

	return 0;
}

int card_sol_grail(int player, int card, event_t event){
/* CARD_ID_SOL_GRAIL	2347
Sol Grail	|3
Artifact
As ~ enters the battlefield, choose a color.
|T: Add one mana of the chosen color to your mana pool. */

	if( event == EVENT_RESOLVE_SPELL ){
		int ai_choice = 0;
		if( player == AI ){
			ai_choice = get_deck_color(player, player);
		}
		card_instance_t *instance = get_card_instance( player, card );
		instance->info_slot = 1<<choose_a_color(player, ai_choice);
	}

	return mana_producer(player, card, event);
}

int card_soldevi_digger(int player, int card, event_t event){
/* CARD_ID_SOLDEVI_DIGGER	2349
Soldevi Digger	|2
Artifact
|2: Put the top card of your graveyard on the bottom of your library. */

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int top_grave = count_graveyard(player)-1;
		deck_ptr[player][count_deck(player)] = get_grave(player)[top_grave];
		remove_card_from_grave(player, top_grave);
	}

	return generic_activated_ability(player, card, event, 0, MANACOST_X(2), 0, NULL, NULL);
}

int card_soldevi_excavations(int player, int card, event_t event){
/* CARD_ID_SOLDEVI_EXCAVATIONS	2350
Soldevi Excavations	""
Land
If ~ would enter the battlefield, sacrifice an untapped |H2Island instead. If you do, put ~ onto the battlefield. If you don't, put it into its owner's graveyard.
|T: Add |C|U to your mana pool.
|1, |T: Scry 1. (Look at the top card of your library. You may put that card on the bottom of your library.) */

	al_lands(player, card, event, SUBTYPE_ISLAND, TARGET_STATE_TAPPED);

	if(event == EVENT_COUNT_MANA){
		if (affect_me(player, card) && mana_producer(player, card, EVENT_CAN_ACTIVATE) ){
			declare_mana_available(player, COLOR_COLORLESS, 1);
			declare_mana_available(player, COLOR_BLUE, 1);
		}
	}

	if( event == EVENT_CAN_ACTIVATE ){
		return mana_producer(player, card, event);
	}

	if (event == EVENT_ACTIVATE){
		int choice = 0;
		if (!paying_mana() && has_mana_for_activated_ability(player, card, MANACOST_X(3))){
			choice = do_dialog(player, player, card, -1, -1, " Add 1U\n Scry 1 \n Cancel", 0);
		}

		card_instance_t* instance = get_card_instance(player, card);
		instance->info_slot = choice;

		if( choice == 0 ){
			produce_mana_tapped2(player, card, COLOR_COLORLESS, 1, COLOR_BLUE, 1);
		}
		else if (choice == 1){
				add_state(player, card, STATE_TAPPED);
				if (!charge_mana_for_activated_ability(player, card, MANACOST_X(1))){
					untap_card_no_event(player, card);
				}
		}
		else{
			spell_fizzled = 1;
		}
	}

	if (event == EVENT_RESOLVE_ACTIVATION){
		card_instance_t* instance = get_card_instance(player, card);
		if (instance->info_slot == 1){
			scrylike_effect(player, player, 1);
		}
	}

	return 0;
}

int card_soldier_of_fortune(int player, int card, event_t event){
/* CARD_ID_SOLDIER_OF_FORTUNE	2355
Soldier of Fortune	|R
Creature - Human Mercenary 1/1
|R, |T: Target player shuffles his or her library. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			shuffle(instance->targets[0].player);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET, MANACOST_R(1), 0, &td, "TARGET_PLAYER");
}

static int spiny_starfish_legacy(int player, int card, event_t event){
	if( eot_trigger(player, card, event) ){
		card_instance_t *instance = get_card_instance(player, card);

		token_generation_t token;
		default_token_definition(instance->targets[0].player, instance->targets[0].card, CARD_ID_STARFISH, &token);
		token.pow = 0;
		token.tou = 1;
		token.qty = instance->targets[1].player;
		token.color_forced = instance->targets[1].card;
		generate_token(&token);

		kill_card(player, card, KILL_EXILE);
	}
	return 0;
}

void spiny_starfish_splitting_ability(int player, int card){
	int found_p = -1;
	int found_c = -1;
	CYCLE_ALL_CARDS(p, c,{
							if( in_play(p, c) && is_what(p, c, TYPE_EFFECT) ){
								card_instance_t *inst = get_card_instance(p, c);
								if( inst->info_slot == (int)spiny_starfish_legacy ){
									if( inst->targets[0].player == player &&
										inst->targets[0].card == card )
									{
										found_p = p;
										found_c = c;
										goto end;
									}
								}
							}
						};
	);
	end:
	if( found_p == -1 ){
		found_p = player;
		found_c = create_legacy_effect(player, card, &spiny_starfish_legacy);
		card_instance_t *tok = get_card_instance(found_p, found_c);
		tok->targets[0].player = player;
		tok->targets[0].card = card;
		tok->number_of_targets = 1;
	}
	card_instance_t *tok = get_card_instance(found_p, found_c);
	if( tok->targets[1].player < 0 ){
		tok->targets[1].player = 0;
	}
	tok->targets[1].player++;
	tok->targets[1].card = in_play(player, card) ? get_sleighted_color_test(player, card, COLOR_TEST_BLUE) : COLOR_TEST_BLUE;
}

int card_spiny_starfish(int player, int card, event_t event){
/* Spiny Starfish	|2|U
 * Creature - Starfish 0/1
 * |U: Regenerate ~.
 * At the beginning of each end step, if ~ regenerated this turn, put a 0/1 |Sblue Starfish creature token onto
 * 		the battlefield for each time it regenerated this turn. */


	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		if( can_regenerate(instance->parent_controller, instance->parent_card) ){
			regenerate_target(instance->parent_controller, instance->parent_card);
		}
	}

	return generic_activated_ability(player, card, event, GS_REGENERATION, MANACOST_U(1), 0, NULL, NULL);
}

int card_splinter_token(int player, int card, event_t event)
{
/*
Cumulative upkeep |G.
When Splinter leaves the battlefield, it deals 1 damage to you and each creature you control.
*/
	cumulative_upkeep(player, card, event, MANACOST_G(1));

	card_instance_t* instance;
	if(trigger_condition == TRIGGER_LEAVE_PLAY && (instance = get_card_instance(player, card)) &&
		instance->damage_source_player >= 0 && instance->damage_source_card >= 0 &&
		in_play(instance->damage_source_player, instance->damage_source_card) &&
		leaves_play(player, card, event))
	{
		new_damage_all(player, card, instance->damage_source_player, 1, NDA_PLAYER_TOO, NULL);
	}
	return 0;
}

int card_splintering_wind(int player, int card, event_t event)
{
  /* Splintering Wind	|2|G|G
   * Enchantment
   * |2|G: ~ deals 1 damage to target creature. Put a 1/1 |Sgreen Splinter creature token onto the battlefield.
   * It has flying and "Cumulative upkeep |G." When it leaves the battlefield, it deals 1 damage to you and each creature you control. */

	if( !IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	if (event == EVENT_RESOLVE_ACTIVATION && valid_target(&td)){
		damage_target0(player, card, 1);
		token_generation_t token;
		default_token_definition(player, card, CARD_ID_SPLINTER_TOKEN, &token);
		token.pow = token.tou = 1;
		token.color_forced = COLOR_TEST_GREEN;
		token.key_plus = KEYWORD_FLYING;
		token.action = TOKEN_ACTION_DONT_COPY_TOKEN_SOURCE;
		generate_token(&token);
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_XG(2,1), 0, &td, "TARGET_CREATURE");
}

int card_surge_of_strength(int player, int card, event_t event){
/* CARD_ID_SURGE_OF_STRENGTH	2363
Surge of Strength	|R|G
Instant
As an additional cost to cast ~, discard a |Sred or |Sgreen card.
Target creature gains trample and gets +X/+0 until end of turn, where X is that creature's converted mana cost. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	char msg[100] = "Select a green or red card.";
	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_ANY, msg);
	this_test.color = COLOR_TEST_GREEN | COLOR_TEST_RED;
	this_test.zone = TARGET_ZONE_HAND;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_CAST &&  generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL) ){
		return check_battlefield_for_special_card(player, card, player, 0, &this_test);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		if( pick_target(&td, "TARGET_CREATURE") ){
			int selected = new_select_a_card(player, player, TUTOR_FROM_HAND, 0, AI_MIN_VALUE, -1, &this_test);
			if( selected != -1 ){
				discard_card(player, selected);
			}
			else{
				spell_fizzled = 1;
			}
		}
	}


	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			int cmc = get_cmc(instance->targets[0].player, instance->targets[0].card);
			pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, cmc, 0, KEYWORD_TRAMPLE, 0);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_ali_from_cairo(int player, int card, event_t event);
int card_sustaining_spirit(int player, int card, event_t event)
{
  /* Sustaining Spirit	|1|W
   * Creature - Angel Spirit 0/3
   * Cumulative upkeep |1|W
   * Damage that would reduce your life total to less than 1 reduces it to 1 instead. */

  cumulative_upkeep(player, card, event, MANACOST_XW(1,1));

  return card_ali_from_cairo(player, card, event);
}

int card_swamp_mosquito(int player, int card, event_t event){
/* CARD_ID_SWAMP_MOSQUITO	2365
Swamp Mosquito	|1|B
Creature - Insect 0/1
Flying
Whenever ~ attacks and isn't blocked, defending player gets a poison counter. (A player with ten or more poison counters loses the game.) */

	if( event == EVENT_DECLARE_BLOCKERS ){
		if( is_unblocked(player, card) && ! is_humiliated(player, card) ){
			poison(1-player, 1);
		}
	}
	return 0;
}

int card_taste_of_paradise(int player, int card, event_t event){
/* CARD_ID_TASTE_OF_PARADISE	2367
Taste of Paradise	|3|G
Sorcery
As an additional cost to cast ~, you may pay |1|G any number of times.
You gain 3 life plus an additional 3 life for each additional |1|G you paid. */

	multikicker(player, card, event, MANACOST_XG(1, 1));

	if( event == EVENT_RESOLVE_SPELL ){
		card_instance_t *instance = get_card_instance(player, card);
		gain_life(player, (instance->info_slot+1)*3);
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
}

int card_thawing_glaciers(int player, int card, event_t event){
/* Thawing Glaciers	""
 * Land
 * ~ enters the battlefield tapped.
 * |1, |T: Search your library for a basic land card, put that card onto the battlefield tapped, then shuffle your library.
		Return ~ to its owner's hand at the beginning of the next cleanup step. */

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (50-count_subtype(player, TYPE_LAND, -1)*5);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		play_land_sound_effect(player, card);
	}

	comes_into_play_tapped(player, card, event);

	if( event == EVENT_ACTIVATE ){
		ai_modifier += (50-count_subtype(player, TYPE_LAND, -1)*5);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		card_instance_t *instance = get_card_instance(player, card);
		get_card_instance(instance->parent_controller, instance->parent_card)->info_slot = 66;
		tutor_basic_land(player, 1, 1);
	}

	if( get_card_instance(player, card)->info_slot == 66 && ! is_humiliated(player, card) && eot_trigger(player, card, event) ){
		bounce_permanent(player, card);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED, MANACOST_X(1), 0, NULL, NULL);
}

static int can_rfg_top_n_cards_of_deck(int player, int card, int number_of_age_counters)
{
  return count_deck(player) >= number_of_age_counters;
}
static int rfg_top_n_cards_of_deck_thunk(int player, int card, int number_of_age_counters)
{
  rfg_top_n_cards_of_deck(player, number_of_age_counters);
  return 1;
}
int card_thought_lash(int player, int card, event_t event)
{
	/* Cumulative upkeep-Exile the top card of your library.
	 * When a player doesn't pay ~'s cumulative upkeep, that player exiles all cards from his or her library.
	 * Exile the top card of your library: Prevent the next 1 damage that would be dealt to you this turn. */

	if( cumulative_upkeep_general(player, card, event, can_rfg_top_n_cards_of_deck, rfg_top_n_cards_of_deck_thunk) ){
		rfg_whole_library(player);
	}

	if( !IS_ACTIVATING(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.extra = damage_card;
	td.special = TARGET_SPECIAL_DAMAGE_PLAYER;

	card_instance_t *instance= get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		if( deck_ptr[player][0] != -1 ){
			return generic_activated_ability(player, card, event, GAA_DAMAGE_PREVENTION | GAA_CAN_TARGET, MANACOST_X(1), 0, &td, NULL);
		}
	}

	if( event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_DAMAGE_PREVENTION | GAA_CAN_TARGET | GAA_LITERAL_PROMPT,
									MANACOST_X(1), 0, &td, "Select target damage card attached to you");
		if( spell_fizzled != 1 ){
			rfg_top_card_of_deck(player);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			card_instance_t *target = get_card_instance(instance->targets[0].player, instance->targets[0].card);
			prevent_damage(target->targets[0].player, target->targets[0].card, 1);
		}
	}

	return 0;
}

int card_tornado(int player, int card, event_t event)
{
/* Tornado	|4|G
 * Enchantment
 * Cumulative upkeep |G
 * |2|G, Pay 3 life for each velocity counter on ~: Destroy target permanent and put a velocity counter on ~.
 * 		Activate this ability only once each turn. */

	cumulative_upkeep(player, card, event, MANACOST_G(1));

	if( !IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_PERMANENT);

	if (event == EVENT_RESOLVE_ACTIVATION && valid_target(&td))
	{
		card_instance_t* instance = get_card_instance(player, card);
		kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		add_counter(instance->parent_controller, instance->parent_card, COUNTER_VELOCITY);
	}

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET|GAA_ONCE_PER_TURN, MANACOST_XG(2,1),
									3 * count_counters(player, card, COUNTER_VELOCITY), &td, "TARGET_PERMANENT");
}

int card_unlikely_alliance(int player, int card, event_t event){
/*
Unlikely Alliance |1|W
Enchantment, 1W
{1}{W}: Target nonattacking, nonblocking creature gets +0/+2 until end of turn.
*/
	if( !IS_GAA_EVENT(event) ){
		return global_enchantment(player, card, event);
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE );
	td.preferred_controller = player;
	td.illegal_state = TARGET_STATE_IN_COMBAT;

	return vanilla_creature_pumper(player, card, event, MANACOST_XW(1, 1), 0, 0, 2, 0, 0, &td);
}

static int rtrue(int player, int card, int number_of_age_counters)
{
  return 1;
}

static int generate_survivors(int player, int card, int number_of_age_counters)
{
	token_generation_t token;
	default_token_definition(player, card, CARD_ID_SURVIVOR, &token);
	token.t_player = 1-player;
	token.pow = token.tou = 1;
	token.color_forced = COLOR_TEST_RED;
	token.qty = number_of_age_counters;
	generate_token(&token);
	return 1;
}

int card_varchilds_war_riders(int player, int card, event_t event)
{
  /* Varchild's War-Riders	|1|R
   * Creature - Human Warrior 3/4
   * Cumulative upkeep-Put a 1/1 |Sred Survivor creature token onto the battlefield under an opponent's control.
   * Trample; rampage 1 */

  cumulative_upkeep_general(player, card, event, rtrue, generate_survivors);

  rampage(player, card, event, 1);

  return 0;
}

static const char* whip_vine_is_blocking_him(int who_chooses, int player, int card, int targeting_player, int targeting_card)
{
  if (is_blocking(targeting_card, card))
	return NULL;
  else
	return "must be blocked by Whip Vine";
}


int card_whip_vine(int player, int card, event_t event){
/*
Whip Vine |2|G
Creature — Plant Wall 1/4
Defender; reach (This creature can block creatures with flying.)
You may choose not to untap Whip Vine during your untap step.
{T}: Tap target creature with flying blocked by Whip Vine. That creature doesn't untap during its controller's untap step for
	as long as Whip Vine remains tapped.
*/

	choose_to_untap(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.allowed_controller = 1-player;
	td.required_abilities = KEYWORD_FLYING;
	td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
	td.extra = (int32_t)whip_vine_is_blocking_him;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			tap_card(instance->targets[0].player, instance->targets[0].card);
			does_not_untap_until_im_tapped(instance->parent_controller, instance->parent_card,
											instance->targets[0].player, instance->targets[0].card);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED | GAA_CAN_TARGET, MANACOST0, 0,
									&td, "Select target creature with flying blocked by Whip Vine.");
}

int card_wild_aesthir(int player, int card, event_t event)
{
  // 0x1202070

  /* Wild Aesthir	|2|W
   * Creature - Bird 1/1
   * Flying, first strike
   * |W|W: ~ gets +2/+0 until end of turn. Activate this ability only once each turn. */

#define GAA(event)	generic_activated_ability(player, card, (event), GAA_ONCE_PER_TURN, MANACOST_W(2), 0, NULL, NULL)

  if (event == EVENT_POW_BOOST && GAA(EVENT_CAN_ACTIVATE))
	return 2;

  if (!IS_GAA_EVENT(event))
	return 0;

  if (event == EVENT_RESOLVE_ACTIVATION)
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  int p = instance->parent_controller, c = instance->parent_card;
	  if (in_play(p, c))
		pump_until_eot(p, c, p, c, 2,0);
	}

  return GAA(event);
#undef GAA
}

int card_whirling_catapult(int player, int card, event_t event){
/*
Whirling Catapult |4
Artifact
{2}, Exile the top two cards of your library: Whirling Catapult deals 1 damage to each creature with flying and each player.
*/
	if( event == EVENT_CAN_ACTIVATE && count_deck(player) > 1 ){
		return generic_activated_ability(player, card, event, 0, MANACOST_X(2), 0, NULL, NULL);
	}

	if(event == EVENT_ACTIVATE ){
		charge_mana_for_activated_ability(player, card, MANACOST_X(2));
		if( spell_fizzled != 1 ){
			rfg_top_card_of_deck(player);
			rfg_top_card_of_deck(player);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.keyword = KEYWORD_FLYING;
		card_instance_t *instance = get_card_instance( player, card );
		APNAP(p, {new_damage_all(instance->parent_controller, instance->parent_card, p, 1, NDA_PLAYER_TOO, &this_test);};);
	}

	return 0;
}

int card_yavimaya_ants(int player, int card, event_t event){

	/* Yavimaya Ants	|2|G|G
	 * Creature - Insect 5/1
	 * Trample, haste
	 * Cumulative upkeep |G|G */

	haste(player, card, event);

	cumulative_upkeep(player, card, event, MANACOST_G(2));

	return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20a91b0; original cards/alliances.c:3847. */
int card_yavimaya_ancients(int player, int card, event_t event)
{
  int result;
  result = generic_shade_merge_pt(player, card, event, 0, 0, 0, 0, 1, 0, 0, 1, -2);
  return result;
}

/* ManalinkEh 2022: 0x20a9640; original cards/alliances.c:3937. */
int card_benthic_explorers(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  color_test_t available;
  target_definition_t td;
  default_target_definition(player, card, &td, 1);
  td.required_state = 1;
  td.allowed_controller = 1 - player;
  td.illegal_abilities = 0;
  td.preferred_controller = td.allowed_controller;
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0, 0,
                                       &td, NULL);
    return (unsigned int)(result != 0);
  }
  if (event == EVENT_ACTIVATE)
  {
    instance = get_card_instance(player, card);
    result = generic_activated_ability(player, card, EVENT_ACTIVATE, 7, 0, 0, 0, 0, 0, 0, 0, &td,
                                       "Select a tapped land an opponent controls.");
    if (result != 0)
    {
      untap_card(instance->targets[0].player, instance->targets[0].card);
      available = get_colors_of_mana_land_could_produce_ignoring_costs(instance->targets[0].player,
                                                                       instance->targets[0].card);
      if ((available & COLOR_TEST_ARTIFACT) == COLOR_TEST_0)
      {
        if (available == COLOR_TEST_0)
        {
          return 0;
        }
      }
      else
      {
        available = available & ~COLOR_TEST_ARTIFACT | COLOR_TEST_COLORLESS;
      }
      produce_mana_all_one_color(player, available, 1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20a9810; original cards/alliances.c:3984. */
int card_burnout(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td_spell;
  if (event == EVENT_CAN_CAST)
  {
    counterspell_target_definition(player, card, &td_spell, 0x30);
    result = counterspell(player, card, EVENT_CAN_CAST, &td_spell, 0);
    if (result != 0)
    {
      return 99;
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      counterspell_target_definition(player, card, &td_spell, 0x30);
      if ((card == affected_card) &&
          ((player == affected_card_controller &&
            (result = counterspell(player, card, EVENT_CAN_CAST, &td_spell, 0), result != 0))))
      {
        result = counterspell(player, card, EVENT_CAST_SPELL, &td_spell, 0);
        return result;
      }
    }
  }
  else if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
           (event == EVENT_CHANGE_TARGET))
  {
    counterspell_target_definition(player, card, &td_spell, 0x30);
    if (event == EVENT_RESOLVE_SPELL)
    {
      instance = get_card_instance(player, card);
      td_spell.required_color = get_sleighted_color_test(player, card, 4);
      result = counterspell_validate(player, card, &td_spell, 0);
      if (result != 0)
      {
        real_counter_a_spell(player, card, instance->targets[0].player, instance->targets[0].card);
        cantrip(player, card, 1);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20a9e30; original cards/alliances.c:4106. */
int card_enslaved_scout(int player, int card, event_t event)
{
  int result;
  result = get_hacked_walk(player, card, 8);
  result = generic_shade(player, card, event, 0, 2, 0, 0, 0, 0, 0, 0, 0, result, 0);
  return result;
}

/* ManalinkEh 2022: 0x20a9ec0; original cards/alliances.c:4115. */
int card_errand_of_duty(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_token_definition(player, card, 0x4958, &token);
    token.key_plus = 0x40;
    token.pow = 1;
    token.tou = 1;
    token.qty = '\x01';
    generate_token(&token);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20a9f50; original cards/alliances.c:4135. */
int card_fevered_strength(int player, int card, event_t event)
{
  int result;
  int legacycard;

  result = vanilla_instant_pump_2022(player, card, event, 2, player, 2, 0, 0x4000000, 0);
  if ((result != 0) && (event == EVENT_RESOLVE_SPELL))
  {
    legacycard = cantrip(player, card, 1);
    alternate_legacy_text(2, player, legacycard);
    kill_card(player, card, KILL_DESTROY);
    return result;
  }
  return result;
}

/* ManalinkEh 2022: 0x20aa000; original cards/alliances.c:4150. */
int card_floodwater_dam(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_t *ptVar4;
  target_t *ptVar5;
  target_definition_t td;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 1);
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar3 = has_mana_for_activated_ability_2022(player, card, 3, 0, 0, 0, 0, 0);
      if (iVar3 == 0)
      {
        return 0;
      }
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, -1, 0, 0, 0, 0, 0, 0,
                                        &td, NULL);
      return iVar3;
    }
    if (event == EVENT_ACTIVATE)
    {
      instance->number_of_targets = '\0';
      iVar3 = 0;
      while (
          ((iVar2 = can_target(&td), iVar2 != 0 && (iVar2 = has_mana_for_activated_ability_2022(
                                                        player, card, iVar3 * 2 + 1, 0, 0, 0, 0, 0),
                                                    iVar2 != 0)) &&
           (iVar2 = new_pick_target(&td, "TARGET_LAND", iVar3, 0), iVar2 != 0)))
      {
        ptVar5 = instance->targets + iVar3;
        ptVar4 = instance->targets + iVar3;
        iVar3 = iVar3 + 1;
        state_untargettable(ptVar4->player, ptVar5->card, 1);
      }
      if (iVar3 < 1)
      {
        return 0;
      }
      ptVar4 = instance->targets;
      do
      {
        ptVar5 = ptVar4 + 1;
        state_untargettable(ptVar4->player, ptVar4->card, 0);
        ptVar4 = ptVar5;
      } while (ptVar5 != instance->targets + iVar3);
      iVar3 = charge_mana_for_activated_ability_2022(player, card, iVar3 * 2 + 1, 0, 0, 0, 0, 0);
      if (iVar3 == 0)
      {
        return 0;
      }
      tap_card(player, card);
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 1);
    instance = get_card_instance(player, card);
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (instance->number_of_targets != '\0'))
  {
    iVar3 = 0;
    do
    {
      while (iVar2 = validate_target(player, card, &td, iVar3), iVar2 != 0)
      {
        ptVar5 = instance->targets + iVar3;
        ptVar4 = instance->targets + iVar3;
        iVar3 = iVar3 + 1;
        tap_card(ptVar4->player, ptVar5->card);
        if ((int)(unsigned int)instance->number_of_targets <= iVar3)
        {
          return 0;
        }
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)(unsigned int)instance->number_of_targets);
  }
  return 0;
}

/* ManalinkEh 2022: 0x20aa310; original cards/alliances.c:4206. */
int card_foresight(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event != EVENT_RESOLVE_SPELL)
    goto LAB_020aa328;
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    result = count_subtype(player, 1, -1);
    if (4 < result)
    {
      result = count_deck_by_type(player, 1);
      if (0 < result)
      {
        new_default_test_definition(&test, 1, "Select a card to exile.");
        ai_modifier = ai_modifier + 0x40;
        goto LAB_020aa3e6;
      }
    }
    new_default_test_definition(&test, 0xff, "Select a card to exile.");
    ai_modifier = ai_modifier + 0x20;
  }
  else
  {
    new_default_test_definition(&test, 0xff, "Select a card to exile.");
  }
LAB_020aa3e6:
  test.qty = 3;
  new_global_tutor(player, player, 1, 6, 0, 3, &test);
  cantrip(player, card, 1);
  kill_card(player, card, KILL_DESTROY);
LAB_020aa328:
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20aa740; original cards/alliances.c:4325. */
int card_insidious_bookworms(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  duh_mode(player);
  result = has_mana_multi(player, 1, 1, 0, 0, 0, 0);
  result = this_dies_trigger(player, card, event, (unsigned int)(result != 0));
  if (result != 0)
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    result = can_target(&td);
    if (result != 0)
    {
      result = charge_mana_multi_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player, 1, 1,
                                                 0, 0, 0, 0);
      if (result != 0)
      {
        result = pick_target(&td, "TARGET_PLAYER");
        if (result != 0)
        {
          instance = get_card_instance(player, card);
          discard(instance->targets[0].player, 1, player);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20aaf20; original cards/alliances.c:4461. */
int card_mishras_groundbreaker(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 1);
    td.illegal_type = 2;
    td.preferred_controller = player;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 1);
    td.illegal_type = 2;
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      land_animation2(instance->parent_controller, instance->parent_card,
                      instance->targets[0].player, instance->targets[0].card, 0, 3, 3, 0, 0, 0,
                      NULL);
      recalculate_all_cards_in_play();
      turn_into_artifact(instance->parent_controller, instance->parent_card,
                         instance->targets[0].player, instance->targets[0].card, 0);
    }
  }
  result = generic_activated_ability(player, card, event, 0xf, 0, 0, 0, 0, 0, 0, 0, &td,
                                     "Select target land it becomes a 3/3 artifact creature.");
  return result;
}

/* ManalinkEh 2022: 0x20ab0e0; original cards/alliances.c:4492. */
int card_misinformation(int player, int card, event_t event)
{
  int result;
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
    else if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
             (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  new_default_test_definition(&this_test, 0xff,
                              "Select up to 3 cards to put on top off opponent\'s library.");
  result = spell_puts_up_to_x_target_cards_from_graveyard_on_top_of_deck(
      player, card, event, 1 - player, 3, &this_test, 0);
  return result;
}

/* ManalinkEh 2022: 0x209d490; original cards/alliances.c:4630. */
static int is_land_or_white_permanent(int unused, int unused2, int player, int card)
{
  int result;
  subtype_t subtype;
  result = is_what(player, card, 1);
  if (result != 0)
  {
    subtype = get_hacked_subtype(player, card, 0x5007);
    result = has_subtype(player, card, subtype);
    if (result != 0)
    {
      return 1;
    }
  }
  result = get_sleighted_color_test(player, card, 0x20);
  return (unsigned int)(result != 0);
}

/* ManalinkEh 2022: 0x20abf90; original cards/alliances.c:4745. */
int card_royal_herbalist(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = can_use_activated_abilities_2022(player, card);
    if (((result != 0) &&
         (result = has_mana_for_activated_ability_2022(player, card, 2, 0, 0, 0, 0, 0), result != 0)) &&
        (*deck_ptr[player] != -1))
    {
      return 1;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = charge_mana_for_activated_ability_2022(player, card, 2, 0, 0, 0, 0, 0);
    if (result != 0)
    {
      rfg_top_card_of_deck(player);
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    gain_life(player, 1);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x20ac0c0; original cards/alliances.c:4774. */
int card_scarab_of_the_unseen(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  test_definition_t this_test;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = player | 4;
    td.preferred_controller = td.allowed_controller;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x47);
    td.allowed_controller = player | 4;
    td.preferred_controller = td.allowed_controller;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      instance = get_card_instance(player, card);
      result = valid_target(&td);
      if (result != 0)
      {
        new_default_test_definition(&this_test, 4, "");
        this_test.owner = (unsigned char)player | 4;
        manipulate_auras_enchanting_target(player, card, instance->targets[0].player,
                                           instance->targets[0].card, NULL, ACT_BOUNCE);
        cantrip(player, card, 1);
      }
    }
  }
  result = generic_activated_ability(
      player, card, event, 0xf, 0, 0, 0, 0, 0, 0, 0, &td,
      "Select target permanent you own, return all auras attached to it to their owners\' hands.");
  return result;
}

/* ManalinkEh 2022: 0x20ac5d0; original cards/alliances.c:4864. */
int card_seasoned_tactician(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0xff);
    td.extra = damage_card;
    td.special = 0x20;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_use_activated_abilities_2022(player, card);
      if ((((result != 0) && (result = can_target(&td), result != 0)) &&
           (result = has_mana_for_activated_ability_2022(player, card, 3, 0, 0, 0, 0, 0),
            result != 0)) &&
          (result = count_deck(player), 3 < result))
      {
        return 99;
      }
      get_card_instance(player, card);
      return 0;
    }
    if (event == EVENT_ACTIVATE)
    {
      result = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x1000004, 3, 0, 0, 0, 0, 0,
                                         0, &td, "TARGET_DAMAGE");
      if (result != 0)
      {
        rfg_top_card_of_deck(player);
        rfg_top_card_of_deck(player);
        rfg_top_card_of_deck(player);
        rfg_top_card_of_deck(player);
      }
      get_card_instance(player, card);
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0xff);
  }
  td.special = 0x20;
  td.extra = damage_card;
  instance = get_card_instance(player, card);
  if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
  {
    prevent_damage(instance->targets[0].player, instance->targets[0].card, -1);
  }
  return 0;
}

/* ManalinkEh 2022: 0x209cb70; original cards/alliances.c:4905. */
static int is_black_or_artifact_creature(int unused, int unused2, int player, int card)
{
  int result;
  result = is_what(player, card, 2);
  if (result != 0)
  {
    result = get_sleighted_color_test(player, card, 2);
    if (result != 0)
    {
      return 1;
    }
  }
  result = is_what(player, card, 2);
  if (result == 0)
  {
    return 0;
  }
  result = is_what(player, card, 0x40);
  return (unsigned int)(result != 0);
}

/* ManalinkEh 2022: 0x20aca60; original cards/alliances.c:4951. */
int card_soldevi_heretic(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  target_definition_t td2;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.extra = damage_card;
    td.required_type = 0;
    td.special = 0x100000;
    default_target_definition(player, card, &td2, 0);
    td2.zone = 0x1000;
    instance = get_card_instance(player, card);
    instance->targets[1].player = 1 - player;
    instance->targets[1].card = -1;
    instance->number_of_targets = '\x01';
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.extra = damage_card;
    td.required_type = 0;
    td.special = 0x100000;
    default_target_definition(player, card, &td2, 0);
    td2.zone = 0x1000;
    instance = get_card_instance(player, card);
    instance->targets[1].player = 1 - player;
    instance->targets[1].card = -1;
    instance->number_of_targets = '\x01';
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        prevent_damage(instance->targets[0].player, instance->targets[0].card, 2);
        result = would_validate_arbitrary_target(&td2, 1 - player, -1);
        if (result != 0)
        {
          draw_some_cards_if_you_want(instance->parent_controller, instance->parent_card,
                                      instance->targets[1].player, 1);
        }
      }
    }
  }
  result = generic_activated_ability(player, card, event, 0x4000005, 0, 0, 0, 0, 0, 1, 0, &td,
                                     "TARGET_DAMAGE");
  return result;
}

/* ManalinkEh 2022: 0x20ad430; original cards/alliances.c:5140. */
int card_stench_of_decay(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&test, 2);
    test.type_flag = 9;
    pump_creatures_until_eot(player, card, 2, 0, -1, -1, 0, 0, &test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x209d350; original cards/alliances.c:5284. */
static char *is_blocking_sd(int who_chooses, int player, int card, int targeting_player,
                            int targeting_card)
{
  int result;
  result = is_blocking(card, targeting_card);
  if (result == 0)
  {
    return "must be blocking Sworn Defender";
  }
  return NULL;
}

/* ManalinkEh 2022: 0x209d320; original cards/alliances.c:5292. */
static char *sd_is_blocking_him(int who_chooses, int player, int card, int targeting_player,
                                int targeting_card)
{
  int result;
  result = is_blocking(targeting_card, card);
  if (result == 0)
  {
    return "must be blocked by Sworn Defender";
  }
  return NULL;
}

/* ManalinkEh 2022: 0x209d780; original cards/alliances.c:5501. */
static int sac_next_endstep(int player, int card, event_t event)
{
  card_instance_t *instance;
  if ((event != EVENT_CLEANUP) && (event != EVENT_SHOULD_AI_PLAY))
  {
    return 0;
  }
  instance = get_card_instance(player, card);
  kill_card((int)instance->damage_target_player, instance->damage_target_card, KILL_SACRIFICE);
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x209d5c0; original cards/alliances.c:5725. */
static char *damage_to_cleric_or_wizard(int who_chooses, int player, int card, int targeting_player,
                                        int targeting_card)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if ((((instance->internal_card_id == damage_card) && (instance->damage_target_card != -1)) &&
       (result = is_what((int)instance->damage_target_player, instance->damage_target_card, 2),
        result != 0)) &&
      ((result = has_subtype((int)instance->damage_target_player, instance->damage_target_card,
                             SUBTYPE_CLERIC),
        result != 0 || (result = has_subtype((int)instance->damage_target_player,
                                             instance->damage_target_card, SUBTYPE_WIZARD),
                        result != 0))))
  {
    return NULL;
  }
  return "must be damage to a Cleric or Wizard creature";
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20ab1a0; original cards/alliances.c:4508. */
int card_mystic_compass(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int card_00;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    get_card_instance(player, card);
    default_target_definition(player, card, &td, 1);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 1);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      result = recorded_rand(player, 5);
      result = choose_a_land(player, result + 1);
      card_00 = create_targetted_legacy_effect(
          player, card, fx_change_landtype, instance->targets[0].player, instance->targets[0].card);
      instance = get_card_instance(player, card_00);
      instance->targets[1].player = result;
    }
  }
  result = generic_activated_ability(
      player, card, event, 7, 1, 0, 0, 0, 0, 0, 0, &td,
      "Select target land it becomes the basic land type of your choice.");
  return result;
}

/* ManalinkEh 2022: 0x20aba20; original cards/alliances.c:4639. */
int card_omen_of_fire(int player, int card, event_t event)
{
  int result;
  const char *pcVar2;
  unsigned int t_player;
  test_definition_t *this_test_00;
  unsigned short uVar3;
  unsigned int uStack_300;
  char buffer[100];
  test_definition_t this_test;
  test_definition_t this_test2;
  test_definition_t this_test4;

  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 1);
    this_test.subtype = get_hacked_subtype(player, card, 0x5002);
    new_manipulate_all(player, card, 2, &this_test, ACT_BOUNCE);
    default_test_definition(&this_test2, 0x47);
    this_test2.color = get_sleighted_color_test(player, card, 0x20);
    uStack_300 = 0;
    do
    {
      t_player = uStack_300 ^ current_turn;
      this_test_00 = &this_test2;
      result = check_battlefield_for_special_card(player, card, t_player, 4, this_test_00);
      pcVar2 = get_hacked_land_text(player, card, "Select %a land", 0x5007, this_test_00, t_player,
                                    result);
      uVar3 = (unsigned short)result;
      result = scnprintf(buffer, 100, pcVar2);
      pcVar2 = get_sleighted_color_text(player, card, " or a %s permanent card to sacrifice.", 5);
      scnprintf(buffer + result, 100 - result, pcVar2);
      new_default_test_definition(&this_test4, 0x47, buffer);
      this_test4.qty = uVar3;
      this_test4.special_selection_function = is_land_or_white_permanent;
      new_sacrifice(player, card, t_player, SAC_CAUSED | SAC_NO_CANCEL, &this_test4);
      uStack_300 = uStack_300 + 1;
    } while (uStack_300 != 2);
    kill_card(player, card, KILL_SACRIFICE);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20adab0; original cards/alliances.c:5300. */
int card_sworn_defender(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t *td_00;
  const char *prompt;
  target_definition_t td;
  target_definition_t td1;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    td_00 = &td;
    default_target_definition(player, card, td_00, 2);
    iVar1 = 1 - player;
    td.special = 0x8000;
    td.extra = (int32_t)is_blocking_sd;
    td.allowed_controller = iVar1;
    td.preferred_controller = iVar1;
    default_target_definition(player, card, &td1, 2);
    td1.special = 0x8000;
    td1.extra = (int32_t)sd_is_blocking_him;
    td1.allowed_controller = iVar1;
    td1.preferred_controller = iVar1;
    if (event == EVENT_CAN_ACTIVATE)
    {
      if (current_phase != 0x18)
      {
        return 0;
      }
      iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 1, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if (iVar1 == 0)
      {
        return 0;
      }
      if (current_turn == player)
      {
        iVar1 = can_target(td_00);
        return iVar1;
      }
      iVar1 = can_target(&td1);
      return iVar1;
    }
    if (event == EVENT_ACTIVATE)
    {
      instance = get_card_instance(player, card);
      instance->number_of_targets = '\0';
      iVar1 = charge_mana_for_activated_ability_2022(player, card, 1, 0, 0, 0, 0, 0);
      if (iVar1 == 0)
      {
        return 0;
      }
      if (current_turn == player)
      {
        prompt = "Select target creature blocking Sworn Defender.";
      }
      else
      {
        prompt = "Select target creature blocked by Sworn Defender.";
        td_00 = &td1;
      }
      iVar1 = select_target(player, card, td_00, prompt, instance->targets);
      if (iVar1 != 0)
      {
        iVar3 = instance->targets[1].player;
        iVar1 = 1;
        if (-1 < iVar3)
        {
          iVar1 = iVar3 + 1;
        }
        instance->targets[1].player = iVar1;
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
    default_target_definition(player, card, &td, 2);
    iVar1 = 1 - player;
    td.special = 0x8000;
    td.extra = (int32_t)is_blocking_sd;
    td.allowed_controller = iVar1;
    td.preferred_controller = iVar1;
    default_target_definition(player, card, &td1, 2);
    td1.preferred_controller = iVar1;
  }
  td1.special = 0x8000;
  td1.extra = (int32_t)sd_is_blocking_him;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    td1.allowed_controller = td1.preferred_controller;
    instance = get_card_instance(player, card);
    if (current_turn == player)
    {
      iVar1 = valid_target(&td);
      if (iVar1 != 0)
        goto LAB_020adce1;
      if (current_turn == player)
      {
        return 0;
      }
    }
    iVar1 = valid_target(&td1);
    if (iVar1 != 0)
    {
    LAB_020adce1:
      iVar1 = get_toughness(instance->targets[0].player, instance->targets[0].card);
      iVar3 = get_power(instance->targets[0].player, instance->targets[0].card);
      set_pt_and_abilities_until_eot(instance->parent_controller, instance->parent_card,
                                     instance->parent_controller, instance->parent_card, iVar1 + -1,
                                     iVar3 + 1, 0, 0, 0);
      return 0;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x209cc10; original cards/alliances.c:4274. */
static void boost_tough_gain_1_life(int player, int card, int t_player, int t_card)
{
  card_instance_t *instance;

  instance = get_card_instance(player, card);
  pump_ability_until_eot_2022(player, card, (int)instance->damage_target_player,
                         instance->damage_target_card, 0, 3, 0, 0);
  gain_life(player, 1);
  return;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *not_the_enchanted(int who_chooses, int player, int card, int targeting_player,
                               int targeting_card);

/* ManalinkEh 2022: 0x209c790; original cards/alliances.c:4347. */
static char *not_the_enchanted(int who_chooses, int player, int card, int targeting_player,
                               int targeting_card)
{
  card_instance_t *instance;

  instance = get_card_instance(targeting_player, targeting_card);
  if ((instance->damage_target_player == player) && (instance->damage_target_card == card))
  {
    return "can\'t be this enchanted creature.";
  }
  return NULL;
}

/* ManalinkEh 2022: 0x20aa880; original cards/alliances.c:4357. */
int card_kjeldoran_pride(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  instance = get_card_instance(player, card);
  if (instance->damage_target_player < '\0')
  {
  LAB_020aa8ab:
    if (event == EVENT_CHECK_PUMP)
    {
      result = vanilla_instant_pump(player, card, EVENT_CHECK_PUMP, 2, player, 1, 2, 0, 0);
      return result;
    }
    if (event == EVENT_CAN_CAST)
      goto LAB_020aa8d0;
    if (event != EVENT_CAST_SPELL)
      goto LAB_020aa8be;
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_020aa8d0;
  }
  else
  {

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.special = 0x8000;
    td.extra = (int32_t)not_the_enchanted;
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      result = generic_activated_ability(player, card, event, 6, 2, 0, 1, 0, 0, 0, 0, &td,
                                         "Select another target creature to attach this.");
      return result;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_020aa8ab;
    result = valid_target(&td);
    if (result != 0)
    {
      attach_aura_to_target_2022(player, instance->parent_card, EVENT_RESOLVE_ACTIVATION,
                            instance->targets[0].player, instance->targets[0].card);
    }
  LAB_020aa8be:
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
      goto LAB_020aa8d0;
  }
  if (((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
        ((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)))) &&
       (event != EVENT_CHANGE_TYPE)) &&
      (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
        (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_020aa8d0:
  result = generic_aura(player, card, event, player, 1, 2, 0, 0, 0, 0, 0);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20a9a00; original cards/alliances.c:4022. */
int card_casting_of_bones(int player, int card, event_t event)
{
  int32_t iVar1;
  card_instance_t *instance;
  int iVar3;
  int (*paiVar4)[100];
  unsigned int uVar5;
  unsigned char bVar6;
  int iStack_40c;
  test_definition_t this_test;
  int dtt[2][100];

  bVar6 = 0;
  instance = in_play(player, card);
  if ((((instance != NULL) &&
        (instance = get_card_instance(player, card), -1 < instance->damage_target_player)) &&
       (iVar3 = is_humiliated_for_ability(player, card, AL_TRIG), iVar3 == 0)) &&
      (iVar3 = attached_creature_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY),
       iVar3 != 0))
  {
    iStack_40c = 3;
    paiVar4 = dtt;

    for (iVar3 = 200; iVar3 != 0; iVar3 = iVar3 + -1)
    {
      (*paiVar4)[0] = 0;
      paiVar4 = (int (*)[100])((int)paiVar4 + ((unsigned int)bVar6 * -2 + 1) * 4);
    }
    uVar5 = 0xffffffff;
    do
    {

      iVar3 = draw_a_card(player);
      if ((-1 < iVar3) && (instance = in_hand(player, iVar3), instance != NULL))
      {
        uVar5 = uVar5 & (int)(uVar5 ^ 0xffffffff) >> 0x1f;
        instance = get_card_instance(player, iVar3);
        iVar1 = instance->internal_card_id;
        dtt[1][uVar5] = iVar3;
        dtt[0][uVar5] = iVar1;
        uVar5 = uVar5 + 1;
      }
      iStack_40c = iStack_40c + -1;
    } while (iStack_40c != 0);
    new_default_test_definition(&this_test, 0xff, "Select a card to discard.");

    if ((-1 < (int)uVar5) &&
        (iVar3 = select_card_from_zone(player, player, dtt[0], uVar5, 1, 3, -1, &this_test),
         iVar3 != -1))
    {
      discard_card(player, dtt[1][iVar3]);
    }
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_020a9a58;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_020a9a58;
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         ((((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
            (event != EVENT_CHANGE_TYPE)) &&
           ((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)))))) &&
        (event != EVENT_STATIC_EFFECTS))
    {
      return 0;
    }
  }
LAB_020a9a58:
  iVar3 = vanilla_aura_2022(player, card, event, player);
  return iVar3;
}

/* ManalinkEh 2022: 0x20aa520; original cards/alliances.c:4284. */
int card_gift_of_the_woods(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;
  int iVar5;

  instance = in_play(player, card);

  if (((instance == NULL) ||
       (instance = get_card_instance(player, card), instance->damage_target_player < '\0')) ||
      (instance = get_card_instance(player, card), event != EVENT_DECLARE_BLOCKERS))
  {
    if (event == EVENT_CAN_CAST)
      goto LAB_020aa578;
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_020aa578;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_020aa578;
  }
  else
  {
    cVar1 = instance->damage_target_player;
    iVar3 = instance->damage_target_card;
    iVar4 = is_humiliated_for_ability(player, card, AL_TRIG);
    if (iVar4 == 0)
    {
      iVar4 = (int)cVar1;
      if (current_turn == player)
      {
        iVar5 = is_attacking(iVar4, iVar3);
        if (iVar5 != 0)
        {
          for_each_creature_blocking_me(iVar4, iVar3, boost_tough_gain_1_life, player, card);
        }
        if (current_turn == player)
          goto LAB_020aa5a4;
      }
      iVar5 = blocking(iVar4, iVar3, EVENT_DECLARE_BLOCKERS);
      if (iVar5 != 0)
      {
        for_each_creature_blocked_by_me(iVar4, iVar3, boost_tough_gain_1_life, player, card);
      }
    }
  }
LAB_020aa5a4:
  if ((((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (2 < event - EVENT_POWER)) &&
        (event != EVENT_RESOLVE_MOVING_AURA)) &&
       ((event != EVENT_CHANGE_TYPE && (event != EVENT_SET_COLOR)))) &&
      ((event != EVENT_GET_SELECTED_CARD && (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_020aa578:
  iVar3 = vanilla_aura_2022(player, card, event, player);
  return iVar3;
}

/* ManalinkEh 2022: 0x20accd0; original cards/alliances.c:4988. */
int card_soldevi_sage(int player, int card, event_t event)
{
  int32_t iVar1;
  unsigned int uVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  test_definition_t *ptVar6;
  unsigned int uVar7;
  unsigned char bVar8;
  test_definition_t test;
  test_definition_t this_test;
  test_definition_t sac_test;
  int aiStack_1a0[100];

  bVar8 = 0;
  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_test_definition(&test, 1);
  LAB_020acd12:
    test.qty = 2;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      iVar5 = 3;
      ptVar6 = &sac_test;

      for (iVar4 = 200; iVar4 != 0; iVar4 = iVar4 + -1)
      {
        ptVar6->message[0] = '\0';
        ptVar6->message[1] = '\0';
        ptVar6->message[2] = '\0';
        ptVar6->message[3] = '\0';
        ptVar6 = (test_definition_t *)((int)ptVar6 + (unsigned int)bVar8 * -8 + 4);
      }
      uVar7 = 0xffffffff;
      do
      {

        iVar4 = draw_a_card(player);
        if ((-1 < iVar4) && (instance = in_hand(player, iVar4), instance != NULL))
        {
          uVar7 = uVar7 & (int)(uVar7 ^ 0xffffffff) >> 0x1f;
          instance = get_card_instance(player, iVar4);
          iVar1 = instance->internal_card_id;
          aiStack_1a0[uVar7] = iVar4;
          *(int32_t *)(sac_test.message + uVar7 * 4) = iVar1;
          uVar7 = uVar7 + 1;
        }
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      new_default_test_definition(&this_test, 0xff, "Select a card to discard.");

      if ((-1 < (int)uVar7) && (iVar4 = select_card_from_zone(player, player, (int *)&sac_test,
                                                              uVar7, 1, 3, -1, &this_test),
                                iVar4 != -1))
      {
        discard_card(player, aiStack_1a0[iVar4]);
      }
    }
    return 0;
  }
  default_test_definition(&test, 1);
  test.qty = 2;
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar4 = generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_CAN_ACTIVATE,
                                                             GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0,
                                                             NULL, NULL, TYPE_NONE, &test, SARV_0);
    return iVar4;
  }
  if (event != EVENT_ACTIVATE)
    goto LAB_020acd12;
  iVar4 =
      generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  if (iVar4 == 0)
    goto LAB_020acf68;

  new_default_test_definition(&sac_test, 1, "Select a land card to sacrifice.");
  uVar7 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                        &sac_test);
  if (uVar7 == 0)
  {
    uVar2 = 0xffffffff;
  LAB_020ad0ab:
    spell_fizzled = 1;
  }
  else
  {
    uVar2 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                          &sac_test);
    if (uVar2 == 0)
      goto LAB_020ad0ab;
  }
  if (-1 < (int)uVar7)
  {
    state_untargettable((int)uVar7 >> 0x10 & 0xff, uVar7 >> 0x18, 0);
  }
  if (-1 < (int)uVar2)
  {
    state_untargettable((int)uVar2 >> 0x10 & 0xff, uVar2 >> 0x18, 0);
  }
  if (spell_fizzled != 1)
  {
    kill_card((int)uVar7 >> 0x10 & 0xff, uVar7 >> 0x18, KILL_SACRIFICE);
    kill_card((int)uVar2 >> 0x10 & 0xff, uVar2 >> 0x18, KILL_SACRIFICE);
    return 0;
  }
LAB_020acf68:
  remove_state(player, card, 0x10);
  return 0;
}

/* ManalinkEh 2022: 0x20ad2c0; original cards/alliances.c:5109. */
int card_soldevi_steam_beast(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int tgt_player;
  target_definition_t td;

  if (((event != EVENT_ACTIVATE) && (1 < event - EVENT_RESOLVE_ACTIVATION)) &&
      (event != EVENT_CLEANUP))
  {
    if (((event == EVENT_TAP_CARD) && (card == affected_card)) &&
        ((player == affected_card_controller &&
          ((instance = in_play(player, card),
            instance != NULL &&
                (result = is_humiliated_for_ability(player, card, AL_TRIG), result == 0))))))
    {

      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      instance = get_card_instance(player, card);
      tgt_player = 1 - player;
      instance->targets[0].player = tgt_player;
      instance->targets[0].card = -1;
      instance->number_of_targets = '\x01';
      result = would_validate_arbitrary_target(&td, tgt_player, -1);
      if (result != 0)
      {
        gain_life_2022(tgt_player, 2);
      }
      instance->number_of_targets = '\0';
    }
    return 0;
  }
  result = regeneration(player, card, event, 2, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x20ad510; original cards/alliances.c:5158. */
int card_storm_cauldron(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = in_play(player, card);
  if (instance != NULL)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      check_playable_lands(current_turn);
    }
  }
  result = is_humiliated_for_ability(player, card, AL_TRIG);
  if (result == 0)
  {
    instance = in_play(player, card);
    if ((event == EVENT_TAP_CARD) && (instance != NULL))
    {
      result = is_what(affected_card_controller, affected_card, 1);
      if ((result != 0) && (tapped_for_mana_color != -1))
      {
        bounce_permanent(affected_card_controller, affected_card);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20ad5f0; original cards/alliances.c:5178. */
int card_storm_elemental(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  target_definition_t td;

  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar3 = can_use_activated_abilities(player, card);
    if (((iVar3 != 0) &&
         (iVar3 = has_mana_for_activated_ability(player, card, 0, 0, 1, 0, 0, 0), iVar3 != 0)) &&
        (*deck_ptr[player] != -1))
    {
      return 1;
    }
    default_target_definition(player, card, &td, 2);
    td.required_abilities = 0x20;
    get_card_instance(player, card);
  }
  else if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.required_abilities = 0x20;
    instance = get_card_instance(player, card);
    iVar3 = charge_mana_for_activated_ability(player, card, 0, 0, 1, 0, 0, 0);
    if (iVar3 != 0)
    {

      instance->number_of_targets = '\0';
      iVar3 = can_target(&td);
      iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Tap target creature", iVar3,
                          10, "Snowland boost", 1, 5, 0, 0, 0, 0);
      if (iVar3 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar3 == 1)
      {
        rfg_top_card_of_deck(player);
        new_pick_target(&td, "Select target creature with flying.", 0, 0x80);
      }
      else if (iVar3 == 2)
      {

        iVar3 = *deck_ptr[player];
        rfg_top_card_of_deck(player);
        if (((iVar3 == -1) || (iVar1 = is_what(-1, iVar3, 1), iVar1 == 0)) ||
            (iVar3 = is_snow_permanent(-1, iVar3), iVar3 == 0))
        {
          instance->targets[1].player = 0;
        }
        else
        {
          instance->targets[1].player = 1;
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {
    default_target_definition(player, card, &td, 2);
    td.required_abilities = 0x20;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      iVar3 = instance->info_slot;
      if (iVar3 == 1)
      {
        iVar3 = valid_target(&td);
        if (iVar3 != 0)
        {
          tap_card(instance->targets[0].player, instance->targets[0].card);
        }
        iVar3 = instance->info_slot;
      }
      if ((iVar3 == 2) && (instance->targets[1].player == 1))
      {
        pump_until_eot_merge_previous_2022(instance->parent_controller, instance->parent_card,
                                      instance->parent_controller, instance->parent_card, 1, 1);
        return 0;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20ade90; original cards/alliances.c:5385. */
int card_undergrowth(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      if ((current_turn == player) ||
          ((current_phase != 0x18 || (result = count_attackers(current_turn), result < 1))))
      {
        ai_modifier = ai_modifier + -0x30;
      }
      result = has_mana_multi(player, 2, 0, 0, 0, 1, 0);
      result = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\016", "Play normally",
                           1, 5, "Additional cost", result, 10, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (instance->info_slot == 2)
      {
        charge_mana_multi(player, 2, 0, 0, 0, 1, 0);
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {

    instance = get_card_instance(player, card);
    result = instance->info_slot;
    if (result == 1)
    {
      result = fog_effect(player, card);
      alternate_legacy_text(1, player, result);
      result = instance->info_slot;
    }
    if (result == 2)
    {
      result = fog_special(player, card, 2, 400);
      alternate_legacy_text(2, player, result);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20ae8f0; original cards/alliances.c:5588. */
int card_viscerid_armor(int player, int card, event_t event)
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
          goto LAB_020ae924;
        if (event == EVENT_RESOLVE_ACTIVATION)
        {
          bounce_permanent(instance->parent_controller, instance->parent_card);
        }
      }
      result = generic_activated_ability(player, card, event, 0, 1, 0, 1, 0, 0, 0, 0, NULL, NULL);
      return result;
    }
  }
LAB_020ae924:
  result = generic_aura_2022(player, card, event, player, 1, 1, 0, 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x20aea20; original cards/alliances.c:5610. */
int card_viscerid_drone(int player, int card, event_t event)
{
  card_instance_t *instance;
  const char *pcVar2;
  int iVar3;
  unsigned int uVar4;
  unsigned int uVar5;
  target_definition_t *ptVar6;
  int iVar7;
  unsigned int uStack_270;
  target_definition_t td;
  target_definition_t td2;
  test_definition_t test;
  test_definition_t test1;

  if (((event != EVENT_ACTIVATE) && (1 < event - EVENT_RESOLVE_ACTIVATION)) &&
      (event != EVENT_CLEANUP))
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  td.illegal_type = 0x40;
  default_target_definition(player, card, &td2, 2);
  instance = get_card_instance(player, card);
  pcVar2 = get_hacked_land_text(player, card, "Select a %s to sacrifice.", 0x5009);
  new_default_test_definition(&test, 1, pcVar2);
  test.subtype = get_hacked_subtype(player, card, 0x5009);
  pcVar2 = get_hacked_land_text(player, card, "Select a snow %s to sacrifice.", 0x5009);
  new_default_test_definition(&test1, 1, pcVar2);
  test1.subtype = get_hacked_subtype(player, card, 0x5009);
  test1.sub2 = 0x2003;
  test1.subtype_flag = 4;
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar3 = new_can_sacrifice(player, card, player, &test);
    if (iVar3 == 0)
    {
      iVar3 = new_can_sacrifice(player, card, player, &test1);
      if (iVar3 == 0)
      {
        return 0;
      }
      ptVar6 = &td2;
    }
    else
    {
      ptVar6 = &td;
    }
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x205, 0, 0, 0, 0, 0, 0, 0,
                                      ptVar6, NULL);
    return iVar3;
  }
  if (event != EVENT_ACTIVATE)
  {
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      return 0;
    }
    iVar3 = instance->info_slot;
    if (iVar3 == 1)
    {
      iVar3 = valid_target(&td);
      if (iVar3 == 0)
      {
        iVar3 = instance->info_slot;
      }
      else
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
        iVar3 = instance->info_slot;
      }
    }
    if (iVar3 != 2)
    {
      return 0;
    }
    iVar3 = valid_target(&td2);
    if (iVar3 == 0)
    {
      return 0;
    }
    kill_card(instance->targets[0].player, instance->targets[0].card, KILL_BURY);
    return 0;
  }
  instance->info_slot = 0;
  iVar3 = charge_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 0, 0);
  if (iVar3 == 0)
  {
    return 0;
  }

  iVar3 = new_can_sacrifice(player, card, player, &test);
  uVar5 = 0;
  if (iVar3 != 0)
  {
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x205, 0, 0, 0, 0, 0, 0, 0,
                                      &td, NULL);
    uVar5 = (unsigned int)(iVar3 != 0);
  }
  iVar3 = new_can_sacrifice(player, card, player, &test1);
  uVar4 = 0;
  if (iVar3 != 0)
  {
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x205, 0, 0, 0, 0, 0, 0, 0,
                                      &td2, NULL);
    uVar4 = (unsigned int)(iVar3 != 0);
  }
  iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\014", "\001\002", "Sac crit & Swamp",
                      uVar5, 5, "Sac crit & snow", uVar4, 10, 0, 0, 0, 0);
  if (iVar3 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (iVar3 == 1)
  {

    uStack_270 =
        new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test);
    if (uStack_270 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    pcVar2 = "Select target nonartifact creature to bury.";
    ptVar6 = &td;
  }
  else
  {
    if (iVar3 != 2)
      goto LAB_020aecef;
    uStack_270 = new_sacrifice(player, card, player,
                               SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK, &test1);
    if (uStack_270 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    pcVar2 = "Select target creature to bury.";
    ptVar6 = &td2;
  }

  iVar7 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x207, 0, 0, 0, 0, 0, 0, 0,
                                    ptVar6, pcVar2);
  if (iVar7 != 0)
  {
    kill_card((int)uStack_270 >> 0x10 & 0xff, uStack_270 >> 0x18, KILL_SACRIFICE);
  }
LAB_020aecef:
  instance->info_slot = iVar3;
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20a9290; original cards/alliances.c:3871. */
int card_agent_of_stromgald(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = has_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 1, 0);
    if (result != 0)
    {
      result = can_produce_mana_2022(player, card);
      return result;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    result = charge_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 1, 0);
    if (result != 0)
    {
      produce_mana(player, 1, 1);
    }
  }
  else if (((event == EVENT_COUNT_MANA) && (card == affected_card)) &&
           (player == affected_card_controller))
  {
    result = can_produce_mana_2022(player, card);
    if (result != 0)
    {
      result = has_mana(player, 4, 1);
      if (result != 0)
      {
        result = has_mana(player, 4, 1);
        has_mana(player, 0, 1);
        declare_mana_available(player, 1, result);
        result = has_mana(player, 4, 1);
        declare_mana_available(player, 0, -result);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20a9440; original cards/alliances.c:3897. */
int card_astrolabe(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_CAN_ACTIVATE)
  {
    result = can_sacrifice_this_as_cost(player, card);
    if (result != 0)
    {
      result = has_mana_for_activated_ability(player, card, 1, 0, 0, 0, 0, 0);
      if (result != 0)
      {
        result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
        return result;
      }
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    result =
        generic_activated_ability(player, card, EVENT_ACTIVATE, 9, 1, 0, 0, 0, 0, 0, 0, NULL, NULL);
    if (result != 0)
    {
      produce_mana_tapped_all_one_color_2022(player, card, COLOR_TEST_ANY_COLORED, 2);
    }
  }
  else
  {
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      cantrip(player, card, 1);
      return 0;
    }
    if (((event == EVENT_COUNT_MANA) && (card == affected_card)) &&
        (player == affected_card_controller))
    {
      result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
      if (result != 0)
      {
        result = can_sacrifice_this_as_cost(player, card);
        if (result != 0)
        {
          result = has_mana(player, 7, 1);
          if (result != 0)
          {
            declare_mana_available_hex(player, 0x3e, 2);
            declare_mana_available(player, 0, -1);
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20aab10; original cards/alliances.c:4396. */
int card_krovikan_plague(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int player_00;
  int iVar3;
  target_definition_t td;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    cantrip(player, card, 1);
    instance = get_card_instance(player, card);
    if (-1 < instance->damage_target_player)
    {
      iVar2 = is_humiliated_for_ability(player, card, AL_NM_NL_ACT);
      if (iVar2 == 0)
      {
        instance = get_card_instance(player, card);
        default_target_definition((int)instance->damage_target_player, instance->damage_target_card,
                                  &td, 0x102);
        td.zone = 0x1200;
      }
      goto LAB_020aab66;
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    if ((instance->damage_target_player < '\0') ||
        (iVar2 = is_humiliated_for_ability(player, card, AL_NM_NL_ACT), iVar2 != 0))
    {
    LAB_020aab58:
      if (event == EVENT_CAN_CAST)
        goto LAB_020aab78;
      if (event == EVENT_CAST_SPELL)
      {
        if ((card == affected_card) && (player == affected_card_controller))
          goto LAB_020aab78;
        goto LAB_020aaccc;
      }
    }
    else
    {

      instance = get_card_instance(player, card);
      player_00 = (int)instance->damage_target_player;
      iVar2 = instance->damage_target_card;
      default_target_definition(player_00, iVar2, &td, 0x102);
      td.zone = 0x1200;
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar2 = is_tapped(player_00, iVar2);
        if ((((iVar2 == 0) && (iVar2 = can_target(&td), iVar2 != 0)) &&
             (iVar2 = can_use_activated_abilities_2022(player, card), iVar2 != 0)) &&
            (iVar2 = has_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 0, 0), iVar2 != 0))
        {
          return 1;
        }
      }
      else if (event == EVENT_ACTIVATE)
      {
        iVar3 = charge_mana_for_activated_ability_2022(player, card, 0, 0, 0, 0, 0, 0);
        if (((iVar3 != 0) && (iVar3 = is_tapped(player_00, iVar2), iVar3 == 0)) &&
            ((iVar3 = can_target(&td),
              iVar3 != 0 && (iVar3 = pick_target(&td, "TARGET_ANY"), iVar3 != 0))))
        {
          tap_card(player_00, iVar2);
        }
      }
      else
      {
        if (event != EVENT_RESOLVE_ACTIVATION)
          goto LAB_020aab58;
        iVar3 = validate_target(player, card, &td, 0);
        if (iVar3 != 0)
        {
          damage_target0(instance->targets[0].player, instance->targets[0].card, 1);
          add_counters_from_source(player, card, player_00, iVar2, COUNTER_M0_M1, 1);
        }
      }
    }
  LAB_020aab66:
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
      goto LAB_020aab78;
  }
LAB_020aaccc:
  if ((((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (2 < event - EVENT_POWER)) &&
        ((event != EVENT_RESOLVE_MOVING_AURA &&
          ((event != EVENT_CHANGE_TYPE && (event != EVENT_SET_COLOR)))))) &&
       (event != EVENT_GET_SELECTED_CARD)) &&
      (event != EVENT_STATIC_EFFECTS))
  {
    return 0;
  }
LAB_020aab78:
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.allowed_controller = player;
  td.required_subtype = 0xd1;
  td.special = 0x1000;
  iVar2 = targeted_aura_custom_prompt(player, card, event, &td,
                                      "Select target non-Wall creature you control.");
  return iVar2;
}

/* ManalinkEh 2022: 0x20abe20; original cards/alliances.c:4723. */
int card_royal_decree(int player, int card, event_t event)
{
  int result;
  unsigned int uVar2;
  unsigned int uVar3;

  cumulative_upkeep(player, card, event, 0, 0, 0, 0, 0, 1);
  if (event != EVENT_TAP_CARD)
    goto LAB_020abe7a;
  result = is_what(affected_card_controller, affected_card, 1);
  if ((result == 0) ||
      ((result = has_subtype(affected_card_controller, affected_card, SUBTYPE_MOUNTAIN),
        result == 0 &&
            (result = has_subtype(affected_card_controller, affected_card, SUBTYPE_SWAMP),
             result == 0))))
  {
    result = is_what(affected_card_controller, affected_card, 0x47);
    if (result == 0)
      goto LAB_020abe7a;
    uVar2 = get_color(affected_card_controller, affected_card);
    uVar3 = get_sleighted_color_test(player, card, 0x12);
    if ((uVar2 & uVar3) == 0)
      goto LAB_020abe7a;
  }
  damage_player(affected_card_controller, 1, player, card);
LAB_020abe7a:
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20ac800; original cards/alliances.c:4914. */
int card_soldevi_adnate(int player, int card, event_t event)
{
  const char *msg;
  card_instance_t *instance;
  int result;
  unsigned int uVar3;
  unsigned int player_00;
  test_definition_t test;

  msg = get_sleighted_color_text(player, card, "Select a %s card or artifact to sacrifice.", 1);
  new_default_test_definition(&test, 2, msg);
  test.special_selection_function = is_black_or_artifact_creature;
  if (event == EVENT_CAN_ACTIVATE)
  {
    result = generic_activated_ability_with_sacrifice_as_cost(player, card, EVENT_CAN_ACTIVATE,
                                                              GAA_UNTAPPED, 0, 0, 0, 0, 0, 0, 0,
                                                              NULL, NULL, TYPE_NONE, &test, SARV_0);
    return result;
  }
  if (event == EVENT_ACTIVATE)
  {
    result =
        generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
    if (result != 0)
    {

      instance = get_card_instance(player, card);
      uVar3 = new_sacrifice(player, card, player, SAC_RETURN_CHOICE | SAC_AS_COST | SAC_JUST_MARK,
                            &test);
      if (uVar3 == 0)
      {
        spell_fizzled = 1;
        remove_state(player, card, 0x10);
      }
      else
      {
        player_00 = (int)uVar3 >> 0x10 & 0xff;
        result = get_cmc(player_00, uVar3 >> 0x18);
        instance->info_slot = result;
        kill_card(player_00, uVar3 >> 0x18, KILL_SACRIFICE);
        ai_modifier = ai_modifier + hand_count[player] * 8;
      }
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {

    instance = get_card_instance(player, card);
    produce_mana(player, 1, instance->info_slot);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x20ad980; original cards/alliances.c:5250. */
int card_stromgald_spy(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_DECLARE_BLOCKERS)
  {
    result = is_attacking(player, card);
    if (result != 0)
    {
      result = is_unblocked(player, card);
      if (result != 0)
      {

        result =
            do_dialog(player, player, card, -1, -1, " Reveal oppenent\'s hand\n Assign damage", 1);
        if (result == 0)
        {
          negate_combat_damage_this_turn(player, card, player, card, 2);
          instance = get_card_instance(player, card);
          instance->targets[0].player = 1;
        }
      }
    }
  }
  else if (event == EVENT_STATIC_EFFECTS)
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      instance = get_card_instance(player, card);
      if (instance->targets[0].player == 1)
      {
        *(unsigned int *)(&player_bits + (1 - player) * 4) =
            *(unsigned int *)(&player_bits + (1 - player) * 4) | 0x200;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20ae0e0; original cards/alliances.c:5426. */
int card_urzas_engine(int player, int card, event_t event)
{
  uint8_t uVar1;
  int iVar2;
  card_instance_t *instance;
  int iVar4;
  unsigned int argument7;

  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {

      instance = get_card_instance(player, card);
      iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if (iVar2 != 0)
      {

        iVar2 = check_for_ability(player, card, 0x40);
        argument7 = 0;
        if (iVar2 != 0)
        {
          argument7 = (unsigned int)(current_phase < 0x15);
        }
        iVar2 =
            do_dialog(player, player, card, -1, -1, " Banding\n Band trample\n Cancel", argument7);
        if (iVar2 == 2)
        {
          spell_fizzled = 1;
          return 0;
        }
        iVar4 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0, 3, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        if (iVar4 != 0)
        {
          instance->info_slot_fn = (CardFunction)(iVar2 + 1);
        }
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {

      instance = get_card_instance(player, card);
      iVar2 = instance->info_slot;
      if (iVar2 == 1)
      {
        iVar2 = pump_ability_until_eot_2022(instance->parent_controller, instance->parent_card,
                                       instance->parent_controller, instance->parent_card, 0, 0,
                                       0x40, 0);
        alternate_legacy_text(1, player, iVar2);
        iVar2 = instance->info_slot;
      }

      if ((((iVar2 == 2) && (instance = in_play(player, card), instance != NULL)) &&
           (uVar1 = instance->blocking, uVar1 != 0xff)) &&
          (1 < active_cards_count[player]))
      {
        iVar2 = 0;
        do
        {
          instance = in_play_and_attacking(player, iVar2);
          if ((instance != NULL) && (uVar1 == instance->blocking))
          {
            iVar4 = pump_ability_until_eot_2022(player, card, player, iVar2, 0, 0, 0x80, 0);
            alternate_legacy_text(2, player, iVar4);
          }
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player] + -1);
      }
    }
    iVar2 = 0;
  }
  return iVar2;
}

/* ManalinkEh 2022: 0x20ae580; original cards/alliances.c:5534. */
int card_veterans_voice(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int argument1;
  int iVar3;
  target_definition_t td;

  instance = get_card_instance(player, card);
  if (instance->damage_target_player < '\0')
  {
  LAB_020ae5a4:
    if (event == EVENT_CAN_CAST)
      goto LAB_020ae5c8;
    if (event != EVENT_CAST_SPELL)
      goto LAB_020ae5b2;
    if ((card == affected_card) && (player == affected_card_controller))
      goto LAB_020ae5c8;
  }
  else
  {

    instance = get_card_instance(player, card);
    argument1 = (int)instance->damage_target_player;
    iVar2 = instance->damage_target_card;
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.special = 0x8000;
    td.extra = (int32_t)not_the_enchanted;
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar3 = is_tapped(argument1, iVar2);
      if ((((iVar3 == 0) && (iVar2 = is_animated_and_sick(argument1, iVar2), iVar2 == 0)) &&
           (iVar2 = can_target(&td), iVar2 != 0)) &&
          ((iVar2 = can_use_activated_abilities(player, card),
            iVar2 != 0 && (iVar2 = has_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0),
                           iVar2 != 0))))
      {
        return 1;
      }
    }
    else if (event == EVENT_ACTIVATE)
    {
      iVar3 = charge_mana_for_activated_ability(player, card, 0, 0, 0, 0, 0, 0);
      if (iVar3 != 0)
      {
        instance->number_of_targets = '\0';
        iVar3 = is_tapped(argument1, iVar2);
        if (((iVar3 == 0) && (iVar3 = is_animated_and_sick(argument1, iVar2), iVar3 == 0)) &&
            ((iVar3 = can_target(&td),
              iVar3 != 0 &&
                  (iVar3 = new_pick_target(
                       &td, "Select target creature other than enchanted it gets +2/+1 until eot.",
                       0, 0x81),
                   iVar3 != 0))))
        {
          tap_card(argument1, iVar2);
        }
      }
    }
    else
    {
      if (event != EVENT_RESOLVE_ACTIVATION)
        goto LAB_020ae5a4;
      iVar2 = validate_target(player, card, &td, 0);
      if (iVar2 != 0)
      {
        pump_ability_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card,
                               2, 1, 0, 0);
      }
    }
  LAB_020ae5b2:
    if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
        (event == EVENT_CHANGE_TARGET))
      goto LAB_020ae5c8;
  }
  if (((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
        ((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)))) &&
       (event != EVENT_CHANGE_TYPE)) &&
      (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
        (event != EVENT_STATIC_EFFECTS))))
  {
    return 0;
  }
LAB_020ae5c8:
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  td.allowed_controller = player;
  iVar2 =
      targeted_aura_custom_prompt(player, card, event, &td, "Select target creature you control.");
  return iVar2;
}

/* ManalinkEh 2022: 0x20af7e0; original cards/alliances.c:5844. */
int card_winters_night(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  color_test_t col;

  iVar1 = is_humiliated_for_ability(player, card, AL_TRIG);
  if (iVar1 == 0)
  {
    if (event == EVENT_COUNT_MANA)
    {
      iVar1 = is_what(affected_card_controller, affected_card, 1);
      if (((((iVar1 != 0) &&
             (iVar1 = has_subtype(affected_card_controller, affected_card, SUBTYPE_SNOW),
              iVar1 != 0)) &&
            (instance = in_play(player, card), instance != NULL)) &&
           ((iVar1 = is_tapped(affected_card_controller, affected_card),
             iVar1 == 0 && (iVar1 = is_animated_and_sick(affected_card_controller, affected_card),
                            iVar1 == 0)))) &&
          (iVar1 = can_produce_mana_2022(affected_card_controller, affected_card), iVar1 != 0))
      {
        instance = get_card_instance(affected_card_controller, affected_card);
        iVar3 = 0;
        col = (color_test_t)instance->card_color;
        iVar1 = 0;
        do
        {
          iVar1 =
              (iVar1 + 1) - (unsigned int)(((int)col >> ((unsigned char)iVar3 & 0x1f) & 1U) == 0);
          iVar3 = iVar3 + 1;
        } while (iVar3 != 7);
        if (iVar1 < 1)
        {
          iVar1 = single_color_test_bit_to_color(col);
          declare_mana_available(affected_card_controller, iVar1, 1);
        }
        else
        {
          declare_mana_available_hex(affected_card_controller, col, 1);
        }
      }
    }
    else if (event < EVENT_UNKNOWN80)
    {
      if (event != EVENT_CAST_SPELL)
      {

        return (unsigned int)(event == EVENT_CAN_CAST);
      }
      if ((card == affected_card) && (player == affected_card_controller))
      {
        ai_modifier = ai_modifier + 0x30;
      }
    }
    else if (((event == EVENT_TAP_CARD) &&
              (iVar1 = is_what(affected_card_controller, affected_card, 1), iVar1 != 0)) &&
             ((iVar1 = has_subtype(affected_card_controller, affected_card, SUBTYPE_SNOW),
               iVar1 != 0 && (instance = in_play(player, card), instance != NULL))))
    {
      produce_mana_of_any_type_tapped_for(player, card, 1);
      does_not_untap_effect(player, card, affected_card_controller, affected_card, 0, 1);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int fyndhorn_druid_dies_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x20a18b0; original cards/alliances.c:1359. */
int card_gusthas_scepter(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int *puVar4;
  int *piVar5;
  int32_t *piVar6;
  unsigned int uVar7;
  int leg_1;
  int idx_1;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    char buffer[100];
    int value;
    uint8_t bytes[100];
  } recovered_stack_leg;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    int value;
    test_definition_t test;
    uint8_t bytes[216];
  } recovered_stack_idx;

  instance = get_card_instance(player, card);
  leaves_play_2022(player, card, event, 1, 0);
  if (event == EVENT_CARDCONTROLLED)
  {
    if ((card != affected_card) || (affected_card_controller != 2))
      goto LAB_020a1c48;
  LAB_020a1b18:

    recovered_stack_leg.value = 0;
    recovered_stack_idx.value = 0;
    while (puVar4 = (unsigned int *)exiledby_find_any(player, card, &recovered_stack_leg.value,
                                                      &recovered_stack_idx.value),
           puVar4 != NULL)
    {

      uVar7 = *puVar4;
      *puVar4 = 0xffffffff;
      iVar2 = add_card_to_rfg(uVar7 >> 0x1f, uVar7 & 0x7fffffff);
      from_exile_to_graveyard(player, iVar2);
    }
  }
  else if (event == EVENT_LEAVES_PLAY_ABILITY)
    goto LAB_020a1b18;
  if (event == EVENT_CAN_ACTIVATE)
  {
    if ((player == 0) &&
        (iVar2 = exiledby_choose(0, card, 0x8f6, EXBY_FIRST_FOUND, 0, "Card", 0), iVar2 != 0))
    {
      return 1;
    }
    iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    return iVar2;
  }
  if (event == EVENT_ACTIVATE)
  {

    iVar2 = exiledby_count(player, card, player);
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    uVar7 = 0;
    if (iVar3 != 0)
    {
      uVar7 = (unsigned int)(0 < hand_count[player]);
    }
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0,
                                      NULL, NULL);
    iVar2 =
        dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Exile a card", uVar7, 5,
                    "Pick a card from exile", (unsigned int)(0 < iVar2 && iVar3 != 0), 10,
                    "Show exiled cards", (unsigned int)(player == 0 && 0 < iVar2), 0, 0, 0, 0, 0);
    if (iVar2 == 0)
    {
      spell_fizzled = 1;
      return 0;
    }
    if (iVar2 == 3)
    {
      exiledby_choose(player, card, 0x8f6, EXBY_CHOOSE, 0, "Card", 0);
      spell_fizzled = 1;
      return 0;
    }
    if (iVar2 != 1)
    {
      if (iVar2 != 2)
      {
        return 0;
      }

      piVar6 = (int32_t *)exiledby_choose(player, card, 0x8f6, EXBY_CHOOSE, 0, "Card", 0);
      if (piVar6 == NULL)
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->targets[6].player = -1;
        instance->targets[6].card = *piVar6;
      }
    }
    iVar2 =
        generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
    return iVar2;
  }
LAB_020a1c48:
  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    return 0;
  }
  if (instance->info_slot == 1)
  {
    if (0 < hand_count[player])
    {

      leg_1 = 0;
      idx_1 = 0;
      scnprintf(recovered_stack_leg.buffer, 100, "Select a card to exile");
      new_default_test_definition(&recovered_stack_idx.test, 0xff, recovered_stack_leg.buffer);
      iVar2 = new_select_a_card(player, player, 4, 1, 3, -1, &recovered_stack_idx.test);
      iVar3 = get_original_internal_card_id(player, iVar2);
      exiledby_remember(player, instance->parent_card, player, iVar3, &leg_1, &idx_1);
      play_sound_effect(WAV_DESTROY);
      obliterate_card(player, iVar2);
      return 0;
    }
    return 0;
  }
  if (instance->info_slot != 2)
  {
    return 0;
  }

  piVar5 = exiledby_find(player, instance->parent_card, instance->targets[6].card, NULL, NULL);
  if (piVar5 == NULL)
  {
    spell_fizzled = 1;
    return 0;
  }

  *piVar5 = -1;
  add_card_to_hand(player, instance->targets[6].card & 0x7fffffff);
  return 0;
}

/* ManalinkEh 2022: 0x209d380; original cards/alliances.c:4238. */
static int fyndhorn_druid_dies_legacy(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  int iVar4;

  if (event == EVENT_GRAVEYARD_FROM_PLAY)
  {
    instance = get_card_instance(player, card);
    iVar1 = instance->targets[0].card;
    instance = get_card_instance(player, card);
    if ((iVar1 == affected_card) && (instance->targets[0].player == affected_card_controller))
    {

      instance = get_card_instance(player, card);
      iVar1 = instance->targets[0].player;
      iVar4 = instance->targets[0].card;
      iVar3 = is_this_dying(iVar1, iVar4);
      if (iVar3 != 0)
      {
        iVar3 = check_for_death_trigger_removal(iVar1, iVar4);
        if (iVar3 != 0)
        {
          iVar3 = is_what(iVar1, iVar4, 2);
          iVar4 = 4;
          if (iVar3 != 0)
          {
            iVar4 = (unsigned int)card_counts[iVar1][5] * 4 + 4;
          }
          gain_life_2022(iVar1, iVar4);
          goto LAB_0209d463;
        }
      }
    }
  }
  else if (event == EVENT_CLEANUP)
  {
  LAB_0209d463:
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x20aa490; original cards/alliances.c:4256. */
int card_fyndhorn_druid(int player, int card, event_t event)
{
  card_instance_t *instance;
  int card_00;

  if (((event == EVENT_PHASE_CHANGED) && (current_phase == 0x18)) && (current_turn == player))
  {

    instance = get_card_instance(player, card);
    if ((instance->state & 0x200) != 0)
    {

      card_00 = create_legacy_effect(player, card, fyndhorn_druid_dies_legacy);
      instance = get_card_instance(player, card_00);
      instance->targets[0].player = player;
      instance = get_card_instance(player, card_00);
      instance->targets[0].card = card;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20abc20; original cards/alliances.c:4677. */
int card_phantasmal_sphere(int player, int card, event_t event)
{
  int result;
  token_generation_t token;
  target_definition_t td;

  upkeep_trigger_ability_mode(player, card, event, player, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    leaves_play_2022(player, card, event, 1, 1);
    if (event == EVENT_LEAVES_PLAY_ABILITY)
    {
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      result = would_validate_arbitrary_target(&td, 1 - player, -1);
      if (result != 0)
      {
        default_token_definition(player, card, 0x496d, &token);
        token.t_player = 1 - player;
        token.pow = count_counters(player, card, COUNTER_P1_P1);
        token.tou = token.pow;
        generate_token(&token);
      }
    }
    return 0;
  }

  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  result = count_counters(player, card, COUNTER_P1_P1);
  result = has_mana(player, 7, result);
  if (result != 0)
  {
    result = count_counters(player, card, COUNTER_P1_P1);
    result =
        charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player, COLOR_ANY, result);
    if (result != 0)
      goto LAB_020abd00;
  }
  kill_card(player, card, KILL_SACRIFICE);
LAB_020abd00:
  leaves_play_2022(player, card, EVENT_UPKEEP_TRIGGER_ABILITY, 1, 1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20ab320; original cards/alliances.c:4535. */
int card_natures_wrath(int player, int card, event_t event)
{
  int t_player;
  int iVar1;
  card_instance_t *instance;
  const char *pcVar3;
  int iVar4;
  test_definition_t *ptVar5;
  test_definition_t test;
  test_definition_t test2;
  test_definition_t test3;
  test_definition_t test4;

  basic_upkeep(player, card, event, 0, 0, 0, 1, 0, 0);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    default_test_definition(&test, 1);
    test.subtype = get_hacked_subtype(player, card, 0x5002);
    iVar1 = new_make_test_in_play(trigger_cause_controller, trigger_cause, -1, &test);
    if (iVar1 != 0)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 1;
      etb_ability_global(2, event, 2, TYPE_NONE, &test, player, card);
    }
    default_test_definition(&test2, 0x47);
    test2.color = get_sleighted_color_test(player, card, 4);
    iVar1 = new_make_test_in_play(trigger_cause_controller, trigger_cause, -1, &test2);
    if (iVar1 != 0)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 2;
      etb_ability_global(2, event, 2, TYPE_NONE, &test2, player, card);
    }
    default_test_definition(&test3, 1);
    test3.subtype = get_hacked_subtype(player, card, 0x5009);
    iVar1 = new_make_test_in_play(trigger_cause_controller, trigger_cause, -1, &test3);
    if (iVar1 != 0)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 3;
      etb_ability_global(2, event, 2, TYPE_NONE, &test3, player, card);
    }
    default_test_definition(&test4, 0x47);
    test4.color = get_sleighted_color_test(player, card, 2);
    iVar1 = new_make_test_in_play(trigger_cause_controller, trigger_cause, -1, &test4);
    if (iVar1 != 0)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 4;
      etb_ability_global(2, event, 2, TYPE_NONE, &test4, player, card);
    }
  }
  if (event != EVENT_ETB)
    goto LAB_020ab38d;

  instance = get_card_instance(player, card);
  t_player = trigger_cause_controller;
  iVar1 = instance->info_slot;
  if (iVar1 - 1U < 2)
  {

    pcVar3 = get_hacked_land_text(player, card, "Select %a land to sacrifice.", 0x5002);
    new_default_test_definition(&test3, 1, pcVar3);
    test3.subtype = get_hacked_subtype(player, card, 0x5002);
    pcVar3 = get_sleighted_color_text(player, card, "Select a %s permanent card to sacrifice.", 2);
    new_default_test_definition(&test4, 0x47, pcVar3);
    test4.color = get_sleighted_color_test(player, card, 4);
    iVar1 = new_can_sacrifice(player, card, t_player, &test4);
    iVar4 = new_can_sacrifice(player, card, t_player, &test3);
    iVar1 = dialog_impl(t_player, card, EVENT_ACTIVATE, "\001\002", "\001\003", "Sac Island", iVar4,
                        5, "Sac Blue permanent", iVar1, 5, 0, 0, 0, 0);
    if (iVar1 == 1)
    {
      ptVar5 = &test3;
    LAB_020ab8ec:
      new_sacrifice(t_player, card, t_player, SAC_CAUSED | SAC_NO_CANCEL, ptVar5);
    }
    else if (iVar1 == 2)
    {
      ptVar5 = &test4;
      goto LAB_020ab8ec;
    }
    iVar1 = instance->info_slot;
  }
  if (iVar1 - 3U < 2)
  {

    pcVar3 = get_hacked_land_text(player, card, "Select %a land to sacrifice.", 0x5009);
    new_default_test_definition(&test3, 1, pcVar3);
    test3.subtype = get_hacked_subtype(player, card, 0x5009);
    pcVar3 = get_sleighted_color_text(player, card, "Select a %s permanent card to sacrifice.", 1);
    new_default_test_definition(&test4, 0x47, pcVar3);
    test4.color = get_sleighted_color_test(player, card, 2);
    iVar1 = new_can_sacrifice(player, card, t_player, &test4);
    iVar4 = new_can_sacrifice(player, card, t_player, &test3);
    iVar1 = dialog_impl(t_player, card, EVENT_ACTIVATE, "\001\002", "\001\003", "Sac Swamp", iVar4,
                        5, "Sac Black permanent", iVar1, 5, 0, 0, 0, 0);
    if (iVar1 == 1)
    {
      ptVar5 = &test3;
    }
    else
    {
      if (iVar1 != 2)
        goto LAB_020ab38d;
      ptVar5 = &test4;
    }
    new_sacrifice(t_player, card, t_player, SAC_CAUSED | SAC_NO_CANCEL, ptVar5);
  }
LAB_020ab38d:
  iVar1 = global_enchantment(player, card, event);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20aa6f0; original cards/alliances.c:4312. */
int card_gorilla_berserkers(int player, int card, event_t event)
{
  rampage(player, card, event, 2);
  minblockers_3(player, card, event);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20ac270; original cards/alliances.c:4804. */
int card_school_of_the_unseen(int player, int card, event_t event)
{
  card_instance_t *instance;
  color_t color;
  unsigned int uVar2;
  int result;
  unsigned int uVar4;
  bool bVar5;

  if (event == EVENT_RESOLVE_SPELL)
  {
    instance = get_card_instance(player, card);
    instance->info_slot = 1;
    play_land_sound_effect_force_color(player, card, 0x3e);
    count_mana_2022();
    return 0;
  }
  if (event == EVENT_CHANGE_TYPE)
  {

    uVar2 = 0x3f;
    result = has_mana(player, 0, 3);
    if (result == 0)
    {
      uVar2 = 1;
      result = is_what(player, card, 1);
      if (result != 0)
      {
        uVar2 = mana_colors_added[player] | 1;
      }
    }
    instance = get_card_instance(player, card);
    instance->info_slot = uVar2;
    return 0;
  }
  if (event == EVENT_COUNT_MANA)
  {
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
    result = is_tapped(player, card);
    if (result != 0)
    {
      return 0;
    }
    result = is_animated_and_sick(player, card);
    if (result != 0)
    {
      return 0;
    }
    result = can_produce_mana_2022(player, card);
    if (result == 0)
    {
      return 0;
    }
    result = has_mana(player, 7, 3);
    if (result != 0)
    {
      declare_mana_available_hex(player, 0x3f, 1);
      return 0;
    }
    declare_mana_available(player, 0, 1);
    return 0;
  }
  if (event == EVENT_CAN_ACTIVATE)
  {
    instance = get_card_instance(player, card);
    if (instance->targets[1].player == 0x42)
    {
      return 0;
    }
    result = mana_producer(player, card, EVENT_CAN_ACTIVATE);
    return (unsigned int)(result != 0);
  }
  if (event != EVENT_ACTIVATE)
  {
    return 0;
  }

  instance = get_card_instance(player, card);
  if (instance->targets[1].player == 0x42)
  {
    return 0;
  }
  result = has_mana(player, 0, 3);
  if (result == 0)
  {
    result = is_what(player, card, 1);
    color = COLOR_COLORLESS;
    if (result == 0)
      goto LAB_020ac323;
    color = COLOR_COLORLESS;
    if (mana_colors_added[player] == 0)
      goto LAB_020ac323;
    color = choose_a_color_exe(player, "What kind of mana?", 1, 0, mana_colors_added[player] | 1);
  }
  else
  {
    color = choose_mana_all_one_color_2022(player, COLOR_TEST_ANY, 1, "What kind of mana?",
                                           COLOR_TEST_COLORLESS);
  }
  if (color == ~COLOR_COLORLESS)
  {
    spell_fizzled = 1;
    return 0;
  }
LAB_020ac323:
  uVar2 = color_to_color_test(color);
  result = is_what(player, card, 1);
  uVar4 = 1;
  if (result != 0)
  {
    uVar4 = mana_colors_added[player] | 1;
  }
  if ((uVar2 & uVar4) == 0)
  {
    instance = get_card_instance(player, card);
    instance->targets[1].player = 0x42;
    charge_mana(player, COLOR_COLORLESS, 2);
    instance = get_card_instance(player, card);
    bVar5 = spell_fizzled == 1;
    instance->targets[1].player = 0;
    if (bVar5)
    {
      return 0;
    }
  }
  produce_mana_tapped_impl(player, card, color, 1, 0, 1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20a9c60; original cards/alliances.c:4072. */
int card_dystopia(int player, int card, event_t event)
{
  int result;
  const char *msg;
  test_definition_t test;

  if (event == EVENT_CAST_SPELL)
  {
    result = get_sleighted_color_test(player, card, 0x28);
    result = count_permanents_by_color(1 - player, 2, result);
    if (result < 1)
    {
      ai_modifier = ai_modifier + -0x18;
    }
    else
    {
      result = get_sleighted_color_test(player, card, 0x28);
      result = count_permanents_by_color(1 - player, 2, result);
      ai_modifier = ai_modifier + result * 8;
    }
    upkeep_trigger_ability(player, card, EVENT_CAST_SPELL, 2);
  }
  else
  {
    upkeep_trigger_ability(player, card, event, 2);
    if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
    {
      if (current_turn == player)
      {
        cumulative_upkeep_general(player, card, EVENT_UPKEEP_TRIGGER_ABILITY, can_pay_1_life_thunk,
                                  lose_1_life_thunk);
      }
      msg = get_sleighted_color_text2(player, card, "Select a %s or %s permanent to sacrifice.", 3,
                                      5);
      new_default_test_definition(&test, 0x47, msg);
      test.color = get_sleighted_color_test(player, card, 0x28);
      new_sacrifice(current_turn, card, current_turn, SAC_CAUSED | SAC_NO_CANCEL, &test);
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *is_damage_to_player_or_planeswalker(int who_chooses, int player, int card,
                                                 int targeting_player, int targeting_card);
static int effect_cant_be_blocked_except_by_walls(int player, int card, event_t event);

/* ManalinkEh 2022: 0x209d520; original cards/alliances.c:5484. */
static int effect_cant_be_blocked_except_by_walls(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_BLOCK_LEGALITY)
  {

    instance = get_card_instance(player, card);
    if (((-1 < instance->damage_target_card) &&
         (instance->damage_target_player == attacking_card_controller)) &&
        (instance->damage_target_card == attacking_card))
    {
      result = has_subtype(affected_card_controller, affected_card, SUBTYPE_WALL);
      if (result == 0)
      {
        event_result = 1;
      }
    }
  }
  else if ((event == EVENT_CLEANUP) || (event == EVENT_SHOULD_AI_PLAY))
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x20ae440; original cards/alliances.c:5512. */
int card_varchilds_crusader(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  if (event == EVENT_ACTIVATE)
  {
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = create_targetted_legacy_effect(instance->parent_controller, instance->parent_card,
                                              effect_cant_be_blocked_except_by_walls,
                                              instance->parent_controller, instance->parent_card);
      alternate_legacy_text(1, player, result);
      result = create_targetted_legacy_effect(instance->parent_controller, instance->parent_card,
                                              sac_next_endstep, instance->parent_controller,
                                              instance->parent_card);
      alternate_legacy_text(2, player, result);
    }
  }
  result = generic_activated_ability_2022(player, card, event, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x209d660; original cards/alliances.c:5714. */
static char *is_damage_to_player_or_planeswalker(int who_chooses, int player, int card,
                                                 int targeting_player, int targeting_card)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if (instance->internal_card_id != damage_card)
  {
    return ",damage";
  }
  if ((-1 < instance->damage_target_card) &&
      (result = damage_is_to_planeswalker(instance), result == 0))
  {
    return ",damage to a player or planeswalker";
  }
  return NULL;
}

/* ManalinkEh 2022: 0x20af030; original cards/alliances.c:5740. */
int card_wandering_mage(int player, int card, event_t event)
{
  card_instance_t *instance;
  int32_t iVar2;
  int iVar3;
  int iVar4;
  unsigned int uVar5;
  unsigned int uVar6;
  target_definition_2022_t td;
  target_definition_2022_t td1;
  target_definition_2022_t td2;
  target_definition_2022_t td3;

  if (((event == EVENT_ACTIVATE) || (event - EVENT_RESOLVE_ACTIVATION < 2)) ||
      (event == EVENT_CLEANUP))
  {
    default_target_definition_2022(player, card, &td, 2);
    td.extra = damage_card;
    td.required_type = 0;
    td.special = 0x100000;
    default_target_definition_2022(player, card, &td1, 0);
    td1.special = 0x8000;
    td1.extra = (int32_t)damage_to_cleric_or_wizard;
    default_target_definition_2022(player, card, &td2, 0xff);
    td2.special = 0x8000;
    td2.extra = (int32_t)is_damage_to_player_or_planeswalker;
    base_target_definition_2022(player, card, &td3, 2);
    td3.allowed_controller = player;
    td3.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar3 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 0x4000004, 0, 0, 0,
                                             0, 0, 1, 0, &td, "TARGET_DAMAGE");
      if (((iVar3 != 0) && (iVar3 = can_pay_life(player, 1), iVar3 != 0)) ||
          ((iVar3 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 0x1000004, 0,
                                                   0, 1, 0, 0, 0, 0, &td1, "TARGET_DAMAGE"),
            iVar3 != 0 || ((iVar3 = can_target_2022(&td3),
                            iVar3 != 0 && (iVar3 = generic_activated_ability_2022(
                                               player, card, EVENT_CAN_ACTIVATE, 0x1000004, 0, 1, 0,
                                               0, 0, 0, 0, &td2, "TARGET_DAMAGE"),
                                           iVar3 != 0))))))
      {
        return 99;
      }
    }
    else if (event == EVENT_ACTIVATE)
    {

      iVar3 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 0x4000004, 0, 0, 0,
                                             0, 0, 1, 0, &td, "TARGET_DAMAGE");
      uVar6 = 0;
      if (iVar3 != 0)
      {
        iVar3 = can_pay_life(player, 1);
        uVar6 = (unsigned int)(iVar3 != 0);
      }
      iVar3 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 0x4000004, 0, 0, 1,
                                             0, 0, 0, 0, &td1, "TARGET_DAMAGE");
      iVar4 = can_target_2022(&td3);
      uVar5 = 0;
      if (iVar4 != 0)
      {
        iVar4 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, 0x1000004, 0, 1, 0,
                                               0, 0, 0, 0, &td2, "TARGET_DAMAGE");
        uVar5 = (unsigned int)(iVar4 != 0);
      }
      iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "Prevent dam to crit.", uVar6,
                          5, "Prevent dam to cleric/wiz.", iVar3, 10,
                          "Prevent dam to player/plansw.", uVar5, 5, 0, 0, 0, 0);
      if (iVar3 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar3 == 1)
      {
        iVar3 = generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 0x4000004, 0, 0, 0, 0,
                                               0, 1, 0, &td, "TARGET_DAMAGE");
        if (iVar3 != 0)
        {
          lose_life_2022(player, 1);
        }
      }
      else
      {
        if (iVar3 == 2)
        {
          iVar3 = generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 0x4000004, 0, 0, 1,
                                                 0, 0, 0, 0, &td1, "TARGET_DAMAGE");
          return iVar3;
        }
        if (((iVar3 == 3) &&
             (iVar3 = generic_activated_ability_2022(player, card, EVENT_ACTIVATE, 0x1000004, 0, 1,
                                                     0, 0, 0, 0, 0, &td2, "TARGET_DAMAGE"),
              iVar3 != 0)) &&
            (iVar3 = new_pick_target_2022(&td3, "TARGET_CREATURE_YOU_CONTROL", 1, 1), iVar3 != 0))
        {
          add_counters_from_source(player, card, instance->targets[1].player,
                                   instance->targets[1].card, COUNTER_M1_M1, 1);
        }
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {
      iVar2 = instance->info_slot;
      if (iVar2 == 1)
      {
        iVar3 = valid_target_2022(&td);
        if (iVar3 == 0)
        {
          iVar2 = instance->info_slot;
        }
        else
        {
          prevent_damage(instance->targets[0].player, instance->targets[0].card, 2);
          iVar2 = instance->info_slot;
        }
      }
      if (iVar2 == 2)
      {
        iVar3 = valid_target_2022(&td1);
        if (iVar3 == 0)
        {
          iVar2 = instance->info_slot;
        }
        else
        {
          prevent_damage(instance->targets[0].player, instance->targets[0].card, 1);
          iVar2 = instance->info_slot;
        }
      }
      if ((iVar2 == 3) && (iVar3 = valid_target_2022(&td2), iVar3 != 0))
      {
        prevent_damage(instance->targets[0].player, instance->targets[0].card, 2);
        return 0;
      }
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 native scalar-word handlers. */
int card_soldevi_sentry(int player, int card, event_t event);

/* ManalinkEh 2022: 0x20ad0c0; original cards/alliances.c:5072. */
int card_soldevi_sentry(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_2022_t td;
  default_target_definition_2022(player, card, &td, 0);
  iVar3 = 1 - player;
  td.zone = 0x1000;
  td.allowed_controller = iVar3;
  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar3 = can_target_2022(&td);
    if (iVar3 != 0)
    {
      iVar3 = generic_activated_ability_2022(player, card, EVENT_CAN_ACTIVATE, GAA_REGENERATION, 1,
                                             0, 0, 0, 0, 0, 0, NULL, NULL);
      return iVar3;
    }
  }
  else if (event == EVENT_ACTIVATE)
  {
    instance = get_card_instance(player, card);
    iVar2 = generic_activated_ability_2022(player, card, EVENT_ACTIVATE,
                                           (GAA_REGENERATION | GAA_CAN_ONLY_TARGET_OPPONENT), 1, 0,
                                           0, 0, 0, 0, 0, &td, NULL);
    if (iVar2 != 0)
    {
      instance->targets[0].player = iVar3;
      instance->targets[0].card = -1;
      instance->number_of_targets = '\x01';
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    iVar3 = valid_target_2022(&td);
    if ((iVar3 != 0) &&
        (iVar3 = can_regenerate(instance->parent_controller, instance->parent_card), iVar3 != 0))
    {
      regenerate_target(instance->parent_controller, instance->parent_card);
      draw_some_cards_if_you_want(instance->parent_controller, instance->parent_card,
                                  instance->targets[0].player, 1);
      return 0;
    }
  }
  return 0;
}

#endif
