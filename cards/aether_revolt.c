#include "manalink.h"

/* Canonical recovered helper declarations. */
int improvise(int player, int card, event_t event);
static void av_free_spell(int player, int max_cmc);

// Functions

static int revolt(int player){
	return get_trap_condition(player, TRAP_REVOLT);
}



static int improvise_impl(int player, int card, event_t event, int x_spell){

	if( event == EVENT_MODIFY_COST ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_ARTIFACT);
		td.allowed_controller = td.preferred_controller = player;
		td.illegal_state = TARGET_STATE_TAPPED;
		td.illegal_abilities = 0;
		COST_COLORLESS -= target_available(player, card, &td);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		if( x_spell )
			set_special_flags2(player, card, SF2_X_SPELL);
		if( ! played_for_free(player, card) && ! is_token(player, card) ){
			card_instance_t *instance = get_card_instance( player, card);
			instance->info_slot = 0;
			int choice = DIALOG(player, card, event, DLG_NO_STORAGE,
								"Use Improvise", 5, 1,
								"Play normally", 1, 1);
			if( ! choice ){
				spell_fizzled = 1;
				return 0;
			}
			int cless = get_updated_casting_cost(player, card, -1, event, -1);

			target_definition_t td;
			default_target_definition(player, card, &td, TYPE_ARTIFACT);
			td.allowed_controller = td.preferred_controller = player;
			td.illegal_state = TARGET_STATE_TAPPED;
			td.illegal_abilities = 0;
			int arts = target_available(player, card, &td);

			int amount = cless - arts;

			int art_to_tap[15];
			int att = 0;
			int xv = 0;
			if( choice == 1 ){
				while( (amount || x_spell) && can_target(&td) ){
						if( new_pick_target(&td, "Select an artifact you control to tap.", 0, GS_LITERAL_PROMPT) ){
							art_to_tap[att] = instance->targets[0].card;
							att++;
							state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
							amount--;
							instance->number_of_targets = 0;
							if( ! amount )
								xv++;
						}
						else{
							break;
						}
				}
			}
			instance->number_of_targets = 0;
			if( amount )
				charge_mana(player, COLOR_COLORLESS, amount);
			if( x_spell ){
				charge_mana(player, COLOR_COLORLESS, -1);
				if( spell_fizzled != 1 )
					xv += x_value;
			}
			int i;
			for(i=0; i<att; i++){
				state_untargettable(player, art_to_tap[i], 0);
				if( spell_fizzled != 1 )
					tap_card(player, art_to_tap[i]);
			}
			if( x_spell ){
				set_x_for_x_spells(player, card, event, xv);
			}
			if( spell_fizzled == 1 )
				return 0;
			if( att )
				return 2;
			return 1;
		}
	}

	return 0;
}



static int eot_revolt(int player, int card, event_t event){
	if( trigger_condition == TRIGGER_EOT && current_turn == player ){
		if( revolt(player) && eot_trigger(player, card, event) ){
			return 1;
		}
	}
	return 0;
}
// Cards

int card_aegis_automaton(int player, int card, event_t event){
/*
Aegis Automaton	|2
Artifact Creature - Construct 0/3
|4|W: Return another target creature you control to its owner's hand.
*/

	if( ! IS_GAA_EVENT(event))
		return 0;

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.allowed_controller = td.preferred_controller = player;
	td.special = TARGET_SPECIAL_NOT_ME;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION && valid_target(&td))
		bounce_permanent(instance->targets[0].player, instance->targets[0].card);

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_LITERAL_PROMPT, MANACOST_XW(4, 1), 0,
									&td, "Select another target creature you control.");
}

static const char* is_creature_or_vehicle(int who_chooses, int player, int card){

	if( is_what(player, card, TYPE_CREATURE) || has_subtype(player, card, SUBTYPE_VEHICLE) )
		return NULL;

	return "must be a creature or a Vehicle.";
}

int card_aerial_modification(int player, int card, event_t event){
/*
Aerial Modification	|4|W
Enchantment - Aura
Enchant creature or Vehicle
As long as enchanted permanent is a Vehicle, it's a creature in addition to its other types.
Enchanted creature gets +2/+2 and has flying.
*/

	if( in_play(player, card) && ! is_humiliated(player, card) ){
		card_instance_t *instance = get_card_instance(player, card);
		int p = instance->damage_target_player;
		int c = instance->damage_target_card;

		if( ! affect_me(p, c) )
			return 0;

		switch( event ){
				case EVENT_POWER:
				case EVENT_TOUGHNESS:
					event_result += 2;
					break;
				case EVENT_ABILITIES:
					event_result |= KEYWORD_FLYING;
					break;
				default:
					break;
		}

		if( event == EVENT_CHANGE_TYPE && has_subtype(p, c, SUBTYPE_VEHICLE) ){
			if( ! is_what(-1, event_result, TYPE_CREATURE) ){
				if( instance->dummy3 < 1 ){
					int newtype = create_a_card_type(event_result);
					cards_at_7c7000[newtype]->type |= (cards_data[event_result].type | TYPE_CREATURE);
					instance->dummy3 = newtype;
					add_status(p, c, STATUS_ANIMATED);
				}
				event_result = instance->dummy3;
			}
			else{
				remove_status(p, c, STATUS_ANIMATED);
			}
		}
	}

	if( ! IS_AURA_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_PERMANENT);
	td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
	td.extra = (int32_t)is_creature_or_vehicle;

	return targeted_aura_custom_prompt(player, card, event, &td, "Select target creature or Vehicle.");
}

int card_aeronaut_admiral(int player, int card, event_t event){
/*
Aeronaut Admiral	|3|W
Creature - Human Pilot 3/1
Flying
Vehicles you control have flying.
*/
	return boost_creature_type(player, card, event, SUBTYPE_VEHICLE, 0, 0, KEYWORD_FLYING, BCT_CONTROLLER_ONLY);
}

int card_aether_chaser(int player, int card, event_t event){
/*
Aether Chaser	|1|R
Creature - Human Artificer 2/1
First strike
When ~ enters the battlefield, you get |E|E (two energy counters).
Whenever ~ attacks, you may pay |E|E. If you do, create a 1/1 colorless Servo artifact creature token.
*/
	// Used externally
	etb_energy(player, card, event, 2);
	if( attack_trigger_with_energy(player, card, event, 2) )
	    remove_energy(player, 2);
		make_servo(player, card, 1);
	return 0;
}

/*
Aether Herder	|3|G --> Aether Chaser
Creature - Elf Artificer Druid 3/3
When ~ enters the battlefield, you get |E|E (two energy counters).
Whenever ~ attacks, you may pay |E|E. If you do, create a 1/1 colorless Servo artifact creature token.
*/

/*
Aether Inspector	|3|W --> Aether Chaser
Creature - Dwarf Artificer 2/3
Vigilance
When ~ enters the battlefield, you get |E|E (two energy counters).
Whenever ~ attacks, you may pay |E|E. If you do, create a 1/1 colorless Servo artifact creature token.
*/

int card_aether_poisoner(int player, int card, event_t event){
/*
Aether Poisoner	|1|B
Creature - Human Artificer 1/1
Deathtouch (Any amount of damage this deals to a creature is enough to destroy it.)
When ~ enters the battlefield, you get |E|E (two energy counters).
Whenever ~ attacks, you may pay |E|E. If you do, create a 1/1 colorless Servo artifact creature token.
*/
	deathtouch(player, card, event);
	return card_aether_chaser(player, card, event);
}

/*
Aether Swooper	|1|U --> Aether Chaser
Creature - Vedalken Artificer 1/2
Flying
When ~ enters the battlefield, you get |E|E (two energy counters).
Whenever ~ attacks, you may pay |E|E. If you do, create a 1/1 colorless Servo artifact creature token.
*/


int card_aethergeode_miner(int player, int card, event_t event){
/*
Aethergeode Miner	|1|W
Creature - Dwarf Scout 3/1
Whenever ~ attacks, you get |E|E (two energy counters).
Pay |E|E: Exile ~, then return it to the battlefield under its owner's control.
*/

	if( declare_attackers_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY, player, card) ){
		add_energy(player, 2);
	}

	if( ! IS_GAA_EVENT(event))
		return 0;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION )
		blink_effect(instance->parent_controller, instance->parent_card, NULL);

	return generic_activated_ability(player, card, event, 0, MANACOST0, GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 2), NULL, NULL);
}

int card_aethersphere_harvester(int player, int card, event_t event){
/*
Aethersphere Harvester	|3
Artifact - Vehicle 3/5
Flying
When ~ enters the battlefield, you get |E|E (two energy counters).
Pay |E: ~ gains lifelink until end of turn.
Crew 1 (Tap any number of creatures you control with total power 1 or more: This Vehicle becomes an artifact creature until end of turn.)
*/
	etb_energy(player, card, event, 2);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	enum{
		CHOICE_LIFELINK = 1,
		CHOICE_CREW
	};

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_CAN_ACTIVATE ){
		if( generic_activated_ability(player, card, event, 0, MANACOST0, GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 1), NULL, NULL) )
			return 1;
		return vehicle(player, card, event, 1);
	}

	if( event == EVENT_ACTIVATE ){
		instance->info_slot = instance->number_of_targets = 0;
		int abils[3] = {	0,
							generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0, MANACOST0,
							GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 1), NULL, NULL),
							vehicle(player, card, EVENT_CAN_ACTIVATE, 1)
		};

		int choice = DIALOG(player, card, event, DLG_RANDOM,
							"Lifelink", abils[1], current_phase == PHASE_AFTER_BLOCKING ? 10 : 1,
							"Crew", abils[2], 5);

		if( ! choice ){
			spell_fizzled = 1;
			return 0;
		}

		if( choice == CHOICE_LIFELINK ){
			return generic_activated_ability(player, card, event, 0, MANACOST0, GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 1), NULL, NULL);
		}

		if( choice == CHOICE_CREW ){
			return vehicle(player, card, event, 1);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( instance->info_slot == CHOICE_LIFELINK ){
			return generic_shade(player, card, event, 0, MANACOST0, 0, 0, 0, SP_KEYWORD_LIFELINK);
		}

		if( instance->info_slot == CHOICE_CREW ){
			return vehicle(player, card, event, 1);
		}
	}

	return 0;
}


int card_aetherstream_leopard(int player, int card, event_t event){
/*
Aetherstream Leopard	|2|G
Creature - Cat 2/3
Trample
When ~ enters the battlefield, you get |E (an energy counter).
Whenever ~ attacks, you may pay |E. If you do, it gets +2/+0 until end of turn.
*/
	etb_energy(player, card, event, 1);
	if( attack_trigger_with_energy(player, card, event, 1) )
	    remove_energy(player, 1);
		pump_until_eot(player, card, player, card, 2, 0);
	return 0;
}

int card_aethertide_whale(int player, int card, event_t event){
/*
Aethertide Whale	|4|U|U
Creature - Whale 6/4
Flying
When ~ enters the battlefield, you get |E|E|E|E|E|E (six energy counters).
Pay |E|E|E|E: Return ~ to its owner's hand.
*/
	etb_energy(player, card, event, 6);

	if( ! IS_GAA_EVENT(event))
		return 0;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION )
		bounce_permanent(instance->parent_controller, instance->parent_card);

	return generic_activated_ability(player, card, event, 0, MANACOST0, GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 4), NULL, NULL);
}

int card_aetherwind_basket(int player, int card, event_t event){
/*
Aetherwind Basker	|4|G|G|G
Creature - Lizard 7/7
Trample
Whenever ~ enters the battlefield or attacks, you get |E (an energy counter) for each creature you control.
Pay |E: ~ gets +1/+1 until end of turn.
*/

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY || declare_attackers_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY, player, card) ){
		add_energy(player, count_subtype(player, TYPE_CREATURE, -1));
	}

	if( event == EVENT_RESOLVE_ACTIVATION )
		return generic_shade(player, card, event, 0, MANACOST0, 1, 1, 0, 0);

	return generic_activated_ability(player, card, event, 0, MANACOST0, GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 1), NULL, NULL);
}

int card_aid_from_the_cowl(int player, int card, event_t event){
/*
Aid from the Cowl	|3|G|G
Enchantment
Revolt - At the beginning of your end step, if a permanent you controlled left the battlefield this turn,
	reveal the top card of your library. If it's a permanent card, you may put it onto the battlefield.
	Otherwise, you may put it on the bottom of your library.
*/

	if( eot_revolt(player, card, event) ){
		int iid = deck_ptr[player][0];
		if( iid > -1 ){
			int abils[4] = {0, is_what(-1, iid, TYPE_PERMANENT), 1, 1};
			int choice = DIALOG(player, card, EVENT_ACTIVATE, DLG_RANDOM, DLG_NO_STORAGE, DLG_NO_CANCEL, DLG_OMIT_ILLEGAL,
								DLG_SMALLCARD_ID(iid),
								"Put this permanent into play", abils[1], 10,
								"Put this card on bottom", abils[2], internal_rand(5),
								"Do nothing", abils[3], 1);
			if( choice == 1 )
				put_into_play_a_card_from_deck(player, player, 0);
			if( choice == 2 )
				put_top_card_of_deck_to_bottom(player);
		}
	}

	return global_enchantment(player, card, event);
}

int card_airdrop_aeronauts(int player, int card, event_t event){
/*
Airdrop Aeronauts	|3|W|W
Creature - Dwarf Scout 4/3
Flying
Revolt - When ~ enters the battlefield, if a permanent you controlled left the battlefield this turn, you gain 5 life.
*/

	if( trigger_condition == TRIGGER_COMES_INTO_PLAY && affect_me(player, card) && revolt(player) )
		etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		gain_life(player, 5);
	}

	return 0;
}

int card_ajani_unyielding(int player, int card, event_t event){
/*
Ajani Unyielding	|4|G|W
Planeswalker - Ajani (4)
+2: Reveal the top three cards of your library. Put all nonland permanent cards revealed this way into your hand and the rest on the
	bottom of your library in any order.
-2: Exile target creature. Its controller gains life equal to its power.
-9: Put five +1/+1 counters on each creature you control and five loyalty counters on each other planeswalker you control.
*/

	if( ! IS_GAA_EVENT(event) ){
		return planeswalker(player, card, event, 4);
	}

	enum{
		CHOICE_REVEAL = 1,
		CHOICE_EXILE,
		CHOICE_ULTIMATUM,
	};

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance(player, card);

	if (event == EVENT_ACTIVATE ){
		instance->info_slot = instance->number_of_targets = 0;

		int choice = DIALOG(player, card, event, DLG_RANDOM, DLG_PLANESWALKER,
							"Reveal top 3", 					1, 					5, 		+2,
							"Produce RR", 						can_target(&td), 	10, 	-2,
							"Counters for all", 				1, 					20, 	-9);

		if( ! choice ){
			spell_fizzled = 1;
			return 0;
		}

		if( choice == CHOICE_EXILE ){
			new_pick_target(&td, "TARGET_CREATURE", 0, 1);
		}
	}

	if (event == EVENT_RESOLVE_ACTIVATION ){
		int choice = instance->info_slot;

		if( choice == CHOICE_REVEAL ){
			int qty = MIN(3, count_deck(player));
			if( qty ){
				APNAP(p, {show_deck(p, deck_ptr[player], qty, "Cards revealed by Ajani Unyielding", 0, 0x7375B0 );};);
				int i;
				int remaining = qty;
				for(i=0; i<qty; i++){
					if( is_what(-1, deck_ptr[player][i], TYPE_PERMANENT) ){
						add_card_to_hand(player, deck_ptr[player][i]);
						remove_card_from_deck(player, i);
						remaining--;
					}
				}
				if( remaining )
					put_top_x_on_bottom(player, player, remaining);
			}
		}

		if( choice == CHOICE_EXILE && valid_target(&td) ){
			gain_life(instance->targets[0].player, get_power(instance->targets[0].player, instance->targets[0].card));
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_EXILE);
		}

		if( choice == CHOICE_ULTIMATUM ){
			int c = active_cards_count[player]-1;
			while( c > -1 ){
					if( in_play(player, c) ){
						if( is_what(player, c, TYPE_CREATURE) )
							add_counters(player, c, COUNTER_P1_P1, 5);
						if( is_planeswalker(player, c) && !(player == instance->parent_controller && c == instance->parent_card) )
							add_counters(player, c, COUNTER_LOYALTY, 5);
					}
					c--;
			}
		}
	}

	return planeswalker(player, card, event, 4);
}

int card_ajanis_aid(int player, int card, event_t event){
/*
Ajani's Aid	|2|G|W
Enchantment
When ~ enters the battlefield, you may search your library and/or graveyard for a card named Ajani, Valiant Protector, reveal it, and put it
	into your hand. If you search your library this way, shuffle it.
Sacrifice ~: Prevent all combat damage a creature of your choice would deal this turn.
*/

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_ANY, "Select a card named Ajani, Valiant Protector.");
		test.id = CARD_ID_AJANI_VALIANT_PROTECTOR;
		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 0, AI_FIRST_FOUND, &test);
	}

	if( ! IS_GAA_EVENT(event) )
		return global_enchantment(player, card, event);

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.illegal_abilities = 0;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION )
		maze_of_ith_effect(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card);

	return generic_activated_ability(player, card, event, GAA_CAN_TARGET | GAA_SACRIFICE_ME | GAA_LITERAL_PROMPT, MANACOST0,
									0, &td, "Select a creature.");
}

int card_ajanis_comrade(int player, int card, event_t event){
/*
Ajani's Comrade	|1|G
Creature - Elf Soldier 2/2
Trample
At the beginning of combat on your turn, if you control an Ajani planeswalker, put a +1/+1 counter on ~.
*/

	if( beginning_of_combat(player, card, event, player) ){
		int c = active_cards_count[player]-1;
		while( c > -1 ){
				if( in_play(player, c) ){
					if( is_planeswalker(player, c) && has_subtype(player, c, SUBTYPE_AJANI) )
						add_counters(player, c, COUNTER_P1_P1, 1);
				}
				c--;
		}
	}

	return 0;
}


int card_ajani_valiant_protector(int player, int card, event_t event){
/*
Ajani, Valiant Protector	|4|G|W
Planeswalker - Ajani (4)
+2: Put two +1/+1 counters on up to one target creature.
+1: Reveal cards from the top of your library until you reveal a creature card. Put that card into your hand and the rest on the bottom of
	your library in a random order.
-11: Put X +1/+1 counters on target creature, where X is your life total. That creature gains trample until end of turn.
*/

	if( ! IS_GAA_EVENT(event) ){
		return planeswalker(player, card, event, 4);
	}

	enum{
		CHOICE_COUNTERS = 1,
		CHOICE_REVEAL,
		CHOICE_MEGAPUMP,
	};

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance(player, card);

	if (event == EVENT_ACTIVATE ){
		instance->info_slot = instance->number_of_targets = 0;
		instance->targets[0].player = instance->targets[0].card = -1;

		int choice = DIALOG(player, card, event, DLG_RANDOM, DLG_PLANESWALKER,
							"Add 2 +1/+1 counter", 				1, 	10, 			+2,
							"Reveal until you get a creature", 	1,					5, 				+1,
							"Megapump a creature", 				can_target(&td), 	life[player]*2, -11);

		if( ! choice ){
			spell_fizzled = 1;
			return 0;
		}

		if( choice == CHOICE_COUNTERS ){
			td.allow_cancel = 2; //"Done" button
			new_pick_target(&td, "TARGET_CREATURE", 0, 0);
		}

		if( choice == CHOICE_MEGAPUMP ){
			new_pick_target(&td, "TARGET_CREATURE", 0, 1);
		}
	}

	if (event == EVENT_RESOLVE_ACTIVATION ){
		int choice = instance->info_slot;

		if( choice == CHOICE_COUNTERS ){
			if( instance->targets[0].player > -1 && valid_target(&td) )
				add_counters(instance->targets[0].player, instance->targets[0].card, COUNTER_P1_P1, 2);
		}

		if( choice == CHOICE_MEGAPUMP && valid_target(&td) ){
			add_counters(instance->targets[0].player, instance->targets[0].card, COUNTER_P1_P1, life[player]);
			pump_ability_until_eot(instance->parent_controller, instance->parent_card,
									instance->targets[0].player, instance->targets[0].card, 0, 0, KEYWORD_TRAMPLE, 0);
		}

		if( choice == CHOICE_REVEAL ){
			test_definition_t this_test;
			default_test_definition(&this_test, TYPE_CREATURE);
			reveal_card_until_you_reveal_test(player, player, &this_test, RCUYRT_ADD_TEST_TO_HAND | RCUYRT_PUT_REST_ON_BOTTOM_RANDOM);
		}

	}

	return planeswalker(player, card, event, 4);
}

int card_alley_evasion(int player, int card, event_t event){
/*
Alley Evasion	|W
Instant
Choose one -
* Target creature you control gets +1/+2 until end of turn.
* Return target creature you control to its owner's hand.
*/
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	enum{
		CHOICE_PUMP = 1,
		CHOICE_BOUNCE,
	};

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.allowed_controller = td.preferred_controller = player;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_CAN_CAST ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, NULL, 1, NULL);
	}

	if(	event == EVENT_CAST_SPELL && affect_me(player, card) ){
		instance->number_of_targets = 0;
		if( ! check_special_flags2(player, card, SF2_COPIED_FROM_STACK) ){
			int choice = DIALOG(player, card, event, DLG_RANDOM,
								"Pump creature", 1, current_phase == PHASE_AFTER_BLOCKING ? 10 : 1,
								"Bounce creature", 1, 5);
			if( ! choice ){
				spell_fizzled = 1;
				return 0;
			}
		}
		return generic_spell(player, card, event, GS_CAN_TARGET | GS_LITERAL_PROMPT, &td, "Select target creature you control.", 1, NULL);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int choice = instance->info_slot;

		if( choice == CHOICE_PUMP && valid_target(&td) )
			return vanilla_instant_pump(player, card, event, player, player, 1, 2, 0, 0);
		if( choice == CHOICE_BOUNCE && valid_target(&td) )
			bounce_permanent(instance->targets[0].player, instance->targets[0].card);

		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

/*
Alley Strangler	|2|B	0x2009977
Creature - Aetherborn Rogue 2/3
Menace
*/

/*
Audacious Infiltrator	|1|W	0x200c28a
Creature - Dwarf Rogue 3/1
~ can't be blocked by artifact creatures.
*/

/*
Augmenting Automaton	|1	0x2007929
Artifact Creature - Construct 1/1
|1|B: ~ gets +1/+1 until end of turn.
*/


int card_barals_expertise(int player, int card, event_t event){
/*
Baral's Expertise	|3|U|U
Sorcery
Return up to three target artifacts and/or creatures to their owners' hands.
You may cast a card with converted mana cost 4 or less from your hand without paying its mana cost.
*/
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE | TYPE_ARTIFACT);
	td.allow_cancel = 2; //"Done" button

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_SPELL ){
		int validated = 0;
		int i;
		for(i=0; i<instance->number_of_targets; i++){
			if( validate_target(player, card, &td, i) ){
				bounce_permanent(instance->targets[i].player, instance->targets[i].card);
				validated++;
			}
		}
		if( validated )
			av_free_spell(player, 4);
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_OPTIONAL_TARGET | GS_LITERAL_PROMPT, &td, "Select target artifact or crearure.", 3, NULL);
}


int card_baral_chief_of_compliance(int player, int card, event_t event){
/*
Baral, Chief of Compliance	|1|U
Legendary Creature - Human Wizard 1/3
Instant and sorcery spells you cast cost |1 less to cast.
Whenever a spell or ability you control counters a spell, you may draw a card. If you do, discard a card.
*/
	check_legend_rule(player, card, event);

	if( event == EVENT_MODIFY_COST_GLOBAL && affected_card_controller == player && ! is_humiliated(player, card) ){
		if( is_what(affected_card_controller, affected_card, TYPE_SPELL) &&
			! is_what(affected_card_controller, affected_card, TYPE_CREATURE) )
		{
			COST_COLORLESS--;
		}
	}

	return 0;
}

int card_barricade_breaker(int player, int card, event_t event){
/*
Barricade Breaker	|7
Artifact Creature - Juggernaut 7/5
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
~ attacks each combat if able.
*/
	attack_if_able(player, card, event);
	return improvise(player, card, event);
}

/*
Bastion Enforcer	|2|W	0x401000
Creature - Dwarf Soldier 3/2
*/


int card_bastion_inventor(int player, int card, event_t event){
/*
Bastion Inventor	|5|U
Creature - Vedalken Artificer 4/4
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
Hexproof (This creature can't be the target of spells or abilities your opponents control.)
*/
	hexproof(player, card, event);
	return improvise(player, card, event);
}

