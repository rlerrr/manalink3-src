#include "manalink.h"

// Cards - Portal
int card_balance_of_power(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t* instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			int amount = hand_count[instance->targets[0].player]-hand_count[player];
			if( amount > 0 ){
				draw_cards(player, amount);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td, NULL, 1, NULL);
}

// bee sting --> shock

int card_blaze(int player, int card, event_t event){
	/* Blaze	|X|R	0x200564c
	 * Sorcery
	 * ~ deals X damage to any target. */
	/* Volcanic Geyser	|X|R|R	0x200564c
	 * Instant
	 * ~ deals X damage to any target. */
	// Original code : 0x4049B0

	return generic_x_spell(player, card, event, TARGET_ZONE_CREATURE_OR_PLAYER, COLOR_COLORLESS, 1);
}

int card_breath_of_life(int player, int card, event_t event){
	// 0x41f9f0

	/* Breath of Life	|3|W
	 * Sorcery
	 * Return target creature card from your graveyard to the battlefield. */

	/* False Defeat	|3|W
	 * Sorcery
	 * Return target creature card from your graveyard to the battlefield. */

	/* Resurrection	|2|W|W
	 * Sorcery
	 * Return target creature card from your graveyard to the battlefield. */

	/* Zombify	|3|B
	 * Sorcery
	 * Return target creature card from your graveyard to the battlefield. */

	if (!IS_GS_EVENT(player, card, event)){
		return 0;
	}

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_CREATURE, "Select target creature card.");
	this_test.ai_selection_mode = AI_MAX_CMC;

	if ( event == EVENT_RESOLVE_SPELL ){
		int selected = validate_target_from_grave_source(player, card, player, 0);
		if( selected != -1 ){
			reanimate_permanent(player, card, player, selected, REANIMATE_DEFAULT);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_GRAVE_RECYCLER, NULL, NULL, 0, &this_test);
}

int card_charging_paladin(int player, int card, event_t event){
	/* Charging Paladin	|2|W	0x200ec79
	 * Creature - Human Knight 2/2
	 * Whenever ~ attacks, it gets +0/+3 until end of turn. */

	return when_attacks_pump_self(player, card, event, 0,3);
}

// cloud dragon --> cloud sprite

// cloud pirates --> cloud sprite

int card_command_of_unsummoning(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.required_state = TARGET_STATE_ATTACKING;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_CAN_CAST && !(current_turn != player && current_phase == PHASE_BEFORE_BLOCKING) ){
		return 0;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int i;
		for(i=0; i<instance->info_slot; i++){
			if( validate_target(player, card, &td, i) ){
				bounce_permanent(instance->targets[i].player, instance->targets[i].card);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 2, NULL);
}

int card_cruel_fate(int player, int card, event_t event){
/*
Cruel Fate English
Sorcery, 4U (5)
Look at the top five cards of target opponent's library.
Put one of those cards into that player's graveyard and the rest on top of his or her library in any order.
*/
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += 24;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			int t_player = instance->targets[0].player;
			int number = 5;
			int *deck = deck_ptr[t_player];
			if( number > count_deck(t_player) ){
				 number = count_deck(t_player);
			}
			if( number > 0 ){
				int selected = -1;
				while( selected == -1 ){
						selected = show_deck( player, deck, number, "Pick a card", 0, 0x7375B0 );
				}
				int iid = deck[selected];
				remove_card_from_deck(t_player, selected);
				raw_put_iid_on_top_of_deck(t_player, iid);
				mill(t_player, 1);
				number--;
				if( number > 0 ){
					rearrange_top_x(t_player, player, number);
				}
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td, NULL, 1, NULL);
}

int card_cruel_tutor(int player, int card, event_t event){
	// Also code for Vampiric Tutor

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (life[player] >= 8 ? 96 : 48);
	}

	if(event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_ANY, "Select a card to put on top of deck.");
		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_DECK, 0, AI_MAX_VALUE, &this_test);
		lose_life(player, 2);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

static int deep_wood_legacy(int player, int card, event_t event)
{
	card_instance_t* damage = damage_being_prevented(event);
	if (damage
		&& (damage->targets[3].player & TYPE_CREATURE)
		&& is_attacking(damage->damage_source_player, damage->damage_source_card)
		&& damage->damage_target_player == player && damage->damage_target_card == -1 && !damage_is_to_planeswalker(damage))
		prevent_damage(affected_card_controller, affected_card, -1);

	if (event == EVENT_CLEANUP)
		kill_card(player, card, KILL_EXILE);

	return 0;
}

int card_deep_wood(int player, int card, event_t event){ //also code for Heavy Fog

	// Cast ~ only during the declare attackers step and only if you've been attacked this step.
	// Prevent all damage that would be dealt to you this turn by attacking creatures.
	if( event == EVENT_CAN_CAST && current_turn != player && current_phase == PHASE_BEFORE_BLOCKING && basic_spell(player, card, event) ){
		int c, p = current_turn;
		for (c = 0; c < active_cards_count[p]; ++c){
			if (in_play_and_attacking(p, c) && !check_special_flags(p, c, SF_ATTACKING_PWALKER)){
				return 1;
			}
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		create_legacy_effect(player, card, &deep_wood_legacy);
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_deja_vu(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_SORCERY, "Select target sorcery card.");

	if( event == EVENT_RESOLVE_SPELL ){
		int selected = validate_target_from_grave_source(player, card, player, 0);
		if( selected != -1 ){
			from_grave_to_hand(player, selected, TUTOR_HAND);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_GRAVE_RECYCLER, NULL, NULL, 1, &this_test);
}

int card_devastation2(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE | TYPE_LAND);
		APNAP(p, { new_manipulate_all(player, card, p, &this_test, KILL_DESTROY); };);
		kill_card(player, card, KILL_DESTROY);
	}


	return basic_spell(player, card, event);
}

int card_dread_reaper(int player, int card, event_t event){

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (life[player]-5 < 6 ? -48 : 0);
	}

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		lose_life(player, 5);
	}

	return 0;
}

int card_ebon_dragon(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_AI(player));

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, 0);
		td.zone = TARGET_ZONE_PLAYERS;

		if( would_validate_arbitrary_target(&td, 1-player, -1) ){
			discard(1-player, 0, player);
		}
	}

	return 0;
}

int card_endless_cockroaches(int player, int card, event_t event)
{
	// Used externally
	int owner, position;
	if (this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY) && find_in_owners_graveyard(player, card, &owner, &position))
		{
			int iid = get_grave(owner)[position];
			obliterate_card_in_grave(owner, position);
			add_card_to_hand(owner, iid);
		}

	return 0;
}

int card_final_strike(int player, int card, event_t event){
	/* Final Strike	|2|B|B	0x2009607
	 * Sorcery
	 * As an additional cost to cast this spell, sacrifice a creature.
	 * ~ deals damage to target opponent or planeswalker equal to the sacrificed creature's power. */
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	opponent_or_pw_target_definition(player, card, &td);

	test_definition_t test;
	new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_CAN_CAST && new_can_sacrifice_as_cost(player, card, &test) ){
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_OPPONENT_OR_PLANESWALKER", 1, NULL);
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		int sac = new_sacrifice(player, card, player, SAC_JUST_MARK|SAC_AS_COST|SAC_RETURN_CHOICE, &test);
		if (!sac){
			cancel = 1;
			return 0;
		}
		instance->info_slot = get_power(BYTE2(sac), BYTE3(sac));
		kill_card(BYTE2(sac), BYTE3(sac), KILL_SACRIFICE);
		return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_OPPONENT_OR_PLANESWALKER", 1, NULL);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			damage_target0(player, card, instance->info_slot);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_fire_dragon(int player, int card, event_t event)
{
	/* Fire Dragon	|6|R|R|R
	 * Creature - Dragon 6/6
	 * Flying
	 * When ~ enters the battlefield, it deals damage equal to the number of |H1Mountains you control to target creature. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.allow_cancel = 0;

		if( can_target(&td) && new_pick_target(&td, "TARGET_CREATURE", 0, 0) ){
			damage_target0(player, card, count_subtype(player, TYPE_LAND, get_hacked_subtype(player, card, SUBTYPE_MOUNTAIN)));
		}
		get_card_instance(player, card)->number_of_targets = 0;
	}
	return 0;
}

int card_fire_imp(int player, int card, event_t event){
/* CARD_ID_FIRE_IMP	2881
Fire Imp	|2|R
Creature - Imp 2/1
When ~ enters the battlefield, it deals 2 damage to target creature. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.allow_cancel = 0;

		if( can_target(&td) && new_pick_target(&td, "TARGET_CREATURE", 0, 0) ){
			damage_target0(player, card, 2);
		}
		get_card_instance(player, card)->number_of_targets = 0;
	}
	return 0;
}

int card_gift_of_estates(int player, int card, event_t event){
/* CARD_ID_GIFT_OF_ESTATES	2905
Gift of Estates	|1|W
Sorcery
If an opponent controls more lands than you, search your library for up to three |H2Plains cards, reveal them, and put them into your hand. Then shuffle your library. */

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (count_subtype(player, TYPE_LAND, -1) < count_subtype(1-player, TYPE_LAND, -1) ? 48 : -96);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( count_subtype(player, TYPE_LAND, -1) < count_subtype(1-player, TYPE_LAND, -1) ){
			test_definition_t this_test;
			new_default_test_definition(&this_test, TYPE_LAND, get_hacked_land_text(player, card, "Select %a card.", SUBTYPE_PLAINS));
			this_test.subtype = get_hacked_subtype(player, card, SUBTYPE_PLAINS);
			this_test.qty = 3;
			new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 0, AI_MAX_VALUE, &this_test);
			shuffle(player);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_gravedigger(int player, int card, event_t event)
{
  // 0x4e3e90

  /* Gravedigger	|3|B
   * Creature - Zombie 2/2
   * When ~ enters the battlefield, you may return target creature card from your graveyard to your hand. */

  /* Cadaver Imp	|1|B|B
   * Creature - Imp 1/1
   * Flying
   * When ~ enters the battlefield, you may return target creature card from your graveyard to your hand. */
	// Used externally
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_CREATURE, "Select target creature card.");
		if( count_graveyard_by_type(player, TYPE_CREATURE) && ! graveyard_has_shroud(player) ){
			new_global_tutor(player, player, TUTOR_FROM_GRAVE, TUTOR_HAND, 0, AI_MAX_VALUE, &test);
		}
	}

  return 0;
}

static int harsh_justice_legacy(int player, int card, event_t event ){

	if( get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t *instance = get_card_instance(player, card);

		int p = instance->damage_target_player;
		int c = instance->damage_target_card;

		card_instance_t *source = damage_being_dealt(player);
		if( source ){
			if( source->damage_source_player == p && source->damage_source_card == c ){
				if( source->damage_target_player == instance->targets[1].player && source->damage_target_card == -1 ){
					if( ! damage_is_to_planeswalker(source) ){
						if( instance->targets[8].player < 0 ){
							instance->targets[8].player = 0;
						}
						instance->targets[8].player += source->info_slot;
					}
				}
			}
		}
		int result = resolve_damage_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY, player, card);
		if( result ){
			damage_player(p, result, p, c);
		}
	}

	if( event == EVENT_CLEANUP ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int card_harsh_justice(int player, int card, event_t event){
/*
Harsh Justice |2|W
Instant
Cast Harsh Justice only during the declare attackers step and only if you've been attacked this step.
This turn, whenever an attacking creature deals combat damage to you, it deals that much damage to its controller.
*/

	if( event == EVENT_CAN_CAST && current_turn != player && current_phase == PHASE_BEFORE_BLOCKING){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.required_state = TARGET_STATE_ATTACKING;
		td.illegal_abilities = 0;
		if( can_target(&td) ){
			return basic_spell(player, card, event);
		}
	}

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (count_attackers(current_turn)*8);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		int count = active_cards_count[current_turn]-1;
		while( count > -1 ){
				if( in_play(current_turn, count) && is_attacking(current_turn, count) ){
					int legacy = create_targetted_legacy_effect(player, card, &harsh_justice_legacy, current_turn, count);
					card_instance_t *instance = get_card_instance( player, legacy );
					instance->targets[1].player = player;
				}
				count--;
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_jungle_lion(int player, int card, event_t event){
/* CARD_ID_JUNGLE_LION	2867
Jungle Lion	|G
Creature - Cat 2/1
~ can't block. */

	cannot_block(player, card, event);
	return 0;
}

int card_kings_assassin(int player, int card, event_t event){
/* CARD_ID_KINGS_ASSASSIN	2826
King's Assassin	|1|B|B
Creature - Human Assassin 1/1
|T: Destroy target tapped creature. Activate this ability only during your turn, before attackers are declared. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.required_state = TARGET_STATE_TAPPED;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN|GAA_LITERAL_PROMPT,
									MANACOST0, 0, &td, "Select target tapped creature.");
}

static int last_chance_legacy(int player, int card, event_t event ){

	if( current_turn == player && eot_trigger(player, card, event) ){
		get_card_instance(player, card)->targets[1].player--;
		if( get_card_instance(player, card)->targets[1].player <= 0 ){
			lose_the_game(player);
		}
	}

	return 0;
}

int card_last_chance(int player, int card, event_t event ){
// also code for final fortune
/* CARD_ID_LAST_CHANCE	2888
Last Chance	|R|R
Sorcery
Take an extra turn after this one. At the beginning of that turn's end step, you lose the game. */

	if( event == EVENT_RESOLVE_SPELL ){
		if( ! check_battlefield_for_id(1-player, CARD_ID_STRANGLEHOLD) ){
			int legacy = create_legacy_effect(player, card, &last_chance_legacy);
			card_instance_t *instance = get_card_instance(player, legacy);
			instance->targets[1].player = 2;
			return card_time_walk(player, card, event);
		}
		else{
			kill_card(player, card, KILL_DESTROY);
		}
		return 0;
	}

	return card_time_walk(player, card, event);
}

int card_lava_axe(int player, int card, event_t event)
{
	/* Lava Axe	|4|R	0x200be93
	 * Sorcery
	 * ~ deals 5 damage to target player or planeswalker. */
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if (event == EVENT_RESOLVE_SPELL)
	{
		if (valid_target(&td))
			damage_target0(player, card, 5);

		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER_OR_PLANESWALKER", 1, NULL);
}

/* CARD_ID_LAVA_FLOW	2889 => Wrecking Ball
Lava Flow	|3|R|R
Sorcery
Destroy target creature or land. */

// mercenary knight --> hidden horror

int card_mind_rot(int player, int card, event_t event)
{
  /* Mind Rot	|2|B
   * Sorcery
   * Target player discards two cards. */

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	if (event == EVENT_RESOLVE_SPELL)
	{
		if (valid_target(&td))
			new_multidiscard(get_card_instance(player, card)->targets[0].player, 2, 0, player);

		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_PLAYER", 1, NULL);
}

int card_monstrous_growth(int player, int card, event_t event){
	/*
	  Monstrous Growth |1|G
	  Sorcery
	  Target creature gets +4/+4 until end of turn.
	*/
	// Used externally
	return vanilla_instant_pump(player, card, event, ANYBODY, player, 4, 4, 0, 0);
}

int card_needle_storm(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.keyword = KEYWORD_FLYING;
		new_damage_all(player, card, 2, 4, 0, &this_test);
		kill_card(player, card, KILL_DESTROY);
	}


	return basic_spell(player, card, event);
}

int card_omen2(int player, int card, event_t event){

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += 24;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		rearrange_top_x(player, player, 3);
		int choice = do_dialog(player, player, card, -1, -1, " Shuffle\n Pass", 1);
		if( choice == 0 ){
			shuffle(player);
		}
		draw_cards(player, 1);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

// owl familiar --> merfolk traders

int card_personal_tutor(int player, int card, event_t event){

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += 96;
	}

	if(event == EVENT_RESOLVE_SPELL ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_SORCERY, "Select a sorcery card to put on top of your deck.");
		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_DECK, 0, AI_MAX_VALUE, &test);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

// pillaging horde --> balduvian horde

int card_plant_elemental(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		default_test_definition(&test, TYPE_LAND);
		test.subtype = get_hacked_subtype(player, card, SUBTYPE_FOREST);
		scnprintf(test.message, 100, "%s", get_hacked_land_text(player, card, "Select a %s to sacrifice.", SUBTYPE_FOREST));

		int kill_me = 1;
		if( new_sacrifice(player, card, player, SAC_NO_CANCEL, &test) ){
			kill_me = 0;
		}
		if( kill_me ){
			kill_card(player, card, KILL_SACRIFICE);
		}
	}

	return 0;
}

int card_primeval_force(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		default_test_definition(&test, TYPE_LAND);
		test.subtype = get_hacked_subtype(player, card, SUBTYPE_FOREST);
		scnprintf(test.message, 100, "%s", get_hacked_land_text(player, card, "Select a %s to sacrifice.", SUBTYPE_FOREST));
		test.qty = 2;

		int kill_me = 1;
		if( new_can_sacrifice(player, card, player, &test) ){
			test.qty = 1;
			int sacced[2];
			int sc = 0;
			int i;
			for(i=0; i<2; i++){
				sacced[i] = new_sacrifice(player, card, player, SAC_JUST_MARK|SAC_RETURN_CHOICE, &test);
				if( ! sacced[i] ){
					break;
				}
				sc++;
			}
			for(i=0; i<sc; i++){
				state_untargettable(BYTE2(sacced[i]), BYTE3(sacced[i]), 0);
				if( sc == 2 ){
					kill_card(BYTE2(sacced[i]), BYTE3(sacced[i]), KILL_SACRIFICE);
				}
			}
			if( sc == 2 ){
				kill_me = 0;
			}
		}
		if( kill_me ){
			kill_card(player, card, KILL_SACRIFICE);
		}
	}

	return 0;
}

int card_sacred_nectar(int player, int card, event_t event){

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (life[player] < 6 ? 96 : 24);
	}

	if(event == EVENT_RESOLVE_SPELL ){
		gain_life(player, 4);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_serpent_assassin(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.illegal_color = get_sleighted_color_test(player, card, COLOR_TEST_BLACK);

		card_instance_t *instance = get_card_instance( player, card );

		if( can_target(&td) && new_pick_target(&td, get_sleighted_color_text(player, card, "Select target non%s creature.", COLOR_BLACK), 0, GS_LITERAL_PROMPT) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
		instance->number_of_targets = 0;
	}

	return 0;
}

// spiritual guardian --> lone missionary

int card_sylvan_tutor(int player, int card, event_t event){
	// Also code of Worldly tutor

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += 96;
	}

	if(event == EVENT_RESOLVE_SPELL ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_CREATURE, "Select a creature card to put on top of your deck.");
		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_DECK, 0, AI_MAX_VALUE, &test);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_temporary_truce(int player, int card, event_t event){
	/* Temporary Truce	|1|W
	 * Sorcery
	 * Each player may draw up to two cards. For each card less than two a player draws this way, that player gains 2 life. */

	if(event == EVENT_RESOLVE_SPELL ){
		int i;
		for(i=0; i<2; i++){
			int amount = 2;
			if( !IS_AI(i) ){
				amount = choose_a_number(i, "Draw how many cards?", 2);
				if( amount > 2 || amount < 0 ){
					amount = 2;
				}
			}
			else{
				while( life[i] < 6 && amount > 1 ){
					amount--;
				}
				if( count_deck(i) <= amount ){
					amount = 0;
				}
			}
			draw_cards(i, amount);
			gain_life(i, 4-(amount*2));
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_thing_from_the_deep(int player, int card, event_t event)
{
	// Whenever ~ attacks, sacrifice it unless you sacrifice |Han Island.
	if (declare_attackers_trigger(player, card, event, 0, player, card)
		&& !player_sacrifices_a_hacked_land(player, card, player, SUBTYPE_ISLAND, 0))
		kill_card(player, card, KILL_SACRIFICE);

	return 0;
}

int card_wicked_pact(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.illegal_color = get_sleighted_color_test(player, card, COLOR_TEST_BLACK);

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_SPELL ){
		int i;
		int result = 0;
		for(i=0; i<2; i++){
			if( validate_target(player, card, &td, i) ){
				kill_card(instance->targets[i].player, instance->targets[i].card, KILL_DESTROY);
				result++;
			}
		}
		if( result ){
			lose_life(player, 5);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 2, NULL);
}

int card_wood_elves(int player, int card, event_t event){
	/*
	  Wood Elves |2|G
	  Creature - Elf Scout 1/1
	  When Wood Elves enters the battlefield, search your library for a Forest card and put that card onto the battlefield. Then shuffle your library.
	*/
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_LAND, get_hacked_land_text(player, card, "Select a %s card.", SUBTYPE_FOREST));
		test.subtype = get_hacked_subtype(player, card, SUBTYPE_FOREST);
		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_PLAY, 0, AI_MAX_VALUE, &test);
	}

	return 0;
}

// Cards - Portal 2

int card_abyssal_nightstalker(int player, int card, event_t event){

	if (event == EVENT_DECLARE_BLOCKERS && is_attacking(player, card) && is_unblocked(player, card) && ! is_humiliated(player,card) ){
		discard(1-player, 0, player);
	}

	return 0;
}

// alaborn grenadier --> serra angel

// alaborn musketeer -> vanilla


int card_alaborn_zealot(int player, int card, event_t event){

	if( event == EVENT_DECLARE_BLOCKERS && blocking(player, card, event) && ! is_humiliated(player,card) ){
		card_instance_t *instance = get_card_instance( player, card );
		kill_card(1-player, instance->blocking, KILL_DESTROY);
		kill_card(player, card, KILL_DESTROY);
	}

	return 0;
}

int card_alluring_scent(int player, int card, event_t event){
	// alluring scent

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
		if( spell_fizzled != 1 ){
			ai_modifier += (get_card_instance(instance->targets[0].player, instance->targets[0].card)->destroys_if_blocked ? 48 : -48);
		}
		return 0;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			pump_ability_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 0, 0, 0, SP_KEYWORD_LURE);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_TARGET, &td, "TARGET_CREATURE", 1, NULL);
}

int card_ancient_craving(int player, int card, event_t event){

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (life[player] >= 9 ? 48 : -96);
	}

	if( event == EVENT_RESOLVE_SPELL ){
		draw_cards(player, 3);
		lose_life(player, 3);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_angel_of_mercy(int player, int card, event_t event){
	return cip_lifegain(player, card, event, 3);
}

// brimstone dragon --> raging goblin

// brutal nightstalker --> ebon dragon

int card_cruel_edict(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, 0);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			test_definition_t test;
			new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");
			new_sacrifice(player, card, instance->targets[0].player, SAC_CAUSED|SAC_NO_CANCEL, &test);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td, NULL, 1, NULL);
}

int card_dakmor_plague(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		new_damage_all(player, card, ANYBODY, 3, NDA_PLAYER_TOO | NDA_ALL_CREATURES, NULL);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_dakmor_sorceress(int player, int card, event_t event){
	// Used externally
	if( event == EVENT_POWER && player > -1 && affect_me(player, card) && ! is_humiliated(player, card) ){
		event_result += count_subtype(player, TYPE_LAND, get_hacked_subtype(player, card, SUBTYPE_SWAMP));
	}

	return 0;
}

// deathcoil wurm --> thorn elemental

int card_denizen_of_the_deep(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.not_me = 1;
		new_manipulate_all(player, card, player, &this_test, ACT_BOUNCE);
	}

	return 0;
}

// false summoning --> remove soul

int card_festival_of_trokin(int player, int card, event_t event){

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		int amount = count_subtype(player, TYPE_CREATURE, -1);
		ai_modifier += (life[player] < 6 ? (amount * 8) : (amount * 2));
	}

	if( event == EVENT_RESOLVE_SPELL ){
		gain_life(player, count_subtype(player, TYPE_CREATURE, -1)*2);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_foul_spirit(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_LAND, "Select a land to sacrifice.");
		new_sacrifice(player, card, player, SAC_CAUSED|SAC_NO_CANCEL, &test);
	}

	return 0;
}

int card_goblin_firestarter(int player, int card, event_t event){
	/* Fire Bowman	|R	0x2009684
	 * Creature - Human Soldier Archer 1/1
	 * Sacrifice ~: It deals 1 damage to any target. Activate this ability only during your turn, before attackers are declared. */
	/* Goblin Firestarter	|R	0x2009684
	 * Creature - Goblin 1/1
	 * Sacrifice ~: It deals 1 damage to any target. Activate this ability only during your turn, before attackers are declared. */
	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE|TARGET_TYPE_PLANESWALKER);
	td.zone = TARGET_ZONE_CREATURE_OR_PLAYER;

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			damage_target0(player, card, 1);
		}
	}

	return generic_activated_ability(player, card, event, GAA_SACRIFICE_ME | GAA_CAN_TARGET | GAA_BEFORE_ATTACKERS | GAA_IN_YOUR_TURN,
									 MANACOST0, 0, &td, "TARGET_ANY");
}

int card_goblin_general(int player, int card, event_t event)
{
	// Whenever ~ attacks, Goblin creatures you control get +1/+1 until end of turn.
	if (declare_attackers_trigger(player, card, event, 0, player, card))
		pump_subtype_until_eot(player, card, player, SUBTYPE_GOBLIN, 1,1, 0,0);

	if ((event == EVENT_POW_BOOST || event == EVENT_TOU_BOOST) && current_turn == player && current_phase <= PHASE_DECLARE_ATTACKERS)
		return 1;

	return 0;
}

int card_goblin_lore(int player, int card, event_t event){

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += 48;
	}

	if(event == EVENT_RESOLVE_SPELL){
		draw_cards(player, 4);
		multidiscard(player, 3, DISC_RANDOM);
		kill_card(player, card, KILL_BURY);
	}

	return basic_spell(player, card, event);
}

int card_lurking_nightstalker(int player, int card, event_t event)
{
	/* Borderland Marauder	|1|R	0x200968e
	 * Creature - Human Warrior 1/2
	 * Whenever ~ attacks, it gets +2/+0 until end of turn. */
	/* Brazen Wolves	|2|R
	 * Creature - Wolf 2/3
	 * Whenever ~ attacks, it gets +2/+0 until end of turn. */
	/* Charging Bandits	|4|B
	 * Creature - Human Rogue 3/3
	 * Whenever ~ attacks, it gets +2/+0 until end of turn. */
	/* Hollow Dogs	|4|B
	 * Creature - Zombie Hound 3/3
	 * Whenever ~ attacks, it gets +2/+0 until end of turn. */
	/* Lurking Nightstalker	|B|B
	 * Creature - Nightstalker 1/1
	 * Whenever ~ attacks, it gets +2/+0 until end of turn. */
	/* Ravenous Skirge	|2|B
	 * Creature - Imp 1/1
	 * Flying
	 * Whenever ~ attacks, it gets +2/+0 until end of turn. */
	/* Vicious Kavu	|1|B|R
	 * Creature - Kavu 2/2
	 * Whenever ~ attacks, it gets +2/+0 until end of turn. */
	/* Wei Ambush Force	|1|B
	 * Creature - Human Soldier 1/1
	 * Whenever ~ attacks, it gets +2/+0 until end of turn. */

	return when_attacks_pump_self(player, card, event, 2,0);
}

int card_mystic_denial(int player, int card, event_t event){
	target_definition_t td;
	counterspell_target_definition(player, card, &td, TYPE_CREATURE | TYPE_SORCERY);

	return counterspell(player, card, event, &td, 0);
}

int card_nightstalker_engine(int player, int card, event_t event){
	/* Nightstalker Engine	|4|B
	 * Creature - Nightstalker 100/3
	 * ~'s power is equal to the number of creature cards in your graveyard. */

	if( event == EVENT_POWER && affect_me(player, card) && player != -1){
		event_result += count_graveyard_by_type(player, TYPE_CREATURE);
	}

	return 0;
}

int card_norwood_priestess(int player, int card, event_t event){
/*
Norwood Priestess |2|G|G
Creature — Elf Druid 1/1
{T}: You may put a green creature card from your hand onto the battlefield.
	Activate this ability only during your turn, before attackers are declared.
*/
	if( event == EVENT_ACTIVATE ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.color = get_sleighted_color_test(player, card, COLOR_TEST_GREEN);
		if( new_special_count_grave(player, &this_test) ){
			ai_modifier += 48;
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_CREATURE,
									get_sleighted_color_text(player, card, "Select a %s creature card.", COLOR_GREEN));
		this_test.color = get_sleighted_color_test(player, card, COLOR_TEST_GREEN);
		new_global_tutor(player, player, TUTOR_FROM_HAND, TUTOR_PLAY, 0, AI_MAX_CMC, &this_test);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN, MANACOST0, 0, NULL, NULL);
}

int card_ogre_arsonist(int player, int card, event_t event){
	// Used externally
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		card_instance_t *instance = get_card_instance(player, card);

		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_LAND);
		td.allow_cancel = 0;

		if( can_target(&td) && new_pick_target(&td, "TARGET_LAND", 0, 0) ){
			kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
		}
		instance->number_of_targets = 0;
	}
	return 0;
}

int card_predatory_nightstalker(int player, int card, event_t event){
/*
Predatory Nightstalker |3|B|B
Creature — Nightstalker 3/2
When Predatory Nightstalker enters the battlefield, you may have target opponent sacrifice a creature.
*/
	etb_ability(player, card, event, RESOLVE_TRIGGER_AI(player));

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.zone = TARGET_ZONE_PLAYERS;

		if( would_validate_arbitrary_target(&td, 1-player, -1) ){
			test_definition_t test;
			new_default_test_definition(&test, TYPE_CREATURE, "Select a creature to sacrifice.");
			new_sacrifice(player, card, 1-player, SAC_CAUSED|SAC_NO_CANCEL, &test);
		}
	}

	return 0;
}