int card_battle_at_the_bridge(int player, int card, event_t event){
/*
Battle at the Bridge	|X|B
Sorcery
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
Target creature gets -X/-X until end of turn. You gain X life.
*/

	if( ! IS_GS_EVENT(player, card, event) && event != EVENT_MODIFY_COST )
		return 0;

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance( player, card);

	if( event == EVENT_MODIFY_COST )
		return improvise(player, card, event);

	if( event == EVENT_CAN_CAST )
		return generic_spell(player, card, event, GS_X_SPELL | GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		int val = improvise_impl(player, card, event, 1);
		if( val ){
			td.allow_cancel = val == 2 ? 0 : 1;
			return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			int val = instance->info_slot;
			pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, -val, -val);
			gain_life(player, val);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_call_for_unity(int player, int card, event_t event){
/*
Call for Unity	|3|W|W
Enchantment
Revolt - At the beginning of your end step, if a permanent you controlled left the battlefield this turn, put a unity counter on ~.
Creatures you control get +1/+1 for each unity counter on ~.
*/
	if( eot_revolt(player, card, event) ){
		add_counters(player, card, COUNTER_UNITY, 1);
	}

	if( in_play(player, card) && ! is_humiliated(player, card) ){
		boost_creature_type(player, card, event, -1, 1, 1, 0, BCT_CONTROLLER_ONLY);
	}

	return global_enchantment(player, card, event);
}

int card_caught_in_the_brights(int player, int card, event_t event){
/*
Caught in the Brights	|2|W
Enchantment - Aura
Enchant creature
Enchanted creature can't attack or block.
When a Vehicle you control attacks, exile enchanted creature.
*/
	int amt;
	if ((amt = declare_attackers_trigger(player, card, event, DAT_TRACK, player, -1)) && ! is_humiliated(player, card) )
	{
		card_instance_t* instance = get_card_instance(player, card);
		unsigned char* attackers = (unsigned char*)(&instance->targets[2].player);
		int exile = 0;
		for(--amt; amt >= 0; --amt){
			if (in_play(current_turn, attackers[amt]) && has_subtype(current_turn, attackers[amt], SUBTYPE_VEHICLE) ){
				exile = 1;
				break;
			}
		}
		if( exile )
			kill_card(instance->damage_target_player, instance->damage_target_card, KILL_EXILE);
	}

	return card_pacifism(player, card, event);
}

int card_chandras_revolution(int player, int card, event_t event){
/*
Chandra's Revolution	|3|R
Sorcery
~ deals 4 damage to target creature. Tap target land. That land doesn't untap during its controller's next untap step.
*/
	if( ! IS_GS_EVENT(player, card, event) )
		return 0;

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	target_definition_t td2;
	default_target_definition(player, card, &td2, TYPE_LAND);

	card_instance_t *instance = get_card_instance( player, card);

	if( event == EVENT_CAN_CAST && can_target(&td) )
		return generic_spell(player, card, event, GS_CAN_TARGET, &td2, NULL, 1, NULL);

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
		if( spell_fizzled != 1 ){
			new_pick_target(&td2, "TARGER_LAND", 1, 1);
		}
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_target0(player, card, 4);
		}
		if( validate_target(player, card, &td2, 1) ){
			effect_frost_titan(player, card, instance->targets[0].player, instance->targets[0].card);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_cogwork_assembler(int player, int card, event_t event){
/*
Cogwork Assembler	|3
Artifact Creature - Assembly-Worker 2/3
|7: Create a token that's a copy of target artifact. That token gains haste. Exile it at the beginning of the next end step.
*/
	if( ! IS_GAA_EVENT(event) )
		return 0;

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_ARTIFACT);

	card_instance_t *instance = get_card_instance( player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			token_generation_t token;
			copy_token_definition(player, card, &token, instance->targets[0].player, instance->targets[0].card);
			token.legacy = 1;
			token.special_code_for_legacy = &haste_and_remove_eot;
			generate_token(&token);
		}
	}


	return generic_activated_ability(player, card, event, GAA_CAN_TARGET, MANACOST_X(7), 0, &td, "TARGET_ARTIFACT");
}

int card_consulate_crackdown(int player, int card, event_t event){
/*
Consulate Crackdown	|3|W|W
Enchantment
When ~ enters the battlefield, exile all artifacts your opponents control until ~ leaves the battlefield.
*/
	if( comes_into_play(player, card, event) ){
		int c = active_cards_count[1-player]-1;
		while( c > -1 ){
				if( in_play(1-player, c) && is_what(1-player, c, TYPE_ENCHANTMENT) ){
					exile_card_and_remember_it_on_exiledby(player, card, 1-player, c);
				}
				c--;
		}
	}

	if( leaves_play(player, card, event) ){
		int leg = 0;
		int idx = 0;
		int* loc;
		while ((loc = exiledby_find_any(player, card, &leg, &idx)) != NULL){
				int owner = (*loc & 0x80000000) ? 1 : 0;
				int iid = *loc & ~0x80000000;
				*loc = -1;
				int card_added = add_card_to_hand(owner, iid);
				remove_card_from_rfg(owner, cards_data[iid].id);
				put_into_play(owner, card_added);
		}
	}

	return global_enchantment(player, card, event);
}

int card_consulate_dreadnought(int player, int card, event_t event){
/*
Consulate Dreadnought	|1
Artifact - Vehicle 7/11
Crew 6 (Tap any number of creatures you control with total power 6 or more: This Vehicle becomes an artifact creature until end of turn.)
*/
	return vehicle(player, card, event, 6);
}

/* Consulate Turret	|3
 * Artifact
 * |T: You get |E.
 * |T, Pay |E|E|E: ~ deals 2 damage to target player or planeswalker. */

/*
Countless Gears Renegade	|1|W
Creature - Dwarf Artificer 2/2
Revolt - When ~ enters the battlefield, if a permanent you controlled left the battlefield this turn, create a 1/1 colorless Servo artifact creature token.
*/

/*
Crackdown Construct	|4
Artifact Creature - Construct 2/2
Whenever you activate an ability of an artifact or creature that isn't a mana ability, ~ gets +1/+1 until end of turn.
*/

/*
Cruel Finality	|2|B
Instant
Target creature gets -2/-2 until end of turn. Scry 1. (Look at the top card of your library. You may put that card on the bottom of your library.)
*/

/*
Daredevil Dragster	|3
Artifact - Vehicle 4/4
At end of combat, if ~ attacked or blocked this combat, put a velocity counter on it. Then if it has two or more velocity counters on it, sacrifice it and draw two cards.
Crew 2 (Tap any number of creatures you control with total power 2 or more: This Vehicle becomes an artifact creature until end of turn.)
*/

/*
Daring Demolition	|2|B|B
Sorcery
Destroy target creature or Vehicle.
*/

/*
Dark Intimations	|2|U|B|R
Sorcery
Each opponent sacrifices a creature or planeswalker, then discards a card. You return a creature or planeswalker card from your graveyard to your hand, then draw a card.
When you cast a Bolas planeswalker spell, exile ~ from your graveyard. That planeswalker enters the battlefield with an additional loyalty counter on it.
*/

/*
Dawnfeather Eagle	|4|W
Creature - Bird 3/3
Flying
When ~ enters the battlefield, creatures you control get +1/+1 and gain vigilance until end of turn.
*/

/*
Deadeye Harpooner	|2|W
Creature - Dwarf Warrior 2/2
Revolt - When ~ enters the battlefield, if a permanent you controlled left the battlefield this turn, destroy target tapped creature an opponent controls.
*/

/*
Decommission	|2|W
Instant
Destroy target artifact or enchantment.
Revolt - If a permanent you controlled left the battlefield this turn, you gain 3 life.
*/

/*
Defiant Salvager	|2|B
Creature - Aetherborn Artificer 2/2
Sacrifice an artifact or creature: Put a +1/+1 counter on ~. Activate this ability only any time you could cast a sorcery.
*/

/*
Deft Dismissal	|3|W
Instant
~ deals 3 damage divided as you choose among one, two, or three target attacking or blocking creatures.
*/

/*
Destructive Tampering	|2|R
Sorcery
Choose one -
* Destroy target artifact.
* Creatures without flying can't block this turn.
*/

/*
Disallow	|1|U|U
Instant
Counter target spell, activated ability, or triggered ability. (Mana abilities can't be targeted.)
*/

/*
Dispersal Technician	|4|U
Creature - Vedalken Artificer 3/2
When ~ enters the battlefield, you may return target artifact to its owner's hand.
*/

/*
Druid of the Cowl	|1|G	=>m14.c:card_elvish_mystic 0x200abc9
Creature - Elf Druid 1/3
|T: Add |G to your mana pool.
*/

/*
Efficient Construction	|3|U
Enchantment
Whenever you cast an artifact spell, create a 1/1 colorless Thopter artifact creature token with flying.
*/

/*
Embraal Gear-Smasher	|2|R
Creature - Human Warrior 2/3
|T, Sacrifice an artifact: ~ deals 2 damage to each opponent.
*/

/*
Enraged Giant	|5|R
Creature - Giant 4/4
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
Trample, haste
*/

/*
Exquisite Archangel	|5|W|W
Creature - Angel 5/5
Flying
If you would lose the game, instead exile ~ and your life total becomes equal to your starting life total.
*/

/*
Fatal Push	|B
Instant
Destroy target creature if it has converted mana cost 2 or less.
Revolt - Destroy that creature if it has converted mana cost 4 or less instead if a permanent you controlled left the battlefield this turn.
*/

/*
Felidar Guardian	|3|W
Creature - Cat Beast 1/4
When ~ enters the battlefield, you may exile another target permanent you control, then return that card to the battlefield under its owner's control.
*/

/*
Fen Hauler	|6|B
Creature - Insect 5/5
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
~ can't be blocked by artifact creatures.
*/

/*
Filigree Crawler	|4
Artifact Creature - Insect 2/2
When ~ dies, create a 1/1 colorless Thopter artifact creature token with flying.
*/

/*
Foundry Assembler	|5
Artifact Creature - Assembly-Worker 3/3
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
*/

/*
Foundry Hornet	|3|B
Creature - Insect 2/3
Flying
When ~ enters the battlefield, if you control a creature with a +1/+1 counter on it, creatures your opponents control get -1/-1 until end of turn.
*/

/*
Fourth Bridge Prowler	|B
Creature - Human Rogue 1/1
When ~ enters the battlefield, you may have target creature get -1/-1 until end of turn.
*/

/*
Freejam Regent	|4|R|R
Creature - Dragon 4/4
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
Flying
|1|R: ~ gets +2/+0 until end of turn.
*/

/*
Frontline Rebel	|2|R	=>m10.c:card_berserkers_of_blood_ridge 0x20025c8
Creature - Human Warrior 3/3
~ attacks each combat if able.
*/

/*
Ghirapur Osprey	|2|W	0x401000
Creature - Bird 2/2
Flying
*/

/*
Gifted Aetherborn	|B|B	=>zendikar.c:card_vampire_nighthawk	0x2001afb
Creature - Aetherborn Vampire 2/3
Deathtouch, lifelink
*/

/*
Glint-Sleeve Siphoner	|1|B
Creature - Human Rogue 2/1
Menace
Whenever ~ enters the battlefield or attacks, you get |E (an energy counter).
At the beginning of your upkeep, you may pay |E|E. If you do, you draw a card and you lose 1 life.
*/

/*
Gonti's Aether Heart	|6
Legendary Artifact
Whenever ~ or another artifact enters the battlefield under your control, you get |E|E (two energy counters).
Pay |E|E|E|E|E|E|E|E, Exile ~: Take an extra turn after this one.
*/

/*
Gonti's Machinations	|B
Enchantment
Whenever you lose life for the first time each turn, you get |E. (You get an energy counter. Damage causes loss of life.)
Pay |E|E, Sacrifice ~: Each opponent loses 3 life. You gain life equal to the life lost this way.
*/

/*
Greenbelt Rampager	|G
Creature - Elephant 3/4
When ~ enters the battlefield, pay |E|E (two energy counters). If you can't, return ~ to its owner's hand and you get |E.
*/

/*
Greenwheel Liberator	|1|G
Creature - Elf Warrior 2/1
Revolt - ~ enters the battlefield with two +1/+1 counters on it if a permanent you controlled left the battlefield this turn.
*/

/*
Gremlin Infestation	|3|R
Enchantment - Aura
Enchant artifact
At the beginning of your end step, ~ deals 2 damage to enchanted artifact's controller.
When enchanted artifact is put into a graveyard, create a 2/2 |Sred Gremlin creature token.
*/

/*
Heart of Kiran	|2
Legendary Artifact - Vehicle 4/4
Flying, vigilance
Crew 3 (Tap any number of creatures you control with total power 3 or more: This Vehicle becomes an artifact creature until end of turn.)
You may remove a loyalty counter from a planeswalker you control rather than pay ~'s crew cost.
*/

/*
Herald of Anguish	|5|B|B
Creature - Demon 5/5
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
Flying
At the beginning of your end step, each opponent discards a card.
|1|B, Sacrifice an artifact: Target creature gets -2/-2 until end of turn.
*/

/*
Heroic Intervention	|1|G
Instant
Permanents you control gain hexproof and indestructible until end of turn.
*/

/*
Hidden Herbalists	|1|G
Creature - Human Druid 2/2
Revolt - When ~ enters the battlefield, if a permanent you controlled left the battlefield this turn, add |G|G to your mana pool.
*/

/*
Hidden Stockpile	|W|B
Enchantment
Revolt - At the beginning of your end step, if a permanent you controlled left the battlefield this turn, create a 1/1 colorless Servo artifact creature token.
|1, Sacrifice a creature: Scry 1.
*/

/*
Highspire Infusion	|1|G
Instant
Target creature gets +3/+3 until end of turn. You get |E|E (two energy counters).
*/

/*
Hinterland Drake	|2|U
Creature - Drake 2/3
Flying
~ can't block artifact creatures.
*/

/*
Hope of Ghirapur	|1
Legendary Artifact Creature - Thopter 1/1
Flying
Sacrifice ~: Until your next turn, target player who was dealt combat damage by ~ this turn can't cast noncreature spells.
*/

/* Hungry Flames	|2|R
 * Instant
 * ~ deals 3 damage to target creature and 2 damage to target player or planeswalker. */

/*
Ice Over	|1|U
Enchantment - Aura
Enchant artifact or creature
Enchanted permanent doesn't untap during its controller's untap step.
*/

/*
Illusionist's Stratagem	|3|U
Instant
Exile up to two target creatures you control, then return those cards to the battlefield under their owner's control.
Draw a card.
*/

/* Implement of Combustion	|1
 * Artifact
 * |R, Sacrifice ~: It deals 1 damage to target player or planeswalker.
 * When ~ is put into a graveyard from the battlefield, draw a card. */

/*
Implement of Examination	|3
Artifact
|U, Sacrifice ~: Draw a card.
When ~ is put into a graveyard from the battlefield, draw a card.
*/

/*
Implement of Ferocity	|1
Artifact
|G, Sacrifice ~: Put a +1/+1 counter on target creature. Activate this ability only any time you could cast a sorcery.
When ~ is put into a graveyard from the battlefield, draw a card.
*/

/*
Implement of Improvement	|1
Artifact
|W, Sacrifice ~: You gain 2 life.
When ~ is put into a graveyard from the battlefield, draw a card.
*/

/*
Implement of Malice	|2
Artifact
|B, Sacrifice ~: Target player discards a card. Activate this ability only any time you could cast a sorcery.
When ~ is put into a graveyard from the battlefield, draw a card.
*/

/*
Indomitable Creativity	|X|R|R|R
Sorcery
Destroy X target artifacts and/or creatures. For each permanent destroyed this way, its controller reveals cards from the top of his or her library until an artifact or creature card is revealed and exiles that card. Those players put the exiled cards onto the battlefield, then shuffle their libraries.
*/

/*
Inspiring Roar	|3|W
Sorcery
Put a +1/+1 counter on each creature you control.
*/

/*
Inspiring Statuary	|3
Artifact
Nonartifact spells you cast have improvise. (Your artifacts can help cast those spells. Each artifact you tap after you're done activating mana abilities pays for |1.)
*/

/*
Invigorated Rampage	|1|R
Instant
Choose one -
* Target creature gets +4/+0 and gains trample until end of turn.
* Two target creatures each get +2/+0 and gain trample until end of turn.
*/

/*
Ironclad Revolutionary	|4|B|B
Creature - Aetherborn Artificer 4/4
When ~ enters the battlefield, you may sacrifice an artifact. If you do, put two +1/+1 counters on ~ and each opponent loses 2 life.
*/

/*
Irontread Crusher	|4
Artifact - Vehicle 6/6
Crew 3 (Tap any number of creatures you control with total power 3 or more: This Vehicle becomes an artifact creature until end of turn.)
*/

/*
Kari Zev's Expertise	|1|R|R
Sorcery
Gain control of target creature or Vehicle until end of turn. Untap it. It gains haste until end of turn.
You may cast a card with converted mana cost 2 or less from your hand without paying its mana cost.
*/

/*
Kari Zev, Skyship Raider	|1|R
Legendary Creature - Human Pirate 1/3
First strike, menace
Whenever ~ attacks, create a legendary 2/1 |Sred Monkey creature token named Ragavan that's tapped and attacking. Exile that token at end of combat.
*/

/*
Lathnu Sailback	|4|R	0x401000
Creature - Lizard 5/4
*/

/*
Leave in the Dust	|3|U
Instant
Return target nonland permanent to its owner's hand.
Draw a card.
*/

/*
Lifecraft Awakening	|X|G
Instant
Put X +1/+1 counters on target artifact you control. If it isn't a creature or Vehicle, it becomes a 0/0 Construct artifact creature.
*/

/*
Lifecraft Cavalry	|4|G
Creature - Elf Warrior 4/4
Trample
Revolt - ~ enters the battlefield with two +1/+1 counters on it if a permanent you controlled left the battlefield this turn.
*/

/*
Lifecrafter's Bestiary	|3
Artifact
At the beginning of your upkeep, scry 1.
Whenever you cast a creature spell, you may pay |G. If you do, draw a card.
*/

/*
Lifecrafter's Gift	|3|G
Instant
Put a +1/+1 counter on target creature, then put a +1/+1 counter on each creature you control with a +1/+1 counter on it.
*/

/*
Lightning Runner	|3|R|R
Creature - Human Warrior 2/2
Double strike, haste
Whenever ~ attacks, you get |E|E (two energy counters), then you may pay |E|E|E|E|E|E|E|E. If you pay, untap all creatures you control, and after this phase, there is an additional combat phase.
*/

/*
Maulfist Revolutionary	|1|G|G
Creature - Human Warrior 3/3
Trample
When ~ enters the battlefield or dies, for each kind of counter on target permanent or player, give that permanent or player another counter of that kind.
*/

/*
Maverick Thopterist	|3|U|R
Creature - Human Artificer 2/2
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
When ~ enters the battlefield, create two 1/1 colorless Thopter artifact creature tokens with flying.
*/

/*
Mechanized Production	|2|U|U
Enchantment - Aura
Enchant artifact you control
At the beginning of your upkeep, create a token that's a copy of enchanted artifact. Then if you control eight or more artifacts with the same name as one another, you win the game.
*/

/*
Merchant's Dockhand	|1
Artifact Creature - Construct 1/2
|3|U, |T, Tap X untapped artifacts you control: Look at the top X cards of your library. Put one of them into your hand and the rest on the bottom of your library in any order.
*/

/*
Metallic Mimic	|2
Artifact Creature - Shapeshifter 2/1
As ~ enters the battlefield, choose a creature type.
~ is the chosen type in addition to its other types.
Each other creature you control of the chosen type enters the battlefield with an additional +1/+1 counter on it.
*/

/*
Metallic Rebuke	|2|U
Instant
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
Counter target spell unless its controller pays |3.
*/

/*
Midnight Entourage	|2|B|B
Creature - Aetherborn Rogue 3/3
Other Aetherborn you control get +1/+1.
Whenever ~ or another Aetherborn you control dies, you draw a card and you lose 1 life.
*/

/*
Mobile Garrison	|3
Artifact - Vehicle 3/4
Whenever ~ attacks, untap another target artifact or creature you control.
Crew 2 (Tap any number of creatures you control with total power 2 or more: This Vehicle becomes an artifact creature until end of turn.)
*/

/*
Monstrous Onslaught	|3|G|G
Sorcery
~ deals X damage divided as you choose among any number of target creatures, where X is the greatest power among creatures you control as you cast ~.
*/

/*
Narnam Renegade	|G
Creature - Elf Warrior 1/2
Deathtouch
Revolt - ~ enters the battlefield with a +1/+1 counter on it if a permanent you controlled left the battlefield this turn.
*/

/*
Natural Obsolescence	|1|G
Instant
Put target artifact on the bottom of its owner's library.
*/

/*
Night Market Aeronaut	|3|B
Creature - Aetherborn Warrior 2/2
Flying
Revolt - ~ enters the battlefield with a +1/+1 counter on it if a permanent you controlled left the battlefield this turn.
*/

/*
Night Market Guard	|3	=>unlimited.c:card_two_headed_giant_of_foriys 0x200b263
Artifact Creature - Construct 3/1
~ can block an additional creature each combat.
*/

/*
Oath of Ajani	|G|W
Legendary Enchantment
When ~ enters the battlefield, put a +1/+1 counter on each creature you control.
Planeswalker spells you cast cost |1 less to cast.
*/

/*
Outland Boar	|2|R|G
Creature - Boar 4/4
~ can't be blocked by creatures with power 2 or less.
*/

/*
Pacification Array	|1
Artifact
|2, |T: Tap target artifact or creature.
*/

/*
Paradox Engine	|5
Legendary Artifact
Whenever you cast a spell, untap all nonland permanents you control.
*/

/*
Peacewalker Colossus	|3
Artifact - Vehicle 6/6
|1|W: Another target Vehicle you control becomes an artifact creature until end of turn.
Crew 4 (Tap any number of creatures you control with total power 4 or more: This Vehicle becomes an artifact creature until end of turn.)
*/

/*
Peema Aether-Seer	|3|G
Creature - Elf Druid 3/2
When ~ enters the battlefield, you get an amount of |E (energy counters) equal to the greatest power among creatures you control.
Pay |E|E|E: Target creature blocks this turn if able.
*/

/*
Pendulum of Patterns	|2
Artifact
When ~ enters the battlefield, you gain 3 life.
|5, |T, Sacrifice ~: Draw a card.
*/

/*
Perilous Predicament	|4|B
Instant
Each opponent sacrifices an artifact creature and a nonartifact creature.
*/

/*
Pia's Revolution	|2|R
Enchantment
Whenever a nontoken artifact is put into your graveyard from the battlefield, return that card to your hand unless target opponent has ~ deal 3 damage to him or her.
*/

/*
Planar Bridge	|6
Legendary Artifact
|8, |T: Search your library for a permanent card, put it onto the battlefield, then shuffle your library.
*/

/*
Precise Strike	|R	=>m13.c:card_kindled_fury 0x20067b3
Instant
Target creature gets +1/+0 and gains first strike until end of turn.
*/

/*
Prizefighter Construct	|5	0x401000
Artifact Creature - Construct 6/2
*/

/* Quicksmith Rebel	|3|R
 * Creature - Human Artificer 3/2
 * When ~ enters the battlefield, target artifact you control gains "|T: This artifact deals 2 damage to any target" for as long as you control ~. */

/*
Quicksmith Spy	|3|U
Creature - Human Artificer 2/3
When ~ enters the battlefield, target artifact you control gains "|T: Draw a card" for as long as you control ~.
*/

/*
Ravenous Intruder	|1|R	=>antiquities.c:card_atog2 0x200c2a8
Creature - Gremlin 1/2
Sacrifice an artifact: ~ gets +2/+2 until end of turn.
*/

/*
Reckless Racer	|2|R
Creature - Human Pilot 2/3
First strike
Whenever ~ becomes tapped, you may discard a card. If you do, draw a card.
*/

/*
Release the Gremlins	|X|X|R
Sorcery
Destroy X target artifacts. Create X 2/2 |Sred Gremlin creature tokens.
*/

/*
Renegade Map	|1
Artifact
~ enters the battlefield tapped.
|T, Sacrifice ~: Search your library for a basic land card, reveal it, put it into your hand, then shuffle your library.
*/

/*
Renegade Rallier	|1|G|W
Creature - Human Warrior 3/2
Revolt - When ~ enters the battlefield, if a permanent you controlled left the battlefield this turn, return target permanent card with converted mana cost 2 or less from your graveyard to the battlefield.
*/

/*
Renegade Wheelsmith	|1|R|W
Creature - Dwarf Pilot 3/2
Whenever ~ becomes tapped, target creature can't block this turn.
*/

/*
Renegade's Getaway	|2|B
Instant
Target permanent gains indestructible until end of turn. Create a 1/1 colorless Servo artifact creature token. (Effects that say "destroy" don't destroy a permanent with indestructible, and if it's a creature, it can't be destroyed by damage.)
*/

/*
Reservoir Walker	|5
Artifact Creature - Construct 3/3
When ~ enters the battlefield, you gain 3 life and get |E|E|E (three energy counters).
*/

/*
Resourceful Return	|1|B
Sorcery
Return target creature card from your graveyard to your hand. If you control an artifact, draw a card.
*/

/*
Restoration Specialist	|1|W
Creature - Dwarf Artificer 2/1
|W, Sacrifice ~: Return up to one target artifact card and up to one target enchantment card from your graveyard to your hand.
*/

/*
Reverse Engineer	|3|U|U
Sorcery
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
Draw three cards.
*/

/*
Ridgescale Tusker	|3|G|G
Creature - Beast 5/5
When ~ enters the battlefield, put a +1/+1 counter on each other creature you control.
*/

/*
Rishkar's Expertise	|4|G|G
Sorcery
Draw cards equal to the greatest power among creatures you control.
You may cast a card with converted mana cost 5 or less from your hand without paying its mana cost.
*/

/*
Rishkar, Peema Renegade	|2|G
Legendary Creature - Elf Druid 2/2
When ~ enters the battlefield, put a +1/+1 counter on each of up to two target creatures.
Each creature you control with a counter on it has "|T: Add |G to your mana pool."
*/

/*
Rogue Refiner	|1|G|U
Creature - Human Rogue 3/2
When ~ enters the battlefield, draw a card and you get |E|E (two energy counters).
*/

/*
Salvage Scuttler	|4|U
Creature - Crab 4/4
Whenever ~ attacks, return an artifact you control to its owner's hand.
*/

/*
Scrap Trawler	|3
Artifact Creature - Construct 3/2
Whenever ~ or another artifact you control is put into a graveyard from the battlefield, return to your hand target artifact card in your graveyard with lesser converted mana cost.
*/

/*
Scrapper Champion	|3|R	=>kaladesh.c:card_thriving_ibex 0x200f16f
Creature - Human Artificer 2/2
Double strike (This creature deals both first-strike and regular combat damage.)
When ~ enters the battlefield, you get |E|E (two energy counters).
Whenever ~ attacks, you may pay |E|E. If you do, put a +1/+1 counter on it.
*/

/*
Scrounging Bandar	|1|G
Creature - Cat Monkey 0/0
~ enters the battlefield with two +1/+1 counters on it.
At the beginning of your upkeep, you may move any number of +1/+1 counters from ~ onto another target creature.
*/

/*
Secret Salvage	|3|B|B
Sorcery
Exile target nonland card from your graveyard. Search your library for any number of cards with the same name as that card, reveal them, and put them into your hand. Then shuffle your library.
*/

/*
Servo Schematic	|2
Artifact
When ~ enters the battlefield or is put into a graveyard from the battlefield, create a 1/1 colorless Servo artifact creature token.
*/

/*
Shielded Aether Thief	|1|U
Creature - Vedalken Rogue 0/4
Flash (You may cast this spell any time you could cast an instant.)
Whenever ~ blocks, you get |E (an energy counter).
|T, Pay |E|E|E: Draw a card.
*/

/*
Shipwreck Moray	|3|U
Creature - Fish 0/5
When ~ enters the battlefield, you get |E|E|E|E (four energy counters).
Pay |E: ~ gets +2/-2 until end of turn.
*/

/*
Siege Modification	|1|R|R
Enchantment - Aura
Enchant creature or Vehicle
As long as enchanted permanent is a Vehicle, it's a creature in addition to its other types.
Enchanted creature gets +3/+0 and has first strike.
*/

/*
Silkweaver Elite	|2|G
Creature - Elf Archer 2/2
Reach (This creature can block creatures with flying.)
Revolt - When ~ enters the battlefield, if a permanent you controlled left the battlefield this turn, draw a card.
*/

/*
Skyship Plunderer	|1|U
Creature - Human Pirate 2/1
Flying
Whenever ~ deals combat damage to a player, for each kind of counter on target permanent or player, give that permanent or player another counter of that kind.
*/

/*
Sly Requisitioner	|4|B
Creature - Human Artificer 2/2
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
Whenever a nontoken artifact you control is put into a graveyard from the battlefield, create a 1/1 colorless Servo artifact creature token.
*/

/*
Solemn Recruit	|1|W|W
Creature - Dwarf Warrior 2/2
Double strike
Revolt - At the beginning of your end step, if a permanent you controlled left the battlefield this turn, put a +1/+1 counter on ~.
*/

/*
Spire Patrol	|2|W|U	=>m15.c:card_frost_lynx 0x200c8c0
Creature - Human Soldier 3/2
Flying
When ~ enters the battlefield, tap target creature an opponent controls. That creature doesn't untap during its controller's next untap step.
*/

/*
Spire of Industry	""
Land
|T: Add |C to your mana pool.
|T, Pay 1 life: Add one mana of any color to your mana pool. Activate this ability only if you control an artifact.
*/

/*
Sram's Expertise	|2|W|W
Sorcery
Create three 1/1 colorless Servo artifact creature tokens.
You may cast a card with converted mana cost 3 or less from your hand without paying its mana cost.
*/

/*
Sram, Senior Edificer	|1|W
Legendary Creature - Dwarf Advisor 2/2
Whenever you cast an Aura, Equipment, or Vehicle spell, draw a card.
*/

/*
Sweatworks Brawler	|3|R
Creature - Human Artificer 3/3
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
Menace
*/

/*
Take into Custody	|U
Instant
Tap target creature. It doesn't untap during its controller's next untap step.
*/

/*
Tezzeret the Schemer	|2|U|B
Planeswalker - Tezzeret (5)
+1: Create a colorless artifact token named Etherium Cell with "|T, Sacrifice this artifact: Add one mana of any color to your mana pool."
-2: Target creature gets +X/-X until end of turn, where X is the number of artifacts you control.
-7: You get an emblem with "At the beginning of combat on your turn, target artifact you control becomes an artifact creature with base power and toughness 5/5."
*/

/*
Tezzeret the Schemer Emblem	""
Emblem - Tezzeret
At the beginning of combat on your turn, target artifact you control becomes an artifact creature with base power and toughness 5/5.
*/

/*
Tezzeret's Betrayal	|3|U|B
Sorcery
Destroy target creature. You may search your library and/or graveyard for a card named Tezzeret, Master of Metal, reveal it, and put it into your hand. If you search your library this way, shuffle it.
*/

/*
Tezzeret's Simulacrum	|3
Artifact Creature - Golem 2/3
|T: Target opponent loses 1 life. If you control a Tezzeret planeswalker, that player loses 3 life instead.
*/

/*
Tezzeret's Touch	|1|U|B
Enchantment - Aura
Enchant artifact
Enchanted artifact is a creature with base power and toughness 5/5 in addition to its other types.
When enchanted artifact is put into a graveyard, return that card to its owner's hand.
*/

/*
Tezzeret, Master of Metal	|4|U|B
Planeswalker - Tezzeret (5)
+1: Reveal cards from the top of your library until you reveal an artifact card. Put that card into your hand and the rest on the bottom of your library in a random order.
-3: Target opponent loses life equal to the number of artifacts you control.
-8: Gain control of all artifacts and creatures target opponent controls.
*/

/*
Thopter Arrest	|2|W
Enchantment
When ~ enters the battlefield, exile target artifact or creature an opponent controls until ~ leaves the battlefield.
*/

/*
Treasure Keeper	|4
Artifact Creature - Construct 3/3
When ~ dies, reveal cards from the top of your library until you reveal a nonland card with converted mana cost 3 or less. You may cast that card without paying its mana cost. Put all revealed cards not cast this way on the bottom of your library in a random order.
*/

/*
Trophy Mage	|2|U
Creature - Human Wizard 2/2
When ~ enters the battlefield, you may search your library for an artifact card with converted mana cost 3, reveal it, put it into your hand, then shuffle your library.
*/

/*
Unbridled Growth	|G
Enchantment - Aura
Enchant land
Enchanted land has "|T: Add one mana of any color to your mana pool."
Sacrifice ~: Draw a card.
*/

/*
Universal Solvent	|1
Artifact
|7, |T, Sacrifice ~: Destroy target permanent.
*/

/*
Untethered Express	|4
Artifact - Vehicle 4/4
Trample
Whenever ~ attacks, put a +1/+1 counter on it.
Crew 1 (Tap any number of creatures you control with total power 1 or more: This Vehicle becomes an artifact creature until end of turn.)
*/

/*
Vengeful Rebel	|2|B
Creature - Aetherborn Warrior 3/2
Revolt - When ~ enters the battlefield, if a permanent you controlled left the battlefield this turn, target creature an opponent controls gets -3/-3 until end of turn.
*/

/*
Verdant Automaton	|2
Artifact Creature - Construct 1/2
|3|G: Put a +1/+1 counter on ~.
*/

/* Walking Ballista	|X|X
 * Artifact Creature - Construct 0/0
 * ~ enters the battlefield with X +1/+1 counters on it.
 * |4: Put a +1/+1 counter on ~.
 * Remove a +1/+1 counter from ~: It deals 1 damage to any target. */

/*
Watchful Automaton	|3
Artifact Creature - Construct 2/2
|2|U: Scry 1. (Look at the top card of your library. You may put that card on the bottom of your library.)
*/

/*
Welder Automaton	|2
Artifact Creature - Construct 2/1
|3|R: ~ deals 1 damage to each opponent.
*/

/*
Weldfast Engineer	|1|B|R
Creature - Human Artificer 3/3
At the beginning of combat on your turn, target artifact creature you control gets +2/+0 until end of turn.
*/

/*
Whir of Invention	|X|U|U|U
Instant
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
Search your library for an artifact card with converted mana cost X or less, put it onto the battlefield, then shuffle your library.
*/

/*
Wind-Kin Raiders	|4|U|U
Creature - Human Artificer 4/3
Improvise (Your artifacts can help cast this spell. Each artifact you tap after you're done activating mana abilities pays for |1.)
Flying
*/

/*
Winding Constrictor	|B|G
Creature - Snake 2/3
If one or more counters would be placed on an artifact or creature you control, that many plus one of each of those kinds of counters are placed on that permanent instead.
If you would get one or more counters, you get that many plus one of each of those kinds of counters instead.
*/

/*
Wrangle	|1|R
Sorcery
Gain control of target creature with power 4 or less until end of turn. Untap that creature. It gains haste until end of turn.
*/

/*
Yahenni's Expertise	|2|B|B
Sorcery
All creatures get -3/-3 until end of turn.
You may cast a card with converted mana cost 3 or less from your hand without paying its mana cost.
*/

/*
Yahenni, Undying Partisan	|2|B
Legendary Creature - Aetherborn Vampire 2/2
Haste
Whenever a creature an opponent controls dies, put a +1/+1 counter on ~.
Sacrifice another creature: ~ gains indestructible until end of turn.
*/

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *non_mana_source(int who_chooses, int player, int card);
static int aer_implement(int player, int card, event_t event);
#ifndef SHANDALAR
static void av_free_spell(int player, int max_cmc);
#endif


/* ManalinkEh 2022: 0x20ba7d0; original cards/aether_revolt.c:120. */
static char *non_mana_source(int who_chooses, int player, int card)
{
  card_instance_t *instance;
  char *pcVar2;
  instance = get_card_instance(player, card);
  pcVar2 = NULL;
  if ((cards_data[instance->internal_card_id].extra_ability & 0x1000) != 0)
  {
    pcVar2 = "must not be a mana source.";
  }
  return pcVar2;
}

/* ManalinkEh 2022: 0x20ba6c0; original cards/aether_revolt.c:127. */
static void modify_cost_for_improvise(int player, int card, event_t event)
{
  card_ptr_t *pcVar1;
  int iVar2;
  int iVar3;
  target_definition_2022_t td;

  default_target_definition(player, card, &td, 0x40);
  td.illegal_state = 1;
  td.illegal_abilities = 0;
  if (player == 1)
  {
    td.special = 0x8000;
    td.extra = (int32_t)non_mana_source;
  }
  td.allowed_controller = player;
  iVar2 = get_id(player, card);
  pcVar1 = cards_ptr[iVar2];
  iVar2 = get_updated_casting_cost(player, card, -1, event, -1);
  iVar3 = target_available(player, card, &td);
  iVar2 = has_mana_multi(player, iVar2 - iVar3 & (int)(iVar2 - iVar3 ^ 0xffffffffU) >> 0x1f,
                         (unsigned int)pcVar1->req_black, (unsigned int)pcVar1->req_blue,
                         (unsigned int)pcVar1->req_green, (unsigned int)pcVar1->req_red,
                         (unsigned int)pcVar1->req_white);
  if (iVar2 == 0)
  {
    return;
  }
  null_casting_cost(player, card);
  return;
}

/* ManalinkEh 2022: 0x20bf7d0; original cards/aether_revolt.c:1249. */
int card_countless_gears_renegade(int player, int card, event_t event)
{
  int result;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event == EVENT_ETB)
  {
    result = get_trap_condition(player, 0x60c);
    if (result != 0)
    {
      make_servo(player, card, 1);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20bf940; original cards/aether_revolt.c:1287. */
int card_cruel_finality(int player, int card, event_t event)
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
    else if (event == EVENT_RESOLVE_SPELL)
    {
      default_target_definition(player, card, &td, 2);
      scry(player, 1);
      kill_card(player, card, KILL_DESTROY);
    }
    else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
    {
      return 0;
    }
  }
  result = vanilla_instant_pump(player, card, event, 2, 1 - player, -2, -2, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x20bfd10; original cards/aether_revolt.c:1365. */
int card_dark_intimations(int player, int card, event_t event)
{
  int result;
  test_definition_t test;
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
        new_default_test_definition(&test, 0x102, "Select a creature or planeswalker.");
        if (event == EVENT_RESOLVE_SPELL)
        {
          new_sacrifice(player, card, 1 - player, SAC_CAUSED | SAC_NO_CANCEL, &test);
          discard(1 - player, 0, player);
          result = count_graveyard_by_type(player, 0x1006);
          if (0 < result)
          {
            new_global_tutor(player, player, 2, 1, 1, 1, &test);
          }
          draw_a_card(player);
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_020bfd60;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  new_default_test_definition(&test, 0x102, "Select a creature or planeswalker.");
LAB_020bfd60:
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20bfe80; original cards/aether_revolt.c:1392. */
int card_dawnfeather_eagle(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  pump_subtype_until_eot(player, card, player, -1, 1, 1, 0, 8);
  return 0;
}

/* ManalinkEh 2022: 0x20bff10; original cards/aether_revolt.c:1410. */
int card_deadeye_harpooner(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  instance = get_card_instance(player, card);
  default_target_definition(player, card, &td, 2);
  td.allowed_controller = 1 - player;
  td.required_state = 1;
  td.preferred_controller = td.allowed_controller;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Select target tapped creature an opponent controls.", 0, 0x80);
      if (result != 0)
      {
        result = get_trap_condition(player, 0x60c);
        if (result != 0)
        {
          kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        }
      }
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x20c0000; original cards/aether_revolt.c:1435. */
int card_decommission(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x44);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            result = get_trap_condition(player, 0x60c);
            if (result != 0)
            {
              gain_life(player, 3);
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_020c0066;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x44);
  get_card_instance(player, card);
LAB_020c0066:
  result = generic_spell(player, card, event, 1, &td, "DISENCHANT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c0630; original cards/aether_revolt.c:1546. */
int card_dispersal_technician(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 0x40);
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "TARGET_ARTIFACT", 0, 0);
    if (result != 0)
    {
      bounce_permanent(instance->targets[0].player, instance->targets[0].card);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x20c0bc0; original cards/aether_revolt.c:1669. */
int card_fatal_push(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  char *prompt;
  target_definition_t td;
  if (event == EVENT_CAN_CAST)
  {
  LAB_020c0c6c:
    result = get_trap_condition(player, 0x60c);
    if (result == 0)
    {
      default_target_definition(player, card, &td, 2);
      td.extra = 2;
      td.special = 0x800;
    LAB_020c0d3c:
      prompt = "Select target creature with CMC 2 or less.";
      goto LAB_020c0cc8;
    }
    default_target_definition(player, card, &td, 2);
    td.extra = 4;
    td.special = 0x800;
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
      goto LAB_020c0c6c;
    }
    if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      goto LAB_020c0c6c;
    }
    result = get_trap_condition(player, 0x60c);
    if (result == 0)
    {
      default_target_definition(player, card, &td, 2);
      td.extra = 2;
      td.special = 0x800;
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = valid_target(&td);
        if (result != 0)
        {
          instance = get_card_instance(player, card);
          kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_020c0d3c;
    }
    default_target_definition(player, card, &td, 2);
    td.extra = 4;
    td.special = 0x800;
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  prompt = "Select target creature with CMC 4 or less.";
LAB_020c0cc8:
  result = generic_spell(player, card, event, 0x81, &td, prompt, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c0e10; original cards/aether_revolt.c:1715. */
int card_felidar_guardian(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 0x47);
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.special = 0x4000;
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "Select target permanent you control.", 0, 0x80);
    if (result != 0)
    {
      blink_effect_controller(instance->targets[0].player, instance->targets[0].card, NULL);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x20c1010; original cards/aether_revolt.c:1783. */
int card_foundry_hornet(int player, int card, event_t event)
{
  int result;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = has_any_counters(player, COUNTER_P1_P1, TYPE_CREATURE);
    if (result != 0)
    {
      pump_subtype_until_eot(player, card, 1 - player, -1, -1, -1, 0, 0);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c10b0; original cards/aether_revolt.c:1801. */
int card_fourth_bridge_prowler(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td, 2);
  instance = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "TARGET_CREATURE", 0, 0);
    if (result != 0)
    {
      pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, -1, -1);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x20c1660; original cards/aether_revolt.c:1941. */
int card_greenwheel_liberator(int player, int card, event_t event)
{
  int result;
  result = get_trap_condition(player, 0x60c);
  if (result == 0)
  {
    return 0;
  }
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, 2);
  return 0;
}

/* ManalinkEh 2022: 0x20c20b0; original cards/aether_revolt.c:2148. */
int card_highspire_infusion(int player, int card, event_t event)
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
        td.preferred_controller = player;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 3,
                           3);
            add_energy(player, 2);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_020c2119;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  get_card_instance(player, card);
LAB_020c2119:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c22c0; original cards/aether_revolt.c:2200. */
int card_hope_of_ghirapur(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  check_legend_rule(player, card, event);
  check_damage(player, card, event, DDBM_TRACE_DAMAGED_PLAYERS | DDBM_MUST_DAMAGE_PLAYER, player,
               card);
  if (event == EVENT_CLEANUP)
  {
    instance = get_card_instance(player, card);
    instance->targets[1].card = 0;
    instance->targets[1].player = 0;
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) && (player == 1))
    {
      instance = get_card_instance(1, card);
      if (instance->targets[1].player < 1)
      {
        return 0;
      }
      result = generic_activated_ability(1, card, EVENT_CAN_ACTIVATE, 8, 0, 0, 0, 0, 0, 0, 0, NULL,
                                         NULL);
      return result;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (instance = get_card_instance(player, card), 0 < instance->targets[1].player))
    {
      result = generate_reserved_token_by_id(player, 0x6160);
      instance = get_card_instance(player, result);
      instance->targets[0].player = 1 - player;
      instance->targets[0].card = 5;
      instance->targets[1].player = 0x38;
      instance->targets[1].card = 0x4153;
      instance->targets[2].player = 1;
      put_into_play(player, result);
      create_card_name_legacy(player, result, 0x4153);
    }
  }
  result = generic_activated_ability(player, card, event, 8, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c2b30; original cards/aether_revolt.c:2328. */
int card_illusionists_strategem(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  int iVar3;
  card_instance_t *instance;
  int iVar5;
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
        td.preferred_controller = player;
        td.allowed_controller = player;
        if (event == EVENT_RESOLVE_SPELL)
        {
          instance = get_card_instance(player, card);
          if (instance->number_of_targets != '\0')
          {
            iVar3 = 0;
            do
            {
              while (iVar5 = validate_target(player, card, &td, iVar3), iVar5 == 0)
              {
                iVar3 = iVar3 + 1;
                if ((int)(unsigned int)instance->number_of_targets <= iVar3)
                  goto LAB_020c2c91;
              }
              ptVar2 = instance->targets + iVar3;
              ptVar1 = instance->targets + iVar3;
              iVar3 = iVar3 + 1;
              blink_effect(ptVar1->player, ptVar2->card, NULL);
            } while (iVar3 < (int)(unsigned int)instance->number_of_targets);
          }
        LAB_020c2c91:
          draw_cards(player, 1);
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_020c2b8d;
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
LAB_020c2b8d:
  iVar3 = generic_spell(player, card, event, 0x21, &td, "ASHNODS_BATTLEGEAR", 2, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x20ba990; original cards/aether_revolt.c:2358. */
static int aer_implement(int player, int card, event_t event)
{
  int result;
  result = graveyard_from_play(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  draw_cards(player, 1);
  return 0;
}

/* ManalinkEh 2022: 0x20c2cd0; original cards/aether_revolt.c:2367. */
int card_implement_of_combustion(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x100);
    td.zone = 0x1000;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = aer_implement(player, card, event);
      return result;
    }
    default_target_definition(player, card, &td, 0x100);
    td.zone = 0x1000;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        damage_target0(player, card, 1);
      }
    }
  }
  result = generic_activated_ability(player, card, event, 0xc, 0, 0, 0, 0, 1, 0, 0, &td,
                                     "TARGET_PLAYER_OR_PLANESWALKER");
  return result;
}

/* ManalinkEh 2022: 0x20c2e10; original cards/aether_revolt.c:2388. */
int card_implement_of_examination(int player, int card, event_t event)
{
  int result;
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = graveyard_from_play(player, card, event);
      if (result == 0)
      {
        return 0;
      }
      draw_cards(player, 1);
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      draw_a_card(player);
    }
  }
  result = generic_activated_ability(player, card, event, 8, 0, 0, 1, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c3050; original cards/aether_revolt.c:2432. */
int card_implement_of_improvement(int player, int card, event_t event)
{
  int result;
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = graveyard_from_play(player, card, event);
      if (result == 0)
      {
        return 0;
      }
      draw_cards(player, 1);
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      gain_life(player, 2);
    }
  }
  result = generic_activated_ability(player, card, event, 8, 0, 0, 0, 0, 0, 1, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c3120; original cards/aether_revolt.c:2450. */
int card_implement_of_malice(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    get_card_instance(player, card);
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = aer_implement(player, card, event);
      return result;
    }
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        discard(instance->targets[0].player, 0, player);
      }
    }
  }
  result = generic_activated_ability(player, card, event, 0x1c, 0, 1, 0, 0, 0, 0, 0, &td,
                                     "TARGET_PLAYER");
  return result;
}

/* ManalinkEh 2022: 0x20c3a50; original cards/aether_revolt.c:2638. */
int card_inspiring_roar(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    manipulate_type(player, card, player, TYPE_CREATURE, ACT_ADD_COUNTERS_BASE | ACT_KILL_BURY);
    kill_card(player, card, KILL_SACRIFICE);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c4160; original cards/aether_revolt.c:2762. */
int card_irontread_crusher(int player, int card, event_t event)
{
  int result;
  result = vehicle(player, card, event, 3);
  return result;
}

/* ManalinkEh 2022: 0x20c44c0; original cards/aether_revolt.c:2839. */
int card_leave_in_the_dust(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x47);
        td.illegal_type = 1;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            bounce_permanent(instance->targets[0].player, instance->targets[0].card);
            draw_cards(player, 1);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_020c452d;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.illegal_type = 1;
  get_card_instance(player, card);
LAB_020c452d:
  result =
      generic_spell(player, card, event, 0x81, &td, "Select target nonland permanent.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c5e40; original cards/aether_revolt.c:3386. */
int card_narnam_renegade(int player, int card, event_t event)
{
  int result;
  deathtouch_2022(player, card, event);
  result = get_trap_condition(player, 0x60c);
  if (result == 0)
  {
    return 0;
  }
  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, 1);
  return 0;
}

/* ManalinkEh 2022: 0x20c5eb0; original cards/aether_revolt.c:3403. */
int card_natural_obsolescence(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x40);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            put_on_bottom_of_deck(instance->targets[0].player, instance->targets[0].card);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_020c5f16;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x40);
  get_card_instance(player, card);
LAB_020c5f16:
  result = generic_spell(player, card, event, 0x81, &td, "Select target artifact.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c6160; original cards/aether_revolt.c:3468. */
int card_pacification_array(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 0x42);
    if (event != EVENT_ACTIVATE)
      goto LAB_020c61be;
    generic_activated_ability(player, card, EVENT_ACTIVATE, 7, 2, 0, 0, 0, 0, 0, 0, &td,
                              "Choose target artifact or creature");
    if (spell_fizzled != 1)
    {
      ai_mod_for_tap_effects(player, card, 0);
    }
  }
  else if ((event == EVENT_RESOLVE_ACTIVATION) || (event == EVENT_CLEANUP))
  {
    default_target_definition(player, card, &td, 0x42);
  LAB_020c61be:
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        tap_card(instance->targets[0].player, instance->targets[0].card);
      }
    }
    result = generic_activated_ability(player, card, event, 5, 2, 0, 0, 0, 0, 0, 0, &td,
                                       "Choose target artifact or creature");
    return result;
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c6a30; original cards/aether_revolt.c:3619. */
int card_pendulum_of_patterns(int player, int card, event_t event)
{
  int result;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    gain_life(player, 3);
  }
  else if (event == EVENT_CAN_ACTIVATE)
  {
    result = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 5, 0, 0, 0, 0, 0, 0,
                                       NULL, NULL);
    if (result != 0)
    {
      return 1;
    }
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_a_card(player);
  }
  result = generic_activated_ability(player, card, event, 9, 5, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c6b80; original cards/aether_revolt.c:3646. */
int card_perilous_predicament(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_default_test_definition(&this_test, 0x42, "Select an artifact creature to sacrifice.");
    this_test.type_flag = 0xb;
    new_sacrifice(player, card, 1 - player, SAC_CAUSED | SAC_NO_CANCEL, &this_test);
    new_default_test_definition(&this_test, 2, "Select a nonartifact creature.");
    this_test.type_flag = 9;
    new_sacrifice(player, card, 1 - player, SAC_CAUSED | SAC_NO_CANCEL, &this_test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20c71e0; original cards/aether_revolt.c:3875. */
int card_planar_bridge(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  check_legend_rule(player, card, event);
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    new_default_test_definition(&this_test, 0x47, "Select a permanent card.");
    new_global_tutor(player, player, 1, 4, 0, 2, &this_test);
  }
  result = generic_activated_ability(player, card, event, 1, 8, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c7d40; original cards/aether_revolt.c:4130. */
int card_renegade_map(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  comes_into_play_tapped(player, card, event);
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    new_default_test_definition(&this_test, 1, "Select a basic land card.");
    this_test.subtype = 0x2000;
    new_global_tutor(player, player, 1, 1, 0, 5, &this_test);
  }
  result = generic_activated_ability(player, card, event, 9, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c7e40; original cards/aether_revolt.c:4149. */
int card_renegade_rallier(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = get_trap_condition(player, 0x60c);
    if (result != 0)
    {
      new_default_test_definition(&this_test, 0x47, "Select a permanent with CMC 2 or less.");
      this_test.cmc = 3;
      this_test.cmc_flag = 3;
      result = new_special_count_grave(player, &this_test);
      if (0 < result)
      {
        result = graveyard_has_shroud(player);
        if (result == 0)
        {
          new_global_tutor(player, player, 2, 4, 0, 1, &this_test);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c8020; original cards/aether_revolt.c:4200. */
int card_renegades_getaway(int player, int card, event_t event)
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
      default_target_definition(player, card, &td, 0x47);
      td.preferred_controller = player;
      get_card_instance(player, card);
      goto LAB_020c80e7;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0x47);
      td.preferred_controller = player;
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = valid_target(&td);
        if (result != 0)
        {
          pump_ability_until_eot(player, card, instance->targets[0].player,
                                 instance->targets[0].card, 0, 0, 0, 0x800);
          make_servo(player, card, 1);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_020c80e7;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.preferred_controller = player;
  get_card_instance(player, card);
LAB_020c80e7:
  result = generic_spell(player, card, event, 1, &td, "TARGET_PERMANENT", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c81d0; original cards/aether_revolt.c:4232. */
int card_reservoir_walker(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  add_energy(player, 3);
  gain_life(player, 3);
  return 0;
}

/* ManalinkEh 2022: 0x20c8240; original cards/aether_revolt.c:4249. */
int card_resourceful_return(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  test_definition_t this_test;
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        new_default_test_definition(&this_test, 2, "Select a creature card.");
        if ((card != affected_card) || (player != affected_card_controller))
          goto LAB_020c829b;
        generic_spell(player, card, EVENT_CAST_SPELL, 8, NULL, NULL, 1, &this_test);
        if (spell_fizzled != 1)
        {
          instance = get_card_instance(player, card);
          ai_modifier =
              ai_modifier +
              cards_ptr[graveyard_ptr_mutable[player][instance->targets[0].player]]->ai_base_value;
        }
      }
      return 0;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      new_default_test_definition(&this_test, 2, "Select a creature card.");
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = validate_target_from_grave_source(player, card, player, 0);
        if (result != -1)
        {
          from_grave_to_hand(player, result, 1);
        }
        result = check_battlefield_for_subtype(player, 0x40, -1);
        if (result != 0)
        {
          draw_a_card(player);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_020c829b;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  new_default_test_definition(&this_test, 2, "Select a creature card.");
LAB_020c829b:
  result = generic_spell(player, card, event, 8, NULL, NULL, 1, &this_test);
  return result;
}

/* ManalinkEh 2022: 0x20c8890; original cards/aether_revolt.c:4364. */
int card_ridgescale_tusker(int player, int card, event_t event)
{
  test_definition_t test;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  new_default_test_definition(&test, 2, "");
  test.not_me = 1;
  new_manipulate_all(player, card, player, &test, ACT_ADD_COUNTERS_BASE | ACT_KILL_BURY);
  return 0;
}

/* ManalinkEh 2022: 0x20c8d30; original cards/aether_revolt.c:4485. */
int card_rogue_refiner(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  draw_a_card(player);
  add_energy(player, 2);
  return 0;
}

/* ManalinkEh 2022: 0x20c96e0; original cards/aether_revolt.c:4774. */
int card_servo_schematic(int player, int card, event_t event)
{
  int result;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    make_servo(player, card, 1);
  }
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  make_servo(player, card, 1);
  return 0;
}

/* ManalinkEh 2022: 0x20c9d00; original cards/aether_revolt.c:4872. */
int card_silkweaver_elite(int player, int card, event_t event)
{
  int result;
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = get_trap_condition(player, 0x60c);
    if (result != 0)
    {
      etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    }
  }
  if (event != EVENT_ETB)
  {
    return 0;
  }
  draw_a_card(player);
  return 0;
}

/* ManalinkEh 2022: 0x20ca560; original cards/aether_revolt.c:5160. */
int card_take_into_custody(int player, int card, event_t event)
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
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            effect_frost_titan(player, card, instance->targets[0].player,
                               instance->targets[0].card);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_020ca5c6;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_020ca5c6:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20cabe0; original cards/aether_revolt.c:5300. */
int card_tezzerets_betrayal(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  test_definition_t test;

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
        instance = get_card_instance(player, card);
        default_target_definition(player, card, &td, 2);
        td.illegal_abilities = 0;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            new_default_test_definition(&test, 0xff,
                                        "Select a card named Tezzeret, Master of Metal.");
            test.id = 0x41ad;
            result = new_special_count_grave(player, &test);
            if ((result == 0) ||
                (result = new_global_tutor(player, player, 2, 1, 0, 5, &test), result < 0))
            {
              new_global_tutor(player, player, 1, 1, 0, 5, &test);
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_020cac5d;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  get_card_instance(player, card);
  default_target_definition(player, card, &td, 2);
  td.illegal_abilities = 0;
LAB_020cac5d:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20cafa0; original cards/aether_revolt.c:5370. */
int card_tezzerets_touch(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  int player_00;
  int owner;
  target_definition_t td;
  result = attached_creature_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    instance = get_card_instance(player, card);
    result = instance->damage_target_card;
    player_00 = get_owner((int)instance->damage_target_player, result);
    result = find_in_owners_graveyard(player_00, result, &owner, &td.who_chooses);
    if (result != 0)
    {
      result = graveyard_ptr_mutable[owner][td.who_chooses];
      remove_card_from_grave(owner, td.who_chooses);
      add_card_to_hand(owner, result);
    }
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_020caff8;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_020caff8;
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         (((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
           (event != EVENT_CHANGE_TYPE)))) &&
        (((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)) &&
          (event != EVENT_STATIC_EFFECTS))))
    {
      return 0;
    }
  }
LAB_020caff8:
  default_target_definition(player, card, &td, 0x40);
  td.preferred_controller = player;
  result = generic_animating_aura(player, card, event, &td, "TARGET_ARTIFACT", 5, 5, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x20cb4f0; original cards/aether_revolt.c:5475. */
int card_thopter_arrest(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  return_from_oblivion(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0x42);
    td.allowed_controller = 1 - player;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(
          &td, "Select target target creature or artifact an opponent controls.", 0, 0x80);
      if (result != 0)
      {
        obliviation(player, card, instance->targets[0].player, instance->targets[0].card);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20cb5f0; original cards/aether_revolt.c:5504. */
int card_treasure_keeper(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  default_test_definition(&this_test, 1);
  this_test.type_flag = 1;
  this_test.cmc = 4;
  this_test.cmc_flag = 3;
  reveal_card_until_you_reveal_test(player, player, &this_test, 0x2004);
  return 0;
}

/* ManalinkEh 2022: 0x20cb680; original cards/aether_revolt.c:5525. */
int card_trophy_mage(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  result = duh_mode(player);
  etb_ability(player, card, event, (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  new_default_test_definition(&this_test, 0x40, "Select an artifact card with CMC 3");
  this_test.cmc = 3;
  this_test.cmc_flag = 0;
  new_global_tutor(player, player, 1, 1, 0, 1, &this_test);
  return 0;
}

/* ManalinkEh 2022: 0x20cbd20; original cards/aether_revolt.c:5622. */
int card_universal_solvent(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x47);
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x47);
    instance = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
  }
  result = generic_activated_ability(player, card, event, 0xd, 7, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_PERMANENT");
  return result;
}

/* ManalinkEh 2022: 0x20cbef0; original cards/aether_revolt.c:5664. */
int card_vengeful_rebel(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 2);
    td.allowed_controller = 1 - player;
    td.allow_cancel = 0;
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_CREATURE_OPPONENT_CONTROLS");
      if (result != 0)
      {
        result = get_trap_condition(player, 0x60c);
        if (result != 0)
        {
          instance = get_card_instance(player, card);
          pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, -3,
                         -3);
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20cc690; original cards/aether_revolt.c:5783. */
int card_watchful_automaton(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    scry(player, 1);
  }
  result = generic_activated_ability(player, card, event, 0, 2, 0, 1, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20ccaa0; original cards/aether_revolt.c:5861. */
int card_wrangle(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    td.power_requirement = 0x2004;
    get_card_instance(player, card);
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = 1 - player;
      td.power_requirement = 0x2004;
      get_card_instance(player, card);
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_020ccb81;
      generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                    "Select target creature with power 4 or less", 1, NULL);
      if (spell_fizzled != 1)
      {
        ai_modifier_for_act_of_treason(player, card);
      }
    }
    return 0;
  }
  if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    td.power_requirement = 0x2004;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        effect_act_of_treason(player, card, instance->targets[0].player, instance->targets[0].card);
      }
      kill_card(player, card, KILL_DESTROY);
    }
  }
  else
  {
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    td.power_requirement = 0x2004;
    get_card_instance(player, card);
  }
LAB_020ccb81:
  result = generic_spell(player, card, event, 1, &td, NULL, 1, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20bfba0; original cards/aether_revolt.c:1338. */
int card_daring_demolition(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x47);
        td.special = 0x8000;
        td.extra = (int32_t)is_creature_or_vehicle;
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
        goto LAB_020bfc18;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x47);
  td.special = 0x8000;
  td.extra = (int32_t)is_creature_or_vehicle;
  get_card_instance(player, card);
LAB_020bfc18:
  result =
      generic_spell(player, card, event, 0x81, &td, "Select target creature or vehicle.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c1c90; original cards/aether_revolt.c:2060. */
int card_herald_of_anguish(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td_creature;

  if (current_turn == player)
  {
    result = eot_trigger(player, card, event);
    if (result != 0)
    {
      discard(1 - player, 0, player);
    }
  }
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td_creature, 2);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = improvise(player, card, event);
      return result;
    }
    default_target_definition(player, card, &td_creature, 2);
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td_creature);
      if (result != 0)
      {
        instance = get_card_instance(player, card);
        result = instance->targets[0].card;
        instance = get_card_instance(player, card);
        pump_until_eot(player, card, instance->targets[0].player, result, -2, -2);
      }
    }
  }
  result = generic_activated_ability_with_sacrifice_as_cost(
      player, card, event, GAA_CAN_TARGET, 1, 1, 0, 0, 0, 0, 0, &td_creature, "TARGET_CREATURE",
      TYPE_ARTIFACT, NULL, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x20c1fc0; original cards/aether_revolt.c:2126. */
int card_hidden_stockpile(int player, int card, event_t event)
{
  int result;

  result = eot_revolt(player, card, event);
  if (result != 0)
  {
    make_servo(player, card, 1);
  }
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      if (event != EVENT_SHOULD_AI_PLAY)
      {
        return (unsigned int)(event == EVENT_CAN_CAST);
      }
      ai_modifier = ai_modifier + (-(unsigned int)(player == 0) & 0xfffffff0) + 8;
      return 0;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      scry(player, 1);
    }
  }
  result = generic_activated_ability(player, card, event, 0x200, 1, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c24e0; original cards/aether_revolt.c:2240. */
int card_hungry_flames(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  bool bVar3;
  target_definition_t *ptStack_bc;
  target_definition_t td;
  target_definition_t td1;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    default_target_definition(player, card, &td1, 0x100);
    td1.zone = 0x1200;
    get_card_instance(player, card);
    result = can_target(&td1);
    if (result == 0)
    {
      return 0;
    }
    result = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
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
    default_target_definition(player, card, &td, 2);
    default_target_definition(player, card, &td1, 0x100);
    td1.zone = 0x1200;
    instance = get_card_instance(player, card);
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    default_target_definition(player, card, &td1, 0x100);
    td1.zone = 0x1200;
    instance = get_card_instance(player, card);
    if (event != EVENT_CHANGE_TARGET)
      goto LAB_020c2672;
  }
  else
  {
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    default_target_definition(player, card, &td1, 0x100);
    td1.zone = 0x1200;
    instance = get_card_instance(player, card);
  }
  ptStack_bc = &td1;
  generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  instance->targets[1].player = instance->targets[0].player;
  bVar3 = spell_fizzled == 1;
  instance->targets[1].card = instance->targets[0].card;
  if (bVar3)
  {
    return 0;
  }
  pick_target(ptStack_bc, "TARGET_PLAYER_OR_PLANESWALKER");
LAB_020c2672:
  ptStack_bc = &td1;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = validate_target(player, card, &td, 1);
    if (result != 0)
    {
      damage_creature(instance->targets[1].player, instance->targets[1].card, 3, player, card);
    }
    result = validate_target(player, card, ptStack_bc, 0);
    if (result != 0)
    {
      damage_target0(player, card, 2);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c4190; original cards/aether_revolt.c:2771. */
int card_kari_zevs_expertise(int player, int card, event_t event)
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
      default_target_definition(player, card, &td, 0x47);
      td.special = 0x8000;
      td.extra = (int32_t)is_creature_or_vehicle;
      get_card_instance(player, card);
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_020c42ed;
    }
    else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0x47);
      td.special = 0x8000;
      td.extra = (int32_t)is_creature_or_vehicle;
      instance = get_card_instance(player, card);
      if (event != EVENT_CHANGE_TARGET)
      {
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if ((result != 0) && (result = player,
                                effect_act_of_treason(player, card, instance->targets[0].player,
                                                      instance->targets[0].card),
                                0 < hand_count[player]))
          {
            av_free_spell(player, 2);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_020c42ed;
      }
    }
    else
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      default_target_definition(player, card, &td, 0x47);
      td.special = 0x8000;
      td.extra = (int32_t)is_creature_or_vehicle;
      get_card_instance(player, card);
    }
    generic_spell(player, card, event, 0x81, &td, "Select target creature or Vehicle.", 1, NULL);
    if (spell_fizzled != 1)
    {
      ai_modifier_for_act_of_treason(player, card);
    }
    return 0;
  }
  default_target_definition(player, card, &td, 0x47);
  td.special = 0x8000;
  td.extra = (int32_t)is_creature_or_vehicle;
  get_card_instance(player, card);
LAB_020c42ed:
  result =
      generic_spell(player, card, event, 0x81, &td, "Select target creature or Vehicle.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c5bb0; original cards/aether_revolt.c:3337. */
int card_monstrous_onslaught(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  target_t *ptVar3;
  target_t *ptVar4;
  int ret_location;
  int iVar5;
  target_definition_t td;
  char msg[100];

  default_target_definition(player, card, &td, 2);
  if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
  {
    td.preferred_controller = 1 - player;
    td.illegal_abilities = get_protections_from(player, card);
    td.special = 0x8000;
    td.extra = (int32_t)not_indestructible;
  }
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
  {
    iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    return iVar2;
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
  LAB_020c5c58:
    iVar2 = get_highest_power(player);
    ai_modifier = ai_modifier + iVar2;
    instance->info_slot = iVar2;
    instance->number_of_targets = '\0';
    ret_location = 0;
    if (iVar2 < 1)
    {
      return 0;
    }
    do
    {
      iVar5 = ret_location + 1;
      scnprintf(msg, 100, "Select target creature (%d of %d) to deal 1 damage.", iVar5, iVar2);
      iVar2 = new_pick_target(&td, msg, ret_location, 0x80);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        iVar5 = ret_location;
        break;
      }
      add_state(instance->targets[ret_location].player, instance->targets[ret_location].card,
                0x200000);
      iVar2 = instance->info_slot;
      ret_location = iVar5;
    } while ((int)(unsigned int)instance->number_of_targets < iVar2);
    if (0 < iVar5)
    {
      ptVar3 = instance->targets;
      do
      {
        ptVar4 = ptVar3 + 1;
        remove_state(ptVar3->player, ptVar3->card, 0x200000);
        ptVar3 = ptVar4;
      } while (ptVar4 != instance->targets + iVar5);
      if (event != EVENT_RESOLVE_SPELL)
      {
        return 0;
      }
      goto LAB_020c5d98;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
    goto LAB_020c5c58;
  if (event != EVENT_RESOLVE_SPELL)
  {
    return 0;
  }
LAB_020c5d98:
  divide_damage(player, card, &td);
  kill_card(player, card, KILL_DESTROY);
  return 0;
}

/* ManalinkEh 2022: 0x20c8930; original cards/aether_revolt.c:4385. */
int card_rishkars_expertise(int player, int card, event_t event)
{
  int amount;
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
      amount = get_highest_power(player);
      result = player;
      draw_cards(player, amount);
      if (0 < hand_count[player])
      {
        av_free_spell(player, 5);
      }
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

/* ManalinkEh 2022: 0x20ca3b0; original cards/aether_revolt.c:5098. */
int card_srams_expertise(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    result = player;
    make_servo(player, card, 3);
    if (0 < hand_count[player])
    {
      av_free_spell(player, 3);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20cc910; original cards/aether_revolt.c:5833. */
int card_whir_of_invention(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  test_definition_t td;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
      LAB_020cca58:
        result = improvise(player, card, event);
        return result;
      }
    }
    else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        if (event == EVENT_RESOLVE_SPELL)
        {
          default_test_definition(&td, 0x40);
          instance = get_card_instance(player, card);
          scnprintf(td.message, 100, "Select an artifact card with CMC %d or less.",
                    instance->info_slot);
          instance = get_card_instance(player, card);
          td.cmc = instance->info_slot + 1;
          td.cmc_flag = 3;
          new_global_tutor(player, player, 1, 4, 0, 1, &td);
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_020cca08;
      }
    }
    else if (event != EVENT_CHANGE_TARGET)
      goto LAB_020cca58;
  }
  improvise(player, card, event);
LAB_020cca08:
  result = generic_spell(player, card, event, 4, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20ccd20; original cards/aether_revolt.c:5900. */
int card_yahennis_expertise(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    result = player;
    pump_subtype_until_eot(player, card, 2, -1, -3, -3, 0, 0);
    if (0 < hand_count[player])
    {
      av_free_spell(player, 3);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20bcc00; original cards/aether_revolt.c:392. */
int card_aether_inspector(int player, int card, event_t event)
{
  int result;

  vigilance(player, card, event);
  result = card_aether_chaser(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20c0840; original cards/aether_revolt.c:1600. */
int card_enraged_giant(int player, int card, event_t event)
{
  int result;

  haste(player, card, event);
  result = improvise(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20c1000; original cards/aether_revolt.c:1774. */
int card_foundry_assembler(int player, int card, event_t event)
{
  int result;

  result = improvise(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20c11a0; original cards/aether_revolt.c:1825. */
int card_freejam_regent(int player, int card, event_t event)
{
  int result;

  if (((event != EVENT_ACTIVATE) && (1 < event - EVENT_RESOLVE_ACTIVATION)) &&
      (event != EVENT_CLEANUP))
  {
    result = improvise(player, card, event);
    return result;
  }
  result = generic_shade_merge_pt(player, card, event, 0, 1, 0, 0, 0, 1, 0, 2, 0);
  return result;
}

/* ManalinkEh 2022: 0x20c5720; original cards/aether_revolt.c:3167. */
int card_metallic_rebuke(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
      LAB_020c57a8:
        result = improvise(player, card, event);
        return result;
      }
    }
    else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      if (event != EVENT_CHANGE_TARGET)
      {
        if (event == EVENT_RESOLVE_SPELL)
        {
          counterspell_resolve_unless_pay_x(player, card, NULL, 0, 3);
          kill_card(player, card, KILL_DESTROY);
          return 0;
        }
        goto LAB_020c575e;
      }
    }
    else if (event != EVENT_CHANGE_TARGET)
      goto LAB_020c57a8;
  }
  improvise(player, card, event);
LAB_020c575e:
  result = generic_spell(player, card, event, 2, NULL, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c87d0; original cards/aether_revolt.c:4341. */
int card_reverse_engineer(int player, int card, event_t event)
{
  int result;

  if (event != EVENT_CAN_CAST)
  {
    if (event != EVENT_CAST_SPELL)
    {
      if (event == EVENT_RESOLVE_SPELL)
      {
        draw_cards(player, 3);
        kill_card(player, card, KILL_DESTROY);
      }
      else if ((event != EVENT_CAN_CHANGE_TARGET) && (event != EVENT_CHANGE_TARGET))
        goto LAB_020c8838;
      goto LAB_020c8858;
    }
    if ((card != affected_card) || (player != affected_card_controller))
    {
    LAB_020c8838:
      result = improvise(player, card, event);
      return result;
    }
  }
  improvise(player, card, event);
LAB_020c8858:
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20ca520; original cards/aether_revolt.c:5148. */
int card_sweatworks_brawler(int player, int card, event_t event)
{
  int result;

  menace(player, card, event);
  result = improvise(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int card_outland_boar(int player, int card, event_t event);

/* ManalinkEh 2022: 0x20bf850; original cards/aether_revolt.c:1267. */
int card_crackdown_construct(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (((event == EVENT_PLAY_ABILITY) || (event == EVENT_TAPPED_TO_PLAY_ABILITY)) &&
      (affected_card_controller == player))
  {
    result = is_what(player, affected_card, 0x42);
    if (result != 0)
    {
      instance = get_card_instance(affected_card_controller, affected_card);
      if (((cards_data[instance->internal_card_id].extra_ability & 0x1000) == 0) ||
          (tapped_for_mana_color < 0))
      {
        instance = in_play(player, card);
        if (instance != NULL)
        {
          result = is_humiliated_for_ability(player, card, AL_TRIG);
          if (result == 0)
          {
            pump_until_eot_merge_previous(player, card, player, card, 1, 1);
          }
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c0210; original cards/aether_revolt.c:1481. */
int card_destructive_tampering(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  test_definition_t *ptStack_140;
  target_definition_t td;
  test_definition_t this_test;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x40);
    td.preferred_controller = 1 - player;
    default_test_definition(&this_test, 2);
    this_test.keyword = 0x20;
    this_test.keyword_flag = 1;
    get_card_instance(player, card);
    iVar3 = basic_spell(player, card, EVENT_CAN_CAST);
    return iVar3;
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
    default_target_definition(player, card, &td, 0x40);
    td.preferred_controller = 1 - player;
    default_test_definition(&this_test, 2);
    this_test.keyword = 0x20;
    this_test.keyword_flag = 1;
    instance = get_card_instance(player, card);
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x40);
    td.preferred_controller = 1 - player;
    default_test_definition(&this_test, 2);
    this_test.keyword = 0x20;
    this_test.keyword_flag = 1;
    instance = get_card_instance(player, card);
    if (event != EVENT_CHANGE_TARGET)
      goto LAB_020c02b8;
  }
  else
  {
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x40);
    td.preferred_controller = 1 - player;
    default_test_definition(&this_test, 2);
    this_test.keyword = 0x20;
    this_test.keyword_flag = 1;
    instance = get_card_instance(player, card);
  }
  instance->number_of_targets = '\0';
  iVar3 = check_special_flags2(player, card, -0x80000000);
  if (iVar3 != 0)
  {
    return 0;
  }

  iVar3 = (unsigned int)(current_phase == 0x14) * 5 + 1;
  iVar4 = iVar3;
  iVar2 = can_target(&td);
  iVar3 =
      dialog_impl(player, card, event, "\001\002", "Destroy target artifact", iVar2, 5,
                  "Creatures without flying can't block this turn", 1, iVar3, 0, 0, 0, 0, iVar4);
  if (iVar3 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (iVar3 == 1)
  {
    iVar3 = generic_spell(player, card, event, 1, &td, "TARGET_ARTIFACT", 1, NULL);
    return iVar3;
  }
  if (iVar3 == 2)
  {
    iVar3 = basic_spell(player, card, event);
    return iVar3;
  }
LAB_020c02b8:
  ptStack_140 = &this_test;
  if (event == EVENT_RESOLVE_SPELL)
  {

    if (instance->info_slot == 1)
    {
      iVar3 = valid_target(&td);
      if (iVar3 != 0)
      {
        kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
      }
    }
    else if (instance->info_slot == 2)
    {
      creatures_cannot_block(player, card, ptStack_140, 1);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c0ef0; original cards/aether_revolt.c:1741. */
int card_fen_hauler(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = in_play(player, card);
  if (instance != NULL)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if ((((event == EVENT_BLOCK_LEGALITY) && (result == 0)) &&
         (attacking_card_controller == player)) &&
        (attacking_card == card))
    {
      result = is_what(affected_card_controller, affected_card, 2);
      if (result != 0)
      {
        result = is_what(affected_card_controller, affected_card, 0x40);
        if (result != 0)
        {
          event_result = 1;
        }
      }
    }
  }
  result = improvise(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20c2220; original cards/aether_revolt.c:2175. */
int card_hinterland_drake(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_BLOCK_LEGALITY) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if ((result == 0) && (attacking_card_controller != -1))
    {
      result = is_what(attacking_card_controller, attacking_card, 2);
      if (result != 0)
      {
        result = is_what(attacking_card_controller, attacking_card, 0x40);
        if (result != 0)
        {
          event_result = 1;
        }
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c3bf0; original cards/aether_revolt.c:2669. */
int card_invigorated_rampage(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  int iVar3;
  card_instance_t *instance;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    get_card_instance(player, card);
    iVar3 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    return iVar3;
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
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
  LAB_020c3cfb:
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
    if (((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET)) &&
        (event != EVENT_CHANGE_TARGET))
    {
      if (event != EVENT_CHECK_PUMP)
      {
        return 0;
      }
    LAB_020c3e3d:
      iVar3 = vanilla_instant_pump(player, card, EVENT_CHECK_PUMP, 2, player, 4, 0, 0x80, 0);
      if (iVar3 == 0)
      {
        iVar3 = vanilla_instant_pump(player, card, EVENT_CHECK_PUMP, 2, player, 2, 2, 0x80, 0);
        return iVar3;
      }
      return 1;
    }
    if (event == EVENT_CHECK_PUMP)
      goto LAB_020c3e3d;
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_CAST_SPELL)
      goto LAB_020c3cfb;
    if (event != EVENT_CHANGE_TARGET)
      goto LAB_020c3c79;
  }

  instance->number_of_targets = '\0';
  iVar3 = can_target(&td);
  iVar2 = target_available(-1, card, &td);
  iVar1 = check_special_flags2(player, card, -0x80000000);
  if (iVar1 != 0)
  {
    return 0;
  }

  iVar3 = dialog_impl(
      player, card, event, "\001\002", "\001\016", "One creature gets +4/+0 and Trample", iVar3, 5,
      "Two creatures get +2/+0 and Trample", (unsigned int)(1 < iVar2), 5, 0, 0, 0, 0);
  if (iVar3 == 0)
  {
    spell_fizzled = 1;
    return 0;
  }
  if (iVar3 == 1)
  {
    iVar3 = 1;
  }
  else
  {
    if (iVar3 != 2)
    {
    LAB_020c3c79:
      if (event == EVENT_RESOLVE_SPELL)
      {

        if (instance->info_slot == 1)
        {
          iVar3 = valid_target(&td);
          if (iVar3 != 0)
          {
            pump_ability_until_eot(player, card, instance->targets[0].player,
                                   instance->targets[0].card, 4, 0, 0x80, 0);
          }
        }
        else if (instance->info_slot == 2)
        {

          iVar3 = 0;
          do
          {
            iVar2 = validate_target(player, card, &td, iVar3);
            if (iVar2 != 0)
            {
              pump_ability_until_eot(player, card, instance->targets[iVar3].player,
                                     instance->targets[iVar3].card, 2, 0, 0x80, 0);
            }
            iVar3 = iVar3 + 1;
          } while (iVar3 != 2);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      return 0;
    }
    iVar3 = 2;
  }
  iVar3 = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", iVar3, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x20c4f20; original cards/aether_revolt.c:3037. */
int card_mechanized_production(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    token_generation_t token;
    uint8_t bytes[84];
  } recovered_stack_td;

  pcVar1 = get_card_instance(player, card);
  pcVar2 = in_play(player, card);
  if (((pcVar2 != NULL) &&
       (pcVar2 = get_card_instance(player, card), -1 < pcVar2->damage_target_player)) &&
      (upkeep_trigger_ability(player, card, event, player), event == EVENT_UPKEEP_TRIGGER_ABILITY))
  {
    copy_token_definition(player, card, &recovered_stack_td.token,
                          (int)pcVar1->damage_target_player, pcVar1->damage_target_card);
    generate_token(&recovered_stack_td.token);
    iVar3 = current_turn;

    iVar6 = active_cards_count[current_turn] + -1;
    if (-1 < iVar6)
    {
      do
      {
        if (player == iVar3)
        {
          iVar4 = get_card_name(player, iVar6);
          iVar4 = count_permanents_by_name(player, TYPE_ARTIFACT, iVar4);
          if (7 < iVar4)
          {
            lose_the_game(1 - player);
          }
        }
        iVar6 = iVar6 + -1;
      } while (iVar6 != -1);
    }
    iVar3 = current_turn;
    iVar6 = 1 - current_turn;
    iVar4 = active_cards_count[iVar6] + -1;
    if (-1 < iVar4)
    {
      do
      {
        if (player == iVar6)
        {
          iVar5 = get_card_name(player, iVar4);
          iVar5 = count_permanents_by_name(player, TYPE_ARTIFACT, iVar5);
          if (7 < iVar5)
          {
            lose_the_game(iVar3);
          }
        }
        iVar4 = iVar4 + -1;
      } while (iVar4 != -1);
    }
    goto LAB_020c4fe4;
  }
  if (event == EVENT_CAN_CAST)
  {
  LAB_020c4f86:
    default_target_definition(player, card, &recovered_stack_td.target, 0x40);
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_020c4fe4;
      default_target_definition(player, card, &recovered_stack_td.target, 0x40);
    }
    else
    {
      if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
          (event == EVENT_CHANGE_TARGET))
        goto LAB_020c4f86;
    LAB_020c4fe4:
      if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
           (2 < event - EVENT_POWER)) &&
          (event != EVENT_RESOLVE_MOVING_AURA))
      {
        if ((event == EVENT_CHANGE_TYPE) || (event == EVENT_SET_COLOR))
          goto LAB_020c4f86;
        if ((event != EVENT_GET_SELECTED_CARD) && (event != EVENT_STATIC_EFFECTS))
        {
          return 0;
        }
      }
      default_target_definition(player, card, &recovered_stack_td.target, 0x40);
      if (event != EVENT_CAST_SPELL)
        goto LAB_020c4fad;
    }
    recovered_stack_td.target.allowed_controller = player;
    if ((card == affected_card) && (player == affected_card_controller))
    {
      recovered_stack_td.target.preferred_controller = recovered_stack_td.target.allowed_controller;
      generic_spell(player, card, EVENT_CAST_SPELL, 1, &recovered_stack_td.target,
                    "TARGET_ARTIFACT", 1, NULL);
      if (spell_fizzled == 1)
      {
        return 0;
      }
      iVar3 = get_cmc(pcVar1->targets[0].player, pcVar1->targets[0].card);
      ai_modifier = ai_modifier + iVar3 * 8;
      return 0;
    }
  }
LAB_020c4fad:
  recovered_stack_td.target.allowed_controller = player;
  recovered_stack_td.target.preferred_controller = recovered_stack_td.target.allowed_controller;
  iVar3 = targeted_aura(player, card, event, &recovered_stack_td.target, "TARGET_ARTIFACT");
  return iVar3;
}

/* ManalinkEh 2022: 0x20c5ab0; original cards/aether_revolt.c:3307. */
int card_mobile_garrison(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    default_target_definition(player, card, &td, 0x42);
    td.preferred_controller = player;
    td.allow_cancel = 0;
    td.special = 0x4000;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Select another artifact or creature you control.", 0, 0x80);
      if (result != 0)
      {
        untap_card(instance->targets[0].player, instance->targets[0].card);
      }
    }
    instance->number_of_targets = '\0';
  }
  result = vehicle(player, card, event, 2);
  return result;
}

/* ManalinkEh 2022: 0x20c5ff0; original cards/aether_revolt.c:3428. */
int card_oath_of_ajani(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_test_definition(&this_test, 2);
    new_manipulate_all(player, card, player, &this_test, ACT_ADD_COUNTERS_BASE | ACT_KILL_BURY);
  }
  else if ((event == EVENT_MODIFY_COST_GLOBAL) && (affected_card_controller == player))
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      result = is_what(player, affected_card, 0x100);
      if (result != 0)
      {
        COST_COLORLESS = COST_COLORLESS + -1;
      }
    }
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20c60f0; original cards/aether_revolt.c:3451. */
int card_outland_boar(int player, int card, event_t event)
{
  int result;

  result = is_humiliated_for_ability(player, card, AL_STATIC);
  if ((((event == EVENT_BLOCK_LEGALITY) && (result == 0)) &&
       (attacking_card_controller == player)) &&
      (attacking_card == card))
  {
    result = get_power(affected_card_controller, affected_card);
    if (result < 3)
    {
      event_result = 1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c63e0; original cards/aether_revolt.c:3518. */
int card_peacewalker_colossus(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  unsigned int uVar3;
  int iVar4;
  target_definition_t td;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x40);
    td.special = 0x4000;
    td.allowed_controller = player;
    td.required_subtype = 0x3005;
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 1, 0, 0, 0, 0, 1, 0,
                                        &td, NULL);
      if (iVar4 == 0)
      {
        iVar4 = vehicle(player, card, EVENT_CAN_ACTIVATE, 4);
        return iVar4;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      iVar4 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 4, 1, 0, 0, 0, 0, 1, 0,
                                        &td, NULL);
      iVar2 = vehicle(player, card, EVENT_CAN_ACTIVATE, 4);
      uVar3 = (current_phase != 0x14) - 1 & 10;
      iVar4 =
          dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016",
                      "Animate another Vehicle", iVar4, uVar3, "Crew", iVar2, uVar3, 0, 0, 0, 0);
      if (iVar4 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar4 != 1)
      {
        if (iVar4 != 2)
        {
          return 0;
        }
        iVar4 = vehicle(player, card, EVENT_ACTIVATE, 4);
        return iVar4;
      }
      iVar4 = generic_activated_ability(player, card, EVENT_ACTIVATE, 6, 1, 0, 0, 0, 0, 1, 0, &td,
                                        "Choose target vehicle you control");
      return iVar4;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x40);
  }
  td.allowed_controller = player;
  td.special = 0x4000;
  td.required_subtype = 0x3005;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    iVar4 = instance->info_slot;
    if (iVar4 == 1)
    {
      iVar4 = valid_target(&td);
      if (iVar4 != 0)
      {
        turn_into_creature(player, card, instance->targets[0].player, instance->targets[0].card, 3,
                           0, 0);
      }
      iVar4 = instance->info_slot;
    }
    if (iVar4 == 2)
    {
      iVar4 = vehicle(player, card, EVENT_RESOLVE_ACTIVATION, 4);
      return iVar4;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c74c0; original cards/aether_revolt.c:4008. */
int card_reckless_racer(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_TAP_CARD) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\014", "Discard, then draw", 1, 1, 0,
                           0, 0, 0);
      if (result != 0)
      {
        discard(player, 0, player);
        draw_a_card(player);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c7f10; original cards/aether_revolt.c:4178. */
int card_renegade_wheelsmith(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (((event == EVENT_TAP_CARD) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    instance = in_play(player, card);
    if (instance != NULL)
    {
      result = is_humiliated_for_ability(player, card, AL_TRIG);
      if (result == 0)
      {

        default_target_definition(player, card, &td, 2);
        td.allow_cancel = 0;
        instance = get_card_instance(player, card);
        result = can_target(&td);
        if (result != 0)
        {
          result = pick_target(&td, "TARGET_CREATURE");
          if (result != 0)
          {
            pump_ability_until_eot(player, card, instance->targets[0].player,
                                   instance->targets[0].card, 0, 0, 0, 0x20);
          }
        }
        instance->number_of_targets = '\0';
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c8470; original cards/aether_revolt.c:4285. */
int card_restoration_specialist(int player, int card, event_t event)
{
  int iVar1;
  int iid;
  card_instance_t *instance;
  int32_t iVar3;
  test_definition_t test;
  test_definition_t test2;

  if (event == EVENT_CAN_ACTIVATE)
  {
    iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 8, 0, 0, 0, 0, 0, 1, 0,
                                      NULL, NULL);
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {
      generic_activated_ability(player, card, EVENT_ACTIVATE, 8, 0, 0, 0, 0, 0, 1, 0, NULL, NULL);
      iVar1 = graveyard_has_shroud(player);
      if (iVar1 == 0)
      {

        new_default_test_definition(&test, 0x40, "Select target artifact card.");
        test.id = (int)cards_data[iid_draw_a_card].id;
        test.id_flag = 1;
        iVar1 = new_select_target_from_grave(player, card, player, SFG_0, 1, &test, 0);
        if (iVar1 == -1)
        {
          new_default_test_definition(&test2, 4, "Select target enchantment card.");
          test2.id = (int)cards_data[iid_draw_a_card].id;
          test2.id_flag = 1;
          new_select_target_from_grave(player, card, player, SFG_0, 1, &test2, 0);
          iVar3 = 0;
        }
        else
        {
          iid = turn_card_in_grave_face_down(player, iVar1);
          new_default_test_definition(&test2, 4, "Select target enchantment card.");
          test2.id = (int)cards_data[iid_draw_a_card].id;
          test2.id_flag = 1;
          new_select_target_from_grave(player, card, player, SFG_0, 1, &test2, 1);
          if (iid == -1)
          {
            iVar3 = 1;
          }
          else
          {
            iVar3 = 1;
            turn_card_in_grave_face_up(player, iVar1, iid);
          }
        }
        instance = get_card_instance(player, card);
        instance->info_slot = iVar3;
      }
    }
    else if (event == EVENT_RESOLVE_ACTIVATION)
    {

      iVar1 = validate_target_from_grave(player, card, player, 0);
      if (iVar1 != -1)
      {
        from_grave_to_hand(player, iVar1, 1);
      }
      iVar1 = validate_target_from_grave(player, card, player, 1);
      if (iVar1 != -1)
      {
        from_grave_to_hand(player, iVar1, 1);
      }
    }
    iVar1 = 0;
  }
  return iVar1;
}

/* ManalinkEh 2022: 0x20c8da0; original cards/aether_revolt.c:4502. */
int card_salvage_scuttler(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Select target artifact you control", 0, 0x80);
      if (result != 0)
      {
        bounce_permanent(instance->targets[0].player, instance->targets[0].card);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c9a40; original cards/aether_revolt.c:4843. */
int card_siege_modification(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;
  int player_00;
  target_definition_t *ptStack_68;
  target_definition_t td;

  pcVar1 = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
  {
  LAB_020c9ab5:
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result != 0)
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x47);
  LAB_020c9af9:
    td.special = 0x8000;
    td.extra = (int32_t)is_creature_or_vehicle;
    player_00 = (int)pcVar1->damage_target_player;
    result = pcVar1->damage_target_card;
    if (((event != EVENT_ABILITIES) || (result != affected_card)) ||
        (player_00 != affected_card_controller))
      goto LAB_020c9c11;
    pcVar3 = in_play(player, card);
    if (pcVar3 != NULL)
    {
      event_result = event_result | 0x100;
    }
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
      {
      LAB_020c9b5c:
        if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) == EVENT_CAN_MOVE_AURA) ||
             (event == EVENT_RESOLVE_MOVING_AURA)) ||
            (event == EVENT_POWER))
          goto LAB_020c9a7c;
        if (((1 < event - EVENT_TOUGHNESS) && (event != EVENT_CHANGE_TYPE)) &&
            ((event != EVENT_SET_COLOR &&
              ((event != EVENT_GET_SELECTED_CARD && (event != EVENT_STATIC_EFFECTS))))))
        {
          return 0;
        }
      }
      goto LAB_020c9ab5;
    }
    if ((event != EVENT_RESOLVE_SPELL) && (event != EVENT_CAN_CHANGE_TARGET))
    {
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_020c9b5c;
      goto LAB_020c9ab5;
    }
  LAB_020c9a7c:
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result != 0)
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x47);
    td.special = 0x8000;
    td.extra = (int32_t)is_creature_or_vehicle;
    if (event != EVENT_POWER)
      goto LAB_020c9af9;
    result = pcVar1->damage_target_card;
    player_00 = (int)pcVar1->damage_target_player;
    if ((result != affected_card) || (player_00 != affected_card_controller))
      goto LAB_020c9c11;
    pcVar3 = in_play(player, card);
    if (pcVar3 != NULL)
    {
      result = pcVar1->damage_target_card;
      event_result = event_result + 3;
      player_00 = (int)pcVar1->damage_target_player;
      goto LAB_020c9c11;
    }
  }
  player_00 = (int)pcVar1->damage_target_player;
  result = pcVar1->damage_target_card;
LAB_020c9c11:
  ptStack_68 = &td;
  result = has_subtype(player_00, result, SUBTYPE_VEHICLE);
  if (result == 0)
  {
    result = targeted_aura_custom_prompt(player, card, event, ptStack_68,
                                         "Select target creature or Vehicle.");
    return result;
  }
  result = generic_animating_aura_custom_prompt(player, card, event, ptStack_68,
                                                "Select target creature or Vehicle.", 0, 0, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x20ba870; original cards/aether_revolt.c:5114. */
static int is_aura_equipment_or_vehicle(int iid, int unused, int player, int card)
{
  int result;

  result = is_what(player, card, 0x40);
  if ((result != 0) && (result = has_subtype(player, card, SUBTYPE_EQUIPMENT), result != 0))
  {
    return 1;
  }
  result = is_what(player, card, 4);
  if ((result != 0) && (result = has_subtype(player, card, SUBTYPE_AURA), result != 0))
  {
    return 1;
  }
  result = is_what(player, card, 0x40);
  if (result != 0)
  {
    result = has_subtype(player, card, SUBTYPE_VEHICLE);
    return (unsigned int)(result != 0);
  }
  return 0;
}

/* ManalinkEh 2022: 0x20ca940; original cards/aether_revolt.c:5235. */
int card_tezzeret_the_schemer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t *ptStack_68;
  target_definition_t td;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      result = can_target(&td);
      result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Etherium Cell", 1,
                           5, 1, "Target creature gets -X/-X", result, 10, 0xfffffffe, "Emblem", 1,
                           0x14, 0xfffffff9, 0, 0, 0, 0);
      if (result == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (result == 2)
      {
        new_pick_target(&td, "TARGET_CREATURE", 0, 1);
      }
      goto LAB_020ca9bc;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_020ca9bc;
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    instance = get_card_instance(player, card);
  }
  ptStack_68 = &td;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    result = instance->info_slot;
    if (result == 1)
    {
      generate_token_by_id(player, card, 0x492e);
    }
    else if (result == 2)
    {
      result = valid_target(ptStack_68);
      if (result != 0)
      {

        result = count_subtype(player, 0x40, -1);
        pump_until_eot_merge_previous(instance->parent_controller, instance->parent_card,
                                      instance->targets[0].player, instance->targets[0].card,
                                      -result, -result);
      }
    }
    else if (result == 3)
    {
      generate_token_by_id(player, card, 0x41a9);
    }
  }
LAB_020ca9bc:
  result = planeswalker(player, card, event, 5);
  return result;
}

/* ManalinkEh 2022: 0x20cb170; original cards/aether_revolt.c:5399. */
int card_tezzeret_master_of_metal(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  target_definition_t *ptStack_194;
  target_definition_t td_player;
  target_definition_t td_art;
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    default_target_definition(player, card, &td_art, 0x40);
    instance = get_card_instance(player, card);
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      instance->targets[0].card = -1;
      instance->targets[0].player = -1;
      iVar2 = count_permanents_by_type(1 - player, TYPE_ARTIFACT | TYPE_CREATURE);
      iVar3 = can_target(&td_player);
      iVar4 = target_available(player, card, &td_art);
      iVar5 = can_target(&td_player);
      iVar2 = dialog_impl(
          player, card, EVENT_ACTIVATE, "\001\002", "\001\017", "Reveal until you get an artifact",
          1, 5, 1, "Target opponent loses life", iVar5, iVar4 * 2, 0xfffffffd,
          "Gain control of all artifacts and creatures", iVar3, iVar2 * 2, 0xfffffff8, 0, 0, 0, 0);
      if (iVar2 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar2 - 2U < 2)
      {
        pick_target(&td_player, "TARGET_PLAYER");
      }
      goto LAB_020cb210;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_020cb210;
    default_target_definition(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    default_target_definition(player, card, &td_art, 0x40);
    instance = get_card_instance(player, card);
  }
  ptStack_194 = &td_art;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    iVar2 = instance->info_slot;
    if (iVar2 == 1)
    {
      default_test_definition(&this_test, 0x40);
      reveal_card_until_you_reveal_test(player, player, &this_test, 0x24);
    }
    else if (iVar2 == 2)
    {
      if (-1 < instance->targets[0].player)
      {

        iVar2 = target_available(player, card, ptStack_194);
        lose_life(instance->targets[0].player, iVar2);
      }
    }
    else if (iVar2 == 3)
    {
      default_test_definition(&this_test, 0x42);
      new_manipulate_all(player, card, instance->targets[0].player, &this_test, ACT_GAIN_CONTROL);
    }
  }
LAB_020cb210:
  iVar2 = planeswalker(player, card, event, 5);
  return iVar2;
}

/* ManalinkEh 2022: 0x20cc7e0; original cards/aether_revolt.c:5811. */
int card_weldfast_engineer(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  td.special = 8;
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.allow_cancel = 0;
  if (event == EVENT_CHANGE_TYPE)
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      event_flags = event_flags | 0x1000000;
      return 0;
    }
  }
  else if ((event == EVENT_BEGIN_COMBAT) && (current_turn == player))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      result = can_target(&td);
      if (result != 0)
      {
        result = new_pick_target(&td, "Select target artifact creature you control", 0, 0x80);
        if (result != 0)
        {
          instance = get_card_instance(player, card);
          result = instance->targets[0].card;
          instance = get_card_instance(player, card);
          pump_until_eot(player, card, instance->targets[0].player, result, 2, 0);
        }
      }
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int midnight_entourage_effect(int player, int card, event_t event);
static int sly_requisitioner_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x20ba9d0; original cards/aether_revolt.c:3192. */
static int midnight_entourage_effect(int player, int card, event_t event)
{
  char cVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  unsigned int uVar4;
  int result;
  test_definition_t this_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar2 = get_card_instance(player, card);
      if ((-1 < pcVar2->damage_target_player) &&
          (result = get_id((int)pcVar2->damage_target_player, pcVar2->damage_target_card),
           result != 0x4171))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar2 = get_card_instance(player, card);
    if ((-1 < pcVar2->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, 0x4700), result != 0))
    {
      return 0;
    }
  LAB_020bab30:
    uVar4 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 == 0)
      goto LAB_020bab4e;
  }
  else
  {
    pcVar2 = get_card_instance(player, card);
    if ((-1 < pcVar2->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, event), result != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_020bab30;

    pcVar2 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);

    if ((pcVar3 != NULL) && (pcVar3 = get_card_instance(affected_card_controller, affected_card),
                             pcVar3->kill_code != '\0'))
    {
      cVar1 = pcVar2->damage_target_player;

      if ((-1 < cVar1) &&
          ((pcVar2->damage_target_card == affected_card && (cVar1 == affected_card_controller))))
      {
        result = is_what((int)cVar1, pcVar2->damage_target_card, 2);
        if (result == 0)
        {
          count_for_gfp_ability(player, card, 0x77, 2, 0x47, NULL);
        }
        pcVar2->damage_target_card = -1;
        pcVar2->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        if (pcVar2->targets[0xb].player < 0)
        {
          pcVar2->targets[0xb].player = 0;
        }
        *(uint8_t *)((int)&pcVar2->targets[0xb].player + 2) = 1;
        return 0;
      }
      default_test_definition(&this_test, 0x47);
      this_test.subtype = 0xec;
      count_for_gfp_ability(player, card, 0x77, player, 0, &this_test);
    }
    uVar4 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {

    uVar4 = uVar4 & 0xff;
    draw_cards(player, uVar4);
    for (; uVar4 != 0; uVar4 = uVar4 - 1)
    {
      lose_life(player, 1);
    }
    kill_card(player, card, KILL_EXILE);
  }
LAB_020bab4e:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c5840; original cards/aether_revolt.c:3254. */
int card_midnight_entourage(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  unsigned int uVar4;
  test_definition_t this_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      if (0 < active_cards_count[player])
      {
        iVar1 = 0;
        do
        {
          instance = in_play(player, iVar1);

          if ((((instance != NULL) && (iVar3 = is_what(player, iVar1, 0x80), iVar3 != 0)) &&
               (instance = get_card_instance(player, iVar1),
                instance->info_slot_fn == midnight_entourage_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_020c588d;
          iVar1 = iVar1 + 1;
        } while (iVar1 < active_cards_count[player]);
      }

      iVar1 = create_targetted_legacy_effect(player, card, midnight_entourage_effect, player, card);
      instance = get_card_instance(player, iVar1);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar1, 0x10000);
    }
  }
  else if (((event == EVENT_GRAVEYARD_FROM_PLAY) &&
            ((card != affected_card || (player != affected_card_controller)))) &&
           (iVar1 = is_humiliated_for_ability(player, card, AL_TRIG), iVar1 == 0))
  {
    default_test_definition(&this_test, 0x47);
    this_test.subtype = 0xec;
    this_test.not_me = 1;
    count_for_gfp_ability(player, card, 0x77, player, 0, &this_test);
  }
LAB_020c588d:
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (uVar4 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar4 != 0))
  {

    draw_cards(player, uVar4 & 0xff);
    lose_life(player, uVar4 & 0xff);
  }
  iVar1 =
      boost_creature_type(player, card, event, SUBTYPE_AETHERBORN, 1, 1, 0, BCT_CONTROLLER_ONLY);
  return iVar1;
}

/* ManalinkEh 2022: 0x20bc5f0; original cards/aether_revolt.c:4406. */
static char *has_a_counter_and_not_mana_src(int who_chooses, int player, int card)
{
  int result;

  result = count_counters(player, card, ~COUNTER_P1_P1);
  if (result != 0)
  {
    result = get_id(player, card);
    result = get_internal_card_id_from_csv_id(result);
    if (((cards_data[result].extra_ability & 0x1000) == 0) &&
        (result = is_humiliated_for_ability(player, card, AL_MANA), result == 0))
    {
      return NULL;
    }
  }
  return "Must have a counter.";
}

/* ManalinkEh 2022: 0x20bad30; original cards/aether_revolt.c:4911. */
static int sly_requisitioner_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  card_instance_t *pcVar3;
  int result;
  test_definition_t this_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar1 = get_card_instance(player, card);
      if ((-1 < pcVar1->damage_target_player) &&
          (result = get_id((int)pcVar1->damage_target_player, pcVar1->damage_target_card),
           result != 0x41a0))
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
  LAB_020bad68:
    uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
      goto LAB_020bad8a;
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
      goto LAB_020bad68;

    pcVar1 = get_card_instance(player, card);
    pcVar3 = in_play(affected_card_controller, affected_card);

    if ((pcVar3 != NULL) && (pcVar3 = get_card_instance(affected_card_controller, affected_card),
                             pcVar3->kill_code != '\0'))
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
      default_test_definition(&this_test, 0x40);
      this_test.type_flag = 3;
      count_for_gfp_ability(player, card, 0x77, player, 0, &this_test);
    }
    uVar2 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar2 == 0)
    {
      return 0;
    }
  }
  result = check_status(player, card, 0x10000);
  if (result == 0)
  {

    make_servo(player, card, uVar2 & 0xff);
    kill_card(player, card, KILL_EXILE);
  }
LAB_020bad8a:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c9e40; original cards/aether_revolt.c:4968. */
int card_sly_requisitioner(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  unsigned int uVar4;
  test_definition_t this_test;

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
                instance->info_slot_fn == sly_requisitioner_effect)) &&
              (((instance->damage_target_player == player &&
                 (instance->damage_target_card == card)) ||
                ((instance->targets[0].player == player && (instance->targets[0].card == card))))))
            goto LAB_020c9e65;
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[player]);
      }

      iVar2 = create_targetted_legacy_effect(player, card, sly_requisitioner_effect, player, card);
      instance = get_card_instance(player, iVar2);
      instance->targets[0].player = player;
      instance->targets[0].card = card;
      instance->number_of_targets = '\x01';
      add_status(player, iVar2, 0x10000);
    }
  }
  else if ((event == EVENT_GRAVEYARD_FROM_PLAY) &&
           (iVar2 = is_humiliated_for_ability(player, card, AL_TRIG), iVar2 == 0))
  {
    default_test_definition(&this_test, 0x40);
    this_test.type_flag = 3;
    count_for_gfp_ability(player, card, 0x77, player, 0, &this_test);
  }