int card_prowling_nightstalker(int player, int card, event_t event){

	if( event == EVENT_BLOCK_LEGALITY ){
		if( attacking_card_controller == player && attacking_card == card && ! is_humiliated(player, card) &&
			!(get_color(affected_card_controller, affected_card) & get_sleighted_color_test(player, card, COLOR_TEST_BLACK)) )
		{
			event_result = 1;
		}
	}

	return 0;
}

// raiding nightstalker --> vanilla

int card_rain_of_daggers(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			test_definition_t this_test;
			default_test_definition(&this_test, TYPE_CREATURE);
			int life_loss = new_manipulate_all(player, card, instance->targets[0].player, &this_test, KILL_DESTROY);
			lose_life(player, life_loss * 2);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td, NULL, 1, NULL);
}

int card_return_of_the_nightstalkers(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_PERMANENT);
		this_test.subtype = SUBTYPE_NIGHTSTALKER;
		new_reanimate_all(player, card, player, &this_test, REANIMATE_DEFAULT);

		default_test_definition(&this_test, TYPE_LAND);
		this_test.subtype = get_hacked_subtype(player, card, SUBTYPE_SWAMP);
		new_manipulate_all(player, card, player, &this_test, KILL_DESTROY);
		kill_card(player, card, KILL_DESTROY);

	}

	return basic_spell(player, card, event);
}

int card_righteous_charge(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		pump_subtype_until_eot(player, card, player, -1, 2, 2, 0, 0);
		kill_card(player, card, KILL_DESTROY);

	}

	return basic_spell(player, card, event);
}

int card_righteous_fury(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.state = STATE_TAPPED;

		int life_gain = 0;
		APNAP(p, { life_gain += new_manipulate_all(player, card, p, &this_test, KILL_DESTROY); };);
		gain_life(player, life_gain * 2);

		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_sea_drake(int player, int card, event_t event){
/*
Sea Drake |2|U
Creature — Drake 4/3
Flying
When Sea Drake enters the battlefield, return two target lands you control to their owner's hand.
*/

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_LAND );
		td.allowed_controller = td.preferred_controller = player;
		td.allow_cancel = 0;

		card_instance_t *instance = get_card_instance(player, card);

		int i;
		for(i=0; i<2; i++){
			if( new_pick_target(&td, "TARGET_LAND", i, 0) ){
				state_untargettable(instance->targets[i].player, instance->targets[i].card, 1);
			}
			else{
				break;
			}
		}
		for(i=0; i<instance->number_of_targets; i++){
			bounce_permanent(instance->targets[i].player, instance->targets[i].card);
		}
		instance->number_of_targets = 0;
	}

	return 0;
}

int card_swarm_of_rats(int player, int card, event_t event){
	/* Swarm of Rats	|1|B
	 * Creature - Rat 100/1
	 * ~'s power is equal to the number of Rats you control. */

	if( event == EVENT_POWER && player != -1 && affect_me(player, card) && ! is_humiliated(player, card) ){
		event_result += count_subtype(player, TYPE_PERMANENT, SUBTYPE_RAT);
	}

	return 0;
}

int card_sylvan_yeti(int player, int card, event_t event)
{
  /* Sylvan Yeti	|2|G|G
   * Creature - Yeti 100/4
   * ~'s power is equal to the number of cards in your hand. */

	if (event == EVENT_POWER && player != -1 && affect_me(player, card) && !is_humiliated(player, card) )
		event_result += hand_count[player];

	return 0;
}

int card_temple_acolyte(int player, int card, event_t event){

	return cip_lifegain(player, card, event, 3);
}

// temporal manipulation --> time walk

int card_vampiric_spirit(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		lose_life(player, 4);
	}

	return 0;
}

// Portal 3 Kingdoms

// Cards

// alert shu infantry

// ambition's cost --> ancient craving

int card_borrowing_the_east_wind(int player, int card, event_t event){

	if(event == EVENT_RESOLVE_SPELL ){
		card_instance_t *instance = get_card_instance(player, card);
		APNAP(p,{
					int count = active_cards_count[p]-1;
					while( count > -1 ){
							if( in_play(p, count) && is_what(p, count, TYPE_CREATURE) &&
								check_for_special_ability(p, count, SP_KEYWORD_HORSEMANSHIP) )
							{
								damage_creature(p, count, instance->info_slot, player, card);
							}
							count--;
					}
					damage_player(p, instance->info_slot, player, card);
				}
		);
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_X_SPELL, NULL, NULL, 0, NULL);
}

int card_burning_of_xinye(int player, int card, event_t event){
/*
Burning of Xinye English
Sorcery, 4RR (6)
You destroy four lands you control, then target opponent destroys four lands he or she controls.
Then Burning of Xinye deals 4 damage to each creature.
*/
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td_player;
	default_target_definition(player, card, &td_player, 0);
	td_player.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if(event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td_player) ){
			APNAP(p,{
						target_definition_t td;
						default_target_definition(player, card, &td, TYPE_LAND);
						td.allowed_controller = p;
						td.who_chooses = p;
						td.illegal_abilities = 0;
						td.allow_cancel = 0;

						int selected = instance->number_of_targets = 0;
						while( selected < 4 && can_target(&td) ){
								if( new_pick_target(&td, "Select a land you control.", selected, GS_LITERAL_PROMPT) ){
									state_untargettable(instance->targets[selected].player, instance->targets[selected].card, 1);
									selected++;
								}
						}

						int t;
						for(t=0; t<instance->number_of_targets; t++){
							action_on_target(player, card, t, KILL_DESTROY);
						}
					}
			);

			APNAP(p, { new_damage_all(player, card, p, 4, NDA_ALL_CREATURES, NULL); };);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td_player, NULL, 1, NULL);
}

int card_cao_cao(int player, int card, event_t event){
//Cao Cao, Lord of Wei

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_LAND );
	td.zone = TARGET_ZONE_PLAYERS;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_ACTIVATE ){
		ai_modifier += (hand_count[1-player]*8);
	}

	if( event == EVENT_RESOLVE_ACTIVATION){
		if( valid_target(&td) ){
			new_multidiscard(instance->targets[0].player, 2, 0, player);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_ONLY_TARGET_OPPONENT|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN,
									 MANACOST0, 0, &td, "TARGET_PLAYER");
}

int card_cao_ren_wei_commander(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	horsemanship2(player, card, event);

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		lose_life(player, 3);
	}

	return 0;
}

// Capture of Jingzhou --> time walk

// control of the cour --> goblin lore

// corrupt cour official --> ravenous rats

int card_diaochan_artful_beauty(int player, int card, event_t event){
/*
Diaochan, Artful Beauty |3|R
Legendary Creature — Human Advisor 1/1, 3R (4)
{T}: Destroy target creature of your choice, then destroy target creature of an opponent's choice.
	Activate this ability only during your turn, before attackers are declared.
*/
	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	target_definition_t td1;
	default_target_definition(player, card, &td1, TYPE_CREATURE);
	td1.who_chooses = 1 - player;
	td1.preferred_controller = player;
	td1.allow_cancel = 0;

	card_instance_t *instance = get_card_instance(player, card);

	if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN,
										MANACOST0, 0, &td, "TARGET_CREATURE");
	}

	if(event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN,
									MANACOST0, 0, &td, "TARGET_CREATURE");
		if( spell_fizzled != 1 ){
			add_state(instance->targets[0].player, instance->targets[0].card, STATE_TARGETTED);
			new_pick_target(&td1, "TARGET_CREATURE", 1, 1);
			remove_state(instance->targets[0].player, instance->targets[0].card, STATE_TARGETTED);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int i;
		for (i = 0; i < 2; i++){
			if( validate_target(player, card, &td, i) ){
				kill_card(instance->targets[i].player, instance->targets[i].card, KILL_DESTROY);
			}
		}
	}

	return 0;
}

int card_dong_zhou_the_tyrant(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.allowed_controller = 1-player;
		td.allow_cancel = 0;

		card_instance_t *instance = get_card_instance(player, card);

		if( can_target(&td) && new_pick_target(&td, "Select target creature your opponent controls.", 0, GS_LITERAL_PROMPT) ){
			damage_player(instance->targets[0].player, get_power(instance->targets[0].player, instance->targets[0].card),
						  instance->targets[0].player, instance->targets[0].card);
		}
	}

	return 0;
}

// false defeat --> breath of life

// famine --> dakmor plague

// fire hambush --> volcanic hammer

// fire bowman --> goblin firestarter

int card_guan_yus_1000_li_march(int player, int card, event_t event){

	if( event == EVENT_RESOLVE_SPELL ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.state = STATE_TAPPED;
		APNAP(p, { new_manipulate_all(player, card, p, &this_test, KILL_DESTROY); };);

		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_guan_yu_sainted_warrior(int player, int card, event_t event){
	// Guan Yu, Sainted Warrior
	check_legend_rule(player, card, event);

	horsemanship2(player, card, event);

	// Owner makes the choice of whether to activate, which is why the card's oracle text isn't "When ~ dies, ..."
	int owner, position;
	if (this_dies_trigger_for_owner(player, card, event, RESOLVE_TRIGGER_AI(get_owner(player, card)))
		&& find_in_owners_graveyard(player, card, &owner, &position))
		from_graveyard_to_deck(owner, position, 3);

	return 0;
}

// heavy fog --> deep wood

int card_hua_tou(int player, int card, event_t event){
/* Hua Tuo, Honored Physician	|1|G|G
 * Legendary Creature - Human 1/2
 * |T: Put target creature card from your graveyard on top of your library. Activate this ability only during your turn,
 *		before attackers are declared. */

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_CREATURE, "Select target creature card.");

	if( event == EVENT_ACTIVATE ){
		int amount = check_battlefield_for_special_card(player, card, player, CBFSC_GET_COUNT, &test);
		test.zone = TARGET_ZONE_HAND;
		amount += check_battlefield_for_special_card(player, card, player, CBFSC_GET_COUNT, &test);
		ai_modifier += (amount <= 1 ? 48 : 0);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int selected = validate_target_from_grave_source(player, card, player, 0);
		if( selected != -1 ){
			from_graveyard_to_deck(player, selected, TUTOR_HAND);
		}

	}

	return generic_activated_ability_with_target_in_grave(player, card, event, GAA_UNTAPPED|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN, MANACOST0,
															0, NULL, NULL, 0, &test, GS_GRAVE_RECYCLER);
}

int card_hunting_cheetah(int player, int card, event_t event){
/*
Hunting Cheetah |2|G
Creature — Cat 2/3
Whenever Hunting Cheetah deals damage to an opponent, you may search your library for a Forest card,
	reveal that card, put it into your hand, then shuffle your library.
*/
	int result = whenever_i_deal_damage_to_a_player(player, card, event, DDBM_MUST_DAMAGE_OPPONENT|DDBM_TRIGGER_OPTIONAL);
	if( result ){
		int times_damage[2] = {BYTE0(result), BYTE1(result)};
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_LAND);
		this_test.subtype = get_hacked_subtype(player, card, SUBTYPE_FOREST);
		this_test.qty = times_damage[1-player];
		int pos = scnprintf(this_test.message, 100, "Select up to %d", times_damage[1-player]);
		scnprintf(this_test.message + pos, 100-pos, " %s%s", get_hacked_land_text(player, card, "%a card", SUBTYPE_FOREST),
					times_damage[1-player] > 1 ? "s." : ".");
		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 0, AI_MAX_VALUE, &this_test);
	}

	return 0;
}

int card_imperial_edict(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td_player;
	default_target_definition(player, card, &td_player, TYPE_CREATURE);
	td_player.zone = TARGET_ZONE_PLAYERS;

	card_instance_t* instance = get_card_instance(player, card);

	if (event == EVENT_RESOLVE_SPELL){
		if (valid_target(&td_player)){
			target_definition_t td_creature;
			default_target_definition(player, card, &td_creature, TYPE_CREATURE);
			td_creature.allowed_controller = 1 - player;
			td_creature.preferred_controller = 1 - player;
			td_creature.who_chooses = 1 - player;
			td_creature.illegal_abilities = 0;
			td_creature.allow_cancel = 0;

			instance->number_of_targets = 0;
			if (can_target(&td_creature) && pick_target(&td_creature, "TARGET_CREATURE")){
				kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
			}
			kill_card(player, card, KILL_DESTROY);
		}
	}

	return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td_player, NULL, 1, NULL);
}

int card_imperial_recruiter(int player, int card, event_t event){

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_CREATURE, "Select a creature with Power 2 or less");
		this_test.power = 3;
		this_test.power_flag = F5_POWER_LESSER_THAN_VALUE;
		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 0, AI_MAX_VALUE, &this_test);
	}

	return 0;
}

int card_kongming_sleeping_dragon(int player, int card, event_t event)
{
	/* Kongming, "Sleeping Dragon"	|2|W|W
	 * Legendary Creature - Human Advisor 2/2
	 * Other creatures you control get +1/+1. */

	check_legend_rule(player, card, event);
	boost_subtype(player, card, event, -1, 1,1, 0,0, BCT_CONTROLLER_ONLY);
	return 0;
}

int card_kongmings_contraptions(int player, int card, event_t event){

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.required_state = TARGET_STATE_ATTACKING;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			damage_creature(instance->targets[0].player, instance->targets[0].card, 2, player, instance->parent_card);
		}
	}

	if (event == EVENT_CAN_ACTIVATE && current_phase != PHASE_BEFORE_BLOCKING){
		return 0;
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET|GAA_IN_OPPONENT_TURN, MANACOST0, 0,
									&td, "TARGET_CREATURE");
}

int card_lady_sun(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			bounce_permanent(instance->targets[0].player, instance->targets[0].card);
			bounce_permanent(instance->parent_controller, instance->parent_card);
		}
	}

	return generic_activated_ability(player, card, event,
									GAA_UNTAPPED|GAA_CAN_TARGET|GAA_NOT_ME_AS_TARGET|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN, MANACOST0,
									0, &td, "TARGET_CREATURE");
}

int card_lady_zhurong_warrior_queen(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	horsemanship2(player, card, event);

	return 0;
}

int card_liu_bei_lord_of_shu(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	horsemanship2(player, card, event);

	if( (event == EVENT_POWER || event == EVENT_TOUGHNESS) && affect_me(player, card) && ! is_humiliated(player, card) ){
		if( check_battlefield_for_id(player, CARD_ID_GUAN_YU_SAINTED_WARRIOR) ||
			check_battlefield_for_id(player, CARD_ID_ZHANG_FEI_FIERCE_WARRIOR) )
		{
			event_result+=2;
		}
	}

	return 0;
}