LAB_020c9e65:
  instance = get_card_instance(player, card);

  if ((instance->kill_code == '\0') &&
      (uVar4 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY), uVar4 != 0))
  {

    make_servo(player, card, uVar4 & 0xff);
  }
  iVar2 = improvise(player, card, event);
  return iVar2;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20bfa40; original cards/aether_revolt.c:1308. */
int card_daredevil_dragster(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event == EVENT_DECLARE_ATTACKERS)
  {

    instance = get_card_instance(player, card);
    instance->info_slot = (int)(instance->state << 0x1d) >> 0x1f & 0x412b;
  }
  else if (event == EVENT_CLEANUP)
  {
    instance = get_card_instance(player, card);
    instance->info_slot = 0;
  }
  if ((((trigger_condition == 0xcc) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    instance = get_card_instance(player, card);
    if (instance->info_slot == 0x412b)
    {
      result = is_humiliated_for_ability(player, card, AL_TRIG);
      if (result == 0)
      {
        if (event == EVENT_TRIGGER)
        {
          event_result = event_result | 2;
        }
        else if (event == EVENT_RESOLVE_TRIGGER)
        {
          add_counters_from_source(player, card, player, card, COUNTER_VELOCITY, 1);
          result = count_counters(player, card, COUNTER_VELOCITY);
          if (1 < result)
          {
            kill_card(player, card, KILL_SACRIFICE);
            draw_cards(player, 2);
          }
        }
      }
    }
  }
  result = vehicle(player, card, event, 2);
  return result;
}

/* ManalinkEh 2022: 0x20c0170; original cards/aether_revolt.c:1465. */
int card_defiant_salvager(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event != EVENT_RESOLVE_ACTIVATION)
  {
    result = generic_husk(player, card, event, 0x42, 1, 1, 0, 0);
    return result;
  }

  instance = get_card_instance(player, card);
  add_counters_from_source(player, card, instance->parent_controller, instance->parent_card,
                           COUNTER_P1_P1, 1);
  return 0;
}

/* ManalinkEh 2022: 0x20c2ed0; original cards/aether_revolt.c:2406. */
int card_implement_of_ferocity(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.allow_cancel = 0;
    td.preferred_controller = player;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = aer_implement(player, card, event);
      return result;
    }
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allow_cancel = 0;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      result = valid_target(&td);
      if (result != 0)
      {
        add_counters_from_source(player, card, instance->targets[0].player,
                                 instance->targets[0].card, COUNTER_P1_P1, 1);
      }
    }
  }
  result = generic_activated_ability(player, card, event, 0x1c, 0, 0, 0, 1, 0, 0, 0, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x20c4620; original cards/aether_revolt.c:2867. */
int card_lifecraft_awakening(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    td.preferred_controller = player;
    get_card_instance(player, card);
    result = generic_spell(player, card, EVENT_CAN_CAST, 5, &td, NULL, 1, NULL);
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
    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event != EVENT_CHANGE_TARGET)
      goto joined_r0x020c4726;
  }
  else
  {
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
  }
  generic_spell(player, card, event, 5, &td, "TARGET_ARTIFACT", 1, NULL);
  if (spell_fizzled == 1)
  {
    return 0;
  }
joined_r0x020c4726:
  if (event == EVENT_RESOLVE_SPELL)
  {
    add_counters_from_source(player, card, instance->targets[0].player, instance->targets[0].card,
                             COUNTER_P1_P1, instance->info_slot);
    result = is_what(instance->targets[0].player, instance->targets[0].card, 0x3007);
    if (result == 0)
    {

      result = artifact_animation(player, card, instance->targets[0].player,
                                  instance->targets[0].card, 2, 0, 0, 0, 0);
      instance = get_card_instance(player, result);
      instance->targets[9].card = 0x26;
    }
    kill_card(player, card, KILL_DESTROY);
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c49f0; original cards/aether_revolt.c:2931. */
int card_lifecrafters_gift(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
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
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        default_target_definition(player, card, &td, 2);
        td.preferred_controller = player;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          add_counters_from_source(player, card, instance->targets[0].player,
                                   instance->targets[0].card, COUNTER_P1_P1, 1);
          iVar2 = current_turn;

          iVar4 = active_cards_count[current_turn] + -1;
          if (-1 < iVar4)
          {
            do
            {
              if (((player == iVar2) && (iVar3 = is_what(player, iVar4, 2), iVar3 != 0)) &&
                  (iVar3 = count_counters(player, iVar4, COUNTER_P1_P1), iVar3 != 0))
              {
                add_counters_from_source(player, card, player, iVar4, COUNTER_P1_P1, 1);
              }
              iVar4 = iVar4 + -1;
            } while (iVar4 != -1);
          }
          iVar2 = 1 - current_turn;
          iVar4 = active_cards_count[iVar2] + -1;
          if (-1 < iVar4)
          {
            do
            {
              if (((player == iVar2) && (iVar3 = is_what(player, iVar4, 2), iVar3 != 0)) &&
                  (iVar3 = count_counters(player, iVar4, COUNTER_P1_P1), iVar3 != 0))
              {
                add_counters_from_source(player, card, player, iVar4, COUNTER_P1_P1, 1);
              }
              iVar4 = iVar4 + -1;
            } while (iVar4 != -1);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_020c4a56;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = player;
  get_card_instance(player, card);
LAB_020c4a56:
  iVar2 = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x20ca050; original cards/aether_revolt.c:5019. */
int card_solemn_recruit(int player, int card, event_t event)
{
  int result;

  result = eot_revolt(player, card, event);
  if (result == 0)
  {
    return 0;
  }
  add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  return 0;
}

/* ManalinkEh 2022: 0x20cbe60; original cards/aether_revolt.c:5646. */
int card_untethered_express(int player, int card, event_t event)
{
  int result;

  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
  }
  result = vehicle(player, card, event, 1);
  return result;
}

/* ManalinkEh 2022: 0x20cbfd0; original cards/aether_revolt.c:5688. */
int card_verdant_automaton(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  if (event != EVENT_COUNT_MANA)
  {
    if (event == EVENT_RESOLVE_ACTIVATION)
    {

      instance = get_card_instance(player, card);
      add_counters_from_source(player, card, instance->parent_controller, instance->parent_card,
                               COUNTER_P1_P1, 1);
    }
    result = generic_activated_ability(player, card, event, 0, 3, 0, 0, 1, 0, 0, 0, NULL, NULL);
    return result;
  }
  result = generic_shade(player, card, EVENT_COUNT_MANA, 0, 3, 0, 0, 1, 0, 0, 1, 1, 0, 0);
  return result;
}

/* ManalinkEh 2022: 0x20cc120; original cards/aether_revolt.c:5707. */
int card_walking_ballista(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int32_t number;
  target_definition_t *ptStack_68;
  target_definition_t td1;

  instance = get_card_instance(player, card);
  if (event != EVENT_CAST_SPELL)
  {
    enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, instance->info_slot);
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      default_target_definition(player, card, &td1, 0x102);
      td1.zone = 0x1200;
      if (event == EVENT_CAN_ACTIVATE)
      {
        iVar2 = generic_shade(player, card, EVENT_CAN_ACTIVATE, 0, 4, 0, 0, 0, 0, 0, 1, 1, 0, 0);
        if (iVar2 != 0)
        {
          return 1;
        }
        iVar2 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x44, 0, 0, 0, 0, 0, 0,
                                          0, &td1, "TARGET_ANY");
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
      default_target_definition(player, card, &td1, 0x102);
    }
    ptStack_68 = &td1;
    td1.zone = 0x1200;
    if (event != EVENT_ACTIVATE)
    {
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        return 0;
      }
      iVar2 = instance->info_slot;
      if (iVar2 == 1)
      {
        add_counters_from_source(player, card, instance->parent_controller, instance->parent_card,
                                 COUNTER_P1_P1, 1);
        iVar2 = instance->info_slot;
      }
      if (iVar2 != 2)
      {
        return 0;
      }
      iVar2 = valid_target(ptStack_68);
      if (iVar2 == 0)
      {
        return 0;
      }
      damage_target0(player, card, 1);
      return 0;
    }

    iVar2 = generic_shade(player, card, EVENT_CAN_ACTIVATE, 0, 4, 0, 0, 0, 0, 0, 1, 1, 0, 0);
    iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 0x44, 0, 0, 0, 0, 0, 0, 0,
                                      ptStack_68, "TARGET_ANY");
    iVar2 = dialog_impl(player, card, EVENT_CAST_SPELL, "\001\002", "\001\016",
                        "Add a +1/+1 Counter", iVar2, (unsigned int)(current_phase == 0x18) * 9 + 1,
                        "Ping Target", iVar3, 5, 0, 0, 0, 0);
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
      iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 0x44, 0, 0, 0, 0, 0, 0, 0,
                                        ptStack_68, "TARGET_ANY");
      return iVar2;
    }
    iVar2 = generic_shade(player, card, EVENT_ACTIVATE, 0, 4, 0, 0, 0, 0, 0, 1, 1, 0, 0);
    return iVar2;
  }
  if (((card == affected_card) && (player == affected_card_controller)) &&
      (iVar2 = check_special_flags2(player, card, -0x80000000), iVar2 == 0))
  {

    instance->info_slot = 0;
    set_special_flags2(player, card, 0x20000);
    iVar2 = charge_mana_for_double_x(player, 6);
    if (spell_fizzled != 1)
    {

      iVar3 = true_get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, -1);
      number = (iVar2 - iVar3) / 2;
      instance->info_slot = number;
      goto LAB_020cc337;
    }
  }
  number = instance->info_slot;