int card_loyal_retainers(int player, int card, event_t event){
	/* Loyal Retainers	|2|W
	 * Creature - Human Advisor 1/1
	 * Sacrifice ~: Return target legendary creature card from your graveyard to the battlefield. Activate this ability only during
	 * 		your turn, before attackers are declared. */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t test;
	new_default_test_definition(&test, TYPE_CREATURE, "Select target Legendary creature card.");
	test.subtype = SUBTYPE_LEGEND;

	if( event == EVENT_ACTIVATE ){
		test.cmc = 3;
		test.cmc_flag = F5_CMC_GREATER_THAN_VALUE;
		ai_modifier += (new_special_count_grave(player, &test) ? 96 : 0);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int selected = validate_target_from_grave_source(player, card, player, 0);
		if( selected != -1 ){
			reanimate_permanent(player, card, player, selected, REANIMATE_DEFAULT);
		}

	}

	return generic_activated_ability_with_target_in_grave(player, card, event, GAA_SACRIFICE_ME|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN,
															MANACOST0, 0, NULL, NULL, 0, &test, GS_GRAVE_RECYCLER);
}

int card_lu_bu_master_at_arms(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	horsemanship2(player, card, event);

	haste(player, card, event);

	return 0;
}

// lu meng wu general --> lady zhurong

int card_lu_su_wu_advisor(int player, int card, event_t event){
/*
Lu Su, Wu Advisor |3|U|U	0x200f543
Legendary Creature — Human Advisor 1/2
{T}: Draw a card. Activate this ability only during your turn, before attackers are declared.
*/
	check_legend_rule(player, card, event);

	if( event == EVENT_ACTIVATE ){
		ai_modifier += (56 - (hand_count[player]*8));
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		draw_cards(player, 1);
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN, MANACOST0, 0, NULL, NULL);
}

int card_lu_xun_scholar_general(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	horsemanship2(player, card, event);

	int result = whenever_i_deal_damage_to_a_player(player, card, event, DDBM_MUST_DAMAGE_OPPONENT|DDBM_TRIGGER_OPTIONAL);
	if( result ){
		int times_damage[2] = {BYTE0(result), BYTE1(result)};
		draw_cards(player, times_damage[1-player]);
	}

	return 0;
}

int card_ma_chao_western_warrior(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	horsemanship2(player, card, event);

	// Whenever ~ attacks alone, it's unblockable this combat.
	if (declare_attackers_trigger(player, card, event, DAT_ATTACKS_ALONE, player, card)){
		int leg = pump_ability_until_eot(player, card, player, card, 0, 0, 0, SP_KEYWORD_UNBLOCKABLE);
		if (leg != -1){
			card_instance_t* legacy = get_card_instance(player, leg);
			legacy->targets[3].player = PAUE_END_AT_END_OF_COMBAT;
		}
	}

	return 0;
}

int card_meng_huo_barbarian_king(int player, int card, event_t event){
	check_legend_rule(player, card, event);
	if ((event == EVENT_POWER || event == EVENT_TOUGHNESS)
		&& affected_card_controller == player && affected_card != card
		&& in_play(player, card) && ! is_humiliated(player, card)
		&& (get_color(affected_card_controller, affected_card) & get_sleighted_color_test(player, card, COLOR_TEST_GREEN))){
		event_result += 1;
	}

	return 0;
}

// mountain bandit --> raging goblin

int card_overwhelming_forces(int player, int card, event_t event){

	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.zone = TARGET_ZONE_PLAYERS;

	if( event == EVENT_RESOLVE_SPELL ){
		if( valid_target(&td) ){
			test_definition_t this_test;
			default_test_definition(&this_test, TYPE_CREATURE);
			int cd = new_manipulate_all(player, card, 1-player, &this_test, KILL_DESTROY);
			draw_cards(player, cd);
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_CAN_ONLY_TARGET_OPPONENT, &td, NULL, 1, NULL);
}

int card_pang_tong_young_phoenix(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			pump_until_eot(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card, 0, 2);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN,
									MANACOST0, 0, &td, "TARGET_CREATURE");
}

// Peach Garden Oath --> Festival of Trokin

// Preemptive Strike --> Remove Soul

// Ravaging Horde --> Ogre Arsonist

int card_riding_red_hare(int player, int card, event_t event){
	/* Riding Red Hare	|2|W
	 * Sorcery
	 * Target creature gets +3/+3 and gains horsemanship until end of turn. */
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;
	return vanilla_pump(player, card, event, &td, 3, 3, 0, SP_KEYWORD_HORSEMANSHIP);
}

int card_riding_the_dilu_horse(int player, int card, event_t event){
	/* Riding the Dilu Horse	|2|G
	 * Sorcery
	 * Target creature gets +2/+2 and gains horsemanship. */
	if( ! IS_GS_EVENT(player, card, event) ){
		return 0;
	}
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;
	return vanilla_pump(player, card, event, &td, 2, 2, 0, SP_KEYWORD_HORSEMANSHIP | SP_KEYWORD_DOES_NOT_END_AT_EOT);
}

int card_rolling_earthquake(int player, int card, event_t event)
{
	if(event == EVENT_RESOLVE_SPELL ){
		card_instance_t *instance = get_card_instance(player, card);
		APNAP(p,{
					int count = active_cards_count[p]-1;
					while( count > -1 ){
							if( in_play(p, count) && is_what(p, count, TYPE_CREATURE) &&
								! check_for_special_ability(p, count, SP_KEYWORD_HORSEMANSHIP) )
							{
								damage_creature(p, count, instance->info_slot, player, card);
							}
							count--;
					}
					damage_player(p, instance->info_slot, player, card);
				}
		);
		kill_card(player, card, KILL_DESTROY);
	}

	return generic_spell(player, card, event, GS_X_SPELL, NULL, NULL, 0, NULL);
}

// sage's knowledge --> deja vu

int card_shu_cavalry(int player, int card, event_t event){
	horsemanship2(player, card, event);
	return 0;
}

// shu elite companions --> shu cavalry

// shu foot soldiers --> vanilla

int card_shu_general(int player, int card, event_t event){
	vigilance(player, card, event);
	horsemanship2(player, card, event);
	return 0;
}

// shu grain caravan --> lone missionary

int card_sima_yi_wei_field_general(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	return card_dakmor_sorceress(player, card, event);
}

int card_sun_ce_young_conquerer(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	horsemanship2(player, card, event);

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);

		card_instance_t *instance = get_card_instance( player, card );

		if( can_target(&td) && new_pick_target(&td, "TARGET_CREATURE", 0, 0) ){
			bounce_permanent(instance->targets[0].player, instance->targets[0].card);
		}
		instance->number_of_targets = 0;
	}

	return 0;
}

int card_sun_quan_lord_of_wu(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	if (event == EVENT_ABILITIES
		&& in_play(player, card)
		&& affected_card_controller == player
		&& !is_humiliated(player, card)
		&& is_what(affected_card_controller, affected_card, TYPE_CREATURE)){
		horsemanship2(affected_card_controller, affected_card, event);
	}

	return 0;
}

// warrior's oath --> last chance

// wei ambush force --> lurking nightstalker

int card_wei_assassins(int player, int card, event_t event){
/*
Wei Assassins |3|B|B
Creature — Human Soldier Assassin 3/2
When Wei Assassins enters the battlefield, target opponent chooses a creature he or she controls. Destroy it.
*/
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.zone = TARGET_ZONE_PLAYERS;

		card_instance_t *instance = get_card_instance( player, card );

		instance->targets[0].player = 1-player;
		instance->number_of_targets = 1;
		if( valid_target(&td) ){
			target_definition_t td1;
			default_target_definition(player, card, &td1, TYPE_CREATURE);
			td1.allowed_controller = 1-player;
			td1.preferred_controller = 1-player;
			td1.who_chooses = 1-player;
			td1.illegal_abilities = 0;

			if( can_target(&td1) && new_pick_target(&td1, "Select a creature you control.", 1, GS_LITERAL_PROMPT) ){
				kill_card(instance->targets[1].player, instance->targets[1].card, KILL_DESTROY);
			}
		}
		instance->number_of_targets = 0;
	}

	return 0;
}

// wei elite companions --> shu cavalry

// wei infantry --> vanilla

int card_wei_night_riders(int player, int card, event_t event){

	horsemanship2(player, card, event);

	int result = whenever_i_deal_damage_to_a_player(player, card, event, DDBM_MUST_DAMAGE_OPPONENT);
	if( result ){
		int times_damage[2] = {BYTE0(result), BYTE1(result)};
		new_multidiscard(1-player, times_damage[1-player], 0, player);
	}

	return 0;
}

// wei scout --> shu cavalry

// wei strike force --> shu cavalry

// wolf pack -->thorn elemental

// wu elite cavalry --> shu cavalry

// wu infantry --> vanilla

// wu light cavalry -->  shu cavalry

int card_xiahou_dun_the_one_eyed(int player, int card, event_t event){
/* Xiahou Dun, the One-Eyed	|2|B|B
 * Legendary Creature - Human Soldier 3/2
 * Horsemanship
 * Sacrifice ~: Return target |Sblack card from your graveyard to your hand. Activate this ability only during your turn,
 *		before attackers are declared. */

	horsemanship2(player, card, event);

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	test_definition_t this_test;
	new_default_test_definition(&this_test, TYPE_ANY, get_sleighted_color_text(player, card, "Select a %s card.", COLOR_BLACK));
	this_test.color = get_sleighted_color_test(player, card, COLOR_TEST_BLACK);

	if( event == EVENT_ACTIVATE ){
		this_test.cmc = 4;
		this_test.cmc_flag = F5_CMC_GREATER_THAN_VALUE;
		ai_modifier += (new_special_count_grave(player, &this_test) ? 96 : 0);
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		int selected = validate_target_from_grave_source(player, card, player, 0);
		if( selected != -1 ){
			from_grave_to_hand(player, selected, TUTOR_HAND);
		}
	}

	return generic_activated_ability_with_target_in_grave(player, card, event, GAA_SACRIFICE_ME|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN,
															MANACOST0, 0, NULL, NULL, 0, &this_test, GS_GRAVE_RECYCLER);
}

int card_xun_yu_wei_advisor(int player, int card, event_t event){

	check_legend_rule(player, card, event);

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance( player, card );

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			pump_until_eot(instance->parent_controller, instance->parent_card, instance->targets[0].player, instance->targets[0].card, 2, 0);
		}
	}

	return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN,
									MANACOST0, 0, &td, "TARGET_CREATURE");
}

int card_zhang_fei_fierce_warrior(int player, int card, event_t event){
/* CARD_ID_ZHANG_FEI_FIERCE_WARRIOR	4238
Zhang Fei, Fierce Warrior	|4|W|W
Legendary Creature - Human Soldier Warrior 4/4
Vigilance; horsemanship (This creature can't be blocked except by creatures with horsemanship.) */

	check_legend_rule(player, card, event);
	vigilance(player, card, event);
	horsemanship2(player, card, event);
	return 0;
}

int card_zhang_he_wei_general(int player, int card, event_t event)
{
/* CARD_ID_ZHANG_HE_WEI_GENERAL	4294
Zhang He, Wei General	|3|B|B
Legendary Creature - Human Soldier 4/2
Horsemanship (This creature can't be blocked except by creatures with horsemanship.)
Whenever ~ attacks, each other creature you control gets +1/+0 until end of turn. */

	check_legend_rule(player, card, event);

	horsemanship2(player, card, event);

	// Whenever ~ attacks, each other creature you control gets +1/+0 until end of turn.
	if (declare_attackers_trigger(player, card, event, 0, player, card)){
		test_definition_t this_test;
		new_default_test_definition(&this_test, TYPE_CREATURE, "");
		this_test.not_me = 1;
		pump_creatures_until_eot(player, card, player, 0, 1,0, 0,0, &this_test);
	}

	return 0;
}

int card_zhang_liao_hero_of_hefei(int player, int card, event_t event){
/* CARD_ID_ZHANG_LIAO_HERO_OF_HEFEI	4295
Zhang Liao, Hero of Hefei	|4|B|B
Legendary Creature - Human Soldier 3/3
Whenever ~ deals damage to an opponent, that opponent discards a card. */

	check_legend_rule(player, card, event);

	int result = whenever_i_deal_damage_to_a_player(player, card, event, DDBM_MUST_DAMAGE_OPPONENT);
	if( result ){
		int times_damage[2] = {BYTE0(result), BYTE1(result)};
		new_multidiscard(1-player, times_damage[1-player], 0, player);
	}

	return 0;
}

int card_zhuge_jin_wu_strategist(int player, int card, event_t event){
/* CARD_ID_ZHUGE_JIN_WU_STRATEGIST	4268
Zhuge Jin, Wu Strategist	|1|U|U
Legendary Creature - Human Advisor 1/1
|T: Target creature can't be blocked this turn. Activate this ability only during your turn, before attackers are declared.
	check_legend_rule(player, card, event); */

	if( ! IS_GAA_EVENT(event) ){
		return 0;
	}

	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_CREATURE);
	td.preferred_controller = player;

	card_instance_t *instance = get_card_instance( player, card );

    if( event == EVENT_CAN_ACTIVATE ){
		return generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN,
                                             MANACOST0, 0, &td, NULL);
	}

	if( event == EVENT_ACTIVATE ){
		generic_activated_ability(player, card, event, GAA_UNTAPPED|GAA_CAN_TARGET|GAA_BEFORE_ATTACKERS|GAA_IN_YOUR_TURN,
									MANACOST0, 0, &td, "TARGET_CREATURE");
		if( spell_fizzled != 1 ){
			ai_modifier += (get_attack_power(instance->targets[0].player, instance->targets[0].card) * 8);
		}
	}

	if( event == EVENT_RESOLVE_ACTIVATION ){
		if( valid_target(&td) ){
			pump_ability_until_eot(instance->parent_controller, instance->parent_card,
									instance->targets[0].player, instance->targets[0].card, 0, 0, 0, SP_KEYWORD_UNBLOCKABLE);
		}
	}

	return 0;
}

int card_zodiac_dragon(int player, int card, event_t event)
{
/* CARD_ID_ZODIAC_DRAGON	4327
Zodiac Dragon	|7|R|R
Creature - Dragon 8/8
When ~ is put into your graveyard from the battlefield, you may return it to your hand. */

  int owner, position;
  if (this_dies_trigger_for_owner(player, card, event, RESOLVE_TRIGGER_AI(get_owner(player, card)))
	  && find_in_owners_graveyard(player, card, &owner, &position))
	{
	  int iid = get_grave(owner)[position];
	  remove_card_from_grave(owner, position);
	  add_card_to_hand(owner, iid);
	}

  return 0;
}

// cards - Starter

int card_dakmor_ghoul(int player, int card, event_t event){
/* CARD_ID_DAKMOR_GHOUL	4369
Dakmor Ghoul	|2|B|B
Creature - Zombie 2/2
When ~ enters the battlefield, target opponent loses 2 life and you gain 2 life. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_CREATURE);
		td.zone = TARGET_ZONE_PLAYERS;

		card_instance_t *instance = get_card_instance( player, card );

		instance->targets[0].player = 1-player;
		instance->number_of_targets = 1;
		if( valid_target(&td) ){
			lose_life(instance->targets[0].player, 2);
			gain_life(player, 2);
		}
		instance->number_of_targets = 0;
	}

	return 0;
}

int card_devout_monk(int player, int card, event_t event){
/* CARD_ID_DEVOUT_MONK	4362
Devout Monk	|W
Creature - Human Monk Cleric 1/1
When ~ enters the battlefield, you gain 1 life. */

   return cip_lifegain(player, card, event, 1);
}