LAB_020cc337:
  enters_the_battlefield_with_counters(player, card, EVENT_CAST_SPELL, COUNTER_P1_P1, number);
  return 0;
}

/* ManalinkEh 2022: 0x20ccdf0; original cards/aether_revolt.c:5916. */
int card_yahenni_undying_partisan(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;

  check_legend_rule(player, card, event);
  haste(player, card, event);
  count_for_gfp_ability(player, card, event, 1 - player, 2, NULL);
  result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (0 < result)
  {
    do
    {
      add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 1);
      result = result + -1;
    } while (result != 0);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {

    instance = get_card_instance(player, card);
    pump_ability_until_eot(instance->parent_controller, instance->parent_card,
                           instance->parent_controller, instance->parent_card, 0, 0, 0, 0x800);
  }
  result = generic_activated_ability(player, card, event, 0x40200, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int pias_revolution_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x20c0770; original cards/aether_revolt.c:1584. */
int card_embraal_gear_smasher(int player, int card, event_t event)
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
      damage_player(1 - player, 2, player, card);
    }
  }
  result = generic_activated_ability_with_sacrifice_as_cost(player, card, event, GAA_UNTAPPED, 0, 0,
                                                            0, 0, 0, 0, 0, NULL, NULL,
                                                            TYPE_ARTIFACT, NULL, SARV_0);
  return result;
}

/* ManalinkEh 2022: 0x20c1f50; original cards/aether_revolt.c:2108. */
int card_hidden_herbalists(int player, int card, event_t event)
{
  int result;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    result = get_trap_condition(player, 0x60c);
    if (result != 0)
    {
      produce_mana(player, 3, 2);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c2870; original cards/aether_revolt.c:2284. */
int card_ice_over(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  target_definition_t td;

  instance = in_play(player, card);
  if (((instance != NULL) &&
       (instance = get_card_instance(player, card), instance->damage_target_player != -1)) &&
      (iVar2 = is_humiliated_for_ability(player, card, AL_STATIC), iVar2 == 0))
  {

    instance = get_card_instance(player, card);
    does_not_untap((int)instance->damage_target_player, instance->damage_target_card, event);
  }
  if (event == EVENT_SHOULD_AI_PLAY)
  {

    instance = get_card_instance(player, card);
    if (instance->damage_target_player == '\0')
    {
      iVar2 = get_cmc(0, instance->damage_target_card);
      ai_modifier = ai_modifier + iVar2 * 4;
    }
    goto LAB_020c293c;
  }
  if (event == EVENT_CAN_CAST)
  {
  LAB_020c28d3:
    default_target_definition(player, card, &td, 0x42);
    get_card_instance(player, card);
  }
  else
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card != affected_card) || (player != affected_card_controller))
        goto LAB_020c293c;
      default_target_definition(player, card, &td, 0x42);
      instance = get_card_instance(player, card);
    }
    else
    {
      if (((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET)) ||
          (event == EVENT_CHANGE_TARGET))
        goto LAB_020c28d3;
    LAB_020c293c:
      if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
           (2 < event - EVENT_POWER)) &&
          (event != EVENT_RESOLVE_MOVING_AURA))
      {
        if ((event == EVENT_CHANGE_TYPE) || (event == EVENT_SET_COLOR))
          goto LAB_020c28d3;
        if ((event != EVENT_GET_SELECTED_CARD) && (event != EVENT_STATIC_EFFECTS))
        {
          return 0;
        }
      }
      default_target_definition(player, card, &td, 0x42);
      instance = get_card_instance(player, card);
      if (event != EVENT_CAST_SPELL)
        goto LAB_020c2900;
    }
    if ((card == affected_card) && (player == affected_card_controller))
    {
      instance->number_of_targets = '\0';
      iVar2 = new_pick_target(&td, "Select target artifact or creature.", 0, 0x81);
      if (iVar2 == 0)
      {
        return 0;
      }

      iVar3 = get_cmc(instance->targets[0].player, instance->targets[0].card);
      iVar2 = iVar3 * 8;
      iVar4 = is_tapped(instance->targets[0].player, instance->targets[0].card);
      if (iVar4 == 0)
      {
        iVar2 = iVar3 * -8;
      }
      ai_modifier = ai_modifier + iVar2;
      return 0;
    }
  }
LAB_020c2900:
  iVar2 =
      disabling_targeted_aura(player, card, event, &td, "Select target artifact or creature.", 0);
  return iVar2;
}

/* ManalinkEh 2022: 0x20bb540; original cards/aether_revolt.c:3668. */
static int pias_revolution_effect(int player, int card, event_t event)
{
  int32_t *piVar1;
  unsigned int uVar2;
  int *piVar3;
  card_instance_t *pcVar4;
  int result;
  int player_00;
  card_instance_t *pcVar6;
  target_t *ptVar7;
  bool bVar8;
  event_t event_00;
  int owner;
  int position;
  target_t dead_array[20];

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      pcVar4 = get_card_instance(player, card);
      if ((-1 < pcVar4->damage_target_player) &&
          (result = get_id((int)pcVar4->damage_target_player, pcVar4->damage_target_card),
           result != 0xd49))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    pcVar4 = get_card_instance(player, card);
    if (-1 < pcVar4->damage_target_player)
    {
      result = effect_follows_control_of_attachment(player, card, 0x4700);
      if (result != 0)
      {
        return 0;
      }
      goto LAB_020bb57b;
    }
    event_00 = EVENT_STATIC_EFFECTS;
  LAB_020bb616:
    result = resolve_gfp_ability(player, card, event_00, RESOLVE_TRIGGER_MANDATORY);
    if (result == 0)
    {
      return 0;
    }
  }
  else
  {
    pcVar4 = get_card_instance(player, card);
    if ((-1 < pcVar4->damage_target_player) &&
        (result = effect_follows_control_of_attachment(player, card, event), result != 0))
    {
      return 0;
    }
    if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {
      pcVar4 = in_play(player, card);
      if ((pcVar4 != NULL) &&
          (result = is_humiliated_for_ability(player, card, AL_TRIG), result == 0))
      {

        pcVar4 = get_card_instance(player, card);
        result = get_owner(affected_card_controller, affected_card);
        if (((result == player) &&
             (result = is_token(affected_card_controller, affected_card), result == 0)) &&
            (((((card != affected_card || (player != affected_card_controller)) &&
                (result = is_what(affected_card_controller, affected_card, 0x40), result != 0)) &&
               ((pcVar6 = in_play(affected_card_controller, affected_card),
                 pcVar6 != NULL && (result = is_this_dying(affected_card_controller, affected_card),
                                    result != 0)))) &&
              (result = check_for_death_trigger_removal(affected_card_controller, affected_card),
               result == 0))))
        {

          result = 1;
          do
          {
            if (pcVar4->targets[result].player == -1)
            {
              *(char *)((int)&pcVar4->targets[result].player + 1) = (char)affected_card_controller;
              *(char *)&pcVar4->targets[result].player = (char)affected_card;
              break;
            }
            if (pcVar4->targets[result].card == -1)
            {
              piVar1 = &pcVar4->targets[result].card;
              *(char *)((int)piVar1 + 1) = (char)affected_card_controller;
              *(char *)piVar1 = (char)affected_card;
              break;
            }
            result = result + 1;
          } while (result != 0xb);
          pcVar4->targets[0xb].player = 0x42;
        }
      }
      event_00 = EVENT_GRAVEYARD_FROM_PLAY;
      goto LAB_020bb616;
    }
  LAB_020bb57b:
    result = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (result == 0)
      goto LAB_020bb5a1;
  }

  pcVar4 = get_card_instance(player, card);
  pcVar4->targets[0xb].player = 0;
  ptVar7 = pcVar4->targets + 1;
  result = 0;
  do
  {
    uVar2 = ptVar7->player;
    if (uVar2 != 0xffffffff)
    {
      dead_array[result].player = uVar2 >> 8 & 0xff;
      dead_array[result].card = uVar2 & 0xff;
      ptVar7->player = -1;
      result = result + 1;
    }
    uVar2 = ptVar7->card;
    if (uVar2 != 0xffffffff)
    {
      dead_array[result].player = uVar2 >> 8 & 0xff;
      dead_array[result].card = uVar2 & 0xff;
      ptVar7->card = -1;
      result = result + 1;
    }
    ptVar7 = ptVar7 + 1;
  } while (ptVar7 != pcVar4->targets + 0xb);
  piVar3 = graveyard_ptr_mutable[player];
  if (0 < result)
  {
    player_00 = 1 - player;
    ptVar7 = (target_t *)(&owner + result * 2);
    do
    {
      result = find_in_owners_graveyard(ptVar7->player, ptVar7->card, &owner, &position);
      if (result != 0)
      {
        if (owner != player)
        {
          backtrace_and_abort("cards/aether_revolt.c:3736\nowner == player");
        }
        result = can_pay_life(player_00, 3);
        if ((result != 0) && (result = opponent_is_valid_target(player, card), result != 0))
        {
          result = my_base_value(player, position);
          result = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\010", player_00,
                               "\001\021", piVar3[position], "\001\014", "\001\003",
                               "Return to opponent's hand", 1, life[player_00] * -5 + 0x32,
                               "Pia's Revolution deals 3 damage to me", 1, result, 0, 0, 0, 0);
          if (result == 2)
          {
            damage_player(player_00, 3, player, card);
            goto LAB_020bb6fa;
          }
        }
        if (player == 0)
        {
          result = my_base_value(0, position);
          ai_modifier = ai_modifier - ((int)((result >> 0x1f & 3U) + result) >> 2);
        }
        from_grave_to_hand(player, position, 1);
      }
    LAB_020bb6fa:
      bVar8 = ptVar7 != dead_array;
      ptVar7 = ptVar7 + -1;
    } while (bVar8);
  }
LAB_020bb5a1:
  if ((event == EVENT_CLEANUP) && (result = check_status(player, card, 0x10000), result == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c6c70; original cards/aether_revolt.c:3764. */
int card_pias_revolution(int player, int card, event_t event)
{
  int32_t *piVar1;
  unsigned int uVar2;
  int *piVar3;
  int iVar4;
  card_instance_t *pcVar5;
  card_instance_t *pcVar6;
  int iVar7;
  target_t *ptVar8;
  bool bVar9;
  int owner;
  int position;
  target_t dead_array[22];

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      iVar4 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          pcVar6 = in_play(player, iVar4);

          if ((((pcVar6 != NULL) && (iVar7 = is_what(player, iVar4, 0x80), iVar7 != 0)) &&
               (pcVar6 = get_card_instance(player, iVar4),
                pcVar6->info_slot_fn == pias_revolution_effect)) &&
              (((pcVar6->damage_target_player == player && (pcVar6->damage_target_card == card)) ||
                ((pcVar6->targets[0].player == player && (pcVar6->targets[0].card == card))))))
            goto LAB_020c6c92;
          iVar4 = iVar4 + 1;
        } while (iVar4 < active_cards_count[player]);
      }

      iVar4 = create_targetted_legacy_effect(player, card, pias_revolution_effect, player, card);
      pcVar6 = get_card_instance(player, iVar4);
      pcVar6->targets[0].player = player;
      pcVar6->targets[0].card = card;
      pcVar6->number_of_targets = '\x01';
      add_status(player, iVar4, 0x10000);
    }
  }
  else if (((event == EVENT_GRAVEYARD_FROM_PLAY) &&
            (pcVar6 = in_play(player, card), pcVar6 != NULL)) &&
           (iVar4 = is_humiliated_for_ability(player, card, AL_TRIG), iVar4 == 0))
  {

    pcVar6 = get_card_instance(player, card);
    iVar4 = get_owner(affected_card_controller, affected_card);
    if ((((iVar4 == player) &&
          (iVar4 = is_token(affected_card_controller, affected_card), iVar4 == 0)) &&
         ((card != affected_card || (player != affected_card_controller)))) &&
        (((iVar4 = is_what(affected_card_controller, affected_card, 0x40),
           iVar4 != 0 &&
               (pcVar5 = in_play(affected_card_controller, affected_card), pcVar5 != NULL)) &&
          (iVar4 = is_this_dying(affected_card_controller, affected_card), iVar4 != 0))))
    {
      iVar7 = check_for_death_trigger_removal(affected_card_controller, affected_card);
      iVar4 = 0;
      if (iVar7 == 0)
      {
        do
        {

          if (pcVar6->targets[iVar4].player == -1)
          {
            *(char *)((int)&pcVar6->targets[iVar4].player + 1) = (char)affected_card_controller;
            *(char *)&pcVar6->targets[iVar4].player = (char)affected_card;
            break;
          }
          if (pcVar6->targets[iVar4].card == -1)
          {
            piVar1 = &pcVar6->targets[iVar4].card;
            *(char *)((int)piVar1 + 1) = (char)affected_card_controller;
            *(char *)piVar1 = (char)affected_card;
            break;
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 != 0xb);
        pcVar6->targets[0xb].player = 0x42;
      }
    }
  }
LAB_020c6c92:
  iVar4 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (iVar4 != 0)
  {

    pcVar6 = get_card_instance(player, card);
    pcVar6->targets[0xb].player = 0;
    ptVar8 = pcVar6->targets;
    iVar4 = 0;
    do
    {
      uVar2 = ptVar8->player;
      if (uVar2 != 0xffffffff)
      {
        dead_array[iVar4].player = uVar2 >> 8 & 0xff;
        dead_array[iVar4].card = uVar2 & 0xff;
        ptVar8->player = -1;
        iVar4 = iVar4 + 1;
      }
      uVar2 = ptVar8->card;
      if (uVar2 != 0xffffffff)
      {
        dead_array[iVar4].player = uVar2 >> 8 & 0xff;
        dead_array[iVar4].card = uVar2 & 0xff;
        ptVar8->card = -1;
        iVar4 = iVar4 + 1;
      }
      ptVar8 = ptVar8 + 1;
    } while (ptVar8 != pcVar6->targets + 0xb);
    piVar3 = graveyard_ptr_mutable[player];
    if (0 < iVar4)
    {
      iVar7 = 1 - player;
      ptVar8 = (target_t *)(&owner + iVar4 * 2);
      do
      {
        iVar4 = find_in_owners_graveyard(ptVar8->player, ptVar8->card, &owner, &position);
        if (iVar4 != 0)
        {
          if (owner != player)
          {
            backtrace_and_abort("cards/aether_revolt.c:3850\nowner == player");
          }
          iVar4 = can_pay_life(iVar7, 3);
          if ((iVar4 != 0) && (iVar4 = opponent_is_valid_target(player, card), iVar4 != 0))
          {
            iVar4 = my_base_value(player, position);
            iVar4 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\010", iVar7,
                                "\001\021", piVar3[position], "\001\014", "\001\003",
                                "Return to opponent's hand", 1, life[iVar7] * -5 + 0x32,
                                "Pia's Revolution deals 3 damage to me", 1, iVar4, 0, 0, 0, 0);
            if (iVar4 == 2)
            {
              damage_player(iVar7, 3, player, card);
              goto LAB_020c6f7a;
            }
          }
          if (player == 0)
          {
            iVar4 = my_base_value(0, position);
            ai_modifier = ai_modifier - ((int)((iVar4 >> 0x1f & 3U) + iVar4) >> 2);
          }
          from_grave_to_hand(player, position, 1);
        }
      LAB_020c6f7a:
        bVar9 = ptVar8 != dead_array;
        ptVar8 = ptVar8 + -1;
      } while (bVar9);
    }
  }
  iVar4 = global_enchantment(player, card, event);
  return iVar4;
}

/* ManalinkEh 2022: 0x20c8a00; original cards/aether_revolt.c:4413. */
int card_rishkar_peema_renegade(int player, int card, event_t event)
{
  target_t *ret_location;
  card_instance_t *instance;
  int result;
  int argument2;
  target_t *ptVar3;
  target_t *ptVar4;
  target_definition_t td;

  check_legend_rule(player, card, event);
  instance = get_card_instance(player, card);
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    result = can_target(&td);
    if (result != 0)
    {
      ret_location = instance->targets;
      result = select_target(player, card, &td, " Select target creature.", ret_location);
      if (result != 0)
      {
        state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
        result = can_target(&td);
        if ((result == 0) || (result = select_target(player, card, &td, " Select target creature.",
                                                     instance->targets + 1),
                              result == 0))
        {
          result = 1;
        }
        else
        {
          state_untargettable(instance->targets[1].player, instance->targets[1].card, 1);
          result = 2;
        }
        ptVar3 = ret_location;
        do
        {
          ptVar4 = ptVar3 + 1;
          state_untargettable(ptVar3->player, ptVar3->card, 0);
          add_counters_from_source(player, card, ptVar3->player, ptVar3->card, COUNTER_P1_P1, 1);
          ptVar3 = ptVar4;
        } while (ptVar4 != ret_location + result);
      }
    }
    instance->number_of_targets = '\0';
    default_target_definition(player, card, &td, 2);
  }
  else
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.allowed_controller = player;
    td.allow_cancel = 2;
    td.illegal_state = 0x1001;
    td.illegal_abilities = 0;
    td.special = 0x8000;
    td.extra = (int32_t)has_a_counter_and_not_mana_src;
    if (event == EVENT_CAN_ACTIVATE)
    {
      result = can_target(&td);
      return result;
    }
    if (event == EVENT_ACTIVATE)
    {
      result = pick_target(&td, "TARGET_CREATURE");
      if (result == 0)
      {
        spell_fizzled = 1;
      }
      else
      {
        instance->number_of_targets = '\x01';
        produce_mana_tapped(instance->targets[0].player, instance->targets[0].card, COLOR_GREEN, 1);
        result = instance->targets[0].player;
        if (result == player)
        {
          argument2 = instance->targets[0].card;
          if (argument2 == card)
          {
            return 0;
          }
        }
        else
        {
          argument2 = instance->targets[0].card;
        }
        dispatch_event(result, argument2, 0x81);
      }
    }
    else if (((event == EVENT_COUNT_MANA) && (card == affected_card)) &&
             (player == affected_card_controller))
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);

      if ((result == 0) && (result = target_available(player, card, &td), 0 < result))
      {
        declare_mana_available(player, 3, result);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20ca6b0; original cards/aether_revolt.c:5186. */
int card_tezzeret_the_schemer_emblem(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int iVar2;
  card_instance_t *pcVar3;
  int iVar4;
  int iVar5;
  int iStack_68;
  target_definition_t td;

  default_target_definition(player, card, &td, 0x40);
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.allow_cancel = 0;
  pcVar1 = get_card_instance(player, card);
  pcVar1->number_of_targets = '\0';
  if (event == EVENT_CHANGE_TYPE)
  {
    event_flags = event_flags | 0x1000000;
    return 0;
  }
  if (event != EVENT_BEGIN_COMBAT)
  {
    return 0;
  }
  if (current_turn != player)
  {
    return 0;
  }
  iVar2 = is_humiliated_for_ability(player, card, AL_TRIG);
  if (iVar2 != 0)
  {
    return 0;
  }

  iStack_68 = -1;
  iVar2 = 1 - player;
  if (0 < active_cards_count[iVar2])
  {
    iVar5 = 0;
    do
    {
      pcVar3 = in_play(iVar2, iVar5);

      if (((pcVar3 != NULL) && (iVar4 = is_what(iVar2, iVar5, 2), iVar4 != 0)) &&
          (iVar4 = get_toughness(iVar2, iVar5), iStack_68 < iVar4))
      {
        iStack_68 = iVar4;
      }
      iVar5 = iVar5 + 1;
    } while (iVar5 < active_cards_count[iVar2]);
  }
  pcVar3 = in_play(player, card);
  if (pcVar3 == NULL)
  {
    return 0;
  }
  iVar2 = can_target(&td);
  if (iVar2 == 0)
  {
    return 0;
  }
  iVar2 = pick_target(&td, "TARGET_ARTIFACT");
  if (iVar2 == 0)
  {
    return 0;
  }
  turn_into_creature(player, card, pcVar1->targets[0].player, pcVar1->targets[0].card, 0, 5, 5);
  iVar2 = is_what(player, pcVar1->targets[0].card, 2);
  if (iVar2 != 0)
  {
    return 0;
  }
  pcVar3 = get_card_instance(player, pcVar1->targets[0].card);
  if ((pcVar3->state & 0x20000) != 0)
  {
    return 0;
  }
  if (iStack_68 < 6)
  {
    iVar5 = my_base_value_by_id(pcVar1->targets[0].card);
    iVar2 = 1;
    if (iVar5 < 6)
      goto LAB_020ca891;
  }
  iVar2 = 2;
LAB_020ca891:
  iVar2 = do_dialog(player, player, card, -1, -1, " Attack with this artifact\n Pass", iVar2);
  if (iVar2 == 0)
  {
    iVar2 = check_for_special_ability(player, pcVar1->targets[0].card, 8);
    if (iVar2 == 0)
    {
      add_state(player, pcVar1->targets[0].card, 0x14);
    }
    else
    {
      add_state(player, pcVar1->targets[0].card, 4);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20cb740; original cards/aether_revolt.c:5543. */
int card_unbridled_growth(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  unsigned int uVar7;
  int player_00;
  target_definition_t td;

  instance = in_play(player, card);
  if ((instance == NULL) ||
      (instance = get_card_instance(player, card), instance->damage_target_player < '\0'))
  {
  LAB_020cb778:

    if (event == EVENT_CAN_CAST)
    {
      default_target_definition(player, card, &td, 1);
      td.preferred_controller = player;
      td.allowed_controller = player;
      get_card_instance(player, card);
      iVar2 = targeted_aura(player, card, EVENT_CAN_CAST, &td, NULL);
      return iVar2;
    }
    if (event != EVENT_CAST_SPELL)
      goto LAB_020cb788;
    if ((card != affected_card) || (player != affected_card_controller))
      goto LAB_020cb7ec;
    default_target_definition(player, card, &td, 1);
    td.preferred_controller = player;
    td.allowed_controller = player;
    instance = get_card_instance(player, card);
  LAB_020cb867:
    if ((card == affected_card) && (player == affected_card_controller))
    {
      instance->number_of_targets = '\0';
      ai_modifier = ai_modifier + 100;
      new_pick_target(&td, "Select target land you control.", 0, 0x81);
    }
  }
  else
  {
    instance = get_card_instance(player, card);
    iVar2 = instance->damage_target_card;
    player_00 = (int)instance->damage_target_player;
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar6 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 8, 0, 0, 0, 0, 0, 0, 0,
                                        NULL, NULL);
      if (iVar6 == 0)
      {
        iVar2 = mana_producer(player_00, iVar2, EVENT_CAN_ACTIVATE);
        return iVar2;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      iVar6 = hand_count[player];
      iVar3 = get_colors_of_mana_land_could_produce_ignoring_costs(player, card);
      iVar4 = can_sacrifice_this_as_cost(player, card);
      iVar5 = is_tapped(player_00, iVar2);
      uVar7 = 0;
      if (iVar5 == 0)
      {
        iVar5 = is_animated_and_sick(player_00, iVar2);
        uVar7 = (unsigned int)(iVar5 == 0);
      }
      iVar6 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\026",
                          "Add one mana of any color", uVar7, 5, "Sacrifice and draw", iVar4,
                          (unsigned int)(iVar6 + -2 <= iVar3) * 5 + 3, 0, 0, 0, 0);
      if (iVar6 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar6 == 1)
      {
        produce_mana_tapped_all_one_color(player_00, iVar2, COLOR_TEST_ANY_COLORED, 1);
        if (spell_fizzled != 1)
        {
          dispatch_event(player_00, iVar2, 0x81);
        }
      }
      else if (iVar6 == 2)
      {
        iVar2 = generic_activated_ability(player, card, EVENT_ACTIVATE, 8, 0, 0, 0, 0, 0, 0, 0,
                                          NULL, NULL);
        return iVar2;
      }
    }
    else
    {
      if (event != EVENT_RESOLVE_ACTIVATION)
      {
        if (event != EVENT_COUNT_MANA)
          goto LAB_020cb778;
        if (((card == affected_card) && (player == affected_card_controller)) &&
            (iVar2 = mana_producer(player_00, iVar2, EVENT_CAN_ACTIVATE), iVar2 != 0))
        {
          declare_mana_available_hex(player_00, 0x3e, 1);
        }
        goto LAB_020cb7ec;
      }
      if (instance->info_slot == 2)
      {
        draw_cards(player, 1);
        iVar2 = generic_activated_ability(player, card, EVENT_RESOLVE_ACTIVATION, 8, 0, 0, 0, 0, 0,
                                          0, 0, NULL, NULL);
        return iVar2;
      }
    }
  LAB_020cb788:
    if ((event == EVENT_CAN_CHANGE_TARGET) || (event == EVENT_RESOLVE_SPELL))
    {
    LAB_020cb79b:
      default_target_definition(player, card, &td, 1);
      td.preferred_controller = player;
      td.allowed_controller = player;
      get_card_instance(player, card);
    }
    else
    {
      if (event == EVENT_CHANGE_TARGET)
      {
        default_target_definition(player, card, &td, 1);
        td.preferred_controller = player;
        td.allowed_controller = player;
        get_card_instance(player, card);
        return 0;
      }
    LAB_020cb7ec:
      if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
           (2 < event - EVENT_POWER)) &&
          (event != EVENT_RESOLVE_MOVING_AURA))
      {
        if ((event == EVENT_CHANGE_TYPE) || (event == EVENT_SET_COLOR))
          goto LAB_020cb79b;
        if ((event != EVENT_GET_SELECTED_CARD) && (event != EVENT_STATIC_EFFECTS))
        {
          return 0;
        }
      }
      default_target_definition(player, card, &td, 1);
      td.preferred_controller = player;
      td.allowed_controller = player;
      instance = get_card_instance(player, card);
      if (event == EVENT_CAST_SPELL)
        goto LAB_020cb867;
    }
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar2 = targeted_aura(player, card, EVENT_RESOLVE_SPELL, &td, NULL);
      return iVar2;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20cc730; original cards/aether_revolt.c:5797. */
int card_welder_automaton(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    damage_player(1 - player, 1, player, card);
  }
  result = generic_activated_ability(player, card, event, 0, 3, 0, 0, 0, 1, 0, 0, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20c48e0; original cards/aether_revolt.c:2906. */
int card_lifecrafters_bestiary(int player, int card, event_t event)
{
  int result;

  upkeep_trigger_ability(player, card, event, player);
  if (event != EVENT_UPKEEP_TRIGGER_ABILITY)
  {
    sp_ability_trigger(player, event, 2, TYPE_CREATURE, NULL, player, card);
    if (event == EVENT_SPA)
    {
      result =
          charge_mana_while_resolving(player, card, EVENT_RESOLVE_TRIGGER, player, COLOR_GREEN, 1);
      if (result != 0)
      {
        draw_cards(player, 1);
      }
    }
    return 0;
  }
  scry(player, 1);
  sp_ability_trigger(player, EVENT_UPKEEP_TRIGGER_ABILITY, 2, TYPE_CREATURE, NULL, player, card);
  return 0;
}

/* ManalinkEh 2022: 0x20c6330; original cards/aether_revolt.c:3498. */
int card_paradox_engine(int player, int card, event_t event)
{
  test_definition_t this_test;

  check_legend_rule(player, card, event);
  sp_ability_trigger(player, event, 2, TYPE_ANY, NULL, player, card);
  if (event != EVENT_SPA)
  {
    return 0;
  }
  default_test_definition(&this_test, 0x146);
  new_manipulate_all(player, card, player, &this_test, ACT_UNTAP);
  return 0;
}

/* ManalinkEh 2022: 0x20ca430; original cards/aether_revolt.c:5125. */
int card_sram_senior_edificer(int player, int card, event_t event)
{
  int result;
  test_definition_t test;

  check_legend_rule(player, card, event);
  if ((((trigger_condition == 0xd3) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      (reason_for_trigger_controller == player))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      default_test_definition(&test, 0x47);
      test.special_selection_function = is_aura_equipment_or_vehicle;
      sp_ability_trigger(player, event, 2, TYPE_NONE, &test, player, card);
    }
  }
  if (event != EVENT_SPA)
  {
    return 0;
  }
  draw_cards(player, 1);
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20c18d0; original cards/aether_revolt.c:1991. */
int card_heart_of_kiran(int player, int card, event_t event)
{
  int iVar1;
  unsigned int uVar2;
  int iVar3;
  card_instance_t *instance;
  target_definition_t td;

  check_legend_rule(player, card, event);
  vigilance(player, card, event);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x100);
    td.allowed_controller = player;
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 6, 0, 0, 0, 0, 0, 0, 0,
                                        &td, "Choose a planeswalker you control");
      if (iVar3 == 0)
      {
        iVar3 = vehicle(player, card, EVENT_CAN_ACTIVATE, 3);
        return iVar3;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance->number_of_targets = '\0';
      instance->info_slot = 0;
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 6, 0, 0, 0, 0, 0, 0, 0,
                                        &td, "Choose a planeswalker you control");
      iVar1 = vehicle(player, card, EVENT_CAN_ACTIVATE, 3);
      uVar2 = (current_phase != 0x14) - 1 & 10;
      iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016",
                          "Remove a loyalty counter from a Planeswalker you control", iVar3, uVar2,
                          "Crew normally", iVar1, uVar2, 0, 0, 0, 0);
      if (iVar3 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      if (iVar3 != 1)
      {
        if (iVar3 != 2)
        {
          return 0;
        }
        iVar3 = vehicle(player, card, EVENT_ACTIVATE, 3);
        return iVar3;
      }
      iVar3 = generic_activated_ability(player, card, EVENT_ACTIVATE, 6, 0, 0, 0, 0, 0, 0, 0, &td,
                                        "Choose a planeswalker you control");
      return iVar3;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 0x100);
  }
  td.allowed_controller = player;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    iVar3 = instance->info_slot;
    if (iVar3 == 1)
    {
      remove_counters_from_source(player, card, player, instance->targets[0].card, COUNTER_LOYALTY,
                                  1);
      turn_into_creature(instance->parent_controller, instance->parent_card,
                         instance->parent_controller, instance->parent_card, 1, 0, 0);
      iVar3 = instance->info_slot;
    }
    if (iVar3 == 2)
    {
      iVar3 = vehicle(player, card, EVENT_RESOLVE_ACTIVATION, 3);
      return iVar3;
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static void make_thopter(int player, int card, int amount);

/* ManalinkEh 2022: 0x20ba940; original cards/aether_revolt.c:289. */
static void make_thopter(int player, int card, int amount)
{
  token_generation_t token;

  default_token_definition(player, card, 0x4995, &token);
  token.qty = (unsigned char)amount;
  token.tou = 1;
  token.pow = 1;
  token.key_plus = 0x20;
  generate_token(&token);
  return;
}

/* ManalinkEh 2022: 0x20c0700; original cards/aether_revolt.c:1569. */
int card_efficient_construction(int player, int card, event_t event)
{
  int result;

  sp_ability_trigger(player, event, 2, TYPE_ARTIFACT, NULL, player, card);
  if (event == EVENT_SPA)
  {
    make_thopter(player, card, 1);
  }
  result = global_enchantment(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x20c0fb0; original cards/aether_revolt.c:1761. */
int card_filigree_crawler(int player, int card, event_t event)
{
  int result;

  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  make_thopter(player, card, 1);
  return 0;
}

/* ManalinkEh 2022: 0x20c4ec0; original cards/aether_revolt.c:3019. */
int card_maverick_thopterist(int player, int card, event_t event)
{
  int result;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    make_thopter(player, card, 2);
  }
  result = improvise(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int scrap_trawler_legacy(int player, int card, event_t event);

/* ManalinkEh 2022: 0x20bb010; original cards/aether_revolt.c:4528. */
static int scrap_trawler_legacy(int player, int card, event_t event)
{
  int32_t *piVar1;
  card_instance_t *pcVar2;
  card_instance_t *pcVar3;
  unsigned int uVar4;
  int iVar5;
  int iVar6;
  int32_t *piVar7;
  test_definition_t this_test;

  pcVar2 = get_card_instance(player, card);
  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
         (-1 < pcVar2->damage_target_player)) &&
        (iVar5 = get_id((int)pcVar2->damage_target_player, pcVar2->damage_target_card),
         iVar5 != 0x4196))
    {
      kill_card(player, card, KILL_EXILE);
    }
    pcVar3 = get_card_instance(player, card);
    if ((-1 < pcVar3->damage_target_player) &&
        (iVar5 = effect_follows_control_of_attachment(player, card, 0x4700), iVar5 != 0))
    {
      return 0;
    }
  LAB_020bb0e0:
    uVar4 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 == 0)
      goto joined_r0x020bb108;
  }
  else
  {
    pcVar3 = get_card_instance(player, card);
    if ((-1 < pcVar3->damage_target_player) &&
        (iVar5 = effect_follows_control_of_attachment(player, card, event), iVar5 != 0))
    {
      return 0;
    }
    if (event != EVENT_GRAVEYARD_FROM_PLAY)
      goto LAB_020bb0e0;
    if ((((affected_card_controller == player) &&
          (iVar5 = is_what(player, affected_card, 0x40), iVar5 != 0)) &&
         (iVar5 = is_humiliated_for_ability((int)pcVar2->damage_target_player,
                                            pcVar2->damage_target_card, AL_TRIG),
          iVar5 == 0)) &&
        ((iVar5 = check_for_death_trigger_removal(affected_card_controller, affected_card),
          iVar5 == 0 &&
              (pcVar3 = in_play(affected_card_controller, affected_card), pcVar3 != NULL))))
    {

      pcVar3 = get_card_instance(affected_card_controller, affected_card);

      if (((pcVar3->kill_code != '\0') && ((-1 < pcVar2->damage_target_player &&
                                            (affected_card == pcVar2->damage_target_card)))) &&
          (affected_card_controller == pcVar2->damage_target_player))
      {
        pcVar2->damage_target_card = -1;
        pcVar2->damage_target_player = -1;
        remove_status(player, card, 0x10000);
        if (pcVar2->targets[0xb].player < 0)
        {
          pcVar2->targets[0xb].player = 0;
        }
        *(uint8_t *)((int)&pcVar2->targets[0xb].player + 2) = 1;
      }
      pcVar3 = in_play(affected_card_controller, affected_card);
      if (((pcVar3 != NULL) &&
           ((iVar5 = is_what(affected_card_controller, affected_card, 0x40),
             iVar5 != 0 || ((pcVar2->targets[0].card == affected_card &&
                             (pcVar2->targets[0].player == affected_card_controller)))))) &&
          (iVar5 = is_this_dying(affected_card_controller, affected_card), iVar5 != 0))
      {

        iVar5 = pcVar2->targets[0xb].player;
        if (iVar5 < 0)
        {
          pcVar2->targets[0xb].player = 0;
          iVar5 = 0;
        }
        else if (10 < iVar5)
          goto LAB_020bb068;
        iVar6 = get_cmc(affected_card_controller, affected_card);
        pcVar2->targets[iVar5 + 1].card = iVar6;
        pcVar2->targets[0xb].player = pcVar2->targets[0xb].player + 1;
        if (((card_counts[player][5] != '\0') &&
             (iVar5 = is_what(affected_card_controller, affected_card, 2), iVar5 != 0)) &&
            (pcVar2->targets[0xb].player < 0xb))
        {

          uVar4 = (unsigned int)card_counts[player][5];
          while ((uVar4 != 0 && (iVar5 = pcVar2->targets[0xb].player, iVar5 < 0xb)))
          {
            uVar4 = uVar4 - 1;
            iVar6 = get_cmc(affected_card_controller, affected_card);
            pcVar2->targets[iVar5 + 1].card = iVar6;
            pcVar2->targets[0xb].player = pcVar2->targets[0xb].player + 1;
          }
        }
      }
    }
  LAB_020bb068:
    uVar4 = resolve_gfp_ability(player, card, EVENT_GRAVEYARD_FROM_PLAY, RESOLVE_TRIGGER_MANDATORY);
    if (uVar4 == 0)
    {
      return 0;
    }
  }
  iVar5 = check_status(player, card, 0x10000);
  if (iVar5 == 0)
  {

    if ((uVar4 & 0xff) != 0)
    {
      piVar1 = &pcVar2->targets[1].card;
      piVar7 = &pcVar2->targets[(uVar4 & 0xff) + 1].card;
      do
      {
        while (true)
        {
          default_test_definition(&this_test, 0x40);
          scnprintf(this_test.message, 100, "Select an artifact card with CMC %d or less.",
                    *piVar7 + -1);
          this_test.cmc = *piVar7;
          this_test.cmc_flag = 3;
          iVar5 = new_special_count_grave(player, &this_test);
          if (iVar5 == 0)
            break;
          piVar7 = piVar7 + -2;
          new_global_tutor(player, player, 2, 1, 0, 1, &this_test);
          if (piVar7 == piVar1)
            goto LAB_020bb258;
        }
        piVar7 = piVar7 + -2;
      } while (piVar7 != piVar1);
    }
  LAB_020bb258:
    kill_card(player, card, KILL_EXILE);
  }
joined_r0x020bb108:
  if ((event == EVENT_CLEANUP) && (iVar5 = check_status(player, card, 0x10000), iVar5 == 0))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c8e70; original cards/aether_revolt.c:4612. */
int card_scrap_trawler(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  unsigned int uVar2;
  int iVar3;
  int iVar4;
  int32_t *piVar5;
  card_instance_t *pcStack_ec;
  test_definition_t this_test;

  if (event == EVENT_STATIC_EFFECTS)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {

      iVar3 = 0;
      if (0 < active_cards_count[player])
      {
        do
        {
          pcVar1 = in_play(player, iVar3);

          if ((((pcVar1 != NULL) && (iVar4 = is_what(player, iVar3, 0x80), iVar4 != 0)) &&
               (pcVar1 = get_card_instance(player, iVar3),
                pcVar1->info_slot_fn == scrap_trawler_legacy)) &&
              (((pcVar1->damage_target_player == player && (pcVar1->damage_target_card == card)) ||
                ((pcVar1->targets[0].player == player && (pcVar1->targets[0].card == card))))))
            goto LAB_020c8edc;
          iVar3 = iVar3 + 1;
        } while (iVar3 < active_cards_count[player]);
      }

      iVar3 = create_targetted_legacy_effect(player, card, scrap_trawler_legacy, player, card);
      pcVar1 = get_card_instance(player, iVar3);
      pcVar1->targets[0].player = player;
      pcVar1->targets[0].card = card;
      pcVar1->number_of_targets = '\0';
      add_status(player, iVar3, 0x10000);
    }
  LAB_020c8edc:
    pcStack_ec = get_card_instance(player, card);
  }
  else
  {
    pcStack_ec = get_card_instance(player, card);
    if (((event == EVENT_GRAVEYARD_FROM_PLAY) && (affected_card_controller == player)) &&
        ((iVar3 = check_for_death_trigger_removal(player, affected_card),
          iVar3 == 0 &&
              (((pcVar1 = in_play(affected_card_controller, affected_card),
                 pcVar1 != NULL && (iVar3 = is_what(affected_card_controller, affected_card, 0x40),
                                    iVar3 != 0)) &&
                (iVar3 = is_this_dying(affected_card_controller, affected_card), iVar3 != 0))))))
    {

      iVar3 = pcStack_ec->targets[0xb].player;
      if (iVar3 < 0)
      {
        pcStack_ec->targets[0xb].player = 0;
        iVar3 = 0;
      }
      else if (10 < iVar3)
        goto LAB_020c8eaf;
      iVar4 = get_cmc(affected_card_controller, affected_card);
      pcStack_ec->targets[iVar3].card = iVar4;
      pcStack_ec->targets[0xb].player = pcStack_ec->targets[0xb].player + 1;
      if (((card_counts[player][5] != '\0') &&
           (iVar3 = is_what(affected_card_controller, affected_card, 2), iVar3 != 0)) &&
          (pcStack_ec->targets[0xb].player < 0xb))
      {

        uVar2 = (unsigned int)card_counts[player][5];
        while ((uVar2 != 0 && (iVar3 = pcStack_ec->targets[0xb].player, iVar3 < 0xb)))
        {
          iVar4 = get_cmc(affected_card_controller, affected_card);
          pcStack_ec->targets[iVar3].card = iVar4;
          uVar2 = uVar2 - 1;
          pcStack_ec->targets[0xb].player = pcStack_ec->targets[0xb].player + 1;
        }
      }
    }
  }
LAB_020c8eaf:
  pcVar1 = get_card_instance(player, card);
  if (pcVar1->kill_code == '\0')
  {

    uVar2 = resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
    if ((uVar2 & 0xff) != 0)
    {
      piVar5 = &pcStack_ec->targets[uVar2 & 0xff].card;
      do
      {
        default_test_definition(&this_test, 0x40);
        scnprintf(this_test.message, 100, "Select an artifact card with CMC %d or less.",
                  *piVar5 + -1);
        this_test.cmc = *piVar5;
        this_test.cmc_flag = 3;
        iVar3 = new_special_count_grave(player, &this_test);
        if (iVar3 != 0)
        {
          new_global_tutor(player, player, 2, 1, 0, 1, &this_test);
        }
        piVar5 = piVar5 + -2;
      } while (&pcStack_ec->targets[0].card != piVar5);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static int quicksmith_spy_effect(int player, int card, event_t event);
static int quicksmith_rebel_effect(int player, int card, event_t event);

/* ManalinkEh 2022: 0x20bbcb0; original cards/aether_revolt.c:3891. */
static int quicksmith_rebel_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;
  target_definition_t td;

  pcVar1 = get_card_instance(player, card);
  pcVar2 = get_card_instance(player, card);
  if (pcVar2->targets[1].player < 0)
  {
  LAB_020bbce3:
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      result = attachment_granting_activated_ability(player, card, event, 5, 0, 0, 0, 0, 0, 0, 0,
                                                     &td, "TARGET_ANY");
      return result;
    }
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (result =
             validate_arbitrary_target(&td, pcVar1->targets[0].player, pcVar1->targets[0].card),
         result != 0))
    {
      damage_target0(player, card, 2);
    }
  }
  else
  {
    other_leaves_play_2022(player, card, pcVar1->targets[1].player, pcVar1->targets[1].card, event,
                           0, 0);
    if (event == EVENT_LEAVES_PLAY_ABILITY)
    {
    LAB_020bbe78:
      kill_card(player, card, KILL_EXILE);
    }
    else
    {
      if (event != EVENT_CARDCONTROLLED)
        goto LAB_020bbce3;
      if ((pcVar1->targets[1].card == affected_card) &&
          (pcVar1->targets[1].player == affected_card_controller))
        goto LAB_020bbe78;
    }
    default_target_definition(player, card, &td, 0x102);
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c72e0; original cards/aether_revolt.c:3921. */
int card_quicksmith_rebel(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;
  target_definition_t td;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }

  default_target_definition(player, card, &td, 0x40);
  td.allow_cancel = 0;
  td.preferred_controller = player;
  pcVar1 = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = pick_target(&td, "TARGET_ARTIFACT");
    if (result != 0)
    {

      result = create_targetted_legacy_activate(player, card, quicksmith_rebel_effect,
                                                pcVar1->targets[0].player, pcVar1->targets[0].card);
      pcVar3 = get_card_instance(player, result);
      pcVar3->targets[1].player = player;
      pcVar3->targets[1].card = card;
    }
  }
  pcVar1->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x20bbb00; original cards/aether_revolt.c:3949. */
static int quicksmith_spy_effect(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  card_instance_t *pcVar2;
  int result;

  pcVar1 = get_card_instance(player, card);
  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE))
  {
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      draw_a_card(player);
      pcVar2 = get_card_instance(player, card);
      if (-1 < pcVar2->targets[1].player)
      {
        other_leaves_play_2022(player, card, pcVar1->targets[1].player, pcVar1->targets[1].card,
                               EVENT_RESOLVE_ACTIVATION, 0, 0);
      }
    }
    else
    {
      pcVar2 = get_card_instance(player, card);
      if ((-1 < pcVar2->targets[1].player) &&
          ((other_leaves_play_2022(player, card, pcVar1->targets[1].player, pcVar1->targets[1].card,
                                   event, 0, 0),
            event == EVENT_LEAVES_PLAY_ABILITY ||
                (((event == EVENT_CARDCONTROLLED && (pcVar1->targets[1].card == affected_card)) &&
                  (pcVar1->targets[1].player == affected_card_controller))))))
      {
        kill_card(player, card, KILL_EXILE);
      }
    }
    return 0;
  }
  result = attachment_granting_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0, 0, NULL,
                                                 NULL);
  return result;
}