int card_goblin_settler(int player, int card, event_t event){
	/*
	  Goblin Settler |3|R
	  Creature - Goblin 1/1
	  When Goblin Settler enters the battlefield, destroy target land.
	*/
	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		target_definition_t td;
		default_target_definition(player, card, &td, TYPE_LAND);
		td.allow_cancel = 0;

		if( can_target(&td) && pick_target(&td, "TARGET_LAND") ){
			action_on_target(player, card, 0, KILL_DESTROY);
		}
		get_card_instance(player, card)->number_of_targets = 0;
	}

	return 0;
}

int card_grim_tutor(int player, int card, event_t event){
/* CARD_ID_GRIM_TUTOR	1516
Grim Tutor	|1|B|B
Sorcery
Search your library for a card and put that card into your hand, then shuffle your library. You lose 3 life. */

	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += (life[player] >= 9 ? 96 : -24);
	}

	if(event == EVENT_RESOLVE_SPELL ){
		test_definition_t test;
		new_default_test_definition(&test, TYPE_ANY, "Select a card.");
		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 0, AI_MAX_VALUE, &test);
		lose_life(player, 3);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

// loyal sentry -> alaborn_zealot

// pride of lions --> thorn elemental

int card_sleight_of_hand(int player, int card, event_t event){
/*
Sleight of Hand |U
Sorcery
Look at the top two cards of your library. Put one of them into your hand and the other on the bottom of your library.
*/
	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += 48;
	}

	if(event == EVENT_RESOLVE_SPELL ){
		int amount = MIN(2, count_deck(player));
		if( amount ){
			test_definition_t test;
			new_default_test_definition(&test, TYPE_ANY, "Select a card to add to your hand.");
			test.create_minideck = amount;
			test.no_shuffle = 1;
			new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 1, AI_MAX_VALUE, &test);
			amount--;
			if( amount ){
				put_top_card_of_deck_to_bottom(player);
			}
		}
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

int card_thunder_dragon(int player, int card, event_t event){
/* CARD_ID_THUNDER_DRAGON	4375
Thunder Dragon	|5|R|R
Creature - Dragon 5/5
Flying
When ~ enters the battlefield, it deals 3 damage to each creature without flying. */

	etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);

	if( event == EVENT_ETB_ABILITY ){
		test_definition_t this_test;
		default_test_definition(&this_test, TYPE_CREATURE);
		this_test.keyword = KEYWORD_FLYING;
		this_test.keyword_flag = 1;
		new_damage_all(player, card, ANYBODY, 3, 0, &this_test);
	}

	return 0;
}

int card_tidings(int player, int card, event_t event){
/*
Tidings |3|U|U
Sorcery
Draw four cards.
*/
	if( event == EVENT_CAST_SPELL && affect_me(player, card) ){
		ai_modifier += 48;
	}

	if( event == EVENT_RESOLVE_SPELL ){
		draw_cards(player, 4);
		kill_card(player, card, KILL_DESTROY);
	}

	return basic_spell(player, card, event);
}

// veteran cavalier --> serra angel

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int taunt_force_attack(int player, int card, event_t event);

/* ManalinkEh 2022: 0x21bf780; original cards/portal_1_2_3k.c:2591. */
int card_yellow_scarves_cavalry(int player, int card, event_t event)
{
  horsemanship2(player, card, event);
  cannot_block(player, card, event);
  return 0;
}

/* ManalinkEh 2022: 0x21c0050; original cards/portal_1_2_3k.c:2817. */
int card_cinder_storm(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0x102);
        td.zone = 0x1200;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            damage_target0(player, card, 7);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c00ac;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0x102);
  td.zone = 0x1200;