/* ManalinkEh 2022: 0x20c73d0; original cards/aether_revolt.c:3978. */
int card_quicksmith_spy(int player, int card, event_t event)
{
  card_instance_t *pcVar1;
  int result;
  card_instance_t *pcVar3;
  target_definition_t td;

  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }

  default_target_definition(player, card, &td, 0x40);
  td.allow_cancel = 0;
  td.preferred_controller = player;
  pcVar1 = get_card_instance(player, card);
  result = can_target(&td);
  if (result != 0)
  {
    result = pick_target(&td, "TARGET_ARTIFACT");
    if (result != 0)
    {

      result = create_targetted_legacy_activate(player, card, quicksmith_spy_effect,
                                                pcVar1->targets[0].player, pcVar1->targets[0].card);
      pcVar3 = get_card_instance(player, result);
      pcVar3->targets[1].player = player;
      pcVar3->targets[1].card = card;
    }
  }
  pcVar1->number_of_targets = '\0';
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20c9230; original cards/aether_revolt.c:4690. */
int card_scrounging_bandar(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_t td;

  enters_the_battlefield_with_counters(player, card, event, COUNTER_P1_P1, 2);
  if (player == 0)
  {
    upkeep_trigger_ability_mode(0, card, event, 0, RESOLVE_TRIGGER_OPTIONAL);
    if (ai_is_speculating != 1)
      goto joined_r0x020c9314;
  }
  else if ((ai_is_speculating != 1) && ((player != 1 || (((unsigned char)trace_mode & 2) != 0))))
    goto joined_r0x020c9314;
  iVar2 = count_subtype(player, 2, -1);
  if (1 < iVar2)
  {
    iVar2 = duh_mode(player);
    upkeep_trigger_ability_mode(player, card, event, player,
                                (iVar2 != 0) + RESOLVE_TRIGGER_OPTIONAL);
  }
joined_r0x020c9314:
  if (event == EVENT_UPKEEP_TRIGGER_ABILITY)
  {

    instance = get_card_instance(player, card);
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.special = 0x4000;
    iVar2 = can_target(&td);

    if (((iVar2 != 0) && (iVar2 = new_pick_target(&td, "TARGET_CREATURE", 0, 0), iVar2 != 0)) &&
        (iVar2 = count_counters(player, card, COUNTER_P1_P1), 0 < iVar2))
    {

      if (((player != 1) &&
           (iVar3 = choose_a_number(player, "Move how many +1/+1 counters?", iVar2),
            iVar3 <= iVar2)) &&
          (-1 < iVar3))
      {
        iVar2 = iVar3;
      }
      move_counters_from_source(player, card, instance->targets[0].player,
                                instance->targets[0].card, player, card, COUNTER_P1_P1, iVar2);
    }
  }
  return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20c4030; original cards/aether_revolt.c:2740. */
int card_ironclad_revolutionary(int player, int card, event_t event)
{
  int result;
  char *pcVar2;
  char *pcVar3;
  unsigned char bVar4;
  char msg[100];
  test_definition_t test;

  bVar4 = 0;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    pcVar2 = "Select an artifact to sacrifice.";
    pcVar3 = msg;
    for (result = 8; result != 0; result = result + -1)
    {
      *(uint32_t *)pcVar3 = *(uint32_t *)pcVar2;
      pcVar2 = pcVar2 + (unsigned int)bVar4 * -8 + 4;
      pcVar3 = pcVar3 + (unsigned int)bVar4 * -8 + 4;
    }
    *pcVar3 = *pcVar2;
    pcVar2 = msg + 0x24;
    for (result = 0x10; result != 0; result = result + -1)
    {
      pcVar2[0] = '\0';
      pcVar2[1] = '\0';
      pcVar2[2] = '\0';
      pcVar2[3] = '\0';
      pcVar2 = pcVar2 + (unsigned int)bVar4 * -8 + 4;
    }
    msg[0x21] = '\0';
    msg[0x22] = '\0';
    msg[0x23] = '\0';
    new_default_test_definition(&test, 0x40, msg);
    result = new_can_sacrifice(player, card, player, &test);
    if (result != 0)
    {
      result = new_sacrifice(player, card, player, 0, &test);
      if (result != 0)
      {
        add_counters_from_source(player, card, player, card, COUNTER_P1_P1, 2);
        lose_life(1 - player, 2);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x20c5290; original cards/aether_revolt.c:3083. */
int card_merchants_dockhand(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  int card_00;
  char *pcVar4;
  bool bVar5;
  unsigned char bVar6;
  target_definition_t td;
  char marked[2][151];

  bVar6 = 0;
  if (event == EVENT_CAN_ACTIVATE)
  {
  LAB_020c52d5:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_020c52de;
  }
  else
  {
    if (event == EVENT_ACTIVATE)
    {

      generic_activated_ability(player, card, EVENT_ACTIVATE, 1, 3, 0, 1, 0, 0, 0, 0, NULL, NULL);
      if (spell_fizzled != 1)
      {
        default_target_definition(player, card, &td, 0x40);
        td.allowed_controller = player;
        td.preferred_controller = player;
        td.illegal_state = 1;
        td.illegal_abilities = 0;
        td.allow_cancel = 3;
        uVar3 = 0x12e;
        bVar5 = ((unsigned int)marked & 2) != 0;
        pcVar4 = marked[0];
        if (bVar5)
        {
          marked[0][0] = '\0';
          marked[0][1] = '\0';
          pcVar4 = marked[0] + 2;
          uVar3 = 300;
        }
        for (uVar3 = uVar3 >> 2; uVar3 != 0; uVar3 = uVar3 - 1)
        {
          pcVar4[0] = '\0';
          pcVar4[1] = '\0';
          pcVar4[2] = '\0';
          pcVar4[3] = '\0';
          pcVar4 = pcVar4 + (unsigned int)bVar6 * -8 + 4;
        }
        if (!bVar5)
        {
          pcVar4[0] = '\0';
          pcVar4[1] = '\0';
        }
        result = mark_up_to_n_targets_noload(&td, "Select an untapped artifact you control.", -1,
                                             marked);

        if ((0 < result) && (0 < active_cards_count[player]))
        {
          card_00 = 0;
          do
          {
            if ((*(char *)((int)marked + card_00 + player * 0x97) != '\0') &&
                (instance = in_play(player, card_00), instance != NULL))
            {
              tap_card(player, card_00);
            }
            card_00 = card_00 + 1;
          } while (card_00 < active_cards_count[player]);
        }
        instance = get_card_instance(player, card);
        instance->info_slot = result;
      }
      return 0;
    }
    if (event != EVENT_RESOLVE_ACTIVATION)
    {
      if (event != EVENT_CLEANUP)
      {
        return 0;
      }
      goto LAB_020c52d5;
    }
  }

  instance = get_card_instance(player, card);
  impulse_effect(player, instance->info_slot, 0);
LAB_020c52de:
  result = generic_activated_ability(player, card, event, 1, 3, 0, 1, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20c16c0; original cards/aether_revolt.c:1955. */
int card_gremlin_infestation(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    target_definition_t target;
    token_generation_t token;
    uint8_t bytes[84];
  } recovered_stack_td;

  instance = in_play(player, card);
  if ((((instance != NULL) &&
        (result = is_humiliated_for_ability(player, card, AL_TRIG), result == 0)) &&
       (instance = get_card_instance(player, card), -1 < instance->damage_target_player)) &&
      ((result = eot_trigger(player, card, event), result != 0 && (current_turn == player))))
  {
    instance = get_card_instance(player, card);
    damage_player((int)instance->damage_target_player, 2, player, card);
  }
  result = attached_creature_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    default_token_definition(player, card, 0x4942, &recovered_stack_td.token);
    recovered_stack_td.target.illegal_abilities = 2;
    recovered_stack_td.target.required_abilities = 2;
    recovered_stack_td.token.color_forced = 0x10;
    generate_token(&recovered_stack_td.token);
  }
  if (event != EVENT_CAN_CAST)
  {
    if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
        goto LAB_020c1740;
    }
    else if ((event == EVENT_RESOLVE_SPELL) ||
             ((event == EVENT_CAN_CHANGE_TARGET || (event == EVENT_CHANGE_TARGET))))
      goto LAB_020c1740;
    if ((((event & ~EVENT_SET_LEGACY_EFFECT_NAME) != EVENT_CAN_MOVE_AURA) &&
         ((((2 < event - EVENT_POWER && (event != EVENT_RESOLVE_MOVING_AURA)) &&
            (event != EVENT_CHANGE_TYPE)) &&
           ((event != EVENT_SET_COLOR && (event != EVENT_GET_SELECTED_CARD)))))) &&
        (event != EVENT_STATIC_EFFECTS))
    {
      return 0;
    }
  }
LAB_020c1740:
  default_target_definition(player, card, &recovered_stack_td.target, 0x40);
  recovered_stack_td.target.preferred_controller = 1 - player;
  result = targeted_aura(player, card, event, &recovered_stack_td.target, "TARGET_ARTIFACT");
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20c1e70; original cards/aether_revolt.c:2090. */
int card_heroic_intervention(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  if (event == EVENT_RESOLVE_SPELL)
  {
    if (0 < active_cards_count[player])
    {
      iVar1 = 0;
    LAB_020c1ec2:
      do
      {
        instance = in_play(player, iVar1);
        if (instance != NULL)
        {
          iVar3 = is_what(player, iVar1, 0x47);
          if (iVar3 != 0)
          {
            iVar3 = iVar1 + 1;
            pump_ability_until_eot(player, card, player, iVar1, 0, 0, 0,
                                   (SP_KEYWORD_INDESTRUCTIBLE | SP_KEYWORD_HEXPROOF));
            iVar1 = iVar3;
            if (active_cards_count[player] <= iVar3)
              break;
            goto LAB_020c1ec2;
          }
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player]);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  iVar1 = basic_spell(player, card, event);
  return iVar1;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20ca0b0; original cards/aether_revolt.c:5034. */
int card_spire_of_industry(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  card_instance_t *instance;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    iVar1 = check_battlefield_for_subtype(player, 0x40, -1);
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar1 = is_humiliated_for_ability(player, card, AL_MANA);
      if ((iVar1 == 0) &&
          ((((ai_is_speculating == 1 ||
              ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))) &&
             (life[player] < 2)) &&
            (instance = get_card_instance(player, card), (instance->mana_color & 1) == 0))))
      {
        return 0;
      }
      goto LAB_020ca10a;
    }
    if (event == EVENT_ACTIVATE)
    {

      instance = get_card_instance(player, card);
      if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
      {
        if ((1 < life[player] && iVar1 != 0) && ((instance->mana_color & 1) == 0))
          goto LAB_020ca2e2;
      }
      else if (iVar1 != 0)
      {
      LAB_020ca2e2:
        produce_mana_tapped_all_one_color(player, card, (unsigned int)instance->mana_color, 1);
        if ((chosen_colors &
             ((mana_colors_added[player] | COLOR_TEST_COLORLESS) ^ ~COLOR_TEST_0)) == COLOR_TEST_0)
        {
          return 0;
        }
        lose_life(player, 1);
        return 0;
      }
      produce_mana_tapped_all_one_color(player, card, COLOR_TEST_COLORLESS, 1);
      return 0;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
      goto LAB_020ca10a;
    iVar1 = check_battlefield_for_subtype(player, 0x40, -1);
  }
  if (((event == EVENT_COUNT_MANA) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    iVar2 = is_tapped(player, card);
    if (((iVar2 == 0) && (iVar2 = is_animated_and_sick(player, card), iVar2 == 0)) &&
        (iVar2 = can_produce_mana(player, card), iVar2 != 0))
    {
      iVar2 = check_special_flags2(player, card, 0x40);
      if ((iVar2 == 0) || (iVar2 = is_what(player, card, 1), iVar2 == 0))
      {

        instance = get_card_instance(player, card);
        if ((ai_is_speculating == 1) || ((player == 1 && (((unsigned char)trace_mode & 2) == 0))))
        {
          if ((life[player] < 2) || (iVar1 == 0))
          {
            if ((instance->mana_color & 1) == 0)
            {
              return 0;
            }
            declare_mana_available(player, 0, 1);
            return 0;
          }
        }
        else if (iVar1 == 0)
        {
          declare_mana_available(player, mana_colors_added[player], 1);
          return 0;
        }
        declare_mana_available_hex(player, (unsigned int)instance->mana_color, 1);
      }
      else
      {
        declare_mana_available(player, 1, 1);
      }
    }
    return 0;
  }
LAB_020ca10a:
  iVar1 = mana_producer(player, card, event);
  return iVar1;
}

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20c4d60; original cards/aether_revolt.c:2986. */
int card_maulfist_revolutionary(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_2022_t td;

  default_target_definition(player, card, &td, 0x47);
  td.zone = 0x1200;
  td.allow_cancel = 0;
  instance = get_card_instance(player, card);
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result != 0)
  {
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Select target permanent or player.", 0, 0x80);
      if (result != 0)
      {
        increment_all_counters(player, card, instance->targets[0].player,
                               instance->targets[0].card);
      }
    }
  }
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  result = can_target(&td);
  if (result != 0)
  {
    result = new_pick_target(&td, "Select target permanent or player.", 0, 0x80);
    if (result != 0)
    {
      increment_all_counters(player, card, instance->targets[0].player, instance->targets[0].card);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x20c9d80; original cards/aether_revolt.c:4889. */
int card_skyship_plunderer(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_2022_t td;

  default_target_definition(player, card, &td, 0x47);
  td.zone = 0x1200;
  td.allow_cancel = 0;
  result =
      damage_dealt_by_me(player, card, event, DDBM_MUST_DAMAGE_PLAYER | DDBM_MUST_BE_COMBAT_DAMAGE);
  if (result != 0)
  {
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "Select target permanent or player.", 0, 0x80);
      if (result != 0)
      {

        instance = get_card_instance(player, card);
        increment_all_counters(player, card, instance->targets[0].player,
                               instance->targets[0].card);
      }
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 energy-card operands. */

/* ManalinkEh 2022: 0x20bd2c0; original cards/aether_revolt.c:547. */
int card_aetherwind_basker(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if ((event == EVENT_ETB) || (event == EVENT_DAT_ABILITY))
  {
    result = count_subtype(player, 2, -1);
    add_energy(player, result);
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    result = instance->parent_card;
    instance = get_card_instance(player, card);
    pump_until_eot_merge_previous(player, card, instance->parent_controller, result, 1, 1);
  }
  result = generic_activated_ability(player, card, event, 0, 0, 0, 0, 0, 0, 0,
                                          GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 1), NULL, NULL);
  return result;
}


/* ManalinkEh 2022: 0x20bf370; original cards/aether_revolt.c:1193. */
int card_consulate_turret(int player, int card, event_t event)
{
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  uint32_t variable_costs;
  target_definition_2022_t *td;
  char *prompt;
  target_definition_2022_t td1;

  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    td = &td1;
    default_target_definition(player, card, td, 0x100);
    td1.zone = 0x1000;
    if (event == EVENT_CAN_ACTIVATE)
    {
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0,
                                             0, NULL, NULL);
      if (iVar3 == 0)
      {
        iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0,
                                               0, GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 3), td,
                                               "TARGET_PLAYER_OR_PLANESWALKER");
        return iVar3;
      }
      return 1;
    }
    if (event == EVENT_ACTIVATE)
    {
      instance = get_card_instance(player, card);
      instance->info_slot = 0;
      iVar3 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 1, 0, 0, 0, 0, 0, 0,
                                             0, NULL, NULL);
      iVar1 = generic_activated_ability(player, card, EVENT_CAN_ACTIVATE, 5, 0, 0, 0, 0, 0, 0,
                                             GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 3), td,
                                             "TARGET_PLAYER_OR_PLANESWALKER");
      iVar3 = dialog_impl(player, card, EVENT_ACTIVATE, "\001\002", "\001\016", "Add energy", iVar3,
                          1, "Deal 2 damage to a player or planeswalker", iVar1, 5, 0, 0, 0, 0);
      if (iVar3 == 0)
      {
        spell_fizzled = 1;
        return 0;
      }
      instance = get_card_instance(player, card);
      if (instance->info_slot == 1)
      {
        prompt = NULL;
        td = NULL;
        variable_costs = 0;
        iVar3 = 1;
      }
      else
      {
        instance = get_card_instance(player, card);
        if (instance->info_slot != 2)
        {
          return 0;
        }
        prompt = "TARGET_PLAYER_OR_PLANESWALKER";
        variable_costs = GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 3);
        iVar3 = 5;
      }
      iVar3 = generic_activated_ability(player, card, EVENT_ACTIVATE, iVar3, 0, 0, 0, 0, 0, 0,
                                             (uint32_t)variable_costs, td, prompt);
      return iVar3;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td1, 0x100);
  }
  td1.zone = 0x1000;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    instance = get_card_instance(player, card);
    if (instance->info_slot == 1)
    {
      add_energy(player, 1);
    }
    instance = get_card_instance(player, card);
    if ((instance->info_slot == 2) && (iVar3 = valid_target(&td1), iVar3 != 0))
    {
      damage_target0(player, card, 2);
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x20c1230; original cards/aether_revolt.c:1841. */
int card_glint_sleeve_siphoner(int player, int card, event_t event)
{
  int result;
  resolve_trigger_t trigger_mode;
  etb_energy(player, card, event, 1);
  menace(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    result = duh_mode(player);
    upkeep_trigger_ability_mode(player, card, event, player,
                                (result != 0) + RESOLVE_TRIGGER_OPTIONAL);
    if ((event == EVENT_UPKEEP_TRIGGER_ABILITY) && (1 < ENERGY_COUNTERS(player)))
    {
      result = is_humiliated_for_ability(player, card, AL_TRIG);
      if (result == 0)
      {
        remove_energy(player, 2);
        draw_a_card(player);
        lose_life(player, 1);
      }
    }
    return 0;
  }
  add_energy(player, 1);
  result = duh_mode(player);
  if (result == 0)
  {
    trigger_mode = RESOLVE_TRIGGER_OPTIONAL;
  }
  else
  {
    trigger_mode = RESOLVE_TRIGGER_MANDATORY;
  }
  upkeep_trigger_ability_mode(player, card, EVENT_DAT_ABILITY, player, trigger_mode);
  return 0;
}


/* ManalinkEh 2022: 0x20c14d0; original cards/aether_revolt.c:1895. */
int card_gontis_machinations(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if (event == EVENT_CLEANUP)
  {
    instance->targets[1].player = 0;
  }
  else
  {
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
    {
      if (event != EVENT_RESOLVE_ACTIVATION)
        goto LAB_020c1525;
    }
    else if (event != EVENT_RESOLVE_ACTIVATION)
    {
      result = global_enchantment(player, card, event);
      return result;
    }
    result = lose_life(1 - player, 3);
    gain_life(player, result);
  }
LAB_020c1525:
  result = generic_activated_ability(player, card, event, 8, 0, 0, 0, 0, 0, 0,
                                          GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 2), NULL, NULL);
  return result;
}


/* ManalinkEh 2022: 0x20c15d0; original cards/aether_revolt.c:1919. */
int card_greenbelt_rampager(int player, int card, event_t event)
{
  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  if (1 < ENERGY_COUNTERS(player))
  {
    remove_energy(player, 2);
    return 0;
  }
  add_energy(player, 1);
  bounce_permanent(player, card);
  return 0;
}


/* ManalinkEh 2022: 0x20c4c70; original cards/aether_revolt.c:2960. */
int card_lightning_runner(int player, int card, event_t event)
{
  int result;
  haste(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {
    add_energy(player, 2);
    if (7 < ENERGY_COUNTERS(player))
    {
      result = do_dialog(player, player, card, -1, -1, " Attack again\n Pass", 0);
      if (result == 0)
      {
        remove_energy(player, 8);
        extra_combat_effect(player, card, EVENT_DAT_ABILITY, 1, NULL, 1);
      }
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x20c67a0; original cards/aether_revolt.c:3581. */
int card_peema_aether_seer(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_2022_t td;

  result = get_highest_power(player);
  etb_energy(player, card, event, result);
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
    if (event == EVENT_ACTIVATE)
    {
      if (((card == affected_card) && (player == affected_card_controller)) &&
          ((current_turn == player || (0x19 < current_phase))))
      {
        ai_modifier = ai_modifier + -0x60;
      }
      if ((player == 1) && (spell_fizzled != 1))
      {
        generic_activated_ability(1, card, EVENT_ACTIVATE, 6, 0, 0, 0, 0, 0, 0,
                                       GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 3), &td,
                                       "Select target creature.");
        if (spell_fizzled == 1)
        {
          return 0;
        }
        ai_modifier_twiddle(1, card, 0);
        return 0;
      }
      goto LAB_020c6834;
    }
  }
  else
  {
    if ((event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = 1 - player;
  }
  if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
  {
    instance = get_card_instance(player, card);
    pump_ability_until_eot(player, card, instance->targets[0].player,
                                instance->targets[0].card, 0, 0, 0, 0x4000000);
  }
LAB_020c6834:
  result = generic_activated_ability(player, card, event, 6, 0, 0, 0, 0, 0, 0,
                                          GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 3), &td,
                                          "Select target creature.");
  return result;
}


/* ManalinkEh 2022: 0x20c9770; original cards/aether_revolt.c:4793. */
int card_shielded_aether_thief(int player, int card, event_t event)
{
  int result;
  if (event != EVENT_ACTIVATE)
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      result = is_humiliated_for_ability(player, card, AL_TRIG);
      if (result == 0)
      {
        result = blocking(player, card, event);
        if (result != 0)
        {
          add_energy(player, 1);
        }
      }
      result = flash(player, card, event);
      return result;
    }
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      draw_cards(player, 1);
    }
  }
  result = generic_activated_ability(player, card, event, 1, 0, 0, 0, 0, 0, 0,
                                          GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 3), NULL, NULL);
  return result;
}


/* ManalinkEh 2022: 0x20c9880; original cards/aether_revolt.c:4815. */
int card_shipwreck_moray(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  etb_energy(player, card, event, 4);
  if (event - EVENT_POW_BOOST < 2)
  {
    if (((ENERGY_COUNTERS(player) != '\0') && (current_turn == player)) &&
        (result = is_attacking(player, card), result != 0))
    {
      result = generic_shade(player, card, event, 0, 0, 0, 0, 0, 0, 0, 2, -2, 0, 0);
      return result;
    }
    return 0;
  }
  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_020c98c0:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_020c98c5;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_020c98c0;
  }
  instance = get_card_instance(player, card);
  result = instance->parent_card;
  instance = get_card_instance(player, card);
  pump_until_eot_merge_previous(player, card, instance->parent_controller, result, 2, -2);
LAB_020c98c5:
  result = generic_activated_ability(player, card, event, 0, 0, 0, 0, 0, 0, 0,
                                          GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 1), NULL, NULL);
  return result;
}


/* ManalinkEh 2022: 0x20cae10; original cards/aether_revolt.c:5336. */
int card_tezzerets_simulacrum(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  target_definition_2022_t td;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    if (event == EVENT_RESOLVE_ACTIVATION)
    {
      iVar3 = active_cards_count[player] + -1;
      if (-1 < iVar3)
      {
        do
        {
          instance = in_play(player, iVar3);
          if (((instance != NULL) && (iVar2 = is_planeswalker(player, iVar3), iVar2 != 0)) &&
              (iVar2 = has_subtype(player, iVar3, SUBTYPE_TEZZERET), iVar2 != 0))
          {
            lose_life(1 - player, 3);
            goto LAB_020caee0;
          }
          iVar3 = iVar3 + -1;
        } while (iVar3 != -1);
      }
      lose_life(1 - player, 1);
    }
  }
LAB_020caee0:
  iVar3 = generic_activated_ability(player, card, event,
                                         (GAA_UNTAPPED | GAA_CAN_ONLY_TARGET_OPPONENT), 0, 0, 0, 0,
                                         0, 0, 0, &td, "TARGET_PLAYER");
  return iVar3;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x20c1380; original cards/aether_revolt.c:1871. */
int card_gontis_aether_heart(int player, int card, event_t event)
{
  int result;
  test_definition_2022_t test;
  check_legend_rule(player, card, event);
  if (((trigger_condition == 0xdb) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (result == 0)
    {
      default_test_definition(&test, 0x40);
      result = new_specific_cip(player, card, event, player, 2, &test);
      if (result != 0)
      {
        add_energy(player, result * 2);
      }
    }
  }
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    time_walk_effect(player, card);
  }
  result = generic_activated_ability(player, card, event, 0x4000, 0, 0, 0, 0, 0, 0,
                                          GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 8), NULL, NULL);
  return result;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20c0880; original cards/aether_revolt.c:1612. */
int card_exquisite_archangel(int player, int card, event_t event)
{
  bool bVar1;
  card_instance_t *pcVar2;
  int amount;
  int iVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int iVar6;

  pcVar2 = get_card_instance(player, card);
  cannot_lose_the_game(player, card, event, player);
  amount = get_starting_life_total(player);
  if (((0 < pcVar2->targets[5].player) && (iVar6 = can_lose_the_game(player), iVar6 != 0)) &&
      (iVar3 = is_humiliated_for_ability(player, card, AL_STATIC), iVar6 = current_turn,
       iVar3 == 0))
  {

    iVar3 = active_cards_count[current_turn] + -1;
    bVar1 = false;
    if (iVar3 < 0)
    {
      iVar3 = 1 - current_turn;
      iVar6 = active_cards_count[iVar3] + -1;
      if (-1 < iVar6)
        goto LAB_020c0af3;
    }
    else
    {
      do
      {
        pcVar4 = in_play(iVar6, iVar3);

        if (((pcVar4 != NULL) && (iVar5 = is_what(iVar6, iVar3, 0x80), iVar5 != 0)) &&
            ((pcVar4 = get_card_instance(player, iVar3),
              pcVar4->internal_card_id == damage_card && (pcVar4->damage_target_player == player))))
        {
          bVar1 = true;
        }
        iVar3 = iVar3 + -1;
      } while (iVar3 != -1);
      iVar3 = 1 - current_turn;
      iVar6 = active_cards_count[iVar3];
      while (iVar6 = iVar6 + -1, -1 < iVar6)
      {
      LAB_020c0af3:
        pcVar4 = in_play(iVar3, iVar6);

        if (((pcVar4 != NULL) && (iVar5 = is_what(iVar3, iVar6, 0x80), iVar5 != 0)) &&
            ((pcVar4 = get_card_instance(player, iVar6),
              pcVar4->internal_card_id == damage_card && (pcVar4->damage_target_player == player))))
        {
          bVar1 = true;
        }
      }
      if (bVar1)
        goto LAB_020c08cc;
    }
    pcVar2->targets[5].player = 0;
    set_life_total(player, amount);
    kill_card(player, card, KILL_EXILE);
  }
LAB_020c08cc:
  if (((0 < pcVar2->targets[5].card) && (iVar6 = can_lose_the_game(player), iVar6 != 0)) &&
      (iVar6 = is_humiliated_for_ability(player, card, AL_STATIC), iVar6 == 0))
  {
    pcVar2->targets[5].card = 0;
    set_life_total(player, amount);
    kill_card(player, card, KILL_EXILE);
  }
  if (((((trigger_condition == 0xcf) && (*deck_ptr[player] == -1)) &&
        ((reason_for_trigger_controller == player &&
          ((card == affected_card && (player == affected_card_controller)))))) &&
       (suppress_draw == 0)) &&
      ((iVar6 = can_lose_the_game(player),
        iVar6 != 0 && (iVar6 = is_humiliated_for_ability(player, card, AL_TRIG), iVar6 == 0))))
  {
    if (event == EVENT_TRIGGER)
    {
      event_result = 2;
    }
    else if (event != EVENT_RESOLVE_TRIGGER)
    {
      return 0;
    }
    suppress_draw = 1;
    set_life_total(player, amount);
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

#endif

#ifndef SHANDALAR
/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x20c4400; original cards/aether_revolt.c:2809. */
int card_kari_zev_skyship_raider(int player, int card, event_t event)
{
  token_generation_2022_t token;

  check_legend_rule(player, card, event);
  menace(player, card, event);
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  default_token_definition(player, card, 0x4975, &token);
  token.pow = 2;
  token.tou = 1;
  token.color_forced = '\x10';
  token.legacy = '\x01';
  token.special_code_for_legacy = exile_at_end_of_combat_2022;
  token.action = TOKEN2022_ACTION_ATTACKING;
  generate_token(&token);
  return 0;
}

/* ManalinkEh 2022: 0x20c5540; original cards/aether_revolt.c:3128. */
int card_metallic_mimic(int player, int card, event_t event)
{
  unsigned int new_type;
  int result;
  card_instance_t *instance;
  test_definition_2022_t test;

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
  else if (event == EVENT_CHANGE_TYPE)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      instance = get_card_instance(player, card);
      add_a_subtype(player, card, instance->info_slot);
    }
  }
  else if (((event == EVENT_CAST_SPELL) || (event == EVENT_ENTERING_THE_BATTLEFIELD_AS_CLONE)) &&
           (result = is_humiliated_for_ability(player, card, AL_STATIC), result == 0))
  {

    instance = get_card_instance(player, card);
    new_type = instance->info_slot;
    result = is_creature_type(new_type);
    if (result != 0)
    {
      default_test_definition(&test, 2);
      test.not_me = 1;
      if ((affected_card_controller == player) &&
          (test.subtype = new_type,
           result = new_make_test_in_play(player, affected_card, -1, &test), result != 0))
      {
        enters_the_battlefield_with_counters(affected_card_controller, affected_card, event,
                                             COUNTER_P1_P1, 1);
      }
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x20c3280; original cards/aether_revolt.c:2475. */
int card_indomitable_creativity(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int iVar6;

  card_instance_t *pcStack_f0;
  target_definition_2022_t td;
  test_definition_2022_t reveal_test;

  pcVar4 = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_GENERAL_EFFECT)
  {
    default_test_definition(&reveal_test, 0x42);
    uint32_t counts = (uint32_t)pcVar4->targets[10].player;
    unsigned int first_count = counts & 0xffffU;
    unsigned int second_count = counts >> 16;
    if (first_count != 0)
    {
      for (unsigned int i = 0; i != first_count; ++i)
        reveal_card_until_you_reveal_test(player, player, &reveal_test, 0x144);
      shuffle(player);
    }
    if ((int)second_count < 1)
      return 0;
    for (unsigned int i = 0; i != second_count; ++i)
      reveal_card_until_you_reveal_test(player, 1 - player, &reveal_test, 0x144);
    shuffle(1 - player);
    return 0;
  }
  if (event == EVENT_CAN_CAST)
  {

    default_target_definition(player, card, &td, 0x42);
    td.allow_cancel = 3;

    td.illegal_abilities = get_protections_from(player, card);
    if (player != 1)
      goto LAB_020c366f;
  LAB_020c3430:
    td.preferred_controller = 1;
    td.special = 0x8000;
    td.extra = (int32_t)not_indestructible;
    if (event == EVENT_CAN_CAST)
    {
    LAB_020c366f:

      iVar5 = generic_spell(player, card, EVENT_CAN_CAST, 5, &td, NULL, 1, NULL);
      return iVar5;
    }
    if (event == EVENT_CAST_SPELL)
      goto LAB_020c3462;
  LAB_020c3323:
    if (event == EVENT_CHANGE_TARGET)
      goto LAB_020c3330;
  LAB_020c34ed:
    if (event == EVENT_RESOLVE_SPELL)
    {
      pcStack_f0 = NULL;
      if (pcVar4->number_of_targets != '\0')
      {
        iVar5 = 0;
        do
        {
          while (true)
          {

            iVar6 = validate_target(player, card, &td, iVar5);
            if (iVar6 == 0)
              break;

            pcStack_f0 = legacy_permanents_destroyed_this_way_accumulate(
                player, card, pcStack_f0, pcVar4->targets[iVar5].player,
                pcVar4->targets[iVar5].card);
            ptVar3 = pcVar4->targets + iVar5;
            ptVar1 = pcVar4->targets + iVar5;
            iVar5 = iVar5 + 1;

            kill_card(ptVar1->player, ptVar3->card, KILL_DESTROY);
            if ((int)(unsigned int)pcVar4->number_of_targets <= iVar5)
              goto LAB_020c3598;
          }
          iVar5 = iVar5 + 1;
        } while (iVar5 < (int)(unsigned int)pcVar4->number_of_targets);
      }
    LAB_020c3598:

      kill_card(player, card, KILL_DESTROY);
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

      default_target_definition(player, card, &td, 0x42);
      td.allow_cancel = 3;

      td.illegal_abilities = get_protections_from(player, card);
      if (player == 1)
        goto LAB_020c3430;
    LAB_020c3462:
      if (card != affected_card)
      {
        return 0;
      }
      if (player != affected_card_controller)
      {
        return 0;
      }

      iVar5 = check_special_flags2(player, card, 0x2000);
      if (iVar5 == 0)
      {

        iVar5 = check_special_flags2(player, card, 0x8000);
        if (iVar5 == 0)
          goto LAB_020c379f;
      }

      set_special_flags2(player, card, 0x20000);

      iVar5 = played_for_free(player, card);
      if (iVar5 != 0)
      {
        return 0;
      }
    LAB_020c34c9:

      iVar5 = is_token(player, card);
      if (iVar5 == 0)
      {

        iVar5 = has_mana(player, 7, 1);

        iVar6 = target_available(-1, card, &td);
        if (iVar6 < iVar5)
        {
          iVar5 = iVar6;
        }

        iVar5 = pick_up_to_n_targets(&td, "PUTREFY", iVar5);
        if (iVar5 == 0)
        {
          spell_fizzled = 1;
          return 0;
        }
        pcVar4->info_slot = 0;
        if (player == 0)
        {

          iVar5 = charge_mana(0, COLOR_ANY, iVar5);

          set_x_for_x_spells(0, card, event, iVar5);
        }
        else
        {

          iVar5 = target_available(player, card, &td);
          while (true)
          {

            iVar6 = has_mana(player, 7, iVar5);
            if (iVar6 != 0)
              break;
            iVar5 = iVar5 + -1;
          }
          if (iVar5 == 0)
          {
            ai_modifier = ai_modifier + -0x32;
          }

          charge_mana(player, COLOR_ANY, iVar5);
          if (spell_fizzled != 1)
          {

            set_x_for_x_spells(player, card, event, iVar5);
          }
        }
      }
    LAB_020c34e0:
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_020c34ed;
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {

        default_target_definition(player, card, &td, 0x42);
        td.allow_cancel = 3;

        td.illegal_abilities = get_protections_from(player, card);
        if (player == 1)
          goto LAB_020c3430;
        goto LAB_020c3323;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }

      default_target_definition(player, card, &td, 0x42);
      td.allow_cancel = 3;

      td.illegal_abilities = get_protections_from(player, card);
      if (player == 1)
        goto LAB_020c3430;
    LAB_020c3330:

      iVar5 = check_special_flags2(player, card, 0x10000);
      if (iVar5 != 0)
      {
        int target_count = pcVar4->number_of_targets;
        int t_array[target_count];
        int shown = 0;
        while (shown < pcVar4->number_of_targets)
        {
          int target_card = pcVar4->targets[shown].card;
          int target_player = pcVar4->targets[shown].player;
          ++shown;
          if (target_card == -1)
            t_array[shown - 1] =
                get_internal_card_id_from_csv_id(player == target_player ? 0x615a : 0x6161);
          else
            t_array[shown - 1] = get_card_instance(target_player, target_card)->internal_card_id;
        }
        return show_deck(
            player, t_array, shown,
            " Select the target to change (Rules Engine = you, Deadbox = opponent)", 0,
            ENGINE_DONE_LABEL);
      }

      iVar5 = check_special_flags2(player, card, 0x2000);
      if (iVar5 == 0)
      {

        iVar5 = check_special_flags2(player, card, 0x8000);
        if (iVar5 != 0)
          goto LAB_020c336e;
      LAB_020c379f:

        set_special_flags2(player, card, 0x20000);

        iVar5 = played_for_free(player, card);
        if (iVar5 == 0)
          goto LAB_020c34c9;
        goto LAB_020c34e0;
      }
    LAB_020c336e:
      td.allow_cancel = 0;
      td.who_chooses = attacking_card_controller;

      set_special_flags2(player, card, 0x20000);

      iVar5 = played_for_free(player, card);
      if (iVar5 == 0)
        goto LAB_020c34c9;
    }
    iVar5 = pcVar4->targets[0].card;
    if (iVar5 != -1)
    {

      state_untargettable(pcVar4->targets[0].player, iVar5, 0);
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x20bbea0; original cards/aether_revolt.c:160. */
static int cast_spell_with_improvise_2022_impl(int player, int card, event_t event)
{
  card_ptr_t *pcVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  card_instance_t *instance;
  card_data_t *cd;
  int *piVar6;
  bool bVar7;
  int iStack_b4;
  int iStack_b0;
  int art_to_tap[15];
  target_definition_2022_t td;

  iVar2 = is_x_spell(player, card);
  if (iVar2 == 0)
  {
    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    td.illegal_state = 1;
    td.illegal_abilities = 0;
    td.allow_cancel = 3;
    iStack_b4 = get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, -1);
    if (iStack_b4 == 0)
    {
      get_id(player, card);
      get_card_instance(player, card);
      goto LAB_020bbf5d;
    }
  }
  else
  {
    set_special_flags2(player, card, 0x20000);
    default_target_definition(player, card, &td, 0x40);
    td.allowed_controller = player;
    td.illegal_state = 1;
    td.illegal_abilities = 0;
    td.allow_cancel = 3;
    iStack_b4 = get_updated_casting_cost(player, card, -1, EVENT_CAST_SPELL, -1);
  }
  iVar3 = do_dialog(player, player, card, -1, -1, " Use Improvise\n Play normally\n Cancel", 0);
  if (iVar3 == 2)
  {
    spell_fizzled = 1;
    return 0;
  }
  iVar4 = get_id(player, card);
  pcVar1 = cards_ptr[iVar4];
  instance = get_card_instance(player, card);
  if (iVar3 == 0)
  {
    iStack_b0 = target_available(player, card, &td);
    if (iStack_b4 < iStack_b0)
    {
      iStack_b0 = iStack_b4;
    }
    bVar7 = iVar2 != 0;
    if ((0 < iStack_b0) || (bVar7))
    {
      iVar3 = 0;
      do
      {
        iVar4 = can_target(&td);
        if ((iVar4 == 0) ||
            (iVar4 = new_pick_target(&td, "Select an artifact to tap for improvise.", 0, 0x80),
             iVar4 == 0))
        {
          bVar7 = iStack_b4 <= iVar3 && bVar7;
          iStack_b0 = iVar3;
          if ((iVar2 == 0) || (iVar3 < iStack_b4))
            goto LAB_020bc39c;
          goto LAB_020bc196;
        }
        iVar4 = instance->targets[0].card;
        art_to_tap[iVar3] = iVar4;
        iVar3 = iVar3 + 1;
        state_untargettable(instance->targets[0].player, iVar4, 1);
        instance->number_of_targets = '\0';
      } while (iVar3 < iStack_b0 || bVar7);
      bVar7 = false;
      iStack_b0 = iVar3;
    LAB_020bc39c:
      if (spell_fizzled != 1)
      {
        charge_mana_multi(player, iStack_b4 - iStack_b0, (unsigned int)pcVar1->req_black,
                          (unsigned int)pcVar1->req_blue, (unsigned int)pcVar1->req_green,
                          (unsigned int)pcVar1->req_red, (unsigned int)pcVar1->req_white);
        if (iVar2 != 0)
        {
          charge_mana(player, COLOR_COLORLESS, -1);
          set_x_for_x_spells(player, card, event, x_value);
        }
      LAB_020bc196:
        if ((bVar7) && (spell_fizzled != 1))
        {
          charge_mana_multi(player, -1, (unsigned int)pcVar1->req_black,
                            (unsigned int)pcVar1->req_blue, (unsigned int)pcVar1->req_green,
                            (unsigned int)pcVar1->req_red, (unsigned int)pcVar1->req_white);
          x_value = (iStack_b0 - iStack_b4) + x_value;
          set_x_for_x_spells(player, card, event, x_value);
        }
      }
    }
    else
    {
      if (spell_fizzled == 1)
      {
        instance->number_of_targets = '\0';
        return 0;
      }
      charge_mana_multi(player, iStack_b4, (unsigned int)pcVar1->req_black,
                        (unsigned int)pcVar1->req_blue, (unsigned int)pcVar1->req_green,
                        (unsigned int)pcVar1->req_red, (unsigned int)pcVar1->req_white);
      iStack_b0 = 0;
    }
    instance->number_of_targets = '\0';
    if (0 < iStack_b0)
    {
      piVar6 = art_to_tap;
      do
      {
        iVar2 = *piVar6;
        state_untargettable(player, iVar2, 0);
        if (spell_fizzled != 1)
        {
          tap_card(player, iVar2);
        }
        piVar6 = piVar6 + 1;
      } while (piVar6 != art_to_tap + iStack_b0);
    }
    if (spell_fizzled == 1)
    {
      return 0;
    }
    set_special_flags3(player, card, 0x200000);
    return iStack_b0;
  }
  if (iVar3 != 1)
  {
    return 0;
  }
  if (iVar2 != 0)
  {
    if (iStack_b4 == 0)
    {
      cd = get_card_data(player, card);
      charge_spell_cost_2022(cd, player, card);
    }
    else
    {
      charge_mana_multi(player, iStack_b4, (unsigned int)pcVar1->req_black,
                        (unsigned int)pcVar1->req_blue, (unsigned int)pcVar1->req_green,
                        (unsigned int)pcVar1->req_red, (unsigned int)pcVar1->req_white);
      charge_mana(player, COLOR_COLORLESS, -1);
    }
    set_x_for_x_spells(player, card, event, x_value);
  }
LAB_020bbf5d:
  return (unsigned int)(spell_fizzled != 1);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x20bc670; original cards/aether_revolt.c:262. */
int improvise(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_MODIFY_COST)
  {
    modify_cost_for_improvise(player, card, EVENT_MODIFY_COST);
    return 0;
  }
  if ((((event == EVENT_CAST_SPELL) && (card == affected_card)) &&
       (player == affected_card_controller)) &&
      ((result = played_for_free(player, card),
        result == 0 && (result = is_token(player, card), result == 0))))
  {
    result = check_special_flags3(player, card, 0x200000);
    if ((result == 0) &&
        (result = cast_spell_with_improvise_2022_impl(player, card, EVENT_CAST_SPELL), result != 0))
    {
      return 0;
    }
    spell_fizzled = 1;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x20c3ac0; original cards/aether_revolt.c:2653. */
int card_inspiring_statuary(int player, int card, event_t event)
{
  int iVar1;
  int iVar2;
  if (event == EVENT_MODIFY_COST_GLOBAL)
  {
    iVar1 = is_humiliated_for_ability(player, card, AL_STATIC);
    if ((iVar1 == 0) && (affected_card_controller == player))
    {
      iVar1 = is_what(player, affected_card, 0x40);
      if (iVar1 == 0)
      {
        modify_cost_for_improvise(affected_card_controller, affected_card,
                                  EVENT_MODIFY_COST_GLOBAL);
      }
    }
  }
  else if ((event == EVENT_CAST_SPELL) && (affected_card_controller == player))
  {
    iVar1 = is_what(player, affected_card, 0x41);
    if (iVar1 == 0)
    {
      iVar1 = played_for_free(affected_card_controller, affected_card);
      if (iVar1 == 0)
      {
        iVar2 = is_token(affected_card_controller, affected_card);
        iVar1 = affected_card;
        if (iVar2 == 0)
        {
          iVar2 = check_special_flags3(player, affected_card, 0x200000);
          if (iVar2 == 0)
          {
            cast_spell_with_improvise_2022_impl(player, iVar1, EVENT_CAST_SPELL);
          }
        }
      }
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x20bc560; original cards/aether_revolt.c:106. */
static void av_free_spell(int player, int max_cmc)
{
  int card;
  test_definition_2022_t test;

  default_test_definition(&test, 1);
  test.type_flag = 1;
  test.cmc_flag = 3;
  test.cmc = max_cmc + 1;
  card = new_select_a_card(player, player, 4, 0, 1, -1, &test);
  if (-1 < card)
  {
    play_card_in_hand_for_free(player, card);
  }
  return;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x20c9410; original cards/aether_revolt.c:4726. */
int card_secret_salvage(int player, int card, event_t event)
{
  unsigned short uVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  char msg[100];
  test_definition_2022_t this_test;
  test_definition_2022_t test;
  if (event == EVENT_CAN_CAST)
  {
    memset(msg, 0, sizeof msg);
    memcpy(msg, "Select a nonland card to exile.", 32);
    new_default_test_definition(&this_test, 0x7e, msg);
    iVar4 = any_in_graveyard_by_type(player, 0x7e);
    return iVar4;
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
  else
  {
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      memset(msg, 0, sizeof msg);
      memcpy(msg, "Select a nonland card to exile.", 32);
      new_default_test_definition(&this_test, 0x7e, msg);
      if (event == EVENT_RESOLVE_SPELL)
      {
        iVar4 = validate_target_from_grave_source(player, card, player, 0);
        if (iVar4 == -1)
        {
          spell_fizzled = 1;
          return 0;
        }
        piVar2 = graveyard_ptr_mutable[player];
        uVar1 = cards_data[piVar2[iVar4]].id;
        default_test_definition(&test, 0xff);
        test.id = (unsigned int)uVar1;
        scnprintf(test.message, 100, "Select a card named %s.", cards_data[piVar2[iVar4]].name);
        test.no_shuffle = 1;
        do
        {
          iVar3 = new_global_tutor(player, player, 1, 1, 0, 1, &test);
        } while (-1 < iVar3);
        shuffle(player);
        rfg_card_from_grave(player, iVar4);
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_020c95f8;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  memset(msg, 0, sizeof msg);
  memcpy(msg, "Select a nonland card to exile.", 32);
  new_default_test_definition(&this_test, 0x7e, msg);
LAB_020c95f8:
  iVar4 = generic_spell(player, card, event, 8, NULL, NULL, 0, &this_test);
  return iVar4;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x20c75a0; original cards/aether_revolt.c:4026. */
int card_release_the_gremlins(int player, int card, event_t event)
{
  target_t *ptVar1;
  int iVar2;
  target_t *ptVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int iVar6;
  unsigned int uVar7;
  card_instance_t *pcVar8;
  int iVar9;
  card_instance_t *pcStack_b8;
  target_definition_2022_t *ptStack_b4;
  token_generation_2022_t token;
  target_definition_2022_t td;

  if (event == EVENT_CAN_CAST)
  {
    ptStack_b4 = &td;
    default_target_definition(player, card, ptStack_b4, 0x40);
    td.allow_cancel = 3;
    if (player != 1)
    {
      get_card_instance(player, card);
      goto LAB_020c7a7f;
    }
  LAB_020c797d:
    td.allow_cancel = 3;
    td.illegal_abilities = get_protections_from(1, card);
    td.special = 0x8000;
    td.extra = (int32_t)not_indestructible;
    pcVar4 = get_card_instance(player, card);
    if (event == EVENT_CAN_CAST)
    {
    LAB_020c7a7f:
      iVar5 = generic_spell(player, card, EVENT_CAN_CAST, 4, ptStack_b4, NULL, 1, NULL);
      return iVar5;
    }
    if (event == EVENT_CAST_SPELL)
      goto LAB_020c79c3;
  LAB_020c761c:
    if (event == EVENT_CHANGE_TARGET)
      goto LAB_020c7629;
  LAB_020c7810:
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar5 = 0;
      if (pcVar4->number_of_targets != '\0')
      {
        do
        {
          while (true)
          {
            iVar6 = validate_target(player, card, ptStack_b4, iVar5);
            if (iVar6 == 0)
              break;
            ptVar3 = pcVar4->targets + iVar5;
            ptVar1 = pcVar4->targets + iVar5;
            iVar5 = iVar5 + 1;
            kill_card(ptVar1->player, ptVar3->card, KILL_DESTROY);
            if ((int)(unsigned int)pcVar4->number_of_targets <= iVar5)
              goto LAB_020c7879;
          }
          iVar5 = iVar5 + 1;
        } while (iVar5 < (int)(unsigned int)pcVar4->number_of_targets);
      }
    LAB_020c7879:
      default_token_definition(player, card, 0x4942, &token);
      token.qty = pcVar4->number_of_targets;
      token.tou = 2;
      token.pow = 2;
      token.color_forced = '\x10';
      generate_token(&token);
      kill_card(player, card, KILL_DESTROY);
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
      ptStack_b4 = &td;
      default_target_definition(player, card, ptStack_b4, 0x40);
      td.allow_cancel = 3;
      if (player == 1)
        goto LAB_020c797d;
      pcVar4 = get_card_instance(player, card);
    LAB_020c79c3:
      if (card != affected_card)
      {
        return 0;
      }
      if (player != affected_card_controller)
      {
        return 0;
      }
      iVar5 = check_special_flags2(player, card, 0x2000);
      if (iVar5 == 0)
      {
        iVar5 = check_special_flags2(player, card, 0x8000);
        if (iVar5 == 0)
          goto LAB_020c778f;
      }
      set_special_flags2(player, card, 0x20000);
      iVar5 = played_for_free(player, card);
      if (iVar5 != 0)
      {
        iVar5 = pcVar4->info_slot;
        iVar6 = target_available(-1, card, ptStack_b4);
        if (iVar6 < iVar5)
        {
          iVar5 = iVar6;
        }
        pick_up_to_n_targets(ptStack_b4, "TARGET_ARTIFACT", iVar5);
        return 0;
      }
      iVar5 = is_token(player, card);
      if (iVar5 != 0)
      {
        iVar5 = pcVar4->info_slot;
        iVar6 = target_available(-1, card, ptStack_b4);
        if (iVar5 < iVar6)
        {
          iVar6 = iVar5;
        }
        pick_up_to_n_targets(ptStack_b4, "TARGET_ARTIFACT", iVar6);
        return 0;
      }
    LAB_020c7b5f:
      pcVar4->info_slot = 0;
      if (player == 0)
      {
        iVar5 = charge_mana_for_double_x(0, 7);
        set_x_for_x_spells(0, card, event, iVar5 / 2);
      }
      else
      {
        iVar5 = target_available(1 - player, card, ptStack_b4);
        iVar5 = iVar5 * 2;
        pcStack_b8 = pcVar4;
        while (true)
        {
          iVar6 = has_mana(player, 7, iVar5);
          pcVar4 = pcStack_b8;
          if (iVar6 != 0)
            break;
          iVar5 = iVar5 + -2;
        }
        if (iVar5 == 0)
        {
          ai_modifier = ai_modifier + -0x32;
        }
        charge_mana(player, COLOR_ANY, iVar5);
        if (spell_fizzled != 1)
        {
          set_x_for_x_spells(player, card, event, iVar5);
        }
      }
    LAB_020c77bd:
      iVar5 = pcVar4->info_slot;
      iVar6 = target_available(-1, card, ptStack_b4);
      if (iVar6 < iVar5)
      {
        iVar5 = iVar6;
      }
      pick_up_to_n_targets(ptStack_b4, "TARGET_ARTIFACT", iVar5);
      if (event != EVENT_CHANGE_TARGET)
        goto LAB_020c7810;
    }
    else
    {
      if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
      {
        ptStack_b4 = &td;
        default_target_definition(player, card, ptStack_b4, 0x40);
        td.allow_cancel = 3;
        if (player == 1)
          goto LAB_020c797d;
        pcVar4 = get_card_instance(player, card);
        goto LAB_020c761c;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
      ptStack_b4 = &td;
      default_target_definition(player, card, ptStack_b4, 0x40);
      td.allow_cancel = 3;
      if (player == 1)
        goto LAB_020c797d;
      pcVar4 = get_card_instance(player, card);
    LAB_020c7629:
      iVar5 = check_special_flags2(player, card, 0x10000);
      if (iVar5 != 0)
      {
        int count = (unsigned int)pcVar4->number_of_targets;
        int t_array[count];
        int tac = 0;
        if (pcVar4->number_of_targets != 0)
        {
          do
          {
            int c = pcVar4->targets[tac].card;
            int p = pcVar4->targets[tac].player;
            ++tac;
            if (c == -1)
              t_array[tac - 1] = get_internal_card_id_from_csv_id(p == player ? 0x615a : 0x6161);
            else
              t_array[tac - 1] = get_card_instance(p, c)->internal_card_id;
          } while (tac < (unsigned int)pcVar4->number_of_targets);
        }
        return show_deck(
            player, t_array, tac,
            " Select the target to change (Rules Engine = you, Deadbox = opponent)", 0,
            ENGINE_DONE_LABEL);
      }
      iVar5 = check_special_flags2(player, card, 0x2000);
      if (iVar5 == 0)
      {
        iVar5 = check_special_flags2(player, card, 0x8000);
        if (iVar5 != 0)
          goto LAB_020c7667;
      LAB_020c778f:
        set_special_flags2(player, card, 0x20000);
        iVar5 = played_for_free(player, card);
        if (iVar5 == 0)
          goto LAB_020c7b48;
        goto LAB_020c77bd;
      }
    LAB_020c7667:
      td.allow_cancel = 0;
      td.who_chooses = attacking_card_controller;
      set_special_flags2(player, card, 0x20000);
      iVar5 = played_for_free(player, card);
      if (iVar5 == 0)
      {
      LAB_020c7b48:
        iVar5 = is_token(player, card);
        if (iVar5 == 0)
          goto LAB_020c7b5f;
        goto LAB_020c77bd;
      }
      iVar5 = pcVar4->info_slot;
      iVar6 = target_available(-1, card, ptStack_b4);
      if (iVar6 < iVar5)
      {
        iVar5 = iVar6;
      }
      pick_up_to_n_targets(ptStack_b4, "TARGET_ARTIFACT", iVar5);
    }
    iVar5 = pcVar4->targets[0].card;
    if (iVar5 != -1)
    {
      state_untargettable(pcVar4->targets[0].player, iVar5, 0);
    }
  }
  return 0;
}

#endif