LAB_021c00ac:
  result = generic_spell(player, card, event, 1, &td, "TARGET_ANY", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c04b0; original cards/portal_1_2_3k.c:2926. */
int card_squall(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 2);
    this_test.keyword = 0x20;
    new_damage_all(player, card, 2, 2, 0, &this_test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x21c08e0; original cards/portal_1_2_3k.c:3052. */
int card_mind_knives(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
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
        td.allowed_controller = 1 - player;
        td.zone = 0x1000;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            instance = get_card_instance(player, card);
            new_multidiscard(instance->targets[0].player, 1, 1, player);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c0946;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0);
  td.allowed_controller = 1 - player;
  td.zone = 0x1000;
LAB_021c0946:
  result = generic_spell(player, card, event, 0xc0, &td,
                         "Target opponent discards a card at random.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c0a40; original cards/portal_1_2_3k.c:3076. */
int card_natures_ruin(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 2);
    this_test.color = get_sleighted_color_test(player, card, 8);
    new_manipulate_all(player, card, 2, &this_test, ACT_KILL_DESTROY);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x21c0af0; original cards/portal_1_2_3k.c:3094. */
int card_noxious_toad(int player, int card, event_t event)
{
  int result;
  result = this_dies_trigger(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (result == 0)
  {
    return 0;
  }
  discard(1 - player, 0, player);
  return 0;
}

/* ManalinkEh 2022: 0x21c0ce0; original cards/portal_1_2_3k.c:3138. */
int card_vampiric_touch(int player, int card, event_t event)
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
        opponent_or_pw_target_definition(player, card, &td);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            damage_target0(player, card, 2);
            gain_life_2022(player, 2);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c0d29;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  opponent_or_pw_target_definition(player, card, &td);
LAB_021c0d29:
  result = generic_spell(player, card, event, 1, &td, "TARGET_OPPONENT_OR_PLANESWALKER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c0e10; original cards/portal_1_2_3k.c:3162. */
int card_virtues_ruin(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 2);
    this_test.color = get_sleighted_color_test(player, card, 0x20);
    new_manipulate_all(player, card, 2, &this_test, ACT_KILL_DESTROY);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x21c10f0; original cards/portal_1_2_3k.c:3222. */
int card_capricious_sorcerer(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0x102);
    td.zone = 0x1200;
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      damage_target0(player, card, 1);
    }
  }
  result = generic_activated_ability(player, card, event, 0x10010005, 0, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_ANY");
  return result;
}

/* ManalinkEh 2022: 0x21c1220; original cards/portal_1_2_3k.c:3249. */
int card_cloak_of_feathers(int player, int card, event_t event)
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
            pump_ability_until_eot_2022(player, card, instance->targets[0].player,
                                   instance->targets[0].card, 0, 0, 0x20, 0);
            draw_cards(player, 1);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c1289;
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
LAB_021c1289:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c13a0; original cards/portal_1_2_3k.c:3277. */
int card_ingenious_thief(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = new_pick_target(&td, "TARGET_PLAYER", 0, 0);
      if (result != 0)
      {
        reveal_target_player_hand(instance->targets[0].player);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c1460; original cards/portal_1_2_3k.c:3302. */
int card_sorcerous_sight(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
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
      if ((card == affected_card) && (player == affected_card_controller))
      {
        ai_modifier = ai_modifier + 0x60;
      }
      goto LAB_021c14d1;
    }
    if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
    {
      default_target_definition(player, card, &td, 0);
      td.zone = 0x1000;
      instance = get_card_instance(player, card);
      if (event == EVENT_RESOLVE_SPELL)
      {
        result = valid_target(&td);
        if (result != 0)
        {
          reveal_target_player_hand(instance->targets[0].player);
          draw_cards(player, 1);
        }
        kill_card(player, card, KILL_DESTROY);
      }
      goto LAB_021c14d1;
    }
    if (event != EVENT_CHANGE_TARGET)
    {
      return 0;
    }
  }
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  get_card_instance(player, card);
LAB_021c14d1:
  result = generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21b8f10; original cards/portal_1_2_3k.c:3333. */
int taunt_force_attack(int player, int card, event_t event)
{
  card_instance_t *instance;
  event_flags = event_flags | 0x1000000;
  if (event == EVENT_BEGIN_COMBAT)
  {
    if ((1 - player == current_turn) && (forbid_attack == 0))
    {
      instance = get_card_instance(player, card);
      if (instance->damage_source_player != current_turn)
      {
        all_must_attack_if_able(current_turn, EVENT_BEGIN_COMBAT, -1);
      }
    }
  }
  else if ((1 - player == current_turn) && (event == EVENT_CLEANUP))
  {
    kill_card(player, card, KILL_EXILE);
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c1610; original cards/portal_1_2_3k.c:3350. */
int card_taunt(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = valid_target(&td);
    if (result != 0)
    {
      create_legacy_effect(player, card, taunt_force_attack);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c1900; original cards/portal_1_2_3k.c:3411. */
int card_fruition(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_CAST_SPELL)
  {
    if (((card == affected_card) && (player == affected_card_controller)) && (life[player] < 6))
    {
      result = get_hacked_subtype(player, card, 0x5001);
      result = count_subtype(2, 1, result);
      ai_modifier = ai_modifier + result * 8;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    result = count_subtype(2, 1, 0x5001);
    gain_life_2022(player, result);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c1b00; original cards/portal_1_2_3k.c:3453. */
int card_burning_cloak(int player, int card, event_t event)
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
            pump_until_eot_2022(player, card, instance->targets[0].player, instance->targets[0].card, 2,
                           0);
            damage_creature(instance->targets[0].player, instance->targets[0].card, 2, player,
                            card);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c1b66;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_021c1b66:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target creature it gets +2/+0 & is dealt 2 damage.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c1c80; original cards/portal_1_2_3k.c:3479. */
int card_forked_lightning(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_t *ptVar3;
  target_t *ptVar4;
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
      if ((card == affected_card) && (player == affected_card_controller))
      {
        instance->number_of_targets = '\0';
        result = hybrid_casting(player, card, 0);
        if (result != 0)
        {
          ptVar3 = instance->targets;
          do
          {
            result = new_pick_target(&td, "Select target creature to deal 1 damage.", -1, 0x81);
            if (result == 0)
              break;
            ptVar4 = ptVar3 + 1;
            add_state(ptVar3->player, ptVar3->card, 0x200000);
            ptVar3 = ptVar4;
          } while (instance->targets + 4 != ptVar4);
          result = 0;
          if (instance->number_of_targets != '\0')
          {
            do
            {
              ptVar4 = instance->targets + result;
              ptVar3 = instance->targets + result;
              result = result + 1;
              remove_state(ptVar3->player, ptVar4->card, 0x200000);
            } while (result < (int)(unsigned int)instance->number_of_targets);
          }
        }
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      divide_damage(player, card, &td);
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

/* ManalinkEh 2022: 0x21c2270; original cards/portal_1_2_3k.c:3596. */
int card_false_peace(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td_player;
  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    result = can_target(&td_player);
    return result;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td_player, 0);
      td_player.zone = 0x1000;
      if ((card == affected_card) && (player == affected_card_controller))
      {
        pick_target(&td_player, "TARGET_PLAYER");
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td_player, 0);
    td_player.zone = 0x1000;
    if (event == EVENT_RESOLVE_SPELL)
    {
      instance = get_card_instance(player, card);
      result = valid_target(&td_player);
      if (result != 0)
      {
        target_player_skips_his_next_attack_step(player, card, instance->targets[0].player, 0);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td_player, 0);
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c2470; original cards/portal_1_2_3k.c:3644. */
int card_renewing_dawn(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_CAST_SPELL)
  {
    if (((card == affected_card) && (player == affected_card_controller)) && (life[player] < 6))
    {
      result = get_hacked_subtype(player, card, 0x5006);
      result = count_subtype(1 - player, 1, result);
      ai_modifier = ai_modifier + result * 0x10;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    result = get_hacked_subtype(player, card, 0x5006);
    result = count_subtype(1 - player, 1, result);
    gain_life_2022(player, result * 2);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c2570; original cards/portal_1_2_3k.c:3662. */
int card_sacred_knight(int player, int card, event_t event)
{
  unsigned int uVar1;
  unsigned int uVar2;
  if (((event == EVENT_BLOCK_LEGALITY) && (attacking_card_controller == player)) &&
      (attacking_card == card))
  {
    uVar1 = get_color(affected_card_controller, affected_card);
    uVar2 = get_sleighted_color_test(player, card, 2);
    if ((uVar1 & uVar2) == 0)
    {
      uVar1 = get_color(affected_card_controller, affected_card);
      uVar2 = get_sleighted_color_test(player, card, 0x10);
      if ((uVar1 & uVar2) == 0)
      {
        return 0;
      }
    }
    event_result = 1;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c2620; original cards/portal_1_2_3k.c:3676. */
int card_starlight(int player, int card, event_t event)
{
  int result;
  test_definition_t this_test;
  if (event == EVENT_CAST_SPELL)
  {
    if (((card == affected_card) && (player == affected_card_controller)) && (life[player] < 6))
    {
      default_test_definition(&this_test, 2);
      this_test.color = 2;
      result = check_battlefield_for_special_card(player, card, 1 - player, 4, &this_test);
      ai_modifier = ai_modifier + result * 0x18;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 2);
    this_test.color = 2;
    result = check_battlefield_for_special_card(player, card, 1 - player, 4, &this_test);
    if (0 < result)
    {
      gain_life_2022(player, result * 3);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x21c2770; original cards/portal_1_2_3k.c:3704. */
int card_steadfastness(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    pump_subtype_until_eot_2022(player, card, player, -1, 0, 3, 0, 0);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c27f0; original cards/portal_1_2_3k.c:3717. */
int card_stern_marshal(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  target_definition_t td;
  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      instance = get_card_instance(player, card);
      pump_ability_until_eot_2022(instance->parent_controller, instance->parent_card,
                             instance->targets[0].player, instance->targets[0].card, 2, 2, 0, 0);
    }
  }
  result = generic_activated_ability(player, card, event, 0x10010005, 0, 0, 0, 0, 0, 0, 0, &td,
                                     "TARGET_CREATURE");
  return result;
}

/* ManalinkEh 2022: 0x21c2950; original cards/portal_1_2_3k.c:3742. */
int card_valorous_charge(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = get_sleighted_color_test(player, card, 0x20);
    pump_color_until_eot_2022(player, card, 2, result, 2, 0, 0, 0);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c29f0; original cards/portal_1_2_3k.c:3755. */
int card_natural_spring(int player, int card, event_t event)
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
        td.zone = 0x1000;
        td.preferred_controller = player;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            gain_life_2022(instance->targets[0].player, 8);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c2a60;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.zone = 0x1000;
  td.preferred_controller = player;
  get_card_instance(player, card);
LAB_021c2a60:
  result = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c2b40; original cards/portal_1_2_3k.c:3781. */
int card_tidal_surge(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  card_instance_t *instance;
  int iVar4;
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
        td.illegal_abilities = td.illegal_abilities | 0x20;
        td.allow_cancel = 3;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          if (instance->number_of_targets != '\0')
          {
            iVar5 = 0;
            do
            {
              while (iVar4 = validate_target(player, card, &td, iVar5), iVar4 == 0)
              {
                iVar5 = iVar5 + 1;
                if ((int)(unsigned int)instance->number_of_targets <= iVar5)
                  goto LAB_021c2c16;
              }
              ptVar2 = instance->targets + iVar5;
              ptVar1 = instance->targets + iVar5;
              iVar5 = iVar5 + 1;
              tap_card(ptVar1->player, ptVar2->card);
            } while (iVar5 < (int)(unsigned int)instance->number_of_targets);
          }
        LAB_021c2c16:
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c2c88;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.illegal_abilities = td.illegal_abilities | 0x20;
  td.allow_cancel = 3;
  get_card_instance(player, card);
LAB_021c2c88:
  iVar5 = generic_spell(player, card, event, 0x20, &td, "TARGET_CREATURE", 3, NULL);
  return iVar5;
}

/* ManalinkEh 2022: 0x21c2d00; original cards/portal_1_2_3k.c:3811. */
int card_angelic_blessing(int player, int card, event_t event)
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
            pump_ability_until_eot_2022(player, card, instance->targets[0].player,
                                   instance->targets[0].card, 3, 3, 0x20, 0);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c2d69;
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
LAB_021c2d69:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c2e70; original cards/portal_1_2_3k.c:3837. */
int card_path_of_peace(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
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
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            instance = get_card_instance(player, card);
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            result = get_owner(instance->targets[0].player, instance->targets[0].card);
            gain_life_2022(result, 4);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c2ec5;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
LAB_021c2ec5:
  result = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c2fe0; original cards/portal_1_2_3k.c:3864. */
int card_natures_cloak(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = get_hacked_walk(player, card, 4);
    pump_color_until_eot_2022(player, card, player, 8, 0, 0, result, 0);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c3210; original cards/portal_1_2_3k.c:3911. */
int card_kiss_of_death(int player, int card, event_t event)
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
        opponent_or_pw_target_definition(player, card, &td);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            damage_target0(player, card, 4);
            gain_life_2022(player, 4);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c3259;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  opponent_or_pw_target_definition(player, card, &td);
LAB_021c3259:
  result = generic_spell(player, card, event, 1, &td, "TARGET_OPPONENT_OR_PLANESWALKER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c3560; original cards/portal_1_2_3k.c:3977. */
int card_remove(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.required_state = 2;
    get_card_instance(player, card);
    if ((current_turn == player) || (current_phase != 0x16))
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
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            bounce_permanent(instance->targets[0].player, instance->targets[0].card);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c35cc;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
    default_target_definition(player, card, &td, 2);
    td.required_state = 2;
    get_card_instance(player, card);
  }
LAB_021c35cc:
  result = generic_spell(player, card, event, 1, &td, "TARGET_ATTACKING_CREATURE", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c3700; original cards/portal_1_2_3k.c:4012. */
int card_talas_explorer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  int tgt_player;
  target_definition_t td;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    td.allow_cancel = 0;
    instance = get_card_instance(player, card);
    tgt_player = 1 - player;
    instance->targets[0].player = tgt_player;
    result = would_validate_arbitrary_target(&td, tgt_player, -1);
    if (result != 0)
    {
      reveal_target_player_hand(tgt_player);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c37b0; original cards/portal_1_2_3k.c:4036. */
int card_talas_researcher(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_ACTIVATE)
  {
    ai_modifier = ai_modifier + (7 - hand_count[player]) * 8;
  }
  else if (event == EVENT_RESOLVE_ACTIVATION)
  {
    draw_cards(player, 1);
  }
  result =
      generic_activated_ability(player, card, event, 0x10010001, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c3880; original cards/portal_1_2_3k.c:4053. */
int card_wind_sail(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    get_card_instance(player, card);
    iVar5 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_CREATURE", 1, NULL);
    return iVar5;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.preferred_controller = player;
      instance = get_card_instance(player, card);
      if ((card == affected_card) &&
          ((player == affected_card_controller &&
            (generic_spell(player, card, EVENT_CAST_SPELL, 1, &td, "TARGET_CREATURE", 1, NULL),
             spell_fizzled != 1))))
      {
        state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
        iVar5 = can_target(&td);
        if (iVar5 != 0)
        {
          td.allow_cancel = 2;
          new_pick_target(&td, "TARGET_CREATURE", 1, 0);
        }
        state_untargettable(instance->targets[0].player, instance->targets[0].card, 0);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar5 = 0;
      if (instance->number_of_targets != '\0')
      {
        do
        {
          while (iVar4 = validate_target(player, card, &td, iVar5), iVar4 != 0)
          {
            ptVar2 = instance->targets + iVar5;
            ptVar1 = instance->targets + iVar5;
            iVar5 = iVar5 + 1;
            pump_ability_until_eot_2022(player, card, ptVar1->player, ptVar2->card, 0, 0, 0x20, 0);
            if ((int)(unsigned int)instance->number_of_targets <= iVar5)
              goto LAB_021c3971;
          }
          iVar5 = iVar5 + 1;
        } while (iVar5 < (int)(unsigned int)instance->number_of_targets);
      }
    LAB_021c3971:
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c4170; original cards/portal_1_2_3k.c:4233. */
int card_goblin_war_cry(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  target_definition_t td_player;
  target_definition_t td_creature;

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
        default_target_definition(player, card, &td_player, 2);
        td_player.zone = 0x1000;
        instance = get_card_instance(player, card);
        if ((event == EVENT_RESOLVE_SPELL) && (iVar2 = valid_target(&td_player), iVar2 != 0))
        {
          default_target_definition(player, card, &td_creature, 2);
          td_creature.who_chooses = 1 - player;
          td_creature.allow_cancel = 0;
          td_creature.illegal_abilities = 0;
          instance->number_of_targets = '\0';
          iVar2 = instance->targets[0].player;
          td_creature.allowed_controller = td_creature.who_chooses;
          td_creature.preferred_controller = td_creature.who_chooses;
          iVar3 = can_target(&td_creature);
          if ((iVar3 != 0) && ((iVar3 = pick_target(&td_creature, "TARGET_CREATURE"),
                                iVar3 != 0 && (0 < active_cards_count[iVar2]))))
          {
            iVar3 = 0;
            do
            {
              iVar4 = is_what(iVar2, iVar3, 2);
              if ((iVar4 != 0) && (instance->targets[0].card != iVar3))
              {
                pump_ability_until_eot_2022(player, card, iVar2, iVar3, 0, 0, 0, 0x20);
              }
              iVar3 = iVar3 + 1;
            } while (iVar3 < active_cards_count[iVar2]);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c41ed;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td_player, 2);
  td_player.zone = 0x1000;
  get_card_instance(player, card);
LAB_021c41ed:
  iVar2 = generic_spell(player, card, event, 0x40, &td_player, NULL, 1, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x21c43d0; original cards/portal_1_2_3k.c:4281. */
int card_angel_of_fury(int player, int card, event_t event)
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
      from_graveyard_to_deck(owner, position, 3);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c4460; original cards/portal_1_2_3k.c:4295. */
int card_bargain(int player, int card, event_t event)
{
  int result;
  target_definition_t td;
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  if (event == EVENT_RESOLVE_SPELL)
  {
    result = would_validate_arbitrary_target(&td, 1 - player, -1);
    if (result != 0)
    {
      draw_cards(1 - player, 1);
      gain_life_2022(player, 7);
    }
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c4830; original cards/portal_1_2_3k.c:4385. */
int card_temple_elder(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_ACTIVATION)
  {
    gain_life_2022(player, 1);
  }
  result =
      generic_activated_ability(player, card, event, 0x10000001, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c4ae0; original cards/portal_1_2_3k.c:4450. */
int card_jagged_lightning(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
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
          iVar3 = 0;
          do
          {
            iVar2 = validate_target(player, card, &td, iVar3);
            if (iVar2 != 0)
            {
              damage_creature(instance->targets[iVar3].player, instance->targets[iVar3].card, 3,
                              player, card);
            }
            iVar3 = iVar3 + 1;
          } while (iVar3 != 2);
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c4b89;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  get_card_instance(player, card);
LAB_021c4b89:
  iVar3 = generic_spell(player, card, event, 1, &td, "TARGET_CREATURE", 2, NULL);
  return iVar3;
}

/* ManalinkEh 2022: 0x21c4c50; original cards/portal_1_2_3k.c:4479. */
int card_emty_city_ruse(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
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
        td.zone = 0x1000;
        if (event == EVENT_RESOLVE_SPELL)
        {
          instance = get_card_instance(player, card);
          instance->targets[0].player = 1 - player;
          result = valid_target(&td);
          if (result != 0)
          {
            target_player_skips_his_next_attack_step(player, card, instance->targets[0].player, 0);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c4cac;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
LAB_021c4cac:
  result = generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c51b0; original cards/portal_1_2_3k.c:4580. */
int card_counterintelligence(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    get_card_instance(player, card);
    iVar5 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL);
    return iVar5;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      instance = get_card_instance(player, card);
      if ((card == affected_card) && ((player == affected_card_controller &&
                                       (generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                                                      "Select target creature to bounce.", 1, NULL),
                                        spell_fizzled != 1))))
      {
        state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
        iVar5 = can_target(&td);
        if (iVar5 != 0)
        {
          td.allow_cancel = 2;
          new_pick_target(&td, "Select target creature to bounce.", 1, 0x80);
        }
        state_untargettable(instance->targets[0].player, instance->targets[0].card, 0);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar5 = 0;
      if (instance->number_of_targets != '\0')
      {
        do
        {
          while (iVar4 = validate_target(player, card, &td, iVar5), iVar4 != 0)
          {
            ptVar2 = instance->targets + iVar5;
            ptVar1 = instance->targets + iVar5;
            iVar5 = iVar5 + 1;
            bounce_permanent(ptVar1->player, ptVar2->card);
            if ((int)(unsigned int)instance->number_of_targets <= iVar5)
              goto LAB_021c526e;
          }
          iVar5 = iVar5 + 1;
        } while (iVar5 < (int)(unsigned int)instance->number_of_targets);
      }
    LAB_021c526e:
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

/* ManalinkEh 2022: 0x21c5530; original cards/portal_1_2_3k.c:4650. */
int card_wu_spy(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;
  test_definition_t test;
  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event == EVENT_ETB)
  {
    default_target_definition(player, card, &td, 2);
    td.zone = 0x1000;
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    result = can_target(&td);
    if (result != 0)
    {
      result = pick_target(&td, "TARGET_PLAYER");
      if (result != 0)
      {
        default_test_definition(&test, 0xff);
        reveal_top_cards_of_library_and_choose(player, card, instance->targets[0].player, 2, 1,
                                               TUTOR_GRAVE, 0, TUTOR_DECK, 0, &test);
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c56e0; original cards/portal_1_2_3k.c:4691. */
int card_cunning_advisor(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 0);
    td.zone = 0x1000;
    instance = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) &&
        (result = would_validate_arbitrary_target(&td, instance->targets[0].player, -1),
         result != 0))
    {
      discard(instance->targets[0].player, 0, player);
    }
  }
  result =
      generic_activated_ability(player, card, event, 0x10410001, 0, 0, 0, 0, 0, 0, 0, &td, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c5840; original cards/portal_1_2_3k.c:4717. */
int card_deception(int player, int card, event_t event)
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
        default_target_definition(player, card, &td, 0);
        td.zone = 0x1000;
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            new_multidiscard(1 - player, 2, 0, player);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c589c;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
LAB_021c589c:
  result = generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c5990; original cards/portal_1_2_3k.c:4741. */
int card_desperate_charge(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    pump_subtype_until_eot_2022(player, card, player, -1, 2, 0, 0, 0);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c5a10; original cards/portal_1_2_3k.c:4754. */
int card_stolen_grain(int player, int card, event_t event)
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
        opponent_or_pw_target_definition(player, card, &td);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            damage_target0(player, card, 5);
            gain_life_2022(player, 5);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c5a59;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  opponent_or_pw_target_definition(player, card, &td);
LAB_021c5a59:
  result = generic_spell(player, card, event, 1, &td, "TARGET_OPPONENT_OR_PLANESWALKER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c5cd0; original cards/portal_1_2_3k.c:4805. */
int card_burning_fields(int player, int card, event_t event)
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
        opponent_or_pw_target_definition(player, card, &td);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            damage_target0(player, card, 5);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c5d15;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  opponent_or_pw_target_definition(player, card, &td);
LAB_021c5d15:
  result = generic_spell(player, card, event, 1, &td, "TARGET_OPPONENT_OR_PLANESWALKER", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c5de0; original cards/portal_1_2_3k.c:4828. */
int card_desert_sandstorm(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    new_damage_all(player, card, 2, 1, 1, NULL);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c6130; original cards/portal_1_2_3k.c:4912. */
int card_spring_of_eternal_peace(int player, int card, event_t event)
{
  int result;
  if (event == EVENT_RESOLVE_SPELL)
  {
    gain_life_2022(player, 8);
    kill_card(player, card, KILL_DESTROY);
  }
  result = generic_spell(player, card, event, 0, NULL, NULL, 0, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21bfd40; original cards/portal_1_2_3k.c:2749. */
int card_dakmor_lancer(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;
  const char *prompt;
  target_definition_t td1;

  etb_ability_2022(player, card, event, RESOLVE_TRIGGER_MANDATORY);
  if (event != EVENT_ETB)
  {
    return 0;
  }
  default_target_definition(player, card, &td1, 2);
  td1.illegal_color = get_sleighted_color_test(player, card, 2);
  td1.allow_cancel = 0;
  instance = get_card_instance(player, card);
  result = can_target(&td1);
  if (result != 0)
  {
    prompt = get_sleighted_color_text(player, card, "Select target non%s creature.", 1);
    result = new_pick_target(&td1, prompt, 0, 0x80);
    if (result != 0)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
  }
  instance->number_of_targets = '\0';
  return 0;
}

/* ManalinkEh 2022: 0x21c0b50; original cards/portal_1_2_3k.c:3112. */
int card_soul_shred(int player, int card, event_t event)
{
  const char *prompt;
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
        td.illegal_color = get_sleighted_color_test(player, card, 2);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            damage_target0(player, card, 3);
            gain_life_2022(player, 3);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c0bc2;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.illegal_color = get_sleighted_color_test(player, card, 2);
LAB_021c0bc2:
  prompt = get_sleighted_color_text(player, card, "Select target non%s creature.", 1);
  result = generic_spell(player, card, event, 0x81, &td, prompt, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c3070; original cards/portal_1_2_3k.c:3880. */
int card_dark_offering(int player, int card, event_t event)
{
  card_instance_t *instance;
  const char *prompt;
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
        td.illegal_color = get_sleighted_color_test(player, card, 2);
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);
          if (result != 0)
          {
            kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
            gain_life_2022(player, 3);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c30f1;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.illegal_color = get_sleighted_color_test(player, card, 2);
  get_card_instance(player, card);
LAB_021c30f1:
  prompt = get_sleighted_color_text(player, card, "Select target non%s creature.", 1);
  result = generic_spell(player, card, event, 0x81, &td, prompt, 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c3b80; original cards/portal_1_2_3k.c:4096. */
int card_harmony_of_nature(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int *piVar4;
  target_definition_t td1;
  int tapped[100];

  instance = get_card_instance(player, card);
  if (event == EVENT_RESOLVE_SPELL)
  {
    default_target_definition(player, card, &td1, 2);
    td1.allowed_controller = player;
    td1.preferred_controller = player;
    td1.illegal_abilities = 0;
    td1.illegal_state = 1;
    td1.allow_cancel = 2;
    iVar2 = 0;
    while (true)
    {
      iVar3 = can_target(&td1);
      if (iVar3 == 0)
        break;
      iVar3 =
          new_pick_target(&td1, "Select a creature you control to tap to gain 4 life.", 0, 0x80);
      if (iVar3 == 0)
        break;
      iVar3 = instance->targets[0].card;
      instance->number_of_targets = '\0';
      tapped[iVar2] = iVar3;
      iVar2 = iVar2 + 1;
      state_untargettable(player, iVar3, 1);
    }
    if (0 < iVar2)
    {
      piVar4 = tapped;
      do
      {
        iVar3 = *piVar4;
        piVar4 = piVar4 + 1;
        tap_card(player, iVar3);
        state_untargettable(player, iVar3, 0);
      } while (piVar4 != tapped + iVar2);
    }
    gain_life_2022(player, iVar2 << 2);
    kill_card(player, card, KILL_DESTROY);
  }
  iVar2 = basic_spell(player, card, event);
  return iVar2;
}

/* ManalinkEh 2022: 0x21c3e20; original cards/portal_1_2_3k.c:4164. */
int card_renewing_touch(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iStack_f0;
  test_definition_t test;

  if (event == EVENT_CAN_CAST)
  {
    get_card_instance(player, card);
    new_default_test_definition(
        &test, 2, "Select any number of target creature cards to shuffle into your library.");
    iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 8, NULL, NULL, 1, &test);
    return iVar2;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      instance = get_card_instance(player, card);
      new_default_test_definition(
          &test, 2, "Select any number of target creature cards to shuffle into your library.");
      if ((card == affected_card) && (player == affected_card_controller))
      {
        iVar2 = count_graveyard_by_type(player, 2);
        if (0x12 < iVar2)
        {
          iVar2 = 0x12;
        }
        iVar2 = select_multiple_cards_from_graveyard(player, player, 0, 1, &test, iVar2,
                                                     instance->targets);
        instance->info_slot = iVar2;
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    instance = get_card_instance(player, card);
    new_default_test_definition(
        &test, 2, "Select any number of target creature cards to shuffle into your library.");
    if (event == EVENT_RESOLVE_SPELL)
    {
      iVar2 = 0;
      iStack_f0 = 0;
      if (0 < instance->info_slot)
      {
        do
        {
          iVar3 = iVar2 + 1;
          iVar2 = validate_target_from_grave(player, card, player, iVar2);
          if (iVar2 != -1)
          {
            from_graveyard_to_deck(player, iVar2, 1);
            iStack_f0 = iStack_f0 + 1;
          }
          iVar2 = iVar3;
        } while (iVar3 < instance->info_slot);
        if ((iStack_f0 == 0) && (0 < iVar3))
        {
          spell_fizzled = 1;
        }
      }
      shuffle(player);
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    get_card_instance(player, card);
    new_default_test_definition(
        &test, 2, "Select any number of target creature cards to shuffle into your library.");
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c5b40; original cards/portal_1_2_3k.c:4778. */
int card_stone_catapult(int player, int card, event_t event)
{
  card_instance_t *instance;
  const char *prompt;
  int result;
  target_definition_t td;

  if (event == EVENT_ACTIVATE)
  {
    default_target_definition(player, card, &td, 2);
    td.required_state = 1;
    td.illegal_color = get_sleighted_color_test(player, card, 2);
    get_card_instance(player, card);
  }
  else
  {
    if ((1 < event - EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
    {
      return 0;
    }
    default_target_definition(player, card, &td, 2);
    td.required_state = 1;
    td.illegal_color = get_sleighted_color_test(player, card, 2);
    instance = get_card_instance(player, card);
    if ((event == EVENT_RESOLVE_ACTIVATION) && (result = valid_target(&td), result != 0))
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
  }
  prompt = get_sleighted_color_text(player, card, "Select target tapped non%s creature.", 1);
  result =
      generic_activated_ability(player, card, event, 0x10010007, 0, 0, 0, 0, 0, 0, 0, &td, prompt);
  return result;
}

/* ManalinkEh 2022: 0x21c5f90; original cards/portal_1_2_3k.c:4882. */
int card_spoils_of_victory(int player, int card, event_t event)
{
  int result;
  const char *msg;
  test_definition_t this_test;

  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = get_total_available_lands(player);
      if (1 < result - 4U)
      {
        ai_modifier = ai_modifier + 0x30;
      }
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    msg = get_hacked_land_text(player, card, "Select %a, %s, %s, %s, or %s card.", 0x5007, 0x5002,
                               0x5009, 0x5006, 0x5001);
    new_default_test_definition(&this_test, 1, msg);
    this_test.subtype = get_hacked_subtype(player, card, 0x5009);
    this_test.sub2 = get_hacked_subtype(player, card, 0x5002);
    this_test.sub3 = get_hacked_subtype(player, card, 0x5006);
    this_test.sub4 = get_hacked_subtype(player, card, 0x5007);
    this_test.sub5 = get_hacked_subtype(player, card, 0x5001);
    this_test.subtype_flag = 3;
    new_global_tutor(player, player, 1, 5, 0, 1, &this_test);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21b8aa0; original cards/portal_1_2_3k.c:4204. */
static void destroy_creature_when_blocked(int player, int card, int t_player, int t_card)
{
  kill_card(t_player, t_card, KILL_DESTROY);
  return;
}

/* ManalinkEh 2022: 0x21c54f0; original cards/portal_1_2_3k.c:4639. */
int card_wu_scout(int player, int card, event_t event)
{
  int result;

  horsemanship2(player, card, event);
  result = card_talas_explorer(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
static char *without_horsemanship(int who_chooses, int player, int card);
static char *with_horsemanship(int who_chooses, int player, int card);
int dread_charge_effect(int player, int card, event_t event);
int card_taoist_mystic(int player, int card, event_t event);

/* ManalinkEh 2022: 0x21bfe50; original cards/portal_1_2_3k.c:2774. */
int card_shrieking_specter(int player, int card, event_t event)
{
  dat_ability(player, card, event, DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event != EVENT_DAT_ABILITY)
  {
    return 0;
  }
  discard(1 - current_turn, 0, player);
  return 0;
}

/* ManalinkEh 2022: 0x21c0670; original cards/portal_1_2_3k.c:2982. */
int card_assassins_blade(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  const char *pcVar4;
  int iVar5;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  td.required_state = 2;
  td.illegal_color = get_sleighted_color_test(player, card, 2);
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
  {
    if ((current_turn != player) && (current_phase == 0x16))
    {
      pcVar4 = get_sleighted_color_text(player, card, "Select target non%s attacking creature.", 1);
      iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 0x81, &td, pcVar4, 1, NULL);
      iVar5 = current_turn;

      if ((iVar2 != 0) && (0 < active_cards_count[current_turn]))
      {
        iVar2 = 0;
        do
        {
          instance = in_play_and_attacking(iVar5, iVar2);
          if ((instance != NULL) && (iVar3 = check_special_flags_2022(iVar5, iVar2, 0x40), iVar3 == 0))
          {
            return 1;
          }
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[iVar5]);
      }
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      pcVar4 = get_sleighted_color_text(player, card, "Select target non%s attacking creature.", 1);
      iVar5 = generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td, pcVar4, 1, NULL);
      return iVar5;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    iVar5 = valid_target(&td);
    if (iVar5 != 0)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21b8cb0; original cards/portal_1_2_3k.c:3023. */
int dread_charge_effect(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  unsigned int uVar3;
  unsigned int uVar4;

  if (event == EVENT_BLOCK_LEGALITY)
  {
    if (attacking_card_controller == player)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        instance = in_play(player, card);
        if (instance != NULL)
        {
          uVar3 = get_color(attacking_card_controller, attacking_card);
          uVar4 = get_sleighted_color_test(player, card, 2);
          if ((uVar3 & uVar4) != 0)
          {
            uVar3 = get_color(affected_card_controller, affected_card);
            uVar4 = get_sleighted_color_test(player, card, 2);
            if ((uVar3 & uVar4) == 0)
            {
              event_result = 1;
            }
          }
        }
      }
    }
  }
  else if (event == EVENT_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c0880; original cards/portal_1_2_3k.c:3038. */
int card_dread_charge(int player, int card, event_t event)
{
  int result;

  if (event == EVENT_RESOLVE_SPELL)
  {
    create_legacy_effect(player, card, dread_charge_effect);
    kill_card(player, card, KILL_DESTROY);
  }
  result = basic_spell(player, card, event);
  return result;
}

/* ManalinkEh 2022: 0x21c0ec0; original cards/portal_1_2_3k.c:3180. */
int card_baleful_stare(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  subtype_t subtype;
  int iVar3;
  unsigned int uVar4;
  int orig_color_test;
  unsigned int uVar5;
  int card_00;
  int iStack_68;
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
        td.zone = 0x1000;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          iVar2 = valid_target(&td);
          if (iVar2 != 0)
          {

            reveal_target_player_hand(instance->targets[0].player);
            iVar2 = instance->targets[0].player;
            subtype = get_hacked_subtype(player, card, 0x5006);
            if (active_cards_count[iVar2] < 1)
            {
              iStack_68 = 0;
            }
            else
            {
              iStack_68 = 0;
              card_00 = 0;
              do
              {

                instance = in_hand(iVar2, card_00);
                if (instance != NULL)
                {
                  iVar3 = has_subtype(iVar2, card_00, subtype);
                  uVar4 = get_color(iVar2, card_00);
                  orig_color_test = get_sleighted_color_test(player, card, 0x10);
                  uVar5 = get_sleighted_color_test(player, card, orig_color_test);
                  iStack_68 = ((iStack_68 - (unsigned int)(iVar3 == 0)) + 2) -
                              (unsigned int)((uVar4 & uVar5) == 0);
                }
                card_00 = card_00 + 1;
              } while (card_00 < active_cards_count[iVar2]);
            }
            draw_cards(player, iStack_68);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c0f2d;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.zone = 0x1000;
  get_card_instance(player, card);
LAB_021c0f2d:
  iVar2 = generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x21c16d0; original cards/portal_1_2_3k.c:3369. */
int card_withering_gaze(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  subtype_t subtype;
  int iVar3;
  unsigned int uVar4;
  int orig_color_test;
  unsigned int uVar5;
  int card_00;
  int iStack_68;
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
        td.zone = 0x1000;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          iVar2 = valid_target(&td);
          if (iVar2 != 0)
          {

            reveal_target_player_hand(instance->targets[0].player);
            iVar2 = instance->targets[0].player;
            subtype = get_hacked_subtype(player, card, 0x5001);
            if (active_cards_count[iVar2] < 1)
            {
              iStack_68 = 0;
            }
            else
            {
              iStack_68 = 0;
              card_00 = 0;
              do
              {

                instance = in_hand(iVar2, card_00);
                if (instance != NULL)
                {
                  iVar3 = has_subtype(iVar2, card_00, subtype);
                  uVar4 = get_color(iVar2, card_00);
                  orig_color_test = get_sleighted_color_test(player, card, 8);
                  uVar5 = get_sleighted_color_test(player, card, orig_color_test);
                  iStack_68 = ((iStack_68 - (unsigned int)(iVar3 == 0)) + 2) -
                              (unsigned int)((uVar4 & uVar5) == 0);
                }
                card_00 = card_00 + 1;
              } while (card_00 < active_cards_count[iVar2]);
            }
            draw_cards(player, iStack_68);
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c173d;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  get_card_instance(player, card);
LAB_021c173d:
  iVar2 = generic_spell(player, card, event, 0x40, &td, NULL, 1, NULL);
  return iVar2;
}

/* ManalinkEh 2022: 0x21c19e0; original cards/portal_1_2_3k.c:3429. */
int card_treetop_defense(int player, int card, event_t event)
{
  int player_00;
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  if (event == EVENT_CAN_CAST)
  {

    if ((((current_turn != player) && (current_phase == 0x16)) &&
         (iVar1 = basic_spell(player, card, EVENT_CAN_CAST), player_00 = current_turn,
          iVar1 != 0)) &&
        (0 < active_cards_count[current_turn]))
    {
      iVar1 = 0;
      do
      {
        instance = in_play_and_attacking(player_00, iVar1);
        if ((instance != NULL) && (iVar3 = check_special_flags_2022(player_00, iVar1, 0x40), iVar3 == 0))
        {
          return 1;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player_00]);
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    pump_subtype_until_eot(player, card, player, -1, 0, 0, 0x400, 0);
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c1ed0; original cards/portal_1_2_3k.c:3524. */
int card_scorching_winds(int player, int card, event_t event)
{
  int player_00;
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  test_definition_t this_test;

  if (event == EVENT_CAN_CAST)
  {

    if ((((current_turn != player) && (current_phase == 0x16)) &&
         (iVar1 = basic_spell(player, card, EVENT_CAN_CAST), player_00 = current_turn,
          iVar1 != 0)) &&
        (0 < active_cards_count[current_turn]))
    {
      iVar1 = 0;
      do
      {
        instance = in_play_and_attacking(player_00, iVar1);
        if ((instance != NULL) && (iVar3 = check_special_flags_2022(player_00, iVar1, 0x40), iVar3 == 0))
        {
          return 1;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player_00]);
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 2);
    this_test.state = 4;
    new_damage_all(player, card, 2, 1, 0, &this_test);
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c2000; original cards/portal_1_2_3k.c:3553. */
int card_defiant_stand(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  target_definition_t td;

  if (event == EVENT_CHECK_PUMP)
  {
    if (current_turn != player)
    {
      iVar4 = vanilla_instant_pump(player, card, EVENT_CHECK_PUMP, 2, player, 1, 3, 0, 0);
      return iVar4;
    }
    default_target_definition(player, card, &td, 2);
    get_card_instance(player, card);
  }
  else
  {
    default_target_definition(player, card, &td, 2);
    instance = get_card_instance(player, card);
    if (event == EVENT_CAN_CAST)
    {

      if ((((current_turn != player) && (current_phase == 0x16)) &&
           (iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 0x81, &td, NULL, 1, NULL),
            iVar4 = current_turn, iVar2 != 0)) &&
          (0 < active_cards_count[current_turn]))
      {
        iVar2 = 0;
        do
        {
          instance = in_play_and_attacking(iVar4, iVar2);
          if ((instance != NULL) && (iVar3 = check_special_flags_2022(iVar4, iVar2, 0x40), iVar3 == 0))
          {
            return 1;
          }
          iVar2 = iVar2 + 1;
        } while (iVar2 < active_cards_count[iVar4]);
      }
    }
    else if (event == EVENT_CAST_SPELL)
    {
      if ((card == affected_card) && (player == affected_card_controller))
      {
        iVar4 = generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                              "Select target creature it gets +1/+3 until end of turn. Untap it.",
                              1, NULL);
        return iVar4;
      }
    }
    else if (event == EVENT_RESOLVE_SPELL)
    {
      iVar4 = valid_target(&td);
      if (iVar4 != 0)
      {
        pump_until_eot(player, card, instance->targets[0].player, instance->targets[0].card, 1, 3);
        untap_card(instance->targets[0].player, instance->targets[0].card);
      }
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c2400; original cards/portal_1_2_3k.c:3629. */
int card_fleet_footed_monk(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_BLOCK_LEGALITY) && (attacking_card_controller == player)) &&
      (attacking_card == card))
  {
    result = get_power(affected_card_controller, affected_card);
    if (1 < result)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        event_result = 1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c40a0; original cards/portal_1_2_3k.c:4208. */
int card_sylvan_basilisk(int player, int card, event_t event)
{
  card_instance_t *instance;
  int result;

  instance = get_card_instance(player, card);
  if (event == EVENT_CHANGE_TYPE)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      result = is_humiliated_for_ability(player, card, AL_TRIG);
      if ((result == 0) && (current_turn == player))
      {
        instance->destroys_if_blocked = instance->destroys_if_blocked | 0xc;
      }
    }
  }
  else if (event == EVENT_DECLARE_BLOCKERS)
  {
    result = is_humiliated_for_ability(player, card, AL_TRIG);
    if (((result == 0) && (current_turn == player)) && ((instance->state & 4) != 0))
    {
      for_each_creature_blocking_me(player, card, destroy_creature_when_blocked, player, card);
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c4540; original cards/portal_1_2_3k.c:4316. */
int card_just_fate(int player, int card, event_t event)
{
  card_instance_t *instance;
  int iVar2;
  int iVar3;
  int iVar4;
  target_definition_t td;

  default_target_definition(player, card, &td, 2);
  td.required_state = 2;
  instance = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
  {

    if ((((current_turn != player) && (current_phase == 0x16)) &&
         (iVar2 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, NULL, 1, NULL),
          iVar4 = current_turn, iVar2 != 0)) &&
        (0 < active_cards_count[current_turn]))
    {
      iVar2 = 0;
      do
      {
        instance = in_play_and_attacking(iVar4, iVar2);
        if ((instance != NULL) && (iVar3 = check_special_flags_2022(iVar4, iVar2, 0x40), iVar3 == 0))
        {
          return 1;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < active_cards_count[iVar4]);
    }
  }
  else if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      iVar4 = generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                            "Select target attacking creature.", 1, NULL);
      return iVar4;
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    iVar4 = valid_target(&td);
    if (iVar4 != 0)
    {
      kill_card(instance->targets[0].player, instance->targets[0].card, KILL_DESTROY);
    }
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c4700; original cards/portal_1_2_3k.c:4359. */
int card_rally_the_troops(int player, int card, event_t event)
{
  int player_00;
  int iVar1;
  card_instance_t *instance;
  int iVar3;
  test_definition_t this_test;

  if (event == EVENT_CAN_CAST)
  {

    if ((((current_turn != player) && (current_phase == 0x16)) &&
         (iVar1 = basic_spell(player, card, EVENT_CAN_CAST), player_00 = current_turn,
          iVar1 != 0)) &&
        (0 < active_cards_count[current_turn]))
    {
      iVar1 = 0;
      do
      {
        instance = in_play_and_attacking(player_00, iVar1);
        if ((instance != NULL) && (iVar3 = check_special_flags_2022(player_00, iVar1, 0x40), iVar3 == 0))
        {
          return 1;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player_00]);
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    default_test_definition(&this_test, 2);
    new_manipulate_all(player, card, player, &this_test, ACT_UNTAP);
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c49a0; original cards/portal_1_2_3k.c:4426. */
int card_warriors_stand(int player, int card, event_t event)
{
  int player_00;
  int iVar1;
  card_instance_t *instance;
  int iVar3;

  if (event == EVENT_CAN_CAST)
  {

    if ((((current_turn != player) && (current_phase == 0x16)) &&
         (iVar1 = basic_spell(player, card, EVENT_CAN_CAST), player_00 = current_turn,
          iVar1 != 0)) &&
        (0 < active_cards_count[current_turn]))
    {
      iVar1 = 0;
      do
      {
        instance = in_play_and_attacking(player_00, iVar1);
        if ((instance != NULL) && (iVar3 = check_special_flags_2022(player_00, iVar1, 0x40), iVar3 == 0))
        {
          return 1;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < active_cards_count[player_00]);
    }
  }
  else if (event == EVENT_RESOLVE_SPELL)
  {
    pump_creatures_until_eot(player, card, player, 1, 2, 2, 0, 0, NULL);
    kill_card(player, card, KILL_DESTROY);
    return 0;
  }
  return 0;
}

/* ManalinkEh 2022: 0x21b8e30; original cards/portal_1_2_3k.c:4525. */
static char *without_horsemanship(int who_chooses, int player, int card)
{
  card_instance_t *instance;
  int result;

  instance = in_play(player, card);
  if (((instance != NULL) && (result = is_what(player, card, 2), result != 0)) &&
      (result = check_for_special_ability(player, card, 0x40000000), result == 0))
  {
    return NULL;
  }
  return "illegal target it has horsemanship.";
}

/* ManalinkEh 2022: 0x21c4eb0; original cards/portal_1_2_3k.c:4535. */
int card_broken_dam(int player, int card, event_t event)
{
  target_t *ptVar1;
  target_t *ptVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  target_definition_t td;

  if (event == EVENT_CAN_CAST)
  {
    default_target_definition(player, card, &td, 2);
    td.preferred_controller = player;
    td.extra = (int32_t)without_horsemanship;
    td.special = 0x8000;
    get_card_instance(player, card);
    iVar5 = generic_spell(player, card, EVENT_CAN_CAST, 1, &td, "TARGET_CREATURE", 1, NULL);
    return iVar5;
  }
  if (event == EVENT_CAST_SPELL)
  {
    if ((card == affected_card) && (player == affected_card_controller))
    {
      default_target_definition(player, card, &td, 2);
      td.extra = (int32_t)without_horsemanship;
      td.special = 0x8000;
      td.preferred_controller = player;
      instance = get_card_instance(player, card);
      if ((card == affected_card) &&
          ((player == affected_card_controller &&
            (generic_spell(player, card, EVENT_CAST_SPELL, 0x81, &td,
                           "Select target creature without horsemanship ability.", 1, NULL),
             spell_fizzled != 1))))
      {
        state_untargettable(instance->targets[0].player, instance->targets[0].card, 1);
        iVar5 = can_target(&td);
        if (iVar5 != 0)
        {
          td.allow_cancel = 2;
          new_pick_target(&td, "Select target creature without horsemanship ability.", 1, 0x80);
        }
        state_untargettable(instance->targets[0].player, instance->targets[0].card, 0);
      }
    }
  }
  else if ((event == EVENT_RESOLVE_SPELL) || (event == EVENT_CAN_CHANGE_TARGET))
  {
    default_target_definition(player, card, &td, 2);
    td.extra = (int32_t)without_horsemanship;
    td.special = 0x8000;
    td.preferred_controller = player;
    instance = get_card_instance(player, card);
    if (event == EVENT_RESOLVE_SPELL)
    {

      iVar5 = 0;
      if (instance->number_of_targets != '\0')
      {
        do
        {
          while (iVar4 = validate_target(player, card, &td, iVar5), iVar4 != 0)
          {
            ptVar2 = instance->targets + iVar5;
            ptVar1 = instance->targets + iVar5;
            iVar5 = iVar5 + 1;
            tap_card(ptVar1->player, ptVar2->card);
            if ((int)(unsigned int)instance->number_of_targets <= iVar5)
              goto LAB_021c4f7e;
          }
          iVar5 = iVar5 + 1;
        } while (iVar5 < (int)(unsigned int)instance->number_of_targets);
      }
    LAB_021c4f7e:
      kill_card(player, card, KILL_DESTROY);
      return 0;
    }
  }
  else if (event == EVENT_CHANGE_TARGET)
  {
    default_target_definition(player, card, &td, 2);
    td.extra = (int32_t)without_horsemanship;
    td.special = 0x8000;
    td.preferred_controller = player;
    get_card_instance(player, card);
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c5460; original cards/portal_1_2_3k.c:4622. */
int card_wu_admiral(int player, int card, event_t event)
{
  int result;

  result = is_humiliated_for_ability(player, card, AL_STATIC);
  if (((result == 0) && (card == affected_card)) && (player == affected_card_controller))
  {
    result = get_hacked_subtype(player, card, 0x5002);
    result = check_battlefield_for_subtype(1 - player, 1, result);
    if ((result != 0) && (event - EVENT_POWER < 2))
    {
      event_result = event_result + 1;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c5640; original cards/portal_1_2_3k.c:4676. */
int card_zhou_yu_chief_commander(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  if (((event == EVENT_ATTACK_LEGALITY) && (card == affected_card)) &&
      (player == affected_card_controller))
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if (result == 0)
    {
      result = get_hacked_subtype(player, card, 0x5002);
      result = check_battlefield_for_subtype(1 - player, 1, result);
      if (result != 0)
      {
        event_result = 1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c5ef0; original cards/portal_1_2_3k.c:4859. */
int card_yuan_shaos_infantry(int player, int card, event_t event)
{
  int card_00;
  card_instance_t *instance;

  dat_ability(player, card, event, DAT_ATTACKS_ALONE | DAT_RESOLVE_TRIGGER_MANDATORY, 0);
  if (event == EVENT_DAT_ABILITY)
  {

    card_00 = pump_ability_until_eot_2022(player, card, player, card, 0, 0, 0, 0x10);
    if (card_00 != -1)
    {

      instance = get_card_instance(player, card_00);
      instance->targets[3].player = 0x40;
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21c6190; original cards/portal_1_2_3k.c:4925. */
int card_taoist_mystic(int player, int card, event_t event)
{
  int result;

  if (((event == EVENT_BLOCK_LEGALITY) && (attacking_card_controller == player)) &&
      (attacking_card == card))
  {
    result = check_for_special_ability(affected_card_controller, affected_card, 0x40000000);
    if (result != 0)
    {
      result = is_humiliated_for_ability(player, card, AL_STATIC);
      if (result == 0)
      {
        event_result = 1;
      }
    }
  }
  return 0;
}

/* ManalinkEh 2022: 0x21b8ea0; original cards/portal_1_2_3k.c:4939. */
static char *with_horsemanship(int who_chooses, int player, int card)
{
  card_instance_t *instance;
  int result;

  instance = in_play(player, card);
  if (((instance != NULL) && (result = is_what(player, card, 2), result != 0)) &&
      (result = check_for_special_ability(player, card, 0x40000000), result != 0))
  {
    return NULL;
  }
  return "illegal target it hasn\'t got horsemanship.";
}

/* ManalinkEh 2022: 0x21c6210; original cards/portal_1_2_3k.c:4949. */
int card_trip_wire(int player, int card, event_t event)
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
        td.preferred_controller = 1 - player;
        td.extra = (int32_t)with_horsemanship;
        td.special = 0x8000;
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
        goto LAB_021c6292;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 2);
  td.preferred_controller = 1 - player;
  td.extra = (int32_t)with_horsemanship;
  td.special = 0x8000;
  get_card_instance(player, card);
LAB_021c6292:
  result = generic_spell(player, card, event, 0x81, &td,
                         "Select target creature with horsemanship.", 1, NULL);
  return result;
}

/* ManalinkEh 2022: 0x21c63a0; original cards/portal_1_2_3k.c:4977. */
int card_zuo_ci_the_mocking_sage(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  hexproof_2022(player, card, event);
  result = card_taoist_mystic(player, card, event);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21c3340; original cards/portal_1_2_3k.c:3935. */
int card_eye_spy(int player, int card, event_t event)
{
  int *deck;
  card_instance_t *instance;
  int result;
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
        td.zone = 0x1000;
        instance = get_card_instance(player, card);
        if (event == EVENT_RESOLVE_SPELL)
        {
          result = valid_target(&td);

          if ((result != 0) && (deck = deck_ptr[instance->targets[0].player], *deck != -1))
          {

            show_deck(player, deck, 1, "Here\'s the first card of target player\'s deck", 0,
                      ENGINE_DONE_LABEL);
            if ((ai_is_speculating == 1) ||
                ((argument7 = 1,
                  player == 1 && (argument7 = 1, ((unsigned char)trace_mode & 2) == 0))))
            {
              argument7 = (unsigned int)(cards_ptr[cards_data[*deck].id]->ai_base_value < 0x4c);
            }
            result = do_dialog(player, player, card, -1, -1, " Mill top card\n Pass", argument7);
            if (result == 0)
            {
              mill(instance->targets[0].player, 1);
            }
          }
          kill_card(player, card, KILL_DESTROY);
        }
        goto LAB_021c33ad;
      }
      if (event != EVENT_CHANGE_TARGET)
      {
        return 0;
      }
    }
  }
  default_target_definition(player, card, &td, 0);
  td.zone = 0x1000;
  get_card_instance(player, card);
LAB_021c33ad:
  result = generic_spell(player, card, event, 1, &td, "TARGET_PLAYER", 1, NULL);
  return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x21c5e60; original cards/portal_1_2_3k.c:4841. */
int card_yuan_shao_the_indecicive(int player, int card, event_t event)
{
  int result;

  check_legend_rule(player, card, event);
  horsemanship2(player, card, event);
  if (event == EVENT_ABILITIES)
  {
    result = is_humiliated_for_ability(player, card, AL_STATIC);
    if ((result == 0) && (affected_card_controller == player))
    {
      maxblockers_1(player, affected_card, EVENT_ABILITIES);
    }
  }
  return 0;
}


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x21b8dc0; original cards/portal_1_2_3k.c:621. */
int last_chance_legacy_2022(int player, int card, event_t event)
{
  int result;
  card_instance_t *instance;
  if (current_turn == player)
  {
    result = eot_trigger(player, card, event);
    if (result != 0)
    {
      instance = get_card_instance(player, card);
      instance->targets[1].player = instance->targets[1].player + -1;
      instance = get_card_instance(player, card);
      if (instance->targets[1].player < 1)
      {
        lose_the_game_2022(player);
      }
    }
  }
  return 0;
}

#endif
